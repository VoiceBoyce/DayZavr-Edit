# DayZ 1.30 — implementation-workflow deltas

> Target: DayZ 1.30 stable (1.30.164078). Version labels: ../../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

Companion to `SKILL.md`. The six-layer debug hierarchy, offline gates, and SyncVar rules still hold. This file is
the 1.30 contract delta for actions, CE rootclasses, `cfggameplay.json`, the underground trigger editor, NV type
switches, headless guards and version defines.

Every `[EXACT]` block was re-opened in `P:\` (1.30.164078, 2026-10-15); this file was first written against 1.30
Experimental 164014 and the stable line numbers below replace the Exp ones. `1.29: O:\…` cites the old tree.

## Action pipeline: `CanBeStarted` + `CCTLiquid`

1.30: the widget can still **show** an action whose `CanBeStarted()` is false; the client manager does not call
`ActionStart` for it. Before 1.30: `Can()` → CCT → CCI → `ActionCondition` was the full client start gate.

```
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actionbase.c:276-291
	//! Should we display additional info panel on ActionTargetsCursor
	bool IsTargetInfoAction()
	{
		return m_InfoData != null;
	}
	
	ActionInfoDataBase GetClientActionInfo()
	{
		return m_InfoData;
	}
	
	//! If for some reason we want a non-performable action (like pure info ones)
	bool CanBeStarted()
	{
		return true;
	}
