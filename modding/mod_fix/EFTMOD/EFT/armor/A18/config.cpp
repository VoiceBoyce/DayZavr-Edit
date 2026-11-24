class CfgPatches
{
	class CfgPatches_A18_Vest
	{
		units[] = {"EFT_Vest_A18_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_A18_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_A18_displayName";
		descriptionShort = "#EFT_Vest_A18_descriptionShort";
		model = "EFT\armor\A18\A18_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 8200;
		itemSize[] = {3,4};
		itemsCargoSize[] = {4, 4};
		varWetMax = 0.49;
		heatIsolation = 0.08;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\A18\A18.p3d";
			female = "EFT\armor\A18\A18.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 300;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\A18\Data\armor.rvmat"}},{0.45,{"EFT\armor\A18\Data\armor.rvmat"}},{0.35,{"EFT\armor\A18\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\A18\Data\armor_damage.rvmat"}},{0,{"EFT\armor\A18\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
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
						damage = 0.4;
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
	
	class EFT_Vest_A18: EFT_Vest_A18_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\A18\Data\armor.paa"};
	};
};
