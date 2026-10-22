# Actions, HUD, Lights, and Sounds in Motorbikes

> Target: DayZ 1.30 stable (1.30.164200). Version labels: `../../_shared/dayz-version-policy.md`. Script, `.agf`,
> `.asi` and `.ast` line numbers were read on 1.30 Exp 164014 and are unchanged on stable (identical line counts,
> sampled cites re-read 2026-10-15); `config.cpp` cites point at stable `P:\DZ` / `P:\bin`. Re-pointed to 1.30.164200 on
> 2026-10-21; that patch changed the bike lights, horn volumes and wheel-VFX cooldown (§3, §4) and added a detach lock
> (`../SKILL.md` §DayZ 1.30.164200 changes).

This document details all user actions specific to motorbikes in DayZ 1.30, the internal structure of `MotorBikeHud`, the lights/horn/VFX components, and the exact acoustic configuration extracted from `P:\DZ\sounds\hpp\config.cpp`.

---

## 1. Motorbike User Actions

### `ActionStartEngineMotorbike`
- **File**: `P:\scripts\4_World\Classes\UserActionsComponent\Actions\Continuous\Vehicles\ActionStartEngineMotorBike.c:9` (in-game `scripts\4_World\...\ActionStartEngineMotorBike.c`).
- **Type**: Continuous action (`ActionContinuousBase`), uses `ActionStartEngineMotorbikeCB` with `CAContinuousTime(UATimeSpent.START_ENGINE)`.
- **Condition (`ActionCondition`)**:
  - Player in `HumanCommandVehicle` (`line 28`).
  - Vehicle is `MotorbikeScript` and its engine is off (`if (!vehicle || vehicle.EngineIsOn())` returns `false`, `line 33`).
  - The player is the driver (`vehicle.CrewDriver() == player`, `line 36`).
- **Execution**:
  - In `OnExecute` (`lines 66-75`): invokes `vehicle.OnIgnition()` (`line 74`), which plays the kickstart sound with `HandleEngineSound(MotorbikeEngineSoundState.STARTING)` (`MotorbikeScript.c:1613-1635`, called at `1634`).
  - In `OnFinishProgress` (`lines 39-64`): invokes `vehicle.EngineStart()` (`line 63`). According to the comment at `line 51`, validation is performed by C++ via the `Motorbike.OnBeforeEngineStart()` callback.
- **Registration**: In `PlayerBase.c:1709` (`AddAction(ActionStartEngineMotorbike, InputActionMap);`) and in `ActionConstructor.c:223`.

### `ActionStopEngineMotorbike`
- **File**: `P:\scripts\4_World\Classes\UserActionsComponent\Actions\SingleUse\Vehicles\ActionStopEngineMotorBike.c:1`.
- **Type**: Single-use action (`ActionSingleUseBase`).
- **Condition**: Driver on `MotorbikeScript`, engine running (`vehicle.EngineIsOn()`), and absolute speed $\le 8$ km/h (`if (vehicle.GetSpeedometerAbsolute() > 8)` returns `false`, `line 29`). Cannot be manually turned off at high speed.
- **Registration**: In `PlayerBase.c:1712` and in `ActionConstructor.c:76`.

### `ActionPickUpMotorbike`
- **File**: `P:\scripts\4_World\Classes\UserActionsComponent\Actions\Interact\Vehicles\ActionPickUpMotorBike.c:1`.
- **Type**: Direct interaction (`ActionInteractBase`, `IsInstant() = true`).
- **Condition**: The object is `Motorbike`, player is not carrying a heavy item in hands (`!IsHeavyBehaviour()`), cursor does not point to a seat (`trans.CrewPositionIndex(componentIndex) < 0`), and the bike is fallen over and eligible to be picked up (`trans.CanPickUp()`, `line 64`).
- **Execution**: Invokes `trans.PickUp();` (`line 74`).
- **Registration**: In `MotorbikeScript.c:367`, on the side fairings of `Motorbike_02` (`P:\scripts\4_World\Entities\Core\Inherited\InventoryItem.c:499, 511, 539, 551`), and in `ActionConstructor.c:348`.

### `ActionAnimateMotorbikeSeat`
- **File**: `P:\scripts\4_World\Classes\UserActionsComponent\Actions\Interact\Vehicles\ActionAnimateMotorbikeSeat.c:1`.
- **Type**: `ActionAnimateSeats`.
- **Condition**: Seat without driver (`!bike.CrewMember(VEHICLESEAT_DRIVER)`), player closer than 1 meter, and selection belongs to animation source `"SeatDriver"`.
- **Registration**: In `Motorbike_01.c:101` (`AddAction(ActionAnimateMotorbikeSeat);`) and in `ActionConstructor.c:313`.

