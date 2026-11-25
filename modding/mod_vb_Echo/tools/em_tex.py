# -*- coding: utf-8 -*-
r"""Текстуры материалов крафта VB_EchoMaterials (14 предметов, список утверждён пользователем 11-24).

  G:\SteamLibrary\steamapps\common\Blender\blender.exe -b --factory-startup --python em_tex.py -- [--only=carbon,pcb] [--n=1024]

Пишет в ../VB_EchoMaterials/data/: em_<имя>_co/_ca.paa, _nohq.paa, _smdi.paa, em_<имя>.rvmat (Super, как у Echo Runner).
PNG-исходники — ../work/texsrc/materials (не в мод). Путь в модели: vb_echomaterials\data\em_<имя>_co.paa.
Масштаб плитки задаёт модель (em_models.py: uvtile детали), «картинки на грань» (этикетка, плата) — uvframe.
Без ржавчины (как у брони Эхо): графит, сатиновый металл, акценты красного Эхо.
"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..", "mods_vb_vehicles", "mod_vb_EchoRunner", "tools")))
import er_texlib as T  # noqa: E402

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
OUT = os.path.join(HERE, "..", "VB_EchoMaterials", "data")
PNG = os.path.join(HERE, "..", "work", "texsrc", "materials")
VP = "vb_echomaterials\\data\\"
N = int(opts.get("n", 1024))
ONLY = set(str(opts.get("only", "")).split(",")) - {""}
os.makedirs(OUT, exist_ok=True)
os.makedirs(PNG, exist_ok=True)


def emit(name, co, h, spec, gloss, nstr=4.0, alpha=None, rv=None):
    suf = "_ca" if alpha is not None else "_co"
    p = os.path.join(OUT, "em_" + name)
    q = os.path.join(PNG, "em_" + name)
    T.save_png(co, q + suf + ".png", alpha=alpha)
    T.save_png(T.normal_from_height(h, nstr), q + "_nohq.png", noncolor=True)
    T.save_png(T.smdi(np.broadcast_to(spec, h.shape), np.broadcast_to(gloss, h.shape)), q + "_smdi.png", noncolor=True)
    for s in (suf, "_nohq", "_smdi"):
        T.to_paa(q + s + ".png", p + s + ".paa")
    T.write_rvmat(p + ".rvmat", VP + "em_%s_nohq.paa" % name, VP + "em_%s_smdi.paa" % name, **(rv or {}))
    print("MAT", name, co.shape)


def grid(n):
    yy, xx = np.mgrid[0:n, 0:n].astype(np.float32)
    return xx / n, yy / n


# ---------------------------------------------------------------- металлы
def satin(n, rgb, seed, brushed=0.05, scr=120):
    c = T.col(rgb, n) + (T.fbm(n, 2.2, seed)[..., None] - 0.5) * 0.04
    br = T.scratches(n, 1400, seed + 1, length=(0.05, 0.25), width=1.0, angle=0.0)
    sc = T.scratches(n, scr, seed + 2)
    c = c + br[..., None] * brushed + sc[..., None] * 0.12
    h = -0.3 * sc + 0.08 * br + 0.04 * T.fbm(n, 0.8, seed + 3)
    return c, h, sc, br


def m_steel(n):
    c, h, sc, br = satin(n, (0.16, 0.16, 0.17), 11)
    emit("steel", c, h, 0.45 + 0.2 * sc + 0.1 * br, 0.55 + 0.15 * sc, 2.5, rv=dict(specular=0.6, power=70))


def m_alu(n):
    """Литой анодированный алюминий графитового цвета (корпуса ЭБУ, сервомотора, гидроцилиндра)."""
    c = T.col((0.085, 0.087, 0.095), n) + (T.fbm(n, 1.6, 21)[..., None] - 0.5) * 0.03
    pit = T.dots(n, 2600, 22, 0.8, 1.8)
    sc = T.scratches(n, 90, 23)
    c = c - pit[..., None] * 0.02 + sc[..., None] * 0.14
    h = -0.25 * pit - 0.3 * sc + 0.05 * T.fbm(n, 0.6, 24)
    emit("alu", c, h, 0.38 + 0.25 * sc, 0.45 + 0.1 * sc - 0.1 * pit, 3.0, rv=dict(specular=0.5, power=55))


def primer(n, name, rgb, seed):
    """Заводской грунт/порошковая краска: «апельсиновая корка», редкие царапины до стали."""
    c = T.col(rgb, n) + (T.fbm(n, 2.6, seed)[..., None] - 0.5) * 0.025
    peel = T.fbm(n, 0.4, seed + 1, fmin=40)
    sc = T.scratches(n, 70, seed + 2)
    c = T.lerp(c, T.col((0.30, 0.30, 0.31), n), sc * 0.85)
    h = 0.12 * peel - 0.4 * sc
    emit(name, c, h, 0.22 + 0.3 * sc, 0.35 + 0.25 * sc, 2.0, rv=dict(specular=0.35, power=35))


def m_primer_k(n):
    primer(n, "primer_k", (0.042, 0.043, 0.047), 31)


def m_primer_g(n):
    primer(n, "primer_g", (0.27, 0.28, 0.285), 36)


def m_chrome(n):
    xx, yy = grid(n)
    streak = T.blur(np.random.default_rng(41).random((n, n)).astype(np.float32), 0.6)
    streak = np.repeat(streak[:, :1], n, 1) * 0.5 + 0.5 * T.fbm(n, 1.0, 42)
    c = T.col((0.70, 0.71, 0.74), n) + (streak[..., None] - 0.5) * 0.10
    emit("chrome", c, 0.02 * streak, 0.95, 0.92, 1.0, rv=dict(specular=1.2, power=180))


def m_paint_red(n):
    """Порошковая краска цвета Эхо (пружина амортизатора, метки)."""
    c = T.col((0.50, 0.035, 0.028), n) + (T.fbm(n, 2.4, 51)[..., None] - 0.5) * np.array([0.06, 0.012, 0.01])
    peel = T.fbm(n, 0.4, 52, fmin=40)
    sc = T.scratches(n, 50, 53)
    c = T.lerp(c, T.col((0.25, 0.25, 0.26), n), sc * 0.8)
    emit("paint_red", c, 0.1 * peel - 0.35 * sc, 0.5 + 0.2 * sc, 0.6, 2.0, rv=dict(specular=0.6, power=60))


def m_copper(n):
    """Обмотка катушки: ряды проволоки поперёк V (на модели — по окружности), 40 витков на плитку."""
    xx, yy = grid(n)
    rows = 40
    fr = (yy * rows) % 1.0
    shade = np.sqrt(np.clip(np.sin(np.pi * fr), 0, 1))
    tone = T.fbm(n, 2.0, 61)
    base = T.lerp(T.col((0.74, 0.36, 0.15), n), T.col((0.55, 0.24, 0.10), n), T.smooth(tone, 0.4, 0.8) * 0.7)
    c = base * (0.45 + 0.65 * shade[..., None])
    emit("copper", c, shade, 0.55 + 0.35 * shade, 0.55 + 0.3 * shade, 3.0, rv=dict(specular=0.9, power=90))


# ---------------------------------------------------------------- композиты, ткань, резина
def m_carbon(n):
    """Карбон, саржа 2/2: 32 клетки на плитку, волокна вдоль клетки, лак сверху."""
    xx, yy = grid(n)
    cells = 32
    u, v = xx * cells, yy * cells
    i, j = np.floor(u).astype(int), np.floor(v).astype(int)
    fu, fv = u - i, v - j
    warp = ((i - j) % 4) < 2
    across = np.where(warp, fv, fu)
    along = np.where(warp, fu, fv)
    tow = np.sin(np.pi * across) ** 0.6
    fib = T.fbm(n, 0.3, 71, fmin=60)
    shade = 0.35 + 0.5 * tow * (0.8 + 0.4 * fib) + 0.15 * np.where(warp, 1.0, 0.0)
    c = T.col((0.055, 0.057, 0.062), n) * (0.6 + 0.9 * shade[..., None])
    h = 0.6 * tow + 0.15 * np.sin(np.pi * along)
    emit("carbon", c, h, 0.55 + 0.35 * tow, 0.8, 2.0, rv=dict(specular=0.9, power=120))


def m_coating(n):
    """Полимерное напыление бронепластины: тёмно-серая «крошка» (матовое)."""
    sp = T.dots(n, 9000, 81, 0.8, 2.2)
    c = T.col((0.155, 0.158, 0.165), n) + (T.fbm(n, 2.0, 82)[..., None] - 0.5) * 0.03 + sp[..., None] * 0.035
    h = 0.5 * sp + 0.05 * T.fbm(n, 0.8, 83)
    emit("coating", c, h, 0.12 + 0.1 * sp, 0.22 + 0.1 * sp, 3.0, rv=dict(specular=0.2, power=20))


def weave(n, cells, seed):
    xx, yy = grid(n)
    u, v = xx * cells, yy * cells
    i, j = np.floor(u).astype(int), np.floor(v).astype(int)
    fu, fv = u - i, v - j
    warp = ((i + j) % 2) == 0
    across = np.where(warp, fv, fu)
    return np.sin(np.pi * across) ** 0.7 * (0.85 + 0.3 * T.fbm(n, 0.4, seed, fmin=50))


def m_fabric(n):
    """Кордура графитовая (бока демпферной вставки)."""
    w = weave(n, 96, 91)
    c = T.col((0.075, 0.077, 0.082), n) * (0.75 + 0.5 * w[..., None]) + (T.fbm(n, 2.0, 92)[..., None] - 0.5) * 0.02
    emit("fabric", c, 0.5 * w, 0.08 + 0.06 * w, 0.18, 2.5, rv=dict(specular=0.12, power=12))


def m_hexpad(n):
    """Стёганая демпферная ткань: соты по 10 на плитку, швы-канавки, ячейки выпуклые, по шву красная нить."""
    xx, yy = grid(n)
    k = 10.0
    # шестиугольная сетка: расстояние до ближайшего центра в двух смещённых решётках
    sx, sy = 1.0, np.sqrt(3.0)
    ky = round(k / sy)                          # целое число рядов на плитку — бесшовно (соты сжаты по V на ~4 %)
    px, py = xx * k, yy * ky * sy

    # Вороной по центрам двух подрешёток: F1 — до ближайшего центра, F2 — до второго; F2 - F1 = 0 на границе ячеек
    ds = []
    for ox, oy in ((0.0, 0.0), (0.5, sy / 2)):
        gx = np.floor((px - ox) / sx)
        gy = np.floor((py - oy) / sy)
        for ix in (-1, 0, 1, 2):
            for iy in (-1, 0, 1, 2):
                cx = (gx + ix) * sx + ox
                cy = (gy + iy) * sy + oy
                ds.append(np.sqrt((px - cx) ** 2 + (py - cy) ** 2))
    ds = np.sort(np.stack(ds, 0), 0)
    d, edge = ds[0], ds[1] - ds[0]
    seam = 1.0 - T.smooth(edge, 0.0, 0.08)
    dome = 1.0 - T.smooth(d, 0.0, 0.55)
    w = weave(n, 160, 93)
    c = T.col((0.07, 0.072, 0.078), n) * (0.75 + 0.35 * w[..., None] + 0.25 * dome[..., None])
    thread = (seam > 0.55) & (((xx + yy) * 260) % 1.0 < 0.55)
    c = np.where(thread[..., None], np.array([0.42, 0.04, 0.03]), c)
    h = 0.55 * dome - 0.6 * seam + 0.1 * w
    emit("hexpad", c, h, 0.1 + 0.05 * w, 0.2, 5.0, rv=dict(specular=0.12, power=12))


def m_rubber(n):
    c = T.col((0.045, 0.045, 0.047), n) + (T.fbm(n, 1.2, 101)[..., None] - 0.5) * 0.02
    dust = T.smooth(T.fbm(n, 2.8, 102), 0.62, 0.9)
    c = T.lerp(c, T.col((0.16, 0.15, 0.14), n), dust * 0.35)
    emit("rubber", c, 0.2 * T.fbm(n, 0.6, 103), 0.08, 0.15, 2.0, rv=dict(specular=0.15, power=12))


def m_plastic(n):
    c = T.col((0.035, 0.036, 0.038), n) + (T.fbm(n, 1.8, 111)[..., None] - 0.5) * 0.02
    sc = T.scratches(n, 40, 112)
    c = c + sc[..., None] * 0.06
    emit("plastic", c, 0.05 * T.fbm(n, 0.5, 113, fmin=30) - 0.2 * sc, 0.3, 0.45, 2.0, rv=dict(specular=0.35, power=40))


# ---------------------------------------------------------------- стекло
# (11-25: текстуры осколка Эхо и породы убраны вместе с предметом — мод универсальный, ничего не светится)
def m_glass(n):
    """Бронестекло: зеленоватый триплекс, полупрозрачный."""
    n = 256
    c = T.col((0.50, 0.66, 0.58), n)
    a = np.full((n, n), 0.42)
    emit("glass", c, np.zeros((n, n)), 0.95, 0.95, 1.0, alpha=a, rv=dict(specular=1.0, power=200))


# ---------------------------------------------------------------- картинки на грань
def m_label(n):
    """Заводская этикетка (512×512, на модели — uvframe): красная полоса, знак-кольцо Эхо, штрихкод, строки."""
    n = 512
    xx, yy = grid(n)
    c = T.col((0.80, 0.78, 0.72), n) + (T.fbm(n, 2.0, 141)[..., None] - 0.5) * 0.03
    band = yy < 0.22
    c = np.where(band[..., None], np.array([0.55, 0.04, 0.03]), c)
    slots = band & (yy > 0.07) & (yy < 0.15) & (((xx * 12) % 1.0) < 0.5) & (xx > 0.05) & (xx < 0.62)
    c = np.where(slots[..., None], np.array([0.85, 0.82, 0.76]), c)
    r = np.sqrt((xx - 0.8) ** 2 + (yy - 0.6) ** 2)
    ringm = (r > 0.09) & (r < 0.14)
    c = np.where(ringm[..., None], np.array([0.55, 0.04, 0.03]), c)
    c = np.where((r < 0.05)[..., None], np.array([0.75, 0.06, 0.04]), c)
    rng = np.random.default_rng(142)
    bars = np.zeros(n, bool)
    x = int(0.06 * n)
    while x < int(0.55 * n):
        w = int(rng.integers(2, 7))
        if rng.random() < 0.55:
            bars[x:x + w] = True
        x += w
    bc = (yy > 0.70) & (yy < 0.92) & bars[None, :]
    c = np.where(bc[..., None], np.array([0.06, 0.06, 0.06]), c)
    for k, (y0, ln) in enumerate(((0.30, 0.50), (0.39, 0.38), (0.48, 0.45), (0.57, 0.30))):
        line = (yy > y0) & (yy < y0 + 0.045) & (xx > 0.06) & (xx < 0.06 + ln)
        c = np.where(line[..., None], np.array([0.18, 0.18, 0.19]), c)
    edge = (xx < 0.015) | (xx > 0.985) | (yy < 0.015) | (yy > 0.985)
    c = np.where(edge[..., None], c * 0.8, c)
    emit("label", c, np.zeros((n, n)), 0.25, 0.4, 1.0, rv=dict(specular=0.3, power=30))


def m_pcb(n):
    """Плата блока управления (512, uvframe): чёрно-зелёная маска, золотые дорожки и площадки, белая шелкография."""
    n = 512
    xx, yy = grid(n)
    c = T.col((0.025, 0.06, 0.045), n) + (T.fbm(n, 2.0, 151)[..., None] - 0.5) * 0.02
    h = np.zeros((n, n), np.float32)
    rng = np.random.default_rng(152)
    gold = np.array([0.78, 0.60, 0.24])
    img = c.copy()
    for _ in range(70):                                  # дорожки «манхэттен»
        x, y = rng.integers(10, n - 10, 2)
        w = int(rng.integers(2, 5))
        for _ in range(int(rng.integers(2, 5))):
            if rng.random() < 0.5:
                x2 = int(np.clip(x + rng.integers(-160, 160), 8, n - 8))
                a, b = sorted((x, x2))
                img[y:y + w, a:b] = gold * 0.75
                h[y:y + w, a:b] = 0.3
                x = x2
            else:
                y2 = int(np.clip(y + rng.integers(-160, 160), 8, n - 8))
                a, b = sorted((y, y2))
                img[a:b, x:x + w] = gold * 0.75
                h[a:b, x:x + w] = 0.3
                y = y2
        img[max(0, y - 4):y + 5, max(0, x - 4):x + 5] = gold
        h[max(0, y - 4):y + 5, max(0, x - 4):x + 5] = 0.5
    for _ in range(26):                                  # шелкография: рамки компонентов
        x, y = rng.integers(20, n - 60, 2)
        w, hh = rng.integers(16, 50, 2)
        img[y:y + 2, x:x + w] = 0.8
        img[y + hh:y + hh + 2, x:x + w] = 0.8
        img[y:y + hh, x:x + 2] = 0.8
        img[y:y + hh + 2, x + w:x + w + 2] = 0.8
    for k in range(12):                                  # ряд контактов разъёма у края
        x0 = 40 + k * 18
        img[n - 30:n - 12, x0:x0 + 10] = gold
        h[n - 30:n - 12, x0:x0 + 10] = 0.5
    emit("pcb", img, h, 0.3 + 0.5 * h, 0.5 + 0.3 * h, 2.0, rv=dict(specular=0.5, power=60))


MATS = {k[2:]: v for k, v in globals().items() if k.startswith("m_")}
for name, fn in MATS.items():
    if ONLY and name not in ONLY:
        continue
    fn(N)
