# DayZ 1.30 — humanoid character delta

Companion to `dayz-characters` SKILL.md. Mesh/rig/LOD rules in the parent skill still hold. This file is
the 1.30 overlay: new classes, graph format, PPE, skinning, surrender, heads. First written against 1.30
Experimental (164014); **re-verified on 1.30.164078 stable** against `P:\` (`P:\DZ\…\config.cpp` is
derapified, so its line numbers differ from the Exp draft). `Before 1.30:` lines cite the 1.29 extraction
`O:\DayZ 1.29 Старый распакованный клиент\` (shortened `O:\…`). Labels: `../../_shared/dayz-version-policy.md`.

## Infected class additions

Hierarchy `DZ_LightAI → DayZInfected → ZombieBase → ZombieMaleBase / ZombieFemaleBase → variants` is still
the inherit path.

1.30 adds female military CfgVehicles (`P:\DZ\characters\zombies\config.cpp`; none of them in 1.29):

- `ZmbF_SoldierNormal_Base: ZombieFemaleBase` — `:11119` (`model="\DZ\characters\zombies\ZmbF_SoldierNormal.p3d"`, `aiAgentTemplate="InfectedMSoldier"`, `:11122-11123`)
- `ZmbF_SoldierNormal: ZmbF_SoldierNormal_Base` — `:11346` (`scope = 2`)
- `ZmbF_ArmyOfficer: ZmbF_SoldierNormal_Base` — `:11354` (`ArmyOfficer_fat_f.p3d`, `aiAgentTemplate="InfectedSoldier_Officer"`, `:11357-11358`)
- `ZmbF_eastSoldier_Base: ZmbF_usSoldier_normal_Base` — `:11577` (config parent is the US female soldier base `:10801`, not `ZombieFemaleBase` directly)
- `ZmbF_eastSoldier_Normal_Navy: ZmbF_eastSoldier_Base` — `:11799`; `ZmbF_eastSoldier_Heavy_Navy: ZmbF_eastSoldier_Normal_Navy` — `:11807`

The models are packed (`P:\DZ\characters\zombies\zmbf_soldiernormal.p3d`, `armyofficer_fat_f.p3d`,
`eastsoldier_normal_navy_f.p3d`).

Script bases (`P:\scripts\4_world\entities\creatures\infected\`):

- `ZmbF_SoldierNormal_Base` / `ZmbF_eastSoldier_Base` extend `ZombieFemaleBase` and return true from
  `IsZombieMilitary()` (`zombiefemalebase.c:139-152`). Note the script parent of `ZmbF_eastSoldier_Base` is
  `ZombieFemaleBase` even though its config parent is `ZmbF_usSoldier_normal_Base`.
- Nasdara male: `ZmbM_takiCitizenUrbanA_Base` … `ZmbM_takiImam_Base`, `ZmbM_takiSoldier_Normal_Base`
  (`zombiemalebase.c:160-250`).
- Nasdara female: `ZmbF_takiCitizenUrbanA_Base` … `ZmbF_takiOilRigWorker_Base` (`zombiefemalebase.c:163-194`)
  plus `Zmbf_IonSoldier_Normal_1` (`:155`).
- **None of the `taki*` / `IonSoldier` classes has a config class in `P:\DZ`** on 1.30.164078 (only
  sound sets reference `ZmbM_IonSoldier_*`). `[UNVERIFIED]` whether they ship in a DLC/map PBO that is not
  unpacked here. Do not inherit them in a mod config.

[DESIGN] Inherit `ZombieFemaleBase` (or one of the 1.30 soldier bases with a config) for a custom female
military infected; do not invent a parallel AI driver.

## Infected animation graph: `.agr` index + `.agf` subgraphs

1.30: `enfanimsys.graphname` still points at `dz\anims\workspaces\infected\infected_main\infected.agr`
(`P:\DZ\characters\zombies\config.cpp:71-77`). The `.agr` is now Enfusion `AnimSrcGraph` text and its
subgraphs are `.agf` files with root `AnimSrcGraphFile`
(`P:\DZ\anims\workspaces\infected\infected_main\locomotion.agf:1`).

```
// [EXACT] P:\DZ\anims\workspaces\infected\infected_main\infected.agr:430
 GraphFilesResourceNames {
  "{055B14354B447015}DZ/anims/workspaces/infected/infected_main/Locomotion.agf" "{809A284592397198}DZ/anims/workspaces/infected/infected_main/Combat.agf" "{A3A83C5CD5FE35F3}DZ/anims/workspaces/infected/infected_main/Interaction.agf"
 }
