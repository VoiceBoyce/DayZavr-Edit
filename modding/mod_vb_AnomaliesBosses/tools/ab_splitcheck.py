"""Проверка разделения на клиентский и серверный мод (VB_AnomaliesBosses / VB_AnomaliesBosses_Server).

Серверный PBO игрокам не отдаётся, поэтому клиентский код не должен упоминать то, что объявлено только в нём:
классы, методы и поля (m_*). Иначе у игрока скрипты не скомпилируются («Unknown type», «Undefined function»).

Как считает: из обоих деревьев scripts/ (без комментариев и строк) собираются объявленные классы, методы и поля.
Нарушение — слово в клиентском коде, которое объявлено только на сервере и не встречается в ванили (P:\\scripts):
ванильные имена (OnWork, EEHitBy...) может переопределять кто угодно.

Запуск: python ab_splitcheck.py [папка_клиента] [папка_сервера]  (по умолчанию — рядом с tools/).
Код выхода 1 — есть нарушения.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
CLIENT = os.path.join(HERE, "..", "VB_AnomaliesBosses", "scripts")
SERVER = os.path.join(HERE, "..", "VB_AnomaliesBosses_Server", "scripts")
VANILLA = "P:\\scripts"

RE_CLASS = re.compile(r"^\s*(modded\s+)?class\s+(\w+)", re.M)
RE_METHOD = re.compile(r"^\s*(?:(?:override|static|protected|private|proto|native|event|sealed|void|ref)\s+)*[\w<>,]+(?:\s*<[^>]*>)?\s+(\w+)\s*\(", re.M)
RE_FIELD = re.compile(r"^\s*(?:(?:protected|private|static|ref|const|autoptr)\s+)*[\w<>, ]+?\s+(m_\w+)\s*[;=]", re.M)
RE_WORD = re.compile(r"\b[A-Za-z_]\w*\b")


def strip(src):
    """Убрать комментарии и строковые литералы, сохранив переводы строк (для номеров строк)."""
    out = []
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if src.startswith("//", i):
            j = src.find("\n", i)
            i = n if j < 0 else j
        elif src.startswith("/*", i):
            j = src.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("\n" * src.count("\n", i, j))
            i = j
        elif c == '"':
            j = i + 1
            while j < n and src[j] != '"':
                j += 2 if src[j] == "\\" else 1
            out.append('""')
            i = j + 1
        else:
            out.append(c)
            i += 1
    return "".join(out)


def load(root):
    files = {}
    for d, _, names in os.walk(root):
        for f in names:
            if f.endswith(".c"):
                p = os.path.join(d, f)
                files[p] = strip(open(p, encoding="utf-8", errors="replace").read())
    return files


def decls(files):
    classes, methods, fields = set(), set(), set()
    for t in files.values():
        for m in RE_CLASS.finditer(t):
            if not m.group(1):
                classes.add(m.group(2))
        methods.update(RE_METHOD.findall(t))
        fields.update(RE_FIELD.findall(t))
    methods -= {"if", "while", "for", "foreach", "switch", "return", "new", "delete", "else"}
    return classes, methods, fields


def vanilla_words():
    words = set()
    if not os.path.isdir(VANILLA):
        print("ВНИМАНИЕ: нет %s, ванильные имена не исключаются" % VANILLA)
        return words
    for d, _, names in os.walk(VANILLA):
        for f in names:
            if f.endswith(".c"):
                try:
                    words.update(RE_WORD.findall(open(os.path.join(d, f), encoding="utf-8", errors="replace").read()))
                except OSError:
                    pass
    return words


def main():
    client = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else CLIENT)
    server = os.path.abspath(sys.argv[2] if len(sys.argv) > 2 else SERVER)
    cf, sf = load(client), load(server)
    cc, cm, cfld = decls(cf)
    sc, sm, sfld = decls(sf)
    van = vanilla_words()
    only_server = (sc | (sm - cm) | (sfld - cfld)) - cc - van
    bad = 0
    for p, t in sorted(cf.items()):
        for ln, line in enumerate(t.split("\n"), 1):
            for w in set(RE_WORD.findall(line)) & only_server:
                print("%s:%d: «%s» есть только в серверном моде" % (os.path.relpath(p, client), ln, w))
                bad += 1
    dup = sc & cc
    for c in sorted(dup):
        print("класс %s объявлен и в клиенте, и на сервере (на сервере нужен modded class)" % c)
        bad += 1
    print("клиент: %d файлов, сервер: %d файлов, нарушений: %d" % (len(cf), len(sf), bad))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
