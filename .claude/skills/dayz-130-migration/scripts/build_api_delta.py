"""Diff two scanner API indices (old build -> new build) into a delta json (schema of data/delta.json).

Usage:
    python -I build_api_delta.py <old-index.json> <new-index.json> <out-delta.json> [--label "1.30.164078 -> 1.30.164200"]
                                 [--md <out.md> --scripts-root P:\\scripts [--md-notes notes.md]]

Indices come from build_api_index.py. Indices built by it carry enum values ("n"), field details ("Fd"), class
attributes ("a") and global declarations ("G"); with an older compact index that lacks them, the parts that need them
(enum value shifts, field types/consts/obsolete, globals values, obsolete classes) are left empty and flagged in "_meta".

Output schema (all keys of data/delta.json, plus a few additive ones marked +):
  classes_removed / classes_added : [{"class", "file", "base", + "cond" (#ifdef guard, if any), + "line" (added only)}]
  + guard_changed                 : [{"class", "old", "new", "file", "line"}]  (#ifdef condition of the class changed)
  base_changed                    : [{"class", "old", "new", "file"}]
  class_changes[class]            : {"file",
        "methods_removed": [{"name", "old": [sig], "moved_to": ancestor|null, "new_in_ancestor": [sig], "old_line"}],
        "methods_added":   [{"name", "new": [sig], "obsolete": msg|null, "was_in_ancestor": ancestor|null, "line",
                             + "proto": bool, + "shadows": {"class", "sigs"}|null (an ancestor in the NEW build that also
                             declares it - i.e. the new member is an override of something older)}],
        "sig_changed":     [{"name", "old": [sig], "new": [sig], "line", + "old_line"}],
        "visibility_changed": [{"name", "old": [vis], "new": [vis]}],
        "became_obsolete": [{"name", "msg", "sig", "line"}],
        + "obsolete_removed": [{"name", "sig", "line"}],
        + "body_changed": [{"name", "sig", "line"}]  (same signature, different method body - behaviour change),
        "fields_added":    [{"name", "type", "init", "obsolete", + "line"}],
        "fields_removed":  [{"name", "type", "moved_to"}],
        "field_type_changed": [{"name", "old", "new"}],
        "const_changed":   [{"name", "old", "new"}],
        "field_became_obsolete": [{"name"}]}
  enums_removed : [name]
  enums_added   : [{"enum", "file", "values"}]
  enum_changes[enum] : {"file", "removed", "added", "shifted_count", "shifted_sample" (first 8),
                        + "shifted": [{"name", "old", "new"}] (all), + "line"}
  globals[name] : {"change": added|removed|changed, "old"?: [decl], "new"?: [decl], "file"}
  typedefs[name]: {"change": added|removed|changed, "old"?: target, "new"?: target}
  + classes_became_obsolete : [{"class", "msg", "at"}]   (class-level [Obsolete] attribute appeared)
  + _meta : {"label", "old", "new", "missing_enrichment": [...]}
"""
import argparse
import json
import os
import re
import sys

SAMPLE = 8
VIS = ("private", "protected")


def load(p):
    with open(p, encoding="utf-8") as fh:
        return json.load(fh)


def strip_gen(b):
    return re.sub(r"<.*", "", b).strip() if b else None


def rbase(idx, b):
    """Base as written, generics stripped and typedefs resolved (BuildingSuper -> House, Inventory_Base -> ItemBase)."""
    b = strip_gen(b)
    seen = set()
    while b and b in idx.get("typedefs", {}) and b not in seen:
        seen.add(b)
        b = strip_gen(idx["typedefs"][b])
    return b


def chain(idx, cname):
    """Ancestors of cname (excluding itself) in an index, following bases and typedefs."""
    out, seen = [], {cname}
    c = idx["classes"].get(cname)
    nxt = strip_gen(c["b"]) if c else None
    while nxt and nxt not in seen:
        seen.add(nxt)
        rec = idx["classes"].get(nxt)
        if not rec:
            td = idx.get("typedefs", {}).get(nxt)
            if td:
                nxt = strip_gen(td)
                continue
            break
        out.append(nxt)
        nxt = strip_gen(rec["b"])
    return out


