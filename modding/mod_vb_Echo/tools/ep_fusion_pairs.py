# -*- coding: utf-8 -*-
r"""Спаренные предметы холодного синтеза (решение пользователя 2026-11-26): сдвоенный бак Echo Runner и сдвоенная
энергоячейка сервопривода — одна модель из двух штук, ставится одним предметом в свой слот.

  blender.exe -b --factory-startup --python ep_fusion_pairs.py -- <папка вывода> [--place] [--install]
  (--install — ещё и в моды; крепления в машину и сервопривод ставит ep_proxies.py)

Вход — готовые MLOD (читаются, не меняются): бак VB_EchoRunner/proxy/vb_er_tank.p3d, ячейка VB_EchoCore/data/eb_cell.p3d;
места — прокси vb_er_tank_1/2 машины (vb_echorunner.p3d) и vb_exocell_slot_1/2 сервопривода (ExoPrivod_m.p3d).
Выход:
  vb_er_tank_pair.p3d — два бака на своих местах, начало — середина между ними (там встанет прокси vb_er_tank_pair);
  eb_cell_pair.p3d    — две ячейки «галочкой», как в гнёздах, начало — середина между гнёздами (прокси vb_exocell_pair);
  с --place ещё place_tank_pair.p3d (оси машины) и place_cell_pair.p3d (оси сервопривода) — для рендера на месте.
Спайка — рама из er_geom: у баков стяжки на концах, коллектор синтеза в ложбине сверху, узел и подводы к бакам;
у ячеек — хомуты, две перемычки и светящийся мостик между крышками. Свет — выделение neon (бирюзовый синтез;
в игре цвет даёт hiddenSelection). Оси Blender: X вправо, Y — вдоль бака, Z вверх; запись в MLOD — er_geom.part_data.
"""
import math
import os
import shutil
import sys

import numpy as np
from mathutils import Vector

HERE = os.path.dirname(os.path.abspath(__file__))
MODDING = os.path.abspath(os.path.join(HERE, "..", ".."))
ER = os.path.join(MODDING, "mods_vb_vehicles", "mod_vb_EchoRunner")
sys.path.insert(0, os.path.join(ER, "tools"))
import er_common as C  # noqa: E402
from er_geom import Part, V, bar, box, cyl, ring, tube, catmull, part_data, append  # noqa: E402

py3d = C.py3d
args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
OUT = [a for a in args if not a.startswith("--")][0]
PLACE = "--place" in args
INSTALL = "--install" in args       # ещё и в моды: VB_EchoRunner/proxy, VB_EchoCore/data, заглушка у сервопривода
os.makedirs(OUT, exist_ok=True)

CAR = os.path.join(ER, "VB_EchoRunner", "vb_echorunner.p3d")
TANK = os.path.join(ER, "VB_EchoRunner", "proxy", "vb_er_tank.p3d")
SERVO = os.path.join(MODDING, "mod_vb_Echo", "VB_EchoClothes", "exo_set", "Backpacks", "ExoPrivod", "ExoPrivod_m.p3d")
CELL = os.path.join(MODDING, "mod_vb_Echo", "VB_EchoCore", "data", "eb_cell.p3d")

ER_PARTS = "vb_echorunner\\data\\parts\\"
CORE = "vb_echocore\\data\\"
FUSION_ER = ER_PARTS + "er_neon_fusion_co.paa"
FUSION_CORE = CORE + "er_neon_fusion_co.paa"
GEO_RES = (1e13, 6e15, 7e15)
MEM_RES = 1e15


def B(v):
    """Оси DayZ (x, вверх, z) -> Blender (x, z, вверх)."""
    return Vector((float(v[0]), float(v[2]), float(v[1])))


def slots(path, names):
    m = C.load_p3d(path)
    lod = [l for l in m.lods if C.is_res(l, 1.0)][0]
    out = {}
    for pr in lod.get_proxies():
        for n in names:
            if n in pr["name"].lower():
                a, a0, a1, a2 = C.proxy_xform(pr)
                out[n] = (np.array(a), np.column_stack([a0, a1, a2]))
    assert len(out) == len(names), (path, sorted(out))
    return out


