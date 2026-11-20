# -*- coding: utf-8 -*-
r"""«Горящие» текстуры неона Echo Runner — для материала er_neon.rvmat (свечение 22, мотор заведён).

  python er_neon_variants.py

Почему отдельные (пользователь 11-20, скриншоты днём): у er_neon_co в красном есть слабая примесь зелёного и синего
(≈ 25 и 12 из 255). При свечении 22 красный канал упирается в потолок, а примесь растёт в 11 раз сильнее, чем
у тлеющего неона (свечение 2), и днём красный неон становится оранжево-розовым (замер: 240,88,88 → 240,160,120).
Чистый цвет без примеси при любой яркости остаётся своим цветом:
  er_neon_hot_co.paa         — красный: R как в er_neon_co, G = B = 0;
  er_neon_fusion_hot_co.paa  — бирюзовый: R = 0, G и B как в er_neon_fusion_co.
Тлеющий и выключенный неон по-прежнему берут er_neon_co / er_neon_fusion_co (там примесь даёт живой оттенок).
Исходник — work/texsrc/parts/er_neon_co.png (пишет er_tex_parts.py); бирюза — та же перекраска, что у
er_neon_fusion_co (R' = G, G' = 0.95 R, B' = 0.88 R).
"""
import os
import subprocess

from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
SRC = os.path.join(ROOT, "work", "texsrc", "parts", "er_neon_co.png")
PNG = os.path.join(ROOT, "work", "texsrc", "parts")
OUT = os.path.join(ROOT, "VB_EchoRunner", "data", "parts")
IMAGE_TO_PAA = r"D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\ImageToPAA\ImageToPAA.exe"


def to_paa(png, paa):
    if os.path.exists(paa):
        os.remove(paa)
    r = subprocess.run([IMAGE_TO_PAA, png, paa], capture_output=True, text=True)
    if not os.path.exists(paa):
        raise RuntimeError("ImageToPAA: %s %s" % (png, r.stdout[-300:] + r.stderr[-300:]))


def write(name, im):
    png = os.path.join(PNG, name + ".png")
    im.save(png)
    to_paa(png, os.path.join(OUT, name + ".paa"))
    print("OK", name, sorted(im.getcolors(1 << 20), reverse=True)[:3])


def main():
    r, g, b = Image.open(SRC).convert("RGB").split()
    zero = r.point(lambda v: 0)
    write("er_neon_hot_co", Image.merge("RGB", (r, zero, zero)))
    write("er_neon_fusion_hot_co", Image.merge("RGB", (zero, r.point(lambda v: round(v * 0.95)),
                                                         r.point(lambda v: round(v * 0.88)))))


if __name__ == "__main__":
    main()
