# -*- coding: utf-8 -*-
"""Лёгкий ретекстур пяти детекторов VB_AnomaliesBosses: новые цвета корпуса и акцентов, маркировка ДА-N.

Место в конвейере: СЛЕДУЮЩИЙ ШАГ ПОСЛЕ tools/ab_detectors.py (и ab_anomalies.py, который кладёт горящую
лампу Отклика). Те пишут в t/d* файлы OOTS (ужатые ab_trim до 1024), этот скрипт их перекрашивает.

  было     стало            папки     вид
  Отклик   «Лоза» ДА-1      do/dox    тёмно-зелёный корпус, жёлтая полоса внизу лицевой панели, янтарная лампа
  Око      «Огонёк» ДА-2    dk/dkx    графитовый корпус, бирюзовый экран, зелёная лампа
  Медведь  «Пеленг» ДА-3    dm/dmx    песочный/хаки корпус, янтарный монохромный экран
  Сварог   «Сито» ДА-4      ds/dsx    чёрный корпус, медные вставки, голубой экран и индикаторы
  Велес    «Невод» ДА-5     dv/dvx    тёмно-синий корпус, белые вставки, экран и метки

Оригиналы. Скрипт всегда читает исходники из «исходники модов/детекторы_оригинал/<папка>/» — это копия
t/dox, dkx, dmx, dsx, dvx, снятая ПОСЛЕ ab_detectors.py + ab_trim и ДО перекраски. Поэтому повторный запуск даёт
тот же результат и не красит уже перекрашенное. Если ab_detectors.py начнёт выдавать другие файлы, копию
нужно обновить сразу после него: «snapshot --force» (команда откажется, если в t/ уже лежит наш результат —
сведения о записанных файлах в _retex_state.json в папке оригиналов).

Что пишет (пути и имена не меняются, формат PAA и размер как у исходника, альфа исходная):
  dox  oklik_defaultl_co, lampa_on_ca, lampa_defaultl_empty_ca
  dkx  okoon_co, okooff_co, oko_ca (у Ока три копии одного атласа: экран вкл/выкл и стекло)
  dmx  medved_default_co, medved_defaulteckangloss_ca, ekranoff, ekran_empty/low/mid/high
  dsx  svarog_default_co, svadog_ekranon
  dvx  veles_default_co, veles_ekranon, veles_ekrangreen (метки)
  .rvmat — только числа цвета там, где материал читается из конфига/скрипта, а значения взяты из OOTS как есть:
       dmx/medved_ekranon (ambient, emmisive), dsx/svadog_ekranon (emmisive), dvx/veles_light (emmisive).
  Материалы, цвет которых задают генераторы (oklik_lampa_on — ab_anomalies.py; svarog_emiss_on, veles_emiss_on,
  medved_ekranoff — ab_detectors.py), здесь НЕ трогаются: нужные числа — в RVMAT_FOR_GENERATORS ниже.
  Карты _nohq/_smdi и стёкла glass_* не меняются.

Как красит: маска по цвету/насыщенности исходника (плюс прямоугольники в координатах развёртки 1024),
перенос оттенка с сохранением яркости (детализация, износ, царапины остаются), экраны — градиент по яркости.
Старые надписи с названиями (ОТКЛИК, ОКО, МЕДВЕДЬ, SVAROG, VELES) затираются заливкой с зерном и на их место
наносится ДА-N простым штриховым шрифтом (без PIL).

Запуск (нужен numpy — Python из Blender):
  python -I tools/ab_retex_detectors.py [build] [--only do,dk,...] [--tmp DIR] [--preview DIR]
  python -I tools/ab_retex_detectors.py snapshot [--force]
Превью «было | стало» — PNG в папке --preview (по умолчанию %TEMP%/vb_ab_retex/preview).
"""
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ab_models as am  # noqa: E402
import ab_tex as at  # noqa: E402

T = os.path.join(am.MOD, "t")
ORIG = os.path.join(am.SRC, "детекторы_оригинал")
STATE = os.path.join(ORIG, "_retex_state.json")
DIRS = ["dox", "dkx", "dmx", "dsx", "dvx"]

# Числа для материалов, которые пишут генераторы (передать в ab_anomalies.py / ab_detectors.py):
RVMAT_FOR_GENERATORS = {
    "dox/oklik_lampa_on.rvmat (ab_anomalies.py)": {"emmisive": "2.5,1.1,0,1"},      # янтарная лампа
    "dsx/svarog_emiss_on.rvmat (ab_detectors.py)": {"emmisive": "0.2,0.75,1.5,1"},  # голубые индикаторы
    "dvx/veles_emiss_on.rvmat (ab_detectors.py)": {"emmisive": "1.1,1.1,1.2,1"},    # белые индикаторы
}
# Материалы из OOTS, которые читаются по имени (hiddenSelectionsMaterials / SetObjectMaterial), меняем здесь.
RVMAT_EDITS = {
    ("dmx", "medved_ekranon"): {"ambient": "150,150,0,1", "emmisive": "1,1,0,1"},   # янтарь: R=G, B=0
    ("dsx", "svadog_ekranon"): {"emmisive": "0.5,1.6,2.2,1"},                      # голубые линии
    ("dvx", "veles_light"): {"emmisive": "1.3,1.3,1.4,1"},                         # белые линии экрана
}

W = np.array([0.30, 0.59, 0.11], np.float32)


# ================================================================== файлы
def stem_of(fname):
    """oklik_defaultl_co_____.paa -> oklik_defaultl_co (имена в моде дополнены «_» до длины пути OOTS)."""
    return os.path.splitext(fname)[0].rstrip("_").lower()


