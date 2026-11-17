# Проверяет, что все пути vb_miningresources\m\... в моделях и материалах ведут на существующие файлы
# и что нигде не осталось ссылок на старый мод brdk_goldrush_client.
# python check_refs.py
import os, re, glob, sys
sys.stdout.reconfigure(encoding='utf-8')

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'VB_MiningResources')
BS = chr(92)

refs, old = {}, []
files = [p for p in glob.glob(os.path.join(SRC, '**', '*'), recursive=True) if os.path.isfile(p)]
for p in files:
    data = open(p, 'rb').read()
    if p.lower().endswith(('.p3d', '.rvmat', '.cpp')) and re.search(rb'brdk_goldrush_client', data, re.I):
        old.append(os.path.relpath(p, SRC))
    for m in re.finditer(rb'vb_miningresources\\m\\[^\x00"\r\n]+', data, re.I):
        refs.setdefault(m.group(0).decode('latin1').lower(), set()).add(os.path.basename(p))

existing = set()
for p in files:
    rel = os.path.relpath(p, os.path.dirname(SRC)).replace('/', BS).lower()
    existing.add(rel)

missing = sorted(r for r in refs if r not in existing)
print('ссылок:', len(refs), '| нет файла:', len(missing), '| старый путь в:', old or 'нигде')
for r in missing:
    print('  нет:', r, '<-', sorted(refs[r])[:3])
sys.exit(1 if missing or old else 0)
