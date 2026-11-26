# -*- coding: utf-8 -*-
r"""Ракурс превью предмета в инвентаре (2026-11-26): точки Memory LOD boundingbox_min / boundingbox_max / invview.

ItemPreviewWidget.SetView(0) ставит камеру в invview и смотрит на ящик boundingbox_min..max
(P:\scripts\3_game\gameplay.c:292-298); без этих точек камера смотрит горизонтально с +Z. Бак Echo Runner лежит вдоль
оси Z — без точек в «Поблизости» и инвентаре был виден только торец (тёмный круг). Здесь камера ставится сбоку от
длинной оси, чуть сверху, на ~2 диагонали ящика (как ванильный Nails_loose_10: 1,95 диагонали).

  python er_invview.py <model.p3d> [...]      (Python из Blender: нужен numpy; повторный запуск переставляет точки)
Вызывается из er_finalize.py для бака (vb_er_tank.p3d).
"""
import math
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..", "..", "..", ".claude", "tools", "py3d")))
import py3d  # noqa: E402

MEM_RES = 1e15


def set_side_view(m, elev_deg=15.0, dist_k=1.95, side=-1.0):
    """Камера сбоку от самой длинной горизонтальной оси модели (X или Z), на elev_deg выше центра ящика."""
    vis = [l for l in m.lods if l.resolution < 1e3]
    pts = np.array([p.coords for p in vis[0].points])
    lo, hi = pts.min(0), pts.max(0)
    c = (lo + hi) / 2
    size = hi - lo
    # смотрим поперёк длинной оси: длинная Z — камера по X, длинная X — по Z
    hdir = np.array((side, 0.0, 0.0)) if size[2] >= size[0] else np.array((0.0, 0.0, side))
    dist = dist_k * float(np.linalg.norm(size))
    e = math.radians(elev_deg)
    inv = c + hdir * dist * math.cos(e) + np.array((0.0, dist * math.sin(e), 0.0))
    mem = next((l for l in m.lods if abs(l.resolution / MEM_RES - 1) < 1e-3), None)
    if mem is None:
        mem = py3d.LOD()
        mem.resolution = MEM_RES
        m.lods.append(mem)
        m.lods.sort(key=lambda x: x.resolution)
    for name, co in (("boundingbox_min", lo), ("boundingbox_max", hi), ("invview", inv)):
        sel = mem.selections.get(name)
        if sel and sel.points:
            for p in sel.points:
                p.coords = tuple(float(x) for x in co)
            continue
        p = py3d.Point()
        p.coords = tuple(float(x) for x in co)
        mem.points.append(p)
        mem.new_selection(name).points[p] = 1
    return tuple(float(x) for x in inv)


def main():
    for path in sys.argv[1:]:
        with open(path, "rb") as f:
            m = py3d.P3D(f)
        inv = set_side_view(m)
        m.save(path)
        print(os.path.basename(path), "invview", [round(x, 3) for x in inv])


if __name__ == "__main__":
    main()
