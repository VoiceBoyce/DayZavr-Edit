# -*- coding: utf-8 -*-
"""Общая геометрия VB_EchoClothes (без Blender): простые меши для новых деталей одежды, поиск трубок модели и их осевых
линий. Координаты — оси модели DayZ (x вправо, y вверх, z — у надетой одежды назад, лицо смотрит в -z).

Меш: pts (N,3), nrm (N,3) — нормаль на точку, uv (N,2), faces — списки индексов (3/4), обход против часовой
стрелки, если смотреть снаружи (cross(b-a, c-a) смотрит наружу). Перевод в порядок MLOD — в сборщике модели.
"""
import heapq
import math

import numpy as np


class Mesh:
    def __init__(self):
        self.pts, self.nrm, self.uv, self.faces = [], [], [], []

    def add(self, p, n, uv=(0.0, 0.0)):
        self.pts.append(np.asarray(p, dtype=np.float64))
        n = np.asarray(n, dtype=np.float64)
        ln = np.linalg.norm(n)
        self.nrm.append(n / ln if ln > 1e-9 else np.array([0.0, 1.0, 0.0]))
        self.uv.append(uv)
        return len(self.pts) - 1

    def extend(self, other):
        base = len(self.pts)
        self.pts += other.pts
        self.nrm += other.nrm
        self.uv += other.uv
        self.faces += [[i + base for i in f] for f in other.faces]
        return self

    def transformed(self, R, t):
        """Копия, повёрнутая матрицей R (3x3) и сдвинутая на t."""
        m = Mesh()
        m.pts = [R @ p + t for p in self.pts]
        m.nrm = [R @ n for n in self.nrm]
        m.uv = list(self.uv)
        m.faces = [list(f) for f in self.faces]
        return m


def rot_z(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, -s, 0.0], [s, c, 0.0], [0.0, 0.0, 1.0]])


def frame_from_axis(axis):
    """Ортонормированный базис (u, v, w) с w вдоль axis."""
    w = np.asarray(axis, dtype=np.float64)
    w = w / np.linalg.norm(w)
    ref = np.array([0.0, 0.0, 1.0]) if abs(w[2]) < 0.9 else np.array([1.0, 0.0, 0.0])
    u = np.cross(ref, w)
    u /= np.linalg.norm(u)
    v = np.cross(w, u)
    return u, v, w


def cylinder(center, axis, r, h, sides=16, caps=True):
    """Цилиндр по оси axis длиной h с центром center."""
    u, v, w = frame_from_axis(axis)
    c = np.asarray(center, dtype=np.float64)
    m = Mesh()
    ring = []
    for end, z in enumerate((-h / 2, h / 2)):
        row = []
        for i in range(sides):
            a = 2 * math.pi * i / sides
            d = u * math.cos(a) + v * math.sin(a)
            row.append(m.add(c + w * z + d * r, d, (i / sides, end)))
        ring.append(row)
    for i in range(sides):
        j = (i + 1) % sides
        m.faces.append([ring[0][i], ring[0][j], ring[1][j], ring[1][i]])
    if caps:
        for end, z, sgn in ((0, -h / 2, -1), (1, h / 2, 1)):
            cap = [m.add(c + w * z + (u * math.cos(2 * math.pi * i / sides) + v * math.sin(2 * math.pi * i / sides)) * r,
                         w * sgn, (0.5 + 0.5 * math.cos(2 * math.pi * i / sides),
                                   0.5 + 0.5 * math.sin(2 * math.pi * i / sides))) for i in range(sides)]
            ctr = m.add(c + w * z, w * sgn, (0.5, 0.5))
            for i in range(sides):
                j = (i + 1) % sides
                m.faces.append([ctr, cap[i], cap[j]] if sgn > 0 else [ctr, cap[j], cap[i]])
    return m


def torus(center, axis, R, r, seg=20, sides=8):
    u, v, w = frame_from_axis(axis)
    c = np.asarray(center, dtype=np.float64)
    m = Mesh()
    idx = []
    for i in range(seg):
        a = 2 * math.pi * i / seg
        d = u * math.cos(a) + v * math.sin(a)
        row = []
        for k in range(sides):
            b = 2 * math.pi * k / sides
            n = d * math.cos(b) + w * math.sin(b)
            row.append(m.add(c + d * R + n * r, n, (i / seg, k / sides)))
        idx.append(row)
    for i in range(seg):
        i2 = (i + 1) % seg
        for k in range(sides):
            k2 = (k + 1) % sides
            m.faces.append([idx[i][k], idx[i2][k], idx[i2][k2], idx[i][k2]])
    return m


