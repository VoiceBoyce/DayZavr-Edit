# -*- coding: utf-8 -*-
r"""Модели устройств базы VB_EchoBase (ТЗ: modding/mod_vb_Echo/ТЗ_VB_EchoBase.md). Концепты одобрены пользователем 11-20.

  G:\SteamLibrary\steamapps\common\Blender\blender.exe -b --factory-startup --python eb_models.py -- <папка вывода> [--only=wind,solar] [--tier=N]

Пишет <папка>/eb_<имя>.p3d (MLOD): LOD 1/2/3, Geometry (выпуклые блоки, масса), Memory (invview, ce_*, габариты,
у ветряка — rotor_axis), View/Fire Geometry. Для мода: папка вывода = ../VB_EchoBase/data (бинаризует pbo.ps1).
Материалы — копии er_* в VB_EchoBase/data (eb_copy_mats.py), пути vb_echobase\data\. --tier — только для листа-примера
уровней: улучшенные модели пользователю пока не нужны (11-20), в мод идёт Mk I.

Стиль семейства (как Echo Runner и аккумулятор Эхо): ржавая клёпаная сталь, тёмный металл, болты, красный неон Эхо.
Светящееся — отдельная часть с выделением «neon» (hiddenSelection: серый без энергии, красный / бирюзовый).
Знак семейства — кольцо резонатора со светящимся ядром (как на антиграв-модуле и аккумуляторе).
Оси Blender: X вправо, Y назад (лицо устройства смотрит в -Y), Z вверх; низ на Z = 0. Материалы — библиотека er_*.
"""
import math
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ER_TOOLS = os.path.join(HERE, "..", "..", "mods_vb_vehicles", "mod_vb_EchoRunner", "tools")
sys.path.insert(0, os.path.abspath(ER_TOOLS))
import bpy  # noqa: E402,F401
import er_common as C  # noqa: E402
from er_geom import Part, V, bar, box, cyl, ring, tube, catmull, sweep, part_data, append  # noqa: E402

py3d = C.py3d
args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
OUTDIR = [a for a in args if not a.startswith("--")][0]
ONLY = set(str(opts.get("only", "")).split(",")) - {""}

X, Y, Z = V(1, 0, 0), V(0, 1, 0), V(0, 0, 1)
SEG = 24

# цвета неона (решение пользователя 11-20): что течёт внутри / уровень устройства.
# «neon» — энергия: у устройств с Эхо красный (бирюзовый на синтезе), у источников — цвет уровня;
# «tier» — метка уровня на устройствах с Эхо и вход тока преобразователя. Mk I жёлтый (= ток), II зелёный,
# III синий, IV фиолетовый. Без энергии — серый (скрипт). Текстуры уровней — eb_neon_tiers.py.
import er_geom as G  # noqa: E402

EB = "vb_echobase\\data\\"
if opts.get("root") == "core":                 # энергоячейка — в VB_EchoCore (материалы: eb_copy_mats.py core)
    EB = "vb_echocore\\data\\"
for _t in (1, 2, 3, 4):
    G.COL["t%d" % _t] = (EB + "eb_neon_t%d_co.paa" % _t, EB + "er_neon.rvmat")
    G.MATS.append("t%d" % _t)
TIER = int(opts.get("tier", 1))
FUSION = bool(opts.get("fusion", False))     # --fusion: неон бирюзовый (холодный синтез) — для листа-примера
if FUSION:
    G.COL["neon"] = ("vb_echorunner\\data\\parts\\er_neon_fusion_co.paa", G.COL["neon"][1])
TM = "t%d" % TIER


# ---------------------------------------------------------------- общие детали
def poly(n, r, rot=0.0):
    return [(r * math.cos(rot + 2 * math.pi * i / n), r * math.sin(rot + 2 * math.pi * i / n)) for i in range(n)]


def chamfer_rect(hx, hy, c):
    return [(hx - c, -hy), (hx, -hy + c), (hx, hy - c), (hx - c, hy),
            (-hx + c, hy), (-hx, hy - c), (-hx, -hy + c), (-hx + c, -hy)]


def prism(part, outline, z0, z1, mat, cx=0.0, cy=0.0):
    sweep(part, [[V(cx + x, cy + y, z0) for x, y in outline], [V(cx + x, cy + y, z1) for x, y in outline]], mat)


def frustum(part, outline0, outline1, z0, z1, mat, cx=0.0, cy=0.0):
    sweep(part, [[V(cx + x, cy + y, z0) for x, y in outline0], [V(cx + x, cy + y, z1) for x, y in outline1]], mat)


def bolts_ring(part, c, axis, r, n, size=0.012, depth=0.008):
    u = axis.normalized()
    a = X if abs(u.dot(X)) < 0.9 else Y
    e1 = u.cross(a).normalized()
    e2 = u.cross(e1).normalized()
    for k in range(n):
        t = 2 * math.pi * k / n
        cyl(part, c + (e1 * math.cos(t) + e2 * math.sin(t)) * r, u, size, depth, 6, "bolt")


def plate_bolts(part, c, nrm, ex, ey, hx, hy, size=0.01):
    """Болты по углам панели (nrm — наружу)."""
    for sx in (-1, 1):
        for sy in (-1, 1):
            cyl(part, c + ex * (sx * hx) + ey * (sy * hy), nrm, size, 0.008, 6, "bolt")


def emblem(body, glow, c, axis, r):
    """Кольцо резонатора со светящимся ядром — знак семейства."""
    ax = axis.normalized()
    ring(body, c, ax, r * 0.66, r, r * 0.38, SEG, "rustm")
    cyl(glow, c - ax * (r * 0.2), ax, r * 0.58, r * 0.4, SEG, "neon")
    cyl(body, c + ax * (r * 0.2), ax, r * 0.36, r * 0.12, 12, "steel")


def status_strip(glow, a, b, size=0.02):
    bar(glow, a, b, size, "neon")


def cable(part, pts, r=0.025):
    tube(part, catmull([tuple(p) for p in pts], 0.05), r, 8, "cable")


def parts(name):
    return Part(name, "", [], [name]), Part("neon", "", [], ["neon", name])


def tier_part(name):
    return Part("tier", "", [], ["tier", name])


def recolor(part, mat):
    """Весь неон детали — в другой материал (источники: цвет уровня вместо красного)."""
    idx = G.MATS.index(mat)
    for f in part.bm.faces:
        f.material_index = idx


def tier_mark(body, mark, c, n, ex, w=0.11, h=0.03):
    """Метка уровня на устройстве с Эхо: ржавая планка с неоновой полосой цвета уровня (выделение tier)."""
    box(body, c, ex, n, Z, w + 0.025, 0.008, h + 0.022, "rustm")
    plate_bolts(body, c + n * 0.008, n, ex, Z, w + 0.012, h + 0.008, 0.006)
    box(mark, c + n * 0.011, ex, n, Z, w, 0.004, h, TM)


