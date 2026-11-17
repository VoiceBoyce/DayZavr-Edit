# -*- coding: utf-8 -*-
r"""Библиотека бесшовных материалов для деталей Echo Runner (наши детали, навесные, колесо, салон).

  blender.exe -b --factory-startup --python er_tex_parts.py -- [--only=steel_dark,neon] [--n=1024]
Пишет в VB_EchoRunner/data/parts/: er_<имя>_co/_ca.png+paa, _nohq, _smdi, er_<имя>.rvmat.
Путь в модели: vb_echorunner\data\parts\er_<имя>_co.paa + vb_echorunner\data\parts\er_<имя>.rvmat.
Масштаб на деталях: 1 плитка = 1 м (UV — коробочная проекция, er_geom.part_data).
"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import er_texlib as T  # noqa: E402

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
OUT = os.path.join(os.path.dirname(HERE), "VB_EchoRunner", "data", "parts")
PNG = os.path.join(os.path.dirname(HERE), "work", "texsrc", "parts")     # исходники PNG — не в мод
VP = "vb_echorunner\\data\\parts\\"
N = int(opts.get("n", 1024))
ONLY = set(str(opts.get("only", "")).split(",")) - {""}
os.makedirs(OUT, exist_ok=True)
os.makedirs(PNG, exist_ok=True)


def emit(name, co, h, spec, gloss, nstr=4.0, alpha=None, rv=None):
    suf = "_ca" if alpha is not None else "_co"
    p = os.path.join(OUT, "er_" + name)
    q = os.path.join(PNG, "er_" + name)
    T.save_png(co, q + suf + ".png", alpha=alpha)
    T.save_png(T.normal_from_height(h, nstr), q + "_nohq.png", noncolor=True)
    T.save_png(T.smdi(spec, gloss), q + "_smdi.png", noncolor=True)
    for s in (suf, "_nohq", "_smdi"):
        T.to_paa(q + s + ".png", p + s + ".paa")
    rv = rv or {}
    T.write_rvmat(p + ".rvmat", VP + "er_%s_nohq.paa" % name, VP + "er_%s_smdi.paa" % name, **rv)
    print("MAT", name, co.shape)


def steel_base(n, seed, rgb=(0.105, 0.105, 0.11)):
    c = T.col(rgb, n)
    var = T.fbm(n, 2.4, seed) - 0.5
    grime = T.smooth(T.fbm(n, 3.0, seed + 1), 0.55, 0.85)
    c = c + var[..., None] * 0.05 - grime[..., None] * 0.035
    return c, var, grime


def m_steel_dark(n):
    c, var, grime = steel_base(n, 11)
    rs = T.smooth(T.fbm(n, 1.1, 12), 0.80, 0.87) * T.smooth(T.fbm(n, 3.0, 13), 0.5, 0.7)
    c = T.lerp(c, T.col((0.27, 0.12, 0.055), n), rs * 0.9)
    sc = T.scratches(n, 260, 14)
    c = c + sc[..., None] * np.array([0.20, 0.20, 0.21])
    h = -0.5 * sc + 0.35 * rs + 0.08 * T.fbm(n, 0.8, 15)
    emit("steel_dark", c, h, 0.35 + 0.3 * sc - 0.3 * rs - 0.1 * grime, 0.45 + 0.2 * sc - 0.35 * rs, 3.0,
         rv=dict(specular=0.55, power=60))


def m_steel_rust(n):
    c, var, grime = steel_base(n, 21, (0.12, 0.115, 0.11))
    r = T.smooth(T.fbm(n, 2.6, 22), 0.36, 0.60)
    tone = T.fbm(n, 1.6, 23)
    rust = T.lerp(T.col((0.31, 0.145, 0.06), n), T.col((0.15, 0.065, 0.032), n), tone)
    hot = T.smooth(T.fbm(n, 1.0, 24), 0.72, 0.8) * r
    rust = T.lerp(rust, T.col((0.46, 0.23, 0.085), n), hot * 0.7)
    c = T.lerp(c, rust, r)
    pits = T.dots(n, 1800, 25, 1.0, 3.0)
    c = c * (1 - pits[..., None] * 0.45)
    sc = T.scratches(n, 120, 26) * (1 - r)
    c = c + sc[..., None] * 0.16
    h = 0.35 * r + 0.25 * T.fbm(n, 1.0, 27) * r - 0.6 * pits - 0.3 * sc
    emit("steel_rust", c, h, 0.32 * (1 - r) + 0.06 + 0.2 * sc, 0.42 * (1 - r) + 0.1, 4.0, rv=dict(specular=0.45, power=40))


def m_flare(n):
    c = T.col((0.034, 0.034, 0.036), n) + (T.fbm(n, 1.0, 31)[..., None] - 0.5) * 0.02
    sc = T.scratches(n, 140, 32, (0.05, 0.3), 2.5)
    c = c + sc[..., None] * 0.09
    dust = T.smooth(T.fbm(n, 3.0, 33), 0.62, 0.9)
    c = T.lerp(c, T.col((0.17, 0.15, 0.12), n), dust * 0.45)
    rs = T.smooth(T.fbm(n, 1.1, 34), 0.86, 0.9)
    c = T.lerp(c, T.col((0.25, 0.11, 0.05), n), rs * 0.8)
    h = 0.15 * T.fbm(n, 0.7, 35) - 0.35 * sc + 0.2 * rs
    emit("flare", c, h, 0.22 - 0.12 * dust, 0.32 - 0.2 * dust, 2.5, rv=dict(specular=0.35, power=30))


def m_bolt(n):
    c = T.col((0.40, 0.38, 0.35), n) + (T.fbm(n, 2.0, 41)[..., None] - 0.5) * 0.12
    g = T.smooth(T.fbm(n, 2.8, 42), 0.5, 0.8)
    c = T.lerp(c, T.col((0.12, 0.09, 0.06), n), g * 0.6)
    emit("bolt", c, 0.1 * T.fbm(n, 1.0, 43), 0.6 - 0.3 * g, 0.55 - 0.3 * g, 2.0, rv=dict(specular=0.8, power=80))


def m_rubber(n):
    c = T.col((0.055, 0.055, 0.055), n) + (T.fbm(n, 0.9, 51)[..., None] - 0.5) * 0.025
    dust = T.smooth(T.fbm(n, 2.8, 52), 0.55, 0.85)
    c = T.lerp(c, T.col((0.21, 0.18, 0.14), n), dust * 0.55)
    mud = T.smooth(T.fbm(n, 1.8, 53), 0.75, 0.85)
    c = T.lerp(c, T.col((0.13, 0.10, 0.07), n), mud * 0.7)
    h = 0.25 * T.fbm(n, 0.6, 54) + 0.2 * mud
    emit("rubber", c, h, 0.08 - 0.05 * dust, 0.14 - 0.1 * dust, 3.0, rv=dict(specular=0.15, power=12))


def m_paint_red(n):
    paint = T.col((0.40, 0.045, 0.035), n) + (T.fbm(n, 2.2, 61)[..., None] - 0.5) * np.array([0.08, 0.02, 0.02])
    chips = T.smooth(T.fbm(n, 1.5, 62), 0.70, 0.74)
    ring = T.smooth(T.fbm(n, 1.5, 62), 0.66, 0.70) - chips
    steel = T.col((0.12, 0.115, 0.11), n)
    c = T.lerp(paint, T.col((0.24, 0.10, 0.045), n), np.clip(ring, 0, 1) * 0.8)
    c = T.lerp(c, steel, chips)
    sc = T.scratches(n, 160, 63)
    c = T.lerp(c, T.col((0.22, 0.21, 0.2), n), sc * 0.8)
    dust = T.smooth(T.fbm(n, 3.0, 64), 0.6, 0.88)
    c = T.lerp(c, T.col((0.20, 0.16, 0.12), n), dust * 0.35)
    h = -0.45 * chips - 0.35 * sc + 0.05 * T.fbm(n, 1.0, 65)
    emit("paint_red", c, h, 0.42 - 0.2 * chips - 0.2 * dust, 0.5 - 0.3 * chips - 0.25 * dust, 3.0,
         rv=dict(specular=0.6, power=60))


def m_neon(n):
    yy, xx = np.mgrid[0:n, 0:n] / n
    band = 0.5 + 0.5 * np.sin(xx * 2 * np.pi * 24)
    c = T.col((0.95, 0.10, 0.05), n) * (0.9 + 0.1 * band[..., None])
    h = 0.05 * band
    emit("neon", c, h, 0.8 * np.ones((n, n)), 0.85 * np.ones((n, n)), 1.0,
         rv=dict(specular=0.9, power=120, emissive=(1, 1, 1, 22)))
    # выключенное состояние — та же текстура, тёмное стекло без свечения (скрипт меняет материал)
    T.write_rvmat(os.path.join(OUT, "er_neon_off.rvmat"), VP + "er_neon_nohq.paa", VP + "er_neon_smdi.paa",
                  specular=0.9, power=120, diffuse=(0.3, 0.3, 0.3, 1))


def m_glass(n):
    n = 256
    c = T.col((0.62, 0.12, 0.08), n)
    a = np.full((n, n), 0.35)
    emit("glass", c, np.zeros((n, n)), np.full((n, n), 0.9), np.full((n, n), 0.95), 1.0, alpha=a,
         rv=dict(specular=1.0, power=200))


def m_lens(n):
    n = 256
    yy, xx = np.mgrid[0:n, 0:n]
    prism = (np.abs(((xx % 16) - 8)) / 8.0) * 0.5 + (np.abs(((yy % 16) - 8)) / 8.0) * 0.5
    c = T.col((0.80, 0.78, 0.70), n) * (0.85 + 0.15 * prism[..., None])
    emit("lens", c, prism * 0.5, np.full((n, n), 0.8), np.full((n, n), 0.9), 3.0, rv=dict(specular=1.0, power=150))


def m_screen(n):
    n = 512
    yy, xx = np.mgrid[0:n, 0:n] / n
    bg = T.col((0.05, 0.004, 0.0), n)
    grid = ((np.abs((xx * 16) % 1 - 0.5) > 0.48) | (np.abs((yy * 16) % 1 - 0.5) > 0.48)).astype(float)
    topo = T.fbm(n, 3.2, 71)
    cont = (np.abs((topo * 14) % 1 - 0.5) > 0.46).astype(float)
    c = bg + grid[..., None] * np.array([0.25, 0.02, 0.01]) + cont[..., None] * np.array([0.45, 0.04, 0.02])
    cx, cy = xx - 0.5, yy - 0.5
    r = np.sqrt(cx * cx + cy * cy)
    cross = ((np.abs(cx) < 0.003) | (np.abs(cy) < 0.003)) & (r < 0.42) & (r > 0.04)
    ring = (np.abs(r - 0.12) < 0.004) | (np.abs(r - 0.25) < 0.003)
    c = np.where((cross | ring)[..., None], np.array([0.95, 0.16, 0.08]), c)
    rng = np.random.default_rng(72)
    for _ in range(7):                                   # отметки целей
        px, py = rng.uniform(0.15, 0.85, 2)
        d = np.sqrt((xx - px) ** 2 + (yy - py) ** 2)
        c = np.where((d < 0.012)[..., None], np.array([1.0, 0.3, 0.12]), c)
    frame = (xx < 0.02) | (xx > 0.98) | (yy < 0.02) | (yy > 0.98)
    c = np.where(frame[..., None], np.array([0.6, 0.06, 0.03]), c)
    scan = 0.85 + 0.15 * ((np.arange(n) % 3) == 0)[:, None]
    c = c * scan[..., None]
    emit("screen", c, np.zeros((n, n)), np.full((n, n), 0.6), np.full((n, n), 0.8), 1.0,
         rv=dict(specular=0.7, power=100, emissive=(1, 1, 1, 10)))


def m_cable(n):
    n = 512
    yy, xx = np.mgrid[0:n, 0:n] / n * 2 * np.pi * 32
    braid = 0.5 + 0.5 * np.sin(xx + yy) * np.sin(xx - yy)
    c = T.lerp(T.col((0.22, 0.02, 0.018), n), T.col((0.52, 0.07, 0.045), n), braid)
    dirt = T.smooth(T.fbm(n, 2.6, 81), 0.55, 0.85)
    c = T.lerp(c, T.col((0.12, 0.08, 0.06), n), dirt * 0.5)
    emit("cable", c, braid * 0.6, 0.3 * braid + 0.05, 0.35 * braid + 0.1, 3.0, rv=dict(specular=0.4, power=40))


ALL = [m_steel_dark, m_steel_rust, m_flare, m_bolt, m_rubber, m_paint_red, m_neon, m_glass, m_lens, m_screen, m_cable]
for fn in ALL:
    name = fn.__name__[2:]
    if ONLY and name not in ONLY:
        continue
    fn(N)
print("OK", OUT)
