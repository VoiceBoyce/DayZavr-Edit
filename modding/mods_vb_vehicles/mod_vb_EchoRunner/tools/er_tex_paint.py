# -*- coding: utf-8 -*-
r"""Перекраска текстур ALEX под Echo Runner (этап 5) по картам из er_tex_bake.py.

  blender.exe -b --factory-startup --python er_tex_paint.py -- <вариант> [--only=body,salon]
  вход:  оригинал ALEX (купленные модели/.../data/*_co, _nohq, _smdi.paa — всегда исходник, без наслоения),
         work/<вариант>/tex/<имя>_pos/_nrm/_mask.npy
  выход: VB_EchoRunner/data/<имя>_co, _nohq, _smdi .png + .paa (поверх копий ALEX под нашими путями)

Слои считаются по 3D-положению текселя (оси Blender машины), поэтому на стыках островов развёртки швов нет,
а зеркальные половины одинаковы (по |x|). Кузов: тёмная сталь с деталировкой ALEX, ржавчина ALEX сохраняется,
своя ржавчина и потёки, швы листов с двумя рядами заклёпок (цвет, высота -> _nohq, блеск), царапины, эмблема на
задней решётке закрашена. Ходовая (и баки) — тёмный ржавый металл. Салон: кожа в тёмно-коричневую, металл
ржавее, приборы как есть. Мотор: темнее и ржавее, таблички на клапанных крышках размыты.
"""
import os
import subprocess
import sys

import bpy
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import er_texlib as T  # noqa: E402

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
VAR = [a for a in args if not a.startswith("--")][0]
MOD = os.path.dirname(HERE)
TEX = os.path.join(MOD, "work", VAR, "tex")
ALEX = os.path.join(os.path.dirname(MOD), "купленные модели", "Interceptor", "Interceptor_byALEX", "data")
OUT = os.path.join(MOD, "VB_EchoRunner", "data")
ONLY = set(str(opts.get("only", "")).split(",")) - {""}
os.makedirs(os.path.join(TEX, "alex"), exist_ok=True)


# ---------------------------------------------------------------- ввод
def alex_img(name):
    """Оригинальная текстура ALEX -> массив (H, W, 4) 0..1, строка 0 — верх."""
    src = None
    for f in os.listdir(ALEX):
        if f.lower() == name.lower():
            src = os.path.join(ALEX, f)
    tga = os.path.join(TEX, "alex", name.lower().replace(".paa", ".tga"))
    if not os.path.exists(tga):
        subprocess.run([T.IMAGE_TO_PAA, src, tga], capture_output=True)
    im = bpy.data.images.load(tga)
    w, h = im.size
    a = np.array(im.pixels[:], np.float32).reshape(h, w, 4)[::-1]
    bpy.data.images.remove(im)
    return a


def resize(a, w, h):
    """Билинейное масштабирование (H, W, C)."""
    H, W = a.shape[:2]
    if (W, H) == (w, h):
        return a
    ys = (np.arange(h) + 0.5) * H / h - 0.5
    xs = (np.arange(w) + 0.5) * W / w - 0.5
    y0 = np.clip(np.floor(ys).astype(int), 0, H - 1)
    x0 = np.clip(np.floor(xs).astype(int), 0, W - 1)
    y1, x1 = np.clip(y0 + 1, 0, H - 1), np.clip(x0 + 1, 0, W - 1)
    fy = np.clip(ys - y0, 0, 1)[:, None, None]
    fx = np.clip(xs - x0, 0, 1)[None, :, None]
    top = a[y0][:, x0] * (1 - fx) + a[y0][:, x1] * fx
    bot = a[y1][:, x0] * (1 - fx) + a[y1][:, x1] * fx
    return top * (1 - fy) + bot * fy


def blur_np(a, r):
    """Размытие без переноса через край (разделимый бокс-фильтр x3 ~ гаусс)."""
    out = a.astype(np.float32)
    k = max(1, int(r))
    for _ in range(3):
        for ax in (0, 1):
            c = np.cumsum(np.pad(out, [(k + 1, k) if i == ax else (0, 0) for i in range(out.ndim)], mode="edge"), axis=ax)
            hi = np.take(c, np.arange(2 * k + 1, c.shape[ax]), axis=ax)
            lo = np.take(c, np.arange(0, c.shape[ax] - 2 * k - 1), axis=ax)
            out = (hi - lo) / (2 * k + 1)
    return out


