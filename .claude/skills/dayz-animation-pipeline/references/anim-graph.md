# Anim graph, state machines, commands & ASI structure (sprint 2026-05-28)

The animation **graph** is the layer DayZ uses to drive characters and creatures: state machines, transitions, commands fired by the engine/AI/script, variables that conduct blending, and the **ASI** (animsetinstance) catalog that maps state names to `.anm` files. This reference covers the parts of that system that you can read in plain text from vanilla unpacked data — and refuses the parts that don't exist.

> Target: DayZ 1.30 stable (1.30.164200). `DZ/…` paths below are read on `P:\DZ\…` (1.30); `1.29: O:\…\DZ\…` marks the old build. Every `[VERIFIED-vanilla]` / `[EXACT]` item was re-read on 1.30.164078 unless it says otherwise.

The `[VERIFIED-vanilla]` items were first greped against the unpacked vanilla in `DZ/` + `SurvivorAnims/` (sprint 2026-05-28, 1.29-era files); they were re-checked on 1.30 and rewritten where the 1.30 file format differs. Cross-ref vault: `20_Knowledge/dayz-animations-creatures-weapons.md`.

## Where the source of truth lives in vanilla

| Topic | Vanilla path (1.30) | Format |
|---|---|---|
| Player master graph index | `DZ/anims/workspaces/player/player_main/player_main.agr` [EXACT] | text Enfusion Config (`AnimSrcGraph`; variables and commands in `ControlTemplate AnimSrcGCT` at `:4-419`; sub-graph list `GraphFilesResourceNames` at `:1587-1589`) |
| Player sub-graphs (Locomotion, Vehicles, Actions, Combat...) | `DZ/anims/workspaces/player/player_main/*.agf` [EXACT] | text Enfusion Config (`AnimSrcGraphFile`). Before 1.30: `.agr` files, also text, in the old `$AnimGraph 7` format (corrected 2026-09-28, see §Vehicles.agf) |
| Player commands + transitions, old format | `SurvivorAnims/animgraph/player_main/*.agr`, or 1.29: `O:\…\DZ\anims\workspaces\player\player_main\*.agr` | text (`$AnimGraph 7`) — only to read a ≤1.29 graph mod |
| Animal/infected/predator graphs | `DZ/animals/animations/!graph_files/<species>/*_graph.agr` (master) + `*_maingraph.agf` (nodes); infected: `DZ/anims/workspaces/infected/infected_main/` | text Enfusion Config |
| Skeleton LOD and missing bones | `DZ/anims/cfg/skeletons.anim.xml` | XML; [CHANGELOG 1.30] global index derived from the bone name, 250-bone limit removed; the file still carries `index` attributes (`player-skeleton.md` §1.30) |
| ASIs (player + props + weapons) | `DZ/anims/workspaces/player/player_main/` | text Enfusion Config (`AnimSetInstanceSource`). Before 1.30: `$animsetinstance` |
| Event table | `DZ/anims/workspaces/player/Player_EventTable.ae` | text |
| Workspaces | `DZ/anims/workspaces/player/player_main/*.aw` (1.30: `BaseSource { … }`, 20 lines, `EventTable` at `player_main.aw:15`); `SurvivorAnims/animgraph/player_main/*.aw` (old source) | text |

You can `grep` all of these in-sandbox if the vanilla data is unpacked on the user's `P:\`. The `.anm` files themselves are binary — leave them to DayZATool / Workbench.

## Commands — `CMD_*` UPPER_SNAKE [VERIFIED-vanilla]

Casing is real and exact: prefix `CMD_`, `UpperCamel` suffix. Community tutorials and video transcripts often render these as `cmd death` or `cmd_death` — **wrong**, use the column from the table.

### Animal commands

Source: `DZ/animals/animations/!graph_files/ambientlife/ambientlife_graph.agr` (1.30: declared as `AnimSrcGCTCmd CMD_Death { … }` etc. at `:65-83`). Same set appears in herbivores/predators/wolf graphs.

| Command | Triggers |
|---|---|
| `CMD_Death` | death state |
| `CMD_LookAt` | head/look tracking towards target |
| `CMD_LookAtXChange` | sub-command for look-at axis change |
| `CMD_Attack` | enter attack state |
| `CMD_AttackSuccess` | attack connected → apply damage. **There is no `CMD_Success` alone** — community tutorials get this wrong |
| `CMD_Hit` | recv hit |
| `CMD_AnimCallBack` | generic event callback |

### Player commands

Source (1.30): the `Commands` block of `DZ/anims/workspaces/player/player_main/player_main.agr:211-419` (`AnimSrcGCTCmd CMD_Reload_Magazine` at `:269` … `CMD_WeaponFire` at `:365`); used in the `.agf` sub-graphs. Before 1.30: `SurvivorAnims/animgraph/player_main/{combat,locomotion,player_main}.agr`.

