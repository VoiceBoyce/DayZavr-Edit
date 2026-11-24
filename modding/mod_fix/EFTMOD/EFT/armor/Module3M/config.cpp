class CfgPatches
{
	class CfgPatches_Module3M_Vest
	{
		units[] = {"EFT_Vest_Module3M_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_Module3M_ColorBase: Clothing
	{
		scope = 0;
		displayName = "Module3M";
		descriptionShort = "Armor class:2 Durability:60 ";
		model = "EFT\armor\Module3M\Module3M_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 6000;
		itemSize[] = {3,3};
		itemsCargoSize[] = {0, 0};
		varWetMax = 0.49;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\Module3M\Module3M.p3d";
			female = "EFT\armor\Module3M\Module3M.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 60;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\Module3M\Data\armor.rvmat"}},{0.45,{"EFT\armor\Module3M\Data\armor.rvmat"}},{0.35,{"EFT\armor\Module3M\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\Module3M\Data\armor_damage.rvmat"}},{0,{"EFT\armor\Module3M\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
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
	
	class EFT_Vest_Module3M: EFT_Vest_Module3M_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\Module3M\Data\armor.paa"};
	};
};
