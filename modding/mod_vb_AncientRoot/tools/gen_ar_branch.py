# -*- coding: utf-8 -*-
"""Полоса расположения леса «живая ветвь» под быстрыми слотами (выбор пользователя 2026-11-16: стиль 4 на месте 1).

  "G:\\SteamLibrary\\steamapps\\common\\Blender\\5.2\\python\\bin\\python.exe" gen_ar_branch.py <папка_для_png>

Шкала −100..+100 слева направо, ноль посередине. Текстуры 512×64, цветные (кроме почки):
  vb_ar_branch_ca.paa     — ветка во всю ширину, на границах ступеней (−60, −20, 0, +20, +60) вырезаны насечки;
  vb_ar_bloom_ca.paa      — правая половина: листья и золотые почки (гуще к правому краю);
  vb_ar_thorn_ca.paa      — левая половина: обугленная кора, шипы и тлеющие трещины (гуще к левому краю);
  vb_ar_mask_right_ca.paa — маска для bloom: альфа от 0 в центре до 1 справа, слева 1 (SetMaskProgress = плюс/100);
  vb_ar_mask_left_ca.paa  — маска для thorn: альфа от 0 в центре до 1 слева, справа 1 (SetMaskProgress = минус/100);
  vb_ar_bud_ca.paa        — почка-маркер на текущем значении: белая маска с ореолом, цвет даёт скрипт.
Пишет в ../VB_AncientRoot/gui.
"""
import math
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ar_theme as RT  # noqa: E402
from ar_texlib import write_png, png_to_paa, smoothstep  # noqa: E402

OUT_DIR = os.path.join(RT.mod_dir(), "gui")
W, H = 512, 64          # степени двойки (ImageToPAA)
TICKS = [0.2, 0.4, 0.6, 0.8]       # −60, −20, +20, +60
CENTER = 0.5

yy, xx = np.mgrid[0:H, 0:W].astype(np.float32)
U = xx / (W - 1)
V = yy / (H - 1)


def center_y(u):
    return 0.5 + 0.06 * np.sin(u * 2 * math.pi * 2.3 + 0.7) + 0.02 * np.sin(u * 2 * math.pi * 7.1)


def half_width(u):
    t = 0.085 + 0.015 * np.sin(u * 2 * math.pi * 5.3)
    for uk in (0.07, 0.31, 0.47, 0.66, 0.9):
        t = t + 0.035 * np.exp(-((u - uk) / 0.012) ** 2)      # узлы
    return t


CY = center_y(U)
HW = half_width(U)
DV = (V - CY)                       # смещение от оси ветки, в долях высоты
DPX = np.abs(DV) * H


def branch_alpha(extra=0.0):
    t = (HW + extra) * H
    return 1 - smoothstep(t - 1.0, t + 0.8, DPX)


def noise1(u, seed, freqs=(37, 83, 151)):
    rng = np.random.default_rng(seed)
    out = np.zeros_like(u)
    for f in freqs:
        out += np.sin(u * 2 * math.pi * f + rng.uniform(0, 6.28)) / len(freqs)
    return out


def rgba(rgb, a):
    out = np.zeros((H, W, 4), np.float32)
    out[..., :3] = np.clip(rgb, 0, 1)
    out[..., 3] = np.clip(a, 0, 1)
    return out


def over(dst, src):
    """Наложение src поверх dst (обе RGBA 0..1)."""
    a = src[..., 3:4]
    out = dst.copy()
    out[..., :3] = src[..., :3] * a + dst[..., :3] * (1 - a)
    out[..., 3:4] = a + dst[..., 3:4] * (1 - a)
    return out


