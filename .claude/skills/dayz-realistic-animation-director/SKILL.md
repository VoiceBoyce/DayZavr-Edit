---
name: dayz-realistic-animation-director
description: Dirige de principio a fin la creación, corrección, pulido, exportación, integración y verificación de animaciones realistas para DayZ. Úsala cuando el resultado deba sentirse físicamente creíble en jugador, manos y armas, recargas, unstuck/unjam, locomoción, criaturas, ocupantes de vehículos, mecanismos, props o simulaciones; especialmente ante clipping, dedos cruzados, articulaciones sobreextendidas, contacto falso, objetos que no siguen la mano, stutter, loops defectuosos o discrepancias Blender-vs-in-game. Orquesta `blender-animation` para autoría y `dayz-animation-pipeline` para contratos del motor; no sustituye consultas puramente técnicas sobre config, ASI, skeletons o exportación.
---

# DayZ Realistic Animation Director

Produce a credible animation and close its integration with honest evidence. This skill directs the work; Blender and DayZ skills retain technical authority.

> Target: DayZ 1.30 stable (1.30.164078). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

## Authority boundary

Before acting, read [authority-and-routing.md](references/authority-and-routing.md).

- Invoke `blender-animation` before operating on Actions, rigs, constraints, curves, renders, or export from Blender.
- Invoke `dayz-animation-pipeline` before deciding format, skeleton, export mask, FPS/frame budget, notetracks, anim graph/ASI, compile, wiring, or DayZ integration.
- Invoke additional domain skill indicated in [authority-and-routing.md](references/authority-and-routing.md) for characters, creatures, vehicles, P3D, retargeting, or in-game test; then apply checks from [domain-gates.md](references/domain-gates.md).
- In case of conflict, specialized authority wins. Record divergence and stop affected gate; do not invent a third rule.

If the request is strictly technical —for example, choosing an `AnimationSource`, registering an `.anm`, or explaining an ASI state— let `dayz-animation-pipeline` be the primary skill. Use this skill when authoring, evaluating, or improving motion.

## Domain routing

| Animation | Skills added to director |
|---|---|
| Player, hands, custom human or infected | `dayz-characters`; for creatures verify first their actual bind/skeleton (1.30: [CHANGELOG] bone index derived from the name, 250-bone limit removed; the vanilla `skeletons.anim.xml` still carries `index` attributes, see `dayz-animation-pipeline` → `references/player-skeleton.md`) |
| Weapon, reload, unjam, or P3D mechanism | `dayz-weapons` if entity contract changes + relevant P3D/model skill (1.30: new bone remaps and dedicated ASI profiles, see `references/domain-gates.md`) |
| Occupant, entry, or vehicle controls | `dayz-vehicles` / `dayz-motorbikes` (1.30: motorbikes `Motorbike_01`/`Motorbike_02` with rider instances `MOTO1`/`MOTO2`, 2-bone IK on handlebar) |
| Mocap/external donor | applicable retargeting skill; never without source→target map |
| Runtime test | active `dayz-test-ingame`/`dayz-mcp-verify`, with lease and lifecycle (1.30: [CHANGELOG] Workbench Enfusion 2021 live editing, not tried on this host) |

`blender-animation` and `dayz-animation-pipeline` remain mandatory for authoring and integration respectively; skills in the table do not replace them.

## Three checks that never substitute for each other

1. **Full chain:** measuring knuckles and tips does not validate intermediate phalanges. Sample all relevant segments and joints.
2. **Positive contact:** "no penetration" does not mean "gripping". Require a contact pair, target surface/landmark, and distance band.
3. **Relative locking:** sharing keyframes or a scalar curve does not prove two elements move together. Measure `T_actor^-1 * T_target` throughout the entire grip window.

A reproducible visual failure from user or in-game invalidates any offline `PASS`. Convert it into a fixture before authoring again.

## Mandatory gated workflow

### 0. Preserve and observe

- Inspect scene, Actions, current frame, selection, constraints, and windows before mutating.
- Always work from a versioned copy; do not overwrite source `.blend`.
- If fixing a defect, first reproduce failure with current scene and preserve that RED evidence.
- Read project contract (current `CLAUDE.md`, product spec, plan, and handoff) when existing.

