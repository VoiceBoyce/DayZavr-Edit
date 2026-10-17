# DayZ 1.30 (stable 1.30.164078) — Particles, explosions, dust, vehicle VFX, sandstorm

Verified against the unpacked 1.30.164078 stable tree: `P:\scripts\`, `P:\graphics\` (plain-text `.ptc` / `.emat`)
and `P:\DZ\` configs. `Before 1.30` notes were checked against the 1.29 unpack (`O:\DayZ 1.29 …`). An earlier
version of this page was written against 1.30 Experimental 164014; every static claim was re-checked on stable and
paths/lines below are the stable ones. Labels: `../../_shared/dayz-version-policy.md`.

## Client gate: `IsHeadlessOrDedicatedServer`

1.30 vanilla VFX code skips **headless clients as well as dedicated servers**.
Before 1.30: wrecks and FireplaceBase used `!g_Game.IsDedicatedServer()` (1.29: `wreck_mi8.c:6`, `fireplacebase.c:1096`).

`CGame.IsHeadlessOrDedicatedServer` is proto native (`P:\scripts\3_game\global\game.c:1136`; `IsHeadless()` at `:1130`).
Comment at `:1132-1135`: combines `IsDedicatedServer` and `IsHeadless`, and includes headed servers (`-headlessMode=0`).

```
// [EXACT] P:\scripts\4_world\entities\building\wrecks\wreck_mi8.c:1
//New russian helicopter crash site
class Wreck_Mi8_Crashed extends CrashBase
{
	void Wreck_Mi8_Crashed()
	{
		if ( !g_Game.IsHeadlessOrDedicatedServer() )
		{
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SMOKING_HELI_WRECK, this, Vector(2, 0, -5));
		}
	}
}
```

`ParticleManager.GetInstance()` only constructs the global pool when `!g_Game.IsHeadlessOrDedicatedServer()`
(`P:\scripts\3_game\particles\particlemanager\particlemanager.c:65`), so it returns **null** on headless clients.
FireplaceBase `PlayParticle` / `StopParticle` use the same check (`P:\scripts\4_world\entities\itembase\fireplacebase.c:1112,1132`),
as do `wreck_uh1y.c:5`, `wreck_santassleigh.c:13,40`, `boatscript.c:382,411`, `oilpittrigger.c:37,190`, and
`SEffectManager.Cleanup` (`P:\scripts\3_game\effectmanager.c:536`). Not every file switched: `wreck_chinook.c:6`,
`wreck_mi24_tk.c:5` and `effectarea.c:206` still test `IsDedicatedServer()` on 1.30.

[DESIGN] A mod that still gates with `!IsDedicatedServer()` runs its VFX branch on headless roboclients, where
`ParticleManager.GetInstance()` is null — a script null-pointer error unless it null-checks. Gate VFX with
`!g_Game.IsHeadlessOrDedicatedServer()` and keep the null-check.

## Synced spawn: `CreateParticleServer`

`SEffectManager.CreateParticleServer` existed in 1.29 (trees, bushes, fishing, traps — `actionminetree.c`,
`actionminebush.c`, `actionfishingnew.c`, `trapspawnbase.c`). 1.30 adds player ceiling dust as a caller.

```
// [EXACT] P:\scripts\4_world\classes\dust\dusteffectceilinghandler.c:91
//! Generic synced solution
class DustEffectCeilingHandlerSynced : DustEffectCeilingHandlerBase
{
	//! Request from server, effecter handles the rest
	override protected void CreateEffects(int ceilingParticleID, vector position)
	{
		//'spam' variant of effecters solution
		SEffectManager.CreateParticleServer(position, new ParticleEffecterParameters("CeilingDustEffecter", 2.0, ceilingParticleID));
	}
}
```

The local path (`DustEffectCeilingHandlerLocal`) uses `SEffectManager.PlayInWorld` on the client (`:77-89`).
Particle ID comes from `surfaceInfo.GetCeilingDustParticleId()` (`:27`). Players use the synced handler
(`P:\scripts\4_world\entities\dayzplayerimplement.c:204`); animals and infected use the local one
(`animalbase.c:49`, `zombiebase.c:1194`). The effecter class `CeilingDustEffecter` overrides the new protected hook
`ParticleEffecter.GetEffectTypename()` (`effectmanager.c:857`, called at `:811`) to spawn `CeilingDustEffectWood`
instead of a plain `EffectParticleGeneral`; 1.30 also adds `SEffectManager.GetEffecterByID(int)` (`effectmanager.c:649`).
Before 1.30: `ParticleEffecter` always created `new EffectParticleGeneral()`.

```
// [EXACT] P:\scripts\3_game\effectmanager.c:585
	//! returns unique effecter ID
	static int CreateParticleServer(vector pos, EffecterParameters parameters)
	{
		EffecterBase eff;
		eff = EffecterBase.Cast(g_Game.CreateObjectEx(parameters.m_EffecterType, pos, ECE_PLACE_ON_SURFACE));
		
		if (eff)
		{
			int id = GetFreeEffecterID();
			m_EffectersMap.Insert(id, eff);
		}
		
		eff.Init(id, parameters);
		return id;
	}