def branch():
    a = branch_alpha()
    shade = 1 - 0.45 * np.clip(DV / HW, -1, 1)          # свет сверху: верх светлее
    streak = 0.08 * noise1(U * 1.0 + V * 0.02, 5)
    bark = np.asarray(PAL["p_bark"], np.float32)
    rgb = bark[None, None, :] * (shade + streak)[..., None] * 1.15
    moss_n = noise1(U, 11, (13, 29, 61))
    moss = np.clip((moss_n - 0.25) * 2.5, 0, 1) * np.clip(-DV / HW, 0, 1)   # мох по верху
    moss_c = np.asarray(PAL["p_bark_moss"], np.float32) * 1.4
    rgb = rgb * (1 - moss[..., None]) + moss_c[None, None, :] * moss[..., None]
    img = rgba(rgb, a)
    # насечки ступеней и метка нуля: светлая резьба поперёк ветки
    for uk, big in [(u, False) for u in TICKS] + [(CENTER, True)]:
        w = 1.5 if big else 1.0
        reach = (HW + (0.12 if big else 0.06)) * H
        m = (1 - smoothstep(w - 0.6, w + 0.6, np.abs(U - uk) * W)) * (1 - smoothstep(reach - 1, reach + 1, DPX))
        c = np.array([0.92, 0.80, 0.52], np.float32)
        img = over(img, rgba(np.broadcast_to(c, (H, W, 3)), m * (0.9 if big else 0.7)))
    return img


def ellipse(cx, cy, ang, length, width):
    """Маска листа: эллипс с центром (cx, cy) в пикселях, длина вдоль ang."""
    ca, sa = math.cos(ang), math.sin(ang)
    dx, dy = xx - cx, yy - cy
    a = (dx * ca + dy * sa) / (length / 2)
    b = (-dx * sa + dy * ca) / (width / 2)
    r = a * a + b * b
    return 1 - smoothstep(0.75, 1.0, r), a


def bloom():
    rng = np.random.default_rng(31)
    img = rgba(np.zeros((H, W, 3)), np.zeros((H, W)))
    n = 26
    for k in range(n):
        # гуще к правому краю
        u = 0.52 + 0.46 * (1 - (1 - (k + rng.uniform(0, 0.8)) / n) ** 1.6)
        u = min(u, 0.985)
        side = -1 if k % 2 == 0 else 1
        cyp = float(center_y(np.float32(u))) * (H - 1)
        hw = float(half_width(np.float32(u))) * H
        ang = side * math.radians(rng.uniform(35, 65)) - math.pi / 2 * side * 0 + (math.pi / 2 if side > 0 else -math.pi / 2)
        ang = (math.radians(-50) if side < 0 else math.radians(50)) + rng.uniform(-0.25, 0.25)
        length = rng.uniform(9, 14) * (0.8 + 0.4 * (u - 0.5) / 0.5)
        cxp = u * (W - 1) + math.cos(ang) * length * 0.45
        cypp = cyp + side * hw * 0.8 + math.sin(ang) * length * 0.45
        m, along = ellipse(cxp, cypp, ang, length, length * 0.42)
        g = np.array([0.24, 0.44, 0.11], np.float32) * rng.uniform(0.85, 1.25)
        rib = 1 - 0.35 * (1 - smoothstep(0.0, 0.12, np.abs((-(xx - cxp) * math.sin(ang) + (yy - cypp) * math.cos(ang)))))
        rgb = g[None, None, :] * rib[..., None] * (0.85 + 0.25 * np.clip(along, -1, 1)[..., None])
        img = over(img, rgba(rgb, m))
    # золотые почки на ветке
    for k in range(11):
        u = 0.53 + 0.45 * (1 - (1 - (k + 0.5) / 11) ** 1.4)
        cxp = u * (W - 1)
        cyp = float(center_y(np.float32(u))) * (H - 1) + rng.uniform(-2, 2)
        r = np.sqrt((xx - cxp) ** 2 + (yy - cyp) ** 2)
        rad = rng.uniform(2.0, 3.2)
        core = 1 - smoothstep(rad - 0.6, rad + 0.6, r)
        halo = np.clip(1 - r / (rad * 3.2), 0, 1) ** 2 * 0.55
        gold = np.array([1.0, 0.82, 0.30], np.float32)
        img = over(img, rgba(np.broadcast_to(gold * 0.9, (H, W, 3)), halo))
        img = over(img, rgba(np.broadcast_to(np.array([1.0, 0.93, 0.6], np.float32), (H, W, 3)), core))
    img[..., 3] *= (U >= 0.5)        # только правая половина
    return img


