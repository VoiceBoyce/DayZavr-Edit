---
name: dayz-preflight
description: "Use when: P:\\ not mounted, DayZ Tools missing, workshop folder, vanilla data unpacked, before building/packing, AddonBuilder path, DayZDiag locatable, is P:\\ / the game 1.30. Read-only env check incl. version probe (P:\\scripts = 1.30, exe FileVersion 1.30.x). Not launch/test: dayz-test-ingame. Also: cacheP3D, unpacked -mod folder, Workbench 1.30."
---

# /dayz-preflight

Verify the DayZ modding environment before doing any DayZ work. Halts with a clear error if `P:\` is not mounted (per `_shared/dayz-conventions.md`); warns on optional checks so the user can decide whether to proceed.

> Target: DayZ 1.30 stable (1.30.164200). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

**Read-only.** Preflight inspects state and reports. It never mounts drives, creates junctions, or otherwise mutates the filesystem — fixes belong to dedicated setup skills.

**Non-redundancy note:** If the flow goes through dayz-test-ingame, its `dayz-test.ps1` preflight already checks AND auto-fixes these (junction creation etc.) — this skill is the read-only/warn variant.

Follow `_shared/dayz-conventions.md`.

## What it checks

| # | Check | Severity |
|---|---|---|
| 1 | `P:\` drive is mounted | **hard fail** — exit 1 |
| 2 | DayZ Tools installed (AddonBuilder.exe locatable) | warn |
| 3 | Vanilla DayZ data unpacked under `P:\` | warn |
| 4 | `P:\Mods\` is a directory junction to `<DayZ install>\!Workshop\` (not a regular folder, not missing, not dangling) | warn — build-pbo hard-fails on the same condition |
| 5 | `DayZDiag_x64.exe` locatable (required for `-filePatching` iteration; retail exes block past loading) | warn |
| 6 | **Version probe, vanilla scripts:** `P:\scripts\3_game\vehicles\motorbike.c` exists (new in 1.30) **and** `P:\scripts\3_game\global\game.c` declares `GAME_STORAGE_VERSION = 144` (`game.c:5` on 1.30.164200; it is `142` on 1.29). 142 / no `motorbike.c` → `P:\ holds pre-1.30 vanilla data; skills target 1.30 - re-extract`. >144 → newer than the skills' target. | warn |
| 7 | **Version probe, scripts revision:** `P:\scripts.txt` `version=` equals the target Scripts Rev `127662` (1.30.164200). Another value → WARN `P:\ extracted from another build` (`127250` = 1.30.164078): the skills' vanilla line refs are numbered for the target build. Missing file / no `version=` line → WARN. | warn |
| 8 | **Version probe, executables:** FileVersion of `DayZ_x64.exe`, `DayZDiag_x64.exe` and `DayZServer_x64.exe` (each one that is locatable) is `1.30.x`. Read with `ctypes` + `version.dll` (no pywin32, no subprocess). Not 1.30 → WARN with the path; 1.30 but not build 164200 → OK with a "skills verified on 1.30.164200" note. | warn |

Exit code 0 = ready (or warnings only). Non-zero = hard environment issue, named in stderr. The version probe never changes the exit code.

Why the version probe: every DayZ skill cites `P:\scripts` as the 1.30 ground truth. A `P:\` still holding 1.29 data (or a game that Steam has not updated) makes every "grep P:\ before you write an API" check confirm the wrong version.

## How paths are resolved

Paths are not blindly hard-coded. Resolution order, first hit wins:

| What | Order |
|---|---|
| **DayZ Tools install root** | 1. `DAYZ_TOOLS_PATH` env var → 2. Windows registry (`HKLM\SOFTWARE\WOW6432Node\Bohemia Interactive\DayZ Tools` → `Path` value, plus the `HKLM` non-WOW6432 and `HKCU` siblings) → 3. Steam uninstall key `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 830640` → `InstallLocation` → 4. Common Steam paths (`C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools`, then `C:\Program Files\…`). A candidate counts only if `Bin\AddonBuilder\AddonBuilder.exe` exists under it. |
| **DayZ game root** (`DayZ_x64.exe`, `DayZDiag_x64.exe`) | 1. `DAYZ_GAME_PATH` (`DAYZ_DIAG_PATH` for the Diag exe) → 2. Steam uninstall key `Steam App 221100` → `InstallLocation` → 3. `<steamapps>\common\DayZ` next to the DayZ Tools root → 4. Common Steam paths. Steps 2–3 find a game in any Steam library (e.g. `T:\…\steamapps\common\DayZ`), not only under `C:\Program Files`. |
| **DayZ Server root** (`DayZServer_x64.exe`) | 1. `DAYZ_SERVER_PATH` → 2. Steam uninstall key `Steam App 223350` (its `InstallLocation` is often empty) → 3. `<steamapps>\common\DayZServer` next to the game root → 4. Common Steam paths. |
| **Vanilla data root** | 1. `DAYZ_VANILLA_DATA_PATH` env var → 2. Canonical names on P:\: `P:\dz`, `P:\DZ`, `P:\dta`. A candidate counts only if it exists and is non-empty. |
| **Vanilla scripts** (version probe) | Fixed: `P:\scripts` (the path every skill cites). |
| **`P:\` and `P:\Mods\`** | Fixed by DayZ engine convention — not configurable. |

To override either env var, set it in your shell or in the user/system environment before running any DayZ skill. Example (PowerShell): `$env:DAYZ_TOOLS_PATH = 'D:\Games\Steam\steamapps\common\DayZ Tools'`.

The `find_dayz_tools()`, `find_dayz_game()`, `find_dayz_diag()`, `find_dayz_server()`, `find_vanilla_data()` and `read_exe_version()` helper functions in `preflight.py` are reusable — future DayZ skills (e.g. `dayz-pbo-build`) should import them instead of re-implementing path discovery.

## How to run

This skill's directory is
`<skills>\dayz-preflight\`.

```cmd
python -I <this-skill's-dir>\preflight.py
```

## When to run

- Before invoking any other DayZ skill.
- After a fresh clone of the repo.
- After a fresh boot of the workstation — `P:\` does not auto-mount; you have to open DayZ Tools and mount it (or use the Tools "Mount P drive" command) at the start of each session.

## Output

Plain text, one line per check:

```
DayZ preflight

