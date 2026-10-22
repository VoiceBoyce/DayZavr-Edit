# Fence gate — the worked example

First authored 2026-07-07 (F4) from vanilla 1.29 scripts; **re-verified 2026-10-15 against 1.30.164078 stable** —
all line numbers are `P:\scripts` (1.30) unless written `1.29:`.

`Fence: BaseBuildingBase` (`4_world\entities\itembase\basebuildingbase\fence.c`, 947 lines) is the richest concrete
entity: it adds a gate state machine, an open/close animation, a combination lock **or** a digital code lock,
barbed-wire area damage, and a persisted gate field. Use it as the
template when adding any stateful sub-behaviour on top of the base part system. Roots: `P:\scripts\` =
`<dayz-projects>\scripts\`.

## 1. Gate state

The gate has its own state field `m_GateState` plus `m_IsOpened`, separate from the part bitmask. A part
declares itself a gate with `is_gate=1` in the `Construction{}` block (read once per type in
`ConstructionPartTypeData.InitBasicStrings`, `typeconstructiondata\constructionparttyped.c:85`). The gate's
open/close animation and locks are driven off `m_IsOpened` / the attached lock, and the barbed-wire area damage
is rotated with the gate. `CanOpenFence()` requires hinges, closed and `!IsLocked()` (`fence.c:472-478`), where
`IsLocked()` covers both lock types (`:127-141`); `OpenFence()` itself does not check the lock (`:488-528`).

The open/close actions are `interact\actionopenfence.c` and `interact\actionclosefence.c`. Opening/closing
plays the gate animation and toggles `m_IsOpened`.

Since 1.30.164200 the Fence config also moves the navmesh with the gate leaf:
`class Navmesh { class TransformParts { class Gate { transformAnimationSource="Wall_Gate_Rotate"; }; }; };`
(`P:\DZ\gear\camping\config.cpp:4418-4427`). No script reads it (engine-side). A mod gate that inherits `Fence`
inherits the block; if its model has no `Wall_Gate_Rotate` AnimationSource, re-declare or override the block
(untested either way). Every later line of the camping config moved +10 with this block.

## 2. Barbed-wire area damage

`CreateAreaDamage` (`basebuildingbase.c:981-1026`) builds an `AreaDamageLoopedDeferred_NoVehicle` from the
`<slot>_min` / `<slot>_max` memory points, ammo `"BarbedWireHit"` (`:1021`), hitting Torso / hands / legs / feet.
Fence overrides it to use a 100° angle when the gate is open (`fence.c:604-612`) and re-creates the mounted wire
volumes on open/close (`UpdateBarbedWireAreaDamagePos`, `:615+`; rotation math `basebuildingbase.c:1028+`) so the
damage volume follows the gate leaf as it swings.

Barbed wire is a `mountables[]` attachment (proxy physics toggled — see `config-contract.md §2`); its damage
zone also unmounts the wire when destroyed.

## 3. Gate persistence and the version bump

The gate state is persisted AFTER the base three sync ints, following the strict `OnStoreSave`/`OnStoreLoad`
ordering invariant (this is the canonical example of the subclass-after-super rule in `persistence-audit.md`):

- `Fence.OnStoreSave` (`fence.c:246-254`): calls `super.OnStoreSave` FIRST (which writes `m_SyncParts01/02/03`
  then `m_HasBase`), THEN writes `m_GateState`, `m_IsOpened`.
- `Fence.OnStoreLoad` (`fence.c:256-289`): calls `super.OnStoreLoad` FIRST, then handles a legacy branch — for
  `version < 110` it reads and DISCARDS a bool, then reads the gate state + opened for current versions.

**Version note:** Fence gate persistence was bumped at **v110**. The base damage-system version is
`GetDamageSystemVersionChange()→111` (`basebuildingbase.c:1133-1136`). When you add a persisted field to a
subclass, you bump the version and add a legacy branch exactly like this — read the old layout for old saves,
the new layout for new ones, so existing bases don't corrupt on the first load after the update. Getting this
wrong loses every fence on the server → treat as data-critical (R9, `persistence-audit.md`).

## 4. Repair

`Fence.CanBeRepairedToPristine()→true` (`fence.c:466-469`). Repair scales the material requirement by
`REPAIR_MATERIAL_PERCENTAGE=0.15` (`constructionbase.c:34`, applied in `HasMaterialsEx` `:1144-1148` and
`TakeMaterialsServer` `:1206-1209`), floor min 1; materials flagged `skipOnRepair` are not consumed (`:1190-1191`).
The repair action itself is the generic `ActionRepairPart` / kit path (not in `fence.c`).

## 5. What to copy when authoring your own stateful entity

1. Add the state field(s) and register them for netsync in the ctor, alongside the base `m_SyncParts*`.
2. Persist them in `OnStoreSave`/`OnStoreLoad` AFTER `super`, in the exact same order both ways.
3. Bump the version and add a legacy read branch for old saves.
4. If the behaviour has a physical volume (area damage, a moving leaf), rotate/relocate it in the same
   handler that mutates the state (`fence.c:604-612,615+` pattern).
5. Gate the whole thing behind server authority — the state mutation runs server-side and syncs to clients.

## 6. 1.30 — `DigitalCodeLock` and combo-lock unlock

Fence also accepts a `DigitalCodeLock` on `Att_CodeLock`; dialling a combination lock no longer unlocks it.
Before 1.30: the only Fence lock was `CombinationLock` on `Att_CombinationLock`, and finishing the dial
unlocked/detached it (1.29: `actiondialcombinationlockontarget.c:66-79`).

```c
// [EXACT] 4_world/entities/itembase/basebuildingbase/fence.c:19-26
	typename ATTACHMENT_COMBINATION_LOCK	= CombinationLock;
	typename ATTACHMENT_CODE_LOCK			= DigitalCodeLock;
	
	const string ATTACHMENT_SLOT_CAMONET			= "Wall_Camonet";
	const string ATTACHMENT_SLOT_BARBEDWIRE_DOWN	= "Wall_Barbedwire_1";
	const string ATTACHMENT_SLOT_BARBEDWIRE_UP		= "Wall_Barbedwire_2";
	const string ATTACHMENT_SLOT_COMBINATION_LOCK 	= "Att_CombinationLock";
	const string ATTACHMENT_SLOT_CODE_LOCK 			= "Att_CodeLock";
