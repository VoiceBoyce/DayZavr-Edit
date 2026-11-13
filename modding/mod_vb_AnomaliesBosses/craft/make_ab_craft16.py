# -*- coding: utf-8 -*-
"""Крафт артефактов сервера 16 из материалов аномалий (ТЗ 2.4, решение пользователя 2026-11-13).

  python make_ab_craft16.py

Логика: артефакт — плод аномалий своей стихии.
  тир I   — материал лёгкой аномалии своей стихии;
  тир II  — материалы лёгкой и средней аномалии + артефакт I той же стихии;
  тир III — три вида, включая тяжёлую аномалию + артефакт II той же стихии;
  тир IV  — материалы тяжёлых аномалий + артефакт III своей стихии + артефакт III соседней + трофей босса той же палитры.
Цепочки внутри стихии и короткие (дерево, не линия): номерные версии растут из предыдущей («Пламя II» из «Пламени»),
остальные берут артефакт тира ниже своей стихии по очереди. Изнанка — только в органике и пси (RIFT).
Детали EFT и лабораторные инструменты берутся из текущего рецепта 16 как есть.

Номера рецептов, названия и результаты те же (800–857, категория «Артефакты», верстак), меняется состав и описание.
Пишет рядом:
  ab_artifacts_16.json — 58 новых рецептов;
  CraftConfig_16.json  — полный файл 16-го с заменой (готов к выкладке);
  artifacts_16.md      — таблица на проверку: артефакт → стихия → тир → состав.
Живые файлы сервера не трогает. Категории не меняются (номера те же).
"""
import csv
import io
import json
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
MOD = os.path.join(HERE, "..", "VB_AnomaliesBosses")
SRV16 = r"D:\DayZavr\DayZavr-Servers\DayZServer16\Instance_1\DRL_CraftMenu"

# Стихия -> материалы аномалий от лёгкой к тяжёлой (урон — ТЗ 4.4)
MATS = {
    "Storm": ["EMP", "Groza", "MlechniPut", "Electra"],
    "Fire": ["Par", "Zharka", "FireIskra", "FireShtorm"],
    "Ice": ["Snow", "Steklo", "Zastoy"],
    "Gravity": ["Tramplin", "Lomka", "Voronka"],
    "Acid": ["Holera", "Studen", "Roy"],
    "Psi": ["Lince", "LinceAgr", "Flash"],
    "Rad": ["DeadZone", "Atom"],
}
RU = {"Storm": "Гроза", "Fire": "Огонь", "Ice": "Лёд", "Gravity": "Гравитация", "Acid": "Кислота", "Psi": "Пси",
      "Rad": "Радиация"}
# Соседняя стихия (для тира IV): кольцо Гроза → Огонь → Радиация → Кислота → Лёд → Гравитация → Пси → Гроза
RING = ["Storm", "Fire", "Rad", "Acid", "Ice", "Gravity", "Psi"]
TROPHY = {
    "Storm": ["VB_AB_Trophy_Ent_Storm", "VB_AB_Trophy_MozgoTras_Storm"],
    "Fire": ["VB_AB_Trophy_MozgoTrasRed_Fire", "VB_AB_Trophy_MutaWolf_Fire"],
    "Ice": ["VB_AB_Trophy_MutaWolf_Ice", "VB_AB_Trophy_Shum_Ice"],
    "Gravity": ["VB_AB_Trophy_MozgoTrasRed_Gravity", "VB_AB_Trophy_Shum_Gravity"],
    "Acid": ["VB_AB_Trophy_Ent_Acid", "VB_AB_Trophy_Ricar_Acid"],
    "Psi": ["VB_AB_Trophy_MozgoTras_Psi", "VB_AB_Trophy_Tenevnik_Psi"],
    "Rad": ["VB_AB_Trophy_Tenevnik_Rad", "VB_AB_Trophy_Ricar_Rad"],
}

