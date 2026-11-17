# settings.json для VB_MiningResources из GoldRush.json сервера 18: те же значения, новые классы.
# python make_settings.py <GoldRush.json> <settings.json>
import json, re, sys
src, dst = sys.argv[1], sys.argv[2]
d = json.load(open(src, encoding='utf-8'))

def item(name):
    m = re.fullmatch(r'BRDK_Big(\w+)', name)
    if m: return 'VB_MR_%s_Bar' % m.group(1)
    m = re.fullmatch(r'BRDK_(\w+)_Ore', name)
    if m: return 'VB_MR_%s_Vein' % m.group(1)
    m = re.fullmatch(r'BRDK_(\w+)', name)
    if m: return 'VB_MR_' + m.group(1)
    return name

for v in d['VeineDataDataList']:
    v['VeinName'] = item(v['VeinName']); v['GoodOre'] = item(v['GoodOre'])
for s in d['SandDataList']:
    s['SandName'] = item(s['SandName']); s['OreName'] = item(s['OreName'])
for o in d['OreDataList']:
    o['OreName'] = item(o['OreName']); o['IngotName'] = item(o['IngotName'])
txt = json.dumps(d, ensure_ascii=False, indent=4)
assert 'BRDK' not in txt
open(dst, 'w', encoding='utf-8', newline='\r\n').write(txt + '\n')
print('ok', len(d['VeineDataDataList']), len(d['SandDataList']), len(d['OreDataList']))
