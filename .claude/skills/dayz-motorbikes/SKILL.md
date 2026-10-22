---
name: dayz-motorbikes
description: >
  Author, import, configure and debug single-track vehicles (motorbikes /
  motorcycles) in DayZ 1.30 (stable 1.30.164200). Covers MotorbikeScript,
  simulation = "motorbike", 2-wheel physics (Wheels block without Axles, Brakes
  split Front/Rear, lean/steering/tipover), kickstand, kickstart actions,
  dedicated 3rd person camera (ID 32), and the DayZ 1.30 modular vehicle refactor
  (VehicleLightsComponent, VehicleHornComponent, VehicleVFXComponent). Use when
  creating a new motorbike, porting two-wheel assets (Jawa, dirt bikes, scooters),
  migrating existing vehicle mods from 1.29 to 1.30, or fixing get-in/fall-over bugs.
  Do NOT use for 4-wheel cars, trucks or quads (see dayz-vehicles), aircraft
  (see dayz-aviation), raw 3D mesh processing (see dayz-model-pipeline), or
  custom player animation authoring (see dayz-animation-pipeline).
---

# DayZ Motorbikes (Single-Track Vehicles) & 1.30 Vehicle Refactor

> Target: DayZ 1.30 stable (1.30.164200). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.
>
> Written on 1.30 Exp 164014 (2026-09-16..10-01) and re-verified on stable 1.30.164078 on 2026-10-15: every script,
> `.agf`, `.asi` and `.ast` file cited here has the same line count on stable and the sampled cites hold, so script
> line numbers are unchanged; all `config.cpp` cites were renumbered to the `P:\DZ` extraction (different DeRap
> layout). In-game results from LFDucati/LFRider were measured on Exp and are tagged `(measured on 1.30 Exp 164014)`.
>
> Patch 1.30.164200 (2026-10-21): script cites renumbered; `singletrack\config.cpp` cites moved +4 (after `:255`) / +7
> (Motorbike_02 block) and `sounds\hpp\config.cpp` cites moved, all re-pointed to `P:\DZ`. What changed for bikes
> (lights, detach lock, `HasCrew`/`IsMoving`, camera clamp, wheel VFX, horn volume): §DayZ 1.30.164200 changes below.

> **Preflight: rider posture (added 2026-09-28, LFDucati T99; B1 measured on the same day) [EXACT except where marked].** Chosen solely by `GetAnimInstance()` -> `SetVehicleType` (`P:\scripts\4_World\Classes\UserActionsComponent\Actions\Interact\ActionGetInTransport.c:104-107`) -> fixed state and column in `Vehicles.agf` (five nodes require literal `VehicleType == 10 || VehicleType == 11`: `:1429, 1474, 1529, 1570, 2806`). Neither the model nor the config moves the hands: they are fixed by the handlebar pose clip, and there is no foot IK. GP clip-ons with MOTO1/MOTO2 leave the hands ~40 cm above: custom clips are required. Survivor Animations MOTORCYCLE (16) = car driver body (Offroad Hatchback) with IK hands, 1.29 format graph (crashes 1.30). Corrects `references/rider-animation.md:77`: 1.29 graphs are TEXT (`$AnimGraph 7`), not binary. Approach without touching the graph: registered child `.asi` + `SetAnimationInstanceByName`, like surrendering (`PlayerBase.c:2087-2088`, `DayZPlayerCfgBase.c:1532-1537`): **works with the motorbike stopped** on dedicated server and local client (measured 2026-09-28, LFDucati B1, LL-530; measured on 1.30 Exp 164014), **and stopped and moving with custom clips** (LFRider on the H2R, same day, LL-533..535; measured on 1.30 Exp 164014): a DayZATool (`ANIMSET5`) clip is within 0.4 mm of its FK; steering clips are distributed across ±45° of REAL wheel angle on all motorbikes (`Vehicles.agf:50`, `:1466`), not at the limit of each one; transitions are exported as full whole-body posture (as additive differences they collapse the skeleton). With observers and in edge cases, untested. Common mod and toolchain: LFRider (not published). Recipe, pitfalls, and the pose in four pieces: `references/rider-animation.md` §Per-bike pose. Detail: LFDucati research notes (not published).

DayZ 1.30 introduces full native engine support for single-track vehicles via the engine simulation type `simulation = "motorbike"` (`P:\bin\config.cpp:1486`). At the class architecture level, `class Motorbike: Transport` is a direct sibling of `Car` and `Boat` in native C++ (`P:\scripts\3_Game\Vehicles\Motorbike.c:30`, in-game `scripts\3_Game\Vehicles\Motorbike.c`), not a child of `Car`. In the game script layer, `MotorbikeScript` inherits directly from `Motorbike` (`P:\scripts\4_World\Entities\Vehicles\MotorbikeScript.c:53`, in-game `scripts\4_World\Entities\Vehicles\MotorbikeScript.c`), NOT from `CarScript`.

## Three-Layer Inheritance Hierarchy
1. **Engine Config Layer (`bin\config.cpp`)**:
   `Transport` -> `Motorbike` (`P:\bin\config.cpp:1481-1486`, with `simulation = "motorbike"`).
