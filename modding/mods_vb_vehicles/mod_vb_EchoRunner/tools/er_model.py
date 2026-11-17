# -*- coding: utf-8 -*-
r"""Echo Runner: модель машины (этап 2 — детали «самосбора» и разбивка на части).

Модуль для Blender 5.2 (импортируется скриптами рендера и, позже, сборки .p3d).
Оси: X вправо, Y назад (нос в -Y), Z вверх, z=0 — земля. Перевод в оси DayZ — при сборке .p3d.

Части (каждая — отдельный объект, как будущие отдельные .p3d / выделения):
  body        кузов с проёмами дверей и крышек, изнанкой, салоном, мотором, трубками, бамперами
  door_l/r    двери «крыло чайки» (петля вдоль крыши), с зеркалами
  hood        передняя крышка (багажник), петля у лобового стекла
  engine_lid  задняя крышка мотора с жалюзи, решёткой и воздухозаборником, петля у конца крыши
  steering    руль (вращается)
  wheel_*     колёса (в игре — один предмет на 4 места)
  pipes       трубки вдоль порогов (отдельная часть под свою текстуру/свечение)
  bumpers     бамперы из трубы (прячутся, когда стоит обвес «Перед»/«Корма»)

План соединений (Connection Map):
  верх (стёкла/крыша) объединён с кузовом булевой операцией — один замкнутый корпус до разрезки;
  двери/крышки вырезаются плоскостями из корпуса — края совпадают с проёмом;
  толщина: корпус 2 см, двери 4 см, крышки 3 см — внутрь, с торцами по краю проёма;
  трубки вдоль порогов: x=±0.93, держатели заходят в порог на 1-2 см;
  бамперы: стойки заходят в кузов на 3-5 см; фары/фонари утоплены в кузов на 1-3 см;
  колёса: шина R=0.36 на ступице HUB_Z=0.30 (кузов на 6 см выше земли, чем был), арки с зазором ~6 см.
Детали (Detail List, метод component-zones): бамперы из трубы со стойками; фары в корпусах (одна круглая —
«с другой машины»); решётка фар с планками; фонари разные (прямоугольный + круглые); решётка мотора с планками;
воздухозаборник из трубы с сеткой; выхлоп; трубки вдоль порогов (сталь + медь) с хомутами, шлангами
и держателями; горловина бака с крышкой и шлангом; зеркала разные (прямоугольное и круглое на пруте);
петли дверей на центральной полосе крыши; сетка на форточках; заплатки — зоны материалов (грунт, ржавчина,
красная дверь); колесо: зимний протектор блоками, штампованный диск с отверстиями, гайки, колпак;
салон: пол, тоннель, торпедо, щиток приборов, руль, кресла, рычаг КПП, перегородки; мотор под решёткой.
"""
import math

import bmesh
import bpy
from mathutils import Matrix, Vector

# ------------------------------------------------------------------ размеры
L = 4.267
WB = 2.408
TRACK = 1.59
R = 0.36          # радиус шины (по концепту пользователя 2026-11-17: крупные колёса, машина выше)
HUB_Z = 0.30      # высота ступицы в осях кузова (было 0.32 при R=0.32): кузов поднят на 6 см
FRONT_OVH = 0.86
Y_FA = -L / 2 + FRONT_OVH
Y_RA = Y_FA + WB

BODY = [
    # y       zb    zr    zc    zsh    zt     hr    hs    hc     hsh
    (-2.1335, 0.30, 0.33, 0.50, 0.575, 0.59,  0.62, 0.66, 0.70,  0.68),
    (-2.10,   0.25, 0.30, 0.53, 0.615, 0.63,  0.74, 0.79, 0.82,  0.80),
    (-1.92,   0.20, 0.29, 0.57, 0.73,  0.745, 0.83, 0.88, 0.905, 0.87),     # крыло +5 см (баг 44)
    (Y_FA,    0.17, 0.30, 0.60, 0.79,  0.81,  0.86, 0.90, 0.925, 0.89),
    (-0.62,   0.10, 0.30, 0.62, 0.78,  0.80,  0.86, 0.905, 0.925, 0.89),     # низ салона 0.10 (баг 43)
    (0.50,    0.10, 0.30, 0.63, 0.85,  0.865, 0.86, 0.905, 0.925, 0.89),
    (Y_RA,    0.17, 0.30, 0.64, 0.88,  0.90,  0.86, 0.905, 0.925, 0.895),
    (1.90,    0.20, 0.30, 0.64, 0.89,  0.91,  0.84, 0.885, 0.91,  0.875),
    (2.10,    0.25, 0.31, 0.64, 0.885, 0.905, 0.78, 0.83,  0.86,  0.83),
    (2.1335,  0.29, 0.33, 0.63, 0.87,  0.89,  0.70, 0.75,  0.79,  0.76),
]
ROOF_UP = 0.16     # крыша выше на 16 см: поза водителя DayZ прямая и наклон прокси игра не применяет (баг 43)
GREEN = [
    (-0.62, 0.76, 0.785, 0.80,  0.86,  0.84),
    (-0.08, 0.79, 1.12 + ROOF_UP,  1.156 + ROOF_UP, 0.875, 0.64),
    (0.45,  0.82, 1.115 + ROOF_UP, 1.151 + ROOF_UP, 0.88,  0.64),
    (0.80,  0.84, 1.115 + ROOF_UP, 1.146 + ROOF_UP, 0.88,  0.64),
    (1.68,  0.86, 0.90,  0.915, 0.875, 0.80),
]
BODY_CREASE = {1: 0.6, 9: 0.6, 2: 0.4, 8: 0.4, 3: 1.0, 7: 1.0, 4: 0.75, 6: 0.75}
GREEN_CREASE = {0: 1.0, 4: 1.0, 1: 0.6, 3: 0.6}

# Разрезы корпуса на двери и крышки
DOOR = dict(x_hinge=0.16, y0=-0.55, y1=0.45, z0=0.30)
HOOD = dict(x=0.72, y0=-2.05, y1=-0.66)
ELID = dict(x=0.80, y0=0.80, y1=2.06)

# Посадка. Поза сидящего в DayZ прямая: от бедра до макушки ~0.82 м, у ванили крыша 1.40-1.49 м.
# У нас крыша 1.14 (DMC-12), поэтому сажаем низко, как в настоящей машине: бедро на 0.28; ноги уходят под пол,
# снаружи их закрывает кузов. Начало прокси человека = бедро (у Сарки 0.573 при сиденье ~0.48 и полу ~0.25).
# Человека откидываем назад на CREW_PITCH (как в спорткаре): ноги поднимаются над днищем, голова ниже.
# Руль — под руки откинутой позы: в прямой позе кисти ~0.45 впереди и 0.40 выше бедра (у Сарки ~0.5 / 0.41).
# 2026-11-17 (баг 43): в игре наклон прокси человека не применяется (скрины: корпус прямой, ступни под полом).
# Поза S120 прямая: ступни ~0.32 ниже бедра, макушка ~0.82 выше. Поэтому бедро 0.44, пол 0.125, крыша +ROOF_UP.
SEAT = {"driver": (0.40, 0.18, 0.44), "codriver": (-0.40, 0.18, 0.44)}
SEAT_DZ = SEAT["driver"][2] - 0.30          # насколько подняты кресло, руль и панель против прежней посадки
CREW_PITCH = 0.0


def crew_offset(fwd, up, pitch=CREW_PITCH):
    """Смещение точки позы (вперёд, вверх от бедра) после наклона назад на pitch: (dy, dz) в осях Blender."""
    a = math.radians(pitch)
    f = fwd * math.cos(a) - up * math.sin(a)
    u = up * math.cos(a) + fwd * math.sin(a)
    return -f, u


def crew_rotation(pitch=CREW_PITCH):
    """Строки «локальное -> мир» для прокси человека в сырых осях Blender (единичная = прямо)."""
    a = math.radians(pitch)
    return ((1.0, 0.0, 0.0), (0.0, math.cos(a), -math.sin(a)), (0.0, math.sin(a), math.cos(a)))


_dy, _dz = crew_offset(0.45, 0.40)
STEER_C = (SEAT["driver"][0], SEAT["driver"][1] + _dy, SEAT["driver"][2] + _dz)

# Места под обвесы (центры прокси)
MOUNTS = {
    "front": (0.0, -2.22, 0.40), "roof": (0.0, 0.50, 1.17 + ROOF_UP), "rear": (0.0, 2.22, 0.42),
    "can_l": (-0.55, 1.88, 0.95), "can_r": (0.55, 1.88, 0.95), "sills": (0.0, -0.10, 0.22),
    "engine": (0.0, 1.72, 0.78), "under": (0.0, 0.0, 0.12),
}

# ------------------------------------------------------------------ атлас кузова (этап 3)
# Корпус, двери и крышки снаружи — одна текстура. Развёртка = проекция мировых координат на 6 островков
# (по нормали грани), поэтому шов «дверь — кузов» совпадает сам. Размеры в метрах, атлас ATLAS_A × ATLAS_A.
ATLAS_A = 5.0
ATLAS_ISL = {           # u0, v0 (от верхнего левого угла), ширина, высота
    "T": (0.05, 0.05, 4.4, 1.9),     # верх:   a = y+2.2,  b = x+0.95
    "L": (0.05, 2.00, 4.4, 1.03),    # левый:  a = y+2.2,  b = 1.15−z
    "R": (0.05, 3.10, 4.4, 1.03),    # правый: a = y+2.2,  b = 1.15−z
    "F": (0.05, 4.15, 1.9, 0.80),    # перед:  a = x+0.95, b = 0.95−z
    "B": (2.05, 4.15, 1.9, 0.80),    # зад:    a = x+0.95, b = 0.95−z
    "D": (4.05, 4.15, 0.84, 0.36),   # низ/арки (масштаб 0.19)
}
ATLAS_MATS = {"steel", "black", "louver"}     # материалы корпуса, которые уходят в атлас
TILE_M = {"screen": 0.20, "strap": 0.12, "crate": 0.40, "tire": 0.25, "seat": 0.45, "floor": 0.5, "net": 0.35, "lamp": 0.15, "tail": 0.15, "amber": 0.15}


