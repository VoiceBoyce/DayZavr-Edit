# -*- coding: utf-8 -*-
"""Лавка лагеря в крафте DRL сервера 18 (ТЗ: Квесты_18.md, раздел 5; образец — quests16/make_shop16.py).
Данные — shop_data18.py, позиции и старые цены — старые ветки покупок 18 (копия в source/old_shop18).

  python make_shop18.py            — собрать в quests18/out/DRL_CraftMenu + shop_18.md (живые файлы не трогает)
  python make_shop18.py --deploy   — то же + бэкап и выкладка в DayZServer18\\Instance_1\\DRL_CraftMenu
  python make_shop18.py --to <папка DRL_CraftMenu>  — выложить в другую папку (стенд), без бэкапа

Основа сборки — текущие CraftConfig.json и CraftCategories.json 18: добавляются или заменяются только рецепты лавки
(Id 5100–5999, описание «Лавка…») и категории «Лавка: …»; чужие рецепты и категории остаются как есть, формат файла
тоже (CRLF, отступ 4, UTF-8 без экранирования, перевод строки в конце — как в исходном файле).

Особенности DRL (проверено по коду на 16):
  - монеты-ингредиенты считаются по количеству в стопке; стакающийся результат — одна стопка ResultCount;
  - ResultQuantityFrac 0 у батарей/жидкостей даёт пустой предмет, поэтому у вещей 1.0 (магазины полные),
    0 только у патронов (там количество = ResultCount);
  - разбор (recraft) возвращает ингредиенты с RecraftDropChance — у всех 0, иначе «купил-разобрал» печатает деньги;
  - ингредиент с несколькими Classnames принимает любой из них («трофей сильного мутанта», «голова Энта»).
"""
import datetime
import io
import json
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import shop_data18 as SD  # noqa: E402
from classindex18 import Index  # noqa: E402

SRV18 = r"D:\DayZavr\DayZavr-Servers\DayZServer18"
LIVE = os.path.join(SRV18, "Instance_1", "DRL_CraftMenu")
LIVE_QUESTS = os.path.join(SRV18, "Instance_1", "INT_Mods", "INT_QUESTS", "Quests")
OUT = os.path.join(HERE, "out", "DRL_CraftMenu")
OLD = os.path.join(HERE, "source", "old_shop18")
BACKUP = r"G:\DayZ_Test\backup"
ID_MIN, ID_MAX = 5100, 5999
MARK = ("Лавка",)          # описание рецептов лавки начинается так — по нему узнаём свои рецепты прошлых выкладок
COINS = {"g": "DZVR_Coin_Gold", "s": "DZVR_Coin_Silver", "m": "DZVR_Coin_Copper"}
BRANCHES = ("COINS_TRADE_CLOTHES", "COINS_TRADE_WEAPONS", "COINS_TRADE_MATERIAL")


def coin(cls, n):
    return {"Classname": cls, "Classnames": [cls], "EnableVariationSelection": 1, "MinHealthFrac": 3, "Amount": n,
            "QuantityFrac": 0.0, "Delete": 1, "ApplyDamageFrac": 0.0, "RecraftDropChance": 0.0, "RecraftAmount": n,
            "RecraftQuantityFrac": 0.0, "UseInheritance": 0, "AttachmentsEx": 0, "RecraftNeedAttachments": 0}


def extras(pairs):
    """Ингредиенты кроме монет: [(класс | группа, сколько)] -> компоненты DRL (расходуются, разбор не возвращает)."""
    out = []
    for cls, n in pairs:
        if cls in SD.GROUPS:
            c = coin(SD.GROUPS[cls][0], n)
            c["Classnames"] = list(SD.GROUPS[cls])
        else:
            c = coin(cls, n)
        out.append(c)
    return out


def comp_key(c):
    """Компонент -> ключ для подписи: группа (по набору Classnames) или класс."""
    if len(c["Classnames"]) > 1:
        for g, lst in SD.GROUPS.items():
            if list(c["Classnames"]) == list(lst):
                return g
    return c["Classname"]


def extras_list(r):
    return [(comp_key(c), c["Amount"]) for c in r["CraftComponents"] if not c["Classname"].startswith("DZVR_Coin_")]


def extras_text(r):
    return ", ".join(SD.EXTRA_RU.get(k, k) + (" ×%d" % n if n > 1 else "") for k, n in extras_list(r))


