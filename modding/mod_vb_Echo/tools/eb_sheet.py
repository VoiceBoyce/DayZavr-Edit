# -*- coding: utf-8 -*-
r"""Листы концептов устройств базы: 3×3, день — на светлом фоне, ночь — на тёмном, с подписями.

  python eb_sheet.py <папка рендеров> <папка вывода>
Выход: <папка>/лист_день.png, <папка>/лист_ночь.png.
"""
import os
import sys

from PIL import Image, ImageDraw, ImageFont

SRC, OUT = sys.argv[1], sys.argv[2]
ORDER = [("wind", "Ветряк"), ("solar", "Солнечная панель"), ("furnace", "Резонансная печь"),
         ("converter", "Резонансный преобразователь"), ("storage", "Эхо-накопитель"), ("station", "Зарядная станция"),
         ("pad", "Беспроводная платформа"), ("relay", "Ретранслятор"), ("crystal", "Энергетический кристалл"),
         ("kit", "Набор (одна модель на все)")]
CELL = 560
LABEL = 44


def font(size):
    for f in ("C:/Windows/Fonts/segoeui.ttf", "C:/Windows/Fonts/arial.ttf"):
        if os.path.exists(f):
            return ImageFont.truetype(f, size)
    return ImageFont.load_default()


def sheet(mode, bg, fg):
    rows = (len(ORDER) + 2) // 3
    W, H = CELL * 3, (CELL + LABEL) * rows
    out = Image.new("RGB", (W, H), bg)
    draw = ImageDraw.Draw(out)
    fnt = font(26)
    for i, (key, title) in enumerate(ORDER):
        p = os.path.join(SRC, "%s_%s.png" % (key, mode))
        if not os.path.exists(p):
            continue
        im = Image.open(p).convert("RGBA").resize((CELL, CELL), Image.LANCZOS)
        x, y = (i % 3) * CELL, (i // 3) * (CELL + LABEL)
        tile = Image.new("RGBA", (CELL, CELL), bg + (255,))
        tile.alpha_composite(im)
        out.paste(tile.convert("RGB"), (x, y + LABEL))
        tw = draw.textlength(title, font=fnt)
        draw.text((x + (CELL - tw) / 2, y + 8), title, fill=fg, font=fnt)
    os.makedirs(OUT, exist_ok=True)
    path = os.path.join(OUT, "лист_%s.png" % ("день" if mode == "day" else "ночь"))
    out.save(path)
    print("OK", path)


def tiers_sheet(mode, bg, fg):
    """Уровни Mk I–IV: строки — устройства, столбцы — уровни (eb_<имя>_mk<N>.p3d из eb_models.py --tier=N)."""
    rows = [("wind", "Ветряк (источник: весь неон — цвет уровня)"), ("storage", "Накопитель (Эхо красное, метка — цвет уровня)"),
            ("kit", "Набор (цвет уровня устройства внутри)")]
    cell = 420
    head = 40
    W, H = cell * 4, (cell + head) * len(rows) + head
    out = Image.new("RGB", (W, H), bg)
    draw = ImageDraw.Draw(out)
    fnt = font(24)
    for c, t in enumerate(("Mk I", "Mk II", "Mk III", "Mk IV")):
        tw = draw.textlength(t, font=fnt)
        draw.text((c * cell + (cell - tw) / 2, 6), t, fill=fg, font=fnt)
    for r, (key, title) in enumerate(rows):
        y0 = head + r * (cell + head)
        draw.text((12, y0 + 6), title, fill=fg, font=fnt)
        for c in range(4):
            nm = key if c == 0 else "%s_mk%d" % (key, c + 1)
            p = os.path.join(SRC, "%s_%s.png" % (nm, mode))
            if not os.path.exists(p):
                continue
            im = Image.open(p).convert("RGBA").resize((cell, cell), Image.LANCZOS)
            tile = Image.new("RGBA", (cell, cell), bg + (255,))
            tile.alpha_composite(im)
            out.paste(tile.convert("RGB"), (c * cell, y0 + head))
    path = os.path.join(OUT, "лист_уровни_%s.png" % ("день" if mode == "day" else "ночь"))
    out.save(path)
    print("OK", path)


sheet("day", (176, 180, 186), (20, 20, 24))
sheet("night", (16, 18, 24), (225, 225, 230))
tiers_sheet("day", (176, 180, 186), (20, 20, 24))
tiers_sheet("night", (16, 18, 24), (225, 225, 230))