### Motorbike Repair Actions
| Action | File | Required tool | Target zone | Registration in ActionConstructor |
|---|---|---|---|---|
| `ActionRepairMotorbikeChassisWithBlowtorch` | `ActionRepairMotorbikeChassisWithBlowtorch.c:9` | `Blowtorch` (blowtorch with gas) | Chassis: damaged zone between `WORN` and `RUINED` (inherited condition, `ActionRepairMotorbikeChassis.c:31-47`) | Yes (`ActionConstructor.c:259`) |
| `ActionRepairMotorbikeChassis` | `ActionRepairMotorbikeChassis.c:1` | Base without vanilla tool | Chassis | Yes (`ActionConstructor.c:255`) |
| `ActionRepairMotorbikeEngineWithBlowtorch` | `ActionRepairMotorbikeEngineWithBlowtorch.c:9` | `Blowtorch` | Engine (`"Engine"`) | Yes (`ActionConstructor.c:260`) |
| `ActionRepairMotorbikeEngine` | `ActionRepairMotorbikeEngine.c:1` | Base (orphaned) | Engine | **NO** (vanilla omission in 1.30) |
| `ActionRepairMotorbikePartWithBlowTorch` | `ActionRepairMotorbikePartWithBlowTorch.c:9` | `Blowtorch` | Parts / fairings (`MotorbikePart`) | Yes (`ActionConstructor.c:253`) |
| `ActionRepairMotorbikePart` | `ActionRepairMotorbikePart.c:12` | Base | Parts / fairings | Yes (`ActionConstructor.c:252`) |

---

## 2. Vehicle HUD: `MotorBikeHud`

- **File**: `P:\scripts\5_Mission\GUI\Vehicles\MotorBikeHud.c:1-248` (in-game `scripts\5_Mission\GUI\Vehicles\MotorBikeHud.c`).
- **Inheritance**: `class MotorbikeHud : VehicleHudBase`.
- **Reused base layout**: `"gui/layouts/day_z_hud_cars.layout"` (`line 36`).
- **Entity binding**: `MotorbikeScript.c:316-319` returns `"VehicleTypeMotorbike"`, registered in `P:\scripts\5_Mission\GUI\IngameHud.c:183` (`m_VehicleHudMap.Insert("VehicleTypeMotorbike", motorbikeHud)`).
- **Active instrumentation (`RefreshVehicleHud`)**:
  - **RPM and Tachometer**: Reads `EngineGetRPM() / EngineGetRPMMax()` (`line 105`). The needle rotates with `rpm_value * 270 - 130` (`line 109`). Redline is calculated with `EngineGetRPMRedline() / EngineGetRPMMax()` (`line 106`).
  - **Speedometer**: Absolute speed divided by 200 for needle (`speed_value * 260 - 130`, `line 110`) and numeric text via `Math.AbsInt(GetSpeedometer()).ToString()` (`line 111`).
  - **Gears**: `UpdateGear()` queries `GetGear()` (`lines 217-237`). On manual transmission (`CarGearboxType.MANUAL`, `line 229`) it updates `Current`, `Prev`, and `Next` (`lines 231-236`) with the R/N/1-8 table built in `lines 77-88`.
  - **Fuel level**: Reads `GetFluidFraction(MotorbikeFluid.FUEL) * 260 - 130` (`line 204`).
  - **Engine light**: Queries `GetHealthLevel("Engine")`. Blinks if `HasEngineZoneReceivedHit()` is active (`lines 118-137`).
  - **Wheel lock**: Illuminates if `WheelIsAnyLocked()` returns `true` (`line 197`).
- **Visual differences compared to cars**:
  - Handbrake light **hidden**: `m_VehicleHandBrakeLight.Show(false);` (`line 196`).
  - Temperature indicator **hidden**: `m_VehicleTemperatureIndicator.Show(false);` because `IsVitalRadiator()` returns `false` (`lines 191-194`).

---

## 3. Modular Vehicle Components

