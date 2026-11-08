// INT Banking - server part (server only: -serverMod / @Servermod).
// Accounts, config, admin log, all money operations. Without it the client part answers every
// request with "server part not installed". Other server mods test for it with #ifdef INT_BANKING_SE.
class CfgPatches
{
	class INT_Banking_SE
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "INT_Banking_CL"};
	};
};

class CfgMods
{
	class INT_Banking_SE
	{
		type = "mod";
		dir = "INT_Banking_SE";
		name = "INT Banking (Server)";
		author = "Intenso";
		version = "1.0.1";
		dependencies[] = {"Game", "World", "Mission"};
		defines[] = {"INT_BANKING_SE"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_Banking_SE/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"INT_Banking_SE/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_Banking_SE/scripts/5_Mission"};
			};
		};
	};
};
