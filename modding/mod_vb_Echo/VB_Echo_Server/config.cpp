// Серверная часть семейства VB_Echo (2026-11-26, «чтобы не украли мод»): то, без чего Эхо бесполезно и что выполняется
// только на сервере — энергосеть баз (выработка, печь, преобразователь, накопители, зарядка, курсор), пропуск в полёт
// и подушку Echo Runner с расходом баков, включение сервопривода экзо с расходом ячеек. Нагрузка та же: этот код и раньше
// работал только на сервере, просто лежал в клиентских PBO. Клиентские моды оставляют на этих местах пустые «крючки».
// Ставится только на сервер (-serverMod), игрокам не отдаётся.
class CfgPatches
{
	class VB_Echo_Server
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"VB_EchoCore", "VB_EchoBase", "VB_EchoClothes", "VB_EchoRunner", "VB_EchoVehicleParts"};
	};
};

class CfgMods
{
	class VB_Echo_Server
	{
		dir = "VB_Echo_Server";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VB_Echo_Server";
		credits = "";
		author = "DayZavr";
		authorID = "";
		version = "0.1";
		extra = 0;
		type = "mod";
		dependencies[] = {"World"};

		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_Echo_Server/scripts/4_World"};
			};
		};
	};
};
