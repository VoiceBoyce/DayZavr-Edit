# -*- coding: utf-8 -*-
r"""Антиграв-модуль ДВ-4 Echo Runner (этап 8.1, концепт 02): ставится вместо колеса.

  blender.exe -b --factory-startup --python er_gravmodule.py -- <out.p3d>

Оси модели — как у колеса (прокси колеса): x — наружу (ось колеса), y — вверх, z — вперёд; начало — центр оси колеса.
Строим в Blender (X = x, Y = z, Z = y — вверх). Модуль горизонтальный, висит под аркой: низ излучателя на y = -0.42
(= радиус колеса: на модулях машина стоит на той же высоте), центр сдвинут внутрь на OX, чтобы не торчать из расширителя.
Снизу вверх (взрыв-схема концепта): излучатель -> генератор с кольцевым резонатором -> корпус (8 бронепластин,
4 стабилизатора поля со светящимися прорезями) -> перфорированный верхний кожух -> светящиеся дуги трубок, кронштейн.
Светящееся — выделение «neon» (скрипт гасит/зажигает), остальное — «module».

Кости (model.cfg, класс vb_er_gravmodule — er_finalize.py): «tilt» — весь модуль, наклон по ходу полёта вокруг фланца
(оси tilt_axis_x / tilt_axis_z в памяти); дочерние «rotor» — кольцо резонатора с лопатками и светящимися секциями,
«rotor_b» — крестовина и прорези излучателя, крутится навстречу (ось rotor_axis); «spin» — бронированный корпус
(стенка, 8 пластин, 4 стабилизатора, нижняя полоса, болты) медленно поворачивается (просьба пользователя 11-17: модули
при парении тоже крутятся, медленно). Купол, мачта, трубки и излучатель неподвижны. Точка «light» — под излучателем.
Корневую кость-противоповорот колеса пробовали 11-17 — с ней корпус в игре кувыркался вокруг оси колеса, убрана;
проворот колеса не даёт нейтраль+тормоза (VB_EchoRunner.OnInput). Точка входит только в одну кость (isDiscrete): вращающиеся части — не в «tilt», наклон им даёт родитель.
"""
import math
import os
import sys

import bpy
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import er_common as C  # noqa: E402
from er_geom import Part, V, bar, box, cyl, ring, tube, catmull, part_data, append  # noqa: E402

py3d = C.py3d
args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
OUT = [a for a in args if not a.startswith("--")][0]

OX = -0.19          # сдвиг внутрь (к машине): не выходить далеко за расширители
R_H = 0.43          # радиус корпуса
Z_BOT = -0.42       # низ (контакт с землёй)


def ctr(z=0.0):
    return V(OX, 0.0, z)


def lathe_ring(part, profile, segs, mat):
    """Замкнутый тор-профиль вокруг вертикальной оси через ctr(): profile [(r, z)] — по кругу."""
    bm = part.bm
    rings = []
    for k in range(segs):
        a = 2 * math.pi * k / segs
        ca, sa = math.cos(a), math.sin(a)
        rings.append([bm.verts.new((OX + r * ca, r * sa, z)) for r, z in profile])
    n = len(profile)
    for k in range(segs):
        m = (k + 1) % segs
        for j in range(n):
            jj = (j + 1) % n
            part.face((rings[k][j], rings[m][j], rings[m][jj], rings[k][jj]), mat)


def solid(part, profile, segs, mat):
    """Тело вращения с крышками: profile [(r, z)] от оси до оси (r первой и последней точки ~0)."""
    bm = part.bm
    rings = []
    for k in range(segs):
        a = 2 * math.pi * k / segs
        ca, sa = math.cos(a), math.sin(a)
        rings.append([bm.verts.new((OX + max(r, 1e-4) * ca, max(r, 1e-4) * sa, z)) for r, z in profile])
    n = len(profile)
    for k in range(segs):
        m = (k + 1) % segs
        for j in range(n - 1):
            part.face((rings[k][j], rings[m][j], rings[m][j + 1], rings[k][j + 1]), mat)


