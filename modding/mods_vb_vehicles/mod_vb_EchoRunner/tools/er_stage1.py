# -*- coding: utf-8 -*-
r"""Echo Runner, этап 1: черновая форма (блокаут) по размерам + рендеры.

Запуск (Blender 5.2, без окна):
  G:\SteamLibrary\steamapps\common\Blender\blender.exe -b --factory-startup --python er_stage1.py -- <папка вывода>

Оси Blender в этом скрипте: X вправо, Y назад (нос машины смотрит в -Y), Z вверх, z=0 — земля.
Перевод в оси DayZ — на этапе сборки .p3d.

Размеры-образец (DMC-12, открытые данные): длина 4267, высота 1140, база 2408, колея 1590/1588,
ширина кузова ~1850 (1989 с зеркалами). Масса ~1230 кг.

План соединений (Connection Map):
  кузов (низ)  <-> «стеклянный» верх: верх утоплен в кузов на 2-4 см по низу
  колёсные арки: вырез только в боковинах (x от 0.55 до 1.05), колесо внутри ниши с зазором >= 4 см
  колёса      <-> земля: центр колеса на высоте радиуса (z = R)
  фары/фонари/бамперы <-> кузов: утоплены в кузов на 1-4 см
Детали (Detail List) — на этапе 2. Здесь только объёмы, стёкла и метки мест под обвесы.
"""
import math
import os
import sys

import bmesh
import bpy
import numpy as np
from mathutils import Vector

ARGS = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
OUT = os.path.abspath(ARGS[0]) if ARGS else os.path.abspath("er_stage1_out")
os.makedirs(OUT, exist_ok=True)

# ------------------------------------------------------------------ размеры
L = 4.267
WB = 2.408
TRACK = 1.59
R = 0.32            # радиус колеса (одно колесо на все 4 оси — один предмет)
TIRE_HW = 0.11      # половина ширины шины
FRONT_OVH = 0.86
Y_FRONT = -L / 2
Y_FA = Y_FRONT + FRONT_OVH          # передняя ось
Y_RA = Y_FA + WB                    # задняя ось

# Сечения кузова: y, низ, верх порога, линия перегиба (самая широкая), плечо, верх по центру;
# полуширины: низ порога, порог, перегиб, плечо.
BODY = [
    # y       zb    zr    zc    zsh   zt     hr    hs    hc     hsh
    (-2.1335, 0.30, 0.33, 0.50, 0.575, 0.59,  0.62, 0.66, 0.70,  0.68),
    (-2.10,   0.25, 0.30, 0.53, 0.615, 0.63,  0.74, 0.79, 0.82,  0.80),
    (-1.92,   0.20, 0.29, 0.57, 0.68,  0.695, 0.83, 0.88, 0.905, 0.87),
    (Y_FA,    0.17, 0.30, 0.60, 0.74,  0.76,  0.86, 0.90, 0.925, 0.89),
    (-0.62,   0.15, 0.30, 0.62, 0.78,  0.80,  0.86, 0.905, 0.925, 0.89),
    (0.50,    0.15, 0.30, 0.63, 0.85,  0.865, 0.86, 0.905, 0.925, 0.89),
    (Y_RA,    0.17, 0.30, 0.64, 0.88,  0.90,  0.86, 0.905, 0.925, 0.895),
    (1.90,    0.20, 0.30, 0.64, 0.89,  0.91,  0.84, 0.885, 0.91,  0.875),
    (2.10,    0.25, 0.31, 0.64, 0.885, 0.905, 0.78, 0.83,  0.86,  0.83),
    (2.1335,  0.29, 0.33, 0.63, 0.87,  0.89,  0.70, 0.75,  0.79,  0.76),
]
# Жёсткость рёбер при сглаживании (0 — плавно, 1 — острое ребро). Индексы точек кольца кузова:
# 0 низ по центру, 1/9 низ порога, 2/8 верх порога, 3/7 перегиб на боку, 4/6 плечо, 5 верх по центру.
BODY_CREASE = {1: 0.6, 9: 0.6, 2: 0.4, 8: 0.4, 3: 1.0, 7: 1.0, 4: 0.75, 6: 0.75}
BODY_END_CREASE = 0.7          # кромка носа и кормы
# Верх: 0/4 низ (по линии плеча), 1/3 край крыши, 2 центр крыши.
GREEN_CREASE = {0: 1.0, 4: 1.0, 1: 0.6, 3: 0.6}
SUBD = 2
# «Стеклянный» верх: y, низ, край крыши, центр крыши, полуширина низа, полуширина крыши.
GREEN = [
    (-0.62, 0.76, 0.785, 0.80,  0.86,  0.84),   # низ лобового стекла (узкая полоска)
    (-0.08, 0.79, 1.12,  1.156, 0.875, 0.64),   # верх лобового = начало крыши (+1,6 см на усадку сглаживания)
    (0.45,  0.82, 1.115, 1.151, 0.88,  0.64),   # конец бокового стекла
    (0.80,  0.84, 1.115, 1.146, 0.88,  0.64),   # конец крыши
    (1.68,  0.86, 0.90,  0.915, 0.875, 0.80),   # низ задних жалюзи (узкая полоска)
]

