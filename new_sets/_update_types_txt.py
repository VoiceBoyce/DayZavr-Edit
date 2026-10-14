# -*- coding: utf-8 -*-
"""Parse all config.cpp under new_sets, extract scope=2 types + characteristics, write TXT."""
import csv, re
from pathlib import Path
from collections import defaultdict

ROOT = Path(r"D:\DayZavr\DayZavr-Edit\new_sets")
OUT = ROOT / "types_characteristics.txt"

# ---------- stringtables ----------
per_mod_str = defaultdict(dict)   # mod -> {key: (ru, en)}
global_str = {}
for st in sorted(ROOT.rglob("stringtable.csv")):
    try:
        mod = st.relative_to(ROOT).parts[0]
    except ValueError:
        mod = "?"
    try:
        with open(st, encoding="utf-8-sig", newline="") as f:
            reader = csv.reader(f)
            rows = list(reader)
    except Exception as e:
        print(f"WARN: cannot read {st}: {e}")
        continue
    if not rows:
        continue
    header = [h.strip().lower() for h in rows[0]]
    def col(*names):
        for n in names:
            if n in header:
                return header.index(n)
        return None
    i_ru = col("russian")
    i_en = col("english")
    i_or = col("original")
    for r in rows[1:]:
        if not r:
            continue
        key = r[0].strip().strip("$")
        if not key:
            continue
        ru = r[i_ru].strip() if i_ru is not None and i_ru < len(r) else ""
        en = r[i_en].strip() if i_en is not None and i_en < len(r) else ""
        if not en and i_or is not None and i_or < len(r):
            en = r[i_or].strip()
        per_mod_str[mod][key] = (ru, en)
        if key not in global_str:
            global_str[key] = (ru, en)

def resolve(raw, mod):
    if not raw:
        return ""
    if not raw.startswith("$"):
        return raw
    key = raw[1:]
    for d in (per_mod_str.get(mod, {}), global_str):
        if key in d:
            ru, en = d[key]
            return ru or en or raw
    return raw

# ---------- config parsing ----------
def find_block(text, start_idx):
    """Given index of '{', return (inner_text, end_index_past_closing)."""
    depth = 0
    for i in range(start_idx, len(text)):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return text[start_idx + 1:i], i + 1
    return None, len(text)

def depth_at(text, idx):
    d = 0
    for ch in text[:idx]:
        if ch == "{":
            d += 1
        elif ch == "}":
            d -= 1
    return d

def top_classes(body):
    """Top-level (depth 0) classes inside a {..} body: list of (name, parent, inner)."""
    out = []
    for m in re.finditer(r'class\s+(\w+)\s*(?::\s*(\w+))?\s*\{', body):
        if depth_at(body, m.start()) != 0:
            continue
        brace = m.end() - 1
        inner, _end = find_block(body, brace)
        out.append((m.group(1), m.group(2), inner if inner is not None else ""))
    return out

def q(body, key):
    m = re.search(r'%s\s*=\s*"([^"]*)"\s*;' % re.escape(key), body)
    return m.group(1) if m else None

def num(body, key):
    m = re.search(r'%s\s*=\s*(-?[0-9]+(?:\.[0-9]+)?)' % re.escape(key), body)
    return m.group(1) if m else None

def arr(body, key):
    m = re.search(r'%s\s*\[\]\s*=\s*\{([^}]*)\}' % re.escape(key), body, re.S)
    if not m:
        return None
    items = [x.strip().strip('"').strip("'") for x in m.group(1).split(",")]
    items = [x for x in items if x]
    return items

def armor_val(body, atype, dtype):
    m = re.search(
        r'class\s+%s\s*\{[^}]*?class\s+%s\s*\{[^}]*?damage\s*=\s*(-?[0-9]+(?:\.[0-9]+)?)' % (atype, dtype),
        body)
    return m.group(1) if m else None

