class CfgPatches
{
	class CfgPatches_BlackKnight_Vest
	{
		units[] = {"EFT_Vest_BlackKnight_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_BlackKnight_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_BlackKnight_displayName";
		descriptionShort = "#EFT_Vest_BlackKnight_descriptionShort";
		model = "EFT\armor\BlackKnight\BlackKnight_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 7700;
		itemSize[] = {4,4};
		itemsCargoSize[] = {4, 3};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\BlackKnight\BlackKnight.p3d";
			female = "EFT\armor\BlackKnight\BlackKnight.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 315;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\BlackKnight\Data\armor.rvmat"}},{0.45,{"EFT\armor\BlackKnight\Data\armor.rvmat"}},{0.35,{"EFT\armor\BlackKnight\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\BlackKnight\Data\armor_damage.rvmat"}},{0,{"EFT\armor\BlackKnight\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.19;
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
	
	class EFT_Vest_BlackKnight: EFT_Vest_BlackKnight_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\BlackKnight\Data\armor.paa"};
	};
};
