# -*- coding: utf-8 -*-
"""Крафт детекторов сервера 16 (ТЗ 2.2, решение пользователя 2026-11-13: ровно 8 расходуемых ингредиентов).

  python make_ab_detectors16.py            — собрать файлы рядом (живые файлы сервера не трогает)
  python make_ab_detectors16.py --deploy   — то же + бэкап и выкладка в DayZServer16\\Instance_1\\DRL_CraftMenu

Прогрессия — цепочка улучшений, каждый следующий детектор собирается из предыдущего:
  Лоза ДА-1    — обычный лут и простые крафты верстака (провода 17, детали 16, скотч 14): вход для всех;
  Огонёк ДА-2  — Лоза + материалы полей лёгких боссов (Ричары, ур. 1);
  Пеленг ДА-3  — Огонёк + компас + материалы лёгких аномалий средних боссов (ур. 2);
  Сито ДА-4    — Пеленг + материалы тяжёлых аномалий средних боссов (ур. 2);
  Невод ДА-5   — Сито + материалы полей боссов ур. 3 + трофей любого босса.
В каждом рецепте ровно 8 расходуемых позиций (Delete 1), инструменты не в счёт. Детектор-основа засчитывается
с наследниками (UseInheritance): старые Anomaly_Detector_Otklik/Medved/Svarog игроков 16 — наследники Лозы, Пеленга
и Сита, ими тоже можно улучшать. Батарейка в рецепт не входит; основа удаляется целиком — вместе с батарейкой внутри.
Инструменты растут с уровнем: отвёртка и плоскогубцы (из инвентаря) у всех, набор инструментов верстака — с Пеленга,
микроскоп верстака — у Сита и Невода (калибровка по материалам аномалий).

Пишет рядом:
  ab_detectors_16.json     — 5 рецептов (ID 950–954);
  CraftConfig_16.json      — полный конфиг 16-го: живой + эти рецепты (тот же файл пишет make_ab_craft16.py;
                             оба берут за основу живой конфиг, поэтому друг другу не мешают);
  CraftCategories_16.json  — полный файл категорий с категорией «Детекторы»;
  detectors_16.md          — таблица на проверку: состав, где брать материалы.
"""
import csv
import datetime
import io
import json
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
MOD = os.path.join(HERE, "..", "VB_AnomaliesBosses")
SERVER_DEF = os.path.join(HERE, "..", "VB_AnomaliesBosses_Server", "defaults", "bosses.json")
SRV16 = r"D:\DayZavr\DayZavr-Servers\DayZServer16\Instance_1\DRL_CraftMenu"
BACKUP = r"G:\DayZ_Test\backup"
P_DZ = "P:" + chr(92) + "DZ"

CATEGORY = "Детекторы"
ICON = "DRL_Core/Icon/drl_radar-sweep.edds"
SOUND = "DRLCraft_Generic1_SoundSet"
WB = ["DRL_Craft_Workbench"]
TROPHIES = ["VB_AB_Trophy_Ricar_Acid", "VB_AB_Trophy_Ricar_Rad", "VB_AB_Trophy_Tenevnik_Psi",
            "VB_AB_Trophy_Tenevnik_Rad", "VB_AB_Trophy_Shum_Ice", "VB_AB_Trophy_Shum_Gravity",
            "VB_AB_Trophy_MutaWolf_Ice", "VB_AB_Trophy_MutaWolf_Fire", "VB_AB_Trophy_MozgoTras_Psi",
            "VB_AB_Trophy_MozgoTras_Storm", "VB_AB_Trophy_MozgoTrasRed_Fire", "VB_AB_Trophy_MozgoTrasRed_Gravity",
            "VB_AB_Trophy_Ent_Storm", "VB_AB_Trophy_Ent_Acid"]
# ванильные предметы рецептов (проверяются по P:\DZ)
VANILLA = {"Compass": "gear/navigation", "PersonalRadio": "gear/radio", "MetalWire": "gear/crafting",
           "Screwdriver": "gear/tools", "Pliers": "gear/tools"}


# названия для таблицы на проверку (в игре названия берутся из модов)
ITEM_RU = {"MetalWire": "Металлическая проволока", "EFT_Magnet": "Магнит", "EFT_incandescentlamp": "Лампа накаливания",
           "EFT_Capacitors": "Конденсаторы", "PersonalRadio": "Рация", "EFT_Wires": "Моток медной электропроводки",
           "EFT_MetalParts": "Набор металлических деталей", "EFT_skoth": "Армированный скотч", "EFT_Rele": "Реле",
           "Compass": "Компас", "EFT_Lens": "Линза", "EFT_OrgGlass": "Оргстекло",
           "EFT_Military_cable": "Военный кабель", "Screwdriver": "отвёртка", "Pliers": "плоскогубцы",
           "DRL_Craft_Tools": "набор инструментов на верстаке", "DRL_Craft_Microscope": "микроскоп на верстаке"}
