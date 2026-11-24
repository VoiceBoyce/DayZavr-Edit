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
	class EFT_food_Icegreen: SodaCan_Pipsi
	{
		scope=2;
		displayName="Icegreen";
		descriptionShort="Icegreen Soda can.";
		model="\EFT\Food\Icegreen\EFT_Icegreen.p3d";
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
			"\EFT\Food\Icegreen\Data\food_icegreen.paa"
		};
		class Nutrition
		{
			totalVolume=1;
			energy=150;
			water=150;
			nutritionalIndex=1;
			toxicity=0;
		};
	};
};