def find(base, d, stem, ext=".paa"):
    hits = [f for f in os.listdir(os.path.join(base, d))
            if f.lower().endswith(ext) and stem_of(f) == stem]
    if len(hits) != 1:
        raise SystemExit("%s/%s: файл %s%s найден %d раз" % (base, d, stem, ext, len(hits)))
    return hits[0]


def sha(path):
    with open(path, "rb") as f:
        return hashlib.sha256(f.read()).hexdigest()


def paa_dim(path):
    d = open(path, "rb").read(4096)
    i = 2
    while d[i:i + 4] == b"GGAT":
        i += 12 + int.from_bytes(d[i + 8:i + 12], "little")
    i += 2 + int.from_bytes(d[i:i + 2], "little") * 3
    return int.from_bytes(d[i:i + 2], "little") & 0x7FFF, int.from_bytes(d[i + 2:i + 4], "little")


class Ctx:
    def __init__(self, tmp, preview):
        self.tmp = tmp
        self.preview = preview
        self.written = {}       # rel -> sha256
        self.checks = []        # строки отчёта


def read_paa(path, tmp, tag):
    os.makedirs(tmp, exist_ok=True)
    tga = os.path.join(tmp, tag + ".tga")
    if os.path.exists(tga):
        os.remove(tga)
    subprocess.run([am.IMAGE_TO_PAA, path, tga], capture_output=True, text=True)
    if not os.path.exists(tga):
        raise RuntimeError("ImageToPAA не смог прочитать " + path)
    return at.read_tga(tga)


def load(ctx, d, stem):
    fn = find(ORIG, d, stem)
    src = os.path.join(ORIG, d, fn)
    rgb, a = read_paa(src, os.path.join(ctx.tmp, "src"), d + "_" + stem)
    return dict(d=d, stem=stem, fn=fn, src=src, rgb=rgb, a=a, fmt=at.paa_format(src))


def save(ctx, tex, out):
    """Перекрашенное -> t/<папка>/<то же имя>. Формат как у исходника (суффикс временного png), альфа исходная."""
    dst = os.path.join(T, tex["d"], tex["fn"])
    at.save_paa(np.clip(out, 0, 1), tex["a"], dst, tex["d"] + "_" + tex["stem"], tex["fmt"], ctx.tmp,
                am.png_to_paa)
    fmt = at.paa_format(dst)
    dim_src, dim_dst = paa_dim(tex["src"]), paa_dim(dst)
    _, a2 = read_paa(dst, os.path.join(ctx.tmp, "check"), tex["d"] + "_" + tex["stem"])
    if tex["fmt"] == "DXT1":
        bad_a = int(((a2 < 128) != (tex["a"] < 128)).sum())
        a_note = "1-бит альфа: несовпадений %d" % bad_a
    else:
        bad_a = int(np.abs(a2.astype(int) - tex["a"].astype(int)).max())
        a_note = "альфа: макс. отклонение %d/255" % bad_a
    ok = fmt == tex["fmt"] and dim_src == dim_dst
    ctx.checks.append("%s %s/%s: %s %dx%d -> %s %dx%d, %s" % (
        "ok" if ok else "!!", tex["d"], tex["fn"], tex["fmt"], dim_src[0], dim_src[1], fmt,
        dim_dst[0], dim_dst[1], a_note))
    if not ok:
        raise SystemExit("формат/размер не совпали: " + dst)
    ctx.written[tex["d"] + "/" + tex["fn"]] = sha(dst)
    return np.clip(out, 0, 1)


def edit_rvmat(ctx, d, stem, values):
    fn = find(ORIG, d, stem, ".rvmat")
    txt = open(os.path.join(ORIG, d, fn), encoding="utf-8").read()
    for key, val in values.items():
        txt, n = re.subn(r"(?m)^(\s*)%s\[\]\s*=\s*\{[^}]*\};" % re.escape(key),
                         lambda m: "%s%s[]={%s};" % (m.group(1), key, val), txt)
        if n != 1:
            raise SystemExit("в %s/%s нет строки %s[]" % (d, fn, key))
    dst = os.path.join(T, d, fn)
    with open(dst, "w", encoding="utf-8", newline="\r\n") as f:
        f.write(txt.replace("\r\n", "\n"))
    ctx.written[d + "/" + fn] = sha(dst)
    ctx.checks.append("ok %s/%s: %s" % (d, fn, ", ".join("%s={%s}" % kv for kv in values.items())))


# ================================================================== цвет
def lum(rgb):
    return rgb @ W


def sstep(e0, e1, x):
    t = np.clip((np.asarray(x, np.float32) - e0) / (e1 - e0), 0, 1)
    return t * t * (3 - 2 * t)


def hsv(rgb):
    r, g, b = rgb[..., 0], rgb[..., 1], rgb[..., 2]
    mx = rgb.max(-1)
    mn = rgb.min(-1)
    d = mx - mn
    dd = np.maximum(d, 1e-6)
    h = np.where(mx == r, ((g - b) / dd) % 6, np.where(mx == g, (b - r) / dd + 2, (r - g) / dd + 4)) * 60
    h = np.where(d < 1e-6, 0, h).astype(np.float32)
    s = np.where(mx > 1e-6, d / np.maximum(mx, 1e-6), 0).astype(np.float32)
    return h, s, mx