def copy_into(dst, src, R, t, tex_fn, sel_name=lambda n: n):
    """Грани src -> dst: точка c -> R @ c + t (оси DayZ). Выделения переносятся (sel_name -> None: пропустить)."""
    flip = np.linalg.det(R) < 0
    base_p, base_n = len(dst.points), len(dst.facenormals)
    newp = []
    for p in src.points:
        q = py3d.Point()
        q.coords = tuple(float(x) for x in (R @ np.array(p.coords) + t))
        q.flags = p.flags
        q.mass = p.mass
        dst.points.append(q)
        newp.append(q)
    for n in src.facenormals:
        dst.facenormals.append(tuple(float(x) for x in (R @ np.array(n))))
    newf = []
    for f in src.faces:
        nf = py3d.Face(dst.points, dst.facenormals)
        nf.texture, nf.material = tex_fn(f.texture, f.material)
        nf.flags = f.flags
        verts = list(reversed(f.vertices)) if flip else f.vertices
        for v in verts:
            nv = py3d.Vertex(dst.points, dst.facenormals)
            nv.point_index = base_p + v.point_index
            nv.normal_index = base_n + v.normal_index
            nv.uv = v.uv
            nf.vertices.append(nv)
        dst.faces.append(nf)
        newf.append(nf)
    dst.sharp_edges.extend((base_p + a, base_p + b) for a, b in src.sharp_edges)
    pi = {id(p): i for i, p in enumerate(src.points)}
    fi = {id(f): i for i, f in enumerate(src.faces)}
    for name, sel in src.selections.items():
        nm = sel_name(name)
        if nm is None:
            continue
        s = dst.new_selection(nm)
        s.points.update({newp[pi[id(p)]]: w for p, w in sel.points.items()})
        s.faces.update({newf[fi[id(f)]]: w for f, w in sel.faces.items()})


def new_lod(res, props=None):
    l = py3d.LOD()
    l.resolution = res
    for k, v in (props or {}).items():
        l.properties[k] = v
    return l


def add_parts(lod, parts, offset, tex_fn):
    """Детали (Blender, начало модели) -> LOD; offset — сдвиг всей модели (оси DayZ)."""
    xf = (-np.array(offset, dtype=float), np.eye(3))
    for p in parts:
        if not p.bm.faces:
            continue
        pts, faces = part_data(p, xf)
        faces = [(i, uv, n, tex_fn(*tm)) for i, uv, n, tm in faces]
        append(lod, (pts, faces), p.sels)


def memory_lod(lod1):
    pts = np.array([p.coords for p in lod1.points])
    lo, hi = pts.min(0), pts.max(0)
    c = (lo + hi) / 2
    r = float(max(hi - lo))
    l = new_lod(MEM_RES)
    for name, co in (("invview", (c[0] - r * 1.2, c[1] + r * 0.9, c[2] - r * 1.2)), ("ce_center", c),
                     ("ce_radius", (c[0] + r / 2, c[1], c[2])), ("boundingbox_min", lo), ("boundingbox_max", hi)):
        s = l.new_selection(name)
        pt = py3d.Point()
        pt.coords = tuple(float(x) for x in co)
        l.points.append(pt)
        s.points[pt] = 1
    return l


def geo_mass(lod, total):
    for p in lod.points:
        p.mass = total / max(len(lod.points), 1)


# ======================================================================= сдвоенный бак
def tank_frame(res, segs):
    """Рама сдвоенного бака (Blender, начало — середина между баками). Возвращает [Part]."""
    frame = Part("tp_frame", "", [], ["tank"])
    neon = Part("tp_neon", "", [], ["neon"])
    if res > 3.0:
        return []
    x0, rt = TANK_X, TANK_R
    z0 = math.sqrt(max((rt + 0.045) ** 2 - x0 ** 2, 0.0))         # коллектор лежит в ложбине на обоих баках
    for y in (-0.42, 0.42):                                       # стяжки у концов
        for sx in (-1, 1):
            ring(frame, V(sx * x0, y, 0), V(0, 1, 0), rt - 0.002, rt + 0.018, 0.07, segs, "steel")
        for sz in (-1, 1):                                        # замки стяжек в ложбинах сверху и снизу
            zz = sz * (z0 if sz > 0 else z0 + 0.01)
            box(frame, V(0, y, zz), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.075, 0.042, 0.055, "steel")
            if res <= 2.0:
                for bx in (-0.045, 0.045):
                    cyl(frame, V(bx, y - 0.048, zz), V(0, 1, 0), 0.011, 0.012, 8, "bolt")
    # коллектор синтеза: светящаяся труба между замками, хомуты
    tube(neon, [V(0, -0.375, z0), V(0, 0.375, z0)], 0.040, segs, "neon")
    if res <= 2.0:
        for y in (-0.30, -0.17, 0.17, 0.30):
            cyl(frame, V(0, y - 0.014, z0), V(0, 1, 0), 0.050, 0.028, segs, "rustm")
        # узел синтеза в середине: корпус и два светящихся кольца
        cyl(frame, V(0, -0.055, z0), V(0, 1, 0), 0.068, 0.11, segs, "steel")
        for y in (-0.072, 0.062):
            ring(neon, V(0, y + 0.005, z0), V(0, 1, 0), 0.054, 0.076, 0.012, segs, "neon")
        # подводы от коллектора к бакам (по два на бак), фланцы на баке
        a = math.radians(112)
        for y in (-0.24, 0.24):
            for sx in (-1, 1):
                tp = V(sx * (x0 + rt * math.cos(a)), y, rt * math.sin(a))
                n = (tp - V(sx * x0, y, 0)).normalized()
                st = V(sx * 0.030, y, z0 + 0.022)
                path = catmull([st, (st + tp) / 2 + V(0, 0, 0.035), tp + n * 0.012], 0.012)
                tube(neon, path, 0.011, 10, "neon")
                cyl(frame, tp - n * 0.004, n, 0.024, 0.016, 12, "rustm")
    return [frame, neon]


