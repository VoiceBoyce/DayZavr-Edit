# -*- coding: utf-8 -*-
r"""Убрать из модели машины ванильные прокси-«заглушки» ALEX (просьба пользователя 2026-11-26):
  \dz\data\proxies\spawnweapon        — светлый силуэт автомата под крышей салона;
  \dz\data\proxies\spawnitembackpack  — то же для рюкзака.
Ни в ванили, ни у нас для них нет класса CfgNonAIVehicles, поэтому к слотам Shoulder/Back они не привязаны (вещь в
слоте по ним не рисуется), а движок рисует саму модель-заглушку. Слоты Shoulder/Back в машине остаются (вещь лежит в
слоте, просто не видна на модели — как было и раньше).

  python er_drop_placeholders.py <model.p3d>        (Python из Блендера; повторный запуск ничего не меняет)
Вызывается из er_finalize.py.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..", "..", "..", ".claude", "tools", "py3d")))
import py3d  # noqa: E402

PLACEHOLDERS = ("\\dz\\data\\proxies\\spawnweapon", "\\dz\\data\\proxies\\spawnitembackpack")


def drop(m):
    """Удалить заглушки во всех LOD. Вернёт число удалённых прокси."""
    n = 0
    for lod in m.lods:
        for name in [s for s in lod.selections if s.lower().startswith("proxy:")]:
            path = name[6:].rsplit(".", 1)[0].lower()
            if path in PLACEHOLDERS:
                lod.remove_proxy(name)
                n += 1
    return n


def main():
    for path in sys.argv[1:]:
        with open(path, "rb") as f:
            m = py3d.P3D(f)
        n = drop(m)
        if n:
            m.save(path)
        print(os.path.basename(path), "убрано заглушек:", n)


if __name__ == "__main__":
    main()