def atlas_local(g, x, y, z):
    if g == "T":
        return y + 2.2, x + 0.95
    if g in ("L", "R"):
        if hasattr(z, "__len__"):
            import numpy as np
            return y + 2.2, np.where(z <= 1.15, 1.15 - z, -(z - 1.15) * 0.26)
        return y + 2.2, (1.15 - z) if z <= 1.15 else -(z - 1.15) * 0.26
    if g in ("F", "B"):
        return x + 0.95, 0.95 - z
    return (y + 2.2) * 0.19, (x + 0.95) * 0.19


def atlas_world(g, a, b):
    """Обратное к atlas_local (для рисования): мировые x, y, z текселя островка."""
    import numpy as np
    one = np.ones_like(a)
    if g == "T":
        return b - 0.95, a - 2.2, one * 1.0
    if g == "L":
        return -0.92 * one, a - 2.2, 1.15 - b
    if g == "R":
        return 0.92 * one, a - 2.2, 1.15 - b
    if g == "F":
        return a - 0.95, -2.13 * one, 0.95 - b
    if g == "B":
        return a - 0.95, 2.13 * one, 0.95 - b
    return b / 0.19 - 0.95, a / 0.19 - 2.2, 0.15 * one


def atlas_group(c, n, mname):
    if mname == "black" and not (abs(n.x) > 0.6 and abs(c.x) > 0.85):
        return "D"                                   # днище и арки
    if n.z > 0.65:
        return "T"
    if n.z < -0.45:
        return "D"
    if abs(n.x) >= abs(n.y):
        return "L" if c.x < 0 else "R"
    return "F" if n.y < 0 else "B"


def atlas_uv(part):
    """Развёртка атласа для граней корпуса; эти грани получают материал body. Изнанка — плитка inner."""
    bm = part.bm
    bm.normal_update()
    uv = bm.loops.layers.uv.verify()
    atl = bm.faces.layers.int.get("atl") or bm.faces.layers.int.new("atl")
    mi_body = part.mi("body")
    for f in bm.faces:
        mname = part.mats[f.material_index]
        if mname not in ATLAS_MATS:
            continue
        g = atlas_group(f.calc_center_median(), f.normal, mname)
        u0, v0, _, _ = ATLAS_ISL[g]
        for lp in f.loops:
            a, b = atlas_local(g, *lp.vert.co)
            lp[uv].uv = ((u0 + a) / ATLAS_A, 1.0 - (v0 + b) / ATLAS_A)
        f.material_index = mi_body
        f[atl] = 1


def box_uv(part):
    """Плиточная развёртка (кубическая проекция) для всех граней, не попавших в атлас."""
    bm = part.bm
    bm.normal_update()
    uv = bm.loops.layers.uv.verify()
    atl = bm.faces.layers.int.get("atl") or bm.faces.layers.int.new("atl")
    for f in bm.faces:
        if f[atl]:
            continue
        sc = TILE_M.get(part.mats[f.material_index], 0.6)
        n = f.normal
        ax = max(range(3), key=lambda i: abs(n[i]))
        for lp in f.loops:
            co = lp.vert.co
            if ax == 0:
                a, b = co.y, co.z
            elif ax == 1:
                a, b = co.x, co.z
            else:
                a, b = co.x, co.y
            lp[uv].uv = (a / sc, b / sc)


# ------------------------------------------------------------------ материалы
PALETTE = {
    "body":     dict(color=(0.62, 0.63, 0.645), metallic=1.0, rough=0.33),
    "steel":    dict(color=(0.62, 0.63, 0.645), metallic=1.0, rough=0.33),
    "steel_in": dict(color=(0.16, 0.165, 0.17), metallic=0.4, rough=0.65),
    "primer":   dict(color=(0.36, 0.37, 0.38), rough=0.85),
    "rust":     dict(color=(0.11, 0.055, 0.032), metallic=0.1, rough=0.95),
    "redpaint": dict(color=(0.40, 0.09, 0.07), metallic=0.1, rough=0.6),
    "black":    dict(color=(0.025, 0.025, 0.027), rough=0.6),
    "glass_ws": dict(color=(0.02, 0.025, 0.03), rough=0.04),
    "glass":    dict(color=(0.02, 0.025, 0.03), rough=0.04),
    "net":      dict(color=(0.09, 0.09, 0.09), metallic=0.8, rough=0.55),
    "louver":   dict(color=(0.05, 0.05, 0.055), metallic=0.6, rough=0.45),
    "tire":     dict(color=(0.018, 0.018, 0.02), rough=0.85),
    "rim":      dict(color=(0.11, 0.115, 0.12), metallic=0.7, rough=0.5),
    "chrome":   dict(color=(0.8, 0.8, 0.82), metallic=1.0, rough=0.12),
    "lamp":     dict(color=(0.85, 0.85, 0.82), metallic=0.3, rough=0.08),
    "tail":     dict(color=(0.5, 0.02, 0.02), rough=0.2),
    "amber":    dict(color=(0.8, 0.35, 0.02), rough=0.2),
    "pipe":     dict(color=(0.20, 0.165, 0.14), metallic=0.6, rough=0.72),
    "copper":   dict(color=(0.72, 0.38, 0.22), metallic=1.0, rough=0.4),
    "hose":     dict(color=(0.03, 0.03, 0.03), rough=0.75),
    "clamp":    dict(color=(0.6, 0.6, 0.62), metallic=1.0, rough=0.35),
    "seat":     dict(color=(0.12, 0.08, 0.06), rough=0.7),
    "dash":     dict(color=(0.04, 0.04, 0.045), rough=0.7),
    "floor":    dict(color=(0.05, 0.05, 0.05), rough=0.9),
    "engine":   dict(color=(0.15, 0.15, 0.16), metallic=0.7, rough=0.55),
    "neon":     dict(color=(0.9, 0.05, 0.03), rough=0.1),
    "strap":    dict(color=(0.45, 0.06, 0.04), rough=0.8),
    "screen":   dict(color=(0.10, 0.45, 0.20), rough=0.2),
    "crate":    dict(color=(0.20, 0.22, 0.13), rough=0.7),
}
_MATS = {}


def mat(name):
    if name in _MATS:
        return _MATS[name]
    p = PALETTE[name]
    m = bpy.data.materials.new(name)
    try:
        m.use_nodes = True
    except Exception:
        pass
    nt = m.node_tree
    pr = next((n for n in nt.nodes if n.type == "BSDF_PRINCIPLED"), None)
    if pr is None:
        pr = nt.nodes.new("ShaderNodeBsdfPrincipled")
        out = next(n for n in nt.nodes if n.type == "OUTPUT_MATERIAL")
        nt.links.new(pr.outputs[0], out.inputs[0])
    for key, val in (("Base Color", (*p["color"], 1.0)), ("Metallic", p.get("metallic", 0.0)),
                     ("Roughness", p.get("rough", 0.5))):
        if key in pr.inputs:
            pr.inputs[key].default_value = val
    _MATS[name] = m
    return m


# ------------------------------------------------------------------ часть = bmesh + список материалов
class Part:
    def __init__(self, name):
        self.name = name
        self.bm = bmesh.new()
        self.mats = []

    def mi(self, m):
        if m not in self.mats:
            self.mats.append(m)
        return self.mats.index(m)

    def finish(self, sharp_deg=35, recalc=True):
        if recalc:
            bmesh.ops.recalc_face_normals(self.bm, faces=self.bm.faces)
        box_uv(self)
        me = bpy.data.meshes.new(self.name)
        self.bm.to_mesh(me)
        self.bm.free()
        for m in self.mats:
            me.materials.append(mat(m))
        obj = bpy.data.objects.new(self.name, me)
        bpy.context.scene.collection.objects.link(obj)
        try:
            me.shade_smooth()
            me.set_sharp_from_angle(angle=math.radians(sharp_deg))
        except Exception:
            pass
        return obj


def _faces_of(verts):
    fs = set()
    for v in verts:
        fs.update(v.link_faces)
    return list(fs)


def box(part, center, half, m, frame=None):
    """Брусок. frame=(ax, ay, az) — правая тройка единичных векторов (по умолчанию оси мира)."""
    c = Vector(center)
    ax, ay, az = frame or (Vector((1, 0, 0)), Vector((0, 1, 0)), Vector((0, 0, 1)))
    res = bmesh.ops.create_cube(part.bm, size=2.0)
    for v in res["verts"]:
        x, y, z = v.co
        v.co = c + ax * (x * half[0]) + ay * (y * half[1]) + az * (z * half[2])
    for f in _faces_of(res["verts"]):
        f.material_index = part.mi(m)


def frame_from(axis):
    """Правая тройка (u, v, w) с w = axis."""
    w = Vector(axis).normalized()
    t = Vector((0, 0, 1)) if abs(w.z) < 0.9 else Vector((1, 0, 0))
    u = t.cross(w).normalized()
    v = w.cross(u)
    return u, v, w


def lathe(part, center, axis, profile, m, segs=32, closed=True):
    """Тело вращения вокруг оси axis через center. profile: [(радиус, смещение по оси)]."""
    c = Vector(center)
    u, v, w = frame_from(axis)
    rings = []
    for k in range(segs):
        t = 2 * math.pi * k / segs
        d = u * math.cos(t) + v * math.sin(t)
        rings.append([part.bm.verts.new(c + d * r + w * a) for r, a in profile])
    n = len(profile)
    mi = part.mi(m)
    for k in range(segs):
        a, b = rings[k], rings[(k + 1) % segs]
        for i in range(n if closed else n - 1):
            j = (i + 1) % n
            f = part.bm.faces.new((a[i], a[j], b[j], b[i]))
            f.material_index = mi


