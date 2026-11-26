# -*- coding: utf-8 -*-
r"""Сборка модели сервопривода экзо Эхо (VB_EchoClothes, этап 3б; ТЗ_VB_EchoClothes.md раздел 8).

  python ec_servo_model.py            (Python из Blender 5.3: нужен numpy)

Вход — оригинал ODOL OOTS (modding/mod_vb_Echo/исходники_моделей/ExoPrivod_m.odol.p3d, в мод не входит).
Выход — MLOD VB_EchoClothes/exo_set/Backpacks/ExoPrivod/ExoPrivod_m.p3d + model.cfg рядом; бинаризует pbo.ps1 -Binarize.

Что делается во всех видимых LOD (через odol2mlod с весами костей; веса новых точек — от ближайшей вершины оригинала):
  * ржавые банки под ячейками (сварены со спинным блоком, ниже плиты) вырезаются по объёму;
  * гнёзда ячеек: стакан с контактным кольцом (свет), бандаж, две направляющие, верхний фиксатор с защёлкой,
    кронштейн, опора с пружиной до пояса — наклонены вместе с ячейкой;
  * крепления ячеек vb_exocell_slot_1/2 наклонены «галочкой» на TILT наружу и сдвинуты назад на CBACK;
  * рифлёные шланги на бёдрах (один из двух) и на голенях заменяются гладкими светящимися трубками с хомутами;
  * плетёные кабели рук, центральная труба и перемычка между ячейками — в светящееся выделение;
  * индикатор под решёткой поясного блока.
Выделения: vbglow (свет: по состоянию сервопривода скрипт ставит неон Эхо / синтез / серый), vbparts (новые детали,
тёмная сталь). Оба — секции model.cfg и hiddenSelections конфига.
"""
import math
import os
import re
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
EDIT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(EDIT, ".claude", "tools", "dayz-render"))
sys.path.insert(0, os.path.join(EDIT, ".claude", "tools", "py3d"))
sys.path.insert(0, HERE)
import odol2mlod as M2  # noqa: E402
import odol_read as O  # noqa: E402
import py3d  # noqa: E402
import ec_geom as G  # noqa: E402

BS = chr(92)
SRC = os.path.join(HERE, "..", "исходники_моделей", "ExoPrivod_m.odol.p3d")
OUTDIR = os.path.join(HERE, "..", "VB_EchoClothes", "exo_set", "Backpacks", "ExoPrivod")
DST = os.path.join(OUTDIR, "ExoPrivod_m.p3d")
CFG = os.path.join(OUTDIR, "model.cfg")
DATA = BS.join(["vb_echoclothes", "exo_set", "backpacks", "exoprivod", "data", ""])
TEX_STEEL, MAT_STEEL = DATA + "vb_ec_steel_dark_co.paa", DATA + "vb_ec_steel_dark.rvmat"
TEX_GLOW, MAT_GLOW = DATA + "vb_ec_neon_empty_co.paa", DATA + "vb_ec_neon_off.rvmat"   # без энергии; скрипт меняет

TEX_RUBBER, MAT_RUBBER = DATA + "vb_ec_rubber_co.paa", DATA + "vb_ec_rubber.rvmat"
PIPE_SPLIT_Y = 1.195             # центральная труба: выше — световод, ниже (гнётся к поясу) — гофра (просьба пользователя)

TILT = math.radians(12)          # наклон ячеек наружу
CBACK = 0.02                     # сдвиг ячеек назад от спины, м
BELT_TOP = 1.085                 # верх поясного блока (высота DayZ) — низ опоры гнезда
# центры кусков в LOD0 (оси DayZ; номера — как в концепте, ec_concept.py --hose_debug)
GLOW_PARTS = [(0.387, 1.27, 0.136), (-0.387, 1.27, 0.136),          # 0/1 плетёные кабели рук
              (0.0, 1.223, 0.267), (0.077, 1.23, 0.215)]            # 16 центральная труба, 17 перемычка
CABLE_PARTS = [(0.217, 0.833, 0.105), (-0.217, 0.833, 0.105),       # 18/19 гофра бедра (одна из двух)
               (0.245, 0.428, 0.077), (-0.245, 0.428, 0.077)]       # 22/23 гофра голени


