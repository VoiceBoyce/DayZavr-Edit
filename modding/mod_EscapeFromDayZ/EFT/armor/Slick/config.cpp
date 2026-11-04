class CfgPatches
{
	class CfgPatches_Slick_Vest
	{
		units[] = {"EFT_Vest_Slick_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_Slick_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_Slick_displayName";
		descriptionShort = "#EFT_Vest_Slick_descriptionShort";
		model = "EFT\armor\Slick\Slick_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 9500;
		itemSize[] = {3,4};
		itemsCargoSize[] = {0, 0};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\Slick\Slick.p3d";
			female = "EFT\armor\Slick\Slick.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 365;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\Slick\Data\armor.rvmat"}},{0.45,{"EFT\armor\Slick\Data\armor.rvmat"}},{0.35,{"EFT\armor\Slick\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\Slick\Data\armor_damage.rvmat"}},{0,{"EFT\armor\Slick\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.07;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.05;
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
						damage = 0.25;
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
	
	class EFT_Vest_Slick: EFT_Vest_Slick_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\Slick\Data\armor.paa"};
	};
};
