# -*- coding: utf-8 -*-
r"""Модели и текстуры 27 новых ресурсов VB_MiningResources (ТЗ, раздел 10.3): 5 форм, у каждого ресурса свой цвет.

Формы (MLOD .p3d, бинаризуются при сборке `pbo.ps1 build ... -Binarize`). У каждой ОДНА скрытая секция `camo`
на всех видимых гранях, текстуру и материал ресурса ставит конфиг (hiddenSelectionsTextures/Materials):
  vb_mr_sand.p3d       — крупицы: горсть из 12 неровных угловатых зёрен разного размера (1x1, 190 г);
  vb_mr_nugget.p3d     — металлический самородок: лепестки, складки, раковины (2x2, 500 г);
  vb_mr_chunk.p3d      — сколотый кусок камня/кристалла: плоские раковистые грани, острые рёбра (2x2, 500 г);
  vb_mr_ingot.p3d      — литой слиток-трапеция со скруглёнными рёбрами, клеймо на верхней грани (3x3, 5,5 кг);
  vb_mr_briquette.p3d  — прессованный брикет с фасками, оттиск пресса сверху, слои прессовки по бокам (2x2, 300 г).
У всех: 3 визуальных LOD (0 и 1 полной детали — правило E79 для мелких предметов, 4 — проще), Geometry с массой
(выпуклая оболочка, autocenter 0), Memory (пустой), View Geometry, Fire Geometry.

Текстуры (data\items):
  vb_mr_<ресурс>_<форма>_co.paa  — на каждый из 27 предметов (1024);
  vb_mr_<форма>_nohq.paa, vb_mr_<форма>_smdi.paa — общие на форму (512); у слитка _nohq свой на металл:
  vb_mr_<ресурс>_ingot_nohq.paa (в клейме знак элемента: Au, Ag, Cu, Fe, Ti);
  vb_mr_<ресурс>_<форма>.rvmat   — материал (Super): металлы с блеском и отражением, кристаллы глянец, остальное матовое.
Ещё model.cfg (sections[] = {"camo"}) и mapping.json: класс -> модель, текстура, материал (пути для конфига;
у .rvmat ведущий «\» — правило E75 для бинаризованных моделей).

Текстуры «запекаются»: каждый тексель атласа растеризацией треугольников получает точку и нормаль на модели,
рисунок считается 3D-шумом в этой точке (без швов на стыках кусков развёртки); _nohq — из производных высоты
вдоль касательных развёртки (DirectX, как у ванили: G вниз по картинке).

Запуск (Python из Blender, в нём есть numpy):
  "G:\SteamLibrary\steamapps\common\Blender\5.2\python\bin\python.exe" -I gen_mr_models.py [<папка_для_png>]
      [--only=sand,ingot] [--models-only] [--tex-only] [--no-paa]
  <папка_для_png> — промежуточные PNG (по умолчанию %TEMP%\vb_mr_png).
Проверка картинками: tools\render_mr_models.py (Blender), см. его заголовок.
"""
import json
import math
import os
import sys
import tempfile
import time

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
MOD_ROOT = os.path.dirname(HERE)
SRC = os.path.join(MOD_ROOT, "VB_MiningResources")
OUT_DIR = os.path.join(SRC, "data", "items")
REPO = os.path.normpath(os.path.join(MOD_ROOT, "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "tools", "py3d"))
import py3d  # noqa: E402
sys.path.insert(0, os.path.join(REPO, "modding", "mod_vb_AncientRoot", "tools"))
import ar_texlib as TL  # noqa: E402  PNG и PAA (ImageToPAA) из генераторов «Древнего корня»

BS = chr(92)
PREFIX = "VB_MiningResources" + BS + "data" + BS + "items" + BS
CO_SIZE = 1024       # _co на каждый предмет
AUX_SIZE = 512       # _nohq и _smdi
PEN = {"cloth": BS.join(["dz", "data", "data", "penetration", "cloth.rvmat"]),
       "iron": BS.join(["dz", "data", "data", "penetration", "iron.rvmat"]),
       "granite": BS.join(["dz", "data", "data", "penetration", "granite.rvmat"])}
ENV = BS.join(["dz", "data", "data", "env_land_co.paa"])

METALS = ["gold", "silver", "bronze", "iron", "titanium"]
STONES = ["coal", "rubin", "nephrite", "sulfur"]
CLASS_RES = {"gold": "Gold", "silver": "Silver", "bronze": "Bronze", "iron": "Iron", "titanium": "Titanium",
             "coal": "Coal", "rubin": "Rubin", "nephrite": "Nephrite", "sulfur": "Sulfur"}
# форма -> ресурсы, шаблон класса, масса (кг) для Geometry, материал проникновения
SHAPES = {
    "sand": {"res": METALS + STONES, "cls": "VB_MR_%s_Sand", "mass": 0.19, "pen": "granite"},
    "nugget": {"res": METALS, "cls": "VB_MR_%s", "mass": 0.5, "pen": "iron"},
    "chunk": {"res": STONES, "cls": "VB_MR_%s", "mass": 0.5, "pen": "granite"},
    "ingot": {"res": METALS, "cls": "VB_MR_%s_Bar", "mass": 5.5, "pen": "iron"},
    "briquette": {"res": STONES, "cls": "VB_MR_%s_Bar", "mass": 0.3, "pen": "granite"},
}
# Формы, которые держат в позе компаса (на открытой ладони, VB_MR_Entities.c, ModItemRegisterCallbacks).
# Точка хвата этой позы (кость RightHand_Dummy) стоит у запястья: с началом координат в центре модель уходила в кисть
# и в руке её не было видно (2026-11-18). Числа — замер для материалов аномалий (mod_vb_AnomaliesBosses,
# tools/ab_materials.py): низ модели на HAND_LIFT над точкой хвата, сдвиг к центру ладони в осях движка (в Blender —
# со знаком минус, оси X и Z модели в игре противоположны p3d).
# Для маленькой горсти (3 см) сдвиг меньше, чем у материалов (там 4 см к костяшкам + 2 см к мизинцу): на стенде
# горсть с тем сдвигом лежала на основаниях пальцев. Здесь 1,5 см вдоль пальцев k = (0.73, 0.69) и 1 см к мизинцу
# n = (-0.69, 0.73) — середина ладони; низ на 4,5 см над точкой хвата.
PALM_SHAPES = {"sand"}
# плоские формы клеймом к камере превью инвентаря (write_p3d): (наклон вокруг длинной оси, градусы; подъём, м)
PREVIEW_TILT = {"ingot": (55, 0.08), "briquette": (55, 0.0)}
HAND_LIFT = 0.045
HAND_SHIFT_ENGINE = (0.0041, 0.0177)
DEFAULT_RES = {"sand": "gold", "nugget": "gold", "chunk": "rubin", "ingot": "gold", "briquette": "coal"}

# палитра ресурсов (sRGB 0..1): base — основной, hi — блик/выпуклость, lo — тень, crev — грязь в углублениях
PAL = {
    "gold": dict(base=(0.86, 0.63, 0.17), hi=(1.00, 0.85, 0.40), lo=(0.55, 0.36, 0.06), crev=(0.24, 0.16, 0.06)),
    "silver": dict(base=(0.75, 0.76, 0.78), hi=(0.95, 0.96, 0.97), lo=(0.47, 0.47, 0.49), crev=(0.17, 0.16, 0.15)),
    "bronze": dict(base=(0.74, 0.37, 0.20), hi=(0.96, 0.60, 0.40), lo=(0.45, 0.18, 0.08), crev=(0.20, 0.10, 0.06),
                   patina=(0.30, 0.52, 0.42)),
    "iron": dict(base=(0.34, 0.34, 0.35), hi=(0.56, 0.56, 0.58), lo=(0.18, 0.18, 0.19), crev=(0.13, 0.08, 0.05),
                 rust=(0.50, 0.23, 0.09), rust2=(0.68, 0.38, 0.15)),
    "titanium": dict(base=(0.38, 0.43, 0.52), hi=(0.58, 0.64, 0.74), lo=(0.22, 0.25, 0.32), crev=(0.13, 0.14, 0.18),
                     tint=(0.34, 0.36, 0.66), straw=(0.66, 0.58, 0.40)),
    "coal": dict(base=(0.060, 0.060, 0.064), hi=(0.17, 0.17, 0.18), lo=(0.026, 0.026, 0.028), crev=(0.015, 0.015, 0.015),
                 brown=(0.10, 0.08, 0.06)),
    "rubin": dict(base=(0.50, 0.03, 0.08), hi=(0.90, 0.17, 0.25), lo=(0.22, 0.01, 0.04), crev=(0.12, 0.01, 0.03),
                  pink=(0.93, 0.55, 0.60)),
    "nephrite": dict(base=(0.17, 0.40, 0.21), hi=(0.36, 0.60, 0.36), lo=(0.06, 0.20, 0.09), crev=(0.04, 0.11, 0.06),
                     cream=(0.75, 0.77, 0.59)),
    "sulfur": dict(base=(0.94, 0.79, 0.10), hi=(1.00, 0.94, 0.40), lo=(0.72, 0.53, 0.04), crev=(0.46, 0.33, 0.05),
                   stain=(0.66, 0.42, 0.10)),
}

# зёрна песка: (цвет, доля, блёстка 0..1)
SAND_MIX = {
    "gold": [((1.00, 0.82, 0.30), 0.38, 1.0), ((0.92, 0.66, 0.15), 0.36, 0.9), ((0.66, 0.44, 0.08), 0.16, 0.6),
             ((0.20, 0.16, 0.12), 0.06, 0.2), ((0.80, 0.74, 0.62), 0.04, 0.3)],
    "silver": [((0.92, 0.93, 0.95), 0.32, 1.0), ((0.72, 0.73, 0.76), 0.34, 0.8), ((0.50, 0.50, 0.53), 0.18, 0.5),
               ((0.20, 0.20, 0.21), 0.10, 0.2), ((0.84, 0.80, 0.70), 0.06, 0.3)],
    "bronze": [((0.92, 0.50, 0.28), 0.32, 0.9), ((0.74, 0.34, 0.16), 0.34, 0.8), ((0.48, 0.20, 0.09), 0.18, 0.4),
               ((0.28, 0.48, 0.38), 0.06, 0.2), ((0.18, 0.13, 0.11), 0.10, 0.2)],
    "iron": [((0.24, 0.24, 0.25), 0.34, 0.6), ((0.13, 0.13, 0.14), 0.30, 0.5), ((0.42, 0.42, 0.44), 0.12, 0.7),
             ((0.55, 0.26, 0.10), 0.16, 0.1), ((0.70, 0.40, 0.16), 0.08, 0.1)],
    "titanium": [((0.55, 0.60, 0.70), 0.34, 0.8), ((0.40, 0.44, 0.53), 0.32, 0.6), ((0.70, 0.74, 0.82), 0.14, 0.9),
                 ((0.22, 0.24, 0.29), 0.14, 0.3), ((0.46, 0.44, 0.66), 0.06, 0.5)],
    "coal": [((0.05, 0.05, 0.055), 0.40, 0.4), ((0.09, 0.09, 0.095), 0.30, 0.6), ((0.025, 0.025, 0.028), 0.20, 0.2),
             ((0.16, 0.15, 0.15), 0.10, 0.5)],
    "rubin": [((0.62, 0.04, 0.10), 0.34, 0.9), ((0.40, 0.02, 0.06), 0.30, 0.7), ((0.90, 0.20, 0.28), 0.16, 1.0),
              ((0.22, 0.01, 0.04), 0.14, 0.3), ((0.95, 0.60, 0.64), 0.06, 1.0)],
    "nephrite": [((0.30, 0.55, 0.32), 0.34, 0.5), ((0.18, 0.40, 0.22), 0.30, 0.4), ((0.50, 0.72, 0.48), 0.16, 0.6),
                 ((0.10, 0.24, 0.13), 0.12, 0.2), ((0.76, 0.78, 0.60), 0.08, 0.3)],
    "sulfur": [((0.98, 0.88, 0.25), 0.40, 0.2), ((0.90, 0.75, 0.12), 0.30, 0.2), ((1.00, 0.96, 0.55), 0.18, 0.3),
               ((0.72, 0.52, 0.12), 0.12, 0.1)],
}

# материалы: блик (r,g,b), specularPower, fresnel(n,k), рассеянный (ambient=diffuse)
MAT = {
    "gold": ((1.55, 1.15, 0.40), 90, (0.64, 2.78), 1.0),
    "silver": ((1.30, 1.30, 1.36), 100, (0.64, 2.78), 1.0),
    "bronze": ((1.55, 0.88, 0.52), 90, (0.64, 2.78), 1.0),
    "iron": ((0.60, 0.60, 0.62), 45, (1.30, 1.12), 1.0),
    "titanium": ((0.70, 0.76, 0.88), 32, (1.30, 1.12), 1.0),
    "coal": ((0.22, 0.22, 0.23), 30, (1.40, 0.40), 1.0),
    "rubin": ((0.85, 0.62, 0.66), 160, (1.77, 0.30), 1.0),
    "nephrite": ((0.40, 0.46, 0.40), 50, (1.60, 0.20), 1.0),
    "sulfur": ((0.12, 0.12, 0.10), 12, (1.30, 0.20), 1.0),
}
# крупицы: блеск чуть слабее самородка (мелкие, с грязью), камни — как кусок
SAND_MAT_K = {"gold": 0.85, "silver": 0.85, "bronze": 0.80, "iron": 0.70, "titanium": 0.75,
              "coal": 0.60, "rubin": 0.75, "nephrite": 0.55, "sulfur": 0.8}


def log(*a):
    print(*a, flush=True)


# ================================================================ шум
GRAD12 = np.array([[1, 1, 0], [-1, 1, 0], [1, -1, 0], [-1, -1, 0], [1, 0, 1], [-1, 0, 1], [1, 0, -1], [-1, 0, -1],
                   [0, 1, 1], [0, -1, 1], [0, 1, -1], [0, -1, -1]], np.float64)
U64 = np.uint64


def _hash(ix, iy, iz, seed):
    """Хеш целочисленной решётки -> uint64."""
    h = (ix.astype(np.int64).astype(np.uint64) * U64(0x8DA6B343)) \
        ^ (iy.astype(np.int64).astype(np.uint64) * U64(0xD8163841)) \
        ^ (iz.astype(np.int64).astype(np.uint64) * U64(0xCB1AB31F)) \
        ^ U64((seed * 0x9E3779B1 + 0x7F4A7C15) & 0xFFFFFFFFFFFF)
    h ^= h >> U64(29)
    h *= U64(0xBF58476D1CE4E5B9)
    h ^= h >> U64(32)
    h *= U64(0x94D049BB133111EB)
    h ^= h >> U64(29)
    return h


def _u01(h):
    return (h >> U64(40)).astype(np.float64) / float(1 << 24)


def hash1(i, seed=0):
    """Случайное 0..1 для целых i (номер нити, зерна и т. п.)."""
    i = np.asarray(i, np.int64)
    return _u01(_hash(i, i * 0 + 7, i * 0 + 13, seed))


def gnoise(p, seed=0):
    """Градиентный шум 3D (примерно -1..1), p (n,3)."""
    p = np.asarray(p, np.float64)
    fl = np.floor(p)
    f = p - fl
    i0 = fl.astype(np.int64)
    w = f * f * f * (f * (f * 6 - 15) + 10)
    out = np.zeros(len(p))
    for c in range(8):
        dx, dy, dz = c & 1, (c >> 1) & 1, (c >> 2) & 1
        h = _hash(i0[:, 0] + dx, i0[:, 1] + dy, i0[:, 2] + dz, seed)
        g = GRAD12[((h >> U64(40)) % U64(12)).astype(np.int64)]
        val = g[:, 0] * (f[:, 0] - dx) + g[:, 1] * (f[:, 1] - dy) + g[:, 2] * (f[:, 2] - dz)
        wx = w[:, 0] if dx else 1 - w[:, 0]
        wy = w[:, 1] if dy else 1 - w[:, 1]
        wz = w[:, 2] if dz else 1 - w[:, 2]
        out += val * wx * wy * wz
    return out


def fbm(p, octaves=4, seed=0, lac=2.03, gain=0.5):
    p = np.asarray(p, np.float64)
    out = np.zeros(len(p))
    amp, tot = 1.0, 0.0
    for o in range(octaves):
        out += amp * gnoise(p * (lac ** o) + o * 17.13, seed + o * 101)
        tot += amp
        amp *= gain
    return out / tot * 1.6


def worley(p, seed=0, jitter=0.9):
    """Клетки Вороного: (F1, F2, случайное 0..1 ближайшей клетки)."""
    p = np.asarray(p, np.float64)
    fl = np.floor(p)
    f = p - fl
    i0 = fl.astype(np.int64)
    b1 = np.full(len(p), 9.0)
    b2 = np.full(len(p), 9.0)
    bid = np.zeros(len(p))
    for dx in (-1, 0, 1):
        for dy in (-1, 0, 1):
            for dz in (-1, 0, 1):
                cx, cy, cz = i0[:, 0] + dx, i0[:, 1] + dy, i0[:, 2] + dz
                h = _hash(cx, cy, cz, seed)
                ox = ((h >> U64(8)) & U64(0xFFFF)).astype(np.float64) / 65535.0
                oy = ((h >> U64(24)) & U64(0xFFFF)).astype(np.float64) / 65535.0
                oz = ((h >> U64(40)) & U64(0xFFFF)).astype(np.float64) / 65535.0
                d = (dx + 0.5 + (ox - 0.5) * jitter - f[:, 0]) ** 2 + (dy + 0.5 + (oy - 0.5) * jitter - f[:, 1]) ** 2 \
                    + (dz + 0.5 + (oz - 0.5) * jitter - f[:, 2]) ** 2
                cid = _u01(_hash(cx, cy, cz, seed + 7))
                m = d < b1
                b2 = np.where(m, b1, np.minimum(b2, d))
                bid = np.where(m, cid, bid)
                b1 = np.where(m, d, b1)
    return np.sqrt(b1), np.sqrt(b2), bid


def sstep(e0, e1, x):
    t = np.clip((np.asarray(x, np.float64) - e0) / (e1 - e0), 0, 1)
    return t * t * (3 - 2 * t)


def lerp(a, b, t):
    a = np.asarray(a, np.float64)
    b = np.asarray(b, np.float64)
    t = np.asarray(t, np.float64)
    if t.ndim == 1:
        t = t[:, None]
    return a * (1 - t) + b * t


def col(c, n):
    return np.tile(np.asarray(c, np.float64), (n, 1))


def normalize(v):
    return v / np.maximum(np.linalg.norm(v, axis=-1, keepdims=True), 1e-12)


# ================================================================ меш
class Mesh:
    """Набор кусков: точки, треугольники (против часовой снаружи, оси Blender), UV на угол, нормали, метка куска,
    атрибуты на точку (для текстур: ao, cav, edge...)."""

    def __init__(self):
        self.P, self.T, self.UV, self.N, self.TAG, self.A = [], [], [], [], [], []
        self.n = 0

    def add(self, P, T, UV, N=None, tag=0, attrs=None):
        P = np.asarray(P, np.float64)
        T = np.asarray(T, np.int64).reshape(-1, 3)
        UV = np.asarray(UV, np.float64).reshape(-1, 3, 2)
        keep = tri_area(P, T) > 1e-14          # без вырожденных граней (полюса тел вращения)
        T, UV = T[keep], UV[keep]
        if N is None:
            N = smooth_normals(P, T)
        self.P.append(P)
        self.T.append(T + self.n)
        self.UV.append(UV)
        self.N.append(np.asarray(N, np.float64))
        self.TAG.append(np.full(len(T), tag, np.int64) if np.isscalar(tag) else np.asarray(tag, np.int64)[keep])
        self.A.append(attrs or {})
        self.n += len(P)

    def finish(self):
        names = sorted({k for a in self.A for k in a})
        attrs = {k: np.concatenate([np.asarray(a.get(k, np.zeros(len(p))), np.float64) for a, p in zip(self.A, self.P)])
                 for k in names}
        return {"P": np.vstack(self.P), "T": np.vstack(self.T), "UV": np.concatenate(self.UV, 0),
                "N": np.vstack(self.N), "TAG": np.concatenate(self.TAG), "ATTR": attrs}


def tri_area(P, T):
    if len(T) == 0:
        return np.zeros(0)
    return 0.5 * np.linalg.norm(np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]]), axis=1)