```

[DESIGN] PlayOnObject remains client-only. Use `CreateParticleServer` when every nearby client must see the same short-lived effecter.

## SurfaceInfo particle IDs

1.30: `Surface.GetStepsParticleID(string)` / `GetWheelParticleID(string)` are `[Obsolete]` wrappers (they still
return the right ID, with a compile warning). Before 1.30: they were the documented lookups.

```
// [EXACT] P:\scripts\4_world\static\surface.c:64
	[Obsolete("1.30: use SurfaceInfo.GetStepParticleId directly")]
	static int GetStepsParticleID(string surface_name) 
	{
		return SurfaceInfo.GetByName(surface_name).GetStepParticleId();
	}
	
	[Obsolete("1.30: use SurfaceInfo.GetWheelParticleId directly")]
	static int GetWheelParticleID(string surface_name) 
	{
		return SurfaceInfo.GetByName(surface_name).GetWheelParticleId();
	}
```

`SurfaceInfo` proto getters (`P:\scripts\3_game\surfaceinfo.c:47-70`): `GetStepParticleId` (already in 1.29),
`GetWheelParticleId` (already in 1.29), and new in 1.30 `GetStepSoundSetName`, `GetWheelSoundSetName`,
`GetWheelContactParticleId`, `GetWheelContactSoundSetName`, `GetVehicleDustParticleId`, `GetVehicleDustSoundSetName`,
`GetDustKickupEffectId` / `Small` / `Big`, `GetCloudEffectParticleId` / `GetCloudEffectSoundSetName`,
`GetGroundEffectParticleId` / `GetGroundEffectSoundSetName`, `GetCeilingDustParticleId`. Missing config entry →
`ParticleList.NONE`; entry set but not registered → `ParticleList.INVALID` (comment `:47-48`). Handles come from
`SurfaceInfo.GetByName(name)` or the new `CGame.GetSurfaceInfoUnderObject(obj)` / `GetSurfaceInfoOn(x, z, out h)`.

Not verified here: the CfgSurfaces key names behind the new getters. The surfaces configs in `P:\DZ` only use
`wheelParticle` (`P:\DZ\surfaces\config.cpp:673`) and `stepParticle` (`P:\DZ\surfaces_sakhal\config.cpp:250`); no
`P:\DZ` config sets vehicle-dust, wheel-contact, cloud/ground or ceiling-dust keys, so copy the key names from the
terrain that ships them before writing a custom `CfgSurfaces` entry.

## `ParticleParamsOverrideData` and vehicle VFX

New in 1.30 (absent from 1.29 scripts). `EffectParticle` declares these empty virtuals (no 1.29 equivalent on the
base; 1.29 had `SetParticleState(int)` only on `EffVehicleSmoke`, `EffWheelSmoke` and `EffectBoatWaterBase`):

```
// [EXACT] P:\scripts\3_game\effects\effectparticle.c:611
	void SelectFromSurface(string surface);
	void SelectFromSurface(SurfaceInfo surfaceInfo);
	
	void SetParticleState(int state);

	/**
	\brief Callback for overriding effect's particle attributes
	*/
	void ParticleParamsOverride(notnull ParticleParamsOverrideData data);