def cyl(part, p0, p1, r, m, segs=12):
    """Цилиндр от точки p0 до p1 (с торцами)."""
    p0, p1 = Vector(p0), Vector(p1)
    h = (p1 - p0).length
    lathe(part, p0, p1 - p0, [(0.0001, 0.0), (r, 0.0), (r, h), (0.0001, h)], m, segs)


def fillet(pts, rad=0.06, n=4):
    """Скругляет углы ломаной."""
    pts = [Vector(p) for p in pts]
    out = [pts[0]]
    for i in range(1, len(pts) - 1):
        a, p, c = pts[i - 1], pts[i], pts[i + 1]
        d = min(rad, (a - p).length / 2, (c - p).length / 2)
        p1 = p + (a - p).normalized() * d
        p2 = p + (c - p).normalized() * d
        for k in range(n + 1):
            t = k / n
            out.append(p1 * (1 - t) ** 2 + p * 2 * t * (1 - t) + p2 * t * t)
    out.append(pts[-1])
    return out


def tube(part, pts, r, m, segs=10, caps=True, round_corners=0.06):
    """Труба по ломаной (углы скругляются), кольца переносятся параллельно."""
    pts = fillet(pts, round_corners) if round_corners and len(pts) > 2 else [Vector(p) for p in pts]
    clean = [pts[0]]
    for p in pts[1:]:
        if (p - clean[-1]).length > 1e-4:
            clean.append(p)
    pts = clean
    tans = []
    for i in range(len(pts)):
        a = pts[max(i - 1, 0)]
        b = pts[min(i + 1, len(pts) - 1)]
        tans.append((b - a).normalized())
    u, v, _ = frame_from(tans[0])
    rings = []
    mi = part.mi(m)
    for i, p in enumerate(pts):
        t = tans[i]
        u = (u - t * u.dot(t)).normalized()
        v = t.cross(u)
        rings.append([part.bm.verts.new(p + (u * math.cos(2 * math.pi * k / segs) +
                                             v * math.sin(2 * math.pi * k / segs)) * r) for k in range(segs)])
    for a, b in zip(rings[:-1], rings[1:]):
        for k in range(segs):
            j = (k + 1) % segs
            part.bm.faces.new((a[k], a[j], b[j], b[k])).material_index = mi
    if caps:
        part.bm.faces.new(rings[0][::-1]).material_index = mi
        part.bm.faces.new(rings[-1]).material_index = mi
    return pts


def clamps(part, pts, r, every=0.35, m="clamp"):
    """Хомуты вдоль трубы через каждые every метров."""
    pts = [Vector(p) for p in pts]
    acc, nxt = 0.0, every / 2
    for a, b in zip(pts[:-1], pts[1:]):
        seg = (b - a).length
        while acc + seg >= nxt:
            t = (nxt - acc) / seg
            p = a + (b - a) * t
            d = (b - a).normalized()
            cyl(part, p - d * 0.012, p + d * 0.012, r * 1.45, m, 10)
            nxt += every
        acc += seg


# ------------------------------------------------------------------ корпус
def _loft(bm, rings):
    vr = [[bm.verts.new(p) for p in ring] for ring in rings]
    n = len(rings[0])
    for a, b in zip(vr[:-1], vr[1:]):
        for i in range(n):
            j = (i + 1) % n
            bm.faces.new((a[i], a[j], b[j], b[i]))
    bm.faces.new(vr[0][::-1])
    bm.faces.new(vr[-1])
    return vr


def _creases(bm, vr, long_w, end_w=0.0, ring_w=None):
    cr = bm.edges.layers.float.get("crease_edge") or bm.edges.layers.float.new("crease_edge")
    for a, b in zip(vr[:-1], vr[1:]):
        for i, w in long_w.items():
            bm.edges.get((a[i], b[i]))[cr] = w
    n = len(vr[0])
    for k, ring in enumerate(vr):
        for i in range(n):
            j = (i + 1) % n
            w = end_w if k in (0, len(vr) - 1) else 0.0
            if ring_w and (i, j) in ring_w:
                w = max(w, ring_w[(i, j)])
            if w:
                bm.edges.get((ring[i], ring[j]))[cr] = w


def _eval_replace(obj, mod):
    dg = bpy.context.evaluated_depsgraph_get()
    me2 = bpy.data.meshes.new_from_object(obj.evaluated_get(dg))
    obj.modifiers.remove(mod)
    old = obj.data
    obj.data = me2
    bpy.data.meshes.remove(old)


def _subd(obj, levels=2):
    mod = obj.modifiers.new("subd", "SUBSURF")
    mod.levels = mod.render_levels = levels
    try:
        mod.use_creases = True
    except Exception:
        pass
    _eval_replace(obj, mod)


def _bool(obj, other, op):
    mod = obj.modifiers.new("bool", "BOOLEAN")
    mod.operation = op
    mod.object = other
    mod.solver = "EXACT"
    try:
        mod.material_mode = "TRANSFER"
    except Exception:
        pass
    _eval_replace(obj, mod)


def _body_ring(s):
    y, zb, zr, zc, zsh, zt, hr, hs, hc, hsh = s
    right = [(hr, zb), (hs, zr), (hc, zc), (hsh, zsh)]
    pts = [(0.0, zb)] + right + [(0.0, zt)] + [(-x, z) for x, z in reversed(right)]
    return [(x, y, z) for x, z in pts]


def _green_ring(g):
    y, zb, ze, zc, hb, ht = g
    return [(x, y, z) for x, z in ((-hb, zb), (-ht, ze), (0.0, zc), (ht, ze), (hb, zb))]


def arch_outline(ya):
    """Верхний контур арки (y, z): трапеция над шиной R на ступице HUB_Z с зазором ~6 см."""
    top = 0.695 if ya < 0 else 0.72          # спереди ниже: верх расширителя под кромкой крыла (баг 44)
    return [(ya - 0.42, 0.37), (ya - 0.30, top), (ya + 0.30, top), (ya + 0.42, 0.37)]


def build_shell():
    """Замкнутый корпус: кузов ∪ верх, со сглаживанием и арками."""
    p = Part("shell_body")
    vr = _loft(p.bm, [_body_ring(s) for s in BODY])
    bmesh.ops.recalc_face_normals(p.bm, faces=p.bm.faces)
    _creases(p.bm, vr, BODY_CREASE, end_w=0.7)
    for f in p.bm.faces:
        f.material_index = p.mi("black" if f.calc_center_median().z < 0.305 else "steel")
    body = p.finish(recalc=False)
    _subd(body)

    g = Part("shell_green")
    vr = _loft(g.bm, [_green_ring(s) for s in GREEN])
    bmesh.ops.recalc_face_normals(g.bm, faces=g.bm.faces)
    _creases(g.bm, vr, GREEN_CREASE, end_w=0.5, ring_w={(4, 0): 1.0})
    y0, y1, ys, yr, _ = (s[0] for s in GREEN)
    for f in g.bm.faces:
        c, n = f.calc_center_median(), f.normal
        if y0 <= c.y <= y1 and n.z > 0.3 and abs(n.x) < 0.5:
            f.material_index = g.mi("glass_ws")
        elif abs(n.x) > 0.55 and c.y < ys and c.z > 0.84:
            f.material_index = g.mi("glass")
        elif c.y > yr and n.z > 0.3 and abs(n.x) < 0.5:
            f.material_index = g.mi("louver")
        else:
            f.material_index = g.mi("steel")
    green = g.finish(recalc=False)
    _subd(green)

    _bool(body, green, "UNION")
    bpy.data.objects.remove(green)

    c = Part("arch_cutter")
    for ya in (Y_FA, Y_RA):
        arch = [(ya - 0.42, -0.05)] + arch_outline(ya) + [(ya + 0.42, -0.05)]
        for x0, x1 in ((0.55, 1.10), (-1.10, -0.55)):
            a = [c.bm.verts.new((x0, y, z)) for y, z in arch]
            b = [c.bm.verts.new((x1, y, z)) for y, z in arch]
            for i in range(len(arch)):
                j = (i + 1) % len(arch)
                c.bm.faces.new((a[i], a[j], b[j], b[i])).material_index = c.mi("black")
            c.bm.faces.new(a[::-1]).material_index = c.mi("black")
            c.bm.faces.new(b).material_index = c.mi("black")
    cutter = c.finish()
    _bool(body, cutter, "DIFFERENCE")
    bpy.data.objects.remove(cutter)
    return body


def _classify(c, n, mname):
    d = DOOR
    if abs(c.x) > d["x_hinge"] and d["y0"] < c.y < d["y1"] and c.z > d["z0"] and mname != "glass_ws" \
            and n.z > -0.6:
        return "door_r" if c.x > 0 else "door_l"
    if abs(c.x) < HOOD["x"] and HOOD["y0"] < c.y < HOOD["y1"] and n.z > 0.35:
        return "hood"
    if abs(c.x) < ELID["x"] and ELID["y0"] < c.y < ELID["y1"] and n.z > 0.25:
        return "engine_lid"
    return "body"


def _thicken(part, t, inner="steel_in", keep=("glass", "glass_ws", "net")):
    """Толщина внутрь: внутренняя обшивка + торцы по открытым краям."""
    bm = part.bm
    bm.normal_update()
    faces = list(bm.faces)
    verts = [v for v in bm.verts if v.link_faces]
    boundary = [e for e in bm.edges if e.is_boundary]
    nv = {v: bm.verts.new(v.co - v.normal * t) for v in verts}
    keep_idx = {part.mats.index(k) for k in keep if k in part.mats}
    mi_in = part.mi(inner)
    for f in faces:
        nf = bm.faces.new([nv[l.vert] for l in reversed(f.loops)])
        nf.material_index = f.material_index if f.material_index in keep_idx else mi_in
    for e in boundary:
        f = e.link_faces[0]
        lp = next(l for l in f.loops if l.edge == e)
        a, b = lp.vert, lp.link_loop_next.vert
        bm.faces.new((b, a, nv[a], nv[b])).material_index = mi_in


