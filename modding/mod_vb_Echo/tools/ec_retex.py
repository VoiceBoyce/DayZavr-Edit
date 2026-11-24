# -*- coding: utf-8 -*-
r"""Ретекстур одежды VB_EchoClothes (этап 3а): из исходной _co OOTS/IMP делает <имя>_<палитра>_co.paa.

  blender.exe -b --factory-startup --python ec_retex.py -- <вещь[,вещь...]|all> [--palette=echo] [--preview=<папка>]
      [--nopaa]

Вещи: jacket, pants, vest, gloves, boots, servo1, servo2, mask, helmet (all — все). Палитра echo — «чёрная броня Эхо»
(решение пользователя 11-23: без ржавчины, чтобы хотелось носить; расцветки потом — новая палитра в PALETTES и тот же
запуск с --palette=<имя>, файлы <имя>_<палитра>_co.paa, классы-расцветки в config.cpp).
ТКАНЬ — отдельный слой (11-24, решение пользователя: ткань чёрная в цвет шлема, металл не трогать; ткань — то, что
перекрашивают расцветки): маска по карте блеска _smdi (у ткани блеска нет, у пластин, кожи, резины — есть; порог —
своя пара на вещь, параметр fabric), сглаженная — без дыр в переплетении (из них были белые точки: нити без оливкового
оттенка считались металлом и получали потёртость стали). Цвет — PAL["fabric"], по ткани — крупные складки и потёртости
исходника, мелкое переплетение приглушено. Ложится поверх базы: где маска ткани 0, пиксели как раньше байт в байт.
База (всё, что не ткань) — по цвету исходника, как на этапе 3а:
- оливковая ткань OOTS -> base_fabric (прежний графит; почти вся под слоем ткани, видна только вне его);
- розовато-бурые кромки накладок (пример 62,53,58) -> окантовка цветом акцента палитры; такие же пятна посреди ткани
  (грязь OOTS) -> в общий тон, без акцента;
- металл, ремни, нашивки (серые) -> тёмный сатиновый металл; светлые грани и царапины -> потёртость до стали;
- углубления (темнее соседей) -> нейтральная пыль.
Яркость берётся из исходника (детали и складки сохраняются), меняется только тон. Альфа — как была.
Сервопривод и шлем ткани не имеют — их ретекстур этим слоем не меняется.
Промежуточный PNG — в --preview (или временная папка), PAA — рядом с исходной _co (ImageToPAA). Исходник не трогается.
"""
import os
import shutil
import subprocess
import sys
import tempfile

import bpy
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
BS = chr(92)
CLOTHES = os.path.join(HERE, "..", "VB_EchoClothes", "exo_set")
IMAGE_TO_PAA = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools", "Bin", "ImageToPAA",
                            "ImageToPAA.exe")

