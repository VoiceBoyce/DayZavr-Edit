# -*- coding: utf-8 -*-
"""Общий types семейства Эхо для сервера 16: VB_Echo.xml из types_*.xml модов.

  python make_types16.py

Решение пользователя 2026-11-25: types на сервере — **только транспорт и механизмы**. Источники (их и править):
mod_vb_Echo/types_VB_EchoCore.xml (энергоячейки), types_VB_EchoBase.xml (устройства базы, наборы, кристаллы),
mods_vb_vehicles/types_VB_EchoVehicles.xml (Echo Runner, детали, баки, трубопроводы, антиграв, аккумуляторы).
Одежда (types_VB_EchoClothes.xml) и материалы (types_VB_EchoMaterials.xml) на 16 без types — файлы остаются в модах
для других серверов. Результат — mod_vb_Echo/types_16/VB_Echo.xml; на сервере 16 он лежит в
mpmissions/dayzOffline.sakhal/_Types_Server_16/DayZavr/ и подключён строкой в cfgeconomycore.xml
(<file name="VB_Echo.xml" type="types" />). Проверяет: XML читается, имена не повторяются.
"""
import io
import os
import sys
import xml.etree.ElementTree as ET

HERE = os.path.dirname(os.path.abspath(__file__))
ECHO = os.path.normpath(os.path.join(HERE, ".."))
SRC = [
    ("@VB_EchoCore", os.path.join(ECHO, "types_VB_EchoCore.xml")),
    ("@VB_EchoBase", os.path.join(ECHO, "types_VB_EchoBase.xml")),
    ("@VB_EchoVehicles", os.path.join(ECHO, "..", "mods_vb_vehicles", "types_VB_EchoVehicles.xml")),
]
OUT = os.path.join(ECHO, "types_16", "VB_Echo.xml")


def main():
    seen = {}
    parts = []
    for mod, path in SRC:
        root = ET.parse(path).getroot()
        assert root.tag == "types", path
        names = [t.get("name") for t in root.findall("type")]
        for n in names:
            assert n not in seen, ("повтор", n, mod, seen.get(n))
            seen[n] = mod
        body = []
        for t in root.findall("type"):
            t.tail = None
            ET.indent(t, space="    ", level=1)
            body.append("    " + ET.tostring(t, encoding="unicode"))
        parts.append("    <!-- ================= %s (%d) -->\n%s" % (mod, len(names), "\n".join(body)))
        print("%-18s %d" % (mod, len(names)))
    text = ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?>\n'
            "<!-- Семейство Эхо для сервера 16: только транспорт и механизмы (решение 2026-11-25), одежда и материалы без types.\n"
            "     Собрано mod_vb_Echo/tools/make_types16.py, руками не править.\n"
            "     Подключение: cfgeconomycore.xml, папка _Types_Server_16/DayZavr, <file name=\"VB_Echo.xml\" type=\"types\" />. -->\n"
            "<types>\n" + "\n".join(parts) + "\n</types>\n")
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with io.open(OUT, "w", encoding="utf-8", newline="\r\n") as f:
        f.write(text)
    assert len(ET.parse(OUT).getroot()) == len(seen)
    print("всего %d -> %s" % (len(seen), OUT))


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    main()
