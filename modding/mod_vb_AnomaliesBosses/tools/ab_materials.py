# -*- coding: utf-8 -*-
"""Материалы аномалий VB_AnomaliesBosses (ТЗ 2.6): 22 предмета VB_AB_Mat_<Аномалия>.

Что делает (всё повторяемо, зёрна фиксированы):
  - модели VB_AnomaliesBosses/i/mat/vb_ab_mat_<имя>.p3d (MLOD: 3 визуальных LOD, Geometry с массой
    и autocenter = 0, Memory, View Geometry, Fire Geometry) — как предметы Изнанки (mod_vb_Rift/tools/gen_rift_veins.py);
  - общий атлас 2048x1024 = 8x4 плиток по 256 (22 заняты, по плитке на аномалию):
    vb_ab_mat_co.paa (цвет), vb_ab_mat_nohq.paa (нормали), vb_ab_mat_e_co.paa (маска свечения, Stage8),
    vb_ab_mat_smdi.paa (блик/глянец, 1024x512);
  - один материал vb_ab_mat.rvmat на все модели: свечение маской в Stage8, как у ванильного Spooky_Goat_e
    и предметов Изнанки (проверено там в игре);
  - cfg/materials.hpp (классы CfgVehicles) и tools/strings/materials.json (строки для ab_strings.py).

Формы (6 на 22 материала): кристалл (друза на камне), комок, осколок, галька, мешочек с пылью, капсула.
У каждой аномалии своя плитка атласа: цвет по виду аномалии (ТЗ 2.1) и узор свечения по палитре (ТЗ 4.6):
огонь — тлеющие трещины, гроза — ветвящиеся молнии, лёд — иней и прожилки, гравитация — кольца и спирали,
кислота — наплывы и пузыри, пси — тонкие переливающиеся нити, радиация — пятна и язвы.

Плитка 256: по краю 8 px поля (повтор содержимого по горизонтали, край по вертикали), внутри 240x240:
  MAIN (строки 0..176) — основное вещество; по горизонтали бесшовное (u вокруг мешочка и капсулы);
  AUX (строки 180..240) — камень под кристаллами, металл капсулы, пыль и шнурок мешочка.

Геометрия строится в осях Blender (Z вверх, метры, грани против часовой снаружи), потом py3d.blender_to_dayz.
Начало координат — центр габаритов LOD0: в руке предмет сидит серединой (как предметы Изнанки).
Лежание на земле (E53): кристалл, комок, мешочек смоделированы стоя -> rotationFlags 2; осколок, галька
и капсула стоят «на ребре» (тонкая ось — Y Blender = Z DayZ) -> rotationFlags 17: на земле ложатся плашмя,
а в инвентаре видны широкой стороной.

Запуск (Python из Blender, в нём есть numpy):
  "G:\\SteamLibrary\\steamapps\\common\\Blender\\5.2\\python\\bin\\python.exe" ab_materials.py [--no-paa] [--only=Electra,Par]
    --no-paa  — не конвертировать PNG в PAA;  --only — собрать только эти модели (текстуры и конфиг всё равно пишутся).
  PNG атласа складываются в %TEMP%\\vb_ab_materials\\png.
Превью (листы PNG: день, ночь, вид как в инвентаре), после обычного запуска:
  "G:\\SteamLibrary\\steamapps\\common\\Blender\\blender.exe" -b --factory-startup --python ab_materials.py -- --preview [папка] [--lod=1]
  по умолчанию %TEMP%\\vb_ab_materials\\preview. Превью Blender — только форма и цвет, не то, как рисует DayZ.
"""
import json
import math
import os
import struct
import subprocess
import sys
import zlib

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, ".."))
MOD = os.path.join(ROOT, "VB_AnomaliesBosses")
OUT = os.path.join(MOD, "i", "mat")
CFG = os.path.join(MOD, "cfg", "materials.hpp")
STRINGS = os.path.join(HERE, "strings", "materials.json")
REPO = os.path.normpath(os.path.join(ROOT, "..", ".."))
TMP = os.path.join(os.environ.get("TEMP") or os.environ.get("TMP") or HERE, "vb_ab_materials")
PNG_DIR = os.path.join(TMP, "png")
PREVIEW_DIR = os.path.join(TMP, "preview")

BS = chr(92)
PREFIX = "VB_AB" + BS + "i" + BS + "mat" + BS
TEX_CO = PREFIX + "vb_ab_mat_co.paa"
RVMAT = PREFIX + "vb_ab_mat.rvmat"
IMAGE_TO_PAA = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools",
                            "Bin", "ImageToPAA", "ImageToPAA.exe")
PENETRATION = {
    "crystal": "glass", "shard": "glass", "lump": "granite", "pebble": "granite", "pouch": "cloth", "capsule": "iron",
}
EMISSIVE = 2.0          # emmisive[] в rvmat; яркость каждой аномалии задаёт маска свечения

# атлас
AW, AH = 2048, 1024
TS = 256                # плитка
PAD = 8                 # поле вокруг содержимого плитки
S = TS - 2 * PAD        # 240 — содержимое плитки
COLS = AW // TS
MAIN = (0.0, 0.0, 240.0, 176.0)     # (x0, y0, x1, y1) в пикселях содержимого
AUX = (0.0, 180.0, 240.0, 240.0)
AUX_L = (0.0, 180.0, 116.0, 240.0)
AUX_R = (124.0, 180.0, 240.0, 240.0)

LOD_RES = (0.0, 1.0, 4.0)           # как у предметов Изнанки
GEOM_K = 0.9                        # коллизия — ящик чуть меньше видимой модели

# лежание (CentralEconomy.c: RF_TOP = 2, RF_FRONT|RF_BACK = 17) и звук удара по форме
RF = {"crystal": 2, "lump": 2, "pouch": 2, "shard": 17, "pebble": 17, "capsule": 17}
SOUND = {"crystal": "glass", "shard": "glass", "lump": "default", "pebble": "default", "pouch": "textile",
         "capsule": "metal"}


def M(key, ru, en, dru, den, shape, pat, w, **kw):
    d = dict(key=key, ru=ru, en=en, dru=dru, den=den, shape=shape, pat=pat, w=w)
    d.update(kw)
    return d


