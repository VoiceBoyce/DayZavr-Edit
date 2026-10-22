---
name: dayz-environment-hazards
description: >
  DayZ 1.30 environmental hazards. Use when: sandstorm, sandstormFrequency,
  SandstormData, ScriptedSandstormController, shelter fade, ThermalBias, heat
  stroke, HeatStroke, IsInShadow, oil pit, OilPitArea, EffectArea OnCEIterate
  (OnCEUpdate no longer called), silicosis, irritated eyes, DEF_DUST_PARTICLE,
  dust_particle_breath/eyes, AGT_AIRBOURNE_SOLID, Nasdara, Badlands, WashHead,
  solar exposure, porting a 1.29 hazard/effect-area mod. Not particle .ptc/.emat
  authoring: dayz-particles. Not worn-model clothing: dayz-clothing. Not character
  import: dayz-characters. Not player stream layout: dayz-persistence. Not AI noise
  multipliers: dayz-ai-patterns. Not vehicle VFX components: dayz-vehicles.
---

# DayZ Environment Hazards (1.30)

> Target: DayZ 1.30 stable (1.30.164200). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

DayZ 1.30 adds native environmental hazards that did not exist on 1.29: a hybrid engine+script sandstorm (`Weather.GetSandstorm()`, `P:\scripts\3_game\weather.c:210`; `SandstormController`, `P:\scripts\3_game\sandstorm.c:6-135`), exposure with shelter fade in `ScriptedSandstormController`, heat stroke via the `ThermalBiasHandler` accumulator (high heat no longer damages health from `HeatComfortMdfr`), eye irritation and silicosis via `AGT_AIRBOURNE_SOLID`, `OilPitArea` oil pits, dust via `SurfaceInfo`, and sun exposure gated by `Environment.IsInShadow()`. The script side of the Nasdara world (DLC Badlands) ships in `P:\scripts\4_world\classes\worlds\nasdara.c` (sand weather, solar table); the Nasdara world data is **not** unpacked on `P:\`, so map/config facts about it come from 1.30 Exp and are labelled as such.

Every vanilla citation below was re-checked against `P:\` (1.30.164078) on 2026-10-15. Nothing here is `runtime_verified` unless a sentence says "measured".

## Family selector

| Case | Skill / reference | Files |
|---|---|---|
| Activate or balance sandstorm in custom world | **This skill** | [references/sandstorm-shelter.md](references/sandstorm-shelter.md) |
| Heatstroke, shadow, WashHead, motorbike/boat in wind | **This skill** | [references/thermal-heatstroke.md](references/thermal-heatstroke.md) |
| Masks, glasses, silicosis, eye irritation | **This skill** + `dayz-clothing` for item | [references/ppe-agents.md](references/ppe-agents.md) |
| Ceiling dust, kickup, wheel `.ptc` | `dayz-particles` (VFX detail) + link here | [references/dust-oilpit-nasdara.md](references/dust-oilpit-nasdara.md) |
| Oil pit / `EffectArea` tick on 1.30 | **This skill** | [references/dust-oilpit-nasdara.md](references/dust-oilpit-nasdara.md) |
| Nasdara / Badlands / mouflon-dog-lizard | **This skill** (script side only; world data not on `P:\`) | [references/dust-oilpit-nasdara.md](references/dust-oilpit-nasdara.md) |
| `PlayerBase` + `ThermalBiasHandler` stream | `dayz-persistence` | `P:\scripts\4_world\classes\thermalbiashandler.c:67-78`, called from `P:\scripts\4_world\entities\manbase\playerbase.c:7395` (save) / `:7520` (load) |
| `AIParams.sandstormToNoiseMultiplier` | `dayz-ai-patterns` | `P:\DZ\data\aiconfigs\config.cpp:20` (class `AIParams` `:12-24`) |

Recommendation: a new environmental hazard inherits the vanilla pattern (`EffectArea` + synced trigger, or `Weather.GetSandstorm().Start` driven from `WorldData`). Do not reuse direct damage from `HeatComfortMdfr` for extreme heat: on 1.30 that path no longer exists (heat damage lives in `HeatStrokePhase4Mdfr`).

## Day-0 critical path for sandstorm + heat in a custom world

1. [EXACT] **Access to phenomenon**: the controller cannot be instantiated in script (private ctor, `P:\scripts\3_game\sandstorm.c:12-13`). On the server: `GetGame().GetWeather().GetSandstorm().Start(duration, false)` / `.Stop(duration, false)` (`sandstorm.c:20, 27`; both server-only per the vanilla comments).
   ```c
   // [EXACT] P:\scripts\3_game\weather.c:210
   	proto native Sandstorm GetSandstorm();
   ```
2. [EXACT] **World config**: `class Sandstorm` block with `soundSets[]` and `particlePath` under `CfgWorlds > CAWorld > Weather` (`P:\DZ\data\config.cpp:1532-1541`). New in 1.30 (config-delta: `CfgWorlds/CAWorld/Weather/Sandstorm`).
3. [EXACT] **JSON frequency**: `"SandstormData": { "sandstormFrequency": 1.0 }` in `cfggameplay.json` (`ITEM_SandstormData`, `P:\scripts\3_game\cfggameplaydatajson.c:440-459`; getter `CfgGameplayHandler.GetSandstormFrequency()`, `P:\scripts\3_game\cfggameplayhandler.c:533-536`). Default 1.0; negative values are clamped to 0 (`P:\scripts\3_game\worlddata.c:427-443`). `0` disables weather-triggered storms; `2` doubles Nasdara's 8 % bad-weather roll to 16 % (`P:\scripts\4_world\classes\worlds\nasdara.c:563-566`). The vanilla `cfggameplay.json` files on `P:\DZ\worlds\{chernarusplus,enoch,sakhal}\ce\` have no `SandstormData` block, so the script default applies.
4. [DESIGN] **WorldData**: for automatic storms like Nasdara, copy the `CalculateWind` + `StartSandstorm` pattern (`nasdara.c:318-323` start, `:561-579` chance roll; helper `WorldData.StartSandstorm`, `P:\scripts\3_game\worlddata.c:477-490`). Only `NasdaraData` calls `StartSandstorm` in vanilla; ChernarusPlus never starts a sandstorm.
5. [EXACT] **Shelter**: per-player intensity is `GetIntensity(head)` attenuated over 1.5 s if underground, interior (roof **and** `IsSoundInsideBuilding`) or `Car` (`P:\scripts\4_world\systems\sandstorm\scriptedsandstormcontroller.c:15, 142-178, 213-248, 250-282`). Motorbike and boat do **not** count as shelter.
6. [EXACT] **PPE**: `DEF_DUST_PARTICLE_BREATH` on a MASK-category item against silicosis; `DEF_DUST_PARTICLE_EYES` on EYEWEAR/MASK/NVG-category items against irritation (`P:\scripts\3_game\constants.c:545-550`, `P:\scripts\4_world\plugins\pluginbase\plugintransmissionagents.c:414-436`). Config keys are `class Protection { dust_particle_breath; dust_particle_eyes; }` (`P:\scripts\4_world\entities\itembase.c:4077-4106`); see [references/ppe-agents.md](references/ppe-agents.md) for the category trap.
7. [EXACT] **Heat**: `HeatComfortMdfr` feeds `ThermalBiasHandler.Add` above comfort thresholds (`P:\scripts\4_world\classes\playermodifiers\modifiers\heatcomfortmdfr.c:74-111`). The 4 phases of `HeatStroke` read `GetThermalBiasHandler().Get()` (`P:\scripts\4_world\classes\playermodifiers\modifiers\diseases\heatstroke.c:4, 58-61`).
8. [EXACT] **Shadow and sun**: `CheckShadowPresence` every `ENVIRO_TICK_SHADOW_RC_CHECK` = 30 s, not 5 (`P:\scripts\3_game\constants.c:760`; `P:\scripts\4_world\classes\environment\environment.c:405-414, 665-720`). The sun term is only added when the world filled `m_TemperatureUnderSunModifier` (`environment.c:868-872`); vanilla fills it only in `NasdaraData.Init` (`nasdara.c:67-73`).
9. [DESIGN] **Economy / CE**: register `OilPitArea` zones via `cfgeffectarea.json` like any `EffectArea`. Detail in [references/dust-oilpit-nasdara.md](references/dust-oilpit-nasdara.md).

## The ten invariants that break an environmental hazard

Paths below are relative to `P:\scripts\` unless they start with `P:\`; `1.29: …` paths are relative to `O:\DayZ 1.29 Старый распакованный клиент\`.

### 1. Only `Car` (closed) cancels sandstorm; motorbike and boat do not
- **Symptom**: motorbike rider keeps squinting and accumulating silicosis inside storm.
- **Cause**: `ApplyVehicleModifier` requires `transport.IsAnyInherited({Car})` (`4_world\systems\sandstorm\scriptedsandstormcontroller.c:265`).
- **Remedy**: do not assume generic vehicular shelter. For a custom closed vehicle, inherit from `Car` or override `GetIntensityForPlayer`.

### 2. Interior = roof **and** building sound
- **Symptom**: being under an eave or a `CrashBase` does not turn off the storm.
- **Cause**: `isCurrentlyInside = (isUnderRoof && isInInterior)` and `EXCLUDED_SHELTER_TYPES = {CrashBase, Plant}` (`scriptedsandstormcontroller.c:20, 222-228`).
- **Remedy**: modded buildings must trigger `IsSoundInsideBuilding()` and have `ObjIntersectIFire` geometry for `MiscGameplayFunctions.IsUnder` (`4_world\static\miscgameplayfunctions.c:1107`).

### 3. Lethal heat no longer lives in `HeatComfortMdfr`
- **Symptom**: a 1.29 mod trimming heat damage in `HeatComfortMdfr` stops doing anything (player still dies from `HeatStrokePhase4`).
- **Cause**: the high-heat tick calls `thermalBiasHandler.Add(...)` (`4_world\classes\playermodifiers\modifiers\heatcomfortmdfr.c:74-111`). Health damage is in `HeatStrokePhase4Mdfr.OnTick` (`4_world\classes\playermodifiers\modifiers\diseases\heatstroke.c:307-309`).
- **Before 1.30**: `HeatComfortMdfr.OnTick` itself subtracted health above `WATER_LOSS_THRESHOLD_HC_PLUS_HIGH` (1.29: `scripts\4_world\classes\playermodifiers\modifiers\heatcomfortmdfr.c:51-56, 73-76`).
- **Remedy**: balance `PlayerConstants.THERMAL_BIAS_*` (`3_game\playerconstants.c:203-210`) or the `HeatStroke` phases. Cold still subtracts health in the same modifier (`heatcomfortmdfr.c:62-71`).

### 4. `HeatStrokePhase1` breaks sickness badge
- **Symptom** [CHANGELOG 1.30; the counter imbalance is still in the 164078 source]: sickness icons do not reappear.
- **Cause**: empty `OnActivate`, but `OnDeactivate` calls `DecreaseDiseaseCount()` (`heatstroke.c:68-75`). `ImmuneSystemMdfr` only refreshes `NTF_SICK` when `HasDisease()` changes (`4_world\classes\playermodifiers\modifiers\immunesystem.c:44-55`, `4_world\entities\manbase\playerbase.c:1119-1122`).
- **Remedy**: if modding phase 1, increment the counter in activate or do not decrement in deactivate.

### 5. Respiratory PPE ≠ Eye PPE, and only categorised items count
- **Symptom**: eyewear prevents silicosis, a mask without `dust_particle_eyes` does not prevent squinting, or a custom headgear with `dust_particle_*` values protects nothing.
- **Cause**: `AGT_AIRBOURNE_SOLID` uses MASK for `SILICOSIS` and EYEWEAR|MASK|NVG for `EYES_IRRITATION` (`4_world\plugins\pluginbase\plugintransmissionagents.c:414-436`). Squint checks `DEF_DUST_PARTICLE_EYES` (`4_world\classes\playersymptoms\states\secondary\squintstate.c:25-40`). Items whose `GetCachedEquipmentCategory()` is `NONE` (the `EntityAI` default, `3_game\entities\entityai.c:322-325`) are never stored in the cached-equipment lookup (`4_world\entities\cachedequipmentstorage.c:31`).
- **Remedy**: declare `class Protection { dust_particle_breath=…; dust_particle_eyes=…; }` in the item config **and** inherit `Mask_Base`/`Glasses_Base` (or override `GetCachedEquipmentCategory()`). Search depth = 2 (`plugintransmissionagents.c:577-592`).

### 6. `OnCEUpdate` is never called on 1.30: the `EffectArea` tick is `OnCEIterate`
- **Symptom**: a ported custom area (or any CE-ticked entity) does not resize particles / does not iterate — and compiles with only a `SCRIPT (W)` obsolete warning.
- **Cause**: `EntityAI.OnCEUpdate()` is `[Obsolete("1.30: Use OnCEIterate(float currentTime, float elapsedTime) instead")]` with no body (`3_game\entities\entityai.c:4924-4925`) and no vanilla script calls it. The CE calls `OnCEIterate(float currentTime, float elapsedTime)` (`entityai.c:3893-3911`); `EffectArea` overrides it to call `Tick()` (`4_world\classes\contaminatedarea\effectarea.c:191-195`). [reported: bzed, Linux 1.30.164014 server: `OnCEUpdate` = 0 calls, `OnCEIterate` = 446k in 2 min]
- **Before 1.30**: `EffectArea` overrode `OnCEUpdate()` (1.29: `scripts\4_world\classes\contaminatedarea\effectarea.c:190-194`) and used `m_ElapsedSinceLastUpdate`; that field and `m_LastUpdatedTime` are gone on 1.30 (compile error) — use the `elapsedTime` parameter.
- **Remedy**: override `OnCEIterate` and call `super.OnCEIterate(currentTime, elapsedTime)` first. Also on 1.30 `SpawnParticles` snaps to `SurfaceRoadY` and only lowers particles above it (`effectarea.c:412-417`; 1.29 used `SurfaceY` unconditionally).

### 7. `Surface.GetStepsParticleID` / `EffWheelSmoke.SetSurface` are obsolete — and `SetSurface` is no longer called
- **Cause**: `[Obsolete]` at `4_world\static\surface.c:64, 70` and `4_world\entities\effects\wheelsmoke.c:32`. Vanilla now passes a `SurfaceInfo` to `SelectFromSurface` (`4_world\entities\vehicles\components\vehiclevfxcomponent.c:177-185, 213`; `4_world\entities\effects\vehicledust.c:25-36`); no vanilla script calls `SetSurface` any more (it was called from `CarScript`, 1.29: `scripts\4_world\entities\vehicles\carscript.c:1172, 1182`).
- **Remedy**: `SurfaceInfo.GetByName(...).GetStepParticleId()` and override `SelectFromSurface(SurfaceInfo)`. VFX detail in `dayz-particles`.

### 8. Oil pit only deals damage in `BURNING` state
- **Cause**: random ignition delay `Math.RandomInt(3, 10)` → 3–9 s, max exclusive (`4_world\classes\contaminatedarea\oilpitarea.c:10-11, 52-60`; `1_core\proto\enmath.c:24-37`). `HeatDamage` 20.0 per tick, tick > 1 s (`4_world\entities\scriptedentities\triggers\oilpittrigger.c:7-8, 53-71`).
- **Remedy**: wait for `AddState(EOilPitState.BURNING)` or the trigger seems inert.

### 9. Shadow is not a raycast every 5 s
- **Cause**: `ENVIRO_TICK_SHADOW_RC_CHECK = 30` (`3_game\constants.c:760`). Capsule 50 m × 0.05 m towards `-GetSunOrMoonDirection()` (`4_world\classes\environment\environment.c:679-719`). Interior, roof or `Car` force shadow true (`:667-676`).
- **Remedy**: do not copy "5 seconds" from third-party notes; test with the 30 s timer.

### 10. `Weather.GetNoiseReductionByWeather()` does not govern AI any more
- **Cause**: `GetNoiseReductionByWeather()` is `[Obsolete("Use Weather.GetNoiseReductionByWeatherEx instead!")]` (`3_game\weather.c:454-467`), and its replacement `GetNoiseReductionByWeatherEx(notnull Object)` is itself `[Obsolete("1.30: AI noise dampening is now done natively via AIParams config parameters. Kept for HUD and old mods.")]` (`weather.c:407-420`). No vanilla script calls either one; the noise calls pass no weather multiplier (`3_game\dayzgame.c:3618, 3793, 3833`). Dampening is native, from `AIParams` `rain/snowfall/sea/fog/wind/sandstormToNoiseMultiplier` (`P:\DZ\data\aiconfigs\config.cpp:15-20`; `sandstormToNoiseMultiplier = 10`).
- **Before 1.30**: vanilla multiplied `AddNoise*` by `GetNoiseReductionByWeather()` (1.29: `scripts\3_game\dayzgame.c:3517, 3692, 3730`) and `AIParams` had only `rainToNoiseMultiplier`/`seaToNoiseMultiplier`. A 1.29 mod that `modded` these functions to change what infected hear compiles but does nothing on 1.30. [reported: bzed]
- **Remedy**: AI → edit `AIParams` in config. HUD read-out only → `GetNoiseReductionByWeatherEx(object)` (obsolete, still includes the sandstorm term via `GetIntensityForPlayer`, `weather.c:432-449`). Sister skill: `dayz-ai-patterns`.

## How to verify

1. **Offline linter**: `python <KNOWLEDGE_PACK>/tools/dayz-script-validator/scripts/script_validator.py <addon_root>`. Gate `len(errors) == 0`.
2. **In-game ladder** (DayZ-MCP or Diag `-filePatching`):
   - **Weather spawn**: on server, `GetGame().GetWeather().GetSandstorm().Start(5, false)`. Client must receive PPE `REQ_SANDSTORMEFFECT` (`P:\scripts\4_world\systems\sandstorm\scriptedsandstormcontroller.c:310-327`).
   - **Shelter**: enter a building with `IsSoundInsideBuilding`; intensity → 0 in 1.5 s. Mount a motorbike: intensity must NOT go to 0. Enter a `Car`: yes.
   - **PPE**: with a dust mask (e.g. `NioshFaceMask`), no cough or silicosis; with glasses, no squint.
   - **Heat**: sustained high heat comfort must raise `ThermalBias` and activate `MDF_HEAT_STROKE1..4`.
   - **WashHead**: reduces 10 `EYES_IRRITATION` (1.6 from a bottle), lowers bias by 0.175 and then blocks every `ThermalBiasHandler.Add` (both directions) for 30 s (`P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actionwashheadwettingclothesbase.c:46-56`, `actionwashheaditemcontinuous.c:41`, `P:\scripts\4_world\classes\thermalbiashandler.c:40-48`).
   - **Oil pit**: wait 3–9 s; then 20 `HeatDamage` per tick (> 1 s) and `OIL_FIRE1`/`OIL_FIRE2` particles.
3. **RPT** (strings verified in source): `Could not get players sandstorm data!` (`scriptedsandstormcontroller.c:65, 69`), `---- failed to load ThermalBiasHandler, read fail  ----` (`P:\scripts\4_world\entities\manbase\playerbase.c:7522`), and `SCRIPT (W)` obsolete warnings for `OnCEUpdate` / `GetNoiseReductionByWeather` / `SetSurface` in a ported mod.

## DayZ 1.30 changes (porting a ≤1.29 hazard / effect-area mod)

None of the sandstorm, thermal-bias, silicosis, eye-irritation, shadow/sun or oil-pit APIs exist on 1.29 (checked: no `GetSandstorm`, `ThermalBias`, `IsInShadow`, `ENVIRO_TICK_SHADOW_RC_CHECK` in `O:\…\scripts`). What breaks or silently changes in an old mod:

1. **Silent**: `OnCEUpdate()` overrides never run → move the body to `OnCEIterate(float currentTime, float elapsedTime)` (invariant 6). `ProcessVariables()` → `ProcessVariables(float elapsedTime)` and `m_ElapsedSinceLastUpdate`/`m_LastUpdatedTime` are compile errors (api-delta §3, §5).
2. **Silent**: `modded` `Weather.GetNoiseReductionByWeather` / `NoiseAIEvaluate.GetNoiseReduction` no longer affect AI (invariant 10).
3. **Silent**: `EffWheelSmoke.SetSurface` overrides never run (invariant 7).
4. **Silent**: heat damage trimmed in `HeatComfortMdfr` no longer matters (invariant 3).
5. **Behaviour**: `EffectArea.SpawnParticles` snaps to `SurfaceRoadY` (invariant 6); `EEffectAreaType` gains `OIL_PIT = 8` (`P:\scripts\4_world\classes\contaminatedarea\effectarea.c:10-16`).
6. **Config**: `dust_particle_breath`/`dust_particle_eyes` already existed in 1.29 item configs but nothing read them; on 1.30 they are live and several vanilla values changed (config-delta, e.g. `AviatorGlasses` eyes 0.5 → 0.4). Re-check custom masks/glasses (invariant 5).
7. **Persistence**: 1.30 `PlayerBase` appends `ThermalBiasHandler` to the player stream with no `version` gate (`playerbase.c:7395, 7520`; `thermalbiashandler.c:72-78`). A `modded PlayerBase.OnStoreSave/Load` must keep `super` order. Effect on characters saved by 1.29 not measured — see `dayz-persistence`.
8. Do not wrap 1.30 code in `#ifdef DAYZ_1_30` (policy rule 1).

