# DayZ version policy for this skill set

**Target: DayZ 1.30 stable, build `1.30.164200`** (client, DayZDiag and DayZServer on this machine,
released 2026-10-21; Scripts Rev 127662, exe FileVersion `1.30.164.200`, ProductVersion `1.30.164200.56`).
Every DayZ skill here describes 1.30 unless a sentence says otherwise.

| Where | What it is |
|---|---|
| `P:\` (`P:\scripts`, `P:\DZ`, `P:\gui`) | Unpacked **1.30.164200** (`P:\scripts.txt` `version=127662`) — the ground truth. Grep here before you write an API, a config key or a path. |
| `O:\DayZ 1.29 Старый распакованный клиент\` | Unpacked **1.29** — only to understand what an old mod was written against. Never cite it as current. |
| `dayz-130-migration` skill | Porting a 1.29 (or older) mod to 1.30: checklist, scanner, full API/config delta. |
| `C:\Users\intenso2891\.claude\dayz-knowledge-pack\` | `<KNOWLEDGE_PACK>` root: `tools\` (dayz-script-validator, dayz-ui-lab, py3d, …), `knowledge\`, `examples\`. Skills that say "relative to the Knowledge Pack root" mean this folder. |

## Labels used in the skills

- No label → true on 1.30.164200. `[EXACT] path:line` refs point at `P:\` (1.30) unless written `1.29: path:line`.
- **`Before 1.30:`** → the legacy behaviour. It exists only so you recognise it in an old mod and port it.
  Never write new code from a `Before 1.30` sentence.
- **`(measured on 1.30 Exp 164014)`** → a runtime observation made on the 1.30 Experimental build that nobody
  has re-measured on stable. The static API it relies on was re-checked on 164078; the behaviour is very likely
  the same, but say "measured on Exp" when you rely on it.
- **`(measured on 1.30.164078)`** → a runtime observation from the previous 1.30 stable build whose API did not
  change in 164200. Valid unless its area is listed in the 164078 → 164200 delta (Patch history below); then
  re-measure before you rely on it. Sentences that say "re-verified / re-read / corrected on 1.30.164078" record
  when a static check was made; their line refs were renumbered to 164200 by `_shared/tools/remap_vanilla_refs.py`.
- **`(measured on 1.29)`** → a runtime observation from 1.29 whose API did not change in 1.30 (checked against the
  delta). Treat it as still valid; re-measure if the result looks off.
- `[reported: <source>]` → a third-party report (e.g. bzed `dayz-dev-plugin` guide, tested on a Linux 1.30 server)
  that this machine has not reproduced.

## Rules

1. Write plain 1.30 code. Do **not** gate on `#ifdef DAYZ_1_30`: the engine defines only the current version's
   macro, so that block silently disappears on 1.31. If a mod must still load on 1.29, put the *old* code under
   `#ifdef DAYZ_1_29` and the 1.30 code in `#else`. Evidence: 1.29 vanilla itself gates on `#ifdef DAYZ_1_29`
   (1.29: `scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayerregistry.c:116`); [reported: bzed] a 1.30 server's
   `defines:` log line lists `DAYZ_1_30` and no `DAYZ_1_29`.
2. `DEVELOPER`, `DIAG_DEVELOPER`, `BUILD_EXPERIMENTAL` are build flavours (DayZDiag / Experimental), never a
   version test.
3. Before trusting a vanilla line number from any skill, grep `P:\scripts` — 1.30 shifted most files.
4. If a skill and `P:\` disagree, `P:\` wins: fix the skill sentence (or flag it to the user) instead of
   coding around it.
5. Persistence: `GAME_STORAGE_VERSION` is **144** on 1.30 (`P:\scripts\3_game\global\game.c:5`; 142 on 1.29).
   A storage written by 1.30 is not readable by a 1.29 server — back up `storage_1` before the first 1.30 boot.

## Patch history (1.30 stable)

| Build | Released | Scripts Rev | What changed for mods |
|---|---|---|---|
| 1.30.164078 | 2026-10-15 | 127250 | First 1.30 stable; the skills were verified on it (1.29 → 1.30 delta: `dayz-130-migration`). |
| 1.30.164200 | 2026-10-21 | 127662 | `vector.Cross` removed (use `a * b`); base-building support API signatures (`GetSupportProvided/Requirement(int, out int)` → `bool`, utility-part update); new `Transport.HasCrew()` (proto native) and `Transport.IsMoving()`; `NVTypes` values after `NV_OPTICS_UNDERGROUND` shifted (+1/+2); server-side `CCTCursor*` distance check; target actions no longer lock the target; motorbike light selections (`FrontOn/Off`, Motorbike_02 brake index 6), `MotorbikeScript.CanReleaseAttachment` (nothing detaches while engine on / crew present). **Workbench script API only under `#ifdef WORKBENCH`** (`Workbench`, `WBModuleDef`, `WorkbenchPluginAttribute`, `ButtonAttribute` …; `WorldEditor*`/`ResourceBrowser` removed) — unguarded use = `Can't find variable 'Workbench'`. Engine, [reported: BI 1.30.164200 notes]: `FindFile` fixed (requested FS, paths normalised to `/`), `#include` fixed, circular-inheritance compile error fixed, `ImageWidget.LoadImageFile` SRGB fix (script-loaded images were too bright), PBO named property `fsShadowing=1`, launch parameter `-resolveFilePatchingUsingEnfusion` → `-loadRVFilesViaEnfusionFS`. **Navmesh format v7 → v9** (all vanilla `navmesh.nm`; the exe now checks for 9 — custom map navmeshes built on 164078 must be regenerated). Exe FileVersion `1.30.164.200`, product `1.30.164200.56`. |

Official notes ("Steam Stable 1.30 Update 2 / Patch 1 for Update 1.30"): forums.dayz.com/topic/266388 (mirror
patchbot.io/games/dayz/articles/1866). Script diff: github.com/BohemiaInteractive/DayZ-Script-Diff `e3dda95…e8204c6`.

Full 164078 → 164200 script delta: `dayz-130-migration` → `references/api-delta-1.30.164078-to-164200.md`.
Bump procedure for the next patch: `dayz-130-migration/scripts/PATCH_BUMP.md` plus
`_shared/tools/remap_vanilla_refs.py` (renumbers the vanilla `path:line` refs in every skill; dry run first).
