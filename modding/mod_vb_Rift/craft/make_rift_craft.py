# -*- coding: utf-8 -*-
"""Рецепты Изнанки для DRL_CraftMenu сервера 16.

  python make_rift_craft.py

Что делает:
  1. Изделия Изнанки (ID 900–910, категория «Изнанка»): жилы из корня, слушающий корень, жгут, нить, ловушка,
     фонарь, кровь, капсула, сердце-аккумулятор, голодный клинок, плащ-кокон.
  2. Артефакты (ID 800–857, категория «Артефакты»): берёт рецепты с сервера 18 и меняет сырьё, которого нет на 16
     (руда BRDK_*, руны и кристаллы MSFZ_*, мясо мутантов MSFZ), на Изнанку и EFT по ступени артефакта:
       I   — 2 Корня + Жила + EFT-деталь по теме;
       II  — 2 Жилы + Споры + EFT-деталь;
       III — Сердце Изнанки + 2 Споры + 2 Жилы + 2 EFT-детали.
     Цепочки «артефакт из артефактов», части мутантов SFP_* и инструменты (респиратор, перчатки, горелка,
     микроскоп) остаются как на 18.
  3. Пишет рядом:
       rift_recipes_16.json      — только новые рецепты;
       rift_categories_16.json   — только новые категории;
       CraftConfig_16.json, CraftCategories_16.json — полные файлы 16-го с добавленным (готовы к выкладке).
Живые файлы сервера не трогает.
"""
import io
import json
import os

HERE = os.path.dirname(os.path.abspath(__file__))
SRV16 = r"D:\DayZavr\DayZavr-Servers\DayZServer16\Instance_1\DRL_CraftMenu"
SRV18 = r"D:\DayZavr\DayZavr-Servers\DayZServer18\Instance_1\DRL_CraftMenu"

ROOT = "VB_RiftRootPiece"
VEIN = "VB_RiftVein"
SPORES = "VB_RiftSpores"
HEART = "VB_RiftHeartChunk"
KNIVES = ["KitchenKnife", "SteakKnife", "HuntingKnife", "CombatKnife", "KukriKnife", "FangeKnife", "Machete",
          "CrudeMachete", "OrientalMachete", "Cleaver", "StoneKnife", "BoneKnife", "VB_RiftBlade"]
BENCH = ["DRL_Craft_Workbench"]
SEWING = ["DRL_Craft_Sewing"]


def comp(names, amount=1, recraft=0.5, inherit=0, att=0):
    """Расходник. recraft — шанс вернуть при разборе (0 для Сердца и Спор: разбор не должен их «освежать»)."""
    names = [names] if isinstance(names, str) else list(names)
    return {"Classname": names[0], "Classnames": names, "EnableVariationSelection": 1, "MinHealthFrac": 3,
            "Amount": amount, "QuantityFrac": 0.0, "Delete": 1, "ApplyDamageFrac": 0.0,
            "RecraftDropChance": recraft, "RecraftAmount": amount, "RecraftQuantityFrac": 0.0,
            "UseInheritance": inherit, "AttachmentsEx": att, "RecraftNeedAttachments": 0}


def tool(names, dmg=0.02, att=2):
    """Инструмент: не расходуется, изнашивается на dmg. att: 1 — навес станка, 2 — навес или инвентарь."""
    c = comp(names, 1, 0.0, 0, att)
    c["Delete"] = 0
    c["ApplyDamageFrac"] = dmg
    return c


def mat(name, amount=1):
    """Материал Изнанки: Сердце и Споры при разборе не возвращаются."""
    return comp(name, amount, 0.0 if name in (HEART, SPORES) else 0.5)


def recipe(rid, name, result, comps, desc, time, free=0, bench=BENCH, count=1):
    return {"RecipeId": rid, "ShowInFreeMenu": free, "WorkbenchClasses": list(bench), "RecipeName": name,
            "Result": result, "ResultClasses": [], "PreviewClassname": "", "CraftIcon": "", "Description": desc,
            "CraftTimeSec": time, "RecraftTimeSec": time, "ResultCount": count, "ResultQuantityFrac": 0.0,
            "ResultDamageFrac": 0.0, "CraftComponents": comps}


SEW_TOOLS = [tool("EFT_Sewing", 0.05, 2), tool("DRL_Craft_Sewing_scissors", 0.0, 1),
             tool("DRL_Craft_Metalic_thimble", 0.0, 1)]