def build(level):
    segs = {1: 40, 2: 28, 3: 18, 4: 10}[level]
    body = Part("module", "", [], ["module", "tilt"])
    glow = Part("neon", "", [], ["neon", "module", "tilt"])
    rot = Part("rotor", "", [], ["module", "rotor"])
    rotg = Part("rotor_neon", "", [], ["neon", "module", "rotor"])
    rotb = Part("rotor_b", "", [], ["module", "rotor_b"])
    rotbg = Part("rotor_b_neon", "", [], ["neon", "module", "rotor_b"])
    hous = Part("spin", "", [], ["module", "spin"])
    housg = Part("spin_neon", "", [], ["neon", "module", "spin"])
    up = V(0, 0, 1)
    # --- корпус: стенка, нижний скос, верхний скос
    solid(hous, [(0.0, -0.205), (R_H - 0.03, -0.205), (R_H, -0.18), (R_H, 0.0), (R_H - 0.035, 0.03), (0.0, 0.03)],
          segs, "steel")
    # --- перфорированный верхний кожух (купол)
    solid(body, [(0.0, 0.02), (0.40, 0.02), (0.37, 0.07), (0.28, 0.115), (0.16, 0.14), (0.0, 0.14)], segs, "perf")
    # --- крышка и мачта кронштейна (уходит вверх в арку)
    solid(body, [(0.0, 0.135), (0.15, 0.135), (0.15, 0.17), (0.11, 0.19), (0.0, 0.19)], max(10, segs // 2), "rustm")
    cyl(body, ctr(0.19), up, 0.055, 0.10, max(8, segs // 3), "steel")
    box(body, ctr(0.30), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.10, 0.03, 0.012, "rustm")          # фланец
    # --- генератор (вращается целиком) и кольцевой резонатор: 12 светящихся секций и 12 лопаток (видно вращение)
    solid(rot, [(0.0, -0.31), (0.34, -0.31), (0.345, -0.30), (0.345, -0.205), (0.0, -0.205)], segs, "steel")
    nsec = 12
    sub = {1: 4, 2: 3, 3: 2, 4: 1}[level]
    for k in range(nsec):
        a0 = 2 * math.pi * k / nsec
        a1 = a0 + 2 * math.pi / nsec * 0.62
        bm = rotg.bm
        rings = []
        for t in np.linspace(a0, a1, sub + 1):
            ca, sa = math.cos(t), math.sin(t)
            rings.append([bm.verts.new((OX + r * ca, r * sa, z)) for r, z in
                          ((0.343, -0.275), (0.362, -0.275), (0.362, -0.235), (0.343, -0.235))])
        for i in range(sub):
            for j in range(4):
                jj = (j + 1) % 4
                rotg.face((rings[i][j], rings[i + 1][j], rings[i + 1][jj], rings[i][jj]), "neon")
        rotg.face(list(reversed(rings[0])), "neon")
        rotg.face(rings[-1], "neon")
    if level <= 3:
        tw = math.radians(28)                       # разворот лопатки (как у крыльчатки)
        for k in range(nsec):
            a = 2 * math.pi * (k + 0.81) / nsec     # между секциями
            ca, sa = math.cos(a), math.sin(a)
            er_ = V(ca, sa, 0)
            et = V(-sa, ca, 0)
            et2 = et * math.cos(tw) + up * math.sin(tw)
            up2 = up * math.cos(tw) - et * math.sin(tw)
            c = V(OX + 0.378 * ca, 0.378 * sa, -0.255)
            box(rot, c, er_, et2, up2, 0.032, 0.005, 0.042, "rustm")
    # --- излучатель снизу: корпус и обод неподвижны, крестовина и прорези крутятся навстречу резонатору
    solid(body, [(0.0, -0.40), (0.20, -0.40), (0.205, -0.39), (0.205, -0.31), (0.0, -0.31)], max(12, segs * 2 // 3), "rustm")
    lathe_ring(body, [(0.17, Z_BOT), (0.215, Z_BOT), (0.215, -0.395), (0.17, -0.395)], max(12, segs * 2 // 3), "flare")
    solid(glow, [(0.0, Z_BOT + 0.004), (0.16, Z_BOT + 0.004), (0.16, -0.40), (0.0, -0.40)], max(12, segs * 2 // 3), "neon")
    cyl(rotb, ctr(Z_BOT + 0.0015), up, 0.045, 0.012, max(8, segs // 3), "steel")
    for k in range(4):
        a = math.pi / 2 * k + math.radians(10)
        er_ = V(math.cos(a), math.sin(a), 0)
        et = V(-math.sin(a), math.cos(a), 0)
        box(rotb, ctr(Z_BOT + 0.0065) + er_ * 0.105, er_, et, up, 0.062, 0.011, 0.005, "steel")
        if level <= 2:
            # светящиеся прорези на боку излучателя
            p = V(OX + 0.206 * math.cos(a + 0.4), 0.206 * math.sin(a + 0.4), -0.355)
            er2 = V(math.cos(a + 0.4), math.sin(a + 0.4), 0)
            et2 = V(-math.sin(a + 0.4), math.cos(a + 0.4), 0)
            box(rotbg, p, er2, et2, up, 0.003, 0.016, 0.03, "neon")
    if level <= 2:
        # --- 8 бронепластин по стенке корпуса, с заклёпками
        for k in range(8):
            a0 = 2 * math.pi * k / 8 + math.radians(3)
            a1 = 2 * math.pi * (k + 1) / 8 - math.radians(3)
            arc = []
            for t in np.linspace(a0, a1, 4 if level == 1 else 2):
                arc.append((math.cos(t), math.sin(t)))
            bm = hous.bm
            ring_top, ring_bot, ring_top_i, ring_bot_i = [], [], [], []
            for (ca, sa) in arc:
                ring_top.append(bm.verts.new((OX + (R_H + 0.014) * ca, (R_H + 0.014) * sa, -0.01)))
                ring_bot.append(bm.verts.new((OX + (R_H + 0.014) * ca, (R_H + 0.014) * sa, -0.175)))
                ring_top_i.append(bm.verts.new((OX + (R_H - 0.004) * ca, (R_H - 0.004) * sa, -0.01)))
                ring_bot_i.append(bm.verts.new((OX + (R_H - 0.004) * ca, (R_H - 0.004) * sa, -0.175)))
            for i in range(len(arc) - 1):
                hous.face((ring_bot[i], ring_bot[i + 1], ring_top[i + 1], ring_top[i]), "rustm")
                hous.face((ring_top[i], ring_top[i + 1], ring_top_i[i + 1], ring_top_i[i]), "rustm")
                hous.face((ring_bot_i[i], ring_bot_i[i + 1], ring_bot[i + 1], ring_bot[i]), "rustm")
                hous.face((ring_top_i[i], ring_top_i[i + 1], ring_bot_i[i + 1], ring_bot_i[i]), "rustm")
            hous.face((ring_bot[0], ring_top[0], ring_top_i[0], ring_bot_i[0]), "rustm")
            hous.face((ring_bot[-1], ring_bot_i[-1], ring_top_i[-1], ring_top[-1]), "rustm")
            if level == 1:
                for (ca, sa) in (arc[0], arc[-1]):
                    for z in (-0.03, -0.155):
                        p = V(OX + (R_H + 0.014) * ca, (R_H + 0.014) * sa, z)
                        cyl(hous, p, V(ca, sa, 0), 0.008, 0.007, 6, "bolt")
        # --- 4 стабилизатора поля со светящимися прорезями
        for k in range(4):
            a = math.radians(45 + 90 * k)
            ca, sa = math.cos(a), math.sin(a)
            er_ = V(ca, sa, 0)
            et = V(-sa, ca, 0)
            c = V(OX + (R_H + 0.045) * ca, (R_H + 0.045) * sa, -0.095)
            box(hous, c, er_, et, up, 0.055, 0.075, 0.085, "steel")
            box(hous, c + er_ * 0.058 + up * 0.0, er_, et, up, 0.004, 0.06, 0.07, "rustm")
            for j in (-1, 0, 1):
                box(housg, c + er_ * 0.063 + et * (j * 0.032), er_, et, up, 0.002, 0.009, 0.05, "neon")
            if level == 1:
                for (s1, s2) in ((-1, -1), (-1, 1), (1, -1), (1, 1)):
                    cyl(hous, c + er_ * 0.062 + et * (s1 * 0.05) + up * (s2 * 0.068), er_, 0.006, 0.006, 6, "bolt")
        # --- светящаяся полоса по низу корпуса (прорези, как на концепте)
        lathe_ring(housg, [(R_H - 0.006, -0.205), (R_H + 0.006, -0.205), (R_H + 0.006, -0.192), (R_H - 0.006, -0.192)],
                   segs, "neon")
        # --- болты по верхнему краю
        if level == 1:
            for k in range(16):
                a = 2 * math.pi * (k + 0.5) / 16
                cyl(hous, V(OX + 0.385 * math.cos(a), 0.385 * math.sin(a), 0.022), up, 0.009, 0.008, 6, "bolt")
        # --- дуги светящихся трубок сверху и гофра
        for sgn in (1, -1):
            a0, a1 = math.radians(20 if sgn > 0 else 200), math.radians(110 if sgn > 0 else 290)
            pts = []
            for t in np.linspace(0, 1, 7):
                a = a0 + (a1 - a0) * t
                rr = 0.30
                pts.append((OX + rr * math.cos(a), rr * math.sin(a), 0.09 + 0.10 * math.sin(math.pi * t)))
            path = catmull(pts, 0.02 if level == 1 else 0.05)
            tube(glow, path, 0.024, 10 if level == 1 else 6, "neon")
            for e in (path[0], path[-1]):
                cyl(body, e - V(0, 0, 0.02), up, 0.033, 0.03, 10 if level == 1 else 6, "rustm")
            if level == 1:
                mid = path[len(path) // 2]
                cyl(body, mid - (path[len(path) // 2 + 1] - mid).normalized() * 0.02,
                    (path[len(path) // 2 + 1] - mid).normalized(), 0.032, 0.04, 10, "rustm")
        if level == 1:
            hp = [ctr(0.17) + V(0.05, 0.05, 0), V(OX + 0.22, 0.20, 0.12), V(OX + 0.33, 0.33, 0.0)]
            tube(body, catmull(hp, 0.012), 0.016, 8, "hose", rfun=lambda i: 1.0 + 0.18 * (i % 2))
    return [body, glow, hous, housg, rot, rotg, rotb, rotbg]


PIVOT_Y = 0.30      # фланец кронштейна: вокруг него модуль наклоняется


def memory_lod():
    """Точки осей (оси DayZ: x наружу, y вверх, z вперёд колеса)."""
    l = py3d.LOD()
    l.resolution = 1e15
    pts = {
        "rotor_axis": [(OX, -0.5, 0.0), (OX, 0.25, 0.0)],
        "tilt_axis_x": [(OX - 0.1, PIVOT_Y, 0.0), (OX + 0.1, PIVOT_Y, 0.0)],
        "tilt_axis_z": [(OX, PIVOT_Y, -0.1), (OX, PIVOT_Y, 0.1)],
        "light": [(OX, -0.62, 0.0)],
    }
    for name, cs in pts.items():
        newp = []
        for co in cs:
            p = py3d.Point()
            p.coords = co
            l.points.append(p)
            newp.append(p)
        sel = l.new_selection(name)
        sel.points.update({p: 1 for p in newp})
    return l


def lod(parts, res):
    l = py3d.LOD()
    l.resolution = res
    for p in parts:
        if p.bm.faces:
            append(l, part_data(p), p.sels)
    return l


def geo_lod(res, mass=None):
    g = Part("geo", "", [], ["Component01"])
    cyl(g, V(OX, 0, Z_BOT), V(0, 0, 1), R_H + 0.02, 0.57, 12, "steel")
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


bpy.ops.wm.read_factory_settings(use_empty=True)
m = py3d.P3D()
for res in (1.0, 2.0, 3.0, 4.0):
    m.lods.append(lod(build(int(res)), res))
    print("LOD", res, "граней", len(m.lods[-1].faces))
m.lods.append(geo_lod(1e13, 40.0))
m.lods.append(memory_lod())
m.lods.append(geo_lod(6e15))
m.lods.append(geo_lod(7e15))
os.makedirs(os.path.dirname(OUT), exist_ok=True)
m.save(OUT)
print("OK", OUT)
