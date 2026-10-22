"""Remap vanilla DayZ script line references in the skills after a game patch.

The DayZ skills cite vanilla script lines, e.g. ``P:\\scripts\\3_game\\vehicles\\transport.c:821``,
``carscript.c:229-271``, ``combinationlock.c:233,273``, ``[EXACT] 4_world/.../itembase.c:4659`` or a
bare continuation ``(`:826`)`` that inherits the file of the previous ref in the same paragraph.
When a patch shifts lines inside a script file, every such number goes stale. This tool maps each
cited OLD line to the NEW line with difflib.SequenceMatcher over the two versions of the file.

Rules
-----
* Only files whose content differs between ``--old`` and ``--new`` (CR ignored) are touched.
* A line that is unchanged and moved -> the number is rewritten. A line that was modified or removed
  -> NOT guessed: listed as NEEDS-REVIEW with the old text and the new candidates. A range ``a-b``
  is rewritten only when every line of it is unchanged and still contiguous; otherwise NEEDS-REVIEW.
* Refs labelled 1.29 are never touched: ``O:\\`` paths, text after a ``1.29`` / ``Before 1.30`` /
  ``pre-1.30`` / ``<=1.29`` marker up to the end of the clause (``;``, the closing parenthesis, the
  table cell, the sentence, the next ``1.30`` mention), a table column whose header says 1.29, and a
  bare ``:N`` that continues a 1.29 ref (reported for review). A ref followed by a 1.29 marker in the
  same parenthesis / cell, or by ``(1.30: ...)``, is reported as 1.29-SUSPECT and not touched.
* Refs into non-script files (config.cpp, .agf, .layout, P:\\DZ ...) and into unchanged files are
  left alone. Paths on a drive other than P: (or O:, 1.29) are not vanilla.
* File resolution: full path, partial path (``\\`` or ``/``, ``...`` / ``…`` allowed) or a bare name that
  is unique in the old tree. An ambiguous bare name (``motorbike_01.c`` = vehicle or light profile) is
  decided by fuller paths in the same paragraph, then in the same document, then by line count and
  identifiers; otherwise AMBIGUOUS. A bare ``:N`` takes the nearest previous file ref or file mention
  in its paragraph / table row / code block; when it has to look further (heading, table header, the
  same table column) it is REMAP-INFERRED (applied only with ``--apply-inferred``), and when those
  candidates disagree it is AMBIGUOUS.
* The old tree is the scripts of the build the refs were written against: keep a copy of ``P:\\scripts``
  (or the BohemiaInteractive/DayZ-Script-Diff tree of that build) before re-extracting ``P:\\``.
* Files scanned: every ``*.md`` under ``--skills``; ``*.py`` only under ``dayz-preflight``.
  Refs inside byte-exact quoted blocks (``<!-- MOVED-EXACT sha256=... -->`` ... ``<!-- END MOVED-EXACT -->``,
  ``SUPERSEDED-EXACT``) are never edited.
  Excluded by default: ``dayz-130-migration/{scripts,data,tests}``, ``synced``, ``_shared/tools``, ``*/history/*``,
  and any ``api-delta-*-to-<newbuild>.md`` written against the new build (``--exclude`` adds globs).

Dry-run by default. ``--apply`` edits files in place; only the digits of remapped numbers change,
everything else (encoding, BOM, line endings) stays byte-for-byte. The rewrite is NOT idempotent (a
second run would shift the numbers again), so ``--apply`` records the (old, new) tree fingerprints in
``_shared/tools/remap_applied.log`` and refuses to run twice for the same pair (``--force-reapply``).
Workflow: dry run -> read the report (REVIEW / SUSPECT-129 / SKIP-129 / AMBIGUOUS / REMAP-INFERRED) ->
write a ``--decisions`` file -> ``--apply [--apply-inferred]`` once -> fix REVIEW items by hand.

Run (bump procedure: dayz-130-migration/scripts/PATCH_BUMP.md):
    python -I remap_vanilla_refs.py --old <old>\\scripts --new P:\\scripts --skills <claude>\\skills \\
        --new-build <build> --report <scratch>\\remap.md [--json <scratch>\\remap.json] \\
        [--decisions <scratch>\\decisions.txt] [--apply [--apply-inferred]]
A decisions file holds ``accept <file:line:col>`` / ``reject <file:line:col>`` lines (ids as printed in
the report; ``#`` starts a comment).
"""
from __future__ import annotations

import argparse
import bisect
import difflib
import fnmatch
import hashlib
import json
import os
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional

# ----------------------------------------------------------------------------------------------
# Vanilla trees
# ----------------------------------------------------------------------------------------------

SCRIPT_EXTS = (".c", ".h")


def _read_lines(path: Path) -> list[str]:
    data = path.read_bytes()
    text = data.decode("utf-8", errors="replace")
    if text.startswith("\ufeff"):
        text = text[1:]
    return text.replace("\r", "").split("\n")


def _index_tree(root: Path) -> dict[str, Path]:
    out: dict[str, Path] = {}
    for dirpath, _dirs, files in os.walk(root):
        for name in files:
            if name.lower().endswith(SCRIPT_EXTS):
                full = Path(dirpath) / name
                rel = full.relative_to(root).as_posix().lower()
                out[rel] = full
    return out