# Порядок = порядок плиток в атласе. w — вес одной штуки, г. c0/c1 — тёмный/светлый тон вещества, glow — цвет
# свечения, gi — яркость свечения. Названия рабочие (ТЗ 2.6), описания — короткий лор без привязки к серверу.
MATS = [
    M("Electra", "Разрядный кварц", "Discharge Quartz",
      "Светлый кварц из сердца Электры. Тонкие прожилки в нём светятся, будто по ним до сих пор бежит ток.",
      "Pale quartz from the heart of an Electra. Its thin veins glow as if current were still running through them.",
      "crystal", "bolts", 90, base="crystal", c0=(0.52, 0.60, 0.72), c1=(0.86, 0.92, 0.98), glow=(0.40, 0.75, 1.0),
      gi=1.0, rock=(0.30, 0.29, 0.30), inner=0.06, form=dict(n=6, r=0.0075, L=0.050)),
    M("EMP", "Ионная пыль", "Ion Dust",
      "Тяжёлая синеватая пыль, осевшая после импульса. Пылинки липнут к пальцам и тихо потрескивают.",
      "Heavy bluish dust that settled after the pulse. The grains cling to your fingers and crackle softly.",
      "pouch", "dust", 25, cloth=(0.24, 0.26, 0.31), c0=(0.18, 0.18, 0.42), c1=(0.55, 0.55, 0.95),
      glow=(0.42, 0.42, 1.0), gi=0.9),
    M("Groza", "Грозовой кристалл", "Storm Crystal",
      "Тёмный кристалл, выросший там, где били молнии. В глубине пробегают голубые разряды, а ладонь от него покалывает.",
      "A dark crystal grown where lightning struck. Blue discharges run through its depths, and it makes your palm tingle.",
      "crystal", "bolts", 100, base="crystal", c0=(0.07, 0.10, 0.20), c1=(0.20, 0.28, 0.46), glow=(0.30, 0.65, 1.0),
      gi=1.0, rock=(0.17, 0.17, 0.19), bolts=10, inner=0.14, form=dict(n=5, r=0.0085, L=0.056)),
    M("MlechniPut", "Звёздный песок", "Star Sand",
      "Тёмный песок, в котором мерцают крошечные искры, как звёзды в ночном небе. Его собирают там, где Млечный путь "
      "рассыпал свои брызги.",
      "Dark sand with tiny sparks twinkling in it like stars in the night sky. It is gathered where the Milky Way "
      "scattered its sparks.",
      "pouch", "stars", 30, cloth=(0.13, 0.14, 0.22), c0=(0.02, 0.025, 0.08), c1=(0.08, 0.10, 0.26),
      glow=(0.75, 0.85, 1.0), gi=1.0),
    M("Zharka", "Окалина", "Fire Scale",
      "Хрупкая корка, сброшенная столбом огня. Под чешуйками тлеет раскалённая сердцевина, хотя на ощупь окалина "
      "едва тёплая.",
      "A brittle crust shed by a pillar of fire. A red-hot core smoulders under the flakes, yet the scale is barely "
      "warm to the touch.",
      "lump", "fire", 120, base="crust", c0=(0.07, 0.04, 0.03), c1=(0.25, 0.12, 0.05), glow=(1.0, 0.48, 0.07),
      gi=1.0, form=dict(rad=(0.028, 0.023, 0.016), amp=0.20, freq=1.6, terrace=0.07)),
    M("FireShtorm", "Пепельное стекло", "Ash Glass",
      "Чёрное стекло, сплавленное огненным вихрем из песка и пепла. В трещинах до сих пор тлеет жар.",
      "Black glass fused from sand and ash by a fiery whirlwind. Heat still smoulders in its cracks.",
      "shard", "fire", 50, base="glass", c0=(0.025, 0.022, 0.022), c1=(0.13, 0.12, 0.115), glow=(1.0, 0.30, 0.03),
      gi=1.0, form=dict(a=0.026, b=0.040, spikes=5, jag=0.45, tipmin=0.55, T=0.0045, e=0.0007)),
    M("FireIskra", "Искровой уголь", "Spark Coal",
      "Лёгкий чёрный уголь, внутри которого то и дело вспыхивают искры. Он не горит и не гаснет.",
      "A light black coal with sparks flaring up inside it now and then. It neither burns nor goes out.",
      "lump", "sparks", 90, base="coal", c0=(0.02, 0.02, 0.022), c1=(0.085, 0.08, 0.08), glow=(1.0, 0.70, 0.20),
      gi=1.0, form=dict(rad=(0.024, 0.021, 0.017), amp=0.28, freq=1.3, facet=True)),
    M("Par", "Горячий конденсат", "Hot Condensate",
      "Пар аномалии, осевший в запаянной капсуле. Внутри он не остывает и медленно клубится.",
      "Steam from the anomaly, condensed in a sealed capsule. It never cools inside and keeps slowly swirling.",
      "capsule", "vapor", 70, c0=(0.70, 0.42, 0.30), c1=(0.98, 0.82, 0.66), glow=(1.0, 0.55, 0.28), gi=0.75),
    M("Snow", "Вечный лёд", "Eternal Ice",
      "Кристалл льда из сердца метели. Он не тает ни у костра, ни за пазухой.",
      "An ice crystal from the heart of a blizzard. It melts neither by the fire nor inside your coat.",
      "crystal", "ice", 80, base="crystal", c0=(0.68, 0.79, 0.90), c1=(0.94, 0.97, 1.0), glow=(0.50, 0.85, 1.0),
      gi=0.55, rock=(0.42, 0.44, 0.48), inner=0.08, form=dict(n=8, r=0.0065, L=0.048)),
    M("Steklo", "Острый осколок", "Sharp Shard",
      "Осколок из роя стеклянных лезвий. Края острее бритвы, а внутри мерцают ледяные прожилки.",
      "A shard from a swarm of glass blades. Its edges are sharper than a razor, and icy veins shimmer inside.",
      "shard", "ice", 40, base="glass", c0=(0.30, 0.46, 0.56), c1=(0.68, 0.83, 0.92), glow=(0.45, 0.85, 1.0),
      gi=0.7, form=dict(a=0.016, b=0.044, spikes=5, jag=0.35, T=0.0035, e=0.0004)),
    M("Zastoy", "Застывший пузырь", "Frozen Bubble",
      "Пузырь застоявшегося воздуха, затвердевший от холода. Если поднести к уху, кажется, что время внутри стоит.",
      "A bubble of stagnant air that hardened in the cold. Hold it to your ear and time inside seems to stand still.",
      "pebble", "ice", 50, base="glass", c0=(0.55, 0.72, 0.84), c1=(0.88, 0.95, 1.0), glow=(0.40, 0.85, 1.0),
      gi=0.6, form=dict(rad=(0.021, 0.020, 0.021), amp=0.012, freq=1.2, uv="box")),
    M("Voronka", "Сжатый камень", "Compressed Stone",
      "Камень, сдавленный воронкой до размера ореха. Для своей величины он удивительно тяжёлый.",
      "A stone squeezed by a vortex to the size of a walnut. It is surprisingly heavy for its size.",
      "pebble", "spiral", 150, base="stone", c0=(0.04, 0.025, 0.06), c1=(0.15, 0.10, 0.19), glow=(0.62, 0.22, 1.0),
      gi=1.0, spiral=(2, 8.0), form=dict(rad=(0.019, 0.012, 0.017), amp=0.05, freq=1.8, uv="planar")),
    M("Lomka", "Скрученный камень", "Twisted Stone",
      "Камень, закрученный вихрем, как мокрая тряпка. Вдоль витков светятся фиолетовые полосы.",
      "A stone wrung by the whirlwind like a wet rag. Violet streaks glow along its twists.",
      "lump", "spiral", 130, base="stone", c0=(0.18, 0.16, 0.155), c1=(0.42, 0.38, 0.35), glow=(0.68, 0.30, 1.0),
      gi=0.9, spiral=(3, 11.0), form=dict(rad=(0.034, 0.019, 0.0095), amp=0.10, freq=1.5, twist=3.0)),
    M("Tramplin", "Гравитационная галька", "Gravity Pebble",
      "Гладкая плоская галька, отшлифованная бесконечными подбрасываниями. По ней расходятся светящиеся кольца, "
      "как круги по воде.",
      "A smooth flat pebble polished by endless tossing. Glowing rings spread across it like ripples on water.",
      "pebble", "rings", 110, base="stone", c0=(0.20, 0.17, 0.25), c1=(0.42, 0.38, 0.48), glow=(0.72, 0.38, 1.0),
      gi=0.9, spiral=(0, 12.0), form=dict(rad=(0.026, 0.0085, 0.021), amp=0.02, freq=1.2, uv="planar")),
    M("Roy", "Хитиновая пыль", "Chitin Dust",
      "Мелкая шелуха с крыльев и панцирей роя. Иногда в ней вспыхивают зелёные искорки.",
      "Fine husks from the wings and shells of the swarm. Green sparks flash in it now and then.",
      "pouch", "chitin", 20, cloth=(0.24, 0.21, 0.12), c0=(0.22, 0.15, 0.06), c1=(0.52, 0.38, 0.15),
      glow=(0.48, 1.0, 0.12), gi=0.85),
    M("Studen", "Студенистая слизь", "Jelly Slime",
      "Кислотный студень, запаянный в капсулу. Медленно колышется и, говорят, разъедает всё, кроме стекла.",
      "Acidic jelly sealed in a capsule. It sways slowly and is said to eat through everything but glass.",
      "capsule", "jelly", 70, c0=(0.12, 0.36, 0.04), c1=(0.42, 0.82, 0.12), glow=(0.42, 1.0, 0.08), gi=0.85),
    M("Holera", "Ядовитая плесень", "Poison Mould",
      "Комок плесени с края ядовитой лужи. Он пузырится и светится, нюхать его не стоит.",
      "A lump of mould from the edge of a poisonous pool. It bubbles and glows; better not sniff it.",
      "lump", "acid", 60, base="mould", c0=(0.08, 0.11, 0.06), c1=(0.24, 0.30, 0.12), glow=(0.58, 1.0, 0.10),
      gi=0.85, sound="organic", form=dict(rad=(0.026, 0.022, 0.015), amp=0.12, freq=2.2, blobs=14)),
    M("Flash", "Световая пыль", "Light Dust",
      "Пыль, оставшаяся от погасшей вспышки. Светится ровным перламутровым светом и переливается, если пересыпать её "
      "с ладони на ладонь.",
      "Dust left over from a faded flash. It glows with an even pearly light and shimmers when poured from palm "
      "to palm.",
      "pouch", "pearl", 20, cloth=(0.66, 0.63, 0.58), c0=(0.78, 0.76, 0.74), c1=(1.0, 0.98, 0.95),
      glow=(1.0, 0.95, 0.88), gi=0.8),
    M("LinceAgr", "Линзовое стекло", "Lens Glass",
      "Круглое стекло из Злой линзы. Сквозь него мир плывёт и кривится, а в глубине бегут тонкие розовые нити.",
      "A round glass from an Evil Lens. Through it the world swims and warps, and thin pink threads run in its depths.",
      "shard", "threads", 60, base="pearl", c0=(0.58, 0.54, 0.62), c1=(0.90, 0.86, 0.92), glow=(1.0, 0.45, 0.75),
      gi=0.9, pal=((1.0, 0.35, 0.65), (0.85, 0.45, 1.0), (1.0, 0.75, 0.9)),
      form=dict(lens=True, a=0.026, b=0.026, T=0.0055, e=0.001)),
    M("Lince", "Пустое стекло", "Empty Glass",
      "Мутное стекло, похожее на линзовое, но внутри почти пустое. Безобидная Линза только такое и оставляет.",
      "Cloudy glass that looks like lens glass but is almost empty inside. A harmless Lens leaves nothing better.",
      "shard", "threads", 50, base="pearl", c0=(0.48, 0.50, 0.53), c1=(0.76, 0.78, 0.81), glow=(0.75, 0.82, 0.9),
      gi=0.3, pal=((0.8, 0.85, 0.95), (0.9, 0.85, 0.95), (0.85, 0.95, 0.95)),
      form=dict(a=0.024, b=0.031, spikes=7, jag=0.22, T=0.004, e=0.0008)),
    M("DeadZone", "Чёрный пепел", "Black Ash",
      "Жирный чёрный пепел из мёртвой зоны. В нём тлеют редкие жёлтые искры, пахнет гарью и железом.",
      "Greasy black ash from a dead zone. Rare yellow sparks smoulder in it, and it smells of burning and iron.",
      "pouch", "embers", 25, cloth=(0.12, 0.12, 0.11), c0=(0.015, 0.015, 0.015), c1=(0.07, 0.07, 0.065),
      glow=(0.72, 0.95, 0.12), gi=0.9),
    M("Atom", "Светящийся шлак", "Glowing Slag",
      "Оплавленный шлак из эпицентра Атома, весь в светящихся язвах. Тёплый, тяжёлый и тихо потрескивает.",
      "Fused slag from the epicentre of an Atom, covered in glowing sores. Warm, heavy and softly crackling.",
      "lump", "sores", 140, base="slag", c0=(0.08, 0.09, 0.06), c1=(0.24, 0.26, 0.16), glow=(0.76, 1.0, 0.10),
      gi=1.0, form=dict(rad=(0.027, 0.024, 0.018), amp=0.14, freq=2.0, pits=9)),
]
for _i, _m in enumerate(MATS):
    _m["idx"] = _i
    _m["seed"] = 501 + _i * 11


# ================================================================ шум
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


def unit(v):
    v = np.asarray(v, float)
    return v / max(float(np.linalg.norm(v)), 1e-12)


# ================================================================ текстуры (плитка 240x240, бесшовная)
YY, XX = np.mgrid[0:S, 0:S].astype(float)


def sn(rng, beta=None, f0=None, bw=None, fmin=1.0, aniso=1.0):
    """Спектральный шум плитки (период = плитка): beta — степенной спектр, f0/bw — полоса (сетка линий)."""
    w = rng.standard_normal((S, S))
    F = np.fft.fft2(w)
    f1 = np.fft.fftfreq(S) * S
    f = np.hypot(f1[None, :], f1[:, None] * aniso)
    if beta is not None:
        filt = np.where(f >= fmin, np.maximum(f, 1e-9) ** (-beta), 0.0)
    else:
        filt = np.exp(-((f - f0) / bw) ** 2)
        filt[0, 0] = 0.0
    out = np.real(np.fft.ifft2(F * filt))
    return out / (out.std() + 1e-12)


def blur(img, sigma):
    f1 = np.fft.fftfreq(S)
    g = np.exp(-2 * (np.pi * sigma) ** 2 * (f1[:, None] ** 2 + f1[None, :] ** 2))
    return np.real(np.fft.ifft2(np.fft.fft2(img) * g))


def wrapd(a):
    return (a + S / 2) % S - S / 2


def dots(rng, n, smin, smax, amin=0.5, amax=1.0):
    """Россыпь светящихся точек (искры, звёзды), с переносом через край плитки."""
    img = np.zeros((S, S))
    for _ in range(n):
        x, y = rng.uniform(0, S, 2)
        s = rng.uniform(smin, smax)
        a = rng.uniform(amin, amax)
        d2 = wrapd(XX - x) ** 2 + wrapd(YY - y) ** 2
        img = np.maximum(img, a * np.exp(-d2 / (s * s)))
    return img


def stamp(img, x, y, w, val):
    r = int(math.ceil(w * 2.5)) + 1
    xs = np.arange(int(math.floor(x)) - r, int(math.floor(x)) + r + 2)
    ys = np.arange(int(math.floor(y)) - r, int(math.floor(y)) + r + 2)
    gx = np.exp(-((xs - x) / w) ** 2)
    gy = np.exp(-((ys - y) / w) ** 2)
    ix, iy = xs % S, ys % S
    sub = img[np.ix_(iy, ix)]
    img[np.ix_(iy, ix)] = np.maximum(sub, np.outer(gy, gx) * val)


def bolt(img, rng, x, y, ang, length, w0, depth, val=1.0):
    """Молния: ломаная со случайными отклонениями и ответвлениями."""
    base = ang
    steps = max(2, int(length / 2.0))
    for i in range(steps):
        t = i / steps
        w = max(0.45, w0 * (1 - 0.6 * t))
        nx, ny = x + 2 * math.cos(ang), y + 2 * math.sin(ang)
        stamp(img, x, y, w, val)
        stamp(img, (x + nx) / 2, (y + ny) / 2, w, val)
        x, y = nx, ny
        ang += rng.normal(0, 0.45)
        ang = 0.8 * ang + 0.2 * base
        if depth < 2 and rng.random() < 0.05:
            bolt(img, rng, x, y, ang + rng.choice((-1, 1)) * rng.uniform(0.5, 1.1),
                 length * (1 - t) * rng.uniform(0.3, 0.6), w * 0.7, depth + 1, val * 0.8)


