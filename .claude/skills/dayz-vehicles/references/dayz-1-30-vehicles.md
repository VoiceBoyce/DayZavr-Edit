# DayZ 1.30 vehicle contract (cars / trucks / boats) — verified on 1.30.164078, updated for 1.30.164200

> Target: DayZ 1.30 stable (1.30.164200). Every `[EXACT]` cite below was re-read on `P:\scripts` / `P:\DZ` /
> `P:\bin` (1.30.164078) on 2026-10-15 and re-pointed to 1.30.164200 on 2026-10-21; `1.29:` cites point at the 1.29
> extraction. First written 2026-09-17 on 1.30 Exp 164014. What the 164200 patch changed for vehicles: §11.
> Labels: `../../_shared/dayz-version-policy.md`.

The atlas `SKILL.md` carries the short 1.30 matrix (top of the file) and rewrites its own lights / battery /
contact sections in place. This file holds the rest: the old→new table, lights / horn / battery / VFX detail,
get-in, boats, wrecks, `speedGeoms`, line drift and the port checklist for a ≤1.29 car mod. Two-wheelers →
`dayz-motorbikes`; aircraft built on `CarScript` → `dayz-aviation/references/dayz-1-30-aviation.md`. Full API
and config delta → the `dayz-130-migration` skill.

## What 1.30 changes (cars / trucks / boats)

