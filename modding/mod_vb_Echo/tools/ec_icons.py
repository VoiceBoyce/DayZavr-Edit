# -*- coding: utf-8 -*-
r"""Иконки слотов семейства VB_Echo (ghostIcon в CfgSlots): белые силуэты 170×170, как ванильные dayz_inventory.

  python ec_icons.py <папка атласа> [--preview=<png листа>]

Пишет <папка>/vb_echo_icons_ca.png (атлас 1024×512, ячейка 170 px) и vb_echo_icons.imageset (путь текстуры —
VB_EchoCore/gui/vb_echo_icons_ca.paa; .paa делает ImageToPAA — такой imageset работает в INT_UIFramework).
В конфиге: ghostIcon="set:vb_echo_icons image:<имя>". Рисуем в 4 раза крупнее и уменьшаем (сглаживание).
Решение пользователя 11-20: свои иконки у кристалла, у станции и у баков/трубок Echo Runner.
"""
import math
import os
import sys

from PIL import Image, ImageDraw, ImageFont

args = sys.argv[1:]
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
OUT = [a for a in args if not a.startswith("--")][0]
PREVIEW = opts.get("preview")

CELL = 170
K = 4                      # сверхвыборка
S = CELL * K
W = (255, 255, 255, 255)
T = (0, 0, 0, 0)


def canvas():
    im = Image.new("RGBA", (S, S), T)
    return im, ImageDraw.Draw(im)


def sc(v):
    return v * S / 680.0


def P(*pts):
    return [(sc(x), sc(y)) for x, y in pts]


def rect(d, x0, y0, x1, y1, fill=W, r=0):
    if r:
        d.rounded_rectangle([sc(x0), sc(y0), sc(x1), sc(y1)], radius=sc(r), fill=fill)
    else:
        d.rectangle([sc(x0), sc(y0), sc(x1), sc(y1)], fill=fill)


def line(d, a, b, w, fill=T):
    d.line(P(a, b), fill=fill, width=int(sc(w)))
    for x, y in (a, b):                                    # круглые концы
        d.ellipse([sc(x - w / 2), sc(y - w / 2), sc(x + w / 2), sc(y + w / 2)], fill=fill)


def stroke(d, pts, w, fill=W):
    for a, b in zip(pts, pts[1:]):
        line(d, a, b, w, fill)


def bezier(p0, p1, p2, p3, n=40):
    out = []
    for i in range(n + 1):
        t = i / n
        u = 1 - t
        out.append((u ** 3 * p0[0] + 3 * u * u * t * p1[0] + 3 * u * t * t * p2[0] + t ** 3 * p3[0],
                    u ** 3 * p0[1] + 3 * u * u * t * p1[1] + 3 * u * t * t * p2[1] + t ** 3 * p3[1]))
    return out


def circle(d, cx, cy, r, fill=W):
    d.ellipse([sc(cx - r), sc(cy - r), sc(cx + r), sc(cy + r)], fill=fill)


# ---------------------------------------------------------------- иконки
def crystal():
    im, d = canvas()
    pts = [(340, 60), (440, 235), (420, 485), (340, 620), (260, 485), (240, 235)]
    d.polygon(P(*pts), fill=W)
    line(d, (340, 90), (340, 590), 12)                     # ребро
    line(d, (250, 240), (340, 300), 10)
    line(d, (430, 240), (340, 300), 10)
    rect(d, 214, 330, 466, 392, T)                         # прорезь под бандаж
    rect(d, 226, 342, 454, 380, W, 8)                      # бандаж
    return im


def tank():
    im, d = canvas()
    rect(d, 70, 215, 610, 485, W, 125)                     # бак с куполами
    rect(d, 300, 168, 380, 220)                            # горловина
    rect(d, 278, 140, 402, 176, W, 6)                      # крышка
    for bx in (205, 340, 475):                             # бандажи (у бака Изнанки — светящиеся кольца)
        rect(d, bx - 30, 180, bx + 30, 520, T)
        rect(d, bx - 17, 192, bx + 17, 508, W, 6)
    return im


