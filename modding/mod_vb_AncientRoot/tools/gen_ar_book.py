# -*- coding: utf-8 -*-
"""Окно «Книги древних» (ТЗ_сервер_18.md, ветка 2в, решения пользователя 2026-11-16): текстуры разворота.

  "G:\\SteamLibrary\\steamapps\\common\\Blender\\5.2\\python\\bin\\python.exe" gen_ar_book.py <папка_для_png>

Пишет в ../VB_AncientRoot/gui/book:
  vb_ar_book_cover_ca.paa  — переплёт из коры под разворотом: мох, корни с золотыми прожилками, янтарь по углам;
  vb_ar_book_page_ca.paa   — страница: старый пергамент, волокна, пятна, потемневший рваный край;
  vb_ar_book_spine_ca.paa  — тень сгиба посередине разворота;
  vb_ar_book_tab_ca.paa    — закладка-лента (белая маска, цвет даёт скрипт);
  vb_ar_book_arrow_ca.paa  — стрелка-лист «дальше» (белая маска; «назад» — та же, отражённая в разметке);
  vb_ar_book_runes_ca.paa  — руническая вязь закрытой страницы (белая маска);
  vb_ar_book_orn_ca.paa    — орнамент-разделитель под заголовком: стебель с листьями (белая маска);
  vb_ar_book_white_ca.paa  — белый квадрат (затемнение фона, цвет и прозрачность — скриптом).
"""
import math
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ar_theme as RT  # noqa: E402
from ar_texlib import write_png, png_to_paa, smoothstep  # noqa: E402

OUT_DIR = os.path.join(RT.mod_dir(), "gui", "book")
PAL = RT.theme()["palette"]


def tile_noise(h, w, rng, scales=(4, 9, 19, 41), amps=(0.5, 0.28, 0.15, 0.07)):
    """Плиточный шум суммой синусов разных частот (быстро и без швов внутри картинки)."""
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    out = np.zeros((h, w), np.float32)
    for s, a in zip(scales, amps):
        for _ in range(3):
            fx = rng.uniform(0.6, 1.4) * s / w
            fy = rng.uniform(0.6, 1.4) * s / h
            ph = rng.uniform(0, 6.28)
            ang = rng.uniform(0, math.pi)
            out += a / 3 * np.sin(2 * math.pi * (xx * fx * math.cos(ang) + yy * fy * math.sin(ang)) + ph)
    return out


def rounded_rect_alpha(h, w, r, pad):
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    x0, y0, x1, y1 = pad, pad, w - 1 - pad, h - 1 - pad
    cx = np.clip(xx, x0 + r, x1 - r)
    cy = np.clip(yy, y0 + r, y1 - r)
    d = np.sqrt((xx - cx) ** 2 + (yy - cy) ** 2)
    return 1 - smoothstep(r - 1.5, r + 1.5, d)


def save(tmp, name, img):
    h, w = img.shape[:2]
    if img.ndim == 2:
        rgba = np.ones((h, w, 4), np.float32)
        rgba[..., 3] = img
        img = rgba
    u8 = (np.clip(img, 0, 1) * 255 + 0.5).astype(np.uint8)
    png = os.path.join(tmp, name + ".png")
    write_png(png, u8)
    png_to_paa(png, os.path.join(OUT_DIR, name + ".paa"))
    print("TEX", name, w, h)


