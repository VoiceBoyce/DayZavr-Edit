# DayZ 1.30 — PBO / binarizer changes (build 1.30.164200)

Companion to `SKILL.md` § DayZ 1.30 changes. Folder structure, stringtable.csv,
include-list, and ODOL non-determinism are unchanged on 1.30 (measured on 1.29;
the 1.30 Tools ship the same AddonBuilder 1.0.240.639). This file is the 1.30
binarizer and CfgMods contract.

## Duplicate **members** abort binarize

[CHANGELOG 1.30]:

> Changed: Binarizer will now stop binarising if a member is defined multiple
> times in a config file

Before 1.30: the pre-build check only covered "Duplicate class names"; a
property repeated inside one class did not stop the binarizer, so an old mod
can carry repeats that built fine on 1.29.

This is a **build blocker**. AddonBuilder / binarize that used to succeed on
a repeated `scope`, `displayName`, `weight`, `hiddenSelections[]`, or an inner
class name now **stops**. The check is engine-side (no script involved). The
message text `%s: Member already defined` is in
`DayZ Tools\Bin\Binarize\binarize.exe` 1.30.164.078 (byte grep, 2026-10-15) and
still in 1.30.164.200 (byte grep, 2026-10-21) —
grep the AddonBuilder/binarize log for it. The stop itself has not been
measured on this machine.

[DESIGN] Audit every `config.cpp` (and inherited fragments) for a member
written twice in the **same class body**. Vanilla-style `class Foo; class Foo:
Bar {}` (forward + definition) is not the same bug; the changelog is
**multiple definitions of a member inside one block**. `x[] += {...}` appends
are not repeats.

`config_cpp_validator.py` (`validation-scripts.md` §1) checks this per class
body (case-insensitive, scope-aware). It reports zero hits on the 208 vanilla
`P:\DZ\**\config.cpp` of 1.30.164078 (run 2026-10-15), so a hit in a mod is
worth reading.

## CfgMods script-module paths: forward slashes

[reported: bzed, Linux dedicated server 1.30.164014] With
`files[] = {"MyMod\scripts\4_World"};` the server loads the mod's config and
shows its `CfgMods` defines, but compiles **none** of its scripts and prints
no error (the World module's file count stays at vanilla). The fix is
`files[] = {"MyMod/scripts/4_World"};`. Windows servers were not tested by the
reporter.

Vanilla 1.30 writes its own script-module paths with forward slashes:
`P:\DZ\data_sakhal\config.cpp:26` `"DZ/data_sakhal/scripts/4_World"`.
Forward slashes therefore work on Windows too: use them in every
`engineScriptModule` / `gameLibScriptModule` / `gameScriptModule` /
`worldScriptModule` / `missionScriptModule` entry, in your own mods and when
auditing a dependency's debinarized `config.bin`. `$PBOPREFIX$` keeps its
backslashes; this rule is about `files[]`.

Pre-build check 2 in `SKILL.md` covers duplicate class names, duplicate
members **and** script-module path slashes.

## Unpacked mods do not retire the PBO gate

[CHANGELOG 1.30]: `-mod` can point at a folder and RV configs load unpacked.
That is a **diag iteration** path (`dayz-test-ingame`). Workshop /
dedicated shipping still needs a PBO. Do not skip the packer for a release
because unpacked load exists.

Navmesh `.nm` regeneration [CHANGELOG 1.30] is a terrain/Workbench task, not
a PBO linter check. Flag it in the release notes of a custom map; this skill
does not validate `.nm` bytes. On 1.30.164200 the vanilla format is v9 (164078:
v7, 1.29: v6; uint32 at offset 4 after `TESM`), so a map built for 164078 needs
a new navmesh (`dayz-ai-patterns` §Navmesh).

## 1.30.164200: PBO named property `fsShadowing=1`

[reported: BI 1.30.164200 notes, not yet measured] Modding, "Added":

> Named property to PBO 'fsShadowing=1' which will allow loading of the PBO
> even if it's prefix matches another, to allow overriding of Enfusion config
> files such as .layout' through 'Variants'

A named property is a `key=value` pair in the PBO header, next to `prefix`,
`product` and `version` (all 158 vanilla 1.30.164200 `.pbo` carry exactly these
three; header scan 2026-10-21). Static evidence that the engine reads the new key:
`fsShadowing` follows `prefix` among the PBO-loader strings of `DayZ_x64.exe`,
`DayZDiag_x64.exe` and `DayZServer_x64.exe` 1.30.164.200, beside
`Version: %s (Shadowed)` (byte grep 2026-10-21).

Packers (2026-10-21; header of a test PBO read back, game not started with it):
`FileBank.exe` writes any property (`-property fsShadowing=1`, measured); Mikero
`MakePbo.exe` 2.16 turns extra `var=value` lines of `$PBOPREFIX$` into properties
(`prefix=<p>` + `fsShadowing=1`, measured); AddonBuilder 1.0.240.639 hard-codes its
FileBank call (`-property "product=dayz ugc" -property prefix={1}`, plus `version`
with `-pboversion`) and has no option for another key; dayz-mcp `mod_build` passes
only `prefix`. Header reader and the table: `SKILL.md` § Named PBO properties and
`fsShadowing`.

Open (measure before relying on it): which PBO wins when two share a prefix,
whether RV files (`config.cpp`, scripts, `.p3d`) are shadowed at all or only
Enfusion config files, and what 'Variants' refers to.
