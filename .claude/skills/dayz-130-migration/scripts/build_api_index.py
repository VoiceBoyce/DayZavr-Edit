"""Build the scanner's compact API index from a DayZ scripts tree (vanilla P:\\scripts or a DayZ-Script-Diff tarball).

Usage:
    python -I build_api_index.py <scripts_root> <out.json> [--only-files-in <ref_root>] [--files-of-index <index.json>]
                                 [--keep <rel> ...] [--exclude <rel> ...] [--full <full.json>]

  <scripts_root>     folder that holds 1_core/ 2_gamelib/ 3_game/ 4_world/ 5_mission/ (e.g. P:\\scripts)
  --only-files-in    keep only .c files whose relative path also exists under <ref_root>. Use it for GitHub
                     DayZ-Script-Diff trees: that repo keeps stale files the game PBO no longer ships
                     (missionbenchmark.c, vehiclemanager.c ...). Pass the matching P:\\scripts as <ref_root>.
  --files-of-index   keep only the .c files listed in an index this script built earlier (its "files" key) - the
                     exact file set of that build. Combine with --only-files-in as needed.
  --keep             relative paths kept even when a filter above would drop them (e.g. files the NEW patch
                     deleted: they are in the old build but no longer under P:\\scripts).
  --exclude          relative paths (forward slashes) to drop, e.g. files the NEXT patch added (they exist in
                     P:\\scripts but not in the old build).
  --full             also write the raw es_parse model (decls/methods/fields with conditions) for debugging.

Index schema (the scanner reads b/f/m/F, enums b/v/f, globals, typedefs; the rest feeds build_api_delta.py):
  classes[name] = {"b": base, "f": first decl file, "m": {method: [{"p": [param types], "d": #defaults,
                   "s": signature, "l": "file:line", "o": "Obsolete(...)" | null, "h": body hash | null}]},
                   "F": [field names, sorted], "c": #ifdef condition of the first declaration (only if any),
                   "Fd": {field: {"t": type, "i": initializer, "k": [modifiers], "o": obsolete attr, "l": "file:line"}},
                   "a": [class attributes], "l": "file:line", "md": true if every decl is `modded class`}
  enums[name]   = {"b": base enum, "v": [value names in order], "f": file, "l": line, "x": [raw initializers],
                   "n": [resolved int values or null]}
  globals       = [names]           (functions + global variables/constants)
  G[name]       = [{"s": signature or "type = init", "l": "file:line", "o": obsolete attr}]
  typedefs[name]= target
  files         = [relative .c paths parsed, sorted]  (the build's file set)
"""
import argparse
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from es_parse import Parser  # noqa: E402


def rel_c_files(root):
    out = []
    for dp, dn, fn in os.walk(root):
        dn.sort()
        for f in sorted(fn):
            if f.lower().endswith(".c"):
                out.append(os.path.relpath(os.path.join(dp, f), root).replace(os.sep, "/"))
    return out


def obsolete_attr(attrs):
    for a in attrs or []:
        if a.startswith("Obsolete"):
            return a
    return None


_INT = re.compile(r"^[+-]?(0x[0-9A-Fa-f]+|\d+)$")


def eval_int(expr, env):
    """Evaluate a simple enum initializer (ints, hex, names of earlier values, + - | << >> ( ))."""
    if expr is None:
        return None
    e = expr.strip()
    if _INT.match(e):
        return int(e, 0)
    toks = re.findall(r"0x[0-9A-Fa-f]+|\d+|[A-Za-z_]\w*(?:\.[A-Za-z_]\w*)?|<<|>>|[-+|&()~*]", e)
    if "".join(toks) != re.sub(r"\s+", "", e):
        return None
    py = []
    for t in toks:
        if re.match(r"^(0x[0-9A-Fa-f]+|\d+)$", t):
            py.append(str(int(t, 0)))
        elif re.match(r"^[A-Za-z_]", t):
            head, _, last = t.rpartition(".")
            v = env.get(t) if not head else (env.get(last) if head in (env.get("__enum__"), env.get("__base__")) else None)
            if v is None:
                return None
            py.append(str(v))
        else:
            py.append(t)
    try:
        return int(eval("".join(py), {"__builtins__": {}}, {}))  # noqa: S307 - digits/operators only
    except Exception:
        return None


def resolve_enum_values(vals, base=None, ename=None, bname=None):
    """vals = [(name, initializer or None)]; base = (names, values) of the base enum, whose last value the
    derived enum continues from. Unknown initializers make the value (and the implicit ones after it) None."""
    env = {"__enum__": ename, "__base__": bname}
    nxt = 0
    if base:
        env.update({n: v for n, v in zip(*base) if v is not None})
        known = [v for v in base[1] if v is not None]
        nxt = (known[-1] + 1) if known else 0
    out = []
    for name, expr in vals:
        v = (nxt if nxt is not None else None) if expr is None else eval_int(expr, env)
        out.append(v)
        if v is not None:
            env[name] = v
        nxt = None if v is None else v + 1
    return out


