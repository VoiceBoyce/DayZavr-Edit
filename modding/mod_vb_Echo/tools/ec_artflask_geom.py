# -*- coding: utf-8 -*-
r"""Геометрия колб для артефактов экзо Эхо (VB_EchoClothes, ТЗ раздел 8.4, утверждено 11-24) — без Blender.

Один код на концепт (ec_artflask_concept.py, ec_concept.py --flask) и на модели (ec_artflask_model.py).

Колба — витрина на левом бедре сервопривода, в гнезде бывшей капсулы (прокси vb_ec_artcase_box). Строится в
координатах сервопривода (как надета; оси модели DayZ: x вправо, y вверх, z назад, лицо смотрит в -z; левое бедро
у x < 0, наружу = -x), потом переводится в оси самой колбы матрицей прокси (local = M · (world - POS)).

* Ячейки без перегородок: артефакт висит перед светящимся кольцом на задней стенке (стенка к ноге), стекло снаружи
  и по бокам. Шаг ячеек PITCH = 14 см; до стекла с краю ещё MARGIN — у крайних ячеек место 16 см. Замер 51 модели
  артефактов OOTS (11-24): в такую ячейку с допуском 1 см помещаются 44 из 46 обычных, «Пузырь» и «Грави» выступают
  на ~2 см; пять огромных (Цветок, Оазис, Изолятор, Штурвал, Каменный цветок — 20–32 см) в колбы не кладутся.
* Артефакт встаёт в точку прокси началом своей модели (autoCenter 0, начало — «земля», артефакт висит над ней на
  6–19 см). Поэтому прокси = центр ячейки минус типичный центр артефакта ART_CENTER (в осях артефакта).
* Оси артефакта на бедре: его x — вдоль бедра (+z), y — вверх, z — наружу (-x): широкие артефакты ложатся вдоль
  ноги, а «лицо» модели (+z) смотрит наружу.
"""
import numpy as np

import ec_geom as G

# прокси vb_ec_artcase_box в ExoPrivod_m (ODOL собранного мода 11-24): строки — оси колбы в осях сервопривода
PROXY_M = np.array([[0.8547911047935486, -0.0036456510424613953, -0.5189594030380249],
                    [0.05936473235487938, 0.9940983653068542, 0.09079767763614655],
                    [0.5155656933784485, -0.10842093825340271, 0.8499628901481628]])
PROXY_POS = np.array([-0.22658777236938477, 0.9841875433921814, -0.12745289504528046])

KINDS = {"small": (1, 2), "medium": (2, 2), "large": (2, 3)}       # (столбцов вдоль бедра, рядов по высоте)
PITCH = 0.14            # шаг ячеек
MARGIN = 0.01           # от крайнего центра ячейки + PITCH/2 до стекла
DEPTH = 0.15            # глубина камеры (наружу от задней стенки до стекла)
POST = 0.012            # стойки рамы
GLASS_T = 0.003
BACK_T = 0.012          # задняя стенка (к ноге)
TOP_H = 0.032           # крышка
BOT_H = 0.016           # дно
X_BACK = -0.270         # задняя стенка (сторона к ноге) — снаружи стойки сервопривода (x до -0,266)
Z_C = -0.03             # центр колбы вдоль бедра (бедро z -0,125..0,10)
Y_TOP = 0.985           # верх крышки — у пояса сервопривода
ART_CENTER = np.array([-0.007, 0.128, -0.035])   # типичный центр артефакта в осях его модели (замер 11-24)
ART_R = np.array([[0.0, 0.0, -1.0],              # столбцы: оси артефакта x, y, z в осях сервопривода
                  [0.0, 1.0, 0.0],
                  [1.0, 0.0, 0.0]])
RING_R, RING_T = 0.05, 0.0035                     # кольцо ячейки на задней стенке


def layout(kind, art_scale=1.0):
    """art_scale < 1 — артефакт показан уменьшенным (если движок даст масштабировать вложение): ячейки меньше."""
    cols, rows = KINDS[kind]
    pitch, margin, depth = PITCH * art_scale, MARGIN * art_scale, DEPTH * art_scale
    w = cols * pitch + 2 * margin
    h = rows * pitch + 2 * margin
    L = {"cols": cols, "rows": rows, "pitch": pitch, "margin": margin, "depth": depth, "scale": art_scale}
    L["z0"], L["z1"] = Z_C - w / 2, Z_C + w / 2
    L["y_ct"] = Y_TOP - TOP_H                    # верх камеры
    L["y_cb"] = L["y_ct"] - h                    # низ камеры
    L["xi"] = X_BACK - BACK_T                    # лицевая сторона задней стенки
    L["xo"] = L["xi"] - depth                    # внутренняя сторона наружного стекла
    L["cells"] = [np.array([L["xi"] - depth / 2, L["y_ct"] - margin - pitch * (j + 0.5),
                            L["z0"] + margin + pitch * (i + 0.5)])
                  for j in range(rows) for i in range(cols)]          # по рядам сверху, в ряду спереди назад
    return L


UV_SCALE = 2.5          # повтор текстуры стали — раз в 40 см (у G.box UV 0..1 на грань, сталь тянулась полосами)