### 1. Capture the DayZ technical contract

Ask `dayz-animation-pipeline` for applicable route and record:

- animation type and authoritative skeleton/rig;
- FPS, duration or frame budget, and loop rules;
- notetracks/events and runtime states;
- included/excluded bones and complete mobile objects/selections;
- artifacts for export, compile, wiring, build, and in-game test.

Do not hardcode 291 frames, 30 FPS, or SR2M notetracks for other animations. Each task obtains its active contract.

### 2. Define acceptance before posing

Read [motion-quality-contract.md](references/motion-quality-contract.md) and create a per-task contract declaring only applicable modules:

- key poses and beats;
- joints/chains and calibrated limits;
- contacts, surfaces, and `contact_on..release` windows;
- prohibited collision pairs and permitted contacts;
- objects that must preserve relative transformation;
- intentional impact, snap, or sliding exceptions;
- actual entry/exit pose and in-game states to test.

Tolerances must come from reference, geometry, or an explicit decision. Never adjust threshold to make current candidate pass.

Every contract declares exactly `contract_mode: "diagnostic"` or `contract_mode: "production"`; missing or unknown values are errors. A contract capable of granting `OFFLINE_PASS` uses `production`. Each check declares provenance (`source_kind`, `source`, `verified_date`, `method`) and any `segment_clearance` check requires shared geometric provenance for capsule radii. A `diagnostic` contract may reproduce a failure, but `eligible_for_offline_pass` will always be false.

For a continuous window of contact, collision, locking, order, joint range, or continuity use consecutive `frame_range` with `step: 1`; production requires it. A sparse list of golden poses does not prove what happens between them.

### 3. Blocking

With `blender-animation`, author only golden poses: start, anticipation, contact, maximum effort, release, recovery, and end.

- Render each pose in on-axis view and at least one oblique view.
- Validate anatomy, silhouette, contact, and collision before interpolating.
- For hands, read [biomechanics-and-contact.md](references/biomechanics-and-contact.md) and evaluate all five fingers, wrist, forearm, elbow, shoulder, and clavicle when part of the gesture.
- Do not advance to spline if a key pose already clips, is overextended, or misses target.

### 4. Motion and effort

- Apply blocking → spline → polish via `blender-animation`.
- Use timing, spacing, arcs, anticipation, overlap, moving holds, and settle according to physical intent.
- Validate position, rotation, velocity, acceleration, and jerk on relevant elements.
- Distinguish an intentional snap/impact via a declared window; outside of it, a spike is stutter.
- In mechanisms, move full hierarchy or functional selection, not only visual piece seen from a camera.

### 5. Offline audit

Run sampler and validator when Blender is available:

```powershell
& $env:BLENDER_EXE --background '<scene.blend>' --python '<skill>\scripts\sample_blender_motion.py' -- --contract '<contract.json>' --output '<report.json>'
python '<skill>\scripts\validate_motion_contract.py' --report '<report.json>' --contract '<contract.json>' --output '<audit.json>'
```

Interpret validator exit codes:

- `0`: all required checks pass;
- `1`: valid input, one or more required checks fail;
- `2`: invalid contract, sample, or execution; no quality verdict.

In addition to JSON:

- review full video at real speed;
- review multi-angle renders of contact, extremes, transitions, and recovery;
- look for clipping between keyframes, not only on key frames;
- check start/end against actual runtime entry pose.

### 6. Exporta e integra

- Hand off Blender export to `blender-animation` following its active contract.
- Hand off artifact to `dayz-animation-pipeline` for compile, ASI/config, build, and deploy.
- Verify artifact actually deployed, not only source. In DayZ 1.30, animation graphs are Enfusion Config `.agf` files indexed from an `AnimSrcGraph` `.agr` (`P:\DZ\anims\workspaces\player\player_main\player_main.agr:1587-1589`), and `.asi` files use `AnimSetInstanceSource` (before 1.30: `$AnimGraph 7` `.agr` sub-graphs and `$animsetinstance`).
- Do not declare that Workbench, DayZATool, Blender MCP, or DayZ executed if no evidence of that execution exists in the session.

### 7. Gate in-game