### `VehicleLightsComponent` (`VehicleLightsComponent.c:1-190`)
- Allows registering rvmat material selections associated with light states. `MotorbikeScript` registers defaults
  (`MotorbikeScript.c:114-122`); the id is the index in the bike's `hiddenSelections[]`, the string is the config key
  whose rvmat is applied (`VehicleLightsComponent.c:66-80`):
  ```c
  m_LightsComponent.RegisterSelection("FrontOn", {new VehicleLightSelectionData(0, "frontReflectorMatOn")});
  m_LightsComponent.RegisterSelection("BrakeOn", {new VehicleLightSelectionData(1, "brakeReflectorMatOn")});
  ```
  1.30.164200: each vanilla bike overrides those defaults with its own slots — Motorbike_01 `FrontOn/Off` 3,
  `BrakeOn/Off` 2 (`Motorbike_01.c:19-22`); Motorbike_02 `FrontOn/Off` 4, `BrakeOn/Off` 6 (`Motorbike_02.c:20-23`; was 2
  on 164078) — and the config now declares `frontReflectorMatOn/Off`, `brakeReflectorMatOn/Off`
  (`P:\DZ\vehicles\singletrack\config.cpp:256-259`, `:1449-1452`). Both bikes dropped their
  `HasGangedTailAndBrakeLights()` = `true` override (base `false`, `MotorbikeScript.c:169-172`), and the server update now
  sets `"BrakeOff"` (was the unregistered `"BrakesOff"`, `:1248`). A custom bike re-registers all four keys with its own
  indices: recipe in `../SKILL.md` §DayZ 1.30.164200 changes.
- Supports dedicated light profiles (`VehicleLightProfileBase`):
  - `Motorbike_01LightProfileFront`: brightness 8 (2 before 1.30.164200), radius 30m, angle 50º, RGB color `(0.85, 0.85, 0.68)`.
  - `Motorbike_01LightProfileRear`: brake brightness 4, radius 6m, angle 120º, RGB color `(0.95, 0.16, 0.05)`.
  - `Motorbike_02LightProfileFront`: brightness 8 (3 before 1.30.164200), radius 65m, angle 65º, RGB color `(0.85, 0.85, 0.68)`.
  - Files: `P:\scripts\4_world\entities\scriptedlightbase\spotlightbase\vehiclelightprofiles\motorbike_01.c:1-34`, `motorbike_02.c:1-34`.
- Handlebar attachment:
  `int driveWheelIndex = GetBoneIndex("drivewheel");` and `m_LightsComponent.AttachLightOnObject("Front", GetMemoryPointPos(m_HeadLightPoint), vector.Zero, VehicleLightMode.SEGREGATED, driveWheelIndex);` (`MotorbikeScript.c:1122-1124`).

### `VehicleHornComponent` (`VehicleHornComponent.c:1-111`)
- Manages modes `OFF`, `SHORT`, `LONG`.
- On server triggers `GenerateHornAINoise` loading `NoiseCarHorn` from `config.cpp` (`strength = 30.0`).
- Synchronizes integer variable `m_HornComponent.m_Mode` over the network.

### `VehicleVFXComponent` (`VehicleVFXComponent.c:1-588`)
- Registers particles in `MotorbikeScript.c:174-205`:
  - `DUST_BEHIND` (`EffVehicleDust`) at `ConvertMemPointToPosition("engine")`.
  - `ENGINE_SMOKE` (`EffEngineSmoke`) at engine position.
  - `EXHAUST_SMOKE` (`EffExhaustSmoke`) oriented between `ptcExhaust_start` and `ptcExhaust_end`.
  - Wheel groups: `WHEEL_SMOKE` (`EffWheelSmoke`) and `WHEEL_CONTACT` (`EffWheelContact`), one effect per wheel, each
    with a restart cooldown `VehicleVFXDataParams(..., 2.0)` seconds (`MotorbikeScript.c:203-204`; 5.0 before 1.30.164200;
    the guard is `VehicleVFXData.CanPlay()`, `VehicleVFXComponent.c:43-50`).

---

## 4. Sound Configuration (`CfgSoundShaders` and `CfgSoundSets`)

Motorbike audio blocks are located in `P:\DZ\sounds\hpp\config.cpp` (line numbers below are class start lines on 1.30.164200, re-pointed 2026-10-21; first cataloged on Exp in `work\soundsets-motorbike-1.30.txt`). 1.30.164200 lowered the horn levels (below) and replaced `Motorbike_02_Engine_Ext_Broken_SoundSet` with `Motorbike_02_Engine_Ext_Broken_High_SoundSet` / `_Low_SoundSet` (`:386435`, `:386443`).

