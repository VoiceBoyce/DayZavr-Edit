class CfgPatches
{
	class TGK_SetComplete_EOD_Specialist
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Scripts"};
	};
};
class cfgVehicles
{
	class Clothing;
	class TGK_SetComplete_EOD_Specialist_Gloves_Colorbase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_Alpha_Gloves";
		descriptionShort = "$STR_VBC_Alpha_Gloves_Desc";
		quickBarBonus = 0;
		model = "SetComplete_EOD_Specialist\set\Gloves\Gloves_g.p3d";
		inventorySlot[] = {"Gloves"};
		simulation = "clothing";
		itemInfo[] = {"Clothing","Gloves"};
		weight = 400;
		itemSize[] = {2,2};
		varWetMax = 0.249;
		heatIsolation = 0.7;
		repairableWithKits[] = {5,2};
		repairCosts[] = {30,25};
		hiddenSelections[] = {"gloves"};
		class ClothingTypes
		{
			male = "SetComplete_EOD_Specialist\set\Gloves\Gloves_m.p3d";
			female = "SetComplete_EOD_Specialist\set\Gloves\Gloves_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"SetComplete_EOD_Specialist\set\data\gloves\gloves.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\gloves\gloves.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\gloves\gloves_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\gloves\gloves_damage.rvmat"}},{0.0,{"SetComplete_EOD_Specialist\set\data\gloves\gloves_destruct.rvmat"}}};
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
					soundSet = "WorkingGloves_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "WorkingGloves_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class TGK_EOD_Specialist_Gloves_Green: TGK_SetComplete_EOD_Specialist_Gloves_Colorbase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"SetComplete_EOD_Specialist\set\data\gloves\gloves_co.paa"};
	};
	class TGK_EOD_Specialist_Gloves_Black: TGK_SetComplete_EOD_Specialist_Gloves_Colorbase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"SetComplete_EOD_Specialist\set\data\gloves\gloves_BaO.paa"};
	};
};
