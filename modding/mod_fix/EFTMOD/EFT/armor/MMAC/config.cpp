class CfgPatches
{
	class CfgPatches_MMAC_Vest
	{
		units[] = {"EFT_Vest_MMAC_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_MMAC_ColorBase: Clothing
	{
		scope = 0;
		displayName = "MMAC";
		descriptionShort = "Armor class:4 Durability:60 ";
		model = "EFT\armor\MMAC\MMAC_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 7200;
		itemSize[] = {3,3};
		itemsCargoSize[] = {3, 4};
		varWetMax = 0.49;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\MMAC\MMAC.p3d";
			female = "EFT\armor\MMAC\MMAC.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 60;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\MMAC\Data\armor.rvmat"}},{0.45,{"EFT\armor\MMAC\Data\armor.rvmat"}},{0.35,{"EFT\armor\MMAC\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\MMAC\Data\armor_damage.rvmat"}},{0,{"EFT\armor\MMAC\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.2;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.1;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.1;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.1;
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
						damage = 0;
					};
					class Shock
					{
						damage = 0.1;
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
	
	class EFT_Vest_MMAC: EFT_Vest_MMAC_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\MMAC\Data\armor.paa"};
	};
};
