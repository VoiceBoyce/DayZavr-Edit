# -*- coding: utf-8 -*-
"""Генератор «Жил Изнанки» (VB_RiftVeins): модель .p3d, текстуры .paa, материалы .rvmat.

Запуск (Python из Blender, в нём есть numpy):
  "G:\\SteamLibrary\\steamapps\\common\\Blender\\5.2\\python\\bin\\python.exe" gen_rift_veins.py <папка_для_png>

Что получается:
  - узел-«сердце» (бугристый ком, наполовину в земле) со светящимся «глазом» сверху;
  - 8 корней расходятся от узла, у части есть отростки, корни местами выгибаются над землёй;
  - по корням бегут светящиеся жилы, у концов корней они уходят в землю светящимися трещинами;
  - на коре корней и узла светится сетка тонких трещин (маска свечения в Stage8, как у ванильной тыквы).

Геометрия строится в осях Blender (Z вверх, грани против часовой снаружи, нормали наружу),
потом py3d.blender_to_dayz переводит всю модель в оси DayZ (правило 12 из dayz-model-pipeline).
Визуальные LOD не децимируются: каждый строится заново с меньшим числом сегментов.
"""
import math
import os
import struct
import subprocess
import sys
import zlib

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
MOD_DIR = os.path.normpath(os.path.join(HERE, "..", "VB_Rift"))
DATA_DIR = os.path.join(MOD_DIR, "data")
REPO = os.path.normpath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "tools", "py3d"))
import py3d  # noqa: E402

BS = chr(92)
PREFIX = "VB_Rift" + BS + "data" + BS
IMAGE_TO_PAA = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools",
                            "Bin", "ImageToPAA", "ImageToPAA.exe")
PENETRATION = BS.join(["dz", "data", "data", "penetration", "flesh.rvmat"])
SEED = 1337

# уровни свечения для пульсации (скрипт переключает материалы 0..3)
CRUST_EMISSIVE = [1.0, 2.2, 4.0, 6.5]
VEIN_EMISSIVE = [2.5, 5.0, 9.0, 15.0]
# Флаг точки MLOD «Keep height» (POINT_KEEPLAND, 0x8) НЕ ставить: на моделях в тысячи вершин клиент DayZDiag
# 1.30 падает с 0xc0000374 (heap corruption) через секунды после появления объекта — проверено 2026-11-11,
# и с пульсом, и на голом HouseNoDestruct без скрипта. В VanillaRoadPartsPack флаг 0x1 стоит на моделях из 4 вершин.
# По склону объекты наклоняет скрипт (VB_RiftBase.VB_AlignToTerrain).
POINT_KEEPLAND = 0x8
CUT_STEP = 0.8      # шаг выборки вдоль корней и трещин для срезателей травы cut_N, м
CUT_GAP = 1.4        # не ставить срезатель 2x2 ближе этого к уже поставленному, м
CUT_CENTER = 2.6     # ближе к центру траву режет один ClutterCutter6x6
BASE_LEVEL = 1

MATS = {
    "crust": {"tex": PREFIX + "vb_rift_crust_co.paa", "mat": PREFIX + "vb_rift_crust.rvmat", "sel": "crust"},
    "vein": {"tex": PREFIX + "vb_rift_vein_co.paa", "mat": PREFIX + "vb_rift_vein.rvmat", "sel": "veins"},
}
# материалы предметов-изделий (без пульса): текстуры и rvmat делает make_prop_textures (--props-tex)
PROP_MATS = ["wood", "cloth", "plastic", "metal", "bone", "glass", "liquid", "bladesteel", "burlap", "sporecloth", "bark"]
for _m in PROP_MATS:
    _tex = PREFIX + "vb_rift_p_%s_%s.paa" % (_m, "ca" if _m == "glass" else "co")
    MATS[_m] = {"tex": _tex, "mat": PREFIX + "vb_rift_p_%s.rvmat" % _m, "sel": "p_" + _m}
# стекло берём ванильное медицинское: Super + noZWrite и своя альфа-текстура (своё _ca рисовалось матовым)
MATS["glass"]["tex"] = BS.join(["dz", "gear", "medical", "data", "glass_CA.paa"])
MATS["glass"]["mat"] = BS.join(["dz", "gear", "medical", "data", "glass.rvmat"])
# корпус «Сердца-аккумулятора»: ванильная текстура автомобильного аккумулятора (атлас AMUKA)
MATS["battery"] = {"tex": BS.join(["dz", "vehicles", "parts", "data", "battery_smallcar_co.paa"]),
                   "mat": BS.join(["dz", "vehicles", "parts", "data", "battery_smallcar.rvmat"]), "sel": "p_battery"}

# параметры визуальных LOD: шаг вдоль трубки (м) и число сегментов по окружности
LOD_PARAMS = [
    {"res": 0.0, "root": (0.09, 8), "vein": (0.09, 5), "crack": (0.08, 4), "heart": (30, 18), "core": (12, 8)},
    {"res": 1.0, "root": (0.16, 6), "vein": (0.16, 4), "crack": (0.14, 3), "heart": (18, 10), "core": (8, 5)},
    {"res": 4.0, "root": (0.35, 4), "vein": (0.35, 3), "crack": (0.35, 3), "heart": (10, 6), "core": (6, 4)},
]

DS = 0.03            # шаг плотного пути, м
SINK = 0.15          # центр корня над землёй в долях радиуса (остальное в земле)
HEART_C = np.array([0.0, 0.0, 0.10])
HEART_R = np.array([0.62, 0.56, 0.46])


# ---------------------------------------------------------------- шум
def _hash3(ix, iy, iz, seed):
    h = (ix.astype(np.uint64) * np.uint64(73856093)) ^ (iy.astype(np.uint64) * np.uint64(19349663)) \
        ^ (iz.astype(np.uint64) * np.uint64(83492791)) ^ np.uint64((seed * 2654435761) & 0xFFFFFFFF)
    h ^= h >> np.uint64(13)
    h *= np.uint64(0x5BD1E995)
    h ^= h >> np.uint64(15)
    return (h & np.uint64(0xFFFFFF)).astype(np.float64) / float(0xFFFFFF)


def vnoise3(p, seed=0):
    p = np.asarray(p, float)
    i = np.floor(p).astype(np.int64)
    f = p - i
    u = f * f * (3 - 2 * f)
    out = np.zeros(len(p))
    for dx in (0, 1):
        for dy in (0, 1):
            for dz in (0, 1):
                w = (u[:, 0] if dx else 1 - u[:, 0]) * (u[:, 1] if dy else 1 - u[:, 1]) \
                    * (u[:, 2] if dz else 1 - u[:, 2])
                out += w * _hash3(i[:, 0] + dx, i[:, 1] + dy, i[:, 2] + dz, seed)
    return out * 2 - 1


def fbm3(p, octaves=3, seed=0):
    q = np.asarray(p, float)
    s = np.zeros(len(q))
    a, norm = 1.0, 0.0
    for o in range(octaves):
        s += a * vnoise3(q, seed + o * 17)
        norm += a
        a *= 0.5
        q = q * 2.03
    return s / norm


def smoothstep(e0, e1, x):
    t = np.clip((np.asarray(x, float) - e0) / (e1 - e0), 0, 1)
    return t * t * (3 - 2 * t)


def bump(x):
    x = np.asarray(x, float)
    return np.where(np.abs(x) < 1, (1 - x * x) ** 2, 0.0)


# ---------------------------------------------------------------- пути
class Tube:
    def __init__(self, pts, radii, kind, lump=0.0, lump_freq=6.0, seed=0, cap_start=True, cap_end=True,
                 mat=None, segs=None, fixed_segs=False, uv_len=None, cap_flat=False, raw=False, ang0=0.0, aspect=1.0):
        self.pts = np.asarray(pts, float)
        self.radii = np.asarray(radii, float)
        self.kind = kind            # root | vein | crack: шаг и сегменты из LOD_PARAMS
        self.lump = lump
        self.lump_freq = lump_freq
        self.seed = seed
        self.cap_start = cap_start
        self.cap_end = cap_end
        # для предметов: материал из MATS (wood, cloth, ...), своё число сегментов (fixed — на всех LOD),
        # шаг UV, плоские торцы, путь без пересэмплирования (резкие ступени профиля), поворот и сплющивание сечения
        self.mat = mat
        self.segs = segs
        self.fixed_segs = fixed_segs
        self.uv_len = uv_len
        self.cap_flat = cap_flat
        self.raw = raw
        self.ang0 = ang0
        self.aspect = aspect


def walk_xy(rng, start, heading, length, wiggle, crack=False):
    """Путь по земле. Корень: плавно гуляющая кривизна. Трещина: рваный излом без закручивания."""
    n = max(6, int(length / DS))
    xy = np.zeros((n + 1, 2))
    hs = np.zeros(n + 1)
    xy[0] = start
    h, curv = heading, 0.0
    hs[0] = h
    for k in range(n):
        if crack:
            if k % 3 == 0:
                h += rng.normal(0, wiggle) - 0.25 * (h - heading)
        else:
            curv = curv * 0.93 + rng.normal(0, wiggle)
            h += curv * DS
            h -= 0.035 * (h - heading)     # тяга наружу: корень извивается, но не закручивается обратно
        xy[k + 1] = xy[k] + DS * np.array([math.cos(h), math.sin(h)])
        hs[k + 1] = h
    return xy, hs


def gen_root(rng, start_xy, heading, length, r0, z_start, blend_len, wiggle=0.9, zmode="ground"):
    """zmode: ground — лежит на земле с редкими выгибами; arch — «змея», ныряет в землю и выходит дугами."""
    xy, hs = walk_xy(rng, start_xy, heading, length, wiggle)
    s = np.arange(len(xy)) * DS
    t = s / s[-1]
    r = r0 * (1 - 0.8 * t ** 0.85)
    r *= 1 + 0.12 * np.sin(s * rng.uniform(2.5, 4.0) + rng.uniform(0, 6.28)) * (1 - t)
    tw = smoothstep(0.8, 1.0, t)
    r = r * (1 - tw) + 0.004 * tw
    if zmode == "arch":
        lam = rng.uniform(1.7, 2.6)
        amp = rng.uniform(0.35, 0.6)
        off = rng.uniform(0.25, 0.75) * lam
        zg = -1.05 * r + (amp + 1.05 * r) * np.abs(np.sin(np.pi * (s + off) / lam)) ** 0.75
    else:
        zg = SINK * r
        for _ in range(int(rng.integers(0, 3))):
            c = rng.uniform(0.3, 0.85) * s[-1]
            w = rng.uniform(0.25, 0.6)
            amp = rng.uniform(0.04, 0.15) * (1 - 0.5 * c / s[-1])
            zg = zg + amp * bump((s - c) / w)
    bl = smoothstep(0, blend_len, s)
    z = z_start * (1 - bl) + zg * bl
    pts = np.column_stack([xy, z])
    return pts, r, hs


def frames(P):
    T = np.gradient(P, axis=0)
    ln = np.linalg.norm(T, axis=1)
    # точки-дубликаты (ступени профиля у тел вращения): берём направление соседа, иначе рамка вырождается
    ok = np.where(ln > 1e-9)[0]
    if len(ok) and len(ok) < len(P):
        for i in np.where(ln <= 1e-9)[0]:
            T[i] = T[ok[np.argmin(np.abs(ok - i))]]
    T /= np.maximum(np.linalg.norm(T, axis=1, keepdims=True), 1e-9)
    up = np.array([0.0, 0.0, 1.0])
    n0 = up - T[0] * np.dot(up, T[0])
    if np.linalg.norm(n0) < 1e-6:
        n0 = np.array([1.0, 0.0, 0.0]) - T[0] * T[0, 0]
    N = np.zeros_like(P)
    N[0] = n0 / np.linalg.norm(n0)
    for i in range(1, len(P)):
        n = N[i - 1] - T[i] * np.dot(N[i - 1], T[i])
        ln = np.linalg.norm(n)
        N[i] = n / ln if ln > 1e-9 else N[i - 1]
    B = np.cross(T, N)
    return T, N, B


def vein_along(root_pts, root_r, rng, s_from, r_min):
    """Жила по поверхности корня: виляет вокруг верха корня и наполовину утоплена в него."""
    T, N, B = frames(root_pts)
    s = np.arange(len(root_pts)) * DS
    ok = (s >= s_from) & (root_r >= r_min)
    idx = np.nonzero(ok)[0]
    if len(idx) < 8:
        return None
    idx = idx[: np.argmax(np.diff(np.concatenate([idx, [idx[-1] + 2]])) > 1) + 1]  # непрерывный участок
    amp = rng.uniform(0.6, 1.1)
    freq = rng.uniform(1.2, 2.6)
    ph = rng.uniform(0, 6.28)
    th = amp * np.sin(freq * s[idx] + ph)
    d = np.cos(th)[:, None] * N[idx] + np.sin(th)[:, None] * B[idx]
    r = root_r[idx]
    rv = np.clip(0.17 * r, 0.007, 0.028)
    pts = root_pts[idx] + d * (r * 0.9)[:, None]
    return pts, rv


def gen_crack(rng, start, heading, length, rv0, depth=0, max_depth=1, p_branch=0.6):
    xy, hs = walk_xy(rng, start[:2], heading, length, 0.32, crack=True)
    s = np.arange(len(xy)) * DS
    t = s / s[-1]
    rv = rv0 * (1 - 0.6 * t)
    tw = smoothstep(0.8, 1.0, t)
    rv = rv * (1 - tw) + 0.0025 * tw
    z = -0.35 * rv
    pts = np.column_stack([xy, z])
    out = [Tube(pts, rv, "crack", lump=0.15, lump_freq=14.0, seed=int(rng.integers(1e6)))]
    if depth < max_depth:
        for _ in range(1 if rng.random() < p_branch else 0):
            k = int(rng.uniform(0.2, 0.7) * (len(xy) - 1))
            hh = hs[k] + rng.choice([-1, 1]) * rng.uniform(0.5, 1.1)
            out += gen_crack(rng, pts[k], hh, (s[-1] - s[k]) * rng.uniform(0.3, 0.6) + 0.15,
                             max(0.005, rv[k] * 0.75), depth + 1, max_depth, p_branch)
    return out


def heart_surface(dirs):
    dirs = np.atleast_2d(dirs)
    k = 1 + 0.16 * fbm3(dirs * 1.8 + 11.3, 3, seed=101)
    return HEART_C + dirs * HEART_R * k[:, None]


def build_scene(seed=SEED):
    rng = np.random.default_rng(seed)
    tubes = []
    n_roots = 8
    base = rng.uniform(0, 2 * math.pi)
    headings = [base + i * 2 * math.pi / n_roots + rng.uniform(-0.25, 0.25) for i in range(n_roots)]
    root_tips = []
    for i, hd in enumerate(headings):
        length = rng.uniform(3.8, 6.0)
        r0 = rng.uniform(0.11, 0.16)
        start = np.array([math.cos(hd), math.sin(hd)]) * 0.22
        pts, r, hs = gen_root(rng, start, hd, length, r0, z_start=0.22, blend_len=0.9)
        tubes.append(Tube(pts, r, "root", lump=0.13, lump_freq=5.5, seed=i * 31 + 7, cap_start=False))
        v = vein_along(pts, r, rng, 0.25, 0.022)
        if v is not None:
            tubes.append(Tube(v[0], v[1], "vein", lump=0.08, lump_freq=9.0, seed=i * 31 + 8))
        root_tips.append((pts[-1], hs[-1], True))
        # отростки
        for b in range(int(rng.integers(1, 3))):
            k = int(rng.uniform(0.3, 0.7) * (len(pts) - 1))
            bh = hs[k] + rng.choice([-1, 1]) * rng.uniform(0.45, 1.0)
            bl = rng.uniform(0.8, 2.0)
            bp, br, bhs = gen_root(rng, pts[k, :2], bh, bl, r[k] * 0.62, z_start=pts[k, 2], blend_len=0.3)
            tubes.append(Tube(bp, br, "root", lump=0.13, lump_freq=5.5, seed=i * 31 + 20 + b, cap_start=False))
            bv = vein_along(bp, br, rng, 0.05, 0.02)
            if bv is not None:
                tubes.append(Tube(bv[0], bv[1], "vein", lump=0.08, lump_freq=9.0, seed=i * 31 + 30 + b))
            root_tips.append((bp[-1], bhs[-1], False))
    # трещины от концов корней: у главных всегда, у отростков через раз
    for tip, hd, main_root in root_tips:
        if main_root or rng.random() < 0.5:
            tubes += gen_crack(rng, np.array([tip[0], tip[1], 0.0]), hd + rng.uniform(-0.6, 0.6),
                               rng.uniform(0.9, 2.0), 0.012)
    # трещины от узла между корнями
    for i in range(0, n_roots, 1):
        if i % 4 == 3:
            continue
        hd = headings[i] + math.pi / n_roots + rng.uniform(-0.2, 0.2)
        st = np.array([math.cos(hd), math.sin(hd), 0.0]) * 0.55
        tubes += gen_crack(rng, st, hd, rng.uniform(1.4, 2.8), 0.014)
    # жилы по узлу: от каждого корня вверх к «глазу»
    heart_veins = []
    for i, hd in enumerate(headings):
        u = np.linspace(0, 1, 48)
        th = 1.45 + (0.2 - 1.45) * u
        ph = hd + 0.3 * np.sin(u * 5 + rng.uniform(0, 6.28))
        dirs = np.column_stack([np.sin(th) * np.cos(ph), np.sin(th) * np.sin(ph), np.cos(th)])
        rv = 0.022 - 0.006 * u
        pts = heart_surface(dirs) + dirs * (rv * 0.3)[:, None]
        heart_veins.append(Tube(pts, rv, "vein", lump=0.08, lump_freq=9.0, seed=500 + i))
    tubes += heart_veins
    top = heart_surface(np.array([[0.0, 0.0, 1.0]]))[0]
    core = {"center": top - np.array([0, 0, 0.035]), "radius": 0.11}
    return {"tubes": tubes, "core": core, "top": top, "heart": True}


