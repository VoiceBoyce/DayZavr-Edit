# -*- coding: utf-8 -*-
r"""Лист концепта материалов крафта (14 предметов): днём сеткой, светящиеся — ещё и ночью.

  python em_sheet.py <папка концепт\материалы>
Вход: <папка>/рендеры/em_<имя>_day|night.png (eb_render.py). Выход: <папка>/лист_материалы.png.
"""
import os
import sys

from PIL import Image, ImageDraw, ImageFont

D = sys.argv[1]
R = os.path.join(D, "рендеры")
BG = (30, 31, 34, 255)
font = ImageFont.truetype("C:/Windows/Fonts/arial.ttf", 21)
small = ImageFont.truetype("C:/Windows/Fonts/arial.ttf", 17)
title = ImageFont.truetype("C:/Windows/Fonts/arialbd.ttf", 28)
ITEMS = [("armorplate", "Бронепластина", "спавн"), ("carbon", "Карбоновый профиль", "спавн"),
         ("servo", "Сервомотор", "спавн"), ("hydraulic", "Гидроцилиндр", "спавн"),
         ("spool", "Бухта медного провода", "спавн"), ("shock", "Амортизатор", "спавн"), ("ecu", "ЭБУ", "спавн"),
         ("damper", "Демпферная вставка", "крафт"), ("exoctrl", "Блок управления экзоскелетом", "крафт"),
         ("conduit", "Проводник Эхо", "крафт"), ("coil", "Резонансная катушка", "крафт"),
         ("frame", "Силовая рама", "крафт"), ("panel", "Кузовная панель", "крафт"), ("glass", "Бронестекло", "крафт")]
NIGHT = []      # ночной ряд пользователю не нужен (11-24: «зачем тут ночные предметы») — только дневная сетка
SIZE = {}
if len(sys.argv) > 2:                      # габариты из вывода em_models.py (необязательно)
    for line in open(sys.argv[2], encoding="utf-8"):
        if line.startswith("OK "):
            p = line.split()
            SIZE[p[1]] = line.split("см ")[1].split("]")[0] + "]"


def load(name, mode, size):
    im = Image.open(os.path.join(R, "em_%s_%s.png" % (name, mode))).convert("RGBA")
    bg = Image.new("RGBA", im.size, (52, 54, 58, 255) if mode == "day" else (14, 15, 20, 255))
    bg.alpha_composite(im)
    return bg.resize((size, size), Image.LANCZOS)


COLS, S, PAD = 5, 340, 24
W = COLS * S + (COLS + 1) * PAD
rows = (len(ITEMS) + COLS - 1) // COLS
H = 70 + rows * (S + 70) + (60 + S + 70 if NIGHT else 10)
sheet = Image.new("RGBA", (W, H), BG)
dr = ImageDraw.Draw(sheet)
dr.text((PAD, 20), "Материалы крафта VB_EchoMaterials — концепт (днём)", fill=(235, 235, 235), font=title)
y0 = 70
for i, (nm, label, src) in enumerate(ITEMS):
    x = PAD + (i % COLS) * (S + PAD)
    y = y0 + (i // COLS) * (S + 70)
    sheet.alpha_composite(load(nm, "day", S), (x, y))
    tw = dr.textlength(label, font=font)
    if tw > S:
        dr.text((x + (S - dr.textlength(label, font=small)) / 2, y + S + 6), label, fill=(235, 235, 235), font=small)
    else:
        dr.text((x + (S - tw) / 2, y + S + 6), label, fill=(235, 235, 235), font=font)
    col = (120, 200, 140) if src == "спавн" else (230, 150, 90)
    sub = src
    tw = dr.textlength(sub, font=small)
    dr.text((x + (S - tw) / 2, y + S + 34), sub, fill=col, font=small)
y = y0 + rows * (S + 70) + 10
if NIGHT:
    dr.text((PAD, y), "Ночью: светится только кристалл Эхо, провод и блок без энергии серые", fill=(235, 235, 235),
        font=title)
y += 50
for i, nm in enumerate(NIGHT):
    x = PAD + i * (S + PAD)
    sheet.alpha_composite(load(nm, "night", S), (x, y))
    label = dict((a, b) for a, b, _ in ITEMS)[nm]
    f = font if dr.textlength(label, font=font) <= S else small
    dr.text((x + (S - dr.textlength(label, font=f)) / 2, y + S + 6), label, fill=(225, 225, 225), font=f)
out = os.path.join(D, "лист_материалы.png")
sheet.convert("RGB").save(out)
print("OK", out, sheet.size)
