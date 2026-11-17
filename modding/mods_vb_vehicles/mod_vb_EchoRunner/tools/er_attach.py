# -*- coding: utf-8 -*-
r"""Навесные Echo Runner (этап 3): бак Изнанки (оригинальный бак ALEX + светящиеся полосы) и трубопроводы
(левый, правый, мотора). Каждый — отдельная модель с началом в своём центре; в машину ставятся прокси.

  blender.exe -b --factory-startup --python er_attach.py -- <вариант>
  вход:  work/<вариант>/final/ (после er_parts.py), work/<вариант>/tank_alex.p3d (после er_build.py)
  выход: work/<вариант>/full/interceptor.p3d (+ прокси навесных), full/proxy/ (детали машины + навесные)

Прокси навесных: \vb_echorunner\proxy\vb_er_tank (.001 левый, .002 правый), vb_er_pipes_l, vb_er_pipes_r,
vb_er_pipes_engine — в LOD 1–4 и видах из кабины 1100/1201; рамка = тождественная (детали в осях машины).
Светящееся — выделение «neon» в моделях навесных (на этапе мода — hiddenSelection, включается скриптом).
"""
import math
import os
import shutil
import sys

import bpy
import numpy as np
from mathutils import Vector
from mathutils.bvhtree import BVHTree

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import er_common as C  # noqa: E402
import er_mlod as M  # noqa: E402
from er_geom import (Part, V, bar, box, cyl, ring, tube, catmull, offset_path, arclen_points,  # noqa: E402
                     part_data, append)

py3d = C.py3d
args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
VAR = [a for a in args if not a.startswith("--")][0]
MOD = os.path.dirname(HERE)
WD = os.path.join(MOD, "work", VAR)
SRC = os.path.join(WD, "final")
OUT = os.path.join(WD, "full")
PROXY_DIR = r"\vb_echorunner\proxy"
RAW_IDENTITY = ((-1.0, 0.0, 0.0), (0.0, 0.0, 1.0), (0.0, 1.0, 0.0))    # тождественная рамка движка в «сырых» осях
CAR_LODS = (1.0, 2.0, 3.0, 4.0, 1100.0, 1201.0)

bpy.ops.wm.read_factory_settings(use_empty=True)
objs, XF = C.load_car(os.path.join(SRC, "interceptor.p3d"), os.path.join(SRC, "proxy"), 1.0, with_mats=False)
dg = bpy.context.evaluated_depsgraph_get()
SOLID = [k for k in objs if "wheel" not in k]
BVH = {k: BVHTree.FromObject(objs[k], dg) for k in SOLID}


def surface(o, d, dist=1.0):
    best = None
    for k in SOLID:
        loc, n, i, dd = BVH[k].ray_cast(Vector(o), Vector(d).normalized(), dist)
        if loc is not None and (best is None or dd < best[1]):
            best = (loc, dd)
    return best[0] if best else None


# ---------------------------------------------------------------- модели навесных
def item_model(parts_by_lod, geo_boxes, mass):
    """parts_by_lod: {res: [Part]} в осях машины (Blender). Начало модели — центр LOD 1 (оси DayZ).
    geo_boxes: [(центр Blender, полуразмеры)] — выпуклые куски Geometry/Fire/View."""
    pts = []
    for p in parts_by_lod[1.0]:
        pts.extend([(v.co.x, v.co.z, v.co.y) for v in p.bm.verts])
    pts = np.array(pts)
    center = (pts.min(0) + pts.max(0)) / 2
    inv = (center, np.eye(3))
    m = py3d.P3D()
    for res, parts in sorted(parts_by_lod.items()):
        lod = py3d.LOD()
        lod.resolution = res
        for p in parts:
            if p.bm.faces:
                append(lod, part_data(p, inv), p.sels)
        m.lods.append(lod)
    for res in (1e13, 6e15, 7e15):
        lod = py3d.LOD()
        lod.resolution = res
        for i, (c, h) in enumerate(geo_boxes):
            gp = Part("geo", "", [], ["Component%02d" % (i + 1)])
            box(gp, c, V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), h[0], h[1], h[2], "steel")
            d = part_data(gp, inv)
            d = (d[0], [(f[0], f[1], f[2], ("", "")) for f in d[1]])
            append(lod, d, gp.sels)
        if res == 1e13:
            per = mass / max(len(lod.points), 1)
            for p in lod.points:
                p.mass = per
            lod.properties["autocenter"] = "0"
        m.lods.append(lod)
    return m, tuple(float(c) for c in center)