def add_root_family(rng, tubes, tips, start_xy, hd, length, r0, zmode, seed_base, n_branches,
                    wiggle=0.7, two_veins=False):
    """Корень, выходящий из-под земли, с жилами и отростками."""
    pts, r, hs = gen_root(rng, np.asarray(start_xy, float), hd, length, r0, z_start=-1.3 * r0,
                          blend_len=0.6, wiggle=wiggle, zmode=zmode)
    tubes.append(Tube(pts, r, "root", lump=0.13, lump_freq=5.5, seed=seed_base))
    for k in range(2 if two_veins else 1):
        v = vein_along(pts, r, rng, 0.15, 0.022)
        if v is not None:
            tubes.append(Tube(v[0], v[1], "vein", lump=0.08, lump_freq=9.0, seed=seed_base + 1 + k))
    tips.append((pts[-1], hs[-1], True))
    for b in range(n_branches):
        k = int(rng.uniform(0.3, 0.75) * (len(pts) - 1))
        if pts[k, 2] < 0:
            continue            # корень здесь под землёй, отросток не растёт
        bh = hs[k] + rng.choice([-1, 1]) * rng.uniform(0.45, 1.0)
        bp, br, bhs = gen_root(rng, pts[k, :2], bh, rng.uniform(0.7, 1.8), r[k] * 0.62,
                               z_start=pts[k, 2], blend_len=0.3)
        tubes.append(Tube(bp, br, "root", lump=0.13, lump_freq=5.5, seed=seed_base + 10 + b, cap_start=False))
        bv = vein_along(bp, br, rng, 0.05, 0.02)
        if bv is not None:
            tubes.append(Tube(bv[0], bv[1], "vein", lump=0.08, lump_freq=9.0, seed=seed_base + 20 + b))
        tips.append((bp[-1], bhs[-1], False))


def build_roots_scene(seed, kind):
    """Корни без сердцевины. Начало координат — место, где корни выходят из земли."""
    rng = np.random.default_rng(seed)
    tubes, tips = [], []
    o = np.zeros(2)
    sb = seed * 100

    def unit(a):
        return np.array([math.cos(a), math.sin(a)])

    broken = 0
    if kind == "crawl":         # один длинный ползучий корень и тонкий спутник
        hd = rng.uniform(0, 2 * math.pi)
        add_root_family(rng, tubes, tips, o, hd, rng.uniform(7.0, 9.0), rng.uniform(0.14, 0.17), "ground",
                        sb, n_branches=4, two_veins=True)
        add_root_family(rng, tubes, tips, o + 0.35 * unit(hd + math.pi / 2), hd + rng.uniform(-0.25, 0.25),
                        rng.uniform(4.0, 6.0), rng.uniform(0.07, 0.09), "ground", sb + 50, n_branches=1)
        broken = 4
    elif kind == "fork":        # выходит из земли и расходится веером на три
        hd = rng.uniform(0, 2 * math.pi)
        for i, off in enumerate((-0.75, 0.0, 0.8)):
            add_root_family(rng, tubes, tips, o, hd + off + rng.uniform(-0.15, 0.15), rng.uniform(3.5, 5.5),
                            rng.uniform(0.11, 0.15), "ground", sb + i * 7, n_branches=1)
        broken = 5
    elif kind == "cluster":     # пучок во все стороны из одной точки
        n = int(rng.integers(5, 7))
        base = rng.uniform(0, 2 * math.pi)
        for i in range(n):
            hd = base + i * 2 * math.pi / n + rng.uniform(-0.3, 0.3)
            add_root_family(rng, tubes, tips, o + 0.12 * unit(hd), hd, rng.uniform(2.2, 4.2),
                            rng.uniform(0.09, 0.14), "ground", sb + i * 7, n_branches=int(rng.integers(0, 2)))
        broken = 6
    elif kind == "arch":        # «змеи»: ныряют в землю и выходят дугами
        hd = rng.uniform(0, 2 * math.pi)
        side = unit(hd + math.pi / 2)
        for i in range(int(rng.integers(2, 4))):
            st = o + (i - 1) * 0.6 * side + rng.normal(0, 0.1, 2)
            add_root_family(rng, tubes, tips, st, hd + rng.uniform(-0.3, 0.3), rng.uniform(4.5, 6.5),
                            rng.uniform(0.10, 0.14), "arch", sb + i * 7, n_branches=1, wiggle=0.45)
        broken = 3
    elif kind == "small":       # мелкая россыпь для заполнения
        n = int(rng.integers(2, 4))
        base = rng.uniform(0, 2 * math.pi)
        for i in range(n):
            hd = base + i * 2 * math.pi / n + rng.uniform(-0.5, 0.5)
            add_root_family(rng, tubes, tips, o, hd, rng.uniform(1.4, 2.6), rng.uniform(0.06, 0.09), "ground",
                            sb + i * 7, n_branches=0)
        broken = 3
    elif kind == "cracks":      # (не используется: старые VB_RiftCracks заменены трещинами gen_rift_fissures.py)
        for c in range(int(rng.integers(2, 4))):
            st = np.array([rng.normal(0, 0.6), rng.normal(0, 0.6), 0.0])
            for _ in range(int(rng.integers(3, 5))):
                tubes += gen_crack(rng, st, rng.uniform(0, 2 * math.pi), rng.uniform(2.0, 4.0),
                                   rng.uniform(0.016, 0.02), max_depth=2, p_branch=0.8)
    for tip, hd, main_root in tips:
        if main_root or rng.random() < 0.4:
            tubes += gen_crack(rng, np.array([tip[0], tip[1], 0.0]), hd + rng.uniform(-0.5, 0.5),
                               rng.uniform(0.8, 1.8), 0.012)
    for _ in range(broken):     # разлом там, где корни выходят из земли
        tubes += gen_crack(rng, np.array([0.0, 0.0, 0.0]), rng.uniform(0, 2 * math.pi), rng.uniform(0.5, 1.3), 0.011)
    return {"tubes": tubes, "core": None, "top": None, "heart": False}


def build_wrap_scene(seed, R):
    """Обвивка ствола радиуса R: корни подползают по земле и спиралью лезут вверх, по коре светящиеся жилы.
    Начало координат — центр ствола на уровне земли. Скрипт сердца ставит её на дерево и подгоняет SetScale."""
    rng = np.random.default_rng(seed)
    tubes = []
    n = int(rng.integers(3, 5))
    base = rng.uniform(0, 2 * math.pi)
    rt0 = 0.045 * math.sqrt(R / 0.2) + 0.012
    for i in range(n):
        a0 = base + i * 2 * math.pi / n + rng.uniform(-0.4, 0.4)
        H = rng.uniform(2.2, 3.4)
        turns = rng.uniform(0.35, 0.9) * rng.choice([-1, 1])
        d_out = rng.uniform(0.6, 1.3)
        # по земле к стволу
        ng = max(6, int(d_out / DS))
        g = np.linspace(0, 1, ng)
        ga = a0 + 0.25 * np.sin(g * 3 + i)
        gr = R + rt0 * 0.5 + d_out * (1 - g)
        ground = np.column_stack([gr * np.cos(ga), gr * np.sin(ga), np.full(ng, SINK * rt0)])
        # спираль вверх
        nh = max(10, int(H / DS))
        h = np.linspace(0, 1, nh)
        ang = a0 + turns * 2 * math.pi * h + 0.15 * np.sin(h * 9 + i)
        rr = R + rt0 * 0.55 * (1 - 0.5 * h)
        z = SINK * rt0 + H * h ** 0.95
        up = np.column_stack([rr * np.cos(ang), rr * np.sin(ang), z])
        pts = np.vstack([ground, up[1:]])
        s = np.concatenate([[0], np.cumsum(np.linalg.norm(np.diff(pts, axis=0), axis=1))])
        t = s / s[-1]
        r = rt0 * (1 - 0.75 * t ** 1.2)
        tw = smoothstep(0.85, 1.0, t)
        r = r * (1 - tw) + 0.003 * tw
        tubes.append(Tube(pts, r, "root", lump=0.15, lump_freq=8.0, seed=seed * 10 + i))
        v = vein_along(pts, r, rng, 0.05, 0.012)
        if v is not None:
            tubes.append(Tube(v[0], v[1], "vein", lump=0.08, lump_freq=12.0, seed=seed * 10 + 50 + i))
    # тонкие светящиеся жилы прямо по коре
    for j in range(int(rng.integers(2, 4))):
        a0 = rng.uniform(0, 2 * math.pi)
        H = rng.uniform(1.6, 3.0)
        nh = max(10, int(H / DS))
        h = np.linspace(0, 1, nh)
        ang = a0 + rng.uniform(-0.5, 0.5) * 2 * math.pi * h + 0.2 * np.sin(h * 13 + j)
        rr = R + 0.006
        pts = np.column_stack([rr * np.cos(ang), rr * np.sin(ang), 0.05 + H * h])
        rv = 0.012 * (1 - 0.6 * h) + 0.003
        tubes.append(Tube(pts, rv, "vein", lump=0.1, lump_freq=14.0, seed=seed * 10 + 80 + j))
    return {"tubes": tubes, "core": None, "top": None, "heart": False, "no_cut": True,
            "handle": ((-0.05, -0.05, -0.3), (0.05, 0.05, 0.0))}


def build_chunk_scene(seed, size=0.16):
    """Предмет «Сердце Изнанки»: бугристый сгусток, обвитый жилами, со светящимся «глазом».
    Строится в масштабе большого сердца (там правильный шаг сетки трубок) и уменьшается до size метров."""
    rng = np.random.default_rng(seed)
    tubes = []
    for k in range(6):
        u = np.linspace(0, 1, 60)
        th = 0.25 + 2.6 * u
        ph = rng.uniform(0, 2 * math.pi) + rng.uniform(0.6, 1.4) * np.sin(u * 3 + k)
        dirs = np.column_stack([np.sin(th) * np.cos(ph), np.sin(th) * np.sin(ph), np.cos(th)])
        rv = 0.035 - 0.012 * np.abs(u - 0.5)
        pts = heart_surface(dirs) + dirs * (rv * 0.35)[:, None]
        tubes.append(Tube(pts, rv, "vein", lump=0.08, lump_freq=9.0, seed=900 + k))
    top = heart_surface(np.array([[0.0, 0.0, 1.0]]))[0]
    extent = 2 * float(HEART_R.max()) * 1.16
    return {"tubes": tubes, "core": None, "top": None, "heart": False, "no_cut": True,
            "blobs": [heart_surface], "bulbs": [(top - np.array([0, 0, 0.04]), 0.13)],
            "scale": size / extent, "center": HEART_C.copy()}


def build_rootpiece_scene(seed, size=0.28):
    """Предмет «Корень Изнанки»: обломок корня с парой жил (выпадает из зомби Изнанки).
    Строится в масштабе корней на земле (шаг DS, радиус ~0.1 м) и уменьшается до size метров в длину."""
    rng = np.random.default_rng(seed)
    length = 1.1
    n = int(length / DS) + 1
    u = np.linspace(0, 1, n)
    ph1, ph2 = rng.uniform(0, 2 * math.pi, 2)
    pts = np.column_stack([u * length - length / 2,
                           0.07 * np.sin(u * 4.5 + ph1),
                           0.12 + 0.05 * np.sin(u * 3.3 + ph2)])
    radii = 0.105 - 0.035 * u
    tubes = [Tube(pts, radii, "root", lump=0.12, lump_freq=7.0, seed=seed)]
    for k in range(2):
        vein = vein_along(pts, radii, rng, 0.0, 0.02)
        if vein:
            tubes.append(Tube(vein[0], vein[1], "vein", seed=seed + 10 + k))
    return {"tubes": tubes, "core": None, "top": None, "heart": False, "no_cut": True,
            "scale": size / length, "center": np.array([0.0, 0.0, 0.12]), "item_mass": 0.3}


def build_spores_scene(seed, size=0.11):
    """Предмет «Споры Изнанки»: маленький тугой мешочек, оплетённый светящимися жилами (собирают у сердца)."""
    sc = build_chunk_scene(seed, size)
    sc["item_mass"] = 0.08
    return sc


def build_vein_coil_scene(seed, size=0.13):
    """Предмет «Жила Изнанки»: смотанная в моток светящаяся жила с парой свободных концов.
    Строится в масштабе корней (правильный шаг сетки трубок) и уменьшается до size метров."""
    rng = np.random.default_rng(seed)
    tubes = []
    R = 0.55
    turns = 3.3
    n = int(turns * 2 * math.pi * R / DS)
    t = np.linspace(0, 1, n)
    ang = t * turns * 2 * math.pi + rng.uniform(0, 6.28)
    rr = R * (1 + 0.07 * np.sin(t * 17 + seed))
    z = 0.06 + 0.12 * t + 0.03 * np.sin(ang * 2)
    pts = np.column_stack([rr * np.cos(ang), rr * np.sin(ang), z])
    r = 0.05 * (1 - 0.25 * t)
    tubes.append(Tube(pts, r, "vein", lump=0.12, lump_freq=8.0, seed=seed))
    # второй виток потоньше: «корневая» нить с жилой
    pts2 = np.column_stack([rr * 0.82 * np.cos(ang + 0.6), rr * 0.82 * np.sin(ang + 0.6), z * 0.8 + 0.02])
    r2 = 0.03 * (1 - 0.3 * t)
    tubes.append(Tube(pts2, r2, "root", lump=0.12, lump_freq=9.0, seed=seed + 1))
    # свободные концы свисают с мотка
    for k in range(2):
        a0 = ang[-1] if k == 0 else ang[0]
        p0 = pts[-1] if k == 0 else pts[0]
        m = 18
        u = np.linspace(0, 1, m)
        dirv = np.array([math.cos(a0 + 1.4), math.sin(a0 + 1.4)])
        tail = np.column_stack([p0[0] + dirv[0] * 0.35 * u, p0[1] + dirv[1] * 0.35 * u, p0[2] * (1 - u) + 0.04 * u])
        rt = 0.04 * (1 - 0.85 * u) + 0.004
        tubes.append(Tube(tail, rt, "vein", lump=0.1, lump_freq=10.0, seed=seed + 5 + k, cap_start=False))
    return {"tubes": tubes, "core": None, "top": None, "heart": False, "no_cut": True,
            "scale": size / (2 * R + 0.4), "center": np.array([0.0, 0.0, 0.1]), "item_mass": 0.05}


def _small_heart(k):
    """Поверхность сердца, уменьшенная в k раз вокруг его центра (для фонаря)."""
    return lambda d: HEART_C + (heart_surface(d) - HEART_C) * k


def _equator_vein(seed, z_shift=0.0, r=0.03):
    """Жила-обвязка по «экватору» сердца."""
    ph = np.linspace(0, 2 * math.pi, 90)
    th = math.pi / 2 + 0.12 * np.sin(ph * 3 + seed)
    dirs = np.column_stack([np.sin(th) * np.cos(ph), np.sin(th) * np.sin(ph), np.cos(th)])
    pts = heart_surface(dirs) + dirs * (r * 0.4) + np.array([0, 0, z_shift])
    return Tube(pts, np.full(len(pts), r), "vein", lump=0.1, lump_freq=12.0, seed=seed, cap_start=False, cap_end=False)


def build_listen_root_scene(seed, size=0.24):
    """Предмет «Слушающий корень»: обломок корня, конец которого закручен в раструб-«ухо», по нему жилы.
    Строится в масштабе корней на земле и уменьшается до size метров."""
    rng = np.random.default_rng(seed)
    L = 0.8
    n = int(L / DS) + 1
    u = np.linspace(0, 1, n)
    stem = np.column_stack([u * L - 0.7, 0.05 * np.sin(u * 4 + 1.0), 0.14 + 0.03 * np.sin(u * 3)])
    turns = 1.6
    ns = int(turns * 2 * math.pi * 0.2 / DS)
    t = np.linspace(0, 1, ns)
    R = 0.32 * (1 - t) + 0.06 * t
    ang = -math.pi / 2 + t * turns * 2 * math.pi
    cx, cz = stem[-1, 0], stem[-1, 2] + 0.32
    sp = np.column_stack([cx + R * np.cos(ang), stem[-1, 1] + 0.12 * t, cz + R * np.sin(ang)])
    pts = np.vstack([stem, sp[1:]])
    radii = np.concatenate([0.1 - 0.015 * u, 0.085 * (1 - t[1:]) + 0.022 * t[1:]])
    tubes = [Tube(pts, radii, "root", lump=0.12, lump_freq=7.0, seed=seed)]
    for k in range(3):
        vein = vein_along(pts, radii, rng, 0.0, 0.018)
        if vein:
            tubes.append(Tube(vein[0], vein[1], "vein", seed=seed + 10 + k))
    lo, hi = pts.min(0), pts.max(0)
    return {"tubes": tubes, "core": None, "top": None, "heart": False, "no_cut": True,
            "scale": size / float(np.max(hi - lo) + 0.2), "center": (lo + hi) / 2, "item_mass": 0.25}


