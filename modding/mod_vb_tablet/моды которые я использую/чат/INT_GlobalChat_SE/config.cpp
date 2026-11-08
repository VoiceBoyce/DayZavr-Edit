// INT GlobalChat — server part. Loaded with -serverMod only (spec.md §3.1).

class CfgPatches
{
	class INT_GlobalChat_SE
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "INT_GlobalChat_CL"};
	};
};

class CfgMods
{
	class INT_GlobalChat_SE
	{
		type = "mod";
		dir = "INT_GlobalChat_SE";
		name = "INT GlobalChat - server";
		author = "Intenso";
		version = "1.0.0";
		dependencies[] = {"Game", "Mission"};
		defines[] = {"INT_GLOBALCHAT_SE"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_GlobalChat_SE/scripts/3_Game"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_GlobalChat_SE/scripts/5_Mission"};
			};
		};
	};
};
