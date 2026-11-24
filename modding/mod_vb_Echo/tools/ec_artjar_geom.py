# -*- coding: utf-8 -*-
r"""Геометрия поясов с баночками для артефактов экзо Эхо (VB_EchoClothes, ТЗ раздел 8.4) — без Blender.

Один код на концепт (ec_concept.py --jars) и на модели (ec_artjar_model.py).

Решение пользователя 11-24: не колба-витрина на бедре («бандура»), а маленькие баночки по горизонтали вокруг пояса.
Малый пояс — 2 баночки (левый бок спереди), средний — 4 (по 2 на боках), большой — 6 (по 3).

* Пояс вешается в гнездо бывшей капсулы на сервоприводе (прокси vb_ec_artcase_box, кость pelvis — ходит с тазом).
  Строится в осях сервопривода (x вправо, y вверх, z назад, лицо в -z), в оси самого пояса переводится матрицей прокси
  (local = M · (world - POS)), как колба 11-24.
* Артефакт в баночке — уменьшенная копия его модели на клиенте (CreateStaticObjectUsingP3D + AddChild, каталог E126):
  вложение в слоте движок рисует только в натуральную величину (10–32 см), масштаб вложения сбрасывает. Сам артефакт
  лежит в слоте пояса без прокси (не виден). Точки копий — память модели vbart1..N (центр баночки).
* Контур пояса (внешний радиус сервопривода и одежды от оси тела по углу, высота 0,89–1,0 м; замер 11-24):
  баночки стоят снаружи контура с зазором, ремешок лежит по контуру.
"""
import math

import numpy as np

import ec_geom as G

# прокси vb_ec_artcase_box в ExoPrivod_m (ODOL собранного мода 11-24): строки — оси пояса в осях сервопривода
PROXY_M = np.array([[0.8547911047935486, -0.0036456510424613953, -0.5189594030380249],
                    [0.05936473235487938, 0.9940983653068542, 0.09079767763614655],
                    [0.5155656933784485, -0.10842093825340271, 0.8499628901481628]])
PROXY_POS = np.array([-0.22658777236938477, 0.9841875433921814, -0.12745289504528046])

# угол вокруг тела: 0 — вперёд (-z), 90 — левый бок (-x), -90 — правый; внешний радиус пояса сервопривода и одежды
# на высоте баночек (y 0,98–1,07; замер 11-24)
CONTOUR = [(0, 0.184), (20, 0.184), (40, 0.190), (55, 0.199), (70, 0.213), (85, 0.228), (100, 0.232), (120, 0.236)]
KINDS = {"small": 2, "medium": 4, "large": 6}
JAR_R = 0.030           # стекло, наружный радиус
JAR_H = 0.066           # высота стекла
CAP_R = 0.033
CAP_H = 0.009
GAP = 0.004             # от контура тела до дна/стенки баночки
STEP = 0.078            # между центрами баночек по дуге
Y_TOP = 1.066           # верх крышки — по верхнему краю ленты пояса сервопривода (лента y 1,00–1,07, замер 11-24)
FIRST = 40.0            # угол первой (передней) баночки на боку
ART_FIT = 0.05          # самый большой размер мини-артефакта (стекло 6 см)
RING_T = 0.0025
UV_SCALE = 2.5


def body_r(a):
    a = abs(a)
    xs = [p[0] for p in CONTOUR]
    return float(np.interp(a, xs, [p[1] for p in CONTOUR]))


def out_dir(a):
    r = math.radians(a)
    return np.array([-math.sin(r), 0.0, -math.cos(r)])


def jar_angles(kind):
    """Углы баночек: по дуге от FIRST назад с шагом STEP (длина дуги), на левом боку; со средним и большим — и на
    правом (зеркально)."""
    n = KINDS[kind]
    per_side = n if kind == "small" else n // 2
    angs, a = [], FIRST
    for _ in range(per_side):
        angs.append(a)
        R = body_r(a) + GAP + CAP_R
        a += math.degrees(STEP / R)
    left = angs
    return left if kind == "small" else left + [-x for x in left]


def layout(kind):
    jars = []
    yc = Y_TOP - CAP_H - JAR_H / 2
    for a in jar_angles(kind):
        d = out_dir(a)
        R = body_r(a) + GAP + CAP_R
        c = np.array([d[0] * R, yc, d[2] * R])
        jars.append({"a": a, "c": c, "out": d})
    return {"jars": jars, "yc": yc}


def _uv_cyl(m, scale=UV_SCALE):
    """UV в метрах: по окружности и высоте (у G.cylinder UV 0..1 на всю окружность)."""
    for i, p in enumerate(m.pts):
        m.uv[i] = (float(m.uv[i][0] * 2 * math.pi * 0.033 * scale), float(p[1] * scale))
    return m