def base_field(rng, kind, c0, c1):
    """Вещество без свечения: цвет, высота (для нормалей), блик и глянец."""
    if kind == "glass":            # стекло: плавные разводы
        t = 0.5 + 0.5 * np.tanh(sn(rng, beta=2.2) * 0.9)
        h = 0.08 * sn(rng, beta=2.4)
        sp, gl = 0.85, 0.92
    elif kind == "crystal":        # кристалл: облачка внутри и полосы роста
        t = 0.5 + 0.5 * np.tanh(sn(rng, beta=2.0) * 0.8)
        streak = 0.5 + 0.5 * np.sin(YY * 2 * math.pi / 9 + sn(rng, beta=2.0) * 1.5)
        t = 0.75 * t + 0.25 * streak
        h = 0.12 * streak
        sp, gl = 0.8, 0.88
    elif kind == "stone":          # камень: пятна и крапины
        t = 0.5 + 0.5 * np.tanh(sn(rng, beta=1.4) * 0.9)
        speck = np.clip((sn(rng, beta=0.2) - 1.8) * 1.5, 0, 1)
        t = t * (1 - 0.5 * speck)
        h = 0.6 * sn(rng, beta=1.3) + 0.15 * sn(rng, beta=0.5)
        sp, gl = 0.12, 0.25
    elif kind == "crust":          # окалина: чешуйки-уступы
        n = sn(rng, beta=1.6)
        terr = np.floor(n * 2.5) / 2.5
        hi = sn(rng, beta=0.6)
        t = 0.5 + 0.5 * np.tanh(terr * 0.7 + 0.25 * hi)
        h = 0.9 * terr + 0.2 * hi
        sp, gl = 0.15, 0.25
    elif kind == "coal":           # уголь: чёрный, с блеском
        t = 0.5 + 0.5 * np.tanh(sn(rng, beta=1.8) * 0.8)
        h = 0.3 * sn(rng, beta=1.6) + 0.05 * sn(rng, beta=0.4)
        sp, gl = 0.45, 0.6
    elif kind == "mould":          # плесень: пушистая, матовая
        fuzz = sn(rng, beta=0.3)
        t = 0.5 + 0.5 * np.tanh(sn(rng, beta=1.5) * 0.9 + 0.35 * fuzz)
        h = 0.5 * sn(rng, beta=1.4) + 0.25 * fuzz
        sp, gl = 0.08, 0.15
    elif kind == "slag":           # шлак: пузырчатый, со стеклянным блеском
        t = 0.5 + 0.5 * np.tanh(sn(rng, beta=1.7) * 0.9)
        pits = smoothstep(1.6, 2.4, sn(rng, beta=0.9))
        t = t * (1 - 0.5 * pits)
        h = 0.5 * sn(rng, beta=1.5) - 0.9 * pits
        sp, gl = 0.4, 0.55
    elif kind == "pearl":          # перламутр: мягкие молочные разводы
        t = 0.5 + 0.5 * np.tanh(sn(rng, beta=2.3) * 0.8)
        h = 0.06 * sn(rng, beta=2.2)
        sp, gl = 0.65, 0.85
    else:
        raise ValueError(kind)
    co = c0[None, None, :] * (1 - t)[..., None] + c1[None, None, :] * t[..., None]
    return co, h, np.full((S, S), sp), np.full((S, S), gl)


def compose(co, h, sp, gl, glow, g, halo, gi):
    """Наложить светящийся узор: g — маска узора, halo — ореол; в цвете узор светлый, в маске свечения — цвет glow."""
    glow = np.asarray(glow, float)
    if glow.ndim == 1:
        glow = glow[None, None, :]
    sp = np.broadcast_to(np.asarray(sp, float), (S, S))
    gl = np.broadcast_to(np.asarray(gl, float), (S, S))
    g3, h3 = g[..., None], halo[..., None]
    co = co * (1 - 0.5 * h3) + glow * 0.35 * h3
    co = co * (1 - g3) + (glow * 0.8 + 0.2 * g3 ** 3) * g3
    em = glow * (g3 + 0.15 * h3) * gi
    return dict(co=np.clip(co, 0, 1), em=np.clip(em, 0, 1), h=h,
                sp=np.clip(sp * (1 - 0.4 * g), 0, 1), gl=np.clip(gl * (1 - 0.3 * g), 0, 1))


# ---- узоры свечения по палитрам ТЗ 4.6
def pat_fire(rng):
    """Огонь: тлеющие трещины, как угли."""
    n1 = sn(rng, f0=4.5, bw=1.8)
    n2 = sn(rng, f0=10.0, bw=3.0)
    m1 = smoothstep(-0.8, 0.3, sn(rng, beta=1.6))
    m2 = smoothstep(-0.2, 0.8, sn(rng, beta=1.4))
    w = 0.10 + 0.08 * smoothstep(-1, 1, sn(rng, beta=1.5))
    g = np.clip(np.exp(-(n1 / w) ** 2) * m1 + 0.55 * np.exp(-(n2 / 0.10) ** 2) * m2, 0, 1)
    return dict(g=g, halo=np.clip(blur(g, 4.0) * 2.0, 0, 1), h=-0.8 * blur(g, 1.0))


def pat_sparks(rng):
    """Огненная искра: редкие трещины и вспыхивающие искры."""
    f = pat_fire(rng)
    g = f["g"] * 0.35
    d = dots(rng, 70, 0.7, 2.0)
    halo = np.clip(blur(d, 3.0) * 3.0 + f["halo"] * 0.3, 0, 1)
    return dict(g=np.clip(g + d, 0, 1), halo=halo, h=-0.5 * blur(g, 1.0))


def pat_bolts(rng, n=6):
    """Гроза: ветвящиеся молнии (сверху вниз)."""
    img = np.zeros((S, S))
    for _ in range(n):
        bolt(img, rng, rng.uniform(0, S), rng.uniform(-10, 40), math.pi / 2 + rng.uniform(-0.5, 0.5),
             rng.uniform(140, 230), rng.uniform(1.2, 1.8), 0)
    halo = np.clip(blur(img, 3.5) * 1.8 + blur(img, 9.0) * 0.8, 0, 1)
    return dict(g=np.clip(img, 0, 1), halo=halo, h=np.zeros((S, S)))


def pat_ice(rng):
    """Лёд: иней и ледяные прожилки."""
    fine = sn(rng, f0=16, bw=5)
    veins = sn(rng, f0=3.5, bw=1.2)
    mask = smoothstep(-0.3, 0.9, sn(rng, beta=1.5))
    frost = np.exp(-(fine / 0.13) ** 2) * mask * 0.5
    v = np.exp(-(veins / 0.06) ** 2)
    g = np.clip(frost + v, 0, 1)
    return dict(g=g, halo=np.clip(blur(g, 3.0) * 1.4, 0, 1), h=0.25 * frost - 0.25 * v)


def pat_spiral(rng, arms, lam):
    """Гравитация: спираль (arms рукавов) или кольца (arms = 0) вокруг центра MAIN — центра предмета в проекции."""
    cx, cy = (MAIN[0] + MAIN[2]) / 2, (MAIN[1] + MAIN[3]) / 2
    dx, dy = XX - cx, YY - cy
    r = np.hypot(dx, dy)
    th = np.arctan2(dy, dx)
    warp = sn(rng, beta=2.2) * 2.5
    ph = 2 * math.pi * (r + warp) / lam + arms * th
    g = smoothstep(0.78, 0.97, np.cos(ph)) * smoothstep(3, 10, r) * (1 - 0.6 * smoothstep(80, 120, r))
    g = np.clip(g + 0.6 * np.exp(-(r / 4.0) ** 2), 0, 1)
    return dict(g=g, halo=np.clip(blur(g, 3.0) * 1.6, 0, 1), h=-0.4 * g)


def pat_acid(rng):
    """Кислота: наплывы, подтёки и пузыри."""
    b = smoothstep(0.9, 1.6, sn(rng, beta=1.8))
    d = smoothstep(1.1, 1.9, sn(rng, beta=1.6, aniso=4.0))
    bub = np.zeros((S, S))
    dome = np.zeros((S, S))
    for _ in range(26):
        x, y = rng.uniform(0, S, 2)
        rr = rng.uniform(3, 11)
        dist = np.hypot(wrapd(XX - x), wrapd(YY - y))
        bub = np.maximum(bub, np.exp(-((dist - rr) / 1.1) ** 2) + 0.25 * smoothstep(rr, 0.3 * rr, dist))
        dome = np.maximum(dome, np.clip(1 - (dist / rr) ** 2, 0, 1))
    g = np.clip(0.5 * b + 0.55 * d + 0.8 * bub, 0, 1)
    return dict(g=g, halo=np.clip(blur(g, 3.0) * 1.2, 0, 1), h=0.5 * b + 0.8 * np.sqrt(dome))


def pat_threads(rng, pal):
    """Пси: тонкие переливающиеся нити (цвет нити плавно меняется по палитре)."""
    t = np.zeros((S, S))
    for k in range(6):
        n = sn(rng, f0=2.5 + 0.9 * k, bw=1.0)
        t = np.maximum(t, np.exp(-(n / 0.035) ** 2) * rng.uniform(0.55, 1.0))
    hue = 0.5 + 0.5 * np.tanh(sn(rng, beta=2.6) * 1.2)
    pal = np.asarray(pal, float)
    kk = hue * (len(pal) - 1)
    i0 = np.clip(np.floor(kk).astype(int), 0, len(pal) - 2)
    fr = (kk - i0)[..., None]
    col = pal[i0] * (1 - fr) + pal[i0 + 1] * fr
    return dict(g=t, halo=np.clip(blur(t, 2.5) * 1.3, 0, 1), h=np.zeros((S, S)), glow=col)


def pat_sores(rng):
    """Радиация: пятна и язвы со светящейся сердцевиной."""
    n = sn(rng, beta=1.4)                         # много язв среднего размера, а не пара огромных
    spots = smoothstep(0.75, 1.3, n)
    core = smoothstep(1.25, 1.95, n)
    p = smoothstep(2.0, 2.6, sn(rng, beta=0.6))
    g = np.clip(core + 0.3 * spots + 0.5 * p, 0, 1)
    return dict(g=g, halo=np.clip(blur(g, 3.0) * 1.5, 0, 1), h=-0.8 * spots - 0.4 * p,
                dark=np.clip(spots - core, 0, 1))


def pattern(rng, m):
    p = m["pat"]
    if p == "fire":
        return pat_fire(rng)
    if p == "sparks":
        return pat_sparks(rng)
    if p == "bolts":
        return pat_bolts(rng, m.get("bolts", 6))
    if p == "ice":
        return pat_ice(rng)
    if p in ("spiral", "rings"):
        return pat_spiral(rng, *m["spiral"])
    if p == "acid":
        return pat_acid(rng)
    if p == "threads":
        return pat_threads(rng, m["pal"])
    if p == "sores":
        return pat_sores(rng)
    raise ValueError(p)


def tex_substance(rng, m):
    co, h, sp, gl = base_field(rng, m["base"], np.asarray(m["c0"], float), np.asarray(m["c1"], float))
    p = pattern(rng, m)
    if p.get("dark") is not None:
        co = co * (1 - 0.6 * p["dark"])[..., None]
    glow = p.get("glow")
    glow = np.asarray(m["glow"], float) if glow is None else glow
    g = p["g"]
    if m.get("inner"):                            # кристалл слабо светится весь, ярче к облачкам внутри
        g = np.clip(g + m["inner"] * (0.5 + 0.5 * np.tanh(sn(rng, beta=2.0))), 0, 1)
    return compose(co, h + p["h"], sp, gl, glow, g, p["halo"], m["gi"])


