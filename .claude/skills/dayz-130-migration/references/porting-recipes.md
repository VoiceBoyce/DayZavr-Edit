# Porting recipes: 1.29 → 1.30

Each recipe: symptom (scanner `kind`), the old code, the 1.30 code, and the vanilla line it was taken from
(`P:\scripts`, build 1.30.164078). Read the cited line before you paste — some bodies matter, not just signatures.

---

## 1. CE tick — `OnCEUpdate` is never called (`dead-hook`, `field-removed m_ElapsedSinceLastUpdate`)

```c
// Before 1.30
override void OnCEUpdate()
{
	super.OnCEUpdate();
	MyDecay(m_ElapsedSinceLastUpdate);      // field removed in 1.30
}

// 1.30 — 3_game\entities\entityai.c:3904 (vanilla calls ProcessVariables(elapsedTime) from here)
override void OnCEIterate(float currentTime, float elapsedTime)
{
	super.OnCEIterate(currentTime, elapsedTime);   // keep: it runs ProcessVariables + construction decay
	MyDecay(elapsedTime);
}
```
`OnCEUpdate()` is still declared `[Obsolete]` with no caller (1.29 vanilla called it 4×, 1.30 0×), so the old override
compiles and silently never runs. `m_LastUpdatedTime` is gone too.

## 2. `ProcessVariables()` → `ProcessVariables(float elapsedTime)` (`override-signature`)

```c
override void ProcessVariables(float elapsedTime)   // entityai.c:3913, itembase.c:4659
{
	super.ProcessVariables(elapsedTime);
}
```

## 3. Car headlights / rear lights (`dead-hook CreateFrontLight/CreateRearLight`, `call-removed ToggleHeadlights`)

`CarScript` builds a `VehicleLightsComponent` in its constructor (`4_world\entities\vehicles\carscript.c:229`) and registers
selections + two generic lights (`:270-271`). Each vanilla car re-registers `"Front"`/`"Rear"` with its own profile.

```c
// Before 1.30 (never called on 1.30)
override CarLightBase CreateFrontLight()   { return CarLightBase.Cast(ScriptedLightBase.CreateLight(MyCarFrontLight, "0 0 0")); }
override CarRearLightBase CreateRearLight() { return CarRearLightBase.Cast(ScriptedLightBase.CreateLight(MyCarRearLight, "0 0 0")); }

// 1.30 — pattern from 4_world\entities\scriptedlightbase\spotlightbase\vehiclelightprofiles\civiliansedan.c
class MyCarLightProfileFront : VehicleLightProfileFront
{
	void MyCarLightProfileFront()
	{
		m_SegregatedBrightness = 5;  m_SegregatedRadius = 65;  m_SegregatedAngle = 90;
		m_SegregatedColorRGB = Vector(1.0, 0.8, 0.8);
		m_AggregatedBrightness = 10; m_AggregatedRadius = 80;  m_AggregatedAngle = 100;
		m_AggregatedColorRGB = Vector(1.0, 0.8, 0.8);
	}
}
class MyCarLightProfileRear : VehicleLightProfileBrakeAndReverse
{
	void MyCarLightProfileRear()
	{
		m_SegregatedBrakeBrightness = 1; m_SegregatedBrakeRadius = 6; m_SegregatedBrakeAngle = 270;
		m_SegregatedBrakeColorRGB = Vector(1, 0.6, 0.05);
		m_SegregatedBrightness = 2; m_SegregatedRadius = 13; m_SegregatedAngle = 180;
		m_SegregatedColorRGB = Vector(1.0, 1.0, 0.8);
		m_AggregatedBrightness = 2.5; m_AggregatedRadius = 15; m_AggregatedAngle = 180;
		m_AggregatedColorRGB = Vector(1.0, 0.8, 0.6);
		m_FadeInTime = 0.1; m_FadeOutTime = 0.1;
	}
}
class MyCar : CarScript
{
	void MyCar()
	{
		// inheritedcars\civiliansedan.c:21-22
		m_LightsComponent.RegisterLight("Front", new VehicleLightData(new MyCarLightProfileFront()));
		m_LightsComponent.RegisterLight("Rear",  new VehicleLightData(new MyCarLightProfileRear()));
	}
}
```
The light position still comes from the model's memory points and the reflector selections/materials registered in
`CarScript` (`carscript.c:230-266`); if your model uses other selection names or materials, call
`m_LightsComponent.RegisterSelection(...)` the same way. Toggle: `LightToggle()` / state: `LightIsOn()` (`Transport`,
`3_game\vehicles\transport.c:227,236`). Brake/reverse/tail helpers (`BrakesRearLight`, `TailLightsShineOn`, …) are obsolete
and uncalled. Details: `dayz-vehicles`.