# ---------------------------------------------------------------- 1. ветряк
def wind():
    body, glow = parts("wind")
    rot = Part("rotor", "", [], ["rotor"])
    rglow = Part("neon", "", [], ["neon", "rotor"])
    # основание: восьмигранная плита и шкаф
    prism(body, poly(8, 0.75, math.pi / 8), 0.0, 0.18, "rustm")
    frustum(body, poly(8, 0.62, math.pi / 8), poly(8, 0.38, math.pi / 8), 0.18, 0.55, "steel")
    bolts_ring(body, V(0, 0, 0.18), Z, 0.68, 8, 0.02, 0.012)
    box(body, V(0, -0.52, 0.42), X, Y, Z, 0.26, 0.12, 0.24, "rustm")
    box(body, V(0, -0.645, 0.46), X, Y, Z, 0.18, 0.006, 0.12, "perf")
    status_strip(glow, V(-0.2, -0.646, 0.25), V(0.2, -0.646, 0.25), 0.025)
    # мачта: две трубы с фланцами
    cyl(body, V(0, 0, 0.55), Z, 0.16, 1.75, SEG, "steel")
    ring(body, V(0, 0, 2.30), Z, 0.15, 0.24, 0.07, SEG, "rustm")
    bolts_ring(body, V(0, 0, 2.34), Z, 0.20, 8)
    cyl(body, V(0, 0, 2.30), Z, 0.12, 1.75, SEG, "steel")
    ring(body, V(0, 0, 4.05), Z, 0.11, 0.22, 0.10, SEG, "rustm")
    # растяжки-подкосы
    for k in range(4):
        a = math.pi / 4 + k * math.pi / 2
        d = V(math.cos(a), math.sin(a), 0)
        bar(body, d * 0.62 + V(0, 0, 0.2), d * 0.14 + V(0, 0, 1.55), 0.07, "rustm")
    # неоновая лента по мачте
    status_strip(glow, V(0, -0.165, 0.7), V(0, -0.165, 2.2), 0.03)
    # ротор: вал, спицы, три винтовые лопасти (вращаются — своё выделение rotor)
    cyl(rot, V(0, 0, 4.05), Z, 0.07, 2.45, 16, "steel")
    z0, z1, R = 4.35, 6.25, 0.95
    for k in range(3):
        a0 = 2 * math.pi * k / 3
        for z, a in ((z0, a0), (z1, a0 + math.radians(70))):
            d = V(math.cos(a), math.sin(a), 0)
            bar(rot, V(0, 0, z), d * R + V(0, 0, z), 0.06, "rustm")
        steps = 14
        rings = []
        for i in range(steps + 1):
            t = i / steps
            a = a0 + math.radians(70) * t
            z = z0 + (z1 - z0) * t
            rd = V(math.cos(a), math.sin(a), 0)
            tg = V(-math.sin(a), math.cos(a), 0)
            c = rd * R + V(0, 0, z)
            w, th = 0.13, 0.035
            rings.append([c + tg * w + rd * th, c - tg * w + rd * th, c - tg * w - rd * th, c + tg * w - rd * th])
        sweep(rot, rings, "plate")
        tube(rglow, [r[0].lerp(r[3], 0.5) + (r[0] - r[3]).normalized() * 0.02 for r in rings], 0.018, 6, "neon")
    # колпак и маяк
    cyl(rot, V(0, 0, 6.50), Z, 0.16, 0.08, 16, "rustm")
    emblem(rot, rglow, V(0, 0, 6.62), Z, 0.14)
    recolor(glow, TM)          # источник: через него идёт ток — неон цвета уровня, не красный
    recolor(rglow, TM)
    return [body, glow, rot, rglow]


# ---------------------------------------------------------------- 2. солнечная панель
def solar():
    body, glow = parts("solar")
    head = Part("panel", "", [], ["panel"])
    hglow = Part("neon", "", [], ["neon", "panel"])
    # крестовая опора на болтах и стойка
    for d in (X, Y):
        bar(body, -d * 0.75 + V(0, 0, 0.06), d * 0.75 + V(0, 0, 0.06), 0.12, "rustm")
    for sx in (-1, 1):
        cyl(body, V(sx * 0.70, 0, 0.0), Z, 0.07, 0.02, 8, "bolt")
        cyl(body, V(0, sx * 0.70, 0.0), Z, 0.07, 0.02, 8, "bolt")
    cyl(body, V(0, 0, 0.12), Z, 0.09, 1.25, 16, "steel")
    ring(body, V(0, 0, 0.25), Z, 0.085, 0.14, 0.08, 16, "rustm")
    # распределительная коробка с неоном
    box(body, V(0, -0.16, 0.75), X, Y, Z, 0.14, 0.07, 0.18, "rustm")
    status_strip(glow, V(-0.1, -0.232, 0.66), V(0.1, -0.232, 0.66), 0.02)
    cable(body, [V(0, -0.16, 0.57), V(0, -0.3, 0.3), V(0, -0.5, 0.02)], 0.02)
    # поворотный узел и рама панели под 35°
    cyl(body, V(0, 0, 1.37), Z, 0.14, 0.12, 16, "rustm")
    tilt = math.radians(35)
    ey = V(0, math.cos(tilt), math.sin(tilt))
    nz = V(0, -math.sin(tilt), math.cos(tilt))
    c = V(0, 0, 1.62)
    box(head, c - nz * 0.06, X, ey, nz, 0.09, 0.09, 0.08, "rustm")
    hx, hy = 1.05, 0.68
    for sy in (-1, 1):
        bar(head, c + ey * (sy * hy) - X * hx, c + ey * (sy * hy) + X * hx, 0.07, "steel")
    for sx in (-1, 0, 1):
        bar(head, c + X * (sx * hx) - ey * hy, c + X * (sx * hx) + ey * hy, 0.07, "steel")
    # ячейки: тёмные пластины с красной сеткой между ними
    nx, ny = 6, 4
    cw, ch = (2 * hx - 0.08) / nx, (2 * hy - 0.08) / ny
    for i in range(nx):
        for j in range(ny):
            cc = c + X * (-hx + 0.04 + cw * (i + 0.5)) + ey * (-hy + 0.04 + ch * (j + 0.5)) + nz * 0.02
            box(head, cc, X, ey, nz, cw / 2 - 0.012, ch / 2 - 0.012, 0.012, "flare")
    for i in range(1, nx):
        x = -hx + 0.04 + cw * i
        bar(hglow, c + X * x - ey * (hy - 0.04) + nz * 0.024, c + X * x + ey * (hy - 0.04) + nz * 0.024, 0.008, "neon")
    for j in range(1, ny):
        y = -hy + 0.04 + ch * j
        bar(hglow, c + ey * y - X * (hx - 0.04) + nz * 0.024, c + ey * y + X * (hx - 0.04) + nz * 0.024, 0.008, "neon")
    emblem(head, hglow, c - nz * 0.16 - ey * 0.0, -nz, 0.08)
    recolor(glow, TM)          # источник тока — цвет уровня
    recolor(hglow, TM)
    return [body, glow, head, hglow]