def tex_cloth(rng, m):
    """Мешковина в цвет аномалии; сквозь плетение проступает светящаяся пыль: пятнами (ионная, световая)
    или россыпью искр (звёздный песок, хитин, пепел)."""
    f = 60                                      # ниток на ширину плитки — целое, чтобы шов вокруг мешочка не был виден
    nx = sn(rng, beta=2.6)
    tx = 0.5 + 0.5 * np.sin(XX * 2 * math.pi * f / S + nx * 0.5)
    ty = 0.5 + 0.5 * np.sin(YY * 2 * math.pi * f / S + nx * 0.5)
    coarse = np.where((np.floor(XX * f / S) + np.floor(YY * f / S)) % 2 == 0, tx, ty)
    stain = sn(rng, beta=2.0)
    co = np.asarray(m["cloth"], float)[None, None, :] * (0.74 + 0.16 * coarse + 0.08 * stain)[..., None]
    h = 0.35 * coarse + 0.1 * stain
    if m["pat"] in ("dust", "pearl"):
        spots = np.clip((sn(rng, beta=1.6) - 1.5) * 2.0, 0, 1) + np.clip((sn(rng, beta=0.6) - 2.0) * 3.0, 0, 1)
        k = 1.0 if m["pat"] == "pearl" else 0.85
    else:
        spots = dots(rng, 150, 0.45, 1.1, 0.4, 1.0)
        k = 0.9
    spots = np.clip(spots * (0.6 + 0.4 * coarse), 0, 1)
    halo = np.clip(blur(spots, 2.5) * 1.2, 0, 1)
    return compose(co, h, 0.04, 0.1, m["glow"], spots * k, halo * 0.6, m["gi"] * k)


# пыль в горловине мешочка: (общее свечение, число искр, размер искр мин/макс)
DUST = {"dust": (0.45, 90, 0.6, 1.4), "stars": (0.05, 70, 0.5, 1.7), "chitin": (0.10, 45, 0.6, 1.6),
        "pearl": (0.55, 110, 0.5, 1.3), "embers": (0.02, 22, 0.6, 1.4)}


def tex_dust(rng, m):
    level, n, smin, smax = DUST[m["pat"]]
    grain = sn(rng, beta=0.4)
    low = sn(rng, beta=1.5)
    t = 0.5 + 0.5 * np.tanh(0.7 * grain + 0.6 * low)
    c0, c1 = np.asarray(m["c0"], float), np.asarray(m["c1"], float)
    co = c0[None, None, :] * (1 - t)[..., None] + c1[None, None, :] * t[..., None]
    sparks = dots(rng, n, smin, smax)
    g = np.clip(level * (0.6 + 0.4 * t) + sparks, 0, 1)
    return compose(co, 0.5 * grain, 0.1, 0.2, m["glow"], g, np.clip(blur(sparks, 2.0) * 1.5, 0, 1), m["gi"])


def tex_cord(rng):
    """Шнурок: скрученные волокна (u — вдоль шнурка, v — вокруг)."""
    st = 0.5 + 0.5 * np.sin(2 * math.pi * (XX / 7.0 + YY / 5.0))
    co = np.array([0.40, 0.31, 0.19])[None, None, :] * (0.62 + 0.32 * st + 0.06 * sn(rng, beta=1.0))[..., None]
    z = np.zeros((S, S))
    return compose(co, 0.6 * st, 0.05, 0.12, (0.0, 0.0, 0.0), z, z, 0.0)


def tex_metal(rng):
    """Металл крышек капсулы: шлифовка вокруг оси (u — вокруг, v — вдоль профиля)."""
    s1 = rng.standard_normal(S)
    s1 = np.real(np.fft.ifft(np.fft.fft(s1) * np.exp(-(np.fft.fftfreq(S) * S / 30.0) ** 2)))
    s1 = s1 / (s1.std() + 1e-12)
    streak = np.repeat(s1[:, None], S, axis=1)
    wear = sn(rng, beta=1.8)
    co = np.array([0.43, 0.44, 0.46])[None, None, :] * (0.85 + 0.06 * streak + 0.08 * wear)[..., None]
    z = np.zeros((S, S))
    return compose(co, 0.15 * streak, 0.7, 0.72, (0.0, 0.0, 0.0), z, z, 0.0)


def tex_rock(rng, m):
    """Камень, на котором растут кристаллы, с редкими светящимися зёрнами."""
    rock = np.asarray(m["rock"], float)
    co, h, sp, gl = base_field(rng, "stone", rock * 0.6, rock * 1.35)
    seeds = dots(rng, 30, 0.6, 1.4) * 0.6
    return compose(co, h, sp, gl, m["glow"], seeds, np.clip(blur(seeds, 2.0) * 1.2, 0, 1), m["gi"] * 0.7)


def tex_liquid(rng, m):
    """Содержимое капсулы: клубящийся пар или кислотный студень с пузырями."""
    n = sn(rng, beta=2.2)
    n2 = sn(rng, beta=1.6)
    swirl = 0.5 + 0.5 * np.sin(n * 2.2 + YY * 2 * math.pi / 60 + 0.6 * n2)
    c0, c1 = np.asarray(m["c0"], float), np.asarray(m["c1"], float)
    co = c0[None, None, :] * (1 - swirl)[..., None] + c1[None, None, :] * swirl[..., None]
    if m["pat"] == "jelly":
        a = pat_acid(rng)
        g = np.clip(0.3 + 0.7 * a["g"], 0, 1)
        halo = a["halo"]
    else:
        g = 0.3 + 0.45 * smoothstep(0.3, 0.95, swirl)
        halo = np.clip(blur(g, 4.0) * 0.3, 0, 1)
    return compose(co, 0.05 * n, 0.85, 0.9, m["glow"], g, halo, m["gi"])


def make_tile(m):
    rng = np.random.default_rng(1000 + m["idx"] * 37)
    shape = m["shape"]
    if shape == "pouch":
        out = tex_cloth(rng, m)
    elif shape == "capsule":
        out = tex_liquid(rng, m)
    else:
        out = tex_substance(rng, m)
    out = {k: np.array(v, copy=True) for k, v in out.items()}
    rows = slice(178, S)
    if shape == "crystal":
        aux = tex_rock(rng, m)
        for k in out:
            out[k][rows] = aux[k][rows]
    elif shape == "capsule":
        aux = tex_metal(rng)
        for k in out:
            out[k][rows] = aux[k][rows]
    elif shape == "pouch":
        dust, cord = tex_dust(rng, m), tex_cord(rng)
        for k in out:
            out[k][rows, :120] = dust[k][rows, :120]
            out[k][rows, 120:] = cord[k][rows, 120:]
    return out


def tile_normals(h, k=2.5):
    gx = (np.roll(h, -1, 1) - np.roll(h, 1, 1)) * 0.5
    gy = (np.roll(h, -1, 0) - np.roll(h, 1, 0)) * 0.5
    nx, ny, nz = -gx * k, -gy * k, np.ones_like(h)
    ln = np.sqrt(nx * nx + ny * ny + nz * nz)
    return np.stack([nx / ln, ny / ln, nz / ln], -1) * 0.5 + 0.5


def pad_tile(a):
    extra = ((0, 0),) * (a.ndim - 2)
    a = np.pad(a, ((0, 0), (PAD, PAD)) + extra, mode="wrap")
    return np.pad(a, ((PAD, PAD), (0, 0)) + extra, mode="edge")


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


