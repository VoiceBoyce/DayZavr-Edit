// MSFZ_MineralPack — пак предметов-минералов (13 шт.): только предметы с названием и описанием.
// Собрано в один текстовый конфиг из 8 config.bin (2026-10-23), свойства предметов перенесены без изменений.
// Убрано: неиспользуемая база точек добычи MSFZ_MineralGathering_Base.

class CfgPatches
{
	class MSFZ_MineralPack
	{
		units[] = {"MSFZ_Crystal_Gold", "MSFZ_Crystal_Blue", "MSFZ_Crystal_Amethyst", "MSFZ_Crystal_Pink", "MSFZ_Crystal_Pyrite", "MSFZ_Crystal_Stone", "MSFZ_Crystal_Brass", "MSFZ_Crystal_Cooper", "MSFZ_Crystal_Damascus", "MSFZ_Crystal_Iron", "MSFZ_Crystal_Mithril", "MSFZ_Crystal_Obsidian", "MSFZ_Crystal_Silver"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Weapons_Melee","DZ_Gear_Consumables","DZ_Gear_Tools","DZ_Characters"};
	};
};

class CfgMods
{
	class MSFZ_MineralPack
	{
		dir = "MSFZ_MineralPack";
		hideName = 1;
		hidePicture = 1;
		name = "MSFZ_MineralPack";
		credits = "mars";
		author = "mars";
		authorID = "76561199100729161";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = {"World"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"MSFZ_MineralPack/Scripts/4_World"};
			};
		};
	};
};

class CfgVehicles
{
	class Inventory_Base;

	// База всех минералов.
	class MSFZ_Mineral_Base: Inventory_Base
	{
		scope = 0;
	};