def box(center, half, R=None):
    """Брусок с полуразмерами half=(hx, hy, hz), повёрнутый R."""
    R = np.eye(3) if R is None else R
    c = np.asarray(center, dtype=np.float64)
    m = Mesh()
    hx, hy, hz = half
    for axis in range(3):
        for sgn in (-1, 1):
            n = np.zeros(3)
            n[axis] = sgn
            a1, a2 = [a for a in range(3) if a != axis]
            corners = []
            for s1, s2 in ((-1, -1), (1, -1), (1, 1), (-1, 1)):
                p = np.zeros(3)
                p[axis] = sgn * half[axis]
                p[a1] = s1 * half[a1]
                p[a2] = s2 * half[a2]
                corners.append(m.add(c + R @ p, R @ n, ((s1 + 1) / 2, (s2 + 1) / 2)))
            # обход наружу: (a1, a2) правая тройка с axis -> для sgn>0 как есть
            if (a2 - a1) % 3 == 1:
                f = corners if sgn > 0 else corners[::-1]
            else:
                f = corners[::-1] if sgn > 0 else corners
            m.faces.append(f)
    return m


def resample(path, step):
    """Ломаная с точками через step по длине (концы сохраняются)."""
    P = [np.asarray(p, dtype=np.float64) for p in path]
    seg = [np.linalg.norm(P[i + 1] - P[i]) for i in range(len(P) - 1)]
    total = sum(seg)
    n = max(2, int(round(total / step)) + 1)
    out, acc, k = [], 0.0, 0
    for j in range(n):
        s = total * j / (n - 1)
        while k < len(seg) - 1 and acc + seg[k] < s:
            acc += seg[k]
            k += 1
        t = 0.0 if seg[k] == 0 else (s - acc) / seg[k]
        out.append(P[k] + (P[k + 1] - P[k]) * min(max(t, 0.0), 1.0))
    return out


def corrugated(path, r, depth, period, sides=12, caps=True):
    """Гофрированный шланг вдоль path: радиус r - depth..r, рёбра через period."""
    pts = resample(path, period / 4)
    s = [0.0]
    for i in range(1, len(pts)):
        s.append(s[-1] + np.linalg.norm(pts[i] - pts[i - 1]))
    return tube_along(pts, r, sides, caps, [r - depth * (0.5 + 0.5 * math.cos(2 * math.pi * x / period)) for x in s])


def tube_along(path, r, sides=10, caps=True, radii=None):
    """Трубка радиуса r (или radii[i] по точкам) вдоль ломаной path (переносом рамки без скручивания)."""
    P = [np.asarray(p, dtype=np.float64) for p in path]
    T = []
    for i in range(len(P)):
        a = P[max(i - 1, 0)]
        b = P[min(i + 1, len(P) - 1)]
        t = b - a
        T.append(t / np.linalg.norm(t))
    u, v, _ = frame_from_axis(T[0])
    m = Mesh()
    rings = []
    total = 0.0
    for i, p in enumerate(P):
        if i > 0:
            total += np.linalg.norm(P[i] - P[i - 1])
            # перенос рамки: убрать из u составляющую вдоль новой касательной
            u = u - T[i] * np.dot(u, T[i])
            u /= np.linalg.norm(u)
            v = np.cross(T[i], u)
        row = []
        ri = radii[i] if radii is not None else r
        for k in range(sides):
            a = 2 * math.pi * k / sides
            d = u * math.cos(a) + v * math.sin(a)
            row.append(m.add(p + d * ri, d, (k / sides, total * 4)))
        rings.append(row)
    for i in range(len(P) - 1):
        for k in range(sides):
            k2 = (k + 1) % sides
            m.faces.append([rings[i][k], rings[i][k2], rings[i + 1][k2], rings[i + 1][k]])
    if caps:
        for i, sgn in ((0, -1), (len(P) - 1, 1)):
            n = T[i] * sgn
            ctr = m.add(P[i], n, (0.5, 0.5))
            cap = [m.add(m.pts[j], n, (0.5, 0.5)) for j in rings[i]]
            for k in range(sides):
                k2 = (k + 1) % sides
                m.faces.append([ctr, cap[k], cap[k2]] if sgn < 0 else [ctr, cap[k2], cap[k]])
    return m


def ellipsoid(center, radii, R=None, seg=16, rings=8):
    R = np.eye(3) if R is None else R
    c = np.asarray(center, dtype=np.float64)
    rx, ry, rz = radii
    m = Mesh()
    grid = []
    for j in range(rings + 1):
        th = math.pi * j / rings
        row = []
        for i in range(seg):
            ph = 2 * math.pi * i / seg
            d = np.array([math.sin(th) * math.cos(ph), math.cos(th), math.sin(th) * math.sin(ph)])
            p = d * np.array([rx, ry, rz])
            n = d / np.array([rx, ry, rz])
            row.append(m.add(c + R @ p, R @ n, (i / seg, j / rings)))
        grid.append(row)
    for j in range(rings):
        for i in range(seg):
            i2 = (i + 1) % seg
            m.faces.append([grid[j][i], grid[j][i2], grid[j + 1][i2], grid[j + 1][i]])
    return m


