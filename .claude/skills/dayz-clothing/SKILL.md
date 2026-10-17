---
name: dayz-clothing
description: "Use when: clothing mod, worn model, ClothingTypes, chaleco, armadura, la ropa flota/rígida/girada, slot Vest/Body/Armband, NBCHead/NBCTop/NBCPants/NBCBoots/NBCGloves, NBCBag, Waterskin, gas mask filter, headSelectionsToHide, wet clothing, varWetMax, heatIsolation, sandstorm goggles/mask protection. Worn clothing (_m/_f), DayZ 1.30. Not humanoids/rigging: dayz-characters; not proxy triangles: dayz-proxy-align."
---

# DayZ Worn Clothing — the verified pipeline

Distilled from ArmorHneck (2026-08-03): third-party body armor RAR → fully working
worn item, ~6 in-game cycles to isolate three independent SILENT failure modes.

> Target: DayZ 1.30 stable (1.30.164078). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

The in-game results below were measured on DayZ 1.29 diag unless labeled otherwise
(measured on 1.29). They rest on the worn-mesh contract (text config, worn frame,
`DayzTemporarySkeleton`, 159-bone model.cfg), which no 1.30 API or config delta touches; on
1.30.164078 the vanilla worn ODOLs still carry `DayzTemporarySkeleton`
(`P:\DZ\characters\tops\chainmail_m.p3d`, `hoodie_m.p3d`, string check). What 1.30 changes is the
config/script contract — NBC slots, filter-gated masks, `ItemBaseType`, the `varWetMax` default,
heat isolation, sandstorm protection categories, shoe `itemSize`, Waterskin, wash/wet actions:
§ DayZ 1.30 changes below and [dayz-1-30-clothing.md](references/dayz-1-30-clothing.md).

## THE THREE SILENT KILLERS (check these FIRST, in this order)

A worn item can fail three independent ways, each with ZERO RPT errors. Any one
of them wastes cycles if you chase the others first.

1. **Config must be TEXT in the PBO.** A `config.bin` produced by CfgConvert in
   this environment does NOT register (class absent: `CreateObjectEx` →
   `unknown_type`, invisible to VPP). Pack `config.cpp` as text (packonly);
   every working local mod ships text. Gate: live spawn of the classname —
   never trust static PBO inspection for "it registered".
2. **Worn mesh must be in the canonical clothing frame** (see next section).
   A mesh built facing +Z renders as EXPLODED rigid pieces floating ~1 m above
   the player (each 0/1-weighted plate is transformed by its bone from a
   flipped frame). Fix: rotate 180° about +Y (det=+1 — do NOT touch winding)
   AND swap left*/right* selection pairs in every LOD. That fix keeps a mirror
   (CANONICAL WORN FRAME → handedness); with the Blender source, re-export it
   with `(x, z, y)` (Rule 12) instead.
3. **The clothing skeleton must be named `DayzTemporarySkeleton`.** ALL vanilla
   worn clothing compiles with that name (verified via ODOL
   `model_info.skeleton.name` on armbend_dynamic_m + chainmail_m): same 159
   bones and hierarchy as OFP2_ManSkeleton, ONLY the name differs — it is the
   placeholder the engine swaps for the wearer's skeleton at bind time. With
   `skeletonName="OFP2_ManSkeleton"` the engine treats the p3d as having its
   OWN skeleton and never re-binds: the item renders as a rigid block anchored
   above the player, not animating. The community 114-bone OFP2 template fails
   clothing TWICE (bone count and name). Template: `references/model.cfg.template`.

## CANONICAL WORN FRAME (measured on chainmail_m / hoodie_m)

- **-Z = chest/front** · **+X = anatomical LEFT** · **+Y = up** · origin at feet
  (ground Y=0); mesh authored over the vanilla A-pose body.
