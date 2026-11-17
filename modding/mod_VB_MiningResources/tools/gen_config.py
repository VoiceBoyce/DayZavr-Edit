# Генератор VB_MiningResources/config.cpp и stringtable.csv.
# Параметры предметов перенесены из BRDK_GoldRush (старый мод/_разбор/client_config.cpp), правки ТЗ v0.3:
#   И6 — брикеты рубина, серы, нефрита 300 г; И8 — цепочка Bronze называется «медь»;
#   пакет 2 — огранённые рубин и нефрит; пакет 3 — новые модели и описания новых ресурсов.
# Старые классы BRDK_* — наследники новых VB_MR_*: своё старое название, старая модель и «Старый образец» в описании.
# Новые модели берутся из VB_MiningResources/data/items/mapping.json (делает tools/gen_mr_models.py), если он есть.
# python -I gen_config.py
import os, csv, sys, json
sys.stdout.reconfigure(encoding='utf-8')

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'VB_MiningResources')
OLD_ST = os.path.join(ROOT, 'старый мод', 'BRDK_GoldRush_CLIENT', 'stringtable.csv')
MAPPING = os.path.join(OUT, 'data', 'items', 'mapping.json')
BS = chr(92)
M = 'VB_MiningResources' + BS + 'm' + BS

METALS = ['Gold', 'Silver', 'Bronze', 'Iron', 'Titanium']
STONES = ['Coal', 'Rubin', 'Nephrite', 'Sulfur']
ALL = METALS + STONES

OLD_SUFFIX_RU = ' Старый образец.'
OLD_SUFFIX_EN = ' Old stock.'

# ---------------------------------------------------------------- тексты новых ресурсов (пакет 3, на утверждение)
# (название RU, описание RU, название EN, описание EN)
# (основа для «...ого песка», род. падеж металла, прилагательное м. р.)
METAL_RU = {'Gold': ('золот', 'золота', 'золотой'), 'Silver': ('серебрян', 'серебра', 'серебряный'), 'Bronze': ('медн', 'меди', 'медный'),
            'Iron': ('железн', 'железа', 'железный'), 'Titanium': ('титанов', 'титана', 'титановый')}
METAL_EN = {'Gold': 'gold', 'Silver': 'silver', 'Bronze': 'copper', 'Iron': 'iron', 'Titanium': 'titanium'}
STONE_SAND = {'Coal': ('угольной крошки', 'Угольная крошка', 'кусок угля', 'coal grit', 'a lump of coal'),
              'Rubin': ('рубиновой крошки', 'Мелкие осколки рубина', 'необработанный рубин', 'ruby grit', 'a rough ruby'),
              'Nephrite': ('нефритовой крошки', 'Мелкие осколки нефрита', 'кусок нефрита', 'jade grit', 'a jade chunk'),
              'Sulfur': ('серной крошки', 'Серная крошка', 'кусок серы', 'sulfur grit', 'a sulfur chunk')}
STONE_CHUNK = {'Coal': ('Кусок угля', 'Кусок каменного угля', 'брикет угля', 'Lump of coal', 'coal briquette'),
               'Rubin': ('Необработанный рубин', 'Рубин', 'брикет рубина', 'Rough ruby', 'ruby briquette'),
               'Nephrite': ('Кусок нефрита', 'Кусок нефрита', 'брикет нефрита', 'Jade chunk', 'jade briquette'),
               'Sulfur': ('Кусок серы', 'Кусок серы', 'брикет серы', 'Sulfur chunk', 'sulfur briquette')}
STONE_BAR = {'Coal': ('Брикет угля', 'Прессованный каменный уголь. Горит в костре дольше дров.', 'Coal briquette', 'Pressed coal. Burns in a fire longer than firewood.'),
             'Rubin': ('Брикет рубина', 'Прессованная рубиновая порода. Ценный материал для крафта и товар для торговцев.', 'Ruby briquette', 'Pressed ruby rock. A valuable crafting material and trade good.'),
             'Nephrite': ('Брикет нефрита', 'Прессованная нефритовая порода. Ценный материал для крафта и товар для торговцев.', 'Jade briquette', 'Pressed jade rock. A valuable crafting material and trade good.'),
             'Sulfur': ('Брикет серы', 'Прессованная сера. Нужна на производстве, ценится у торговцев.', 'Sulfur briquette', 'Pressed sulfur. Used in production, valued by traders.')}