def build_heart_battery_scene(seed, size=0.26):
    """Предмет «Сердце-аккумулятор»: сердце, перетянутое жилой, с двумя отростками-«клеммами» сверху."""
    sc = build_chunk_scene(seed, size)
    for k, side in enumerate((-1, 1)):
        d = np.array([0.45 * side, 0.15, 0.88])
        d /= np.linalg.norm(d)
        p0 = heart_surface(d[None, :])[0] - d * 0.05
        u = np.linspace(0, 1, 26)
        pts = p0[None, :] + np.column_stack([side * 0.22 * u ** 2, 0.05 * np.sin(u * 3 + k), 0.34 * u - 0.1 * u ** 3])
        r = 0.075 * (1 - 0.45 * u)
        sc["tubes"].append(Tube(pts, r, "root", lump=0.12, lump_freq=7.0, seed=seed * 10 + k))
        sc["tubes"].append(Tube(pts + np.array([0, 0, 0.004]), r * 0.35, "vein", lump=0.1, lump_freq=10.0,
                                seed=seed * 10 + 5 + k))
    sc["tubes"].append(_equator_vein(seed, 0.0, 0.035))
    sc["tubes"].append(_equator_vein(seed + 1, -0.18, 0.028))
    sc["item_mass"] = 3.0
    return sc


def build_spore_trap_scene(seed, size=0.15):
    """Предмет «Споровая ловушка»: мешочек спор с торчащей «трубкой»-отростком сбоку."""
    sc = build_chunk_scene(seed, size)
    d = np.array([1.0, 0.0, 0.25])
    d /= np.linalg.norm(d)
    p0 = heart_surface(d[None, :])[0] - d * 0.06
    u = np.linspace(0, 1, 24)
    pts = p0[None, :] + np.column_stack([0.5 * u, 0.06 * np.sin(u * 4), 0.12 * u ** 2])
    sc["tubes"].append(Tube(pts, np.full(len(u), 0.07) * (1 - 0.2 * u), "root", lump=0.04, lump_freq=6.0, seed=seed * 10))
    sc["tubes"].append(_equator_vein(seed, 0.05, 0.03))
    sc["item_mass"] = 0.3
    return sc


def build_fuel_capsule_scene(seed, size=0.32):
    """Предмет «Топливная капсула»: толстый корневой стручок с закрытыми концами, обвитый тремя жилами."""
    L = 1.2
    n = int(L / DS) + 1
    u = np.linspace(0, 1, n)
    axis = np.column_stack([u * L - L / 2, 0.04 * np.sin(u * 3 + seed), 0.3 + 0.03 * np.sin(u * 2.5)])
    r = 0.24 * np.sin(math.pi * u) ** 0.55 + 0.02
    tubes = [Tube(axis, r, "root", lump=0.06, lump_freq=5.0, seed=seed)]
    for k in range(3):
        a = u * 4 * math.pi + k * 2 * math.pi / 3
        rr = r + 0.012
        pts = axis + np.column_stack([np.zeros(n), rr * np.cos(a), rr * np.sin(a)])
        tubes.append(Tube(pts[3:-3], np.full(n - 6, 0.022), "vein", lump=0.1, lump_freq=12.0, seed=seed + 1 + k))
    return {"tubes": tubes, "core": None, "top": None, "heart": False, "no_cut": True,
            "scale": size / (L + 0.05), "center": np.array([0.0, 0.0, 0.3]), "item_mass": 2.0}


def build_lantern_scene(seed, size=0.26):
    """Предмет «Фонарь-сердце»: маленькое сердце в клетке из корней, сверху петля-ручка."""
    rng = np.random.default_rng(seed)
    tubes = []
    cage = HEART_R * 1.05
    for k in range(5):
        ph = k * 2 * math.pi / 5 + rng.uniform(-0.15, 0.15)
        th = np.linspace(0.12 * math.pi, 0.9 * math.pi, 40)
        dirs = np.column_stack([np.sin(th) * np.cos(ph), np.sin(th) * np.sin(ph), np.cos(th)])
        pts = HEART_C + dirs * cage
        tubes.append(Tube(pts, np.full(len(th), 0.045), "root", lump=0.12, lump_freq=7.0, seed=seed * 10 + k))
    for zc, rad in ((math.cos(0.12 * math.pi), math.sin(0.12 * math.pi)), (math.cos(0.9 * math.pi), math.sin(0.9 * math.pi))):
        ph = np.linspace(0, 2 * math.pi, 60)
        pts = HEART_C + np.column_stack([cage[0] * rad * np.cos(ph), cage[1] * rad * np.sin(ph), np.full(60, cage[2] * zc)])
        tubes.append(Tube(pts, np.full(60, 0.05), "root", lump=0.1, lump_freq=8.0, seed=seed * 10 + 20, cap_start=False, cap_end=False))
    top = HEART_C[2] + cage[2] * math.cos(0.12 * math.pi)
    a = np.linspace(0, math.pi, 40)
    hr = 0.28
    pts = np.column_stack([hr * np.cos(a), np.zeros(40), top + hr * np.sin(a)])
    tubes.append(Tube(pts, np.full(40, 0.05), "root", lump=0.1, lump_freq=8.0, seed=seed * 10 + 30))
    tubes.append(Tube(pts + np.array([0, 0.03, 0.02]), np.full(40, 0.018), "vein", lump=0.1, lump_freq=12.0, seed=seed * 10 + 31))
    small = _small_heart(0.6)
    top_c = small(np.array([[0.0, 0.0, 1.0]]))[0]
    height = top + hr - (HEART_C[2] - cage[2])
    return {"tubes": tubes, "core": None, "top": None, "heart": False, "no_cut": True,
            "blobs": [small], "bulbs": [(top_c - np.array([0, 0, 0.03]), 0.09)],
            "scale": size / (height + 0.1), "center": HEART_C.copy(), "item_mass": 0.6}


# ---------------------------------------------------------------- предметы с «чужими» материалами
# Строятся в масштабе x10 (1 единица = 10 см, как шаг трубок у корней), потом scale 0.1.

def _tileable(n, rng, beta=2.0):
    return spectral(n, rng, beta=beta)


def make_prop_textures(png_dir):
    """Плиточные текстуры 256x256 для дерева, ткани, пластика, металла, кости, стекла, жидкости + rvmat."""
    n = 256
    rng = np.random.default_rng(4242)
    yy, xx = np.mgrid[0:n, 0:n] / n
    out = []

    def save(name, rgb, alpha=None):
        rgb = np.clip(rgb, 0, 1)
        if alpha is not None:
            arr = np.dstack([rgb, np.clip(alpha, 0, 1)[..., None]])
        else:
            arr = rgb
        path = os.path.join(png_dir, name + ".png")
        write_png(path, (arr * 255).astype(np.uint8))
        out.append((name, path))

    def col(c, k):
        return np.asarray(c, float)[None, None, :] * k[..., None]

    nz = _tileable(n, rng, 1.6)
    grain = np.sin((xx * 3 + nz * 0.08) * 2 * math.pi * 9) * 0.5 + 0.5
    save("vb_rift_p_wood_co", col((0.36, 0.22, 0.12), 0.75 + 0.25 * grain + 0.06 * nz))
    weave = (np.sin(xx * 2 * math.pi * 48) * np.sin(yy * 2 * math.pi * 48)) * 0.5 + 0.5
    dirt = _tileable(n, rng, 2.2)
    save("vb_rift_p_cloth_co", col((0.34, 0.30, 0.24), 0.82 + 0.1 * weave + 0.1 * dirt))
    # корпус аккумулятора и запал ловушки: бурый, как перекраски Изнанки (решение пользователя 11-12)
    save("vb_rift_p_plastic_co", col((0.30, 0.14, 0.075), 0.9 + 0.12 * _tileable(n, rng, 2.5)))
    brushed = _tileable(n, rng, 1.2)
    brushed = np.roll(np.repeat(brushed[:, :1], n, axis=1), 0, axis=0) * 0.5 + _tileable(n, rng, 2.4) * 0.15
    save("vb_rift_p_metal_co", col((0.55, 0.55, 0.56), 0.9 + 0.12 * brushed))
    pores = (_tileable(n, rng, 0.8) > 1.6).astype(float)
    save("vb_rift_p_bone_co", col((0.84, 0.79, 0.66), 0.95 + 0.06 * _tileable(n, rng, 2.0) - 0.25 * pores))
    save("vb_rift_p_glass_ca", col((0.78, 0.86, 0.86), np.ones((n, n))), np.full((n, n), 0.28))
    swirl = _tileable(n, rng, 1.8)
    save("vb_rift_p_liquid_co", col((0.55, 0.03, 0.03), 0.8 + 0.25 * swirl))
    # сталь «Голодного клинка»: бурая, по ней сеть светящихся жил (+ маска свечения)
    r1 = _tileable(n, rng, 2.4)
    r2 = _tileable(n, rng, 2.8)
    m = np.clip(np.exp(-(r1 / 0.09) ** 2) + 0.6 * np.exp(-(r2 / 0.07) ** 2), 0, 1)
    steel = col((0.36, 0.17, 0.09), 0.85 + 0.15 * _tileable(n, rng, 2.0))
    vein = np.asarray((0.95, 0.12, 0.10))[None, None, :]
    save("vb_rift_p_bladesteel_co", steel * (1 - m[..., None]) + vein * m[..., None])
    save("vb_rift_p_bladesteel_e_co", vein * m[..., None])
    # мешковина «Спор Изнанки»: грубое плетение, нитки неровной толщины, бурые пятна
    nx = _tileable(n, rng, 2.6)
    tx = np.sin(xx * 2 * math.pi * 40 + nx * 0.5) * 0.5 + 0.5
    ty = np.sin(yy * 2 * math.pi * 40 + nx * 0.5) * 0.5 + 0.5
    coarse = np.where((np.floor(xx * 40) + np.floor(yy * 40)) % 2 == 0, tx, ty)
    stain = _tileable(n, rng, 2.0)
    burlap = col((0.50, 0.33, 0.17), 0.66 + 0.26 * coarse + 0.08 * stain)
    save("vb_rift_p_burlap_co", burlap)
    # мешковина, пропитанная спорами: светящиеся пятна и точки проступают сквозь плетение (+ маска свечения)
    sp = _tileable(n, rng, 1.6)
    sp2 = _tileable(n, rng, 0.6)
    spots = np.clip((sp - 1.9) * 2.5, 0, 1) + np.clip((sp2 - 2.3) * 3.0, 0, 1)
    spots = np.clip(spots * (0.55 + 0.45 * coarse), 0, 1)
    glow = np.asarray((1.0, 0.22, 0.16))[None, None, :]
    save("vb_rift_p_sporecloth_co", burlap * (1 - spots[..., None]) + glow * spots[..., None])
    save("vb_rift_p_sporecloth_e_co", glow * spots[..., None])
    # кора корней на предметах (аккумулятор): бурая, продольные борозды вдоль корня (v — вдоль трубки), без свечения
    low = _tileable(n, rng, 2.4)
    ridge = np.sin(xx * 2 * math.pi * 7 + low * 1.2) * 0.5 + 0.5
    ridge = ridge ** 0.6
    save("vb_rift_p_bark_co", col((0.34, 0.19, 0.09), 0.55 + 0.4 * ridge + 0.1 * _tileable(n, rng, 1.8)))
    return out


def write_prop_materials():
    flat_n = "#(argb,8,8,3)color(0.5,0.5,1,1,NOHQ)"
    black = "#(argb,8,8,3)color(0,0,0,1,CO)"
    fres = "#(ai,64,64,1)fresnel(1.3,0.3)"
    cfg = {
        "wood": (0.0, "#(argb,8,8,3)color(0.3,0.2,0.3,1,SMDI)", black, 0.1, 20),
        "cloth": (0.0, "#(argb,8,8,3)color(0.2,0.1,0.2,1,SMDI)", black, 0.05, 10),
        "plastic": (0.0, "#(argb,8,8,3)color(0.6,0.4,0.6,1,SMDI)", black, 0.35, 60),
        "metal": (0.0, "#(argb,8,8,3)color(1,0.8,0.9,1,SMDI)", black, 0.8, 120),
        "bone": (0.0, "#(argb,8,8,3)color(0.5,0.3,0.4,1,SMDI)", black, 0.2, 30),
        "glass": (0.0, "#(argb,8,8,3)color(1,1,1,1,SMDI)", black, 1.0, 200),
        "liquid": (1.6, "#(argb,8,8,3)color(1,0.6,0.7,1,SMDI)", PREFIX + "vb_rift_p_liquid_co.paa", 0.6, 90),
        "bark": (0.0, "#(argb,8,8,3)color(0.2,0.15,0.2,1,SMDI)", black, 0.06, 12),
        "burlap": (0.0, "#(argb,8,8,3)color(0.15,0.1,0.15,1,SMDI)", black, 0.04, 8),
        "sporecloth": (1.5, "#(argb,8,8,3)color(0.15,0.1,0.15,1,SMDI)", PREFIX + "vb_rift_p_sporecloth_e_co.paa", 0.04, 8),
        "bladesteel": (1.4, "#(argb,8,8,3)color(1,0.6,0.8,1,SMDI)", PREFIX + "vb_rift_p_bladesteel_e_co.paa", 0.7, 110),
    }
    for m, (e, smdi, etex, spec, power) in cfg.items():
        with open(os.path.join(DATA_DIR, "vb_rift_p_%s.rvmat" % m), "w", encoding="utf-8", newline="\r\n") as f:
            f.write(rvmat(e, flat_n, smdi, etex, spec, power, fres))


def _helix(c0, c1, radius, turns, n_per_turn=48, phase=0.0, axis=2):
    """Спираль вокруг оси axis от c0 до c1 (координаты по оси), радиус может быть массивом."""
    n = int(turns * n_per_turn) + 2
    t = np.linspace(0, 1, n)
    a = phase + t * turns * 2 * math.pi
    r = np.linspace(radius[0], radius[1], n) if isinstance(radius, tuple) else np.full(n, float(radius))
    pos = c0 + (c1 - c0) * t
    pts = np.zeros((n, 3))
    o = [k for k in range(3) if k != axis]
    pts[:, axis] = pos
    pts[:, o[0]] = r * np.cos(a)
    pts[:, o[1]] = r * np.sin(a)
    return pts


def _lathe_z(profile, mat, segs=16, uv_len=0.6, x=0.0, y=0.0, seed=0, lump=0.0, cap_start=True, cap_end=True):
    """Тело вращения вокруг вертикали: profile = [(z, r), ...] (резкие ступени сохраняются)."""
    pr = np.asarray(profile, float)
    pts = np.column_stack([np.full(len(pr), x), np.full(len(pr), y), pr[:, 0]])
    return Tube(pts, pr[:, 1], "root", lump=lump, lump_freq=5.0, seed=seed, mat=mat, segs=segs, uv_len=uv_len,
                cap_flat=True, raw=True, cap_start=cap_start, cap_end=cap_end)


def _dense_profile(zr, step=0.04):
    """Профиль с промежуточными точками (плавные участки), но ступени (одинаковый z) остаются ступенями."""
    out = [zr[0]]
    for (z0, r0), (z1, r1) in zip(zr[:-1], zr[1:]):
        k = max(1, int(abs(z1 - z0) / step))
        for i in range(1, k + 1):
            out.append((z0 + (z1 - z0) * i / k, r0 + (r1 - r0) * i / k))
    return out


def _item(tubes, blobs=None, bulbs=None, mass=0.1, center=(0, 0, 0)):
    return {"tubes": tubes, "core": None, "top": None, "heart": False, "no_cut": True,
            "blobs": blobs or [], "bulbs": bulbs or [], "scale": 0.1, "center": np.asarray(center, float),
            "item_mass": mass}


