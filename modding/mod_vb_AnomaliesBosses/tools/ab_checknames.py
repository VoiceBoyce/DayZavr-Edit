# -*- coding: utf-8 -*-
"""Проверка имён переменных и полей мода против ванили (ловушки E5x каталога ошибок).

Enforce не даёт назвать переменную так же, как класс/enum/typedef (Effect, Debug, Pos...),
и не даёт использовать ключевые слова (out, local, ...). Компилятор узнаёт об этом только на сервере.
Скрипт собирает объявления «Тип имя;» / «Тип имя =» / параметры функций в scripts/ мода
и сверяет имена с типами из P:\\scripts и самого мода.
Запуск: python ab_checknames.py   (код выхода 1 — есть конфликты)
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
MOD = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "..", "VB_AnomaliesBosses", "scripts")
VANILLA = "P:\\scripts"

KEYWORDS = {
    "out", "inout", "local", "auto", "ref", "autoptr", "proto", "native", "static", "const", "private",
    "protected", "override", "event", "external", "volatile", "owned", "notnull", "modded", "sealed",
    "class", "enum", "typedef", "extends", "new", "delete", "return", "if", "else", "for", "foreach",
    "while", "do", "switch", "case", "default", "break", "continue", "this", "super", "null", "true",
    "false", "thread", "goto", "void", "int", "float", "bool", "string", "vector", "typename", "func",
    "array", "set", "map", "Class", "Managed", "reference", "abstract",
}

TYPE_RE = re.compile(r"^\s*(?:modded\s+|sealed\s+)?(?:class|enum)\s+(\w+)", re.M)
TYPEDEF_RE = re.compile(r"^\s*typedef\s+[\w<>,\s]+?\s+(\w+)\s*;", re.M)
# объявление: [модификаторы] Тип[<...>][[]] имя ( ; | = | , | ) | [ )
DECL_RE = re.compile(
    r"(?:^|[;{}(,]\s*|\b(?:protected|private|static|const|ref|autoptr|out|inout|notnull|owned)\s+)"
    r"([A-Za-z_]\w*(?:<[^;(){}]*?>)?)\s+([A-Za-z_]\w*)\s*(?:\[[^\]]*\]\s*)?(?=[;=,)\[])")
NOT_TYPES = {"return", "else", "new", "delete", "case", "typedef", "class", "enum", "extends", "modded",
             "override", "proto", "native", "event", "goto", "thread"}


def strip_comments(text):
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//[^\n]*", "", text)
    text = re.sub(r'"(?:\\.|[^"\\])*"', '""', text)
    return text


def read(fn):
    with open(fn, encoding="utf-8", errors="replace") as f:
        return strip_comments(f.read())


def walk(root):
    for d, _, files in os.walk(root):
        for n in files:
            if n.lower().endswith(".c"):
                yield os.path.join(d, n)


def collect_types(root):
    names = {}
    for fn in walk(root):
        t = read(fn)
        for m in TYPE_RE.finditer(t):
            names.setdefault(m.group(1), fn)
        for m in TYPEDEF_RE.finditer(t):
            names.setdefault(m.group(1), fn)
    return names


def main():
    if not os.path.isdir(VANILLA):
        sys.exit("нет %s (subst P: G:\\DayZ_P)" % VANILLA)
    types = collect_types(VANILLA)
    mod_types = collect_types(MOD)
    for k, v in mod_types.items():
        types.setdefault(k, v)
    bad = 0
    for fn in walk(MOD):
        t = read(fn)
        for m in DECL_RE.finditer(t):
            typ, name = m.group(1), m.group(2)
            if typ in NOT_TYPES:
                continue
            line = t.count("\n", 0, m.start(2)) + 1
            rel = os.path.relpath(fn, MOD)
            if name in KEYWORDS:
                print("%s:%d: имя '%s' — ключевое слово" % (rel, line, name))
                bad += 1
            elif name in types:
                print("%s:%d: имя '%s' занято типом (%s)" % (rel, line, name, os.path.relpath(types[name], VANILLA) if types[name].startswith(VANILLA) else "мод"))
                bad += 1
    print("конфликтов:", bad)
    sys.exit(1 if bad else 0)


main()
