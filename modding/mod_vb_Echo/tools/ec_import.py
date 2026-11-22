# -*- coding: utf-8 -*-
"""Перенос экзо-сета в VB_EchoClothes (ТЗ_VB_EchoClothes.md, этап 1, 11-23): модели, текстуры, материалы из OOTS и IMP
в свою папку exo_set, пути на свой PBO.

  python -I ec_import.py --oots=<распакованное ec_pbo_pick.py (папка с Model)> --imp=<IMPGMOD распакованный> --out=<VB_EchoClothes>

Модели бинаризованные (ODOL): путь в них менять можно только строкой той же длины (иначе сдвинутся смещения LOD),
поэтому префиксы подобраны по длине:
  oots_clothespack\\model\\                    (23) -> vb_echoclothes\\exo_set\\                    (23)
  impgmod\\equipment\\helmet\\devtacroninheavy\\ (42) -> vb_echoclothes\\exo_set\\helmet\\ronin_heavy\\ (42)
  impgmod\\equipment\\helmet\\devtacroninlight\\ (42) -> vb_echoclothes\\exo_set\\helmet\\ronin_light\\ (42)
Материалы (.rvmat) — текстом (у IMP бинарные: сначала DeRap). Чужие расцветки фракций и пояс ExoBelt не берём.
"""
import os
import re
import shutil
import subprocess
import sys
import tempfile

args = dict(a[2:].split('=', 1) for a in sys.argv[1:] if a.startswith('--') and '=' in a)
OOTS = args['oots']
IMP = args['imp']
OUT = args['out']
DERAP = r'C:\Program Files (x86)\Mikero\DePboTools\bin\DeRap.exe'

P3D_RULES = [
    (b'oots_clothespack\\model\\', b'vb_echoclothes\\exo_set\\'),
    (b'impgmod\\equipment\\helmet\\devtacroninheavy\\', b'vb_echoclothes\\exo_set\\helmet\\ronin_heavy\\'),
    (b'impgmod\\equipment\\helmet\\devtacroninlight\\', b'vb_echoclothes\\exo_set\\helmet\\ronin_light\\'),
]
TEXT_RULES = [
    (r'\\?oots_clothespack\\model\\', r'VB_EchoClothes\\exo_set\\'),
    (r'\\?impgmod\\equipment\\helmet\\devtacroninheavy\\', r'VB_EchoClothes\\exo_set\\helmet\\ronin_heavy\\'),
    (r'\\?impgmod\\equipment\\helmet\\devtacroninlight\\', r'VB_EchoClothes\\exo_set\\helmet\\ronin_light\\'),
]
for a, b in P3D_RULES:
    assert len(a) == len(b), (a, b)

# OOTS: какие папки и что из них не брать
OOTS_DIRS = ['Backpacks/ExoPrivod', 'Boots/ExoBoots', 'Gloves/ExoGloves', 'Mask/MaskGhost', 'Pants/ExoPants',
             'Tops/ExoJacket', 'Vest/ExoVest']
# ExoPrivod2_co — нужна (выделение Pr2 сервопривода); ExoJacket2/ExoPants2/ExoPrivod22/33 — чужие расцветки
SKIP = re.compile(r'(Band|CN|Dolg|Free|Merk|Mono)_co\.paa$|^(ExoJacket2|ExoPants2|ExoPrivod22|ExoPrivod33)_co\.paa$'
                  r'|^ExoBelt', re.I)

stats = {'p3d': 0, 'paa': 0, 'rvmat': 0, 'замен в p3d': 0}


def patch_p3d(src, dst):
    data = bytearray(open(src, 'rb').read())
    low = bytes(data).lower()
    n = 0
    for a, b in P3D_RULES:
        start = 0
        while True:
            i = low.find(a, start)
            if i < 0:
                break
            data[i:i + len(a)] = b
            n += 1
            start = i + len(a)
    rest = bytes(data).lower()
    assert b'oots_clothespack' not in rest and b'impgmod' not in rest, ('остался старый путь', src)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    open(dst, 'wb').write(bytes(data))
    stats['p3d'] += 1
    stats['замен в p3d'] += n
    return n


def text_rvmat(src):
    raw = open(src, 'rb').read()
    if raw.startswith(b'\0raP'):                 # бинарный материал (IMP) — в текст через DeRap
        tmp = tempfile.mkdtemp()
        b = os.path.join(tmp, 'm.bin')
        shutil.copy(src, b)
        subprocess.run([DERAP, '-P', b], capture_output=True)
        raw = open(os.path.join(tmp, 'm.cpp'), 'rb').read()
        shutil.rmtree(tmp, ignore_errors=True)
    t = raw.decode('utf-8', 'replace')
    t = '\n'.join(l for l in t.splitlines() if not l.startswith('//') and l.strip() != '#define _ARMA_')
    for a, b in TEXT_RULES:
        t = re.sub(a, b, t, flags=re.I)
    assert 'oots_clothespack' not in t.lower() and 'impgmod' not in t.lower(), src
    return t.strip() + '\n'


def put_rvmat(src, dst):
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    open(dst, 'w', encoding='utf-8', newline='\n').write(text_rvmat(src))
    stats['rvmat'] += 1


def put_file(src, dst):
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copy2(src, dst)
    stats['paa'] += 1


def copy_tree(src_dir, dst_dir, only=None):
    for root, _, files in os.walk(src_dir):
        for f in files:
            if SKIP.search(f) or (only and not only(f)):
                continue
            s = os.path.join(root, f)
            d = os.path.join(dst_dir, os.path.relpath(s, src_dir))
            ext = f.lower().rsplit('.', 1)[-1]
            if ext == 'p3d':
                print('  %-34s замен путей: %d' % (f, patch_p3d(s, d)))
            elif ext == 'rvmat':
                put_rvmat(s, d)
            elif ext == 'paa':
                put_file(s, d)


EXO = os.path.join(OUT, 'exo_set')
for sub in OOTS_DIRS:
    print(sub)
    copy_tree(os.path.join(OOTS, 'Model', *sub.split('/')), os.path.join(EXO, *sub.split('/')))

print('Helmet/ronin_heavy (IMP)')
ronin = os.path.join(IMP, 'Equipment', 'Helmet', 'DevtacRoninHeavy')
copy_tree(ronin, os.path.join(EXO, 'helmet', 'ronin_heavy'),
          only=lambda f: not re.search(r'_(Green|Tan)\.paa$', f, re.I))
put_file(os.path.join(IMP, 'Equipment', 'Helmet', 'DevtacRoninLight', 'data', 'glass_ca.paa'),
         os.path.join(EXO, 'helmet', 'ronin_light', 'data', 'glass_ca.paa'))
print(stats)
