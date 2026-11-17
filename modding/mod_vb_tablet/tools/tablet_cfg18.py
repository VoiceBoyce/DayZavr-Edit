# -*- coding: utf-8 -*-
"""Конфиг планшета сервера 18 под «Древний корень» (ТЗ: modding/mod_vb_tablet/ТЗ_планшет_18.md, утверждено 2026-11-17).

  python tablet_cfg18.py            — собрать в tools/out18 (живые файлы не трогает), показать изменения
  python tablet_cfg18.py --deploy   — то же + бэкап и запись в DayZServer18\\Instance_1\\VB_Mods\\VB_Tablet\\config
                                      + копия в «Обновлены или готовые моды\\18 сервер\\…\\config\\»

Что меняет:
  - ranks.json: XpBoss 5 (Хранитель рощи), XpRiftHeart 2 (подношение), блок Labels — подписи и значки;
  - achievements.json: достижения 16–19 со ступенями (на 18 — «Победитель Хранителя», «Друг леса», «Янтарщик»,
    «Защитник корней»);
  - contracts.json: четыре списка особых счётчиков, группы и контракты Корня, Энты, группы крафта артефактов по тирам
    Дока (Мозг был вне групп).
Контракты Корня выдаются, только когда на карте есть рощи: флаг GROVES_ON — тот же, что у квестов
(quests18/quests_data.py). Пока False — Need 0/0/0 (контракт без сложностей не выдаётся).
Формат файлов как у сервера: LF, отступ 4, группы и контракты — по строке на запись.
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
from quests_data import GROVES_ON  # noqa: E402

LIVE = os.path.join(INST18, "VB_Mods", "VB_Tablet", "config")
DOC = os.path.join(INST18, "Trader", "TraderConfig9_Doc.txt")
OUT = os.path.join(HERE, "out18")
READY = os.path.join(REPO, "Обновлены или готовые моды", "18 сервер", "Instance_1", "VB_Mods", "VB_Tablet", "config")
BACKUP = r"G:\DayZ_Test\backup"

# ---------------------------------------------------------------------------------------------- решения ТЗ
XP_BOSS, XP_HEART = 5, 2
LABELS = {
    "SrcBoss": "Хранитель", "SrcHeart": "Подношение", "SrcBossIco": "sym_tree", "SrcHeartIco": "sym_leaf",
    "CardBoss": "ХРАНИТЕЛЕЙ", "CardHeart": "ПОДНОШЕНИЙ",
    "AchName": ["Победитель Хранителя", "Друг леса", "Янтарщик", "Защитник корней"],
    "AchText": ["Одолеть Хранителя рощи", "Сделать подношения роще", "Найти янтарь", "Убить пауков-мутантов"],
    "AchIco": ["sym_tree", "sym_leaf", "sym_drop", "sym_target"],
}
ACH = [("ab_bosses", [1, 10, 50]), ("rift_hearts", [10, 100, 500]), ("ab_materials", [10, 100, 1000]),
       ("rift_zombies", [50, 500, 2500])]
SPECIAL = [("AnomalyBosses", ["VB_RootGuard_Ent"]), ("RiftZombies", ["MSFZ_Mutant_Spider*"]),
           ("RiftHeartActions", ["VB_ActionOfferRoot"]), ("AnomalyMaterials", ["VB_AR_Amber*"])]
KILL_GROUPS = [{"Kind": "m_ent", "Classes": ["MSFZ_Mutant_Ent*"]}, {"Kind": "ab_boss", "Classes": ["VB_RootGuard_Ent"]}]
ACTION_GROUPS = [
    {"Kind": "ar_offer", "Actions": ["VB_ActionOfferRoot"], "Targets": [], "PerTarget": 0},
    {"Kind": "ar_spores", "Actions": ["VB_ActionCollectSpores"], "Targets": [], "PerTarget": 0},
    {"Kind": "ar_root", "Actions": ["VB_ActionBreakRootRoot"], "Targets": [], "PerTarget": 0},
    {"Kind": "ar_amber", "Actions": ["VB_ActionSplitAmber"], "Targets": [], "PerTarget": 0},
    {"Kind": "ar_seed", "Actions": ["VB_ActionPlantSeed"], "Targets": [], "PerTarget": 0},
]
CRAFT_AR = ["VB_AR_ListenRoot", "VB_AR_SporeTrap", "VB_AR_Lantern", "VB_AR_Blood", "VB_AR_Blade", "VB_AR_Cloak",
            "VB_AR_Amulet", "VB_AR_PollenBrew", "VB_AR_Reconcile", "VB_AR_Seed", "VB_AR_Book"]
# Kind, Title, Hint, Ico, Need, ClanMult, нужны рощи
CONTRACTS = [
    ("ar_offer", "Дары роще", "Ночью сделать подношение роще", "sym_leaf", [1, 3, 6], 3, True),
    ("ar_spores", "Золотая пыльца", "Ночью собрать пыльцу у сердцевины", "sym_star", [1, 3, 6], 3, True),
    ("ar_root", "Отростки корня", "Ночью отломить отросток Древнего корня", "sym_tree", [2, 5, 10], 3, True),
    ("ar_amber", "Расколоть янтарь", "Расколоть окаменевшую сердцевину или янтарь с вкраплением", "sym_drop", [1, 2, 4], 3, True),
    ("ar_seed", "Вернуть лес", "Посадить Семя у вырезанной сердцевины", "sym_sun", [0, 1, 1], 2, True),
    ("craft_ar", "Изделия леса", "Скрафтить изделие Древнего корня", "sym_hammer", [1, 2, 4], 3, True),
    ("ab_boss", "Хранитель рощи", "Одолеть Хранителя древней рощи", "trophy", [0, 0, 1], 1, True),
    ("m_ent", "Энты", "Убить Энта", "crosshair", [1, 2, 3], 2, False),
]



# ---------------------------------------------------------------------------------------------- файлы
def read(path):
    raw = open(path, "rb").read()
    tail = "\n" if raw.endswith(b"\n") else ""
    return json.loads(raw.decode("utf-8-sig")), tail


def house(obj):
    """Как VB_CfgWriter: ключи верхнего уровня по строке, списки объектов — по объекту на строку."""
    lines, items = ["{"], list(obj.items())
    for i, (k, v) in enumerate(items):
        comma = "," if i < len(items) - 1 else ""
        if isinstance(v, list) and v and isinstance(v[0], dict):
            lines.append('    "%s": [' % k)
            for j, e in enumerate(v):
                lines.append("        " + json.dumps(e, ensure_ascii=False) + ("," if j < len(v) - 1 else ""))
            lines.append("    ]" + comma)
        else:
            lines.append('    "%s": %s%s' % (k, json.dumps(v, ensure_ascii=False), comma))
    lines.append("}")
    return "\n".join(lines)


def write(path, text, tail):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "wb") as f:
        f.write((text + tail).encode("utf-8"))


def doc_tiers():
    """Тиры артефактов из скупки Дока (заголовки «N ТИР АРТЕФАКТОВ»)."""
    t = io.open(DOC, encoding="utf-8-sig", errors="replace").read()
    sect = t[t.index("Скупка артефактов"):]
    tiers, cur = {1: [], 2: [], 3: [], 4: []}, 0
    for line in sect.splitlines():
        m = re.search(r"(\d) ТИР АРТЕФАКТОВ", line)
        if m:
            cur = int(m.group(1))
            continue
        m = re.match(r"\s*(OOTS_ART_\w+)\s*,", line)
        if m and cur:
            tiers[cur].append(m.group(1))
    return tiers


def insert_after(d, after, key, value):
    out = {}
    for k, v in d.items():
        if k == key:
            continue
        out[k] = v
        if k == after:
            out[key] = value
    if key not in out:
        out[key] = value
    return out


# ---------------------------------------------------------------------------------------------- сборка
def build():
    ranks, t_r = read(os.path.join(LIVE, "ranks.json"))
    ach, t_a = read(os.path.join(LIVE, "achievements.json"))
    con, t_c = read(os.path.join(LIVE, "contracts.json"))

    ranks = insert_after(ranks, "XpContract", "XpBoss", XP_BOSS)
    ranks = insert_after(ranks, "XpBoss", "XpRiftHeart", XP_HEART)
    ranks["Labels"] = LABELS

    have = {a["Id"]: a for a in ach["List"]}
    names = dict(zip([a for a, _ in ACH], LABELS["AchName"]))
    for aid, steps in ACH:
        have[aid] = {"Id": aid, "Name": names[aid], "Steps": steps}
    order = [a["Id"] for a in ach["List"] if a["Id"] in have] + [a for a, _ in ACH if a not in [x["Id"] for x in ach["List"]]]
    ach["List"] = [have[i] for i in order]

    after = "Bosses"
    for key, val in SPECIAL:
        con = insert_after(con, after, key, val)
        after = key
    kinds = {g["Kind"] for g in KILL_GROUPS}
    con["KillGroups"] = [g for g in con["KillGroups"] if g["Kind"] not in kinds] + KILL_GROUPS
    kinds = {g["Kind"] for g in ACTION_GROUPS}
    con["ActionGroups"] = [g for g in con["ActionGroups"] if g["Kind"] not in kinds] + ACTION_GROUPS
    # Группы находок артефактов (ItemGroups) не трогаем: пустой список сервер сам заполняет теми же группами
    # (VB_CfgContracts.Fill → DefaultItemGroups) и пишет предупреждение на каждом старте. Контрактов на них на 18 нет.
    tiers = doc_tiers()
    craft = [{"Kind": "craft_art%d" % n, "Classes": tiers[n]} for n in (1, 2, 3, 4)] + [{"Kind": "craft_ar", "Classes": CRAFT_AR}]
    kinds = {g["Kind"] for g in craft}
    con["CraftGroups"] = [g for g in con["CraftGroups"] if g["Kind"] not in kinds] + craft
    kinds = {c[0] for c in CONTRACTS}
    rows = [c for c in con["Contracts"] if c["Kind"] not in kinds]
    for kind, title, hint, ico, need, mult, groves in CONTRACTS:
        rows.append({"Kind": kind, "Title": title, "Hint": hint, "Ico": ico,
                     "Need": need if (GROVES_ON or not groves) else [0, 0, 0], "ClanMult": mult})
    con["Contracts"] = rows
    return (ranks, t_r), (ach, t_a), (con, t_c), tiers


def validate(ix, con, tiers):
    err = []
    allc = {c.lower() for c in (ix.vanilla | ix.ours | ix.live)}

    def cls_ok(c):
        if c.endswith("*"):
            return any(x.startswith(c[:-1].lower()) for x in allc)
        return c.lower() in allc

    for key, val in SPECIAL:
        for c in val:
            if key == "RiftHeartActions":
                if c not in ix.actions:
                    err.append("%s: нет длительного действия %s" % (key, c))
            elif not cls_ok(c):
                err.append("%s: нет класса %s" % (key, c))
    for g in KILL_GROUPS + [{"Kind": "craft_ar", "Classes": CRAFT_AR}]:
        for c in g["Classes"]:
            if not cls_ok(c):
                err.append("группа %s: нет класса %s" % (g["Kind"], c))
    for g in ACTION_GROUPS:
        for a in g["Actions"]:
            if a not in ix.actions:
                err.append("группа %s: нет длительного действия %s" % (g["Kind"], a))
    n = sum(len(v) for v in tiers.values())
    if n != 58 or "OOTS_ART_Braen" not in sum(tiers.values(), []):
        err.append("тиры Дока: %d артефактов (нужно 58, с Мозгом)" % n)
    kinds = {g["Kind"] for k in ("KillGroups", "LootGroups", "ActionGroups", "ItemGroups", "CraftGroups") for g in con.get(k, [])}
    base = {"zombie", "mutant", "animal", "skin", "fish", "repair", "craft", "build", "fire", "wood", "bush", "rock"}
    for c in con["Contracts"]:
        if c["Kind"] not in kinds | base:
            err.append("контракт %s: нет группы" % c["Kind"])
    return err


def main():
    args = sys.argv[1:]
    (ranks, t_r), (ach, t_a), (con, t_c), tiers = build()
    err = validate(Index(), con, tiers)
    if err:
        print("ОШИБКИ (%d):" % len(err))
        print("\n".join("  " + e for e in err))
        sys.exit(1)
    files = {"ranks.json": (json.dumps(ranks, ensure_ascii=False, indent=4), t_r),
             "achievements.json": (house(ach), t_a), "contracts.json": (house(con), t_c)}
    for fn, (text, tail) in files.items():
        write(os.path.join(OUT, fn), text, tail)
    print("тиры Дока -> craft_art1–4: %s; рощи %s: контракты Корня %s" % (
        " / ".join(str(len(tiers[i])) for i in (1, 2, 3, 4)), "есть" if GROVES_ON else "ещё не расставлены",
        "включены" if GROVES_ON else "с Need 0/0/0"))
    print("контрактов %d, групп действий %d, крафта %d, находок %d -> %s" % (
        len(con["Contracts"]), len(con["ActionGroups"]), len(con["CraftGroups"]), len(con["ItemGroups"]), OUT))
    if "--deploy" in args:
        stamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        bdir = os.path.join(BACKUP, stamp + "_srv18_tablet_cfg")
        os.makedirs(bdir)
        for fn in files:
            shutil.copy2(os.path.join(LIVE, fn), os.path.join(bdir, fn))
        for fn, (text, tail) in files.items():
            write(os.path.join(LIVE, fn), text, tail)
            os.makedirs(READY, exist_ok=True)
            shutil.copy2(os.path.join(LIVE, fn), os.path.join(READY, fn))
        print("выложено на 18 (локальная копия) и в готовые моды; бэкап:", bdir)


if __name__ == "__main__":
    main()
