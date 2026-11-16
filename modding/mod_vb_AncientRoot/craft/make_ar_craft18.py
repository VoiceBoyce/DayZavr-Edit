# -*- coding: utf-8 -*-
"""Крафт изделий «Древнего корня» в DRL сервера 18 и скупка даров леса у Дока (ТЗ_сервер_18.md, этап 2, шаг 6).

  python make_ar_craft18.py            — собрать в craft/out (живые файлы не трогает) + ar_craft_18.md
  python make_ar_craft18.py --deploy   — то же + бэкап и выкладка в DayZServer18\\Instance_1 (локальная копия)
                                         + копия в «Обновлены или готовые моды\\18 сервер\\Instance_1\\…»

Что делает:
  1. Рецепты 900–911, категория «Древний корень»: лыко, вещун, ловушка, светоч, живица, клинок, плащ, оберег, настой,
     отвар примирения, Семя, Книга древних. Состав — по описаниям предметов в config.cpp мода
     («кусок янтаря и волчий клык на шнурке из живого лыка» и т. п.).
  2. Док (`Trader\\TraderConfig9_Doc.txt`): категория «Дары леса» — скупка сырья по тем же ценам, по которым считалась
     скупка артефактов (ТЗ этап 3.4): тогда артефакт всегда дороже своего сырья.
     Семя и лыко Док не берёт: их можно скрафтить, скупка превратила бы крафт в печатный станок.

Основа — текущие файлы 18: свои рецепты (900–911) и категория заменяются, чужие остаются байт в байт (самопроверка).
Формат DRL-файлов: UTF-8 без BOM, CRLF, отступ 4; файл Дока — табы, CRLF и кодировка как в исходнике.
"""
import datetime
import io
import json
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "quests18"))
from classindex18 import Index, INST18  # noqa: E402

LIVE_DRL = os.path.join(INST18, "DRL_CraftMenu")
LIVE_DOC = os.path.join(INST18, "Trader", "TraderConfig9_Doc.txt")
OUT = os.path.join(HERE, "out")
READY = os.path.join(REPO, "Обновлены или готовые моды", "18 сервер", "Instance_1")
BACKUP = r"G:\DayZ_Test\backup"

ID_MIN, ID_MAX = 900, 949
CATEGORY = "Древний корень"
DESC_MAX = 170          # окно DRL — две строки

# ---------------------------------------------------------------------------------------------- материалы
ROOT, VEIN, SPORES, AMBER = "VB_AR_RootPiece", "VB_AR_Vein", "VB_AR_Spores", "VB_AR_Amber"
HEART, DEAD, FANG = "VB_AR_HeartChunk", "VB_AR_HeartDead", "VB_AR_WolfFang"
# Разбор не возвращает то, что портится (сердцевина к рестарту каменеет, пыльца гаснет за сутки) и дорогое сырьё:
# иначе «скрафтил — разобрал» освежало бы его.
NO_RECRAFT = {HEART, SPORES, DEAD, AMBER, FANG}
KNIVES = ["KitchenKnife", "SteakKnife", "HuntingKnife", "CombatKnife", "KukriKnife", "FangeKnife", "Machete",
          "CrudeMachete", "OrientalMachete", "Cleaver", "StoneKnife", "BoneKnife", "VB_AR_Blade"]
GOOD_KNIVES = ["FangeKnife", "CombatKnife", "HuntingKnife", "KukriKnife"]
BENCH, SEWING = ["DRL_Craft_Workbench"], ["DRL_Craft_Sewing"]

# Скупка у Дока: цены = оценки сырья, по которым считались артефакты 18 (ТЗ_сервер_18.md, этап 3.4)
DOC = [
    (SPORES, 300, "Золотая пыльца — 3 порции у рощи за рестарт, дар за мясо; гаснет за сутки"),
    (ROOT, 500, "Отросток Древнего корня — отлом ночью у корня"),
    (FANG, 800, "Клык лесового волка — со стража рощи"),
    (AMBER, 1500, "Янтарь — дар рощи, раскол окаменевшей сердцевины"),
    (DEAD, 3000, "Окаменевшая сердцевина — вырезанная к рестарту каменеет"),
    ("VB_AR_AmberInclusion", 4000, "Янтарь с вкраплением — внутри руна или кристалл"),
    (HEART, 10000, "Сердцевина Древнего корня — вырезать ночью, к рестарту каменеет"),
    ("VB_AR_KeeperHeart", 15000, "Сердце Хранителя — с мини-босса древней рощи"),
]
DOC_CATEGORY = "Дары леса | Forest gifts"


