// VB_Tablet_PTY: кланы планшета и друзья INT_PartyMode (только сервер). Отдельный PBO рядом с VB_Tablet_SE.pbo.
// Только для серверов, где стоит INT_PartyMode_SE: без него сервер не запустится и потребует этот аддон.
// Мод пати не меняется, удаление дружбы вызывается через modded class.
class CfgPatches
{
	class VB_Tablet_PTY
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_Tablet_SE", "INT_PartyMode_SE"};
	};
};

class CfgMods
{
	class VB_Tablet_PTY
	{
		dir = "VB_Tablet_PTY";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VB_Tablet_PTY";
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
				files[] = {"VB_Tablet_PTY/scripts/5_Mission"};
			};
		};
	};
};