def split_shell(shell):
    """Режет корпус плоскостями и раскладывает грани по частям. Возвращает {имя: Part}."""
    src = bmesh.new()
    src.from_mesh(shell.data)
    names = [m.name for m in shell.data.materials]
    planes = [((DOOR["x_hinge"], 0, 0), (1, 0, 0)), ((-DOOR["x_hinge"], 0, 0), (1, 0, 0)),
              ((0, DOOR["y0"], 0), (0, 1, 0)), ((0, DOOR["y1"], 0), (0, 1, 0)), ((0, 0, DOOR["z0"]), (0, 0, 1)),
              ((HOOD["x"], 0, 0), (1, 0, 0)), ((-HOOD["x"], 0, 0), (1, 0, 0)),
              ((0, HOOD["y0"], 0), (0, 1, 0)), ((0, HOOD["y1"], 0), (0, 1, 0)),
              ((ELID["x"], 0, 0), (1, 0, 0)), ((-ELID["x"], 0, 0), (1, 0, 0)),
              ((0, ELID["y0"], 0), (0, 1, 0)), ((0, ELID["y1"], 0), (0, 1, 0))]
    for co, no in planes:
        geom = list(src.verts) + list(src.edges) + list(src.faces)
        bmesh.ops.bisect_plane(src, geom=geom, dist=1e-5, plane_co=co, plane_no=no)
    src.normal_update()
    owner = {}
    for f in src.faces:
        nm = names[f.material_index] if f.material_index < len(names) else "steel"
        owner[f.index] = _classify(f.calc_center_median(), f.normal, nm)
    parts = {}
    for key in ("body", "door_l", "door_r", "hood", "engine_lid"):
        bm = src.copy()
        bm.faces.ensure_lookup_table()
        drop = [f for f in bm.faces if owner[f.index] != key]
        bmesh.ops.delete(bm, geom=drop, context="FACES")
        p = Part(key)
        p.bm.free()
        p.bm = bm
        p.mats = list(names)
        parts[key] = p
    src.free()
    return parts


def paint_zones(parts):
    """Сетка на форточках (геометрия). Заплатки (ржавчина, грунт, красная дверь) с этапа 3 рисует атлас
    кузова — er_textures.paint_body."""
    for key in ("door_l", "door_r"):
        d = parts[key]
        for f in d.bm.faces:
            if d.mats[f.material_index] == "glass" and f.calc_center_median().y < -0.25:
                f.material_index = d.mi("net")       # форточки — сетка


def hinge_z(part, x_near):
    zs = [v.co.z for v in part.bm.verts if abs(abs(v.co.x) - x_near) < 0.03]
    return max(zs) if zs else 1.14


# ------------------------------------------------------------------ детали кузова
def add_body_details(b):
    # петли дверей на центральной полосе крыши
    zr = 1.145
    for x in (-0.135, 0.135):
        cyl(b, (x, -0.05, zr), (x, 0.42, zr), 0.022, "chrome", 12)
    # бамперы из трубы — отдельная часть, см. build_bumpers
    # расширители арок: широкая чёрная полоса по контуру арки, наружу на 6 см, на болтах (по концепту)
    for ya in (Y_FA, Y_RA):
        arch = [(ya - 0.43, 0.22)] + arch_outline(ya) + [(ya + 0.43, 0.22)]
        for s in (1.0, -1.0):
            fender_flare(b, arch, s)
    # выхлоп
    tube(b, [(0.55, 1.70, 0.20), (0.55, 2.05, 0.19), (0.58, 2.22, 0.22)], 0.035, "pipe", 12)
    # горловина бака на левом переднем крыле + шланг под капот
    cyl(b, (-0.80, -1.72, 0.66), (-0.80, -1.72, 0.74), 0.045, "pipe", 14)
    cyl(b, (-0.80, -1.72, 0.74), (-0.80, -1.72, 0.765), 0.055, "black", 14)
    tube(b, [(-0.80, -1.72, 0.67), (-0.74, -1.62, 0.66), (-0.70, -1.45, 0.69)], 0.018, "hose", 10)
    # фары: правая пара прямоугольная, слева одна прямоугольная и одна круглая «с донора»
    for x, round_ in ((0.68, False), (0.48, False), (-0.48, False), (-0.68, True)):
        if round_:
            box(b, (x, -2.115, 0.515), (0.085, 0.03, 0.065), "black")
            lathe(b, (x, -2.145, 0.515), (0, -1, 0), [(0.0001, 0.012), (0.055, 0.012), (0.062, 0.0),
                                                       (0.062, -0.02), (0.0001, -0.02)], "chrome", 24)
            lathe(b, (x, -2.15, 0.515), (0, -1, 0), [(0.0001, 0.014), (0.05, 0.012), (0.05, 0.0),
                                                      (0.0001, 0.0)], "lamp", 24)
        else:
            box(b, (x, -2.12, 0.515), (0.088, 0.03, 0.052), "black")
            box(b, (x, -2.145, 0.515), (0.074, 0.006, 0.04), "lamp")
    # решётка между фарами: рамка и вертикальные планки (по концепту)
    box(b, (0.0, -2.12, 0.515), (0.37, 0.022, 0.06), "black")
    box(b, (0.0, -2.142, 0.572), (0.36, 0.008, 0.006), "steel")
    box(b, (0.0, -2.142, 0.458), (0.36, 0.008, 0.006), "steel")
    for k in range(13):
        box(b, (-0.33 + k * 0.055, -2.143, 0.515), (0.006, 0.010, 0.052), "steel_in")
    # задние фонари: слева прямоугольный, справа два круглых грузовых
    box(b, (-0.55, 2.12, 0.72), (0.22, 0.03, 0.07), "black")
    box(b, (-0.62, 2.148, 0.72), (0.12, 0.006, 0.055), "tail")
    box(b, (-0.43, 2.148, 0.72), (0.06, 0.006, 0.055), "amber")
    for x in (0.45, 0.64):
        lathe(b, (x, 2.13, 0.72), (0, 1, 0), [(0.0001, 0.0), (0.07, 0.0), (0.07, 0.03), (0.0001, 0.03)], "black", 20)
        lathe(b, (x, 2.16, 0.72), (0, 1, 0), [(0.0001, 0.008), (0.058, 0.006), (0.058, 0.0), (0.0001, 0.0)],
              "tail", 20)
    box(b, (0.0, 2.12, 0.60), (0.24, 0.025, 0.07), "black")      # место под номер
    # мотор под решёткой (виден сверху)
    box(b, (0.0, 1.60, 0.52), (0.30, 0.32, 0.18), "engine")
    for x in (-0.18, 0.18):
        box(b, (x, 1.60, 0.72), (0.10, 0.28, 0.04), "engine")
    lathe(b, (0.0, 1.62, 0.76), (0, 0, 1), [(0.0001, 0.0), (0.16, 0.0), (0.17, 0.03), (0.16, 0.07),
                                             (0.0001, 0.07)], "black", 24)
    # салон (посадка выше на SEAT_DZ, пол ниже — баг 43)
    box(b, (0.0, 0.07, 0.125), (0.82, 0.72, 0.012), "floor")            # пол
    box(b, (0.0, -0.05, 0.20), (0.12, 0.62, 0.075), "dash")             # тоннель
    box(b, (0.0, -0.66, 0.45), (0.83, 0.012, 0.32), "steel_in")         # моторный щит
    box(b, (0.0, 0.66, 0.60), (0.83, 0.012, 0.47), "steel_in")          # перегородка за креслами
    box(b, (0.0, -0.47, 0.765), (0.82, 0.09, 0.075), "dash")            # торпедо (под лобовым стеклом)
    box(b, (0.40, -0.40, 0.875), (0.20, 0.03, 0.06), "dash")            # корпус щитка (водитель слева = +X)
    box(b, (0.40, -0.43, 0.945), (0.21, 0.07, 0.012), "dash")           # козырёк щитка
    cyl(b, (0.40, -0.55, 0.70), (0.40, -0.31, 0.815), 0.025, "dash", 12)    # рулевая колонка
    ang = math.radians(20)
    back = (Vector((1, 0, 0)), Vector((0, math.sin(ang), math.cos(ang))), Vector((0, -math.cos(ang), math.sin(ang))))
    for x in (-0.40, 0.40):                                             # кресла: подставка, подушка, спинка, подголовник
        box(b, (x, 0.20, 0.23), (0.18, 0.20, 0.08), "dash")
        box(b, (x, 0.20, 0.34), (0.23, 0.25, 0.04), "seat")
        box(b, (x, 0.533, 0.517 + SEAT_DZ), (0.23, 0.30, 0.06), "seat", back)
        box(b, (x, 0.662, 0.869 + SEAT_DZ), (0.12, 0.07, 0.045), "seat", back)
    cyl(b, (0.0, -0.12, 0.27), (0.0, -0.07, 0.50), 0.009, "chrome", 8)   # высокий рычаг КПП
    lathe(b, (0.0, -0.07, 0.505), (0, 0, 1), [(0.0001, -0.02), (0.024, -0.012), (0.024, 0.014), (0.0001, 0.024)],
          "black", 14)
    lathe(b, (0.0, -0.12, 0.275), (0, 0, 1), [(0.0001, 0.0), (0.055, 0.0), (0.03, 0.05), (0.013, 0.10),
                                               (0.0001, 0.10)], "seat", 14)           # чехол рычага
    box(b, (0.0, -1.35, 0.33), (0.62, 0.58, 0.012), "steel_in")         # дно переднего багажника