def new_texts(r, form):
    if form == 'sand':
        if r in METALS:
            g1, g2, adj = METAL_RU[r]
            return ('Мешочек %sого песка' % g1, 'Крупицы %s, намытые в реке. 10 штук плавятся в плавильной чаше в %s самородок.' % (g2, adj),
                    'Pouch of %s sand' % METAL_EN[r], '%s grains washed from a river. Melt 10 in a melting bowl to get a %s nugget.' % (METAL_EN[r].capitalize(), METAL_EN[r]))
        n, d, res, en_n, en_res = STONE_SAND[r]
        return ('Мешочек ' + n, '%s, намытые в реке. 10 штук прессуются в прессе в %s.' % (d, res),
                'Pouch of ' + en_n, '%s washed from a river. Press 10 in a press to get %s.' % (en_n.capitalize(), en_res))
    if form == 'nugget':
        if r in METALS:
            g1, g2, adj = METAL_RU[r]
            return ('%s самородок' % adj.capitalize(), 'Самородок %s из рудной жилы или плавильной чаши. 5 штук плавятся в чаше в слиток %s.' % (g2, g2),
                    '%s nugget' % METAL_EN[r].capitalize(), 'A %s nugget from an ore vein or a melting bowl. Melt 5 in a melting bowl to get a %s ingot.' % (METAL_EN[r], METAL_EN[r]))
        n, d, res, en_n, en_res = STONE_CHUNK[r]
        return (n, '%s из рудной жилы или пресса. 5 штук прессуются в прессе в %s.' % (d, res),
                en_n, '%s from an ore vein or a press. Press 5 to get a %s.' % (en_n, en_res))
    if r in METALS:
        g1, g2, adj = METAL_RU[r]
        return ('Слиток %s' % g2, 'Литой слиток %s с клеймом. Ценный материал для крафта и товар для торговцев.' % g2,
                '%s ingot' % METAL_EN[r].capitalize(), 'A cast %s ingot with a stamp. A valuable crafting material and trade good.' % METAL_EN[r])
    return STONE_BAR[r]


GEM_TEXTS = {'Rubin': ('Огранённый рубин', 'Редкая находка в рубиновой жиле. Дорого ценится у торговцев.', 'Cut ruby', 'A rare find in a ruby vein. Traders pay well for it.'),
             'Nephrite': ('Огранённый нефрит', 'Редкая находка в нефритовой жиле. Дорого ценится у торговцев.', 'Cut jade', 'A rare find in a jade vein. Traders pay well for it.')}

INFO_TEXTS = {
    'STR_VB_MR_INFO_PRESS': ('Прессовка: %1/%2 с', 'Pressing: %1/%2 s'),
    'STR_VB_MR_INFO_NEED': ('Нужно %1 шт., лежит %2', 'Need %1 pcs, have %2'),
    'STR_VB_MR_INFO_MELT': ('Плавка: %1/%2 с, %3°', 'Melting: %1/%2 s, %3°'),
    'STR_VB_MR_INFO_COLD': ('Чаша холодная: %1° из %2°', 'The bowl is cold: %1° of %2°'),
}

# И8: исправления старых текстов
OLD_FIX = {'STR_BRDK_BigBronze0': ('Большой слиток меди', 'Large copper ingot'),
           'STR_BRDK_BigBronze1': ('Большой слиток меди, полученный путем сплава из нескольких кусков меньшего размера',
                                   'A large copper ingot obtained by alloying several smaller pieces')}

HP_ITEM = '\t\tclass DamageSystem\n\t\t{\n\t\t\tclass GlobalHealth\n\t\t\t{\n\t\t\t\tclass Health\n\t\t\t\t{\n\t\t\t\t\thitpoints = %d;\n\t\t\t\t\thealthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{%s,{}}};\n\t\t\t\t};\n\t\t\t};\n\t\t};\n'


