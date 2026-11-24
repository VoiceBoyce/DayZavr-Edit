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
	class EFT_food_Kvass: SodaCan_Pipsi
	{
		scope=2;
		displayName="Kvass";
		descriptionShort="Kvass Norvinsky Yadrenyi, Premium";
		model="\EFT\Food\Kvass\EFT_Kvass.p3d";
		weight=600;
		stackedUnit="";
		varQuantityInit=600;
		varQuantityMin=0;
		varQuantityMax=600;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"\EFT\Food\Kvass\Data\food.paa"
		};
		class Nutrition
		{
			totalVolume=1;
			energy=600;
			water=900;
			nutritionalIndex=1;
			toxicity=0;
		};
	};
};