2. **3_Game Script Layer (`scripts\3_Game\Vehicles\`)**:
   - `TransportType` -> `MotorbikeType` (`P:\scripts\3_Game\Vehicles\Motorbike.c:16-18`).
   - `TransportOwnerState` -> `MotorbikeOwnerState` (`Motorbike.c:20-23`).
   - `TransportMove` -> `MotorbikeMove` (`Motorbike.c:25-27`).
   - `Transport` -> `Motorbike` (`Motorbike.c:30-370`).
3. **4_World Script Layer (`scripts\4_World\Entities\Vehicles\`)**:
   - `MotorbikeOwnerState` -> `MotorbikeScriptOwnerState` (`P:\scripts\4_World\Entities\Vehicles\MotorbikeScript.c:23-25`).
   - `MotorbikeMove` -> `MotorbikeScriptMove` (`MotorbikeScript.c:27-29`).
   - `Motorbike` -> `MotorbikeScript` (`MotorbikeScript.c:53-1780`).
   - `MotorbikeScript` -> `Motorbike_01_ColorBase` (`P:\scripts\4_World\Entities\Vehicles\InheritedMotorbikes\Motorbike_01.c:1-163`).
   - `MotorbikeScript` -> `Motorbike_02_ColorBase` (`P:\scripts\4_World\Entities\Vehicles\InheritedMotorbikes\Motorbike_02.c:1-137`).

A motorcycle in DayZ 1.30 has no relationship to the traditional four-wheel subsystem: it completely lacks the `Axles` hierarchy, does not use doors (`IsAreaAtDoorFree` always returns `true`), does not require a radiator or vital coolant circuit (`IsVitalRadiator()` returns `false`, `MotorbikeScript.c:321-324`), and the two vanilla motorbikes dispense with a battery because they override `NeedElectricitySourceDevice()` to `false` (`Motorbike_01.c:30-33`, `Motorbike_02.c:31-34`); the default in `Transport` is `true` (`Transport.c:830-833`) and `MotorbikeScript` does not change it. In return, the Enfusion engine simulates a dynamic two-wheel balance model with gyroscopic lean (`maxLeanAngle[]`), decoupled lever and pedal brakes, kickstand (`SetKickstand`), fall and destabilization (`FallOver`), and mandatory driver ejection on tipover (`CrewShouldEject`).

All technical evidence in this skill comes from code and configuration extraction (`source_verified`), first of 1.30 Exp 164014 and re-checked on stable 1.30.164078. Dynamic behaviour is runtime tested only where a line says so (LFDucati / LFRider, measured on 1.30 Exp 164014).

## Family Selector

| Use case | Tool and skill path | Key files |
|---|---|---|
| New vanilla-like motorbike (created from scratch or derived from Motorbike_01 / Motorbike_02) | **This skill (`dayz-motorbikes`)** | [references/forward-contract.md](references/forward-contract.md) and [references/config-contract.md](references/config-contract.md) |
| Motorbike imported from another game (ripped / asset store / Blender) | **This skill** + `dayz-model-pipeline` (mesh and LODs) | [references/forward-contract.md](references/forward-contract.md) and `dayz-model-pipeline/SKILL.md` |
| Traditional car, truck, quad, or boat | `dayz-vehicles` (sibling land vehicle skill) | `dayz-vehicles/SKILL.md` |
| Car mod migration from 1.29 to 1.30 (components, lights, horn, VFX) | [references/vehicle-1.30-refactor.md](references/vehicle-1.30-refactor.md) | [references/vehicle-1.30-refactor.md](references/vehicle-1.30-refactor.md) |
| Rider animations, handlebar poses, or .agf graph | [references/rider-animation.md](references/rider-animation.md) + `dayz-animation-pipeline` | [references/rider-animation.md](references/rider-animation.md) and `dayz-animation-pipeline/SKILL.md` |

Architecture recommendation: for a new motorbike, inherit in Enforce Script directly from `MotorbikeScript` and copy the structure of `Motorbike_01_ColorBase` in your `config.cpp` (see [references/config-contract.md](references/config-contract.md)). Do not attempt to inherit from `CarScript` or adapt 4-wheel `Axles` blocks: `simulation = "motorbike"` does not use them (invariant 1; exact failure mode is not measured [UNVERIFIED]). If inheriting directly from `MotorbikeScript`, override `NeedElectricitySourceDevice()`: without override it returns `true` and the motorbike would require a battery (`Transport.c:830-833`).

## Day-0 critical path for a new motorbike

Ordered checklist of minimum mandatory deliverables to have an operational motorbike:

1. [EXACT] **CfgPatches and CfgVehicles declaration**: declare the addon with mandatory dependency on `"DZ_Vehicles_Singletrack"` (`P:\DZ\vehicles\singletrack\config.cpp:1-13`). Inherit from `MotorbikeScript` with `simulation = "motorbike"`.
   ```cpp
   class CfgPatches { class MiMoto { units[] = {}; requiredAddons[] = {"DZ_Data", "DZ_Vehicles_Singletrack"}; }; };
   ```
2. [EXACT] **`class Crew` block**: define `class Driver` and `class CoDriver: Driver` pointing to `actionSel = "seat_driver"`, `proxyPos = "crewDriver"`, `getInPos = "pos_driver"`, `getInDir = "pos_driver_dir"`, and both with `isDriver = 1;` (`config.cpp:52-69`).
3. [EXACT] **`class SimulationModule` block**: declare `class Wheels` with `class Front` and `class Rear` (without `Axles`), `class Brakes` with `Front` (`input = 1`) and `Rear` (`input = 0`), and lean curves in `class Steering` (`config.cpp:267-382`).
4. [EXACT] **Animation sources (`AnimationSources`)**: declare shock absorbers `damper_1`, `damper_2` and wheel state `AnimHitWheel_1`, `AnimHitWheel_2` (`config.cpp:111-147, 383-403`). If it has a flip-up seat, add `SeatDriver`.
5. [EXACT] **Damage system (`DamageSystem`)**: define `GlobalHealth` (1000 HP) and the zones from `Motorbike_01`: `Chassis`, `Fender` (transfer target for `Front`), `Engine` (`inventorySlots[] = {"SparkPlug"}`), `Front`, `Reflector_1_1`, `Back`, and `FuelTank`, plus `GUIInventoryAttachmentsProps` (`config.cpp:404-993`).
6. [EXACT] **Wheels and Proxies**: define classes derived from `MotorbikeWheel` for front and rear wheels with their `_Ruined` versions in `CfgVehicles` (`config.cpp:154-219`), and proxies in `CfgNonAIVehicles` (`config.cpp:2323-2380`):
   ```cpp
   class MiMoto_Wheel_1: MotorbikeWheel { scope = 2; radius = 0.29; width = 0.06; tyreLateralFriction = 3.0; };
   ```
7. [EXACT] **High-speed anti-snag geometry**: define `speedGeomActivation = 0.29;` and `speedGeoms[] = {"geotohide"};` (`config.cpp:262-266`).
8. [EXACT] **4_World Script Class**: create class derived from `MotorbikeScript` (`P:\scripts\4_World\Entities\Vehicles\InheritedMotorbikes\Motorbike_01.c:1-163`). Configure in constructor impact damages (`m_VehicleContactDamageCoef`, `m_CrewContactDamageCoef`), engine and horn SoundSets, and register lights and the `FrontOn/Off` / `BrakeOn/Off` selections (your `hiddenSelections[]` indices, §DayZ 1.30.164200 changes) in `m_LightsComponent`:
   ```c
   m_VehicleContactDamageCoef = 0.03;
   m_CrewContactDamageCoef = 0.018;
   ```
9. [EXACT] **Mandatory script overrides**: implement `GetAnimInstance()` returning `VehicleAnimInstances.MOTO1` (10) or `MOTO2` (11), `HasDirectionalInOutAction()` (`true`), `GetSeatAnimationTypeDirectional()`, `GetTransportCameraDistance()`, `GetTransportCameraOffset()`, and `NeedElectricitySourceDevice()` (`false` on magneto/kickstart bikes).
10. [DESIGN] **Geometry and P3D model**: ensure bone `"drivewheel"` on the front fork, light memory points (`light_1_1`, `light_1_2_reverse`), smoke (`ptcExhaust_start/end`, `ptcEnginePos`), fuel cap (`refill`), and damage selections (`dmgZone_*`). Details in [references/forward-contract.md](references/forward-contract.md).
11. [EXACT] **Sounds**: reference engine, exhaust, shock absorber, and horn SoundSets defined in `P:\DZ\sounds\hpp\config.cpp` (motorbike SoundSets `:383946-385029`, SoundShaders `:316513-318240`, class start lines):
    ```c
    m_EngineStartOK = "Motorbike_01_engine_start_SoundSet";
    m_HornComponent.RegisterSound(VehicleHornMode.SHORT, "Motorbike_01_Horn_Short_SoundSet");
    ```
12. [DESIGN] **Economy and Spawn**: register in `types.xml` and `cfgspawnabletypes.xml` with spark plug (`SparkPlug`) and wheels installed.
13. [EXACT] **Decide rider posture BEFORE modeling (added 2026-09-19, LFDucati)**: no runtime hand adjustment exists. Each vanilla motorbike has its own column in the animation set (`"Jawa_05"`, `"Jawa_Bitrak"`: `P:\DZ\anims\workspaces\player\player_main\player_main.ast:1114-1121`) with its own clip set (26 and 27 distinct `.anm` clips; e.g. `Vehicle.Jawa_05.DriverSteeringMain` → `…/Jawa_05/p_motorbike_01_driver_steering_pose.anm`, `player_main.asi:5131`; `Vehicle.Jawa_Bitrak.DriverSteeringMain` → `…/Jawa_Bitrak/p_motorbike_02_driver_steering_pose.anm`, `player_main.asi:5179`), and hand IK takes its targets `LHandIKTarget`/`RHandIKTarget` from those clips (`AnimNodeIK2Target0` with child `SteeringBlend`, `Vehicles.agf:17-46`), not from the motorbike P3D. There are no Arma-style config keys (`driverLeftHandAnimName`: 0 occurrences in `vehicles_singletrack`, `vehicles_wheeled`, and `bin` `config.cpp`). Consequence: either you fit the handlebar/footpegs/seat of YOUR model to the contact points of MOTO1 or MOTO2 (measure them in-game on the vanilla bike before touching ergonomics), or you add a column with custom clips, which means modifying `player_main` (the single-animation-mod wall, see [references/rider-animation.md](references/rider-animation.md) §5). (corrected 2026-09-28) There is a third way without touching `player_main`: a child `.asi` with custom clips only on your bike class (preflight above; tested in-game stationary and moving on the H2R). A handlebar of a different width does NOT fix itself.

## The ten invariants that break a motorbike

Each rule must be strictly followed to prevent critical failures in the entity:

### 1. Flat `Wheels` in config, NEVER `Axles` block
- **Symptom** [UNVERIFIED, expected and not measured in-game]: simulation initialization failure when spawning the entity; exact failure mode (RPT error, inert entity, or process crash) has not been measured.
- **Cause**: The `simulation = "motorbike"` engine reads directly `class Wheels { class Front; class Rear; }` (`P:\DZ\vehicles\singletrack\config.cpp:71-91`). The `class Axles` hierarchy belongs exclusively to `simulation = "car"` / `"carx"`.
- **Citation**: `P:\DZ\vehicles\singletrack\config.cpp:71-91, 344-381`.
- **Remedy**: Remove any `Axles` block from `SimulationModule`. Declare `Front` and `Rear` directly inside `Wheels: Wheels`:
  ```cpp
  class Wheels: Wheels
  {
      class Front: Front { inventorySlot = "MiMoto_Wheel_1"; animDamper = "damperfront"; };
      class Rear: Rear   { inventorySlot = "MiMoto_Wheel_2"; animDamper = "damperback"; };
  };
  ```

### 2. Brakes Front input=1 / Rear input=0
- **Symptom**: The front brake lever does not respond, or the rear pedal blocks the front wheel unexpectedly.
- **Cause**: On motorbikes, the front brake is bound to the handbrake channel (`input = 1`) and the rear to the regular pedal brake (`input = 0`). The API documents it: `SetBrake` is the rear brake and `SetHandbrake` is the front brake (`P:\scripts\3_Game\Vehicles\Motorbike.c:129-139`).
- **Citation**: `P:\DZ\vehicles\singletrack\config.cpp:296, 305`.
- **Remedy**: In `SimulationModule -> Brakes`, define `class Front { input = 1; ... };` and `class Rear { input = 0; ... };`:
  ```cpp
  class Brakes
  {
      class Front { input = 1; pressureBySpeed[] = {0,0.85,10,0.7,40,0.6,60,0.5,80,0.5}; };
      class Rear  { input = 0; pressureBySpeed[] = {0,0.95,10,0.1,20,0.1,40,0.1,50,0.1}; };
  };
  ```
  The rear pressure drops from `0.95` at standstill to `0.1` from point `10` on the curve (`config.cpp:306`); whether this serves to prevent rear axle skids is an interpretation [UNVERIFIED].

### 3. `"drivewheel"` bone in P3D for headlight
- **Symptom**: The headlight light cone remains pointed rigidly forward along the chassis and does not rotate when turning the handlebars.
- **Cause**: `MotorbikeScript.c:1122-1124` looks up the `"drivewheel"` bone in a hardcoded manner via `GetBoneIndex("drivewheel")` to align the light cone with the front fork.
- **Citation**: `P:\scripts\4_World\Entities\Vehicles\MotorbikeScript.c:1122-1124`.
- **Remedy**: Name the front fork and handlebar selection `"drivewheel"` in the P3D geometry and in `skeletonBones[]` of your `model.cfg`:
  ```c
  int driveWheelIndex = GetBoneIndex("drivewheel");
  m_LightsComponent.AttachLightOnObject("Front", GetMemoryPointPos(m_HeadLightPoint), vector.Zero, VehicleLightMode.SEGREGATED, driveWheelIndex);
  ```

### 4. `SeatDriver` and `CanGetIn` entry condition
- **Symptom**: The player receives no prompt to get on the bike (`CanGetIn()` returns `false`).
- **Cause**: If inheriting from `Motorbike_01_ColorBase`, the script requires `GetAnimationPhase("SeatDriver") == 0.0` (`Motorbike_01.c:94`). If your model lacks the `"SeatDriver"` animation, the condition permanently fails.
- **Citation**: `P:\scripts\4_World\Entities\Vehicles\InheritedMotorbikes\Motorbike_01.c:89-95`.
- **Remedy**: For motorbikes without a flip-up seat, inherit directly from `MotorbikeScript` or override `CanGetIn()` without evaluating `"SeatDriver"`. If keeping the seat, declare `SeatDriver` in `AnimationSources` with initial phase `0.0`:
  ```cpp
  class AnimationSources: AnimationSources
  {
      class SeatDriver { source = "user"; initPhase = 0.0; animPeriod = 1.0; };
  };
  ```

### 5. Contact damage coefficients and instant death
- **Symptom**: The rider dies instantly upon grazing a minor obstacle at low speed.
- **Cause**: Each contact produces two quantities on the server (`CheckContactCache`, `MotorbikeScript.c:1267-1372`, called inside `if (g_Game.IsServer())` at `:758-773`): vehicle damage `dmg = |impulso × m_VehicleContactDamageCoef|` (`:1278`) and the crew damage base `crewDmgBase = |impulso / masa × 500 × m_CrewContactDamageCoef|` (`:1279`, calculates as if the motorbike weighed 500 kg). Threshold tiers are decided with `dmg`; only the last damages the crew, and there death depends on `crewDmgBase` (`DamageCrew`, `:1375-1427`).
- **Citation**: `P:\scripts\4_World\Entities\Vehicles\MotorbikeScript.c:1278-1279, 1316-1365, 1384-1398` and `P:\scripts\3_Game\constants.c:898-901`.
- **Tiers according to vehicle `dmg` (`constants.c:898-901`)**:
  - `dmg < 10` (`MOTORBIKES_CONTACT_DMG_MIN`): ignored (`:1301-1302`).
  - `10 ≤ dmg < 25` (`MOTORBIKES_CONTACT_DMG_THRESHOLD_SMALL`): damage to zone without transfer (`NO_TRANSFER`) and light crash sound (`:1310-1320`).
  - `25 ≤ dmg < 40` (`MOTORBIKES_CONTACT_DMG_THRESHOLD_MID`): `FallOver()` and light sound; crew takes no damage (`:1321-1331`).
  - `dmg ≥ 40`: `DamageCrew(crewDmgBase)`, `FallOver()` and heavy sound (`:1332-1344`).
  - From `dmg ≥ 10`, `ProcessDirectDamage` applies `dmg` to the impacted zone (`:1350`).
- **Inside `DamageCrew`**: if `crewDmgBase > 80` (`MOTORBIKES_CONTACT_DMG_KILLCREW`, strictly greater) it executes `crew.SetHealth(0.0)` (`:1369, 1383`); otherwise, it subtracts shock from 50 to 150 and health from 2 to 100, interpolated between 25 and 80 (`:1387-1409`).
- **Remedy**: `m_VehicleContactDamageCoef` decides when the motorbike falls (`dmg ≥ 25`) and when the crew is damaged (`dmg ≥ 40`); `m_CrewContactDamageCoef` decides if that damage kills (`crewDmgBase > 80`). Vanilla values: `0.03` / `0.018` (`Motorbike_01.c:5-6`) and `0.035` / `0.012` (`Motorbike_02.c:5-6`).

### 6. Lean angles (`maxLeanAngle`) and tipover (`tipoverAngle`)
- **Symptom** [UNVERIFIED, expected and not measured in-game]: the motorbike falls over on its own when accelerating or stays vertical without leaning in turns.
- **Cause**: `Steering` declares lean curves and threshold `tipoverAngle = 50.0`; both are consumed by native simulation (`simulation = "motorbike"`), not script, so their exact effect above the threshold cannot be verified in text [UNVERIFIED].
- **Citation**: `P:\DZ\vehicles\singletrack\config.cpp:270-282`.
- **Remedy**: Copy the validated table from `Motorbike_01`:
  ```cpp
  class Steering
  {
      tipoverAngle = 50.0;
      maxLeanAngle[] = {1,0,5,8,15,10,30,17,40,22,80,30};
      maxSteeringAngle[] = {1,40,5,40,15,20,30,5,40,3,80,1};
      directControl[] = {0.0,1.0,5.0,1.0,30.0,0.6,60.0,0.2,80.0,0.0};
  };
  ```

### 7. Kickstand (`Kickstand`) in lifecycle
- **Symptom** [UNVERIFIED, expected and not measured in-game]: the motorbike collapses sideways upon spawning on the ground.
- **Cause**: the vanilla constructor deploys the kickstand using native method `SetKickstand(1)`; what native physics specifically does with that value is an assumption from the original research notes, not read in Enforce [UNVERIFIED].
- **Citation**: `P:\scripts\4_World\Entities\Vehicles\MotorbikeScript.c:141`.
- **Remedy**: Ensure the constructor maintains `SetKickstand(1);`. No retail script retracts the kickstand: the only `SetKickstand(0.0)` is in `OnInput` within `#ifdef DIAG_DEVELOPER` and only runs with debug mode active (`MotorbikeScript.c:856-858, 930`). If it retracts while riding, native simulation does it [UNVERIFIED].

### 8. Hidden geometry at speed (`speedGeomActivation` / `speedGeoms`)
- **Symptom**: The motorbike collides with invisible terrain micro-bumps or is flung into the air at high speed.
- **Cause**: The support stand geometry collides with ground unevenness if not disabled when moving at high speed. In `Motorbike_01` it activates at `0.29` and in `Motorbike_02` at `0.27`.
- **Citation**: `P:\DZ\vehicles\singletrack\config.cpp:262-266, 1455-1459`.
- **Remedy**: Define in config `speedGeomActivation = 0.29;` and `speedGeoms[] = {"geotohide"};`, isolating kickstand geometry into the `"geotohide"` component of LOD Geometry.
- **1.30.164200**: "Motorbikes could be launched into air when colliding with small objects or on bridges" is fixed in the engine [reported: BI 1.30.164200 notes]; no script or `speedGeom*` config changed. Re-test a launch you saw on 164078 before tuning a mod's geometry or contact coefficients for it.

### 9. Mandatory modular components (Lights, Horn, VFX)
- **Symptom** [UNVERIFIED, expected and not measured in-game]: failure when accessing an uninstantiated component in the lighting code path, or horn without sound and without noise for infected.
- **Cause**: In DayZ 1.30, lights no longer reside in `m_Headlight` nor the horn in `m_CarHornState`; they operate in independent components.
- **Citation**: `P:\scripts\4_World\Entities\Vehicles\MotorbikeScript.c:113, 127, 139`.
- **Remedy**: Instantiate `VehicleLightsComponent`, `VehicleHornComponent`, and `VehicleVFXComponent` in the constructor, registering their corresponding profiles and SoundSets:
  ```c
  m_LightsComponent = VehicleLightsComponent(this);
  m_HornComponent = VehicleHornComponent(this);
  InitializeParticles(); // Instancia m_VFXComponent
  ```
- [EXACT] Vehicle lights are toggled with `UAToggleVehicleLights` via `ToggleVehicleLightsActionInput` (`P:\scripts\4_World\Classes\UserActionsComponent\ActionInput.c:849-855`); `ActionSwitchLights.GetInputType()` returns that class (`ActionSwitchLights.c:16-19`). `UAToggleHeadlight` is the headtorch (`ToggleLightsActionInput`, `ActionInput.c:776-782`). Both names exist in `P:\bin\constants.xml:90-91`. Detail: [references/actions-hud-lights-sounds.md](references/actions-hud-lights-sounds.md) §5.

### 10. Blowtorch-exclusive repair actions (1.30; still true on stable 1.30.164078)
- **Symptom**: The player receives no action to repair engine or chassis when aiming with a wrench or epoxy putty.
- **Cause**: On 1.30 (first seen on Exp 164014, unchanged on stable 164078), motorbike repair actions are assigned only to the blowtorch (`Blowtorch.c`). Additionally, `ActionRepairMotorbikeEngine` was not registered in `ActionConstructor.c`.
- **Citation**: `P:\scripts\4_World\Entities\ItemBase\Blowtorch.c:109, 111, 113` (the only motorbike repair `AddAction` in all of `scripts`) and `P:\scripts\4_World\Classes\UserActionsComponent\ActionConstructor.c:252-260` (registration of motorbike actions, without `ActionRepairMotorbikeEngine`).
- **Remedy**: If your mod requires repairing with traditional hand tools, you must bind actions to items via `modded class` and insert them into `ActionConstructor`:
  ```c
  modded class ActionConstructor
  {
      override void RegisterActions(TTypenameArray actions)
      {
          super.RegisterActions(actions);
          actions.Insert(ActionRepairMotorbikeEngine);
      }
  }
  ```

## Measured lessons from LFDucati (measured on 1.30 Exp 164014, September 2026)

Measured in-game (1.30 Exp 164014; the script and config API involved is unchanged on stable 164078) with a 157 kg Desmosedici GP16 inherited from `Motorbike_02_ColorBase` (DayZDiag, dedicated server and client). Each rule cites its lesson from the vault (`AI/20_Knowledge/lessons-learned.md`), which stores the narrative and numbers.

1. **Overall first gear must be short.** [EXACT] With the automatic clutch, first × `driveRatio` between 20 and 26 starts (Motorbike_02: 25.7; LFDucati settled on 20); 6.77, that of a real MotoGP, does not move the bike: 0.05-0.1 km/h wide open and rpm stuck at two values. Decide it with a session A/B using the vanilla drivetrain as control, and measure the ratio applied by the engine (rpm·π/30 ÷ ω of the drive wheel) to confirm config loaded. On 1.30 motorbikes `w0` is the front: distinguish wheels by rolling radius (ω·r = v), not by `WheelGetPositionLS` (LL-506).
2. **Suspension is sized to the load of each wheel.** [INFERRED] The model fitting measurements is F = k (u + travelMaxDown), with the wheel hanging from the modeled axle and u linear in the damper phase (phase 0 = -travelMaxDown, phase 1 = +travelMaxUp). [EXACT] With it, in the variant resting solely on the springs the sum of forces / g yielded 157.1 kg for a 157.0 kg bike; in the product red variant the springs only carried 132.7 kg and the rest rested on the Engine box against the ground. For it to rest on the axle, travelMaxDown = F / k on each wheel (or increase k and scale damping with √(k·m)). A motorbike inheriting suspension from a lighter one inherits its sag; if the sum does not match the mass, look for what else is resting on the ground (LL-512).
3. **No Geometry box can touch the ground at full suspension compression.** [EXACT] Each hit is a contact: damage deciding the tier is `Math.AbsInt(data[0].m_Impulse * m_VehicleContactDamageCoef)` (`MotorbikeScript.c:1278`) and, from 25 upwards, the branches call `FallOver()` (`:1325-1357`), throwing the rider off (invariant 5). Gate before testing: lowest point of each component, minus remaining compression from rest, ≥ 5 mm; and `m_VehicleContactDamageCoef` scaled by mass (× m_vanilla / m_propia) so the same hit counts equally (LL-507).
   - (LL-507 details) [EXACT] If a `..._damper_land` box is lowered to seat the physical wheel,
     shrink it keeping the SAME centre - the physical wheel does not move; and log contacts
     server-side (`Transport.OnContact`: zone, point, impulse change) before tuning coefficients.
     (measured on 1.30 Exp 164014)
4. **The rider proxy only positions and yaws around the vertical.** [EXACT] The engine applies the crew proxy frame but keeps the player vertical: a proxy tilted 20 degrees forward resulted in yaw of about 17 degrees and zero lean; a rolled proxy produced a player standing 30 cm off the bike. Crouching or leaning the rider requires animation (preflight above and `references/rider-animation.md`) (LL-511, LL-509).
5. **A hidden selection only changes material if `model.cfg` lists it in `sections[]`.** [EXACT] Config alone is not enough and the failure is silent: check it in the ODOL (sectional selection), not in config. Lights render by index in `hiddenSelections`: the `MotorbikeScript` defaults are headlight 0, brake 1, tail light 2 and dashboard 3 (`MotorbikeScript.c:114-122`); the vanilla bikes re-register `FrontOn/FrontOff` and `BrakeOn/BrakeOff` onto their own light slots: Motorbike_01 front 3 / brake 2 (`Motorbike_01.c:19-22`), Motorbike_02 front 4 / brake 6 (`Motorbike_02.c:20-23`), matching `hiddenSelections[]` at `P:\DZ\vehicles\singletrack\config.cpp:232-239` and `:1419-1428`. In a custom model the default 0-3 positions usually fall onto livery sections: re-register every key you use (recipe in §DayZ 1.30.164200 changes) (LL-508). Before 1.30.164200: Motorbike_02 registered brake at 2 (`camo_fenderfront`), neither bike re-registered `FrontOn/FrontOff`, and the singletrack config had no `*ReflectorMatOn/Off` keys, so the swaps were empty no-ops.

## DayZ 1.30.164200 changes (patch of 2026-10-21)

Static reading of `P:\scripts` / `P:\DZ` on 1.30.164200; none of it was measured in game yet. Full script delta:
`dayz-130-migration` → `references/api-delta-1.30.164078-to-164200.md`.

1. **Lights ("Headlights on the bikes were misconfigured", DZEXP-92).**
   - Config: both vanilla bikes gained `frontReflectorMatOn/Off` and `brakeReflectorMatOn/Off`
     (`P:\DZ\vehicles\singletrack\config.cpp:256-259`, `:1449-1452`); Motorbike_02 `hiddenSelections[]` slots 5-7 were
     renamed `light_1_1, light_1_2, light_brake_1_2` → `lights_front, lights_rear, lights_brake` (`:1419-1428`).
   - Script: each bike re-registers `FrontOn/FrontOff` on its front slot — Motorbike_01 index 3
     (`InheritedMotorbikes\Motorbike_01.c:19-20`), Motorbike_02 index 4 (`Motorbike_02.c:20-21`) — and Motorbike_02 moves
     `BrakeOn/BrakeOff` from index 2 to 6 (`:22-23`; Motorbike_01 keeps 2, `Motorbike_01.c:21-22`). Both dropped their
     `HasGangedTailAndBrakeLights()` override (`true`), so they use the `MotorbikeScript` default `false`
     (`MotorbikeScript.c:169-172`) and `BrakeOff` is applied again when the brake is released (`:1236-1255`). The typo
     `"BrakesOff"` (an unregistered key, which made `SetSelectionMaterial` log
     `ErrorEx("No VehicleLightSelectionData available for key:BrakesOff")`, `VehicleLightsComponent.c:66-73`) is now
     `"BrakeOff"` (`MotorbikeScript.c:1248`).
   - Beam: the front profiles' `m_SegregatedBrightness` went 2 (Motorbike_01) / 3 (Motorbike_02) → 8; radius/angle/colour
     unchanged (`P:\scripts\4_world\entities\scriptedlightbase\spotlightbase\vehiclelightprofiles\motorbike_01.c:5`,
     `motorbike_02.c:5`).
   - **Recipe for a custom bike** (pattern of `Motorbike_01.c:16-22` / `Motorbike_02.c:17-23`): list the light slots in
     `hiddenSelections[]` (and in `model.cfg` `sections[]`, lesson 5 above), give the config the four material keys, and
     register the four keys with YOUR indices after `super` has run (`RegisterSelection` is a `map.Set`,
     `VehicleLightsComponent.c:56-59`):
     ```cpp
     // config.cpp, class MiMoto: MotorbikeScript
     hiddenSelections[] = {"camo1","camo_fender","lights_brake","lights_front","lights_rear"}; // 0..4
     frontReflectorMatOn  = "MiMod\data\mimoto_lights_front_e.rvmat";
     frontReflectorMatOff = "MiMod\data\mimoto_lights_nolight.rvmat";
     brakeReflectorMatOn  = "MiMod\data\mimoto_lights_rear_brake.rvmat";
     brakeReflectorMatOff = "MiMod\data\mimoto_lights_nolight.rvmat";
     ```
     ```c
     // MiMoto constructor
     m_LightsComponent.RegisterLight("Front", new VehicleLightData(new MiMotoLightProfileFront())); // : VehicleLightProfileFront
     m_LightsComponent.RegisterLight("Rear",  new VehicleLightData(new MiMotoLightProfileRear()));  // : VehicleLightProfileBrakeAndReverse
     m_LightsComponent.RegisterSelection("FrontOn",  {new VehicleLightSelectionData(3, "frontReflectorMatOn")});
     m_LightsComponent.RegisterSelection("FrontOff", {new VehicleLightSelectionData(3, "frontReflectorMatOff")});
     m_LightsComponent.RegisterSelection("BrakeOn",  {new VehicleLightSelectionData(2, "brakeReflectorMatOn")});
     m_LightsComponent.RegisterSelection("BrakeOff", {new VehicleLightSelectionData(2, "brakeReflectorMatOff")});
     ```
     `TailOn/Off` (index 2, key `TailReflectorMatOn/Off`) and `DashboardOn/Off` (index 3, `dashboardMatOn/Off`) stay
     registered by `MotorbikeScript` (`:118-122`); they are no-ops while the config has no such key (vanilla bikes have
     none). If you add those keys, re-register those two pairs with your indices as well. A bike that brakes and shows its
     tail light through one bulb overrides `HasGangedTailAndBrakeLights()` to `true` (then `BrakeOff` is never applied).
   - **Existing bike mods — check this:** a config class that inherits `Motorbike_01_ColorBase`/`Motorbike_02_ColorBase`
     now inherits the four vanilla material keys, and a script class that inherits the vanilla script class inherits the
     vanilla indices (3/2 or 4/6). If the mod's own `hiddenSelections[]` differs, the vanilla light rvmat now lands on
     whatever slot sits at that index (often livery) when lights or brakes switch; on 164078 the same code did nothing
     (no keys). Fix: re-register the four keys with your indices and set the four config keys to your rvmats (an empty
     string disables a swap, `VehicleLightsComponent.c:77-79`). A bike that inherits `MotorbikeScript` directly and
     declares no `*ReflectorMat*` key is unaffected.
2. **No detaching while the engine runs or anyone sits on the bike.** New `MotorbikeScript.CanReleaseAttachment`
   (`MotorbikeScript.c:612-625`): after the `EntityAI` slot-lock rule it returns `false` while `EngineIsOn() || HasCrew()`,
   for EVERY attachment — wheels, `SparkPlug`, `Reflector_1_1` and any slot a mod adds. Cars are different: `CarScript`
   blocks only while `EngineIsOn() && IsMoving()` plus its spare-wheel rule (`carscript.c:638-667`), and each car adds
   part rules (e.g. `civiliansedan.c:136-150`). A mod that adds its own bike slot (cassette, radio, saddle bag) and wants
   it usable while riding exempts only that slot [DESIGN, not compiled here]:
   ```c
   modded class MotorbikeScript
   {
   	override bool CanReleaseAttachment(EntityAI attachment)
   	{
   		if (attachment && attachment.IsInherited(MyMod_Cassette))	// own item only; vanilla parts keep the lock
   		{
   			InventoryLocation il = new InventoryLocation();
   			attachment.GetInventory().GetCurrentInventoryLocation(il);
   			return !GetInventory().GetSlotLock(il.GetSlot());	// the EntityAI base rule, P:\scripts\3_game\entities\entityai.c:1512-1533
   		}
   		return super.CanReleaseAttachment(attachment);
   	}
   }
   ```
3. **`HasCrew()` / `IsMoving()` on `Transport`.** `proto native bool HasCrew()` is new (`P:\scripts\3_game\vehicles\transport.c:175`)
   and is what the detach lock uses. `bool IsMoving()` is new on `Transport` and its body is
   `Error("IsMoving() not implemented"); return false;` (`transport.c:819-823`); only `CarScript` overrides it
   (`GetSpeedometerAbsolute() > 3.5`, `carscript.c:2221-2224`). `MotorbikeScript` does not, so `bike.IsMoving()` logs a
   script error on every call: read `GetSpeedometerAbsolute()` (`Motorbike.c:55-58`) or give your bike class
   `override bool IsMoving() { return GetSpeedometerAbsolute() > 3.5; }`. A bike class written for 164078 that declares
   its own `bool HasCrew()` or `bool IsMoving()` now collides with (proto native) or shadows (script) the new member:
   rename it with a mod prefix, or make it a real `override` that calls `super` where that makes sense. Rule for new
   code: never give a helper on a `Transport` subclass a generic name; prefix it (`MyMod_HasRider()`).
4. **3rd-person bike camera clamp.** `DayZPlayerCamera3rdPersonVehicleMotorbike.OnUpdate` now clamps the final
   `rot[0]` and `rot[1]` with the class constants `CONST_UD_MIN = -60` / `CONST_UD_MAX = 80`
   (`dayzplayercameravehicles.c:241-242`, `:407-408`) after the `UP_ANGLE_CAP` cap (`:402-405`). Vanilla applies the
   pitch limits to the yaw component too; what that does to side free-look was not measured. In the same patch the base
   `UpdateUDAngleUnlocked` / `UpdateLRAngleUnlocked` ignore the `pMin/pMax` they are passed and use
   `m_pPlayer.GetLookLimits(...)` instead ("match the ones that are used in C++", `dayzplayercamera_base.c:152-156`,
   `:197-202`), so the bike camera's limits at `:336-337` no longer bound the look input; only the final clamp does.
   A custom bike camera that subclasses this class inherits both.
5. **Wheel VFX cooldown.** `RegisterEffectGroup(WHEEL_SMOKE / WHEEL_CONTACT, ..., new VehicleVFXDataParams(pos, dir, 2.0))`
   (`MotorbikeScript.c:203-204`; cars `carscript.c:381-382`): the third argument is `m_CooldownSec`, the minimum seconds
   between two (re)starts of one wheel's effect (`vehiclevfxcomponent.c:82-93`; guard `VehicleVFXData.CanPlay()`,
   `:43-50`, checked first in `PlayEffectData`, `:171-175`). It went 5.0 → 2.0, so a wheel's smoke/contact puff can restart
   every 2 s instead of 5 s. Detail: `dayz-particles` → `references/dayz-1-30-particles.md`.
