// Сгенерировано tools/gen_config.py. Правки вносить в генератор и перезапускать.
// Параметры предметов перенесены из BRDK_GoldRush. Старые классы BRDK_* — наследники новых VB_MR_*.
class CfgPatches
{
	class VB_MiningResources
	{
		units[] = {"VB_MR_Gold","BRDK_Gold","VB_MR_Gold_Sand","BRDK_Gold_Sand","VB_MR_Gold_Bar","BRDK_BigGold","VB_MR_Silver","BRDK_Silver","VB_MR_Silver_Sand","BRDK_Silver_Sand","VB_MR_Silver_Bar","BRDK_BigSilver","VB_MR_Bronze","BRDK_Bronze","VB_MR_Bronze_Sand","BRDK_Bronze_Sand","VB_MR_Bronze_Bar","BRDK_BigBronze","VB_MR_Iron","BRDK_Iron","VB_MR_Iron_Sand","BRDK_Iron_Sand","VB_MR_Iron_Bar","BRDK_BigIron","VB_MR_Titanium","BRDK_Titanium","VB_MR_Titanium_Sand","BRDK_Titanium_Sand","VB_MR_Titanium_Bar","BRDK_BigTitanium","VB_MR_Coal","BRDK_Coal","VB_MR_Coal_Sand","BRDK_Coal_Sand","VB_MR_Coal_Bar","BRDK_BigCoal","VB_MR_Rubin","BRDK_Rubin","VB_MR_Rubin_Sand","BRDK_Rubin_Sand","VB_MR_Rubin_Bar","BRDK_BigRubin","VB_MR_Nephrite","BRDK_Nephrite","VB_MR_Nephrite_Sand","BRDK_Nephrite_Sand","VB_MR_Nephrite_Bar","BRDK_BigNephrite","VB_MR_Sulfur","BRDK_Sulfur","VB_MR_Sulfur_Sand","BRDK_Sulfur_Sand","VB_MR_Sulfur_Bar","BRDK_BigSulfur","VB_MR_Rubin_Gem","VB_MR_Nephrite_Gem","VB_MR_Press_Kit","BRDK_Press_kit","VB_MR_Press","BRDK_Stone_Press","VB_MR_MeltingBowl","BRDK_MeltingBowl"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Gear_Cooking"};
	};
};
class CfgMods
{
	class VB_MiningResources
	{
		dir = "VB_MiningResources";
		name = "VB_MiningResources";
		author = "DayZavr";
		type = "mod";
		dependencies[] = {"World"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_MiningResources/scripts/4_World"};
			};
		};
	};
};
// Имена слотов оставлены старыми: по ним в сохранении лежат вещи в прессах и чашах игроков
class CfgSlots
{
	class Slot_BRDK_STONES
	{
		name = "BRDK_STONES";
		displayName = "$STR_VB_MR_SLOT_STONES";
		ghostIcon = "set:dayz_inventory image:stones";
	};
	class Slot_BRDK_METALS
	{
		name = "BRDK_METALS";
		displayName = "$STR_VB_MR_SLOT_METALS";
		ghostIcon = "set:dayz_inventory image:stones";
	};
};
class CfgVehicles
{
	class HouseNoDestruct;
	class Inventory_Base;
	class Cauldron;

