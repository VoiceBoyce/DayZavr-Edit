# -*- coding: utf-8 -*-
r"""Построение деталей Echo Runner в Blender (bmesh) и запись их в MLOD (py3d).

Детали строятся в осях Blender машины (нос -Y, верх +Z, водитель +X) — см. er_common.py.
Запись: (x, y, z)_Blender -> (x, z, y)_DayZ, порядок вершин сохраняется, нормали со знаком минус (MLOD — внутрь).
Материалы — черновые цвета «#(argb,...)»; в поле rvmat — подсказка рендеру (metal / emit), не путь.
"""
import math

import bmesh
import numpy as np
from mathutils import Vector

import er_common as C

py3d = C.py3d

VP = "vb_echorunner\\data\\parts\\"


def _m(name, ca=False):
    return (VP + "er_%s_%s.paa" % (name, "ca" if ca else "co"), VP + "er_%s.rvmat" % name)


# материалы деталей — библиотека er_tex_parts.py (плитка 1 м)
COL = {
    "flare": _m("flare"),
    "steel": _m("steel_dark"),
    "net": _m("steel_dark"),
    "plate": _m("steel_rust"),
    "bolt": _m("bolt"),
    "lens": _m("lens"),
    "house": _m("flare"),
    "vent": _m("flare"),
    "neon": _m("neon"),
    "pipe": _m("steel_rust"),
    "hose": _m("rubber"),
    "rustm": _m("steel_rust"),
    "screen": _m("screen"),
    "red": _m("paint_red"),
    "glass": _m("glass", True),
    "cable": _m("cable"),
    "perf": _m("perf"),
}
UV_TILE = 1.0          # метров на плитку
MATS = list(COL)


# ---------------------------------------------------------------- построение
class Part:
    def __init__(self, name, target, lods, sels):
        self.name, self.target, self.lods, self.sels = name, target, lods, sels
        self.bm = bmesh.new()

    def face(self, vs, mat):
        try:
            f = self.bm.faces.new(vs)
        except ValueError:
            return None
        f.material_index = MATS.index(mat)
        return f


def V(*a):
    return Vector(a)


def perp(d):
    d = d.normalized()
    a = Vector((0, 0, 1)) if abs(d.z) < 0.9 else Vector((1, 0, 0))
    u = d.cross(a).normalized()
    return u, d.cross(u).normalized()


def bar(part, p0, p1, size, mat, up=None):
    """Брусок квадратного сечения от p0 до p1."""
    d = (p1 - p0)
    if d.length < 1e-5:
        return
    if up is not None:
        u = d.cross(up).normalized()
        v = d.normalized().cross(u).normalized()
    else:
        u, v = perp(d)
    h = size / 2
    corners = [u * h + v * h, -u * h + v * h, -u * h - v * h, u * h - v * h]
    bm = part.bm
    a = [bm.verts.new(p0 + c) for c in corners]
    b = [bm.verts.new(p1 + c) for c in corners]
    for i in range(4):
        j = (i + 1) % 4
        part.face((a[i], a[j], b[j], b[i]), mat)
    part.face(list(reversed(a)), mat)
    part.face(b, mat)


def box(part, c, ex, ey, ez, hx, hy, hz, mat):
    bm = part.bm
    vs = [bm.verts.new(c + ex * (sx * hx) + ey * (sy * hy) + ez * (sz * hz))
          for sx in (-1, 1) for sy in (-1, 1) for sz in (-1, 1)]
    for q in ((0, 1, 3, 2), (4, 6, 7, 5), (0, 4, 5, 1), (2, 3, 7, 6), (0, 2, 6, 4), (1, 5, 7, 3)):
        part.face([vs[i] for i in q], mat)


def cyl(part, c, axis, r, h, segs, mat):
    u, v = perp(axis)
    ax = axis.normalized()
    bm = part.bm
    a = [bm.verts.new(c + (u * math.cos(2 * math.pi * i / segs) + v * math.sin(2 * math.pi * i / segs)) * r)
         for i in range(segs)]
    b = [bm.verts.new(p.co + ax * h) for p in a]
    for i in range(segs):
        j = (i + 1) % segs
        part.face((a[i], a[j], b[j], b[i]), mat)
    part.face(list(reversed(a)), mat)
    part.face(b, mat)


def sweep(part, rings, mat, cap=True):
    bm = part.bm
    vr = [[bm.verts.new(p) for p in ring] for ring in rings]
    n = len(rings[0])
    for i in range(len(vr) - 1):
        for k in range(n):
            m = (k + 1) % n
            part.face((vr[i][k], vr[i][m], vr[i + 1][m], vr[i + 1][k]), mat)
    if cap:
        part.face(vr[0], mat)
        part.face(list(reversed(vr[-1])), mat)



def part_data(part, xf_inv=None):
    bm = part.bm
    bmesh.ops.remove_doubles(bm, verts=bm.verts, dist=1e-6)
    bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
    bmesh.ops.triangulate(bm, faces=[f for f in bm.faces if len(f.verts) > 4])
    bm.verts.index_update()
    pts = []
    for v in bm.verts:
        d = np.array((v.co.x, v.co.z, v.co.y))
        if xf_inv is not None:
            a, M = xf_inv
            d = M @ (d - a)
        pts.append(tuple(float(c) for c in d))
    faces = []
    for f in bm.faces:
        n = f.normal
        nd = np.array((-n.x, -n.z, -n.y))
        if xf_inv is not None:
            nd = xf_inv[1] @ nd
        uvs = []
        frame = getattr(part, "uvframe", None)          # (начало, ex, ey, полуширина, полувысота) — картинка на грань
        for v in f.verts:
            c = v.co
            if frame is not None:
                o, ex, ey, hx, hy = frame
                d = c - o
                uvs.append((d.dot(ex) / (2 * hx) + 0.5, d.dot(ey) / (2 * hy) + 0.5))
                continue
            ax = max(range(3), key=lambda i: abs(f.normal[i]))
            uv = {0: (c.y, c.z), 1: (c.x, c.z), 2: (c.x, c.y)}[ax]
            uvs.append((uv[0] / UV_TILE, uv[1] / UV_TILE))
        faces.append(([v.index for v in f.verts], uvs, tuple(float(c) for c in nd), COL[MATS[f.material_index]]))
    return pts, faces


