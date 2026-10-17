# DayZ 1.30 — MCP / script-console tooling (build 1.30.164078)

Companion to `SKILL.md` § DayZ 1.30 changes. The managed `dayz_test_run` /
`dayz_test_stop` lifecycle and Mode=all capture rule still hold. Script paths
below point at `P:\scripts` 1.30.164078 (re-checked 2026-10-15). Runtime
results from the pre-release build are labelled "(measured on 1.30 Exp 164014)".

## `ScriptConsoleTabRegistry` (inject a Diag console tab)

On 1.30 tabs register through `ScriptConsoleTabRegistry.Register`. Before 1.30:
adding a Script Console tab meant editing / replacing `ScriptConsole.c`.

```
// [EXACT] P:\scripts\5_mission\gui\scriptconsole.c:33-43
class ScriptConsoleTabRegistry
{
	protected static ref array<ref ScriptConsoleTabRegistryEntry> s_Entries;
	
	static void Register(int id, string displayName, string widgetName, typename tabClass)
	{
		if (!s_Entries)
			s_Entries = new array<ref ScriptConsoleTabRegistryEntry>();
			
		s_Entries.Insert(new ScriptConsoleTabRegistryEntry(id, displayName, widgetName, tabClass));
	}
```

Vanilla `InitializeRegistry` (`:50-66`) registers GENERAL through WEATHER;
`ScriptConsoleTabID` ends with `COUNT` (`:14`, "KEEP AS LAST ENTRY").
`ScriptConsole` ctor calls `InitializeRegistry()` then iterates `GetAll()`
(`:293-296`).

[DESIGN] A verification / debug tab for a mod: `modded` the registry
initializer or register from a mission module **after** vanilla init, with a
new `ScriptConsoleTabID` value **below** `COUNT`. Do not replace the whole
`ScriptConsole.c`. Tabs are Diag/developer UI, not MCP verbs — they do not
replace `bridge_status` / spawn / raycast.

[UNVERIFIED] Whether a mod can `Register` from `4_World` before
`InitializeRegistry` clears the array (`:52-53` Clear). Prefer registering
inside a `modded` `InitializeRegistry` after `super` (if super exists) or by
appending after vanilla Register calls via modded class. Confirm in-game
before treating the order as contract.

## Headless vs rendered MCP

New in 1.30: `IsHeadless()` / `IsHeadlessOrDedicatedServer()` exist on
`CGame` (`P:\scripts\3_game\global\game.c:1130`, `:1136`; absent on 1.29). This skill
already forbids `-Mode server` for capture (no window). Roboclients are
headless: they poll neither a client_peer suitable for `capture_screenshot`
nor a rendered HUD.

[DESIGN] Keep `-Mode all` + `@DayZ_MCP` for visual smokes. Do not treat a
headless roboclient as the MCP client_peer.

## Roboclients vs server-side dummy bots (measured on 1.30 Exp 164014, 2026-09-26)

Build 1.30.164014 Exp, public DayZDiag, local server outside the managed
lifecycle. Evidence: `VAULT/AI/10_Projects/DayZ_MCP/lanes/2026-09-26-roboclient-130/`
(`evidence_extracts.txt`, `results/`). Source paths below are under
`P:\scripts\` (1.30.164078; every line re-checked 2026-10-15).

[EXACT][CLAIM-DZ130-ROBOCLIENT-DEFINE-PUBLIC-DIAG] The public 1.30 Exp DayZDiag
compiles every script module with `ROBOCLIENT` and `INPUT_OVERRIDE` defined, as
client and as `-server` (script log: `Module: …; defines: "…,ROBOCLIENT,INPUT_OVERRIDE,…"`).
`-headlessMode=1` adds `NO_GUI,NO_GUI_INGAME`. The 1.29 executables carry no
`ROBOCLIENT` or `RoboclientUtils` strings. Stable 1.30.164078: the
`DayZ_x64.exe`, `DayZDiag_x64.exe` and `DayZServer_x64.exe` binaries contain
`roboclient` strings (byte grep, 2026-10-15), and `-headlessMode` only the Diag
exe; whether the stable Diag still defines `ROBOCLIENT` for every module is not
re-measured — read the `defines:` line before relying on `m_Bot`
(`playerbase.c:607` is `#ifdef ROBOCLIENT`).

What that switches on (source-read):
- Every `PlayerBase` builds a `Bot` in `Init()` (`4_World\Entities\ManBase\PlayerBase.c:607-613`),
  and so does a Script Console spawn through `OnSpawnedFromConsole` (`:5281-5296`,
  called from `4_World\Plugins\PluginBase\PluginDeveloper.c:350-362`).
- Without `-roboclient` the bot idles in a debug FSM whose states start on
  `EActions.PLAYER_BOT_*` (`4_World\Systems\Bot\Bot.c:344-383`, `:331-342`; enum
  `3_Game\Enums\EActions.c:92-107`): stance and movement randomizers, user-action
  spam, attach/drop cycle, item move back and forth, spawn/open/eat/destroy a can,
  hand swaps. The Diag debug-action menu lists them (`PlayerBase.c:8427-8456`) and
  `OnAction` forwards them to `m_Bot.StartAction` (`:8356-8363`).
- With `-roboclient` the FSM comes from config through
  `RoboclientUtils.ReadStateMachines` (`Bot.c:80-84`). Vanilla ships none that we
  found, so a rendered `-roboclient` connects and stands still.