def pipes_side():
    im, d = canvas()
    path = bezier((80, 500), (330, 500), (330, 190), (600, 190))
    stroke(d, path, 64)
    for t in (0.18, 0.82):                                 # хомуты
        i = int(t * (len(path) - 1))
        (x0, y0), (x1, y1) = path[i - 1], path[i + 1]
        ang = math.atan2(y1 - y0, x1 - x0) + math.pi / 2
        cx, cy = path[i]
        dx, dy = math.cos(ang) * 62, math.sin(ang) * 62
        line(d, (cx - dx * 1.2 - math.cos(ang - math.pi / 2) * 26, cy - dy * 1.2 - math.sin(ang - math.pi / 2) * 26),
             (cx + dx * 1.2 - math.cos(ang - math.pi / 2) * 26, cy + dy * 1.2 - math.sin(ang - math.pi / 2) * 26), 14)
        line(d, (cx - dx * 1.2 + math.cos(ang - math.pi / 2) * 26, cy - dy * 1.2 + math.sin(ang - math.pi / 2) * 26),
             (cx + dx * 1.2 + math.cos(ang - math.pi / 2) * 26, cy + dy * 1.2 + math.sin(ang - math.pi / 2) * 26), 14)
    for (x, y) in ((70, 500), (610, 190)):                 # фланцы на концах
        rect(d, x - 16, y - 52, x + 16, y + 52, W, 6)
    return im


def pipes_engine():
    im, d = canvas()
    rect(d, 220, 420, 460, 610, W, 14)                     # блок мотора
    for y in (465, 515, 565):
        rect(d, 240, y - 7, 440, y + 7, T)
    for off, w in ((0, 46), (88, 40)):                     # две дуги трубок над мотором
        path = bezier((250 - off, 425), (230 - off, 110 - off * 0.3), (450 + off, 110 - off * 0.3), (430 + off, 425))
        stroke(d, path, w)
    rect(d, 200, 400, 480, 432, W, 6)                      # фланец коллектора
    return im


def battery():
    im, d = canvas()
    rect(d, 120, 240, 560, 590, W, 18)                     # корпус
    rect(d, 100, 215, 580, 268, W, 10)                     # крышка
    rect(d, 112, 268, 568, 282, T)                         # шов крышки
    rect(d, 170, 158, 240, 220, W, 6)                      # клеммы
    rect(d, 440, 158, 510, 220, W, 6)
    line(d, (475, 175), (475, 205), 8)                     # «плюс»
    line(d, (460, 190), (490, 190), 8)
    circle(d, 340, 425, 92, T)                             # кольцо резонатора — знак семейства
    circle(d, 340, 425, 72, W)
    circle(d, 340, 425, 50, T)
    circle(d, 340, 425, 30, W)
    return im


def cell_shape(d, cx):
    rect(d, cx - 66, 140, cx + 66, 600, W, 26)             # колба
    rect(d, cx - 80, 140, cx + 80, 200, W, 12)             # крышка
    rect(d, cx - 80, 540, cx + 80, 600, W, 12)             # дно
    rect(d, cx - 76, 200, cx + 76, 212, T)
    rect(d, cx - 76, 528, cx + 76, 540, T)
    for x in (cx - 24, cx + 24):                           # клетка
        rect(d, x - 7, 222, x + 7, 518, T)
    rect(d, cx - 66, 362, cx + 66, 376, T)
    d.arc([sc(cx - 40), sc(70), sc(cx + 40), sc(160)], 180, 360, fill=W, width=int(sc(18)))   # ручка


def arrow(d, cx, down):
    if down:
        rect(d, cx - 22, 200, cx + 22, 410, W, 6)
        d.polygon(P((cx - 72, 400), (cx + 72, 400), (cx, 510)), fill=W)
    else:
        rect(d, cx - 22, 310, cx + 22, 520, W, 6)
        d.polygon(P((cx - 72, 320), (cx + 72, 320), (cx, 210)), fill=W)


def cell():
    im, d = canvas()
    cell_shape(d, 340)
    return im


def cell_charge():
    im, d = canvas()
    cell_shape(d, 250)
    arrow(d, 510, True)
    return im


