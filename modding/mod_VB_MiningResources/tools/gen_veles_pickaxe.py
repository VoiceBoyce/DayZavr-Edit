# -*- coding: utf-8 -*-
r"""«Велесова кирка» (VB_MR_Pickaxe_Veles): своя модель, текстуры и материалы (ТЗ_Велесова_кирка.md, разделы 3 и 7).

Что строится (MLOD .p3d через py3d, бинаризуется при сборке `pbo.ps1 build ... -Binarize`):
  навершие — кованый титан: клюв (+Z в осях DayZ) и узкое долото (-Z); по металлу золотые прожилки-трещины,
             на обеих щеках руна Медведя (лапа). Прожилки и руна — выпуклые золотые «валики», отдельные грани
             в секции vbmr_glow (её материал скрипт меняет на светящийся через SetObjectMaterial);
  рукоять  — деревянный стержень, спиралью его оплетают 4 живых корня (местами сходятся и расходятся), между
             ними тонкие отростки и две «обвязки»; под навершием корни выходят на металл «пальцами» и держат его,
             два тонких отростка идут под клюв и под долото;
  торец    — янтарь с тёмным вкраплением (мошка) в корневой «лапе»: те же 4 корня обхватывают его снизу.

Хват как у ванильной кирки (P:\DZ\weapons\melee\blade\pickaxe.p3d, замер .claude\tools\dayz-render\odol_read.py
2026-11-18): начало координат, ось рукояти — Y DayZ, длина -0.245..0.702, навершие на высоте 0.62..0.70, клюв в +Z
до 0.270, долото в -Z до -0.248 (ширина 6.8 см). Проверено на своих моделях: при autocenter 0 координаты MLOD
после бинаризации не меняются, поэтому числа ванили берём как есть.
Memory LOD — точки ванили с теми же именами (invview, boundingbox_min/max, meleerangestart/end, ce_center,
ce_radius, throwingimpulseposition) + vbmr_light (свет «Чует жилу», на щеке рядом с руной).

LOD: 1.0 и 2.0 — полная деталь (правило E79), 4.0 — средняя, 8.0 — простая, 1100 (View Pilot, вид от первого
лица, как у ванили) — полная. Geometry: 4 выпуклых куска (рукоять, обух, клюв, долото), масса 2.5 кг, autocenter 0.
View Geometry — те же куски, Fire Geometry — те же с проникновением iron/wood.

Текстуры (data\veles), запекаются по точкам модели (как в gen_mr_models.py: тексель -> точка и нормаль на модели,
рисунок — 3D-шум, без швов на стыках развёртки), _nohq из производных высоты (DirectX, G вниз):
  vb_mr_veles_co.paa / _nohq.paa / _smdi.paa — общий атлас: навершие, рукоять, корни, янтарь;
  vb_mr_veles_glow_co.paa — золото прожилок и руны (повторяется вдоль валика), vb_mr_veles_glow_e_co.paa — свечение.
Материалы (Super): vb_mr_veles_metal / _wood / _amber (+ _damage, _destruct для healthLevels),
  vb_mr_veles_glow.rvmat (золото, обычное) и vb_mr_veles_glow_on.rvmat (золото со свечением, Stage8 + emmisive,
  как у ванильных светящихся вещей). В конфиге и скрипте пути к .rvmat — БЕЗ ведущего «\» (E75, стенд 2026-11-20).
model.cfg: класс vb_mr_pickaxe_veles, sections[] = {"vbmr_glow"}.

Запуск (Python из Blender, в нём есть numpy):
  "G:\SteamLibrary\steamapps\common\Blender\5.2\python\bin\python.exe" -I gen_veles_pickaxe.py [<папка_для_png>]
      [--models-only] [--p3d-only] [--tex-only] [--no-paa] [--co=2048] [--aux=2048]
  <папка_для_png> — промежуточные PNG (по умолчанию %TEMP%\vb_mr_veles_png).
Проверка картинками: tools\render_veles_pickaxe.py (Blender), см. его заголовок.
"""
import math
import os
import sys
import tempfile
import time

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import gen_mr_models as G  # noqa: E402  шум, меш, запекание, нормали — общие приёмы генератора ресурсов
import ar_theme  # noqa: E402  палитра «Древнего корня» (путь добавил gen_mr_models)

py3d = G.py3d
TL = G.TL
gnoise, fbm, worley, sstep, normalize = G.gnoise, G.fbm, G.worley, G.sstep, G.normalize

BS = chr(92)
OUT_DIR = os.path.join(G.SRC, "data", "veles")
PREFIX = "VB_MiningResources" + BS + "data" + BS + "veles" + BS
NAME = "vb_mr_pickaxe_veles"
TEX_CO = PREFIX + "vb_mr_veles_co.paa"
TEX_GOLD = PREFIX + "vb_mr_veles_glow_co.paa"
MATS = {k: PREFIX + "vb_mr_veles_%s.rvmat" % k for k in ("metal", "wood", "amber", "glow")}
GLOW_SEL = "vbmr_glow"
MASS = 2.5
PEN = {"iron": G.PEN["iron"], "wood": BS.join(["dz", "data", "data", "penetration", "wood.rvmat"])}
ENV = G.ENV

TAG_HEAD, TAG_SHAFT, TAG_ROOT, TAG_AMBER, TAG_GOLD = 0, 1, 2, 3, 4
TAG_MAT = {TAG_HEAD: "metal", TAG_SHAFT: "wood", TAG_ROOT: "wood", TAG_AMBER: "amber", TAG_GOLD: "glow"}

# атлас (u0, v0, u1, v1), v вниз по картинке
CH_HEAD = (0.004, 0.004, 0.996, 0.420)
CH_SHAFT = (0.004, 0.428, 0.996, 0.540)
CH_ROOTS = (0.004, 0.548, 0.996, 0.906)
CH_AMBER = (0.004, 0.914, 0.996, 0.996)

PAL = ar_theme.AR_PALETTE
# цвета sRGB 0..1
TI_DARK = (0.130, 0.140, 0.160)                        # тёмный сине-серый титан
TI_BASE = (0.235, 0.255, 0.285)
TI_LIGHT = (0.360, 0.385, 0.420)
TI_EDGE = (0.580, 0.605, 0.640)
TI_SCALE = (0.150, 0.150, 0.158)
TINT_STRAW, TINT_BRONZE, TINT_PURPLE, TINT_BLUE = (0.72, 0.60, 0.33), (0.58, 0.38, 0.22), (0.38, 0.25, 0.46), \
    (0.20, 0.31, 0.56)
GOLD = PAL["g_vein"]                                   # (0.95, 0.72, 0.20) — золото «Древнего корня»
# кора корней — p_bark «Древнего корня» (0.30, 0.21, 0.11), чуть темнее и серее: на свету она уходила в рыжий
BARK = (0.250, 0.200, 0.140)
BARK_TOP = (0.400, 0.340, 0.250)
BARK_FISS = (0.060, 0.042, 0.026)
MOSS = PAL["p_bark_moss"]                              # (0.20, 0.28, 0.08)
MOSS_DARK = (0.08, 0.12, 0.035)
MOSS_LIGHT = (0.30, 0.38, 0.11)
WOOD_DARK = (0.110, 0.062, 0.035)
WOOD_BASE = (0.215, 0.128, 0.068)
WOOD_LIGHT = (0.330, 0.205, 0.115)
AMB_DARK = (0.30, 0.085, 0.010)                        # янтарь — по gen_ar_amber.py (DARK/MID/LIGHT), глубже
AMB_MID = (0.68, 0.33, 0.045)
AMB_LIGHT = (0.98, 0.71, 0.26)
AMB_INCL = (0.06, 0.030, 0.008)


def log(*a):
    print(*a, flush=True)


def col(c, n):
    return np.tile(np.asarray(c, np.float64), (n, 1))


def lerp(a, b, t):
    return G.lerp(a, b, t)


def orient(P, T, UV, ref):
    """Обход граней наружу (против часовой снаружи, оси Blender): ref — вектор «наружу» в центрах граней."""
    fn = np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]])
    if np.sum((fn * ref).sum(1)) < 0:
        T = T[:, [0, 2, 1]]
        UV = UV[:, [0, 2, 1]]
    return T, UV


# ================================================================ навершие
# Оси Blender: X — толщина (щёки), Y — вдоль навершия (+Y клюв, -Y долото), Z — вверх по рукояти.
H_EDGE, H_TIP = -0.2440, 0.2712      # кромка долота (середина выступает ещё на 4.5 мм), острие клюва
H_YC, H_YB = 0.038, 0.058            # обух |y| < H_YC, переход к клюву и долоту до H_YB
COL_W, COL_ZB, COL_ZT, COL_N = 0.0315, 0.6135, 0.7035, 4.2
M_RING = 480


def head_params(y):
    """Сечение навершия поперёк Y: полутолщина w, центр zc, полувысота hz, показатель суперэллипса n."""
    y = np.asarray(y, np.float64)
    ts = np.clip((y - 0.040) / (H_TIP - 0.040), 0, 1)
    tc = np.clip((-0.040 - y) / (-0.040 - H_EDGE), 0, 1)
    # клюв: сужается к острию, слегка опущен
    ws = 0.0011 + 0.0186 * (1 - ts) ** 1.12
    hs = 0.0010 + 0.0182 * (1 - ts) ** 1.05
    zs = 0.6745 - 0.0175 * ts ** 1.55
    ns = 3.1 - 0.5 * ts
    # долото: сначала чуть уже, к кромке расходится до 6.8 см и становится плоским
    wc = 0.0188 + 0.0154 * tc ** 2.4 - 0.0028 * np.sin(np.pi * tc) * (1 - tc)
    hc = 0.0008 + 0.0181 * (1 - tc) ** 0.95
    zcc = 0.6745 - 0.0265 * tc ** 1.6
    nc = 3.2 + 1.0 * tc
    sp = y > 0
    wa, ha, za, na = (np.where(sp, a, b) for a, b in ((ws, wc), (hs, hc), (zs, zcc), (ns, nc)))
    s = sstep(H_YC, H_YB, np.abs(y))
    col_zt = COL_ZT - 0.0028 * np.clip(y / H_YC, -1, 1) ** 2      # верх обуха чуть скруглён
    zt = col_zt + (za + ha - col_zt) * s
    zb = COL_ZB + (za - ha - COL_ZB) * s
    w = COL_W + (wa - COL_W) * s
    n = COL_N + (na - COL_N) * s
    return w, (zt + zb) / 2, (zt - zb) / 2, n


_PHI = np.linspace(-np.pi / 2, 1.5 * np.pi, M_RING + 1)
_SPAR = np.linspace(0, 1, M_RING + 1)


def head_warp_y(P, w):
    """Кромка долота выпуклая в плане: середина выступает на 4.5 мм."""
    k = sstep(-0.205, H_EDGE, P[:, 1])
    P[:, 1] -= 0.0045 * (1 - np.clip(P[:, 0] / w, -1, 1) ** 2) * k
    return P


def head_ring(y):
    """Сечение при y: точки (M+1, 3) от низа (-Z) через +X, верх, -X обратно к низу; доля длины дуги σ."""
    w, zc, hz, n = (float(v) for v in head_params(y))
    c, s = np.cos(_PHI), np.sin(_PHI)
    e = 2.0 / n
    x = w * np.sign(c) * np.abs(c) ** e
    z = zc + hz * np.sign(s) * np.abs(s) ** e
    P = np.column_stack([x, np.full(len(x), float(y)), z])
    P = head_warp_y(P, w)
    seg = np.linalg.norm(np.diff(P[:, [0, 2]], axis=0), axis=1)
    sig = np.concatenate([[0.0], np.cumsum(seg)])
    return P, sig / sig[-1]


def head_point(y, sig):
    P, sa = head_ring(y)
    sig = sig % 1.0
    return np.array([np.interp(sig, sa, P[:, k]) for k in range(3)])


def head_frame(y, sig):
    """Точка поверхности навершия, нормаль наружу, производные по y и по σ."""
    P = head_point(y, sig)
    ey, es = 2e-4, 1e-4
    Py = (head_point(y + ey, sig) - head_point(y - ey, sig)) / (2 * ey)
    Ps = (head_point(y, sig + es) - head_point(y, sig - es)) / (2 * es)
    n = normalize(np.cross(Py, Ps))
    return P, n, Py, Ps