## 4. Car battery / "needs no battery" (`dead-hook IsVitalCarBattery/IsVitalTruckBattery`)

```c
// Before 1.30 (uncalled)
override bool IsVitalCarBattery() { return false; }

// 1.30 — transport.c:821-829; vanilla overrides: inheritedcars\civiliansedan.c:59-68, truck_01_base.c:60-74
override bool NeedElectricitySourceDevice() { return false; }          // no battery needed
// or, battery in a custom slot:
override bool NeedElectricitySourceDevice() { return true; }
override EntityAI GetElectricitySourceDevice() { super.GetElectricitySourceDevice(); return GetInventory().FindAttachment(CarBattery.SLOT_ID); }
override string GetElectricitySourceDeviceType() { return "CarBattery"; }
```
`CheckVitalItem(...)` → `CheckVitalItemCallback(bool isVital, int slotId, Object caller = null, string callbackMethodName = "", Param params = null)`
(protected, `transport.c:420`). `CarPartsHealthCheck` → `PartsHealthCheck`.

## 5. Car horn (`dead-field m_CarHornShortSoundName`, `call-removed GenerateCarHornAINoise`)

```c
// Before 1.30 — the fields are kept but marked DEPRECATED and never read (carscript.c:2765-2766): silent horn
m_CarHornShortSoundName = "MyCar_Horn_Short_SoundSet";
m_CarHornLongSoundName  = "MyCar_Horn_SoundSet";

// 1.30 — in the car ctor (inheritedcars\civiliansedan.c:24-25; component: 4_world\entities\vehicles\components\vehiclehorncomponent.c:36)
m_HornComponent.RegisterSound(VehicleHornMode.SHORT, "MyCar_Horn_Short_SoundSet");
m_HornComponent.RegisterSound(VehicleHornMode.LONG,  "MyCar_Horn_SoundSet");
```
Actions: `ActionVehicleHornShort/Long` with `VehicleHornShortActionInput/VehicleHornLongActionInput` (the old `CarHorn*`
inputs now just inherit from them). AI noise of the horn is inside `VehicleHornComponent`.

## 6. Base building hooks (`dead-hook BuildPartServer/DismantlePartServer`, `override-signature SetParent`)

```c
// Before 1.30 — compiles, never called (vanilla 1.30 calls the Ex versions)
override void BuildPartServer(notnull Man player, string part_name, int action_id) { MyHook(); super.BuildPartServer(player, part_name, action_id); }

// 1.30 — 4_world\classes\constructionbase.c:179 / :190 (Construction : ConstructionBase)
override void BuildPartServerEx(Man player, string part_name, int action_id)
{
	MyHook(player, part_name);
	super.BuildPartServerEx(player, part_name, action_id);
}
override void DismantlePartServerEx(Man player, string part_name, int action_id)
{
	super.DismantlePartServerEx(player, part_name, action_id);
}
```
- `Construction.SetParent(BaseBuildingBase)` → `protected void SetParent(EntityAI parent)` (`construction.c:44`); ctor gained an
  `EntityAI` overload (`:6`). `IsColliding(string)` → `IsCollidingEx(CollisionCheckData)` (`constructionbase.c:1481`).
- `ConstructionPart.m_Name/m_PartName/m_Id/m_IsBase/m_IsGate/...` are obsolete → `ConstructionPartTypeData`.
- `ConstructionActionData.SetSlotId/GetSlotId` obsolete; `ActionBuildPart.SetBuildingAnimation` → `SetBuildingAnimationEx`.
- `DestroyPartServer(Man, string, int, bool destroyed_by_connected_part = false)` is at `constructionbase.c:209`.
- Raid-protection / anti-build mods: also check the new instant actions (`ActionBuildPartInstant`, `ActionDismantlePartInstant`,
  `ActionDestroyPartInstant`) and map-structure rebuilding (`Rebuilding`, `RebuildingSpawner`) — they reach `BuildPartServerEx`
  through other paths. Details: `dayz-basebuilding`.