```

Before 1.30 the graph was **not** monolithic (correction to the Exp draft): 1.29 `infected.agr` was in the
old `$AnimGraph 7` format and already referenced `Locomotion.agr` / `Combat.agr` / `Interaction.agr`
(1.29: `O:\…\DZ\anims\workspaces\infected\infected_main\infected.agr:155-157`). What changed is the
format and the subgraph extension. The player graph changed the same way: 1.30
`P:\DZ\anims\workspaces\player\player_main\` holds `player_main.agr` + `actions.agf`, `combat.agf`,
`gestures.agf`, `locomotion.agf`, `master.agf`, `tests.agf`, `vehicles.agf`, `weapons.agf` (1.29: the same
names as `.agr`).

**Migration:** a 1.29 custom infected (or player) workspace cannot be dropped in as-is.
`[CHANGELOG 1.30]` official process (Bohemia tweet): open the old graph in Workbench 1.30 and re-save the
workspace. Bohemia notes the converter was not tested.

**THE WALL** (one loaded anim-graph mod) still applies to *new* graphs. Inheriting vanilla `enfanimsys`
still avoids it.

## `MINDSTATE_COWER`

`DayZInfectedConstants` appends `MINDSTATE_COWER` after `MINDSTATE_FIGHT`
(`P:\scripts\3_game\entities\dayzinfected.c:12-18`; earlier values unchanged). `ZombieBase` holds the calm
idle pose when cowering (`P:\scripts\4_world\entities\creatures\infected\zombiebase.c:474-477`, comment:
"sandstorm cowering, movement is already stopped by the AI"). The behaviour itself is native AI config:
`CfgAIBehaviours` gained `SlotCowering` / `BehaviourCowering` (`P:\DZ\AI\config.cpp:30-32`, …). [DESIGN]
Custom infected script that switches on mind state must handle this value or it falls through the switch.

## New infected attack anims (binaries)

1.30 ships `P:\DZ\anims\anm\infected\attacks\erected\z_erc_run_attackl_heavy.anm` (25534 bytes) and
`z_erc_run_attackr_heavy.anm` (25264 bytes); 1.29 had only the standing `z_erc_attackl/r_heavy.anm`.

[DESIGN] A custom infected that replaces the combat `.agf` should expect these heavy run-attack sources in
vanilla 1.30.

## Human skinning: `Lard` removed

1.30: `Skinning` on `SurvivorBase` is Steaks (`HumanSteakMeat`) / Guts / Bones / BloodInfectionSettings
only — `P:\DZ\characters\data\config.cpp:167-190`. No `Lard` class. (Before 1.30: `class Lard` inside
that block — 1.29: `O:\…\DZ\characters\data\config.cpp:180`.)

## Surrender (no dummy item)

1.30: `Man.IsSurrendered()` (`P:\scripts\3_game\entities\man.c:67`, overridden at
`P:\scripts\4_world\entities\manbase\playerbase.c:8962`); `PlayerBase.SetSurrenderState(bool)`
(`playerbase.c:2116`); `EmoteManager.ForceSurrenderState` is
`[Obsolete("1.30: Use PlayerBase::SetSurrenderState instead")]` (`P:\scripts\4_world\classes\emotemanager.c:1255-1256`);
`SurrenderDummyItem` is `[Obsolete("1.30: no replacement - no longer in use")]`
(`P:\scripts\4_world\entities\itembase\surrenderdummyitem.c:1-2`) and lost its `m_player` field.

Do not detect surrender by an item-in-hands classname.

## Sandstorm PPE and head-gear sun isolation

These sit on worn clothing, but they fire on the humanoid:

- `DEF_DUST_PARTICLE_BREATH = 3` and `DEF_DUST_PARTICLE_EYES = 4` (`P:\scripts\3_game\constants.c:549-550`)
- Airborne solid agents: `eAgents.SILICOSIS = 512`, `eAgents.EYES_IRRITATION = 1024` (`P:\scripts\3_game\enums\eagents.c:14-15`)
- `PluginTransmissionAgents` `AGT_AIRBOURNE_SOLID` **sums** `GetProtectionLevel(DEF_DUST_PARTICLE_BREATH)`
  over cached-equipment attachments in category `MASK` for silicosis, and `DEF_DUST_PARTICLE_EYES` over
  `EYEWEAR|MASK|NVG` for eye irritation (`P:\scripts\4_world\plugins\pluginbase\plugintransmissionagents.c:414-433`,
  helper `GetProtectionLevelCachedEquipment` `:577-594`). Coughing / squinting use the first item with
  protection > 0 (see `dayz-clothing` reference § Sandstorm protection).
- `Environment.HeadGearProtectionAgainstSun` reads the heat isolation of the headgear attachment
  (`P:\scripts\4_world\classes\environment\environment.c:2037-2065`).
- Heat path: `HeatComfortMdfr` feeds `ThermalBiasHandler` (`P:\scripts\4_world\classes\thermalbiashandler.c`);
  `HeatStrokeMdfr` has four stage thresholds `{0.0, 0.4, 0.6, 0.85}`
  (`P:\scripts\4_world\classes\playermodifiers\modifiers\diseases\heatstroke.c:1-4`). Dust diseases:
  `silicosis.c`, `irritatedeyes.c` in the same folder.

[DESIGN] A custom mask/helmet for a humanoid must report `MASK`/`EYEWEAR` and set those protection types or
sandstorm agents ignore it. Full clothing slot rules live in `dayz-clothing`.

## Heads (`CfgHeads` / `CfgFaces`)

1.30 moved the `CfgHeads Default Grimaces` animation paths from `dz\anims\data\anim\fce\*.rtm` to
`dz\characters\heads\anim\*.rtm` (`AwareFace` … `SafeFace`, `NeutralFace`, `RTM_Face`, and the `Lipsync`
`vizem[]` list) — e.g. `anim="dz\characters\heads\anim\aware.rtm"` at
`P:\DZ\characters\heads\config.cpp:4705` (config-delta). `CfgFaces MaleAdamHeadDecayed*` textures were
renamed to `hhl_Adam_body_shaved_decayed*.paa`. A custom head that inherits `CfgHeads Default` picks up the
new paths; one that copied the old grimace paths verbatim should be re-pointed. `[UNVERIFIED]` whether the
old `fce` paths still resolve on 1.30 (the `.rtm` files are not unpacked on `P:\`).

## Bone indices (rigging)

See `references/character-rigging.md` § DayZ 1.30 changes. Short: `[CHANGELOG 1.30]` 250-bone cap gone; XML
`index` unused except `EntityPosition`; names must match the `.xob`.
