// VB_Tablet_DRL: крафт DRL_CraftMenu в контрактах планшета (только сервер). Отдельный PBO рядом с VB_Tablet_SE.pbo.
// Только для серверов, где стоит DRL_CraftMenu_Server: без него сервер не запустится и потребует этот аддон.
// Мод крафта не меняется, завершение крафта перехватывается через modded class.
class CfgPatches
{
	class VB_Tablet_DRL
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_Tablet_SE", "DRL_CraftMenu_Server"};
	};
};

class CfgMods
{
	class VB_Tablet_DRL
	{
		dir = "VB_Tablet_DRL";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VB_Tablet_DRL";
		credits = "";
		author = "DayZavr";
		authorID = "";
		version = "1";
		extra = 0;
		type = "mod";
		dependencies[] = {"World", "Mission"};

		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_Tablet_DRL/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_Tablet_DRL/scripts/5_Mission"};
			};
		};
	};
};