| Topic | Before 1.30 | 1.30 | Cite (1.30 unless marked) |
|---|---|---|---|
| Transport children | `Car` + `Boat` | third sibling `Motorbike` (`simulation="motorbike"`) | `P:\bin\config.cpp:1481-1486`, `motorbike.c:30` |
| Headlight beams | subclass `CarLightBase` + override `CreateFrontLight`/`CreateRearLight` | `VehicleLightProfile*` subclass registered with `m_LightsComponent.RegisterLight`; live entity `VehicleLightBase` | `vehiclelightbase.c:1`, `offroad_02.c:21-22`, `carscript.c:2937-2948` (obsolete, no callers) |
| Light materials | `*ShineOn/Off`, `BrakesRearLight`... helpers | `m_LightsComponent.RegisterSelection` + `SetSelectionMaterial` | `carscript.c:230-268`, `vehiclelightscomponent.c:66-80`, obsolete helpers `carscript.c:2829-2934` |
| Battery | `IsVitalCarBattery` / `IsVitalTruckBattery` (default `true`, `GetBattery()` found the slot) | `NeedElectricitySourceDevice` / `GetElectricitySourceDevice` (**base `null`**) / `GetElectricitySourceDeviceType` | `transport.c:830-843`, `carscript.c:1768-1774`, `:2811-2821`; 1.29: `carscript.c:3015-3027` |
| Light switch (server) | `car.ToggleHeadlights()` | native `transport.LightToggle()`; `ToggleHeadlights` **removed** | `actionswitchlights.c:57-65` |
| Light toggle input | `UAToggleHeadlight` (`ToggleLightsActionInput`) | `UAToggleVehicleLights` (`ToggleVehicleLightsActionInput`) | `actionswitchlights.c:16-18`, `actioninput.c:776-782,849-855`, `P:\bin\constants.xml:90-91` |
| Forced light refresh | `ForceUpdateLightsStart`/`End` + synced `m_ForceUpdateLights` | `[Obsolete("1.30: no replacement")]`, empty bodies; flag gone | `carscript.c:2805-2809` |
| Horn actions | `ActionCarHornShort`/`Long` | `ActionVehicleHornShort`/`Long`; old classes `[Obsolete]` but still registered and routed to the component | `carscript.c:2319-2320`, `actioncarhorn.c:1,69-70,213-214`, `actionconstructor.c:340-343`, `carscript.c:2481-2488` |
| Horn sound | `m_CarHornShortSoundName`/`m_CarHornLongSoundName` | `m_HornComponent.RegisterSound(VehicleHornMode.SHORT/LONG, ...)`; old fields unread | `offroad_02.c:33-34`, `vehiclehorncomponent.c:36-68`, `carscript.c:2765-2766` |
| Horn inputs | `CarHornShort/LongActionInput` | `VehicleHornShort/LongActionInput`; old names are empty aliases | `actioninput.c:683-689,758,760-764,774` |
| Part health tick | `CarPartsHealthCheck()` | `PartsHealthCheck()` (the obsolete name only forwards) | `carscript.c:843,2121,2976-2980`, `transport.c:440` |
| Vital item stop | `CheckVitalItem(bool, int)` | `CheckVitalItemCallback(bool, int, Object, string, Param)` | `transport.c:423-438`, `carscript.c:853-854` |
| Contact damage | `CarScript.OnContact`, `CarContactData.impulse` | `Transport.OnContact`, `VehicleContactData.m_Impulse` (`CarContactData` = empty alias) | `transport.c:318-345,1153-1157`, `carscript.c:53,1291-1292` |
| Fluids | `CarFluid` / separate `BoatFluid` | `enum ETransportFluid`; `CarFluid : ETransportFluid {}`, `BoatFluid : ETransportFluid {}`; `Transport.Get*Script/FillScript(ETransportFluid,...)` | `transport.c:52-63,443-453`, `car.c:18`, `boat.c:13`, `carscript.c:420-433` |
| Fluid tick | `GameConstants.CARS_FLUIDS_TICK` | `VEHICLE_FLUIDS_TICK` (old name kept as alias) | `3_game\constants.c:866,877` |
| Boat speedo | none on `Boat` | `Boat.GetSpeedometer` + `GetSpeedometerAbsolute` | `boat.c:43-56` |
| Get-in | `CrewCanGetThrough` + `IsAreaAtDoorFree` + `CanReachSeatFromDoors(...,1.0)` | adds `CanGetIn()` (default `true`) and uses `IsAreaAtDoorFreeDiag` | `actiongetintransport.c:36,54,64`, `transport.c:675-678,910-924`; 1.29: `actiongetintransport.c:73` |
| Get-out | `CrewCanGetThrough && IsAreaAtDoorFree` | adds `!IsTransitioning()`, uses `IsAreaAtDoorFreeDiag` | `actiongetouttransport.c:70-82` |
| VFX | per-car particle members (`EffVehicleSmoke`...) | `VehicleVFXComponent` (client only) + `EVehicleVFXEffect(Group)`; scale `GetWeightCoef()` | `carscript.c:343-382`, `3_game\enums\evehiclevfxtypes.c:2-17`, `transport.c:478-481` |
| Ganged rear bulbs | — | `HasGangedTailAndBrakeLights()` (default `false`) | `carscript.c:468-471,2061-2094` |
| Tyre model | — | wheel keys `tyreType`, `tyreRoughness`, `tyreLongitudinalFriction`, `tyreLateralFriction` | `P:\DZ\vehicles\wheeled\config.cpp:874-880` (`HatchbackWheel`) |
| Wheel rules | — | `cfggameplay.json` `VehicleData.canDetach/DamageAttachedCarWheels` (+ `BikeWheels`), default `true` | `3_game\cfggameplaydatajson.c:415-418`, `inventoryitem.c:275,284` |
| High-speed geom | — | config `speedGeomActivation` + `speedGeoms[]` (only the two motorbikes use it) | `P:\DZ\vehicles\singletrack\config.cpp:262-266,1455-1459` |
| Wheel position | — | `Car.WheelGetPositionLS(int)` | `car.c:300` |
| Motorbike 3PP camera | — | `DAYZCAMERA_3RD_VEHICLE_MOTORBIKE = 32` | `dayzplayercameras.c:20,63` — detail in `dayz-motorbikes/references/rider-animation.md` |

[EXACT][CLAIM-ODOL-V56-130] Vanilla P3Ds on 1.30 are ODOL **v56** (1.29: v54). Header bytes 4-7 read 56 on
1.30.164078 for `P:\DZ\vehicles\wheeled\offroadhatchback\offroadhatchback.p3d`, `civiliansedan.p3d`,
`singletrack\motorbike_01\motorbike_01.p3d` and `motorbike_02.p3d`; the 1.29 `offroadhatchback.p3d` reads 54
(re-read 2026-10-15). A v54 reader fails with `ValueError: subsection not found`. [DESIGN] Preflight: on a new
build, read the header before any extraction or parity tooling, and run a positive control with the previous
build's reader. Motorbike anatomy tables read from the v56 binaries: `dayz-motorbikes/references/vanilla-p3d-anatomy.md`.

## 1. Lights: profiles on `m_LightsComponent` (not `CarLightBase`)