## 7. Combination lock / gates (`call-removed UnlockServer`, `enum-value-removed LockAction.*`)

Locks now have an inside and an outside dial.
```c
// Before 1.30
lock.UnlockServer(player, fence);
// 1.30 — 4_world\entities\itembase\combinationlock.c:409
lock.UnlockOnServer(player, fence);                 // isInsideLock = false
```
`SetCombination(int combination, bool setOtherSide = true)` (`:233`), `DialNextNumber(int lockSide = FACING_OUTSIDE)` (`:273`),
`SetNextDial(int lockSide = FACING_OUTSIDE)` (`:340`), `CheckLockedStateServer(bool insideLock = false)` (`:511`);
`ShuffleLock`, `DestroyLock`, `UpdateVisuals`, `UpdateSound`, `SoundLockChange*` became `protected`.
`LockAction.DIAL_NUMBER_CHANED`/`DIAL_INDEX_CHANGED` → `…_INSIDE` / `…_OUTSIDE`. `Unlock` / `IsLockedOnGate` obsolete.
New: `CodeLock` item (`4_world\entities\itembase\codelock.c`, `UnlockOnServer(EntityAI parent)` at `:277`); `Fence`
config `attachments[]` gained `"Att_CodeLock"`.

## 8. Action cursor / HUD (`override-signature ActionTargetsCursor.Update/BuildFixedCursor`)

```c
// 1.30 — 5_mission\gui\actiontargetscursor.c:310 / :267
modded class ActionTargetsCursor
{
	override void Update(bool fullUpdate = true)
	{
		super.Update(fullUpdate);
	}
	override protected void BuildFixedCursor(bool forceRebuild = true)
	{
		super.BuildFixedCursor(forceRebuild);
	}
}
```
`Set*XboxIcon` helpers are obsolete with no replacement. Vanilla `day_z_hud.layout` changed in 1.30 — a mod that ships its own
copy must re-diff it against `P:\gui\layouts\day_z_hud.layout`. New HUD layouts: `new_ui\hud\action_info_spacer.layout`,
`simpleicontemplate.layout`. Details: `dayz-ui-development`.

## 9. Attachment slot visibility (`dead-hook CanDisplayAttachmentSlot(string)`)

```c
// Before 1.30 (vanilla now only calls the int overload — entityai.c:1788, called at :1804)
override bool CanDisplayAttachmentSlot(string slot_name) { ... }
// 1.30
override bool CanDisplayAttachmentSlot(int slot_id)
{
	if (slot_id == InventorySlots.GetSlotIdFromString("CarBattery"))
		return false;
	return super.CanDisplayAttachmentSlot(slot_id);
}
```

## 10. Inventory moves (`call-obsolete *TakeEntityAsAttachment*/ *TakeEntityToCargo*`)

| Before 1.30 (self as target) | 1.30 (explicit target) — `3_game\entities\entityai.c:1972-2010`, `man.c:514-580` |
|---|---|
| `PredictiveTakeEntityToCargo(item)` | `PredictiveTakeEntityToTargetCargo(this, item)` |
| `PredictiveTakeEntityAsAttachment(item)` | `PredictiveTakeEntityToTargetAttachment(this, item)` |
| `PredictiveTakeEntityAsAttachmentEx(item, slot)` | `PredictiveTakeEntityToTargetAttachmentEx(this, item, slot)` |
| `PredictiveTakeEntityToCargoEx(item, row, col)` | `PredictiveTakeEntityToTargetCargoEx(cargo, item, row, col)` (cargo = `target.GetInventory().GetCargo()`) |
| `GetInventory().TakeEntityAsAttachment(mode, item)` | `GetInventory().TakeEntityAsTargetAttachment(mode, target, item)` (`inventory.c:1142`) |
| `GetInventory().TakeEntityAsAttachmentEx(mode, item, slot)` | `TakeEntityAsTargetAttachmentEx(mode, target, item, slot)` (`inventory.c:1100`) |
| `GetInventory().TakeEntityToCargo(mode, item)` | `TakeEntityToTargetCargo(mode, target, item)` (`inventory.c:1077`) |

