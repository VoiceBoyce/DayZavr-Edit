"""Данные для макета крафта DRL: рецепты/категории сервера 18 -> data.js, icons.js, img/sets.
Запуск после правок конфига сервера: python -I _gen/make_mock_data.py "<папка макета>"
"""
import base64, io, json, os, re, sys
from PIL import Image

OUT = sys.argv[1]
SRV = r'D:\DayZavr\DayZavr-Servers\DayZServer18\Instance_1\DRL_CraftMenu'
ICONS = r'D:\DayZavr\DayZavr-Edit\modding\mod_vb_DRL_Core\DRL_Core\Icon'
ITEMS = r'D:\DayZavr\DayZavr-Edit\modding\mod_vb_clothes_dayzavr\craft\Предметы для крафта.txt'
SETS = r'D:\DayZavr\DayZavr-Edit\modding\mod_vb_clothes_dayzavr\картинки сетов'
V2 = r'D:\DayZavr\DayZavr-Edit\modding\mod_vb_DRL_CraftMenu\ui_redesign\v2\assets'

os.makedirs(os.path.join(OUT, 'img', 'ic'), exist_ok=True)
os.makedirs(os.path.join(OUT, 'img', 'sets'), exist_ok=True)

cfg = json.load(open(os.path.join(SRV, 'CraftConfig.json'), encoding='utf-8'))
cats = json.load(open(os.path.join(SRV, 'CraftCategories.json'), encoding='utf-8'))
settings = json.load(open(os.path.join(SRV, 'CraftSettings.json'), encoding='utf-8'))

# --- названия предметов -------------------------------------------------------------
names = {}
c_name = c_role = None
for line in open(ITEMS, encoding='utf-8'):
    line = line.rstrip('\n')
    if line.startswith('класснейм'):           # столбцы — по шапке таблицы
        c_name, c_role = line.index('название'), line.index('роль')
        continue
    m = re.match(r'^([A-Za-z_]\w*)\s', line)
    if m and c_name:
        nm_ = line[c_name:c_role].strip()
        if nm_:
            names[m.group(1)] = nm_
# пустые в списке — из названий рецептов, где предмет результат (руны, артефакты, MSFZ)
for r in cfg:
    for cls in [r.get('Result')] + (r.get('ResultClasses') or []):
        if cls and cls not in names:
            names[cls] = r.get('RecipeName') or cls
STATION_ITEMS = {
    'DRL_Craft_Vise': 'Тиски', 'DRL_Craft_Anvil': 'Наковальня', 'DRL_Craft_Anvil2': 'Наковальня',
    'DRL_Craft_DrillSmal': 'Сверлильный станок', 'DRL_Craft_Tools': 'Набор инструментов',
    'DRL_Craft_lab_bunsen': 'Штатив', 'DRL_Craft_Microscope': 'Микроскоп', 'DRL_Craft_Box': 'Ящик для деталей',
    'DRL_Craft_Sewing_Table': 'Швейный стол', 'DRL_Craft_Sewing_scissors': 'Ножницы',
    'DRL_Craft_Metalic_thimble': 'Шпуля', 'DRL_Craft_Car_Jack': 'Домкрат', 'DRL_Craft_Sewing': 'Швейная машинка',
    'MetalPlate': 'Листовой металл',
}
names.update({k: v for k, v in STATION_ITEMS.items() if k not in names or k.startswith('DRL_')})

