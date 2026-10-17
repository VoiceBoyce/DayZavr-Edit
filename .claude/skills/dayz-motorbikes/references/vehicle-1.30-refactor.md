# Vehicle Refactor in DayZ 1.30 (1.29 -> 1.30 Migration Guide)

> Target: DayZ 1.30 stable (1.30.164078). Version labels: `../../_shared/dayz-version-policy.md`. Full API delta: the `dayz-130-migration` skill.

This document details the internal restructuring of the vehicle system introduced in DayZ 1.30 (first read on Exp 164014; every cite below re-checked on stable 1.30.164078 on 2026-10-15, script line numbers unchanged). It covers the modular decomposition of `CarScript`, the escalation of responsibilities to `Transport`, the adoption of specialized components, and the migration guide for existing car mods.

---

## 1. Responsibility Decomposition in CarScript and Transport

In DayZ 1.30, `CarScript.c` (-1155 / +877 lines) ceases to be a self-contained monolith:
- **Moved to `Transport.c` (+438 lines)**:
  - Unified fluids definition: `enum ETransportFluid` (`Transport.c:52-63`), from which `CarFluid`, `BoatFluid`, and `MotorbikeFluid` inherit.
  - Physics collision caching and handling: `m_ContactCache`, `m_MomentumPrevTick`, `m_VelocityPrevTick`, `m_dmgContactCoef`, `m_VehicleContactDamageCoef`, `m_CrewContactDamageCoef` (`Transport.c:90-96`).
  - Impact callback: `OnContact(...)` (`Transport.c:315`).
  - Electrical source detection: `NeedElectricitySourceDevice()`, `GetElectricitySourceDevice()`, `GetElectricitySourceDeviceType()` (`Transport.c:821-834`; default returns `true`, `null`, and `""`).
  - Vital parts check with callback: `CheckVitalItemCallback(...)` and virtual declaration `PartsHealthCheck()` (`Transport.c:420, 437`).
