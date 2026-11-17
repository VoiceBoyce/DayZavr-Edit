# -*- coding: utf-8 -*-
r"""Echo Runner: процедурные текстуры (numpy, внутри Blender 5.2).

Атлас кузова 2048×2048 (корпус, двери, крышки — одна развёртка, проекции в мировых координатах, см.
er_model.ATLAS_*), плитки 512×512 для деталей. На каждую текстуру: _co (цвет, sRGB), _nohq (нормали DirectX:
зелёный инвертирован относительно OpenGL), _smdi (R=1, G — блик, B — глянец).
Для предпросмотра в Blender отдельно отдаются нормали OpenGL, металличность и шероховатость.

Массивы — float32, строка 0 = верх картинки (v = 1).
"""
import math
import os
import struct

import numpy as np

import er_model as M

RES = 2048
S = RES / M.ATLAS_A                        # пикселей на метр
ISL_ORDER = ["T", "L", "R", "F", "B", "D"]


# ------------------------------------------------------------------ шум
def vnoise(h, w, cy, cx, rng):
    """Сглаженный value-noise, периодический (тайлится). cy/cx — число ячеек."""
    cy, cx = max(1, int(cy)), max(1, int(cx))
    g = rng.random((cy, cx), dtype=np.float32)
    ys = (np.arange(h, dtype=np.float32) + 0.5) * cy / h
    xs = (np.arange(w, dtype=np.float32) + 0.5) * cx / w
    y0 = np.floor(ys).astype(np.int64)
    x0 = np.floor(xs).astype(np.int64)
    fy = ys - y0
    fx = xs - x0
    fy = fy * fy * (3 - 2 * fy)
    fx = fx * fx * (3 - 2 * fx)
    y1, x1 = (y0 + 1) % cy, (x0 + 1) % cx
    y0, x0 = y0 % cy, x0 % cx
    a, b = g[y0][:, x0], g[y0][:, x1]
    c, d = g[y1][:, x0], g[y1][:, x1]
    top = a + (b - a) * fx[None, :]
    bot = c + (d - c) * fx[None, :]
    return top + (bot - top) * fy[:, None]


def fbm(h, w, cy, cx, rng, octaves=4, gain=0.5):
    out = np.zeros((h, w), np.float32)
    amp, tot = 1.0, 0.0
    for o in range(octaves):
        out += vnoise(h, w, min(cy * 2 ** o, h), min(cx * 2 ** o, w), rng) * amp
        tot += amp
        amp *= gain
    return out / tot


def smooth(e0, e1, x):
    t = np.clip((x - e0) / (e1 - e0), 0.0, 1.0)
    return t * t * (3 - 2 * t)


def mix(a, b, t):
    """Смешивание цветов (последняя ось — RGB); маска 2D расширяется до (H, W, 1)."""
    t = np.asarray(t, np.float32)
    if t.ndim == 2:
        t = t[..., None]
    return a + (b - a) * t


def rgb(c):
    return np.array(c, np.float32)


# ------------------------------------------------------------------ карты из высоты
def normals_from_height(hgt, px_per_m, strength=1.0):
    gy, gx = np.gradient(hgt.astype(np.float32))
    nx = -gx * px_per_m * strength
    ny_gl = gy * px_per_m * strength          # строка вниз = -v, поэтому знак «+»
    nz = np.ones_like(nx)
    ln = np.sqrt(nx * nx + ny_gl * ny_gl + nz * nz)
    gl = np.stack([nx / ln, ny_gl / ln, nz / ln], -1) * 0.5 + 0.5
    dx = gl.copy()
    dx[..., 1] = 1.0 - dx[..., 1]             # DirectX (DayZ _nohq): зелёный инвертирован
    return gl, dx


def pack(col, hgt, spec, gloss, metal, px_per_m, nstrength=1.0, alpha=None):
    gl, dx = normals_from_height(hgt, px_per_m, nstrength)
    smdi = np.stack([np.ones_like(spec), np.clip(spec, 0, 1), np.clip(gloss, 0, 1)], -1)
    return dict(co=np.clip(col, 0, 1), alpha=alpha, nohq=dx, nohq_gl=gl, smdi=smdi,
                rough=np.clip(1.0 - gloss * 0.92, 0.04, 1.0), metal=np.clip(metal, 0, 1))


# ------------------------------------------------------------------ атлас кузова
def atlas_coords(margin_px=8):
    """Мировые координаты каждого текселя атласа: X, Y, Z и номер островка (−1 — пусто)."""
    X = np.zeros((RES, RES), np.float32)
    Y = np.zeros((RES, RES), np.float32)
    Z = np.zeros((RES, RES), np.float32)
    I = np.full((RES, RES), -1, np.int8)
    for code, name in enumerate(ISL_ORDER):
        u0, v0, w, h = M.ATLAS_ISL[name]
        c0 = max(0, int(u0 * S) - margin_px)
        c1 = min(RES, int(math.ceil((u0 + w) * S)) + margin_px)
        r0 = max(0, int(v0 * S) - margin_px)
        r1 = min(RES, int(math.ceil((v0 + h) * S)) + margin_px)
        a = (np.arange(c0, c1, dtype=np.float32) + 0.5) / S - u0
        b = (np.arange(r0, r1, dtype=np.float32) + 0.5) / S - v0
        aa, bb = np.meshgrid(a, b)
        x, y, z = M.atlas_world(name, aa, bb)
        X[r0:r1, c0:c1], Y[r0:r1, c0:c1], Z[r0:r1, c0:c1] = x, y, z
        I[r0:r1, c0:c1] = code
    return X, Y, Z, I


