# ThermalBias, heat stroke, shadow, and cooling

Source: DayZ 1.30 stable `1.30.164078`; paths relative to `P:\scripts\` unless they start with `P:\`. None of these APIs exist on 1.29.

## ThermalBiasHandler

Server-only instance in `PlayerBase` (field `4_world\entities\manbase\playerbase.c:339`, created inside `if (g_Game.IsServer())` at `:426`; getter `GetThermalBiasHandler()` `:8158-8161`). Backed by stat `EPlayerStats_current.THERMAL_BIAS` (`4_world\classes\thermalbiashandler.c:13-19`). `Add` does nothing — in either direction — while `m_TemporaryResistanceTime > 0` (`thermalbiashandler.c:40-48`); `Set` is not blocked (`:50-55`). Handler persistence:

```c
// [EXACT] P:\scripts\4_world\classes\thermalbiashandler.c:67
    void OnStoreSave(ParamsWriteContext ctx)
    {
        ctx.Write(m_TemporaryResistanceTime);
    }

    bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!ctx.Read(m_TemporaryResistanceTime))
            return false;

        return true;
    }
```

Callers: `PlayerBase.OnStoreSave` writes it last, after the arrow manager (`playerbase.c:7395`); `PlayerBase.OnStoreLoad` reads it last with **no** `version` gate and returns `false` on a read failure (`playerbase.c:7520-7524`). Before 1.30 the player stream ended with `arrowManager.Save(ctx)` (1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\4_world\entities\manbase\playerbase.c:7074`). If a `modded class PlayerBase` appends its own data, it MUST call `super.OnStoreSave` / `super.OnStoreLoad` first and keep the order. How 1.29-saved characters load on 1.30 was not measured. Contract skill: `dayz-persistence`.

## HeatComfortMdfr → bias

