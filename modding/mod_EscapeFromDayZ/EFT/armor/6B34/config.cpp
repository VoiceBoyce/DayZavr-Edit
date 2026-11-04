class CfgPatches
{
	class CfgPatches_6B34_Vest
	{
		units[] = {"EFT_Vest_6B43_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_6B43_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_6B43_Green_displayName";
		descriptionShort = "#EFT_Vest_6B43_Green_descriptionShort";
		model = "EFT\armor\6B34\6B34_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 20000;
		itemSize[] = {4,4};
		itemsCargoSize[] = {0, 0};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\6B34\6B34.p3d";
			female = "EFT\armor\6B34\6B34.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 375;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\6B34\Data\armor.rvmat"}},{0.45,{"EFT\armor\6B34\Data\armor.rvmat"}},{0.35,{"EFT\armor\6B34\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\6B34\Data\armor_damage.rvmat"}},{0,{"EFT\armor\6B34\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.05;
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
						damage = 0.15;
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
	
	class EFT_Vest_6B43_Green: EFT_Vest_6b43_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\6b34\Data\armor.paa"};
	};
};