def weld_ids(P, tol=1e-7):
    key = np.round(P / tol).astype(np.int64)
    _, inv = np.unique(key, axis=0, return_inverse=True)
    return inv.reshape(-1)


def smooth_normals(P, T):
    """Нормали точек: взвешенные площадью нормали граней по сваренным точкам (шов развёртки не виден)."""
    ids = weld_ids(P)
    fn = np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]])
    acc = np.zeros((ids.max() + 1, 3))
    for k in range(3):
        np.add.at(acc, ids[T[:, k]], fn)
    return normalize(acc[ids])


def grid_mesh(G, UVg):
    """Сетка rows x cols точек -> треугольники. Порядок (i,j),(i,j+1),(i+1,j+1) даёт нормаль
    «поперёк колонок x вдоль строк»: у тела вращения (строки — профиль снизу вверх снаружи,
    колонки — угол против часовой) это наружу."""
    R, C = G.shape[:2]
    P = G.reshape(-1, 3)
    UVf = UVg.reshape(-1, 2)
    i, j = np.meshgrid(np.arange(R - 1), np.arange(C - 1), indexing="ij")
    a = (i * C + j).ravel()
    b = (i * C + j + 1).ravel()
    c = ((i + 1) * C + j + 1).ravel()
    d = ((i + 1) * C + j).ravel()
    T = np.concatenate([np.column_stack([a, b, c]), np.column_stack([a, c, d])])
    UV = UVf[T]
    return P, T, UV


def frames(P):
    """Параллельный перенос: касательная T, нормаль N, бинормаль B (T x N = B)."""
    n = len(P)
    T = np.gradient(P, axis=0)
    T = normalize(T)
    N = np.zeros_like(P)
    ref = np.array([0, 0, 1.0]) if abs(T[0, 2]) < 0.9 else np.array([1.0, 0, 0])
    N[0] = normalize(np.cross(np.cross(T[0], ref), T[0]))
    for i in range(1, n):
        v = N[i - 1] - T[i] * np.dot(N[i - 1], T[i])
        N[i] = v / max(np.linalg.norm(v), 1e-12)
    B = np.cross(T, N)
    return T, N, B


def tube(path, radius, segs, uv_fn, cap_start=True, cap_end=True, tip=0.6):
    """Трубка по ломаной: строки — точки пути, колонки — угол (шов продублирован). uv_fn(s, a01) -> (u, v)."""
    path = np.asarray(path, np.float64)
    r = np.broadcast_to(np.asarray(radius, np.float64), (len(path),))
    Tn, Nn, Bn = frames(path)
    s = np.concatenate([[0.0], np.cumsum(np.linalg.norm(np.diff(path, axis=0), axis=1))])
    a = np.linspace(0, 2 * math.pi, segs + 1)
    G = path[:, None, :] + r[:, None, None] * (np.cos(a)[None, :, None] * Nn[:, None, :]
                                               + np.sin(a)[None, :, None] * Bn[:, None, :])
    UVg = np.stack(np.broadcast_arrays(*uv_fn(s[:, None], (a / (2 * math.pi))[None, :])), -1)
    P, T, UV = grid_mesh(G, UVg)
    P, T, UV = list(P), list(T), list(UV)
    C = segs + 1
    for i, sg in ([(0, -1)] if cap_start else []) + ([(len(path) - 1, 1)] if cap_end else []):
        tipp = path[i] + sg * Tn[i] * r[i] * tip
        ti = len(P)
        P.append(tipp)
        uv_tip = np.array(uv_fn(np.array([s[i]]), np.array([0.5]))).reshape(-1)
        for j in range(segs):
            p0, p1 = i * C + j, i * C + j + 1
            tri = (p0, p1, ti) if sg > 0 else (p1, p0, ti)
            T = np.vstack([T, [tri]])
            UV.append(np.array([UVg[i, tri[0] - i * C], UVg[i, tri[1] - i * C], uv_tip]))
    return np.array(P), np.array(T).reshape(-1, 3), np.array(UV).reshape(-1, 3, 2)


# ---------------------------------------------------------------- выпуклые многогранники
def box_polys(lo, hi):
    x0, y0, z0 = lo
    x1, y1, z1 = hi
    V = np.array([[x0, y0, z0], [x1, y0, z0], [x0, y1, z0], [x1, y1, z0],
                  [x0, y0, z1], [x1, y0, z1], [x0, y1, z1], [x1, y1, z1]], np.float64)
    quads = [(0, 4, 6, 2), (1, 3, 7, 5), (0, 1, 5, 4), (2, 6, 7, 3), (0, 2, 3, 1), (4, 5, 7, 6)]
    return [V[list(q)] for q in quads]


def poly_normal(V):
    c = V.mean(0)
    n = np.zeros(3)
    for i in range(len(V)):
        n += np.cross(V[i] - c, V[(i + 1) % len(V)] - c)
    return normalize(n)


def clip_polys(polys, n, o, eps=1e-9):
    """Отсечь выпуклый многогранник (список граней — многоугольники против часовой снаружи)
    полупространством n·x <= o, отверстие закрыть новой гранью."""
    n = normalize(np.asarray(n, np.float64))
    out, cut = [], []
    for V in polys:
        d = V @ n - o
        if np.all(d <= eps):
            out.append(V)
            cut += [V[k] for k in range(len(V)) if abs(d[k]) <= eps]
            continue
        if np.all(d >= -eps):
            cut += [V[k] for k in range(len(V)) if abs(d[k]) <= eps]
            continue
        res = []
        for k in range(len(V)):
            a, b = V[k], V[(k + 1) % len(V)]
            da, db = d[k], d[(k + 1) % len(V)]
            if da <= eps:
                res.append(a)
                if abs(da) <= eps:
                    cut.append(a)
            if (da < -eps and db > eps) or (da > eps and db < -eps):
                p = a + (b - a) * (da / (da - db))
                res.append(p)
                cut.append(p)
        res = dedup_ring(np.array(res))
        if len(res) >= 3:
            out.append(res)
    if len(cut) >= 3:
        C = np.unique(np.round(np.array(cut) / 1e-9).astype(np.int64), axis=0) * 1e-9
        if len(C) >= 3:
            c = C.mean(0)
            e1 = normalize(C[0] - c)
            e2 = np.cross(n, e1)
            ang = np.arctan2((C - c) @ e2, (C - c) @ e1)
            out.append(C[np.argsort(ang)])
    return out


