# -*- coding: utf-8 -*-
"""Красные молнии Изнанки: копии ванильных blesk1.p3d / blesk2.p3d с перекрашенным встроенным материалом.

  python gen_rift_lightning.py
Модели молний ванильные и скомпилированные (ODOL v56). Геометрия белая (#(argb...)color(1,1,1)),
цвет даёт материал, встроенный в ODOL сразу после имени: uint32 версия, затем emmisive, ambient,
diffuse, forcedDiffuse (по 4 float). Скрипт:
  1) меняет имя материала на путь той же длины в нашем PBO (dz\\data\\data\\blesk.rvmat ->
     vb_riftsky\\d\\blesk.rvmat), длина файла и смещения не меняются;
  2) умножает RGB этих четырёх цветов на TINT;
  3) кладёт рядом такие же красные rvmat — на случай, если движок грузит материал по имени.
Итог: VB_RiftSky/data/vb_blesk1.p3d, vb_blesk2.p3d, VB_RiftSky/d/blesk.rvmat, blesk2.rvmat.
"""
import os
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
SKY_DIR = os.path.normpath(os.path.join(HERE, "..", "VB_RiftSky"))
BS = chr(92)
SRC_DIR = "P:" + BS + "DZ" + BS + "data" + BS + "data" + BS

# Молния очень яркая (emmisive 3, ambient/diffuse 10), а ночью игра ещё поднимает экспозицию: при G/B 0.22/0.32
# и даже 0.06/0.10 главный канал выгорал в белый (ответвления с emmisive 1.3 уже были красными) — проверено
# на стенде 2026-11-11. С B 0.02 канал стал малиново-фиолетовым: любой ненулевой канал выжигается в полный.
# Поэтому чисто красный.
TINT = (1.0, 0.0, 0.0)

RENAME = {
    b"dz" + bytes([92]) + b"data" + bytes([92]) + b"data" + bytes([92]) + b"blesk.rvmat":
        b"vb_riftsky" + bytes([92]) + b"d" + bytes([92]) + b"blesk.rvmat",
    b"dz" + bytes([92]) + b"data" + bytes([92]) + b"data" + bytes([92]) + b"blesk2.rvmat":
        b"vb_riftsky" + bytes([92]) + b"d" + bytes([92]) + b"blesk2.rvmat",
}

# исходные rvmat (P:\DZ\data\data\blesk*.rvmat): emmisive 3 / 1.3, ambient, diffuse, forcedDiffuse по 10
RVMAT = """ambient[] = {%(a)s,1.0};
diffuse[] = {%(a)s,1.0};
forcedDiffuse[] = {%(a)s,0.0};
emmisive[] = {%(e)s,1.0};
specular[] = {0.0,0.0,0.0,0.0};
specularPower = 0.0;
PixelShaderID = "Normal";
VertexShaderID = "Basic";
"""


def rgb(v):
    return ",".join("%.3f" % (v * t) for t in TINT)


def patch(data):
    data = bytearray(data)
    hits = 0
    for old, new in RENAME.items():
        assert len(old) == len(new), (old, new)
        start = 0
        while True:
            i = data.find(old + b"\x00", start)
            if i < 0:
                break
            data[i:i + len(old)] = new
            o = i + len(old) + 1
            ver = struct.unpack_from("<I", data, o)[0]
            assert ver == 0x14, "неожиданная версия встроенного материала %#x" % ver
            o += 4
            for k in range(4):              # emmisive, ambient, diffuse, forcedDiffuse
                r, g, b, a = struct.unpack_from("<4f", data, o + k * 16)
                struct.pack_into("<4f", data, o + k * 16, r * TINT[0], g * TINT[1], b * TINT[2], a)
            hits += 1
            start = i + len(new)
    return bytes(data), hits


def main():
    os.makedirs(os.path.join(SKY_DIR, "data"), exist_ok=True)
    os.makedirs(os.path.join(SKY_DIR, "d"), exist_ok=True)
    for name in ("blesk1", "blesk2"):
        src = open(SRC_DIR + name + ".p3d", "rb").read()
        out, hits = patch(src)
        assert hits >= 1 and len(out) == len(src)
        dst = os.path.join(SKY_DIR, "data", "vb_" + name + ".p3d")
        open(dst, "wb").write(out)
        print("MODEL", dst, len(out), "материалов перекрашено:", hits)
    for name, e in (("blesk", 3.0), ("blesk2", 1.3)):
        p = os.path.join(SKY_DIR, "d", name + ".rvmat")
        with open(p, "w", encoding="utf-8", newline="\r\n") as f:
            f.write(RVMAT % {"a": rgb(10.0), "e": rgb(e)})
        print("RVMAT", p)


main()