def make_atlas(png_dir):
    """Атлас 2048x1024 из плиток; пустые плитки — серые без свечения."""
    co = np.full((AH, AW, 3), 0.35)
    em = np.zeros((AH, AW, 3))
    nrm = np.zeros((AH, AW, 3))
    nrm[...] = (0.5, 0.5, 1.0)
    smdi = np.zeros((AH, AW, 3))
    smdi[...] = (1.0, 0.1, 0.2)
    for m in MATS:
        t = make_tile(m)
        tx, ty = (m["idx"] % COLS) * TS, (m["idx"] // COLS) * TS
        co[ty:ty + TS, tx:tx + TS] = pad_tile(t["co"])
        em[ty:ty + TS, tx:tx + TS] = pad_tile(t["em"])
        nrm[ty:ty + TS, tx:tx + TS] = pad_tile(tile_normals(t["h"]))
        smdi[ty:ty + TS, tx:tx + TS] = pad_tile(np.stack([np.ones((S, S)), t["sp"], t["gl"]], -1))
    smdi = smdi.reshape(AH // 2, 2, AW // 2, 2, 3).mean(axis=(1, 3))     # блик — половинное разрешение
    os.makedirs(png_dir, exist_ok=True)
    out = []
    for name, arr in (("vb_ab_mat_co", co), ("vb_ab_mat_e_co", em), ("vb_ab_mat_nohq", nrm), ("vb_ab_mat_smdi", smdi)):
        p = os.path.join(png_dir, name + ".png")
        write_png(p, to_u8(arr))
        out.append((name, p))
    return out


def to_paa(pngs):
    for name, png in pngs:
        dst = os.path.join(OUT, name + ".paa")
        r = subprocess.run([IMAGE_TO_PAA, png, dst], capture_output=True, text=True)
        if r.returncode != 0 or not os.path.exists(dst):
            raise RuntimeError("ImageToPAA: " + name + " " + r.stdout[-400:] + r.stderr[-400:])


# ================================================================ геометрия (оси Blender, метры)
_ICO = {}


def icosphere(level):
    if level in _ICO:
        return _ICO[level]
    t = (1 + 5 ** 0.5) / 2
    V = [unit(v) for v in ((-1, t, 0), (1, t, 0), (-1, -t, 0), (1, -t, 0), (0, -1, t), (0, 1, t), (0, -1, -t),
                           (0, 1, -t), (t, 0, -1), (t, 0, 1), (-t, 0, -1), (-t, 0, 1))]
    F = [(0, 11, 5), (0, 5, 1), (0, 1, 7), (0, 7, 10), (0, 10, 11), (1, 5, 9), (5, 11, 4), (11, 10, 2), (10, 7, 6),
         (7, 1, 8), (3, 9, 4), (3, 4, 2), (3, 2, 6), (3, 6, 8), (3, 8, 9), (4, 9, 5), (2, 4, 11), (6, 2, 10),
         (8, 6, 7), (9, 8, 1)]
    for _ in range(level):
        cache = {}

        def mid(a, b):
            key = (min(a, b), max(a, b))
            if key not in cache:
                V.append(unit(V[a] + V[b]))
                cache[key] = len(V) - 1
            return cache[key]

        F2 = []
        for a, b, c in F:
            ab, bc, ca = mid(a, b), mid(b, c), mid(c, a)
            F2 += [(a, ab, ca), (b, bc, ab), (c, ca, bc), (ab, bc, ca)]
        F = F2
    _ICO[level] = (np.array(V), np.array(F, np.int64))
    return _ICO[level]


def orient(P, T, UV, ref):
    """Грани наружу: ref — точка внутри (для выпуклых/звёздных частей) или функция центров граней -> «наружу»."""
    a, b, c = P[T[:, 0]], P[T[:, 1]], P[T[:, 2]]
    n = np.cross(b - a, c - a)
    cen = (a + b + c) / 3
    out = ref(cen) if callable(ref) else cen - np.asarray(ref, float)
    bad = np.einsum("ij,ij->i", n, out) < 0
    T = T.copy()
    T[bad] = T[bad][:, [0, 2, 1]]
    if UV is not None:
        UV = UV.copy()
        UV[bad] = UV[bad][:, [0, 2, 1]]
    return T, UV


def split_flat(P, T):
    """Каждой грани свои вершины: грани (кристалл, уголь) остаются плоскими при любом сглаживании."""
    return P[T].reshape(-1, 3), np.arange(len(T) * 3, dtype=np.int64).reshape(-1, 3)


def _fit(A, B, ea, eb, rect, inset=2.0):
    x0, y0, x1, y1 = rect
    rw, rh = x1 - x0 - 2 * inset, y1 - y0 - 2 * inset
    s = min(rw / max(ea, 1e-9), rh / max(eb, 1e-9))
    x = x0 + inset + (rw - s * ea) / 2 + A * s
    y = y0 + inset + (rh - s * eb) / 2 + B * s
    return np.stack([x, y], -1)


def box_uv(P, T, rect, lo=None, hi=None):
    """Кубическая проекция по нормали грани в прямоугольник плитки (без растяжения, по центру)."""
    lo = P.min(0) if lo is None else lo
    hi = P.max(0) if hi is None else hi
    ext = hi - lo
    a, b, c = P[T[:, 0]], P[T[:, 1]], P[T[:, 2]]
    k = np.argmax(np.abs(np.cross(b - a, c - a)), axis=1)
    UV = np.zeros((len(T), 3, 2))
    for kk, (ia, ib) in {0: (1, 2), 1: (0, 2), 2: (0, 1)}.items():
        sel = k == kk
        if not sel.any():
            continue
        pts = P[T[sel]]
        A = pts[..., ia] - lo[ia]
        B = (hi[ib] - pts[..., ib]) if ib == 2 else (pts[..., ib] - lo[ib])
        UV[sel] = _fit(A, B, ext[ia], ext[ib], rect)
    return UV


def planar_uv(P, T, ia, ib, rect, lo=None, hi=None):
    """Плоская проекция по осям ia (u) и ib (v, верх картинки = максимум оси)."""
    lo = P.min(0) if lo is None else lo
    hi = P.max(0) if hi is None else hi
    pts = P[T]
    return _fit(pts[..., ia] - lo[ia], hi[ib] - pts[..., ib], hi[ia] - lo[ia], hi[ib] - lo[ib], rect)


def grid(rings, us, vs):
    """Сетка из колец одинаковой длины m: грань (i,j),(i,j+1),(i+1,j+1). Кольца против часовой (вид сверху),
    профиль идёт так, что снаружи справа -> нормали наружу. us — m+1 значений u, vs — по значению v на кольцо."""
    R = np.asarray(rings, float)
    nz, m = R.shape[:2]
    P = R.reshape(-1, 3)
    i = np.arange(nz - 1)[:, None]
    j = np.arange(m)[None, :]
    a, b = i * m + j, i * m + (j + 1) % m
    c, d = (i + 1) * m + (j + 1) % m, (i + 1) * m + j
    T = np.concatenate([np.stack([a, b, c], -1).reshape(-1, 3), np.stack([a, c, d], -1).reshape(-1, 3)])
    us, vs = np.asarray(us, float), np.asarray(vs, float)
    shp = (nz - 1, m)
    ua, ub = np.broadcast_to(us[:-1][None, :], shp), np.broadcast_to(us[1:][None, :], shp)
    va, vb = np.broadcast_to(vs[:-1][:, None], shp), np.broadcast_to(vs[1:][:, None], shp)
    uv1 = np.stack([np.stack([ua, va], -1), np.stack([ub, va], -1), np.stack([ub, vb], -1)], -2)
    uv2 = np.stack([np.stack([ua, va], -1), np.stack([ub, vb], -1), np.stack([ua, vb], -1)], -2)
    UV = np.concatenate([uv1.reshape(-1, 3, 2), uv2.reshape(-1, 3, 2)])
    return P, T.astype(np.int64), UV


def fan(ring, center, us, v_ring, uv_center, down):
    """Веер от кольца к центру: down — грани смотрят вниз (дно), иначе вверх (макушка)."""
    m = len(ring)
    P = np.vstack([ring, np.asarray(center, float)[None]])
    j = np.arange(m)
    j1 = (j + 1) % m
    cc = np.full(m, m)
    uc = np.broadcast_to(np.asarray(uv_center, float), (m, 2))
    uj = np.stack([np.asarray(us, float)[j], np.full(m, v_ring)], -1)
    uj1 = np.stack([np.asarray(us, float)[j + 1], np.full(m, v_ring)], -1)
    if down:
        T = np.stack([cc, j1, j], 1)
        UV = np.stack([uc, uj1, uj], 1)
    else:
        T = np.stack([cc, j, j1], 1)
        UV = np.stack([uc, uj, uj1], 1)
    return P, T.astype(np.int64), UV


def frames(P, closed=False):
    if closed:
        T = (np.roll(P, -1, 0) - np.roll(P, 1, 0)) / 2
    else:
        T = np.gradient(P, axis=0)
    T = T / np.maximum(np.linalg.norm(T, axis=1, keepdims=True), 1e-12)
    up = np.array([0.0, 0.0, 1.0])
    n0 = up - T[0] * np.dot(up, T[0])
    if np.linalg.norm(n0) < 1e-6:
        n0 = np.array([1.0, 0.0, 0.0]) - T[0] * T[0, 0]
    N = np.zeros_like(P)
    N[0] = unit(n0)
    for i in range(1, len(P)):
        n = N[i - 1] - T[i] * np.dot(N[i - 1], T[i])
        ln = np.linalg.norm(n)
        N[i] = n / ln if ln > 1e-9 else N[i - 1]
    return T, N, np.cross(T, N)


def tube(path, radii, segs, rect, closed=False):
    """Трубка вдоль пути (шнурок). UV: u — вдоль, v — вокруг. Концы — купола."""
    path = np.asarray(path, float)
    n = len(path)
    radii = np.broadcast_to(np.asarray(radii, float), (n,)).astype(float)
    Tg, N, B = frames(path, closed)
    ang = 2 * math.pi * np.arange(segs) / segs
    offs = np.cos(ang)[None, :, None] * N[:, None, :] + np.sin(ang)[None, :, None] * B[:, None, :]
    P = (path[:, None, :] + radii[:, None, None] * offs).reshape(-1, 3)
    pts = np.vstack([path, path[:1]]) if closed else path
    s = np.concatenate([[0.0], np.cumsum(np.linalg.norm(np.diff(pts, axis=0), axis=1))])
    x0, y0, x1, y1 = rect
    xs = x0 + 2 + (x1 - x0 - 4) * s / s[-1]
    ys = y0 + 2 + (y1 - y0 - 4) * np.arange(segs + 1) / segs
    rows = n + 1 if closed else n
    tris, uvs = [], []
    for i in range(rows - 1):
        for j in range(segs):
            a, b = (i % n) * segs + j, (i % n) * segs + (j + 1) % segs
            c, d = ((i + 1) % n) * segs + (j + 1) % segs, ((i + 1) % n) * segs + j
            tris += [(a, b, c), (a, c, d)]
            uvs += [((xs[i], ys[j]), (xs[i], ys[j + 1]), (xs[i + 1], ys[j + 1])),
                    ((xs[i], ys[j]), (xs[i + 1], ys[j + 1]), (xs[i + 1], ys[j]))]
    if not closed:
        base = len(P)
        tip0 = path[0] - Tg[0] * radii[0] * 0.5
        tip1 = path[-1] + Tg[-1] * radii[-1] * 0.5
        P = np.vstack([P, tip0, tip1])
        o = (n - 1) * segs
        for j in range(segs):
            j1 = (j + 1) % segs
            tris.append((j1, j, base))
            uvs.append(((xs[0], ys[j + 1]), (xs[0], ys[j]), (xs[0], (ys[j] + ys[j + 1]) / 2)))
            tris.append((o + j, o + j1, base + 1))
            uvs.append(((xs[-1], ys[j]), (xs[-1], ys[j + 1]), (xs[-1], (ys[j] + ys[j + 1]) / 2)))
    return P, np.array(tris, np.int64), np.array(uvs, float)


def dense_profile(zr, step):
    """Профиль с промежуточными точками; ступени (одинаковый z) остаются ступенями."""
    out = [zr[0]]
    for (z0, r0), (z1, r1) in zip(zr[:-1], zr[1:]):
        k = max(1, int(round(abs(z1 - z0) / step)))
        for i in range(1, k + 1):
            out.append((z0 + (z1 - z0) * i / k, r0 + (r1 - r0) * i / k))
    return out


# ---- комок и галька
def build_lump(m, q):
    f = m["form"]
    seed = m["seed"]
    lvl = (2, 1, 1)[q] if f.get("facet") else (3, 2, 1)[q]
    D, F = icosphere(lvl)
    r = 1 + f["amp"] * fbm3(D * f.get("freq", 1.6) + seed * 0.13, 3, seed)
    if f.get("terrace"):
        r = r + f["terrace"] * np.round(fbm3(D * 2.0 + 3.1, 2, seed + 5) * 3) / 3
    rng = np.random.default_rng(seed)
    for _ in range(f.get("pits", 0)):             # кратеры с валиком
        c = unit(rng.standard_normal(3))
        s = rng.uniform(0.18, 0.35)
        dep = rng.uniform(0.05, 0.10)
        ang = np.arccos(np.clip(D @ c, -1, 1))
        r = r - dep * bump(ang / s) + 0.4 * dep * bump((ang - s) / (0.4 * s))
    for _ in range(f.get("blobs", 0)):            # наросты плесени
        c = unit(rng.standard_normal(3))
        s = rng.uniform(0.25, 0.5)
        ang = np.arccos(np.clip(D @ c, -1, 1))
        r = r + rng.uniform(0.04, 0.09) * bump(ang / s)
    P = D * r[:, None] * np.asarray(f["rad"], float)
    T, _ = orient(P, F, None, (0.0, 0.0, 0.0))
    if f.get("twist"):                            # скрутка вдоль X (после ориентации: она её не меняет)
        ang = f["twist"] * P[:, 0] / f["rad"][0]
        y, z = P[:, 1].copy(), P[:, 2].copy()
        P[:, 1] = y * np.cos(ang) - z * np.sin(ang)
        P[:, 2] = y * np.sin(ang) + z * np.cos(ang)
    if f.get("facet"):
        P, T = split_flat(P, T)
    if f.get("uv") == "planar":
        UV = planar_uv(P, T, 0, 2, MAIN)
    else:
        UV = box_uv(P, T, MAIN)
    return [(P, T, UV)]


# ---- кристалл: друза из шестигранных призм на камне
def crystal_params(m):
    f = m["form"]
    rng = np.random.default_rng(m["seed"])

    def dvec(az, el):
        return np.array([math.sin(el) * math.cos(az), math.sin(el) * math.sin(az), math.cos(el)])

    pr = [dict(root=np.array([0.0, 0.0, 0.0065]), d=dvec(rng.uniform(0, 2 * math.pi), math.radians(rng.uniform(2, 10))),
               r=f["r"], L=f["L"])]
    for _ in range(f["n"] - 1):
        az = rng.uniform(0, 2 * math.pi)
        rr = rng.uniform(0.003, 0.0135)
        root = np.array([rr * math.cos(az), 0.85 * rr * math.sin(az), 0.0062 - 0.2 * rr])
        el = math.radians(rng.uniform(15, 30) + 22 * rr / 0.0135)
        pr.append(dict(root=root, d=dvec(az + rng.uniform(-0.4, 0.4), el), r=f["r"] * rng.uniform(0.42, 0.72),
                       L=f["L"] * rng.uniform(0.38, 0.7)))
    for p in pr:
        p["rot"] = rng.uniform(0, math.pi / 3)
        p["jit"] = 1 + rng.uniform(-0.14, 0.14, 6)
        p["apex"] = rng.uniform(-0.2, 0.2, 2)
        p["taper"] = rng.uniform(0.88, 1.0)
        p["sh"] = rng.uniform(0.72, 0.84)
    pr.sort(key=lambda p: -p["r"] * p["L"])
    return pr


def prism_mesh(p):
    d = unit(p["d"])
    up = np.array([0.0, 0.0, 1.0]) if abs(d[2]) < 0.95 else np.array([1.0, 0.0, 0.0])
    u = unit(np.cross(d, up))
    w = np.cross(d, u)
    ang = p["rot"] + 2 * math.pi * np.arange(6) / 6
    offs = (np.cos(ang)[:, None] * u + np.sin(ang)[:, None] * w) * (p["r"] * p["jit"])[:, None]
    base = p["root"] - d * 0.006
    r0 = base + offs
    r1 = p["root"] + d * p["L"] * p["sh"] + offs * p["taper"]
    apex = p["root"] + d * p["L"] + (u * p["apex"][0] + w * p["apex"][1]) * p["r"]
    P = np.vstack([r0, r1, apex[None], base[None]])
    T = []
    for k in range(6):
        k1 = (k + 1) % 6
        T += [(k, k1, 6 + k1), (k, 6 + k1, 6 + k), (6 + k, 6 + k1, 12), (k1, k, 13)]
    T, _ = orient(P, np.array(T, np.int64), None, p["root"] + d * p["L"] * 0.4)
    return split_flat(P, T)


def build_crystal(m, q):
    prisms = crystal_params(m)[:(len(crystal_params(m)), 4, 2)[q]]
    meshes = [prism_mesh(p) for p in prisms]
    allp = np.vstack([P for P, _ in meshes])
    lo, hi = allp.min(0), allp.max(0)
    parts = [(P, T, box_uv(P, T, MAIN, lo, hi)) for P, T in meshes]
    D, F = icosphere((2, 1, 1)[q])
    r = 1 + 0.18 * fbm3(D * 1.8 + m["seed"] * 0.07, 3, m["seed"] + 3)
    Pr = D * r[:, None] * np.array([0.024, 0.020, 0.0085]) + np.array([0.0, 0.0, 0.0015])
    Tr, _ = orient(Pr, F, None, (0.0, 0.0, 0.0015))
    parts.append((Pr, Tr, box_uv(Pr, Tr, AUX)))
    return parts


# ---- осколок (и линза): контур в плоскости XZ, толщина по Y — стоит «на ребре»
def shard_outline(m, q):
    f = m["form"]
    rng = np.random.default_rng(m["seed"])
    if f.get("lens"):
        ph = rng.uniform(0, 2 * math.pi)
        chip = rng.uniform(0, 2 * math.pi)
        n = (40, 24, 14)[q]
        phi = np.linspace(0, 2 * math.pi, n, endpoint=False)
        rho = 1 + 0.03 * np.sin(3 * phi + ph)
        dch = (phi - chip + math.pi) % (2 * math.pi) - math.pi
        rho = rho * (1 - 0.12 * bump(dch / 0.4))          # скол на краю линзы
        return np.column_stack([f["a"] * rho * np.cos(phi), f["b"] * rho * np.sin(phi)])
    n = f["spikes"]
    off = rng.uniform(0, 2 * math.pi)
    tips = (np.arange(n) + rng.uniform(-0.12, 0.12, n)) / n * 2 * math.pi + off
    trho = rng.uniform(f.get("tipmin", 0.72), 1.0, n)
    up = np.abs((tips - math.pi / 2 + math.pi) % (2 * math.pi) - math.pi)
    trho[int(np.argmin(up))] = 1.0                       # самое длинное острие — вверх
    nxt = np.roll(tips, -1)
    nxt[-1] += 2 * math.pi
    notch = (tips + nxt) / 2 + rng.uniform(-0.1, 0.1, n) * (2 * math.pi / n)
    nrho = 1 - f["jag"] * rng.uniform(0.4, 1.0, n)
    phi = np.ravel(np.column_stack([tips, notch]))
    rho = np.ravel(np.column_stack([trho, nrho]))
    pts = np.column_stack([f["a"] * rho * np.cos(phi), f["b"] * rho * np.sin(phi)])
    s = (4, 2, 1)[q]
    out = []
    for i in range(len(pts)):
        p0, p1 = pts[i], pts[(i + 1) % len(pts)]
        for t in range(s):
            out.append(p0 + (p1 - p0) * t / s)
    return np.array(out)


def build_shard(m, q):
    f = m["form"]
    lens = bool(f.get("lens"))
    o = shard_outline(m, q)
    k = len(o)
    phi = np.arctan2(o[:, 1], o[:, 0])
    ts = ([0.34, 0.67, 1.0], [0.5, 1.0], [1.0])[q]

    def thick(t, front):
        h = f["e"] + (f["T"] - f["e"]) * (1 - t * t) ** 0.7
        if front and not lens:                            # раковистый скол на лицевой стороне
            h = h + 0.00035 * np.sin(t * 13 + phi * 2 + m["seed"]) * t
        return h

    def surface(sign, front):
        pts = [np.array([[0.0, sign * f["T"], 0.0]])]
        for t in ts:
            pts.append(np.column_stack([t * o[:, 0], np.full(k, 0.0) + sign * thick(t, front), t * o[:, 1]]))
        P = np.vstack(pts)
        T = [(0, 1 + j, 1 + (j + 1) % k) for j in range(k)]
        for i in range(len(ts) - 1):
            for j in range(k):
                a, b = 1 + i * k + j, 1 + i * k + (j + 1) % k
                c, d = 1 + (i + 1) * k + (j + 1) % k, 1 + (i + 1) * k + j
                T += [(a, b, c), (a, c, d)]
        return P, np.array(T, np.int64)

    Pf, Tf = surface(-1.0, True)
    Pb, Tb = surface(1.0, False)
    last = 1 + (len(ts) - 1) * k
    Pe = np.vstack([Pf[last:last + k], Pb[last:last + k]])
    Te = []
    for j in range(k):
        j1 = (j + 1) % k
        Te += [(j, j1, k + j1), (j, k + j1, k + j)]
    Te = np.array(Te, np.int64)
    parts = []
    lo = np.minimum(Pf.min(0), Pb.min(0))
    hi = np.maximum(Pf.max(0), Pb.max(0))
    for P, T in ((Pf, Tf), (Pb, Tb), (Pe, Te)):
        T, _ = orient(P, T, None, (0.0, 0.0, 0.0))
        parts.append((P, T, planar_uv(P, T, 0, 2, MAIN, lo, hi)))
    return parts


# ---- мешочек с пылью
POUCH_PROF = [(0.0, 0.007), (0.0015, 0.014), (0.005, 0.0195), (0.012, 0.0235), (0.021, 0.0248), (0.030, 0.0232),
              (0.037, 0.0192), (0.042, 0.0128), (0.0445, 0.0092), (0.047, 0.0095), (0.0505, 0.0122),
              (0.055, 0.0148), (0.0595, 0.0162), (0.0615, 0.0158)]
NECK_Z = 0.0448


def sack_rings(prof, mseg, ph, zmax, inner=False, z_min=None):
    """Кольца мешка: складки сильнее к горловине, верхний край рваный; inner — изнанка оборки (на 12 % уже)."""
    th = np.linspace(0, 2 * math.pi, mseg, endpoint=False)
    rag = 0.0018 * np.sin(7 * th + ph[3]) + 0.0012 * np.sin(12 * th + ph[4]) + 0.001 * np.sin(3 * th + ph[0])
    fa = 1.0 if mseg >= 18 else 0.3                       # на грубых LOD складки мельче (иначе сетка их искажает)
    rings, zs = [], []
    for z, r in prof:
        if z_min is not None and z < z_min:
            continue
        k = float(np.clip((z - 0.012) / (zmax - 0.012), 0, 1)) ** 1.2
        fold = 1 + fa * ((0.025 + 0.15 * k) * np.sin(9 * th + ph[0] + 1.5 * k) + 0.035 * np.sin(4 * th + ph[1] + z * 60))
        lx, ly = 0.0012 * math.sin(z * 45 + ph[2]), 0.001 * math.cos(z * 36 + ph[2])
        top = z >= zmax - 1e-9
        rr = r * fold * (0.88 if (inner and not top) else 1.0)
        zz = z + rag * float(np.clip((z - (zmax - 0.006)) / 0.006, 0, 1))
        rings.append(np.column_stack([rr * np.cos(th) + lx, rr * np.sin(th) + ly, zz]))
        zs.append(z)
    return rings, zs


def build_pouch(m, q):
    rng = np.random.default_rng(m["seed"])
    ph = rng.uniform(0, 6.28, 5)
    prof = dense_profile(POUCH_PROF, (0.003, 0.006, 0.012)[q])
    zmax = prof[-1][0]
    mseg = (24, 18, 9)[q]
    us = np.linspace(MAIN[0], MAIN[2], mseg + 1)

    def vmain(zs):
        return MAIN[1] + 2 + (1 - np.asarray(zs) / zmax) * (MAIN[3] - MAIN[1] - 4)

    parts = []
    rings, zs = sack_rings(prof, mseg, ph, zmax)
    vs = vmain(zs)
    parts.append(grid(rings, us, vs))
    cen = np.append(rings[0][:, :2].mean(0), -0.0006)
    parts.append(fan(rings[0], cen, us, vs[0], (120.0, MAIN[3] - 2), down=True))
    ri, zi = sack_rings(prof, mseg, ph, zmax, inner=True, z_min=NECK_Z + 0.002)
    Pi, Ti, UVi = grid(ri, us, vmain(zi))
    parts.append((Pi, Ti[:, [0, 2, 1]], UVi[:, [0, 2, 1]]))            # изнанка оборки смотрит внутрь
    # пыль в горловине: бугристый купол
    ineck = int(np.argmin([abs(z - NECK_Z) for z in zs]))
    nc = rings[ineck][:, :2].mean(0)
    rd, hc = 0.0098, 0.0072
    Rs = (rd * rd + hc * hc) / (2 * hc)
    amax = math.asin(rd / Rs)
    cz = NECK_Z + 0.0005 + hc - Rs
    nr, ns = (5, 3, 2)[q], (16, 10, 6)[q]
    th = np.linspace(0, 2 * math.pi, ns, endpoint=False)
    ctr = np.array([nc[0], nc[1], cz])
    drings = []
    for i in range(nr):
        a = amax * (1 - i / nr)
        d = np.column_stack([np.sin(a) * np.cos(th), np.sin(a) * np.sin(th), np.full(ns, math.cos(a))])
        kk = 1 + 0.07 * fbm3(d * 3.0 + m["seed"] * 0.1, 2, m["seed"])
        drings.append(ctr + d * Rs * kk[:, None])
    pole = ctr + np.array([0.0, 0.0, Rs * (1 + 0.07 * float(fbm3(np.array([[0, 0, 3.0]]) + m["seed"] * 0.1, 2, m["seed"])[0]))])
    Pd, Td, _ = grid(drings, np.zeros(ns + 1), np.zeros(nr))
    Pt, Tt, _ = fan(drings[-1], pole, np.zeros(ns + 1), 0.0, (0.0, 0.0), down=False)
    Pd2 = np.vstack([Pd, Pt])
    Td2 = np.vstack([Td, Tt + len(Pd)])
    parts.append((Pd2, Td2, planar_uv(Pd2, Td2, 0, 1, AUX_L)))
    # шнурок на горловине, узел и два хвоста
    rad = np.linalg.norm(rings[ineck][:, :2] - nc, axis=1)
    rc = float(np.percentile(rad, 75)) + 0.0009
    ncord, cs = (28, 16, 10)[q], (6, 5, 4)[q]
    a = np.linspace(0, 2 * math.pi, ncord, endpoint=False)
    ring_path = np.column_stack([nc[0] + rc * np.cos(a), nc[1] + rc * np.sin(a), np.full(ncord, NECK_Z)])
    parts.append(tube(ring_path, 0.0016, cs, AUX_R, closed=True))
    a0 = -math.pi / 2 + 0.45                              # узел спереди (перед в Blender — -Y)
    knot = np.array([nc[0] + (rc + 0.0012) * math.cos(a0), nc[1] + (rc + 0.0012) * math.sin(a0), NECK_Z])
    Dk, Fk = icosphere((1, 1, 0)[q])
    Pk = Dk * np.array([0.0027, 0.0027, 0.0022]) + knot
    Tk, _ = orient(Pk, Fk, None, knot)
    parts.append((Pk, Tk, box_uv(Pk, Tk, AUX_R)))
    pz = [z for z, _ in POUCH_PROF]
    pr = [r for _, r in POUCH_PROF]
    nt = (7, 5, 3)[q]
    for da, zend in ((-0.25, 0.031), (0.3, 0.035)):
        zt = np.linspace(NECK_Z - 0.001, zend, nt)
        at = a0 + np.linspace(0, da, nt)
        rt = np.interp(zt, pz, pr) * 1.12 + 0.0018
        pts = np.column_stack([nc[0] + rt * np.cos(at), nc[1] + rt * np.sin(at), zt])
        pts[0] = knot
        parts.append(tube(pts, np.linspace(0.0015, 0.0011, nt), cs, AUX_R))
    return parts


# ---- капсула: содержимое за «стеклом», металлические крышки и поясок
CAP_PROF = [(0.0, 0.0050), (0.0010, 0.0076), (0.0028, 0.0088), (0.0102, 0.0088), (0.0112, 0.0082), (0.0112, 0.0074)]
CAP_L = 0.066
CAP_R = 0.0078


def build_capsule(m, q):
    segs = (24, 14, 8)[q]
    th = np.linspace(0, 2 * math.pi, segs, endpoint=False)
    us = np.linspace(0.0, 240.0, segs + 1)

    def ring(z, r):
        return np.column_stack([r * np.cos(th), r * np.sin(th), np.full(segs, z)])

    parts = []
    z0, z1 = 0.0095, CAP_L - 0.0095
    zs = np.linspace(z0, z1, (9, 5, 3)[q])
    vb = MAIN[1] + 2 + (1 - (zs - z0) / (z1 - z0)) * (MAIN[3] - MAIN[1] - 4)
    parts.append(grid([ring(z, CAP_R) for z in zs], us, vb))
    prof = CAP_PROF if q < 2 else [CAP_PROF[0], CAP_PROF[2], CAP_PROF[3], CAP_PROF[5]]
    pa = np.array(prof)
    s = np.concatenate([[0.0], np.cumsum(np.hypot(np.diff(pa[:, 0]), np.diff(pa[:, 1])))])
    vc = AUX[1] + 2 + s / s[-1] * (AUX[3] - AUX[1] - 4)
    bottom = [ring(z, r) for z, r in prof]
    parts.append(grid(bottom, us, vc))
    parts.append(fan(bottom[0], (0.0, 0.0, -0.0004), us, vc[0], (120.0, AUX[1] + 2), down=True))
    top = [ring(CAP_L - z, r) for z, r in reversed(prof)]         # зеркально, z по возрастанию
    vt = vc[::-1]
    parts.append(grid(top, us, vt))
    parts.append(fan(top[-1], (0.0, 0.0, CAP_L + 0.0004), us, vt[-1], (120.0, AUX[1] + 2), down=False))
    band = [(0.0318, 0.0074), (0.0322, 0.0083), (0.0344, 0.0083), (0.0348, 0.0074)]
    vband = AUX[1] + 2 + np.arange(4) / 3 * (AUX[3] - AUX[1] - 4)
    parts.append(grid([ring(z, r) for z, r in band], us, vband))
    return parts


BUILDERS = {"lump": build_lump, "pebble": build_lump, "crystal": build_crystal, "shard": build_shard,
            "pouch": build_pouch, "capsule": build_capsule}


def assemble(parts):
    Ps, Ts, UVs = [], [], []
    n = 0
    for P, T, UV in parts:
        Ps.append(np.asarray(P, float))
        Ts.append(np.asarray(T, np.int64) + n)
        UVs.append(np.asarray(UV, float))
        n += len(P)
    return np.vstack(Ps), np.vstack(Ts), np.concatenate(UVs)


def lod_meshes(m):
    """3 визуальных LOD: (P, T, UV атласа). Все сдвинуты на центр габаритов LOD0."""
    meshes = [list(assemble(BUILDERS[m["shape"]](m, q))) for q in range(3)]
    P0 = meshes[0][0]
    c = (P0.min(0) + P0.max(0)) / 2
    tx, ty = (m["idx"] % COLS) * TS, (m["idx"] // COLS) * TS
    for mm in meshes:
        mm[0] = mm[0] - c
        uv = mm[2]
        mm[2] = np.stack([(tx + PAD + uv[..., 0]) / AW, (ty + PAD + uv[..., 1]) / AH], -1)
    return meshes


# ================================================================ p3d
def vertex_normals(P, T):
    fn = np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]])
    N = np.zeros_like(P)
    for k in range(3):
        np.add.at(N, T[:, k], fn)
    ln = np.linalg.norm(N, axis=1, keepdims=True)
    bad = ln[:, 0] < 1e-15
    N = N / np.maximum(ln, 1e-15)
    N[bad] = (0, 0, 1)
    return N