Lee [evidence-and-integration.md](references/evidence-and-integration.md).

- Test stances, cameras, and states that might alter IK or blending. [CHANGELOG 1.30] the Workbench Animation Editor allows **live editing** on active clients for interactive visual debugging of transitions and graphs (not tried on this host).
- Review RPT and compare timing, contact, clipping, and mechanical states against contract.
- If in-game contradicts Blender, in-game rules and the case becomes a regression.
- If environment does not allow testing, terminate in `MANUAL_REQUIRED`, not PASS.

## Output states

- `FAIL`: at least one mandatory gate fails.
- `OFFLINE_PASS`: contract, scene, audit, renders, and offline artifact approved; game test pending.
- `MANUAL_REQUIRED`: next gate requires an action or tool that is unavailable.
- `IN_GAME_PASS`: deployed build and DayZ behavior are verified with evidence.

Always report which gates were executed, what evidence exists, what was omitted, and why.

## Regression policy

When a new defect appears:

1. freeze failed scene as read-only input;
2. write a minimal fixture that fails for that reason;
3. demonstrate RED;
4. implement or tighten the check;
5. demonstrate GREEN with a corrected sample to avoid a tautological test;
6. only then modify production animation.

Run reusable suite with:

```powershell
python '<skill>\scripts\run_regression_tests.py'
```

To include the real SR2M fixture, define `SR2M_V44_BLEND` and add `--real-fixtures`. If variable is missing, correct result is `SKIP_REAL_FIXTURE`, not PASS.

## DayZ 1.30 changes

