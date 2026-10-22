# DayZ 1.30 changes — model pipeline (build 1.30.164200)

Companion to `SKILL.md` § DayZ 1.30 changes. The LOD resolution table, py3d assembly
path, winding rules, and artist-return preflight (SP-246) still hold. This file
is the 1.30 engine/config delta for meshes, proxies, houses, and cache.

Script and config line numbers point at `P:\` (unpacked 1.30.164200), re-checked
2026-10-15. Changelog quotes come from the official 1.30 modding changelog
transcription (`work\changelog-1.30-exp-modding.md`, not shipped in this pack);
a statement that rests only on it is tagged `[CHANGELOG 1.30]`. Digest O/F/H
line numbers were **not** copied.

## Vanilla P3Ds are ODOL v56

[EXACT] Header census 2026-10-15 (bytes 0-3 = `ODOL`, bytes 4-7 = uint32 version):
all 8,253 `*.p3d` under `P:\DZ` (1.30.164078) are ODOL **56**; all 8,188 under the
1.29 extraction are ODOL **54**. Example: `P:\DZ\weapons\firearms\m249\m249.p3d`
starts `4F 44 4F 4C 38 00 00 00` (1.29: `... 36 00 00 00`).

Consequence: an ODOL→MLOD reader or anatomy diff that only knows v54/v55 fails on a
1.30 vanilla reference (`ValueError: subsection not found` with the v54 reader). Read
the header before extracting; see `dayz-vehicles/references/dayz-1-30-vehicles.md`.
What version DayZ Tools' binarize writes for YOUR model is a separate question — read
the header of your own output too.

## Inventory-attachment proxy position on dedicated server

Before 1.30: if the proxy triangle was missing from the **final** visual LOD, a
dedicated server could report the attachment at the parent origin / mesh center.
1.30 [CHANGELOG 1.30]: the server falls back to the **View Geometry LOD** to read
the inventory-attachment proxy position.

Official changelog (`work\changelog-1.30-exp-modding.md:39`):

> Fixed: Getting the position of an inventory attachment proxy on the server
> will now use the view geometry LOD to prevent getting the center of the
> parent object if the proxy was not defined in the final lod

[DESIGN] Duplicate the proxy triangle + `proxy:…p3d.NNN` selection in
ViewGeometry (6.0e15), not only in Resolution LODs. The visual-LOD requirement
in `memory-and-selections.md` remains; ViewGeometry is now the server-side
fallback, not a substitute for visual proxies.

There is no Enforce Script call-site in `P:\scripts` that names the LOD
fallback — it is engine C++. Do not invent an API for it.

## `-cacheP3D=0/1` and `.p3dcache` next to unbinarized meshes

[CHANGELOG 1.30], `work\changelog-1.30-exp-modding.md:11`:

> Added: '-cacheP3D=0/1' command line launch option, creates .p3dcache files
> next to .p3d files to allow for quicker subsequent launches of the
> Game/Buldozer when working with unbinarized assets

[DESIGN] Use it on DayZDiag / Buldozer loops that iterate **MLOD** `.p3d` (not
yet AddonBuilder-binarized). The cache files are siblings of the `.p3d`; they
are not packed into a shipping PBO. No `.p3dcache` ships under `P:\DZ` (every
vanilla P3D is binarized ODOL), so the flag itself is changelog-only here — not
exercised on 164078 on this machine.

Cross-ref launch argv: `dayz-test-ingame` § DayZ 1.30 changes and
`dayz-test-ingame/references/dayz-1-30-test-ingame.md`.

## `disableSimulation` on house-based entities

[CHANGELOG 1.30], `work\changelog-1.30-exp-modding.md:13`:

> Added: 'disableSimulation' config parameter to house-based entities to
> eliminate unnecessary entity ticking impacting client/server performance

Script API on `Entity` (verified; NOT new — 1.29 declared the same natives at
`scripts\3_game\entities\entity.c:3`):

```
// [EXACT] P:\scripts\3_game\entities\entity.c:1-6
class Entity extends ObjectTyped
{
	proto native void DisableSimulation(bool disable);

	//! Returns whether simulation is disabled
	proto native bool GetIsSimulationDisabled();
```

[UNVERIFIED] No `config.cpp` (or any other file) under `P:\DZ` assigns
`disableSimulation` (grep 2026-10-15, 1.30.164078; none in 1.29 either). The
config key is engine-side and changelog-only; the script natives are exact.
Prefer the config parameter on static `House` / house-based classes that never
need per-tick simulation; use `DisableSimulation(true)` only when toggling at
runtime.

## Selection `liquid_source` (wells / liquid interactables)

New in 1.30 scripts (absent from the 1.29 scripts). Liquid actions that target a
**well object** (not a pond surface) look up ViewGeometry components named
`liquid_source`:

```
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\actionconstants.c:175-181
class UAMisc
{
	const float FLAG_STEP_INCREMENT = 0.2; //0..1
	const float CONSUMPTION_SLOWDOWN_COEF_HOT = 5.0;
	