def in_old_can(c):
    """Центр грани (DayZ) — в объёме старой банки под ячейкой (сварена со спинным блоком)."""
    return 0.05 < abs(c[0]) < 0.135 and 1.09 < c[1] < 1.175 and 0.12 < c[2] < 0.24


# ------------------------------------------------------------------ новые детали
def seat_meshes(anchor):
    """Гнездо ячейки в системе крепления: возвращает (сталь, свет) в осях DayZ. Локально: x — вбок, y — вверх
    вдоль ячейки, z — назад от спины. Ячейка стоит основанием на anchor (начало модели eb_cell — у основания)."""
    side = 1 if anchor[0] > 0 else -1
    R = G.rot_z(-side * TILT)                        # верх ячейки — наружу
    base = np.array(anchor) + np.array([0.0, 0.0, CBACK])
    up = np.array([0.0, 1.0, 0.0])
    steel, glow = G.Mesh(), G.Mesh()
    steel.extend(G.cylinder((0, 0, 0), up, 0.044, 0.06, 24))                         # стакан
    steel.extend(G.torus((0, -0.016, 0), up, 0.045, 0.005, 24, 6))                    # бандаж
    glow.extend(G.torus((0, 0.031, 0), up, 0.040, 0.003, 24, 6))                      # контактное кольцо
    for sx in (-1, 1):
        steel.extend(G.box((sx * 0.047, 0.1, 0.0), (0.004, 0.075, 0.006)))           # направляющие
    steel.extend(G.torus((0, 0.168, 0), up, 0.043, 0.006, 24, 6))                     # верхний фиксатор
    steel.extend(G.box((0.0, 0.168, 0.046), (0.011, 0.009, 0.005)))                   # защёлка (сзади)
    steel.extend(G.box((0.0, 0.0, -0.05), (0.025, 0.025, 0.025)))                     # кронштейн к спине
    steel = steel.transformed(R, base)
    glow = glow.transformed(R, base)
    bot = R @ np.array([0.0, -0.03, 0.0]) + base                                      # низ стакана
    top_y, low_y = bot[1], BELT_TOP
    mid = np.array([bot[0], (top_y + low_y) / 2, bot[2]])
    steel.extend(G.cylinder(mid, up, 0.011, top_y - low_y + 0.01, 12))                # опора
    steel.extend(G.torus(mid, up, 0.017, 0.003, 16, 6))                               # пружина
    return G.fix_winding(steel), G.fix_winding(glow)