# --- иконки: ключевое слово в классе -> иконка DRL_Core ----------------------------------
RULES = [
    ('sewing_table', 'drl_sewing-machine'), ('scissors', 'drl_scissors_lorc'), ('thimble', 'drl_sewing-string'),
    ('car_jack', 'drl_tire-iron'), ('vise', 'drl_gear-hammer'), ('anvil', 'drl_anvil'), ('drill', 'drl_drill_lorc'),
    ('microscope', 'drl_microscope'), ('lab_bunsen', 'drl_round-bottom-flask'), ('drl_craft_tools', 'drl_toolbox'),
    ('drl_craft_box', 'drl_cardboard-box-closed'),
    ('cloth_', 'drl_rolled-cloth'), ('rag', 'drl_rolled-cloth'), ('fleece', 'drl_rolled-cloth'), ('paracord', 'drl_sewing-string'),
    ('rope', 'drl_sewing-string'), ('metalparts', 'drl_gears'), ('bolts', 'drl_screw_lorc'), ('nuts', 'drl_hexagonal-nut'),
    ('screw_pack', 'drl_screw_delapouite'), ('nail', 'drl_coiled-nail'), ('wire', 'drl_wire-coil'), ('cable', 'drl_wire-coil'),
    ('powercord', 'drl_wire-coil'), ('battery', 'drl_car-battery'), ('wheel', 'drl_car-wheel'), ('tire', 'drl_car-wheel'),
    ('skoth', 'drl_crime-scene-tape'), ('tape', 'drl_crime-scene-tape'), ('glass', 'drl_cracked-glass'),
    ('leather', 'drl_leather-armor'), ('rune', 'drl_rune-stone'), ('titanium', 'drl_crystal-cluster'),
    ('brdk_', 'drl_crystal-cluster'), ('crystal', 'drl_crystal-cluster'), ('_art_', 'drl_fire-gem'), ('oots_', 'drl_fire-gem'),
    ('eft_sewing', 'drl_sewing-needle'), ('shilo', 'drl_needle-drill'), ('wrench', 'drl_monkey-wrench'),
    ('hammer', 'drl_claw-hammer'), ('saw', 'drl_crosscut-saw'), ('pliers', 'drl_bolt-cutter'), ('cutter', 'drl_bolt-cutter'),
    ('screwdriver', 'drl_screwdriver'), ('blowtorch', 'drl_torch'), ('sparkplug', 'drl_spark-plug'), ('oil', 'drl_jerrycan'),
    ('gasolin', 'drl_jerrycan'), ('radiator', 'drl_cog'), ('engine', 'drl_boat-engine'), ('magnet', 'drl_magnet'),
    ('capacitor', 'drl_circuitry'), ('rele', 'drl_circuitry'), ('headlight', 'drl_light-bulb'), ('hose', 'drl_pipes'),
    ('tube', 'drl_pipes'), ('pipe', 'drl_pipes'), ('chain', 'drl_gem-chain'), ('badge', 'drl_police-badge'),
    ('feather', 'drl_feather'), ('bone', 'drl_broken-bone'), ('filtr', 'drl_gas-mask_lorc'), ('foam', 'drl_spray'),
    ('poxearm', 'drl_spray'), ('klean', 'drl_spray'), ('gauge', 'drl_cog'), ('plate', 'drl_metal-plate'),
    ('mask', 'drl_gas-mask_skoll'), ('helmet', 'drl_brodie-helmet'), ('_head', 'drl_brodie-helmet'), ('vest', 'drl_kevlar-vest'),
    ('jacket', 'drl_moncler-jacket'), ('_top', 'drl_moncler-jacket'), ('pants', 'drl_trousers_lorc'), ('boots', 'drl_boots'),
    ('gloves', 'drl_gloves'), ('backpack', 'drl_backpack'), ('bag', 'drl_backpack'), ('crossbow', 'drl_crossbow'),
    ('knife', 'drl_bowie-knife_lorc'), ('blade', 'drl_broadsword'), ('sword', 'drl_broadsword'), ('ammo', 'drl_ammo-box'),
    ('bolt', 'drl_arrow-flights'), ('door', 'drl_car-door'), ('hood', 'drl_car-door'), ('trunk', 'drl_car-door'),
    ('kovh', 'drl_car-door'), ('bumper', 'drl_car-door'), ('interceptor', 'drl_police-car'), ('mustang', 'drl_horse-head_lorc'),
    ('dodge', 'drl_death-skull'), ('scout', 'drl_crossbow'), ('belt', 'drl_sewing-string'),
]
def icon_for(cls):
    c = cls.lower()
    for k, ic in RULES:
        if k in c:
            return ic
    return 'drl_cog'

used_icons = set()
def ic(name):
    used_icons.add(name)
    return name

# --- рецепты ----------------------------------------------------------------------
STATION = {'DRL_Craft_Workbench': 'wb', 'DRL_Craft_Sewing': 'sew', 'DRL_Craft_Truck_Lift': 'lift'}
recipes = {}
for r in cfg:
    comps = []
    for c in r.get('CraftComponents') or []:
        cls = c.get('Classname') or (c.get('Classnames') or [''])[0]
        ax = c.get('AttachmentsEx') or 0
        if ax == 1:
            kind = 'a'            # навес станции
        elif not c.get('Delete'):
            kind = 't'            # инструмент: не тратится, изнашивается
        else:
            kind = 'c'            # расход
        comps.append({
            'cls': cls, 'n': c.get('Amount') or 1, 'k': kind, 'ax': ax,
            'wear': round((c.get('ApplyDamageFrac') or 0) * 100), 'mh': c.get('MinHealthFrac', 3),
            'qf': c.get('QuantityFrac') or 0, 'ch': c.get('RecraftDropChance') or 0,
            'inh': c.get('UseInheritance') or 0,
            'vars': [v for v in (c.get('Classnames') or []) if v != cls][:4],
            'ic': icon_for(cls),
        })
        names.setdefault(cls, cls)
        ic(icon_for(cls))
    st = STATION.get((r.get('WorkbenchClasses') or [''])[0], 'wb')
    rid = r['RecipeId']
    res = r.get('Result') or ''
    recipes[rid] = {
        'id': rid, 'st': st, 'free': r.get('ShowInFreeMenu') or 0, 'name': r.get('RecipeName') or res,
        'res': res, 'vars': r.get('ResultClasses') or [], 'desc': r.get('Description') or '',
        't': r.get('CraftTimeSec') or 0, 'rt': r.get('RecraftTimeSec') or 0, 'cnt': r.get('ResultCount') or 1,
        'qf': r.get('ResultQuantityFrac') or 0, 'dmg': r.get('ResultDamageFrac') or 0,
        'att': r.get('AttachedRecipeIds') or [], 'comps': comps, 'ic': ic(icon_for(res)),
    }
    names.setdefault(res, r.get('RecipeName') or res)