# Места под обвесы (центры прокси), оси Blender этого скрипта.
MOUNTS = {
    "front":   ((0.0, -2.22, 0.40), (1.0, 0.45, 0.05)),   # Перед — оранжевый
    "roof":    ((0.0, 0.50, 1.16), (0.10, 0.55, 1.0)),    # Крыша — синий
    "rear":    ((0.0, 2.22, 0.42), (0.85, 0.10, 0.85)),   # Корма — фиолетовый
    "can_l":   ((-0.55, 1.88, 0.95), (0.95, 0.85, 0.05)), # Канистра левая — жёлтый
    "can_r":   ((0.55, 1.88, 0.95), (0.95, 0.85, 0.05)),  # Канистра правая — жёлтый
    "sills":   ((0.0, -0.10, 0.22), (0.10, 0.80, 0.25)),  # Пороги — зелёный (деталь на обе стороны)
    "engine":  ((0.0, 1.72, 0.78), (0.95, 0.10, 0.10)),   # Мотор — красный
    "under":   ((0.0, 0.0, 0.12), (0.05, 0.90, 0.95)),    # Днище — бирюзовый
}

# ------------------------------------------------------------------ сцена
bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene
COL = scene.collection


def link_mesh(name, bm, mats=None):
    me = bpy.data.meshes.new(name)
    bm.to_mesh(me)
    bm.free()
    obj = bpy.data.objects.new(name, me)
    COL.objects.link(obj)
    for m in mats or []:
        me.materials.append(m)
    return obj


def material(name, color, metallic=0.0, rough=0.5, emission=None, alpha=1.0, transmission=0.0):
    m = bpy.data.materials.new(name)
    try:
        m.use_nodes = True
    except Exception:
        pass
    nt = m.node_tree
    p = next((n for n in nt.nodes if n.type == "BSDF_PRINCIPLED"), None)
    if p is None:
        p = nt.nodes.new("ShaderNodeBsdfPrincipled")
        out = next(n for n in nt.nodes if n.type == "OUTPUT_MATERIAL")
        nt.links.new(p.outputs[0], out.inputs[0])

    def put(key, val):
        if key in p.inputs:
            p.inputs[key].default_value = val

    put("Base Color", (*color, 1.0))
    put("Metallic", metallic)
    put("Roughness", rough)
    put("Alpha", alpha)
    put("Transmission Weight", transmission)
    if emission:
        put("Emission Color", (*emission[0], 1.0))
        put("Emission Strength", emission[1])
    m["base_alpha"] = alpha
    return m


M_STEEL = material("steel", (0.62, 0.63, 0.645), metallic=1.0, rough=0.33)
M_BLACK = material("black", (0.025, 0.025, 0.027), rough=0.6)
M_GLASS = material("glass", (0.02, 0.025, 0.03), rough=0.04)
M_LOUVER = material("louver", (0.05, 0.05, 0.055), metallic=0.6, rough=0.45)
M_TIRE = material("tire", (0.018, 0.018, 0.02), rough=0.85)
M_RIM = material("rim", (0.35, 0.35, 0.36), metallic=1.0, rough=0.4)
M_LAMP = material("lamp", (0.85, 0.85, 0.82), metallic=0.2, rough=0.1)
M_TAIL = material("tail", (0.45, 0.02, 0.02), rough=0.25)
M_FLOOR = material("floor", (0.55, 0.56, 0.58), rough=0.9)
M_MAN = material("man", (0.30, 0.32, 0.36), rough=0.7)