def comp(names, amount=1, recraft=None, inherit=0, att=0):
    names = [names] if isinstance(names, str) else list(names)
    if recraft is None:
        recraft = 0.0 if names[0] in NO_RECRAFT else 0.5
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


SEW_TOOLS = [tool("EFT_Sewing", 0.05, 2), tool("DRL_Craft_Sewing_scissors", 0.0, 1),
             tool("DRL_Craft_Metalic_thimble", 0.0, 1)]


def recipe(rid, name, result, comps, desc, sec, free=0, bench=BENCH, count=1):
    return {"RecipeId": rid, "ShowInFreeMenu": free, "WorkbenchClasses": list(bench), "RecipeName": name,
            "Result": result, "ResultClasses": [], "PreviewClassname": "", "CraftIcon": "", "Description": desc,
            "CraftTimeSec": sec, "RecraftTimeSec": sec, "ResultCount": count,
            # 1.0: у вещуна (10 зарядов) и светоча (360 мин) 0 дал бы пустой предмет (проверено на лавке 16)
            "ResultQuantityFrac": 1.0, "ResultDamageFrac": 0.0, "CraftComponents": comps}


def recipes():
    R = []
    R.append(recipe(900, "Живое лыко", VEIN, [comp(ROOT), tool(KNIVES)],
                    "Снимаешь с отростка живое волокно и сматываешь на палочку. Из одного отростка — два мотка лыка.",
                    5, free=1, count=2))
    R.append(recipe(901, "Корень-вещун", "VB_AR_ListenRoot", [comp(ROOT, 2), comp(VEIN, 2), comp(SPORES), tool(KNIVES)],
                    "Отросток в раструбе, стянутом лыком, с пыльцой внутри. Ночью слышит ближайшую сердцевину, гудит "
                    "перед грозой. 10 зарядов.", 8))
    R.append(recipe(902, "Пыльцевая ловушка", "VB_AR_SporeTrap", [comp(SPORES, 2), comp(VEIN), comp("Rag")],
                    "Пыльца в мешочке, перетянутом лыком. Раскрой и брось: через 10 с звери и нечисть рядом уснут "
                    "секунд на двадцать.", 6, free=1))
    R.append(recipe(903, "Светоч корня", "VB_AR_Lantern", [comp(HEART), comp(ROOT, 3), comp(VEIN, 2), comp(SPORES)],
                    "Живая сердцевина в клетке из отростков. Горит часов шесть, заражённые к нему не подходят. "
                    "Сердцевина каменеет к рестарту — собирай в ту же ночь.", 10))
    R.append(recipe(904, "Живица", "VB_AR_Blood", [comp(AMBER), comp(VEIN), comp("Syringe")],
                    "Янтарь растапливаешь до густой живицы и набираешь в шприц, оплетённый лыком. Укол: кровь и силы, "
                    "кость срастается.", 8))
    R.append(recipe(905, "Терновый клинок", "VB_AR_Blade",
                    [comp(GOOD_KNIVES, 1, 0.0), comp(ROOT, 2), comp(VEIN, 2), comp("MSFZ_Zanoza_stick", 2), comp(AMBER)],
                    "Клинок, обвитый терновым отростком, с шипами Занозы. Рана кровоточит, жертва не может бежать; "
                    "убийства возвращают кровь.", 12))
    R.append(recipe(906, "Плащ лешего", "VB_AR_Cloak",
                    [comp("Raincoat_ColorBase", 1, 0.0, inherit=1), comp(VEIN, 4), comp(ROOT, 2), comp("BurlapSack")]
                    + SEW_TOOLS,
                    "Дождевик, обшитый корой отростков и мешковиной на живом лыке. Ночью присел и замер — нечисть и "
                    "звери тебя не видят.", 15, bench=SEWING))
    R.append(recipe(907, "Янтарный оберег", "VB_AR_Amulet", [comp(AMBER), comp(FANG), comp(VEIN)],
                    "Кусок янтаря и волчий клык на шнурке из живого лыка. При тебе — стражи рощ не нападают первыми, "
                    "Благословение дольше.", 6, free=1))
    R.append(recipe(908, "Настой пыльцы", "VB_AR_PollenBrew", [comp(SPORES, 2), comp("CaninaBerry", 2), comp("WaterBottle")],
                    "Пыльца, настоянная на шиповнике. Выпей — минут десять звери и стражи рощи светятся золотом даже "
                    "в темноте.", 6))
    R.append(recipe(909, "Отвар примирения", "VB_AR_Reconcile",
                    [comp(AMBER), comp("MSFZ_Spider_Lapka", 2), comp("CaninaBerry", 2), comp("WaterBottle")],
                    "Травы, паучий яд и крошка янтаря. Выпей ночью у живой рощи — лес смягчится и примет дар даже от "
                    "Проклятого. Раз в сутки.", 8))
    R.append(recipe(910, "Семя Древнего корня", "VB_AR_Seed", [comp(DEAD), comp(SPORES, 2), comp(AMBER)],
                    "Окаменевшую сердцевину не колешь, а будишь пыльцой и янтарём — выходит семя. Посади у вырезанной "
                    "сердцевины — роща вернётся.", 10))
    R.append(recipe(911, "Книга древних", "VB_AR_Book",
                    [comp("Paper", 4), comp(ROOT, 2), comp(VEIN, 2), comp(AMBER), comp("MSFZ_RuneStone_Day")],
                    "Листы в переплёте из коры, корешок из лыка, янтарь по углам, Руна Дня на крышке. Всё, что знали "
                    "о Древнем корне.", 15))
    return R


