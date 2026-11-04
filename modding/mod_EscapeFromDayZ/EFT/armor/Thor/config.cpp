class CfgPatches
{
	class CfgPatches_Thor_Vest
	{
		units[] = {"EFT_Vest_Thor_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_Thor_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_Thor_displayName";
		descriptionShort = "#EFT_Vest_Thor_descriptionShort";
		model = "EFT\armor\Thor\Thor_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 9000;
		itemSize[] = {3,3};
		itemsCargoSize[] = {7, 7};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\Thor\Thor.p3d";
			female = "EFT\armor\Thor\Thor.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 225;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\Thor\Data\armor.rvmat"}},{0.45,{"EFT\armor\Thor\Data\armor.rvmat"}},{0.35,{"EFT\armor\Thor\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\Thor\Data\armor_damage.rvmat"}},{0,{"EFT\armor\Thor\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
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
						damage = 0.6;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.6;
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
	
	class EFT_Vest_Thor: EFT_Vest_Thor_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\Thor\Data\armor.paa"};
	};
};
