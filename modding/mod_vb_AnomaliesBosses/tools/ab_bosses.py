# -*- coding: utf-8 -*-
"""bosses.json по умолчанию: 14 стартовых боссов (ТЗ 4.4, 4.5) -> VB_AnomaliesBosses_Server/defaults/bosses.json (серверный PBO).

Мод копирует этот файл в профиль сервера, если там bosses.json ещё нет. Дальше сервер правит свою копию.
Здесь только стартовые значения: модели (4.4), аномалии и способности (4.5), артефакты по уровню, трофей.
Артефакты — классы OOTS_ART_* с сервера 16 (тиры из конфига AnomalySpawner): у 1-го уровня тиры 1–2,
у 2-го — 2–3, у 3-го — 3–4; артефакты своей стихии весят больше. Нет класса на сервере — строка пропускается
с предупреждением в логе.
Запуск: python ab_bosses.py
"""
import json
import os

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "..", "VB_AnomaliesBosses_Server", "defaults", "bosses.json")

A = {   # русское имя из ТЗ -> класс
    "Электра": "Electra", "Млечный путь": "MlechniPut", "ЭМИ": "EMP", "Гроза": "Groza",
    "Студень": "Studen", "Холера": "Holera", "Пар": "Par", "Рой": "Roy", "Мёртвая зона": "DeadZone",
    "Жарка": "Zharka", "Огненная искра": "FireIskra", "Огненный шторм": "FireShtorm",
    "Ломка": "Lomka", "Трамплин": "Tramplin", "Воронка": "Voronka", "Атом": "Atom",
    "Злая линза": "LinceAgr", "Линза-обманка": "Lince", "Вспышка": "Flash",
    "Застой": "Zastoy", "Стекло": "Steklo", "Снег": "Snow",
}


def cls(name):
    return "VB_AB_Anomaly_" + A[name]


# Модели (ТЗ 4.4): поле, потолок аномалий, поводок, здоровье, масштаб
# MeleeMul — во сколько раз удар босса сильнее удара обычного зомби (здоровье и кровь; шок как у зомби)
MODELS = {
    "Ricar":        dict(Radius=15, MaxAnomalies=8,  LeashRadius=30, Health=2500, Scale=1.0, MeleeMul=1.5),
    "MutaWolf":     dict(Radius=35, MaxAnomalies=12, LeashRadius=50, Health=3500, Scale=1.1, MeleeMul=2.0),
    "Tenevnik":     dict(Radius=25, MaxAnomalies=12, LeashRadius=40, Health=3000, Scale=1.0, MeleeMul=1.5),
    "Shum":         dict(Radius=20, MaxAnomalies=14, LeashRadius=30, Health=3500, Scale=1.0, MeleeMul=1.5),
    "MozgoTras":    dict(Radius=25, MaxAnomalies=12, LeashRadius=40, Health=4500, Scale=1.1, MeleeMul=2.5),
    "MozgoTrasRed": dict(Radius=18, MaxAnomalies=12, LeashRadius=30, Health=5000, Scale=1.1, MeleeMul=2.5),
    "Ent":          dict(Radius=30, MaxAnomalies=16, LeashRadius=45, Health=8000, Scale=1.6, MeleeMul=3.0),
}

# «Вырезать трофей», секунд (ТЗ 2.7)
CUT_TIME = {"Ricar": 10, "MutaWolf": 15, "Shum": 15, "Tenevnik": 15, "MozgoTras": 20, "MozgoTrasRed": 20, "Ent": 30}

# Способности (ТЗ 4.3): стартовые числа, подбираются на стенде (этап 4)
ABIL = {
    "AnomalyUnderFoot": dict(Cooldown=25, Range=0),
    "Discharge":  dict(Cooldown=12, Range=20, Damage=25),
    "Pulse":      dict(Cooldown=40, Range=30, DurationSec=20),
    "Heat":       dict(Cooldown=20, Range=10, Damage=15),
    "IceBreath":  dict(Cooldown=18, Range=10, Damage=10, DurationSec=6),
    "Blizzard":   dict(Cooldown=60, Range=20, DurationSec=25),
    "Pull":       dict(Cooldown=20, Range=20),
    "Knockback":  dict(Cooldown=20, Range=8, Damage=10),
    "AcidSpit":   dict(Cooldown=10, Range=25, Damage=15),
    "PsiStrike":  dict(Cooldown=25, Range=20, DurationSec=6),
    "Flash":      dict(Cooldown=30, Range=15, DurationSec=5),
    "Decay":      dict(Cooldown=2,  Range=9,  Damage=2),
    "Pack":       dict(Cooldown=90, Range=10),
    "Dash":       dict(Cooldown=12, Range=15, Damage=20),
    "Shift":      dict(Cooldown=20, Range=0),
    "Scream":     dict(Cooldown=25, Range=15, DurationSec=4),
    "Call":       dict(Cooldown=60, Range=30),
    "Rage":       dict(Cooldown=0,  Range=0),
    "GroundSlam": dict(Cooldown=20, Range=6, Damage=20),
}

