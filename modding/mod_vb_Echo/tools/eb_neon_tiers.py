# -*- coding: utf-8 -*-
r"""Неон уровней устройств базы (решение пользователя 11-20): цвет неона = что течёт внутри / уровень устройства.

  python eb_neon_tiers.py

Ток и уровень Mk I — жёлтый, Mk II — зелёный, Mk III — синий (тёмный, не бирюза синтеза), Mk IV — фиолетовый.
Эхо — красный (er_neon_hot_co Echo Runner), синтез — бирюзовый (er_neon_fusion_hot_co), без энергии — серый.
Узор — от er_neon_co Echo Runner (яркость по красному каналу), цвет — чистый: каналы, которых нет в цвете, строго 0,
иначе днём при свечении ×22 цвет выгорает (E116). Выход: VB_EchoBase/data/eb_neon_t1..t4_co.paa (+ PNG в work).
"""
import os
import subprocess

from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
SRC = os.path.join(ROOT, "..", "mods_vb_vehicles", "mod_vb_EchoRunner", "work", "texsrc", "parts", "er_neon_co.png")
OUT = os.path.join(ROOT, "VB_EchoBase", "data")
PNG = os.path.join(ROOT, "work", "tex")
IMAGE_TO_PAA = r"D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\ImageToPAA\ImageToPAA.exe"

TIERS = {
    1: (1.00, 0.80, 0.00),   # жёлтый — ток и Mk I
    2: (0.00, 1.00, 0.00),   # зелёный — Mk II
    3: (0.00, 0.00, 1.00),   # синий — Mk III
    4: (0.72, 0.00, 1.00),   # фиолетовый — Mk IV
}


def main():
    os.makedirs(OUT, exist_ok=True)
    os.makedirs(PNG, exist_ok=True)
    mask = Image.open(SRC).convert("RGB").split()[0]          # яркость узора 0..255 (красный канал)
    for t, rgb in TIERS.items():
        chans = [mask.point(lambda v, k=k: round(v * k)) for k in rgb]
        im = Image.merge("RGB", chans)
        name = "eb_neon_t%d_co" % t
        png = os.path.join(PNG, name + ".png")
        im.save(png)
        paa = os.path.join(OUT, name + ".paa")
        if os.path.exists(paa):
            os.remove(paa)
        r = subprocess.run([IMAGE_TO_PAA, png, paa], capture_output=True, text=True)
        if not os.path.exists(paa):
            raise RuntimeError("ImageToPAA: %s %s" % (png, r.stdout[-300:] + r.stderr[-300:]))
        print("OK", name, sorted(im.getcolors(1 << 20), reverse=True)[:2])


if __name__ == "__main__":
    main()
