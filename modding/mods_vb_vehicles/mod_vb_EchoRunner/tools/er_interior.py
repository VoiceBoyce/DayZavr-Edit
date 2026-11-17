# -*- coding: utf-8 -*-
r"""Салон Echo Runner (этап 4): каркас, красный неон, экран с панелью тумблеров, ящик между сиденьями,
колба Изнанки, кабели. Вызывается из er_parts.py: build(ctx).

Оси Blender (нос -Y, верх +Z, водитель +X). Замеры салона ALEX после подъёма (лучи, 2026-11-17):
потолок изнутри z 1.50–1.52; подушки сидений y -0.05..0.45, спинки y 0.5–0.6, подголовники до z 1.11;
тоннель между сиденьями |x| < 0.13, верх z 0.58–0.65; торпедо: лицевая панель y -0.34..-0.44, верх z 1.14;
двери изнутри |x| 0.84; средние стойки y 0.70 (изнутри |x| 0.74); у ALEX есть поперечины под крышей над
лобовым (y -0.09..0.03, z 1.42–1.49) и у средних стоек (y 0.67–0.72, z 1.45–1.52).
Прокси, которым нельзя мешать: оружие (0.10, 0.66, 0.64), рюкзак (-0.32, 1.38, 0.97), гранаты на торпедо
пассажира (x -0.22..-0.56, y -0.54, z 0.80); голова водителя ~ (0.43, 0.30, 1.22).
"""
import math

from mathutils import Vector

from er_geom import Part, V, bar, box, cyl, ring, tube, catmull

INT_LODS = [1, 2, 1100, 1201]
EYE = Vector((0.43, 0.32, 1.22))