def _sack_mesh(seed, prof, pleats=9, outer=True, z_min=None):
    """Мешок из ткани: кольца по высоте, радиус = профиль x (складки + шум); складки сильнее к горловине,
    верхний край рваный. outer=False — изнанка горловины (грани внутрь, на 10 % уже, верхнее кольцо общее).
    Возвращает точки, треугольники, UV и число точек в кольце."""
    rng = np.random.default_rng(seed)
    zs_all = np.array([a for a, _ in prof])
    rs_all = np.array([b for _, b in prof])
    keep = zs_all >= (z_min if z_min is not None else -1e9)
    zs, rs = zs_all[keep], rs_all[keep]
    m = 30
    th = np.linspace(0, 2 * math.pi, m, endpoint=False)
    ph = rng.uniform(0, 6.28, 5)
    zmax = zs_all[-1]
    rag = 0.035 * np.sin(7 * th + ph[3]) + 0.025 * np.sin(12 * th + ph[4]) + 0.02 * np.sin(3 * th + ph[0])
    rings = []
    for z, r in zip(zs, rs):
        k = np.clip((z - 0.25) / (zmax - 0.25), 0, 1) ** 1.2
        fold = 1 + (0.03 + 0.24 * k) * np.sin(pleats * th + ph[0] + 1.5 * k) + 0.05 * np.sin(4 * th + ph[1] + z * 3)
        lean = np.array([0.035 * math.sin(z * 2.5 + ph[2]), 0.03 * math.cos(z * 2 + ph[2])])
        top = z >= zmax - 1e-9
        rr = r * fold * (1.0 if (outer or top) else 0.9)
        zz = z + rag * np.clip((z - (zmax - 0.12)) / 0.12, 0, 1)
        rings.append(np.column_stack([rr * np.cos(th) + lean[0], rr * np.sin(th) + lean[1], zz]))
    P = np.vstack(rings)
    nz = len(zs)
    tris, uvs = [], []

    def add(a, b, c, ua, ub, uc, out):
        if np.dot(np.cross(P[b] - P[a], P[c] - P[a]), out) < 0:
            b, c, ub, uc = c, b, uc, ub
        tris.append((a, b, c))
        uvs.append((ua, ub, uc))

    for i in range(nz - 1):
        cen = np.mean(np.vstack([rings[i], rings[i + 1]]), axis=0)
        for j in range(m):
            a, b = i * m + j, i * m + (j + 1) % m
            c, d = (i + 1) * m + (j + 1) % m, (i + 1) * m + j
            u0, u1 = j / m * 3, (j + 1) / m * 3
            v0, v1 = zs[i] / 0.9, zs[i + 1] / 0.9
            out = (P[a] + P[c]) / 2 - cen
            out[2] = 0
            if not outer:
                out = -out
            add(a, b, c, (u0, v0), (u1, v0), (u1, v1), out)
            add(a, c, d, (u0, v0), (u1, v1), (u0, v1), out)
    if outer:
        bi = len(P)
        P = np.vstack([P, [np.append(np.mean(rings[0][:, :2], axis=0), zs[0] - 0.01)]])
        for j in range(m):
            add(j, (j + 1) % m, bi, (j / m, 0), ((j + 1) / m, 0), (0.5, 0.15), np.array([0, 0, -1.0]))
    return P, tris, uvs, m


def build_spores_sack_scene(seed):
    """«Споры Изнанки»: мятый мешочек из мешковины, пропитанной спорами (светящиеся пятна сквозь плетение),
    с вертикальными складками; горловина стянута шнурком и жилой, верх — рваная оборка, изнутри светится
    комок спор, сквозь ткань пробились гроздья спор. Высота ~12 см, ширина ~10 см (масштаб x10)."""
    rng = np.random.default_rng(seed)
    prof = [(0.0, 0.16), (0.03, 0.30), (0.1, 0.39), (0.25, 0.45), (0.42, 0.46), (0.58, 0.40), (0.72, 0.27),
            (0.82, 0.15), (0.87, 0.12), (0.93, 0.15), (1.02, 0.23), (1.11, 0.28), (1.16, 0.27)]
    prof = _dense_profile(prof, 0.03)
    P, tris, uvs, m = _sack_mesh(seed, prof, outer=True)
    Pi, trisi, uvsi, _ = _sack_mesh(seed, prof, outer=False, z_min=0.9)
    meshes = [(P, tris, uvs, "sporecloth"), (Pi, trisi, uvsi, "burlap")]
    tubes = []
    # шнурок из мешковины, поверх него жила
    ang = np.linspace(0, 2 * math.pi, 49)[:-1]
    for k, (r, z, mat, rad) in enumerate(((0.14, 0.86, "burlap", 0.028), (0.155, 0.905, None, 0.02))):
        ring = np.column_stack([r * np.cos(ang), r * np.sin(ang), z + 0.012 * np.sin(ang * 3 + k)])
        ring = np.vstack([ring, ring[:1]])
        tubes.append(Tube(ring, np.full(len(ring), rad), "vein", lump=0.12, lump_freq=12, seed=seed + 1 + k, mat=mat,
                          segs=7 if mat else None, cap_start=False, cap_end=False))
    # два хвоста шнурка от узла
    pz = [a for a, _ in prof]
    pr = [b for _, b in prof]
    for k, (a0, a1, z1) in enumerate(((0.25, 0.55, 0.5), (0.25, 0.0, 0.6))):
        zt = np.linspace(0.86, z1, 9)
        at = np.linspace(a0, a1, 9)
        rt = np.interp(zt, pz, pr) * 1.12 + 0.03
        tail = np.column_stack([rt * np.cos(at), rt * np.sin(at), zt])
        tubes.append(Tube(tail, np.linspace(0.026, 0.018, 9), "vein", mat="burlap", segs=7, seed=seed + 3 + k))
    # комок спор в горловине
    bulbs = [(np.array([0.0, 0.0, 0.99]), 0.15), (np.array([0.06, 0.04, 1.05]), 0.08),
             (np.array([-0.05, -0.05, 1.04]), 0.07), (np.array([0.01, -0.07, 1.01]), 0.07)]
    # гроздья спор, пробившихся сквозь ткань: точно на поверхности (точки колец мешка)
    nz = (len(P) - 1) // m
    zring = np.array([P[i * m, 2] for i in range(nz)])
    rows = np.nonzero((zring > 0.12) & (zring < 0.62))[0]
    for c in range(5):
        i0 = int(rng.choice(rows))
        j0 = int(rng.integers(0, m))
        for t in range(int(rng.integers(3, 6))):
            i = int(np.clip(i0 + rng.integers(-1, 2), rows[0], rows[-1]))
            j = (j0 + int(rng.integers(-1, 2))) % m
            bulbs.append((P[i * m + j].copy(), rng.uniform(0.016, 0.032)))
    sc = _item(tubes, bulbs=bulbs, mass=0.08, center=(0, 0, 0.58))
    sc["meshes"] = meshes
    return sc
def build_vein_skein_scene(seed):
    """«Жила Изнанки»: светящаяся жила, смотанная на деревянную палочку."""
    stick = Tube(np.array([[-0.75, 0, 0], [0.75, 0, 0]]), np.array([0.055, 0.05]), "root", mat="wood", segs=10,
                 uv_len=0.8, seed=seed)
    tubes = [stick]
    for layer, (r, turns) in enumerate(((0.10, 9), (0.14, 8), (0.18, 7))):
        h = _helix(-0.38 + layer * 0.04, 0.38 - layer * 0.04, r, turns, 40, phase=layer * 1.3, axis=0)
        tubes.append(Tube(h, np.full(len(h), 0.024), "vein", lump=0.08, lump_freq=14, seed=seed + layer + 1,
                          cap_start=False, cap_end=False))
    end = np.array([[0.34, 0.0, 0.18], [0.45, 0.05, 0.26], [0.55, 0.0, 0.33]])
    tubes.append(Tube(end, np.array([0.024, 0.016, 0.008]), "vein", seed=seed + 9))
    return _item(tubes, mass=0.05)


def build_heart_battery2_scene(seed):
    """«Сердце-аккумулятор»: корпус автомобильного аккумулятора, из крышки прорастает сердце, корни
    оплетают корпус и клеммы."""
    rng = np.random.default_rng(seed)
    L, W, H = 2.4, 1.7, 1.9
    r_box = (H / 2) * math.sqrt(2)
    body = Tube(np.array([[-L / 2, 0, H / 2], [L / 2, 0, H / 2]]), np.array([r_box, r_box]), "root", mat="plastic",
                segs=4, fixed_segs=True, uv_len=2.0, cap_flat=True, raw=True, ang0=math.pi / 4, aspect=W / H)
    tubes = [body]
    for sx in (-0.8, 0.8):
        tubes.append(_lathe_z([(H - 0.02, 0.14), (H + 0.12, 0.14), (H + 0.16, 0.10)], "metal", segs=12,
                              uv_len=0.3, x=sx, y=0.45))
    top = H + 0.05
    heart_c = np.array([0.0, -0.1, top + 0.35])
    small = lambda d: (heart_surface(d) - HEART_C) * 0.75 + heart_c
    # корни: от сердца через ребро крышки вниз по бокам
    for k, (x0, side) in enumerate(((-0.5, 1), (0.3, 1), (-0.2, -1), (0.6, -1), (-0.85, -1))):
        yE = side * (W / 2 + 0.04)
        pts = np.array([[x0 * 0.4, heart_c[1] + side * 0.25, top + 0.15], [x0, side * (W / 2 - 0.15), top + 0.03],
                        [x0 + 0.05, yE, top - 0.15], [x0 + rng.uniform(-0.15, 0.15), yE, H * 0.5],
                        [x0 + rng.uniform(-0.2, 0.2), yE + side * 0.02, 0.15], [x0, yE + side * 0.25, 0.02]])
        rr = np.linspace(0.085, 0.04, len(pts))
        tubes.append(Tube(pts, rr, "root", lump=0.14, lump_freq=7, seed=seed * 10 + k))
        v = vein_along(pts, rr, rng, 0.0, 0.015)
        if v:
            tubes.append(Tube(v[0], v[1], "vein", seed=seed * 10 + 30 + k))
    # жилы-провода от клемм к сердцу
    for k, sx in enumerate((-0.8, 0.8)):
        pts = np.array([[sx, 0.45, H + 0.16], [sx * 0.8, 0.35, H + 0.45], [sx * 0.4, 0.1, top + 0.55], [0.0, -0.05, top + 0.5]])
        tubes.append(Tube(pts, np.full(4, 0.035), "vein", lump=0.1, lump_freq=10, seed=seed * 10 + 60 + k))
    eye = small(np.array([[0.0, 0.0, 1.0]]))[0]
    return _item(tubes, blobs=[small], bulbs=[(eye - np.array([0, 0, 0.03]), 0.1)], mass=3.0, center=(0, 0, 0))


def build_trap_grenade_scene(seed):
    """«Споровая ловушка»: тугой мешочек спор, перетянутый жилами крест-накрест, сверху трубка-запал и кольцо-чека."""
    rng = np.random.default_rng(seed)
    zr = [(0.0, 0.05), (0.05, 0.26), (0.2, 0.38), (0.42, 0.40), (0.62, 0.30), (0.74, 0.14), (0.78, 0.1)]
    sack = _lathe_z(_dense_profile(zr, 0.03), "cloth", segs=18, uv_len=0.5, lump=0.06, seed=seed)
    tubes = [sack]
    for k in range(2):
        a = k * math.pi / 2
        t = np.linspace(0.08, math.pi - 0.08, 30)
        rr = np.interp(0.4 - 0.36 * np.cos(t), [a_ for a_, _ in zr], [b for _, b in zr]) + 0.02
        z = 0.4 - 0.36 * np.cos(t)
        pts = np.column_stack([rr * np.cos(a) * np.sign(np.cos(t + 1e-9)) * 0 + rr * np.cos(a), rr * np.sin(a), z])
        pts2 = np.column_stack([-rr * np.cos(a), -rr * np.sin(a), z])
        loop = np.vstack([pts, pts2[::-1]])
        tubes.append(Tube(loop, np.full(len(loop), 0.03), "vein", lump=0.1, lump_freq=12, seed=seed + k))
    fuse = np.array([[0.0, 0.0, 0.74], [0.02, 0.0, 0.95], [0.1, 0.03, 1.1], [0.22, 0.05, 1.18]])
    tubes.append(Tube(fuse, np.full(4, 0.06), "root", mat="plastic", segs=10, uv_len=0.3, cap_flat=True, seed=seed + 5))
    ang = np.linspace(0, 2 * math.pi, 36)
    ring = np.column_stack([0.12 + 0.17 * np.cos(ang), 0.0 * ang - 0.08, 0.86 + 0.17 * np.sin(ang)])
    tubes.append(Tube(ring, np.full(36, 0.022), "vein", mat="metal", segs=8, cap_start=False, cap_end=False, seed=seed + 6))
    pin = np.array([[0.0, -0.08, 0.80], [0.03, -0.02, 0.80], [0.06, 0.06, 0.80]])
    tubes.append(Tube(pin, np.full(3, 0.018), "vein", mat="metal", segs=8, seed=seed + 7))
    bulbs = []
    for i in range(8):
        z = rng.uniform(0.1, 0.6)
        rr = np.interp(z, [a for a, _ in zr], [b for _, b in zr])
        a = rng.uniform(0, 2 * math.pi)
        bulbs.append((np.array([rr * math.cos(a), rr * math.sin(a), z]), rng.uniform(0.025, 0.04)))
    return _item(tubes, bulbs=bulbs, mass=0.3, center=(0, 0, 0.5))


def build_ampoule_scene(seed):
    """«Кровь Изнанки»: стеклянная ампула со светящейся кровью, пробка из корня, по стеклу ползут жилы."""
    rng = np.random.default_rng(seed)
    # прозрачность стекла у несжатой модели движок не включает: колба там, где кровь, светится красным,
    # горлышко — светлое стекло (ванильный материал)
    filled = [(0.0, 0.0), (0.0, 0.2), (0.02, 0.24), (0.06, 0.25), (0.62, 0.25)]
    tubes = [_lathe_z(_dense_profile(filled, 0.03), "liquid", segs=16, uv_len=0.5, cap_end=False)]
    neck = [(0.62, 0.25), (0.72, 0.25), (0.8, 0.2), (0.86, 0.11), (0.9, 0.1)]
    tubes.append(_lathe_z(_dense_profile(neck, 0.03), "glass", segs=16, uv_len=0.5, cap_start=False, cap_end=False))
    cork = [(0.86, 0.0), (0.86, 0.12), (0.9, 0.13), (1.02, 0.12), (1.06, 0.09), (1.07, 0.0)]
    tubes.append(_lathe_z(cork, "crust", segs=12, uv_len=0.4, lump=0.1, seed=seed))
    for k in range(2):
        h = _helix(0.08, 0.95, (0.27, 0.14), 1.6, 75, phase=k * math.pi + rng.uniform(0, 1), axis=2)
        tubes.append(Tube(h, np.linspace(0.022, 0.01, len(h)), "vein", lump=0.1, lump_freq=12, seed=seed + 3 + k))
    return _item(tubes, mass=0.06, center=(0, 0, 0.5))


def build_strap_scene(seed):
    """«Живой жгут»: плоский ремень из корня, свёрнутый в кольцо, с пряжкой из кости и светящейся жилой по ремню."""
    rng = np.random.default_rng(seed)
    t = np.linspace(0, 1.25, 120)
    a = t * 2 * math.pi
    r = 0.42 - 0.07 * t
    path = np.column_stack([r * np.cos(a), r * np.sin(a), 0.11 + 0.02 * t])
    # сечение: N = вертикаль (ширина ремня), B = радиально (толщина)
    tubes = [Tube(path, np.full(len(t), 0.11), "root", lump=0.05, lump_freq=8, seed=seed, segs=8, fixed_segs=True,
                  aspect=0.22, uv_len=0.6)]
    vein = path.copy()
    vein[:, 0] *= (r + 0.03) / r
    vein[:, 1] *= (r + 0.03) / r
    tubes.append(Tube(vein, np.full(len(t), 0.018), "vein", lump=0.12, lump_freq=14, seed=seed + 1, cap_start=False, cap_end=False))
    end = path[-1]
    ang = np.linspace(0, 2 * math.pi, 5)[:-1] + math.pi / 4
    sq = np.column_stack([end[0] + 0.0 * ang, end[1] + 0.13 * np.cos(ang), end[2] + 0.13 * np.sin(ang)])
    sq = np.vstack([sq, sq[:1]])
    tubes.append(Tube(sq, np.full(len(sq), 0.035), "vein", mat="bone", segs=6, cap_start=False, cap_end=False, seed=seed + 2))
    return _item(tubes, mass=0.04, center=(0, 0, 0.11))


def build_spool_scene(seed):
    """«Нить Изнанки»: деревянная катушка, плотно намотанная светящейся нитью, сбоку воткнута игла."""
    prof = [(0.0, 0.0), (0.0, 0.34), (0.07, 0.34), (0.07, 0.16), (0.63, 0.16), (0.63, 0.34), (0.70, 0.34), (0.70, 0.0)]
    tubes = [_lathe_z(prof, "wood", segs=20, uv_len=0.6)]
    for layer, r in enumerate((0.185, 0.22, 0.255)):
        h = _helix(0.09, 0.61, r, 16, 24, phase=layer * 0.7, axis=2)
        tubes.append(Tube(h, np.full(len(h), 0.018), "vein", seed=seed + layer, cap_start=False, cap_end=False,
                          segs=4, fixed_segs=True, uv_len=0.3))
    needle = np.array([[0.05, -0.32, 0.18], [0.0, 0.0, 0.42], [-0.05, 0.3, 0.66]])
    tubes.append(Tube(needle, np.array([0.012, 0.012, 0.004]), "vein", mat="metal", segs=6, seed=seed + 9))
    tail = np.array([[0.25, 0.05, 0.55], [0.32, 0.12, 0.62], [0.36, 0.25, 0.68]])
    tubes.append(Tube(tail, np.full(3, 0.012), "vein", segs=4, seed=seed + 10))
    return _item(tubes, mass=0.02, center=(0, 0, 0.35))


