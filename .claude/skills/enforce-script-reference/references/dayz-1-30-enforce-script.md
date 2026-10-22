# DayZ 1.30 — Enforce Script contract

> Target: DayZ 1.30 stable (1.30.164200). Version labels: ../../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

Consultation reference for `enforce-script-reference`. Every `[EXACT]` block below was re-opened in
`P:\scripts` (1.30.164078, re-checked 2026-10-15); the first written version of this file was checked
against 1.30 Experimental 164014, and every claim that did not hold on stable has been corrected here.
`1.29: O:\…` cites the old tree only to show what an old mod was written against. `[DESIGN]` = migration
advice. `[reported: bzed]` = third-party report from a Linux 1.30.164014 server, not reproduced here.
Patch 1.30.164200 (2026-10-21): the sentences it made wrong were corrected in place (§7.5 server-side CCT, §8.3
`vector.Cross`, §8.7 `FindFile`); what it broke and how to design mods that survive the next patch: §9.
`[reported: BI 1.30.164200 notes]` = an engine fix from the official patch notes that this machine has not measured.

Before 1.30: CE tick `OnCEUpdate()`, inventory hooked `ProcessInputData`, fill-bottle `ActionFillBottleBase`,
3rd-person `World.Is3rdPersonDisabled()`, storage version `142`, `TakeEntityToCargo`/`TakeEntityAsAttachment`.
The 1.30 replacements are below.

## 1. Central Economy tick: `OnCEIterate(float currentTime, float elapsedTime)`

```c
// [EXACT] P:\scripts\3_game\entities\entityai.c:3904
	void OnCEIterate(float currentTime, float elapsedTime)
	{
		ProcessVariables(elapsedTime);
		
		ConstructionBasic constructionComponent = GetConstructionBasic();
		if (constructionComponent && ConstructionBasic.CanConstructionDecay() && GetAllowDamage()) //checks if server allows damage AND decay
			constructionComponent.ProcessConstructionDecay(elapsedTime);
	}
	
	void ProcessVariables(float elapsedTime)
```

```c
// [EXACT] P:\scripts\3_game\entities\entityai.c:4924
	[Obsolete("1.30: Use OnCEIterate(float currentTime, float elapsedTime) instead")]
	void OnCEUpdate();
```

- `OnCEUpdate()` is declared without a body and nothing in `P:\scripts` calls it; vanilla overrides use
  `OnCEIterate` (`4_world\classes\contaminatedarea\effectarea.c:191`, `4_world\entities\explosivesbase.c:82`,
  `4_world\entities\explosivesbase\remotedetonator.c:97`, `4_world\entities\itembase\basebuildingbase\totem.c:157`).
  An `override void OnCEUpdate()` compiles with an obsolete warning and never runs: 0 calls against 446k
  `OnCEIterate` calls in 2 min with 20 items [reported: bzed, 1.30.164014 server].
- `ProcessVariables` now takes the elapsed time: `EntityAI.ProcessVariables(float elapsedTime)` (`entityai.c:3913`),
  `ItemBase` override `4_world\entities\itembase.c:4663`. A 1.29 `override void ProcessVariables()` is a compile
  error ("marked as override, but there is no function with this name", [reported: bzed]).
- The fields `m_ElapsedSinceLastUpdate` / `m_LastUpdatedTime` are gone (1.29: `O:\…\3_game\entities\entityai.c:138-139`);
  use the `elapsedTime` argument.

`[DESIGN]` Port: `override void OnCEIterate(float currentTime, float elapsedTime) { super.OnCEIterate(currentTime, elapsedTime); … }`.
Before 1.30: `override void OnCEUpdate() { super.OnCEUpdate(); … m_ElapsedSinceLastUpdate … }` (1.29: `O:\…\3_game\entities\entityai.c:3869`).

## 2. `GAME_STORAGE_VERSION = 144`

```c
// [EXACT] P:\scripts\3_game\global\game.c:5
static int GAME_STORAGE_VERSION = 144;
```

`CGame` still calls `StorageVersion(GAME_STORAGE_VERSION)` (`game.c:50`). Before 1.30: `142` (1.29: `O:\…\3_game\global\game.c:5`).
Do not hardcode `142`/`144` against `g_Game.SaveVersion()`; compare with `>=` gates as vanilla does. Stream layout,
the player-record break and the PCO stats change: skill `dayz-persistence`, `references/dayz-1-30-persistence.md`.

## 3. Inventory command pipeline (retail does not call `ProcessInputData`)

`ProcessInputData(ParamsReadContext ctx, bool isJuncture, bool isRemote)` still exists
(`P:\scripts\4_world\systems\inventory\dayzplayerinventory.c:3319`) but every call site is inside
`#ifdef DIAG_DEVELOPER` **and** behind `PluginInventoryDebug.IsOldProcessInputDataEnable()` (default `false`,
`4_world\plugins\pluginbase\plugininventorydebug.c:14`). Retail uses Validate / Execute*.

```c
// [EXACT] P:\scripts\4_world\systems\inventory\dayzplayerinventory.c:669
#ifdef DIAG_DEVELOPER
		if (PluginInventoryDebug.Cast(GetPlugin(PluginInventoryDebug)).IsOldProcessInputDataEnable())
		{
			ProcessInputData(ctx, true, false);
		}
		else
		{
#endif
			if(g_Game.IsServer())
			{
				ExecuteInventoryCommandServer(ctx);
			}
			else
			{
				ExecuteInventoryCommandClient(ctx);
			}
#ifdef DIAG_DEVELOPER
		}
#endif
```

| Method | Cite (`dayzplayerinventory.c`) | Role |
|---|---|---|
| `bool ValidateInventoryCommandServer(ParamsReadContext ctx)` | `:3410` | Server: validate input user data, send juncture |
| `bool ExecuteInventoryCommandServer(ParamsReadContext ctx)` | `:3501` | Server execute |
| `bool ExecuteInventoryCommandClient(ParamsReadContext ctx)` | `:3569` | Owner client after juncture |
| `bool ExecuteInventoryCommandRemote(ParamsReadContext ctx)` | `:3633` | Remote client execute |

