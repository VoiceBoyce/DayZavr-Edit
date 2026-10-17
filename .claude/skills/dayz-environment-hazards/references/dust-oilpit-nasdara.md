# Surface dust, oil pits, and Nasdara

Source: DayZ 1.30 stable `1.30.164078`; paths relative to `P:\scripts\` unless they start with `P:\`. Detail for `.ptc` / `ParticleParamsOverrideData` / headless belongs to `dayz-particles`. Here only hazard and world contract.

## SurfaceInfo

New protos (`3_game\surfaceinfo.c:49-70`): step / wheel / wheel-contact / vehicle-dust (id + soundset), kickup (id / small / big), cloud and ground ambient, `GetCeilingDustParticleId()`. Thickness: `GetThickness()` (`:31`). Lookup: `SurfaceInfo.GetByName(name)` / `GetByFile(name)` (`:16, 22`).

Obsolete wrappers (still compile with `SCRIPT (W)`):

```c
// [EXACT] P:\scripts\4_world\static\surface.c:64
	[Obsolete("1.30: use SurfaceInfo.GetStepParticleId directly")]
	static int GetStepsParticleID(string surface_name) 
	{
		return SurfaceInfo.GetByName(surface_name).GetStepParticleId();
	}
```

`Surface.GetWheelParticleID` is obsolete the same way (`surface.c:70-74`). `EffWheelSmoke.SetSurface(string)` is `[Obsolete("1.30: use SelectFromSurface instead")]` (`4_world\entities\effects\wheelsmoke.c:32-33`); the replacement is `override void SelectFromSurface(SurfaceInfo surfaceInfo)` (`:5-17`, base declaration `3_game\effects\effectparticle.c:611-612`). Vanilla no longer calls `SetSurface` anywhere — overrides of it are dead code on 1.30 (1.29 called it from `CarScript`, 1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\4_world\entities\vehicles\carscript.c:1172, 1182`).

## Ceilings

`DustEffectCeilingHandlerBase.OnStepUpdate` (`4_world\classes\dust\dusteffectceilinghandler.c:25-41`): if `GetCeilingDustParticleId() > 0`, every N steps (4–8 default) lowers `pos.y` by `GetThickness()` (default 0.4 if -1) and spawns. Local: `SEffectManager.PlayInWorld`. Synced: `SEffectManager.CreateParticleServer(..., ParticleEffecterParameters("CeilingDustEffecter", ...))` (`:91-99`).

## Environment and triggering

`DustEffectManager` reads `WorldData.GetDustEffectsSettings()` (`4_world\classes\dust\dusteffectsmanager.c:81`, `3_game\worlddata.c:548-551`). Nasdara turns on ground+cloud (`4_world\classes\worlds\nasdara.c:152-169`): minimum wind 8 / 5, distances 30 / 16. Kickup: `DustKickupEffects` + `KickupDustEffect_SoundSet` (`3_game\dustkickupeffects.c:43-45`).

## Vehicles

`VehicleVFXComponent.PlayEffectData` detects ground with `g_Game.GetSurface` and passes `SurfaceInfo` (`4_world\entities\vehicles\components\vehiclevfxcomponent.c:171-185, 213`). `EffVehicleDust.SelectFromSurface` uses `GetVehicleDustParticleId()`; `ParticleParamsOverride` sets LIFETIME/REPEAT/WIND/VELOCITY/SIZE/AIR_RESISTANCE from speed × weight (`4_world\entities\effects\vehicledust.c:5-36`). `Transport.GetWeightCoef()` default 1.0 (`3_game\vehicles\transport.c:475-478`); overrides: `Offroad_02` 1.5 (`4_world\entities\vehicles\inheritedcars\offroad_02.c:133-136`), `Truck_01_Base` 2.0 (`4_world\entities\vehicles\inheritedcars\truck_01_base.c:120-123`), `Motorbike_01` 0.5 (`4_world\entities\vehicles\inheritedmotorbikes\motorbike_01.c:87-90`), `Motorbike_02` 0.75 (`4_world\entities\vehicles\inheritedmotorbikes\motorbike_02.c:77-80`). An older note quoting "motorbike 0.25" is wrong for 164078.

`EffWheelContact.SelectFromSurface` (`4_world\entities\effects\effwheelcontact.c:25-58`): `GetWheelContactParticleId` + `GetWheelContactSoundSetName`.

## Oil pit

Area enum (`4_world\classes\contaminatedarea\effectarea.c:10-16`): `EEffectAreaType.OIL_PIT = 8` (new in 1.30; the enum is `EEffectAreaType`, not `EOffsetDataAreaType`).

`OilPitArea : EffectArea` (`4_world\classes\contaminatedarea\oilpitarea.c:7-62`): `PRE_BURNING_DURATION_MIN/MAX = 3/10`, rolled with `Math.RandomInt` (max exclusive → 3–9 s, `:52-55`). `InitZoneServer` creates the trigger and `CallLater(StartBurning, m_RandomizedBurningStart * 1000)` (`:33-50`). `StartBurning` → `AddState(BURNING)` (`:57-61`). `EEDelete` removes the pending call and stops effects (`:23-31`).

Damage:

```c
// [EXACT] P:\scripts\4_world\entities\scriptedentities\triggers\oilpittrigger.c:7
	protected const float 	DAMAGE_TICK_RATE 	= 1.0;	// seconds between fire damage ticks
	protected const float 	DAMAGE_MULTIPLIER	= 20.0;	// damage per tick
```

```c
// [EXACT] P:\scripts\4_world\entities\scriptedentities\triggers\oilpittrigger.c:63
	override void OnStayServerEvent(TriggerInsider insider, float deltaTime)
	{
		if (!m_DealDamageFlag)
			return;

		EntityAI entity = EntityAI.Cast(insider.GetObject());
		if (entity)
			entity.ProcessDirectDamage(DamageType.CUSTOM, this, "", "HeatDamage", "0 0 0", DAMAGE_MULTIPLIER);
	}
```

The damage tick: `OnStayStartServerEvent` accumulates time only while `BURNING` and raises `m_DealDamageFlag` once it exceeds `DAMAGE_TICK_RATE` (`oilpittrigger.c:53-61`); the flag and accumulator reset in `OnStayFinishServerEvent` (`:73-80`). State is a net-synced bitmask `m_OilPitState` (`:14, 28`; `AddState/RemoveState/CheckState` `:166-184`).

Client: `BonfireLight` at +2.5 m (inside `#ifndef SERVER` / `#ifndef NO_GUI`); fire `OIL_FIRE1`+`BONFIRE_SMOKE` or `OIL_FIRE2`+`SMOKE_GENERIC_WRECK`; sound `oil_pit_burn_SoundSet`, audible range 200 m, re-checked every 2 s from `EOnFrame` (`oilpittrigger.c:3-5, 90-126, 186-224`). Position adjusted with `SurfaceRoadY(..., RoadSurfaceDetection.UNDER)` (`:128-134`). The ignition branch of `UpdateState` runs from `OnVariablesSynchronized` without a headless guard (`:82-88, 92-116`; only the `DeferredInit` call checks `IsHeadlessOrDedicatedServer`, `:37`).

[DESIGN] Custom area: inherit `OilPitArea` or copy the trigger + net-sync pattern. Override `OnCEIterate(float currentTime, float elapsedTime)`, never `OnCEUpdate()` — on 1.30 `OnCEUpdate` is obsolete and not called (`3_game\entities\entityai.c:4924-4925`; `EffectArea` tick at `effectarea.c:191-195`).

[UNVERIFIED] Spawning from `cfgeffectarea.json`: the area and trigger are created by type name (`4_world\classes\contaminatedarea\contaminatedarealoader.c:93-96` for the area, `effectarea.c:496` for the trigger). No `CfgVehicles` entry for `OilPitArea`/`OilPitTrigger` exists in the unpacked `P:\DZ` configs, and the vanilla `cfgeffectarea.json` files on `P:\DZ\worlds\*\ce\` contain no oil pit (Nasdara's are not on `P:\`). Test a JSON-spawned oil pit in game before relying on it.

## Nasdara (DLC Badlands)

Stable 1.30 script side (verified on `P:\`): `NasdaraData : WorldData` (`4_world\classes\worlds\nasdara.c:2`), sand weather + `CalculateWind` (see [sandstorm-shelter.md](sandstorm-shelter.md)), July max temperature 38.4 (`nasdara.c:49`), solar table (`:67-73`), dust ground/cloud ON (`:152-169`). Lighting path `DZ\worlds\nasdara\data\lighting\lighting_nasdara.txt` (`3_game\worldlighting.c:6`); DLC id `EDLCId.BADLANDS` (`3_game\services\contentdlc.c:7`); dynamic music `DynamicMusicPlayerRegistryNasdara` (`3_game\systems\dynamicmusicplayer\dynamicmusicplayerregistrynasdara.c:1-5`); Nasdara houses use `BuildingWithFireplaceNasdara` (`4_world\entities\building\buildingwithfireplace.c:19`).

World data — **observed on 1.30 Exp 164014 only; the Nasdara DLC data is not unpacked on `P:\`, so none of this was re-verified on stable**:

- `CfgMods.nasdara` `appId = 3816030`; internal prefix `DZ\data_takistan`; the Exp data PBO had 6 files (`aiconfigs\config.bin`, `basicdefines.hpp`, `config.bin`, plus an impact-test `.p3d`/`.bisurf`/`.rvmat`) and no `.wrp`/navmesh.
- Skinning (`basicDefines.hpp`): mouflon 10 steaks / 2 guts / 1 lard / 1 bones; dog + head; lizard 1 of each.
- AI: `DZMouflonGroupBeh` type `DomesticHerbivores`; `DZDogGroupBeh` type `Predators` with `siegeAttackCountdownMin/Max` 10–15.

[CHANGELOG 1.30] The 1.30 navmesh format is not backwards compatible: regenerate navmeshes for custom terrains.

## AI noise (pointer)

`AIParams.sandstormToNoiseMultiplier = 10` (`P:\DZ\data\aiconfigs\config.cpp:20`; new in 1.30 together with `snowfall/fog/windToNoiseMultiplier`, `:15-20`). The engine applies it natively; script `Weather.GetNoiseReductionByWeather()` / `GetNoiseReductionByWeatherEx()` are obsolete and not called by vanilla (`3_game\weather.c:407-467`), so tune AI hearing in config, not by modding those functions. See `dayz-ai-patterns`.
