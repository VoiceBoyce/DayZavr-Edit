# -*- coding: utf-8 -*-
r"""Резонансный аккумулятор семейства VB_Echo (общая деталь всех машин, ТЗ: modding/mod_vb_Echo/ТЗ_VB_Echo.md).

  G:\SteamLibrary\steamapps\common\Blender\blender.exe -b --factory-startup --python ep_battery.py -- <out.p3d>

Габариты и оси — как у ванильного автомобильного аккумулятора (dz\vehicles\parts\battery_car.p3d): x ±0.08, z ±0.125,
низ на y = 0, корпус до 0.165, ручка до 0.24 — чтобы встал в прокси аккумулятора любой машины, в том числе ванильной.
Строим в осях Blender (X = x, Y = z, Z = y — вверх), запись в MLOD — er_geom (как модуль Echo Runner).

Вид: тёмный клёпаный корпус на ржавом основании, угловые стойки на болтах; по длинным бокам светящиеся окна
«конденсата» под решёткой; сверху кольцо резонатора со светящимся ядром (то же кольцо, что у антиграв-модуля, —
знак семейства), две клеммы (плюс — красный колпачок), три лампы уровня и пробка; складная ручка.
Светящееся — выделение «neon» (hiddenSelection: Эхо — красное, холодный синтез — бирюзовое, модель одна).
Материалы — библиотека Echo Runner er_*, пути переписываются на vb_echovehicleparts\data\ (копии лежат в PBO деталей).
"""
import math
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ER_TOOLS = os.path.join(HERE, "..", "..", "mod_vb_EchoRunner", "tools")
sys.path.insert(0, os.path.abspath(ER_TOOLS))
import bpy  # noqa: E402,F401
import er_common as C  # noqa: E402
from er_geom import Part, V, bar, box, cyl, ring, tube, catmull, sweep, part_data, append  # noqa: E402

py3d = C.py3d
args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
OUT = [a for a in args if not a.startswith("--")][0]

SRC_PREFIX = "vb_echorunner\\data\\parts\\"
DST_PREFIX = "vb_echovehicleparts\\data\\"

HX, HY = 0.071, 0.117      # полуразмеры корпуса (X — ширина, Y — длина)
Z_LID = 0.150              # низ крышки
Z_TOP = 0.163              # верх крышки


def chamfer_rect(hx, hy, c):
    return [(hx - c, -hy), (hx, -hy + c), (hx, hy - c), (hx - c, hy),
            (-hx + c, hy), (-hx, hy - c), (-hx, -hy + c), (-hx + c, -hy)]


def prism(part, outline, z0, z1, mat):
    sweep(part, [[V(x, y, z0) for x, y in outline], [V(x, y, z1) for x, y in outline]], mat)


