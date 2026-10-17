# Underground triggers: schema and recipes (DayZ 1.30)

> Target: DayZ 1.30 stable (1.30.164078). Labels: ../../_shared/dayz-version-policy.md.

Citations are `P:\scripts\…` (1.30.164078), re-opened on 2026-10-15; a short file name
repeats the full path given earlier, and a bare `:line` refers to the file cited just
before it in the same paragraph or list, else to `undergroundarealoader.c`.
`1.29: O:\…` = the unpacked 1.29 client.
`source_verified` except what is marked `[DESIGN]` or unverified.

## Schema of `cfgundergroundtriggers.json`

[EXACT][CLAIM-UG-TRIGGER-JSON-130] Root `{ "Triggers": [ … ] }`
(`P:\scripts\3_game\undergroundarealoader.c:1-3`). Each trigger
(`JsonUndergroundAreaTriggerData`, `:69-100`):

| Field | Type | Purpose |
|---|---|---|
| `Comment` | string | freeform note (new in 1.30; a 1.29 JSON without it loads unchanged) |
| `CustomSpawn` | bool | not spawned at boot: created by parent object (see §Triggers parented to an object) |
| `Tag` | string | link to Object Spawner entry or manual spawn |
| `ParentNetworkId` | int[2] | link to map object by its network id |
| `Position`, `Orientation`, `Size` | float[3] | trigger box |
| `EyeAccommodation` | float | eye adaptation; `1.0` without breadcrumbs gives `OUTER` type |
| `InterpolationSpeed` | float | transition speed |
| `UseLinePointFade` | bool | simple fade between `Breadcrumbs` points |
| `AmbientSoundType` | string | ambient type for sound controller |
| `AmbientSoundSet` | string | manual ambient (Livonia uses `Underground_SoundSet`) |
| `Breadcrumbs` | array | transition points; if any present, type is `TRANSITIONING` (up to 32) |

Each breadcrumb (`JsonUndergroundAreaBreadcrumb`, `:36-51`): `Position` (float[3]),
`EyeAccommodation` (float), `UseRaycast` (bool), `Radius` (float; Livonia uses `-1`),
`LightLerp` (bool, only with `UseLinePointFade`), `Comment` (new in 1.30, `:48`) and
`ExternalValueController` (`Type` + `Params`; for example `BreadcrumbDoorStateController`,
taking selection name of a door, `:53-67`; read in
`P:\scripts\4_world\classes\undergroundhandlerclient.c:114-118`; Sakhal uses it 4 times).

Trigger type is decided by
`P:\scripts\4_world\entities\scriptedentities\triggers\undergroundtrigger.c:100-118`:

- with breadcrumbs, `TRANSITIONING` (more than 32 only logs `ErrorEx`, `:102-105`);
- without them, `EyeAccommodation == 1.0` gives `OUTER`;
- any other value gives `INNER`.

Player presence comes from type (`TranslateTriggerTypeToPresence`, `:190-203`, new in
1.30; before 1.30 the same mapping was inline in
`UndergroundHandlerClient.SetUndergroundPresence`, now `undergroundhandlerclient.c:464-475`).

## Loading and synchronization

1. **File.** `UndergroundAreaLoader.GetData` first searches
   `$mission:cfgundergroundtriggers.json`; if missing, warns in RPT and tries
   `dz/worlds/<world>/ce/cfgundergroundtriggers.json` (`:105-125`).
2. **Startup.** Server creates an `UndergroundTriggerCarrier` per trigger without
   `CustomSpawn` (`SpawnAllTriggerCarriers`, `:135-154`, called in
   `P:\scripts\5_mission\mission\missionserver.c:91`, `MissionServer.OnInit`).
3. **Synchronization.** Upon player connection, server sends entire JSON via
   `RPC_UNDERGROUND_SYNC` (`:174-177`; `missionserver.c:342,363`) and client stores it
   in `m_JsonData` (`:181-193`). Client does not need file locally.
4. **Client trigger.** The carrier's `OnVariablesSynchronized` spawns the real
   `UndergroundTrigger` locally (`ECE_LOCAL`) from `m_JsonData.Triggers[m_TriggerIndex]`
   (`undergroundtrigger.c:23-32,44-69`), retrying every 100 ms until the JSON arrived.
5. **Local.** In local game with `DIAG_DEVELOPER` (DayZDiag offline), client loads and
   creates triggers itself (`P:\scripts\5_mission\mission\missiongameplay.c:111-118`).