def build(ctx):
    P = ctx.PARTS
    hit = ctx.hit

    # ------------------------------------------------ каркас безопасности
    cage = Part("cage", "body", [1, 2, 3, 1100, 1201], ["er_cage"])
    R = 0.042
    for s in (1, -1):
        top_f = V(s * 0.58, -0.04, 1.455)          # концы поперечины ALEX над лобовым
        top_r = V(s * 0.57, 0.695, 1.475)          # концы поперечины ALEX у средних стоек
        bar(cage, top_f, top_r, R, "steel")                                  # боковая дуга под крышей
        foot_r = V(s * 0.70, 0.72, 0.47)
        mid_r = V(s * 0.72, 0.72, 1.10)
        bar(cage, top_r, mid_r, R, "steel")                                   # стойка главной дуги
        bar(cage, mid_r, foot_r, R, "steel")
        box(cage, foot_r + V(0, 0, 0.004), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.05, 0.05, 0.006, "steel")
        dash = V(s * 0.70, -0.37, 1.02)                                       # передняя труба вдоль стойки
        bar(cage, dash, top_f, R * 0.9, "steel")
        box(cage, dash, V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.035, 0.03, 0.02, "steel")
    bar(cage, V(-0.72, 0.72, 1.02), V(0.72, 0.72, 1.02), R, "steel")         # поперечина ремней
    bar(cage, V(-0.71, 0.72, 0.55), V(0.57, 0.72, 1.44), R * 0.9, "steel")   # диагональ главной дуги
    P.append(cage)

    # ------------------------------------------------ красный неон
    neon = Part("int_neon", "body", INT_LODS, ["er_int", "neon"])
    for s in (1, -1):
        for y0, y1 in ((0.02, 0.62),):
            box(neon, V(s * 0.44, (y0 + y1) / 2, 1.487), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1),
                0.011, (y1 - y0) / 2, 0.005, "neon")
    box(neon, V(-0.42, -0.395, 0.80), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.30, 0.008, 0.006, "neon")
    P.append(neon)
    rail = Part("int_neon_rail", "body", INT_LODS, ["er_int"])
    for s in (1, -1):
        box(rail, V(s * 0.44, 0.32, 1.494), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.018, 0.31, 0.005, "house")
    box(rail, V(-0.42, -0.39, 0.808), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.31, 0.014, 0.006, "house")
    P.append(rail)
    # полосы на дверях изнутри (в моделях дверей)
    for s in (1, -1):
        tgt = "door_driver" if s > 0 else "door_codriver"
        dn = Part("door_neon_%d" % s, tgt, [1, 2], ["neon"])
        dh = Part("door_neon_rail_%d" % s, tgt, [1, 2], ["er_int"])
        pts = []
        for k in range(9):
            y = -0.34 + k * 0.10
            h = hit(V(s * 0.3, y, 0.93), V(s, 0, 0), [ctx.DOOR[s]])
            if h:
                pts.append(h)
        for a, b in zip(pts, pts[1:]):
            n = V(-s, 0, 0)
            bar(dh, a + n * 0.006, b + n * 0.006, 0.026, "house", up=n)
            bar(dn, a + n * 0.013, b + n * 0.013, 0.012, "neon", up=n)
        P.extend([dh, dn])

    # ------------------------------------------------ экран и тумблеры (на торпедо справа от руля)
    scr = Part("screen", "body", INT_LODS, ["er_int"])
    glow = Part("screen_glow", "body", INT_LODS, ["er_int", "screen"])   # своё скрытое выделение: экран вкл/выкл
    c = V(-0.17, -0.37, 1.055)
    n = (EYE - c).normalized()
    ex = Vector((0, 0, 1)).cross(n).normalized()
    ey = n.cross(ex).normalized()
    box(scr, c, ex, ey, n, 0.105, 0.07, 0.022, "house")
    box(glow, c + n * 0.023, ex, ey, n, 0.088, 0.055, 0.002, "screen")
    glow.uvframe = (c + n * 0.023, ex, ey, 0.088, 0.055)               # карта на весь экран
    for sx in (-1, 1):                                         # кронштейны
        bar(scr, c + ex * sx * 0.08 - n * 0.02, c + ex * sx * 0.08 - n * 0.06 - ey * 0.05, 0.012, "steel")
    tp = c - ey * 0.105 + n * 0.005
    box(scr, tp, ex, ey, n, 0.09, 0.022, 0.018, "steel")
    for k in range(5):
        q = tp + ex * (-0.064 + k * 0.032) + n * 0.018
        cyl(scr, q, n, 0.006, 0.02, 6, "bolt")
        cyl(scr, q + n * 0.012 + ey * 0.004, n, 0.0095, 0.016, 8, "red")
    P.extend([scr, glow])

    # ------------------------------------------------ ящик между сиденьями и колба Изнанки
    bx = Part("box", "body", INT_LODS, ["er_int"])
    zb = 0.60
    b0 = V(0, 0.27, zb + 0.09)
    box(bx, b0, V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.098, 0.15, 0.09, "rustm")
    box(bx, b0 + V(0, 0, 0.095), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.102, 0.155, 0.008, "steel")   # крышка
    for sx in (-1, 1):
        box(bx, b0 + V(sx * 0.1, -0.08, 0.06), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.006, 0.018, 0.022, "bolt")
        for sy in (-1, 1):
            cyl(bx, b0 + V(sx * 0.085, sy * 0.135, 0.104), V(0, 0, 1), 0.006, 0.004, 6, "bolt")
    bar(bx, b0 + V(-0.05, -0.04, 0.103), b0 + V(-0.05, -0.04, 0.125), 0.01, "steel")
    bar(bx, b0 + V(0.05, -0.04, 0.103), b0 + V(0.05, -0.04, 0.125), 0.01, "steel")
    bar(bx, b0 + V(-0.05, -0.04, 0.125), b0 + V(0.05, -0.04, 0.125), 0.01, "steel")          # ручка
    P.append(bx)
    fl = Part("flask", "body", INT_LODS, ["er_int"])
    fg = Part("flask_glow", "body", INT_LODS, ["er_int", "neon"])
    fc = b0 + V(0, 0.09, 0.103)
    cyl(fl, fc, V(0, 0, 1), 0.05, 0.03, 16, "steel")                        # основание
    cyl(fl, fc + V(0, 0, 0.19), V(0, 0, 1), 0.046, 0.025, 16, "steel")      # крышка
    cyl(fl, fc + V(0, 0, 0.215), V(0, 0, 1), 0.016, 0.02, 8, "bolt")
    for k in range(4):                                                      # стойки клетки колбы
        a = 2 * math.pi * k / 4 + math.pi / 4
        p0 = fc + V(0.047 * math.cos(a), 0.047 * math.sin(a), 0.03)
        bar(fl, p0, p0 + V(0, 0, 0.16), 0.008, "steel")
    cyl(fl, fc + V(0, 0, 0.03), V(0, 0, 1), 0.036, 0.16, 16, "glass")       # стекло — не в «neon»
    core = [fc + V(0.012 * math.sin(k * 1.7), 0.012 * math.cos(k * 2.3), 0.045 + 0.13 * k / 8) for k in range(9)]
    tube(fg, catmull(core, 0.008), 0.0045, 6, "neon")                       # «молния» внутри
    P.extend([fl, fg])

    # ------------------------------------------------ кабели от ящика к экрану
    cb = Part("cables", "body", [1, 2, 1100, 1201], ["er_int"])
    for dx in (-0.03, 0.03):
        path = [b0 + V(dx, -0.15, 0.03), V(dx * 0.5, 0.0, 0.66), V(-0.05 + dx, -0.25, 0.78),
                V(-0.12 + dx, -0.36, 0.95), c - ey * 0.06 - n * 0.03 + ex * dx]
        tube(cb, catmull(path, 0.02), 0.0075, 6, "cable")
    P.append(cb)