# Артефакт (без OOTS_ART_) -> стихия, тир. Стихии из THEME (ab_bosses.py), остальные — по смыслу названия;
# тиры — из тиров спавнера OOTS 16 (ART_TIER), выправлены так, чтобы в каждой стихии была лестница I→IV
# и номерные версии шли по порядку. Проверяет пользователь по artifacts_16.md.
ART = {
    # Гроза
    "Plasma": ("Storm", 1), "izolator": ("Storm", 1), "Battery": ("Storm", 2), "vspishka": ("Storm", 2),
    "Battery3": ("Storm", 3), "Sparkler": ("Storm", 3), "NightStar": ("Storm", 4),
    # Огонь
    "DevelHert": ("Fire", 1), "Plamya": ("Fire", 1), "Plamya2": ("Fire", 2), "Burn": ("Fire", 2),
    "Plamya3": ("Fire", 3), "FireBall": ("Fire", 4),
    # Лёд
    "snejinka": ("Ice", 1), "Crustal": ("Ice", 1), "Crustal2": ("Ice", 2), "CrustalThorn": ("Ice", 2),
    "Crustal3": ("Ice", 3), "MoonLight": ("Ice", 3), "Oasis": ("Ice", 4),
    # Кислота (химия и органика)
    "Klyaksa": ("Acid", 1), "Drop": ("Acid", 1), "SeaUrchin": ("Acid", 1), "Drop2": ("Acid", 2), "myaso": ("Acid", 2),
    "Thorn": ("Acid", 2), "Drop3": ("Acid", 3), "puzir": ("Acid", 3), "Mohovik": ("Acid", 3),
    "Flower": ("Acid", 4), "ShineOfTheForest": ("Acid", 4),
    # Гравитация
    "kolobok": ("Gravity", 1), "kameniy_cvetok": ("Gravity", 1), "shturval": ("Gravity", 1),
    "kameniy_cvetok2": ("Gravity", 2), "Medusa": ("Gravity", 2), "kameniy_cvetok3": ("Gravity", 3),
    "ribka": ("Gravity", 3), "Gravi": ("Gravity", 4), "vivert": ("Gravity", 4), "Compas": ("Gravity", 4),
    "Spring": ("Gravity", 4),
    # Пси
    "svet": ("Psi", 1), "Glaz": ("Psi", 2), "Angel": ("Psi", 2), "Glaz2": ("Psi", 3), "Glaz3": ("Psi", 3),
    "DemonEye": ("Psi", 3), "Braen": ("Psi", 4), "Light": ("Psi", 4), "Monolit": ("Psi", 4),
    # Радиация
    "BloodStone_New": ("Rad", 1), "AngelFool": ("Rad", 1), "busy2": ("Rad", 2), "DarkSoul": ("Rad", 3),
    "Mikrob": ("Rad", 3), "krov_kamnya": ("Rad", 4), "NanoCell": ("Rad", 4),
}
# Номерные версии растут из предыдущей
FAMILY = {"Plamya2": "Plamya", "Plamya3": "Plamya2", "Crustal2": "Crustal", "Crustal3": "Crustal2",
          "Drop2": "Drop", "Drop3": "Drop2", "Glaz2": "Glaz", "Glaz3": "Glaz2",
          "kameniy_cvetok2": "kameniy_cvetok", "kameniy_cvetok3": "kameniy_cvetok2", "Battery3": "Battery"}
# Изнанка — только органика и пси (решение пользователя)
RIFT = {"Braen", "Glaz", "Glaz2", "Glaz3", "DemonEye", "myaso", "Mikrob", "DevelHert", "Flower"}
ROOT, VEIN, SPORES, HEART = "VB_RiftRootPiece", "VB_RiftVein", "VB_RiftSpores", "VB_RiftHeartChunk"
ROMAN = {1: "I", 2: "II", 3: "III", 4: "IV"}