# вещь -> (папка data, исходная текстура, параметры)
# metal_k — яркость металла/прочего; tape — синюю изоленту в краску палитры; emblem — (x, y, масштаб) знака Эхо в
# координатах файла 2048 (сверху вниз); no_accent — без окантовки (маска: бежевая кость — не кромка); wear — сила потёртости
# fabric — слой ткани: (порог блеска G «ещё ткань», «уже не ткань») по _smdi рядом; dark — ещё и только тёмное
# в исходнике (у маски блеска нет нигде: балаклава — ткань, светлая пластина черепа — нет)
ITEMS = {
    "jacket": (os.path.join(CLOTHES, "Tops", "ExoJacket", "data"), "ExoJacket_co.paa", {"fabric": (0.03, 0.07)}),
    "pants": (os.path.join(CLOTHES, "Pants", "ExoPants", "data"), "ExoPants_co.paa", {"fabric": (0.03, 0.07)}),
    "vest": (os.path.join(CLOTHES, "Vest", "ExoVest", "data"), "ExoVest_co.paa",
             {"metal_k": 0.8, "emblem": (1235, 190, 1.0), "fabric": (0.03, 0.07)}),
    "gloves": (os.path.join(CLOTHES, "Gloves", "ExoGloves", "data"), "ExoGloves_co.paa",
               {"metal_k": 1.0, "tape": True, "fabric": (0.01, 0.025)}),
    "boots": (os.path.join(CLOTHES, "Boots", "ExoBoots", "data"), "ExoBoots_co.paa",
              {"metal_k": 1.0, "fabric": (0.045, 0.06), "fabric_blur": 10}),     # пик ткани G 0,03–0,045
    "servo1": (os.path.join(CLOTHES, "Backpacks", "ExoPrivod", "data"), "ExoPrivod1_co.paa", {"metal_k": 0.8}),
    "servo2": (os.path.join(CLOTHES, "Backpacks", "ExoPrivod", "data"), "ExoPrivod2_co.paa", {"metal_k": 0.8}),
    "mask": (os.path.join(CLOTHES, "Mask", "MaskGhost", "Data"), "MaskGhost_co.paa",
             {"metal_k": 0.5, "no_accent": True, "wear": 0.3, "fabric": (0.02, 0.05), "dark": (0.18, 0.28)}),
    "helmet": (os.path.join(CLOTHES, "helmet", "ronin_heavy", "data"), "RoninHeavy_Black.paa",
               {"metal_k": 1.0, "emblem_geo": True}),
}
# emblem_geo — знак на висках шлема по его геометрии (ec_helmet_geom.emblem_alpha: раскладка концепта через UV)
# палитры (sRGB 0..1): fabric — слой ткани (ЭТО и перекрашивают расцветки); base_fabric — оливковая ткань в базе
# (прежний графит этапа 3а, видна только вне слоя ткани — например, у маски); metal — множитель тона металла;
# accent — окантовка кромок; steel — потёртость граней; dust — пыль в углублениях; paint — знак и изолента
PALETTES = {
    "echo": {
        "fabric": (0.098, 0.102, 0.11),         # чёрная, в цвет шлема (медиана шлема 25,27,29) — 11-24
        "base_fabric": (0.165, 0.17, 0.18),
        "metal": (0.94, 0.97, 1.03),
        "accent": (0.30, 0.04, 0.05),
        "steel": (0.58, 0.60, 0.64),
        "dust": (0.13, 0.135, 0.14),
        "paint": (0.50, 0.05, 0.06),
    },
}

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
NAMES = [a for a in args if not a.startswith("--")][0].split(",")
if NAMES == ["all"]:
    NAMES = list(ITEMS)
PAL_NAME = str(opts.get("palette", "echo"))
PAL = {k: np.array(v) for k, v in PALETTES[PAL_NAME].items()}
PREVIEW = opts.get("preview")
NOPAA = bool(opts.get("nopaa", False))


def smooth(x, a, b):
    t = np.clip((x - a) / (b - a), 0.0, 1.0)
    return t * t * (3 - 2 * t)


def box_blur(a, r):
    """Размытие квадратом (2r+1) по суммам."""
    p = np.pad(a, r, mode="edge")
    c = p.cumsum(0).cumsum(1)
    c = np.pad(c, ((1, 0), (1, 0)))
    n = 2 * r + 1
    s = c[n:, n:] - c[:-n, n:] - c[n:, :-n] + c[:-n, :-n]
    return s / (n * n)


def hue_of(rgb):
    mx = rgb.max(-1)
    mn = rgb.min(-1)
    d = mx - mn
    r, g, b = rgb[..., 0], rgb[..., 1], rgb[..., 2]
    h = np.zeros_like(mx)
    m = d > 1e-6
    rm = m & (mx == r)
    gm = m & (mx == g) & ~rm
    bm = m & ~rm & ~gm
    h[rm] = ((g - b)[rm] / d[rm]) % 6
    h[gm] = (b - r)[gm] / d[gm] + 2
    h[bm] = (r - g)[bm] / d[bm] + 4
    return h * 60.0


