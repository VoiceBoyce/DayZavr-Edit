class CfgPatches
{
	class CfgPatches_Kirasa_Vest
	{
		units[] = {"EFT_Vest_Kirasa_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_Kirasa_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_Kirasa_displayName";
		descriptionShort = "#EFT_Vest_Kirasa_descriptionShort";
		model = "EFT\armor\Kirasa\Kirasa_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 4100;
		itemSize[] = {3,3};
		itemsCargoSize[] = {5, 4};
		varWetMax = 0.29;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\Kirasa\Kirasa.p3d";
			female = "EFT\armor\Kirasa\Kirasa.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 225;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\Kirasa\Data\armor.rvmat"}},{0.45,{"EFT\armor\Kirasa\Data\armor.rvmat"}},{0.35,{"EFT\armor\Kirasa\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\Kirasa\Data\armor_damage.rvmat"}},{0,{"EFT\armor\Kirasa\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
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
						damage = 0.9;
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
	
	class EFT_Vest_Kirasa: EFT_Vest_Kirasa_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\Kirasa\Data\armor.paa"};
	};
};