6. **Sounds (config only).** Horn `volumefactor` lowered on both bikes — `*_Horn_SoundSet` 0.6 → 0.5, `*_Horn_Short_SoundSet`
   0.8 → 0.4 (`P:\DZ\sounds\hpp\config.cpp:386288`, `:386297`, `:386851`, `:386860`) — and the `*_Horn_Short_SoundShader`
   `volume` 1.4 → 0.9 (`:317961`, `:318877`). `Motorbike_02_Engine_Ext_Broken_SoundSet` was removed and replaced by
   `_High` / `_Low` (`:386435`, `:386443`, listed in `Motorbike_02_ColorBase` `soundSetsFilter[]`, `:519768-519769`):
   a mod config that lists the old name in its `soundSetsFilter[]` points at a SoundSet that no longer exists.

## How to verify

1. **Offline linter before opening the game**:
   Run the offline validator on your addon directory:
   `python <KNOWLEDGE_PACK>/tools/dayz-script-validator/scripts/script_validator.py <addon_root>`
   - Check `errors` and `warnings` keys at the root of output JSON.
   - Return code is 0 (PASS), 1 (FAIL), or 2 (WARN).
   - Always gate by `len(errors) == 0`; a clean tree with minor warnings returns 2 and is valid.
   - Check that no broken references exist in Enforce `.c`, layouts, `config.cpp`, or `.rvmat`.
   - Use delta comparison against the unchanged base tree to isolate errors introduced by your mod.