def rect_sd(p, q, c, h):
    return np.maximum(np.abs(p - c[0]) - h[0], np.abs(q - c[1]) - h[1])


def rot_rect_mask(p, q, c, h, ang_deg):
    a = math.radians(ang_deg)
    dp, dq = p - c[0], q - c[1]
    lp = dp * math.cos(a) + dq * math.sin(a)
    lq = -dp * math.sin(a) + dq * math.cos(a)
    return rect_sd(lp, lq, (0, 0), h)


def rivets_along_rect(p, q, c, h, inset, step, r):
    """Высота заклёпок по периметру прямоугольника (купол радиуса r)."""
    hgt = np.zeros_like(p)
    x0, x1 = c[0] - h[0] + inset, c[0] + h[0] - inset
    y0, y1 = c[1] - h[1] + inset, c[1] + h[1] - inset
    best = np.full_like(p, 9.0)
    inside_x = (p >= x0 - r) & (p <= x1 + r)
    inside_y = (q >= y0 - r) & (q <= y1 + r)
    for qe in (y0, y1):                                     # верх и низ
        t = np.mod(p - x0 + step / 2, step) - step / 2
        d = np.sqrt(t * t + (q - qe) ** 2)
        best = np.where(inside_x, np.minimum(best, d), best)
    for pe in (x0, x1):                                     # бока
        t = np.mod(q - y0 + step / 2, step) - step / 2
        d = np.sqrt(t * t + (p - pe) ** 2)
        best = np.where(inside_y, np.minimum(best, d), best)
    dome = np.clip(1 - (best / r) ** 2, 0, 1)
    return dome, best


def draw_lines(shape, segs, width=1.0):
    """Маска тонких линий (царапины)."""
    m = np.zeros(shape, np.float32)
    h, w = shape
    for (r0, c0, r1, c1, val) in segs:
        n = int(max(abs(r1 - r0), abs(c1 - c0)) * 2) + 2
        rr = np.linspace(r0, r1, n)
        cc = np.linspace(c0, c1, n)
        for o in ([0.0] if width <= 1 else [-0.5, 0.5]):
            ri = np.clip(np.round(rr + o).astype(int), 0, h - 1)
            ci = np.clip(np.round(cc).astype(int), 0, w - 1)
            np.maximum.at(m, (ri, ci), val)
    return m


# ------------------------------------------------------------------ надписи (трафарет / краска из баллона)
GLYPHS = {   # штрихи на сетке 4×6 (x вправо, y вверх)
    "E": [((0, 0), (0, 6)), ((0, 6), (4, 6)), ((0, 3), (3, 3)), ((0, 0), (4, 0))],
    "C": [((4, 6), (0, 6)), ((0, 6), (0, 0)), ((0, 0), (4, 0))],
    "H": [((0, 0), (0, 6)), ((4, 0), (4, 6)), ((0, 3), (4, 3))],
    "O": [((0, 0), (4, 0)), ((4, 0), (4, 6)), ((4, 6), (0, 6)), ((0, 6), (0, 0))],
    "R": [((0, 0), (0, 6)), ((0, 6), (4, 6)), ((4, 6), (4, 3)), ((4, 3), (0, 3)), ((1.5, 3), (4, 0))],
    "U": [((0, 6), (0, 0)), ((0, 0), (4, 0)), ((4, 0), (4, 6))],
    "N": [((0, 0), (0, 6)), ((0, 6), (4, 0)), ((4, 0), (4, 6))],
    "1": [((2.2, 0), (2.2, 6)), ((2.2, 6), (0.8, 4.8)), ((0.8, 0), (3.6, 0))],
    "6": [((4, 6), (0, 6)), ((0, 6), (0, 0)), ((0, 0), (4, 0)), ((4, 0), (4, 3)), ((4, 3), (0, 3))],
    "-": [((0.8, 3), (3.2, 3))],
}


def text_segs(s, origin, d, e, h):
    """Штрихи строки в мировых 2D-координатах островка. d — направление строки, e — «вверх» букв."""
    k = h / 6.0
    o, d, e = np.array(origin, float), np.array(d, float), np.array(e, float)
    segs, x = [], 0.0
    for ch in s:
        if ch != " ":
            for a, b in GLYPHS[ch]:
                pa = o + d * (x + a[0] * k) + e * (a[1] * k)
                pb = o + d * (x + b[0] * k) + e * (b[1] * k)
                segs.append((tuple(pa), tuple(pb)))
        x += 6.0 * k
    return segs