def loft(bm, rings, cap=True):
    """Мост между кольцами одинаковой длины; кольца замкнуты. Возвращает списки вершин."""
    vr = [[bm.verts.new(p) for p in ring] for ring in rings]
    n = len(rings[0])
    for a, b in zip(vr[:-1], vr[1:]):
        for i in range(n):
            j = (i + 1) % n
            bm.faces.new((a[i], a[j], b[j], b[i]))
    if cap:
        bm.faces.new(vr[0][::-1])
        bm.faces.new(vr[-1])
    return vr


def body_ring(s):
    y, zb, zr, zc, zsh, zt, hr, hs, hc, hsh = s
    right = [(hr, zb), (hs, zr), (hc, zc), (hsh, zsh)]
    pts = [(0.0, zb)] + right + [(0.0, zt)] + [(-x, z) for x, z in reversed(right)]
    return [(x, y, z) for x, z in pts]


def green_ring(g):
    y, zb, ze, zc, hb, ht = g
    pts = [(-hb, zb), (-ht, ze), (0.0, zc), (ht, ze), (hb, zb)]
    return [(x, y, z) for x, z in pts]


def box(bm, center, half):
    cx, cy, cz = center
    hx, hy, hz = half
    res = bmesh.ops.create_cube(bm, size=2.0)
    for v in res["verts"]:
        v.co = Vector((cx + v.co.x * hx, cy + v.co.y * hy, cz + v.co.z * hz))


def lathe_x(bm, center, profile, side=1.0, segs=40):
    """Тело вращения вокруг оси X. profile — замкнутый контур (радиус, смещение по оси)."""
    cx, cy, cz = center
    rings = []
    for k in range(segs):
        t = 2 * math.pi * k / segs
        c, s = math.cos(t), math.sin(t)
        rings.append([bm.verts.new((cx + a * side, cy + r * c, cz + r * s)) for r, a in profile])
    n = len(profile)
    for k in range(segs):
        a, b = rings[k], rings[(k + 1) % segs]
        for i in range(n):
            j = (i + 1) % n
            bm.faces.new((a[i], a[j], b[j], b[i]))


def lathe_z(bm, center, profile, segs=24):
    """Тело вращения вокруг оси Z. profile — открытая ломаная (радиус, z) от низа к верху."""
    cx, cy, cz = center
    rings = []
    for k in range(segs):
        t = 2 * math.pi * k / segs
        c, s = math.cos(t), math.sin(t)
        rings.append([bm.verts.new((cx + r * c, cy + r * s, cz + z)) for r, z in profile])
    n = len(profile)
    for k in range(segs):
        a, b = rings[k], rings[(k + 1) % segs]
        for i in range(n - 1):
            bm.faces.new((a[i], a[i + 1], b[i + 1], b[i]))


def smooth(obj, angle=28):
    me = obj.data
    try:
        me.shade_smooth()
        me.set_sharp_from_angle(angle=math.radians(angle))
    except Exception:
        for p in me.polygons:
            p.use_smooth = False


def integrity(obj):
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    nm = sum(1 for e in bm.edges if len(e.link_faces) > 2)
    bd = sum(1 for e in bm.edges if len(e.link_faces) < 2)
    deg = sum(1 for f in bm.faces if f.calc_area() < 1e-9)
    bm.free()
    print("INTEGRITY %-10s non_manifold=%d boundary=%d degenerate=%d faces=%d"
          % (obj.name, nm, bd, deg, len(obj.data.polygons)))


def set_creases(bm, vr, long_w, end_w=None, ring_w=None):
    """Жёсткость рёбер: вдоль машины по индексам точек кольца, по кромке первого/последнего кольца,
    по отдельным рёбрам колец {(i, j): w}."""
    cr = bm.edges.layers.float.get("crease_edge") or bm.edges.layers.float.new("crease_edge")
    for a, b in zip(vr[:-1], vr[1:]):
        for i, w in long_w.items():
            bm.edges.get((a[i], b[i]))[cr] = w
    n = len(vr[0])
    for k, ring in enumerate(vr):
        for i in range(n):
            j = (i + 1) % n
            w = 0.0
            if end_w and k in (0, len(vr) - 1):
                w = end_w
            if ring_w and (i, j) in ring_w:
                w = max(w, ring_w[(i, j)])
            if w:
                bm.edges.get((ring[i], ring[j]))[cr] = w