def hsv2rgb(h, s, v):
    h6 = (h % 360) / 60.0
    i = np.floor(h6).astype(int) % 6
    f = h6 - np.floor(h6)
    p, q, t = v * (1 - s), v * (1 - s * f), v * (1 - s * (1 - f))
    r = np.choose(i, [v, q, p, p, t, v])
    g = np.choose(i, [t, v, v, q, p, p])
    b = np.choose(i, [p, p, t, v, v, q])
    return np.stack([r, g, b], -1).astype(np.float32)


def hue_w(h, lo, hi, soft=8.0):
    """1 внутри дуги оттенков [lo, hi] (градусы, по кругу), к 0 за soft градусов."""
    c, half = (lo + hi) / 2.0, (hi - lo) / 2.0
    dh = np.abs(((h - c + 180) % 360) - 180)
    return 1 - sstep(half, half + soft, dh)


def box(img, r):
    """Размытие окном (2r+1), края повторяются (без заворота, как у FFT)."""
    if r <= 0:
        return img
    k = 2 * r + 1
    p = np.pad(img, ((r + 1, r), (0, 0)) + ((0, 0),) * (img.ndim - 2), mode="edge")
    c = np.cumsum(p, 0, dtype=np.float64)
    img = ((c[k:] - c[:-k]) / k).astype(np.float32)
    p = np.pad(img, ((0, 0), (r + 1, r)) + ((0, 0),) * (img.ndim - 2), mode="edge")
    c = np.cumsum(p, 1, dtype=np.float64)
    return ((c[:, k:] - c[:, :-k]) / k).astype(np.float32)


def soft(m, r=1):
    return box(box(m.astype(np.float32), r), r)


def rects(shape, *rs, feather=1):
    m = np.zeros(shape[:2], np.float32)
    for x0, y0, x1, y1 in rs:
        m[y0:y1, x0:x1] = 1
    return soft(m, feather) if feather else m


def circle(shape, cx, cy, r, feather=1.5):
    yy, xx = np.mgrid[0:shape[0], 0:shape[1]]
    d = np.hypot(xx + 0.5 - cx, yy + 0.5 - cy)
    return (1 - sstep(r - feather, r + feather, d)).astype(np.float32)


def mix(a, b, m):
    m = np.asarray(m, np.float32)
    if m.ndim == a.ndim - 1:
        m = m[..., None]
    return a + (b - a) * m


def tint(rgb, color, ref):
    """Перенос оттенка: цвет color получает пиксель с яркостью ref, остальные — пропорционально своей яркости."""
    color = np.asarray(color, np.float32)
    lc = float(color @ W)
    return (color / lc)[None, None, :] * (lum(rgb) * (lc / ref))[..., None]


def desat(rgb, m):
    """Убрать цвет пропорционально маске (чтобы на краю маски не оставалась кайма исходного оттенка)."""
    return mix(rgb, np.repeat(lum(rgb)[..., None], 3, -1), m)


def ref_lum(rgb, m, thr=0.8):
    sel = lum(rgb)[m > thr]
    return float(np.median(sel)) if sel.size else 0.5


def ramp(t, stops):
    return at.apply_ramp(np.clip(t, 0, 1), stops)


# ================================================================== затирание старых надписей
def inpaint(rgb, mask, src_shift=None, iters=500, grain=1.0):
    """Залить пиксели mask гладко по соседям (диффузия), затем вернуть зерно — высокие частоты из
    соседнего чистого участка (сдвиг src_shift = (dx, dy))."""
    ys, xs = np.nonzero(mask)
    if not len(xs):
        return rgb
    h, w = mask.shape
    y0, y1 = max(0, ys.min() - 3), min(h, ys.max() + 4)
    x0, x1 = max(0, xs.min() - 3), min(w, xs.max() + 4)
    sub = rgb[y0:y1, x0:x1].copy()
    m = mask[y0:y1, x0:x1]
    sub[m] = sub[~m].mean(0)
    for _ in range(iters):
        p = np.pad(sub, ((1, 1), (1, 1), (0, 0)), mode="edge")
        avg = (p[:-2, 1:-1] + p[2:, 1:-1] + p[1:-1, :-2] + p[1:-1, 2:]) * 0.25
        sub[m] = avg[m]
    if src_shift and grain:
        dx, dy = src_shift
        hp = rgb - box(box(rgb, 2), 2)
        src = hp[y0 + dy:y1 + dy, x0 + dx:x1 + dx]
        if src.shape == sub.shape:
            sub[m] += src[m] * grain
    out = rgb.copy()
    out[y0:y1, x0:x1] = sub
    return out


def dilate(m, r=1):
    return soft(m.astype(np.float32), r) > 0.01