def build(level):
    seg = {1: 24, 2: 14, 3: 8}[level]
    small = {1: 10, 2: 8, 3: 6}[level]
    body = Part("battery", "", [], ["battery"])
    glow = Part("neon", "", [], ["neon", "battery"])

    # --- корпус
    prism(body, chamfer_rect(0.078, 0.124, 0.012), 0.0, 0.012, "rustm")          # основание
    prism(body, chamfer_rect(HX, HY, 0.014), 0.012, Z_LID, "steel")               # корпус
    prism(body, chamfer_rect(0.076, 0.122, 0.012), Z_LID, Z_TOP, "rustm")         # крышка
    for sx in (-1, 1):
        for sy in (-1, 1):
            bar(body, V(sx * 0.066, sy * 0.112, 0.006), V(sx * 0.066, sy * 0.112, 0.166), 0.018, "rustm")
            if level == 1:
                for z in (0.03, 0.13):
                    cyl(body, V(sx * 0.0712, sy * 0.098, z), V(sx, 0, 0), 0.0055, 0.004, 8, "bolt")

    # --- окна «конденсата» по длинным бокам: светящаяся панель, рамка, решётка
    for sx in (-1, 1):
        x = sx * HX
        box(glow, V(x + sx * 0.0005, 0, 0.080), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.0015, 0.068, 0.036, "neon")
        fx = x + sx * 0.003
        for z in (0.042, 0.118):
            bar(body, V(fx, -0.076, z), V(fx, 0.076, z), 0.008, "rustm")
        for y in (-0.072, 0.072):
            bar(body, V(fx, y, 0.038), V(fx, y, 0.122), 0.008, "rustm")
        if level <= 2:
            for y in (-0.048, -0.024, 0.0, 0.024, 0.048):
                bar(body, V(x + sx * 0.0035, y, 0.044), V(x + sx * 0.0035, y, 0.116), 0.005, "steel")

    # --- торцы: перфорированная решётка вентиляции на болтах
    for sy in (-1, 1):
        y = sy * HY
        box(body, V(0, y + sy * 0.0012, 0.085), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.048, 0.0015, 0.048, "perf")
        if level == 1:
            for bx in (-0.042, 0.042):
                for bz in (0.043, 0.127):
                    cyl(body, V(bx, y, bz), V(0, sy, 0), 0.005, 0.004, 8, "bolt")

    # --- верх: кольцо резонатора со светящимся ядром (знак семейства, как у антиграв-модуля)
    ring(body, V(0, 0, Z_TOP + 0.008), V(0, 0, 1), 0.028, 0.042, 0.016, seg, "rustm")
    cyl(glow, V(0, 0, Z_TOP), V(0, 0, 1), 0.024, 0.020, seg, "neon")
    cyl(body, V(0, 0, Z_TOP + 0.020), V(0, 0, 1), 0.015, 0.005, small, "steel")
    for k in range(4):
        a = math.pi / 4 + k * math.pi / 2
        d = V(math.cos(a), math.sin(a), 0)
        bar(body, d * 0.040 + V(0, 0, Z_TOP + 0.008), d * 0.052 + V(0, 0, Z_TOP + 0.008), 0.008, "steel")

    # --- клеммы (сторона +X): основание, штырь, колпачок (+ красный, − чёрный)
    for sy, cap in ((-1, "red"), (1, "hose")):
        c = V(0.044, sy * 0.086, Z_TOP)
        cyl(body, c, V(0, 0, 1), 0.017, 0.008, small, "rustm")
        cyl(body, c + V(0, 0, 0.008), V(0, 0, 1), 0.010, 0.016, small, "bolt")
        cyl(body, c + V(0, 0, 0.022), V(0, 0, 1), 0.014, 0.009, small, cap)

    # --- сторона −X: три лампы уровня заряда и пробка
    for y in (-0.090, -0.068, -0.046):
        cyl(body, V(-0.046, y, Z_TOP), V(0, 0, 1), 0.0085, 0.004, small, "rustm")
        cyl(glow, V(-0.046, y, Z_TOP + 0.004), V(0, 0, 1), 0.0065, 0.004, small, "neon")
    cyl(body, V(-0.046, 0.082, Z_TOP), V(0, 0, 1), 0.013, 0.012, small, "rustm")
    cyl(body, V(-0.046, 0.082, Z_TOP + 0.012), V(0, 0, 1), 0.016, 0.006, small, "bolt")

    # --- неоновые полосы по верхним длинным кромкам
    for sx in (-1, 1):
        bar(glow, V(sx * 0.0735, -0.098, Z_LID - 0.003), V(sx * 0.0735, 0.098, Z_LID - 0.003), 0.005, "neon")

    # --- ручка: крепления, дуга, обрезиненный хват
    for sy in (-1, 1):
        box(body, V(0, sy * 0.104, Z_TOP + 0.006), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.014, 0.008, 0.006, "rustm")
    path = catmull([(0, -0.104, Z_TOP + 0.004), (0, -0.104, 0.200), (0, -0.090, 0.228), (0, -0.060, 0.236),
                    (0, 0.060, 0.236), (0, 0.090, 0.228), (0, 0.104, 0.200), (0, 0.104, Z_TOP + 0.004)],
                   0.012 if level == 1 else 0.03)
    tube(body, path, 0.0065, {1: 10, 2: 8, 3: 6}[level], "rustm")
    tube(body, [V(0, -0.055, 0.2365), V(0, 0.055, 0.2365)], 0.0105, {1: 12, 2: 8, 3: 6}[level], "hose")
    return [body, glow]


def relink(data):
    pts, faces = data
    out = []
    for idx, uvs, n, (tex, rv) in faces:
        out.append((idx, uvs, n, (tex.replace(SRC_PREFIX, DST_PREFIX), rv.replace(SRC_PREFIX, DST_PREFIX))))
    return pts, out


def lod(parts, res):
    l = py3d.LOD()
    l.resolution = res
    for p in parts:
        if p.bm.faces:
            append(l, relink(part_data(p)), p.sels)
    return l


def geo_lod(res, mass=None):
    g = Part("geo", "", [], ["Component01"])
    prism(g, chamfer_rect(0.078, 0.124, 0.014), 0.0, 0.166, "steel")
    d = part_data(g)
    d = (d[0], [(f[0], f[1], f[2], ("", "")) for f in d[1]])
    l = py3d.LOD()
    l.resolution = res
    append(l, d, g.sels)
    if mass:
        for p in l.points:
            p.mass = mass / len(l.points)
        l.properties["autocenter"] = "0"
    return l


def memory_lod():
    """Точки как у ванильного аккумулятора (оси DayZ): вид в инвентаре, центр и радиус для экономики, габариты."""
    l = py3d.LOD()
    l.resolution = 1e15
    pts = {
        "invview": (-0.275, 0.362, 0.251),
        "ce_center": (0.0, 0.08, 0.0),
        "ce_radius": (0.10, 0.20, 0.15),
        "boundingbox_min": (-0.08, 0.0, -0.125),
        "boundingbox_max": (0.08, 0.24, 0.125),
    }
    for name, co in pts.items():
        p = py3d.Point()
        p.coords = co
        l.points.append(p)
        sel = l.new_selection(name)
        sel.points.update({p: 1})
    return l


bpy.ops.wm.read_factory_settings(use_empty=True)
m = py3d.P3D()
for res in (1.0, 2.0, 3.0):
    m.lods.append(lod(build(int(res)), res))
    print("LOD", res, "граней", len(m.lods[-1].faces))
m.lods.append(geo_lod(1e13, 12.0))
m.lods.append(memory_lod())
m.lods.append(geo_lod(6e15))
m.lods.append(geo_lod(7e15))
os.makedirs(os.path.dirname(OUT), exist_ok=True)
m.save(OUT)
print("OK", OUT)
