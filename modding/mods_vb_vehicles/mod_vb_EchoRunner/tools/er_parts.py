# -*- coding: utf-8 -*-
r"""Детали кузова Echo Runner (этап 2): расширители арок, люстра, сетки окон, решётка крыла, накладки с заклёпками;
запаска ALEX убрана. Детали строятся лучами по поверхности кузова; итог — MLOD машины и дверей.

  blender.exe -b --factory-startup --python er_parts.py -- <вариант> [--W=0.36]
  вход:  work/<вариант>/interceptor.p3d + proxy/ (после er_build.py и er_wheel.py)
  выход: work/<вариант>/final/interceptor.p3d + final/proxy/ (двери с сетками и накладками, колесо, капот)

Материалы деталей — черновые цвета «#(argb,...)» (в поле rvmat — подсказка рендеру: metal / emit);
на этапе ретекстура заменяются атласом деталей. Выделения деталей: er_flares, er_lightbar, er_net, er_vent,
er_plates, er_rivets (в dmgzone_* не кладём: там hiddenSelectionsTextures ALEX перекрасит их в кузов).
Оси Blender: машина как в er_common (нос -Y, верх +Z, водитель +X).
"""
import math
import os
import shutil
import sys

import bmesh
import bpy
import numpy as np
from mathutils import Vector
from mathutils.bvhtree import BVHTree

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import er_common as C  # noqa: E402
import er_mlod as M  # noqa: E402
import er_interior  # noqa: E402

py3d = C.py3d
args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
VAR = [a for a in args if not a.startswith("--")][0]
MOD = os.path.dirname(HERE)
WD = os.path.join(MOD, "work", VAR)
OUT = os.path.join(WD, "final")
W = float(opts.get("W", 0.36))
WR = None   # радиус колеса — из модели колеса

from er_geom import COL, MATS, Part, V, perp, bar, box, cyl, sweep, part_data, append  # noqa: E402


# ---------------------------------------------------------------- лучи
bpy.ops.wm.read_factory_settings(use_empty=True)
objs, XF = C.load_car(os.path.join(WD, "interceptor.p3d"), os.path.join(WD, "proxy"), 1.0, with_mats=False)
dg = bpy.context.evaluated_depsgraph_get()
BVH = {k: BVHTree.FromObject(o, dg) for k, o in objs.items()}
DOOR = {1: [k for k in objs if "doors_driver" in k][0], -1: [k for k in objs if "doors_codriver" in k][0]}


def ray(name, o, d, dist=4.0):
    d = Vector(d).normalized()
    loc, n, i, dd = BVH[name].ray_cast(Vector(o), d, dist)
    if loc is None:
        return None, None
    if n.dot(-d) < 0:
        n = -n
    return loc, n


def hit(o, d, names, dist=1.2):
    """Ближайшее попадание луча по объектам names (точка) или None."""
    best = None
    for nm in names:
        loc, n = ray(nm, o, d, dist)
        if loc is not None and (best is None or (loc - Vector(o)).length < (best - Vector(o)).length):
            best = loc
    return best


def side_hit(names, y, z, s, xmin=0.55):
    """Первое попадание с борта s (+1 левый) по объектам names; только внешняя обшивка (|x| > xmin)."""
    best = None
    for nm in names:
        loc, n = ray(nm, (s * 2.0, y, z), (-s, 0, 0))
        if loc is not None and s * loc.x > xmin and (best is None or s * loc.x > s * best[0].x):
            best = (loc, n)
    return best


# колёса: центры из прокси (оси DayZ -> Blender)
WHEELS = []
for nm, xf in XF.items():
    if nm.startswith("interceptor_wheel_00") and not nm.endswith("005"):
        a = xf[0]
        WHEELS.append((nm, Vector((a[0], a[2], a[1]))))
wm = C.load_p3d(os.path.join(WD, "proxy", "interceptor_wheel.p3d"))
WR = max(abs(p.coords[1]) for p in wm.lods[0].points)
print("WHEELS", [(n, tuple(round(c, 3) for c in v)) for n, v in WHEELS], "R", round(WR, 3))
PARTS = []


