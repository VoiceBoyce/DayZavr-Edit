class CfgPatches
{
	class msfz_stone_altar
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
	class msfz_stone_altar_1: MSFZ_RuneGathering_Base
	{
		scope = 1;
		hiddenSelections[] = {"camo","stihiya"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2.paa","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_shar.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2.rvmat","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_3\msfz_stone_altar.p3d";
	};
	class msfz_stone_altar_2: MSFZ_RuneGathering_Base
	{
		scope = 1;
		hiddenSelections[] = {"camo","stihiya"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_blue.paa","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_shar.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_blue.rvmat","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_blue.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_3\msfz_stone_altar.p3d";
	};
	class msfz_stone_altar_3: MSFZ_RuneGathering_Base
	{
		scope = 1;
		hiddenSelections[] = {"camo","stihiya"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_green.paa","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_shar.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_green.rvmat","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_green.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_3\msfz_stone_altar.p3d";
	};
	class msfz_stone_altar_4: MSFZ_RuneGathering_Base
	{
		scope = 1;
		hiddenSelections[] = {"camo","stihiya"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_red.paa","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_shar.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_red.rvmat","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_red.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_3\msfz_stone_altar.p3d";
	};
	class msfz_stone_altar_1_NoActive: HouseNoDestruct
	{
		scope = 1;
		hiddenSelections[] = {"camo","stihiya"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2.paa","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_shar.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_NoActive.rvmat","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_NoActive.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_3\msfz_stone_altar.p3d";
	};
	class msfz_stone_altar_2_NoActive: HouseNoDestruct
	{
		scope = 1;
		hiddenSelections[] = {"camo","stihiya"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_blue.paa","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_shar.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_NoActive.rvmat","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_NoActive.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_3\msfz_stone_altar.p3d";
	};
	class msfz_stone_altar_3_NoActive: HouseNoDestruct
	{
		scope = 1;
		hiddenSelections[] = {"camo","stihiya"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_green.paa","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_shar.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_NoActive.rvmat","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_NoActive.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_3\msfz_stone_altar.p3d";
	};
	class msfz_stone_altar_4_NoActive: HouseNoDestruct
	{
		scope = 1;
		hiddenSelections[] = {"camo","stihiya"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_red.paa","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_shar.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_NoActive.rvmat","MSFZ_Mifical_Pack\Static\Stone_3\data\MSFZ_Stihiy_Stone_2_NoActive.rvmat"};
		model = "MSFZ_Mifical_Pack\Static\Stone_3\msfz_stone_altar.p3d";
	};
};
