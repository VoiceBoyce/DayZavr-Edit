---
name: dayz-physics-engine
description: "Use when: física DayZ, dBody, collision layers, player walks through my object, action/cursor does not appear, EOnContact, TransportHit, thrown/rolling objects, ragdoll, .ragdoll, fall damage, PhysicsSetSimpleDeath, VehicleContactData, GetSurfaceInfo, offload physics / reduce server load. Targets DayZ 1.30. Not vehicle authoring: dayz-vehicles; not flight: dayz-aviation."
---

# DayZ Engine Physics (Enforce Script)

Engine-truth reference for rigid-body work in DayZ mods. The API claims below were first verified
against the decompiled vanilla v1.24 scripts and re-checked against 1.30.164078 (`P:\scripts`);
citations are `path:line` relative to `P:\scripts` (1.30) unless written `1.29: …`. The physics core
(`1_core/proto/enphysics.c`, `1_core/physics/*.c`, `3_game/global/dayzphysics.c`, `3_game/entities/pawn.c`,
`hand_actions.c`, `actionpushcar.c`) is byte-identical between 1.29 and 1.30; vehicle, item and player
files moved. When something common-sounding is missing here, check section "What does NOT exist"
before assuming it exists.

> Target: DayZ 1.30 stable (1.30.164200). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

## How to use this skill

The body covers the 90% working set. Three bundled references hold the full deep-dives — load them
only when the task needs that depth:

| Reference | Read it when |
|---|---|
| `references/fisica-engine-deep-dive.md` | full dBody/dGeom/dJoint signatures, raycast family tables, surfaces, drop-physics lifecycle details |
| `references/transport-netsync-vehiculos.md` | anything about why/how vehicles replicate, contact damage by momentum (`Transport.OnContact` in 1.30), Pawn/TransportOwnerState, push-action pattern |
| `references/dano-transporthit.md` | damage pipeline (MDF), EEHitBy chain, the complete TransportHit flow line-by-line, hitzones, armor reality |
| `references/dayz-1-30-ragdoll-and-fall.md` | 1.30: `.ragdoll` / `RagdollDef`, `PhysicsSetSimpleDeath`, unconscious wake-while-falling, fall-damage `CurveExp` |

The three deep-dives were written against v1.24/1.29 sources; each now opens with a 1.30 note listing what moved. The body of this file states 1.30.

## The 11 engine truths (prevent the classic bugs)

1. **A body only collides where it exists.** The player capsule (CCT) is simulated client-side
   (CCT API `3_game/human.c:1426-1452`; 1.29: `:1397-1418`). A rigid body created only on the server can never block a player —
   the client's CCT has nothing to collide with. This is architectural; no layer mask fixes it.
