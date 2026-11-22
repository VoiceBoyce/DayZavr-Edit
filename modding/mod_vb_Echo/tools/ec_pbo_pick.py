# -*- coding: utf-8 -*-
"""Выборочная распаковка большого PBO, в том числе запутанного JAPM (OOTS_ClothesPack, 1,13 млн записей-приманок):
только записи, имя которых начинается с одного из префиксов. Модели и текстуры у JAPM лежат под настоящими именами.

  python -I ec_pbo_pick.py <file.pbo> <outdir> <prefix1> [prefix2 ...]     (префиксы без учёта регистра, через \\)

Для экзо-сета VB_EchoClothes (11-23):
  python -I ec_pbo_pick.py ...\\OOTS_ClothesPack.pbo <out> "Model\\Backpacks\\ExoPrivod" "Model\\Boots\\ExoBoots"
     "Model\\Gloves\\ExoGloves" "Model\\Mask\\MaskGhost" "Model\\Pants\\ExoPants" "Model\\Tops\\ExoJacket" "Model\\Vest\\ExoVest"
"""
import os
import struct
import sys

pbo, outdir = sys.argv[1], sys.argv[2]
prefixes = [p.lower().replace('/', '\\') for p in sys.argv[3:]]


def lzss(src, outlen):
    out = bytearray()
    i = 0
    while len(out) < outlen and i < len(src):
        flags = src[i]
        i += 1
        for bit in range(8):
            if len(out) >= outlen or i >= len(src):
                break
            if flags & (1 << bit):
                out.append(src[i])
                i += 1
            else:
                b1, b2 = src[i], src[i + 1]
                i += 2
                rpos = len(out) - (b1 | ((b2 & 0xF0) << 4))
                rlen = (b2 & 0x0F) + 3
                for _ in range(rlen):
                    if len(out) >= outlen:
                        break
                    out.append(out[rpos] if rpos >= 0 else 0x20)
                    rpos += 1
    return bytes(out)


with open(pbo, 'rb') as fh:
    head = fh.read(256 * 1024 * 1024)       # заголовок JAPM огромный (~130 МБ), но меньше 256 МБ

pos = 0


def cstr():
    global pos
    e = head.index(b'\0', pos)
    s = head[pos:e]
    pos = e + 1
    return s


entries = []
while True:
    name = cstr()
    method, orig, res, ts, size = struct.unpack_from('<5I', head, pos)
    pos += 20
    if method == 0x56657273 and not name:
        while True:
            k = cstr()
            if not k:
                break
            cstr()
        continue
    if not name and method == 0 and size == 0:
        break
    entries.append((name, method, orig, size))

picked = []
off = pos
for name, method, orig, size in entries:
    n = name.decode('utf-8', 'replace')
    if any(n.lower().startswith(p) for p in prefixes):
        picked.append((n, method, orig, size, off))
    off += size
print('записей', len(entries), 'подходит', len(picked))

with open(pbo, 'rb') as fh:
    for n, method, orig, size, o in picked:
        fh.seek(o)
        raw = fh.read(size)
        body = lzss(raw, orig) if (method == 0x43707273 and orig) else raw
        p = os.path.join(outdir, *n.split('\\'))
        os.makedirs(os.path.dirname(p), exist_ok=True)
        with open(p, 'wb') as f:
            f.write(body)
print('готово:', outdir)
