class CfgPatches
{
	class MSFZ_Portal_dobicha
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {};
	};
};
class CfgVehicles
{
	class HouseNoDestruct;
	class MSFZ_Portal: HouseNoDestruct
	{
		scope = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Portal\data\MSFZ_Portal.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Portal\data\MSFZ_Portal.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Portal\MSFZ_Portal.p3d";
	};
	class MSFZ_Portal_Exit: HouseNoDestruct
	{
		scope = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Portal\data\MSFZ_Portal.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Portal\data\MSFZ_Portal_Exit.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Portal\MSFZ_Portal_Exit.p3d";
	};
};
