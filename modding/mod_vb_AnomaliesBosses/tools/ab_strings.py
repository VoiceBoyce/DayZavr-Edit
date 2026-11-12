# -*- coding: utf-8 -*-
"""stringtable.csv мода VB_AnomaliesBosses. Файл генерируется (CRLF, UTF-8).
Строки: словарь STRINGS ниже + все tools/strings/*.json (формат {"STR_KEY": ["русский", "english"], ...}).
Каждая часть мода (детекторы, скины, материалы, трофеи) ведёт свой json, чтобы не править один файл.
Запуск: python ab_strings.py
"""
import glob
import json
import os

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "..", "VB_AnomaliesBosses", "stringtable.csv")
LANGS = ["original", "english", "czech", "german", "russian", "polish", "hungarian", "italian", "spanish",
         "french", "chinese", "japanese", "portuguese", "chinesesimp"]

# ключ: (русский, английский)
STRINGS = {
    # действия
    "STR_VB_AB_ActionCutTrophy": ("Вырезать трофей", "Carve a trophy"),
    # детекторы
    "STR_VB_AB_Detector_Otklik": ("Детектор «Отклик»", "Detector \"Otklik\""),
    "STR_VB_AB_Detector_Otklik_Desc": (
        "Самый простой детектор аномальных находок. Ловит только то, что впереди, и подсказывает лишь расстояние: "
        "чем ближе находка, тем чаще писк и ярче лампа. Работает от батарейки 9 В.",
        "The simplest detector of anomalous finds. Senses only what is ahead and tells just the distance: "
        "the closer the find, the faster the beeping and the brighter the lamp. Runs on a 9V battery."),
}


def q(s):
    return '"' + s.replace('"', '""') + '"'


def main():
    allstr = dict(STRINGS)
    for fn in sorted(glob.glob(os.path.join(HERE, "strings", "*.json"))):
        with open(fn, encoding="utf-8") as f:
            for key, val in json.load(f).items():
                if key in allstr:
                    raise SystemExit("ключ %s повторяется (%s)" % (key, os.path.basename(fn)))
                allstr[key] = (val[0], val[1])
    lines = [",".join(q(x) for x in ["Language"] + LANGS) + ","]
    for key, (ru, en) in allstr.items():
        row = [key]
        for lang in LANGS:
            row.append(ru if lang in ("original", "russian") else en)
        lines.append(",".join(q(x) for x in row) + ",")
    with open(OUT, "w", encoding="utf-8", newline="\r\n") as f:
        f.write("\n".join(lines) + "\n")
    print("строк:", len(allstr))


main()
