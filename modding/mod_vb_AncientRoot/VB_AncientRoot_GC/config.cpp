// VB_AncientRoot_GC: команда /grove в окне общего чата INT_GlobalChat. Отдельный PBO в папке @VB_AncientRoot\Addons.
// Только для серверов, где стоит @INT_GlobalChat_CL: без него игра не запустится и потребует этот аддон.
// Мод чата не меняется, перехват через modded class. После обновления чата аддон проверить сборкой.
class CfgPatches
{
	class VB_AncientRoot_GC
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_AncientRoot", "INT_GlobalChat_CL"};
	};
};

class CfgMods
{
	class VB_AncientRoot_GC
	{
		dir = "VB_AncientRoot_GC";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VB_AncientRoot_GC";
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
				files[] = {"VB_AncientRoot_GC/scripts/5_Mission"};
			};
		};
	};
};