class VanillaTrees:
    def __init__(self, old_root: Path, new_root: Path):
        self.old_root, self.new_root = old_root, new_root
        self.old_idx = _index_tree(old_root)
        self.new_idx = _index_tree(new_root)
        # Real file set of the old build = files present in both trees (the old tree may come from
        # a git mirror that keeps stale files; files only in new were added by the patch).
        self.common = sorted(set(self.old_idx) & set(self.new_idx))
        self.added = sorted(set(self.new_idx) - set(self.old_idx))
        self.only_old = sorted(set(self.old_idx) - set(self.new_idx))
        self.by_base: dict[str, list[str]] = {}
        for rel in self.common:
            self.by_base.setdefault(rel.rsplit("/", 1)[-1], []).append(rel)
        self._lines_cache: dict[tuple[str, str], list[str]] = {}
        self._changed: dict[str, bool] = {}
        self._maps: dict[str, "LineMap"] = {}

    def lines(self, which: str, rel: str) -> list[str]:
        key = (which, rel)
        if key not in self._lines_cache:
            idx = self.old_idx if which == "old" else self.new_idx
            self._lines_cache[key] = _read_lines(idx[rel])
        return self._lines_cache[key]

    def is_changed(self, rel: str) -> bool:
        if rel not in self._changed:
            self._changed[rel] = self.lines("old", rel) != self.lines("new", rel)
        return self._changed[rel]

    def changed_files(self) -> list[str]:
        return [rel for rel in self.common if self.is_changed(rel)]

    def linemap(self, rel: str) -> "LineMap":
        if rel not in self._maps:
            self._maps[rel] = LineMap(self.lines("old", rel), self.lines("new", rel))
        return self._maps[rel]


class LineMap:
    """old line (1-based) -> new line (1-based) or None (modified / removed)."""

    def __init__(self, old: list[str], new: list[str]):
        self.old, self.new = old, new
        a = [s.rstrip() for s in old]
        b = [s.rstrip() for s in new]
        sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
        self.map: dict[int, int] = {}
        self.opcode_of: dict[int, tuple[str, int, int, int, int]] = {}
        self.opcodes = sm.get_opcodes()
        for tag, i1, i2, j1, j2 in self.opcodes:
            if tag == "equal":
                for k in range(i2 - i1):
                    self.map[i1 + k + 1] = j1 + k + 1
            else:
                for i in range(i1, i2):
                    self.opcode_of[i + 1] = (tag, i1, i2, j1, j2)

    def new_of(self, n: int) -> Optional[int]:
        return self.map.get(n)

    def candidates(self, n: int, context: int = 1) -> list[tuple[int, str]]:
        """New-side lines of the diff hunk that replaced old line n (plus a little context)."""
        op = self.opcode_of.get(n)
        if not op:
            return []
        _tag, _i1, _i2, j1, j2 = op
        lo, hi = max(0, j1 - context), min(len(self.new), j2 + context)
        return [(j + 1, self.new[j]) for j in range(lo, hi)]

    def inserted_between(self, a: int, b: int) -> bool:
        """True when the new file has extra lines between old lines a..b (both mapped)."""
        na, nb = self.map.get(a), self.map.get(b)
        return na is not None and nb is not None and (nb - na) != (b - a)


# ----------------------------------------------------------------------------------------------
# Reference scanning
# ----------------------------------------------------------------------------------------------

NUM = r"\d{1,5}"
RNG = rf"{NUM}(?:[ ]?[-\u2013][ ]?{NUM})?"
END = r"(?=[`'\")\],;:\s/|*>]|\.(?:\s|$|`)|$)"
NUMLIST = rf"{RNG}{END}(?:(?:[ ]?,[ ]?|/):?{RNG}{END})*"

PATH_SEG = r"(?:[\w.\-]+|\.\.\.|\u2026)"
FILE_REF_RE = re.compile(
    rf"(?P<path>(?:[A-Za-z]:[\\/]+)?(?:{PATH_SEG}[\\/]+)*[\w\-]+\.(?P<ext>[A-Za-z][A-Za-z0-9]{{0,7}}))"
    rf"(?:(?P<colon>:)|[ ]+(?:line|lines|l\.)[ ]?)(?P<nums>{NUMLIST})",
)
BARE_REF_RE = re.compile(rf"(?<![^`(\s,;])(?P<colon>:)(?P<nums>{NUMLIST})")
MENTION_EXTS = ("c|h|cpp|hpp|agf|agr|ast|asi|layout|xml|json|py|ps1|bat|cmd|md|txt|csv|rvmat|p3d|paa|emat|ptc|"
                "cfg|ext|xob|anm|txa|edds|imageset|styles|sqf|et|conf|ini|log|rpt|bisign|pbo|dll|exe|bin|html|"
                "js|ts|yaml|yml|toml|gproj|wav|ogg|tga|png|jpg|dds|fbx|blend|glb|gltf|ragdoll|nm|map|wrp|bisurf")
MENTION_RE = re.compile(
    rf"(?P<path>(?:[A-Za-z]:[\\/]+)?(?:{PATH_SEG}[\\/]+)*[\w\-]+\.(?P<ext>{MENTION_EXTS}))(?![\w])",
    re.IGNORECASE,
)
NUM_ITEM_RE = re.compile(rf"(?P<a>{NUM})(?:[ ]?[-\u2013][ ]?(?P<b>{NUM}))?")

MARK_129_RE = re.compile(
    r"(?:\bBefore\s+1\.30\b|\bpre-1\.30\b|\buntil\s+1\.30\b|[\u2264<]=?\s?1\.29\b|\b1\.29(?!\.\d)\b)",
    re.IGNORECASE,
)
MARK_130_RE = re.compile(r"\b1\.30\b")
SENTENCE_END_RE = re.compile(r"(?<!e\.g)(?<!i\.e)(?<!etc)(?<!vs)\.(?=\s+[A-Z\[`*(])")
STOPWORDS = {
    "true", "false", "null", "void", "bool", "float", "string", "override", "return", "class",
    "protected", "private", "static", "const", "this", "super", "proto", "native", "modded",
    "auto", "array", "vector", "else", "case", "break", "scripts", "world", "game", "core",
    "mission", "exact", "vanilla", "line", "lines", "note", "server", "client",
}


@dataclass
class NumItem:
    a: int
    b: Optional[int]
    a_span: tuple[int, int]  # absolute char offsets in the file text
    b_span: Optional[tuple[int, int]]


