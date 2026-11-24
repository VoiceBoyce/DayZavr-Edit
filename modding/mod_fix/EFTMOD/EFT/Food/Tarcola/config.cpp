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
	class EFT_food_Tarcola: SodaCan_Pipsi
	{
		scope=2;
		displayName="Tarcola";
		descriptionShort="Tarcola ";
		model="\EFT\Food\Tarcola\EFT_Tarcola.p3d";
		itemSize[]={1,1};
		weight=300;
		stackedUnit="";
		varQuantityInit=350;
		varQuantityMin=0;
		varQuantityMax=350;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"\EFT\Food\Tarcola\Data\food_tarcola.paa"
		};
		class Nutrition
		{
			totalVolume=1;
			energy=350;
			water=350;
			nutritionalIndex=1;
			toxicity=0;
		};
	};
};