def cover():
    h, w = 512, 1024
    rng = np.random.default_rng(71)
    a = rounded_rect_alpha(h, w, 28, 6)
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    # кора: продольные борозды вдоль высоты книги + шум
    n = tile_noise(h, w, rng, (3, 7, 15, 33), (0.5, 0.3, 0.15, 0.08))
    ridge = np.sin(xx / w * 2 * math.pi * 26 + n * 2.2) * 0.5 + 0.5
    ridge = ridge ** 0.7
    bark = np.asarray(PAL["p_bark"], np.float32) * 0.85
    rgb = bark[None, None, :] * (0.55 + 0.45 * ridge[..., None] + 0.12 * n[..., None])
    # мох пятнами, больше по краям
    edge = np.minimum(np.minimum(xx, w - 1 - xx), np.minimum(yy, h - 1 - yy))
    edge_k = np.clip(1 - edge / 70, 0, 1)
    mn = tile_noise(h, w, np.random.default_rng(72), (5, 11, 23), (0.6, 0.3, 0.15))
    moss = np.clip((mn - 0.25 + 0.3 * edge_k) * 1.8, 0, 1) * (1 - ridge * 0.5) * 0.8
    moss_c = np.asarray(PAL["p_bark_moss"], np.float32) * 0.95
    rgb = rgb * (1 - moss[..., None]) + moss_c[None, None, :] * moss[..., None]
    # затемнение к краю
    rgb *= (0.75 + 0.25 * np.clip(edge / 40, 0, 1))[..., None]
    # золотые прожилки корней по краю
    vn = tile_noise(h, w, np.random.default_rng(73), (6, 14), (0.7, 0.3))
    vein = (1 - smoothstep(0.015, 0.05, np.abs(vn))) * edge_k ** 1.5
    gold = np.array([1.0, 0.74, 0.22], np.float32)
    rgb = rgb * (1 - vein[..., None] * 0.85) + gold[None, None, :] * vein[..., None] * 0.85
    # янтарные заклёпки по углам
    for cx, cy in ((22, 22), (w - 23, 22), (22, h - 23), (w - 23, h - 23)):     # у самого края: страницы их не закрывают
        r = np.sqrt((xx - cx) ** 2 + (yy - cy) ** 2)
        core = 1 - smoothstep(8, 10, r)
        halo = np.clip(1 - r / 22, 0, 1) ** 2 * 0.6
        shade = np.clip(1 - (xx - cx + yy - cy) / 24, 0.4, 1.3)
        amber = np.array([0.95, 0.58, 0.12], np.float32)
        rgb = rgb * (1 - halo[..., None]) + gold[None, None, :] * halo[..., None]
        rgb = rgb * (1 - core[..., None]) + (amber[None, None, :] * shade[..., None]) * core[..., None]
    out = np.dstack([np.clip(rgb, 0, 1), a])
    return out


def page():
    h, w = 512, 512
    rng = np.random.default_rng(81)
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    base = np.array([0.86, 0.77, 0.58], np.float32)
    n = tile_noise(h, w, rng, (3, 8, 21, 55), (0.35, 0.22, 0.12, 0.06))
    fib = np.sin(yy / h * 2 * math.pi * 120 + n * 4) * 0.5 + 0.5      # волокна поперёк
    rgb = base[None, None, :] * (0.93 + 0.08 * n[..., None] + 0.025 * fib[..., None])
    # пятна
    st = tile_noise(h, w, np.random.default_rng(82), (2, 5, 9), (0.6, 0.3, 0.2))
    stain = np.clip((st - 0.35) * 1.6, 0, 1) * 0.18
    rgb *= (1 - stain[..., None])
    # потемневший край и рваная кромка
    edge = np.minimum(np.minimum(xx, w - 1 - xx), np.minimum(yy, h - 1 - yy))
    en = tile_noise(h, w, np.random.default_rng(83), (11, 27, 61), (0.5, 0.3, 0.2))
    dist = edge + en * 5
    burn = np.clip(1 - dist / 46, 0, 1) ** 1.6
    rgb *= (1 - 0.5 * burn[..., None])
    rgb += np.array([-0.05, -0.08, -0.1], np.float32)[None, None, :] * burn[..., None]
    a = smoothstep(1.0, 4.0, dist)
    return np.dstack([np.clip(rgb, 0, 1), a])


def spine():
    h, w = 512, 64
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    u = (xx - (w - 1) / 2) / ((w - 1) / 2)          # -1..1, 0 — сгиб
    a = (1 - np.abs(u)) ** 1.8 * 0.55
    a *= 1 - 0.15 * np.abs((yy - h / 2) / (h / 2))
    rgb = np.zeros((h, w, 3), np.float32) + np.array([0.12, 0.08, 0.04], np.float32)
    return np.dstack([rgb, a])


def tab():
    h, w = 64, 256
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    # лента: прямоугольник со скруглёнными углами слева и «ласточкиным хвостом» справа
    a = rounded_rect_alpha(h, w, 10, 4)
    notch = (xx - (w - 30)) > np.abs(yy - h / 2) * 0.9
    a = a * (1 - notch.astype(np.float32))
    # лёгкая фактура ткани, чтобы маска не была плоской (скрипт красит SetColor)
    n = np.sin(xx / w * 2 * math.pi * 60) * 0.5 + 0.5
    shade = 0.82 + 0.18 * (1 - np.abs(yy - h / 2) / (h / 2)) + 0.04 * n
    rgb = np.repeat(np.clip(shade, 0, 1)[..., None], 3, 2)
    return np.dstack([rgb, a])


