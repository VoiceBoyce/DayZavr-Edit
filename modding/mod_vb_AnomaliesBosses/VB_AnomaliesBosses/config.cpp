// VB_AnomaliesBosses — «Хозяева аномалий». ТЗ: modding/mod_vb_AnomaliesBosses/ТЗ.md
// Префикс PBO короткий (VB_AB): пути текстур внутри перенесённых моделей должны влезть в длину исходных путей.

// Ванильное здоровье босса: огромный запас и нет смерти «в голову». Настоящее здоровье — запас из bosses.json
// (Health), его считает скрипт VB_AB_BossBase. Зоны и коэффициенты брони — как у ванильного ZombieBase.
#define VB_AB_HP 100000
#define VB_AB_BOSS_DAMAGE \
	class DamageSystem \
	{ \
		class GlobalHealth \
		{ \
			class Health \
			{ \
				hitpoints = VB_AB_HP; \
				healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.0,{}}}; \
			}; \
		}; \
		class DamageZones \
		{ \
			class Head \
			{ \
				class Health { hitpoints = VB_AB_HP; transferToGlobalCoef = 1; }; \
				class ArmorType \
				{ \
					class Projectile { class Health { damage = 1.5; }; }; \
					class Melee { class Health { damage = 0.7; }; }; \
					class FragGrenade { class Health { damage = 3; }; }; \
				}; \
				componentNames[] = {"Head","Neck"}; \
				fatalInjuryCoef = -1; \
				inventorySlots[] = {"Headgear","Mask"}; \
			}; \
			class Torso \
			{ \
				class Health { hitpoints = VB_AB_HP; transferToGlobalCoef = 1; }; \
				class ArmorType \
				{ \
					class Melee { class Health { damage = 1.2; }; }; \
					class FragGrenade { class Health { damage = 2; }; }; \
					class Projectile { class Health { damage = 1.42; }; }; \
				}; \
				componentNames[] = {"Spine1","Spine3"}; \
				fatalInjuryCoef = -1; \
				inventorySlots[] = {"Vest","Body","Back"}; \
				inventorySlotsCoefs[] = {1.0,1.0,0.9}; \
			}; \
			class LeftArm: Torso { componentNames[] = {"LeftArm","LeftForeArm"}; inventorySlots[] = {"Vest","Body"}; inventorySlotsCoefs[] = {1.0,1.0}; }; \
			class RightArm: LeftArm { componentNames[] = {"RightArm","RightForeArm"}; }; \
			class LeftLeg \
			{ \
				class Health { hitpoints = VB_AB_HP; transferToGlobalCoef = 0.5; }; \
				class ArmorType { class FragGrenade { class Health { damage = 2; }; }; }; \
				componentNames[] = {"LeftLeg","LeftUpLeg"}; \
				fatalInjuryCoef = -1; \
				inventorySlots[] = {"Legs"}; \
			}; \
			class RightLeg: LeftLeg { componentNames[] = {"RightLeg","RightUpLeg"}; }; \
			class LeftFoot \
			{ \
				class Health { hitpoints = VB_AB_HP; transferToGlobalCoef = 0.1; }; \
				class ArmorType { class FragGrenade { class Health { damage = 2; }; }; }; \
				transferToZonesNames[] = {"LeftLeg"}; \
				transferToZonesCoefs[] = {0.1}; \
				componentNames[] = {"LeftFoot"}; \
				fatalInjuryCoef = -1; \
				inventorySlots[] = {"Feet"}; \
			}; \
			class RightFoot: LeftFoot { transferToZonesNames[] = {"RightLeg"}; componentNames[] = {"RightFoot"}; }; \
		}; \
	};

class CfgPatches
{
	class VB_AnomaliesBosses
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Characters_Zombies", "DZ_Weapons_Melee", "DZ_Gear_Consumables"};
	};
};

// Звуки аномалий и сигналы детекторов (генерирует tools/ab_anomalies.py)
class CfgSounds
{
#include "cfg\sounds.hpp"
};

class CfgSoundShaders
{
	// находка детектором появилась (artefact_spawn OOTS)
	class VB_AB_Found_SoundShader
	{
		samples[] = {{"VB_AB\s\d\artefact_spawn", 1}};
		volume = 1.0;
		range = 30;
		rangeCurve[] = {{0, 1}, {10, 0.7}, {30, 0}};
	};
	// писк для чужих детекторов (Signal Generic, VB_AB_GenericDetectors.c)
	class VB_AB_DetectorBeep_SoundShader
	{
		samples[] = {{"VB_AB\s\d\detector_middle", 1}};
		volume = 1.0;
		range = 15;
		rangeCurve[] = {{0, 1}, {5, 0.6}, {15, 0}};
	};
};

class CfgSoundSets
{
	class VB_AB_Found_SoundSet
	{
		soundShaders[] = {"VB_AB_Found_SoundShader"};
		spatial = 1;
		doppler = 0;
		loop = 0;
		volumeCurve = "LinearCurve";
		sound3DProcessingType = "ObjectsInner13DProcessingType";
	};
	class VB_AB_DetectorBeep_SoundSet
	{
		soundShaders[] = {"VB_AB_DetectorBeep_SoundShader"};
		spatial = 1;
		doppler = 0;
		loop = 0;
		volumeCurve = "LinearCurve";
		sound3DProcessingType = "ObjectsInner13DProcessingType";
	};
};

// Урон аномалий (генерирует tools/ab_anomalies.py). Свой тип урона: броня его не гасит, как и в OOTS.
class CfgAmmo
{
	class MeleeDamage;
#include "cfg\ammo.hpp"
};

