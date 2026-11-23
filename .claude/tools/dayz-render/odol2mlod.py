# -*- coding: utf-8 -*-
r"""ODOL (бинарная модель DayZ) -> MLOD (редактируемый p3d) + восстановленный model.cfg.

  python odol2mlod.py <in.p3d> <out.p3d> [--cfg=<model.cfg>] [--mass=1800]

Что переносится (проверено на ODOL v53 Interceptor_byALEX и своём v56, 2026-11-17):
  * все LOD: точки (каждая вершина ODOL = точка MLOD), нормали, UV, грани (порядок вершин РАЗВЁРНУТ:
    в ODOL он обратный MLOD — калибровка на паре echorunner MLOD/ODOL, 2973 из 2973 граней),
    текстура и материал грани (по секциям), именованные выделения (грани + вершины), свойства LOD;
  * прокси: треугольник из матрицы ODOL, имя «proxy:<путь>.<номер>», грань добавлена в выделение кости;
  * Geometry LOD: массы точек не хранятся в LOD — раскладываем --mass поровну по точкам;
  * model.cfg: скелет (кости и родители), анимации (тип, источник, пределы, адрес, углы/смещения), кость
    анимации и ось — по таблицам после классов; ось сопоставляется с выделением памяти (2 точки) по положению
    и направлению; sections[] = «секционные» выделения LOD0.
Не переносится: флаги граней/точек (тени, освещение), второй набор UV, распределение массы.
"""
import math
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(os.path.dirname(HERE), "py3d"))
import odol_read as O  # noqa: E402
import py3d  # noqa: E402

ANIM_TYPES = {0: "rotation", 1: "rotationX", 2: "rotationY", 3: "rotationZ", 4: "translation",
              5: "translationX", 6: "translationY", 7: "translationZ", 8: "direct", 9: "hide"}
ADDR = {0: "clamp", 1: "loop", 2: "mirror"}


# ------------------------------------------------------------------ заголовок: скелет и анимации
def _asciiz(d, p):
    e = d.index(b"\0", p)
    return d[p:e].decode("latin1"), e + 1


def _printable(s):
    return all(32 <= ord(c) < 127 for c in s)


def parse_skeleton(d, start, limit=6000):
    """Ищет структуру: имя\\0, isDiscrete(u8 0/1), count(u32), count пар asciiz (кость, родитель)."""
    for p0 in range(start, min(len(d) - 8, start + limit)):
        try:
            name, p = _asciiz(d, p0)
        except ValueError:
            return None
        if len(name) < 2 or not _printable(name):
            continue
        if d[p] not in (0, 1):
            continue
        n = struct.unpack_from("<I", d, p + 1)[0]
        if not 1 <= n <= 2000:
            continue
        q = p + 5
        bones = []
        ok = True
        for _ in range(n):
            b, q = _asciiz(d, q)
            par, q = _asciiz(d, q)
            if not b or not _printable(b) or not _printable(par) or len(b) > 64:
                ok = False
                break
            bones.append((b, par))
        if ok:
            return {"name": name, "discrete": d[p], "bones": bones, "end": q, "start": p0}
    return None


def parse_anims(d, start, nlods, nbones, limit=4000):
    """Классы анимаций + таблицы «кость -> анимации» и «анимация -> кость/ось» по LOD."""
    for p0 in range(start, min(len(d) - 8, start + limit)):
        cnt = struct.unpack_from("<I", d, p0)[0]
        if not 1 <= cnt <= 1000:
            continue
        p = p0 + 4
        anims = []
        try:
            for _ in range(cnt):
                t = struct.unpack_from("<I", d, p)[0]
                if t > 9:
                    raise ValueError
                p += 4
                name, p = _asciiz(d, p)
                src, p = _asciiz(d, p)
                if not name or not _printable(name) or not _printable(src):
                    raise ValueError
                mn, mx, mnp, mxp = struct.unpack_from("<4f", d, p)
                p += 16
                addr = struct.unpack_from("<I", d, p)[0]
                p += 4
                if t <= 7 or t == 9:
                    extra = list(struct.unpack_from("<2f", d, p))
                    p += 8
                else:
                    extra = list(struct.unpack_from("<8f", d, p))
                    p += 32
                anims.append({"type": t, "name": name, "source": src, "min": mn, "max": mx,
                              "minPhase": mnp, "maxPhase": mxp, "addr": addr, "extra": extra})
        except (ValueError, struct.error):
            continue
        nl = struct.unpack_from("<I", d, p)[0]
        if nl != nlods:
            continue
        p += 4
        b2a = []
        for _ in range(nl):
            nb = struct.unpack_from("<I", d, p)[0]
            p += 4
            lst = []
            for _ in range(nb):
                k = struct.unpack_from("<I", d, p)[0]
                p += 4
                lst.append(list(struct.unpack_from("<%dI" % k, d, p)))
                p += 4 * k
            b2a.append(lst)
        a2b = []                                     # по LOD: [(кость, ось (pos, dir) или None)]
        for _ in range(nl):
            row = []
            for a in anims:
                bi = struct.unpack_from("<i", d, p)[0]
                p += 4
                ax = None
                if bi >= 0 and a["type"] <= 7:
                    ax = struct.unpack_from("<6f", d, p)
                    p += 24
                row.append((bi, ax))
            a2b.append(row)
        return {"anims": anims, "bone2anims": b2a, "anim2bone": a2b, "end": p}
    return None


