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
	class Bottle_Base;
	class WaterBottle: Bottle_Base
	{
	};
	class EFT_food_Vodka: WaterBottle
	{
		scope=2;
		displayName="Vodka";
		descriptionShort="Vodka Tarkovskaya";
		model="\EFT\Food\Vodka\EFT_Vodka.p3d";
		itemSize[]={1,2};
		destroyOnEmpty=1;
		varQuantityDestroyOnMin=0;
		varLiquidTypeInit=512;
		liquidContainerType="1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048 + 4096 + 8192 + 16384 + 32768 + 65536  - (1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256) -32768";
		varQuantityInit=500;
		varQuantityMin=0;
		varQuantityMax=500;
		isMeleeWeapon=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"\EFT\Food\Vodka\Data\food.paa"
		};
		class Nutrition
		{
			totalVolume=1;
			energy=700;
			water=300;
			nutritionalIndex=1;
			toxicity=0;
		};
	};
};
