// Серверная часть VB_AncientRoot «Изнанка»: гнёзда (nests.json, settings.json, state.json), редактор гнёзд (сервер),
// вырезание сердца, волны зомби, лут, протухание и заряды предметов, логи [VB_AncientRoot].
// Ставится только на сервер (-serverMod), игрокам не отдаётся. Клиентский аддон VB_AncientRoot (модели, частицы, звуки,
// классы предметов, эффекты, интерфейс редактора) нужен и серверу, и игрокам.
class CfgPatches
{
	class VB_AncientRoot_Server
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_AncientRoot"};
	};
};

class CfgMods
{
	class VB_AncientRoot_Server
	{
		dir = "VB_AncientRoot_Server";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VB_AncientRoot_Server";
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
				files[] = {"VB_AncientRoot_Server/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_AncientRoot_Server/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_AncientRoot_Server/scripts/5_Mission"};
			};
		};
	};
};
