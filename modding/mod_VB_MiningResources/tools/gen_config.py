# Генератор VB_MiningResources/config.cpp и stringtable.csv.
# Все параметры предметов перенесены 1:1 из старого BRDK_GoldRush (старый мод/_разбор/client_config.cpp).
# Старые классы BRDK_* объявлены наследниками новых VB_MR_*: тот же предмет, в описании «Старый образец».
# python gen_config.py
import os, csv, sys
sys.stdout.reconfigure(encoding='utf-8')

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'VB_MiningResources')
OLD_ST = os.path.join(ROOT, 'старый мод', 'BRDK_GoldRush_CLIENT', 'stringtable.csv')
M = 'VB_MiningResources\\m\\'

METALS = ['Gold', 'Silver', 'Bronze', 'Iron', 'Titanium']
STONES = ['Coal', 'Rubin', 'Nephrite', 'Sulfur']
ALL = METALS + STONES

OLD_SUFFIX_RU = ' Старый образец.'
OLD_SUFFIX_EN = ' Old stock.'

HP_ITEM = '\t\tclass DamageSystem\n\t\t{\n\t\t\tclass GlobalHealth\n\t\t\t{\n\t\t\t\tclass Health\n\t\t\t\t{\n\t\t\t\t\thitpoints = %d;\n\t\t\t\t\thealthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{%s,{}}};\n\t\t\t\t};\n\t\t\t};\n\t\t};\n'


def dmg_mat(hp, mats):
    lv = ','.join('{%s,{"%s"}}' % (lvl, mat) for lvl, mat in mats)
    return ('\t\tclass DamageSystem\n\t\t{\n\t\t\tclass GlobalHealth\n\t\t\t{\n\t\t\t\tclass Health\n\t\t\t\t{\n'
            '\t\t\t\t\thitpoints = %d;\n\t\t\t\t\thealthLevels[] = {%s};\n\t\t\t\t};\n\t\t\t};\n\t\t};\n') % (hp, lv)


def five(ok, dam, des):
    return [('1.0', ok), ('0.7', ok), ('0.5', dam), ('0.3', dam), ('0.1', des)]


# (новый класс, старый класс, ключ старой строки без 0/1, тело класса)
items = []
units = []
strings = []          # (новый ключ, старый ключ) — копируются все языки
strings_old = []      # (новый ключ, старый ключ) — описание + «Старый образец»


def add_item(new, old, base, old_str, body):
    items.append((new, old, base, body))
    units.extend([new, old])
    strings.append(('STR_VB_MR_%s_N' % new[6:], old_str + '0'))
    strings.append(('STR_VB_MR_%s_D' % new[6:], old_str + '1'))
    strings_old.append(('STR_VB_MR_%s_DO' % new[6:], old_str + '1'))


for r in ALL:
    stone = r in STONES
    folder = 'Kamens' if stone else 'metall'
    # самородок / кусок
    add_item('VB_MR_' + r, 'BRDK_' + r, 'VB_MR_StoneNugget_Base' if stone else 'VB_MR_Nugget_Base', 'STR_BRDK_' + r,
             '\t\tmodel = "%s%s\\Smal%s.p3d";\n\t\tanimClass = "NoFireClass";\n\t\tweight = 500;\n\t\titemSize[] = {2,2};\n\t\trotationFlags = 17;\n%s'
             % (M, folder, r, HP_ITEM % (9900, '0.1')))
    # песок / крупицы
    add_item('VB_MR_%s_Sand' % r, 'BRDK_%s_Sand' % r, 'VB_MR_StoneSand_Base' if stone else 'VB_MR_Sand_Base', 'STR_BRDK_%s_Sand' % r,
             '\t\tmodel = "%s%s\\Sand%s.p3d";\n\t\tanimClass = "Compass";\n\t\titemSize[] = {1,1};\n\t\tweight = 190;\n%s'
             % (M, folder, r, HP_ITEM % (9900, '0.1')))
    # слиток / брикет
    if not stone:
        body = ('\t\tmodel = "%smetall\\%sBigStone.p3d";\n\t\tanimClass = "NoFireClass";\n\t\tweight = 5500;\n\t\titemSize[] = {3,3};\n\t\tisMeleeWeapon = 1;\n%s'
                % (M, r, HP_ITEM % (9900, '0.1')))
        base = 'VB_MR_MetalBar_Base'
    elif r == 'Coal':
        body = ('\t\tmodel = "%sKamens\\BigCoalStone.p3d";\n\t\tinventorySlot[] += {"Firewood"};\n\t\tcanBeSplit = 1;\n\t\tvarQuantityInit = 1;\n'
                '\t\tvarQuantityMin = 0;\n\t\tvarQuantityMax = 6;\n\t\tvarQuantityDestroyOnMin = 1;\n\t\tvarStackMax = 1;\n\t\tweight = 300;\n\t\titemSize[] = {2,2};\n%s'
                % (M, HP_ITEM % (50, '0.0')))
        base = 'VB_MR_StoneBar_Base'
    else:
        body = ('\t\tmodel = "%sKamens\\Big%sStone.p3d";\n\t\tweight = 0;\n\t\titemSize[] = {2,2};\n%s' % (M, r, HP_ITEM % (50, '0.0')))
        base = 'VB_MR_StoneBar_Base'
    add_item('VB_MR_%s_Bar' % r, 'BRDK_Big' + r, base, 'STR_BRDK_Big' + r, body)

