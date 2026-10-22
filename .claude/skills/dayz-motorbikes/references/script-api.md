# Motorbike Script API (`Motorbike.c` and `MotorbikeScript.c`)

> Target: DayZ 1.30 stable (1.30.164200). Version labels: `../../_shared/dayz-version-policy.md`. Script, `.agf`,
> `.asi` and `.ast` line numbers were read on 1.30 Exp 164014 and are unchanged on stable (identical line counts,
> sampled cites re-read 2026-10-15); `config.cpp` cites point at stable `P:\DZ` / `P:\bin`.

This document compiles the complete native interface (`proto native`) of `Motorbike.c`, simulation callbacks, the `MotorbikeScript.c` lifecycle, network synchronization variables, and the differences table relative to `CarScript.c`.

---

## 1. Proto native Signatures of Motorbike.c

All native functions declared in `P:\scripts\3_Game\Vehicles\Motorbike.c` (in-game `scripts\3_Game\Vehicles\Motorbike.c`):

```c
// P:\scripts\3_Game\Vehicles\Motorbike.c:100
proto native float GetSteering();
// P:\scripts\3_Game\Vehicles\Motorbike.c:103
proto native void SetSteering(float value);

// P:\scripts\3_Game\Vehicles\Motorbike.c:106
proto native bool CanPickUp();
// P:\scripts\3_Game\Vehicles\Motorbike.c:109
proto native void PickUp();

// P:\scripts\3_Game\Vehicles\Motorbike.c:112
proto native bool IsFallen();
// P:\scripts\3_Game\Vehicles\Motorbike.c:115
proto native bool FallOver();

// P:\scripts\3_Game\Vehicles\Motorbike.c:118
proto native float GetThrottle();
// P:\scripts\3_Game\Vehicles\Motorbike.c:121
proto native void SetThrottle(float value);

// P:\scripts\3_Game\Vehicles\Motorbike.c:124
proto native int GetClutch();
// P:\scripts\3_Game\Vehicles\Motorbike.c:127
proto native void SetClutch(float value);

// P:\scripts\3_Game\Vehicles\Motorbike.c:130
proto native float GetBrake();
// P:\scripts\3_Game\Vehicles\Motorbike.c:133
proto native void SetBrake(float value);

// P:\scripts\3_Game\Vehicles\Motorbike.c:136
proto native float GetHandbrake();
// P:\scripts\3_Game\Vehicles\Motorbike.c:139
proto native void SetHandbrake(float value);

// P:\scripts\3_Game\Vehicles\Motorbike.c:142
proto native float GetKickstand();
// P:\scripts\3_Game\Vehicles\Motorbike.c:145
proto native void SetKickstand(float value);

// P:\scripts\3_Game\Vehicles\Motorbike.c:148
proto native float EngineGetRPMMin();
// P:\scripts\3_Game\Vehicles\Motorbike.c:151
proto native float EngineGetRPMIdle();
// P:\scripts\3_Game\Vehicles\Motorbike.c:154
proto native float EngineGetRPMMax();
// P:\scripts\3_Game\Vehicles\Motorbike.c:157
proto native float EngineGetRPMRedline();
// P:\scripts\3_Game\Vehicles\Motorbike.c:160
proto native float EngineGetRPM();
// P:\scripts\3_Game\Vehicles\Motorbike.c:163
proto native bool EngineIsOn();
// P:\scripts\3_Game\Vehicles\Motorbike.c:166
proto native void EngineStart();
// P:\scripts\3_Game\Vehicles\Motorbike.c:169
proto native void EngineStop();

// P:\scripts\3_Game\Vehicles\Motorbike.c:172
proto native vector GetEnginePos();
// P:\scripts\3_Game\Vehicles\Motorbike.c:175
proto native void SetEnginePos(vector pos);

// P:\scripts\3_Game\Vehicles\Motorbike.c:178
proto native int GetCurrentGear();
// P:\scripts\3_Game\Vehicles\Motorbike.c:181
proto native int GetGear();
// P:\scripts\3_Game\Vehicles\Motorbike.c:184
proto native int GetNeutralGear();
// P:\scripts\3_Game\Vehicles\Motorbike.c:187
proto native int GetGearCount();
// P:\scripts\3_Game\Vehicles\Motorbike.c:190
proto native void ShiftUp();
// P:\scripts\3_Game\Vehicles\Motorbike.c:193
proto native void ShiftTo(int gear);
// P:\scripts\3_Game\Vehicles\Motorbike.c:196
proto native void ShiftDown();

// P:\scripts\3_Game\Vehicles\Motorbike.c:199
proto native CarGearboxType GearboxGetType();
// P:\scripts\3_Game\Vehicles\Motorbike.c:202
proto native CarAutomaticGearboxMode GearboxGetMode();

// P:\scripts\3_Game\Vehicles\Motorbike.c:205
proto native bool WheelIsAnyLocked();
// P:\scripts\3_Game\Vehicles\Motorbike.c:212
proto native float WheelGetAngularVelocity(int wheelIdx);
// P:\scripts\3_Game\Vehicles\Motorbike.c:219
proto native bool WheelHasContact(int wheelIdx);
// P:\scripts\3_Game\Vehicles\Motorbike.c:226
proto native vector WheelGetContactPosition(int wheelIdx);
// P:\scripts\3_Game\Vehicles\Motorbike.c:233
proto native vector WheelGetPositionLS(int wheelIdx);
// P:\scripts\3_Game\Vehicles\Motorbike.c:240
proto native vector WheelGetContactNormal(int wheelIdx);
// P:\scripts\3_Game\Vehicles\Motorbike.c:247
proto native vector WheelGetDirection(int wheelIdx);
// P:\scripts\3_Game\Vehicles\Motorbike.c:254
proto native SurfaceInfo WheelGetSurface(int wheelIdx);
// P:\scripts\3_Game\Vehicles\Motorbike.c:261
proto native CarWheelWaterState WheelGetWaterState(int wheelIdx);
// P:\scripts\3_Game\Vehicles\Motorbike.c:268
proto native EntityAI WheelGetEntity(int wheelIdx);
// P:\scripts\3_Game\Vehicles\Motorbike.c:275
proto native bool WheelIsLocked(int wheelIdx);
// P:\scripts\3_Game\Vehicles\Motorbike.c:278
proto native int WheelCount();
// P:\scripts\3_Game\Vehicles\Motorbike.c:281
proto native int WheelCountPresent();

// P:\scripts\3_Game\Vehicles\Motorbike.c:288
proto native float GetFluidCapacity(MotorbikeFluid fluid);
// P:\scripts\3_Game\Vehicles\Motorbike.c:296
proto native float GetFluidFraction(MotorbikeFluid fluid);
// P:\scripts\3_Game\Vehicles\Motorbike.c:299
proto native void Leak(MotorbikeFluid fluid, float amount);
// P:\scripts\3_Game\Vehicles\Motorbike.c:302
proto native void LeakAll(MotorbikeFluid fluid);
// P:\scripts\3_Game\Vehicles\Motorbike.c:305
proto native void Fill(MotorbikeFluid fluid, float amount);
```

