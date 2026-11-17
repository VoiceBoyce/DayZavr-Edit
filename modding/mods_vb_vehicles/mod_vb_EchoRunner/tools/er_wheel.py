# -*- coding: utf-8 -*-
r"""Колесо Echo Runner (концепт 05): грязевая шина с шашками, стальной диск 8 отверстий + 5 гаек, бедлок на болтах.

  blender.exe -b --factory-startup --python er_wheel.py -- <out.p3d> --R=0.46 --W=0.36 [--alex=<interceptor_wheel.p3d>]

Модель в осях колеса: ось вращения = x, внешняя сторона (диск, бедлок) = +x (как у колеса ALEX: у левых колёс
локальная +x смотрит наружу, у правых прокси повёрнут на 180°). Строим в Blender (ось колеса Blender X),
пишем MLOD: (x, y, z)_Blender -> (x, z, y)_DayZ, порядок вершин сохраняется, нормали со знаком минус
(конвенция MLOD — внутрь, см. py3d.blender_to_dayz).
Визуальные LOD 1–4 строятся здесь; служебные (Geometry, Memory, Fire, View) — из колеса ALEX с масштабом R/W.
"""
import math
import os
import sys

import bmesh
import bpy
from mathutils import Vector

TOOLS = r"d:\DayZavr\DayZavr-Edit\.claude\tools"
sys.path.insert(0, os.path.join(TOOLS, "py3d"))
import py3d  # noqa: E402

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
OUT = [a for a in args if not a.startswith("--")][0]
R = float(opts.get("R", 0.46))
W = float(opts.get("W", 0.36))
ALEX = opts.get("alex", r"d:\DayZavr\DayZavr-Edit\modding\mods_vb_vehicles\mod_vb_EchoRunner\основа_ALEX_mlod\proxy\interceptor_wheel.p3d")
ALEX_R, ALEX_W = 0.366, 0.288

RIM = 0.51 * R                    # радиус обода (посадка шины)
DESTROYED = bool(opts.get("destroyed"))   # --destroyed: разбитое колесо — шина спущена (просела к ободу), без шашек
TB = R - 0.024                    # основание протектора
TH = 0.026                        # высота шашки

# материалы предпросмотра (на этапе ретекстура заменятся атласом колеса)
VP = "vb_echorunner\\data\\parts\\"
MATS = {                                    # библиотека er_tex_parts.py
    "tyre": (VP + "er_rubber_co.paa", VP + "er_rubber.rvmat"),
    "rim": (VP + "er_paint_red_co.paa", VP + "er_paint_red.rvmat"),
    "ring": (VP + "er_steel_dark_co.paa", VP + "er_steel_dark.rvmat"),
    "bolt": (VP + "er_bolt_co.paa", VP + "er_bolt.rvmat"),
    "hub": (VP + "er_steel_dark_co.paa", VP + "er_steel_dark.rvmat"),
}
TILE = 0.5                                  # м на плитку (колесо — мельче, чем кузовные детали)
MAT_ORDER = list(MATS)


def new_bm():
    return bmesh.new()


def ring_pt(x, r, a):
    return (x, r * math.cos(a), r * math.sin(a))


def lathe(bm, profile, segs, mat, a0=0.0, closed=True):
    """profile: [(r, x)], замкнутый контур (последняя точка соединяется с первой)."""
    rings = []
    for (r, x) in profile:
        rings.append([bm.verts.new(ring_pt(x, max(r, 1e-5), a0 + 2 * math.pi * j / segs)) for j in range(segs)])
    n = len(rings)
    rng = range(n) if closed else range(n - 1)
    out = []
    for i in rng:
        A, B = rings[i], rings[(i + 1) % n]
        for j in range(segs):
            k = (j + 1) % segs
            try:
                f = bm.faces.new((A[j], A[k], B[k], B[j]))
                f.material_index = MAT_ORDER.index(mat)
                out.append(f)
            except ValueError:
                pass
    return out