def split_coins(gold_total):
    """Цена в золоте (1.53 = 1 з 5 с 3 м при курсе 10:1) -> (золото, серебро, медь)."""
    c = SD.to_copper(gold_total)
    g, rest = divmod(c, SD.RATE * SD.RATE)
    s, m = divmod(rest, SD.RATE)
    return g, s, m


def price_comps(gold_total):
    g, s, m = split_coins(gold_total)
    out = []
    if g:
        out.append(coin(COINS["g"], g))
    if s:
        out.append(coin(COINS["s"], s))
    if m:
        out.append(coin(COINS["m"], m))
    return out


def price_text(gold_total):
    g, s, m = split_coins(gold_total)
    parts = []
    if g:
        parts.append("%d з" % g)
    if s:
        parts.append("%d с" % s)
    if m:
        parts.append("%d м" % m)
    return " ".join(parts) or "0"


def recipe(rid, name, wb, variants, comps, desc, count=1, frac=1.0, attached=None, sec=3):
    r = {"RecipeId": rid, "ShowInFreeMenu": 0, "WorkbenchClasses": [wb], "RecipeName": name, "Result": variants[0],
         "ResultClasses": list(variants) if len(variants) > 1 else [], "PreviewClassname": "", "CraftIcon": "",
         "Description": desc, "CraftTimeSec": sec, "RecraftTimeSec": sec, "ResultCount": count,
         "ResultQuantityFrac": frac, "ResultDamageFrac": 0.0, "CraftComponents": comps}
    if attached:
        r["AttachedRecipeIds"] = list(attached)
    return r


def old_quests(branch):
    """Квесты старой ветки покупок (копия в source/old_shop18; если её нет — с сервера, в т.ч. из _old_*)."""
    d = os.path.join(OLD, branch)
    if not os.path.isdir(d):
        d = os.path.join(LIVE_QUESTS, branch)
        if not os.path.isdir(d):
            d = os.path.join(LIVE_QUESTS, "_old_" + branch)
    out = []
    for f in sorted(os.listdir(d)):
        if f.endswith(".json") and f != "_branch.json":
            q = json.load(io.open(os.path.join(d, f), encoding="utf-8-sig"))
            if q.get("RepeatMode") != "ONCE":       # «Начало» (WD-40 за вход) — не покупка
                out.append(q)
    return out


def gold_price(q):
    g = [t for t in q["Targets"] if t["Target"] == "DZVR_Coin_Gold"]
    return int(g[0]["Count"]) if g else 0


def rewards(q):
    out = {}
    for r in q["Rewards"]:
        out[r["Item"]] = out.get(r["Item"], 0) + int(r.get("Count", 1))
    return out


def mag_rounds(cls):
    m = re.search(r"(\d+)\s*r(?:n)?d", cls, re.I)
    return int(m.group(1)) if m else 0


def fix_class(cls):
    """Опечатки старых квестов (лишняя точка в «kr_swatsuit_combatShirt_multicam.»)."""
    return cls.rstrip(".")


class Shop:
    def __init__(self):
        self.recipes, self.rows = [], []
        self.cats = {k: [] for k in SD.CATEGORIES}
        self.station = {}      # Id -> ожидаемая станция
        self.price = {}        # Id -> цена в золоте
        self.sold = set()      # все классы-результаты

    def put(self, cat, r, gold, note="", wb=None):
        self.recipes.append(r)
        self.cats[cat].append(r["RecipeId"])
        self.station[r["RecipeId"]] = wb or r["WorkbenchClasses"][0]
        self.price[r["RecipeId"]] = gold
        self.sold.update([r["Result"]] + r["ResultClasses"])
        coins = sum(c["Amount"] for c in r["CraftComponents"] if c["Classname"].startswith("DZVR_Coin_"))
        st = "швейная" if r["WorkbenchClasses"][0] == SD.SEW else "верстак"
        self.rows.append((SD.CATEGORIES[cat][0], r["RecipeId"], r["RecipeName"], st, len(r["ResultClasses"]) or 1,
                          price_text(gold) if gold else "", coins, extras_text(r), note))