def subdivide(obj, levels=SUBD):
    mod = obj.modifiers.new("subd", "SUBSURF")
    mod.levels = levels
    mod.render_levels = levels
    try:
        mod.use_creases = True
    except Exception:
        pass
    dg = bpy.context.evaluated_depsgraph_get()
    me2 = bpy.data.meshes.new_from_object(obj.evaluated_get(dg))
    obj.modifiers.remove(mod)
    old = obj.data
    obj.data = me2
    bpy.data.meshes.remove(old)


def apply_boolean(target, cutter, op="DIFFERENCE"):
    mod = target.modifiers.new("bool", "BOOLEAN")
    mod.operation = op
    mod.object = cutter
    mod.solver = "EXACT"
    try:
        mod.material_mode = "TRANSFER"
    except Exception:
        pass
    dg = bpy.context.evaluated_depsgraph_get()
    me2 = bpy.data.meshes.new_from_object(target.evaluated_get(dg))
    target.modifiers.remove(mod)
    old = target.data
    target.data = me2
    bpy.data.meshes.remove(old)


# ------------------------------------------------------------------ кузов
bm = bmesh.new()
vr = loft(bm, [body_ring(s) for s in BODY])
bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
set_creases(bm, vr, BODY_CREASE, end_w=BODY_END_CREASE)
for f in bm.faces:                       # низ кузова ниже порога — чёрный
    if f.calc_center_median().z < 0.305:
        f.material_index = 1
body = link_mesh("body", bm, [M_STEEL, M_BLACK])
subdivide(body)

# колёсные арки: угловатые, вырез только в боковинах
bm = bmesh.new()
for ya in (Y_FA, Y_RA):
    arch = [(ya - 0.42, -0.05), (ya - 0.42, 0.37), (ya - 0.28, 0.685),
            (ya + 0.28, 0.685), (ya + 0.42, 0.37), (ya + 0.42, -0.05)]
    for x0, x1 in ((0.55, 1.10), (-1.10, -0.55)):
        a = [bm.verts.new((x0, y, z)) for y, z in arch]
        b = [bm.verts.new((x1, y, z)) for y, z in arch]
        n = len(arch)
        for i in range(n):
            j = (i + 1) % n
            bm.faces.new((a[i], a[j], b[j], b[i]))
        bm.faces.new(a[::-1])
        bm.faces.new(b)
bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
cutter = link_mesh("arch_cutter", bm, [M_BLACK])
apply_boolean(body, cutter)
bpy.data.objects.remove(cutter)

smooth(body, 35)
integrity(body)

# ------------------------------------------------------------------ «стеклянный» верх
bm = bmesh.new()
vr = loft(bm, [green_ring(g) for g in GREEN])
bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
set_creases(bm, vr, GREEN_CREASE, end_w=0.5, ring_w={(4, 0): 1.0})
y_ws0, y_ws1, y_side1, y_roof1, y_lv1 = (g[0] for g in GREEN)
for f in bm.faces:
    c, nrm = f.calc_center_median(), f.normal
    if y_ws0 <= c.y <= y_ws1 and nrm.z > 0.3 and abs(nrm.x) < 0.5:
        f.material_index = 1                      # лобовое стекло
    elif abs(nrm.x) > 0.55 and c.y < y_side1 and c.z > 0.84:
        f.material_index = 1                      # боковые стёкла
    elif c.y > y_roof1 and nrm.z > 0.3 and abs(nrm.x) < 0.5:
        f.material_index = 2                      # задние жалюзи
green = link_mesh("greenhouse", bm, [M_STEEL, M_GLASS, M_LOUVER])
subdivide(green)
smooth(green, 35)
integrity(green)

