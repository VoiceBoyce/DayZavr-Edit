"""Текстуры кругового меню VB_VehicleCamera.

Запуск: python gen_radial_textures.py <папка для PNG>
PNG потом переводятся в PAA через ImageToPAA и кладутся в VB_VehicleCamera/gui/textures.
Текстуры белые: цвет задаёт тема фреймворка через SetColor. Хаб запечён в своём цвете.
"""
import math
import os
import sys

from PIL import Image, ImageChops, ImageDraw, ImageFilter

DESIGN = 560          # размер меню в дизайн-пикселях холста 1920x1080
R_OUT = 250
R_IN = 100
GAP = 8               # зазор между секторами
ARC_R = 258
ARC_W = 4
ARC_INSET_DEG = 3
SIDE_H = 1024         # высота текстуры половины меню
SS = 4                # суперсэмплинг, чтобы края были гладкими


def scale() -> float:
    return SIDE_H / DESIGN


def white_with_alpha(mask: Image.Image) -> Image.Image:
    """Белые RGB везде: при фильтрации на краях нет тёмной каймы."""
    image = Image.new("RGBA", mask.size, (255, 255, 255, 0))
    image.putalpha(mask)
    return image


def half_fill() -> Image.Image:
    """Левая половина кольца, центр круга у правого края текстуры."""
    w, h = SIDE_H // 2 * SS, SIDE_H * SS
    k = scale() * SS
    cx, cy = w, h // 2
    mask = Image.new("L", (w, h), 0)
    draw = ImageDraw.Draw(mask)
    r = R_OUT * k
    draw.ellipse((cx - r, cy - r, cx + r, cy + r), fill=255)
    r = R_IN * k
    draw.ellipse((cx - r, cy - r, cx + r, cy + r), fill=0)
    inset = GAP / 2 * k
    draw.rectangle((cx - inset, 0, w, h), fill=0)
    return white_with_alpha(mask.resize((SIDE_H // 2, SIDE_H), Image.LANCZOS))


def half_arc() -> Image.Image:
    """Яркая дуга и мягкое свечение по внешнему краю левой половины."""
    w, h = SIDE_H // 2 * SS, SIDE_H * SS
    k = scale() * SS
    cx, cy = w, h // 2
    r = ARC_R * k
    box = (cx - r, cy - r, cx + r, cy + r)
    start, end = 90 + ARC_INSET_DEG, 270 - ARC_INSET_DEG
    width = round(ARC_W * k)

    line = Image.new("L", (w, h), 0)
    draw = ImageDraw.Draw(line)
    draw.arc(box, start, end, fill=255, width=width)
    for deg in (start, end):
        a = math.radians(deg)
        rm = r - width / 2
        x, y = cx + rm * math.cos(a), cy + rm * math.sin(a)
        draw.ellipse((x - width / 2, y - width / 2, x + width / 2, y + width / 2), fill=255)

    thick = Image.new("L", (w, h), 0)
    ImageDraw.Draw(thick).arc(box, start, end, fill=255, width=round(12 * k))
    glow = thick.resize((w // SS, h // SS), Image.BILINEAR).filter(ImageFilter.GaussianBlur(12)).point(lambda v: int(v * 0.42))

    line = line.resize((w // SS, h // SS), Image.LANCZOS)
    return white_with_alpha(ImageChops.lighter(line, glow))


def hub() -> Image.Image:
    size = 512
    big = size * SS
    image = Image.new("RGBA", (big, big), (16, 19, 22, 0))
    draw = ImageDraw.Draw(image)
    rim = round(2.5 * SS)
    draw.ellipse((0, 0, big - 1, big - 1), fill=(46, 50, 54, 250))
    draw.ellipse((rim, rim, big - 1 - rim, big - 1 - rim), fill=(16, 19, 22, 250))
    return image.resize((size, size), Image.LANCZOS)


def main() -> None:
    out = sys.argv[1]
    os.makedirs(out, exist_ok=True)
    fill = half_fill()
    arc = half_arc()
    fill.save(os.path.join(out, "vb_cam_sector_l_ca.png"))
    fill.transpose(Image.FLIP_LEFT_RIGHT).save(os.path.join(out, "vb_cam_sector_r_ca.png"))
    arc.save(os.path.join(out, "vb_cam_arc_l_ca.png"))
    arc.transpose(Image.FLIP_LEFT_RIGHT).save(os.path.join(out, "vb_cam_arc_r_ca.png"))
    hub().save(os.path.join(out, "vb_cam_hub_ca.png"))


if __name__ == "__main__":
    main()