def _box_uv(b, scale=UV_SCALE):
    for f in b.faces:
        ax = int(np.argmax(np.abs(b.nrm[f[0]])))
        u, v = [k for k in range(3) if k != ax]
        for i in f:
            b.uv[i] = (float(b.pts[i][u] * scale), float(b.pts[i][v] * scale))
    return b


def build(kind, seg=24):
    """Пояс в осях сервопривода: {"steel", "glass": Mesh, "rings": [Mesh на баночку], "art": [центр баночки],
    "hull": (lo, hi)}. Кольца — свои выделения vbjar1..N (горят у занятых баночек)."""
    L = layout(kind)
    up = np.array([0.0, 1.0, 0.0])
    steel, glass, rings, arts = G.Mesh(), G.Mesh(), [], []
    y_bot = L["yc"] - JAR_H / 2 - CAP_H
    for j in L["jars"]:
        c, d = j["c"], j["out"]
        t = np.cross(up, d)                                          # касательная к поясу
        cb = c - up * (JAR_H / 2 + CAP_H / 2)                        # дно
        ct = c + up * (JAR_H / 2 + CAP_H / 2)                        # крышка
        steel.extend(_uv_cyl(G.cylinder(cb, up, CAP_R, CAP_H, seg)))
        steel.extend(_uv_cyl(G.cylinder(ct, up, CAP_R, CAP_H, seg)))
        steel.extend(_uv_cyl(G.cylinder(ct + up * (CAP_H / 2 + 0.004), up, 0.012, 0.008, max(8, seg // 2))))  # пробка
        glass.extend(G.cylinder(c, up, JAR_R, JAR_H, seg, caps=False))
        # три прутка обвязки: сзади (к телу) и по бокам ближе к спине — спереди стекло открыто
        for k in (180.0, 70.0, -70.0):
            rr = math.radians(k)
            off = (math.cos(rr) * d + math.sin(rr) * t) * (JAR_R + 0.0015)
            steel.extend(G.cylinder(c + off, up, 0.0022, JAR_H, 6))
        # скоба к ремешку: от задней стенки к телу
        R = np.stack([t, up, d], axis=1)
        steel.extend(_box_uv(G.box(c - d * (JAR_R + 0.006) + up * (JAR_H / 2 - 0.008), (0.012, 0.016, 0.006), R)))
        # свет: кольцо по краю дна + площадка под артефактом
        rm = G.Mesh()
        rm.extend(G.torus(cb + up * (CAP_H / 2 + RING_T), up, JAR_R - 0.003, RING_T, seg, 6))
        rm.extend(G.cylinder(cb + up * (CAP_H / 2 + 0.0008), up, 0.017, 0.0016, max(8, seg // 2)))
        rings.append(G.fix_winding(rm))
        arts.append(c.copy())
    # ремешок по контуру пояса на каждом боку: от первой баночки до последней (+ запас), лежит на поясе сервопривода
    sides = [[j for j in L["jars"] if j["a"] > 0], [j for j in L["jars"] if j["a"] < 0]]
    y_band = Y_TOP - 0.022
    for js in sides:
        if not js:
            continue
        a0, a1 = min(j["a"] for j in js), max(j["a"] for j in js)
        sgn = 1 if a0 > 0 else -1
        lo_a, hi_a = (a0 - 6, a1 + 6) if sgn > 0 else (a0 - 6, a1 + 6)
        pts = []
        for a in np.linspace(lo_a, hi_a, 24):
            d = out_dir(a)
            pts.append(d * (body_r(a) + 0.003) + np.array([0.0, y_band, 0.0]))
        for i in range(len(pts) - 1):                           # лента: короткие бруски по дуге
            p0, p1 = pts[i], pts[i + 1]
            mid = (p0 + p1) / 2
            tv = (p1 - p0) / np.linalg.norm(p1 - p0)
            nv = np.cross(tv, up)
            R = np.stack([tv, up, nv], axis=1)
            steel.extend(_box_uv(G.box(mid, (np.linalg.norm(p1 - p0) / 2 + 0.0005, 0.013, 0.003), R)))
    P = np.array(steel.pts + glass.pts)
    return {"steel": G.fix_winding(steel), "glass": G.fix_winding(glass), "rings": rings, "art": arts,
            "hull": (P.min(0), P.max(0)), "layout": L, "y_bot": y_bot}


def to_local_mesh(m):
    """Меш из осей сервопривода в оси пояса (его модель): local = M · (world - POS)."""
    return m.transformed(PROXY_M, -PROXY_M @ PROXY_POS)


def to_local_point(p):
    return PROXY_M @ (np.asarray(p, dtype=np.float64) - PROXY_POS)


if __name__ == "__main__":
    for k in KINDS:
        b = build(k)
        lo, hi = b["hull"]
        print("%-6s баночек %d, углы %s, габарит %.0f x %.0f x %.0f см" % (
            k, len(b["art"]), [round(j["a"]) for j in b["layout"]["jars"]], *((hi - lo) * 100)))
