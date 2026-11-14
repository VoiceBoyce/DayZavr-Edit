# -*- coding: utf-8 -*-
"""Квесты сервера 16 (ТЗ: Квесты_16.md). Данные — quests_data.py.

  python make_quests16.py            — собрать в quests16/out/INT_QUESTS (живые файлы не трогает) + quests_16_list.md
  python make_quests16.py --deploy   — то же + бэкап INT_QUESTS и выкладка на 16:
                                       старые ветки -> _old_<имя> (мод пропускает папки с «_»), новые ветки, Givers.json
  python make_quests16.py --to <папка INT_QUESTS>  — выложить в другую папку (стенд), без бэкапа

Без ошибок проверки файлы не пишутся: каждый класс целей, наград, квестовых предметов и одежды квестодателей найден
(classindex.py), действия есть в скриптах модов, Id уникальны, требования существуют, лимиты мода не превышены.
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
import quests_data as QD  # noqa: E402
from classindex import Index, REPO, SRV16  # noqa: E402

LIVE = os.path.join(SRV16, "INT_Mods", "INT_QUESTS")
OUT = os.path.join(HERE, "out", "INT_QUESTS")
BACKUP = r"G:\DayZ_Test\backup"
COINS_CFG = os.path.join(os.environ.get("QUESTS16_COINS", ""), "config.cpp") if os.environ.get("QUESTS16_COINS") else ""
ACTION_SRC = ["modding/mod_vb_Rift/VB_Rift", "modding/mod_vb_AnomaliesBosses/VB_AnomaliesBosses"]

# пределы мода (INT_QST_Const)
MAX_TARGETS, MAX_REWARDS, MAX_QITEMS, MAX_REQ = 40, 30, 10, 60
MAX_NAME, PIECE, MAX_TEXT, MAX_GREETING = 200, 900, 12000, 500


def nbytes(s):
    return len(s.encode("utf-8"))


def split_text(text, limit=PIECE):
    """Куски не длиннее limit байт, по границам предложений (движок держит 1023 байта одной строки)."""
    parts, cur = [], ""
    for sent in re.split(r"(?<=[.!?»])\s+", text.strip()):
        cand = (cur + " " + sent).strip() if cur else sent
        if nbytes(cand) <= limit:
            cur = cand
            continue
        if cur:
            parts.append(cur + " ")
        while nbytes(sent) > limit:  # одно огромное предложение — режем по словам
            words, chunk = sent.split(" "), ""
            for w in words:
                if nbytes(chunk + w + " ") > limit:
                    break
                chunk += w + " "
            parts.append(chunk)
            sent = sent[len(chunk):]
        cur = sent
    if cur:
        parts.append(cur)
    return parts


def target_json(t):
    return {"Kind": t["Kind"], "Target": t.get("Target", ""), "Count": float(t.get("Count", 1)),
            "Filter": t.get("Filter", ""), "WorstState": t.get("WorstState", ""), "MinQuantity": t.get("MinQuantity", 0),
            "Position": t.get("Position", ""), "Radius": float(t.get("Radius", 0.0)), "GroundY": t.get("GroundY", 0),
            "ShowOnMap": t.get("ShowOnMap", 0), "ZoneCenter": t.get("ZoneCenter", ""),
            "ZoneRadius": float(t.get("ZoneRadius", 0.0)), "Text": t.get("Text", "")}


def reward_json(r):
    return {"Kind": r["Kind"], "Item": r.get("Item", ""), "Count": r.get("Count", 1), "Quantity": r.get("Quantity", ""),
            "State": r.get("State", ""), "Amount": r.get("Amount", 0), "Text": r.get("Text", "")}


def quest_json(q):
    return {"Version": 1, "Id": q.Id, "Order": q.Order, "Enabled": 1, "Name": q.Name,
            "Description": split_text(q.text), "GiverStart": q.GiverStart, "GiverFinish": q.GiverFinish,
            "Requires": list(q.Requires), "RequireAny": q.RequireAny, "RepeatMode": q.RepeatMode,
            "Cooldown": q.Cooldown,
            "ResetHour": 6, "ResetMinute": 0, "ResetWeekDay": 1, "TimeLimit": q.TimeLimit,
            "QuestItems": [dict(Item=x["Item"], Count=x["Count"]) for x in q.QuestItems],
            "Targets": [target_json(t) for t in q.Targets], "Rewards": [reward_json(r) for r in q.Rewards],
            "Note": "make_quests16.py " + q.code}


def action_names():
    out = set()
    for m in ACTION_SRC:
        for d, _, fs in os.walk(os.path.join(REPO, *m.split("/"))):
            for f in fs:
                if f.endswith(".c"):
                    t = io.open(os.path.join(d, f), encoding="utf-8", errors="replace").read()
                    out |= set(re.findall(r"class\s+(\w+)\s*:\s*ActionContinuousBase\b", t))
    return out


def validate(ix, quests, givers):
    err, ids = [], {}
    actions = action_names()
    keys = {QD.BOSS} | {g["Key"] for g in givers}

    def need(cls, where):
        if cls and not ix.has(cls):
            err.append("%s: нет класса %s" % (where, cls))

    for q in quests:
        w = "%s %d «%s»" % (q.code, q.Id, q.Name)
        if q.Id in ids:
            err.append("%s: Id занят квестом %s" % (w, ids[q.Id]))
        ids[q.Id] = q.code
        if q.branch not in QD.BRANCHES:
            err.append(w + ": нет ветки " + q.branch)
        if nbytes(q.Name) > MAX_NAME:
            err.append(w + ": длинное название")
        if nbytes(q.text) > MAX_TEXT:
            err.append(w + ": длинный текст")
        if q.GiverStart not in keys or q.GiverFinish not in keys:
            err.append(w + ": нет квестодателя")
        if len(q.Targets) > MAX_TARGETS or len(q.Rewards) > MAX_REWARDS or len(q.QuestItems) > MAX_QITEMS \
                or len(q.Requires) > MAX_REQ:
            err.append(w + ": больше целей/наград/предметов/требований, чем можно")
        if not q.Rewards:
            err.append(w + ": нет наград")
        if not q.Targets and q.GiverStart == q.GiverFinish and not q.Requires:
            err.append(w + ": без целей, без требований и у того же квестодателя — пустой квест")
        for t in q.Targets:
            k = t["Kind"]
            if t.get("Count", 0) < 1 or float(t["Count"]) != int(t["Count"]):
                err.append(w + ": количество цели не целое")
            if k in ("DELIVER", "SEARCH", "KILL", "FISH"):
                need(t["Target"], w)
            if k == "KILL":
                need(t.get("Filter", ""), w + " (оружие)")
                if t.get("ZoneCenter") and not t.get("ZoneRadius"):
                    err.append(w + ": зона без радиуса")
            elif k == "ACTION":
                if t["Target"] not in actions:
                    err.append("%s: нет длительного действия %s" % (w, t["Target"]))
                need(t.get("Filter", ""), w + " (цель действия)")
            elif k == "EXPLORE":
                if not re.fullmatch(r"-?[\d.]+ -?[\d.]+ -?[\d.]+", t.get("Position", "")):
                    err.append(w + ": точка не задана")
            elif k not in ("DELIVER", "SEARCH", "FISH"):
                err.append(w + ": неизвестный тип цели " + k)
            if not t.get("Text"):
                err.append(w + ": у цели нет подписи")
        for r in q.Rewards:
            need(r["Item"], w + " (награда)")
            if not r.get("Text"):
                err.append(w + ": у награды нет подписи")
        for x in q.QuestItems:
            need(x["Item"], w + " (квестовый предмет)")
    for q in quests:
        for rid in q.Requires:
            if rid not in ids:
                err.append("%s %d: требует несуществующий %d" % (q.code, q.Id, rid))
    for g in givers:
        w = "квестодатель " + g["Key"]
        need(g["Model"], w)
        need(g.get("InHands", ""), w)
        for c in g["Clothes"]:
            need(c, w)
        if not re.fullmatch(r"[a-z0-9_]+", g["Key"]) or nbytes(g["Greeting"]) > MAX_GREETING:
            err.append(w + ": плохой ключ или длинное приветствие")
    return err


def dump(obj, path):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with io.open(path, "w", encoding="utf-8", newline="\n") as f:
        json.dump(obj, f, ensure_ascii=False, indent=4)


def givers_file(live_path):
    """Квестодатели: записи живого файла остаются как есть (их ставят, двигают и одевают в игре админкой —
    2026-11-14 повторная выкладка сбросила позиции, переставленные пользователем). Добавляются только недостающие."""
    data = json.load(io.open(live_path, encoding="utf-8-sig")) if os.path.isfile(live_path) else {"Version": 1, "Givers": []}
    out, have = [], set()
    for g in data.get("Givers", []):
        g.pop("NoSign", None)
        key = g.get("Key", "").lower()
        have.add(key)
        if key == QD.BOSS and g.get("Title") == "COINS QUESTS ALL":  # только первая выкладка: старое техническое имя
            g["Title"], g["Greeting"] = QD.BOSS_TITLE, QD.BOSS_GREETING
        out.append(g)
    for g in QD.GIVERS:
        if g["Key"] in have:
            continue
        out.append({"Key": g["Key"], "Title": g["Title"], "Kind": 0, "Model": g["Model"], "Position": g["Position"],
                    "Orientation": g["Orientation"], "Clothes": list(g["Clothes"]), "InHands": g["InHands"],
                    "Disabled": 0, "Greeting": g["Greeting"]})
    return {"Version": 1, "Givers": out}


SNAPSHOT = os.path.join(HERE, "out", "deployed")   # что генератор выложил в прошлый раз (для поиска правок в игре)


def tree_json(root):
    """{путь ветка/файл: JSON} наших веток."""
    out = {}
    for b in QD.BRANCHES:
        d = os.path.join(root, "Quests", b)
        if os.path.isdir(d):
            for f in os.listdir(d):
                if f.endswith(".json"):
                    out[b + "/" + f] = json.load(io.open(os.path.join(d, f), encoding="utf-8-sig"))
    return out


def edited_in_game():
    """Файлы наших веток на сервере, которые отличаются от прошлой выкладки (правка в игре редактором)."""
    if not os.path.isdir(SNAPSHOT):
        return []
    was, now = tree_json(SNAPSHOT), tree_json(LIVE)
    return sorted(k for k in set(was) | set(now) if was.get(k) != now.get(k))


def write_tree(root, givers_src):
    for b, (title, sort) in QD.BRANCHES.items():
        d = os.path.join(root, "Quests", b)
        if os.path.isdir(d):
            shutil.rmtree(d)
        dump({"Version": 1, "Title": title, "Color": "", "Sort": sort}, os.path.join(d, "_branch.json"))
    for q in QD.QUESTS:
        dump(quest_json(q), os.path.join(root, "Quests", q.branch, "%s_%02d.json" % (q.branch, q.Order)))
    dump(givers_file(givers_src), os.path.join(root, "Givers.json"))


QUEST_ITEM_RU = {"EFT_q_Letter1": "запечатанное письмо", "EFT_q_Letter2": "письмо с печатями",
                 "EFT_q_Document3": "папка «Медицинские исследования»", "EFT_q_Document6": "папка «Рукописные заметки»",
                 "EFT_q_Jurnals1": "журнал о ремонте авто", "EFT_q_ProtectedCase0013": "защищённый кейс 0013"}
COIN_RU = {"DZVR_Coin_Copper": "медн.", "DZVR_Coin_Silver": "серебр.", "DZVR_Coin_Gold": "золот."}


def review_md(path):
    """Список квестов для чтения: по веткам, с текстами, целями, наградами и требованиями названиями."""
    names = {QD.BOSS: QD.BOSS_TITLE}
    names.update({g["Key"]: g["Title"] for g in QD.GIVERS})
    by_id = {q.Id: q for q in QD.QUESTS}
    rep = {"ONCE": "один раз", "DAILY": "ежедневно (сброс в 06:00)", "WEEKLY": "еженедельно (сброс в пн 06:00)",
           "COOLDOWN": "сколько угодно раз"}

    def reward(r):
        if r["Item"] in COIN_RU:
            return "%d %s" % (r["Count"], COIN_RU[r["Item"]])
        n = r["Quantity"] if r.get("Quantity") and r["Quantity"] != "max" else r["Count"]
        return r["Text"] + (" ×%s" % n if str(n) != "1" else "")

    def needs(q):
        if q.Requires == QD.JOURNAL_IDS:
            return "все 14 записей журнала охотника"
        return ", ".join("«%s»" % by_id[i].Name for i in q.Requires)

    order = sorted(QD.QUESTS, key=lambda x: (QD.BRANCHES[x.branch][1], x.Order))
    n = {m: sum(q.RepeatMode == m for q in QD.QUESTS) for m in ("ONCE", "DAILY", "WEEKLY", "COOLDOWN")}
    lines = ["# Квесты сервера 16", "",
             "Сгенерировано `make_quests16.py` из `quests_data.py`. Всего %d: разовых %d, ежедневных %d, недельных %d, "
             "размен монет %d." % (len(QD.QUESTS), n["ONCE"], n["DAILY"], n["WEEKLY"], n["COOLDOWN"]), "",
             "Монеты: медн. — медные, серебр. — серебряные, золот. — золотые (курс 10:1: 10 медн. = 1 серебр., "
             "10 серебр. = 1 золот.).",
             "", "**Ветки:** " + " · ".join("%s (%d)" % (QD.BRANCHES[b][0], sum(q.branch == b for q in QD.QUESTS))
                                           for b in sorted(QD.BRANCHES, key=lambda k: QD.BRANCHES[k][1])), ""]
    cur = None
    for q in order:
        if q.branch != cur:
            cur = q.branch
            lines += ["---", "", "## " + QD.BRANCHES[cur][0], ""]
        lines.append("### %s. %s" % (q.code, q.Name))
        meta = [names[q.GiverStart] if q.GiverStart == q.GiverFinish else
                "%s → сдать: %s" % (names[q.GiverStart], names[q.GiverFinish]), rep[q.RepeatMode]]
        if q.TimeLimit:
            meta.append("на выполнение %d мин" % (q.TimeLimit // 60))
        meta.append("Id %d" % q.Id)
        lines += ["*" + " · ".join(meta) + "*", ""]
        lines += ["> " + q.text, ""]
        if q.Requires:
            lines.append("- **Открывается после:** " + needs(q))
        if q.QuestItems:
            lines.append("- **Выдаётся с квестом:** " + ", ".join(QUEST_ITEM_RU.get(x["Item"], x["Item"])
                                                               for x in q.QuestItems))
        if q.Targets:
            lines.append("- **Цели:** " + "; ".join(
                t["Text"] + (" ×%d" % t["Count"] if t["Count"] > 1 else "") for t in q.Targets))
        else:
            lines.append("- **Цели:** прийти к квестодателю" if q.GiverStart != q.GiverFinish else
                         "- **Цели:** просто сдать")
        lines += ["- **Награда:** " + "; ".join(reward(r) for r in q.Rewards), ""]
    with io.open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))


def main():
    args = sys.argv[1:]
    ix = Index(COINS_CFG or None)
    err = validate(ix, QD.QUESTS, QD.GIVERS)
    if err:
        print("ОШИБКИ (%d):" % len(err))
        print("\n".join("  " + e for e in err))
        sys.exit(1)
    write_tree(OUT, os.path.join(LIVE, "Givers.json"))
    review_md(os.path.join(HERE, "quests_16_list.md"))
    n = {m: sum(q.RepeatMode == m for q in QD.QUESTS) for m in ("ONCE", "DAILY", "WEEKLY", "COOLDOWN")}
    print("квестов %d (разовых %d, ежедневных %d, недельных %d, размен %d), веток %d, квестодателей %d -> %s"
          % (len(QD.QUESTS), n["ONCE"], n["DAILY"], n["WEEKLY"], n["COOLDOWN"], len(QD.BRANCHES),
             len(QD.GIVERS) + 1, OUT))

    if "--to" in args:
        dst = args[args.index("--to") + 1]
        write_tree(dst, os.path.join(dst, "Givers.json"))
        print("записано в", dst)
    if "--deploy" in args:
        changed = edited_in_game()
        if changed and "--force" not in args:
            print("СТОП: в игре изменены файлы квестов после прошлой выкладки — перенесите правки в quests_data.py "
                  "или выкладывайте с --force (затрёт их):")
            print("\n".join("  " + c for c in changed))
            sys.exit(2)
        stamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        bdir = os.path.join(BACKUP, stamp + "_server16_quests", "INT_QUESTS")
        shutil.copytree(LIVE, bdir, ignore=shutil.ignore_patterns("Logs"))
        qdir = os.path.join(LIVE, "Quests")
        renamed = []
        for b in sorted(os.listdir(qdir)):
            if os.path.isdir(os.path.join(qdir, b)) and not b.startswith("_") and b not in QD.BRANCHES:
                os.rename(os.path.join(qdir, b), os.path.join(qdir, "_old_" + b))
                renamed.append(b)
        write_tree(LIVE, os.path.join(LIVE, "Givers.json"))
        if os.path.isdir(SNAPSHOT):
            shutil.rmtree(SNAPSHOT)
        for b in QD.BRANCHES:
            shutil.copytree(os.path.join(LIVE, "Quests", b), os.path.join(SNAPSHOT, "Quests", b))
        print("выложено на 16; бэкап:", bdir)
        print("старые ветки -> _old_:", ", ".join(renamed) or "нет")


if __name__ == "__main__":
    main()