def comp(names, amount=1, recraft=0.5):
    names = [names] if isinstance(names, str) else list(names)
    return {"Classname": names[0], "Classnames": names, "EnableVariationSelection": 1, "MinHealthFrac": 3,
            "Amount": amount, "QuantityFrac": 0.0, "Delete": 1, "ApplyDamageFrac": 0.0,
            "RecraftDropChance": recraft, "RecraftAmount": amount, "RecraftQuantityFrac": 0.0,
            "UseInheritance": 0, "AttachmentsEx": 0, "RecraftNeedAttachments": 0}


def mat(key, amount):
    return comp("VB_AB_Mat_" + key, amount)


def materials(elem, tier):
    m = MATS[elem]
    heavy = m[-1]
    if tier == 1:
        return [(m[0], 4)]
    if tier == 2:
        return [(m[0], 3), (m[1], 3)]
    if tier == 3:
        if len(m) == 2:
            return [(m[0], 3), (heavy, 4)]
        return [(m[0], 2), (m[1], 3), (heavy, 2)]
    if len(m) == 2:                                   # Радиация: всего два материала
        return [(m[0], 3), (heavy, 4)]
    out = [(m[1], 3), (heavy, 4)]
    if len(m) >= 4:
        out.insert(1, (m[-2], 2))
    return out


def rift(tier):
    return {1: [(ROOT, 2), (VEIN, 1)], 2: [(VEIN, 2), (SPORES, 1)], 3: [(VEIN, 2), (SPORES, 2)],
            4: [(HEART, 1), (SPORES, 2), (VEIN, 2)]}[tier]


def pick_inputs():
    """Артефакты-ингредиенты: {артефакт: [артефакты]}; ротация, чтобы одни и те же не шли везде."""
    by = {}
    for a, (e, t) in ART.items():
        by.setdefault((e, t), []).append(a)
    for k in by:
        by[k].sort()
    turn = {}

    def take(e, t, avoid=()):
        for tt in range(t, 0, -1):
            cand = [x for x in by.get((e, tt), []) if x not in avoid]
            if cand:
                i = turn.get((e, tt), 0)
                turn[(e, tt)] = i + 1
                return cand[i % len(cand)]
        return None

    inputs = {}
    for a in sorted(ART, key=lambda x: (ART[x][1], x)):
        e, t = ART[a]
        need = []
        if t > 1:
            prev = FAMILY.get(a) or take(e, min(t - 1, 3), avoid=(a,))
            if prev:
                need.append(prev)
        if t == 4:
            ne = RING[(RING.index(e) + 1) % len(RING)]
            other = take(ne, 3)
            if other:
                need.append(other)
        inputs[a] = need
    return inputs


def names_ru():
    rows = list(csv.reader(io.open(os.path.join(MOD, "stringtable.csv"), encoding="utf-8")))
    ri = [h.lower() for h in rows[0]].index("russian")
    return {r[0]: r[ri] for r in rows[1:] if r}