Before 1.30: subclass `CarLightBase` / `CarRearLightBase` and override `CreateFrontLight`/`CreateRearLight`
(1.29: `carscript.c:2502-2511`, called from `UpdateLightsClient`).

1.30: `CreateFrontLight`/`CreateRearLight` are `[Obsolete("1.30: use VehicleLightsComponent instead")]`
(`carscript.c:2937-2948`) and a grep of `P:\scripts` finds no caller. The vanilla `*FrontLight` classes stay on
disk as obsolete wrappers:

```c
// [EXACT] P:\scripts\4_world\entities\scriptedlightbase\spotlightbase\carlightbase\civiliansedanfrontlight.c:1-4
[Obsolete("use VehicleLightBase and VehicleLightProfileBase instead")]
class CivilianSedanFrontLight extends CarLightBase
{
	void CivilianSedanFrontLight()
```

The live light entity is `VehicleLightBase : SpotLightBase` (`vehiclelightbase.c:1`). Parameters live on a
profile: `VehicleLightProfileBase` (`vehiclelightprofiles\vehiclelightprofilebase.c:2-29`) with the subclasses
`VehicleLightProfileFront` (`:31`), `VehicleLightProfileRear` (`:57`) and `VehicleLightProfileBrakeAndReverse`
(`:74`). `CarScript`'s constructor already registers a generic profile for `"Front"` and `"Rear"`
(`carscript.c:270-271`); a car replaces it in its own constructor (`RegisterLight` is a `map.Set`,
`vehiclelightscomponent.c:61-64`):

```c
// [EXACT] P:\scripts\4_world\entities\vehicles\inheritedcars\offroad_02.c:21-22
		m_LightsComponent.RegisterLight("Front", new VehicleLightData(new Offroad_02LightProfileFront()));
		m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new Offroad_02LightProfileRear()));
```

```c
// [EXACT] P:\scripts\4_world\entities\scriptedlightbase\spotlightbase\vehiclelightprofiles\offroad_02.c:1-18
class Offroad_02LightProfileFront : VehicleLightProfileFront
{
	void Offroad_02LightProfileFront()
	{
		m_SegregatedBrightness = 5;
		m_SegregatedRadius = 75;
		m_SegregatedAngle = 90;
		m_SegregatedColorRGB = Vector(0.85, 0.85, 0.58);
		
		m_AggregatedBrightness = 10;
		m_AggregatedRadius = 100;
		m_AggregatedAngle = 120;
		m_AggregatedColorRGB = Vector(0.85, 0.85, 0.58);
		
		m_FadeInTime = 0.1;
		m_FadeOutTime = 0.05;
	}
}
```

Material swaps: the base registers `DashboardOn/Off`, `FrontLeftOn/Off`, `FrontRightOn/Off`, `TailOn/Off`,
`TailLeftOnly`, `TailRightOnly`, `BrakesOn/Off`, `ReverseOn/Off` against the selection IDs `0..8`
(`carscript.c:174-182`) and the config keys `dashboardMatOn/Off`, `frontReflectorMatOn/Off`,
`TailReflectorMatOn/Off`, `brakeReflectorMatOn/Off`, `ReverseReflectorMatOn/Off` (`carscript.c:230-268`).
`SetSelectionMaterial(key)` reads the config key and calls `SetObjectMaterial` (`vehiclelightscomponent.c:66-80`);
an unregistered key logs `ErrorEx("No VehicleLightSelectionData available ...")` (`:69-73`). Re-register a key in
your constructor when your bulbs use other selections (Offroad_02 rebinds `BrakesOn/Off` to the tail selections,
`offroad_02.c:24-31`).

Switching: `ActionSwitchLights` uses `ToggleVehicleLightsActionInput` and calls native `transport.LightToggle()`:

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\interact\vehicles\actionswitchlights.c:16-19
	override typename GetInputType()
	{
		return ToggleVehicleLightsActionInput;
	}
```

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actioninput.c:849-855
class ToggleVehicleLightsActionInput : DefaultActionInput
{
	ref ActionTarget targetNew;
	
	void ToggleVehicleLightsActionInput(PlayerBase player)
	{
		SetInput("UAToggleVehicleLights");
```

