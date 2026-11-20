# -*- coding: utf-8 -*-
"""Указатель классов сервера 18 — общий для make_quests18.py и make_shop18.py.

Источники (что считаем «класс есть на 18»):
  - ваниль: все config.cpp на P:\\DZ (распакованная 1.30);
  - наши и восстановленные моды из репозитория (конфиги текстом), только те, что стоят на 18 (OUR_MODS18);
  - «уже работают на 18»: классы из квестов (кроме старых веток покупок — по ним строится лавка), крафта DRL,
    торговцев (Instance_1\\Trader\\TraderConfig*.txt), types, подключённых в cfgeconomycore.xml миссии,
    и спавнеров существ (VD_SpavnMutant, SpawnerBubaku).
Чужие PBO не распаковываем (правило пользователя). Классы с не-словесными символами (опечатка «...multicam.»)
в «уже работают» не попадают.

Index().actions — длительные действия (наследники ActionContinuousBase по скриптам P:\\scripts и модов репозитория)
плюс FOREIGN_LONG_ACTIONS (чужие моды без исходников, из ТЗ Квесты_18 §10 — проверять на стенде).
"""
import io
import json
import os
import re

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
P_DZ = "P:" + chr(92) + "DZ"
P_SCRIPTS = "P:" + chr(92) + "scripts"
SRV18 = r"D:\DayZavr\DayZavr-Servers\DayZServer18"
INST18 = I18 = os.path.join(SRV18, "Instance_1")
MISSION = os.path.join(SRV18, "mpmissions", "dayzOffline.enoch")
# Моды репозитория, которые стоят на 18 (modlist_18server.txt). VB_Rift, VB_AnomaliesBosses, VB_SakhalGrip,
# INT_VppMap — только 16, их классов на 18 нет, поэтому сюда не входят.
OUR_MODS18 = ["modding/mod_EscapeFromDayZ", "modding/mod_vb_DRL_Core", "modding/mod_vb_DRL_CraftMenu",
              "modding/mod_MSFZ_Mifical_Pack", "modding/mod_AdvansWeapScope_v2", "modding/mod_Scopes_DJ_LaserSight",
              "modding/mod_AI Bandits", "modding/mod_vb_clothes_dayzavr", "modding/mod_vb_AncientRoot",
              "modding/mod_OTTS_Artifacts", "modding/mod_vb_tablet",
              "modding/mod_VB_MiningResources/VB_MiningResources"]
# Старые ветки покупок 18 — источник лавки; в «уже работают» не берём, иначе проверка лавки сама себя подтверждает.
SHOP_BRANCHES = {"COINS_TRADE_CLOTHES", "COINS_TRADE_WEAPONS", "COINS_TRADE_MATERIAL"}
# Длительные действия чужих модов без исходников. Было BRDK GoldRush (ActionGoldMine, ActionSearchGold), но с
# 2026-11-17 руду на 18 делает VB_MiningResources (его действия находятся по исходникам), BRDK на сервере нет — баг №59.
FOREIGN_LONG_ACTIONS = set()
RX = re.compile(r"^\s*class\s+(\w+)\s*(?::\s*(\w+))?\s*\{", re.M)
RX_SCRIPT = re.compile(r"^\s*(?:modded\s+)?class\s+(\w+)\s*(?:extends|:)\s*(\w+)", re.M)
RX_TRADER = re.compile(r"^\s*([A-Za-z_]\w*)\s*,\s*[^,]*,\s*-?\d+\s*,\s*-?\d+", re.M)
RX_TYPE = re.compile(r'<type\s+name="([^"]+)"')


def _scan(root, parents, exts=(".cpp", ".hpp")):
    out = set()
    for d, _, fs in os.walk(root):
        for f in fs:
            if f.lower().endswith(exts):
                t = io.open(os.path.join(d, f), encoding="utf-8", errors="replace").read()
                t = re.sub(r"/\*.*?\*/", "", t, flags=re.S)
                for m in RX.finditer(t):
                    out.add(m.group(1))
                    if m.group(2):
                        parents.setdefault(m.group(1).lower(), m.group(2).lower())
    return out


def _script_parents(roots):
    """{класс: родитель} из скриптов Enforce (*.c)."""
    par = {}
    for root in roots:
        for d, _, fs in os.walk(root):
            for f in fs:
                if f.lower().endswith(".c"):
                    t = io.open(os.path.join(d, f), encoding="utf-8", errors="replace").read()
                    for m in RX_SCRIPT.finditer(t):
                        par.setdefault(m.group(1), m.group(2))
    return par


def _long_actions(roots):
    par = _script_parents(roots)
    out = set()
    for c in par:
        x = c
        for _ in range(40):
            x = par.get(x)
            if not x:
                break
            if x == "ActionContinuousBase":
                out.add(c)
                break
    return out | FOREIGN_LONG_ACTIONS


