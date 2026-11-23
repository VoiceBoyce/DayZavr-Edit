# -*- coding: utf-8 -*-
"""Крепления сервопривода экзо (VB_EchoClothes, 11-23): переименовать прокси в модели на свои, чтобы на сервоприводе
были видны две энергоячейки Эхо и свой контейнер артефактов и не было конфликта с OOTS (у OOTS — ProxyExoPprivodBattery
и ArtCase мода OOTS_Artifacts).

  python -I ec_servo_proxies.py <VB_EchoClothes>          (после ec_import.py; повторный запуск ничего не меняет)

В модели OOTS две батареи — две прокси с одним именем `exopprivodbattery` (номера 1 и 2) и кейс `\\dd_artifacts\\...\\artcase`.
ODOL: имя прокси — строка с нулём, за ней матрица 12 float и номер прокси (i32). Длина строк не меняется:
  exopprivodbattery (17)  -> vb_exocell_slot_1 / vb_exocell_slot_2 (по номеру прокси)
  \\dd_artifacts\\model\\cases\\caseart\\artcase (41) -> \\vb_echoclothes\\exo_set\\vb_ec_artcase_box (41)
Классы креплений в config.cpp: Proxyvb_exocell_slot_1/2, Proxyvb_ec_artcase_box. Файлы-заглушки моделей креплений —
копии ExoPprivodBattery.p3d (движок рисует на креплении модель вложенного предмета, а не заглушку).
"""
import os
import shutil
import struct
import sys

ROOT = sys.argv[1]
SERVO = os.path.join(ROOT, 'exo_set', 'Backpacks', 'ExoPrivod')
BAT_DIR = b'\\vb_echoclothes\\exo_set\\backpacks\\exoprivod\\'
OLD_BAT = b'exopprivodbattery'
NEW_BAT = {1: b'vb_exocell_slot_1', 2: b'vb_exocell_slot_2'}
OLD_ART = b'\\dd_artifacts\\model\\cases\\caseart\\artcase'
NEW_ART = b'\\vb_echoclothes\\exo_set\\vb_ec_artcase_box'
assert all(len(v) == len(OLD_BAT) for v in NEW_BAT.values())
assert len(NEW_ART) == len(OLD_ART), (len(NEW_ART), len(OLD_ART))


def patch(path):
    data = bytearray(open(path, 'rb').read())
    n_proxy = n_sel = n_art = 0
    full = BAT_DIR + OLD_BAT
    i = 0
    # 1) записи прокси: имя + \0 + матрица 48 байт + номер прокси
    while True:
        i = bytes(data).find(full + b'\0', i)
        if i < 0:
            break
        pid = struct.unpack_from('<i', data, i + len(full) + 1 + 48)[0]
        if pid not in NEW_BAT:
            raise SystemExit('%s: неожиданный номер прокси батареи %d' % (path, pid))
        s = i + len(BAT_DIR)
        data[s:s + len(OLD_BAT)] = NEW_BAT[pid]
        n_proxy += 1
        i += len(full)
    # 2) имена выделений прокси: proxy:<путь>.00N
    for pid, new in NEW_BAT.items():
        old_sel = full + b'.%03d' % pid
        new_sel = BAT_DIR + new + b'.%03d' % pid
        while True:
            j = bytes(data).find(old_sel)
            if j < 0:
                break
            data[j:j + len(old_sel)] = new_sel
            n_sel += 1
    # 3) кейс артефактов — одна прокси, имя везде одинаковое
    while True:
        j = bytes(data).find(OLD_ART)
        if j < 0:
            break
        data[j:j + len(OLD_ART)] = NEW_ART
        n_art += 1
    rest = bytes(data)
    if OLD_BAT in rest or b'dd_artifacts' in rest:
        raise SystemExit('%s: остались старые имена креплений' % path)
    if n_proxy or n_sel or n_art:
        open(path, 'wb').write(bytes(data))
    print('%-22s прокси батарей %d, выделений %d, кейс %d' % (os.path.basename(path), n_proxy, n_sel, n_art))


for f in sorted(os.listdir(SERVO)):
    if f.lower().endswith('.p3d') and f.lower().startswith('exoprivod_'):
        patch(os.path.join(SERVO, f))

# заглушки моделей креплений
src = os.path.join(SERVO, 'ExoPprivodBattery.p3d')
for dst in (os.path.join(SERVO, 'vb_exocell_slot_1.p3d'), os.path.join(SERVO, 'vb_exocell_slot_2.p3d'),
            os.path.join(ROOT, 'exo_set', 'vb_ec_artcase_box.p3d')):
    if not os.path.exists(dst):
        shutil.copy2(src, dst)
        print('заглушка', os.path.relpath(dst, ROOT))