def build_tank(offset):
    src = C.load_p3d(TANK)
    m = py3d.P3D()
    neon_er = lambda t, mt: ((FUSION_ER, mt) if t.lower().endswith("er_neon_co.paa") else (t, mt))  # noqa: E731
    for sl in src.lods:
        res = sl.resolution
        if abs(res / MEM_RES - 1) < 1e-3:              # у бака своё превью (er_invview.py), у пары — memory_lod ниже
            continue
        if any(abs(res / g - 1) < 1e-3 for g in GEO_RES):
            l = new_lod(res, dict(sl.properties))
            for k, sx in enumerate((1, -1)):
                copy_into(l, sl, np.eye(3), np.array(offset) + np.array((sx * TANK_X, 0.0, 0.0)), neon_er,
                          lambda n, k=k: n.replace("01", "%02d" % (k + 1)) if n.startswith("Component") else n)
            if abs(res / 1e13 - 1) < 1e-3:
                geo_mass(l, 70.0)          # как два бака (2 × 35 кг)
                l.properties["autocenter"] = "0"
            m.lods.append(l)
            continue
        l = new_lod(res, dict(sl.properties))
        for sx in (1, -1):
            copy_into(l, sl, np.eye(3), np.array(offset) + np.array((sx * TANK_X, 0.0, 0.0)), neon_er)
        segs = {1.0: 24, 2.0: 16, 3.0: 10}.get(res, 8)
        add_parts(l, tank_frame(res, segs), offset, neon_er)
        m.lods.append(l)
    m.lods.append(memory_lod(m.lods[0]))
    m.lods.sort(key=lambda x: x.resolution)
    return m


# ======================================================================= сдвоенная энергоячейка
CELL_H = 0.1935


def cell_frame(res, cells):
    """cells: [(база, ось)] в Blender (начало — середина между гнёздами)."""
    frame = Part("cp_frame", "", [], ["cell"])
    neon = Part("cp_neon", "", [], ["neon"])
    segs = 20 if res <= 1.0 else 12
    (b1, a1), (b2, a2) = cells
    up = (a1 + a2).normalized()
    for h in (0.035, 0.165):                                   # хомуты и перемычки
        c1, c2 = b1 + a1 * h, b2 + a2 * h
        ring(frame, c1, a1, 0.0365, 0.046, 0.016, segs, "steel")
        ring(frame, c2, a2, 0.0365, 0.046, 0.016, segs, "steel")
        d = (c2 - c1).normalized()
        bar(frame, c1 + d * 0.044, c2 - d * 0.044, 0.014, "steel", up=up)
    if res > 2.0:
        return [frame, neon]
    # светящийся мостик между крышками
    t1, t2 = b1 + a1 * (CELL_H - 0.006), b2 + a2 * (CELL_H - 0.006)
    d = (t2 - t1).normalized()
    p1, p2 = t1 + d * 0.024, t2 - d * 0.024
    mid = (p1 + p2) / 2 + up * 0.05
    tube(neon, catmull([p1, (p1 + mid) / 2 + up * 0.012, mid, (p2 + mid) / 2 + up * 0.012, p2], 0.008), 0.0055, 8,
         "neon")
    for p, ax in ((p1, a1), (p2, a2)):
        cyl(frame, p - ax * 0.010, ax, 0.010, 0.012, 10, "steel")
    # значок спайки на верхней перемычке: шайба со светящимися донышками
    c1, c2 = b1 + a1 * 0.165, b2 + a2 * 0.165
    m = (c1 + c2) / 2
    n = (c2 - c1).cross(up).normalized()
    cyl(frame, m - n * 0.011, n, 0.019, 0.022, 16, "steel")
    for s in (-1, 1):
        cyl(neon, m + n * (s * 0.0112) - n * 0.0011, n, 0.012, 0.0022, 16, "neon")
    return [frame, neon]