def cable_meshes(line, sides):
    """Гладкая светящаяся трубка по осевой линии гофры + два хомута и разъёмы на концах (сталь)."""
    glow = G.fix_winding(G.tube_along(line, 0.0065, sides, caps=True))
    steel = G.Mesh()
    n = len(line)
    for k in (n // 3, 2 * n // 3):
        t = line[min(k + 1, n - 1)] - line[max(k - 1, 0)]
        steel.extend(G.cylinder(line[k], t, 0.0095, 0.012, 12))
    for p in (line[0], line[-1]):
        steel.extend(G.ellipsoid(p, (0.012, 0.012, 0.012), None, 10, 6))
    return glow, G.fix_winding(steel)


def pipe_hose(line, sides):
    """Нижняя часть центральной трубы (от хомута вниз к поясу): гофрированный шланг по осевой линии трубы + стальной
    хомут на стыке со световодом. Осевая линия идёт по высоте монотонно (снизу у пояса вверх к изгибу у плиты)."""
    L = sorted([np.asarray(p) for p in line], key=lambda p: p[1])
    low = [p for p in L if p[1] < PIPE_SPLIT_Y]
    hi = next((p for p in L if p[1] >= PIPE_SPLIT_Y), None)
    if hi is not None and low:
        a = low[-1]
        t = (PIPE_SPLIT_Y + 0.006 - a[1]) / max(hi[1] - a[1], 1e-6)        # чуть под световод (без щели)
        low.append(a + (hi - a) * min(t, 1.0))
    path = low[::-1]                                                        # сверху вниз
    hose = G.fix_winding(G.corrugated(path, 0.021, 0.003, 0.007, sides, caps=True))
    tan = path[1] - path[0]
    collar = G.fix_winding(G.cylinder(path[0] + tan / np.linalg.norm(tan) * 0.004, tan, 0.0235, 0.012, 16))
    return hose, collar


def led_mesh(points):
    """Индикатор под решёткой поясного блока сзади (по точкам оригинала, как в концепте)."""
    P = np.asarray(points)
    m = (np.abs(P[:, 0]) < 0.05) & (P[:, 1] > 0.93) & (P[:, 1] < 1.06)
    B = P[m]
    zmax = B[:, 2].max()
    yc = float(np.median(B[B[:, 2] > zmax - 0.012][:, 1]))
    return G.fix_winding(G.box((0.0, yc - 0.035, zmax + 0.004), (0.05, 0.0035, 0.003)))


# ------------------------------------------------------------------ правка LOD
class LodEditor:
    def __init__(self, lod, od, weights, bones):
        self.lod, self.od, self.W, self.bones = lod, od, weights, bones
        self.P = np.asarray(od["points"], dtype=np.float64)
        proxy = set()
        for s in od["selections"]:
            if s["name"].startswith("proxy:"):
                proxy.update(s["face_set"])
        self.proxy_faces = proxy
        kept = [fi for fi, f in enumerate(od["faces"]) if len(set(f)) == len(f) >= 3 and fi not in proxy]
        self.mlod_face = {fi: lod.faces[j] for j, fi in enumerate(kept)}     # грань ODOL -> грань MLOD
        self.winding = self._measure_winding()
        self.sel_low = {k.lower(): k for k in lod.selections}

    def _measure_winding(self):
        """+1: в MLOD cross(b-a, c-a) смотрит по нормали; -1: наоборот (порядок вершин у odol2mlod развёрнут)."""
        s = 0
        for fa in self.lod.faces[:3000]:
            v = fa.vertices
            a, b, c = (np.array(self.lod.points[x.point_index].coords) for x in v[:3])
            n = np.array(self.lod.facenormals[v[0].normal_index])
            s += 1 if np.dot(np.cross(b - a, c - a), n) > 0 else -1
        return 1 if s >= 0 else -1

    def winding_outward(self, center, pick=None, rmin=0.06, direction=None):
        """Обход по геометрии, без нормалей модели: в выбранной области (pick(центр грани) -> bool) лицевые грани
        смотрят наружу от center. Для моделей с вывернутыми нормалями (шлем IMP: лицевая пластина — нормали внутрь)
        _measure_winding ошибается, и добавленные плоские детали выходят изнанкой — игра их не рисует (ленты прорезей
        шлема, 11-24). direction — постоянное «наружу» для области (лицевая пластина: (0, 0, -1)) вместо «от center».
        -> (+1/-1, доля граней «наружу»)."""
        c0 = np.asarray(center, dtype=np.float64)
        pos = neg = 0
        for fa in self.lod.faces:
            v = fa.vertices
            a, b, c = (np.array(self.lod.points[x.point_index].coords) for x in v[:3])
            m = (a + b + c) / 3
            out = m - c0 if direction is None else np.asarray(direction, dtype=np.float64)
            if np.linalg.norm(m - c0) < rmin or (pick and not pick(m)):
                continue
            d = np.dot(np.cross(b - a, c - a), out)
            pos += d > 0
            neg += d < 0
        return (1 if pos >= neg else -1), max(pos, neg) / max(pos + neg, 1)

    def sel(self, name):
        key = self.sel_low.get(name.lower())
        if key is None:
            self.lod.new_selection(name)
            self.sel_low[name.lower()] = name
            key = name
        return self.lod.selections[key]

    def delete_faces(self, odol_faces):
        dead = {id(self.mlod_face[fi]) for fi in odol_faces if fi in self.mlod_face}
        self.lod.faces[:] = [f for f in self.lod.faces if id(f) not in dead]
        for s in self.lod.selections.values():
            for f in [f for f in s.faces if id(f) in dead]:
                del s.faces[f]
        return len(dead)

    def to_glow(self, odol_faces):
        """Существующие грани -> светящееся выделение: материал света, вон из Pr1/Pr2 (там hiddenSelections)."""
        faces = [self.mlod_face[fi] for fi in odol_faces if fi in self.mlod_face]
        glow = self.sel("vbglow")
        for f in faces:
            f.texture, f.material = TEX_GLOW, MAT_GLOW
            glow.faces[f] = 1
            for v in f.vertices:
                glow.points[self.lod.points[v.point_index]] = 1
        for nm in ("pr1", "pr2"):
            if nm in self.sel_low:
                s = self.lod.selections[self.sel_low[nm]]
                for f in faces:
                    s.faces.pop(f, None)
        return len(faces)

    def add_mesh(self, mesh, tex, mat, selname, weight_from):
        """Добавить меш: точки, нормали, грани (порядок MLOD), выделение, веса костей от ближайших вершин."""
        lod = self.lod
        cand = np.array(sorted(weight_from))
        CP = self.P[cand]
        base_pt = len(lod.points)
        new_pts = []
        for p, n in zip(mesh.pts, mesh.nrm):
            pt = py3d.Point()
            pt.coords = (float(p[0]), float(p[1]), float(p[2]))
            pt.flags = 0
            pt.mass = None
            lod.points.append(pt)
            lod.facenormals.append((float(n[0]), float(n[1]), float(n[2])))
            new_pts.append(pt)
        nrm_base = len(lod.facenormals) - len(mesh.pts)
        sel = self.sel(selname)
        for f in mesh.faces:
            order = f if self.winding > 0 else f[::-1]
            fa = py3d.Face(lod.points, lod.facenormals)
            fa.flags = 0
            fa.texture, fa.material = tex, mat
            for i in order:
                vx = py3d.Vertex(lod.points, lod.facenormals)
                vx.point_index = base_pt + i
                vx.normal_index = nrm_base + i
                vx.uv = (float(mesh.uv[i][0]), float(mesh.uv[i][1]))
                fa.vertices.append(vx)
            lod.faces.append(fa)
            sel.faces[fa] = 1
        for pt in new_pts:
            sel.points[pt] = 1
        # веса костей: ближайшая вершина оригинала из weight_from
        Q = np.array([p for p in mesh.pts])
        for qi in range(0, len(Q), 512):
            chunk = Q[qi:qi + 512]
            d = ((chunk[:, None, :] - CP[None, :, :]) ** 2).sum(-1)
            near = cand[d.argmin(1)]
            for k, src in enumerate(near):
                for b, w in self.W[src]:
                    if 0 <= b < len(self.bones):
                        s = self.sel(self.bones[b][0])
                        s.points[new_pts[qi + k]] = 1 if w >= 0.999 else w
        return len(mesh.faces)

    def tilt_cell_proxies(self):
        n = 0
        for name in list(self.lod.selections):
            low = name.lower()
            if not low.startswith("proxy:") or "vb_exocell_slot_" not in low:
                continue
            s = self.lod.selections[name]
            pts = list(s.points)
            if len(pts) != 3:
                continue
            anchor = None
            for px in self.lod.get_proxies():
                if px["name"] == name:
                    anchor = np.array(px["anchor"], dtype=np.float64)
            if anchor is None:
                continue
            side = 1 if anchor[0] > 0 else -1
            R = G.rot_z(-side * TILT)
            for p in pts:
                c = np.array(p.coords)
                q = R @ (c - anchor) + anchor + np.array([0.0, 0.0, CBACK])
                p.coords = (float(q[0]), float(q[1]), float(q[2]))
            n += 1
        return n


def cell_anchors(od):
    out = []
    for px in od["proxies"]:
        if "vb_exocell_slot_" in px["name"].lower():
            out.append(np.array(px["matrix"][9:12], dtype=np.float64))
    return out


# ------------------------------------------------------------------ главная
def main():
    import tempfile
    tmp = os.path.join(tempfile.mkdtemp(), "ExoPrivod_m.p3d")      # имя файла = имя класса в model.cfg
    m = M2.convert(SRC, tmp, cfg=CFG)
    os.remove(tmp)
    os.rmdir(os.path.dirname(tmp))
    data = open(SRC, "rb").read()
    ver, nl, res, p = O.header(data)
    skel = M2.parse_skeleton(data, p)
    bones = skel["bones"]
    for li in range(nl):
        r = res[li]
        if r >= 1000:
            continue
        od = O.read_visual_lod(SRC, li)
        W = O.read_bone_weights(data, od)
        ed = LodEditor(m.lods[li], od, W, bones)
        parts = G.thin_parts(od["points"], od["faces"], ed.proxy_faces)
        rep = {"LOD": r}
        # 1) свет на существующих трубках; у центральной трубы нижняя часть (гнётся к поясу) — гофра, без света
        sides = 10 if r <= 2 else 8
        dead = set()
        gl = 0
        for c in GLOW_PARTS:
            part = G.find_part(parts, c)
            if not part:
                continue
            faces = part["faces"]
            if c == GLOW_PARTS[2]:
                lower = {fi for fi in faces if ed.P[list(od["faces"][fi])].mean(0)[1] < PIPE_SPLIT_Y}
                faces = faces - lower
                if lower:
                    verts = {v for fi in part["faces"] for v in od["faces"][fi]}
                    h, collar = pipe_hose(G.centerline(od["points"], od["faces"], sorted(part["faces"])), sides + 2)
                    ed.add_mesh(h, TEX_RUBBER, MAT_RUBBER, "vbhose", verts)
                    ed.add_mesh(collar, TEX_STEEL, MAT_STEEL, "vbparts", verts)
                    dead |= lower
                    rep["гофра трубы граней"] = len(lower)
            gl += ed.to_glow(faces)
        rep["свет"] = gl
        # 2) гофра -> гладкие трубки
        nc = 0
        for c in CABLE_PARTS:
            part = G.find_part(parts, c)
            if not part:
                continue
            line = G.centerline(od["points"], od["faces"], sorted(part["faces"]))
            verts = {v for fi in part["faces"] for v in od["faces"][fi]}
            g, s = cable_meshes(line, sides)
            ed.add_mesh(g, TEX_GLOW, MAT_GLOW, "vbglow", verts)
            ed.add_mesh(s, TEX_STEEL, MAT_STEEL, "vbparts", verts)
            dead |= part["faces"]
            nc += 1
        rep["трубки"] = nc
        # 3) банки -> гнёзда
        cans = {fi for fi, f in enumerate(od["faces"]) if fi not in ed.proxy_faces
                and in_old_can(ed.P[list(f)].mean(0))}
        dead |= cans
        rep["банки граней"] = len(cans)
        allv = set(range(len(od["points"])))
        for a in cell_anchors(od):
            s, g = seat_meshes(a)
            near = {i for i in allv if np.linalg.norm(ed.P[i] - a) < 0.3}
            ed.add_mesh(s, TEX_STEEL, MAT_STEEL, "vbparts", near)
            ed.add_mesh(g, TEX_GLOW, MAT_GLOW, "vbglow", near)
        # 4) индикатор на поясе
        ed.add_mesh(led_mesh(od["points"]), TEX_GLOW, MAT_GLOW, "vbglow",
                    {i for i in allv if abs(ed.P[i][0]) < 0.12 and 0.9 < ed.P[i][1] < 1.1})
        rep["удалено граней"] = ed.delete_faces(dead)
        rep["крепления повёрнуты"] = ed.tilt_cell_proxies()
        print("  ", rep)
    # крепление сдвоенной ячейки холодного синтеза между гнёздами, кость spine3 (2026-11-26, ep_proxies.py)
    import ep_proxies
    print("   прокси пары ячеек:", ep_proxies.patch_servo(m, ep_proxies.ident_template(ep_proxies.load(ep_proxies.CAR))))
    m.save(DST)
    print("MLOD ->", DST)
    # секции vbglow/vbparts — в model.cfg (без них hiddenSelections не работают)
    cfg = open(CFG, encoding="utf-8").read()

    def add_sections(mt):
        items = [x.strip() for x in mt.group(1).split(",") if x.strip()]
        for s in ('"vbglow"', '"vbparts"'):
            if s not in items:
                items.append(s)
        return "sections[] = {%s};" % ", ".join(items)
    head, sep, tail = cfg.partition("class ExoPrivod_m: Default")
    assert sep, "в model.cfg нет класса ExoPrivod_m"
    cfg = head + sep + re.sub(r"sections\[\] = \{([^}]*)\};", add_sections, tail, count=1)
    cfg = cfg.replace("// Восстановлено из ODOL утилитой odol2mlod.py (ExoPrivod_m)",
                      "// Сервопривод экзо Эхо: собрано tools/ec_servo_model.py из ODOL OOTS (odol2mlod с весами костей)")
    open(CFG, "w", encoding="utf-8").write(cfg)
    print("model.cfg ->", CFG)


if __name__ == "__main__":
    main()
