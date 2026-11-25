# -*- coding: utf-8 -*-
r"""Модели 14 материалов крафта VB_EchoMaterials (список утверждён пользователем 11-24).

  G:\SteamLibrary\steamapps\common\Blender\blender.exe -b --factory-startup --python em_models.py -- <папка вывода> [--only=servo,coil]

Пишет <папка>/em_<имя>.p3d (MLOD): LOD 1/2/3, Geometry (коробка по габариту, масса), Memory (invview, ce_*, габарит),
View/Fire Geometry. Для мода папка вывода = ../VB_EchoMaterials/data (бинаризует pbo.ps1 -Binarize).
Текстуры — em_tex.py (vb_echomaterials\data\em_*), болты и пустой неон — копии er_* (eb_copy_mats.py materials).
Отдельный мод @VB_EchoMaterials, ни от чего не зависит (решение пользователя 11-24); новые материалы — сюда же.
Мод УНИВЕРСАЛЬНЫЙ, не только под 16 (пользователь 11-25): материалы — обычные детали, ничем не светятся и не опираются
на контент одного сервера; «Эхо» — только слово в текстах (так энергия называется по лору 16).
Оси Blender: X вправо, Y назад, Z вверх, предмет лежит на Z = 0. Геометрия — er_geom Echo Runner (порядок граней
в MLOD верный: запись (x, z, y) зеркалит, см. ГАЙД). Масштаб плитки текстуры — uvtile детали, картинка на грань — uvframe.
"""
import math
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..", "mods_vb_vehicles", "mod_vb_EchoRunner", "tools")))
import bmesh  # noqa: E402
import bpy  # noqa: E402,F401
import er_common as C  # noqa: E402
import er_geom as G  # noqa: E402
from er_geom import Part, V, box, catmull, cyl, perp, ring, sweep, tube, part_data, append, arclen_points  # noqa: E402

py3d = C.py3d
args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
pos = [a for a in args if not a.startswith("--")]
OUTDIR = pos[0] if pos else os.path.join(HERE, "..", "VB_EchoMaterials", "data")
ONLY = set(str(opts.get("only", "")).split(",")) - {""}

EM = "vb_echomaterials\\data\\"
for _n in ("steel", "alu", "primer_k", "primer_g", "chrome", "paint_red", "copper", "carbon", "coating", "fabric",
           "hexpad", "rubber", "plastic", "glass", "label", "pcb"):
    G.COL["e_" + _n] = (EM + "em_%s_%s.paa" % (_n, "ca" if _n == "glass" else "co"), EM + "em_%s.rvmat" % _n)
    G.MATS.append("e_" + _n)
# Болты и пустой неон — копии er_* в своей data (eb_copy_mats.py materials): мод отдельный, ни от чего не зависит
# (решение пользователя 11-24: по раскладке ТЗ_VB_Echo ядро — только энергия, материалов крафта в нём нет).
G.COL["e_bolt"] = (EM + "er_bolt_co.paa", EM + "er_bolt.rvmat")
# Свет — по логике энергии (решение пользователя 11-24): у снятого провода и блока энергии нет — трубка и огоньки
# серые пустые, как трубки Echo Runner без энергии. В материалах не светится ничего (11-25: осколок Эхо и кристалл
# в катушке убраны — заменены бухтой медного провода и ферритовым сердечником).
G.COL["e_neon"] = (EM + "er_neon_empty_co.paa", EM + "er_neon_empty.rvmat")
G.MATS += ["e_bolt", "e_neon"]
ALIAS = {"e_pk": "e_primer_k", "e_pg": "e_primer_g", "e_red": "e_paint_red", "e_coat": "e_coating", "e_hex": "e_hexpad"}
for _a, _t in ALIAS.items():            # короткие имена годятся и в box/cyl напрямую
    G.COL[_a] = G.COL[_t]
    G.MATS.append(_a)

X, Y, Z = V(1, 0, 0), V(0, 1, 0), V(0, 0, 1)
SEG = 24


def M(m):
    return ALIAS.get(m, m)


def P(name, tile=0.25):
    p = Part(name, "", [], [])
    p.uvtile = tile
    return p


# ---------------------------------------------------------------- примитивы
def rrect(hw, hh, r, seg=3):
    """Скруглённый прямоугольник (2D, против часовой стрелки)."""
    pts = []
    for cx, cy, a0 in ((hw - r, -hh + r, -90), (hw - r, hh - r, 0), (-hw + r, hh - r, 90), (-hw + r, -hh + r, 180)):
        for k in range(seg + 1):
            a = math.radians(a0 + 90.0 * k / seg)
            pts.append((cx + r * math.cos(a), cy + r * math.sin(a)))
    return pts


def chamfer_rect(hx, hy, c):
    return [(hx - c, -hy), (hx, -hy + c), (hx, hy - c), (hx - c, hy),
            (-hx + c, hy), (-hx, hy - c), (-hx, -hy + c), (-hx + c, -hy)]


