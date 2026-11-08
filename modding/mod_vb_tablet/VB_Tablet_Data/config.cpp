// VB_Tablet_Data: значки, корпус, стекло. Меняется редко, поэтому отдельным PBO.
// Модели банкомата клана здесь больше нет: склад клана работает в банкоматах INT_Banking (аддон VB_Tablet_BANK).
class CfgPatches
{
	class VB_Tablet_Data
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
