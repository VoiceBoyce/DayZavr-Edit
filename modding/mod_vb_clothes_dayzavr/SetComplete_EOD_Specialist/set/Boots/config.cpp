class CfgPatches
{
	class TGK_SetComplete_EOD_Specialist
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters","DZ_Characters_Shoes"};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class TGK_SetComplete_EOD_Specialist_Boots_Colorbase: Clothing
	{
		displayName = "$STR_VBC_Alpha_Boots";
		descriptionShort = "$STR_VBC_Alpha_Boots_Desc";
		quickBarBonus = 0;
		model = "SetComplete_EOD_Specialist\set\Boots\Boots_g.p3d";
		inventorySlot[] = {"Feet"};
		itemInfo[] = {"Clothing","Feet"};
		itemSize[] = {3,2};
		weight = 1300;
		durability = 0.35;
		varWetMax = 0.249;
		heatIsolation = 0.8;
		visibilityModifier = 0.9;
		repairableWithKits[] = {3};
		repairCosts[] = {25.0};
		soundAttType = "Sneakers";
		soundImpactType = "default";
		hiddenSelections[] = {"camo"};
		class ClothingTypes
		{
			male = "SetComplete_EOD_Specialist\set\Boots\Boots_m.p3d";
			female = "SetComplete_EOD_Specialist\set\Boots\Boots_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1.0,{"SetComplete_EOD_Specialist\set\data\boots\Boots.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\boots\Boots.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\boots\Boots_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\boots\Boots_damage.rvmat"}},{0.0,{"SetComplete_EOD_Specialist\set\data\boots\Boots_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet = "AthleticShoes_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "AthleticShoes_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class TGK_EOD_Specialist_Boots: TGK_SetComplete_EOD_Specialist_Boots_Colorbase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"SetComplete_EOD_Specialist\set\data\boots\Boots_co.paa"};
	};
};
