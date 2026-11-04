class CfgPatches
{
	class CfgPatches_6b13_k_Vest
	{
		units[] = {"EFT_Vest_6b13_Killa_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	
	class EFT_Vest_6b13_Killa_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_6b13_Killa_displayName";
		descriptionShort = "#EFT_Vest_6b13_Killa_descriptionShort";
		model = "EFT\armor\6b13_k\6b13_k_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 7500;
		itemSize[] = {3,4};
		itemsCargoSize[] = {6, 5};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\6b13_k\6b13_k.p3d";
			female = "EFT\armor\6b13_k\6b13_k.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 330;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\6b13_k\Data\armor.rvmat"}},{0.45,{"EFT\armor\6b13_k\Data\armor.rvmat"}},{0.35,{"EFT\armor\6b13_k\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\6b13_k\Data\armor_damage.rvmat"}},{0,{"EFT\armor\6b13_k\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
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
						damage = 0.25;
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
	
	class EFT_Vest_6b13_Killa: EFT_Vest_6b13_Killa_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\6b13_k\Data\armor_killa.paa"};
	};
};