def head_sigma_side(y, z, side):
    """σ точки щеки (side=+1 — +X, -1 — -X) с заданной высотой z."""
    P, sa = head_ring(y)
    half = (sa <= 0.5) if side > 0 else (sa >= 0.5)
    zz, ss = P[half, 2], sa[half]
    if side < 0:
        zz, ss = zz[::-1], ss[::-1]
    return float(np.interp(z, zz, ss))


def head_ys(d):
    k = (1.0, 0.5, 0.27)[d]
    parts = [(H_EDGE, -0.215, 7), (-0.215, -0.065, 14), (-0.065, -0.034, 11), (-0.034, 0.034, 6),
             (0.034, 0.065, 11), (0.065, 0.225, 14), (0.225, 0.2685, 8)]
    ys = [np.linspace(a, b, max(2, int(round(n * k))) + 1)[:-1] for a, b, n in parts]
    return np.concatenate(ys + [[0.2685]])


def build_head(mesh, d):
    ys = head_ys(d)
    ncol = (32, 16, 8)[d]
    u0, v0, u1, v1 = CH_HEAD
    Gp = np.zeros((len(ys), ncol + 1, 3))
    UVg = np.zeros((len(ys), ncol + 1, 2))
    tj = np.linspace(0, 1, ncol + 1)
    for i, y in enumerate(ys):
        P, sa = head_ring(y)
        tau = 0.55 * sa + 0.45 * _SPAR                  # вершины гуще на рёбрах, текстура — по длине дуги
        fi = np.interp(tj, tau, np.arange(M_RING + 1))
        i0 = np.clip(np.floor(fi).astype(int), 0, M_RING - 1)
        f = fi - i0
        Gp[i] = P[i0] * (1 - f)[:, None] + P[i0 + 1] * f[:, None]
        UVg[i, :, 0] = u0 + (y - H_EDGE) / (H_TIP - H_EDGE) * (u1 - u0)
        UVg[i, :, 1] = v0 + (sa[i0] * (1 - f) + sa[i0 + 1] * f) * (v1 - v0)
    P, T, UV = G.grid_mesh(Gp, UVg)
    zc = head_params(P[:, 1])[1]
    cen = P[T].mean(1)
    ref = cen - np.column_stack([np.zeros(len(cen)), cen[:, 1], head_params(cen[:, 1])[1]])
    T, UV = orient(P, T, UV, ref)
    mesh.add(P, T, UV, tag=TAG_HEAD)
    # торцы: веер к точке кромки долота и к острию клюва
    for row, tip, dirv in ((0, None, -1.0), (len(ys) - 1, None, 1.0)):
        ring = Gp[row]
        ruv = UVg[row]
        y = ys[row]
        w, zc0, hz, _ = head_params(y)
        if dirv < 0:
            tp = np.array([0.0, ring[:, 1].min() - 0.0004, float(zc0)])
        else:
            tp = np.array([0.0, H_TIP, float(head_params(H_TIP)[1])])
        Pc = np.vstack([ring[:-1], tp])
        k = len(ring) - 1
        Tc = np.array([[j, (j + 1) % k, k] for j in range(k)])
        UVc = np.array([[ruv[j], ruv[j + 1], [ruv[0, 0], (ruv[j, 1] + ruv[j + 1, 1]) / 2]] for j in range(k)])
        ref = np.tile([0.0, dirv, 0.0], (len(Tc), 1))
        Tc, UVc = orient(Pc, Tc, UVc, ref)
        mesh.add(Pc, Tc, UVc, tag=TAG_HEAD)


# ================================================================ рукоять
SH_Z0, SH_Z1 = -0.200, 0.650


def shaft_ab(z):
    """Полуоси эллипса рукояти (как у ванили: 1.46 x 2.32 см у торца, к навершию толще) и шейка над янтарём."""
    z = np.asarray(z, np.float64)
    t = (z + 0.245) / 0.95
    neck = 1 - 0.30 * sstep(-0.155, -0.200, z)
    return (0.0146 + 0.0015 * t) * neck, (0.0232 + 0.0018 * t) * neck


def shaft_wob(theta, z):
    th = np.asarray(theta, np.float64)
    zz = np.broadcast_to(np.asarray(z, np.float64), th.shape)
    q = np.column_stack([np.cos(th).ravel() * 1.6, np.sin(th).ravel() * 1.6, zz.ravel() * 14.0])
    return (1 + 0.012 * gnoise(q, 501)).reshape(th.shape)


def shaft_point(theta, z):
    a, b = shaft_ab(z)
    k = shaft_wob(theta, z)
    return np.stack([a * k * np.cos(theta), b * k * np.sin(theta), np.broadcast_to(z, np.shape(theta))], -1)


def shaft_normal(theta, z):
    a, b = shaft_ab(z)
    n = np.stack([np.cos(theta) / a, np.sin(theta) / b, np.zeros(np.shape(theta))], -1)
    return normalize(n.reshape(-1, 3)).reshape(np.shape(n))


def shaft_zs(d):
    step = (0.009, 0.020, 0.060)[d]
    zs = list(np.arange(-0.150, SH_Z1, step)) + [SH_Z1]
    zs = list(np.linspace(SH_Z0, -0.150, (8, 5, 3)[d], endpoint=False)) + zs
    return np.array(zs)


def build_shaft(mesh, d):
    nseg = (14, 10, 6)[d]
    zs = shaft_zs(d)
    th = np.linspace(0, 2 * np.pi, nseg + 1)
    Gp = np.stack([shaft_point(th, z) for z in zs])
    u0, v0, u1, v1 = CH_SHAFT
    UVg = np.zeros(Gp.shape[:2] + (2,))
    UVg[..., 0] = (u0 + (zs - SH_Z0) / (SH_Z1 - SH_Z0) * (u1 - u0))[:, None]
    UVg[..., 1] = (v0 + th / (2 * np.pi) * (v1 - v0))[None, :]
    P, T, UV = G.grid_mesh(Gp, UVg)
    cen = P[T].mean(1)
    T, UV = orient(P, T, UV, cen * np.array([1, 1, 0]))
    mesh.add(P, T, UV, tag=TAG_SHAFT)
    # нижний торец (внутри янтаря)
    ring = Gp[0]
    tp = np.array([0.0, 0.0, SH_Z0 - 0.003])
    k = nseg
    Pc = np.vstack([ring[:-1], tp])
    Tc = np.array([[j, (j + 1) % k, k] for j in range(k)])
    uv = UVg[0]
    UVc = np.array([[uv[j], uv[(j + 1) % k], [uv[0, 0], 0.5 * (v0 + v1)]] for j in range(k)])
    Tc, UVc = orient(Pc, Tc, UVc, np.tile([0, 0, -1.0], (k, 1)))
    mesh.add(Pc, Tc, UVc, tag=TAG_SHAFT)


# ================================================================ янтарь
AMB_C = np.array([0.0, 0.0, -0.2200])
AMB_R = np.array([0.0250, 0.0262, 0.0275])


def amber_point(dirs):
    d = normalize(np.atleast_2d(dirs))
    re = 1.0 / np.sqrt(((d / AMB_R) ** 2).sum(1))
    lump = 1 + 0.040 * fbm(d * 1.3 + 33.0, 3, seed=611) + 0.012 * fbm(d * 3.5, 2, seed=612)
    return AMB_C + d * (re * lump)[:, None]


def amber_normal(dirs):
    d = normalize(np.atleast_2d(dirs))
    ref = np.where(np.abs(d[:, 2:3]) < 0.9, [[0, 0, 1.0]], [[1.0, 0, 0]])
    t1 = normalize(np.cross(d, ref))
    t2 = np.cross(d, t1)
    e = 2e-3
    pa = amber_point(d + e * t1) - amber_point(d - e * t1)
    pb = amber_point(d + e * t2) - amber_point(d - e * t2)
    n = normalize(np.cross(pa, pb))
    return np.where((n * d).sum(1, keepdims=True) < 0, -n, n)


def build_amber(mesh, d):
    N = (8, 5, 3)[d]
    u0, v0, u1, v1 = CH_AMBER
    side = v1 - v0
    t = np.tan(np.linspace(-1, 1, N + 1) * math.pi / 4)
    for fi, (ax, au, av) in enumerate(G.CUBE_FACES):
        S, Tt = np.meshgrid(t, t, indexing="xy")
        dirs = normalize(ax[None, None, :] + S[..., None] * au + Tt[..., None] * av).reshape(-1, 3)
        p = amber_point(dirs)
        nrm = amber_normal(dirs)
        su = np.linspace(0, 1, N + 1)
        U, V = np.meshgrid(su, su[::-1], indexing="xy")
        ins = 0.04
        UVg = np.stack([u0 + (fi + ins + U * (1 - 2 * ins)) * side * 1.02, v0 + (ins + V * (1 - 2 * ins)) * side], -1)
        P, T, UV = G.grid_mesh(p.reshape(N + 1, N + 1, 3), UVg)
        T, UV = orient(P, T, UV, P[T].mean(1) - AMB_C)
        mesh.add(P, T, UV, N=nrm, tag=TAG_AMBER)


# ================================================================ корни
Z_T, Z_B = 0.540, -0.150            # спираль по рукояти
TURNS = 1.55
CORNERS = [(+1, +1), (-1, +1), (-1, -1), (+1, -1)]   # к какому углу обуха идёт корень (знак X, знак Y)
R_MAIN = [0.0050, 0.0045, 0.0052, 0.0046]
EMB_SHAFT = 0.72                    # доля толщины корня, утопленная в рукоять


def corner_theta(sx, sy):
    a, b = shaft_ab(Z_T)
    return math.atan2(sy * 0.038 / float(b), sx * 0.0315 / float(a)) % (2 * math.pi)


# Где соседние корни сходятся до касания: (номер пары соседей по кругу, высота z). Пары с общим корнем
# разнесены по высоте, чтобы сдвиги одного корня не гасили друг друга.
MEETS = [(0, 0.34), (1, 0.18), (2, 0.40), (2, 0.02), (3, 0.24), (3, -0.09)]
MEET_W = 0.034


def _gap_arc(th, i1, i2, z):
    a, b = shaft_ab(z)
    dth = (th[i2] - th[i1]) % (2 * math.pi)
    mid = th[i1] + dth / 2
    return dth, dth * np.sqrt((a * np.sin(mid)) ** 2 + (b * np.cos(mid)) ** 2)


def make_spiral_thetas():
    """Углы 4 корней по рукояти: общая закрутка, лёгкая волна и сближения MEETS (корни сходятся до касания
    и снова расходятся). Проверка: корни не проходят друг сквозь друга (зазор не меньше -1.5 мм)."""
    zz = np.linspace(Z_B, Z_T, 400)
    u = (Z_T - zz) / (Z_T - Z_B)
    base = np.array([corner_theta(*c) for c in CORNERS])
    order = np.argsort(base)
    rng = np.random.default_rng(9100)
    th = np.zeros((4, len(zz)))
    for i in range(4):
        m = np.zeros(len(zz))
        for k in (2, 3, 5):
            m += rng.normal(0, 0.10) * np.sin(math.pi * k * u + rng.uniform(0, 2 * math.pi))
        th[i] = base[i] - 2 * math.pi * TURNS * u + m * np.sin(math.pi * u) ** 0.7
    for j, ze in MEETS:
        i1, i2 = order[j], order[(j + 1) % 4]
        k = int(np.argmin(np.abs(zz - ze)))
        _, arc = _gap_arc(th[:, k:k + 1], i1, i2, zz[k:k + 1])
        a, b = shaft_ab(ze)
        rloc = math.sqrt(float(a) * float(b))
        target = (R_MAIN[i1] + R_MAIN[i2]) * 0.92
        delta = (float(arc[0]) - target) / rloc
        g = np.exp(-0.5 * ((zz - ze) / MEET_W) ** 2) * np.sin(math.pi * u) ** 0.5
        th[i1] += 0.5 * delta * g
        th[i2] -= 0.5 * delta * g
    gaps = []
    for j in range(4):
        i1, i2 = order[j], order[(j + 1) % 4]
        _, arc = _gap_arc(th, i1, i2, zz)
        gaps.append(float((arc - (R_MAIN[i1] + R_MAIN[i2])).min()))
    if min(gaps) < -0.0015:
        raise RuntimeError("корни проходят друг сквозь друга: зазор %.1f мм" % (1000 * min(gaps)))
    log("  корни: сближений %d, мин. зазор %.1f мм" % (len(MEETS), 1000 * min(gaps)))
    return zz, th


