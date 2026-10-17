// INT_UIFramework - shared UI framework for Intenso mods.
// Client-side by behaviour, but loaded with -mod on BOTH sides: a mod that
// lists it in requiredAddons is compiled by the server too.
class CfgPatches
{
	class INT_UIFramework
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
	};
};

class CfgMods
{
	class INT_UIFramework
	{
		type = "mod";
		dir = "INT_UIFramework";
		name = "INT UI Framework";
		author = "Intenso";
		version = "2.0.0";
		inputs = "INT_UIFramework/inputs.xml";
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_UIFramework/scripts/3_Game"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_UIFramework/scripts/5_Mission"};
			};
		};
	};
};