def prism(part, outline, z0, z1, mat, cx=0.0, cy=0.0):
    sweep(part, [[V(cx + x, cy + y, z0) for x, y in outline], [V(cx + x, cy + y, z1) for x, y in outline]], M(mat))


def along_x(part, outline, x0, x1, mat, cy=0.0, cz=0.0):
    """Профиль (y, z) вытянут вдоль X."""
    sweep(part, [[V(x0, cy + y, cz + z) for y, z in outline], [V(x1, cy + y, cz + z) for y, z in outline]], M(mat))


def hollow(part, outer, inner, a0, a1, mat, axis="x", c=(0.0, 0.0)):
    """Полая труба/рамка: внешний и внутренний контуры (одинаковое число точек) вдоль оси x или z."""
    bm = part.bm

    def pt(a, u, v):
        if axis == "x":
            return V(a, c[0] + u, c[1] + v)
        return V(c[0] + u, c[1] + v, a)

    o0 = [bm.verts.new(pt(a0, u, v)) for u, v in outer]
    o1 = [bm.verts.new(pt(a1, u, v)) for u, v in outer]
    i0 = [bm.verts.new(pt(a0, u, v)) for u, v in inner]
    i1 = [bm.verts.new(pt(a1, u, v)) for u, v in inner]
    n = len(outer)
    for k in range(n):
        m = (k + 1) % n
        part.face((o0[k], o0[m], o1[m], o1[k]), M(mat))
        part.face((i0[k], i1[k], i1[m], i0[m]), M(mat))
        part.face((o0[k], i0[k], i0[m], o0[m]), M(mat))
        part.face((o1[k], o1[m], i1[m], i1[k]), M(mat))


def shell_x(part, r_in, r_out, a_deg, x0, x1, mat, cz, n=10):
    """Изогнутая полоска поверх цилиндра (ось X), дуга ±a_deg от верха — этикетки на корпусах."""
    arc = [math.radians(-a_deg + 2 * a_deg * k / n) for k in range(n + 1)]
    outline = [(r_out * math.sin(a), r_out * math.cos(a)) for a in arc] + \
              [(r_in * math.sin(a), r_in * math.cos(a)) for a in reversed(arc)]
    along_x(part, outline, x0, x1, mat, 0.0, cz)


def eye(part, c, r_in, r_out, w):
    """Проушина с резиновой втулкой (ось Y)."""
    ring(part, c, Y, r_in, r_out, w, SEG, M("e_steel"))
    ring(part, c, Y, r_in * 0.62, r_in, w + 0.002, 12, M("e_rubber"))


def bolt_x(part, c, r=0.0045, h=0.004):
    cyl(part, c, X, r, h, 6, "e_bolt")


# ---------------------------------------------------------------- 1. бронепластина
def armorplate():
    body, trim, lab = P("plate", 0.25), P("trim", 0.25), P("label", 1.0)
    hx, hy, t, s = 0.125, 0.16, 0.02, 0.022
    cut_t, cut_b = 0.045, 0.012

    def zlo(x):
        return s * (1 - (x / hx) ** 2)

    def ys(x):
        e = hx - abs(x)
        return -hy + max(0.0, cut_t - e), hy - max(0.0, cut_b - e)

    xs = sorted(set([round(-hx + 2 * hx * k / 24, 6) for k in range(25)] +
                    [round(sg * (hx - c), 6) for sg in (-1, 1) for c in (cut_t, cut_b)]))
    rings = []
    for x in xs:
        y0, y1 = ys(x)
        z = zlo(x)
        rings.append([V(x, y0, z), V(x, y1, z), V(x, y1, z + t), V(x, y0, z + t)])
    sweep(body, rings, M("e_coat"))
    # резиновая окантовка по контуру (начало — середина верхнего края, чтобы концы трубки сошлись по прямой)
    corners = [(0.0, -hy), (hx - cut_t, -hy), (hx, -hy + cut_t), (hx, hy - cut_b), (hx - cut_b, hy),
               (-hx + cut_b, hy), (-hx, hy - cut_b), (-hx, -hy + cut_t), (-hx + cut_t, -hy), (0.0, -hy)]
    pts = []
    for a, b in zip(corners[:-1], corners[1:]):
        n = max(1, int(math.dist(a, b) / 0.015))
        for k in range(n):
            x = a[0] + (b[0] - a[0]) * k / n
            y = a[1] + (b[1] - a[1]) * k / n
            pts.append(V(x, y, zlo(x) + t / 2))
    pts.append(pts[0].copy())
    tube(trim, pts, 0.0115, 10 if SEG > 12 else 6, M("e_rubber"), caps=True)
    # этикетка по изгибу
    lx = [-0.04 + 0.08 * k / 8 for k in range(9)]
    sweep(lab, [[V(x, 0.0, zlo(x) + t + 0.0004), V(x, 0.08, zlo(x) + t + 0.0004), V(x, 0.08, zlo(x) + t + 0.0009),
                 V(x, 0.0, zlo(x) + t + 0.0009)] for x in lx], M("e_label"))
    lab.uvframe = (V(0, 0.04, 0), X, -Y, 0.04, 0.04)
    return [body, trim, lab]