SPIRAL_Z, SPIRAL_TH = None, None


def spiral_theta(i, z):
    return np.interp(z, SPIRAL_Z, SPIRAL_TH[i])


def root_radius_shaft(i, z):
    scalar = np.ndim(z) == 0
    z = np.atleast_1d(np.asarray(z, np.float64))
    r = R_MAIN[i] * (1 + 0.10 * fbm(np.column_stack([z * 30, z * 0 + i * 7.3, z * 0]), 3, seed=620 + i))
    rng = np.random.default_rng(640 + i)
    for _ in range(3):                                   # узлы-утолщения
        zk = rng.uniform(-0.10, 0.48)
        r *= 1 + 0.22 * np.exp(-((z - zk) / 0.007) ** 2)
    r *= 1 + 0.30 * sstep(0.44, Z_T, z) + 0.15 * sstep(-0.10, Z_B, z)
    r *= 1 - 0.40 * grip_zone(z)                         # под ладонью корни стёрты и тоньше
    return float(r[0]) if scalar else r


GRIP_Z = (-0.060, 0.070)             # хват правой руки: начало координат модели (как у ванили) ± пол-ладони


def grip_zone(z):
    z = np.asarray(z, np.float64)
    return sstep(GRIP_Z[0] - 0.03, GRIP_Z[0], z) * sstep(GRIP_Z[1] + 0.03, GRIP_Z[1], z)


def shaft_root_center(theta, z, r, emb=EMB_SHAFT):
    return shaft_point(theta, z) + shaft_normal(theta, z) * (r * (1 - emb))[..., None]


def catmull(K, R, n_per=10):
    """Центростремительный Catmull-Rom через точки K (радиусы R линейно) -> плотная ломаная."""
    K = np.asarray(K, np.float64)
    R = np.asarray(R, np.float64)
    P = np.vstack([2 * K[0] - K[1], K, 2 * K[-1] - K[-2]])
    out, outr = [], []
    for i in range(1, len(P) - 2):
        p0, p1, p2, p3 = P[i - 1], P[i], P[i + 1], P[i + 2]
        t0 = 0.0
        t1 = t0 + max(np.linalg.norm(p1 - p0), 1e-9) ** 0.5
        t2 = t1 + max(np.linalg.norm(p2 - p1), 1e-9) ** 0.5
        t3 = t2 + max(np.linalg.norm(p3 - p2), 1e-9) ** 0.5
        for j in range(n_per):
            t = t1 + (t2 - t1) * j / n_per
            a1 = (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1
            a2 = (t2 - t) / (t2 - t1) * p1 + (t - t1) / (t2 - t1) * p2
            a3 = (t3 - t) / (t3 - t2) * p2 + (t - t2) / (t3 - t2) * p3
            b1 = (t2 - t) / (t2 - t0) * a1 + (t - t0) / (t2 - t0) * a2
            b2 = (t3 - t) / (t3 - t1) * a2 + (t - t1) / (t3 - t1) * a3
            out.append((t2 - t) / (t2 - t1) * b1 + (t - t1) / (t2 - t1) * b2)
            outr.append(R[i - 1] + (R[i] - R[i - 1]) * j / n_per)
    out.append(K[-1])
    outr.append(R[-1])
    return np.array(out), np.array(outr)


def arclen(P):
    return np.concatenate([[0.0], np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))])


def resample(P, R, s_new):
    s = arclen(P)
    return np.column_stack([np.interp(s_new, s, P[:, k]) for k in range(3)]), np.interp(s_new, s, R)


def head_surface_path(ys, sigs, rs, emb):
    pts = []
    for y, sg, r in zip(ys, sigs, rs):
        P, n, _, _ = head_frame(y, sg)
        pts.append(P + n * r * (1 - emb))
    return np.array(pts)


def amber_path(alpha, lats, rs, emb, twist):
    pts = []
    for k, (la, r, e) in enumerate(zip(lats, rs, emb)):
        al = alpha + twist * k / max(1, len(lats) - 1)
        d = np.array([[math.cos(la) * math.cos(al), math.cos(la) * math.sin(al), math.sin(la)]])
        p = amber_point(d)[0]
        nn = amber_normal(d)[0]
        pts.append(p + nn * r * (1 - e))
    return np.array(pts)


def main_root(i):
    """Корень i: кончик на щеке обуха -> угол обуха -> раскрылся вниз к рукояти -> спираль -> лапа на янтаре."""
    sx, sy = CORNERS[i]
    r0 = R_MAIN[i]
    side = (lambda s: s) if sx > 0 else (lambda s: 1.0 - s)
    # 1) «палец»: кончик лежит на щеке клюва/долота, корень обнимает плечо обуха и уходит под его низ
    sg = np.array([0.252, 0.250, 0.243, 0.222, 0.192, 0.160, 0.128, 0.098])
    ys = sy * np.array([0.071, 0.063, 0.054, 0.0455, 0.0405, 0.0378, 0.0366, 0.0356])
    rs = r0 * np.array([0.20, 0.46, 0.74, 0.95, 1.08, 1.17, 1.23, 1.26])
    k_f = np.linspace(0, 1, 40)
    ysf = np.interp(k_f, np.linspace(0, 1, len(ys)), ys)
    sgf = side(np.interp(k_f, np.linspace(0, 1, len(sg)), sg))
    rsf = np.interp(k_f, np.linspace(0, 1, len(rs)), rs)
    finger = head_surface_path(ysf, sgf, rsf, 0.38)
    # 2) под обухом и вниз к рукояти (свободно, «держит» металл)
    th_t = float(spiral_theta(i, Z_T))
    r_t = float(root_radius_shaft(i, Z_T))
    p_t = shaft_root_center(np.array(th_t), Z_T, np.array(r_t))
    a, b = shaft_ab(0.585)
    dirc = normalize(np.array([[sx * 0.0315 / float(a), sy * 0.038 / float(b), 0.0]]))[0]
    p_flare = np.array([dirc[0] * 0.033, dirc[1] * 0.040, 0.586])
    # 3) спираль по рукояти
    zz = np.linspace(Z_T, Z_B, 360)
    th = spiral_theta(i, zz)
    rr = root_radius_shaft(i, zz)
    spiral = shaft_root_center(th, zz, rr, EMB_SHAFT + 0.14 * grip_zone(zz))
    # 4) лапа на янтаре: от шейки рукояти по плечу янтаря вниз, кончик ниже экватора
    th_b = float(spiral_theta(i, Z_B))
    pb = shaft_point(np.array(th_b), Z_B)
    alpha = math.atan2(float(pb[1]), float(pb[0]))
    r_b = float(root_radius_shaft(i, Z_B))
    nb = shaft_normal(np.array(th_b), -0.170)
    p_neck = shaft_point(np.array(th_b), -0.170) + nb * r_b * 0.85
    # коготь: обхватывает янтарь по плечу, закручиваясь, ниже экватора сужается и загибается внутрь
    kk = np.linspace(0, 1, 40)
    lats = np.radians(40 - 100 * kk ** 1.1)
    rcl = r_b * np.interp(kk, [0, 0.25, 0.45, 0.62, 0.85, 1.0], [0.92, 0.74, 0.78, 0.66, 0.40, 0.10])
    emb = np.interp(kk, [0, 0.70, 1.0], [0.18, 0.22, 0.70])
    claw = amber_path(alpha, lats, rcl, emb, 0.85)
    K = np.vstack([finger, p_flare, spiral[::6], spiral[-1], p_neck, claw])
    R = np.concatenate([rsf, [r0 * 1.28], rr[::6], [rr[-1], r_b], rcl])
    P, Rr = catmull(K, R, 6)
    return {"name": "main%d" % i, "P": P, "R": Rr, "segs": (8, 6, 4), "step": (0.013, 0.024, 0.045),
            "alpha": alpha + 0.26}                      # азимут когтя у экватора янтаря (с учётом закрутки)


def root_at(rootP, z):
    """Точка спирали корня на высоте z (центр)."""
    i = int(np.argmin(np.abs(rootP[:, 2] - z) + (np.abs(rootP[:, 2]) > 0.6) * 9))
    return rootP[i]


def rootlet_shaft(ia, za, ib, zb, r, bulge, seed):
    """Тонкий отросток по рукояти: отходит от корня ia на za и врастает в корень ib на zb."""
    tha, thb = float(spiral_theta(ia, za)), float(spiral_theta(ib, zb))
    dth = (thb - tha + math.pi) % (2 * math.pi) - math.pi
    u = np.linspace(0, 1, 60)
    rng = np.random.default_rng(seed)
    th = tha + dth * u + bulge * np.sin(math.pi * u) + 0.06 * np.sin(2 * math.pi * u * 2 + rng.uniform(0, 6))
    z = za + (zb - za) * u
    rr = r * (0.75 + 0.25 * np.sin(math.pi * u) ** 0.5) * (1 + 0.10 * np.sin(u * 23 + seed))
    P = shaft_root_center(th, z, rr, 0.55)
    return {"name": "rootlet", "P": P, "R": rr, "segs": (6, 4, 0), "step": (0.010, 0.020, 0.0)}


def rootlet_arm(i, sgn):
    """Тонкий отросток под клювом (sgn=+1) или под долотом (-1): от «пальца» корня i вдоль низа."""
    ys = sgn * np.linspace(0.040, 0.118, 30)
    sg = 0.02 * np.sin(np.linspace(0, 3.0, 30)) * sgn
    rs = np.interp(np.linspace(0, 1, 30), [0, 0.2, 1.0], [0.0028, 0.0024, 0.0005])
    P = head_surface_path(ys, sg % 1.0, rs, 0.40)
    sx, sy = CORNERS[i]
    p0 = np.array([sx * 0.020, sy * 0.034, COL_ZB - 0.002])
    P = np.vstack([p0, P])
    rs = np.concatenate([[0.0030], rs])
    P, rs = catmull(P[::2], rs[::2], 6)
    return {"name": "armlet", "P": P, "R": rs, "segs": (6, 4, 3), "step": (0.006, 0.014, 0.03)}


def binding(z0, z1, turns, r, start_root, seed):
    """«Обвязка»: корень несколько раз обвивает рукоять поверх главных корней (проходит над ними)."""
    th0 = float(spiral_theta(start_root, z0))
    u = np.linspace(0, 1, int(160 * turns))
    th = th0 + 2 * math.pi * turns * u
    z = z0 + (z1 - z0) * u
    rng = np.random.default_rng(seed)
    rr = r * np.interp(u, [0, 0.06, 0.94, 1.0], [0.45, 1.0, 1.0, 0.35]) * (1 + 0.12 * np.sin(u * 19 + seed))
    for _ in range(2):                                   # узлы
        rr *= 1 + 0.28 * np.exp(-((u - rng.uniform(0.2, 0.8)) / 0.05) ** 2)
    z = z + 0.0018 * np.sin(2 * math.pi * u * 2.3 + seed)
    a, b = shaft_ab(z)
    Rl = np.sqrt((a * np.cos(th)) ** 2 + (b * np.sin(th)) ** 2)
    lift = 0.35 * rr
    for i in range(4):
        thi = spiral_theta(i, z)
        dd = (th - thi + math.pi) % (2 * math.pi) - math.pi
        ri = root_radius_shaft(i, z)
        wth = (ri + rr) / Rl * 1.05
        lift = np.maximum(lift, np.exp(-(dd / wth) ** 2) * ((1 - EMB_SHAFT) * ri + ri + 0.55 * rr))
    P = shaft_point(th, z) + shaft_normal(th, z) * lift[:, None]
    return {"name": "binding", "P": P, "R": rr, "segs": (8, 5, 0), "step": (0.008, 0.016, 0.0)}