# ---------------------------------------------------------------- 1. расширители арок
def flares():
    for nm, wc in WHEELS:
        s = 1 if wc.x > 0 else -1
        front = wc.y < 0
        p = Part("flare_" + nm, "body", [1, 2, 3, 4], ["er_flares"])
        found = []
        for th in range(0, 181, 6):
            t = math.radians(th)
            dy, dz = -math.cos(t), math.sin(t)
            for k in range(38, 66):
                r = k / 100
                h = side_hit(["body"], wc.y + r * dy, wc.z + r * dz, s, 0.85)
                if h:
                    found.append((th, r))
                    break
        th0, th1 = found[0][0], found[-1][0]
        th0, th1 = max(th0, 4), min(th1, 176)
        nseg = 8
        ri = WR + 0.035
        ro = ri + 0.115
        tire_out = abs(wc.x) + W / 2 + 0.008
        rings, mids = [], []
        for i in range(nseg + 1):
            t = math.radians(th0 + (th1 - th0) * i / nseg)
            dy, dz = -math.cos(t), math.sin(t)
            hi = side_hit(["body"], wc.y + ri * dy, wc.z + ri * dz, s, 0.7)
            ho = side_hit(["body"], wc.y + ro * dy, wc.z + ro * dz, s, 0.7)
            xs_i = abs(hi[0].x) if hi else tire_out - 0.03
            xs_o = abs(ho[0].x) if ho else xs_i
            x_lip = max(tire_out + 0.03, xs_i + 0.05)
            prof = [(ri, xs_i - 0.02), (ri, x_lip), (ri + 0.03, x_lip + 0.004), (ro, xs_o + 0.012), (ro, xs_o - 0.015)]
            ring = [V(s * xa, wc.y + rad * dy, wc.z + rad * dz) for rad, xa in prof]
            rings.append(ring)
        sweep(p, rings, "flare")
        # болты на верхней грани, по середине каждого сегмента
        for i in range(nseg):
            a, b = rings[i], rings[i + 1]
            c = (a[2] + a[3] + b[2] + b[3]) / 4
            n = (b[2] - a[2]).cross(a[3] - a[2]).normalized()
            if n.x * s < 0:
                n = -n
            cyl(p, c - n * 0.002, n, 0.009, 0.008, 6, "bolt")
        PARTS.append(p)
        print("FLARE", nm, "th", th0, th1, "ri", round(ri, 3), "tire_out", round(tire_out, 3))


# ---------------------------------------------------------------- 2. сетки окон
def flood_region(names, s, seed, y0, y1, z0, z1, step=0.015):
    ny, nz = int((y1 - y0) / step) + 1, int((z1 - z0) / step) + 1
    hit = np.zeros((ny, nz), bool)
    hx = np.full((ny, nz), np.nan)
    for i in range(ny):
        for j in range(nz):
            h = side_hit(names, y0 + i * step, z0 + j * step, s, 0.55)
            if h:
                hit[i, j] = True
                hx[i, j] = abs(h[0].x)
    si, sj = int((seed[0] - y0) / step), int((seed[1] - z0) / step)
    if hit[si, sj]:
        return None
    reg = np.zeros_like(hit)
    stack = [(si, sj)]
    while stack:
        i, j = stack.pop()
        if i < 0 or j < 0 or i >= ny or j >= nz or reg[i, j] or hit[i, j]:
            continue
        reg[i, j] = True
        stack.extend(((i + 1, j), (i - 1, j), (i, j + 1), (i, j - 1)))
    if reg[0, :].any() or reg[-1, :].any() or reg[:, 0].any() or reg[:, -1].any():
        print("WARN область окна не замкнута", seed)
    pts = [(y0 + i * step, z0 + j * step) for i, j in zip(*np.nonzero(reg))]
    # плоскость окна по рамке: x = a + b*y + c*z
    fr = []
    for i, j in zip(*np.nonzero(reg)):
        for di, dj in ((1, 0), (-1, 0), (0, 1), (0, -1), (2, 0), (-2, 0), (0, 2), (0, -2)):
            a, b = i + di, j + dj
            if 0 <= a < ny and 0 <= b < nz and hit[a, b]:
                fr.append((y0 + a * step, z0 + b * step, hx[a, b]))
    fr = np.array(fr)
    A = np.c_[np.ones(len(fr)), fr[:, 0], fr[:, 1]]
    coef, *_ = np.linalg.lstsq(A, fr[:, 2], rcond=None)
    return pts, coef


def hull(pts):
    pts = sorted(set(pts))
    if len(pts) < 3:
        return pts

    def cr(o, a, b):
        return (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0])
    lo, up = [], []
    for p in pts:
        while len(lo) >= 2 and cr(lo[-2], lo[-1], p) <= 0:
            lo.pop()
        lo.append(p)
    for p in reversed(pts):
        while len(up) >= 2 and cr(up[-2], up[-1], p) <= 0:
            up.pop()
        up.append(p)
    return lo[:-1] + up[:-1]


