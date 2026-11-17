# Своя распаковка PBO (с LZSS «Cprs»), имена с мусорными символами экранируются.
# python pbox.py <file.pbo> <outdir>
import sys, os, struct, re
sys.stdout.reconfigure(encoding='utf-8')

def lzss(src, outlen):
    out = bytearray()
    i = 0
    while len(out) < outlen and i < len(src):
        flags = src[i]; i += 1
        for bit in range(8):
            if len(out) >= outlen or i >= len(src):
                break
            if flags & (1 << bit):
                out.append(src[i]); i += 1
            else:
                b1, b2 = src[i], src[i + 1]; i += 2
                rpos = len(out) - (b1 | ((b2 & 0xF0) << 4))
                rlen = (b2 & 0x0F) + 3
                for _ in range(rlen):
                    if len(out) >= outlen:
                        break
                    out.append(out[rpos] if rpos >= 0 else 0x20)
                    rpos += 1
    return bytes(out)

data = open(sys.argv[1], 'rb').read()
outdir = sys.argv[2]
pos = 0
def cstr():
    global pos
    e = data.index(b'\0', pos)
    s = data[pos:e]; pos = e + 1
    return s
entries, headers = [], []
while True:
    name = cstr()
    method, orig, res, ts, size = struct.unpack_from('<5I', data, pos); pos += 20
    if method == 0x56657273 and not name:
        while True:
            k = cstr()
            if not k: break
            headers.append((k, cstr()))
        continue
    if not name and method == 0 and size == 0:
        break
    entries.append((name, method, orig, size))
off = pos
for k, v in headers:
    if k.strip() == b'prefix':
        print('prefix =', v.decode('utf-8', 'replace'))
for name, method, orig, size in entries:
    raw = data[off:off + size]; off += size
    if method == 0x43707273 and orig:
        body = lzss(raw, orig)
    else:
        body = raw
    n = name.decode('utf-8', errors='replace').replace('\\', '/')
    n = ''.join(ch if (ch.isprintable() and ch not in '<>:"|?*') else '%%%02x' % ord(ch) for ch in n)
    p = os.path.join(outdir, *n.split('/'))
    os.makedirs(os.path.dirname(p), exist_ok=True)
    open(p, 'wb').write(body)
print('files', len(entries))