def all_roots():
    roots = [main_root(i) for i in range(4)]
    roots += [rootlet_shaft(0, 0.36, 1, 0.16, 0.0021, 0.35, 1),
              rootlet_shaft(2, 0.10, 3, -0.08, 0.0019, -0.30, 2),
              rootlet_shaft(1, 0.46, 2, 0.28, 0.0020, 0.28, 3),
              rootlet_shaft(3, -0.02, 0, -0.12, 0.0018, 0.22, 4)]
    roots += [rootlet_arm(0, +1), rootlet_arm(2, -1)]
    roots += [binding(0.490, 0.515, 1.10, 0.0037, 1, 5), binding(-0.130, -0.114, 1.0, 0.0034, 3, 6)]
    for k, r in enumerate(roots):
        r["rid"] = k
        r["L"] = float(arclen(r["P"])[-1])
    return roots


def pack_root_charts(roots):
    """Развёртка корней: каждая трубка — полоса (вдоль — длина, поперёк — окружность), длинные режутся на куски.
    Плотность подбирается так, чтобы всё влезло в CH_ROOTS. Возвращает куски: (rid, s0, s1, u0, v0, ku, kv)."""
    u0, v0, u1, v1 = CH_ROOTS
    W, H = u1 - u0, v1 - v0
    gut = 0.004

    def attempt(D):
        lmax = (W - 2 * gut) / D
        pieces = []
        for r in roots:
            n = int(math.ceil(r["L"] / lmax))
            circ = 2 * math.pi * float(r["R"].max()) * 1.05
            for k in range(n):
                pieces.append((r["rid"], r["L"] * k / n, r["L"] * (k + 1) / n, circ))
        pieces.sort(key=lambda p: -p[3])
        x, y, row = gut, gut, 0.0
        out = []
        for rid, s0, s1, circ in pieces:
            w, h = (s1 - s0) * D, circ * D
            if x + w + gut > W:
                x, y, row = gut, y + row + gut, 0.0
            out.append((rid, s0, s1, u0 + x, v0 + y, D, h))
            x += w + gut
            row = max(row, h)
        return out if y + row + gut <= H else None

    lo, hi = 0.05, 20.0
    for _ in range(40):
        mid = (lo + hi) / 2
        if attempt(mid):
            lo = mid
        else:
            hi = mid
    out = attempt(lo)
    log("  развёртка корней: %d кусков, %.2f пикс/мм при 2048" % (len(out), lo * 2048 / 1000))
    return out


def adaptive_s(P, R, step_max, ang_max):
    """Шаги колец вдоль корня: не длиннее step_max, гуще на изгибах (поворот > ang_max) и где меняется толщина."""
    s = arclen(P)
    T = normalize(np.gradient(P, axis=0))
    out = [0.0]
    lt, ls, lr = T[0], 0.0, R[0]
    cmax = math.cos(ang_max)
    for i in range(1, len(P)):
        ds = s[i] - ls
        if ds < 0.0015:
            continue
        if ds >= step_max or np.dot(T[i], lt) < cmax or abs(R[i] - lr) > 0.22 * max(lr, 1e-4):
            out.append(s[i])
            lt, ls, lr = T[i], s[i], R[i]
    out.append(s[-1])
    return np.array(out)


def build_root(mesh, root, pieces, d):
    segs = root["segs"][d]
    if segs == 0:
        return
    step = root["step"][d]
    L = root["L"]
    bounds = sorted({p[1] for p in pieces if p[0] == root["rid"]} | {L})
    s_ad = adaptive_s(root["P"], root["R"], step, math.radians((11.0, 20.0, 34.0)[d]))
    s_new = np.unique(np.concatenate([s_ad, bounds, [L]]))
    P, R = resample(root["P"], root["R"], s_new)
    Tn, Nn, Bn = G.frames(P)
    a = np.linspace(0, 2 * math.pi, segs + 1)
    ring = P[:, None, :] + R[:, None, None] * (np.cos(a)[None, :, None] * Nn[:, None, :]
                                               + np.sin(a)[None, :, None] * Bn[:, None, :])
    for (rid, s0, s1, pu, pv, D, h) in [p for p in pieces if p[0] == root["rid"]]:
        sel = np.nonzero((s_new >= s0 - 1e-9) & (s_new <= s1 + 1e-9))[0]
        if len(sel) < 2:
            continue
        Gp = ring[sel]
        UVg = np.zeros(Gp.shape[:2] + (2,))
        UVg[..., 0] = (pu + (s_new[sel] - s0) * D)[:, None]
        UVg[..., 1] = (pv + a / (2 * math.pi) * h)[None, :]
        Pm, T, UV = G.grid_mesh(Gp, UVg)
        Tt = np.repeat(Tn[sel], segs + 1, axis=0)
        cen = np.repeat(P[sel], segs + 1, axis=0)
        attrs = {"tx": Tt[:, 0], "ty": Tt[:, 1], "tz": Tt[:, 2],
                 "rr": np.repeat(R[sel], segs + 1), "rid": np.full(len(Pm), float(rid)),
                 "rs": np.repeat(s_new[sel], segs + 1)}
        ref = Pm[T].mean(1) - cen[T].mean(1)
        T, UV = orient(Pm, T, UV, ref)
        mesh.add(Pm, T, UV, tag=TAG_ROOT, attrs=attrs)
        # кончики: конус к точке (концы корней)
        for end, sg in ((sel[0], -1), (sel[-1], 1)):
            if not (end == 0 or end == len(s_new) - 1):
                continue
            rg = ring[end]
            tp = P[end] + sg * Tn[end] * max(R[end], 0.0003) * 1.6
            k = segs
            Pc = np.vstack([rg[:-1], tp])
            Tc = np.array([[j, (j + 1) % k, k] for j in range(k)])
            uu = pu + (s_new[end] - s0) * D
            UVc = np.array([[[uu, pv + a[j] / (2 * math.pi) * h], [uu, pv + a[j + 1] / (2 * math.pi) * h],
                             [uu, pv + 0.5 * h]] for j in range(k)])
            Tc, UVc = orient(Pc, Tc, UVc, np.tile(sg * Tn[end], (k, 1)))
            endv = {"tx": Tn[end][0], "ty": Tn[end][1], "tz": Tn[end][2], "rr": R[end], "rs": s_new[end],
                    "rid": float(rid)}
            mesh.add(Pc, Tc, UVc, tag=TAG_ROOT, attrs={kk: np.full(k + 1, endv[kk]) for kk in attrs})


# ================================================================ золото: прожилки и руна
RUNE_Z = 0.6605
RUNE_S = 0.0215                     # половина размера знака
# Руна Медведя — лапа: широкая подушка и пять когтей веером, концы когтей загнуты внутрь.
# Координаты (a — вдоль Y, b — вверх), знак симметричный (с обеих щёк читается одинаково).
def _rune_glyph():
    """Штрихи знака: (точки, заострённый конец, полуширина в долях RUNE_HW)."""
    out = []
    t = np.linspace(0, 2 * math.pi, 29)
    x = 0.62 * np.cos(t)
    y = -0.30 + np.where(np.sin(t) > 0, 0.36, 0.40) * np.sin(t)
    y -= 0.07 * np.exp(-(x / 0.22) ** 2) * (np.sin(t) > 0)          # подушка лапы: широкая, сверху ямка
    out.append((list(zip(x, y)), False, 1.0))
    for ang in (-64, -32, 0, 32, 64):                                 # пальцы и когти веером
        a = math.radians(ang)
        d = lambda r, aa: (r * math.sin(aa), -0.02 + r * math.cos(aa))
        out.append(([d(0.44, a), d(0.57, a)], False, 1.40))
        out.append(([d(0.68, a), d(0.83, a * 1.05), d(0.97, a * 0.90)], True, 0.90))
    return out


RUNE_STROKES = _rune_glyph()
RUNE_HW = 0.00118                   # полуширина штриха руны
RUNE_EXCL = (0.0265, 0.0245)        # область руны на щеке (|y|, |z - RUNE_Z|): прожилки туда не заходят


def rune_strokes():
    out = []
    for side in (+1, -1):
        for pts, pointed, wk in RUNE_STROKES:
            K = np.array(pts, np.float64)
            dense = []
            for k in range(len(K) - 1):
                n = max(2, int(np.linalg.norm(K[k + 1] - K[k]) * RUNE_S / 0.0014))
                for t in np.linspace(0, 1, n, endpoint=False):
                    dense.append(K[k] + (K[k + 1] - K[k]) * t)
            dense.append(K[-1])
            dense = np.array(dense)
            P, Nn = [], []
            for a, b in dense:
                y = a * RUNE_S * (1 if side > 0 else -1)          # с щеки -X знак читается не зеркально
                z = RUNE_Z + b * RUNE_S - 0.0015
                sg = head_sigma_side(y, z, side)
                p, n, _, _ = head_frame(y, sg)
                P.append(p)
                Nn.append(n)
            P, Nn = np.array(P), np.array(Nn)
            s = arclen(P)
            L = s[-1]
            closed = np.linalg.norm(P[0] - P[-1]) < 1e-6
            hw0 = RUNE_HW * wk
            if closed:
                hw = np.full(len(P), hw0)
            else:
                end = min(0.0010, 0.3 * L)
                hw = hw0 * np.minimum(1.0, 0.55 + 0.45 * sstep(0.0, end, s))
                if pointed:
                    hw *= np.clip((L - s) / 0.0035, 0.0, 1.0) ** 0.7
                else:
                    hw *= np.minimum(1.0, 0.55 + 0.45 * sstep(L, L - end, s))
            out.append({"kind": "rune", "P": P, "N": Nn, "HW": hw, "s": s, "main": True})
    return out


def in_rune(P, n):
    return abs(n[0]) > 0.85 and abs(P[1]) < RUNE_EXCL[0] and abs(P[2] - RUNE_Z) < RUNE_EXCL[1]


def walk_vein(y, sg, d, budget, w0, rng, target, depth, out, step=0.003):
    """Трещина, залитая золотом: случайное блуждание по поверхности навершия с изломами и ветвлением."""
    pts, nrm = [], []
    s = 0.0
    turn = 0.0
    while s <= budget:
        P, n, Py, Ps = head_frame(y, sg)
        d = normalize(d - n * np.dot(d, n))
        if (y > 0.214 or y < -0.198) or (n[2] < -0.80) or (len(pts) > 3 and in_rune(P, n)):
            break
        pts.append(P)
        nrm.append(n)
        # излом и плавный поворот
        turn = 0.65 * turn + rng.normal(0, 0.11)
        ang = turn + (rng.normal(0, 0.55) if rng.random() < 0.07 else 0.0)
        b = np.cross(n, d)
        d = normalize(d * math.cos(ang) + b * math.sin(ang))
        tg = normalize(target - n * np.dot(target, n))
        d = normalize(d + 0.16 * tg)
        if n[2] < -0.35:                                   # снизу не видно — тянуть вверх
            up = normalize(np.array([0, 0, 1.0]) - n * n[2])
            d = normalize(d + 0.35 * up)
        # ветка
        if depth < 2 and s > 0.012 and rng.random() < 0.040 and budget - s > 0.02:
            ba = (1 if rng.random() < 0.5 else -1) * rng.uniform(0.45, 0.85)
            bd = normalize(d * math.cos(ba) + b * math.sin(ba))
            walk_vein(y, sg, bd, min((budget - s) * 0.65, rng.uniform(0.018, 0.055)),
                      w0 * 0.66 * (1 - 0.6 * s / budget), rng, target, depth + 1, out, step)
        J = np.column_stack([Py, Ps])
        dp = np.linalg.lstsq(J, step * d, rcond=None)[0]
        y += dp[0]
        sg = (sg + dp[1]) % 1.0
        s += step
    if len(pts) < 4:
        return
    P = np.array(pts)
    s = arclen(P)
    L = s[-1]
    if depth > 0 and L < 0.009:                          # мелкие веточки не нужны
        return
    u = s / max(L, 1e-9)
    hw = 0.5 * w0 * (1 - 0.70 * u ** 1.3) * (0.85 + 0.15 * np.sin(s * 900 + rng.uniform(0, 6)))
    hw *= sstep(0.0, 0.0020 if depth else 0.0060, s) * np.clip((L - s) / 0.004, 0, 1) ** 0.8
    hw = np.maximum(hw, 0.0)
    out.append({"kind": "vein", "P": P, "N": np.array(nrm), "HW": hw, "s": s, "main": depth == 0, "L": L})


