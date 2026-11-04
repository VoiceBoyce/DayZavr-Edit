class CfgPatches
{
	class CfgPatches_Zhuk3_Vest
	{
		units[] = {"EFT_Vest_Zhuk3_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_Zhuk3_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_Zhuk3_displayName";
		descriptionShort = "#EFT_Vest_Zhuk3_descriptionShort";
		model = "EFT\armor\Zhuk3\Zhuk3_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 5200;
		itemSize[] = {3,3};
		itemsCargoSize[] = {6, 4};
		varWetMax = 0.4;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\Zhuk3\Zhuk3.p3d";
			female = "EFT\armor\Zhuk3\Zhuk3.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 225;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\Zhuk3\Data\armor.rvmat"}},{0.45,{"EFT\armor\Zhuk3\Data\armor.rvmat"}},{0.35,{"EFT\armor\Zhuk3\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\Zhuk3\Data\armor_damage.rvmat"}},{0,{"EFT\armor\Zhuk3\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.36;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.3;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.3;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.3;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.3;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.3;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.65;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.7;
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
	
	class EFT_Vest_Zhuk3: EFT_Vest_Zhuk3_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\Zhuk3\Data\armor.paa"};
	};
};
