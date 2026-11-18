# -*- coding: utf-8 -*-
"""Рецепт «Велесовой кирки» в DRL сервера 18 (ТЗ_Велесова_кирка.md, раздел 4).

  python -I make_veles_craft.py            — проверить и показать, что изменится (файлы не трогает)
  python -I make_veles_craft.py --deploy   — бэкап и запись в DayZServer18\\Instance_1\\DRL_CraftMenu (локальная копия)
                                             и в «Обновлены или готовые моды\\18 сервер\\Instance_1\\DRL_CraftMenu»

Рецепт 303, категория «Инструменты» (рядом с Магической киркой 301). 8 тратящихся ингредиентов + молот и наковальня,
как у Магической кирки (не тратятся, чуть изнашиваются). Повторный запуск заменяет рецепт 303 целиком.
Формат файлов DRL: UTF-8 без BOM, CRLF, отступ 4; чужие рецепты остаются байт в байт (самопроверка).
"""
import datetime
import io
import json
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8")
BS = chr(92)
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", "..", ".."))
SRV_DRL = "D:" + BS + "DayZavr" + BS + "DayZavr-Servers" + BS + "DayZServer18" + BS + "Instance_1" + BS + "DRL_CraftMenu"
READY_DRL = os.path.join(REPO, "Обновлены или готовые моды", "18 сервер", "Instance_1", "DRL_CraftMenu")
BACKUP = os.path.join("G:" + BS, "DayZ_Test", "backup",
                      datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S") + "_srv18_veles_craft")

RECIPE_ID = 303
CATEGORY = "Инструменты"
RESULT = "VB_MR_Pickaxe_Veles"
NAME = "Велесова кирка"
DESC = "Магическая кирка, перекованная из титана и живого корня. Чует руду рядом, добывает быстрее и почти не тупится."

# (класс, количество, наследники засчитываются, шанс вернуть при разборе)
INGREDIENTS = [
    ("MSFZ_Magic_Kirka", 1, 0, 0.5),
    ("VB_MR_Titanium_Bar", 2, 1, 0.5),
    ("VB_MR_Gold_Bar", 1, 1, 0.5),
    ("VB_MR_Coal_Bar", 3, 1, 0.5),
    ("VB_AR_RootPiece", 3, 0, 0.5),
    ("VB_AR_Amber", 1, 0, 0.0),        # янтарь при разборе не возвращается (как в крафте Древнего корня)
    ("MSFZ_RuneStone_Medved", 1, 0, 0.5),
    ("MSFZ_Troll_Hand", 1, 0, 0.5),
]
# инструменты — как у Магической кирки (301): не тратятся, изнашиваются
TOOLS = [("MSFZ_Magic_Hammer", 0.2), ("DRL_Craft_Anvil", 0.02)]


def component(cls, amount, inherit, recraft, delete=1, damage=0.0):
    return {"Classname": cls, "Classnames": [cls], "EnableVariationSelection": 1, "MinHealthFrac": 3, "Amount": amount,
            "QuantityFrac": 0.0, "Delete": delete, "ApplyDamageFrac": damage, "RecraftDropChance": recraft,
            "RecraftAmount": 1, "RecraftQuantityFrac": 0.0, "UseInheritance": inherit, "AttachmentsEx": 0,
            "RecraftNeedAttachments": 0}


def recipe():
    comps = [component(c, a, i, r) for c, a, i, r in INGREDIENTS]
    comps += [component(c, 1, 0, 0.0, delete=0, damage=d) for c, d in TOOLS]
    return {"RecipeId": RECIPE_ID, "ShowInFreeMenu": 1, "WorkbenchClasses": ["DRL_Craft_Workbench"],
            "RecipeName": NAME, "Result": RESULT, "ResultClasses": [], "PreviewClassname": "", "CraftIcon": "",
            "Description": DESC, "CraftTimeSec": 20, "RecraftTimeSec": 20, "ResultCount": 1,
            "ResultQuantityFrac": 0.0, "ResultDamageFrac": 0.0, "CraftComponents": comps}


def dump(obj):
    return json.dumps(obj, ensure_ascii=False, indent=4).replace("\n", "\r\n")


def load(path):
    raw = io.open(path, "rb").read()
    assert not raw.startswith(b"\xef\xbb\xbf"), "у файла DRL неожиданно есть BOM: " + path
    text = raw.decode("utf-8")
    data = json.loads(text)
    # самопроверка формата: файл записан тем же json.dumps — тогда чужие рецепты останутся байт в байт
    assert dump(data) == text.rstrip("\r\n") or dump(data) + "\r\n" == text, "формат файла не как у json.dumps: " + path
    return data, text.endswith("\r\n")


def patch(folder, deploy):
    cfg_path = os.path.join(folder, "CraftConfig.json")
    cat_path = os.path.join(folder, "CraftCategories.json")
    cfg, cfg_nl = load(cfg_path)
    cats, cat_nl = load(cat_path)
    if len(DESC) > 170:
        raise SystemExit("описание длиннее 170 знаков")
    old = [r for r in cfg if r["RecipeId"] == RECIPE_ID]
    if old and old[0]["Result"] != RESULT:
        raise SystemExit("номер %d занят другим рецептом: %s" % (RECIPE_ID, old[0]["Result"]))
    new_cfg = [r for r in cfg if r["RecipeId"] != RECIPE_ID]
    pos = next((k + 1 for k, r in enumerate(new_cfg) if r["RecipeId"] == 301), len(new_cfg))
    new_cfg.insert(pos, recipe())
    cat = next(c for c in cats if c["Name"] == CATEGORY)
    if RECIPE_ID not in cat["List"]:
        cat["List"].append(RECIPE_ID)
    print("%s: рецепт %d %s, категория «%s»: %s" % (folder, RECIPE_ID, "заменён" if old else "добавлен", CATEGORY, cat["List"]))
    if not deploy:
        return
    for path in (cfg_path, cat_path):
        rel = os.path.relpath(path, os.path.dirname(os.path.dirname(folder)))
        dst = os.path.join(BACKUP, rel)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copy2(path, dst)
    io.open(cfg_path, "w", encoding="utf-8", newline="").write(dump(new_cfg) + ("\r\n" if cfg_nl else ""))
    io.open(cat_path, "w", encoding="utf-8", newline="").write(dump(cats) + ("\r\n" if cat_nl else ""))


deploy = "--deploy" in sys.argv
for folder in (SRV_DRL, READY_DRL):
    patch(folder, deploy)
if deploy:
    print("бэкап:", BACKUP)