def vein_strokes():
    out = []
    for side, seed in ((+1, 7711), (-1, 7712)):
        rng = np.random.default_rng(seed)
        # (угол от центра руны в плоскости щеки, бюджет длины, ширина)
        plan = [(rng.uniform(16, 28), 0.20, 0.0021), (rng.uniform(-62, -50), 0.085, 0.0015),
                (rng.uniform(198, 212), 0.19, 0.0021), (rng.uniform(132, 144), 0.085, 0.0015),
                (rng.uniform(84, 96), 0.075, 0.0014)]
        for ang, budget, w0 in plan:
            a = math.radians(ang)
            r0 = 0.0285
            y = side * 0 + r0 * math.cos(a) * (1 if side > 0 else -1)
            z = RUNE_Z + r0 * math.sin(a) * 0.80
            z = min(z, COL_ZT - 0.004)
            sg = head_sigma_side(y, z, side)
            P, n, Py, Ps = head_frame(y, sg)
            d = normalize(np.array([0.0, math.cos(a) * (1 if side > 0 else -1), math.sin(a)]))
            if ang > 60 and ang < 120:                     # через верх обуха на другую щёку
                target = np.array([-side, rng.uniform(-0.3, 0.3), 0.6])
            else:
                target = np.array([0.0, np.sign(math.cos(a)) * (1 if side > 0 else -1), 0.05])
            walk_vein(y, sg, d, budget, w0, rng, normalize(target), 0, out)
    return out


def bead_mesh(mesh, st, d):
    """Выпуклый золотой валик вдоль штриха: края утоплены в металл, середина выше поверхности."""
    keep_every = (1, 2, 4)[d]
    P, Nn, hw, s = st["P"], st["N"], st["HW"], st["s"]
    if d > 0 and len(P) > 3:
        idx = np.unique(np.concatenate([np.arange(0, len(P), keep_every), [len(P) - 1]]))
        P, Nn, hw, s = P[idx], Nn[idx], hw[idx], s[idx]
    if len(P) < 2:
        return
    Tn = normalize(np.gradient(P, axis=0))
    Bn = normalize(np.cross(Tn, Nn))
    sink, top = ((0.00030, 0.00055), (0.00055, 0.00070), (0.0010, 0.0009))[d]
    if d == 0:
        e = np.array([-1.0, -0.42, 0.42, 1.0])
        hh = np.array([-sink, 0.92 * top, 0.92 * top, -sink])
    else:
        e = np.array([-1.0, 0.0, 1.0])
        hh = np.array([-sink, top, -sink])
    scale_h = np.clip(hw / 0.0008, 0.35, 1.0)            # тонкий конец ниже
    Gp = (P[:, None, :] + Bn[:, None, :] * (hw[:, None] * e[None, :])[..., None]
          + Nn[:, None, :] * (hh[None, :] * scale_h[:, None])[..., None])
    UVg = np.zeros(Gp.shape[:2] + (2,))
    UVg[..., 0] = (s * 30.0)[:, None]
    UVg[..., 1] = ((e + 1) / 2)[None, :]
    Pm, T, UV = G.grid_mesh(Gp, UVg)
    T, UV = orient(Pm, T, UV, np.repeat(Nn, len(e), axis=0)[T].mean(1))
    mesh.add(Pm, T, UV, tag=TAG_GOLD)


def gold_strokes_for_lod(strokes, d):
    if d == 0:
        return strokes
    out = []
    for st in strokes:
        if st["kind"] == "rune":
            out.append(st)
        elif d == 1 and (st["main"] or st.get("L", 0) > 0.02):
            out.append(st)
        elif d == 2 and st["main"] and st.get("L", 0) > 0.05:
            out.append(st)
    return out


# ================================================================ сборка мешей
class Model:
    def __init__(self):
        global SPIRAL_Z, SPIRAL_TH
        t0 = time.time()
        SPIRAL_Z, SPIRAL_TH = make_spiral_thetas()
        self.roots = all_roots()
        self.pieces = pack_root_charts(self.roots)
        # азимуты когтей на высоте вкрапления (по самим путям корней)
        CLAW_ALPHAS[:] = []
        for r in self.roots[:4]:
            P = r["P"][(r["P"][:, 2] < AMB_C[2] + 0.004) & (r["P"][:, 2] > AMB_C[2] - 0.012)]
            CLAW_ALPHAS.extend(np.arctan2(P[:, 1], P[:, 0]).tolist())
        self.strokes = rune_strokes() + vein_strokes()
        nv = sum(1 for s in self.strokes if s["kind"] == "vein")
        lv = sum(float(s["s"][-1]) for s in self.strokes if s["kind"] == "vein")
        log("  золото: прожилок %d (%.2f м), штрихов руны %d; %.1f с" % (
            nv, lv, sum(1 for s in self.strokes if s["kind"] == "rune"), time.time() - t0))

    def lod_meshes(self, d):
        atlas = G.Mesh()
        build_head(atlas, d)
        build_shaft(atlas, d)
        build_amber(atlas, d)
        for r in self.roots:
            build_root(atlas, r, self.pieces, d)
        gold = G.Mesh()
        for st in gold_strokes_for_lod(self.strokes, d):
            bead_mesh(gold, st, d)
        return atlas.finish(), gold.finish()


def merge(a, b):
    n = len(a["P"])
    return {"P": np.vstack([a["P"], b["P"]]), "T": np.vstack([a["T"], b["T"] + n]),
            "UV": np.concatenate([a["UV"], b["UV"]]), "N": np.vstack([a["N"], b["N"]]),
            "TAG": np.concatenate([a["TAG"], b["TAG"]])}


# ================================================================ p3d
def visual_lod(m, resolution):
    lod = py3d.LOD()
    lod.resolution = resolution
    lod.properties["lodnoshadow"] = "1"                  # как у ванильной кирки: визуальные LOD без тени
    P, T, UV, N, TAG = m["P"], m["T"], m["UV"], m["N"], m["TAG"]
    for c in P:
        p = py3d.Point()
        p.coords = (float(c[0]), float(c[1]), float(c[2]))
        p.flags = 0
        lod.points.append(p)
    for n in N:
        lod.facenormals.append((float(n[0]), float(n[1]), float(n[2])))
    fn = normalize(np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]]))
    glow = lod.new_selection(GLOW_SEL)
    for k in range(len(T)):
        tag = int(TAG[k])
        f = py3d.Face(lod.points, lod.facenormals)
        f.texture = TEX_GOLD if tag == TAG_GOLD else TEX_CO
        f.material = MATS[TAG_MAT[tag]]
        flat_idx = None
        for c in range(3):
            idx = int(T[k, c])
            v = py3d.Vertex(lod.points, lod.facenormals)
            v.point_index = idx
            if float(np.dot(N[idx], fn[k])) >= 0.35:
                v.normal_index = idx
            else:                                   # острый угол: своя плоская нормаль у угла грани
                if flat_idx is None:
                    flat_idx = len(lod.facenormals)
                    lod.facenormals.append((float(fn[k, 0]), float(fn[k, 1]), float(fn[k, 2])))
                v.normal_index = flat_idx
            v.uv = (float(UV[k, c, 0]), float(UV[k, c, 1]))
            f.vertices.append(v)
            if tag == TAG_GOLD:
                glow.points[lod.points[idx]] = 1
        lod.faces.append(f)
        if tag == TAG_GOLD:
            glow.faces[f] = 1
    return lod


def geo_components():
    """Выпуклые куски Geometry/View/Fire (оси Blender): рукоять, обух, клюв, долото."""
    handle = G.box_polys((-0.025, -0.030, -0.248), (0.025, 0.030, COL_ZB))    # с корнями и янтарём
    collar = G.box_polys((-0.0325, -0.045, COL_ZB - 0.001), (0.0325, 0.045, COL_ZT + 0.001))
    spike = G.box_polys((-0.0195, 0.045, 0.648), (0.0195, H_TIP + 0.001, 0.695))
    spike = G.clip_polys(spike, (0, 0.141, 1.0), 0.70134)
    spike = G.clip_polys(spike, (1.0, 0.0749, 0), 0.02287)
    spike = G.clip_polys(spike, (-1.0, 0.0749, 0), 0.02287)
    chisel = G.box_polys((-0.035, H_EDGE - 0.005, 0.640), (0.035, -0.045, 0.695))
    chisel = G.clip_polys(chisel, (0, -0.2086, 1.0), 0.70439)
    chisel = G.clip_polys(chisel, (1.0, 0.0743, 0), 0.016156)
    chisel = G.clip_polys(chisel, (-1.0, 0.0743, 0), 0.016156)
    comps = [G.snap_polys(c, 1e-6) for c in (handle, collar, spike, chisel)]
    return comps, ["wood", "iron", "iron", "iron"], [0.95, 0.80, 0.35, 0.40]


def comps_lod(resolution, comps, pens, masses=None):
    lod = py3d.LOD()
    lod.resolution = resolution
    for ci, polys in enumerate(comps):
        P, T = G.polys_to_tris(polys)
        base = len(lod.points)
        fbase = len(lod.faces)
        for c in P:
            p = py3d.Point()
            p.coords = (float(c[0]), float(c[1]), float(c[2]))
            p.flags = 0
            p.mass = (masses[ci] / len(P)) if masses else None
            lod.points.append(p)
        fn = normalize(np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]]))
        cen = P.mean(0)
        for k in range(len(T)):
            tri = T[k]
            n = fn[k]
            if np.dot(P[tri].mean(0) - cen, n) < 0:
                tri = tri[[0, 2, 1]]
                n = -n
            lod.facenormals.append((float(n[0]), float(n[1]), float(n[2])))
            f = py3d.Face(lod.points, lod.facenormals)
            f.material = PEN[pens[ci]]
            f.texture = ""
            for idx in tri:
                v = py3d.Vertex(lod.points, lod.facenormals)
                v.point_index = base + int(idx)
                v.normal_index = len(lod.facenormals) - 1
                v.uv = (0.0, 0.0)
                f.vertices.append(v)
            lod.faces.append(f)
        lod.set_selection("Component%02d" % (ci + 1), face_idx=range(fbase, len(lod.faces)),
                          point_idx=range(base, len(lod.points)))
    if masses:
        lod.properties["autocenter"] = "0"
    return lod


# точки памяти ванильной кирки (оси DayZ, замер ODOL); границы и ce_* пересчитываются по своей модели
MEM_VANILLA = {
    "invview": (1.0123, 0.2280, 0.0124),
    "meleerangestart": (0.0, 0.0001, 0.0),
    "meleerangeend": (0.0, 0.8331, 0.3249),
    "throwingimpulseposition": (0.0, 0.1109, 0.0),
}
LIGHT_POINT = (COL_W + 0.010, RUNE_Z, 0.0)          # vbmr_light в осях DayZ: 1 см от щеки +X, центр руны


def memory_points(P0):
    """Точки памяти в осях DayZ. P0 — точки LOD0 в осях Blender."""
    D = P0[:, [0, 2, 1]]
    lo, hi = D.min(0), D.max(0)
    mem = dict(MEM_VANILLA)
    # как у ванили: в boundingbox_min максимум по Z, в boundingbox_max минимум (так у оригинала)
    mem["boundingbox_min"] = (float(lo[0]), float(lo[1]), float(hi[2]))
    mem["boundingbox_max"] = (float(hi[0]), float(hi[1]), float(lo[2]))
    c = (lo + hi) / 2
    mem["ce_center"] = tuple(float(v) for v in c)
    van_off = np.array([0.0514, 0.7215, 0.2873]) - np.array([0.0013, 0.2282, 0.0114])
    rad = 0.5 * float(np.linalg.norm(hi - lo)) * 1.05
    mem["ce_radius"] = tuple(float(v) for v in c + normalize(van_off) * rad)
    mem["vbmr_light"] = LIGHT_POINT
    return mem