def dmg_mat(hp, mats):
    lv = ','.join('{%s,{"%s"}}' % (lvl, mat) for lvl, mat in mats)
    return ('\t\tclass DamageSystem\n\t\t{\n\t\t\tclass GlobalHealth\n\t\t\t{\n\t\t\t\tclass Health\n\t\t\t\t{\n'
            '\t\t\t\t\thitpoints = %d;\n\t\t\t\t\thealthLevels[] = {%s};\n\t\t\t\t};\n\t\t\t};\n\t\t};\n') % (hp, lv)


def five(ok, dam, des):
    return [('1.0', ok), ('0.7', ok), ('0.5', dam), ('0.3', dam), ('0.1', des)]


mapping = json.load(open(MAPPING, encoding='utf-8')) if os.path.exists(MAPPING) else {}


def model_lines(cls, old_model):
    """Новая модель с цветом через camo, если она уже есть; иначе старая."""
    m = mapping.get(cls)
    if not m:
        return '\t\tmodel = "%s";\n' % old_model
    return ('\t\tmodel = "%s";\n\t\thiddenSelections[] = {"camo"};\n\t\thiddenSelectionsTextures[] = {"%s"};\n'
            '\t\thiddenSelectionsMaterials[] = {"%s"};\n' % (m['model'], m['texture'], m['material']))


# (новый класс, старый класс или None, база, ключ старой строки, тело нового, старая модель, тексты нового)
items = []
units = []
strings = []          # (новый ключ, старый ключ) — копия всех языков
strings_old = []      # (новый ключ, старый ключ) — описание + «Старый образец»
strings_new = []      # (ключ, RU, EN) — новые тексты


def add_item(new, old, base, old_str, body, old_model, texts):
    items.append((new, old, base, body, old_model))
    units.append(new)
    key = new[6:]
    strings_new.append(('STR_VB_MR_%s_N' % key, texts[0], texts[2]))
    strings_new.append(('STR_VB_MR_%s_D' % key, texts[1], texts[3]))
    if old:
        units.append(old)
        strings.append(('STR_VB_MR_%s_NO' % key, old_str + '0'))
        strings_old.append(('STR_VB_MR_%s_DO' % key, old_str + '1'))


for r in ALL:
    stone = r in STONES
    folder = 'Kamens' if stone else 'metall'
    add_item('VB_MR_' + r, 'BRDK_' + r, 'VB_MR_StoneNugget_Base' if stone else 'VB_MR_Nugget_Base', 'STR_BRDK_' + r,
             '\t\tanimClass = "NoFireClass";\n\t\tweight = 500;\n\t\titemSize[] = {2,2};\n\t\trotationFlags = 17;\n%s' % (HP_ITEM % (9900, '0.1')),
             '%s%s\\Smal%s.p3d' % (M, folder, r), new_texts(r, 'nugget'))
    add_item('VB_MR_%s_Sand' % r, 'BRDK_%s_Sand' % r, 'VB_MR_StoneSand_Base' if stone else 'VB_MR_Sand_Base', 'STR_BRDK_%s_Sand' % r,
             '\t\tanimClass = "Compass";\n\t\titemSize[] = {1,1};\n\t\tweight = 190;\n%s' % (HP_ITEM % (9900, '0.1')),
             '%s%s\\Sand%s.p3d' % (M, folder, r), new_texts(r, 'sand'))
    if not stone:
        body = ('\t\tanimClass = "NoFireClass";\n\t\tweight = 5500;\n\t\titemSize[] = {3,3};\n\t\tisMeleeWeapon = 1;\n%s' % (HP_ITEM % (9900, '0.1')))
        base, old_model = 'VB_MR_MetalBar_Base', '%smetall\\%sBigStone.p3d' % (M, r)
    elif r == 'Coal':
        body = ('\t\tinventorySlot[] += {"Firewood"};\n\t\tcanBeSplit = 1;\n\t\tvarQuantityInit = 1;\n'
                '\t\tvarQuantityMin = 0;\n\t\tvarQuantityMax = 6;\n\t\tvarQuantityDestroyOnMin = 1;\n\t\tvarStackMax = 1;\n\t\tweight = 300;\n\t\titemSize[] = {2,2};\n%s'
                % (HP_ITEM % (50, '0.0')))
        base, old_model = 'VB_MR_StoneBar_Base', '%sKamens\\BigCoalStone.p3d' % M
    else:
        body = '\t\tweight = 300;\n\t\titemSize[] = {2,2};\n%s' % (HP_ITEM % (50, '0.0'))
        base, old_model = 'VB_MR_StoneBar_Base', '%sKamens\\Big%sStone.p3d' % (M, r)
    add_item('VB_MR_%s_Bar' % r, 'BRDK_Big' + r, base, 'STR_BRDK_Big' + r, body, old_model, new_texts(r, 'bar'))