# ================================================================== штриховой шрифт ДА-N
# Координаты: x — вправо (0..ширина), y — вниз (0 — верх буквы, 1 — линия строки), ширина в долях высоты.
GLYPHS = {
    "Д": (0.95, [[(0.28, 0.0), (0.80, 0.0), (0.80, 1.0)],
                 [(0.28, 0.0), (0.25, 0.6), (0.12, 1.0)],
                 [(0.0, 1.18), (0.0, 1.0), (0.95, 1.0), (0.95, 1.18)]]),
    "А": (0.85, [[(0.0, 1.0), (0.425, 0.0), (0.85, 1.0)], [(0.17, 0.64), (0.68, 0.64)]]),
    "-": (0.50, [[(0.06, 0.56), (0.44, 0.56)]]),
    "1": (0.50, [[(0.05, 0.22), (0.32, 0.0), (0.32, 1.0)]]),
    "2": (0.70, [[(0.04, 0.2), (0.12, 0.06), (0.32, 0.0), (0.54, 0.05), (0.66, 0.2), (0.62, 0.38),
                  (0.04, 1.0), (0.70, 1.0)]]),
    "3": (0.70, [[(0.04, 0.1), (0.20, 0.0), (0.46, 0.0), (0.62, 0.1), (0.64, 0.26), (0.52, 0.42),
                  (0.30, 0.47)],
                 [(0.30, 0.47), (0.56, 0.53), (0.68, 0.70), (0.62, 0.90), (0.42, 1.0), (0.18, 1.0),
                  (0.02, 0.9)]]),
    "4": (0.75, [[(0.56, 1.0), (0.56, 0.0), (0.0, 0.70), (0.75, 0.70)]]),
    "5": (0.70, [[(0.64, 0.0), (0.12, 0.0), (0.06, 0.45), (0.30, 0.38), (0.52, 0.42), (0.66, 0.60),
                  (0.66, 0.80), (0.52, 0.96), (0.30, 1.0), (0.04, 0.92)]]),
}


def text_len(text, h, wscale=1.0, gap=0.22):
    return h * (sum(GLYPHS[c][0] * wscale for c in text) + gap * (len(text) - 1))


def text_cover(shape, text, x0, y0, h, stroke=0.14, wscale=1.0, gap=0.22, rot=False):
    """Покрытие 0..1 надписи. rot=False: (x0, y0) — левый верх строки. rot=True — строка повёрнута на 90° по
    часовой (читается сверху вниз, верх букв смотрит вправо, как SVAROG/VELES/ОКО на развёртке): (x0, y0) —
    правый верх полосы надписи."""
    segs = []
    u = 0.0
    for c in text:
        gw, strokes = GLYPHS[c]
        for line in strokes:
            for (ax, ay), (bx, by) in zip(line[:-1], line[1:]):
                segs.append(((u + ax * wscale * h), ay * h, (u + bx * wscale * h), by * h))
        u += (gw * wscale + gap) * h
    yy, xx = np.mgrid[0:shape[0], 0:shape[1]].astype(np.float32) + 0.5
    if rot:
        tu, tv = yy - y0, x0 - xx          # вдоль строки — вниз, «вниз по букве» — влево
    else:
        tu, tv = xx - x0, yy - y0
    d = np.full(shape[:2], 1e9, np.float32)
    for ax, ay, bx, by in segs:
        vx, vy = bx - ax, by - ay
        ll = vx * vx + vy * vy + 1e-9
        t = np.clip(((tu - ax) * vx + (tv - ay) * vy) / ll, 0, 1)
        d = np.minimum(d, np.hypot(tu - ax - t * vx, tv - ay - t * vy))
    return np.clip(stroke * h / 2 + 0.5 - d, 0, 1).astype(np.float32)


def draw_text(rgb, cover, color, alpha=0.92):
    """Краска надписи с зерном поверхности под ней (не плоская заливка)."""
    L = lum(rgb)
    g = np.clip((L + 0.02) / (box(L, 3) + 0.02), 0.6, 1.4) ** 0.7
    col = np.asarray(color, np.float32)[None, None, :] * g[..., None]
    return mix(rgb, np.clip(col, 0, 1), cover * alpha)


# ================================================================== детекторы
def otklik(ctx, pv):
    """«Лоза» ДА-1: жёлтая краска -> тёмно-зелёная; нижняя полоса лицевой панели остаётся жёлтой (с маркировкой),
    табличка шкалы — светлая (надписи читаются), красная головка — янтарная; лампа — янтарная."""
    GREEN = (0.14, 0.22, 0.115)
    CREAM = (0.74, 0.70, 0.56)
    AMBER_KNOB = (0.85, 0.50, 0.10)
    STRIPE = (0, 946, 216, 1024)        # низ лицевой панели под табличкой
    PLATE = (22, 835, 195, 948)         # табличка шкалы
    KNOB = (405, 110, 505, 210)         # красная головка
    TXT = (62, 953, 154, 975)           # «ОТКЛИК»
    SUB = (38, 977, 180, 986)           # «детектор аномальной активности»

    tex = load(ctx, "dox", "oklik_defaultl_co")
    rgb = tex["rgb"]
    # 1) затереть название и подпись (тёмные штрихи на жёлтом)
    L = lum(rgb)
    dark = L < 0.86 * box(L, 6)
    m = np.zeros_like(dark)
    for x0, y0, x1, y1 in (TXT, SUB):
        m[y0:y1, x0:x1] = dark[y0:y1, x0:x1]
    m = dilate(m, 1) & (rects(rgb.shape, TXT, SUB, feather=0) > 0)
    work = inpaint(rgb, m, src_shift=(0, 38))
    # 2) перекраска
    h, s, v = hsv(work)
    paint = hue_w(h, 26, 58, 10) * sstep(0.20, 0.40, s) * sstep(0.08, 0.20, v)
    stripe = rects(rgb.shape, STRIPE)
    plate = rects(rgb.shape, PLATE)
    ref = ref_lum(work, paint * (1 - stripe) * (1 - plate))
    m_green = paint * (1 - stripe) * (1 - plate)
    m_cream = paint * plate
    out = desat(work, (m_green + m_cream) * 0.9)
    out = mix(out, tint(work, GREEN, ref), m_green)
    out = mix(out, tint(work, CREAM, ref), m_cream)
    knob = (hue_w(h, -25, 12, 8) * sstep(0.30, 0.50, s) * rects(rgb.shape, KNOB))
    out = mix(out, tint(work, AMBER_KNOB, ref_lum(work, knob)), knob)
    # 3) маркировка на жёлтой полосе (тёмная, как была «ОТКЛИК»)
    hgt = 18
    tl = text_len("ДА-1", hgt)
    cov = text_cover(rgb.shape, "ДА-1", 108 - tl / 2, 954, hgt)
    out = draw_text(out, cov, (0.10, 0.08, 0.04))
    res = save(ctx, tex, out)
    pv.append(("atlas", rgb, res))
    pv.append(("crop", rgb[800:1024, 0:216], res[800:1024, 0:216]))

    # лампа: колпак (прямоугольник слева) и красные ободки/донышки
    INNER = (0, 22, 602, 964)
    for stem, on in (("lampa_on_ca", True), ("lampa_defaultl_empty_ca", False)):
        tx = load(ctx, "dox", stem)
        rgb = tx["rgb"]
        h, s, v = hsv(rgb)
        inner = rects(rgb.shape, INNER, feather=0)
        rims = hue_w(h, -20, 18, 8) * sstep(0.20, 0.35, s) * (1 - inner)
        out = desat(rgb, rims * 0.9)
        out = mix(out, tint(rgb, (0.62, 0.36, 0.10), ref_lum(rgb, rims)), rims)
        if on:      # бирюза -> янтарь
            out = mix(out, tint(rgb, (1.0, 0.62, 0.12), ref_lum(rgb, inner)), inner)
        else:       # матовое стекло -> тёплое
            out = mix(out, tint(rgb, (0.92, 0.84, 0.70), ref_lum(rgb, inner)), inner)
        res = save(ctx, tx, out)
        pv.append(("tile", rgb, res))


