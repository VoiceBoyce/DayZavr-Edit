class CfgPatches
{
	class VB_Rift
	{
		units[] = {
			"VB_RiftHeart",
			"VB_RiftRoots_Crawl1", "VB_RiftRoots_Crawl2", "VB_RiftRoots_Fork1",
			"VB_RiftRoots_Cluster1", "VB_RiftRoots_Cluster2", "VB_RiftRoots_Arch1", "VB_RiftRoots_Arch2",
			"VB_RiftRoots_Small1", "VB_RiftRoots_Small2", "VB_RiftCracks1", "VB_RiftCracks2"
		};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
	};
};

class CfgMods
{
	class VB_Rift
	{
		dir = "VB_Rift";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 0;
		name = "VB_Rift";
		credits = "";
		author = "DayZavr";
		authorID = "";
		version = "0.2";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game", "World"};

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"VB_Rift/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_Rift/scripts/4_World"};
			};
		};
	};
};

// Сердцебиение Изнанки: низкое «тук-тук», три варианта (tools/gen_rift_sounds.py).
// Запускает клиентский VB_RiftPulse ночью у сердец ближе 50 м. Настройки 3D как у ванильной SpookyArea.
class CfgSoundShaders
{
	class VB_RiftHeartbeat_SoundShader
	{
		samples[] = {{"VB_Rift\sounds\vb_rift_heartbeat_1", 1}, {"VB_Rift\sounds\vb_rift_heartbeat_2", 1}, {"VB_Rift\sounds\vb_rift_heartbeat_3", 1}};
		volume = 0.9;
		range = 45;
		rangeCurve[] = {{0, 1}, {12, 0.7}, {30, 0.25}, {45, 0}};
	};
};

class CfgSoundSets
{
	class VB_RiftHeartbeat_SoundSet
	{
		soundShaders[] = {"VB_RiftHeartbeat_SoundShader"};
		spatial = 1;
		doppler = 0;
		loop = 0;
		volumeCurve = "LinearCurve";
		sound3DProcessingType = "ObjectsInner13DProcessingType";
	};
};

class CfgVehicles
{
	class HouseNoDestruct;

	// Объекты Изнанки. Начало координат модели = уровень земли (у корней — место выхода из земли).
	// Пульс свечения и свет — клиентский скрипт (VB_RiftPulse). Модели генерирует tools/gen_rift_veins.py.
	class VB_RiftBase: HouseNoDestruct
	{
		scope = 0;
		displayName = "Rift";
		vbAlignToTerrain = 1;	// скрипт наклоняет объект по склону, если наклон не задан вручную
		vbAlignRadius = 3.0;	// шаг сетки замера рельефа, м
		hiddenSelections[] = {"crust", "veins"};
		hiddenSelectionsTextures[] = {"VB_Rift\data\vb_rift_crust_co.paa", "VB_Rift\data\vb_rift_vein_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Rift\data\vb_rift_crust.rvmat", "VB_Rift\data\vb_rift_vein.rvmat"};
	};

	// Сердце: узел с корнями ~12 м, коллизия у узла, красный свет.
	class VB_RiftHeart: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Heart";
		model = "\VB_Rift\data\vb_rift_heart.p3d";
	};

	// Корни без сердца: проходимы, без света; рядом с сердцем (до 30 м) бьются в его ритме.
	class VB_RiftRoots_Crawl1: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Crawl 1";
		model = "\VB_Rift\data\vb_rift_roots_crawl1.p3d";
	};
	class VB_RiftRoots_Crawl2: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Crawl 2";
		model = "\VB_Rift\data\vb_rift_roots_crawl2.p3d";
	};
	class VB_RiftRoots_Fork1: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Fork 1";
		model = "\VB_Rift\data\vb_rift_roots_fork1.p3d";
	};
	class VB_RiftRoots_Cluster1: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Cluster 1";
		model = "\VB_Rift\data\vb_rift_roots_cluster1.p3d";
	};
	class VB_RiftRoots_Cluster2: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Cluster 2";
		model = "\VB_Rift\data\vb_rift_roots_cluster2.p3d";
	};
	class VB_RiftRoots_Arch1: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Arch 1";
		model = "\VB_Rift\data\vb_rift_roots_arch1.p3d";
	};
	class VB_RiftRoots_Arch2: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Arch 2";
		model = "\VB_Rift\data\vb_rift_roots_arch2.p3d";
	};
	class VB_RiftRoots_Small1: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Small 1";
		model = "\VB_Rift\data\vb_rift_roots_small1.p3d";
	};
	class VB_RiftRoots_Small2: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Small 2";
		model = "\VB_Rift\data\vb_rift_roots_small2.p3d";
	};

	// Только светящиеся трещины в земле — самый дешёвый вариант для россыпи.
	class VB_RiftCracks1: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Cracks 1";
		model = "\VB_Rift\data\vb_rift_cracks1.p3d";
	};
	class VB_RiftCracks2: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Cracks 2";
		model = "\VB_Rift\data\vb_rift_cracks2.p3d";
	};};
