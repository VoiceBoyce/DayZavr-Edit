class CfgPatches
{
	class CfgPatches_SS_bigpipe_Vest
	{
		units[] = {"EFT_Vest_SS_bigpipe_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_SS_bigpipe_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_SS_bigpipe_displayName";
		descriptionShort = "#EFT_Vest_SS_bigpipe_descriptionShort";
		model = "EFT\armor\SS_bigpipe\SS_bigpipe_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 6400;
		itemSize[] = {3,3};
		itemsCargoSize[] = {6, 5};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\SS_bigpipe\SS_bigpipe.p3d";
			female = "EFT\armor\SS_bigpipe\SS_bigpipe.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 205;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\SS_bigpipe\Data\armor.rvmat"}},{0.45,{"EFT\armor\SS_bigpipe\Data\armor.rvmat"}},{0.35,{"EFT\armor\SS_bigpipe\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\SS_bigpipe\Data\armor_damage.rvmat"}},{0,{"EFT\armor\SS_bigpipe\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.22;
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
						damage = 0.5;
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
	
	class EFT_Vest_SS_bigpipe: EFT_Vest_SS_bigpipe_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\SS_bigpipe\Data\armor.paa"};
	};
};