2. **In-game verification ladder (via DayZ-MCP or local Diag server)**:
   - **Step 1 (Spawn)**: Spawn the bike using the `world_spawn` tool on a local server run with `-filePatching`. Confirm it stays upright on the kickstand without endless oscillations or sinking into the ground.
   - **Step 2 (Render)**: Check that wheels show their correct proxies, rvmat materials respond, and there are no grey panels or invisible meshes due to incorrect selection names.
   - **Step 3 (Get-In)**: Trigger `vehicle_get_in_client` approaching from the left flank and then from the right; verify that `ActionGetInTransport` activates directional animations `GetIn_L` and `GetIn_R`.
   - **Step 4 (Starting and Driving)**: Trigger `ActionStartEngineMotorbike` or the `engine_set` tool. Listen for kickstart lever sound, verify front headlight rotates together with handlebars (`drivewheel`), and check that the 3rd person camera follows steering with spring damping.
   - **Step 5 (Driving Telemetry)**: Monitor RPM, speed, and gear via `vehicle_telemetry` or `telemetry_read` to verify `SimulationModule` response.
   - **Step 6 (Fall and Recovery)**: Cause a side collision or sharp turn to induce a forced tipover. Check that the player is ejected to the ground (`CrewShouldEject`) and that `ActionPickUpMotorbike` allows picking it up via `action_use`.
   > **Measured correction (2026-09-19, LFDucati; citations re-read in `DayZ_MCP_dev` on that date):** this ladder could NOT be run with DayZ-MCP while 1.30 was Experimental. Status on 2026-10-15 [UNVERIFIED, tool not re-checked]: point (a) should no longer apply now that the Stable `DayZDiag_x64.exe` is 1.30 (164078, now 164200); points (b) and (c) are properties of the DayZ-MCP addon and were not re-checked.
   > (a) The approved launcher only starts Stable: the executable is hardcoded in `tools\native-launchers\dayz-test-v1\worker-runtime.json` (all 10 projects point to `...\common\DayZ\DayZDiag_x64.exe`) and the daemon
   > only accepts `<DAYZ_GAME_PATH>\DayZDiag_x64.exe`, a global setting (`tools\dayz_mcp\daemon.py:552-557`, `process_lifecycle.py:1782-1793`). (b) The `@DayZ_MCP` addon is unaware of `Motorbike`
   > (0 occurrences in `addon\`): control and telemetry cast to `CarScript` (`addon\scripts\5_Mission\MCPClientBridge.c:1191, 2812-2826`), so `engine_set`, `vehicle_control`, and the engine portion
   > of `vehicle_telemetry` do not see a motorbike, which in 1.30 inherits from `Transport` and not from `CarScript`. (c) There is no verb to read player bones (rider pose).
   > Approach used while 1.30 was Experimental: custom diagnostic addon with scenario in a `modded MissionServer` behind a command line parameter, `LFD_…` output in `script_*.log`,
   > two `.bat` files pointing to `DayZ Exp\DayZDiag_x64.exe` launched via `cmd /c start` and a Python driver awaiting log markers (kit: LFDucati probe kit, not published). Seating the player from the
   > server produces crew without `HumanCommandVehicle` on client (skill `dayz-test-ingame` §SP-295): serves to measure pose on server, not to validate actions or HUD.
3. **What to look for in the RPT file (`server-profiles/` or `client-profiles/`)** [UNVERIFIED: message texts are illustrative, not measured; search by symbol]:
   - `Cannot load surface info` or `Undefined sound shader`: missing SoundSets or particles in `CfgSurfaces`.
   - `AnimationSource ... not found`: missing source registration in config `AnimationSources`.
   - `Bone 'drivewheel' not found`: missing bone in front fork of P3D model.
   - Spawn failure with uninitialized simulation: check if an `Axles` block was configured instead of `Wheels` (exact failure mode not measured).

## Work-in-progress markers in vanilla (found on Exp 164014; 1, 2, 4-6 still present on 1.30.164200, 3 fixed in 164200)

Work-in-progress aspects detected in Bohemia code; re-checked on stable 2026-10-15 and unchanged (they may change in a later build):
1. **Provisional unconsciousness ejection**: In `P:\scripts\4_World\Entities\DayZPlayerImplement.c:2452-2458`, motorbike tipover triggers `eModifiers.MDF_UNCONSCIOUSNESS` because the native ragdoll command was not finished in time. Bohemia is expected to replace this with a dedicated ragdoll command.
2. **Orphaned engine repair action**: `ActionRepairMotorbikeEngine.c` exists on disk, but is not inserted into `ActionConstructor.c:252-260` nor bound to hand tools. Only `ActionRepairMotorbikeEngineWithBlowtorch` operates via blowtorch.
3. **Light rvmat properties (fixed in 1.30.164200, DZEXP-92)**: the light selections are swapped with a material read by `ConfigGetString` (`VehicleLightsComponent.c:77`); a missing key reads as empty and the swap is skipped (`:77-79`). 1.30.164200 added `frontReflectorMatOn/Off` and `brakeReflectorMatOn/Off` to both vanilla bikes (`P:\DZ\vehicles\singletrack\config.cpp:256-259`, `:1449-1452`) and renamed Motorbike_02's light slots in `hiddenSelections[]` to `lights_front`, `lights_rear`, `lights_brake` (`:1419-1428`). `TailReflectorMatOn/Off` and `dashboardMatOn/Off` are still absent, so `TailOn/Off` and `DashboardOn/Off` (`MotorbikeScript.c:118-122`) remain no-ops on vanilla bikes. Before 1.30.164200: no `*ReflectorMat*` key in the singletrack config, so every bike light swap was a no-op (cars always had them, e.g. `P:\DZ\vehicles\wheeled\config.cpp:1338`).
4. **Displaced audio definitions**: `P:\DZ\sounds\vehicles\config.cpp` contains only `CfgPatches` (10 lines on 1.30.164078). All motorbike SoundSets reside in `P:\DZ\sounds\hpp\config.cpp:385779-386870`, and their SoundShaders in `:317170-318910`.
5. **Alignment with 1.29 community guides**: Many vehicle modding guides assumed methods such as `CreateFrontLight`, `CarPartsHealthCheck`, or variables such as `data[0].impulse`. In 1.30 `CreateFrontLight`/`CreateRearLight` are `[Obsolete]` and never called (`CarScript.c:2937-2948`), `CarPartsHealthCheck` only forwards to `PartsHealthCheck` (`:2976-2980`) and the contact field is `VehicleContactData.m_Impulse` (`Transport.c:1153-1157`) (see [references/vehicle-1.30-refactor.md](references/vehicle-1.30-refactor.md)).
6. **Vehicle lights input vs headtorch**: `ActionSwitchLights` binds `UAToggleVehicleLights`, not `UAToggleHeadlight`. An `inputs.xml` declaring only `UAToggleHeadlight` will not switch off the headlight. See [references/actions-hud-lights-sounds.md](references/actions-hud-lights-sounds.md) §5.

## Cite-then-verify

Every technical claim in this skill includes the source file path and exact line range. Script, `.agf`, `.asi` and `.ast` cites point at 1.30.164200 (`P:\`, renumbered on 2026-10-21); `config.cpp` cites point at `P:\DZ` / `P:\bin` 1.30.164200. Behaviour that changed in 164200 is stated in §DayZ 1.30.164200 changes and in place.
- If you need to verify a citation, open the corresponding file at that exact line and check the symbol.
- Last full audit: 2026-09-19. All `file:line` citations, `[EXACT]` blocks (byte-for-byte identical), and `references/sources.md` counts were verified against the extraction; displaced citations and claims regarding contact damage, kickstand (DIAG only), default battery, `Jawa_05` clips, and sound ranges were corrected. Previous copies: `*.bak-20260919-pre-audit`. Still unverified: anything depending on native simulation (curve units, effect of `tipoverAngle`).
- (2026-10-15) Stable 1.30.164078 was diffed by line count against the Exp extraction: identical for every cited script/animation file; config files differ only in DeRap layout and were renumbered.
- When Bohemia publishes a new build, **DO NOT overwrite** the current extraction directory. Extract the new version into a sibling directory identified by its build number (for example `<extract-root>/1.30.XXXXXX/`), run a textual diff against `1.30.164014`, and update shifted line numbers.

## References Index

- [references/config-contract.md](references/config-contract.md): Open to copy literal [EXACT] `config.cpp` blocks (Crew, SimulationModule, DamageSystem, Wheels, AnimationSources, speedGeoms) and view the complete motorbike parameter table.
- [references/script-api.md](references/script-api.md): Open to view `proto native` signatures from `Motorbike.c`, stability callbacks, `MotorbikeScript` lifecycle, and differences from `CarScript`.
- [references/actions-hud-lights-sounds.md](references/actions-hud-lights-sounds.md): Open to implement or fix kickstart actions, ground pickup, `MotorBikeHud` instrumentation, light/horn components, and exact `sounds_hpp` SoundSets. Includes §5: `UAToggleVehicleLights` vs `UAToggleHeadlight`.
- [references/rider-animation.md](references/rider-animation.md): Open when configuring rider poses, handlebar IK, `Vehicles.agf` graph nodes (`MotorBikeSTM`), directional animations, or 3rd person camera.
- [references/vehicle-1.30-refactor.md](references/vehicle-1.30-refactor.md): Open when migrating a car mod from 1.29 to 1.30; contains deprecated method list, component migration, and 1.29 guide myth table.
- [references/forward-contract.md](references/forward-contract.md): Open for the full production checklist before packaging a new motorbike and to know what to measure on the P3D model.
- [references/sources.md](references/sources.md): Open to audit extracted sources, verify total line count of each cited file, and understand audio PBO layout.
- [references/vanilla-p3d-anatomy.md](references/vanilla-p3d-anatomy.md) (added 2026-09-19): Open for the REAL P3D/model.cfg anatomy of `Motorbike_01` and `Motorbike_02` read from ODOL v56 binaries — mass and center of mass, LODs with resolution and faces, skeleton with parents, all 44/60 animations with their source, angles and axes, LOD Memory points with coordinates, proxies, selections, and named properties. It is the parity benchmark for a new motorbike. `parsed` state: read its header before citing any number.

## WHAT THIS SKILL COULD NOT VERIFY

1. **Binary 3D models (`.p3d`)**: Exact face selections, memory points not cited in script, and collision LODs of `motorbike_01.p3d` and `motorbike_02.p3d` cannot be inspected in text; they must be measured with `dayz-p3d-audit` or P3D analysis tools.
   - (added 2026-09-19, LFDucati; header re-read on stable 2026-10-15: `motorbike_01.p3d` / `motorbike_02.p3d` still v56) **1.30 P3Ds are ODOL v56 (1.29 = v54) and the external ODOL->MLOD converter only reads v54**: today they CANNOT be measured without patching the reader; see the §Preflight of that skill (an unpublished reader patch is in progress). Verified via binary search in `Motorbike_01.p3d`: `drivewheel`, `damper_1`, `damperfront`, `kickstand`, `geotohide`, `pos_driver`, `light_1_1`, `refill`, and 49 `proxy:` names exist.
   - (added 2026-09-19; corrected 2026-09-19) **Corrected errata**: the Motorbike_02 column in table §6 of [references/config-contract.md](references/config-contract.md) (lines 512-535) had incorrect values (tipover, steering/lean tables, torque curve, idle, redline, ratios); it now matches `P:\DZ\vehicles\singletrack\config.cpp:1465-1467, 1515, 1520, 1523, 1535` (Exp numbering at the time: 736-738, 786, 791, 794, 806). Motorbike_01 values were correct, but there were shifted line citations in both columns (Steering, torque, ratios, caster, and scripts), also corrected. Previous copy: `references/config-contract.md.bak-20260919`.
2. **Vanilla motorbike model.cfg file**: Not included in plain text inside extracted Bohemia PBOs. Bone names are only known when explicitly referenced in script (`"drivewheel"`).
3. **Internal C++ physics calculations**: The gyroscopic solver, internal stability algorithm, and Enfusion dynamic balance calculation reside compiled in `DayZDiag_x64.exe` and are not accessible via Enforce Script.
4. **In-game tuning of tyre lateral friction**: Wheel friction tables (`tyreLateralFriction`, `tyreLongitudinalFriction`) are verified in config, but grip feel on wet curves or gravel requires dynamic in-game testing.

FIN-SKILL-DRAFT