	const string LIQUID_SOURCE_SELECTION = "liquid_source";
};
```

Call-sites (same string, ViewGeometry level), all under
`P:\scripts\4_world\classes\useractionscomponent\`:
`actions\continuous\actiondrinkwellcontinuous.c:54`,
`actions\continuous\actionfillbottlebase.c:62`,
`actions\continuous\actionwashhandswell.c:38`,
`actions\continuous\actionwashheadwell.c:38`,
`actions\continuous\actionwetclothinginhandswell.c:30`,
`targetconditionscomponents\cctliquid.c:89`
(`GetActionComponentsForSelectionName(…GetViewGeometryLevel(),
UAMisc.LIQUID_SOURCE_SELECTION, …)`).

**The selection is optional; it narrows, it does not enable.** Every call-site
rejects the target only when the model HAS `liquid_source` components and the
aimed component is not one of them:

```
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actiondrinkwellcontinuous.c:53-56
			TIntArray componentIndices = new TIntArray();
			targetObject.GetActionComponentsForSelectionName(targetObject.GetViewGeometryLevel(), UAMisc.LIQUID_SOURCE_SELECTION, componentIndices);
			if (componentIndices.Count() > 0 && componentIndices.Find(target.GetComponentIndex()) == -1)
				return false;
```

`cctliquid.c:91-94` is the same rule (`Count() > 0` → must hit one of them;
otherwise `isValidInteract = true`). What makes an object a well is still script:
`GetWaterSourceObjectType()` returning `EWaterSourceObjectType.WELL`
(`P:\scripts\4_world\entities\building\well.c:8-11`; `IsWell()` in
`P:\scripts\3_game\entities\object.c:664-667` is deprecated in its favour).

[DESIGN] A custom well / pump `.p3d` works without `liquid_source`, as on 1.29.
Add a ViewGeometry named selection `liquid_source` (not a memory point) only to
limit the wash/drink/fill prompt to the spout or water surface of a large
model. No vanilla P3D under `P:\DZ` contains the string `liquid_source`
(`grep -a`, 2026-10-15), so there is no vanilla model to copy it from.
Pond/sea surfaces go through `CCTLiquid` without this selection.

## `StaticObjectType` + `CfgNonAIVehicles` `VegetationSounds`

New in 1.30 scripts (no `staticobjecttype.c` in 1.29). Loads optional per-object
vegetation step sounds from config:

```
// [EXACT] P:\scripts\3_game\entities\staticobjecttype.c:1-15
class StaticObjectType : EntityType
{
	//may not contain every soundset info, defaults from DayZPlayerType used in that case
	protected ref map<int, ref SoundObjectBuilder> m_CustomVegetationSoundsMap; //<stepEventID, SoundObjectBuilder>
	
	void StaticObjectType()
	{
		LoadCustomStepSounds();
	}
	