```

[DESIGN] Because these names now exist on the base class, vanilla added `override` to every subclass that already
declared them (`vehiclesmoke.c:3`, `wheelsmoke.c:19`, `boatwatereffects.c:292`). Do the same in your own
`EffectParticle` subclasses; whether omitting it is a compile error or only a warning on 1.30 was not tested.

```
// [EXACT] P:\scripts\3_game\effects\effectparticle.c:651
class ParticleParamsOverrideData
{
	ref TFloatArray m_FloatModifiers = new TFloatArray();
	
	void ClearAll()
	{
		m_FloatModifiers.Clear();
	}
}
```

`EffVehicleDust` (`P:\scripts\4_world\entities\effects\vehicledust.c:5-23`) reads `m_FloatModifiers[0]` as speed and `[1]` as weight, then `SetParticleParam` on `LIFETIME` (`2.5 * weight`), `REPEAT`, `WIND`, `VELOCITY` (`speed * weight`), `SIZE` (`speed * weight`), `AIR_RESISTANCE` (`1.0 * weight`). Threshold `WHEEL_DUST_THRESHOLD = 3.0` (`:3`). Surface ID: `surfaceInfo.GetVehicleDustParticleId()` (`:27`).

`EffWheelContact` (`P:\scripts\4_world\entities\effects\effwheelcontact.c:5-23`) sets (does not scale) `LIFETIME = 1.0 * weight`, `SPRING 15`, `REPEAT false`, `WIND false`, `SIZE 1`, `AIR_RESISTANCE 1.0`. Surface ID: `GetWheelContactParticleId()`; it also swaps a looping sound from `GetWheelContactSoundSetName()` via `SetEffectSound` (`:25-58`).

`EffExhaustSmoke` (`P:\scripts\3_game\effects\effectparticle\vehiclesmoke\exhaustsmoke.c:3-29`) scales `LIFETIME` / `LIFETIME_RND` by `1 + speed * 0.1` when `speed < 100` (else `0.1`), `BIRTH_RATE` / `BIRTH_RATE_RND` by `1 + speed * 0.1`, and sets `SIZE` / `VELOCITY` to `speed * 2`, `AIR_RESISTANCE 2.0`. Before 1.30: `SetParticleStateLight()` read the car speed itself and used `(100 - speed) / 100` for lifetime (1.29: `exhaustsmoke.c:3-27`); 1.30 keeps that method only "for possible backward compatibility" (`:31-41`).

`EffVehicleSmoke` lost its constructor (1.29: `vehiclesmoke.c:3-6` called `SetParticleStateLight()`), so a freshly created `EffExhaustSmoke` / `EffEngineSmoke` / `EffCoolantSteam` has no particle ID until you call `SetParticleState(id)`.

`EffWheelSmoke.SetSurface(string)` is `[Obsolete("1.30: use SelectFromSurface instead")]` (`P:\scripts\4_world\entities\effects\wheelsmoke.c:32-48`). The live path is `SelectFromSurface(SurfaceInfo)` → `GetWheelParticleId()` (`:5-17`).

`VehicleVFXComponent` (`P:\scripts\4_world\entities\vehicles\components\vehiclevfxcomponent.c:96`) registers single effects with `RegisterEffect(EVehicleVFXEffect, typename, VehicleVFXDataParams)` (`:126`) and groups with `RegisterEffectGroup(EVehicleVFXEffectGroup, typename, int count, VehicleVFXDataParams)` (`:139`); enums `EVehicleVFXEffect` (`INVALID`, `DUST_BEHIND`, `ENGINE_SMOKE`, `COOLANT_STEAM`, `EXHAUST_SMOKE`) and `EVehicleVFXEffectGroup` (`INVALID`, `WHEEL_SMOKE`, `WHEEL_CONTACT`) are in `P:\scripts\3_game\enums\evehiclevfxtypes.c:2-17`. `GetParticleParamsOverrideData` (`:373`) / `GetEffectGroupParticleParamsOverrideData` (`:530`) hand the caller the data object to fill. `PlayEffectData` calls `ParticleParamsOverride` on first play and, on later calls, `SelectFromSurface(surfaceInfo)` then `ParticleParamsOverride` (`:171-221`). CarScript wires it in `InitializeParticles()` (`P:\scripts\4_world\entities\vehicles\carscript.c:343-382`; memory points `ptcDustPos`, `ptcEnginePos`, `ptcCoolantPos`, `ptcExhaust_start` / `ptcExhaust_end`) and skips it on `IsHeadlessOrDedicatedServer()` (`:345`).

[DESIGN] Porting a modded car: 1.30 `CarScript` still declares `m_coolantFx`, `m_engineFx`, `m_exhaustFx`, `m_WheelSmokeFx` under `//! old particles` (`carscript.c:2776-2781`) but never assigns them. Code that tweaks those fields compiles and does nothing; go through `m_VFXComponent`.

