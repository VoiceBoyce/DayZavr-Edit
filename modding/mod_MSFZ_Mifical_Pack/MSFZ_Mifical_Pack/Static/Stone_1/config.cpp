class CfgPatches
{
	class msfz_stone_dobicha
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
	class MSFZ_RuneGathering_Base;
	class msfz_stone_1: MSFZ_RuneGathering_Base
	{
		scope = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_1\data\MSFZ_Stihiy_Stone_1.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_1\data\MSFZ_Stihiy_Stone_1.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_1\msfz_stone_1.p3d";
	};
	class msfz_stone_2: MSFZ_RuneGathering_Base
	{
		scope = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_1\data\MSFZ_Stihiy_Stone_1.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_1\data\MSFZ_Stihiy_Stone_1.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_1\msfz_stone_2.p3d";
	};
	class msfz_stone_1_NoActive: HouseNoDestruct
	{
		scope = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_1\data\MSFZ_Stihiy_Stone_1.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_1\data\MSFZ_Stihiy_Stone_1_NoActive.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_1\msfz_stone_1.p3d";
	};
	class msfz_stone_2_NoActive: HouseNoDestruct
	{
		scope = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_1\data\MSFZ_Stihiy_Stone_1.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_1\data\MSFZ_Stihiy_Stone_1_NoActive.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_1\msfz_stone_2.p3d";
	};
};