def dedup_ring(V, tol=1e-9):
    keep = []
    for k in range(len(V)):
        if np.linalg.norm(V[k] - V[(k + 1) % len(V)]) > tol:
            keep.append(V[k])
    return np.array(keep)


def snap_polys(polys, tol):
    """Сварить почти совпадающие вершины многогранника (после отсечений бывают рёбра в доли мм),
    убрать вырожденные грани — иначе оболочка Geometry с дырами."""
    allp = np.vstack(polys)
    rep_ = []
    lab = np.full(len(allp), -1)
    for i in range(len(allp)):
        if lab[i] >= 0:
            continue
        close = np.linalg.norm(allp - allp[i], axis=1) < tol
        lab[close & (lab < 0)] = len(rep_)
        rep_.append(allp[close].mean(0))
    rep_ = np.array(rep_)
    out, k = [], 0
    for V in polys:
        ids = list(lab[k:k + len(V)])
        k += len(V)
        ring = []
        for i in ids:
            if not ring or ring[-1] != i:
                ring.append(i)
        while len(ring) > 1 and ring[0] == ring[-1]:
            ring.pop()
        if len(set(ring)) >= 3:
            W = rep_[ring]
            if np.linalg.norm(np.cross(W[1] - W[0], W[2] - W[0])) > 1e-9 or len(W) > 3:
                out.append(W)
    return out


def polys_to_tris(polys):
    """Многоугольники -> общий список точек (сварка) и треугольников веером — для Geometry LOD."""
    P, T = [], []
    for V in polys:
        base = len(P)
        P += list(V)
        for k in range(1, len(V) - 1):
            T.append((base, base + k, base + k + 1))
    P = np.array(P)
    ids = weld_ids(P, 1e-7)
    uniq = np.zeros((ids.max() + 1, 3))
    uniq[ids] = P
    return uniq, ids[np.array(T)]


# ---------------------------------------------------------------- развёртка плоских граней (кусок, брикет)
def chart_frame(V):
    """Базис грани: e1 вдоль длинного ребра, e2 = n x e1, начало — в углу габарита."""
    n = poly_normal(V)
    k = int(np.argmax([np.linalg.norm(V[(i + 1) % len(V)] - V[i]) for i in range(len(V))]))
    e1 = normalize(V[(k + 1) % len(V)] - V[k])
    e2 = np.cross(n, e1)
    x, y = V @ e1, V @ e2
    return n, e1, e2, np.array([x.min(), y.min()]), np.array([x.max() - x.min(), y.max() - y.min()])


def pack_charts(sizes, gutter):
    """Полки: возвращает масштаб (UV на метр) и левый верхний угол каждой карты."""
    order = sorted(range(len(sizes)), key=lambda i: -sizes[i][1])

    def try_pack(s):
        x = y = gutter
        row = 0.0
        pos = {}
        for i in order:
            w, h = sizes[i][0] * s, sizes[i][1] * s
            if w + 2 * gutter > 1:
                return None
            if x + w + gutter > 1:
                x = gutter
                y += row + gutter
                row = 0.0
            pos[i] = (x, y)
            x += w + gutter
            row = max(row, h)
        return pos if y + row + gutter <= 1 else None

    lo, hi = 1.0, 1000.0
    for _ in range(50):
        mid = (lo + hi) / 2
        if try_pack(mid):
            lo = mid
        else:
            hi = mid
    return lo, try_pack(lo)


# ================================================================ формы
# Все размеры в метрах, оси Blender: Z вверх, -Y — перёд. Центр модели — центр габарита.

# ---------------------------------------------------------------- 1. крупицы: горсть зёрен
# 12 неровных угловатых зёрен разного размера (как намытые крупицы): девять лежат кучкой, три сверху на соседях.
# Каждое зерно — кубосфера, как самородок, только мельче и площе; в атласе своя клетка 4x3, грани куба — 3x2 внутри.
GRAIN_N = 12
GRAIN_COLS, GRAIN_ROWS = 4, 3              # клетки атласа под зёрна
GRAIN_CAT = [0, 1, 0, 1, 0, 2, 1, 0, 1, 0, 2, 1]   # оттенок зерна из SAND_MIX (только основные цвета ресурса)


def _fib_dirs(n):
    i = np.arange(n) + 0.5
    z = 1 - 2 * i / n
    r = np.sqrt(1 - z * z)
    a = i * math.pi * (3 - math.sqrt(5))
    return np.column_stack([r * np.cos(a), r * np.sin(a), z])


def _rot(yaw, tilt_axis, tilt):
    cy, sy = math.cos(yaw), math.sin(yaw)
    Rz = np.array([[cy, -sy, 0], [sy, cy, 0], [0, 0, 1.0]])
    ax = normalize(np.array([tilt_axis[0], tilt_axis[1], 0.0]))
    K = np.array([[0, -ax[2], ax[1]], [ax[2], 0, -ax[0]], [-ax[1], ax[0], 0]])
    Rt = np.eye(3) + math.sin(tilt) * K + (1 - math.cos(tilt)) * (K @ K)
    return Rt @ Rz


def grain_local(d, g):
    """Точка поверхности зерна по направлению d (в осях зерна) и признаки: cav — впадина, high — выпуклость."""
    abc = g["abc"]
    re = 1.0 / np.sqrt(((d / abc) ** 2).sum(1))
    lob = np.zeros(len(d))
    lob_wide = np.zeros(len(d))
    for u, a, w in g["lobes"]:
        lob += a * np.exp(-(1 - d @ u) / w)
        lob_wide += a * 0.5 * np.exp(-(1 - d @ u) / (3 * w))
    det = 0.11 * fbm(d * 3.2 + g["off"], 4, seed=g["seed"]) + 0.06 * fbm(d * 8.5 + g["off"], 3, seed=g["seed"] + 1)
    smooth = re * (1 + lob_wide)
    r = re * (1 + lob) * (1 + det)
    p = d * r[:, None]
    zf = 0.70 * abc[2]                                # снизу плоско — лежит, а не катается
    k = 16.0 / abc[2]
    p[:, 2] = -zf + np.logaddexp(0, k * (p[:, 2] + zf)) / k
    rel = (r - smooth) / smooth
    return p, np.clip(-rel / 0.08, 0, 1), np.clip(rel / 0.10, 0, 1)


def _grain_params():
    rng = np.random.default_rng(4242)
    sizes = np.sort(rng.uniform(0.0028, 0.0062, GRAIN_N))[::-1]      # полудлина зерна, м
    grains = []
    for i, a in enumerate(sizes):
        g = {"abc": np.array([a, a * rng.uniform(0.60, 0.85), a * rng.uniform(0.42, 0.62)]),
             "seed": 300 + 11 * i, "off": rng.uniform(-50, 50, 3), "lobes": []}
        for _ in range(9):
            u = normalize(rng.normal(size=3) * np.array([1.0, 1.0, 0.5]))
            g["lobes"].append((u, rng.uniform(0.10, 0.30), rng.uniform(0.03, 0.10)))
        grains.append(g)
    # раскладка: 9 на земле плотной кучкой, 3 мелких сверху на крупных
    placed = []
    for i, g in enumerate(grains[:9]):
        a = g["abc"][0]
        best = None
        for _ in range(600):
            ang = rng.uniform(0, 2 * math.pi)
            rad = 0.0 if i == 0 else rng.uniform(0.004, 0.014)
            x, y = rad * math.cos(ang), 0.8 * rad * math.sin(ang)
            gap = min([math.hypot(x - px, y - py) - 0.68 * (a + pa) for px, py, pa in placed] or [1.0])
            if gap > 0:
                best = (x, y)
                break
            if best is None or gap > best[2]:
                best = (x, y, gap)
        placed.append((best[0], best[1], a))
        g["xy"] = (best[0], best[1])
        g["zb"] = 0.0
        g["R"] = _rot(rng.uniform(0, 2 * math.pi), rng.normal(size=2), rng.uniform(-0.12, 0.12))
    for j, g in enumerate(grains[9:]):
        host = grains[j]
        hx, hy = host["xy"]
        ang = rng.uniform(0, 2 * math.pi)
        g["xy"] = (hx + 0.35 * host["abc"][0] * math.cos(ang), hy + 0.35 * host["abc"][1] * math.sin(ang))
        g["zb"] = 1.45 * host["abc"][2]
        g["R"] = _rot(rng.uniform(0, 2 * math.pi), (math.cos(ang), math.sin(ang)), rng.uniform(0.18, 0.32))
    # низ каждого зерна на своей высоте (по плотной выборке направлений — одинаково для всех LOD)
    dirs = _fib_dirs(1500)
    for g in grains:
        p = grain_local(dirs, g)[0] @ g["R"].T
        g["t"] = np.array([g["xy"][0], g["xy"][1], g["zb"] - p[:, 2].min()])
    return grains


GRAINS = _grain_params()


def build_sand(detail):
    """detail 0 — полная деталь, 1 — проще (LOD 4.0)."""
    N = (8, 4)[detail]
    mesh = Mesh()
    t = np.tan(np.linspace(-1, 1, N + 1) * math.pi / 4)
    ins = 0.02
    for gi, g in enumerate(GRAINS):
        cu, cv = gi % GRAIN_COLS, gi // GRAIN_COLS
        for fi, (ax, au, av) in enumerate(CUBE_FACES):
            S, Tt = np.meshgrid(t, t, indexing="xy")
            d = normalize(ax[None, None, :] + S[..., None] * au + Tt[..., None] * av).reshape(-1, 3)
            p, cav, high = grain_local(d, g)
            e = 2e-3
            t1 = normalize(np.cross(d, np.where(np.abs(d[:, 2:3]) < 0.9, [[0, 0, 1.0]], [[1.0, 0, 0]])))
            t2 = np.cross(d, t1)
            pa = grain_local(normalize(d + e * t1), g)[0] - grain_local(normalize(d - e * t1), g)[0]
            pb = grain_local(normalize(d + e * t2), g)[0] - grain_local(normalize(d - e * t2), g)[0]
            nrm = normalize(np.cross(pa, pb))
            nrm = np.where((nrm * d).sum(1, keepdims=True) < 0, -nrm, nrm)
            pw = p @ g["R"].T + g["t"]
            nw = nrm @ g["R"].T
            su = np.linspace(0, 1, N + 1)
            U, V = np.meshgrid(su, su[::-1], indexing="xy")
            col_i, row_i = fi % 3, fi // 3
            UVg = np.stack([(cu + (col_i + ins + U * (1 - 2 * ins)) / 3) / GRAIN_COLS,
                            (cv + (row_i + ins + V * (1 - 2 * ins)) / 2) / GRAIN_ROWS], -1)
            P, T, UV = grid_mesh(pw.reshape(N + 1, N + 1, 3), UVg)
            fn = np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]])
            if np.mean((fn * (P[T].mean(1) - g["t"])).sum(1)) < 0:
                T = T[:, [0, 2, 1]]
                UV = UV[:, [0, 2, 1]]
            mesh.add(P, T, UV, N=nw, tag=gi, attrs={"cav": cav, "high": high})
    return mesh.finish()


# ---------------------------------------------------------------- 2. самородок (кубосфера)
CUBE_FACES = [  # ось, u, v
    (np.array([1.0, 0, 0]), np.array([0, 1.0, 0]), np.array([0, 0, 1.0])),
    (np.array([-1.0, 0, 0]), np.array([0, -1.0, 0]), np.array([0, 0, 1.0])),
    (np.array([0, 1.0, 0]), np.array([-1.0, 0, 0]), np.array([0, 0, 1.0])),
    (np.array([0, -1.0, 0]), np.array([1.0, 0, 0]), np.array([0, 0, 1.0])),
    (np.array([0, 0, 1.0]), np.array([1.0, 0, 0]), np.array([0, 1.0, 0])),
    (np.array([0, 0, -1.0]), np.array([1.0, 0, 0]), np.array([0, -1.0, 0])),
]


def _nugget_params():
    rng = np.random.default_rng(5150)
    lobes = []                                        # бугры-«наплывы»: много, узкие — неровный контур
    for _ in range(12):
        u = normalize(rng.normal(size=3) * np.array([1.0, 0.9, 0.5]))
        lobes.append((u, rng.uniform(0.10, 0.30), rng.uniform(0.035, 0.14)))
    creases = []                                      # узкие складки между наплывами
    for _ in range(3):
        nrm = normalize(rng.normal(size=3))
        ctr = normalize(np.cross(nrm, rng.normal(size=3)))
        creases.append((nrm, ctr, rng.uniform(0.04, 0.08)))
    return lobes, creases


NUG_LOBES, NUG_CREASES = _nugget_params()
NUG_ABC = np.array([0.031, 0.0235, 0.0185])