# ------------------------------------------------------------------ LOD -> py3d
def lod_to_mlod(od, bones, mass_total=None, weights=None):
    """weights — веса костей вершин (odol_read.read_bone_weights) для одежды и персонажей: выделения костей
    пишутся с весами вершин (MLOD хранит вес в выделении), а не «точка целиком в кости»."""
    lod = py3d.LOD()
    lod.resolution = float(od["res"])
    pts = od["points"]
    for c in pts:
        p = py3d.Point()
        p.coords = (float(c[0]), float(c[1]), float(c[2]))
        p.flags = 0
        p.mass = None
        lod.points.append(p)
    if mass_total and pts:
        per = mass_total / len(pts)
        for p in lod.points:
            p.mass = per
    nrm = od["normals"]
    for i in range(len(pts)):
        n = nrm[i] if i < len(nrm) else (0.0, 1.0, 0.0)
        lod.facenormals.append((float(n[0]), float(n[1]), float(n[2])))
    uv = od["uv"]
    # Треугольники прокси ODOL (грани выделений «proxy:*») НЕ переносим: прокси ниже создаются заново
    # (add_proxy). Иначе старый треугольник остаётся обычной гранью без текстуры в выделении места крепления
    # (у ALEX — до 2 м: серые клинья и тонкая линия в салоне; найдено 2026-11-17 на Interceptor, 21 грань/LOD).
    proxy_faces = set()
    for s in od["selections"]:
        if s["name"].startswith("proxy:"):
            proxy_faces.update(s["face_set"])

    def keep_face(fi, f):
        return len(set(f)) == len(f) >= 3 and fi not in proxy_faces

    for fi, f in enumerate(od["faces"]):
        if not keep_face(fi, f):
            continue
        fa = py3d.Face(lod.points, lod.facenormals)
        fa.flags = 0
        fa.texture = od["face_tex"][fi] or ""
        fa.material = od["face_mat"][fi] or ""
        for vi in reversed(f):                       # ODOL -> MLOD: обратный порядок вершин
            vx = py3d.Vertex(lod.points, lod.facenormals)
            vx.point_index = vi
            vx.normal_index = vi
            vx.uv = (float(uv[vi][0]), float(uv[vi][1])) if vi < len(uv) else (0.0, 0.0)
            fa.vertices.append(vx)
        lod.faces.append(fa)
    nf_kept = len(lod.faces)
    face_remap = {}
    k = 0
    for fi, f in enumerate(od["faces"]):
        if keep_face(fi, f):
            face_remap[fi] = k
            k += 1
    sel_faces = {}
    used_kept = {v for fi in face_remap for v in od["faces"][fi]}
    proxy_only = {v for fi in proxy_faces for v in od["faces"][fi]} - used_kept   # точки только старых прокси
    for s in od["selections"]:
        if s["name"].startswith("proxy:"):
            continue
        fl = sorted(face_remap[fi] for fi in s["face_set"] if fi in face_remap)
        pl = sorted((set(s.get("vertices", [])) - proxy_only) | {v for fi in s["face_set"] if fi in face_remap
                                                                 for v in od["faces"][fi]})
        sel_faces[s["name"]] = (fl, pl)
    # веса костей: выделение кости = вершины с ненулевым весом этой кости (с весом); выделения с именем кости,
    # у которой в LOD нет весов, — без точек (иначе MLOD привяжет их к кости целиком)
    weighted = {}
    if weights:
        bone_pts = {}
        for vi, vw in enumerate(weights):
            if vi in proxy_only:
                continue
            for b, w in vw:
                if 0 <= b < len(bones):
                    bone_pts.setdefault(b, {})[vi] = w
        by_low = {name.lower(): name for name in sel_faces}
        bone_low = {b[0].lower() for b in bones}
        for b, pw in bone_pts.items():
            bname = bones[b][0]
            key = by_low.get(bname.lower(), bname)
            fl, _ = sel_faces.get(key, ([], []))
            if not fl:
                fl = [face_remap[fi] for fi in face_remap if all(v in pw for v in od["faces"][fi])]
            sel_faces[key] = (fl, sorted(pw))
            weighted[key] = pw
        for name in list(sel_faces):
            if name.lower() in bone_low and name not in weighted:
                sel_faces[name] = (sel_faces[name][0], [])
    # прокси: треугольник + выделение «proxy:...»; грань прокси — в выделение своей кости
    for px in od.get("proxies", []):
        m = px["matrix"]
        # py3d add_proxy(space="raw") кладёт строки (a, b, c) как (-a, c, b) в ODOL (сверка по кругу на 21 прокси
        # Interceptor, 2026-11-17) — передаём обратное: (-r0, r2, r1), тогда после binarize матрица как в оригинале
        r0, r1, r2 = m[0:3], m[3:6], m[6:9]
        rows = (tuple(-x for x in r0), r2, r1)
        path = px["name"]
        idx = px["ids"][0]
        try:
            lod.add_proxy(path, max(idx, 0), tuple(m[9:12]), rows, 0.001, "raw")
        except ValueError as e:
            print("   proxy skip", path, idx, e)
            continue
        bi = px["ids"][2] if len(px["ids"]) > 2 else -1
        if 0 <= bi < len(bones):
            bname = bones[bi][0]
            fl, pl = sel_faces.setdefault(bname, ([], []))
            fl.append(len(lod.faces) - 1)
            pl.extend(range(len(lod.points) - 3, len(lod.points)))
    for name, (fl, pl) in sel_faces.items():
        lod.set_selection(name, face_idx=sorted(set(fl)), point_idx=sorted(set(pl)))
        pw = weighted.get(name)
        if pw:                                       # вес точки в выделении кости (точки прокси — 1)
            sel = lod.selections[name]
            sel.points = {lod.points[i]: (1 if pw.get(i, 1.0) >= 0.999 else pw[i]) for i in sorted(set(pl))}
    for kk, vv in od["props"]:
        lod.properties[kk] = vv
    return lod, nf_kept