def sigs(recs):
    return sorted({r["s"] for r in recs})


def vis_of(sig):
    head = sig.split("(", 1)[0].split()
    for v in VIS:
        if v in head[:-1]:
            return v
    return "public"


def obs_msg(o):
    if not o:
        return None
    m = re.match(r'^Obsolete\s*\(\s*"(.*)"\s*\)\s*$', o, re.S)
    return m.group(1) if m else re.sub(r"^Obsolete\s*", "", o).strip("()") or o


def is_proto(sig):
    head = sig.split("(", 1)[0].split()
    return "proto" in head or "native" in head


def first_line(recs):
    return recs[0]["l"] if recs else None


def class_delta(cn, o, n, old, new, enriched):
    ch = {}
    om, nm = o["m"], n["m"]
    new_chain, old_chain = chain(new, cn), chain(old, cn)
    rem, add, sigc, visc, obs, unobs, body = [], [], [], [], [], [], []
    for name in sorted(set(om) - set(nm)):
        moved = next((a for a in new_chain if name in new["classes"][a]["m"]), None)
        rem.append({"name": name, "old": sigs(om[name]), "moved_to": moved,
                    "new_in_ancestor": sigs(new["classes"][moved]["m"][name]) if moved else None,
                    "old_line": first_line(om[name])})
    for name in sorted(set(nm) - set(om)):
        was = next((a for a in old_chain if name in old["classes"][a]["m"]), None)
        shadow = next((a for a in new_chain if name in new["classes"][a]["m"]), None)
        recs = nm[name]
        add.append({"name": name, "new": sigs(recs), "obsolete": obs_msg(next((r["o"] for r in recs if r["o"]), None)),
                    "was_in_ancestor": was, "line": first_line(recs),
                    "proto": all(is_proto(r["s"]) for r in recs),
                    "shadows": {"class": shadow, "sigs": sigs(new["classes"][shadow]["m"][name])} if shadow else None})
    for name in sorted(set(om) & set(nm)):
        so, sn = sigs(om[name]), sigs(nm[name])
        if so != sn:
            sigc.append({"name": name, "old": so, "new": sn, "line": first_line(nm[name]), "old_line": first_line(om[name])})
        vo, vn = sorted({vis_of(s) for s in so}), sorted({vis_of(s) for s in sn})
        if vo != vn:
            visc.append({"name": name, "old": vo, "new": vn})
        old_obs = {r["s"] for r in om[name] if r["o"]}
        for r in nm[name]:
            if r["o"] and r["s"] not in old_obs and not any(x["o"] for x in om[name] if x["s"] == r["s"]):
                obs.append({"name": name, "msg": obs_msg(r["o"]), "sig": r["s"], "line": r["l"]})
        oh = {r["s"]: r.get("h") for r in om[name]}
        for r in nm[name]:
            if r.get("h") and oh.get(r["s"]) and oh[r["s"]] != r["h"]:
                body.append({"name": name, "sig": r["s"], "line": r["l"]})
        new_obs = {r["s"] for r in nm[name] if r["o"]}
        for r in om[name]:
            if r["o"] and r["s"] in {x["s"] for x in nm[name]} and r["s"] not in new_obs:
                unobs.append({"name": name, "sig": r["s"], "line": next(x["l"] for x in nm[name] if x["s"] == r["s"])})
    if rem:
        ch["methods_removed"] = rem
    if add:
        ch["methods_added"] = add
    if sigc:
        ch["sig_changed"] = sigc
    if visc:
        ch["visibility_changed"] = visc
    if obs:
        ch["became_obsolete"] = obs
    if unobs:
        ch["obsolete_removed"] = unobs
    if body:
        ch["body_changed"] = body
    # fields
    of, nf = set(o["F"]), set(n["F"])
    ofd, nfd = o.get("Fd") or {}, n.get("Fd") or {}
    fa, fr, ftc, cc, fbo = [], [], [], [], []
    for name in sorted(nf - of):
        d = nfd.get(name, {})
        fa.append({"name": name, "type": d.get("t"), "init": d.get("i"), "obsolete": obs_msg(d.get("o"))})
        if d.get("l"):
            fa[-1]["line"] = d["l"]
    for name in sorted(of - nf):
        d = ofd.get(name, {})
        moved = next((a for a in new_chain if name in new["classes"][a]["F"]), None)
        fr.append({"name": name, "type": d.get("t"), "moved_to": moved})
    if enriched:
        for name in sorted(of & nf):
            a, b = ofd.get(name), nfd.get(name)
            if not a or not b:
                continue
            if a["t"] != b["t"]:
                ftc.append({"name": name, "old": a["t"], "new": b["t"]})
            if "const" in (b.get("k") or []) and a.get("i") != b.get("i"):
                cc.append({"name": name, "old": a.get("i"), "new": b.get("i")})
            if b.get("o") and not a.get("o"):
                fbo.append({"name": name})
    if fa:
        ch["fields_added"] = fa
    if fr:
        ch["fields_removed"] = fr
    if ftc:
        ch["field_type_changed"] = ftc
    if cc:
        ch["const_changed"] = cc
    if fbo:
        ch["field_became_obsolete"] = fbo
    return ch