---

## 2. Engine-Script Events and Callbacks in Motorbike.c

- `protected override event typename GetOwnerStateType()` (`Motorbike.c:33-36`): Returns `MotorbikeOwnerState`.
- `protected override event typename GetMoveType()` (`Motorbike.c:39-42`): Returns `MotorbikeMove`.
- `float GetSpeedometer()` (`Motorbike.c:45-52`): Returns longitudinal speed by multiplying projection by 3.6 (km/h).
- `float GetSpeedometerAbsolute()` (`Motorbike.c:55-58`): `Math.AbsFloat(GetSpeedometer())`.
- `override bool IsAreaAtDoorFree(...)` (`Motorbike.c:60-68`): Unconditionally returns `true` (motorbikes have no doors).
- `bool OnBeforeEngineStart()` (`Motorbike.c:312-316`): Callback evaluated before allowing engine start. `MotorbikeScript.c:1573-1579` checks that operational state is `EMotorbikeOperationalState.OK`.
- `void OnEngineStart()` (`Motorbike.c:321`, empty): invoked upon starting. The override in `MotorbikeScript.c:1018-1039` updates headlights and plays `START_OK`.
- `void OnEngineStop()` (`Motorbike.c:326`): Invoked upon engine stopping.
- `void OnGearChanged(int newGear, int oldGear)` (`Motorbike.c:334`): Invoked upon changing gear.
- `void OnFluidChanged(MotorbikeFluid fluid, float newValue, float oldValue)` (`Motorbike.c:344`): Invoked on the owner upon fluid changes.
- `float OnSound(MotorbikeSoundCtrl ctrl, float oldValue)` (`Motorbike.c:354-358`): Modulates sound controllers (`ENGINE`, `RPM`, `SPEED`, `PLAYER`).
- `void OnStabilityStart()` (`Motorbike.c:363`): Invoked when the motorbike regains balance.
- `void OnStabilityStop()` (`Motorbike.c:368`): Invoked when the motorbike falls to the ground.

