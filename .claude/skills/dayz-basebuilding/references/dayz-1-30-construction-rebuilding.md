# DayZ 1.30 — Construction hierarchy, Rebuilding, masonry, Fence code lock

First authored 2026-09-16 (P08) from the 1.30.164014 Experimental scripts (Digest J as the map).
**Re-verified 2026-10-15 against `P:\scripts` / `P:\DZ` of 1.30.164078 stable**: every `[EXACT]` block below was
re-copied from the stable file named in its header (paths relative to `P:\scripts\` unless they start with
`P:\DZ`). The construction code did not differ between Exp and stable except for line numbers; the few
statements that were wrong on both builds are corrected in place. Nothing here was exercised in-game unless
a sentence says so. `Before 1.30:` notes cite `1.29: O:\DayZ 1.29 …\scripts\…`.
**Updated 2026-10-21 for 1.30.164200** (line refs renumbered by tool, content re-read by hand): the support
getters, utility parts, `IgnoreHouseCollisions` and `DropNonUsableMaterialsServer` changed — see §4 (support
graph), §9.2, §9.4 and the new §11. `P:\DZ` of 164200 also contains the Takistan data (`structures_takistan`,
`gear_takistan`), so the "not unpacked here" notes of earlier versions are resolved in §5 and §9.4.

## 1. Type hierarchy

`ConstructionBasic` in `3_game` is a **stub** (`3_game/systems/construction_basic.c:2-44`) — it does **not**
contain `GetParent()` / `IsPartConstructed()` / `HasBase()`. Those live on `ConstructionBase` in `4_world`.

```c
// [EXACT] 4_world/classes/constructionbase.c:30
class ConstructionBase : ConstructionBasic
```

```c
// [EXACT] 4_world/classes/basebuilding/construction.c:2
class Construction : ConstructionBase
```

```c
// [EXACT] 4_world/classes/rebuilding/rebuilding.c:1
class Rebuilding : ConstructionBase
```

`EntityAI.CreateConstructionComponent()` returns null (`3_game/entities/entityai.c:3474-3478`).
`ConstructionInit()` (`:3464-3472`, `protected`) only creates and `Init()`s a component when none exists yet
and the override returns one; it runs from the `EntityAI` constructor when the type has a `Construction`
config class (`:248-249`) and again from `BaseBuildingBase`'s constructor / `OnCreatePhysics`
(`basebuildingbase.c:51,493`), where it is then a no-op.
Before 1.30: `BaseBuildingBase.ConstructionInit()` was public, created `new Construction(this)` and re-ran
`Init()` on every call (1.29: `basebuildingbase.c:965-973`).

```c
// [EXACT] 4_world/entities/itembase/basebuildingbase.c:15
	protected ref ConstructionBasic 	m_Construction;
```

```c
// [EXACT] 4_world/entities/itembase/basebuildingbase.c:872-886
	override protected ConstructionBasic CreateConstructionComponent()
	{
		m_Construction = new Construction(this);
		return m_Construction;
	}
	
	override ConstructionBasic GetConstructionBasic()
	{
		return m_Construction;
	}
	
	Construction GetConstruction()
	{
		return Construction.Cast(m_Construction);
	}
```

```c
// [EXACT] 4_world/entities/game/super/building.c:23-27
	override protected ConstructionBasic CreateConstructionComponent()
	{
		m_Construction = new Rebuilding(this);
		return m_Construction;
	}
```

`Construction` itself gained an `EntityAI` constructor and a widened `SetParent`:

```c
// [EXACT] 4_world/classes/basebuilding/construction.c:6-19
	void Construction( EntityAI parent )
	{
		m_ConstructionParts = new map<string, ref ConstructionPart>;
		
		//set parent object
		SetParent(parent);
	}
	
	//legacy constructor (unused)
	void Construction( BaseBuildingBase parent )
	{
		m_ConstructionParts = new map<string, ref ConstructionPart>;
		SetParent(parent);
	}
```

`SetParent` is `override protected void SetParent( EntityAI parent )` (`construction.c:44-49`; base
`constructionbase.c:120-123`). Before 1.30: `protected void SetParent( BaseBuildingBase parent )`
(1.29: `construction.c:44`) — a 1.29 override of that signature no longer compiles (also reported by bzed, tested).

[DESIGN] Mods that stored `Construction m_Construction` on a `BaseBuildingBase` subclass should switch to `GetConstruction()` (already a `Construction.Cast`) or `Construction.Cast(GetConstructionBasic())`. Direct field access from outside the class no longer compiles (`protected`). To install your own `Construction` subclass, override `CreateConstructionComponent()`.

## 2. `ConstructionPartTypeData` vs `ConstructionPart`

Type data is held on `EntityAIType` (`3_game/entities/entityaitype.c:4,54,66-85`) via `ConstructionDataTypeHolder`.
Before 1.30: each `ConstructionPart` stored its config fields and `UpdateConstructionParts` re-read the config per
instance (1.29: `construction.c:235-270`; `constructionpart.c:1-22`).

`ConstructionDataTypeHolderBasic` in `3_game` is also a stub (`3_game/systems/constructiontypedata_basic.c:2-5`). The real holder is `ConstructionDataTypeHolder` (`4_world/classes/basebuilding/typeconstructiondata/constructionparttypedholder.c:2-63`), which walks `Construction > <main> > <part>` once per type.

```c
// [EXACT] 4_world/classes/basebuilding/constructionpart.c:2-16
//! Instanced construction part data classes for dynamic data, statics now stored on EntityType level in ConstructionPartTypeData
class ConstructionPart
{
	//TODO: start with 2 bits per part, and iterate into differentiated simple/complex part structure
	static const int BITWISE_SYNCINFO_SIZE_BASE = 2; //! no. of bits to store synced info, per part. System not ready for variable size parts yet.
	protected const int ANIMATION_EFFECT_DELAY_BUILD = 350; //ms
	
	protected ConstructionPartTypeData m_PartTypeData; //! Entity-type object storing non-instanced data
	protected int 	m_LocalSyncBitMask = 0;
	bool 	m_IsBuilt;			//defines part build state
	protected ref TConstructionPartInsiderBoxData m_InsiderBoxData; //Grouped modelspace positions of min/max insidebox points
	
	void ConstructionPart( string name, string part_name, string main_part_name, int id, bool is_built, bool is_base, bool is_gate, array<string> required_parts )
	{
	}
```

Digest J claimed a new ctor `ConstructionPart(notnull ConstructionPartTypeData)`. That ctor is **not** in the file. Type data is assigned after spawn:

```c
// [EXACT] 4_world/classes/basebuilding/constructionpart.c:401-407
	void SetPartTypeData(ConstructionPartTypeData ptd)
	{
		if (!m_PartTypeData)
			m_PartTypeData = ptd;
		else
			ErrorEx("cannot override m_PartTypeData during runtime!");
	}
```

`UpdateConstructionParts` (`4_world/classes/constructionbase.c:517-547`) reads `entityType.GetConstructionDataHolderBasic()`, spawns each part from its custom instance typename or from `GetDefaultPartInstanceTypename()` (`ConstructionPart` for `Construction`, `construction.c:34-37`), then calls `part.SetPartTypeData(typeData)` (`:531-538`). The map is keyed by `typeData.GetPartName()` (`:543`). Config field readers (`name`, `id`, `is_base`, `is_gate`, `required_parts`, `build_action_type`, …) live on `ConstructionPartTypeData.InitPartTypeData` (`typeconstructiondata/constructionparttyped.c:45-71`, basic strings `:75-86`, action masks `:95-107`).

The old public fields are still declared but **never written** — reading them gives `""` / `0` / `false` / null:

```c
// [EXACT] 4_world/classes/basebuilding/constructionpart.c:496-508
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	string 	m_Name;
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	string 	m_PartName;
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	string 	m_MainPartName;
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	int		m_Id;
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	bool 	m_IsBase;
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	bool 	m_IsGate;
	ref array<string> m_RequiredParts; //!obsolete, stored on 'ConstructionPartTypeData'
```

Use `GetName()`, `GetPartName()`, `GetMainPartName()`, `GetId()`, `IsBase()`, `IsGate()`, `GetRequiredParts()` (`constructionpart.c:193-286`), which forward to `m_PartTypeData`.

`SetRequestBuiltState` is `[Obsolete("1.30: Use 'SetBuiltState' directly")]` (`constructionpart.c:510-518`). Its body is the same as on 1.29 (sets the flag only in multiplayer), and neither 1.29 nor 1.30 vanilla calls it.

## 3. `*ServerEx` and collision

**No vanilla 1.30 script calls `BuildPartServer`, `DismantlePartServer` or `IsColliding(string)`** (grep of
`P:\scripts`: every caller uses `BuildPartServerEx` / `DismantlePartServerEx` / `IsCollidingEx` —
`actionbuildpart.c:140,167,170,476`, `actiondismantlepart.c:107`, `actionbuildshelter.c:194,240`,
`actionpartinfo.c:192`, `miscgameplayfunctions.c:1012`, the rebuildable-house debug spawns). A 1.29 mod that
overrides one of the old three still compiles (obsolete warning) and is then **silently never called**.

```c
// [EXACT] 4_world/classes/constructionbase.c:1751-1765
	[Obsolete("call BuildPartServerEx instead")]
	void BuildPartServer(notnull Man player, string part_name, int action_id)
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] Construction BuildPartServer | " + part_name);
		
		BuildPartServerEx(player,part_name,action_id);
	}
	
	[Obsolete("call DismantlePartServerEx instead")]
	void DismantlePartServer(notnull Man player, string part_name, int action_id)
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] Construction DismantlePartServer | " + part_name);
		
		DismantlePartServerEx(player,part_name,action_id);
	}