def _box(m, lo, hi):
    lo, hi = np.asarray(lo, dtype=np.float64), np.asarray(hi, dtype=np.float64)
    b = G.box((lo + hi) / 2, tuple((hi - lo) / 2))
    for f in b.faces:                            # кубическая развёртка в метрах: по двум осям, кроме оси нормали
        ax = int(np.argmax(np.abs(b.nrm[f[0]])))
        u, v = [a for a in range(3) if a != ax]
        for i in f:
            b.uv[i] = (float(b.pts[i][u] * UV_SCALE), float(b.pts[i][v] * UV_SCALE))
    m.extend(b)


def build(kind, seg=32, art_scale=1.0):
    """Колба в осях сервопривода: {"steel", "glass": Mesh, "cells": [Mesh на ячейку], "proxies": [(ART_R, точка)],
    "hull": (lo, hi)}. Кольца ячеек — свои выделения (vbcell1..N), гаснут/горят по одной."""
    L = layout(kind, art_scale)
    z0, z1, y_ct, y_cb, xi, xo = L["z0"], L["z1"], L["y_ct"], L["y_cb"], L["xi"], L["xo"]
    PITCH, MARGIN, s = L["pitch"], L["margin"], art_scale      # noqa: N806 (шаг этой колбы)
    xg = xo - GLASS_T                            # наружная сторона стекла
    steel, glass = G.Mesh(), G.Mesh()
    # задняя стенка (к ноге)
    _box(steel, (X_BACK - BACK_T, y_cb - BOT_H, z0 - 0.004), (X_BACK, y_ct, z1 + 0.004))
    # крышка: нижний пояс с выступом по кругу + верх чуть уже (фаска ступенькой)
    _box(steel, (xg - 0.006, y_ct, z0 - 0.008), (X_BACK, y_ct + 0.014, z1 + 0.008))
    _box(steel, (xg - 0.001, y_ct + 0.014, z0 - 0.003), (X_BACK - 0.004, Y_TOP, z1 + 0.003))
    # дно
    _box(steel, (xg - 0.006, y_cb - BOT_H, z0 - 0.008), (X_BACK, y_cb, z1 + 0.008))
    # стойки по углам камеры
    for x in (xg, xi):
        for z in (z0, z1):
            _box(steel, (x - POST / 2, y_cb, z - POST / 2), (x + POST / 2, y_ct, z + POST / 2))
    # тонкие рёбра по наружному стеклу между ячейками
    for i in range(1, L["cols"]):
        z = z0 + MARGIN + PITCH * i
        _box(steel, (xg - 0.004, y_cb, z - 0.003), (xg, y_ct, z + 0.003))
    for j in range(1, L["rows"]):
        y = y_ct - MARGIN - PITCH * j
        _box(steel, (xg - 0.004, y - 0.003, z0), (xg, y + 0.003, z1))
    # скобы к поясу: из-за крышки к ноге, у переднего края (и у заднего, если колба широкая)
    clips = [z0 + 0.006] + ([z1 - 0.046] if L["cols"] > 1 else [])
    for zc in clips:
        _box(steel, (X_BACK - 0.002, Y_TOP - 0.04, zc), (-0.222, Y_TOP + 0.004, zc + 0.04))
        _box(steel, (-0.232, Y_TOP - 0.07, zc + 0.008), (-0.222, Y_TOP - 0.04, zc + 0.032))   # язычок за пояс
    # стекло: наружу и по бокам (вдоль бедра)
    _box(glass, (xg, y_cb, z0), (xo, y_ct, z1))
    _box(glass, (xg, y_cb, z0 - GLASS_T), (xi, y_ct, z0))
    _box(glass, (xg, y_cb, z1), (xi, y_ct, z1 + GLASS_T))
    # кольца ячеек на задней стенке + ядро в центре кольца
    cells, proxies = [], []
    for c in L["cells"]:
        cm = G.Mesh()
        cm.extend(G.torus((xi - RING_T, c[1], c[2]), (1.0, 0.0, 0.0), RING_R * s, RING_T, seg, 6))
        cm.extend(G.cylinder((xi - 0.0015, c[1], c[2]), (1.0, 0.0, 0.0), 0.012 * s, 0.003, max(8, seg // 2)))
        cells.append(G.fix_winding(cm))
        proxies.append((ART_R.copy(), c - ART_R @ (ART_CENTER * s)))
    lo = np.array([xg - 0.006, y_cb - BOT_H, z0 - 0.008])
    hi = np.array([X_BACK, Y_TOP, z1 + 0.008])
    return {"steel": G.fix_winding(steel), "glass": G.fix_winding(glass), "cells": cells, "proxies": proxies,
            "hull": (lo, hi), "layout": L}


def to_local_mesh(m):
    """Меш из осей сервопривода в оси колбы (её модель): local = M · (world - POS)."""
    return m.transformed(PROXY_M, -PROXY_M @ PROXY_POS)


def to_local_point(p):
    return PROXY_M @ (np.asarray(p, dtype=np.float64) - PROXY_POS)


def to_local_axes(R):
    """Оси (столбцы R) из осей сервопривода в оси колбы."""
    return PROXY_M @ R


def to_world_point(p):
    return PROXY_POS + PROXY_M.T @ np.asarray(p, dtype=np.float64)


if __name__ == "__main__":
    for k in KINDS:
        b = build(k)
        lo, hi = b["hull"]
        print("%-6s ячеек %d  размер %.0f x %.0f x %.0f см (наружу x высота x вдоль бедра), низ на высоте %.2f м" % (
            k, len(b["cells"]), (hi[0] - lo[0]) * 100, (hi[1] - lo[1]) * 100, (hi[2] - lo[2]) * 100, lo[1]))
