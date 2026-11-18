# Переносит модели и текстуры старого BRDK_GoldRush_CLIENT в исходники VB_MiningResources\m\.
# Запускается один раз (повторный запуск перезапишет папку m\).
#
# Пути внутри бинарных моделей (ODOL) хранятся строками с нулём на конце, поэтому меняем их
# байтами на строку той же длины: brdk_goldrush_client (20) -> vb_miningresources\m (20).
# Материалы .rvmat в старом PBO бинарные (raP): переводим в текст DeRap и меняем путь в тексте.
#
# python import_assets.py
import os, re, shutil, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'старый мод', 'BRDK_GoldRush_CLIENT')
DST = os.path.join(ROOT, 'VB_MiningResources', 'm')
FOLDERS = ['KIT', 'Kamens', 'metall', 'pot', 'press', 'stones']   # Diamonds (огранённые камни) убраны 2026-11-18
DERAP = r'C:\Program Files (x86)\Mikero\DePboTools\bin\DeRap.exe'

OLD = b'brdk_goldrush_client'
NEW = b'vb_miningresources\\m'
assert len(OLD) == len(NEW)
RE_OLD_BIN = re.compile(re.escape(OLD), re.I)
RE_OLD_TXT = re.compile(r'brdk_goldrush_client', re.I)


def patch_p3d(src, dst):
    data = open(src, 'rb').read()
    if data[:4] != b'ODOL':
        raise SystemExit('не ODOL: ' + src)
    new, n = RE_OLD_BIN.subn(lambda m: NEW, data)
    assert len(new) == len(data)
    open(dst, 'wb').write(new)
    return n


def convert_rvmat(src, dst):
    data = open(src, 'rb').read()
    if data[:4] == b'\0raP':
        tmp = dst + '.bin'
        shutil.copyfile(src, tmp)
        # DeRap пишет рядом файл с тем же именем и расширением .cpp
        r = subprocess.run([DERAP, '-P', tmp], capture_output=True, text=True)
        out = tmp[:-4] + '.cpp'
        if not os.path.exists(out):
            raise SystemExit('DeRap не справился: %s\n%s%s' % (src, r.stdout, r.stderr))
        text = open(out, encoding='utf-8', errors='replace').read()
        os.remove(tmp); os.remove(out)
        # шапку DeRap (комментарии и #define _ARMA_) вырезаем
        lines = text.splitlines()
        while lines and (lines[0].startswith('//') or lines[0].startswith('#define') or not lines[0].strip()):
            lines.pop(0)
        text = '\n'.join(lines) + '\n'
    else:
        text = data.decode('utf-8', errors='replace')
    text, n = RE_OLD_TXT.subn(lambda m: 'VB_MiningResources\\m', text)
    open(dst, 'w', encoding='utf-8', newline='\r\n').write(text)
    return n


def main():
    if os.path.isdir(DST):
        shutil.rmtree(DST)
    total = {'p3d': 0, 'rvmat': 0, 'copy': 0, 'refs': 0}
    for folder in FOLDERS:
        for dirpath, _, files in os.walk(os.path.join(SRC, folder)):
            rel = os.path.relpath(dirpath, SRC)
            out_dir = os.path.join(DST, rel)
            os.makedirs(out_dir, exist_ok=True)
            for name in files:
                s = os.path.join(dirpath, name)
                d = os.path.join(out_dir, name)
                ext = name.lower().rsplit('.', 1)[-1]
                if ext == 'p3d':
                    total['refs'] += patch_p3d(s, d); total['p3d'] += 1
                elif ext == 'rvmat':
                    total['refs'] += convert_rvmat(s, d); total['rvmat'] += 1
                else:
                    shutil.copyfile(s, d); total['copy'] += 1
    print(total)


if __name__ == '__main__':
    main()
