---
name: dayz-basebuilding
description: >
  Author, extend and debug DayZ 1.30 buildable structures on BaseBuildingBase:
  fences, watchtowers, gates, walls, shelters, flag poles and barbed wire.
  Covers kit/deploy/hologram flow, Construction{} fields, the ConstructionBase/
  Construction/Rebuilding component, ConstructionPart(TypeData), build/
  dismantle/destroy actions, synced-bitmask persistence, damage-zone mapping,
  the Fence gate and its combination/code locks, bricks/mortar. Use for "base
  won't save", parts reset after restart, part or action missing, CanBuildPart,
  RegisterPartForSync, blocked dismantle, missing collision or hologram
  placement, porting a ≤1.29 base mod (BuildPartServerEx; hooks that
  compile but are no longer called). Invoke before authoring/debugging a
  buildable structure or its persistence. Delegate geometry to
  dayz-model-pipeline, script APIs to enforce-script-reference, persistence
  audit to rigorous-data-audit, packaging to dayz-pbo-build. Not terrain
  holes: dayz-underground.
---

# DayZ Base Building

Buildable player structures in DayZ — fences, watchtowers, gates, shelters, flag poles — are built by
**extending the vanilla `BaseBuildingBase` chain**, not from `EntityAI`. Inheriting `BaseBuildingBase`
gives you the whole system for free: the per-part build/dismantle/destroy action set, the synced-bitmask
persistence, the hologram deploy flow, and the damage-driven part destruction. The only things you author
are a `.p3d` with the right selections + memory points and a config `Construction{}` block that declares
each buildable part.

> Target: DayZ 1.30 stable (1.30.164078). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

This skill owns the base-building-specific layer. Generic geometry, config-script mechanics, packaging and
data-critical audit are delegated to the skills in the table below.