# оборудование
kit_mats = five(M + 'KIT\\data\\box_kit.rvmat', M + 'KIT\\data\\box_kitDam.rvmat', M + 'KIT\\data\\box_kitDes.rvmat')
add_item('VB_MR_Press_Kit', 'BRDK_Press_kit', 'Inventory_Base', 'STR_Press_kit',
         '\t\tmodel = "%sKIT\\BRDK_Press_Kit.p3d";\n\t\tprojectionTypename = "VB_MR_Press";\n\t\tallowOwnedCargoManipulation = 1;\n'
         '\t\thologramMaterial = "fishnettrap";\n\t\thologramMaterialPath = "dz\\gear\\traps\\data";\n\t\toverrideDrawArea = 8.0;\n'
         '\t\tforcarBubble = "true";\n\t\tdestroyOnEmpty = 0;\n\t\tcarveNavmesh = 1;\n\t\theavyItem = 1;\n\t\tweight = 1000;\n\t\titemBehaviour = 1;\n'
         '\t\tphysLayer = "item_large";\n\t\titemSize[] = {5,3};\n\t\tlootTag[] = {"Work"};\n%s' % (M, dmg_mat(500, kit_mats)))
press_mats = five(M + 'press\\data\\Press.rvmat', M + 'press\\data\\PressDam.rvmat', M + 'press\\data\\PressDes.rvmat')
add_item('VB_MR_Press', 'BRDK_Stone_Press', 'Inventory_Base', 'STR_BRDK_Stone_Press',
         '\t\tmodel = "%spress\\BRDK_Press.p3d";\n\t\tphysLayer = "item_large";\n\t\tattachments[] = {"BRDK_STONES"};\n\t\tforcarBubble = "true";\n'
         '\t\thandheld = "false";\n\t\tweight = 1000000;\n\t\tuseEntityHierarchy = "true";\n\t\tyawPitchRollLimit[] = {10,10,10};\n'
         '\t\talignHologramToTerain = 0;\n\t\tslopeTolerance = 50.3;\n\t\titemSize[] = {2,4};\n\t\titemsCargoSize[] = {3,6};\n\t\tcarveNavmesh = 1;\n'
         '\t\theavyItem = 1;\n\t\thiddenSelections[] = {"camo"};\n\t\thiddenSelectionsTextures[] = {"%spress\\data\\Press_co.paa"};\n'
         '\t\thiddenSelectionsMaterials[] = {"%spress\\data\\Press.rvmat"};\n%s' % (M, M, M, dmg_mat(10000, press_mats)))
pot_mats = five(M + 'pot\\data\\pot.rvmat', M + 'pot\\data\\potDam.rvmat', M + 'pot\\data\\potDes.rvmat')
add_item('VB_MR_MeltingBowl', 'BRDK_MeltingBowl', 'Cauldron', 'STR_BRDK_MeltingBowl',
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
         % (M, dmg_mat(1000000, pot_mats)))

# строки действий и слотов
strings += [('STR_VB_MR_ACT_MINE', 'STR_NACHATDOBICHU'), ('STR_VB_MR_ACT_PAN', 'STR_PROMYTPESOK'),
            ('STR_VB_MR_ACT_PRESS', 'STR_BRDK_Spresovat'), ('STR_VB_MR_ACT_DISMANTLE', 'STR_razobratpress'),
            ('STR_VB_MR_SLOT_STONES', 'STR_BRDK_STONES'), ('STR_VB_MR_SLOT_METALS', 'STR_BRDK_METALS')]
# у чаши ключ названия в старом моде с опечаткой
strings = [(n, 'STR_BRDK_MeltingBowln0' if o == 'STR_BRDK_MeltingBowl0' else o) for n, o in strings]

# ---------------------------------------------------------------- config.cpp
out = []
w = out.append
w('// Сгенерировано tools/gen_config.py. Правки вносить в генератор и перезапускать.\n')
w('// Параметры предметов перенесены 1:1 из BRDK_GoldRush. Старые классы BRDK_* — наследники новых VB_MR_*.\n')
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

w('\n\t// ---------------------------------------------------------------- предметы: новый класс и старый\n')
for new, old, base, body in items:
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
res = [header]
for new, key in strings:
    if key not in old:
        raise SystemExit('нет строки ' + key)
    res.append([new] + old[key][1:])
for new, key in strings_old:
    row = [new]
    for i, val in enumerate(old[key][1:], 1):
        val = val.rstrip()
        row.append(val + (OLD_SUFFIX_RU if i == ru else OLD_SUFFIX_EN))
    res.append(row)
with open(os.path.join(OUT, 'stringtable.csv'), 'w', encoding='utf-8', newline='') as f:
    csv.writer(f, quoting=csv.QUOTE_ALL, lineterminator='\r\n').writerows(res)

print('предметов: %d новых + %d старых, жил: %d + %d, строк: %d' % (len(items), len(items), len(ALL), len(ALL), len(res) - 1))