[OK]    P:\ is mounted
[OK]    DayZ Tools found: C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools
[OK]    DayZDiag_x64.exe found: C:\Program Files (x86)\Steam\steamapps\common\DayZ\DayZDiag_x64.exe
[OK]    Vanilla data found: P:\dz
[OK]    P:\Mods -> C:\Program Files (x86)\Steam\steamapps\common\DayZ\!Workshop
[OK]    P:\scripts is DayZ 1.30 vanilla (GAME_STORAGE_VERSION = 144, motorbike.c present)
[OK]    P:\scripts.txt version=127662 (Scripts Rev of 1.30.164200)
[OK]    DayZ_x64.exe 1.30.164.200 (product 1.30.164200.56)
[OK]    DayZDiag_x64.exe 1.30.164.200 (product 1.30.164200.56)
[OK]    DayZServer_x64.exe 1.30.164.200 (product 1.30.164200.56)

Preflight complete.
```

`FileVersion` of the 1.30 stable exes is `1.30.164.200`; the `ProductVersion` string carries the build as `1.30.164200.56` and `P:\scripts.txt` reads `version=127662` (measured on this machine, 2026-10-21). The skills' "build 164200" is that number. The previous stable read `1.30.164.078` / `1.30.164078.38` / Scripts Rev 127250 (measured 2026-10-15); patch history: `../_shared/dayz-version-policy.md`.

A `P:\` extracted from another build (exes already updated, `P:\` not re-extracted) prints:

```
[WARN]  P:\scripts.txt version=127250, expected 127662 (1.30.164200): P:\ extracted from another build (1.30.164078)
        Vanilla line refs in the skills are numbered for the target build; re-extract scripts.pbo
        from the installed game, or grep P:\scripts before trusting a line number.
```

A `P:\` that still holds 1.29 data prints instead (exit code stays 0):

```
[WARN]  P:\ holds pre-1.30 vanilla data; skills target 1.30 - re-extract
        GAME_STORAGE_VERSION = 142 (1.30 = 144, 1.29 = 142), motorbike.c missing.
        Re-extract with DayZ Tools 'Extract Game Data' from a 1.30 install.
```

If `P:\` is not mounted:

```
DayZ preflight

[FAIL]  P:\ is NOT mounted
        Open DayZ Tools and mount the P drive (Tools menu > Mount P drive),
        or map it directly: subst P: "<dayz-projects>"