def rift_items():
    R = []
    R.append(recipe(900, "Жилы Изнанки", VEIN, [mat(ROOT), tool(KNIVES, 0.02)],
                    "Расщепляешь корень Изнанки ножом вдоль и вытягиваешь из него две светящиеся жилы.",
                    5, free=1, count=2))
    R.append(recipe(901, "Слушающий корень", "VB_RiftListenRoot",
                    [mat(ROOT, 3), mat(VEIN, 2), comp("EFT_Capacitors"), tool(KNIVES, 0.02)],
                    "Закручиваешь корни в раструб, стягиваешь жилами и вплетаешь конденсатор. Ночью в нём слышно "
                    "ближайшее живое сердце Изнанки. 10 зарядов, подпитывается у живого сердца.", 10))
    R.append(recipe(902, "Живой жгут", "VB_RiftTourniquet", [mat(VEIN, 2), comp("Rag", 2)],
                    "Сплетаешь жилы в ремень, обматываешь тряпкой. Жгут сам стянет рану, когда пойдёт кровь, и немного её выпьет.",
                    5, free=1))
    R.append(recipe(903, "Нить Изнанки", "VB_RiftThread", [mat(VEIN, 2)] + SEW_TOOLS,
                    "Скручиваешь жилы в тонкую живую нить и наматываешь на катушку. Пришитая к одежде, она часа три сама затягивает прорехи.",
                    6, bench=SEWING))
    R.append(recipe(904, "Споровая ловушка", "VB_RiftSporeTrap", [mat(SPORES), comp("Rag"), comp("EFT_SiliconTube")],
                    "Заворачиваешь споры в тряпку и вставляешь трубку-запал. Раскрой и брось: облако тянет зомби "
                    "и через десять секунд лопается.", 6, free=1))
    R.append(recipe(905, "Фонарь-сердце", "VB_RiftLantern",
                    [mat(SPORES), comp("GlassBottle"), mat(VEIN), comp("EFT_Wires")],
                    "Сажаешь споры в бутылку, оплетаешь жилой и проводами. Светит красным часов шесть, "
                    "бьётся чаще рядом с зомби, а ночью на свет приходят зомби Изнанки.", 8))
    R.append(recipe(906, "Кровь Изнанки", "VB_RiftBlood", [mat(SPORES), mat(VEIN, 2), comp("WaterBottle")],
                    "Выжимаешь жилы в воду, растворяешь споры и запаиваешь в ампулу. Укол быстро возвращает кровь, но минут "
                    "десять трясутся руки и колотится сердце, а зомби вокруг светятся в темноте.", 8))
    R.append(recipe(907, "Канистра Изнанки", "VB_RiftFuelCapsule",
                    [mat(SPORES), comp("CanisterGasoline"), comp("EFT_Hose")],
                    "Сбраживаешь споры в канистре до горючего сока, канистра прорастает жилами. Двадцать литров в бак, "
                    "но полчаса за машиной тянется красный шлейф.", 8))
    R.append(recipe(908, "Сердце-аккумулятор", "VB_RiftHeartBattery",
                    [mat(HEART), mat(VEIN, 3), comp("EFT_Carbattery"), comp("EFT_Wires"),
                     tool(["Pliers", "EFT_kusajki", "EFT_Cablecutter"], 0.03)],
                    "Подключаешь живое сердце Изнанки к клеммам аккумулятора. Ночью само заряжается, днём слабеет; "
                    "рядом с живым гнездом стучит в салоне.", 12))
    R.append(recipe(909, "Голодный клинок", "VB_RiftBlade",
                    [comp(["Machete", "CrudeMachete", "OrientalMachete", "FangeKnife", "CombatKnife", "HuntingKnife",
                           "KukriKnife"], 1, 0.0), mat(HEART), mat(ROOT, 2), comp("EFT_MetalParts")],
                    "Вживляешь в клинок кусок сердца Изнанки и обвиваешь рукоять корнями. Убийства возвращают кровь, "
                    "но голодный клинок пьёт кровь хозяина.", 12))
    R.append(recipe(910, "Плащ-кокон", "VB_RiftCloak",
                    [comp("Raincoat_ColorBase", 1, 0.0, inherit=1), mat(VEIN, 6), mat(ROOT), comp("EFT_Cloth_aramid", 2)]
                    + SEW_TOOLS,
                    "Прошиваешь дождевик жилами Изнанки и подбиваешь арамидом. Тёплый и непромокаемый; ночью, если "
                    "присесть и замереть, обычные зомби теряют тебя.", 15, bench=SEWING))
    return R


# ---------------------------------------------------------------- артефакты
OLD_PREFIXES = ("BRDK_", "MSFZ_", "MutantRicar", "MutantTroll")

# тема артефакта по классу результата -> EFT-детали (первая для I/II, обе для III)
THEMES = [
    (("Plamya", "Burn", "vspishka", "FireBall", "Sparkler"), ["EFT_Gasolin", "EFT_Kerosin"]),
    (("Battery", "Mohovik", "Gravi", "vivert", "NanoCell", "Plasma"), ["EFT_Capacitors", "EFT_Rele"]),
    (("Crustal", "snejinka", "MoonLight", "NightStar", "svet", "Light"), ["EFT_Lens", "EFT_OrgGlass"]),
    (("Drop", "Klyaksa", "puzir", "Medusa", "Oasis", "Flower"), ["EFT_SiliconTube", "EFT_Hose"]),
    (("Thorn", "SeaUrchin", "shturval", "Spring", "Compas", "Monolit"), ["EFT_MetalParts", "EFT_Magnet"]),
    (("kameniy_cvetok", "krov_kamnya", "BloodStone", "izolator", "ShineOfTheForest"), ["EFT_Mounting_foam", "EFT_Poxearm"]),
    (("myaso", "kolobok", "Mikrob", "Braen", "Glaz", "DemonEye", "DevelHert", "DarkSoul", "busy"), ["EFT_Bloodset", "EFT_Bleach"]),
    (("Angel", "ribka"), ["EFT_gold_chain", "EFT_incandescentlamp"]),
]