```

`IsColliding(string)` is `[Obsolete("call IsCollidingEx instead")]` right below (`constructionbase.c:1767-1803`).

```c
// [EXACT] 4_world/classes/constructionbase.c:180-185
	void BuildPartServerEx(Man player, string part_name, int action_id)
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] Construction BuildPartServerEx | " + part_name);
		//on action
		TakeMaterialsServer(part_name);
	}
```

Note: `player` **can be null** (comment at `:178`). `Construction` then resets the damage zone and calls
`m_Parent.OnPartBuiltServer` (`construction.c:52-74`) — that hook is `notnull Man player`
(`basebuildingbase.c:549`), so only `Rebuilding` (which uses `ProcessOnPartBuiltServer`, `rebuilding.c:32-37`)
tolerates a null player in practice. `DismantlePartServerEx` (`constructionbase.c:191-207`) refunds materials only
when `player` is set, then drops non-usable materials, drops the part's `essential_attachments[]` and makes items
on the part fall unless `disableItemFalling` is set. Vanilla `ActionBuildPart` calls Ex:

```c
// [EXACT] 4_world/classes/useractionscomponent/actions/continuous/actionbuildpart.c:167-170
		if (!construction.IsCollidingEx(checkData) && canBuild)
		{
			//build
			construction.BuildPartServerEx(action_data.m_Player, partName, AT_BUILD_PART);
```

`IsCollidingEx` takes a `CollisionCheckData`, not a string:

```c
// [EXACT] 4_world/classes/constructionbase.c:1527-1528
	bool IsCollidingEx(CollisionCheckData check_data)
	{
```

Before 1.30: `IsCollidingEx(CollisionCheckData)` and `CollisionCheckData` **already existed** (1.29:
`construction.c:1070,1322`) and 1.29's `ActionBuildPart` already used them; only `ActionBuildShelter` and
`MiscGameplayFunctions` still called `IsColliding(string)` (1.29: `actionbuildshelter.c:238`,
`miscgameplayfunctions.c:908,944`).

```c
// [EXACT] 4_world/classes/constructionbase.c:1965-1979
class CollisionCheckData
{
	ref array<Object> m_AdditionalExcludes;
	string m_PartName;
	int m_PrimaryGeometry;
	int m_SecondaryGeometry;
	
	void CollisionCheckData()
	{
		m_AdditionalExcludes = new array<Object>();
		m_PartName = "";
		m_PrimaryGeometry = ObjIntersectGeom;
		m_SecondaryGeometry = ObjIntersectView;
	}
}
```

Collision-trigger helpers still exist, marked obsolete with **no replacement**:

```c
// [EXACT] 4_world/classes/constructionbase.c:1805-1806
	[Obsolete("no replacement")]
	void CreateCollisionTrigger(string part_name, vector min_max[2], vector center)
```

```c
// [EXACT] 4_world/classes/constructionbase.c:1833-1844
	[Obsolete("no replacement")]
	void DestroyCollisionTrigger()
	{
		if (!m_ConstructionBoxTrigger)
			return;
		
		g_Game.ObjectDelete(m_ConstructionBoxTrigger);
		m_ConstructionBoxTrigger = NULL;
	}
	
	[Obsolete("no replacement")]
	bool IsTriggerColliding()
```

`m_ConstructionBoxTrigger` is tagged `//! Deprecated` (`constructionbase.c:1852`). 1.29 never created the
trigger either (the call was commented out, 1.29: `construction.c:1046`).

### 3.1 Destroy path

Part ruin no longer runs in `BaseBuildingBase.EEHealthLevelChanged` (1.30: only `super`,
`basebuildingbase.c:497-503`). `EntityAI.EEHealthLevelChanged` forwards to the component on the server
(`3_game/entities/entityai.c:1061-1062,1094-1099`), and `ConstructionBase.OnHealthLevelChanged` destroys the part
whose name equals the lowercased zone (`constructionbase.c:353-368`). `Construction.DestroyPartServer` then
notifies the parent and destroys dependent parts itself:

```c
// [EXACT] 4_world/classes/basebuilding/construction.c:93-106
	override void DestroyPartServer( Man player, string part_name, int action_id, bool destroyed_by_connected_part = false )
	{
		super.DestroyPartServer(player,part_name,action_id,destroyed_by_connected_part);
		
		//call events, 'special' BBB handling, TODO: some parameter into BBB to allow the new handling?
		if (m_Parent)
		{
			m_Parent.OnPartDestroyedServer(player, part_name, action_id);
			DestroyConnectedParts(part_name);
		}
		else
		{
			ProcessOnPartDestroyedServer(player, part_name, action_id, destroyed_by_connected_part);
		}
```

`OnPartDestroyedServer` gets no 4th argument, so on 1.30 `destroyed_by_connected_part` is always `false` inside
`BaseBuildingBase` subclasses. Before 1.30 it was forwarded (1.29: `construction.c:131`) and
`DestroyConnectedParts` was called from `BaseBuildingBase.EEHealthLevelChanged` (1.29: `basebuildingbase.c:517`).
Consequence (static reading, not measured): vanilla `Fence.OnPartDestroyedServer`'s
`IsGate() && destroyed_by_connected_part` branch (`fence.c:378-390`, drops barbed wire / camonet / locks and
closes the gate) cannot be reached through vanilla code; the locks are still dropped by the gate's
`essential_attachments[]` (`P:\DZ\gear\camping\config.cpp:4913-4917`; `DropEssentialAttachments`,
`constructionbase.c:1376`).

## 4. Rebuilding persistence (map buildings only)

`BuildingBase` registers ten netsync ints on the component (`4_world/entities/game/super/building.c:29-45`). `Rebuilding` writes its own versioned blob through `EntityAI.OnStoreSave` → `HandleStoreSave` (`3_game/entities/entityai.c:2912-2914`), read back only when the storage version is ≥ 143 (`:2980-2984`) — this is **not** `BaseBuildingBase.OnStoreSave`. `ConstructionBasic.HandleStorageEvents()` returns false (`3_game/systems/construction_basic.c:29-32`) and only `Rebuilding` overrides it to true (`rebuilding.c:468-471`), so a `Construction` component writes nothing there.

```c
// [EXACT] 4_world/classes/rebuilding/rebuilding.c:1-16
class Rebuilding : ConstructionBase
{
	static int REBUILDING_STORAGE_VERSION = 1;
	
	protected int m_LastStorageVersion;
	
	protected int m_SyncParts1;
	protected int m_SyncParts2;
	protected int m_SyncParts3;
	protected int m_SyncParts4;
	protected int m_SyncParts5;
	protected int m_SyncParts6;
	protected int m_SyncParts7;
	protected int m_SyncParts8;
	protected int m_SyncParts9;
	protected int m_SyncParts10;
```

```c
// [EXACT] 4_world/classes/rebuilding/rebuilding.c:501-514
	protected void SerializeConstructionData(ParamsWriteContext ctx)
	{
		ctx.Write(REBUILDING_STORAGE_VERSION);
		ctx.Write(m_SyncParts1);
		ctx.Write(m_SyncParts2);
		ctx.Write(m_SyncParts3);
		ctx.Write(m_SyncParts4);
		ctx.Write(m_SyncParts5);
		ctx.Write(m_SyncParts6);
		ctx.Write(m_SyncParts7);
		ctx.Write(m_SyncParts8);
		ctx.Write(m_SyncParts9);
		ctx.Write(m_SyncParts10);
	}
```

`DeserializeConstructionData` (`rebuilding.c:516+`) rejects a stored version lower than
`REBUILDING_STORAGE_VERSION` and logs "presumed corrupted data" (`:483-499`).

Packing math [DESIGN]:
- `BIT_INT_SIZE = 32` (`3_game/tools/bitarray.c:4`)
- `GetSyncDataSize()` returns `BITWISE_SYNCINFO_SIZE_BASE = 2` (`4_world/classes/basebuilding/constructionpart.c:6,65-68`)
- `adjustedID = (partId - 1) * GetSyncDataSize()`; `syncIntIdx = adjustedID / BIT_INT_SIZE` (`rebuilding.c:146-148`)
- 10 ints × 32 bits / 2 bits/part = 160 part ids (1..160)

Default `CalculateBitMaskOnBuild` sets only bit 0 at `offset` (`constructionpart.c:111-117`). The reversible
rebuild parts (`ConstructionPartRebuildReversibleBase`, `constructionpart.c:1041`, used by the reversible door
and barricade parts `:1093,1125`) also OR their facing into bit 1 (`:1057-1063`); the plain
`ConstructionPartRebuild` does not. Digest J's 00=ruin / 01=built / 10=destroyed mapping is not in those
methods — there is no three-state ruin encoding.

`BaseBuildingBase.OnStoreSave` is unchanged from 1.29 (still 1 bit per part, ids 1..93):

```c
// [EXACT] 4_world/entities/itembase/basebuildingbase.c:420-430
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
		
		//sync parts 01
		ctx.Write( m_SyncParts01 );
		ctx.Write( m_SyncParts02 );
		ctx.Write( m_SyncParts03 );
		
		ctx.Write( m_HasBase );
	}
```

Support graph: `ConstructionPartStaticsSupportData` parses `StaticsSupportData { SupportProviders / SupportRequirements }` (`4_world/classes/rebuilding/constructionstatics.c:6-78`); since 1.30.164200 a side that the config does not declare is stored as `-1` (`:63-76`) and the getters return `bool` + `out int`, `false` for "not configured" (`:80-96`; §11). `ConstructionBase.SupportRemovalCheck` defaults to true (`constructionbase.c:826-830`); `Rebuilding` overrides it (`rebuilding.c:654-666`: removal fails when a category the part provides would drop below the summed requirement). `CanBuildPartEx` also requires `PartHasAnySupportFulfilled` (`constructionbase.c:599-600,739`; always true on `ConstructionBase`, overridden by `Rebuilding` at `rebuilding.c:624-652`). [DESIGN] Do not treat `required_parts[]` as the only structural gate on rebuildable houses.

## 5. Masonry items and `TOOL_BRICKLAYING`

```c
// [EXACT] 4_world/classes/basebuilding/constructionconstants.c:13-26
//has to mirror tool defines in configs!
enum EConstructionTools
{
	TOOL_NO_ACTION = 0,
	TOOL_MOUNT_WIRE = 1,
	TOOL_NAIL = 2,
	TOOL_DIG_BASE = 4,
	TOOL_MOUNT_LOG = 8,
	TOOL_HEAVY = 16,
	TOOL_EXCAVATION = 32,
	TOOL_WOODWORK = 64,
	TOOL_HANDS = 128,
	TOOL_BRICKLAYING = 256
}
```

```c
// [EXACT] 4_world/entities/itembase/bricktrowel.c:1-8
class BrickTrowel extends ToolBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionBuildPart);
		AddAction(ActionRepairPart);
```

```c
// [EXACT] 4_world/classes/useractionscomponent/actions/continuous/actionpickupbricks.c:16-18
class ActionPickUpBricks: ActionContinuousBase
{	
	static const int YIELD = 3;
```

```c
// [EXACT] 4_world/entities/itembase/mortarmix.c:1-8
class MortarMix extends ItemBase
{	
	override void SetActions()
	{
		super.SetActions();
        AddAction(ActionMixMortarAtWater);
		AddAction(ActionMixMortarAtWell);
	}
```

`ActionMixMortarAtWater` uses `CCTLiquid` (`4_world/classes/useractionscomponent/actions/continuous/actionmixmortaratwater.c:23,50`). The mix is crafted by `CraftMortarMix` (`4_world/classes/recipes/recipes/craftmortarmix.c:1`, registered `pluginrecipesmanagerbase.c:168`). `MortarMix_Opened` only adds `ActionAttachToConstruction` (`4_world/entities/itembase/mortarmix_opened.c:1-9`). `CommonBrick` adds `ActionAttachToConstruction` and `ActionAttachOnSelection` (`commonbrick.c:1-10`). Bricks are picked up from a `PileOfBricks` three at a time (`actionpickupbricks.c:18,59`). Rebuildable house scripts live under `4_world/entities/building/rebuildable/` (e.g. `house_1m1.c:1`). The build animation for the trowel is `CMD_ACTIONFB_BRICKTROWEL` (`actionbuildpart.c:260-261`).

`ConstructionMaterialType` moved to `constructionbase.c:2-12` and adds brick/rubble (before 1.30 it was declared in 1.29: `construction.c:1-9`):

```c
// [EXACT] 4_world/classes/constructionbase.c:2-12
enum ConstructionMaterialType
{
	MATERIAL_NONE	= 0,
	MATERIAL_LOG	= 1,
	MATERIAL_WOOD	= 2,
	MATERIAL_STAIRS	= 3,
	MATERIAL_METAL	= 4,
	MATERIAL_WIRE	= 5,
	MATERIAL_BRICK 	= 6,
	MATERIAL_RUBBLE = 7
}
```

Configs (in the 1.30.164200 `P:\DZ`; the 164078 extract here lacked them): `class BrickTrowel: Inventory_Base` with `build_action_type=256` = `TOOL_BRICKLAYING` (`P:\DZ\gear_takistan\tools\config.cpp:20,29`); `PileOfBricks` (`varQuantityMax=252`), `CommonBrick`, `MortarMix`, `MortarMix_Opened` (`P:\DZ\gear_takistan\consumables\config.cpp:18,30,101,286,349`); `DigitalCodeLock` and `DigitalCodeLock_Bunker` (`P:\DZ\gear_takistan\camping\config.cpp:19,315`). The trowel sounds are in the character sound config (`P:\DZ\characters\data\config.cpp:7296,8694`).

## 6. Fence `DigitalCodeLock` and combo-lock unlock

```c
// [EXACT] 4_world/entities/itembase/basebuildingbase/fence.c:18-26
	typename ATTACHMENT_WOODEN_LOG			= WoodenLog;
	typename ATTACHMENT_COMBINATION_LOCK	= CombinationLock;
	typename ATTACHMENT_CODE_LOCK			= DigitalCodeLock;
	
	const string ATTACHMENT_SLOT_CAMONET			= "Wall_Camonet";
	const string ATTACHMENT_SLOT_BARBEDWIRE_DOWN	= "Wall_Barbedwire_1";
	const string ATTACHMENT_SLOT_BARBEDWIRE_UP		= "Wall_Barbedwire_2";
	const string ATTACHMENT_SLOT_COMBINATION_LOCK 	= "Att_CombinationLock";
	const string ATTACHMENT_SLOT_CODE_LOCK 			= "Att_CodeLock";
```

```c
// [EXACT] 4_world/entities/itembase/basebuildingbase/fence.c:161-165
	DigitalCodeLock GetCodeLock()
	{
		DigitalCodeLock codeLock = DigitalCodeLock.Cast(FindAttachmentBySlotName(ATTACHMENT_SLOT_CODE_LOCK));
		return codeLock;
	}
```

The Fence accepts one lock type at a time: a combination lock only if no code lock is attached and vice versa,
both only on a fully built, closed gate (`fence.c:438-445`; inventory display `:200-226`). `IsLocked()` is true for
either lock (`fence.c:127-141`). The slot-name members were renamed: `ATTSLOT_CAMONET` / `ATTSLOT_BARBEDWIRE_DOWN`
/ `ATTSLOT_BARBEDWIRE_UP` (1.29: `fence.c:22-24`) are now the `const string ATTACHMENT_SLOT_*` above — a 1.29 mod
that references the old names no longer compiles. Config: `Att_CodeLock` is in Fence `attachments[]` and the
"Attachments" GUI category (`P:\DZ\gear\camping\config.cpp:4362-4374,4388-4402`); the slot class is
`Slot_Att_CodeLock` (`P:\scripts\config.cpp:2768-2774`).

`DigitalCodeLock` owns a `CodeLockComponent` (`4_world/entities/itembase/codelock.c:1,9,30`). PIN length 4..6 (`4_world/classes/codelockcomponent.c:23-24`). Persist order on the component: `m_LockPIN`, `m_IsLocked`, `m_DoorIndex` (`codelockcomponent.c:257-262`). Wrong-code brute-force protection reads `cfggameplay.json` `ExternalLockData` (`3_game/cfggameplaydatajson.c:421-438`: `protectionCountStageOne=3`, `protectionCountStageTwo=1`, `protectionTime=30.0`, `protectionResetTime=3600.0`; used at `codelockcomponent.c:66-67,474-479`). Building-door lock slots (`Att_CombinationLock` / `Att_CodeLock` on `BuildingBase`, `building.c:10-11`) are owned by `dayz-doors`; Fence is the base-building attachment point. The lock's look on its parent comes from `CodeLockVisualManager` (`4_world/classes/codelockvisualmanager.c`): the back part sits at phase `0.92` on a `BaseBuildingBase` such as the Fence and at `0.50` on a `House` (`:46-48,452-465`); since 1.30.164200 these values are `private const` and the `House` value no longer depends on the reversed state (164078: 0.60, or 0.50 reversed). Override `GetAnimationPhaseForFrontPart` / `GetAnimationPhaseForBackPart` (`:438,452`, `protected`) for a custom parent. Item config: `P:\DZ\gear_takistan\camping\config.cpp:19` (slots `:96-119`, animation sources `:286-297`).

### 6.1 `CombinationLock` inside/outside API

A combination lock now has an outside and an inside dial (`FACING_OUTSIDE = 0`, `FACING_INSIDE = 1`,
`combinationlock.c:29-30`; second combination `m_CombinationInside` `:34`).

| 1.29 | 1.30 (`4_world/entities/itembase/combinationlock.c`) |
|---|---|
| `SetCombination(int)` | `SetCombination(int combination, bool setOtherSide = true)` `:233`; `SetCombinationInside(...)` `:242` |
| `DialNextNumber()` | `DialNextNumber(int lockSide = FACING_OUTSIDE)` `:273` |
| `SetNextDial()` | `SetNextDial(int lockSide = FACING_OUTSIDE)` `:340` |
| `CheckLockedStateServer()` | `CheckLockedStateServer(bool insideLock = false)` `:511` |
| `UnlockServer(EntityAI player, EntityAI parent)` | removed → `UnlockOnServer(EntityAI player, EntityAI parent, bool isInsideLock = false)` `:409` |
| `IsLockedOnGate()` | `[Obsolete]` → `IsLocked()` `:506,846-858` |
| — | `Unlock()`, `SetDigitRotationAnimationPhase`, `AnimateDigits`: new and already `[Obsolete]`, empty declarations `:861-866` |
| public `DestroyLock`, `ShuffleLock`, `UpdateVisuals`, `UpdateSound`, `SoundLock*` | `protected` (`:471,546,552,618,822-844`) |
| `LockAction { NONE, DIAL_NUMBER_CHANED, DIAL_INDEX_CHANGED, LOCKED, UNLOCKED }` | `{ NONE, DIAL_NUMBER_CHANED_INSIDE, DIAL_NUMBER_CHANED_OUTSIDE, DIAL_INDEX_CHANGED_INSIDE, DIAL_INDEX_CHANGED_OUTSIDE, LOCKED, UNLOCKED, COUNT }` `:12-23` (numeric values shifted) |

Behaviour: dialling no longer unlocks. 1.29's `ActionDialCombinationLockOnTarget` called `UnlockServer` as soon
as the dialled number matched (1.29: `actiondialcombinationlockontarget.c:66-79`); 1.30 only dials
(`actiondialcombinationlockontarget.c:72-82`) and the player must run the new single-use
`ActionCombinationLockUnlock` ("#detach_combination_lock"), which checks the side the player faces and then calls
`UnlockOnServer` (`4_world/classes/useractionscomponent/actions/singleuse/actioncombinationlockunlock.c:57-82`;
added to the lock at `combinationlock.c:686`). A lock attaches to any parent whose `CanUseConstruction()` is true
(`IsLockAttached` `combinationlock.c:534-543`), not only to a Fence.

Stream v143 adds `m_CombinationInside`:

```c
// [EXACT] 4_world/entities/itembase/combinationlock.c:128-136
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave(ctx);
		
		//write data
		ctx.Write( m_Combination );
		ctx.Write( m_CombinationLocked );
		ctx.Write( m_CombinationInside );
	}
```

The load reads `m_CombinationInside` only when `version >= 143` (`combinationlock.c:169-177`); storages written
before that keep the two-int layout. Fence gate save itself is still after `super` (`fence.c:246-254`; 1.29:
`fence.c:212-220`).

## 7. `ConstructionActionData` slot helpers

Not removed. Marked obsolete — and vanilla no longer calls `SetSlotId` anywhere, so `GetSlotId()` returns
whatever was last stored (1.29's `ActionAttachToConstruction` set it, 1.29: `actionattachtoconstruction.c:36,55`).
The attach action now carries the slot in its own action data (`AttachToConstructionActionData.m_SlotID`,
`4_world/classes/useractionscomponent/actions/singleuse/actionattachtoconstruction.c:1-4,38,68`).

```c
// [EXACT] 4_world/classes/basebuilding/constructionactiondata.c:617-627
	[Obsolete("1.30: Unsafe, overridden ActionData used in-system instead")]
	void SetSlotId( int slot_id )
	{
		m_SlotId = slot_id;
	}
	
	[Obsolete("1.30: Unsafe, overridden ActionData used in-system instead")]
	int GetSlotId()
	{
		return m_SlotId;
	}
```

## 8. `disableSimulation` (changelog only for houses)

```c
// [EXACT] 3_game/entities/entity.c:3-6
	proto native void DisableSimulation(bool disable);

	//! Returns whether simulation is disabled
	proto native bool GetIsSimulationDisabled();
```

`[CHANGELOG 1.30]` `disableSimulation` config parameter on house-based entities (from the Exp modding changelog
digest, `work/changelog-1.30-exp-modding.md:13`). **Not confirmed on stable:** no `P:\DZ\**\config.cpp` and no
`P:\scripts` file contains `disableSimulation`, and the two natives above already existed with the same lines in
1.29 (1.29: `3_game/entities/entity.c:3-6`). Not a `BaseBuildingBase` field.

## 9. Activation, new per-part keys and rebuildable buildings

First written 2026-09-24 from the Exp `scripts.pbo`; re-verified 2026-10-15 on 1.30.164078; §9.2 utility/support
rows and §9.4 updated 2026-10-21 on 1.30.164200. Nothing in this section has been exercised in-game.

### 9.1 What turns construction on

```c
// [EXACT][CLAIM-BB-CONSTRUCTION-ACTIVATION-130] 3_game/entities/entityai.c:248-249
		if (g_Game.ConfigIsExisting("cfgVehicles" + " " + GetType() + " " + "Construction"))
			ConstructionInit();
```

Any `CfgVehicles` class with a `Construction` subclass gets its component when the entity is constructed; the
type data is cached by `EntityAIType.InitConstructionDataTypeHolder` on the same condition
(`3_game/entities/entityaitype.c:66-74`). The concrete type comes from `CreateConstructionComponent()`:
`new Rebuilding(this)` on `BuildingBase` (`4_world/entities/game/super/building.c:23-26`) and
`new Construction(this)` on `BaseBuildingBase` (`basebuildingbase.c:872-876`). Inference from those two
overrides, not measured: a modded building that inherits a `BuildingBase` descendant needs no script override to
become rebuildable, only the config block and the model selections. Other `EntityAI` classes get no component
(the base `CreateConstructionComponent` returns null) and the config block is then only cached, not used.

### 9.2 Per-part keys that 1.30 reads

[EXACT][CLAIM-BB-REBUILD-PART-KEYS-130] Readers are in
`4_world/classes/basebuilding/typeconstructiondata/constructionparttyped.c` unless another file is named. The
paths are relative to `Construction <main_part> <part>`. Keys marked † are new in 1.30 (no reader in 1.29).

| Key | Reader | What the code shows |
|---|---|---|
| `name`, `id`, `is_base`, `is_gate` | `:75-86` | as on 1.29 |
| `required_parts[]`, `conflicted_parts[]` | `:88-93`, `:139-145` | as on 1.29 |
| `build_action_type`, `dismantle_action_type` | `:95-107` | `EConstructionTools` masks; `TOOL_HANDS` lets `ActionBuildPartHandsGeneric` build with empty hands (`constructionbase.c:1442-1479`) |
| `Materials` > `<material>` > `type`, `slot_name`, `quantity`, `lockable`, † `skipOnRepair`, † `skipOnDismantle` | `4_world/classes/basebuilding/constructionmaterials.c:26-32` | `skipOnRepair`: not consumed on repair (`constructionbase.c:1190-1191`, still required by `HasMaterialsEx`); `skipOnDismantle`: skipped when a base part's materials are dropped (`:1278-1279`) |
| `material_type`, `collision_data[]` | `:129-137`, `:147-157` | as on 1.29 |
| † `custom_part_type` | `constructionparttypedholder.c:34-45` | names the part **instance** class; the type-data class spawned is `<value>TypeData` (e.g. `ConstructionPartRebuild` → `ConstructionPartRebuildTypeData`). If `<value>TypeData` does not exist the holder silently falls back to `ConstructionPartTypeData` and the default instance class (`:42-45`; `constructionbase.c:532-537`). Vanilla values: `ConstructionPartRebuildRubble`, `…ReversibleDoor`, `…ReversibleBarricade`, and since 1.30.164200 `ConstructionPartRebuildInvertedSupport` (§11) |
| † `can_decay_general` (on the `Construction` class itself) | `constructionparttypedholder.c:53-55` | applied after the per-part read, so it overrides `can_part_decay` for every part |
| † `can_part_decay`, `part_decay_threshold`, `part_decay_rate` | `:199-218` | per-part decay, defaults threshold 0.3 / rate −1 HP/s (`constructionbase.c:36-37`), ×1.5 during a sandstorm (`:39,334-340`); processed from `EntityAI.OnCEIterate` (`entityai.c:3904-3911`). Global switch `GeneralData.disableBaseDecay` (`3_game/cfggameplaydatajson.c:60`, `cfggameplayhandler.c:172-175`, `construction_basic.c:37-40`). No vanilla `P:\DZ` config sets these keys. |
| † `insider_minMaxs[]` | `:159-186` | inner box, next to `collision_data` |
| † `item_falling_data[]` | `:188-197` | box from which items fall when the part is dismantled/destroyed (`constructionbase.c:242-248` + `ItemFall`), unless `BaseBuildingData.ConstructionData.disableItemFalling` (`cfggameplaydatajson.c:279`). Vanilla now sets it on Fence platform parts and Watchtower roofs/stairs (config delta). |
| † `hand_build_FB_anim_override` | `:220-226` | FB animation for empty-hand building |
| † `essential_attachments[]` | `:228-237` | attachments dropped when the part is dismantled/destroyed (`DropEssentialAttachments`, `constructionbase.c:1376`); vanilla Fence `wall_gate` lists `Att_CodeLock`, `Att_CombinationLock` (`P:\DZ\gear\camping\config.cpp:4913-4917`; 1.29 listed only the combination lock) |
| † `related_GUI_categories[]` | `:239-248` | ties the part to `GUIInventoryAttachmentsProps` categories |
| † `damage_hands_on_build` | `:250-258` | empty-hand build can damage gloves or cause bleeding (`actionbuildpart.c:183-202`) |
| † `is_utility` | `:260-264` | helper ("utility") part with `id=-1`: never synced or saved (`rebuilding.c:379-390`), built/unbuilt locally on every machine by `ConstructionBase.UpdateUtilityParts` from its own `required_parts[]` / `conflicted_parts[]` / `SupportRequirements` (`constructionbase.c:614-666`); skipped by `HasDependentPart` (`constructionbase.c:817`; `rebuilding.c:741`) and by `GetValidDepenentPartsArray` (`rebuilding.c:772`) |
| † `toggle_utility_part_update` (int, since 1.30.164200) | `:266-274` | on a normal part: its build/dismantle queues a re-evaluation of all utility parts (`ConstructionPartRebuild.TryQueueUtilityPartsUpdate`, `constructionpart.c:885-891`). Vanilla: 46 Takistan parts, e.g. `P:\DZ\structures_takistan\Industrial\config.cpp:411,461`, `Residential\config.cpp:9059`. Replaces `utility_build[]`, which 164200 no longer reads (dead leftovers at `Industrial\config.cpp:402-405,462-465`); 1.30.164078 read `utility_build[]` as the list of utility parts to (un)build directly. §11 |
| † `RequiredPartsVariants` > `<child>` > `partVariants[]` | `:276-300` | alternative requirement sets |
| † `build_text_override`, † `dismantle_text_override` | `:315-336` | action text (`ShelterSite` already used `build_text_override` in 1.29 config) |
| † `physics_geom_names[]` + `physics_geom_behaviors_invert[]`, † `override_animsource_names[]` + `override_animsource_behaviors_invert[]`, † `proxyroadway_names[]` + `proxyroadway_behaviors_invert[]` | `:339-405` | per-part overrides of which geometry / animation source / roadway is toggled; counts must match or the pair is cleared (`ErrorEx` for geoms/roadways). Used by the `ConstructionPartRebuild*` handlers (`constructionpart.c:544+`); a plain `ConstructionPart` still toggles `SetAnimationPhase(part_name)` / `AddProxyPhysics(part_name)` (`:418-436`) |
| † `EffectsData` > `soundEffectPositions[]`, `particleEffectPositions[]`, `build_soundset_override`, `dismantle_soundset_override`, `destroy_soundset_override`, `destroy_particle_override` | `4_world/classes/rebuilding/constructioneffects.c:19-58` | memory points and overrides for build effects |
| † `StaticsSupportData` > `SupportProviders` / `SupportRequirements` > `<child>` > `supportCategory` (string, hashed), `supportStrength` (int) | `4_world/classes/rebuilding/constructionstatics.c:13,26,37-38,45,56-57` | structural support: what a part provides and needs, by category. A side not declared for a category is stored as `-1` and its getter returns `false` (since 1.30.164200, `:63-96`; §11). `Rebuilding` sums provided/required strength of built parts per category; a part with requirements needs **any one** category met (`rebuilding.c:624-652`) |

No longer read on 1.30: `show_on_init` (1.29: `construction.c:259`) and `platform_support` (1.29:
`construction.c:792`) — grep of `P:\scripts` finds no reader.

### 9.3 Locks on rebuilt doors

`BuildingBase` declares the slots `Att_CombinationLock` and `Att_CodeLock` (`building.c:10-11`), with per-door
slot classes `Att_CodeLock1_Wood` … `Att_CodeLock10_Metal` and `Att_CombinationLock1_Wood` … (`P:\scripts\config.cpp:2316-2462,2768-2914`).
A door cannot hold both lock types (`HasConflictingDoorAttachment`, `building.c:224-228,258-261,344`), and
`CanDoorBeOpened(notnull DoorManipulationParams)` is overridden (`:307`). Door details: `dayz-doors`.

### 9.4 What 1.30 ships for rebuildable buildings

- **Script classes.** The Nasdara houses (`4_world/entities/building/rebuildable/house_*.c`),
  `Land_IrrigationTunnel_Entrance_01 : BuildingSuper` (`irrigationtunnel_entrance.c:1-11`, whose debug spawn puts a
  `WoodenStick` and a `Rope` in its inventory) and `Land_WaterWell_Brick : Well`, whose water is clean only when the
  `roof` part is built (`4_world/entities/building/land_waterwell_brick.c:1-20`).
- **Configs and models (unpacked on 1.30.164200).** `P:\DZ\structures_takistan\` holds them: houses in
  `Residential\config.cpp` (e.g. `Land_House_1M1` `:18`, `Land_House_2M1` `:35149`, `Land_House_3S1_Dam3` `:91187`),
  `Land_IrrigationTunnel_Entrance_01` in `Industrial\config.cpp:20` (its `Construction` block `:383-529` is the
  smallest complete rebuild example: two ladder parts + one utility part), `Land_WaterWell_Brick` in
  `Props\config.cpp:18`. Models sit next to them (e.g. `Industrial\construction\*.p3d`). The 1.30.164078 extract on
  this machine lacked these folders, which is why earlier versions of this file marked the part lists `[UNVERIFIED]`.
  Real part-class pattern: `Land_House_1M2_Dam2 > Construction > rebuild_wall1 > wall1_wood` (`Residential:7993-8075`:
  `physics_geom_names[]`, `required_parts[]`, `conflicted_parts[]`, `collision_data[]`, `item_falling_data[]`,
  `toggle_utility_part_update=1;`, `insider_minMaxs[]`, `Materials`, `StaticsSupportData`, `EffectsData`).
- **Example part names.** `Land_House_2M1_Dam2.OnDebugSpawn` builds `rubble_rubble1`, `rubble_rubble2`,
  `wall1_wood`, `wall2_wood`, `wall3_brick` and `roof1` with `BuildPartServerEx(null, <part>, -1)`
  (`house_2m1.c:11-21`); `Land_WaterWell_Brick` builds `rubble_rubble1`, `well_brick`, `roof`
  (`land_waterwell_brick.c:20-27`). The full lists are in the configs above.
- **1.30.164200 config changes on these buildings** (from `dayz-130-migration` →
  `references/config-delta-1.30.164078-to-164200.md` §2.2): `toggle_utility_part_update=1;` added to 46 parts and
  43 `utility_build[]` arrays removed; `Land_House_3S1_Dam3` floor logic rebuilt on `SupportProviders` +
  `RequiredPartsVariants` with new utility parts `util_dyn_roadway_floor{2,3}_{ext,int}` (the `_ext` ones inverted,
  `Residential:103151,103238`); `Land_IrrigationTunnel_Entrance_01` gained a `Navmesh { HideParts }` block
  (`Industrial:25`) and the roadway `proxyroadway_block_01` toggled by `ladder_utility` (`Industrial:63,511-515`);
  several `physics_geom_names[]` fixes.
- **`RebuildingSpawner` is diag-only** (`4_world/classes/basebuilding/rebuildingspawner.c:1`, `#ifdef DIAG_DEVELOPER`),
  as are the instant build/dismantle/destroy debug actions (`actions/instant/actionbuildpartinstant.c:2`).

### 9.5 Object Spawner flag

- **The flag.** `ECE_OBJECT_SPAWNER = 256` (`3_game/ce/centraleconomy.c:15`) is set by the Object Spawner on the
  classname path (`3_game/objectspawner.c:48`). The `.p3d` path (`:44`) calls `CreateStaticObjectUsingP3D(…, true)`
  — `createLocal = true`, `objSpawner` left at its default `false`.
- **New parameter.** `CreateObject` and `CreateStaticObjectUsingP3D` gained `bool objSpawner = false`
  (`3_game/global/game.c:680,691`).
- **Unknowns.** Script does not say what the engine does with the flag. Whether a rebuildable building placed by
  the Object Spawner keeps its parts across a restart is unmeasured.

## 10. Porting a ≤1.29 base-building mod — hooks that compile but no longer run

Static reading of `P:\scripts` (written on 1.30.164078, refs renumbered to 164200) against 1.29; none measured
in-game. For a mod already ported to 164078, see §11.5.

| 1.29 hook / call | On 1.30 | Use instead |
|---|---|---|
| `override BuildPartServer(...)` | obsolete forwarder, no vanilla caller (`constructionbase.c:1751-1757`) | `BuildPartServerEx` or `BaseBuildingBase.OnPartBuiltServer` |
| `override DismantlePartServer(...)` | same (`:1759-1765`) | `DismantlePartServerEx` or `OnPartDismantledServer` |
| `override IsColliding(string)` | same (`:1767-1803`) | `IsCollidingEx(CollisionCheckData)` |
| `override ActionBuildPart.SetBuildingAnimation(ItemBase)` | obsolete, not called (`actionbuildpart.c:227,370-374`) | `SetBuildingAnimationEx(player, target, item, action_data, extra_data)` |
| `part.m_PartName` / `m_Name` / `m_Id` / `m_IsBase` / `m_IsGate` / `m_MainPartName` / `m_RequiredParts` | declared, never written (`constructionpart.c:496-508`) | getters |
| `new ConstructionPart(name, part_name, …)` | empty constructor, no type data | let `UpdateConstructionParts` create parts; custom class via `custom_part_type` or `GetDefaultPartInstanceTypename()` |
| `CanBuildPart(part, tool, false)` to skip the tool check | `use_tool` ignored (`constructionbase.c:584-612`) | `CanBuildPartEx` and read the failure mask, or give the part `TOOL_HANDS` |
| `ConstructionActionData.GetSlotId()` | never set (`constructionactiondata.c:617-627`) | `AttachToConstructionActionData.m_SlotID` |
| `ActionBuildShelter`, `ActionActionBuildPartNoTool`, `ConstructionActionData.OnUpdateActionsNoTool` | obsolete / unregistered (`actionbuildshelter.c:1`; `actionbuildpart.c:543`; `constructionactiondata.c:205`) | `ActionBuildPartHandsGeneric` + `TOOL_HANDS` parts |
| `override ShelterSite.CanUseHandConstruction()` | vanilla override removed; value comes from `TOOL_HANDS` parts (`constructionbase.c:348-351,546`) | config masks |
| `OnPartDestroyedServer(..., destroyed_by_connected_part)` branch | flag always false (§3.1) | check `GetConstruction()` state / dependencies yourself |
| logic after `super` in `BaseBuildingBase.EEHealthLevelChanged` meant to run before vanilla destroys the part | vanilla destroys inside `super` (`entityai.c:1061-1062`) | `modded class Construction` → `OnHealthLevelChanged` / `DestroyPartServer` |
| `show_on_init`, `platform_support` config keys | not read | — |
| "last dial unlocks the lock" automation; `UnlockServer` | dial only dials; `UnlockServer` removed | `ActionCombinationLockUnlock` / `UnlockOnServer` |

Terrain holes, underground triggers and the underground placement block are in `dayz-underground`.

## 11. 1.30.164200 delta — support API, utility parts, house collisions

Written 2026-10-21 from `P:\scripts` / `P:\DZ` of 1.30.164200 (Scripts Rev 127662) and the 164078 → 164200 script
diff. Static reading; nothing here was exercised in-game. Short version for authors: `SKILL.md` →
"1.30.164200 changes". The scanner side: `dayz-130-migration` → `references/api-delta-1.30.164078-to-164200.md`.

### 11.1 Support getters: `bool` + `out int`, `-1` = not configured

```c
// [EXACT] 4_world/classes/rebuilding/constructionstatics.c:63-96
			//now insert as 'StaticsSupportCategoryValues'
			foreach (int category : m_ConfiguredCategories)
			{
				//default invalid values
				int provided = -1;
				if (providerMap.Contains(category))
					provided = providerMap.Get(category);
				
				int required = -1;
				if (requirementMap.Contains(category))
					required = requirementMap.Get(category);
				
				m_ValueMap.Insert(category, new StaticsSupportCategoryValues(category, provided, required));
			}
		}
	}
	
	bool GetSupportProvided(int category, out int providedStrength)
	{
		if (!m_ValueMap || !m_ValueMap.Contains(category))
			return false;
		
		providedStrength = m_ValueMap.Get(category).m_ProvidedSupport;
		return providedStrength > -1;
	}
	
	bool GetSupportRequirement(int category, out int requiredStrength)
	{
		if (!m_ValueMap || !m_ValueMap.Contains(category))
			return false;
		
		requiredStrength = m_ValueMap.Get(category).m_RequiredSupport;
		return requiredStrength > -1;
	}
```

The same signatures forward through `ConstructionPartTypeData.GetPartTypeSupportProvided/Requirement`
(`typeconstructiondata/constructionparttyped.c:633-641`) and `ConstructionPart.GetSupportProvided/Requirement`
(`basebuilding/constructionpart.c:371-379`). Before 164200 all six returned `int`, and a side missing from the
config read as `0`. A category is listed by `GetConfiguredSupportCategories()` when **either** side declares it,
so always test the `bool`.

[DESIGN] Override and call pattern on 164200 (a 164078 `override int GetSupportProvided(int category)` no longer
compiles):

```c
modded class ConstructionPartRebuild
{
	override bool GetSupportProvided(int category, out int providedStrength)
	{
		if (!super.GetSupportProvided(category, providedStrength))
			return false;	// not configured on this part: keep it "not configured", do not invent a 0

		// adjust providedStrength here
		return true;
	}
}

// caller
int required;
if (part.GetSupportRequirement(category, required))
{
	// configured; 'required' may legitimately be 0 (always met)
}
```

### 11.2 `Rebuilding.PartHasAnySupportFulfilled` and inverted parts

```c
// [EXACT] 4_world/classes/rebuilding/rebuilding.c:624-652
	override protected bool PartHasAnySupportFulfilled(string part_name)
	{
		if (!super.PartHasAnySupportFulfilled(part_name))
			return false;
		
		ConstructionPartRebuild rebPart = GetRebuildingPart(part_name);
		set<int> tmp = rebPart.GetConfiguredSupportCategories();
		if (tmp.Count() > 0) //can fail only if some REQUIREMENT support category is defined
		{
			bool validRequirementDefined;
			foreach (int category : tmp)
			{
				int requirement;
				if (rebPart.GetSupportRequirement(category, requirement))
				{
					validRequirementDefined = true;
					if (requirement <= m_SupportFulfillment.Get(category))
						return !rebPart.InvertSupportBehavior();
				}
			}
			
			if (validRequirementDefined)
				return rebPart.InvertSupportBehavior();
			
			return true;
		}
		
		return true;
	}
```

| Part declares | Normal part | Inverted part (`InvertSupportBehavior()` true) |
|---|---|---|
| no `StaticsSupportData`, or only `SupportProviders` | `true` | `true` |
| `SupportRequirements`, at least one category met | `true` | `false` |
| `SupportRequirements`, none met | `false` | `true` |

`m_SupportFulfillment` is the per-category sum of `supportStrength` provided by the **built** parts, kept by
`Add/RemoveSupportFulfillmentFromSourcePart` (`rebuilding.c:668-696`); `m_SupportRequirement` sums the
requirements of built non-utility parts (`:698-734`) and feeds `SupportRemovalCheck` (`:654-666`). Since 164200
both maps change only for categories the part actually configures.

```c
// [EXACT] 4_world/classes/basebuilding/typeconstructiondata/constructionpartrebuildinvertedsupport.c:1-13
//type
class ConstructionPartRebuildInvertedSupportTypeData : ConstructionPartRebuildTypeData
{
}

//instance
class ConstructionPartRebuildInvertedSupport : ConstructionPartRebuild
{
	override bool InvertSupportBehavior()
	{
		return true;
	}
}
```

Selected by `custom_part_type="ConstructionPartRebuildInvertedSupport";` (`constructionparttypedholder.c:34-45`
spawns `"<value>TypeData"`). Vanilla pair in `Land_House_3S1_Dam3`: `util_dyn_roadway_floor2_ext`
(`P:\DZ\structures_takistan\Residential\config.cpp:103121-103152`, inverted, `required_parts[]={"roof1"}`) and
`util_dyn_roadway_floor2_int` (`:103153-103183`, normal) share the requirements
`support_utility_floor_switcher2_category1` (3) / `…category2` (5), so once `roof1` is built exactly one of the
two roadway proxies is active, depending on whether the floor is supported.

### 11.3 Utility parts: post-update queue instead of an explicit list

```c
// [EXACT] 4_world/classes/basebuilding/constructionpart.c:884-891
	//! Queues utility part update, if needed
	override void TryQueueUtilityPartsUpdate(ConstructionBase con)
	{
		super.TryQueueUtilityPartsUpdate(con);
		
		if (m_PartTypeData.TriggersUtilityPartsUpdate())
			con.QueueUpdateUtilityParts();
	}
```

```c
// [EXACT] 4_world/classes/constructionbase.c:613-666
	//! Simplified condition for utility (non-)parts
	bool CanBuildUtilityPart(string partName)
	{
		if (!HasRequiredPart(partName))
			return false;
		if (!PartHasAnySupportFulfilled(partName))
			return false;
		if (HasConflictPart(partName))
			return false;
		
		return true;
	}
	
	void QueueUpdateUtilityParts()
	{
		if (!m_UpdateUtilityParts)
		{
			m_UpdateUtilityParts = true;
			g_Game.GetPostUpdateQueue(CALL_CATEGORY_SYSTEM).Insert(this.UpdateUtilityParts);
		}
	}
	
	//! returns 'true' if changed
	bool UpdateUtilityParts()
	{
		m_UpdateUtilityParts = false;
		bool changed = false;
		
		foreach (ConstructionPart part : m_ConstructionPartArray)
		{
			if (part.IsUtilityPart())
			{
				bool built = part.IsBuilt();
				string partName = part.GetPartName();
				if (CanBuildUtilityPart(partName))
				{
					if (!built)
					{
						AddToConstructedParts(partName);
						ConstructionManagerStatic.AddPartToProcess(this, part);
					}
				}
				else if (built)
				{
					RemoveFromConstructedParts(partName);
					ConstructionManagerStatic.AddPartToProcess(this, part);
				}
			}
		}
		
		g_Game.GetPostUpdateQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateUtilityParts);
		
		return changed;
	}
```

- The trigger is per part: `toggle_utility_part_update` (int) read by `InitUtilityBuildData`
  (`constructionparttyped.c:266-274`); the plain `ConstructionPart.TryQueueUtilityPartsUpdate` is an empty
  declaration (`constructionpart.c:491`), so a player `Construction` part never queues.
- It is called from `AddToConstructedParts` / `RemoveFromConstructedParts` (`constructionbase.c:148,166`), which
  `Rebuilding` reaches on the server when a part is built and on clients from `SetPartStateLocal` when synced parts
  arrive (`rebuilding.c:392-418`). Utility parts (`id=-1`) are skipped by the sync (`UpdatePartSyncStateLocal`,
  `:379-390`), so each machine derives them itself. The post-update queue runs once per frame
  (`DayZGame.OnPostUpdate`, `3_game/dayzgame.c:3097`).
- `UpdateUtilityParts` always returns `false` (`changed` is never assigned). Do not branch on it.
- Before 164200: `ConstructionPartRebuild.TryHandleUtilityParts(EntityAI)` immediately (un)built the utility parts
  named in the trigger part's `utility_build[]`, and `CanBuildUtilityPart` returned `false` for a part already
  built.

Vanilla minimal example, `Land_IrrigationTunnel_Entrance_01` (`P:\DZ\structures_takistan\Industrial\config.cpp:383-529`):
`ladder_rebuild` and `ladder_rebuild_inside` conflict with each other, each provides support `"ladder"` 1 and sets
`toggle_utility_part_update=1;` (`:411`, `:461`); `utility_nonparts > ladder_utility` (`is_utility=1; id=-1;`,
`:494-528`) requires `"ladder"` 1 and, through inverted `override_animsource_*`, `physics_geom_*` and
`proxyroadway_*` arrays, hides the destroyed ladder and removes the blocking roadway `proxyroadway_block_01`
while a ladder stands. The `utility_build[]` arrays still at `:402-405` and `:462-465` are not read.

[DESIGN] Do not put `toggle_utility_part_update=1;` on an `is_utility` part. `UpdateUtilityParts` resets the flag,
builds that part, the part re-queues (`Insert` appends the same method), and the closing `Remove` drops every copy
(`ScriptInvoker.Remove(func fn, int flags = EScriptInvokerRemoveFlags.ALL)`, `2_gamelib/tools.c:122`) while
`m_UpdateUtilityParts` is left `true` — after that `QueueUpdateUtilityParts` is a no-op for the entity. Static
reading, not measured; no vanilla `structures_takistan` part sets both keys (checked by script over all three
configs).

### 11.4 House collisions and the dismantle error

```c
// [EXACT] 4_world/classes/constructionbase.c:1518-1524
	//! Whether House entities are skipped in the part collision check.
	//! Rebuildable buildings are static, so a part may legitimately overlap
	//! a House placed touching the rebuilt one (map authoring decision).
	bool IgnoreHouseCollisions()
	{
		return false;
	}
```

`IsCollidingEx` skips a collided `House` when it returns true (`constructionbase.c:1568-1569`); `Rebuilding`
returns true (`rebuilding.c:805-810`). A modded building that must still block on neighbouring houses overrides
`IgnoreHouseCollisions()` in its own `Rebuilding` subclass (installed through `CreateConstructionComponent()`).

`DropNonUsableMaterialsServer` (`constructionbase.c:1262-1314`) now `continue`s when the material slot of the base
part is empty (`:1288-1290`). 1.30.164078 raised `ErrorEx("Attachment is not present for material type '%1' on
part %2")` there; a Namalsk fence dismantle was reported to end in a VM exception at that line on 164078
[reported: SumrakDZN/DayZNamalsk#4]. The `ErrorEx` for an invalid cached slot id stays (`:1282-1286`).

### 11.5 Porting table (mod written on 1.30.164078)

| 164078 code | On 164200 | Use instead |
|---|---|---|
| `override int GetSupportProvided(int)` / `GetSupportRequirement(int)` (ConstructionPart*), `GetPartTypeSupportProvided/Requirement(int)` (TypeData), `ConstructionPartStaticsSupportData.GetSupport*(int)` | compile error | `override bool …(int category, out int strength)` (§11.1) |
| `int v = part.GetSupportProvided(cat);` | compile error | `int v; if (part.GetSupportProvided(cat, v)) …` |
| `override void TryHandleUtilityParts(EntityAI parent)` / calls | compile error (removed) | `TryQueueUtilityPartsUpdate(ConstructionBase con)`; force with `con.QueueUpdateUtilityParts()` |
| `GetPartTypeUtilityAutobuildPartsList()`, `m_UtilityAutomaticBuild` | compile error (removed) | iterate `GetConstructionParts()` (`constructionbase.c:574`) and test `IsUtilityPart()` |
| own members `InvertSupportBehavior`, `IgnoreHouseCollisions`, `QueueUpdateUtilityParts`, `UpdateUtilityParts`, `m_UpdateUtilityParts` | name meets a new vanilla member | rename, or `override` if it is meant to be the vanilla one |
| config `utility_build[]={"x_utility"};` | not read, the utility part never switches | `toggle_utility_part_update=1;` on the trigger part; describe the utility part's state through its own `required_parts[]` / `conflicted_parts[]` / `SupportRequirements` |
| logic expecting `CanBuildUtilityPart` false for a built part | now true while the conditions hold | compare with `IsBuilt()` yourself |
| reading a utility part's state right after `BuildPartServerEx` | still the old state until the same frame's post-update | react in `UpdateUtilityParts` (override + `super`) or later |