def build_blade_roots_scene(seed):
    """Корни на «Голодном клинке» (ванильный FangeKnife): обвивают рукоять и выползают на клинок.
    Оси модели ножа (замер ClippingInfo/GetCollisionBox на стенде 11-12, в м): X ширина клинка -0.039..0.050,
    Y вдоль ножа -0.085..0.343 (рукоять у начала), Z толщина -0.021..0.022. Здесь оси Blender: X=X, Z=Y DayZ, Y=Z DayZ.
    Масштаб x10. Начало координат = начало ножа: объект крепится к ножу без смещения."""
    rng = np.random.default_rng(seed)
    tubes = []
    cx = 0.05
    for k in range(2):
        h = _helix(-0.8, 0.55, 0.0, 3.4, 40, phase=k * math.pi, axis=2)
        a = np.arctan2(h[:, 1], h[:, 0] + 1e-9)
        t = np.linspace(0, 2 * math.pi * 3.2, len(h)) + k * math.pi
        h[:, 0] = cx + 0.24 * np.cos(t)
        h[:, 1] = 0.26 * np.sin(t)
        rr = np.full(len(h), 0.05)
        tubes.append(Tube(h, rr, "root", lump=0.15, lump_freq=9, seed=seed + k, cap_start=False))
        v = vein_along(h, rr, rng, 0.0, 0.012)
        if v:
            tubes.append(Tube(v[0], v[1], "vein", seed=seed + 10 + k))
    for k, side in enumerate((-1, 1)):
        z = np.linspace(0.5, 2.4, 30)
        x = 0.25 + 0.08 * np.sin(z * 4 + k) - 0.06 * (z - 0.5)
        y = side * (0.09 + 0.01 * np.sin(z * 7))
        pts = np.column_stack([x, y, z])
        rr = np.linspace(0.04, 0.008, len(z))
        tubes.append(Tube(pts, rr, "root", lump=0.12, lump_freq=10, seed=seed + 20 + k))
        tubes.append(Tube(pts + np.array([0, side * 0.03, 0]), rr * 0.35, "vein", seed=seed + 30 + k))
    sc = _item(tubes, mass=0.01, center=(0, 0, 0))
    sc["no_geom"] = True
    return sc


def _bowie_profile(t):
    """Клинок боуи (Blender x10, X = поперёк клинка): обух по +X прямой до 60 % длины, дальше скос к острию;
    кромка по -X с выпуклым брюшком. Возвращает x обуха, x кромки, полутолщину обуха."""
    x_tip = 0.09
    sc = np.clip((t - 0.62) / 0.38, 0, 1)
    x_s = np.where(t < 0.62, 0.22, 0.22 - (0.22 - x_tip) * (1 - (1 - sc) ** 1.8))   # вогнутый скос обуха
    x_e = -0.31 + (0.31 + x_tip) * t ** 4.0                                       # брюшко поднимается к острию
    x_e = np.where(t < 0.045, -0.2, x_e)                                          # чойл у основания
    ts = 0.03 * (1 - 0.55 * t)
    return x_s, x_e, ts


def _bowie_mesh(z0=0.64, z1=3.42, n=44):
    """Сетка клинка боуи: сечение — острая кромка, спуски, плоский обух (5 точек), остриё сходится в точку.
    Грани наружу (против часовой снаружи, как у _box_face)."""
    t = np.linspace(0.0, 0.985, n)
    z = z0 + (z1 - z0) * t
    x_s, x_e, ts = _bowie_profile(t)
    x_g = x_e + 0.45 * (x_s - x_e)
    tg = ts * 0.85
    rings = []
    for i in range(n):
        rings.append([(x_e[i], 0.0, z[i]), (x_g[i], tg[i], z[i]), (x_s[i], ts[i], z[i]),
                      (x_s[i], -ts[i], z[i]), (x_g[i], -tg[i], z[i])])
    P = [pt for ring in rings for pt in ring]
    tip_i = len(P)
    xs1, xe1, _ = _bowie_profile(np.array([1.0]))
    P.append(((xs1[0] + xe1[0]) / 2, 0.0, z1))
    base_i = len(P)
    P.append(tuple(np.mean(np.array(rings[0]), axis=0)))
    P = np.array(P, float)
    m = 5
    per = np.array([np.linalg.norm(P[(k + 1) % m] - P[k]) for k in range(m)])
    vcum = np.concatenate([[0], np.cumsum(per)]) / 0.6
    tris, uvs = [], []

    def add(a, b, c, ua, ub, uc, outward):
        nrm = np.cross(P[b] - P[a], P[c] - P[a])
        if np.dot(nrm, outward) < 0:
            b, c, ub, uc = c, b, uc, ub
        tris.append((a, b, c))
        uvs.append((ua, ub, uc))

    for i in range(n - 1):
        ci = np.mean(P[i * m:(i + 1) * m], axis=0)
        for k in range(m):
            a = i * m + k
            b = i * m + (k + 1) % m
            c = (i + 1) * m + (k + 1) % m
            d = (i + 1) * m + k
            mid = (P[a] + P[b] + P[c] + P[d]) / 4
            out = mid - np.array([ci[0], ci[1], mid[2]])
            u0, u1 = z[i] / 1.2, z[i + 1] / 1.2
            v0, v1 = vcum[k], vcum[k + 1]
            add(a, b, c, (u0, v0), (u0, v1), (u1, v1), out)
            add(a, c, d, (u0, v0), (u1, v1), (u1, v0), out)
    o = (n - 1) * m
    ci = np.mean(P[o:o + m], axis=0)
    for k in range(m):
        a, b = o + k, o + (k + 1) % m
        mid = (P[a] + P[b] + P[tip_i]) / 3
        add(a, b, tip_i, (z[-1] / 1.2, vcum[k]), (z[-1] / 1.2, vcum[k + 1]), (z1 / 1.2, (vcum[k] + vcum[k + 1]) / 2),
            mid - np.array([ci[0], ci[1], mid[2] - 0.5]))
    for k in range(m):
        add(k, (k + 1) % m, base_i, (0, vcum[k]), (0, vcum[k + 1]), (0.05, 0.5), np.array([0.0, 0.0, -1.0]))
    return P, tris, uvs


def build_blade_full_scene(seed):
    """«Голодный клинок» целиком своей моделью (корни у прикреплённого декора не видны в руках и в превью, E54):
    клинок боуи из бурой стали с жилами (прямой обух со скосом к острию, выпуклое брюшко, спуски), костяная гарда,
    рукоять из корня в витках, навершие. Оси и размеры как у ванильного FangeKnife (рукоять у начала, клинок
    вдоль +Y DayZ, обух к +X, толщина по Z), хват в руке тот же. Оси Blender: X=X, Z=Y DayZ, Y=Z DayZ. Масштаб x10."""
    rng = np.random.default_rng(seed)
    tubes = []
    cx = 0.03
    # рукоять и навершие
    z = np.linspace(-0.85, 0.6, 24)
    hr = 0.165 + 0.025 * np.sin((z + 0.85) / 1.45 * math.pi)
    tubes.append(Tube(np.column_stack([np.full(24, cx), np.zeros(24), z]), hr, "root", mat="crust", segs=10,
                      aspect=1.2, lump=0.08, lump_freq=8, seed=seed + 40, cap_flat=True))
    tubes.append(Tube(np.array([[cx, 0, -0.98], [cx, 0, -0.9], [cx, 0, -0.82]]), np.array([0.13, 0.21, 0.18]), "root",
                      mat="bone", segs=12, aspect=1.15, seed=seed + 43))
    # витки корней по рукояти
    for k in range(2):
        t = np.linspace(0, 2 * math.pi * 3.4, 140) + k * math.pi
        zz = np.linspace(-0.8, 0.56, 140)
        h = np.column_stack([cx + 0.235 * np.cos(t), 0.26 * np.sin(t), zz])
        rr = np.full(len(h), 0.05)
        tubes.append(Tube(h, rr, "root", lump=0.15, lump_freq=9, seed=seed + k, cap_start=False))
        v = vein_along(h, rr, rng, 0.0, 0.012)
        if v:
            tubes.append(Tube(v[0], v[1], "vein", seed=seed + 10 + k))
    # гарда: кость, к кромке чуть загнута вперёд
    gx = np.linspace(-0.5, 0.4, 16)
    gz = 0.63 + 0.06 * np.clip(-gx - 0.2, 0, None) ** 1.2
    tubes.append(Tube(np.column_stack([gx, np.zeros(16), gz]), np.full(16, 0.055), "root", mat="bone", segs=8,
                      aspect=1.35, seed=seed + 41))
    # корни переползают с рукояти через гарду на клинок, по плоскости у обуха
    for k, side in enumerate((-1, 1)):
        zr = np.linspace(0.45, 2.25, 34)
        tr = np.clip((zr - 0.64) / 2.78, 0, 1)
        _, _, tsr = _bowie_profile(tr)
        rr = np.linspace(0.042, 0.008, len(zr))
        xr = 0.13 + 0.035 * np.sin(zr * 4 + k * 2)
        yr = side * np.where(zr < 0.66, 0.2 - (zr - 0.45) / 0.21 * 0.1, tsr * 0.95 + rr * 0.8)
        pts = np.column_stack([xr, yr, zr])
        tubes.append(Tube(pts, rr, "root", lump=0.12, lump_freq=10, seed=seed + 20 + k))
        tubes.append(Tube(pts + np.array([0, side * 0.02, 0]), rr * 0.35, "vein", seed=seed + 30 + k))
    P, tris, uvs = _bowie_mesh()
    sc = _item(tubes, mass=0.2, center=(0, 0, 0))
    sc["meshes"] = [(P, tris, uvs, "bladesteel")]
    return sc


def _box_face(corners, uv_rect, outward):
    """Грань-прямоугольник: corners = 4 точки по кругу, uv_rect = (u0, v0, u1, v1) в долях текстуры.
    Порядок вершин выбирается так, чтобы грань смотрела наружу (против часовой снаружи, как у sphere_mesh)."""
    c = np.asarray(corners, float)
    u0, v0, u1, v1 = uv_rect
    uv = [(u0, v1), (u1, v1), (u1, v0), (u0, v0)]
    if np.dot(np.cross(c[1] - c[0], c[2] - c[0]), outward) < 0:
        c = c[[0, 3, 2, 1]]
        uv = [uv[0], uv[3], uv[2], uv[1]]
    tris = [(0, 1, 2), (0, 2, 3)]
    uvs = [(uv[0], uv[1], uv[2]), (uv[0], uv[2], uv[3])]
    return c, tris, uvs


def build_battery_full_scene(seed):
    """«Сердце-аккумулятор» целиком своей моделью (декор на ванильной модели не виден в руках и превью):
    корпус в ванильной текстуре аккумулятора AMUKA, свинцовые клеммы, сверху сердце, корни свисают по бокам.
    Габариты как у ванильного battery_car.p3d (Blender, x10): X ±0.78, Y ±1.25 (длина), Z 0..1.75."""
    sc = build_battery_heart_scene(seed)
    W, L, H = 0.78, 1.25, 1.75
    px = 1.0 / 1024
    faces = [
        # (углы, uv-прямоугольник по атласу, нормаль наружу)
        ([(W, -L, 0), (W, L, 0), (W, L, H), (W, -L, H)], (20 * px, 690 * px, 445 * px, 925 * px), (1, 0, 0)),       # бок AMUKA
        ([(-W, L, 0), (-W, -L, 0), (-W, -L, H), (-W, L, H)], (470 * px, 625 * px, 895 * px, 875 * px), (-1, 0, 0)),  # бок Pb
        ([(-W, -L, 0), (W, -L, 0), (W, -L, H), (-W, -L, H)], (470 * px, 92 * px, 740 * px, 350 * px), (0, -1, 0)),   # торец
        ([(W, L, 0), (-W, L, 0), (-W, L, H), (W, L, H)], (470 * px, 360 * px, 740 * px, 610 * px), (0, 1, 0)),      # торец
        ([(-W, -L, H), (W, -L, H), (W, L, H), (-W, L, H)], (785 * px, 165 * px, 985 * px, 295 * px), (0, 0, 1)),    # крышка
        ([(-W, L, 0), (W, L, 0), (W, -L, 0), (-W, -L, 0)], (25 * px, 110 * px, 440 * px, 350 * px), (0, 0, -1)),    # дно
    ]
    meshes = []
    for corners, rect, nrm in faces:
        c, tris, uvs = _box_face(corners, rect, np.asarray(nrm, float))
        meshes.append((c, tris, uvs, "battery"))
    sc["meshes"] = meshes
    for sx in (-1, 1):
        sc["tubes"].append(_lathe_z([(H - 0.02, 0.12), (H + 0.13, 0.12), (H + 0.17, 0.09)], "metal", segs=12, uv_len=0.3,
                                    x=0.5, y=sx * 0.95))
    sc["no_geom"] = False
    sc["item_mass"] = 10.0
    return sc


def _side_walk(rng, lat, z, heading, length, nrm, tang, F, S):
    """Извилистый путь по боковой грани в координатах (вдоль грани, высота); общий курс heading (−pi/2 — вниз).
    У края грани разворачивается, у дна останавливается."""
    step = 0.045
    out = [(lat, z)]
    h, curv = heading, 0.0
    for _ in range(max(3, int(length / step))):
        curv = curv * 0.85 + rng.normal(0, 0.16)
        h += curv
        h -= 0.1 * (h - heading)
        lat += step * math.cos(h)
        z += step * math.sin(h)
        if abs(lat) > S - 0.06:
            lat = math.copysign(S - 0.06, lat)
            h = math.pi - h
            heading = math.pi - heading
        if z < 0.05:
            break
        out.append((lat, z))
    return np.array(out)


def _on_face(walk, R, nrm, tang, F, emb=0.45):
    """Путь по грани -> 3D: центр трубки над гранью на emb·R (остальное «вросло» в корпус)."""
    xy = (F + emb * R)[:, None] * nrm[None, :] + walk[:, :1] * tang[None, :]
    return np.column_stack([xy, walk[:, 1]])


def _taper_tip(n, r0, r1, tip=0.006, frac=0.25):
    """Радиусы: плавно r0 -> r1, последние frac пути сходятся в острие."""
    t = np.linspace(0, 1, n)
    r = r0 + (r1 - r0) * t
    w = smoothstep(1 - frac, 1.0, t)
    return r * (1 - w) + tip * w


