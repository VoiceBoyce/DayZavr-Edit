class CfgPatches
{
	class KOD_Bipod
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class KOD_Bipod_Base: Inventory_Base
	{
		simpleHiddenSelections[]=
		{
			"fold",
			"unfold"
		};
		recoilDecrease=0.1;
	};
};
