# -*- coding: utf-8 -*-
"""Ужать текстуры детекторов до 1024 (отбросить верхние мипмапы, данные не пережимаются).

Детекторы OOTS пришли с картами 2048×2048 — около 90 МБ на 5 ручных предметов. Для предмета в руках хватает 1024,
это вчетверо меньше. Папки t/do, t/dk, t/dm, t/ds, t/dv (общие карты) и t/<код>x (цвет, экраны, лампы).
Вызывается в конце ab_detectors.py и ab_anomalies.py (лампа Отклика), можно и отдельно: python ab_trim.py
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ab_models as am  # noqa: E402

T = os.path.join(HERE, "..", "VB_AnomaliesBosses", "t")
DETECTOR_DIRS = ["do", "dox", "dk", "dkx", "dm", "dmx", "ds", "dsx", "dv", "dvx"]
MAX_DIM = 1024


def paa_dim(path):
    d = open(path, "rb").read(4096)
    i = 2
    while d[i:i + 4] == b"GGAT":
        i += 12 + int.from_bytes(d[i + 8:i + 12], "little")
    i += 2 + int.from_bytes(d[i:i + 2], "little") * 3
    return int.from_bytes(d[i:i + 2], "little") & 0x7FFF, int.from_bytes(d[i + 2:i + 4], "little")


def trim_detectors(max_dim=MAX_DIM, quiet=False):
    before = after = n = 0
    for sub in DETECTOR_DIRS:
        d = os.path.join(T, sub)
        if not os.path.isdir(d):
            continue
        for f in os.listdir(d):
            if not f.lower().endswith(".paa"):
                continue
            p = os.path.join(d, f)
            size = os.path.getsize(p)
            before += size
            if max(paa_dim(p)) > max_dim:
                am.paa_trim(p, p, max_dim)
                n += 1
            after += os.path.getsize(p)
    if not quiet:
        print("текстуры детекторов: ужато %d, %.1f -> %.1f МБ" % (n, before / 1048576.0, after / 1048576.0))


if __name__ == "__main__":
    trim_detectors()
