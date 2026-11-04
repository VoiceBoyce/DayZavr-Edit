class CfgPatches
{
	class CfgPatches_Plate_carrier_Vest
	{
		units[] = {"EFT_Vest_Plate_carrier_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_Plate_carrier_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_Plate_carrier_displayName";
		descriptionShort = "#EFT_Vest_Plate_carrier_descriptionShort";
		model = "EFT\armor\Plate_carrier\Plate_carrier_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 7500;
		itemSize[] = {3,3};
		itemsCargoSize[] = {0, 0};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\Plate_carrier\Plate_carrier.p3d";
			female = "EFT\armor\Plate_carrier\Plate_carrier.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 355;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\Plate_carrier\Data\armor.rvmat"}},{0.45,{"EFT\armor\Plate_carrier\Data\armor.rvmat"}},{0.35,{"EFT\armor\Plate_carrier\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\Plate_carrier\Data\armor_damage.rvmat"}},{0,{"EFT\armor\Plate_carrier\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.09;
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
						damage = 0.3;
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
	
	class EFT_Vest_Plate_carrier: EFT_Vest_Plate_carrier_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\Plate_carrier\Data\armor.paa"};
	};
};
