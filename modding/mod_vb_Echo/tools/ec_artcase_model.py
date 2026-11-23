# -*- coding: utf-8 -*-
r"""Модель капсулы для артефактов экзо Эхо VB_EC_ArtCase (VB_EchoClothes, этап 3б; ТЗ раздел 8.4).

  python ec_artcase_model.py         (Python из Blender 5.3: нужен numpy)

Пишет VB_EchoClothes/exo_set/artcase/vb_ec_artcase.p3d (MLOD) + model.cfg: LOD 1/2/3, Geometry (восьмигранная призма,
масса), View/Fire Geometry, Memory (invview, ce_*, габариты). Бинаризует pbo.ps1 -Binarize.
Капсула висит на левом бедре сервопривода (прокси vb_ec_artcase_box, бывший кейс OOTS) и лежит на земле той же моделью.
Начало модели = точка крепления; капсула круглая (4 окна по кругу), поэтому поворот крепления не важен.
Тёмная сталь (vbbody), окна и кольцо на крышке — свет (vbglow): горит, когда внутри артефакт (скрипт).
Оси — DayZ (x, y вверх, z).
"""
import math
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
EDIT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(EDIT, ".claude", "tools", "py3d"))
sys.path.insert(0, HERE)
import py3d  # noqa: E402
import ec_geom as G  # noqa: E402

BS = chr(92)
OUTDIR = os.path.join(HERE, "..", "VB_EchoClothes", "exo_set", "artcase")
DST = os.path.join(OUTDIR, "vb_ec_artcase.p3d")
CFG = os.path.join(OUTDIR, "model.cfg")
DATA = BS.join(["vb_echoclothes", "exo_set", "backpacks", "exoprivod", "data", ""])
STEEL = (DATA + "vb_ec_steel_dark_co.paa", DATA + "vb_ec_steel_dark.rvmat")
GLOW = (DATA + "vb_ec_neon_empty_co.paa", DATA + "vb_ec_neon_off.rvmat")
C = np.array([-0.03, -0.08, 0.0])                   # центр капсулы от точки крепления (как в концепте)
UP = np.array([0.0, 1.0, 0.0])
MASS = 0.6


def build(seg):
    body, glow = G.Mesh(), G.Mesh()
    body.extend(G.cylinder(C, UP, 0.055, 0.19, seg))                                    # корпус
    for dy in (-0.075, 0.075):
        body.extend(G.torus(C + UP * dy, UP, 0.057, 0.007, seg, 6))                      # обручи
    for k in range(4):                                                                   # окна по кругу
        a = math.pi / 4 + k * math.pi / 2
        d = np.array([math.cos(a), 0.0, math.sin(a)])
        t = np.cross(UP, d)
        R = np.stack([t, UP, d], axis=1)                                                 # оси бруска: вдоль, вверх, наружу
        glow.extend(G.box(C + d * 0.053, (0.012, 0.05, 0.004), R))
    body.extend(G.cylinder(C + UP * 0.105, UP, 0.06, 0.025, seg))                        # крышка
    glow.extend(G.torus(C + UP * 0.119, UP, 0.045, 0.004, seg, 6))                       # кольцо на крышке
    body.extend(G.cylinder(C + UP * 0.125, UP, 0.02, 0.015, max(8, seg // 2)))           # колпачок
    body.extend(G.box(C + np.array([0.06, 0.05, 0.0]), (0.025, 0.025, 0.015)))           # скоба к поясу
    return G.fix_winding(body), G.fix_winding(glow)


def append(lod, mesh, tex, mat, selname):
    base = len(lod.points)
    nbase = len(lod.facenormals)
    sel = lod.new_selection(selname) if selname else None
    for p, n in zip(mesh.pts, mesh.nrm):
        pt = py3d.Point()
        pt.coords = tuple(float(x) for x in p)
        pt.flags = 0
        pt.mass = None
        lod.points.append(pt)
        lod.facenormals.append(tuple(float(x) for x in n))
        if sel is not None:
            sel.points[pt] = 1
    for f in mesh.faces:                     # MLOD: cross(b-a, c-a) по нормали (проверено на круге odol2mlod)
        fa = py3d.Face(lod.points, lod.facenormals)
        fa.flags = 0
        fa.texture, fa.material = tex, mat
        for i in f:
            vx = py3d.Vertex(lod.points, lod.facenormals)
            vx.point_index = base + i
            vx.normal_index = nbase + i
            vx.uv = (float(mesh.uv[i][0]), float(mesh.uv[i][1]))
            fa.vertices.append(vx)
        lod.faces.append(fa)
        if sel is not None:
            sel.faces[fa] = 1


def visual(res, seg):
    lod = py3d.LOD()
    lod.resolution = res
    body, glow = build(seg)
    append(lod, body, STEEL[0], STEEL[1], "vbbody")
    append(lod, glow, GLOW[0], GLOW[1], "vbglow")
    return lod


def hull(res, mass=None):
    lod = py3d.LOD()
    lod.resolution = res
    top = C[1] + 0.133
    bot = C[1] - 0.095
    m = G.fix_winding(G.cylinder(np.array([C[0], (top + bot) / 2, C[2]]), UP, 0.062, top - bot, 8))
    append(lod, m, "", "", "Component01")
    if mass:
        for pt in lod.points:
            pt.mass = mass / len(lod.points)
        lod.properties["autocenter"] = "0"
    return lod


def memory(points):
    P = np.array(points)
    lo, hi = P.min(0), P.max(0)
    c = (lo + hi) / 2
    r = float((hi - lo).max())
    lod = py3d.LOD()
    lod.resolution = 1e15
    for name, co in (("invview", (c[0] - r * 1.2, c[1] + r * 0.9, c[2] - r * 1.2)), ("ce_center", c),
                     ("ce_radius", (c[0] + r / 2, c[1], c[2])), ("boundingbox_min", lo), ("boundingbox_max", hi)):
        sel = lod.new_selection(name)
        pt = py3d.Point()
        pt.coords = tuple(float(x) for x in co)
        lod.points.append(pt)
        sel.points[pt] = 1
    return lod


def main():
    os.makedirs(OUTDIR, exist_ok=True)
    m = py3d.P3D()
    for res, seg in ((1.0, 32), (2.0, 16), (3.0, 10)):
        m.lods.append(visual(res, seg))
    m.lods.append(hull(1e13, MASS))
    m.lods.append(memory([p.coords for p in m.lods[0].points]))
    m.lods.append(hull(6e15))
    m.lods.append(hull(7e15))
    m.save(DST)
    print("MLOD ->", DST, "граней LOD1", len(m.lods[0].faces))
    with open(CFG, "w", encoding="utf-8") as f:
        f.write("// Капсула для артефактов экзо Эхо: собрано tools/ec_artcase_model.py\n"
                "class CfgModels\n{\n\tclass Default\n\t{\n\t\tsectionsInherit = \"\";\n\t\tsections[] = {};\n"
                "\t\tskeletonName = \"\";\n\t};\n\tclass vb_ec_artcase: Default\n\t{\n"
                "\t\tsections[] = {\"vbbody\", \"vbglow\"};\n\t};\n};\n")
    print("model.cfg ->", CFG)


if __name__ == "__main__":
    main()
