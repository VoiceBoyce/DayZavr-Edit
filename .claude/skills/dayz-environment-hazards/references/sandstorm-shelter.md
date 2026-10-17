# Sandstorm: architecture and shelter

Source: DayZ 1.30 stable `1.30.164078`, citations re-opened on `P:\scripts` (paths below relative to it unless they start with `P:\`). The native controller lives in the exe and is not inspected. Sandstorm does not exist on 1.29.

## How to obtain and trigger the phenomenon

`SandstormController` has private constructors (`3_game\sandstorm.c:12-13`). Alias: `typedef SandstormController Sandstorm` (`:135`). The engine instantiates the scripted type:

```c
// [EXACT] P:\scripts\3_game\sandstorm.c:124
	private static event typename GetScriptedType()
	{
		//
		// The following class is present in module 4_World and
		// hence its typename cannot be used directly in 3_Game.
		//
		string className = "ScriptedSandstormController";
		return className.ToType();
	}
```

`Start(float duration, bool calledByWeather)` / `Stop(float duration, bool calledByWeather)` are server-only (`3_game\sandstorm.c:20, 27`). Server `OnStart`/`OnStop` callbacks forward to `WorldData.WeatherSandstormStart/Stop` (`sandstorm.c:90-116`), which call the overridable `OnSandstormStart(bool isStartedByWeather)` / `OnSandstormStop(bool calledFromWeather)` (`3_game\worlddata.c:495-528`). `WorldData.StartSandstorm` sets the `m_IsSandstormStartedByWeather` flag and calls `GetSandstorm().Start(timeToGather, true)` (`worlddata.c:477-490`). `NasdaraData.OnSandstormStart` randomises wind/overcast when the storm was **not** started by weather (`4_world\classes\worlds\nasdara.c:497-511`).

Note: `ScriptedSandstormController.OnStart` calls `super.OnStart(calledByWeather, continuedSession)` — the two booleans are swapped relative to the base signature (`4_world\systems\sandstorm\scriptedsandstormcontroller.c:310-312` vs `sandstorm.c:90`). If you override `OnStart`, pass your own arguments in the base order; the runtime effect of the vanilla swap was not measured.

Useful native API (all require an active storm except `IsActive`/`GetDirection`/`GetMagnitude`, per the comments): `GetDirection`, `IsPositionAtEnd`, `GetPosition`, `GetSpeed`, `GetRemainingMovementDuration`, `GetPointOnEdge` (negative distance = inside the storm), `GetIntensity(worldPosition)`, `GetMagnitude` (`sandstorm.c:32-83`). `GetIntensityForPlayer` is script, not proto (`:78`).

Script hooks: `ScriptedSandstormController.GetSandstormStartSI()` / `GetSandstormStopSI()` (client-side invokers, fired after the PPE start/stop; `scriptedsandstormcontroller.c:329-330, 372-384`). `Object.IsInfluencedBySandstorm()` (default `false`) and `OnClientSandstormEnter/Leave()` are declared on `Object` (`3_game\entities\object.c:1375-1381`), but no vanilla script overrides or calls them — presumably engine-driven; not measured.

[DESIGN] Manual trigger recipe: vanilla's DEVELOPER-only debug RPC first aligns the wind direction with `GetSandstorm().GetDirection()`, forces overcast ≥ 0.8, wind ≥ 18 m/s and rain 0, then calls `Start(fadeIn, false)` (`3_game\dayzgame.c:3408-3436`). `WorldDataWeatherSettings.m_SandstormMinWindMagnitude = 18` (`worlddata.c:596`). Copy that preparation in a mission/admin tool if you start storms by hand.

## Perceived intensity (shelter)

`GetIntensityForPlayer` (`4_world\systems\sandstorm\scriptedsandstormcontroller.c:142-178`):

1. Head position → `GetIntensity`. If ≤ 0, return 0.
2. `ApplyUndergroundModifier`: `EUndergroundPresence.TRANSITIONING` or `FULL`, fade 1.5 s; also if `playerPos[1] < SurfaceY` attenuates by depth (`MAX_SURFACE_DISTANCE = 2.0`) (`:180-210`). If `HumanCommandVehicle` is present, underground returns early (`:182-184`).
3. If still > 0: `ApplyBuildingCheckModifier` — `MiscGameplayFunctions.IsUnder(..., ObjIntersectIFire)` **and** `IsSoundInsideBuilding()` (`:222-228`).
4. If still > 0: `ApplyVehicleModifier` only if transport `IsAnyInherited({Car})` (`:265`).

Fade:

```c
// [EXACT] P:\scripts\4_world\systems\sandstorm\scriptedsandstormcontroller.c:15
	private static const float INTERIOR_TRANSITION_DURATION = 1.5; // 1.5 seconds transition
```

```c
// [EXACT] P:\scripts\4_world\systems\sandstorm\scriptedsandstormcontroller.c:129
	protected void ApplyShelterFade(inout float intensity, bool isCurrentlySheltered, float transitionProgress)
	{
		if (isCurrentlySheltered)
		{
			intensity = Math.Lerp(intensity, 0.0, transitionProgress);
		}
		else
		{
			intensity = Math.Lerp(0.0, intensity, transitionProgress);
		}
	}
```

State per player: `PlayerSandstormData` (`4_world\systems\sandstorm\playersandstormdata.c`). In MP the key is the identity id; in SP `"-1"` (`scriptedsandstormcontroller.c:49-58`). The underground check reads `PlayerBase.GetUndergroundPresence()` (`4_world\entities\manbase\playerbase.c:2848`; enum `EUndergroundPresence`, `4_world\classes\undergroundhandlerclient.c:1-7`) and is skipped while the player is in a vehicle (`scriptedsandstormcontroller.c:182-184`).

## Client: PPE

Client `OnStart` requests `PPERequesterBank.REQ_SANDSTORMEFFECT` and sets the target intensity (`scriptedsandstormcontroller.c:310-327`); `OnUpdate` refreshes it every tick (`:336-352`). Post-process: `PPERequester_SandstormEffect` (`3_game\ppemanager\requesters\ppersandstorm.c:1-39` constants: amber glow colour, saturation, contrast, godrays; registered at `3_game\ppemanager\pperequesterbank.c:49`). Material `P:\graphics\materials\postprocess\sandstorm.emat:1` shader `SnowEffect`, `ParticlesColor 0.69 0.49 0.29 1` at `:29`.

## cfgWorlds

New in 1.30 (config-delta: `CfgWorlds/CAWorld/Weather/Sandstorm`). `P:\DZ\data\config.cpp` is a de-rapified unpack, hence the multi-line array:

```cpp
// [EXACT] P:\DZ\data\config.cpp:1532  (CfgWorlds > CAWorld > Weather)
			class Sandstorm
			{
				soundSets[]=
				{
					"SandStorm_Debris_SoundSet",
					"SandStorm_Close_SoundSet",
					"SandStorm_Dist_SoundSet"
				};
				particlePath="Graphics/Particles/sandstorm/sandstorm";
			};
```

The particle file exists at `P:\graphics\particles\sandstorm\sandstorm.ptc`.

## cfggameplay.json

```c
// [EXACT] P:\scripts\3_game\cfggameplaydatajson.c:440
class ITEM_SandstormData : ITEM_DataBase
{
	override void InitServer()
	{
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	
	/*!
	* \brief Rate at which the sandstorm should occur.
	* \note Example values: 0 => never occur, 1 => occur at normal rate, 2 => occur twice as frequently.
	*/ 
	float sandstormFrequency = 1.0;
};
```

Registered as `ref ITEM_SandstormData SandstormData` on `CfgGameplayJson` (`cfggameplaydatajson.c:23`). `CfgGameplayHandler.GetSandstormFrequency()` (`3_game\cfggameplayhandler.c:533-536`) is copied to `WorldDataWeatherSettings.m_sandstormFrequency` in `SetupWeatherSettings`, negatives clamped to 0 (`3_game\worlddata.c:427-443`). The vanilla `cfggameplay.json` shipped for chernarusplus/enoch/sakhal (`P:\DZ\worlds\*\ce\cfggameplay.json`, `"version": 123`) has no `SandstormData` block, so the default 1.0 applies unless you add it.

## Nasdara: weather trigger

In `CalculateWind` bad weather, `sandstormChance = Clamp(8 * m_sandstormFrequency, 0, 100)` and a storm is chosen only if the previous weather had none (`4_world\classes\worlds\nasdara.c:561-579`). If a sandstorm results: `m_IsSandstorm = true`, and in the OVERCAST change `StartSandstorm(phmnTime * WIND_MAGNITUDE_TIME_MULTIPLIER)` (`:318-323`); it is stopped on the next overcast change (`:332-358`). Only `NasdaraData` calls `StartSandstorm`; ChernarusPlus/Enoch/Sakhal do not.

[DESIGN] On a custom map without a `CalculateWind` like Nasdara's, `sandstormFrequency` does nothing until someone calls `Start` (mission or custom `WorldData`).
