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
	class EFT_food_MaxEnergy: SodaCan_Pipsi
	{
		scope=2;
		displayName="MaxEnergy";
		descriptionShort="Max Energy drink";
		model="\EFT\Food\MaxEnergy\EFT_MaxEnergy.p3d";
		itemSize[]={1,1};
		weight=100;
		stackedUnit="";
		varQuantityInit=400;
		varQuantityMin=0;
		varQuantityMax=400;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"\EFT\Food\MaxEnergy\Data\food_maxenergy.paa"
		};
		class Nutrition
		{
			totalVolume=1;
			energy=400;
			water=400;
			nutritionalIndex=1;
			toxicity=0;
		};
	};
};