def circle_segs(c, r, n, rng, wob=0.012):
    pts = []
    for i in range(n + 1):
        t = 2 * math.pi * i / n
        rr = r + wob * math.sin(3 * t + 0.7) + rng.normal(0, wob * 0.3)
        pts.append((c[0] + rr * math.cos(t), c[1] + rr * math.sin(t)))
    return list(zip(pts[:-1], pts[1:]))


def seg_dist(P, Q, segs):
    best = np.full(P.shape, 9.0, np.float32)
    for (ax, ay), (bx, by) in segs:
        vx, vy = bx - ax, by - ay
        l2 = vx * vx + vy * vy + 1e-12
        t = np.clip(((P - ax) * vx + (Q - ay) * vy) / l2, 0, 1)
        dx, dy = P - (ax + t * vx), Q - (ay + t * vy)
        best = np.minimum(best, np.sqrt(dx * dx + dy * dy))
    return best


def spray(maps, isl, P, Q, segs, width, color, wear_map, wear=0.30, halo=0.007, drips=0, rng=None, down=None):
    """Краска по штрихам: ядро, напыление вокруг, потёртость, потёки вниз (down — вектор «вниз» в P/Q)."""
    col, hgt, gloss, spec, metal = maps
    allp = [p for s in segs for p in s]
    pmin = np.min(allp, 0) - width - halo - 0.08
    pmax = np.max(allp, 0) + width + halo
    sel = isl & (P > pmin[0]) & (P < pmax[0]) & (Q > pmin[1]) & (Q < pmax[1])
    idx = np.nonzero(sel)
    if not idx[0].size:
        return
    p, q = P[idx], Q[idx]
    if drips and rng is not None and down is not None:
        extra = []
        for _ in range(drips):
            (ax, ay), (bx, by) = segs[rng.integers(len(segs))]
            t = rng.random()
            sx, sy = ax + (bx - ax) * t, ay + (by - ay) * t
            ln = 0.015 + rng.random() * 0.05
            extra.append(((sx, sy), (sx + down[0] * ln, sy + down[1] * ln)))
        dd = seg_dist(p, q, extra)
        dcore = smooth(0.0028, 0.0012, dd)
    else:
        dcore = 0.0
    d = seg_dist(p, q, segs)
    core = smooth(width * 0.5 + 0.0012, width * 0.5 - 0.0006, d)
    hal = smooth(width * 0.5 + halo, width * 0.5, d) * 0.22
    wn = wear_map[idx]
    a = np.maximum(np.maximum(core, dcore) * smooth(wear - 0.08, wear + 0.08, wn), hal)
    c = rgb(color)
    col[idx] = col[idx] + (c - col[idx]) * a[:, None]
    gloss[idx] = gloss[idx] + (0.28 - gloss[idx]) * a
    spec[idx] = spec[idx] + (0.2 - spec[idx]) * a
    metal[idx] = metal[idx] * (1 - a)
    hgt[idx] = hgt[idx] + 0.00006 * np.maximum(core, dcore)