results = defaultdict(list)  # mod -> list of dict
n_files = 0
for cfg in sorted(ROOT.rglob("config.cpp")):
    try:
        mod = cfg.relative_to(ROOT).parts[0]
    except ValueError:
        continue
    try:
        text = cfg.read_text(encoding="utf-8", errors="replace")
    except Exception as e:
        print(f"WARN: cannot read {cfg}: {e}")
        continue
    m = re.search(r'class\s+[Cc]fg[Vv]ehicles\s*\{', text)
    if not m:
        continue
    inner, _ = find_block(text, m.end() - 1)
    if inner is None:
        continue
    n_files += 1
    classes = top_classes(inner)
    by_name = {n: (p, b) for n, p, b in classes}
    for name, parent, cbody in classes:
        scope = num(cbody, "scope")
        if scope != "2":
            continue
        # merge parent chain (same file), child wins -> search child first
        merged = cbody
        seen = {name}
        p = parent
        while p and p in by_name and p not in seen:
            seen.add(p)
            pp, pb = by_name[p]
            merged += "\n" + pb
            p = pp
        disp = q(merged, "displayName")
        desc = q(merged, "descriptionShort")
        entry = {
            "type": name,
            "parent": parent or "—",
            "display_raw": disp or "—",
            "display": resolve(disp, mod) if disp else "—",
            "desc": resolve(desc, mod) if desc else "—",
            "slot": arr(merged, "inventorySlot"),
            "iteminfo": arr(merged, "itemInfo"),
            "model": q(merged, "model") or "—",
            "weight": num(merged, "weight"),
            "itemsize": arr(merged, "itemSize"),
            "cargo": arr(merged, "itemsCargoSize"),
            "quickbar": num(merged, "quickBarBonus"),
            "rag": num(merged, "ragQuantity"),
            "wetmin": num(merged, "varWetMin"),
            "wetmax": num(merged, "varWetMax"),
            "absorb": num(merged, "absorbency"),
            "heat": num(merged, "heatIsolation"),
            "kits": arr(merged, "repairableWithKits"),
            "repair": arr(merged, "repairCosts"),
            "hp": num(merged, "hitpoints"),
            "attach": arr(merged, "attachments"),
            "tex": arr(cbody, "hiddenSelectionsTextures"),  # только свои текстуры варианта
            "file": str(cfg.relative_to(ROOT)),
        }
        armor = {}
        for at in ("Projectile", "Melee", "Infected", "FragGrenade"):
            armor[at] = {dt: armor_val(merged, at, dt) for dt in ("Health", "Blood", "Shock")}
        entry["armor"] = armor
        results[mod].append(entry)

# ---------- write TXT ----------
import textwrap

def fmt_list(v):
    if v is None:
        return "—"
    if isinstance(v, list):
        return "{" + ", ".join(v) + "}" if v else "{}"
    return v

def sig_of(e):
    """Сигнатура игровых характеристик (без имени тайпса, текстур и файла)."""
    def norm(v):
        if isinstance(v, list):
            return tuple(v)
        if isinstance(v, dict):
            return tuple(sorted((k, tuple(sorted(d.items()))) for k, d in v.items()))
        return v
    keys = ("display_raw", "display", "desc", "slot", "iteminfo", "model",
            "weight", "itemsize", "cargo", "quickbar", "rag", "wetmin",
            "wetmax", "absorb", "heat", "kits", "repair", "hp", "attach", "armor")
    return tuple(norm(e[k]) for k in keys)

def tex_short(tex):
    if not tex:
        return "—"
    return ", ".join(t.replace("\\", "/").split("/")[-1] for t in tex)