```

Client start guard:

```
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actionmanagerclient.c:333-340
					if (ain.JustActivate())
					{
						ActionBase action = ain.GetAction();
						if (action && action.CanBeStarted())
						{
							ActionStart(action, ain.GetUsedActionTarget(), ain.GetUsedMainItem());
							break;
						}
```

Info-only override (widget shows, F does nothing):

```
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actionpartinfo.c:18-21
	override bool CanBeStarted()
	{
		return false;
	}
```

Vanilla water/wash/fill no longer uses `CCTWaterSurfaceEx` at the wash-hands pond action. `ActionWashHandsWater`
constructs `CCTLiquid` (1.29 used `CCTWaterSurfaceEx` at the same line):

```
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actionwashhandswater.c:25-29
	override void CreateConditionComponents()
	{		
		m_ConditionItem		= new CCINone();
		m_ConditionTarget 	= new CCTLiquid(LIQUID_GROUP_WATER - LIQUID_SNOW - LIQUID_HOTWATER, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
	}
```

[DESIGN] Copy `CCTLiquid` for custom drink/wash/fill on sea/pond; keep `CCTWaterSurfaceEx` only if you intentionally
diverge from vanilla. A listed action that never starts is often `CanBeStarted()==false`, not a broken CCT.

`ActionFillBottleBase` is `[Obsolete]` on 1.30 (replaced by `ActionObtainLiquidBase`, `actionfillbottlebase.c:28-29`,
`actionconstructor.c:156`) — domain detail in `enforce-script-reference` / `dayz-basebuilding`. Workflow implication:
do not `modded` an obsolete vanilla action that `ActionConstructor` no longer registers.

## CE rootclasses: `MotorbikeScript` and `HouseDestructible`

1.30 `cfgeconomycore.xml` on Chernarus / Livonia / Sakhal adds two rootclasses:

```
// [EXACT] P:\DZ\worlds\chernarusplus\ce\cfgeconomycore.xml:15-18
		<rootclass name="CarScript" act="car" reportMemoryLOD="no" /> <!-- cars (sedan, hatchback, transitBus, V3S, ...) -->
		<rootclass name="BoatScript" act="car" reportMemoryLOD="no" /> <!-- boats -->
		<rootclass name="MotorbikeScript" act="car" reportMemoryLOD="no" /> <!-- Motorbike -->
		<rootclass name="HouseDestructible" reportMemoryLOD="no" /> <!-- houses, wrecks -->
```

Same pair, same lines: `P:\DZ\worlds\enoch\ce\cfgeconomycore.xml:17-18`, `P:\DZ\worlds\sakhal\ce\cfgeconomycore.xml:17-18`.

[DESIGN] A custom map CE that omits `MotorbikeScript` will not economy-spawn motorbikes; omitting `HouseDestructible`
drops destructible-house CE coverage. `act="car"` on MotorbikeScript matches boats/cars (moveable vehicles).

## `cfggameplay.json` new members

1.30 `CfgGameplayJson` gained `ExternalLockData` and `SandstormData`; `ITEM_GeneralData` gained `disableBaseDecay`;
`ITEM_VehicleData` gained wheel detach/damage flags.

```
// [EXACT] P:\scripts\3_game\cfggameplaydatajson.c:14-23
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	ref ITEM_GeneralData GeneralData			= new ITEM_GeneralData;
	ref ITEM_PlayerData PlayerData 				= new ITEM_PlayerData;
	ref ITEM_WorldData WorldsData 				= new ITEM_WorldData;
	ref ITEM_BaseBuildingData BaseBuildingData 	= new ITEM_BaseBuildingData;
	ref ITEM_UIData UIData 						= new ITEM_UIData;
	ref ITEM_MapData MapData 					= new ITEM_MapData;
	ref ITEM_VehicleData VehicleData 			= new ITEM_VehicleData;
	ref ITEM_ExternalLockData ExternalLockData 	= new ITEM_ExternalLockData;
	ref ITEM_SandstormData SandstormData        = new ITEM_SandstormData;
```

```
// [EXACT] P:\scripts\3_game\cfggameplaydatajson.c:59-63
	bool disableBaseDamage;
	bool disableBaseDecay;
	bool disableContainerDamage;
	bool disableRespawnDialog;
	bool disableRespawnInUnconsciousness;
```

```
// [EXACT] P:\scripts\3_game\cfggameplaydatajson.c:414-418
	float boatDecayMultiplier = 1;
	bool canDetachAttachedCarWheels = true;
	bool canDamageAttachedCarWheels = true;
	bool canDetachAttachedBikeWheels = true;
	bool canDamageAttachedBikeWheels = true;
```

`ITEM_ExternalLockData` (`:421`, members `:434-437`): `protectionCountStageOne = 3`, `protectionCountStageTwo = 1`,
`protectionTime = 30.0`, `protectionResetTime = 3600.0`. `ITEM_SandstormData` (`:440`): `sandstormFrequency = 1.0` (`:458`).

Note: `InitServer` has `disableBaseDecay` **commented out** (`cfggameplaydatajson.c:48`) — the JSON member exists; the
server.cfg int bridge is not wired in this build.

## Underground trigger editor (`PluginUndergroundTriggerManager`)

1.30 diag-only in-game editor: the whole class is wrapped in `#ifdef DIAG_DEVELOPER`
(`P:\scripts\4_world\plugins\pluginbase\pluginundergroundtriggermanager.c:1-3`). Keybinds:

```
// [EXACT] P:\scripts\4_world\plugins\pluginbase\pluginkeybinding.c:55-56
		RegisterKeyBind(	 MENU_NONE|MENU_UNDERGROUND_TRIGGER_EDITOR	,KeyCode.KC_LCONTROL	,KeyCode.KC_SLASH		,"PluginUndergroundTriggerManager"		,"EditorToggle"			,"[LCTRL]+[/]"			,"Show/Hide Underground Trigger Editor");
		RegisterKeyBind(	 MENU_NONE|MENU_UNDERGROUND_TRIGGER_EDITOR	,KeyCode.KC_F	,-1		,"PluginUndergroundTriggerManager"		,"FocusOnObject"		,"[F]"							,"Focus on selected object");
```

Mouse binds: `pluginkeybinding.c:109-112`. [DESIGN] Level-design of underground volumes: use this editor on Diag rather
than hand-editing trigger configs blindly. Not available in retail.

The trigger JSON schema, the 4096-trigger limit, triggers tied to Object Spawner objects, the underground presence sync
and terrain holes are in `dayz-underground`.

## NVTypes environmental variants (custom cameras)

1.30 adds underground and sandstorm variants; a `switch` over the enum that is not exhaustive will miss them. Before
1.30: `NVTypes` ended at the optics/pumpkin night variants.

```
// [EXACT] P:\scripts\4_world\entities\manbase\dayzplayer\dayzplayercamera_base.c:54-78
enum NVTypes
{
	NONE = 0,
	NV_GOGGLES,
	NV_GOGGLES_2D,
	NV_GOGGLES_OFF,
	NV_OPTICS_ON,
	NV_OPTICS_OFF,
	NV_PUMPKIN,
	NV_OPTICS_KAZUAR_DAY,
	NV_OPTICS_KAZUAR_NIGHT,
	NV_OPTICS_STARLIGHT_DAY,
	NV_OPTICS_STARLIGHT_NIGHT,
	
	// ---- Optics condition variants ----
	NV_OPTICS_UNDERGROUND,
	NV_OPTICS_SANDSTORM_DAY,
	NV_OPTICS_SANDSTORM_NIGHT,
	NV_OPTICS_UNDERGROUND_SANDSTORM,
	
	// ---- Glasses/goggles condition variants ----
	NV_GOGGLES_DAY,
	NV_GOGGLES_UNDERGROUND,
	NV_GOGGLES_SANDSTORM,
	NV_GOGGLES_UNDERGROUND_SANDSTORM,
```

[DESIGN] Custom NV cameras: handle the new values or call vanilla `ResolveNVType` (used at
`4_world\entities\manbase\dayzplayer\dayzplayercameraironsights.c:272`) instead of applying the raw type directly.
Domain cameras: `enforce-script-reference`.

## Headless vs dedicated guards

1.30 `CGame` exposes `IsHeadless()` and `IsHeadlessOrDedicatedServer()` — **not** in 1.29 `game.c`.

```
// [EXACT] P:\scripts\3_game\global\game.c:1125-1136
	proto native bool		IsDedicatedServer();
	
	/**
	 \brief Check whether the application is headless or not (applies to both server and headless roboclients or anything else that is headless)
	*/
	proto native bool		IsHeadless();
	
	/**
	 \brief Check whether the application is headless or a dedicated server (it combines IsDedicatedServer and IsHeadless)
		\note This includes servers with a head as well (-headlessMode=0)
	*/
	proto native bool		IsHeadlessOrDedicatedServer();
```

1.30 vanilla skips client FX with `!g_Game.IsHeadlessOrDedicatedServer()` so roboclients are treated like servers.
Before 1.30: server-only particle/sound skips used `IsDedicatedServer()` / `IsServer()`. Workflow: E18 still forbids
`IsServer()`/`IsClient()` as the client/server **guard**; for "skip VFX" prefer the combined helper so headless
roboclients do not run particles.

Measured 2026-09-26 on 1.30 Exp 164014: vanilla calls `IsHeadlessOrDedicatedServer()` 97 times and `IsHeadless()` alone
never (stable `P:\scripts`: still 97 call lines plus the declaration), and the script log lists `DAYZ_1_30` among the defines (1.29 logs `DAYZ_1_29`). By their documented semantics (the
excerpt above) both guards agree on a rendered client and on a dedicated server and differ only on a headless client;
that is inferred, not measured. `ROBOCLIENT` and `INPUT_OVERRIDE` were defined in the public 1.30 Exp DayZDiag, client and
server (measured on 1.30 Exp 164014; not re-checked on the stable DayZDiag), so code under `#ifdef ROBOCLIENT` compiled
for everyone on that exe and every `PlayerBase` carried an idle `m_Bot`. Details:
`dayz-mcp-verify/references/dayz-1-30-mcp-verify.md`.

## Version defines (`DAYZ_1_29`, `DAYZ_1_30`)

The engine fills in one define for the running version: `\brief Define filled in with the current Major and Minor
version (e.g. DAYZ_1_16)` / `#define DAYZ_X_XX` (`P:\scripts\1_core\defines.c:12-14`, documentation-only file under
`#ifdef DOXYGEN`; same text on 1.29). Evidence that it is the *current* version only: 1.29 vanilla itself gated a
1.29-exclusive menu track with `#ifdef DAYZ_1_29 // Road to Badlands exclusive`
(1.29: `O:\…\scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayerregistry.c:116-118`), and 1.30 removed that
block (`P:\scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayerregistry.c:116` registers the standard tracks;
no `DAYZ_1_` token is left anywhere in `P:\scripts`). A 1.30 server log lists `DAYZ_1_30` and not `DAYZ_1_29`
(measured on 1.30 Exp 164014; [reported: bzed] on a 1.30.164014 Linux server).

Rules (`../../_shared/dayz-version-policy.md` rules 1-2):

1. Write plain 1.30 code; a mod built only for 1.30 needs no version `#ifdef`.
2. **Never** gate new code on `#ifdef DAYZ_1_30`: that block silently disappears on 1.31.
3. If one PBO must still load on 1.29, put the **old** code under `#ifdef DAYZ_1_29` and the 1.30 code in `#else`, so the
   1.30 code stays the default on 1.31+.
4. `DEVELOPER`, `DIAG_DEVELOPER`, `BUILD_EXPERIMENTAL` are build flavours (DayZDiag / Experimental), never a version test.
5. Runtime version string for logs only: `string v; g_Game.GetVersion(v);` [reported: bzed]; it cannot select code that
   would not compile.

```c
// [DESIGN] only for a PBO that must still compile on 1.29 (no IsHeadlessOrDedicatedServer there)
static bool MyMod_IsHeadlessOrDedicated()
{
#ifdef DAYZ_1_29
	return g_Game.IsDedicatedServer();
#else
	return g_Game.IsHeadlessOrDedicatedServer();
#endif
}
```

## Migration checklist (implementation)

1. Action mods: implement `CanBeStarted`; do not treat a visible widget as startable. Water actions: `CCTLiquid`.
2. Custom CE: add `MotorbikeScript` / `HouseDestructible` rootclasses.
3. Mission `cfggameplay.json`: merge new keys; do not assume `disableBaseDecay` is wired from server.cfg (commented `InitServer`).
4. Exhaustive `NVTypes` switches: add underground/sandstorm cases.
5. FX that must not run on roboclients: `IsHeadlessOrDedicatedServer()`.
6. No `#ifdef DAYZ_1_30`; legacy-only code under `#ifdef DAYZ_1_29` + `#else`.
7. Boot on a 1.30 server and read `script_*.log` for `(E)` and `SCRIPT (W): '…' is obsolete` [reported: bzed] — obsolete
   APIs compile but some are never called by vanilla (`OnCEUpdate`); see `enforce-script-reference`
   `references/dayz-1-30-enforce-script.md`.