# ------------------------------------------------------------------ колёса
for ya in (Y_FA, Y_RA):
    for side in (1.0, -1.0):
        c = (side * TRACK / 2, ya, R)
        bm = bmesh.new()
        tire = [(0.20, -0.100), (0.27, -0.112), (0.305, -0.108), (0.318, -0.095), (R, -0.06),
                (R, 0.06), (0.318, 0.095), (0.305, 0.108), (0.27, 0.112), (0.20, 0.100)]
        lathe_x(bm, c, tire, side)
        bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
        t = link_mesh("tire_%s_%s" % ("f" if ya < 0 else "r", "r" if side > 0 else "l"), bm, [M_TIRE])
        smooth(t, 40)
        bm = bmesh.new()
        rim = [(0.02, 0.0), (0.02, 0.055), (0.15, 0.065), (0.19, 0.085), (0.203, 0.092),
               (0.203, -0.092), (0.19, -0.092), (0.18, -0.02), (0.15, 0.0)]
        lathe_x(bm, c, rim, side)
        bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
        rm = link_mesh("rim_%s_%s" % ("f" if ya < 0 else "r", "r" if side > 0 else "l"), bm, [M_RIM])
        smooth(rm, 40)

# ------------------------------------------------------------------ фары, фонари, бамперы
bm = bmesh.new()
box(bm, (0.0, -2.125, 0.515), (0.79, 0.025, 0.07))            # чёрная полоса решётки
bumpers = link_mesh("grille_band", bm, [M_BLACK])
bm = bmesh.new()
for x in (-0.68, -0.48, 0.48, 0.68):
    box(bm, (x, -2.13, 0.515), (0.085, 0.03, 0.05))
link_mesh("headlamps", bm, [M_LAMP])
bm = bmesh.new()
box(bm, (0.0, 2.125, 0.72), (0.80, 0.03, 0.085))
link_mesh("taillights", bm, [M_TAIL])
bm = bmesh.new()
box(bm, (0.0, -2.11, 0.33), (0.80, 0.06, 0.07))
box(bm, (0.0, 2.11, 0.35), (0.80, 0.06, 0.07))
link_mesh("bumpers", bm, [M_BLACK])

# ------------------------------------------------------------------ метки мест под обвесы
markers = []
for key, (pos, colr) in MOUNTS.items():
    mm = material("mk_" + key, colr, rough=0.3, emission=(colr, 2.5))
    bm = bmesh.new()
    if key == "sills":
        box(bm, (0.95, pos[1], pos[2]), (0.03, 0.55, 0.03))
        box(bm, (-0.95, pos[1], pos[2]), (0.03, 0.55, 0.03))
    else:
        bmesh.ops.create_uvsphere(bm, u_segments=16, v_segments=10, radius=0.07,
                                  matrix=__import__("mathutils").Matrix.Translation(pos))
    o = link_mesh("mount_" + key, bm, [mm])
    smooth(o, 80)
    o.hide_render = True
    markers.append(o)

# ------------------------------------------------------------------ человек 1,8 м для масштаба
bm = bmesh.new()
lathe_z(bm, (1.55, -3.0, 0.0), [(0.0001, 0.0), (0.14, 0.0), (0.16, 0.85), (0.20, 1.30), (0.17, 1.45),
                                 (0.07, 1.52), (0.10, 1.60), (0.10, 1.72), (0.06, 1.79), (0.0001, 1.80)])
man = link_mesh("man_1_8m", bm, [M_MAN])
smooth(man, 60)

# ------------------------------------------------------------------ пол, свет
bm = bmesh.new()
bmesh.ops.create_grid(bm, x_segments=1, y_segments=1, size=400.0)
link_mesh("floor", bm, [M_FLOOR])

world = bpy.data.worlds.new("w")
try:
    world.use_nodes = True
except Exception:
    pass
bg = world.node_tree.nodes.get("Background") or next(n for n in world.node_tree.nodes if n.type == "BACKGROUND")
bg.inputs["Color"].default_value = (0.62, 0.66, 0.72, 1)
bg.inputs["Strength"].default_value = 0.9
scene.world = world

sun_d = bpy.data.lights.new("sun", "SUN")
sun_d.energy = 3.2
sun_d.angle = math.radians(8)
sun = bpy.data.objects.new("sun", sun_d)
sun.rotation_euler = (math.radians(50), 0.0, math.radians(-35))
COL.objects.link(sun)

# ------------------------------------------------------------------ рендер
scene.render.engine = "CYCLES"
gpu = False
try:
    cp = bpy.context.preferences.addons["cycles"].preferences
    for dt in ("OPTIX", "CUDA"):
        try:
            cp.compute_device_type = dt
            cp.get_devices()
            if any(d.type == dt for d in cp.devices):
                for d in cp.devices:
                    d.use = d.type == dt
                gpu = True
                break
        except Exception:
            pass