Input user data takes the same DIAG gate and then `ValidateInventoryCommandServer` on the server (`:734-752`).
`[DESIGN]` A `modded DayZPlayerInventory` that only overrides `ProcessInputData` is silent in retail.

### 3.1 `INPUT_UDT_INVENTORY_CHECK` / `OnInventoryCheck`

```c
// [EXACT] P:\scripts\3_game\tools\component\_constants.c:19
const int INPUT_UDT_INVENTORY_CHECK					= 16;
```

Same value on 1.29. Caps: `INVENTORY_REPAIR_MAX_DISTANCE = 500.0`, `INVENTORY_REPAIR_MAX_ITEMS = 5`
(`dayzplayerinventory.c:163-164`, comments misspelled in vanilla). `override bool OnInventoryCheck(int userDataType, ParamsReadContext ctx)`
is at `:5003`. A mismatch sends `InventoryInputUserData.SendServerInventoryCheck` (`:5052`): unconditionally on a
retail build; on a `DIAG_DEVELOPER` build only while `PluginInventoryDebug.IsDesyncRepairEnable()` is true (default
`true`, `plugininventorydebug.c:12`) (`:5048-5055`).

---

## 4. `[Obsolete]` takes → `*ToTarget*` variants

`GameInventory`: the four old takes are `[Obsolete]` wrappers that forward to the target variants with
`GetInventoryOwner()` as the target.

```c
// [EXACT] P:\scripts\3_game\systems\inventory\inventory.c:1345
	[Obsolete("1.30: Use TakeEntityToTargetCargo instead")]
	bool TakeEntityToCargo(InventoryMode mode, notnull EntityAI item)
```

Same file: `TakeEntityToCargoEx` `[Obsolete]` `:1357`; `TakeEntityAsAttachmentEx` `[Obsolete]` `:1374` (attribute
line is extra-indented in vanilla) forwarding to `TakeEntityAsTargetAttachmentEx(mode, GetInventoryOwner(), item, slot)`
(`:1378`); `TakeEntityAsAttachment` `[Obsolete]` `:1382`.

Replacements: `TakeEntityToTargetCargo(mode, target, item)` `:1077`; `TakeEntityToTargetCargoEx(mode, cargo, item, row, col)`
`:1084`; `TakeEntityAsTargetAttachmentEx(mode, target, item, slot)` `:1100`; `TakeEntityAsTargetAttachment(mode, target, item)` `:1142`.

Entity level: `EntityAI.Predictive/Local/ServerTakeEntityToTargetAttachmentEx(target, item, slot)`
(`3_game\entities\entityai.c:2020-2033`); the old `*TakeEntityAsAttachmentEx(item, slot)` are `[Obsolete]`
(`entityai.c:4872-4888`). `Man` overrides the target variants (`3_game\entities\man.c:653-688`); its old
`JunctureTakeEntityAsAttachmentEx` / `TakeEntityAsAttachmentExImpl` are `[Obsolete]` (`man.c:918-929`). The
`Man.*TakeEntity*` family moved to `EntityAI`, so any entity can be the actor [reported: bzed; consistent with the
declarations above].

The SKILL.md SP-184 identity gate (`FindAttachment(slot) == item`) still applies: an occupied slot returns `true`
after `CombineItemsClient(item, true)` without attaching (`inventory.c:1106-1117`), and on a live player the LOCAL
path ends in `DayZPlayerInventory.TakeToDst` → `case InventoryMode.LOCAL: LocationSyncMoveEntity(src, dst); return true;`
(`dayzplayerinventory.c:3728`, `:3808-3810`).

---

## 5. Hand FSM guards

Before 1.30: `HandGuardCanMove` / `CanSwap` / `CanForceSwap` auto-passed `e.m_IsJuncture || e.m_IsRemote`
(1.29: `O:\…\3_game\systems\inventory\hand_guards.c:299,325,351`). 1.30: only `e.m_IsRemote`; otherwise
`LocationCanMoveEntity` / `CanSwapEntitiesEx` / `CanForceSwapEntitiesEx`.

```c
// [EXACT] P:\scripts\3_game\systems\inventory\hand_guards.c:299
		bool result = e.m_IsRemote;
		if (result == false)
		{
			result = GameInventory.LocationCanMoveEntity(es.GetSrc(), es.GetDst());
		}
```

Same `m_IsRemote` test at `:325` and `:351`. New guard `HandGuardIsNotSurrendered` (`:405`) returns
`!m_Player.IsSurrendered()` (`:412`; `IsSurrendered` is declared on `Man`, `3_game\entities\man.c:67`).
`[DESIGN]` Hand events on a surrendered player fail the FSM.

---

## 6. `GUIInventoryAttachmentsProps`, `TypeAttachmentGroupData`, `ItemBaseType`

```c
// [EXACT] P:\scripts\3_game\entities\typeattachmentgroupdataholders.c:39
		m_PropsPath = "" + pathBase + " GUIInventoryAttachmentsProps";
		if (!g_Game.ConfigIsExisting(m_PropsPath) || m_AttachmentGroupMap != null)
			return;
```

Category reach is a 1.5 m dome with vertical leeway `-range/5`: `IsCategoryReachable(EntityAI ent, string categoryName, float range = 1.5)`
(`5_mission\gui\inventorynew\inherited\attachmentcategoriesrow.c:764`), `negativeTolerance = -range / 5` (`:778`) → −0.3 m.

`ItemBaseType` (`typedef ItemBaseType Inventory_BaseType`, `4_world\entities\itembasetype.c:2`) caches type config in its
constructor (`:55-139`). Keys: `varWetInit` / `varWetMin` / `varWetMax` `:69-71`, `itemModelLength` / `itemAttachOffset` `:87-92`,
`lockType` / `compatibleLocks` `:96-99`, `headSelectionsToHide` `:105-108`, `hideSelectionsByinventorySlot` `:112-113`,
`quickBarBonus` `:115`, `temperaturePerQuantityWeight` `:118-119`, `EnvironmentWetnessIncrements` Soaking/Drying `:121-137`.
Rebuilding slots `Slot_RB_LvL*` start at `P:\scripts\config.cpp:2915` (`Slot_RB_LvL1_Log`, `stackMax = 15`).