# ---------------------------------------------------------------- 2. карбоновый профиль
def carbon():
    tb, st = P("tubes", 0.1), P("straps", 0.25)
    L = 0.25
    out, inn = rrect(0.015, 0.015, 0.004), rrect(0.0115, 0.0115, 0.0025)
    for cy in (-0.0155, 0.0155):
        hollow(tb, out, inn, -L, L, "e_carbon", "x", (cy, 0.015))
        hollow(st, rrect(0.0157, 0.0157, 0.0045), out, 0.205, 0.225, "e_red", "x", (cy, 0.015))   # маркировка
    for x in (-0.15, 0.15):
        hollow(st, rrect(0.0335, 0.0175, 0.006), rrect(0.0312, 0.0152, 0.004), x - 0.008, x + 0.008, "e_plastic",
               "x", (0.0, 0.015))
    return [tb, st]


# ---------------------------------------------------------------- 3. сервомотор
def servo():
    b, lab = P("servo", 0.15), P("label", 1.0)
    zc = 0.04
    cyl(b, V(-0.06, 0, zc), X, 0.033, 0.09, SEG, "e_alu")
    for x in (-0.056, -0.05, 0.004, 0.012, 0.02):
        ring(b, V(x, 0, zc), X, 0.032, 0.0375, 0.003, SEG, M("e_alu"))
    ring(b, V(-0.0435, 0, zc), X, 0.0325, 0.0345, 0.004, SEG, M("e_red"))
    cyl(b, V(-0.07, 0, zc), X, 0.029, 0.01, SEG, "e_steel")
    box(b, V(0.038, 0, zc), X, Y, Z, 0.008, 0.04, 0.04, "e_steel")
    for sy in (-1, 1):
        for sz in (-1, 1):
            bolt_x(b, V(0.046, sy * 0.03, zc + sz * 0.03))
    cyl(b, V(0.046, 0, zc), X, 0.022, 0.012, SEG, "e_alu")
    cyl(b, V(0.058, 0, zc), X, 0.007, 0.028, 12, "e_chrome")
    box(b, V(0.075, 0, zc + 0.0072), X, Y, Z, 0.009, 0.0018, 0.0012, "e_steel")
    # разъём сбоку и кабель
    box(b, V(-0.02, 0.040, zc), X, Y, Z, 0.013, 0.009, 0.011, "e_plastic")
    box(b, V(-0.02, 0.054, zc), X, Y, Z, 0.010, 0.006, 0.008, "e_plastic")
    box(b, V(-0.02, 0.054, zc + 0.0085), X, Y, Z, 0.004, 0.004, 0.0012, "e_red")
    tube(b, catmull([(-0.02, 0.059, zc), (-0.02, 0.08, zc - 0.006), (-0.035, 0.10, 0.012), (-0.07, 0.11, 0.005),
                     (-0.12, 0.10, 0.004)], 0.01), 0.004, 8, M("e_plastic"))
    shell_x(lab, 0.0333, 0.0337, 55, -0.040, -0.006, "e_label", zc)
    lab.uvframe = (V(-0.023, 0, 0), X, -Y, 0.022, 0.028)
    return [b, lab]


# ---------------------------------------------------------------- 4. гидроцилиндр
def hydraulic():
    b, lab = P("hyd", 0.2), P("label", 1.0)
    zc = 0.036
    cyl(b, V(-0.13, 0, zc), X, 0.026, 0.22, SEG, "e_alu")
    for xc in (-0.14, 0.10):
        box(b, V(xc, 0, zc), X, Y, Z, 0.01, 0.034, 0.034, "e_steel")
        cyl(b, V(xc, 0, zc + 0.034), Z, 0.009, 0.008, 6, "e_bolt")
        cyl(b, V(xc, 0, zc + 0.042), Z, 0.0055, 0.010, 12, "e_steel")
        cyl(b, V(xc, 0, zc + 0.052), Z, 0.0068, 0.006, 12, "e_red")
    for sy in (-1, 1):
        for sz in (-1, 1):
            c = V(0, sy * 0.027, zc + sz * 0.027)
            cyl(b, c + V(-0.156, 0, 0), X, 0.0035, 0.272, 8, "e_steel")
            bolt_x(b, c + V(-0.162, 0, 0), 0.006, 0.006)
            bolt_x(b, c + V(0.11, 0, 0), 0.006, 0.006)
    cyl(b, V(0.11, 0, zc), X, 0.017, 0.008, SEG, "e_steel")
    cyl(b, V(0.11, 0, zc), X, 0.012, 0.10, SEG, "e_chrome")
    box(b, V(0.222, 0, zc), X, Y, Z, 0.012, 0.012, 0.016, "e_steel")
    eye(b, V(0.25, 0, zc), 0.008, 0.019, 0.022)
    for sy in (-1, 1):
        box(b, V(-0.172, sy * 0.014, zc), X, Y, Z, 0.022, 0.004, 0.02, "e_steel")
    cyl(b, V(-0.182, -0.022, zc), Y, 0.006, 0.044, 12, "e_chrome")
    shell_x(lab, 0.0262, 0.0266, 45, -0.085, -0.035, "e_label", zc)
    lab.uvframe = (V(-0.06, 0, 0), X, -Y, 0.025, 0.02)
    return [b, lab]


