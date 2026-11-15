// Серверная часть VB_Rift «Изнанка»: гнёзда (nests.json, settings.json, state.json), редактор гнёзд (сервер),
// вырезание сердца, волны зомби, лут, протухание и заряды предметов, логи [VB_Rift].
// Ставится только на сервер (-serverMod), игрокам не отдаётся. Клиентский аддон VB_Rift (модели, частицы, звуки,
// классы предметов, эффекты, интерфейс редактора) нужен и серверу, и игрокам.
class CfgPatches
{
	class VB_Rift_Server
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_Rift"};
	};
};

class CfgMods
{
	class VB_Rift_Server
	{
		dir = "VB_Rift_Server";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VB_Rift_Server";
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
				files[] = {"VB_Rift_Server/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_Rift_Server/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_Rift_Server/scripts/5_Mission"};
			};
		};
	};
};