---

## 7. Action system 1.30

### 7.1 `CanBeStarted`, `IsTargetInfoAction`, `SortActions`, `InitInfoData`

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actionbase.c:276
	//! Should we display additional info panel on ActionTargetsCursor
	bool IsTargetInfoAction()
	{
		return m_InfoData != null;
	}
```

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actionbase.c:287
	//! If for some reason we want a non-performable action (like pure info ones)
	bool CanBeStarted()
	{
		return true;
	}
```

`InitInfoData()` is the empty hook (`actionbase.c:159`). Info-only construction: `actions\continuous\actionpartinfo.c:18-21`
`override bool CanBeStarted() { return false; }`. The client does not `ActionStart` unless `CanBeStarted()`
(`actionmanagerclient.c:336-337`).

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actioninput.c:464
	//! sorts in order: performable -> non-performable (usually info). Extend/override as needed
	protected void SortActions(array<ActionBase> allActions)
```

### 7.2 `PerformAction` / `Can()` / `UA_AM_*`

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actionmanagerclient.c:796
#ifdef BOT
	/// used for bots
	void PerformAction(int user_action_id, ActionTarget target, ItemBase item, Param extraData = NULL)
	{
		ActionStart(GetAction(user_action_id), target, item, extraData);
	}
#endif
	
	void PerformActionStart(ActionBase action, ActionTarget target, ItemBase item, Param extra_data = NULL)
```

`BOT` still has no `#define` anywhere in `P:\scripts`. In single player `PerformActionStart` no-ops while a pending or
current action exists (`:806-814`); `GetActionState()` returns `UA_AM_INIT` while an action is pending (`:822-827`).
`Can(PlayerBase, ActionTarget, ItemBase)` is at `actionbase.c:968`; the 4-arg overload with `condition_mask` is above
it at `:929`. Probing the public `Can` (never the protected `ActionCondition`) is still the rule.
Before 1.30: `PerformAction` `:756` inside `#ifdef BOT` `:754-760`, `PerformActionStart` `:762`, `Can` `:912`
(1.29: `O:\…\4_world\classes\useractionscomponent\actionmanagerclient.c`, `actionbase.c`).

```c
// [EXACT] P:\scripts\3_game\constants.c:508
const int		UA_AM_INIT = 14;
const int		UA_AM_PENDING = 15;
const int		UA_AM_ACCEPTED = 16;
const int		UA_AM_REJECTED = 17;
```

Before 1.30: `UA_AM_PENDING = 14`, `ACCEPTED = 15`, `REJECTED = 16` (1.29: `O:\…\3_game\constants.c:479-481`). Never
hardcode the integers. `ActionTarget` gained a trailing `string = ""` constructor parameter; existing `new ActionTarget(…)`
calls compile unchanged.

### 7.3 `ActionFillBottleBase` → `ActionObtainLiquidBase`

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actionfillbottlebase.c:28
[Obsolete("1.30: replaced by class ActionObtainLiquidBase")]
class ActionFillBottleBase: ActionContinuousBase
```

Its callback is obsolete too (`:2-3`, → `ActionObtainLiquidBaseCB`). `RegisterActions` inserts `ActionObtainLiquidBase`
(`actionconstructor.c:156`), not FillBottle; `Bottle_Base.SetActions` adds it (`4_world\entities\itembase\edible_base\bottle_base.c:364`).

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actionobtainliquidbase.c:50
		m_AllowedLiquidMask = LIQUID_GROUP_DRINKWATER | LIQUID_GASOLINE;
		m_AllowedLiquidMask &= ~LIQUID_SNOW;
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTLiquid(m_AllowedLiquidMask, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
```

`[DESIGN]` `modded ActionFillBottleBase` is never instantiated by vanilla; port custom fill actions to `ActionObtainLiquidBase`.

### 7.4 Animation overrides on `EntityAI`

`class ActionOverrideData : Managed` is at `3_game\entities\entityai.c:103`. Map:

```c
// [EXACT] P:\scripts\3_game\entities\entityai.c:117
	static ref map<typename, ref TActionAnimOverrideMap> m_EntityActionOverrides = new map<typename, ref TActionAnimOverrideMap>; 
```

`OverrideActionAnimation(typename action, int commandUID, int stanceMask = -1, int commandUIDProne = -1)` is at `:4781`.
`AnimatedActionBase.GetCommandOverride` (`4_world\classes\useractionscomponent\animatedactionbase.c:279`) looks up the
main item's type, then the **target object's** type (`:288-290`). Before 1.30 this lived on `ItemBase.m_ItemActionOverrides`;
that field is gone (compile error in an old mod).

### 7.5 CCT: `CCTLiquid`, `CCTCursorInherited`, head distance

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\targetconditionscomponents\cctliquid.c:37
	void CCTLiquid(int allowedLiquidSource, float maximalTargetDistance = UAMaxDistances.DEFAULT, float maxVerticalReach = UAMaxDistances.DEFAULT, SurfaceDetectionType surfaceDetectionType = SurfaceDetectionType.Roadway, bool allowSeaLevelCursorFallback = false)
```

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\targetconditionscomponents\cctcursorinherited.c:28
				vector cursorHitPos = target.GetCursorHitPos();
				bool fromPlayerRoot = vector.DistanceSq(cursorHitPos, player.GetPosition()) <= m_MaximalActionDistanceSq;
				bool fromPlayerHead = vector.DistanceSq(cursorHitPos, MiscGameplayFunctions.GetPlayerHeadPosition(player)) <= m_MaximalActionDistanceSq;
```

