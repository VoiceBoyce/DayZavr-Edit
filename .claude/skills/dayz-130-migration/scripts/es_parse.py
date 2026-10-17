"""Extract a declaration-level API model from an Enforce Script tree (DayZ scripts).

Usage: python -I es_api.py <scripts_root> <out.json>
Output: {"classes": {...}, "enums": {...}, "globals": {...}, "typedefs": {...}}
Every declaration carries file (relative), line and the preprocessor condition stack.
"""
import json
import os
import re
import sys

IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
MODIFIERS = {
    "static", "override", "proto", "native", "private", "protected", "event", "external",
    "owned", "ref", "const", "volatile", "notnull", "autoptr", "sealed", "modded", "reference",
    "local", "out", "inout", "void",
}
METHOD_MODS = {"static", "override", "proto", "native", "private", "protected", "event", "external", "sealed", "volatile"}


def clean(src):
    """Blank comments and string contents, keep newlines so line numbers survive."""
    out = []
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if c == "/" and i + 1 < n and src[i + 1] == "/":
            j = src.find("\n", i)
            j = n if j < 0 else j
            out.append(" " * (j - i))
            i = j
        elif c == "/" and i + 1 < n and src[i + 1] == "*":
            j = src.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("".join(ch if ch == "\n" else " " for ch in src[i:j]))
            i = j
        elif c == '"':
            j = i + 1
            while j < n and src[j] != '"':
                if src[j] == "\\":
                    j += 1
                if j < n and src[j] == "\n":
                    break
                j += 1
            seg = src[i:j + 1]
            # keep the string literal itself for attribute text (Obsolete messages); blank braces/semicolons inside
            out.append(seg.replace("{", "(").replace("}", ")").replace(";", ","))
            i = j + 1
        elif c == "'":
            j = i + 1
            while j < n and src[j] != "'" and src[j] != "\n":
                if src[j] == "\\":
                    j += 1
                j += 1
            out.append("'" + " " * max(0, j - i - 1) + "'")
            i = j + 1
        else:
            out.append(c)
            i += 1
    return "".join(out)


def split_top(s, sep=","):
    parts, depth, cur = [], 0, []
    for ch in s:
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
        if ch == sep and depth == 0:
            parts.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
    if "".join(cur).strip():
        parts.append("".join(cur))
    return parts


def norm(s):
    s = re.sub(r"\s+", " ", s).strip()
    s = re.sub(r"\s*([<>,()\[\]])\s*", r"\1", s)
    s = s.replace(",", ", ")
    return s


def parse_param(p):
    p = p.strip()
    if not p:
        return None
    default = None
    eq = None
    depth = 0
    for k, ch in enumerate(p):
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
        elif ch == "=" and depth == 0:
            eq = k
            break
    if eq is not None:
        default = norm(p[eq + 1:])
        p = p[:eq].strip()
    p = re.sub(r"\[[^\]]*\]\s*$", "[]", p)  # name[3] -> name[]
    toks = p.split()
    arr = ""
    if toks and toks[-1].endswith("[]"):
        arr = "[]"
        toks[-1] = toks[-1][:-2]
    if len(toks) >= 2 and IDENT.fullmatch(toks[-1]):
        name = toks[-1]
        typ = norm(" ".join(toks[:-1])) + arr
    else:
        name = ""
        typ = norm(" ".join(toks)) + arr
    return {"type": typ, "name": name, "default": default}


def strip_attrs(stmt):
    attrs = []
    s = stmt.lstrip()
    while s.startswith("["):
        depth = 0
        for k, ch in enumerate(s):
            if ch == "[":
                depth += 1
            elif ch == "]":
                depth -= 1
                if depth == 0:
                    attrs.append(norm(s[1:k]))
                    s = s[k + 1:].lstrip()
                    break
        else:
            break
    return attrs, s