def build_pipes():
    """Трубки вдоль порогов: охлаждение (сталь) и топливо (медь), шланги на концах, хомуты, держатели."""
    p = Part("pipes")
    for s in (1.0, -1.0):
        x = 0.93 * s
        for z, m, r in ((0.245, "pipe", 0.016), (0.285, "copper", 0.011)):
            run = [(0.80 * s, -0.86, z + 0.06), (x, -0.80, z), (x, 0.64, z), (0.80 * s, 0.70, z + 0.06)]
            pts = tube(p, run[1:3], r, m, 12, caps=False, round_corners=0)
            clamps(p, pts, r)
            tube(p, [run[0], (x, -0.82, z), run[1]], r * 1.25, "hose", 12, caps=False, round_corners=0.03)
            tube(p, [run[2], (x, 0.66, z), run[3]], r * 1.25, "hose", 12, caps=False, round_corners=0.03)
        # две красные светящиеся трубки снаружи (выделение neon, горят со светом), на кронштейнах.
        # Всё ниже 0.30: выше по борту проходит нижняя кромка поднимающейся двери-«крыла».
        for z in (0.215, 0.258):
            pts = tube(p, [(s * 0.972, -0.74, z), (s * 0.972, 0.58, z)], 0.016, "neon", 12, caps=False,
                       round_corners=0)
            clamps(p, pts, 0.016)
            for y in (-0.755, 0.595):
                cyl(p, (s * 0.972, y - 0.02, z), (s * 0.972, y + 0.02, z), 0.021, "black", 12)
        for y in (-0.55, -0.05, 0.45):                                    # кронштейны к порогу
            box(p, (0.955 * s, y, 0.236), (0.035, 0.014, 0.045), "clamp")
    return p


def build_bumpers():
    p = Part("bumpers")
    for s, y_bar, y_base in ((-1, -2.19, -2.08), (1, 2.19, 2.08)):
        for z in (0.36, 0.25):
            tube(p, [(-0.86, y_base, z), (-0.82, y_bar, z), (0.82, y_bar, z), (0.86, y_base, z)], 0.028,
                 "pipe", 12, round_corners=0.05)
        for x in (-0.45, 0.45):
            tube(p, [(x, y_bar, 0.23), (x, y_bar, 0.38)], 0.024, "pipe", 10, round_corners=0)
            box(p, (x, (y_bar + y_base) / 2 - 0.01 * s, 0.30), (0.05, abs(y_bar - y_base) / 2 + 0.02, 0.012),
                "pipe")
    build_tow_hooks(p)
    # сзади: фаркоп с шаром
    box(p, (0.0, 2.23, 0.23), (0.05, 0.05, 0.03), "pipe")
    cyl(p, (0.0, 2.25, 0.25), (0.0, 2.25, 0.31), 0.012, "chrome", 10)
    lathe(p, (0.0, 2.25, 0.31), (0, 0, 1), [(0.0001, 0.0), (0.02, 0.006), (0.025, 0.025), (0.018, 0.042),
                                             (0.0001, 0.048)], "chrome", 16)
    return p


def build_tow_hooks(p):
    """Буксировочные проушины спереди (кенгурятник с прожекторами убран: в концепте пользователя его нет)."""
    for s in (1, -1):
        lathe(p, (s * 0.55, -2.235, 0.20), (1, 0, 0),
              [(0.03 + 0.009 * math.cos(2 * math.pi * k / 8), 0.009 * math.sin(2 * math.pi * k / 8)) for k in range(8)],
              "pipe", 16)
        box(p, (s * 0.55, -2.205, 0.235), (0.02, 0.03, 0.012), "pipe")


def fender_flare(part, arch, s, width=0.055, x_in=0.90, x_out=0.96, bolt_step=0.11):
    """Расширитель арки: лента по контуру arch [(y, z)...] шириной width наружу от колеса (в плоскости борта),
    по X от x_in (внутри борта) до x_out; кромка у колеса чуть отогнута наружу. s — борт (+1 левый)."""
    n = len(arch)
    P = [Vector((0.0, y, z)) for y, z in arch]
    nrm = []
    for i in range(n):
        a = P[max(0, i - 1)]
        c = P[min(n - 1, i + 1)]
        t = (c - a).normalized()
        o = Vector((0.0, -t.z, t.y))                      # нормаль в плоскости YZ
        mid = (P[0] + P[-1]) / 2 + Vector((0, 0, 0.30))
        if o.dot(P[i] - mid) < 0:
            o = -o
        nrm.append(o)
    rings = []
    for i in range(n):
        inner, outer = P[i], P[i] + nrm[i] * width
        rings.append([Vector((s * x_in, inner.y, inner.z)), Vector((s * x_out, inner.y, inner.z)) - nrm[i] * 0.01,
                      Vector((s * x_out, outer.y, outer.z)), Vector((s * x_in, outer.y, outer.z))])
    tmp = bmesh.new()                                     # замкнутая лента: нормали считаем во временном меше
    vs = [[tmp.verts.new(p) for p in r] for r in rings]
    for i in range(n - 1):
        for k in range(4):
            tmp.faces.new((vs[i][k], vs[i][(k + 1) % 4], vs[i + 1][(k + 1) % 4], vs[i + 1][k]))
    tmp.faces.new(vs[0])
    tmp.faces.new(vs[-1][::-1])
    bmesh.ops.recalc_face_normals(tmp, faces=tmp.faces)
    tmp.verts.index_update()
    bm = part.bm
    mi = part.mi("black")
    nv = [bm.verts.new(v.co) for v in tmp.verts]
    for f in tmp.faces:
        bm.faces.new([nv[v.index] for v in f.verts]).material_index = mi
    tmp.free()
    # болты по средней линии ленты
    for i in range(n - 1):
        a = P[i] + nrm[i] * width * 0.55
        c = P[i + 1] + nrm[i + 1] * width * 0.55
        k = max(1, int((c - a).length / bolt_step))
        for j in range(k):
            q = a + (c - a) * ((j + 0.5) / k)
            cyl(part, (s * (x_out - 0.004), q.y, q.z), (s * (x_out + 0.008), q.y, q.z), 0.008, "chrome", 6)


def _near_verts(part, pred):
    return [v.co for v in part.bm.verts if pred(v.co)]


def surf_x(part, y, z, sgn, tol=0.05):
    """Наружная поверхность борта: самый дальний от оси X вблизи (y, z)."""
    xs = [c.x * sgn for c in _near_verts(part, lambda c: abs(c.y - y) < tol and abs(c.z - z) < tol and c.x * sgn > 0)]
    return sgn * max(xs) if xs else None


def surf_z(part, x, y, tol=0.05):
    zs = [c.z for c in _near_verts(part, lambda c: abs(c.x - x) < tol and abs(c.y - y) < tol)]
    return max(zs) if zs else None


def glass_plane(part, mname="glass_ws"):
    """Плоскость лобового стекла z = a + b*y + c*x по вершинам его граней (МНК)."""
    if mname not in part.mats:
        return None
    mi = part.mats.index(mname)
    pts = {v.index: v.co.copy() for f in part.bm.faces if f.material_index == mi for v in f.verts}
    pts = [c for c in pts.values() if c.y < 0]
    n = len(pts)
    if n < 3:
        return None
    sy = sum(c.y for c in pts); sx = sum(c.x for c in pts); sz = sum(c.z for c in pts)
    syy = sum(c.y * c.y for c in pts); sxx = sum(c.x * c.x for c in pts); sxy = sum(c.x * c.y for c in pts)
    szy = sum(c.z * c.y for c in pts); szx = sum(c.z * c.x for c in pts)
    A = Matrix(((n, sy, sx), (sy, syy, sxy), (sx, sxy, sxx)))
    try:
        a, b, c = A.inverted() @ Vector((sz, szy, szx))
    except ValueError:
        return None
    return a, b, c


def strap(part, pts, half_w, thick, m, bolts=True):
    """Плоская накладка по точкам поверхности (лента из брусков по сегментам) с болтами на концах."""
    up = Vector((0, 1, 0))
    for p0, p1 in zip(pts[:-1], pts[1:]):
        p0, p1 = Vector(p0), Vector(p1)
        ax = (p1 - p0).normalized()
        az = ax.cross(up).normalized()
        if az.z < 0:
            az = -az
        ay = az.cross(ax)
        box(part, (p0 + p1) / 2 + az * thick, ((p1 - p0).length / 2 + 0.004, half_w, thick), m, (ax, ay, az))
    if bolts:
        for q in (Vector(pts[0]), Vector(pts[-1])):
            cyl(part, q, q + Vector((0, 0, thick * 2 + 0.006)), 0.008, "chrome", 6)


def marker(part, base, color="amber"):
    """Маленький круглый габарит: чёрный стакан и цветной колпак."""
    lathe(part, base, (0, 0, 1), [(0.0001, -0.01), (0.032, -0.01), (0.032, 0.025), (0.0001, 0.025)], "black", 14)
    lathe(part, Vector(base) + Vector((0, 0, 0.025)), (0, 0, 1),
          [(0.0001, 0.0), (0.027, 0.0), (0.022, 0.018), (0.0001, 0.024)], color, 14)


def dial(part, c, axis, r, ring="amber"):
    """Круглый прибор: хромовый ободок, чёрная шкала, светлые риски, стрелка (статичная)."""
    c = Vector(c)
    ax = Vector(axis).normalized()
    lathe(part, c, ax, [(r, -0.004), (r + 0.009, -0.004), (r + 0.009, 0.010), (r, 0.010)], "chrome", 24)
    lathe(part, c, ax, [(0.0001, -0.004), (r, -0.004), (r, 0.0), (0.0001, 0.0)], "black", 24)
    u, v, w = frame_from(ax)
    for k in range(9):                                    # риски по дуге 240 градусов
        t = math.radians(-120 + k * 30)
        d = u * math.sin(t) + v * math.cos(t)
        box(part, c + d * (r * 0.82) + ax * 0.002, (0.0018, r * 0.10, 0.0015), ring, (d.cross(ax), d, ax))
    t = math.radians(-70)
    d = u * math.sin(t) + v * math.cos(t)
    box(part, c + d * (r * 0.35) + ax * 0.004, (0.0016, r * 0.42, 0.0015), "tail", (d.cross(ax), d, ax))
    lathe(part, c + ax * 0.004, ax, [(0.0001, 0.0), (0.006, 0.0), (0.006, 0.004), (0.0001, 0.004)], "chrome", 10)