@dataclass
class Ref:
    skill_file: str
    line_no: int  # 1-based line in the skill file
    col: int
    raw: str
    kind: str  # "file" | "bare"
    path_text: str
    ext: str
    items: list[NumItem]
    start: int
    end: int
    para_id: int
    is_table: bool = False
    in_code: bool = False
    label_129: str = ""  # reason when the ref is 1.29-labelled
    suspect_129: str = ""
    rel: Optional[str] = None  # resolved vanilla rel path
    resolve_note: str = ""
    parent: Optional["Ref"] = None
    parent_inferred: str = ""  # bare ref whose file was found outside its paragraph/row
    after_130: bool = False  # a "1.30" mention sits between the parent ref and this bare ref
    alt_parents: list["Ref"] = field(default_factory=list)  # other plausible files for an inferred bare ref
    idents: list[str] = field(default_factory=list)


def _line_starts(text: str) -> list[int]:
    starts = [0]
    for m in re.finditer(r"\n", text):
        starts.append(m.end())
    return starts


def _paragraphs(lines: list[str]) -> tuple[list[int], list[bool], list[bool]]:
    """Paragraph id per line, is-table-row flag, in-code-block flag.

    A fenced code block is one paragraph; outside code, blank lines, headings and list items
    start a new paragraph.
    """
    pid, cur = [], 0
    is_table, in_code = [], []
    code = False
    prev_blank = True
    for ln in lines:
        s = ln.strip()
        fence = s.startswith("```") or s.startswith("~~~")
        if fence:
            code = not code
            cur += 1
            pid.append(cur)
            is_table.append(False)
            in_code.append(True)
            prev_blank = True
            if not code:
                cur += 1
            continue
        if code:
            pid.append(cur)
            is_table.append(False)
            in_code.append(True)
            continue
        if not s:
            cur += 1 if not prev_blank else 0
            pid.append(cur)
            is_table.append(False)
            in_code.append(code)
            prev_blank = True
            continue
        if not code and (re.match(r"^(#{1,6}\s|[-*+]\s|\d+[.)]\s)", s)):
            cur += 1  # headings and list items start a new paragraph
        pid.append(cur)
        is_table.append(not code and s.startswith("|"))
        in_code.append(code)
        prev_blank = False
    return pid, is_table, in_code


def _table_header_flags(lines: list[str], is_table: list[bool]) -> dict[int, list[str]]:
    """line index -> list of header cell texts of its table."""
    out: dict[int, list[str]] = {}
    i = 0
    while i < len(lines):
        if is_table[i]:
            j = i
            while j < len(lines) and is_table[j]:
                j += 1
            header = [c.strip() for c in lines[i].strip().strip("|").split("|")]
            for k in range(i, j):
                out[k] = header
            i = j
        else:
            i += 1
    return out


def _cell_index(line: str, col: int) -> int:
    return line[:col].count("|") - (1 if line.lstrip().startswith("|") else 0)


class SkillDoc:
    def __init__(self, path: Path, rel: str):
        self.path, self.rel = path, rel
        raw = path.read_bytes()
        self.bom = raw.startswith(b"\xef\xbb\xbf")
        self.text = raw[3:].decode("utf-8") if self.bom else raw.decode("utf-8")
        self.lines = self.text.split("\n")
        self.starts = _line_starts(self.text)
        self.pid, self.is_table, self.in_code = _paragraphs([l.rstrip("\r") for l in self.lines])
        self.headers = _table_header_flags([l.rstrip("\r") for l in self.lines], self.is_table)
        # byte-exact quoted blocks (<!-- MOVED-EXACT sha256=... --> ... <!-- END MOVED-EXACT -->) are never edited
        self.exact_ranges: list[tuple[int, int]] = []
        for m in re.finditer(r"<!--\s*([A-Z]+-EXACT)\b.*?-->", self.text):
            end = re.compile(r"<!--\s*END\s+" + m.group(1) + r"\s*-->").search(self.text, m.end())
            self.exact_ranges.append((m.start(), end.end() if end else len(self.text)))

    def in_exact_block(self, pos: int) -> bool:
        return any(lo <= pos < hi for lo, hi in self.exact_ranges)

    def pos_to_line(self, pos: int) -> tuple[int, int]:
        i = bisect.bisect_right(self.starts, pos) - 1
        return i + 1, pos - self.starts[i]

    def para_bounds(self, line_idx: int) -> tuple[int, int]:
        p = self.pid[line_idx]
        lo = line_idx
        while lo > 0 and self.pid[lo - 1] == p:
            lo -= 1
        hi = line_idx
        while hi + 1 < len(self.lines) and self.pid[hi + 1] == p:
            hi += 1
        return self.starts[lo], self.starts[hi] + len(self.lines[hi])


def _clause_end(text: str, start: int, limit: int, single_line: bool) -> int:
    """End offset of a 1.29-labelled clause that starts at `start`."""
    depth = 0
    i = start
    while i < limit:
        ch = text[i]
        if ch == "(":
            depth += 1
        elif ch == ")":
            if depth == 0:
                return i
            depth -= 1
        elif ch in ";|":
            return i
        elif ch == "\n":
            if single_line:
                return i
            nxt = text[i + 1:i + 8].lstrip()
            if nxt[:1] in "-*+|>#" or re.match(r"\d+[.)]\s", nxt) or text[i + 1:i + 2] == "\n":
                return i
        elif ch == "." and SENTENCE_END_RE.match(text, i):
            return i
        elif ch == "1" and MARK_130_RE.match(text, i) and not _inside_129_marker(text, i):
            return i
        i += 1
    return limit


def _inside_129_marker(text: str, pos: int) -> bool:
    lo = max(0, pos - 12)
    for m in MARK_129_RE.finditer(text, lo, min(len(text), pos + 8)):
        if m.start() <= pos < m.end():
            return True
    return False