```

Exit 1, and downstream DayZ skills should refuse to run.

## Do not

- Don't try to mount `P:\` programmatically — it's a DayZ Tools function and assumes Tools is installed and configured. Surface a clear message and let the user mount it. (Manual alternative when Tools' mount is unavailable: `subst P: "<dayz-projects>"` — the operational route registered in dayz-test-ingame.)
- Don't gate on the DayZ Tools / vanilla data / Workshop folder checks — they're warnings, not errors. The user may have a custom layout.
- Don't reintroduce hard-coded paths in other DayZ skills. Import `find_dayz_tools` / `find_vanilla_data` from this skill so resolution stays consistent (env var → registry → fallback).

## SP-010 — Debug/diagnosis preflight extension

For DEBUG/diagnosis tasks (not just builds), also verify/request mounted: (1) unpacked vanilla scripts (`P:\scripts`) — the authoritative source for engine/action/CarScript behavior; (2) the deployed mod tree (`P:\<Mod>`) to confirm WHAT is actually in the build; (3) the working reference mod. Without these, diagnosis degrades to memory/diff. [LL-075, LFQuad 2026-05-25]

## SP-374 — Object Builder's external viewer points at a non-existent exe by default

Buldozer is the cheap answer to "how does it look?" — materials, alpha and animations previewed
without paying an in-game cycle. On a stock install the viewer is wired to a path that does not
exist, so it silently never gets used and the question goes in-game instead.

Check the registry value, and that the exe behind it is really there:

```powershell
$v = (Get-ItemProperty 'HKCU:\Software\Bohemia Interactive\Dayz Tools\Object Builder\CConfig').'%External Viewer'
$exe = ($v -split ' -buldozer')[0].Trim('"')
Test-Path $exe
```

FAIL with the real path as the suggestion: `<steamapps>\common\DayZ\DayZ_x64.exe`. The DayZ Tools
installer writes `<steamapps>\common\DayZ_x64.exe` — without the `DayZ\` directory — which is why
the default is broken out of the box.

Measured on this box: configured path `Test-Path` = False, real path = True, `P:\Buldozer` present.
Cost of not checking it: ~20 SUB_BRZ sessions paid in-game cycles for "does it show?" questions
(label alpha, needle size, quad behind the screen) with zero mentions of Buldozer in the project
handoff.

Not verified yet: that Buldozer opens a *custom* `.p3d` with its rvmats. Do not promote this to
`dayz-model-pipeline` / `dayz-vehicles` as a visual gate until that part is measured.

## DayZ 1.30 changes

Checks 1–5 are version-independent and unchanged on 1.30. Checks 6–8 (the version probe) are new for 1.30
stable: they confirm that `P:\scripts` and the installed exes are the 1.30 the skills describe (check 7, the
`scripts.txt` revision, was added with 1.30.164200, when a patch shifted vanilla line numbers inside 1.30).

What 1.30 changes around this skill (Tools/launch awareness; preflight itself still does not launch):

- `-cacheP3D=0/1` next to unbinarized `.p3d` speeds later Game/Buldozer launches [CHANGELOG 1.30]. The
  `cacheP3D` string is present in `DayZ_x64.exe`, `DayZDiag_x64.exe`, `DayZServer_x64.exe` and
  `DayZ Tools\Bin\Workbench\WorkbenchApp.exe` 1.30.164.078 (byte grep, 2026-10-15); its behaviour is not measured.
  Surface it when the user is about to iterate MLOD in Buldozer.
- `-mod` may point at a physical unpacked folder; RV configs load unpacked [CHANGELOG 1.30]. Preflight still
  checks `P:\` + the Workshop junction for the **packed** deploy path; unpacked iteration is `dayz-test-ingame`.
- Workbench 1.30 is installed with DayZ Tools: `DayZ Tools\Bin\Workbench\WorkbenchApp.exe`, FileVersion
  1.30.164.078. The exe contains `RagdollConfigEditWindow`, `ConfigEditor` and `Animation Editor` strings, which
  matches the changelog's Ragdoll editor / Enfusion config editor / Animation Editor [CHANGELOG 1.30]; none of
  them has been used from these skills yet.
- SP-374 Buldozer viewer path is unchanged as a check.

Operational flags and argv: `dayz-test-ingame` / `dayz-test-ingame/references/dayz-1-30-test-ingame.md`. Mesh cache: `dayz-model-pipeline` / `dayz-model-pipeline/references/dayz-1-30-model-pipeline.md`.
