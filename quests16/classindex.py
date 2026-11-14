# -*- coding: utf-8 -*-
"""Указатель классов для проверки генераторов квестов и лавки 16.

Источники:
  - ваниль: все config.cpp на P:\\DZ (распакованная 1.30);
  - наши моды из репозитория: Изнанка, боссы аномалий, AI Bandits, DRL, EFT (конфиги текстом);
  - монеты: config.cpp мода @Items_DayZavr_Coins (наш, исходников в репо нет) — распаковка из G:\\Games\\DayZavr;
  - «уже работают на 16»: классы из квестов и крафта живого 16 (там стоят моды других авторов, их PBO не смотрим).
Известные опечатки из живых файлов (лишняя точка в классе) в «уже работают» не попадают.
"""
import io
import json
import os
import re

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
P_DZ = "P:" + chr(92) + "DZ"
SRV16 = r"D:\DayZavr\DayZavr-Servers\DayZServer16\Instance_1"
OUR_MODS = ["modding/mod_vb_Rift/VB_Rift", "modding/mod_vb_AnomaliesBosses/VB_AnomaliesBosses",
            "modding/mod_AI Bandits", "modding/mod_vb_DRL_CraftMenu", "modding/mod_EscapeFromDayZ",
            "modding/mod_vb_DRL_Core"]
RX = re.compile(r"^\s*class\s+(\w+)\s*(?::\s*(\w+))?\s*\{", re.M)


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


def _live16():
    """Классы, которые уже работают в квестах и крафте 16 (кроме классов с не-словесными символами)."""
    out = set()
    qroot = os.path.join(SRV16, "INT_Mods", "INT_QUESTS", "Quests")
    for d, _, fs in os.walk(qroot):
        for f in fs:
            if not f.endswith(".json") or f == "_branch.json":
                continue
            q = json.load(io.open(os.path.join(d, f), encoding="utf-8-sig"))
            for t in q.get("Targets") or []:
                out.add(t.get("Target", ""))
            for r in q.get("Rewards") or []:
                out.add(r.get("Item", ""))
    cfg = json.load(io.open(os.path.join(SRV16, "DRL_CraftMenu", "CraftConfig.json"), encoding="utf-8-sig"))
    for r in cfg:
        out.add(r.get("Result", ""))
        out.update(r.get("ResultClasses") or [])
        for c in r.get("CraftComponents") or []:
            out.update(c.get("Classnames") or [])
    return {c for c in out if c and re.fullmatch(r"\w+", c)}


class Index:
    def __init__(self, coins_cfg=None):
        self.parents = {}
        self.vanilla = _scan(P_DZ, self.parents)
        self.ours = set()
        for m in OUR_MODS:
            p = os.path.join(REPO, *m.split("/"))
            if os.path.isdir(p):
                self.ours |= _scan(p, self.parents)
        if coins_cfg and os.path.isfile(coins_cfg):
            self.ours |= _scan(os.path.dirname(coins_cfg), self.parents)
        self.live = _live16()
        self.low = {c.lower(): c for c in (self.vanilla | self.ours | self.live)}

    def has(self, cls):
        return cls.lower() in self.low

    def source(self, cls):
        if cls in self.vanilla:
            return "ваниль"
        if cls in self.ours:
            return "наш мод"
        if cls in self.live:
            return "работает на 16"
        return ""

    def is_kind_of(self, cls, base):
        c, base = cls.lower(), base.lower()
        for _ in range(60):
            if c == base:
                return True
            c = self.parents.get(c)
            if not c:
                return False
        return False