def _label_129(doc: SkillDoc, ref: Ref) -> None:
    line_idx = ref.line_no - 1
    line = doc.lines[line_idx].rstrip("\r")
    # 1. O:\ path (or a 1.29 install path) inside the same backtick span / token
    if ref.kind == "file":
        ls = doc.starts[line_idx]
        seg_start = max(line.rfind("`", 0, ref.col), line.rfind("(", 0, ref.col), 0)
        before = line[seg_start:ref.col]
        if re.search(r"(?i)\bO:[\\/]", ref.path_text) or re.search(r"(?i)\bO:[\\/]", before):
            ref.label_129 = "O:\\ path"
            return
        if re.search(r"1\.29", ref.path_text):
            ref.label_129 = "1.29 path"
            return
    # 2. table column header says 1.29
    if doc.is_table[line_idx]:
        header = doc.headers.get(line_idx, [])
        ci = _cell_index(line, ref.col)
        if 0 <= ci < len(header):
            h = header[ci]
            if MARK_129_RE.search(h) and not MARK_130_RE.search(MARK_129_RE.sub("", h)):
                ref.label_129 = f"table column '{h}'"
                return
    # 3. 1.29 clause before the ref
    single = doc.is_table[line_idx] or doc.in_code[line_idx]
    if single:
        lo, hi = doc.starts[line_idx], doc.starts[line_idx] + len(line)
    else:
        lo, hi = doc.para_bounds(line_idx)
    text = doc.text
    for m in MARK_129_RE.finditer(text, lo, ref.start):
        end = _clause_end(text, m.end(), hi, single)
        if end > ref.start:
            ref.label_129 = f"after marker '{m.group(0)}'"
            return
    # 4a. "x.c:557 (1.30: :788)" -> the ref before the "(1.30: ...)" note is the old one
    if re.match(r"`?\s*[,;]?\s*\(\s*(?:on\s+|in\s+)?1\.30\s*[:=→]", text[ref.end:ref.end + 24]):
        ref.suspect_129 = "followed by '(1.30: ...)'"
        return
    # 4. a 1.29 marker AFTER the ref in the same parenthesis / table cell / code line
    depth = 0
    i = ref.end
    limit = hi
    while i < limit:
        ch = text[i]
        if ch == "(":
            depth += 1
        elif ch == ")":
            if depth == 0:
                break
            depth -= 1
        elif ch in "|;\n":
            break
        elif ch == "." and SENTENCE_END_RE.match(text, i):
            break
        elif ch == "1" and depth == 0 and MARK_130_RE.match(text, i) and not _inside_129_marker(text, i):
            break
        m = MARK_129_RE.match(text, i) if depth == 0 else None
        if m:
            ref.suspect_129 = f"followed by '{m.group(0)}'"
            return
        i += 1


def _gather_idents(doc: SkillDoc, ref: Ref) -> list[str]:
    line_idx = ref.line_no - 1
    if doc.is_table[line_idx] or doc.in_code[line_idx]:
        lo = doc.starts[line_idx]
        hi = lo + len(doc.lines[line_idx])
        if doc.in_code[line_idx]:
            # code comment refs describe the code lines around them
            lo = doc.starts[max(0, line_idx - 3)]
            j = min(len(doc.lines) - 1, line_idx + 6)
            hi = doc.starts[j] + len(doc.lines[j])
    else:
        plo, phi = doc.para_bounds(line_idx)
        lo, hi = max(plo, ref.start - 220), min(phi, ref.end + 120)
    window = doc.text[lo:hi]
    toks: list[str] = []
    spans = re.findall(r"`([^`\n]+)`", window) if not doc.in_code[line_idx] else [window]
    for s in spans:
        for t in re.findall(r"[A-Za-z_][A-Za-z0-9_]{3,}", s):
            tl = t.lower()
            if tl in STOPWORDS or re.match(r"^\d", t):
                continue
            if re.search(r"\.(c|h|cpp|agf|layout|md)$", t):
                continue
            if t not in toks:
                toks.append(t)
    return toks


def scan_doc(doc: SkillDoc) -> list[Ref]:
    text = doc.text
    refs: list[Ref] = []
    masked: list[tuple[int, int]] = []
    for m in FILE_REF_RE.finditer(text):
        path_text = m.group("path")
        ext = m.group("ext").lower()
        # numbers like 1.30.164078 or urls are not file refs
        if re.fullmatch(r"[\d.]+", path_text.replace("\\", "").replace("/", "")):
            continue
        items = _parse_items(m, "nums")
        if not items:
            continue
        line_no, col = doc.pos_to_line(m.start())
        refs.append(Ref(doc.rel, line_no, col, m.group(0), "file", path_text, ext, items,
                        m.start(), m.end(), doc.pid[line_no - 1],
                        is_table=doc.is_table[line_no - 1], in_code=doc.in_code[line_no - 1]))
        masked.append((m.start(), m.end()))
    masked.sort()

    def is_masked(pos: int) -> bool:
        k = bisect.bisect_right(masked, (pos, 10**12)) - 1
        return k >= 0 and masked[k][0] <= pos < masked[k][1]

    for m in BARE_REF_RE.finditer(text):
        if is_masked(m.start()):
            continue
        items = _parse_items(m, "nums")
        if not items:
            continue
        line_no, col = doc.pos_to_line(m.start())
        refs.append(Ref(doc.rel, line_no, col, m.group(0), "bare", "", "", items,
                        m.start(), m.end(), doc.pid[line_no - 1],
                        is_table=doc.is_table[line_no - 1], in_code=doc.in_code[line_no - 1]))
    # file mentions without a line number set the context of later bare refs too
    mentions: list[Ref] = []
    for m in MENTION_RE.finditer(text):
        if is_masked(m.start()):
            continue
        line_no, col = doc.pos_to_line(m.start())
        mentions.append(Ref(doc.rel, line_no, col, m.group(0), "mention", m.group("path"),
                            m.group("ext").lower(), [], m.start(), m.end(), doc.pid[line_no - 1],
                            is_table=doc.is_table[line_no - 1], in_code=doc.in_code[line_no - 1]))
    refs.sort(key=lambda r: r.start)
    ctx = sorted([r for r in refs if r.kind == "file"] + mentions, key=lambda r: r.start)
    ctx_starts = [r.start for r in ctx]
    for r in refs:
        if r.kind != "bare":
            continue
        _attach_parent(doc, r, ctx, ctx_starts)
    for r in refs + mentions:
        _label_129(doc, r)
        r.idents = _gather_idents(doc, r)
    doc.mentions = mentions
    return refs


