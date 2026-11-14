// INT Quests (DAYZAVR QUESTS) — server part. Loaded with -serverMod only (spec.md §4).
// INT_Banking is a SOFT dependency: its API is used inside #ifdef INT_BANKING_SE (defined only when INT_Banking_SE
// is loaded) and it is deliberately NOT in requiredAddons.

class CfgPatches
{
	class INT_Quests_SE
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "INT_Quests_CL"};
	};
};

class CfgMods
{
	class INT_Quests_SE
	{
		type = "mod";
		dir = "INT_Quests_SE";
		name = "DayZavr Quest System - server";
		author = "Intenso";
		version = "1.0.0";
		dependencies[] = {"Game", "World", "Mission"};
		defines[] = {"INT_QUESTS_SE"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_Quests_SE/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"INT_Quests_SE/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_Quests_SE/scripts/5_Mission"};
			};
		};
	};
};