# что ещё и крафтится на верстаке 16 (номер рецепта)
FROM_WB = {"EFT_Wires": 17, "EFT_MetalParts": 16, "EFT_skoth": 14}


def comp(names, amount=1, recraft=0.5, inherit=0):
    names = [names] if isinstance(names, str) else list(names)
    return {"Classname": names[0], "Classnames": names, "EnableVariationSelection": 1, "MinHealthFrac": 3,
            "Amount": amount, "QuantityFrac": 0.0, "Delete": 1, "ApplyDamageFrac": 0.0,
            "RecraftDropChance": recraft, "RecraftAmount": amount, "RecraftQuantityFrac": 0.0,
            "UseInheritance": inherit, "AttachmentsEx": 0, "RecraftNeedAttachments": 0}


def tool(cls, wear, where):
    """Инструмент: не расходуется, изнашивается на wear. where: 2 — инвентарь или верстак, 1 — обвес верстака."""
    return {"Classname": cls, "Classnames": [cls], "EnableVariationSelection": 1, "MinHealthFrac": 3,
            "Amount": 1, "QuantityFrac": 0.0, "Delete": 0, "ApplyDamageFrac": wear,
            "RecraftDropChance": 0.0, "RecraftAmount": 1, "RecraftQuantityFrac": 0.0,
            "UseInheritance": 0, "AttachmentsEx": where, "RecraftNeedAttachments": 0}


def det(key):
    return "VB_AB_Detector_" + key


def mat(key, n):
    return comp("VB_AB_Mat_" + key, n)


HAND_TOOLS = [tool("Screwdriver", 0.1, 2), tool("Pliers", 0.1, 2)]
WB_TOOLS = [tool("DRL_Craft_Tools", 0.02, 1)]
LAB = [tool("DRL_Craft_Microscope", 0.02, 1)]
BATTERY = " Батарейка 9 В в рецепт не входит; из старого детектора выньте её заранее — основа уходит в сборку целиком."

# ID, ключ класса, название, время, описание, расходники (ровно 8), инструменты
RECIPES = [
    (950, "Loza", "Детектор «Лоза» ДА-1", 8,
     "Простейший детектор аномальных находок: две проволочные «лозы», магнит-датчик и лампа от обычной лампочки. "
     "Ищет только впереди, 10 м; чем ближе находка, тем ярче лампа. Собирается из обычного лута — с него начинают "
     "все." + BATTERY.split(";")[0] + ".",
     [comp("MetalWire", 2), comp("EFT_Magnet", 1), comp("EFT_incandescentlamp", 1), comp("EFT_Capacitors", 1),
      comp("PersonalRadio", 1), comp("EFT_Wires", 1), comp("EFT_MetalParts", 1), comp("EFT_skoth", 1)],
     HAND_TOOLS),
    (951, "Ogonek", "Детектор «Огонёк» ДА-2", 10,
     "Улучшение «Лозы»: лампа-маячок мигает тем чаще, чем ближе находка, радиус 12 м. Светящаяся плесень даёт "
     "маячку свет, слизь Студня — гель для датчика. Материалы — из полей самых слабых боссов (Ричары)." + BATTERY,
     [comp(det("Loza"), 1, 1.0, 1), mat("Holera", 3), mat("Studen", 2), comp("EFT_Rele", 1),
      comp("EFT_incandescentlamp", 1), comp("EFT_Capacitors", 1), comp("EFT_Wires", 1), comp("EFT_skoth", 1)],
     HAND_TOOLS),
    (952, "Peleng", "Детектор «Пеленг» ДА-3", 12,
     "Улучшение «Огонька»: стрелка на экране поворачивается к находке, радиус 15 м. Стрелку даёт компас, "
     "гравитационная галька тянет её к находке, линзовое стекло — экран. Материалы — из полей средних боссов: "
     "Шум-Воронка, Пси-Мозготряс, Снежная вдова, Теневик-мираж." + BATTERY,
     [comp(det("Ogonek"), 1, 1.0, 1), comp("Compass", 1), mat("Tramplin", 3), mat("LinceAgr", 2),
      comp("EFT_Magnet", 2), comp("EFT_Lens", 1), comp("EFT_Wires", 2), comp("EFT_MetalParts", 1)],
     HAND_TOOLS + WB_TOOLS),
    (953, "Sito", "Детектор «Сито» ДА-4", 14,
     "Улучшение «Пеленга»: до 4 находок сразу метками на экране, полукруг впереди, 20 м. Разрядный кварц питает "
     "экран, сжатый камень и застывший пузырь держат метки. Материалы — из тяжёлых аномалий средних боссов: "
     "Грозовой Мозготряс, Шум-Воронка, Ледяной Мутаволк." + BATTERY,
     [comp(det("Peleng"), 1, 1.0, 1), mat("Electra", 3), mat("Voronka", 2), mat("Zastoy", 2),
      comp("EFT_Capacitors", 2), comp("EFT_Rele", 1), comp("EFT_OrgGlass", 1), comp("EFT_Wires", 2)],
     HAND_TOOLS + WB_TOOLS + LAB),
    (954, "Nevod", "Детектор «Невод» ДА-5", 15,
     "Вершина: до 4 находок, полукруг впереди, 25 м — самый большой радиус. Звёздный песок и световая пыль "
     "рисуют сеть меток, скрученный камень тянет сигнал издалека, трофей босса — сердце прибора. Материалы — "
     "из полей сильнейших боссов: Громовой Энт, Давящий Мозготряс; световая пыль — Пси-Мозготряс и "
     "Теневик-мираж." + BATTERY,
     [comp(det("Sito"), 1, 1.0, 1), comp(TROPHIES, 1, 0.0), mat("MlechniPut", 3), mat("Lomka", 2),
      mat("Flash", 2), comp("EFT_Capacitors", 2), comp("EFT_Magnet", 2), comp("EFT_Military_cable", 1)],
     HAND_TOOLS + WB_TOOLS + LAB),
]


