class CfgPatches
{
	class msfz_stone_gate
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
	class msfz_stone_gate: HouseNoDestruct
	{
		scope = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_2\data\MSFZ_Obichniye_Stone_1.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_2\data\MSFZ_Obichniye_Stone_1.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_2\msfz_stone_gate.p3d";
	};
};
