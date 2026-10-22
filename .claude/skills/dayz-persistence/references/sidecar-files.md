# Sidecar File Contract

## Scope and available primitives

Use a sidecar for state that is not owned by one entity, must be inspected or
repaired outside the game, or needs an explicit file-level recovery surface. The
file owns its format header and mod version.

DayZ exposes these relevant primitives:

- `FileExist`, `OpenFile`, `ReadFile`, and `CloseFile`
  (`VANILLA/1_core/proto/ensystem.c:397,417,425,443`).
- `FPrint`, `FGets`, and `MakeDirectory`
  (`VANILLA/1_core/proto/ensystem.c:462,501,525`).
- `DeleteFile` and `CopyFile`
  (`VANILLA/1_core/proto/ensystem.c:528,531`).

There is no DayZ rename or move primitive; a search for `Rename` or `MoveFile`
in the cited `1_core` surface returns no match
(`VANILLA/1_core/proto/ensystem.c`). A design that says rename, move, or atomic
replace is using an API DayZ does not provide.

For JSON, use
`JsonFileLoader<T>.LoadFile(string, out T, out string)` and check its `bool`
return before accepting the output; preserve or report `errorMessage` without
private paths or player identifiers
(`VANILLA/3_game/tools/jsonfileloader.c:7-40`). Do not use `JsonLoadFile`: it is
deprecated and returns no success signal
(`VANILLA/3_game/tools/jsonfileloader.c:99-133`).

## Load contract

1. If the destination cannot be opened, return failure with the original
   untouched and create nothing.
2. Read into staged bytes. A failed or short read returns failure and applies no
   state.
3. Parse and validate magic/header, version, schema, declared lengths, and the
   complete payload before applying state.
4. Invalid JSON, a foreign schema, trailing data, or truncation preserves the
   original as evidence and applies no partial values.

## Recoverable save flow

Use deterministic sibling names: `<dest>.tmp`, `<dest>.bak`, and
`<dest>.sha256`.

1. Serialize the complete new record in memory, including its own version
   header and lengths.
2. Write it to `<dest>.tmp` and close the handle. If writing fails, discard the
   incomplete fragment; the original remains intact.
3. Reopen `<dest>.tmp`, read it fully, parse it, and verify it reproduces the
   intended record. A completed candidate that fails this verification is kept
   as evidence.
4. If `<dest>` exists, copy it to `<dest>.bak`. If backup fails, stop before
   replacement; keep the original and verified temporary file.
5. Delete `<dest>`, then copy the verified `<dest>.tmp` to `<dest>`. If deletion
   fails, stop with the original, temporary file, and backup available. If the
   process stops after deletion or the copy fails, the destination is absent but
   the temporary file and backup make recovery deterministic.
6. Reopen and fully verify the copied destination against the intended bytes and
   schema. If it is truncated or invalid, delete the bad destination and retain
   `<dest>.tmp` and `<dest>.bak`.
7. Write the verified destination digest to `<dest>.sha256`. If this fails, keep
   the destination, temporary file, and backup as evidence; do not claim a clean
   save.
8. Delete `<dest>.tmp` only after destination verification and sidecar creation
   succeed. A completed candidate is never deleted on a verification failure.

The production sequence is temp -> verify -> backup -> delete -> copy -> verify
-> hash sidecar -> delete temp
(`LFVS_SOURCE/Scripts/4_World/LFV_FileStorage.c:28-45`). Post-copy verification
exists because `CopyFile` can produce an incomplete destination; compare the
copied bytes before removing the temporary source
(`LFVS_SOURCE/Scripts/4_World/LFV_FileStorage.c:998-1009,1281-1282`).

An incomplete fragment from a failed temp write is discardable. That is distinct
from a completed `.tmp` that reached verification: once complete, it is retained
until the post-copy verify and digest sidecar succeed.

## Atomicity honesty

Temp -> verify -> replace is not atomic in DayZ. The actual replace operation is
`DeleteFile(dest)` followed by `CopyFile(tmp, dest)`. Between those calls the
destination does not exist. A backup and retained temporary file make the window
recoverable; they do not make it atomic. Document the window and every recovery
artifact explicitly.

## Nine I/O boundaries

| Boundary | Injected failure | Required invariant |
|---|---|---|
| open (read) | Handle is `0` | Original intact; no new file is created. |
| read | Input is cut halfway | No partial state is applied. |
| parse | Invalid JSON or foreign schema | Original intact and preserved as evidence. |
| backup / rotate | Backup copy fails | Stop before replacement; original and `.tmp` remain. |
| temp-write | `FPrint` fails or storage is full | Incomplete temp is discardable; original remains intact. |
| temp-verify | Header or payload does not reproduce | Preserve the completed `.tmp`; original remains intact. |
| replace | Failure between destination delete and temp copy | Destination may be absent; `.tmp` and `.bak` are present and recoverable. |
| post-copy verify | Destination copy is truncated | Delete the bad destination; do not delete `.tmp`; retain backup. |
| orphan `.tmp` | Candidate is valid or truncated | Promote only verified content; never decide from mtime. |

Every boundary preserves either the original bytes or deterministic recovery
evidence. Returning failure without identifying which artifacts remain is not a
complete result.

## Orphan policy

Evaluate `<dest>.tmp` by content, never by modification time.

- If the candidate cannot be opened/read fully, has an invalid header, declares
  a length it does not contain, or otherwise fails schema verification, do not
  promote it. Preserve it as truncated evidence and leave the destination
  untouched.
- If the candidate verifies, back up an existing destination, perform the same
  delete-and-copy replacement, verify the copied destination, create the digest
  sidecar, and only then delete the orphan temporary file.