| Command | Triggers |
|---|---|
| `CMD_WeaponFire` | weapon fire (value 2 = fire while cocked-empty: state `FireCocked` starts on `GetCommandI(CMD_WeaponFire) == 2`, `combat.agf:1120-1126`; 1.29: `combat.agr:850`) |
| `CMD_Reload_Magazine` | full magazine reload |
| `CMD_Reload_BoltAction` | bolt-action reload |
| `CMD_Reload_Chambering` | chamber a round |
| `CMD_Reload_ChamberingFast` | fast chambering variant |
| `CMD_Reload_Clip` | clip reload |
| `CMD_Modifier_Additive` | **state modifier** for sickness/cough/sneeze (`locomotion.agf:5482-5520`; 1.29: `locomotion.agr:3939-3956`). **NOT for reload** — see the refutation below |

### New player action commands in 1.30 [EXACT]

1.30 adds 2 modifier and 11 full-body action commands to `DayZPlayerConstants` (`P:\scripts\3_game\dayzplayer.c:766-767`, `:889-899`). No existing value changed: `CMD_ACTIONMOD_PICKUP_HANDS` is still 500 (`:770`; 1.29: `:768`). `CMD_ACTIONMOD_WASHFACE` has no explicit value and no trailing comma in the file (`:767`), so it takes 257 after `COMBINATIONLOCK = 256`.

| Command | Constant Value | Stance / Context (`dayzplayer.c`) |
|---|---|---|
| `CMD_ACTIONMOD_COMBINATIONLOCK` | `256` | Mod action: combination lock (erc, cro) (`dayzplayer.c:766`) |
| `CMD_ACTIONMOD_WASHFACE` | `257` | Mod action: wash face (`dayzplayer.c:767`) |
| `CMD_ACTIONFB_COMBINATIONLOCK` | `256` | Full-body: combination lock (pne) (`dayzplayer.c:889`) |
| `CMD_ACTIONFB_WASHFACEPOND` | `257` | Full-body: wash face in pond (cro) (`dayzplayer.c:890`) |
| `CMD_ACTIONFB_BUILDROPELADDER` | `258` | Full-body: build rope ladder (cro) (`dayzplayer.c:891`) |
| `CMD_ACTIONFB_BRICKSTACK` | `259` | Full-body: brick stacking (erc, cro) (`dayzplayer.c:892`) |
| `CMD_ACTIONFB_BRICKTROWEL` | `260` | Full-body: brick trowel (erc, cro) (`dayzplayer.c:893`) |
| `CMD_ACTIONFB_MIX_MORTAR` | `261` | Full-body: mix mortar in bucket (cro) (`dayzplayer.c:894`) |
| `CMD_ACTIONFB_DRINKWELL_BUCKET` | `262` | Full-body: drink from well bucket (erc) (`dayzplayer.c:895`) |
| `CMD_ACTIONFB_SHARPEN` | `263` | Full-body: sharpen tools with stone (cro) (`dayzplayer.c:896`) |
| `CMD_ACTIONFB_WASHFACEWELL` | `264` | Full-body: wash face at well (erc) (`dayzplayer.c:897`) |
| `CMD_ACTIONFB_WETCLOTH` | `265` | Full-body: wet cloth in pond (cro) (`dayzplayer.c:898`) |
| `CMD_ACTIONFB_WETCLOTHWELL` | `266` | Full-body: wet cloth at well (erc) (`dayzplayer.c:899`) |

## Variables — `ControlTemplate AnimSrcGCT` [VERIFIED-vanilla 1.30]

1.30 graphs declare variables in the root `.agr` (`AnimSrcGraph`) under `ControlTemplate AnimSrcGCT { Variables { … } }`, one typed block per variable. Examples from `DZ/animals/animations/!graph_files/herbivores/herbivores_graph.agr`:

- `AnimSrcGCTVarFloat speed { DefaultValue 0 MinValue 0 MaxValue 5 }` (`:11-15`) — used by the locomotion blend tree.
- `AnimSrcGCTVarFloat SlopeAngleX { … MinValue -90 MaxValue 90 }` (`:36-40`) — provided by the engine, drives terrain alignment.
- `AnimSrcGCTVarFloat SlopeAngleZ` (`:41-45`) — same.

Before 1.30: the same variables were `#Var <name> <type> <default> <min> <max> ""` lines, e.g. `#Var speed float 0.0 0.0 5.0 ""` (1.29: `O:\…\herbivores\herbivores_graph.agr:17`, `SlopeAngleX/Z` at `:22-23`).

`swimming` is **NOT** a variable in any animal graph. In the player it appears as a state tag (`TagSwimming` on `SwimmingMaster`, `locomotion.agf:7700-7702`), not as a numeric variable. Community tutorials that treat `swimming` as a creature variable are inventing it.

### ControlTemplate in AnimSrcGraph (.agr) [EXACT]

The wolf master graph, as shipped in 1.30:

```enfusion
// [EXACT] P:\DZ\animals\animations\!graph_files\wolf\wolf_graph.agr:4-16
 ControlTemplate AnimSrcGCT "{6930A8DF8B23ACB9}" {
  Variables {
   AnimSrcGCTVarFloat speed {
    DefaultValue 0
    MinValue 0
    MaxValue 5
   }
   AnimSrcGCTVarFloat turn {
    DefaultValue 0
    MinValue -180
    MaxValue 180
   }
   AnimSrcGCTVarInt behaviorAction {
```

## Terrain alignment [VERIFIED-vanilla 1.30]

There is no special "terrain alignment node". It's a procedural transform node, `AnimSrcNodeProcTransform`, in the creature's `*_maingraph.agf`, whose `Expression` consumes `SlopeAngleX/Z` multiplied by `0.01745329…` (= π/180, degrees → radians):

- Wolf: `AlignToTerrain_Rot` (`DZ/animals/animations/!graph_files/wolf/wolf_maingraph.agf:5-18`, block below).
- Herbivores: `TerrainRot_BoarAndPig`, `TerrainRot_CowAndBull`, `TerrainRot_Deers`, `TerrainRot_SheepAndGoat` (`herbivores_maingraph.agf:2407`, `:2449`, `:2505`, `:2547`) — same shape.

Before 1.30: the same node was an `AnimNodeRot` line in `wolf_maingraph.agr` (1.29: `O:\…\wolf\wolf_maingraph.agr:4`, `"AlignToTerrain_Rot" "" "Master_SM" "SlopeAngleX  * 0.01745329…"`).

For a new creature, **copy the formula from the vanilla animal closest in proportions** (large quadruped → cow, medium → boar, small → sheep). Don't reinvent.

