class CfgPatches
{
	class CfgPatches_HEXGRID_Vest
	{
		units[] = {"EFT_Vest_HEXGRID_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_HEXGRID_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_HEXGRID_displayName";
		descriptionShort = "#EFT_Vest_HEXGRID_descriptionShort";
		model = "EFT\armor\HEXGRID\HEXGRID_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 7700;
		itemSize[] = {3,3};
		itemsCargoSize[] = {0, 0};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\HEXGRID\HEXGRID.p3d";
			female = "EFT\armor\HEXGRID\HEXGRID.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 360;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\HEXGRID\Data\armor.rvmat"}},{0.45,{"EFT\armor\HEXGRID\Data\armor.rvmat"}},{0.35,{"EFT\armor\HEXGRID\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\HEXGRID\Data\armor_damage.rvmat"}},{0,{"EFT\armor\HEXGRID\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.08;
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
						damage = 0.35;
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
	
	class EFT_Vest_HEXGRID: EFT_Vest_HEXGRID_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\HEXGRID\Data\armor.paa"};
	};
};