# ---------------------------------------------------------------- 3D-шум
def vnoise3(P, scale, seed=0):
    q = P * scale
    i = np.floor(q)
    f = q - i
    u = f * f * (3 - 2 * f)

    def h(ix, iy, iz):
        v = np.sin(ix * 127.1 + iy * 311.7 + iz * 74.7 + seed * 19.19) * 43758.5453
        return v - np.floor(v)
    ix, iy, iz = i[:, 0], i[:, 1], i[:, 2]
    ux, uy, uz = u[:, 0], u[:, 1], u[:, 2]
    c = 0
    for dx in (0, 1):
        for dy in (0, 1):
            for dz in (0, 1):
                w = (ux if dx else 1 - ux) * (uy if dy else 1 - uy) * (uz if dz else 1 - uz)
                c = c + w * h(ix + dx, iy + dy, iz + dz)
    return c


def fbm3(P, scale, seed=0, oct=5):
    s, a, tot = 0.0, 0.5, 0.0
    for o in range(oct):
        s = s + a * vnoise3(P, scale * (2 ** o), seed + o * 7)
        tot += a
        a *= 0.5
    return s / tot


def sm(x, a, b):
    t = np.clip((x - a) / (b - a), 0, 1)
    return t * t * (3 - 2 * t)


def tri(x, period, off=0.0):
    """Расстояние до ближайшей линии сетки с шагом period."""
    return np.abs(((x - off) / period + 0.5) % 1.0 - 0.5) * period


def lum(c):
    return c[..., 0] * 0.3 + c[..., 1] * 0.59 + c[..., 2] * 0.11


# ---------------------------------------------------------------- запись
def write_set(name, co, nohq, smdi_arr):
    p = os.path.join(OUT, name)
    q = os.path.join(TEX, "png", name)                  # исходники PNG — в work/, не в мод
    os.makedirs(os.path.dirname(q), exist_ok=True)
    T.save_png(co, q + "_co.png")
    T.save_png(nohq, q + "_nohq.png", noncolor=True)
    T.save_png(smdi_arr, q + "_smdi.png", noncolor=True)
    for s in ("_co", "_nohq", "_smdi"):
        T.to_paa(q + s + ".png", p + s + ".paa")
    print("WROTE", name, co.shape)


def combine_normals(alex_n, h, strength, valid):
    """ALEX _nohq (0..1) + наша высота -> _nohq (смешение UDN)."""
    a = alex_n[..., :3] * 2 - 1
    b = T.normal_from_height_np(h, strength, valid.astype(np.float32)) * 2 - 1
    n = np.stack([a[..., 0] + b[..., 0], a[..., 1] + b[..., 1], a[..., 2]], -1)
    n /= np.maximum(np.linalg.norm(n, axis=-1, keepdims=True), 1e-6)
    return n * 0.5 + 0.5


def load_maps(name):
    pos = np.load(os.path.join(TEX, name + "_pos.npy")).astype(np.float32)
    nrm = np.load(os.path.join(TEX, name + "_nrm.npy")).astype(np.float32)
    mask = np.load(os.path.join(TEX, name + "_mask.npy"))
    valid = (np.abs(pos) < 3.9).all(-1)          # с полями запекания (16 px) — без швов на краях островов
    return pos, nrm, mask, valid


