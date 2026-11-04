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
	class TGK_SetComplete_EOD_Specialist_Pants_Colorbase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_Alpha_Pants";
		descriptionShort = "$STR_VBC_Alpha_Pants_Desc";
		model = "SetComplete_EOD_Specialist\set\Pants\Pants_g.p3d";
		inventorySlot[] = {"Legs"};
		itemInfo[] = {"Clothing","Legs"};
		weight = 1200;
		itemSize[] = {4,6};
		varWetMin = 0.049;
		varWetMax = 0.55;
		absorbency = 0.55;
		heatIsolation = 0.5;
		quickBarBonus = 2;
		itemsCargoSize[] = {7,5};
		repairableWithKits[] = {2,3};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"pants","pads"};
		class ClothingTypes
		{
			male = "SetComplete_EOD_Specialist\set\Pants\Pants_m.p3d";
			female = "SetComplete_EOD_Specialist\set\Pants\Pants_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 150;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"SetComplete_EOD_Specialist\set\data\pants\pants.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\pants\pants.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\pants\pants_damage.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\pants\pants_damage.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads_damage.rvmat"}},{0,{"SetComplete_EOD_Specialist\set\data\pants\pants_destruct.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.95;
					};
					class Blood
					{
						damage = 0.95;
					};
					class Shock
					{
						damage = 0;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.95;
					};
					class Blood
					{
						damage = 0.95;
					};
					class Shock
					{
						damage = 0.45;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.1;
					};
					class Blood
					{
						damage = 0.5;
					};
					class Shock
					{
						damage = 0.15;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 1;
					};
					class Blood
					{
						damage = 0.1;
					};
					class Shock
					{
						damage = 0.1;
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
	class TGK_EOD_Specialist_Pants_Green: TGK_SetComplete_EOD_Specialist_Pants_Colorbase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"SetComplete_EOD_Specialist\set\data\pants\pants_co.paa","SetComplete_EOD_Specialist\set\data\pads\pads_co.paa"};
	};
	class TGK_EOD_Specialist_Pants_Black: TGK_SetComplete_EOD_Specialist_Pants_Colorbase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"SetComplete_EOD_Specialist\set\data\pants\pants_Bao.paa","SetComplete_EOD_Specialist\set\data\pads\pads_Bao.paa"};
	};
};