## EffectParticle: other 1.30 changes

- Field `m_ForceRotationRelativeToWorld` is gone (compile error if read); the flag now lives in `m_PPFlags` as `ParticlePropertiesFlags.FORCE_WORLD_ROT`. Use `ForceParticleRotationRelativeToWorld(bool)` / `IsParticleRotationRelativeToWorld()` (`P:\scripts\3_game\effects\effectparticle.c:542-563`).
- `Start()` now creates with `ParticleManager.CreateParticleEx(m_ParticleID, pos, m_PPFlags, GetParent(), ori)` and then calls `PlayParticleEx` (`:198-213`). Before 1.30: `CreateParticle(..., true, ..., IsParticleRotationRelativeToWorld())`.
- `SetEffectSound(EffectSound)` (`:599`): the sound starts/stops with the effect (`:201-211`, `:232-235`).
- `DisableParticleInUI(bool)` / `IsDisableParticleInUI()` (`:569-580`) toggle the new `ParticlePropertiesFlags.DISABLED_IN_UI` (`P:\scripts\3_game\particles\particlemanager\particlesource.c:62-63`, comment "Not created in player review"; the native side reads it via `ParticleProperties.IsDisabledInUI()`, `:102`). Meaning per the `EffectParticle` doc comment: do not create the particle for the preview player. Vanilla user: `EffCloudDust` constructor (`P:\scripts\4_world\entities\effects\effclouddust.c:5`). The flags are combined bitwise (`m_PPFlags | flag`, `effectparticle.c:521-534`); the numeric value is not written in script — always use the enum name.
- `ParticleBase` gained `SetStartSoundParams(string soundSetName, vector position)` / `PlayStartSound()` hooked on `Event_OnParticleStart` (`P:\scripts\3_game\particles\particlebase.c:77,166-187`); explosion sounds use it.

## Explosion particles (moved to AmmoTypesAPI / CfgAmmo)

1.30 plays the explosion particle centrally, for every explosion with a source:

```
// [EXACT] P:\scripts\3_game\dayzgame.c:3634
			ExplosiveEffectData explosiveEffectData = AmmoTypesAPI.GetExplosiveEffectData(ammoType, surface);
			if (explosiveEffectData.m_ParticleID > 0)
			{
				ParticleSource particleSource = ParticleManager.GetInstance().PlayInWorld(explosiveEffectData.m_ParticleID, pos);
				particleSource.SetStartSoundParams(explosiveEffectData.m_SoundSetName, pos);
			}
```

`AmmoTypesAPI` (`P:\scripts\3_game\global\ammotypes.c`):
- `class ExplosiveEffectData { int m_ParticleID; string m_SoundSetName; }` (`:1-11`).
- `static void AddExplosionParticleEffect(string ammoName, string surfaceName, int particleID, string soundSetName = "")` (`:78`). Before 1.30: three parameters.
- `static ExplosiveEffectData GetExplosiveEffectData(string ammoName, string surfaceName)` (`:90-132`): surface entry → `"default"` entry → reads `cfgAmmo <ammo> particle` and resolves it with `ParticleList.GetParticleIDByName` (`:108-112,121-125`), caching it as `"default"`. Returns an empty result under `NO_GUI` (`:94-96`).
- `Init()` registers vanilla surface variants for `Explosion_40mm_Ammo`, `LandMineExplosion_Ammo`, `RGD5Grenade_Ammo`, `M67Grenade_Ammo` on `Hit_Snow` / `Hit_Ice` / `Hit_Desert_Sand` / `Hit_Sand` (`:52-72`); it runs from `MissionBase` (`P:\scripts\5_mission\mission\missionbase.c:40`) and only builds the map under `#ifndef NO_GUI` (`:47-50`).
- `GetExplosionParticleID(string, string)` was removed (1.29: `ammotypes.c:57`).