---

## 3. Lifecycle and Synchronization in MotorbikeScript.c

### Constructor (`MotorbikeScript.c:105-148`)
1. Activates event mask: `SetEventMask(EntityEvent.POSTSIMULATE | EntityEvent.POSTFRAME);` (`line 111`).
2. Instantiates `m_LightsComponent = VehicleLightsComponent(this);` and registers selections:
   - `"FrontOn"`, `"FrontOff"`, `"BrakeOn"`, `"BrakeOff"`, `"TailOn"`, `"TailOff"`, `"DashboardOn"`, `"DashboardOff"` on
     default indices 0-3 (`MotorbikeScript.c:114-122`). Since 1.30.164200 the vanilla subclasses re-register Front/Brake on
     their own `hiddenSelections[]` slots (`Motorbike_01.c:19-22`: 3/2; `Motorbike_02.c:20-23`: 4/6).
   - Registers base profiles: `VehicleLightProfileFront` and `VehicleLightProfileRear`.
3. Instantiates `m_HornComponent = VehicleHornComponent(this);` in `OFF` mode.
4. Initializes particles via `InitializeParticles()` (`lines 174-205`).
5. Deploys kickstand: `SetKickstand(1);` (`line 141`).
6. Registers network synchronization:
   ```c
   RegisterNetSyncVariableBool("m_BrakesArePressed");
   RegisterNetSyncVariableBoolSignal("m_PlayCrashSoundLight");
   RegisterNetSyncVariableBoolSignal("m_PlayCrashSoundHeavy");
   RegisterNetSyncVariableInt("m_HornComponent.m_Mode", VehicleHornMode.OFF, VehicleHornMode.COUNT);
   ```

### Central Economy and Persistence
- `override void EEOnCECreate()` (`MotorbikeScript.c:440-447`): Assigns random initial fuel between 0.0 and 35% of capacity:
  `float maxVolume = GetFluidCapacity( MotorbikeFluid.FUEL );`, `float amount = Math.RandomFloat(0.0, maxVolume * 0.35 );` and `Fill( MotorbikeFluid.FUEL, amount );` (`lines 443-446`).
- **Persistence (`OnStoreSave`/`OnStoreLoad`)**: `MotorbikeScript.c` does **NOT** implement these methods; it delegates directly to standard serialization of `Transport` and `EntityAI`.

---

## 4. Relevant Global Constants (`constants.c`)

Located in `P:\scripts\3_Game\constants.c`:
- `ENVIRO_HEATCOMFORT_MOTORBIKE_SPEED_MIN = 20.0;` (`line 792`)
- `ENVIRO_HEATCOMFORT_MOTORBIKE_SPEED_MAX = 100.0;` (`line 793`)
- `ENVIRO_HEATCOMFORT_MOTORBIKE_TARGET_MAX = 0.8;` (`line 794`)
- `MOTORBIKES_CONTACT_DMG_THRESHOLD_SMALL = 25.0;` (`line 898`)
- `MOTORBIKES_CONTACT_DMG_THRESHOLD_MID = 40.0;` (`line 899`)
- `MOTORBIKES_CONTACT_DMG_MIN = 10.0;` (`line 900`)
- `MOTORBIKES_CONTACT_DMG_KILLCREW = 80.0;` (`line 901`)

