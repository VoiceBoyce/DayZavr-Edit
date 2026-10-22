"""Compare two API indices on what the scanner reads (classes b/f/F/m without body hashes, enums b/v/f, globals, typedefs).

Usage: python -I compare_api_index.py <a.json> <b.json> [--max 80]
Exit 0 when they match, 1 otherwise. Use it to check an index built from a DayZ-Script-Diff tree against the one
built from P:\\scripts (same build => 0 differences).
"""
import argparse
import json
import sys


def strip_m(m):
    return {k: [{kk: vv for kk, vv in r.items() if kk != "h"} for r in v] for k, v in m.items()}


def compare(a, b):
    diffs = []
    ca, cb = a["classes"], b["classes"]
    for n in sorted(set(ca) | set(cb)):
        if n not in ca or n not in cb:
            diffs.append(f"class only in {'B' if n not in ca else 'A'}: {n}")
            continue
        x, y = ca[n], cb[n]
        for k in ("b", "f", "F"):
            if x.get(k) != y.get(k):
                diffs.append(f"class {n}.{k}: {str(x.get(k))[:200]} != {str(y.get(k))[:200]}")
        mx, my = strip_m(x["m"]), strip_m(y["m"])
        for mn in sorted(set(mx) | set(my)):
            if mx.get(mn) != my.get(mn):
                diffs.append(f"method {n}.{mn}: {json.dumps(mx.get(mn))[:300]} != {json.dumps(my.get(mn))[:300]}")
    ea, eb = a["enums"], b["enums"]
    for n in sorted(set(ea) | set(eb)):
        if n not in ea or n not in eb:
            diffs.append(f"enum only in {'B' if n not in ea else 'A'}: {n}")
            continue
        for k in ("b", "v", "f"):
            if ea[n].get(k) != eb[n].get(k):
                diffs.append(f"enum {n}.{k}: {ea[n].get(k)} != {eb[n].get(k)}")
    if sorted(a["globals"]) != sorted(b["globals"]):
        diffs.append(f"globals: A-B {sorted(set(a['globals']) - set(b['globals']))} B-A {sorted(set(b['globals']) - set(a['globals']))}")
    if a["typedefs"] != b["typedefs"]:
        diffs.append(f"typedefs differ: {sorted(set(a['typedefs'].items()) ^ set(b['typedefs'].items()))[:20]}")
    return diffs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("a")
    ap.add_argument("b")
    ap.add_argument("--max", type=int, default=80)
    o = ap.parse_args()
    a = json.load(open(o.a, encoding="utf-8"))
    b = json.load(open(o.b, encoding="utf-8"))
    diffs = compare(a, b)
    sys.stdout.reconfigure(encoding="utf-8")
    print(f"differences: {len(diffs)}")
    for d in diffs[:o.max]:
        print(d)
    sys.exit(1 if diffs else 0)


if __name__ == "__main__":
    main()