def build_delta(old, new, label=""):
    oc, nc = old["classes"], new["classes"]
    enriched = all("Fd" in c for c in list(oc.values())[:50]) and all("Fd" in c for c in list(nc.values())[:50])
    enum_vals = all("n" in e for e in old["enums"].values()) and all("n" in e for e in new["enums"].values())
    globals_d = "G" in old and "G" in new
    missing = [k for k, ok in (("field details (Fd)", enriched), ("enum values (n)", enum_vals), ("global decls (G)", globals_d)) if not ok]
    d = {"classes_removed": [dict({"class": c, "file": oc[c]["f"], "base": rbase(old, oc[c]["b"])}, **({"cond": oc[c]["c"]} if oc[c].get("c") else {}))
                            for c in sorted(set(oc) - set(nc))],
         "classes_added": [dict({"class": c, "file": nc[c]["f"], "base": rbase(new, nc[c]["b"])}, **({"cond": nc[c]["c"]} if nc[c].get("c") else {}),
                                **({"line": nc[c]["l"]} if nc[c].get("l") else {}))
                          for c in sorted(set(nc) - set(oc))],
         "base_changed": [{"class": c, "old": rbase(old, oc[c]["b"]), "new": rbase(new, nc[c]["b"]), "file": nc[c]["f"]}
                          for c in sorted(set(oc) & set(nc)) if rbase(old, oc[c]["b"]) != rbase(new, nc[c]["b"])],
         "class_changes": {}}
    for c in sorted(set(oc) & set(nc)):
        ch = class_delta(c, oc[c], nc[c], old, new, enriched)
        if ch:
            d["class_changes"][c] = dict({"file": nc[c]["f"]}, **ch)
    # classes that moved under / out of a preprocessor guard (e.g. #ifdef WORKBENCH in 1.30.164200)
    d["guard_changed"] = [{"class": c, "old": oc[c].get("c", ""), "new": nc[c].get("c", ""), "file": nc[c]["f"], "line": nc[c].get("l")}
                          for c in sorted(set(oc) & set(nc)) if oc[c].get("c", "") != nc[c].get("c", "")]
    # enums
    oe, ne = old["enums"], new["enums"]
    d["enums_removed"] = sorted(set(oe) - set(ne))
    d["enums_added"] = [{"enum": e, "file": ne[e]["f"], "values": ne[e]["v"]} for e in sorted(set(ne) - set(oe))]
    d["enum_changes"] = {}
    for e in sorted(set(oe) & set(ne)):
        ov, nv = oe[e]["v"], ne[e]["v"]
        removed = [v for v in ov if v not in nv]
        added = [v for v in nv if v not in ov]
        shifted = []
        if enum_vals:
            on = dict(zip(ov, oe[e]["n"]))
            nn = dict(zip(nv, ne[e]["n"]))
            for v in ov:
                if v in nn and on[v] != nn[v]:
                    shifted.append({"name": v, "old": on[v], "new": nn[v]})
        if removed or added or shifted:
            rec = {"file": ne[e]["f"], "removed": removed, "added": added, "shifted_count": len(shifted),
                   "shifted_sample": shifted[:SAMPLE], "shifted": shifted}
            if "l" in ne[e]:
                rec["line"] = f"{ne[e]['f']}:{ne[e]['l']}"
            if oe[e].get("b") != ne[e].get("b"):
                rec["base_changed"] = {"old": oe[e].get("b"), "new": ne[e].get("b")}
            d["enum_changes"][e] = rec
    # globals
    d["globals"] = {}
    og, ng = set(old["globals"]), set(new["globals"])
    OG, NG = old.get("G", {}), new.get("G", {})

    def gdecl(recs):
        return sorted({r["s"] for r in recs})

    def gfile(recs):
        return recs[0]["l"].rsplit(":", 1)[0] if recs else None

    for g in sorted(ng - og):
        d["globals"][g] = {"change": "added", "new": gdecl(NG.get(g, [])), "file": gfile(NG.get(g, []))}
        if NG.get(g):
            d["globals"][g]["line"] = NG[g][0]["l"]
    for g in sorted(og - ng):
        d["globals"][g] = {"change": "removed", "old": gdecl(OG.get(g, [])), "file": gfile(OG.get(g, []))}
    if globals_d:
        for g in sorted(og & ng):
            a, b = gdecl(OG.get(g, [])), gdecl(NG.get(g, []))
            if a != b:
                d["globals"][g] = {"change": "changed", "old": a, "new": b, "file": gfile(NG[g]), "line": NG[g][0]["l"]}
    # typedefs
    d["typedefs"] = {}
    ot, nt = old.get("typedefs", {}), new.get("typedefs", {})
    for t in sorted(set(ot) | set(nt)):
        if t not in ot:
            d["typedefs"][t] = {"change": "added", "new": nt[t]}
        elif t not in nt:
            d["typedefs"][t] = {"change": "removed", "old": ot[t]}
        elif ot[t] != nt[t]:
            d["typedefs"][t] = {"change": "changed", "old": ot[t], "new": nt[t]}
    # class-level [Obsolete]
    d["classes_became_obsolete"] = []
    for c in sorted(nc):
        na = [a for a in nc[c].get("a", []) if a.startswith("Obsolete")]
        oa = [a for a in oc.get(c, {}).get("a", []) if a.startswith("Obsolete")] if c in oc else []
        if na and not oa:
            d["classes_became_obsolete"].append({"class": c, "msg": na[0], "at": nc[c].get("l") or nc[c]["f"]})
    d["_meta"] = {"label": label, "missing_enrichment": missing}
    return d


