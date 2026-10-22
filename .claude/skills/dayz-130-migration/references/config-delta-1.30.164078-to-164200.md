# Data delta: DayZ 1.30.164078 → 1.30.164200 (configs, models, other files)

<!-- Generated 2026-10-21 by comparing the two dedicated-server installs entry by entry:
     OLD O:\DayZ Server 1.30\ (DayZServer_x64.exe 1.30.164.78) vs NEW ...\steamapps\common\DayZServer\ (1.30.164.200).
     Every config.bin was derapped with a Python rapify reader whose text output is byte-identical to the
     P:\DZ\...\config.cpp files extracted from 164200 (checked on 6 files), so every line number below is a
     line in P:\ (164200). Script-side changes are in api-delta-1.30.164078-to-164200.md. -->

Machine data: `data/cfgdelta-1.30.164078-to-164200.json`. It uses the same schema as `cfgdelta.json`:
320 added, 267 removed and 205 changed class paths. `file` is relative to `P:\DZ`, and `../scripts/config.cpp`
means `P:\scripts\config.cpp`. `scan_mod_130.py` loads it automatically.

**Citations** are written `name:line` and refer to 164200 files. The names are:

| Name | File |
|---|---|
| `singletrack` | `P:\DZ\vehicles\singletrack\config.cpp` |
| `hpp` | `P:\DZ\sounds\hpp\config.cpp` |
| `tk_sounds` | `P:\DZ\sounds_takistan\config.cpp` |
| `Residential`, `Industrial`, `Military`, `Props` | `P:\DZ\structures_takistan\<name>\config.cpp` |
| `camping` | `P:\DZ\gear\camping\config.cpp` |
| `drinks` | `P:\DZ\gear\drinks\config.cpp` |
| `tk_drinks` | `P:\DZ\gear_takistan\drinks\config.cpp` |
| `containers` | `P:\DZ\gear_takistan\containers\config.cpp` |
| `blunt` | `P:\DZ\weapons\melee\blunt\config.cpp` |
| `tk_magazine` | `P:\DZ\weapons_takistan\attachments\magazine\config.cpp` |
| `shepherd` | `P:\DZ\animals_takistan\canis_familiaris_german_shepherd\config.cpp` |
| `scripts` | `P:\scripts\config.cpp` |
| `hud` | `P:\gui\layouts\day_z_hud.layout` |
| `imageset` | `P:\gui\imagesets\dayz_inventory.imageset` |

"old line" = the 1.30.164078 text derapped in the same format, which matches the pre-update P:\ numbering.

**Limits.**
- Server PBOs ship stub `.paa` files (about 200 B) and stub `.ogg` files, so texture and audio *content* is not compared. Only paths, materials and configs are.
- `worlds_nasdara.ebo` and `worlds_sakhal.ebo` are encrypted, so their entries cannot be compared.
- ODOL `.p3d` files are compared by bytes and by the texture/material paths they embed. Selections, LODs and memory points were **not** parsed.

## 1. PBO level