# пакет 2: огранённые камни (модели Diamonds из старого мода)
for r in ('Rubin', 'Nephrite'):
    add_item('VB_MR_%s_Gem' % r, None, 'VB_MR_Gem_Base', None,
             '\t\tweight = 50;\n\t\titemSize[] = {1,1};\n\t\trotationFlags = 17;\n%s' % (HP_ITEM % (9900, '0.1')),
             '%sDiamonds\\%sDiamond.p3d' % (M, r), GEM_TEXTS[r])

# оборудование (модели и тексты прежние)
equipment = []
kit_mats = five(M + 'KIT\\data\\box_kit.rvmat', M + 'KIT\\data\\box_kitDam.rvmat', M + 'KIT\\data\\box_kitDes.rvmat')
equipment.append(('VB_MR_Press_Kit', 'BRDK_Press_kit', 'Inventory_Base', 'STR_Press_kit',
                  '\t\tmodel = "%sKIT\\BRDK_Press_Kit.p3d";\n\t\tprojectionTypename = "VB_MR_Press";\n\t\tallowOwnedCargoManipulation = 1;\n'
                  '\t\thologramMaterial = "fishnettrap";\n\t\thologramMaterialPath = "dz\\gear\\traps\\data";\n\t\toverrideDrawArea = 8.0;\n'
                  '\t\tforcarBubble = "true";\n\t\tdestroyOnEmpty = 0;\n\t\tcarveNavmesh = 1;\n\t\theavyItem = 1;\n\t\tweight = 1000;\n\t\titemBehaviour = 1;\n'
                  '\t\tphysLayer = "item_large";\n\t\titemSize[] = {5,3};\n\t\tlootTag[] = {"Work"};\n%s' % (M, dmg_mat(500, kit_mats))))
press_mats = five(M + 'press\\data\\Press.rvmat', M + 'press\\data\\PressDam.rvmat', M + 'press\\data\\PressDes.rvmat')
equipment.append(('VB_MR_Press', 'BRDK_Stone_Press', 'Inventory_Base', 'STR_BRDK_Stone_Press',
                  '\t\tmodel = "%spress\\BRDK_Press.p3d";\n\t\tphysLayer = "item_large";\n\t\tattachments[] = {"BRDK_STONES"};\n\t\tforcarBubble = "true";\n'
                  '\t\thandheld = "false";\n\t\tweight = 1000000;\n\t\tuseEntityHierarchy = "true";\n\t\tyawPitchRollLimit[] = {10,10,10};\n'
                  '\t\talignHologramToTerain = 0;\n\t\tslopeTolerance = 50.3;\n\t\titemSize[] = {2,4};\n\t\titemsCargoSize[] = {3,6};\n\t\tcarveNavmesh = 1;\n'
                  '\t\theavyItem = 1;\n\t\thiddenSelections[] = {"camo"};\n\t\thiddenSelectionsTextures[] = {"%spress\\data\\Press_co.paa"};\n'
                  '\t\thiddenSelectionsMaterials[] = {"%spress\\data\\Press.rvmat"};\n%s' % (M, M, M, dmg_mat(10000, press_mats))))