def _battery_root(rng, start, a, W, L, H, r0):
    """Корень от сердца: по крышке к ребру (с вилянием), через скруглённое ребро и извилисто вниз по грани,
    с 1–2 отростками. Корень наполовину врос в корпус. Возвращает [(путь, радиусы, начало_с_торцом)], сведения
    о грани (нормаль xy, касательная xy, смещение, полуширина) и точки пути по грани для жил."""
    d = np.array([math.cos(a), math.sin(a)])
    tx = W / abs(d[0]) if abs(d[0]) > 1e-6 else 1e9
    ty = L / abs(d[1]) if abs(d[1]) > 1e-6 else 1e9
    face_x = tx < ty
    edge = d * min(tx, ty)
    nrm = np.array([math.copysign(1, d[0]), 0.0]) if face_x else np.array([0.0, math.copysign(1, d[1])])
    tang = np.array([-nrm[1], nrm[0]])
    F, S = (W, L) if face_x else (L, W)
    r_edge = r0 * 0.75
    # крышка: от сердца к ребру, виляет поперёк
    s0 = np.asarray(start[:2], float)
    e0 = edge - nrm * 0.07
    t = np.linspace(0, 1, 14)
    side = np.array([-d[1], d[0]])
    wig = 0.07 * np.sin(t * math.pi * rng.uniform(1.0, 2.0) + rng.uniform(0, 6.28)) * np.sin(t * math.pi)
    lid_xy = s0[None, :] + (e0 - s0)[None, :] * t[:, None] + side[None, :] * wig[:, None]
    lid_r = np.linspace(r0, r_edge, len(t))
    lid = np.column_stack([lid_xy, H + 0.45 * lid_r])
    lid[0, 2] = start[2]
    # ребро: скругление на 45°
    lat0 = float(np.dot(lid_xy[-1], tang))
    corner = np.array([[*(nrm * (F + 0.32 * r_edge) + tang * lat0), H + 0.32 * r_edge]])
    # грань: извилисто вниз
    walk = _side_walk(rng, lat0, H - 0.06, -math.pi / 2 + rng.uniform(-0.5, 0.5), rng.uniform(0.9, 1.7), nrm, tang, F, S)
    side_r = _taper_tip(len(walk), r_edge * 0.95, r_edge * 0.45)
    side_p = _on_face(walk, side_r, nrm, tang, F)
    pts = np.vstack([lid, corner, side_p])
    rr = np.concatenate([lid_r, [r_edge], side_r])
    P, R, _ = resample(pts, rr, DS)
    out = [(P, R, True)]
    # отростки от корня на грани
    for b in range(int(rng.integers(1, 3))):
        if len(walk) < 6:
            break
        k = int(rng.integers(2, max(3, len(walk) * 2 // 3)))
        hb = -math.pi / 2 + rng.choice([-1, 1]) * rng.uniform(0.5, 1.0)
        wb = _side_walk(rng, float(walk[k, 0]), float(walk[k, 1]), hb, rng.uniform(0.35, 0.8), nrm, tang, F, S)
        if len(wb) < 4:
            continue
        rb = _taper_tip(len(wb), side_r[k] * 0.7, side_r[k] * 0.35, frac=0.35)
        Pb, Rb, _ = resample(_on_face(wb, rb, nrm, tang, F), rb, DS)
        out.append((Pb, Rb, False))
    return out, (nrm, tang, F, S), walk
def _face_vein(rng, nrm, tang, F, S, lat0, z0, H, steps):
    """Тонкая жила, ползущая по боковой грани корпуса (наполовину утоплена в неё), в основном вниз и вбок."""
    lat, z = lat0, z0
    ang = rng.uniform(-math.pi + 0.4, -0.4)
    out = []
    for _ in range(steps):
        out.append([*(nrm * (F + 0.006) + tang * lat), z])
        ang += rng.normal(0, 0.45)
        lat = float(np.clip(lat + 0.06 * math.cos(ang), -S + 0.04, S - 0.04))
        z = float(np.clip(z + 0.06 * math.sin(ang) - 0.012, 0.06, H - 0.05))
    return np.array(out)


def build_battery_heart_scene(seed):
    """Сердце на аккумуляторе (габариты как у dz/vehicles/parts/battery_car.p3d): крупное сердце наполовину вросло
    в крышку, тёмные бугристые корни (светится только жила вдоль них) расползаются вразнобой, переваливают
    через рёбра и идут по бокам с уводом, по корпусу и крышке ползут тонкие светящиеся жилы, от клемм к сердцу
    — жилы-провода. Оси Blender: X=X, Y=Z DayZ, Z=Y DayZ. Масштаб x10."""
    rng = np.random.default_rng(seed)
    W, L, H = 0.78, 1.26, 1.75
    tubes = []
    heart_c = np.array([-0.08, -0.12, H + 0.12])
    big = lambda d: (heart_surface(d) - HEART_C) * 0.95 + heart_c
    # направления корней: вразнобой, но не через клеммы (x 0.5, y ±0.95)
    term = [math.atan2(0.95 - heart_c[1], 0.5 - heart_c[0]), math.atan2(-0.95 - heart_c[1], 0.5 - heart_c[0])]
    angs = []
    for a in np.linspace(0, 2 * math.pi, 9, endpoint=False) + rng.uniform(-0.3, 0.3, 9) + 0.2:
        if min(abs((a - t + math.pi) % (2 * math.pi) - math.pi) for t in term) > 0.3:
            angs.append(a)
    for k, a in enumerate(angs):
        dirv = np.array([math.cos(a), math.sin(a), -0.45])
        start = big((dirv / np.linalg.norm(dirv))[None, :])[0] * 0.9 + heart_c * 0.1
        r0 = rng.uniform(0.07, 0.1)
        parts, (nrm, tang, F, S), walk = _battery_root(rng, start, a, W, L, H, r0)
        for n_, (P, R, main) in enumerate(parts):
            tubes.append(Tube(P, R, "root", mat="bark", lump=0.2, lump_freq=7, seed=seed * 10 + k * 5 + n_,
                              cap_start=main))
            v = vein_along(P, R, rng, 0.0, 0.018)
            if v:
                tubes.append(Tube(v[0], v[1], "vein", seed=seed * 10 + 300 + k * 5 + n_))
        # от корня по грани расходятся тонкие светящиеся жилы
        for b in range(int(rng.integers(1, 3)) if len(walk) > 4 else 0):
            i0 = int(rng.integers(1, len(walk)))
            q = _face_vein(rng, nrm, tang, F, S, float(walk[i0, 0]), float(walk[i0, 1]), H, int(rng.integers(7, 13)))
            tubes.append(Tube(q, np.linspace(0.016, 0.006, len(q)), "vein", lump=0.1, lump_freq=14,
                              seed=seed * 10 + 80 + k * 3 + b))
    # тонкие жилы по крышке от сердца
    for b in range(5):
        a = rng.uniform(0, 2 * math.pi)
        p0 = heart_c[:2] + 0.5 * np.array([math.cos(a), math.sin(a)])
        pts = [p0]
        for _ in range(int(rng.integers(6, 11))):
            a += rng.normal(0, 0.4)
            nq = pts[-1] + 0.07 * np.array([math.cos(a), math.sin(a)])
            nq = np.clip(nq, [-W + 0.05, -L + 0.05], [W - 0.05, L - 0.05])
            pts.append(nq)
        pts = np.column_stack([np.array(pts), np.full(len(pts), H + 0.006)])
        tubes.append(Tube(pts, np.linspace(0.016, 0.006, len(pts)), "vein", lump=0.1, lump_freq=14, seed=seed * 10 + 120 + b))
    # жилы-провода от клемм к сердцу
    for k, (x0, y0) in enumerate(((0.5, 0.95), (0.5, -0.95))):
        pts = np.array([[x0, y0, H + 0.17], [x0 * 0.9, y0 * 0.75, H + 0.42], [x0 * 0.5, y0 * 0.35, H + 0.55],
                        [heart_c[0] + 0.2, heart_c[1] + y0 * 0.15, H + 0.42]])
        tubes.append(Tube(pts, np.full(4, 0.035), "vein", lump=0.1, lump_freq=10, seed=seed * 10 + 60 + k))
    eye = big(np.array([[0.0, 0.0, 1.0]]))[0]
    sc = _item(tubes, blobs=[big], bulbs=[(eye - np.array([0, 0, 0.06]), 0.13)], mass=0.01, center=(0, 0, 0))
    sc["no_geom"] = True
    return sc
def build_hang_scene(seed, kind):
    """Свисающие щупальца: горизонтальный корень-«якорь» и висящие отростки с острыми концами.
    Начало координат — середина якоря (точка крепления); ставятся вручную на мосты, скалы, провода."""
    rng = np.random.default_rng(seed)
    tubes, bulbs = [], []
    if kind == "drape":
        W, sag, n, Lmin, Lmax = 6.0, 0.5, 9, 0.8, 3.0
    elif kind == "long":
        W, sag, n, Lmin, Lmax = 2.6, 0.12, 7, 2.5, 5.0
    else:
        W, sag, n, Lmin, Lmax = 2.0, 0.08, 6, 0.8, 2.0
    na = max(10, int(W / DS))
    x = np.linspace(-W / 2, W / 2, na)
    za = -sag * (1 - (2 * x / W) ** 2) + 0.03 * np.sin(x * 4 + seed)
    ya = 0.05 * np.sin(x * 2.3 + seed)
    anchor = np.column_stack([x, ya, za])
    ra = 0.07 * (1 - 0.35 * np.abs(2 * x / W))
    tubes.append(Tube(anchor, ra, "root", lump=0.15, lump_freq=6.0, seed=seed * 10))
    v = vein_along(anchor, ra, rng, 0.0, 0.02)
    if v is not None:
        tubes.append(Tube(v[0], v[1], "vein", lump=0.08, lump_freq=10.0, seed=seed * 10 + 1))
    for i in range(n):
        xi = rng.uniform(-W / 2 * 0.9, W / 2 * 0.9)
        k = int(np.argmin(np.abs(x - xi)))
        L = rng.uniform(Lmin, Lmax)
        m = max(10, int(L / DS))
        u = np.linspace(0, 1, m)
        ph1, ph2 = rng.uniform(0, 6.28, 2)
        amp = rng.uniform(0.05, 0.18) * (L / 2)
        px = anchor[k, 0] + amp * np.sin(u * rng.uniform(2, 4) + ph1) * u
        py = anchor[k, 1] + amp * np.cos(u * rng.uniform(2, 4) + ph2) * u
        pz = anchor[k, 2] - 0.02 - L * u
        pts = np.column_stack([px, py, pz])
        r0 = rng.uniform(0.035, 0.05)
        # к концу сходится в острие, как у корней (капель на концах нет — решение пользователя 11-11)
        r = r0 * (1 - 0.8 * u ** 0.85)
        tw = smoothstep(0.8, 1.0, u)
        r = r * (1 - tw) + 0.003 * tw
        tubes.append(Tube(pts, r, "root", lump=0.15, lump_freq=9.0, seed=seed * 10 + 10 + i, cap_start=False))
        vv = vein_along(pts, r, rng, 0.05, 0.008)
        if vv is not None:
            tubes.append(Tube(vv[0], vv[1], "vein", lump=0.08, lump_freq=12.0, seed=seed * 10 + 40 + i))
    return {"tubes": tubes, "core": None, "top": None, "heart": False, "no_cut": True, "bulbs": bulbs,
            "handle": ((-0.05, -0.05, -0.05), (0.05, 0.05, 0.05))}


CORNER_RC = 0.12     # радиус скругления ребра угла дома для корней, м


def _surf_flat(s, v, off):
    """Плоская стена: плоскость y = 0, наружу +y; s — вдоль стены, v — вверх."""
    s = np.asarray(s, float)
    return np.column_stack([s, np.broadcast_to(off, s.shape), np.asarray(v, float)])


def _norm_flat(s):
    s = np.asarray(s, float)
    return np.column_stack([np.zeros_like(s), np.ones_like(s)])


def _surf_corner(s, v, off):
    """Наружный угол дома: дом в четверти x < 0, y < 0. Грань A — плоскость y = 0 (s < 0, наружу +y),
    грань B — плоскость x = 0 (s > 0, наружу +x), ребро скруглено радиусом CORNER_RC.
    s — развёртка по поверхности: отрицательная на грани A, положительная на грани B."""
    s = np.asarray(s, float)
    off = np.broadcast_to(np.asarray(off, float), s.shape)
    rc = CORNER_RC
    ph = np.clip((s + rc) / (2 * rc), 0.0, 1.0) * (math.pi / 2)
    x = np.where(s < -rc, s, np.where(s > rc, off, -rc + (rc + off) * np.sin(ph)))
    y = np.where(s < -rc, off, np.where(s > rc, -s, -rc + (rc + off) * np.cos(ph)))
    return np.column_stack([x, y, np.asarray(v, float)])


def _norm_corner(s):
    s = np.asarray(s, float)
    rc = CORNER_RC
    ph = np.clip((s + rc) / (2 * rc), 0.0, 1.0) * (math.pi / 2)
    return np.column_stack([np.sin(ph), np.cos(ph)])


def _clip_uv(uv, smin, smax, vmax):
    """Путь по развёртке обрывается там, где выходит за грань или выше vmax."""
    bad = (uv[:, 0] < smin) | (uv[:, 0] > smax) | (uv[:, 1] > vmax)
    if bad.any():
        uv = uv[:int(np.argmax(bad))]
    return uv


def _taper(n_pts, r0, cut=0.8):
    s = np.linspace(0, 1, n_pts)
    r = r0 * (1 - cut * s ** 0.9)
    tw = smoothstep(0.85, 1.0, s)
    return r * (1 - tw) + 0.003 * tw


def _surface_roots(rng, seed, surf, norm, starts, hmin, hmax, smin, smax, n_veins):
    """Корни на поверхности (стена или угол): из земли перед ней загибаются и ползут вверх, ветвятся;
    по самой поверхности расходится сеть тонких светящихся жил. Всё в развёртке (s, v), потом на поверхность."""
    tubes = []
    climb_uv = []
    vmax = hmax + 0.4
    for i, u0 in enumerate(starts):
        r0 = rng.uniform(0.09, 0.14)
        H = rng.uniform(hmin, hmax)
        bend = 0.28
        yw = r0 * 0.85
        z0 = SINK * r0
        n0 = norm([u0])[0]
        base = surf([u0], [0.0], 0.0)[0, :2]
        # 1. по земле к поверхности, с лёгким боковым уводом
        d_out = rng.uniform(0.6, 1.3)
        ng = max(6, int(d_out / DS))
        g = np.linspace(0, 1, ng)
        side = np.array([-n0[1], n0[0]])
        sway = rng.uniform(-0.5, 0.5) * (1 - g) ** 2 + 0.04 * np.sin(g * 5 + i)
        gp = base[None, :] + n0[None, :] * (yw + bend + d_out * (1 - g))[:, None] + side[None, :] * sway[:, None]
        ground = np.column_stack([gp, np.full(ng, z0)])
        # 2. загиб вверх (четверть окружности)
        phi = np.linspace(0, math.pi / 2, max(6, int(bend * math.pi / 2 / DS)))[1:]
        ap = base[None, :] + n0[None, :] * (yw + bend * (1 - np.sin(phi)))[:, None]
        arc = np.column_stack([ap, z0 + bend * (1 - np.cos(phi))])
        # 3. вверх по поверхности
        uv, hs = walk_xy(rng, np.array([u0, 0.0]), math.pi / 2 + rng.uniform(-0.25, 0.25), H, 0.95)
        uv = _clip_uv(uv, smin, smax, vmax - z0 - bend)
        if len(uv) < 12:
            continue
        hs = hs[:len(uv)]
        nc = len(uv) - 1
        tot = len(ground) + len(arc) + nc
        r_all = _taper(tot, r0)
        r_all *= 1 + 0.12 * np.sin(np.arange(tot) * DS * rng.uniform(2.5, 4.0) + rng.uniform(0, 6.28)) * np.linspace(1, 0, tot)
        rc_ = r_all[-nc:]
        lift = 0.025 * np.maximum(0, np.sin(np.arange(nc) * DS * rng.uniform(1.5, 2.5) + rng.uniform(0, 6.28))) ** 2
        climb = surf(uv[1:, 0], z0 + bend + uv[1:, 1], rc_ * 0.85 + lift)
        pts = np.vstack([ground, arc, climb])
        tubes.append(Tube(pts, r_all, "root", lump=0.14, lump_freq=6.0, seed=seed * 10 + i))
        v = vein_along(pts, r_all, rng, 0.1, 0.02)
        if v is not None:
            tubes.append(Tube(v[0], v[1], "vein", lump=0.08, lump_freq=10.0, seed=seed * 10 + 50 + i))
        cuv = uv[1:] + np.array([0.0, z0 + bend])
        climb_uv.append(cuv)
        # отростки вбок-вверх по поверхности
        for b in range(int(rng.integers(2, 5))):
            k = int(rng.uniform(0.25, 0.8) * (len(cuv) - 1))
            bh = hs[min(k + 1, len(hs) - 1)] + rng.choice([-1, 1]) * rng.uniform(0.7, 1.4)
            buv, _ = walk_xy(rng, cuv[k], bh, rng.uniform(0.8, 2.0), 0.9)
            buv = _clip_uv(buv, smin, smax, vmax)
            if len(buv) < 8:
                continue
            br = _taper(len(buv), rc_[k] * 0.62)
            bp = surf(buv[:, 0], buv[:, 1], br * 0.85)
            tubes.append(Tube(bp, br, "root", lump=0.14, lump_freq=7.0, seed=seed * 10 + 20 + i * 5 + b, cap_start=False))
            bv = vein_along(bp, br, rng, 0.05, 0.015)
            if bv is not None:
                tubes.append(Tube(bv[0], bv[1], "vein", lump=0.08, lump_freq=12.0, seed=seed * 10 + 70 + i * 5 + b))
    # сеть тонких светящихся жил прямо по поверхности
    for j in range(n_veins):
        if not climb_uv:
            break
        src = climb_uv[int(rng.integers(len(climb_uv)))]
        p0 = src[int(rng.uniform(0.0, 0.8) * (len(src) - 1))]
        uv, _ = walk_xy(rng, p0, rng.uniform(0.1, math.pi - 0.1), rng.uniform(1.0, 3.0), 1.3)
        uv = _clip_uv(uv, smin, smax, vmax)
        if len(uv) < 8:
            continue
        u = np.linspace(0, 1, len(uv))
        rv = 0.02 * (1 - 0.7 * u) + 0.004
        pts = surf(uv[:, 0], uv[:, 1], rv * 0.45)
        tubes.append(Tube(pts, rv, "vein", lump=0.1, lump_freq=14.0, seed=seed * 10 + 90 + j))
    return tubes


# ширина грани (м) для корней по стене: скрипт сердца проверяет, что стена есть на всю ширину
WALL_KINDS = {
    "low": (3, 2.0, 3.2, 2.4),       # корней, высота от, до, ширина
    "tall": (4, 3.4, 5.0, 3.0),
    "wide": (5, 1.8, 3.2, 4.4),
}


def build_wall_scene(seed, kind):
    """Корни по стене дома: выползают из земли перед стеной, загибаются и лезут вверх по фасаду, ветвятся;
    по самой стене тянется сеть тонких светящихся жил. Плоскость стены — y = 0, наружу — +y (в игре +Z модели,
    «вперёд»), земля — z = 0. Начало координат — основание стены. Всё в пределах ширины width/2 + 0.3 в стороны."""
    rng = np.random.default_rng(seed)
    n, hmin, hmax, width = WALL_KINDS[kind]
    starts = [((i + 0.5) / n - 0.5) * width + rng.uniform(-0.2, 0.2) for i in range(n)]
    lim = width / 2 + 0.3
    tubes = _surface_roots(rng, seed, _surf_flat, _norm_flat, starts, hmin, hmax, -lim, lim, int(rng.integers(10, 17)))
    return {"tubes": tubes, "core": None, "top": None, "heart": False,
            "handle": ((-0.05, -0.05, -0.05), (0.05, 0.05, 0.05))}


CORNER_FACE = 2.0    # м: на сколько корни угла заходят на каждую из двух стен


def build_corner_scene(seed):
    """Корни на наружном углу дома: вылезают у ребра, ползут вверх и переваливают через угол на обе стены,
    сеть жил расходится по обеим граням. Дом — четверть x < 0, y < 0; грань A — y = 0 (наружу +y, в игре +Z),
    грань B — x = 0 (наружу +x, в игре +X). Начало координат — ребро угла у земли."""
    rng = np.random.default_rng(seed)
    starts = [rng.uniform(-0.55, -0.15), rng.uniform(-0.1, 0.1), rng.uniform(0.15, 0.55)]
    lim = CORNER_FACE
    tubes = _surface_roots(rng, seed, _surf_corner, _norm_corner, starts, 2.6, 4.4, -lim, lim, int(rng.integers(12, 18)))
    return {"tubes": tubes, "core": None, "top": None, "heart": False,
            "handle": ((-0.05, -0.05, -0.05), (0.05, 0.05, 0.05))}


# ---------------------------------------------------------------- сетка
def resample(pts, radii, step):
    d = np.linalg.norm(np.diff(pts, axis=0), axis=1)
    s = np.concatenate([[0.0], np.cumsum(d)])
    n = max(2, int(math.ceil(s[-1] / step)) + 1)
    ss = np.linspace(0, s[-1], n)
    P = np.column_stack([np.interp(ss, s, pts[:, k]) for k in range(3)])
    R = np.interp(ss, s, radii)
    return P, R, ss


class MeshBuilder:
    def __init__(self):
        self.P, self.T, self.UV, self.M = [], [], [], []
        self.n = 0

    def add(self, P, tris, uvs, mat):
        P = np.asarray(P, float)
        tris = np.asarray(tris, np.int64)
        self.P.append(P)
        self.T.append(tris + self.n)
        self.UV.append(np.asarray(uvs, float))
        self.M += [mat] * len(tris)
        self.n += len(P)

    def finish(self):
        P = np.vstack(self.P)
        T = np.vstack(self.T)
        UV = np.concatenate(self.UV, axis=0)
        return {"P": P, "T": T, "UV": UV, "M": list(self.M)}


def tube_mesh(tube, step, segs, uv_len):
    if tube.raw:
        P, R = tube.pts, tube.radii
        S = np.concatenate([[0.0], np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))])
    else:
        P, R, S = resample(tube.pts, tube.radii, step)
    T, N, B = frames(P)
    n = len(P)
    ang = tube.ang0 + 2 * math.pi * np.arange(segs) / segs
    offs = np.cos(ang)[None, :, None] * N[:, None, :] + tube.aspect * np.sin(ang)[None, :, None] * B[:, None, :]
    ring = P[:, None, :] + R[:, None, None] * offs
    if tube.lump > 0:
        flat = ring.reshape(-1, 3)
        k = 1 + tube.lump * fbm3(flat * tube.lump_freq + tube.seed * 0.37, 3, seed=tube.seed)
        ring = P[:, None, :] + R[:, None, None] * offs * k.reshape(n, segs)[:, :, None]
    verts = [ring.reshape(-1, 3)]
    circ = max(1, int(round(2 * math.pi * float(np.mean(R)) / uv_len)))
    uj = np.arange(segs + 1) / segs * circ
    vi = S / uv_len
    tris, uvs = [], []
    for i in range(n - 1):
        for j in range(segs):
            a = i * segs + j
            b = i * segs + (j + 1) % segs
            c = (i + 1) * segs + (j + 1) % segs
            d = (i + 1) * segs + j
            ua, ub = uj[j], uj[j + 1]
            tris.append((a, b, c))
            uvs.append(((ua, vi[i]), (ub, vi[i]), (ub, vi[i + 1])))
            tris.append((a, c, d))
            uvs.append(((ua, vi[i]), (ub, vi[i + 1]), (ua, vi[i + 1])))
    base = n * segs
    extra = []
    if tube.cap_end:
        tip = P[-1] + T[-1] * R[-1] * (0.0 if tube.cap_flat else 0.8)
        ti = base + len(extra)
        extra.append(tip)
        o = (n - 1) * segs
        for j in range(segs):
            tris.append((o + j, o + (j + 1) % segs, ti))
            uvs.append(((uj[j], vi[-1]), (uj[j + 1], vi[-1]), ((uj[j] + uj[j + 1]) / 2, vi[-1] + 0.05)))
    if tube.cap_start:
        tip = P[0] - T[0] * R[0] * (0.0 if tube.cap_flat else 0.8)
        ti = base + len(extra)
        extra.append(tip)
        for j in range(segs):
            tris.append(((j + 1) % segs, j, ti))
            uvs.append(((uj[j + 1], vi[0]), (uj[j], vi[0]), ((uj[j] + uj[j + 1]) / 2, vi[0] - 0.05)))
    if extra:
        verts.append(np.array(extra))
    return np.vstack(verts), tris, uvs


def sphere_mesh(nu, nv, surf_fn, uv_tiles):
    """UV-сфера, грани против часовой снаружи. surf_fn(dirs) -> точки."""
    rows = []
    thetas = np.pi * np.arange(1, nv) / nv
    phis = 2 * np.pi * np.arange(nu) / nu
    for th in thetas:
        d = np.column_stack([np.sin(th) * np.cos(phis), np.sin(th) * np.sin(phis), np.full(nu, np.cos(th))])
        rows.append(d)
    dirs = np.vstack(rows + [np.array([[0, 0, 1.0]]), np.array([[0, 0, -1.0]])])
    P = surf_fn(dirs)
    top_i = (nv - 1) * nu
    bot_i = top_i + 1
    ut, vt = uv_tiles
    uu = np.arange(nu + 1) / nu * ut
    vv = np.arange(nv + 1) / nv * vt
    tris, uvs = [], []
    for j in range(nu):
        tris.append((top_i, j, (j + 1) % nu))
        uvs.append((((uu[j] + uu[j + 1]) / 2, vv[0]), (uu[j], vv[1]), (uu[j + 1], vv[1])))
    for i in range(nv - 2):
        for j in range(nu):
            a = i * nu + j
            b = (i + 1) * nu + j
            c = (i + 1) * nu + (j + 1) % nu
            d = i * nu + (j + 1) % nu
            tris.append((a, b, c))
            uvs.append(((uu[j], vv[i + 1]), (uu[j], vv[i + 2]), (uu[j + 1], vv[i + 2])))
            tris.append((a, c, d))
            uvs.append(((uu[j], vv[i + 1]), (uu[j + 1], vv[i + 2]), (uu[j + 1], vv[i + 1])))
    o = (nv - 2) * nu
    for j in range(nu):
        tris.append((bot_i, o + (j + 1) % nu, o + j))
        uvs.append((((uu[j] + uu[j + 1]) / 2, vv[-1]), (uu[j + 1], vv[-2]), (uu[j], vv[-2])))
    return P, tris, uvs


def mesh_lod(scene, prm):
    mb = MeshBuilder()
    for tb in scene["tubes"]:
        step, segs = prm[tb.kind]
        if tb.segs:
            segs = tb.segs if tb.fixed_segs else max(4, int(round(tb.segs * segs / LOD_PARAMS[0][tb.kind][1])))
        uv_len = tb.uv_len or (0.6 if tb.kind == "root" else 0.25)
        P, tris, uvs = tube_mesh(tb, step, segs, uv_len)
        mb.add(P, tris, uvs, tb.mat or ("crust" if tb.kind == "root" else "vein"))
    for center, radius in scene.get("bulbs", []):
        cu, cv = prm["core"]
        P, tris, uvs = sphere_mesh(cu, cv, lambda d, c=center, rr=radius: c + d * rr * (1 + 0.12 * fbm3(d * 3 + rr * 50, 2, 77))[:, None], (1, 1))
        mb.add(P, tris, uvs, "vein")
    for P, tris, uvs, mat in scene.get("meshes", []):
        mb.add(P, tris, uvs, mat)
    for surf in scene.get("blobs", []):
        nu, nv = prm["heart"]
        P, tris, uvs = sphere_mesh(nu, nv, surf, (3, 2))
        mb.add(P, tris, uvs, "crust")
    if not scene["heart"]:
        mesh = mb.finish()
        if scene.get("scale"):          # предмет: центрируем и уменьшаем
            mesh["P"] = (mesh["P"] - scene.get("center", 0)) * scene["scale"]
        return mesh
    nu, nv = prm["heart"]
    P, tris, uvs = sphere_mesh(nu, nv, heart_surface, (3, 2))
    mb.add(P, tris, uvs, "crust")
    c = scene["core"]
    cu, cv = prm["core"]
    P, tris, uvs = sphere_mesh(cu, cv, lambda d: c["center"] + d * c["radius"] * (1 + 0.1 * fbm3(d * 3 + 2, 2, 77))[:, None], (1, 1))
    mb.add(P, tris, uvs, "vein")
    return mb.finish()


def vertex_normals(P, T):
    fn = np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]])
    N = np.zeros_like(P)
    for k in range(3):
        np.add.at(N, T[:, k], fn)
    ln = np.linalg.norm(N, axis=1, keepdims=True)
    bad = ln[:, 0] < 1e-12
    N = N / np.maximum(ln, 1e-12)
    N[bad] = (0, 0, 1)
    return N