def oko_atlas(rgb, screen_on):
    """«Огонёк»: графит, бирюзовый экран, зелёная лампа. Одинаково для okoon/okooff/oko_ca, кроме экрана:
    у okooff он погасший (тёмный, приглушённый)."""
    SCREEN = (375, 528, 505, 638)
    LAMPS = (728, 692, 890, 932)
    LABEL = (595, 602, 621, 643)    # «ОКО» в окошке лимба
    # затереть оранжевое «ОКО»
    h, s, v = hsv(rgb)
    lab = (s > 0.28) & (hue_w(h, 12, 55, 6) > 0.5) & (rects(rgb.shape, LABEL, feather=0) > 0)
    work = inpaint(rgb, dilate(lab, 1) & (rects(rgb.shape, LABEL, feather=0) > 0), src_shift=(-14, 0))
    h, s, v = hsv(work)
    L = lum(work)
    excl = np.maximum(rects(rgb.shape, SCREEN, feather=2), rects(rgb.shape, LAMPS, feather=2))
    vivid = sstep(0.55, 0.78, s) * sstep(0.28, 0.45, v)
    graph = tint(work, (0.215, 0.225, 0.24), 0.20)
    out = mix(work, graph, (1 - excl) * (1 - 0.9 * vivid) * 0.9)
    # экран
    scr = rects(rgb.shape, SCREEN, feather=1)
    if screen_on:
        stops = [(0.0, (0.0, 0.03, 0.035)), (0.22, (0.02, 0.17, 0.17)), (0.45, (0.10, 0.50, 0.50)),
                 (0.62, (0.30, 0.80, 0.78)), (0.85, (0.70, 0.98, 0.95)), (1.0, (0.92, 1.0, 1.0))]
    else:
        stops = [(0.0, (0.0, 0.01, 0.012)), (0.3, (0.045, 0.10, 0.10)), (0.6, (0.12, 0.22, 0.22)),
                 (1.0, (0.40, 0.52, 0.52))]
    out = mix(out, ramp(L / 0.85, stops), scr)
    # лампа: голубое/синее -> зелёное
    lamp = rects(rgb.shape, LAMPS, feather=1) * sstep(0.12, 0.25, s) * hue_w(h, 150, 265, 10)
    nh = 122 + (h - 180) * 0.25
    out = mix(out, hsv2rgb(nh, s, v), lamp)
    # маркировка ДА-2 в окошке (бирюзовая, повёрнута как было «ОКО»)
    hgt = 12
    tl = text_len("ДА-2", hgt, wscale=0.72, gap=0.14)
    cov = text_cover(rgb.shape, "ДА-2", 615, 622.5 - tl / 2, hgt, stroke=0.18, wscale=0.72, gap=0.14, rot=True)
    out = draw_text(out, cov, (0.02, 0.26, 0.24), alpha=0.97)
    return out


def oko(ctx, pv):
    for stem in ("okoon_co", "okooff_co", "oko_ca"):
        tex = load(ctx, "dkx", stem)
        res = save(ctx, tex, oko_atlas(tex["rgb"], stem != "okooff_co"))
        if stem == "okoon_co":
            pv.append(("atlas", tex["rgb"], res))
            pv.append(("crop", tex["rgb"][480:940, 360:900], res[480:940, 360:900]))
        elif stem == "okooff_co":
            pv.append(("crop", tex["rgb"][480:700, 360:700], res[480:700, 360:700]))


AMBER_STOPS = [(0.0, (0.01, 0.005, 0.0)), (0.25, (0.17, 0.08, 0.005)), (0.5, (0.50, 0.27, 0.03)),
               (0.78, (0.88, 0.55, 0.10)), (1.0, (1.0, 0.80, 0.36))]


