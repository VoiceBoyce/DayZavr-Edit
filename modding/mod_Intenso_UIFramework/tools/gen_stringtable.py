"""INT_UIFramework stringtable generator.

Source: tools/i18n/int_uif.json -- "STR_INT_UIF_<KEY>" -> {lang: text} for the
12 engine languages (en es de ru zh fr it pl cs hu ja pt); an optional "_ctx"
entry is a note for translators and is not written out.

Output: INT_UIFramework/stringtable.csv with the full 14-column header. With
fewer language columns the engine loads the PBO but leaves keys raw
(dayz-ui-development, stringtable CSV ladder). "original" repeats English.

Refuses to write when a key misses a language, a text is empty, or a
translation lost a %n placeholder: a half-translated table is exactly what
this generator exists to prevent.

Run:  python tools/gen_stringtable.py
"""
import csv
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "tools", "i18n", "int_uif.json")
OUT = os.path.join(ROOT, "INT_UIFramework", "stringtable.csv")
HEADER = ["Language", "original", "english", "spanish", "german", "russian", "chinesesimp",
          "french", "italian", "polish", "czech", "hungarian", "japanese", "portuguese"]
# CSV column -> language code in the source
COLUMNS = [("english", "en"), ("spanish", "es"), ("german", "de"), ("russian", "ru"),
           ("chinesesimp", "zh"), ("french", "fr"), ("italian", "it"), ("polish", "pl"),
           ("czech", "cs"), ("hungarian", "hu"), ("japanese", "ja"), ("portuguese", "pt")]
PLACEHOLDER = re.compile(r"%\d")

with open(SRC, encoding="utf-8") as f:
    strings = json.load(f)

errors = []
for key, texts in strings.items():
    if not key.startswith("STR_INT_UIF_"):
        errors.append("%s: key must start with STR_INT_UIF_" % key)
    wanted = sorted(PLACEHOLDER.findall(texts.get("en", "")))
    for _, code in COLUMNS:
        value = texts.get(code, "")
        if not value.strip():
            errors.append("%s: missing '%s'" % (key, code))
        elif sorted(PLACEHOLDER.findall(value)) != wanted:
            errors.append("%s: '%s' placeholders differ from en" % (key, code))
        elif '"' in value:
            errors.append("%s: '%s' contains a straight double quote" % (key, code))
if errors:
    print("stringtable NOT written:")
    for e in errors:
        print("  " + e)
    sys.exit(1)

with open(OUT, "w", encoding="utf-8", newline="") as f:
    w = csv.writer(f, quoting=csv.QUOTE_ALL, lineterminator="\n")
    w.writerow(HEADER)
    for key in sorted(strings):
        texts = strings[key]
        row = [key, texts["en"]] + [texts[code] for _, code in COLUMNS]
        w.writerow(row)
print("stringtable: %d keys x %d languages -> %s" % (len(strings), len(COLUMNS), OUT))