def recipe(rid, key, name, sec, desc, consum, tools):
    return {"RecipeId": rid, "ShowInFreeMenu": 0, "WorkbenchClasses": list(WB), "RecipeName": name,
            "Result": det(key), "ResultClasses": [], "PreviewClassname": "", "CraftIcon": "",
            "Description": desc, "CraftTimeSec": sec, "RecraftTimeSec": sec, "ResultCount": 1,
            "ResultQuantityFrac": 0.0, "ResultDamageFrac": 0.0,
            "CraftComponents": [dict(c) for c in consum] + [dict(t) for t in tools]}


def category(ids):
    return {"Name": CATEGORY, "Icon": ICON, "ShowAll": 0, "UseList": 1, "ShowOnlyAvailableCraft": 0,
            "ShowOnlyAvailableStudy": 0, "CategoryFonImagePath": "set:drl_craft_work image:work",
            "CategoryCraftFonImagePath": "set:drl_craft_recipemanust image:recipemanust",
            "CategoryCraftSoundSet": SOUND, "List": ids}


def mod_classes():
    txt = ""
    for root, _, files in os.walk(MOD):
        for f in files:
            if f.endswith((".cpp", ".hpp")):
                txt += io.open(os.path.join(root, f), encoding="utf-8", errors="replace").read()
    return set(re.findall(r"class (\w+)\s*:", txt))


def names_ru():
    rows = list(csv.reader(io.open(os.path.join(MOD, "stringtable.csv"), encoding="utf-8")))
    ri = [h.lower() for h in rows[0]].index("russian")
    return {r[0]: r[ri] for r in rows[1:] if r}


def sources():
    """Материал -> боссы, в чьём поле стоит его аномалия (bosses.json по умолчанию)."""
    bosses = json.load(io.open(SERVER_DEF, encoding="utf-8"))["Bosses"]
    out = {}
    for b in bosses:
        seen = {}

        def names(a):
            # Classname — одна аномалия, Classnames — одна из списка на выбор (тогда «не всегда»)
            return [a["Classname"]] if "Classname" in a else a.get("Classnames", [])

        for a in b["Anomalies"]:
            for cls in names(a):
                k = cls.replace("VB_AB_Anomaly_", "")
                seen[k] = seen.get(k, False) or ("Classname" in a and a.get("Chance", 100) >= 100)
        for p in b.get("Phases", []):
            for a in p.get("Anomalies", []):
                for cls in names(a):
                    seen.setdefault(cls.replace("VB_AB_Anomaly_", ""), False)
        for k, always in seen.items():
            out.setdefault(k, []).append("%s (ур. %d%s)" % (b["Name"], b["Tier"], "" if always else ", не всегда"))
    return out


def load(name):
    return json.load(io.open(os.path.join(SRV16, name), encoding="utf-8-sig"))


def dump(obj, path):
    with io.open(path, "w", encoding="utf-8", newline="\r\n") as f:
        json.dump(obj, f, ensure_ascii=False, indent=4)