def build():
    shop = Shop()
    put = shop.put
    # одежда
    for rid, name, variants, g in SD.CLOTHES:
        desc = "Лавка лагеря." + (" Выберите расцветку." if len(variants) > 1 else "")
        put(SD.clothes_category(rid), recipe(rid, name, SD.SEW, variants,
                                             price_comps(g) + extras(SD.CLOTHES_EXTRA.get(rid, [])), desc), g, wb=SD.SEW)
    # снаряжение (сервопривод и модули OOTS — верстак)
    for rid, name, cls, g in SD.GEAR:
        put(SD.clothes_category(rid), recipe(rid, name, SD.WB, [cls],
                                             price_comps(g) + extras(SD.GEAR_EXTRA.get(rid, [])),
                                             SD.GEAR_DESC.get(rid, "Лавка лагеря.")), g, wb=SD.WB)

    # оружие и обвес
    addon_ids, addon_price, next_addon = {}, {}, [5400]

    def addon(cls, n, weapon_name):
        """Навесной рецепт для класса (n — сколько штук в пачке для патронов). Возвращает (id, цена одной покупки)."""
        key = (cls, n if cls in SD.AMMO else 1)
        if key in addon_ids:
            return addon_ids[key], addon_price[key]
        rid = next_addon[0]
        next_addon[0] += 1
        if cls in SD.AMMO:
            nm, each = SD.AMMO[cls]
            p = each * n
            r = recipe(rid, "%s ×%d" % (nm, n), SD.WB, [cls], price_comps(p), "Лавка лагеря: пачка %d шт." % n,
                       count=n, frac=0.0)
        elif cls in SD.ADDONS:
            nm, p = SD.ADDONS[cls]
            r = recipe(rid, nm, SD.WB, [cls], price_comps(p) + extras(SD.addon_extra(cls, p)), "Лавка лагеря.")
        else:  # магазин
            rounds = mag_rounds(cls)
            p = SD.MAG_OVERRIDE.get(cls, SD.MAG_BIG_PRICE if rounds >= SD.MAG_BIG_ROUNDS else SD.MAG_PRICE)
            nm = "Магазин %s" % weapon_name + (" (%d)" % rounds if rounds else "")
            r = recipe(rid, nm, SD.WB, [cls], price_comps(p), "Лавка лагеря. Магазин полный.")
        addon_ids[key], addon_price[key] = rid, p
        put("addons", r, p, wb=SD.WB)
        return rid, p

    for q in old_quests("COINS_TRADE_WEAPONS"):
        old_kit, rw = gold_price(q), rewards(q)
        kit = SD.from_old_gold(old_kit)
        weapons = [c for c in rw if c in SD.WEAPON_NAMES]
        if not weapons:  # пачка патронов Gauss
            (cls, n), = rw.items()
            addon(cls, n, "")
            continue
        wcls = weapons[0]
        wname = SD.WEAPON_NAMES[wcls]
        att, spent = [], 0
        for cls, n in rw.items():
            if cls == wcls:
                continue
            if cls in SD.AMMO:
                aid, p = addon(cls, n, wname)
                spent += p
            else:
                aid, p = addon(cls, 1, wname)
                spent += p * n
            att.append(aid)
        wp = SD.nice(max(kit - spent, kit * SD.WEAPON_MIN_SHARE))
        rid = 5300 + (q["Id"] - 18301)
        desc = ("Лавка лагеря. Ствол без обвеса. Ниже отметьте магазины и обвес из прежнего набора — встанут на оружие "
                "или лягут рядом. Набор целиком: %s." % price_text(wp + spent))
        put("weapons", recipe(rid, wname, SD.WB, [wcls], price_comps(wp) + extras(SD.weapon_extra(wcls, kit)), desc,
                              attached=att, sec=5), wp,
            "набор %s (старый %d з)" % (price_text(wp + spent), old_kit), wb=SD.WB)

    # для базы
    groups = {g[0]: [] for g in SD.BASE_GROUPS}
    gprice = {}
    for q in old_quests("COINS_TRADE_MATERIAL"):
        (cls, _), = rewards(q).items()
        for gid, _, cond in SD.BASE_GROUPS:
            if cond(cls):
                groups[gid].append(cls)
                gprice.setdefault(gid, set()).add(gold_price(q))
                break
        else:
            raise SystemExit("для базы: класс без группы " + cls)
    for gid, name, _ in SD.BASE_GROUPS:
        if not groups[gid]:
            continue
        if len(gprice[gid]) != 1:
            raise SystemExit("для базы: в группе «%s» разные старые цены %s" % (name, sorted(gprice[gid])))
        g = SD.from_old_gold(gprice[gid].pop())
        desc = "Лавка лагеря." + (" Выберите вариант (%d)." % len(groups[gid]) if len(groups[gid]) > 1 else "")
        put("base", recipe(gid, name, SD.WB, groups[gid], price_comps(g), desc), g, wb=SD.WB)
    return shop