# ---------------------------------------------------------------- 3. резонансная печь
def furnace():
    body, glow = parts("furnace")
    R = 0.52
    for k in range(4):
        a = math.pi / 4 + k * math.pi / 2
        d = V(math.cos(a), math.sin(a), 0)
        bar(body, d * (R - 0.05) + V(0, 0, 0.0), d * (R - 0.1) + V(0, 0, 0.32), 0.09, "rustm")
        cyl(body, d * (R - 0.05), Z, 0.08, 0.025, 8, "rustm")
    cyl(body, V(0, 0, 0.28), Z, R, 1.05, SEG, "rustm")
    for z in (0.30, 0.78, 1.30):
        ring(body, V(0, 0, z), Z, R - 0.01, R + 0.035, 0.07, SEG, "steel")
        bolts_ring(body, V(0, 0, z + 0.035), Z, R + 0.02, 12, 0.012, 0.01)
    # купол и труба
    frustum(body, poly(SEG, R), poly(SEG, R * 0.55), 1.33, 1.55, "steel")
    cyl(body, V(0, 0, 1.55), Z, R * 0.55, 0.06, SEG, "rustm")
    cyl(body, V(0.15, 0.1, 1.6), Z, 0.12, 1.15, 16, "pipe")
    cyl(body, V(0.15, 0.1, 2.72), Z, 0.18, 0.05, 16, "rustm")
    cyl(body, V(0.15, 0.1, 2.85), Z, 0.20, 0.03, 16, "steel")
    for k in range(3):
        a = 2 * math.pi * k / 3
        bar(body, V(0.15, 0.1, 2.75) + V(math.cos(a), math.sin(a), 0) * 0.17, V(0.15, 0.1, 2.85) + V(math.cos(a), math.sin(a), 0) * 0.17, 0.025, "steel")
    # катушки резонатора у основания трубы
    for z in (1.85, 2.10):
        ring(body, V(0.15, 0.1, z), Z, 0.13, 0.22, 0.08, 16, "rustm")
        ring(glow, V(0.15, 0.1, z), Z, 0.22, 0.235, 0.05, 16, "neon")
    # топка: дверца со светящимся окном и решёткой
    tilt_c = V(0, -R - 0.02, 0.62)
    box(body, tilt_c, X, Y, Z, 0.26, 0.04, 0.22, "steel")
    box(glow, tilt_c + V(0, -0.045, 0.03), X, Y, Z, 0.17, 0.006, 0.11, "neon")
    for x in (-0.12, -0.06, 0.0, 0.06, 0.12):
        bar(body, tilt_c + V(x, -0.055, -0.09), tilt_c + V(x, -0.055, 0.15), 0.018, "steel")
    plate_bolts(body, tilt_c + V(0, -0.045, 0), -Y, X, Z, 0.23, 0.19)
    bar(body, tilt_c + V(0.30, -0.06, -0.08), tilt_c + V(0.30, -0.06, 0.12), 0.03, "bolt")
    # загрузочный бункер сбоку (отсек топлива) с крышкой
    hb = V(R + 0.22, 0, 0.95)
    frustum(body, chamfer_rect(0.22, 0.24, 0.04), chamfer_rect(0.26, 0.28, 0.04), 0.75, 1.15, "rustm", hb.x, hb.y)
    box(body, V(hb.x, hb.y, 1.18), X, Y, Z, 0.28, 0.30, 0.025, "steel")
    bar(body, V(hb.x + 0.2, -0.2, 1.22), V(hb.x + 0.2, 0.2, 1.22), 0.025, "bolt")
    status_strip(glow, V(R + 0.005, -0.3, 1.05), V(R + 0.005, 0.3, 1.05), 0.02)
    emblem(body, glow, V(-R - 0.02, 0, 0.95), -X, 0.13)
    recolor(glow, TM)          # источник тока — цвет уровня
    return [body, glow]


# ---------------------------------------------------------------- 4. преобразователь
def converter():
    body, glow = parts("converter")
    prism(body, chamfer_rect(0.66, 0.56, 0.08), 0.0, 0.16, "rustm")
    prism(body, chamfer_rect(0.55, 0.45, 0.06), 0.16, 1.02, "steel")
    for sx in (-1, 1):
        for sy in (-1, 1):
            bar(body, V(sx * 0.54, sy * 0.44, 0.16), V(sx * 0.54, sy * 0.44, 1.04), 0.09, "rustm")
            for z in (0.3, 0.9):
                cyl(body, V(sx * 0.585, sy * 0.44, z), V(sx, 0, 0), 0.016, 0.01, 6, "bolt")
    prism(body, chamfer_rect(0.6, 0.5, 0.07), 1.02, 1.10, "rustm")
    # стеклянная колба с парящим кристаллом и кольцами резонатора
    cyl(body, V(0, 0, 1.10), Z, 0.30, 0.06, SEG, "steel")
    cyl(body, V(0, 0, 1.16), Z, 0.26, 0.62, SEG, "glass")
    # кристалл — вставленный в слот (решение пользователя 11-20): та же форма, что у предмета (crystal_shape), ×CONV_CRYSTAL_S
    # под колбу. Кость «crystal» (вращение вокруг crystal_axis, скрытие без кристалла — model.cfg), светящееся —
    # выделение crystal_glow (скрипт красит: Эхо красный, синтез бирюзовый)
    cbody = Part("crystal", "", [], ["crystal"])
    cglow = Part("crystal_glow", "", [], ["crystal_glow", "crystal"])
    crystal_shape(cbody, cglow, CONV_CRYSTAL_S, CONV_CRYSTAL_Z0)
    for z in (1.24, 1.47, 1.70):
        ring(body, V(0, 0, z), Z, 0.26, 0.33, 0.05, SEG, "rustm")
    for k in range(4):
        a = math.pi / 4 + k * math.pi / 2
        d = V(math.cos(a), math.sin(a), 0)
        bar(body, d * 0.30 + V(0, 0, 1.12), d * 0.30 + V(0, 0, 1.80), 0.05, "rustm")
    cyl(body, V(0, 0, 1.78), Z, 0.32, 0.06, SEG, "steel")
    # излучатель сети на крыше
    cyl(body, V(0, 0, 1.84), Z, 0.035, 0.55, 8, "steel")
    ring(body, V(0, 0, 2.20), Z, 0.12, 0.16, 0.03, 16, "rustm")
    ring(glow, V(0, 0, 2.20), Z, 0.09, 0.12, 0.02, 16, "neon")
    cyl(glow, V(0, 0, 2.39), Z, 0.04, 0.05, 8, "neon")
    # лицевая панель: экран, лючок кристалла, лампы
    box(body, V(0, -0.46, 0.68), X, Y, Z, 0.34, 0.012, 0.2, "rustm")
    box(body, V(-0.08, -0.474, 0.70), X, Y, Z, 0.2, 0.006, 0.13, "screen")
    plate_bolts(body, V(0, -0.474, 0.68), -Y, X, Z, 0.31, 0.17)
    for i in range(3):
        cyl(glow, V(0.22, -0.47, 0.78 - i * 0.08), -Y, 0.022, 0.012, 10, "neon")
    box(body, V(0, -0.46, 0.33), X, Y, Z, 0.22, 0.015, 0.1, "steel")
    bar(body, V(-0.12, -0.48, 0.33), V(0.12, -0.48, 0.33), 0.025, "bolt")
    # гнёзда кабелей по бокам — вход тока: цвет уровня (выделение tier); кристалл и выход — красные (Эхо)
    tin = tier_part("converter")
    for sx in (-1, 1):
        for y in (-0.2, 0.0, 0.2):
            cyl(body, V(sx * 0.55, y, 0.5), V(sx, 0, 0), 0.05, 0.08, 10, "rustm")
            cyl(tin, V(sx * 0.63, y, 0.5), V(sx, 0, 0), 0.03, 0.012, 10, TM)
        bar(tin, V(sx * 0.556, -0.35, 0.95), V(sx * 0.556, 0.35, 0.95), 0.025, TM)
    return [body, glow, tin, cbody, cglow]