def shrink(poly, d):
    cy = sum(p[0] for p in poly) / len(poly)
    cz = sum(p[1] for p in poly) / len(poly)
    out = []
    for y, z in poly:
        vy, vz = y - cy, z - cz
        L = math.hypot(vy, vz)
        out.append((y - vy / L * d, z - vz / L * d) if L > d else (y, z))
    return out


def clip_line(poly, p0, d):
    """Отрезок прямой p0 + t*d внутри выпуклого многоугольника (против часовой)."""
    t0, t1 = -1e9, 1e9
    n = len(poly)
    for i in range(n):
        a, b = poly[i], poly[(i + 1) % n]
        ex, ez = b[0] - a[0], b[1] - a[1]
        nx, nz = -ez, ex                       # внутренняя нормаль для обхода против часовой
        num = nx * (p0[0] - a[0]) + nz * (p0[1] - a[1])
        den = nx * d[0] + nz * d[1]
        if abs(den) < 1e-12:
            if num < 0:
                return None
            continue
        t = -num / den
        if den > 0:
            t0 = max(t0, t)
        else:
            t1 = min(t1, t)
    return (t0, t1) if t1 > t0 else None


def net(part, poly, coef, s, inset=0.012, spacing=0.07, rod=0.0055, frame=0.014):
    def X(y, z):
        return s * (coef[0] + coef[1] * y + coef[2] * z - inset)
    poly = shrink(poly, 0.006)
    n = len(poly)
    for i in range(n):
        a, b = poly[i], poly[(i + 1) % n]
        bar(part, V(X(*a), a[0], a[1]), V(X(*b), b[0], b[1]), frame, "net", up=Vector((s, 0, 0)))
    ys = [p[0] for p in poly]
    zs = [p[1] for p in poly]
    span = (max(ys) - min(ys)) + (max(zs) - min(zs))
    for d in ((1 / math.sqrt(2), 1 / math.sqrt(2)), (1 / math.sqrt(2), -1 / math.sqrt(2))):
        nrm = (-d[1], d[0])
        c0 = min(ys) * nrm[0] + min(zs) * nrm[1]
        k = -int(span / spacing) - 2
        while k < int(span / spacing) + 2:
            c = k * spacing
            p0 = (nrm[0] * c, nrm[1] * c)
            seg = clip_line(poly, p0, d)
            if seg:
                ya, za = p0[0] + d[0] * seg[0], p0[1] + d[1] * seg[0]
                yb, zb = p0[0] + d[0] * seg[1], p0[1] + d[1] * seg[1]
                if math.hypot(yb - ya, zb - za) > 0.02:
                    bar(part, V(X(ya, za) - s * 0.002, ya, za), V(X(yb, zb) - s * 0.002, yb, zb), rod, "net",
                        up=Vector((s, 0, 0)))
            k += 1
    _ = c0


def window_nets():
    for s in (1, -1):
        door = DOOR[s]
        dv = [v.co for v in objs[door].data.vertices]
        dy0, dy1 = min(v.y for v in dv), max(v.y for v in dv)
        # окно двери: между верхом двери и крышей; сетка едет с дверью
        r = flood_region([door, "body"], s, (0.15, 1.30), dy0 - 0.25, dy1 + 0.05, 1.0, 1.62)
        if r:
            pts, coef = r
            poly = hull(pts)
            p = Part("net_door_%d" % s, "door_driver" if s > 0 else "door_codriver", [1, 2, 3], ["er_net"])
            net(p, poly, coef, s)
            PARTS.append(p)
            print("NET door", s, "pts", len(pts), "poly", len(poly), "plane", [round(c, 3) for c in coef])
        # задняя форточка (кузов)
        r = flood_region(["body", door], s, (0.86, 1.30), 0.55, 1.6, 1.0, 1.62)
        if r:
            pts, coef = r
            poly = hull(pts)
            p = Part("net_quarter_%d" % s, "body", [1, 2, 3, 1100, 1201], ["er_net"])
            net(p, poly, coef, s)
            PARTS.append(p)
            print("NET quarter", s, "pts", len(pts), "poly", len(poly))