def _attach_parent(doc: "SkillDoc", r: Ref, ctx: list[Ref], ctx_starts: list[int]) -> None:
    """Nearest previous file ref/mention: same table row, same code block or same paragraph;
    otherwise (inferred) the nearest one above, up to the closest heading / 60 lines."""
    k = bisect.bisect_left(ctx_starts, r.start) - 1
    line_idx = r.line_no - 1
    if r.is_table:
        region_lo = doc.starts[line_idx]
    else:
        region_lo, _ = doc.para_bounds(line_idx)
    if k >= 0 and ctx[k].end <= r.start and ctx[k].start >= region_lo:
        r.parent = ctx[k]
        between = doc.text[ctx[k].end:r.start]
        r.after_130 = any(not _inside_129_marker(between, m.start()) for m in MARK_130_RE.finditer(between))
        return
    # inferred context
    lo_line = line_idx
    if r.is_table:
        while lo_line > 0 and doc.is_table[lo_line - 1]:
            lo_line -= 1
    steps = 0
    while lo_line > 0 and steps < 60:
        lo_line -= 1
        steps += 1
        if re.match(r"^\s*#{1,6}\s", doc.lines[lo_line]) and not doc.in_code[lo_line]:
            break
    search_lo = doc.starts[lo_line]
    cands: list[tuple[Ref, str]] = []
    if r.is_table:
        t0 = line_idx
        while t0 > 0 and doc.is_table[t0 - 1]:
            t0 -= 1
        hdr_lo, hdr_hi = doc.starts[t0], doc.starts[t0] + len(doc.lines[t0])
        hdr = [c for c in ctx if hdr_lo <= c.start < hdr_hi and c.end <= r.start]
        if hdr and t0 != line_idx:
            cands.append((hdr[-1], "table header"))
        region_lo = doc.starts[t0]
        # "| same (`:1710-1716`) |" under a row that cites the file in the same column
        my_cell = _cell_index(doc.lines[line_idx].rstrip("\r"), r.col)
        for c in reversed([c for c in ctx if region_lo <= c.start < doc.starts[line_idx]]):
            c_line = doc.lines[c.line_no - 1].rstrip("\r")
            if _cell_index(c_line, c.col) == my_cell:
                cands.append((c, f"same table column, line {c.line_no}"))
                break
    while k >= 0 and ctx[k].end > r.start:
        k -= 1
    # for table refs: the nearest mention above the table (rows of the table are handled above)
    while k >= 0 and r.is_table and ctx[k].start >= region_lo:
        k -= 1
    if k >= 0 and ctx[k].start >= search_lo:
        cands.append((ctx[k], f"nearest above (line {ctx[k].line_no})"))
    if cands:
        r.parent, r.parent_inferred = cands[0]
        r.alt_parents = [c for c, _why in cands[1:]]


def _parse_items(m: re.Match, group: str) -> list[NumItem]:
    s, base = m.group(group), m.start(group)
    out: list[NumItem] = []
    prev_end = 0
    for im in NUM_ITEM_RE.finditer(s):
        if out and " " in s[prev_end:im.start()] and re.match(r"[ \t]+[A-Za-z]", m.string[base + im.end():]):
            break  # "x.c:821, 2 lines below": a prose number, not a line
        prev_end = im.end()
        a = int(im.group("a"))
        a_span = (base + im.start("a"), base + im.end("a"))
        b = b_span = None
        if im.group("b"):
            b = int(im.group("b"))
            b_span = (base + im.start("b"), base + im.end("b"))
        out.append(NumItem(a, b, a_span, b_span))
    return out


# ----------------------------------------------------------------------------------------------
# Resolution
# ----------------------------------------------------------------------------------------------

ALLOWED_PREFIXES = {(), ("exp",), ("exp", "scripts"), ("vanilla",)}


def _components(path_text: str) -> tuple[Optional[str], list[str]]:
    comps = [c for c in re.split(r"[\\/]+", path_text) if c]
    drive = None
    if comps and re.fullmatch(r"[A-Za-z]:", comps[0]):
        drive = comps[0][0].upper()
        comps = comps[1:]
    return drive, [c.lower() for c in comps]