# ---------------------------------------------------------------- p3d
def visual_lod(mesh, resolution, point_flags=0):
    lod = py3d.LOD()
    lod.resolution = resolution
    P, T, UV, M = mesh["P"], mesh["T"], mesh["UV"], mesh["M"]
    Nrm = vertex_normals(P, T)
    for c in P:
        p = py3d.Point()
        p.coords = (float(c[0]), float(c[1]), float(c[2]))
        p.flags = point_flags
        lod.points.append(p)
    for n in Nrm:
        lod.facenormals.append((float(n[0]), float(n[1]), float(n[2])))
    # автосглаживание: угол нормали вершины к грани больше 60° -> у угла грани своя плоская нормаль
    fn = np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]])
    fn /= np.maximum(np.linalg.norm(fn, axis=1, keepdims=True), 1e-12)
    sels = {k: lod.new_selection(v["sel"]) for k, v in MATS.items()}
    for k in range(len(T)):
        mat = M[k]
        f = py3d.Face(lod.points, lod.facenormals)
        f.texture = MATS[mat]["tex"]
        f.material = MATS[mat]["mat"]
        flat_idx = None
        for c in range(3):
            idx = int(T[k, c])
            v = py3d.Vertex(lod.points, lod.facenormals)
            v.point_index = idx
            if float(np.dot(Nrm[idx], fn[k])) >= 0.5:
                v.normal_index = idx
            else:
                if flat_idx is None:
                    flat_idx = len(lod.facenormals)
                    lod.facenormals.append((float(fn[k, 0]), float(fn[k, 1]), float(fn[k, 2])))
                v.normal_index = flat_idx
            v.uv = (float(UV[k, c, 0]), float(UV[k, c, 1]))
            f.vertices.append(v)
            sels[mat].points[lod.points[idx]] = 1
        lod.faces.append(f)
        sels[mat].faces[f] = 1
    return lod


BOX_QUADS = [(0, 4, 6, 2), (1, 3, 7, 5), (0, 1, 5, 4), (2, 6, 7, 3), (0, 2, 3, 1), (4, 5, 7, 6)]
BOX_NORMALS = [(-1, 0, 0), (1, 0, 0), (0, -1, 0), (0, 1, 0), (0, 0, -1), (0, 0, 1)]


def box_lod(resolution, lo, hi, mass_per_point=None):
    """Выпуклый ящик (оси Blender, грани против часовой снаружи) — компонент коллизии."""
    lod = py3d.LOD()
    lod.resolution = resolution
    for i in range(8):
        p = py3d.Point()
        p.coords = (float(hi[0] if i & 1 else lo[0]), float(hi[1] if i & 2 else lo[1]),
                    float(hi[2] if i & 4 else lo[2]))
        p.mass = mass_per_point
        lod.points.append(p)
    for n in BOX_NORMALS:
        lod.facenormals.append(tuple(float(x) for x in n))
    comp = lod.new_selection("Component01")
    for fi, q in enumerate(BOX_QUADS):
        for tri in ((q[0], q[1], q[2]), (q[0], q[2], q[3])):
            f = py3d.Face(lod.points, lod.facenormals)
            f.material = PENETRATION
            f.texture = ""
            for idx in tri:
                v = py3d.Vertex(lod.points, lod.facenormals)
                v.point_index = idx
                v.normal_index = fi
                v.uv = (0.0, 0.0)
                f.vertices.append(v)
            lod.faces.append(f)
            comp.faces[f] = 1
    for p in lod.points:
        comp.points[p] = 1
    lod.properties["autocenter"] = "0"
    return lod


def cut_points(scene):
    """Точки для ClutterCutter2x2 вдоль корней и трещин, чтобы трава не закрывала их."""
    pts = []
    if scene.get("no_cut"):
        return pts
    for tb in scene["tubes"]:
        if tb.kind not in ("root", "crack"):
            continue
        d = np.linalg.norm(np.diff(tb.pts, axis=0), axis=1)
        s = np.concatenate([[0.0], np.cumsum(d)])
        for sv in np.arange(0.0, s[-1] + 1e-6, CUT_STEP):
            p = np.array([np.interp(sv, s, tb.pts[:, 0]), np.interp(sv, s, tb.pts[:, 1])])
            if scene["heart"] and np.max(np.abs(p)) < CUT_CENTER:
                continue
            if all(np.linalg.norm(p - q) > CUT_GAP for q in pts):
                pts.append(p)
    return pts


def build_p3d(scene, out_path):
    model = py3d.P3D()
    stats = []
    meshes = []
    for prm in LOD_PARAMS:
        mesh = mesh_lod(scene, prm)
        meshes.append(mesh)
        model.lods.append(visual_lod(mesh, prm["res"], scene.get("point_flags", 0)))
        stats.append((prm["res"], len(mesh["P"]), len(mesh["T"])))
    mem = py3d.LOD()
    mem.resolution = 1e15
    if scene["heart"]:
        # коллизия только у узла-«сердца», корни проходимы; свет и большой срезатель травы над ним
        zt = float(scene["top"][2])
        lo = (-0.48, -0.43, -0.05)
        hi = (0.48, 0.43, zt * 0.85)
        mem.set_memory_point("light", (0.0, 0.0, zt + 0.45))
        mem.set_memory_point("cut_big", (0.0, 0.0, 0.0))
    elif scene.get("scale"):
        # предмет: коллизия — ящик чуть меньше видимой модели, масса в Geometry
        pm = meshes[0]["P"]
        lo = tuple(float(v) * 0.8 for v in pm.min(0))
        hi = tuple(float(v) * 0.8 for v in pm.max(0))
    elif scene.get("handle"):
        lo, hi = scene["handle"]
    else:
        # корни без сердца: плоская «ручка» вровень с землёй, чтобы объект выделялся в редакторе
        lo = (-0.25, -0.25, -0.3)
        hi = (0.25, 0.25, 0.02)
    for i, (x, y) in enumerate(cut_points(scene), start=1):
        mem.set_memory_point("cut_%d" % i, (float(x), float(y), 0.0))
    mass = 30.0
    if scene.get("scale"):
        mass = scene.get("item_mass", 0.9) / 8     # предмет: масса в кг на 8 точек ящика
    if scene.get("no_geom"):
        # декор на чужом предмете: коллизии по сути нет (кубик 2 мм в начале координат), но Geometry LOD нужен
        # ради autocenter = 0 — без него движок сдвигает модель к центру габаритов (корни уезжали с рукояти)
        model.lods.append(box_lod(1e13, (-0.001, -0.001, -0.001), (0.001, 0.001, 0.001), mass_per_point=0.001))
        model.lods.append(mem)
    else:
        model.lods.append(box_lod(1e13, lo, hi, mass_per_point=mass))
        model.lods.append(mem)
        model.lods.append(box_lod(6e15, lo, hi))
        if scene["heart"] or scene.get("scale"):
            model.lods.append(box_lod(7e15, lo, hi))
    py3d.blender_to_dayz(model)
    model.save(out_path, verify=True)
    findings = [f for f in model.validate() if getattr(f, "severity", "") in ("ERROR", "WARN")]
    return stats, findings


# ---------------------------------------------------------------- текстуры
def spectral(n, rng, f0=None, bw=None, beta=None, fmin=1.0):
    w = rng.standard_normal((n, n))
    F = np.fft.fft2(w)
    f1 = np.fft.fftfreq(n) * n
    f = np.hypot(f1[:, None], f1[None, :])
    if beta is not None:
        filt = np.where(f >= fmin, np.maximum(f, 1e-9) ** (-beta), 0.0)
    else:
        filt = np.exp(-((f - f0) / bw) ** 2)
        filt[0, 0] = 0
    out = np.real(np.fft.ifft2(F * filt))
    return out / out.std()


def blur(img, sigma):
    n = img.shape[0]
    f1 = np.fft.fftfreq(n)
    g = np.exp(-2 * (np.pi * sigma) ** 2 * (f1[:, None] ** 2 + f1[None, :] ** 2))
    return np.real(np.fft.ifft2(np.fft.fft2(img) * g))


def to_u8(rgb):
    return (np.clip(rgb, 0, 1) * 255 + 0.5).astype(np.uint8)


def write_png(path, arr):
    h, w, c = arr.shape
    ct = 2 if c == 3 else 6
    raw = b"".join(b"\x00" + arr[y].tobytes() for y in range(h))

    def chunk(t, d):
        return struct.pack(">I", len(d)) + t + d + struct.pack(">I", zlib.crc32(t + d) & 0xFFFFFFFF)

    data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, ct, 0, 0, 0)) \
        + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b"")
    with open(path, "wb") as f:
        f.write(data)


