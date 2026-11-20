# -*- coding: utf-8 -*-
r"""Лист концепта v2 (11-20): станция с вложениями (день/ночь), энергоячейки по уровням, иконки слотов.

  python eb_sheet_v2.py <папка концепт\v2>
Вход: <папка>/рендеры/*.png (eb_render.py), <папка>/иконки/лист_иконок.png (ec_icons.py). Выход: <папка>/лист_v2.png.
"""
import os
import sys

from PIL import Image, ImageDraw, ImageFont

D = sys.argv[1]
R = os.path.join(D, "рендеры")
BG = (30, 31, 34, 255)
font = ImageFont.truetype("C:/Windows/Fonts/arial.ttf", 22)
title = ImageFont.truetype("C:/Windows/Fonts/arialbd.ttf", 28)


def load(name, size):
    im = Image.open(os.path.join(R, name)).convert("RGBA")
    bg = Image.new("RGBA", im.size, (52, 54, 58, 255) if "_day" in name else (14, 15, 20, 255))
    bg.alpha_composite(im)
    return bg.resize((size, size), Image.LANCZOS)


W = 1900
st = [load("station_day.png", 820), load("station_night.png", 820)]
cells = [("cell", "Mk I · 64 kE"), ("cell_mk2", "Mk II · 128 kE"), ("cell_mk3", "Mk III · 256 kE"),
         ("cell_mk4", "Mk IV · 512 kE"), ("cell_fusion", "Синтез (?)")]
icons = Image.open(os.path.join(D, "иконки", "лист_иконок.png")).convert("RGBA")
H = 70 + 820 + 70 + 360 + 360 + 70 + icons.size[1] + 40
sheet = Image.new("RGBA", (W, H), BG)
dr = ImageDraw.Draw(sheet)
y = 20
dr.text((40, y), "Зарядная станция v2: баки стоя, аккумулятор, ячейки — 2 на зарядку (▼) и 2 на разрядку (▲)", fill=(235, 235, 235), font=title)
y += 50
sheet.alpha_composite(st[0], (40, y))
sheet.alpha_composite(st[1], (W - 40 - 820, y))
y += 820 + 30
dr.text((40, y), "Энергоячейка: одна модель, кольцо на крышке — уровень, ядро — заряд (день / ночь)", fill=(235, 235, 235), font=title)
y += 45
cw = (W - 80) // len(cells)
for i, (nm, label) in enumerate(cells):
    x = 40 + i * cw
    sheet.alpha_composite(load(nm + "_day.png", 330), (x + (cw - 330) // 2, y))
    sheet.alpha_composite(load(nm + "_night.png", 330), (x + (cw - 330) // 2, y + 340))
    tw = dr.textlength(label, font=font)
    dr.text((x + (cw - tw) / 2, y + 680), label, fill=(225, 225, 225), font=font)
y += 720
dr.text((40, y), "Иконки слотов (свои, вместо «бочки» и «верёвки»)", fill=(235, 235, 235), font=title)
y += 45
sheet.alpha_composite(icons, ((W - icons.size[0]) // 2, y))
out = os.path.join(D, "лист_v2.png")
sheet.convert("RGB").save(out)
print("OK", out, sheet.size)
