# Domain gates

> Target: DayZ 1.30 stable (1.30.164078). `P:\…` paths are 1.30, `1.29: O:\…` the old build; every `[EXACT]` below was re-read on 1.30.164078 (first written against 1.30 Exp 164014).

Use only the sections relevant to the task, in addition to the common contract and the current `dayz-animation-pipeline` route.

## Player or humanoid action

- Verify the allowed body layer/export mask before authoring.
- Track centre-of-mass implication, shoulder/clavicle contribution and counter-motion.
- Preserve locomotion/root channels that the runtime layer owns.
- Test stance and camera variants that can change IK/blending.
- Review transitions into and out of the action, not only its central pose.
- **Surrender state (DayZ 1.30 [EXACT]):**
  1.30 surrenders without an item in hands: `PlayerBase.SetSurrenderState(bool)` (`P:\scripts\4_world\entities\manbase\playerbase.c:2116`) and `Man.IsSurrendered()` (`P:\scripts\3_game\entities\man.c:67`), checked in `EmoteManager` (`P:\scripts\4_world\classes\emotemanager.c:314`, `:1246-1252`). `SurrenderDummyItem` still exists but is `[Obsolete("1.30: no replacement - no longer in use")]` (`P:\scripts\4_world\entities\itembase\surrenderdummyitem.c:1`). Before 1.30: surrender spawned that invisible item in hands (1.29: `O:\…\scripts\4_world\classes\emotemanager.c:309`).
  Do not author interactions assuming an invisible dummy weapon in hands during surrender.
- **Dynamic Animation Instance switching [EXACT]:**
  Script can now force transitions between `.asi` instances at runtime via `Human.SetAnimationInstanceByName(string animationInstanceName, float blendingTime)` (`// [EXACT] P:\scripts\3_game\human.c:1384`, new in 1.30). To keep the unarmed base instance loaded, vanilla registers an `"Empty"` hands profile pointing at `player_main.asi` (`// [EXACT] P:\scripts\4_world\entities\manbase\dayzplayer\dayzplayercfgbase.c:1546-1549`); switching back uses the `.asi` path (`playerbase.c:2107`).
- **Stance-based speed and camera rotation limits [EXACT]:**
  `HumanInputController` can clamp movement speed and freeze horizontal camera rotation per stance:
  - `SetErectSpeedLimit(bool bShouldDisable, int pMaxErectSpeed)`, `SetCrouchSpeedLimit(...)`, `SetProneSpeedLimit(...)` (`// [EXACT] P:\scripts\3_game\human.c:230-232`, new in 1.30).
  - `DisableErectCameratHorizontalRotation(bool pDisable)`, `DisableCrouchCameratHorizontalRotation(...)`, `DisableProneCameraHorizontalRotation(...)` (`// [EXACT] P:\scripts\3_game\human.c:240-242`, new in 1.30).
  Author transitions knowing that extreme horizontal looking can be restricted natively during heavy or prone gestures.
- **Animation tag evaluation [EXACT]:**
  Active graph tags can be queried via `HumanAnimInterface.IsTag(TAnimGraphTag iTag)` (`// [EXACT] P:\scripts\3_game\human.c:319`, new in 1.30; [CHANGELOG 1.30] on the fixed tick).
- **New Full-Body action constants (DayZ 1.30 [EXACT]):**
  DayZ 1.30 registers 11 new full-body actions in `DayZPlayerConstants` (`// [EXACT] P:\scripts\3_game\dayzplayer.c:889-899`; the two modifier commands at `:766-767`; no existing value changed, `CMD_ACTIONMOD_PICKUP_HANDS` is still 500 at `:770`):
  - `CMD_ACTIONFB_COMBINATIONLOCK = 256` (prone combination lock manipulation, paired with `CMD_ACTIONMOD_COMBINATIONLOCK = 256` for erect/crouch)
  - `CMD_ACTIONFB_WASHFACEPOND = 257` (crouch face washing, paired with `CMD_ACTIONMOD_WASHFACE`, implicit 257)
  - `CMD_ACTIONFB_BUILDROPELADDER = 258` (crouch crafting rope ladder)
  - `CMD_ACTIONFB_BRICKSTACK = 259` (erect/crouch brick stacking)
  - `CMD_ACTIONFB_BRICKTROWEL = 260` (erect/crouch trowel masonry)
  - `CMD_ACTIONFB_MIX_MORTAR = 261` (crouch bucket mortar mixing)
  - `CMD_ACTIONFB_DRINKWELL_BUCKET = 262` (erect drinking from well bucket)
  - `CMD_ACTIONFB_SHARPEN = 263` (crouch stone tool sharpening)
  - `CMD_ACTIONFB_WASHFACEWELL = 264` (erect well face washing)
  - `CMD_ACTIONFB_WETCLOTH = 265` (crouch cloth wetting at pond)
  - `CMD_ACTIONFB_WETCLOTHWELL = 266` (erect cloth wetting at well)

