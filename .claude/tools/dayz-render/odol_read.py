# -*- coding: utf-8 -*-
"""Чтение визуального LOD из бинарной модели DayZ (ODOL v54) для рендера.

Свой код по образцу декомпилированного BisDll (ScripyZz/BisDLL-Arma-3, только как справочник):
  LOD.read, Polygons, Section, NamedSelection, UVSet, EmbeddedMaterial, сжатые массивы (LZO1X).
Заголовок модели (ModelInfo, анимации) в DayZ отличается от Arma, поэтому таблица адресов LOD
ищется перебором, как в .claude/skills/dayz-vehicles/references/gate_odol_sections.py.

read_visual_lod(path, index=0) -> dict:
  res, points (N,3) в осях DayZ, normals (N,3), uv (N,2), faces [список индексов 3/4],
  face_tex [путь текстуры на грань], face_mat [путь rvmat на грань], textures, materials
"""
import struct


class R:
    def __init__(self, data, pos=0, version=54):
        self.d = data
        self.p = pos
        self.v = version

    def u8(self):
        b = self.d[self.p]
        self.p += 1
        return b

    def boolean(self):
        return self.u8() != 0

    def u16(self):
        x = struct.unpack_from("<H", self.d, self.p)[0]
        self.p += 2
        return x

    def i16(self):
        x = struct.unpack_from("<h", self.d, self.p)[0]
        self.p += 2
        return x

    def u32(self):
        x = struct.unpack_from("<I", self.d, self.p)[0]
        self.p += 4
        return x

    def i32(self):
        x = struct.unpack_from("<i", self.d, self.p)[0]
        self.p += 4
        return x

    def f32(self):
        x = struct.unpack_from("<f", self.d, self.p)[0]
        self.p += 4
        return x

    def vec(self):
        x = struct.unpack_from("<3f", self.d, self.p)
        self.p += 12
        return x

    def asciiz(self):
        e = self.d.index(b"\0", self.p)
        s = self.d[self.p:e].decode("latin-1")
        self.p = e + 1
        return s

    def raw(self, n):
        b = self.d[self.p:self.p + n]
        if len(b) != n:
            raise EOFError("raw %d at %d" % (n, self.p))
        self.p += n
        return b

    # --- сжатие: v54 -> LZO, если размер >= 1024, флага сжатия нет (он с v64)
    def compressed(self, size):
        if size == 0:
            return b""
        if size < 1024:
            return self.raw(size)
        out, self.p = lzo1x(self.d, self.p, size)
        return out

    def compressed_array(self, elem):
        n = self.i32()
        return n, self.compressed(n * elem)

    def condensed_array(self, elem):
        n = self.i32()
        if self.boolean():                      # все элементы одинаковые
            return n, self.raw(elem) * n
        return n, self.compressed(n * elem)


def lzo1x(src, ip, out_len):
    """LZO1X (как lzo1x_decompress_safe). Возвращает (bytes, позиция после потока)."""
    out = bytearray()
    state = 0
    t = src[ip]
    if t > 17:
        ip += 1
        t -= 17
        out += src[ip:ip + t]
        ip += t
        state = t if t < 4 else 4
    while True:
        t = src[ip]
        ip += 1
        if t < 16:
            if state == 0:                      # серия литералов
                if t == 0:
                    while src[ip] == 0:
                        t += 255
                        ip += 1
                    t += 15 + src[ip]
                    ip += 1
                t += 3
                out += src[ip:ip + t]
                ip += t
                state = 4
                continue
            nxt = t & 3
            if state == 4:                      # совпадение 3 байта, дальняя дистанция
                m = len(out) - (1 + 0x0800) - (t >> 2) - (src[ip] << 2)
                ln = 3
            else:                               # совпадение 2 байта
                m = len(out) - 1 - (t >> 2) - (src[ip] << 2)
                ln = 2
            ip += 1
        elif t >= 64:
            nxt = t & 3
            m = len(out) - 1 - ((t >> 2) & 7) - (src[ip] << 3)
            ip += 1
            ln = (t >> 5) + 1
        elif t >= 32:
            ln = t & 31
            if ln == 0:
                while src[ip] == 0:
                    ln += 255
                    ip += 1
                ln += 31 + src[ip]
                ip += 1
            ln += 2
            v = src[ip] | (src[ip + 1] << 8)
            ip += 2
            m = len(out) - 1 - (v >> 2)
            nxt = v & 3
        else:                                   # 16..31
            m = len(out) - ((t & 8) << 11)
            ln = t & 7
            if ln == 0:
                while src[ip] == 0:
                    ln += 255
                    ip += 1
                ln += 7 + src[ip]
                ip += 1
            ln += 2
            v = src[ip] | (src[ip + 1] << 8)
            ip += 2
            m -= v >> 2
            nxt = v & 3
            if m == len(out):                   # конец потока
                break
            m -= 0x4000
        if m < 0:
            raise ValueError("LZO: ссылка назад за начало (%d)" % m)
        if ln <= len(out) - m:
            out += out[m:m + ln]
        else:
            for k in range(ln):
                out.append(out[m + k])
        out += src[ip:ip + nxt]
        ip += nxt
        state = nxt
    if len(out) != out_len:
        raise ValueError("LZO: размер %d, ожидалось %d" % (len(out), out_len))
    return bytes(out), ip


