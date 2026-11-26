# -*- coding: utf-8 -*-
r"""Крепления спаренных предметов холодного синтеза (2026-11-26, модели — ep_fusion_pairs.py):
  машина  vb_echorunner.p3d — прокси \vb_echorunner\proxy\vb_er_tank_pair посередине между баками;
  сервопривод ExoPrivod_m.p3d — прокси \vb_echoclothes\exo_set\backpacks\exoprivod\vb_exocell_pair посередине между
  гнёздами ячеек, на кости spine3 (как крепления ячеек; без веса кости ячейки не пошли бы за телом).

  python -I ep_proxies.py            (оба файла; повторный запуск ничего не меняет)

Рамка обоих — прямая, как у прокси баков: модель пары уже лежит в осях машины / сервопривода, сдвинутых в середину.
Треугольник — копия треугольника vb_er_tank_1 машины (те же смещения вершин в том же порядке грани и нормаль).
LOD — те же, где стоят крепления баков / ячеек. Вызывается и из er_finalize.py (пересборка машины) и
ec_servo_model.py (пересборка сервопривода).
"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
MODDING = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.abspath(os.path.join(MODDING, "..", ".claude", "tools", "py3d")))
import py3d  # noqa: E402

CAR = os.path.join(MODDING, "mods_vb_vehicles", "mod_vb_EchoRunner", "VB_EchoRunner", "vb_echorunner.p3d")
SERVO = os.path.join(MODDING, "mod_vb_Echo", "VB_EchoClothes", "exo_set", "Backpacks", "ExoPrivod", "ExoPrivod_m.p3d")
TANK_PAIR = "proxy:\\vb_echorunner\\proxy\\vb_er_tank_pair.001"
CELL_PAIR = "proxy:\\vb_echoclothes\\exo_set\\backpacks\\exoprivod\\vb_exocell_pair.001"
IDENT_FRAME = ((-1.0, 0.0, 0.0), (0.0, 0.0, 1.0), (0.0, 1.0, 0.0))      # «сырая» прямая рамка (как у баков)


def tri(lod, key):
    """Прокси с key в имени: (имя, якорь, [смещения вершин грани от якоря по порядку], нормаль, первая точка)."""
    for pr in lod.get_proxies():
        name = pr["name"]
        if key not in name.lower():
            continue
        sel = lod.selections[name]
        face = next(iter(sel.faces))
        anchor = np.array(pr["anchor"], dtype=float)
        offs = [np.array(lod.points[v.point_index].coords) - anchor for v in face.vertices]
        normal = lod.facenormals[face.vertices[0].normal_index]
        return name, anchor, offs, normal, lod.points[face.vertices[0].point_index], pr
    return None


def ident_template(car):
    """Смещения и нормаль треугольника vb_er_tank_1 (прямая рамка) — образец для обоих креплений пар."""
    for lod in car.lods:
        t = tri(lod, "vb_er_tank_1")
        if t:
            frame = [tuple(round(c, 4) for c in v) for v in t[5]["frame"]]
            assert np.allclose(frame, IDENT_FRAME, atol=1e-3), frame
            return t[2], t[3]
    raise RuntimeError("в машине нет прокси vb_er_tank_1")


def add_proxy(lod, name, anchor, tmpl, weights):
    offs, normal = tmpl
    base = len(lod.points)
    for o in offs:
        p = py3d.Point()
        p.coords = tuple(float(x) for x in anchor + o)
        lod.points.append(p)
    lod.facenormals.append(tuple(float(x) for x in normal))
    ni = len(lod.facenormals) - 1
    fa = py3d.Face(lod.points, lod.facenormals)
    fa.flags = 0
    fa.texture = ""
    fa.material = ""
    for i in range(len(offs)):
        vx = py3d.Vertex(lod.points, lod.facenormals)
        vx.point_index = base + i
        vx.normal_index = ni
        vx.uv = (0.0, 0.0)
        fa.vertices.append(vx)
    lod.faces.append(fa)
    new = lod.points[base:]
    s = lod.new_selection(name)
    s.points = {p: 1 for p in new}
    s.faces = {fa: 1}
    for sel_name, w in weights.items():
        lod.new_selection(sel_name).points.update({p: w for p in new})


def patch_car(car, tmpl=None):
    """Во всех LOD с баками — прокси пары посередине между vb_er_tank_1 и _2. Вернёт число LOD, куда добавлено."""
    tmpl = tmpl or ident_template(car)
    added = 0
    for lod in car.lods:
        t1, t2 = tri(lod, "vb_er_tank_1"), tri(lod, "vb_er_tank_2")
        if not (t1 and t2) or TANK_PAIR in lod.selections:
            continue
        add_proxy(lod, TANK_PAIR, (t1[1] + t2[1]) / 2, tmpl, {})
        added += 1
    return added


def patch_servo(servo, tmpl):
    """Во всех LOD с ячейками — прокси пары посередине между гнёздами; кости — как у крепления первой ячейки."""
    added = 0
    for lod in servo.lods:
        t1, t2 = tri(lod, "vb_exocell_slot_1"), tri(lod, "vb_exocell_slot_2")
        if not (t1 and t2) or CELL_PAIR in lod.selections:
            continue
        p0 = t1[4]
        weights = {n: s.points[p0] for n, s in lod.selections.items()
                   if not n.lower().startswith("proxy:") and p0 in s.points}
        assert weights, "у крепления ячейки нет кости — сервопривод собран не так"
        add_proxy(lod, CELL_PAIR, (t1[1] + t2[1]) / 2, tmpl, weights)
        added += 1
    return added


def load(path):
    with open(path, "rb") as f:
        return py3d.P3D(f)


def main():
    car = load(CAR)
    tmpl = ident_template(car)
    n = patch_car(car, tmpl)
    if n:
        car.save(CAR)
    print("машина: прокси пары баков добавлено в LOD:", n)
    servo = load(SERVO)
    k = patch_servo(servo, tmpl)
    if k:
        servo.save(SERVO)
    print("сервопривод: прокси пары ячеек добавлено в LOD:", k)
    for path, key in ((CAR, "vb_er_tank_pair"), (SERVO, "vb_exocell_pair")):          # проверка: рамка и место
        m = load(path)
        for lod in m.lods:
            t = tri(lod, key)
            if t:
                print("  ", os.path.basename(path), "LOD", lod.resolution, "якорь", np.round(t[1], 4),
                      "рамка", [tuple(round(c, 3) for c in v) for v in t[5]["frame"]],
                      "кости", [n for n, s in lod.selections.items() if t[4] in s.points and not n.startswith("proxy")])


if __name__ == "__main__":
    main()
