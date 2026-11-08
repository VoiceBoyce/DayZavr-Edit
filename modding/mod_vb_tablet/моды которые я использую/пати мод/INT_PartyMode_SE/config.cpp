// INT PartyMode — server part. Loaded with -serverMod only (spec.md §3).

class CfgPatches
{
	class INT_PartyMode_SE
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "INT_PartyMode_CL"};
	};
};

class CfgMods
{
	class INT_PartyMode_SE
	{
		type = "mod";
		dir = "INT_PartyMode_SE";
		name = "INT PartyMode - server";
		author = "Intenso";
		version = "1.0.0";
		dependencies[] = {"Game", "Mission"};
		defines[] = {"INT_PARTYMODE_SE"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_PartyMode_SE/scripts/3_Game"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_PartyMode_SE/scripts/5_Mission"};
			};
		};
	};
};
