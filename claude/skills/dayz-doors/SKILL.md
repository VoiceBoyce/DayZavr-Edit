---
name: dayz-doors
description: "Use when working on a DayZ door, animated door, class Doors, doors on buildings, a door with button, a door with lever, hatch/lid animation, model.cfg door, CfgSkeletons door, code lock, combination lock, DoorManipulationParams, CanDoorBeOpened, porting a 1.29 door mod to DayZ 1.30, or diagnosing 'door won't open/animate' on buildings and static props."
---

# DayZ Doors

> Target: DayZ 1.30 stable (1.30.164078). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

## Scope

Use this skill for the **class Doors** subsystem on DayZ buildings and static props: doors, hatches, lids, and the buttons or levers that drive them.

Do not use it as a general animation guide. For **AnimationSources**, **SetAnimationPhase**, character or creature animation graphs, vehicle-rider IK, weapons, RTM, ANM, or TXA work, use the **dayz-animation-pipeline** skill.

## Core contract

A door action crosses five name domains:

1. Visible and physical named selections in the P3D LODs.
2. Bones in **CfgSkeletons**.
3. Rotation classes in **CfgModels > class Animations**.
4. Each animation's **source**, normally the View Geometry interaction selection.
5. A **CfgVehicles > HouseNoDestruct child > class Doors** entry whose **component** matches that source.

The moving **selection** and interactive **source** may differ. This is the key to a door controlled by an adjacent button or lever.

Read [door-model-cfg-and-config.md](references/door-model-cfg-and-config.md) before editing model.cfg or config.cpp. Read [lods-and-object-builder.md](references/lods-and-object-builder.md) before editing the P3D. Read [worked-examples.md](references/worked-examples.md) when choosing a pattern. The three patterns are the examples of novoGOD's door tutorial *Doors_Buttons_Lesson*; it carries no license, so the Pack does not ship its files, and the references describe them and cite them by path and line.