### Engine Shaders (RPM and Offload)
- `Motorbike_01_Engine_Ext_Rpm0_SoundShader` through `Rpm6_SoundShader` (`P:\DZ\sounds\hpp\config.cpp:317170-317313`): acceleration layers based on `rpm` and `thrust`.
- `Motorbike_01_Engine_Offload_Ext_Rpm1_SoundShader` through `Offload_Rpm6_SoundShader` (`config.cpp:317196-317326`): offload overrun layers upon releasing throttle.
- Equivalents in `Motorbike_02`: 8 acceleration layers (`Rpm0` to `Rpm7`) and 7 offload layers (`Offload_Rpm1` to `Offload_Rpm7`) (`config.cpp:318015-318197`).

### Primary SoundSets Registered in Script
- **Starting and stopping**:
  - `Motorbike_01_engine_start_SoundSet` (`config.cpp:386137`) -> sample `\DZ\sounds\vehicles\Motorbike_01\engine_start`.
  - `Motorbike_01_engine_failed_start_fuel_SoundSet` (`config.cpp:386147`) -> `\DZ\sounds\vehicles\Motorbike_01\engine_failed_start_fuel`.
  - `Motorbike_01_engine_stop_SoundSet` (`config.cpp:386157`) -> `\DZ\sounds\vehicles\Motorbike_01\engine_stop`.
  - `Motorbike_01_engine_stop_fuel_SoundSet` (`config.cpp:386166`) -> `\DZ\sounds\vehicles\Motorbike_01\engine_stop_fuel`.
- **Kickstart**:
  - `Motorbike_01_Kickstart_FoldOut_SoundSet` (`config.cpp:386229`) -> lever fold out.
  - `Motorbike_01_Starting_Engine_Off_SoundSet` (`config.cpp:386236`) -> false kick sound.
- **Horn**:
  - `Motorbike_01_Horn_SoundSet` (`config.cpp:386282`): continuous loop (`loop = 1`, `volumefactor = 0.5`, `:386288`; 0.6 before 1.30.164200).
  - `Motorbike_01_Horn_Short_SoundSet` (`config.cpp:386291`): short honk (`volumefactor = 0.4`, `:386297`; 0.8 before 1.30.164200; its shader `volume` 1.4 → 0.9, `:317961`). Motorbike_02 got the same values (`:386851`, `:386860`, `:318877`).
  - `Motorbike_01_Horn_Start_SoundSet` and `Motorbike_01_Horn_End_SoundSet` (`config.cpp:386299`, `:386307`).
- **Flip-up seat (`Motorbike_01`)**:
  - `Motorbike_01_Seat_FlipUp_SoundSet` and `Motorbike_01_Seat_FlipDown_SoundSet` (`Motorbike_01.c:116, 120`).
- **Shock Absorbers and Collision**:
  - `Motorbike_01_damper_front_SoundSet` and `Motorbike_01_damper_rear_SoundSet` (`config.cpp:386052, 386070`), both based on `Motorbike_01_damper_SoundShader` (`config.cpp:317627`), with 10 samples `motorbike_damper_01` through `motorbike_damper_10`.
  - `Motorbike_01_hit_character_SoundSet` and `Motorbike_01_hit_heavy_SoundSet` (`Motorbike_01.c:140, 147`).

## 5. Lights input: `UAToggleVehicleLights` versus `UAToggleHeadlight` (re-verified 2026-09-17, 2026-09-19 and on stable 1.30.164078 on 2026-10-15)

`ActionSwitchLights` (common to cars and motorbikes) uses the **vehicle** lights action, not the headtorch:

```c
// [EXACT] P:\scripts\4_World\Classes\UserActionsComponent\Actions\Interact\Vehicles\ActionSwitchLights.c:16-19
	override typename GetInputType()
	{
		return ToggleVehicleLightsActionInput;
	}
```

```c
// [EXACT] P:\scripts\4_World\Classes\UserActionsComponent\ActionInput.c:849-855
class ToggleVehicleLightsActionInput : DefaultActionInput
{
	ref ActionTarget targetNew;
	
	void ToggleVehicleLightsActionInput(PlayerBase player)
	{
		SetInput("UAToggleVehicleLights");
```

`ToggleLightsActionInput` still executes `SetInput("UAToggleHeadlight")` (`ActionInput.c:776-782`). Both names exist in `P:\bin\constants.xml:90-91`. A custom `inputs.xml` that only assigns `UAToggleHeadlight` will turn on the headtorch, not the motorbike headlight. Horn aliases `CarHornShortActionInput : VehicleHornShortActionInput {}` (`ActionInput.c:757-758`, under `//! backward compatibility (pre 1.30)`) are empty compatibility shells with 1.29; register `ActionVehicleHornShort`/`Long` (`ActionConstructor.c:340-341`).
