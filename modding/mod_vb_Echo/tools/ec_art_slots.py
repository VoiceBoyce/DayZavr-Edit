# -*- coding: utf-8 -*-
r"""Заплатка VB_EchoClothes_Art: артефактам OOTS — слоты баночек пояса VB_EC_ArtJar1..6 и габарит их модели
(ТЗ_VB_EchoClothes.md 8.4).

  python ec_art_slots.py <config.cpp артефактов> <папка, где лежит DD_Artifacts с моделями>
  (Python из Blender 5.3: numpy; модели читает .claude/tools/dayz-render/odol_read.py)

<config.cpp артефактов> — распакованный и переведённый в текст model\Artifacts\config.bin из DD_Artifacts.pbo
(@OTTS_Artifacts; ExtractPbo -F=config.bin переводит сам); модели — ExtractPbo -F=*.p3d того же PBO.
Пишет VB_EchoClothes_Art/config.cpp.

* Слоты задаёт inventorySlot[] класса. У артефактов OOTS он объявлен в базовых DD_Artifact_* (у OOTS_ART_* — по
  наследству), значит переобъявляем эти базы тем же родителем со своим списком: прежние слоты кейсов OOTS (Artefacts,
  Artefacts1, Artefacts2; у наследника без своего списка — родительский) + наши. Свой inventorySlot[] у OOTS_ART_* —
  переобъявляем и его. Кейсы OOTS_ART_CASE* не трогаем.
* vbArtBox[] = {центр x, y, z, наибольший размер} видимой модели (LOD0) в осях модели — по нему скрипт пояса
  (VB_EC_ArtBelt.c) вписывает уменьшенную копию в баночку. ClippingInfo движка для этого не годится: это габарит из
  заголовка модели (все LOD) плюс запас ~2 см с каждой стороны — копии выходили вдвое мельче (стенд 11-24).
"""
import os
import re
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
EDIT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(EDIT, ".claude", "tools", "dayz-render"))
import odol_read as O  # noqa: E402

OUT = os.path.join(HERE, "..", "VB_EchoClothes_Art", "config.cpp")
OURS = ["VB_EC_ArtJar%d" % i for i in range(1, 7)]


def parse(text):
    """Классы верхнего уровня CfgVehicles: имя -> [родитель, inventorySlot[] или None, model или None]."""
    body = text[text.index("class CfgVehicles"):]
    out, depth, cur = {}, 0, None
    for line in body.splitlines()[1:]:
        s = line.strip()
        if depth == 1:
            m = re.match(r"class\s+(\w+)\s*(?::\s*(\w+))?\s*$", s)
            if m:
                cur = m.group(1)
                out[cur] = [m.group(2), None, None]
        if depth == 2 and cur:
            m = re.match(r"inventorySlot\[\]\s*=\s*\{([^}]*)\};", s)
            if m:
                out[cur][1] = [x.strip().strip('"') for x in m.group(1).split(",") if x.strip()]
            m = re.match(r'model\s*=\s*"([^"]+)";', s)
            if m:
                out[cur][2] = m.group(1)
        depth += s.count("{") - s.count("}")
        if depth <= 0 and s.startswith("}"):
            break
    return out


def vis_box(path):
    """Центр и наибольший размер видимой модели (LOD0, без граней прокси)."""
    lod = O.read_visual_lod(path, 0)
    skip = set()
    for s in lod["selections"]:
        if s["name"].lower().startswith("proxy:"):
            skip.update(s["face_set"])
    P = np.array(lod["points"])[sorted({v for fi, f in enumerate(lod["faces"]) if fi not in skip for v in f})]
    lo, hi = P.min(0), P.max(0)
    return (lo + hi) / 2, float((hi - lo).max())


def main():
    src, root = sys.argv[1], sys.argv[2]
    classes = parse(open(src, encoding="utf-8", errors="replace").read())
    arts = [n for n in classes if n.startswith("OOTS_ART_") and not n.startswith("OOTS_ART_CASE")]

    def effective(n, k):
        """Значение поля k (1 — inventorySlot[], 2 — model) с учётом наследования."""
        while n in classes:
            if classes[n][k] is not None:
                return classes[n][k]
            n = classes[n][0]
        return None
    targets = {}
    for n in arts:
        parent, slots, model = classes[n]
        if slots is not None or model is not None:     # свой список слотов или своя модель у самого артефакта
            targets[n] = parent
        if parent in classes and parent not in targets:
            targets[parent] = classes[parent][0]
    boxes, cache = {}, {}
    for n in targets:
        model = effective(n, 2)
        if not model:
            continue
        path = os.path.join(root, *model.strip("\\").split("\\"))
        if model not in cache:
            try:
                cache[model] = vis_box(path)
            except Exception as e:  # noqa: BLE001
                print("НЕ ПРОЧИТАНА модель %s (%s): %s" % (model, n, e))
                cache[model] = None
        if cache[model]:
            boxes[n] = cache[model]
    parents = sorted({p for p in targets.values() if p})
    L = ["// VB_EchoClothes_Art — артефактам OOTS слоты баночек пояса Эхо (VB_EC_ArtJar1..6) и габарит модели",
         "// vbArtBox[] = {центр x, y, z, наибольший размер} LOD0 — по нему скрипт пояса вписывает копию в баночку.",
         "// Сгенерировано tools/ec_art_slots.py из config и моделей DD_Artifacts (@OTTS_Artifacts), руками не править.",
         "// Без @OTTS_Artifacts этот PBO не грузить (requiredAddons DD_Artifacts) — на сервере 16 он есть всегда.",
         "class CfgPatches", "{", "\tclass VB_EchoClothes_Art", "\t{", "\t\tunits[]={};", "\t\tweapons[]={};",
         "\t\trequiredVersion=0.1;", "\t\trequiredAddons[]={\"DZ_Data\",\"DD_Artifacts\",\"VB_EchoClothes\"};",
         "\t};", "};", "class CfgVehicles", "{"]
    for p in parents:
        if p not in targets:
            L.append("\tclass %s;" % p)
    for n in sorted(targets, key=lambda k: (k.startswith("OOTS_ART_"), k)):
        slots = effective(n, 1) or []
        allslots = [s for s in slots if s not in OURS] + OURS
        L.append("\tclass %s: %s" % (n, targets[n]))
        L.append("\t{")
        L.append("\t\tinventorySlot[]={%s};" % ",".join('"%s"' % s for s in allslots))
        if n in boxes:
            c, size = boxes[n]
            L.append("\t\tvbArtBox[]={%.4f,%.4f,%.4f,%.4f};" % (c[0], c[1], c[2], size))
        L.append("\t};")
    L.append("};")
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(L) + "\n")
    print("артефактов OOTS_ART_*: %d, переобъявлено классов: %d, с габаритом: %d -> %s" % (
        len(arts), len(targets), len(boxes), OUT))


if __name__ == "__main__":
    main()