class Parser:
    def __init__(self, root):
        self.root = root
        self.classes = {}
        self.enums = {}
        self.globals = {}
        self.typedefs = {}

    def cls(self, name):
        return self.classes.setdefault(name, {"decls": [], "methods": {}, "fields": {}})

    def parse_file(self, path):
        rel = os.path.relpath(path, self.root).replace("\\", "/")
        with open(path, "r", encoding="utf-8", errors="replace") as fh:
            raw = fh.read()
        text = clean(raw)
        lines = text.split("\n")
        # preprocessor: blank lines starting with #, record condition per line
        cond_stack = []
        line_cond = []
        for idx, ln in enumerate(lines):
            st = ln.strip()
            if st.startswith("#"):
                m = re.match(r"#\s*(ifdef|ifndef|else|endif|if|define|undef|include)\b\s*(.*)", st)
                if m:
                    d, arg = m.group(1), m.group(2).strip()
                    if d == "ifdef":
                        cond_stack.append(arg)
                    elif d == "ifndef":
                        cond_stack.append("!" + arg)
                    elif d == "if":
                        cond_stack.append("(" + arg + ")")
                    elif d == "else" and cond_stack:
                        top = cond_stack.pop()
                        cond_stack.append(top[1:] if top.startswith("!") else "!" + top)
                    elif d == "endif" and cond_stack:
                        cond_stack.pop()
                line_cond.append(" && ".join(cond_stack))
                lines[idx] = " " * len(ln)
            else:
                line_cond.append(" && ".join(cond_stack))
                if re.match(r"^\s*typedef\b[^;{]*$", ln):
                    lines[idx] = ln + ";"  # vanilla 1.29 has a typedef without ';' (inventorygrid.c)
        text = "\n".join(lines)
        self._scan(text, rel, line_cond)

    def _scan(self, text, rel, line_cond):
        ctx = [("global", None)]
        buf = []
        buf_start = None
        i, n = 0, len(text)
        line = 1
        while i < n:
            c = text[i]
            if c == "\n":
                line += 1
            kind, name = ctx[-1]
            if kind in ("global", "class"):
                if c == "{" or c == ";":
                    stmt = "".join(buf)
                    sline = buf_start or line
                    cond = line_cond[sline - 1] if sline - 1 < len(line_cond) else ""
                    buf, buf_start = [], None
                    if c == ";":
                        self._decl(stmt, ctx, rel, sline, cond, terminated=True)
                    else:
                        res = self._decl(stmt, ctx, rel, sline, cond, terminated=False)
                        if res and res[0] == "class":
                            ctx.append(("class", res[1]))
                        elif res and res[0] == "enum":
                            j, depth = i + 1, 1
                            while j < n and depth:
                                if text[j] == "{":
                                    depth += 1
                                elif text[j] == "}":
                                    depth -= 1
                                j += 1
                            body = text[i + 1:j - 1]
                            self._enum_body(res[1], body)
                            line += text[i:j].count("\n")
                            i = j
                            continue
                        else:
                            # method body or initializer list: skip to matching brace
                            j, depth = i + 1, 1
                            while j < n and depth:
                                if text[j] == "{":
                                    depth += 1
                                elif text[j] == "}":
                                    depth -= 1
                                j += 1
                            line += text[i:j].count("\n")
                            i = j
                            if res and res[0] == "init":
                                # field with brace initializer: consume up to ';'
                                k = text.find(";", i)
                                if k >= 0:
                                    line += text[i:k].count("\n")
                                    i = k + 1
                            continue
                elif c == "}":
                    buf, buf_start = [], None
                    if len(ctx) > 1:
                        ctx.pop()
                else:
                    if not c.isspace() and buf_start is None:
                        buf_start = line
                    buf.append(c)
            i += 1

    def _enum_body(self, ename, body):
        vals = []
        for part in split_top(body):
            part = norm(part)
            if not part:
                continue
            k, v = (part.split("=", 1) + [None])[:2] if "=" in part else (part, None)
            names = k.split()
            # vanilla 1.30 dayzplayer.c:767 has `CMD_ACTIONMOD_WASHFACE` with no trailing comma; the compiler accepts it
            for extra in names[:-1]:
                if IDENT.fullmatch(extra):
                    vals.append([extra, None])
            vals.append([names[-1] if names else k.strip(), v.strip() if v is not None else None])
        self.enums[ename]["values"] = vals

    def _decl(self, stmt, ctx, rel, line, cond, terminated):
        attrs, s = strip_attrs(stmt)
        s = norm(s)
        if not s:
            return None
        kind, cname = ctx[-1]
        m = re.match(r"^((?:modded |sealed )*)class ([A-Za-z_]\w*)(?:<[^>]*>)?\s*(?:(?:extends|:)\s*([A-Za-z_][\w<>, ]*))?$", s)
        if m:
            if terminated:
                return None  # forward declaration
            name = m.group(2)
            base = (m.group(3) or "").strip()
            if " class " in base:  # #ifdef-split header: FEATURE_* branch is the defined one
                base = base.split(" class ")[0].strip()
            c = self.cls(name)
            c["decls"].append({"file": rel, "line": line, "base": base or None,
                               "modded": "modded" in m.group(1), "cond": cond, "attrs": attrs})
            return ("class", name)
        m = re.match(r"^enum ([A-Za-z_]\w*)(?:\s*:\s*(\w+))?$", s)
        if m:
            self.enums[m.group(1)] = {"file": rel, "line": line, "base": m.group(2), "cond": cond, "values": []}
            return ("enum", m.group(1))
        if s.startswith("typedef "):
            m = re.match(r"^typedef (.+) ([A-Za-z_]\w*)$", s)
            if m:
                self.typedefs[m.group(2)] = {"target": m.group(1), "file": rel, "line": line}
            return None
        # method vs field: find first '(' at angle depth 0 not preceded by '='
        depth = 0
        paren = None
        eqpos = None
        for k, ch in enumerate(s):
            if ch == "<":
                depth += 1
            elif ch == ">":
                depth -= 1
            elif ch == "=" and depth == 0 and eqpos is None:
                eqpos = k
            elif ch == "(" and depth == 0:
                paren = k
                break
        owner = cname if kind == "class" else "<global>"
        if paren is not None and (eqpos is None or eqpos > paren):
            head = s[:paren].strip()
            # find matching paren
            d, end = 0, None
            for k in range(paren, len(s)):
                if s[k] == "(":
                    d += 1
                elif s[k] == ")":
                    d -= 1
                    if d == 0:
                        end = k
                        break
            if end is None:
                return None
            params_s = s[paren + 1:end]
            tail = s[end + 1:].strip()
            htoks = head.split(" ")
            mname = htoks[-1]
            if not (IDENT.fullmatch(mname) or (mname.startswith("~") and IDENT.fullmatch(mname[1:]))):
                return None
            pre = htoks[:-1]
            mods = [t for t in pre if t in METHOD_MODS]
            ret = norm(" ".join(t for t in pre if t not in METHOD_MODS)) or ""
            params = [p for p in (parse_param(x) for x in split_top(params_s)) if p]
            if params == [{"type": "void", "name": "", "default": None}]:
                params = []
            rec = {"ret": ret, "mods": mods, "params": params, "attrs": attrs, "file": rel, "line": line,
                   "cond": cond, "proto": "proto" in mods or (terminated and "native" in mods), "tail": tail}
            rec["sig"] = sig_of(mname, rec)
            if owner == "<global>":
                self.globals.setdefault(mname, []).append(rec)
            else:
                self.cls(owner)["methods"].setdefault(mname, []).append(rec)
            return ("method", mname)
        # field(s)
        if kind != "class" and kind != "global":
            return None
        decl = s
        init = None
        if eqpos is not None:
            decl = s[:eqpos].strip()
            init = s[eqpos + 1:].strip()
        if not terminated and init is not None:
            res = ("init", None)
        else:
            res = None
        toks = split_top(decl)
        first = toks[0].strip().split(" ")
        if len(first) < 2:
            return res
        names = [first[-1]] + [t.strip() for t in toks[1:]]
        pre = first[:-1]
        mods = [t for t in pre if t in METHOD_MODS or t in ("const", "ref", "autoptr", "owned")]
        typ = norm(" ".join(t for t in pre if t not in mods))
        for nm in names:
            nm = re.sub(r"\[.*\]$", "", nm).strip()
            if not IDENT.fullmatch(nm):
                continue
            rec = {"type": typ, "mods": mods, "init": norm(init) if init else None, "attrs": attrs,
                   "file": rel, "line": line, "cond": cond}
            if owner == "<global>":
                self.globals.setdefault(nm, []).append(dict(rec, field=True))
            else:
                self.cls(owner)["fields"][nm] = rec
        return res


def sig_of(name, rec):
    ps = []
    for p in rec["params"]:
        t = p["type"]
        if p["default"] is not None:
            t += "=" + p["default"]
        ps.append(t)
    mods = " ".join(m for m in rec["mods"] if m in ("static", "proto", "native", "private", "protected", "event"))
    return (mods + " " if mods else "") + (rec["ret"] + " " if rec["ret"] else "") + name + "(" + ", ".join(ps) + ")"


def main():
    root, out = sys.argv[1], sys.argv[2]
    p = Parser(root)
    for dp, dn, fn in os.walk(root):
        dn.sort()
        for f in sorted(fn):
            if f.lower().endswith(".c"):
                p.parse_file(os.path.join(dp, f))
    data = {"classes": p.classes, "enums": p.enums, "globals": p.globals, "typedefs": p.typedefs}
    with open(out, "w", encoding="utf-8") as fh:
        json.dump(data, fh, indent=0, ensure_ascii=False)
    nm = sum(len(v) for c in p.classes.values() for v in c["methods"].values())
    print(f"classes={len(p.classes)} methods={nm} enums={len(p.enums)} globals={len(p.globals)}")


if __name__ == "__main__":
    main()