# ------------------------------------------------------------------ оси анимаций по точкам памяти
def match_axes(mem, anims, a2b0):
    cand = []
    for s in mem["selections"]:
        vs = sorted(set(s.get("vertices", [])))
        if len(vs) >= 2:                             # ось = первые две точки выделения
            a, b = mem["points"][vs[0]], mem["points"][vs[1]]
            cand.append((s["name"], a, b))
    res = {}
    for ai, a in enumerate(anims):
        bi, ax = a2b0[ai]
        if ax is None:
            continue
        pos, d = ax[0:3], ax[3:6]
        best = None
        for name, A, B in cand:
            v = [B[i] - A[i] for i in range(3)]
            if a["type"] >= 4:                       # перенос: вектор оси = смещение (с длиной)
                sc = min(math.dist(v, d), math.dist([-x for x in v], d))
            else:                                    # поворот: начало оси = одна из точек, направление ±
                ln = math.sqrt(sum(x * x for x in v)) or 1.0
                vn = [x / ln for x in v]
                sc = min(math.dist(pos, A) + math.dist(vn, d),
                         math.dist(pos, B) + math.dist([-x for x in vn], d))
            if "axis" not in name.lower():           # при равенстве — имя с «axis»
                sc += 0.001
            if best is None or sc < best[0]:
                best = (sc, name)
        res[ai] = best
    return res