## Hand with weapon or object

- Apply every rule in `biomechanics-and-contact.md`.
- Identify the functional moving assembly before posing.
- Define jammed/closed/open or equivalent mechanical states from verified geometry/data.
- Keep contact lock until the declared release; after release enforce clearance.
- Validate regrip as approach → open hand → enclose target → close, rather than teleporting a closed fist.
- Compare start/end against the real runtime IK pose.
- **Hands item decoupling [EXACT]:**
  On 1.30, changing the item in hands does not switch the animation instance when `pChangeAnimationInstance = false` is passed to `HumanItemAccessor.OnItemInHandsChanged(bool pInstant = false, bool pChangeAnimationInstance = true)` (`// [EXACT] P:\scripts\3_game\humanitems.c:112`; 1.29 had only `pInstant`) or when `EnableAutoAnimInstUpdateOnHandsChange(false)` is set (`humanitems.c:115`). Script passes both flags through `Man.OnItemInHandsChangedEx(bool, bool)` (`P:\scripts\3_game\entities\man.c:168`).
- **Dedicated weapon and tool `.asi` profiles [EXACT]:**
  DayZ 1.30 moves several tools from generic instances to dedicated `.asi` files and adds weapons with their own (`P:\scripts\4_world\entities\manbase\dayzplayer\dayzplayercfgbase.c`, line per profile; 1.29 instance in brackets):
  - Bandage (`BandageDressing`): `props/player_main_1h_bandage.asi` (`:1251`; 1.29: `player_main_1h.asi`)
  - Sickle: `props/player_main_1h_sickle.asi` (`:747`; 1.29: `player_main_1h_pipe.asi`)
  - HayHook: `weapons/player_main_1h_hayhook.asi` (`:753`; 1.29: `player_main_1h_pipe.asi`)
  - Hatchet: `weapons/player_main_1h_hatchet.asi` (`:757`; 1.29: `player_main_1h_pipe.asi`)
  - Pitchfork: `weapons/player_main_2h_pitchfork.asi` (`:807`; 1.29: `player_main_2h_spear.asi`)
  - Sword: `weapons/player_main_2h_sword.asi` (`:809`; 1.29: `player_main_2h_bat.asi`)
  - Luger P08 (new weapon): `weapons/player_main_luger08.asi` (`:395`; `LugerBoneRemap` at `:1621-1622`)
  - MP18 (new weapon): `weapons/player_main_mp18.asi` (`:432`)
  - Lee-Enfield (new weapon): `weapons/player_main_leeEnfield.asi` (`:446`; `LeeEnfieldBoneRemap` at `:1605-1606`)
  - `SCARH_Base`: `weapons/player_main_SCARH.asi` (`:444-445`) — not new, 1.29 had it (1.29: `dayzplayercfgbase.c:442`). (corrected on 1.30.164078: this list used to call it SCAR-L and count it as new.)
  Audit hand poses against the exact weapon/prop `.asi` profile rather than assuming generic `player_main_1h.asi` or `player_main_2h.asi`.

## Locomotion

- Define gait, stride, cadence, stance and root-motion ownership.
- Check foot locking, ground penetration, slip distance and support transitions.
- Track pelvis/COM arcs and upper-body counter-rotation.
- Match cycle pose and velocity at the seam.
- Test slopes/turning only if the DayZ graph or runtime consumer will exercise them.
- Audit transitions under posture speed limits (`SetErectSpeedLimit`, `SetCrouchSpeedLimit`, `SetProneSpeedLimit` in `human.c:230-233`). When speed limits are enforced, root motion scaling must not cause foot sliding.

## Creature

- Obtain the actual skeleton, rest pose and gait reference for that species/rig.
- **Skeleton bone index (DayZ 1.30):**
  [CHANGELOG 1.30] the 250-bone limit is removed and the global bone index is derived from the bone name. [EXACT] The vanilla `P:\DZ\anims\cfg\skeletons.anim.xml` was not de-indexed: `hermit_newbindpose.xob` keeps 24 `index` attributes (`:5-17` onwards, as in 1.29), and every skeleton carries `index = "0" movement = "true"` on `EntityPosition` (e.g. `:986-988`); before 1.30 the animal skeletons had no `index` at all. (corrected on 1.30.164078: this item used to say only `EntityPosition` keeps an index.)
  Four new animal skeletons are introduced in DayZ 1.30: `ovis_gmelini_skeleton.xob` (mouflon), `canis_familiaris_dobermann_skeleton.xob`, `canis_familiaris_german_shepherd_skeleton.xob`, and `varanus_griseus_skeleton.xob` (`skeletons.anim.xml:986-1334`; corrected from `:986-1150`).
