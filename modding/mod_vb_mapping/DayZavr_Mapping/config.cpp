// DayZavr_Mapping — маппинг сервера из файлов .map (экспорт DayZ Editor в формате Expansion).
// Мод клиент-серверный: скрытие объектов карты и статичные модели (.p3d) делаются на обеих сторонах.

class CfgPatches
{
	class DayZavr_Mapping
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts"};
	};
};

class CfgMods
{
	class DayZavr_Mapping
	{
		dir = "DayZavr_Mapping";
		hideName = 1;
		hidePicture = 1;
		name = "DayZavr_Mapping";
		credits = "DayZavr";
		author = "DayZavr";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = {"World","Mission"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"DayZavr_Mapping/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"DayZavr_Mapping/scripts/5_Mission"};
			};
		};
	};
};

// Список файлов маппинга (исходники — папка Map рядом с модом). Новый файл: положить в data\ и добавить класс.
// world — мир, на котором файл применяется (имя мира, как его отдаёт GetWorldName); пусто — на любом.
// Чтобы временно отключить файл, закомментировать его класс.
class CfgDayZavrMapping
{
	class FullMapping
	{
		file = "DayZavr_Mapping/data/FullMapping.map";
		world = "";
	};
	class VS_Base
	{
		file = "DayZavr_Mapping/data/Mapping_VS_Base.map";
		world = "";
	};
	class Korabli_Prok
	{
		file = "DayZavr_Mapping/data/Maping_korabli_Prok.map";
		world = "";
	};
	class BunkerYanow_Prok
	{
		file = "DayZavr_Mapping/data/Maping_BunkerYanow_Prok.map";
		world = "";
	};
	class Kapachi_Prok
	{
		file = "DayZavr_Mapping/data/Maping_Kapachi_Prok.map";
		world = "";
	};
	class Gradirni
	{
		file = "DayZavr_Mapping/data/Mapping_Gradirni.map";
		world = "";
	};
	class Oasis
	{
		file = "DayZavr_Mapping/data/Mapping_Oasis.map";
		world = "";
	};
	class Doroga_Limansk
	{
		file = "DayZavr_Mapping/data/Doroga_Limansk.map";
		world = "";
	};
	class AOD_ChNPP
	{
		file = "DayZavr_Mapping/data/aod_chnpp_003.map";
		world = "";
	};
	class AOD_Radar_Pripyat
	{
		file = "DayZavr_Mapping/data/aod_radar_pripyat_110.map";
		world = "";
	};
	class AOD_Rebra
	{
		file = "DayZavr_Mapping/data/aod_rebra.map";
		world = "";
	};
	class AOD_Zaton
	{
		file = "DayZavr_Mapping/data/aod_zaton_108.map";
		world = "";
	};
};