### DayZ 1.30.164200 changes

Patch 1.30.164200 (2026-10-21). No sandstorm, thermal-bias, heat-stroke, silicosis, eye-irritation, oil-pit or effect-area script changed (script delta file list). What did:

1. **Irrigation tunnel water no longer gives cholera** [reported: BI 1.30.164200 notes]. The data fix (`dayz-130-migration` config-delta §2.8): 38 irrigation-tunnel / water-stream models use the roadway surface `clean_water_int` (`liquidType=4194304`, `LIQUID_CLEANWATER`, `P:\DZ\surfaces\config.cpp:1803-1809`) instead of `still_water_int` (`liquidType=512`, `LIQUID_WATER`, `:1775-1781`). Drinking a `LIQUID_CLEANWATER` surface counts as `ENVIRO_WELL`, any other liquid as `ENVIRO_POND` (`P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actiondrinkpondcontinuous.c:78-86`), and only `ENVIRO_POND` adds `CHOLERA` (`P:\scripts\4_world\entities\manbase\playerbase.c:7696-7704`). Water in a custom hazard map follows the same rule.
2. **Action order on pots.** `Pot` now removes and re-adds Wash Hands / Wash Head / Wet Clothing after `ActionDrinkCookingPot` (`P:\scripts\4_world\entities\itembase\edible_base\bottle_base\pot.c:137-144`; `cauldron.c:113-117` has the same order), and `ActionDrinkCan` moved up in `ActionConstructor` (`actionconstructor.c:208`), so Drink is offered before washing (BI: "'Wash Face' action was incorrectly offered first instead of 'Drink'", DZEXP-164). The wash actions themselves (eye irritation, thermal bias; see How to verify) are unchanged. [reported: BI 1.30.164200 notes, known issue] no audio when performing Wash Head or Wet Clothes.
3. **`vector.Cross` removed.** The shadow capsule in `Environment.CheckShadowPresence` now builds its axes with `(forward * up)` / `(up * right)` (`P:\scripts\4_world\classes\environment\environment.c:696-697`; `vector * vector` is the cross product). Same behaviour; a mod that copied this function with `.Cross(` no longer compiles.
4. **Interior detection** [reported: BI 1.30.164200 notes, known issue]: "Incorrect interior/exterior detection in some buildings". Invariant 2 (shelter = roof **and** `IsSoundInsideBuilding`) depends on that detection: if a vanilla building does not shelter from the storm on 164200, suspect this before your mod. Not measured.
5. **Night vision in a storm** (client PPE): an optic in day mode underground during a sandstorm now gets its own profile `NV_UNDERGROUND_SANDSTORM_DAYTIME_OPTICS = 18`; `NVTypes` values after `NV_OPTICS_UNDERGROUND` moved (+1/+2). Surface sandstorm optics/goggles are unchanged. Detail: `dayz-underground` §Night vision underground.
6. **Badlands data on `P:\`.** The 1.30.164200 unpack added the Nasdara CE, lighting, navmesh and `*_takistan` data (`references/dust-oilpit-nasdara.md` §Nasdara); `worlds_nasdara.ebo` stays encrypted. Vanilla navmeshes are now format v9 (`dayz-ai-patterns` §Navmesh).

## Known issues in 1.30.164078, still present in 1.30.164200 unless noted (static reading of the source)

1. **Nasdara data**: on 1.30 Exp 164014 the Nasdara data PBO was a 6-file stub without `.wrp`/navmesh. Resolved for `P:\` by the 1.30.164200 unpack: the data PBO (8 files), CE, navmesh and the lighting file the scripts reference (`P:\DZ\worlds\nasdara\data\lighting\lighting_nasdara.txt`, `P:\scripts\3_game\worldlighting.c:6`) are there; only the world config `worlds_nasdara.ebo` is encrypted (`references/dust-oilpit-nasdara.md`).
2. **Wash Head with frozen bottle** [CHANGELOG 1.30, known issue]: `ActionWashHeadItemContinuous.ActionCondition` only rejects gasoline, no `IsFrozen()` test (`actionwashheaditemcontinuous.c:32-38`, unchanged on 164078).
3. **Sickness icons**: `HeatStrokePhase1` + `HasDisease` counter imbalance (invariant 4).
4. **No sun outside Nasdara**: `ChernarusPlusData.Init` does not fill `m_TemperatureUnderSunModifier` (`P:\scripts\4_world\classes\worlds\chernarusplus.c:31-80`; `EnochData`/`SakhalData` do not either — only `nasdara.c` writes the map), and `Environment` adds the sun term only if the current daytime is a key of that map (`environment.c:868-872`, `P:\scripts\3_game\worlddata.c:645-654`). A custom map gets sun exposure only if its `WorldData.Init` fills the map like `nasdara.c:67-73`.
5. **OilPitTrigger fire particles**: the ignition branch of `UpdateState` (`P:\scripts\4_world\entities\scriptedentities\triggers\oilpittrigger.c:92-116`) has no headless guard; only the `DeferredInit` call is guarded by `IsHeadlessOrDedicatedServer` (`:37`), the `OnVariablesSynchronized` path (`:82-88`) is not.
6. **HeatStroke water drain**: `WATER_DRAIN_FROM_FEVER_ST1..3` are declared `const int` with values 0.35/0.5/0.75 (`heatstroke.c:7-9`); with integer conversion the per-tick drain is probably 0. Not measured.
7. **`ScriptedSandstormController.OnStart`** calls `super.OnStart(calledByWeather, continuedSession)` — arguments swapped versus the base signature `OnStart(bool continuedSession, bool calledByWeather)` (`scriptedsandstormcontroller.c:310-312`, `P:\scripts\3_game\sandstorm.c:90`). Runtime effect not measured.

## Cite-then-verify

`[EXACT]` citations point at `P:\` (1.30.164200). Before trusting any line number, grep `P:\scripts` (policy rule 3). Older third-party notes about this content shifted many citations (e.g. "5 s" shadow tick); never copy an `[EXACT]` block from them.

## References index

- [references/sandstorm-shelter.md](references/sandstorm-shelter.md): native API, `ScriptedSandstormController`, cfgWorlds, JSON, Nasdara `CalculateWind`.
- [references/thermal-heatstroke.md](references/thermal-heatstroke.md): `ThermalBiasHandler`, `HeatComfortMdfr`, 4 phases, WashHead, shadow, motorbike/boat.
- [references/ppe-agents.md](references/ppe-agents.md): agents, modifiers, symptoms, PPE config and item category, cough.
- [references/dust-oilpit-nasdara.md](references/dust-oilpit-nasdara.md): `SurfaceInfo`, roofs, oil pit, Nasdara, animals.
- [references/sources.md](references/sources.md): inventory of re-opened `P:\` files.

## What could not be verified

1. **`SandstormController` C++**: progression, magnitude and `GetPointOnEdge` live in the exe.
2. **Nasdara world config** (`worlds_nasdara.ebo`, with the `.wrp`): encrypted. CE, animals (`P:\DZ\animals_takistan\`) and AI templates are on `P:\` since the 1.30.164200 unpack.
3. **`.p3d` / `.ptc` / `.edds` binaries**: existence only (e.g. `P:\graphics\particles\sandstorm\sandstorm.ptc`); emitter curves [UNVERIFIED].
4. **`Object.IsInfluencedBySandstorm()` / `OnClientSandstormEnter/Leave()`** (`P:\scripts\3_game\entities\object.c:1375-1381`): declared only; no vanilla script overrides or calls them, presumably engine-driven. Not measured.
5. **Runtime**: no tick was measured in-game, neither on 1.30 Exp nor on stable.