# ---------------------------------------------------------------- 5. бухта медного провода
# (11-24 вместо «Осколка Эхо»: пользователь — «не подойдёт, придумай что-то приземлённое»)
def poly(n, r, rot=0.0):
    return [(r * math.cos(rot + 2 * math.pi * k / n), r * math.sin(rot + 2 * math.pi * k / n)) for k in range(n)]


def spool():
    """Обмоточный медный провод в лаке на пластиковой катушке: щёки, ступица с отверстием, конец провода заправлен
    в прорезь щеки, этикетка сверху."""
    b, w, lab = P("spool", 0.15), P("wire", 0.05), P("label", 1.0)
    R_fl, R_w, R_hub, R_hole, t, H = 0.06, 0.047, 0.02, 0.012, 0.006, 0.08
    n = SEG
    for z0 in (0.0, H - t):
        hollow(b, poly(n, R_fl), poly(n, R_hole), z0, z0 + t, "e_plastic", "z")
    hollow(b, poly(n, R_hub), poly(n, R_hole), t, H - t, "e_plastic", "z")
    hollow(w, poly(n, R_w), poly(n, R_hub + 0.001), t, H - t, "e_copper", "z")
    a0, a1 = math.radians(20), math.radians(38)
    d0, d1 = V(math.cos(a0), math.sin(a0), 0), V(math.cos(a1), math.sin(a1), 0)
    tube(w, catmull([tuple(d0 * (R_w + 0.0008) + V(0, 0, H - 0.026)), tuple(d0 * (R_w + 0.006) + V(0, 0, H - 0.012)),
                     tuple(d1 * (R_fl + 0.0015) + V(0, 0, H - 0.002)), tuple(d1 * (R_fl - 0.006) + V(0, 0, H + 0.0016)),
                     tuple(d1 * (R_fl - 0.014) + V(0, 0, H + 0.0016))], 0.004), 0.0012, 6, M("e_copper"))
    box(lab, V(-0.036, 0, H + 0.0004), X, Y, Z, 0.016, 0.016, 0.0004, "e_label")
    lab.uvframe = (V(-0.036, 0, 0), X, -Y, 0.016, 0.016)
    return [b, w, lab]


# ---------------------------------------------------------------- 6. амортизатор (койловер)
def shock():
    b, sp, lab = P("shock", 0.2), P("spring", 0.1), P("label", 1.0)
    zc = 0.043
    cyl(b, V(-0.17, 0, zc), X, 0.022, 0.20, SEG, "e_pk")
    box(b, V(-0.178, 0, zc), X, Y, Z, 0.01, 0.012, 0.012, "e_steel")
    eye(b, V(-0.2, 0, zc), 0.007, 0.016, 0.02)
    ring(b, V(-0.11, 0, zc), X, 0.021, 0.030, 0.008, SEG, M("e_alu"))
    ring(b, V(-0.1, 0, zc), X, 0.021, 0.043, 0.006, SEG, M("e_steel"))
    cyl(b, V(0.03, 0, zc), X, 0.0085, 0.13, 12, "e_chrome")
    cyl(b, V(0.03, 0, zc), X, 0.016, 0.01, SEG, "e_steel")
    cyl(b, V(0.12, 0, zc), X, 0.014, 0.022, 12, "e_rubber")
    ring(b, V(0.15, 0, zc), X, 0.006, 0.043, 0.008, SEG, M("e_steel"))
    box(b, V(0.168, 0, zc), X, Y, Z, 0.012, 0.012, 0.012, "e_steel")
    eye(b, V(0.19, 0, zc), 0.007, 0.016, 0.02)
    # пружина — винтовая линия, 6,5 витка между тарелками
    x0, x1, R, turns = -0.097, 0.146, 0.034, 6.5
    n = int(turns * (36 if SEG > 12 else 14))
    path = [V(x0 + (x1 - x0) * i / n, R * math.cos(2 * math.pi * turns * i / n), zc + R * math.sin(2 * math.pi * turns * i / n))
            for i in range(n + 1)]
    tube(sp, path, 0.0055, 10 if SEG > 12 else 6, M("e_red"))
    shell_x(lab, 0.0222, 0.0226, 45, -0.165, -0.125, "e_label", zc)
    lab.uvframe = (V(-0.145, 0, 0), X, -Y, 0.02, 0.016)
    return [b, sp, lab]