# ---------------------------------------------------------------- 5. накопитель
STORAGE_LEVELS = 12        # секций столба в каждой колбе (шаг ~8 %); выделения lvl0..lvl11 — как в model.cfg и config.cpp
STORAGE_LAMPS = 8          # ламп шкалы спереди; выделения lamp0..lamp7
STO_CELL_X, STO_CELL_Z = 0.815, 0.64      # гнёзда разрядки ячеек на правом боку: X, низ ячейки
STO_CELL_Y = (-0.075, 0.075)               # Y гнёзд d1, d2


def storage_proxies():
    """Ячейки в гнёздах разрядки накопителя — те же прокси, что у станции (слоты VB_Echo_DrainCell1/2)."""
    return [("vb_eb_st_cell_d%d" % (i + 1), (STO_CELL_X, STO_CELL_Z, y), RAW_IDENTITY) for i, y in enumerate(STO_CELL_Y)]


def storage():
    body, glow = parts("storage")
    hx, hy = 0.75, 0.32
    prism(body, chamfer_rect(hx, hy, 0.05), 0.0, 0.14, "rustm")
    prism(body, chamfer_rect(hx, hy, 0.05), 1.42, 1.52, "rustm")
    for sx in (-1, 1):
        for sy in (-1, 1):
            bar(body, V(sx * (hx - 0.05), sy * (hy - 0.05), 0.14), V(sx * (hx - 0.05), sy * (hy - 0.05), 1.42), 0.07, "steel")
    # три колбы: стекло, светящийся столб уровня заряда, колпаки и бандажи. Столб — STORAGE_LEVELS секций снизу вверх,
    # секция k всех трёх колб — выделение lvl<k> (заполняются вместе, как сообщающиеся сосуды); красит скрипт по заряду
    # (VB_EB_Storage.VB_Paint: заполненное — красное, пустое — серое; решение пользователя 11-20)
    lv = [Part("lvl%d" % k, "", [], ["lvl%d" % k, "storage"]) for k in range(STORAGE_LEVELS)]
    seg_h = 1.04 / STORAGE_LEVELS
    for i in (-1, 0, 1):
        cx = i * 0.46
        cyl(body, V(cx, 0, 0.14), Z, 0.2, 0.08, SEG, "steel")
        cyl(body, V(cx, 0, 0.22), Z, 0.18, 1.08, SEG, "glass")
        for k in range(STORAGE_LEVELS):
            cyl(lv[k], V(cx, 0, 0.24 + k * seg_h + 0.004), Z, 0.14, seg_h - 0.008, SEG, "neon")
        cyl(body, V(cx, 0, 1.30), Z, 0.2, 0.12, SEG, "steel")
        for z in (0.55, 0.95):
            ring(body, V(cx, 0, z), Z, 0.18, 0.21, 0.04, SEG, "rustm")
        bolts_ring(body, V(cx, 0, 1.42), Z, 0.12, 6, 0.012, 0.01)
    # трубы-перемычки сверху
    tube(body, [V(-0.46, 0, 1.55), V(-0.46, 0, 1.68), V(0.46, 0, 1.68), V(0.46, 0, 1.55)], 0.04, 10, "pipe")
    cyl(body, V(0, 0, 1.52), Z, 0.04, 0.16, 10, "pipe")
    emblem(body, glow, V(0, 0, 1.72), Z, 0.12)
    # шкала уровня спереди: 8 ламп слева направо, лампа k — выделение lamp<k> (горит столько, сколько заряда)
    box(body, V(0, -hy - 0.01, 0.07), X, Y, Z, 0.4, 0.012, 0.05, "steel")
    lamps = [Part("lamp%d" % k, "", [], ["lamp%d" % k, "storage"]) for k in range(STORAGE_LAMPS)]
    for k in range(STORAGE_LAMPS):
        cyl(lamps[k], V(-0.32 + k * 0.09, -hy - 0.024, 0.07), -Y, 0.018, 0.01, 8, "neon")
    mark = tier_part("storage")
    tier_mark(body, mark, V(0, -hy - 0.012, 1.47), -Y, X)
    # полочка разрядки ячеек на правом боку (идея пользователя 11-20, как гнёзда ▲ станции): плита на стойках,
    # 2 стакана со светящимся ободком, прижимы, стрелки вверх (энергия — в запас сети). Ячейки — на прокси
    # (storage_proxies, те же модели-заглушки vb_eb_st_cell_d*, что у станции)
    box(body, V(hx - 0.01, 0, 0.78), X, Y, Z, 0.008, 0.16, 0.27, "steel")
    plate_bolts(body, V(hx - 0.002, 0, 0.78), X, Y, Z, 0.14, 0.25, 0.008)
    box(body, V(STO_CELL_X, 0, STO_CELL_Z - 0.03), X, Y, Z, 0.07, 0.15, 0.015, "rustm")
    for sy in (-1, 1):
        bar(body, V(STO_CELL_X + 0.05, sy * 0.13, STO_CELL_Z - 0.045), V(hx, sy * 0.13, STO_CELL_Z - 0.17), 0.02, "rustm")
    for y in STO_CELL_Y:
        cyl(body, V(STO_CELL_X, y, STO_CELL_Z - 0.01), Z, 0.044, 0.05, 16, "rustm")
        ring(glow, V(STO_CELL_X, y, STO_CELL_Z + 0.036), Z, 0.037, 0.045, 0.005, 16, "neon")
        box(body, V(hx + 0.012, y, STO_CELL_Z + 0.12), X, Y, Z, 0.02, 0.022, 0.012, "steel")
        zt = STO_CELL_Z + 0.27
        a = glow.bm.verts.new(V(hx + 0.002, y - 0.035, zt - 0.025))
        b = glow.bm.verts.new(V(hx + 0.002, y + 0.035, zt - 0.025))
        c = glow.bm.verts.new(V(hx + 0.002, y, zt + 0.03))
        glow.face([a, c, b], "neon")
    return [body, glow, mark] + lv + lamps


