// VB_Tablet_GC: вкладка «Клан» в окне общего чата INT_GlobalChat. Отдельный PBO в папке @VB_Tablet\Addons.
// Только для серверов, где стоит @INT_GlobalChat_CL: без него игра не запустится и потребует этот аддон.
// Мод чата не меняется, вкладка добавляется через modded class. После обновления чата дополнение проверить сборкой.
class CfgPatches
{
	class VB_Tablet_GC
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_Tablet", "INT_GlobalChat_CL"};
	};
};

class CfgMods
{
	class VB_Tablet_GC
	{
		dir = "VB_Tablet_GC";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VB_Tablet_GC";
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
				files[] = {"VB_Tablet_GC/scripts/5_Mission"};
			};
		};
	};
};