`cctcursor.c:36` (and `cctcursornoobject.c:19`, `cctcursornoruincheck.c:36`, `cctcursorparent.c:33`) also use
`GetPlayerHeadPosition`: a stance-based height from `GetPlayerHeadHeight` (`4_world\static\miscgameplayfunctions.c:761-772`,
`:778-789`), not the Head bone. Since 1.30.164200 that head / cursor-hit test runs **only on the client** for
`CCTCursor`, `CCTCursorNoRuinCheck` and `CCTCursorParent`. On the server they return early with a plain distance test
from the target's origin (the parent's for `CCTCursorParent`) to the player's origin, widened by the target's collision radius:

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\targetconditionscomponents\cctcursor.c:29-33
		if (player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			float maxDistanceWithSize = m_MaximalActionDistance + targetObject.GetCollisionRadius();
			return (vector.DistanceSq(targetObject.GetPosition(), player.GetPosition()) <= maxDistanceWithSize * maxDistanceWithSize);
		}
```

Same block: `cctcursornoruincheck.c:29-33`, `cctcursorparent.c:26-30`; new field `m_MaximalActionDistance` (`cctcursor.c:4`).
BI lists it as the fix for "exploit enabling action usage at any distance (T187421)": the server used to test a cursor
hit point that the client supplies. Unchanged: `CCTCursorNoObject` still returns `true` on an MP server
(`cctcursornoobject.c:15-16`); `CCTCursorInherited` keeps the head/root test above on both sides.

- The two sides now measure different things. The client measures from the cursor hit point to the feet or the stance
  head; the server measures origin to origin. A small target (collision radius near 0) whose origin sits above or
  beside the spot the player aims at, e.g. an item on a shelf or on top of a wall, can be refused by the server at the
  edge of reach while the client shows the prompt. A large object gets a larger server allowance.
- The server no longer checks the cursor hit point, and `CCTCursor` never ran a line-of-sight test. An action whose
  security depends on what the player really aims at (through a wall, a specific selection or component, a lock part)
  must re-check it in its own server-side `ActionCondition`.
- A custom CCT that copies the 164078 `CCTCursor.Can` body keeps the old exploitable server behaviour. Extend the vanilla
  class or copy the server branch too.
- Before 1.30.164200 (1.30.164078): the server ran the same head/root cursor-hit test as the client.

### 7.6 New inputs in `P:\bin\constants.xml`

```xml
// [EXACT] P:\bin\constants.xml:90
		<input name="UAToggleHeadlight" loc="TOGGLE_LIGHT_HEAD" />
		<input name="UAToggleVehicleLights" loc="TOGGLE_LIGHT_HEAD" />
```

`UAToggleVehicleLights` is new (1.29 had only `UAToggleHeadlight`, `O:\…\bin\constants.xml:90`). Also new: `:282-285`
`UABuldLinkCamToTerrain` / `UABuldCopyTileCoord` / `UABuldMarkUnderground` / `UABuldRemoveUnderground`; `:308-311`
`UAUICodeLockLeft/Right/Up/Down`. `UACarHorn` remains (`:203`). Vehicle lights use `ToggleVehicleLightsActionInput`
→ `UAToggleVehicleLights` (`4_world\classes\useractionscomponent\actioninput.c:849-855`); `ToggleLightsActionInput`
still binds `UAToggleHeadlight` (`:776-782`).

### 7.7 Recipes

Before 1.30, `RecipeBase` called `MiscGameplayFunctions.TransferItemProperties(ingr, res)` (1.29: `O:\…\4_world\classes\recipes\recipebase.c:353`),
so a result with `m_ResultReplacesIngredient` took the ingredient's health. 1.30 passes `transfer_health = false`; the
signature itself is unchanged (`4_world\static\miscgameplayfunctions.c:271`, defaults `true`).

```c
// [EXACT] P:\scripts\4_world\classes\recipes\recipebase.c:353
						MiscGameplayFunctions.TransferItemProperties(ingr, res, true, true, false, false);