# ---------------------------------------------------------------- 6. зарядная станция
# Станция v2 (идея пользователя 11-20): заряжаемое вешается на станцию (вложения, без отсека), видно на модели.
# Гнёзда: 2 бака стоя (бак Echo Runner Ø0,55 × 1,04 м, на машине лежит вдоль), аккумулятор на тумбе справа,
# полка ячеек: 2 на зарядку (стрелки вниз) и 2 на разрядку в сеть (стрелки вверх). Вещи — на прокси (STATION_PROXIES).
ST_TANKS = (-0.52, 0.08)            # X баков; Y центра гнёзд — ST_Y
ST_Y = -0.30
ST_TANK_Z = 0.12                    # низ бака (верх гнезда)
TANK_HALF = 0.521                   # полудлина бака (vb_er_tank.p3d: z ±0.521, радиус 0.272)
ST_BAT = (0.66, 0.44)               # X аккумулятора, низ (верх тумбы)
ST_CELLS = ((0.45, "c1"), (0.57, "c2"), (0.71, "d1"), (0.83, "d2"))     # c — зарядка, d — разрядка
ST_CELL_Y, ST_CELL_Z = -0.03, 0.865


def station():
    body, glow = parts("station")
    # задний шкаф шире прежнего: баки стоят рядом, справа полка ячеек
    prism(body, chamfer_rect(0.88, 0.18, 0.04), 0.0, 1.55, "steel", 0, 0.22)
    for sx in (-1, 1):
        bar(body, V(sx * 0.86, 0.06, 0.0), V(sx * 0.86, 0.06, 1.58), 0.08, "rustm")
    box(body, V(0, 0.22, 1.6), X, Y, Z, 0.92, 0.22, 0.05, "rustm")
    box(body, V(-0.25, 0.035, 1.33), X, Y, Z, 0.32, 0.01, 0.13, "screen")
    plate_bolts(body, V(-0.25, 0.03, 1.33), -Y, X, Z, 0.35, 0.16)
    status_strip(glow, V(-0.84, 0.035, 1.5), V(0.84, 0.035, 1.5), 0.025)
    emblem(body, glow, V(0, 0.22, 1.72), Z, 0.12)
    # плита основания
    prism(body, chamfer_rect(0.9, 0.36, 0.05), 0.0, 0.06, "rustm", 0, ST_Y)
    # гнёзда баков: круглая опора со светящимся кольцом, хомут у верха, кабель от шкафа
    for cx in ST_TANKS:
        prism(body, poly(12, 0.30, math.pi / 12), 0.06, ST_TANK_Z, "rustm", cx, ST_Y)
        ring(glow, V(cx, ST_Y, ST_TANK_Z), Z, 0.24, 0.285, 0.008, SEG, "neon")
        bolts_ring(body, V(cx, ST_Y, ST_TANK_Z), Z, 0.265, 8, 0.012, 0.008)
        zc = ST_TANK_Z + TANK_HALF * 1.6
        ring(body, V(cx, ST_Y, zc), Z, 0.28, 0.31, 0.05, SEG, "rustm")
        bolts_ring(body, V(cx, ST_Y, zc), Z, 0.295, 6, 0.01, 0.06)
        box(body, V(cx, 0.03, zc), X, Y, Z, 0.07, 0.03, 0.04, "steel")
        cable(body, [V(cx - 0.18, 0.04, 0.42), V(cx - 0.24, -0.04, 0.2), V(cx - 0.25, -0.12, ST_TANK_Z + 0.01)], 0.028)
    # тумба аккумулятора: плита-контакт и два светящихся штыря
    bx, bz = ST_BAT
    box(body, V(bx, ST_Y, (0.06 + bz - 0.01) / 2), X, Y, Z, 0.13, 0.2, (bz - 0.01 - 0.06) / 2, "rustm")
    box(body, V(bx, ST_Y, bz - 0.005), X, Y, Z, 0.11, 0.18, 0.005, "flare")
    for sy in (-1, 1):
        cyl(glow, V(bx, ST_Y + sy * 0.16, bz), Z, 0.018, 0.025, 8, "neon")
    status_strip(glow, V(bx - 0.132, ST_Y - 0.17, 0.2), V(bx - 0.132, ST_Y + 0.17, 0.2), 0.02)
    cable(body, [V(bx - 0.1, 0.04, 0.7), V(bx - 0.12, -0.06, 0.4), V(bx - 0.12, -0.12, 0.3)], 0.025)
    # полка ячеек: стаканы со светящимся ободком, прижимы к шкафу, стрелки над парами
    box(body, V(0.64, ST_CELL_Y + 0.01, ST_CELL_Z - 0.03), X, Y, Z, 0.24, 0.07, 0.02, "rustm")
    plate_bolts(body, V(0.64, ST_CELL_Y - 0.06, ST_CELL_Z - 0.03), -Y, X, Z, 0.22, 0.008, 0.006)
    box(body, V(0.64, 0.02, ST_CELL_Z + 0.09), X, Y, Z, 0.006, 0.02, 0.12, "rustm")       # разделитель пар
    for x, kind in ST_CELLS:
        cyl(body, V(x, ST_CELL_Y, ST_CELL_Z - 0.01), Z, 0.044, 0.05, 16, "rustm")
        ring(glow, V(x, ST_CELL_Y, ST_CELL_Z + 0.036), Z, 0.037, 0.045, 0.005, 16, "neon")
        box(body, V(x, 0.015, ST_CELL_Z + 0.12), X, Y, Z, 0.022, 0.02, 0.012, "steel")
        # стрелка: зарядка — вниз (в ячейку), разрядка — вверх (в сеть)
        zt = ST_CELL_Z + 0.25
        d = -1 if kind[0] == "c" else 1
        a = glow.bm.verts.new(V(x - 0.035, 0.033, zt - d * 0.025))
        b = glow.bm.verts.new(V(x + 0.035, 0.033, zt - d * 0.025))
        c = glow.bm.verts.new(V(x, 0.033, zt + d * 0.03))
        glow.face([a, b, c] if d < 0 else [a, c, b], "neon")
    mark = tier_part("station")
    tier_mark(body, mark, V(0.64, -0.005, 1.33), -Y, X)
    return [body, glow, mark]


# прокси станции (оси DayZ: x, вверх, z; z DayZ = Y Blender). Рамка — строки (x, y, z) в «сырых» осях py3d
# (как у навесных Echo Runner): тождественная ((-1,0,0),(0,0,1),(0,1,0)); бак стоя — его ось (z детали) вверх:
# ((-1,0,0),(0,1,0),(0,0,-1)), горловина бака смотрит на игрока.
RAW_IDENTITY = ((-1.0, 0.0, 0.0), (0.0, 0.0, 1.0), (0.0, 1.0, 0.0))
RAW_UPRIGHT = ((-1.0, 0.0, 0.0), (0.0, 1.0, 0.0), (0.0, 0.0, -1.0))
PROXY_ROOT = "\\vb_echobase\\proxy\\"