2. **The action cursor sees View Geometry, not physics.** Action targeting uses `RaycastRVProxy`
   with default `ObjIntersectView` (`4_world/classes/useractionscomponent/actiontargets.c:255`; 1.29: `:214-219`;
   default type at `3_game/global/dayzphysics.c:88`). No View Geometry LOD in the .p3d → no action, no
   admin-tool selection, regardless of any physics body. New in 1.30: liquid surfaces with no object are
   also scored action targets at utility 0.01 (`actiontargets.c:535-537`), and the `ActionTarget` constructor
   gained a trailing `string surfaceName = ""` (`actiontargets.c:125`; 1.29 had five parameters) with
   `GetSurfaceLiquidType()` at `:184`. A ≤1.29 call with five arguments still compiles; a subclass that
   overrides the constructor must match the new signature.
   *(Qualified 2026-10-03 from the 1.29 source, not measured; the same code is in 1.30: at a camera pitch of −45° or
   lower, targeting also takes the objects of a 30°, 3 m cone (`DayZPlayerUtils.GetEntitiesInCone`, `actiontargets.c:324-325`,
   `c_MaxTargetDistance = 3.0` / `c_ConeAngle = 30.0` at `:844-846`; 1.29: `:269-270`) and scores them by their distance to
   the cursor ray (`ComputeUtility`, `:490-532`), with no hit on them needed, so an item on
   the ground can become a target without any View Geometry hit. See `dayz-mcp-verify`, "Picking an item up proves
   nothing about its collision".)*
3. **`dSetInteractionLayer` is GLOBAL.** It edits the world's layer↔layer interaction matrix
   (`1_core/physics/physicsworld.c:14-21`); the first parameter only resolves the world. To stop two
   specific entities from colliding use the surgical pair-block instead:
   `dBlock b = dBodyCollisionBlock(entA, entB); ... dBodyRemoveBlock(world, b);` (`1_core/proto/enphysics.c:116-117`).
4. **Friction/restitution are material strings, not setters.** They come from the physics material
   assigned per-geometry (`PhysicsGeomDef.MaterialName`, e.g. `"material/default"` —
   `1_core/physics/physicsgeomdef.c:18-24`) and surface definitions (.bisurf / CfgSurfaces). There is
   no `dBodySetFriction`-style API (0 matches in all of scripts/). Measured (BenchRE 2026-08-26):
   .bisurf `restitution` does not translate into bounce on the native item path — SmallStone dropped
   from 5 m onto open-field terrain rebounds 0.02-10 mm (apparent e = 0.009-0.046, n=6).
5. **Transport replicates because it is a Pawn.** `Transport extends Pawn` with continuous
   `TransportOwnerState` (world transform + linear + angular velocity) streamed to all proxies
   (`3_game/vehicles/transport.c:9-30`, class at `:66`; `3_game/entities/pawn.c:20-24`). The `Pawn` base sits
   under `#ifdef FEATURE_NETWORK_RECONCILIATION` (`transport.c:9`, `#else class Transport extends EntityAI` at
   `:67-68`), and that symbol is `#define`d in `1_core/defines.c:64` in both 1.29 and 1.30. 1.30 adds
   `Motorbike extends Transport` (`3_game/vehicles/motorbike.c:30`) with its own `MotorbikeOwnerState`/`MotorbikeMove`. `ItemBase` is not a
   Pawn: clients get only occasional entity snapshots — never velocities. A free dynamic body on an
   ItemBase looks "synced" only at low latency/local tests.
6. **The native throw path runs on server AND owner client.** `HandActionThrow` guards with
   `GetInstanceType() != INSTANCETYPE_REMOTE` before `item.ThrowPhysically(player, force)`
   (`3_game/systems/inventory/hand_actions.c:77-81`). That is why thrown items collide for the
   thrower. Whether REMOTE clients get a local body is not visible in scripts — treat third-party
   collision as unverified until tested in-game.
7. **Drop/throw physics is temporary by design.** It is governed by a lifetime:
   `StopItemDynamicPhysics()` kills it via `SetDynamicPhysicsLifeTime(0.01)`
   (`4_world/entities/itembase.c:4534-4538`). A long-rolling object must renew/extend the lifetime or
   avoid depending on the drop path. Measured (BenchRE 2026-08-26): the default lifetime is exactly
   **15 s** (n=2, `dBodyIsDynamic` polled at 4 Hz flips at t=15.00); `SetDynamicPhysicsLifeTime(3600)`
   is honored (still dynamic past a 300 s watchdog).
8. **A player's EOnContact only reacts to Transport.** `DayZPlayerImplement.EOnContact` casts
   `Transport.Cast(other)` and ignores everything else (`4_world/entities/dayzplayerimplement.c:3958-3973`;
   1.29: `:3822-3836`). Because `Motorbike` is a `Transport`, motorbikes reach `RegisterTransportHit` too.
   A custom ItemBase that should hurt players must detect the contact itself and call
   `target.ProcessDirectDamage(...)` from its own `EOnContact`.
9. **Script impulses push corpses, not living players.** Vanilla applies `dBodyApplyImpulse` to the
   victim only when `IsDamageDestroyed()` (ragdoll) — `3_game/entities/entityai.c:4159-4164` (car), `:4178-4183`
   (motorbike, new in 1.30, `5 × velocity` with Y = 5), `:4216-4221` (generic); 1.29: `:4111-4115`. Living
   players are displaced by the contact solver itself (both bodies present + interacting layers), not
   by script.
10. **Config-driven animated geometry on a building is NOT a mover** (measured on 1.29: in game, DayZDiag 1.29.163709) [EXACT]: a standing player stays suspended while the piece descends; a moving player rides up but does not ride down — they stay up, can walk a few metres on "nothing", then fall; a parked car is clipped through by the rising piece; a running car rises a bit and drops. Any elevator or lift platform needs a script layer that carries the rider. (On the way down the client seems to keep the old collision for a while — hypothesis, not measured.)
11. **A platform that is a SEPARATE entity does carry riders** (measured on 1.29: in game, DayZDiag 1.29.163709, one client on localhost; SP-450; the script APIs involved — `SetPosition`, `PhysicsGetLinkedEntity`, the update queue — are unchanged in 1.30, only their line numbers moved) [EXACT]: a `House` whose config inherits `HouseNoDestruct` with `animPhysDetachSpeed=100`, moved with `SetPosition` every frame on the server AND on each client from the same clock (the server re-syncs the time every 0.25 s, the client extrapolates), lifted a standing player 8.9 m in 15 s (peak 0.93 m/s) without touching the player's position: within 5 cm on the server and 7.9 cm on the client, the client error being one frame of lag (correlation 0.89 with speed × frame time). The player must be linked (`PhysicsGetLinkedEntity() == platform`, `3_game/human.c:1434`; 1.29: `:1403`); after a teleport they need one native step before they link.
    - **Walking and jumping on it** (SP-452; the same platform, build and setup, four 8.9 m trips played by hand, run `8e032fd9`, the player's position logged at 20 Hz while the platform moved and 2 Hz while it stood: 1,448 samples on the server, 1,369 on the client) [EXACT][CLAIM-PHYS-PLATFORM-WALK-INGAME]:
      - *Walking during the travel keeps the link.* `link=1` in every sample until the player stepped off onto the deck at the end, and no slide in any sample; on each of the three trips walked, 96.0-97.8 % of the server samples and 98.4-99.6 % of the client samples stayed within 8 cm of the face (the descent counted without its two jumps).
      - *A jump keeps the link and is relative to the platform.* The two jumps, both on the way down, spent 0.42-0.47 s in the air with a peak 0.30-0.32 m above the face, `link=1` throughout, and landed on the platform. That is close to the vanilla jump on still ground: `StartCommand_Fall(2.6)` (`4_world/entities/dayzplayerimplementjumpclimb.c:97`) starts the fall at 2.6 m/s up (`StartCommand_Fall(float pYVelocity)`, `3_game/human.c:1501`; 1.29: `:1467`), a 0.345 m, 0.53 s arc by ballistics. The run does not tell whether the link carries the player in the air or the take-off inherits the platform's velocity: on these two jumps the two differ by about 2 cm at most, below what the trace resolves. It does rule out a jump that leaves the player at 2.6 m/s up in the world, neither carried nor inheriting: by the same ballistics its peak would be about 0.5-0.6 m above the face, which was descending at 0.60-0.83 m/s. No jump was made going up.
      - *Walking aboard links on its own.* A player who walked onto the stopped platform was linked with no preparatory step, about 8 s before it started: that step is only needed after a teleport. No sample read a fall when the player stepped off at the bottom (0.35 m down to the floor, logged at 2 Hz, so a drop that short can fall between two samples) or onto the deck at the top in about the last half second of the travel, with the platform within 3 cm of it (logged at 20 Hz).
      - *Short spikes.* The player read up to 22 cm off the face on the server and 12.6 cm on the client, for one sample (up to three in a row on the server), and was back within 5 cm at the next. The offset trails the travel (above the face going down, below it going up) except in one client sample. On the client the spikes follow long frames (correlation about 0.7 with speed × frame time; 53 ms frames at the spikes, 8 ms elsewhere); on the server the correlation is 0.31 and the cause is open.
      - Not measured: a jump while the platform rises, a vehicle with a driver aboard, the latency of a second client on another machine.
    - **Vehicles on that platform.** Asleep (`dBodyActive` `INACTIVE`) + `SetPosition` + `Transport.Synchronize()`: the server carries them but the client receives nothing until they wake — do not use. Awake only (`ALWAYS_ACTIVE`): a sedan rides well, but a `CarScript` helicopter with its own flight model bounces up to 24 cm and slides 31 cm. Awake with `SetVelocity` = platform velocity + 4 × position error, every frame: the sedan (with and without wheels) and the helicopter ride within 4.6 cm on the server and 11.5 cm on the client, and stop exactly when released.
    - **The frame hook.** `EOnFrame` never reaches a `House` instance, even with `SetEventMask(EntityEvent.FRAME)` (measured `fe=0`); the `GetGame().GetUpdateQueue(CALL_CATEGORY_GAMEPLAY)` invoker runs every frame on server and client (`3_game/dayzgame.c:3080-3082`; 1.29: `:3035-3038`).
    - **A script can walk a client-controlled player only with the overrides on both sides, every move.** In 1.29 multiplayer with reconciliation, `OverrideMovementSpeed`/`OverrideMovementAngle` (`3_game/human.c:253`, `:256`; 1.29: `:234-237`) moved the player 0.24-0.40 m in 1.5 s when set on the client only, and 0.00 m when also set once on the server by RPC. What walks it (DayZDiag 1.29, one client on localhost, spike run 0db321a8; dayz-mcp `player_move`, #191, merged 2026-10-01): the owner sets both overrides `ENABLED` on every command tick and sends a `ScriptInputUserData` request; the server applies the same two overrides `ENABLED` in every `ConsumeMove` after `super` and in its `CommandHandler`, then `DISABLED` at the end. Owner and server walked 0.76 m per 0.5 s (about 1.52 m/s) and stopped at the same position. Script fields added to `PlayerBaseMove` never reached the server. Reference implementation: dayz-mcp `addon/scripts/4_World/MCP_PlayerMove.c`.
    - **A Roadway face of another model across the travel stops the ride down** (SecretRock elevator, measured on 1.29: in game, DayZDiag 1.29.163709, 2026-10-02/03) [EXACT][CLAIM-PHYS-ROADWAY-ACROSS-TRAVEL]: an elevator cabin of this kind (a `HouseNoDestruct` moved with `SetPosition` on the update queue) carried a player up through its stops but not down: going down, it left them standing at the stop's floor height, 18.59 or 12.78 m, and went on without them. `player_trace` read the building's interior model as the floor, no linked entity and no fall. That model's collision slabs had the shaft cut out, but its Roadway LOD did not: the project counted 26 Roadway triangles at 12.78 and 16 at 18.59 inside the shaft's footprint, the faces the rising cabin had carried the player through. With the shaft cut out of that Roadway too (per the project, the build changed only that model, and in it only those Roadway polygons), the player rode down 2 → 1 → 0, and on a later build a 2 → 0 ride stayed linked to the cabin in all 1,033 samples, none falling. So a character stands on a Roadway face where no Geometry is, and no `RaycastRV` probe can target it: `ObjIntersect` offers Fire, View, Geom, IFire and None, not Roadway (`3_game/constants.c:31-38`, same in 1.29). Before shipping a lift, cabin or sliding piece, check offline that no Roadway face of any other model lies in the volume its collision sweeps (the piece's own Roadway moves with it: the cabin has one, and the project's gate leaves the cabin out). The project's gate swept the cabin's Geometry, shrunk 1 cm, from stop 0 to stop 2: it failed the old models on 25 faces and passed the fixed ones with 0. Not measured: a `RayCastBullet` with `PhxInteractionLayers.ROADWAY` against a model's Roadway LOD; one cabin in one building.

## API quick map (verified signatures)

### Create / destroy (`1_core/proto/enphysics.c`)

```c
proto bool dBodyCreateStaticEx (notnull IEntity ent, PhysicsGeomDef geoms[]);                        // :38
proto bool dBodyCreateGhostEx  (notnull IEntity ent, PhysicsGeomDef geoms[]);                        // :39
proto bool dBodyCreateDynamicEx(notnull IEntity ent, vector centerOfMass, float mass, PhysicsGeomDef geoms[]); // :51
proto native void dBodyDestroy(notnull IEntity ent);                                                 // :54
proto native bool dBodyIsSet(notnull IEntity ent);                                                   // :57
```
- `PhysicsGeomDef(string name, dGeom geom, string materialName, int layerMask)` + public `Frame[4]`,
  `ParentNode` (`1_core/physics/physicsgeomdef.c:9-26`).
- Geoms: `dGeomCreateBox(size)` :186, `dGeomCreateSphere(radius)` :189, `dGeomCreateCapsule` :192,
  `dGeomCreateCylinder` :195, `dGeomDestroy` :198.
- `dBodyCreateDynamic/Static` (no `Ex`) exist only under `#ifdef GAME_TEMPLATE`
  (`2_gamelib/entities/scriptmodel.c:31,35`) — they do not compile in DayZ. The wrapper
  `Physics.CreateDynamic(ent, mass, layerMask)` builds geometry from the entity's .p3d
  (`1_core/physics/physics.c:189`).
- Recreate pattern: always `if (dBodyIsSet(this)) dBodyDestroy(this);` before re-creating.

### State, damping, sleep, CCD

```c
dBodySetInteractionLayer(ent, mask)            // :59  (per body)   dBodyGetInteractionLayer :60
dBodyActive(ent, ActiveState.X)                // :64  INACTIVE | ACTIVE | ALWAYS_ACTIVE (activestate.c:9-17)
dBodyDynamic(ent, bool)                        // :65
dBodyEnableGravity(ent, bool)                  // :69  bool only — no vector
dBodySetDamping(ent, linear, angular)          // :70
dBodySetSleepingTreshold(body, lin, ang)       // :71
dBodyEnableCCD(body, maxMotion, castRadius)    // :83  (-1 disables) anti-tunneling
dBodySetLinearFactor(body, vector)             // :87  zero an axis => 2D physics
dBodyGetMass / dBodySetMass                    // :122-123
```

### Impulses, forces, velocities

```c
dBodyApplyImpulse(body, impulse)               // :141
dBodyApplyImpulseAt(body, impulse, worldPos)   // :136  off-center => natural torque/roll
dBodyApplyForce / dBodyApplyForceAt            // :146 / :151
dBodyApplyTorque / dBodyApplyTorqueImpulse     // :153 / :125
GetVelocity(ent) / SetVelocity(ent, v)         // :104 / :111  global functions (work on Man too)
dBodyGetAngularVelocity / dBodySetAngularVelocity  // :158 / :165  rad/s per axis, NOT yaw/pitch/roll
dBodyGetVelocityAt(body, worldPos)             // :177
dBodySetTargetMatrix(body, matrix, timeslice)  // :170  kinematic move
dBodyGetKineticEnergy(body)                    // :175
```
Wrapper-only extras: `Physics.ClearForces/GetTotalForce/GetTotalTorque/SetResponseIndex`
(`physics.c:106-115`); constants `Physics.STANDARD_GRAVITY(9.81)/VGravity/KMH2MS` (`physics.c:18-21`).

### Interaction layers (`3_game/global/dayzphysics.c:1-43`)

`PhxInteractionLayers` (bit order): NOCOLLISION, DEFAULT, BUILDING, CHARACTER, VEHICLE, DYNAMICITEM,
DYNAMICITEM_NOCHAR, ROADWAY, VEHICLE_NOTERRAIN, CHARACTER_NO_GRAVITY, RAGDOLL_NO_CHARACTER/FIREGEOM,
DOOR, RAGDOLL, WATERLAYER, TERRAIN, GHOST, WORLDBOUNDS, FENCE, AI, AI_NO_COLLISION, AI_COMPLEX,
TINYCAPSULE, TRIGGER, TRIGGER_NOTERRAIN, ITEM_SMALL, ITEM_LARGE, CAMERA, TEMP.

Runtime query pattern (vanilla, `3_game/vehicles/transport.c:790-791`; 1.29: `:556-557`):
```c
int layer = dBodyGetInteractionLayer(obj);
bool blocksPlayer = dGetInteractionLayer(this, PhxInteractionLayers.CHARACTER, layer);
```
Use the same query to log whether `CHARACTER × DYNAMICITEM` is active when debugging "walks through".

### Raycast / overlap families (`3_game/global/dayzphysics.c:123-230`)

Two separate worlds — pick the right one:

| Family | Sees | Use for |
|---|---|---|
| `RaycastRV` :199 / `RaycastRVProxy` :208 | RV geometries via `ObjIntersect*`: Fire(0), View(1), Geom(2), IFire(3), None(4) (:66-71) | cursor/action targeting, hit surfaces, melee aim |
| `RayCastBullet` :211, `SphereCastBullet` :213, `*OverlapBullet` :216-228 | Bullet physics world, filtered by `PhxInteractionLayers` | physical LOS, ground probes, area queries on bodies |

`CollisionFlags` (FIRSTCONTACT, NEARESTCONTACT, ONLYSTATIC, ONLYDYNAMIC, ONLYWATER, ALLOBJECTS) —
`1_core/proto/endebug.c:140-148`. Overlap callback: `CollisionOverlapCallback.OnContact(IEntity, Contact)`
(`dayzphysics.c:115-121`). Action-cursor ground fallback uses `RayCastBullet` with
`ROADWAY|TERRAIN|WATERLAYER` (`4_world/classes/useractionscomponent/actiontargets.c:383-384`; 1.29: `:329-331`).

### Contacts

`sealed class Contact` (`1_core/physics/contact.c:9-50`): `Impulse` (:21), `Normal`, `Position`,
`PenetrationDepth`, `RelativeVelocityBefore/After`, `GetNormalImpulse()` (:47). Receive it via
`SetEventMask(EntityEvent.CONTACT)` → `override void EOnContact(IEntity other, Contact extra)`
(`1_core/proto/enentity.c:213`). Vanilla measures impact magnitude with
`extra.RelativeVelocityBefore.Length()` (`3_game/entities/inventoryitem.c:202`, threshold 0.3,
throttled 0.33 s) — not with `Impulse`. Vehicle contacts arrive through `Transport.OnContact(string zoneName,
vector localPos, IEntity other, Contact data)`, which in 1.30 has a body on `Transport` itself
(`3_game/vehicles/transport.c:318-346`): server-only, it caches one `VehicleContactData` (`SetData(localPos,
other, momentumDelta)`, class at `:1153-1165`) per frame in `m_ContactCache`, and `CarScript`/`MotorbikeScript`
consume it in `EOnPostSimulate` → `CheckContactCache()` (`4_world/entities/vehicles/carscript.c:807-826`,
`:1279`). The engine can call it many times per frame ("Can be called very frequently in one frame",
`4_world/entities/vehicles/boatscript.c:425`): buffer contacts, process once per tick.
Before 1.30: the body lived in `CarScript.OnContact` (1.29: `carscript.c:1453-1480`) and cached
`CarContactData(localPos, other, impulse)` with public fields `localPos`/`other`/`impulse`. In 1.30
`CarContactData` is an empty subclass of `VehicleContactData` kept "for backward compatibility"
(`carscript.c:52-54`) whose fields are `m_LocalPos`/`m_Other`/`m_Impulse`; a ≤1.29 mod that calls
`new CarContactData(a, b, c)` or reads `.impulse` must be ported.

### Native dynamic-item path (drop/throw)

```c
// 3_game/entities/inventoryitem.c
proto native void EnableCollisionsWithCharacter(bool state);   // :21
proto native void ThrowPhysically(DayZPlayer player, vector force, bool collideWithCharacters = true); // :26
// 3_game/entities/object.c
proto native void CreateDynamicPhysics(int interactionLayers); // :456 (1.29: :462)  (never called by vanilla script)
proto native void EnableDynamicCCD(bool state);                // :457 (1.29: :463)
proto native void SetDynamicPhysicsLifeTime(float lifeTime);   // :458 (1.29: :464)
```
Lifecycle: throw (`3_game/systems/inventory/hand_actions.c:62-88`, server+owner) · inventory dump uses
`ThrowPhysically(null, force, false)` (`4_world/static/miscgameplayfunctions.c:1323-1347`; 1.29: `:1164-1220`), and since
1.30.164200 `ThrowAllItemsInInventoryEx` passes `-GetVelocity(parent)` as that force whatever its own `force` argument
says (`:1295`; see §DayZ 1.30.164200 changes) · admin
spawn-with-gravity uses `item.ThrowPhysically(null, "0 0 0")` server-side
(`4_world/plugins/pluginbase/plugindeveloper.c:430,553,593`; 1.29: `:381,504`) · re-config hook
`override void OnCreatePhysics()` → `RefreshPhysics()` (`4_world/entities/itembase.c:1150-1155`; 1.29: `:1224-1227`; real
`RefreshPhysics` overrides in tentbase.c, fireplace.c, batterycharger.c, hescobox.c, kitbase.c). The `DYNAMICITEM` / `DYNAMICITEM_NOCHAR` pair maps to `collideWithCharacters`
by naming (inference — internal assignment not script-visible).

### Player CCT (`3_game/human.c`)

`PhysicsIsFalling(bool pValidate)` :1428 (comment `:1426-1427`: also "returns true if the ragdoll is moving greater than 1m/s", new wording in 1.30; 1.29: :1397), `PhysicsGetFloorEntity` :1431, `PhysicsGetLinkedEntity` :1434,
`PhysicsGetVelocity` :1441, `PhysicsEnableGravity` :1443, `PhysicsIsSolid`/`PhysicsSetSolid` :1445-1446,
`PhysicsSetRagdoll` :1451 (1.29: :1418, whose comment about the `RAGDOLL` layers at 1.29 `:1417` is gone),
`PhysicsSetSimpleDeath` :1449 (new; comment `:1448` "Old system for handling player death state, was previously handled by PhysicsSetRagdoll"), `PhysicsIsRagdoll` :1452 (new; also `HumanCommandUnconscious.IsRagdoll` at `:647`, new),
`CheckFreeSpace` :1381, `CollisionMoveTest` :1387, `LinkToLocalSpaceOf` :1391 (1.29: :1354, :1357, :1361).

**Lifting the player from a `HumanCommandScript` needs the controller's gravity off** (measured on 1.30 Exp 164014: DayZDiag, LFSkateboard test R1c, 2026-10-06, server and one client on one machine; the APIs below re-read on 1.30.164078) [EXACT][CLAIM-PHYS-SCRIPTCMD-GRAVITY-130]. A skateboard ride command integrated its own ollie (`dy = vy·dt − ½g·dt²` each tick, vy 2.69 m/s at take-off) and passed `dy` as the y of `PrePhys_SetTranslation`, which is local space (`3_game/human.c:1313`). In `PostPhysUpdate` it compared `PostPhys_GetPosition` with the integrated height and, past 3 cm, pinned the height with `PostPhys_SetPosition` on every air tick (world space, `human.c:1325-1327`).
- *Gravity on, the default:* the body did not rise. The first air tick ended 0.088 m under the integrated height on the server and on the owner client, which is that tick's whole 0.084 m step lost. 0.2 s into the flight, with the pin already on, the server read the body 1 cm under its take-off height, where the integration asked for about +0.34 m.
- *`Human.PhysicsEnableGravity(false)` from take-off to landing* (`human.c:1443`), the only script change in the next build: the body followed the translation. The server never needed the pin (every air tick within 3 cm). On the owner client the root peaked at 0.996 and 1.001 × vy²/2g in the two landed flights, sampled at about 10 Hz; in the second one the client had switched to the pin after reading the body 0.079 m above the integration on its second air tick.
- *`PhysicsEnableGravity(true)` at the landing*, and in `OnDeactivate` for a ride that ends in the air: after the first landing the rider rolled down a slope again, 2.6 m of descent over the next 11 s.

Vanilla calls `PhysicsEnableGravity` once, with `true`, when a vehicle death's simulation ends (`4_world/entities/dayzplayerimplement.c:32`), so a command that switches the gravity off owns switching it back on every exit path. Not measured: a remote observer, flights longer than the 0.6 s these lasted, and fall damage after a flight with the gravity off.

### Joints

`dJointCreateHinge/Hinge2/Slider/BallSocket/Fixed/ConeTwist/6DOF/6DOFSpring(..., bool block, float breakThreshold)`
+ per-type setters (`enphysics.c:212-270`); break event `EOnJointBreak` (`1_core/proto/enentity.c:207`).

### Surfaces

`SurfaceInfo` (`3_game/surfaceinfo.c:8-71`): GetByName/GetByFile, roughness/dustness/penetrability,
IsLiquid/IsSolid, step/wheel particle ids, and new in 1.30 the sound-set and effect getters (`GetStepSoundSetName`,
`GetWheelContactParticleId`, `GetVehicleDustParticleId`, `GetDustKickupEffectId`, … `:50-70`). Detection on 1.30:
`CGame.GetSurfaceInfoOn(x, z, out height)`, `GetSurfaceInfoUnderObject(obj)`, `GetSurfaceInfoUnderObjectByBone(obj, boneType)`
return a `SurfaceInfo` (`3_game/global/game.c:1171-1173`, new); `GetSurface/SurfaceY/SurfaceGetType/SurfaceGetType3D/SurfaceGetNormal`
remain (`:1169-1188`). `SurfaceUnderObject/Ex/ByBone(…, out string type, …)` are still declared without an `[Obsolete]` tag
(`:1184-1186`), but vanilla's own wrappers now go through `GetSurfaceInfoUnderObject` (`game.c:1273-1291`) and
`DayZPlayerImplement.GetSurfaceType` is `[Obsolete("1.30: uses CGame GetSurfaceInfo* methods instead")]`
(`4_world/entities/dayzplayerimplement.c:4121`); [reported: bzed guide] the 1.30 compiler flags `SurfaceUnderObject*`
as obsolete — prefer `GetSurfaceInfo*` in new code. Impact surface:
`DayZPhysics.GetHitSurfaceAndLiquid` (`dayzphysics.c:206`; used `inventoryitem.c:168-176`). No
friction getters — see truth #4.

## Recipes

**A. Custom dynamic sphere body (server-side baseline)**
```c
PhysicsGeomDef geoms[] = {PhysicsGeomDef("", dGeomCreateSphere(0.5), "material/default",
    PhxInteractionLayers.DYNAMICITEM)};
dBodyCreateDynamicEx(this, GetCenterOfMassOffset(), 80.0, geoms);
dBodySetInteractionLayer(this, PhxInteractionLayers.DYNAMICITEM);
dBodyActive(this, ActiveState.ALWAYS_ACTIVE);     // no mid-slope sleep
dBodySetDamping(this, 0.05, 0.2);
dBodyEnableCCD(this, 0.4, 0.45);                  // ~diameter*0.8, inner radius
```
Remember truth #1: created only server-side, players walk through it.

**B. Native path (recommended for items that must block/hit players)**
```c
// run on server AND owner client (replicate the hand_actions.c:77 guard):
item.ThrowPhysically(null, impulse, true);   // true => collides with CHARACTER
item.EnableDynamicCCD(true);
item.SetDynamicPhysicsLifeTime(3600);        // renew for long rolling [validate in-game]
```
Model requirements: Geometry LOD (collision), View Geometry (cursor — truth #2), FireGeometry (bullets).

**C. Contact damage to players (vehicle-parity, from the stone side)**
```c
void MyItem() { SetEventMask(EntityEvent.CONTACT); }
override void EOnContact(IEntity other, Contact extra)
{
    if (!g_Game.IsServer()) return;
    EntityAI target = EntityAI.Cast(other);
    if (target && target.IsAlive())
    {
        float speed = GetVelocity(this).Length();          // m/s
        if (speed > 0.5 && m_CanHit)                        // guard vs multi-contact per tick
        {
            m_CanHit = false;                               // reset via timer or target EEHitBy
            target.ProcessDirectDamage(DT_CUSTOM, this, "", "TransportHit", "0 0 0", speed);
        }
    }
}
```
`damageCoef` IS the velocity: real damage = base damage of ammo `"TransportHit"` × coef
(`RegisterTransportHit`, `3_game/entities/entityai.c:4134-4224`, with the new Motorbike branch at `:4166-4184`;
1.29: `:4089-4157`). The ammo lives in binary game data, not scripts. Vanilla resets its
one-hit guard in the victim's `EEHitBy` (`4_world/entities/dayzplayerimplement.c:1567`; 1.29: `:1551`). Corpse launch only:
`impulse = 40 * velocity; impulse[1] = 60; dBodyApplyImpulse(victim, impulse);` (car and generic branches; the
motorbike branch uses `5 * velocity`, Y = 5) gated by `IsDamageDestroyed()` (truth #9). Full pipeline: `references/dano-transporthit.md`.

**D. Push action (vanilla parity)**
`ActionPushCar` applies `dBodyApplyImpulseAt(car, impulse, car.ModelToWorld(car.GetEnginePos()))`
(`4_world/classes/useractionscomponent/actions/continuous/actionpushcar.c:52`). Off-center
application point gives natural roll. Stamina cost is one line: `EStaminaModifiers.PUSH_CAR` already
exists (`3_game/enums/estaminamodifiers.c:13`).

**E. Wake a sleeping body before impulses**
`dBodyActive(ent, ActiveState.ACTIVE); dBodyDynamic(ent, true);` then apply the impulse — impulses
on sleeping bodies are lost.

## Debugging physics

- Visual overlay works in retail builds: `Shape.CreateSphere(0x88FF0000, ShapeFlags.TRANSP|ShapeFlags.NOZBUFFER, pos, r)`
  and `Shape.CreateLines(...)` with `ShapeFlags.ONCE` for per-frame draws (`1_core/proto/endebug.c:114-230`;
  never keep a pointer to a ONCE shape).
- Log the layer matrix at runtime with the `dGetInteractionLayer` query pattern above.
- Count world bodies: `dGetNumDynamicBodies(world)` / `dGetDynamicBody(world, i)` (`enphysics.c:9-10`).
- Fast iteration: DayZDiag_x64 + `-filePatching` reloads scripts without PBO rebuild (no BattlEye, no
  signature checks); see the dayz-mod-workflow skill §6 for the full loop.

## What does NOT exist (verified: 0 matches in scripts/)

| Plausible-sounding API | Reality |
|---|---|
| `dBodySetFriction` / `SetRestitution` / `SetBounciness` | physics material string per geometry + .bisurf only |
| per-body gravity vector (`dBodySetGravity`) | only `dBodyEnableGravity(ent, bool)` and global `dSetGravity(world, g)` (`enphysics.c:17`) |
| `dBodySetVelocity` / `dBodyGetVelocity` | global `SetVelocity/GetVelocity` (`enphysics.c:104,111`) |
| `dBodyApplyAngularImpulse` | `dBodyApplyTorqueImpulse` (`enphysics.c:125`) |
| `AddForce` (Unity-style) | `dBodyApplyForce*` |
| `SetMaxLinearVelocity` clamp | clamp manually per tick |
| `PhxRaycast*` | the physics-world casts are the `*Bullet` family in `DayZPhysics` |
| `EOnSimulate` on CarScript | car physics is 100% native; script only calls Set* controls (no `EOnSimulate` override in `3_game/vehicles/` or `4_world/entities/vehicles/` on 1.30) |
| vanilla script call to `CreateDynamicPhysics` | declared but never called from script — no vanilla usage pattern |
| `Synchronize()` on EntityAI/ItemBase | Transport-only (`3_game/vehicles/transport.c:171-172`; 1.29: `:108-109`) |
| `CarScript.OnContact` override in vanilla | gone in 1.30: the contact cache lives in `Transport.OnContact` (`transport.c:318`); only `BoatScript` still overrides it (`boatscript.c:426`) |

## Sleep/active enum

`enum ActiveState { INACTIVE, ACTIVE, ALWAYS_ACTIVE }` (`1_core/physics/activestate.c:9-17`).
`ALWAYS_ACTIVE` prevents mid-slope sleep for objects that must keep simulating.

## Offloading physics from the server (checked 2026-09-26)

Gameplay physics cannot leave the authority; only cosmetic physics can.

- **No out-of-process channel for per-tick work.** Vanilla scripts expose no native-extension call
  (0 matches for `callExtension`, `LoadLibrary`, `DllImport` in the 1.29 tree and in `P:\scripts` 1.30.164078). The only
  network exit is `RestApi`: asynchronous with a callback (`3_game/http/restapi.c:103,123`) or
  thread-blocking `GET_now`/`POST_now` (`:106-108`, `:126-128`). Fine for slow, latency-tolerant work;
  never for a physics step.
- **Owner prediction does not remove server work.** The authority consumes and replays the owner's move
  (`3_game/entities/pawn.c:80-81`, `ConsumeMove` `:270-274`), and a native comparison runs as well, with
  the more severe result winning (`:261-265`). A script `CompareMove` that always returns `APPROVE`
  therefore cannot suppress corrections. Vanilla cars follow this under `NetworkMoveStrategy.PHYSICS`
  (`CarScript.IsServerOrOwner`, `4_world/entities/vehicles/carscript.c:2592-2601`; 1.29: `:3220-3230`).
- **Cosmetic physics can run per client at no server cost.** `ECE_LOCAL` creates a machine-local object
  (`3_game/ce/centraleconomy.c:25`; 1.29: `:24`); vanilla gives local objects collision with
  `ECE_LOCAL|ECE_CREATEPHYSICS` (`5_mission/gui/scriptconsoleitemstab.c:696`). Whether a local object
  accepts a dynamic body is unverified.
- **Measure before optimizing.** `EnProfiler` profiles script only (`1_core/proto/enprofiler.c:62-63`,
  `-profile`). For native cost, compare server CPU with and without the load on a capped server
  (`dayz-test-ingame/references/dayz-1-30-test-ingame.md` § Measuring server load). Measured on 1.30 Exp 164014:
  50 server-side dummy players sprinting did not show at 60 FPS and 100 did
  (`dayz-mcp-verify/references/dayz-1-30-mcp-verify.md`).

## Cross-skill pointers

- `enforce-script-reference` — language rules, RPC/SyncVars, config.cpp, action system basics.
- `dayz-p3d-audit` / `dayz-model-pipeline` — building the Geometry/ViewGeo/FireGeo LODs that physics
  and the cursor require.
- `dayz-mod-workflow` — implementation protocol + DayZDiag/filePatching fast loop.
- `dayz-sound-system` — impact/rolling audio driven from contact events (client-only rules).

## PhysicsSetRagdoll on a LIVE player — empirical evidence (added 2026-06-11)

Origin: LFSlidingFloor spike B, in-game test 2026-06-10 (measured on 1.29; script logs with complete telemetry). Updates previous expectation "no public usage on live players / probably does not simulate":

(1.30: ragdoll is no longer a fully opaque native blob — vanilla ships `RagdollDef` in `P:\DZ\characters\bodies\human.ragdoll`, bound in `SurvivorBase` `enfAnimSys` (`P:\DZ\characters\data\config.cpp:160`). `PhysicsSetRagdoll(bool)` is unchanged, but 1.30 added `PhysicsSetSimpleDeath` and the 1.30 ragdoll def may change mass and joint behaviour, so the live-ragdoll numbers below need re-measuring on 1.30. Format, editor and `PhysicsSetSimpleDeath`: `references/dayz-1-30-ragdoll-and-fall.md`.)

- **SERVER-SIDE DOES SIMULATE**: with (1) DisableSimulation(false) before toggle (parity with death flow, `4_world/entities/dayzplayerimplement.c:735`; 1.29: `:726`), (2) pre-wake `dBodyActive(p, ActiveState.ACTIVE)` + `dBodyDynamic(p, true)`, (3) `dBodyApplyImpulse(p, V*masa)` — dBodyGetMass returned actual player mass (87.5 kg) and impulse caught on first try (without need for SetVelocity). Body slid 77.6 m at 4-6.7 m/s following terrain (including uphills — extremely low effective friction).
- **OWNER CLIENT DOES NOT**: local avatar never ragdolls — remains standing and controllable (client-authoritative movement). Total server-owner desync. Live-ragdoll is only viable end-to-end with custom position sync (see LL-138).
- Toggle `PhysicsSetRagdoll(false)` does NOT rubber-band: entity stays exactly where body ended up (pos pre == post, verified).
- **DANGER get-up**: `StartCommand_Unconscious(0)` + `WakeUp` at 0.5 s left player server-side 40 m UNDER terrain, with void fall, real uncon and death. Vanilla anti-wake-early protection on 1.30: the 2 s guard `m_UnconsciousTime > 2` (`4_world/entities/manbase/playerbase.c:3420`) AND a refusal to wake while falling, `if (false == PhysicsIsFalling(false))` before `hcu.WakeUp()` (`:3423-3431`). Before 1.30 only the 2 s guard existed (1.29: `playerbase.c:3184-3192`). Custom 0.5 s wake paths that skip either guard still risk burying the body.

## Script bodies on vanilla items — BenchRE empirical evidence (added 2026-08-26)

Measured on 1.29: DayZDiag 1.29 server+client run, BenchRE mod build 0004 (the APIs involved are unchanged in 1.30; only line numbers moved). Evidence:
`C:\Users\<you>\dayz_re_scratch\bench_results\` (CSVs + raw logs); synthesis with 7
matrix questions in `C:\Users\<you>\dayz_re_scratch\physics_matrix.md` §6.

- **`Physics.CreateDynamic` / `CreateDynamicEx` / `CreateStaticEx` return falsy on
  vanilla InventoryItem** (SmallStone/WoodenStick spawned with `CreateObjectEx`): 7/7 attempts
  server-side. The NATIVE path on the same items works (`ThrowPhysically` +
  `SetDynamicPhysicsLifeTime` + `dBodyIsDynamic` read 2402 ticks). Items already possess a native
  body and the `Create*Ex` family does not adhere to them — coincides with its 0 gameplay uses in
  vanilla. A viable host for script bodies must lack own physics (unvalidated yet:
  custom entity style `scriptmodel.c`).
- **`DayZPhysics.GetHitSurfaceAndLiquid` does not name TERRAIN surfaces**: `RayCastBullet`
  over open field returns valid hit_pos, but the path requires an Object and terrain is not
  one (6/6 nameless probes). For terrain: `CGame.SurfaceGetType(x, z, out type)`
  (`3_game/global/game.c:1180`) / `SurfaceGetType3D` (`game.c:1183`), or on 1.30 `GetSurfaceInfoOn(x, z, out height)`
  (`game.c:1171`, returns a `SurfaceInfo`; not measured on terrain here).
- **MP client: `CreateObjectEx` without `ECE_LOCAL` returns null** (n=2). For client test
  local geometry add `ECE_LOCAL` (`3_game/ce/centraleconomy.c:25`; client pattern:
  `3_game/particles/particle.c:119`).

## Rules promoted from the lessons corpus (added 2026-07-27)

Promoted from `AI/20_Knowledge/lessons-learned.md` so that they arrive via trigger instead
of relying on someone remembering to look them up. Each rule cites its source `LL-NNN`;
the full entry lives there. Do not remove the citation: the index detects promotion by it.

- **LL-014** — For rolling bodies, use very low but non-zero linear and angular damping and adjust sleeping threshold. Do not minimize contact friction: lives in `.bisurf`; if it slides without rotating, raise it, and if it vibrates or does not stop, raise damping.

## DayZ 1.30 changes

Re-verified on 1.30.164078 (`P:\`); first written against 1.30 Exp 164014. The body above already states 1.30; this is the porting summary.

What changes:
- Ragdoll is data-driven. Vanilla ships `RagdollDef` with 11 bones (73.9 kg in total), capsules/sphere, `flesh.bisurf`, joints (`P:\DZ\characters\bodies\human.ragdoll:1-202`), bound to the player by `ragdoll="DZ/characters/bodies/human.ragdoll"` in `SurvivorBase` `enfAnimSys` (`P:\DZ\characters\data\config.cpp:160`; config key absent in 1.29). [CHANGELOG 1.30] Workbench adds a ragdoll editor.
- Death-state handling: the non-command death branch calls `PhysicsSetSimpleDeath(true)` (`4_world/entities/dayzplayerimplement.c:745`). Before 1.30 it called `PhysicsSetRagdoll(true)` (1.29: `dayzplayerimplement.c:736`). `PhysicsSetRagdoll` remains and `PhysicsIsRagdoll` is new (`3_game/human.c:1451-1452`).
- Unconscious ragdoll unhooks vehicle re-attach: `if (hcu && hcu.IsRagdoll()) m_TransportCache = null` (`4_world/entities/manbase/playerbase.c:3346-3350`).
- Fall damage is exponential `CurveExp` with much lower height gates (health from **2 m**, shock from **0 m**, broken legs from **3 m**; `4_world/entities/dayzplayerimplementfalldamage.c:26-32`, `:92-97`). `Randomize` no longer rolls a lethal 1.0 coef back down (`:160-163`). Detail in `references/dayz-1-30-ragdoll-and-fall.md`.
- Vehicles: `CarScript` contact handling moved up to `Transport` (`OnContact` body and `m_ContactCache` on `Transport`, `3_game/vehicles/transport.c:90`, `:318-346`; `CarScript`'s own `EEHitBy` override, 1.29 `carscript.c:531-537`, is gone and `Transport.EEHitBy`, `:144-151`, now also schedules the light update), so boats and the new `Motorbike` share it. `VehicleContactData` replaces the old `CarContactData` fields (`transport.c:1153-1165`; `CarContactData : VehicleContactData {}` kept for compatibility, `carscript.c:53`). Motorbikes have their own contact thresholds (`GameConstants.MOTORBIKES_CONTACT_DMG_*`, `3_game/constants.c:898-901`) and their own `RegisterTransportHit` branch (`entityai.c:4166-4184`). `CarFluid` is now `enum CarFluid : ETransportFluid {}` (`3_game/vehicles/car.c:18`, values at `transport.c:52-61`); `CarFluid.FUEL` still compiles (vanilla uses it, `carscript.c:850`).
- Surfaces: `CGame.GetSurfaceInfoOn/GetSurfaceInfoUnderObject/GetSurfaceInfoUnderObjectByBone` return a `SurfaceInfo` (`3_game/global/game.c:1171-1173`); `DayZPlayerImplement.GetSurfaceType` is obsolete (`dayzplayerimplement.c:4121`). See §Surfaces.
- Action cursor still uses View Geometry (`RaycastRVProxy` + `ObjIntersectView`). Liquid surfaces without an object are now targets (utility 0.01); `ActionTarget` ctor gained `string surfaceName = ""` (`actiontargets.c:125`).
- Uncon wake-up is refused while `PhysicsIsFalling(false)` (`playerbase.c:3423-3431`).
- [CHANGELOG 1.30] hiding physics components of static meshes no longer leaves a residual collision at the origin. No script counterpart; not tested here.

What breaks (≤1.29 mods):
- `modded DayZPlayerImplementFallDamage` that copies 1.29 `Math.InverseLerp` thresholds: compiles, then restores the old, weaker curve; with vanilla 1.30, 2.5–4 m falls cause shock and can break legs.
- Custom uncon wake that calls `hcu.WakeUp()` while `PhysicsIsFalling(false)` is true: vanilla now refuses; skipping the guard can still bury the body (measured on 1.29).
- A `modded CarScript` that overrides `OnContact` and builds `new CarContactData(localPos, other, momentumDelta)` or reads `data.impulse` / `.localPos` / `.other`: the 3-argument constructor and those fields are gone (`api-delta` in `dayz-130-migration`); use `VehicleContactData.SetData(...)` and `m_Impulse` / `m_LocalPos` / `m_Other`, and override `OnContact` on `Transport`-level semantics (`super.OnContact` now does the caching).
- Assuming player ragdoll collision/mass is 100% native and uneditable: it is authored data now.

Migration checklist:
- [ ] If you override fall damage: port `CurveExp` + 1.30 height constants; keep `if (pValue == 1) return pValue;` in `Randomize`.
- [ ] Custom humanoids: a ragdoll binds through `enfAnimSys { ragdoll = "<path>.ragdoll"; }` like `SurvivorBase` (`P:\DZ\characters\data\config.cpp:160`); there is no `LoadRagdollFile`-style script call (0 hits in `P:\scripts`). Whether a custom character class inheriting `SurvivorBase` with its own `.ragdoll` works was not tested.
- [ ] Custom uncon wake: keep `m_UnconsciousTime > 2` AND `PhysicsIsFalling(false) == false`.
- [ ] Action targeting over water: expect a surface target even with no object (`ActionTarget.GetSurfaceLiquidType`, `actiontargets.c:184`).
- [ ] Death path: `PhysicsSetSimpleDeath(true)` is the script-visible non-ragdoll death branch.
- [ ] Vehicle contact damage mods: port `CarContactData` usage to `VehicleContactData`; motorbikes use `MotorbikeScript.CheckContactCache` (`4_world/entities/vehicles/motorbikescript.c:1267`) with their own constants.
- [ ] Surface reads: move `SurfaceUnderObject*` / `GetSurfaceType` code to `GetSurfaceInfo*` → `SurfaceInfo`.

Detail: `references/dayz-1-30-ragdoll-and-fall.md`, `references/transport-netsync-vehiculos.md` (vehicle contact path), `references/fisica-engine-deep-dive.md`.

### DayZ 1.30.164200 changes (patch of 2026-10-21)

Script side (static reading of `P:\scripts` 1.30.164200, not measured):
- **Inventory dump velocity.** `MiscGameplayFunctions.ThrowAllItemsInInventoryEx(parent, flags, force)` now throws every
  attachment and cargo item with `-GetVelocity(parent)` (`4_world/static/miscgameplayfunctions.c:1295`); the `force`
  argument is no longer used anywhere in the function (`:1251-1297`). `ThrowAllItemsInInventory` already passed
  `-GetVelocity(parent)` (`:1246-1249`). Vanilla callers: tents and base-building pass `vector.Zero`
  (`tentbase.c:1003`, `basebuildingbase.c:1212`), fireplace / barrel pass nothing (`fireplace.c:81`,
  `barrelholes_colorbase.c:576`). A mod that calls the `Ex` variant to scatter loot with its own impulse (explosion,
  wreck) gets none of it on 164200: call `ThrowEntityFromInventory(ent, pos, quat, force, flags)` per item (`:1299`) or
  `ThrowPhysically` yourself. For a resting parent the result is ~zero velocity, as before.
- **Split-to-ground.** Right-click splitting a stack (`ItemBase.OnRightClick`, client, `4_world/entities/itembase.c:2059`)
  with no cargo space now drops the new stack with `GameInventory.PrepareDropEntityPos(player, this, m4, false,
  GameConstants.INVENTORY_ENTITY_DROP_OVERLAP_DEPTH)` + `dst.SetGround(this, m4)` (`itembase.c:2087-2091`) instead of
  `SetInventoryLocationToVicinityOrCurrent`. That is the same call `GameInventory.SetGroundPosByOwner` makes
  (`3_game/systems/inventory/inventory.c:1214-1220`): it snaps the transform out of conflicting entities such as cars, depth
  `INVENTORY_ENTITY_DROP_OVERLAP_DEPTH = 2` (`3_game/constants.c:1115`; proto and doc `inventory.c:606-615`). Use the same
  pair when a mod drops an item next to a player.
- **Transport.** `HasCrew()` (proto native) and `IsMoving()` are new on `Transport` (`3_game/vehicles/transport.c:175`,
  `:819-823`, base logs an error; only `CarScript` implements it). Motorbikes refuse detaching any attachment while the
  engine runs or a crew member is seated (`4_world/entities/vehicles/motorbikescript.c:612-625`). Naming rule and recipes:
  `dayz-vehicles/references/dayz-1-30-vehicles.md` §11, `dayz-motorbikes` §DayZ 1.30.164200 changes.

Engine side, not visible in scripts [reported: BI 1.30.164200 notes]:
- "Motorbikes could be launched into air when colliding with small objects or on bridges" — fixed. Bike contact damage
  scripts and constants are unchanged; re-test any 164078 "bike flies off a bridge" repro before tuning a mod for it.
- "Items appeared to be bouncing on terrain after being thrown" and "Thrown items could sometimes pass through thin
  walls" — fixed in the native item path (`ThrowPhysically` and the drop/throw bodies). The rebound numbers in truth 4
  and the BenchRE results in §Script bodies on vanilla items were measured on 1.29: re-measure before relying on them
  for thrown items. No script API changed for this (`ThrowPhysically`, `EnableDynamicCCD` and
  `SetDynamicPhysicsLifeTime` have the same signatures).
