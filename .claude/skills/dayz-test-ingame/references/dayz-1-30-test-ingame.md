# DayZ 1.30 — launch / session-test changes (build 1.30.164200)

Companion to `SKILL.md` § DayZ 1.30 changes. Diag-only managed lifecycle, absolute
`-mod` paths, and the filepatching-vs-PBO caveats still hold. This file is the
1.30 launch-flag and action-injection delta.

Script line numbers below point at `P:\scripts` (1.30.164200), re-checked
2026-10-15. The installed exes are 1.30.164.200 since 2026-10-21 (1.30.164.078
before); the new flags exist as strings in them, but their runtime behaviour is
[CHANGELOG 1.30] (not measured here).
Runtime observations from the pre-release build are labelled
"(measured on 1.30 Exp 164014)".

## Unpacked `-mod` folder (configs without packing)

Before 1.30: this skill's loop was "re-pack the PBO, deploy it where the
engine reads mods, and launch DayZDiag". [CHANGELOG 1.30]:

> Changed: Mods now have their own named filesystem
> Changed: Improved unpacked mod workflow; -mod can now point to a physical folder
> Changed: RV configs can now be read when loaded as an unpacked mod

[DESIGN] For **config.cpp / script** iteration, `-mod` may name the addon
**source folder** (still prefer absolute paths; the silent-no-mount gotcha of
bare `@Name` is unchanged). `.p3d` / `.paa` still follow the existing
filepatching-does-not-reload-binaries rule. A shipping test still packs.

