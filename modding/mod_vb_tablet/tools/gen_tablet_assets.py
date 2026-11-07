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


def render_icons(tmp):
    css = open(os.path.join(HTML, 'icons.css'), encoding='utf-8').read()
    icons = re.findall(r'^\.i-([a-z0-9_]+)\{[^}]*?url\((data:image/[^)]+)\)', css, flags=re.M)
    page = """<!doctype html><meta charset=utf-8><pre id=r></pre><script>
const L=%s,R={};let n=0;
L.forEach(([k,u])=>{const i=new Image();i.onload=()=>{const c=document.createElement('canvas');c.width=c.height=%d;const g=c.getContext('2d');g.drawImage(i,0,0,%d,%d);g.globalCompositeOperation='source-in';g.fillStyle='#fff';g.fillRect(0,0,%d,%d);R[k]=c.toDataURL('image/png').split(',')[1];if(++n===L.length)document.getElementById('r').textContent='@@'+JSON.stringify(R)+'@@';};i.src=u;});
</script>""" % (json.dumps(icons), CELL, CELL, CELL, CELL, CELL)
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
    icons = render_icons(tmp)
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
    for name in ('device', 'glass'):
        src = Image.open(os.path.join(HTML, name + '.png')).convert('RGBA').resize((2048, 1024), Image.LANCZOS)
        p = os.path.join(tmp, 'vb_tablet_%s_ca.png' % name)
        src.save(p)
        to_paa(p, os.path.join(DATA, 'textures', 'vb_tablet_%s_ca.paa' % name))
    # Блик карточки: слева направо от плотного к пустому. Сигнальная полоса: косые штрихи.
    glow = Image.new('RGBA', (512, 8), (255, 255, 255, 0))
    for x in range(512):
        k = max(0.0, 1 - x / (512 * 0.58))
        for y in range(8):
            glow.putpixel((x, y), (255, 255, 255, int(255 * k)))
    p = os.path.join(tmp, 'vb_tablet_glow_ca.png')
    glow.save(p)
    to_paa(p, os.path.join(DATA, 'textures', 'vb_tablet_glow_ca.paa'))
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
