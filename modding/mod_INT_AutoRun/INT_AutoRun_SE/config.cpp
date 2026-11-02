// INT AutoRun - server part (server only: -serverMod).
// Answers the client hello with an ack; prints the ready line.
// Without it the client refuses to run and the client part logs a server Error.
class CfgPatches
{
	class INT_AutoRun_SE
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "INT_AutoRun_CL"};
	};
};

class CfgMods
{
	class INT_AutoRun_SE
	{
		type = "mod";
		dir = "INT_AutoRun_SE";
		name = "INT AutoRun (Server)";
		author = "Intenso";
		version = "1.0.0";
		dependencies[] = {"Game", "World", "Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_AutoRun_SE/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"INT_AutoRun_SE/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_AutoRun_SE/scripts/5_Mission"};
			};
		};
	};
};