def append(lod, data, sels):
    pts, faces = data
    base = len(lod.points)
    for co in pts:
        p = py3d.Point()
        p.coords = co
        lod.points.append(p)
    newf = []
    for idx, uvs, n, (tex, rv) in faces:
        ni = len(lod.facenormals)
        lod.facenormals.append(n)
        fa = py3d.Face(lod.points, lod.facenormals)
        fa.texture, fa.material = tex, rv
        for k, vi in enumerate(idx):
            vx = py3d.Vertex(lod.points, lod.facenormals)
            vx.point_index = base + vi
            vx.normal_index = ni
            vx.uv = (uvs[k][0], 1.0 - uvs[k][1])
            fa.vertices.append(vx)
        lod.faces.append(fa)
        newf.append(fa)
    newp = lod.points[base:]
    for s in sels:
        sel = lod.new_selection(s)
        sel.points.update({p: 1 for p in newp})
        sel.faces.update({f: 1 for f in newf})


def catmull(points, step=0.03):
    """Сглаженный путь через точки (Catmull-Rom), шаг ~step м."""
    P = [Vector(p) for p in points]
    if len(P) < 3:
        return P
    ext = [P[0] + (P[0] - P[1])] + P + [P[-1] + (P[-1] - P[-2])]
    out = []
    for i in range(1, len(ext) - 2):
        p0, p1, p2, p3 = ext[i - 1], ext[i], ext[i + 1], ext[i + 2]
        n = max(2, int((p2 - p1).length / step))
        for k in range(n):
            t = k / n
            t2, t3 = t * t, t * t * t
            out.append(0.5 * ((2 * p1) + (-p0 + p2) * t + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2 +
                              (-p0 + 3 * p1 - 3 * p2 + p3) * t3))
    out.append(P[-1])
    return out


def tangents(path):
    T = []
    for i in range(len(path)):
        a = path[max(i - 1, 0)]
        b = path[min(i + 1, len(path) - 1)]
        T.append((b - a).normalized())
    return T


def tube(part, path, r, segs, mat, rfun=None, caps=True):
    """Труба по пути (параллельный перенос рамки). rfun(i) -> множитель радиуса (гофра рукава)."""
    T = tangents(path)
    N = perp(T[0])[0]
    rings = []
    for i, p in enumerate(path):
        N = (N - T[i] * N.dot(T[i]))
        if N.length < 1e-6:
            N = perp(T[i])[0]
        N.normalize()
        B = T[i].cross(N).normalized()
        rr = r * (rfun(i) if rfun else 1.0)
        rings.append([p + (N * math.cos(2 * math.pi * k / segs) + B * math.sin(2 * math.pi * k / segs)) * rr
                      for k in range(segs)])
    sweep(part, rings, mat, cap=caps)
    return T


def offset_path(path, side_vec, d):
    """Параллельный путь: смещение на d вдоль T x side_vec (для пары труб). Где труба идёт вдоль side_vec
    (поперёк машины), направление берётся с предыдущей точки — без перескоков и зигзагов."""
    T = tangents(path)
    out, prev = [], None
    for p, t in zip(path, T):
        o = t.cross(side_vec)
        if o.length < 0.35 and prev is not None:
            o = prev - t * prev.dot(t)
        if o.length < 1e-6:
            o = perp(t)[0]
        o.normalize()
        if prev is not None and o.dot(prev) < 0:
            o = -o
        prev = o
        out.append(p + o * d)
    return out


def ring(part, c, axis, r_in, r_out, width, segs, mat):
    """Плоское кольцо-бандаж (замкнутое, без донышек) вокруг оси axis, центр c, ширина width вдоль оси."""
    ax = axis.normalized()
    u, v = perp(ax)
    prof = [(r_in, -width / 2), (r_out, -width / 2), (r_out, width / 2), (r_in, width / 2)]
    rings = []
    for k in range(segs):
        a = 2 * math.pi * k / segs
        rd = u * math.cos(a) + v * math.sin(a)
        rings.append([c + rd * r + ax * h for r, h in prof])
    bm = part.bm
    vr = [[bm.verts.new(p) for p in rr] for rr in rings]
    for k in range(segs):
        m = (k + 1) % segs
        for j in range(4):
            jj = (j + 1) % 4
            part.face((vr[k][j], vr[m][j], vr[m][jj], vr[k][jj]), mat)


def arclen_points(path, every, start=0.0):
    """Точки и касательные через каждые every м по длине пути."""
    T = tangents(path)
    out, acc, nxt = [], 0.0, start
    for i in range(1, len(path)):
        seg = (path[i] - path[i - 1]).length
        while acc + seg >= nxt and seg > 0:
            t = (nxt - acc) / seg
            out.append((path[i - 1].lerp(path[i], t), T[i]))
            nxt += every
        acc += seg
    return out
