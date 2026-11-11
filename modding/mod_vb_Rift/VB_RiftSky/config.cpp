// VB_RiftSky — красноватый оттенок неба Сахала в тучи и грозу («Изнанка»).
// Отдельный PBO внутри @VB_Rift: убрал VB_RiftSky.pbo — небо ванильное.
// 1) Облака уровней Weather4–6 подменены красноватыми копиями (tools/gen_rift_sky.py).
// 2) В наборах освещения «пасмурно» (O_*, overcast = 1) цвет неба и тумана чуть уведён в красный.
//    Движок смешивает их с «ясными» (C_*, overcast = 0.35) по облачности: в ясную погоду оттенка почти нет.
// 3) Молнии красные: копии ванильных blesk1/blesk2.p3d с перекрашенным встроенным материалом
//    (tools/gen_rift_lightning.py); звук грома ванильный.
// Имена классов мира сняты дампом конфига в DayZDiag (CfgWorlds sakhal: CAWorld, патч DZ_Worlds_Sakhal),
// исходные значения — P:\DZ\worlds\sakhal\data\lighting\lighting_sakhal.txt; молнии —
// CfgWorlds CAWorld Weather ThunderboltNorm/Heavy (P:\DZ\sounds\hpp\config.cpp, патч DZ_Sounds_Effects).

class CfgPatches
{
	class VB_RiftSky
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Sounds_Effects", "DZ_Worlds_Chernarusplus_World", "DZ_Worlds_Sakhal"};
	};
};

class CfgWorlds
{
	class DefaultWorld;
	class CAWorld: DefaultWorld
	{
		class Weather
		{
			class ThunderboltNorm;
			class ThunderboltHeavy;
			class Overcast
			{
				class Weather4;
				class Weather5;
				class Weather6;
			};
		};
	};

	class sakhal: CAWorld
	{
		class Weather: Weather
		{
			class ThunderboltNorm: ThunderboltNorm
			{
				model = "\VB_RiftSky\data\vb_blesk1.p3d";
			};
			class ThunderboltHeavy: ThunderboltHeavy
			{
				model = "\VB_RiftSky\data\vb_blesk2.p3d";
			};

			class Overcast: Overcast
			{
				class Weather4: Weather4
				{
					cloud = "VB_RiftSky\data\vb_Sky_Stage20_StratoCumulus_sakhal_sky.paa";
				};
				class Weather5: Weather5
				{
					cloud = "VB_RiftSky\data\vb_Sky_Stage21_StratoCumulus_sakhal_sky.paa";
				};
				class Weather6: Weather6
				{
					farCloud = "VB_RiftSky\data\vb_Sky_Stage30_Nimbostratus_sakhal_sky.paa";
					cloud = "VB_RiftSky\data\vb_Sky_Stage22_StratoCumulus_sakhal_sky.paa";
				};
			};

			class LightingNew
			{
				// ночь и сумерки: было {0.01,0.01,0.01}, {0.036,0.056,0.087}, {0.1,0.14,0.22}
				class O_Night
				{
					sky[] = {0.013, 0.008, 0.009};
					skyAroundSun[] = {0.013, 0.008, 0.009};
				};
				class O_Astro1
				{
					sky[] = {0.013, 0.008, 0.009};
					skyAroundSun[] = {0.013, 0.008, 0.009};
				};
				class O_Nautical1
				{
					sky[] = {0.05, 0.04, 0.055};
					skyAroundSun[] = {0.07, 0.05, 0.068};
				};
				class O_Civil1
				{
					sky[] = {0.14, 0.11, 0.15};
					skyAroundSun[] = {0.14, 0.11, 0.15};
				};
				class O_Civil2
				{
					sky[] = {0.14, 0.11, 0.15};
					skyAroundSun[] = {0.14, 0.11, 0.15};
				};
				class O_Day0
				{
					sky[] = {0.14, 0.11, 0.15};
					skyAroundSun[] = {0.14, 0.11, 0.15};
				};
				// день: цвет {{r,g,b}, EV}; было {0.7,0.8,1.0} и {1,1,1}, яркость (EV) прежняя
				class O_Day4
				{
					sky[] = {{0.85, 0.7, 0.82}, 4.415};
					skyAroundSun[] = {{0.9, 0.7, 0.8}, 4.799};
				};
				class O_Day8
				{
					sky[] = {{0.85, 0.7, 0.82}, 8.0};
					skyAroundSun[] = {{0.9, 0.7, 0.8}, 8.8};
				};
				class O_Day12
				{
					sky[] = {{1.0, 0.86, 0.88}, 9.0};
					skyAroundSun[] = {{1.0, 0.84, 0.86}, 9.8};
				};
				class O_Day35
				{
					sky[] = {{1.0, 0.86, 0.88}, 11};
					skyAroundSun[] = {{1.0, 0.84, 0.86}, 11.8};
				};
				class O_Day60
				{
					sky[] = {{1.0, 0.86, 0.88}, 12};
					skyAroundSun[] = {{1.0, 0.84, 0.86}, 12.8};
				};
			};
		};
	};
};
