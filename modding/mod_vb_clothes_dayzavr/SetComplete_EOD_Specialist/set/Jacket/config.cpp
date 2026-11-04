class CfgPatches
{
	class TGK_SetComplete_EOD_Specialist
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters","DZ_Data"};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class TGK_SetComplete_EOD_Specialist_Top_Colorbase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_Alpha_Top";
		descriptionShort = "$STR_VBC_Alpha_Top_Desc";
		model = "SetComplete_EOD_Specialist\set\Jacket\Jacket_g.p3d";
		inventorySlot[] = {"Body"};
		itemInfo[] = {"Clothing","Body"};
		weight = 1500;
		itemSize[] = {4,3};
		varWetMin = 0.049;
		varWetMax = 0.249;
		absorbency = 0.55;
		heatIsolation = 0.7;
		quickBarBonus = 0;
		repairableWithKits[] = {2,3};
		itemsCargoSize[] = {10,6};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"jacket"};
		class ClothingTypes
		{
			male = "SetComplete_EOD_Specialist\set\Jacket\Jacket_m.p3d";
			female = "SetComplete_EOD_Specialist\set\Jacket\Jacket_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"SetComplete_EOD_Specialist\set\data\top\top.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\top\top.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\top\top_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\top\top_damage.rvmat"}},{0,{"SetComplete_EOD_Specialist\set\data\top\top_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.85;
					};
					class Blood
					{
						damage = 0.85;
					};
					class Shock
					{
						damage = 0.85;
					};
				};
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
				class FragGrenade
				{
					class Health
					{
						damage = 0.85;
					};
					class Blood
					{
						damage = 0.85;
					};
					class Shock
					{
						damage = 0.85;
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
					soundSet = "SmershVest_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "SmershVest_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class TGK_EOD_Specialist_Top_Green: TGK_SetComplete_EOD_Specialist_Top_Colorbase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"SetComplete_EOD_Specialist\set\data\top\top_co.paa"};
	};
	class TGK_EOD_Specialist_Top_Black: TGK_SetComplete_EOD_Specialist_Top_Colorbase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"SetComplete_EOD_Specialist\set\data\top\top_Bao.paa"};
	};
};
