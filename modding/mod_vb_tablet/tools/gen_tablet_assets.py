"""Собирает текстуры VB_Tablet_Data из ассетов макета html_tablet.

Атлас значков: все маски из assets/icons.css рисует браузер (часть значков SVG), затем они
складываются в один белый атлас с альфой, рядом пишется imageset.
Корпус и стекло растягиваются до степени двойки: ImageWidget растянет их обратно.

Запуск: python gen_tablet_assets.py <временная папка>
"""
import base64
import io
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import uuid

from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HTML = os.path.join(ROOT, 'html_tablet', 'assets')
DATA = os.path.join(ROOT, 'VB_Tablet_Data', 'gui')
EDGE = r'C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe'
TO_PAA = r'D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\ImageToPAA\ImageToPAA.exe'
CELL = 64
PAD = 2
ATLAS = 1024
# Эмблемы крупные, им нужен свой атлас: 64 пикселя на 170 на экране смотрятся мылом.
BIG_CELL = 256
BIG_ATLAS = 2048


def render_icons(tmp, cell, only=None):
    css = open(os.path.join(HTML, 'icons.css'), encoding='utf-8').read()
    icons = re.findall(r'^\.i-([a-z0-9_]+)\{[^}]*?url\((data:image/[^)]+)\)', css, flags=re.M)
    if only:
        icons = [i for i in icons if i[0].startswith(only)]
    page = """<!doctype html><meta charset=utf-8><pre id=r></pre><script>
const L=%s,R={};let n=0;
L.forEach(([k,u])=>{const i=new Image();i.onload=()=>{const c=document.createElement('canvas');c.width=c.height=%d;const g=c.getContext('2d');g.drawImage(i,0,0,%d,%d);g.globalCompositeOperation='source-in';g.fillStyle='#fff';g.fillRect(0,0,%d,%d);R[k]=c.toDataURL('image/png').split(',')[1];if(++n===L.length)document.getElementById('r').textContent='@@'+JSON.stringify(R)+'@@';};i.src=u;});
</script>""" % (json.dumps(icons), cell, cell, cell, cell, cell)
    f = os.path.join(tmp, 'icons.html')
    open(f, 'w', encoding='utf-8').write(page)
    prof = os.path.join(tempfile.gettempdir(), 'edge_a_' + uuid.uuid4().hex)
    url = 'file:///' + f.replace('\\', '/')
    out = subprocess.run([EDGE, '--headless=new', '--disable-gpu', '--allow-file-access-from-files', '--user-data-dir=' + prof, '--virtual-time-budget=12000', '--dump-dom', url], capture_output=True, timeout=120).stdout.decode('utf-8', 'replace')
    shutil.rmtree(prof, ignore_errors=True)
    data = json.loads(re.search(r'<pre id="r">@@(.*?)@@</pre>', out, flags=re.S).group(1).replace('&quot;', '"'))
    assert len(data) == len(icons), (len(data), len(icons))
    return data


def to_paa(png, paa):
    os.makedirs(os.path.dirname(paa), exist_ok=True)
    r = subprocess.run([TO_PAA, png, paa], capture_output=True, timeout=120)
    assert os.path.exists(paa), r.stdout.decode('utf-8', 'replace')[-300:]