Headgear torches still bind `UAToggleHeadlight` (`ToggleLightsActionInput`, `actioninput.c:776-782`); both inputs
are declared in `P:\bin\constants.xml:90-91`. A custom car action that `SetInput("UAToggleHeadlight")` for vehicle
lights will not be what the vanilla key fires. `CarScript.ToggleHeadlights()` is gone: an `override` of it does not
compile on 1.30 [reported: bzed, tested on a 1.30 server; consistent with the api delta].

[DESIGN] LEDs that are not vehicle headlights (bloom vs real light via `PointLightBase`) are unchanged; see
`emissive-leds-and-dynamic-lights.md`.

## 2. Electricity: `NeedElectricitySourceDevice` (not `IsVital*Battery`)

```c
// [EXACT] P:\scripts\3_game\vehicles\transport.c:830-843
	bool NeedElectricitySourceDevice()
	{
		return true;
	}
	
	EntityAI GetElectricitySourceDevice()
	{
		return null;
	}
	
	string GetElectricitySourceDeviceType()
	{
		return "";
	}
```

Consumers: `CarScript.CheckOperationalRequirements` (`carscript.c:1768-1774`, `NO_BATTERY`), `GetBattery()`
(`:2437-2440`), `SpawnUniversalParts` (`:2508-2509`), `ActionSwitchLights.ActionCondition`
(`actionswitchlights.c:38-46`) and `ActionVehicleHornBase` (`actionvehiclehorn.c:15-17`). Vanilla Offroad_02:

```c
// [EXACT] P:\scripts\4_world\entities\vehicles\inheritedcars\offroad_02.c:73-88
	override bool NeedElectricitySourceDevice()
	{
		return true;
	}
	
	override EntityAI GetElectricitySourceDevice()
	{
		super.GetElectricitySourceDevice();
		
		return GetInventory().FindAttachment(CarBattery.SLOT_ID);
	}
	
	override string GetElectricitySourceDeviceType()
	{
		return "CarBattery";
	}
```

Trucks return `TruckBattery` (`truck_01_base.c:60-75`, `truck_02.c:38-54`). Kick-start bikes override
`NeedElectricitySourceDevice()` to `false` (`inheritedmotorbikes\motorbike_01.c:30-33`, `motorbike_02.c:31-34`).