except Exception:
    pass
scene.cycles.device = "GPU" if gpu else "CPU"
scene.cycles.samples = 96
scene.cycles.use_denoising = True
scene.render.resolution_x = 1600
scene.render.resolution_y = 900
scene.render.image_settings.file_format = "PNG"
scene.render.image_settings.color_mode = "RGB"


def camera(name, loc, target, ortho=None, lens=45):
    cd = bpy.data.cameras.new(name)
    if ortho:
        cd.type = "ORTHO"
        cd.ortho_scale = ortho
    else:
        cd.lens = lens
    cd.clip_start = 0.05
    cd.clip_end = 200
    cam = bpy.data.objects.new(name, cd)
    COL.objects.link(cam)
    cam.location = loc
    d = Vector(target) - Vector(loc)
    cam.rotation_euler = d.to_track_quat("-Z", "Y").to_euler()
    return cam


VIEWS = [
    ("1_bok", camera("side", (12.0, -0.6, 0.62), (0.0, -0.6, 0.62), ortho=6.6)),
    ("2_3-4_pered", camera("p34f", (-5.2, -6.8, 2.3), (0.0, -0.3, 0.55), lens=50)),
    ("3_pered", camera("front", (0.3, -12.0, 0.75), (0.3, 0.0, 0.75), ortho=3.9)),
    ("4_zad", camera("rear", (0.3, 12.0, 0.75), (0.3, 0.0, 0.75), ortho=3.9)),
    ("5_sverhu", camera("top", (0.4, -0.6, 14.0), (0.4, -0.6, 0.0), ortho=6.6)),
    ("6_obvesy", camera("mk", (-4.6, -5.4, 4.6), (0.0, 0.0, 0.5), lens=42)),
]
# камера «сверху» смотрит вниз: поворачиваем так, чтобы нос был слева
VIEWS[4][1].rotation_euler = (0.0, 0.0, math.radians(90))

files = []
for tag, cam in VIEWS:
    scene.camera = cam
    is_mk = tag.endswith("obvesy")
    for o in markers:
        o.hide_render = not is_mk
    for m in (M_STEEL, M_GLASS, M_LOUVER, M_BLACK):
        p = next(n for n in m.node_tree.nodes if n.type == "BSDF_PRINCIPLED")
        p.inputs["Alpha"].default_value = 0.28 if is_mk else m["base_alpha"]
    man.hide_render = is_mk
    path = os.path.join(OUT, tag + ".png")
    scene.render.filepath = path
    bpy.ops.render.render(write_still=True)
    files.append(path)
    print("RENDER", path)

# ------------------------------------------------------------------ лист 3×2
cw, ch = 800, 450
sheet = np.ones((ch * 2, cw * 3, 4), np.float32)
for i, f in enumerate(files):
    im = bpy.data.images.load(f)
    w, h = im.size
    px = np.array(im.pixels[:], np.float32).reshape(h, w, 4)
    sm = px[(np.arange(ch) * h / ch).astype(int)][:, (np.arange(cw) * w / cw).astype(int)]
    r, c = i // 3, i % 3
    y0 = (1 - r) * ch
    sheet[y0:y0 + ch, c * cw:(c + 1) * cw] = sm
img = bpy.data.images.new("sheet", cw * 3, ch * 2, alpha=False)
img.pixels.foreach_set(sheet.ravel())
img.filepath_raw = os.path.join(OUT, "list_etap1.png")
img.file_format = "PNG"
img.save()

# ------------------------------------------------------------------ контроль размеров
def bounds(objs):
    pts = [o.matrix_world @ v.co for o in objs for v in o.data.vertices]
    a = np.array([tuple(p) for p in pts])
    return a.min(0), a.max(0)


car = [o for o in COL.objects if o.type == "MESH" and not o.name.startswith(("mount_", "man_", "floor"))]
lo, hi = bounds(car)
print("SIZE length=%.3f width=%.3f height=%.3f wheelbase=%.3f track=%.3f"
      % (hi[1] - lo[1], hi[0] - lo[0], hi[2] - lo[2], Y_RA - Y_FA, TRACK))
print("DONE", OUT)
