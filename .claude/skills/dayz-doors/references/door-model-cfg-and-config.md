# Door model.cfg and config.cpp contract

## Contents

- [Evidence convention](#evidence-convention)
- [Name flow](#name-flow)
- [model.cfg contract](#modelcfg-contract)
- [config.cpp contract](#configcpp-contract)
- [DamageSystem contract](#damagesystem-contract)
- [Verified mappings](#verified-mappings)
- [Unverified or conflicting details](#unverified-or-conflicting-details)
- [Door open conditions and locks (DayZ 1.30)](#door-open-conditions-and-locks-dayz-130)
- [Hooks vanilla 1.30 calls, and hooks it does not](#hooks-vanilla-130-calls-and-hooks-it-does-not)
- [Sources](#sources)

## Evidence convention

- **Verified** means the class/property appears in one of the example files of novoGOD's door tutorial *Doors_Buttons_Lesson*, or the stated behavior is explicitly taught by one of its four texts. The tutorial carries no license, so the Pack does not ship it: citations name its files by path and line, for example **Doors_Buttons_Lesson/Door/Simple_Door.cfg:1-14**.
- A leading **?** means the supplied files do not independently confirm the claim, or the sources conflict.
- [worked-examples.md](worked-examples.md) describes the three examples in tables; neither page copies their code. This document describes the contract without silently normalizing those examples.

## Name flow

Normal mapping:

**P3D View Geometry interaction selection -> model.cfg source -> config.cpp class Doors component**

Moving geometry mapping:

**P3D moving named selection -> CfgSkeletons bone -> CfgModels animation selection**

The two chains can share a name (**door1**) or diverge (**selection = "door1"**, **source = "door1_open"**). See **WELCOME TO novoGODS Shit Door Tutorial.txt:17-22** and **Doors_Buttons_Lesson/Door_w_Button/Door_w_Button.cfg:27-38**.

## model.cfg contract

### CfgSkeletons

The tutorial's files spell the root class **cfgSkeletons**; the tutorial/common convention calls it **CfgSkeletons**. Preserve the working syntax when following an example.

| Class/property | Contract | Verification |
|---|---|---|
| **class CfgSkeletons / cfgSkeletons** | Root skeleton declaration. | Simple **Doors_Buttons_Lesson/Door/Simple_Door.cfg:1-14**; button **:1-12**; expert **:1-14**. |
| Custom skeleton class | Referenced by the model's **skeletonName**. | **Simple_DoorSkeleton** at Simple **:3-13**; equivalents in other CFGs. |
| **skeletonInherit** | Parent skeleton; all examples use **""**. | Simple **:5**; button **:5**; expert **:5**. |
| **isDiscrete** | All three real examples use **0**. | Simple **:6**; button **:6**; expert **:6**. |
| **SkeletonBones[]** | Flat (child,parent) string pairs. | Simple **:7-12**; button **:7-10**; expert **:7-12**. |
| Parent **""** | Bone follows no animated parent. | Fundamentals **:11-14**; real **door1** roots and expert **lever**. |
| Parent bone name | Child follows that parent's transform. | Simple **handle,door1** at **:10-11**; expert **:9-11**. |

Prefer a model-unique skeleton name derived from the P3D basename, such as **<p3dname>_skeleton**. Real files use **Simple_DoorSkeleton**, **Door_w_ButtonSkeleton**, and **Expert_ModeSkeleton**; separator/suffix spelling is a convention, not proven as an engine requirement.

? The fundamentals tutorial says duplicate skeleton names can cause server crashes (**WELCOME TO novoGODS Shit Door Tutorial.txt:11**), but the supplied files cannot reproduce that outcome. Treat uniqueness as a preventive convention.

### CfgModels and class Animations

| Class/property | Contract | Verification |
|---|---|---|
| **class CfgModels** | Root model declaration. | Simple **Doors_Buttons_Lesson/Door/Simple_Door.cfg:15-55**; button **:13-41**; expert **:15-67**. |
| **class Default** | Empty base with sections and skeleton fields. | Simple **:17-22**; button **:15-20**; expert **:17-22**. |
| **sections[]** | Named material sections; empty here. | Simple **:19,26**; button **:17,24**; expert **:19,26**. |
| **sectionsInherit** | Inherited sections; empty here. | Simple **:20**; button **:18**; expert **:20**. |
| **skeletonName** | Exact custom skeleton class name. | Simple **:25**; button **:23**; expert **:25**. |
| Model class | Matches P3D basename in all examples. | **Simple_Door :23**; **Door_w_Button :21**; **Expert_Mode :23**. |
| **class Animations** | One class per animated selection, not per source. | Simple **:27-53**; expert has three at **:27-65**. |
| **type = "rotation"** | Rotates the named selection. | Every real animation class. |
| **selection** | Moving named selection in the LODs where it exists. | Fundamentals **:17**; Simple **:32,44**. |
| **source** | Controller/interaction name; may differ from selection. | Fundamentals **:18**; button **:30-32**; expert **:32-33,44-45,56-57**. |
| **axis** | Named Memory LOD axis selection. | Simple **:34,46**; button **:32**; expert **:34,46,58**. |
| **memory = 1** | All real rotations use Memory axes. | Simple **:35,47**; button **:33**; expert **:35,47,59**. |
| **minValue** | Source phase where motion starts. | Simple door **0.15 :36**; handle **0 :48**. |
| **maxValue** | Source phase where angle1 is reached. | Simple handle **0.15 :49**; expert lever **0.5 :61**. |
| **angle0** | Rotation at minValue; all examples use **0**. | Simple **:38,50**; button **:36**; expert **:38,50,62**. |
| **angle1** | Rotation at maxValue; sign sets direction in these files. | Simple **1.9/-1.4 :39,51**; expert **1.9/-1.7/-0.88 :39,51,63**. |

**Verified: `angle0/angle1` are in RADIANS.** BI Model Config wiki: angle values are specified in radians (= degrees x pi/180); a bare number is radians, or write `"rad 90"` to give degrees. Independently confirmed in the user's `dayz-animation-pipeline/references/config-driven-animation.md:40`. The tutorial's values fit: Simple Door `angle1 = 1.9` rad ~= 109 deg of swing. (Verified 2026-07-14 vs BI wiki + local skill.)

### Source aggregation

A single source can drive several animation classes:

- Simple Door: **Door1** and **handle** both use **source = "door1"** (**Doors_Buttons_Lesson/Door/Simple_Door.cfg:29-52**).
- Button: only **Door1** animates, using **source = "door1_open"** (**Doors_Buttons_Lesson/Door_w_Button/Door_w_Button.cfg:27-38**).
- Expert: **Door1**, **Handle**, and **Lever** all use **source = "door1_open"** (**Doors_Buttons_Lesson/Expert_Mode/Expert_Mode.cfg:29-64**).

The button is absent from its skeleton because it does not animate (**Door_w_Button Readme.txt:65**).

## config.cpp contract

### Outer classes and object declaration

| Class/property | Contract | Verification |
|---|---|---|
| **class CfgPatches** | Addon patch root. | Every real **config.cpp:1-7**. |
| Patch subclass | Example-specific patch name. | Each real **config.cpp:3**. |
| **requiredAddons[] = {"DZ_Data"}** | Required in all working examples. | Every real **config.cpp:5**; fundamentals **:101-107,228**. |
| **class CfgVehicles** | Entity declaration root. | Every real **config.cpp:9**. |
| **class HouseNoDestruct;** | Base forward declaration. | Every real **config.cpp:11**. |
| **class land_*: HouseNoDestruct** | Entity class pattern used here. | Every real **config.cpp:12**. |
| **scope** | All real examples use **scope = 1**. | Every real **config.cpp:14**. |
| **model** | Addon-relative P3D path. | Every real **config.cpp:15**. |
| **class Doors** | Door subsystem under the entity. | Every real **config.cpp:16**. |

**Verified: `scope` = 0 private / 1 protected / 2 public.** BI CfgVehicles Config Reference. In DayZ practice, 1 = protected (placed via Terrain Builder or inherited, not directly spawnable) and 2 = player/admin-spawnable (user's `enforce-script-reference/references/config-cpp.md:123`). Buildings ship `scope = 1`, matching the tutorial's intent. (Verified 2026-07-14.)

### Per-door entry

| Property | Contract in supplied examples | Verification |
|---|---|---|
| Doors subclass | Corresponds by convention to source/component; **component** is exact binding. | Simple **Doors_Buttons_Lesson/Door/config.cpp:18-21**; button **Doors_Buttons_Lesson/Door_w_Button/config.cpp:18-21**. |
| **displayName** | Player-facing door/controller label. | Simple/button/expert main **:20**; expert secondary **:33**. |
| **component** | Exact model.cfg interaction source. | Simple **:21**; button **:21**; expert **:21,34**. |
| **soundPos** | Memory point where door sound originates. | Simple **:22**; button **:22**; expert main **:22**; real Expert Lever omits it. |
| **animPeriod** | Duration in seconds per tutorial. | Simple **1.3 :23**; button/expert main **1.0**; expert secondary **0.50 :35**. |
| **initPhase** | All real entries use **0.0**. | Simple **:24**; button **:24**; expert **:24,36**. |
| **initOpened** | World-spawn behavior per tutorial. | Simple/button **0.0**; expert **0.5/0.0 :25,37**. |
| **soundOpen** | Opening sound name. | Full door entries Simple/button/expert **:26**. |
| **soundClose** | Closing sound name. | Full door entries Simple/button/expert **:27**. |
| **soundLocked** | Locked/rattle sound. | Full door entries Simple/button/expert **:28**. |
| **soundOpenABit** | Partial-opening sound. | Full door entries Simple/button/expert **:29**. |
| **relatedInventorySlots[]** (new in 1.30) | Names of inventory slots tied to this door (locks). Parsed into `AdditionalDoorInfo.m_RelatedInventorySlotNames`. Absent from the tutorial's examples. | `P:\scripts\3_game\entities\additionaldoorsinfo.c:23-27`. **[UNVERIFIED]** no `config.cpp` under `P:\DZ` (1.30.164078) sets the property in a **class Doors** entry. |
| **lockCompatibilityBitMask** (new in 1.30) | Integer already in bitwise form. If omitted, script default is `1 << EBuildingLockType.LOCKPICK`. | `P:\scripts\3_game\entities\additionaldoorsinfo.c:29-32`, `P:\scripts\3_game\enums\ebuildinglocktypes.c:7-16`. |
| **interactPositionPoint** / **interactDirPoint** (new in 1.30) | Memory point names for one-sided open/close. Config keys are these two strings; members are `m_InteractLimitingPositionPoint` / `m_InteractLimitingDirPoint`. | `P:\scripts\3_game\entities\additionaldoorsinfo.c:34-38`, `P:\scripts\4_world\entities\game\super\building.c:335-342`. |
| **doorConstructionPart** / **doorConstructionPhysicsSource** (new in 1.30) | Rebuildable-house door: construction part name and AnimationSource that toggles door physics. | `P:\scripts\3_game\entities\additionaldoorsinfo.c:40-44`, `P:\scripts\4_world\classes\rebuilding\rebuilding.c:439-463`, `P:\scripts\4_world\classes\basebuilding\constructionpart.c:455`. |

Tutorial sound list: **DZ\sounds\hpp\config.cpp** (**WELCOME TO novoGODS Shit Door Tutorial.txt:255-256**). Verify a sound there before adding it.

**Verified: `initPhase` is the initial animation phase value (0..1) at spawn, NOT a start delay.** BI Doors_on_buildings wiki ("initial value for the animation"). The tutorial's "time it takes to start" phrasing is WRONG; real files use `0.0` = spawn closed. (Verified 2026-07-14 vs BI wiki.)

**Verified: `initOpened` is a spawn probability.** BI Doors_on_buildings wiki: when `rand < initOpened` the door spawns opened, so `0` never spawns open and `0.5` spawns open ~half the time. The tutorial's description is correct. (Verified 2026-07-14 vs BI wiki.)

### One source to one Doors entry

Preferred starting rule: create one **class Doors** subclass per unique **source**, with **component** equal to that source. Animated selections sharing the source do not get separate normal door entries. Simple and Button follow this.

? Expert contradicts a universal rule: its model has only **source = "door1_open"** (**Doors_Buttons_Lesson/Expert_Mode/Expert_Mode.cfg:33,45,57**), but config declares **Door1_Open** and **Lever**, both with **component = "door1_open"** (**Doors_Buttons_Lesson/Expert_Mode/config.cpp:18-38**). Ask before copying the second entry.

## DamageSystem contract

| Class/property | Role in supplied examples | Verification |
|---|---|---|
| **class DamageSystem** | Root damage definition. | Simple/button **config.cpp:32-84**; expert **:40-92**. |
| **GlobalHealth > Health** | Global **hitpoints = 1000**. | Simple/button **:34-40**; expert **:42-48**. |
| **GlobalArmor** | Projectile/Melee Health, Blood, Shock damage all **0**. | Simple/button **:41-55**; expert **:49-63**. |
| **DamageZones** | Per-component zones. | Simple/button **:56-83**; expert **:64-91**. |
| Zone **class Door1** | Zone name used in all examples. | Simple/button **:58**; expert **:66**. |
| Zone **class Health** | **hitpoints = 1000**, **transferToGlobalCoef = 0**. | Simple/button **:60-64**; expert **:68-72**. |
| **componentNames[]** | Moving geometry **{"door1"}**, even when source is **door1_open**. | Simple/button **:65**; expert **:73**. |
| **fatalInjuryCoef** | **-1** in every zone. | Simple/button **:66**; expert **:74**. |
| **class ArmorType** | Projectile Health **2**; Melee Health **2.5**; Blood/Shock **0**. | Simple/button **:67-81**; expert **:75-89**. |

Do not replace **DamageZones.componentNames[]** automatically with the Doors component: Button and Expert prove the two can differ.

## Verified mappings

| Pattern | Bones | Animation selections | Unique source(s) | Real Doors component(s) |
|---|---|---|---|---|
| Simple Door | door1 -> ""; handle -> door1 | door1, handle | door1 | door1 |
| Door with Button | door1 -> "" | door1 | door1_open | door1_open |
| Expert Mode | door1 -> ""; handle -> door1; lever -> "" | door1, handle, lever | door1_open | door1_open twice |

## Unverified or conflicting details

Resolved 2026-07-14 (details in the per-property sections above): `angle0/angle1` = radians, `initPhase` = initial phase not a delay, `initOpened` = spawn probability, `scope` 0/1/2 = private/protected/public. All confirmed against the BI wiki + the user's verified skills.

Remaining:

- ? Duplicate skeleton names causing a server crash: tutorial warning, not independently reproduced. Treat skeleton-name uniqueness as a cheap preventive convention.
- Conflict (resolved by authority, not a ?): the one-source/one-Doors rule is contradicted by Expert -- two Doors entries share component `door1_open`. A real quirk of the tutorial's example.
- Conflict (resolved by authority): Expert prose has `soundOpen = "switchOpen"` / `soundClose = "switchClose"`, but the tutorial's real `Doors_Buttons_Lesson/Expert_Mode/config.cpp:31-38` omits both. The real file wins.
- ? No `config.cpp` under `P:\DZ` (1.30.164078, 208 files) sets `relatedInventorySlots[]`, `lockCompatibilityBitMask`, `interactPositionPoint`, `interactDirPoint`, `doorConstructionPart` or `doorConstructionPhysicsSource`; the configs of the script classes most likely to use them (rebuildable `Land_House_*`, `Land_Bunker_*`) are not in the `P:\DZ` extract at all. The parser and script consumers are verified; a copy-paste vanilla Doors block with those keys is **[UNVERIFIED]**. The only config trace is the macro `PART_DOOR_ESSENTIAL_ATTACHMENTS(doorID, doorMaterial)` → `Att_CodeLock<n>_<material>` / `Att_CombinationLock<n>_<material>` (`P:\DZ\data\basicdefines.hpp:662-663`). Fence lists `"Att_CombinationLock"` and `"Att_CodeLock"` in entity-level `attachments[]` (`P:\DZ\gear\camping\config.cpp:4362-4374`), but `Fence` is a `BaseBuildingBase` item (`:3252`), not a `Building`, so per-door `AdditionalDoorInfo` does not apply to it.

## Door open conditions and locks (DayZ 1.30)

Both signatures exist. The `int` form is a wrapper that builds `DoorManipulationParams` and does **not** set `m_Caller`. Vanilla Open uses the params form plus a directional check, and no vanilla script calls the `int` form (see [Hooks vanilla 1.30 calls](#hooks-vanilla-130-calls-and-hooks-it-does-not)).

(Before 1.30: `Building.CanDoorBeOpened(int doorIndex, bool checkIfLocked = false)` was the only script signature, and vanilla `ActionOpenDoors` cast the target to `Building` and called it directly — 1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\4_world\classes\useractionscomponent\actions\interact\actionopendoors.c:36,45,57,63`. `GetLockCompatibilityType` always returned `1 << EBuildingLockType.LOCKPICK` — 1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\3_game\entities\building.c:167-170`.)

```c
// [EXACT] P:\scripts\3_game\entities\building.c:5
class DoorManipulationParams
{
	int m_DoorIndex;
	bool m_CheckIfLocked;
	Object m_Caller;
}
```

```c
// [EXACT] P:\scripts\3_game\entities\building.c:141
	bool CanDoorBeOpened(notnull DoorManipulationParams params)
	{
		if (IsDoorOpen(params.m_DoorIndex))
			return false;

		if (params.m_CheckIfLocked)
		{
			if (IsDoorLocked(params.m_DoorIndex))
				return false;
		}
		else
		{
			if (!IsDoorLocked(params.m_DoorIndex))
				return false;
		}

		return true;
	}
```

```c
// [EXACT] P:\scripts\3_game\entities\building.c:160
	bool CanDoorBeOpened(int doorIndex, bool checkIfLocked = false)
	{
		DoorManipulationParams doorParams = new DoorManipulationParams();
		doorParams.m_DoorIndex = doorIndex;
		doorParams.m_CheckIfLocked = checkIfLocked;

		return CanDoorBeOpened(doorParams);
	}
```

`ActionOpenDoors` casts to `BuildingBase`, fills `m_DoorIndex`, `m_CheckIfLocked`, `m_Caller`, and requires `DoorsDirectionalCheck` before Open (`OnStartServer` repeats the params call without the directional check, `:63-75`):

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\interact\actionopendoors.c:36
		BuildingBase building;
		if (Class.CastTo(building, target.GetObject()))
		{
			DoorManipulationParams doorParams = new DoorManipulationParams();
			doorParams.m_DoorIndex = building.GetDoorIndex(target.GetComponentIndex());
			if (doorParams.m_DoorIndex != -1)
			{
				if (!IsInReach(player, target, UAMaxDistances.DEFAULT)) 
					return false;
				
				if (!building.DoorsDirectionalCheck(player,target.GetComponentIndex()))
					return false;
				
				doorParams.m_CheckIfLocked = CheckIfDoorIsLocked();
				doorParams.m_Caller = player;

				return building.CanDoorBeOpened(doorParams);
			}
		}
```

`BuildingBase` then rejects Open when a related `DigitalCodeLock` or `CombinationLock` is locked:

```c
// [EXACT] P:\scripts\4_world\entities\game\super\building.c:312
		int doorIndex = params.m_DoorIndex;
		AdditionalDoorInfo doorInfo = GetDoorInfo(doorIndex);
		if (doorInfo && doorInfo.m_RelatedInventorySlotNames)
		{
			foreach(string relatedSlotName: doorInfo.m_RelatedInventorySlotNames)
			{
				EntityAI slotEntity = FindAttachmentBySlotName(relatedSlotName);
				if (!slotEntity)
					continue;
				
				DigitalCodeLock codeLock;
				if (Class.CastTo(codeLock, slotEntity) && codeLock.IsLocked())
					return false;
				
				CombinationLock combinationLock;
				if (Class.CastTo(combinationLock, slotEntity) && combinationLock.IsLocked())
					return false;
			}
		}
		
		return true;
```

Directional check (missing/empty memory points pass — the helper returns true):

```c
// [EXACT] P:\scripts\4_world\entities\game\super\building.c:335
	//! checks against player relative to defined door direction, some doors can only be operated from one side
	bool DoorsDirectionalCheck(PlayerBase player, int componentIdx)
	{
		string memPointPos = GetDoorInfo(GetDoorIndex(componentIdx)).m_InteractLimitingPositionPoint;
		string memPointDir = GetDoorInfo(GetDoorIndex(componentIdx)).m_InteractLimitingDirPoint;
		
		return MiscGameplayFunctions.MemPointDirectionalCheck(player, this, memPointPos, memPointDir);
	}
```

```c
// [EXACT] P:\scripts\4_world\static\miscgameplayfunctions.c:940
	//! Intentionally returns true (no restriction) when the check cannot be performed (missing player/entity or memory points)
	//! Callers where a failed check must deny access have to verify the preconditions (e.g. MemoryPointExists) themselves
	static bool MemPointDirectionalCheck(PlayerBase player, EntityAI targetEnt, string memPointStartName, string memPointEndName, float toleranceAngle = Math.PI_HALF)
	{
		if (player && targetEnt && memPointEndName != "" && targetEnt.MemoryPointExists(memPointEndName) && memPointStartName != "" && targetEnt.MemoryPointExists(memPointStartName))
		{
			vector memPointEndPos = targetEnt.ModelToWorld(targetEnt.GetMemoryPointPos(memPointEndName));
			vector memPointStartPos = targetEnt.ModelToWorld(targetEnt.GetMemoryPointPos(memPointStartName));
			vector playerPosition = player.GetPosition();
			vector playerHeadPos = MiscGameplayFunctions.GetPlayerHeadPosition(player);
			vector playerDir = vector.Direction(playerHeadPos, memPointStartPos);
			playerDir.Normalize();
			vector memDir = vector.Direction(memPointStartPos, memPointEndPos);
			memDir.Normalize();
```

If that `if` fails (null player, empty names or missing Memory points), the function returns `true` (`miscgameplayfunctions.c:967`). The 1.30 comment above it makes that explicit: a caller that must deny access on a failed check has to test `MemoryPointExists` itself. `DoorsDirectionalCheck` also dereferences `GetDoorInfo(...)` without a null check (`building.c:338-339`); every child of **class Doors** gets an info object (`P:\scripts\3_game\entities\additionaldoorsinfo.c:65-78`), so this only bites if the engine door index and the config child order disagree.

### `EBuildingLockType` (not attachment locks)

```c
// [EXACT] P:\scripts\3_game\enums\ebuildinglocktypes.c:7
enum EBuildingLockType
{
	NONE = 0, //unlockable only via script
	LOCKPICK,
	SHIP_CONTAINER_0,
	SHIP_CONTAINER_1,
	SHIP_CONTAINER_2,
	SHIP_CONTAINER_3,
	SHIP_CONTAINER_4
}
```

`SHIP_CONTAINER_4` is new in 1.30 (1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\3_game\enums\ebuildinglocktypes.c:1-9` ends at `SHIP_CONTAINER_3`); vanilla uses it for `Land_Mil_ContainerHabitat_Armory_Locked_Green_DE` and `ShippingContainerKeys_Green` (`P:\scripts\4_world\entities\itembase\containerlocked.c:140-146`, `P:\scripts\4_world\entities\itembase\shippingcontainerkeys_colorbase.c:61`).

**[DESIGN]** Combination locks and code locks are `IsExternalLockType()` inventory items (`DigitalCodeLock` / `CombinationLock` override, `P:\scripts\4_world\entities\itembase\codelock.c:542`, `P:\scripts\4_world\entities\itembase\combinationlock.c:779`), not extra `EBuildingLockType` values. Notes that list `COMBINATION_LOCK = 2` / `CODE_LOCK = 4` are false against this file.

`GetLockCompatibilityType` now returns `GetDoorInfo(doorIdx).m_LockCompatibilityBitMask` (`P:\scripts\3_game\entities\building.c:193-196`). A script override of `GetLockCompatibilityType` (as in `containerlocked.c:108-146`) still wins, because the actions call the method, not the field. `ActionLockDoors` / `ActionUnlockDoors` still use that mask against `tool.GetKeyCompatibilityType()`, but they return false first if any related slot already holds an `IsExternalLockType()` item (`P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actionlockdoors.c:40-57`, `...\actionunlockdoors.c:39-56`). Both still cast to `Building`, not `BuildingBase` (`actionlockdoors.c:34`, `actionunlockdoors.c:33`).

Native engine locks are unchanged: `LockDoor`, `UnlockDoor`, `IsDoorLocked` (`P:\scripts\3_game\entities\building.c:55-70`).

## Hooks vanilla 1.30 calls, and hooks it does not

From a grep of every `.c` under `P:\scripts` (1.30.164078). "No vanilla caller" means no script calls it; engine-side callers are not visible and were not tested.

| Hook | Vanilla script callers on 1.30 | Status for a mod override |
|---|---|---|
| `Building.CanDoorBeOpened(notnull DoorManipulationParams)` | `ActionOpenDoors` / `ActionLockedDoors` condition and server start (`...\actions\interact\actionopendoors.c:52,72`); `BuildingBase` and `Bunker` overrides call `super` | **Called.** Put Open conditions here; `params.m_Caller` is the player. |
| `Building.CanDoorBeOpened(int, bool)` | none — only the wrapper body (`building.c:160-167`) and overrides calling `super` (`land_warheadstorage_main.c:352-354`, `land_warheadstorage_bunker_facility.c:20-22`) | **Compiles, not called by Open.** Reached only if your own code calls it; it then forwards to the params form with `m_Caller = null`. |
| `Building.CanDoorBeClosed(int)` | `ActionCloseDoors` (`...\actions\interact\actionclosedoors.c:38,55`) | **Called.** Put Close conditions here. |
| `Building.CanDoorBeClosed(int, Object)` (new in 1.30) | none (`building.c:169-172`; it does not forward to the `int` form, nor the reverse) | **Compiles, never called.** |
| `Building.CanDoorBeLocked(int)` | `ActionLockDoors` (`actionlockdoors.c:57`) | Called (lockpick lock). |
| `Building.GetLockCompatibilityType(int)` | `ActionLockDoors:56`, `ActionUnlockDoors:55`, `ActionUnlockContainerDoor` (`...\actions\continuous\actionunlockcontainerdoor.c:20`) | Called; overrides beat the config mask. |
| `BuildingBase.DoorsDirectionalCheck(PlayerBase, int)` | `ActionOpenDoors:46`, `ActionCloseDoors:35` | Called (overridable). |
| `BuildingBase.IsLocked(int)` (new in 1.30) | none | Returns false on every path (`P:\scripts\4_world\entities\game\super\building.c:283-305`). Do not use it. |
| Script class `: Building` (not `BuildingBase`) | Open / Close cast to `BuildingBase` (`actionopendoors.c:36-37,63-64`, `actionclosedoors.c:26-27,49-50`) | **Compiles, Open / Close never offered.** Lock / unlock with a lockpick still work (`Building` cast). Vanilla classes in this state: `BunkerBroadcastProxy`, `AnniversaryMusicSource`, `AnniversarySpotLight`, `NavmeshTest`. `HouseNoDestruct` has no script class; its config parent is `House` (`P:\DZ\data\config.cpp:3119`), whose script class is `House : BuildingBase` (`building.c:380`). |

(Before 1.30, every Open / Close action cast to `Building` and called `CanDoorBeOpened(int, bool)` / `CanDoorBeClosed(int)` — 1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\4_world\classes\useractionscomponent\actions\interact\actionopendoors.c:36,45,57,63`, `...\actionclosedoors.c:26,35`.)

Porting rule: a 1.29 mod that overrode `CanDoorBeOpened(int, bool)` keeps compiling and silently stops restricting Open. Move the body to `override bool CanDoorBeOpened(notnull DoorManipulationParams params)`, read `params.m_DoorIndex` / `params.m_CheckIfLocked`, and call `super.CanDoorBeOpened(params)`. If the mod also has to load on 1.29, follow `../../_shared/dayz-version-policy.md` rule 1 (old code under `#ifdef DAYZ_1_29`).

## Sources

novoGOD's tutorial *Doors_Buttons_Lesson* (files dated 2025-04-23; no license, not shipped with the Pack). Texts read in full:

- **WELCOME TO novoGODS Shit Door Tutorial.txt**
- **Simple_Door Readme.txt**
- **Door_w_Button Readme.txt**
- **Welcome to novoGODs Expert_Mode Door mod.txt**

Example files:

- **Doors_Buttons_Lesson/Door/Simple_Door.cfg** and **Doors_Buttons_Lesson/Door/config.cpp**
- **Doors_Buttons_Lesson/Door_w_Button/Door_w_Button.cfg** and **Doors_Buttons_Lesson/Door_w_Button/config.cpp**
- **Doors_Buttons_Lesson/Expert_Mode/Expert_Mode.cfg** and **Doors_Buttons_Lesson/Expert_Mode/config.cpp**

Official references cited by tutorials, not fetched during authoring:

- https://community.bistudio.com/wiki/DayZ:Doors_on_buildings
- https://github.com/BohemiaInteractive/DayZ-Samples
- https://community.bistudio.com/wiki/LOD
