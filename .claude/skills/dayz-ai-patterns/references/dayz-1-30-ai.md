# DayZ 1.30 — vanilla AI noise, infected cower, animal groups

First written against 1.30 Experimental (164014); every vanilla citation below was **re-verified on
1.30.164078 stable** against `P:\` (`P:\scripts\…`, `P:\DZ\…`). `Before 1.30:` lines cite the 1.29
extraction `O:\DayZ 1.29 Старый распакованный клиент\` (shortened `O:\…`). Expansion eAI scripts are
**not** on this machine; this file is vanilla infected / animal / NoiseSystem plus what an eAI-style mod
must re-check. Labels: `../../_shared/dayz-version-policy.md`.

## Native environment noise reduction

1.30: the engine applies weather/sandstorm/sea reduction to AI hearing natively, driven by `class AIParams`
(below). Script can query the value the AI uses:

```c
// [EXACT] P:\scripts\3_game\noise.c:12
	//! Absolute noise-strength reduction the environment (weather/sandstorm/sea) applies at 'pos'. Single source of truth used by AI.
	proto native float GetEnvironmentNoiseReduction(vector pos);
```

Per its comment it is an **absolute strength reduction**, not the 0..1 multiplier the 1.29 helpers
returned — do not drop it into a slot that used to take `GetNoiseReduction()`. `[UNVERIFIED]` exact
units/curve (native code).

Only the debug HUD reads it in vanilla script: `P:\scripts\4_world\plugins\pluginbase\pluginpresencenotifier.c:155-159`
(`noiseSys.GetEnvironmentNoiseReduction(m_pPlayer.GetPosition())`).

No vanilla script folds weather into a noise multiplier any more (grep of `P:\scripts` for
`GetNoiseReduction(`: only its own declaration). Player steps:

```c
// [EXACT] P:\scripts\4_world\entities\dayzplayerimplement.c:3472
			noiseMultiplier = NoiseAIEvaluate.GetNoiseMultiplier(this);
									
			AddNoise(noiseParams, noiseMultiplier);
```

Infected voice events:

```c
// [EXACT] P:\scripts\4_world\entities\creatures\infected\zombiebase.c:597
		if (g_Game.IsServer())
		{
			if (sound_event.m_NoiseParams != NULL)
				GetGame().GetNoiseSystem().AddNoise(this, sound_event.m_NoiseParams);
		}
```

Before 1.30, script multiplied `NoiseAIEvaluate.GetNoiseReduction(g_Game.GetWeather())` into almost every
emitter: player steps (`O:\…\scripts\4_world\entities\dayzplayerimplement.c:3344`), player sound events
(`:2497-2522`, `:2620`, `:3474`, `:3578`), door/fence open-close actions
(`…\actions\interact\actionopendoors.c:81` etc.), infected voices (`…\zombiebase.c:594`) and
`MiscGameplayFunctions.GenerateAINoiseAtPosition` (`…\static\miscgameplayfunctions.c:1760`).

`MiscGameplayFunctions.GenerateAINoiseAtPosition(vector, float, NoiseParams)` is
`[Obsolete("Use MiscGameplayFunctions.GenerateAINoiseAtPositionEx instead!")]`
(`P:\scripts\4_world\static\miscgameplayfunctions.c:2010-2021`); the replacement
`GenerateAINoiseAtPositionEx(notnull Object source, float lifeTime, NoiseParams)` (`:1824-1834`) calls
`AddNoiseTarget(source.GetPosition(), lifeTime, noiseParams)` with no weather factor. `CarScript.GenerateCarHornAINoise`
is gone; the horn lives in `VehicleHornComponent`
(`P:\scripts\4_world\entities\vehicles\components\vehiclehorncomponent.c:10`, reached through
`Transport.GetVehicleHornComponent()`, `P:\scripts\3_game\vehicles\transport.c:851`).

## Obsolete script weather helpers (kept for HUD / old mods)

`[CHANGELOG 1.30]` says `'Weather.GetNoiseReductionByWeather'` was removed. On 1.30.164078 the method is
still in script, marked obsolete (calls compile with a warning):

```c
// [EXACT] P:\scripts\3_game\weather.c:454
	[Obsolete("Use Weather.GetNoiseReductionByWeatherEx instead!")]
	float GetNoiseReductionByWeather()
	{
		float rainReduction = GetRain().GetActual() * GameConstants.RAIN_NOISE_REDUCTION_WEIGHT;
		float snowfallReduction = GetSnowfall().GetActual() * GameConstants.SNOWFALL_NOISE_REDUCTION_WEIGHT;
		
		if (rainReduction == 0 && snowfallReduction == 0)
			return 1;
		
		if (rainReduction > snowfallReduction)	// combined phenomenons dont need to have multiplicative effects
			return 1 - rainReduction;
		else
			return 1 - snowfallReduction;
	}
```

Replacement that also sees sandstorm (new in 1.30 and **already** `[Obsolete]`, not the AI source of truth):

```c
// [EXACT] P:\scripts\3_game\weather.c:407
	[Obsolete("1.30: AI noise dampening is now done natively via AIParams config parameters. Kept for HUD and old mods.")]
	float GetNoiseReductionByWeatherEx(notnull Object object)
	{
		float rainReduction = GetRainNoiseReduction();
		float snowfallReduction = GetSnowfallNoiseReduction();
		float sandstormReduction = GetSandstormNoiseReduction(object);
```

```c
// [EXACT] P:\scripts\4_world\static\sensesaievaluate.c:92
	[Obsolete("Use GetNoiseReductionEx instead!")]
	static float GetNoiseReduction(Weather weather)
	{
		if (weather)
			return weather.GetNoiseReductionByWeather();
		 
		return 0;
	}
```

`NoiseAIEvaluate.GetNoiseReductionEx(notnull Object)` (`sensesaievaluate.c:18-26`, also `[Obsolete]`)
forwards to `GetNoiseReductionByWeatherEx`. (The task brief's "`SensesAIEvaluate.GetNoiseReduction*`" is
this class: the file is `sensesaievaluate.c`, the class is `NoiseAIEvaluate`.)

## `AIParams` climate multipliers

1.30 (`P:\DZ\data\aiconfigs\config.cpp:12-22`):

```cpp
// [EXACT] P:\DZ\data\aiconfigs\config.cpp:12
class AIParams
{
	maxNoiseRange=300;
	rainToNoiseMultiplier=10;
	snowfallToNoiseMultiplier=5;
	seaToNoiseMultiplier=15;
	fogToNoiseMultiplier=10;
	windToNoiseMultiplier=10;
	sandstormToNoiseMultiplier=10;
	noiseDampeningMultiplier=0.69999999;
	noiseCollisionDampeningMultiplier=0.2;
```

(Before 1.30: `rainToNoiseMultiplier` and `seaToNoiseMultiplier` only —
`O:\…\DZ\data\aiconfigs\config.cpp:12-18`.)

`[DESIGN]` To make infected hear less in a sandstorm, raise `sandstormToNoiseMultiplier` (or the other
`*ToNoiseMultiplier` keys) in a config patch of `AIParams`. Do not override `GetNoiseReductionByWeather`
expecting native AI to follow. `[UNVERIFIED]` the in-game magnitude of each key (native code, not measured
here).

## Infected `MINDSTATE_COWER`

```c
// [EXACT] P:\scripts\3_game\entities\dayzinfected.c:12
	//! mind states
	MINDSTATE_CALM,
	MINDSTATE_DISTURBED,
	MINDSTATE_ALERTED,
	MINDSTATE_CHASE,
	MINDSTATE_FIGHT,
	MINDSTATE_COWER,
```

```c
// [EXACT] P:\scripts\4_world\entities\creatures\infected\zombiebase.c:54
		RegisterNetSyncVariableInt("m_MindState", -1, 5);
```

(Before 1.30: range `-1, 4` — 1.29: `O:\…\zombiebase.c:54`.)

```c
// [EXACT] P:\scripts\4_world\entities\creatures\infected\zombiebase.c:474
			//! sandstorm cowering, movement is already stopped by the AI, hold the calm idle pose
			case DayZInfectedConstants.MINDSTATE_COWER:
				if ( moveCommand && !moveCommand.IsTurning() )
					moveCommand.SetIdleState(0);
				break;
```

`[UNVERIFIED]` Native C++ that **enters** `MINDSTATE_COWER` from sandstorm intensity is not in Enforce; only
the script reaction is visible. The `CfgAIBehaviours Infected` template is unchanged from 1.29
(`P:\DZ\AI\config.cpp:6214`, diff against 1.29 empty), so the switch is native.

Animals cower too: 1.30 adds `class SlotCowering { class BehaviourCowering { poseAction=…; }; }` to the
animal behaviour trees in `CfgAIBehaviours` — 14 in `P:\DZ\AI\config.cpp` (every `AmbientLife*`,
`Herbivores*` and `Predators_Wolf`; e.g. `AmbientLife` at `:30-36`) plus `Predator_UrsusArctos` in
`P:\DZ\AI_bliss\config.cpp:38`. Goats (`Herbivores_CapraHircus`)
also switched several `pathFilter` values from `"NoJumping"` to `"GoatOnRun"` (config-delta).

## Animal group templates

On 1.30.164078 `GroupBehaviourTemplates` in `P:\DZ\data\aiconfigs\config.cpp:77` lists the same five
templates as 1.29: `DZAmbientLifeGroupBeh` (`:79`), `DZWolfGroupBeh` (`:87`), `DZDeerGroupBeh` (`:151`),
`DZdomesticGroupBeh` (`:201`), `DZSheepGroupBeh` (`:252`); Bliss adds `BlissBearGroupBeh`
(`P:\DZ\data_bliss\aiconfigs\config.cpp:9`).

Badlands (Nasdara) adds two templates; first quoted from the Exp 164014 data, now on `P:\` since the
1.30.164200 unpack (`P:\DZ\data_takistan\aiconfigs\config.cpp`): `DZMouflonGroupBeh` (`:9`,
`type = "DomesticHerbivores"`, graze/rest lifecycle, `safeKeeperIntervalMin/Max`, catch-up radii) and
`DZDogGroupBeh` (`:61`, `type = "Predators"`, `zoneType = "HuntingGround"`,
`siegeAttackCountdownMin/Max = 10/15`, `huntAttackCountdownMin/Max = 1/4`, `safeKeeperIntervalMin/Max = 20/40`,
`:85-90`). The new animal scripts `Animal_OvisGmelini`, `Animal_CanisFamiliaris_Dobermann_*` /
`_GermanShepherd_*`, `Animal_VaranusGriseus` (`P:\scripts\4_world\entities\creatures\animals\`) have their
configs in `P:\DZ\animals_takistan\<animal>\config.cpp` (e.g. `Animal_OvisGmelini: AnimalBase`,
`ovis_gmelini\config.cpp:20`) and their `CfgAIBehaviours` in `P:\DZ\AI_takistan\config.cpp`
(`Predators_Dobermann` `:3`, `Predators_GermanShepherd` `:676`, `Herbivores_OvisGmelini` `:1338`,
`AmbientLife_Varanus_Griseus` `:2010`; `Predators_Dobermann` names `defaultGroupTemplateName="DZDogGroupBeh"`,
`:7`). (Corrected 2026-10-21: this paragraph used to say none of it was in `P:\DZ`; `P:\` held no DLC data
before the 164200 unpack.)

## Navmesh (eAI pathfinding on custom maps)

`[CHANGELOG 1.30]`: the navmesh file is a newer version with no backwards compatibility — regenerate for
custom terrains. Navmesh can use a hide animation source if configured in the EntityType Navmesh config.

Measured since (header `TESM` + uint32 version at offset 4): 1.29 vanilla = **6**
(`O:\…\DZ\worlds\*\navmesh\navmesh.nm`), 1.30.164078 vanilla = **7**, 1.30.164200 vanilla = **9**
(`P:\DZ\worlds\{chernarusplus,enoch,nasdara,sakhal}\navmesh\navmesh.nm`, and the installed
`worlds_*_nav.pbo`, PBO header `version` 127639-127643). 1.30.164078 rejected v6 with
`AI (E): Navmesh could not be loaded: Invalid version. (Navmesh version: 6, Desired version: 7)` and
`AI (F): Unable to load navmesh for '<world>'` (measured on this machine; infected do not spawn, animals and
native AI freeze). For 164200 the exe compares with `cmp r9d,9` (164078: `cmp r9d,7`; exe analysis in
`dayz-130-migration` config-delta §2.9, not run in game), so a v7 navmesh most likely fails the same way.
Regenerate with the 164200 data server (`-startNavmeshDataServer`) and `NavMeshGenerator_x64.exe`
1.30.164.165: `SKILL.md` § DayZ 1.30.164200 changes.

Config side (1.30.164200 vanilla): `class Navmesh { HideParts { class x { hideAnimationSource="…"; } } }` with
`generateWithRoadway` / `generateWithProxies` on `Land_IrrigationTunnel_Entrance_01`
(`P:\DZ\structures_takistan\Industrial\config.cpp:25-35`) and
`class Navmesh { TransformParts { class Gate { transformAnimationSource="Wall_Gate_Rotate"; } } }` on `Fence`
(`P:\DZ\gear\camping\config.cpp:4418-4427`).

## What an eAI-style (script AI on `PlayerBase`) mod must re-check on 1.30

Expansion eAI bots are `PlayerBase` subclasses driven by script FSMs, so they inherit every player-side
1.30 change. Static list (api-delta + `P:\`); none of it was tested against an Expansion build:

1. **Hearing.** If the mod scales what bots hear with `NoiseAIEvaluate.GetNoiseReduction(weather)` /
   `Weather.GetNoiseReductionByWeather()`, it still compiles (obsolete warning) but now disagrees with
   native infected/animals, which use `AIParams` (incl. fog/wind/sandstorm). Use
   `GetNoiseSystem().GetEnvironmentNoiseReduction(pos)` (absolute reduction) for parity, or
   `GetNoiseReductionByWeatherEx(obj)` for the old multiplier shape with sandstorm.
2. **Noise the bots emit.** Bots step through vanilla `DayZPlayerImplement` code, which no longer passes a
   weather multiplier (`dayzplayerimplement.c:3472-3474`). A mod override that still multiplies weather in
   double-dampens (engine attenuates too). Replace `GenerateAINoiseAtPosition` with
   `GenerateAINoiseAtPositionEx`; car-horn noise moved to `VehicleHornComponent`.
3. **Surrender.** Player surrender is no longer an item in hands: use `Man.IsSurrendered()`
   (`P:\scripts\3_game\entities\man.c:67`) / `PlayerBase.SetSurrenderState(bool)` (`playerbase.c:2116`).
   `EmoteManager.ForceSurrenderState` and `SurrenderDummyItem` are `[Obsolete]`; any threat logic that
   detected surrender by `SurrenderDummyItem` in hands silently stops working.
4. **Infected as targets.** `MINDSTATE_COWER` (value after `MINDSTATE_FIGHT`) — threat/aggro code that
   switches on infected mind state needs a case for it.
5. **Inventory/hands.** `Man.*TakeEntity*` moved to `EntityAI` and `HumanInventory.TakeEntity*Ex` to
   `GameInventory` (calls compile; overrides on `Man`/`HumanInventory` must move);
   `HumanItemAccessor.OnItemInHandsChanged(bool pInstant = false, bool pChangeAnimationInstance = true)`
   (`P:\scripts\3_game\humanitems.c:112`).
6. **Weapons.** `EEFired` is now declared on `EntityAI`; `FirearmActionAttachMagazine` requires
   `TestAttachMagazine(...)`; `GetExplosionParticleID` is gone — see `dayz-weapons` § DayZ 1.30 changes.
7. **Ticks/persistence.** `EntityAI/ItemBase.ProcessVariables()` now takes a `float` (overrides break,
   `P:\scripts\3_game\entities\entityai.c:3913`); `OnCEUpdate` is obsolete in favour of
   `OnCEIterate(float, float)`; `GAME_STORAGE_VERSION` is 144 — version-gate any `OnStoreLoad` reads.
8. **Navmesh.** Regenerate navmesh for custom terrains (`[CHANGELOG 1.30]`), which matters to
   `ExpansionPathHandler` path filters (PC-15/PC-16). On 1.30.164200 the required format is v9; a v7
   navmesh built for 164078 most likely no longer loads (§Navmesh above).