# ---------------------------------------------------------------- 7. ЭБУ
def ecu():
    b, lab = P("ecu", 0.15), P("label", 1.0)
    hx, hy, h = 0.075, 0.06, 0.03
    prism(b, chamfer_rect(hx, hy, 0.006), 0.0, h, "e_alu")
    for k in range(9):
        box(b, V(-0.062 + 0.124 * k / 8, 0.03, h + 0.006), X, Y, Z, 0.0016, 0.024, 0.006, "e_alu")
    for sx in (-1, 1):
        for sy in (-1, 1):
            box(b, V(sx * (hx + 0.011), sy * 0.036, 0.0025), X, Y, Z, 0.012, 0.009, 0.0025, "e_alu")
            cyl(b, V(sx * (hx + 0.013), sy * 0.036, 0.005), Z, 0.0042, 0.003, 6, "e_bolt")
    for x in (-0.035, 0.03):
        box(b, V(x, -hy - 0.008, 0.015), X, Y, Z, 0.02, 0.008, 0.009, "e_plastic")
        box(b, V(x, -hy - 0.022, 0.015), X, Y, Z, 0.017, 0.007, 0.0075, "e_plastic")
        box(b, V(x, -hy - 0.022, 0.0235), X, Y, Z, 0.006, 0.005, 0.0012, "e_red")
        tube(b, catmull([(x, -hy - 0.028, 0.015), (x, -hy - 0.05, 0.012), (x + 0.01, -hy - 0.08, 0.006),
                         (x + 0.03, -hy - 0.10, 0.0045)], 0.01), 0.0045, 8, M("e_plastic"))
    box(lab, V(-0.025, -0.028, h + 0.0005), X, Y, Z, 0.025, 0.025, 0.0005, "e_label")
    lab.uvframe = (V(-0.025, -0.028, 0), X, -Y, 0.025, 0.025)
    return [b, lab]


# ---------------------------------------------------------------- 8. демпферная вставка
def damper():
    b = P("pad", 0.2)
    hx, hy, h = 0.10, 0.075, 0.022
    base = rrect(hx, hy, 0.02, 4)
    top = rrect(hx - 0.004, hy - 0.004, 0.017, 4)
    sweep(b, [[V(x, y, 0.0) for x, y in base], [V(x, y, h - 0.005) for x, y in base],
              [V(x, y, h) for x, y in top]], M("e_fabric"))
    hexi = G.MATS.index(M("e_hex"))
    for f in b.bm.faces:
        if f.calc_center_median().z > h - 0.0005 and len(f.verts) > 4:
            f.material_index = hexi
    pip = [V(x, y, h - 0.004) for x, y in rrect(hx - 0.0005, hy - 0.0005, 0.02, 4)]
    tube(b, catmull([tuple(p) for p in pip + [pip[0]]], 0.008), 0.0028, 8, M("e_fabric"))
    box(b, V(hx + 0.007, -0.03, h - 0.006), X, Y, Z, 0.009, 0.008, 0.0008, "e_red")
    return [b]