def write_model_cfg(path, model_name, skel, an, sections, axes):
    L = []
    L.append("// Восстановлено из ODOL утилитой odol2mlod.py (%s)" % model_name)
    L.append("class CfgSkeletons\n{\n\tclass Default\n\t{\n\t\tisDiscrete = 1;\n\t\tskeletonInherit = \"\";"
             "\n\t\tskeletonBones[] = {};\n\t};")
    if skel:
        L.append("\tclass %s: Default\n\t{\n\t\tisDiscrete = %d;\n\t\tskeletonBones[] =\n\t\t{" % (skel["name"],
                                                                                              skel["discrete"]))
        L.append(",\n".join("\t\t\t\"%s\", \"%s\"" % bp for bp in skel["bones"]))
        L.append("\t\t};\n\t};")
    L.append("};\nclass CfgModels\n{\n\tclass Default\n\t{\n\t\tsectionsInherit = \"\";\n\t\tsections[] = {};"
             "\n\t\tskeletonName = \"\";\n\t};")
    L.append("\tclass %s: Default\n\t{" % model_name)
    if skel:
        L.append("\t\tskeletonName = \"%s\";" % skel["name"])
    L.append("\t\tsections[] = {%s};" % ", ".join("\"%s\"" % s for s in sections))
    if an:
        L.append("\t\tclass Animations\n\t\t{")
        bones = skel["bones"] if skel else []
        for ai, a in enumerate(an["anims"]):
            bi = an["anim2bone"][0][ai][0]
            sel = bones[bi][0] if 0 <= bi < len(bones) else ""
            t = ANIM_TYPES.get(a["type"], "rotation")
            L.append("\t\t\tclass %s\n\t\t\t{" % a["name"])
            L.append("\t\t\t\ttype = \"%s\";" % t)
            L.append("\t\t\t\tsource = \"%s\";" % a["source"])
            L.append("\t\t\t\tselection = \"%s\";" % sel)
            ax = axes.get(ai)
            if ax and a["type"] in (0, 4, 8):
                L.append("\t\t\t\taxis = \"%s\";   // совпадение %.4f" % (ax[1], ax[0]))
            if a["addr"]:
                L.append("\t\t\t\tsourceAddress = \"%s\";" % ADDR.get(a["addr"], "clamp"))
            L.append("\t\t\t\tminValue = %.7g;\n\t\t\t\tmaxValue = %.7g;" % (a["min"], a["max"]))
            e = a["extra"]
            if a["type"] <= 3:
                L.append("\t\t\t\tangle0 = %.7g;\n\t\t\t\tangle1 = %.7g;" % (e[0], e[1]))
            elif a["type"] <= 7:
                L.append("\t\t\t\toffset0 = %.7g;\n\t\t\t\toffset1 = %.7g;" % (e[0], e[1]))
            elif a["type"] == 9:
                L.append("\t\t\t\thideValue = %.7g;" % e[0])
            L.append("\t\t\t};")
        L.append("\t\t};")
    L.append("\t};\n};")
    with open(path, "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")


# ------------------------------------------------------------------ главная
def convert(src, dst, cfg=None, mass=1800.0):
    data = open(src, "rb").read()
    ver, n, res, p = O.header(data)
    skel = parse_skeleton(data, p)
    an = parse_anims(data, skel["end"], n, len(skel["bones"])) if skel else None
    bones = skel["bones"] if skel else []
    print("%s: ODOL v%d, LOD %d, кости %d, анимации %d" % (os.path.basename(src), ver, n, len(bones),
                                                         len(an["anims"]) if an else 0))
    m = py3d.P3D()
    mem = None
    lod0 = None
    for i in range(n):
        od = O.read_visual_lod(src, i)
        r = od["res"]
        is_geo = abs(r - 1e13) / 1e13 < 1e-3
        W = O.read_bone_weights(data, od) if bones else None
        lod, nf = lod_to_mlod(od, bones, mass if is_geo else None, W)
        m.lods.append(lod)
        if abs(r - 1e15) / 1e15 < 1e-3:
            mem = od
        if i == 0:
            lod0 = od
        print("  LOD %-12.4g точки %6d грани %6d выделения %3d прокси %2d веса %s" % (
            r, len(od["points"]), nf, len(od["selections"]), len(od.get("proxies", [])),
            "да" if W else "нет"))
    os.makedirs(os.path.dirname(os.path.abspath(dst)), exist_ok=True)
    m.save(dst)
    print("  -> MLOD", dst)
    if cfg:
        sections = [s["name"] for s in lod0["selections"] if s["sectional"]] if lod0 else []
        axes = match_axes(mem, an["anims"], an["anim2bone"][0]) if (mem and an) else {}
        name = os.path.splitext(os.path.basename(dst))[0]
        write_model_cfg(cfg, name, skel, an, sections, axes)
        print("  -> model.cfg", cfg, "(секций %d)" % len(sections))
    return m


if __name__ == "__main__":
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    opts = {a.split("=", 1)[0][2:]: a.split("=", 1)[1] for a in sys.argv[1:] if a.startswith("--") and "=" in a}
    convert(args[0], args[1], opts.get("cfg"), float(opts.get("mass", 1800)))
