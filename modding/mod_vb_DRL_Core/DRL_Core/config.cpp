class CfgPatches
{
	class DRL_Core
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data"
		};
	};
};
class CfgMods
{
	class DRL_Core
	{
		dir = "DRL_Core";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 1;
		name = "DRL_Core";
		credits = "";
		author = "Drulik";
		authorID = "0";
		version = "1";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"DRL_Core/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"DRL_Core/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"DRL_Core/scripts/5_Mission"};
			};
		};
	};
};