# ---------------------------------------------------------------- 9. блок управления экзоскелетом
def exoctrl():
    b, pcb, glow = P("ctl", 0.15), P("pcb", 1.0), P("glow", 0.25)
    hx, hy, h = 0.06, 0.045, 0.026
    box(b, V(0, 0, 0.002), X, Y, Z, hx, hy, 0.002, "e_steel")
    for sy in (-1, 1):
        box(b, V(0, sy * (hy - 0.002), h / 2), X, Y, Z, hx, 0.002, h / 2, "e_steel")
    for sx in (-1, 1):
        box(b, V(sx * (hx - 0.002), 0, h / 2), X, Y, Z, 0.002, hy - 0.004, h / 2, "e_steel")
    box(pcb, V(0, 0, 0.008), X, Y, Z, hx - 0.005, hy - 0.005, 0.0008, "e_pcb")
    pcb.uvframe = (V(0, 0, 0), X, -Y, hx - 0.005, hy - 0.005)
    # компоненты: процессор с радиатором, микросхемы, конденсаторы
    box(b, V(0.018, -0.004, 0.0105), X, Y, Z, 0.015, 0.015, 0.0017, "e_plastic")
    for k in range(5):
        box(b, V(0.006 + k * 0.006, -0.004, 0.0165), X, Y, Z, 0.0011, 0.013, 0.0045, "e_alu")
    box(b, V(0.018, -0.004, 0.0125), X, Y, Z, 0.014, 0.014, 0.0008, "e_alu")
    for x, y in ((-0.03, 0.02), (-0.012, 0.024), (-0.03, -0.02)):
        box(b, V(x, y, 0.0098), X, Y, Z, 0.007, 0.005, 0.001, "e_plastic")
    for x, y in ((-0.045, 0.0), (-0.038, 0.0), (-0.045, 0.01), (0.044, 0.028)):
        cyl(b, V(x, y, 0.0088), Z, 0.0032, 0.010, 10, "e_alu")
    # крышка над третью платы, болты, разъём и шлейф
    box(b, V(-hx + 0.02, 0, h + 0.0015), X, Y, Z, 0.02, hy, 0.0015, "e_steel")
    for sx in (-1, 1):
        for sy in (-1, 1):
            cyl(b, V(sx * (hx - 0.004), sy * (hy - 0.004), h), Z, 0.003, 0.0025, 6, "e_bolt")
    box(b, V(hx + 0.005, 0.012, 0.013), X, Y, Z, 0.005, 0.016, 0.006, "e_plastic")
    tube(b, catmull([(hx + 0.01, 0.012, 0.013), (hx + 0.03, 0.012, 0.012), (hx + 0.05, 0.03, 0.005),
                     (hx + 0.07, 0.06, 0.004)], 0.008), 0.003, 6, M("e_plastic"))
    # статус Эхо: полоса на передней стенке и три огонька на крышке
    box(glow, V(0, -hy - 0.0006, 0.018), X, Y, Z, 0.032, 0.0008, 0.0025, "e_neon")
    for k in range(3):
        box(glow, V(-hx + 0.009 + k * 0.011, -0.032, h + 0.0032), X, Y, Z, 0.0032, 0.0022, 0.0004, "e_neon")
    return [b, pcb, glow]


# ---------------------------------------------------------------- 10. проводник Эхо
def conduit():
    b, glow = P("cond", 0.15), P("glow", 0.1)
    r = 0.008
    turns, r0, r1 = 2.0, 0.072, 0.036
    n = int(turns * (40 if SEG > 12 else 16))
    path = []
    for i in range(n + 1):
        t = i / n
        a = 2 * math.pi * turns * t
        rr = r0 + (r1 - r0) * t
        path.append(V(rr * math.cos(a), rr * math.sin(a), r))
    # концы — прямые хвосты наружу/внутрь
    t0 = (path[1] - path[0]).normalized()
    path = [path[0] - t0 * 0.03] + path
    T = tube(glow, path, r, 12 if SEG > 12 else 6, M("e_neon"))
    for p, t in arclen_points(path, 0.07, 0.05):
        ring(b, p, t, r * 0.97, r * 1.22, 0.006, 12, M("e_steel"))
    for p, t in ((path[0], -T[0]), (path[-1], T[-1])):
        cyl(b, p - t * 0.012, t, 0.0115, 0.026, 12, "e_steel")
        cyl(b, p + t * 0.014, t, 0.0085, 0.004, 12, "e_plastic")
        for k in range(3):
            a = 2 * math.pi * k / 3
            u, v = perp(t)
            cyl(b, p + t * 0.018 + (u * math.cos(a) + v * math.sin(a)) * 0.0045, t, 0.0013, 0.006, 6, "e_bolt")
    return [b, glow]


# ---------------------------------------------------------------- 11. резонансная катушка
def coil():
    """Медная обмотка на ферритовом сердечнике между стальными щеками (11-25: без кристалла Эхо — мод универсальный,
    материалы ничем не светятся; сердечник выходит в окно верхней щеки)."""
    b, w = P("coil", 0.15), P("wind", 0.05)
    prism(b, chamfer_rect(0.045, 0.045, 0.008), 0.0, 0.01, "e_steel")
    hollow(b, chamfer_rect(0.045, 0.045, 0.008), chamfer_rect(0.017, 0.017, 0.007), 0.08, 0.09, "e_steel", "z")
    cyl(w, V(0, 0, 0.01), Z, 0.036, 0.07, SEG, "e_copper")
    cyl(b, V(0, 0, 0.01), Z, 0.0145, 0.086, SEG, "e_coat")
    for sx in (-1, 1):
        for sy in (-1, 1):
            cyl(b, V(sx * 0.035, sy * 0.035, 0.0), Z, 0.0035, 0.096, 8, "e_steel")
            cyl(b, V(sx * 0.035, sy * 0.035, 0.09), Z, 0.0065, 0.005, 6, "e_bolt")
        cyl(b, V(sx * 0.03, 0, 0.09), Z, 0.0048, 0.014, 10, "e_copper")
        cyl(b, V(sx * 0.03, 0, 0.096), Z, 0.0075, 0.004, 6, "e_bolt")
        tube(w, catmull([(sx * 0.036, 0.0, 0.072), (sx * 0.05, 0.0, 0.078), (sx * 0.05, 0.0, 0.098),
                         (sx * 0.031, 0.0, 0.103)], 0.006), 0.0016, 6, M("e_copper"))
    return [b, w]


