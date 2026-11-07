// VB_Tablet_SE: серверная логика. Только в -serverMod, игрокам не раздаётся.
class CfgPatches
{
	class VB_Tablet_SE
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_Tablet"};
	};
};

class CfgMods
{
	class VB_Tablet_SE
	{
		dir = "VB_Tablet_SE";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 0;
		name = "VB_Tablet_SE";
		credits = "";
		author = "DayZavr";
		authorID = "";
		version = "1";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"VB_Tablet_SE/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_Tablet_SE/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_Tablet_SE/scripts/5_Mission"};
			};
		};
	};
};
