# -*- coding: utf-8 -*-
r"""Контактный лист прозрачных PNG на сером фоне + проверка альфы углов.
blender.exe -b --factory-startup --python sheet.py -- <папка с png> <sheet.png> [--cols=8] [--cell=260]
Порядок — по алфавиту имён файлов (печатается с номерами)."""
import glob
import os
import sys

import bpy
import numpy as np

args = sys.argv[sys.argv.index("--") + 1:]
opts = {a.split("=", 1)[0][2:]: a.split("=", 1)[1] for a in args if a.startswith("--") and "=" in a}
pos = [a for a in args if not a.startswith("--")]
SRC, OUT = pos[0], pos[1]
COLS, CELL = int(opts.get("cols", 8)), int(opts.get("cell", 260))
files = sorted(glob.glob(os.path.join(SRC, "*.png")), key=lambda p: os.path.basename(p).lower())
rows = (len(files) + COLS - 1) // COLS
sheet = np.zeros((rows * CELL, COLS * CELL, 4), np.float32)
sheet[..., :3] = 0.45
sheet[..., 3] = 1
bad = []
for i, f in enumerate(files):
    im = bpy.data.images.load(f)
    w, h = im.size
    px = np.array(im.pixels[:], np.float32).reshape(h, w, 4)
    corners = max(px[0, 0, 3], px[0, -1, 3], px[-1, 0, 3], px[-1, -1, 3])
    if corners > 0.01 or im.channels != 4:
        bad.append(os.path.basename(f))
    sm = px[(np.arange(CELL) * h / CELL).astype(int)][:, (np.arange(CELL) * w / CELL).astype(int)]
    a = sm[..., 3:4]
    r, c = i // COLS, i % COLS
    y0 = (rows - 1 - r) * CELL
    cell = sheet[y0:y0 + CELL, c * CELL:(c + 1) * CELL]
    cell[..., :3] = sm[..., :3] * a + cell[..., :3] * (1 - a)
    print("SHEET %2d %s %dx%d" % (i + 1, os.path.basename(f), w, h))
new = bpy.data.images.new("sheet", COLS * CELL, rows * CELL, alpha=True)
new.pixels.foreach_set(sheet.ravel())
new.filepath_raw = OUT
new.file_format = "PNG"
new.save()
print("SHEETDONE", len(files), "bad alpha:", bad)
