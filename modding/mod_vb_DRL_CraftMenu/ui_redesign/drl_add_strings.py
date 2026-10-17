"""Добавляет/обновляет строки нового интерфейса в stringtable.csv DRL_CraftMenu.
Формат файла мода: 18 колонок, UTF-8 без BOM, CRLF, все поля в кавычках.
Использование: python drl_add_strings.py <stringtable.csv> <strings.tsv>
strings.tsv: KEY<TAB>english<TAB>russian
"""
import csv, io, sys

path, tsv = sys.argv[1], sys.argv[2]
with open(path, encoding="utf-8", newline="") as f:
    rows = list(csv.reader(f))
header = rows[0]
en = header.index("english"); ru = header.index("russian"); orig = header.index("original")
index = {r[0]: i for i, r in enumerate(rows) if r}

added = updated = 0
for line in open(tsv, encoding="utf-8"):
    line = line.rstrip("\r\n")
    if not line or line.startswith("#"):
        continue
    key, english, russian = line.split("\t")
    row = [english] * len(header)
    row[0] = key
    row[orig] = english
    row[en] = english
    row[ru] = russian
    if "ukrainian" in header:
        row[header.index("ukrainian")] = russian
    if key in index:
        rows[index[key]] = row
        updated += 1
    else:
        rows.append(row)
        index[key] = len(rows) - 1
        added += 1

out = io.StringIO()
w = csv.writer(out, quoting=csv.QUOTE_ALL, lineterminator="\r\n")
w.writerows(rows)
with open(path, "w", encoding="utf-8", newline="") as f:
    f.write(out.getvalue())
print(f"added={added} updated={updated} total={len(rows) - 1}")