def category(ids):
    return {"Name": CATEGORY, "Icon": "DRL_Core/Icon/drl_tree-roots.edds", "ShowAll": 0, "UseList": 1,
            "ShowOnlyAvailableCraft": 0, "ShowOnlyAvailableStudy": 0,
            "CategoryFonImagePath": "set:drl_craft_work image:work",
            "CategoryCraftFonImagePath": "set:drl_craft_recipemanust image:recipemanust",
            "CategoryCraftSoundSet": "DRLCraft_Generic1_SoundSet", "List": sorted(ids)}


# ---------------------------------------------------------------------------------------------- файлы
def read_json(path):
    raw = open(path, "rb").read()
    tail = b""
    for t in (b"\r\n", b"\n"):
        if raw.endswith(t):
            tail = t
            break
    return json.loads(raw.decode("utf-8-sig")), tail


def dump_json(obj, path, tail=b""):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    data = json.dumps(obj, ensure_ascii=False, indent=4).replace("\n", "\r\n").encode("utf-8") + tail
    with open(path, "wb") as f:
        f.write(data)


def merged_drl(src_dir, R):
    (cfg, t1), (cc, t2) = read_json(os.path.join(src_dir, "CraftConfig.json")), \
        read_json(os.path.join(src_dir, "CraftCategories.json"))
    ours = {r["RecipeId"] for r in R}
    full = [r for r in cfg if r["RecipeId"] not in ours] + R
    cats = [c for c in cc if c["Name"] != CATEGORY]
    # своя категория — после «Артефакты», если такая есть, иначе в конец
    pos = next((i + 1 for i, c in enumerate(cats) if c["Name"] == "Артефакты"), len(cats))
    cats.insert(pos, category(ours))
    # самопроверка: чужие рецепты и категории не изменились
    a = [json.dumps(r, sort_keys=True, ensure_ascii=False) for r in cfg if r["RecipeId"] not in ours]
    b = [json.dumps(r, sort_keys=True, ensure_ascii=False) for r in full if r["RecipeId"] not in ours]
    ca = [json.dumps(c, sort_keys=True, ensure_ascii=False) for c in cc if c["Name"] != CATEGORY]
    cb = [json.dumps(c, sort_keys=True, ensure_ascii=False) for c in cats if c["Name"] != CATEGORY]
    return cfg, cc, full, cats, t1, t2, (a == b and ca == cb)


def read_text(path):
    raw = open(path, "rb").read()
    bom = raw.startswith(b"\xef\xbb\xbf")
    text = raw.decode("utf-8-sig")
    nl = "\r\n" if "\r\n" in text else "\n"
    return text.replace("\r\n", "\n"), nl, bom