Same for the `Local*`, `Server*`, `Juncture*` (Man) variants. The old ones still compile with a warning and mostly forward;
a mod that **overrides** them on `Man`/`PlayerBase` to intercept moves is bypassed, because vanilla calls the new ones.

## 11. Explosion effects per surface (`dead-hook AddExplosionEffectForSurface`, `call-removed GetExplosionParticleID`)

```c
// Before 1.30 (ExplosivesBase registry — ignored on 1.30)
AddExplosionEffectForSurface("Hit_Snow", ParticleList.EXPLOSION_GRENADE_SNOW);
int id = AmmoTypesAPI.GetExplosionParticleID(ammo, surface);

// 1.30 — 3_game\global\ammotypes.c:78, :90 (client side; vanilla fills the table in AmmoTypesAPI.Init(), called from
// MissionBase ctor under #ifndef SERVER: 5_mission\mission\missionbase.c:40)
AmmoTypesAPI.AddExplosionParticleEffect("MyGrenade_Ammo", "Hit_Snow", ParticleList.EXPLOSION_GRENADE_SNOW, "ExplosionDustEffect_SoundSet");
ExplosiveEffectData data = AmmoTypesAPI.GetExplosiveEffectData(ammo, surface);   // data.m_ParticleID, data.m_SoundSetName
```
Register modded ammo after vanilla's `Init()` (e.g. in your `modded class MissionBase` constructor after `super`, client only).

## 12. Renamed enum values and removed classes

- `EDLCId.DLC_UNKNOWN / DLC_FROSTLINE` → `EDLCId.UNKNOWN / FROSTLINE` (+ `BADLANDS`) — `3_game\services\contentdlc.c:3`.
- `MainMenuConsole/ServerBrowserTab.OnDLCChange(EDLCId)` → `OnDLCChange()`; `ModInfo.GetDLCImage` obsolete.
- `MissionBenchmark`, `BenchmarkConfig`, `BenchmarkLocation`, `Settings`/`GameSettings`/`SettingsMenu` (2_GameLib, never usable),
  `UndergroundDiag*`, `DebugAgentData` — removed; delete the code.
- `ERPCs`: never send/store vanilla RPC ids as numbers; your own RPC ids must live in your own range.

## 13. Third person, surfaces, headless

```c
// Before 1.30
if (GetGame().GetWorld().Is3rdPersonDisabled()) ...
// 1.30 — 3_game\global\world.c:177, enum at :301
if (GetGame().GetWorld().GetThirdPersonViewMode() == ThirdPersonMode.DISABLED) ...   // ENABLED / VEHICLES_ONLY exist too

// Surface under an object: CGame.SurfaceUnderObject* still exist and are NOT [Obsolete] on 1.30.164078 (game.c:1184-1186);
// Surface.GetStepsParticleID / GetWheelParticleID and DayZPlayerImplement.GetSurfaceType are obsolete. New, richer API:
SurfaceInfo info = GetGame().GetSurfaceInfoUnderObject(obj);   // 3_game\global\game.c:1172; SurfaceInfo : SurfaceProperties

// "no rendering here": headless robo-clients exist on 1.30
if (GetGame().IsHeadlessOrDedicatedServer()) ...
```

## 14. Liquids (drink / fill / wash)

`CCTWaterSurface(Ex)` and `ActionFillBottleBase(CB)` are class-level `[Obsolete]` (the `ActionWashHands*One` actions, already
unregistered in 1.29, are now marked obsolete too); vanilla uses the `CCTLiquid` target
condition (`4_world\classes\useractionscomponent\targetconditionscomponents\cctliquid.c`) and `ActionObtainLiquidBase`.
`CCTLiquid` still honours `Object.IsWell()`; new is `Object.GetLiquidSourceObjectType()` → `ELiquidSourceObjectType`
(`3_game\entities\object.c:652`; override example `land_fuelstation_feed.c:43`).

## 15. config.cpp

```cpp
class CfgMods { class MyMod { class defs {
	class worldScriptModule { value = ""; files[] = {"MyMod/scripts/4_World"}; };   // forward slashes
}; }; };
```
If your config redefines a vanilla class (not a subclass), the scanner lists keys 1.30 changed under you
(e.g. `Fence attachments[]` now has `"Att_CodeLock"`). Merge the 1.30 values from `config-delta-1.29-to-1.30.md`.

