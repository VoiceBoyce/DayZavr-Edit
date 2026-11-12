# -*- coding: utf-8 -*-
"""Перекраска цветовых текстур мутантов под палитру босса (VB_AnomaliesBosses, ТЗ 4.6).

Вызывает ab_models.recolor_copy: recolor(src, dst, skin, tmp, paa_to_tga, png_to_paa)
  src  — исходная .paa (SFP/MSFZ), dst — куда положить перекрашенную .paa (имя уже дополнено «_»),
  skin — палитра: fire, storm, ice, gravity, acid, psi, rad.
Нужен numpy (Python из Blender). Образец приёмов — mod_vb_Rift/tools/gen_rift_zombies.py.

Как перекрашивает (make_base):
  1. яркость исходника -> градиент палитры. Детализация (складки, поры, ворс, швы) остаётся, потому что
     градиент строится по родной яркости, плюс поверх добавляется её высокочастотная часть;
     немного исходного цвета и крупные пятна в цветах палитры — чтобы не было ровной заливки;
  2. узор прожилок своей палитры, процедурный, seed = crc32(<имя текстуры>|<палитра>), всегда одинаковый:
       fire    — тлеющие трещины, как угли (границы ячеек Вороного по искажённой сетке, угли вдоль трещин);
       storm   — ветвящиеся молнии (ломаные со смещением середины и ветками), вокруг подпалина;
       ice     — ледяные прожилки (нулевые линии шума) и иней (кристаллы-«папоротники» под 60°, изморозь);
       gravity — кольца и спирали (логарифмические спирали и концентрические кольца вокруг случайных центров);
       acid    — наплывы, пузыри и подтёки (пятна шума со светящимся краем, пузыри внутри, капли вниз);
       psi     — тонкие переливающиеся нити (вытянутый полосовой шум, цвет нитей и отлив — радужный перламутр);
       rad     — пятна и язвы (язвы с тёмной коркой и светящимся гноем, россыпь пятнышек, рыжие разводы);
  3. прожилка = затемнение вокруг + цветной ореол (screen) + яркая сердцевина цвета палитры;
  4. альфа остаётся исходной (в PNG идёт тот же канал, формат PAA выбирается как у исходника: DXT1/DXT5);
  5. _em (карта свечения Энта) — старые блики и те же прожилки, что на цветовой текстуре, в цвете палитры;
     glow_mask() — такая же маска свечения для материала с секцией camo (Ричар);
  6. карты нормалей и бликов сюда не попадают: ab_models копирует их как есть.

Для каждой перекрашенной текстуры в RESULTS[dst] остаются уменьшенная картинка и фрагмент 1:1 с прожилками
(ab_skins собирает из них превью).

Отдельный запуск для подбора: python ab_tex.py <файл.paa> <палитра> [<папка для png>]
"""
import math
import os
import shutil
import struct
import subprocess
import sys
import zlib

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))

# ------------------------------------------------------------------ палитры
# ramp      — градиент по яркости: (позиция 0..1, цвет); ramp_dark — тёмный вариант (плащи Теневика)
# mean      — к какой средней яркости тянуть (mean_dark — для тёмного варианта)
# keep      — сколько исходного цвета оставить поверх градиента; mix — доля перекраски (остальное — исходник)
# mottle    — крупные пятна: (цвет, сила); dark — затемнение у узора; halo — цветной ореол (screen)
# core, hot — сердцевина прожилки и самый яркий её центр; em — цвет свечения (_em, маска свечения)
PALETTES = {
    "fire": dict(
        ramp=[(0.0, (0.012, 0.010, 0.009)), (0.45, (0.075, 0.048, 0.036)), (0.8, (0.20, 0.12, 0.08)),
              (1.0, (0.40, 0.27, 0.18))],
        mean=0.12, keep=0.08, mix=0.93,
        mottle=((0.24, 0.22, 0.21), 0.30),
        dark=((0.010, 0.006, 0.004), 0.55),
        halo=((0.90, 0.20, 0.03), 0.70),
        core=(1.0, 0.46, 0.07), hot=(1.0, 0.90, 0.55),
        em=(1.0, 0.34, 0.04), pattern="cracks"),
    "storm": dict(
        ramp=[(0.0, (0.030, 0.034, 0.042)), (0.5, (0.20, 0.215, 0.235)), (1.0, (0.66, 0.70, 0.76))],
        mean=0.24, keep=0.06, mix=0.93,
        mottle=((0.07, 0.08, 0.10), 0.30),
        dark=((0.03, 0.04, 0.06), 0.55),
        halo=((0.15, 0.45, 1.00), 0.65),
        core=(0.55, 0.85, 1.0), hot=(0.93, 0.98, 1.0),
        em=(0.30, 0.65, 1.0), pattern="lightning"),
    "ice": dict(
        ramp=[(0.0, (0.10, 0.15, 0.22)), (0.5, (0.52, 0.64, 0.76)), (1.0, (0.93, 0.97, 1.0))],
        mean=0.46, keep=0.04, mix=0.94,
        mottle=((0.82, 0.91, 1.00), 0.25),
        dark=((0.04, 0.16, 0.38), 0.65),
        halo=((0.30, 0.75, 1.00), 0.50),
        core=(0.40, 0.86, 1.0), hot=(0.90, 1.0, 1.0),
        em=(0.45, 0.85, 1.0), pattern="ice"),
    "gravity": dict(
        ramp=[(0.0, (0.022, 0.010, 0.040)), (0.5, (0.17, 0.075, 0.25)), (1.0, (0.56, 0.42, 0.70))],
        mean=0.20, keep=0.06, mix=0.93,
        mottle=((0.05, 0.02, 0.12), 0.35),
        dark=((0.012, 0.004, 0.028), 0.55),
        halo=((0.55, 0.15, 0.95), 0.65),
        core=(0.82, 0.45, 1.0), hot=(0.97, 0.86, 1.0),
        em=(0.65, 0.25, 1.0), pattern="rings"),
    "acid": dict(
        ramp=[(0.0, (0.015, 0.022, 0.010)), (0.5, (0.11, 0.15, 0.055)), (1.0, (0.42, 0.49, 0.24))],
        mean=0.13, keep=0.08, mix=0.92,
        mottle=((0.20, 0.16, 0.07), 0.30),
        dark=((0.015, 0.04, 0.008), 0.50),
        halo=((0.35, 0.85, 0.05), 0.60),
        core=(0.60, 1.0, 0.12), hot=(0.92, 1.0, 0.62),
        em=(0.45, 1.0, 0.08), pattern="acid"),
    "psi": dict(
        ramp=[(0.0, (0.11, 0.10, 0.12)), (0.5, (0.56, 0.54, 0.59)), (1.0, (0.94, 0.93, 0.96))],
        ramp_dark=[(0.0, (0.015, 0.015, 0.022)), (0.5, (0.09, 0.09, 0.12)), (1.0, (0.42, 0.41, 0.48))],
        mean=0.46, mean_dark=0.11, keep=0.05, mix=0.93,
        mottle=((0.86, 0.76, 0.90), 0.22),
        dark=((0.16, 0.13, 0.22), 0.40),
        halo=((0.75, 0.70, 1.00), 0.50),
        core=(0.97, 0.96, 1.0), hot=(1.0, 1.0, 1.0),
        em=(0.85, 0.80, 1.0), pattern="psi"),
    "rad": dict(
        ramp=[(0.0, (0.035, 0.035, 0.028)), (0.5, (0.24, 0.24, 0.20)), (1.0, (0.62, 0.62, 0.53))],
        ramp_dark=[(0.0, (0.008, 0.008, 0.007)), (0.5, (0.05, 0.05, 0.045)), (1.0, (0.28, 0.28, 0.24))],
        mean=0.20, mean_dark=0.10, keep=0.08, mix=0.93,
        mottle=((0.36, 0.33, 0.10), 0.35),
        dark=((0.06, 0.04, 0.012), 0.75),
        halo=((0.55, 0.85, 0.05), 0.55),
        core=(0.80, 1.0, 0.14), hot=(0.97, 1.0, 0.64),
        em=(0.65, 1.0, 0.08), pattern="rad"),
}