pot_mats = five(M + 'pot\\data\\pot.rvmat', M + 'pot\\data\\potDam.rvmat', M + 'pot\\data\\potDes.rvmat')
equipment.append(('VB_MR_MeltingBowl', 'BRDK_MeltingBowl', 'Cauldron', 'STR_BRDK_MeltingBowl',
                  '\t\tmodel = "%spot\\BRDK_MeltingBowl.p3d";\n\t\tweight = 700;\n\t\titemSize[] = {3,3};\n\t\titemsCargoSize[] = {3,3};\n'
                  '\t\tattachments[] = {"BRDK_METALS"};\n\t\tinventorySlot[] = {"CookingEquipment","DirectCookingA","DirectCookingB","DirectCookingC"};\n'
                  '\t\tstackedRandom = 0;\n\t\tcanBeDigged = 0;\n\t\tallowOwnedCargoManipulation = 1;\n\t\tdestroyOnEmpty = 0;\n\t\tquantityBar = 0;\n'
                  '\t\tstackedUnit = "pc.";\n\t\tvarQuantityDestroyOnMin = 0;\n\t\tvarQuantityInit = 0;\n\t\tvarQuantityMin = 0;\n\t\tvarQuantityMax = 0;\n'
                  '\t\tvolume = 3000;\n\t\tvarTemperatureMax = 200;\n\t\tvarLiquidTypeInit = 0;\n\t\tliquidContainerType = "";\n%s'
                  '\t\trepairableWithKits[] = {10};\n\t\trepairCosts[] = {25};\n\t\tclass AnimationSources\n\t\t{\n\t\t\tclass handle1Rotate\n\t\t\t{\n'
                  '\t\t\t\tsource = "user";\n\t\t\t\tanimPeriod = 0.01;\n\t\t\t\tinitPhase = 1;\n\t\t\t};\n\t\t};\n\t\tsoundImpactType = "metal";\n'
                  '\t\tclass AnimEvents\n\t\t{\n\t\t\tclass SoundWeapon\n\t\t\t{\n'
                  '\t\t\t\tclass pickUpPotLight\n\t\t\t\t{\n\t\t\t\t\tsoundSet = "pickUpPotLight_SoundSet";\n\t\t\t\t\tid = 796;\n\t\t\t\t};\n'
                  '\t\t\t\tclass pickUpPot\n\t\t\t\t{\n\t\t\t\t\tsoundSet = "pickUpPot_SoundSet";\n\t\t\t\t\tid = 797;\n\t\t\t\t};\n'
                  '\t\t\t\tclass drop\n\t\t\t\t{\n\t\t\t\t\tsoundset = "pot_drop_SoundSet";\n\t\t\t\t\tid = 898;\n\t\t\t\t};\n\t\t\t};\n\t\t};\n'
                  % (M, dmg_mat(1000000, pot_mats))))
for new, old, base, old_str, body in equipment:
    units.extend([new, old])
    key = new[6:]
    strings.append(('STR_VB_MR_%s_N' % key, 'STR_BRDK_MeltingBowln0' if old_str == 'STR_BRDK_MeltingBowl' else old_str + '0'))
    strings.append(('STR_VB_MR_%s_D' % key, old_str + '1'))
    strings_old.append(('STR_VB_MR_%s_DO' % key, old_str + '1'))

strings += [('STR_VB_MR_ACT_MINE', 'STR_NACHATDOBICHU'), ('STR_VB_MR_ACT_PAN', 'STR_PROMYTPESOK'),
            ('STR_VB_MR_ACT_PRESS', 'STR_BRDK_Spresovat'), ('STR_VB_MR_ACT_DISMANTLE', 'STR_razobratpress'),
            ('STR_VB_MR_SLOT_STONES', 'STR_BRDK_STONES'), ('STR_VB_MR_SLOT_METALS', 'STR_BRDK_METALS')]