def paint_body(seed=1607):
    rng = np.random.default_rng(seed)
    X, Y, Z, I = atlas_coords()
    H = W = RES
    T, Lf, Rt, Fr, Bk, Dk = (I == k for k in range(6))
    side = Lf | Rt
    ends = Fr | Bk

    # --- нержавейка, шлифованная вдоль машины
    fine = vnoise(H, W, 900, 5, rng)
    med = vnoise(H, W, 220, 22, rng)
    mott = fbm(H, W, 6, 6, rng, 4)
    lum = 0.63 + 0.05 * (fine - 0.5) + 0.03 * (med - 0.5) + 0.07 * (mott - 0.5)
    col = lum[..., None] * rgb((1.0, 1.012, 1.035))
    spec = 0.55 + 0.12 * (med - 0.5)
    gloss = 0.60 + 0.14 * (fine - 0.5) - 0.12 * (mott - 0.5)
    metal = np.ones((H, W), np.float32)
    hgt = 0.00004 * (fine - 0.5)

    # --- низ (порог, днище, арки): тёмный, шершавый
    low = (side & (Z < 0.30)) | Dk
    rough_n = fbm(H, W, 96, 96, rng, 3)
    blk = (0.07 + 0.05 * rough_n)[..., None] * rgb((1.0, 0.98, 0.95))
    col = np.where(low[..., None], blk, col)
    spec = np.where(low, 0.08, spec)
    gloss = np.where(low, 0.12 + 0.08 * rough_n, gloss)
    metal = np.where(low, 0.0, metal)
    hgt = hgt + np.where(low, 0.0004 * (rough_n - 0.5), 0)

    # --- жалюзи на задней крышке
    lv = T & (Y > 0.80) & (Y < 1.68) & (np.abs(X) < 0.82)
    ph = np.mod(Y - 0.80, 0.04) / 0.04
    lvc = np.where(ph < 0.14, 0.22, 0.07 + 0.03 * ph)[..., None] * rgb((1, 1, 1.02))
    col = np.where(lv[..., None], lvc, col)
    gloss = np.where(lv, 0.30, gloss)
    spec = np.where(lv, 0.25, spec)
    metal = np.where(lv, 0.4, metal)
    hgt = np.where(lv, 0.004 * ph, hgt)

    # --- капот в грунте (с другой машины)
    hood = T & (np.abs(X) < 0.72) & (Y > -2.05) & (Y < -0.66)
    pn = fbm(H, W, 48, 48, rng, 4)
    primer = (0.43 + 0.07 * (pn - 0.5))[..., None] * rgb((1.0, 1.01, 1.02))
    over = smooth(0.55, 0.75, fbm(H, W, 10, 14, rng, 3))           # пятна другой краски
    primer = mix(primer, rgb((0.30, 0.33, 0.31)), over * 0.6)
    edge_rust = smooth(-1.88, -2.04, Y) * smooth(0.5, 0.75, fbm(H, W, 64, 64, rng, 3))
    primer = mix(primer, rgb((0.36, 0.17, 0.08)), edge_rust)
    col = np.where(hood[..., None], primer, col)
    gloss = np.where(hood, 0.14 + 0.05 * pn, gloss)
    spec = np.where(hood, 0.12, spec)
    metal = np.where(hood, 0.0, metal)
    hgt = hgt + np.where(hood, 0.0002 * (pn - 0.5), 0)

    # --- правая дверь: красная, выцветшая, со сколами (донор)
    red = (Rt & (Y > -0.55) & (Y < 0.45) & (Z > 0.30)) | (T & (X > 0.16) & (Y > -0.55) & (Y < 0.45))
    fade = fbm(H, W, 8, 8, rng, 4)
    rc = mix(rgb((0.50, 0.11, 0.085)), rgb((0.66, 0.32, 0.26)), smooth(0.35, 0.75, fade) * 0.75)
    rc = mix(rc, rgb((0.70, 0.40, 0.34)), (T & red)[..., None].astype(np.float32) * 0.35)   # крыша выгорела сильнее
    chip_n = fbm(H, W, 110, 110, rng, 3)
    chip = smooth(0.66, 0.70, chip_n)
    chip_rust = smooth(0.74, 0.78, chip_n)
    rc = mix(rc, rgb((0.50, 0.51, 0.52)), chip)
    rc = mix(rc, rgb((0.33, 0.15, 0.07)), chip_rust)
    col = np.where(red[..., None], rc, col)
    gloss = np.where(red, 0.32 - 0.18 * chip, gloss)
    spec = np.where(red, 0.25, spec)
    metal = np.where(red, 0.0, metal)
    hgt = hgt - np.where(red, 0.00025 * chip, 0)

    # --- левое переднее крыло: ржавый лист на сварке
    rc_c, rc_h = (-1.45, 0.53), (0.56, 0.20)
    sd = rect_sd(Y, Z, rc_c, rc_h)
    rust_in = Lf & (sd < 0)
    rn = fbm(H, W, 70, 70, rng, 5)
    pits = smooth(0.70, 0.78, vnoise(H, W, 400, 400, rng))
    rcol = mix(rgb((0.47, 0.22, 0.09)), rgb((0.20, 0.09, 0.045)), rn)
    rcol = mix(rcol, rgb((0.12, 0.06, 0.03)), pits * 0.7)
    col = np.where(rust_in[..., None], rcol, col)
    gloss = np.where(rust_in, 0.06 + 0.05 * rn, gloss)
    spec = np.where(rust_in, 0.05, spec)
    metal = np.where(rust_in, 0.1, metal)
    hgt = hgt + np.where(rust_in, 0.0006 * (rn - 0.5) - 0.0004 * pits, 0)
    weld = Lf & (np.abs(sd) < 0.0065)
    ripple = vnoise(H, W, 20, 700, rng)
    bead = np.clip(1 - (sd / 0.0065) ** 2, 0, 1) * (0.8 + 0.4 * ripple)
    col = np.where(weld[..., None], mix(rgb((0.28, 0.25, 0.23)), rgb((0.16, 0.14, 0.13)), ripple), col)
    hgt = hgt + np.where(weld, 0.0016 * bead, 0)
    heat = Lf & (sd > 0) & (sd < 0.03)                                 # цвета побежалости у шва
    col = np.where(heat[..., None], mix(col, rgb((0.38, 0.34, 0.45)), (1 - sd / 0.03)[..., None] * 0.35), col)
    bleed = Lf & (Z < rc_c[1] - rc_h[1]) & (Z > 0.30) & (np.abs(Y - rc_c[0]) < rc_h[0]) \
        & (vnoise(H, W, 4, 260, rng) > 0.62)
    fall = smooth(rc_c[1] - rc_h[1] - 0.18, rc_c[1] - rc_h[1], Z)
    col = np.where(bleed[..., None], mix(col, rgb((0.40, 0.19, 0.08)), fall[..., None] * 0.6), col)

    # --- правая задняя четверть: лист в грунте на заклёпках
    pc_c, pc_h = (1.38, 0.585), (0.55, 0.235)
    sdp = rect_sd(Y, Z, pc_c, pc_h)
    pr_in = Rt & (sdp < 0)
    pn2 = fbm(H, W, 40, 40, rng, 4)
    pcol = (0.45 + 0.06 * (pn2 - 0.5))[..., None] * rgb((1.0, 1.0, 1.01))
    col = np.where(pr_in[..., None], pcol, col)
    gloss = np.where(pr_in, 0.15, gloss)
    spec = np.where(pr_in, 0.1, spec)
    metal = np.where(pr_in, 0.0, metal)
    hgt = hgt + np.where(pr_in, 0.0012, 0)                             # лист поверх кузова
    edge_line = Rt & (np.abs(sdp) < 0.003)
    col = np.where(edge_line[..., None], col * 0.55, col)
    dome, dist = rivets_along_rect(Y, Z, pc_c, pc_h, 0.022, 0.055, 0.0065)
    rv = Rt & (dome > 0)
    col = np.where(rv[..., None], mix(col, rgb((0.55, 0.55, 0.56)), dome[..., None] * 0.8), col)
    hgt = hgt + np.where(Rt, 0.0016 * dome, 0)
    streak = Rt & (sdp < 0) & (np.mod(Y - pc_c[0] + pc_h[0] - 0.022 + 0.0275, 0.055) < 0.004) \
        & (vnoise(H, W, 30, 600, rng) > 0.45)
    col = np.where(streak[..., None], mix(col, rgb((0.42, 0.20, 0.09)), 0.55), col)  # рыжие потёки от заклёпок

    # --- скотч: крест на трещине правого переднего крыла, полоса на левой задней четверти
    crack = Rt & (np.abs((Z - 0.58) - 0.35 * (Y + 1.55)) < 0.0025) & (np.abs(Y + 1.55) < 0.11)
    col = np.where(crack[..., None], col * 0.25, col)
    tape = np.zeros((H, W), bool)
    for isl, cc, hh, ang in ((Rt, (-1.55, 0.58), (0.10, 0.024), 35), (Rt, (-1.55, 0.58), (0.10, 0.024), -35),
                             (Lf, (1.25, 0.55), (0.085, 0.022), 80)):
        tape |= isl & (rot_rect_mask(Y, Z, cc, hh, ang) < 0)
    crum = fbm(H, W, 300, 300, rng, 3)
    col = np.where(tape[..., None], (0.58 + 0.10 * (crum - 0.5))[..., None] * rgb((1.0, 1.01, 1.03)), col)
    gloss = np.where(tape, 0.45, gloss)
    spec = np.where(tape, 0.35, spec)
    metal = np.where(tape, 0.0, metal)
    hgt = hgt + np.where(tape, 0.0004 + 0.0003 * (crum - 0.5), 0)

    # --- щели панелей точно по линиям разреза дверей и крышек (в модели они заподлицо)
    D_, Hd, El = M.DOOR, M.HOOD, M.ELID
    gaps = []                                                       # (маска островка, P, p0, Q, q0, q1)
    for isl in (Lf, Rt):
        gaps += [(isl, Y, D_["y0"], Z, D_["z0"], 1.16), (isl, Y, D_["y1"], Z, D_["z0"], 1.16),
                 (isl, Z, D_["z0"], Y, D_["y0"], D_["y1"])]
    for s in (1, -1):
        gaps += [(T, X, s * D_["x_hinge"], Y, -0.10, D_["y1"]),
                 (T, X, s * Hd["x"], Y, Hd["y0"], Hd["y1"]), (T, X, s * El["x"], Y, El["y0"], El["y1"])]
    gaps += [(T & (X > D_["x_hinge"]), Y, D_["y1"], X, D_["x_hinge"], 1.0),
             (T & (X < -D_["x_hinge"]), Y, D_["y1"], X, -1.0, -D_["x_hinge"]),
             (T, Y, Hd["y0"], X, -Hd["x"], Hd["x"]), (T, Y, Hd["y1"], X, -Hd["x"], Hd["x"]),
             (T, Y, El["y0"], X, -El["x"], El["x"]), (T, Y, El["y1"], X, -El["x"], El["x"])]
    shade = np.ones((H, W), np.float32)
    for isl, P_, p0, Q_, q0, q1 in gaps:
        inr = isl & (Q_ > q0 - 0.004) & (Q_ < q1 + 0.004)
        dist = np.abs(P_ - p0)
        shade = np.where(inr, np.minimum(shade, 0.22 + 0.78 * smooth(0.0016, 0.004, dist)
                                         * (0.80 + 0.20 * smooth(0.004, 0.016, dist))), shade)
        hgt = hgt - np.where(inr, 0.0018 * smooth(0.004, 0.0015, dist), 0)
    col = col * shade[..., None]
    gloss = gloss * (0.4 + 0.6 * shade)

    # --- рёбра на задней панели (между фонарями и бампером)
    rib = Bk & (np.abs(X) < 0.80) & (Z > 0.44) & (Z < 0.86)
    rph = 0.5 + 0.5 * np.sin(2 * math.pi * Z / 0.022)
    hgt = hgt + np.where(rib, 0.0012 * smooth(0.35, 0.65, rph), 0)
    col = np.where(rib[..., None], col * (0.80 + 0.20 * rph)[..., None], col)

    # --- «зебра» на носу под фарами
    zeb = Fr & (np.abs(X) < 0.62) & (Z > 0.335) & (Z < 0.445)
    stripe = np.mod(X - Z * 1.0 + 1.0, 0.09) < 0.045
    wear_z = smooth(0.30, 0.42, fbm(H, W, 90, 90, rng, 3))
    zc = np.where(stripe[..., None], rgb((0.80, 0.62, 0.06)), rgb((0.05, 0.05, 0.05)))
    za = (zeb * wear_z)[..., None]
    col = col + (zc - col) * za
    gloss = gloss + (0.3 - gloss) * za[..., 0]
    metal = metal * (1 - za[..., 0])

    # --- надписи краской
    wear = fbm(H, W, 160, 160, rng, 3)
    maps = [col, hgt, gloss, spec, metal]
    white = (0.86, 0.86, 0.84)
    # «16» в круге на левой двери (левый борт смотрят снаружи — строка идёт к носу, то есть в −Y)
    c16 = (-0.06, 0.56)
    spray(maps, Lf, Y, Z, circle_segs(c16, 0.175, 40, rng), 0.022, white, wear, 0.26, drips=0)
    spray(maps, Lf, Y, Z, text_segs("16", (c16[0] + 0.125, c16[1] - 0.075), (-1, 0), (0, 1), 0.15), 0.026, white,
          wear, 0.24, drips=6, rng=rng, down=(0, -1))
    # зарубки у водителя: три пятёрки на красной двери (левый борт машины = правый островок атласа R, x>0)
    for gi in range(3):
        y0 = 0.17 + gi * 0.07
        tl = [((y0 + k * 0.011, 0.70), (y0 + k * 0.011, 0.78)) for k in range(4)]
        tl.append(((y0 - 0.006, 0.715), (y0 + 0.044, 0.765)))
        spray(maps, Rt, Y, Z, tl, 0.006, white, wear, 0.20, halo=0.003)
    # трафарет на капоте (в грунте): читается спереди, строка вдоль +X, «вверх» к стеклу (+Y)
    hood_txt = text_segs("ECHO RUNNER", (-0.453, -1.72), (1, 0), (0, 1), 0.085)
    spray(maps, T, X, Y, hood_txt, 0.014, (0.07, 0.07, 0.07), wear, 0.34, halo=0.005)
    # сзади над номером: «ER-16»
    spray(maps, Bk, X, Z, text_segs("ER-16", (0.14, 0.455), (-1, 0), (0, 1), 0.06), 0.011, white, wear, 0.3,
          halo=0.004, drips=4, rng=rng, down=(0, -1))
    col, hgt, gloss, spec, metal = maps

    # --- грязь снизу, брызги, иней Сахала, дождевые потёки, пыль сверху
    dirt_t = (np.clip((0.62 - Z) / 0.42, 0, 1) ** 1.4) * (side | ends)
    dn = fbm(H, W, 30, 60, rng, 4)
    col = mix(col, rgb((0.27, 0.24, 0.20)), (dirt_t * (0.35 + 0.45 * dn))[..., None] * 0.8)
    gloss = gloss * (1 - 0.6 * dirt_t)
    zone = (side | ends).astype(np.float32)
    splat = zone * smooth(0.48, 0.30, Z) * smooth(0.80, 0.90, vnoise(H, W, 220, 220, rng))
    col = mix(col, rgb((0.20, 0.17, 0.14)), splat * 0.6)
    frost = zone * smooth(0.40, 0.31, Z) * (Z > 0.30) * smooth(0.84, 0.94, fbm(H, W, 260, 260, rng, 2))
    col = mix(col, rgb((0.80, 0.84, 0.88)), frost * 0.45)
    rain = side * (Z < 0.95) * (Z > 0.32) * smooth(0.72, 0.85, vnoise(H, W, 4, 420, rng)) \
        * smooth(0.3, 0.7, fbm(H, W, 6, 30, rng, 2))
    col = col * (1.0 - 0.07 * rain)[..., None]
    dust = T & ~lv
    col = np.where(dust[..., None], mix(col, rgb((0.55, 0.52, 0.48)), (smooth(0.45, 0.8, fbm(H, W, 12, 12, rng, 3))
                                                                        * 0.22)[..., None]), col)

    # --- царапины
    segs = []
    for _ in range(260):
        code = rng.integers(0, 5)
        u0, v0, w, h = M.ATLAS_ISL[ISL_ORDER[code]]
        r0 = (v0 + rng.random() * h) * S
        c0 = (u0 + rng.random() * w) * S
        ang = rng.normal(0, 0.35) if rng.random() < 0.7 else rng.random() * math.pi
        ln = 15 + rng.random() * 110
        segs.append((r0, c0, r0 + math.sin(ang) * ln, c0 + math.cos(ang) * ln, 0.5 + 0.5 * rng.random()))
    scr = draw_lines((H, W), segs) * (I >= 0) * (~low)
    col = mix(col, rgb((0.80, 0.80, 0.82)), (scr * 0.45)[..., None])
    hgt = hgt - 0.00015 * scr

    empty = I < 0
    col = np.where(empty[..., None], rgb((0.55, 0.56, 0.57)), col)
    return pack(col, hgt, spec, gloss, metal, S, nstrength=1.0)


