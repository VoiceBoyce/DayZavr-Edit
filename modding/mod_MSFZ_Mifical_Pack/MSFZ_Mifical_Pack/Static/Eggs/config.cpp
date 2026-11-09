class CfgPatches
{
	class MSFZ_Eggs
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {};
	};
};
class CfgVehicles
{
	class StaticObject;
	class HouseNoDestruct;
	class MSFZ_Eggs: HouseNoDestruct
	{
		scope = 1;
		hiddenSelections[] = {"camo","camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Eggs\data\MSFZ_Eggs.paa","MSFZ_Mifical_Pack\Static\Eggs\data\MSFZ_Eggs_web.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Eggs\data\MSFZ_Eggs.rvmat","MSFZ_Mifical_Pack\Static\Eggs\data\MSFZ_Eggs_web.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Eggs\MSFZ_Eggs.p3d";
	};
};
