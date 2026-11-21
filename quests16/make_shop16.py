# -*- coding: utf-8 -*-
"""Лавка лагеря и обмен монет в крафте DRL сервера 16 (ТЗ: Квесты_16.md, раздел 4.8). Данные — shop_data.py,
цены и состав наборов — старые квесты магазина в source/old_shop.

  python make_shop16.py            — собрать в quests16/out/DRL_CraftMenu + shop_16.md (живые файлы не трогает)
  python make_shop16.py --deploy   — то же + бэкап и выкладка в DayZServer16\\Instance_1\\DRL_CraftMenu
  python make_shop16.py --to <папка DRL_CraftMenu>  — выложить в другую папку (стенд), без бэкапа

Свои рецепты — Id 5000–5999, остальные рецепты 16 не трогает. Особенности DRL (проверено по коду):
  - монеты-ингредиенты считаются по количеству в стопке; стакающийся результат — одна стопка ResultCount;
  - ResultQuantityFrac 0 у батарей/жидкостей даёт пустой предмет, поэтому у вещей 1.0 (магазины полные),
    0 только у монет и патронов (там количество = ResultCount);
  - разбор (recraft) возвращает ингредиенты с RecraftDropChance — у монет 0, иначе «купил-разобрал» печатает деньги.
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
import shop_data as SD  # noqa: E402
from classindex import Index, SRV16  # noqa: E402

LIVE = os.path.join(SRV16, "DRL_CraftMenu")
OUT = os.path.join(HERE, "out", "DRL_CraftMenu")
OLD = os.path.join(HERE, "source", "old_shop")
BACKUP = r"G:\DayZ_Test\backup"
ID_MIN, ID_MAX = 5000, 5999
COINS = {"g": "DZVR_Coin_Gold", "s": "DZVR_Coin_Silver", "m": "DZVR_Coin_Copper"}


def coin(cls, n):
    return {"Classname": cls, "Classnames": [cls], "EnableVariationSelection": 1, "MinHealthFrac": 3, "Amount": n,
            "QuantityFrac": 0.0, "Delete": 1, "ApplyDamageFrac": 0.0, "RecraftDropChance": 0.0, "RecraftAmount": n,
            "RecraftQuantityFrac": 0.0, "UseInheritance": 0, "AttachmentsEx": 0, "RecraftNeedAttachments": 0}


def extras(pairs):
    """Ингредиенты кроме монет: [(класс | TROPHY, сколько)] -> компоненты DRL (расходуются, разбор не возвращает)."""
    out = []
    for cls, n in pairs:
        c = coin(SD.TROPHY_CLASSES[0] if cls == SD.TROPHY else cls, n)
        if cls == SD.TROPHY:
            c["Classnames"] = list(SD.TROPHY_CLASSES)
        out.append(c)
    return out


def extras_text(r):
    parts = []
    for c in r["CraftComponents"]:
        if c["Classname"].startswith("DZVR_Coin_"):
            continue
        key = SD.TROPHY if len(c["Classnames"]) > 1 and c["Classname"] in SD.TROPHY_CLASSES else c["Classname"]
        parts.append(SD.EXTRA_RU.get(key, key) + (" ×%d" % c["Amount"] if c["Amount"] > 1 else ""))
    return ", ".join(parts)


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
    d = os.path.join(OLD, branch)
    out = []
    for f in sorted(os.listdir(d)):
        if f.endswith(".json") and f != "_branch.json":
            q = json.load(io.open(os.path.join(d, f), encoding="utf-8-sig"))
            if q.get("RepeatMode") != "ONCE":
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


def build():
    recipes, cats, rows = [], {k: [] for k in SD.CATEGORIES}, []

    def put(cat, r, price_silver, note=""):
        recipes.append(r)
        cats[cat].append(r["RecipeId"])
        coins = sum(c["Amount"] for c in r["CraftComponents"] if c["Classname"].startswith("DZVR_Coin_"))
        rows.append((SD.CATEGORIES[cat][0], r["RecipeId"], r["RecipeName"], len(r["ResultClasses"]) or 1,
                     price_text(price_silver) if price_silver else "", coins, extras_text(r), note))

    # обмен
    for rid, name, (cls, n), res, cnt in SD.EXCHANGE:
        put("exchange", recipe(rid, name, SD.WB, [res], [coin(cls, n)],
                               "Курс лагеря: 10 медных = 1 серебряная, 10 серебряных = 1 золотая.",
                               count=cnt, frac=0.0, sec=2), 0, "%s ×%d → %s ×%d" % (cls[10:], n, res[10:], cnt))
    # одежда (цены в серебре)
    for rid, name, variants, s in SD.CLOTHES:
        desc = ""  # расцветки видно по вариантам, подсказку «Выберите расцветку.» убрали 2026-11-21
        put(SD.clothes_category(rid), recipe(rid, name, SD.SEW, variants,
                                             price_comps(s) + extras(SD.CLOTHES_EXTRA.get(rid, [])), desc), s)
    # снаряжение (сервопривод — к экзоскелету, модули — своя категория)
    for rid, name, cls, s in SD.GEAR:
        put(SD.clothes_category(rid), recipe(rid, name, SD.WB, [cls],
                                             price_comps(s) + extras(SD.GEAR_EXTRA.get(rid, [])),
                                             SD.GEAR_DESC.get(rid, "")), s)
    if SD.RECHARGE:
        rid, name, cls, s = SD.RECHARGE
        batt = coin(cls, 1)
        put("exo", recipe(rid, name, SD.WB, [cls], [batt] + price_comps(s),
                           "Старая батарея с любым зарядом + плата — полная батарея."), s, "+ старая батарея")

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
            r = recipe(rid, "%s ×%d" % (nm, n), SD.WB, [cls], price_comps(p), "Пачка %d шт." % n,
                       count=n, frac=0.0)
        elif cls in SD.ADDONS:
            nm, p = SD.ADDONS[cls]
            r = recipe(rid, nm, SD.WB, [cls], price_comps(p) + extras(SD.addon_extra(cls, p)), "")
        else:  # магазин
            rounds = mag_rounds(cls)
            p = SD.MAG_OVERRIDE.get(cls, SD.MAG_BIG_PRICE if rounds >= SD.MAG_BIG_ROUNDS else SD.MAG_PRICE)
            nm = "Магазин %s" % weapon_name + (" (%d)" % rounds if rounds else "")
            r = recipe(rid, nm, SD.WB, [cls], price_comps(p) + extras(SD.mag_extra(rounds)), "")  # «Магазин полный.» убрали 2026-11-21
        addon_ids[key], addon_price[key] = rid, p
        put("addons", r, p)
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
        # Цену «Набор целиком: …» из описания убрали 2026-11-21 — она и так видна в окне.
        desc = ("Ствол без обвеса. Ниже отметьте магазины и обвес из прежнего набора — встанут на оружие "
                "или лягут рядом.")
        put("weapons", recipe(rid, wname, SD.WB, [wcls], price_comps(wp) + extras(SD.weapon_extra(wcls, kit)), desc,
                              attached=att, sec=5), wp,
            "набор %s (старый %d з)" % (price_text(wp + spent), old_kit))

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
        assert len(gprice[gid]) == 1, (name, gprice[gid])
        s = SD.from_old_gold(gprice[gid].pop())
        desc = "Выберите вариант (%d)." % len(groups[gid]) if len(groups[gid]) > 1 else ""
        put("base", recipe(gid, name, SD.WB, groups[gid], price_comps(s) + extras(SD.BASE_EXTRA.get(gid, [])), desc), s)
    return recipes, cats, rows


def validate(ix, recipes, live):
    err, warn, seen = [], [], set()
    live_ids = {r["RecipeId"]: r for r in live}
    for r in recipes:
        rid = r["RecipeId"]
        if rid in seen:
            err.append("Id %d повторяется" % rid)
        seen.add(rid)
        if not ID_MIN <= rid <= ID_MAX:
            err.append("Id %d вне 5000–5999" % rid)
        for cls in [r["Result"]] + r["ResultClasses"] + [c for comp in r["CraftComponents"] for c in comp["Classnames"]]:
            if cls in SD.UNVERIFIED:
                warn.append("Id %d: класс %s не проверен (чужой мод) — проверить в игре" % (rid, cls))
            elif not ix.has(cls):
                err.append("Id %d «%s»: нет класса %s" % (rid, r["RecipeName"], cls))
        if not r["CraftComponents"]:
            err.append("Id %d: бесплатный рецепт" % rid)
        if len(r["CraftComponents"]) > SD.MAX_INGREDIENTS:
            err.append("Id %d «%s»: ингредиентов %d, больше %d" % (rid, r["RecipeName"], len(r["CraftComponents"]),
                                                                SD.MAX_INGREDIENTS))
        for comp in r["CraftComponents"]:
            if comp["RecraftDropChance"] != 0.0:
                err.append("Id %d: разбор возвращает %s" % (rid, comp["Classname"]))
        for comp in r["CraftComponents"]:
            if comp["Classname"].startswith("DZVR_Coin_") and comp["RecraftDropChance"] != 0.0:
                err.append("Id %d: разбор возвращает монеты" % rid)
        for a in r.get("AttachedRecipeIds", []):
            if a not in seen and a not in {x["RecipeId"] for x in recipes}:
                err.append("Id %d: навес %d не найден" % (rid, a))
    # Свой рецепт прошлых выкладок — стоит в категории лавки. По описанию «Лавка лагеря…» узнавать больше нельзя:
    # с 2026-11-21 этих слов в описаниях нет.
    names = {v[0] for v in SD.CATEGORIES.values()} | set(SD.OBSOLETE_CATEGORIES)
    cats_path = os.path.join(LIVE, "CraftCategories.json")
    live_cats = json.load(io.open(cats_path, encoding="utf-8-sig")) if os.path.isfile(cats_path) else []
    shop_live = {i for c in live_cats if c.get("Name") in names for i in c.get("List", [])}
    for rid in seen:
        own = rid in shop_live or str(live_ids.get(rid, {}).get("Description", "")).startswith(("Курс", "Старая"))
        if rid in live_ids and not own:
            err.append("Id %d уже занят чужим рецептом на 16" % rid)
    # перепродажа: лавка не продаёт то, что квесты просят принести или показать (купил дёшево — сдал за награду)
    import quests_data as QD
    asked = {t["Target"].lower(): q.Name for q in QD.QUESTS for t in q.Targets if t["Kind"] in ("DELIVER", "SEARCH")}
    for r in recipes:
        for cls in [r["Result"]] + r["ResultClasses"]:
            if cls.lower() in asked:
                err.append("Id %d продаёт %s, а квест «%s» его просит — перепродажа" % (r["RecipeId"], cls, asked[cls.lower()]))
    return err, warn


def category(key, ids):
    name, icon, sound = SD.CATEGORIES[key]
    return {"Name": name, "Icon": icon, "ShowAll": 0, "UseList": 1, "ShowOnlyAvailableCraft": 0,
            "ShowOnlyAvailableStudy": 0, "CategoryFonImagePath": "set:drl_craft_work image:work",
            "CategoryCraftFonImagePath": "set:drl_craft_recipemanust image:recipemanust",
            "CategoryCraftSoundSet": sound, "List": sorted(ids)}


def dump(obj, path):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with io.open(path, "w", encoding="utf-8", newline="\r\n") as f:
        json.dump(obj, f, ensure_ascii=False, indent=4)


def merged(src_dir, recipes, cats):
    cfg = json.load(io.open(os.path.join(src_dir, "CraftConfig.json"), encoding="utf-8-sig"))
    cc = json.load(io.open(os.path.join(src_dir, "CraftCategories.json"), encoding="utf-8-sig"))
    ours = {r["RecipeId"] for r in recipes}
    names = {v[0] for v in SD.CATEGORIES.values()} | set(SD.OBSOLETE_CATEGORIES)
    full = sorted([r for r in cfg if r["RecipeId"] not in ours and not ID_MIN <= r["RecipeId"] <= ID_MAX] + recipes,
                  key=lambda r: r["RecipeId"])
    cats2 = [c for c in cc if c["Name"] not in names] + [category(k, v) for k, v in cats.items() if v]
    return cfg, full, cats2


def main():
    args = sys.argv[1:]
    recipes, cats, rows = build()
    ix = Index(os.path.join(os.environ["QUESTS16_COINS"], "config.cpp") if os.environ.get("QUESTS16_COINS") else None)
    live, full, cats2 = merged(LIVE, recipes, cats)
    err, warn = validate(ix, recipes, live)
    for w in warn:
        print("ВНИМАНИЕ:", w)
    if err:
        print("ОШИБКИ (%d):" % len(err))
        print("\n".join("  " + e for e in err))
        sys.exit(1)
    dump(full, os.path.join(OUT, "CraftConfig.json"))
    dump(cats2, os.path.join(OUT, "CraftCategories.json"))
    lines = ["# Лавка лагеря и обмен (крафт DRL сервера 16) — таблица на проверку", "",
             "Сгенерировано `make_shop16.py`. Одежда — швейная машинка, остальное — верстак. Экономика v2.0: курс "
             "1 з = %d с = %d м, цены = старый магазин ÷ %d (ТЗ, раздел 2а). «Монет» — сколько монет-предметов отдать."
             % (SD.RATE, SD.RATE * SD.RATE, SD.OLD_DIV),
             "", "| Категория | Id | Рецепт | Вариантов | Цена | Монет | Кроме монет | Примечание |",
             "|---|---|---|---|---|---|---|---|"]
    lines += ["| %s | %d | %s | %d | %s | %d | %s | %s |" % r for r in rows]
    with io.open(os.path.join(HERE, "shop_16.md"), "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines) + "\n")
    print("рецептов лавки %d (%s), всего на 16 станет %d, категорий %d"
          % (len(recipes), ", ".join("%s %d" % (SD.CATEGORIES[k][0], len(v)) for k, v in cats.items()), len(full),
             len(cats2)))
    if "--to" in args:
        dst = args[args.index("--to") + 1]
        _, f2, c2 = merged(dst, recipes, cats)
        dump(f2, os.path.join(dst, "CraftConfig.json"))
        dump(c2, os.path.join(dst, "CraftCategories.json"))
        print("записано в", dst)
    if "--deploy" in args:
        stamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        bdir = os.path.join(BACKUP, stamp + "_server16_shop_DRL")
        os.makedirs(bdir)
        for fn in ("CraftConfig.json", "CraftCategories.json"):
            shutil.copy2(os.path.join(LIVE, fn), os.path.join(bdir, fn))
        dump(full, os.path.join(LIVE, "CraftConfig.json"))
        dump(cats2, os.path.join(LIVE, "CraftCategories.json"))
        print("выложено на 16; бэкап:", bdir)


if __name__ == "__main__":
    main()