- **Graph modularity (`.agf` files) [EXACT]:**
  Creature graphs migrate from old-format text `.agr` sub-graphs to Enfusion Config `.agf` files (`wolf_maingraph.agf`, `ambientlife_maingraph.agf`, `herbivores_maingraph.agf`; infected: `DZ/anims/workspaces/infected/infected_main/locomotion.agf`) referenced by master `AnimSrcGraph` `.agr` indexes (`P:\DZ\animals\animations\!graph_files\wolf\wolf_graph.agr:157-159`). (corrected 2026-09-28: this line used to say "monolithic binary `.agr`". In the 1.29 extraction the wolf graph is already two text files, `Wolf_Graph.agr` listing `Wolf_MainGraph.agr` at `:58`, both opening with `$AnimGraph 7 {`, and all six 1.29 animal `.agr` files are text.)
- **Procedural terrain alignment (`AnimSrcNodeProcTransform`) [EXACT]:**
  Quadrupeds use procedural bone transforms driven by terrain slope:
  ```enfusion
  // [EXACT] P:\DZ\animals\animations\!graph_files\wolf\wolf_maingraph.agf:5-18
  AnimSrcNodeProcTransform AlignToTerrain_Rot {
   EditorPos -1 -4.7
   Child "Master_SM"
   Expression "SlopeAngleX  * 0.01745329251994329576923690768489"
   Bones {
    AnimSrcNodeProcTrBoneItem "{6930A8DF8B23795C}" {
     Bone "scene_root"
     Axis X
     Space Model
     Op Rotate
     Amount 1
    }
  ```
  Validate that root and collarbone rotation compensations match the terrain slope angle (`SlopeAngleX`) to prevent feet penetrating slopes.
- **Prediction and 2D Look poses [EXACT]:**
  Creatures use `AnimSrcNodePrediction` on `EntityPosition` (`PercentTime 0.3`, `wolf_maingraph.agf:52-55`) for translational and rotational foot prediction, and `AnimSrcNodePose2` for head tracking: the infected read `LookDirX`/`LookDirY` (`DZ/anims/workspaces/infected/infected_main/locomotion.agf:5-13`), the wolf `lookXAngle`/`lookYAngle` (`wolf_maingraph.agf:2024-2032`). (corrected on 1.30.164078: this line used to give `LookDirX/Y` for creatures in general.)
- **Infected mental states [EXACT]:**
  DayZ 1.30 adds `MINDSTATE_COWER` (`// [EXACT] P:\scripts\3_game\entities\dayzinfected.c:18`; absent in 1.29), enabling cowering/fear behavior. Look blending uses `AnimSrcNodeBlendT AggroLookBlendT` conditioned on `Look` (`// [EXACT] P:\DZ\anims\workspaces\infected\infected_main\locomotion.agf:19-31`).

## Vehicle occupant