def write_p3d(model, out_path):
    meshes = {}
    for d in (0, 1, 2):
        a, g = model.lod_meshes(d)
        meshes[d] = (a, g, merge(a, g))
    p3 = py3d.P3D()
    stats = []
    for res, d in ((1.0, 0), (2.0, 0), (4.0, 1), (8.0, 2), (1100.0, 0)):
        p3.lods.append(visual_lod(meshes[d][2], res))
        stats.append((res, len(meshes[d][2]["T"]), len(meshes[d][1]["T"])))
    comps, pens, masses = geo_components()
    p3.lods.append(comps_lod(1e13, comps, pens, masses))
    mem = py3d.LOD()
    mem.resolution = 1e15
    for name, c in memory_points(meshes[0][2]["P"]).items():
        mem.set_memory_point(name, (c[0], c[2], c[1]))      # в оси Blender, ниже общий перевод
    p3.lods.append(mem)
    p3.lods.append(comps_lod(6e15, comps, pens))
    p3.lods.append(comps_lod(7e15, comps, pens))
    py3d.blender_to_dayz(p3)
    p3.save(out_path, verify=True)
    findings = [f for f in p3.validate() if getattr(f, "severity", "") in ("ERROR", "WARN")]
    return meshes, stats, findings, memory_points(meshes[0][2]["P"])


# ================================================================ поля текстур
class SegIndex:
    """Расстояние от точек до набора отрезков-«капсул» (полуширина линейно по отрезку), ячейки в 3D."""

    def __init__(self, A, B, RA, RB, reach):
        self.A, self.B = np.asarray(A, np.float64), np.asarray(B, np.float64)
        self.RA, self.RB = np.asarray(RA, np.float64), np.asarray(RB, np.float64)
        L = np.linalg.norm(self.B - self.A, axis=1)
        self.reach = reach
        self.c = reach + float(max(self.RA.max(), self.RB.max())) + float(L.max()) * 0.5 + 1e-4
        key = np.floor((self.A + self.B) / 2 / self.c).astype(np.int64)
        self.cells = {}
        for i, k in enumerate(map(tuple, key)):
            self.cells.setdefault(k, []).append(i)

    def query(self, P):
        P = np.asarray(P, np.float64)
        out = np.full(len(P), self.reach)
        if len(P) == 0:
            return out
        kp = np.floor(P / self.c).astype(np.int64)
        uniq, inv = np.unique(kp, axis=0, return_inverse=True)
        inv = inv.reshape(-1)
        order = np.argsort(inv, kind="stable")
        starts = np.searchsorted(inv[order], np.arange(len(uniq)))
        ends = np.searchsorted(inv[order], np.arange(len(uniq)), side="right")
        pcell = {tuple(u): order[s:e] for u, s, e in zip(uniq, starts, ends)}
        for k, segs in self.cells.items():
            ids = [pcell.get((k[0] + dx, k[1] + dy, k[2] + dz)) for dx in (-1, 0, 1) for dy in (-1, 0, 1)
                   for dz in (-1, 0, 1)]
            ids = [i for i in ids if i is not None]
            if not ids:
                continue
            ids = np.concatenate(ids)
            q = P[ids]
            s = np.array(segs)
            A, B = self.A[s], self.B[s]
            AB = B - A
            l2 = np.maximum((AB * AB).sum(1), 1e-18)
            t = np.clip(((q[:, None, :] - A[None]) * AB[None]).sum(2) / l2[None], 0, 1)
            cp = A[None] + t[..., None] * AB[None]
            dist = np.linalg.norm(q[:, None, :] - cp, axis=2) - (self.RA[s][None] + (self.RB[s] - self.RA[s])[None] * t)
            out[ids] = np.minimum(out[ids], dist.min(1))
        return out


def stroke_index(strokes, kind, reach):
    A, B, RA, RB = [], [], [], []
    for st in strokes:
        if st["kind"] != kind:
            continue
        P, hw = st["P"], st["HW"]
        A.append(P[:-1])
        B.append(P[1:])
        RA.append(hw[:-1])
        RB.append(hw[1:])
    return SegIndex(np.vstack(A), np.vstack(B), np.concatenate(RA), np.concatenate(RB), reach)


def root_index(roots, reach):
    A, B, RA, RB = [], [], [], []
    for r in roots:
        P, R = resample(r["P"], r["R"], np.arange(0, r["L"], 0.004))
        A.append(P[:-1])
        B.append(P[1:])
        RA.append(R[:-1])
        RB.append(R[1:])
    return SegIndex(np.vstack(A), np.vstack(B), np.concatenate(RA), np.concatenate(RB), reach)


class Idx:
    def __init__(self, model):
        self.vein = stroke_index(model.strokes, "vein", 0.006)
        self.rune = stroke_index(model.strokes, "rune", 0.004)
        self.root = root_index(model.roots, 0.008)


INCLUSIONS = None
CLAW_ALPHAS = []
INCL_VIEW = math.radians(-30.0)     # куда смотрит снимок янтаря (азимут от +X): мошку видно между когтями


def _inclusions():
    """Вкрапления в янтаре (центр, радиус) в осях Blender: мошка (тело, голова, лапки, усики) в просвете лапы,
    ближайшем к INCL_VIEW, семечко — в противоположном просвете."""
    rng = np.random.default_rng(6060)
    al = np.array(CLAW_ALPHAS)

    def clearance(g):
        return float(np.min(np.abs((al - g + math.pi) % (2 * math.pi) - math.pi)))
    cand = INCL_VIEW + np.radians(np.arange(-60, 61, 3))
    score = [clearance(g) - 0.25 * abs(g - INCL_VIEW) for g in cand]
    g0 = float(cand[int(np.argmax(score))])
    cand1 = g0 + math.pi + np.radians(np.arange(-60, 61, 3))
    g1 = float(cand1[int(np.argmax([clearance(g) for g in cand1]))])
    log("  вкрапление: азимут %.0f° (просвет %.0f°), семечко %.0f°" % (
        math.degrees(g0), math.degrees(clearance(g0)), math.degrees(g1)))
    blobs = []
    rad = np.array([math.cos(g0), math.sin(g0), 0.0])
    c0 = AMB_C + rad * 0.0195 + np.array([0, 0, -0.0035])
    ax = normalize(np.array([0.0, 0.0, 1.0]) + 0.35 * np.cross(rad, [0, 0, 1.0]))
    side = normalize(np.cross(ax, rad))
    for t, r in ((-0.0052, 0.0027), (-0.0020, 0.0022), (0.0012, 0.0016), (0.0035, 0.0012)):
        blobs.append((c0 + ax * t, r))
    for k in range(3):                                  # лапки
        base = c0 + ax * (-0.0014 + 0.0012 * k)
        for sg in (-1, 1):
            for j in range(1, 5):
                p = base + side * sg * 0.0012 * j + ax * (0.00045 * j * (k - 1)) + rng.normal(0, 0.0001, 3)
                blobs.append((p, 0.00045))
    for sg in (-1, 1):                                  # усики
        for j in range(1, 4):
            blobs.append((c0 + ax * (0.0038 + 0.0009 * j) + side * sg * 0.0007 * j, 0.00035))
    rad1 = np.array([math.cos(g1), math.sin(g1), 0.0])
    c1 = AMB_C + rad1 * 0.0165 + np.array([0, 0, 0.004])                    # семечко
    blobs.append((c1, 0.0021))
    blobs.append((c1 + np.array([0, 0, -0.0018]), 0.0015))
    return blobs


def tex_fields(ctx, idx, need_color=True):
    """Поля рисунка всех частей атласа: h — высота (м) для _nohq, остальное — для цвета и блеска."""
    global INCLUSIONS
    n = ctx.n
    p = ctx.pos
    tag = ctx.tag
    h = np.zeros(n)
    F = {"h": h}
    # ---------------- навершие
    m = np.nonzero(tag == TAG_HEAD)[0]
    if len(m):
        q = p[m]
        w, zc, hz, _ = head_params(q[:, 1])
        cx = np.abs(q[:, 0]) / w
        cz = np.abs(q[:, 2] - zc) / hz
        corner = sstep(0.80, 0.975, np.minimum(cx, cz))
        tip = np.maximum(sstep(0.232, 0.266, q[:, 1]), sstep(-0.212, -0.246, q[:, 1]))
        warp = np.column_stack([gnoise(q * 160, 701), gnoise(q * 160 + 5.5, 702), gnoise(q * 160 + 9.1, 703)])
        f1, f2, cid = worley(q / 0.0075 + 0.45 * warp, seed=704)            # следы молота: крупные лунки
        g1, _, _ = worley(q / 0.0031 + 0.6 * warp, seed=711)                # и мелкие
        amp = 0.6 + 0.4 * sstep(-0.4, 0.6, fbm(q * 35, 2, seed=712))       # где били сильнее
        dent = (0.65 * f1 ** 2 + 0.35 * g1 ** 2) * amp
        pit = sstep(0.55, 0.85, fbm(q * 1400, 2, seed=705))
        dv = idx.vein.query(q)
        dr = idx.rune.query(q)
        groove = sstep(0.0010, 0.00030, dr)
        crack = sstep(0.00055, 0.0, dv)
        hh = 0.00013 * (dent - 0.25) * (1 - 0.75 * np.maximum(corner, tip)) - 0.00003 * pit \
            - 0.00045 * groove - 0.00020 * crack + 0.00003 * fbm(q * 500, 2, seed=706) \
            + 0.00010 * fbm(q * 90, 2, seed=713)
        h[m] = hh
        F["head"] = dict(m=m, corner=corner, tip=tip, dent=dent, cid=cid, dv=dv, dr=dr, groove=groove,
                         crack=crack, pit=pit)
        if need_color:
            F["head"].update(big=fbm(q * 40, 3, seed=707), mid=fbm(q * 150, 3, seed=708),
                             scale=fbm(q * 65 + 3.0, 4, seed=709), droot=idx.root.query(q))
    # ---------------- рукоять
    m = np.nonzero(tag == TAG_SHAFT)[0]
    if len(m):
        q = p[m]
        qa = q * np.array([1.0, 1.0, 0.07])
        rho = np.sqrt((q[:, 0] - 0.055) ** 2 + (q[:, 1] + 0.031) ** 2)          # кольца дерева (центр ствола в стороне)
        wob = fbm(qa * 120, 3, seed=721)
        rings = np.sin(2 * math.pi * (rho / 0.0021 + 1.6 * wob))
        fine = fbm(q * np.array([900, 900, 60]), 2, seed=722)
        h[m] = 0.000045 * rings + 0.00003 * fine
        F["shaft"] = dict(m=m, rings=rings, fine=fine)
        if need_color:
            F["shaft"].update(streak=fbm(qa * 260, 3, seed=723), big=fbm(q * 25, 2, seed=724),
                              droot=idx.root.query(q))
    # ---------------- корни (кора)
    m = np.nonzero(tag == TAG_ROOT)[0]
    if len(m):
        q = p[m]
        tv = normalize(np.column_stack([ctx.attr["tx"][m], ctx.attr["ty"][m], ctx.attr["tz"][m]]))
        along = (q * tv).sum(1)
        qa = q + (0.10 - 1) * along[:, None] * tv                   # вытянуто вдоль корня в 10 раз
        ql = q + (0.05 - 1) * along[:, None] * tv                   # и в 20 раз — для продольных трещин
        warp = np.column_stack([gnoise(qa * 160, 731), gnoise(qa * 160 + 4.4, 732), gnoise(qa * 160 + 8.8, 733)])
        n1 = gnoise(ql * 380 + 0.6 * warp, 734)
        fiss = sstep(0.11, 0.0, np.abs(n1)) * sstep(-0.35, 0.25, fbm(ql * 110, 2, seed=740))
        n2 = gnoise(ql * 950 + 0.4 * warp, 735)
        fiss = np.maximum(fiss, 0.55 * sstep(0.07, 0.0, np.abs(n2)))
        f1, f2, cid = worley(qa / 0.0045 + 0.35 * warp, seed=741)    # крупные пластины — оттенок
        wrink = np.sin(along / 0.0019 * 2 * math.pi + 2.5 * fbm(qa * 90, 2, seed=742))
        wrink *= sstep(0.2, 0.7, fbm(qa * 55, 2, seed=743))           # поперечные морщины местами
        ridge = fbm(ql * 600, 2, seed=744)
        plate = sstep(0.0, 0.5, f2 - f1)
        h[m] = -0.00060 * fiss + 0.00008 * plate + 0.00006 * ridge + 0.00004 * (cid - 0.5) + 0.00007 * wrink
        F["root"] = dict(m=m, fiss=fiss, cid=cid, ridge=ridge, plate=plate)
        if need_color:
            nr = ctx.nrm[m]
            F["root"].update(moss_n=fbm(q * 48, 4, seed=736), moss_f=fbm(q * 650, 2, seed=737),
                             gv=fbm(qa * 55, 3, seed=738), big=fbm(qa * 40, 3, seed=739), nz=nr[:, 2],
                             rad=nr, z=q[:, 2])
    # ---------------- янтарь
    m = np.nonzero(tag == TAG_AMBER)[0]
    if len(m):
        q = p[m]
        f1, f2, _ = worley(q / 0.006, seed=741)
        craze = sstep(0.035, 0.0, f2 - f1) * sstep(0.2, 0.6, fbm(q * 90, 2, seed=742))
        h[m] = -0.00006 * craze + 0.000012 * fbm(q * 600, 2, seed=743)
        F["amber"] = dict(m=m, craze=craze)
        if need_color:
            if INCLUSIONS is None:
                INCLUSIONS = _inclusions()
            nr = ctx.nrm[m]
            dark = np.zeros(len(m))
            for c, r in INCLUSIONS:
                v = c - q
                dep = -(v * nr).sum(1)                       # глубина под поверхностью вдоль -нормали
                perp = np.linalg.norm(v + dep[:, None] * nr, axis=1)
                soft = r + 0.22 * np.maximum(dep, 0)
                dark = np.maximum(dark, np.exp(-(perp / soft) ** 2) * sstep(0.0, 0.0015, dep)
                                  * np.clip(1.15 - dep / 0.016, 0, 1))
            fb1, _, bid = worley(q / 0.0011, seed=744)
            bub = sstep(0.22, 0.0, fb1) * (bid > 0.93)
            flow = np.sin((q[:, 2] * 0.85 + q[:, 0] * 0.35 + q[:, 1] * 0.2) / 0.0017 + 3.2 * fbm(q * 55, 3, seed=747))
            win = np.zeros(len(m))                               # «окно» чистого янтаря вокруг вкраплений
            for c, r in INCLUSIONS[:1] + INCLUSIONS[-2:-1]:
                v = c - q
                dep = -(v * nr).sum(1)
                perp = np.linalg.norm(v + dep[:, None] * nr, axis=1)
                win = np.maximum(win, np.exp(-(perp / 0.0085) ** 2) * (dep > 0))
            F["amber"].update(win=win, swirl=fbm(q * 70, 4, seed=745), fine=fbm(q * 380, 2, seed=746), dark=dark, bub=bub,
                              rel=normalize(q - AMB_C), nr=nr, flow=flow, droot=idx.root.query(q))
    return F