def resolve(trees: VanillaTrees, ref: Ref, siblings: list[Ref], doc_all: list[Ref] = ()) -> None:
    if ref.kind == "bare":
        p = ref.parent
        if p is None:
            ref.resolve_note = "orphan"
            return
        ref.rel = p.rel
        ref.resolve_note = (p.resolve_note or "") + f" (inherited from {p.raw.strip()!r}" + (
            f", INFERRED {ref.parent_inferred})" if ref.parent_inferred else ")")
        alts = {a.rel for a in ref.alt_parents} | {p.rel}
        if len(alts) > 1:
            # the inferred context is not unique: report when any candidate is a vanilla file
            ref.rel = None
            ref.resolve_note = "AMBIGUOUS context: " + ", ".join(sorted(str(x) for x in alts))
            return
        if ref.after_130:
            pass  # "x.c:557 (1.30: `:788`)" - the bare ref is the 1.30 one
        elif p.label_129 and not ref.label_129:
            ref.suspect_129 = f"continues a 1.29 ref ({p.label_129})"
        elif p.suspect_129 and not ref.label_129 and not ref.suspect_129:
            ref.suspect_129 = f"continues a 1.29-suspect ref ({p.suspect_129})"
        return
    if ref.ext not in ("c", "h"):
        ref.resolve_note = "non-script"
        return
    drive, comps = _components(ref.path_text)
    if drive == "O":
        ref.resolve_note = "1.29 drive"
        return
    if drive and drive != "P":
        ref.resolve_note = f"non-vanilla drive {drive}:"
        return
    if "scripts" in comps[:-1]:
        k = max(i for i, c in enumerate(comps[:-1]) if c == "scripts")
        prefix = tuple(c for c in comps[:k] if c not in ("...", "\u2026"))
        if prefix not in ALLOWED_PREFIXES:
            ref.resolve_note = f"non-vanilla prefix {'/'.join(prefix)}"
            return
        comps = comps[k + 1:]
    elif drive == "P":
        ref.resolve_note = "P:\\ non-script path"
        return
    ell = [i for i, c in enumerate(comps) if c in ("...", "\u2026")]
    if ell:
        head, tail = comps[:ell[0]], comps[ell[-1] + 1:]
    else:
        head, tail = [], comps
    base = tail[-1]
    cands = []
    for rel in trees.by_base.get(base, []):
        rc = rel.split("/")
        if len(tail) <= len(rc) and rc[-len(tail):] == tail and rc[:len(head)] == head:
            cands.append(rel)
    if not cands:
        ref.resolve_note = "not in vanilla tree"
        return
    if len(cands) == 1:
        ref.rel = cands[0]
        return
    # ambiguous bare / partial name: 1) a sibling ref in the paragraph with a fuller path to the
    # same base name or to the same directory, 2) the line number must exist, 3) identifiers.
    scores = {c: 0 for c in cands}
    for s in siblings:
        if s is ref or s.rel is None or s.kind == "bare":
            continue
        sdir = s.rel.rsplit("/", 1)[0]
        for c in cands:
            if s.rel == c:
                scores[c] += 3
            elif c.rsplit("/", 1)[0] == sdir:
                scores[c] += 2
    best = max(scores.values())
    top = [c for c in cands if scores[c] == best]
    if best > 0 and len(top) == 1:
        ref.rel, ref.resolve_note = top[0], "ambiguous name, chosen by sibling refs"
        return
    # the same base name written with a fuller path elsewhere in the document
    full = {c: 0 for c in top}
    for s in doc_all:
        if s is ref or s.rel not in full or s.kind == "bare":
            continue
        if re.search(r"[\\/]", s.path_text):
            full[s.rel] += 1
    used = [c for c in top if full[c] > 0]
    if len(used) == 1:
        ref.rel, ref.resolve_note = used[0], "ambiguous name, chosen by fuller paths in the document"
        return
    maxline = max((max(it.a, it.b or 0) for it in ref.items), default=0)
    feas = [c for c in top if len(trees.lines("old", c)) >= maxline]
    if len(feas) == 1:
        ref.rel, ref.resolve_note = feas[0], "ambiguous name, chosen by line count"
        return
    if feas and ref.idents:
        hits = {}
        for c in feas:
            old = trees.lines("old", c)
            hits[c] = sum(1 for it in ref.items for t in ref.idents
                          if any(t in old[k] for k in range(max(0, it.a - 3), min(len(old), (it.b or it.a) + 2))))
        hb = max(hits.values())
        ht = [c for c in feas if hits[c] == hb]
        if hb > 0 and len(ht) == 1:
            ref.rel, ref.resolve_note = ht[0], "ambiguous name, chosen by identifiers"
            return
    ref.resolve_note = "AMBIGUOUS: " + ", ".join(cands)


# ----------------------------------------------------------------------------------------------
# Mapping
# ----------------------------------------------------------------------------------------------

@dataclass
class Result:
    ref: Ref
    status: str  # REMAP | SAME | REVIEW | SKIP-129 | SUSPECT-129 | AMBIGUOUS
    edits: list[tuple[tuple[int, int], int, int]] = field(default_factory=list)  # span, old, new
    detail: list[str] = field(default_factory=list)
    ident_check: str = "n/a"


def _ident_check(trees: VanillaTrees, ref: Ref) -> str:
    if not ref.idents or ref.rel is None:
        return "n/a"
    old = trees.lines("old", ref.rel)
    for it in ref.items:
        lo, hi = max(0, it.a - 4), min(len(old), (it.b or it.a) + 3)
        blob = "\n".join(old[lo:hi])
        if any(t in blob for t in ref.idents):
            return "hit"
    return "MISS"


def map_ref(trees: VanillaTrees, ref: Ref) -> Result:
    lm = trees.linemap(ref.rel)
    res = Result(ref, "SAME")
    res.ident_check = _ident_check(trees, ref)
    review = False
    n_old = len(lm.old)
    for it in ref.items:
        nums = [it.a] + ([it.b] if it.b is not None else [])
        if any(n < 1 or n > n_old for n in nums):
            review = True
            res.detail.append(f"line {it.a}{'-' + str(it.b) if it.b else ''} is outside the old file "
                              f"({n_old} lines)")
            continue
        mapped = [lm.new_of(n) for n in nums]
        if any(x is None for x in mapped):
            review = True
            for n in nums:
                if lm.new_of(n) is None:
                    op = lm.opcode_of.get(n)
                    res.detail.append(f"old {n} [{op[0] if op else '?'}]: {lm.old[n - 1].strip()!r}")
                    for j, t in lm.candidates(n):
                        res.detail.append(f"    new {j}: {t.strip()!r}")
            continue
        if it.b is not None:
            lo, hi = it.a, it.b
            interior = [lm.new_of(n) for n in range(lo, hi + 1)] if hi >= lo else []
            if any(x is None for x in interior) or lm.inserted_between(lo, hi):
                review = True
                res.detail.append(f"range {lo}-{hi}: lines inside changed; endpoints map to "
                                  f"{mapped[0]}-{mapped[1]}")
                continue
        for n, new_n, span in zip(nums, mapped, [it.a_span] + ([it.b_span] if it.b_span else [])):
            if n != new_n:
                res.edits.append((span, n, new_n))
    if review:
        res.status = "REVIEW"
    elif res.edits:
        res.status = "REMAP"
    return res


# ----------------------------------------------------------------------------------------------
# Driver
# ----------------------------------------------------------------------------------------------

DEFAULT_EXCLUDES = [
    "dayz-130-migration/scripts/*",
    "dayz-130-migration/data/*",
    "dayz-130-migration/tests/*",
    "synced/*",
    "_shared/tools/*",
    "*/history/*",  # "HISTORY ONLY" records keep the text they were written with
]