# ---------------------------------------------------------------- бак Изнанки
def tank_item():
    src = C.load_p3d(os.path.join(WD, "tank_alex.p3d"))
    lod1 = [l for l in src.lods if C.is_res(l, 1.0)][0]
    keep = []
    for c in M.components(lod1, lod1.faces):
        bb = M.face_bbox(lod1, c)
        if bb[0][0] >= 0.0 and bb[0][1] <= 0.56 and (bb[0][0] + bb[0][1]) / 2 > 0.15:
            keep.extend(c)
    bb = M.face_bbox(lod1, keep)
    center = np.array([(a + b) / 2 for a, b in bb])                         # оси DayZ
    dist = sorted(math.hypot(lod1.points[v.point_index].coords[0] - center[0],
                             lod1.points[v.point_index].coords[1] - center[1]) for f in keep for v in f.vertices)
    radius = dist[int(len(dist) * 0.80)]                       # поверхность бака, без крышки горловины
    length = bb[2][1] - bb[2][0]
    m = py3d.P3D()
    for res in (1.0, 2.0, 3.0, 4.0):
        l = [x for x in src.lods if C.is_res(x, res)]
        if not l:
            continue
        l = l[0]
        sel = []
        for c in M.components(l, l.faces):
            b = M.face_bbox(l, c)
            if b[0][0] >= 0.0 and b[0][1] <= 0.56 and (b[0][0] + b[0][1]) / 2 > 0.15:
                sel.extend(c)
        new = M.copy_faces_to_lod(l, sel, res)
        for p in new.points:
            p.coords = tuple(float(a - b) for a, b in zip(p.coords, center))
        s = new.new_selection("tank")
        s.points = {p: 1 for p in new.points}
        s.faces = {f: 1 for f in new.faces}
        if res <= 2.0:                                        # светящиеся полосы Изнанки
            neon = Part("tank_neon", "", [], ["neon"])
            house = Part("tank_band", "", [], ["tank"])
            segs = 32 if res == 1.0 else 20
            for zz in (-0.30, 0.0, 0.30):
                zc = zz * length
                ring(house, V(0, zc, 0), V(0, 1, 0), radius - 0.012, radius + 0.010, 0.050, segs, "rustm")
                ring(neon, V(0, zc, 0), V(0, 1, 0), radius + 0.008, radius + 0.016, 0.026, segs, "neon")
            for p in (house, neon):
                append(new, part_data(p), p.sels)
        m.lods.append(new)
    # Geometry / Fire / View: 12-гранная призма по оси бака (ось DayZ z)
    for res in (1e13, 6e15, 7e15):
        g = Part("geo", "", [], ["Component01"])
        cyl(g, V(0, -length / 2, 0), V(0, 1, 0), radius, length, 12, "steel")
        lod = py3d.LOD()
        lod.resolution = res
        d = part_data(g)
        d = (d[0], [(f[0], f[1], f[2], ("", "")) for f in d[1]])
        append(lod, d, g.sels)
        if res == 1e13:
            for p in lod.points:
                p.mass = 35.0 / len(lod.points)
            lod.properties["autocenter"] = "0"
        m.lods.append(lod)
    print("TANK centre (DayZ)", [round(c, 3) for c in center], "R", round(radius, 3), "L", round(length, 3))
    right = (-center[0], center[1], center[2])
    return m, [tuple(float(c) for c in center), tuple(float(c) for c in right)]


# ---------------------------------------------------------------- трубопроводы
R_MAIN, R_SEC = 0.024, 0.017
SEGS = {1.0: 12, 2.0: 8, 3.0: 6, 4.0: 4}


def flange(part, p, t, r, segs):
    cyl(part, p - t * 0.008, t, r * 1.75, 0.016, segs, "pipe")
    for k in range(4):
        a = 2 * math.pi * k / 4 + 0.4
        u = t.cross(Vector((0, 0, 1)) if abs(t.z) < 0.9 else Vector((1, 0, 0))).normalized()
        w = t.cross(u).normalized()
        cyl(part, p + (u * math.cos(a) + w * math.sin(a)) * r * 1.4 + t * 0.006, t, 0.0045, 0.008, 6, "bolt")