- **Moved to Modular Components (`P:\scripts\4_World\Entities\Vehicles\Components\`)**:
  - Lights and reflectors -> `VehicleLightsComponent`.
  - Horn and AI acoustic noise -> `VehicleHornComponent`.
  - Exhaust, radiator, and wheel dust particles -> `VehicleVFXComponent`.

---

## 2. Removed and Obsolete-Marked Methods

`CarScript.c` includes an explicit `//! DEPRECATED` section (`P:\scripts\4_World\Entities\Vehicles\CarScript.c:2760-2981`). Nothing in vanilla calls the obsolete light/battery/vital methods any more (a grep of `P:\scripts` finds only their declarations; `Truck_01_Cargo` still overrides the dead `IsVitalCarBattery`):

| Obsolete Method in 1.30 | Official Code Annotation | Corrective Action in Mod |
|---|---|---|
| `CreateFrontLight()` | `[Obsolete("1.30: use VehicleLightsComponent instead")]` | Register `VehicleLightProfileFront` in `m_LightsComponent`. |
| `CreateRearLight()` | `[Obsolete("1.30: use VehicleLightsComponent instead")]` | Register `VehicleLightProfileRear` in `m_LightsComponent`. |
| `IsVitalCarBattery()` | `[Obsolete("1.30: use Transport.NeedElectricitySourceDevice")]` | Override `NeedElectricitySourceDevice()` (no parameters, `Transport.c:821`) to return `true` **and** `GetElectricitySourceDevice()` to return the attached battery: the base returns `null` (`Transport.c:826-829`), which `CarScript.CheckOperationalRequirements` turns into `NO_BATTERY` (`CarScript.c:1768-1774`). |
| `IsVitalTruckBattery()` | `[Obsolete("1.30: use Transport.NeedElectricitySourceDevice")]` | Same as above with `TruckBattery.SLOT_ID` / `"TruckBattery"` (`truck_01_base.c:60-75`). |
| `IsScriptedLightsOn()` | `[Obsolete("1.30: use Transport.LightIsOn")]` | Invoke `LightIsOn()`. |
| `CarPartsHealthCheck()` | `[Obsolete("1.30: use Transport::PartsHealthCheck instead")]` | Rename signature to `override protected void PartsHealthCheck()` (the tick calls `PartsHealthCheck()` directly, `CarScript.c:843`; the old name only forwards, `:2976-2980`). |
| `CheckVitalItem(bool, int)` | `[Obsolete("1.30: use Transport::CheckVitalItemCallback instead")]` | Use `CheckVitalItemCallback(...)`. |
| `OnBeforeSwitchLights()` | `[Obsolete("1.30: no replacement")]` | Remove prior logic; control is immediate in components. |
| `ForceUpdateLightsStart/End()` | `[Obsolete("1.30: no replacement")]` | Remove manual lights synchronization. |
| `BrakesRearLight()`, etc. | `[Obsolete("1.30: use VehicleLightsComponent instead")]` | Use `m_LightsComponent.SetLightTypeAndMode(...)`. |

---

## 3. New Tyre Physics Parameters in config.cpp

In DayZ 1.30, all car wheel classes (`HatchbackWheel`, `CivSedanWheel`, `Offroad_02_Wheel`, etc.) incorporate four new physical parameters, absent in `stable-1.29` (example in `HatchbackWheel`: `P:\DZ\vehicles\wheeled\config.cpp:874-880`):
- `tyreType = 0;`: Tyre type identifier.
- `tyreRoughness = 1.0;` (pristine) / `0.2;` (ruined): Ground surface roughness.
- `tyreLongitudinalFriction = 0.85 - 1.5;` (`2.5` in `Truck_01_WheelDouble`, `config.cpp:18147`): longitudinal grip during acceleration and braking. On ruined wheels it drops to `0.25 - 0.35`.
- `tyreLateralFriction = 0.95 - 1.8;` (`2.8` in `Truck_01_WheelDouble`, `config.cpp:18148`): lateral skid resistance. On ruined rims it rises to `4.95` across all seven vanilla ruined car wheels (e.g. `config.cpp:906`); that this mimics metal on asphalt is an interpretation [UNVERIFIED].

> **Cars only.** These ranges come from `vehicles_wheeled`; motorbike wheels (`P:\DZ\vehicles\singletrack\config.cpp`) do not follow them: `tyreType = 1` (`:167, 200, 1033, 1065`); pristine `tyreLateralFriction` 3.0-7.0 (`:173, 206, 1038, 1070`) and ruined 0.5 (`:185, 218, 1049, 1081`), meaning it drops when ruined rather than rising; pristine `tyreLongitudinalFriction` 0.85-1.8 (`:172, 205, 1037, 1069`) and ruined 0.5 (`:184, 217, 1048, 1080`); Motorbike_02 wheels do not declare `tyreRoughness` (`:1019-1082`). For a bike, start from `:154-219` (Motorbike_01) or `:1019-1082` (Motorbike_02).

---

## 4. Restructuring of Offroad_02 (Bohemia's Reference Example)

1. **Dual door selection (`hiddenSelections`)**:
   `Offroad_02` doors now differentiate exterior and interior panels:
   `hiddenSelections[] = {"texture_roof_ext","texture_roof_int"};` (`Offroad_02_Door_1_1`, `P:\DZ\vehicles\wheeled\config.cpp:20576`, selections at `:20587-20590`).
   The `Doors` damage zone includes dual `.rvmat` arrays in `RefTexsMats[]` and `healthLevels[]`.
2. **Front impact zone adjustment**:
   In `DamageZones -> Front`, `"dmgZone_bumper_1"` is removed from `componentNames[]` (`{"dmgZone_front","dmgZone_bumper_1"}` → `{"dmgZone_front"}`, `dayz-130-migration/references/config-delta-1.29-to-1.30.md`); whether this is to avoid parasitic damage when hitting the bumper is an interpretation [UNVERIFIED].
3. **New beige variants**:
   `Offroad_02_Beige` and the `_Beige` / `_BeigeRust` parts are introduced (`P:\DZ\vehicles\wheeled\config.cpp:22394, 23003, 23441`; they do not exist in 1.29).

---

## 5. Migration Checklist for a Car Mod from 1.29 to 1.30

Follow this step-by-step list to update an existing car:

- [EXACT] **Step 1: Remove obsolete light factories**:
  Delete any implementation of `override CarLightBase CreateFrontLight()` and `override CarRearLightBase CreateRearLight()`.
- [EXACT] **Step 2: Register lighting profiles**:
  Create your profile derived from `VehicleLightProfileFront` / `VehicleLightProfileRear` and register it in the vehicle constructor:
  ```c
  m_LightsComponent.RegisterLight("Front", new VehicleLightData(new MiCocheLightProfileFront()));
  m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new MiCocheLightProfileRear()));
  ```
- [EXACT] **Step 3: Register horn in component**:
  In the constructor, replace `m_CarHornShortSoundName`/`m_CarHornLongSoundName` (still declared under `//! DEPRECATED`, `CarScript.c:2765-2766`, but never read: a car that only sets them has a silent horn) with the vanilla pattern (`civiliansedan.c:24-25`):
  ```c
  m_HornComponent.RegisterSound(VehicleHornMode.SHORT, "MiCoche_Horn_Short_SoundSet");
  m_HornComponent.RegisterSound(VehicleHornMode.LONG,  "MiCoche_Horn_Long_SoundSet");
  ```
- [EXACT] **Step 4: Update electrical supply**:
  Replace `IsVitalCarBattery()` and `IsVitalTruckBattery()` with the new `Transport` API. Mandatory for any class that extends `CarScript` directly (vanilla cars already do it, `civiliansedan.c:59-74`); without it the base `GetElectricitySourceDevice()` returns `null` and the engine never starts (static reading, `CarScript.c:1768-1774`):
  ```c
  override bool NeedElectricitySourceDevice() { return true; }
  override EntityAI GetElectricitySourceDevice() { return GetInventory().FindAttachment(CarBattery.SLOT_ID); }
  override string GetElectricitySourceDeviceType() { return "CarBattery"; }
  ```
- [EXACT] **Step 5: Rename signatures and collision data access**:
  - If you had `protected void CarPartsHealthCheck()`, change it to `override protected void PartsHealthCheck()`.
  - If you were reading `data[0].impulse`, update it to `data[0].m_Impulse`: it is a public member of `VehicleContactData` (`Transport.c:1144-1148`), and `CarContactData` remains as an empty subclass (`CarScript.c:53`).
- [EXACT] **Step 6: Update horn actions**:
  Verify that your vehicle actions register `ActionVehicleHornShort` and `ActionVehicleHornLong` instead of the legacy `ActionCarHorn*`.
- [DESIGN] **Step 7: Add modern friction in wheel config.cpp**:
  Ensure that your mod's wheels define `tyreRoughness`, `tyreLongitudinalFriction`, and `tyreLateralFriction`.

---

## 6. Claims in 1.29 Guides That Become False in 1.30

| Common claim in 1.29 guides | Reality in DayZ 1.30 (stable 1.30.164078) | Evidence citation |
|---|---|---|
| *"To add lights to a car you must override `CreateFrontLight()` and create a `CarLightBase`"* | **FALSE**. Those methods are deprecated (`[Obsolete]`). The engine does not invoke them; you must create a `VehicleLightProfileBase` and register it in `m_LightsComponent`. | `CarScript.c:2937-2948` and `VehicleLightsComponent.c:61-64` |
| *"The horn is scripted with `SetCarHornState(ECarHornState)` and `ActionCarHorn`"* | **FALSE**. The `ActionCarHorn*` classes still exist, but marked `[Obsolete("replaced by ActionVehicleHornBase")]`, and cars now register `ActionVehicleHornShort/Long`. The horn is operated with `m_HornComponent.SetMode(VehicleHornMode)`. | `ActionCarHorn.c:1-2`, `CarScript.c:2319-2320`, and `VehicleHornComponent.c:41-53` |
| *"The battery is validated with `IsVitalCarBattery()`"* | **FALSE**. Obsolete method with no effect on starting. `Transport.NeedElectricitySourceDevice()` is consulted now. | `CarScript.c:2811-2817` and `Transport.c:821` |
| *"Engine parts checking is scripted in `CarPartsHealthCheck()`"* | **FALSE**. The signature was absorbed by `Transport.c` and renamed to `PartsHealthCheck()`. | `Transport.c:437`, `CarScript.c:2121`, and `CarScript.c:2976-2979` |
| *"Collision impulses are read in `data[0].impulse`"* | **FALSE**. The field is now `m_Impulse`, public, in the base class `VehicleContactData`; `CarContactData` is an empty subclass. No `impulse` member exists, so `.impulse` should not compile [UNVERIFIED: not compiled]. | `Transport.c:1144-1148` and `CarScript.c:53` |
| *"Exhaust smoke is controlled directly via `m_exhaustFx`"* | **FALSE**. Manual FX variables are obsolete. The entire particle pipeline is managed in `VehicleVFXComponent`. | `CarScript.c:477-478` (`m_VFXComponent.PlayEffect`) and `CarScript.c:2760, 2779` (`m_exhaustFx` remains in the `//! DEPRECATED` section) |
| *"Vehicle animation graphs are inaccessible `.agr` binary files"* | **FALSE**. In 1.30, `Vehicles.agf` is shipped as editable Enfusion structured text. (corrected 2026-09-28) It was already false in 1.29, so it is not a 1.29 truth that 1.30 broke: 1.29 ships the vehicle sub-graph as the text `Vehicles.agr` in the old `$AnimGraph 7` format, and 1.30 changed the syntax. | `Vehicles.agf:1-3655`; 1.29: `O:\...\DZ\anims\workspaces\player\player_main\vehicles.agr:1` (`$AnimGraph 7 {`) |
| *"Bailing out while moving always uses the unique 8 km/h threshold of cars"* | **FALSE**. Dedicated thresholds exist: 8 km/h for cars, 5 km/h for boats, and 10 km/h for motorbikes (`MOTO_JUMPOUT_THRESHOLD`). | `ActionGetOutTransport.c:34-36` |