def nugget_surface(d):
    """Точка поверхности по направлению d (n,3) и признаки: cav — впадина, high — выпуклость."""
    re = 1.0 / np.sqrt(((d / NUG_ABC) ** 2).sum(1))
    lob = np.zeros(len(d))
    lob_wide = np.zeros(len(d))                       # те же бугры, размытые: относительно них ищем ложбины
    for u, a, w in NUG_LOBES:
        lob += a * np.exp(-(1 - d @ u) / w)
        lob_wide += a * 0.5 * np.exp(-(1 - d @ u) / (3 * w))
    cr = np.zeros(len(d))
    for nrm, ctr, a in NUG_CREASES:
        cr += a * np.exp(-((d @ nrm) / 0.05) ** 2) * np.exp(-(1 - d @ ctr) / 0.45)
    det = 0.05 * fbm(d * 2.3, 4, seed=41) + 0.03 * fbm(d * 6.5, 3, seed=42)
    smooth = re * (1 + lob_wide)
    r = re * (1 + lob) * (1 - cr) * (1 + det)
    p = d * r[:, None]
    zf = 0.68 * NUG_ABC[2]                           # плоское основание — лежит, а не катается
    k = 900.0
    p[:, 2] = -zf + np.logaddexp(0, k * (p[:, 2] + zf)) / k
    rel = (r - smooth) / smooth
    return p, np.clip(-rel / 0.08, 0, 1), np.clip(rel / 0.10, 0, 1)


def build_nugget(detail):
    N = (22, 11)[detail]
    mesh = Mesh()
    t = np.tan(np.linspace(-1, 1, N + 1) * math.pi / 4)
    for fi, (ax, au, av) in enumerate(CUBE_FACES):
        S, Tt = np.meshgrid(t, t, indexing="xy")       # строки — v, колонки — u
        dirs = normalize(ax[None, None, :] + S[..., None] * au + Tt[..., None] * av)
        d = dirs.reshape(-1, 3)
        p, cav, high = nugget_surface(d)
        # нормали — разностями по самой поверхности (одинаковы на стыках граней куба)
        e = 2e-3
        t1 = normalize(np.cross(d, np.where(np.abs(d[:, 2:3]) < 0.9, [[0, 0, 1.0]], [[1.0, 0, 0]])))
        t2 = np.cross(d, t1)
        pa = nugget_surface(normalize(d + e * t1))[0] - nugget_surface(normalize(d - e * t1))[0]
        pb = nugget_surface(normalize(d + e * t2))[0] - nugget_surface(normalize(d - e * t2))[0]
        nrm = normalize(np.cross(pa, pb))
        nrm = np.where((nrm * d).sum(1, keepdims=True) < 0, -nrm, nrm)
        col_i, row_i = fi % 3, fi // 3
        ins = 0.012
        su = (np.linspace(0, 1, N + 1))
        U, V = np.meshgrid(su, su[::-1], indexing="xy")    # v вниз по картинке, «вверх» грани — к верху ячейки
        UVg = np.stack([(col_i + ins + U * (1 - 2 * ins)) / 3, (row_i + ins + V * (1 - 2 * ins)) / 2], -1)
        G = p.reshape(N + 1, N + 1, 3)
        P, T, UV = grid_mesh(G, UVg)
        # ориентация: наружу (по направлению d)
        fn = np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]])
        if np.mean((fn * P[T].mean(1)).sum(1)) < 0:
            T = T[:, [0, 2, 1]]
            UV = UV[:, [0, 2, 1]]
        mesh.add(P, T, UV, N=nrm, tag=0, attrs={"cav": cav, "high": high})
    return mesh.finish()


# ---------------------------------------------------------------- 3. кусок с гранями
CHUNK_ABC = np.array([0.044, 0.030, 0.026])


def chunk_polys():
    rng = np.random.default_rng(777)
    a, b, c = CHUNK_ABC
    polys = box_polys(-CHUNK_ABC * 1.05, CHUNK_ABC * 1.05)

    def support(n):
        return math.sqrt((a * n[0]) ** 2 + (b * n[1]) ** 2 + (c * n[2]) ** 2)
    k = 10                                             # крупные сколы разной глубины — грани неравные
    gold = math.pi * (3 - math.sqrt(5))
    for i in range(k):
        zz = 1 - 2 * (i + 0.5) / k
        rr = math.sqrt(1 - zz * zz)
        n = normalize(np.array([rr * math.cos(gold * i), rr * math.sin(gold * i), zz]) + rng.normal(size=3) * 0.35)
        polys = clip_polys(polys, n, support(n) * rng.uniform(0.62, 0.86))
    for i in range(11):                                # мелкие сколы на углах
        n = normalize(rng.normal(size=3))
        polys = clip_polys(polys, n, support(n) * rng.uniform(0.78, 0.90))
    polys = clip_polys(polys, np.array([0.08, -0.05, -1.0]), c * 0.60)   # основание
    polys = snap_polys(polys, 4e-4)
    allp = np.vstack(polys)
    ctr = (allp.min(0) + allp.max(0)) / 2
    return [V - ctr for V in polys]


CHUNK_POLYS = chunk_polys()


def _facet_impacts():
    rng = np.random.default_rng(91)
    out = []
    for V in CHUNK_POLYS:
        n, e1, e2, o, sz = chart_frame(V)
        c = V.mean(0) + e1 * rng.uniform(-0.6, 0.6) * sz[0] * 0.6 + e2 * rng.uniform(-0.6, 0.6) * sz[1] * 0.6
        out.append((c, rng.uniform(0, 6.28), rng.choice([-1.0, 1.0]) * rng.uniform(0.6, 1.0)))
    return out


CHUNK_IMPACT = _facet_impacts()


def poly_edge_dist(V, n, pts):
    """Расстояние от точек в плоскости грани до её контура."""
    best = np.full(len(pts), 1e9)
    for k in range(len(V)):
        a, b = V[k], V[(k + 1) % len(V)]
        ab = b - a
        t = np.clip(((pts - a) @ ab) / max(ab @ ab, 1e-18), 0, 1)
        best = np.minimum(best, np.linalg.norm(pts - (a + t[:, None] * ab), axis=1))
    return best


def facet_charts(polys, gutter_px=6, size=CO_SIZE):
    frames_ = [chart_frame(V) for V in polys]
    scale, pos = pack_charts([f[4] for f in frames_], gutter_px / size)
    return frames_, scale, pos


def build_facets(polys, sub, displace, tag_base=0, size=CO_SIZE):
    """Грани многогранника -> меш: каждая грань веером, треугольники веера делятся на sub^2,
    внутренние точки грани сдвигаются по нормали displace(facet, точки, окно) (рёбра на месте — без щелей).
    UV — плоская проекция грани в свою карту атласа."""
    frames_, scale, pos = facet_charts(polys, size=size)
    mesh = Mesh()
    for fi, V in enumerate(polys):
        n, e1, e2, o, sz = frames_[fi]
        c = V.mean(0)
        pts, tris = [], []
        for k in range(len(V)):
            A, B, C = c, V[k], V[(k + 1) % len(V)]
            base = len(pts)
            idx = {}
            for i in range(sub + 1):
                for j in range(sub + 1 - i):
                    idx[(i, j)] = len(pts)
                    pts.append(A + (B - A) * (i / sub) + (C - A) * (j / sub))
            for i in range(sub):
                for j in range(sub - i):
                    tris.append((idx[(i, j)], idx[(i + 1, j)], idx[(i, j + 1)]))
                    if i + j + 1 < sub:
                        tris.append((idx[(i + 1, j)], idx[(i + 1, j + 1)], idx[(i, j + 1)]))
        pts = np.array(pts)
        tris = np.array(tris)
        ids = weld_ids(pts, 1e-9)
        uniq = np.zeros((ids.max() + 1, 3))
        uniq[ids] = pts
        tris = ids[tris]
        pts = uniq
        # ориентация наружу
        fnn = np.cross(pts[tris[:, 1]] - pts[tris[:, 0]], pts[tris[:, 2]] - pts[tris[:, 0]])
        flip = (fnn @ n) < 0
        tris[flip] = tris[flip][:, [0, 2, 1]]
        ed = poly_edge_dist(V, n, pts)
        inr = max(ed.max(), 1e-6)
        if displace is not None and sub > 1:
            pts = pts + n[None, :] * displace(fi, pts, sstep(0, 1, ed / (0.75 * inr)))[:, None]
        x = (pts @ e1 - o[0]) * scale + pos[fi][0]
        y = (pts @ e2 - o[1]) * scale + pos[fi][1]
        uvp = np.column_stack([x, y])
        P, T = pts, tris
        UV = uvp[T]
        mesh.add(P, T, UV, tag=tag_base + fi, attrs={"edge": ed})
    return mesh.finish()


def chunk_displace(fi, pts, win):
    c, ph, sg = CHUNK_IMPACT[fi]
    d = np.linalg.norm(pts - c, axis=1)
    return win * (-0.0011 * sg * (1 - np.clip(d / 0.03, 0, 1)) ** 1.5)       # пологая раковина, без колец


def build_chunk(detail):
    return build_facets(CHUNK_POLYS, (4, 1)[detail], chunk_displace)


# ---------------------------------------------------------------- 4. слиток
ING_A0, ING_B0 = 0.125, 0.050      # половина длины и ширины основания
ING_A1, ING_B1 = 0.1125, 0.0375    # верх
ING_H = 0.065
ING_RC = 0.008                     # скругление вертикальных рёбер (в плане)
ING_RR = 0.0045                    # скругление верхнего и нижнего ребра


def rrect_loop(a, b, rc, n_long, n_short, n_arc):
    """Скруглённый прямоугольник против часовой, начало — середина торца +X; точка n/2 — середина торца -X."""
    pts = []
    rc = min(rc, a * 0.9, b * 0.9)

    def side(p0, p1, k, incl_end=False):
        for i in range(k + (1 if incl_end else 0)):
            pts.append(p0 + (p1 - p0) * i / k)

    def arc(cx, cy, a0, k):
        for i in range(k):
            t = a0 + (math.pi / 2) * i / k
            pts.append(np.array([cx + rc * math.cos(t), cy + rc * math.sin(t)]))
    side(np.array([a, 0.0]), np.array([a, b - rc]), n_short)
    arc(a - rc, b - rc, 0.0, n_arc)
    side(np.array([a - rc, b]), np.array([-a + rc, b]), n_long)
    arc(-a + rc, b - rc, math.pi / 2, n_arc)
    side(np.array([-a, b - rc]), np.array([-a, 0.0]), n_short)
    side(np.array([-a, 0.0]), np.array([-a, -b + rc]), n_short)
    arc(-a + rc, -b + rc, math.pi, n_arc)
    side(np.array([-a + rc, -b]), np.array([a - rc, -b]), n_long)
    arc(a - rc, -b + rc, 1.5 * math.pi, n_arc)
    side(np.array([a, -b + rc]), np.array([a, 0.0]), n_short)
    return np.array(pts)


def ing_half(z):
    t = z / ING_H
    return ING_A0 + (ING_A1 - ING_A0) * t, ING_B0 + (ING_B1 - ING_B0) * t


# развёртка слитка: верх, две половины боковины, низ (v вниз)
ING_UV = {"top": (0.004, 0.360), "side0": (0.368, 0.588), "side1": (0.596, 0.816), "bot": (0.824, 0.996)}
ING_TOP_K = 0.992 / (2 * ING_A1)                 # UV на метр для верха
ING_BOT_K = (ING_UV["bot"][1] - ING_UV["bot"][0]) / (2 * ING_B0)


