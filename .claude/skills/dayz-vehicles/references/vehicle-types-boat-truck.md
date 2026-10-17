# Vehicle types — boats, trucks (and the ATV gap; motorbikes moved)

> Authored 2026-07-07 (F4) from vanilla `P:\scripts` and `DZ\vehicles\` config on 1.29; every `path:line`
> re-verified and renumbered on 1.30.164078 (`P:\`) on 2026-10-15. Unlabelled cites = 1.30; `1.29:` marks the old
> build. `[UNVERIFIED]` marks anything the source does not pin down. Labels: `../../_shared/dayz-version-policy.md`.

The core skill (`SKILL.md`) is written around the **CarScript** (wheeled) case.
This reference is the delta for the OTHER `Transport` subclasses: boats (a
sibling of Car under `Transport`), trucks (a Car config, not a new class), plus
honest notes on community ATVs. Motorbikes are a vanilla family on 1.30 → `dayz-motorbikes`.

---

## 1. The hierarchy — Car, Boat and Motorbike are SIBLINGS (the central invariant)

```
Pawn (FEATURE_NETWORK_RECONCILIATION) / EntityAI
 └─ Transport            transport.c:66-69
     ├─ Car              car.c:87            → CarScript        carscript.c:1
     ├─ Boat             boat.c:28           → BoatScript       boatscript.c:41
     └─ Motorbike (1.30) motorbike.c:30      → MotorbikeScript  motorbikescript.c:53