	// ---------------------------------------------------------------- жилы (объекты карты)
	class VB_MR_Vein_Base: HouseNoDestruct
	{
		scope = 0;
	};
	class VB_MR_Gold_Vein: VB_MR_Vein_Base
	{
		scope = 1;
		model = "VB_MiningResources\m\stones\GoldStone.p3d";
		physLayer = "item_large";
	};
	class VB_MR_Silver_Vein: VB_MR_Vein_Base
	{
		scope = 1;
		model = "VB_MiningResources\m\stones\SilverStone.p3d";
		physLayer = "item_large";
	};
	class VB_MR_Bronze_Vein: VB_MR_Vein_Base
	{
		scope = 1;
		model = "VB_MiningResources\m\stones\BronzeStone.p3d";
		physLayer = "item_large";
	};
	class VB_MR_Iron_Vein: VB_MR_Vein_Base
	{
		scope = 1;
		model = "VB_MiningResources\m\stones\IronStone.p3d";
		physLayer = "item_large";
	};
	class VB_MR_Titanium_Vein: VB_MR_Vein_Base
	{
		scope = 1;
		model = "VB_MiningResources\m\stones\TitaniumStone.p3d";
		physLayer = "item_large";
	};
	class VB_MR_Coal_Vein: VB_MR_Vein_Base
	{
		scope = 1;
		model = "VB_MiningResources\m\stones\CoalStone.p3d";
		physLayer = "item_large";
	};
	class VB_MR_Rubin_Vein: VB_MR_Vein_Base
	{
		scope = 1;
		model = "VB_MiningResources\m\stones\RubinStone.p3d";
		physLayer = "item_large";
	};
	class VB_MR_Nephrite_Vein: VB_MR_Vein_Base
	{
		scope = 1;
		model = "VB_MiningResources\m\stones\NephriteStone.p3d";
		physLayer = "item_large";
	};
	class VB_MR_Sulfur_Vein: VB_MR_Vein_Base
	{
		scope = 1;
		model = "VB_MiningResources\m\stones\SulfurStone.p3d";
		physLayer = "item_large";
	};
	// старые жилы (стоят в .dze сервера 18)
	class BRDK_Gold_Ore: VB_MR_Gold_Vein
	{
		scope = 1;
	};
	class BRDK_Silver_Ore: VB_MR_Silver_Vein
	{
		scope = 1;
	};
	class BRDK_Bronze_Ore: VB_MR_Bronze_Vein
	{
		scope = 1;
	};
	class BRDK_Iron_Ore: VB_MR_Iron_Vein
	{
		scope = 1;
	};
	class BRDK_Titanium_Ore: VB_MR_Titanium_Vein
	{
		scope = 1;
	};
	class BRDK_Coal_Ore: VB_MR_Coal_Vein
	{
		scope = 1;
	};
	class BRDK_Rubin_Ore: VB_MR_Rubin_Vein
	{
		scope = 1;
	};
	class BRDK_Nephrite_Ore: VB_MR_Nephrite_Vein
	{
		scope = 1;
	};
	class BRDK_Sulfur_Ore: VB_MR_Sulfur_Vein
	{
		scope = 1;
	};

	// ---------------------------------------------------------------- базы ресурсов
	class VB_MR_Nugget_Base: Inventory_Base
	{
		scope = 0;
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 5;
		varQuantityDestroyOnMin = 1;
		varTemperatureMax = 200;
		canBeSplit = 1;
		inventorySlot[] += {"BRDK_METALS"};
	};
	class VB_MR_StoneNugget_Base: Inventory_Base
	{
		scope = 0;
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 5;
		varQuantityDestroyOnMin = 1;
		canBeSplit = 1;
		inventorySlot[] += {"BRDK_STONES"};
	};
	class VB_MR_Sand_Base: Inventory_Base
	{
		scope = 0;
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 10;
		varQuantityDestroyOnMin = 1;
		canBeSplit = 1;
		inventorySlot[] += {"BRDK_METALS"};
	};
	class VB_MR_StoneSand_Base: Inventory_Base
	{
		scope = 0;
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 10;
		varQuantityDestroyOnMin = 1;
		canBeSplit = 1;
		inventorySlot[] += {"BRDK_STONES"};
	};
	class VB_MR_MetalBar_Base: Inventory_Base
	{
		scope = 0;
	};
	class VB_MR_StoneBar_Base: Inventory_Base
	{
		scope = 0;
	};
	class VB_MR_Gem_Base: Inventory_Base
	{
		scope = 0;
	};