```

`GetCodeLock()` finds the slot (`fence.c:161-165`); `GetCombinationLock()` is `:155-159`. The old members
`ATTSLOT_CAMONET` / `ATTSLOT_BARBEDWIRE_DOWN` / `ATTSLOT_BARBEDWIRE_UP` (1.29: `fence.c:22-24`) are gone; use the
`ATTACHMENT_SLOT_*` constants above. Only one lock type at a time: the inventory hides the other lock slot
(`CanDisplayAttachmentSlot`, `fence.c:200-226`) and `CanReceiveAttachment` refuses a second lock or an open gate
(`:438-445`). `IsLocked()` — and therefore `CanOpenFence()` — is true for a locked combination lock **or** a locked
code lock (`:127-141,472-478`). When the `wall_gate` part is dismantled or destroyed, both locks are dropped by its
`essential_attachments[]` (`P:\DZ\gear\camping\config.cpp:4913-4917`; `DropEssentialAttachments`,
`constructionbase.c:1376`); when `wall_base_down` / `wall_base_up` is destroyed, `Fence.OnPartDestroyedServer`
drops them too (`fence.c:391-403`). Note: the branch at `fence.c:378-390` that also drops barbed
wire / camonet and closes the gate needs `destroyed_by_connected_part == true`, which vanilla 1.30 never passes
(see `dayz-1-30-construction-rebuilding.md` §3.1; static reading, not measured).

`CombinationLock.SetActions` now includes `ActionCombinationLockUnlock` (`combinationlock.c:686`). Dialling
the right combination is not enough: the player must use the unlock ("detach") action, which checks the side they
face (`actioncombinationlockunlock.c:57-82`). The lock has an inside and an outside dial; the API changes
(`SetCombination(int, bool)`, `DialNextNumber(int)`, `UnlockOnServer`, …) are tabled in
`dayz-1-30-construction-rebuilding.md` §6.1. Persist: `OnStoreSave` writes `m_Combination`, `m_CombinationLocked`,
then `m_CombinationInside`; `OnStoreLoad` reads the third int only when `version >= 143`
(`combinationlock.c:128-136,169-177`). Fence gate fields themselves still follow the v110 branch
(`fence.c:256-289`).

The code lock (`DigitalCodeLock`, `4_world\entities\itembase\codelock.c:1`) persists through its
`CodeLockComponent` (`m_LockPIN`, `m_IsLocked`, `m_DoorIndex`, `4_world\classes\codelockcomponent.c:257-262`); its
brute-force protection is tuned in `cfggameplay.json` `ExternalLockData` (`3_game\cfggameplaydatajson.c:421-438`).
Building-door lock slots on `BuildingBase` (`4_world\entities\game\super\building.c:10-11`) are documented in
`dayz-doors`. This file only covers the Fence attachment. On a Fence the lock's back part is posed at animation
phase `0.92` (`CodeLockVisualManager`, `4_world\classes\codelockvisualmanager.c:48,459-462`); 1.30.164200 turned
these numbers into `private const` without changing the Fence value (the `House` value changed, see `dayz-doors`).

`Fence.OnStoreSave` (unchanged from 1.29 except its line numbers):

```c
// [EXACT] 4_world/entities/itembase/basebuildingbase/fence.c:246-254
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
		
		//write
		ctx.Write( m_GateState );
		ctx.Write( m_IsOpened );
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] OnStoreSave - build=" + m_GateState + " opened=" + m_IsOpened);
	}
```
