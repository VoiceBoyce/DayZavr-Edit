# Config Wiring and Damage Materials

Use this reference when connecting textures and materials to DayZ configs.

## Texture-only hidden selection

Use `hiddenSelectionsTextures[]` when only the color/albedo changes and material response remains the same.

[DESIGN]

```cpp
hiddenSelections[]=
{
	"camo"
};
hiddenSelectionsTextures[]=
{
	"myaddon\data\asset_co.paa"
};
```

Verify the selection name in the model and config before using it. Some selections can hide geometry until a texture is assigned; local knowledge calls out `camoGround` as one such trap.

## Material swap through config

Use `hiddenSelectionsMaterials[]` when the material itself changes: SMDI, normal, emissive, glass response, or damage material.

[EXACT] Vanilla AKM pattern (1.30: two selections — `AKM_Base` declares
`hiddenSelections[]={"camo","camo2"}`, `P:\DZ\weapons\firearms\akm\config.cpp:72-76`):

```cpp
hiddenSelectionsTextures[]=
{
	"dz\weapons\firearms\AKM\data\akm_co.paa",
	"dz\weapons\firearms\AKM\data\akm_co.paa"
};
hiddenSelectionsMaterials[]=
{
	"dz\weapons\firearms\AKM\data\AKM.rvmat",
	"dz\weapons\firearms\AKM\data\AKM_grip.rvmat"
};
```

Source: `P:\DZ\weapons\firearms\akm\config.cpp:208` to `:217` (1.30.164078).

Before 1.30: `AKM_Base` had `hiddenSelections[]={"camo"}` and one texture/material
(`AKM.rvmat`). **Porting a 1.29 retexture:** a subclass of a vanilla class that now has
more hidden selections and still writes one-entry arrays only repaints index 0; the new
selection keeps the vanilla look. Write one entry per vanilla selection (and the matching
`healthLevels[]` materials). Vanilla classes whose effective `hiddenSelections[]` changed
in 1.30: `AKM_Base`, `AK_WoodBttstck`, `AK_WoodHndgrd` (`{"camo"}` → `{"camo","camo2"}`);
`Magnum_Base` (new `{"camo","camo2"}`), `Magnum_Cylinder`, `Magnum_Ejector` (new
`{"camo"}`); `Glock19` (new `{"zasleh","zbytek","magazine","bullet"}`); `Offroad_02`
(four paint selections appended after the nine lights: `camo`, `camo_interior`,
`camo_roof`, `camo_interior_roof` — `P:\DZ\vehicles\wheeled\config.cpp:20922-20937`) and
`Offroad_02_Door_1_1` (new
`texture_roof_ext`/`texture_roof_int`). Many other vanilla texture paths were only
lower-cased or moved from `.tga` to `.paa` — full list in
`dayz-130-migration/references/config-delta-1.29-to-1.30.md`.

[DESIGN] Adapted addon pattern:

```cpp
hiddenSelections[]=
{
	"camo"
};
hiddenSelectionsTextures[]=
{
	"myaddon\data\asset_co.paa"
};
hiddenSelectionsMaterials[]=
{
	"myaddon\data\asset.rvmat"
};
```

The arrays must align with the model selections. Do not add a material path for a selection that does not exist.

## Damage and destruct health levels

Use `healthLevels[]` to bind normal, damaged, and destroyed materials.

[EXACT] Vanilla AKM structure (1.30: one material per hidden selection in every level):

```cpp
healthLevels[]=
{
	{ 1, { "DZ\weapons\firearms\AKM\data\AKM.rvmat", "DZ\weapons\firearms\AKM\data\AKM_grip.rvmat" } },
	{ 0.69999999, { "DZ\weapons\firearms\AKM\data\AKM.rvmat", "DZ\weapons\firearms\AKM\data\AKM_grip.rvmat" } },
	{ 0.5, { "DZ\weapons\firearms\AKM\data\AKM_damage.rvmat", "DZ\weapons\firearms\AKM\data\AKM_grip_damage.rvmat" } },
	{ 0.30000001, { "DZ\weapons\firearms\AKM\data\AKM_damage.rvmat", "DZ\weapons\firearms\AKM\data\AKM_grip_damage.rvmat" } },
	{ 0, { "DZ\weapons\firearms\AKM\data\AKM_destruct.rvmat", "DZ\weapons\firearms\AKM\data\AKM_grip_destruct.rvmat" } }
};
```

(Before 1.30 each level listed only the `AKM*.rvmat` entry.)

Source: `P:\DZ\weapons\firearms\akm\config.cpp:301` to `:348` (1.30.164078, written one entry per line).

For custom assets:

- keep normal/damage/destruct material stage layouts aligned;
- change Stage3 to a damage/destruct overlay texture;
- keep Stage1 normal and Stage5 SMDI consistent unless damage should also change those maps;
- verify the class hierarchy actually uses `DamageSystem` for the asset type.

## Damage/destruct rvmat pattern

Vanilla Saiga evidence:

- normal material Stage3 is procedural MC: `P:\DZ\weapons\shotguns\saiga\data\saiga.rvmat:35`.
- damage material Stage3 uses `dz\weapons\data\weapons_damage_generic_mc.paa`: `P:\DZ\weapons\shotguns\saiga\data\saiga_damage.rvmat:35`.
- destruct material Stage3 uses `dz\weapons\data\weapons_destruct_generic_mc.paa`: `P:\DZ\weapons\shotguns\saiga\data\saiga_destruct.rvmat:35`.
- damage/destruct Stage3 uses 4x tiling: `saiga_damage.rvmat:39` to `:40`, `saiga_destruct.rvmat:39` to `:40`.

## Vehicle light material swaps

Vehicles often use config properties that point to on/off material pairs.

[EXACT] Vanilla sedan excerpt:

```cpp
dashboardMatOn="dz\vehicles\wheeled\sedan_02\data\sedan_02_int2e.rvmat";
dashboardMatOff="dz\vehicles\wheeled\sedan_02\data\sedan_02_int2.rvmat";
frontReflectorMatOn="dz\vehicles\wheeled\sedan_02\data\sedan_02_chrome_e.rvmat";
frontReflectorMatOff="dz\vehicles\wheeled\sedan_02\data\sedan_02_chrome.rvmat";
```

Source: `P:\DZ\vehicles\wheeled\config.cpp:14045` to `:14048` (1.30.164078).

When adapting:

- do not assume these properties apply to non-vehicle classes;
- clone the nearest vehicle family;
- keep on/off material paths paired and verify case-sensitive packed paths.

## Runtime material/texture changes

If a task needs runtime swaps from script, verify the method and side in the target codebase before writing final code. DayZ client/server API names are easy to confuse. For a skill response, prefer config-driven swaps unless runtime behavior is explicitly required.