def theme(result):
    for keys, eft in THEMES:
        if any(k.lower() in result.lower() for k in keys):
            return eft
    return ["EFT_MetalParts", "EFT_Wires"]


def tier(r):
    if "III" in r["RecipeName"] or r["CraftTimeSec"] >= 13:
        return 3
    if r["CraftTimeSec"] >= 10:
        return 2
    return 1


def rift_artifacts():
    d18 = json.load(io.open(os.path.join(SRV18, "CraftConfig.json"), encoding="utf-8"))
    out = []
    for r in sorted((x for x in d18 if 800 <= x["RecipeId"] <= 857), key=lambda x: x["RecipeId"]):
        keep = [c for c in r["CraftComponents"] if not c["Classname"].startswith(OLD_PREFIXES)]
        arts = [c for c in keep if c["Delete"] == 1]
        tools = [c for c in keep if c["Delete"] == 0]
        eft = theme(r["Result"])
        t = tier(r)
        if t == 1:
            new = [mat(ROOT, 2), mat(VEIN), comp(eft[0])]
        elif t == 2:
            new = [mat(VEIN, 2), mat(SPORES), comp(eft[0])]
        else:
            new = [mat(HEART), mat(SPORES, 2), mat(VEIN, 2), comp(eft[0]), comp(eft[1])]
        rr = dict(r)
        rr["CraftComponents"] = arts + new + tools
        rr["Description"] = ("Артефакт из сырья Изнанки: ступень " + "I" * t + ". "
                             + ("Сердце Изнанки протухает к рестарту — крафтить в ту же ночь." if t == 3 else ""))
        rr["Description"] = rr["Description"].strip()
        out.append(rr)
    return out


def category(name, icon, ids, sound):
    return {"Name": name, "Icon": icon, "ShowAll": 0, "UseList": 1, "ShowOnlyAvailableCraft": 0,
            "ShowOnlyAvailableStudy": 0, "CategoryFonImagePath": "set:drl_craft_work image:work",
            "CategoryCraftFonImagePath": "set:drl_craft_recipemanust image:recipemanust",
            "CategoryCraftSoundSet": sound, "List": ids}


def dump(obj, path):
    with io.open(path, "w", encoding="utf-8", newline="\r\n") as f:
        f.write(json.dumps(obj, ensure_ascii=False, indent=4))


def main():
    items = rift_items()
    # С 2026-11-13 артефакты 800–857 собирает крафт аномалий (mod_vb_AnomaliesBosses/craft/make_ab_craft16.py):
    # артефакт — из материалов аномалий своей стихии, Изнанка только в органике и пси. Здесь их больше не трогаем,
    # иначе повторный запуск вернул бы старые рецепты. rift_artifacts() оставлен как история.
    arts = []
    new = items + arts
    cats = [category("Изнанка", "DRL_Core/Icon/drl_bleeding-heart.edds", [r["RecipeId"] for r in items],
                     "DRLCraft_Generic1_SoundSet")]

    cfg16 = json.load(io.open(os.path.join(SRV16, "CraftConfig.json"), encoding="utf-8"))
    cat16 = json.load(io.open(os.path.join(SRV16, "CraftCategories.json"), encoding="utf-8"))
    # повторный запуск: свои прошлые рецепты и категории убираем, остальное не трогаем
    new_ids = {r["RecipeId"] for r in new}
    cfg16 = [r for r in cfg16 if r["RecipeId"] not in new_ids]
    cat_names = {c["Name"] for c in cats}
    cat16 = [c for c in cat16 if c["Name"] not in cat_names]

    dump(new, os.path.join(HERE, "rift_recipes_16.json"))
    dump(cats, os.path.join(HERE, "rift_categories_16.json"))
    dump(cfg16 + new, os.path.join(HERE, "CraftConfig_16.json"))
    dump(cat16 + cats, os.path.join(HERE, "CraftCategories_16.json"))

    used = sorted({n for r in new for c in r["CraftComponents"] for n in c["Classnames"]})
    print("изделий: %d, артефактов: %d, всего рецептов на 16 станет: %d" % (len(items), len(arts), len(cfg16) + len(new)))
    print("классы в рецептах:", ", ".join(used))
    for r in arts:
        mats = [c["Classname"] + "x" + str(c["Amount"]) for c in r["CraftComponents"] if c["Delete"] == 1]
        print(r["RecipeId"], r["Result"], "ступень", tier(r), ":", ", ".join(mats))


if __name__ == "__main__":
    main()
