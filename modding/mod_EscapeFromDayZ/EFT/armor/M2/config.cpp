class CfgPatches
{
	class CfgPatches_M2_Vest
	{
		units[] = {"EFT_Vest_M2_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_M2_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_M2_displayName";
		descriptionShort = "#EFT_Vest_M2_descriptionShort";
		model = "EFT\armor\M2\M2_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 7000;
		itemSize[] = {4,4};
		itemsCargoSize[] = {4, 4};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\M2\M2.p3d";
			female = "EFT\armor\M2\M2.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 195;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\M2\Data\armor.rvmat"}},{0.45,{"EFT\armor\M2\Data\armor.rvmat"}},{0.35,{"EFT\armor\M2\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\M2\Data\armor_damage.rvmat"}},{0,{"EFT\armor\M2\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.24;
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
						damage = 0.75;
					};
					class Blood
					{
						damage = 0;
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
	
	class EFT_Vest_M2: EFT_Vest_M2_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\M2\Data\armor.paa"};
	};
};