def build_ingot(detail):
    n_long, n_short, n_arc, n_rr, n_side, n_cap = ((24, 5, 6, 5, 4, 7), (12, 3, 3, 3, 2, 4))[detail]
    mesh = Mesh()
    # профиль боковины: (отступ внутрь, z, признак ребра)
    prof = []
    for i in range(n_rr + 1):
        f = (math.pi / 2) * i / n_rr
        prof.append((ING_RR * (1 - math.sin(f)), ING_RR * (1 - math.cos(f)), 1.0 - i / n_rr))
    for i in range(1, n_side):
        prof.append((0.0, ING_RR + (ING_H - 2 * ING_RR) * i / n_side, 0.0))
    for i in range(n_rr + 1):
        f = (math.pi / 2) * i / n_rr
        prof.append((ING_RR * (1 - math.cos(f)), ING_H - ING_RR + ING_RR * math.sin(f), i / n_rr))
    prof = np.array(prof)
    loops = []
    for inset, z, _ in prof:
        a, b = ing_half(z)
        loop = rrect_loop(a - inset, b - inset, ING_RC - 0.6 * inset, n_long, n_short, n_arc)
        loops.append(np.column_stack([loop, np.full(len(loop), z)]))
    L = np.array(loops)                                          # (строки профиля, точки контура, 3)
    npt = L.shape[1]
    half = npt // 2
    # длина профиля и периметр (по средней строке) — для UV боковины
    sprof = np.concatenate([[0.0], np.cumsum(np.linalg.norm(np.diff(prof[:, :2], axis=0), axis=1))])
    mid = L[len(L) // 2]
    per = np.concatenate([[0.0], np.cumsum(np.linalg.norm(np.diff(np.vstack([mid, mid[:1]]), axis=0), axis=1))])
    edge_attr = np.repeat(prof[:, 2][:, None], 1, 1)
    for h, (c0, c1) in enumerate(((0, half), (half, npt))):
        cols = list(range(c0, c1)) + [c1 % npt]
        G = L[:, cols]
        pu = per[c0:c1 + 1] - per[c0]
        pu = pu / pu[-1]
        v0, v1 = ING_UV["side%d" % h]
        UVg = np.stack(np.broadcast_arrays(0.004 + 0.992 * pu[None, :], (v1 - (v1 - v0) * sprof / sprof[-1])[:, None]), -1)
        P, T, UV = grid_mesh(G, UVg)
        # вертикальные скругления тоже «ребро» (износ)
        a_, b_ = ing_half(G[..., 2])
        corner = np.clip((np.abs(G[..., 0]) - (a_ - ING_RC)) / ING_RC, 0, 1) * np.clip((np.abs(G[..., 1]) - (b_ - ING_RC)) / ING_RC, 0, 1)
        ed = np.maximum(np.broadcast_to(edge_attr, G.shape[:2]), corner ** 0.5)
        mesh.add(P, T, UV, tag=1, attrs={"edge": ed.reshape(-1)})
    # крышки: верх (слегка вогнут — усадка) и низ; кольца от края к центру
    for which in ("top", "bot"):
        ring = L[-1] if which == "top" else L[0]
        z0 = ring[0, 2]
        rho = np.linspace(1, 0, n_cap + 1)
        G = ring[None, :, :2] * rho[:, None, None]
        zz = np.full(G.shape[:2], z0)
        if which == "top":
            zz = zz - 0.0007 * (1 - rho[:, None] ** 2)
        G = np.concatenate([G, zz[..., None]], -1)
        G = np.concatenate([G, G[:, :1]], 1)                    # замкнуть контур
        if which == "top":
            v0, v1 = ING_UV["top"]
            UVg = np.stack([0.5 + G[..., 0] * ING_TOP_K, (v0 + v1) / 2 - G[..., 1] * ING_TOP_K], -1)
        else:
            v0, v1 = ING_UV["bot"]
            UVg = np.stack([0.5 + G[..., 0] * ING_BOT_K, (v0 + v1) / 2 + G[..., 1] * ING_BOT_K], -1)
        P, T, UV = grid_mesh(G, UVg)
        fn = np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]])
        want = 1 if which == "top" else -1
        bad = fn[:, 2] * want < 0
        T[bad] = T[bad][:, [0, 2, 1]]
        UV[bad] = UV[bad][:, [0, 2, 1]]
        mesh.add(P, T, UV, tag=0 if which == "top" else 2, attrs={"edge": np.zeros(len(P))})
    m = mesh.finish()
    # общие нормали на стыке крышек и боковины
    m["N"] = smooth_normals(m["P"], m["T"])
    m["P"][:, 2] -= ING_H / 2
    return m


def ingot_hull():
    lo = []
    for z in (0.0, ING_H):
        a, b = ing_half(z)
        lo.append((a, b, z))
    (a0, b0, _), (a1, b1, _) = lo
    V = []
    for i in range(8):
        a, b = (a1, b1) if i & 4 else (a0, b0)
        V.append(((a if i & 1 else -a), (b if i & 2 else -b), (ING_H if i & 4 else 0.0) - ING_H / 2))
    V = np.array(V)
    quads = [(0, 4, 6, 2), (1, 3, 7, 5), (0, 1, 5, 4), (2, 6, 7, 3), (0, 2, 3, 1), (4, 5, 7, 6)]
    return [V[list(q)] for q in quads]


# ---------------------------------------------------------------- 5. брикет
BRQ_H = np.array([0.040, 0.030, 0.022])
BRQ_C = 0.0045


def briquette_polys():
    A, B, C = BRQ_H
    polys = box_polys(-BRQ_H, BRQ_H)
    h = BRQ_H
    for i, j in ((0, 1), (1, 2), (0, 2)):
        for si in (-1, 1):
            for sj in (-1, 1):
                n = np.zeros(3)
                n[i], n[j] = si, sj
                polys = clip_polys(polys, n, (h[i] + h[j] - BRQ_C) / math.sqrt(2))
    for sx in (-1, 1):
        for sy in (-1, 1):
            for sz in (-1, 1):
                n = np.array([sx, sy, sz], np.float64)
                polys = clip_polys(polys, n, (A + B + C - 1.85 * BRQ_C) / math.sqrt(3))
    return polys


BRQ_POLYS = briquette_polys()


def build_briquette(detail):
    return build_facets(BRQ_POLYS, 1, None)


BUILDERS = {"sand": build_sand, "nugget": build_nugget, "chunk": build_chunk, "ingot": build_ingot,
            "briquette": build_briquette}


def center_mesh(m, ctr=None):
    if ctr is None:
        ctr = (m["P"].min(0) + m["P"].max(0)) / 2
    m["P"] = m["P"] - ctr
    return ctr


def hull_for(shape, m0):
    """Выпуклая оболочка для Geometry/View/Fire: многоугольники в осях модели (после центрирования)."""
    if shape == "chunk":
        return CHUNK_POLYS
    if shape == "briquette":
        return BRQ_POLYS
    if shape == "ingot":
        return ingot_hull()
    P = m0["P"]
    lo, hi = P.min(0), P.max(0)
    c = (lo + hi) / 2
    hl = (hi - lo) / 2
    k = np.array([0.86, 0.86, 1.0])
    lo2 = c - hl * k
    hi2 = c + hl * k
    hi2[2] = c[2] + hl[2] * 0.9
    return box_polys(lo2, hi2)


# ================================================================ p3d
def visual_lod(m, resolution, tex, mat):
    lod = py3d.LOD()
    lod.resolution = resolution
    P, T, UV, N = m["P"], m["T"], m["UV"], m["N"]
    for c in P:
        p = py3d.Point()
        p.coords = (float(c[0]), float(c[1]), float(c[2]))
        p.flags = 0
        lod.points.append(p)
    for n in N:
        lod.facenormals.append((float(n[0]), float(n[1]), float(n[2])))
    fn = normalize(np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]]))
    sel = lod.new_selection("camo")
    for k in range(len(T)):
        f = py3d.Face(lod.points, lod.facenormals)
        f.texture = tex
        f.material = mat
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
            sel.points[lod.points[idx]] = 1
        lod.faces.append(f)
        sel.faces[f] = 1
    return lod


def hull_lod(resolution, polys, pen, mass=None):
    P, T = polys_to_tris(polys)
    lod = py3d.LOD()
    lod.resolution = resolution
    for c in P:
        p = py3d.Point()
        p.coords = (float(c[0]), float(c[1]), float(c[2]))
        p.mass = (mass / len(P)) if mass is not None else None
        lod.points.append(p)
    fn = normalize(np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]]))
    cen = P.mean(0)
    comp = lod.new_selection("Component01")
    for k in range(len(T)):
        tri = T[k]
        n = fn[k]
        if np.dot(P[tri].mean(0) - cen, n) < 0:      # наружу (против часовой снаружи, как у визуальных)
            tri = tri[[0, 2, 1]]
            n = -n
        lod.facenormals.append((float(n[0]), float(n[1]), float(n[2])))
        f = py3d.Face(lod.points, lod.facenormals)
        f.material = pen
        f.texture = ""
        for idx in tri:
            v = py3d.Vertex(lod.points, lod.facenormals)
            v.point_index = int(idx)
            v.normal_index = len(lod.facenormals) - 1
            v.uv = (0.0, 0.0)
            f.vertices.append(v)
        lod.faces.append(f)
        comp.faces[f] = 1
    for p in lod.points:
        comp.points[p] = 1
    lod.properties["autocenter"] = "0"
    return lod


def write_p3d(shape, meshes, hull, out_path):
    res = DEFAULT_RES[shape]
    tex = PREFIX + "vb_mr_%s_%s_co.paa" % (res, shape)
    mat = PREFIX + "vb_mr_%s_%s.rvmat" % (res, shape)
    model = py3d.P3D()
    for m, r in zip(meshes, (0.0, 1.0, 4.0)):
        model.lods.append(visual_lod(m, r, tex, mat))
    pen = PEN[SHAPES[shape]["pen"]]
    model.lods.append(hull_lod(1e13, hull, pen, SHAPES[shape]["mass"]))
    mem = py3d.LOD()
    mem.resolution = 1e15
    model.lods.append(mem)
    model.lods.append(hull_lod(6e15, hull, pen))
    model.lods.append(hull_lod(7e15, hull, pen))
    py3d.blender_to_dayz(model)
    tilt = PREVIEW_TILT.get(shape)
    if tilt:
        # превью инвентаря показывает модель в её собственных осях, камера смотрит с +Z: плоский предмет, лежащий
        # плашмя, виден ребром (серый прямоугольник без клейма, 2026-11-19, каталог E109). Разворот на 180° вокруг
        # вертикали (иначе клеймо вверх ногами) и наклон вокруг X ставят верх (+Y, клеймо) к камере; подъём —
        # чтобы ладони двуручного хвата камня (слиток) лежали на предмете, а не над ним. На землю ложится физикой.
        deg, lift = tilt
        a = math.radians(deg)
        c, s = math.cos(a), math.sin(a)
        model.transform(((-1, 0, 0), (0, c, s), (0, s, -c)))      # R_x(deg) · R_y(180°), det = +1
        if lift:
            for lod in model.lods:
                for p in lod.points:
                    p.coords = (p.coords[0], p.coords[1] + lift, p.coords[2])
    model.save(out_path, verify=True)
    return [f for f in model.validate() if getattr(f, "severity", "") in ("ERROR", "WARN")]


# ================================================================ запекание
class Ctx:
    """Тексели атласа на модели: точка, нормаль, касательные, UV, метка куска, атрибуты."""

    def __init__(self, **kw):
        self.__dict__.update(kw)

    def shifted(self, vec_name, eps):
        d = getattr(self, vec_name)
        duv = (self.Jinv @ d[:, :, None])[:, :, 0]
        return Ctx(pos=self.pos + eps * d, uv=self.uv + eps * duv, nrm=self.nrm, tag=self.tag, attr=self.attr,
                   T=self.T, B=self.B, Jinv=self.Jinv, n=self.n)


def bake(m, size):
    """Растеризация треугольников в атлас size x size: на каждый тексель — грань и барицентры."""
    W = H = size
    face = np.full((H, W), -1, np.int64)
    bary = np.zeros((H, W, 3))
    UVp = m["UV"] * np.array([W, H])
    for k in range(len(m["T"])):
        (x0, y0), (x1, y1), (x2, y2) = UVp[k]
        den = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2)
        if abs(den) < 1e-12:
            continue
        xa = max(int(math.floor(min(x0, x1, x2) - 0.5)), 0)
        xb = min(int(math.ceil(max(x0, x1, x2) + 0.5)), W - 1)
        ya = max(int(math.floor(min(y0, y1, y2) - 0.5)), 0)
        yb = min(int(math.ceil(max(y0, y1, y2) + 0.5)), H - 1)
        if xb < xa or yb < ya:
            continue
        X, Y = np.meshgrid(np.arange(xa, xb + 1) + 0.5, np.arange(ya, yb + 1) + 0.5)
        l0 = ((y1 - y2) * (X - x2) + (x2 - x1) * (Y - y2)) / den
        l1 = ((y2 - y0) * (X - x2) + (x0 - x2) * (Y - y2)) / den
        l2 = 1 - l0 - l1
        ins = (l0 >= -1e-6) & (l1 >= -1e-6) & (l2 >= -1e-6)
        if not ins.any():
            continue
        yy, xx = np.nonzero(ins)
        face[ya + yy, xa + xx] = k
        bary[ya + yy, xa + xx] = np.column_stack([l0[ins], l1[ins], l2[ins]])
    mask = face >= 0
    f = face[mask]
    b = bary[mask]
    T = m["T"][f]
    P, N = m["P"], m["N"]
    pos = (P[T] * b[:, :, None]).sum(1) + m.get("CTR", 0.0)     # рисунок — в исходных осях формы (до центровки)
    nrm = normalize((N[T] * b[:, :, None]).sum(1))
    # касательные грани по UV
    p0, p1, p2 = P[m["T"][:, 0]], P[m["T"][:, 1]], P[m["T"][:, 2]]
    uv0, uv1, uv2 = m["UV"][:, 0], m["UV"][:, 1], m["UV"][:, 2]
    dp1, dp2 = p1 - p0, p2 - p0
    du1, dv1 = (uv1 - uv0)[:, 0], (uv1 - uv0)[:, 1]
    du2, dv2 = (uv2 - uv0)[:, 0], (uv2 - uv0)[:, 1]
    det = du1 * dv2 - du2 * dv1
    det = np.where(np.abs(det) < 1e-20, 1e-20, det)
    Tg = (dp1 * dv2[:, None] - dp2 * dv1[:, None]) / det[:, None]       # dP/du
    Bg = (dp2 * du1[:, None] - dp1 * du2[:, None]) / det[:, None]       # dP/dv
    J = np.stack([Tg, Bg], -1)                                          # (грани, 3, 2)
    JtJ = np.einsum("fij,fik->fjk", J, J)
    Jinv = np.linalg.pinv(JtJ) @ np.transpose(J, (0, 2, 1))             # (грани, 2, 3)
    Tt = Tg[f] - nrm * (Tg[f] * nrm).sum(1, keepdims=True)
    Bt = Bg[f] - nrm * (Bg[f] * nrm).sum(1, keepdims=True)
    uv = (m["UV"][f] * b[:, :, None]).sum(1)
    attr = {k: (v[T] * b).sum(1) for k, v in m["ATTR"].items()}
    ctx = Ctx(pos=pos, nrm=nrm, T=normalize(Tt), B=normalize(Bt), uv=uv, tag=m["TAG"][f], attr=attr,
              Jinv=Jinv[f], n=len(pos))
    return ctx, mask


