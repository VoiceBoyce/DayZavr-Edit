// VB_MiningResources_Server: серверная часть (кладётся в @Servermod\Addons сервера).
// Результаты добычи и промывки, пресс, плавка, настройки VB_Mods\VB_MiningResources\settings.json.
class CfgPatches
{
	class VB_MiningResources_Server
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts","VB_MiningResources"};
	};
};
class CfgMods
{
	class VB_MiningResources_Server
	{
		dir = "VB_MiningResources_Server";
		name = "VB_MiningResources_Server";
		author = "DayZavr";
		type = "mod";
		dependencies[] = {"World","Mission"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_MiningResources_Server/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_MiningResources_Server/scripts/5_Mission"};
			};
		};
	};
};