# --- категории по станциям ----------------------------------------------------------
st_cats = {'wb': [], 'sew': [], 'lift': [], 'free': []}
for c in cats:
    icon = os.path.splitext(os.path.basename(c.get('Icon') or ''))[0] or 'drl_cog'
    ic(icon)
    if c.get('ShowAll') == 1:
        for st in st_cats:
            ids = [rid for rid, r in recipes.items() if (r['st'] == st if st != 'free' else r['free'])]
            st_cats[st].insert(0, {'name': c.get('Name') or 'Все рецепты', 'ic': icon, 'ids': ids, 'all': 1})
        continue
    ids = [i for i in (c.get('List') or []) if i in recipes]
    if not ids:
        continue
    st = recipes[ids[0]]['st']
    st_cats[st].append({'name': c.get('Name'), 'ic': icon, 'ids': ids})
    free_ids = [i for i in ids if recipes[i]['free']]
    if free_ids:
        st_cats['free'].append({'name': c.get('Name'), 'ic': icon, 'ids': free_ids})

# варианты ингредиентов для состояния «Вариант ингредиента» (в конфиге сервера их пока нет)
for extra in ('OOTS_ART_Drop', 'OOTS_ART_Crustal', 'EFT_Cloth_ripstop', 'EFT_Cloth_fleece', 'EFT_PowerCord', 'CableReel'):
    ic(icon_for(extra))
data = {
    'names': names, 'recipes': recipes, 'cats': st_cats,
    'maxQueue': settings.get('MaxQueueSlots', 5),
    'icons': {k: icon_for(k) for k in ('OOTS_ART_Drop', 'OOTS_ART_Crustal', 'EFT_Cloth_ripstop', 'EFT_Cloth_fleece', 'EFT_PowerCord', 'CableReel')},
}
with open(os.path.join(OUT, 'data.js'), 'w', encoding='utf-8') as f:
    f.write('// Данные макета: рецепты и категории сервера 18 (DRL_CraftMenu), собраны make_mock_data.py\n')
    f.write('window.DRL = ' + json.dumps(data, ensure_ascii=False, separators=(',', ':')) + ';\n')

# --- иконки: DDS -> PNG (белая маска, красится в CSS) -----------------------------------------
miss = []
for name in sorted(used_icons):
    src = os.path.join(ICONS, name + '.edds')
    if not os.path.exists(src):
        miss.append(name)
        continue
    im = Image.open(src).convert('RGBA')
    a = im.getchannel('A')
    if a.getextrema() == (255, 255):     # без альфы — маска по яркости
        a = im.convert('L')
    out = Image.new('RGBA', im.size, (255, 255, 255, 0))
    out.putalpha(a)
    out.save(os.path.join(OUT, 'img', 'ic', name + '.png'))
print('recipes', len(recipes), {k: len(v) for k, v in st_cats.items()}, 'icons', len(used_icons), 'missing', miss)

# иконки интерфейса (не из рецептов) + упаковка всех в icons.js (data URI: CSS-маска с file:// иначе не грузится)
for name in ('drl_sewing-machine', 'drl_car-key', 'drl_open-book', 'drl_tire-iron', 'drl_scroll-unfurled', 'drl_metal-plate'):
    src = os.path.join(ICONS, name + '.edds')
    if os.path.exists(src):
        im = Image.open(src).convert('RGBA')
        a = im.getchannel('A')
        if a.getextrema() == (255, 255):
            a = im.convert('L')
        out = Image.new('RGBA', im.size, (255, 255, 255, 0))
        out.putalpha(a)
        out.save(os.path.join(OUT, 'img', 'ic', name + '.png'))
packed = {}
icdir = os.path.join(OUT, 'img', 'ic')
for f in sorted(os.listdir(icdir)):
    im = Image.open(os.path.join(icdir, f)).convert('RGBA').resize((64, 64), Image.LANCZOS)
    b = io.BytesIO()
    im.save(b, 'PNG', optimize=True)
    packed[f[:-4]] = 'data:image/png;base64,' + base64.b64encode(b.getvalue()).decode()
with open(os.path.join(OUT, 'icons.js'), 'w', encoding='utf-8') as f:
    f.write('// Иконки DRL_Core (DDS -> PNG 64px, белая маска) — data URI, чтобы CSS-маска работала с file://\n')
    f.write('window.ICONS=' + json.dumps(packed) + ';\n')
print('icons.js', len(packed))

# --- картинки сетов (манекен швейной машинки) ------------------------------------------------
for f in os.listdir(SETS):
    if not f.lower().startswith(('set', 'setcomplete')):
        continue
    im = Image.open(os.path.join(SETS, f)).convert('RGB')
    h = 560
    im = im.resize((round(im.width * h / im.height), h), Image.LANCZOS)
    im.save(os.path.join(OUT, 'img', 'sets', os.path.splitext(f)[0].lower() + '.jpg'), quality=86)
print('sets ok')
