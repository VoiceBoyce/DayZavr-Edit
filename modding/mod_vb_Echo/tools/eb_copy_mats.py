# -*- coding: utf-8 -*-
r"""Материалы Echo Runner (er_*) — копией в VB_EchoBase/data с путями vb_echobase\data\ (мод базы не зависит от машины).

  python eb_copy_mats.py          — в VB_EchoBase/data
  python eb_copy_mats.py core     — в VB_EchoCore/data (энергоячейка ядра, 11-20): только её материалы,
                                    неон и метки уровней eb_neon_t1..4 (из VB_EchoBase/data)
  python eb_copy_mats.py materials — в VB_EchoMaterials/data (материалы крафта, 11-24): болты и пустой серый неон
Как у общих деталей машин (VB_EchoVehicleParts/data). Берутся только материалы, которые есть в моделях eb_models.py,
и все варианты неона (красный, «горящий» чистый, синтез, пустой серый) — их ставит скрипт по состоянию устройства.
"""
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
SRC = os.path.abspath(os.path.join(HERE, "..", "..", "mods_vb_vehicles", "mod_vb_EchoRunner", "VB_EchoRunner", "data", "parts"))
DST = os.path.abspath(os.path.join(HERE, "..", "VB_EchoBase", "data"))
OLD = "vb_echorunner\\data\\parts\\"
NEW = "vb_echobase\\data\\"

MATS = ["steel_dark", "steel_rust", "bolt", "flare", "lens", "glass", "screen", "cable", "perf", "rubber", "paint_red"]
TIERS = []
if len(sys.argv) > 1 and sys.argv[1] == "core":
    DST = os.path.abspath(os.path.join(HERE, "..", "VB_EchoCore", "data"))
    NEW = NEW.replace("vb_echobase", "vb_echocore")
    MATS = ["steel_dark", "steel_rust", "bolt", "glass", "cable"]
    TIERS = [os.path.join(HERE, "..", "VB_EchoBase", "data", "eb_neon_t%d_co.paa" % t) for t in (1, 2, 3, 4)]
NEON_TEX = ["er_neon_co", "er_neon_hot_co", "er_neon_fusion_co", "er_neon_fusion_hot_co", "er_neon_empty_co",
            "er_neon_nohq", "er_neon_smdi"]
NEON_RV = ["er_neon", "er_neon_idle", "er_neon_off", "er_neon_empty", "er_screen_off"]
if len(sys.argv) > 1 and sys.argv[1] == "materials":
    # материалы крафта (11-24): отдельный мод @VB_EchoMaterials, ни от чего не зависит — болты и пустой серый неон
    DST = os.path.abspath(os.path.join(HERE, "..", "VB_EchoMaterials", "data"))
    NEW = "vb_echomaterials\\data\\"
    MATS = ["bolt"]
    NEON_TEX = ["er_neon_empty_co", "er_neon_nohq", "er_neon_smdi"]
    NEON_RV = ["er_neon_empty"]


def copy_rvmat(name):
    with open(os.path.join(SRC, name + ".rvmat"), "r", encoding="utf-8", errors="replace") as f:
        s = f.read()
    with open(os.path.join(DST, name + ".rvmat"), "w", encoding="utf-8", newline="\n") as f:
        f.write(s.replace(OLD, NEW))


def main():
    os.makedirs(DST, exist_ok=True)
    n = 0
    for m in MATS:
        for suf in ("_co", "_ca", "_nohq", "_smdi"):
            p = os.path.join(SRC, "er_%s%s.paa" % (m, suf))
            if os.path.exists(p):
                shutil.copy2(p, DST)
                n += 1
        copy_rvmat("er_" + m)
        n += 1
    for t in NEON_TEX:
        shutil.copy2(os.path.join(SRC, t + ".paa"), DST)
        n += 1
    for r in NEON_RV:
        copy_rvmat(r)
        n += 1
    for t in TIERS:
        shutil.copy2(t, DST)
        n += 1
    print("OK файлов", n, "->", DST)


if __name__ == "__main__":
    main()