def make_textures(png_dir):
    rng = np.random.default_rng(SEED + 1)
    n = 1024
    base = spectral(n, rng, beta=1.15)
    hi = spectral(n, rng, beta=0.6, fmin=24)
    n1 = spectral(n, rng, f0=5, bw=2.0)
    n2 = spectral(n, rng, f0=11, bw=3.0)
    mask = smoothstep(-0.2, 0.8, spectral(n, rng, beta=1.4))
    mask1 = smoothstep(-0.7, 0.3, spectral(n, rng, beta=1.6))
    width = 0.13 + 0.12 * smoothstep(-1, 1, spectral(n, rng, beta=1.5))
    lines1 = np.exp(-(n1 / width) ** 2) * mask1
    lines2 = np.exp(-(n2 / 0.14) ** 2) * mask * (1 - mask1 * 0.5)
    glow = np.clip(lines1 + 0.6 * lines2, 0, 1)
    halo = np.clip(blur(glow, 5.0) * 2.2, 0, 1)
    tex = {}
    # кора: почти чёрная бордовая, у трещин красный подтёк
    t = 0.5 + 0.5 * np.tanh(base * 0.8)
    dark1 = np.array([0.045, 0.022, 0.027])
    dark2 = np.array([0.15, 0.06, 0.062])
    co = dark1 * (1 - t)[..., None] + dark2 * t[..., None]
    co *= (1 + 0.18 * np.tanh(hi))[..., None]
    co = co * (1 - halo[..., None]) + np.array([0.32, 0.04, 0.07]) * halo[..., None]
    co = co * (1 - glow[..., None]) + np.array([0.75, 0.10, 0.16]) * glow[..., None]
    tex["vb_rift_crust_co"] = to_u8(co)
    # маска свечения коры
    e = np.array([1.0, 0.22, 0.34]) * glow[..., None] + np.array([0.15, 0.3, 0.3]) * (glow ** 6)[..., None]
    e += np.array([0.25, 0.02, 0.05]) * halo[..., None] * 0.5
    tex["vb_rift_crust_e_co"] = to_u8(e)
    # нормали коры: трещины — бороздки
    h = 0.55 * base + 0.2 * hi - 1.4 * blur(glow, 1.5)
    gx = (np.roll(h, -1, 1) - np.roll(h, 1, 1)) * 0.5
    gy = (np.roll(h, -1, 0) - np.roll(h, 1, 0)) * 0.5
    k = 2.2
    nx, ny, nz = -gx * k, -gy * k, np.ones_like(h)
    ln = np.sqrt(nx * nx + ny * ny + nz * nz)
    nrm = np.stack([nx / ln, ny / ln, nz / ln], -1) * 0.5 + 0.5
    tex["vb_rift_crust_nohq"] = to_u8(nrm)
    # жила: тёмно-красная, светится неровно, с яркими узлами
    m = 256
    vb = spectral(m, rng, beta=1.2)
    knots = np.clip(spectral(m, rng, f0=3, bw=1.0), 0, None)
    vco = np.array([0.42, 0.04, 0.08]) * (0.75 + 0.25 * np.tanh(vb))[..., None]
    tex["vb_rift_vein_co"] = to_u8(vco)
    ve = np.array([1.0, 0.25, 0.36]) * (0.6 + 0.3 * np.tanh(vb))[..., None]
    ve += np.array([0.25, 0.3, 0.3]) * np.clip(knots - 0.6, 0, 1)[..., None]
    tex["vb_rift_vein_e_co"] = to_u8(ve)
    # день: Изнанка «спит» — сухая серо-бурая кора, трещины тёмные, жилы тусклые
    day1 = np.array([0.20, 0.17, 0.14])
    day2 = np.array([0.36, 0.31, 0.26])
    dco = day1 * (1 - t)[..., None] + day2 * t[..., None]
    dco *= (1 + 0.2 * np.tanh(hi))[..., None]
    dco = dco * (1 - 0.6 * halo[..., None]) + np.array([0.13, 0.10, 0.09]) * 0.6 * halo[..., None]
    dco = dco * (1 - glow[..., None]) + np.array([0.07, 0.05, 0.05]) * glow[..., None]
    tex["vb_rift_crust_day_co"] = to_u8(dco)
    vday = np.array([0.24, 0.17, 0.15]) * (0.8 + 0.2 * np.tanh(vb))[..., None]
    tex["vb_rift_vein_day_co"] = to_u8(vday)
    os.makedirs(png_dir, exist_ok=True)
    out = []
    for name, arr in tex.items():
        p = os.path.join(png_dir, name + ".png")
        write_png(p, arr)
        out.append((name, p))
    return out


def to_paa(pngs):
    for name, png in pngs:
        dst = os.path.join(DATA_DIR, name + ".paa")
        r = subprocess.run([IMAGE_TO_PAA, png, dst], capture_output=True, text=True)
        if r.returncode != 0 or not os.path.exists(dst):
            raise RuntimeError("ImageToPAA: " + name + " " + r.stdout[-400:] + r.stderr[-400:])


# ---------------------------------------------------------------- материалы
def _texgens():
    one = ("class TexGen{i}\n{{\n\tuvSource = \"tex\";\n\tclass uvTransform\n\t{{\n"
           "\t\taside[] = {{1,0,0}};\n\t\tup[] = {{0,1,0}};\n\t\tdir[] = {{0,0,1}};\n\t\tpos[] = {{0,0,0}};\n\t}};\n}};\n")
    return "".join(one.format(i=i) for i in range(8))


def rvmat(emissive, nohq, smdi, e_tex, spec, spec_power, fresnel):
    stages = [
        (1, nohq), (2, "#(argb,8,8,3)color(0.5,0.5,0.5,1,DT)"), (3, "#(argb,8,8,3)color(0,0,0,0,MC)"),
        (4, "#(argb,8,8,3)color(1,1,1,1,AS)"), (5, smdi), (6, fresnel), (7, "#(argb,8,8,3)color(0,0,0,1,CO)"),
    ]
    s = "ambient[] = {1,1,1,1};\ndiffuse[] = {1,1,1,1};\nforcedDiffuse[] = {0,0,0,0};\n"
    s += "emmisive[] = {%.3f,%.3f,%.3f,1};\n" % (emissive, emissive, emissive)
    s += "specular[] = {%.3f,%.3f,%.3f,1};\nspecularPower = %d;\n" % (spec, spec, spec, spec_power)
    s += "PixelShaderID = \"Super\";\nVertexShaderID = \"Super\";\n"
    s += _texgens()
    for i, tx in stages:
        s += "class Stage%d\n{\n\ttexture = \"%s\";\n\ttexGen = \"%d\";\n};\n" % (i, tx, i)
    s += "class Stage8\n{\n\ttexture = \"%s\";\n\ttexGen = \"0\";\n};\n" % e_tex
    return s


def write_materials():
    crust = dict(nohq=PREFIX + "vb_rift_crust_nohq.paa", smdi="#(argb,8,8,3)color(1,0.35,0.45,1,SMDI)",
                 e_tex=PREFIX + "vb_rift_crust_e_co.paa", spec=0.35, spec_power=60, fresnel="#(ai,64,64,1)fresnel(1.3,0.3)")
    vein = dict(nohq="#(argb,8,8,3)color(0.5,0.5,1,1,NOHQ)", smdi="#(argb,8,8,3)color(1,0.5,0.6,1,SMDI)",
                e_tex=PREFIX + "vb_rift_vein_e_co.paa", spec=0.5, spec_power=90, fresnel="#(ai,64,64,1)fresnel(1.3,0.3)")
    for name, cfg, levels in (("vb_rift_crust", crust, CRUST_EMISSIVE), ("vb_rift_vein", vein, VEIN_EMISSIVE)):
        for i, e in enumerate(levels):
            with open(os.path.join(DATA_DIR, "%s_%d.rvmat" % (name, i)), "w", encoding="utf-8", newline="\r\n") as f:
                f.write(rvmat(e, **cfg))
        with open(os.path.join(DATA_DIR, name + ".rvmat"), "w", encoding="utf-8", newline="\r\n") as f:
            f.write(rvmat(levels[BASE_LEVEL], **cfg))
        # день: без свечения и с меньшим бликом (сухая кора); текстуру _day_co скрипт ставит сам
        day = dict(cfg)
        day["spec"] = 0.12
        day["spec_power"] = 25
        with open(os.path.join(DATA_DIR, name + "_day.rvmat"), "w", encoding="utf-8", newline="\r\n") as f:
            f.write(rvmat(0.0, **day))


# модель -> (вид, зерно). Класс в config.cpp: VB_RiftHeart, VB_RiftRoots_<Вид><N>.
# Трещин в земле в моде нет: старые VB_RiftCracks и плоские VB_RiftFissure убраны по решению пользователя 2026-11-11.
VARIANTS = [
    ("vb_rift_heart", "heart", SEED),
    ("vb_rift_roots_crawl1", "crawl", 101),
    ("vb_rift_roots_crawl2", "crawl", 102),
    ("vb_rift_roots_fork1", "fork", 201),
    ("vb_rift_roots_cluster1", "cluster", 301),
    ("vb_rift_roots_cluster2", "cluster", 302),
    ("vb_rift_roots_arch1", "arch", 401),
    ("vb_rift_roots_arch2", "arch", 402),
    ("vb_rift_roots_small1", "small", 501),
    ("vb_rift_roots_small2", "small", 502),
    # ещё варианты для разнообразия (2026-11-11)
    ("vb_rift_roots_crawl3", "crawl", 103),
    ("vb_rift_roots_fork2", "fork", 202),
    ("vb_rift_roots_cluster3", "cluster", 303),
    ("vb_rift_roots_small3", "small", 503),
    # обвивки стволов: число в имени — радиус ствола в сантиметрах (скрипт сердца выбирает ближайший больший)
    ("vb_rift_wrap_10", "wrap:0.10", 701),
    ("vb_rift_wrap_15", "wrap:0.15", 702),
    ("vb_rift_wrap_20", "wrap:0.20", 703),
    ("vb_rift_wrap_27", "wrap:0.27", 704),
    ("vb_rift_wrap_35", "wrap:0.35", 705),
    ("vb_rift_wrap_45", "wrap:0.45", 706),
    # свисающие щупальца
    ("vb_rift_hang_short1", "hang:short", 801),
    ("vb_rift_hang_short2", "hang:short", 802),
    ("vb_rift_hang_long1", "hang:long", 803),
    ("vb_rift_hang_long2", "hang:long", 804),
    ("vb_rift_hang_drape1", "hang:drape", 805),
    # корни по стенам домов (ставит скрипт сердца на дома рядом): низкие, высокие, широкие
    ("vb_rift_wall_low1", "wall:low", 1001),
    ("vb_rift_wall_low2", "wall:low", 1002),
    ("vb_rift_wall_tall1", "wall:tall", 1003),
    ("vb_rift_wall_wide1", "wall:wide", 1004),
    ("vb_rift_wall_low3", "wall:low", 1005),
    ("vb_rift_wall_tall2", "wall:tall", 1006),
    ("vb_rift_wall_wide2", "wall:wide", 1007),
    # корни на наружном углу дома: переваливают через ребро на обе стены
    ("vb_rift_wall_corner1", "corner", 1101),
    ("vb_rift_wall_corner2", "corner", 1102),
    ("vb_rift_wall_corner3", "corner", 1103),
    # предмет «Сердце Изнанки» (выпадает при вырезании сердца)
    ("vb_rift_heart_chunk", "chunk", 901),
    # предмет «Корень Изнанки» (иногда выпадает из зомби Изнанки)
    ("vb_rift_root_piece", "rootpiece", 902),
    # предметы-материалы: «Споры Изнанки» (собирают у сердца) и «Жила Изнанки» (разделывают корень)
    ("vb_rift_spores_item", "sporesack", 903),
    ("vb_rift_vein_item", "veinskein", 904),
    # предметы-изделия: «Слушающий корень», «Сердце-аккумулятор», «Споровая ловушка», «Топливная капсула»,
    # «Фонарь-сердце», «Живой жгут», «Нить Изнанки»
    ("vb_rift_listen_root", "listenroot", 905),
    ("vb_rift_spore_trap", "trapgrenade", 907),
    ("vb_rift_lantern", "lantern", 909),
    ("vb_rift_tourniquet", "strap", 910),
    ("vb_rift_thread", "spool", 911),
    ("vb_rift_blood_ampoule", "ampoule", 912),
    ("vb_rift_blade", "bladefull", 913),
    ("vb_rift_heart_battery", "batteryfull", 914),
]


def variant_scene(kind, seed, name=""):
    if kind == "heart":
        return build_scene(seed)
    if kind.startswith("wrap:"):
        return build_wrap_scene(seed, float(kind.split(":")[1]))
    if kind.startswith("hang:"):
        return build_hang_scene(seed, kind.split(":")[1])
    if kind.startswith("wall:"):
        return build_wall_scene(seed, kind.split(":")[1])
    if kind == "corner":
        return build_corner_scene(seed)
    if kind == "chunk":
        return build_chunk_scene(seed)
    if kind == "rootpiece":
        return build_rootpiece_scene(seed)
    if kind == "spores":
        return build_spores_scene(seed)
    if kind == "veincoil":
        return build_vein_coil_scene(seed)
    if kind.startswith("veincoil:"):
        sc = build_vein_coil_scene(seed, float(kind.split(":")[1]))
        sc["item_mass"] = 0.04
        return sc
    if kind == "sporesack":
        return build_spores_sack_scene(seed)
    if kind == "veinskein":
        return build_vein_skein_scene(seed)
    if kind == "heartbattery2":
        return build_heart_battery2_scene(seed)
    if kind == "trapgrenade":
        return build_trap_grenade_scene(seed)
    if kind == "strap":
        return build_strap_scene(seed)
    if kind == "spool":
        return build_spool_scene(seed)
    if kind == "ampoule":
        return build_ampoule_scene(seed)
    if kind == "bladefull":
        return build_blade_full_scene(seed)
    if kind == "batteryfull":
        return build_battery_full_scene(seed)
    if kind == "listenroot":
        return build_listen_root_scene(seed)
    if kind == "heartbattery":
        return build_heart_battery_scene(seed)
    if kind == "sporetrap":
        return build_spore_trap_scene(seed)
    if kind == "fuelcapsule":
        return build_fuel_capsule_scene(seed)
    if kind == "lantern":
        return build_lantern_scene(seed)
    return build_roots_scene(seed, kind)


def write_model_cfg():
    """model.cfg: crust и veins отдельными секциями, иначе SetObjectTexture/SetObjectMaterial не работают."""
    s = "// VB_Rift: сгенерировано gen_rift_veins.py. Секции нужны для смены материалов (пульс, день/ночь).\n"
    s += "class CfgSkeletons\n{\n\tclass Default\n\t{\n\t\tisDiscrete = 1;\n\t\tskeletonInherit = \"\";\n\t\tskeletonBones[] = {};\n\t};\n};\n"
    s += "class CfgModels\n{\n\tclass Default\n\t{\n\t\tsectionsInherit = \"\";\n\t\tsections[] = {};\n\t\tskeletonName = \"\";\n\t};\n"
    s += "\tclass vb_rift_base: Default\n\t{\n\t\tsections[] = {\"crust\", \"veins\"};\n\t};\n"
    for name, _, _ in VARIANTS:
        s += "\tclass %s: vb_rift_base {};\n" % name
    s += "};\n"
    with open(os.path.join(DATA_DIR, "model.cfg"), "w", encoding="utf-8", newline="\r\n") as f:
        f.write(s)


def main():
    """gen_rift_veins.py <папка_для_png> [--only=имя1,имя2]: --only собирает только эти модели
    (подстрока имени), model.cfg пишет целиком, текстуры и материалы не трогает."""
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    only = [o for a in sys.argv if a.startswith("--only=") for o in a.split("=", 1)[1].split(",") if o]
    png_dir = args[0] if args else os.path.join(HERE, "_png")
    os.makedirs(DATA_DIR, exist_ok=True)
    for name, kind, seed in VARIANTS:
        if only and not any(o in name for o in only):
            continue
        scene = variant_scene(kind, seed, name)
        stats, findings = build_p3d(scene, os.path.join(DATA_DIR, name + ".p3d"))
        allp = np.vstack([tb.pts for tb in scene["tubes"]])
        span = float(np.max(np.linalg.norm(allp[:, :2], axis=1)))
        print("%-24s LOD0 %6d тр.  LOD1 %5d  LOD2 %5d  радиус %.1f м  срезателей %d" % (
            name, stats[0][2], stats[1][2], stats[2][2], span, len(cut_points(scene))))
        for f in findings:
            if "LOWCONF" not in f.code and "POS_CENTER" not in f.code:
                print("   validate:", f)
    write_model_cfg()
    if "--props-tex" in sys.argv:
        pngs = make_prop_textures(png_dir)
        to_paa(pngs)
        write_prop_materials()
        print("текстуры предметов:", len(pngs))
    if only:
        print("готово (только модели):", DATA_DIR)
        return
    pngs = make_textures(png_dir)
    if "--no-paa" not in sys.argv:
        to_paa(pngs)
    write_materials()
    print("готово:", DATA_DIR)


if __name__ == "__main__":
    main()