def clothes_sets():
    """Сверка: старые наборы одежды ÷ 10 против суммы цен вещей в лавке (вещи, общие для сетов, стоят одинаково,
    поэтому у части сетов сумма немного отличается — так же на 16)."""
    price = {}
    for rid, name, variants, g in SD.CLOTHES:
        for v in variants:
            price[v] = g
    for rid, name, cls, g in SD.GEAR:
        price[cls] = g
    out = []
    for q in old_quests("COINS_TRADE_CLOTHES"):
        rw = rewards(q)
        total, dropped = 0.0, []
        for cls, n in rw.items():
            cls = fix_class(cls)
            if cls in price:
                total += price[cls] * n
            else:
                dropped.append("%s×%d" % (cls, n) if n > 1 else cls)
        if len(rw) > 1:
            out.append((q["Id"], SD.from_old_gold(gold_price(q)), SD.nice(total), ", ".join(dropped)))
    return out


def old_classes():
    """Все классы-результаты старых веток покупок (с исправленными опечатками)."""
    out = set()
    for b in BRANCHES:
        for q in old_quests(b):
            out.update(fix_class(c) for c in rewards(q))
    return out


def load_quests_data():
    """quests18/quests_data.py (новые квесты 18), если он уже есть. None — файла нет (проверка пропускается)."""
    path = os.path.join(HERE, "quests_data.py")
    if not os.path.isfile(path):
        return None
    import importlib
    return importlib.import_module("quests_data")