def pipe_run(parts, path, sideX, res, glow=True, couple_every=0.33, hose=None, brackets=None, end_flanges=True):
    """Главная (светящаяся) труба + параллельная вторая (сталь; на участке hose=(доля0, доля1) — гофро-рукав).
    parts: {"neon": Part, "metal": Part}. sideX: Vector наружу (для смещения второй трубы)."""
    segs = SEGS[res]
    main = catmull(path, 0.025 if res <= 2 else 0.06)
    tube(parts["neon" if glow else "metal"], main, R_MAIN, segs, "neon" if glow else "pipe")
    sec = offset_path(main, sideX, R_MAIN + R_SEC + 0.012)
    if hose is not None and res <= 2.0:
        a, b = int(hose[0] * (len(sec) - 1)), int(hose[1] * (len(sec) - 1))
        tube(parts["metal"], sec[:a + 1], R_SEC, segs, "pipe")
        hose = catmull(sec[a:b], 0.012)
        tube(parts["metal"], hose, R_SEC * 1.15, segs, "hose", rfun=lambda i: 1.0 + 0.16 * (i % 2))
        if b < len(sec):
            tube(parts["metal"], sec[b - 1:], R_SEC, segs, "pipe")
    else:
        tube(parts["metal"], sec, R_SEC, segs, "pipe")
    if res <= 2.0:
        for p, t in arclen_points(main, couple_every, couple_every * 0.5):
            cyl(parts["metal"], p - t * 0.02, t, R_MAIN * 1.32, 0.04, segs, "pipe")
            cyl(parts["metal"], p - t * 0.024, t, R_MAIN * 1.42, 0.006, segs, "rustm")
            cyl(parts["metal"], p + t * 0.018, t, R_MAIN * 1.42, 0.006, segs, "rustm")
        if end_flanges:
            for pth in (main, sec):
                t0 = (pth[1] - pth[0]).normalized()
                t1 = (pth[-1] - pth[-2]).normalized()
                flange(parts["metal"], pth[0], -t0, R_MAIN, segs)
                flange(parts["metal"], pth[-1], t1, R_MAIN, segs)
    if brackets and res <= 3.0:
        for p, t in arclen_points(main, brackets[1], brackets[0]):
            if not (brackets[2] <= p.y <= brackets[3]):
                continue
            q = p.lerp(offset_path([p - t * 0.01, p, p + t * 0.01], sideX, R_MAIN + R_SEC + 0.012)[1], 0.5)
            hit = surface(q, -sideX, 0.25)
            if hit is None:
                continue
            bar(parts["metal"], hit - sideX * 0.004, q, 0.022, "steel", up=t)
            cyl(parts["metal"], p - t * 0.012, t, R_MAIN * 1.25, 0.024, segs, "steel")
    return main, sec


def side_route(s):
    pts = [(0.92, -0.66, 0.43), (0.92, 0.88, 0.43), (0.96, 0.99, 0.54), (1.06, 1.00, 0.80), (1.05, 1.01, 1.02),
           (1.00, 1.10, 1.22), (0.94, 1.35, 1.33), (0.90, 1.62, 1.42), (0.72, 1.70, 1.47), (0.60, 1.72, 1.40),
           (0.555, 1.72, 1.31)]
    return [(s * x, y, z) for x, y, z in pts]


def engine_route(s):
    # подъём между крылом и дверью -> по верху крыла вперёд -> дугой над капотом к боку нагнетателя
    # (не поперёк проёма у лобового: там светящаяся труба мешала обзору водителя, этап 4)
    pts = [(0.92, -0.66, 0.43), (0.97, -0.61, 0.53), (1.04, -0.60, 0.80), (1.03, -0.60, 1.02), (0.93, -0.66, 1.155),
           (0.895, -0.90, 1.145), (0.885, -1.18, 1.125), (0.66, -1.30, 1.17), (0.34, -1.33, 1.21), (0.155, -1.33, 1.26)]
    return [(s * x, y, z) for x, y, z in pts]


