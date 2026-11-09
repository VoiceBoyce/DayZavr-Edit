class CfgPatches
{
	class MSFZ_Obelisk_dobicha
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
	class MSFZ_RuneGathering_Base: HouseNoDestruct
	{
		scope = 0;
	};
	class MSFZ_Obelisk: MSFZ_RuneGathering_Base
	{
		scope = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Obelisk\data\MSFZ_Obelisk.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Obelisk\data\MSFZ_Obelisk.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Obelisk\MSFZ_Obelisk.p3d";
	};
	class MSFZ_Obelisk_NoActive: HouseNoDestruct
	{
		scope = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Obelisk\data\MSFZ_Obelisk.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Obelisk\data\MSFZ_Obelisk_NoActive.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Obelisk\MSFZ_Obelisk.p3d";
	};
};
