// VB_Tablet_Data: значки, корпус, стекло, модель банкомата. Меняется редко, поэтому отдельным PBO.
class CfgPatches
{
	class VB_Tablet_Data
	{
		units[] = {"VB_TabletAtm"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};

class CfgModels
{
	class vb_atm
	{
		sections[] = {"everything"};
	};
};

class CfgVehicles
{
	class HouseNoDestruct;
	// Банкомат клана. Создаёт только сервер из файла клана, в хранилище мира не попадает.
	class VB_TabletAtm : HouseNoDestruct
	{
		scope = 1;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_VB_TAB_C_ATM_NAME";
		descriptionShort = "$STR_VB_TAB_C_ATM_DESC";
		model = "\VB_Tablet_Data\models\vb_atm.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"VB_Tablet_Data\data\atmblack_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Tablet_Data\data\atm.rvmat"};
	};
	// Голограмма: локальный объект на клиенте, без физики и без действий.
	class VB_TabletAtmGhost : HouseNoDestruct
	{
		scope = 1;
		storageCategory = 10;
		displayName = "$STR_VB_TAB_C_ATM_NAME";
		model = "\VB_Tablet_Data\models\vb_atm.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"#(argb,8,8,3)color(0.5,0.5,0.5,0.75,ca)"};
		hiddenSelectionsMaterials[] = {"VB_Tablet_Data\data\vb_atm_ghost.rvmat"};
	};
};