def put(mask, vals, fill=0.0):
    img = np.full(mask.shape + ((vals.shape[1],) if vals.ndim == 2 else ()), fill, np.float64)
    img[mask] = vals
    return img


def dilate(img, mask, iters=10):
    img = img.copy()
    m = mask.copy()
    if img.ndim == 2:
        img = img[..., None]
    for _ in range(iters):
        acc = np.zeros_like(img)
        cnt = np.zeros(m.shape)
        for sh in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            mm = np.roll(m, sh, (0, 1))
            acc += np.roll(img, sh, (0, 1)) * mm[..., None]
            cnt += mm
        new = (~m) & (cnt > 0)
        img[new] = acc[new] / cnt[new][:, None]
        m = m | new
    if (~m).any():
        img[~m] = img[m].mean(0)
    return img


def normal_map(ctx, hfun, eps=1.5e-4):
    """Касательная нормаль (DirectX) из высоты hfun(ctx) в метрах."""
    hT = (hfun(ctx.shifted("T", eps)) - hfun(ctx.shifted("T", -eps))) / (2 * eps)
    hB = (hfun(ctx.shifted("B", eps)) - hfun(ctx.shifted("B", -eps))) / (2 * eps)
    n = normalize(np.column_stack([-hT, -hB, np.ones(ctx.n)]))
    return n * 0.5 + 0.5


def save_tex(name, img, png_dir, paa=True):
    path = os.path.join(png_dir, name + ".png")
    TL.write_png(path, TL.to_u8(img))
    if paa:
        TL.png_to_paa(path, os.path.join(OUT_DIR, name + ".paa"))
    return path


# ================================================================ рисунки форм
# --- крупицы
def sand_fields(ctx, need_color=True):
    p = ctx.pos
    warp = np.column_stack([gnoise(p * 900, 61), gnoise(p * 900 + 7.7, 62), gnoise(p * 900 + 3.1, 63)])
    f1, f2, pid = worley(p / 0.0007 + 0.45 * warp, seed=64)
    pit = sstep(0.42, 0.12, f1) * (pid > 0.72)
    mid = fbm(p * 700, 3, seed=65)
    fine = fbm(p * 2600, 2, seed=66)
    h = 0.00007 * mid + 0.000015 * fine - 0.00010 * pit
    F = {"h": h, "pit": pit, "mid": mid, "fine": fine}
    if need_color:
        F["big"] = fbm(p * 160, 3, seed=67)
    return F


def grain_cat(tag, res):
    return np.clip(np.array(GRAIN_CAT)[np.clip(tag, 0, GRAIN_N - 1)], 0, len(SAND_MIX[res]) - 1)


def sand_color(ctx, F, res):
    n = ctx.n
    mix = SAND_MIX[res]
    cols = np.array([c for c, _, _ in mix])
    cat = grain_cat(ctx.tag, res)
    base = cols[cat] * (0.92 + 0.16 * hash1(ctx.tag, 7))[:, None]
    cav = np.clip(ctx.attr["cav"], 0, 1)
    high = np.clip(ctx.attr["high"], 0, 1)
    tt = np.clip(0.55 + 0.25 * F["big"] + 0.25 * F["mid"] + 0.45 * high - 0.4 * cav, 0, 1)
    c = lerp(base * 0.55, base, sstep(0.0, 0.55, tt))
    c = lerp(c, np.clip(base * 1.25 + 0.08, 0, 1), sstep(0.6, 1.0, tt) * 0.7)
    c *= (0.94 + 0.06 * F["fine"])[:, None]
    dirt = np.clip(cav * 1.2 + F["pit"] * 0.8, 0, 1)
    if res in METALS:
        k = cat <= 1                                   # основные зёрна металла: ржавчина, патина, побежалость
        if k.any():
            c[k] = metal_tint(res, c[k], ctx.pos[k], dirt[k], {"fine": F["fine"][k]}, int(k.sum()))
    return lerp(c, col(PAL[res]["crev"], n), dirt * 0.55)


def sand_smdi(ctx, F):
    n = ctx.n
    spark = np.array([s for _, _, s in SAND_MIX["gold"]])          # блёстки одинаковы для всех ресурсов (_smdi общий)
    s = spark[grain_cat(ctx.tag, "gold")]
    dirt = np.clip(np.clip(ctx.attr["cav"], 0, 1) * 1.2 + F["pit"], 0, 1)
    g = (0.30 + 0.65 * s) * (1 - 0.75 * dirt)
    b = np.clip(0.30 + 0.45 * s - 0.4 * dirt + 0.1 * F["mid"] + 0.15 * np.clip(ctx.attr["high"], 0, 1), 0.05, 1)
    return np.column_stack([np.ones(n), g, b])


# --- самородок
def nugget_fields(ctx, need_color=True):
    p = ctx.pos
    # раковины неровной формы: клетки с искажённой границей, только часть клеток
    warp = np.column_stack([gnoise(p * 300, 35), gnoise(p * 300 + 7.7, 36), gnoise(p * 300 + 3.1, 37)])
    f1, f2, pid = worley(p / 0.0032 + 0.45 * warp, seed=31)
    pit = sstep(0.42, 0.12, f1) * (pid > 0.62)
    pit = np.maximum(pit, sstep(0.55, 0.85, -fbm(p * 160 + warp * 0.3, 3, seed=38)))
    mid = fbm(p * 230, 3, seed=33)
    fine = fbm(p * 900, 2, seed=32)
    h = 0.00017 * mid + 0.00004 * fine - 0.00028 * pit
    F = {"h": h, "pit": pit, "mid": mid, "fine": fine}
    if need_color:
        F["big"] = fbm(p * 55, 3, seed=34)
    return F


def metal_tint(res, c, p, dirt, F, n):
    """Особенности металлов: ржавчина у железа, патина у меди, побежалость у титана, потемнение серебра."""
    pal = PAL[res]
    if res == "iron":
        rm = sstep(0.15, 0.75, fbm(p * 70, 4, seed=51) + 0.9 * dirt)
        r2 = sstep(0.4, 1.0, fbm(p * 160, 3, seed=52))
        rust = lerp(col(pal["rust"], n), col(pal["rust2"], n), r2)
        c = lerp(c, rust * (0.85 + 0.25 * F["fine"])[:, None], rm * 0.85)
    elif res == "bronze":
        pm = sstep(0.55, 1.0, dirt + 0.3 * fbm(p * 90, 3, seed=53))
        c = lerp(c, col(pal["patina"], n), pm * 0.55)
        tm = sstep(0.0, 0.9, fbm(p * 40, 3, seed=54))
        c = lerp(c, c * np.array([0.72, 0.58, 0.52]), tm * 0.35)
    elif res == "titanium":
        tt = fbm(p * 30, 3, seed=55)
        c = lerp(c, col(pal["tint"], n), sstep(0.4, 1.1, tt) * 0.35)
        c = lerp(c, col(pal["straw"], n), sstep(0.5, 1.2, -tt) * 0.25)
    elif res == "silver":
        tm = sstep(0.2, 1.0, fbm(p * 45, 3, seed=56))
        c = lerp(c, c * np.array([0.80, 0.74, 0.62]), tm * 0.35)
    elif res == "gold":
        tm = sstep(0.3, 1.1, fbm(p * 45, 3, seed=57))
        c = lerp(c, c * np.array([0.92, 0.80, 0.62]), tm * 0.3)
    return c


def nugget_color(ctx, F, res):
    n = ctx.n
    pal = PAL[res]
    cav = np.clip(ctx.attr["cav"], 0, 1)
    high = np.clip(ctx.attr["high"], 0, 1)
    t = np.clip(0.55 + 0.25 * F["big"] + 0.25 * F["mid"] + 0.45 * high - 0.4 * cav, 0, 1)
    c = lerp(col(pal["lo"], n), col(pal["base"], n), sstep(0.0, 0.55, t))
    c = lerp(c, col(pal["hi"], n), sstep(0.6, 1.0, t) * 0.75)
    c *= (0.94 + 0.06 * F["fine"])[:, None]
    dirt = np.clip(cav * 1.25 + F["pit"] * 0.85, 0, 1)
    c = metal_tint(res, c, ctx.pos, dirt, F, n)
    c = lerp(c, col(pal["crev"], n), dirt * 0.85)
    return c


def nugget_smdi(ctx, F):
    n = ctx.n
    cav = np.clip(ctx.attr["cav"], 0, 1)
    dirt = np.clip(cav * 1.2 + F["pit"], 0, 1)
    g = 0.95 - 0.75 * dirt
    b = np.clip(0.72 - 0.6 * dirt + 0.12 * F["mid"] + 0.2 * np.clip(ctx.attr["high"], 0, 1), 0.05, 1)
    return np.column_stack([np.ones(n), g, b])


# --- кусок
def facet_edge_dist(ctx, polys):
    ed = np.zeros(ctx.n)
    for fi, V in enumerate(polys):
        k = ctx.tag == fi
        if k.any():
            ed[k] = poly_edge_dist(V, poly_normal(V), ctx.pos[k])
    return ed


def chunk_fields(ctx, need_color=True):
    p = ctx.pos
    n = ctx.n
    # раковистый излом: слабые обрывки дуг у точки удара (не кольца целиком)
    rip = np.zeros(n)
    wob = gnoise(p * 140, 60)
    arcs = sstep(0.05, 0.55, gnoise(p * 45 + 3.3, 59))
    for fi, (c, ph, sg) in enumerate(CHUNK_IMPACT):
        k = ctx.tag == fi
        if k.any():
            d = np.linalg.norm(p[k] - c, axis=1)
            rip[k] = np.sin(d / 0.0032 * 6.28 + ph + 1.2 * wob[k]) * np.exp(-d / 0.011) * arcs[k]
    rn = 1 - np.abs(gnoise(p * 110, 61))
    cmask = sstep(0.2, 0.7, fbm(p * 30, 2, seed=62))
    crack = sstep(0.972, 0.993, rn) * cmask
    grain = fbm(p * 1100, 2, seed=63)
    ed = ctx.attr.get("edge_px")
    chip = np.zeros(n)
    if ed is not None:
        # сколы — неровные выщербины вдоль рёбер (не круглые)
        jag = ed + 0.0011 * fbm(p * 380, 2, seed=64)
        chip = sstep(0.0020, 0.0006, jag) * sstep(0.15, 0.55, gnoise(p * 70, 58))
    h = 0.00003 * rip - 0.0001 * crack + 0.00002 * grain - 0.0003 * chip
    F = {"h": h, "rip": rip, "crack": crack, "grain": grain, "chip": chip}
    if need_color:
        F["m1"] = fbm(p * 60, 4, seed=65)
        F["m2"] = fbm(p * 18, 3, seed=66)
    return F