def _quests18():
    """Классы целей и наград квестов 18, кроме старых веток покупок (и их копий _old_*)."""
    out = set()
    qroot = os.path.join(I18, "INT_Mods", "INT_QUESTS", "Quests")
    for d, _, fs in os.walk(qroot):
        b = os.path.basename(d)
        if b in SHOP_BRANCHES or (b.startswith("_old_") and b[5:] in SHOP_BRANCHES):
            continue
        for f in fs:
            if not f.endswith(".json") or f == "_branch.json":
                continue
            q = json.load(io.open(os.path.join(d, f), encoding="utf-8-sig"))
            for t in q.get("Targets") or []:
                out.add(t.get("Target", ""))
            for r in q.get("Rewards") or []:
                out.add(r.get("Item", ""))
    return out


def _craft18():
    out = set()
    cfg = json.load(io.open(os.path.join(I18, "DRL_CraftMenu", "CraftConfig.json"), encoding="utf-8-sig"))
    for r in cfg:
        if 5000 <= r["RecipeId"] <= 5999:   # лавка, если уже выложена, — не доказательство
            continue
        out.add(r.get("Result", ""))
        out.update(r.get("ResultClasses") or [])
        for c in r.get("CraftComponents") or []:
            out.update(c.get("Classnames") or [])
    return out


def _traders18():
    out = set()
    tdir = os.path.join(I18, "Trader")
    for f in os.listdir(tdir):
        if f.lower().startswith("traderconfig") and f.lower().endswith(".txt"):
            t = io.open(os.path.join(tdir, f), encoding="utf-8", errors="replace").read()
            t = "\n".join(line.split("//", 1)[0] for line in t.splitlines())
            out.update(m.group(1) for m in RX_TRADER.finditer(t))
    return out


def _types18():
    """types, подключённые в cfgeconomycore.xml (папки <ce folder>), и db/types.xml."""
    out = set()
    core = io.open(os.path.join(MISSION, "cfgeconomycore.xml"), encoding="utf-8", errors="replace").read()
    core = re.sub(r"<!--.*?-->", "", core, flags=re.S)
    files = [os.path.join(MISSION, "db", "types.xml")]
    for m in re.finditer(r'<ce\s+folder="([^"]+)"\s*>(.*?)</ce>', core, re.S):
        for fm in re.finditer(r'<file\s+name="([^"]+)"\s+type="types"', m.group(2)):
            files.append(os.path.join(MISSION, *m.group(1).split("/"), fm.group(1)))
    for p in files:
        if os.path.isfile(p):
            t = io.open(p, encoding="utf-8", errors="replace").read()
            t = re.sub(r"<!--.*?-->", "", t, flags=re.S)
            out.update(RX_TYPE.findall(t))
    return out


def _spawners18():
    """Существа и предметы из спавнеров: VD_SpavnMutant (строка — классы через запятую), SpawnerBubaku (строки JSON с «_»)."""
    out = set()
    vd = os.path.join(I18, "VD_SpavnMutant", "CONFIGS")
    if os.path.isdir(vd):
        for f in os.listdir(vd):
            if f.lower().endswith(".txt"):
                for line in io.open(os.path.join(vd, f), encoding="utf-8", errors="replace"):
                    parts = [s.strip() for s in line.split(",")]
                    if all(re.fullmatch(r"[A-Za-z]\w*_\w+", s) for s in parts):
                        out.update(parts)
    bb = os.path.join(I18, "SpawnerBubaku", "SpawnerBubakuV2.json")
    if os.path.isfile(bb):
        t = io.open(bb, encoding="utf-8-sig", errors="replace").read()
        out.update(re.findall(r'"([A-Za-z][A-Za-z0-9]*_[A-Za-z0-9_]+)"', t))
    return out


def _clean(s):
    return {c for c in s if c and re.fullmatch(r"\w+", c)}


class Index:
    def __init__(self):
        self.parents = {}
        self.vanilla = _scan(P_DZ, self.parents)
        self.ours = set()
        roots = [P_SCRIPTS]
        for m in OUR_MODS18:
            p = os.path.join(REPO, *m.split("/"))
            if os.path.isdir(p):
                self.ours |= _scan(p, self.parents)
                roots.append(p)
        self.quests = _clean(_quests18())
        self.craft = _clean(_craft18())
        self.traders = _clean(_traders18())
        self.types = _clean(_types18())
        self.spawners = _clean(_spawners18())
        self.live = self.quests | self.craft | self.traders | self.types | self.spawners
        self.low = {c.lower(): c for c in (self.vanilla | self.ours | self.live)}
        self._src = {}
        for name, s in (("ваниль", self.vanilla), ("наш мод", self.ours), ("квесты 18", self.quests),
                        ("крафт 18", self.craft), ("торговцы 18", self.traders), ("types 18", self.types),
                        ("спавнеры 18", self.spawners)):
            for c in s:
                self._src.setdefault(c.lower(), []).append(name)
        self.actions = _long_actions(roots)

    def has(self, cls):
        return cls.lower() in self.low

    def source(self, cls):
        return ", ".join(self._src.get(cls.lower(), []))

    def is_kind_of(self, cls, base):
        c, base = cls.lower(), base.lower()
        for _ in range(60):
            if c == base:
                return True
            c = self.parents.get(c)
            if not c:
                return False
        return False
