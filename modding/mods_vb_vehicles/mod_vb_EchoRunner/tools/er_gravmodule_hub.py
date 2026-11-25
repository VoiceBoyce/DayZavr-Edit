# -*- coding: utf-8 -*-
r"""Антиграв-модуль: корневая кость hub и ось hub_axis по оси колеса (Баги №61, 2026-11-25). Без Blender.

  python er_gravmodule_hub.py [путь_к_vb_er_gravmodule.p3d]   (по умолчанию — модуль в VB_EchoRunner/proxy)
  er_finalize.py вызывает patch() после копирования модуля в мод. Повторный запуск ничего не меняет.

Модуль стоит в слоте колеса и висит на кости колеса машины: после езды кость держит последний угол прокрутки, и
модуль висел повёрнутым. Компенсация — на самом модуле (свои источники машины ломали двери, E130): пустая кость
`hub` — родитель `pitch` (скелет в model.cfg, er_modelcfg_fix.py), ось `hub_axis` — две точки памяти на оси колеса
(у модуля начало координат = центр колеса, ось колеса = x модели). Анимация `unspin` крутит hub на минус угол колеса.
"""
import os
import sys

sys.path.insert(0, r"d:\DayZavr\DayZavr-Edit\.claude\tools\py3d")
import py3d  # noqa: E402

DEF = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "VB_EchoRunner", "proxy",
                   "vb_er_gravmodule.p3d")
AXIS = ((-0.29, 0.0, 0.0), (-0.09, 0.0, 0.0))      # как tilt_axis_x, но через центр колеса (y 0, z 0)


def _sel(lod, name):
    for k, v in lod.selections.items():
        if k.lower() == name.lower():
            return v
    return None


def patch(path=DEF):
    with open(path, "rb") as f:
        m = py3d.P3D(f)
    changed = 0
    for lod in m.lods:
        if _sel(lod, "pitch") is not None and _sel(lod, "hub") is None:
            lod.new_selection("hub")                  # пустая, как pitch
            changed += 1
        if abs(lod.resolution - 1e15) / 1e15 < 1e-3 and _sel(lod, "hub_axis") is None:
            s = lod.new_selection("hub_axis")
            for c in AXIS:
                p = py3d.Point()
                p.coords = c
                lod.points.append(p)
                s.points[p] = 1
            changed += 1
    if changed:
        m.save(path)                                  # атомарная запись с проверкой (py3d)
    return changed


if __name__ == "__main__":
    p = sys.argv[1] if len(sys.argv) > 1 else DEF
    n = patch(p)
    print("модуль:", p, "— изменений", n)