# ---------------------------------------------------------------- кузов
def paint_body():
    pos, nrm, mask, valid = load_maps("body")
    H, W = valid.shape
    base = resize(alex_img("body_co.paa"), W, H)[..., :3]
    an = resize(alex_img("body_nohq.paa"), W, H)
    asm = resize(alex_img("body_smdi.paa"), W, H)
    L = lum(base)
    sat = (base.max(-1) - base.min(-1)) / np.maximum(base.max(-1), 1e-4)
    alex_rust = sm(sat, 0.28, 0.55) * (base[..., 0] > base[..., 2] * 1.15) * sm(L, 0.08, 0.2)
    rel = np.clip(L / np.maximum(blur_np(L, 24), 1e-3), 0.55, 1.45)
    idx = np.nonzero(valid)
    P = pos[idx].astype(np.float64)
    N = nrm[idx]
    Pa = P.copy()
    Pa[:, 0] = np.abs(Pa[:, 0])                         # зеркальные половины одинаковы
    x, y, z = Pa[:, 0], Pa[:, 1], Pa[:, 2]
    nx, ny, nz = np.abs(N[:, 0]), np.abs(N[:, 1]), N[:, 2]

    fine = fbm3(Pa, 9.0, 1, 3)
    steel = np.array([0.112, 0.108, 0.104]) * (rel[idx] * (0.82 + 0.36 * fine))[:, None]
    col = steel * (1 - alex_rust[idx][:, None] * 0.85) + base[idx] * 0.92 * (alex_rust[idx][:, None] * 0.85)

    r = fbm3(Pa, 1.7, 3) + 0.30 * (1 - sm(z, 0.25, 1.05)) + 0.08 * (nz > 0.6)
    rmask = sm(r, 0.555, 0.69)
    tone = fbm3(Pa, 6.0, 5, 4)
    rust = np.array([0.29, 0.13, 0.055]) * (1 - tone[:, None]) + np.array([0.15, 0.066, 0.03]) * tone[:, None]
    hot = sm(fbm3(Pa, 15.0, 7, 3), 0.66, 0.74) * rmask
    rust = rust * (1 - hot[:, None] * 0.7) + np.array([0.44, 0.22, 0.08]) * hot[:, None] * 0.7
    col = col * (1 - rmask[:, None]) + rust * rmask[:, None]

    vert = (np.abs(nz) < 0.55).astype(np.float64)
    s = fbm3(np.stack([x * 10, y * 10, z * 0.6], 1), 1.0, 9, 4)
    streak = sm(s, 0.57, 0.72) * vert * (1 - rmask * 0.4)
    col = col * (1 - streak[:, None] * 0.65) + np.array([0.19, 0.085, 0.038]) * streak[:, None] * 0.65

    # швы листов и заклёпки (борта / верх / перед-зад)
    side = (nx > 0.55)
    top = (nz > 0.55) & ~side
    fb = (ny > 0.55) & ~side & ~top
    PITCH, OFFS, RR = 0.058, 0.017, 0.0078
    seam = np.full(len(x), 9.0)
    riv = np.full(len(x), 9.0)
    step = np.zeros(len(x))
    # борта: вертикальные швы каждые 0.5 м, горизонтальные на z 0.52 и 0.80
    d_v = tri(y, 0.50, 0.02)
    seam = np.where(side, np.minimum(seam, d_v), seam)
    rv = np.sqrt((np.abs(d_v - OFFS)) ** 2 + tri(z, PITCH) ** 2)
    riv = np.where(side, np.minimum(riv, rv), riv)
    for zs in (0.52, 0.80):
        d_h = np.abs(z - zs)
        seam = np.where(side, np.minimum(seam, d_h), seam)
        rh = np.sqrt((np.abs(d_h - OFFS)) ** 2 + tri(y, PITCH) ** 2)
        riv = np.where(side, np.minimum(riv, rh), riv)
    step = np.where(side, (((y - 0.02) / 0.50) % 1.0 < 0.5) * 1.0, step)
    # верх: поперечные швы каждые 0.55 м, продольные на |x| 0.40
    d_t = tri(y, 0.55, 0.10)
    seam = np.where(top, np.minimum(seam, d_t), seam)
    riv = np.where(top, np.minimum(riv, np.sqrt((np.abs(d_t - OFFS)) ** 2 + tri(x, PITCH) ** 2)), riv)
    d_l = np.abs(x - 0.40)
    seam = np.where(top, np.minimum(seam, d_l), seam)
    riv = np.where(top, np.minimum(riv, np.sqrt((np.abs(d_l - OFFS)) ** 2 + tri(y, PITCH) ** 2)), riv)
    step = np.where(top, ((((y - 0.10) / 0.55) % 1.0 < 0.5) ^ (x > 0.40)) * 1.0, step)
    # перед и зад: горизонтальный шов на z 0.80, вертикальный по центру
    d_f = np.abs(z - 0.80)
    seam = np.where(fb, np.minimum(seam, np.minimum(d_f, x)), seam)
    riv = np.where(fb, np.minimum(riv, np.sqrt((np.abs(d_f - OFFS)) ** 2 + tri(x, PITCH) ** 2)), riv)

    groove = sm(0.0022 - seam, 0, 0.0012)
    head = np.sqrt(np.clip(1 - (riv / RR) ** 2, 0, 1))
    ring = sm(RR + 0.004 - riv, 0, 0.004) * (riv > RR)
    col = col * (1 - groove[:, None] * 0.6)
    col = col * (1 - ring[:, None] * 0.35) + np.array([0.26, 0.11, 0.045]) * ring[:, None] * 0.35
    rivc = np.array([0.20, 0.19, 0.18]) * (0.85 + 0.3 * fine[:, None])
    hd = sm(head, 0.0, 0.25)
    col = col * (1 - hd[:, None]) + rivc * hd[:, None]
    # подтёки ржавчины под заклёпками бортов
    below = side & (tri(z, PITCH) < 0.03)
    drip = sm(0.004 - np.abs(np.minimum(rv, 9) - 0.0), 0, 0.004) * 0
    _ = (below, drip)

    out = base.copy() * 0.8
    out[idx] = np.clip(col, 0, 1)
    # царапины (в развёртке, слабо)
    sc = T.scratches(max(W, H), 900, 91, (0.004, 0.03), 1.2)[:H, :W]
    out = out + (sc * valid)[..., None] * 0.10
    # эмблема по центру задней решётки (2048-пространство ~ (1690, 55), r ~ 18) -> размыть
    cx, cy, rad = int(1690 * W / 2048), int(55 * H / 1024), int(22 * W / 2048)
    yy, xx = np.mgrid[0:H, 0:W]
    badge = ((xx - cx) ** 2 + (yy - cy) ** 2) < rad * rad
    y0, y1 = max(cy - 3 * rad, 0), min(cy + 3 * rad, H)
    x0, x1 = max(cx - 3 * rad, 0), min(cx + 3 * rad, W)
    bl = blur_np(out[y0:y1, x0:x1], rad * 0.6)
    sub = badge[y0:y1, x0:x1]
    out[y0:y1, x0:x1][sub] = bl[sub]

    h = np.zeros((H, W), np.float32)
    hv = 1.2 * head - 0.7 * groove + 0.25 * step + 0.35 * rmask * fbm3(Pa, 30.0, 11, 2)
    h[idx] = hv
    nohq = combine_normals(an, h, 2.6, valid)
    spec = asm[..., 1].copy()
    gloss = asm[..., 2].copy()
    rt = np.zeros((H, W))
    rt[idx] = np.clip(rmask + alex_rust[idx] * 0.6 + streak * 0.4, 0, 1)
    hdm = np.zeros((H, W))
    hdm[idx] = hd
    spec = spec * (1 - 0.7 * rt) + 0.15 * hdm
    gloss = gloss * (1 - 0.7 * rt) + 0.1 * hdm
    write_set("body", out, nohq, T.smdi(spec, gloss))


