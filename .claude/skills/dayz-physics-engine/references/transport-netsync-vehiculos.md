# 04 — Vehicles and Network Synchronization (Transport / CarScript)
> DayZ Standalone v1.24 · Decompiled Enforce Script · 2026-06-06  
> Source of truth: `<dayz-projects>\scripts\`  
> Anti-confabulation: every class/signature verified with Grep/Read + path:line
>
> **Version note (re-checked on 1.30.164078, `P:\scripts`):** Target: DayZ 1.30 stable. Version labels: ../../_shared/dayz-version-policy.md.
> Written against v1.24; line numbers below are now the 1.30 ones where a `1.30:` tag is shown, otherwise v1.24-era. What changed in 1.30:
> (1) `Motorbike extends Transport` joins the tree; (2) the contact-damage cache moved from `CarScript.OnContact` to `Transport.OnContact`
> with `VehicleContactData` (`m_LocalPos`/`m_Other`/`m_Impulse`) — `CarContactData` is an empty compatibility subclass and its old fields
> and 3-argument constructor are gone; (3) `CarFluid` is `enum CarFluid : ETransportFluid {}` (values unchanged, `CarFluid.FUEL` still
> compiles); (4) `ActionGetInTransport` no longer overrides `OnStartServer` and picks a directional seat animation for single-track
> vehicles; (5) `CarScript.EEHitBy` override is gone (logic on `Transport.EEHitBy`). Pawn/netsync mechanics are unchanged (`pawn.c` is
> byte-identical to 1.29).

---

## Resumen Ejecutivo

DayZ vehicle system is built across three layers:

1. **Transport (native + script)** — base class of all vehicles; inherits from `Pawn` (with active `FEATURE_NETWORK_RECONCILIATION`) or from `EntityAI` in builds without that feature. This is the level where **network magic** occurs: Transport registers `TransportOwnerState` / `TransportMove` and uses `NetworkMoveStrategy.NONE` (own network tick, entirely native — not the player reconciliation system).
2. **Car / Boat / Helicopter (native)** — classes inheriting from Transport and exposing physics API (throttle, steering, brake, fluids, gears). All simulation is native (C++/PhysX); script only calls setters.
3. **CarScript / BoatScript / HelicopterScript (pure script)** — script wrappers adding contact damage, fluids, lights, particles, sound, temperature. Damage caches (`m_ContactCache`) run on server only.

Position/rotation **synchronization** of a Transport is not "ItemBase magic" nor does it use `SetSynchDirty()`; it is an intrinsic property of native `Pawn`/Transport type that the engine synchronizes continuously (position + linear/angular velocities) to all clients.

---

## API Verificada

### Inheritance hierarchy

```
EntityAI
└── Pawn  [FEATURE_NETWORK_RECONCILIATION]          pawn.c:191
    └── Transport                                    1.30: transport.c:9 (#ifdef), :66 (extends Pawn), :67-69 (#else EntityAI)
        ├── Car                                      1.30: car.c:87
        │   └── CarScript                            1.30: carscript.c:94
        │       ├── OffroadHatchback                 offroadhatchback.c:1
        │       ├── Truck_02                         truck_02.c:1
        │       ├── Sedan_02, Van_01, Hatchback_02…
        │       └── Truck_01_Base → Chassis/Cargo/Covered
        ├── Boat                                     1.30: boat.c:28
        │   └── BoatScript                           boatscript.c:41
        │       └── Boat_01_ColorBase                boat_01.c:1
        ├── Motorbike (new in 1.30)                  motorbike.c:30
        │   └── MotorbikeScript                      4_world/entities/vehicles/motorbikescript.c:53
        │       └── Motorbike_01/02_ColorBase        inheritedmotorbikes/motorbike_01.c, motorbike_02.c
        └── Helicopter → HelicopterAuto              helicopter.c:9-14
            └── HelicopterScript                     helicopterscript.c:4
```

Without `FEATURE_NETWORK_RECONCILIATION` the tree is `EntityAI → Transport → …` (without `Pawn`).  
Define active in v1.24, 1.29 and 1.30: `#define FEATURE_NETWORK_RECONCILIATION` at `1_core/defines.c:64` (comment: "Work in progress feature flag for the new reconciliation for players").

---

### Transport — API clave

Line numbers: 1.30 (`P:\scripts\3_game\vehicles\transport.c`).

| Method | Signature | Note |
|--------|-------|------|
| `Synchronize()` | `proto native void Synchronize()` | transport.c:172 — forces state sync when simulation is not running |
| `HasCrew()` | `proto native bool HasCrew()` | transport.c:175 — new in 1.30.164200: true if at least one seat is occupied (replaces a `CrewSize`/`CrewMember` loop); a mod's own `bool HasCrew()` on a Transport subclass now collides with it — rename |
| `CrewSize()` | `proto native int CrewSize()` | transport.c:178 |
| `CrewPositionIndex(int componentIdx)` | `proto native int` | transport.c:182 — maps raycast component index to seat index |
| `CrewMemberIndex(Human player)` | `proto native int` | transport.c:186 — returns -1 if not inside |
| `CrewMember(int posIdx)` | `proto native Human` | transport.c:190 — null if empty |
| `CrewDriver()` | `proto native Human` | transport.c:194 |
| `CrewGetIn(Human player, int posIdx)` | `proto native void` | transport.c:209 |
| `CrewGetOut(int posIdx)` | `proto native Human` | transport.c:212 |
| `CrewDeath(int posIdx)` | `proto native void` | transport.c:215 |
| `ApplyForce/Torque/Impulse…` | `proto void` | transport.c:263+ — deterministic physics |
| `Random/RandomRange/Random01` | `proto native` | transport.c:286-299 — only use in EOnSimulate/EOnPostSimulate |
| `OnContact(zoneName, localPos, other, data)` | `void` | transport.c:318-346 — collision callback; 1.30: has a body (server-only contact cache, see §Damage); v1.24/1.29: empty stub at :252 |
| `OnInput(float dt)` | `void` | transport.c:356 — called after each input step |
| `OnUpdate(float dt)` | `void` | transport.c:362 — every frame (client) / fixed rate (server) |
| `IsTransport()` | `override bool → true` | transport.c:367 |
| `HasDirectionalInOutAction()` / `GetSeatAnimationTypeDirectional(posIdx, dir)` | `bool` / `int` | transport.c:680 / :693 — new in 1.30, single-track boarding side |
| `CrewCanEject/CrewShouldEject(posIdx)` | `bool` | transport.c:714 / :722 — new in 1.30 |
| `GetFluidCapacityScript/GetFluidFractionScript/FillScript(ETransportFluid, …)` | script wrappers | transport.c:443-453 — new in 1.30 |

### Car — script physics API

```cpp
// car.c — todos proto native, verificados
proto native float GetSpeedometer();          // km/h con signo
proto native float GetSteering();             // <-1,1>
proto native void  SetSteering(float value, bool unused0 = false);
proto native float GetThrottle();             // <0,1>
proto native void  SetThrottle(float value);
proto native float GetBrake();
proto native void  SetBrake(float value, float unused0 = 0, bool unused1 = false);
proto native float GetHandbrake();
proto native void  SetHandbrake(float value);
proto native void  SetBrakesActivateWithoutDriver(bool activate = true);
proto native float EngineGetRPM();
proto native bool  EngineIsOn();
proto native void  EngineStart();
proto native void  EngineStop();
proto native int   GetCurrentGear();          // ver enum CarGear (REVERSE, NEUTRAL, FIRST…SIXTEENTH)
proto native int   GetGear();                 // future gear (before applying)
proto native int   GetNeutralGear();
proto native int   GetGearCount();
proto native void  ShiftUp/ShiftDown/ShiftTo(int gear);
proto native CarGearboxType GearboxGetType(); // MANUAL | AUTOMATIC
proto native float GetFluidFraction(CarFluid fluid);  // <0,1>
proto native float GetFluidCapacity(CarFluid fluid);
proto native void  Fill(CarFluid fluid, float amount);
proto native void  Leak(CarFluid fluid, float amount);
proto native void  LeakAll(CarFluid fluid);
proto native int   WheelCount();              // hubs totales
proto native int   WheelCountPresent();       // ruedas realmente instaladas
proto native bool  WheelHasContact(int idx);
proto native vector WheelGetContactPosition(int idx);
```

**Fluidos (CarFluid enum):** `FUEL, OIL, BRAKE, COOLANT, USER1..USER4`. 1.30: `enum CarFluid : ETransportFluid {};` (`car.c:18`) with the values declared once in `ETransportFluid` (`transport.c:52-61`), shared with `BoatFluid` (`boat.c:13`) and `MotorbikeFluid` (`motorbike.c:14`). `CarFluid.FUEL` etc. still compile — vanilla uses them (`carscript.c:850`, `:901`). Before 1.30: a standalone enum at `car.c:17-29`.  
**Marchas (CarGear enum):** `REVERSE, NEUTRAL, FIRST..SIXTEENTH` — `car.c:32` (v1.24: `:43-63`).

### CarController — OBSOLETO

`GetController()` is marked `[Obsolete("Use methods directly on Car")]` — `car.c:435-436` (1.30).  
`CarController` exists as backwards-compatible legacy class: `car.c:461` (1.30). Do not use in new mods.

### EOnPostSimulate — signature and where it runs

```cpp
// carscript.c:207-208 (1.30) — registered in constructor:
SetEventMask(EntityEvent.POSTSIMULATE);
SetEventMask(EntityEvent.POSTFRAME);

// carscript.c:807 (1.30)
override void EOnPostSimulate(IEntity other, float timeSlice)
{
    m_Time += timeSlice;
    if (g_Game.IsServer())
    {
        CheckContactCache();         // applies accumulated collision damage
        m_VelocityPrevTick = GetVelocity(this);
        m_MomentumPrevTick = GetMomentum();
    }
    // fluid checks, FX, brake lights… (server + cliente)
}
```

`EOnPostSimulate` runs on **server and client**. The damage section (`CheckContactCache`) and fluid draining is guarded by `g_Game.IsServer()`. Visual effects are in `!g_Game.IsDedicatedServer()`. The `IsServerOrOwner()` function (carscript.c:2592 on 1.30) returns `IsServer()` for classic networking or `IsOwner()` when `NetworkMoveStrategy == PHYSICS`.

`EOnSimulate` is not in carscript.c or transport.c — physics simulation is entirely **native**. Script does not override physics loop.

### OnInput and OnUpdate

```cpp
// carscript.c:1041 (1.30)
override void OnInput(float dt)  // called by engine for script to apply controls
{
    // (in DIAG: automatic test mode)
    SetThrottle(thrustWanted);
    SetSteering(steeringWanted);
    SetBrake(0.0);
    SetHandbrake(0.0);
}

// carscript.c:1210 (1.30)
override void OnUpdate(float dt)
{
    Human driver = CrewDriver();
    if (driver && !driver.IsControllingVehicle())
        if (driver.IsAlive())
            SetBrake(0.5);     // brakes if driver is unconscious
}
```

Under normal conditions (without test code), `CarScript.OnInput` only acts if driver is test AI. Human driver controls vehicle via own input (native physics reads player input directly).

---

## Config CfgVehicles [WEB]

Config structure for vehicles uses `SimulationModule` and is completely declarative (does not exist in decompiled scripts; lives in `config.cpp` files of P3D addons).  
Reference: https://community.bistudio.com/wiki/DayZ:Vehicle_Configuration

Relevant classes confirmed by naming convention in scripts (indirectly verified):

- `class SimulationModule` — vehicle-wide PhysX physics parameters
  - `axles[]` — axle list (front/rear), each with `wheels[]`
  - Inside each wheel: `steerAngle`, `frictionCoef`, `dampingRate`
  - `engine {}` — `torque[][]`, `RPMMin`, `RPMIdle`, `RPMMax`, `RPMRedline`
  - `gearbox {}` — type (MANUAL/AUTOMATIC), gear ratios
  - `clutch {}` — `maxRPMDrop`, `engagingSpeed`
  - `brakes {}` — `torqueMax`
  - `aerodynamics {}` — `dragCoef`, `frontalArea`
  - `drive` — `DRIVE_AWD`, `DRIVE_FWD`, `DRIVE_RWD`
- `inventorySlots[]` — wheel slots (for `CarWheel` attachment)
- `attachments[]` — `CarRadiator`, `CarBattery`, `SparkPlug`, `GlowPlug`
- `dmgZones` — damage zones (Engine, FuelTank, wheels, fenders, etc.)

Enums `CarFluid` (FUEL/OIL/BRAKE/COOLANT; 1.30: derived from `ETransportFluid`) and `CarGear` do exist in scripts and determine fluid and gearbox behavior via script.

---

## ⚠️ Transport Netsync — Why It Replicates and ItemBase Does Not

### The fundamental difference

```
Transport extends Pawn extends EntityAI    ← transport.c:66 (1.30)
ItemBase extends EntityAI                  ← NO es Pawn
```

**`Pawn`** is the native type treated by the engine as a "possessed" or controlled entity with continuous network movement. Class `Pawn` has:

- `GetOwnerStateType()` → returns `PawnOwnerState` — contains position/velocities in a compressed snapshot for desync correction — `pawn.c:238`
- `GetMoveType()` → returns `PawnMove` — movement packet sent each tick — `pawn.c:246`
- `GetNetworkMoveStrategy()` → returns active strategy (`NONE`, `LATEST`, `PHYSICS`) — `pawn.c:218`

**Transport** sobreescribe estos tipos:

```cpp
// transport.c:160-169 (1.30)
protected override event typename GetOwnerStateType() { return TransportOwnerState; }
protected override event typename GetMoveType()        { return TransportMove;       }
```

`TransportOwnerState` has `SetWorldTransform/GetWorldTransform`, `SetLinearVelocity/GetLinearVelocity`, `SetAngularVelocity/GetAngularVelocity` (and buoyancy) — `transport.c:11-30`. These fields are what the native engine serializes and sends to proxy clients on each vehicle network tick.

The comment in transport.c:65 (1.30) explicitly states:
```
//! Uses NetworkMoveStrategy.NONE
class Transport extends Pawn
```
`NetworkMoveStrategy.NONE` means that **it does not use the client reconciliation system** (the same one used by player). Instead, engine uses its own vehicle physics synchronization mechanism (server owner, proxy on clients), replicating position + velocities continuously to all proxies.

### Why ItemBase with dynamic dBody does NOT replicate automatically

- `ItemBase` inherits from `EntityAI`, not `Pawn`. The engine does not know it must treat its transform as "continuous network state".
- The `SetSynchDirty()` / `RegisterNetSyncVariable*` system is a discrete state RPC mechanism (per value change), not a continuous position/velocity stream.
- A dynamic `dBody` on `ItemBase` has physics on server but client does not receive transform in real time — only updates when item enters its area of interest and on periodic resync events.
- **That is why the "provisional PASS" behavior** of project (client saw stone roll in S1 test) may be an artifact of low latency + high resync frequency on local network / singleplayer, or of some EntityAI replication mechanism undocumented in scripts.

### Relevant network methods in Transport

```cpp
// transport.c:171-172 (1.30)
//! Synchronizes car's state in case the simulation is not running.
proto native void Synchronize();
```

This `Synchronize()` is a state "force push" for when vehicle is not in active simulation (e.g.: just activated, or driver exited and car remained still). Confirms normal synchronization is continuous by engine, and this is a manual override.

```cpp
// transport.c:813 (1.30)
void SetEngineZoneReceivedHit(bool pState)
{
    m_EngineZoneReceivedHit = pState;
    SetSynchDirty();  // <- uses netSyncVar system for discrete state
}
```

Transport uses BOTH systems: native position stream (via Pawn) AND `SetSynchDirty()` for state variables (lights, engine damage, etc.).

---

## Contact damage (Transport → CarScript / MotorbikeScript)

### OnContact + CheckContactCache

```cpp
// [EXACT] 1.30: P:\scripts\3_game\vehicles\transport.c:318-346 (Transport, not CarScript)
void OnContact(string zoneName, vector localPos, IEntity other, Contact data)
{
    if (GetGame().IsServer())
    {
        if (m_ContactCache.Count() == 0)  // first zone per frame only
        {
            array<ref VehicleContactData> ccd = new array<ref VehicleContactData>();
            m_ContactCache.Insert(zoneName, ccd);
            float momentumDelta = GetMomentum() - m_MomentumPrevTick;
            float dot = vector.Dot(m_VelocityPrevTick.Normalized(), GetVelocity(this).Normalized());
            if (dot < 0) momentumDelta = m_MomentumPrevTick;
            VehicleContactData contactData = new VehicleContactData();
            contactData.SetData(localPos, other, momentumDelta);
            ccd.Insert(contactData);
        }
    }
}
```

`OnContact` only runs on server. Uses **momentum variation** (momentum delta = change in velocity × mass) as proxy for collision impulse; the `Contact data` struct has `data.Impulse`, used only in the DIAG log. Before 1.30 this body was `CarScript.OnContact` (1.29: `carscript.c:1453-1480`) and inserted `new CarContactData(localPos, other, momentumDelta)`; that 3-argument constructor and the public fields `localPos`/`other`/`impulse` are gone in 1.30 (`CarContactData : VehicleContactData {}`, `carscript.c:53`; `VehicleContactData` fields `m_LocalPos`/`m_Other`/`m_Impulse` and `SetData(...)`, `transport.c:1153-1165`). `m_ContactCache` itself moved to `Transport` (`transport.c:90`).

Actual processing occurs in `CheckContactCache()` called from `EOnPostSimulate` (`CarScript`: `carscript.c:1279`; `MotorbikeScript`: `motorbikescript.c:1267` with `MOTORBIKES_CONTACT_DMG_*` thresholds 10/25/40/80, `constants.c:898-901`):

```cpp
// carscript.c:1279+ (1.30; v1.24: 1482-1588)
void CheckContactCache()
{
    float dmg = Math.AbsInt(data[0].m_Impulse * m_dmgContactCoef);  // m_dmgContactCoef = 0.058 (carscript.c:224); before 1.30: data[0].impulse
    float crewDmgBase = Math.AbsInt((data[0].m_Impulse / dBodyGetMass(this)) * 1000 * m_dmgContactCoef);
    
    if (dmg < GameConstants.CARS_CONTACT_DMG_MIN) continue;    // minimum threshold
    
    if (dmg < GameConstants.CARS_CONTACT_DMG_THRESHOLD)
        SynchCrashLightSound(true);    // choque leve
    else
    {
        DamageCrew(crewDmgBase);       // daño a tripulantes
        SynchCrashHeavySound(true);    // choque fuerte
    }
    
    ProcessDirectDamage(DamageType.CUSTOM, null, zoneName, "EnviroDmg", "0 0 0", dmg, pddfFlags);
}
```

**Damage zones** (Engine, FuelTank, fenders, front, back) are mapped from model memory points (`dmgZone_engine`, `dmgZone_front`, etc.) — carscript.c:369-375 (1.30; v1.24: 393-426).

### Daño a tripulación

`DamageCrew(float dmg)` — carscript.c:1374 (1.30; v1.24: 1592). If `dmg > CARS_CONTACT_DMG_KILLCREW` → `player.SetHealth(0.0)`. Otherwise, calculates shock + HP via `Math.InverseLerp`.

### EEHitBy in Transport

`Transport.EEHitBy` activates `SetEngineZoneReceivedHit(dmgZone == "Engine")` — transport.c:144-151 (1.30; v1.24: 84-89). This flag is synchronized via `SetSynchDirty()`. 1.30 also schedules the light refresh there; the separate `CarScript.EEHitBy` override (1.29: `carscript.c:531-537`) is gone.

---

## Patrones Vanilla

### OnInput: fully native physics, script only reads/writes

The standard pattern for mods that want to modify driving behavior:

```cpp
override void OnInput(float dt)
{
    super.OnInput(dt);         // important: let base logic run
    // leer state: GetThrottle(), GetSteering()
    // modificar: SetThrottle(newVal), SetSteering(newVal)
}
```

### Fluids via script

```cpp
// Fill on debug spawn (offroadhatchback/boat_01 pattern). CarFluid.FUEL still valid on 1.30 (CarFluid : ETransportFluid);
// transport-generic code can use FillScript/GetFluidCapacityScript(ETransportFluid.FUEL, …) (transport.c:443-453).
float amount = GetFluidCapacity(CarFluid.FUEL);
Fill(CarFluid.FUEL, amount);

// Chequeo de nivel:
if (GetFluidFraction(CarFluid.FUEL) <= 0)
    EngineStop();

// Leak progresivo:
if (m_FuelTankHealth < GameConstants.DAMAGE_DAMAGED_VALUE)
    LeakFluid(CarFluid.FUEL);  // wrapper that calls Leak() with rate
```

### Engine temperature (UTSource pattern)

All vanilla cars (OffroadHatchback, Truck_02, Van_01, etc.) instantiate `UniversalTemperatureSource` in `EEInit` only on server/SP, update it in `EOnPostSimulate`, and activate/deactivate it in `OnEngineStart/Stop`. The client does not touch UTSource.

### Boarding completo

1. Player looks at the vehicle → raycast hits model component.
2. `ActionGetInTransport.ActionCondition` verifies: `transport.CanGetIn()`, `CrewPositionIndex(componentIndex)` >= 0, empty seat, door area free — actiongetintransport.c:26-69 (1.30).
3. In `Start()`: `player.StartCommand_Vehicle(transport, crewIndex, seat)` → creates `HumanCommandVehicle` (`:104`); on 1.30 `seat` comes from `GetSeatAnimationTypeDirectional(crewIndex, direction)` when `HasDirectionalInOutAction()` (`:81-100`), else `GetSeatAnimationType`.
4. Before 1.30 only: on server (`OnStartServer`) it updated car lights (1.29: `actiongetintransport.c:100-109`); 1.30 has no `OnStartServer` override there.
5. To exit: `ActionGetOutTransport` + `OnVehicleJumpOutServer` calculates speed damage when disembarking — carscript.c:951 (1.30; v1.24: 1217-1291).

---

## Gotchas

1. **`OnContact` runs only on server** — transport.c:320 (1.30; v1.24/1.29: carscript.c:1456). There is no collision callback on client for cars.
2. **`EOnPostSimulate` runs on both** — but most logic is guarded by `g_Game.IsServer()`.
3. **`Random/RandomRange/Random01` only in EOnSimulate/EOnPostSimulate** — transport.c:282-303 (1.30). Using them outside these callbacks breaks determinism.
4. **`GetController()` is obsolete** — use direct `Car` methods (SetThrottle, SetSteering, etc.).
5. **`IsServerOrOwner()`** — carscript.c:2592 (1.30). With new networking (`NetworkMoveStrategy.PHYSICS`), the "owner" (client driving) also executes the simulation. With classic networking, only the server.
6. **`dBodyApplyImpulseAt` in ActionPushCar** — actionpushcar.c:52. Uses the global dBody API (not Transport's); the result propagates because the physics body does exist on server and Transport replicates it.
7. **`Synchronize()`** — call manually when the vehicle transitions from static to active state, to force snapshot.
8. **The cached "impulse" is `momentumDelta`** — `VehicleContactData.m_Impulse` (1.30; before 1.30 `CarContactData.impulse`) is not `data.Impulse` from the Contact struct; it is `GetMomentum() - m_MomentumPrevTick`.
9. **`FEATURE_NETWORK_RECONCILIATION` is defined** — transport.c:9 / :66-69 (1.30) show that if NOT defined, Transport inherits directly from EntityAI. It is `#define`d at `1_core/defines.c:64` in v1.24, 1.29 and 1.30.
10. **Driverless cars**: `SetBrakesActivateWithoutDriver(true)` — car.c:212 (1.30). If the driver loses control, `OnUpdate` applies brake=0.5.

---

## What DOES NOT Exist / Typical Confabulations

- **`CarScript` with `SimulationModule` via script**: FALSE. `SimulationModule` is strictly config (CfgVehicles). There is no way to create wheel/axle physics parameters via Enforce Script code.
- **Custom Transport without model with sim module**: FALSE. You need a P3D model with appropriate geometry (PhysX collision, LODs, memory points) registered in config.
- **`GetCrewIndex()`**: DOES NOT EXIST under that name. Correct method is `CrewPositionIndex(int componentIdx)` (transport.c:182) or `CrewMemberIndex(Human player)` (transport.c:186) (1.30 lines).
- **`EFluidType` enum for vehicles**: DOES NOT EXIST under that name. It is `CarFluid` (car.c:18), `BoatFluid` (boat.c:13) and, new in 1.30, `MotorbikeFluid` (motorbike.c:14), all derived from `ETransportFluid` (transport.c:52). `EFluidType` may exist for other systems (water, fireplace) but not for cars.
- **`EOnSimulate` in CarScript**: NOT overridden in any car script class. Physics simulation is 100% native.
- **`HelicopterScript` with full physics**: `HelicopterScript.EOnPostSimulate` is empty — helicopterscript.c:11-13. All flight is native in `HelicopterAuto`.
- **BoatScript inherits from BoatScript**: Confirm — `BoatScript : Boat` (boatscript.c:41), and `Boat extends Transport` (boat.c:28 on 1.30). Class `BoatScript` DOES exist (contrary to what some assume).

---

## Relevance for LF_RollingStone

### ⚠️RELEVANT: Why Transport replicates and your ItemBase does not

The exact reason is that `Transport extends Pawn` and the engine treats all Pawns with a continuous stream of `TransportOwnerState` (worldTransform + linearVelocity + angularVelocity) to all proxies. `ItemBase` does not inherit from `Pawn`, so the engine does not generate that stream.

The provisional "PASS" of S1 (client sees stone roll) can be explained by:
- In **singleplayer**: no network, the same process sees everything.
- In **local MP / low latency**: EntityAI performs periodic position resync when changed sufficiently (engine automatic `SetSynchDirty` mechanism when moving entities). This resync is infrequent (possibly every N ms or when server decides) and does not send velocities, so client interpolates poorly or sees jumps.

### ⚠️RELEVANT: Real options for LF_RollingStone

1. **Option A (Plan S3 — ThrowPhysically / DYNAMICITEM)**: Turn stone into a `Transport` or leverage some "throwable" entity that engine does synchronize. Risky without full sim module mod.
2. **Option B (Manual position RPC)**: On each server tick, read `GetPosition()` and `GetVelocity()` from dBody and send an RPC to client to move stone manually (`SetPosition` + `dBodySetVelocity`). Expensive but correct.
3. **Option C (Inherit from Transport)**: Create a class inheriting from `Transport` (not from `ItemBase`), register with minimal sim module in config. Would have automatic netsync. The issue is Transport requires CfgVehicles config with SimulationModule + suitable P3D model.
4. **Option D (Native dBody throttle)**: Empirically confirm whether engine resyncs EntityAI at an interval tolerable for gameplay. If frequency is ~100ms, it may be "good enough" for S1.

### ⚠️RELEVANT: ActionPushCar as reference for LFRS_ActionPush

`ActionPushCarCB.ApplyForce` uses exactly the pattern that `LFRS_ActionPush` should use:
```cpp
dBodyApplyImpulseAt(car, impulse, car.ModelToWorld(car.GetEnginePos()));
// actionpushcar.c:52
```
Adapted for the stone:
```cpp
dBodyApplyImpulseAt(stone, impulse, stone.GetPosition());
```
The impulse is calculated as `bodyMass × force × coef × direction`. This code is already on server (action runs on server) and push physically moves dBody on server. Client will see the effect only if netsync exists (known issue).

### ⚠️RELEVANT: OnContact for stone EOnContact

Transport `OnContact` (running on server only) is the same pattern as ItemBase `EOnContact`. The difference is that for Transport there are well-defined callbacks with zone name; for ItemBase the pattern is `EOnContact(IEntity other, Contact data)`. Damage architecture (accumulate in cache, process in PostSimulate) is reusable for stone if player impact damage is desired.

---

## Fuentes

| File | Description |
|---------|-------------|
| `3_game/vehicles/transport.c` | Transport base class: crew, forces, netsync state types |
| `3_game/vehicles/car.c` | Car proto-native API: steering/throttle/brake/gear/fluid |
| `3_game/vehicles/boat.c` | Boat proto-native API (confirms BoatScript exists) |
| `3_game/vehicles/helicopter.c` | Helicopter + HelicopterAuto (native auto-hover) |
| `3_game/entities/pawn.c` | Pawn, PawnOwnerState, PawnMove, NetworkMoveStrategy enum |
| `1_core/defines.c` | FEATURE_NETWORK_RECONCILIATION defined (v1.24, 1.29, 1.30: `:64`) |
| `4_world/entities/vehicles/carscript.c` | Full CarScript: contact processing, fluids, lights, EOnPostSimulate (1.30: contact cache filled by `Transport.OnContact`) |
| `3_game/vehicles/motorbike.c`, `4_world/entities/vehicles/motorbikescript.c` | 1.30 Motorbike / MotorbikeScript |
| `4_world/entities/vehicles/boatscript.c` | BoatScript: confirms existence and structure |
| `4_world/entities/vehicles/helicopterscript.c` | HelicopterScript: empty EOnPostSimulate |
| `4_world/entities/vehicles/inheritedcars/offroadhatchback.c` | UTSource pattern, GetSeatAnimationType |
| `4_world/entities/vehicles/inheritedcars/truck_02.c` | Truck pattern with temperature |
| `4_world/entities/vehicles/inheritedboats/boat_01.c` | Boat_01: fluids, seats |
| `4_world/classes/useractionscomponent/actions/interact/actiongetintransport.c` | Full boarding |
| `4_world/classes/useractionscomponent/actions/continuous/actionpushcar.c` | dBodyApplyImpulseAt as pattern |
| [WEB] community.bistudio.com/wiki/DayZ:Vehicle_Configuration | CfgVehicles SimulationModule |
