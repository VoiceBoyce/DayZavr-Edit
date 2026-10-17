# Persistence audit — the synced bitmask (data-critical, R9)

First authored 2026-07-07 (F4) from vanilla 1.29 scripts; **re-verified 2026-10-15 against 1.30.164078 stable** —
line numbers are `P:\scripts` (1.30) unless written `1.29:`. The `BaseBuildingBase` packing, the 93-part cap and
the save/load order are unchanged from 1.29; only the line numbers moved.

**R9 gate.** Any mod that adds parts, changes the bitmask packing or the `id` ranges, or touches
`OnStoreSave`/`OnStoreLoad` ordering is modifying player progression — a bug here means players lose their
bases after a server restart. Invoke `rigorous-data-audit` (R9) BEFORE declaring release-safe. This file is
the hand-off checklist. Roots: `P:\scripts\` = `<dayz-projects>\scripts\`.

## 1. The representation — three 31-bit ints, netsync AND save at once

Part-built state is NOT stored per part. It is packed into three sync ints `m_SyncParts01/02/03`
(`basebuildingbase.c:10-12`), registered for netsync in the ctor (`:43-45`). This SAME representation is both
the network sync AND the persistence format — there is no separate per-part save record.

Part `id` → bit:

- `id` 1..31  → `m_SyncParts01`
- `id` 32..62 → `m_SyncParts02`
- `id` 63..93 → `m_SyncParts03`

Bit ops in `RegisterPartForSync` (`basebuildingbase.c:137-164`). Also `m_InteractedPartId` /
`m_PerformedActionId` (`:13-14`, registered `:46-47`) sync the last action so clients play the right SFX
(`SetActionFromSyncData` `:248-262`).

## 2. The 93-part cap

Three ints × 31 usable bits = **93 distinct parts maximum per BaseBuildingBase entity**. Every part's `id`
MUST be unique and in 1..93 across the whole `Construction{}` block. A duplicate id makes two parts share a
bit (they toggle together); an out-of-range id writes outside the three ints (silent corruption). There is
no runtime warning — verify by hand or with a script that every `id` in the config is unique and ≤93.

## 3. OnStoreSave / OnStoreLoad — strict ordering

- `OnStoreSave` (`basebuildingbase.c:420-430`): writes `m_SyncParts01`, `02`, `03`, then `m_HasBase`.
- `OnStoreLoad` (`:432-464`): reads the three ints then `m_HasBase`, each guarded with a `return false` on
  read failure (default-then-abort).

**Subclass-after-super invariant:** a subclass writes/reads its extra fields AFTER calling `super`, in the
exact same order in both methods. Fence appends `m_GateState`, `m_IsOpened` after the base three ints
(`fence.c:246-254` save / `:256-289` load). Reorder either side, or write in the subclass before `super`, and
every saved base corrupts on the next load.

## 4. Load reconciliation

- `AfterStoreLoad` (`:466-474`) → `SetPartsAfterStoreLoad()` (`:476-487`) → `SetPartsFromSyncData()`: rebuilds
  each part's built flag from the bitmask, sets base state from the base part, re-syncs.
- Guarded by `m_FixDamageSystemInit` — the damage-system migration path uses `EEOnAfterLoad` + a 500ms
  deferred call instead (`:505-513`).
- `SetPartFromSyncData` (`:265-304`) is the core: per part, compares "built in sync bitmask" vs "built flag",
  adds/removes constructed parts, shows/hides physics, toggles the `Deployed` proxy for the base, and
  **failsafe-relocks attached materials** (`:303`, "failsafe for corrupted sync/storage data").

## 5. Version bumps

- Base damage-system version: `GetDamageSystemVersionChange()→111` (`basebuildingbase.c:1133-1136`).
- Fence gate persistence bumped at **v110** — `Fence.OnStoreLoad` reads a discarded bool for `version < 110`,
  then the gate fields for current versions (`fence.c:256-289`).

When you add a persisted field, bump the version and add a legacy read branch so old saves load with the old
layout and new saves with the new one.

## 6. R9 hand-off checklist

Before declaring a base-building persistence change release-safe, confirm with `rigorous-data-audit`:

1. Every part `id` in the `Construction{}` block is unique and in 1..93.
2. `OnStoreSave` and `OnStoreLoad` write and read the same fields in the same order.
3. Every subclass writes/reads its fields AFTER `super`, same order both ways.
4. A new persisted field bumps the version and adds a legacy branch for old saves.
5. Crash-recovery walk: kill the server between each I/O hop of save/load — does the next boot recover the
   base or lose parts? (R8 mental walk.)
6. The failsafe relock path (`SetPartFromSyncData`, `basebuildingbase.c:303`) still runs for corrupted sync/storage data.
7. In-game gate: build parts, restart the server, confirm every part and the base survive (not just an
   offline audit — the R9 case history shows bases-lost bugs that only surface on a real restart).

## 7. Out of scope for this pass

`[UNVERIFIED]` `TerritoryFlag: BaseBuildingBase` exists at `P:\DZ\gear\camping\config.cpp:12470` and
`staticflagpole.c:46-47` calls `AnimateFlagEx` / `AddRefresherTime01` (declared on `Totem`, `totem.c:506`), but the
territory system (radius, decay, flag-refresher mechanics) was not opened here — do not assume its persistence
follows the same bitmask model without tracing it. The `show_on_init` config field is no longer read on 1.30
(1.29 read it in `construction.c:259`).

## 8. 1.30 — Rebuilding is a second stream (not the 93-part cap)

The three-int / 93-part model is still the persistence of every **`BaseBuildingBase`**; 1.30 adds a second,
independent stream for map buildings (`Rebuilding`). Before 1.30 the three-int model was the only construction
persistence.

Verified unchanged on `BaseBuildingBase` (1.30.164078):

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

`RegisterPartForSync` still packs 1 bit per `id` into `m_SyncParts01/02/03` with the same 1..31 / 32..62 /
63..93 ranges (`basebuildingbase.c:137-164`). `GetDamageSystemVersionChange()` is still `111` (`:1133-1136`).

`Rebuilding` (component on `BuildingBase`, created by `CreateConstructionComponent` at
`4_world/entities/game/super/building.c:23-26`) is a **different** save:

- written from `EntityAI.OnStoreSave` → component `HandleStoreSave` before the energy-manager data
  (`3_game/entities/entityai.c:2912-2914`), read back only when the storage version is ≥ 143 (`:2980-2984`);
  `HandleStorageEvents()` is true only on `Rebuilding` (`4_world/classes/rebuilding/rebuilding.c:468-471`;
  false on `ConstructionBasic`, `3_game/systems/construction_basic.c:29-32`), so a `Construction` (player
  structure) writes nothing extra
- version const `REBUILDING_STORAGE_VERSION = 1` (`rebuilding.c:3`); an older stored version is rejected with
  "presumed corrupted data" (`:483-499,516+`)
- ten ints `m_SyncParts1`..`m_SyncParts10` (`:7-16`), netsync-registered on `BuildingBase` (`building.c:35-44`)
- `HandleStoreSave` → `SerializeConstructionData`: version then the ten ints (`rebuilding.c:473-514`)
- 2 bits reserved per part (`ConstructionPart.BITWISE_SYNCINFO_SIZE_BASE = 2` at `constructionpart.c:6`)
- capacity [DESIGN]: `BIT_INT_SIZE = 32` (`3_game/tools/bitarray.c:4`) × 10 / 2 = **160 part ids**
- bit 0 = built (`constructionpart.c:111-117`); the reversible parts (`ConstructionPartRebuildReversibleBase`,
  `:1049`) store their facing in bit 1 (`:1065-1071`)

Do **not** put Rebuilding parts on a `BaseBuildingBase` entity or write the ten-int blob from a Fence/Watchtower
subclass. Fence extra fields still go **after** `super.OnStoreSave` (`fence.c:246-254`).

`CombinationLock` (Fence attachment) now writes a third int `m_CombinationInside` and reads it only at
stream **version >= 143** (`4_world/entities/itembase/combinationlock.c:128-136,169-177`). A modded `OnStoreLoad`
that always reads the 1.29 two-int layout after `super` will desync every later field. The 1.30 storage version is
144 (`3_game/global/game.c:5`; 142 on 1.29), so a world saved by 1.30 cannot be loaded by a 1.29 server — back up
`storage_1` before the first 1.30 boot. Delegate stream chronology to `dayz-persistence`; the Fence attachment path
is this skill's concern.

R9 extras for 1.30:

8. Confirm you are editing `BaseBuildingBase` (3 ints, 1 bit, id 1..93) or `Rebuilding` (10 ints, 2 bits), never both on one class.
9. If the entity attaches `CombinationLock`, `OnStoreLoad` handles `version >= 143`.
10. `DigitalCodeLock` / `CodeLockComponent` persist `m_LockPIN`, `m_IsLocked`, `m_DoorIndex` (`4_world/classes/codelockcomponent.c:257-262`).
11. Decay: since 1.30 a part whose config sets `can_part_decay` (or `can_decay_general`) loses health over time even
    on a `BaseBuildingBase`, clamped at `part_decay_threshold` × max health, so decay alone never ruins it
    (`4_world/classes/constructionbase.c:307-331`); it is skipped when `cfggameplay.json`
    `GeneralData.disableBaseDecay` is true. Health is not part of the bitmask stream.