def write_text(path, text, nl, bom):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    data = text.replace("\n", nl).encode("utf-8")
    with open(path, "wb") as f:
        f.write((b"\xef\xbb\xbf" if bom else b"") + data)


def doc_block():
    lines = ["\t<Category> " + DOC_CATEGORY]
    for cls, price, note in DOC:
        lines.append("\t\t%s,%s*,\t\t-1,\t\t\t%d\t\t// %s" % (cls, "\t" * max(1, 10 - (len(cls) + 1) // 4), price, note))
    return "\n".join(lines) + "\n"


def merged_doc(text):
    """Своя категория заменяется целиком; новая — перед «Скупка документов» (после трофеев мутантов)."""
    rx = re.compile(r"^[ \t]*<Category>[ \t]*" + re.escape(DOC_CATEGORY) + r"[^\n]*\n(?:(?![ \t]*<Category>)[^\n]*\n)*?"
                    r"(?=[ \t]*(?:<Category>|\n|$))", re.M)
    text = rx.sub("", text)
    m = re.search(r"^[ \t]*<Category>[ \t]*Скупка документов", text, re.M)
    if not m:
        raise SystemExit("в файле Дока нет категории «Скупка документов»")
    return text[:m.start()] + doc_block() + "\n" + text[m.start():]


# ---------------------------------------------------------------------------------------------- проверки
def validate(ix, R, live_cfg, doc_text):
    err = []
    ids = [r["RecipeId"] for r in R]
    if len(ids) != len(set(ids)) or not all(ID_MIN <= i <= ID_MAX for i in ids):
        err.append("Id рецептов не уникальны или вне %d–%d" % (ID_MIN, ID_MAX))
    taken = {r["RecipeId"]: r["RecipeName"] for r in live_cfg if r["RecipeId"] in ids
             and not str(r.get("Result", "")).startswith("VB_AR_")}
    for i, n in taken.items():
        err.append("Id %d на 18 занят чужим рецептом «%s»" % (i, n))
    for r in R:
        w = "%d «%s»" % (r["RecipeId"], r["RecipeName"])
        if not ix.has(r["Result"]):
            err.append(w + ": нет класса результата " + r["Result"])
        if len(r["Description"]) > DESC_MAX:
            err.append("%s: описание %d знаков > %d" % (w, len(r["Description"]), DESC_MAX))
        for c in r["CraftComponents"]:
            for n in c["Classnames"]:
                if not ix.has(n):
                    err.append("%s: нет класса %s" % (w, n))
    for cls, _, _ in DOC:
        if not ix.has(cls):
            err.append("Док: нет класса " + cls)
        if re.search(r"^[ \t]*" + cls + r"[ \t]*,", doc_text, re.M):
            err.append("Док: %s уже есть в файле вне категории «%s»" % (cls, DOC_CATEGORY))
    return err


def write_md(R, path):
    names = {}
    for r in R:
        names[r["Result"]] = r["RecipeName"]
    nice = dict(names)
    nice.update({ROOT: "отросток", VEIN: "лыко", SPORES: "пыльца", AMBER: "янтарь", HEART: "сердцевина",
                 DEAD: "окаменевшая сердцевина", FANG: "клык волка", "Rag": "тряпка", "Syringe": "шприц",
                 "VB_AR_AmberInclusion": "янтарь с вкраплением", "VB_AR_KeeperHeart": "Сердце Хранителя",
                 "MSFZ_Zanoza_stick": "хвост Занозы", "BurlapSack": "мешковина", "CaninaBerry": "шиповник",
                 "WaterBottle": "бутылка воды", "MSFZ_Spider_Lapka": "лапка паука", "Paper": "бумага",
                 "MSFZ_RuneStone_Day": "Руна Дня", "Raincoat_ColorBase": "дождевик", "EFT_Sewing": "набор для шитья",
                 "DRL_Craft_Sewing_scissors": "ножницы", "DRL_Craft_Metalic_thimble": "напёрсток"})
    lines = ["# Крафт «Древнего корня» на 18 (DRL) и скупка у Дока", "",
             "Сгенерировано `make_ar_craft18.py`. Категория «%s», Id %d–%d." % (CATEGORY, R[0]["RecipeId"], R[-1]["RecipeId"]),
             "", "| Id | Изделие | Станция | Из чего | Время, с |", "|---|---|---|---|---|"]
    for r in R:
        parts = []
        for c in r["CraftComponents"]:
            n = c["Classnames"]
            label = "нож" if n == KNIVES else ("хороший нож" if n == GOOD_KNIVES else nice.get(n[0], n[0]))
            if c["Delete"] == 0:
                label += " (инструмент)"
            parts.append(label + (" ×%d" % c["Amount"] if c["Amount"] > 1 else ""))
        st = "швейная" if r["WorkbenchClasses"] == SEWING else "верстак"
        if r["ShowInFreeMenu"]:
            st += ", и без станка"
        res = r["RecipeName"] + (" ×%d" % r["ResultCount"] if r["ResultCount"] > 1 else "")
        lines.append("| %d | %s | %s | %s | %d |" % (r["RecipeId"], res, st, ", ".join(parts), r["CraftTimeSec"]))
    lines += ["", "**Док, «%s»** (только скупка): " % DOC_CATEGORY.split(" |")[0]
              + "; ".join("%s — %d" % (nice.get(c, c), p) for c, p, _ in DOC) + ".",
              "", "Семя и лыко Док не берёт: они крафтятся, скупка дала бы бесконечные деньги.",
              "Разбор не возвращает пыльцу, сердцевину, окаменевшую сердцевину, янтарь и клык."]
    with io.open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines) + "\n")


def main():
    args = sys.argv[1:]
    R = recipes()
    ix = Index()
    cfg, cc, full, cats, t1, t2, same = merged_drl(LIVE_DRL, R)
    doc_text, nl, bom = read_text(LIVE_DOC)
    # проверка дублей в Доке — по тексту без своей категории
    rx_own = re.compile(r"^[ \t]*<Category>[ \t]*" + re.escape(DOC_CATEGORY) + r".*?(?=^[ \t]*<Category>|\Z)", re.M | re.S)
    err = validate(ix, R, cfg, rx_own.sub("", doc_text))
    if not same:
        err.append("сборка изменила чужие рецепты или категории")
    if err:
        print("ОШИБКИ (%d):" % len(err))
        print("\n".join("  " + e for e in err))
        sys.exit(1)
    doc_new = merged_doc(doc_text)
    dump_json(full, os.path.join(OUT, "DRL_CraftMenu", "CraftConfig.json"), t1)
    dump_json(cats, os.path.join(OUT, "DRL_CraftMenu", "CraftCategories.json"), t2)
    write_text(os.path.join(OUT, "Trader", "TraderConfig9_Doc.txt"), doc_new, nl, bom)
    write_md(R, os.path.join(HERE, "ar_craft_18.md"))
    print("рецептов %d (Id %d–%d), категория «%s»; на 18 станет рецептов %d (было %d), категорий %d (было %d); "
          "Док: скупка %d даров -> %s" % (len(R), R[0]["RecipeId"], R[-1]["RecipeId"], CATEGORY, len(full), len(cfg),
                                          len(cats), len(cc), len(DOC), OUT))
    if "--deploy" in args:
        stamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        bdir = os.path.join(BACKUP, stamp + "_srv18_ar_craft")
        os.makedirs(os.path.join(bdir, "DRL_CraftMenu"))
        os.makedirs(os.path.join(bdir, "Trader"))
        for fn in ("CraftConfig.json", "CraftCategories.json"):
            shutil.copy2(os.path.join(LIVE_DRL, fn), os.path.join(bdir, "DRL_CraftMenu", fn))
        shutil.copy2(LIVE_DOC, os.path.join(bdir, "Trader", os.path.basename(LIVE_DOC)))
        dump_json(full, os.path.join(LIVE_DRL, "CraftConfig.json"), t1)
        dump_json(cats, os.path.join(LIVE_DRL, "CraftCategories.json"), t2)
        write_text(LIVE_DOC, doc_new, nl, bom)
        for sub, fn in (("DRL_CraftMenu", "CraftConfig.json"), ("DRL_CraftMenu", "CraftCategories.json"),
                        ("Trader", "TraderConfig9_Doc.txt")):
            os.makedirs(os.path.join(READY, sub), exist_ok=True)
            shutil.copy2(os.path.join(INST18, sub, fn), os.path.join(READY, sub, fn))
        print("выложено на 18 (локальная копия) и в готовые моды; бэкап:", bdir)


if __name__ == "__main__":
    main()