# ------------------------------------------------------------------ плитки 512
def tile(kind, seed, n=512, m_per_tile=0.6):
    rng = np.random.default_rng(seed)
    ppm = n / m_per_tile
    f1 = fbm(n, n, 8, 8, rng, 5)
    f2 = fbm(n, n, 64, 64, rng, 3)
    f3 = vnoise(n, n, 256, 256, rng)
    hgt = 0.0003 * (f2 - 0.5)
    alpha = None
    if kind == "metal_rust":
        base = mix(rgb((0.30, 0.27, 0.25)), rgb((0.45, 0.22, 0.10)), smooth(0.45, 0.7, f1)[..., None])
        col = mix(base, rgb((0.18, 0.10, 0.06)), (smooth(0.6, 0.8, f2) * 0.6)[..., None])
        gloss, spec, metal = 0.18 + 0.12 * (1 - f1), 0.2 + 0 * f1, 0.5 * (1 - smooth(0.45, 0.7, f1))
        hgt = 0.0006 * (f2 - 0.5) - 0.0003 * smooth(0.7, 0.8, f3)
    elif kind == "copper":
        col = mix(rgb((0.72, 0.42, 0.27)), rgb((0.30, 0.38, 0.30)), (smooth(0.55, 0.8, f1) * 0.8)[..., None])
        col = mix(col, rgb((0.35, 0.20, 0.12)), (smooth(0.6, 0.8, f2) * 0.5)[..., None])
        gloss, spec, metal = 0.45 - 0.25 * f1, 0.5 + 0 * f1, 0.9 - 0.5 * smooth(0.55, 0.8, f1)
    elif kind == "rubber":
        col = (0.055 + 0.03 * f2)[..., None] * rgb((1, 1, 1.03))
        gloss, spec, metal = 0.12 + 0.05 * f3, 0.08 + 0 * f1, 0 * f1
        hgt = 0.0002 * (f3 - 0.5)
    elif kind == "plastic":
        col = (0.075 + 0.03 * f1 + 0.02 * (f3 - 0.5))[..., None] * rgb((1, 1, 1.05))
        col = mix(col, rgb((0.22, 0.21, 0.20)), (smooth(0.72, 0.85, f2) * 0.5)[..., None])
        gloss, spec, metal = 0.28 - 0.1 * f2, 0.15 + 0 * f1, 0 * f1
        hgt = 0.00008 * (f3 - 0.5)
    elif kind == "chrome":
        col = (0.72 + 0.10 * (f1 - 0.5))[..., None] * rgb((1, 1.01, 1.03))
        col = mix(col, rgb((0.35, 0.33, 0.30)), (smooth(0.65, 0.85, f2) * 0.5)[..., None])
        gloss, spec, metal = 0.82 - 0.3 * smooth(0.65, 0.85, f2), 0.9 + 0 * f1, 1 + 0 * f1
    elif kind == "inner":
        col = (0.19 + 0.05 * (f1 - 0.5))[..., None] * rgb((1, 1.02, 1.05))
        col = mix(col, rgb((0.33, 0.17, 0.08)), (smooth(0.75, 0.85, f2) * 0.7)[..., None])
        gloss, spec, metal = 0.25 + 0 * f1, 0.2 + 0 * f1, 0.3 + 0 * f1
    elif kind == "engine":
        col = (0.17 + 0.06 * (f1 - 0.5))[..., None] * rgb((1, 1, 1.02))
        col = mix(col, rgb((0.05, 0.045, 0.04)), (smooth(0.55, 0.75, f2) * 0.7)[..., None])
        gloss, spec, metal = 0.35 + 0.2 * smooth(0.55, 0.75, f2), 0.35 + 0 * f1, 0.7 + 0 * f1
    elif kind == "seat":
        col = mix(rgb((0.20, 0.12, 0.08)), rgb((0.33, 0.24, 0.17)), smooth(0.5, 0.8, f1)[..., None])
        crack = smooth(0.48, 0.5, np.abs(vnoise(n, n, 40, 40, rng) - 0.5) * -1 + 0.5)
        col = mix(col, rgb((0.10, 0.06, 0.04)), (crack * 0.6)[..., None])
        gloss, spec, metal = 0.28 - 0.15 * f1, 0.15 + 0 * f1, 0 * f1
        hgt = 0.0004 * (f2 - 0.5) - 0.0003 * crack
    elif kind == "floor":
        rib = (np.sin(np.arange(n) / n * 2 * math.pi * 24) > 0.3).astype(np.float32)[None, :].repeat(n, 0)
        col = (0.06 + 0.02 * rib + 0.04 * f1)[..., None] * rgb((1, 0.98, 0.95))
        col = mix(col, rgb((0.25, 0.22, 0.18)), (smooth(0.6, 0.8, f2) * 0.6)[..., None])
        gloss, spec, metal = 0.12 + 0 * f1, 0.08 + 0 * f1, 0 * f1
        hgt = 0.002 * rib
    elif kind == "rim":
        col = (0.20 + 0.05 * (f1 - 0.5))[..., None] * rgb((1, 1.02, 1.05))
        col = mix(col, rgb((0.38, 0.18, 0.08)), (smooth(0.68, 0.8, f2) * 0.8)[..., None])
        gloss, spec, metal = 0.3 - 0.15 * smooth(0.68, 0.8, f2), 0.3 + 0 * f1, 0.6 + 0 * f1
    elif kind == "net":
        u = (np.arange(n) + 0.5) / n
        uu, vv = np.meshgrid(u, u)
        k = 20
        d1 = np.abs(np.mod((uu + vv) * k, 1.0) - 0.5)
        d2 = np.abs(np.mod((uu - vv) * k, 1.0) - 0.5)
        wire = np.maximum(smooth(0.42, 0.47, d1), smooth(0.42, 0.47, d2))
        col = (0.22 + 0.08 * f1)[..., None] * rgb((1, 1, 1.02))
        alpha = wire
        gloss, spec, metal = 0.35 + 0 * f1, 0.5 + 0 * f1, 0.9 + 0 * f1
        hgt = 0.001 * wire
    elif kind in ("lamp", "tail", "amber"):
        base = {"lamp": (0.80, 0.82, 0.80), "tail": (0.55, 0.03, 0.02), "amber": (0.85, 0.40, 0.03)}[kind]
        prism = (np.sin(np.arange(n) / n * 2 * math.pi * 40) * 0.5 + 0.5)[None, :].repeat(n, 0)
        col = rgb(base) * (0.85 + 0.15 * prism)[..., None]
        col = mix(col, rgb((0.3, 0.28, 0.25)), (smooth(0.7, 0.85, f2) * 0.4)[..., None])
        gloss, spec, metal = 0.85 + 0 * f1, 0.9 + 0 * f1, 0 * f1
        hgt = 0.0004 * prism
    else:
        raise ValueError(kind)
    return pack(col, hgt, spec + 0 * f1, gloss + 0 * f1, metal + 0 * f1, ppm, alpha=alpha)