def pipes_side(s):
    out = {}
    for res in (1.0, 2.0, 3.0, 4.0):
        parts = {"neon": Part("neon", "", [], ["neon", "pipes"]), "metal": Part("metal", "", [], ["pipes"])}
        main, sec = pipe_run(parts, side_route(s), V(s, 0, 0), res, hose=(0.50, 0.66), brackets=(0.12, 0.42, -0.6, 0.9))
        if res <= 2.0:                                           # распределительный коллектор на подъёме
            p = main[int(len(main) * 0.47)]
            box(parts["metal"], p + V(s * 0.012, 0, 0), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.038, 0.05, 0.07, "rustm")
            box(parts["neon"], p + V(s * 0.052, 0, 0), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.003, 0.03, 0.045, "neon")
        out[res] = list(parts.values())
    path = [Vector(p) for p in side_route(s)]
    boxes = []
    for a, b in zip(path, path[1:]):
        lo = Vector((min(a.x, b.x), min(a.y, b.y), min(a.z, b.z))) - V(0.05, 0.05, 0.05)
        hi = Vector((max(a.x, b.x), max(a.y, b.y), max(a.z, b.z))) + V(0.05, 0.05, 0.05)
        boxes.append(((lo + hi) / 2, (hi - lo) / 2))
    return item_model(out, boxes, 14.0)


def pipes_engine():
    out = {}
    for res in (1.0, 2.0, 3.0, 4.0):
        parts = {"neon": Part("neon", "", [], ["neon", "pipes"]), "metal": Part("metal", "", [], ["pipes"])}
        for s in (1, -1):
            r = engine_route(s)
            pipe_run(parts, r, V(s, 0, 0), res, hose=(0.06, 0.36), brackets=(0.10, 0.30, -0.65, -0.55))
        out[res] = list(parts.values())
    boxes = []
    for s in (1, -1):
        path = [Vector(p) for p in engine_route(s)]
        for a, b in zip(path, path[1:]):
            lo = Vector((min(a.x, b.x), min(a.y, b.y), min(a.z, b.z))) - V(0.05, 0.05, 0.05)
            hi = Vector((max(a.x, b.x), max(a.y, b.y), max(a.z, b.z))) + V(0.05, 0.05, 0.05)
            boxes.append(((lo + hi) / 2, (hi - lo) / 2))
    return item_model(out, boxes, 12.0)


# ---------------------------------------------------------------- сборка
def add_proxy(car, name, index, center):
    for lod in car.lods:
        if any(C.is_res(lod, r) for r in CAR_LODS):
            lod.add_proxy(PROXY_DIR + "\\" + name, index, center, RAW_IDENTITY, 0.001, "raw")


def main():
    os.makedirs(os.path.join(OUT, "proxy"), exist_ok=True)
    car = C.load_p3d(os.path.join(SRC, "interceptor.p3d"))
    tank, tank_pos = tank_item()
    tank.save(os.path.join(OUT, "proxy", "vb_er_tank.p3d"))
    add_proxy(car, "vb_er_tank", 1, tank_pos[0])
    add_proxy(car, "vb_er_tank", 2, tank_pos[1])
    for s, nm in ((1, "vb_er_pipes_l"), (-1, "vb_er_pipes_r")):
        m, c = pipes_side(s)
        m.save(os.path.join(OUT, "proxy", nm + ".p3d"))
        add_proxy(car, nm, 1, c)
        print("PIPES", nm, "centre (DayZ)", [round(x, 3) for x in c], "LOD1 граней",
              len([l for l in m.lods if C.is_res(l, 1.0)][0].faces))
    m, c = pipes_engine()
    m.save(os.path.join(OUT, "proxy", "vb_er_pipes_engine.p3d"))
    add_proxy(car, "vb_er_pipes_engine", 1, c)
    print("PIPES engine centre (DayZ)", [round(x, 3) for x in c], "LOD1 граней",
          len([l for l in m.lods if C.is_res(l, 1.0)][0].faces))
    car.save(os.path.join(OUT, "interceptor.p3d"))
    for f in os.listdir(os.path.join(SRC, "proxy")):
        shutil.copy2(os.path.join(SRC, "proxy", f), os.path.join(OUT, "proxy", f))
    print("OK", OUT)


main()
