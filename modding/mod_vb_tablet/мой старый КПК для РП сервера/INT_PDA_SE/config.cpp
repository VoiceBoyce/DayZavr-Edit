// INT_PDA — server part. Loaded with -serverMod only (spec.md §1). Accounts, chat routing, factions, admin, spawn.

class CfgPatches
{
	class INT_PDA_SE
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "INT_PDA_CL"};
	};
};

class CfgMods
{
	class INT_PDA_SE
	{
		type = "mod";
		dir = "INT_PDA_SE";
		name = "INT PDA - server";
		author = "Intenso";
		version = "1.0.0";
		dependencies[] = {"Game", "World", "Mission"};
		defines[] = {"INT_PDA_SE"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_PDA_SE/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"INT_PDA_SE/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_PDA_SE/scripts/5_Mission"};
			};
		};
	};
};