Cap: `m_TriggerIndex` synchronizes in `-1..4095` (`undergroundtrigger.c:10`), so
a JSON supports up to 4096 triggers. Before 1.30: `-1..255`, i.e. 256
(1.29: `O:\…\undergroundtrigger.c:10`). Vanilla sizes on 1.30: Chernarus 0 triggers,
Livonia 8, Sakhal 30 (`P:\DZ\worlds\<world>\ce\cfgundergroundtriggers.json`), Nasdara 1076
(stable install `nasdara\addons\worlds_nasdara_ce.pbo`, read 2026-10-15).

## Triggers parented to an object

Two ways, both with `CustomSpawn = true`:

- **Map object.** `ParentNetworkId` with network id of object; object itself
  calls `JsonUndergroundTriggers.SpawnParentedTriggers(this)` (`:5-22`). Done by
  `Land_WarheadStorage_Main` (`P:\scripts\4_world\entities\building\underground\land_warheadstorage_main.c:72`)
  and `Land_WarheadStorage_Bunker_Facility` (`land_warheadstorage_bunker_facility.c:16`).
  Sakhal is the only vanilla map with such entries (4, `ParentNetworkId` at
  `P:\DZ\worlds\sakhal\ce\cfgundergroundtriggers.json:186,220,341,366`).
- **Object placed by Object Spawner.**
  - Spawner entry (`ITEM_SpawnerObject`: `name`, `pos`, `ypr`, `scale`,
    `enableCEPersistency`, `customString`; `P:\scripts\3_game\objectspawner.c:100-108`) carries
    `"customString": "undergroundTriggerTag=TAG"`.
  - Spawner calls `object.OnSpawnByObjectSpawner(item)` (`objectspawner.c:63`).
  - Class iterates `CustomSpawn` triggers and creates those matching that `Tag`
    (`land_warheadstorage_bunker_facility.c:62-93`); `customString` accepts multiple
    pairs separated by `;`.
  - That logic lives in Sakhal classes, not in a common base: a mod class
    must write it. No vanilla map JSON uses `Tag` (vanilla `objectSpawnersArr` are empty).
  - All of this already worked the same way in 1.29 (`land_warheadstorage_bunker_facility.c`
    is identical; `objectspawner.c` differs only at `:44,48`, see `SKILL.md` §Authoring workflow).

Unverified: whether `Position` of parented trigger is world or relative to parent. The
code creates carrier at `data.GetPosition()` and then calls `SetParent(parent)`
(`undergroundarealoader.c:24-33`); how that transforms position has not been measured.
Hint: Sakhal's parented entries hold map-scale coordinates (e.g. `[2582.6, 20.0, 5976.4]`,
`P:\DZ\worlds\sakhal\ce\cfgundergroundtriggers.json:188`), which suggests world space.

## Recipe [DESIGN]: mod bunker placed with Object Spawner

Untested. Follows vanilla Sakhal pattern.

1. **Trigger in mission JSON**, with `"CustomSpawn": true`, a unique `Tag` and the
   interior box. For a dark zone without transition: no breadcrumbs and
   `EyeAccommodation` other than `1`, which yields `INNER`.
2. **Object Spawner entry** with `"customString": "undergroundTriggerTag=<Tag>"`.
3. **Bunker script class.** Override `OnSpawnByObjectSpawner(ITEM_SpawnerObject item)`
   with same iteration as `land_warheadstorage_bunker_facility.c:62-93`, calling
   `JsonUndergroundTriggers.SpawnTriggerCarrier(this, index, triggerData)`.
4. **Hole.** If bunker opens to terrain, hole is also needed
   (`CfgWorlds >> <world> >> Holes`, see `SKILL.md`).

A mod cannot add its own triggers file without touching loader: `GetData`
reads a single file. A `modded class UndergroundAreaLoader` adding mod
triggers to `m_JsonData` before `SyncDataSend` is the obvious path; not tested.
Keep the indices consistent: the carrier syncs only an index, which the client resolves
in `m_JsonData` (`undergroundtrigger.c:52-54`), but `SpawnParentedTriggers` and the
Object-Spawner path take their indices from a fresh `GetData()` file read
(`undergroundarealoader.c:10`, `land_warheadstorage_bunker_facility.c:69`). Append mod
triggers **after** the file's own entries, or make `GetData()` return the merged list.

## Presence and restrictions

See `SKILL.md` §Underground presence. In summary: client calculates presence, sends it to
the server (`INPUT_UDT_UNDERGROUND_SYNC`, new in 1.30; before 1.30 the server always saw
`NONE`), and inside a trigger `CanPlaceItem` blocks `disallowedTypesInUnderground`.
