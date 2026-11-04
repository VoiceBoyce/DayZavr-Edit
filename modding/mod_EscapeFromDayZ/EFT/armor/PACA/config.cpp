class CfgPatches
{
	class CfgPatches_PACA_Vest
	{
		units[] = {"EFT_Vest_PACA_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_PACA_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_PACA_displayName";
		descriptionShort = "#EFT_Vest_PACA_descriptionShort";
		model = "EFT\armor\PACA\PACA_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 3500;
		itemSize[] = {3,3};
		itemsCargoSize[] = {6, 5};
		varWetMax = 0.2;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\PACA\PACA.p3d";
			female = "EFT\armor\PACA\PACA.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 225;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\PACA\Data\armor.rvmat"}},{0.45,{"EFT\armor\PACA\Data\armor.rvmat"}},{0.35,{"EFT\armor\PACA\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\PACA\Data\armor_damage.rvmat"}},{0,{"EFT\armor\PACA\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.32;
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
				class Melee
				{
					class Health
					{
						damage = 0.35;
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
				class Infected
				{
					class Health
					{
						damage = 0.35;
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
	
	class EFT_Vest_PACA: EFT_Vest_PACA_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\PACA\Data\armor.paa"};
	};
};