def box_local(bm, center_fn, dims, mat, bevel=0.0):
    """Коробка в локальной рамке (ex, ey, ez) с центром c; dims = полуразмеры."""
    c, ex, ey, ez = center_fn
    hx, hy, hz = dims
    vs = []
    for sx in (-1, 1):
        for sy in (-1, 1):
            for sz in (-1, 1):
                vs.append(bm.verts.new(c + ex * (sx * hx) + ey * (sy * hy) + ez * (sz * hz)))
    idx = [(0, 1, 3, 2), (4, 6, 7, 5), (0, 4, 5, 1), (2, 3, 7, 6), (0, 2, 6, 4), (1, 5, 7, 3)]
    fs = []
    for q in idx:
        f = bm.faces.new([vs[i] for i in q])
        f.material_index = MAT_ORDER.index(mat)
        fs.append(f)
    if bevel > 0:
        edges = list({e for f in fs for e in f.edges})
        res = bmesh.ops.bevel(bm, geom=edges + vs, offset=bevel, segments=1, affect="EDGES", profile=0.5)
        for f in res.get("faces", []):
            f.material_index = MAT_ORDER.index(mat)
    return fs


def tread_frame(a, x, r):
    """Рамка шашки: ex = ось колеса, ey = радиально, ez = по окружности."""
    radial = Vector((0.0, math.cos(a), math.sin(a)))
    tang = Vector((0.0, -math.sin(a), math.cos(a)))
    return Vector((x, 0, 0)) + radial * r, Vector((1, 0, 0)), radial, tang


def cylinder(bm, x0, x1, r, center_yz, segs, mat, cap0=True, cap1=True, hexa=False):
    cy, cz = center_yz
    a = []
    b = []
    for j in range(segs):
        t = 2 * math.pi * j / segs + (math.pi / 6 if hexa else 0)
        a.append(bm.verts.new((x0, cy + r * math.cos(t), cz + r * math.sin(t))))
        b.append(bm.verts.new((x1, cy + r * math.cos(t), cz + r * math.sin(t))))
    m = MAT_ORDER.index(mat)
    for j in range(segs):
        k = (j + 1) % segs
        f = bm.faces.new((a[j], a[k], b[k], b[j]))
        f.material_index = m
    if cap0:
        f = bm.faces.new(list(reversed(a)))
        f.material_index = m
    if cap1:
        f = bm.faces.new(b)
        f.material_index = m