TILES = ["metal_rust", "copper", "rubber", "plastic", "chrome", "inner", "engine", "seat", "floor", "rim", "net",
         "lamp", "tail", "amber"]


def all_textures():
    out = {"body": paint_body()}
    for i, k in enumerate(TILES):
        out[k] = tile(k, 100 + i)
    return out


# ------------------------------------------------------------------ запись
def write_tga(path, img, alpha=None):
    """Несжатый TGA (тип 2), начало координат снизу — ImageToPAA читает его без вопросов."""
    h, w = img.shape[:2]
    a = (np.clip(img[..., :3], 0, 1) * 255 + 0.5).astype(np.uint8)[..., ::-1]      # BGR
    if alpha is not None:
        al = (np.clip(alpha, 0, 1) * 255 + 0.5).astype(np.uint8)[..., None]
        a = np.concatenate([a, al], -1)
    a = a[::-1]                                                                    # снизу вверх
    bpp = 32 if alpha is not None else 24
    desc = 8 if alpha is not None else 0
    hdr = struct.pack("<BBBHHBHHHHBB", 0, 0, 2, 0, 0, 0, 0, 0, w, h, bpp, desc)
    with open(path, "wb") as f:
        f.write(hdr)
        f.write(np.ascontiguousarray(a).tobytes())


def export_sources(tex, out_dir, prefix="er_"):
    """TGA-исходники: <prefix><имя>_co/_ca, _nohq, _smdi. Возвращает список файлов."""
    os.makedirs(out_dir, exist_ok=True)
    files = []
    for name, t in tex.items():
        base = os.path.join(out_dir, prefix + name)
        if t["alpha"] is not None:
            write_tga(base + "_ca.tga", t["co"], t["alpha"])
            files.append(base + "_ca.tga")
        else:
            write_tga(base + "_co.tga", t["co"])
            files.append(base + "_co.tga")
        write_tga(base + "_nohq.tga", t["nohq"])
        write_tga(base + "_smdi.tga", t["smdi"])
        files += [base + "_nohq.tga", base + "_smdi.tga"]
    return files