def build_cell(offset):
    src = C.load_p3d(CELL)
    sl = slots(SERVO, ["vb_exocell_slot_1", "vb_exocell_slot_2"])
    (an1, R1), (an2, R2) = sl["vb_exocell_slot_1"], sl["vb_exocell_slot_2"]
    mid = (an1 + an2) / 2
    print("CELL slots: mid", np.round(mid, 4), "det", round(np.linalg.det(R1), 4), round(np.linalg.det(R2), 4))
    places = [(an1 - mid + offset, R1), (an2 - mid + offset, R2)]
    cells_b = [(B(t - offset), B(R[:, 1]).normalized()) for t, R in places]

    def tex(t, mt):
        tl = t.lower()
        if tl.endswith("er_neon_co.paa") or "eb_neon_t" in tl:
            return FUSION_CORE, mt
        return t.replace(ER_PARTS, CORE), mt.replace(ER_PARTS, CORE)

    m = py3d.P3D()
    for s in src.lods:
        res = s.resolution
        if abs(res / MEM_RES - 1) < 1e-3:
            continue
        l = new_lod(res, dict(s.properties))
        geo = any(abs(res / g - 1) < 1e-3 for g in GEO_RES)
        for k, (t, R) in enumerate(places):
            copy_into(l, s, R, t, tex,
                      lambda n, k=k: (n.replace("01", "%02d" % (k + 1)) if n.startswith("Component") else n))
        if not geo:
            add_parts(l, cell_frame(res, cells_b), offset, tex)
        m.lods.append(l)
    m.lods.append(memory_lod(m.lods[0]))
    m.lods.sort(key=lambda x: x.resolution)
    return m, mid


# ======================================================================= запуск
ts = slots(CAR, ["vb_er_tank_1", "vb_er_tank_2"])
t1, t2 = ts["vb_er_tank_1"][0], ts["vb_er_tank_2"][0]
TANK_MID = (t1 + t2) / 2
TANK_X = float(abs(t1[0] - t2[0]) / 2)
TANK_R = 0.2724
print("TANK slots: mid", np.round(TANK_MID, 4), "half spacing", round(TANK_X, 4))

written = []
tank_m = build_tank(np.zeros(3))
cell_m, CELL_MID = build_cell(np.zeros(3))
tank_m.save(os.path.join(OUT, "vb_er_tank_pair.p3d"))
cell_m.save(os.path.join(OUT, "eb_cell_pair.p3d"))
written += [os.path.join(OUT, "vb_er_tank_pair.p3d"), os.path.join(OUT, "eb_cell_pair.p3d")]
if PLACE:
    build_tank(TANK_MID).save(os.path.join(OUT, "place_tank_pair.p3d"))
    build_cell(CELL_MID)[0].save(os.path.join(OUT, "place_cell_pair.p3d"))
    written += [os.path.join(OUT, "place_tank_pair.p3d"), os.path.join(OUT, "place_cell_pair.p3d")]
if INSTALL:
    # в моды: модель предмета = модель прокси у бака (как vb_er_tank_1/2), у ячейки — data ядра; заглушка крепления
    # сервопривода — копия заглушки гнезда ячейки (движок рисует вложенный предмет, ep_proxies.py ставит крепления)
    dst_tank = os.path.join(ER, "VB_EchoRunner", "proxy", "vb_er_tank_pair.p3d")
    dst_cell = os.path.join(MODDING, "mod_vb_Echo", "VB_EchoCore", "data", "eb_cell_pair.p3d")
    srv_dir = os.path.dirname(SERVO)
    tank_m.save(dst_tank)
    cell_m.save(dst_cell)
    shutil.copy2(os.path.join(srv_dir, "vb_exocell_slot_1.p3d"), os.path.join(srv_dir, "vb_exocell_pair.p3d"))
    written += [dst_tank, dst_cell, os.path.join(srv_dir, "vb_exocell_pair.p3d")]
for fn in written:
    with open(fn, "rb") as fh:
        if fh.read(4) != b"MLOD":                     # заглушка крепления — ODOL OOTS, только копия
            print("WROTE", fn, "(копия заглушки)")
            continue
    mm = C.load_p3d(fn)
    print("WROTE", fn, [(round(l.resolution, 1) if l.resolution < 1e3 else "%.0e" % l.resolution, len(l.faces))
                        for l in mm.lods])