# Артефакты сервера 16 по тирам (AnomalySpawner/Anomalies_Work: тир аномалии -> её список артефактов)
ART_TIER = {
    1: "izolator Klyaksa kameniy_cvetok kolobok Plasma snejinka shturval DevelHert",
    2: "svet Battery Battery3 AngelFool BloodStone_New Plamya2 CrustalThorn SeaUrchin myaso",
    3: "DarkSoul kameniy_cvetok2 kameniy_cvetok3 puzir Burn Glaz Glaz2 Glaz3 Crustal Crustal2 Crustal3 busy2 "
       "Plamya Plamya3 Angel MoonLight FireBall Mohovik DemonEye vspishka Sparkler",
    4: "krov_kamnya vivert Gravi NightStar ShineOfTheForest Mikrob Braen NanoCell Light Compas Spring Oasis "
       "Flower ribka Monolit",
}
# Стихия палитры -> артефакты «своей» стихии (вес ×2,5, если по тиру они доступны уровню босса)
THEME = {
    "Storm":   "Battery Battery3 Plasma vspishka Sparkler NightStar",
    "Fire":    "Plamya Plamya2 Plamya3 FireBall Burn",
    "Ice":     "snejinka Crustal Crustal2 Crustal3 CrustalThorn MoonLight",
    "Acid":    "Klyaksa puzir Mohovik myaso SeaUrchin",
    "Gravity": "Gravi kolobok shturval Spring Compas",
    "Psi":     "Glaz Glaz2 Glaz3 DemonEye Braen svet",
    "Rad":     "DarkSoul Mikrob NanoCell krov_kamnya busy2",
}
LEVEL = {   # уровень босса -> (тиры, бросков, «ничего», макс. в поле)
    1: ((1, 2), 3, 30, 4),
    2: ((2, 3), 4, 25, 6),
    3: ((3, 4), 5, 20, 8),
}
DETECTOR_TIER = {1: 1, 2: 1, 3: 2, 4: 3}    # тир артефакта -> какой уровень детектора его находит


def an(name, count=None, place="Any", chance=None, max_in_field=None, one_of=None):
    d = {}
    if one_of:
        d["OneOf"] = [{"Classname": cls(n), "Weight": w} for n, w in one_of]
    else:
        d["Classname"] = cls(name)
    if count:
        d["Count"] = list(count) if isinstance(count, (list, tuple)) else [count, count]
    d["Place"] = place
    if chance is not None:
        d["Chance"] = chance
    if max_in_field:
        d["MaxInField"] = max_in_field
    return d


def ab(t, requires=None, chance=None):
    d = {"Type": t}
    d.update({k: v for k, v in ABIL[t].items() if v})
    if requires:
        d["RequiresAnomaly"] = [cls(r) for r in requires]
    if chance is not None:
        d["Chance"] = chance
    return d


def artifacts(level, palette):
    tiers, rolls, nothing, max_in_field = LEVEL[level]
    theme = set(THEME[palette].split())
    tier_of = {}
    for t in sorted(ART_TIER):
        for a in ART_TIER[t].split():
            tier_of.setdefault(a, t)        # артефакт из нескольких тиров считаем по младшему
    items = []
    for a, t in sorted(tier_of.items(), key=lambda x: (x[1], x[0].lower())):
        if t not in tiers:
            continue
        w = 10 if t == tiers[0] else 4
        if a in theme:
            w = round(w * 2.5)
        it = {"Classname": "OOTS_ART_" + a, "Chance": w}
        if DETECTOR_TIER[t] > 1:
            it["MinTier"] = DETECTOR_TIER[t]
        items.append(it)
    return {"MaxInField": max_in_field, "NearAnomalyShare": 0.5, "Rolls": rolls, "Nothing": nothing, "Items": items}


# ТЗ 4.5: (id, имя, модель, палитра, уровень, всегда+с шансом, фазы, способности)
B = []


def boss(bid, name, model, pal, level, anomalies, phases, abilities):
    B.append((bid, name, model, pal, level, anomalies, phases, abilities))


