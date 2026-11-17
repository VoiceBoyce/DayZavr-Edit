# Этап 4 ТЗ: перевод конфигов сервера 18 (локальная копия) с BRDK_GoldRush на VB_MiningResources.
# Делает бэкап, правит файлы, кладёт изменённое в «Обновлены или готовые моды\18 сервер».
# Повторный запуск безопасен: уже переведённые файлы не меняются.
# python -I deploy18.py
import datetime, filecmp, io, os, re, shutil, sys
sys.stdout.reconfigure(encoding='utf-8')

BS = chr(92)
MOD = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPO = os.path.dirname(os.path.dirname(MOD))
SRV = 'D:' + BS + 'DayZavr' + BS + 'DayZavr-Servers' + BS + 'DayZServer18'
READY = os.path.join(REPO, 'Обновлены или готовые моды')
READY18 = os.path.join(READY, '18 сервер')
GAME = 'G:' + BS + 'Games' + BS + 'DayZavr'
STAMP = datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S')
BACKUP = os.path.join('G:' + BS, 'DayZ_Test', 'backup', STAMP + '_srv18_mining')

TRADER = 'Instance_1/Trader/TraderConfig12_Master.txt'
CRAFT = 'Instance_1/DRL_CraftMenu/CraftConfig.json'
RF = 'Instance_1/RF/CONTAINERS/PRESETS/Preset_Pool_Items.json'
KT = 'Instance_1/KTPrivateTerritory/Config.json'
BASEBOX = 'Instance_1/INT_Mods/INT_BaseBoxAuthorization/INT_BaseBoxAuthorization_Building_Config.json'
TYPES = 'mpmissions/dayzOffline.enoch/_Types_Server_18/DayZavr/Furniture_BRDK.xml'
MODLISTS = ['modlist_18server.txt', 'ServerManagerPanelModsServer18.txt']
SETTINGS = 'Instance_1/VB_Mods/VB_MiningResources/settings.json'


def p(root, rel):
    return os.path.join(root, *rel.split('/'))


def new_class(name):
    m = re.fullmatch(r'BRDK_Big(\w+)', name)
    if m:
        return 'VB_MR_%s_Bar' % m.group(1)
    m = re.fullmatch(r'BRDK_(\w+)_Ore', name)
    if m:
        return 'VB_MR_%s_Vein' % m.group(1)
    special = {'BRDK_Stone_Press': 'VB_MR_Press', 'BRDK_Press_kit': 'VB_MR_Press_Kit', 'BRDK_MeltingBowl': 'VB_MR_MeltingBowl'}
    if name in special:
        return special[name]
    m = re.fullmatch(r'BRDK_(\w+)', name)
    return 'VB_MR_' + m.group(1)


def read(path):
    with io.open(path, 'rb') as f:
        raw = f.read()
    bom = raw.startswith(b'\xef\xbb\xbf')
    return raw.decode('utf-8-sig'), bom


def write(path, text, bom):
    data = text.encode('utf-8')
    with io.open(path, 'wb') as f:
        f.write((b'\xef\xbb\xbf' if bom else b'') + data)


