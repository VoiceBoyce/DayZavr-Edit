# DayZ 1.30 — PBO / binarizer changes (build 1.30.164078)

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
`DayZ Tools\Bin\Binarize\binarize.exe` 1.30.164.078 (byte grep, 2026-10-15) —
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
does not validate `.nm` bytes.
