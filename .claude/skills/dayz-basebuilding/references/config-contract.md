# Config contract — declaring a buildable BaseBuildingBase entity

First authored 2026-07-07 (F4) from vanilla 1.29 scripts; **rewritten 2026-10-15 for 1.30.164078 stable** —
every reader below is a `P:\scripts` (1.30) line; 1.29 readers are kept only in §8 for porting. **Updated
2026-10-21 for 1.30.164200**: `cbase:` refs and the Fence config refs (+10 after the new `Fence` `Navmesh` block at
`P:\DZ\gear\camping\config.cpp:4418-4427`) re-read on 164200; §3.1, §3.2, §3.3 cover the utility/support changes.

Since 1.30 the `Construction{}` block is read **once per type**: `EntityAIType.InitConstructionDataTypeHolder`
(`P:\scripts\3_game\entities\entityaitype.c:66-74`) spawns a `ConstructionDataTypeHolder`, which walks the block
(`4_world\classes\basebuilding\typeconstructiondata\constructionparttypedholder.c:10-63`) and fills one
`ConstructionPartTypeData` per part via `InitPartTypeData` (`…\constructionparttyped.c:45-71`). Instances
(`ConstructionPart`) only hold `m_PartTypeData` + built/sync state. Field names of the 1.29 block did not change;
1.30 adds optional keys (§3.1) and stops reading two (§3.2). Roots: `P:\scripts\` = `<dayz-projects>\scripts\`;
config root `P:\DZ\gear\camping\config.cpp`. Line numbers are for the Fence entity unless noted.

## 1. The entity class

Base class (`config.cpp:25-31`):

```cpp
class BaseBuildingBase: Inventory_Base { debug_ItemCategory=10; weight=100000; itemSize[]={50,50}; disallowSwapCollisions="true"; };
```

A concrete entity, e.g. `class Fence: BaseBuildingBase` (`config.cpp:3252-3282`):

- `scope=2; model="\DZ\gear\camping\fence.p3d"; carveNavmesh=1; physLayer="item_large";`
- `createProxyPhysicsOnInit="false";` + `createdProxiesOnInit[]={"Deployed"};` — the `Deployed` proxy is
  the base collision, toggled by animation (see the SKILL flow and `entity-lifecycle.md`).
- Since 1.30.164200: `class Navmesh { class TransformParts { class Gate { transformAnimationSource="Wall_Gate_Rotate"; }; }; };`
  (`config.cpp:4418-4427`) — the navmesh follows the gate leaf. Subclasses of `Fence` inherit it; a gate model
  without the `Wall_Gate_Rotate` AnimationSource was not tested. Engine-side (no `P:\scripts` reader).

## 2. Attachment / material slots on the entity class

| Field | Meaning | Reader |
|---|---|---|
| `attachments[]` | every material and attachment slot the entity accepts (1.30 Fence adds `Att_CodeLock`) | `GetAttachmentSlots()` reads `CfgVehicles <type> attachments` — `basebuildingbase.c:847-854`; declared `config.cpp:4362-4374` |
| `hybridAttachments[]` | attachments whose health mirrors a damage zone of the same name | `CheckForHybridAttachments` `basebuildingbase.c:1121+`; read in ctor `:53-57`; declared `config.cpp:3270` |
| `mountables[]` | attachments that get proxy physics toggled (barbed wire) | read in ctor `:58-62`; `UpdateAttachmentPhysics` early-returns if a slot is NOT in `mountables` `:782-786`; declared `config.cpp:3277` |

## 3. The `Construction{}` block field-by-field

Two-level nesting: `Construction { class <main_part> { class <part> {...} } }`. Example
`Construction > wall > base` (`config.cpp:4543-4564`) and `wall > wall_base_down` (`:4565-4597`); the whole
Fence block is `config.cpp:4539-4920`.

Paths below: `parttyped` = `P:\scripts\4_world\classes\basebuilding\typeconstructiondata\constructionparttyped.c`,
`cbase` = `P:\scripts\4_world\classes\constructionbase.c`.

| Field | Type / meaning | Reader (1.30) | Used by (1.30) |
|---|---|---|---|
| `name` | localized display name (`$STR_...`) | `parttyped:75-79` | `ConstructionPart.GetName()` |
| `id` | UNIQUE sync/persistence bit index, 1..93 on a `BaseBuildingBase` | `parttyped:83` | `RegisterPartForSync` `basebuildingbase.c:137-164` |
| `is_base` | 1 = foundation part; dismantling it destroys the whole entity | `parttyped:84` | `basebuildingbase.c:553-560,614-619` |
| `is_gate` | 1 = this part is a gate (Fence) | `parttyped:85` | `Construction.ExceptionCheck` / `GetGateConstructionPart` `construction.c:136-161` |
| `required_parts[]` | parts that must be built first | `parttyped:88-93` | `HasRequiredPart` `cbase:723-737`; `HasDependentPart` `cbase:813-824` |
| `conflicted_parts[]` | parts that block this one if already built | `parttyped:139-145` | `HasConflictPart` `cbase:745-776` |
| `collision_data[]` | two memory-point names `{min, max}` for the build collision box | `parttyped:147-157` | `GetCollisionBoxData` `cbase:1635`; `IsCollidingEx` `cbase:1527` |
| `build_action_type` | bitmask AND-matched against the tool's own `build_action_type`; `TOOL_HANDS` (128) = buildable with empty hands | `parttyped:95-102` | `CanUseToolToBuildPart` `cbase:1442-1479` |
| `dismantle_action_type` | same, for dismantle | `parttyped:104-106` | `CanUseToolToDismantlePart` `cbase:1481+` |
| `material_type` | `ConstructionMaterialType` enum — drives build/dismantle SFX | `parttyped:129-137` | `GetMaterialType` `cbase:1509-1513`; SFX switch `basebuildingbase.c:1088-1118` |

### `ConstructionMaterialType` enum (`cbase:2-12`)

`MATERIAL_NONE=0`, `MATERIAL_LOG=1`, `MATERIAL_WOOD=2`, `MATERIAL_STAIRS=3`, `MATERIAL_METAL=4`,
`MATERIAL_WIRE=5`, `MATERIAL_BRICK=6`, `MATERIAL_RUBBLE=7`. `BaseBuildingBase` maps only log/wood/stairs/metal/wire
to sound sets (`basebuildingbase.c:1088-1118`); brick/rubble are used by the rebuilding effects.
Before 1.30: the enum was in `construction.c:1-9` with the first six values (1.29).

### `Materials` sub-block

Each `Material1..N` child under a part, read by `ConstructionMaterialData` (`P:\scripts\4_world\classes\basebuilding\constructionmaterials.c:26-32`, via `parttyped:109-128`):

| Field | Meaning |
|---|---|
| `type` | item classname — used when refunding piles on dismantle |
| `slot_name` | attachment slot the material sits in; `HasMaterialsEx` requires the attachment here with `GetQuantity() >= quantity` (`cbase:1122-1160,1170-1177`) |
| `quantity` | amount consumed; `-1` = delete the whole object; `0` = ignore quantity |
| `lockable` | `1` = lock the material in its slot (stays attached, visually part of the wall) instead of consuming it (`TakeMaterialsServer` `cbase:1180-1220`) |
| `skipOnRepair` | 1.30: material not consumed when repairing (`TakeMaterialsServer`, `cbase:1190-1191`); `HasMaterialsEx` still checks it is present |
| `skipOnDismantle` | 1.30: skipped by `DropNonUsableMaterialsServer` when a **base** part is removed (`cbase:1278-1279`); no other reader |

If the `Materials` block is absent, `HasMaterials` returns true (no material required, `cbase:1159`).

### 3.1 Optional per-part keys added in 1.30

`essential_attachments[]`, `item_falling_data[]`, `insider_minMaxs[]`, `can_part_decay` / `part_decay_threshold`
/ `part_decay_rate` (+ `can_decay_general` on the `Construction` class), `hand_build_FB_anim_override`,
`related_GUI_categories[]`, `damage_hands_on_build`, `is_utility` / `toggle_utility_part_update` (int, since
1.30.164200; it replaced `utility_build[]`, which 164200 no longer reads — `parttyped:266-274`),
`RequiredPartsVariants`, `build_text_override` / `dismantle_text_override`, the geometry/animation-source/roadway
override arrays, `custom_part_type`, `EffectsData`, `StaticsSupportData` (support getters are `bool …(int, out int)`
since 1.30.164200). Readers and effects: table in `dayz-1-30-construction-rebuilding.md` §9.2 and §11. Vanilla Fence now uses `essential_attachments[]` on `wall_gate`
(`config.cpp:4913-4917`) and `item_falling_data[]` on its platform parts; Watchtower uses `item_falling_data[]` on
roofs and stairs (config delta 1.29→1.30). [DESIGN] The rest is rebuild-oriented; a player `BaseBuildingBase` can
ignore it.

### 3.2 Keys 1.30 no longer reads

`show_on_init` (1.29 passed it as the initial built flag, 1.29: `construction.c:259,264`) and
`platform_support` (1.29: `construction.c:792`, used to drop materials from a platform) — no reader in
`P:\scripts`. `DropNonUsableMaterialsServer` now only acts on the **base** part and unlocks its locked materials
(`cbase:1262-1314`). Since 1.30.164200 an empty material slot is skipped silently (`cbase:1288-1290`); 1.30.164078
raised `ErrorEx("Attachment is not present for material type …")` there, reported to throw a VM exception on a
Namalsk fence dismantle [reported: SumrakDZN/DayZNamalsk#4].

### 3.3 Keys 1.30.164200 no longer reads

`utility_build[]` (a text array naming utility parts to (un)build when this part changed). Replace it with
`toggle_utility_part_update=1;` (`parttyped:266-274`); the utility part's own `required_parts[]`,
`conflicted_parts[]` and `SupportRequirements` then decide its state. Vanilla still carries two dead copies
(`P:\DZ\structures_takistan\Industrial\config.cpp:402-405,462-465`).

## 4. `GUIInventoryAttachmentsProps` (`config.cpp:4375-4417`)

Groups attachment slots into inventory categories (Base / Attachments / Material). Each child has
`attachmentSlots[]`, `selection` (the model selection the category maps to), `icon`, `name`. On 1.30 the block is
cached per type in `TypeAttachmentGroupsDataHolder` (`P:\scripts\3_game\entities\typeattachmentgroupdataholders.c:10,39`;
held by `EntityAIType`, `entityaitype.c:3,53,61-64`) and read through it by `GetAttachmentSlotFromSelection` /
`GetAttachmentsFromSelection` (`constructionactiondata.c:469-530`) to resolve which slot an attach action targets.
A part's `related_GUI_categories[]` ties it to these categories (`constructionparttypedholder.c:65-80,82+`).

## 5. Tool contract

A tool participates by declaring `build_action_type` / `dismantle_action_type` in its own CfgVehicles
entry (read at `cbase:1462,1489`). Build is allowed when `(part_mask & tool_mask) > 0`; with no tool, only
parts whose mask contains `TOOL_HANDS` pass (`cbase:1457-1476`). Masks are the enum `EConstructionTools`
(`constructionconstants.c:14-26`: `MOUNT_WIRE=1, NAIL=2, DIG_BASE=4, MOUNT_LOG=8, HEAVY=16, EXCAVATION=32,
WOODWORK=64, HANDS=128, BRICKLAYING=256`). Pickaxe / Shovel / FarmingHoe / FieldShovel, Pliers, SledgeHammer and
BrickTrowel switch the player animation command (`SetBuildingAnimationEx`, `actionbuildpart.c:241-267`).
Whether the tool loses health comes from `ConstructionPartTypeData.DamageToolOnBuild()` (always true for plain
parts, `parttyped:453-456`). Before 1.30: masks were plain ints and the animation switch was
`SetBuildingAnimation(ItemBase)` (now obsolete, `actionbuildpart.c:370-374`).

## 6. DamageZones ↔ part mapping

The entity's `class DamageSystem { class GlobalHealth ...; class DamageZones {...} }` (`config.cpp:3283+`).
**Damage-zone names MUST match part names lowercased** — on ruin `ConstructionBase.OnHealthLevelChanged`
lowercases the zone name and destroys the part of that name (`cbase:353-368`, reached from
`EntityAI.EEHealthLevelChanged`, `3_game\entities\entityai.c:1061-1062,1094-1099`). A zone whose name does not
match a part will not trigger that part's destruction on ruin. `DestroyPartServer` then cascades to dependent
parts (`construction.c:93-114`).

## 7. What the readers verify (cross-check before shipping)

- Base uses the `"Deployed"` proxy; every other part uses a per-part proxy via `AddProxyPhysics(part_name)`
  (`constructionpart.c:428-436`, called from `ShowConstructionPartPhysics` `cbase:1091-1107`). Missing memory
  point / selection → no collision on that part.
- `show/hide` a part = `SetAnimationPhase(part_name, 0/1)` (`constructionpart.c:418-426,480-489`, phase 0 =
  visible). Every `part_name` must be an AnimationSource in config AND a selection in the `.p3d`.

## 8. Before 1.30 — where the 1.29 readers were (porting map)

| Field | 1.29 reader (`O:\…\scripts\4_world\classes\basebuilding\construction.c`) | 1.30 reader |
|---|---|---|
| `name` / `id` / `is_base` / `is_gate` / `show_on_init` | `UpdateConstructionParts` `:235-270` (read per instance) | `parttyped:75-86`; `show_on_init` gone |
| `required_parts[]` | `HasRequiredPart` `:412-435` (protected) | `cbase:723-737` (public) + `parttyped:88-93` |
| `conflicted_parts[]` | `HasConflictPart` `:438-455` (protected) | `cbase:745-776` (public, plus an `out string` overload) + `parttyped:139-145` |
| `build_action_type` / `dismantle_action_type` | `:959-1003` | `parttyped:95-107`; `cbase:1442-1507` |
| `Materials` | `:617-723` (re-read config each call) | `constructionmaterials.c:26-32` (cached) |
| `platform_support` | `DropNonUsableMaterialsServer` `:788-908` | not read |

`UpdateConstructionParts` (`cbase:517-547`) no longer walks config children; it iterates the cached type data.
Collision for a build goes through `IsCollidingEx(CollisionCheckData)` (`cbase:1527`; fill `m_PartName` and
optional `m_AdditionalExcludes` — vanilla inserts the player, `actionbuildpart.c:157-160`) — the same API 1.29
already had. `CreateCollisionTrigger` / `DestroyCollisionTrigger` / `IsTriggerColliding` still exist as
`[Obsolete("no replacement")]` (`cbase:1805-1850`); do not call them from new code.