def build_interior(b, back):
    """Салон по макету пользователя (2026-11-17): приборы, экран, тумблеры, кабели, свет, колба, ящик, ремни."""
    dz = SEAT_DZ
    # щиток: 5 приборов перед водителем (смотрят на водителя: +Y и чуть вверх)
    ax = Vector((0, 1, 0.25))
    for x, r in ((0.27, 0.030), (0.335, 0.040), (0.40, 0.046), (0.465, 0.040), (0.53, 0.030)):
        dial(b, (x, -0.368, 0.875 + (0.006 if r > 0.04 else -0.004)), ax, r)
    # экран с картой посередине панели (материал screen светится сам)
    box(b, (0.02, -0.383, 0.845), (0.105, 0.012, 0.072), "black")
    box(b, (0.02, -0.370, 0.845), (0.092, 0.003, 0.060), "screen")
    for x in (-0.08, 0.12):
        cyl(b, (x, -0.37, 0.80), (x, -0.36, 0.80), 0.006, "chrome", 8)
    # коробка-блок справа на торпедо
    box(b, (-0.30, -0.375, 0.78), (0.11, 0.010, 0.04), "steel")
    for x in (-0.37, -0.23):
        for z in (0.755, 0.805):
            cyl(b, (x, -0.366, z), (x, -0.36, z), 0.004, "chrome", 6)
    # панель тумблеров на тоннеле: плита, 5 тумблеров с красными откидными крышками, две красные кнопки
    box(b, (0.0, -0.30, 0.285), (0.10, 0.11, 0.008), "steel")
    cov = (Vector((1, 0, 0)), Vector((0, math.cos(1.1), math.sin(1.1))), Vector((0, -math.sin(1.1), math.cos(1.1))))
    for k in range(5):
        x = -0.072 + k * 0.036
        box(b, (x, -0.33, 0.297), (0.008, 0.012, 0.006), "black")
        cyl(b, (x, -0.33, 0.30), (x, -0.34, 0.33), 0.0028, "chrome", 6)
        box(b, (x, -0.348, 0.315), (0.011, 0.016, 0.003), "tail", cov)
    for x in (-0.04, 0.04):
        lathe(b, (x, -0.25, 0.293), (0, 0, 1), [(0.0001, 0.0), (0.012, 0.0), (0.012, 0.010), (0.0001, 0.012)],
              "tail", 12)
    # кабели в красной оплётке и медные трубки: из-под торпедо по тоннелю к перегородке
    for x, m, r in ((0.10, "strap", 0.008), (0.115, "copper", 0.006), (-0.10, "strap", 0.008),
                    (-0.115, "hose", 0.007)):
        tube(b, [(x, -0.62, 0.70), (x, -0.45, 0.66), (x * 1.1, -0.40, 0.33), (x * 1.1, 0.10, 0.285),
                 (x * 1.1, 0.60, 0.29)], r, m, 8, caps=True, round_corners=0.05)
    # красный свет: полоса под торпедо и над лобовым (выделение neon, горит со светом машины)
    box(b, (0.0, -0.376, 0.688), (0.62, 0.006, 0.006), "neon")
    zr = surf_z(b, 0.0, -0.02, 0.06) or (1.156 + ROOF_UP)
    box(b, (0.0, -0.03, zr - 0.05), (0.13, 0.012, 0.008), "neon")
    # плафон посередине потолка (по схеме салона): рамка и красная светящаяся панель
    zc_ = surf_z(b, 0.0, 0.35, 0.08) or (1.15 + ROOF_UP)
    box(b, (0.0, 0.35, zc_ - 0.045), (0.12, 0.07, 0.012), "black")
    box(b, (0.0, 0.35, zc_ - 0.059), (0.10, 0.055, 0.004), "neon")
    # внутренние трубы вдоль порогов (питание/топливо по схеме) с хомутами
    for s in (1, -1):
        for dz_, m, r in ((0.0, "pipe", 0.016), (0.04, "strap", 0.010)):
            pts = tube(b, [(s * 0.80, -0.60, 0.17 + dz_), (s * 0.80, 0.60, 0.17 + dz_)], r, m, 10, caps=True,
                       round_corners=0)
            clamps(b, pts, r)
    # два коротких рычага на консоли рядом с КПП
    for x in (-0.06, 0.06):
        cyl(b, (x, -0.02, 0.28), (x, 0.02, 0.40), 0.006, "chrome", 6)
        lathe(b, (x, 0.024, 0.405), (0, 0, 1), [(0.0001, -0.012), (0.014, -0.006), (0.014, 0.008), (0.0001, 0.014)],
              "tail" if x < 0 else "black", 10)
    # зеркало заднего вида под гребнем крыши
    cyl(b, (0.0, -0.06, zr - 0.04), (0.0, -0.07, zr - 0.09), 0.006, "black", 8)
    box(b, (0.0, -0.075, zr - 0.11), (0.11, 0.012, 0.03), "black")
    box(b, (0.0, -0.063, zr - 0.11), (0.10, 0.002, 0.025), "chrome")
    # колба: стеклянный цилиндр с красным свечением, медные крышки и шланги (у пассажира, у щита)
    cb = Vector((-0.55, -0.56, 0.16))
    lathe(b, cb, (0, 0, 1), [(0.0001, 0.0), (0.062, 0.0), (0.062, 0.03), (0.0001, 0.03)], "copper", 16)
    lathe(b, cb + Vector((0, 0, 0.03)), (0, 0, 1), [(0.0001, 0.0), (0.05, 0.0), (0.05, 0.17), (0.0001, 0.17)],
          "neon", 16)
    lathe(b, cb + Vector((0, 0, 0.20)), (0, 0, 1), [(0.0001, 0.0), (0.062, 0.0), (0.062, 0.03), (0.0001, 0.03)],
          "copper", 16)
    for k in range(4):
        t = 2 * math.pi * k / 4 + 0.4
        q = cb + Vector((math.cos(t) * 0.056, math.sin(t) * 0.056, 0.0))
        cyl(b, q, q + Vector((0, 0, 0.23)), 0.004, "copper", 6)
    tube(b, [cb + Vector((0, 0, 0.23)), cb + Vector((0, -0.04, 0.30)), (-0.45, -0.64, 0.50)], 0.010, "strap", 8,
         round_corners=0.04)
    tube(b, [cb + Vector((0.05, 0.0, 0.03)), (-0.40, -0.56, 0.18), (-0.20, -0.62, 0.30)], 0.008, "hose", 8,
         round_corners=0.04)
    # военный ящик между креслами, за тоннелем
    box(b, (0.0, 0.40, 0.215), (0.10, 0.17, 0.08), "crate")
    box(b, (0.0, 0.40, 0.298), (0.104, 0.174, 0.006), "crate")
    for y in (0.29, 0.51):
        tube(b, [(-0.06, y, 0.30), (-0.06, y, 0.32), (0.06, y, 0.32), (0.06, y, 0.30)], 0.004, "steel", 6,
             round_corners=0.01)
    # красные 4-точечные ремни на креслах
    for x in (-0.40, 0.40):
        for dx in (-0.09, 0.09):
            box(b, Vector((x + dx, 0.533, 0.517 + dz)) + back[2] * -0.064 + back[1] * 0.05, (0.024, 0.26, 0.003),
                "strap", back)
            box(b, (x + dx * 0.6, 0.30, 0.385), (0.024, 0.13, 0.003), "strap")
        box(b, (x, 0.20, 0.388), (0.20, 0.022, 0.003), "strap")                  # поясной
        box(b, (x, 0.24, 0.392), (0.03, 0.03, 0.006), "chrome")                  # замок
    # подкрылки передних колёс со стороны багажника (баг 44): стенка и крыша арки
    for s in (1, -1):
        top = arch_outline(Y_FA)[1][1] + 0.03
        box(b, (s * 0.600, Y_FA, 0.52), (0.015, 0.44, 0.19), "steel_in")
        box(b, (s * 0.755, Y_FA, top), (0.155, 0.32, 0.012), "steel_in")
        for sy in (-1, 1):
            p0 = Vector((s * 0.755, Y_FA + sy * 0.42, 0.37))
            p1 = Vector((s * 0.755, Y_FA + sy * 0.30, top))
            ax2 = (p1 - p0).normalized()
            fr = (Vector((1, 0, 0)), ax2, Vector((1, 0, 0)).cross(ax2))
            box(b, (p0 + p1) / 2, (0.155, (p1 - p0).length / 2, 0.012), "steel_in", fr)