# ------------------------------------------------------------------ структуры LOD
def skip_proxy(r):
    r.asciiz()
    r.raw(48)                                   # Matrix4P: 3x3 + позиция
    r.i32(); r.i32(); r.i32()
    if r.v >= 40:
        r.i32()


def read_material(r):
    name = r.asciiz()
    ver = r.u32()
    r.raw(6 * 16)                               # emissive, ambient, diffuse, forcedDiffuse, specular, specularCopy
    r.f32()
    r.u32(); r.u32(); r.u32(); r.u32()          # pixel/vertex shader, mainLight, fogMode
    if ver == 3:
        r.boolean()
    if ver >= 6:
        r.asciiz()                              # surface
    if ver >= 4:
        r.u32(); r.u32()
    n_st = r.u32() if ver > 6 else 0
    n_tg = r.u32() if ver > 8 else 0
    stages = []

    def stage_tex():
        if ver >= 5:
            r.u32()
        tex = r.asciiz()
        if ver >= 8:
            r.u32()
        if ver >= 11:
            r.boolean()
        return tex

    if ver < 8:
        for _ in range(n_st):
            r.u32(); r.raw(48)
            stages.append(stage_tex())
    else:
        for _ in range(n_st):
            stages.append(stage_tex())
        for _ in range(n_tg):
            r.u32(); r.raw(48)
    if ver >= 10:
        stage_tex()                             # stageTI
    return {"name": name, "version": ver, "stages": stages}


def skip_materials(r, nmat, vcount, vi):
    """Встроенные материалы DayZ (версия 20) не совпадают с разметкой Arma: пропускаем блок целиком.
    За ним идут pointToVertex (n, сжатые n*vi) и vertexToPoint (vcount, ...): ищем смещение, где первый
    массив распаковывается и сразу за ним стоит vcount. Имена материалов — строки *.rvmat в блоке, по порядку."""
    start = r.p
    d = r.d
    for off in range(start, min(len(d) - 16, start + 50000)):
        nf, sz, z = struct.unpack_from("<IIH", d, off)
        if nf == 0 or nf > 1000000 or z != 0 or not (nf * 8 <= sz <= nf * 10):
            continue
        # проверка: первые грани — 3/4 индекса меньше vcount
        p, ok = off + 10, True
        for _ in range(min(nf, 64)):
            k = d[p]
            if k not in (3, 4):
                ok = False
                break
            idx = struct.unpack_from("<%dH" % k, d, p + 1) if vi == 2 else struct.unpack_from("<%di" % k, d, p + 1)
            if any(i >= vcount for i in idx):
                ok = False
                break
            p += 1 + k * vi
        if not ok:
            continue
        # перед гранями — pointToVertex и vertexToPoint; в DayZ они пустые (два нулевых счётчика)
        off -= 8
        if True:
            names = []
            p = start
            while p < off:
                e = d.find(b"\0", p, off)
                if e < 0:
                    break
                s = d[p:e]
                if s.lower().endswith(b".rvmat") and all(32 <= c < 127 for c in s):
                    names.append(s.decode("latin-1"))
                p = e + 1
            # имя материала — первая строка блока; в стадиях .rvmat не встречаются
            r.p = off
            return [{"name": nm, "stages": []} for nm in names[:nmat]]
    raise ValueError("не нашёл конец блока материалов")


def read_section(r):
    lo, hi = r.i32(), r.i32()
    r.i32(); r.i32(); r.u32()
    tex = r.i16()
    special = r.u32()
    mat = r.i32()
    mat_name = r.asciiz() if mat == -1 else None
    if r.v >= 36:
        n = r.u32()
        r.raw(4 * n)
        if r.v >= 67 and r.i32() >= 1:
            r.raw(44)
    else:
        r.f32()
    return {"lo": lo, "hi": hi, "tex": tex, "mat": mat, "mat_name": mat_name, "special": special}