**Locks and open conditions.** A building door can be locked by the engine native pair `LockDoor` / `UnlockDoor` (lockpick through `ActionLockDoors` / `ActionUnlockDoors`), and `BuildingBase` also accepts inventory locks on slots `Att_CombinationLock` and `Att_CodeLock`, bound per door by `AdditionalDoorInfo`. Vanilla `ActionOpenDoors` casts the target to **`BuildingBase`**, runs `DoorsDirectionalCheck`, then calls `CanDoorBeOpened(notnull DoorManipulationParams params)` (`P:\scripts\4_world\classes\useractionscomponent\actions\interact\actionopendoors.c:36-54,63-75`). The old `CanDoorBeOpened(int, bool)` is only a wrapper that no vanilla script calls, so overriding it alone changes nothing in game. See [Door open conditions and locks](references/door-model-cfg-and-config.md#door-open-conditions-and-locks-dayz-130), [Hooks vanilla calls](references/door-model-cfg-and-config.md#hooks-vanilla-130-calls-and-hooks-it-does-not) and [Building locks](references/worked-examples.md#building-locks-dayz-130). (Before 1.30: native lock + lockpick was the only lock path, and Open cast to `Building` and called `CanDoorBeOpened(int doorIndex, bool checkIfLocked = false)` — 1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\4_world\classes\useractionscomponent\actions\interact\actionopendoors.c:36,45,63`.)

## Workflow

1. **Model the object.** Decide what moves, what follows another bone, where the action appears, and where the sound originates.
2. **Build the LOD contract.** Name visible parts in Resolution LOD; add only space-occupying parts to Geometry; add interaction and occlusion shapes to View Geometry; add penetration-material components to Fire Geometry; add axes, action/sound, and interaction points to Memory.
3. **Write model.cfg.** Give the model a unique skeleton, list (child,parent) pairs, then add one rotation class per animated selection. Reuse a source when several parts move from one action.
4. **Write config.cpp.** Inherit from **HouseNoDestruct**, set **scope** and **model**, map the model source through **class Doors**, then define **DamageSystem**. If the object gets its own script class, derive it from `House` (a `BuildingBase`), never straight from `Building`: vanilla Open/Close only accept `BuildingBase` targets (`actionopendoors.c:36-37`, `P:\scripts\4_world\classes\useractionscomponent\actions\interact\actionclosedoors.c:26-27`).
5. **Validate names.** Compare exact spelling and case across every LOD, skeleton bone, animation **selection**, **source**, **axis**, Doors **component**, **soundPos**, and DamageZone **componentNames**.
6. **Test in game.** Verify action location, closed/open motion, child motion, sub-ranges, collision, occlusion, penetration, sounds, world-spawn state, and server/client RPT logs.

Follow the debug order: config load -> entity spawn -> action location -> animation -> collision/occlusion -> audio/damage. Do not debug a later layer before the earlier layer is proven.

## Pick a worked pattern

| Need | Pattern | Defining mapping |
|---|---|---|
| Door plus moving handle | Simple Door | Door and handle share source **door1**; handle is a child of **door1** and finishes at phase **0.15**. |
| Door controlled by a static button | Door with Button | Only **door1** is a bone; animation source and Doors component are **door1_open** at the button. |
| Door plus moving handle and lever | Expert Mode | Door, handle, and lever share **door1_open**; handle follows door, lever follows nothing. |
| Building door that accepts a combination lock or code lock (new in 1.30) | Building lock attachments | Keep the **class Doors** mapping above; add per-door `relatedInventorySlots[]` / `lockCompatibilityBitMask` plus entity slots `Att_CombinationLock` / `Att_CodeLock`. The building also needs a construction component, or the attach action is never offered (see the checklist). |

## New door from scratch checklist

- [ ] P3D basename, CfgModels class, model path, and vehicle class are chosen.
- [ ] Skeleton name is unique to this P3D.
- [ ] Every animated selection is a skeleton bone.
- [ ] Every (child,parent) pair matches the intended transform hierarchy.
- [ ] Resolution LOD names every visible component.
- [ ] Geometry/View/Fire shapes are closed and convex.
- [ ] View Geometry contains the intended interaction selection/source.
- [ ] Memory contains each rotation axis, the sound/action point, and the interaction point.
- [ ] Every animation has verified type, selection, source, axis, memory, phase range, and angles.
- [ ] **CfgPatches.requiredAddons[]** includes **"DZ_Data"**.
- [ ] The object inherits from **HouseNoDestruct**.
- [ ] **class Doors** maps each intended source through **component**.
- [ ] (If the door should take a padlock or code lock) entity `attachments[]` include `Att_CombinationLock` and/or `Att_CodeLock`; the matching **class Doors** entry sets `relatedInventorySlots[]`; Memory has slot selections `att_combinationlock` / `att_codelock` and any `interactPositionPoint` / `interactDirPoint` pair.
- [ ] (Lockable door) the building's CfgVehicles class has a `class Construction`. `ActionAttachToConstruction` needs `CanUseConstruction()` (`P:\scripts\4_world\classes\useractionscomponent\actions\singleuse\actionattachtoconstruction.c:47-48,151`), which is true only once a construction component exists, and `EntityAI` creates one only when `cfgVehicles <class> Construction` exists (`P:\scripts\3_game\entities\entityai.c:248-249,3464-3488`; `BuildingBase` makes it a `Rebuilding`, `P:\scripts\4_world\entities\game\super\building.c:23-27`). ? Whether a minimal `Construction` class is enough for a custom house was not tested.
- [ ] Script overrides of door conditions use the hooks vanilla calls: `CanDoorBeOpened(notnull DoorManipulationParams params)` for Open and `CanDoorBeClosed(int)` for Close — not `CanDoorBeOpened(int, bool)` or `CanDoorBeClosed(int, Object)`, which nothing in vanilla calls.
- [ ] Door sound names are verified in **DZ\sounds\hpp\config.cpp**.
- [ ] **DamageSystem** has GlobalHealth, GlobalArmor, and appropriate DamageZones.
- [ ] In-game tests cover action position, motion, sounds, physical LODs, damage, spawn state, and RPT logs.

## Common gotchas

- A button can be the source without being animated or listed in the skeleton.
- A child handle must name the door bone as its parent or it will not follow the door transform.
- Only sources need a normal Doors mapping; animated selections sharing one source do not each need one.
- **DamageZones.componentNames[]** can target moving geometry (**door1**) even when the action source is **door1_open**.
- Use phase sub-ranges to sequence motion: Simple Door handle runs **0..0.15**, then the door runs **0.15..1**.
- **initOpened** is a spawn probability: `rand < initOpened` spawns the door opened (0 = always closed, 0.5 ~= half). Verified vs BI Doors_on_buildings wiki.
- Keep skeleton names unique. The tutorial warns that duplicate skeleton names can crash a server.
- The tutorial's Expert Mode example has a source-to-Doors anomaly; read its warning in [worked-examples.md](references/worked-examples.md) before reusing the pattern.
- A locked `DigitalCodeLock` or `CombinationLock` on a related slot makes `BuildingBase.CanDoorBeOpened` return false (`P:\scripts\4_world\entities\game\super\building.c:307-333`). Lock / unlock with a lockpick is not offered while an external lock sits on that door (`P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actionlockdoors.c:40-54`, `...\actionunlockdoors.c:39-53`). A combination lock opens through the separate action `ActionCombinationLockUnlock` after the dials match. (Before 1.30: `LockDoor` / `UnlockDoor` plus lockpick was the only building-door lock path, and a combination lock dropped itself when the last dial matched.)
- `EBuildingLockType` is still lockpick / ship-container keys (1.30 adds `SHIP_CONTAINER_4`, `P:\scripts\3_game\enums\ebuildinglocktypes.c:15`). Combination and code locks are **inventory attachments**, not extra enum bits. Do not invent `COMBINATION_LOCK = 2` / `CODE_LOCK = 4`.
- `BuildingBase.IsLocked(int doorIndex)` returns false on every path (`P:\scripts\4_world\entities\game\super\building.c:283-305`) and nothing in vanilla calls it. To ask "is an external lock engaged on this door", read the related slots as `CanDoorBeOpened` does (`:313-330`).
- **Vanilla house furniture is PROXY geometry inside the building's ODOL, not a placed object.** Fridges and wardrobes inside houses are not instances of `StaticObj_Furniture_*`: the model path appears as a proxy inside ~25 house ODOLs of `DZ/structures` (1,546+ `kitchen\fridge\fridge` placements in `dayzOffline.chernarusplus\mapgrouppos.xml`, lower bound), so neither a `modded` override of `StaticObj_Furniture_fridge` nor `CF_ObjectManager.HideMapObject` reaches them. [EXACT][CLAIM-FURNITURE-FRIDGE-CLASS] The trap: `StaticObj_Furniture_fridge : HouseNoDestruct` exists with `scope=1` and no `Doors` subclass (`P:\DZ\structures\furniture\config.cpp:547-551`) — the class existing does not make house fridges instances of it. Before designing doors for furniture, grep the extracted `DZ/structures` P3Ds for the model path as a proxy (use `.` and not `\` as path separator: in shell tooling the escaped backslash collapses and even the control stops matching) and count placements in the world WRP.
- **Measured way to make house furniture interactive (measured on 1.29: DayZDiag 1.29.163709, 2026-09-24): replace the model by path.** [EXACT] A normal mod PBO whose `$PBOPREFIX$` is `DZ\structures\furniture\kitchen\fridge` and which ships its own `fridge.p3d` (ODOL, with a fresh memory point) overrides the vanilla model for script-created instances AND for the proxies inside houses: `MemoryPointExists` true on server and client on the overridden class, vanilla geometry on the control class, and the replacement geometry visible sticking out of the house roofs. Open question: whether the house Geometry/View LODs carry the furniture proxy (vertical `RaycastRVProxy` sweeps, Geom and View, over 7 houses never hit the 16 m marker).

## Stop and ask

STOP and ask the user when a source, interaction selection, axis, component, LOD membership, or duplicate **class Doors** mapping cannot be proven from the real P3D/config files. Do not invent a plausible name. Prefix any unresolved technical claim with **?**.

## References

- [Model/config contract](references/door-model-cfg-and-config.md)
- [LODs and Object Builder](references/lods-and-object-builder.md)
- [Worked examples](references/worked-examples.md)
- [Official DayZ doors reference](https://community.bistudio.com/wiki/DayZ:Doors_on_buildings)
- [Official DayZ Samples](https://github.com/BohemiaInteractive/DayZ-Samples)
- [Official LOD reference](https://community.bistudio.com/wiki/LOD)

## Vehicle doors are out of scope (added 2026-08-31)

Vehicle doors do **not** use the building `class Doors` contract documented here.

- A detachable car door is a `CarDoor` attachment. `ActionCarDoorsOutside` resolves its target by
  raycast against the **item's ViewGeometry**.
- A door that stays part of the vehicle shell needs vehicle actions around
  `SetDoorOpen`/`IsDoorOpen` plus `model.cfg` `AnimationSources`.

Route vehicle work through `dayz-vehicles`: invariant #21 redirects to the public
`rip-vehicle-import/cookbooks/family-b/radial-puerta-ausente.md` **DOOR MECHANISM SELECTOR**, and
invariant #22 carries the attachment ViewGeometry rule. Applying building `class Doors` to a
vehicle will not produce a working radial, even though both mechanisms use names such as `source`,
`component`, and `axis`.

## DayZ 1.30 changes

All paths below are 1.30.164078 (`P:\scripts\...`) unless written `1.29:`. `...\actions\` stands for `P:\scripts\4_world\classes\useractionscomponent\actions\`.

### What changes

- **Open API.** `DoorManipulationParams` carries `m_DoorIndex`, `m_CheckIfLocked`, and `m_Caller`. `Building.CanDoorBeOpened(notnull DoorManipulationParams params)` is the real check; the old `CanDoorBeOpened(int doorIndex, bool checkIfLocked = false)` still exists as a wrapper that builds the params **without** `m_Caller` (`P:\scripts\3_game\entities\building.c:5-10,141-167`). Vanilla `ActionOpenDoors` and its subclass `ActionLockedDoors` (the locked-door rattle) cast the target to `BuildingBase`, call `DoorsDirectionalCheck`, fill the params and call the params form (`...\actions\interact\actionopendoors.c:36-54,63-75,101-112`).
- **Close API.** `ActionCloseDoors` also casts to `BuildingBase` and adds `DoorsDirectionalCheck`, but still calls `CanDoorBeClosed(int)` (`...\actions\interact\actionclosedoors.c:26-38,49-55`). The new overload `CanDoorBeClosed(int doorIndex, Object caller)` (`building.c:169-172`) has no vanilla caller.
- **Per-door extra info.** `AdditionalDoorInfo` is parsed from each **class Doors** child: `relatedInventorySlots[]`, `lockCompatibilityBitMask`, `interactPositionPoint`, `interactDirPoint`, `doorConstructionPart`, `doorConstructionPhysicsSource`. Default mask if the bitmask is omitted: `1 << EBuildingLockType.LOCKPICK`. Entries are keyed by their position among the `class Doors` children, which the script assumes equals the engine door index (`P:\scripts\3_game\entities\additionaldoorsinfo.c:1-46,59-80`). `GetLockCompatibilityType` now returns that mask (`building.c:193-196`; before 1.30 it always returned `1 << LOCKPICK`, 1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\3_game\entities\building.c:167-170`).
- **Attachment locks.** `BuildingBase` defines `Att_CombinationLock` and `Att_CodeLock`. A locked `DigitalCodeLock` or `CombinationLock` on a related slot blocks `CanDoorBeOpened`. Conflicting twin slots are rejected. A locked lock cannot be released. (`P:\scripts\4_world\entities\game\super\building.c:10-11,192-333`)
- **New item `DigitalCodeLock`.** Script class in `P:\scripts\4_world\entities\itembase\codelock.c:1-42`; logic in `CodeLockComponent`; UI layout `P:\gui\layouts\day_z_digital_lock.layout`. Needs `Battery9V` in slot `BatteryD` (`codelock.c:6,307-309`). PIN length 4–6. Two brute-force stages via `CfgGameplayHandler.GetExternalLockProtectionCountStageOne/Two` and `GetExternalLockProtectionTime` / `GetExternalLockProtectionResetTime` (`P:\scripts\4_world\classes\codelockcomponent.c:4-11,23-24,66-67,468-483`, `P:\scripts\3_game\cfggameplayhandler.c:513-531`).
- **`CombinationLock` two-sided + unlock action.** Dials no longer drop the lock by themselves; `ActionCombinationLockUnlock` runs after the combination matches and calls `UnlockOnServer` (`...\actions\singleuse\actioncombinationlockunlock.c:1-7,57-82`). Persistence writes `m_CombinationInside` at vanilla stream **version 143** (`P:\scripts\4_world\entities\itembase\combinationlock.c:25-35,128-180`). `CombinationLock.UnlockServer(EntityAI, EntityAI)` is gone (compile error for old callers) and several lock methods became `protected` (dayz-130-migration `api-delta`).
- **Bunker doors.** `Bunker.CanDoorBeOpened` uses `MemPointDirectionalCheck` on `{doorType}_action` / `{doorType}_inside`. Main door auto-closes after `DOOR_AUTOCLOSE_TIME = 12` and re-locks the `DigitalCodeLock_Bunker`. (`P:\scripts\4_world\entities\building\bunker.c:3,47-62,73-90`)
- **Rebuildable house doors.** `Rebuilding` drives `doorConstructionPhysicsSource` on open/close start. (`P:\scripts\4_world\classes\rebuilding\rebuilding.c:439-463`)
- **Lock types.** `EBuildingLockType` gains `SHIP_CONTAINER_4` (`P:\scripts\3_game\enums\ebuildinglocktypes.c:15`; 1.29 stopped at `SHIP_CONTAINER_3`), used by `Land_Mil_ContainerHabitat_Armory_Locked_Green_DE` (`P:\scripts\4_world\entities\itembase\containerlocked.c:140-146`).
- **Pathgraph doors on non-house entities.** `World.UpdatePathgraphDoorByAnimationSourceName(notnull Object, string)` is `[Obsolete("1.30: Use UpdatePathgraphEntityState")]`; the replacement is `bool UpdatePathgraphEntityState(notnull Object object)`, with no animation-source argument (`P:\scripts\3_game\global\world.c:118-132`). Vanilla 1.30 still calls the obsolete one for the underground entrance door (`P:\scripts\4_world\entities\building\underground\entrance\land_underground_entrance.c:149`) and the new one from `ConstructionBase` (`P:\scripts\4_world\classes\constructionbase.c:513`). ? The proto comment does not document the new call; whether it is a drop-in for door state was not verified. Obsolete calls compile with a `SCRIPT (W)` warning [reported: bzed].
- **[CHANGELOG 1.30]** House-based `disableSimulation` and a fix for inventory attachments on buildings created at load time. Not confirmed on `P:\`: no `config.cpp` under `P:\DZ` sets `disableSimulation`, and the `Entity` natives `DisableSimulation` / `GetIsSimulationDisabled` (`P:\scripts\3_game\entities\entity.c:3,6`) already existed in 1.29 (1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\3_game\entities\entity.c:3,6`) — they are not new.

### Hooks that compile on 1.30 but vanilla no longer calls

Full table with call sites: [Hooks vanilla 1.30 calls](references/door-model-cfg-and-config.md#hooks-vanilla-130-calls-and-hooks-it-does-not). In short:

- `CanDoorBeOpened(int, bool)` overrides — no vanilla script calls the `int` form any more; only explicit calls from your own code reach it.
- `CanDoorBeClosed(int, Object)` — new and never called; `ActionCloseDoors` calls `CanDoorBeClosed(int)`.
- Script classes deriving straight from `Building` (not `House` / `BuildingBase`) — compile, but Open and Close are no longer offered on them (lock / unlock with a lockpick still cast to `Building`).
- `BuildingBase.IsLocked(int)` — compiles, always returns false.

### What breaks for a 1.29 door mod

| Break | Severity | Migration |
|---|---|---|
| Override of `CanDoorBeOpened(int, bool)` is skipped by vanilla Open | **HIGH** | Override `CanDoorBeOpened(notnull DoorManipulationParams params)` and call `super`. The `int` wrapper still exists (`P:\scripts\3_game\entities\building.c:160-167`) but `ActionOpenDoors` no longer calls it (`...\actions\interact\actionopendoors.c:52,72`). Vanilla `Land_WarheadStorage_Main` (`P:\scripts\4_world\entities\building\underground\land_warheadstorage_main.c:352`) and `Land_WarheadStorage_Bunker_Facility` (`...\underground\land_warheadstorage_bunker_facility.c:20`) still override only the `int` form — do not copy them as the 1.30 pattern. ? Read from code: their door interlocks are therefore not consulted by Open; not tested in game. |
| Custom door class derived from `Building`: Open / Close vanish | **HIGH** | Derive from `House` (or `BuildingBase`). 1.30 Open / Close cast to `BuildingBase` (`actionopendoors.c:36-37,63-64`, `actionclosedoors.c:26-27,49-50`); 1.29 cast to `Building` (1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\4_world\classes\useractionscomponent\actions\interact\actionopendoors.c:36,57`). |
| `modded class ActionOpenDoors` that pasted the 1.29 condition body | **MEDIUM** | It still compiles, but drops `DoorsDirectionalCheck` and calls the `int` wrapper, so `m_Caller` is null. Re-base it on the 1.30 body. ? Read from code, not tested: with a null caller `MemPointDirectionalCheck` returns true (`P:\scripts\4_world\static\miscgameplayfunctions.c:940-944,967`), so a `Bunker` main door's "inside" branch passes without the code (`bunker.c:55-58`). |
| Door opens from the wrong side, or Open never appears | **MEDIUM** | Add Memory points named in `interactPositionPoint` / `interactDirPoint`, or omit both (empty names make `MemPointDirectionalCheck` return true). (`miscgameplayfunctions.c:940-968`, `P:\scripts\4_world\entities\game\super\building.c:335-342`) |
| Lockpick still offered on a padlocked door | **MEDIUM** | Set `relatedInventorySlots[]` on that Doors entry so `ActionLockDoors` / `ActionUnlockDoors` see the external lock. (`...\actions\continuous\actionlockdoors.c:40-57`, `...\actions\continuous\actionunlockdoors.c:39-56`) |
| Custom combination-lock auto-unlock on last dial | **MEDIUM** | Unlock through `ActionCombinationLockUnlock` (vanilla `CombinationLock.SetActions` adds it, `combinationlock.c:686`) and `UnlockOnServer` (`combinationlock.c:409`); `UnlockServer` is gone. Persist `m_CombinationInside` when `version >= 143`. (Before 1.30: the dial action unlocked on match, 1.29: `O:\DayZ 1.29 Старый распакованный клиент\scripts\4_world\classes\useractionscomponent\actions\continuous\actiondialcombinationlockontarget.c:67-79`.) |
| Code lock / combination lock never attaches to a custom building | **HIGH** | Declare the slots on the entity, proxy selections `att_codelock` / `att_combinationlock`, `relatedInventorySlots[]` on the door, and a `class Construction` on the building (`CanUseConstruction()` gate). `ActionAttachToConstruction` looks up the slot from `AdditionalDoorInfo`. (`...\actions\singleuse\actionattachtoconstruction.c:47-48,151-180`) |

### 1.30 migration checklist (building doors)

- [ ] Search the mod for `CanDoorBeOpened(` and convert building overrides to the `DoorManipulationParams` signature.
- [ ] Search for `CanDoorBeClosed(` — keep Close logic in `CanDoorBeClosed(int)`; the `(int, Object)` overload is never called.
- [ ] Search for `: Building` / `extends Building` door classes and re-base them on `House`.
- [ ] Search for `modded class ActionOpenDoors` / `ActionCloseDoors` and re-base their bodies on 1.30.
- [ ] If the door should take a lock, add `attachments[]` + `relatedInventorySlots[]` + Memory slot selections + `class Construction`.
- [ ] Do not treat `EBuildingLockType` as the combination/code-lock API; those are attachments.
- [ ] Combination-lock mods: replace `UnlockServer` with `UnlockOnServer`, keep `ActionCombinationLockUnlock`, and add the v143 `m_CombinationInside` read.
- [ ] Rebuildable doors: set `doorConstructionPart` and `doorConstructionPhysicsSource` on the Doors entry.
- [ ] Non-house door entities: replace `UpdatePathgraphDoorByAnimationSourceName` with `UpdatePathgraphEntityState` once you can test it in game.
- [ ] Do not add `disableSimulation = 1` to house classes on the strength of the changelog alone: no vanilla config on `P:\DZ` uses it ([CHANGELOG 1.30] only).

### 1.30 references in this skill

- [Door open conditions and locks](references/door-model-cfg-and-config.md#door-open-conditions-and-locks-dayz-130)
- [Hooks vanilla 1.30 calls](references/door-model-cfg-and-config.md#hooks-vanilla-130-calls-and-hooks-it-does-not)
- [Building lock integration examples](references/worked-examples.md#building-locks-dayz-130)
- [1.30 Memory points and lock slot selections](references/lods-and-object-builder.md#dayz-130-memory-points-and-lock-slots)