def visual_lod(py3d, P, T, UV, resolution):
    lod = py3d.LOD()
    lod.resolution = resolution
    Nrm = vertex_normals(P, T)
    for c in P:
        p = py3d.Point()
        p.coords = (float(c[0]), float(c[1]), float(c[2]))
        lod.points.append(p)
    for n in Nrm:
        lod.facenormals.append((float(n[0]), float(n[1]), float(n[2])))
    fn = np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]])
    fn /= np.maximum(np.linalg.norm(fn, axis=1, keepdims=True), 1e-15)
    for k in range(len(T)):
        f = py3d.Face(lod.points, lod.facenormals)
        f.texture = TEX_CO
        f.material = RVMAT
        flat_idx = None
        for c in range(3):
            idx = int(T[k, c])
            v = py3d.Vertex(lod.points, lod.facenormals)
            v.point_index = idx
            if float(np.dot(Nrm[idx], fn[k])) >= 0.5:
                v.normal_index = idx
            else:                                   # острое ребро: у угла грани своя плоская нормаль
                if flat_idx is None:
                    flat_idx = len(lod.facenormals)
                    lod.facenormals.append((float(fn[k, 0]), float(fn[k, 1]), float(fn[k, 2])))
                v.normal_index = flat_idx
            v.uv = (float(UV[k, c, 0]), float(UV[k, c, 1]))
            f.vertices.append(v)
        lod.faces.append(f)
    return lod