[EXACT] Measured bound of that [DESIGN] (measured on 1.30 Exp 164014, dedicated server, 2026-09-27): an unpacked `-mod` folder mounts as a `relative directory` (RPT: `Adding relative directory ...` vs a PBO's `Adding package ... (pbo count: N)`); with a `CfgMods` block it injects the mod's define into the script modules and loads its scripts, but it does NOT merge its `config.cpp` into the runtime config tree — `ConfigIsExisting("CfgPatches <mod>")` stays false. World configs (`CfgWorlds` terrain `Holes`) therefore require a PACKED PBO: unpacked, `SurfaceIsHole` stays false and the RPT logs `No entry 'config.bin/CfgWorlds/<world>.Holes'`; the same PBO mounted and measured true when packed (the PBO may carry `config.cpp` as text, no binarize needed). Measured in game on 1.30 Exp 164014; not re-measured on stable. Note the tension with the changelog line "RV configs can now be read when loaded as an unpacked mod": trust the measurement until a stable run says otherwise.

## New launch flags

[CHANGELOG 1.30]:

> Added: '-resolveFilePatchingUsingEnfusion=0/1' for file patching to
> prioritize looking through named enfusion filesystems before checking
> launched directory (requires -filepatching, always enabled in Workbench)
> Added: '-cacheP3D=0/1' command line launch option, creates .p3dcache files
> next to .p3d files to allow for quicker subsequent launches of the
> Game/Buldozer when working with unbinarized assets

**Renamed in 1.30.164200** [reported: BI 1.30.164200 notes, Modding: "Renamed
'resolveFilePatchingUsingEnfusion' to 'loadRVFilesViaEnfusionFS'"]: the flag
is now `-loadRVFilesViaEnfusionFS=0/1`. The changelog quote above keeps the
1.30.164078 name. The value form `=0/1` is assumed unchanged (not checked).

[DESIGN] Append to Diag argv when iterating unpacked / MLOD assets, for
example:

`-filePatching -loadRVFilesViaEnfusionFS=1 -cacheP3D=1`

(on 1.30.164078: `-filePatching -resolveFilePatchingUsingEnfusion=1 -cacheP3D=1`).
Requires `-filePatching`. Workbench always enables the resolve flag.
`.p3dcache` siblings are local caches — do not commit them as product.

Static check (2026-10-15): the strings `cacheP3D`, `resolveFilePatchingUsingEnfusion`
and `limitFPS` were present in `DayZ_x64.exe`, `DayZDiag_x64.exe` and
`DayZServer_x64.exe` 1.30.164.078; `headlessMode` only in `DayZDiag_x64.exe`.
Re-check 2026-10-21 on the 1.30.164.200 exes: `cacheP3D`, `loadRVFilesViaEnfusionFS`
and `limitFPS` present in all three, `resolveFilePatchingUsingEnfusion` in none.
That proves the switches are parsed, not what they do. A launch file that still
passes the old name should be updated; `dayz-130-migration`'s scanner reports it
as `launch-param-renamed`.

## Script log `defines:` line, compile crashes, profile folder

- `defines:` contains `DAYZ_1_30` on 1.30 (measured on 1.30 Exp 164014, item 1
  of the next section; [reported: bzed] `DAYZ_1_30, SERVER, ...` on a 1.30
  server). The exes build it from the format string `DAYZ_%d_%02d`
  (`DayZDiag_x64.exe` 1.30.164.078), so only the current version's macro exists.
- [reported: bzed] On the pre-release 1.30 Linux server a script compile failure
  was followed by a segfault right after `Can't compile "World" script module!`.
  A crash at boot is first a compile error: read `script_*.log`.
- [reported: bzed, Expansion 1.9.74 announcement] On 1.30.164078 the `-profiles=` folder
  must sit inside the server exe's folder (a symlink there works) for mods that
  enumerate `$profile:` via `CF.FindFileEx` (CF 1.5.9+): the vanilla `FindFile()`
  ignored `$profile:` on 1.30.164078 (DZEXP-134) and the workaround resolves profile
  paths relative to the exe folder. [reported: BI 1.30.164200 notes] 1.30.164200
  fixes `FindFile` (searches the requested file system, normalises paths to `/`);
  not re-measured here, so test vanilla `FindFile("$profile:…")` before keeping the
  workaround. `FindFile`'s declaration did not change
  (`P:\scripts\1_core\proto\ensystem.c:520`, identical on 1.29), so no compile
  error warns you. The managed template's `<Mod>_dev\_server\profiles` is outside
  the DayZ folder; a Windows junction as the fix is untested here.

## Headless roboclients vs `Mode=all`

`CGame.IsHeadless()` (`P:\scripts\3_game\global\game.c:1130`) and
`IsHeadlessOrDedicatedServer()` (`:1136`) are new in 1.30 —
see `dayz-mod-workflow/references/dayz-1-30-mod-workflow.md`. This skill's
MCP composition still requires `-Mode all` for a **rendered** client
(`dayz-mcp-verify`: headless has no window to grab).

[DESIGN] A load-test roboclient is `IsHeadless()==true` and must not be used
as the capture peer. Script that skips VFX should use
`IsHeadlessOrDedicatedServer()` so roboclients match dedicated.

`IsHeadless` is absent from 1.29 (`1.29: scripts\3_game\global\game.c`, 0 matches).

Measured 2026-09-26 (measured on 1.30 Exp 164014): the headless client (`-headlessMode=1`)
crashes before loading `World`, so there was no headless load-test roboclient in
that build. Not re-tried on 1.30.164078 stable. Server-side dummy bots do the job instead; details and numbers in
`dayz-mcp-verify/references/dayz-1-30-mcp-verify.md` § Roboclients vs server-side
dummy bots.

## Launching DayZDiag directly (outside the managed lifecycle)

When these traps were measured (2026-09-26) the managed launcher ran the 1.29
install and the 1.30 Exp install was probed by hand (evidence in
`VAULT/AI/10_Projects/DayZ_MCP/lanes/2026-09-26-roboclient-130/`). Since
2026-10-15 the installed game itself is 1.30 (1.30.164078; 1.30.164200 since 2026-10-21), so any launch runs 1.30;
the traps are about the host and the launch, not the build. Six traps, each
costing a run (measured on 1.30 Exp 164014):

1. **The script log is the define oracle.** Each module logs
   `Module: <name>; …; defines: "…"`. Reading that line after a launch to the main
   menu (~20 s) answers "is X defined in this exe?" without writing a probe mod.
   It also shows the version define, `DAYZ_1_30`.
2. **From the Claude desktop app the game can read a dead Steam PID.** The app sees a
   private, frozen copy of `HKCU\Software\Valve\Steam\ActiveProcess` (MSIX registry
   virtualization, LL-516). After Steam restarts, a game launched from the app reads a
   dead PID, logs `Unable to locate a running instance of Steam` and shows an error
   dialog while `steam.exe` runs. Compare the app's view with the real value (WMI
   `StdRegProv`) and the live `steam.exe`; then launch through WMI
   (`Win32_Process.Create`) or sync the copy right before launching. Writing the
   value from a shell inside the app only fixes the private copy, and only until the
   next Steam restart.
   Two measured corollaries (2026-09-23/24, LL-510 / SP-420). Never START or restart Steam from a shell inside the sandboxed app: the new Steam registers in the private copy, and the REAL key can be left at `pid = 0, ActiveUser = 0` — a state the outside WMI check reports as broken while the app's own view looks healthy, and which `auto_remediate_steam` cannot fix because the worker sits inside the app too. Never write or delete another app's HKCU keys from an app shell: a write creates the private copy (shadow), a delete leaves a tombstone that hides the real key even after it is rewritten outside; the shadow only disappears with the app closed and the hive loaded offline. Preflight BOTH views: WMI `StdRegProv` reads the real hive, `Get-ItemProperty` inside the app reads the copy the launched client will see — require both to point at the live `steam.exe`. [EXACT] (measured, LL-510 / SP-420 / LL-516)
3. **A very long `-profiles=` path is a crash suspect.** A ~245-character profiles
   path ended in `0xC0000005` after 1 s with no log written; a short path did not
   repeat it. Steam was broken at the same time, so the cause is probable, not
   isolated. Keep profiles paths short.
4. **Launch DayZ with a whitelisted environment (LL-525).** A DayZ `ErrorMessage_*.mdmp` or
   crash minidump embeds the parent's whole environment block, and an agent shell
   usually carries API keys and passwords. On every crash the DayZ CrashReporter
   also sends "analytical data" before offering the dump. Start DayZ with
   `ProcessStartInfo.UseShellExecute = $false` and remove every variable not on a
   short whitelist (`SystemRoot`, `Path`, `TEMP`, `USERPROFILE`, `APPDATA`,
   `LOCALAPPDATA`, `ProgramFiles*`, processor/OS variables, `ComSpec`), and delete
   dumps you do not need.
5. **Own the PID.** Out-of-lifecycle probes start and stop only the process they
   launched, and hold the MCP lease while they run so no managed run lands on top.
6. **Use a fresh `-profiles=` directory per run.** A readiness check that greps the
   newest `script_*.log` found a log left by an earlier run in the same directory.
   It reported the server ready and the client connected at +0 s (2026-09-26): the
   run looked healthy and measured nothing.

## Measuring server load

What worked on 2026-09-26 (dummy-bot load, same evidence folder):

- **Cap the server.** Uncapped, the Diag server ran at ~1958 FPS and FPS was no load
  signal (10 moving dummies: 1900; 25 idle: 1908). `-limitFPS=60` (the 1.30.164.078
  exes carry a `limitFPS` string) turns frame drops into the saturation point and makes
  process CPU per window comparable across load steps.
- **Meter frames inside the server.** Count `timeslice` in an `override void OnUpdate(float timeslice)`
  of the server mission (`P:\scripts\5_mission\mission\missionserver.c:108`) and log per
  window: frames, seconds, FPS, max frame time. Sample the process
  `TotalProcessorTime` every second from outside and join by timestamp.
- **Bracket the run with baselines, and do not trust a fixed warm-up.** The first three
  windows of run 2 (a fresh storage) ran at 36-84 % machine CPU. In run 3 a 120 s
  warm-up was not enough either: its first baseline used 0.276 server cores, against
  0.001 at the baseline taken after deleting all load. Record a baseline at both ends,
  and treat a large mismatch as an inconclusive comparison.
- **Check post-spawn idle windows.** After 15 s settles in run 2, the idle window right
  after spawning 25 dummies used 0.201 server cores against 0.010 in the moving window
  after it, with the machine nearly idle (3.8 % CPU). At 50 dummies the gap was 0.179
  against 0.032 cores, but machine CPU was 49.2 % during that idle window. The cause
  was not isolated. Run 3 used 45 s settles and its idle windows after spawning 50 and
  100 averaged 60.0 FPS; that does not make 45 s a validated settle time for CPU.
- **Keep the CPU sampler cheap, and check it for gaps.** In run 3 the 1 s sampler went
  58.7 s without a sample across a spawn. Its loop also ran a WMI machine-CPU query and
  a script-log scan every 5 s; the cause was not isolated. The gap crossed two windows,
  which kept only 32 and 43 of their 45 s sampled. An analyzer that takes the nearest
  samples around a window then mixes stages into its CPU delta. Use the samples inside
  the window, report how much of it they cover, and drop the window when coverage is
  short. The in-server FPS meter was unaffected.
- **Write CSV numbers with the invariant culture (LL-524).** On an es-ES host,
  `'{0:F3}' -f` writes decimal commas and silently splits every CSV row into
  extra columns. Use `[string]::Format([Globalization.CultureInfo]::InvariantCulture, …)`.

## `PerformActionStart` in SP — pending guard + `UA_AM_INIT`

On 1.30 the method is at `actionmanagerclient.c:804` and singleplayer no longer
overwrites an in-flight pending/current action. Before 1.30: `:762`, without the
guard.

```
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actionmanagerclient.c:804-827
	void PerformActionStart(ActionBase action, ActionTarget target, ItemBase item, Param extra_data = NULL)
	{
		if (!g_Game.IsMultiplayer())
		{
			if (!m_PendingActionData && !m_CurrentActionData)
			{
				m_PendingActionData = new ActionData;
	
				if (!action.SetupAction(m_Player, target, item, m_PendingActionData, extra_data))
					m_PendingActionData = null;
			}
		}
		else
		{
			ActionStart(action, target, item, extra_data);
		}
	}
	
	override int GetActionState()
	{
		if (m_PendingActionData)
			return UA_AM_INIT;
		
		return super.GetActionState();
	}
```

1.29 contrast: `1.29: scripts\4_world\classes\useractionscomponent\actionmanagerclient.c:762` is the old
`PerformActionStart` line.

`UA_AM_INIT = 14` was inserted; later `UA_AM_*` values shifted (1.29:
`UA_AM_PENDING = 14` at `1.29: scripts\3_game\constants.c:479`):

```
// [EXACT] P:\scripts\3_game\constants.c:506-511
	const int 		UA_INITIALIZE = 12;
	const int		UA_CHECK_CON = 13;
	const int		UA_AM_INIT = 14;
	const int		UA_AM_PENDING = 15;
	const int		UA_AM_ACCEPTED = 16;
	const int		UA_AM_REJECTED = 17;
```

[DESIGN] Autotest that injects a second `PerformActionStart` in SP without
waiting for idle will silently no-op. Wait until `GetActionState()` is not
`UA_AM_INIT` / not pending. Never hardcode the integer `14` — use the
constant (`14` was `UA_AM_PENDING` on 1.29 and is `UA_AM_INIT` on 1.30).

MP path is unchanged: `PerformActionStart` still calls `ActionStart` directly
(`:816-819`).

## Navmesh / Tools (session awareness)

Custom terrains: regenerate `.nm` (no backwards compatibility) [CHANGELOG 1.30].
The format moved again in 1.30.164200: vanilla navmeshes are v9 (1.29: v6,
1.30.164078: v7; header `TESM`, uint32 version at offset 4), and the server's
version check is strict (`cmp r9d,9`, exe analysis by the config-delta pass,
not run in game), so a custom map's v7 navmesh most likely fails with
`Navmesh could not be loaded: Invalid version. (Navmesh version: 7, Desired version: 9)`.
Regeneration recipe: `dayz-ai-patterns` §Navmesh.
Workbench 1.30 is installed with DayZ Tools (`Bin\Workbench\WorkbenchApp.exe`,
FileVersion 1.30.164.200 since 2026-10-21; 1.30.164.078 before); its new editors are
[CHANGELOG 1.30] and unused here. [reported: BI 1.30.164200 notes] Known issue:
Workbench requires the command-line argument `-filePatching`; it gained a Diag menu.

## DayZ 1.30.164200 changes (summary)

- `-resolveFilePatchingUsingEnfusion` → `-loadRVFilesViaEnfusionFS` (§New launch flags).
- `FindFile` fixed, `#include` fixed, circular-inheritance compile error fixed
  [reported: BI 1.30.164200 notes]; not re-measured.
- Navmesh v9 (above); `ENF_DIAG_DENavmesh` diag entry
  (`P:\scripts\3_game\enums\ediagmenugame.c:7`); `NavmeshTest*` → `Land_NavmeshTest*`.
- `Debug.LOG_DEBUG_AUTOTEST` = `"AutotestLog"` (`P:\scripts\3_game\tools\debug.c:19`).
- `UA_AM_*` constants, `PerformActionStart` and `IsHeadless` are unchanged.