# ---------------------------------------------------------------- ходовая (и баки)
def paint_chassis():
    pos, nrm, mask, valid = load_maps("chassis")
    H, W = valid.shape
    base = resize(alex_img("chassis_co.paa"), W, H)[..., :3]
    an = resize(alex_img("chassis_nohq.paa"), W, H)
    asm = resize(alex_img("chassis_smdi.paa"), W, H)
    L = lum(base)
    rel = np.clip(L / np.maximum(blur_np(L, 20), 1e-3), 0.5, 1.5)
    out = (np.array([0.13, 0.10, 0.08]) * rel[..., None] * 0.9).astype(np.float32)
    idx = np.nonzero(valid)
    P = pos[idx].astype(np.float64)
    P[:, 0] = np.abs(P[:, 0])
    r = sm(fbm3(P, 2.2, 21), 0.42, 0.62)
    tone = fbm3(P, 7.0, 23, 4)
    rust = np.array([0.27, 0.12, 0.05]) * (1 - tone[:, None]) + np.array([0.13, 0.06, 0.028]) * tone[:, None]
    c = out[idx] * (1 - r[:, None]) + rust * rel[idx][:, None] * r[:, None]
    out[idx] = np.clip(c, 0, 1)
    h = np.zeros((H, W), np.float32)
    h[idx] = 0.4 * r * fbm3(P, 28.0, 25, 2)
    rt = np.zeros((H, W))
    rt[idx] = r
    write_set("chassis", out, combine_normals(an, h, 2.0, valid),
              T.smdi(asm[..., 1] * (1 - 0.6 * rt), asm[..., 2] * (1 - 0.6 * rt)))