Vanilla CfgAmmo now carries `particle=` (added in 1.30 per the config delta): e.g. `RGD5Grenade_Ammo particle="explosion_RGD5_01"` (`P:\DZ\weapons\explosives\config.cpp:29`), `Plastic_Explosive_Ammo` `:209`, `ClaymoreMine_Ammo` `:247`, `Explosion_40mm_Ammo` `:359`, `LandMineExplosion_Ammo` (`P:\DZ\gear\traps\config.cpp:22`).

`ExplosivesBase` (`P:\scripts\4_world\entities\explosivesbase.c`): `OnExplosionEffects` only calls super and `CreateLight()` (`:66-70`). `SetParticleExplosion`, `InitSpecificsExplosionEffectForSurface`, both `AddExplosionEffectForSurface` overloads, `GetParticleExplosionID` and `GetExplosionSoundSetName` are `[Obsolete("1.30: Handled in AmmoTypes instead")]` (`:306-438`); vanilla no longer calls them (`claymoremine.c:18`, `plastic_explosive.c:15`, `grenade_chemgas.c:10` keep the old call commented out). Their static maps (`:41,43`) are never allocated on 1.30, so calling `AddExplosionEffectForSurface` dereferences null. `SetParticlePosition` / `SetParticleOrientation` still store values but the explosion particle no longer uses them (it plays in world at the explosion position).

Before 1.30: `ExplosivesBase.OnExplosionEffects` played `GetParticleExplosionID(surface)` with `PlayOnObject(…, m_ParticlePosition, m_ParticleOrientation)` (1.29: `explosivesbase.c:64-85`), the ID came from `SetParticleExplosion(...)` in each explosive's constructor, and `DayZGame.ExplosionEffects` only handled `ShootsExplosiveAmmo()` sources via `GetExplosionParticleID` (1.29: `dayzgame.c:3532-3539`).

[DESIGN] Custom explosive on 1.30: register the `.ptc` in `modded class ParticleList` with a unique file name, set `particle = "<file name>"` on its CfgAmmo class, and for per-surface variants call `AmmoTypesAPI.AddExplosionParticleEffect` after `AmmoTypesAPI.Init()` under `#ifndef NO_GUI`. The hook point in a mod was not tested here.

## Bullet impacts (desert sand, mud brick)

New effect classes `Hit_Desert_Sand` and `Hit_Mud_Brick : EffBulletImpactBase` (`P:\scripts\3_game\effects\effectparticle\bulletimpactbase\hit_desert_sand.c:1-9`, `hit_mud_brick.c:1-9`) use the new `IMPACT_DESERT_SAND_*` / `IMPACT_MUD_BRICK_*` particles. `ImpactMaterials.MUD_BRICK = RegisterSurface("Hit_Mud_Brick")` is new (`P:\scripts\3_game\impacteffects.c:45`); `DESERT_SAND = RegisterSurface("Hit_Desert_Sand")` already existed in 1.29 (`:44`) but had no effect class.

## ParticleList registration (1.30)

`RegisterParticle(string root_path, string file_name)` now lowercases both arguments and keys the name map by the last path segment (`P:\scripts\3_game\particles\particlelist.c:455-462,485`); `GetParticleID` and `GetParticleIDByName` lowercase their argument (`:543,554`), and `GetParticleIDByName` returns `ParticleList.NONE` when not found (`:556`). Consequences: `GetParticlePath(id)` returns a lowercase path; `GetParticleIDByName("impacts/hit_wood_ent_01")` (worked on 1.29) now fails — pass `"hit_wood_ent_01"`; two particles with the same file name in different folders log `Duplicate particle filename` and name lookup returns the first. Before 1.30: no lowercasing and the name key was the `file_name` argument as passed (1.29: `particlelist.c:441,462`).