def tex_color(ctx, F):
    n = ctx.n
    c = np.zeros((n, 3))
    if "head" in F:
        H = F["head"]
        m = H["m"]
        k = len(m)
        t = np.clip(0.55 + 0.30 * H["big"] + 0.15 * H["mid"], 0, 1)
        cc = lerp(col(TI_DARK, k), col(TI_BASE, k), sstep(0.05, 0.55, t))
        cc = lerp(cc, col(TI_LIGHT, k), sstep(0.65, 1.0, t) * 0.5)
        cc *= (0.96 + 0.06 * H["cid"])[:, None]                   # грани от молота чуть разного тона
        cc *= (1.0 - 0.08 * sstep(0.10, 0.45, H["dent"]))[:, None] * (1.04 - 0.08 * H["pit"])[:, None]
        scale = sstep(0.15, 0.75, H["scale"]) * (1 - H["corner"]) * (1 - H["tip"])
        cc = lerp(cc, col(TI_SCALE, k), scale * 0.55)            # окалина
        tt = fbm(ctx.pos[m] * 28, 3, seed=710)                    # побежалость титана
        cc = lerp(cc, cc * np.array([0.85, 0.92, 1.25]), sstep(0.3, 1.0, tt) * 0.30)
        cc = lerp(cc, col(TI_EDGE, k), np.clip(0.70 * H["corner"] + 0.80 * H["tip"], 0, 1))
        # цвета побежалости вокруг залитых золотом трещин (металл грели)
        dv = np.maximum(H["dv"], 0)
        halo = np.exp(-dv / 0.0026) * (H["dv"] < 0.006)
        ramp = np.where(dv[:, None] < 0.0007, np.array(TINT_STRAW),
                        np.where(dv[:, None] < 0.0015, np.array(TINT_BRONZE),
                                 np.where(dv[:, None] < 0.0026, np.array(TINT_PURPLE), np.array(TINT_BLUE))))
        cc = lerp(cc, ramp * (0.75 + 0.25 * cc.mean(1, keepdims=True) / 0.2), halo * 0.42)
        cc = lerp(cc, cc * 0.35, H["crack"])
        # гравировка руны: тёмная канавка, светлый полированный бортик
        rim = sstep(0.0010, 0.0013, H["dr"]) * sstep(0.0019, 0.0013, H["dr"])
        cc = lerp(cc, col(TI_EDGE, k), rim * 0.45)
        cc = lerp(cc, col((0.05, 0.05, 0.06), k), H["groove"] * 0.9)
        ao = 1 - 0.55 * np.exp(-np.maximum(H["droot"], 0) / 0.0030)
        c[m] = cc * ao[:, None]
    if "shaft" in F:
        S = F["shaft"]
        m = S["m"]
        k = len(m)
        z = ctx.pos[m, 2]
        t = np.clip(0.55 + 0.25 * S["streak"] + 0.20 * S["big"], 0, 1)
        cc = lerp(col(WOOD_DARK, k), col(WOOD_BASE, k), sstep(0.15, 0.65, t))
        cc = lerp(cc, col(WOOD_LIGHT, k), sstep(0.7, 1.0, t) * 0.5)
        cc = lerp(cc, cc * 0.62, sstep(0.35, 0.95, S["rings"]) * 0.55)
        cc *= (0.95 + 0.07 * S["fine"])[:, None]
        grip = np.maximum(sstep(-0.21, -0.17, z) * sstep(0.02, -0.02, z), sstep(0.05, 0.10, z) * sstep(0.34, 0.28, z))
        cc = lerp(cc, cc * np.array([1.12, 1.08, 1.02]), grip * 0.4)   # затёрто руками
        ao = 1 - 0.65 * np.exp(-np.maximum(S["droot"], 0) / 0.0028)
        ao *= 1 - 0.35 * sstep(0.60, 0.645, z)                          # под обухом
        c[m] = cc * ao[:, None]
    if "root" in F:
        R = F["root"]
        m = R["m"]
        k = len(m)
        z = R["z"]
        t = np.clip(0.5 + 0.25 * R["big"] + 0.15 * (R["cid"] - 0.5) + 0.12 * R["ridge"] + 0.25 * (R["plate"] - 0.5),
                    0, 1)
        cc = lerp(col(BARK, k), col(BARK_TOP, k), sstep(0.50, 0.95, t))
        cc = lerp(cc, col(BARK, k) * 0.75, sstep(0.40, 0.05, t) * 0.5)
        cc = lerp(cc, col(BARK_FISS, k), R["fiss"] * 0.90)
        # золотые жилки в трещинах коры (редко)
        gold = R["fiss"] * sstep(0.48, 0.72, R["gv"])
        cc = lerp(cc, col(GOLD, k) * (0.85 + 0.2 * R["ridge"])[:, None].clip(0.6, 1.1), gold * 0.85)
        # мох: сверху и в стыках, больше у навершия и у лапы, в местах хвата стёрт
        zone = np.clip(0.30 + sstep(0.40, 0.58, z) + sstep(-0.10, -0.175, z) * 0.9 + sstep(0.60, 0.66, z) * 0.4, 0, 1.2)
        zone *= 1 - 0.65 * (sstep(-0.19, -0.15, z) * sstep(0.02, -0.03, z) + sstep(0.05, 0.10, z) * sstep(0.34, 0.28, z))
        up = np.clip(0.55 + 0.6 * R["nz"], 0, 1)
        moss = sstep(0.42, 0.62, R["moss_n"] + 0.30 * R["fiss"] + 0.40 * (up - 0.5) + 0.25 * (zone - 0.6)) * \
            np.clip(zone, 0, 1) * (0.75 + 0.25 * sstep(-0.3, 0.3, R["moss_f"]))
        moss = np.clip(moss, 0, 1)
        mc = lerp(col(MOSS_DARK, k), col(MOSS, k), sstep(-0.4, 0.4, R["moss_f"]))
        mc = lerp(mc, col(MOSS_LIGHT, k), sstep(0.35, 0.9, R["moss_f"]) * 0.6)
        cc = lerp(cc, mc, moss * 0.92)
        R["moss"] = moss
        R["gold"] = gold
        # тень у рукояти: сторона корня, обращённая к стержню
        q = ctx.pos[m]
        a, b = shaft_ab(q[:, 2])
        rr = np.sqrt((q[:, 0] / a) ** 2 + (q[:, 1] / b) ** 2)
        dist = (rr - 1) * np.sqrt(a * b)
        radial = normalize(np.column_stack([q[:, 0] / a ** 2, q[:, 1] / b ** 2, np.zeros(k)]))
        facing = np.clip(-(R["rad"] * radial).sum(1), 0, 1)
        on_shaft = (q[:, 2] > SH_Z0) & (q[:, 2] < COL_ZB)
        ao = 1 - 0.55 * np.exp(-np.maximum(dist, 0) / 0.0022) * facing * on_shaft
        c[m] = cc * ao[:, None]
    if "amber" in F:
        A = F["amber"]
        m = A["m"]
        k = len(m)
        t = np.clip(0.48 + 0.55 * A["swirl"] + 0.10 * A["fine"], 0, 1)
        cc = lerp(col(AMB_DARK, k), col(AMB_MID, k), sstep(0.10, 0.75, t))
        cc = lerp(cc, col(AMB_LIGHT, k), np.clip(sstep(0.62, 1.0, t) * 0.55 + 0.30 * sstep(0.55, 1.0, A["flow"]) * t, 0, 1))
        cc = lerp(cc, cc * 0.72, sstep(0.3, -0.9, A["flow"]) * 0.35)        # слои смолы
        # «глубина»: снизу и в тени лапы темнее, сверху светлее (свет проходит сквозь)
        cc *= (0.80 + 0.25 * np.clip(A["rel"][:, 2] * 0.5 + 0.5, 0, 1))[:, None]
        cc = lerp(cc, col(AMB_MID, k) * 1.12, A["win"] * 0.65)
        cc = lerp(cc, col(AMB_INCL, k), np.clip(A["dark"] * 1.15, 0, 0.95))
        cc = lerp(cc, col((1.0, 0.88, 0.55), k), A["bub"] * 0.5)
        cc = lerp(cc, cc * 1.18, A["craze"] * 0.4)
        cc *= (1 - 0.45 * np.exp(-np.maximum(A["droot"], 0) / 0.0025))[:, None]   # тень у когтей
        c[m] = cc
    return np.clip(c, 0, 1)


def tex_smdi(ctx, F):
    n = ctx.n
    g = np.zeros(n)
    b = np.zeros(n)
    if "head" in F:
        H = F["head"]
        m = H["m"]
        scale = sstep(0.15, 0.75, H["scale"]) * (1 - H["corner"]) * (1 - H["tip"]) if "scale" in H else 0
        g[m] = 0.62 + 0.30 * H["corner"] + 0.30 * H["tip"] - 0.30 * scale - 0.55 * H["groove"] - 0.4 * H["crack"]
        b[m] = 0.46 + 0.36 * H["corner"] + 0.34 * H["tip"] - 0.28 * scale - 0.40 * H["groove"] \
            - 0.10 * sstep(0.1, 0.5, H["dent"])
    if "shaft" in F:
        S = F["shaft"]
        m = S["m"]
        z = ctx.pos[m, 2]
        grip = np.maximum(sstep(-0.21, -0.17, z) * sstep(0.02, -0.02, z), sstep(0.05, 0.10, z) * sstep(0.34, 0.28, z))
        g[m] = 0.30 + 0.18 * grip - 0.10 * sstep(0.35, 0.95, S["rings"])
        b[m] = 0.34 + 0.20 * grip
    if "root" in F:
        R = F["root"]
        m = R["m"]
        moss = R.get("moss", 0)
        gold = R.get("gold", 0)
        g[m] = 0.16 - 0.10 * R["fiss"] - 0.10 * moss + 0.70 * gold
        b[m] = 0.20 - 0.12 * R["fiss"] - 0.15 * moss + 0.55 * gold
    if "amber" in F:
        A = F["amber"]
        m = A["m"]
        g[m] = 0.88 - 0.25 * A["craze"]
        b[m] = 0.92 - 0.30 * A["craze"]
    return np.column_stack([np.ones(n), np.clip(g, 0, 1), np.clip(b, 0, 1)])