- Measure orientation with ANATOMICAL COMPONENTS, never a global Z histogram
  (both ±Z masses are ~55/45 on real clothing — the histogram false-negatives a
  180° flip). Reliable signals: front detail components (hoodie drawstrings at
  Z −0.14..−0.07), pelvis/spine weighted centroids (vanilla ≈ Z −0.04..−0.08),
  `left*` selection centroids on +X.
- Torso clothing spans Y ≈ 0.61..1.66 m. Reference garment for overlay checks:
  `dz\characters\tops\chainmail_m.p3d` (body armor, full-body bone set).
- **Handedness (added 2026-10-01).** This frame holds a non-mirrored body in
  DayZ's left-handed space. A Blender garment (right-handed; front −Y and left
  +X, like the official rig) lands on it with `(x, y, z) → (x, z, y)`: det −1,
  faces in Blender order, normals negated (dayz-model-pipeline Rule 12). A
  det +1 map mirrors it: `(x, z, −y)` faces +Z (killer #2), and `(−x, z, y)`
  faces −Z with `left*` on −X. A mesh that needs "rotate 180° + swap L/R" was
  exported with such a map. ArmorHneck's received p3d was the `(x, z, −y)` of
  the artist's Blender file (`ArmorHneck_dev\CLAUDE.md`, 2026-08-04). Rotation
  + swap fixes the binding but keeps the mirror: text, logos, buckles and
  one-sided straps read reversed. The properly handed fix is a re-export of
  the Blender source with Rule 12, with no swap. With only the p3d, reflect it
  on the axis its state calls for: a p3d as received from `(x, z, −y)` (facing
  +Z, never rotated or swapped) takes `z → −z`; one that already had "rotate
  180° + swap L/R", i.e. `(−x, z, y)` like the shipped ArmorHneck, takes
  `x → −x` AND the L/R swap undone (`z → −z` on it would face +Z again).
  Either reflection moves every point (memory points included), reverses
  every face and reflects the normals the same way; `P3D.transform()` with
  that diagonal matrix (det −1) does all three.
- **Chiral in-game check [run 2026-10-02, DayZDiag 1.29.163709].** The anatomical test above
  rules out a det +1 export only while the L/R selections were never swapped by
  hand; a swapped build needs an asymmetric feature. Put an "F" in relief on the
  left chest of a test garment (weighted 100 % to `spine3`). Export it once with
  Rule 12 and once with the old `(x, z, −y)` + rotate + swap as the control,
  wear each with an item in hands and set the camera in front. Pass: the "F"
  reads correctly and sits on the side opposite the item hand (items sit on
  `RightHand_Dummy`). Expected on the control: the "F" mirrored, on the item
  side. On ArmorHneck itself, compare one asymmetric detail against the
  artist's Blender file.
  Result, on a vest-like test garment worn in the `Armband` slot (binarized,
  ODOL skeleton `DayzTemporarySkeleton`, driven by dayz-mcp): the Rule 12
  build (`py3d.blender_to_dayz()`), worn by the player with a crowbar in the
  right hand, shows the "F" reading correctly on the wearer's left chest,
  opposite the crowbar. The control (`ROT_X_NEG90`, every face reversed and
  every normal negated, then 180° about +Y) shows the "F" mirrored on the
  wearer's right chest. The garment carries only `spine3`, so the control's
  L/R swap renamed nothing. dayz-mcp cannot take a worn item off the player,
  so the control was worn by a `SurvivorM_Francis` spawned beside the player,
  facing the camera with empty hands; its right side was read from its facing.
  (claim: CLAIM-CLOTHING-CHIRAL-INGAME) Not covered: ArmorHneck itself, a
  garment with left/right-weighted parts, a female body.

## THE CLOTHING CONTRACT (structure of a working wearable)

config.cpp (text!):
- Class inherits a vanilla clothing base for the slot (`Armband_ColorBase`,
  `Chainmail`, vest/top bases...). `model=` the GROUND p3d (`*_g.p3d`).
- `class ClothingTypes { male="..._m.p3d"; female="..._f.p3d"; };`
- `inventorySlot[]` names the worn slots. 1.30: vanilla NBC pieces list a primary slot plus an
  `NBCHead`/`NBCTop`/`NBCPants`/`NBCBoots`/`NBCGloves` secondary slot. The player body does **not**
  expose those slots (`SurvivorBase.attachments[]`, `P:\DZ\characters\data\config.cpp:122`), so they
  do not let the suit be worn over regular clothing; they are slots for a container (likely the NBC
  pack, config not in `P:\DZ`) — see the reference. (Before 1.30: primary slot only.)
- `hiddenSelections[]` per model section; textures/materials arrays aligned.
- **Declare `varWetMax` on every garment.** 1.30 lowered the `Inventory_Base` default to `0.049` (=
  `varWetMin`; `P:\DZ\data\config.cpp:3229-3231`; before 1.30: `0.249`), so a garment without its own
  value can never get damp, keeps full insulation in rain, and cannot be wetted by hand. Vanilla
  headgear uses `0.249`, bandanas `1`.
- 1.30: type metadata lives on `ItemBaseType` (`P:\scripts\4_world\entities\itembasetype.c`):
  `headSelectionsToHide[]`, `hideSelectionsByinventorySlot`, `varWetInit/Min/Max`, `heatIsolation`,
  `temperaturePerQuantityWeight`, `EnvironmentWetnessIncrements`. Set them in **config**; the 1.29
  instance fields (`m_VarWetMax`, `m_HeadHidingSelections`, …) are gone, and `GetHeatIsolationInit()` is
  obsolete and no longer called (override `GetHeatIsolation()` if you must compute it). Dual-slot
  headgear (bandana/shemag) still uses `hideSelectionsByinventorySlot = 1` plus `simpleHiddenSelections[]`.
- Goggles / masks / visors that should block sandstorm dust: inherit `Glasses_Base` (`EYEWEAR`),
  `Mask_Base` / `MaskBase` (`MASK`) or override `GetCachedEquipmentCategory()`, and give
  `class Protection` `dust_particle_eyes` / `dust_particle_breath` values (1.30 only checks
  `EYEWEAR|MASK` items — reference § Sandstorm protection).
- Damage rvmats referenced only from healthLevels must be re-added at pack
  time (AddonBuilder whitelist gap — see dayz-pbo-build SP-155).

Worn p3d (_m/_f):
- Visual LODs skinned by NAMED SELECTIONS in lowercase = bone names
  (leftarm, rightarm, leftforearm, rightforearm, leftupleg, rightupleg,
  neck, pelvis, spine, spine3 — more is better: vanilla also weights
  shoulder/roll/extra transition bones for smooth joints).
- Per-vertex weight sums MUST be 1.0 (vanilla ODOL encodes raw bytes summing
  255). 0/1 plate-rigid weights WORK but articulate coarsely.
- Camo sections: tops/vests use `camoMale` / `camoFemale` alone; ONLY armbands
  carry the 4 complexion variants (camoMale_big_a/b + small_a/b). + `zbytek`.
- Empty Geometry LOD with `autocenter=0` = the vanilla worn pattern (keep it).
- Ground `_g`: normal item p3d (Geometry+mass+Component01, memory points,
  ViewGeo, FireGeo with a penetration rvmat).

model.cfg:
- Copy `references/model.cfg.template` VERBATIM (CfgSkeletons
  DayzTemporarySkeleton, 159 exact pairs, isDiscrete=0) and add one CfgModels
  class per p3d (class name == p3d filename) with
  `skeletonName="DayzTemporarySkeleton"` and its sections.

## BUILD (staged — see ArmorHneck_dev\tools\build.ps1 as template)

1. Stage the mod to %TEMP% (binarize with -addon=P:\ dies on any broken
   third-party config in the tree — "Error 3", names the wrong culprit).
   Stage beside it every file the p3d cite outside the mod's prefix, the
   vanilla penetration rvmat of the Fire Geometry included, or binarize
   embeds those materials EMPTY (dayz-pbo-build SP-155 rules 1 and 5,
   corrected 2026-10-03: ArmorHneck's ODOL of 2026-08-04 carries
   `dz\data\data\penetration\armor_5mm_plate.rvmat` empty).
2. Full AddonBuilder pass to binarize the p3d (model.cfg NEXT TO the p3d in
   the staging; ODOL must come out with skeleton name DayzTemporarySkeleton —
   verify with the external ODOL parser, `model_info.skeleton.name`).
3. Assemble pack tree: config.cpp TEXT + model.cfg + ODOL p3d + ALL paa/rvmat
   (including damage/destruct) → AddonBuilder `-packonly` (separate -temp!).
4. Verify PBO entries (classname, NO config.bin, ODOL magic) → deploy by hash.
5. Gate = in-game: spawn (registers?) → equip (worn renders on body?) → move
   (follows animations?). A worn item CANNOT be validated offline-only.

## DIAGNOSIS LADDER (when a worn item misbehaves)

| Symptom | Cause | Verify with |
|---|---|---|
| Item unknown/unspawnable, no RPT error | config.bin in PBO (killer #1) | spawn gate; PBO entry list |
| Worn = rigid block floating ~1 m, rotated, no anim | skeleton name != DayzTemporarySkeleton (killer #3) | ODOL `model_info.skeleton.name` vs vanilla |
| Worn = EXPLODED separate pieces above head | mesh frame flipped 180° (killer #2) | anatomical component Z-signs vs chainmail |
| Fits but plates misaligned (arms/legs) | rip proportions vs canonical bind | alignment viewer (below) or artist pass |
| Body/arms invisible when equipped | expected: torso-slot hide (Body/Vest classes hide skin) | wear it with the intended slot class |
| Coarse joints while moving | 0/1 plate weights, no transition bones | optional weight-smoothing pass |
| Gas mask worn, still takes bio/gas damage | 1.30: GP5/Airborne `Protection` no longer carries biological/dust_particle_breath; those come from attached `GasMask_Filter` | config `class Protection` vs slot `GasMaskFilter` |
| Custom backpack cannot attach NBC pack | 1.30: backpack `attachments[]` missing `"NBCBag"` (not every vanilla bag has it) | compare `TaloonBag_ColorBase` vs `SmershBag` / `ChildBag_ColorBase` |
| Garment never gets damp/wet in rain, "wet clothing" action never offered (1.30) | no `varWetMax` declared → inherits `0.049` = `varWetMin` | config `varWetMax`; `GetWetMax()` in a debug print |
| Custom insulation override does nothing (1.30) | mod overrides `GetHeatIsolationInit()` / sets `m_HeatIsolation`; 1.30 reads `ItemBaseType.m_HeatIsolation` | set `heatIsolation` in config or override `GetHeatIsolation()` |
| Custom goggles/visor/mask give no sandstorm protection (1.30) | item category is not `EYEWEAR`/`MASK`, or no `dust_particle_*` in `Protection` | `GetCachedEquipmentCategory()`; reference § Sandstorm protection |

**Config-vs-p3d bisection (one config-only rebuild, answers "which side is broken"):**
add two test classes — {your slot class + a VANILLA worn p3d (chainmail)} and
{a vanilla clothing class + YOUR p3d}. Vanilla-p3d-OK + custom-p3d-broken in
both classes = the p3d. Both OK = your main class fields. Both broken = pipeline.

**ODOL parity check vs a debinarized vanilla worn** (external ODOL parser):
skeleton name + 159 bones, per-vertex weight sums, sub_skeleton palette (unused
palette entries with 0 weight are normal in vanilla), sections, ModelInfo flags.
SP-034 caveat: vanilla CHARACTER BODY vertex positions decode corrupt, but
clothing worn p3ds decoded fine (armband, chainmail, hoodie all plausible).

## ALIGNMENT VIEWER (fit plates to the body interactively)

`references/alignment_viewer_extract.py` → JSON of mesh + per-region bone
weights + pivots (+ vanilla chainmail ghost as the placement reference);
`references/alignment_viewer_build.py` → self-contained HTML (embedded
three.js UMD): per-region rotation/offset sliders (weight-masked, around the
region pivot), L/R mirroring, front/back/side views, exports an adjustments
JSON. `references/apply_alignment_json.py` applies that JSON to the MLOD
sources with the EXACT same math (three.js Euler XYZ replicated; cross-check
a sample vertex against the live viewer before batch-applying — err ~1e-8).
Works for arbitrary region sets (arm/leg/abdomen masks, combined selections).

## ARTIST HANDOFF (when a human does the fitting — the usual best route)

`references/export_clothing_fbx.py` (Blender headless + py3d): MLOD worn →
FBX with geometry, UVs (V flipped for Blender), the diffuse as PNG
(ImageToPAA paa→png), and the bone weights carried by a 10-bone WEIGHT-CARRIER
armature (FBX drops loose vertex groups — verified: without an armature
deformer the groups vanish on reimport; with it, round-trip keeps all 10
groups at exact weight sums). Axis map DayZ→Blender: `(x, z, y)`, det −1 and
its own inverse, so the return trip is the same map with faces in the order
they come back and normals negated (dayz-model-pipeline Rule 12); with py3d
≥ 1.8.0 that is `py3d.blender_to_dayz()` on the model rebuilt from the FBX in
Blender coordinates. The artist sees the garment like the official rig: Z up,
front −Y, left +X, faces outward. [OFFLINE MEASURED 2026-10-01, ArmorHneck
`_m`, Blender 5.1.1 + py3d 1.7.0] FBX round trip back to DayZ exact to 2.7e-7 m
with the same face order; `leftarm` centroid x = +0.34, pelvis/spine centroid
y = −0.045; signed volume +0.026 in Blender, i.e. faces outward. Until
2026-10-01 the script used `(x, −z, y)` (det +1) with `py3d.BLENDER_TO_DAYZ`
for the return. The artist then saw a mirror image (front +Y, left +X) with
inward faces (signed volume −0.026), and only that exact inverse brought it
back. A package made by the old script returns through `(x, z, −y)`, i.e.
`transform(py3d.ROT_X_NEG90)` (the name py3d 1.8.0 gives that matrix; the
old `py3d.BLENDER_TO_DAYZ` still works but is deprecated and warns), never
through Rule 12 or `py3d.blender_to_dayz()`, which would land it facing +Z,
mirrored and inside-out. ALWAYS verify by reimporting the FBX in a clean
scene (counts + vgroups + weight sums) before shipping.
Package: FBX m/f + PNG + the MLOD p3ds + model.cfg + README from
`references/LEEME-ARTISTA.template.md` (states the three do-not-undo fixes,
the group-preservation rule, and the return flow). Scripts carry the
ArmorHneck paths — adapt per garment.

## PROVENANCE

ArmorHneck case (full trail): `ArmorHneck_dev\CLAUDE.md`, research workspace
`%TEMP%\armorhneck_research_ws\REPORT.md` (Codex dual research: canonical frame
measurement + transform prescription). Cross-refs: dayz-characters §2026-08-03
(same finding, humanoid context), dayz-pbo-build SP-155 (staged build + rvmat
gap), dayz-test-ingame (launch/lifecycle), external ODOL parsing (converter not distributed).

## DayZ 1.30 changes

Re-verified on 1.30.164078 (`P:\`). Worn-mesh killers #1–#3, the canonical frame,
`DayzTemporarySkeleton`, and the staged packonly build are unchanged. What 1.30 adds is
**config/script contract** around slots, protection, wetness, insulation and one classname rename.
Details, line numbers and `[EXACT]` blocks: [dayz-1-30-clothing.md](references/dayz-1-30-clothing.md).

### What changes

- **NBC secondary slots.** Vanilla NBC hood/jacket/pants/boots/gloves list both the primary clothing
  slot and `NBCHead` / `NBCTop` / `NBCPants` / `NBCBoots` / `NBCGloves` (`CfgSlots` in
  `P:\scripts\config.cpp:3115-3150`). The player (`SurvivorBase`) has none of these slots; they are
  container slots (most likely the NBC pack — its config is not in `P:\DZ`, `[UNVERIFIED]`), not a
  way to layer the suit over clothes. (The Exp draft said "coexist with regular clothing": wrong.)
- **`NBCBag` on some backpacks, not all.** `TaloonBag_ColorBase`, `TortillaBag`,
  `DryBag_ColorBase`, `HuntingBag`, `MountainBag_ColorBase`, `AssaultBag_ColorBase`,
  `Attack2Bag_ColorBase`, `CoyoteBag_ColorBase`, `AliceBag_ColorBase` append `"NBCBag"`; `SmershBag`
  and `ChildBag_ColorBase` do not. The pack item is scripted as `NBCPack_ColorBase`
  (`Container_Base`, refuses attachments that hold cargo); no `CfgVehicles` class in `P:\DZ`.
- **GP5 / Airborne protection moved to the filter.** 1.30: both masks keep only
  `dust_particle_eyes = 1`; `GetProtectionLevel` returns the attached `GasMask_Filter`'s level
  (`P:\scripts\4_world\entities\itembase\clothing\gp5gasmask.c:13-28`). The sealed `GasMask` still has
  full passive Protection. There is no class named `FilterBase`. (Before 1.30: the masks themselves
  had `biological = 1` and `dust_particle_breath = 1`.)
- **`ItemBaseType`** (new class) parses `headSelectionsToHide`, `hideSelectionsByinventorySlot`,
  `varWetInit/Min/Max`, `heatIsolation`, `temperaturePerQuantityWeight`,
  `EnvironmentWetnessIncrements`; `ItemBase` getters read the type. The 1.29 instance fields
  (`m_VarWetMax`, `m_HeadHidingSelections`, …) are removed; `GetHeatIsolationInit()` is obsolete and
  no longer called.
- **`varWetMax` default 0.249 → 0.049** on `Inventory_Base` (`P:\DZ\data\config.cpp:3231`).
- **Sandstorm protection** looks only at items whose `GetCachedEquipmentCategory()` is `EYEWEAR` or
  `MASK` (new enum values, `P:\scripts\3_game\enums\ecachedequipmentitemcategory.c:9-10`) and reads
  their `dust_particle_eyes` / `dust_particle_breath`. Several vanilla glasses had their
  `dust_particle_eyes` lowered; `MotoHelmet_ColorBase` lost it.
- **Headgear vs sun.** `Environment.HeadGearProtectionAgainstSun()` scales sun heating by the
  headgear's heat isolation (`P:\scripts\4_world\classes\environment\environment.c:2037-2065`).
- **Shoe `itemSize` width.** Most footwear bases went from width 4 to width 3 (height also shrank on
  several boots). `LeatherShoes_ColorBase` stays `{4,2}`.
- **`WaterPouch_*` → `Waterskin_*`** in `CfgVehicles`. Script stub `WaterPouch_ColorBase:
  Bottle_Base{}` remains. Craft: 2 `TannedLeather` + `LeatherSewingKit` (−10) → `Waterskin_Natural`.
  De-craft: empty `Waterskin_ColorBase` + blade (−12 HP) → 1 `TannedLeather`.
- **`CraftArmbandFlag.CanDo`** no longer requires `IsEmpty()`; it only rejects a flag that is already
  an attachment. Extra knives (`Jambiya`, `Scimitar`, `DonerKnife`, `Akinaka`) are listed as tools
  (script-only classes on 1.30.164078 — no config in `P:\DZ`).
- **Wash head / wet clothing.** New `ActionWashHead*` / `ActionWetClothing*` apply per-slot wetness
  (capped by `varWetMax`) and `GetThermalBiasHandler().Add(-THERMAL_BIAS_WASH_HEAD_DECREMENT)`.
  `[CHANGELOG 1.30]` known issue "bottles can still Wash Head while frozen" — still true in
  `P:\scripts`: `ActionWashHeadItemContinuous.ActionCondition` does not call `GetIsFrozen()`.
- **New garments, script-only.** `OBRCap/OBRJacket/OBRPants_ColorBase`, `FNOJacket`, `FNOPants`,
  `KameezShirt_ColorBase`, `Mens/WomensShalwarPants_ColorBase`, `TurbanHat_ColorBase`,
  `PakolHat_ColorBase`, `SandalsShoes_ColorBase`, `TraditionalSandalShoes_ColorBase`,
  `DustGoggles_ColorBase`, `ChestRigVest/LinenVest/RobeVest_ColorBase`, `ShortSleeveShirt_ColorBase`,
  `PoliceShortSleeveShirt_ColorBase`, `LeatherHipPack_ColorBase` have scripts (some with IK) but **no**
  config class in `P:\DZ` on 1.30.164078 (`[UNVERIFIED]` whether a DLC/map PBO ships them).

### What breaks for a ≤1.29 clothing/gear mod

1. **Compile:** code that touches the removed `ItemBase` fields (`m_VarWetMax`, `m_VarWetMin`,
   `m_VarWetInit`, `m_HeadHidingSelections`, `m_HideSelectionsBySlot`, `m_VarQuantity*`, …) — move the
   value to config.
2. **Silent:** a garment without its own `varWetMax` stays dry forever (0.049 cap).
3. **Silent:** `GetHeatIsolationInit()` overrides / `m_HeatIsolation` writes no longer change
   insulation.
4. Custom backpacks without `"NBCBag"` cannot take the NBC pack attachment.
5. GP5/Airborne clones with hardcoded `biological` on the mask, or that skip
   `attachments[] = {"GasMaskFilter"}`, will not match 1.30 filter-gated protection.
6. Custom goggles/visors that do not report `EYEWEAR`/`MASK` give no sandstorm protection.
7. `CreateObject("WaterPouch_Natural")` / recipes naming `WaterPouch_*` fail: the config class is
   `Waterskin_Natural`.
8. Shoe mods still at width 4 occupy more grid than vanilla 1.30 footwear (balance, not a break).

### Migration checklist

- [ ] Declare `varWetMax` (and `heatIsolation`) on every clothing class; drop script writes to the
      removed `m_Var*` fields.
- [ ] Replace `GetHeatIsolationInit()` overrides with config `heatIsolation` or a
      `GetHeatIsolation()` override.
- [ ] NBC-like pieces that should fit the NBC pack: add the matching `NBC*` secondary slot (they will
      not be wearable over regular clothing either way).
- [ ] Backpacks that should carry the NBC pack: `attachments[]` includes `"NBCBag"`.
- [ ] Filter masks: `attachments[] = {"GasMaskFilter"}`; biological protection lives on
      `GasMask_Filter`, not on the mask `Protection` block.
- [ ] Eye/face protection: `GetCachedEquipmentCategory()` returns `EYEWEAR`/`MASK` and `Protection`
      has `dust_particle_eyes` / `dust_particle_breath`.
- [ ] Declare `headSelectionsToHide[]` / optional `EnvironmentWetnessIncrements` on the clothing class
      (read via `ItemBaseType`).
- [ ] Replace `WaterPouch_*` with `Waterskin_*` (optional 1.29 alias: empty
      `WaterPouch_Natural: Waterskin_Natural { scope = 1; }` `[DESIGN]`, untested).
- [ ] Recheck shoe `itemSize[]` if you balanced against vanilla width 4.
- [ ] Armband-from-flag: `CanDo` must not assume `IsEmpty()`.
- [ ] Gate remains in-game: spawn → equip → move (killers #1–#3 unchanged).

