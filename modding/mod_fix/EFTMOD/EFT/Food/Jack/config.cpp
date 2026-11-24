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
	class Edible_Base;
	class Bottle_Base;
	class SodaCan_ColorBase: Edible_Base
	{
	};
	class WaterBottle: Bottle_Base
	{
	};
	class EFT_food_Jack: WaterBottle
	{
		scope=2;
		displayName="Jack Daniels";
		descriptionShort="All knows this whiskey. But it can be only bottle...";
		model="\EFT\Food\Jack\EFT_Jack.p3d";
		itemSize[]={1,2};
		destroyOnEmpty=1;
		varQuantityDestroyOnMin=0;
		varLiquidTypeInit=512;
		liquidContainerType="1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048 + 4096 + 8192 + 16384 + 32768 + 65536  - (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256) -32768";
		varQuantityInit=900;
		varQuantityMin=0;
		varQuantityMax=900;
		isMeleeWeapon=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"\EFT\Food\Jack\Data\food.paa"
		};
		class Nutrition
		{
			totalVolume=1;
			energy=900;
			water=100;
			nutritionalIndex=1;
			toxicity=0;
		};
	};
};