# ---------------------------------------------------------------- config.cpp
out = []
w = out.append
w('// Сгенерировано tools/gen_config.py. Правки вносить в генератор и перезапускать.\n')
w('// Параметры предметов перенесены из BRDK_GoldRush. Старые классы BRDK_* — наследники новых VB_MR_*.\n')
w('class CfgPatches\n{\n\tclass VB_MiningResources\n\t{\n')
w('\t\tunits[] = {%s};\n' % ','.join('"%s"' % u for u in units))
w('\t\tweapons[] = {};\n\t\trequiredVersion = 0.1;\n\t\trequiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Gear_Cooking"};\n\t};\n};\n')
w('class CfgMods\n{\n\tclass VB_MiningResources\n\t{\n\t\tdir = "VB_MiningResources";\n\t\tname = "VB_MiningResources";\n'
  '\t\tauthor = "DayZavr";\n\t\ttype = "mod";\n\t\tdependencies[] = {"World"};\n\t\tclass defs\n\t\t{\n'
  '\t\t\tclass worldScriptModule\n\t\t\t{\n\t\t\t\tvalue = "";\n\t\t\t\tfiles[] = {"VB_MiningResources/scripts/4_World"};\n\t\t\t};\n\t\t};\n\t};\n};\n')
w('// Имена слотов оставлены старыми: по ним в сохранении лежат вещи в прессах и чашах игроков\n')
w('class CfgSlots\n{\n')
w('\tclass Slot_BRDK_STONES\n\t{\n\t\tname = "BRDK_STONES";\n\t\tdisplayName = "$STR_VB_MR_SLOT_STONES";\n\t\tghostIcon = "set:dayz_inventory image:stones";\n\t};\n')
w('\tclass Slot_BRDK_METALS\n\t{\n\t\tname = "BRDK_METALS";\n\t\tdisplayName = "$STR_VB_MR_SLOT_METALS";\n\t\tghostIcon = "set:dayz_inventory image:stones";\n\t};\n};\n')
w('class CfgVehicles\n{\n\tclass HouseNoDestruct;\n\tclass Inventory_Base;\n\tclass Cauldron;\n')

w('\n\t// ---------------------------------------------------------------- жилы (объекты карты)\n')
w('\tclass VB_MR_Vein_Base: HouseNoDestruct\n\t{\n\t\tscope = 0;\n\t};\n')
for r in ALL:
    w('\tclass VB_MR_%s_Vein: VB_MR_Vein_Base\n\t{\n\t\tscope = 1;\n\t\tmodel = "%sstones\\%sStone.p3d";\n\t\tphysLayer = "item_large";\n\t};\n' % (r, M, r))
w('\t// старые жилы (стоят в .dze сервера 18)\n')
for r in ALL:
    w('\tclass BRDK_%s_Ore: VB_MR_%s_Vein\n\t{\n\t\tscope = 1;\n\t};\n' % (r, r))

w('\n\t// ---------------------------------------------------------------- базы ресурсов\n')
w('\tclass VB_MR_Nugget_Base: Inventory_Base\n\t{\n\t\tscope = 0;\n\t\tvarQuantityInit = 1;\n\t\tvarQuantityMin = 0;\n\t\tvarQuantityMax = 5;\n'
  '\t\tvarQuantityDestroyOnMin = 1;\n\t\tvarTemperatureMax = 200;\n\t\tcanBeSplit = 1;\n\t\tinventorySlot[] += {"BRDK_METALS"};\n\t};\n')
w('\tclass VB_MR_StoneNugget_Base: Inventory_Base\n\t{\n\t\tscope = 0;\n\t\tvarQuantityInit = 1;\n\t\tvarQuantityMin = 0;\n\t\tvarQuantityMax = 5;\n'
  '\t\tvarQuantityDestroyOnMin = 1;\n\t\tcanBeSplit = 1;\n\t\tinventorySlot[] += {"BRDK_STONES"};\n\t};\n')
w('\tclass VB_MR_Sand_Base: Inventory_Base\n\t{\n\t\tscope = 0;\n\t\tvarQuantityInit = 1;\n\t\tvarQuantityMin = 0;\n\t\tvarQuantityMax = 10;\n'
  '\t\tvarQuantityDestroyOnMin = 1;\n\t\tcanBeSplit = 1;\n\t\tinventorySlot[] += {"BRDK_METALS"};\n\t};\n')