```

Before 1.30 the diagram had only `Car` and `Boat`. On 1.30 `Motorbike` (`simulation="motorbike"`,
`P:\bin\config.cpp:1481-1486`) sits next to them; `MotorbikeScript` does **not** inherit `CarScript`. Two-wheel
contract: `dayz-motorbikes`. Full 1.30 vehicle deltas: `dayz-1-30-vehicles.md`.

- `class Transport extends Pawn` (with `FEATURE_NETWORK_RECONCILIATION`) /
  `extends EntityAI` (without) — `scripts\3_game\vehicles\transport.c:66-69`.
- `class Car extends Transport` — `scripts\3_game\vehicles\car.c:87`.
- `class Boat extends Transport` — `scripts\3_game\vehicles\boat.c:28`.
- Scripted layer: `class CarScript` — `scripts\4_world\entities\vehicles\carscript.c` (file starts with
  `enum CarDoorState` at `:1`). `class BoatScript : Boat` — `scripts\4_world\entities\vehicles\boatscript.c:41`.
- Config layer: `class BoatScript: Boat` base at `DZ\vehicles\water\config.cpp:17`;
  concrete `Boat_01_ColorBase: BoatScript` at `:47`. Truck base
  `Truck_01_Base: CarScript` at `DZ\vehicles\wheeled\config.cpp:18429`.

**Key structural fact:** Car and Boat are SIBLINGS, both directly under
`Transport`. They are NOT parent/child. A boat does not inherit anything wheeled,
and a car does not inherit anything buoyant. This governs where each feature
lives:

| Concern | Lives on | Why it matters |
|---|---|---|
| Crew / get-in / flip / fuel plumbing / lights / physics helpers; 1.30: battery contract, contact damage, crash sounds, `ETransportFluid` | **Transport** | shared by cars, boats and bikes |
| Wheels / brakes / handbrake / `CarFluid` (OIL/BRAKE/COOLANT) | **Car** | boat has NONE of these |
| Propeller / buoyancy / `BoatFluid` (fuel only) | **Boat** | car has none of these |

1.30: `enum ETransportFluid` on `Transport` (`transport.c:52-63`) is the shared fluid enum; `CarFluid`
(`car.c:18`), `BoatFluid` (`boat.c:13`) and `MotorbikeFluid` (`motorbike.c:14`) are empty `: ETransportFluid {}`
enums that inherit its values. Generic code uses `GetFluidCapacityScript/GetFluidFractionScript/FillScript(ETransportFluid, ...)`
(`transport.c:440-450`).

A **truck is just a `CarScript` config** with more axles and double wheels — no
new class (see §4).

### 1.1 What Transport OWNS (shared by Car AND Boat)

- Crew system (native): `CrewSize/CrewMemberIndex/CrewMember/CrewDriver/CrewEntry/
  CrewEntryWS/CrewTransform/CrewGetIn/CrewGetOut/CrewDeath` — `transport.c:175-212`.
  `OnDriverEnter/OnDriverExit` hooks — `transport.c:218-224`.
- Lights: `LightIsOn/LightOn/LightOff/LightToggle`, `OnBeforeLightOn`,
  `UpdateLights` — `transport.c:227-255,362`.
- Fuel refill point: `m_fuelPos` from memory point `"refill"`,
  `GetRefillPointPosWS` — `transport.c:112-115,459-472`.
- Flip detection scaffolding: `DetectFlipped` (override per-impl), `IsFlipped`,
  `DetectFlippedUsingSurface` (water-aware, uses `GetWaterDepth`) —
  `transport.c:522-619`.
- Deterministic physics helpers: `ApplyForce/ApplyTorque/ApplyImpulseAt/...` —
  `transport.c:260-275`.
- Get-in door-clearance: `IsAreaAtDoorFree` — `transport.c:844-899`; 1.30 adds the
  `IsAreaAtDoorFreeDiag` wrappers the actions call (`:901-915`) and `CanGetIn()` (`:672-675`).
- `GetVehicleType()` returns `"VehicleTypeUndefined"` on base —
  `transport.c:454-457` (each subclass overrides).
- 1.30: battery trio `NeedElectricitySourceDevice/GetElectricitySourceDevice/GetElectricitySourceDeviceType`
  (`transport.c:821-834`), `OnContact` + `VehicleContactData` (`:315-342`, `:1144-1148`),
  `CheckVitalItemCallback` (`:420-435`), `PartsHealthCheck` (`:437`), `GetWeightCoef` (`:475-478`).

### 1.2 CAR-only additions (`car.c`)

- Wheels API (native): `WheelCount/WheelCountPresent/WheelGetAngularVelocity/
  WheelHasContact/WheelGetSurface/WheelGetWaterState/WheelGetEntity/
  WheelIsLocked` — `car.c:272-348`; 1.30 adds `WheelGetPositionLS(int)` (`car.c:300`). `CarWheelWaterState` enum
  (ON_LAND/IN_WATER/UNDER_WATER) — `car.c:67-72`.
- Brakes + handbrake: `GetBrake/SetBrake/GetHandbrake/SetHandbrake/
  SetBrakesActivateWithoutDriver` — `car.c:200-212`. **Boat has NO brake API.**
- Fluids: `CarFluid` = FUEL/OIL/BRAKE/COOLANT (+USER1-4) — 1.30: `car.c:18` (`: ETransportFluid {}`), values at
  `transport.c:52-63` (Before 1.30 a standalone enum, 1.29: `car.c:18-29`).
- Speedometer `GetSpeedometer` — `car.c:102`. On 1.30 it is **not** Car-only: `Boat` implements
  `GetSpeedometer()` / `GetSpeedometerAbsolute()` (`boat.c:43-56`).
- Flip via wheels: `DetectFlippedUsingWheels` (all wheels must have contact) —
  `car.c:157-178`.
- Scripted get-in actions: `SetActions()` adds `ActionOpenCarDoorsOutside,
  ActionCloseCarDoorsOutside, ActionGetInTransport, ActionSwitchLights,
  ActionVehicleHornShort/Long, ActionPushCar` — `carscript.c:2313-2322`.
  Before 1.30 the horn pair was `ActionCarHornShort/Long` (now `[Obsolete]`, `actioncarhorn.c:69-70,213-214`;
  input aliases `CarHornShortActionInput : VehicleHornShortActionInput {}`, `actioninput.c:758`).

---

## 2. BOAT — the deltas vs the CarScript contract

A boat is authored like a car up to the `Transport`-shared layer, then diverges.
Do NOT copy a car's `Axles`/brakes/`CarFluid` blocks into a boat.

| Area | Boat | vs Car |
|---|---|---|
| Fluids | `BoatFluid` = **FUEL only** in practice — 1.30: `enum BoatFluid : ETransportFluid {}`, `boat.c:13` (Before 1.30 its own `{ FUEL }` enum, 1.29: `boat.c:13-16`) | Car has FUEL/OIL/BRAKE/COOLANT |
| Propulsion | **Propeller** native: `PropellerGetPosition()`, `PropellerGetAngularVelocity()` — `boat.c:125-128` | Car has wheels; no propeller |
| Brakes | **none** (no brake/handbrake protos) | Car has full brake API |
| Steering | `GetSteering/SetSteering(float)` — `boat.c:59-62` (Car's `SetSteering` has an extra `bool unused0`, `car.c:185`) | signature differs |
| Sound ctrl | `BoatSoundCtrl` = ENGINE/SPEED/PLAYER — `boat.c:2-10` (no RPM, no DOORS) | Car adds RPM, DOORS |
| Speedometer | 1.30: `GetSpeedometer()` / `GetSpeedometerAbsolute()` — `boat.c:43-56` (none Before 1.30) | Car native `car.c:102` |
| Get-in | `CrewCanGetThrough/CanReachSeatFromSeat/...` all return **true** — `boatscript.c:205-223` (no doors, open cockpit) | Car GATES by door state |
| Anim | `GetAnimInstance()` = `VehicleAnimInstances.ZODIAC` — `boatscript.c:195-198` | Car uses its own instance |
| Actions | `SetActions()` = only `ActionGetInTransport` + `ActionPushBoat` — `boatscript.c:758-762` | Car adds doors/lights/horn |
| Type | `GetVehicleType()` = `"VehicleTypeBoat"` — `boatscript.c:175-178` | Car returns its own |

### 2.1 Get-in is ALWAYS free on a boat

This is the single most useful contrast for the core-skill reader. The CarScript
preflight #1 exists because a bare `CarScript` inherits `CrewCanGetThrough()` →
`false`, killing the get-in radial. A boat does NOT have that trap: `BoatScript`
overrides `CrewCanGetThrough` (and `CanReachSeatFromSeat`, etc.) to return
**true** unconditionally — `boatscript.c:205-223` — because it is an open cockpit
with no doors. There is no door-state gating to satisfy.

### 2.2 Water physics (config, not script)

On the config base `BoatScript` there is a `class Buoyancy` in place of a car's
`Axles`/brakes — `DZ\vehicles\water\config.cpp:32-40`:

```cpp
class Buoyancy
{
    linearDampeningCoefficient;
    angularDampeningCoefficient;
    linearDragCoefficient  = 0.2;
    quadraticDragCoefficient = 0.2;
    falloffPower = 0.8;
    sinkRate = 0.05;
};
```

Native buoyancy state lives on the owner: `SetBuoyancySubmerged/
GetBuoyancySubmerged` — `transport.c:22-23` (`TransportOwnerState`).

The boat `SimulationModule` (`Boat_01_ColorBase`, block `DZ\vehicles\water\config.cpp:124-180`) has **NO
`Axles` block and NO brakes**. Instead it carries a `class Propeller` (`:167-179`) plus the
usual Engine/Clutch/Gearbox/Throttle/`class Steering`:

```cpp
class Propeller
{
    position; radius; outerRadius; innerRadius;
    efficiency; cavitationThreshold; pitch; width;
    numberOfBlades = 3; mass;
};
```

There is no `Axles`/`Wheels`/brake class anywhere in a boat's `SimulationModule`.
1.30 adds an AI noise class on the base: `CfgVehicles/BoatScript/NoiseBoatEngine { type="sound"; strength=30; }`
(`DZ\vehicles\water\config.cpp:41-45`).

### 2.3 Boat runtime deltas (`boatscript.c`)

- **Auto-stop out of water:** the engine auto-stops if the propeller lifts out of
  the water — `GetWaterDepth(pos) < -0.2 → EngineStop()` in `EOnSimulate` —
  `boatscript.c:393-407`. Also stops on no-fuel — `:361-371`.
- **Decay while unoccupied:** server-side `Timer DecayHealthTick` every 10s, ×4
  if flipped — `boatscript.c:44-47,110-111,534-548`; scaled by `cfggameplay.json`
  `VehicleData.boatDecayMultiplier` (`3_game\cfggameplaydatajson.c:414`).
- **4 water particle effects** attached to memory points `ptcFxFront/Back/Side1/
  Side2` — `boatscript.c:129-144`. Effect classes `EffectBoatWaterFront/Back/Side`
  in `scripts\4_world\entities\effects\boatwatereffects.c:10,116,199` (speed-gated
  emitters; the Back effect reads `PropellerGetAngularVelocity` for speed
  `boatwatereffects.c:183`).
- **Vital `SparkPlug`** gates engine start — `boatscript.c:278-286`
  (`CheckOperationalRequirements`). (Contrast the car's SparkPlug+GlowPlug double
  default in SKILL.md preflight #8 — the boat only ever demands its SparkPlug.)
- **Flip uses the SURFACE detector** (water-aware), not wheels —
  `boatscript.c:490-495`.
- 1.30: `BoatScript.GetHeatComfortOverride()` (`boatscript.c:256`); fluid `*Script` overrides
  (`:225-238`); `CleanupEffects()` is now public (`:844`).

### 2.4 Boat actions (`scripts\4_world\...\actions\`)

- `ActionStartEngineBoat` (continuous) / `ActionStopEngineBoat` (single-use):
  both cast target to `BoatScript`, require `CrewDriver()==player`, and guard
  `GetNetworkMoveStrategy()==PHYSICS` to skip server exec —
  `actionstartengineboat.c:9-70`, `actionstopengineboat.c:1-52`. On execute they
  call `vehicle.OnIgnition()`; on finish, `EngineStart()`.
- `ActionPushBoat extends ActionPushObject` — full-body, syncs `SyncSoundPushBoat`
  on start/end (server) — `actionpushboat.c:17-46`.
- `ActionRepairBoatChassis/Engine extends ActionRepairVehiclePartBase` — repairs
  non-`Engine` zones between WORN and RUINED — `actionrepairboatchassis.c:1-47`.

(These action files are line-for-line unchanged between 1.29 and 1.30.)

---

## 3. (reserved — see §4 for the truck deltas)

---

## 4. TRUCK — a Car config with more axles + double wheels (NOT a new class)

A truck (`Truck_01_Base: CarScript` — `DZ\vehicles\wheeled\config.cpp:18429`) is
NOT a new class. It is a plain `CarScript` config; the deltas are pure config.

| Feature | Truck config | Standard car (contrast) |
|---|---|---|
| Axle count | **3 axles**: `class Front`, `class Middle: Rear`, `class Rear: Rear` — `config.cpp:18569-18673` | 2 axles (Front/Rear) |
| Drivetrain | `drive="DRIVE_642"` (6×4) — `config.cpp:18539`; `class CentralDifferential { type="DIFFERENTIAL_LOCKED"; }` — `:18564-18568`; 1.30 adds `Throttle.slopeCoef=0.75` (config delta) | typically DRIVE_4x2/4x4 |
| Wheel slots | 6 driven hubs `Truck_01_Wheel_1_1..2_3` + 2 spares — `config.cpp:18440-18447` (in `attachments[]`, `:18433`) | 4 hubs |
| Double wheel | Middle+Rear hubs take **`Truck_01_WheelDouble`** — `config.cpp:18128-18146`; its `inventorySlot[]` = the 4 rear slots `1_2,1_3,2_2,2_3`. `width=0.40` (vs single `0.20`), `weight=50000` (vs 30000), better grip/resistance; 1.30 adds the `tyre*` friction keys | single `Truck_01_Wheel` |
| Dual-wheel craft | `Truck_01_Wheel + Truck_01_Wheel → Truck_01_WheelDouble` recipe — `crafttruck01doublewheel.c:27-46`; reverse decraft needs Crowbar/LugWrench → 2 wheels — `decrafttruck01doublewheel.c:36,48-60` | n/a |
| Crew | **4 seats**: Driver + CoDriver + Cargo1 + Cargo2 (`class Crew` w/ `proxyPos="crewCargo1/2"`) — base `config.cpp:265-283`, OffroadHatchback `:1348-1371` region + truck cargo `crewCargo1/2` | 2-4 depending |
| Cargo | `class Cargo { itemsCargoSize[]={10,40}; }` — `config.cpp:18674-18679` | small/none |
| Fuel | `fuelCapacity=120; fuelConsumption=30` — `config.cpp:18506-18507` | ~50 / lower |
| Battery | `TruckBattery` via `GetElectricitySourceDevice()` (`truck_01_base.c:60-75`) | `CarBattery` |

### 4.1 The "double wheel" invariant

**One attachment classname per rear hub covers BOTH physical tyres via the
`Truck_01_WheelDouble.p3d` model** — it is NOT two wheel entities per hub. The
rear axles reference it via `inventorySlot` in the Axles/Wheels blocks
(`config.cpp:18598-18672`). Each Middle/Rear wheel also declares
`wheelHub="wheel_X_Y_damper_land"` plus `animDamper`/`animTurn`/`animRotation`.

So a truck's structural parity is a car's parity (SKILL.md preflight #1-#9) with:
(a) 6 wheel hubs instead of 4, (b) the rear four resolving to one WheelDouble
classname each, (c) 3 axle blocks with a locked central differential, (d) 4-seat
crew. Nothing about get-in, DamageZones, or the vital-igniter chain changes — it
is still `CarScript`.

1.30: abandoned cistern wrecks `Land_Wreck_Truck01_Aban1_Cistern` / `Aban2_Cistern` extend `Well` — they are
drinkable, not vehicle entities (`scripts\4_world\entities\building\wrecks\wreck_truck01_aban1_cistern.c:1-2`).

---

## 5. ATV (quad) — community pattern, one line

Community ATVs are plain `CarScript` with slots hung off `Chassis` instead of
`Body`. (No vanilla ATV in the base game; this is the community convention
observed in the heli/RaG research pass.) Everything else is the standard
`CarScript` contract in the core skill (LFQuad is the worked example throughout
`SKILL.md` — a quad IS a car).

---

## 6. Motorbikes — vanilla on 1.30 (the old gap is closed)

1.30 ships `class Motorbike: Transport` with `simulation="motorbike"` (`P:\bin\config.cpp:1481-1486`),
`MotorbikeScript` (`4_world\entities\vehicles\motorbikescript.c:53`, not a `CarScript`), two vanilla bikes
(`Motorbike_01_ColorBase` / `Motorbike_02_ColorBase : MotorbikeScript`, `P:\DZ\vehicles\singletrack\config.cpp:220`,
`:1401`) and a 2-wheel `SimulationModule` (`Wheels{Front,Rear}` instead of `Axles`). The contract, script API,
actions, sounds and rider animation live in the `dayz-motorbikes` skill.

Before 1.30: no vanilla motorbike and no community source on disk. The warning from that time still holds: do not
derive a two-wheel `SimulationModule` from the car's 4-wheel `Axles` blocks — copy the vanilla bike instead.