- If orphan promotion fails after destination deletion, retain the temporary
  file and backup. The same non-atomic window applies.

mtime says when a directory entry changed, not whether its bytes form a complete
record. It is never a promotion or deletion criterion.

## Verification checklist

- The file has an independent mod-format header.
- Every loader result and `errorMessage` is checked before state is applied.
- Reads are staged; parse or length failure applies nothing.
- Temp data is reopened and verified before backup or replacement.
- Backup failure stops before deleting the destination.
- The delete-copy window is documented as non-atomic.
- Post-copy verification compares complete bytes and schema.
- A bad copied destination is removed while `.tmp` and `.bak` remain.
- The digest sidecar is created before `.tmp` cleanup.
- Valid and truncated orphan candidates take different deterministic actions.
- No orphan decision uses mtime.
- (1.30) A `$mission:` `FileSerializer` overwrite is not this
  contract. See the bunker exception below.

## DayZ 1.30: enumerating sidecar files

History of `FindFile` enumeration on 1.30 (the declaration never changed, `VANILLA/1_core/proto/ensystem.c:520`):

- 1.30.164014 [reported: bzed, Linux server]: the `$profile:` / `$mission:` / `$storage:` / `$saves:`
  placeholders were ignored and the server's working directory was listed instead, with no error
  (BI ticket DZEXP-134).
- 1.30.164078 (measured on 1.30.164078, this machine's Windows server, 2026-10-15): `$profile:` and absolute
  paths were ignored, `..` did not work; only the server working folder and its subfolders were listed. A
  P2P board booted with zero lots and accounts and would have written fresh accounts over existing files.
- 1.30.164200 [reported: BI 1.30.164200 notes]: fixed — "searches through the requested file system and
  normalises any path given to the internal engine delimiter '/'" (DZEXP-134). Not re-measured here.

Consequences for this contract:

- Reading or writing a known file name through a placeholder (`OpenFile`, `FileExist`, `MakeDirectory`,
  `CopyFile`, `DeleteFile`, `JsonFileLoader`) kept working on every 1.30 build.
- Orphan scans that **enumerate** `.tmp`/`.bak` candidates must not trust `FindFile` blindly. Probe at
  runtime: write a known sentinel through the placeholder, enumerate, and use the listing only if the
  sentinel is in it. Otherwise fall back to a resolver that turns the placeholder into a path relative to the
  server folder, `CF.FindFileEx` (CF 1.5.9+), or an id/index file written next to the data (the pattern that
  worked on 164078: INT_P2P_Trading `INT_P2P_FileStore`). Open results through the placeholder path. Treat
  "no candidates" as unverified until the scan has seen the sentinel, and never delete, overwrite or re-create
  data because a listing came back empty.
- The probe switches the workaround off by itself once BI's fix holds on the running build; re-test a 164078
  workaround on 164200 before removing it. Build patterns with `/` (BI normalises the given path to `/`); do not
  assume either separator in returned names (unmeasured).
- `$storage:` was reported unusable before the first `MissionServer.OnUpdate` / `CallLater` tick after
  `OnMissionStart`, and on an instance's very first boot.
- `g_Game.GetMissionFolderPath()` returns `""` when the engine passes a forward-slash mission path:
  `DayZGame.SetMissionPath` splits only on `\` (`VANILLA/3_game/dayzgame.c:1206-1221`, verified
  statically). Derive a mission folder from `GetMissionPath()` (`:1223`) instead.

## DayZ 1.30: `$mission:BunkerBroadcastPersistenceStorage.bin`

Vanilla 1.30 adds a mission-level binary that is **not** a sidecar in this
contract's sense: no version header, no JSON, no `.tmp`/`.bak`, no post-write
verify. Path and types:

`VANILLA/4_world/classes/bunkerbroadcasthandler.c:16-82`

- `STORAGE_PATH = "$mission:BunkerBroadcastPersistenceStorage.bin"` (`:18`).
- `BunkerBroadcastHandlerPersistenceData.m_LastActiveBunkers` (`TIntStringMap`,
  bunker id → lock code) (`:11-14`).
- `BunkerBroadcastSchedulerPersistenceData.m_LastReportHour` /
  `m_LastReportMinute` (`:5-9`).
- `Save()` opens `FileMode.WRITE` and `Write`s both objects (`:73-81`).
- `Load()` opens `FileMode.READ`, `Read`s both, returns `true` if `Open`
  succeeded (`:56-68`). `Serializer.Read` returns `bool`
  (`VANILLA/1_core/proto/serializer.c:58`) and is not checked.

`DeleteFile` works only on `$profile:` and `$saves:`, and `CopyFile`'s destination must be
there (`VANILLA/1_core/proto/ensystem.c:527-531`). The recoverable
replace in this file cannot be implemented on `$mission:` with those
primitives.

The bunker scheduler persists across mission restarts in that `.bin`; it does not touch the
character/world entity stream. Before 1.30: this file did not exist.

[DESIGN] Do not copy this as a persistence template. If a mod needs
administrator-inspectable bunker-like state, keep this sidecar contract on
`$profile:` / `$saves:` with an explicit version header. If the data must
live next to the mission, document that `DeleteFile` cannot rotate it and
that a failed `Read` is undetectable in vanilla.

`JsonFileLoader<T>.LoadFile` / deprecated `JsonLoadFile` are unchanged on 1.30
(`VANILLA/3_game/tools/jsonfileloader.c:7-40`, `:99-133`).

Full `[EXACT]` copies:
[dayz-1-30-persistence.md](dayz-1-30-persistence.md).