def medved_atlas(rgb):
    """«Пеленг»: оранжевая краска -> песочный/хаки, ЖК и лимб — янтарный монохром, МЕДВЕДЬ -> ДА-3."""
    SAND = (0.50, 0.46, 0.31)
    LCD = (395, 8, 535, 128)
    LED = (851, 89, 16)                 # красная лампочка на панели — остаётся
    PEACH = (0, 966, 110, 1010)         # персиковая плашка внизу слева — остаётся
    PLATE_TXT = (428, 160, 508, 196)    # «МЕДВЕДЬ» на светлой крышке
    PANEL_TXT = (727, 38, 795, 70)      # «МЕДВЕДЬ» и подпись на панели (светлые буквы с тёмной обводкой)
    COVER = (338, 136, 602, 404)        # светлая крышка: на ней бледные оранжевые крапины
    h, s, v = hsv(rgb)
    # затереть надписи: на крышке — по маске цвета, на панели — весь прямоугольник (обводка букв тёмная)
    m_plate = (s > 0.16) & (rects(rgb.shape, PLATE_TXT, feather=0) > 0)
    work = inpaint(rgb, dilate(m_plate, 1) & (rects(rgb.shape, PLATE_TXT, feather=0) > 0), src_shift=(0, 60))
    # зерно — с чистого места большой тёмной крышки слева (100..168, 200..232)
    work = inpaint(work, rects(rgb.shape, PANEL_TXT, feather=0) > 0, src_shift=(-627, 162), iters=800)
    h, s, v = hsv(work)
    L = lum(work)
    sat_lo = 0.58 - 0.36 * rects(rgb.shape, COVER, feather=0)
    keep = np.maximum(circle(rgb.shape, *LED), rects(rgb.shape, PEACH, feather=0))
    core = hue_w(h, -14, 36, 8) * sstep(sat_lo, sat_lo + 0.16, s) * sstep(0.32, 0.50, v)
    # кайма: у края краски пиксели «оранжевый + серый металл» менее насыщены — берём их, если рядом ядро
    near = soft((core > 0.5).astype(np.float32), 2) > 0.02
    fringe = near * hue_w(h, -20, 45, 10) * sstep(0.10, 0.28, s)
    orange = np.maximum(core, fringe) * (1 - keep)
    out = desat(work, orange * 0.9)
    out = mix(out, tint(work, SAND, ref_lum(work, orange)), orange)
    # ЖК и круглый лимб — янтарный монохром
    lcd = rects(rgb.shape, LCD, feather=1)
    dial = circle(rgb.shape, 820, 415, 127)
    mono = np.maximum(lcd, dial)
    out = mix(out, ramp(L / 0.62, AMBER_STOPS), mono)
    # маркировка
    hgt = 24
    tl = text_len("ДА-3", hgt)
    out = draw_text(out, text_cover(rgb.shape, "ДА-3", 468 - tl / 2, 163, hgt), (0.24, 0.21, 0.13))
    hgt = 16
    tl = text_len("ДА-3", hgt)
    out = draw_text(out, text_cover(rgb.shape, "ДА-3", 762 - tl / 2, 41, hgt), (0.82, 0.75, 0.56))
    return out


def medved(ctx, pv):
    for stem in ("medved_default_co", "medved_defaulteckangloss_ca"):
        tex = load(ctx, "dmx", stem)
        res = save(ctx, tex, medved_atlas(tex["rgb"]))
        if stem == "medved_default_co":
            pv.append(("atlas", tex["rgb"], res))
            pv.append(("crop", tex["rgb"][0:560, 330:960], res[0:560, 330:960]))
        else:
            pv.append(("tile", tex["rgb"], res))
    for stem in ("ekran_empty", "ekran_low", "ekran_mid", "ekran_high", "ekranoff"):
        tex = load(ctx, "dmx", stem)
        L = lum(tex["rgb"])
        if stem == "ekranoff":      # погасший ЖК: тёмный тёплый
            out = ramp(L / 0.55, [(0.0, (0.0, 0.0, 0.0)), (0.5, (0.17, 0.12, 0.06)), (1.0, (0.36, 0.27, 0.14))])
        else:
            out = ramp(L / 0.75, AMBER_STOPS)
        res = save(ctx, tex, out)
        if stem in ("ekran_high", "ekranoff"):
            pv.append(("crop", tex["rgb"][0:560, 380:960], res[0:560, 380:960]))


BUTTON = (305, 740, 44, 2)              # красная кнопка пульта Сварога/Велеса (центр, радиус, край)
PLATE_TXT = (198, 801, 225, 895)        # столбец букв SVAROG/VELES на шильдике


def wipe_plate(rgb, pct=70, win=14):
    """Шильдик Сварога/Велеса: выпуклые буквы с бликами по маске яркости не вычищаются, поэтому столбец букв
    заменяется фоном: в каждом столбце пикселей — скользящий процентиль вдоль строки (буквы тёмные и узкие,
    фон светлее), так остаётся поперечная светотень выпуклой таблички. Зерно — с чистого металла крышки под
    экраном (254..442, 254..482)."""
    from numpy.lib.stride_tricks import sliding_window_view
    x0, y0, x1, y1 = PLATE_TXT
    region = rgb[y0 - win:y1 + win, x0:x1]
    sw = sliding_window_view(region, 2 * win + 1, axis=0)          # (y1-y0, w, 3, окно)
    bg = np.percentile(sw, pct, axis=-1).astype(np.float32)
    bg = box(bg, 1)
    hp = rgb - box(box(rgb, 2), 2)
    bg += hp[y0 - 500:y1 - 500, x0 + 72:x1 + 72] * 0.8
    m = rects(rgb.shape, PLATE_TXT, feather=1)
    full = rgb.copy()
    full[y0:y1, x0:x1] = bg
    return mix(rgb, full, m)