class CfgMods
{
	class VB_AnomaliesBosses
	{
		dir = "VB_AnomaliesBosses";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 0;
		name = "VB_AnomaliesBosses";
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
				files[] = {"VB_AB/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_AB/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_AB/scripts/5_Mission"};
			};
		};
	};
};

class CfgVehicles
{
	class ZmbM_HermitSkinny_Base;
	class HouseNoDestruct;
	class Inventory_Base;

	// ---- Аномалии (ТЗ 2.1). Пустая модель, всё видимое — частицы, свет и звук.
	class VB_AB_AnomalyBase: HouseNoDestruct
	{
		scope = 1;
		model = "\DZ\data\empty.p3d";
	};
#include "cfg\anomalies.hpp"

	// ---- Детекторы (ТЗ 2.2). Модели OOTS с заменёнными путями (tools/ab_models.py).
	// ---- Трофеи и материалы аномалий (свои генераторы в tools/)
#include "cfg\trophies.hpp"
#include "cfg\materials.hpp"

	class VB_AB_DetectorBase: Inventory_Base
	{
		scope = 0;
		rotationFlags = 0;
		weight = 250;
		itemSize[] = {1, 2};
		attachments[] = {"BatteryD"};
		class AnimationSources
		{
			class top
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
		};
		class EnergyManager
		{
			hasIcon = 1;
			autoSwitchOffWhenInCargo = 1;
			energyUsagePerSecond = 0.01;
			plugType = 1;
			attachmentAction = 1;
			updateInterval = 1;
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Attack_flashlight_Light1
				{
					soundSet = "Char_Attack_pipemetal_light1_SoundSet";
					id = 806;
				};
				class Attack_flashlight_Light2
				{
					soundSet = "Char_Attack_pipemetal_light2_SoundSet";
					id = 807;
				};
				class Attack_flashlight_Heavy1
				{
					soundSet = "Char_Attack_pipemetal_heavy1_SoundSet";
					id = 808;
				};
				class Attack_flashlight_Heavy2
				{
					soundSet = "Char_Attack_pipemetal_heavy2_SoundSet";
					id = 809;
				};
				class compass_open
				{
					soundSet = "compass_open_SoundSet";
					id = 204;
				};
				class compass_close
				{
					soundSet = "compass_close_SoundSet";
					id = 205;
				};
			};
		};
	};
	class VB_AB_Detector_Otklik: VB_AB_DetectorBase
	{
		scope = 2;
		displayName = "$STR_VB_AB_Detector_Otklik";
		descriptionShort = "$STR_VB_AB_Detector_Otklik_Desc";
		model = "VB_AB\m\vb_detector_otklik_x.p3d";
		hiddenSelections[] = {"zbytek", "Lampa", "glass"};
		hiddenSelectionsTextures[] =
		{
			"VB_AB\t\dox\oklik_defaultl_co__________________________________________.paa",
			"VB_AB\t\dox\lampa_defaultl_empty_ca__________________________________________.paa",
			"VB_AB\t\dox\glass_defaultl_ca__________________________________________.paa"
		};
		hiddenSelectionsMaterials[] =
		{
			"VB_AB\t\dox\oklik_adef__________________________________________.rvmat",
			"VB_AB\t\dox\oklik_lampa__________________________________________.rvmat",
			"VB_AB\t\dox\oklik_glass__________________________________________.rvmat"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 150;
					healthLevels[] =
					{
						{1.0, {"VB_AB\t\dox\oklik_adef__________________________________________.rvmat", "VB_AB\t\dox\oklik_lampa__________________________________________.rvmat", "VB_AB\t\dox\oklik_glass__________________________________________.rvmat"}},
						{0.7, {"VB_AB\t\dox\oklik_adef__________________________________________.rvmat", "VB_AB\t\dox\oklik_lampa__________________________________________.rvmat", "VB_AB\t\dox\oklik_glass__________________________________________.rvmat"}},
						{0.5, {"VB_AB\t\dox\oklik_adef__________________________________________.rvmat", "VB_AB\t\dox\oklik_lampa__________________________________________.rvmat", "VB_AB\t\dox\oklik_glass__________________________________________.rvmat"}},
						{0.3, {"VB_AB\t\dox\oklik_adef__________________________________________.rvmat", "VB_AB\t\dox\oklik_lampa__________________________________________.rvmat", "VB_AB\t\dox\oklik_glass__________________________________________.rvmat"}},
						{0.0, {"DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat"}}
					};
				};
			};
		};
	};
#include "cfg\detectors.hpp"

	// База всех боссов. Секции и материалы ванильного отшельника сбрасываем: у Энта и Ричара в модели
	// есть своя секция camo, и ванильный hermit.rvmat лёг бы на неё.
	class VB_AB_BossBase: ZmbM_HermitSkinny_Base
	{
		scope = 0;
		displayName = "VB_AB_BossBase";
		hiddenSelections[] = {};
		hiddenSelectionsTextures[] = {};
		hiddenSelectionsMaterials[] = {};
		vbABScale = 1.0;
		vbABHealth = 3000;		// запас здоровья, если босс поставлен не из bosses.json
		VB_AB_BOSS_DAMAGE
	};

	// Тестовые копии моделей этапа 0 (VB_AB_Test_*, скины x/r) убраны 2026-11-12: этап закрыт, их делает
	// tools/ab_models.py build <код> x, если понадобятся снова.
	// ---- Скины боссов (tools/ab_skins.py): наследники VB_AB_BossBase, поэтому в конце
#include "cfg\bosses.hpp"
};
