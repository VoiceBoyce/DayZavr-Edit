// Серверная часть VB_AnomaliesBosses: вся логика боссов, полей, аномалий, находок, способностей и редактора.
// Ставится только на сервер (-serverMod), игрокам не отдаётся. Клиентский аддон VB_AnomaliesBosses
// (модели, частицы, звуки, классы предметов, интерфейс) нужен и серверу, и игрокам.
class CfgPatches
{
	class VB_AnomaliesBosses_Server
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_AnomaliesBosses"};
	};
};

class CfgMods
{
	class VB_AnomaliesBosses_Server
	{
		dir = "VB_AnomaliesBosses_Server";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VB_AnomaliesBosses_Server";
		credits = "";
		author = "DayZavr";
		authorID = "";
		version = "0.1";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"VB_AnomaliesBosses_Server/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_AnomaliesBosses_Server/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_AnomaliesBosses_Server/scripts/5_Mission"};
			};
		};
	};
};
