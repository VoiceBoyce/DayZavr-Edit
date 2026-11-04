class CfgPatches
{
	class CfgPatches_M1_Vest
	{
		units[] = {"EFT_Vest_M1_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_M1_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_M1_displayName";
		descriptionShort = "#EFT_Vest_M1_descriptionShort";
		model = "EFT\armor\M1\M1_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 8300;
		itemSize[] = {4,4};
		itemsCargoSize[] = {5, 4};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\M1\M1.p3d";
			female = "EFT\armor\M1\M1.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\M1\Data\armor.rvmat"}},{0.45,{"EFT\armor\M1\Data\armor.rvmat"}},{0.35,{"EFT\armor\M1\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\M1\Data\armor_damage.rvmat"}},{0,{"EFT\armor\M1\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.23;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.25;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.25;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.25;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.25;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.25;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.5;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.5;
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
	
	class EFT_Vest_M1: EFT_Vest_M1_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\M1\Data\armor.paa"};
	};
};