def cell_drain():
    im, d = canvas()
    cell_shape(d, 250)
    arrow(d, 510, False)
    return im


def artcase():
    """Капсула для артефактов экзо (VB_EchoClothes, слот на сервоприводе): корпус, крышка, обручи, окно с артефактом."""
    im, d = canvas()
    rect(d, 220, 170, 460, 610, W, 40)                     # корпус
    rect(d, 200, 120, 480, 190, W, 14)                     # крышка
    rect(d, 300, 90, 380, 130, W, 8)                       # колпачок
    rect(d, 212, 190, 468, 204, T)                         # шов крышки
    for y in (250, 540):                                   # обручи
        rect(d, 208, y - 7, 472, y + 7, T)
    rect(d, 286, 290, 394, 500, T, 22)                     # окно
    d.polygon(P((340, 318), (378, 395), (340, 472), (302, 395)), fill=W)   # артефакт в окне
    return im


ICONS = [("crystal", crystal, "Кристалл"), ("tank", tank, "Бак Эхо"), ("pipes", pipes_side, "Трубки (бок)"),
         ("pipes_engine", pipes_engine, "Трубки мотора"), ("battery", battery, "Аккумулятор Эхо"),
         ("cell", cell, "Энергоячейка"), ("cell_charge", cell_charge, "Ячейка: зарядка"),
         ("cell_drain", cell_drain, "Ячейка: разрядка"), ("artcase", artcase, "Капсула артефактов")]
COLS = 6
ATLAS = (1024, 512)


def main():
    os.makedirs(OUT, exist_ok=True)
    atlas = Image.new("RGBA", ATLAS, (255, 255, 255, 0))
    defs = []
    imgs = []
    for i, (name, fn, label) in enumerate(ICONS):
        ic = fn().resize((CELL, CELL), Image.LANCZOS)
        x, y = (i % COLS) * CELL, (i // COLS) * CELL
        atlas.alpha_composite(ic, (x, y))
        defs.append((name, x, y))
        imgs.append((ic, label))
    atlas.save(os.path.join(OUT, "vb_echo_icons_ca.png"))
    lines = ["ImageSetClass {", ' Name "vb_echo_icons"', " RefSize %d %d" % ATLAS, " Textures {",
             "  ImageSetTextureClass {", "   mpix 1", '   path "VB_EchoCore/gui/vb_echo_icons_ca.paa"', "  }",
             " }", " Images {"]
    for name, x, y in defs:
        lines += ["  ImageSetDefClass %s {" % name, '   Name "%s"' % name, "   Pos %d %d" % (x, y),
                  "   Size %d %d" % (CELL, CELL), "   Flags 0", "  }"]
    lines += [" }", "}", ""]
    with open(os.path.join(OUT, "vb_echo_icons.imageset"), "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))
    if PREVIEW:
        tile_w, tile_h = 230, 250
        sheet = Image.new("RGBA", (tile_w * 4 + 20, tile_h * 2 + 20), (24, 24, 24, 255))
        font = ImageFont.truetype("C:/Windows/Fonts/arial.ttf", 18)
        dr = ImageDraw.Draw(sheet)
        for i, (ic, label) in enumerate(imgs):
            x, y = 10 + (i % 4) * tile_w, 10 + (i // 4) * tile_h
            dr.rectangle([x + 22, y + 10, x + 22 + 186, y + 10 + 186], fill=(48, 48, 46, 255), outline=(170, 170, 160, 255), width=3)
            ghost = ic.copy()
            r, g, b, a = ghost.split()
            ghost = Image.merge("RGBA", (r.point(lambda v: 225), g.point(lambda v: 222), b.point(lambda v: 200),
                                         a.point(lambda v: int(v * 0.75))))
            sheet.alpha_composite(ghost, (x + 30, y + 18))
            tw = dr.textlength(label, font=font)
            dr.text((x + 115 - tw / 2, y + 210), label, fill=(220, 220, 220, 255), font=font)
        sheet.convert("RGB").save(PREVIEW)
    print("OK", len(ICONS), "иконок")


main()