Re-verified on 1.30.164078 (`P:\`); first written against 1.30 Exp 164014. `P:\scripts\…` and `P:\DZ\…` are 1.30; `1.29: O:\…` is the old build. Detail lives in `dayz-animation-pipeline` (`SKILL.md` §DayZ 1.30 changes, `references/anim-graph.md`) and `references/domain-gates.md` here.

### What changes in 1.30 for realistic animation direction

1. **Motorbike riders [EXACT]:**
   - Rider animation instances `MOTO1 = 10` (`Motorbike_01`, "Jana 50") and `MOTO2 = 11` (`Motorbike_02`, "Bitrak 682"), both two-wheelers (`P:\scripts\4_world\entities\vehicles\vehicleaniminstances.c:13-14`; mapping in `inheritedmotorbikes\motorbike_01.c:48-50`, `motorbike_02.c:49-51`; names from `STR_Motorbike1/2` in `P:\languagecore\stringtable.csv`; front and rear wheels in `P:\DZ\vehicles\singletrack\config.cpp`). The graph columns are named `Jawa_05` / `Jawa_Bitrak` (`player_main.ast:1119-1120`). (corrected on 1.30.164078: this item used to call them "Jawa 50cc" and a "Jawa Bitrak tricycle".)
   - Third-person motorbike camera: `DAYZCAMERA_3RD_VEHICLE_MOTORBIKE = 32` (`P:\scripts\4_world\entities\manbase\dayzplayer\dayzplayercameras.c:20`).
   - Hands on the bars by two-bone IK (`AnimSrcNodeIK2` / `AnimSrcNodeIK2Target`, `P:\DZ\anims\workspaces\player\player_main\vehicles.agf:17-46`); the steering pose is read from `VehicleSteering` over ±45° of wheel angle (`vehicles.agf:50`). The graph also declares `VehicleThrottle`, `VehicleLean` and `VehicleSuspension` (`player_main.agr:118`, `:197`, `:205`).
   - `HumanCommandVehicle.IsTransitioning()` (`P:\scripts\3_game\human.c:735-738`) = getting in, getting out or switching seat.
2. **Surrender [EXACT]:** 1.30 no longer puts `SurrenderDummyItem` in the hands; the class is `[Obsolete("1.30: no replacement - no longer in use")]` (`P:\scripts\4_world\entities\itembase\surrenderdummyitem.c:1`). The state is `PlayerBase.SetSurrenderState(bool)` (`playerbase.c:2116`) / `Man.IsSurrendered()` (`P:\scripts\3_game\entities\man.c:67`), cleared by `EmoteManager.ClearSurrenderState()` (`4_world\classes\emotemanager.c:1246-1252`). Before 1.30: `CreateInHands("SurrenderDummyItem")` (1.29: `O:\…\scripts\4_world\classes\emotemanager.c:309`).
3. **Runtime instance control and in-hands decoupling [EXACT]:**
   - `Human.SetAnimationInstanceByName(string animationInstanceName, float blendingTime)` (`P:\scripts\3_game\human.c:1384`). Switching back to the base uses the `.asi` path; vanilla keeps the base loaded through an `"Empty"` hands profile registered in script (`P:\scripts\4_world\entities\manbase\dayzplayer\dayzplayercfgbase.c:1546-1549`), not in `player_main.asi`.
   - `HumanItemAccessor.OnItemInHandsChanged(bool pInstant = false, bool pChangeAnimationInstance = true)` (`P:\scripts\3_game\humanitems.c:112`; 1.29 had only `pInstant`), `EnableAutoAnimInstUpdateOnHandsChange(bool)` (`:115`), and `Man.OnItemInHandsChangedEx(bool, bool)` (`P:\scripts\3_game\entities\man.c:168`).
4. **Per-stance camera rotation and speed limits [EXACT]:** `HumanInputController.SetErectSpeedLimit` / `SetCrouchSpeedLimit` / `SetProneSpeedLimit` (`P:\scripts\3_game\human.c:230-232`) and `DisableErectCameratHorizontalRotation` [sic] / `DisableCrouchCameratHorizontalRotation` [sic] / `DisableProneCameraHorizontalRotation` (`:240-242`). Vanilla surrender uses both (`playerbase.c:2073-2090`).
5. **11 new full-body actions [EXACT]:** `CMD_ACTIONFB_COMBINATIONLOCK = 256` … `CMD_ACTIONFB_WETCLOTHWELL = 266` (`P:\scripts\3_game\dayzplayer.c:889-899`), plus `CMD_ACTIONMOD_COMBINATIONLOCK = 256` and `CMD_ACTIONMOD_WASHFACE` (257) (`:766-767`); no existing value changed.
6. **Skeletons and creatures:**
   - [CHANGELOG 1.30] no 250-bone limit; the global bone index is derived from the name. [EXACT] The vanilla `skeletons.anim.xml` was not de-indexed: `hermit_newbindpose.xob` keeps 24 `index` attributes and every skeleton carries `index = "0" movement = "true"` on `EntityPosition` (`P:\DZ\anims\cfg\skeletons.anim.xml:5-17`, `:986-988`). (corrected on 1.30.164078: this item used to say only `EntityPosition` keeps an index.) Four new animal skeletons: `:986-1334`.
   - Terrain alignment in quadrupeds: `AnimSrcNodeProcTransform AlignToTerrain_Rot`, `Expression "SlopeAngleX  * 0.0174532925…"` (`P:\DZ\animals\animations\!graph_files\wolf\wolf_maingraph.agf:5-18`). Before 1.30: an `AnimNodeRot` line in `wolf_maingraph.agr`.
   - New infected mind state `MINDSTATE_COWER` (`P:\scripts\3_game\entities\dayzinfected.c:18`; absent in 1.29).
7. **Workbench and death physics:**
   - [CHANGELOG 1.30] Workbench Animation Editor on Enfusion 2021 with live editing of graphs on running clients; a ragdoll editor for `.ragdoll` files. [EXACT] Vanilla ships `P:\DZ\characters\bodies\human.ragdoll`, bound by `ragdoll=` in `SurvivorBase` `enfAnimSys` (`P:\DZ\characters\data\config.cpp:160`).
   - [EXACT] `PhysicsSetSimpleDeath(bool)` (new) vs `PhysicsSetRagdoll(bool)` / `PhysicsIsRagdoll()` (`P:\scripts\3_game\human.c:1448-1452`); vanilla's non-command death branch calls `PhysicsSetSimpleDeath(true)` (`P:\scripts\4_world\entities\dayzplayerimplement.c:745`).

### What breaks in ≤1.29 contracts and assets and how to migrate

| Broken element | Cause in 1.30 | Required action |
|---|---|---|
| Player/creature graph edits in `$AnimGraph 7` `.agr` files (1.29 already split them into sub-graphs, not monolithic) | Sub-graphs are Enfusion Config `.agf` text files listed by the `AnimSrcGraph` master (`player_main.agr:1587-1589`) [EXACT] | Rebuild the modifications on the 1.30 `.agf` modules (Workbench 2021 editor or by hand); a text diff across the two formats is noise. |
| `.ast` templates with anonymous groups | [CHANGELOG 1.30] the default unnamed group was removed; vanilla names every group (`Name "Default"`, `Name ".unnamed"`) [EXACT, `anim-graph.md`] | Give each group an explicit `Name`. |
| `.asi` files with `$animsetinstance` syntax | Vanilla `.asi` are `AnimSetInstanceSource` (`P:\DZ\anims\workspaces\player\player_main\player_main_rifle.asi:1-15`) [EXACT] | Convert `#template`/`#parent`/`#ikpose`/`$animations` to `Template`/`ParentTemplates`/`IkPose`/`Lines`. Whether 1.30 still loads the old syntax was not tested. |
| `Buffer Save` / `Buffer Use` nodes | [CHANGELOG 1.30] removed; confirmed by the files: 1.29 `locomotion.agr:2415-2422` had them, no 1.30 graph does [EXACT] | Replace buffer logic with control variables in `ControlTemplate AnimSrcGCT` or direct state machine connections. |
| `SurrenderDummyItem` dependency | No longer created; class obsolete (`surrenderdummyitem.c:1`) [EXACT] | Replace item-in-hands checks with `IsSurrendered()`; drive state with `PlayerBase.SetSurrenderState(bool)`. |
| `index` attributes in `skeletons.anim.xml` | [CHANGELOG 1.30] index derived from the name; vanilla still ships `index` attributes [EXACT] | No stripping needed; make the names match the `.xob` exactly. |

### Migration checklist for animation director

- [ ] **Weapon mappings:** check whether the weapon or prop uses a 1.30 remap (`LugerBoneRemap`, `LeeEnfieldBoneRemap`, `dayzplayercfgbase.c:1605-1622`) or a dedicated `.asi` that replaced a generic one (`props/player_main_1h_bandage.asi`, `weapons/player_main_1h_hayhook.asi`, etc., `references/domain-gates.md`).
- [ ] **Vehicle occupants:** On motorcycles, audit relative locking of both hands to handlebars (`AnimSrcNodeIK2`) and feet to footrests across all phases of `HumanCommandVehicle.IsTransitioning()`.
- [ ] **Surrender:** Confirm no offline check or fixture assumes a virtual item in hands during surrender state.
- [ ] **Creature terrain:** Check that animal paw contacts include procedural angular offset of `AnimSrcNodeProcTransform` against slopes (`SlopeAngleX`).
- [ ] **Templates and Instances:** Audit that delivery `.ast` files declare `Name` on all groups and `.asi` files use `AnimSetInstanceSource` class.
- [ ] **Live validation:** [CHANGELOG 1.30] Workbench Enfusion 2021 live editing may help verify continuous blend parameters on an active client before final packaging; not tried on this host.

## Resource index

- [authority-and-routing.md](references/authority-and-routing.md) — authority, precedence, and skill selection.
- [motion-quality-contract.md](references/motion-quality-contract.md) — sample/contract format and catalog of checks.
- [biomechanics-and-contact.md](references/biomechanics-and-contact.md) — anatomy, contact, self-collision, and synchronization.
- [domain-gates.md](references/domain-gates.md) — checks by animation domain.
- [evidence-and-integration.md](references/evidence-and-integration.md) — offline evidence, export, deploy, and in-game.
- `scripts/sample_blender_motion.py` — Blender → neutral JSON report.
- `scripts/validate_motion_contract.py` — report + contract → deterministic audit.
- `scripts/run_regression_tests.py` — synthetic and optional real fixtures.

## Stop conditions

Stop progression and request missing decision or evidence when:

- no input/output reference exists and a choice would alter choreography;
- current pipeline and a reference disagree on skeleton, export mask, or runtime state;
- full mechanical part cannot be identified;
- a tolerance can only be chosen by looking at candidate intended to pass;
- offline gate passes but visual or in-game review fails;
- integration test requires an unavailable tool or permission.
