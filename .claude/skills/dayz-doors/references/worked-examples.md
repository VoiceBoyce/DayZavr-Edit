# Worked examples

## Contents

- [Where these examples come from](#where-these-examples-come-from)
- [Simple Door](#simple-door-door-plus-handle)
- [Door with Button](#door-with-button-static-controller)
- [Expert Mode](#expert-mode-door-plus-handle-plus-lever)
- [Cross-pattern diff](#cross-pattern-diff)
- [Source discrepancies](#source-discrepancies)
- [Building locks (DayZ 1.30)](#building-locks-dayz-130)

## Where these examples come from

The three doors on this page are the examples of novoGOD's door tutorial *Doors_Buttons_Lesson*: three models, each with its model.cfg and config.cpp, and four tutorial texts, files dated 2025-04-23. The tutorial's archive holds no license or permission text, so the Pack does not ship its files. This page describes each example in its own words and tables, and cites the tutorial's files by path and line, such as `Doors_Buttons_Lesson/Door/Simple_Door.cfg:29-40`. Pack v1.3.0 to v1.5.0 shipped those files under `assets/`, wrongly labelled as the Pack author's MIT work; they were removed on 2026-10-02.

Read each example as a pair. model.cfg declares the bones, the selections that move and the source that drives them; config.cpp binds that source to a **class Doors** entry and sets its timing, sounds and damage. The three share their scaffolding (a CfgPatches entry requiring **"DZ_Data"**, an empty **Default** model class, **skeletonInherit = ""**, **isDiscrete = 0**, **scope = 1** on a **HouseNoDestruct** child) and one DamageSystem, the same lines in all three files; [door-model-cfg-and-config.md](door-model-cfg-and-config.md) describes those once, with line citations. The tables below keep to what each pattern adds.

## Simple Door: door plus handle

A door that swings on its hinge, and a handle that turns first and then rides along with the door. One source, **door1**, drives both.

### Side-by-side mapping

| Tutorial model.cfg | Tutorial config.cpp |
|---|---|
| **door1 -> ""**, **handle -> door1**. | One Doors entry **Door1**. |
| Door and handle source **door1**. | **component = "door1"**. |
| Handle **0..0.15**; door **0.15..1**. | **animPeriod = 1.3**; init values **0.0**. |

The handle follows the door transform. It reaches angle1 **-1.4** by phase **0.15**; then the door begins its rotation to **1.9**.

### Bones (`Doors_Buttons_Lesson/Door/Simple_Door.cfg:1-14`)

| Bone | Parent | Effect |
|---|---|---|
| **door1** | none (**""**) | Turns about its own axis. |
| **handle** | **door1** | Carried by the door's rotation, on top of its own turn. |

The skeleton class is **Simple_DoorSkeleton**; the model class **Simple_Door**, named after the P3D, points at it through **skeletonName** (`:23-25`).

### Animations (`Doors_Buttons_Lesson/Door/Simple_Door.cfg:27-53`)

| Class | selection | source | axis | Phase window | Turn, radians |
|---|---|---|---|---|---|
| **Door1** | **door1** | **door1** | **door1_axis** | 0.15 to 1 | 0 to 1.9 |
| **handle** | **handle** | **door1** | **handle_axis** | 0 to 0.15 | 0 to -1.4 |

Both are rotations about a Memory LOD axis (**type = "rotation"**, **memory = 1**). The windows do not overlap: the handle turns during the first 15 % of the action, and the door swings during the rest.

### Doors entry (`Doors_Buttons_Lesson/Door/config.cpp:16-31`)

| Doors class | component | soundPos | animPeriod | initPhase | initOpened | Sounds |
|---|---|---|---|---|---|---|
| **Door1** | **door1** | **door1_action** | 1.3 s | 0.0 | 0.0 | **doorMetalSmall** Open, Close, Rattle (locked), OpenABit |

The entity class is **land_Simple_Door** (`:12`); its **model** points at the addon's `Door\Simple_Door.p3d` (`:15`).

## Door with Button: static controller

A door opened from a button next to it. The button never moves, so it is not a bone: it only carries the interaction selection **door1_open**, and that name is the source of the door's rotation.

### Side-by-side mapping

| Tutorial model.cfg | Tutorial config.cpp |
|---|---|
| Only bone **door1 -> ""**; no button bone. | One Doors entry **Door1_Open**. |
| Moving **door1**; source **door1_open**. | **component = "door1_open"**. |
| Door phase **0..1**. | **soundPos = "door1_action"**, period **1.0**. |

The button is static. The tutorial places View Geometry selection and interaction point at the button as **door1_open**; interacting there drives moving selection **door1**.

### Bones (`Doors_Buttons_Lesson/Door_w_Button/Door_w_Button.cfg:1-12`)

| Bone | Parent | Effect |
|---|---|---|
| **door1** | none (**""**) | The only bone; the button is not listed. |

### Animations (`Doors_Buttons_Lesson/Door_w_Button/Door_w_Button.cfg:25-39`)

| Class | selection | source | axis | Phase window | Turn, radians |
|---|---|---|---|---|---|
| **Door1** | **door1** | **door1_open** | **door1_axis** | 0 to 1 | 0 to 1.9 |

The selection that moves (**door1**) and the source that drives it (**door1_open**) have different names; that split is the whole pattern.

### Doors entry (`Doors_Buttons_Lesson/Door_w_Button/config.cpp:16-31`)

| Doors class | component | soundPos | animPeriod | initPhase | initOpened | Sounds |
|---|---|---|---|---|---|---|
| **Door1_Open** | **door1_open** | **door1_action** | 1.0 s | 0.0 | 0.0 | the same four **doorMetalSmall** sounds |

The DamageZone still names the moving part, **componentNames[] = {"door1"}** (`:65`), not the button's **door1_open**.

## Expert Mode: door plus handle plus lever

A door with a handle, opened from a separate lever. All three parts move from one source, **door1_open**, which the lever carries as its interaction selection.

### Side-by-side mapping

| Tutorial model.cfg | Tutorial config.cpp |
|---|---|
| door root, handle child, lever root. | Main **Door1_Open**, secondary **Lever**. |
| All source **door1_open**. | Both component **door1_open**. |
| Door starts **0.10**; lever ends **0.5**. | Main **initOpened = 0.5**; Lever period **0.50**. |

The handle follows the moving door. The lever is an independent root at the controller. All three animations share one source.

? The tutorial's config has two Doors entries for one source/component, contradicting a universal one-entry-per-source rule. The secondary Lever also omits **soundPos** and all sounds. Ask before reusing that duplicate mapping.

### Bones (`Doors_Buttons_Lesson/Expert_Mode/Expert_Mode.cfg:1-14`)

| Bone | Parent | Effect |
|---|---|---|
| **door1** | none (**""**) | Turns about its own axis. |
| **handle** | **door1** | Carried by the door's rotation, on top of its own turn. |
| **lever** | none (**""**) | Turns about its own axis, independent of the door. |

### Animations (`Doors_Buttons_Lesson/Expert_Mode/Expert_Mode.cfg:27-65`)

| Class | selection | source | axis | Phase window | Turn, radians |
|---|---|---|---|---|---|
| **Door1** | **door1** | **door1_open** | **door1_axis** | 0.10 to 1 | 0 to 1.9 |
| **Handle** | **handle** | **door1_open** | **handle_axis** | 0 to 1 | 0 to -1.7 |
| **Lever** | **lever** | **door1_open** | **lever_axis** | 0 to 0.5 | 0 to -0.88 |

Here the windows overlap: the lever finishes its throw by mid-action, the handle turns over the whole action, and the door starts at 10 %.

### Doors entries (`Doors_Buttons_Lesson/Expert_Mode/config.cpp:16-39`)

| Doors class | component | soundPos | animPeriod | initPhase | initOpened | Sounds |
|---|---|---|---|---|---|---|
| **Door1_Open** | **door1_open** | **door1_action** | 1.0 s | 0.0 | 0.5 | the four **doorMetalSmall** sounds |
| **Lever** | **door1_open** | none | 0.50 s | 0.0 | 0.0 | none |

With **initOpened = 0.5** the main door spawns open about half the time (the **initOpened** note in [door-model-cfg-and-config.md](door-model-cfg-and-config.md)). Its DamageZone names **door1** (`:73`).

## Cross-pattern diff

| Concern | Simple Door | Door with Button | Expert Mode |
|---|---|---|---|
| Moving bones | Door + handle | Door only | Door + handle + lever |
| Parent graph | Handle follows door | Door root | Handle follows door; lever root |
| Interactive source | **door1** | **door1_open** at button | **door1_open** at lever |
| Static controller bone | N/A | Button omitted | N/A; lever animates |
| Phase sequencing | Handle 0..0.15; door 0.15..1 | Door 0..1 | Door 0.10..1; handle 0..1; lever 0..0.5 |
| Doors components | **door1** | **door1_open** | **door1_open** twice |
| DamageZone component | **door1** | **door1** | **door1** |

No pattern introduces **AnimationSources** or script calls; those belong to **dayz-animation-pipeline**.

## Source discrepancies

- Where the tutorial's texts and its files disagree, the files are authoritative.
- Expert prose at **Welcome to novoGODs Expert_Mode Door mod.txt:128-137** shows **switchOpen/switchClose**.
- The tutorial's **Doors_Buttons_Lesson/Expert_Mode/config.cpp:31-38** has neither sound.
- ? No in-game test established the exact purpose/behavior of the duplicate Expert Lever Doors entry.

## Building locks (DayZ 1.30)

The three tutorial buildings above have no lock slots. `House` is a `BuildingBase` (as in 1.29), and in 1.30 `BuildingBase` accepts `CombinationLock` and `DigitalCodeLock` as attachments and binds them per door through `AdditionalDoorInfo`. (Before 1.30: a world-building door locked only through engine `LockDoor` / `UnlockDoor` and lockpick; 1.29 `BuildingBase` had no lock code at all — 1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\4_world\entities\game\super\building.c:1-92`.)

The player-built `Fence` (gate) lists both slots at **entity** level — not inside **class Doors**. `Fence` is a `BaseBuildingBase` item (`P:\DZ\gear\camping\config.cpp:3252`), not a `Building`, so this is a slot-name reference, not a door pattern:

```cpp
// [EXACT] P:\DZ\gear\camping\config.cpp:4362
		attachments[]=
		{
			"Wall_Barbedwire_1",
			"Wall_Barbedwire_2",
			"Wall_Camonet",
			"Att_CombinationLock",
			"Att_CodeLock",
			"Material_Nails",
			"Material_WoodenPlanks",
			"Material_MetalSheets",
			"Material_WoodenLogs",
			"Material_MetalWire"
		};
```

`BuildingBase` slot name constants:

```c
// [EXACT] P:\scripts\4_world\entities\game\super\building.c:8
class BuildingBase : Building
{
	const string ATTACHMENT_SLOT_COMBINATION_LOCK = "Att_CombinationLock";
	const string ATTACHMENT_SLOT_CODE_LOCK 	= "Att_CodeLock";
```

`CfgSlots` selections the P3D Memory/proxy must match:

```cpp
// [EXACT] P:\scripts\config.cpp:2316
	class Slot_Att_CombinationLock
	{
		name="Att_CombinationLock";
		displayName="#STR_CombinationLock0";
		selection="att_combinationlock";
		ghostIcon="set:dayz_inventory image:combolock";
	};
```

```cpp
// [EXACT] P:\scripts\config.cpp:2768
	class Slot_Att_CodeLock
	{
		name="Att_CodeLock";
		displayName="#STR_CfgVehicles_codelock0";
		selection="att_codelock";
		ghostIcon="set:dayz_inventory image:digitallock";
	};
```

`P:\scripts\config.cpp` also defines numbered variants `Att_CombinationLock1_Wood` … `Att_CombinationLock10_Metal` and `Att_CodeLock1_Wood` … `Att_CodeLock10_Metal` (selections `att_combinationlock1_wood`, `att_codelock1_wood`, …; 42 lock slot classes in all, `:2316-2914`).

### `AdditionalDoorInfo` on each `class Doors` child

Parsed once per building type in `Building.InitializeDoorInfos` (`P:\scripts\3_game\entities\building.c:302-310`). Config keys (not the C++ member names):

```c
// [EXACT] P:\scripts\3_game\entities\additionaldoorsinfo.c:1
//! Single door info
class AdditionalDoorInfo
{
	protected string m_DoorPath;
	
	string m_DoorType;
	string m_InteractLimitingPositionPoint; //! memory point name for limiting open/close actions on the door/window
	string m_InteractLimitingDirPoint; //! memory point name for limiting open/close actions on the door/window
	string m_DoorConstructionPartName; //! Name of the construction part linked to the doors, if any (empty default)
	string m_DoorConstructionPhysicsSource; //! Name of the AnimationSource controlling door physics (required for rebuildable doors, empty default)
	int m_LockCompatibilityBitMask; //! Lock compatibility type, now in door config. Lockpick enabled by default if undefined.
	ref TStringArray m_RelatedInventorySlotNames; //! Inventory slots that have something to do with this door. Mostly external locks?
```

```c
// [EXACT] P:\scripts\3_game\entities\additionaldoorsinfo.c:21
	void Init()
	{
		if (g_Game.ConfigIsExisting(m_DoorPath + " relatedInventorySlots"))
		{
			m_RelatedInventorySlotNames = new TStringArray();
			g_Game.ConfigGetTextArray(m_DoorPath + " relatedInventorySlots", m_RelatedInventorySlotNames);
		}
		
		if (g_Game.ConfigIsExisting(m_DoorPath + " lockCompatibilityBitMask"))
			m_LockCompatibilityBitMask = g_Game.ConfigGetInt(m_DoorPath + " lockCompatibilityBitMask");
		else
			m_LockCompatibilityBitMask = 1 << EBuildingLockType.LOCKPICK; //default behavior if not configured
		
		if (g_Game.ConfigIsExisting(m_DoorPath + " interactPositionPoint"))
			m_InteractLimitingPositionPoint = g_Game.ConfigGetTextOut(m_DoorPath + " interactPositionPoint");
		
		if (g_Game.ConfigIsExisting(m_DoorPath + " interactDirPoint"))
			m_InteractLimitingDirPoint = g_Game.ConfigGetTextOut(m_DoorPath + " interactDirPoint");
		
		if (g_Game.ConfigIsExisting(m_DoorPath + " doorConstructionPart"))
			m_DoorConstructionPartName = g_Game.ConfigGetTextOut(m_DoorPath + " doorConstructionPart");
		
		if (g_Game.ConfigIsExisting(m_DoorPath + " doorConstructionPhysicsSource"))
			m_DoorConstructionPhysicsSource = g_Game.ConfigGetTextOut(m_DoorPath + " doorConstructionPhysicsSource");
	}
```

The holder keys each `AdditionalDoorInfo` by the position of its child inside **class Doors** (`additionaldoorsinfo.c:59-80`); every door-index lookup (`GetDoorInfo(doorIdx)`, `building.c:312-315`) assumes that position equals the engine door index.

**[DESIGN]** A lockable custom house door therefore needs, in addition to the tutorial **class Doors** block:

1. Entity `attachments[]` containing `Att_CombinationLock` and/or `Att_CodeLock` (and numbered wood/metal variants if you copy rebuildable-house slot naming).
2. On that door's Doors subclass: `relatedInventorySlots[] = {"Att_CombinationLock","Att_CodeLock"};` so Open, lockpick, and attach-to-construction know which slots belong to which door.
3. Optional `lockCompatibilityBitMask` if lockpick / ship-container keys should differ from the default.
4. Memory selections `att_combinationlock` / `att_codelock`.
5. Optional `interactPositionPoint` / `interactDirPoint` for one-sided operation.
6. A construction component: `ActionAttachToConstruction` is only offered when `CanUseConstruction()` is true (`P:\scripts\4_world\classes\useractionscomponent\actions\singleuse\actionattachtoconstruction.c:47-48,151`), and the building's inventory is only shown under the same condition (`P:\scripts\3_game\entities\building.c:328-331`). `EntityAI` creates the component only when `cfgVehicles <class> Construction` exists (`P:\scripts\3_game\entities\entityai.c:248-249,3464-3488`); for a `BuildingBase` it is a `Rebuilding` (`P:\scripts\4_world\entities\game\super\building.c:23-27`). ? What the smallest working `class Construction` for a lock-only custom house looks like was not tested.
7. A script class derived from `House`, not `Building` (Open / Close cast to `BuildingBase`; see [Hooks vanilla 1.30 calls](door-model-cfg-and-config.md#hooks-vanilla-130-calls-and-hooks-it-does-not)).

Vanilla reference (the 1.30.164200 `P:\DZ` extract has the Takistan houses; the 164078 extract here did not): `Land_House_1M1` (`P:\DZ\structures_takistan\Residential\config.cpp:18-45`) — door `door1_door_wood` with `relatedInventorySlots[]={"Att_CodeLock1_Wood","Att_CombinationLock1_Wood"};`, `lockCompatibilityBitMask=0;`, `doorConstructionPart="door1_wood";`, `doorConstructionPhysicsSource="phx_door1_move";` (`:25-45`); its windows add `interactPositionPoint` / `interactDirPoint` (`:80-81`). The house also sets `disableSimulation=1;` (`:22`). These are rebuildable doors; a plain custom house needs only the lock keys (items 1-7 above).

### Attaching a lock to a building door

`ActionAttachToConstruction` (building branch) resolves the slot from `relatedInventorySlots[]` only when the door is closed and not engine-locked:

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\singleuse\actionattachtoconstruction.c:151
		else if (Class.CastTo(building, targetEntity) && building.CanUseConstruction())	// Building entities
		{
			if (item.IsExternalLockType()) // External lock handling of building objects with doors and lock attachments and internal locks
			{
				int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
				if (doorIndex > -1 && !building.IsDoorLocked(doorIndex) && !building.IsDoorOpen(doorIndex))
				{
					AdditionalDoorInfo doorInfo = building.GetDoorInfo(doorIndex);
					if (!doorInfo || !doorInfo.m_RelatedInventorySlotNames)
						return InventorySlots.INVALID;
					
					foreach (string slotName: doorInfo.m_RelatedInventorySlotNames)
					{
						EntityAI slotEntity = building.FindAttachmentBySlotName(slotName);
						if (slotEntity)
							continue;
						
						slotId = InventorySlots.GetSlotIdFromString(slotName);
						GameInventory inventory = building.GetInventory();
						if (inventory && inventory.CanAddAttachmentEx(item, slotId))
							break;
```

`BuildingBase.CanReceiveAttachment` refuses a second lock on the twin slot (`HasConflictingDoorAttachment` swaps `Att_CodeLock` ↔ `Att_CombinationLock` prefixes and keeps the suffix, e.g. `Att_CodeLock1_Wood` ↔ `Att_CombinationLock1_Wood`) and refuses any external lock while that door is open (`P:\scripts\4_world\entities\game\super\building.c:192-250,344-356`). `CanReleaseAttachment` refuses while `DigitalCodeLock.IsLocked()` or `CombinationLock.IsLocked()` (`:267-281`). `CanDisplayAttachmentSlot` hides the twin slot once one lock is on (`:252-265`).

? Read from code, not tested: the scan in `CanReceiveAttachment` stops (`skipDoorInfoCheck`) at the first **class Doors** entry it meets that has no `relatedInventorySlots[]`, and then neither the twin-slot nor the door-open check runs (`building.c:209-216,236-244`). On a building where only some doors are lockable, expect those two checks to be skipped depending on map order; the attach action still checks closed / not engine-locked itself (`actionattachtoconstruction.c:156`).

### Combination lock: two faces and explicit unlock

Matching dials is not enough: `ActionCombinationLockUnlock` must run, and it calls `UnlockOnServer` (`P:\scripts\4_world\classes\useractionscomponent\actions\singleuse\actioncombinationlockunlock.c:57-82`; vanilla `CombinationLock.SetActions` registers it, `P:\scripts\4_world\entities\itembase\combinationlock.c:686`). Inside and outside combinations are separate. (Before 1.30: `ActionDialCombinationLockOnTarget` called `UnlockServer` as soon as the last dial matched — 1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\4_world\classes\useractionscomponent\actions\continuous\actiondialcombinationlockontarget.c:67-79`; `UnlockServer` no longer exists.)

```c
// [EXACT] P:\scripts\4_world\entities\itembase\combinationlock.c:25
class CombinationLock extends ItemBase
{
	static const string LOCK_SELECTION_OUTSIDE = "lock_attached_outside";
	static const string LOCK_SELECTION_INSIDE = "lock_attached_inside";
	static const int FACING_OUTSIDE = 0;
	static const int FACING_INSIDE = 1;
```

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\singleuse\actioncombinationlockunlock.c:1
class ActionCombinationLockUnlock: ActionInteractBase
{
	void ActionCombinationLockUnlock()
	{
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_Text = "#detach_combination_lock";
	}
```

Vanilla stream v143 adds `m_CombinationInside`:

```c
// [EXACT] P:\scripts\4_world\entities\itembase\combinationlock.c:128
override void OnStoreSave( ParamsWriteContext ctx )
{   
	super.OnStoreSave(ctx);
	
	//write data
	ctx.Write( m_Combination );
	ctx.Write( m_CombinationLocked );
	ctx.Write( m_CombinationInside );
}
```

```c
// [EXACT] P:\scripts\4_world\entities\itembase\combinationlock.c:169
		if (version >= 143)						//added with 143
		{
			//combination two (inside lock)
			if (!ctx.Read(m_CombinationInside))
			{
				m_CombinationInside = 0;
				return false;
			}
		}
```

### Digital code lock

Script item class is `DigitalCodeLock` (`P:\scripts\4_world\entities\itembase\codelock.c:1`), with subclass `DigitalCodeLock_Bunker` (`:567`). Config: `class DigitalCodeLock: Inventory_Base` (`P:\DZ\gear_takistan\camping\config.cpp:19`, model `\dz\gear_takistan\camping\codelock.p3d`; `inventorySlot[]` = `Att_CodeLock` + `Att_CodeLock1_Wood` … `Att_CodeLock10_Metal` `:96-119`; AnimationSources `CodelockBackTranslation` / `CodelockFrontTranslation` / `CodelockBackRotation` / `CodelockRotation` `:286-297`), `DigitalCodeLock_Bunker` at `:315` (the 1.30.164078 extract here lacked `gear_takistan`). It owns `CodeLockComponent` and `CodeLockVisualManager` (`codelock.c:9-10`). `CodeLockVisualManager` poses the lock on its hierarchy root: on a `House` the back part is at phase `0.50` and the front part at `0.50` only when reversed; on a `BaseBuildingBase` (Fence) the back part is at `0.92` (`P:\scripts\4_world\classes\codelockvisualmanager.c:43-52,438-465`). 1.30.164200 changed the `House` back part from 0.60 (0.50 reversed) to a constant 0.50 and made all these numbers `private const`; a custom lock pose belongs in an override of the `protected` `GetAnimationPhaseForFrontPart` / `GetAnimationPhaseForBackPart`. Battery slot `BatteryD`, item `Battery9V` (`codelock.c:6,307-309`). PIN 4–6 digits (`P:\scripts\4_world\classes\codelockcomponent.c:23-24`). UI: `DigitalCodeLockUI.GetLayout()` returns `"gui/layouts/day_z_digital_lock.layout"` (`P:\scripts\5_mission\gui\digitalcodelockui.c:127-129`; the file is `P:\gui\layouts\day_z_digital_lock.layout`). Memory on the **item**: `ce_center`, `inside` (`codelock.c:3-4`).

```c
// [EXACT] P:\scripts\4_world\classes\codelockcomponent.c:4
enum CodeLockStage
{
	NONE,
	NORMAL,
	SETTING,
	PROTECTION_STAGE_ONE,
	PROTECTION_STAGE_TWO
}
```

Brute-force thresholds: `CfgGameplayHandler.GetExternalLockProtectionCountStageOne`, `GetExternalLockProtectionCountStageTwo`, `GetExternalLockProtectionTime`, `GetExternalLockProtectionResetTime` (`P:\scripts\3_game\cfggameplayhandler.c:513-531`, `codelockcomponent.c:66-67,468-483`), read from `cfggameplay.json` `ExternalLockData` (`protectionCountStageOne = 3`, `protectionCountStageTwo = 1`, `protectionTime = 30.0`, `protectionResetTime = 3600.0` by default, `P:\scripts\3_game\cfggameplaydatajson.c:22,421-438`). The component falls back to 30 s / 3600 s when the time values are ≤ 0 (`codelockcomponent.c:26-27,69-73`). There is **no** `GetExternalLockProtectionTimeStageOne` on 1.30.

Component persist order: `m_LockPIN` (string), `m_IsLocked` (bool), `m_DoorIndex` (int) (`codelockcomponent.c:257-261`). The lock finds its door index from `relatedInventorySlots[]` when it is attached (`codelock.c:172-212`).

### Bunker door (broadcast)

`Bunker : House` overrides `CanDoorBeOpened(DoorManipulationParams)`: inside + inactive uses `MemPointDirectionalCheck` on `{m_DoorType}_action` / `{m_DoorType}_inside` (`m_DoorType` is the **class Doors** child name); outside requires `m_IsActive` and an unlocked `DigitalCodeLock_Bunker`. Main door (index `GetMainDoorIndex()`, 0 by default) auto-closes at 12 s and re-locks on close start (`P:\scripts\4_world\entities\building\bunker.c:3,73-90,103-113`). `CanDoorBeLocked` always returns false, so no lockpick (`:66-69`).

```c
// [EXACT] P:\scripts\4_world\entities\building\bunker.c:47
	override bool CanDoorBeOpened(notnull DoorManipulationParams params)
	{
		if (IsMainDoor(params.m_DoorIndex))
		{
			AdditionalDoorInfo doorInfo = GetDoorInfo(params.m_DoorIndex);
			string memPointStartName = string.Format("%1_action", doorInfo.m_DoorType);
			string memPointEndName = string.Format("%1_inside", doorInfo.m_DoorType);
			
			bool insideAndInactive = MiscGameplayFunctions.MemPointDirectionalCheck(PlayerBase.Cast(params.m_Caller), this, memPointStartName, memPointEndName) && super.CanDoorBeOpened(params);
			bool outside = m_IsActive && !GetCodeLock().IsLocked() && super.CanDoorBeOpened(params);

			return insideAndInactive || outside;
		}
		
		return super.CanDoorBeOpened(params);
	}
```

If you copy this pattern, test `MemoryPointExists` for both names first: `MemPointDirectionalCheck` returns true when a point is missing or `m_Caller` is null (`P:\scripts\4_world\static\miscgameplayfunctions.c:942-946,969`), so a model without `Door1_action` / `Door1_inside` would open from "inside" everywhere (read from code, not tested in game).

`BroadcastedBunker` binds `DigitalCodeLock_Bunker` on slot string `"att_codelock"`: the server creates it in that slot if it is empty, and the client retries every 200 ms until the attachment has replicated (`bunker.c:170-224`).

### Rebuildable house doors

`Rebuilding.OnConstructionDoorOpenStart` / `OnConstructionDoorCloseStart` set `doorConstructionPhysicsSource` phase 1.0 / 0.0 (`P:\scripts\4_world\classes\rebuilding\rebuilding.c:439-463`). When that construction part is dismantled or destroyed, `ConstructionPart.CloseDoorOnPartUnbuild` closes every open door whose `doorConstructionPart` names it (`P:\scripts\4_world\classes\basebuilding\constructionpart.c:442-464`). Put `doorConstructionPart` and `doorConstructionPhysicsSource` on the Doors child if the door is a rebuildable construction part.

### Script override trap

Vanilla Open no longer calls `CanDoorBeOpened(int, bool)`, and nothing calls the new `CanDoorBeClosed(int, Object)`. Override `CanDoorBeOpened(notnull DoorManipulationParams params)` for Open and `CanDoorBeClosed(int)` for Close. `Land_WarheadStorage_Main.CanDoorBeOpened(int doorIndex, bool checkIfLocked = false)` (`P:\scripts\4_world\entities\building\underground\land_warheadstorage_main.c:352`) and `Land_WarheadStorage_Bunker_Facility.CanDoorBeOpened(int, bool)` (`...\underground\land_warheadstorage_bunker_facility.c:20`) are the old signature still present in vanilla 1.30 — do not treat them as the current Open path. Full list: [Hooks vanilla 1.30 calls](door-model-cfg-and-config.md#hooks-vanilla-130-calls-and-hooks-it-does-not).