### AnimSrcNodeProcTransform in .agf [EXACT]

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
      AnimSrcNodeProcTrBoneItem "{6930A8DF8B237969}" {
       Bone "lflegcollarbone"
```

## ASI — `AnimSetInstanceSource` structure [VERIFIED-vanilla 1.30]

ASIs map `StateName.SubName.Phase` → `{GUID}path.anm`. They form a parent chain (one base `.asi` + children that override). In 1.30 each file is an `AnimSetInstanceSource` (block below):

- `Template` → the `.ast` (animset template) that defines which states this instance can populate.
- `ParentTemplates { "<.asi>" }` → the `.asi` to inherit from. `player_main.asi` is the root of the player chain.
- `Lines { AnimSetInstanceSource_Line "<state>" { Resource "<.anm>" } }` → the state-to-anm mapping. State path is dotted: `Category.SubCategory.StateName`.

Before 1.30 (what a ≤1.29 mod ships; 1.29: `O:\…\DZ\anims\workspaces\player\player_main\player_main_rifle.asi:1-5`):

```
$animsetinstance {
  #template "{GUID}DZ/anims/workspaces/player/player_main/player_main.ast"
  #nparents 1
  #parent  "{GUID}DZ/anims/workspaces/player/player_main/player_main.asi"
  $animations {
    "ActionContinuous.BlowFireplaceCro.In" "{GUID}DZ/anims/anm/player/..."
    ...
  }
}
```

### AnimSetInstanceSource (.asi) [EXACT]

```enfusion
// [EXACT] P:\DZ\anims\workspaces\player\player_main\player_main_rifle.asi:1-15
AnimSetInstanceSource {
 Template "{F0C651DE8E24A5DE}DZ/anims/workspaces/player/player_main/player_main.ast"
 ParentTemplates {
  "{0F5E6205A5E823C3}DZ/anims/workspaces/player/player_main/player_main.asi"
 }
 Lines {
  AnimSetInstanceSource_Line "ActionContinuous.BlowFireplaceCro.In" {
   Resource "{DACEA97B45382D18}DZ/anims/anm/player/actions/items/p_rfl_cro_blow_fireplace_in.anm"
  }
  AnimSetInstanceSource_Line "ActionContinuous.BlowFireplaceCro.Loop" {
   Resource "{71CBDFD73BA4352D}DZ/anims/anm/player/actions/items/p_rfl_cro_blow_fireplace_loop.anm"
  }
  AnimSetInstanceSource_Line "ActionContinuous.BlowFireplaceCro.Out" {
   Resource "{17BC83680E9A434B}DZ/anims/anm/player/actions/items/p_rfl_cro_blow_fireplace_out.anm"
  }
```

### Player ASI catalog [VERIFIED-vanilla `DZ/anims/workspaces/player/player_main/`]

| ASI | Purpose |
|---|---|
| `player_main.asi` | base / root parent for all |
| `player_main_1h.asi` | one-handed items/weapons |
| `player_main_1h_restrained.asi` | one-handed + restrained |
| `player_main_2h.asi` | two-handed items/weapons |
| `player_main_heavy.asi` | heavy items (wheel/door/barrel) — target of `AddItemInHandsProfileIK` for heavy |
| `player_main_pistol.asi` | pistols |
| `player_main_rifle.asi` | rifles |
| `player_main_bow.asi` | bow (partial, see vol.3 caveat — additive locomotion unfinished) |
| `player_main_surrender.asi` | hands up |
| `menu_rifle.asi` | rifle in menu/preview |
| `props/*.asi` | 30+ ASIs, one per prop |
| `weapons/*.asi` | one per specific weapon (`player_main_akm.asi`, `player_main_1911.asi`, ...) |

### Weapon state path convention [VERIFIED-vanilla]

Weapon-specific ASIs use `WeaponOperations.<RigKey>.<StateName>` where `<RigKey>` encodes pose + rail accessory system (e.g. `ErcRas` = erected + rail-accessory).

Example (`weapons/player_main_1911.asi:19-20`, `:52-53` on 1.30):

| State path | Maps to |
|---|---|
| `WeaponOperations.ErcRas.FireCocked` | `p_erc_empty_cocked_1911_ras.anm` |
| `WeaponOperations.ErcRas.ReloadMagazineDetach` | `p_erc_reload_mag_remove_1911_ras.anm` |

Real state names corrected from community/video tutorial naming:

| Tutorials call it | Real state | Notes |
|---|---|---|
| "weapon cocked" | `FireCocked` | fired while cocked-empty |
| "mag remove" | `ReloadMagazineDetach` | the `.anm` filename has `_mag_remove_` but the state name is `ReloadMagazineDetach` |
| "bullet in chamber" | **no such state** | chambering uses commands (`CMD_Reload_Chambering`, `CMD_Reload_ChamberingFast`), not a state name |

## Vehicles.agf & Enfusion .agf format (added 2026-09-16; re-read on 1.30.164078) [EXACT]

In 1.30 the player sub-graphs live in `.agf` text files (`AnimSrcGraphFile`) referenced by `GraphFilesResourceNames` in `player_main.agr:1587`. The vehicle graph `DZ/anims/workspaces/player/player_main/Vehicles.agf` (3654 lines) is the reference implementation. Root for the citations below: `P:\DZ\anims\workspaces\player\player_main\` (first read in the 1.30 Exp 164014 extract; every line below holds on stable).

```
// Vehicles.agf:1564-1576
AnimSrcNodeBlendT MotorBikeB {
 EditorPos 4 -18.3
 BlendTime "0.3"
 BlendFn S
 TriggerOn ""
 TriggerOff ""
 Condition "VehicleType == 10 || VehicleType == 11"
 Child0 "VehicleSTM"
 Child1 "MotorBikeSTM"
 OptimizeMin 1
 OptimizeMax 1
 SelectMainPath 0
}
```

Node types seen in the file (each with the line where the first instance starts):
- `AnimSrcNodeBlendN`: multi-input threshold blend (`BlendWeight "VehicleSpeed"`, `Thresholds { 3.5 8 }`, `Vehicles.agf:5-16`).
- `AnimSrcNodeBlendT`: conditional transition selector (`Condition`, `BlendTime`, `Child0`/`Child1`, `Vehicles.agf:1564`).
- `AnimSrcNodeBlend`: continuous 2-input formula blend (`MotoYIntertiaB`, `Vehicles.agf:1556`).
- `AnimSrcNodePose`: 1D static pose evaluated along its timeline by a variable (`Vehicles.agf:47-51`).
- `AnimSrcNodeStateMachine`: hierarchical state machine with `states { AnimSrcNodeState ... }` and `transitions { AnimSrcNodeTransition ... }` (`MotorBikeSTM`, `Vehicles.agf:1582-1751`; the node's closing brace is at `:1751`) [EXACT][CLAIM-ANIM-MOTORBIKE-STM-RANGE].
- `AnimSrcNodeIK2Target` and `AnimSrcNodeIK2`: two-bone IK pinning the driver's hands to the handlebar (`AnimNodeIK2Target0` at `Vehicles.agf:17`, `AnimNodeIK2hands` at `:31`).
- `AnimSrcNodeSourceSync`: clip playback bound to a sync line (`Vehicles.agf:69-80`).

`MotorBikeSTM` rider states: `Idle`, `GetIn_L`, `GetIn_R`, `GetOut_L`, `GetOut_R`, `JumpOut_L`, `JumpOut_R`, `Death`, `GettingInDeath` (`Vehicles.agf:1582-1751`) [EXACT]; the get-in states start on integer direction flags (`GetIn_L`: `GetCommandI(CMD_Vehicle_GetIn) == 0`, `GetIn_R`: `== 1`). The rider types are `VehicleAnimInstances.MOTO1 = 10` and `MOTO2 = 11` (`P:\scripts\4_world\entities\vehicles\vehicleaniminstances.c:13-14`, new in 1.30; the 1.29 enum ends at `ZODIAC = 9`), and the graph declares `VehicleType` with `MaxValue 12` (`player_main.agr:108-112`).

(corrected 2026-09-28) [EXACT][CLAIM-ANIM-GRAPH-TEXT-129] This paragraph used to end by saying the 1.29 `.agr` was binary and could not be diffed in text. It was text. In the 1.29 stable extraction (build 1.29.0.163709, root `stable-1.29\anims_workspaces\DZ\anims\workspaces\player\player_main\`; the same lines on this machine's 1.29 unpack `O:\DayZ 1.29 Старый распакованный клиент\DZ\anims\workspaces\player\player_main\`), `player_main.agr:1` is `$AnimGraph 7 {`; `:99` declares `#Var VehicleType int -1 -1 10 ""` and no `Jawa_*` column exists anywhere in that graph, so the motorbike rider is new in 1.30; `:540-547` list eight sub-graphs as `.agr` files, among them `Vehicles.agr` (`:544`), which also opens with `$AnimGraph 7 {`. None of the nine files holds a NUL byte. What 1.30 changed is the syntax, not text versus binary: 1.29 writes `$Node AnimNodeGroupSelect {` with positional fields (`"Boat_01ActionG" "" "VehicleTalkingT" "VehicleActions" "Boat_01"`, `Vehicles.agr:3-4`), 1.30 writes named ones (`AnimSrcNodeGroupSelect Jawa_BitrakActionG { ... Group "VehicleActions" Column "Jawa_Bitrak" }`, `Vehicles.agf:1381-1386`). A line diff across the two formats is noise; compare parsed nodes, variables and commands instead. Bohemia's warning when 1.30 Exp shipped says the same: in 1.29 a graph mod could be compared with vanilla in a text editor, and for 1.30 graph mods must redo their graph changes from scratch, with new limits such as the number of groups ([@DayZ, 2026-09-16](https://x.com/DayZ/status/2100152871580823661)).

A measured instance of that warning (2026-09-24, dedicated server; measured on 1.30 Exp 164014): the Workshop mod Survivor Animations (2918418331) replaces the player graph — the Workshop build tested, of 2026-07-20, predates 1.30 — and the server CRASHED at character creation with it loaded (`CreateCharacter`, native fault; twice with a dependent addon, once with ONLY the mod and a vanilla-init mission — nothing of ours in the stack). Before building on a third-party graph mod in a newer game version, load it ALONE with a vanilla mission and check a player can join; with a native fault under third-party mods, that single-mod isolation run is the cheapest way to split "theirs" from "ours". Consume its vehicle anim types via a runtime `ConfigIsExisting("CfgPatches <Mod>")` check, never `#ifdef` (see enforce-script-reference). [EXACT] (measured on 1.30 Exp 164014; not repeated on stable)

## Reload is NOT additive in vanilla [REFUTED-vanilla]

Some community tutorials say "reload is an additive animation: only torso/shoulders, the rest is handled by another layer". Vanilla does not confirm this:

- `CMD_Modifier_Additive` (declared at `player_main.agr:338`, applied in `locomotion.agf:5482-5520`; 1.29: `player_main.agr:48`, `locomotion.agr:3939-3956`) drives **sickness/cough/sneeze** state modifiers (`SickSneezeStanceSTM`, `SickCoughStanceSTM`). Nothing about reload.
- Reloads use dedicated commands without an additive flag: `CMD_Reload_Magazine`, `CMD_Reload_BoltAction`, `CMD_Reload_Chambering`, `CMD_Reload_ChamberingFast`, `CMD_Reload_Clip` (`player_main.agr:269-286`).
- There is no additive node or `TagAdditive` for reload in the player animgraph (0 hits for "additive" in `combat.agf` and `weapons.agf`).

The tutorial may be describing authoring convention in Blender (only animate torso/arms during reload, leave legs to locomotion) — not a runtime additive system. When you author a custom reload, look at the vanilla weapon graph closest in feel and copy how it transitions in/out of the state; don't assume the engine will mix layers for you.

## Workbench Animation Editor — the event table line

[EXACT] 1.30: the workspace is an Enfusion Config `BaseSource { … }` of 20 lines, and `DZ/anims/workspaces/player/player_main/player_main.aw:15` declares its event table as a field: `EventTable "{3037156104937B91}DZ/anims/workspaces/player/Player_EventTable.ae"`. There is no `#eventtable` line any more. [CHANGELOG 1.30] the Workbench Animation Editor moved to the 2021 Enfusion editor with live editing of a running graph and a "Show In Explorer" context action. [UNVERIFIED] Whether the editor opens the 1.30 player workspace without removing that field has not been tried on this host.

Before 1.30 [VERIFIED-vanilla on 1.29] — the tutorial trick "edit the player graph and remove an event table line":

- **Compiled workspace** `player_main.aw` (1.29: `O:\…\DZ\anims\workspaces\player\player_main\player_main.aw:137`, 139 lines) contains:
  ```
  #eventtable "{3037156104937B91}DZ/anims/workspaces/player/Player_EventTable.ae"
  ```
- **Source workspace** `SurvivorAnims/animgraph/player_main/player_main.aw` does NOT have that line.

To edit a 1.29 player graph in the old Workbench Animation Editor: take a `.aw` that has `#eventtable`, delete that line, and the editor will open. Workbench/Workshop re-binds the `.ae` on export/compile.

## Building a creature anim graph — minimal-first workflow

1. Create the graph + state machine with **one state** (idle) and **one anim source** (one `.anm`).
2. Set up the creature in-game with a minimal `model.cfg` (geometry, mass, basic FireGeo). Cross-ref `dayz-p3d-audit` and `dayz-model-pipeline`.
3. Confirm the creature spawns, plays idle, does not crash. **Only then** add walk → run blending by `speed`, terrain alignment, hit, death, attack/success.

Building the whole graph offline and discovering the bug on first spawn is the recurring failure mode — incremental validation is the only protection.

## Creature special bones [VERIFIED-vanilla]

See `references/player-skeleton.md` for the full bone catalog. The two creature-critical bones referenced by anim graphs:

- `EntityPosition` (`skeletons.anim.xml:5`, `movement = "true"`) — the bone the engine reads for predicted entity displacement. Animal graphs reference it explicitly, e.g. wolf's `AnimSrcNodePrediction PredictionTurn { Bone "EntityPosition" … PercentTime 0.3 }` (`wolf_maingraph.agf:52-55`; 1.29: `"PredictionTurn" "EntityPosition"`, `wolf_maingraph.agr:30`).
- `LookAt` (`skeletons.anim.xml:17`) — head/look tracking. The name is literally `LookAt`, not `Pin Look At` or `PinLookAt`.

## Animation tags and HumanAnimInterface.IsTag (new in 1.30) [EXACT]

DayZ 1.30 lets script query the active animation-graph tags via `HumanAnimInterface.IsTag` (`P:\scripts\3_game\human.c:318-319`; `IsTag` is new, `BindTag` existed in 1.29 at `human.c:299`). [CHANGELOG 1.30] says it works on the fixed tick.
```c
// P:\scripts\3_game\human.c:318-319
proto native TAnimGraphTag 			BindTag(string pTagName);
proto native bool                   IsTag(TAnimGraphTag iTag);
```

In the graph, states declare tags with a `Tags { "TagName" }` block. In `Vehicles.agf`: `TagVehicleGetIn` in `GetIn_L` / `GetIn_R` (`Vehicles.agf:1594, 1614`) and `TagVehicleGetOut` in the get-out / jump-out states (`:1604, 1624, 1634, 1651`). Transitions consume them, e.g. the driver death animation is blocked while boarding is in progress: `Condition "IsCommand(CMD_Death) && !IsTag(\"TagVehicleGetIn\")"` (`Vehicles.agf:1719`). [DESIGN] Script can cache the handle with `BindTag("TagVehicleGetIn")` and poll `IsTag(handle)` on the fixed tick to sync gameplay with an animation phase; no vanilla script caller was found in this extraction, so treat the usage pattern as a proposal.

## What this reference does NOT cover

- The exact `.ast` template grammar — vanilla `.ast` files exist but the dialect is not fully documented; treat them as schemas to be copy-edited from the closest vanilla example.
- Player animation **FPS** — not declared in any text file under `SurvivorAnims/animgraph/` or `DZ/anims/workspaces/` (not re-checked on 1.30). Probably baked into the `.anm` binary or set by the exporter. Convention reported by tutorials is 30 fps; assume that and round-trip-test before committing a full set.
- Binary `.anm` / `.xob` internals — out of scope; route via DayZATool / Workbench.

## [2026-06-28] Weapon-anim corrections (verified vs dayzplayer.c / human.c)

### The one-anim-mod wall does NOT apply to weapon anims via the ASI route [VERIFIED-vanilla]

The "only one mod modifying player/creature animations at a time" wall (in this skill's anchor 3, `tooling-and-walls.md` wall #1, and `skeletal-anm-enfusion.md`) applies ONLY to mods that REPLACE the player animation graph (`player_main.aw`/`.agr`, e.g. Expansion-Animations). A custom-weapon animation authored via the ASI route does **NOT** touch the graph and is therefore **conflict-free across mods** — multiple weapon-anim mods coexist.

Why: a player weapon animation reaches the engine through three decoupled layers, none of which edits `.agr`/`.aw`:
1. Author `.txa` → Workbench `.anm`.
2. A per-weapon `.asi` (1.30: `AnimSetInstanceSource`; before 1.30: `$animsetinstance`) maps `WeaponOperations.<RigKey>.<State>` → your `.anm`, inheriting the parent chain (`your.asi` → `player_main_rifle.asi` → `player_main.asi`).
3. Enforce Script `AddItemInHandsProfileIK(itemClass, asi, behavior, ikPose.anm, weaponStates.anm)` binds the weapon classname to the `.asi` + a one-time IK pose + a weapon-states `.anm`; `AddItemBoneRemap(itemClass, pairs[])` maps the weapon's part selections to the player's `Weapon_*` bones.

Because binding is per-item via these script calls (not a graph replacement), each weapon mod registers its own items independently. (`P:\scripts\3_game\dayzplayer.c:243` `AddItemInHandsProfileIK` 5-arg, unchanged from 1.29; vanilla calls in `4_world\entities\manbase\dayzplayer\dayzplayercfgbase.c`, e.g. `TruckBattery` at `:867`; refuted the "needs a full player-anim mod" pessimism. Vehicles were written up as the unsupported exception; corrected 2026-09-28: a child `.asi` swaps one vehicle class's rider pose without a graph change, `vehicle-rider-ik-pose.md`.) When the user's plan is a custom WEAPON animation, do NOT warn about the one-anim-mod wall — it is a false blocker here.

### Weapon-states `.anm` is 2–4 keys/channel, not "exactly 3 frames" [VERIFIED-vanilla]

The 3-frame closed/open/jammed is the AUTHORING intent (one frame per init state); Workbench TRIMS near-identical consecutive keys, so the stored per-channel keycount across 47 vanilla `w_*_states.anm` is 2, 3 or 4. `w_akm_states.anm` = 2 distinct frames @30fps (`Weapon_Bolt` slides f0→f1, `Weapon_Bullet` ejects). All player weapon reload/fire/jam/state/ikpose anims are authored at framerate 30.0; `frame_count = last-key-index + 1`. See `references/weapon-anim-blender-complete.md` for the full AKM frame budget.

### ikpose_* keys live in the WeaponIK graph node, not player_main.ast [VERIFIED-vanilla]

The `ikpose_chainoffset`/`ikpose_weaponoffset`/`ikpose_secchainoffset`/`ikpose_chainmiddledir`/`ikpose_secchainmiddledir` keys (plus two omitted by community sources: `ikpose_chainmiddlediro`, `ikpose_secchainmiddlediro`) are parameters of the weapon-IK node of the graph: 1.30 writes them inside the `Ikconfig` string of `AnimSrcNodeWeaponIk` nodes (`DZ/anims/workspaces/player/player_main/combat.agf:29-35`, `actions.agf:3054-3060`), 14 nodes across 6 `.agf` files; 1.29 had the same 14 `AnimNodeWeaponIK` blocks across 6 `.agr` files (1.29: `combat.agr:24-30`). They are NOT in `player_main.ast`/`.aw` top-level (a grep there returns 0) and NOT in config.cpp/model.cfg. They are explicit IK-role→bone mappings, identical in every vanilla occurrence. Full block in `references/weapon-anim-blender-complete.md`.

## 1.30: Animation Set Templates (.ast) & no unnamed groups

[CHANGELOG 1.30] the default unnamed group of animation set templates was removed. Before 1.30: `.ast` files allowed anonymous groups `$groupType { #ngroupnames 0 ... }`. Whether the 1.30 tools reject an unnamed group or rename it was not tested here.

[EXACT] In 1.30, `.ast` files are `AnimSetTemplateSource` and every vanilla group declares a `Name` (`AnimSetTemplateSource_AnimationGroup`):

```enfusion
// [EXACT] P:\DZ\anims\workspaces\player\player_main\player_main.ast:1216-1224
  AnimSetTemplateSource_AnimationGroup "{694EE03ABF952F04}" {
   Name "WeaponOperations"
   Animations {
    "ChamberingBulletL"
    "ChamberingBulletR"
    "ChamberingCancel"
    "ChamberingFast_Closed"
    "ChamberingFast_Open"
    "ChamberingIn"
```

In vanilla templates, groups that used to be unnamed are named `Name "Default"` (`DZ/anims/workspaces/infected/infected_main/infected.ast:76`, `DZ/animals/animations/!graph_files/wolf/wolf.ast:4`) or `Name ".unnamed"` (`DZ/anims/workspaces/samples/zombie_commonsamples/zow.ast:4`, `DZ/anims/cfg/testworkspaces/zow.ast:4`, `DZ/anims/workspaces/samples/cow_example/cow_sample.ast:4`). (corrected on 1.30.164078: this line used to cite `zoW.ast:643`, which does not exist; the `zow.ast` files have 64 and 92 lines.)

## 1.30: no Buffer Save / Buffer Use nodes [EXACT]

[CHANGELOG 1.30] `Buffer Save` and `Buffer Use` nodes were removed from animation graphs. [EXACT] Confirmed by the files: the 1.29 player graph used them (`AnimNodeBufferSave LocomotionQBufferSave` and `AnimNodeBufferUse LocomotionQBufferUse`, 1.29: `O:\…\player_main\locomotion.agr:2415-2422`, consumed at `master.agr:80`), and no 1.30 `.agf`/`.agr` under `P:\DZ` has a buffer node (the only "buffer" hit is the `outputweaponoffsettobuffer` key inside an `Ikconfig` string, `locomotion.agf:3891`). A ported graph that relied on buffer nodes must be rebuilt with variables (`AnimSrcGCTVar*`) or direct state machine transitions.

## Script animation APIs new in 1.30 [EXACT]

All in `P:\scripts\3_game\` unless stated; each is absent from 1.29 except where noted.

1. **Animation instance control**:
   `proto native void SetAnimationInstanceByName(string animationInstanceName, float blendingTime);` (`human.c:1384`; comment at `:1383`: "careful when using, could create desync if not used properly")
2. **Item-in-hands notifications**:
   `proto native void OnItemInHandsChanged(bool pInstant = false, bool pChangeAnimationInstance = true);` (`humanitems.c:112`; signature changed — 1.29 had only `bool pInstant = false`)
   `proto native void EnableAutoAnimInstUpdateOnHandsChange(bool pNewValue);` (`humanitems.c:115`), `proto native bool IsAutoAnimInstUpdateOnHandsChangeEnabled();` (`:118`)
   `void OnItemInHandsChangedEx(bool pInstant = false, bool pChangeAnimationInstance = true)` on `Man` (`entities\man.c:168`); `Man.OnItemInHandsChanged()` now forwards to it (`man.c:163-166`), and `DayZPlayerImplement` moved its body there (`4_world\entities\dayzplayerimplement.c:1002-1015`). Before 1.30: `DayZPlayerImplement.OnItemInHandsChanged()` held the body (1.29: `dayzplayerimplement.c:988-1001`).
3. **Locomotion speed limits & camera horizontal locks** (`HumanInputController`):
   `proto native void SetErectSpeedLimit(bool bShouldDisable, int pMaxErectSpeed = DayZPlayerConstants.MOVEMENTIDX_SPRINT);` (`human.c:230`)
   `proto native void SetCrouchSpeedLimit(bool bShouldDisable, int pMaxErectSpeed = DayZPlayerConstants.MOVEMENTIDX_SPRINT);` (`human.c:231`)
   `proto native void SetProneSpeedLimit(bool bShouldDisable, int pMaxErectSpeed = DayZPlayerConstants.MOVEMENTIDX_SPRINT);` (`human.c:232`)
   `DisableErectCameratHorizontalRotation` / `DisableCrouchCameratHorizontalRotation` / `DisableProneCameraHorizontalRotation(bool pDisable)` (`human.c:240-242`, vanilla typo "Camerat" kept)
4. **Vehicle transition state query**:
   `bool IsTransitioning()` on `HumanCommandVehicle` returns `IsGettingIn() || IsGettingOut() || IsSwitchSeat();` (`human.c:735-738`)
5. **Simple death vs ragdoll**:
   `proto native void PhysicsSetSimpleDeath(bool pEnable);` (`human.c:1449`, comment `:1448`: "Old system for handling player death state, was previously handled by PhysicsSetRagdoll")
   `proto native void PhysicsSetRagdoll(bool pEnable);` (`human.c:1451`; existed in 1.29)
   `proto native bool PhysicsIsRagdoll();` (`human.c:1452`), `HumanCommandUnconscious.IsRagdoll()` (`human.c:647`)
6. **Client render optimization**:
   `proto native bool IsFirstRenderFrame();` (`dayzplayer.c:1189`)

## 1.30: surrender (hands up) without an item in hands [EXACT]

1.30 no longer puts `SurrenderDummyItem` in the hands; the class still exists but is `[Obsolete("1.30: no replacement - no longer in use")]` (`P:\scripts\4_world\entities\itembase\surrenderdummyitem.c:1`), and its in-hands profile is still registered (`dayzplayercfgbase.c:1541-1544`) so `player_main_surrender.asi` is loaded. Surrender is a player state:
- `PlayerBase.SetSurrenderState(bool)` (`P:\scripts\4_world\entities\manbase\playerbase.c:2116`)
- `Man.IsSurrendered()` (`P:\scripts\3_game\entities\man.c:67`, new in 1.30)
- `EnableAutoAnimInstUpdateOnHandsChange(false)` + `SetAnimationInstanceByName("dz/anims/workspaces/player/player_main/player_main_surrender.asi", 1)` (`playerbase.c:2087-2088`), reversed with `player_main.asi` at `:2104-2107`.
- The base instance stays loaded through the `"Empty"` profile (`dayzplayercfgbase.c:1546-1549`).

Before 1.30: the surrender emote created a `SurrenderDummyItem` in the hands (1.29: `O:\…\scripts\4_world\classes\emotemanager.c:309`, `CreateInHands("SurrenderDummyItem")`), the state lived in `EmoteManager.m_IsSurrendered`, and the hands-driven instance switch did the rest.

## 1.30: Workbench Animation Editor (Enfusion 2021)

- [CHANGELOG 1.30] Live editing against a running game client.
- [EXACT] The workspace names its event table as a field, `EventTable "…Player_EventTable.ae"` (`player_main.aw:15`); see §Workbench Animation Editor above.
- [CHANGELOG 1.30] A ragdoll editor for `.ragdoll` files; vanilla ships `P:\DZ\characters\bodies\human.ragdoll` [EXACT].
