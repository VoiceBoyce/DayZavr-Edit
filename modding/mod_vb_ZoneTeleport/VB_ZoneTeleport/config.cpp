class CfgPatches
{
	class VB_ZoneTeleport
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
	};
};

class CfgMods
{
	class VB_ZoneTeleport
	{
		dir = "VB_ZoneTeleport";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 0;
		name = "VB_ZoneTeleport";
		credits = "";
		author = "DayZavr";
		authorID = "";
		version = "1";
		extra = 0;
		type = "mod";
		dependencies[] = {"Mission"};

		class defs
		{
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_ZoneTeleport/scripts/5_Mission"};
			};
		};
	};
};