def station_proxies():
    out = []
    for i, cx in enumerate(ST_TANKS):
        out.append(("vb_eb_st_tank%d" % (i + 1), (cx, ST_TANK_Z + TANK_HALF, ST_Y), RAW_UPRIGHT))
    out.append(("vb_eb_st_battery", (ST_BAT[0], ST_BAT[1], ST_Y), RAW_IDENTITY))
    for x, kind in ST_CELLS:
        out.append(("vb_eb_st_cell_" + kind, (x, ST_CELL_Z, ST_CELL_Y), RAW_IDENTITY))
    return out


# ---------------------------------------------------------------- энергоячейка (VB_EchoCore, идея пользователя 11-20)
def cell():
    """Энергоячейка Эхо: одна модель, уровень — цвет кольца на крышке (выделение tier), заряд — светящееся ядро
    (neon: серое пустое, красное Эхо, бирюзовое синтез). Стеклянная колба в клёпаной клетке, мягкая ручка-петля.
    Питание экзоскелета (будущее) и перенос энергии: заряжается на станции, разряжается в сеть станцией."""
    body, glow = parts("cell")
    mark = tier_part("cell")
    R = 0.034
    cyl(body, V(0, 0, 0.0), Z, R, 0.016, SEG, "rustm")
    cyl(body, V(0, 0, -0.0), Z, 0.012, 0.002, 10, "bolt")
    cyl(glow, V(0, 0, 0.016), Z, 0.022, 0.13, SEG, "neon")
    cyl(body, V(0, 0, 0.016), Z, 0.027, 0.13, SEG, "glass")
    for k in range(4):
        a = math.pi / 4 + k * math.pi / 2
        d = V(math.cos(a), math.sin(a), 0)
        bar(body, d * 0.029 + V(0, 0, 0.016), d * 0.029 + V(0, 0, 0.146), 0.008, "steel")
    ring(body, V(0, 0, 0.075), Z, 0.026, 0.033, 0.012, SEG, "rustm")
    cyl(body, V(0, 0, 0.146), Z, R, 0.02, SEG, "rustm")
    bolts_ring(body, V(0, 0, 0.166), Z, 0.026, 4, 0.004, 0.003)
    ring(mark, V(0, 0, 0.149), Z, R - 0.001, R + 0.003, 0.009, SEG, TM)
    cyl(body, V(0, 0, 0.166), Z, 0.02, 0.006, SEG, "steel")
    cyl(glow, V(0, 0, 0.172), Z, 0.009, 0.004, 12, "neon")
    arc = [V(-0.016, 0, 0.168), V(-0.013, 0, 0.184), V(0, 0, 0.19), V(0.013, 0, 0.184), V(0.016, 0, 0.168)]
    tube(body, catmull([tuple(p) for p in arc], 0.004), 0.0035, 8, "cable")
    return [body, glow, mark]


# ---------------------------------------------------------------- 7. беспроводная платформа
def pad():
    body, glow = parts("pad")
    hx, hy, h = 2.0, 3.0, 0.16
    frustum(body, chamfer_rect(hx, hy, 0.25), chamfer_rect(hx - 0.25, hy - 0.25, 0.2), 0.0, h, "plate")
    # сетка неона на верхней плоскости
    ix, iy = hx - 0.45, hy - 0.45
    for k in range(5):
        x = -ix + 2 * ix * k / 4
        bar(glow, V(x, -iy, h + 0.012), V(x, iy, h + 0.012), 0.03, "neon")
    for k in range(7):
        y = -iy + 2 * iy * k / 6
        bar(glow, V(-ix, y, h + 0.012), V(ix, y, h + 0.012), 0.03, "neon")
    # кромка-бордюр (только стенка, без крышки — иначе закрывает сетку) и болты
    sweep(body, [[V(x, y, h) for x, y in chamfer_rect(hx - 0.25, hy - 0.25, 0.2)],
                 [V(x, y, h + 0.04) for x, y in chamfer_rect(hx - 0.25, hy - 0.25, 0.2)]], "rustm", cap=False)
    for sx in (-1, 1):
        for k in range(7):
            cyl(body, V(sx * (hx - 0.12), -hy + 0.4 + k * (2 * hy - 0.8) / 6, h * 0.5), Z, 0.025, 0.09, 6, "bolt")
    # центральный излучатель
    ring(body, V(0, 0, h + 0.02), Z, 0.45, 0.6, 0.05, SEG, "rustm")
    cyl(glow, V(0, 0, h), Z, 0.42, 0.035, SEG, "neon")
    # угловые пилоны: лампы — метка уровня (выделение tier)
    mark = tier_part("pad")
    for sx in (-1, 1):
        for sy in (-1, 1):
            c = V(sx * (hx - 0.35), sy * (hy - 0.35), 0)
            prism(body, chamfer_rect(0.12, 0.12, 0.03), h, h + 0.5, "steel", c.x, c.y)
            cyl(mark, c + V(0, 0, h + 0.5), Z, 0.08, 0.06, 12, TM)
            cyl(body, c + V(0, 0, h + 0.56), Z, 0.1, 0.03, 12, "rustm")
    return [body, glow, mark]


# ---------------------------------------------------------------- 8. ретранслятор
def relay():
    body, glow = parts("relay")
    for k in range(3):
        a = 2 * math.pi * k / 3 + math.pi / 2
        d = V(math.cos(a), math.sin(a), 0)
        bar(body, d * 0.7 + V(0, 0, 0.03), V(0, 0, 0.9), 0.06, "rustm")
        cyl(body, d * 0.7, Z, 0.08, 0.04, 8, "rustm")
    cyl(body, V(0, 0, 0.0), Z, 0.08, 3.4, 12, "steel")
    ring(body, V(0, 0, 0.9), Z, 0.07, 0.13, 0.08, 12, "rustm")
    box(body, V(0, -0.14, 1.25), X, Y, Z, 0.16, 0.07, 0.22, "rustm")
    status_strip(glow, V(0, -0.212, 1.08), V(0, -0.212, 1.30), 0.02)
    mark = tier_part("relay")
    tier_mark(body, mark, V(0, -0.212, 1.40), -Y, X, 0.09, 0.025)
    # антенна: два кольца, спицы, шпиль и маяк
    for z, r in ((3.0, 0.45), (3.3, 0.32)):
        ring(body, V(0, 0, z), Z, r - 0.03, r, 0.04, SEG, "rustm")
        ring(glow, V(0, 0, z - 0.03), Z, r - 0.025, r - 0.005, 0.015, SEG, "neon")
        for k in range(4):
            a = k * math.pi / 2
            bar(body, V(0, 0, z), V(math.cos(a), math.sin(a), 0) * (r - 0.02) + V(0, 0, z), 0.025, "steel")
    cyl(body, V(0, 0, 3.4), Z, 0.03, 0.6, 8, "steel")
    emblem(body, glow, V(0, 0, 3.45), Z, 0.1)
    cyl(glow, V(0, 0, 4.0), Z, 0.05, 0.06, 10, "neon")
    return [body, glow, mark]