## EffectArea tick and snap

1.30 tick hook and road snap. Before 1.30: `override void OnCEUpdate()` and `partPos[1] = g_Game.SurfaceY(...)` (1.29: `effectarea.c:190,413`).

```
// [EXACT] P:\scripts\4_world\classes\contaminatedarea\effectarea.c:190
	// Through this we will evaluate the resize of particles
	override void OnCEIterate(float currentTime, float elapsedTime)
	{
		super.OnCEIterate(currentTime, elapsedTime);
		Tick();
	}
```

```
// [EXACT] P:\scripts\4_world\classes\contaminatedarea\effectarea.c:412
	protected void SpawnParticles(ParticlePropertiesArray props, vector centerPos, vector partPos, inout int count)
	{
		float roadY = g_Game.SurfaceRoadY(partPos[0], partPos[2]);	// Snap particles to ground
		
		if (partPos[1] > roadY)
			partPos[1] = roadY;
```

[DESIGN] `OnCEUpdate` is now an empty `[Obsolete]` stub on `EntityAI` (`P:\scripts\3_game\entities\entityai.c:4924-4925`) with no script caller; a custom area that only overrides it will not tick [reported: bzed guide — 0 `OnCEUpdate` calls vs 446k `OnCEIterate` in 2 min on a Linux 1.30.164014 server]. Override `OnCEIterate(float currentTime, float elapsedTime)` and call super. The emitter starts at the area centre's Y and is only lowered (never raised) to `SurfaceRoadY`, so gas sits on bridge decks/roads; an emitter starting below the surface keeps the centre height (it is no longer forced onto terrain). The trigger cylinder and damage did not change. `EEffectAreaType` gained `OIL_PIT = 8` (`effectarea.c:15`; `OilPitArea : EffectArea`).

## Sandstorm: postprocess `.emat` vs `.ptc`

Weather `CfgWorlds/CAWorld/Weather/Sandstorm` (`P:\DZ\data\config.cpp:1532-1541`, new in 1.30) sets `soundSets[] = {"SandStorm_Debris_SoundSet","SandStorm_Close_SoundSet","SandStorm_Dist_SoundSet"}` and `particlePath = "Graphics/Particles/sandstorm/sandstorm"` (`:1540`). No script references that path; the engine reads it.

`sandstorm.emat` is **not** a `Particle` / `ParticleSprite` material. It starts with shader `SnowEffect` (`P:\graphics\materials\postprocess\sandstorm.emat:1`). Tint is `ParticlesColor 0.69 0.49 0.29 1` (`:29`). Texture `SnowFlakesSheets_BCA.edds` (`:13`). Nasdara rain uses a separate postprocess shader `RainEffect` (`P:\graphics\materials\postprocess\rainnasdara.emat:1`).