- Source seat, grip, pedal/footrest and steering anchors from actual model data.
- Keep hands/feet locked to moving controls through their travel.
- Audit elbow/knee solutions at steering extremes and animation transitions.
- Validate get-in/out approach side and clearance separately from the seated loop.
- Delegate anchor extraction and vehicle runtime behavior to their domain skills.
- **Motorcycles and 2-wheelers (DayZ 1.30 [EXACT]):**
  - **Animation instances:** `P:\scripts\4_world\entities\vehicles\vehicleaniminstances.c:13-14` registers `MOTO1 = 10` (`Motorbike_01`, "Jana 50"; graph column `Jawa_05`) and `MOTO2 = 11` (`Motorbike_02`, "Bitrak 682"; graph column `Jawa_Bitrak`). Both are two-wheelers (front and rear wheels, `P:\DZ\vehicles\singletrack\config.cpp`). (corrected on 1.30.164078: this line used to call the Bitrak a 3-wheeler.)
  - **Camera:** 3rd person motorcycle camera with spring lag is registered as `DAYZCAMERA_3RD_VEHICLE_MOTORBIKE = 32` (`// [EXACT] P:\scripts\4_world\entities\manbase\dayzplayer\dayzplayercameras.c:20`).
  - **Handlebar IK:** Hand grip choreography uses two-bone IK nodes (`AnimSrcNodeIK2` / `AnimSrcNodeIK2Target`, `P:\DZ\anims\workspaces\player\player_main\vehicles.agf:17-46`) that re-solve the hand chains toward where the steering-pose clip puts them; they read no vehicle steering pivot or memory point (`dayz-animation-pipeline` → `references/vehicle-rider-ik-pose.md`). (corrected on 1.30.164078: this line used to say the IK is attached to the steering pivots.)
  - **Rider dynamics:** the graph declares `VehicleSteering`, `VehicleThrottle`, `VehicleLean` and `VehicleSuspension` (`player_main.agr:113`, `:118`, `:197`, `:205`); the steering pose is read from `VehicleSteering` over ±45° of wheel angle (`vehicles.agf:50`). Which lean/spine layers use the others was not traced here.
  - **Transition queries:** Script queries transitions cleanly via `HumanCommandVehicle.IsTransitioning()` (`// [EXACT] P:\scripts\3_game\human.c:735-738`, new in 1.30), checking `IsGettingIn()`, `IsGettingOut()`, or `IsSwitchSeat()`.
  - **Crash and ejection:** Rider fall/crash ejection currently delegates to unconsciousness (`eModifiers.MDF_UNCONSCIOUSNESS` at `// [EXACT] P:\scripts\4_world\entities\dayzplayerimplement.c:2458`, comment at `:2454-2455`) pending full ragdoll command support.

## Mechanical object or prop

- Identify selection/hierarchy, pivot/axis, limits and all attached sub-pieces.
- Use physically plausible acceleration, hard-stop, overshoot and damping for the mechanism.
- Check travel envelope and object-object collisions.
- Confirm whether DayZ drives it through `model.cfg`, script, weapon bones or skeletal animation; this decision belongs to `dayz-animation-pipeline`.
- **Graph and Template constraints (DayZ 1.30 [EXACT]):**
  - **No Buffer nodes:** [CHANGELOG 1.30] `Buffer Save` and `Buffer Use` nodes are removed from animation graphs; [EXACT] the 1.29 player graph had them (1.29: `locomotion.agr:2415-2422`) and no 1.30 graph file does. State persistence must use control variables (`AnimSrcGCTVar*`).
  - **Named AST groups:** [CHANGELOG 1.30] the default unnamed animation group was removed from `.ast` templates; vanilla 1.30 names every group (`Name "Default"`, `Name ".unnamed"`, see `dayz-animation-pipeline` → `references/anim-graph.md`). Give every group an explicit `Name`.
  - **Enfusion Config ASI:** vanilla 1.30 instance files use `AnimSetInstanceSource` syntax rather than legacy `$animsetinstance`; write new ones that way (whether the old syntax still loads was not tested).

## Physics simulation

- Use `blender-animation` for simulation setup and bake discipline.
- Bake once before sampling; repeated unbaked evaluation is not deterministic evidence.
- Validate contacts and energy/settling after bake.
- Confirm the export route can carry the baked result into DayZ; do not assume Blender-only deformation survives.
- **Ragdoll vs Simple Death separation (DayZ 1.30 [EXACT]):**
  [CHANGELOG 1.30] Workbench includes a **ragdoll editor** for `.ragdoll` files. [EXACT] Vanilla ships `P:\DZ\characters\bodies\human.ragdoll`, bound by `ragdoll=` in `SurvivorBase` `enfAnimSys` (`P:\DZ\characters\data\config.cpp:160`).
  Scripts separate animated death from ragdoll simulation:
  - `PhysicsSetSimpleDeath(bool pEnable)` handles animated death (`// [EXACT] P:\scripts\3_game\human.c:1448-1449`, new in 1.30).
  - `PhysicsSetRagdoll(bool pEnable)` and `PhysicsIsRagdoll()` handle physical ragdoll simulation (`// [EXACT] P:\scripts\3_game\human.c:1451-1452`; `PhysicsIsRagdoll` is new in 1.30).
  - `HumanCommandUnconscious.IsRagdoll()` queries whether an unconscious character is ragdolling (`P:\scripts\3_game\human.c:647`, new in 1.30).
  When authoring death animations, note that vanilla's non-command death branch calls `PhysicsSetSimpleDeath(true)` (`P:\scripts\4_world\entities\dayzplayerimplement.c:745`); when a death command can start, `StartCommand_Death` plays instead (`:740-741`). Before 1.30 that branch called `PhysicsSetRagdoll(true)` (1.29: `O:\…\dayzplayerimplement.c:736`).