# ---------------------------------------------------------------- 3. люстра на крыше
def lightbar():
    yf = None
    for k in range(-30, 40):
        y = k / 100
        loc, n = ray("body", (0, y, 3), (0, 0, -1))
        if loc is not None and loc.z > 1.45:
            yf = y
            break
    yb = yf + 0.12
    zr = ray("body", (0, yb, 3), (0, 0, -1))[0].z
    p = Part("lightbar", "body", [1, 2, 3, 4], ["er_lightbar"])
    zbar = zr + 0.095
    for x in (-0.48, 0.48):
        zx = ray("body", (x, yb, 3), (0, 0, -1))[0].z
        bar(p, V(x, yb, zx - 0.01), V(x, yb, zbar), 0.03, "house")
        box(p, V(x, yb, zx + 0.004), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.035, 0.05, 0.006, "steel")
    bar(p, V(-0.66, yb, zbar), V(0.66, yb, zbar), 0.036, "house", up=Vector((0, 0, 1)))
    for x in (-0.48, -0.16, 0.16, 0.48):
        c = V(x, yb - 0.035, zbar + 0.075)
        box(p, c, V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.075, 0.045, 0.058, "house")
        box(p, c + V(0, -0.046, 0), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), 0.062, 0.003, 0.046, "lens")
        bar(p, V(x, yb, zbar), V(x, yb - 0.02, zbar + 0.03), 0.02, "steel")
    PARTS.append(p)
    print("LIGHTBAR roof front y", yf, "bar y", round(yb, 3), "roof z", round(zr, 3))


# ---------------------------------------------------------------- 4. решётка на переднем крыле
def surface_patch(names, s, y0, y1, z0, z1, nu, nv):
    grid = []
    for i in range(nu + 1):
        row = []
        for j in range(nv + 1):
            y = y0 + (y1 - y0) * i / nu
            z = z0 + (z1 - z0) * j / nv
            h = side_hit(names, y, z, s, 0.55)
            if not h:
                return None
            row.append(h)
        grid.append(row)
    return grid


def vents():
    for s in (1, -1):
        g = surface_patch(["body"], s, -0.80, -0.68, 0.80, 0.94, 4, 4)
        if not g:
            print("WARN нет поверхности под решётку", s)
            continue
        p = Part("vent_%d" % s, "body", [1, 2, 3], ["er_vent"])

        def P(i, j, off):
            loc, n = g[i][j]
            return loc + n * off
        # подложка (чёрная) и рамка
        bm_rings = [[p.bm.verts.new(P(i, j, 0.002)) for j in range(5)] for i in range(5)]
        for i in range(4):
            for j in range(4):
                p.face((bm_rings[i][j], bm_rings[i + 1][j], bm_rings[i + 1][j + 1], bm_rings[i][j + 1]) if s > 0 else
                       (bm_rings[i][j], bm_rings[i][j + 1], bm_rings[i + 1][j + 1], bm_rings[i + 1][j]), "vent")
        corners = [P(0, 0, 0.008), P(4, 0, 0.008), P(4, 4, 0.008), P(0, 4, 0.008)]
        for a, b in zip(corners, corners[1:] + corners[:1]):
            bar(p, a, b, 0.016, "steel")
        for k in range(1, 6):
            t = k / 6
            a = P(0, 0, 0.006).lerp(P(0, 4, 0.006), t)
            b = P(4, 0, 0.006).lerp(P(4, 4, 0.006), t)
            bar(p, a, b, 0.008, "steel")
        for (i, j) in ((0, 0), (4, 0), (4, 4), (0, 4)):
            loc, n = g[i][j]
            cyl(p, loc + n * 0.012, n, 0.006, 0.006, 6, "bolt")
        PARTS.append(p)