	class MSFZ_Crystal_Gold: MSFZ_Mineral_Base
	{
		scope = 2;
		displayName = "$STR_Crystal_Gold";
		descriptionShort = "$STR_Crystal_Gold_text";
		model = "MSFZ_MineralPack\Minerals\Crystal_Gold\MSFZ_Crystal_Gold.p3d";
		weight = 45.28;
		itemSize[] = {1,2};
		canBeSplit = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Gold\data\MSFZ_Crystal_Gold.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_MineralPack\Minerals\Crystal_Gold\data\MSFZ_Crystal_Gold.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{"MSFZ_MineralPack\Minerals\Crystal_Gold\data\MSFZ_Crystal_Gold.rvmat"}},{0.7,{"MSFZ_MineralPack\Minerals\Crystal_Gold\data\MSFZ_Crystal_Gold.rvmat"}},{0.5,{"MSFZ_MineralPack\Minerals\Crystal_Gold\data\MSFZ_Crystal_Gold_damage.rvmat"}},{0.3,{"MSFZ_MineralPack\Minerals\Crystal_Gold\data\MSFZ_Crystal_Gold_damage.rvmat"}},{0,{"MSFZ_MineralPack\Minerals\Crystal_Gold\data\MSFZ_Crystal_Gold_destruct.rvmat"}}};
				};
			};
		};
		soundImpactType = "metal";
	};


	class MSFZ_Crystal_Blue: MSFZ_Mineral_Base
	{
		scope = 2;
		displayName = "$STR_Crystal_Blue";
		descriptionShort = "$STR_Crystal_Blue_text";
		model = "MSFZ_MineralPack\Minerals\Crystal_Blue\MSFZ_Crystal_Blue.p3d";
		weight = 45.28;
		itemSize[] = {1,2};
		canBeSplit = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Blue\data\MSFZ_Crystal_Blue.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_MineralPack\Minerals\Crystal_Blue\data\MSFZ_Crystal_Blue.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{"MSFZ_MineralPack\Minerals\Crystal_Blue\data\MSFZ_Crystal_Blue.rvmat"}},{0.7,{"MSFZ_MineralPack\Minerals\Crystal_Blue\data\MSFZ_Crystal_Blue.rvmat"}},{0.5,{"MSFZ_MineralPack\Minerals\Crystal_Blue\data\MSFZ_Crystal_Blue_damage.rvmat"}},{0.3,{"MSFZ_MineralPack\Minerals\Crystal_Blue\data\MSFZ_Crystal_Blue_damage.rvmat"}},{0,{"MSFZ_MineralPack\Minerals\Crystal_Blue\data\MSFZ_Crystal_Blue_destruct.rvmat"}}};
				};
			};
		};
		soundImpactType = "metal";
	};


	class MSFZ_Crystal_Amethyst: MSFZ_Mineral_Base
	{
		scope = 2;
		displayName = "$STR_Crystal_Amethyst";
		descriptionShort = "$STR_Crystal_Amethyst_text";
		model = "MSFZ_MineralPack\Minerals\Crystal_Amethyst\MSFZ_Crystal_Amethyst.p3d";
		weight = 45.28;
		itemSize[] = {1,2};
		canBeSplit = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Amethyst\data\MSFZ_Crystal_Amethyst.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_MineralPack\Minerals\Crystal_Amethyst\data\MSFZ_Crystal_Amethyst.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{"MSFZ_MineralPack\Minerals\Crystal_Amethyst\data\MSFZ_Crystal_Amethyst.rvmat"}},{0.7,{"MSFZ_MineralPack\Minerals\Crystal_Amethyst\data\MSFZ_Crystal_Amethyst.rvmat"}},{0.5,{"MSFZ_MineralPack\Minerals\Crystal_Amethyst\data\MSFZ_Crystal_Amethyst_damage.rvmat"}},{0.3,{"MSFZ_MineralPack\Minerals\Crystal_Amethyst\data\MSFZ_Crystal_Amethyst_damage.rvmat"}},{0,{"MSFZ_MineralPack\Minerals\Crystal_Amethyst\data\MSFZ_Crystal_Amethyst_destruct.rvmat"}}};
				};
			};
		};
		soundImpactType = "metal";
	};


	class MSFZ_Crystal_Pink: MSFZ_Mineral_Base
	{
		scope = 2;
		displayName = "$STR_Crystal_Pink";
		descriptionShort = "$STR_Crystal_Pink_text";
		model = "MSFZ_MineralPack\Minerals\Crystal_Pink\MSFZ_Crystal_Pink.p3d";
		weight = 45.28;
		itemSize[] = {1,2};
		canBeSplit = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Pink\data\MSFZ_Crystal_Pink.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_MineralPack\Minerals\Crystal_Pink\data\MSFZ_Crystal_Pink.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{"MSFZ_MineralPack\Minerals\Crystal_Pink\data\MSFZ_Crystal_Pink.rvmat"}},{0.7,{"MSFZ_MineralPack\Minerals\Crystal_Pink\data\MSFZ_Crystal_Pink.rvmat"}},{0.5,{"MSFZ_MineralPack\Minerals\Crystal_Pink\data\MSFZ_Crystal_Pink_damage.rvmat"}},{0.3,{"MSFZ_MineralPack\Minerals\Crystal_Pink\data\MSFZ_Crystal_Pink_damage.rvmat"}},{0,{"MSFZ_MineralPack\Minerals\Crystal_Pink\data\MSFZ_Crystal_Pink_destruct.rvmat"}}};
				};
			};
		};
		soundImpactType = "metal";
	};


	class MSFZ_Crystal_Pyrite: MSFZ_Mineral_Base
	{
		scope = 2;
		displayName = "$STR_Crystal_Pyrite";
		descriptionShort = "$STR_Crystal_Pyrite_text";
		model = "MSFZ_MineralPack\Minerals\Crystal_Pyrite\MSFZ_Crystal_Pyrite.p3d";
		weight = 45.28;
		itemSize[] = {2,1};
		canBeSplit = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Pyrite\data\MSFZ_Crystal_Pyrite.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_MineralPack\Minerals\Crystal_Pyrite\data\MSFZ_Crystal_Pyrite.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{"MSFZ_MineralPack\Minerals\Crystal_Pyrite\data\MSFZ_Crystal_Pyrite.rvmat"}},{0.7,{"MSFZ_MineralPack\Minerals\Crystal_Pyrite\data\MSFZ_Crystal_Pyrite.rvmat"}},{0.5,{"MSFZ_MineralPack\Minerals\Crystal_Pyrite\data\MSFZ_Crystal_Pyrite_damage.rvmat"}},{0.3,{"MSFZ_MineralPack\Minerals\Crystal_Pyrite\data\MSFZ_Crystal_Pyrite_damage.rvmat"}},{0,{"MSFZ_MineralPack\Minerals\Crystal_Pyrite\data\MSFZ_Crystal_Pyrite_destruct.rvmat"}}};
				};
			};
		};
		soundImpactType = "metal";
	};


	class MSFZ_Crystal_Stone: MSFZ_Mineral_Base
	{
		scope = 2;
		displayName = "$STR_Crystal_Stone";
		descriptionShort = "$STR_Crystal_Stone_text";
		model = "MSFZ_MineralPack\Minerals\Crystal_Stone\MSFZ_Crystal_Stone.p3d";
		weight = 45.28;
		itemSize[] = {2,1};
		canBeSplit = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Stone\data\MSFZ_Crystal_Stone.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_MineralPack\Minerals\Crystal_Stone\data\MSFZ_Crystal_Stone.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{"MSFZ_MineralPack\Minerals\Crystal_Stone\data\MSFZ_Crystal_Stone.rvmat"}},{0.7,{"MSFZ_MineralPack\Minerals\Crystal_Stone\data\MSFZ_Crystal_Stone.rvmat"}},{0.5,{"MSFZ_MineralPack\Minerals\Crystal_Stone\data\MSFZ_Crystal_Stone_damage.rvmat"}},{0.3,{"MSFZ_MineralPack\Minerals\Crystal_Stone\data\MSFZ_Crystal_Stone_damage.rvmat"}},{0,{"MSFZ_MineralPack\Minerals\Crystal_Stone\data\MSFZ_Crystal_Stone_destruct.rvmat"}}};
				};
			};
		};
		soundImpactType = "metal";
	};


	class MSFZ_Crystal_Brass: MSFZ_Mineral_Base
	{
		scope = 2;
		displayName = "$STR_CrystalOre";
		descriptionShort = "$STR_CrystalOre_text";
		model = "MSFZ_MineralPack\Minerals\Crystal_Ores\MSFZ_CrystalOre.p3d";
		weight = 45.28;
		itemSize[] = {1,2};
		canBeSplit = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_Crystal_Brass.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_CrystalOre.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_CrystalOre.rvmat"}},{0.7,{"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_CrystalOre.rvmat"}},{0.5,{"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_CrystalOre_damage.rvmat"}},{0.3,{"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_CrystalOre_damage.rvmat"}},{0,{"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_CrystalOre_destruct.rvmat"}}};
				};
			};
		};
		soundImpactType = "metal";
	};
	class MSFZ_Crystal_Cooper: MSFZ_Crystal_Brass
	{
		displayName = "$STR_CrystalCooper";
		descriptionShort = "$STR_CrystalCooper_text";
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_Crystal_Cooper.paa"};
	};
	class MSFZ_Crystal_Damascus: MSFZ_Crystal_Brass
	{
		displayName = "$STR_CrystalDamascus";
		descriptionShort = "$STR_CrystalDamascus_text";
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_Crystal_Damascus.paa"};
	};
	class MSFZ_Crystal_Iron: MSFZ_Crystal_Brass
	{
		displayName = "$STR_CrystalIron";
		descriptionShort = "$STR_CrystalIron_text";
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_Crystal_Iron.paa"};
	};
	class MSFZ_Crystal_Mithril: MSFZ_Crystal_Brass
	{
		displayName = "$STR_CrystalMithril";
		descriptionShort = "$STR_CrystalMithril_text";
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_Crystal_Mithril.paa"};
	};
	class MSFZ_Crystal_Obsidian: MSFZ_Crystal_Brass
	{
		displayName = "$STR_CrystalObsidian";
		descriptionShort = "$STR_CrystalObsidian_text";
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_Crystal_Obsidian.paa"};
	};
	class MSFZ_Crystal_Silver: MSFZ_Crystal_Brass
	{
		displayName = "$STR_CrystalSilver";
		descriptionShort = "$STR_CrystalSilver_text";
		hiddenSelectionsTextures[] = {"MSFZ_MineralPack\Minerals\Crystal_Ores\data\MSFZ_Crystal_Silver.paa"};
	};
};