**Porting trap (static reading of `P:\`, not measured):** a 1.29 car that extends `CarScript` directly and relied on
the defaults (`IsVitalCarBattery()`/`IsVitalTruckBattery()` = `true`, 1.29: `carscript.c:2733-2741`, with
`GetBattery()` looking up `CarBattery` then `TruckBattery`, 1.29: `carscript.c:3015-3027`) has no
`GetElectricitySourceDevice()` override on 1.30. The base returns `null`, so the engine reports `NO_BATTERY`, the
light action and the horn actions are never offered, and `SpawnUniversalParts` spawns no battery. Override all
three methods. Cars that inherit a vanilla car script (e.g. `extends OffroadHatchback`) get them from the parent.

## 3. Horn

`CarScript.SetActions()` adds `ActionVehicleHornShort`/`Long` (`carscript.c:2319-2320`). `ActionCarHornShort`/`Long`
are `[Obsolete("replaced by ActionVehicleHornShort/Long")]` (`actioncarhorn.c:69-70,213-214`) but are still
registered (`actionconstructor.c:342-343`) and still work through `CarScript.SetCarHornState`, which now forwards to
`m_HornComponent.SetMode` (`carscript.c:2481-2488`). `CarHornShortActionInput : VehicleHornShortActionInput {}` and
`CarHornLongActionInput : VehicleHornLongActionInput {}` are empty aliases (`actioninput.c:758,774`); both inputs use
`UACarHorn` (`:689`, `:764`).

Sound: `VehicleHornComponent` plays only what was registered (`vehiclehorncomponent.c:36-39,55-68`); register in the
car's constructor, as vanilla does:

```c
// [EXACT] P:\scripts\4_world\entities\vehicles\inheritedcars\offroad_02.c:33-34
		m_HornComponent.RegisterSound(VehicleHornMode.SHORT, 	"Offroad_02_Horn_Short_SoundSet");
		m_HornComponent.RegisterSound(VehicleHornMode.LONG, 	"Offroad_02_Horn_SoundSet");
```

Trucks: `truck_01_base.c:25-26`. AI noise is read from the config class `cfgVehicles <type> NoiseCarHorn`
(`vehiclehorncomponent.c:86`) and generated server-side in `SetMode` (`:41-53`). The horn state replicates through
`RegisterNetSyncVariableInt("m_HornComponent.m_Mode", ...)` (`carscript.c:283`). **Porting trap (static reading):**
the 1.29 fields `m_CarHornShortSoundName`/`m_CarHornLongSoundName` still exist (`carscript.c:2765-2766`, under
`//! DEPRECATED`) but nothing reads them; a ported car that only sets them is silent.

## 4. `GetWeightCoef`, `HasGangedTailAndBrakeLights`, VFX component

```c
// [EXACT] P:\scripts\3_game\vehicles\transport.c:478-481
	float GetWeightCoef()
	{
		return 1.0;
	}
```

Vanilla overrides: `Truck_01_Base` → `2.0` (`truck_01_base.c:120-123`); `Offroad_02` → `1.5` (`offroad_02.c:133-136`);
`Motorbike_01` → `0.5` (`inheritedmotorbikes\motorbike_01.c:84-87`); `Motorbike_02` → `0.75` (`motorbike_02.c:74-77`).
`CarScript` feeds the coef into the dust and wheel-contact VFX float modifiers (`carscript.c:720,793`).
`motorbikescript.c:661` *uses* `GetWeightCoef()`; it does not return `0.25` (an earlier digest claimed so).

```c
// [EXACT] P:\scripts\4_world\entities\vehicles\carscript.c:468-471
	bool HasGangedTailAndBrakeLights()
	{
		return false;
	}
```

When true, reverse/position material swaps do not turn the brake selection off (`carscript.c:2061-2094`).
Offroad_02 overrides it to `true` (`offroad_02.c:68-71`). `MotorbikeScript` has its own copy, also default `false`
(`motorbikescript.c:169-172`); since 1.30.164200 neither vanilla bike overrides it, so bikes apply `BrakeOff` again when
the brake is released (`motorbikescript.c:1236-1255`). Before 1.30.164200: Motorbike_01/02 returned `true`.

VFX: `InitializeParticles()` returns early on a dedicated/headless server, then builds `m_VFXComponent` and
registers `EVehicleVFXEffect.DUST_BEHIND/ENGINE_SMOKE/COOLANT_STEAM/EXHAUST_SMOKE` and the
`EVehicleVFXEffectGroup.WHEEL_SMOKE/WHEEL_CONTACT` groups from memory points `ptcDustPos`, `ptcEnginePos`,
`ptcCoolantPos`, `ptcExhaust_start/end` (`carscript.c:343-382`; enums `3_game\enums\evehiclevfxtypes.c:2-17`).
`m_VFXComponent` is therefore `null` on a dedicated server: guard any server-side use.
The wheel groups are registered with `new VehicleVFXDataParams(vector.Zero, dir, 2.0)` (`carscript.c:381-382`): the third
argument is `m_CooldownSec`, the minimum seconds between two (re)starts of one wheel's effect
(`vehiclevfxcomponent.c:82-93`, guard `VehicleVFXData.CanPlay()` `:43-50`, checked first in `PlayEffectData` `:171-175`).
Before 1.30.164200 it was 5.0. Pass your own value when you register an effect group on a custom vehicle.

## 5. Get-in (1.30 condition)

Before 1.30 the script chain was `CrewCanGetThrough` + `IsAreaAtDoorFree` + `CanReachSeatFromDoors(..., 1.0)`
(1.29: `actiongetintransport.c:63-76`). It already called `CanReachSeatFromDoors` from script on 1.29 (1.29:
`:73`); an older note in this pack ("engine-native only") was wrong on both builds.

1.30 adds `transport.CanGetIn()` (default `true`, `transport.c:675-678`) and swaps in `IsAreaAtDoorFreeDiag`, which
only adds a DayZDiag switch (`DiagMenuIDs.TRANSPORT_FREESPACE_DISABLED`) in front of `IsAreaAtDoorFree`
(`transport.c:910-924`), so overrides of `IsAreaAtDoorFree` still apply:

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\interact\actiongetintransport.c:54-66
		if (!transport.CrewCanGetThrough(crewIndex) || !transport.IsAreaAtDoorFreeDiag(crewIndex))
			return false;
		
		array<string> selections = new array<string>();
		transport.GetActionComponentNameList(componentIndex, selections);
		int selectionCount = selections.Count();
		vector playerPosition = player.GetPosition();
		
		for (int i = 0; i < selectionCount; ++i)
		{
			if (transport.CanReachSeatFromDoors(selections[i], playerPosition, 1.0))
				return true;
		}
```

New on `Transport`, used by the bikes: `HasDirectionalInOutAction()` (default `false`, `transport.c:680-683`) and
`GetSeatAnimationTypeDirectional(int, int)` (`:693-699`, read by `actiongetintransport.c:99`). Get-out:
`!vehCommand.IsTransitioning() && crewIndex >= 0 && CrewCanGetThrough && IsAreaAtDoorFreeDiag`
(`actiongetouttransport.c:70-82`).

## 6. Boats

```c
// [EXACT] P:\scripts\3_game\vehicles\boat.c:12-13
//!	Type of vehicle's fluid. (native, do not change or extend)
enum BoatFluid : ETransportFluid {}
```

```c
// [EXACT] P:\scripts\3_game\vehicles\boat.c:42-56
	//!	Returns the current speed of the vehicle in km/h.
	float GetSpeedometer()
	{
		vector transform[4];
		GetTransform(transform);

		float velocity = GetVelocity(this).InvMultiply3(transform)[2];
		return velocity * 3.6;
	}
	
	//! Returns the current speed of the vehicle in km/h. Value is absolute
	float GetSpeedometerAbsolute()
	{
		return Math.AbsFloat(GetSpeedometer());
	}
```

Before 1.30 `BoatFluid` was its own `{ FUEL }` enum and `Boat` had no speedometer (1.29: `boat.c:13-16`).
`BoatScript.GetHeatComfortOverride()` at `boatscript.c:256`. Config AI noise `CfgVehicles/BoatScript/NoiseBoatEngine
{ type="sound"; strength=30; }` is new (`P:\DZ\vehicles\water\config.cpp:41-45`). `ActionFillFuel` casts the target to
`Transport` and checks `GetFluidFractionScript(ETransportFluid.FUEL)` (`actionfillfuel.c:37,50`); the fill itself is
`FillScript(ETransportFluid.FUEL, ...)` (`cacontinuousfillfuel.c:121`), overridden by cars (`carscript.c:420-433`),
boats (`boatscript.c:225-238`) and bikes (`motorbikescript.c:243-256`). `BoatScript.CleanupEffects()` became public
(`boatscript.c:844`).

## 7. `speedGeomActivation` / `speedGeoms`

[CHANGELOG 1.30] Official MODDING notes: hide named components when travelling at speed to prevent ground snags.
Vanilla motorbikes (P:\ configs are derapified, so floats print with binary noise):

```cpp
// [EXACT] P:\DZ\vehicles\singletrack\config.cpp:262-266 (class Motorbike_01_ColorBase, from :220)
		speedGeomActivation=0.28999999;
		speedGeoms[]=
		{
			"geotohide"
		};
```

`Motorbike_02_ColorBase` uses `0.27` (`config.cpp:1455-1459`). A grep of every `config.cpp` under `P:\DZ` finds the
pair only in `vehicles\singletrack\config.cpp`: no vanilla car, truck or boat declares it. The C++ filter itself is
not in Enforce [UNVERIFIED]. Copy the motorbike pattern only if a high-speed geometry snag is measured.

## 8. Wreck cisterns as `Well` (new in 1.30)

```c
// [EXACT] P:\scripts\4_world\entities\building\wrecks\wreck_truck01_aban1_cistern.c:1-2
class Land_Wreck_Truck01_Aban1_Cistern : Well {}
class Land_Wreck_Truck01_Aban1_Cistern_DE : Land_Wreck_Truck01_Aban1_Cistern {}
```

`Land_Wreck_Truck01_Aban2_Cistern` is the same pattern (`wreck_truck01_aban2_cistern.c:1-2`). They drink as wells,
not as vehicle wrecks. None of these files exist in the 1.29 extraction.

Helicopter wreck scripts `StaticObj_Wreck_Chinook : CrashBase` (`wreck_chinook.c:1-23`) and
`StaticObj_Wreck_Mi24_TK : CrashBase` (`wreck_mi24_tk.c:1-10`) are also new; no `config.cpp` under `P:\DZ` declares
those classnames and `P:\DZ\structures_wrecks.txt` lists no matching model on 1.30.164078 — aviation-adjacent, see
the aviation 1.30 note.

## 9. Motorbike camera (pointer)

`DAYZCAMERA_3RD_VEHICLE_MOTORBIKE = 32` (`dayzplayercameras.c:20,63`); class
`DayZPlayerCamera3rdPersonVehicleMotorbike` at `dayzplayercameravehicles.c:239` with `UP_ANGLE_CAP = 10` (`:243`),
`CONST_YAW_LAG_REVERSE = 0.3` (`:256`), `CONST_SPRING_SMOOTHTIME_YAW = 0.5` (`:258`). The car 3PP camera now sets
`m_fIgnoreParentYaw = 0.0` explicitly (`:189`; 1.29 had the line commented out). Do not duplicate the bike
contract here — open `dayz-motorbikes/references/rider-animation.md` §2.
1.30.164200: the bike camera clamps its final rotation to its `CONST_UD_MIN/MAX` (-60/80, `:241-242`, `:407-408`), and
the base `UpdateUDAngleUnlocked` / `UpdateLRAngleUnlocked` (used by the car and bike 3PP cameras at `:109-110`,
`:336-337`) now replace the `pMin/pMax` they are given with `m_pPlayer.GetLookLimits(...)`
(`dayzplayercamera_base.c:152-156`, `:197-202`). A custom vehicle camera that passes its own limits to those two
methods no longer gets them; clamp the result yourself if you need tighter limits.

## 10. Line drift (1.29 → 1.30) for symbols this pack cites

| Symbol | 1.29 | 1.30.164200 |
|---|---|---|
| `CarScript` ctor `SetEventMask(POSTSIMULATE/POSTFRAME)` | `carscript.c:325-326` | `carscript.c:207-208` |
| `CarScript.EOnPostSimulate` | `carscript.c:948` | `carscript.c:807` |
| `CarScript.OnDriverExit` gear-stop | `carscript.c:1207-1215` | `carscript.c:940-948` |
| `Transport.OnDriverExit` empty body | `transport.c:161` | `transport.c:227` |
| `Transport.CrewCanGetThrough` (returns false) | `transport.c:493` | `transport.c:727` |
| `CarScript` light selection IDs 0..8 | `carscript.c:293-301` | `carscript.c:174-182` |
| `CarScript.UpdateLights` (client half `#ifndef SERVER`) | `carscript.c:2095-2101` | `carscript.c:1853-1859` |
| `CarScript.CheckOperationalRequirements` plug checks | `carscript.c:2004`, `:2011` | `carscript.c:1776`, `:1783` |
| `CarScript.EEOnCECreate` | `carscript.c:2967-2974` | `carscript.c:2412-2419` |
| `CarScript.FillUpCarFluids` | `carscript.c:3191-3196` | `carscript.c:2561-2566` |
| `CarScript.SpawnUniversalParts` | `carscript.c:3121-3158` | `carscript.c:2500-2528` |
| `CarScript.GetDoorConditionPointFromSelection` | `carscript.c:2674-2689` | `carscript.c:2141-2156` |
| `ForceUpdateLightsStart`/`End` | called at `:535-536`, defined `:2981/:2990` | `[Obsolete]`, empty, `carscript.c:2805-2809`; no caller |

## Port checklist (1.29 car mod → 1.30)

1. Delete overrides of `CreateFrontLight` / `CreateRearLight` / `IsVitalCarBattery` / `IsVitalTruckBattery` /
   `CarPartsHealthCheck` / `CheckVitalItem` / `OnBeforeSwitchLights` / the `*ShineOn/Off` and `*RearLight` helpers, and
   every call to `ForceUpdateLightsStart`/`End`. Rename a `CarPartsHealthCheck` override to
   `override protected void PartsHealthCheck()`. An `override void ToggleHeadlights()` no longer compiles: remove it.
2. Keep `super` in the constructor; `m_LightsComponent`, `m_HornComponent` (and, client-side, `m_VFXComponent`) are
   built by `CarScript` (`carscript.c:229-283`, `:343-348`).
3. Beams: `m_LightsComponent.RegisterLight("Front"|"Rear", new VehicleLightData(new YourProfile()))` with
   `YourProfile : VehicleLightProfileFront` / `VehicleLightProfileBrakeAndReverse`. Materials: keep the config
   `*MatOn/Off` keys; re-`RegisterSelection` only keys whose selections differ.
4. Horn: `m_HornComponent.RegisterSound(VehicleHornMode.SHORT/LONG, "<SoundSet>")`; setting
   `m_CarHornShortSoundName`/`LongSoundName` alone gives a silent horn.
5. Battery: if the class does not inherit a vanilla car script, override `NeedElectricitySourceDevice`,
   `GetElectricitySourceDevice` (return the attached battery) and `GetElectricitySourceDeviceType`; return `false`
   from the first one for a vehicle without battery.
6. Vehicle light action input is `UAToggleVehicleLights`, not `UAToggleHeadlight`.
7. `AddAction(ActionVehicleHornShort/Long)` — not `ActionCarHorn*` (obsolete, still functional).
8. Collision code that read `data[0].impulse` → `data[0].m_Impulse`; `OnContact` overrides now override
   `Transport.OnContact` (same signature, `transport.c:318`).
9. Fill code: `CarFluid.*` still compiles; generic code can use `FillScript/GetFluidCapacityScript/
   GetFluidFractionScript(ETransportFluid, ...)`. Raw fluid integers are unchanged (`FUEL=0 ... USER4=7`).
10. Override `GetWeightCoef` if dust/exhaust scale is wrong; `HasGangedTailAndBrakeLights` if brake and tail share
    a bulb; `CanGetIn()` only if you need to veto get-in.
11. Re-verify every `carscript.c:NNNN` cite you copied from an old guide against `P:\scripts` before shipping.
12. 1.30.164200: rename any own `HasCrew()` / un-`override`d `IsMoving()` on a `Transport` subclass (§11).

## 11. DayZ 1.30.164200 changes (patch of 2026-10-21)

Static reading of `P:\scripts` 1.30.164200 (not measured in game). Full delta: `dayz-130-migration` →
`references/api-delta-1.30.164078-to-164200.md`.

- **`Transport.HasCrew()`** — new `proto native bool HasCrew()`, "true if at least one member of crew is present"
  (`P:\scripts\3_game\vehicles\transport.c:175`).
- **`Transport.IsMoving()`** — new script method whose body is `Error("IsMoving() not implemented"); return false;`
  (`transport.c:819-823`). `CarScript.IsMoving()` (`GetSpeedometerAbsolute() > 3.5`) is now its `override`
  (`carscript.c:2221-2224`). `BoatScript` and `MotorbikeScript` do not implement it, so calling it on a boat or bike
  logs a script error every time and returns `false`; use `GetSpeedometerAbsolute()` there (`boat.c:53-56`,
  `motorbike.c:55-58`) or add an `override` in your class.
- **Naming rule for vehicle mods.** A mod written for 164078 that declares `bool HasCrew()` on any `Transport`
  subclass (car, boat, bike, helicopter on `CarScript`, or a `modded class` of one) now collides with the native; a
  `bool IsMoving()` without `override` on a non-`CarScript` `Transport` subclass shadows the new base method, and one
  with another signature does not compile. Rename helpers with a mod prefix (`MyMod_HasDriver()`), and when you mean
  to change vanilla behaviour write `override` and call `super`. Scanner kinds `transport-hascrew` /
  `transport-ismoving` in `dayz-130-migration` find them.
- **Detach rules.** `CarScript.CanReleaseAttachment` is unchanged (no detach while `EngineIsOn() && IsMoving()`, spare
  wheels only through `CanManipulateSpareWheel`, `carscript.c:638-667`). Motorbikes now refuse every detach while the
  engine is on or anyone is seated (`motorbikescript.c:612-625`) — detail and an own-slot exemption recipe in
  `dayz-motorbikes` §DayZ 1.30.164200 changes.
- **Wheel VFX cooldown** 5.0 → 2.0 s (§4).
- **Vehicle cameras** take the player's look limits (§9).
- **Motorbike lights** (selections, config keys, brightness): `dayz-motorbikes` §DayZ 1.30.164200 changes. Cars are
  unchanged.
- **Items dumped out of a container** by `MiscGameplayFunctions.ThrowAllItemsInInventoryEx` now get
  `-GetVelocity(parent)` as their throw force instead of the `force` argument (`miscgameplayfunctions.c:1295`) —
  relevant if a vehicle mod empties a destroyed vehicle's cargo with it; see `dayz-physics-engine` §DayZ 1.30 changes.