1.30: the **high**-heat branch of `HeatComfortMdfr.OnTick` feeds the bias instead of damaging health; **cold** still deducts health (`4_world\classes\playermodifiers\modifiers\heatcomfortmdfr.c:62-71`). Before 1.30: the same modifier subtracted health above `WATER_LOSS_THRESHOLD_HC_PLUS_HIGH` (1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\4_world\classes\playermodifiers\modifiers\heatcomfortmdfr.c:51-56, 73-76`).

```c
// [EXACT] P:\scripts\4_world\classes\playermodifiers\modifiers\heatcomfortmdfr.c:74
		//! Thermal Bias value feeding (based on HeatStroke stages)	
		if (heatComfortValue > PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_EMPTY)
		{
			thermalBiasHandler.Add(PlayerConstants.THERMAL_BIAS_POSITIVE_HC_HIGH_INCREMENT * deltaT);
		}
		else if (heatComfortValue > PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_CRITICAL)
		{
			thermalBiasHandler.Add(PlayerConstants.THERMAL_BIAS_POSITIVE_HC_MEDIUM_INCREMENT * deltaT);
		}
```

Above `THRESHOLD_HEAT_COMFORT_PLUS_WARNING` (0.15) the bias also grows with low water: below `SL_WATER_NORMAL` (1000 on 1.30, was 800 on 1.29 per api-delta §8) by LOW/MEDIUM/HIGH increments chosen by the current stage, between `SL_WATER_NORMAL` and `SL_WATER_HIGH` (3500) by LOW below stage 2 or `-DECREMENT` otherwise; below the warning threshold it decays and is clamped to 0 with `Set(0.0)` (`heatcomfortmdfr.c:84-111`; thresholds `3_game\playerconstants.c:49-50, 89-91`).

Constants (`P:\scripts\3_game\playerconstants.c:203-210`):

| Symbol | Value |
|---|---|
| `THERMAL_BIAS_WASH_HEAD_DECREMENT` | 0.175 |
| `THERMAL_BIAS_WASH_HEAD_TEMPORARY_RESISTANCE_TIME` | 30.0 s |
| `THERMAL_BIAS_POSITIVE_HC_DECREMENT` | 0.0001 |
| `THERMAL_BIAS_POSITIVE_HC_LOW_INCREMENT` | 0.00022 |
| `THERMAL_BIAS_POSITIVE_HC_MEDIUM_INCREMENT` | 0.00032 |
| `THERMAL_BIAS_POSITIVE_HC_HIGH_INCREMENT` | 0.00050 |

## HeatStroke — 4 simultaneous phases per threshold

```c
// [EXACT] P:\scripts\4_world\classes\playermodifiers\modifiers\diseases\heatstroke.c:3
	const int NUMBER_OF_STAGES = 4;
	const float STAGE_THRESHOLDS[NUMBER_OF_STAGES] = {0.0, 0.4, 0.6, 0.85};
```

IDs: `MDF_HEAT_STROKE1..4` (`4_world\classes\playermodifiers\emodifiers.c:66-69`). Sync: `MODIFIER_SYNC_HEAT_STROKE` (`4_world\classes\playermodifiers\modifiersmanager.c:12`). Line refs in the table are in `heatstroke.c`.

| Phase | Active if bias | Effects (static reading) |
|---|---|---|
| 1 | `> 0.0` | Water drain `WATER_DRAIN_FROM_FEVER_ST1` per tick (`:77-81`) — see the `int` note below. `OnActivate` empty; `OnDeactivate` → `DecreaseDiseaseCount()` (`:68-75`). |
| 2 | `> 0.4` | `SYMPTOM_FEVERBLUR`, stamina `DISEASE_PNEUMONIA`, `IncreaseDiseaseCount`, water drain ST2, random `SYMPTOM_HOT` complaints (`:110-136`). |
| 3 | `> 0.6` | Vanilla comment: stays active alongside phase 4 (`:139`). Water drain ST3, random vomiting −70 water / −55 energy, `VOMIT_EXHAUSTION` (`:186-230`). Deactivates with `<`, not `<=` (`:164-167`). |
| 4 | `> 0.85` | Health damage `Remap(0.85..1.0 → 0.01..0.35) * deltaT` (`:26-28, 307-309`), `SYMPTOM_FAINT` vignette cycle, unconsciousness 5–10 s every 60–120 s with shock set to 25 (`:18-22, 265-387`). The uncon trigger also needs shock > 90 and no pending faint fade-out (`:24, 369-370`); its `thermalBias > 0.75` test is always true while phase 4 is active. |

`int` note: `WATER_DRAIN_FROM_FEVER_ST1..3` are declared `protected static const int` with values 0.35 / 0.5 / 0.75 (`heatstroke.c:7-9`). With integer conversion the effective drain is probably 0 — not measured; do not quote "0.35/tick" as a gameplay value.

## WashHead and wet clothing

`ActionWashHeadWettingClothesBase.OnFinishProgressServer` (`4_world\classes\useractionscomponent\actions\continuous\actionwashheadwettingclothesbase.c:49-57`):

- `ReduceAgent(EYES_IRRITATION, 10)` (bottle override 1.6 in `actionwashheaditemcontinuous.c:41`).
- `ThermalBiasHandler.Add(-THERMAL_BIAS_WASH_HEAD_DECREMENT)` — ignored if a previous wash's resistance is still running (`thermalbiashandler.c:42-43`).
- `SetTemporaryResistance(30 s)`: for 30 s every `Add`, heating **and** cooling, is ignored.
- Wets slots according to the container vs surface map (`:6-44`).
- Interrupted action: proportional reduction in `OnEndServer` (`:59-70`).

`ActionWashHeadItemContinuous.ActionCondition` accepts any liquid ≠ gasoline, no `IsFrozen()` test (`actionwashheaditemcontinuous.c:32-38`); it also consumes 50 quantity (`:48`). [CHANGELOG 1.30, known issue]: frozen bottles.

`ActionWetClothingInHandsBase` wets the item in hands and subtracts bias proportional to the wet gain (`actionwetclothinginhandsbase.c:44-45`) — also blocked during the 30 s resistance.

## Shadow and sun exposure

Timer: `GameConstants.ENVIRO_TICK_SHADOW_RC_CHECK = 30` (`3_game\constants.c:760`). **Not** 5 s.

`IsInShadow()` → `m_ShadowPresence` (`4_world\classes\environment\environment.c:519-522`). Update (`:405-414`): if `IsInsideBuilding()` → true; otherwise `CheckShadowPresence`.

`CheckShadowPresence` (`:665-720`): interior/roof/`Car` → true. Otherwise, capsule 50 m × radius 0.05 toward `-g_Game.GetWorld().GetSunOrMoonDirection()` (`3_game\global\world.c:52`), layers ITEM_LARGE|BUILDING|VEHICLE|TERRAIN|ROADWAY, `DayZPhysics.CapsuleOverlapBullet`.

If `!IsInShadow()` **and** the current daytime is a key of `m_TemperatureUnderSunModifier`, temperature += `GetSunEffectOnTemperature(m_DayTime) * HeadGearProtectionAgainstSun()` (`environment.c:868-872`; `WorldDataDaytime.IsInRange`, `3_game\worlddata.c:645-654`). `GetSunEffectOnTemperature` scales by overcast (`worlddata.c:355-360`).

**Nasdara** solar table (`4_world\classes\worlds\nasdara.c:67-73`): DAWN 2, MORNING 6, NOON 8, AFTERNOON 4, EVENING 2, DUSK 0, NIGHT 0. `ChernarusPlusData.Init` does not write the map (`4_world\classes\worlds\chernarusplus.c:31-80`), nor do Enoch/Sakhal → no sun term on those maps.

`HeadGearProtectionAgainstSun` (`environment.c:2037-2064`): with a HEADGEAR item, `isolation * HC_EXPOSED_HEAD_TO_SUN + 0.4` (`HC_EXPOSED_HEAD_TO_SUN = 1.2`, `:79`); but the loop returns `HC_EXPOSED_HEAD_TO_SUN` as soon as it meets any clothing attachment whose first slot is not HEADGEAR (`:2048-2057`), so headgear only reduces sun heat when no other clothing is worn [static reading; likely unintended; not measured].

## Motorbike and boat (wind)

`DetermineHeatcomfortBehavior` (`environment.c:575-626`): `Car` with engine on → heat comfort driven to 0; `Motorbike` if speed > 20; `Boat` if speed > 15 (`3_game\constants.c:792-799`; categories `EEnvironmentHeatcomfortBehaviorCategory.MOTORBIKE/BOAT` are new in 1.30). `SetHeatcomfortDirectly` subtracts comfort with `Easing.EaseInCubic` (`environment.c:1369-1445`). `MotorbikeScript.GetHeatComfortOverride()` / `BoatScript.GetHeatComfortOverride()` return `m_HeatComfortOverride`, initialised to 0.0 (`4_world\entities\vehicles\motorbikescript.c:135, 164-167`; `4_world\entities\vehicles\boatscript.c:108, 256-259`). The boat branch uses `ENVIRO_HEATCOMFORT_MOTORBIKE_TARGET_MAX`, not the BOAT constant (`environment.c:1431`); both are 0.8, so no difference today.

Porting note: mods overriding `Environment` wet/dry helpers break at compile time on 1.30 (`ApplyWetnessToItem`, `ApplyDrynessToItemEx` signatures changed, api-delta §3).