[EXACT][CLAIM-DZ130-DUMMY-BOT-SERVER-MOVES] A server-side dummy moves with no client
connected. Recipe used (APIs: `3_Game\Global\Game.c:703`, `PlayerBase.c:5281`,
`Bot.c:205-215`):

```c
// [DESIGN] server mission script; m_Bot exists only under #ifdef ROBOCLIENT
PlayerBase d = PlayerBase.Cast(GetGame().CreateObjectEx("SurvivorM_Mirek", pos, ECE_PLACE_ON_SURFACE|ECE_INITAI|ECE_EQUIP_ATTACHMENTS));
d.OnSpawnedFromConsole();                                    // scheduler + Bot, as the console does
d.m_Bot.StartAction(EActions.PLAYER_BOT_RANDOMIZE_MOVEMENT); // start
d.m_Bot.StartAction(EActions.PLAYER_BOT_STOP_CURRENT);       // back to idle
```

Measured: ~33.7 m per 5 s (sprint, ~6.75 m/s) in a straight line north, ~1.7 m/s
once in water. Mean displacement: 169.2, 169.1, 168.1 and 158.9 m in 25 s for 10, 25,
50 and 100 dummies (run 2); 297.1 and 291.0 m in 45 s for 50 and 100 (run 3). The
"randomizer" is not random: it forces `OverrideMovementSpeed(ONE_FRAME, 3.0)`
and `OverrideMovementAngle(ONE_FRAME, 0.0)` every frame
(`4_World\Systems\Bot\Bot_MovementRandomizer.c:26-40`). Spawn inland; from the
coast the dummies run into the sea.

[EXACT][CLAIM-DZ130-HEADLESS-CLIENT-CRASH] The headless roboclient is unusable in
this build: `-headlessMode=1 -roboclient` crashes with
`Access violation. Illegal read … at 0x0` at the same instruction, standalone and
with `-connect` (3 of 3 on 2026-09-26), after compiling `Game` and before `World`.

Load: one server at `-limitFPS=60`, no client, Ryzen 7 7800X3D. Two runs on
2026-09-26, one sample per cell:

- Run 2 added dummies in steps of 10, 25, 50 and 100 with 15 s settles, and did not
  move them back between steps.
- Run 3 spawned 50 and then 100 on a 10-wide grid (6 m by 8 m), settled 45 s, and put
  every dummy back on the grid before each moving window.

| Moving dummies | Run 2 FPS | Run 2 server CPU (cores) | Run 2 machine CPU | Run 3 FPS | Run 3 server CPU (cores) | Run 3 machine CPU |
|---:|---:|---:|---:|---:|---:|---:|
| 0 (final baseline) | 60.0 | 0.007 | 0.6 % | 60.0 | 0.001 | 0.4 % |
| 25 | 60.0 | 0.010 | 0.4 % | not run | not run | not run |
| 50 | 60.0 | 0.032 | 2.0 % | 60.0 | 0.024 (32 of 45 s sampled) | 3.2 % |
| 100 | 54.7 | 0.384 | 34.2 % | 60.0 | 0.008 | 2.0 % |

FPS is the mean over each window (run 2: 25 s for moving windows and 30 s for the final
baseline; run 3: 45 s); machine CPU is the whole host. In run 3's setup the 100-dummy moving window averaged 60.0 FPS (33 ms
max frame) at 0.008 server cores. Run 2's averaged 54.7 FPS while the host ran at
34.2 % CPU, of which the server's 0.384 cores are about 2.4 %. The runs differ in
settles, placement, window length and host load, and none of them was isolated, so
neither run is a capacity figure. Other contamination seen that day:

- Post-spawn idle windows. In run 2 the idle window right after spawning 25 dummies
  used 0.201 server cores against 0.010 in the moving window after it, with the host
  nearly idle; the idle window after spawning 50 ran with the host at 49.2 %.
- Startup. Machine CPU was 36-84 % in the first three windows of run 2, and the run-3
  baseline after a 120 s warm-up still used 0.276 server cores against 0.001 at its
  final baseline.

Method and traps:
`dayz-test-ingame/references/dayz-1-30-test-ingame.md` § Measuring server load.

[DESIGN] For the MCP: server verbs `bot_dummy_spawn(n, pos)` plus `bot_start(action)`
and `bot_stop()` on this path would give N moving players on one machine for load
and soak tests. Since the 2026-10-15 update the installed DayZDiag the managed
launcher starts is 1.30.164078, so no separate 1.30 lane is needed (not checked:
whether the launcher's sealed registry accepts the updated game exe on its first
run); the `ROBOCLIENT` define on that exe is the open question (above). This
is not a way to offload simulation: dummies and roboclients alike run under server
authority.

## Action injection vs 1.30 SP pending

MCP `action_use` / `PerformActionStart` in singleplayer now no-ops if
pending/current is set (`P:\scripts\4_world\classes\useractionscomponent\actionmanagerclient.c:804-814`). Continuous actions
already do not complete via MCP (existing SKILL.md limitation). Instant
actions: if a second inject returns `setup_failed` / no-op, wait for
`GetActionState()` to leave `UA_AM_INIT`. Detail:
`dayz-test-ingame/references/dayz-1-30-test-ingame.md`.