# ---------------------------------------------------------------- 9. кристалл
CONV_CRYSTAL_S = 3.0           # кристалл в колбе преобразователя — форма предмета, увеличенная под колбу (0,45 м)
CONV_CRYSTAL_Z0 = 1.245        # низ кристалла: середина по высоте колбы (1,16–1,78); ось — memory crystal_axis


def crystal_shape(body, glow, s=1.0, z0=0.0):
    """Шестигранная бипирамида-кристалл с двумя обручами: предмет (s = 1) и он же в колбе преобразователя."""
    h, r = 0.075 * s, 0.022 * s
    top, bot = V(0, 0, z0 + h * 2), V(0, 0, z0)
    mid = [V(math.cos(a) * r, math.sin(a) * r, z0 + h * 1.15) for a in [2 * math.pi * k / 6 for k in range(6)]]
    low = [V(math.cos(a) * r * 0.8, math.sin(a) * r * 0.8, z0 + h * 0.35) for a in [2 * math.pi * k / 6 + 0.3 for k in range(6)]]
    bm = glow.bm
    for i in range(6):
        j = (i + 1) % 6
        glow.face([bm.verts.new(top), bm.verts.new(mid[i]), bm.verts.new(mid[j])], "neon")
        glow.face([bm.verts.new(mid[i]), bm.verts.new(low[i]), bm.verts.new(low[j]), bm.verts.new(mid[j])], "neon")
        glow.face([bm.verts.new(bot), bm.verts.new(low[j]), bm.verts.new(low[i])], "neon")
    ring(body, V(0, 0, z0 + h * 0.75), Z, r * 0.95, r * 1.25, 0.012 * s, 12, "rustm")
    ring(body, V(0, 0, z0 + h * 1.25), Z, r * 0.95, r * 1.25, 0.008 * s, 12, "steel")


def crystal():
    body, glow = parts("crystal")
    crystal_shape(body, glow)
    return [body, glow]


# ---------------------------------------------------------------- 10. набор (кит) — одна модель на все устройства
def kit():
    """Транспортный кейс: тёмный клёпаный корпус, ржавые уголки, крышка со знаком семейства, неон по шву,
    ручки по бокам, замки и табличка спереди, полозья снизу. Как кит станка DRL: одна модель, разные названия.
    Габариты и оси — как у ванильного деревянного ящика WoodenCrate (dz\gear\camping\wooden_case.p3d: x ±0.21,
    вверх 0..0.26, длина ±0.40, начало у дна): в руках держится его хватом (ik heavy/wooden_crate.anm), решение
    пользователя 11-20 — прежний кейс 0,88 × 0,5 м был слишком большим."""
    body, glow = parts("kit")
    hx, hy, H = 0.205, 0.39, 0.26          # полуширина (X), полудлина (Y — длинная ось), высота
    zl = H - 0.045                         # низ крышки
    # полозья вдоль длины
    for sx in (-1, 1):
        bar(body, V(sx * (hx - 0.05), -hy + 0.04, 0.015), V(sx * (hx - 0.05), hy - 0.04, 0.015), 0.03, "rustm")
    # корпус и крышка
    prism(body, chamfer_rect(hx, hy, 0.03), 0.03, zl, "steel")
    prism(body, chamfer_rect(hx + 0.008, hy + 0.008, 0.032), zl, H - 0.015, "rustm")
    frustum(body, chamfer_rect(hx + 0.008, hy + 0.008, 0.032), chamfer_rect(hx - 0.02, hy - 0.02, 0.03), H - 0.015, H, "rustm")
    # неон по шву крышки
    for sy in (-1, 1):
        bar(glow, V(-hx + 0.04, sy * (hy + 0.003), zl - 0.004), V(hx - 0.04, sy * (hy + 0.003), zl - 0.004), 0.009, "neon")
    for sx in (-1, 1):
        bar(glow, V(sx * (hx + 0.003), -hy + 0.04, zl - 0.004), V(sx * (hx + 0.003), hy - 0.04, zl - 0.004), 0.009, "neon")
    # уголки на болтах
    for sx in (-1, 1):
        for sy in (-1, 1):
            c = V(sx * (hx - 0.008), sy * (hy - 0.008), 0)
            bar(body, c + V(0, 0, 0.03), c + V(0, 0, H - 0.015), 0.04, "rustm")
            for z in (0.08, 0.17):
                cyl(body, V(sx * (hx + 0.012), sy * (hy - 0.022), z), V(sx, 0, 0), 0.006, 0.005, 6, "bolt")
    # рёбра жёсткости по длинным бокам
    for sx in (-1, 1):
        for z in (0.09, 0.15):
            bar(body, V(sx * (hx + 0.006), -hy + 0.05, z), V(sx * (hx + 0.006), hy - 0.05, z), 0.014, "rustm")
    # ручки по бокам (за них ваниль держит ящик)
    for sx in (-1, 1):
        x = sx * (hx + 0.008)
        tube(body, catmull([(x, -0.09, 0.15), (x + sx * 0.035, -0.075, 0.15), (x + sx * 0.042, 0.0, 0.15),
                            (x + sx * 0.035, 0.075, 0.15), (x, 0.09, 0.15)], 0.015), 0.009, 8, "rustm")
        box(body, V(x, 0, 0.15), X, Y, Z, 0.006, 0.1, 0.022, "steel")
    # замки и табличка спереди (торец -Y)
    for sx in (-1, 1):
        box(body, V(sx * 0.12, -hy - 0.009, zl), X, Y, Z, 0.025, 0.009, 0.035, "bolt")
        bar(body, V(sx * 0.12, -hy - 0.019, zl - 0.02), V(sx * 0.12, -hy - 0.019, zl + 0.02), 0.009, "steel")
    box(body, V(0, -hy - 0.005, 0.11), X, Y, Z, 0.11, 0.005, 0.045, "plate")
    plate_bolts(body, V(0, -hy - 0.009, 0.11), -Y, X, Z, 0.095, 0.032, 0.006)
    status_strip(glow, V(-0.08, -hy - 0.011, 0.09), V(0.08, -hy - 0.011, 0.09), 0.01)
    # знак семейства на крышке
    emblem(body, glow, V(0, 0, H), Z, 0.07)
    recolor(glow, TM)          # в наборе энергии нет — неон показывает уровень устройства внутри
    return [body, glow]


BUILDERS = {"wind": wind, "solar": solar, "furnace": furnace, "converter": converter, "storage": storage,
            "station": station, "pad": pad, "relay": relay, "crystal": crystal, "kit": kit, "cell": cell}


