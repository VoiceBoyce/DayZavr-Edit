class CfgPatches
{
	class VehicleDrownFix
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
	};
};

class CfgMods
{
	class VehicleDrownFix
	{
		dir = "VehicleDrownFix";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 0;
		name = "VehicleDrownFix";
		credits = "";
		author = "DayZavr";
		authorID = "";
		version = "1";
		extra = 0;
		type = "mod";
		dependencies[] = {"World"};

		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"VehicleDrownFix/scripts/4_World"};
			};
		};
	};
};