All `path:line` citations point at vanilla 1.30 source under `P:\scripts\` (= `<dayz-projects>\scripts\`) and
`P:\DZ\gear\camping\config.cpp` unless written `1.29: …`. Verify against those before writing any class name
or field. On 1.30 the system is a construction **component** any `EntityAI` can own: `ConstructionBasic`
(`P:\scripts\3_game\systems\construction_basic.c:2`) → `ConstructionBase`
(`P:\scripts\4_world\classes\constructionbase.c:30`) → `Construction` for player structures
(`4_world\classes\basebuilding\construction.c:2`) or `Rebuilding` for map buildings
(`4_world\classes\rebuilding\rebuilding.c:1`). Static part config is cached once per type in
`ConstructionPartTypeData` (`4_world\classes\basebuilding\typeconstructiondata\constructionparttyped.c:2`),
held by `EntityAIType` (`3_game\entities\entityaitype.c:4,66-85`). Full map:
`references/dayz-1-30-construction-rebuilding.md`.
Before 1.30: `Construction` was a standalone class in `construction.c` that re-read the config on every
instance (1.29: `O:\…\scripts\4_world\classes\basebuilding\construction.c:11,235-270`).

## THE MODEL — kit → deploy → build → upgrade → dismantle

The classes and who owns what:

| Class | Role | Where |
|---|---|---|
| `BaseBuildingBase: ItemBase` | the persisted world entity; owns the construction component, the sync ints, events, physics, area damage | `4_world\entities\itembase\basebuildingbase.c:2` |
| `ConstructionBasic` | 3_Game stub so `EntityAI` can hold a component: `Init`, `OnHealthLevelChanged`, `HandleStoreSave/Load`, `ProcessConstructionDecay` | `3_game\systems\construction_basic.c:2-44` |
| `ConstructionBase : ConstructionBasic` | shared 4_World controller: part map, `CanBuildPart(Ex)`, `BuildPartServerEx` / `DismantlePartServerEx` / `DestroyPartServer`, `IsCollidingEx`, materials, decay | `4_world\classes\constructionbase.c:30,179,190,209,1481` |
| `Construction : ConstructionBase` | the `BaseBuildingBase` flavour: forwards to `OnPartBuiltServer` / `OnPartDismantledServer` / `OnPartDestroyedServer` on the parent; gate exception | `4_world\classes\basebuilding\construction.c:2-162` |
| `Rebuilding : ConstructionBase` | map-building restoration on `BuildingBase`; ten `m_SyncParts*` ints, own versioned save | `4_world\classes\rebuilding\rebuilding.c:1-16`; `4_world\entities\game\super\building.c:23-26` |
| `ConstructionPartTypeData` | type-level cache of one part's `Construction{}` fields (name, id, flags, masks, materials) | `4_world\classes\basebuilding\typeconstructiondata\constructionparttyped.c:2,45-71` |
| `ConstructionPart` | instance state of one part: `m_PartTypeData`, built flag, local sync bits; getters forward to the type data | `4_world\classes\basebuilding\constructionpart.c:3-16,193-221` |
| `ConstructionActionData` | per-player scratch state on `PlayerBase`; caches which parts are buildable under the cursor, drives radial action variants | `4_world\classes\basebuilding\constructionactiondata.c:1,163-203` |

`Construction` is NOT itself persisted — it is rebuilt from the (type-cached) config on every load and its part states are
restored from the sync bitmask (see PERSISTENCE). On 1.30 the entity creates it through the `EntityAI`
component hooks: `EntityAI` calls `ConstructionInit()` when `cfgVehicles <type> Construction` exists
(`3_game\entities\entityai.c:248-249`; `ConstructionInit` `:3464-3472`), which calls
`CreateConstructionComponent()` (`:3474-3478`, returns null by default). `BaseBuildingBase` overrides it to
`m_Construction = new Construction(this)` (`basebuildingbase.c:872-876`); the field is
`protected ref ConstructionBasic m_Construction` (`:15`); `GetConstructionBasic()` returns it (`:878-881`) and
`GetConstruction()` is `Construction.Cast(m_Construction)` (`:883-886`). `ConstructionBase.UpdateConstructionParts`
spawns one `ConstructionPart` per cached type entry and calls `SetPartTypeData` (`constructionbase.c:516-546`;
`constructionpart.c:395-401`).
Before 1.30: `BaseBuildingBase` owned `ref Construction m_Construction` and a public `ConstructionInit()` did
`new Construction(this)` + `Init()` (1.29: `basebuildingbase.c:965-973`); `ConstructionPart` stored its config
fields in public members filled by its constructor. On 1.30 those members still exist but are never written
(see "What breaks").

End-to-end flow:

1. **Craft the kit.** A `RecipeBase` (`4_world\classes\recipes\recipes\craftfencekit.c:1`) consumes materials →
   a `KitBase: ItemBase` (`4_world\entities\itembase\kitbase.c:1`, `IsBasebuildingKit()→true` `:5`).
2. **Deploy (hologram).** Kit `SetActions()` adds `ActionTogglePlaceObject` + `ActionDeployObject`
   (`kitbase.c:146-152`); `ActionDeployObject.ActionUsesHologram()→true` (`actiondeployobject.c:15`). On
   finish the SERVER spawns the real entity — `FenceKit.OnPlacementComplete` (`fencekit.c:19-34`)
   `CreateObjectEx("Fence", …)` + `HideAllSelections()` — and the kit self-deletes on `OnEndServer`
   (`actiondeployobject.c:205-234`). These four files are byte-identical to 1.29 apart from line endings.
3. **Base part.** The spawned entity has NO parts built. Player attaches the base material then builds the
   `base` part (`is_base=1`). Building it sets `HasBase()=true`, spawns a construction kit back, and toggles
   the `"Deployed"` proxy/animation (`OnPartBuiltServer` `basebuildingbase.c:549-579`, base branch `:553-560`;
   `InitVisuals` `:677-691`).
4. **Build part.** `ActionBuildPart` (`4_world\classes\useractionscomponent\actions\continuous\actionbuildpart.c:33`)
   with a tool in hand → `OnFinishProgressServer` (`:150-204`) re-checks `CanBuildPart` + `IsCollidingEx` →
   `construction.BuildPartServerEx(player, partName, AT_BUILD_PART)` (`:170`). `ConstructionBase.BuildPartServerEx`
   takes the materials (`constructionbase.c:179-184`); `Construction.BuildPartServerEx` then resets the
   damage-zone health and calls `BaseBuildingBase.OnPartBuiltServer` (`construction.c:52-74`), which registers the
   part in the sync bitmask, syncs, shows physics+visual and regenerates the navmesh. The tool loses health only
   if `part.DamageToolOnBuild()` (always true for plain parts, `constructionparttyped.c:453-456`).
   Before 1.30: the action called `BuildPartServer(player, part_name, action_id)`. On 1.30 that method is an
   `[Obsolete("call BuildPartServerEx instead")]` forwarder (`constructionbase.c:1702-1708`) and **no vanilla
   code calls it** — an override of `BuildPartServer` compiles and never runs. Override `BuildPartServerEx`.
5. **Upgrade** = building further parts whose `required_parts[]` are satisfied (`HasRequiredPart`
   `constructionbase.c:682-696`), whose `conflicted_parts[]` are not built (`HasConflictPart` `:704-735`) and
   whose structural support is met (`PartHasAnySupportFulfilled` `:698`, always true unless the part declares
   `StaticsSupportData`).
6. **Dismantle.** `ActionDismantlePart` (`actiondismantlepart.c:23`) → `DismantlePartServerEx` (`:107`):
   refund materials (only when `player` is not null), drop non-usable materials, drop the part's
   `essential_attachments[]`, let items on the part fall (`constructionbase.c:190-206`), then
   `OnPartDismantledServer` (`construction.c:77-90`). Blocked if the part `HasDependentPart`
   (`constructionbase.c:772-783`). Dismantling the **base** part destroys the whole construction
   (`basebuildingbase.c:614-619`).
   Before 1.30: `DismantlePartServer`; on 1.30 it is an obsolete forwarder (`constructionbase.c:1710-1716`) with no
   vanilla caller.
7. **Fold.** With no base and no attachments (`CanFoldBaseBuildingObject` `basebuildingbase.c:962-970`),
   `ActionFoldBaseBuildingObject` converts back to a kit in hands (`FoldBaseBuildingObject` `:972-978`).

## THE Construction{} CONFIG BLOCK (the single most-reused artifact)

Two-level nesting: `Construction { class <main_part> { class <part> {...} } }`. Annotated from the Fence
block (`P:\DZ\gear\camping\config.cpp:4529-4587`). Full field-by-field in `references/config-contract.md`.

```cpp
class Construction {
  class wall {                          // main_part_name (m_MainPartName)
    class base {                        // part_name — config class == m_PartName AND the p3d selection
      name="$STR_..._Part_Base";        // localized display name
      is_base=1;                        // foundation part; dismantling it destroys the whole entity
      id=1;                             // UNIQUE sync/persistence bit index, 1..93 (see PERSISTENCE)
      required_parts[]={};              // parts that must be built first
      conflicted_parts[]={};            // parts that block this one if already built
      collision_data[]={};              // {min_memorypoint, max_memorypoint} for the build collision box
      build_action_type=4;             // bitmask AND-matched against the tool's build_action_type
      dismantle_action_type=4;
      material_type=1;                  // ConstructionMaterialType — drives build/dismantle SFX
      class Materials {
        class Material1 {
          type="WoodenLog";             // item classname (used when refunding piles)
          slot_name="Material_WoodenLogs"; // attachment slot the material sits in
          quantity=2;                   // consumed; -1 = delete whole object; 0 = ignore qty
          lockable=1;                   // 1 = lock material in slot instead of consuming it
        };
      };
    };
  }
}
```

Every field is read once per type by `ConstructionPartTypeData.InitPartTypeData`
(`constructionparttyped.c:45-71`) — readers cited in `references/config-contract.md`. 1.30 adds optional
per-part keys (`essential_attachments[]`, `item_falling_data[]`, `can_part_decay`, `custom_part_type`,
`StaticsSupportData`, …; vanilla Fence/Watchtower now use the first two — `config.cpp:4903-4907`). 1.30 no
longer reads `show_on_init` or `platform_support` (no reader in `P:\scripts`; 1.29 read them at
`construction.c:259,792`). The entity class itself declares `attachments[]` (material/attachment slots),
`hybridAttachments[]` (health mirrors a damage zone), `mountables[]` (proxy-physics toggled, e.g. barbed wire),
and a `DamageSystem { class DamageZones }`.

## PREFLIGHT — the hard invariants

Gate real build work on `/dayz-preflight` (P:\ mounted, AddonBuilder, P:\Mods junction) per
`_shared/dayz-conventions.md`. Authoring config offline does not need it; packing does. Before declaring a
base-building entity done, verify ALL of these — each is a silent-corruption source:

- **`id` MUST be unique and in 1..93** across the whole `Construction{}` block. The id is a bit index into
  three 31-bit sync ints; a duplicate or out-of-range id collides in the bitmask and corrupts part state
  (`RegisterPartForSync` `basebuildingbase.c:137-164`). This caps a single `BaseBuildingBase` at 93 distinct
  parts; packing and `OnStoreSave` (`:420-430`) are unchanged from 1.29. `Rebuilding` on `BuildingBase` is a
  different stream (ten ints, 2 bits per part, ids 1..160, `REBUILDING_STORAGE_VERSION = 1`,
  `rebuilding.c:3,7-16,501-514`). Do not mix the two caps.
- **Damage-zone name MUST equal the part name, lowercased.** On ruin, `EntityAI.EEHealthLevelChanged` forwards
  to the component (`3_game\entities\entityai.c:1061-1062,1094-1099`) and `ConstructionBase.OnHealthLevelChanged`
  lowercases the zone, looks up the part by that name and calls `DestroyPartServer(null, part, AT_DESTROY_PART)`
  (`constructionbase.c:352-367`); a mismatch means damage never destroys the part.
  Before 1.30: the same lookup lived in `BaseBuildingBase.EEHealthLevelChanged` (1.29: `basebuildingbase.c:496-528`);
  on 1.30 that override only calls `super` (`basebuildingbase.c:497-503`). A mod that skips `super` there
  still blocks part destruction. A mod that copied the 1.29 body into its override now repeats work the
  component already did (the part is usually no longer built by then — static reading, not measured); delete
  the copy and hook `Construction.OnHealthLevelChanged` / `DestroyPartServer` instead.
- **`Fence.OpenFence()` does not consult the locks.** `CanOpenFence()` requires `HasHinges() && !IsOpened() && !IsLocked()` (`fence.c:472-478`). `IsLocked()` is true for a locked `CombinationLock` **or** a locked `DigitalCodeLock` (`fence.c:127-141`; before 1.30 only the combination lock). `OpenFence()` skips that check, plays the gate animations and calls `SetOpenedState(true)` (`fence.c:488-528`). A script that calls `OpenFence()` directly opens a locked fence. Test `!IsLocked()` or call `CanOpenFence()` first. `CanCloseFence()` does not require the lock to be open (`fence.c:480-486`).
- **`OnStoreSave`/`OnStoreLoad` order is strict.** Save writes `m_SyncParts01`, `02`, `03`, then `m_HasBase`
  (`basebuildingbase.c:420-430`); load reads them in the same order (`:432-464`). A subclass writes/reads its
  extra fields AFTER calling `super`, in the exact same order both ways (Fence appends gate state after the
  three ints — `fence.c:246-254` / `:256-289`). Reordering silently corrupts every saved base.
  `EntityAI.OnStoreSave` also calls `HandleStoreSave` on the component (`entityai.c:2912-2914`), but only
  `Rebuilding` writes anything there (`rebuilding.c:468-481`), so the `BaseBuildingBase` stream is unchanged.
- **Every `part_name` MUST be both an AnimationSource in config AND a selection in the `.p3d`.** Show/hide is
  `SetAnimationPhase(part_name, 0/1)` (`constructionpart.c:412-420,474-483`, phase 0 = visible) and collision is
  `AddProxyPhysics(part_name)` (`:422-430`); the base uses the `"Deployed"` proxy instead. A missing
  selection/memory point → the part builds but has no visual or no collision.
- **Action-type ints** (`4_world\classes\useractionscomponent\_constants.c:6-8`): `AT_BUILD_PART=193`,
  `AT_DISMANTLE_PART=195`, `AT_DESTROY_PART=209`. Passed into `BuildPartServerEx`/`DismantlePartServerEx`/
  `DestroyPartServer` and synced so clients play the right SFX. Tool masks are now the enum
  `EConstructionTools`, including `TOOL_HANDS = 128` (a part with this bit is built with empty hands by
  `ActionBuildPartHandsGeneric`) and `TOOL_BRICKLAYING = 256` (`constructionconstants.c:14-26`;
  `constructionbase.c:1404-1441`).
- **How much of a material fits in one slot is decided by the SLOT's `stackMax`, NOT by the item's
  `varQuantityMax`.** For a splitable item, `GetTargetQuantityMax` asks `InventorySlots.GetStackMaxForSlotId`
  first and only falls back to `varStackMax`, then to `varQuantityMax` (`4_world\entities\itembase.c:3449-3469`;
  the values now live on `ItemBaseType`, `itembasetype.c`). Vanilla declares the cap per slot in
  `P:\scripts\config.cpp`: `Material_WoodenPlanks` and `Material_MetalSheets` are `stackMax=20`
  (`:2285-2300`), `Material_Nails` is `99` (`:2277-2284`). `MetalWire` still ends at one wire per slot, by a
  different route on 1.30: it is now splitable with `varStackMax=1`, `varQuantityMax=50`
  (`P:\DZ\gear\crafting\config.cpp:369-394`), and neither `Material_MetalWire` nor `Material_FPole_MetalWire`
  declares `stackMax` (`scripts\config.cpp:2309-2315,2508-2514`), so the fallback lands on `varStackMax=1`.
  Before 1.30: `MetalWire` had no quantity at all (1.29: `DZ\gear\crafting\config.cpp:356-371`). Reading the
  item class instead overstates plank capacity 5× (100 vs 20) and makes any recipe above the slot cap silently unbuildable: the
  player fills the slot and never reaches the threshold, so the build action simply never appears. Derive
  slots per material as `ceil(default_qty / slot stackMax)` and get that number from `CfgSlots`, not from the
  item. Any design promising a uniform configurable range ("1..100 of each material") is broken before it is
  written. Origin: caught by an independent plan review after the item-class reading had already been written
  into a spec — the item's ceiling exists, it just is not the one that governs.

- **The same fallback sets a crop's harvest yield: raising its `varStackMax` changes the vanilla yield** [EXACT][CLAIM-HARVEST-YIELD-STACKMAX-130]: `PlantBase.Harvest` creates each crop pile with `SetQuantity(GetQuantityMax())` (`4_world\entities\gardenbase\plantbase.c:574-575`, file unchanged from 1.29), and for a splitable item `GetQuantityMax` resolves to `varStackMax` when the slot declares no stack max (`4_world\entities\itembase.c:3435-3469`). `SetQuantity` clamps to `[min, max]` (`itembase.c:3352`), so a scripted yield above the stack size was silently clipped until the mod replaced the harvest quantity (measured on 1.30 Exp 164014; the code path is identical on 164078).

- **A green/red placement hologram needs TWO declarations, and neither is the one the name
  suggests.** `Hologram.RefreshVisual` tints by calling `SetObjectTexture`/`SetObjectMaterial` on a
  **hidden-selection index** (`hologram.c:1546-1565`). That index comes from `GetHiddenSelection`,
  which **falls back to 0 when the name is absent** (`:1523-1531`) — so a model with no
  `hiddenSelections[]` in config silently renders the ghost in its normal material, with no error
  anywhere. And the config entry alone is not enough: the model must expose that selection as
  retexturable via `sections[]` in `model.cfg`, or the swap has nothing to bind to. Declare
  `hiddenSelections[]` + `hiddenSelectionsTextures[]` + `hiddenSelectionsMaterials[]` with the values
  the model **already uses** (so normal rendering is unchanged), and put the same selection in
  `sections[]`. Note the hologram never looks the name up: it asks for `placing`/`inventory`
  (`:40-43,178-194`) and lands on index 0 regardless, so the name only maps index 0 to a real
  section. Also declare `hologramMaterial` + `hologramMaterialPath`; the engine appends
  `_deployable.rvmat` / `_undeployable.rvmat` (`:14-16`). Cost of learning this the hard way: one
  in-game cycle, plus a review finding that was raised, retired as a likely false positive, and
  turned out to be true.
- **`itemBehaviour` does NOT choose the in-hands carry pose.** The int drives behaviour rules and the
  deploy command (`4_world\entities\itembasetype.c:33,101-103`: `-1 = unset, 0 = heavy, 1 = onehanded,
  2 = twohanded`, read through `ItemBaseType` and used at `itembase.c:4326-4342`; before 1.30 it was the
  `ItemBase.m_ItemBehaviour` field, now removed), but the pose comes
  from **class registration in the anim graph**. Vanilla registers every heavy object explicitly —
  barrel at `4_world\entities\manbase\dayzplayer\dayzplayercfgbase.c:841`, wooden crate at `:854` — through
  `ModItemRegisterCallbacks.RegisterHeavy` (`:305,:317`) calling `AddItemInHandsProfileIK(class, …
  player_main_heavy.asi, pBehavior, … .anm)`. With no registration the item falls back to the
  `Inventory_Base` one-handed default and is carried in one hand no matter what `itemBehaviour` says.
  The `.anm` is an IK pose, not a class binding, so reusing a vanilla one for a different class is
  fine and needs no new animation. Pick the value from the object family you are imitating, not from
  a mod that ships a different shape: a cabinet is `0` like the barrel, not `2` like a small kit box. `SetActions` adds
  `ActionTogglePlaceObject` + `ActionPlaceObject` only in the deployable subclass (`container_base.c:32,48-54`),
  which is what `Barrel_ColorBase` inherits from (`barrel_colorbase.c:1`). A storage entity extending
  `Container_Base` gets no placement at all, and `IsDeployable()` is `false` by default (`itembase.c:4348-4351`).
  Adding the two actions to the single class that needs them is cheaper than reparenting a shared base and
  cannot regress its siblings. The ghost material comes from `hologramMaterial` + `hologramMaterialPath`
  (`hologram.c:1554-1557`) plus the suffixes `_deployable.rvmat` / `_undeployable.rvmat` (`:14-16`); declare
  both keys or the hologram renders untextured.
- **A custom wall-placement hologram (modded `Hologram.UpdateHologram`) needs four things vanilla does not
  hand you.** (1) The wall normal: neither `RaycastRVProxy.dir` nor `RaycastRV.contactDir` is a face normal for
  object hits — both are the ray direction, so a yaw taken from them faces the camera and the ghost's box
  collides with the wall at any angle. Take it from `DayZPhysics.RayCastBullet` on
  `PhxInteractionLayers.BUILDING` and keep the fire-geometry ray for identity (`enforce-script-reference`
  SP-LFS-2). (2) The ground: pick the nearest support at or BELOW the aimed point (Y ≤ aimY + 2 mm); a ray
  started 2 m above the point selects sills, ledges and the floor above. (3) The collision box: lift its
  bottom 5 cm like `IsCollidingBBox` (`hologram.c:547,556`) or every building floor blocks placement, and trim
  the back so the support wall does not count. (4) One diagnostic line per cause bit (wall/ground/spacing/
  player/water/box), emitted on change and rate-limited, each line under 255 chars, with the unlifted box
  queried only when logging — that is what turned "the hologram is grey" into a named cause in one cycle.
  Origin: LFSecure, 2026-09-09, three in-game cycles and two Codex rounds
  (`LFSecure_dev/plans/2026-09-09-colocacion-fix-diag.md`); (1) shipped but not yet exercised in-game.

## PERSISTENCE — one synced bitmask is BOTH netsync AND save (data-critical)

Part-built state is NOT stored per part; it is packed into three 31-bit ints `m_SyncParts01/02/03`
(`basebuildingbase.c:10-12`), which serve as both the netsync representation and the persistence format.
Part `id` 1..31 → SyncParts01, 32..62 → SyncParts02, 63..93 → SyncParts03. On load,
`SetPartsFromSyncData` rebuilds each part's built flag from the bitmask and reconciles physics/visuals
(`SetPartFromSyncData` `:265-304`, which also failsafe-relocks attached materials against corrupted data, `:303`).
This packing and the `OnStoreSave`/`OnStoreLoad` order are the same as on 1.29.

**R9 gate:** any mod that adds parts, changes the bitmask packing or the `id` ranges, or touches
`OnStoreSave`/`OnStoreLoad` ordering is modifying player progression — a bug means lost bases after a
restart. Delegate to `rigorous-data-audit` (R9) BEFORE declaring release-safe. Full detail, the 93-part
cap, the version bumps (`GetDamageSystemVersionChange()→111`, `basebuildingbase.c:1133-1136`; Fence gate
persistence bumped at v110) and the hand-off checklist are in `references/persistence-audit.md`.
`Rebuilding` is a second, versioned stream on map buildings, written through the component hook in
`EntityAI.OnStoreSave` and read only when the storage version is ≥ 143 (`entityai.c:2912-2914,2980-2984`) —
see `references/persistence-audit.md` §8. `CombinationLock` now also writes `m_CombinationInside` and reads it
only at stream **version ≥ 143** (`4_world\entities\itembase\combinationlock.c:128-136,169-176`) — a subclass
`OnStoreLoad` that skips that branch desyncs the item. The 1.30 storage version is 144
(`3_game\global\game.c:5`); a 1.30 storage cannot be loaded by a 1.29 server — back up `storage_1` first.

## QUICK TRIAGE

| Symptom | Likely cause | Where |
|---|---|---|
| **Base won't save / parts reset on server restart** | `OnStoreSave`/`OnStoreLoad` order broke, or a subclass wrote fields before `super` / in a different order | PERSISTENCE; `references/persistence-audit.md` |
| **A built part doesn't show (visual missing)** | `part_name` is not an AnimationSource in config or not a selection in the `.p3d` | PREFLIGHT show/hide; `dayz-animation-pipeline`, `dayz-model-pipeline` |
| **A built part has no collision** | missing per-part proxy memory/selection, or (base) missing `"Deployed"` proxy | `dayz-model-pipeline` (+ `dayz-p3d-audit`) |
| **Part state corrupts / two parts toggle together** | duplicate or out-of-range `id` in `Construction{}` (bitmask collision) | PREFLIGHT id≤93; `RegisterPartForSync` `basebuildingbase.c:137-164` |
| **Damage never destroys a part** | damage-zone name ≠ part name lowercased, or an override of `EEHealthLevelChanged` skips `super` | PREFLIGHT zone==part; `constructionbase.c:352-367`; `entityai.c:1094-1099` |
| **The build action never appears** | tool's `build_action_type` doesn't AND-match the part's, or `CanBuildPartEx` fails (constructed, missing requirement/support, conflict, missing/ruined material, tool) — the failure bits are `EConstructionInfoCategories` | `constructionbase.c:590-611,1404-1441`; `constructionconstants.c:1-11` |
| **Dismantle is blocked** | the part `HasDependentPart` (something built depends on it) or `SupportRemovalCheck` fails (Rebuilding only) | `constructionbase.c:739-741,772-789` |
| **Mod's `CanBuildPart(part, null, false)` returns false on 1.30** | the `use_tool` argument is ignored: 1.30 always runs the tool check, and a null tool passes only for `TOOL_HANDS` parts. Before 1.30: `use_tool=false` skipped it | `constructionbase.c:583-611,1404-1441`; 1.29: `construction.c:296-304` |
| **Placement / hologram won't confirm** | height check or collision fails on the hologram | `actiondeployobject.c:35-75`; `references/entity-lifecycle.md` |
| **Perf hitch when building** | `UpdatePhysics()` is a "massive performance hit" per the code's own warning | `basebuildingbase.c:745` — avoid frequent calls |
| **Override of `BuildPartServer` / `DismantlePartServer` / `IsColliding(string)` never runs** | they are `[Obsolete]` forwarders on `ConstructionBase` with **no vanilla caller**; actions call the `*Ex` / `IsCollidingEx` versions | `actionbuildpart.c:167-170`; `actiondismantlepart.c:107`; `constructionbase.c:1702-1754` |
| **`part.m_PartName` / `m_Id` / `m_IsBase` read as empty/0** | the public `ConstructionPart` fields are `[Obsolete]` and never written; use `GetPartName()`, `GetId()`, `IsBase()` … | `constructionpart.c:490-502,193-286` |
| **Custom build animation per tool ignored** | `SetBuildingAnimation(ItemBase)` is obsolete and not called; `SetupAction` calls `SetBuildingAnimationEx` | `actionbuildpart.c:227,241-267,370-374` |
| **Mod's `ConstructionActionData.GetSlotId()` returns a stale value** | vanilla no longer calls `SetSlotId`; the attach action keeps the slot in `AttachToConstructionActionData.m_SlotID` | `constructionactiondata.c:617-627`; `actionattachtoconstruction.c:1-4,38,68` |
| **Shelter / no-tool build action of a 1.29 mod never appears** | `ActionBuildShelter` is `[Obsolete]` and unregistered, `ActionActionBuildPartNoTool` is an empty unregistered subclass; empty-hand parts use `ActionBuildPartHandsGeneric` (`TOOL_HANDS`) | `actionbuildshelter.c:1`; `actionbuildpart.c:377,543`; `playerbase.c:1737` |
| **Compile warning on `CreateCollisionTrigger`** | methods still exist but are `[Obsolete("no replacement")]`; collision is `IsCollidingEx(CollisionCheckData)` | `constructionbase.c:1481,1756-1801` |
| **Items on a platform/roof drop when it is dismantled** | 1.30 makes items in a part's `item_falling_data[]` box fall (vanilla Fence platform, Watchtower roofs/stairs); `cfggameplay.json` `BaseBuildingData.ConstructionData.disableItemFalling` turns it off | `constructionbase.c:190-206,241-247`; `3_game\cfggameplaydatajson.c:279` |
| **Fence part decays on its own** | a part with `can_part_decay=1` (or `can_decay_general`) loses `part_decay_rate` HP/s (default −1, ×1.5 in a sandstorm) down to `part_decay_threshold` (default 0.3); `cfggameplay.json` `GeneralData.disableBaseDecay` stops it | `constructionbase.c:36-39,307-338`; `entityai.c:3909-3910`; `cfggameplaydatajson.c:60` |
| **Rebuilt house / well part resets** | mixed up `BaseBuildingBase` 3-int save with `Rebuilding` 10-int `HandleStoreSave` | `rebuilding.c:468-530`; `building.c:23-45` |
| **Fence code lock ignored** | slot is `Att_CodeLock`; getter is `Fence.GetCodeLock()` | `fence.c:20,26,161-165` |
| **Combination lock dialled right but stays on** | 1.30 dialling no longer unlocks; the player must use `ActionCombinationLockUnlock` ("detach") | `actiondialcombinationlockontarget.c:72-82`; `actioncombinationlockunlock.c:1-95`; `combinationlock.c:686` |
| **Powered opener opens a locked fence** | caller used `OpenFence()` and skipped `CanOpenFence()` / `IsLocked()` | `fence.c:127-141,472-528` |

Do NOT build new features on `ActionPlugIntoFence` — it is DEPRECATED (`actionplugintofence.c:1`).

## DELEGATIONS

| Concern | Skill |
|---|---|
| `.p3d` selections / memory points / the `Deployed` proxy / LODs / missing-collision or action-target bugs | `dayz-model-pipeline` (+ `dayz-p3d-audit`) |
| config.cpp + Enforce script — RPC, sync vars, `OnStoreSave/Load`, `modded class`, side checks | `enforce-script-reference` |
| Persistence / bitmask packing / recovery paths (data-critical, id ranges, save ordering) | `rigorous-data-audit` (R9) |
| Show/hide a part (`SetAnimationPhase`), gate open/close, AnimationSources | `dayz-animation-pipeline` |
| Build / deploy / launch to test; smoke it in-game | `dayz-pbo-build` + `dayz-test-ingame` (+ `dayz-mcp-verify`) |
| Terrain holes, underground triggers, `disallowedTypesInUnderground`, bunkers under the terrain | `dayz-underground` |

## CITE-THEN-VERIFY

Base-building class and field names are easy to half-remember. Before writing a class name, base, config
field or named selection, grep vanilla 1.30 (`P:\scripts\4_world\classes\constructionbase.c`,
`P:\scripts\4_world\classes\basebuilding\`, `P:\DZ\gear\camping\config.cpp`) or the cited reference. Most
1.29 line numbers moved in 1.30; a 1.29-era snippet that names `construction.c:<n>` for anything but the
`Construction` subclass is pointing at code that now lives in `constructionbase.c`. The `id`/damage-zone/selection
invariants above corrupt data silently when wrong — verify them against the real `.c` and `.p3d`, never from memory.

## REFERENCES

- `references/config-contract.md` — the `Construction{}` block, `Materials`, `GUIInventoryAttachmentsProps`,
  `attachments[]`/`hybridAttachments[]`/`mountables[]`, tool `build_action_type`, DamageZones↔part mapping,
  every field with its reader `path:line`.
- `references/entity-lifecycle.md` — the deploy→build→sync→persist→load call chain hop by hop with `path:line`
  (hologram, `OnPlacementComplete`, `OnPartBuiltServer`, `SetPartFromSyncData`, `AfterStoreLoad`), plus the
  action set and `AT_*` ints.
- `references/fence-gate-case.md` — the Fence worked example: gate state machine, open/close animation,
  combolock, barbed-wire area damage, and the v110 gate-persistence version bump.
- `references/persistence-audit.md` — the three-int bitmask packing, the 93-part cap, `OnStoreSave/Load`
  ordering, the version bumps, and the R9 hand-off checklist.
- `references/dayz-1-30-construction-rebuilding.md` — 1.30 hierarchy (`ConstructionBasic` /
  `ConstructionBase` / `Rebuilding`), `ConstructionPartTypeData`, `*ServerEx` / `IsCollidingEx`,
  obsolete collision-trigger wrappers, per-part config keys, brick/mortar/trowel loop, Fence `DigitalCodeLock`,
  `CombinationLock` inside/outside API, and the porting list for ≤1.29 mods.

## Rules promoted from lessons learned corpus (added 2026-07-27)

Promoted from `AI/20_Knowledge/lessons-learned.md` so they trigger automatically rather
than depending on someone remembering to look them up. Each rule cites its originating `LL-NNN`;
the full entry (symptom, origin, evidence) lives there. Do not remove citation: the index
`lessons-index.md` detects promotion by searching for that reference inside the skills.

- **LL-042** — Create an eponymous script class for each placeable config class and inherit from kit/static base providing `SetActions`, `IsDeployable`, and the kit↔static link. Verify both halves before diagnosing hologram or textures.

## DayZ 1.30 changes

Re-verified against `P:\scripts` / `P:\DZ` of **1.30.164078 stable** on 2026-10-15 (first written from the
1.30.164014 Experimental scripts via Digest J). No difference between Exp and stable was found in the
construction code beyond shifted line numbers; the corrections below fix statements that were wrong on both.
The body of this skill already describes 1.30; this section is the delta for people porting a ≤1.29 mod.

### What changed

- **Hierarchy.** `EntityAI.CreateConstructionComponent()` returns null by default (`3_game\entities\entityai.c:3474-3478`). `BaseBuildingBase` overrides it to `new Construction(this)` (`basebuildingbase.c:872-876`); `BuildingBase` to `new Rebuilding(this)` (`4_world\entities\game\super\building.c:23-26`). Both hold `protected ref ConstructionBasic m_Construction` (`basebuildingbase.c:15`; `building.c:15`). `Construction` gained a `Construction(EntityAI)` constructor and `override protected void SetParent(EntityAI)`; the `Construction(BaseBuildingBase)` constructor is kept as "legacy (unused)" (`construction.c:6-19,44-49`).
- **Static vs instance.** `ConstructionPartTypeData` caches each part's `Construction{}` fields per type (`constructionparttyped.c:2-71`), held by `EntityAIType` (`3_game\entities\entityaitype.c:66-85`). `ConstructionPart` keeps `m_PartTypeData`, `m_LocalSyncBitMask`, `m_IsBuilt` (`constructionpart.c:9-11`); its old 8-argument constructor has an empty body (`:14-16`); parts are spawned from a typename and get `SetPartTypeData` (`constructionbase.c:531-538`).
- **`*Ex` path.** `BuildPartServer` / `DismantlePartServer` / `IsColliding(string)` are `[Obsolete]` forwarders on `ConstructionBase` (`constructionbase.c:1702-1754`). Vanilla calls only `BuildPartServerEx` / `DismantlePartServerEx` / `IsCollidingEx(CollisionCheckData)` (`:179,190,1481`) — grep of `P:\scripts` finds no caller of the old three. `IsCollidingEx` and `CollisionCheckData` are **not** new: they existed in 1.29 (1.29: `construction.c:1070,1322`) and 1.29's `ActionBuildPart` already used them; what changed is that `ActionBuildShelter` and `MiscGameplayFunctions` stopped calling `IsColliding(string)` (1.29: `actionbuildshelter.c:238`, `miscgameplayfunctions.c:908,944`).
- **`player` may be null** in `BuildPartServerEx` / `DismantlePartServerEx` (`constructionbase.c:175-178,186-189`; materials are refunded only when it is not null, `:197-198`). On a `BaseBuildingBase` the call still ends in `OnPartBuiltServer(notnull Man …)` / `OnPartDismantledServer(notnull Man …)` (`basebuildingbase.c:549,589`), so pass a real player there.
- **Destroy path.** Part ruin is handled by the component (`constructionbase.c:352-367`), and `Construction.DestroyPartServer` now cascades to dependent parts itself (`construction.c:93-114`). It calls `OnPartDestroyedServer(player, part, action)` **without** the 4th argument, so `destroyed_by_connected_part` is always `false` in that hook (`construction.c:100`; 1.29 passed it: `construction.c:131`). Vanilla `Fence.OnPartDestroyedServer`'s `IsGate() && destroyed_by_connected_part` branch (`fence.c:378-390`) can therefore no longer run through vanilla code; the gate's locks are dropped instead by `essential_attachments[]` (`constructionbase.c:1338+`; `config.cpp:4903-4907`). Static reading, not measured in-game.
- **Collision triggers.** `CreateCollisionTrigger` / `DestroyCollisionTrigger` / `IsTriggerColliding` still exist, `[Obsolete("no replacement")]` (`constructionbase.c:1756-1801`). 1.29 never called `CreateCollisionTrigger` either (commented out, 1.29: `construction.c:1046`).
- **`ConstructionActionData.SetSlotId` / `GetSlotId`.** `[Obsolete("1.30: Unsafe, overridden ActionData used in-system instead")]` (`constructionactiondata.c:617-627`); vanilla no longer calls `SetSlotId` — the slot travels in `AttachToConstructionActionData.m_SlotID` (`actionattachtoconstruction.c:1-4`).
- **Actions.** `ActionBuildPart` works on any `EntityAI` with a component (`actionbuildpart.c:150-153`) and sets the animation in `SetBuildingAnimationEx` (`:241-267`; the old `SetBuildingAnimation(ItemBase)` is obsolete, `:370-374`). New `ActionBuildPartHandsGeneric : ActionBuildPart` builds `TOOL_HANDS` parts with empty hands (`:377`, registered on the player `playerbase.c:1737`); `ActionActionBuildPartNoTool` is now an empty subclass of it and no longer registered (`actionbuildpart.c:543`). `ShelterSite` builds through `ActionBuildPart` (`sheltersite.c:164`); `ActionBuildShelter` is `[Obsolete("1.30: driven by ActionBuildPart")]` (`actionbuildshelter.c:1`). `ActionConstructionPartInfoContinuous` shows build info (`playerbase.c:1735`). The "instant" build/dismantle/destroy actions (`ActionBuildPartInstantDebug` etc.) are `#ifdef DIAG_DEVELOPER` only (`actions\instant\actionbuildpartinstant.c:2-3`) — not available on retail servers.
- **Rebuilding (map buildings, not player fences).** `BuildingBase` registers `m_Construction.m_SyncParts1`..`10` (`building.c:29-45`). Packing: `BIT_INT_SIZE = 32` (`3_game\tools\bitarray.c:4`) × 10 ints / `BITWISE_SYNCINFO_SIZE_BASE = 2` (`constructionpart.c:6`) = **160 part ids**. Saved via `HandleStoreSave` → `SerializeConstructionData` (`rebuilding.c:468-514`). Default build mask sets bit 0 (`constructionpart.c:111-117`); reversible parts (`ConstructionPartRebuildReversibleBase`) also store their facing in bit 1 (`:1049,1065-1071`). No three-state "ruined" encoding exists in those methods.
- **Masonry loop.** `PileOfBricks` + `ActionPickUpBricks.YIELD = 3` (`actionpickupbricks.c:16-18`). `MortarMix` mixes at water/well (`mortarmix.c:6-7`, `ActionMixMortarAtWater` uses `CCTLiquid`, `actionmixmortaratwater.c:50`) into `MortarMix_Opened`, which only attaches to a construction (`mortarmix_opened.c:7`). `BrickTrowel` adds `ActionBuildPart` and `ActionRepairPart` (`bricktrowel.c:1-8`); build anim `CMD_ACTIONFB_BRICKTROWEL` (`actionbuildpart.c:260-261`). Tool mask `TOOL_BRICKLAYING = 256` (`constructionconstants.c:25`). `[UNVERIFIED]` the `CfgVehicles` entries of `BrickTrowel`, `CommonBrick`, `MortarMix`, `PileOfBricks` and `DigitalCodeLock` — none of them is in the unpacked `P:\DZ` configs (they ship in packages not unpacked here).
- **Fence locks.** `Fence` accepts `DigitalCodeLock` on `Att_CodeLock` beside the combination lock (`fence.c:18-26,161-165`; config `attachments[]` `config.cpp:4362-4374`), never both at once (`fence.c:438-445`). `IsLocked()` covers both (`:127-141`). The old slot-name members `ATTSLOT_CAMONET` / `ATTSLOT_BARBEDWIRE_DOWN` / `ATTSLOT_BARBEDWIRE_UP` were renamed `ATTACHMENT_SLOT_*` (`:22-24`). Code-lock brute-force protection reads `cfggameplay.json` `ExternalLockData` (`protectionCountStageOne=3`, `protectionCountStageTwo=1`, `protectionTime=30`, `protectionResetTime=3600`; `3_game\cfggameplaydatajson.c:421-438`; `codelockcomponent.c:66-67,474-479`).
- **`CombinationLock` has an inside and an outside.** `SetCombination(int, bool setOtherSide = true)` (`combinationlock.c:233`), `DialNextNumber(int lockSide = FACING_OUTSIDE)` (`:273`), `SetNextDial(int lockSide = FACING_OUTSIDE)` (`:340`), `CheckLockedStateServer(bool insideLock = false)` (`:511`), `UnlockOnServer(EntityAI player, EntityAI parent, bool isInsideLock = false)` (`:409`) replaces the removed `UnlockServer`; `Unlock()` is an obsolete empty declaration (`:861-862`), `IsLockedOnGate()` is obsolete → `IsLocked()` (`:846-858`). `DestroyLock`, `ShuffleLock`, `UpdateVisuals`, `UpdateSound` and the sound methods are now `protected`. `LockAction` values were renamed and renumbered (`DIAL_NUMBER_CHANED_INSIDE/_OUTSIDE`, `DIAL_INDEX_CHANGED_INSIDE/_OUTSIDE`; `LOCKED` moved from 3 to 5, `:12-23`). Dialling no longer unlocks (`actiondialcombinationlockontarget.c:72-82`); the player must use `ActionCombinationLockUnlock` (`combinationlock.c:686`; `actioncombinationlockunlock.c`). A lock attaches to any entity whose `CanUseConstruction()` is true (`combinationlock.c:523-543`), not only a Fence.
- **Material enum moved.** `ConstructionMaterialType` now lives in `constructionbase.c:2-12` and adds `MATERIAL_BRICK = 6`, `MATERIAL_RUBBLE = 7`; `BaseBuildingBase`'s build/dismantle sound switch still handles only log/wood/stairs/metal/wire (`basebuildingbase.c:1088-1118`).
- **`disableSimulation`.** `[CHANGELOG 1.30]` house entities may set a `disableSimulation` config parameter. Not confirmed: no `P:\DZ` config and no `P:\scripts` file uses that key; the natives `Entity.DisableSimulation` / `GetIsSimulationDisabled` (`3_game\entities\entity.c:3-6`) already existed in 1.29. Not a `BaseBuildingBase` field.
- **Activation and new keys.** Any `CfgVehicles` class with a `Construction` subclass gets a component at init (`entityai.c:248-249`). 1.30 per-part keys (decay, `essential_attachments`, `item_falling_data`, `StaticsSupportData`, `EffectsData`, `custom_part_type`, overrides, `skipOnRepair`/`skipOnDismantle`): `references/dayz-1-30-construction-rebuilding.md` §9.2. Decay is generic — it applies to a `BaseBuildingBase` part too if its config enables it (`constructionbase.c:307-331`) — and `cfggameplay.json` `GeneralData.disableBaseDecay` switches it off (`cfggameplaydatajson.c:60`; `construction_basic.c:37-40`).
- **Rebuildable map buildings.** Script classes exist for the Nasdara houses (`4_world\entities\building\rebuildable\house_*.c`), `Land_IrrigationTunnel_Entrance_01` and `Land_WaterWell_Brick` (whose water depends on the built `roof` part, `land_waterwell_brick.c:1-20`). Their configs/models are not in the unpacked `P:\DZ` (Nasdara data is not unpacked here), so part names come only from the debug spawns. Terrain holes and underground areas: `dayz-underground`.