def read_named_selection(r):
    vi = 4 if r.v >= 69 else 2
    name = r.asciiz()
    nf, fdata = r.compressed_array(vi)
    r.i32()
    sectional = r.boolean()
    ns, sdata = r.compressed_array(4)
    r.compressed_array(vi)
    r.compressed(r.i32())
    fmt = "<%dH" % nf if vi == 2 else "<%di" % nf
    return {"name": name, "faces": list(struct.unpack_from(fmt, fdata)) if nf else [],
            "sectional": sectional, "sections": list(struct.unpack_from("<%di" % ns, sdata)) if ns else []}


def read_uvset(r):
    mn = mx = None
    if r.v >= 45:
        mn = (r.f32(), r.f32())
        mx = (r.f32(), r.f32())
    n = r.u32()
    fill = r.boolean()
    elem = 4 if r.v >= 45 else 8
    if fill:
        data = r.raw(elem) * n
    else:
        data = r.compressed(n * elem)
    uv = []
    if r.v >= 45:
        su, sv = mx[0] - mn[0], mx[1] - mn[1]
        for i in range(n):
            a, b = struct.unpack_from("<hh", data, i * 4)
            uv.append((1.52587890625e-05 * (a + 32767) * su + mn[0],
                       1.52587890625e-05 * (b + 32767) * sv + mn[1]))
    else:
        for i in range(n):
            uv.append(struct.unpack_from("<ff", data, i * 8))
    return uv


def read_lod(r, res):
    for _ in range(r.i32()):
        skip_proxy(r)
    n = r.i32(); r.raw(4 * n)                   # subSkeletonsToSkeleton
    for _ in range(r.i32()):                    # skeletonToSubSkeleton
        k = r.i32(); r.raw(4 * k)
    vcount = None
    if r.v >= 50:
        vcount = r.u32()
    else:
        r.condensed_array(4)
    if r.v >= 51:
        r.f32()
    r.i32(); r.i32()
    r.raw(36); r.f32()                          # bMin, bMax, bCenter, bRadius
    textures = [r.asciiz() for _ in range(r.i32())]
    nmat = r.i32()
    vi = 4 if r.v >= 69 else 2
    materials = skip_materials(r, nmat, vcount, vi)
    r.compressed_array(vi)                      # pointToVertex
    r.compressed_array(vi)                      # vertexToPoint
    nf = r.u32(); r.u32(); r.u16()
    faces = []
    for _ in range(nf):
        k = r.u8()
        if vi == 2:
            faces.append(list(struct.unpack_from("<%dH" % k, r.raw(2 * k))))
        else:
            faces.append(list(struct.unpack_from("<%di" % k, r.raw(4 * k))))
    sections = [read_section(r) for _ in range(r.i32())]
    selections = [read_named_selection(r) for _ in range(r.i32())]
    props = []
    for _ in range(r.u32()):
        props.append((r.asciiz(), r.asciiz()))
    for _ in range(r.i32()):                    # keyframes
        r.f32()
        r.raw(12 * r.u32())
    r.i32(); r.i32(); r.i32()
    r.boolean()
    r.u32()
    if r.v >= 50:
        r.condensed_array(4)                    # clip
    uvsets = [read_uvset(r)]
    for _ in range(r.u32() - 1):
        uvsets.append(read_uvset(r))
    nv, vdata = r.compressed_array(12)
    points = [struct.unpack_from("<3f", vdata, i * 12) for i in range(nv)]
    nn, ndata = r.condensed_array(4)
    normals = []
    for i in range(nn):
        val = struct.unpack_from("<i", ndata, i * 4)[0]
        comp = []
        for sh in (0, 10, 20):
            c = (val >> sh) & 1023
            if c > 511:
                c -= 1024
            comp.append(c * -0.00195694715)
        normals.append(tuple(comp))
    # дальше STCoords, кости — для рендера не нужны

    # грани секций (смещения в байтах: 8 на треугольник, +2 на четырёхугольник при 16-бит индексах)
    step, extra = (8, 2) if vi == 2 else (16, 4)
    face_tex = [None] * nf
    face_mat = [None] * nf
    face_sec = [None] * nf
    for si, s in enumerate(sections):
        off = 0
        for fi in range(nf):
            if s["lo"] <= off < s["hi"]:
                face_sec[fi] = si
                face_tex[fi] = textures[s["tex"]] if 0 <= s["tex"] < len(textures) else ""
                if s["mat"] == -1:
                    face_mat[fi] = s["mat_name"] or ""
                elif 0 <= s["mat"] < len(materials):
                    face_mat[fi] = materials[s["mat"]]["name"]
            off += step + (extra if len(faces[fi]) == 4 else 0)
            if off >= s["hi"]:
                break
    # грани каждого именованного выделения: по секциям (sectional) или по списку граней
    for sel in selections:
        if sel["sections"]:
            ss = set(sel["sections"])
            sel["face_set"] = [fi for fi in range(nf) if face_sec[fi] in ss]
        else:
            sel["face_set"] = [fi for fi in sel["faces"] if fi < nf]
    return {"res": res, "points": points, "normals": normals, "uv": uvsets[0], "faces": faces,
            "face_tex": face_tex, "face_mat": face_mat, "face_sec": face_sec, "textures": textures,
            "materials": materials, "sections": sections, "selections": selections, "props": props}


