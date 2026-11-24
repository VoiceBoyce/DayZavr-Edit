class CfgPatches
{
	class CfgPatches_Untar_Vest
	{
		units[] = {"EFT_Vest_Untar_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_Untar_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_Untar_displayName";
		descriptionShort = "#EFT_Vest_Untar_descriptionShort";
		model = "EFT\armor\Untar\Untar_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 8500;
		itemSize[] = {3,3};
		itemsCargoSize[] = {5,5};
		varWetMax = 0.4;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\Untar\Untar.p3d";
			female = "EFT\armor\Untar\Untar.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 225;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\Untar\Data\armor.rvmat"}},{0.45,{"EFT\armor\Untar\Data\armor.rvmat"}},{0.35,{"EFT\armor\Untar\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\Untar\Data\armor_damage.rvmat"}},{0,{"EFT\armor\Untar\Data\armor_destruct.rvmat"}}};
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
						damage = 0.35;
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
						damage = 0.65;
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
	
	class EFT_Vest_Untar: EFT_Vest_Untar_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\Untar\Data\armor.paa"};
	};
};