boss("ent_storm", "Громовой Энт", "Ent", "Storm", 3,
     [an("Электра", (4, 6), "Ring"), an("Млечный путь", (3, 4), "Edge"),
      an("ЭМИ", (1, 2), "Ring", 50), an("Гроза", None, "Center", 60)],
     [(0.6, [an("Электра", 2, "Ring", 50)]), (0.3, [an("Гроза", None, "Center")])],
     [ab("Discharge", ["Электра", "Гроза"]), ab("Pulse", ["ЭМИ"]), ab("GroundSlam")])
boss("ent_acid", "Болотный Энт", "Ent", "Acid", 3,
     [an("Студень", (4, 5), "Ring"), an("Холера", (3, 4), "Edge"),
      an(None, None, "Center", 70, one_of=[("Рой", 2), ("Мёртвая зона", 1)]), an("Пар", (1, 2), "Ring", 30)],
     [(0.3, [an("Рой", 2, "Ring")])],
     [ab("AcidSpit"), ab("Decay", ["Мёртвая зона"]), ab("GroundSlam")])
boss("mozgotras_fire", "Пламенный Мозготряс", "MozgoTrasRed", "Fire", 3,
     [an("Жарка", (4, 5), "Ring"), an("Огненная искра", 2, "Edge"),
      an("Огненный шторм", None, "Center", 60), an("Пар", (1, 2), "Ring", 40)],
     [(0.3, [an("Огненный шторм", None, "Center")])],
     [ab("Heat"), ab("Rage")])
boss("mozgotras_gravity", "Давящий Мозготряс", "MozgoTrasRed", "Gravity", 3,
     [an("Ломка", (3, 4), "Ring"), an("Трамплин", 3, "Edge"),
      an("Воронка", None, "Center", 70), an("Атом", 1, "Ring", 20)],
     [(0.3, [an("Воронка", 1, "Ring")])],
     [ab("Pull", ["Воронка"]), ab("Knockback"), ab("Rage")])
boss("mozgotras_psi", "Пси-Мозготряс", "MozgoTras", "Psi", 2,
     [an("Злая линза", (3, 5), "Ring"), an("Линза-обманка", 3, "Any"),
      an("Вспышка", None, "Center", 60), an("Застой", 1, "Edge", 30)],
     [(0.3, [an("Злая линза", 2, "Ring")])],
     [ab("PsiStrike"), ab("Flash", ["Вспышка"]), ab("Call")])
boss("mozgotras_storm", "Грозовой Мозготряс", "MozgoTras", "Storm", 2,
     [an("ЭМИ", None, "Center"), an("Электра", (3, 4), "Ring"),
      an("Млечный путь", (2, 3), "Edge", 60), an("Злая линза", 2, "Ring", 30)],
     [(0.3, [an("Электра", 2, "Ring")])],
     [ab("Discharge"), ab("Pulse"), ab("PsiStrike", ["Злая линза"]), ab("Call")])
boss("wolf_ice", "Ледяной Мутаволк", "MutaWolf", "Ice", 2,
     [an("Застой", (4, 6), "Edge"), an("Стекло", (2, 3), "Ring"), an("Снег", None, "Center", 35)],
     [(0.3, [an("Стекло", 2, "Ring")])],
     [ab("IceBreath"), ab("Blizzard", ["Снег"]), ab("Dash")])
boss("wolf_fire", "Огненный Мутаволк", "MutaWolf", "Fire", 2,
     [an("Огненная искра", (4, 5), "Edge"), an("Жарка", 2, "Ring"),
      an("Трамплин", 2, "Edge", 40), an("Огненный шторм", None, "Center", 25)],
     [(0.6, [an("Огненная искра", 2, "Edge")])],
     [ab("Heat"), ab("Dash")])
boss("shum_gravity", "Шум-Воронка", "Shum", "Gravity", 2,
     [an("Трамплин", (4, 6), "Edge"), an("Воронка", (2, 3), "Ring"),
      an("Атом", None, "Center", 40), an("Ломка", 2, "Ring", 50)],
     [(0.6, [an("Трамплин", 2, "Edge")])],
     [ab("Pull"), ab("Knockback", ["Атом"]), ab("Scream")])
boss("shum_ice", "Снежная вдова", "Shum", "Ice", 2,
     [an("Стекло", (3, 4), "Ring"), an("Злая линза", 2, "Ring"),
      an("Застой", (2, 3), "Edge", 60), an("Линза-обманка", 2, "Any", 30), an("Стекло", None, "Center", 50)],
     [(0.3, [an("Застой", 2, "Edge")])],
     [ab("IceBreath"), ab("Scream"), ab("PsiStrike", ["Злая линза"]), ab("Blizzard", None, 30)])