```

`GetRecipeClassName(int recipe_id)` is `4_world\plugins\pluginbase\pluginrecipesmanager.c:84`.

### 7.8 `ActionTargetsCursor.Update(bool fullUpdate = true)`

1.30: `void Update(bool fullUpdate = true)` (`P:\scripts\5_mission\gui\actiontargetscursor.c:310`) and
`protected void BuildFixedCursor(bool forceRebuild = true)` (`:267`). Before 1.30: `Update()` / `BuildFixedCursor()`
(1.29: `O:\…\5_mission\gui\actiontargetscursor.c:305`, `:230`). A 1.29 override does not compile on 1.30:
`Overloaded function 'Update' not compatible` (DayZ-MCP PBO 12, `mcp_actioncursor.c(60)`; measured on 1.30 Exp 164014,
2026-10-09). Write the 1.30 signature. Only if the same PBO must still load on 1.29, keep the old signature under the
old version's define (rule 1 of `../../_shared/dayz-version-policy.md`):

```c
#ifdef DAYZ_1_29
	override void Update()
	{
		super.Update();
#else
	override void Update(bool fullUpdate = true)
	{
		super.Update(fullUpdate);
#endif
		// shared body
	}
```

### 7.9 `CanDisplayAttachmentSlot(string)` is `[Obsolete]`

```c
// [EXACT] P:\scripts\3_game\entities\entityai.c:4918
	[Obsolete("1.30: Use CanDisplayAttachmentSlot( int slot_id ) instead")]
	bool CanDisplayAttachmentSlot( string slot_name )
	{
		return CanDisplayAttachmentSlot(InventorySlots.GetSlotIdFromString(slot_name));
	}
```

The live overload is `bool CanDisplayAttachmentSlot(int slot_id)` (`entityai.c:1788`). Vanilla inventory UI calls only the
int overload (`5_mission\gui\inventorynew\attachments.c:331`, `:536`; `inherited\attachmentcategoriesrow.c:663`;
`containeditems\constructioninfoicons.c:262`), so an override of the **string** overload is never consulted by vanilla.
That was already true on 1.29 (same call sites, `O:\…\5_mission\gui\inventorynew\attachments.c:330,535`); 1.30 adds the
`[Obsolete]` warning and makes the string overload forward to the int one. Override the int overload.

### 7.10 Patch 1.30.164200: same signatures, different behaviour

Nothing below fails to compile; each item changes what an existing mod sees. Server-side CCT distance: §7.5.

- **Target actions no longer lock the target.** `ActionBase` reserves the target with `AddInventoryJunctureEx` only when
  `IsLockTargetOnUse()` is true (`4_world\classes\useractionscomponent\actionbase.c:1178-1190`). 23 vanilla actions on
  another player now return false: bandage, sew, burn-sew, splint, give blood/saline, collect blood/sample, test blood,
  defibrillate, inject, disinfect, check pulse, measure temperature, restrain/unrestrain(-empty)/handcuff, gag/ungag,
  cover/uncover head, shave (e.g. `actions\continuous\medical\actionbandagetarget.c:71`). Official note: "desynchronization
  caused by interacting with another character while they are dropping an item". A mod action that extends one of them
  inherits `false`: two players can now run it on the same target at once, and the target's inventory is not reserved.
  If your action must be exclusive, override `IsLockTargetOnUse()` back to true or guard it on the server yourself.
- **Action ids.** `ActionConstructor.ConstructActions` gives each action `SetID(actionsArray.Count())` in list order after
  `RegisterActions` and a `Sort(...)` call (`actionconstructor.c:3-24`). 164200 moved `ActionDrinkCan` up next to
  `ActionWashHandsWell` (`:208`) to fix "Wash Face offered before Drink" (DZEXP-164), so registration order is
  observable. Whether that `Sort` reorders an `array<typename>` was not measured. Either way the runtime id of a vanilla
  or mod action depends on the build and on the mod set: never store, hardcode or send an action id as a raw int across
  builds; pass the action typename/class name.
- **Obstruction helpers (client side).** `ActionTargets` limits its obstruction raycast to 10 m
  (`actiontargets.c:449`) and gives trees/bushes target utility 0.1 (`:505`, between ground 0.01 and buildings 0.25).
  `MiscGameplayFunctions.FilterObstructedObjectsByGrouping` (`4_world\static\miscgameplayfunctions.c:1438`) no longer
  groups: one raycast per object, and the `range`, `distanceDelta` and `obstructingObjects` arguments are ignored.
  `IsObjectObstructedEx` (`:1510`) with `maxDist > 0` now clamps the ray to `maxDist` (`:1521-1530`), so an obstacle beyond
  that distance no longer counts. `IsObjectObstructedFilterEx` (`:1593`) skips both raycasts when the target centre is
  closer than 0.25 m (`OBJECT_OBSTRUCTION_MIN_DISTANCE_SQ = 0.0625`, `:215`), runs the cheap `RaycastRV` first and the proxy
  raycast second. `IsObjectObstructedCache` gained `RaycastEnd` (`:2056`) and `GetRaycastParams` (`:2072`). Both helpers still
  read `g_Game.GetPlayer()` (`:1503`, `:1515`), so they remain client-side tools; do not call them on a server.
- **Inventory side effects.** Split-to-ground uses `GameInventory.PrepareDropEntityPos(…, INVENTORY_ENTITY_DROP_OVERLAP_DEPTH)`
  (`4_world\entities\itembase.c:2090`); items thrown out of a parent get `-GetVelocity(parent)` (`miscgameplayfunctions.c:1248`,
  `:1295`); `MotorbikeScript.CanReleaseAttachment` refuses while the engine runs or crew is present
  (`4_world\entities\vehicles\motorbikescript.c:612-626`); compass and GPS count items inside worn clothing as "in
  possession" and the GPS switches off when moved anywhere else (`itembase\compass.c:63`, `gear\navigation\gpsreceiver.c:169`).
- **Plugins and logs.** `PluginInventoryRepair` is now a regular client plugin in release builds (client `true`, server
  `false`; `4_world\plugins\pluginmanager.c:58`), before 164200 debug-only. `Debug`'s autotest log channel is
  `"AutotestLog"` (`3_game\tools\debug.c:19`, was `"Autotest"`): a log filter or tool that greps the old name misses it.
- Vehicles (`Transport.HasCrew()` proto native, `Transport.IsMoving()`), base building support API, motorbike lights:
  skill `dayz-vehicles` / `dayz-motorbikes` / `dayz-basebuilding`, full list `dayz-130-migration`
  `references/api-delta-1.30.164078-to-164200.md`.

---

## 8. Engine / proto API (`1_Core`, `3_Game`)

### 8.1 Third person

```c
// [EXACT] P:\scripts\3_game\global\world.c:177
	proto native ThirdPersonMode GetThirdPersonViewMode();
```

```c
// [EXACT] P:\scripts\3_game\global\world.c:291
	[Obsolete("1.30: replaced by World::GetThirdPersonViewMode returning current thrid person mode")]
	proto native bool Is3rdPersonDisabled();
```

```c
// [EXACT] P:\scripts\3_game\global\world.c:301
enum ThirdPersonMode
{
	DISABLED,
	ENABLED,
	VEHICLES_ONLY
}
```

`[DESIGN]` Compare to `ThirdPersonMode.ENABLED` (or `VEHICLES_ONLY`); a bool cannot express "vehicles only". Vanilla:
`m_Camera3rdPerson = g_Game.GetWorld().GetThirdPersonViewMode() == ThirdPersonMode.ENABLED;`
(`4_world\entities\dayzplayerimplement.c:194`). The typo `thrid` is in vanilla's `[Obsolete]` string.

### 8.2 Weather noise — not deleted, and no longer used by vanilla AI

Both methods exist and both are `[Obsolete]`. AI noise dampening moved to native `AIParams` config (attribute text below).

```c
// [EXACT] P:\scripts\3_game\weather.c:407
	[Obsolete("1.30: AI noise dampening is now done natively via AIParams config parameters. Kept for HUD and old mods.")]
	float GetNoiseReductionByWeatherEx(notnull Object object)
```

```c
// [EXACT] P:\scripts\3_game\weather.c:454
	[Obsolete("Use Weather.GetNoiseReductionByWeatherEx instead!")]
	float GetNoiseReductionByWeather()
```

The only remaining caller in `P:\scripts` is the equally obsolete wrapper `NoiseAIEvaluate.GetNoiseReductionEx`
(`4_world\static\sensesaievaluate.c:18-23`); no HUD script calls `GetNoiseReductionByWeatherEx` on 1.30.0. A mod that
scales noise through these methods no longer changes what AI hears [reported: bzed; consistent with the attribute text].

### 8.3 Cross product, `vector.SanitizeMinsMaxs`, `array.Slice`, `EnumFlagsToString`

New pure-script helpers (bodies in script, not `proto native`; absent from 1.29).

**`vector.Cross` does not exist on 1.30.164200.** 1.30.164078 added it as a script method (`1_core\proto\enconvert.c:239-246`
on that build) and 164200 deleted it again, so `a.Cross(b)` no longer compiles. The cross product is the `*` operator
between two vectors, which is what vanilla writes (it is on 1.29 too):

```c
// [EXACT] P:\scripts\4_world\classes\environment\environment.c:696-697
		vector right = (forward * up).Normalized(); 	// X axis
		vector back = (up * right).Normalized(); 	// Z axis
```

`vector.Perpend()` is `value * vector.Up` (`enconvert.c:222-224`). A mod that must compile on 1.29, 164078 and 164200 alike
uses `a * b` (or its own static helper with a prefixed name); never a vanilla script helper that appeared in one build.

```c
// [EXACT] P:\scripts\1_core\proto\enscript.c:720
	array<T> Slice(int from, int to)
	{
	    array<T> result = new array<T>();
		for (int i = from; i <= to && i < Count(); ++i)
	        result.Insert(Get(i));

	    return result;
	}
```

`to` is inclusive. `static void SanitizeMinsMaxs(inout vector min, inout vector max)` `enconvert.c:521`.
`typename.EnumFlagsToString(typename enumType, int value)` `enconvert.c:593`; `EnumTools.EnumFlagsToString` `:683` forwards to it.
Also new: `RGBToHSV` / `HSVToRGB` (`1_core\proto\proto.c:362`, `:403`), `COLOR_CYAN` / `COLOR_MAGENTA` (`1_core\constants.c:68-69`),
`ProjectWorldToViewport` / `ProjectViewportToWorld` (`1_core\proto\enworld.c:113-114`). Being script, they can be copied into
a mod that must still compile on 1.29 [reported: bzed] — under a mod-prefixed name, or the copy collides with vanilla on 1.30.

### 8.4 `DoOnce`, `IsFirstRenderFrame`

```c
// [EXACT] P:\scripts\3_game\doonce.c:1
class DoOnce
{
    protected static ref map<string, bool> m_Done;
```

`static bool DoOnce.Run(string key, Class target, string fnName, Param params = null)` (`:12`) marks the key and then calls
`g_Game.GameScript.CallFunctionParams` once per key (`:21-27`); `HasRun` `:30`, `Clear` `:37`.

```c
// [EXACT] P:\scripts\3_game\dayzplayer.c:1186
	//! Returns true if this is the first tick of the render frame.
	//! If the client FPS is below 30fps, the player could tick multiple times in one render frame, some systems don't 
	//! need to be processed on the client again if it has no visible effect to the screen or server simulation.
	proto native bool IsFirstRenderFrame();
```

Not new: `Entity.DisableSimulation(bool)` / `GetIsSimulationDisabled()` (`3_game\entities\entity.c:3-6`) exist with the same
lines on 1.29 — an earlier version of this file listed them as 1.30 additions by mistake.

### 8.5 `CGame`: headless, object spawner, surface info

```c
// [EXACT] P:\scripts\3_game\global\game.c:1130
	proto native bool		IsHeadless();
	
	/**
	 \brief Check whether the application is headless or a dedicated server (it combines IsDedicatedServer and IsHeadless)
		\note This includes servers with a head as well (-headlessMode=0)
	*/
	proto native bool		IsHeadlessOrDedicatedServer();
```

Both are new in 1.30 (headless roboclients). `[DESIGN]` For "no rendering / skip VFX" prefer `IsHeadlessOrDedicatedServer()`;
for the client/server **side** guard keep the existing rules (SKILL.md rules 19-20).

- `CreateObject(string type, vector pos, bool create_local = false, bool init_ai = false, bool create_physics = true, bool objSpawner = false)`
  (`game.c:691`) and `CreateStaticObjectUsingP3D(…, bool createLocal = false, bool objSpawner = false)` (`game.c:680`) gained a
  trailing defaulted `objSpawner` (undocumented in vanilla). Existing calls compile unchanged. Related new flag:
  `ECE_OBJECT_SPAWNER = 256` "set when the object is spawned through object spwner" (`3_game\ce\centraleconomy.c:15`).
- Surface info: `proto SurfaceInfo GetSurfaceInfoOn(float x, float z, out float height)`, `GetSurfaceInfoUnderObject(notnull Object)`,
  `GetSurfaceInfoUnderObjectByBone(notnull Object, int boneType)` (`game.c:1171-1173`); `class SurfaceInfo : SurfaceProperties`
  (`3_game\surfaceinfo.c:8`, e.g. `GetSurfaceType()` `:26`, `IsLiquid()` `:36`). `[Obsolete]` on 1.30:
  `DayZPlayerImplement.GetSurfaceType(SurfaceAnimationBone)` (`4_world\entities\dayzplayerimplement.c:4121-4122`) and
  `Surface.GetStepsParticleID` (`4_world\static\surface.c:64`). **Not** obsolete on 1.30.164078, contrary to the bzed guide:
  `CGame.SurfaceUnderObject` / `SurfaceUnderObjectEx` / `SurfaceUnderObjectByBone` (`game.c:1184-1186`, no attribute).

### 8.6 `ERPCs` values shifted

`RPC_SYMPTOM_PARAM_SYNC` was inserted right after `RPC_PLAYER_SYMPTOM_OFF` (`3_game\enums\erpcs.c:23`), plus 14 more
entries (sandstorm debug, item sound, `RPC_CODE_LOCK_*` `:116-118`, bunker broadcast, diag). Every vanilla ID after
`RPC_PLAYER_SYMPTOM_OFF` changes value (e.g. `RPC_STAMINA` 15 → 16). Mods that store or send vanilla RPC IDs as raw ints, or
count from a vanilla value, break silently. Use the enum names and keep mod IDs in your own high range (`references/networking.md`).

### 8.7 File I/O on 1.30 servers

The script declarations did not change in 1.30 or in 164200 (`FindFile` `1_core\proto\ensystem.c:520`, `DeleteFile` `:528`
"Works only on `$profile:` and `$saves:`", `CopyFile` `:531` destination `$profile:`/`$saves:`). The engine-side behaviour:

- **`FindFile` enumeration — history and current status.**
  - 1.30.164014 [reported: bzed, Linux server]: ignores the `$profile:` / `$mission:` / `$storage:` / `$saves:` placeholders
    and silently lists the server's working directory (BI ticket DZEXP-134).
  - 1.30.164078 (measured on 1.30.164078, this machine's Windows server, 2026-10-15): `$profile:` **and absolute paths**
    were ignored; only the server working folder and its subfolders were listed, `..` did not work. Reading or writing a
    known file name through a placeholder (`OpenFile`, `FileExist`, `MakeDirectory`, `CopyFile`, `DeleteFile`,
    `JsonFileLoader`) worked. Working pattern then: a path relative to the server folder (found once via a sentinel
    file) plus an id index file as fallback (INT_P2P_Trading `INT_P2P_FileStore`); `CF.FindFileEx` (CF 1.5.9+) is the
    community alternative.
  - 1.30.164200 [reported: BI 1.30.164200 notes]: "Global function 'FindFile', now searches through the requested file
    system and normalises any path given to the internal engine delimiter '/' (DZEXP-134)". Not re-measured here.
  - `[DESIGN]` Keep enumeration behind a runtime probe: write a known sentinel file through the placeholder, enumerate,
    and use the result only if the sentinel is listed; otherwise fall back to the relative-path resolver or the index.
    The probe switches the workaround off by itself on a build where BI's fix holds. Re-test every 164078 workaround on
    164200 before deleting it; never delete or overwrite data on the strength of an empty listing. BI says the pattern you pass is
    normalised to `/`: build patterns with `/`, and do not assume either separator in what comes back (unmeasured).
- `CfgMods` script module paths with backslashes (`files[] = {"MyMod\scripts\4_World"};`) load no scripts and print no error
  [reported: bzed, Linux]; also measured on 1.30.164078 on this machine's Windows server (2026-10-16: the folder's 25 `.c`
  files were missing from `Module: Game; loaded … files` until the path used `/`). Use forward slashes in every path a mod
  builds.
- `$storage:` is not usable before the first `MissionServer.OnUpdate` / `CallLater` tick after `OnMissionStart` returns, and not
  at all on the very first boot of an instance [reported: bzed].
- `g_Game.GetMissionFolderPath()` returns `""` [reported: bzed]. **Script side verified here:** `DayZGame.SetMissionPath` ("Called from C++",
  `3_game\dayzgame.c:1206-1221`) splits only on `"\\"` (`:1217`), so any path without a backslash leaves `pos_end = 0` and
  `m_MissionFolderPath = path.Substring(0, 0)` = `""` (`:1220`); `GetMissionFolderPath()` returns that field (`:1228-1231`). The
  code is identical on 1.29 (`O:\…\3_game\dayzgame.c:1190-1204`), so the 1.30 symptom comes from the engine passing a
  forward-slash mission path (bzed: `mpmissions/<mission>/mission.c`). Derive the folder from `g_Game.GetMissionPath()`
  (`:1223`) by splitting on both `/` and `\`. Vanilla's own consumers of the empty value: `pluginmissionconfig.c:35`,
  `pluginconfigscene.c:40`, `5_mission\gui\scriptedwindows\missionloader.c:22`.

---

## What breaks for a 1.29 mod

1. `override OnCEUpdate` — compiles (obsolete), never runs → `OnCEIterate`. `override ProcessVariables()` → compile error, add `float elapsedTime`.
2. `modded DayZPlayerInventory.ProcessInputData` — not called in retail → Validate / Execute*.
3. `TakeEntityToCargo*` / `TakeEntityAsAttachment*` (any prefix) — obsolete → `*ToTarget*`.
4. `ActionFillBottleBase` — obsolete and **absent** from `RegisterActions` → `ActionObtainLiquidBase`.
5. `ItemBase.m_ItemActionOverrides` — compile error → `EntityAI.OverrideActionAnimation`.
6. `World.Is3rdPersonDisabled()` — obsolete; `VEHICLES_ONLY` is not a bool.
7. Hardcoded `UA_AM_*` ints (`UA_AM_INIT = 14` shifted later values) or raw `ERPCs` ints (`RPC_SYMPTOM_PARAM_SYNC` shifted them).
8. Hand takes on surrendered players — `HandGuardIsNotSurrendered` rejects.
9. `GetNoiseReductionByWeather*()` — still compiles (obsolete); AI damping is native; no vanilla consumer left.
10. Recipe replace no longer copies ingredient health. Drink/fill CCT is `CCTLiquid`, not `CCTWaterSurfaceEx`.
11. `override void Update()` on `ActionTargetsCursor` — compile error; 1.30 takes `Update(bool fullUpdate = true)`.
12. `override bool CanDisplayAttachmentSlot(string)` — never consulted by vanilla; override the int overload.
13. `FindFile` with placeholders (broken on 164014/164078, fixed per BI in 164200 — probe, §8.7), backslash `CfgMods` paths, early `$storage:`, `GetMissionFolderPath()` — section 8.7.

## Migration checklist

- [ ] `OnCEIterate(currentTime, elapsedTime)` + `super`; `ProcessVariables(float elapsedTime)`. Inventory: Validate + Execute* (not `ProcessInputData`).
- [ ] `*ToTarget*` takes. `AddAction(ActionObtainLiquidBase)`. `EntityAI.OverrideActionAnimation` (item and target).
- [ ] Info actions: `InitInfoData` + `CanBeStarted()==false`. `new CCTLiquid(mask, …)`. `GetPlayerHeadPosition`. Server-side checks of what the player aims at live in `ActionCondition` (§7.5).
- [ ] `GetThirdPersonViewMode() == ThirdPersonMode.ENABLED`. `CanDisplayAttachmentSlot(int)`. Never hardcode `UA_AM_*` / `ERPCs` / `NVTypes` / action ids.
- [ ] Cross product as `a * b` (not `vector.Cross`, removed in 164200) / `array.Slice` / `EnumFlagsToString`. Client catch-up: `IsFirstRenderFrame()`. VFX skip: `IsHeadlessOrDedicatedServer()`.
- [ ] Recipes with `m_ResultReplacesIngredient`: set `m_ResultInheritsHealth`. Bind `UAToggleVehicleLights` / `UAUICodeLock*`.
- [ ] Storage `144` (gate with `>=`). File I/O per 8.7.
- [ ] Workbench API only inside `#ifdef WORKBENCH`; no own `HasCrew()` / un-`override` `IsMoving()` on a Transport subclass (§9).

---

## 9. Patch 1.30.164200: what broke a 1.30.164078 mod, and designing mods that survive patches

Full delta and scanner (`scan_mod_130.py --from 1.30.164078`): skill `dayz-130-migration`,
`references/api-delta-1.30.164078-to-164200.md`. Compile breakers for Enforce code:

- `vector.Cross` removed → `a * b` (§8.3).
- `proto native bool HasCrew()` added to `Transport` (`3_game\vehicles\transport.c:175`). A mod `bool HasCrew()` on any
  Transport subclass (CarScript, BoatScript, MotorbikeScript, helicopter mods, a `modded` class of them) now collides with a
  native method; rename it. `bool IsMoving()` added to `Transport` (`:819`, logs `Error("IsMoving() not implemented")` and
  returns false); `CarScript.IsMoving()` is now `override` (`4_world\entities\vehicles\carscript.c:2221`); MotorbikeScript and
  BoatScript have no implementation. Details: skill `dayz-vehicles`.
- **Workbench script API is Workbench-only.** In 164078 `1_core\workbenchapi.c` declared `Workbench`, `WBModuleDef`,
  `ScriptEditor`, `ResourceBrowser`, `WorldEditor`, `WorldEditorAPI`, `WorldEditorTool`, `ButtonAttribute`,
  `WorkbenchPluginAttribute`, `WorkbenchToolAttribute` with no guard. In 164200 everything except `ResourceName`
  (`workbenchapi.c:1-4`) and `WorkbenchPlugin` (`1_core\workbenchapi\plugins\workbenchplugin.c:2`) sits inside
  `#ifdef WORKBENCH` (`workbenchapi.c:6-93` and every `1_core\workbenchapi\**\*.c`); `ResourceBrowser`, `WorldEditor`,
  `WorldEditorAPI`, `WorldEditorTool` are gone; `Workbench.GetModule/OpenModule/CloseModule` take a `typename`. Game and
  server builds do not define `WORKBENCH`, so any use outside the guard (including `[WorkbenchPluginAttribute(…)]` /
  `[ButtonAttribute(…)]` attributes on a class) fails with `Can't find variable 'Workbench'` → `Can't compile "Mission"
  script module!` [reported: Dabs Framework PR #50, `samplemvc.c` line 33].

### 9.1 Designing mods that survive patches

Rules 1-7 each come from a breakage that 1.30 or its first patch (164200) actually caused; rule 8 is the version policy.

1. **Prefix your own method names on engine and vanilla subclasses.** 164200 added `Transport.HasCrew()` as a
   `proto native` and `Transport.IsMoving()` as a script method; a mod that already had either name on a vehicle class broke
   or silently shadowed vanilla. `MyMod_HasCrew()` cannot collide with a future vanilla name. The same holds for fields
   (`m_MaximalActionDistance` appeared on three CCT classes in 164200).
2. **Never persist or send vanilla enum values as raw ints.** `NVTypes` shifted twice (1.30 inserted underground/sandstorm
   variants; 164200 inserted `NV_OPTICS_UNDERGROUND_DAY` and `NV_OPTICS_UNDERGROUND_SANDSTORM_DAY`, moving every later value by
   +1/+2, `4_world\entities\manbase\dayzplayer\dayzplayercamera_base.c:54-89`), as did `ERPCs` and `UA_AM_*` in 1.30 (§8.6,
   §7.2) and action ids (§7.10). Store the enum name (`typename.EnumToString`) or your own stable mapping, and write a
   version in every stream.
3. **Wrap any Workbench API use in `#ifdef WORKBENCH`**, including plugin classes and their attributes, and keep editor
   tooling in a separate file or PBO from game code.
4. **Do not rely on server-side cursor validation.** Since 164200 the server's `CCTCursor*` checks only origin distance
   (§7.5). Re-check line of sight, the aimed selection, ownership and lock state in the action's server-side
   `ActionCondition` (or `OnStartServer`), with server data only.
5. **Keep engine-bug workarounds behind a runtime probe** so they switch themselves off when BI fixes the bug. `FindFile`
   (§8.7) was broken on 164078 and is fixed per BI in 164200; a hardcoded workaround keeps using the slow path or, worse,
   misreads the fixed behaviour. Log which path the probe chose.
6. **Always call `super` in item location and attachment hooks** (`CanReleaseAttachment`, `CanReceiveAttachment`,
   `CanPutInCargo`, `EEItemLocationChanged`, `EEItemAttached` …) and combine its result with yours. Vanilla keeps adding rules
   there: 164200 added `MotorbikeScript.CanReleaseAttachment` (no detaching while the engine runs or crew is present,
   `4_world\entities\vehicles\motorbikescript.c:612-626`) and `EEItemLocationChanged` on compass and GPS. An override that
   returns `true` without `super` silently removes the new vanilla rule.
7. **Do not depend on vanilla script helpers that are new in the current build** unless the mod targets only that build:
   `vector.Cross` lived exactly one build. Prefer operators and long-standing API, or a prefixed local helper.
8. **Gate on behaviour, not on the build.** No `#ifdef DAYZ_1_30` (rule 1 of `../../_shared/dayz-version-policy.md`); for
   engine behaviour use the probe of rule 5, for script API the compile itself.