def validate(ix, shop, live, live_cats):
    recipes = shop.recipes
    err, warn, info, seen = [], [], [], set()
    ids = {r["RecipeId"] for r in recipes}
    old = old_classes()
    only_old = set()
    for r in recipes:
        rid = r["RecipeId"]
        if rid in seen:
            err.append("Id %d повторяется" % rid)
        seen.add(rid)
        if not ID_MIN <= rid <= ID_MAX:
            err.append("Id %d вне %d–%d" % (rid, ID_MIN, ID_MAX))
        if r["ShowInFreeMenu"] != 0:
            err.append("Id %d: виден в свободном меню" % rid)
        if r["WorkbenchClasses"] != [shop.station[rid]]:
            err.append("Id %d: станция %s, ожидалась %s" % (rid, r["WorkbenchClasses"], shop.station[rid]))
        for cls in [r["Result"]] + r["ResultClasses"] + [c for comp in r["CraftComponents"] for c in comp["Classnames"]]:
            if cls in SD.UNVERIFIED:
                warn.append("Id %d: класс %s исправлен при переносе (в старом квесте с точкой) — проверить в игре"
                            % (rid, cls))
            elif not ix.has(cls):
                if cls in old:
                    only_old.add(cls)
                else:
                    err.append("Id %d «%s»: нет класса %s" % (rid, r["RecipeName"], cls))
        if not any(c["Classname"].startswith("DZVR_Coin_") for c in r["CraftComponents"]):
            err.append("Id %d: рецепт без монет" % rid)
        if len(r["CraftComponents"]) > SD.MAX_INGREDIENTS:
            err.append("Id %d «%s»: ингредиентов %d, больше %d" % (rid, r["RecipeName"], len(r["CraftComponents"]),
                                                                SD.MAX_INGREDIENTS))
        for comp in r["CraftComponents"]:
            if comp["RecraftDropChance"] != 0.0:
                err.append("Id %d: разбор возвращает %s" % (rid, comp["Classname"]))
        for a in r.get("AttachedRecipeIds", []):
            if a not in ids:
                err.append("Id %d: навес %d не найден" % (rid, a))
    shop.only_old = sorted(only_old)
    if only_old:
        warn.append("%d классов подтверждены только старыми покупками 18 (выдавались наградой; в ванили, модах "
                    "репозитория, других квестах, крафте, торговцах и types 18 их нет; чужие PBO не смотрим) — "
                    "список в shop_18.md" % len(only_old))
    # станции существуют
    for wb in (SD.WB, SD.SEW):
        if not ix.has(wb):
            err.append("нет станции %s" % wb)
    # каждый рецепт ровно в одной категории
    incat = [i for v in shop.cats.values() for i in v]
    for rid in ids:
        if incat.count(rid) != 1:
            err.append("Id %d в %d категориях" % (rid, incat.count(rid)))
    # чужие Id и категории не трогаем
    for lr in live:
        lid = lr["RecipeId"]
        theirs = not str(lr.get("Description", "")).startswith(MARK)
        if theirs and (lid in ids or ID_MIN <= lid <= ID_MAX):
            err.append("Id %d «%s» на 18 занят чужим рецептом" % (lid, lr.get("RecipeName", "")))
    names = {v[0] for v in SD.CATEGORIES.values()}
    for c in live_cats:
        if c["Name"].startswith(SD.CATEGORY_PREFIX) and c["Name"] not in names | SD.OBSOLETE_CATEGORIES:
            err.append("на 18 есть чужая категория «%s» — снять или переименовать вручную" % c["Name"])
        if c["Name"] in names:
            alien = [i for i in c["List"] if not ID_MIN <= i <= ID_MAX]
            if alien:
                err.append("в категории «%s» на 18 чужие рецепты %s" % (c["Name"], alien))
    # все позиции старых веток разобраны (продаются или сознательно убраны)
    lost = sorted(c for c in old if c not in shop.sold and c not in SD.DROPPED)
    for c in lost:
        err.append("класс старых покупок %s не продаётся и не в DROPPED" % c)
    # перепродажа: лавка не продаёт то, что квесты 18 просят принести или показать
    QD, qd_broken = None, False
    try:
        QD = load_quests_data()
    except Exception as e:  # файл есть, но не грузится — проверить нельзя
        qd_broken = True
        err.append("quests_data.py не загрузился (%s: %s) — проверка перепродажи невозможна" % (type(e).__name__, e))
    if QD is None and not qd_broken:
        warn.append("quests18/quests_data.py нет — проверка «лавка не продаёт то, что просят квесты» пропущена")
    if QD is not None:
        asked = {}
        given = {}
        for q in QD.QUESTS:
            for t in q.Targets:
                if t["Kind"] in ("DELIVER", "SEARCH"):
                    asked.setdefault(t["Target"].lower(), q.Name)
            for rw in q.Rewards:
                given.setdefault(rw.get("Item", "").lower(), q.Name)
        for r in recipes:
            for cls in sorted(set([r["Result"]] + r["ResultClasses"])):
                if cls.lower() in asked:
                    err.append("Id %d продаёт %s, а квест «%s» его просит — перепродажа"
                               % (r["RecipeId"], cls, asked[cls.lower()]))
                if cls.lower() in given:
                    info.append("квест «%s» даёт наградой %s — его же продаёт лавка (Id %d, %s)"
                                % (given[cls.lower()], cls, r["RecipeId"], price_text(shop.price[r["RecipeId"]])))
        info.append("перепродажа проверена по quests_data.py: квестов %d, «принести/показать» классов %d"
                    % (len(QD.QUESTS), len(asked)))
    return err, warn, info


def category(key, ids):
    name, icon, sound = SD.CATEGORIES[key]
    return {"Name": name, "Icon": icon, "ShowAll": 0, "UseList": 1, "ShowOnlyAvailableCraft": 0,
            "ShowOnlyAvailableStudy": 0, "CategoryFonImagePath": "set:drl_craft_work image:work",
            "CategoryCraftFonImagePath": "set:drl_craft_recipemanust image:recipemanust",
            "CategoryCraftSoundSet": sound, "List": sorted(ids)}


def read_json(path):
    raw = open(path, "rb").read()
    tail = b""
    for t in (b"\r\n", b"\n"):
        if raw.endswith(t):
            tail = t
            break
    return json.loads(raw.decode("utf-8-sig")), tail


