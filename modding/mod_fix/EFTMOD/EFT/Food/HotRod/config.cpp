class CfgPatches
{
	class DZ_Gear_Drinks
	{
		units[]=
		{
			"Drink_Canteen",
			"Drink_SodaZlutaKlasik",
			"Drink_SodaZlutaKolaloka",
			"Drink_SodaZlutaMalinovka",
			"Drink_WaterBottle"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class SodaCan_Pipsi;
	class EFT_food_HotRod: SodaCan_Pipsi
	{
		scope=2;
		displayName="HotRod";
		descriptionShort="HotRod Energy drink. Be as rod in ass!";
		model="\EFT\Food\HotRod\EFT_HotRod.p3d";
		itemSize[]={1,1};
		weight=100;
		stackedUnit="";
		varQuantityInit=300;
		varQuantityMin=0;
		varQuantityMax=300;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"\EFT\Food\HotRod\Data\food.paa"
		};
		class Nutrition
		{
			totalVolume=1;
			energy=300;
			water=300;
			nutritionalIndex=1;
			toxicity=0;
		};
	};
};
