# -*- coding: utf-8 -*-
r"""Лист расцветок экзоскелета (VB_EchoClothes, 11-25) из рендеров ec_concept.py (обычный Python + Pillow).

  python ec_colorway_sheet.py <папка рендеров> <выходной png>

В папке рендеров — подпапки r_echo, r_ranger, r_coyote, r_arctic с exo_base_front34.png и exo_base_back34.png:
  ec_retex.py -- jacket,pants,vest,gloves,boots,mask --palette=<п> --preview=<папка>\tex_<п> --nopaa
  ec_concept.py -- <папка>\r_<п> --variant=base --tex_echo --views=front34,back34 --built=<exo_set PBO>
      --tex_pal=<п> --tex_dir=<папка>\tex_<п>          (для echo — без --tex_pal: готовые *_echo_co.paa)
Рамка кадра — общая для всех рендеров (по непустым пикселям), масштаб одинаковый.
Путь с кириллицей — запускать из PowerShell, не из Git Bash (E127).
"""
import os
import sys

from PIL import Image, ImageChops, ImageDraw, ImageFont

D, OUT = sys.argv[1], sys.argv[2]
FONT = os.path.join(os.environ.get("WINDIR", "C:" + chr(92) + "Windows"), "Fonts", "arial.ttf")
BOLD = os.path.join(os.environ.get("WINDIR", "C:" + chr(92) + "Windows"), "Fonts", "arialbd.ttf")
f_t = ImageFont.truetype(BOLD, 40)
f_h = ImageFont.truetype(BOLD, 28)
f_s = ImageFont.truetype(FONT, 20)
BG = (18, 19, 23)
FG = (225, 225, 230)
DIM = (150, 152, 160)

COLS = [
    ("echo", "Чёрная (сейчас)", "ткань в цвет шлема", (25, 26, 28)),
    ("ranger", "«Рейнджер»", "тёмная олива — лес", (40, 45, 32)),
    ("coyote", "«Койот»", "песочно-коричневая — степь, пустыня, осень", (112, 92, 64)),
    ("arctic", "«Арктика»", "светло-серая — зима, Сахал", (153, 158, 163)),
]
ROWS = [("front34", "спереди"), ("back34", "сзади")]

ims = {}
box = None
for key, *_ in COLS:
    for view, _ in ROWS:
        im = Image.open(os.path.join(D, "r_" + key, "exo_base_%s.png" % view)).convert("RGB")
        ims[key, view] = im
        bg = Image.new("RGB", im.size, im.getpixel((2, 2)))
        diff = ImageChops.difference(im, bg).convert("L").point(lambda v: 255 if v > 14 else 0)
        b = diff.getbbox()
        box = b if box is None else (min(box[0], b[0]), min(box[1], b[1]), max(box[2], b[2]), max(box[3], b[3]))
m = 20
box = (max(0, box[0] - m), max(0, box[1] - m), box[2] + m, box[3] + m)
tw = 430
th = int((box[3] - box[1]) * tw / (box[2] - box[0]))
pad = 18
head = 120
col_head = 92
W = pad + len(COLS) * (tw + pad)
H = head + col_head + len(ROWS) * (th + pad) + 70
sheet = Image.new("RGB", (W, H), BG)
d = ImageDraw.Draw(sheet)
d.text((pad, 24), "Экзоскелет — расцветки ткани", font=f_t, fill=FG)
d.text((pad, 76), "Меняется только ткань (куртка, штаны, разгрузка, перчатки, вставки ботинок, балаклава). "
                  "Пластины, металл, окантовка, знак, шлем и сервопривод — общие.", font=f_s, fill=DIM)
for ci, (key, title, sub, sw) in enumerate(COLS):
    x = pad + ci * (tw + pad)
    d.rectangle((x, head + 6, x + 34, head + 40), fill=sw, outline=(90, 92, 98))
    d.text((x + 46, head + 4), title, font=f_h, fill=FG)
    d.text((x, head + 50), sub, font=f_s, fill=DIM)
    for ri, (view, _) in enumerate(ROWS):
        y = head + col_head + ri * (th + pad)
        sheet.paste(ims[key, view].crop(box).resize((tw, th), Image.LANCZOS), (x, y))
d.text((pad, H - 50), "Рендер Blender (Cycles) по текстурам расцветок; в игре свет и туман другие — финальная проверка "
                      "на стенде.", font=f_s, fill=DIM)
sheet.save(OUT)
print("лист", OUT, sheet.size)
