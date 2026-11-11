# -*- coding: utf-8 -*-
"""Небо Изнанки: красноватые копии облаков Сахала для туч и грозы (аддон VB_RiftSky).

  blender.exe -b --factory-startup --python gen_rift_sky.py -- <папка_для_png>
Берёт ванильные *_sakhal_sky.paa с P:, конвертирует в PNG (ImageToPAA сам снимает перестановку каналов
*_sky: G<->1-A), подкрашивает и пишет обратно в VB_RiftSky/data/*_sky.paa (суффикс _sky обязателен:
по нему ImageToPAA снова переставляет каналы и жмёт в DXT5).
Сила оттенка растёт к грозе; тёмные низы облаков краснеют сильнее светлых верхушек.
"""
import os
import subprocess
import sys

import bpy
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.normpath(os.path.join(HERE, "..", "VB_RiftSky", "data"))
BS = chr(92)
IMAGE_TO_PAA = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools",
                            "Bin", "ImageToPAA", "ImageToPAA.exe")
SRC = "P:" + BS + "DZ" + BS + "worlds" + BS + "sakhal" + BS + "data" + BS

TINT = np.array([1.0, 0.70, 0.74])   # куда тянем цвет (розово-красный)

# исходник -> (сила в светлых местах, добавка в тёмных). Уровни облачности Сахала:
# Weather4 0.42 cloud=Stage20, Weather5 0.54 cloud=Stage21, Weather6 1.0 cloud=Stage22 farCloud=Stage30
CLOUDS = {
    "Sky_Stage20_StratoCumulus_sakhal_sky": (0.18, 0.20),
    "Sky_Stage21_StratoCumulus_sakhal_sky": (0.28, 0.25),
    "Sky_Stage22_StratoCumulus_sakhal_sky": (0.40, 0.30),
    "Sky_Stage30_Nimbostratus_sakhal_sky": (0.45, 0.35),
}


def load(path):
    im = bpy.data.images.load(path)
    w, h = im.size
    px = np.array(im.pixels[:], dtype=np.float32).reshape(h, w, 4)
    bpy.data.images.remove(im)
    return px


def save(path, px):
    h, w, _ = px.shape
    im = bpy.data.images.new("out", w, h, alpha=True)
    im.pixels[:] = np.clip(px, 0, 1).ravel().tolist()
    im.filepath_raw = path
    im.file_format = "PNG"
    im.save()
    bpy.data.images.remove(im)


def main():
    argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
    png_dir = argv[0] if argv else os.path.join(HERE, "_sky")
    os.makedirs(png_dir, exist_ok=True)
    os.makedirs(OUT_DIR, exist_ok=True)
    for name, (base, dark) in CLOUDS.items():
        src_png = os.path.join(png_dir, name + "_src.png")
        r = subprocess.run([IMAGE_TO_PAA, SRC + name + ".paa", src_png], capture_output=True, text=True)
        if not os.path.exists(src_png):
            raise RuntimeError("paa->png " + name + ": " + r.stdout + r.stderr)
        px = load(src_png)
        rgb = px[..., :3]
        lum = rgb @ np.array([0.299, 0.587, 0.114], dtype=np.float32)
        f = base + dark * np.clip((0.9 - lum) / 0.4, 0, 1)
        mult = 1 - f[..., None] + f[..., None] * TINT
        px[..., :3] = rgb * mult
        out_png = os.path.join(png_dir, name + ".png")
        save(out_png, px)
        dst = os.path.join(OUT_DIR, "vb_" + name + ".paa")
        r = subprocess.run([IMAGE_TO_PAA, out_png, dst], capture_output=True, text=True)
        if r.returncode != 0 or not os.path.exists(dst):
            raise RuntimeError("png->paa " + name + ": " + r.stdout + r.stderr)
        print("SKY", dst, os.path.getsize(dst), "mean rgb", np.round(px[..., :3].reshape(-1, 3).mean(0), 3))


main()
