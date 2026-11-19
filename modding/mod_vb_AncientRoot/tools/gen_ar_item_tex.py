# -*- coding: utf-8 -*-
"""Текстуры предметов «Древнего корня» для превью инвентаря (2026-11-19).

Превью инвентаря не рисует свечение (emmisive + карта Stage8), поэтому предметы, у которых золото жил живёт за счёт
свечения, в инвентаре выглядели тёмной корой (каталог ошибок E109). Здесь из текущих текстур делаются отдельные
текстуры ТОЛЬКО для предметов:
  vb_ar_i_crust_co / vb_ar_i_vein_co — кора и жилы с «запечённым» свечением (цвет + доля карты свечения _e_co);
  vb_ar_i_<проп>_co                   — тёмные материалы изделий (кора, дерево, мешковина…), чуть светлее.
Рощи, корни и сердце в мире берут прежние vb_ar_crust_co / vb_ar_vein_co и не меняются. Какую текстуру берёт
модель предмета — ITEM_TEX в gen_ar_veins.py (модели пересобрать: gen_ar_veins.py --only=...).
Исходные текстуры не трогаются, поэтому повторный запуск даёт тот же результат.

  <python из Blender> gen_ar_item_tex.py [--preview <папка>]   — --preview: ещё и PNG «было | стало» для проверки
"""
import os
import struct
import subprocess
import sys
import tempfile
import zlib

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
DATA_DIR = os.path.normpath(os.path.join(HERE, "..", "VB_AncientRoot", "data"))
IMAGE_TO_PAA = r"D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\ImageToPAA\ImageToPAA.exe"

# кора/жилы: итог = 1 - (1 - цвет) ** K + свечение * GLOW (sRGB, обрезка до 1): тени коры светлеют сильнее светов
BAKE = {
    "vb_ar_i_crust_co": ("vb_ar_crust_co", "vb_ar_crust_e_co", 1.9, 0.75),
    "vb_ar_i_vein_co": ("vb_ar_vein_co", "vb_ar_vein_e_co", 1.3, 0.45),
}
# тёмные пропы: итог = 1 - (1 - цвет) ** K (поднимает тени сильнее светов, цвет не выцветает)
LIFT = {"bark": 1.55, "wood": 1.45, "burlap": 1.35, "cloth": 1.35, "herb": 1.4, "sporecloth": 1.35}


def read_tga(path):
    data = open(path, "rb").read()
    idlen, cmap, itype = data[0], data[1], data[2]
    w, h, bpp, desc = struct.unpack("<HHBB", data[12:18])
    if itype != 2 or bpp not in (24, 32) or cmap:
        raise ValueError("нужен несжатый TGA 24/32 бит: %s" % path)
    ch = bpp // 8
    px = np.frombuffer(data, np.uint8, w * h * ch, 18 + idlen).reshape(h, w, ch).astype(np.float32) / 255.0
    return data[:18 + idlen], px


def write_tga(path, header, px):
    out = (np.clip(px, 0, 1) * 255.0 + 0.5).astype(np.uint8)
    with open(path, "wb") as f:
        f.write(header)
        f.write(out.tobytes())


def write_png(path, rgb):
    """PNG RGB без фильтров (для --preview)."""
    h, w, _ = rgb.shape
    raw = b"".join(b"\x00" + (np.clip(rgb[y], 0, 1) * 255 + 0.5).astype(np.uint8).tobytes() for y in range(h))
    def chunk(t, b):
        return struct.pack(">I", len(b)) + t + b + struct.pack(">I", zlib.crc32(t + b) & 0xffffffff)
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
                + chunk(b"IDAT", zlib.compress(raw, 6)) + chunk(b"IEND", b""))


def paa_to_tga(name, tmp):
    src = os.path.join(DATA_DIR, name + ".paa")
    dst = os.path.join(tmp, name + ".tga")
    r = subprocess.run([IMAGE_TO_PAA, src, dst], capture_output=True, text=True)
    if r.returncode != 0 or not os.path.isfile(dst):
        raise RuntimeError("ImageToPAA %s: %s" % (name, (r.stdout + r.stderr)[-300:]))
    return read_tga(dst)


def tga_to_paa(name, header, px, tmp):
    src = os.path.join(tmp, name + ".tga")
    write_tga(src, header, px)
    dst = os.path.join(DATA_DIR, name + ".paa")
    r = subprocess.run([IMAGE_TO_PAA, src, dst], capture_output=True, text=True)
    if r.returncode != 0 or not os.path.isfile(dst):
        raise RuntimeError("ImageToPAA %s: %s" % (name, (r.stdout + r.stderr)[-300:]))


def lum(px):
    # TGA хранит BGR(A): яркость по весам sRGB
    return float((0.0722 * px[..., 0] + 0.7152 * px[..., 1] + 0.2126 * px[..., 2]).mean() * 255)


def main():
    preview = sys.argv[sys.argv.index("--preview") + 1] if "--preview" in sys.argv else None
    if preview:
        os.makedirs(preview, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmp:
        jobs = []
        for out, (col, glow, base, gain) in BAKE.items():
            hc, c = paa_to_tga(col, tmp)
            _, e = paa_to_tga(glow, tmp)
            if e.shape[:2] != c.shape[:2]:
                raise ValueError("%s и %s разного размера" % (col, glow))
            res = c.copy()
            res[..., :3] = 1.0 - (1.0 - c[..., :3]) ** base + e[..., :3] * gain
            jobs.append((out, hc, c, res))
        for m, k in LIFT.items():
            src = "vb_ar_p_%s_co" % m
            hc, c = paa_to_tga(src, tmp)
            res = c.copy()
            res[..., :3] = 1.0 - (1.0 - c[..., :3]) ** k
            jobs.append(("vb_ar_i_%s_co" % m, hc, c, res))
        for out, hc, c, res in jobs:
            tga_to_paa(out, hc, res, tmp)
            print("%-22s яркость %5.1f -> %5.1f" % (out, lum(c), lum(np.clip(res, 0, 1))))
            if preview:
                step = max(1, c.shape[1] // 256)
                a, b = c[::step, ::step, 2::-1], np.clip(res[::step, ::step, 2::-1], 0, 1)
                write_png(os.path.join(preview, out + ".png"), np.concatenate([a, b], axis=1))


if __name__ == "__main__":
    main()
