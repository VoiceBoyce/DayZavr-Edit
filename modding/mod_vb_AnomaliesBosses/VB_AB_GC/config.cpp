// VB_AB_GC: команда /boss в окне общего чата INT_GlobalChat. Отдельный PBO в папке @VB_AnomaliesBosses\Addons.
// Только для серверов, где стоит @INT_GlobalChat_CL: без него игра не запустится и потребует этот аддон.
// Мод чата не меняется, перехват через modded class. После обновления чата аддон проверить сборкой.
class CfgPatches
{
	class VB_AB_GC
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_AnomaliesBosses", "INT_GlobalChat_CL"};
	};
};

class CfgMods
{
	class VB_AB_GC
	{
		dir = "VB_AB_GC";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VB_AB_GC";
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
				files[] = {"VB_AB_GC/scripts/5_Mission"};
			};
		};
	};
};
