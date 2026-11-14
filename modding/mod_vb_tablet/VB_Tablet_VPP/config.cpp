// VB_Tablet_VPP: связка планшета с картой INT_VppMap (@INT_VppMap_CL, форк VanillaPlusPlusMap; аддон карты по-прежнему
// VanillaPlusPlus). Отдельный PBO в папке @VB_Tablet\Addons. Только для серверов, где стоит эта карта: без неё игра не
// запустится и потребует этот аддон. Со старой @VanillaPlusPlusMap не собирается (там нет классов INT).
// Мод карты не меняется, всё через modded class.
class CfgPatches
{
	class VB_Tablet_VPP
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_Tablet", "VanillaPlusPlus"};
	};
};

class CfgMods
{
	class VB_Tablet_VPP
	{
		dir = "VB_Tablet_VPP";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VB_Tablet_VPP";
		credits = "";
		author = "DayZavr";
		authorID = "";
		version = "1";
		extra = 0;
		type = "mod";
		dependencies[] = {"Mission"};

		class defs
		{
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_Tablet_VPP/scripts/5_Mission"};
			};
		};
	};
};