def chunk_color(ctx, F, res):
    n = ctx.n
    pal = PAL[res]
    p = ctx.pos
    ed = ctx.attr["edge_px"]
    near = np.exp(-ed / 0.0022)
    if res == "coal":
        axis = normalize(np.array([0.2, 0.35, 1.0]))
        # слоистость: блестящие полосы витрена и тусклые дюрена
        band = np.sin((p @ axis) / 0.0042 * 6.28 + 1.6 * F["m2"]) + 0.35 * np.sin((p @ axis) / 0.0011 * 6.28 + F["m1"])
        vit = sstep(0.2, 0.8, band)
        c = lerp(col(pal["lo"], n), col((0.085, 0.085, 0.09), n), vit)
        c = lerp(c, col(pal["brown"], n), sstep(0.3, 1.0, F["m1"]) * (1 - vit) * 0.45)
        c = lerp(c, col(pal["hi"], n), np.clip(near * 0.15 + F["chip"] * 0.3, 0, 1))
        c = lerp(c, col(pal["crev"], n), F["crack"] * 0.8)
        F["vit"] = vit
    elif res == "rubin":
        t = np.clip(0.5 + 0.35 * F["m1"] + 0.25 * F["m2"], 0, 1)
        c = lerp(col(pal["lo"], n), col(pal["base"], n), t)
        glow = near * (0.45 + 0.35 * sstep(-0.4, 0.6, F["m2"]))             # тонкие края пропускают свет
        c = lerp(c, col(pal["hi"], n), np.clip(glow, 0, 1))
        # «шёлк» (иглы рутила) — редкие короткие штрихи в трёх направлениях, только местами
        silk = np.zeros(n)
        for a in (0.0, 2.094, 4.189):
            d = normalize(np.array([math.cos(a), math.sin(a), 0.25]))
            silk = np.maximum(silk, sstep(0.93, 0.99, np.abs(np.sin((p @ d) / 0.0045 * 3.14 + 0.6 * F["m1"]))))
        silk *= sstep(0.45, 0.95, F["m2"])
        c = lerp(c, col(pal["pink"], n), np.clip(silk * 0.22 + F["crack"] * 0.55 + F["chip"] * 0.3, 0, 1))
    elif res == "nephrite":
        fib = fbm(p * np.array([140, 40, 60]), 3, seed=67)
        t = np.clip(0.5 + 0.3 * F["m1"] + 0.3 * F["m2"] + 0.15 * fib, 0, 1)
        c = lerp(col(pal["lo"], n), col(pal["base"], n), sstep(0.0, 0.6, t))
        c = lerp(c, col(pal["hi"], n), sstep(0.6, 1.0, t) * 0.7)
        f1, _, sid = worley(p / 0.004, seed=68)
        spots = sstep(0.35, 0.1, f1) * (sid > 0.7)
        c = lerp(c, col(pal["lo"], n) * 0.6, spots * 0.6)
        vein = sstep(0.972, 0.995, 1 - np.abs(gnoise(p * 40, 69))) * sstep(0.0, 0.6, gnoise(p * 12, 72))
        c = lerp(c, col(pal["cream"], n), vein * 0.4)
        rind = np.isin(ctx.tag, [2, 7, 11]).astype(float) * sstep(-0.3, 0.5, F["m2"])
        c = lerp(c, col((0.62, 0.55, 0.36), n), rind * 0.7)
        c = lerp(c, col(pal["hi"], n), near * 0.25)
        c = lerp(c, col(pal["cream"], n), F["chip"] * 0.25 + F["crack"] * 0.3)
    else:  # sulfur
        # рыхлая: зёрна-кристаллики, поры, бледная корка пятнами, бурые потёки
        f1, f2, gid = worley(p / 0.0008, seed=70)
        gr = sstep(0.0, 0.35, f2 - f1)
        t = np.clip(0.5 + 0.35 * F["m1"] + 0.35 * (gid - 0.5), 0, 1)
        c = lerp(col(pal["lo"], n), col(pal["base"], n), sstep(0.0, 0.55, t))
        c = lerp(c, col(pal["hi"], n), sstep(0.7, 1.0, t) * 0.8)
        c *= (0.62 + 0.38 * gr)[:, None]
        pf1, _, pid = worley(p / 0.0025, seed=73)
        pore = sstep(0.3, 0.1, pf1) * (pid > 0.55)
        c = lerp(c, col(pal["crev"], n) * 0.8, pore * 0.7)
        crust = sstep(0.45, 1.0, F["m2"])
        c = lerp(c, col((1.0, 0.95, 0.62), n), crust * 0.25)
        stain = sstep(0.4, 1.2, fbm(p * 50, 3, seed=71) + F["crack"])
        c = lerp(c, col(pal["stain"], n), stain * 0.45)
        F["pore"] = pore
        c = lerp(c, col(pal["crev"], n), F["crack"] * 0.6)
        c = lerp(c, col(pal["hi"], n), F["chip"] * 0.3)
    c *= (0.96 + 0.04 * F["grain"])[:, None]
    return np.clip(c, 0, 1)


def chunk_smdi(ctx, F):
    n = ctx.n
    g = np.clip(0.9 - 0.7 * F["crack"] - 0.4 * F["chip"], 0, 1)
    b = np.clip(0.85 - 0.6 * F["crack"] - 0.5 * F["chip"] + 0.05 * F["grain"], 0, 1)
    return np.column_stack([np.ones(n), g, b])


# --- слиток: клеймо (горное «молот и кайло» + знак элемента)
SYMBOL = {"gold": "Au", "silver": "Ag", "bronze": "Cu", "iron": "Fe", "titanium": "Ti"}


def _arc(cx, cy, rx, ry, a0, a1, k=14):
    t = np.radians(np.linspace(a0, a1, k))
    return np.column_stack([cx + rx * np.cos(t), cy + ry * np.sin(t)])


# штрихи букв в рамке высотой 1 (ширина ~0.6)
GLYPHS = {
    "A": [np.array([(0.0, 0.0), (0.32, 1.0), (0.64, 0.0)]), np.array([(0.12, 0.36), (0.52, 0.36)])],
    "C": [_arc(0.34, 0.5, 0.32, 0.5, 48, 312, 20)],
    "F": [np.array([(0.58, 1.0), (0.08, 1.0), (0.08, 0.0)]), np.array([(0.08, 0.52), (0.48, 0.52)])],
    "T": [np.array([(0.0, 1.0), (0.64, 1.0)]), np.array([(0.32, 1.0), (0.32, 0.0)])],
    "u": [np.vstack([[(0.06, 0.64)], _arc(0.27, 0.22, 0.21, 0.22, 180, 360, 12), [(0.48, 0.64)]]),
          np.array([(0.48, 0.64), (0.48, 0.0)])],
    "g": [_arc(0.26, 0.38, 0.21, 0.26, 0, 360, 20),
          np.vstack([[(0.47, 0.64)], [(0.47, -0.06)], _arc(0.26, -0.06, 0.21, 0.2, 0, -160, 10)])],
    "e": [np.vstack([[(0.05, 0.34), (0.47, 0.34)], _arc(0.26, 0.32, 0.21, 0.32, 0, 300, 16)])],
    "i": [np.array([(0.12, 0.0), (0.12, 0.64)]), np.array([(0.12, 0.86), (0.12, 0.90)])],
}
GLYPH_W = {"A": 0.64, "C": 0.66, "F": 0.58, "T": 0.64, "u": 0.54, "g": 0.50, "e": 0.50, "i": 0.24}


def emblem_strokes():
    """Горный знак: скрещённые молот и кайло, рамка 1 x 1."""
    thin = [np.array([(0.12, 0.08), (0.84, 0.86)]),                 # рукоять молота
            np.array([(0.88, 0.08), (0.18, 0.84)])]                 # рукоять кайла
    d = np.array([-0.7071, 0.7071])
    c = np.array([0.80, 0.82])
    thick = [np.array([c - d * 0.17, c + d * 0.17])]                # боёк молота
    pick = _arc(0.36, 0.56, 0.42, 0.42, 72, 168, 12)               # дуга кайла
    return thin + [pick], thick


def stamp_strokes(symbol):
    """Клеймо в осях верхней грани (м), по центру грани: тонкие штрихи, толстые (боёк молота)
    и полуразмеры рамки (hx, hy)."""
    thin, thick = [], []
    hgt = 0.026
    # горный знак слева
    e_thin, e_thick = emblem_strokes()
    ox, oy, sc = 0.0, -0.0145, 0.030
    thin += [np.column_stack([ox + s[:, 0] * sc, oy + s[:, 1] * sc]) for s in e_thin]
    thick += [np.column_stack([ox + s[:, 0] * sc, oy + s[:, 1] * sc]) for s in e_thick]
    # знак элемента справа
    x = sc + 0.012
    y0 = -0.0115
    for k, ch in enumerate(symbol):
        for s in GLYPHS[ch]:
            thin.append(np.column_stack([x + s[:, 0] * hgt, y0 + s[:, 1] * hgt]))
        x += (GLYPH_W[ch] + (0.22 if k + 1 < len(symbol) else 0.0)) * hgt
    shift = x / 2                                   # центр по X
    thin = [s - np.array([shift, 0.0]) for s in thin]
    thick = [s - np.array([shift, 0.0]) for s in thick]
    return thin, thick, (x / 2 + 0.008, 0.0215)


def seg_dist(q, strokes):
    best = np.full(len(q), 1e9)
    for s in strokes:
        for a, b in zip(s[:-1], s[1:]):
            ab = b - a
            t = np.clip(((q - a) @ ab) / max(ab @ ab, 1e-18), 0, 1)
            best = np.minimum(best, np.linalg.norm(q - (a + t[:, None] * ab), axis=1))
    return best


def ingot_fields(ctx, symbol, need_color=True):
    p = ctx.pos
    n = ctx.n
    tag = ctx.tag
    h = np.zeros(n)
    stamp = np.zeros(n)
    top = tag == 0
    side = tag == 1
    bot = tag == 2
    F = {}
    if top.any():
        q = p[top, :2]
        thin, thick, (hx, hy) = stamp_strokes(symbol)
        d1 = seg_dist(q, thin)
        d2 = seg_dist(q, thick)
        let = np.maximum(sstep(0.0019, 0.0011, d1), sstep(0.0034, 0.0026, d2))
        # рамка клейма
        bd = np.maximum(np.abs(q[:, 0]) - hx, np.abs(q[:, 1]) - hy)
        frame = sstep(0.0011, 0.0005, np.abs(bd))
        st = np.maximum(let, frame)
        stamp[top] = st
        ham = fbm(p[top] * 140, 3, seed=81)
        h[top] = -0.00055 * st + 0.00004 * ham + 0.00003 * fbm(p[top] * 700, 2, seed=82)
    if side.any():
        z = p[side, 2]
        flow = np.sin(z / 0.0045 * 6.28 + 3.0 * fbm(p[side] * 30, 3, seed=83))
        f1, _, pid = worley(p[side] / 0.0035, seed=84)
        pore = sstep(0.25, 0.0, f1) * (pid > 0.82)
        h[side] = 0.000035 * flow * sstep(0.0, 0.8, fbm(p[side] * 14, 2, seed=85)) - 0.00035 * pore \
            + 0.00003 * fbm(p[side] * 600, 2, seed=86)
        F["pore_side"] = (side, pore)
    if bot.any():
        f1, _, pid = worley(p[bot] / 0.003, seed=87)
        pore = sstep(0.3, 0.0, f1) * (pid > 0.6)
        h[bot] = 0.00012 * fbm(p[bot] * 300, 3, seed=88) - 0.0003 * pore
        F["pore_bot"] = (bot, pore)
    pore_all = np.zeros(n)
    for key in ("pore_side", "pore_bot"):
        if key in F:
            mk, v = F[key]
            pore_all[mk] = v
    F = {"h": h, "stamp": stamp, "pore": pore_all}
    if need_color:
        F["m1"] = fbm(p * 30, 4, seed=89)
        F["fine"] = fbm(p * 500, 2, seed=90)
    return F


def ingot_color(ctx, F, res):
    n = ctx.n
    pal = PAL[res]
    tag = ctx.tag
    edge = np.clip(ctx.attr["edge"], 0, 1)
    t = np.clip(0.62 + 0.22 * F["m1"] + 0.06 * F["fine"], 0, 1)
    c = lerp(col(pal["lo"], n), col(pal["base"], n), sstep(0.0, 0.6, t))
    c = lerp(c, col(pal["hi"], n), sstep(0.65, 1.0, t) * 0.5)
    c = c * np.where(tag == 1, 0.92, np.where(tag == 2, 0.78, 1.0))[:, None]
    c = lerp(c, col(pal["hi"], n), edge * 0.55)                      # стёртые рёбра светлее
    dirt = np.clip(F["stamp"] * 0.75 + F["pore"] * 0.9 + (tag == 2) * 0.25, 0, 1)
    c = metal_tint(res, c, ctx.pos, dirt, {"fine": F["fine"]}, n)
    c = lerp(c, col(pal["crev"], n), dirt * 0.8)
    return np.clip(c, 0, 1)


def ingot_smdi(ctx, F):
    n = ctx.n
    tag = ctx.tag
    edge = np.clip(ctx.attr["edge"], 0, 1)
    g = np.where(tag == 0, 0.92, np.where(tag == 1, 0.80, 0.45)) - 0.6 * F["stamp"] - 0.6 * F["pore"] + 0.08 * edge
    b = np.where(tag == 0, 0.78, np.where(tag == 1, 0.58, 0.25)) - 0.45 * F["stamp"] - 0.4 * F["pore"] + 0.2 * edge
    return np.column_stack([np.ones(n), np.clip(g, 0, 1), np.clip(b, 0, 1)])


# --- брикет: оттиск пресса сверху
def brq_face_kind(fi):
    nz = poly_normal(BRQ_POLYS[fi])
    if nz[2] > 0.95:
        return "top"
    if nz[2] < -0.95:
        return "bot"
    if abs(nz[2]) < 0.05 and (abs(nz[0]) > 0.95 or abs(nz[1]) > 0.95):
        return "side"
    return "chamfer"


BRQ_KIND = [brq_face_kind(i) for i in range(len(BRQ_POLYS))]


def brq_stamp(q):
    """Оттиск плиты пресса на верхней грани: углублённое поле со скруглёнными углами,
    в нём выпуклый ромб с точкой и по две планки по бокам. q — (x, y) в метрах. Возвращает высоту (м)."""
    A, B = BRQ_H[0] - BRQ_C - 0.004, BRQ_H[1] - BRQ_C - 0.004
    rc = 0.004
    dx = np.maximum(np.abs(q[:, 0]) - (A - rc), 0)
    dy = np.maximum(np.abs(q[:, 1]) - (B - rc), 0)
    sd = np.sqrt(dx ** 2 + dy ** 2) - rc + np.minimum(np.maximum(np.abs(q[:, 0]) - (A - rc), np.abs(q[:, 1]) - (B - rc)), 0)
    panel = sstep(0.0006, -0.0006, sd)
    rh = np.abs(q[:, 0]) / 0.016 + np.abs(q[:, 1]) / 0.011
    rhomb = sstep(0.16, 0.08, np.abs(rh - 1.0))
    dot = sstep(0.0028, 0.0020, np.linalg.norm(q, axis=1))
    bars = np.zeros(len(q))
    for sx in (-1, 1):
        for yy in (-0.006, 0.006):
            bx = np.abs(q[:, 0] - sx * 0.026) - 0.0055
            by = np.abs(q[:, 1] - yy) - 0.0011
            bars = np.maximum(bars, sstep(0.0005, -0.0003, np.maximum(bx, by)))
    raised = np.maximum(np.maximum(rhomb, dot), bars) * panel
    return -0.0007 * panel + 0.0006 * raised, panel, raised


