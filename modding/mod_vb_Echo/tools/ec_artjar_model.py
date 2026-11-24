# -*- coding: utf-8 -*-
r"""Модели поясов с баночками для артефактов экзо Эхо (VB_EchoClothes, ТЗ раздел 8.4, концепт одобрен 11-24).

  python ec_artjar_model.py          (Python из Blender 5.3: нужен numpy)

Пишет VB_EchoClothes/exo_set/artjar/vb_ec_artjar_{s,m,l}.p3d (MLOD) + model.cfg; бинаризует pbo.ps1 -Binarize.
Форма — ec_artjar_geom.py (тот же код рисует концепт ec_concept.py --jars). Пояс висит в гнезде vb_ec_artcase_box
сервопривода (кость pelvis); модель — в осях этого прокси, на земле лежит той же моделью.

LOD 1/2/3 (сталь — vbbody, стекло, кольца баночек — vbjar1..N: скрипт зажигает кольцо занятой баночки), Geometry
(коробка по габариту, масса), View/Fire Geometry, Memory: invview, ce_*, габарит и точки уменьшенных копий артефактов
vbart<N> (центр баночки), vbart<N>u (на 5 см вверх), vbart<N>f (на 5 см наружу) — по ним скрипт ставит копию (E126).
"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
EDIT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(EDIT, ".claude", "tools", "py3d"))
sys.path.insert(0, HERE)
import py3d  # noqa: E402
import ec_geom as G  # noqa: E402
import ec_artjar_geom as AJ  # noqa: E402

BS = chr(92)
OUTDIR = os.path.join(HERE, "..", "VB_EchoClothes", "exo_set", "artjar")
CFG = os.path.join(OUTDIR, "model.cfg")
DATA = BS.join(["vb_echoclothes", "exo_set", "backpacks", "exoprivod", "data", ""])
STEEL = (DATA + "vb_ec_steel_dark_co.paa", DATA + "vb_ec_steel_dark.rvmat")
GLOW = (DATA + "vb_ec_neon_empty_co.paa", DATA + "vb_ec_neon_off.rvmat")
GLASS = (BS.join(["vb_echocore", "data", "er_glass_ca.paa"]), BS.join(["vb_echocore", "data", "er_glass.rvmat"]))
NAMES = {"small": "vb_ec_artjar_s", "medium": "vb_ec_artjar_m", "large": "vb_ec_artjar_l"}
MASS = {"small": 0.3, "medium": 0.55, "large": 0.8}
UP = np.array([0.0, 1.0, 0.0])


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
    # Порядок вершин MLOD — ОБРАТНЫЙ мешу ec_geom (там cross(b-a, c-a) наружу): иначе после бинаризации грани смотрят
    # внутрь и игра рисует изнанку — крышки баночек «пропадали», всё было видно насквозь (стенд 11-24; капсула 11-23 с
    # прямым порядком тоже была вывернута). Проверка — по геометрии собранного ODOL, не по нормалям (odol_read
    # декодирует их с обратным знаком): у ванильных банок cross(b-a, c-a) смотрит наружу.
    for f in mesh.faces:
        fa = py3d.Face(lod.points, lod.facenormals)
        fa.flags = 0
        fa.texture, fa.material = tex, mat
        for i in f[::-1]:
            vx = py3d.Vertex(lod.points, lod.facenormals)
            vx.point_index = base + i
            vx.normal_index = nbase + i
            vx.uv = (float(mesh.uv[i][0]), float(mesh.uv[i][1]))
            fa.vertices.append(vx)
        lod.faces.append(fa)
        if sel is not None:
            sel.faces[fa] = 1


def visual(kind, res, seg):
    b = AJ.build(kind, seg)
    lod = py3d.LOD()
    lod.resolution = res
    append(lod, AJ.to_local_mesh(b["steel"]), STEEL[0], STEEL[1], "vbbody")
    append(lod, AJ.to_local_mesh(b["glass"]), GLASS[0], GLASS[1], "vbglass")
    for i, rm in enumerate(b["rings"]):
        append(lod, AJ.to_local_mesh(rm), GLOW[0], GLOW[1], "vbjar%d" % (i + 1))
    return lod, b


def hull(pts_local, res, mass=None):
    """Коробка по габариту в осях пояса (у среднего и большого — дуга вокруг тела, коробка с запасом)."""
    P = np.array(pts_local)
    lo, hi = P.min(0), P.max(0)
    lod = py3d.LOD()
    lod.resolution = res
    m = G.fix_winding(G.box((lo + hi) / 2, tuple((hi - lo) / 2)))
    append(lod, m, "", "", "Component01")
    if mass:
        for pt in lod.points:
            pt.mass = mass / len(lod.points)
        lod.properties["autocenter"] = "0"
    return lod


def memory(pts_local, b):
    P = np.array(pts_local)
    lo, hi = P.min(0), P.max(0)
    c = (lo + hi) / 2
    r = float((hi - lo).max())
    lod = py3d.LOD()
    lod.resolution = 1e15
    named = [("invview", (c[0] - r * 1.2, c[1] + r * 0.9, c[2] - r * 1.2)), ("ce_center", c),
             ("ce_radius", (c[0] + r / 2, c[1], c[2])), ("boundingbox_min", lo), ("boundingbox_max", hi)]
    for i, (ac, j) in enumerate(zip(b["art"], b["layout"]["jars"])):
        k = i + 1
        named.append(("vbart%d" % k, AJ.to_local_point(ac)))
        named.append(("vbart%du" % k, AJ.to_local_point(ac + UP * 0.05)))
        named.append(("vbart%df" % k, AJ.to_local_point(ac + j["out"] * 0.05)))
    for name, co in named:
        sel = lod.new_selection(name)
        pt = py3d.Point()
        pt.coords = tuple(float(x) for x in co)
        lod.points.append(pt)
        sel.points[pt] = 1
    return lod


def build_model(kind):
    m = py3d.P3D()
    b = None
    for res, seg in ((1.0, 24), (2.0, 14), (3.0, 8)):
        lod, bb = visual(kind, res, seg)
        m.lods.append(lod)
        b = b or bb
    pts = [p.coords for p in m.lods[0].points]
    m.lods.append(hull(pts, 1e13, MASS[kind]))
    m.lods.append(memory(pts, b))
    m.lods.append(hull(pts, 6e15))
    m.lods.append(hull(pts, 7e15))
    dst = os.path.join(OUTDIR, NAMES[kind] + ".p3d")
    m.save(dst)
    P = np.array(pts)
    print("%s: баночек %d, граней LOD1 %d, габарит %s см -> %s" % (
        NAMES[kind], len(b["art"]), len(m.lods[0].faces), np.round((P.max(0) - P.min(0)) * 100, 1).tolist(), dst))


def write_cfg():
    L = ["// Пояса с баночками для артефактов экзо Эхо: собрано tools/ec_artjar_model.py",
         "class CfgModels\n{\n\tclass Default\n\t{\n\t\tsectionsInherit = \"\";\n\t\tsections[] = {};"
         "\n\t\tskeletonName = \"\";\n\t};"]
    for kind, name in NAMES.items():
        secs = ", ".join("\"vbjar%d\"" % (i + 1) for i in range(AJ.KINDS[kind]))
        L.append("\tclass %s: Default\n\t{\n\t\tsections[] = {%s};\n\t};" % (name, secs))
    L.append("};")
    with open(CFG, "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")
    print("model.cfg ->", CFG)


def main():
    os.makedirs(OUTDIR, exist_ok=True)
    for kind in AJ.KINDS:
        build_model(kind)
    write_cfg()


if __name__ == "__main__":
    main()