def compact(p):
    classes = {}
    for cname, c in p.classes.items():
        decls = c["decls"]
        base = next((d["base"] for d in decls if d["base"]), None)
        m = {}
        for mname, recs in c["methods"].items():
            m[mname] = [{"p": [x["type"] for x in r["params"]],
                         "d": sum(1 for x in r["params"] if x["default"] is not None),
                         "s": r["sig"], "l": f"{r['file']}:{r['line']}", "o": obsolete_attr(r["attrs"]),
                         "h": r.get("body")} for r in recs]
        fd = {n: {"t": r["type"], "i": r["init"], "k": r["mods"], "o": obsolete_attr(r["attrs"]), "l": f"{r['file']}:{r['line']}"}
              for n, r in c["fields"].items()}
        attrs = []
        for d in decls:
            for a in d.get("attrs") or []:
                if a not in attrs:
                    attrs.append(a)
        rec = {"b": base, "f": decls[0]["file"] if decls else None, "m": m, "F": sorted(c["fields"].keys()),
               "Fd": fd, "a": attrs, "l": f"{decls[0]['file']}:{decls[0]['line']}" if decls else None}
        if decls and all(d["modded"] for d in decls):
            rec["md"] = True
        if decls and decls[0]["cond"]:
            rec["c"] = decls[0]["cond"]
        classes[cname] = rec
    enums = {}
    for ename, e in p.enums.items():
        enums[ename] = {"b": e["base"], "v": [v[0] for v in e["values"]], "f": e["file"], "l": e["line"],
                        "x": [v[1] for v in e["values"]]}
        if e.get("cond"):
            enums[ename]["c"] = e["cond"]
    # resolve values (base enums first)
    done = {}

    def res(name, seen=()):
        if name in done:
            return done[name]
        e = enums.get(name)
        if not e or name in seen:
            return None
        bv = res(e["b"], seen + (name,)) if e["b"] else None
        vals = resolve_enum_values(list(zip(e["v"], e["x"])), (enums[e["b"]]["v"], bv) if bv is not None else None,
                                   name, e["b"])
        done[name] = vals
        return vals

    for ename in enums:
        enums[ename]["n"] = res(ename)
    G = {}
    for gname, recs in p.globals.items():
        out = []
        for r in recs:
            if r.get("field"):
                s = r["type"] + (" = " + r["init"] if r.get("init") else "")
            else:
                s = r["sig"]
            out.append({"s": s, "l": f"{r['file']}:{r['line']}", "o": obsolete_attr(r.get("attrs"))})
        G[gname] = out
    return {"classes": classes, "typedefs": {k: v["target"] for k, v in p.typedefs.items()}, "enums": enums,
            "globals": sorted(p.globals.keys()), "G": G}


def build(root, only_in=None, exclude=(), files_of_index=None, keep=()):
    files = rel_c_files(root)
    kept = {k.replace("\\", "/").lower() for k in keep}
    if only_in:
        ref = set(rel_c_files(only_in))
        files = [f for f in files if f in ref or f.lower() in kept]
    if files_of_index:
        with open(files_of_index, encoding="utf-8") as fh:
            listed = {f.lower() for f in json.load(fh)["files"]}
        files = [f for f in files if f.lower() in listed or f.lower() in kept]
    ex = {e.replace("\\", "/").lower() for e in exclude}
    files = [f for f in files if f.lower() not in ex]
    p = Parser(root)
    for f in files:
        p.parse_file(os.path.join(root, f))
    return p, files


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("root")
    ap.add_argument("out")
    ap.add_argument("--only-files-in")
    ap.add_argument("--exclude", nargs="*", default=[])
    ap.add_argument("--keep", nargs="*", default=[])
    ap.add_argument("--files-of-index")
    ap.add_argument("--full")
    a = ap.parse_args()
    p, files = build(a.root, a.only_files_in, a.exclude, a.files_of_index, a.keep)
    idx = compact(p)
    idx["files"] = sorted(files)
    with open(a.out, "w", encoding="utf-8") as fh:
        json.dump(idx, fh, separators=(",", ":"), ensure_ascii=False)
    if a.full:
        with open(a.full, "w", encoding="utf-8") as fh:
            json.dump({"classes": p.classes, "enums": p.enums, "globals": p.globals, "typedefs": p.typedefs}, fh,
                      indent=0, ensure_ascii=False)
    nm = sum(len(v) for c in p.classes.values() for v in c["methods"].values())
    print(f"files={len(files)} classes={len(p.classes)} methods={nm} enums={len(p.enums)} globals={len(p.globals)} -> {a.out}")


if __name__ == "__main__":
    main()
