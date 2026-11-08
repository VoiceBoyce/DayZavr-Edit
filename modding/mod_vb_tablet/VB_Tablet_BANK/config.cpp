// VB_Tablet_BANK: склад клана в окне банкомата INT_Banking. Отдельный PBO в папке @VB_Tablet\Addons.
// Только для серверов, где стоит @INT_Banking_CL: без него игра не запустится и потребует этот аддон.
// Мод банка не меняется, окно расширяется через modded class. После обновления банка дополнение проверить сборкой.
class CfgPatches
{
	class VB_Tablet_BANK
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_Tablet", "INT_Banking_CL"};
	};
};

class CfgMods
{
	class VB_Tablet_BANK
	{
		dir = "VB_Tablet_BANK";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VB_Tablet_BANK";
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
				files[] = {"VB_Tablet_BANK/scripts/5_Mission"};
			};
		};
	};
};
