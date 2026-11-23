# -*- coding: utf-8 -*-
r"""Лист концепта экзоскелета Эхо из рендеров ec_concept.py (обычный Python + Pillow).

  python ec_concept_sheet.py <папка концепт\одежда>

Ждёт файлы: exo_base_front34/back34, exo_echo_front34/back34 (день), exo_echo_front34_night/back34_night,
крупно\exo_echo_back34_night (сервопривод), крупно\exo_echo_front34_night (маска). Пишет exo_concept_sheet.png.
"""
import os
import sys

from PIL import Image, ImageDraw, ImageFont

D = sys.argv[1]
FONT = os.path.join(os.environ.get("WINDIR", "C:" + chr(92) + "Windows"), "Fonts", "arial.ttf")
BOLD = os.path.join(os.environ.get("WINDIR", "C:" + chr(92) + "Windows"), "Fonts", "arialbd.ttf")
f_t = ImageFont.truetype(BOLD, 40)
f_h = ImageFont.truetype(BOLD, 26)
f_s = ImageFont.truetype(FONT, 21)
BG = (18, 19, 23)
FG = (225, 225, 230)
RED = (255, 60, 70)


def tile(name, box=(150, 90, 850, 1160), w=560):
    im = Image.open(os.path.join(D, name)).convert("RGB").crop(box)
    return im.resize((w, int(im.height * w / im.width)))


cols = [("Сейчас (OOTS)", "exo_base_%s.png"), ("Эхо — день", "exo_echo_%s.png"), ("Эхо — ночь", "exo_echo_%s_night.png")]
rows = ["front34", "back34"]
tw = 560
th = tile(cols[0][1] % rows[0]).height
pad = 20
head = 110
W = pad + 3 * (tw + pad)
close_h = 640
legend_h = 700
H = head + 2 * (th + 44 + pad) + close_h + 44 + legend_h
sheet = Image.new("RGB", (W, H), BG)
d = ImageDraw.Draw(sheet)
d.text((pad, 24), "VB_EchoClothes — концепт экзоскелета Эхо (этап 3)", font=f_t, fill=FG)
d.text((pad, 74), "палитра «чёрная броня Эхо»: графит, сатиновый металл, стальные потёртости; свет — только энергия Эхо (красный), синтез — бирюзовый",
       font=f_s, fill=(170, 170, 180))
y = head
for r in rows:
    x = pad
    for title, pat in cols:
        d.text((x, y + 6), title + ("" if r == "front34" else " · сзади"), font=f_h, fill=FG)
        sheet.paste(tile(pat % r), (x, y + 44))
        x += tw + pad
    y += th + 44 + pad
# крупные планы
d.text((pad, y + 6), "Посадка ячеек: гнёзда вместо банок", font=f_h, fill=FG)
d.text((pad + tw + pad, y + 6), "Ноги: светящиеся трубки вместо гофры", font=f_h, fill=FG)
d.text((pad + 2 * (tw + pad), y + 6), "Маска: линзы", font=f_h, fill=FG)
sv = tile(os.path.join("крупно", "exo_echo_back_night.png"), box=(360, 290, 640, 590), w=tw)
sv = sv.crop((0, 0, sv.width, min(sv.height, close_h)))
sheet.paste(sv, (pad, y + 44))
lg = tile(os.path.join("крупно", "exo_echo_back34_night.png"), box=(360, 520, 680, 900), w=tw)
lg = lg.crop((0, 0, lg.width, min(lg.height, close_h)))
sheet.paste(lg, (pad + tw + pad, y + 44))
mk = tile(os.path.join("крупно", "exo_echo_front34_night.png"), box=(170, 150, 550, 520), w=tw)
mk = mk.crop((0, 0, mk.width, min(mk.height, close_h)))
sheet.paste(mk, (pad + 2 * (tw + pad), y + 44))
y += close_h + 44 + pad
legend = [
    ("Логика света", "светится только то, по чему идёт энергия Эхо: ячейки → перемычка → центральный световод → "
                     "поясной блок (индикатор) → силовые кабели к рукам и бёдрам. Гофра — гидравлика, остаётся тёмной."),
    ("Сервопривод", "две энергоячейки на спине видны в слотах (ядро горит при заряде, кольцо — уровень), "
                    "наклонены «галочкой» на 12° наружу и отодвинуты от спины на 2 см — не входят в плиту и трубу; ржавые банки "
                    "OOTS под ними убраны, вместо них свои гнёзда: стакан с контактным кольцом (горит цветом "
                    "ячейки), стальной бандаж, две направляющие, верхний фиксатор с защёлкой, опора с пружиной до "
                    "пояса; "
                    "трубка между ячейками — светящаяся перемычка; центральная труба — световод вместо тканевого "
                    "рукава; плетёные кабели рук светятся; на ногах гофра заменена гладкими светящимися "
                    "трубками с хомутами (на бёдрах — одна из двух, вторая остаётся гидравликой; на голенях — "
                    "обе); гофра на плечах не светится."),
    ("Контейнер артефактов", "своя капсула на левом бедре (слот на сервоприводе): тёмный корпус, ржавые обручи, "
                             "узкое окно — виден свет артефакта внутри, неоновое кольцо на крышке."),
    ("Маска и комплект", "череп — графит, линзы светятся красным (свои линзы в глазницах); куртка, штаны, жилет, "
                         "перчатки, ботинки — «чёрная броня» без ржавчины (графит, сатиновый металл, стальные "
                         "потёртости, тонкая бордовая окантовка), знак Эхо на груди, красная изолента на перчатках. "
                         "Шлем — отдельный рендер позже."),
]
for k, v in legend:
    d.text((pad, y), k, font=f_h, fill=RED)
    words, line, yy = v.split(" "), "", y + 34
    for wd in words:
        t = (line + " " + wd).strip()
        if d.textlength(t, font=f_s) > W - 2 * pad - 20:
            d.text((pad + 20, yy), line, font=f_s, fill=FG)
            yy += 27
            line = wd
        else:
            line = t
    d.text((pad + 20, yy), line, font=f_s, fill=FG)
    y = yy + 40
sheet = sheet.crop((0, 0, W, min(H, y + 10)))
out = os.path.join(D, "exo_concept_sheet.png")
sheet.save(out)
print("OK", out, sheet.size)