def build(level):
    """level 1..4 — детализация."""
    segs = {1: 48, 2: 36, 3: 24, 4: 14}[level]
    bm = new_bm()
    # --- каркас шины (замкнутый профиль: борт -> боковина -> плечо -> корона -> ... -> борт)
    hw = W / 2
    side = [(RIM + 0.004, -0.40 * W), (RIM + 0.35 * (R - RIM), -0.495 * W), (RIM + 0.72 * (R - RIM), -0.505 * W),
            (TB - 0.012, -0.49 * W), (TB, -0.44 * W)]
    prof = side + [(TB + 0.002, 0.0)] + [(r, -x) for (r, x) in reversed(side)]
    prof += [(RIM + 0.004, 0.0)]
    if level >= 3 and not DESTROYED:                 # без шашек: корона на полном радиусе
        prof = [(r + (TH * 0.8 if r >= TB - 0.013 else 0), x) for (r, x) in prof]
    if DESTROYED:                                    # спущенная шина: профиль просел к ободу, боковины раздуты
        prof = [(RIM + (r - RIM) * 0.55, x * (1.12 if r > RIM + 0.01 else 1.0)) for (r, x) in prof]
    lathe(bm, prof, segs, "tyre")
    # --- шашки
    if level <= 2 and not DESTROYED:
        pitches = 20 if level == 1 else 16
        bev = 0.006 if level == 1 else 0.0
        for i in range(pitches):
            a = 2 * math.pi * i / pitches
            ah = a + math.pi / pitches
            ang = 2 * math.pi / pitches
            lt = TB * ang * 0.25                      # полудлина по окружности
            # плечевые шашки: слева на шаге a, справа на шаге ah (ступенькой)
            for (aa, sgn) in ((a, -1), (ah, 1)):
                xc = sgn * (hw - 0.065)
                c, ex, ey, ez = tread_frame(aa, xc, TB + TH / 2)
                box_local(bm, (c, ex, ey, ez), (0.062, TH / 2, lt), "tyre", bev)
                # зацеп на боковине (уходит вниз по плечу)
                c2, ex2, ey2, ez2 = tread_frame(aa, sgn * (hw * 1.0 - 0.006), TB - 0.035)
                box_local(bm, (c2, ex2, ey2, ez2), (0.012, 0.035, lt * 0.85), "tyre", bev * 0.6)
            # центральные шашки: две, в разбежку
            for (aa, xc) in ((a + ang * 0.25, -0.045 * W / 0.36), (ah + ang * 0.25, 0.045 * W / 0.36)):
                c, ex, ey, ez = tread_frame(aa, xc, TB + TH / 2)
                box_local(bm, (c, ex, ey, ez), (0.046 * W / 0.36, TH / 2, lt * 0.9), "tyre", bev)
    # --- обод (бочка с закраинами)
    xin, xout = -0.41 * W, 0.41 * W
    lathe(bm, [(RIM - 0.014, xin), (RIM + 0.012, xin), (RIM + 0.012, xin + 0.008), (RIM, xin + 0.012),
               (RIM, xout - 0.012), (RIM + 0.012, xout - 0.008), (RIM + 0.012, xout), (RIM - 0.014, xout)],
          segs, "rim")
    # --- диск: смещён наружу, конус к ободу
    xd = 0.11 * W
    dprof = [(0.03 * R / 0.46, xd + 0.008), (RIM * 0.80, xd + 0.008), (RIM - 0.012, xd - 0.03),
             (RIM - 0.012, xd - 0.042), (RIM * 0.80 - 0.004, xd - 0.004), (0.03 * R / 0.46, xd - 0.004)]
    lathe(bm, dprof, segs, "rim")
    # --- колпак ступицы и 5 гаек вокруг (гайки вне зоны отверстий диска: 0.43–0.69 RIM)
    hr = 0.042 * R / 0.46
    lathe(bm, [(0.0005, xd + 0.004), (hr, xd + 0.004), (hr, xd + 0.026), (hr * 0.70, xd + 0.040),
               (0.0005, xd + 0.043)], max(12, segs // 2), "hub")
    if level <= 2:
        pcd = 0.068 * R / 0.46
        for i in range(5):
            t = 2 * math.pi * i / 5 + 0.3
            cylinder(bm, xd + 0.006, xd + 0.026, 0.011 * R / 0.46, (pcd * math.cos(t), pcd * math.sin(t)), 6, "bolt",
                     cap0=False, hexa=True)
    # --- бедлок: кольцо на внешней закраине + болты
    xr = xout
    lathe(bm, [(RIM - 0.006, xr), (RIM + 0.045, xr), (RIM + 0.045, xr + 0.013), (RIM - 0.006, xr + 0.013)],
          segs, "ring")
    if level <= 2:
        nb = 20 if level == 1 else 12
        for i in range(nb):
            t = 2 * math.pi * (i + 0.5) / nb
            rr = RIM + 0.022
            cylinder(bm, xr + 0.012, xr + 0.022, 0.0075 * R / 0.46, (rr * math.cos(t), rr * math.sin(t)),
                     6 if level == 1 else 4, "bolt", cap0=False, hexa=True)
    bmesh.ops.remove_doubles(bm, verts=bm.verts, dist=1e-6)
    bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
    me = bpy.data.meshes.new("wheel_%d" % level)
    bm.to_mesh(me)
    bm.free()
    for k in MAT_ORDER:
        mt = bpy.data.materials.get(k) or bpy.data.materials.new(k)
        me.materials.append(mt)
    ob = bpy.data.objects.new("wheel_%d" % level, me)
    bpy.context.scene.collection.objects.link(ob)
    # отверстия в диске (8 шт.) — булево вычитание
    if level <= 2:
        cut_bm = new_bm()
        rh = RIM * 0.56
        for i in range(8):
            t = 2 * math.pi * i / 8
            cylinder(cut_bm, xd - 0.08, xd + 0.05, RIM * 0.13, (rh * math.cos(t), rh * math.sin(t)), 16, "rim")
        cme = bpy.data.meshes.new("cut")
        cut_bm.to_mesh(cme)
        cut_bm.free()
        for k in MAT_ORDER:                          # стенки отверстий получают материал диска
            cme.materials.append(bpy.data.materials[k])
        cut = bpy.data.objects.new("cut", cme)
        bpy.context.scene.collection.objects.link(cut)
        # вычитаем только из диска: отделим диск, вырежем, вернём
        mod = ob.modifiers.new("holes", "BOOLEAN")
        mod.operation = "DIFFERENCE"
        mod.solver = "EXACT"
        mod.object = cut
        bpy.context.view_layer.objects.active = ob
        bpy.ops.object.modifier_apply(modifier="holes")
        bpy.data.objects.remove(cut)
    return ob


def uv_and_export(ob, res):
    """UV в метрах (плитка TILE): протектор и плечи — по окружности (u = дуга, v = вдоль оси), боковины и диск —
    плоская проекция на плоскость колеса. Угол разворачивается относительно центра грани (без скачка на ±pi)."""
    me = ob.data
    me.set_sharp_from_angle(angle=math.radians(35))
    bm = new_bm()
    bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=[f for f in bm.faces if len(f.verts) > 4])
    uvl = bm.loops.layers.uv.new("UV")
    for f in bm.faces:
        cen = f.calc_center_median()
        ac = math.atan2(cen.z, cen.y)
        radial = abs(f.normal.x) < 0.7
        for lp in f.loops:
            x, y, z = lp.vert.co
            if radial:
                a = math.atan2(z, y)
                while a - ac > math.pi:
                    a -= 2 * math.pi
                while a - ac < -math.pi:
                    a += 2 * math.pi
                lp[uvl].uv = (a * R / TILE, x / TILE)
            else:
                lp[uvl].uv = (y / TILE, z / TILE)
    bm.to_mesh(me)
    bm.free()
    me.update()
    lod = py3d.LOD()
    lod.resolution = float(res)
    for v in me.vertices:
        p = py3d.Point()
        p.coords = (v.co.x, v.co.z, v.co.y)
        lod.points.append(p)
    nidx = {}
    cn = me.corner_normals
    uvd = me.uv_layers["UV"].data
    for poly in me.polygons:
        fa = py3d.Face(lod.points, lod.facenormals)
        tex, rv = MATS[MAT_ORDER[poly.material_index]]
        fa.texture, fa.material = tex, rv
        for li in poly.loop_indices:
            lp = me.loops[li]
            n = cn[li].vector
            key = (round(-n.x, 4), round(-n.z, 4), round(-n.y, 4))
            if key not in nidx:
                nidx[key] = len(lod.facenormals)
                lod.facenormals.append(key)
            vx = py3d.Vertex(lod.points, lod.facenormals)
            vx.point_index = lp.vertex_index
            vx.normal_index = nidx[key]
            u, v = uvd[li].uv
            vx.uv = (u, 1.0 - v)
            fa.vertices.append(vx)
        lod.faces.append(fa)
    sel = lod.new_selection("wheel")
    sel.points = {p: 1 for p in lod.points}
    sel.faces = {f: 1 for f in lod.faces}
    print("LOD", res, "points", len(lod.points), "faces", len(lod.faces))
    return lod


def scaled_service_lods():
    with open(ALEX, "rb") as f:
        w = py3d.P3D(f)
    kx, kr = W / ALEX_W, R / ALEX_R
    out = []
    for lod in w.lods:
        if lod.resolution < 1000:
            continue
        for p in lod.points:
            x, y, z = p.coords
            p.coords = (x * kx, y * kr, z * kr)
        out.append(lod)
    return out


bpy.ops.wm.read_factory_settings(use_empty=True)
model = py3d.P3D()
for level in (1, 2, 3, 4):
    model.lods.append(uv_and_export(build(level), level))
model.lods.extend(scaled_service_lods())
os.makedirs(os.path.dirname(OUT), exist_ok=True)
model.save(OUT)
print("WHEEL", OUT, "R", R, "W", W, "rim", round(RIM, 3))