# ---------------------------------------------------------------- 12. силовая рама
def frame():
    b = P("frame", 0.3)
    for sy in (-1, 1):
        hollow(b, rrect(0.02, 0.025, 0.003), rrect(0.016, 0.021, 0.002), -0.3, 0.3, "e_pk", "x", (sy * 0.11, 0.025))
        for x in (-0.27, 0.27):
            box(b, V(x, sy * 0.11, 0.0505), X, Y, Z, 0.012, 0.0185, 0.0008, "e_red")
        for k in range(7):
            cyl(b, V(-0.225 + k * 0.075, sy * 0.13, 0.025), V(0, sy, 0), 0.006, 0.003, 10, "e_bolt")
    for x in (-0.22, 0.0, 0.22):
        box(b, V(x, 0, 0.025), X, Y, Z, 0.018, 0.091, 0.02, "e_pk")
        for sy in (-1, 1):
            prism(b, [(x - 0.045, sy * 0.09), (x, sy * 0.045), (x + 0.045, sy * 0.09)] if sy > 0 else
                  [(x + 0.045, sy * 0.09), (x, sy * 0.045), (x - 0.045, sy * 0.09)], 0.045, 0.049, "e_pk")
            for dx in (-0.025, 0.025):
                cyl(b, V(x + dx, sy * 0.08, 0.049), Z, 0.0055, 0.004, 6, "e_bolt")
    return [b]


# ---------------------------------------------------------------- 13. кузовная панель
def panel():
    b, lab = P("panel", 0.5), P("label", 1.0)
    hx, hy, t = 0.35, 0.225, 0.003

    def zt(y):
        return 0.04 - 0.58 * y * y + 0.004 * math.exp(-((y - 0.08) / 0.009) ** 2)

    ys = [-hy + 0.003 + (2 * hy - 0.006) * k / 60 for k in range(61)]
    outer = [(-hy, 0.0)] + [(y, zt(y)) for y in ys] + [(hy, 0.0)]
    inner = [(hy - t, 0.0)] + [(y, zt(y) - t) for y in reversed(ys[1:-1])] + [(-hy + t, 0.0)]
    along_x(b, outer + inner, -hx, hx, "e_pg")
    # отбортовка по торцам (вниз на 2 см)
    for xe, sg in ((hx, 1), (-hx, -1)):
        rings = []
        for y in ys[1:-1]:
            z1 = zt(y)
            z0 = max(0.0, z1 - 0.02)
            x0, x1 = xe - sg * t, xe
            rings.append([V(x0, y, z0), V(x1, y, z0), V(x1, y, z1), V(x0, y, z1)])
        sweep(b, rings, M("e_pg"))
        for y in (-0.15, 0.0, 0.15):
            cyl(b, V(xe - sg * 0.014, y, zt(y) - 0.0008), Z, 0.0055, 0.0028, 10, "e_bolt")
    ly = [-0.15 + 0.06 * k / 8 for k in range(9)]
    sweep(lab, [[V(0.19, y, zt(y) + 0.0004), V(0.25, y, zt(y) + 0.0004), V(0.25, y, zt(y) + 0.0009),
                 V(0.19, y, zt(y) + 0.0009)] for y in ly], M("e_label"))
    lab.uvframe = (V(0.22, -0.12, 0), X, -Y, 0.03, 0.03)
    return [b, lab]


# ---------------------------------------------------------------- 14. бронестекло
def glass():
    b, gl, lab = P("guard", 0.15), P("glass", 0.3), P("label", 1.0)
    hx, hy = 0.17, 0.12
    for k in range(3):
        z = 0.0015 + k * 0.009
        box(gl, V(0, 0, z + 0.004), X, Y, Z, hx, hy, 0.004, "e_glass")
    for sx in (-1, 1):
        for sy in (-1, 1):
            box(b, V(sx * (hx - 0.02), sy * (hy + 0.003), 0.0145), X, Y, Z, 0.022, 0.003, 0.0145, "e_rubber")
            box(b, V(sx * (hx + 0.003), sy * (hy - 0.02), 0.0145), X, Y, Z, 0.003, 0.022, 0.0145, "e_rubber")
            for z in (0.0006, 0.0285):
                box(b, V(sx * (hx - 0.013), sy * (hy - 0.013), z), X, Y, Z, 0.016, 0.016, 0.0006, "e_rubber")
    box(lab, V(0.11, -0.07, 0.0283), X, Y, Z, 0.02, 0.02, 0.0004, "e_label")
    lab.uvframe = (V(0.11, -0.07, 0), X, -Y, 0.02, 0.02)
    return [b, gl, lab]