def body_lines(e):
    """Строки характеристик (общие для записи)."""
    B = []
    B.append(f"    Название: {e['display']}" + (f"  [{e['display_raw']}]" if e['display_raw'] != "—" and e['display'] != e['display_raw'] else ""))
    if e["desc"] and e["desc"] != "—":
        B.append(f"    Описание: {e['desc']}")
    B.append(f"    Родитель: {e['parent']}")
    B.append(f"    Слот: {fmt_list(e['slot'])} | Тип: {fmt_list(e['iteminfo'])}")
    B.append(f"    Модель: {e['model']}")
    B.append(f"    Вес: {e['weight'] or '—'} | Размер вещи: {fmt_list(e['itemsize'])} | Размер груза: {fmt_list(e['cargo'])}")
    B.append(f"    Быстрый слот: {e['quickbar'] or '—'} | Тряпки: {e['rag'] or '—'}")
    B.append(f"    Намокание min/max: {e['wetmin'] or '—'} / {e['wetmax'] or '—'} | Впитываемость: {e['absorb'] or '—'} | Теплоизоляция: {e['heat'] or '—'}")
    B.append(f"    Ремкомплекты: {fmt_list(e['kits'])} | Цена ремонта: {fmt_list(e['repair'])}")
    B.append(f"    Прочность (hitpoints): {e['hp'] or '—'}")
    a = e["armor"]
    has_armor = any(v for at in a.values() for v in at.values())
    if has_armor:
        for at in ("Projectile", "Melee", "Infected", "FragGrenade"):
            vals = a[at]
            B.append(f"    Броня {at}: Health={vals['Health'] or '—'}, Blood={vals['Blood'] or '—'}, Shock={vals['Shock'] or '—'}")
    else:
        B.append("    Броня: — (нет GlobalArmor)")
    B.append(f"    Аттачменты: {fmt_list(e['attach'])}")
    return B

# Группировка: один родитель + один файл + одинаковые характеристики = одна запись
grouped = {}  # mod -> list of groups; group = {"types": [...], "e": entry}
for mod, items in results.items():
    buckets = defaultdict(list)
    for e in items:
        buckets[(e["parent"], e["file"], sig_of(e))].append(e)
    groups = []
    for (parent, _f, _s), members in buckets.items():
        members = sorted(members, key=lambda e: e["type"].lower())
        groups.append({"types": [m["type"] for m in members], "e": members[0],
                       "tex": [(m["type"], m["tex"]) for m in members]})
    groups.sort(key=lambda g: g["types"][0].lower())
    grouped[mod] = groups

total_types = sum(len(v) for v in results.values())
total_groups = sum(len(v) for v in grouped.values())

L = []
L.append("ТАЙПСЫ И ХАРАКТЕРИСТИКИ МОДОВЫХ ВЕЩЕЙ (DayZ, new_sets)")
L.append(f"Модов с вещами: {len(grouped)} | Всего тайпсов: {total_types} | Записей: {total_groups} (варианты одной вещи с одинаковыми характеристиками объединены)")
L.append("Сортировка: по названию мода, внутри — по classname.")
L.append("Прочерк «—» = параметр отсутствует в конфиге (наследуется из ванилы).")
L.append("Броня: коэффициент урона (меньше = лучше защита; 1 = без защиты).")
L.append("")
for mod in sorted(grouped.keys(), key=str.lower):
    groups = grouped[mod]
    n_types = sum(len(g["types"]) for g in groups)
    L.append("=" * 78)
    L.append(f"МОД: {mod}  (тайпсов: {n_types}, записей: {len(groups)})")
    L.append("=" * 78)
    for g in groups:
        e = g["e"]
        if len(g["types"]) == 1:
            L.append(f"  ТАЙПС: {g['types'][0]}")
            L.extend(body_lines(e))
            if e["tex"]:
                L.append(f"    Текстуры: {tex_short(e['tex'])}")
        else:
            L.append(f"  ТАЙПСЫ ({len(g['types'])} вариантов, характеристики одинаковые):")
            wrapped = textwrap.wrap(", ".join(g["types"]), width=72,
                                    initial_indent="    ", subsequent_indent="    ")
            L.extend(wrapped)
            L.extend(body_lines(e))
            L.append("    Текстуры по вариантам:")
            for tname, tex in g["tex"]:
                L.append(f"      {tname}: {tex_short(tex)}")
        L.append(f"    Файл: {e['file']}")
        L.append("  " + "-" * 74)
    L.append("")

OUT.write_text("\n".join(L), encoding="utf-8-sig")
print(f"config.cpp с CfgVehicles: {n_files}")
print(f"модов: {len(grouped)}, тайпсов: {total_types}, записей: {total_groups}")
for mod in sorted(grouped.keys(), key=str.lower):
    n_types = sum(len(g["types"]) for g in grouped[mod])
    print(f"  {mod}: {n_types} тайпсов / {len(grouped[mod])} записей")
print(f"WROTE: {OUT} ({OUT.stat().st_size} bytes)")