def main():
    cfg = json.load(io.open(os.path.join(SRV16, "CraftConfig.json"), encoding="utf-8-sig"))
    old = {r["RecipeId"]: r for r in cfg if 800 <= r["RecipeId"] <= 857}
    results = {r["Result"].replace("OOTS_ART_", ""): rid for rid, r in old.items()}
    miss = sorted(set(results) - set(ART))
    extra = sorted(set(ART) - set(results))
    assert not miss and not extra, ("разметка не совпадает с рецептами 16", miss, extra)

    # классы наших материалов и трофеев должны существовать в моде
    cfgtxt = "".join(io.open(os.path.join(MOD, "cfg", f), encoding="utf-8").read() for f in ("materials.hpp", "trophies.hpp"))
    have = set(re.findall(r"class (VB_AB_\w+)", cfgtxt))
    for e, ms in MATS.items():
        for k in ms:
            assert "VB_AB_Mat_" + k in have, k
        for tr in TROPHY[e]:
            assert tr in have, tr

    art_name = {a: old[rid]["RecipeName"].replace("Артефакт - ", "") for a, rid in results.items()}
    ru = names_ru()
    inputs = pick_inputs()
    new = []
    review = []
    for a, rid in sorted(results.items(), key=lambda x: x[1]):
        e, t = ART[a]
        o = old[rid]
        comps = [comp("OOTS_ART_" + x, 1) for x in inputs[a]]
        comps += [mat(k, n) for k, n in materials(e, t)]
        if a in RIFT:
            comps += [comp(n, k, 0.0 if n in (HEART, SPORES) else 0.5) for n, k in rift(t)]
        eft = [c for c in o["CraftComponents"] if c["Classname"].startswith("EFT_") and c["Delete"] == 1]
        if t == 1:
            eft = eft[:1]
        comps += [dict(c) for c in eft]
        if t == 4:
            comps.append(comp(TROPHY[e], 1, 0.0))
        comps += [dict(c) for c in o["CraftComponents"] if c["Delete"] == 0]      # лабораторные инструменты
        r = dict(o)
        r["CraftComponents"] = comps
        desc = "Артефакт стихии «%s», тир %s: материалы аномалий своей стихии" % (RU[e], ROMAN[t])
        if t == 4:
            desc += " и трофей босса этой стихии"
        if a in RIFT:
            desc += ", сырьё Изнанки"
        r["Description"] = desc + "."
        new.append(r)
        review.append((rid, art_name[a], a, RU[e], ROMAN[t],
                       ", ".join(art_name[x] for x in inputs[a]) or "—",
                       ", ".join("%s ×%d" % (ru.get("STR_VB_AB_Mat_" + k, k), n) for k, n in materials(e, t)),
                       "трофей «%s»" % RU[e] if t == 4 else "",
                       "да" if a in RIFT else "",
                       ", ".join(c["Classname"].replace("EFT_", "") for c in eft)))

    ids = {r["RecipeId"] for r in new}
    full = [r for r in cfg if r["RecipeId"] not in ids] + new
    full.sort(key=lambda r: r["RecipeId"])
    with io.open(os.path.join(HERE, "ab_artifacts_16.json"), "w", encoding="utf-8", newline="\r\n") as f:
        json.dump(new, f, ensure_ascii=False, indent=4)
    with io.open(os.path.join(HERE, "CraftConfig_16.json"), "w", encoding="utf-8", newline="\r\n") as f:
        json.dump(full, f, ensure_ascii=False, indent=4)

    lines = ["# Крафт артефактов сервера 16 — таблица на проверку",
             "",
             "Сгенерировано `make_ab_craft16.py`. Логика — ТЗ 2.4. Во всех рецептах ещё лабораторные инструменты",
             "(респиратор, перчатки, горелка, микроскоп — не расходуются). «Из» — артефакты-ингредиенты (расходуются).",
             ""]
    for e in RING:
        lines += ["## " + RU[e] + " — материалы: " +
                  ", ".join(ru.get("STR_VB_AB_Mat_" + k, k) for k in MATS[e]), "",
                  "| ID | Артефакт | Класс | Тир | Из | Материалы аномалий | Трофей | Изнанка | EFT |",
                  "|---|---|---|---|---|---|---|---|---|"]
        for row in sorted((x for x in review if x[3] == RU[e]), key=lambda x: ("I II III IV".split().index(x[4]), x[0])):
            lines.append("| %d | %s | `%s` | %s | %s | %s | %s | %s | %s |" % (row[0], row[1], row[2], row[4], row[5],
                                                                           row[6], row[7], row[8], row[9]))
        lines.append("")
    with io.open(os.path.join(HERE, "artifacts_16.md"), "w", encoding="utf-8", newline="\r\n") as f:
        f.write("\n".join(lines) + "\n")

    per = {}
    for a, (e, t) in ART.items():
        per.setdefault(RU[e], [0, 0, 0, 0])[t - 1] += 1
    print("рецептов артефактов: %d, всего на 16: %d" % (len(new), len(full)))
    for k, v in per.items():
        print("  %-10s I %d  II %d  III %d  IV %d" % (k, *v))


if __name__ == "__main__":
    main()
