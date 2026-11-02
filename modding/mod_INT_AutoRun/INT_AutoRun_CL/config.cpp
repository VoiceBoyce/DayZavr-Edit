// INT AutoRun - client part (loaded with -mod on clients AND on the server).
// Autorun controller, HUD pill, handshake hello. Server-only logic lives in INT_AutoRun_SE.
class CfgPatches
{
	class INT_AutoRun_CL
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "INT_UIFramework"};
	};
};

class CfgMods
{
	class INT_AutoRun_CL
	{
		type = "mod";
		dir = "INT_AutoRun_CL";
		name = "INT AutoRun (Client)";
		author = "Intenso";
		version = "1.0.0";
		inputs = "INT_AutoRun_CL/inputs.xml";
		dependencies[] = {"Game", "World", "Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_AutoRun_CL/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"INT_AutoRun_CL/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_AutoRun_CL/scripts/5_Mission"};
			};
		};
	};
};
