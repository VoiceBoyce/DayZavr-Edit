# -*- coding: utf-8 -*-
r"""Лист видов из рендеров er_render.py: серый фон, подписи.

  python er_sheet.py <папка с png> <лист.png> [--title=Текст] [--order=f34,r34,side,side2,front,rear,top] [--w=2400]
Несколько папок через «;» — варианты в столбцах (подпись = имя папки), строки = виды.
"""
import os
import sys

from PIL import Image, ImageDraw, ImageFont

args = sys.argv[1:]
opts = {a.split("=", 1)[0][2:]: a.split("=", 1)[1] for a in args if a.startswith("--") and "=" in a}
pos = [a for a in args if not a.startswith("--")]
SRCS, OUT = pos[0].split(";"), pos[1]
ORDER = opts.get("order", "f34,r34,side,side2,front,rear,top").split(",")
W = int(opts.get("w", 2400))
TITLE = opts.get("title", "")
NAMES = {"f34": "3/4 спереди", "r34": "3/4 сзади", "side": "бок (водитель)", "side2": "бок (пассажир)",
         "front": "спереди", "rear": "сзади", "top": "сверху",
         "vb_er_tank": "бак Изнанки (оригинальный бак ALEX + светящиеся бандажи)",
         "vb_er_pipes_l": "трубопровод левый (правый — зеркальный)", "vb_er_pipes_engine": "трубопровод мотора",
         "1_drv": "место водителя (вид из кабины)", "2_drv_night": "место водителя ночью",
         "3_pas": "с места пассажира", "4_rear": "из-за сидений", "5_top": "салон сверху (крыша срезана)",
         "6_f34": "снаружи: трубопровод мотора по крылу к нагнетателю",
         "engine": "капот и нагнетатель", "close": "борт крупно: швы с заклёпками, ржавчина",
         "close_rear": "корма крупно: баки и трубопроводы", "night": "ночь",
         "night_f34": "ночь, 3/4 спереди", "night_side": "ночь, бок", "bottom": "снизу"}
BG, FG = (92, 96, 102), (235, 235, 235)


def font(sz):
    for f in ("C:/Windows/Fonts/segoeui.ttf", "C:/Windows/Fonts/arial.ttf"):
        if os.path.exists(f):
            return ImageFont.truetype(f, sz)
    return ImageFont.load_default()


def crop(im):
    bb = im.getchannel("A").point(lambda a: 255 if a > 8 else 0).getbbox()
    return im.crop(bb) if bb else im


def on_bg(im, w, h):
    im = crop(im)
    k = min((w - 20) / im.width, (h - 20) / im.height)
    im = im.resize((max(1, int(im.width * k)), max(1, int(im.height * k))), Image.LANCZOS)
    cell = Image.new("RGB", (w, h), BG)
    cell.paste(im, ((w - im.width) // 2, (h - im.height) // 2), im)
    return cell


rows = []
if len(SRCS) == 1:          # один вариант: сетка 2 столбца
    files = [(v, os.path.join(SRCS[0], v + ".png")) for v in ORDER if os.path.exists(os.path.join(SRCS[0], v + ".png"))]
    cw = W // 2
    ch = int(cw * 0.62)
    nrow = (len(files) + 1) // 2
    head = 70 if TITLE else 0
    sheet = Image.new("RGB", (W, head + nrow * (ch + 40)), BG)
    d = ImageDraw.Draw(sheet)
    if TITLE:
        d.text((20, 15), TITLE, fill=FG, font=font(38))
    for i, (v, f) in enumerate(files):
        x, y = (i % 2) * cw, head + (i // 2) * (ch + 40)
        sheet.paste(on_bg(Image.open(f).convert("RGBA"), cw, ch), (x, y + 40))
        d.text((x + 16, y + 4), NAMES.get(v, v), fill=FG, font=font(28))
else:                       # варианты в столбцах
    n = len(SRCS)
    cw = W // n
    views = [v for v in ORDER if all(os.path.exists(os.path.join(s, v + ".png")) for s in SRCS)]
    ch = int(cw * 0.6)
    head = 70 + (60 if TITLE else 0)
    sheet = Image.new("RGB", (W, head + len(views) * (ch + 36)), BG)
    d = ImageDraw.Draw(sheet)
    if TITLE:
        d.text((20, 15), TITLE, fill=FG, font=font(38))
    for j, s in enumerate(SRCS):
        d.text((j * cw + 16, head - 52), os.path.basename(s.rstrip("/\\")), fill=FG, font=font(34))
    for i, v in enumerate(views):
        y = head + i * (ch + 36)
        d.text((16, y + 2), NAMES.get(v, v), fill=FG, font=font(26))
        for j, s in enumerate(SRCS):
            sheet.paste(on_bg(Image.open(os.path.join(s, v + ".png")).convert("RGBA"), cw, ch), (j * cw, y + 36))
sheet.save(OUT)
print("SHEET", OUT, sheet.size)