BOX_QUADS = [(0, 4, 6, 2), (1, 3, 7, 5), (0, 1, 5, 4), (2, 6, 7, 3), (0, 2, 3, 1), (4, 5, 7, 6)]
BOX_NORMALS = [(-1, 0, 0), (1, 0, 0), (0, -1, 0), (0, 1, 0), (0, 0, -1), (0, 0, 1)]


def box_lod(py3d, resolution, lo, hi, penetration, mass_per_point=None):
    """Выпуклый ящик (оси Blender, грани против часовой снаружи) — компонент коллизии, autocenter = 0 (E54)."""
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
    for fi, qd in enumerate(BOX_QUADS):
        for tri in ((qd[0], qd[1], qd[2]), (qd[0], qd[2], qd[3])):
            f = py3d.Face(lod.points, lod.facenormals)
            f.material = penetration
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


def model_path(m):
    return os.path.join(OUT, "vb_ab_mat_%s.p3d" % m["key"].lower())


def build_p3d(m):
    sys.path.insert(0, os.path.join(REPO, ".claude", "tools", "py3d"))
    import py3d
    meshes = lod_meshes(m)
    model = py3d.P3D()
    for (P, T, UV), res in zip(meshes, LOD_RES):
        model.lods.append(visual_lod(py3d, P, T, UV, res))
    P0 = meshes[0][0]
    lo, hi = P0.min(0), P0.max(0)
    glo, ghi = lo * GEOM_K, hi * GEOM_K
    pen = BS.join(["dz", "data", "data", "penetration", PENETRATION[m["shape"]] + ".rvmat"])
    model.lods.append(box_lod(py3d, 1e13, glo, ghi, pen, mass_per_point=m["w"] / 1000.0 / 8))
    mem = py3d.LOD()
    mem.resolution = 1e15
    mem.set_memory_point("boundingbox_min", tuple(float(v) for v in lo))
    mem.set_memory_point("boundingbox_max", tuple(float(v) for v in hi))
    model.lods.append(mem)
    model.lods.append(box_lod(py3d, 6e15, glo, ghi, pen))
    model.lods.append(box_lod(py3d, 7e15, glo, ghi, pen))
    py3d.blender_to_dayz(model)
    model.save(model_path(m), verify=True)
    findings = [f for f in model.validate() if getattr(f, "severity", "") in ("ERROR", "WARN")]
    stats = [len(T) for _, T, _ in meshes]
    return stats, (hi - lo), findings


# ================================================================ материал, конфиг, строки
def write_rvmat():
    # без комментариев: формат как у проверенных в игре rvmat Изнанки
    s = "ambient[] = {1,1,1,1};\ndiffuse[] = {1,1,1,1};\nforcedDiffuse[] = {0,0,0,0};\n"
    s += "emmisive[] = {%.2f,%.2f,%.2f,1};\n" % (EMISSIVE, EMISSIVE, EMISSIVE)
    s += "specular[] = {1,1,1,1};\nspecularPower = 120;\n"
    s += "PixelShaderID = \"Super\";\nVertexShaderID = \"Super\";\n"
    for i in range(8):
        s += ("class TexGen%d\n{\n\tuvSource = \"tex\";\n\tclass uvTransform\n\t{\n\t\taside[] = {1,0,0};\n"
              "\t\tup[] = {0,1,0};\n\t\tdir[] = {0,0,1};\n\t\tpos[] = {0,0,0};\n\t};\n};\n") % i
    stages = [
        (1, PREFIX + "vb_ab_mat_nohq.paa", 1), (2, "#(argb,8,8,3)color(0.5,0.5,0.5,1,DT)", 2),
        (3, "#(argb,8,8,3)color(0,0,0,0,MC)", 3), (4, "#(argb,8,8,3)color(1,1,1,1,AS)", 4),
        (5, PREFIX + "vb_ab_mat_smdi.paa", 5), (6, "#(ai,64,64,1)fresnel(1.3,0.3)", 6),
        (7, BS.join(["dz", "data", "data", "env_land_co.paa"]), 7), (8, PREFIX + "vb_ab_mat_e_co.paa", 0),
    ]
    for i, tx, tg in stages:
        s += "class Stage%d\n{\n\ttexture = \"%s\";\n\ttexGen = \"%d\";\n};\n" % (i, tx, tg)
    with open(os.path.join(OUT, "vb_ab_mat.rvmat"), "w", encoding="utf-8", newline="\r\n") as f:
        f.write(s)