def svarog(ctx, pv):
    """«Сито» ДА-4: корпус чёрный, лицевые вставки (крышка под экраном, пульт, шильдик) — медь, рамки
    индикаторов — голубые, экран — голубые линии."""
    COPPER = (0.66, 0.38, 0.23)
    SQUARE = (254, 254, 442, 482)
    PANEL = (248, 628, 412, 964)
    PLATE = (180, 794, 232, 902)
    FRAMES = (198, 333, 230, 622)
    tex = load(ctx, "dsx", "svarog_default_co")
    rgb = tex["rgb"]
    work = wipe_plate(rgb)
    h, s, v = hsv(work)
    L = lum(work)
    button = circle(rgb.shape, *BUTTON)
    # чёрный: без ржавого оттенка, темнее; красная кнопка остаётся
    out = mix(work, tint(work, (0.20, 0.205, 0.215), 0.235), 0.92 * (1 - button))
    copper = rects(rgb.shape, SQUARE, PANEL, PLATE, feather=1) * (1 - button)
    out = mix(out, tint(work, COPPER, 0.49), copper)
    frames = rects(rgb.shape, FRAMES, feather=1) * sstep(0.18, 0.32, L)
    out = mix(out, tint(work, (0.45, 0.72, 0.92), ref_lum(work, frames, 0.5)), frames)
    hgt = 21
    tl = text_len("ДА-4", hgt)
    out = draw_text(out, text_cover(rgb.shape, "ДА-4", 222, 848 - tl / 2, hgt, rot=True), (0.10, 0.055, 0.03))
    res = save(ctx, tex, out)
    pv.append(("atlas", rgb, res))
    pv.append(("crop", rgb[600:1000, 160:440], res[600:1000, 160:440]))

    tex = load(ctx, "dsx", "svadog_ekranon")
    t = tex["rgb"].max(-1)
    res = save(ctx, tex, t[..., None] * np.array([0.45, 0.80, 1.0], np.float32))
    pv.append(("crop", tex["rgb"][0:270, 230:460], res[0:270, 230:460]))


def veles(ctx, pv):
    """«Невод» ДА-5: корпус тёмно-синий, оранжевые вставки (пульт, рамки, полосы, шильдик) — белые,
    экран — белые линии, метки — белые (с поправкой на встроенный материал, см. VELES_MARK)."""
    NAVY = (0.105, 0.135, 0.235)
    WHITE = (0.86, 0.87, 0.88)
    ACCENT = [(198, 333, 230, 622), (248, 628, 412, 964), (860, 500, 1000, 1024), (180, 794, 232, 902)]
    tex = load(ctx, "dvx", "veles_default_co")
    rgb = tex["rgb"]
    work = wipe_plate(rgb)
    h, s, v = hsv(work)
    L = lum(work)
    button = circle(rgb.shape, *BUTTON)
    dark = (1 - sstep(0.16, 0.34, L)) * (1 - button)
    out = mix(work, tint(work, NAVY, 0.15), dark * 0.92)
    # светлый металл — холодная сталь (не синий)
    out = mix(out, tint(work, (0.50, 0.515, 0.545), 0.5), (1 - dark) * (1 - button) * 0.7)
    acc = rects(rgb.shape, *ACCENT, feather=1) * hue_w(h, 24, 56, 8) * sstep(0.40, 0.58, s) * sstep(0.18, 0.32, v)
    out = desat(out, acc * 0.9)
    out = mix(out, tint(work, WHITE, ref_lum(work, acc)), acc)
    # шильдик (тёмное золото) — белый целиком, по прямоугольнику
    plate = rects(rgb.shape, (182, 797, 230, 899), feather=1)
    out = mix(out, tint(work, (0.80, 0.81, 0.82), ref_lum(work, plate)), plate)
    hgt = 21
    tl = text_len("ДА-5", hgt)
    out = draw_text(out, text_cover(rgb.shape, "ДА-5", 222, 848 - tl / 2, hgt, rot=True), (0.07, 0.10, 0.22))
    res = save(ctx, tex, out)
    pv.append(("atlas", rgb, res))
    pv.append(("crop", rgb[300:1000, 160:440], res[300:1000, 160:440]))

    tex = load(ctx, "dvx", "veles_ekranon")
    t = tex["rgb"].max(-1)
    res = save(ctx, tex, t[..., None] * np.array([0.88, 0.94, 1.0], np.float32))
    pv.append(("crop", tex["rgb"][0:270, 230:460], res[0:270, 230:460]))
    tex = load(ctx, "dvx", "veles_ekrangreen")
    t = tex["rgb"].max(-1)
    t = t / max(float(np.median(t)), 1e-3)
    res = save(ctx, tex, np.clip(t[..., None] * np.array(VELES_MARK, np.float32), 0, 1))
    pv.append(("tile", tex["rgb"], res))


# Метки Велеса: свечение = текстура × встроенный в модель emmisive (материал veles_light внутри p3d, файлом не
# перекрывается). В OOTS он {1.07, 1.36, 0}; с 2026-11-13 ab_detectors.py (MODEL_PATCHES) ставит в модель
# {1.2, 1.2, 1.2} — равные каналы, поэтому текстура меток просто белая.
VELES_MARK = (1.0, 1.0, 1.0)


DETECTORS = dict(do=otklik, dk=oko, dm=medved, ds=svarog, dv=veles)


