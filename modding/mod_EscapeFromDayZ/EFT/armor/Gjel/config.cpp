class CfgPatches
{
	class CfgPatches_Gjel_Vest
	{
		units[] = {"EFT_Vest_Gjel_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_Gjel_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_Gjel_displayName";
		descriptionShort = "#EFT_Vest_Gjel_descriptionShort";
		model = "EFT\armor\Gjel\Gjel_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 8900;
		itemSize[] = {3,3};
		itemsCargoSize[] = {7, 5};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\Gjel\Gjel.p3d";
			female = "EFT\armor\Gjel\Gjel.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 335;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\Gjel\Data\armor.rvmat"}},{0.45,{"EFT\armor\Gjel\Data\armor.rvmat"}},{0.35,{"EFT\armor\Gjel\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\Gjel\Data\armor_damage.rvmat"}},{0,{"EFT\armor\Gjel\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.14;
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
						damage = 0.35;
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
	
	class EFT_Vest_Gjel: EFT_Vest_Gjel_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\Gjel\Data\armor.paa"};
	};
};