# ---------------------------------------------------------------- золото (своя маленькая текстура)
GOLD_W, GOLD_H = 512, 128


def gold_textures(png_dir, paa):
    rng = np.random.default_rng(4747)
    noise = TL.spectral(GOLD_W, rng, beta=1.6)[:GOLD_H]          # по u периодичен (FFT)
    spots = TL.spectral(GOLD_W, rng, beta=0.8)[:GOLD_H]
    v = (np.arange(GOLD_H) + 0.5) / GOLD_H
    across = (1 - (2 * v - 1) ** 2)[:, None] * np.ones((1, GOLD_W))
    center = np.array([1.0, 0.80, 0.36])
    edge = np.array([0.50, 0.32, 0.07])
    t = np.clip(across ** 0.55 * (0.92 + 0.10 * noise), 0, 1)
    co = edge * (1 - t)[..., None] + center * t[..., None]
    co *= (0.93 + 0.07 * np.tanh(noise))[..., None]
    co = co * (1 - 0.25 * np.clip(spots - 1.2, 0, 1))[..., None] + np.array([1.0, 0.92, 0.62]) * \
        (0.35 * np.clip(spots - 1.6, 0, 1))[..., None]
    e_center = np.array([1.0, 0.87, 0.50])
    e_edge = np.array([0.80, 0.58, 0.20])
    te = np.clip(across ** 0.45 * (0.95 + 0.05 * noise), 0, 1)
    e = e_edge * (1 - te)[..., None] + e_center * te[..., None]
    out = []
    for name, img in (("vb_mr_veles_glow_co", co), ("vb_mr_veles_glow_e_co", e)):
        out.append(save_tex(name, img, png_dir, paa))
    return out


def save_tex(name, img, png_dir, paa=True):
    path = os.path.join(png_dir, name + ".png")
    TL.write_png(path, TL.to_u8(img))
    if paa:
        TL.png_to_paa(path, os.path.join(OUT_DIR, name + ".paa"))
    return path


def make_textures(model, atlas_mesh, png_dir, co_size, aux_size, paa=True):
    t0 = time.time()
    idx = Idx(model)
    log("  индексы расстояний: %.1f с" % (time.time() - t0))
    ctx, mask = G.bake(atlas_mesh, co_size)
    log("  запекание %d: %d текселей, %.1f с" % (co_size, ctx.n, time.time() - t0))
    F = tex_fields(ctx, idx)
    c = tex_color(ctx, F)
    save_tex("vb_mr_veles_co", G.dilate(G.put(mask, c), mask, 12), png_dir, paa)
    log("  _co: %.1f с" % (time.time() - t0))
    if aux_size != co_size:
        ctx, mask = G.bake(atlas_mesh, aux_size)
        F = tex_fields(ctx, idx)
        tex_color(ctx, F)                                  # заполняет маски мха и золота для _smdi
    sm = tex_smdi(ctx, F)
    save_tex("vb_mr_veles_smdi", G.dilate(G.put(mask, sm), mask, 12), png_dir, paa)
    nm = G.normal_map(ctx, lambda cc: tex_fields(cc, idx, need_color=False)["h"])
    save_tex("vb_mr_veles_nohq", G.dilate(G.put(mask, nm), mask, 12), png_dir, paa)
    log("  _smdi, _nohq: %.1f с" % (time.time() - t0))
    gold_textures(png_dir, paa)


# ================================================================ материалы
def rvmat(spec, power, fresnel, nohq, smdi, emissive=(0, 0, 0), e_tex=None, dt=0.5, mc=None, comment="",
          env=True):
    s = "// VB_MiningResources: Велесова кирка, %s (сгенерировано tools\\gen_veles_pickaxe.py)\n" % comment
    s += "ambient[] = {1,1,1,1};\ndiffuse[] = {1,1,1,1};\nforcedDiffuse[] = {0,0,0,0};\n"
    s += "emmisive[] = {%.3f,%.3f,%.3f,1};\n" % tuple(emissive)
    s += "specular[] = {%.3f,%.3f,%.3f,1};\nspecularPower = %d;\n" % (spec[0], spec[1], spec[2], power)
    s += 'PixelShaderID = "Super";\nVertexShaderID = "Super";\n'
    s += ("class TexGen0\n{\n\tuvSource = \"tex\";\n\tclass uvTransform\n\t{\n\t\taside[] = {1,0,0};\n\t\tup[] = {0,1,0};\n"
          "\t\tdir[] = {0,0,1};\n\t\tpos[] = {0,0,0};\n\t};\n};\n")
    for i in range(1, 8):
        s += "class TexGen%d: TexGen0 {};\n" % i
    stages = [(1, nohq), (2, "#(argb,8,8,3)color(%.2f,%.2f,%.2f,1,DT)" % (dt, dt, dt)),
              (3, mc or "#(argb,8,8,3)color(0,0,0,0,MC)"), (4, "#(argb,8,8,3)color(1,1,1,1,AS)"), (5, smdi),
              (6, "#(ai,64,64,1)fresnel(%.2f,%.2f)" % fresnel),
              (7, ENV if env else "#(argb,8,8,3)color(0,0,0,1,CO)")]
    for i, tx in stages:
        s += "class Stage%d\n{\n\ttexture = \"%s\";\n\ttexGen = \"%d\";\n};\n" % (i, tx, i)
    if e_tex:
        s += "class Stage8\n{\n\ttexture = \"%s\";\n\ttexGen = \"0\";\n};\n" % e_tex
    return s


GLOW_ON_EMISSIVE = 9.0               # стенд 2026-11-20: при 2.2 прожилки ночью еле видны (как у стражей Корня, баг №58)
DMG_MC = BS.join(["dz", "characters", "data", "generic_damage_mc.paa"])
DST_MC = BS.join(["dz", "weapons", "data", "weapons_destruct_metal_mc.paa"])


def write_materials():
    nohq = PREFIX + "vb_mr_veles_nohq.paa"
    smdi = PREFIX + "vb_mr_veles_smdi.paa"
    base = {
        "metal": dict(spec=(0.75, 0.80, 0.90), power=55, fresnel=(1.30, 1.12), comment="навершие (титан)"),
        "wood": dict(spec=(0.40, 0.37, 0.32), power=36, fresnel=(1.50, 0.30), comment="рукоять и корни"),
        "amber": dict(spec=(0.80, 0.68, 0.48), power=130, fresnel=(1.55, 0.20), emissive=(0.12, 0.12, 0.12),
                      e_tex="#(argb,8,8,3)color(1,0.55,0.12,1,CO)", comment="янтарь (глянец, тёплый свет изнутри)"),
    }
    files = []
    for name, kw in base.items():
        for suf, k_spec, dt, mc in (("", 1.0, 0.5, None), ("_damage", 0.55, 0.40, DMG_MC), ("_destruct", 0.25, 0.30, DST_MC)):
            kk = dict(kw)
            kk["spec"] = tuple(v * k_spec for v in kw["spec"])
            kk["comment"] = kw["comment"] + ({"": "", "_damage": ", повреждена", "_destruct": ", разрушена"}[suf])
            txt = rvmat(nohq=nohq, smdi=smdi, dt=dt, mc=mc, **kk)
            fn = os.path.join(OUT_DIR, "vb_mr_veles_%s%s.rvmat" % (name, suf))
            with open(fn, "w", encoding="utf-8", newline="\r\n") as f:
                f.write(txt)
            files.append(fn)
    g_nohq = "#(argb,8,8,3)color(0.5,0.5,1,1,NOHQ)"
    g_smdi = "#(argb,8,8,3)color(1,0.95,0.80,1,SMDI)"
    for name, em, et, cm in (("glow", (0, 0, 0), None, "золото прожилок и руны, обычное"),
                             ("glow_on", (GLOW_ON_EMISSIVE,) * 3, PREFIX + "vb_mr_veles_glow_e_co.paa",
                              "золото прожилок и руны, светится («Чует жилу»)")):
        txt = rvmat(spec=(1.55, 1.15, 0.40), power=90, fresnel=(0.64, 2.78), nohq=g_nohq, smdi=g_smdi,
                    emissive=em, e_tex=et, comment=cm)
        fn = os.path.join(OUT_DIR, "vb_mr_veles_%s.rvmat" % name)
        with open(fn, "w", encoding="utf-8", newline="\r\n") as f:
            f.write(txt)
        files.append(fn)
    return files


def write_model_cfg():
    s = "// VB_MiningResources: Велесова кирка, сгенерировано tools\\gen_veles_pickaxe.py.\n"
    s += "// Секция vbmr_glow — золотые прожилки и руна: hiddenSelections, материал меняет скрипт («Чует жилу»).\n"
    s += "class CfgSkeletons\n{\n\tclass Default\n\t{\n\t\tisDiscrete = 1;\n\t\tskeletonInherit = \"\";\n\t\tskeletonBones[] = {};\n\t};\n};\n"
    s += "class CfgModels\n{\n\tclass Default\n\t{\n\t\tsectionsInherit = \"\";\n\t\tsections[] = {};\n\t\tskeletonName = \"\";\n\t};\n"
    s += "\tclass %s: Default\n\t{\n\t\tsections[] = {\"%s\"};\n\t};\n};\n" % (NAME, GLOW_SEL)
    with open(os.path.join(OUT_DIR, "model.cfg"), "w", encoding="utf-8", newline="\r\n") as f:
        f.write(s)


# ================================================================ main
def main():
    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    opt = {a.split("=", 1)[0][2:]: a.split("=", 1)[1] for a in sys.argv[1:] if a.startswith("--") and "=" in a}
    png_dir = args[0] if args else os.path.join(tempfile.gettempdir(), "vb_mr_veles_png")
    os.makedirs(png_dir, exist_ok=True)
    os.makedirs(OUT_DIR, exist_ok=True)
    models = "--tex-only" not in sys.argv
    textures = "--models-only" not in sys.argv
    paa = "--no-paa" not in sys.argv
    co_size = int(opt.get("co", 2048))
    aux_size = int(opt.get("aux", 2048))
    t0 = time.time()
    model = Model()
    if models:
        meshes, stats, findings, mem = write_p3d(model, os.path.join(OUT_DIR, NAME + ".p3d"))
        P = meshes[0][2]["P"][:, [0, 2, 1]]
        lo, hi = P.min(0), P.max(0)
        log("%s.p3d: габарит (оси DayZ) x %.4f..%.4f  y %.4f..%.4f  z %.4f..%.4f" % (
            NAME, lo[0], hi[0], lo[1], hi[1], lo[2], hi[2]))
        for res, ntri, ngold in stats:
            log("   LOD %-7g треугольников %6d (золото %5d)" % (res, ntri, ngold))
        for d in (0, 1, 2):
            tg = meshes[d][2]["TAG"]
            log("   LOD-уровень %d: навершие %d, рукоять %d, корни %d, янтарь %d, золото %d" % (
                (d,) + tuple(int(np.sum(tg == t)) for t in range(5))))
        for k, v in mem.items():
            log("   memory %-24s (%.4f, %.4f, %.4f)" % ((k,) + tuple(v)))
        for f in findings:
            log("   validate:", f)
    else:
        a, g = model.lod_meshes(0)
        meshes = {0: (a, g, merge(a, g))}
    if textures:
        make_textures(model, meshes[0][0], png_dir, co_size, aux_size, paa)
    if "--p3d-only" not in sys.argv:                     # --p3d-only: только модель, rvmat и model.cfg не трогать
        write_materials()
        write_model_cfg()
    log("готово: %s (%.0f с)" % (OUT_DIR, time.time() - t0))


if __name__ == "__main__":
    main()
