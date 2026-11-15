# -*- coding: utf-8 -*-
"""Перекраска ванильных текстур изделий «Древнего корня»: нож (Терновый клинок), дождевик (Плащ лешего),
канистра (Канистра живицы). Цвета — ar_theme.py, вывод — ../VB_AncientRoot/data/retex/*.paa.

  "G:\\SteamLibrary\\steamapps\\common\\Blender\\5.2\\python\\bin\\python.exe" gen_ar_retex.py <папка_для_png>

Перенесено 2026-11-15 из временного retex.py (сессия 2026-11-12), которым сделаны текстуры сервера 16.
Тот читал PNG через bpy (строки снизу вверх), поэтому здесь картинка тоже переворачивается на время обработки:
рисунок жил тот же. Материалы *.rvmat в data/retex нейтральные (свечение белое × маска), их не трогаем.
"""
import os
import subprocess
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ar_theme as RT  # noqa: E402  палитра и стражи «Древнего корня», папка вывода
from ar_texlib import IMAGE_TO_PAA, read_tga, write_png, png_to_paa  # noqa: E402

PAL = RT.theme()["palette"]
OUT_DIR = os.path.join(RT.mod_dir(), "data", "retex")
BS = chr(92)
SRC = {
    "fange_knife_co": BS.join(["P:", "DZ", "weapons", "melee", "blade", "data", "fange_knife_co.paa"]),
    "raincoat_black_co": BS.join(["P:", "DZ", "characters", "tops", "data", "raincoat_black_co.paa"]),
    "raincoat_g_black_co": BS.join(["P:", "DZ", "characters", "tops", "data", "raincoat_g_black_co.paa"]),
    "jerrycan_co": BS.join(["P:", "DZ", "vehicles", "parts", "data", "jerrycan_co.paa"]),
}


def load(name, tmp):
    tga = os.path.join(tmp, name + "_src.tga")
    if not os.path.exists(tga):
        r = subprocess.run([IMAGE_TO_PAA, SRC[name], tga], capture_output=True, text=True)
        if r.returncode != 0 or not os.path.exists(tga):
            raise RuntimeError("ImageToPAA: " + SRC[name] + " " + r.stdout[-300:] + r.stderr[-300:])
    rgb, a = read_tga(tga)
    px = np.dstack([rgb, a])
    return px[::-1].copy()            # как bpy: строки снизу вверх


def save(px, png, paa):
    out = np.clip(px[::-1], 0, 1)
    write_png(png, (out * 255 + 0.5).astype(np.uint8))
    png_to_paa(png, paa)


def vnoise(h, w, cy, cx, rng):
    g = rng.standard_normal((cy + 2, cx + 2))
    ys = np.linspace(0, cy, h, endpoint=False)
    xs = np.linspace(0, cx, w, endpoint=False)
    y0 = ys.astype(int)
    x0 = xs.astype(int)
    fy = ys - y0
    fx = xs - x0
    fy = fy * fy * (3 - 2 * fy)
    fx = fx * fx * (3 - 2 * fx)
    a = g[y0][:, x0]
    b = g[y0][:, x0 + 1]
    c = g[y0 + 1][:, x0]
    d = g[y0 + 1][:, x0 + 1]
    top = a + (b - a) * fx[None, :]
    bot = c + (d - c) * fx[None, :]
    return top + (bot - top) * fy[:, None]


def fbm(h, w, cells, rng, octaves=4):
    asp = w / h
    s = np.zeros((h, w), np.float32)
    amp = 1.0
    tot = 0.0
    for o in range(octaves):
        k = cells * (2 ** o)
        s += amp * vnoise(h, w, k, max(1, int(k * asp)), rng)
        tot += amp
        amp *= 0.5
    return s / tot


def veins(h, w, cells, width, seed):
    rng = np.random.default_rng(seed)
    n1 = fbm(h, w, cells, rng)
    n2 = fbm(h, w, cells * 2, rng)
    m = np.exp(-(n1 / width) ** 2) + 0.6 * np.exp(-(n2 / (width * 0.7)) ** 2) * (fbm(h, w, cells // 2 + 1, rng) > 0)
    return np.clip(m, 0, 1)


def retex(src, name, dark, vein, cells, width, strength, seed, tmp, keep=0.0, moss=None):
    px = load(src, tmp)
    h, w = px.shape[:2]
    rgb = px[..., :3]
    lum = rgb[..., 0] * 0.3 + rgb[..., 1] * 0.59 + rgb[..., 2] * 0.11
    base = (0.25 + 0.95 * lum)[..., None] * np.array(dark, np.float32)[None, None, :]
    base = base * (1 - keep) + rgb * keep
    if moss is not None:
        # пятна мха (Плащ лешего); свой генератор шума
        mrng = np.random.default_rng(seed + 77)
        mm = np.clip((fbm(h, w, 6, mrng) - 0.05) * 3.0, 0, 1)[..., None] * 0.8
        base = base * (1 - mm) + (0.3 + 0.9 * lum)[..., None] * np.array(moss, np.float32)[None, None, :] * mm
    m = veins(h, w, cells, width, seed)[..., None] * strength
    out = base * (1 - m) + np.array(vein, np.float32)[None, None, :] * m
    px[..., :3] = out
    os.makedirs(OUT_DIR, exist_ok=True)
    save(px, os.path.join(tmp, name + "_co.png"), os.path.join(OUT_DIR, name + "_co.paa"))
    # маска свечения: только жилы (Stage8 в rvmat)
    e = np.zeros_like(px)
    e[..., :3] = np.array(vein, np.float32)[None, None, :] * m
    e[..., 3] = 1.0
    save(e, os.path.join(tmp, name + "_e_co.png"), os.path.join(OUT_DIR, name + "_e_co.paa"))
    print("retex", name, w, h)


def main():
    tmp = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "_retex")
    os.makedirs(tmp, exist_ok=True)
    D, V = PAL["r_base"], PAL["r_vein"]
    cloak = PAL["r_cloak_base"]
    moss = PAL.get("r_cloak_moss")
    retex("fange_knife_co", "vb_rift_blade", D, V, 6, 0.03, 0.85, 31, tmp, keep=0.1)
    retex("raincoat_black_co", "vb_rift_cloak", cloak, V, 5, 0.025, 0.9, 32, tmp, moss=moss)
    retex("raincoat_g_black_co", "vb_rift_cloak_g", cloak, V, 4, 0.03, 0.9, 33, tmp, moss=moss)
    retex("jerrycan_co", "vb_rift_canister", D, V, 5, 0.03, 0.85, 35, tmp, keep=0.1)


if __name__ == "__main__":
    main()
