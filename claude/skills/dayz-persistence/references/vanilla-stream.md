# Vanilla Entity Stream Contract

## Scope

Use this contract only when the mod owns both the data and the entity, the data
is naturally part of that entity's save stream, uninstall survival is not
required, and outside-game inspection is not required. Otherwise return to the
router in `../SKILL.md`.

The engine hooks are `EntityAI.OnStoreSave(ParamsWriteContext)` and
`EntityAI.OnStoreLoad(ParamsReadContext, int)` returning `bool`
(`VANILLA/3_game/entities/entityai.c:2910`; `VANILLA/3_game/entities/entityai.c:2978`; `VANILLA/` = `P:\scripts`,
1.30.164078). The stream has no field names or
random access: correctness is the exact sequence of typed writes and reads.

## Normative write/load order

A subclass writes in this order:

1. Call `super.OnStoreSave(ctx)` so the base class owns the prefix.
2. Write every subclass field in one declared order and with one declared type.
3. If the subclass has an independent mod schema, write its own header inside
   its portion of the stream; do not reinterpret the hook's build version.

A subclass loads in this order:

1. Call `super.OnStoreLoad(ctx, version)` and return `false` if it returns
   `false`.
2. Read subclass fields sequentially in the exact save order and types.
3. Check the boolean result of every `ctx.Read` and return `false` on the first
   failure.
4. Stage values separately. Commit them to entity state only after every read
   and validation succeeds.

This propagation and fail-closed read behavior follows the vanilla contract at
`VANILLA/3_game/entities/entityai.c:2958-2975` (the documented example). A load that consumed some bytes
before failure is still a failed load; byte consumption does not authorize
partial state.

## Version unit

The `version` argument is the DayZ save/build version produced by
`g_Game.SaveVersion()` (`VANILLA/3_game/global/game.c:435`). On 1.30 a fresh save carries
`GAME_STORAGE_VERSION = 144` (`VANILLA/3_game/global/game.c:5`; before 1.30: 142). Vanilla subsystems
use storage-version gates and helpers at
`VANILLA/4_world/classes/playerstomach.c:255` (`OnStoreLoad` `:471`, `version >= 140` temperature read `:492`) and
`VANILLA/4_world/classes/playermodifiers/modifiersmanager.c:178` (`OnStoreLoad` `:273`). These build
checks describe engine compatibility; they do not version a mod release.

## Variable-width super trap

`EntityAI.OnStoreSave` conditionally writes eight energy-component fields when
`m_EM` exists and writes none when it does not
(`VANILLA/3_game/entities/entityai.c:2917-2948`; eight on 1.29 too — an older version of this
text said nine). Therefore the base prefix width is a runtime property.

A reader that seeks to a remembered offset, skips a fixed number of fields, or
assumes that `super` always consumed the energy fields is wrong. With the
component absent it reads the subclass's first value as a base value and then
fails or misinterprets later bytes. Let the real `super.OnStoreLoad` advance the
context by the width that the matching base save wrote; begin subclass reads at
the resulting sequential position.

This trap applies even when a fixture with the component present passes. Test
both component-present and component-absent streams against the same reader.

On 1.30 energy is no longer the only optional prefix. `EntityAI.OnStoreSave`
calls `GetConstructionBasic().HandleStoreSave` **before** the `m_EM` block
(`VANILLA/3_game/entities/entityai.c:2912-2914`); `OnStoreLoad`
consumes it only when `version >= 143` (`:2980-2984`). `ConstructionBasic`
writes nothing (`HandleStorageEvents` returns `false` at
`VANILLA/3_game/systems/construction_basic.c:29-32`). `Rebuilding`
writes eleven ints when present. `PlayerBase` reads its stats with the new
`PlayerStatsPCO_v130` layout (12 values) and appends `ThermalBiasHandler`
after the arrow manager with no version gate — two widths that 1.29 did not
have. Before 1.30: energy-or-nothing was the only optional prefix. Bodies:
[dayz-1-30-persistence.md](dayz-1-30-persistence.md).

## Failure and ownership boundaries

- A wrong type, truncated field, failed base load, failed subclass `ctx.Read`, or
  trailing-layout mismatch rejects the load and preserves the pre-load state.
- Never convert a failed read into defaults and then report success. Defaults
  are valid only for an explicitly classified fresh or legacy case.
- Saving and loading must call `super` in the same relative position. Reordering
  the subclass prefix changes the wire contract.
- The stream belongs to the entity. If the mod that appended bytes disappears,
  no remaining handler understands or re-emits those bytes. Use CF ModStorage
  when uninstall/reinstall survival is a requirement.

## Verification checklist

- Save and load order and types are identical.
- `super` is propagated before subclass fields.
- Every `ctx.Read` result is checked.
- State mutation happens only after the complete read succeeds.
- Both optional-component widths are exercised.
- The hook build version is not presented as the mod's format version.
- Uninstall behavior is declared rather than implied.
- (1.30) Construction-present and construction-absent EntityAI
  prefixes are both exercised; the PlayerBase thermal float and the 12-value stats block
  are present on 1.30 MP saves; CombinationLock `version >= 143` is exercised against a 142 fixture.

## Vanilla stream version gates on 1.30

Engine `version` cuts that matter on 1.30 (the hook argument from `g_Game.SaveVersion()`, not a mod
`storageVersion`); cites are `VANILLA/` = `P:\scripts` 1.30.164078:

| Engine `version` | Writer | Extra field | Load gate |
|---|---|---|---|
| `< 105` (legacy remnant) | `CombinationLock` (until removed) | attached-bool | still skipped when `version < 105` (`4_world/entities/itembase/combinationlock.c:160-167`) |
| `>= 140` | `EntityAI` variables / `PlayerStomach` temperature | existing 1.29 gates | `3_game/entities/entityai.c:3053-3058`; `4_world/classes/playerstomach.c:492` |
| `>= 143` | `CombinationLock` | `int m_CombinationInside` | `combinationlock.c:169-177` |
| `>= 143` | `EntityAI` construction handle | `Rebuilding` payload or nothing | `entityai.c:2980-2984` |
| `>= 144` | `Mag_MP18_Drum32Rnd` (new 1.30 class) | `float m_LastAnimPhase` | `4_world/entities/itembase/magazine/magazines.c:112` |
| (no gate: layout chosen by version) | `PlayerStatsPCO` | 12th stat `THERMAL_BIAS`; any record with `version` ≥ 130 is read with the v130 layout, including 1.29's 142 | `4_world/classes/playerstats/playerstatspco.c:29-55`, `:312-351`; `playerstats.c:115-128` |
| (ungated on 1.30 MP load) | `PlayerBase` | `float m_TemporaryResistanceTime` | `4_world/entities/manbase/playerbase.c:7520-7524`; `4_world/classes/thermalbiashandler.c:72-77` |

`BaseBuildingBase` subclass suffix is unchanged: `super`, three sync ints,
`m_HasBase` (`VANILLA/4_world/entities/itembase/basebuildingbase.c:420-429`).

New 1.30 entity (no 1.29 fixture): `DigitalCodeLock` writes
`m_LockPIN` (string), `m_IsLocked` (bool), `m_DoorIndex` (int) after `super`
(`VANILLA/4_world/entities/itembase/codelock.c:484-500`; `VANILLA/4_world/classes/codelockcomponent.c:257-277`).

Full `[EXACT]` copies:
[dayz-1-30-persistence.md](dayz-1-30-persistence.md).