def write_hpp():
    L = [
        "// СГЕНЕРИРОВАНО tools/ab_materials.py — руками не править",
        "// Материалы аномалий (ТЗ 2.6): простые предметы без свойств и скриптов, не еда, не портятся.",
        "// Стопка до 20: параметры количества как у ванильного ChickenFeather (DZ/gear/crafting/config.cpp):",
        "// canBeSplit, varQuantityInit = 1, varQuantityMin = 0, varQuantityMax = 20. weight — вес ОДНОЙ штуки:",
        "// у стакающегося предмета вес = weight x количество (ItemBase.GetWeightSpecialized).",
        "// rotationFlags: 2 — лежит как смоделирован (кристалл, комок, мешочек); 17 — плашмя (осколок, галька,",
        "// капсула смоделированы стоя на ребре), см. E53. Модели, атлас и материал — VB_AB/i/mat.",
        "\tclass VB_AB_Mat_Base: Inventory_Base",
        "\t{",
        "\t\tscope = 0;",
        "\t\titemSize[] = {1, 1};",
        "\t\tweight = 50;",
        "\t\tcanBeSplit = 1;",
        "\t\tvarQuantityInit = 1;",
        "\t\tvarQuantityMin = 0;",
        "\t\tvarQuantityMax = 20;",
        "\t\tvarQuantityDestroyOnMin = 1;",
        "\t\trotationFlags = 2;",
        "\t\tsoundImpactType = \"default\";",
        "\t};",
    ]
    for m in MATS:
        L += [
            "\t// %s — %s (%s)" % (m["key"], m["ru"], m["shape"]),
            "\tclass VB_AB_Mat_%s: VB_AB_Mat_Base" % m["key"],
            "\t{",
            "\t\tscope = 2;",
            "\t\tdisplayName = \"$STR_VB_AB_Mat_%s\";" % m["key"],
            "\t\tdescriptionShort = \"$STR_VB_AB_Mat_%s_Desc\";" % m["key"],
            "\t\tmodel = \"%svb_ab_mat_%s.p3d\";" % (PREFIX, m["key"].lower()),
            "\t\tweight = %d;" % m["w"],
            "\t\trotationFlags = %d;" % RF[m["shape"]],
            "\t\tsoundImpactType = \"%s\";" % m.get("sound", SOUND[m["shape"]]),
            "\t};",
        ]
    with open(CFG, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(L) + "\n")


def write_strings():
    d = {}
    for m in MATS:
        d["STR_VB_AB_Mat_" + m["key"]] = [m["ru"], m["en"]]
        d["STR_VB_AB_Mat_" + m["key"] + "_Desc"] = [m["dru"], m["den"]]
    os.makedirs(os.path.dirname(STRINGS), exist_ok=True)
    with open(STRINGS, "w", encoding="utf-8", newline="\n") as f:
        json.dump(d, f, ensure_ascii=False, indent=1)
        f.write("\n")


# ================================================================ превью (только внутри Blender)
def _render_sheet(bpy, data, mode, out_path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    scene = bpy.context.scene
    for eng in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try:
            scene.render.engine = eng
            break
        except TypeError:
            pass
    scene.render.resolution_x, scene.render.resolution_y = 2400, 1650
    scene.render.filepath = out_path
    try:
        scene.view_settings.view_transform = "Standard"
    except TypeError:
        pass
    co_img = bpy.data.images.load(os.path.join(PNG_DIR, "vb_ab_mat_co.png"))
    e_img = bpy.data.images.load(os.path.join(PNG_DIR, "vb_ab_mat_e_co.png"))
    mat = bpy.data.materials.new("vb_ab_mat")
    mat.use_nodes = True
    nt = mat.node_tree
    bsdf = nt.nodes["Principled BSDF"]
    t_co = nt.nodes.new("ShaderNodeTexImage")
    t_co.image = co_img
    nt.links.new(t_co.outputs["Color"], bsdf.inputs["Base Color"])
    t_e = nt.nodes.new("ShaderNodeTexImage")
    t_e.image = e_img
    nt.links.new(t_e.outputs["Color"], bsdf.inputs["Emission Color"])
    bsdf.inputs["Emission Strength"].default_value = EMISSIVE
    bsdf.inputs["Roughness"].default_value = 0.35
    try:
        mat.use_backface_culling = True          # вывернутая грань будет видна дырой
    except AttributeError:
        pass
    try:
        font = bpy.data.fonts.load("C:/Windows/Fonts/arial.ttf")
    except Exception:
        font = None
    tm = bpy.data.materials.new("label")
    tm.use_nodes = True
    tb = tm.node_tree.nodes["Principled BSDF"]
    tb.inputs["Base Color"].default_value = (1, 1, 1, 1)
    tb.inputs["Emission Color"].default_value = (1, 1, 1, 1)
    tb.inputs["Emission Strength"].default_value = 0.8
    cols, step = 6, 0.15
    rows = (len(data) - 1) // cols + 1
    for idx, (m, (P, T, UV)) in enumerate(data):
        Q = P.copy()
        col, row = idx % cols, idx // cols
        if mode == "icon":
            loc = (col * step, 0.0, -row * step)
        else:
            if RF[m["shape"]] == 17:                 # лёг плашмя: ось Y Blender (тонкая) становится вертикалью
                Q = np.column_stack([Q[:, 0], -Q[:, 2], Q[:, 1]])
            Q[:, 2] -= Q[:, 2].min()
            loc = (col * step, -row * step * 1.2, 0.0)
        me = bpy.data.meshes.new(m["key"])
        me.from_pydata(Q.tolist(), [], T.tolist())
        uvl = me.uv_layers.new(name="UV")
        fl = UV.copy()
        fl[..., 1] = 1 - fl[..., 1]                  # в p3d v сверху вниз, в Blender снизу вверх
        uvl.data.foreach_set("uv", fl.reshape(-1).tolist())
        me.polygons.foreach_set("use_smooth", [True] * len(T))
        me.materials.append(mat)
        ob = bpy.data.objects.new(m["key"], me)
        ob.location = loc
        scene.collection.objects.link(ob)
        cu = bpy.data.curves.new(m["key"] + "_t", "FONT")
        cu.body = "%s\n%s  %d г" % (m["ru"], m["key"], m["w"])
        if font:
            cu.font = font
        cu.size = 0.0105
        cu.align_x = "CENTER"
        cu.materials.append(tm)
        to = bpy.data.objects.new(m["key"] + "_t", cu)
        if mode == "icon":
            to.location = (col * step, -0.05, -row * step - 0.058)
            to.rotation_euler = (math.pi / 2, 0, 0)
        else:
            to.location = (col * step, -row * step * 1.2 - 0.06, 0.0005)
        scene.collection.objects.link(to)
    cx = (cols - 1) * step / 2
    cc = -(rows - 1) * step * (1.0 if mode == "icon" else 1.2) / 2 - 0.015
    world = bpy.data.worlds.new("w")
    scene.world = world
    world.use_nodes = True
    bg = world.node_tree.nodes["Background"]
    sun = bpy.data.lights.new("sun", "SUN")
    if mode != "icon":
        bpy.ops.mesh.primitive_plane_add(size=3.0, location=(cx, cc, 0))
        gm = bpy.data.materials.new("ground")
        gm.use_nodes = True
        gb = gm.node_tree.nodes["Principled BSDF"]
        gb.inputs["Base Color"].default_value = (0.20, 0.18, 0.15, 1)
        gb.inputs["Roughness"].default_value = 0.9
        bpy.context.active_object.data.materials.append(gm)
    if mode == "day":
        bg.inputs["Color"].default_value = (0.5, 0.55, 0.62, 1)
        bg.inputs["Strength"].default_value = 0.9
        sun.energy, sun.color, rot = 3.0, (1.0, 0.97, 0.92), (math.radians(40), 0, math.radians(35))
    elif mode == "night":
        bg.inputs["Color"].default_value = (0.003, 0.004, 0.007, 1)
        sun.energy, sun.color, rot = 0.03, (0.6, 0.7, 1.0), (math.radians(40), 0, math.radians(35))
    else:
        bg.inputs["Color"].default_value = (0.16, 0.16, 0.16, 1)
        sun.energy, sun.color, rot = 2.2, (1.0, 1.0, 1.0), (math.radians(55), 0, math.radians(-25))
    so = bpy.data.objects.new("sun", sun)
    so.rotation_euler = rot
    scene.collection.objects.link(so)
    cd = bpy.data.cameras.new("cam")
    cd.type = "ORTHO"
    cd.ortho_scale = cols * step + 0.06
    co = bpy.data.objects.new("cam", cd)
    if mode == "icon":                               # как превью инвентаря: спереди, в осях модели
        co.location = (cx, -1.0, cc)
        co.rotation_euler = (math.pi / 2, 0, 0)
    else:
        tilt = math.radians(50)
        co.location = (cx, cc - 2.0 * math.sin(tilt), 2.0 * math.cos(tilt))
        co.rotation_euler = (tilt, 0, 0)
    scene.collection.objects.link(co)
    scene.camera = co
    bpy.ops.render.render(write_still=True)


def preview(out_dir, lod=0):
    import bpy
    os.makedirs(out_dir, exist_ok=True)
    need = [os.path.join(PNG_DIR, n + ".png") for n in ("vb_ab_mat_co", "vb_ab_mat_e_co")]
    if not all(os.path.exists(p) for p in need):
        make_atlas(PNG_DIR)
    data = [(m, lod_meshes(m)[lod]) for m in MATS]
    outs = []
    for mode in (("day", "night", "icon") if lod == 0 else ("icon",)):
        p = os.path.join(out_dir, "vb_ab_materials_%s%s.png" % (mode, "_lod%d" % lod if lod else ""))
        _render_sheet(bpy, data, mode, p)
        outs.append(p)
    print("PREVIEW", " | ".join(outs))


# ================================================================
def main():
    args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else sys.argv[1:]
    if "--preview" in args:
        i = args.index("--preview")
        out = args[i + 1] if len(args) > i + 1 and not args[i + 1].startswith("--") else PREVIEW_DIR
        lods = [int(a.split("=", 1)[1]) for a in args if a.startswith("--lod=")]
        preview(out, lods[0] if lods else 0)
        return
    only = [o for a in args if a.startswith("--only=") for o in a.split("=", 1)[1].split(",") if o]
    os.makedirs(OUT, exist_ok=True)
    for m in MATS:
        if only and m["key"] not in only:
            continue
        stats, dims, findings = build_p3d(m)
        print("%-11s %-8s LOD0 %5d тр.  LOD1 %4d  LOD2 %4d  %4.1f x %4.1f x %4.1f см  RF %d  %d г" % (
            m["key"], m["shape"], stats[0], stats[1], stats[2], dims[0] * 100, dims[1] * 100, dims[2] * 100,
            RF[m["shape"]], m["w"]))
        for f in findings:
            if "LOWCONF" not in f.code and "POS_CENTER" not in f.code:
                print("   validate:", f)
    pngs = make_atlas(PNG_DIR)
    if "--no-paa" not in args:
        to_paa(pngs)
    write_rvmat()
    write_hpp()
    write_strings()
    total = sum(os.path.getsize(os.path.join(OUT, fn)) for fn in os.listdir(OUT))
    print("PNG:", PNG_DIR)
    print("готово: %s, %.2f МБ" % (OUT, total / 1048576.0))


if __name__ == "__main__":
    main()