def thorn():
    rng = np.random.default_rng(47)
    # обугленная кора поверх ветки
    a = branch_alpha(0.01) * 0.88
    char = np.array([0.07, 0.05, 0.045], np.float32)
    img = rgba(np.broadcast_to(char, (H, W, 3)), a)
    # тлеющие трещины вдоль ветки
    # волнистые линии вдоль ветки, кусками
    wave = DV / HW - 0.4 * np.sin(U * 2 * math.pi * 11 + 0.8) - 0.15 * np.sin(U * 2 * math.pi * 29)
    seg = np.clip((noise1(U, 53, (17, 31)) + 0.05) * 4, 0, 1)
    crack = (1 - smoothstep(0.06, 0.16, np.abs(wave))) * seg * branch_alpha()
    ember = np.array([0.95, 0.30, 0.08], np.float32)
    img = over(img, rgba(np.broadcast_to(ember, (H, W, 3)), crack * 0.9))
    # шипы: треугольники от края ветки наружу, гуще к левому краю
    n = 24
    for k in range(n):
        u = 0.48 - 0.46 * (1 - (1 - (k + rng.uniform(0, 0.8)) / n) ** 1.6)
        u = max(u, 0.015)
        side = -1 if k % 2 == 0 else 1
        cyp = float(center_y(np.float32(u))) * (H - 1)
        hw = float(half_width(np.float32(u))) * H
        base_y = cyp + side * hw * 0.85
        length = rng.uniform(7, 11) * (0.8 + 0.4 * (0.5 - u) / 0.5)
        tilt = rng.uniform(-0.5, 0.2)          # чуть назад, влево
        tip_x = u * (W - 1) + math.sin(tilt) * length
        tip_y = base_y + side * math.cos(tilt) * length
        bx = u * (W - 1)
        half = 2.2
        # треугольник: расстояние до оси шипа, сужение к кончику
        vx, vy = tip_x - bx, tip_y - base_y
        L = math.hypot(vx, vy)
        t = np.clip(((xx - bx) * vx + (yy - base_y) * vy) / (L * L), 0, 1)
        px, py = bx + t * vx, base_y + t * vy
        d = np.sqrt((xx - px) ** 2 + (yy - py) ** 2)
        wdt = half * (1 - t) + 0.25
        m = (1 - smoothstep(wdt - 0.5, wdt + 0.5, d)) * (t < 0.999)
        tipc = np.array([0.55, 0.12, 0.05], np.float32)
        rgb = char[None, None, :] * (1 - t[..., None]) + tipc[None, None, :] * t[..., None]
        img = over(img, rgba(rgb, m))
    img[..., 3] *= (U <= 0.5)
    return img


# Маска: градиент и в альфе, и в RGB — на стенде 11-16 при белом RGB и градиенте только в альфе зелень не проявилась
def mask_right():
    a = np.where(U >= 0.5, (U - 0.5) / 0.5, 1.0)
    return rgba(np.repeat(a[..., None], 3, 2), a)


def mask_left():
    a = np.where(U <= 0.5, (0.5 - U) / 0.5, 1.0)
    return rgba(np.repeat(a[..., None], 3, 2), a)


def bud():
    n = 64
    y, x = np.mgrid[0:n, 0:n].astype(np.float32)
    c = (n - 1) / 2
    r = np.sqrt((x - c) ** 2 + (y - c) ** 2) / c
    core = 1 - smoothstep(0.26, 0.34, r)
    halo = np.clip(1 - r, 0, 1) ** 2.2 * 0.6
    a = np.maximum(core, halo)
    out = np.ones((n, n, 4), np.float32)
    out[..., 3] = a
    return out


def save(tmp, name, img):
    h, w = img.shape[:2]
    u8 = (np.clip(img, 0, 1) * 255 + 0.5).astype(np.uint8)
    png = os.path.join(tmp, name + ".png")
    write_png(png, u8)
    png_to_paa(png, os.path.join(OUT_DIR, name + ".paa"))
    print("TEX", name, w, h)


PAL = RT.theme()["palette"]


def main():
    tmp = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "_hud")
    os.makedirs(tmp, exist_ok=True)
    os.makedirs(OUT_DIR, exist_ok=True)
    save(tmp, "vb_ar_branch_ca", branch())
    save(tmp, "vb_ar_bloom_ca", bloom())
    save(tmp, "vb_ar_thorn_ca", thorn())
    save(tmp, "vb_ar_mask_right_ca", mask_right())
    save(tmp, "vb_ar_mask_left_ca", mask_left())
    save(tmp, "vb_ar_bud_ca", bud())
    print("готово:", OUT_DIR)


if __name__ == "__main__":
    main()
