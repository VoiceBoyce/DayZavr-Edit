# Entity lifecycle — deploy → build → sync → persist → load

First authored 2026-07-07 (F4) from vanilla 1.29 scripts; **rewritten 2026-10-15 for 1.30.164078 stable**. All
line numbers are `P:\scripts` (1.30) unless written `1.29:`.

The exact call chain from crafting a kit to a base surviving a server restart, hop by hop. Roots:
`P:\scripts\` = `<dayz-projects>\scripts\`. Server-authoritative: every
mutating hop runs through a `*Server` method gated by `g_Game.IsServer()`; the client only previews.

## 1. Deploy (hologram)

1. `RecipeBase` (`craftfencekit.c:1`) consumes materials → a `KitBase: ItemBase` (`kitbase.c:1`;
   `IsBasebuildingKit()→true` `:5`).
2. Kit `SetActions()` adds `ActionTogglePlaceObject` + `ActionDeployObject` (`kitbase.c:146-152`).
3. `ActionDeployObject` (`actiondeployobject.c:7`) is hologram-driven (`ActionUsesHologram()→true` `:15`).
   Placement validity is checked against `player.GetHologramLocal()/GetHologramServer()` — `IsColliding()`,
   `item.CanBePlaced(...)` (`actiondeployobject.c:35-75`). Kits declare `DoPlacingHeightCheck()→true` and a
   `HeightCheckOverride()` (FenceKit 2.54 `fencekit.c:36-44`; WatchtowerKit 2.83 `watchtowerkit.c:32-40`).
4. On finish the client calls `entity.OnPlacementComplete(...)` (`actiondeployobject.c:164`). The SERVER-only
   `FenceKit.OnPlacementComplete` (`fencekit.c:19-34`) does `g_Game.CreateObjectEx("Fence", …)` with
   `ECE_PLACE_ON_SURFACE`, sets pos/orient, `HideAllSelections()`.
5. The kit self-deletes on `OnEndServer` if it was placed (`actiondeployobject.c:205-234`, delete at `:232`).
   Steps 1-5 use files that are unchanged from 1.29 apart from line endings.

## 2. Construction init

The `EntityAI` constructor calls `ConstructionInit()` when `cfgVehicles <type> Construction` exists
(`3_game\entities\entityai.c:248-249`); `ConstructionInit()` (`:3464-3472`, `protected`) calls
`CreateConstructionComponent()` (`:3474-3478`), registers construction sync variables and `Init()`s the component
once. `BaseBuildingBase` overrides the factory to `m_Construction = new Construction(this)`
(`4_world\entities\itembase\basebuildingbase.c:872-876`) and stores it in `protected ref ConstructionBasic
m_Construction` (`:15`); its own constructor (`:51`) and `OnCreatePhysics()` (`:489-495`) call `ConstructionInit()`
again, which is then a no-op. `GetConstruction()` is `Construction.Cast(m_Construction)` (`:883-886`).
`Construction.Init()` → `ConstructionBase.UpdateConstructionParts()` (`construction.c:21-24`;
`4_world\classes\constructionbase.c:516-546`) creates one `ConstructionPart` per entry of the type's cached
`ConstructionDataTypeHolder` and fills `map<string, ref ConstructionPart> m_ConstructionParts`
(`constructionbase.c:42`), keyed by part config-class name. `EntityAI.EEInit` then calls
`EEInitConstruction()` and `InitBaseState()` on the component (`entityai.c:937-942`).
Before 1.30: `BaseBuildingBase` owned `ref Construction m_Construction` and a public `ConstructionInit()`
created it and re-read the config on every call (1.29: `basebuildingbase.c:8,965-973`; `construction.c:235-270`).

## 3. Base part

The freshly-spawned entity has NO parts built. The player attaches the base material (e.g. WoodenLogs) then
builds the `base` part (`is_base=1`). Building the base sets `HasBase()=true`, spawns a construction kit back
(`OnPartBuiltServer` `basebuildingbase.c:553-560`), and toggles the `"Deployed"` animation/proxy
(`InitVisuals` `:677-691`; physics in `SetPartFromSyncData` `:279-283`).

## 4. Build a part

1. `ActionBuildPart` (`4_world\classes\useractionscomponent\actions\continuous\actionbuildpart.c:33`) with a tool
   in hand (or `ActionBuildPartHandsGeneric` `:377` with empty hands for `TOOL_HANDS` parts). Collision is
   re-checked server-side in `ActionConditionContinue` / `OnFinishProgressServer` via `IsCollidingEx`
   (`actionbuildpart.c:123-148, 150-204`).
2. `OnFinishProgressServer` (`:150-204`) gets the component with `ent.GetConstructionBasic()` (works on any
   `EntityAI`), re-validates `CanBuildPart` (the "do we need to check it here" comment at `:162`) and
   `IsCollidingEx(checkData)` with the player excluded (`:157-167`), then calls
   `construction.BuildPartServerEx(action_data.m_Player, partName, AT_BUILD_PART)` (`:170`) and damages the tool
   (or, for empty-hand parts with `damage_hands_on_build`, the gloves / hands) (`:178-202`).
   Before 1.30: `construction.BuildPartServer(...)` (1.29: `actionbuildpart.c:112-130`). That method is now an
   obsolete forwarder with no vanilla caller (`constructionbase.c:1702-1708`).
3. `ConstructionBase.BuildPartServerEx` (`constructionbase.c:179-184`) runs `TakeMaterialsServer`
   (`:1139-1179` — `lockable=1` locks the slot; else subtract `quantity`, or delete on `quantity==-1`). The
   `Construction` override (`construction.c:52-74`) then resets the part's damage-zone health and calls
   `m_Parent.OnPartBuiltServer` (`basebuildingbase.c:549-579`).
   Before 1.30: `BuildPartServer` also destroyed a construction-box trigger; the trigger helpers still exist as
   `[Obsolete("no replacement")]` (`constructionbase.c:1756-1801`). Do not write new trigger calls.
4. `OnPartBuiltServer` → `RegisterPartForSync` (sets the part's `id` bit in `m_SyncParts01/02/03`
   `basebuildingbase.c:137-164`), `RegisterActionForSync`, sync, `SetPartFromSyncData` (show physics + visual),
   regen navmesh (`:549-579`).

### CanBuildPart / CanDismantlePart / CanDestroyPart gates

- `CanBuildPart(part, tool, use_tool)` (`constructionbase.c:583-587`) → `CanBuildPartEx` (`:590-611`):
  `!IsPartConstructed && HasRequiredPart && PartHasAnySupportFulfilled && !HasConflictPart && HasMaterials &&
  !MaterialIsRuined && CanUseToolToBuildPart`; failure bits in `EConstructionInfoCategories`
  (`constructionconstants.c:1-11`). **`use_tool` is ignored** — the tool check always runs, and a null tool passes
  only for `TOOL_HANDS` parts (`:1404-1441`). Before 1.30: `(!use_tool || CanUseToolToBuildPart)` (1.29:
  `construction.c:296-304`).
- `CanDismantlePart` (`constructionbase.c:739-741`): `IsPartConstructed && !HasDependentPart &&
  SupportRemovalCheck && CanUseToolToDismantlePart`.
- `CanDestroyPart` (`:1022-1028`): `IsPartConstructed && !HasDependentPart && SupportRemovalCheck` (no
  tool/material gate). Damage-driven destruction does not go through this gate (see §6).

## 5. Upgrade

Just building further parts whose `required_parts[]` are satisfied (`HasRequiredPart` `constructionbase.c:682-696`)
and whose `conflicted_parts[]` are not built (`HasConflictPart` `:704-735`). E.g. `wall_wood_down` requires
`wall_base_down` and conflicts with `wall_metal_down`. Radial variants: multiple buildable parts under one
selection produce N action variants — `ActionBuildPart` uses `m_VariantID`; `ConstructionActionData.OnUpdateActions`
(`constructionactiondata.c:163-203`) calls `ConstructionBase.GetConstructionPartsToBuild(...)` (`constructionbase.c:642-654`)
for every action component name under the cursor and sets the variant count (`constructionactiondata.c:187`).
Empty-hand parts use `OnUpdateActionsHandsGeneric` (`:236`).

## 6. Dismantle / destroy / fold

- `ActionDismantlePart` (`actiondismantlepart.c:23`) → `DismantlePartServerEx` (`:107`;
  `constructionbase.c:190-206`): refunds materials only if `player` is set (`ReceiveMaterialsServer` →
  `StaticConstructionMethods.SpawnConstructionMaterialPilesEx`, `:1183-1194,1809`), drops non-usable materials of a
  base part, drops `essential_attachments[]`, lets items in the `item_falling_data[]` box fall (unless
  `disableItemFalling`), then `Construction` calls `OnPartDismantledServer` (`construction.c:77-90`). Blocked if
  the part `HasDependentPart` (`constructionbase.c:772-783`). Dismantling the **base** destroys the whole
  construction (`basebuildingbase.c:614-619`).
  Before 1.30: `DismantlePartServer` (now an obsolete forwarder, `constructionbase.c:1710-1716`) and
  `SpawnConstructionMaterialPiles` (obsolete → `…Ex`, `:1908-1912`).
- Destroy on ruin: `EntityAI.EEHealthLevelChanged` → `HandleDamageLevelConstruction` (`3_game\entities\entityai.c:1061-1062,1094-1099`)
  → `ConstructionBase.OnHealthLevelChanged` (`constructionbase.c:352-367`): when a zone hits `STATE_RUINED`,
  `DestroyPartServer(null, part, AT_DESTROY_PART)`. `Construction.DestroyPartServer` (`construction.c:93-114`)
  calls `OnPartDestroyedServer` (without the `destroyed_by_connected_part` argument) and then
  `DestroyConnectedParts` (`constructionbase.c:223-238`, dependents, with the gate exception `ExceptionCheck`
  `construction.c:136-149`). `Construction.OnHealthLevelChanged` also unmounts ruined barbed wire
  (`construction.c:116-133`).
  Before 1.30: all of this sat in `BaseBuildingBase.EEHealthLevelChanged` (1.29: `basebuildingbase.c:496-528`).
- Fold: with no base and no attachments (`CanFoldBaseBuildingObject` `basebuildingbase.c:962-970`),
  `ActionFoldBaseBuildingObject` → `FoldBaseBuildingObject` (`:972-978`) converts back to a kit in hands.

## 7. Persist → load reconciliation

- `OnStoreSave` (`basebuildingbase.c:420-430`): writes `m_SyncParts01`, `02`, `03`, then `m_HasBase`.
  A subclass appends its extra fields AFTER `super` (Fence adds `m_GateState`, `m_IsOpened` — `fence.c:246-254`).
  `EntityAI.OnStoreSave` additionally calls the component's `HandleStoreSave` (`entityai.c:2912-2914`), which
  writes nothing for `Construction` (only `Rebuilding` overrides `HandleStorageEvents`, `rebuilding.c:468-471`).
- `OnStoreLoad` (`:432-464`): reads the three ints then `m_HasBase`, each with a `return false` on read
  failure (default-then-abort). Subclass reads AFTER `super` in the same order (`fence.c:256-289`, with a
  `version<110` legacy branch).
- `AfterStoreLoad` (`:466-474`) → `SetPartsAfterStoreLoad()` (`:476-487`) → `SetPartsFromSyncData()`: rebuilds
  each part's built flag from the bitmask, sets base state from the base part, re-syncs. Guarded by
  `m_FixDamageSystemInit` (the damage-system migration path uses `EEOnAfterLoad` + a 500ms deferred call
  instead, `:505-513`).
- `SetPartFromSyncData` (`:265-304`) — the reconciliation core: per part, compares "built in sync bitmask"
  vs "built flag", adds/removes constructed parts, shows/hides physics, toggles the `Deployed` proxy for the
  base, and **failsafe-relocks attached materials** (`:303`, "failsafe for corrupted sync/storage data").
  This whole section is unchanged in logic from 1.29.

## 8. Action-type ints and the action set

`4_world\classes\useractionscomponent\_constants.c:6-8`: `AT_BUILD_PART=193`, `AT_DISMANTLE_PART=195`,
`AT_DESTROY_PART=209`. Also `m_InteractedPartId` / `m_PerformedActionId` (`basebuildingbase.c:13-14`, registered
`:46-47`) sync the last action so clients play the right SFX (`SetActionFromSyncData` `:248-262`).

Action classes (`scripts\4_world\classes\useractionscomponent\actions\`): `continuous\actionbuildpart.c`
(`ActionBuildPart`, `ActionBuildPartHandsGeneric` — empty-hand build, registered on the player
`playerbase.c:1737`), `continuous\actiondismantlepart.c`, `continuous\actiondestroypart.c`,
`continuous\deployactions\actiondeployobject.c`, `continuous\actionfoldbasebuildingobject.c`,
`continuous\actionpartinfo.c` (`ActionConstructionPartInfoContinuous`, `playerbase.c:1735`),
`continuous\actionpickupbricks.c`, `interact\actionopenfence.c`, `interact\actionclosefence.c`,
`singleuse\actionattachtoconstruction.c`, `singleuse\actioncombinationlockunlock.c`,
`singleuse\actioninteractcodelockitem.c` / `actionresetcodeoncodelock..c` (code lock),
`singleuse\actionplugintofence.c` (DEPRECATED — do not build on it). The `instant\*InstantDebug` build/dismantle/
destroy actions exist only under `DIAG_DEVELOPER` (`instant\actionbuildpartinstant.c:2`).

No-tool shelter path on 1.30: `ShelterSite` adds `ActionBuildPart` (`entities\itembase\basebuildingbase\sheltersite.c:164`)
and its parts are buildable with empty hands through `ActionBuildPartHandsGeneric`; `CanUseHandConstruction()`
comes from the component (`TOOL_HANDS` parts, `constructionbase.c:347-350,545`). `ActionBuildShelter` is
`[Obsolete("1.30: driven by ActionBuildPart")]` and no longer registered (`interact\actionbuildshelter.c:1`), and
`ActionActionBuildPartNoTool` is an empty, unregistered subclass of `ActionBuildPartHandsGeneric`
(`actionbuildpart.c:543`). `[UNVERIFIED]` the `ShelterSite` part masks in config (`P:\DZ\gear\camping\config.cpp:11712+`)
were not re-traced. Before 1.30: `ShelterSite` added `ActionBuildShelter` and overrode `CanUseHandConstruction()→true`
(1.29: `sheltersite.c:83-87,172`).

## 9. 1.30 hop notes

- `CreateConstructionComponent` / `GetConstructionBasic` are the hooks; see
  `references/dayz-1-30-construction-rebuilding.md` §1 for EXACT getters.
- `BuildingBase` uses the same hooks to install `Rebuilding` (`4_world\entities\game\super\building.c:23-51`). That
  path does **not** go through `FenceKit.OnPlacementComplete`.
- `SetRequestBuiltState` is obsolete (`constructionpart.c:504-512`); use `SetBuiltState`.
- Silent porting traps (hooks that compile but no longer run): `references/dayz-1-30-construction-rebuilding.md` §10.