### What breaks (≤1.29 mods)

Compile breakers:
1. `override void SetParent(BaseBuildingBase parent)` in a `modded class Construction` / subclass → must be `override protected void SetParent(EntityAI parent)` (`construction.c:44`).
2. Code outside the class that reads `bbb.m_Construction` (now `protected ConstructionBasic`) → use `GetConstruction()` / `GetConstructionBasic()`.
3. Calling `bbb.ConstructionInit()` from outside: it now lives on `EntityAI` as `protected void ConstructionInit()` (`entityai.c:3464`). A 1.29-style public `override void ConstructionInit()` in a subclass now overrides a protected method (whether the compiler accepts the visibility change was not tested here) and bypasses the 1.30 factory path (`ConstructionInit` → `CreateConstructionComponent` → `RegisterConstructionSyncVariables` → `Init`, `entityai.c:3464-3478`). To install your own `Construction` subclass, override `CreateConstructionComponent()` instead.
4. `CombinationLock.UnlockServer(...)` (removed), `SetCombination(int)` / `DialNextNumber()` / `SetNextDial()` / `CheckLockedStateServer()` overrides (signatures changed), outside calls to the now-`protected` lock methods, `LockAction.DIAL_NUMBER_CHANED` / `DIAL_INDEX_CHANGED`, `Fence.ATTSLOT_*`.