# ------------------------------------------------------------------ заголовок
def header(data):
    if data[:4] != b"ODOL":
        raise ValueError("не ODOL")
    ver = struct.unpack_from("<I", data, 4)[0]
    p = 8
    if ver >= 59:
        p += 4
    if ver >= 58:
        p = data.index(b"\0", p) + 1
    n = struct.unpack_from("<i", data, p)[0]
    p += 4
    res = list(struct.unpack_from("<%df" % n, data, p))
    p += 4 * n
    return ver, n, res, p


def find_table(data, n, start):
    """Таблица start[n], end[n], permanent[n] (как find_table в gate_odol_sections.py)."""
    size = len(data)
    best = None
    for off in range(start, min(size - n * 9, start + 400000)):
        ls = struct.unpack_from("<%dI" % n, data, off)
        if any(s == 0 or s >= size for s in ls):
            continue
        le = struct.unpack_from("<%dI" % n, data, off + 4 * n)
        if any(e > size or e < s for s, e in zip(ls, le)):
            continue
        pm = data[off + 8 * n: off + 9 * n]
        if any(x not in (0, 1) for x in pm):
            continue
        score = 0
        if max(le) >= size - 8:
            score += 100
        srt = sorted(zip(ls, le))
        if all(srt[i][1] <= srt[i + 1][0] for i in range(n - 1)):
            score += 60                         # LOD не перекрываются
        if all(ls[i] > off for i in range(n)):
            score += 20
        if best is None or score > best[0]:
            best = (score, off, ls, le, list(pm))
    return best


def read_visual_lod(path, index=0):
    data = open(path, "rb").read()
    ver, n, res, p = header(data)
    tab = find_table(data, n, p)
    if not tab:
        raise ValueError("таблица LOD не найдена")
    score, off, ls, le, pm = tab
    r = R(data, ls[index], ver)
    lod = read_lod(r, res[index])
    lod.update({"version": ver, "resolutions": res, "table_off": off, "table_score": score,
                "lod_start": ls[index], "lod_end": le[index], "read_end": r.p})
    return lod


if __name__ == "__main__":
    import sys
    for path in sys.argv[1:]:
        lod = read_visual_lod(path)
        print(path)
        print("  v%d res=%s table@%d score=%d" % (lod["version"], lod["resolutions"][:6], lod["table_off"], lod["table_score"]))
        print("  LOD0 %d..%d, прочитано до %d" % (lod["lod_start"], lod["lod_end"], lod["read_end"]))
        print("  points=%d normals=%d uv=%d faces=%d sections=%d" % (len(lod["points"]), len(lod["normals"]),
              len(lod["uv"]), len(lod["faces"]), len(lod["sections"])))
        print("  textures:", lod["textures"])
        print("  materials:", [(m["name"], m["stages"]) for m in lod["materials"]])
        xs = [p[0] for p in lod["points"]]; ys = [p[1] for p in lod["points"]]; zs = [p[2] for p in lod["points"]]
        print("  bbox x %.3f..%.3f y %.3f..%.3f z %.3f..%.3f" % (min(xs), max(xs), min(ys), max(ys), min(zs), max(zs)))
        print("  faces without section:", sum(1 for t in lod["face_tex"] if t is None))
        print("  selections:", [(s["name"], len(s["face_set"]), "sec" if s["sections"] else "")
                                for s in lod["selections"] if s["face_set"]])