# ================================================================== превью
def thumb(img, size):
    h, w = img.shape[:2]
    k = max(1, int(np.ceil(max(h, w) / float(size))))
    hh, ww = h // k * k, w // k * k
    return img[:hh, :ww].reshape(hh // k, k, ww // k, k, -1).mean((1, 3))


def upscale(img, size):
    h, w = img.shape[:2]
    k = max(1, size // max(h, w))
    return np.repeat(np.repeat(img, k, 0), k, 1)


def write_preview(path, pv):
    """Ряд «атлас было | стало» 2×512, ниже пары «было | стало» для экранов, ламп и фрагментов с надписями."""
    BG = np.array([0.16, 0.16, 0.17], np.float32)
    rows = []
    cur, cur_w = [], 0
    for kind, a, b in pv:
        if kind == "atlas":
            pair = [thumb(a, 512), thumb(b, 512)]
        elif kind == "tile":
            pair = [thumb(a, 256), thumb(b, 256)]
        else:
            big = max(a.shape[:2])
            pair = [upscale(a, 400), upscale(b, 400)] if big < 200 else [thumb(a, 400), thumb(b, 400)]
        pw = sum(p.shape[1] for p in pair) + 8
        if kind == "atlas" or cur_w + pw > 1040:
            if cur:
                rows.append(cur)
            cur, cur_w = [], 0
        cur.append(pair)
        cur_w += pw + 24
        if kind == "atlas":
            rows.append(cur)
            cur, cur_w = [], 0
    if cur:
        rows.append(cur)
    width = max(sum(sum(p.shape[1] for p in pair) + 8 + 24 for pair in r) for r in rows)
    blocks = []
    for r in rows:
        hgt = max(p.shape[0] for pair in r for p in pair)
        blk = np.tile(BG, (hgt + 16, width, 1))
        x = 8
        for pair in r:
            for i, p in enumerate(pair):
                blk[8:8 + p.shape[0], x:x + p.shape[1]] = p[..., :3]
                x += p.shape[1] + (8 if i == 0 else 24)
        blocks.append(blk)
    at.write_png(path, at.to_u8(np.concatenate(blocks, 0)))


# ================================================================== команды
def load_state():
    if os.path.exists(STATE):
        return json.load(open(STATE, encoding="utf-8"))
    return {}


def cmd_snapshot(force):
    state = load_state()
    ours = set((state.get("outputs") or {}).values())
    hit = []
    for d in DIRS:
        for f in os.listdir(os.path.join(T, d)):
            if sha(os.path.join(T, d, f)) in ours:
                hit.append(d + "/" + f)
    if hit:
        raise SystemExit("в t/ уже лежит результат перекраски (%s ...): сначала запусти ab_detectors.py "
                         "(и ab_anomalies.py), потом snapshot" % ", ".join(hit[:3]))
    if os.path.isdir(ORIG) and not force:
        raise SystemExit("копия оригиналов уже есть: %s (обновить — snapshot --force)" % ORIG)
    for d in DIRS:
        dst = os.path.join(ORIG, d)
        if os.path.isdir(dst):
            shutil.rmtree(dst)
        shutil.copytree(os.path.join(T, d), dst)
    print("оригиналы скопированы в", ORIG)


def cmd_build(only, tmp, preview):
    if not os.path.isdir(ORIG):
        raise SystemExit("нет копии оригиналов %s: сначала «snapshot» сразу после ab_detectors.py" % ORIG)
    ctx = Ctx(tmp, preview)
    os.makedirs(preview, exist_ok=True)
    for code in only:
        pv = []
        DETECTORS[code](ctx, pv)
        path = os.path.join(preview, "%s_%s.png" % (code, DETECTORS[code].__name__))
        write_preview(path, pv)
        print("превью", path)
    for (d, stem), values in RVMAT_EDITS.items():
        if d[:2] in only:
            edit_rvmat(ctx, d, stem, values)
    for line in ctx.checks:
        print("  " + line)
    state = load_state()
    outs = state.get("outputs") or {}
    outs.update(ctx.written)
    state["outputs"] = outs
    with open(STATE, "w", encoding="utf-8") as f:
        json.dump(state, f, ensure_ascii=False, indent=1, sort_keys=True)
    print("материалы для генераторов (не правятся здесь):")
    for k, v in RVMAT_FOR_GENERATORS.items():
        print("   %s: %s" % (k, ", ".join("%s[]={%s}" % kv for kv in v.items())))


def main():
    a = sys.argv[1:]
    cmd = "build"
    if a and not a[0].startswith("--"):
        cmd = a.pop(0)
    opts = {}
    while a:
        k = a.pop(0)
        if k == "--force":
            opts["force"] = True
        elif k in ("--only", "--tmp", "--preview") and a:
            opts[k[2:]] = a.pop(0)
        else:
            raise SystemExit("неизвестный аргумент: " + k)
    tmp = opts.get("tmp") or os.path.join(os.environ.get("TEMP", HERE), "vb_ab_retex")
    if cmd == "snapshot":
        cmd_snapshot(opts.get("force", False))
    elif cmd == "build":
        only = opts["only"].split(",") if opts.get("only") else list(DETECTORS)
        for c in only:
            if c not in DETECTORS:
                raise SystemExit("нет детектора %s (есть %s)" % (c, ", ".join(DETECTORS)))
        cmd_build(only, tmp, opts.get("preview") or os.path.join(tmp, "preview"))
    else:
        raise SystemExit("неизвестная команда: " + cmd)


if __name__ == "__main__":
    main()