Compile, but silently stop working (not in the API delta as behaviour changes):
5. Overrides of `BuildPartServer`, `DismantlePartServer`, `IsColliding(string)` — never called. Move the hook to `BuildPartServerEx` / `DismantlePartServerEx` / `IsCollidingEx`, or to `BaseBuildingBase.OnPartBuiltServer` / `OnPartDismantledServer`.
6. Overrides of `ActionBuildPart.SetBuildingAnimation(ItemBase)` — never called; override `SetBuildingAnimationEx`.
7. Reading `ConstructionPart.m_Name` / `m_PartName` / `m_MainPartName` / `m_Id` / `m_IsBase` / `m_IsGate` / `m_RequiredParts` — always empty/0/null. Creating parts with `new ConstructionPart(name, part_name, …)` gives a part without type data (its getters then dereference null `m_PartTypeData`).
8. `CanBuildPart(part, tool, false)` used to skip the tool check — the flag is ignored now.
9. `ConstructionActionData.GetSlotId()` — never set by vanilla any more.
10. Hooks on `ActionBuildShelter`, `ActionActionBuildPartNoTool`, `ConstructionActionData.OnUpdateActionsNoTool` (no longer registered, `constructionactiondata.c:205` has no subscriber), `ShelterSite.CanUseHandConstruction` (override removed; now derived from `TOOL_HANDS` parts, `constructionbase.c:347-350,545`).
11. `OnPartDestroyedServer(..., destroyed_by_connected_part)` logic — the flag is always false (see Destroy path).
12. Logic placed in `BaseBuildingBase.EEHealthLevelChanged` after `super` that expected to run *before* vanilla destroyed the part — vanilla now does it inside `super` (`entityai.c:1061-1062`).
13. `show_on_init` and `platform_support` config keys — no longer read.
14. Fence/combination-lock automation that assumed "last dial = unlocked".
15. `CombinationLock` subclasses with their own `OnStoreLoad` that do not read `m_CombinationInside` when `version >= 143`.

