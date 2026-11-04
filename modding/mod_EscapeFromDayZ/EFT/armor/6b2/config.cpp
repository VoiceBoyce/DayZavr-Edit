class CfgPatches
{
	class CfgPatches_6B2_Vest
	{
		units[] = {"EFT_Vest_6B2_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_6B2_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_6B2_Green_displayName";
		descriptionShort = "#EFT_Vest_6B2_Green_descriptionShort";
		model = "EFT\armor\6b2\6b2_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 4100;
		itemSize[] = {3,3};
		itemsCargoSize[] = {6, 6};
		varWetMax = 0.19;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\6b2\6b2.p3d";
			female = "EFT\armor\6b2\6b2.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 225;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\6b2\Data\armor.rvmat"}},{0.45,{"EFT\armor\6b2\Data\armor.rvmat"}},{0.35,{"EFT\armor\6b2\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\6b2\Data\armor_damage.rvmat"}},{0,{"EFT\armor\6b2\Data\armor_destruct.rvmat"}}};
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
						damage = 0.32;
					};
				};
				class Melee
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
						damage = 0.32;
					};
				};
				class Infected
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
						damage = 0.32;
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
	class EFT_Vest_6B2_Green: EFT_Vest_6b2_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\6b2\Data\armor.paa"};
	};
};
