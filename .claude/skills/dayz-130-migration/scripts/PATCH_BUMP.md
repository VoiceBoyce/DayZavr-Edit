# Refreshing the scanner for the next DayZ patch

Used for 1.30.164078 → 1.30.164200 on 2026-10-21. Run everything with `python -I`, in Git Bash (binary redirects of
`gh api` are safe there; Windows PowerShell 5.1 `>` re-encodes them and corrupts the tarball).
Downloaded trees are untrusted data: own empty folders, outside the skill, never run anything from inside them.

## What is where

| File | Built by | Meaning |
|---|---|---|
| `data/api130-index.json` | `build_api_index.py P:\scripts` | the NEWEST build (the scanner's "1.30") |
| `data/api130_<build>-index.json` | `build_api_index.py` | an older 1.30 build, the "old" side of a hotfix step |
| `data/api129-index.json` | `build_api_index.py "O:\DayZ 1.29 Старый распакованный клиент\scripts"` | 1.29 |
| `data/delta.json` | `build_api_delta.py api129 api130_164078` | 1.29 → 1.30.164078 (step `1.29 -> 1.30`) |
| `data/delta-<old>-to-<new>.json` | `build_api_delta.py` | one hotfix step |
| `data/cfgdelta-<old>-to-<new>.json` | made from the two servers' PBOs (not by this skill; schema = `cfgdelta.json`: `added` / `removed` / `changed`) | optional config checks of the step |
| `references/api-delta-<old>-to-<new>.md` | `build_api_delta.py --md` + a hand-written notes file | human-readable delta |

Every index records its own `.c` file set (`"files"`), so a later rebuild of the same build can be checked exactly.

## Steps (example: OLD = 1.30.164200, NEW = 1.30.16XXXX)

```bash
SKILL="$HOME/.claude/skills/dayz-130-migration"
WORK="<your scratch folder>/bump"            # new, empty, outside the skill
OLD=1.30.164200; NEW=1.30.16XXXX
OLDN=${OLD##*.}; NEWN=${NEW##*.}               # 164200 / 16XXXX
mkdir -p "$WORK/dl" "$WORK/old" "$WORK/new"
```

### 1. Before (or right after) Steam updates P:\ — archive the current newest index
`data/api130-index.json` was built from `P:\scripts` while P:\ still held OLD, so it *is* the OLD index:
```bash
cp "$SKILL/data/api130-index.json" "$SKILL/data/api130_${OLDN}-index.json"
```
Then make sure P:\ holds NEW (`dayz-preflight`; `cat P:/scripts.txt` → `version=<Scripts Rev>`).

### 2. Download the two Bohemia script trees
```bash
gh api "repos/BohemiaInteractive/DayZ-Script-Diff/commits?per_page=20" \
   --jq '.[] | [.sha[0:7], .commit.author.date, (.commit.message|split("\n")[0])] | @tsv'
OLD_SHA=<sha of "Build $OLD">; NEW_SHA=<sha of "Build $NEW">          # 164078: e3dda95, 164200: e8204c6
gh api "repos/BohemiaInteractive/DayZ-Script-Diff/tarball/$OLD_SHA" > "$WORK/dl/old.tar.gz"
gh api "repos/BohemiaInteractive/DayZ-Script-Diff/tarball/$NEW_SHA" > "$WORK/dl/new.tar.gz"
tar -xzf "$WORK/dl/old.tar.gz" -C "$WORK/old" --strip-components=1
tar -xzf "$WORK/dl/new.tar.gz" -C "$WORK/new" --strip-components=1
gh api "repos/BohemiaInteractive/DayZ-Script-Diff/compare/$OLD_SHA...$NEW_SHA" \
   --jq '.files[] | [.status, .filename] | @tsv' > "$WORK/files.tsv"       # added / removed / modified
```
The repo keeps **stale files the game no longer ships** (e.g. `5_mission/mission/missionbenchmark.c`,
`4_world/classes/vehiclemanager.c`): never index a GitHub tree unfiltered.

### 3. Check the trees against the indices (same build ⇒ 0 differences)
```bash
cd "$SKILL/scripts"
# OLD: the archived index lists its own file set
python -I build_api_index.py "$WORK/old/scripts" "$WORK/chk_old.json" --files-of-index "$SKILL/data/api130_${OLDN}-index.json"
python -I compare_api_index.py "$SKILL/data/api130_${OLDN}-index.json" "$WORK/chk_old.json"
```
If step 1 was missed (P:\ already updated, as on 2026-10-21), build the OLD index from the GitHub tree instead,
restricted to the real OLD file set = files under P:\scripts (drops the stale ones; files the patch ADDED are not
in the old tree anyway) + files the patch DELETED (`removed` lines of files.tsv, without the `scripts/` prefix):
```bash
python -I build_api_index.py "$WORK/old/scripts" "$SKILL/data/api130_${OLDN}-index.json" --only-files-in P:/scripts \
       --keep $(awk -F'\t' '$1=="removed" && $2 ~ /^scripts\/.*\.c$/ {sub(/^scripts\//,"",$2); print $2}' "$WORK/files.tsv")
```

### 4. Build the NEW index from P:\ and cross-check it with the GitHub NEW tree
```bash
python -I build_api_index.py P:/scripts "$SKILL/data/api130-index.json"
python -I build_api_index.py "$WORK/new/scripts" "$WORK/chk_new.json" --files-of-index "$SKILL/data/api130-index.json"
python -I compare_api_index.py "$SKILL/data/api130-index.json" "$WORK/chk_new.json"        # expect: differences: 0
```

### 5. Delta + readable reference
Write `$WORK/notes.md` first (compile breakers / behaviour changes / official patch notes, with `P:\scripts\…:line`);
look at `files.tsv`, `gh api …/compare/…` patches and the official notes. Then:
```bash
python -I build_api_delta.py "$SKILL/data/api130_${OLDN}-index.json" "$SKILL/data/api130-index.json" \
       "$SKILL/data/delta-$OLD-to-$NEWN.json" --label "$OLD -> $NEW" \
       --md "$SKILL/references/api-delta-$OLD-to-$NEWN.md" --md-notes "$WORK/notes.md"
```
(File names follow the existing pattern `delta-1.30.164078-to-164200.json`.) The summary line must end with
`missing: none` — otherwise one index came from an older `build_api_index.py` and lacks enum values / field details.
Optional config delta: put it at `data/cfgdelta-$OLD-to-$NEWN.json`.

### 6. Wire the step into `scan_mod_130.py`
- `HOTFIX_STEPS`: the previous last step now ends at the archived index — change its `"new_index"` from
  `"api130-index.json"` to `"api130_${OLDN}-index.json"`; append
  `{"from": OLD, "to": NEW, "old_index": "api130_${OLDN}-index.json", "new_index": "api130-index.json", "delta": "delta-$OLD-to-$NEWN.json", "cfgdelta": "cfgdelta-$OLD-to-$NEWN.json"}`.
  `--from` gets the new choice automatically; `--from 1.29` runs every step.
- Generic rules work from the delta alone: removed classes/methods/fields, changed signatures (overrides and call arity),
  methods/fields newly added to ancestors (`shadows-new-vanilla`), enum value shifts, classes moved under an `#ifdef`,
  changed method bodies, config deltas.
- Hand-written fix hints: `STEP_HINTS["<NEW>"] = {"<member name>": "…"}`. Rules for one specific API belong in
  `scan_scripts_hotfix` behind a data check (see `hascrew_new`, `wb_typename`) so older steps stay silent.
- Re-read every vanilla `file:line` the scanner prints against the new `P:\scripts`:
  `grep -nE "\.c:[0-9]" scan_mod_130.py` (164200: transport.c:830/835, constructionbase.c:180/191, itembase.c:2800 moved).

### 7. Tests
- Add `tests/fixture_mod_130_${OLDN}/` (a small mod written for OLD that hits each new rule) and its EXPECTED set /
  message checks in `tests/test_scan_fixture.py`; keep the older fixtures (update only line numbers that legitimately moved).
- `python -I tests/test_scan_fixture.py` → `ALL PASS`.

### 8. Only when `es_parse.py` / `build_api_index.py` changed
Rebuild every index (1.29 from `O:\DayZ 1.29 Старый распакованный клиент\scripts`, the archived 1.30 ones from their
GitHub trees with `--files-of-index`), then `delta.json`:
```bash
python -I build_api_delta.py "$SKILL/data/api129-index.json" "$SKILL/data/api130_164078-index.json" "$SKILL/data/delta.json" --label "1.29 -> 1.30.164078"
```
and diff the new delta against the old one before replacing it.

### 9. Beyond the scanner: the rest of a patch bump (learned on 1.30.164200, 2026-10-21)
1. **Version check first**: `python -I ../../dayz-preflight/preflight.py` — exe FileVersion and `P:\scripts.txt version=`
   must be the new build (update `TARGET_BUILD` / `TARGET_SCRIPTS_REV` in `preflight.py` and `_shared/dayz-version-policy.md`).
2. **Official notes**: BI forums "DayZ Update 1.xx" topic (forums.dayz.com, 403 to tools — use the patchbot.io mirror).
   Engine fixes that are not in the script diff (FindFile, `#include`, SRGB, launch params, PBO properties) live only there.
3. **Data side**: keep a copy of the OLD dedicated server (e.g. `O:\DayZ Server 1.30\`) before Steam updates it, then
   compare PBOs entry by entry against the new install (config.bin derap → class/key delta in the `cfgdelta.json`
   schema; .p3d/.rvmat/.layout/.imageset; `navmesh.nm` header version = uint32 at offset 4). Write
   `data/cfgdelta-<old>-to-<new>.json` + `references/config-delta-<old>-to-<new>.md`; add the new navmesh version to
   `NAVMESH_VERSION` in `scan_mod_130.py`.
4. **Skill refs**: `python -I ../../_shared/tools/remap_vanilla_refs.py --old <old scripts> --new P:\scripts --skills ~/.claude/skills`
   (dry run, review, then `--apply` once; it refuses a second apply via `remap_applied.log` — archive that log before
   the next patch). Known gaps: refs with a trailing `+`, alias refs (`cbase:123`), and documents whose header declares
   an older numbering (v1.24 / 1.29 without a per-ref label) — exclude those or list them in `--decisions`.
   Config line refs (`P:\DZ\…config.cpp:N`) are not remapped: take the stale list from the data-side compare.
5. **Content**: fan out one subagent per skill domain (base building+doors; vehicles+physics+fx+sound; script core+UI+
   persistence; tooling+env+NV+AI), each owning disjoint skill folders, then one QA agent ([EXACT] quotes vs P:\,
   leftover markers, frontmatter `description` ≤ 1024 chars).
6. **Mods**: scan only the owner's INT mods (`mod_Intenso_*`) with `--from <old>`; measure community claims in game
   before writing them into a skill (164200: "all `\\` paths broke" was false — only a leading separator fails).
7. Rebuild other vanilla indices: Knowledge Pack `dayz-script-validator` (`build_vanilla_index.py --vanilla-root P:\scripts`,
   then `vanilla_control.py --update` with triage notes) and dayz-mcp `knowledge_build(layer="core")` per project.