def side_nets(parts):
    """Сетка-ромб на окнах: изнутри стёкол дверей и снаружи на задних боковых окнах (по макету)."""
    def fit_x(pts):
        n = len(pts)
        sy = sum(c.y for c in pts); sz = sum(c.z for c in pts); sx = sum(c.x for c in pts)
        syy = sum(c.y * c.y for c in pts); szz = sum(c.z * c.z for c in pts); syz = sum(c.y * c.z for c in pts)
        sxy = sum(c.x * c.y for c in pts); sxz = sum(c.x * c.z for c in pts)
        A = Matrix(((n, sy, sz), (sy, syy, syz), (sz, syz, szz)))
        return A.inverted() @ Vector((sx, sxy, sxz))

    def panel(part, pts, s, offset):
        if len(pts) < 3:
            return
        try:
            a, bb, cc = fit_x(pts)
        except ValueError:
            return
        ys = [c.y for c in pts]
        zs = [c.z for c in pts]
        yc, zc = (min(ys) + max(ys)) / 2, (min(zs) + max(zs)) / 2
        n = Vector((1.0, -bb, -cc)).normalized() * s
        e1 = Vector((0, 1, 0))
        e1 = (e1 - n * e1.dot(n)).normalized()
        e2 = n.cross(e1)
        if e2.z < 0:
            e1 = -e1
            e2 = n.cross(e1)
        c = Vector((a + bb * yc + cc * zc, yc, zc)) + n * offset
        box(part, c, ((max(ys) - min(ys)) / 2 * 0.86, (max(zs) - min(zs)) / 2 * 0.80, 0.002), "net", (e1, e2, n))

    for key, s in (("door_r", 1), ("door_l", -1)):
        p = parts[key]
        if "glass" not in p.mats:
            continue
        mi = p.mats.index("glass")
        pts = {}
        for f in p.bm.faces:
            if f.material_index == mi and abs(f.normal.x) > 0.5:
                for v in f.verts:
                    pts[v.index] = v.co.copy()
        panel(p, list(pts.values()), s, -0.015)
    b = parts["body"]
    if "glass" in b.mats:
        mi = b.mats.index("glass")
        for s in (1, -1):
            pts = {}
            for f in b.bm.faces:
                if f.material_index == mi and f.normal.x * s > 0.5 and f.calc_center_median().y > DOOR["y1"]:
                    for v in f.verts:
                        pts[v.index] = v.co.copy()
            panel(b, list(pts.values()), s, 0.012)


def add_extras(parts):
    """Детали «самосбора» по концепту пользователя (2026-11-17)."""
    b = parts["body"]
    # красная полоса-мигалка на гребне крыши у лобового стекла (выделение neon, горит со светом).
    # Шире гребня (±0.16) нельзя: над ним поворачиваются петли дверей-«крыльев».
    zr = surf_z(b, 0.0, 0.05, 0.06) or (1.156 + ROOF_UP)
    box(b, (0.0, 0.05, zr + 0.012), (0.15, 0.095, 0.014), "black")
    for k, y in enumerate((-0.02, 0.05, 0.12)):
        box(b, (0.0, y, zr + 0.034), (0.135, 0.028, 0.010), "neon")
    for x in (-0.13, 0.13):
        box(b, (x, 0.05, zr + 0.004), (0.012, 0.10, 0.006), "chrome")
    # оранжевые габариты на заднем левом крыле (три в ряд)
    for k in range(3):
        y = 1.88 + k * 0.08
        z = surf_z(b, 0.84, y, 0.07)
        if z is not None:
            marker(b, (0.84, y, z))
    # антенна на правом заднем крыле
    za = surf_z(b, -0.86, 1.98, 0.08) or 0.88
    base = Vector((-0.86, 1.98, za))
    lathe(b, base, (0, 0, 1), [(0.0001, -0.01), (0.022, -0.01), (0.018, 0.03), (0.0001, 0.03)], "black", 12)
    cyl(b, base + Vector((0, 0, 0.02)), base + Vector((0.0, 0.10, 1.15)), 0.0035, "black", 6)
    # дворники по плоскости лобового стекла
    gp = glass_plane(b)
    if gp:
        a0, bb, cc = gp
        n = Vector((-cc, -bb, 1.0)).normalized()
        for x0, x1 in ((0.44, 0.03), (-0.04, -0.45)):
            y0, y1 = -0.625, -0.605
            p0 = Vector((x0, y0, a0 + bb * y0 + cc * x0)) + n * 0.014
            p1 = Vector((x1, y1, a0 + bb * y1 + cc * x1)) + n * 0.014
            cyl(b, p0 - n * 0.02, p0 + n * 0.004, 0.009, "black", 8)                 # ось
            tube(b, [p0, p1], 0.0045, "black", 6, caps=True, round_corners=0)
            ax = (p1 - p0)
            ax = (ax - n * ax.dot(n)).normalized()
            ay = n.cross(ax)
            box(b, (p0 + p1) / 2 + n * 0.008, ((p1 - p0).length / 2 * 0.95, 0.006, 0.006), "hose", (ax, ay, n))
    # каркас: дуга за креслами, поперечина под ремни, трубы по стойкам лобового и под крышей
    zt = 1.035 + SEAT_DZ + 0.02
    tube(b, [(0.72, 0.645, 0.14), (0.72, 0.645, 0.86), (0.55, 0.645, zt), (-0.55, 0.645, zt),
             (-0.72, 0.645, 0.86), (-0.72, 0.645, 0.14)], 0.021, "pipe", 12, round_corners=0.09)
    tube(b, [(-0.71, 0.645, 1.09), (0.71, 0.645, 1.09)], 0.016, "pipe", 10, round_corners=0)
    for s in (1, -1):
        tube(b, [(s * 0.70, -0.58, 0.14), (s * 0.70, -0.52, 0.76), (s * 0.52, -0.14, zt),
                 (s * 0.52, 0.645, zt)], 0.018, "pipe", 10, round_corners=0.08)
    for x in (-0.72, 0.72):
        box(b, (x, 0.645, 0.14), (0.04, 0.04, 0.006), "pipe")                     # пятки дуги на полу
    # боковые валики кресел
    ang = math.radians(20)
    back = (Vector((1, 0, 0)), Vector((0, math.sin(ang), math.cos(ang))), Vector((0, -math.cos(ang), math.sin(ang))))
    for x in (-0.40, 0.40):
        for dx in (-0.20, 0.20):
            box(b, (x + dx, 0.20, 0.385), (0.035, 0.24, 0.03), "seat")
            box(b, Vector((x + dx, 0.533, 0.517 + SEAT_DZ)) + back[2] * 0.045, (0.035, 0.28, 0.05), "seat", back)
    build_interior(b, back)
    # ручки дверей и замки
    for key, s in (("door_r", 1), ("door_l", -1)):
        d_ = parts[key]
        sx = surf_x(d_, 0.30, 0.70, s)
        if sx is None:
            continue
        box(d_, (sx + s * 0.014, 0.30, 0.70), (0.008, 0.055, 0.010), "chrome")
        for dy in (-0.045, 0.045):
            box(d_, (sx + s * 0.007, 0.30 + dy, 0.70), (0.008, 0.008, 0.008), "chrome")
        cyl(d_, (sx, 0.40, 0.70), (sx + s * 0.008, 0.40, 0.70), 0.008, "chrome", 10)
    # красная дверь водителя: на её крышевой части две стальные накладки и красная полоса (по концепту)
    d_ = parts["door_r"]
    for y in (-0.22, 0.18):
        pts = []
        for x in (0.21, 0.30, 0.39, 0.48):
            z = surf_z(d_, x, y, 0.06)
            if z is not None:
                pts.append((x, y, z))
        if len(pts) >= 2:
            strap(d_, pts, 0.022, 0.004, "steel")
    zs = surf_z(d_, 0.24, -0.02, 0.06)
    if zs is not None:
        box(d_, (0.235, -0.02, zs + 0.006), (0.010, 0.30, 0.006), "tail")
    # габариты на передних углах капота
    h = parts["hood"]
    for x in (-0.60, 0.60):
        z = surf_z(h, x, -1.90, 0.07)
        if z is not None:
            marker(h, (x, -1.90, z))


def build_engine_lid_details(e):
    # решётка над мотором: рамка и поперечные планки
    z0 = 0.915
    box(e, (0.0, 1.91, z0), (0.56, 0.12, 0.012), "black")
    for i in range(9):
        y = 1.81 + i * 0.025
        box(e, (0.0, y, z0 + 0.018), (0.54, 0.006, 0.012), "steel")
    # большая загнутая труба по концепту: из решётки на крышке вверх и назад, торец перфорирован, внутри красное
    zb = 1.0
    cands = [v.co.z for v in e.bm.verts if abs(v.co.x - 0.36) < 0.08 and abs(v.co.y - 1.42) < 0.08]
    if cands:
        zb = max(cands)
    box(e, (0.36, 1.42, zb + 0.004), (0.13, 0.13, 0.008), "black")
    box(e, (0.36, 1.42, zb + 0.013), (0.115, 0.115, 0.002), "net")
    tube(e, [(0.36, 1.42, zb - 0.02), (0.36, 1.42, zb + 0.24), (0.36, 1.62, zb + 0.31)], 0.066, "pipe", 18,
         caps=False, round_corners=0.10)
    for t in (0.08, 0.20):                                   # хомуты
        lathe(e, (0.36, 1.42, zb + t), (0, 0, 1), [(0.066, -0.012), (0.074, -0.012), (0.074, 0.012), (0.066, 0.012)],
              "clamp", 18)
    lathe(e, (0.36, 1.625, zb + 0.31), (0, 1, 0), [(0.0001, 0.0), (0.068, 0.0), (0.068, 0.006), (0.0001, 0.006)],
          "net", 18)
    lathe(e, (0.36, 1.60, zb + 0.31), (0, 1, 0), [(0.0001, 0.0), (0.06, 0.0), (0.06, 0.004), (0.0001, 0.004)],
          "tail", 18)
    tube(e, [(0.36, 1.50, 0.92), (0.30, 1.62, 0.90), (0.15, 1.66, 0.82)], 0.03, "hose", 12, caps=False,
         round_corners=0.05)
    # вторая труба поменьше на правом заднем углу (вид сзади на макете)
    zc = max([v.co.z for v in e.bm.verts if abs(v.co.x + 0.55) < 0.07 and abs(v.co.y - 1.95) < 0.07] or [0.92])
    tube(e, [(-0.55, 1.95, zc - 0.02), (-0.55, 1.95, zc + 0.13), (-0.55, 2.02, zc + 0.17)], 0.035, "pipe", 14,
         caps=False, round_corners=0.05)
    lathe(e, (-0.55, 2.025, zc + 0.17), (0, 1, 0), [(0.0001, 0.0), (0.04, 0.0), (0.04, 0.006), (0.0001, 0.006)],
          "black", 14)