# ------------------------------------------------------------------ markdown
def md_report(d, label, scripts_root, notes=None):
    root = scripts_root.rstrip("\\/")

    def loc(l):
        if not l:
            return ""
        return f"`{root}\\{l.replace('/', chr(92))}`"

    L = [f"# DayZ script API delta {label}", "",
         "Generated by `scripts/build_api_delta.py` from the two API indices; `path:line` = the NEW build "
         f"(`{root}`). Signatures show parameter types only (`=x` marks a default).", ""]
    if notes:
        L += [notes.rstrip(), ""]
    L += ["## 1. Removed members (compile breakers for code that overrides or calls them)", ""]
    rows = []
    for c, ch in d["class_changes"].items():
        for r in ch.get("methods_removed", []):
            rows.append(f"| `{c}` | method `{r['name']}` | `{'; '.join(r['old'])}` | "
                        + (f"still in ancestor `{r['moved_to']}`: `{'; '.join(r['new_in_ancestor'])}`" if r["moved_to"] else "**gone**")
                        + f" | old build: `{r['old_line']}` |")
        for r in ch.get("fields_removed", []):
            rows.append(f"| `{c}` | field `{r['name']}` | `{r['type']}` | "
                        + (f"still in ancestor `{r['moved_to']}`" if r["moved_to"] else "**gone**") + " | |")
    for r in d["classes_removed"]:
        rows.append(f"| `{r['class']}` | class | base `{r['base'] or '-'}` | **gone** | old build: `{r['file']}` |")
    L += (["| Class | Member | Old declaration | Now | Where |", "|---|---|---|---|---|"] + rows) if rows else ["(none)"]
    L += ["", "## 2. Changed signatures (an `override` with the old parameter list no longer compiles)", ""]
    rows = []
    for c, ch in d["class_changes"].items():
        for r in ch.get("sig_changed", []):
            rows.append(f"| `{c}.{r['name']}` | `{'; '.join(r['old'])}` | `{'; '.join(r['new'])}` | {loc(r['line'])} |")
    L += (["| Member | Old | New | Where (new) |", "|---|---|---|---|"] + rows) if rows else ["(none)"]
    vis = [(c, r) for c, ch in d["class_changes"].items() for r in ch.get("visibility_changed", [])]
    if vis:
        L += ["", "Visibility changes: " + "; ".join(f"`{c}.{r['name']}` {'/'.join(r['old'])} → {'/'.join(r['new'])}" for c, r in vis)]
    L += ["", "## 3. New members on existing classes", "",
          "A mod method with the same name declared WITHOUT `override` in a subclass now shadows the vanilla one "
          "(compile error for proto/native, otherwise a hidden-override bug).", ""]
    rows = []
    for c, ch in d["class_changes"].items():
        for r in ch.get("methods_added", []):
            note = []
            if r.get("proto"):
                note.append("**proto/native**")
            if r.get("shadows"):
                note.append(f"overrides `{r['shadows']['class']}`")
            if r.get("was_in_ancestor"):
                note.append(f"was only in `{r['was_in_ancestor']}`")
            if r.get("obsolete"):
                note.append(f"[Obsolete: {r['obsolete']}]")
            rows.append(f"| `{c}` | `{'; '.join(r['new'])}` | {', '.join(note)} | {loc(r['line'])} |")
        for r in ch.get("fields_added", []):
            rows.append(f"| `{c}` | field `{r['type']} {r['name']}`" + (f" = `{r['init']}`" if r.get("init") else "") + f" | | {loc(r.get('line'))} |")
    L += (["| Class | Declaration | Note | Where |", "|---|---|---|---|"] + rows) if rows else ["(none)"]
    obs = [(c, r) for c, ch in d["class_changes"].items() for r in ch.get("became_obsolete", [])]
    unobs = [(c, r) for c, ch in d["class_changes"].items() for r in ch.get("obsolete_removed", [])]
    if obs or unobs or d.get("classes_became_obsolete"):
        L += ["", "## 3b. Obsolete markers", ""]
        L += [f"- `{c}.{r['sig']}` became `[Obsolete(\"{r['msg']}\")]` — {loc(r['line'])}" for c, r in obs]
        L += [f"- `{c}.{r['sig']}` is no longer obsolete — {loc(r['line'])}" for c, r in unobs]
        L += [f"- class `{r['class']}` became {r['msg']} — {loc(r['at'])}" for r in d.get("classes_became_obsolete", [])]
    misc = [(c, k, r) for c, ch in d["class_changes"].items() for k in ("field_type_changed", "const_changed", "field_became_obsolete")
            for r in ch.get(k, [])]
    if misc:
        L += ["", "## 3c. Field type / constant changes", ""]
        for c, k, r in misc:
            if k == "field_became_obsolete":
                L.append(f"- `{c}.{r['name']}` became obsolete")
            else:
                L.append(f"- `{c}.{r['name']}` ({k.replace('_', ' ')}): `{r['old']}` → `{r['new']}`")
    if d.get("guard_changed"):
        L += ["", "## 3d. Classes that moved under / out of a preprocessor guard", "",
              "Code outside the same `#ifdef` no longer sees these classes (compile error `Can't find variable`).", ""]
        L += [f"- `{r['class']}`: `{r['old'] or '(no guard)'}` → `#ifdef {r['new']}`" if r["new"] else
              f"- `{r['class']}`: `#ifdef {r['old']}` → (no guard)" for r in d["guard_changed"]]
        L[-len(d["guard_changed"]):] = [x + f" — {loc(r['line'])}" for x, r in zip(L[-len(d["guard_changed"]):], d["guard_changed"])]
    body = [(c, r) for c, ch in d["class_changes"].items() for r in ch.get("body_changed", [])]
    if body:
        L += ["", "## 3e. Same signature, changed body (behaviour changes)", "",
              "An override that replaces one of these without calling `super` (typically a copy of the old vanilla code) keeps the "
              "old behaviour; one that calls `super` gets the new one.", "",
              "| Method | Where (new) |", "|---|---|"]
        L += [f"| `{c}.{r['sig']}` | {loc(r['line'])} |" for c, r in body]
    L += ["", "## 4. Enums", ""]
    if d["enums_removed"]:
        L.append("Removed enums: " + ", ".join(f"`{e}`" for e in d["enums_removed"]))
    for e, r in d["enum_changes"].items():
        L.append(f"### `{e}` — {loc(r.get('line') or r['file'])}")
        if r["removed"]:
            L.append(f"- removed: {', '.join(r['removed'])}")
        if r["added"]:
            L.append(f"- added: {', '.join(r['added'])}")
        if r["shifted_count"]:
            L.append(f"- **{r['shifted_count']} existing values changed their number** (code that stores/sends/compares raw ints breaks):")
            L.append("")
            L.append("  | Value | Old | New |")
            L.append("  |---|---|---|")
            L += [f"  | `{s['name']}` | {s['old']} | {s['new']} |" for s in r.get("shifted", r["shifted_sample"])]
        L.append("")
    for r in d["enums_added"]:
        L.append(f"- new enum `{r['enum']}` ({r['file']}): {', '.join(r['values'])}")
    L += ["", "## 5. New classes", ""]
    L += [f"- `{r['class']}`" + (f" : `{r['base']}`" if r["base"] else "") + (f" (only under `#ifdef {r['cond']}`)" if r.get("cond") else "")
          + f" — {loc(r.get('line') or r['file'])}" for r in d["classes_added"]] or ["(none)"]
    if d["base_changed"]:
        L += ["", "## 6. Base class changes", ""]
        L += [f"- `{r['class']}`: `{r['old']}` → `{r['new']}` ({r['file']})" for r in d["base_changed"]]
    if d["globals"] or d["typedefs"]:
        L += ["", "## 7. Globals / typedefs", ""]
        for g, r in d["globals"].items():
            L.append(f"- `{g}` {r['change']}: " + (f"`{'; '.join(r.get('old', []))}` → " if r.get("old") else "")
                     + f"`{'; '.join(r.get('new', []))}`" + (f" — {loc(r.get('line'))}" if r.get("line") else ""))
        for t, r in d["typedefs"].items():
            L.append(f"- typedef `{t}` {r['change']}: {r.get('old', '')} → {r.get('new', '')}")
    return "\n".join(L) + "\n"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("old")
    ap.add_argument("new")
    ap.add_argument("out")
    ap.add_argument("--label", default="")
    ap.add_argument("--md")
    ap.add_argument("--scripts-root", default="P:\\scripts")
    ap.add_argument("--md-notes")
    a = ap.parse_args()
    old, new = load(a.old), load(a.new)
    d = build_delta(old, new, a.label)
    d["_meta"]["old"] = os.path.basename(a.old)
    d["_meta"]["new"] = os.path.basename(a.new)
    with open(a.out, "w", encoding="utf-8") as fh:
        json.dump(d, fh, indent=1, ensure_ascii=False)
    if a.md:
        notes = open(a.md_notes, encoding="utf-8").read() if a.md_notes else None
        with open(a.md, "w", encoding="utf-8") as fh:
            fh.write(md_report(d, a.label, a.scripts_root, notes))
    cc = d["class_changes"]
    n = lambda k: sum(len(c.get(k, [])) for c in cc.values())  # noqa: E731
    print(f"classes -{len(d['classes_removed'])} +{len(d['classes_added'])} base~{len(d['base_changed'])} | changed classes {len(cc)}: "
          f"methods -{n('methods_removed')} +{n('methods_added')} sig~{n('sig_changed')} obsolete+{n('became_obsolete')} | "
          f"fields -{n('fields_removed')} +{n('fields_added')} | enums -{len(d['enums_removed'])} +{len(d['enums_added'])} "
          f"changed {len(d['enum_changes'])} | globals {len(d['globals'])} | missing: {d['_meta']['missing_enrichment'] or 'none'} -> {a.out}")


if __name__ == "__main__":
    sys.exit(main())