# ------------------------------------------------------------------ текстуры
# ключ — имя исходного файла без расширения (нижний регистр)
#   dens — густота узора (0..1), k — масштаб узора (меньше — мельче; 1 = тело на текстуре 2048)
#   kind — base (по умолчанию) | fur (ворс) | eye (глаз целиком светится) | em (карта свечения)
#   key  — для em: по какой цветовой текстуре строить узор (тот же seed и размер — прожилки совпадут)
#   tone — dark: тёмный вариант палитры, если он у неё есть (psi, rad)
#   size — уменьшить до этого размера до перекраски (мелкие детали; экономия места)
#   post_size — уменьшить до этого размера после перекраски (узор считается в родном размере)
TEX = {
    # Энт
    "msfz_ent_base": dict(dens=1.0, k=1.0),
    "msfz_ent_color1": dict(dens=0.45, k=0.5),                  # ветки и листья (альфа)
    # карта свечения: узор строится в 2048 (как на цветовой, чтобы прожилки совпали), потом ужимается до 1024
    "msfz_ent_base_em": dict(kind="em", key="msfz_ent_base", em_mul=(1.0, 1.0, 0.749), post_size=1024),
    # Ричар
    "msfz_ricar_body": dict(dens=1.0, k=0.9),
    "msfz_ricar_helm": dict(dens=0.55, k=0.8, size=1024),      # шлем — мелкая деталь, 2048 не нужно
    "msfz_ricar_cloth": dict(dens=0.55, k=0.55),
    "msfz_ricar_sword": dict(dens=0.30, k=0.55),
    # Мозготрясы
    "mozgot_ca": dict(dens=1.0, k=0.75),
    "mozgot_red_ca": dict(dens=1.0, k=0.75),
    "mozgot_att_co": dict(dens=0.45, k=0.6),
    "izlom_var2_green_co": dict(dens=0.9, k=1.0, size=1024),
    "meat2_co": dict(dens=0.8, k=0.5),
    "armor_co": dict(dens=0.40, k=1.0, size=1024),
    # Теневик и Шум: тело и плащ
    "zmb_z_e_co": dict(dens=1.0, k=1.0),
    "plash_tkan_co": dict(dens=0.75, k=1.0, tone="dark"),
    "shum_f_co": dict(dens=1.0, k=1.0),
    "plash_koza_co": dict(dens=0.75, k=1.0, tone="dark"),
    # Мутаволк и коготь (текстура медведя-зомби)
    "mutawolf_co": dict(dens=1.0, k=1.0),
    "mutawolf_fur_ca": dict(dens=0.22, k=1.0, kind="fur"),
    "mutawolf_eye_co": dict(kind="eye"),
    "bear_zmb_co": dict(dens=0.9, k=1.0, size=1024),
}

# Альфа карты свечения Энта — бинарная маска старых бликов. Если в игре окажется, что шейдер умножает свечение
# на альфу (новые прожилки Энта не светятся, а старые блики светятся), поставить True: альфа станет
# max(исходная, маска прожилок). По умолчанию альфу не трогаем.
EM_ALPHA_FROM_GLOW = False

RESULTS = {}      # dst -> dict(stem, skin, thumb, crop, em_thumb)
_PAT_CACHE = {}


# ------------------------------------------------------------------ картинки
def read_tga(path):
    with open(path, "rb") as f:
        d = f.read()
    idlen, cmap, itype = d[0], d[1], d[2]
    w, h, bpp, desc = struct.unpack("<HHBB", d[12:18])
    if itype != 2 or cmap != 0 or bpp not in (24, 32):
        raise RuntimeError("TGA не того формата: " + path)
    c = bpp // 8
    px = np.frombuffer(d, np.uint8, w * h * c, 18 + idlen).reshape(h, w, c)
    if not desc & 0x20:
        px = px[::-1]
    rgb = px[..., 2::-1].astype(np.float32) / 255.0
    a = px[..., 3].copy() if c == 4 else np.full((h, w), 255, np.uint8)
    return rgb, a


def write_png(path, arr):
    arr = np.ascontiguousarray(arr)
    h, w, c = arr.shape
    ct = 2 if c == 3 else 6
    raw = b"".join(b"\x00" + arr[y].tobytes() for y in range(h))

    def chunk(t, dd):
        return struct.pack(">I", len(dd)) + t + dd + struct.pack(">I", zlib.crc32(t + dd) & 0xFFFFFFFF)

    data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, ct, 0, 0, 0)) \
        + chunk(b"IDAT", zlib.compress(raw, 4)) + chunk(b"IEND", b"")
    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    with open(path, "wb") as f:
        f.write(data)


def to_u8(x):
    return (np.clip(x, 0, 1) * 255 + 0.5).astype(np.uint8)


PAA_TYPES = {0xFF01: "DXT1", 0xFF02: "DXT2", 0xFF03: "DXT3", 0xFF04: "DXT4", 0xFF05: "DXT5",
             0x4444: "ARGB4444", 0x1555: "ARGB1555", 0x8080: "AI88", 0x8888: "ARGB8888"}


def paa_format(path):
    with open(path, "rb") as f:
        t = struct.unpack("<H", f.read(2))[0]
    return PAA_TYPES.get(t, "?")