def arrow():
    h, w = 64, 128
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    u = xx / (w - 1)
    v = (yy - (h - 1) / 2) / ((h - 1) / 2)
    # лист: ширина синусом вдоль, острый кончик справа; черешок-стрелка
    width = np.clip(np.sin(np.clip(u, 0, 1) * math.pi), 0, 1) ** 0.8 * (1 - u * 0.25)   # sin(pi) < 0 -> NaN без clip
    leaf = 1 - smoothstep(width * 0.9 - 0.06, width * 0.9 + 0.04, np.abs(v))
    rib = (1 - smoothstep(0.02, 0.07, np.abs(v))) * (u > 0.05) * (u < 0.95)
    a = np.clip(leaf, 0, 1)
    shade = 0.85 + 0.15 * (1 - np.abs(v)) - 0.25 * rib
    rgb = np.repeat(np.clip(shade, 0, 1)[..., None], 3, 2)
    return np.dstack([rgb, a])


def runes():
    h, w = 256, 512
    rng = np.random.default_rng(91)
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    a = np.zeros((h, w), np.float32)

    def seg(ax, ay, bx, by, wdt):
        vx, vy = bx - ax, by - ay
        L2 = vx * vx + vy * vy + 1e-6
        t = np.clip(((xx - ax) * vx + (yy - ay) * vy) / L2, 0, 1)
        d = np.sqrt((xx - ax - t * vx) ** 2 + (yy - ay - t * vy) ** 2)
        return 1 - smoothstep(wdt - 0.7, wdt + 0.7, d)

    rows = 6
    for r in range(rows):
        y0 = 22 + r * 40
        x = 14 + rng.uniform(0, 14)
        while x < w - 30:
            gw = rng.uniform(12, 20)
            gh = rng.uniform(22, 30)
            # ствол и 1–3 ветви (руны в духе футарка)
            a = np.maximum(a, seg(x + gw / 2, y0, x + gw / 2, y0 + gh, 1.4))
            for _ in range(rng.integers(1, 4)):
                yb = y0 + rng.uniform(0, gh * 0.7)
                side = rng.choice([-1, 1])
                a = np.maximum(a, seg(x + gw / 2, yb, x + gw / 2 + side * gw / 2, yb + rng.uniform(-8, 10), 1.3))
            x += gw + rng.uniform(5, 11)
            if rng.uniform() < 0.12:
                x += 18                           # пробел между «словами»
    return a


def ornament():
    h, w = 32, 512
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    u = xx / (w - 1)
    cy = h / 2 + 3 * np.sin(u * 2 * math.pi * 3)
    fade = np.clip(np.minimum(u, 1 - u) / 0.12, 0, 1)
    stem = (1 - smoothstep(0.8, 1.6, np.abs(yy - cy))) * fade
    a = stem
    # листья парами
    for k in range(13):
        uk = 0.08 + 0.84 * k / 12
        x0 = uk * (w - 1)
        y0 = h / 2 + 3 * math.sin(uk * 2 * math.pi * 3)
        for side in (-1, 1):
            ang = side * 0.9
            ca, sa = math.cos(ang), math.sin(ang)
            dx, dy = xx - (x0 + 4 * ca), yy - (y0 - side * 4 + 0 * sa)
            pa = (dx * ca + dy * sa) / 6
            pb = (-dx * sa + dy * ca) / 2.4
            leaf = 1 - smoothstep(0.7, 1.0, pa * pa + pb * pb)
            a = np.maximum(a, leaf * (min(uk, 1 - uk) / 0.12 if min(uk, 1 - uk) < 0.12 else 1))
    # ромб в центре
    d = np.abs(xx - (w - 1) / 2) / 7 + np.abs(yy - h / 2) / 7
    a = np.maximum(a, 1 - smoothstep(0.85, 1.0, d))
    return a


def white():
    return np.ones((8, 8), np.float32)


def main():
    tmp = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "_book")
    os.makedirs(tmp, exist_ok=True)
    os.makedirs(OUT_DIR, exist_ok=True)
    save(tmp, "vb_ar_book_cover_ca", cover())
    save(tmp, "vb_ar_book_page_ca", page())
    save(tmp, "vb_ar_book_spine_ca", spine())
    save(tmp, "vb_ar_book_tab_ca", tab())
    save(tmp, "vb_ar_book_arrow_ca", arrow())
    save(tmp, "vb_ar_book_runes_ca", runes())
    save(tmp, "vb_ar_book_orn_ca", ornament())
    save(tmp, "vb_ar_book_white_ca", white())
    print("готово:", OUT_DIR)


if __name__ == "__main__":
    main()