def main():
    deploy = "--deploy" in sys.argv[1:]
    cfg = load("CraftConfig.json")
    cats = load("CraftCategories.json")
    ours = {r[0] for r in RECIPES}

    # проверки: ровно 8 расходников, классы существуют, ID не заняты чужими рецептами
    have = mod_classes()
    live_cls = {c2 for r in cfg for c in r["CraftComponents"] for c2 in c["Classnames"]}
    for rid, key, name, _, _, consum, tools in RECIPES:
        assert det(key) in have, det(key)
        n = sum(1 for c in consum if c["Delete"] == 1)
        assert n == 8 and len(consum) == 8, "%s: расходников %d" % (name, n)
        for c in consum + tools:
            for cls in c["Classnames"]:
                if cls.startswith("VB_AB_"):
                    assert cls in have, "нет в моде: " + cls
                elif cls in VANILLA:
                    p = os.path.join(P_DZ, *VANILLA[cls].split("/"), "config.cpp")
                    assert re.search(r"class %s\s*:" % cls, io.open(p, encoding="utf-8", errors="replace").read()), cls
                else:
                    assert cls in live_cls, "класс не встречается в крафте 16: " + cls
    clash = [r["RecipeId"] for r in cfg if r["RecipeId"] in ours and not r["Result"].startswith("VB_AB_Detector_")]
    assert not clash, ("ID заняты чужими рецептами", clash)

    new = [recipe(*r) for r in RECIPES]
    full = [r for r in cfg if r["RecipeId"] not in ours] + new
    full.sort(key=lambda r: r["RecipeId"])
    cats2 = [c for c in cats if c["Name"] != CATEGORY] + [category(sorted(ours))]

    dump(new, os.path.join(HERE, "ab_detectors_16.json"))
    dump(full, os.path.join(HERE, "CraftConfig_16.json"))
    dump(cats2, os.path.join(HERE, "CraftCategories_16.json"))

    ru = names_ru()
    src = sources()
    title = {}
    for rid, key, name, *_ in RECIPES:
        title[det(key)] = name.replace("Детектор ", "")

    def label(c):
        cls = c["Classname"]
        if cls.startswith("VB_AB_Trophy_"):
            return "трофей любого босса"
        if cls.startswith("VB_AB_Detector_"):
            return title[cls] + (" (или старый OOTS)" if cls[15:] in ("Loza", "Peleng", "Sito") else "")
        if cls.startswith("VB_AB_Mat_"):
            return ru.get("STR_VB_AB_Mat_" + cls[10:], cls)
        return ITEM_RU.get(cls, cls)

    lines = ["# Крафт детекторов сервера 16 — таблица на проверку", "",
             "Сгенерировано `make_ab_detectors16.py`. Категория «%s», верстак, ID 950–954, " % CATEGORY +
             "в каждом рецепте ровно 8 расходуемых позиций.", ""]
    for rid, key, name, sec, desc, consum, tools in RECIPES:
        lines += ["## %d. %s (%d с)" % (rid, name, sec), "", desc, "",
                  "| # | Ингредиент | Кол. | Где брать |", "|---|---|---|---|"]
        for i, c in enumerate(consum, 1):
            where = ""
            if c["Classname"].startswith("VB_AB_Mat_"):
                where = "; ".join(src.get(c["Classname"][10:], ["нигде — проверить!"]))
            elif c["Classname"].startswith("VB_AB_Detector_"):
                where = "предыдущий шаг"
            elif c["Classname"].startswith("VB_AB_Trophy_"):
                where = "вырезать из тела любого босса"
            elif c["Classname"] in FROM_WB:
                where = "лут или верстак, рецепт %d" % FROM_WB[c["Classname"]]
            else:
                where = "обычный лут"
            lines.append("| %d | %s | %d | %s |" % (i, label(c), c["Amount"], where))
        lines += ["", "Инструменты (не расходуются): " + ", ".join(ITEM_RU.get(t["Classname"], t["Classname"])
                                                               for t in tools) + ".", ""]
    with io.open(os.path.join(HERE, "detectors_16.md"), "w", encoding="utf-8", newline="\r\n") as f:
        f.write("\n".join(lines) + "\n")

    print("рецептов детекторов: %d, всего на 16: %d, категорий: %d" % (len(new), len(full), len(cats2)))
    if deploy:
        stamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        bdir = os.path.join(BACKUP, stamp + "_srv16_DRL_CraftMenu_detectors")
        os.makedirs(bdir)
        for fn in ("CraftConfig.json", "CraftCategories.json"):
            shutil.copy2(os.path.join(SRV16, fn), os.path.join(bdir, fn))
        dump(full, os.path.join(SRV16, "CraftConfig.json"))
        dump(cats2, os.path.join(SRV16, "CraftCategories.json"))
        print("выложено, бэкап:", bdir)


if __name__ == "__main__":
    main()