	// ---------------------------------------------------------------- ресурсы: новый класс и старый
	class VB_MR_Gold: VB_MR_Nugget_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Gold_N";
		descriptionShort = "$STR_VB_MR_Gold_D";
		model = "VB_MiningResources\data\items\vb_mr_nugget.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_gold_nugget_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_gold_nugget.rvmat"};
		animClass = "NoFireClass";
		weight = 500;
		itemSize[] = {2,2};
		rotationFlags = 17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Gold: VB_MR_Gold
	{
		scope = 2;
		displayName = "$STR_VB_MR_Gold_NO";
		descriptionShort = "$STR_VB_MR_Gold_DO";
		model = "VB_MiningResources\m\metall\SmalGold.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Gold_Sand: VB_MR_Sand_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Gold_Sand_N";
		descriptionShort = "$STR_VB_MR_Gold_Sand_D";
		model = "VB_MiningResources\data\items\vb_mr_sand.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_gold_sand_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_gold_sand.rvmat"};
		animClass = "Compass";
		itemSize[] = {1,1};
		weight = 190;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Gold_Sand: VB_MR_Gold_Sand
	{
		scope = 2;
		displayName = "$STR_VB_MR_Gold_Sand_NO";
		descriptionShort = "$STR_VB_MR_Gold_Sand_DO";
		model = "VB_MiningResources\m\metall\SandGold.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Gold_Bar: VB_MR_MetalBar_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Gold_Bar_N";
		descriptionShort = "$STR_VB_MR_Gold_Bar_D";
		model = "VB_MiningResources\data\items\vb_mr_ingot.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_gold_ingot_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_gold_ingot.rvmat"};
		animClass = "NoFireClass";
		weight = 5500;
		itemSize[] = {3,3};
		isMeleeWeapon = 1;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_BigGold: VB_MR_Gold_Bar
	{
		scope = 2;
		displayName = "$STR_VB_MR_Gold_Bar_NO";
		descriptionShort = "$STR_VB_MR_Gold_Bar_DO";
		model = "VB_MiningResources\m\metall\GoldBigStone.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Silver: VB_MR_Nugget_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Silver_N";
		descriptionShort = "$STR_VB_MR_Silver_D";
		model = "VB_MiningResources\data\items\vb_mr_nugget.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_silver_nugget_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_silver_nugget.rvmat"};
		animClass = "NoFireClass";
		weight = 500;
		itemSize[] = {2,2};
		rotationFlags = 17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Silver: VB_MR_Silver
	{
		scope = 2;
		displayName = "$STR_VB_MR_Silver_NO";
		descriptionShort = "$STR_VB_MR_Silver_DO";
		model = "VB_MiningResources\m\metall\SmalSilver.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Silver_Sand: VB_MR_Sand_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Silver_Sand_N";
		descriptionShort = "$STR_VB_MR_Silver_Sand_D";
		model = "VB_MiningResources\data\items\vb_mr_sand.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_silver_sand_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_silver_sand.rvmat"};
		animClass = "Compass";
		itemSize[] = {1,1};
		weight = 190;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Silver_Sand: VB_MR_Silver_Sand
	{
		scope = 2;
		displayName = "$STR_VB_MR_Silver_Sand_NO";
		descriptionShort = "$STR_VB_MR_Silver_Sand_DO";
		model = "VB_MiningResources\m\metall\SandSilver.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Silver_Bar: VB_MR_MetalBar_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Silver_Bar_N";
		descriptionShort = "$STR_VB_MR_Silver_Bar_D";
		model = "VB_MiningResources\data\items\vb_mr_ingot.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_silver_ingot_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_silver_ingot.rvmat"};
		animClass = "NoFireClass";
		weight = 5500;
		itemSize[] = {3,3};
		isMeleeWeapon = 1;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_BigSilver: VB_MR_Silver_Bar
	{
		scope = 2;
		displayName = "$STR_VB_MR_Silver_Bar_NO";
		descriptionShort = "$STR_VB_MR_Silver_Bar_DO";
		model = "VB_MiningResources\m\metall\SilverBigStone.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Bronze: VB_MR_Nugget_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Bronze_N";
		descriptionShort = "$STR_VB_MR_Bronze_D";
		model = "VB_MiningResources\data\items\vb_mr_nugget.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_bronze_nugget_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_bronze_nugget.rvmat"};
		animClass = "NoFireClass";
		weight = 500;
		itemSize[] = {2,2};
		rotationFlags = 17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Bronze: VB_MR_Bronze
	{
		scope = 2;
		displayName = "$STR_VB_MR_Bronze_NO";
		descriptionShort = "$STR_VB_MR_Bronze_DO";
		model = "VB_MiningResources\m\metall\SmalBronze.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Bronze_Sand: VB_MR_Sand_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Bronze_Sand_N";
		descriptionShort = "$STR_VB_MR_Bronze_Sand_D";
		model = "VB_MiningResources\data\items\vb_mr_sand.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_bronze_sand_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_bronze_sand.rvmat"};
		animClass = "Compass";
		itemSize[] = {1,1};
		weight = 190;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Bronze_Sand: VB_MR_Bronze_Sand
	{
		scope = 2;
		displayName = "$STR_VB_MR_Bronze_Sand_NO";
		descriptionShort = "$STR_VB_MR_Bronze_Sand_DO";
		model = "VB_MiningResources\m\metall\SandBronze.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Bronze_Bar: VB_MR_MetalBar_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Bronze_Bar_N";
		descriptionShort = "$STR_VB_MR_Bronze_Bar_D";
		model = "VB_MiningResources\data\items\vb_mr_ingot.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_bronze_ingot_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_bronze_ingot.rvmat"};
		animClass = "NoFireClass";
		weight = 5500;
		itemSize[] = {3,3};
		isMeleeWeapon = 1;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_BigBronze: VB_MR_Bronze_Bar
	{
		scope = 2;
		displayName = "$STR_VB_MR_Bronze_Bar_NO";
		descriptionShort = "$STR_VB_MR_Bronze_Bar_DO";
		model = "VB_MiningResources\m\metall\BronzeBigStone.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Iron: VB_MR_Nugget_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Iron_N";
		descriptionShort = "$STR_VB_MR_Iron_D";
		model = "VB_MiningResources\data\items\vb_mr_nugget.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_iron_nugget_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_iron_nugget.rvmat"};
		animClass = "NoFireClass";
		weight = 500;
		itemSize[] = {2,2};
		rotationFlags = 17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Iron: VB_MR_Iron
	{
		scope = 2;
		displayName = "$STR_VB_MR_Iron_NO";
		descriptionShort = "$STR_VB_MR_Iron_DO";
		model = "VB_MiningResources\m\metall\SmalIron.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Iron_Sand: VB_MR_Sand_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Iron_Sand_N";
		descriptionShort = "$STR_VB_MR_Iron_Sand_D";
		model = "VB_MiningResources\data\items\vb_mr_sand.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_iron_sand_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_iron_sand.rvmat"};
		animClass = "Compass";
		itemSize[] = {1,1};
		weight = 190;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Iron_Sand: VB_MR_Iron_Sand
	{
		scope = 2;
		displayName = "$STR_VB_MR_Iron_Sand_NO";
		descriptionShort = "$STR_VB_MR_Iron_Sand_DO";
		model = "VB_MiningResources\m\metall\SandIron.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Iron_Bar: VB_MR_MetalBar_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Iron_Bar_N";
		descriptionShort = "$STR_VB_MR_Iron_Bar_D";
		model = "VB_MiningResources\data\items\vb_mr_ingot.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_iron_ingot_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_iron_ingot.rvmat"};
		animClass = "NoFireClass";
		weight = 5500;
		itemSize[] = {3,3};
		isMeleeWeapon = 1;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_BigIron: VB_MR_Iron_Bar
	{
		scope = 2;
		displayName = "$STR_VB_MR_Iron_Bar_NO";
		descriptionShort = "$STR_VB_MR_Iron_Bar_DO";
		model = "VB_MiningResources\m\metall\IronBigStone.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Titanium: VB_MR_Nugget_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Titanium_N";
		descriptionShort = "$STR_VB_MR_Titanium_D";
		model = "VB_MiningResources\data\items\vb_mr_nugget.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_titanium_nugget_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_titanium_nugget.rvmat"};
		animClass = "NoFireClass";
		weight = 500;
		itemSize[] = {2,2};
		rotationFlags = 17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Titanium: VB_MR_Titanium
	{
		scope = 2;
		displayName = "$STR_VB_MR_Titanium_NO";
		descriptionShort = "$STR_VB_MR_Titanium_DO";
		model = "VB_MiningResources\m\metall\SmalTitanium.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Titanium_Sand: VB_MR_Sand_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Titanium_Sand_N";
		descriptionShort = "$STR_VB_MR_Titanium_Sand_D";
		model = "VB_MiningResources\data\items\vb_mr_sand.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_titanium_sand_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_titanium_sand.rvmat"};
		animClass = "Compass";
		itemSize[] = {1,1};
		weight = 190;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Titanium_Sand: VB_MR_Titanium_Sand
	{
		scope = 2;
		displayName = "$STR_VB_MR_Titanium_Sand_NO";
		descriptionShort = "$STR_VB_MR_Titanium_Sand_DO";
		model = "VB_MiningResources\m\metall\SandTitanium.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Titanium_Bar: VB_MR_MetalBar_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Titanium_Bar_N";
		descriptionShort = "$STR_VB_MR_Titanium_Bar_D";
		model = "VB_MiningResources\data\items\vb_mr_ingot.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_titanium_ingot_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_titanium_ingot.rvmat"};
		animClass = "NoFireClass";
		weight = 5500;
		itemSize[] = {3,3};
		isMeleeWeapon = 1;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_BigTitanium: VB_MR_Titanium_Bar
	{
		scope = 2;
		displayName = "$STR_VB_MR_Titanium_Bar_NO";
		descriptionShort = "$STR_VB_MR_Titanium_Bar_DO";
		model = "VB_MiningResources\m\metall\TitaniumBigStone.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Coal: VB_MR_StoneNugget_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Coal_N";
		descriptionShort = "$STR_VB_MR_Coal_D";
		model = "VB_MiningResources\data\items\vb_mr_chunk.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_coal_chunk_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_coal_chunk.rvmat"};
		animClass = "NoFireClass";
		weight = 500;
		itemSize[] = {2,2};
		rotationFlags = 17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Coal: VB_MR_Coal
	{
		scope = 2;
		displayName = "$STR_VB_MR_Coal_NO";
		descriptionShort = "$STR_VB_MR_Coal_DO";
		model = "VB_MiningResources\m\Kamens\SmalCoal.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Coal_Sand: VB_MR_StoneSand_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Coal_Sand_N";
		descriptionShort = "$STR_VB_MR_Coal_Sand_D";
		model = "VB_MiningResources\data\items\vb_mr_sand.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_coal_sand_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_coal_sand.rvmat"};
		animClass = "Compass";
		itemSize[] = {1,1};
		weight = 190;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Coal_Sand: VB_MR_Coal_Sand
	{
		scope = 2;
		displayName = "$STR_VB_MR_Coal_Sand_NO";
		descriptionShort = "$STR_VB_MR_Coal_Sand_DO";
		model = "VB_MiningResources\m\Kamens\SandCoal.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Coal_Bar: VB_MR_StoneBar_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Coal_Bar_N";
		descriptionShort = "$STR_VB_MR_Coal_Bar_D";
		model = "VB_MiningResources\data\items\vb_mr_briquette.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_coal_briquette_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_coal_briquette.rvmat"};
		inventorySlot[] += {"Firewood"};
		canBeSplit = 1;
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 6;
		varQuantityDestroyOnMin = 1;
		varStackMax = 1;
		weight = 300;
		itemSize[] = {2,2};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.0,{}}};
				};
			};
		};
	};
	class BRDK_BigCoal: VB_MR_Coal_Bar
	{
		scope = 2;
		displayName = "$STR_VB_MR_Coal_Bar_NO";
		descriptionShort = "$STR_VB_MR_Coal_Bar_DO";
		model = "VB_MiningResources\m\Kamens\BigCoalStone.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Rubin: VB_MR_StoneNugget_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Rubin_N";
		descriptionShort = "$STR_VB_MR_Rubin_D";
		model = "VB_MiningResources\data\items\vb_mr_chunk.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_rubin_chunk_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_rubin_chunk.rvmat"};
		animClass = "NoFireClass";
		weight = 500;
		itemSize[] = {2,2};
		rotationFlags = 17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Rubin: VB_MR_Rubin
	{
		scope = 2;
		displayName = "$STR_VB_MR_Rubin_NO";
		descriptionShort = "$STR_VB_MR_Rubin_DO";
		model = "VB_MiningResources\m\Kamens\SmalRubin.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Rubin_Sand: VB_MR_StoneSand_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Rubin_Sand_N";
		descriptionShort = "$STR_VB_MR_Rubin_Sand_D";
		model = "VB_MiningResources\data\items\vb_mr_sand.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_rubin_sand_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_rubin_sand.rvmat"};
		animClass = "Compass";
		itemSize[] = {1,1};
		weight = 190;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Rubin_Sand: VB_MR_Rubin_Sand
	{
		scope = 2;
		displayName = "$STR_VB_MR_Rubin_Sand_NO";
		descriptionShort = "$STR_VB_MR_Rubin_Sand_DO";
		model = "VB_MiningResources\m\Kamens\SandRubin.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Rubin_Bar: VB_MR_StoneBar_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Rubin_Bar_N";
		descriptionShort = "$STR_VB_MR_Rubin_Bar_D";
		model = "VB_MiningResources\data\items\vb_mr_briquette.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_rubin_briquette_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_rubin_briquette.rvmat"};
		weight = 300;
		itemSize[] = {2,2};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.0,{}}};
				};
			};
		};
	};
	class BRDK_BigRubin: VB_MR_Rubin_Bar
	{
		scope = 2;
		displayName = "$STR_VB_MR_Rubin_Bar_NO";
		descriptionShort = "$STR_VB_MR_Rubin_Bar_DO";
		model = "VB_MiningResources\m\Kamens\BigRubinStone.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Nephrite: VB_MR_StoneNugget_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Nephrite_N";
		descriptionShort = "$STR_VB_MR_Nephrite_D";
		model = "VB_MiningResources\data\items\vb_mr_chunk.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_nephrite_chunk_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_nephrite_chunk.rvmat"};
		animClass = "NoFireClass";
		weight = 500;
		itemSize[] = {2,2};
		rotationFlags = 17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Nephrite: VB_MR_Nephrite
	{
		scope = 2;
		displayName = "$STR_VB_MR_Nephrite_NO";
		descriptionShort = "$STR_VB_MR_Nephrite_DO";
		model = "VB_MiningResources\m\Kamens\SmalNephrite.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Nephrite_Sand: VB_MR_StoneSand_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Nephrite_Sand_N";
		descriptionShort = "$STR_VB_MR_Nephrite_Sand_D";
		model = "VB_MiningResources\data\items\vb_mr_sand.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_nephrite_sand_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_nephrite_sand.rvmat"};
		animClass = "Compass";
		itemSize[] = {1,1};
		weight = 190;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Nephrite_Sand: VB_MR_Nephrite_Sand
	{
		scope = 2;
		displayName = "$STR_VB_MR_Nephrite_Sand_NO";
		descriptionShort = "$STR_VB_MR_Nephrite_Sand_DO";
		model = "VB_MiningResources\m\Kamens\SandNephrite.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Nephrite_Bar: VB_MR_StoneBar_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Nephrite_Bar_N";
		descriptionShort = "$STR_VB_MR_Nephrite_Bar_D";
		model = "VB_MiningResources\data\items\vb_mr_briquette.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_nephrite_briquette_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_nephrite_briquette.rvmat"};
		weight = 300;
		itemSize[] = {2,2};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.0,{}}};
				};
			};
		};
	};
	class BRDK_BigNephrite: VB_MR_Nephrite_Bar
	{
		scope = 2;
		displayName = "$STR_VB_MR_Nephrite_Bar_NO";
		descriptionShort = "$STR_VB_MR_Nephrite_Bar_DO";
		model = "VB_MiningResources\m\Kamens\BigNephriteStone.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Sulfur: VB_MR_StoneNugget_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Sulfur_N";
		descriptionShort = "$STR_VB_MR_Sulfur_D";
		model = "VB_MiningResources\data\items\vb_mr_chunk.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_sulfur_chunk_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_sulfur_chunk.rvmat"};
		animClass = "NoFireClass";
		weight = 500;
		itemSize[] = {2,2};
		rotationFlags = 17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Sulfur: VB_MR_Sulfur
	{
		scope = 2;
		displayName = "$STR_VB_MR_Sulfur_NO";
		descriptionShort = "$STR_VB_MR_Sulfur_DO";
		model = "VB_MiningResources\m\Kamens\SmalSulfur.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Sulfur_Sand: VB_MR_StoneSand_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Sulfur_Sand_N";
		descriptionShort = "$STR_VB_MR_Sulfur_Sand_D";
		model = "VB_MiningResources\data\items\vb_mr_sand.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_sulfur_sand_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_sulfur_sand.rvmat"};
		animClass = "Compass";
		itemSize[] = {1,1};
		weight = 190;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class BRDK_Sulfur_Sand: VB_MR_Sulfur_Sand
	{
		scope = 2;
		displayName = "$STR_VB_MR_Sulfur_Sand_NO";
		descriptionShort = "$STR_VB_MR_Sulfur_Sand_DO";
		model = "VB_MiningResources\m\Kamens\SandSulfur.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Sulfur_Bar: VB_MR_StoneBar_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Sulfur_Bar_N";
		descriptionShort = "$STR_VB_MR_Sulfur_Bar_D";
		model = "VB_MiningResources\data\items\vb_mr_briquette.p3d";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\items\vb_mr_sulfur_briquette_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_MiningResources\data\items\vb_mr_sulfur_briquette.rvmat"};
		weight = 300;
		itemSize[] = {2,2};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.0,{}}};
				};
			};
		};
	};
	class BRDK_BigSulfur: VB_MR_Sulfur_Bar
	{
		scope = 2;
		displayName = "$STR_VB_MR_Sulfur_Bar_NO";
		descriptionShort = "$STR_VB_MR_Sulfur_Bar_DO";
		model = "VB_MiningResources\m\Kamens\BigSulfurStone.p3d";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
	};
	class VB_MR_Rubin_Gem: VB_MR_Gem_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Rubin_Gem_N";
		descriptionShort = "$STR_VB_MR_Rubin_Gem_D";
		model = "VB_MiningResources\m\Diamonds\RubinDiamond.p3d";
		weight = 50;
		itemSize[] = {1,1};
		rotationFlags = 17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};
	class VB_MR_Nephrite_Gem: VB_MR_Gem_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Nephrite_Gem_N";
		descriptionShort = "$STR_VB_MR_Nephrite_Gem_D";
		model = "VB_MiningResources\m\Diamonds\NephriteDiamond.p3d";
		weight = 50;
		itemSize[] = {1,1};
		rotationFlags = 17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9900;
					healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.1,{}}};
				};
			};
		};
	};

	// ---------------------------------------------------------------- оборудование: новый класс и старый
	class VB_MR_Press_Kit: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Press_Kit_N";
		descriptionShort = "$STR_VB_MR_Press_Kit_D";
		model = "VB_MiningResources\m\KIT\BRDK_Press_Kit.p3d";
		projectionTypename = "VB_MR_Press";
		allowOwnedCargoManipulation = 1;
		hologramMaterial = "fishnettrap";
		hologramMaterialPath = "dz\gear\traps\data";
		overrideDrawArea = 8.0;
		forcarBubble = "true";
		destroyOnEmpty = 0;
		carveNavmesh = 1;
		heavyItem = 1;
		weight = 1000;
		itemBehaviour = 1;
		physLayer = "item_large";
		itemSize[] = {5,3};
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1.0,{"VB_MiningResources\m\KIT\data\box_kit.rvmat"}},{0.7,{"VB_MiningResources\m\KIT\data\box_kit.rvmat"}},{0.5,{"VB_MiningResources\m\KIT\data\box_kitDam.rvmat"}},{0.3,{"VB_MiningResources\m\KIT\data\box_kitDam.rvmat"}},{0.1,{"VB_MiningResources\m\KIT\data\box_kitDes.rvmat"}}};
				};
			};
		};
	};
	class BRDK_Press_kit: VB_MR_Press_Kit
	{
		scope = 2;
		descriptionShort = "$STR_VB_MR_Press_Kit_DO";
	};
	class VB_MR_Press: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_VB_MR_Press_N";
		descriptionShort = "$STR_VB_MR_Press_D";
		model = "VB_MiningResources\m\press\BRDK_Press.p3d";
		physLayer = "item_large";
		attachments[] = {"BRDK_STONES"};
		forcarBubble = "true";
		handheld = "false";
		weight = 1000000;
		useEntityHierarchy = "true";
		yawPitchRollLimit[] = {10,10,10};
		alignHologramToTerain = 0;
		slopeTolerance = 50.3;
		itemSize[] = {2,4};
		itemsCargoSize[] = {3,6};
		carveNavmesh = 1;
		heavyItem = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\m\press\data\Press_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_MiningResources\m\press\data\Press.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 10000;
					healthLevels[] = {{1.0,{"VB_MiningResources\m\press\data\Press.rvmat"}},{0.7,{"VB_MiningResources\m\press\data\Press.rvmat"}},{0.5,{"VB_MiningResources\m\press\data\PressDam.rvmat"}},{0.3,{"VB_MiningResources\m\press\data\PressDam.rvmat"}},{0.1,{"VB_MiningResources\m\press\data\PressDes.rvmat"}}};
				};
			};
		};
	};
	class BRDK_Stone_Press: VB_MR_Press
	{
		scope = 2;
		descriptionShort = "$STR_VB_MR_Press_DO";
	};
	class VB_MR_MeltingBowl: Cauldron
	{
		scope = 2;
		displayName = "$STR_VB_MR_MeltingBowl_N";
		descriptionShort = "$STR_VB_MR_MeltingBowl_D";
		model = "VB_MiningResources\m\pot\BRDK_MeltingBowl.p3d";
		weight = 700;
		itemSize[] = {3,3};
		itemsCargoSize[] = {3,3};
		attachments[] = {"BRDK_METALS"};
		inventorySlot[] = {"CookingEquipment","DirectCookingA","DirectCookingB","DirectCookingC"};
		stackedRandom = 0;
		canBeDigged = 0;
		allowOwnedCargoManipulation = 1;
		destroyOnEmpty = 0;
		quantityBar = 0;
		stackedUnit = "pc.";
		varQuantityDestroyOnMin = 0;
		varQuantityInit = 0;
		varQuantityMin = 0;
		varQuantityMax = 0;
		volume = 3000;
		varTemperatureMax = 200;
		varLiquidTypeInit = 0;
		liquidContainerType = "";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1000000;
					healthLevels[] = {{1.0,{"VB_MiningResources\m\pot\data\pot.rvmat"}},{0.7,{"VB_MiningResources\m\pot\data\pot.rvmat"}},{0.5,{"VB_MiningResources\m\pot\data\potDam.rvmat"}},{0.3,{"VB_MiningResources\m\pot\data\potDam.rvmat"}},{0.1,{"VB_MiningResources\m\pot\data\potDes.rvmat"}}};
				};
			};
		};
		repairableWithKits[] = {10};
		repairCosts[] = {25};
		class AnimationSources
		{
			class handle1Rotate
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 1;
			};
		};
		soundImpactType = "metal";
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpPotLight
				{
					soundSet = "pickUpPotLight_SoundSet";
					id = 796;
				};
				class pickUpPot
				{
					soundSet = "pickUpPot_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "pot_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class BRDK_MeltingBowl: VB_MR_MeltingBowl
	{
		scope = 2;
		descriptionShort = "$STR_VB_MR_MeltingBowl_DO";
	};
};
