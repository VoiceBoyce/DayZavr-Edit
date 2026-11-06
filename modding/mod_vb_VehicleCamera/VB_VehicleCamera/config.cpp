class CfgPatches
{
	class VB_VehicleCamera
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "INT_UIFramework"};
	};
};

class CfgMods
{
	class VB_VehicleCamera
	{
		dir = "VB_VehicleCamera";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 0;
		name = "VB_VehicleCamera";
		credits = "";
		author = "DayZavr";
		authorID = "";
		version = "1";
		extra = 0;
		type = "mod";
		dependencies[] = {"World", "Mission"};
		inputs = "VB_VehicleCamera/inputs.xml";

		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_VehicleCamera/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_VehicleCamera/scripts/5_Mission"};
			};
		};
	};
};