w('\tclass VB_MR_StoneSand_Base: Inventory_Base\n\t{\n\t\tscope = 0;\n\t\tvarQuantityInit = 1;\n\t\tvarQuantityMin = 0;\n\t\tvarQuantityMax = 10;\n'
  '\t\tvarQuantityDestroyOnMin = 1;\n\t\tcanBeSplit = 1;\n\t\tinventorySlot[] += {"BRDK_STONES"};\n\t};\n')
w('\tclass VB_MR_MetalBar_Base: Inventory_Base\n\t{\n\t\tscope = 0;\n\t};\n')
w('\tclass VB_MR_StoneBar_Base: Inventory_Base\n\t{\n\t\tscope = 0;\n\t};\n')
w('\tclass VB_MR_Gem_Base: Inventory_Base\n\t{\n\t\tscope = 0;\n\t};\n')

w('\n\t// ---------------------------------------------------------------- ресурсы: новый класс и старый\n')
for new, old, base, body, old_model in items:
    key = new[6:]
    w('\tclass %s: %s\n\t{\n\t\tscope = 2;\n\t\tdisplayName = "$STR_VB_MR_%s_N";\n\t\tdescriptionShort = "$STR_VB_MR_%s_D";\n%s%s\t};\n'
      % (new, base, key, key, model_lines(new, old_model), body))
    if old:
        # старый предмет: старая модель, старое название и «Старый образец»
        w('\tclass %s: %s\n\t{\n\t\tscope = 2;\n\t\tdisplayName = "$STR_VB_MR_%s_NO";\n\t\tdescriptionShort = "$STR_VB_MR_%s_DO";\n'
          '\t\tmodel = "%s";\n\t\thiddenSelections[] = {};\n\t\thiddenSelectionsTextures[] = {};\n\t\thiddenSelectionsMaterials[] = {};\n\t};\n'
          % (old, new, key, key, old_model))

w('\n\t// ---------------------------------------------------------------- оборудование: новый класс и старый\n')
for new, old, base, old_str, body in equipment:
    key = new[6:]
    w('\tclass %s: %s\n\t{\n\t\tscope = 2;\n\t\tdisplayName = "$STR_VB_MR_%s_N";\n\t\tdescriptionShort = "$STR_VB_MR_%s_D";\n%s\t};\n'
      % (new, base, key, key, body))
    w('\tclass %s: %s\n\t{\n\t\tscope = 2;\n\t\tdescriptionShort = "$STR_VB_MR_%s_DO";\n\t};\n' % (old, new, key))
w('};\n')
open(os.path.join(OUT, 'config.cpp'), 'w', encoding='utf-8', newline='\r\n').write(''.join(out))

# ---------------------------------------------------------------- stringtable.csv
rows = list(csv.reader(open(OLD_ST, encoding='utf-8-sig')))
header = [h for h in rows[0] if h]
old = {r[0]: r[:len(header)] for r in rows[1:] if r}
ru = header.index('russian')
for k, (tru, ten) in OLD_FIX.items():
    old[k] = [k] + [tru if i == ru else ten for i in range(1, len(header))]
res = [header]
for new, key in strings:
    if key not in old:
        raise SystemExit('нет строки ' + key)
    res.append([new] + old[key][1:])
for new, key in strings_old:
    row = [new]
    for i, val in enumerate(old[key][1:], 1):
        row.append(val.rstrip() + (OLD_SUFFIX_RU if i == ru else OLD_SUFFIX_EN))
    res.append(row)
for key, tru, ten in strings_new + [(k, v[0], v[1]) for k, v in INFO_TEXTS.items()]:
    res.append([key] + [tru if i == ru else ten for i in range(1, len(header))])
with open(os.path.join(OUT, 'stringtable.csv'), 'w', encoding='utf-8', newline='') as f:
    csv.writer(f, quoting=csv.QUOTE_ALL, lineterminator='\r\n').writerows(res)

print('ресурсов: %d (из них со старым классом %d), оборудования: %d, строк: %d, новых моделей: %d'
      % (len(items), sum(1 for i in items if i[1]), len(equipment), len(res) - 1, sum(1 for i in items if i[0] in mapping)))
