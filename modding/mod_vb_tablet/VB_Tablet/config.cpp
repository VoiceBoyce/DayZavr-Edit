// VB_Tablet: интерфейс и клиентские скрипты. Стоит у игроков и на сервере.
class CfgPatches
{
	class VB_Tablet
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "INT_UIFramework", "VB_Tablet_Data"};
	};
};

class CfgMods
{
	class VB_Tablet
	{
		dir = "VB_Tablet";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 0;
		name = "VB_Tablet";
		credits = "";
		author = "DayZavr";
		authorID = "";
		version = "1";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};
		inputs = "VB_Tablet/inputs.xml";

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"VB_Tablet/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_Tablet/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_Tablet/scripts/5_Mission"};
			};
		};
	};
};