def main():
    tmp = sys.argv[1]
    os.makedirs(tmp, exist_ok=True)
    icons = render_icons(tmp, CELL)
    per = (ATLAS) // (CELL + PAD * 2)
    assert (len(icons) // per + 1) * (CELL + PAD * 2) <= ATLAS, 'значки не влезают в атлас'
    atlas = Image.new('RGBA', (ATLAS, ATLAS), (255, 255, 255, 0))
    rows = []
    for i, (name, b64) in enumerate(sorted(icons.items())):
        x = (i % per) * (CELL + PAD * 2) + PAD
        y = (i // per) * (CELL + PAD * 2) + PAD
        atlas.paste(Image.open(io.BytesIO(base64.b64decode(b64))).convert('RGBA'), (x, y))
        rows.append('  ImageSetDefClass %s {\n   Name "%s"\n   Pos %d %d\n   Size %d %d\n   Flags 0\n  }\n' % (name, name, x, y, CELL, CELL))
    png = os.path.join(tmp, 'vb_tablet_icons_ca.png')
    atlas.save(png)
    to_paa(png, os.path.join(DATA, 'textures', 'vb_tablet_icons_ca.paa'))
    os.makedirs(os.path.join(DATA, 'imagesets'), exist_ok=True)
    with open(os.path.join(DATA, 'imagesets', 'vb_tablet.imageset'), 'w', encoding='ascii', newline='\n') as f:
        f.write('ImageSetClass {\n Name "vb_tablet"\n RefSize %d %d\n Textures {\n  ImageSetTextureClass {\n   mpix 1\n   path "VB_Tablet_Data/gui/textures/vb_tablet_icons_ca.paa"\n  }\n }\n Images {\n' % (ATLAS, ATLAS))
        f.write(''.join(rows))
        f.write(' }\n}\n')
    big = render_icons(tmp, BIG_CELL, ('shp_', 'sym_'))
    per = BIG_ATLAS // (BIG_CELL + PAD * 2)
    assert (len(big) // per + 1) * (BIG_CELL + PAD * 2) <= BIG_ATLAS, 'эмблемы не влезают в атлас'
    atlas = Image.new('RGBA', (BIG_ATLAS, BIG_ATLAS), (255, 255, 255, 0))
    rows = []
    for i, (name, b64) in enumerate(sorted(big.items())):
        x = (i % per) * (BIG_CELL + PAD * 2) + PAD
        y = (i // per) * (BIG_CELL + PAD * 2) + PAD
        atlas.paste(Image.open(io.BytesIO(base64.b64decode(b64))).convert('RGBA'), (x, y))
        rows.append('  ImageSetDefClass %s {\n   Name "%s"\n   Pos %d %d\n   Size %d %d\n   Flags 0\n  }\n' % (name, name, x, y, BIG_CELL, BIG_CELL))
    # Блик и уголки тоже в атласе: у отдельной текстуры край заворачивается и даёт тёмный шов.
    # Вокруг рабочей области запас тех же значений, поэтому выборка у границы чистая.
    gy = 1830
    for x in range(1040):
        k = max(0.0, 1 - max(0, x - 8) / (1024 * 0.58))
        for y in range(16):
            atlas.putpixel((x, gy + y), (255, 255, 255, int(255 * k)))
    rows.append('  ImageSetDefClass glow {\n   Name "glow"\n   Pos 8 %d\n   Size 1024 8\n   Flags 0\n  }\n' % (gy + 4))
    for name, cx, cy in (('corner_t', 1060, 256), ('corner_b', 1220, 0)):
        k = Image.new('L', (512, 512), 0)
        for py in range(512):
            for px in range(512):
                if (px + 0.5 - 512) ** 2 + (py + 0.5 - cy * 2) ** 2 <= 512 ** 2:
                    k.putpixel((px, py), 255)
        k = k.resize((128, 128), Image.LANCZOS)
        cell = Image.new('RGBA', (144, 144), (255, 255, 255, 0))
        cell.putalpha(0)
        top = 8
        for py in range(128):
            for px in range(128):
                cell.putpixel((px + 8, py + top), (255, 255, 255, k.getpixel((px, py))))
            # Справа запас непрозрачный, как продолжение уголка.
            for px in range(128, 144 - 8 + 8):
                cell.putpixel((min(143, px + 8), py + top), (255, 255, 255, k.getpixel((127, py))))
        # Запас сверху или снизу: край, которым уголок примыкает к полосе, не должен темнеть.
        rows_from, rows_to = (135, range(136, 144)) if name == 'corner_t' else (8, range(0, 8))
        for py in rows_to:
            for px in range(144):
                cell.putpixel((px, py), cell.getpixel((px, rows_from)))
        atlas.paste(cell, (cx - 8, gy - 8 + 24))
        rows.append('  ImageSetDefClass %s {\n   Name "%s"\n   Pos %d %d\n   Size 128 128\n   Flags 0\n  }\n' % (name, name, cx, gy + 24))
    # Сплошной участок: Line фреймворка растягивает крошечный спрайт, и его край расплывается на десятки пикселей.
    atlas.paste(Image.new('RGBA', (64, 64), (255, 255, 255, 255)), (1400, 1846))
    rows.append('  ImageSetDefClass solid {\n   Name "solid"\n   Pos 1416 1862\n   Size 32 32\n   Flags 0\n  }\n')
    png = os.path.join(tmp, 'vb_tablet_emblem_ca.png')
    atlas.save(png)
    to_paa(png, os.path.join(DATA, 'textures', 'vb_tablet_emblem_ca.paa'))
    with open(os.path.join(DATA, 'imagesets', 'vb_tablet_em.imageset'), 'w', encoding='ascii', newline='\n') as f:
        f.write('ImageSetClass {\n Name "vb_tablet_em"\n RefSize %d %d\n Textures {\n  ImageSetTextureClass {\n   mpix 1\n   path "VB_Tablet_Data/gui/textures/vb_tablet_emblem_ca.paa"\n  }\n }\n Images {\n' % (BIG_ATLAS, BIG_ATLAS))
        f.write(''.join(rows))
        f.write(' }\n}\n')
    for name in ('device', 'glass'):
        src = Image.open(os.path.join(HTML, name + '.png')).convert('RGBA').resize((2048, 1024), Image.LANCZOS)
        p = os.path.join(tmp, 'vb_tablet_%s_ca.png' % name)
        src.save(p)
        to_paa(p, os.path.join(DATA, 'textures', 'vb_tablet_%s_ca.paa' % name))
    hz = Image.new('RGBA', (1024, 16), (255, 255, 255, 0))
    for y in range(16):
        for x in range(1024):
            if (x + y) % 40 < 20:
                hz.putpixel((x, y), (255, 255, 255, 255))
    p = os.path.join(tmp, 'vb_tablet_hazard_ca.png')
    hz.save(p)
    to_paa(p, os.path.join(DATA, 'textures', 'vb_tablet_hazard_ca.paa'))
    print('значков', len(icons))


if __name__ == '__main__':
    main()