---

## 5. Table of What Does NOT Exist in Motorbikes vs CarScript

| Car Subsystem | Status in Motorbike (`MotorbikeScript`) | Alternative or Behavior |
|---|---|---|
| `class Axles` | **Nonexistent** | Flat `class Wheels { class Front; class Rear; }` |
| Doors (`CarDoor`) | **Nonexistent** | No doors; `IsAreaAtDoorFree` always returns `true` (`Motorbike.c:60-68`) |
| Radiator / Coolant circuit | **Nonexistent / Non-vital** | `IsVitalRadiator()` returns `false` (`MotorbikeScript.c:321-324`). No temperature gauge in HUD (`MotorBikeHud.c:191-194`) |
| Oil / Brake fluid circuit | Declared in config, unused in script | `MotorbikeScript` declares capacities (`P:\DZ\vehicles\singletrack\config.cpp:30-33`), but `MotorbikeScript.c` only uses `MotorbikeFluid.FUEL` |
| Battery mandatory for ignition | **Depends on override** | `Transport.NeedElectricitySourceDevice()` returns `true` (`Transport.c:830-833`) and `MotorbikeScript` does not change it; `Motorbike_01`/`_02` override it to `false` (`Motorbike_01.c:30-33`, `Motorbike_02.c:31-34`). A bike inheriting from `MotorbikeScript` must override it |
| Door actions (`ActionOpenCarDoors`) | **Nonexistent** | In `Motorbike_01` flip-up seat `SeatDriver` exists |
| Parking handbrake | **Nonexistent** | Handbrake channel actuates front brake (`SetHandbrake`, `Motorbike.c:135-139`; `Brakes.Front.input = 1`, `P:\DZ\vehicles\singletrack\config.cpp:296`); handbrake indicator light is hidden in HUD (`MotorBikeHud.c:196`) |
| Symmetrical 4-wheel steering | **Nonexistent** | Simulation with dynamic lean (`maxLeanAngle[]`) and `tipoverAngle` |
| Dismount in gear without stalling | **Nonexistent** | If driver exits in gear, engine stops (`OnDriverExit` → `EngineStop()`, `MotorbikeScript.c:217-222`) |
| `IsMoving()` (1.30.164200) | **Not implemented** | New base `Transport.IsMoving()` logs `Error("IsMoving() not implemented")` and returns `false` (`Transport.c:819-823`); only `CarScript` overrides it (`> 3.5` km/h, `CarScript.c:2221-2224`). On a bike read `GetSpeedometerAbsolute()` (`Motorbike.c:55-58`) or add your own `override bool IsMoving()` |
| Detaching parts while running (1.30.164200) | **Blocked for every attachment** | `MotorbikeScript.CanReleaseAttachment` returns `false` while `EngineIsOn() \|\| HasCrew()` (`MotorbikeScript.c:612-625`; `HasCrew()` is the new `proto native` on `Transport`, `Transport.c:175`). Cars block only while `EngineIsOn() && IsMoving()` (`CarScript.c:638-667`). Own-slot exemption: `../SKILL.md` §DayZ 1.30.164200 changes |

### 1.30.164200 additions on `Transport` that bikes inherit

- `proto native bool HasCrew()` — "true if at least one member of crew is present" (`P:\scripts\3_game\vehicles\transport.c:175`).
- `bool IsMoving()` — base logs an error; see the table row above (`transport.c:819-823`).
- Naming rule: a bike class (or `modded class MotorbikeScript`) written for 164078 that declares its own `bool HasCrew()`
  or `bool IsMoving()` now collides with the native / shadows the script member; rename it with a mod prefix or make it a
  real `override`. Scanner kinds `transport-hascrew` / `transport-ismoving` in `dayz-130-migration`.