def dump(obj, path, tail=b""):
    """Формат DRL-файлов 18: UTF-8 без BOM, CRLF, отступ 4, кириллица как есть; перевод строки в конце — как было."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    data = json.dumps(obj, ensure_ascii=False, indent=4).replace("\n", "\r\n").encode("utf-8") + tail
    with open(path, "wb") as f:
        f.write(data)


def is_ours(r, ours):
    return r["RecipeId"] in ours or (ID_MIN <= r["RecipeId"] <= ID_MAX and str(r.get("Description", "")).startswith(MARK))


def merged(src_dir, shop):
    """Текущие файлы папки + лавка: свои рецепты и категории заменяются, чужие остаются как были (и на своём месте)."""
    (cfg, t1), (cc, t2) = read_json(os.path.join(src_dir, "CraftConfig.json")), \
        read_json(os.path.join(src_dir, "CraftCategories.json"))
    ours = {r["RecipeId"] for r in shop.recipes}
    names = {v[0] for v in SD.CATEGORIES.values()} | SD.OBSOLETE_CATEGORIES
    full = [r for r in cfg if not is_ours(r, ours)] + sorted(shop.recipes, key=lambda r: r["RecipeId"])
    cats2 = [c for c in cc if c["Name"] not in names] + [category(k, v) for k, v in shop.cats.items() if v]
    return cfg, cc, full, cats2, t1, t2


def self_check(cfg, cc, full, cats2, shop):
    """Чужие рецепты и категории в результате байт в байт те же, что в исходнике."""
    ours = {r["RecipeId"] for r in shop.recipes}
    a = [json.dumps(r, ensure_ascii=False, sort_keys=True) for r in cfg if not is_ours(r, ours)]
    b = [json.dumps(r, ensure_ascii=False, sort_keys=True) for r in full if r["RecipeId"] not in ours]
    names = {v[0] for v in SD.CATEGORIES.values()} | SD.OBSOLETE_CATEGORIES
    ca = [json.dumps(c, ensure_ascii=False, sort_keys=True) for c in cc if c["Name"] not in names]
    cb = [json.dumps(c, ensure_ascii=False, sort_keys=True) for c in cats2 if c["Name"] not in names]
    return a == b and ca == cb


def tiers(shop):
    """Сводка цен по ступеням (как в ТЗ 16, раздел 2а)."""
    by = {}
    for r in shop.recipes:
        g = shop.price[r["RecipeId"]]
        ex = bool(extras_list(r))
        if g < 0.1:
            k = "медь (до 9 м)"
        elif g < 1:
            k = "серебро (1–9 с)"
        elif g < 15:
            k = "1–15 з"
        elif g < 25:
            k = "15–25 з"
        else:
            k = "от 25 з"
        b = by.setdefault(k, [0, 0, 99, 0, 0])
        b[0] += 1
        b[1] += ex
        b[2] = min(b[2], g)
        b[3] = max(b[3], g)
        coins = sum(c["Amount"] for c in r["CraftComponents"] if c["Classname"].startswith("DZVR_Coin_"))
        b[4] = max(b[4], coins)
    order = ["медь (до 9 м)", "серебро (1–9 с)", "1–15 з", "15–25 з", "от 25 з"]
    return [(k,) + tuple(by[k]) for k in order if k in by]


def write_md(shop, err_free_info):
    lines = ["# Лавка лагеря (крафт DRL сервера 18) — таблица на проверку", "",
             "Сгенерировано `make_shop18.py` из `shop_data18.py`. Одежда — швейная машинка, остальное — верстак. "
             "Экономика v2.0, как на 16: курс 1 з = %d с = %d м, цены = старые покупки 18 ÷ %d (старые ветки "
             "COINS_TRADE_* те же, что были на 16, поэтому цены и Id совпадают с лавкой 16). «Монет» — сколько "
             "монет-предметов отдать. «Кроме монет» — вместо материалов аномалий и трофеев боссов 16: EFT и трофеи "
             "мутантов; «трофей сильного мутанта» — любой из: рука Тролля, челюсть Шума, глаз Теневика, голова "
             "сильного Энта." % (SD.RATE, SD.RATE * SD.RATE, SD.OLD_DIV), "",
             "## Ступени", "", "| Цена вещи | Рецептов | Из них «кроме монет» | Цены | Монет до |", "|---|---|---|---|---|"]
    for k, n, ex, lo, hi, mc in tiers(shop):
        lines.append("| %s | %d | %d | %s – %s | %d |" % (k, n, ex, price_text(lo), price_text(hi), mc))
    lines += ["", "## Позиции", "",
              "| Категория | Id | Позиция | Станция | Вариантов | Цена | Монет | Кроме монет | Примечание |",
              "|---|---|---|---|---|---|---|---|---|"]
    lines += ["| %s | %d | %s | %s | %d | %s | %d | %s | %s |" % r for r in shop.rows]
    sets = clothes_sets()
    diff = [SD.nice(was - now) for _, was, now, dropped in sets if not dropped and was != now]
    lines += ["", "## Сверка наборов одежды (старый набор ÷ 10 и сумма вещей в лавке)", "",
              "Вещи, общие для разных сетов (шлем, жилет, ботинки SWAT), стоят одинаково, поэтому у %d сетов сумма "
              "вещей меньше старой цены набора на %s – %s (сеты без шлема или противогаза) — так же на 16."
              % (len(diff), price_text(min(diff)), price_text(max(diff))) if diff else "Все суммы совпадают.", "",
              "| Старый квест | Набор ÷ 10 | Сумма вещей | Не продаётся (как на 16) |", "|---|---|---|---|"]
    for qid, was, now, dropped in sets:
        lines.append("| %d | %s | %s | %s |" % (qid, price_text(was), price_text(now), dropped))
    if err_free_info:
        lines += ["", "## К сведению", ""] + ["- " + i for i in err_free_info]
    if getattr(shop, "only_old", None):
        by = {}
        for c in shop.only_old:
            by.setdefault(c.split("_")[0] + "_", []).append(c)
        lines += ["", "## Классы, подтверждённые только старыми покупками 18 (%d)" % len(shop.only_old), "",
                  "Выдавались наградой в старых ветках COINS_TRADE_* на проде 18, но больше нигде в конфигах 18 "
                  "(квесты, крафт, торговцы, types), в ванили и в модах репозитория их нет. Чужие PBO по правилу не "
                  "смотрим. На 16 те же классы работают в лавке. Проверить в игре выборочно.", ""]
        for k in sorted(by):
            lines.append("- `%s*` (%d): %s" % (k, len(by[k]), ", ".join(by[k])))
    with io.open(os.path.join(HERE, "shop_18.md"), "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines) + "\n")


def main():
    args = sys.argv[1:]
    shop = build()
    ix = Index()
    cfg, cc, full, cats2, t1, t2 = merged(LIVE, shop)
    err, warn, info = validate(ix, shop, cfg, cc)
    if not self_check(cfg, cc, full, cats2, shop):
        err.append("сборка изменила чужие рецепты или категории")
    for w in warn:
        print("ВНИМАНИЕ:", w)
    if err:
        print("ОШИБКИ (%d):" % len(err))
        print("\n".join("  " + e for e in err))
        sys.exit(1)
    for i in info:
        print("к сведению:", i)
    dump(full, os.path.join(OUT, "CraftConfig.json"), t1)
    dump(cats2, os.path.join(OUT, "CraftCategories.json"), t2)
    write_md(shop, info)
    print("рецептов лавки %d (Id %d–%d), категорий лавки %d: %s"
          % (len(shop.recipes), min(r["RecipeId"] for r in shop.recipes), max(r["RecipeId"] for r in shop.recipes),
             sum(1 for v in shop.cats.values() if v),
             ", ".join("%s %d" % (SD.CATEGORIES[k][0][len(SD.CATEGORY_PREFIX):], len(v)) for k, v in shop.cats.items())))
    print("на 18 станет рецептов %d (было %d), категорий %d (было %d) -> %s" % (len(full), len(cfg), len(cats2), len(cc), OUT))
    if "--to" in args:
        dst = args[args.index("--to") + 1]
        _, _, f2, c2, d1, d2 = merged(dst, shop)
        dump(f2, os.path.join(dst, "CraftConfig.json"), d1)
        dump(c2, os.path.join(dst, "CraftCategories.json"), d2)
        print("записано в", dst)
    if "--deploy" in args:
        stamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        bdir = os.path.join(BACKUP, stamp + "_srv18_shop_DRL")
        os.makedirs(bdir)
        for fn in ("CraftConfig.json", "CraftCategories.json"):
            shutil.copy2(os.path.join(LIVE, fn), os.path.join(bdir, fn))
        dump(full, os.path.join(LIVE, "CraftConfig.json"), t1)
        dump(cats2, os.path.join(LIVE, "CraftCategories.json"), t2)
        print("выложено на 18 (локальная копия); бэкап:", bdir)
        print("не забыть: копия обоих файлов в «Обновлены или готовые моды\\18 сервер\\Instance_1\\DRL_CraftMenu\\»")


if __name__ == "__main__":
    main()