def shrink(rgb, alpha, size):
    """Уменьшение вдвое (среднее 2x2), пока сторона больше size."""
    a = alpha.astype(np.float32)
    while size and max(rgb.shape[:2]) > size:
        h, w = rgb.shape[0] // 2 * 2, rgb.shape[1] // 2 * 2
        rgb = rgb[:h, :w].reshape(h // 2, 2, w // 2, 2, 3).mean((1, 3))
        a = a[:h, :w].reshape(h // 2, 2, w // 2, 2).mean((1, 3))
    return rgb, (a + 0.5).astype(np.uint8) if a.dtype != np.uint8 else a


def lum(rgb):
    return rgb @ np.array([0.30, 0.59, 0.11], np.float32)


# ------------------------------------------------------------------ шум и фильтры
def smoothstep(e0, e1, x):
    t = np.clip((np.asarray(x, np.float32) - e0) / (e1 - e0), 0, 1)
    return t * t * (3 - 2 * t)


def mix(a, b, t):
    return a + (b - a) * t


_FREQ = {}


def _freq(n):
    if n not in _FREQ:
        fy = (np.fft.fftfreq(n)[:, None] * n).astype(np.float32)
        fx = (np.fft.rfftfreq(n)[None, :] * n).astype(np.float32)
        _FREQ[n] = (fy, fx, np.hypot(fy, fx))
    return _FREQ[n]


def _shape_noise(n, rng, filt):
    w = rng.standard_normal((n, n)).astype(np.float32)
    out = np.fft.irfft2(np.fft.rfft2(w) * filt, s=(n, n)).astype(np.float32)
    return out / (out.std() + 1e-9)


def fractal(h, w, rng, beta, max_period):
    """Фрактальный шум (1/f^beta), самые крупные детали ~max_period пикселей. Среднее 0, СКО 1."""
    n = max(h, w)
    fy, fx, f = _freq(n)
    fmin = max(1.0, n / max(max_period, 1.0))
    filt = np.where(f >= fmin, np.maximum(f, 1e-9) ** (-beta), 0.0).astype(np.float32)
    return _shape_noise(n, rng, filt)[:h, :w]


def band(h, w, rng, period, rel_bw=0.3):
    """Полосовой шум с периодом ~period пикселей."""
    n = max(h, w)
    fy, fx, f = _freq(n)
    f0 = n / period
    filt = np.exp(-((f - f0) / (f0 * rel_bw)) ** 2).astype(np.float32)
    filt[0, 0] = 0
    return _shape_noise(n, rng, filt)[:h, :w]


def aniso_band(h, w, rng, period, angle, stretch=4.0):
    """Полосовой шум, вытянутый вдоль направления angle: нулевые линии — длинные волнистые нити."""
    n = max(h, w)
    fy, fx, f = _freq(n)
    f0 = n / period
    u = fx * math.cos(angle) + fy * math.sin(angle)
    v = -fx * math.sin(angle) + fy * math.cos(angle)
    filt = (np.exp(-((np.abs(u) - f0) / (0.35 * f0)) ** 2) * np.exp(-(v / (f0 / stretch)) ** 2)).astype(np.float32)
    filt[0, 0] = 0
    return _shape_noise(n, rng, filt)[:h, :w]


def blur(img, sigma):
    if sigma <= 0:
        return img
    n0, n1 = img.shape
    fy = np.fft.fftfreq(n0)[:, None]
    fx = np.fft.rfftfreq(n1)[None, :]
    g = np.exp(-2 * (np.pi * sigma) ** 2 * (fy ** 2 + fx ** 2))
    return np.fft.irfft2(np.fft.rfft2(img) * g, s=img.shape).astype(np.float32)


def zero_dist(nz):
    """Расстояние в пикселях до нулевой линии поля nz (|n| / |grad n|) — линии одной толщины."""
    gy, gx = np.gradient(nz)
    return np.abs(nz) / (np.hypot(gx, gy) + 1e-6)


def band_full(n, rng, period, rel_bw=0.3):
    """Полосовой шум n x n без обрезки (периодичный — для выборки по искажённым координатам)."""
    fy, fx, f = _freq(n)
    f0 = n / period
    filt = np.exp(-((f - f0) / (f0 * rel_bw)) ** 2).astype(np.float32)
    filt[0, 0] = 0
    return _shape_noise(n, rng, filt)


def sample(arr, xs, ys):
    """Билинейная выборка из периодичного поля arr по координатам xs, ys (искажение области)."""
    h, w = arr.shape
    x0 = np.floor(xs)
    y0 = np.floor(ys)
    fx = (xs - x0).astype(np.float32)
    fy = (ys - y0).astype(np.float32)
    x0 = x0.astype(np.int64) % w
    y0 = y0.astype(np.int64) % h
    x1 = (x0 + 1) % w
    y1 = (y0 + 1) % h
    top = arr[y0, x0] * (1 - fx) + arr[y0, x1] * fx
    bot = arr[y1, x0] * (1 - fx) + arr[y1, x1] * fx
    return top * (1 - fy) + bot * fy


def grid(h, w):
    y, x = np.mgrid[0:h, 0:w].astype(np.float32)
    return x, y


class Canvas:
    """Слои узора и рисование отрезков/кругов в своём окне (быстро, без полного прохода по картинке)."""

    def __init__(self, h, w):
        self.h, self.w = h, w
        self.core = np.zeros((h, w), np.float32)
        self.halo = np.zeros((h, w), np.float32)
        self.dark = np.zeros((h, w), np.float32)

    def window(self, x0, y0, x1, y1):
        xa, ya = max(0, int(math.floor(x0))), max(0, int(math.floor(y0)))
        xb, yb = min(self.w, int(math.ceil(x1)) + 1), min(self.h, int(math.ceil(y1)) + 1)
        if xa >= xb or ya >= yb:
            return None
        Y, X = np.mgrid[ya:yb, xa:xb].astype(np.float32)
        return (slice(ya, yb), slice(xa, xb)), X, Y

    def segment(self, ax, ay, bx, by, wd, inten, halo_w, dark_w=0.0, dark_k=0.0):
        r = max(halo_w, dark_w) * 2.6 + wd * 2
        win = self.window(min(ax, bx) - r, min(ay, by) - r, max(ax, bx) + r, max(ay, by) + r)
        if win is None:
            return
        sl, X, Y = win
        vx, vy = bx - ax, by - ay
        t = np.clip(((X - ax) * vx + (Y - ay) * vy) / (vx * vx + vy * vy + 1e-9), 0, 1)
        d = np.hypot(X - (ax + t * vx), Y - (ay + t * vy))
        np.maximum(self.core[sl], inten * np.exp(-(d / wd) ** 2), out=self.core[sl])
        np.maximum(self.halo[sl], inten * np.exp(-(d / halo_w) ** 2), out=self.halo[sl])
        if dark_k > 0:
            np.maximum(self.dark[sl], dark_k * np.exp(-(d / dark_w) ** 2), out=self.dark[sl])


# ------------------------------------------------------------------ узоры
# каждая функция: (h, w, rng, s) -> dict(core, halo[, dark][, tint][, veils])
#   s — масштаб в пикселях: 1.0 = тело на текстуре 2048; все размеры ниже даны «в пикселях при s = 1»
def _units(h, w, s):
    """Сколько «текстур 2048 при s = 1» помещается в картинку (для числа объектов)."""
    return h * w / (2048.0 * s) ** 2


def pat_cracks(h, w, rng, s):
    """Fire: тлеющие трещины — границы ячеек Вороного по искажённой сетке; угли и жар вдоль них."""
    x, y = grid(h, w)
    wx = fractal(h, w, rng, 2.2, 420 * s) * 22 * s
    wy = fractal(h, w, rng, 2.2, 420 * s) * 22 * s
    xw, yw = x + wx + 300 * s, y + wy + 300 * s
    e1 = voronoi_edges(xw, yw, 100 * s, rng)
    e2 = voronoi_edges(xw * 1.07 + 13 * s, yw * 0.95 + 7 * s, 40 * s, rng)
    width = max(0.9, 1.7 * s) * (0.55 + 0.9 * smoothstep(-1.2, 1.2, fractal(h, w, rng, 1.6, 300 * s)))
    keep1 = smoothstep(-0.9, 0.2, fractal(h, w, rng, 2.0, 500 * s))
    keep2 = smoothstep(0.1, 1.0, fractal(h, w, rng, 2.0, 400 * s)) * keep1
    core = np.maximum(np.exp(-(e1 / width) ** 2) * keep1, np.exp(-(e2 / (width * 0.6)) ** 2) * keep2 * 0.75)
    halo = np.maximum(np.exp(-(e1 / (width * 5)) ** 2) * keep1, np.exp(-(e2 / (width * 3)) ** 2) * keep2 * 0.5)
    dark = np.maximum(np.exp(-(e1 / (width * 14)) ** 2) * keep1, np.exp(-(e2 / (width * 7)) ** 2) * keep2 * 0.6)
    # жар неровный: одни трещины горят, другие едва тлеют; вдоль трещин — отдельные угли
    heat = 0.35 + 0.65 * smoothstep(-1.0, 1.0, fractal(h, w, rng, 1.5, 250 * s))
    ember = smoothstep(1.3, 2.6, fractal(h, w, rng, 0.8, 40 * s)) * np.clip(halo * 1.6, 0, 1)
    core = np.maximum(core * heat, ember * 0.9)
    return dict(core=core, halo=np.maximum(halo * heat, ember * 0.7), dark=dark)


def voronoi_edges(xw, yw, cell, rng):
    """Расстояние (px) до ближайшей границы ячеек Вороного; точки — сетка cell с дрожанием."""
    gx = np.floor(xw / cell).astype(np.int32)
    gy = np.floor(yw / cell).astype(np.int32)
    gx -= gx.min() - 1
    gy -= gy.min() - 1
    ox = (np.floor(xw / cell).min() - 1) * cell
    oy = (np.floor(yw / cell).min() - 1) * cell
    jx = rng.random((gy.max() + 3, gx.max() + 3)).astype(np.float32)
    jy = rng.random((gy.max() + 3, gx.max() + 3)).astype(np.float32)
    big = np.float32(1e18)
    d1 = np.full(xw.shape, big, np.float32)
    d2 = np.full(xw.shape, big, np.float32)
    p1x = np.zeros_like(xw)
    p1y = np.zeros_like(xw)
    p2x = np.zeros_like(xw)
    p2y = np.zeros_like(xw)
    for dy in (-1, 0, 1):
        for dx in (-1, 0, 1):
            cx, cy = gx + dx, gy + dy
            px = (cx + 0.1 + 0.8 * jx[cy, cx]) * cell + ox
            py = (cy + 0.1 + 0.8 * jy[cy, cx]) * cell + oy
            d = (xw - px) ** 2 + (yw - py) ** 2
            m1 = d < d1
            m2 = (~m1) & (d < d2)
            d2 = np.where(m1, d1, np.where(m2, d, d2))
            p2x = np.where(m1, p1x, np.where(m2, px, p2x))
            p2y = np.where(m1, p1y, np.where(m2, py, p2y))
            d1 = np.where(m1, d, d1)
            p1x = np.where(m1, px, p1x)
            p1y = np.where(m1, py, p1y)
    sep = np.hypot(p2x - p1x, p2y - p1y) + 1e-6
    return (d2 - d1) / (2 * sep)


def pat_lightning(h, w, rng, s):
    """Storm: ветвящиеся молнии (как фигуры Лихтенберга на коре) с подпалиной вокруг."""
    cv = Canvas(h, w)
    min_seg = max(3.0, 6.0 * s)

    def bolt(ax, ay, bx, by, wd, inten, level):
        L = math.hypot(bx - ax, by - ay)
        if L < min_seg:
            cv.segment(ax, ay, bx, by, wd, inten, wd * 5.5, wd * 13, 0.55 * inten)
            return
        nx, ny = -(by - ay) / L, (bx - ax) / L
        off = float(np.clip(rng.normal(0, 0.20), -0.45, 0.45)) * L
        mx, my = (ax + bx) / 2 + nx * off, (ay + by) / 2 + ny * off
        if level < 5 and L > 4 * min_seg and rng.random() < 0.42 * inten:
            ang = math.atan2(by - ay, bx - ax) + rng.choice([-1, 1]) * rng.uniform(0.35, 0.95)
            bl = L * rng.uniform(0.45, 0.95)
            bolt(mx, my, mx + math.cos(ang) * bl, my + math.sin(ang) * bl,
                 max(0.6, wd * 0.62), inten * 0.72, level + 1)
        bolt(ax, ay, mx, my, wd, inten, level)
        bolt(mx, my, bx, by, max(0.6, wd * 0.93), inten * 0.97, level)

    count = max(3, int(round(20 * _units(h, w, s))))
    for _ in range(count):
        ax, ay = rng.uniform(0, w), rng.uniform(0, h)
        ang = rng.uniform(0, 2 * math.pi)
        L = rng.uniform(260, 680) * s
        bolt(ax, ay, ax + math.cos(ang) * L, ay + math.sin(ang) * L,
             max(0.9, rng.uniform(1.4, 2.4) * s), rng.uniform(0.75, 1.0), 0)
    # слабые «старые» разряды: тонкая сетка нулевых линий шума
    faint = np.exp(-(zero_dist(band(h, w, rng, 220 * s, 0.4)) / max(0.7, 0.9 * s)) ** 2)
    faint *= smoothstep(0.4, 1.4, fractal(h, w, rng, 2.0, 400 * s)) * 0.35
    core = np.maximum(cv.core, faint)
    return dict(core=core, halo=np.maximum(cv.halo, faint * 0.6), dark=cv.dark)


def pat_ice(h, w, rng, s):
    """Ice: ледяные прожилки — ломаные трещины (границы ячеек Вороного, почти прямые отрезки),
    кристаллы инея — «папоротники» с ветками под 60°, изморозь пятнами с искрой."""
    x, y = grid(h, w)
    wx = fractal(h, w, rng, 2.4, 500 * s) * 6 * s
    wy = fractal(h, w, rng, 2.4, 500 * s) * 6 * s
    e1 = voronoi_edges(x + wx + 300 * s, y + wy + 300 * s, 130 * s, rng)
    e2 = voronoi_edges(x * 0.93 + wy + 200 * s, y * 1.05 + wx + 200 * s, 55 * s, rng)
    width = max(0.8, 1.5 * s) * (0.6 + 0.8 * smoothstep(-1, 1, fractal(h, w, rng, 1.6, 300 * s)))
    keep1 = smoothstep(-0.7, 0.2, fractal(h, w, rng, 2.0, 450 * s))
    keep2 = smoothstep(0.2, 1.1, fractal(h, w, rng, 2.0, 350 * s)) * keep1
    veins = np.maximum(np.exp(-(e1 / width) ** 2) * keep1, np.exp(-(e2 / (width * 0.65)) ** 2) * keep2 * 0.7)
    halo = np.maximum(np.exp(-(e1 / (width * 6)) ** 2) * keep1, np.exp(-(e2 / (width * 4)) ** 2) * keep2 * 0.5)
    dark = np.maximum(np.exp(-(e1 / (width * 3.5)) ** 2) * keep1, np.exp(-(e2 / (width * 2.5)) ** 2) * keep2 * 0.6)
    # кристаллы инея: «папоротники» — прямой ствол и боковые ветки под 60°, у веток свои ветки
    cv = Canvas(h, w)

    def fern(ax, ay, ang, L, wd, inten, level):
        bx, by = ax + math.cos(ang) * L, ay + math.sin(ang) * L
        cv.segment(ax, ay, bx, by, wd, inten, wd * 4, wd * 3, 0.6 * inten)
        if level >= 2 or L < 5 * s:
            return
        steps = int(rng.integers(4, 7))
        for i in range(1, steps + 1):
            t = i / (steps + 1)
            px, py = ax + (bx - ax) * t, ay + (by - ay) * t
            bl = L * (1 - t) * rng.uniform(0.35, 0.6)
            for sgn in (-1, 1):
                fern(px, py, ang + sgn * math.pi / 3, bl, max(0.55, wd * 0.72), inten * 0.85, level + 1)

    count = max(4, int(round(70 * _units(h, w, s))))
    for _ in range(count):
        ax, ay = rng.uniform(0, w), rng.uniform(0, h)
        fern(ax, ay, rng.uniform(0, 2 * math.pi), rng.uniform(35, 110) * s,
             max(0.65, 1.2 * s), rng.uniform(0.7, 1.0), 0)
    frost = smoothstep(0.0, 1.3, fractal(h, w, rng, 1.8, 350 * s))
    sparkle = smoothstep(1.2, 2.6, fractal(h, w, rng, 0.4, 30 * s))
    frost_veil = frost * (0.45 + 0.55 * sparkle)
    return dict(core=np.maximum(veins, cv.core * 0.9), halo=np.maximum(halo * 0.8, cv.halo * 0.6),
                dark=np.maximum(dark * 0.85, cv.dark), veils=[(frost_veil, (0.90, 0.95, 1.0), 0.5)])


def pat_rings(h, w, rng, s):
    """Gravity: светящиеся логарифмические спирали и разорванные кольца-орбиты вокруг случайных центров,
    к центру — тёмный провал с яркой точкой."""
    core = np.zeros((h, w), np.float32)
    halo = np.zeros((h, w), np.float32)
    dark = np.zeros((h, w), np.float32)
    count = max(3, int(round(26 * _units(h, w, s))))
    lw = max(0.85, 1.5 * s)
    for _ in range(count):
        cx, cy = rng.uniform(0, w), rng.uniform(0, h)
        R = rng.uniform(90, 300) * s
        ya, yb = max(0, int(cy - R)), min(h, int(cy + R) + 1)
        xa, xb = max(0, int(cx - R)), min(w, int(cx + R) + 1)
        if xa >= xb or ya >= yb:
            continue
        Y, X = np.mgrid[ya:yb, xa:xb].astype(np.float32)
        sl = (slice(ya, yb), slice(xa, xb))
        dx, dy = X - cx, Y - cy
        r = np.hypot(dx, dy) + 1e-3
        th = np.arctan2(dy, dx)
        k1 = int(rng.integers(2, 5))
        rw = r * (1 + 0.08 * np.sin(k1 * th + rng.uniform(0, 6.3)))
        inten = rng.uniform(0.7, 1.0)
        if rng.random() < 0.7:   # спираль: витки на радиусе r идут с шагом 2*pi*r/a
            arms = int(rng.integers(2, 5))
            a = rng.uniform(7.0, 13.0)
            sgn = rng.choice([-1, 1])
            phase = (a * np.log(rw) + sgn * arms * th) / (2 * math.pi) + rng.random()
            spacing = 2 * math.pi * rw / a
            gaps = 1.0
        else:                    # кольца-орбиты с разрывами
            spacing = np.float32(rng.uniform(16, 34) * s)
            phase = rw / spacing + rng.random()
            gaps = smoothstep(-0.3, 0.4, np.sin(int(rng.integers(2, 5)) * th + rng.uniform(0, 6.3)
                                                 + rw / (spacing * 1.7)))
        f = phase - np.floor(phase)
        dpx = np.minimum(f, 1 - f) * spacing
        env = smoothstep(R, R * 0.45, r) * smoothstep(R * 0.04, R * 0.16, r) * gaps
        c = np.exp(-(dpx / lw) ** 2) * env * inten
        hl = np.exp(-(dpx / (lw * 5)) ** 2) * env * inten
        dot = np.exp(-(r / (3.5 * s + 1)) ** 2) * inten
        well = (1 - smoothstep(0, R * 0.5, r)) * smoothstep(R, R * 0.45, r)
        np.maximum(core[sl], np.maximum(c, dot), out=core[sl])
        np.maximum(halo[sl], np.maximum(hl, dot), out=halo[sl])
        np.maximum(dark[sl], well * 0.8 + env * 0.12, out=dark[sl])
    return dict(core=core, halo=halo, dark=np.clip(dark, 0, 1))


def pat_acid(h, w, rng, s):
    """Acid: наплывы (мокрые пятна со светящимся рваным краем), пузыри в них и подтёки с каплями вниз."""
    nf = fractal(h, w, rng, 2.4, 480 * s) + 0.12 * fractal(h, w, rng, 1.6, 120 * s)
    thr = float(np.quantile(nf, 0.86))
    flow = smoothstep(thr, thr + 0.35, nf)
    d = zero_dist(nf - thr)
    rim_w = max(0.9, 1.8 * s)
    rim = np.exp(-(d / rim_w) ** 2)
    halo = np.exp(-(d / (rim_w * 4)) ** 2)
    cv = Canvas(h, w)
    # подтёки: от нижнего края наплыва вниз по текстуре, с лёгким вилянием, капля на конце
    gy, gx = np.gradient(nf)
    cand = np.argwhere((rim > 0.6) & (gy < -0.5 * np.abs(gx)))
    n_drip = min(len(cand), max(3, int(round(150 * _units(h, w, s)))))
    if n_drip:
        for py, px in cand[rng.choice(len(cand), n_drip, replace=False)]:
            L = rng.uniform(20, 140) * s
            wd = max(0.8, rng.uniform(1.0, 2.4) * s)
            x0, y0 = float(px), float(py)
            parts = 4
            for i in range(parts):
                x1 = x0 + rng.normal(0, 0.06) * L / parts * 2
                y1 = y0 + L / parts
                cv.segment(x0, y0, x1, y1, wd * (1 - 0.15 * i), 0.9, wd * 3.5, wd * 5, 0.45)
                x0, y0 = x1, y1
            cv.segment(x0, y0, x0, y0 + wd * 1.4, wd * 1.9, 1.0, wd * 4.5, wd * 6, 0.45)
    # пузыри внутри наплывов
    inside = np.argwhere(flow > 0.55)
    n_bub = min(len(inside), max(3, int(round(320 * _units(h, w, s)))))
    bub = np.zeros((h, w), np.float32)
    if n_bub:
        for py, px in inside[rng.choice(len(inside), n_bub, replace=False)]:
            R = rng.uniform(2.5, 11) * s + 1.0
            win = cv.window(px - R * 1.6, py - R * 1.6, px + R * 1.6, py + R * 1.6)
            if win is None:
                continue
            sl, X, Y = win
            r = np.hypot(X - px, Y - py)
            ring = np.exp(-((r - R) / max(0.7, 0.18 * R)) ** 2)
            hi = np.exp(-(np.hypot(X - (px - 0.35 * R), Y - (py - 0.35 * R)) / max(0.6, 0.2 * R)) ** 2)
            np.maximum(bub[sl], np.maximum(ring * 0.8, hi), out=bub[sl])
    # внутри наплыва — мокрый тёмный слой с редкими светящимися прожилками, а не сплошная заливка
    inner = np.exp(-(zero_dist(band(h, w, rng, 60 * s, 0.4)) / max(0.7, 0.9 * s)) ** 2) * flow * 0.55
    core = np.maximum.reduce([rim * 0.95, cv.core, bub * flow, inner, flow * 0.06])
    halo = np.maximum.reduce([halo * 0.75, cv.halo * 0.65, flow * 0.12])
    wet = np.clip(flow + cv.dark * 0.5, 0, 1)
    return dict(core=core, halo=halo, dark=np.maximum(cv.dark, wet * 0.35),
                veils=[(wet, (0.07, 0.17, 0.02), 0.55)])


def pat_psi(h, w, rng, s):
    """Psi: тонкие длинные нити — нулевые линии полосового шума по сильно искажённым координатам
    (текут, изгибаются, местами сходятся в узелки); цвет нитей и отлив кожи — радужный перламутр."""
    x, y = grid(h, w)
    n = max(h, w)
    core = np.zeros((h, w), np.float32)
    halo = np.zeros((h, w), np.float32)
    lw = max(0.65, 1.0 * s)
    for i, period in enumerate((190 * s, 120 * s)):
        wx = fractal(h, w, rng, 2.6, 700 * s) * 90 * s
        wy = fractal(h, w, rng, 2.6, 700 * s) * 90 * s
        nz = sample(band_full(n, rng, period, 0.35), x + wx, y + wy)
        d = zero_dist(nz)
        fade = smoothstep(-0.6, 0.6, fractal(h, w, rng, 2.0, 450 * s))
        k = 1.0 if i == 0 else 0.7
        np.maximum(core, np.exp(-(d / (lw * (1.0 if i == 0 else 0.8))) ** 2) * fade * k, out=core)
        np.maximum(halo, np.exp(-(d / (lw * 7)) ** 2) * fade * k, out=halo)
    # узелки там, где нити сходятся
    nodes = smoothstep(0.5, 0.85, blur(core, 2.0 * s + 0.6) * 3.2)
    core = np.maximum(core, nodes * 0.85)
    # перламутр: оттенок плавно гуляет по поверхности
    hue = 0.6 * fractal(h, w, rng, 2.2, 600 * s) + (x + y) / (1100.0 * s)
    tint = np.stack([0.86 + 0.14 * np.cos(2 * np.pi * (hue + o)) for o in (0.0, 1 / 3, 2 / 3)], -1)
    sheen = smoothstep(-0.3, 1.2, fractal(h, w, rng, 1.8, 300 * s))
    return dict(core=core, halo=halo, dark=halo * 0.4, tint=tint.astype(np.float32),
                veils=[(sheen * 0.6, None, 0.35)])


def pat_rad(h, w, rng, s):
    """Rad: язвы (светящийся гной, тёмный кратер, светящийся рваный край, корка вокруг),
    светящиеся пятна и россыпь пятнышек на заражённых участках, рыжие разводы."""
    x, y = grid(h, w)
    patch = smoothstep(-0.1, 1.0, fractal(h, w, rng, 2.2, 480 * s))
    lumpy = smoothstep(-1.2, 1.2, fractal(h, w, rng, 1.0, 12 * s + 4))     # бугристость гноя
    ragged = fractal(h, w, rng, 1.4, 40 * s + 6)                               # рваный край
    cv = Canvas(h, w)
    count = max(4, int(round(85 * _units(h, w, s))))
    pts = []
    tries = 0
    while len(pts) < count and tries < count * 40:
        tries += 1
        px, py = rng.uniform(0, w), rng.uniform(0, h)
        if rng.random() < 0.15 + 0.85 * patch[int(py), int(px)]:
            pts.append((px, py))
            for _ in range(int(rng.integers(0, 4))):          # мелкие язвочки рядом
                pts.append((px + rng.normal(0, 25 * s), py + rng.normal(0, 25 * s)))
    for i, (px, py) in enumerate(pts):
        R = (rng.uniform(14, 50) if i % 3 == 0 else rng.uniform(5, 16)) * s + 1.5
        win = cv.window(px - R * 2, py - R * 2, px + R * 2, py + R * 2)
        if win is None:
            continue
        sl, X, Y = win
        rn = np.hypot(X - px, Y - py) / (R * (1 + 0.16 * ragged[sl]))
        pus = (1 - smoothstep(0.30, 0.55, rn)) * (0.72 + 0.28 * lumpy[sl])
        crater = smoothstep(0.35, 0.55, rn) * (1 - smoothstep(0.82, 0.98, rn))
        rim = np.exp(-((rn - 0.95) / 0.10) ** 2) * smoothstep(-0.6, 0.4, ragged[sl])
        crust = 1 - smoothstep(0.95, 1.8, rn)
        inten = rng.uniform(0.75, 1.0)
        np.maximum(cv.core[sl], np.maximum(pus * 0.95, rim * 0.75) * inten, out=cv.core[sl])
        np.maximum(cv.halo[sl], np.exp(-(np.maximum(rn - 0.3, 0) / 0.7) ** 2) * 0.7 * inten, out=cv.halo[sl])
        np.maximum(cv.dark[sl], np.maximum(crater * 0.9, crust * 0.6), out=cv.dark[sl])
    # светящиеся пятна неровной формы и мелкая россыпь
    blot_n = fractal(h, w, rng, 2.0, 70 * s)
    blots = smoothstep(1.35, 2.1, blot_n) * patch
    specks = smoothstep(2.1, 2.9, fractal(h, w, rng, 0.3, 25 * s)) * patch
    spots = np.maximum(blots * 0.75, specks * 0.9)
    spots_halo = np.clip(blur(spots, 3.0 * s + 0.6) * 2.2, 0, 1)
    core = np.maximum(cv.core, spots)
    halo = np.maximum(cv.halo, spots_halo * 0.7)
    dark = np.maximum(cv.dark, smoothstep(0.95, 1.45, blot_n) * patch * 0.6)
    stain = patch * smoothstep(-0.5, 1.0, fractal(h, w, rng, 1.6, 200 * s))
    return dict(core=core, halo=halo, dark=dark, veils=[(stain, (0.40, 0.36, 0.10), 0.40)])


PATTERNS = dict(cracks=pat_cracks, lightning=pat_lightning, ice=pat_ice, rings=pat_rings,
                acid=pat_acid, psi=pat_psi, rad=pat_rad)


def pattern(key, skin, h, w):
    """Узор палитры для текстуры key размера h x w. Повторяемый: тот же key/skin/размер -> тот же узор."""
    ck = (key, skin, h, w)
    if ck in _PAT_CACHE:
        return _PAT_CACHE[ck]
    info = TEX.get(key, {})
    s = max(h, w) / 2048.0 * info.get("k", 1.0)
    rng = np.random.default_rng(zlib.crc32((key + "|" + skin).encode("utf-8")))
    dens = info.get("dens", 0.8)
    # густота: где-то узор плотный, где-то почти пропадает — чтобы не лежал ровной сеткой
    D = dens * smoothstep(-1.0, 0.4, fractal(h, w, rng, 2.0, 650 * s))
    P = PATTERNS[PALETTES[skin]["pattern"]](h, w, rng, s)
    P["core"] = np.clip(P["core"], 0, 1) * D
    P["halo"] = np.clip(P["halo"], 0, 1) * D
    if "dark" in P:
        P["dark"] = np.clip(P["dark"], 0, 1) * np.sqrt(D)
    if len(_PAT_CACHE) > 2:
        _PAT_CACHE.clear()
    _PAT_CACHE[ck] = P
    return P


# ------------------------------------------------------------------ перекраска
def apply_ramp(t, stops):
    out = np.empty(t.shape + (3,), np.float32)
    pos = [p for p, _ in stops]
    for c in range(3):
        out[..., c] = np.interp(t, pos, [col[c] for _, col in stops])
    return out


def make_base(rgb, alpha, skin, key, info):
    pal = PALETTES[skin]
    h, w, _ = rgb.shape
    s = max(h, w) / 2048.0 * info.get("k", 1.0)
    valid = alpha > 127
    if valid.mean() < 0.03:
        valid = np.ones_like(valid)
    L = lum(rgb)
    lo, hi = np.percentile(L[valid], [1.0, 99.5])
    Ln = np.clip((L - lo) / max(hi - lo, 1e-3), 0, 1)
    tone = info.get("tone")
    ramp = pal.get("ramp_" + tone, pal["ramp"]) if tone else pal["ramp"]
    target = pal.get("mean_" + tone, pal["mean"]) if tone else pal["mean"]
    m = float(np.clip(Ln[valid].mean(), 0.03, 0.97))
    g = float(np.clip(math.log(target) / math.log(m), 0.35, 3.0))
    Lt = Ln ** g
    base = apply_ramp(Lt, ramp)
    # родная мелкая детализация поверх градиента (поры, складки, ворс)
    hp = L - blur(L, max(1.0, 2.5 * s))
    base += hp[..., None] * 0.35
    # немного исходного цвета (кровь, грязь, швы остаются читаемыми)
    base += (rgb - L[..., None]) * pal["keep"]
    # крупные пятна в цвете палитры
    rng = np.random.default_rng(zlib.crc32((key + "|" + skin + "|mottle").encode("utf-8")))
    mt = smoothstep(0.2, 1.6, fractal(h, w, rng, 2.0, 520 * s))
    mcol, ma = pal["mottle"]
    shade = (0.55 + 0.9 * Lt)[..., None]
    base = mix(base, np.array(mcol, np.float32) * shade, (mt * ma)[..., None])

    P = pattern(key, skin, h, w)
    tint = P.get("tint")
    for mask, col, a in P.get("veils", []):
        if col is None:            # перламутровый отлив (psi)
            veil = base * (0.75 + 0.5 * tint)
        else:
            veil = np.array(col, np.float32) * shade
        base = mix(base, veil, (np.clip(mask, 0, 1) * a)[..., None])
    if "dark" in P:
        dc, da = pal["dark"]
        base = mix(base, np.array(dc, np.float32), (P["dark"] * da)[..., None])
    hc, ha = pal["halo"]
    hcol = np.array(hc, np.float32) * (tint if tint is not None else 1.0)
    base = 1 - (1 - np.clip(base, 0, 1)) * (1 - hcol * (P["halo"] * ha)[..., None])
    core = P["core"]
    ccol = np.array(pal["core"], np.float32) * (tint if tint is not None else 1.0)
    ccol = ccol * np.clip(0.88 + 0.5 * (Lt - 0.5), 0.7, 1.12)[..., None]
    ccol = mix(ccol, np.array(pal["hot"], np.float32), (core ** 3)[..., None])
    base = mix(base, ccol, core[..., None])
    return mix(rgb, np.clip(base, 0, 1), pal["mix"]), core


def make_fur(rgb, alpha, skin, key, info):
    out, core = make_base(rgb, alpha, skin, key, info)
    if skin == "ice":       # кончики ворса в инее
        L = lum(rgb)
        tips = smoothstep(0.25, 0.7, L / max(float(np.percentile(L[alpha > 127], 99)), 1e-3))
        out = mix(out, np.array([0.92, 0.96, 1.0], np.float32), (tips * 0.35)[..., None])
    return out, core


def make_eye(rgb, alpha, skin, key, info):
    """Глаз целиком: радужка в цвете свечения палитры, зрачок остаётся тёмным."""
    pal = PALETTES[skin]
    L = lum(rgb)
    Ln = np.clip(L / max(float(np.percentile(L, 99.5)), 1e-3), 0, 1)
    out = np.array(pal["core"], np.float32) * (Ln ** 1.1 * 1.25)[..., None] \
        + np.array(pal["hot"], np.float32) * (Ln ** 4 * 0.5)[..., None]
    return np.clip(out, 0, 1), Ln


def em_rgb(skin, P, glints=None, em_mul=None, scale=1.0):
    pal = PALETTES[skin]
    k = P["core"] * 1.0 + P["halo"] * 0.22
    if glints is not None:
        k = np.maximum(k, glints * 0.85)
    em = np.array(pal["em"], np.float32) * k[..., None]
    if P.get("tint") is not None:
        em = em * P["tint"]
    em = em + np.array(pal["hot"], np.float32) * (P["core"] ** 3 * 0.35)[..., None]
    em = em * scale
    if em_mul is not None:     # emmisive[] встроенного материала умножает текстуру — заранее делим
        em = em / np.array(em_mul, np.float32)
        em = em / np.maximum(1.0, em.max(-1))[..., None]
    return np.clip(em, 0, 1)


def make_em(rgb, alpha, skin, key, info):
    """_em Энта: старые блики (их яркость) + прожилки той же цветовой текстуры, всё в цвете палитры."""
    h, w, _ = rgb.shape
    pkey = info.get("key", key)
    P = pattern(pkey, skin, h, w)
    L = lum(rgb)
    lit = L[L > 0.02]
    top = float(np.percentile(lit, 99.5)) if lit.size else 1.0
    glints = np.clip(L / max(top, 1e-3), 0, 1)
    return em_rgb(skin, P, glints, info.get("em_mul")), P["core"]


KINDS = dict(base=make_base, fur=make_fur, eye=make_eye, em=make_em)


def process(src, skin, tmp, paa_to_tga):
    """Перекраска в памяти: (rgb 0..1, альфа uint8, имя, сведения о текстуре, маска узора, исходный rgb)."""
    if skin not in PALETTES:
        raise RuntimeError("неизвестная палитра: " + skin)
    stem = os.path.splitext(os.path.basename(src))[0].lower()
    info = TEX.get(stem, {})
    rgb, alpha = read_tga(paa_to_tga(src, tmp))
    rgb, alpha = shrink(rgb, alpha, info.get("size"))
    out, mask = KINDS[info.get("kind", "base")](rgb, alpha, skin, stem, info)
    if info.get("kind") == "em" and EM_ALPHA_FROM_GLOW:
        alpha = np.maximum(alpha, to_u8(np.clip(mask * 2, 0, 1)))
    if info.get("post_size"):
        full = alpha
        out, alpha = shrink(out, full, info["post_size"])
        rgb, _ = shrink(rgb, full, info["post_size"])
        st = mask.shape[0] // out.shape[0]
        mask = mask[::st, ::st]
    return out, alpha, stem, info, mask, rgb


def _thumb(arr, size=512):
    h, w = arr.shape[:2]
    st = max(1, max(h, w) // size)
    return np.ascontiguousarray(arr[::st, ::st])


def _crop(arr, mask, size=640, pos=False):
    """Фрагмент 1:1, где узора больше всего (pos=True — ещё и его угол y, x)."""
    h, w = mask.shape
    size = min(size, h, w)
    b = 8
    hb, wb = h // b, w // b
    m = mask[:hb * b, :wb * b].reshape(hb, b, wb, b).mean((1, 3))
    k = size // b
    c = np.pad(m, ((1, 0), (1, 0))).cumsum(0).cumsum(1)
    win = c[k:, k:] - c[:-k, k:] - c[k:, :-k] + c[:-k, :-k]
    iy, ix = np.unravel_index(np.argmax(win), win.shape)
    out = np.ascontiguousarray(arr[iy * b:iy * b + size, ix * b:ix * b + size])
    return (out, (iy * b, ix * b)) if pos else out


def save_paa(out, alpha, dst, name, fmt, tmp, png_to_paa):
    """PNG с исходной альфой -> PAA. Имя временного файла с суффиксом _co/_ca, чтобы ImageToPAA взял
    тот же формат, что у исходника (DXT1 / DXT5)."""
    has_alpha = bool((alpha < 255).any())
    suffix = "_ca" if (fmt == "DXT5" or (has_alpha and fmt not in ("DXT1",))) else "_co"
    arr = to_u8(out)
    if has_alpha or suffix == "_ca":
        arr = np.concatenate([arr, alpha[..., None]], -1)
    png = os.path.join(tmp, "png", name + suffix + ".png")
    write_png(png, arr)
    paa = png[:-4] + ".paa"
    if os.path.exists(paa):
        os.remove(paa)
    png_to_paa(png, paa)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(paa, dst)
    return arr


def recolor(src, dst, skin, tmp, paa_to_tga, png_to_paa):
    out, alpha, stem, info, mask, src_rgb = process(src, skin, tmp, paa_to_tga)
    arr = save_paa(out, alpha, dst, stem + "_" + skin, paa_format(src), tmp, png_to_paa)
    rgb8 = arr[..., :3]
    crop, (cy, cx) = _crop(rgb8, mask, pos=True)
    orig = to_u8(src_rgb[cy:cy + crop.shape[0], cx:cx + crop.shape[1]])
    RESULTS[dst] = dict(stem=stem, skin=skin, kind=info.get("kind", "base"), size=rgb8.shape[:2],
                        thumb=_thumb(rgb8), crop=crop, orig_crop=orig, orig_thumb=_thumb(to_u8(src_rgb)))
    print("   перекраска %-24s %-8s %dx%d" % (stem, skin, rgb8.shape[1], rgb8.shape[0]))


def glow_mask(src, dst, skin, tmp, paa_to_tga, png_to_paa, size=1024, scale=1.0):
    """Маска свечения (_e_co, Stage8 материала) по узору цветовой текстуры src: те же прожилки, цвет палитры.
    Узор строится в родном размере текстуры (как на цветовой), потом уменьшается до size."""
    stem = os.path.splitext(os.path.basename(src))[0].lower()
    info = TEX.get(stem, {})
    rgb, alpha = read_tga(paa_to_tga(src, tmp))
    rgb, alpha = shrink(rgb, alpha, info.get("size"))
    h, w, _ = rgb.shape
    em = em_rgb(skin, pattern(stem, skin, h, w), scale=scale)
    em, _ = shrink(em, np.full((h, w), 255, np.uint8), size)
    arr = save_paa(em, np.full(em.shape[:2], 255, np.uint8), dst, stem + "_" + skin + "_e",
                   "DXT1", tmp, png_to_paa)
    RESULTS[dst] = dict(stem=stem + "_e", skin=skin, kind="glow", size=arr.shape[:2], thumb=_thumb(arr),
                        crop=None)
    print("   свечение   %-24s %-8s %dx%d" % (stem, skin, arr.shape[1], arr.shape[0]))


# ------------------------------------------------------------------ отдельный запуск (подбор палитр)
def main():
    a = sys.argv[1:]
    if len(a) < 2:
        raise SystemExit(__doc__)
    import ab_models  # noqa: E402
    src, skin = a[0], a[1]
    out_dir = a[2] if len(a) > 2 else os.path.join(os.environ.get("TEMP", HERE), "vb_ab_skins", "try")
    tmp = os.path.join(os.environ.get("TEMP", HERE), "vb_ab_skins", "tex")
    out, alpha, stem, info, mask, _ = process(src, skin, tmp, ab_models.paa_to_tga)
    write_png(os.path.join(out_dir, stem + "_" + skin + ".png"), to_u8(out))
    write_png(os.path.join(out_dir, stem + "_" + skin + "_crop.png"), _crop(to_u8(out), mask))
    print("готово:", out_dir)


if __name__ == "__main__":
    sys.path.insert(0, HERE)
    main()