| What | Count / detail |
|---|---|
| PBO/EBO files | 160 in both installs. None added, none removed. Every file was rebuilt. The `Vers` header `version` is now `127638` for 147 data PBOs. It is `127662` for `dta\graphics/gui/languagecore/scripts/system` and `127639`-`127643` for the four `*_nav`. Three Nasdara structure PBOs are at `127654`-`127666`. `dta\bin` stays at `126541`. Every `.bisign` was re-signed. `keys\dayz.bikey` is unchanged (2025-03-22). |
| Content-identical PBOs (header/signature only) | 39: `ai`, `ai_bliss`, `anims_*` (4), `characters_data`, `data_bliss`, `gear_books`, `modules_dayz`, `rocks_bliss`, `server_data`, `structures_data(_bliss)`, `structures_proxy_parts`, `surfaces_bliss`, `ui`, `vehicles_data`, `water(_bliss)`, `weapon_lights`, `weapon_underslung`, `weapons_animations/data/misc`, `worlds_chernarusplus_ce/_data`, `worlds_enoch_ce/_data`, `dta\bin/graphics/system`, `ai_nasdara`, `rocks_nasdara`, `surfaces_nasdara`, `data_sakhal`, `surfaces_sakhal`, `worlds_sakhal_ce/_data` |
| `config.bin` changed | 15 (see §2): `gear_camping`, `gear_drinks`, `sounds_hpp`, `vehicles_singletrack`, `weapons_melee` (`blunt\`), `dta\scripts`, and in `nasdara\`: `animals_nasdara` (`canis_familiaris_german_shepherd\`), `gear_nasdara` (`containers\`, `drinks\`), `sounds_nasdara`, `structures_industrial/military/props/residential_nasdara`, `weapons_nasdara` (`attachments\magazine\`). No Sakhal, Chernarus, Livonia, characters, weapons-firearms or wheeled-vehicle config changed. |
| `.ogg` | 28 436 changed in 10 sound PBOs. In all but one only the Ogg stream serial and page CRCs differ (bytes 14-17 / 22-25 of each page), so the audio is identical. Real changes: `sounds_vehicles\motorbike_02\motorbike_02_Horn_loop.ogg` was replaced. New files: `motorbike_01\MotorBike_01_Engine_Ext_Broken.ogg`, `motorbike_02\MotorBike_02_Engine_Ext_Broken_High/_Low.ogg`, and in `sounds_nasdara`, `ai\animals\dog_Shepherd\dog_Shepherd_Pain_01…06.ogg`. |
| `.p3d` | 3 377 re-binarized in 96 PBOs, still ODOL v56. 1 960 keep the same size. 69 models changed which textures/materials they embed (§2). 5 are new: `structures_industrial_nasdara\Irrigation\Proxies\IrrigationTunnel_ProxyRoadway_Block_01.p3d` and `structures_residential_nasdara\Houses\Proxy\RoadwayProxy\proxy_3S1_Dam3_floor{2,3}_dynamic_{ext,int}.p3d`. |
| `.rvmat` | 25 changed, 3 new (`acog_tan*.rvmat`). 7 570 terrain-layer materials were removed from `worlds_nasdara_data` (§2.9). |
| `.paa` (server stubs) | 10 new in `weapon_attachments_textures`: `acog_as/damage/destruct.paa` and the `acog_tan_*` set. 3 new in `gear_cooking`: `cooking_pot_as/damage/destruct.paa`. 14 stubs changed: ACOG `_co/_nohq/_smdi`, cooking pot `_co/_nohq/_smdi`, Nasdara floor `_NOHQ` ×4, gantry `_ca` ×2 and `house_1m5_ads/_mc`. |
| Other | `dta\gui`: `day_z_hud.layout`, `dayz_inventory.imageset`, `dayz_inventory_additional.edds`. `dta\languagecore\stringtable.csv`. `animals_nasdara`: 2 dog `.asi` and 4 dog hit `.anm`. `worlds_nasdara_ce`: `db\types.xml`, `mapgrouppos.xml`. `ChernarusPlus.wrp`, `enoch.wrp`. 4× `navmesh.nm`. `dta\scripts`: 14 new `.c`, 67 changed `.c` (script delta). |
| Non-PBO | `DayZServer_x64.exe` 24 849 464 → 24 918 584 B. |

## 2. Changes that matter to mod authors

### 2.1 Vehicles / motorbikes
- **`Motorbike_02_ColorBase.hiddenSelections[]` renamed slots 5-7** (`singletrack:1419`). The list went from
  `light_1_1, light_1_2, light_brake_1_2` to `lights_front, lights_rear, lights_brake`, and the count stays 7.
  `hiddenSelectionsMaterials[]` (`singletrack:1429`) and `hiddenSelectionsTextures[]` (`singletrack:1439`) keep 7 entries.
  - A retexture that copies all 7 still works.
  - A mod that addresses the light slots by selection name, or swaps light materials with `SetObjectMaterial(4..6, …)`, must re-check them.
  - `Motorbike_01_ColorBase.hiddenSelections[]` (`singletrack:232`) is unchanged.
- **New per-bike light materials** (keys that cars already had):
  - `Motorbike_01_ColorBase` `frontReflectorMatOn/Off`, `brakeReflectorMatOn/Off` (`singletrack:256-259`).
  - `Motorbike_02_ColorBase` the same four keys (`singletrack:1449-1452`).
  - `MotorbikeScript` already read these keys in 164078 (`FrontOn/Off`, `BrakeOn/Off` selections), but the vanilla bike configs had no values. In 164200 the vanilla bike scripts also register `FrontOn/FrontOff` (script delta).
  - A custom bike needs the four keys, or its front/brake lamp material never swaps.
- `Motorbike_02_ShieldFront_ColorBase` (`singletrack:1087`) no longer declares `itemBehaviour=0;` (old line 1106), so it now inherits `itemBehaviour` from its parent.
- Light rvmats `motorbike_0{1,2}_lights_{front_e,rear_e,rear_brake}.rvmat`: the procedural NOHQ is replaced by `motorbike_0x_lights_nohq.paa`. Motorbike_01's emissive map moved from `motorbike_01_lights_e_co.paa` to `motorbike_01_lights_e.paa`.
  - `Motorbike_01.p3d` now embeds `motorbike_01_lights_nohq.paa`.
  - `Motorbike_02.p3d` now embeds separate `motorbike_02_fenderfront*` / `fenderrear*` materials.
- Sounds:
  - `Motorbike_02_Engine_Ext_Broken_SoundSet`/`_SoundShader` were **removed** (old `hpp` lines 384602 / 317553). They are replaced by `Motorbike_02_Engine_Ext_Broken_High/Low_SoundSet` (`hpp:386435`, `hpp:386443`) and their `…_SoundShader` (`hpp:318210`, `hpp:318223`), and `Motorbike_02_ColorBase/Sounds.soundSetsFilter[]` (`hpp:519751`) was updated to match. A mod that inherits from or lists the removed set must switch to the new names.
  - `Motorbike_01_Engine_Ext_Broken_SoundShader`: `samples[]` (`hpp:317341`) now points at `\DZ\sounds\vehicles\Motorbike_01\Motorbike_01_Engine_Ext_Broken`, `frequency` 0.95→1 (`hpp:317349`), and the `volume` curve was rewritten (`hpp:317350`).
  - Horns: `Motorbike_0{1,2}_Horn_SoundSet.volumefactor` 0.6→0.5 (`hpp:386288`, `hpp:386851`); `…_Horn_Short_SoundSet` 0.8→0.4 (`hpp:386297`, `hpp:386860`); `Motorbike_0{1,2}_Horn_Short_SoundShader.volume` 1.4→0.9 (`hpp:317961`, `hpp:318877`).
- `vehicles_wheeled`: config unchanged. 90 `.p3d` were re-binarized, and none of them changed an embedded texture or material path.

### 2.2 Base building and Takistan rebuildable houses
- **`toggle_utility_part_update=1;` is new, `utility_build[]` is gone.**
  - 46 rebuild parts now set `toggle_utility_part_update=1;`: 44 in `Residential` and 2 in `Industrial` (`Industrial:411`, `Industrial:461`, the `Land_IrrigationTunnel_Entrance_01` ladder). Example: `Land_House_1M2_Dam2/Construction/rebuild_roof1/roof1` → `Residential:9059`.
  - Houses: `Land_House_1M2_Dam2` (5), `1M3_Dam2` (4), `1M4_Dam2` (3), `1M5` (4), `1S2_Dam2` (5), `2M1_Dam2` (4), `2M2_Dam2` (3), `2S1_Dam3` (3), `3S1_Dam3` (9), `3S2_Dam2` (4).
  - 43 `utility_build[]={"floor_switcher_server_N"}` arrays were deleted from those parts.
  - The script no longer reads `utility_build[]` (`ConstructionPartTypeData.InitUtilityBuildData` reads `toggle_utility_part_update`). The two `utility_build[]={"ladder_utility"}` arrays left at `Industrial:402` and `Industrial:462` are dead data.
  - A mod rebuildable building must replace `utility_build[]` with `toggle_utility_part_update=1;` on every part whose (de)construction should re-evaluate the utility parts.
- **`Land_House_3S1_Dam3` floor logic re-modelled** (`Residential:91187`):
  - Floor switching no longer uses `required_parts[]`, `override_animsource_*` or `proxyroadway_*` on the floor switchers. Roof and wall parts now carry `StaticsSupportData/SupportProviders` (`floorSwitcher{1..3}Category{1..3}`, e.g. roof1 `Residential:100811`).
  - `floor_switcher_server_1..3` (`Residential:103048`, `Residential:103087`, `Residential:103184`) toggle `physics_geom_names[]` (`Residential:103053`) and use `RequiredPartsVariants`.
  - New utility parts `util_dyn_roadway_floor{2,3}_{ext,int}`: the `_ext` ones use `custom_part_type="ConstructionPartRebuildInvertedSupport"` (`Residential:103151`, `Residential:103238`, the new script class). Their AnimationSources start at `Residential:92857`, and they use 4 new roadway proxies.
- **`Land_IrrigationTunnel_Entrance_01`**:
  - New `class Navmesh { generateWithRoadway=1; generateWithProxies=1; class HideParts { class proxyblock { hideAnimationSource="proxyroadway_block_01"; } } }` at `Industrial:25`.
  - New AnimationSource `proxyroadway_block_01: AnimSourceShown` (`Industrial:63`).
  - `ladder_utility` gains `proxyroadway_names[]={"proxyroadway_block_01"}` with `proxyroadway_behaviors_invert[]={1}` (`Industrial:511`).
- **`Fence` gate navmesh** (`camping:4418`): new `class Navmesh { class TransformParts { class Gate { transformAnimationSource="Wall_Gate_Rotate"; } } }`.
  - Every class that inherits `Fence` inherits it, so the navmesh follows the gate leaf.
  - Mod fences/gates that inherit `Fence` with a model lacking the `Wall_Gate_Rotate` source are untested.
  - This block shifts every later line of the camping config by +10.
- Other house fixes in `Residential`:
  - `physics_geom_names[]` corrections: `Land_House_1M3_Dam2` `phx_wall1_part1`→`phx_door1` (`Residential:18798`, `Residential:18917`, `Residential:18957`; AnimationSource `phx_wall1_part1` removed, old line 14451); `Land_House_2S1` `door2_barricade` `phx_door1`→`phx_door2` (`Residential:77446`); `Land_House_2M2` `window8_wood` right→left (`Residential:51632`).
  - New `physics_geom_names[]` on the `*_window_brick/_wood` parts of `1M4_Dam2` (`Residential:30481`, `Residential:30674`) and `3S1_Dam3` (6 parts).
  - `Land_House_1S1` (`Residential:59919`-`59952`): 12 malformed `phx_*` AnimationSources (`type="hide"`, `source`, `selection`…) now simply inherit `AnimSourceHidden`.
  - `Land_House_1S2_Dam2`: `proxyroadway_floor1_ext` and `_int` swapped bases. `_ext` is now `AnimSourceShown` (`Residential:66704`) and `_int` is now `AnimSourceHidden` (`Residential:66707`).
  - `Land_House_2M2_Dam2` rubble: `build_text_override` "Clear Room 1 Rubble 1[WIP]" → `$STR_ClearRubble` (`Residential:57159`, `Residential:57243`).
- `Land_WaterWell_Brick` (`Props:458`, `Props:524`): `rebuild_well/well_brick` and `rebuild_roof/roof` gained `EffectsData` (`soundEffectPositions[]`, `particleEffectPositions[]` = `"well_audio"`).
- `CfgSlots/Slot_Att_CodeLock*` (21 slots, `scripts:2768`-`2914`): `displayName` changed from `#STR_CodeLock0` to `#STR_CfgVehicles_codelock0` (e.g. `scripts:2771`). The slot names and selections are unchanged.

### 2.3 Weapons / ammo
- `PoliceBaton/MeleeModes/Heavy` and `/Sprint`: `ammo` `MeleeBluntLight_2`→`MeleeBluntHeavy_2` (`blunt:1255`, `blunt:1260`). The 1.30.164078 config had given the heavy attack the light ammo.
- `CfgMagazines/Mag_LeeEnfield_10Rnd.ammoItems[]` (`tk_magazine:327`) now lists `"Ammo_762x54","Ammo_762x54Tracer"`, so tracers load.
- ACOG (`weapon_attachments_textures`):
  - `acog.rvmat`/`_damage`/`_destruct` were rewritten. Ambient and diffuse went up (`acog.rvmat`: 1→3), AS moved to `acog_as.paa`, and the macro `aside/up` changed from 4 to 1.
  - New `acog_tan*` texture/material set. No vanilla config references it yet; `grep acog_tan P:\DZ\**\config.cpp` finds nothing.
  - `optic_acog.p3d` now embeds `acog_as.paa` and `env_land_optic_co.paa`.
  - A retexture that overrides `hiddenSelectionsMaterials` with a copy of the old `acog.rvmat` keeps the old (flatter) look.
- 5.45×39: `545x39*.rvmat` were re-lit and now use `545x39_smdi.paa` instead of `_nohq` in the SMDI stage. `545x39_LooseRounds.p3d`, `545x39_SingleRound.p3d` and `ssg82.p3d` embed `545x39_smdi.paa`.
- `stringtable.csv`: a duplicate lowercase key `str_cfgvehicles_buisoptic1` was removed. `STR_cfgVehicles_BUISOptic1` is still there, so this has no effect.

### 2.4 Items
- Drinks: `Nutrition` had the bogus key `totalVolume=1`, which was replaced by `fullnessIndex=1`. The key `totalVolume` no longer exists anywhere in `P:\DZ`. Affected classes:
  - `SodaCan_ColorBase` (`drinks:863`).
  - `Ayran`, `Lassi_ColorBase`, `SmallGlassBottle_Afza`, `SmallGlassBottle_Jallab` (`tk_drinks:277`, `tk_drinks:415`, `tk_drinks:792`, `tk_drinks:817`).
- `GunCase_*`:
  - `hiddenSelectionsMaterials[]` paths lost the leading `\` (`containers:166`, `containers:248`, `containers:312`).
  - `healthLevels[]` now leaves the 0.7/0.3 stages empty (`{}`) (`containers:183`).
  - `GunCase_Orange/White` now declare `DamageSystem: DamageSystem` with `RefTexsMats[]={"…guncase_black.rvmat"}` (`containers:258`, `containers:322`) and no longer redeclare `hitpoints=750`. This is the vanilla `RefTexsMats` damage-swap pattern.
  - A retexture with its own rvmats needs its own `RefTexsMats[]` and `healthLevels[]`.
  - `guncase_black_damage/destruct.rvmat`: diffuse and specular went from 1 to 1.5.
- Cooking pot (`gear_cooking`): new `cooking_pot_as/damage/destruct.paa`. `cooking_pot*.rvmat` were re-lit and now use their own damage/destruct maps instead of `dz\characters\data\generic_*_mc.paa`. `CookingPot.p3d` embeds `cooking_pot.rvmat` and its maps instead of `default.rvmat`.
- `loot_paper*.rvmat` (`gear_consumables`) were re-lit, with ambient 1→0.35 and fresnel lowered.
- Clothing, gear, food, tools and weapons `.p3d` were re-binarized in bulk (`characters_*` 466 files, `gear_*` 178). No embedded texture/material path changed, apart from the ones listed here, and no `config.bin` in those PBOs changed apart from drinks and camping.

### 2.5 Sounds
- `sounds_takistan` (CfgPatches `DZ_Sounds_Nasdara`) is a near-complete copy of the sound config. It loads after `DZ_Sounds_Effects`, so both files carry the same changes; the JSON names `sounds/hpp` as `file`.
- 180 sound classes were added, all Nasdara/Sakhal surface footsteps, bodyfalls, hand-steps and wood-plank variants. The counts are 161 SoundSets (+2 motorbike), 17 SoundShaders (+2 motorbike) and 5 `drop_item_LookupTable` entries (`nasdara_soil/forest_pine/stony`, `building_dirt_ext/int`, `hpp:471074`-`471106`).
- Plus `dog_Shepherd_Pain_SoundSet`/`_SoundShader`, only in `tk_sounds` (`tk_sounds:327713`, `tk_sounds:14491`).
- 43 SoundSets changed only their `soundShaders[]` (`hpp:358272`-`375644`, `tk_sounds:367743`-`385195`):
  - `Handstep(_Hard)_nasdara_grass_dry_Char` and `Handsstep_nasdara_grass_dry_Char` now use the generic `Handstep*_grass_dry_ext_Char_SoundShader`.
  - 40 `runProne_*_Zmb_Soundset` use the `*_Char_SoundShader` walk/hand shaders instead of the `*_ZMB_` ones.
- `CfgVehicles/Animal_CanisFamiliaris_GermanShepherd_ColorBase/AnimEvents/Sounds/Pain` is new (`shepherd:489`). The dog `.asi` graphs and 4 hit `.anm` were also replaced.

### 2.6 Effects / particles
- No particle config (`dz.pbo` `data\ParticleEffects`) changed. The only effect-related config change is `Land_WaterWell_Brick` `EffectsData` (§2.2).
- The VFX value change (wheel smoke 5→2) is script-side.

### 2.7 GUI
- `dayz_inventory.imageset`: image `handsactive` was **renamed back to `woodcrate`** (same `Pos 1707 1365`, `Size 170 170`, `imageset:683`). The texture path lost its `{21F550A0F5780D77}` GUID prefix.
  - Vanilla `CfgSlots/Slot_Truck_01_WoodenCrate1-4.ghostIcon` already pointed at `image:woodcrate` in 164078 (`scripts:1603`-`1621`), so 164078 had a missing ghost icon.
  - A mod that uses `set:dayz_inventory image:handsactive` now gets no image.
  - `dayz_inventory_additional.edds` was re-encoded.
- `day_z_hud.layout` was converted to GUID widget form (`ImageWidgetClass "{…}" { Name "…"`).
  - Every widget name is kept. Colours were rounded to 3 decimals.
  - The only behavioural change: the 12 `*_mtl_down` / `*_mtl_down_not_select` arrow images went from `mode additive` to `mode blend` (e.g. `ia_interact_mtl_down`, `hud:4353`). The new `ItemActionsWidget` dims these arrows with `SetAlpha`.
  - Mods that ship a copy of `day_z_hud.layout` should re-copy it.

### 2.8 Takistan / Nasdara structures (non-house)
- `Land_Mil_ContainerHabitat_Armory_Locked_Green_DE` (`Military:1845`) **changed parent**, from `Land_Mil_ContainerHabitat_Armory` to `HouseNoDestruct`.
  - It now declares its own `Doors/DoorsTwin1` (`Military:1860`) and **no longer inherits the armory's `DamageSystem`** (hitpoints 1000, zero armor).
  - Mods that modded `Land_Mil_ContainerHabitat_Armory` to affect the locked variant must target it directly.
- `Land_PoliceStation_Large_Takistan/DamageSystem/DamageZones` were rebuilt to match its real doors.
  - Removed zones: `twin4`, `twin5`, `doors24`, `doors26`-`doors39` (old lines 146773, 147688…). They had no door.
  - Added zones: `doors7`-`doors10` (`Residential:146841`…).
  - `doors25` armor `damage` changed: Projectile 2→3 (`Residential:147896`), Melee 2.5→5 (`Residential:147911`), FragGrenade 3→10 (`Residential:147926`).
  - Raid/breach mods that name these zones must re-check them.
- Door sound typo fixed on `Land_PoliceStation_Large_Takistan` (22 doors) and `Land_Hospital_Takistan` (8 doors): `"door WoodSmallOpen"` became `"doorWoodSmallOpen"`, and likewise for Close/Rattle/OpenABit and the Twin/Metal variants (e.g. `Residential:145096`, `Residential:146398`). The old value with a space names no sound class. The target class exists, e.g. `class doorWoodSmallOpen` at `hpp:521578`.
- Models: 38 irrigation-tunnel and water-stream `.p3d` swap the roadway surface texture `dz\surfaces\data\roadway\still_water_int.paa` for `clean_water_int.paa` (`IrrigationTunnel_36m_01` uses `still_water_ext.paa`).
  - Highway bridges dropped `concrete_int`/`metal_thick_int` roadway.
  - `PoliceStation_Large_Takistan.p3d` and `TajbegPalace_*.p3d` gained penetration materials (glass, rubber, weaponlift, armor_5mm_plate, ironplate, fabric_thin, metalplate).
  - Underground `ammoboxes_single*.p3d` (Livonia) fell back to `default.rvmat`.
  - Chernarus `Shed_M1/M3/W4` and Livonia `Slum_House2` dropped `dirt_int` roadway.
  - Effect: changed surface sounds and bullet penetration on those objects. Retexture mods are not affected.
- `structures_takistan\Residential\Houses\data` rvmats: `destruct_half_house_*` and `house_3s1_02_multi` now reference `dz\structures_takistan\data\…` instead of `dz\structures\test\data\…`. `house_2s1_rooms_multi` swaps the macro `dir`/`pos`.

### 2.9 Worlds / navmesh / maps
- **Navmesh format v7 → v9 on all four maps.** The header is `TESM` with the version at offset 4.
  - `worlds_chernarusplus_nav` 569.6→640.3 MB, `worlds_enoch_nav` 402.2→429.6 MB, `worlds_nasdara_nav` 782.6→870.0 MB, `worlds_sakhal_nav` 535.8→558.1 MB.
  - `P:\DZ\worlds\*\navmesh\navmesh.nm` are v9 too.
  - The 164200 server checks the version **strictly**. In `DayZServer_x64.exe` the code right before `"Navmesh could not be loaded: Invalid version. (Navmesh version: %d, Desired version: %d)"` is `cmp r9d,9` (164078: `cmp r9d,7`).
  - **Every custom-map navmesh built for 164078 (v7) is rejected on 164200**, just as v6 was on 164078. The log line is `Navmesh version: 7, Desired version: 9`, and infected/AI stand still.
  - Regenerate the navmesh with the 164200 DayZDiag/NavMeshGenerator.
- `ChernarusPlus.wrp` (+1 B) and `enoch.wrp` (−5 B) were re-binarized. Their model tables are identical (2 352 / 2 468 `.p3d` paths); the remaining differences are in compressed data and were not decoded.
- `worlds_nasdara.ebo` 196 241 884 → 196 337 971 B and `worlds_sakhal.ebo` −27 B. Both are encrypted and could not be compared.
- `worlds_nasdara_data.pbo` 35.2 → 18.6 MB: 7 570 of 15 500 `layers\P_xxx-yyy_*.rvmat` terrain-layer materials were deleted. The `.paa` layer textures (5 547) and `config.bin` are unchanged.
- Nasdara CE inside `worlds_nasdara_ce.pbo`:
  - `db\types.xml`: `Ayran` nominal/min 0/0 → 50/45; `HuntingBag_Hannah` `count_in_map` 0→1. The shipped `mpmissions\dayzOffline.nasdara\db\types.xml` already has Ayran 50/45. **Merge these into your server mission by hand.**
  - `mapgrouppos.xml`: 13 719 → 13 717 groups. Net changes: `Land_Misc_Scaffolding` −2; `Land_Wreck_Pickup01_Aban_Red`, `_sed01_aban1_white`, `_sed02_aban2_yellow`, `_hb02_aban1/2_red` −1 each; `Land_Wreck_UAZ_TK`, `Land_Mil_Tent_Big2_4_Desert`, `Land_House_1M1`, `Land_Shed_M1`, `Land_Shed_W6` +1 each. About 100 groups were reordered or moved slightly.
- Sakhal: no config changed. `structures_sakhal` (160), `water_sakhal` (37), `plants_sakhal` (22) and `rocks_sakhal` (1) `.p3d` were re-binarized with no embedded path change. The navmesh is v9 (above).
- `NavmeshTest` → `Land_NavmeshTest*` is script-only (`4_world\entities\navmeshtest\navmeshtest.c`). There is no `CfgVehicles` entry and no model for it in the shipped data. `Land_House_2M1_Dam2_NAVMESHTEST` (`Residential:45602`) is in both builds, unchanged.

## 3. Checklist for a mod built on 1.30.164078
1. Custom terrain: rebuild `navmesh.nm` as v9. Check the RPT for `Desired version: 9`.
2. Rebuildable buildings: replace `utility_build[]` with `toggle_utility_part_update=1;`, and port the script overrides (see api-delta, `TryQueueUtilityPartsUpdate`).
3. Motorbike mods:
   - Selection names for `Motorbike_02` light slots 5-7.
   - Add `frontReflectorMatOn/Off` and `brakeReflectorMatOn/Off`.
   - Replace the removed `Motorbike_02_Engine_Ext_Broken_SoundSet`.
4. Overrides of `Land_Mil_ContainerHabitat_Armory` or of PoliceStation Takistan damage zones.
5. Copies of `day_z_hud.layout` or `dayz_inventory.imageset` (`handsactive` → `woodcrate`).
6. Retextures of ACOG, GunCase, cooking pot and 5.45 ammo: vanilla materials changed, so copied rvmats now look different from vanilla.
7. Server missions on Nasdara: merge the `types.xml` (Ayran, HuntingBag_Hannah) and `mapgrouppos.xml` changes.