def load(paa):
    tmp = tempfile.mkdtemp()
    tga = os.path.join(tmp, "src.tga")
    subprocess.run([IMAGE_TO_PAA, paa, tga], capture_output=True)
    im = bpy.data.images.load(tga)
    w, h = im.size
    px = np.empty(w * h * 4, dtype=np.float32)
    im.pixels.foreach_get(px)
    bpy.data.images.remove(im)
    shutil.rmtree(tmp, ignore_errors=True)             # TGA 16 МБ на вещь — не оставлять в TEMP (11-23)
    return px.reshape(h, w, 4).astype(np.float64)


def save_png(arr, path):
    h, w = arr.shape[:2]
    im = bpy.data.images.new(os.path.basename(path), w, h, alpha=True)
    im.pixels.foreach_set(arr.astype(np.float32).ravel())
    im.filepath_raw = path
    im.file_format = "PNG"
    im.save()


def med(L, m, default):
    return float(np.median(L[m > 0.5])) if (m > 0.5).any() else default


def resample(a, h, w):
    """К размеру (h, w) ближайшим соседом (у _smdi бывает другой размер, чем у _co)."""
    if a.shape[:2] == (h, w):
        return a
    return a[np.arange(h) * a.shape[0] // h][:, np.arange(w) * a.shape[1] // w]


def fabric_layer(out, L, prm, smdi_paa, exclude):
    """Слой ткани поверх базы (см. шапку). -> (out, маска F 0..1)."""
    h, w = L.shape
    # блеск, сглаженный — переплетение не рвёт маску; fabric_blur — окно побольше, где у кожи блеск пятнами (ботинки)
    G = box_blur(resample(load(smdi_paa), h, w)[..., 1], prm.get("fabric_blur", 3))
    t0, t1 = prm["fabric"]
    F = 1 - smooth(G, t0, t1)
    if prm.get("dark"):
        a, b = prm["dark"]
        F = F * (1 - smooth(box_blur(L, 3), a, b))
    F = smooth(box_blur(F, 2), 0.3, 0.7)                          # без дыр и одиночных точек, мягкий край 2–3 px
    F = F * (1 - exclude)
    lf = max(med(L, F * (L > 0.02), 0.2), 0.02)                  # пустые (чёрные) места развёртки — не в счёт
    Lb = box_blur(L, 2)
    # крупные складки и потёртости — по исходнику, мелкое переплетение приглушено, светлые точки ограничены
    ratio = np.clip(Lb / lf + 0.3 * (L - Lb) / lf, 0.55, 1.45)
    fab = PAL["fabric"][None, None, :] * ratio[..., None]
    out = out * (1 - F[..., None]) + fab * F[..., None]
    print("  ткань: доля %.2f (с пустыми местами развёртки), медиана яркости ткани исходника %.3f" % (
        (F > 0.5).mean(), lf))
    return out, F


def retex(src, prm, smdi_paa=None):
    rgb = src[..., :3]
    L = 0.299 * rgb[..., 0] + 0.587 * rgb[..., 1] + 0.114 * rgb[..., 2]
    hue = hue_of(rgb)
    chroma = rgb.max(-1) - rgb.min(-1)              # у тёмных пикселей насыщенность шумит — считаем по цветности
    olive = smooth(chroma, 0.012, 0.03) * smooth(hue, 45, 55) * (1 - smooth(hue, 120, 140))
    # кромки накладок: розовато-бурые (оттенок 290–45°, цветность ~0,035, яркость ~0,22); посреди ткани — не кромки
    warm = ((hue > 290) | (hue < 45)).astype(np.float64)
    edge = smooth(chroma, 0.018, 0.04) * smooth(L, 0.13, 0.2) * warm * (1 - olive)
    # окантовка — только тонкие линии кромок: пятна (густые области) и всё рядом с тканью — в общий тон
    line_like = 1 - smooth(box_blur(edge, 6), 0.3, 0.5)
    accent = edge * line_like * (1 - smooth(box_blur(olive, 10), 0.15, 0.35)) * 0.8
    if prm.get("no_accent"):
        accent = accent * 0
    metal = np.clip(1 - olive - accent, 0, 1)
    lf = med(L, olive, 0.22)
    fabric = PAL["base_fabric"][None, None, :] * (L / lf)[..., None]
    la = med(L, accent, 0.22)
    acc_c = PAL["accent"][None, None, :] * np.clip(L / la, 0.5, 1.4)[..., None]
    metal_c = (L * prm.get("metal_k", 0.85))[..., None] * PAL["metal"][None, None, :]
    out = fabric * olive[..., None] + acc_c * accent[..., None] + metal_c * metal[..., None]
    tape = np.zeros_like(L)
    if prm.get("tape"):
        tape = smooth(chroma, 0.06, 0.12) * smooth(hue, 195, 210) * (1 - smooth(hue, 255, 270))
        lt = med(L, tape, 0.3)
        tape_c = PAL["paint"][None, None, :] * 0.8 * np.clip(L / lt, 0.4, 1.1)[..., None]
        out = out * (1 - tape[..., None]) + tape_c * tape[..., None]
        print("  изолента: доля %.4f" % tape.mean())
    # потёртость граней металла до стали: светлее соседей (рёбра, царапины, винты)
    ridge = smooth(L - box_blur(L, 4), 0.03, 0.09) * metal * prm.get("wear", 0.55)
    steel = PAL["steel"][None, None, :] * np.clip(L / max(float(np.median(L)), 1e-3), 0.6, 1.3)[..., None]
    out = out * (1 - ridge[..., None]) + steel * ridge[..., None]
    # углубления: темнее соседей -> нейтральная пыль
    cav = smooth(box_blur(L, 6) - L, 0.015, 0.08) * 0.4
    dust = PAL["dust"][None, None, :] * np.clip(L / lf, 0.3, 1.2)[..., None]
    out = out * (1 - cav[..., None]) + dust * cav[..., None]
    F = None
    if prm.get("fabric") and smdi_paa:
        out, F = fabric_layer(out, L, prm, smdi_paa, tape)        # изолента перчаток — не ткань
    if prm.get("emblem"):
        out = emblem(out, L, *prm["emblem"])
    if prm.get("emblem_geo"):
        out = emblem_geo(out, L)
    res = src.copy()
    res[..., :3] = np.clip(out, 0, 1)
    print("  доли базы: олива %.2f окантовка %.3f металл/прочее %.2f, потёртость %.3f" %
          (olive.mean(), accent.mean(), metal.mean(), (ridge > 0.2).mean()))
    return res, F


def seg_dist(px, py, a, b):
    """Расстояние от точек до отрезка a-b."""
    ax, ay = a
    bx, by = b
    dx, dy = bx - ax, by - ay
    t = np.clip(((px - ax) * dx + (py - ay) * dy) / (dx * dx + dy * dy + 1e-9), 0, 1)
    return np.hypot(px - (ax + t * dx), py - (ay + t * dy))


def emblem(out, L, cx, cy, k):
    """Знак Эхо трафаретной краской: кольцо и линия пульса сквозь него. Потёртость — по шуму и светлым царапинам
    исходника; яркость краски следует исходнику (грязь и вмятины остаются)."""
    h, w = L.shape
    r = int(110 * k)
    y0, y1 = max(0, cy - r), min(h, cy + r)
    x0, x1 = max(0, cx - r), min(w, cx + r)
    yy, xx = np.mgrid[y0:y1, x0:x1].astype(np.float64)        # координаты файла (сверху вниз)
    ring = np.abs(np.hypot(xx - cx, yy - cy) - 62 * k) - 7 * k
    pulse = [(-92, 0), (-34, 0), (-20, -30), (-6, 44), (10, -62), (24, 0), (92, 0)]
    pts = [(cx + px * k, cy + py * k) for px, py in pulse]
    line = np.min([seg_dist(xx, yy, pts[i], pts[i + 1]) for i in range(len(pts) - 1)], axis=0) - 5 * k
    d = np.minimum(ring, line)
    a = np.clip(0.5 - d / 1.5, 0, 1)                             # сглаженный край
    rows = (h - 1) - yy.astype(int)                               # массив снизу вверх
    cols = xx.astype(int)
    out = paint_alpha(out, L, a, rows, cols)
    print("  знак Эхо в (%d, %d)" % (cx, cy))
    return out


def paint_alpha(out, L, a, rows, cols):
    """Трафаретная краска по альфе a (на пикселях rows×cols массива bpy, снизу вверх): потёртость — по шуму и
    светлым царапинам исходника; яркость краски следует исходнику (грязь и вмятины остаются)."""
    rng = np.random.default_rng(7)
    noise = box_blur(rng.random(a.shape), 3)
    lw = L[rows, cols]
    scratch = smooth(lw - box_blur(L, 4)[rows, cols], 0.02, 0.06)
    a = a * smooth(noise, 0.38, 0.55) * (1 - scratch) * 0.9
    lm = float(np.median(lw)) + 1e-3
    paint = PAL["paint"][None, None, :] * np.clip(lw / lm, 0.5, 1.4)[..., None]
    out[rows, cols] = out[rows, cols] * (1 - a[..., None]) + paint * a[..., None]
    return out


def emblem_geo(out, L):
    """Знак Эхо на висках шлема: альфа в пространстве текстуры по геометрии (как в одобренном концепте)."""
    sys.path.insert(0, HERE)
    import ec_helmet_geom as HG
    h, w = L.shape
    A, info = HG.emblem_alpha(h, w)
    A = A[::-1]                                                   # строка 0 — верх файла -> массив bpy снизу вверх
    for side, u0, u1, v0, v1 in info:
        r0, r1 = (h - 1) - v1, (h - 1) - v0
        rows, cols = np.mgrid[r0:r1 + 1, u0:u1 + 1]
        out = paint_alpha(out, L, A[r0:r1 + 1, u0:u1 + 1], rows, cols)
        print("  знак Эхо (висок %+d): UV x %d..%d y %d..%d" % (side, u0, u1, v0, v1))
    return out


for name in NAMES:
    folder, co, prm = ITEMS[name]
    src_paa = os.path.join(folder, co)
    print("ВЕЩЬ", name, src_paa, "палитра", PAL_NAME)
    base = co[:-len(".paa")]
    if base.lower().endswith("_co"):
        base = base[:-len("_co")]
    smdi = os.path.join(folder, base + "_smdi.paa")
    res, F = retex(load(src_paa), prm, smdi if os.path.exists(smdi) else None)
    out_name = base + "_" + PAL_NAME + "_co"                     # исходник не перезаписывается никогда
    assert out_name.lower() + ".paa" != co.lower()
    png_dir = PREVIEW or tempfile.mkdtemp()
    os.makedirs(png_dir, exist_ok=True)
    png = os.path.join(png_dir, out_name + ".png")
    save_png(res, png)
    print("  PNG", png)
    if PREVIEW and F is not None:                                # маска слоя ткани — для проверки
        save_png(np.dstack([F, F, F, np.ones_like(F)]), os.path.join(png_dir, base + "_fabric_mask.png"))
    if not NOPAA:
        paa = os.path.join(folder, out_name + ".paa")
        r = subprocess.run([IMAGE_TO_PAA, png, paa], capture_output=True, text=True)
        print("  PAA", paa, "ok" if os.path.exists(paa) else ("ОШИБКА " + r.stdout + r.stderr))
    if not PREVIEW and not NOPAA:
        shutil.rmtree(png_dir, ignore_errors=True)               # временный PNG больше не нужен