# ---------------------------------------------------------------- геометрия, память, запись
# Geometry / View / Fire: выпуклые блоки (каждый — свой Component). ("box", cx, cy, z0, z1, hx, hy) или
# ("oct", cx, cy, z0, z1, r) — восьмигранная призма, ("tbox", c, ex, ey, ez, hx, hy, hz) — повёрнутый ящик.
# Ротор ветряка, антенны, кабели, люльки — без столкновений.
GEO = {
    "wind": [("oct", 0, 0, 0.0, 0.55, 0.75), ("box", 0, 0, 0.55, 4.05, 0.17, 0.17)],
    "solar": [("box", 0, 0, 0.0, 0.14, 0.78, 0.07), ("box", 0, 0, 0.0, 0.14, 0.07, 0.78), ("box", 0, 0, 0.12, 1.5, 0.1, 0.1),
              ("panel",)],
    "furnace": [("oct", 0, 0, 0.0, 1.56, 0.56), ("box", 0.74, 0, 0.75, 1.2, 0.28, 0.3), ("box", 0.15, 0.1, 1.55, 2.9, 0.2, 0.2)],
    "converter": [("box", 0, 0, 0.0, 1.10, 0.66, 0.56), ("oct", 0, 0, 1.10, 1.84, 0.34)],
    "storage": [("box", 0, 0, 0.0, 1.76, 0.75, 0.33)],
    "station": [("box", 0, 0.22, 0.0, 1.65, 0.92, 0.22), ("box", 0, -0.30, 0.0, 0.12, 0.9, 0.36),
                ("box", 0.66, -0.30, 0.12, 0.44, 0.13, 0.2)],
    "pad": [("box", 0, 0, 0.0, 0.16, 2.0, 3.0)] + [("box", sx * 1.65, sy * 2.65, 0.16, 0.72, 0.12, 0.12)
                                                for sx in (-1, 1) for sy in (-1, 1)],
    "relay": [("box", 0, 0, 0.0, 1.0, 0.35, 0.35), ("box", 0, 0, 1.0, 3.4, 0.09, 0.09)],
    "crystal": [("box", 0, 0, 0.0, 0.15, 0.03, 0.03)],
    "kit": [("box", 0, 0, 0.0, 0.26, 0.215, 0.40)],
    "cell": [("oct", 0, 0, 0.0, 0.19, 0.035)],
}
MASS = {"wind": 120, "solar": 60, "furnace": 150, "converter": 140, "storage": 160, "station": 120, "pad": 300,
        "relay": 50, "crystal": 0.3, "kit": 15, "cell": 0.4}
OLD_PREFIX = "vb_echorunner\\data\\parts\\"


def relink(data):
    pts, faces = data
    return pts, [(i, uv, n, (t.replace(OLD_PREFIX, EB), r.replace(OLD_PREFIX, EB))) for i, uv, n, (t, r) in faces]


def lod(parts_list, res):
    l = py3d.LOD()
    l.resolution = res
    for p in parts_list:
        if p.bm.faces:
            append(l, relink(part_data(p)), p.sels)
    return l


def geo_parts(name):
    out = []
    for k, g in enumerate(GEO[name]):
        p = Part("geo", "", [], ["Component%02d" % (k + 1)])
        if g[0] == "box":
            _, cx, cy, z0, z1, hx, hy = g
            prism(p, [(hx, -hy), (hx, hy), (-hx, hy), (-hx, -hy)], z0, z1, "steel", cx, cy)
        elif g[0] == "oct":
            _, cx, cy, z0, z1, r = g
            prism(p, poly(8, r, math.pi / 8), z0, z1, "steel", cx, cy)
        elif g[0] == "panel":                       # панель солнечной батареи под 35°
            tilt = math.radians(35)
            ey = V(0, math.cos(tilt), math.sin(tilt))
            nz = V(0, -math.sin(tilt), math.cos(tilt))
            box(p, V(0, 0, 1.62), X, ey, nz, 1.1, 0.72, 0.06, "steel")
        out.append(p)
    return out


def geo_lod(name, res, mass=None):
    l = py3d.LOD()
    l.resolution = res
    for p in geo_parts(name):
        d = part_data(p)
        append(l, (d[0], [(f[0], f[1], f[2], ("", "")) for f in d[1]]), p.sels)
    if mass:
        for pt in l.points:
            pt.mass = mass / len(l.points)
        l.properties["autocenter"] = "0"
    return l


def memory_lod(name, parts_list):
    """Точки (оси DayZ: x, вверх, z): вид в инвентаре, центр и радиус для экономики, габариты; у ветряка — ось ротора."""
    xs, ys, zs = [], [], []
    for p in parts_list:
        for v in p.bm.verts:
            xs.append(v.co.x)
            ys.append(v.co.z)
            zs.append(v.co.y)
    lo = (min(xs), min(ys), min(zs))
    hi = (max(xs), max(ys), max(zs))
    c = tuple((a + b) / 2 for a, b in zip(lo, hi))
    r = max(hi[i] - lo[i] for i in range(3))
    l = py3d.LOD()
    l.resolution = 1e15
    pts = {
        "invview": (c[0] - r * 1.2, c[1] + r * 0.9, c[2] - r * 1.2),
        "ce_center": c,
        "ce_radius": (c[0] + r / 2, c[1], c[2]),
        "boundingbox_min": lo,
        "boundingbox_max": hi,
    }
    groups = {k: [v] for k, v in pts.items()}
    if name == "wind":
        groups["rotor_axis"] = [(0.0, 4.05, 0.0), (0.0, 6.7, 0.0)]
    if name == "converter":           # ось кристалла в колбе (вертикаль через центр)
        groups["crystal_axis"] = [(0.0, CONV_CRYSTAL_Z0, 0.0), (0.0, CONV_CRYSTAL_Z0 + 0.15 * CONV_CRYSTAL_S, 0.0)]
    for sel_name, cos in groups.items():
        sel = l.new_selection(sel_name)
        for co in cos:
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
        SEG = seg                                   # упрощение LOD 2/3: меньше граней у крупных цилиндров
        built = fn()
        if first is None:
            first = built
        m.lods.append(lod(built, res))
        if name in ("station", "storage"):          # вложения рисуются на прокси (как навесное машины)
            plist = station_proxies() if name == "station" else storage_proxies()
            for pname, anchor, rows in plist:
                m.lods[-1].add_proxy(PROXY_ROOT + pname, 1, anchor, rows, 0.001, "raw")
    m.lods.append(geo_lod(name, 1e13, MASS[name]))
    m.lods.append(memory_lod(name, first))
    m.lods.append(geo_lod(name, 6e15))
    m.lods.append(geo_lod(name, 7e15))
    out = os.path.join(OUTDIR, ("eb_%s.p3d" % name) if TIER == 1 else ("eb_%s_mk%d.p3d" % (name, TIER)))
    if FUSION:
        out = out[:-4] + "_fusion.p3d"
    m.save(out)
    print("OK", name, "LOD", len(m.lods), "граней LOD1", len(m.lods[0].faces), out)