def iter_skill_files(root: Path, excludes: list[str], new_build: Optional[str]) -> list[tuple[Path, str]]:
    out = []
    for dirpath, dirs, files in os.walk(root):
        dirs.sort()
        for name in sorted(files):
            full = Path(dirpath) / name
            rel = full.relative_to(root).as_posix()
            low = name.lower()
            if low.endswith(".md"):
                pass
            elif low.endswith(".py") and rel.startswith("dayz-preflight/"):
                pass
            else:
                continue
            if any(fnmatch.fnmatch(rel, pat) for pat in excludes):
                continue
            if new_build and re.search(rf"-to-{re.escape(new_build)}\.md$", low):
                continue  # written against the new build already
            out.append((full, rel))
    return out


def main(argv: Optional[list[str]] = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--old", required=True, type=Path, help="old vanilla scripts root (refs were written against it)")
    ap.add_argument("--new", required=True, type=Path, help="new vanilla scripts root (e.g. P:\\scripts)")
    ap.add_argument("--skills", required=True, type=Path, help="skills root")
    ap.add_argument("--apply", action="store_true", help="edit files in place (default: dry run)")
    ap.add_argument("--report", type=Path, help="write a Markdown report here")
    ap.add_argument("--json", type=Path, help="write every classified ref as JSON here")
    ap.add_argument("--exclude", action="append", default=[], help="extra glob (relative to --skills) to skip")
    ap.add_argument("--new-build", default=None,
                    help="new build number, e.g. 164200: skips references/*-to-<build>.md written against it")
    ap.add_argument("--force-reapply", action="store_true", help="ignore remap_applied.log (see above)")
    ap.add_argument("--apply-inferred", action="store_true",
                    help="also apply REMAP-INFERRED (bare ':N' whose file was found outside its paragraph)")
    ap.add_argument("--decisions", type=Path,
                    help="text file of 'accept <file:line:col>' / 'reject <file:line:col>' lines from a manual "
                         "review (accept overrides a 1.29 label or suspicion, reject leaves the ref alone)")
    args = ap.parse_args(argv)
    accept, reject = set(), set()
    if args.decisions:
        for ln in args.decisions.read_text(encoding="utf-8").splitlines():
            parts = ln.split("#", 1)[0].split()
            if len(parts) >= 2 and parts[0] in ("accept", "reject"):
                (accept if parts[0] == "accept" else reject).add(parts[1])

    for p in (args.old, args.new, args.skills):
        if not p.is_dir():
            print(f"not a directory: {p}", file=sys.stderr)
            return 2

    trees = VanillaTrees(args.old, args.new)
    changed = set(trees.changed_files())
    print(f"vanilla: {len(trees.common)} files in both trees, {len(changed)} changed, "
          f"{len(trees.added)} added, {len(trees.only_old)} only in old")
    fp_old, fp_new = _fingerprint(trees, "old", changed), _fingerprint(trees, "new", changed)
    log_path = args.skills / "_shared" / "tools" / "remap_applied.log"
    if args.apply and log_path.exists() and not args.force_reapply:
        if f"old={fp_old} new={fp_new}" in log_path.read_text(encoding="utf-8"):
            print(f"refusing --apply: this old/new pair was already applied ({log_path}); "
                  "a second run would shift the numbers again (use --force-reapply only after restoring "
                  "the skills from backup)", file=sys.stderr)
            return 3

    results: list[Result] = []
    counters: dict[str, int] = {}
    docs: dict[str, SkillDoc] = {}
    for full, rel in iter_skill_files(args.skills, DEFAULT_EXCLUDES + args.exclude, args.new_build):
        try:
            doc = SkillDoc(full, rel)
        except UnicodeDecodeError:
            print(f"skip (not utf-8): {rel}", file=sys.stderr)
            continue
        refs = scan_doc(doc)
        if not refs:
            continue
        docs[rel] = doc
        mentions = getattr(doc, "mentions", [])
        by_para: dict[int, list[Ref]] = {}
        for r in refs + mentions:
            if r.kind != "bare":
                by_para.setdefault(r.para_id, []).append(r)
        # resolve full paths first so ambiguous names can lean on them
        for r in sorted(refs + mentions, key=lambda r: r.path_text.count("\\") + r.path_text.count("/") == 0):
            if r.kind != "bare":
                resolve(trees, r, by_para.get(r.para_id, []), refs + mentions)
        for r in refs:
            if r.kind == "bare":
                resolve(trees, r, [])
        for r in refs:
            counters["refs scanned"] = counters.get("refs scanned", 0) + 1
            if r.rel is None:
                key = "AMBIGUOUS" if r.resolve_note.startswith("AMBIGUOUS") else "not vanilla script / unresolved"
                counters[key] = counters.get(key, 0) + 1
                if key == "AMBIGUOUS":
                    cands = [c for c in r.resolve_note.split(": ", 1)[1].split(", ") if c != "None"]
                    if not any(c in changed for c in cands):
                        counters["AMBIGUOUS"] -= 1
                        counters["vanilla, file unchanged"] = counters.get("vanilla, file unchanged", 0) + 1
                        continue
                    results.append(Result(r, "AMBIGUOUS", detail=[r.resolve_note]))
                continue
            if r.rel not in changed:
                counters["vanilla, file unchanged"] = counters.get("vanilla, file unchanged", 0) + 1
                continue
            if doc.in_exact_block(r.start):
                counters["inside a byte-exact block"] = counters.get("inside a byte-exact block", 0) + 1
                continue
            rid = ref_id(r)
            if r.label_129 and rid not in accept:
                would = map_ref(trees, r)
                results.append(Result(r, "SKIP-129", detail=[r.label_129 + f"; as a 1.30 ref it would be {would.status}"
                                                             + (": " + ", ".join(f"{o}->{n}" for _s, o, n in would.edits)
                                                                if would.edits else "")]))
                counters["SKIP-129"] = counters.get("SKIP-129", 0) + 1
                continue
            res = map_ref(trees, r)
            if rid in reject:
                res.detail.insert(0, f"rejected by decisions file; would be {res.status}")
                res.status, res.edits = "REJECTED", []
            elif rid in accept:
                res.detail.insert(0, "accepted by decisions file")
            elif r.suspect_129:
                res.detail.insert(0, f"1.29-suspect: {r.suspect_129}; would be {res.status}"
                                  + (": " + ", ".join(f"{o}->{n}" for _s, o, n in res.edits) if res.edits else ""))
                res.status, res.edits = "SUSPECT-129", []
            elif r.parent_inferred and res.status == "REMAP":
                res.status = "REMAP-INFERRED"
            results.append(res)
            counters[res.status] = counters.get(res.status, 0) + 1

    # ------------------------------------------------------------------ apply
    applied = 0
    if args.apply:
        per_doc: dict[str, list[tuple[tuple[int, int], int, int]]] = {}
        for res in results:
            if res.status == "REMAP" or (res.status == "REMAP-INFERRED" and args.apply_inferred):
                per_doc.setdefault(res.ref.skill_file, []).extend(res.edits)
        for rel, edits in per_doc.items():
            doc = docs[rel]
            current = doc.path.read_bytes()
            expect = (b"\xef\xbb\xbf" if doc.bom else b"") + doc.text.encode("utf-8")
            if current != expect:
                print(f"SKIP {rel}: changed on disk since the scan, re-run", file=sys.stderr)
                continue
            text = doc.text
            for (s, e), old_n, new_n in sorted(edits, key=lambda x: x[0][0], reverse=True):
                assert text[s:e] == str(old_n), (rel, s, text[s:e], old_n)
                text = text[:s] + str(new_n) + text[e:]
                applied += 1
            doc.path.write_bytes((b"\xef\xbb\xbf" if doc.bom else b"") + text.encode("utf-8"))
        import datetime
        with open(log_path, "a", encoding="utf-8") as fh:
            fh.write(f"{datetime.date.today().isoformat()} old={fp_old} new={fp_new} "
                     f"new_build={args.new_build or '?'} files={len(per_doc)} numbers={applied}\n")

    # ------------------------------------------------------------------ report
    order = ["refs scanned", "not vanilla script / unresolved", "vanilla, file unchanged",
             "inside a byte-exact block", "SAME", "REMAP", "REMAP-INFERRED", "REVIEW", "SUSPECT-129", "SKIP-129",
             "AMBIGUOUS", "REJECTED"]
    for k in order:
        print(f"{k:34s} {counters.get(k, 0)}")
    n_remap = sum(len(r.edits) for r in results if r.status == "REMAP")
    n_inf = sum(len(r.edits) for r in results if r.status == "REMAP-INFERRED")
    if args.apply:
        print(f"{'numbers rewritten':34s} {applied} (REMAP {n_remap}"
              f"{f' + REMAP-INFERRED {n_inf}' if args.apply_inferred else f'; REMAP-INFERRED {n_inf} not applied'})")
    else:
        print(f"{'numbers to rewrite':34s} {n_remap} (+{n_inf} REMAP-INFERRED with --apply-inferred)")

    if args.report:
        args.report.write_text(_report_md(args, trees, changed, results, counters, applied), encoding="utf-8")
        print(f"report: {args.report}")
    if args.json:
        args.json.write_text(json.dumps([_res_json(r) for r in results], indent=1, ensure_ascii=False),
                             encoding="utf-8")
        print(f"json: {args.json}")
    return 0


def _fingerprint(trees: VanillaTrees, which: str, changed: set[str]) -> str:
    h = hashlib.sha256()
    for rel in sorted(changed):
        h.update(rel.encode("utf-8"))
        h.update("\n".join(trees.lines(which, rel)).encode("utf-8", "replace"))
    return h.hexdigest()[:16]


def ref_id(r: Ref) -> str:
    return f"{r.skill_file}:{r.line_no}:{r.col + 1}"


def _res_json(res: Result) -> dict:
    r = res.ref
    return {
        "status": res.status, "id": ref_id(r), "file": r.skill_file, "line": r.line_no, "col": r.col + 1,
        "raw": r.raw,
        "kind": r.kind, "vanilla": r.rel, "resolve": r.resolve_note.strip(), "ident": res.ident_check,
        "idents": r.idents[:8], "edits": [[o, n] for _s, o, n in res.edits], "detail": res.detail,
    }


def _report_md(args, trees, changed, results, counters, applied) -> str:
    out = ["# Vanilla ref remap report", ""]
    out.append(f"- old: `{args.old}`  new: `{args.new}`  skills: `{args.skills}`  mode: "
               f"{'APPLY' if args.apply else 'dry run'}")
    out.append(f"- vanilla files changed: {len(changed)}")
    for k in ["refs scanned", "not vanilla script / unresolved", "vanilla, file unchanged",
              "inside a byte-exact block", "SAME", "REMAP", "REMAP-INFERRED", "REVIEW", "SUSPECT-129", "SKIP-129",
              "AMBIGUOUS", "REJECTED"]:
        out.append(f"- {k}: {counters.get(k, 0)}")
    out.append(f"- numbers {'rewritten' if args.apply else 'to rewrite'}: "
               f"{applied if args.apply else sum(len(r.edits) for r in results if r.status == 'REMAP')}"
          f" (+{sum(len(r.edits) for r in results if r.status == 'REMAP-INFERRED')} inferred"
          f"{', applied' if args.apply and args.apply_inferred else ''})")
    for status in ["REVIEW", "AMBIGUOUS", "SUSPECT-129", "REMAP-INFERRED", "REMAP", "REJECTED", "SKIP-129", "SAME"]:
        rows = [r for r in results if r.status == status]
        out += ["", f"## {status} ({len(rows)})", ""]
        for res in rows:
            r = res.ref
            edits = ", ".join(f"{o}->{n}" for _s, o, n in res.edits)
            out.append(f"- `{ref_id(r)}` `{r.raw.strip()}` -> `{r.rel}`"
                       + (f" [{edits}]" if edits else "") + f" ident={res.ident_check}"
                       + (f" ({r.resolve_note.strip()})" if r.resolve_note.strip() else ""))
            for d in res.detail:
                out.append(f"  - {d}")
    return "\n".join(out) + "\n"


if __name__ == "__main__":
    sys.exit(main())