# ---------------------------------------------------------------- салон
def paint_salon():
    pos, nrm, mask, valid = load_maps("salon")
    H, W = valid.shape
    base = resize(alex_img("salon_co.paa"), W, H)[..., :3]
    an = resize(alex_img("salon_nohq.paa"), W, H)
    asm = resize(alex_img("salon_smdi.paa"), W, H)
    L = lum(base)
    sat = (base.max(-1) - base.min(-1)) / np.maximum(base.max(-1), 1e-4)
    bright = sm(L, 0.45, 0.6)                                   # приборы, хром — не трогаем
    metal = sm(sat, 0.22, 0.40) * (base[..., 0] > base[..., 2]) * (1 - bright)
    leather = (1 - metal) * (1 - bright)
    rel = np.clip(L / np.maximum(blur_np(L, 16), 1e-3), 0.5, 1.5)
    leath = np.array([0.125, 0.085, 0.062]) * rel[..., None]
    out = base * bright[..., None] + leath * leather[..., None] + base * 0.85 * metal[..., None]
    idx = np.nonzero(valid)
    P = pos[idx].astype(np.float64)
    P[:, 0] = np.abs(P[:, 0])
    wear = sm(fbm3(P, 5.0, 31, 4), 0.58, 0.7) * leather[idx]
    c = out[idx] * (1 - wear[:, None] * 0.5) + np.array([0.24, 0.18, 0.13]) * wear[:, None] * 0.5
    r = sm(fbm3(P, 3.0, 33), 0.5, 0.66) * metal[idx]
    c = c * (1 - r[:, None] * 0.6) + np.array([0.26, 0.12, 0.05]) * r[:, None] * 0.6
    out[idx] = np.clip(c, 0, 1)
    h = np.zeros((H, W), np.float32)
    h[idx] = 0.3 * wear
    write_set("salon", np.clip(out, 0, 1), combine_normals(an, h, 1.5, valid),
              T.smdi(asm[..., 1] * (1 - 0.3 * metal), asm[..., 2] * (0.8 + 0.2 * leather)))


# ---------------------------------------------------------------- мотор
def paint_engine():
    pos, nrm, mask, valid = load_maps("engine")
    H, W = valid.shape
    base = resize(alex_img("engine_co.paa"), W, H)[..., :3]
    an = resize(alex_img("engine_nohq.paa"), W, H)
    asm = resize(alex_img("engine_smdi.paa"), W, H)
    out = base * 0.82
    k = W / 512.0                                                # таблички на клапанных крышках -> размыть
    for (x0, x1, y0, y1) in ((34, 78, 294, 314), (34, 78, 355, 377)):
        a, b, c, d = int(y0 * k), int(y1 * k), int(x0 * k), int(x1 * k)
        out[a:b, c:d] = blur_np(out[a - 8:b + 8, c - 8:d + 8], 6 * k)[8:-8, 8:-8]
    idx = np.nonzero(valid)
    P = pos[idx].astype(np.float64)
    P[:, 0] = np.abs(P[:, 0])
    r = sm(fbm3(P, 4.0, 41), 0.55, 0.7)
    c = out[idx] * (1 - r[:, None] * 0.55) + np.array([0.27, 0.12, 0.05]) * r[:, None] * 0.55
    out[idx] = np.clip(c, 0, 1)
    h = np.zeros((H, W), np.float32)
    rt = np.zeros((H, W))
    rt[idx] = r
    write_set("engine", out, combine_normals(an, h, 1.0, valid),
              T.smdi(asm[..., 1] * (1 - 0.5 * rt), asm[..., 2] * (1 - 0.5 * rt)))


for nm, fn in (("body", paint_body), ("chassis", paint_chassis), ("salon", paint_salon), ("engine", paint_engine)):
    if ONLY and nm not in ONLY:
        continue
    fn()
print("OK", OUT)
