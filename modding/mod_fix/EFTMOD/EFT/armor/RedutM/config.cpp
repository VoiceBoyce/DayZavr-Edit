class CfgPatches
{
	class CfgPatches_RedutM_Vest
	{
		units[] = {"EFT_Vest_RedutM_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_RedutM_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_RedutM_displayName";
		descriptionShort = "#EFT_Vest_RedutM_descriptionShort";
		model = "EFT\armor\RedutM\RedutM_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 10500;
		itemSize[] = {3,4};
		itemsCargoSize[] = {6, 6};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\RedutM\RedutM.p3d";
			female = "EFT\armor\RedutM\RedutM.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 340;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\RedutM\Data\armor.rvmat"}},{0.45,{"EFT\armor\RedutM\Data\armor.rvmat"}},{0.35,{"EFT\armor\RedutM\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\RedutM\Data\armor_damage.rvmat"}},{0,{"EFT\armor\RedutM\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.13;
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
	
	class EFT_Vest_RedutM: EFT_Vest_RedutM_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\RedutM\Data\armor.paa"};
	};
};