boss("tenevnik_psi", "Теневик-мираж", "Tenevnik", "Psi", 2,
     [an("Линза-обманка", (3, 5), "Any"), an("Злая линза", 2, "Ring"),
      an("Вспышка", (1, 2), "Ring", 50), an("Застой", None, "Center", 40)],
     [(0.6, [an("Линза-обманка", 2, "Any")])],
     [ab("Shift"), ab("PsiStrike"), ab("Flash", ["Вспышка"])])
boss("tenevnik_rad", "Теневик-тьма", "Tenevnik", "Rad", 2,
     [an("Рой", (3, 4), "Ring"), an("Холера", 3, "Edge"),
      an("Мёртвая зона", None, "Center", 50), an("Атом", 1, "Ring", 30), an("Линза-обманка", 2, "Any", 30)],
     [(0.3, [an("Мёртвая зона", None, "Center")])],
     [ab("Decay", ["Мёртвая зона"]), ab("Shift"), ab("Knockback", ["Атом"])])
boss("ricar_acid", "Кислотный Ричар", "Ricar", "Acid", 1,
     [an("Студень", (3, 4), "Ring"), an("Холера", (1, 2), "Edge", 70), an("Рой", None, "Center", 20)],
     [(0.3, [an("Студень", 1, "Ring")])],
     [ab("AcidSpit"), ab("Pack")])
boss("ricar_rad", "Гнилой Ричар", "Ricar", "Rad", 1,
     [an("Холера", 2, "Ring"), an("Рой", (1, 2), "Edge"),
      an("Атом", None, "Center", 25), an("Студень", (1, 2), "Ring", 30)],
     [(0.3, [an("Рой", 1, "Edge")])],
     [ab("Decay", None, 50), ab("Pack"), ab("Knockback", ["Атом"])])


def max_count(line):
    c = line.get("Count", [1, 1])
    return c[1]


def main():
    bosses, problems = [], []
    for bid, name, model, pal, level, anomalies, phases, abilities in B:
        m = MODELS[model]
        top = sum(max_count(x) for x in anomalies) + sum(max_count(x) for _, ph in phases for x in ph)
        if top > m["MaxAnomalies"]:
            problems.append("%s: аномалий до %d при потолке %d" % (bid, top, m["MaxAnomalies"]))
        bosses.append({
            "Id": bid,
            "Name": name,
            "Classname": "VB_AB_%s_%s" % (model, pal),
            "Tier": level,
            "Health": m["Health"],
            "Scale": m["Scale"],
            "MeleeMul": m["MeleeMul"],
            "Field": {"Radius": m["Radius"], "MaxAnomalies": m["MaxAnomalies"], "MinGap": 4, "KeepFromPlayers": 3,
                      "LeashRadius": m["LeashRadius"], "AfterDeathSec": 120},
            "Anomalies": anomalies,
            "Phases": [{"Health": h, "Anomalies": ph} for h, ph in phases],
            "Abilities": [ab("AnomalyUnderFoot")] + abilities,
            # "Artifacts" не пишем: артефакты в полях боссов не спавнятся (решение пользователя 2026-11-13).
            # Код поля их по-прежнему умеет — таблицу можно вписать боссу в bosses.json руками; artifacts() ниже — её образец.
            "Trophy": {"CutTimeSec": CUT_TIME[model], "ToolDamage": 15, "CorpseLifetimeMin": 15,
                       "Items": [{"Classname": "VB_AB_Trophy_%s_%s" % (model, pal)}]},
        })
    used = {x.get("Classname") for b in bosses for x in b["Anomalies"]} | \
           {o["Classname"] for b in bosses for x in b["Anomalies"] for o in x.get("OneOf", [])} | \
           {x.get("Classname") for b in bosses for p in b["Phases"] for x in p["Anomalies"]}
    missing = sorted(set(cls(n) for n in A) - used)
    if missing:
        problems.append("не использованы аномалии: " + ", ".join(missing))
    if problems:
        raise SystemExit("\n".join(problems))
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w", encoding="utf-8", newline="\n") as f:
        json.dump({"Bosses": bosses}, f, ensure_ascii=False, indent=2)
        f.write("\n")
    print("боссов %d, все 22 аномалии в деле, файл %s (%.0f КБ)" % (len(bosses), os.path.normpath(OUT),
                                                                  os.path.getsize(OUT) / 1024.0))


main()