BUILDERS = {"armorplate": armorplate, "carbon": carbon, "servo": servo, "hydraulic": hydraulic, "spool": spool,
            "shock": shock, "ecu": ecu, "damper": damper, "exoctrl": exoctrl, "conduit": conduit, "coil": coil,
            "frame": frame, "panel": panel, "glass": glass}
MASS = {"armorplate": 1.8, "carbon": 0.6, "servo": 0.9, "hydraulic": 2.5, "spool": 1.5, "shock": 3.0, "ecu": 0.8,
        "damper": 0.3, "exoctrl": 0.6, "conduit": 0.4, "coil": 1.2, "frame": 8.0, "panel": 6.0, "glass": 4.0}


# Хват двумя руками как у ванильного ящика WoodenCrate (VB_EM_Hands.c, стенд 11-25): ящик держат поперёк груди на руках
# снизу, его длинная ось — Z модели (wooden_case.p3d: x ±0.21, z ±0.40). Панель и раму строим длинной стороной по X —
# для хвата поворачиваем на 90° вокруг вертикали (X Blender -> Y Blender = Z DayZ), вместе с рамками этикеток (uvframe).
CRATE_HOLD = {"panel", "frame"}


def rot_z90(parts_list):
    def r(v):
        return V(-v.y, v.x, v.z)

    for p in parts_list:
        for v in p.bm.verts:
            v.co = r(v.co)
        fr = getattr(p, "uvframe", None)
        if fr is not None:
            o, ex, ey, hx, hy = fr
            p.uvframe = (r(o), r(ex), r(ey), hx, hy)


# ---------------------------------------------------------------- запись
def lod(parts_list, res):
    l = py3d.LOD()
    l.resolution = res
    for p in parts_list:
        if p.bm.faces:
            G.UV_TILE = getattr(p, "uvtile", 1.0)
            append(l, part_data(p), p.sels)
    G.UV_TILE = 1.0
    return l


def bounds(parts_list):
    xs, ys, zs = [], [], []
    for p in parts_list:
        for v in p.bm.verts:
            xs.append(v.co.x)
            ys.append(v.co.z)
            zs.append(v.co.y)
    return (min(xs), min(ys), min(zs)), (max(xs), max(ys), max(zs))


def geo_lod(lo, hi, res, mass=None):
    """Коробка по габариту (оси DayZ: x, вверх, z)."""
    p = Part("geo", "", [], ["Component01"])
    p.uvtile = 1.0
    c = V((lo[0] + hi[0]) / 2, (lo[2] + hi[2]) / 2, (lo[1] + hi[1]) / 2)
    box(p, c, X, Y, Z, max((hi[0] - lo[0]) / 2, 0.005), max((hi[2] - lo[2]) / 2, 0.005),
        max((hi[1] - lo[1]) / 2, 0.005), "steel")
    d = part_data(p)
    l = py3d.LOD()
    l.resolution = res
    append(l, (d[0], [(f[0], f[1], f[2], ("", "")) for f in d[1]]), p.sels)
    if mass:
        for pt in l.points:
            pt.mass = mass / len(l.points)
        l.properties["autocenter"] = "0"
    return l


def memory_lod(lo, hi):
    c = tuple((a + b) / 2 for a, b in zip(lo, hi))
    r = max(hi[i] - lo[i] for i in range(3))
    l = py3d.LOD()
    l.resolution = 1e15
    pts = {"invview": (c[0] - r * 1.2, c[1] + r * 0.9, c[2] - r * 1.2), "ce_center": c,
           "ce_radius": (c[0] + r / 2, c[1], c[2]), "boundingbox_min": lo, "boundingbox_max": hi}
    for sel_name, co in pts.items():
        sel = l.new_selection(sel_name)
        pt = py3d.Point()
        pt.coords = tuple(float(x) for x in co)
        l.points.append(pt)
        sel.points.update({pt: 1})
    return l


bpy.ops.wm.read_factory_settings(use_empty=True)
os.makedirs(OUTDIR, exist_ok=True)
for name, fn in BUILDERS.items():
    if ONLY and name not in ONLY:
        continue
    m = py3d.P3D()
    first = None
    for res, seg in ((1.0, 24), (2.0, 14), (3.0, 8)):
        SEG = seg
        built = fn()
        if name in CRATE_HOLD:
            rot_z90(built)
        if first is None:
            lo, hi = bounds(built)
            first = built
        m.lods.append(lod(built, res))
    m.lods.append(geo_lod(lo, hi, 1e13, MASS[name]))
    m.lods.append(memory_lod(lo, hi))
    m.lods.append(geo_lod(lo, hi, 6e15))
    m.lods.append(geo_lod(lo, hi, 7e15))
    out = os.path.join(OUTDIR, "em_%s.p3d" % name)
    m.save(out)
    print("OK", name, "граней LOD1", len(m.lods[0].faces), "габарит см",
          [round((hi[i] - lo[i]) * 100, 1) for i in range(3)], out)