`P:\graphics\particles\sandstorm\sandstorm.ptc` is plain text: one emitter `EmitorDef sand` (`:4`), `ShapeType Box` (`:5`), `ShapeSize 800 800 9000` (`:6`), `MaxNum 200` (`:8`), `BirthRate 20` (`:9`), `Material "{9BE091A165651ACE}Graphics/Particles/materials/sandstorm/dust_fluid_02.emat"` (`:14`, an ordinary `Particle`-shader material). The other sandstorm particle materials are in `P:\graphics\particles\materials\sandstorm\`.

## Oil pits

`OilPitTrigger.UpdateState` (`P:\scripts\4_world\entities\scriptedentities\triggers\oilpittrigger.c:90-116`) randomly plays `ParticleList.OIL_FIRE1` + `BONFIRE_SMOKE` or `OIL_FIRE2` + `SMOKE_GENERIC_WRECK` with `ParticleManager.PlayInWorld`. Light is `BonfireLight` at `GetPosition() + "0 2.5 0"`. Client paths are gated with `IsHeadlessOrDedicatedServer` (`:37`, `:190`).

## Player ambient dust (`DustEffectManager`, `EffCloudDust`)

`DustEffectManager` (`P:\scripts\4_world\classes\dust\dusteffectsmanager.c:1`, singleton `GetDustEffectManager()`) runs only for the local client player (`P:\scripts\4_world\entities\manbase\playerbase.c:3315-3318,6377`) and is skipped underground. When wind exceeds its thresholds it spawns ground puffs with `ParticleManager.PlayInWorldEx(surfaceInfo.GetGroundEffectParticleId(), …)` (`dusteffectsmanager.c:275,286`) and a cloud effect `EffCloudDust` attached to the player via `SEffectManager.PlayOnObject` (`:367-381`), whose particle comes from `GetCloudEffectParticleId()` or the default `ParticleList.DUST_CLOUD` (`effclouddust.c:8-33`). `EffCloudDust` calls `DisableParticleInUI(true)` (`effclouddust.c:5`). It also owns the ceiling-dust type pairing (`RegisterCeilingPair(ParticleList.CEILING_WOOD, "CeilingDustEffectWood")`, `dusteffectsmanager.c:152`).

## Dust kickup (`DustKickupEffects`)

`DustKickupEffects` (`P:\scripts\3_game\dustkickupeffects.c:43`) is **weapon-blast** dust, not footsteps: it reads `CfgMagazines <class containing "Ammo"> Effects <stance> { minDist; maxDist; Effects[] }` (`:89-132`) and `CfgSurfaces <surface> DustEffects <name> path` (`:135-173`, registered via `RegisterParticleByFullPath`), and `PlayParticle(DustKickupParams)` plays when the hit distance of a trace is under 2 m (`MAX_DISTANCE_FOR_EFFECT`, `:53`, `:208-255`). Not verified: who calls it — nothing in `P:\scripts` calls `GetInstance()` / `PlayParticle` (only `CleanupInstance`, `P:\scripts\3_game\global\game.c:81`), no `P:\DZ` config defines `DustEffects`, and the `SurfaceInfo.GetDustKickupEffect*Id` getters have no script caller. Treat it as unwired in vanilla script until seen working in game.

## New ParticleList constants (1.30 vs 1.29)

Verified in `P:\scripts\3_game\particles\particlelist.c` by diff against 1.29: 26 constants added, none removed or renamed (304 total vs 278; the commented `MODDED_PARTICLE` example is not counted). `STEP_DESERT` / `STEP_SOIL` already existed in 1.29 (`:374-375`) but were missing from the catalog:

| Constant | .ptc | Lines |
|---|---|---|
| `TANDOOR_HOUSE_SMALL_FIRE` … `TANDOOR_HOUSE_FIRE_STEAM_2END` | tandoor_fire_* / tandoor_smoke_* / tandoor_steam_* | 48-54 |
| `EXPLOSION_GRENADE_SAND` / `EXPLOSION_GRENADE_DESERT_SAND` | explosion_grenade_sand, explosion_grenade_desert_sand | 145-146 |
| `IMPACT_DESERT_SAND_*` / `IMPACT_MUD_BRICK_*` | impacts/hit_desert_sand_*, hit_mud_brick_* | 266-271 |
| `CEILING_WOOD` | `RegisterParticleByFullPath("graphics/particles/dust/dust_ceiling_wood")` | 378 |
| `BUILDNG_DESTRUCT_WOOD` / `BRICK` / `METAL` | building_destruction_* | 381-383 |
| `VEHICLE_WHEEL_SOIL` / `VEHICLE_WHEEL_DESERT` | vehicle_wheel_soil / vehicle_wheel_desert | 390-391 |
| `DUST_GROUND` / `DUST_CLOUD` / `DUST_VEHICLE` / `DUST_VEHICLE_SOFT` | dust/dust_grnd_01, dust/dust_effect_01, dust_vehicle, dust_vehicle_soft | 405-409 |
| `OIL_FIRE1` / `OIL_FIRE2` | fire_oil1, fire_oil2 | 411-412 |

Every `.ptc` above exists as plain text under `P:\graphics\particles\` (checked by file). Five of them —
`building_destruction_wood/brick/metal.ptc`, `explosion_grenade_sand.ptc`, `explosion_grenade_desert_sand.ptc` —
spell the emitter block `EmitterDef` instead of `EmitorDef`; 1.29 shipped no `EmitterDef` file. A `.ptc` parser
or generator must accept both spellings.

Catalog tables: `references/vanilla-particle-catalog.md`.
