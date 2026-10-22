# DayZ 1.30 — clothing / gear contract

Companion to `SKILL.md` § DayZ 1.30 changes. Worn-mesh killers, canonical frame, and
`DayzTemporarySkeleton` are unchanged; this file is the config/script delta.

First written against 1.30 Experimental (164014); **re-verified on 1.30.164078 stable** by opening the
files under `P:\` (`P:\DZ\…\config.cpp` is the derapified 1.30 config, so its line numbers differ from
the Exp text configs the first draft quoted; `P:\scripts\…` for script). `Before 1.30:` lines cite the
1.29 extraction `O:\DayZ 1.29 Старый распакованный клиент\` (shortened `O:\…`). Digest L/E/F line
numbers were **not** copied. Labels: `../../_shared/dayz-version-policy.md`.

## NBC secondary slots (and what they are *not*)

1.30: each NBC clothing base lists its primary slot **and** a dedicated NBC slot. (Before 1.30:
primary slot only, e.g. `NBCHoodBase.inventorySlot[] = {"Headgear"}` — 1.29:
`O:\…\DZ\characters\headgear\config.cpp:7478`.)

| Class | `inventorySlot[]` on 1.30 | Where |
|---|---|---|
| `NBCHoodBase` | `{"Headgear","NBCHead"}` | `P:\DZ\characters\headgear\config.cpp:7487` |
| `NBCJacketBase` | `{"Body","NBCTop"}` | `P:\DZ\characters\tops\config.cpp:7570` |
| `NBCPantsBase` | `{"Legs","NBCPants"}` | `P:\DZ\characters\pants\config.cpp:4448` |
| `NBCBootsBase` | `{"Feet","NBCBoots"}` | `P:\DZ\characters\shoes\config.cpp:3185` |
| `NBCGloves_ColorBase` | `{"Gloves","NBCGloves"}` | `P:\DZ\characters\gloves\config.cpp:1207` |

```cpp
// [EXACT] P:\DZ\characters\headgear\config.cpp:7487
	class NBCHoodBase: Clothing
	{
		scope=0;
		displayName="$STR_CfgVehicles_NBCHoodBase0";
		descriptionShort="$STR_CfgVehicles_NBCHoodBase1";
		model="\DZ\characters\headgear\NBC_Hood_g.p3d";
		inventorySlot[]=
		{
			"Headgear",
			"NBCHead"
		};
```

`CfgSlots` names (ghost icons + display strings) live in `P:\scripts\config.cpp:3115-3150`:
`Slot_NBCBag` (`name="NBCBag"`, ghost `image:back`), `Slot_NBCHead` (`image:headgear`), `Slot_NBCTop`
(`image:body`), `Slot_NBCGloves` (`image:gloves`), `Slot_NBCPants` (`image:legs`), `Slot_NBCBoots`
(`image:feet`).

```cpp
// [EXACT] P:\scripts\config.cpp:3115
	class Slot_NBCBag
	{
		name="NBCBag";
		displayName="#STR_CfgVehicles_NBCPack_Colorbase0";
		ghostIcon="set:dayz_inventory image:back";
	};
	class Slot_NBCHead
	{
		name="NBCHead";
		displayName="#STR_CfgVehicles_nbchoodbase0";
		ghostIcon="set:dayz_inventory image:headgear";
	};
```

**Correction to the Exp draft.** The draft said the NBC slots let the suit "coexist with regular
clothing". On 1.30.164078 the player does **not** expose them: `SurvivorBase.attachments[]` is
`Head, Shoulder, Melee, Headgear, Mask, Eyewear, Hands, LeftHand, Gloves, Armband, Vest, Body, Back,
Hips, Legs, Feet, Splint_Right` (`P:\DZ\characters\data\config.cpp:122`), and no config under `P:\DZ`
other than the NBC pieces themselves mentions `NBCHead` / `NBCTop` / `NBCPants` / `NBCBoots` /
`NBCGloves`. The secondary slot is what lets a piece be attached **into** something that exposes it —
the NBC pack below, whose Badlands config lists exactly those five slots in `attachments[]`
(`P:\DZ\gear_takistan\containers\config.cpp:30-37`, on `P:\` since the 1.30.164200 unpack). Do not promise
"wear the NBC suit over normal clothes" in a mod.

## NBCBag on backpacks — not universal

1.30: nine backpack bases append `"NBCBag"`; the Exp digest's "every vanilla backpack" is false.
(Before 1.30: `TaloonBag_ColorBase.attachments[] = {"Chemlight","WalkieTalkie","Backpack_1"}` — 1.29:
`O:\…\DZ\characters\backpacks\config.cpp:47`.)

**Have `"NBCBag"`** (class line in `P:\DZ\characters\backpacks\config.cpp`): `TaloonBag_ColorBase:47`,
`TortillaBag:208`, `DryBag_ColorBase:861`, `HuntingBag:1050`, `MountainBag_ColorBase:1245`,
`AssaultBag_ColorBase:2890`, `Attack2Bag_ColorBase:3054`, `CoyoteBag_ColorBase:3218`,
`AliceBag_ColorBase:3370`.

**Do not:** `SmershBag:1412`, `ChildBag_ColorBase:1543`, plus courier/improvised/leather/sling/drysack
bases that never gained the slot.

```cpp
// [EXACT] P:\DZ\characters\backpacks\config.cpp:47
	class TaloonBag_ColorBase: Clothing
	{
		displayName="$STR_cfgVehicles_TaloonBag_ColorBase0";
		descriptionShort="$STR_cfgVehicles_TaloonBag_ColorBase1";
		model="\dz\characters\backpacks\taloon_g.p3d";
		debug_ItemCategory=9;
		inventorySlot[]=
		{
			"Back"
		};
		attachments[]=
		{
			"Chemlight",
			"WalkieTalkie",
			"Backpack_1",
			"NBCBag"
		};
```

The attachable pack is scripted as `NBCPack_ColorBase : Container_Base` (+ `NBCPack_Grey` / `_White` /
`_Yellow`) with `CanDisplayAttachmentCargo` forced false and `CanReceiveAttachment` refusing any
attachment that still holds cargo (`P:\scripts\4_world\entities\itembase\gear\containers\containers.c:83-107`).
Its config is Badlands DLC data (`P:\DZ\gear_takistan\containers\config.cpp`, on `P:\` since the
1.30.164200 unpack; corrected 2026-10-21, this used to say no config existed): `class NBCPack_ColorBase:
Container_Base` (`:20`), `inventorySlot[] = {"NBCBag"}`, `attachments[] = {"NBCHead","NBCTop","NBCGloves",
"NBCPants","NBCBoots"}` (`:26-37`), `itemSize[]={3,3}`, `itemsCargoSize[]={0,0}`, `varWetMax=0.249`, model
`\dz\gear_takistan\containers\NBCPack.p3d`; variants `NBCPack_White` / `_Grey` / `_Yellow` (`:112`, `:123`, `:134`).

## Gas masks and `GasMask_Filter`

There is **no** class `FilterBase`. Protection for cartridge masks is `GasMask_Filter`
(`P:\DZ\gear\consumables\config.cpp:4295`, script `GasMask_Filter : ItemBase` in
`P:\scripts\4_world\entities\itembase\clothing\gasmask_filter.c:1`).

1.30: `GP5GasMask` and `AirborneMask` **omit** `biological` and `dust_particle_breath`; they keep
`dust_particle_eyes = 1` (they do not write `= 0`). `GP5GasMask_Filter` (masks config) gained
`dust_particle_breath = 1`. (Before 1.30: GP5 `Protection` was `biological = 1; dust_particle_breath = 1;
dust_particle_eyes = 1;` — 1.29: `O:\…\DZ\characters\masks\config.cpp:950`, Airborne `:1085`. The Exp
digest's "1.29 was biological = 0.25" is false.)

```cpp
// [EXACT] P:\DZ\characters\masks\config.cpp:951
		class Protection
		{
			dust_particle_eyes=1;
		};
```

Same block on `AirborneMask` at `:1084-1087`. Both classes declare `attachments[] = {"GasMaskFilter"}`
(`GP5GasMask` class at `:845`, slot list `:855-857`; `AirborneMask` at `:974`, `:984-986`).

The sealed `GasMask` (no filter slot) still has full passive protection:

```cpp
// [EXACT] P:\DZ\characters\masks\config.cpp:134
		class Protection
		{
			biological=1;
			chemical=1;
			dust_particle_breath=1;
			dust_particle_eyes=1;
		};
```

Filter item:

```cpp
// [EXACT] P:\DZ\gear\consumables\config.cpp:4324
		class Protection
		{
			biological=1;
			chemical=1;
			dust_particle_breath=1;
		};
```

Runtime: with a filter attached, `GetProtectionLevel` returns the filter's level except
`DEF_DUST_PARTICLE_EYES`, which stays 1.0 on the mask. Without a filter it falls through to `super`
(config Protection on the mask). Before 1.30 `GP5GasMask.c` had no such override (1.29:
`O:\…\scripts\4_world\entities\itembase\clothing\gp5gasmask.c`).

```c
// [EXACT] P:\scripts\4_world\entities\itembase\clothing\gp5gasmask.c:13
	override float GetProtectionLevel(int type, bool consider_filter = false, int system = 0)
	{
		if (IsDamageDestroyed())
			return 0.0;

		ItemBase filter = ItemBase.Cast(FindAttachmentBySlotName("GasMaskFilter"));
		if (filter)
		{
			if (type == DEF_DUST_PARTICLE_EYES)
				return 1.0;

			return filter.GetProtectionLevel(type, false, system);
		}

		return super.GetProtectionLevel(type, consider_filter, system);
	}
```

`airbornemask.c:18-33` is the same override. `[DESIGN]`: a custom cartridge mask must attach
`GasMaskFilter` and must not rely on mask-level `biological`.

Other 1.30 `Protection` value changes (config-delta, `P:\DZ\characters\…`): `MotoHelmet_ColorBase` lost
`dust_particle_eyes` and `dust_particle_breath` went 1→0.25; glasses `dust_particle_eyes` lowered
(`AviatorGlasses` 0.5→0.4, `DesignerGlasses` 0.7→0.4, `ThickFramesGlasses` 0.7→0.4,
`ThinFramesGlasses` 0.7→0.3, `SportGlasses_ColorBase` 0.8→0.7).

## Sandstorm protection: `ECachedEquipmentItemCategory`

1.30 adds `EYEWEAR = 32` and `MASK = 64` to `ECachedEquipmentItemCategory`
(`P:\scripts\3_game\enums\ecachedequipmentitemcategory.c:1-11`; Before 1.30 the enum stopped at
`FIREARM_LONG = 16`). Sandstorm coughing and the squint symptom query the player's cached equipment
for `EYEWEAR|MASK` **attachments** (depth 2) and take the first item whose `GetProtectionLevel` is
`> 0` for `DEF_DUST_PARTICLE_BREATH` (cough) / `DEF_DUST_PARTICLE_EYES` (squint):
`P:\scripts\4_world\classes\playermodifiers\modifiers\conditions\sandstormexposure.c:126-141`,
`P:\scripts\4_world\classes\playersymptoms\states\secondary\squintstate.c:25-37`. The sandstorm agents
(`SILICOSIS`, `EYES_IRRITATION`) instead **sum** the protection of all matching attachments: breath over
`MASK`, eyes over `EYEWEAR|MASK|NVG`
(`P:\scripts\4_world\plugins\pluginbase\plugintransmissionagents.c:414-433`, helper `:577-594`).

Who reports a category (`GetCachedEquipmentCategory()` overrides, 1.30):
`Glasses_Base` → `EYEWEAR` (`P:\scripts\4_world\entities\itembase\clothing_base.c:176-179`),
`Mask_Base` → `MASK` (`clothing_base.c:196-199`), `MaskBase` (gas masks) → `MASK`
(`clothing\maskbase.c:6-9`), `AirborneMask` → `MASK|EYEWEAR`, `NVGoggles` → `NVG|EYEWEAR`, and the
visor helmets `MotoHelmet_ColorBase`, `DarkMotoHelmet_ColorBase`, `DirtBikeHelmet_ColorBase`,
`GorkaHelmetComplete`, `GreatHelm`, `WeldingMask` → `MASK`.

`[DESIGN]` A custom goggle/visor/face-cover that should block sand must (1) inherit one of those bases
or override `GetCachedEquipmentCategory()`, and (2) declare `class Protection { dust_particle_eyes=…;
dust_particle_breath=…; }`. A helmet with a visor that inherits plain `HelmetBase` reports `NONE` and
protects nothing. Static reading; not tested in-game here.

## ItemBaseType — hide selections, wetness, heat isolation

`ItemBaseType` (`P:\scripts\4_world\entities\itembasetype.c:4`, `: InventoryItemType`) is new in 1.30
(no `itembasetype.c` in 1.29). The **config keys** `headSelectionsToHide` and
`hideSelectionsByinventorySlot` already existed in 1.29 (1.29:
`O:\…\scripts\4_world\entities\itembase.c:178-187` parsed them on the instance). 1.30 moves the parse
onto the type:

```c
// [EXACT] P:\scripts\4_world\entities\itembasetype.c:105
		if (ConfigIsExisting("headSelectionsToHide"))
		{
			m_HeadHidingSelections = new TStringArray();
			ConfigGetTextArray("headSelectionsToHide", m_HeadHidingSelections);
		}
		
		m_HideSelectionsBySlot = false;
		if (ConfigIsExisting("hideSelectionsByinventorySlot"))
			m_HideSelectionsBySlot = ConfigGetBool("hideSelectionsByinventorySlot");
```

`varWetInit` / `varWetMin` / `varWetMax` are read at `itembasetype.c:69-71`, `heatIsolation` at `:84`,
`temperaturePerQuantityWeight` at `:118-119`, `EnvironmentWetnessIncrements` Soaking/Drying at
`:121-137`.

Instance accessors read the type:

```c
// [EXACT] P:\scripts\4_world\entities\itembase.c:4522
	array<string> GetHeadHidingSelection()
	{
		return ItemBaseType.Cast(GetEntityType()).m_HeadHidingSelections;
	}
	
	bool HidesSelectionBySlot()
	{
		return ItemBaseType.Cast(GetEntityType()).m_HideSelectionsBySlot;
	}
```

`InitItemVariables` copies `m_VarWet` from `itemBaseType.m_VarWetInit` (`itembase.c:165`);
`SetWet` clamps to `GetWetMin()..GetWetMax()` (`:3692-3702`), both read from the type (`:3726-3734`).

**Porting break (compile):** the 1.29 instance fields `ItemBase.m_HeadHidingSelections`,
`m_HideSelectionsBySlot`, `m_VarWetInit`, `m_VarWetMin`, `m_VarWetMax`, `m_TemperaturePerQuantityWeight`,
`m_VarQuantityInit/Min/Max`, `m_VarStackMax`, `m_QuickBarBonus`, `m_CleannessInit/Min/Max` and others
are gone (api-delta §5). A 1.29 `modded class` / subclass that assigned e.g. `m_VarWetMax = 1;` in its
constructor no longer compiles: set the value in `config.cpp` (`varWetMax = 1;`) instead — it is a
per-type value now.

**Porting break (silent): heat isolation.** `ItemBase.GetHeatIsolation()` returns
`ItemBaseType.m_HeatIsolation` (`itembase.c:3657-3660`), i.e. the config `heatIsolation`.
`GetHeatIsolationInit()` is `[Obsolete("1.30: No replacement")]` (`:4942-4946`) and nothing calls it.
(Before 1.30: `InitItemVariables` did `m_HeatIsolation = GetHeatIsolationInit();` and
`GetHeatIsolation()` returned that instance field — 1.29: `O:\…\itembase.c:231`, `:3674-3682`.) A 1.29
mod that overrode `GetHeatIsolationInit()` or wrote `m_HeatIsolation` still compiles but no longer
changes insulation; override `GetHeatIsolation()` or set `heatIsolation` in config.

`GetDryingIncrement(string)` / `GetSoakingIncrement(string)` are `[Obsolete]`
(`itembase.c:3672`, `:3682`); 1.30 adds overloads taking `EEnvironmentDryingCategories` /
`EEnvironmentSoakingCategories` (`:3662-3670`) that read the type's cached maps.

### `varWetMax` default dropped to 0.049

1.30: `Inventory_Base` sets `varWetInit=0; varWetMin=0.049; varWetMax=0.049;`
(`P:\DZ\data\config.cpp:3229-3231`). Before 1.30 `varWetMax` was `0.249` (1.29:
`O:\…\DZ\data\config.cpp:3214`). `Clothing_Base` (`P:\DZ\data\config.cpp:3442`) and the per-category
`class Clothing: Clothing_Base` stubs declare no `varWetMax`, so any garment that does not declare its
own inherits 0.049.

Impact (static reading of `P:\scripts`, not measured in-game): with min = max = 0.049, `SetWet` clamps
every value to 0.049; `GetWetLevelInternal` reports `DRY` below `STATE_DAMP = 0.05`
(`P:\scripts\3_game\entities\entityai.c:2201-2206`, `3_game\constants.c:933-937`). Such an item never
reads damp/wet, keeps its dry heat isolation in rain, and `ActionWetClothingInHandsWater` refuses it
because `GetWet() < GetWetMax()` is false (`…\actions\continuous\actionwetclothinginhandswater.c:20-26`).
Most vanilla clothing declares its own value (headgear `0.249`, `Bandana` `1`, …); vanilla items that
newly declare one in 1.30: `Rag`, `BandageDressing`, `ChernarusMap` (1), `Broom`, `Firewood`, sticks,
`Matchbox` (0.49), `Bark_ColorBase` (0.249); `EyeMask_ColorBase`, `GasMask_Filter`,
`GasMask_Filter_Improvised`, `Candycane_Colorbase` **dropped** theirs (now 0.049). **Every custom
garment should declare `varWetMax` explicitly.**

### Dual-slot headgear (Bandana / Shemag)

```cpp
// [EXACT] P:\DZ\characters\headgear\config.cpp:2955 (inside Bandana_ColorBase, class at :2941)
		inventorySlot[]=
		{
			"Headgear",
			"Mask"
		};
		rotationFlags=16;
		weight=120;
		itemSize[]={3,1};
		ragQuantity=1;
		varWetMax=1;
		heatIsolation=0.40000001;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		headSelectionsToHide[]=
		{
			"Clipping_BandanaHead",
			"Clipping_BandanaFace"
		};
		hideSelectionsByinventorySlot=1;
```

`Shemag_ColorBase` (`:3168`) uses the same hide-by-slot pattern (`:3195-3200`). `NBCHoodBase` sets
`headSelectionsToHide[]` (`:7512`) without `hideSelectionsByinventorySlot`. 1.30 also changed
`MilitaryBeret_ColorBase.headSelectionsToHide[]` from `{"Clipping_MilitaryBeret_xx"}` to
`{"Clipping_prison_cap"}` (config-delta).

Default soaking/drying increments (inherited unless a class overrides
`class EnvironmentWetnessIncrements`) sit on `Inventory_Base`:

```cpp
// [EXACT] P:\DZ\data\config.cpp:3191
		class EnvironmentWetnessIncrements
		{
			class Soaking
			{
				parentWithLiquid=1;
				wetParent=0.0049999999;
			};
			class Drying
			{
				player=0.00029;
				ground=0.00011;
				playerHeatSource=0.0035000001;
				groundHeatSource=0.0018;
			};
		};
```

## Headgear vs sun (heat comfort)

1.30 `Environment` adds sun exposure: outdoors and not in shadow, the sun term of the player's
temperature is multiplied by `HeadGearProtectionAgainstSun()` (`P:\scripts\4_world\classes\environment\environment.c:868-871`),
which returns `HC_EXPOSED_HEAD_TO_SUN = 1.2` (`:79`) bare-headed, or
`heatIsolation(current) * 1.2 + 0.4` from a clothing attachment whose first slot is `Headgear`
(`:2037-2065`, via `MiscGameplayFunctions.GetCurrentItemHeatIsolation`). `heatIsolation` on headgear
therefore matters in hot maps; vanilla `NorseHelm` went `0` → `0.1` (config-delta). The player-side
`ThermalBiasHandler` (`P:\scripts\4_world\classes\thermalbiashandler.c`) is medical/player state, not a
clothing API — see wash head below. `[UNVERIFIED]` in-game magnitude.

## Shoe `itemSize` (width 3, with exceptions)

Digest L: "all shoes and boots rescaled to width 3". **False as a universal.**
`LeatherShoes_ColorBase` is still `{4,2}` (`P:\DZ\characters\shoes\config.cpp:2669`).
`NBCBootsBase` is `{2,2}` (`:3207`); `FeetCover_Improvised` is `{2,2}` (`:3473`).

1.29 width-4 → 1.30 width-3 (height often shrank too). Line = the `itemSize[]` line in
`P:\DZ\characters\shoes\config.cpp` (the 1.29 file has the same line ±1):

| Class | 1.29 | 1.30 | 1.30 line |
|---|---|---|---|
| `AthleticShoes_ColorBase` | `{4,2}` | `{3,2}` | 800 |
| `HikingBoots_ColorBase` | `{4,4}` | `{3,3}` | 988 |
| `HikingBootsLow_ColorBase` | `{4,3}` | `{3,2}` | 1149 |
| `Wellies_ColorBase` | `{4,4}` | `{3,4}` | 1330 |
| `WorkingBoots_ColorBase` | `{4,3}` | `{3,3}` | 1515 |
| `JungleBoots_ColorBase` | `{4,4}` | `{3,3}` | 1706 |
| `DressShoes_ColorBase` | `{4,2}` | `{3,2}` | 1899 |
| `MilitaryBoots_ColorBase` | `{4,4}` | `{3,3}` | 2091 |
| `CombatBoots_ColorBase` | `{4,4}` | `{3,3}` | 2287 |
| `JoggingShoes_ColorBase` | `{4,2}` | `{3,2}` | 2480 |
| `Sneakers_ColorBase` | `{4,2}` | `{3,2}` | 3042 |
| `Ballerinas_ColorBase` | `{4,2}` | `{3,2}` | 3325 |
| `TTSKOBoots` | `{4,4}` | `{3,3}` | 3585 |
| `MedievalBoots` | `{4,3}` | `{3,4}` | 3669 |
| `TraditionalBoots_ColorBase` | `{4,4}` | `{3,4}` | 3817 |
| `ColdOperationBoots_ColorBase` | `{4,4}` | `{3,4}` | 3977 |

## Other clothing config deltas worth knowing (config-delta, spot-checked)

- Helmet `hitpoints`: `BallisticHelmet_ColorBase` 75→85, `GorkaHelmet` 100→120, `Mich2001Helmet` 80→100,
  `Ssh68Helmet` 85→100. `GorkaEJacket` / `GorkaPants` Infected+Melee armor damage → 0.9.
- `ragQuantity = 3` added to `M65Jacket`, `OMKJacket`, `OMKPants`, `USMCJacket`, `USMCPants`, `WoolCoat`
  colour bases. `Shirt_ColorBase.itemsCargoSize[]` `{3,3}` → `{4,3}`. `PolicePants.quickBarBonus` 0→1.
  `NioshFaceMask` / `SurgicalMask` weight 340 → 10 / 4.

## Waterskin (was WaterPouch)

1.30: the config classnames are `Waterskin_ColorBase` / `Waterskin_Natural`
(`P:\DZ\gear\drinks\config.cpp:474`, `:595`). AnimEvents still use `WaterPouch_*` sound class names.
(Before 1.30: `class WaterPouch_ColorBase: Bottle_Base` / `class WaterPouch_Natural:
WaterPouch_ColorBase` — 1.29: `O:\…\DZ\gear\drinks\config.cpp:474`, `:590`.)

Script: `P:\scripts\4_world\entities\itembase\gear\drinks\waterpouch_colorbase.c` is now the empty stub
`class WaterPouch_ColorBase: Bottle_Base{};`. Behaviour lives in `waterskin_colorbase.c`.

```cpp
// [EXACT] P:\DZ\gear\drinks\config.cpp:474 (blank lines removed)
	class Waterskin_ColorBase: Bottle_Base
	{
		displayName="$STR_CfgVehicles_Waterskin_ColorBase0";
		descriptionShort="$STR_CfgVehicles_Waterskin_ColorBase1";
		model="\dz\gear\drinks\waterskin.p3d";
		lootCategory="Crafted";
		hiddenSelections[]=
		{
			"camoground"
		};
		weight=250;
		itemSize[]={2,4};
		repairableWithKits[]={3};
		repairCosts[]={25};
		destroyOnEmpty=0;
		varQuantityDestroyOnMin=0;
		varLiquidTypeInit=512;
		liquidContainerType="1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048 + 4096 + 8192 + 16384 + 32768 + 65536  + 131072 + 262144 + 524288 + 2097152 + 4194304 - (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256) - 32768";
		varTemperatureFreezePoint=-200;
		varTemperatureThawPoint=-200;
		varTemperatureMax=120;
		varQuantityInit=0;
		varQuantityMin=0;
		varQuantityMax=1250;
		temperaturePerQuantityWeight=4;
```

Craft (`P:\scripts\4_world\classes\recipes\recipes\craftwaterskin.c:26-44`): ingredient 0
`TannedLeather` with `m_IngredientAddQuantity[0] = -2`; ingredient 1 `LeatherSewingKit` with
`m_IngredientAddQuantity[1] = -10`; result `Waterskin_Natural`.

De-craft (`decraftwaterskin.c:26-90`): ingredient 0 `Waterskin_ColorBase` destroyed; ingredient 1 a long
knife/axe list including `Jambiya`, `Scimitar`, `DonerKnife`, `BrickTrowel`, `Akinaka`;
`m_IngredientAddHealth[1] = -12`; result 1 `TannedLeather`. `CanDo` is `return ingredients[0].IsEmpty();`
(`:90`). Note: `Jambiya`, `Scimitar`, `DonerKnife`, `Akinaka` have scripts
(`P:\scripts\4_world\entities\itembase\jambiya.c` etc.) and Badlands DLC configs
(`P:\DZ\weapons_takistan\melee\blade\config.cpp:17,156,317,584`; on `P:\` since the 1.30.164200 unpack).

`[DESIGN]` persistence alias if a 1.29 world still stores `WaterPouch_Natural`:

```cpp
class Waterskin_Natural;
class WaterPouch_Natural: Waterskin_Natural { scope = 1; };
```

That alias is not in vanilla 1.30. `[UNVERIFIED]` that the engine restores a stored
`WaterPouch_Natural` through such an alias — test on a copy of the storage.

## CraftArmbandFlag

1.30: `CanDo` only rejects a flag that is already an attachment; the tool list gained `Jambiya`,
`Scimitar`, `DonerKnife`, `Akinaka`. (Before 1.30: `return ingredients[0].IsEmpty();` — 1.29:
`O:\…\scripts\4_world\classes\recipes\recipes\craftarmbandflag.c:99-102`.)

```c
// [EXACT] P:\scripts\4_world\classes\recipes\recipes\craftarmbandflag.c:103
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		return !ingredients[0].GetInventory().IsAttachment();
	}
```

## Wash head / wet clothing / thermal bias

New continuous actions (`P:\scripts\4_world\classes\useractionscomponent\actions\continuous\`:
`actionwashheaditemcontinuous.c`, `actionwashheadwater .c` (sic, space in the file name),
`actionwashheadwell.c`, `actionwashheadwettingclothesbase.c`, `actionwetclothingfromcontainer.c`,
`actionwetclothinginhands{base,water,well}.c`). Wetness is added per clothing slot from
`ActionWashHeadWettingClothesBase.GetWetnessMap` (`actionwashheadwettingclothesbase.c:6-43`; from a
container: Headgear/Mask 0.24, Body 0.12, Vest 0.08, Legs/Armband/Hips 0.06, Gloves/Feet/Back 0.04;
from a water surface: Mask/Gloves 0.24, Headgear/Body 0.12, rest as above) and applied with
`SetWet(Clamp(GetWet() + add, 0, 1))` (`ApplyWetnessToClothes`, `:72-89`) — so `varWetMax` still caps it. Wash-head also
reduces `eAgents.EYES_IRRITATION` and calls `GetThermalBiasHandler().Add(-THERMAL_BIAS_WASH_HEAD_DECREMENT)`
+ `SetTemporaryResistance(...)` (`:53-55`).

```c
// [EXACT] P:\scripts\3_game\playerconstants.c:203
	static const float THERMAL_BIAS_WASH_HEAD_DECREMENT = 0.175;
	static const float THERMAL_BIAS_WASH_HEAD_TEMPORARY_RESISTANCE_TIME = 30.0;
```

`ActionWetClothingInHandsWater.ActionCondition` requires `item.IsClothing()` and
`item.GetWet() < item.GetWetMax()` (`actionwetclothinginhandswater.c:20-26`).

`[CHANGELOG 1.30]` known issue "Bottles can still be used to Wash Head while Frozen". Still true in
`P:\scripts` on 1.30.164078: `ActionWashHeadItemContinuous.ActionCondition`
(`actionwashheaditemcontinuous.c:32-39`) only rejects `LIQUID_GASOLINE`; it does not call `GetIsFrozen()`.

Heat-stroke coupling beyond these constants is out of scope here (medical lane). `[DESIGN]`: do not
treat wash-head as a full heat-stroke API.

## New garments: Badlands DLC clothing

1.30.164078 adds clothing scripts under `P:\scripts\4_world\entities\itembase\clothing\` (class names
as declared — the Exp draft's `Kameez_ColorBase` / `Sandals_ColorBase` / `Shalwar_ColorBase` were wrong):

| File | Classes |
|---|---|
| `obrcap.c` / `obrjacket.c` / `obrpants.c` | `OBRCap_ColorBase` (`HeadGear_Base`), `OBRJacket_ColorBase` (`Top_Base`), `OBRPants_ColorBase` (`Pants_Base`) + `_Beige` / `_Olive` |
| `fnojacket.c` / `fnopants.c` | `FNOJacket : Top_Base`, `FNOPants : Pants_Base` |
| `kameez_colorbase.c` | `KameezShirt_ColorBase : Top_Base` (+ `_Beige`, `_Black`, …) |
| `mensshalwarpants_colorbase.c` / `womensshalwarpants_colorbase.c` | `MensShalwarPants_ColorBase`, `WomensShalwarPants_ColorBase` (`Pants_Base`) |
| `turbanhat_colorbase .c` (sic) / `pakolhat_colorbase.c` | `TurbanHat_ColorBase`, `PakolHat_ColorBase` (`HeadGear_Base`) |
| `sandals_colorbase.c` / `traditionalsandal_colorbase.c` | `SandalsShoes_ColorBase`, `TraditionalSandalShoes_ColorBase` (`Shoes_Base`) |
| `dustgoggles_colorbase.c` | `DustGoggles_ColorBase : Glasses_Base` (→ `EYEWEAR` category) |
| `chestrigvest_colorbase.c`, `linenvest_colorbase.c`, `robevest_colorbase.c` | `ChestRigVest_ColorBase`, `LinenVest_ColorBase`, `RobeVest_ColorBase` (`Vest_Base`) |
| `shortsleeveshirt_colorbase.c`, `policeshortsleeveshirt_colorbase.c` | `ShortSleeveShirt_ColorBase`, `PoliceShortSleeveShirt_ColorBase` (`Top_Base`) |
| `leatherhippack_colorbase.c` | `LeatherHipPack_ColorBase : Belt_Base` |

Several have in-hands IK registrations (`dayzplayercfgbase.c:1336` `KameezShirt_ColorBase`, `:1342`
`OBRJacket_ColorBase`, `:1449` `DustGoggles_ColorBase`, `:1498` `SandalsShoes_Colorbase`), and the
Nasdara world script yields `SandalsShoes_*` (`P:\scripts\4_world\classes\worlds\nasdara.c:112-115`).
Their configs and models ship in the Badlands DLC data, `P:\DZ\characters_takistan\` since the 1.30.164200
unpack (corrected 2026-10-21; the 2026-10-15 grep found nothing because `P:\` held no DLC data): e.g.
`KameezShirt_ColorBase` `tops\config.cpp:651`, `ShortSleeveShirt_ColorBase` `:929`,
`PoliceShortSleeveShirt_ColorBase` `:1168`, `OBRJacket_ColorBase` `:1319`, `DustGoggles_ColorBase`
`glasses\config.cpp:25`, `SandalsShoes_ColorBase` `shoes\config.cpp:786`, `LeatherHipPack_Colorbase` (sic)
`belts\config.cpp:175` (all `: Clothing`). Read isolation, cargo and `ClothingTypes` there before
inheriting; whether a non-Badlands server or a player without the DLC can use them was not checked.

`LeatherHipPack_ColorBase` blocks `CanPutInCargo` unless empty; `CanReceiveAttachment` refuses non-`Man`
parents.

## What 1.30 does **not** change (still the SKILL.md contract)

- `config.cpp` packed as text; `ClothingTypes` male/female; camo `hiddenSelections`.
- Canonical worn frame (−Z chest, +X anatomical left, +Y up).
- Skeleton name `DayzTemporarySkeleton`, 159-pair `model.cfg.template`. Static check on 1.30.164078:
  the vanilla worn ODOLs `P:\DZ\characters\tops\chainmail_m.p3d` and `hoodie_m.p3d` still contain the
  string `DayzTemporarySkeleton`.
- In-game gate: spawn → equip → move.
