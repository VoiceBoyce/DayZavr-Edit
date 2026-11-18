# settings.json (версия 2) для VB_MiningResources из GoldRush.json сервера 18: те же значения, новые классы,
# новые поля ТЗ v0.3 со стартовыми значениями.
# python -I make_settings.py <GoldRush.json> <settings.json>
import json, re, sys

src, dst = sys.argv[1], sys.argv[2]
d = json.load(open(src, encoding='utf-8'))


def item(name):
    m = re.fullmatch(r'BRDK_Big(\w+)', name)
    if m:
        return 'VB_MR_%s_Bar' % m.group(1)
    m = re.fullmatch(r'BRDK_(\w+)_Ore', name)
    if m:
        return 'VB_MR_%s_Vein' % m.group(1)
    m = re.fullmatch(r'BRDK_(\w+)', name)
    if m:
        return 'VB_MR_' + m.group(1)
    return name


RARE = {}   # редкая находка в жиле: огранённые камни убраны 2026-11-18, по умолчанию выключено
veins = []
for v in d['VeineDataDataList']:
    v['VeinName'] = item(v['VeinName'])
    v['GoodOre'] = item(v['GoodOre'])
    v['RareItem'] = RARE.get(v['VeinName'], '')
    v['RareChance'] = 2.0 if v['VeinName'] in RARE else 0.0
    veins.append(v)
sands = []
for s in d['SandDataList']:
    s['SandName'] = item(s['SandName'])
    s['OreName'] = item(s['OreName'])
    s['Portion'] = 0
    sands.append(s)
ores = []
for o in d['OreDataList']:
    o['OreName'] = item(o['OreName'])
    o['IngotName'] = item(o['IngotName'])
    o['Portion'] = 0
    ores.append(o)

tools = [t for t in d['MineTools'] if t != 'MSFZ_Magic_Kirka']
out = {
    'ConfigVersion': 3,
    'GoldInRiverSearchTime': d['GoldInRiverSearchTime'],
    'MinCountInRiver': d['MinCountInRiver'],
    'MaxCountInRiver': d['MaxCountInRiver'],
    'BadRiverBounty': d['BadRiverBounty'],
    'MeltUpdatePeriod': d['MeltUpdatePeriod'],
    'MeltTempTreshold': d['MeltTempTreshold'],
    'PressUpdatePeriod': 5.0,
    'MineTools': tools,
    'MineToolsData': [{
        'Tool': 'MSFZ_Magic_Kirka', 'MineTimeMul': 0.7, 'ToolDamageMul': 0.5, 'OreChanceAdd': 10.0,
        'ExtraRollsMin': 0, 'ExtraRollsMax': 0, 'RareChanceAdd': 0.0, 'BonusItem': '', 'BonusChance': 0.0, 'Veins': []}, {
        # Велесова кирка (ТЗ_Велесова_кирка.md): «Дар Велеса» — янтарь 5 % за жилу
        'Tool': 'VB_MR_Pickaxe_Veles', 'MineTimeMul': 0.6, 'ToolDamageMul': 0.35, 'OreChanceAdd': 15.0,
        'ExtraRollsMin': 0, 'ExtraRollsMax': 1, 'RareChanceAdd': 0.0, 'BonusItem': 'VB_AR_Amber', 'BonusChance': 5.0,
        'Veins': []}],
    'VeineDataDataList': veins,
    'SandDataList': sands,
    'OreDataList': ores,
    'PanZones': [],
    'AmberItem': 'VB_AR_Amber',
    'AmberSourceClass': 'VB_AR_Heart',
    'AmberRadius': 60.0,
    'AmberChance': 3.0,
}
txt = json.dumps(out, ensure_ascii=False, indent=4)
assert 'BRDK' not in txt
open(dst, 'w', encoding='utf-8', newline='\r\n').write(txt + '\n')
print('ok', len(veins), len(sands), len(ores))