	protected void LoadCustomStepSounds()
	{
		string pathBase = "CfgNonAIVehicles " + GetName() + " VegetationSounds";
		if (!g_Game.ConfigIsExisting(pathBase))
			return;
```

Vanilla **player** vegetation macros live on `SurvivorBase` (not
CfgNonAIVehicles), and they are not new (1.29 had the same block at line 353):

```
// [EXACT] P:\DZ\characters\data\config.cpp:348-354
		class VegetationSounds
		{
			class bush_walkErc
			{
				soundSet="bush_walkErc_SoundSet";
				animEventIds[]={1,2,23,24,51,52};
			};
```

[UNVERIFIED] `P:\DZ` has **no** `class VegetationSounds` under a
`CfgNonAIVehicles` class (that `SurvivorBase` block is the only one, grep
2026-10-15). The loader path is exact; a shipping bush/tree example is not in
the unpacked data. Sound-set authoring belongs to `dayz-sound-system`.

## Animation graphs `.agr` → Enfusion `.agf` (out of scope for model.cfg)

Digest O attributed `.agr` / 250-bone / `skeletons.anim.xml` claims to
`references/animations.md`. That file is **model.cfg object animation**
(CfgSkeletons / CfgModels). It never mentioned `.agr`, a 250-bone cap, or
`skeletons.anim.xml`. Those are the **player/infected Enfusion animation
graph** (skill `dayz-animation-pipeline`).

[CHANGELOG 1.30], `work\changelog-1.30-exp-modding.md:24, 30-31, 43`:
animation-graph files use Enfusion config format; the 250 global bone limit is
removed; most of `skeletons.anim.xml` is unused except the `lod` attribute.
The file side is visible in `P:\DZ\anims\workspaces`: 1.30 ships 20 `.agf` next
to 13 `.agr` (e.g. `P:\DZ\anims\workspaces\player\player_main\locomotion.agf`);
1.29 shipped 26 `.agr` and no `.agf`.

**This skill's model.cfg contract does not change.** Do not convert object
`model.cfg` to `.agf`. Character/infected graph ports belong to
`dayz-animation-pipeline`.

## Unpacked-mod / named filesystem (packaging implication)

[CHANGELOG 1.30]: `-mod` may point at a physical folder; RV configs read
unpacked; mods get a named filesystem — `work\changelog-1.30-exp-modding.md:25-28`.
Iteration of **config.cpp** no longer always requires AddonBuilder. Unbinarized
`.p3d` still benefit from `-cacheP3D`. Shipping / Workshop still packs a PBO.
Operational launch: `dayz-test-ingame` (the Enfusion file-patching flag is
`-loadRVFilesViaEnfusionFS=0/1` since 1.30.164200, `-resolveFilePatchingUsingEnfusion` before).

## DayZ 1.30.164200 changes (model / config)

Patch 1.30.164200 (2026-10-21). Data side from `dayz-130-migration`
`config-delta-1.30.164078-to-164200.md` (install-vs-install comparison); notes
items are [reported: BI 1.30.164200 notes].

- **ODOL stays v56.** 3 377 vanilla `.p3d` were re-binarized, all still v56;
  69 changed the textures/materials they embed. Binarize and Workbench in DayZ
  Tools are 1.30.164.200 now (AddonBuilder unchanged, 1.0.240.639).
- **Workbench FBX import fixed** [reported: BI notes, "Importing FBX files as
  models (DZEXP-151)"]. Not tried here; this skill's path stays Blender → py3d.
- **Roadway surfaces carry the liquid type.** 38 irrigation-tunnel and water-stream
  models swapped the roadway texture `dz\surfaces\data\roadway\still_water_int.paa`
  for `clean_water_int.paa`, which is how "Irrigation tunnel water gave character
  cholera" was fixed: `still_water_int` is `liquidType=512` (`LIQUID_WATER`, drinks
  as pond → cholera), `clean_water_int` is `liquidType=4194304` (`LIQUID_CLEANWATER`,
  drinks as well) (`P:\DZ\surfaces\config.cpp:1775-1781`, `:1803-1809`). For water
  in a custom model, the roadway texture name picks the surface and so the liquid.
- **Navmesh blocks on dynamic structures.** `class Navmesh` in an entity config can
  hide or move model parts for navmesh generation: `HideParts { class x {
  hideAnimationSource="…"; } }` (+ `generateWithRoadway`, `generateWithProxies`) on
  `Land_IrrigationTunnel_Entrance_01` (`P:\DZ\structures_takistan\Industrial\config.cpp:25-35`)
  and `TransformParts { class Gate { transformAnimationSource="Wall_Gate_Rotate"; } }`
  on `Fence` (`P:\DZ\gear\camping\config.cpp:4418-4427`). The named AnimationSource
  must exist in the model's `model.cfg`/config. Vanilla navmeshes are now format v9
  (`dayz-ai-patterns` §Navmesh).
- **Material changes that retextures inherit**: ACOG (`acog.rvmat` re-lit, new
  `acog_as.paa`, new unused `acog_tan*` set), cooking pot (own `_as/_damage/_destruct`),
  GunCase (`RefTexsMats[]` damage swap, `healthLevels[]` 0.7/0.3 empty), 5.45×39
  rounds (`545x39_smdi.paa`). A mod that copied the old rvmat now looks different
  from vanilla.
- **`fsShadowing=1`** PBO property [reported: BI notes, not yet measured]: loads a PBO
  whose prefix matches another, aimed at Enfusion config files (`.layout`) via
  'Variants'. RV models/configs are not named; do not use it to override a vanilla
  `.p3d` until measured (`dayz-pbo-build`).

## Migration checklist (model / config)

1. Inventory attachments: proxy triangle in visual LODs **and** ViewGeometry.
2. Custom wells: optional ViewGeometry selection `liquid_source` to limit where
   the liquid actions trigger (a 1.29 well without it keeps working).
3. Static houses with no tick: consider `disableSimulation` (config key
   [CHANGELOG 1.30]; script toggle `Entity.DisableSimulation`).
4. Unbinarized mesh loop: `-cacheP3D=1` on Diag/Buldozer.
5. Duplicate **members** inside one config class now stop binarize
   [CHANGELOG 1.30] — see `dayz-pbo-build`.
6. Vanilla reference models are ODOL v56: update any ODOL reader before diffing.
7. Custom character/infected **graphs**: `.agf` via Workbench, not this skill.
