# -*- coding: utf-8 -*-
r"""Модели-заглушки прокси станции v2 (как ванильные dz\gear\cooking\proxy\direct_cooking_*.p3d): маленький ящик,
сам не рисуется — на месте прокси движок рисует вложенный предмет. Имена — eb_models.station_proxies().

  python eb_proxies.py
Выход: VB_EchoBase/proxy/vb_eb_st_<tank1|tank2|battery|cell_c1|cell_c2|cell_d1|cell_d2>.p3d
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..", "..", ".claude", "tools", "py3d")))
import py3d  # noqa: E402

NAMES = ["vb_eb_st_tank1", "vb_eb_st_tank2", "vb_eb_st_battery",
         "vb_eb_st_cell_c1", "vb_eb_st_cell_c2", "vb_eb_st_cell_d1", "vb_eb_st_cell_d2"]
OUT = os.path.abspath(os.path.join(HERE, "..", "VB_EchoBase", "proxy"))
H = 0.02
CORNERS = [(x, y, z) for x in (-H, H) for y in (0.0, 2 * H) for z in (-H, H)]
QUADS = [(0, 1, 3, 2), (4, 6, 7, 5), (0, 4, 5, 1), (2, 3, 7, 6), (0, 2, 6, 4), (1, 5, 7, 3)]


def box_lod(res):
    lod = py3d.LOD()
    lod.resolution = res
    for c in CORNERS:
        p = py3d.Point()
        p.coords = c
        lod.points.append(p)
    lod.facenormals.append((0.0, 1.0, 0.0))
    for q in QUADS:
        fa = py3d.Face(lod.points, lod.facenormals)
        fa.flags = 0
        fa.texture = ""
        fa.material = ""
        for i in q:
            v = py3d.Vertex(lod.points, lod.facenormals)
            v.point_index = i
            v.normal_index = 0
            v.uv = (0.0, 0.0)
            fa.vertices.append(v)
        lod.faces.append(fa)
    return lod


def main():
    os.makedirs(OUT, exist_ok=True)
    for n in NAMES:
        m = py3d.P3D()
        m.lods.append(box_lod(1.0))
        m.save(os.path.join(OUT, n + ".p3d"))
    print("OK", len(NAMES), "->", OUT)


if __name__ == "__main__":
    main()