## 16. File enumeration `[reported: bzed, Linux 1.30.164014; DZEXP-134]`

```c
// Breaks on 1.30 (placeholder ignored, lists the server working dir)
FindFileHandle h = FindFile("$profile:MyMod/*.json", name, attr, FindFileFlags.ALL);
// With Community Framework 1.5.9+
FindFileHandle h = CF.FindFileEx("$profile:MyMod/*.json", name, attr, FindFileFlags.ALL);   // flags has no default
// open the results with the placeholder path ("$profile:MyMod/" + name), not the resolved one
```
Without CF: resolve `$profile:` yourself from the `-profiles=` launch parameter (relative to the server folder, forward
slashes) and call `FindFile` on that. Verify on this machine's Windows server before shipping.

## 17. Keeping one PBO for 1.29 and 1.30 (only if really needed)

```c
#ifdef DAYZ_1_29
	override void ProcessVariables()
	{
		super.ProcessVariables();
#else
	override void ProcessVariables(float elapsedTime)
	{
		super.ProcessVariables(elapsedTime);
#endif
		MyCommonBody();
	}
```
Old code under `DAYZ_1_29`, new code in `#else`, so the new code stays the default on 1.31+.

## 18. Door blocking — `CanDoorBeOpened(int, bool)` is bypassed (`dead-hook`)

```c
// Before 1.30 — compiles on 1.30 but the open-door action no longer calls this overload
modded class BuildingBase
{
	override bool CanDoorBeOpened(int doorIndex, bool checkIfLocked = false)
	{
		if (MyRaidProtection_IsBlocked(this, doorIndex))
			return false;
		return super.CanDoorBeOpened(doorIndex, checkIfLocked);
	}
}

// 1.30 — 3_game\entities\building.c:141 (params class at :5-10); called by actionopendoors.c:52,72
modded class BuildingBase
{
	override bool CanDoorBeOpened(notnull DoorManipulationParams params)
	{
		if (MyRaidProtection_IsBlocked(this, params.m_DoorIndex, params.m_Caller))   // m_Caller = who opens (new in 1.30)
			return false;
		return super.CanDoorBeOpened(params);
	}
}
```
The `(int, bool)` overload now just builds a `DoorManipulationParams` and forwards (`building.c:160-167`). Closing still goes
through `CanDoorBeClosed(int doorIndex)` (`actionclosedoors.c:38,55`); a `CanDoorBeClosed(int, Object caller)` overload exists
(`building.c:169`). Vanilla's own bunker uses the params overload (`4_world\entities\building\bunker.c:55-61`).

## 19. HUD visibility overrides (`dead-hook IngameHudVisibility.UpdateElement/UpdateVisibility`)

```c
// Before 1.30 — still declared, only forwards, vanilla never calls it
override protected void UpdateElement(EHudElement element) { ... }

// 1.30 — 5_mission\gui\ingamehudvisibility.c:128 / :177 (called at :100 / :169)
modded class IngameHudVisibility
{
	override protected void UpdateElement(EHudElement element, bool ignoreWidgetVisiblityState)
	{
		super.UpdateElement(element, ignoreWidgetVisiblityState);
		// your extra rule here
	}
	override protected void UpdateVisibility(EHudElement element, bool target, bool ignoreWidgetVisiblityState)
	{
		super.UpdateVisibility(element, target, ignoreWidgetVisiblityState);
	}
}
```
`SetContextFlag(EHudContextFlags, bool)` is still called by vanilla (`ingamehud.c:332,364`); 1.30 added a 3-arg overload.

## 20. Drying / soaking increments (`dead-hook GetDryingIncrement(string)`)

```c
// Before 1.30 — vanilla called GetDryingIncrement("player") (1.29 environment.c:888)
override float GetDryingIncrement(string pIncrementName) { ... }
// 1.30 — vanilla calls the enum overload (environment.c:1169, itembase.c:4581, universaltemperaturesourcelambdabaseimpl.c:24)
override float GetDryingIncrement(EEnvironmentDryingCategories category) { return super.GetDryingIncrement(category) * 2; }
override float GetSoakingIncrement(EEnvironmentSoakingCategories category) { return super.GetSoakingIncrement(category); }
```