def brq_fields(ctx, need_color=True):
    p = ctx.pos
    n = ctx.n
    kind = np.array(BRQ_KIND)[ctx.tag]
    f1, f2, gid = worley(p / 0.0009, seed=91)
    gr = sstep(0.0, 0.4, f2 - f1)
    h = 0.00011 * gr + 0.00005 * fbm(p * 450, 2, seed=92)
    side = kind == "side"
    layers = np.zeros(n)
    layers[side] = np.sin(p[side, 2] / 0.0017 * 6.28 + 1.5 * fbm(p[side] * 40, 2, seed=93))
    h += 0.00007 * layers
    panel = np.zeros(n)
    raised = np.zeros(n)
    top = kind == "top"
    if top.any():
        hs, pn, rs = brq_stamp(p[top, :2])
        h[top] += hs
        panel[top] = pn
        raised[top] = rs
    ed = ctx.attr.get("edge_px", np.full(n, 1.0))
    f1c, _, cid = worley(p / 0.0022, seed=94)
    chip = np.clip(sstep(0.0025, 0.0004, ed) * (kind != "chamfer") + (kind == "chamfer") * 0.6, 0, 1) \
        * (cid > 0.55) * sstep(0.55, 0.25, f1c)
    h -= 0.0004 * chip
    F = {"h": h, "gr": gr, "gid": gid, "layers": layers, "panel": panel, "raised": raised, "chip": chip,
         "kind": kind}
    if need_color:
        F["m1"] = fbm(p * 40, 3, seed=95)
    return F


def brq_color(ctx, F, res):
    n = ctx.n
    pal = PAL[res]
    mix = SAND_MIX[res]
    wts = np.cumsum([w for _, w, _ in mix])
    wts = wts / wts[-1]
    cat = np.searchsorted(wts, F["gid"], side="right").clip(0, len(mix) - 1)
    cols = np.array([c for c, _, _ in mix])
    jit = hash1((F["gid"] * 1e6).astype(np.int64), 6)
    grain = cols[cat] * (0.85 + 0.25 * jit)[:, None]
    binder = col(pal["lo"], n) * 0.75
    c = lerp(binder, grain, 0.35 + 0.65 * F["gr"])
    c = lerp(c, col(pal["base"], n), 0.25)
    c *= (0.92 + 0.08 * F["m1"])[:, None]
    c *= (1.0 + 0.06 * F["layers"])[:, None]
    kind = F["kind"]
    # поле оттиска гладкое (плотнее, темнее), выпуклый знак светлее; сколы — светлая крошка
    c = lerp(c, c * 0.82, F["panel"] * (1 - F["raised"]) * 0.6)
    c = lerp(c, col(pal["hi"], n) * 0.8 + c * 0.2, F["raised"] * 0.35)
    c = lerp(c, col(pal["hi"], n) * 0.7 + c * 0.3, F["chip"] * 0.6)
    c = c * np.where(kind == "bot", 0.85, 1.0)[:, None]
    return np.clip(c, 0, 1)


def brq_smdi(ctx, F):
    n = ctx.n
    g = np.clip(0.35 + 0.45 * F["gr"] + 0.25 * F["panel"] * (1 - F["raised"]) - 0.3 * F["chip"], 0, 1)
    b = np.clip(0.30 + 0.30 * F["gr"] + 0.30 * F["panel"] - 0.25 * F["chip"], 0, 1)
    return np.column_stack([np.ones(n), g, b])


# ================================================================ материалы
def rvmat_text(res, shape):
    spec, power, (fn_, fk), dif = MAT[res]
    if shape == "sand":
        k = SAND_MAT_K[res]
        spec = tuple(s * k for s in spec)
        power = max(12, int(power * 0.75))
    elif shape == "briquette":
        spec = tuple(s * (0.6 if res != "sulfur" else 1.0) for s in spec)
        power = max(10, int(power * 0.6))
    nohq = PREFIX + ("vb_mr_%s_ingot_nohq.paa" % res if shape == "ingot" else "vb_mr_%s_nohq.paa" % shape)
    smdi = PREFIX + "vb_mr_%s_smdi.paa" % shape
    s = "// VB_MiningResources: %s, %s (сгенерировано tools\\gen_mr_models.py)\n" % (shape, res)
    s += "ambient[] = {%.2f,%.2f,%.2f,1};\ndiffuse[] = {%.2f,%.2f,%.2f,1};\n" % ((dif,) * 6)
    s += "forcedDiffuse[] = {0,0,0,0};\nemmisive[] = {0,0,0,1};\n"
    s += "specular[] = {%.3f,%.3f,%.3f,1};\nspecularPower = %d;\n" % (spec[0], spec[1], spec[2], power)
    s += 'PixelShaderID = "Super";\nVertexShaderID = "Super";\n'
    s += ("class TexGen0\n{\n\tuvSource = \"tex\";\n\tclass uvTransform\n\t{\n\t\taside[] = {1,0,0};\n\t\tup[] = {0,1,0};\n"
          "\t\tdir[] = {0,0,1};\n\t\tpos[] = {0,0,0};\n\t};\n};\n")
    for i in range(1, 8):
        s += "class TexGen%d: TexGen0 {};\n" % i
    stages = [(1, nohq), (2, "#(argb,8,8,3)color(0.5,0.5,0.5,1,DT)"), (3, "#(argb,8,8,3)color(0,0,0,0,MC)"),
              (4, "#(argb,8,8,3)color(1,1,1,1,AS)"), (5, smdi), (6, "#(ai,64,64,1)fresnel(%.2f,%.2f)" % (fn_, fk)),
              (7, ENV)]
    for i, tx in stages:
        s += "class Stage%d\n{\n\ttexture = \"%s\";\n\ttexGen = \"%d\";\n};\n" % (i, tx, i)
    return s


def write_model_cfg():
    s = "// VB_MiningResources: сгенерировано tools\\gen_mr_models.py.\n"
    s += "// Секция camo — для hiddenSelections (текстура и материал ресурса из config.cpp).\n"
    s += "class CfgSkeletons\n{\n\tclass Default\n\t{\n\t\tisDiscrete = 1;\n\t\tskeletonInherit = \"\";\n\t\tskeletonBones[] = {};\n\t};\n};\n"
    s += "class CfgModels\n{\n\tclass Default\n\t{\n\t\tsectionsInherit = \"\";\n\t\tsections[] = {};\n\t\tskeletonName = \"\";\n\t};\n"
    s += "\tclass vb_mr_base: Default\n\t{\n\t\tsections[] = {\"camo\"};\n\t};\n"
    for shape in SHAPES:
        s += "\tclass vb_mr_%s: vb_mr_base {};\n" % shape
    s += "};\n"
    with open(os.path.join(OUT_DIR, "model.cfg"), "w", encoding="utf-8", newline="\r\n") as f:
        f.write(s)


def write_mapping():
    out = {}
    for shape, sd in SHAPES.items():
        for res in sd["res"]:
            cls = sd["cls"] % CLASS_RES[res]
            out[cls] = {"model": PREFIX + "vb_mr_%s.p3d" % shape,
                        "texture": PREFIX + "vb_mr_%s_%s_co.paa" % (res, shape),
                        "material": BS + PREFIX + "vb_mr_%s_%s.rvmat" % (res, shape),
                        "selection": "camo", "shape": shape, "resource": res}
    with open(os.path.join(OUT_DIR, "mapping.json"), "w", encoding="utf-8", newline="\n") as f:
        json.dump(out, f, ensure_ascii=False, indent=2)
    return out


# ================================================================ сборка
FIELDS = {"sand": sand_fields, "nugget": nugget_fields, "chunk": chunk_fields, "briquette": brq_fields}
COLORS = {"sand": sand_color, "nugget": nugget_color, "chunk": chunk_color, "ingot": ingot_color, "briquette": brq_color}
SMDIS = {"sand": sand_smdi, "nugget": nugget_smdi, "chunk": chunk_smdi, "ingot": ingot_smdi, "briquette": brq_smdi}


def prepare_ctx(shape, m, size):
    ctx, mask = bake(m, size)
    if shape == "chunk":
        ctx.attr["edge_px"] = facet_edge_dist(ctx, CHUNK_POLYS)
    elif shape == "briquette":
        ctx.attr["edge_px"] = facet_edge_dist(ctx, BRQ_POLYS)
    return ctx, mask


def make_textures(shape, m, png_dir, paa=True):
    t0 = time.time()
    res_list = SHAPES[shape]["res"]
    # _co на каждый ресурс
    ctx, mask = prepare_ctx(shape, m, CO_SIZE)
    if shape == "ingot":
        F_by = {r: ingot_fields(ctx, SYMBOL[r]) for r in res_list}
    else:
        F = FIELDS[shape](ctx)
    for r in res_list:
        Fr = F_by[r] if shape == "ingot" else F
        c = COLORS[shape](ctx, Fr, r)
        img = dilate(put(mask, np.clip(c, 0, 1)), mask)
        save_tex("vb_mr_%s_%s_co" % (r, shape), img, png_dir, paa)
    # _nohq и _smdi
    ctx, mask = prepare_ctx(shape, m, AUX_SIZE)
    if shape == "ingot":
        for r in res_list:
            nm = normal_map(ctx, lambda c, rr=r: ingot_fields(c, SYMBOL[rr], need_color=False)["h"])
            save_tex("vb_mr_%s_ingot_nohq" % r, dilate(put(mask, nm), mask), png_dir, paa)
        F = ingot_fields(ctx, SYMBOL["gold"], need_color=False)
    else:
        nm = normal_map(ctx, lambda c: FIELDS[shape](c, need_color=False)["h"])
        save_tex("vb_mr_%s_nohq" % shape, dilate(put(mask, nm), mask), png_dir, paa)
        F = FIELDS[shape](ctx, need_color=False)
    sm = SMDIS[shape](ctx, F)
    save_tex("vb_mr_%s_smdi" % shape, dilate(put(mask, np.clip(sm, 0, 1)), mask), png_dir, paa)
    for r in res_list:
        with open(os.path.join(OUT_DIR, "vb_mr_%s_%s.rvmat" % (r, shape)), "w", encoding="utf-8", newline="\r\n") as f:
            f.write(rvmat_text(r, shape))
    log("  текстуры %s: %d ресурсов, %.1f с" % (shape, len(res_list), time.time() - t0))


def build_shape(shape, png_dir, models=True, textures=True, paa=True):
    t0 = time.time()
    meshes = [BUILDERS[shape](0)]
    meshes.append(meshes[0])                       # LOD 1.0 = полная деталь (E79)
    meshes.append(BUILDERS[shape](1))
    if shape not in ("ingot",):
        ctr = (meshes[0]["P"].min(0) + meshes[0]["P"].max(0)) / 2
        if shape in PALM_SHAPES:
            # поза компаса: начало координат — точка хвата у запястья; модель поднята над ней и сдвинута к ладони
            low = meshes[0]["P"][:, 2].min() - ctr[2]
            ctr = ctr - np.array([-HAND_SHIFT_ENGINE[0], -HAND_SHIFT_ENGINE[1], HAND_LIFT - low])
        center_mesh(meshes[0], ctr)
        center_mesh(meshes[2], ctr)
    else:
        ctr = np.zeros(3)
    meshes[0]["CTR"] = ctr
    hull = hull_for(shape, meshes[0])
    if shape in ("chunk", "briquette"):
        hull = [V - ctr for V in hull]
    if models:
        findings = write_p3d(shape, meshes, hull, os.path.join(OUT_DIR, "vb_mr_%s.p3d" % shape))
        P = meshes[0]["P"]
        ext = P.max(0) - P.min(0)
        log("%-10s LOD0 %5d тр.  LOD4 %5d тр.  габарит %.1f x %.1f x %.1f см  масса %.2f кг  (%.1f с)" % (
            shape, len(meshes[0]["T"]), len(meshes[2]["T"]), ext[0] * 100, ext[1] * 100, ext[2] * 100,
            SHAPES[shape]["mass"], time.time() - t0))
        for f in findings:
            if "LOWCONF" not in f.code and "POS_CENTER" not in f.code:
                log("   validate:", f)
    if textures:
        make_textures(shape, meshes[0], png_dir, paa)


def main():
    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    only = [o for a in sys.argv if a.startswith("--only=") for o in a.split("=", 1)[1].split(",") if o]
    png_dir = args[0] if args else os.path.join(tempfile.gettempdir(), "vb_mr_png")
    os.makedirs(png_dir, exist_ok=True)
    os.makedirs(OUT_DIR, exist_ok=True)
    models = "--tex-only" not in sys.argv
    textures = "--models-only" not in sys.argv
    paa = "--no-paa" not in sys.argv
    for shape in SHAPES:
        if only and shape not in only:
            continue
        build_shape(shape, png_dir, models, textures, paa)
    write_model_cfg()
    write_mapping()
    log("готово:", OUT_DIR)


if __name__ == "__main__":
    main()