def backup(rel):
    src = p(SRV, rel)
    dst = p(BACKUP, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copy2(src, dst)


# ---------------------------------------------------------------- преобразования
def tr_trader(t):
    if 'VB_MR_' in t:
        return t
    nl = '\r\n' if '\r\n' in t else '\n'
    old_lines = []
    out = []
    for line in t.split(nl):
        m = re.match(r'^(\s*)(BRDK_\w+)(\s*,\s*)([^,]*?)(\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(.*)$', line)
        if m:
            ind, cls, s1, qty, s2, buy, s3, sell, rest = m.groups()
            comment = rest.strip()
            old_lines.append('\t\t%s,\t\t\t\t%s,\t\t\t\t-1,\t\t\t\t%s\t\t\t%s (старый образец)' % (cls, qty, sell, comment))
            line = ind + new_class(cls) + line[len(ind) + len(cls):]
        out.append(line)
    cat = ['', '\t<Category> Старые ресурсы | Old resources',
           '------------------------------------------------------------ Старый образец, только продажа | Old stock, sell only ----------------------------------------------------------------------------, *, -1,-1',
           '\t\t// Ресурсы и оборудование BRDK_GoldRush, оставшиеся у игроков. Новые не появляются.'] + old_lines + ['']
    i = max(k for k, l in enumerate(out) if l.strip().startswith('//<OpenFile>') or l.strip().startswith('<FileEnd>'))
    while i > 0 and out[i - 1].strip().startswith('//<OpenFile>'):
        i -= 1
    out[i:i] = cat
    return nl.join(out)


def tr_craft(t):
    rx = re.compile(r'\{[^{}]*"Classname"\s*:\s*"BRDK_\w+"[^{}]*\}')

    def fix(m):
        s = re.sub(r'"(BRDK_\w+)"', lambda x: '"%s"' % new_class(x.group(1)), m.group(0))
        return re.sub(r'("UseInheritance"\s*:\s*)0', r'\g<1>1', s)
    t2, n = rx.subn(fix, t)
    assert '"BRDK_' not in t2, 'в крафте остались BRDK вне компонентов'
    return t2


def tr_rf(t):
    return t.replace('"BRDK_MeltingBowl"', '"VB_MR_MeltingBowl"')


def tr_insert(t):
    if 'VB_MR_' in t:
        return t
    t = re.sub(r'^([ \t]*)"BRDK_Stone_Press",(\r?\n)', r'\g<0>\g<1>"VB_MR_Press",\g<2>', t, flags=re.M)
    t = re.sub(r'^([ \t]*)"BRDK_Press_kit",(\r?\n)', r'\g<0>\g<1>"VB_MR_Press_Kit",\g<2>', t, flags=re.M)
    return t


def tr_types(t):
    if 'VB_MR_' in t:
        return t
    nl = '\r\n'
    return nl.join([
        '<?xml version="1.0" encoding="utf-8"?>', '<types>', '',
        '  <!-- VB_MiningResources: электрический пресс -->',
        '  <type name="VB_MR_Press">', '\t\t<lifetime>3888000</lifetime>', '  </type>', '',
        '  <!-- VB_MiningResources: плавильная чаша (есть в спавне) -->',
        '    <type name="VB_MR_MeltingBowl">', '        <nominal>3</nominal>', '        <lifetime>14400</lifetime>',
        '        <restock>0</restock>', '        <min>2</min>', '        <quantmin>-1</quantmin>', '        <quantmax>-1</quantmax>',
        '        <cost>100</cost>',
        '        <flags count_in_cargo="0" count_in_hoarder="0" count_in_map="1" count_in_player="0" crafted="0" deloot="0"/>',
        '        <category name="tools"/>', '\t\t<usage name="Town" />', '\t\t<usage name="Hunting" />', '\t\t<usage name="Village" />',
        '\t\t<usage name="Farm" />', '\t\t<usage name="Industrial" />', '    </type>', '',
        '  <!-- Старые BRDK_GoldRush: только время жизни, в спавне их больше нет -->',
        '  <type name="BRDK_Stone_Press">', '\t\t<lifetime>3888000</lifetime>', '  </type>',
        '  <type name="BRDK_MeltingBowl">', '\t\t<lifetime>14400</lifetime>', '  </type>', '', '</types>', ''])


def tr_modlist(t):
    return re.sub(r'@BRDK_GoldRush(?![\w])', '@VB_MiningResources', t)


# ---------------------------------------------------------------- выполнение
changed = []


def apply(rel, fn):
    path = p(SRV, rel)
    text, bom = read(path)
    new = fn(text)
    if new != text:
        backup(rel)
        write(path, new, bom)
        changed.append(rel)
    # в готовые моды: итоговый файл; если там уже лежит более новая версия — она тоже правится
    ready = p(READY18, rel)
    if os.path.exists(ready):
        rtext, rbom = read(ready)
        rnew = fn(rtext)
        if rnew != rtext:
            write(ready, rnew, rbom)
    elif new != text:
        os.makedirs(os.path.dirname(ready), exist_ok=True)
        shutil.copy2(path, ready)


os.makedirs(BACKUP, exist_ok=True)
apply(TRADER, tr_trader)
apply(CRAFT, tr_craft)
apply(RF, tr_rf)
apply(KT, tr_insert)
apply(BASEBOX, tr_insert)
apply(TYPES, tr_types)
for ml in MODLISTS:
    apply(ml, tr_modlist)

# настройки мода
for root in (SRV, READY18):
    dst = p(root, SETTINGS)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copy2(os.path.join(MOD, 'настройки', 'settings.json'), dst)

# серверная часть: локальная копия 18 и готовые моды (голым .pbo)
addons = p(SRV, '@Servermod/Addons')
for name in os.listdir(addons):
    if name.startswith('BRDK_GoldRush_SERVER'):
        os.makedirs(p(BACKUP, '@Servermod/Addons'), exist_ok=True)
        shutil.move(os.path.join(addons, name), p(BACKUP, '@Servermod/Addons/' + name))
srv_pbo = os.path.join(MOD, '@VB_MiningResources_Server', 'Addons', 'VB_MiningResources_Server.pbo')
shutil.copy2(srv_pbo, addons)
shutil.copy2(srv_pbo, READY18)

# клиентский мод: в готовые моды (Addons без keys) и в папку игры, откуда локальный 18 берёт моды
cli = os.path.join(MOD, '@VB_MiningResources', 'Addons')
for dst in (os.path.join(READY, '@VB_MiningResources', 'Addons'), os.path.join(GAME, '@VB_MiningResources', 'Addons')):
    os.makedirs(dst, exist_ok=True)
    for name in os.listdir(cli):
        shutil.copy2(os.path.join(cli, name), dst)
game_keys = os.path.join(GAME, '@VB_MiningResources', 'keys')
os.makedirs(game_keys, exist_ok=True)
shutil.copy2(os.path.join(MOD, '@VB_MiningResources', 'keys', 'DayZavrN.bikey'), game_keys)

print('бэкап:', BACKUP)
print('изменено в локальной копии 18:')
for c in changed:
    print('  ', c)