def build_mirrors(door_l, door_r):
    # левое: прямоугольное на короткой ножке
    cyl(door_l, (-0.86, -0.43, 0.83), (-0.97, -0.43, 0.86), 0.012, "black", 8)
    box(door_l, (-1.00, -0.43, 0.88), (0.035, 0.012, 0.06), "black",
        (Vector((1, 0, 0)), Vector((0, 1, 0)), Vector((0, 0, 1))))
    box(door_l, (-1.00, -0.443, 0.88), (0.03, 0.002, 0.052), "chrome")
    # правое: круглое на гнутом пруте
    tube(door_r, [(0.86, -0.42, 0.82), (0.95, -0.42, 0.86), (1.00, -0.44, 0.93)], 0.007, "pipe", 8, round_corners=0.03)
    lathe(door_r, (1.00, -0.45, 0.95), (0, -1, 0), [(0.0001, 0.012), (0.06, 0.012), (0.06, 0.0), (0.0001, 0.0)],
          "black", 20)
    lathe(door_r, (1.00, -0.463, 0.95), (0, -1, 0), [(0.0001, 0.002), (0.052, 0.002), (0.052, 0.0), (0.0001, 0.0)],
          "chrome", 20)


def build_steering():
    """Спортивный руль по макету: кожаный обод, три плоские хромовые спицы с отверстиями, ступица."""
    p = Part("steering")
    c = Vector(STEER_C)                 # левый руль: левый борт машины в Blender = +X
    ax = Vector((0, math.cos(math.radians(28)), math.sin(math.radians(28))))    # ось руля: к водителю и вверх
    u, v, w = frame_from(ax)
    torus = [(0.175 + 0.018 * math.cos(2 * math.pi * k / 10), 0.018 * math.sin(2 * math.pi * k / 10)) for k in range(10)]
    lathe(p, c, ax, torus, "seat", 32)
    for ang in (90, 210, 330):
        d = u * math.cos(math.radians(ang)) + v * math.sin(math.radians(ang))
        side = ax.cross(d)
        box(p, c + d * 0.09 + ax * 0.004, (0.022, 0.085, 0.004), "chrome", (side, d, ax))
        for t in (0.07, 0.12):
            q = c + d * t + ax * 0.006
            cyl(p, q, q + ax * 0.004, 0.009, "black", 10)
    lathe(p, c, ax, [(0.0001, -0.02), (0.045, -0.02), (0.045, 0.012), (0.0001, 0.018)], "black", 16)
    lathe(p, c + ax * 0.018, ax, [(0.0001, 0.0), (0.03, 0.0), (0.03, 0.004), (0.0001, 0.006)], "chrome", 16)
    return p, (tuple(c), tuple(ax))


def build_wheel(name, center, side):
    """Колесо по концепту: широкая всесезонная шина R с крупными блоками и грунтозацепами на плечах,
    чёрный штампованный диск с 8 отверстиями, 6 гаек, колпак ступицы, ниппель."""
    p = Part(name)
    c = Vector(center)
    ax = Vector((side, 0, 0))
    k = R / 0.36
    hw = 0.13                                            # полуширина шины
    tire = [(0.215, -hw + 0.012), (0.29 * k, -hw), (0.335 * k, -hw + 0.004), (0.346 * k, -hw + 0.02),
            (0.35 * k, -0.07), (0.35 * k, 0.07), (0.346 * k, hw - 0.02), (0.335 * k, hw - 0.004), (0.29 * k, hw),
            (0.215, hw - 0.012)]
    lathe(p, c, ax, tire, "tire", 36)
    u, v, w = frame_from(ax)

    def block(rad_c, a, half, tilt=0.0):
        rad = u * math.cos(rad_c) + v * math.sin(rad_c)
        tan = w.cross(rad)
        if tilt:                                         # плечевой зацеп: наклон по плечу шины
            rad2 = (rad * math.cos(tilt) + w * math.sin(tilt) * (1 if a > 0 else -1)).normalized()
        else:
            rad2 = rad
        ww = rad2.cross(tan).normalized() * -1
        fr = (tan, ww, rad2) if tan.cross(ww).dot(rad2) > 0 else (tan * -1, ww, rad2)
        return fr, rad

    nb = 20
    for row, a in ((0, -0.045), (1, 0.045)):             # центральные блоки, шахматкой
        for i in range(nb):
            t = 2 * math.pi * (i + 0.5 * row) / nb
            fr, rad = block(t, a, None)
            box(p, c + rad * (0.352 * k) + w * a, (0.036, 0.034, 0.010), "tire", fr)
    for a in (-0.112, 0.112):                            # плечевые зацепы, заходят на боковину
        for i in range(nb):
            t = 2 * math.pi * (i + 0.25 + (0.5 if a > 0 else 0.0)) / nb
            fr, rad = block(t, a, None, tilt=math.radians(35))
            box(p, c + rad * (0.338 * k) + w * a, (0.028, 0.030, 0.012), "tire", fr)
    rim = [(0.06, 0.035), (0.13, 0.042), (0.14, 0.058), (0.18, 0.058), (0.20, 0.085), (0.215, 0.10),
           (0.215, -0.10), (0.20, -0.10), (0.195, -0.02), (0.14, 0.012), (0.06, 0.012)]
    lathe(p, c, ax, rim, "rim", 36)
    for i in range(8):                                   # отверстия в диске
        t = 2 * math.pi * i / 8
        q = c + (u * math.cos(t) + v * math.sin(t)) * 0.16 + w * 0.059
        cyl(p, q, q + w * 0.004, 0.021, "black", 12)
    for i in range(6):                                   # гайки
        t = 2 * math.pi * i / 6 + 0.3
        q = c + (u * math.cos(t) + v * math.sin(t)) * 0.088 + w * 0.042
        cyl(p, q, q + w * 0.02, 0.011, "chrome", 6)
    lathe(p, c + w * 0.037, ax, [(0.0001, 0.035), (0.03, 0.03), (0.046, 0.012), (0.046, 0.0), (0.0001, 0.0)],
          "chrome", 20)
    q = c + u * 0.17 + w * 0.06                          # ниппель
    cyl(p, q, q + w * 0.03 + u * 0.01, 0.004, "black", 6)
    return p


# ------------------------------------------------------------------ сборка
def build_all():
    """Строит все части. Возвращает (objs, pivots)."""
    shell = build_shell()
    parts = split_shell(shell)
    bpy.data.objects.remove(shell)
    paint_zones(parts)
    pivots = {}
    zh = hinge_z(parts["door_r"], DOOR["x_hinge"])
    pivots["door_r"] = ((DOOR["x_hinge"], 0.0, zh), (0, 1, 0))
    pivots["door_l"] = ((-DOOR["x_hinge"], 0.0, zh), (0, 1, 0))
    zhood = max(v.co.z for v in parts["hood"].bm.verts if v.co.y > HOOD["y1"] - 0.03)
    pivots["hood"] = ((0.0, HOOD["y1"], zhood), (1, 0, 0))
    zel = max(v.co.z for v in parts["engine_lid"].bm.verts if v.co.y < ELID["y0"] + 0.03)
    pivots["engine_lid"] = ((0.0, ELID["y0"], zel), (1, 0, 0))

    _thicken(parts["body"], 0.02)
    _thicken(parts["door_l"], 0.04)
    _thicken(parts["door_r"], 0.04)
    _thicken(parts["hood"], 0.03)
    _thicken(parts["engine_lid"], 0.03)
    for p in parts.values():
        atlas_uv(p)                                  # до деталей: в атлас уходит только корпус
    add_body_details(parts["body"])
    build_engine_lid_details(parts["engine_lid"])
    build_mirrors(parts["door_l"], parts["door_r"])
    add_extras(parts)
    side_nets(parts)

    objs = {k: p.finish(recalc=False) for k, p in parts.items()}
    objs["pipes"] = build_pipes().finish()
    objs["bumpers"] = build_bumpers().finish()
    st, pivots["steering"] = build_steering()
    objs["steering"] = st.finish()
    for ya in (Y_FA, Y_RA):
        for side in (1.0, -1.0):
            nm = "wheel_%s_%s" % ("f" if ya < 0 else "r", "r" if side > 0 else "l")
            objs[nm] = build_wheel(nm, (side * TRACK / 2, ya, HUB_Z), side).finish(40)
    return objs, pivots


def opened_copy(obj, pivot, angle_deg, want_up=True):
    """Копия части, повёрнутая вокруг петли. Знак угла выбирается так, чтобы часть поднялась."""
    p, axis = Vector(pivot[0]), Vector(pivot[1])
    cz = sum((v.co.z for v in obj.data.vertices)) / len(obj.data.vertices)
    best = None
    for sgn in (1, -1):
        m = Matrix.Translation(p) @ Matrix.Rotation(math.radians(angle_deg) * sgn, 4, axis) @ Matrix.Translation(-p)
        z = sum(((m @ v.co).z for v in obj.data.vertices)) / len(obj.data.vertices)
        if best is None or (z > best[0]) == want_up:
            best = (z, m)
    me = obj.data.copy()
    me.transform(best[1])
    o = bpy.data.objects.new(obj.name + "_open", me)
    bpy.context.scene.collection.objects.link(o)
    return o


def integrity(obj):
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    nm = sum(1 for e in bm.edges if len(e.link_faces) > 2)
    bd = sum(1 for e in bm.edges if len(e.link_faces) < 2)
    tris = sum(len(f.verts) - 2 for f in bm.faces)
    bm.free()
    return nm, bd, tris