### Migration checklist

- [ ] Replace overrides of `BuildPartServer` / `DismantlePartServer` / `IsColliding` / `SetBuildingAnimation` with the `Ex` versions.
- [ ] Replace `part.m_*` field reads with the getters; never `new ConstructionPart(...)` yourself.
- [ ] Stop using construction-box triggers; build `CollisionCheckData` and call `IsCollidingEx`.
- [ ] Access construction via `GetConstruction()` / `GetConstructionBasic()`; customise it via `CreateConstructionComponent()`.
- [ ] Re-check every `CanBuildPart(…, false)` call and every no-tool build action.
- [ ] Keep `BaseBuildingBase` `id` in 1..93 and the `OnStoreSave` order `01,02,03,m_HasBase`.
- [ ] Do not write Rebuilding's 10-int stream into a `BaseBuildingBase` subclass.
- [ ] If attaching a code lock to a Fence, use slot `Att_CodeLock` and `DigitalCodeLock`.
- [ ] If subclassing `CombinationLock`, port to the inside/outside API and handle stream v143.
- [ ] Decide on `disableBaseDecay` / `disableItemFalling` in `cfggameplay.json` for your server.
- [ ] R9-audit any bitmask / `OnStoreSave` change (`references/persistence-audit.md`).

### Detail

`references/dayz-1-30-construction-rebuilding.md` (EXACT blocks), `references/persistence-audit.md` §8, `references/config-contract.md` §3 and §8, `references/entity-lifecycle.md` §2/§4, `references/fence-gate-case.md` §6.
