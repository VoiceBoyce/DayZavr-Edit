# -*- coding: utf-8 -*-
"""Знак расположения леса в худе (ТЗ 2б.11): белые маски с альфой, цвет даёт скрипт (SetColor).

  "G:\\SteamLibrary\\steamapps\\common\\Blender\\5.2\\python\\bin\\python.exe" gen_ar_hud.py <папка_для_png>

  vb_ar_disc_ca.paa  — подложка: круг с мягким краем (скрипт красит в тёмный полупрозрачный);
  vb_ar_rune_ca.paa  — кольцо и руна Альгиз (ᛉ, «защита леса») внутри;
  vb_ar_glow_ca.paa  — мягкое сияние вокруг (только у «Друга леса»).
Пишет в ../VB_AncientRoot/gui. Белые маски + SetColor — цвет не зависит от гаммы картинки (dayz-ui-development).
"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ar_theme as RT  # noqa: E402
from ar_texlib import write_png, png_to_paa, smoothstep  # noqa: E402

OUT_DIR = os.path.join(RT.mod_dir(), "gui")
N = 128


def seg_dist(px, py, ax, ay, bx, by):
    """Расстояние от точек (px, py) до отрезка AB."""
    vx, vy = bx - ax, by - ay
    t = np.clip(((px - ax) * vx + (py - ay) * vy) / (vx * vx + vy * vy), 0, 1)
    dx, dy = px - (ax + t * vx), py - (ay + t * vy)
    return np.sqrt(dx * dx + dy * dy)


def grid(n):
    y, x = np.mgrid[0:n, 0:n].astype(np.float32)
    c = (n - 1) / 2.0
    return (x - c) / c, (y - c) / c        # -1..1, y вниз


def disc():
    x, y = grid(N)
    r = np.sqrt(x * x + y * y)
    return 1 - smoothstep(0.86, 0.96, r)


def rune():
    x, y = grid(N)
    r = np.sqrt(x * x + y * y)
    ring = 1 - smoothstep(0.055, 0.09, np.abs(r - 0.80))
    w = 0.075
    # Альгиз: ствол и две ветви вверх-в-стороны
    d = seg_dist(x, y, 0.0, 0.52, 0.0, -0.55)
    d = np.minimum(d, seg_dist(x, y, 0.0, -0.02, -0.40, -0.48))
    d = np.minimum(d, seg_dist(x, y, 0.0, -0.02, 0.40, -0.48))
    glyph = 1 - smoothstep(w, w + 0.035, d)
    return np.maximum(ring, glyph)


def glow():
    n = 2 * N
    x, y = grid(n)
    r = np.sqrt(x * x + y * y)
    a = np.clip(1 - r, 0, 1) ** 2.2
    a *= 1 - smoothstep(0.97, 1.0, r)
    return a


def save(tmp, name, alpha):
    h, w = alpha.shape
    rgba = np.zeros((h, w, 4), np.uint8)
    rgba[..., :3] = 255
    rgba[..., 3] = (np.clip(alpha, 0, 1) * 255 + 0.5).astype(np.uint8)
    png = os.path.join(tmp, name + ".png")
    write_png(png, rgba)
    png_to_paa(png, os.path.join(OUT_DIR, name + ".paa"))
    print("TEX", name, w, h)


def main():
    tmp = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "_hud")
    os.makedirs(tmp, exist_ok=True)
    os.makedirs(OUT_DIR, exist_ok=True)
    save(tmp, "vb_ar_disc_ca", disc())
    save(tmp, "vb_ar_rune_ca", rune())
    save(tmp, "vb_ar_glow_ca", glow())
    print("готово:", OUT_DIR)


if __name__ == "__main__":
    main()
