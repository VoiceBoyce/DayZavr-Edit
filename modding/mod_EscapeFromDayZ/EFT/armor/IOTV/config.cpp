class CfgPatches
{
	class CfgPatches_IOTV_Vest
	{
		units[] = {"EFT_Vest_IOTV_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_IOTV_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_IOTV_displayName";
		descriptionShort = "#EFT_Vest_IOTV_descriptionShort";
		model = "EFT\armor\IOTV\IOTV_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 13000;
		itemSize[] = {3,4};
		itemsCargoSize[] = {9, 5};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\IOTV\IOTV.p3d";
			female = "EFT\armor\IOTV\IOTV.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 345;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\IOTV\Data\armor.rvmat"}},{0.45,{"EFT\armor\IOTV\Data\armor.rvmat"}},{0.35,{"EFT\armor\IOTV\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\IOTV\Data\armor_damage.rvmat"}},{0,{"EFT\armor\IOTV\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.12;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.15;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.15;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.15;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.15;
					};
					class Blood
					{
						damage = 0;
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
						damage = 0.4;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.35;
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
	
	class EFT_Vest_IOTV: EFT_Vest_IOTV_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\IOTV\Data\armor.paa"};
	};
};