# ---------------------------------------------------------------- 5. накладки с заклёпками
def plate(name, target, names, s, y0, y1, z0, z1, lods):
    nu, nv = 10, 6
    g = surface_patch(names, s, y0, y1, z0, z1, nu, nv)
    if not g:
        print("WARN нет поверхности под накладку", name)
        return
    p = Part(name, target, lods, ["er_plates"])
    bm = p.bm
    top = [[bm.verts.new(g[i][j][0] + g[i][j][1] * 0.007) for j in range(nv + 1)] for i in range(nu + 1)]
    bot = [[bm.verts.new(g[i][j][0] + g[i][j][1] * 0.0015) for j in range(nv + 1)] for i in range(nu + 1)]
    for i in range(nu):
        for j in range(nv):
            p.face((top[i][j], top[i + 1][j], top[i + 1][j + 1], top[i][j + 1]), "plate")
            p.face((bot[i][j], bot[i][j + 1], bot[i + 1][j + 1], bot[i + 1][j]), "plate")
    border = [(i, 0) for i in range(nu)] + [(nu, j) for j in range(nv)] + \
             [(i, nv) for i in range(nu, 0, -1)] + [(0, j) for j in range(nv, 0, -1)]
    for k in range(len(border)):
        a, b = border[k], border[(k + 1) % len(border)]
        p.face((top[a[0]][a[1]], bot[a[0]][a[1]], bot[b[0]][b[1]], top[b[0]][b[1]]), "plate")
    r = Part(name + "_rivets", target, lods[:2], ["er_rivets"])
    for (i, j) in border:
        for sub in (0.0, 0.5):
            ii, jj = i, j
            loc, n = g[ii][jj]
            # отступ к центру накладки
            cen = g[nu // 2][nv // 2][0]
            q = loc + (cen - loc).normalized() * 0.022 if sub == 0.0 else None
            if q is None:
                continue
            cyl(r, q + n * 0.006, n, 0.0065, 0.005, 6, "bolt")
    PARTS.append(p)
    PARTS.append(r)


def plates():
    for s in (1, -1):
        tgt = "door_driver" if s > 0 else "door_codriver"
        plate("plate_door_%d" % s, tgt, [DOOR[s]], s, -0.30, 0.52, 0.60, 0.98, [1, 2, 3, 4])
        plate("plate_quarter_%d" % s, "body", ["body"], s, 0.88, 1.26, 0.76, 1.04, [1, 2, 3])


# ---------------------------------------------------------------- MLOD
def remove_spare(m):
    """Запаски у Echo Runner нет (решение пользователя 2026-11-17): прокси, выделение слота и цепь ALEX — долой."""
    SPARE_BOX = ((-0.70, 0.0), (1.35, 1.72), (1.20, 1.95))     # оси DayZ: запаска лежала на баках
    for lod in m.lods:
        if C.is_res(lod, 6e15) or C.is_res(lod, 7e15):           # Fire / View Geometry: компонент запаски
            for n, sel in list(lod.selections.items()):
                pts = [p.coords for p in sel.points]
                if n.lower().startswith("component") and pts and all(
                        SPARE_BOX[i][0] <= min(c[i] for c in pts) and max(c[i] for c in pts) <= SPARE_BOX[i][1]
                        for i in range(3)):
                    k = M.prune_selection(lod, [n])
                    print("  запаска: LOD %g %s граней %d" % (lod.resolution, n, k))
            continue
        if lod.resolution > 1e4 and not C.is_res(lod, 1e15):
            continue
        names = [n for n in lod.selections if n.lower().endswith("interceptor_wheel.005")] + ["wheel_spare_1"]
        names += [n for n in lod.selections if "55galdrum" in n.lower()]          # места ALEX под бочки
        if lod.resolution < 1e4:
            names.append("lancuch")
        k = M.prune_selection(lod, names)
        if k:
            print("  запаска/цепь: LOD %g граней %d" % (lod.resolution, k))


def main():
    flares()
    window_nets()
    lightbar()
    vents()
    plates()
    ctx = type("ctx", (), {})()
    ctx.PARTS, ctx.DOOR, ctx.hit = PARTS, DOOR, hit
    er_interior.build(ctx)
    os.makedirs(os.path.join(OUT, "proxy"), exist_ok=True)
    car = C.load_p3d(os.path.join(WD, "interceptor.p3d"))
    doors = {"door_driver": "interceptor_doors_driver", "door_codriver": "interceptor_doors_codriver"}
    dm = {k: C.load_p3d(os.path.join(WD, "proxy", v + ".p3d")) for k, v in doors.items()}
    inv = {}
    for k, v in doors.items():
        nm = [n for n in XF if n.startswith(v)][0]
        a, A0, A1, A2 = XF[nm]
        inv[k] = (np.array(a), np.stack([A0, A1, A2]))     # c = M @ (p - a)
    for part in PARTS:
        if not part.bm.faces:
            continue
        model = car if part.target == "body" else dm[part.target]
        data = part_data(part, None if part.target == "body" else inv[part.target])
        for res in part.lods:
            lod = [l for l in model.lods if C.is_res(l, res)]
            if lod:
                append(lod[0], data, part.sels)
        print("PART %-22s -> %-13s LOD %s граней %d" % (part.name, part.target, part.lods, len(data[1])))
    remove_spare(car)
    car.save(os.path.join(OUT, "interceptor.p3d"))
    for k, v in doors.items():
        dm[k].save(os.path.join(OUT, "proxy", v + ".p3d"))
    for n in ("interceptor_wheel", "interceptor_doors_hood"):
        shutil.copy2(os.path.join(WD, "proxy", n + ".p3d"), os.path.join(OUT, "proxy", n + ".p3d"))
    print("OK", OUT)


main()
