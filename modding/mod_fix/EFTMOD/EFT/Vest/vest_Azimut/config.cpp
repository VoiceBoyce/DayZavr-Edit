class CfgPatches
{
	class CfgPatches_vest_Azimut_Vest
	{
		units[] = {"EFT_vest_Azimut_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_vest_Azimut_ColorBase: Clothing
	{
		scope = 0;
		displayName = "Azimuth СС";
		descriptionShort = "Azimuth СС ";
		model = "EFT\vest\vest_Azimut\vest_Azimut_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 1600;
		itemSize[] = {4,3};
		itemsCargoSize[] = {4, 5};
		varWetMax = 0.49;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\vest\vest_Azimut\vest_Azimut.p3d";
			female = "EFT\vest\vest_Azimut\vest_Azimut.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1000;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\vest\vest_Azimut\Data\vest.rvmat"}},{0.45,{"EFT\vest\vest_Azimut\Data\vest.rvmat"}},{0.35,{"EFT\vest\vest_Azimut\Data\vest_damage.rvmat"}},{0.3,{"EFT\vest\vest_Azimut\Data\vest_damage.rvmat"}},{0,{"EFT\vest\vest_Azimut\Data\vest_destruct.rvmat"}}};
				};
			};
			class Globalvest
			{
				class Projectile
				{
					class Health
					{
						damage = 1.0;
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
						damage = 1.0;
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
	class EFT_vest_Azimut_black: EFT_vest_Azimut_ColorBase
	{
		scope = 2;
		displayName = "Азимут СС Black";
		hiddenSelections[] = {"zbytek"};
		hiddenSelectionsTextures[] = {"EFT\vest\vest_Azimut\Data\vest_black.paa"};
	};
	class EFT_vest_Azimut_pixel: EFT_vest_Azimut_ColorBase
	{
		scope = 2;
		displayName = "Азимут СС Pixel";
		hiddenSelections[] = {"zbytek"};
		hiddenSelectionsTextures[] = {"EFT\vest\vest_Azimut\Data\vest_pixel.paa"};
	};
};
