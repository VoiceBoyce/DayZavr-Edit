class CfgPatches
{
	class CfgPatches_6b5_Vest
	{
		units[] = {"EFT_Vest_6b5_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_6b5_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_6b5_displayName";
		descriptionShort = "#EFT_Vest_6b5_descriptionShort";
		model = "EFT\armor\6b5\6b5_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 7200;
		itemSize[] = {3,4};
		itemsCargoSize[] = {6, 4};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\6b5\6b5.p3d";
			female = "EFT\armor\6b5\6b5.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 185;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\6b5\Data\armor.rvmat"}},{0.45,{"EFT\armor\6b5\Data\armor.rvmat"}},{0.35,{"EFT\armor\6b5\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\6b5\Data\armor_damage.rvmat"}},{0,{"EFT\armor\6b5\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.26;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.26;
					};
				};
				class Melee
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
						damage = 0.2;
					};
				};
				class Infected
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
						damage = 0.2;
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
	class EFT_Vest_6b5: EFT_Vest_6b5_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\6b5\Data\armor.paa"};
	};
};