def fix_winding(m):
    """Обход граней наружу по нормалям точек (на случай ошибок построения): cross(b-a,c-a) · n > 0."""
    for fi, f in enumerate(m.faces):
        a, b, c = m.pts[f[0]], m.pts[f[1]], m.pts[f[2]]
        cr = np.cross(b - a, c - a)
        if np.dot(cr, sum(m.nrm[i] for i in f)) < 0:
            m.faces[fi] = f[::-1]
    return m


# ------------------------------------------------------------------ трубки модели
def components(points, faces, skip=()):
    """Связные куски граней (точки склеены по позиции 0,1 мм). -> список списков индексов граней."""
    P = np.round(np.asarray(points, dtype=np.float64), 4)
    key, vid = {}, np.zeros(len(P), dtype=np.int64)
    for i, p in enumerate(map(tuple, P)):
        vid[i] = key.setdefault(p, len(key))
    par = list(range(len(key)))

    def find(a):
        while par[a] != a:
            par[a] = par[par[a]]
            a = par[a]
        return a
    for fi, f in enumerate(faces):
        if fi in skip:
            continue
        r0 = find(vid[f[0]])
        for v in f[1:]:
            r = find(vid[v])
            if r != r0:
                par[r] = r0
    comp = {}
    for fi, f in enumerate(faces):
        if fi in skip:
            continue
        comp.setdefault(find(vid[f[0]]), []).append(fi)
    return list(comp.values())


def thin_parts(points, faces, skip=()):
    """Длинные тонкие куски (трубки, шланги, кабели): центр, длина, грани."""
    P = np.asarray(points, dtype=np.float64)
    out = []
    for fl in components(points, faces, skip):
        pts = P[sorted({v for fi in fl for v in faces[fi]})]
        c = pts.mean(0)
        sv = np.linalg.svd(pts - c, full_matrices=False)[1] / math.sqrt(len(pts))
        length = float(np.linalg.norm(pts.max(0) - pts.min(0)))
        if length > 0.1 and sv[1] < 0.03 and sv[1] / max(sv[0], 1e-6) < 0.4 and len(fl) >= 16:
            out.append({"faces": set(fl), "c": c, "n": len(fl), "len": length})
    return out


def find_part(parts, center, tol=0.035):
    """Кусок с центром ближе всего к center (не дальше tol)."""
    best = None
    for p in parts:
        d = float(np.linalg.norm(p["c"] - np.asarray(center)))
        if d <= tol and (best is None or d < best[0]):
            best = (d, p)
    return best[1] if best else None


def centerline(points, faces, face_ids, nbins=12, smooth_passes=4):
    """Осевая линия трубки: геодезическое расстояние по рёбрам от одного конца, центры по корзинам, сглаживание."""
    P = np.asarray(points, dtype=np.float64)
    key, wid = {}, {}
    for fi in face_ids:
        for v in faces[fi]:
            wid[v] = key.setdefault(tuple(np.round(P[v], 4)), len(key))
    W = np.zeros((len(key), 3))
    for v, w in wid.items():
        W[w] = P[v]
    adj = [[] for _ in range(len(key))]
    for fi in face_ids:
        f = [wid[v] for v in faces[fi]]
        for a, b in zip(f, f[1:] + f[:1]):
            if a != b:
                d = float(np.linalg.norm(W[a] - W[b]))
                adj[a].append((b, d))
                adj[b].append((a, d))

    def dijkstra(s):
        dist = np.full(len(key), np.inf)
        dist[s] = 0.0
        h = [(0.0, s)]
        while h:
            d, u = heapq.heappop(h)
            if d > dist[u]:
                continue
            for v, w in adj[u]:
                if d + w < dist[v]:
                    dist[v] = d + w
                    heapq.heappush(h, (d + w, v))
        return dist
    d0 = dijkstra(0)
    a = int(np.argmax(np.where(np.isfinite(d0), d0, -1)))
    da = dijkstra(a)
    ok = np.isfinite(da)
    edges = np.linspace(0, da[ok].max(), nbins + 1)
    line = []
    for i in range(nbins):
        m = ok & (da >= edges[i]) & (da <= edges[i + 1])
        if m.any():
            line.append(W[m].mean(0))
    line = np.array(line)
    for _ in range(smooth_passes):
        line[1:-1] = 0.25 * line[:-2] + 0.5 * line[1:-1] + 0.25 * line[2:]
    return line
