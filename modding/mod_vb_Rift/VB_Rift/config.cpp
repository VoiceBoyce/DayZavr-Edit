// Здоровье зомби Изнанки: полная копия ванильного ZombieBase DamageSystem (P:\DZ\characters\zombies\config.cpp),
// только очки другие: GLOBAL — общее, LIMB — торс, руки, ноги, ступни, HEAD — голова. У ванили 100/100/30.
// Копия целиком, а не наследование: вложенные классы чужого аддона через «class X: X» здесь не достать.
#define VB_RIFT_ZMB_DAMAGE(GLOBAL,LIMB,HEAD) \
	class DamageSystem \
	{ \
		class GlobalHealth \
		{ \
			class Health \
			{ \
				hitpoints = GLOBAL; \
				healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.0,{}}}; \
			}; \
		}; \
		class DamageZones \
		{ \
			class Head \
			{ \
				class Health \
				{ \
					hitpoints = HEAD; \
					transferToGlobalCoef = 1; \
				}; \
				class ArmorType \
				{ \
					class Projectile { class Health { damage = 1.5; }; }; \
					class Melee { class Health { damage = 0.7; }; }; \
					class FragGrenade { class Health { damage = 3; }; }; \
				}; \
				componentNames[] = {"Head","Neck"}; \
				fatalInjuryCoef = 0.1; \
				inventorySlots[] = {"Headgear","Mask"}; \
			}; \
			class Torso \
			{ \
				class Health \
				{ \
					hitpoints = LIMB; \
					transferToGlobalCoef = 1; \
				}; \
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
			class LeftArm: Torso \
			{ \
				componentNames[] = {"LeftArm","LeftForeArm"}; \
				inventorySlots[] = {"Vest","Body"}; \
				inventorySlotsCoefs[] = {1.0,1.0}; \
				fatalInjuryCoef = -1; \
			}; \
			class RightArm: LeftArm \
			{ \
				componentNames[] = {"RightArm","RightForeArm"}; \
				inventorySlots[] = {"Vest","Body"}; \
				inventorySlotsCoefs[] = {1.0,1.0}; \
				fatalInjuryCoef = -1; \
			}; \
			class LeftLeg \
			{ \
				class Health \
				{ \
					hitpoints = LIMB; \
					transferToGlobalCoef = 0.5; \
				}; \
				class ArmorType \
				{ \
					class FragGrenade { class Health { damage = 2; }; }; \
				}; \
				componentNames[] = {"LeftLeg","LeftUpLeg"}; \
				fatalInjuryCoef = -1; \
				inventorySlots[] = {"Legs"}; \
			}; \
			class RightLeg: LeftLeg \
			{ \
				componentNames[] = {"RightLeg","RightUpLeg"}; \
				fatalInjuryCoef = -1; \
				inventorySlots[] = {"Legs"}; \
			}; \
			class LeftFoot \
			{ \
				class Health \
				{ \
					hitpoints = LIMB; \
					transferToGlobalCoef = 0.1; \
				}; \
				class ArmorType \
				{ \
					class FragGrenade { class Health { damage = 2; }; }; \
				}; \
				transferToZonesNames[] = {"LeftLeg"}; \
				transferToZonesCoefs[] = {0.1}; \
				componentNames[] = {"LeftFoot"}; \
				fatalInjuryCoef = -1; \
				inventorySlots[] = {"Feet"}; \
			}; \
			class RightFoot: LeftFoot \
			{ \
				transferToZonesNames[] = {"RightLeg"}; \
				transferToZonesCoefs[] = {0.1}; \
				componentNames[] = {"RightFoot"}; \
				fatalInjuryCoef = -1; \
				inventorySlots[] = {"Feet"}; \
			}; \
		}; \
	};

class CfgPatches
{
	class VB_Rift
	{
		units[] = {
			"VB_RiftHeart",
			"VB_RiftRoots_Crawl1", "VB_RiftRoots_Crawl2", "VB_RiftRoots_Fork1",
			"VB_RiftRoots_Cluster1", "VB_RiftRoots_Cluster2", "VB_RiftRoots_Arch1", "VB_RiftRoots_Arch2",
			"VB_RiftRoots_Small1", "VB_RiftRoots_Small2", "VB_RiftCracks1", "VB_RiftCracks2",
			"VB_RiftWrap_10", "VB_RiftWrap_15", "VB_RiftWrap_20", "VB_RiftWrap_27", "VB_RiftWrap_35", "VB_RiftWrap_45",
			"VB_RiftHang_Short1", "VB_RiftHang_Short2", "VB_RiftHang_Long1", "VB_RiftHang_Long2", "VB_RiftHang_Drape1",
			"VB_RiftHeartChunk", "VB_RiftRootPiece",
			"VB_RiftZmbM_VillagerOld", "VB_RiftZmbF_VillagerOld", "VB_RiftZmbM_Prisoner", "VB_RiftZmbM_Jacket",
			"VB_RiftZmbM_Hiker", "VB_RiftZmbM_Worker", "VB_RiftZmbM_Sailor",
			"VB_RiftZmbF_Patient", "VB_RiftZmbF_Citizen", "VB_RiftZmbF_Doctor"
		};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Characters_Zombies"};
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
	// гул-петля у сердца (включается вместе со спорами, VB_RiftBase.VB_SetHum)
	class VB_RiftHum_SoundShader
	{
		samples[] = {{"VB_Rift\sounds\vb_rift_hum", 1}};
		volume = 0.7;
		range = 40;
		rangeCurve[] = {{0, 1}, {10, 0.8}, {25, 0.35}, {40, 0}};
	};
	// скрип корня рядом с игроком (VB_RiftPulse.VB_PlayAmbient)
	class VB_RiftCreak_SoundShader
	{
		samples[] = {{"VB_Rift\sounds\vb_rift_creak_1", 1}, {"VB_Rift\sounds\vb_rift_creak_2", 1}, {"VB_Rift\sounds\vb_rift_creak_3", 1}};
		volume = 0.8;
		range = 40;
		rangeCurve[] = {{0, 1}, {15, 0.6}, {40, 0}};
	};
	// шёпот за спиной у самого сердца
	class VB_RiftWhisper_SoundShader
	{
		samples[] = {{"VB_Rift\sounds\vb_rift_whisper_1", 1}, {"VB_Rift\sounds\vb_rift_whisper_2", 1}, {"VB_Rift\sounds\vb_rift_whisper_3", 1}};
		volume = 0.55;
		range = 15;
		rangeCurve[] = {{0, 1}, {6, 0.6}, {15, 0}};
	};
	// зомби Изнанки лопается при смерти
	class VB_RiftBurst_SoundShader
	{
		samples[] = {{"VB_Rift\sounds\vb_rift_burst_1", 1}, {"VB_Rift\sounds\vb_rift_burst_2", 1}};
		volume = 1.0;
		range = 60;
		rangeCurve[] = {{0, 1}, {20, 0.5}, {60, 0}};
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
	class VB_RiftHum_SoundSet: VB_RiftHeartbeat_SoundSet
	{
		soundShaders[] = {"VB_RiftHum_SoundShader"};
		loop = 1;
	};
	class VB_RiftCreak_SoundSet: VB_RiftHeartbeat_SoundSet
	{
		soundShaders[] = {"VB_RiftCreak_SoundShader"};
	};
	class VB_RiftWhisper_SoundSet: VB_RiftHeartbeat_SoundSet
	{
		soundShaders[] = {"VB_RiftWhisper_SoundShader"};
	};
	class VB_RiftBurst_SoundSet: VB_RiftHeartbeat_SoundSet
	{
		soundShaders[] = {"VB_RiftBurst_SoundShader"};
	};
};

// Шум у сердца, пока его режут: зомби идут проверить (VB_RiftHorde.VB_CallNoise).
// Тип как у выстрелов и ракетницы: на "sound" (шаги, рык, strength 10) зомби волны не шли.
// Для сравнения: ракетница NoiseFlare 30, АКМ NoiseShoot 80.
class CfgNoises
{
	class VB_RiftCallNoise
	{
		type = "shot";
		strength = 50;
	};
};

class CfgVehicles
{
	class HouseNoDestruct;
	class Inventory_Base;

	// Зомби Изнанки: ванильные модели и поведение, свои текстуры и материалы (tools/gen_rift_zombies.py):
	// мертвенно-серая кожа в светящихся жилах, светящиеся красные глаза, одежда в тёмный бордовый.
	// Появляются волной, когда игрок начинает вырезать сердце (VB_RiftHorde); можно спавнить и как обычных зомби.
	// vbRiftZombie = 1 — скрипт VB_RiftZombie: при смерти гаснут (vbRiftDeadMaterial), лопаются спорами,
	// с шансом оставляют «Корень Изнанки». Здоровье вдвое больше ванильного (VB_RIFT_ZMB_DAMAGE).
	class ZmbM_VillagerOld_Base;
	class ZmbF_VillagerOld_Base;
	class ZmbM_PrisonerSkinny_Base;
	class ZmbM_Jacket_Base;
	class ZmbM_HikerSkinny_Base;
	class ZmbM_HandymanNormal_Base;
	class ZmbM_usSoldier_normal_Base;
	class ZmbF_PatientOld_Base;
	class ZmbF_CitizenANormal_Base;
	class ZmbF_DoctorSkinny_Base;
	class VB_RiftZmbM_VillagerOld: ZmbM_VillagerOld_Base
	{
		scope = 2;
		vbRiftZombie = 1;
		vbRiftDeadMaterial = "VB_Rift\data\zombies\vb_rift_zmb_villager_m_dead.rvmat";
		hiddenSelectionsTextures[] = {"VB_Rift\data\zombies\vb_rift_zmb_villager_m_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Rift\data\zombies\vb_rift_zmb_villager_m.rvmat"};
		VB_RIFT_ZMB_DAMAGE(200,200,60)
	};
	class VB_RiftZmbF_VillagerOld: ZmbF_VillagerOld_Base
	{
		scope = 2;
		vbRiftZombie = 1;
		vbRiftDeadMaterial = "VB_Rift\data\zombies\vb_rift_zmb_villager_f_dead.rvmat";
		hiddenSelectionsTextures[] = {"VB_Rift\data\zombies\vb_rift_zmb_villager_f_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Rift\data\zombies\vb_rift_zmb_villager_f.rvmat"};
		VB_RIFT_ZMB_DAMAGE(200,200,60)
	};
	class VB_RiftZmbM_Prisoner: ZmbM_PrisonerSkinny_Base
	{
		scope = 2;
		vbRiftZombie = 1;
		vbRiftDeadMaterial = "VB_Rift\data\zombies\vb_rift_zmb_prisoner_dead.rvmat";
		hiddenSelectionsTextures[] = {"VB_Rift\data\zombies\vb_rift_zmb_prisoner_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Rift\data\zombies\vb_rift_zmb_prisoner.rvmat"};
		VB_RIFT_ZMB_DAMAGE(200,200,60)
	};
	class VB_RiftZmbM_Jacket: ZmbM_Jacket_Base
	{
		scope = 2;
		vbRiftZombie = 1;
		vbRiftDeadMaterial = "VB_Rift\data\zombies\vb_rift_zmb_jacket_dead.rvmat";
		hiddenSelectionsTextures[] = {"VB_Rift\data\zombies\vb_rift_zmb_jacket_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Rift\data\zombies\vb_rift_zmb_jacket.rvmat"};
		VB_RIFT_ZMB_DAMAGE(200,200,60)
	};
	class VB_RiftZmbM_Hiker: ZmbM_HikerSkinny_Base
	{
		scope = 2;
		vbRiftZombie = 1;
		vbRiftDeadMaterial = "VB_Rift\data\zombies\vb_rift_zmb_hiker_dead.rvmat";
		hiddenSelectionsTextures[] = {"VB_Rift\data\zombies\vb_rift_zmb_hiker_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Rift\data\zombies\vb_rift_zmb_hiker.rvmat"};
		VB_RIFT_ZMB_DAMAGE(200,200,60)
	};
	class VB_RiftZmbM_Worker: ZmbM_HandymanNormal_Base
	{
		scope = 2;
		vbRiftZombie = 1;
		vbRiftDeadMaterial = "VB_Rift\data\zombies\vb_rift_zmb_worker_dead.rvmat";
		hiddenSelectionsTextures[] = {"VB_Rift\data\zombies\vb_rift_zmb_worker_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Rift\data\zombies\vb_rift_zmb_worker.rvmat"};
		VB_RIFT_ZMB_DAMAGE(200,200,60)
	};
	// моряк на солдатской базе: у ванильного солдата общее здоровье 115 -> 230
	class VB_RiftZmbM_Sailor: ZmbM_usSoldier_normal_Base
	{
		scope = 2;
		vbRiftZombie = 1;
		vbRiftDeadMaterial = "VB_Rift\data\zombies\vb_rift_zmb_sailor_dead.rvmat";
		hiddenSelectionsTextures[] = {"VB_Rift\data\zombies\vb_rift_zmb_sailor_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Rift\data\zombies\vb_rift_zmb_sailor.rvmat"};
		VB_RIFT_ZMB_DAMAGE(230,200,60)
	};
	class VB_RiftZmbF_Patient: ZmbF_PatientOld_Base
	{
		scope = 2;
		vbRiftZombie = 1;
		vbRiftDeadMaterial = "VB_Rift\data\zombies\vb_rift_zmb_patient_f_dead.rvmat";
		hiddenSelectionsTextures[] = {"VB_Rift\data\zombies\vb_rift_zmb_patient_f_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Rift\data\zombies\vb_rift_zmb_patient_f.rvmat"};
		VB_RIFT_ZMB_DAMAGE(200,200,60)
	};
	class VB_RiftZmbF_Citizen: ZmbF_CitizenANormal_Base
	{
		scope = 2;
		vbRiftZombie = 1;
		vbRiftDeadMaterial = "VB_Rift\data\zombies\vb_rift_zmb_citizen_f_dead.rvmat";
		hiddenSelectionsTextures[] = {"VB_Rift\data\zombies\vb_rift_zmb_citizen_f_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Rift\data\zombies\vb_rift_zmb_citizen_f.rvmat"};
		VB_RIFT_ZMB_DAMAGE(200,200,60)
	};
	class VB_RiftZmbF_Doctor: ZmbF_DoctorSkinny_Base
	{
		scope = 2;
		vbRiftZombie = 1;
		vbRiftDeadMaterial = "VB_Rift\data\zombies\vb_rift_zmb_doctor_f_dead.rvmat";
		hiddenSelectionsTextures[] = {"VB_Rift\data\zombies\vb_rift_zmb_doctor_f_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_Rift\data\zombies\vb_rift_zmb_doctor_f.rvmat"};
		VB_RIFT_ZMB_DAMAGE(200,200,60)
	};

	// Иногда выпадает из зомби Изнанки (VB_RiftZombie.ROOT_CHANCE, в инвентарь трупа). Пока без применения.
	// Для types.xml: nominal 0 — сам в лут не спавнится.
	class VB_RiftRootPiece: Inventory_Base
	{
		scope = 2;
		displayName = "Корень Изнанки";
		descriptionShort = "Обломок корня, проросшего в теле заражённого. Жилы в нём ещё слабо тлеют.";
		model = "\VB_Rift\data\vb_rift_root_piece.p3d";
		weight = 300;
		itemSize[] = {1, 3};
		rotationFlags = 17;
	};

	// Выпадает тому, кто вырезал сердце (действие «Вырезать сердце» ножом/топором). Пока без применения.
	// Для types.xml: nominal 0, lifetime по вкусу — сам предмет в лут не спавнится.
	class VB_RiftHeartChunk: Inventory_Base
	{
		scope = 2;
		displayName = "Сердце Изнанки";
		descriptionShort = "Тёплый сгусток, вырезанный из корней Изнанки. Слабо светится изнутри, будто ещё бьётся.";
		model = "\VB_Rift\data\vb_rift_heart_chunk.p3d";
		weight = 900;
		itemSize[] = {2, 2};
		rotationFlags = 17;
	};

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
	// На клиенте само надевает обвивки на ближайшие деревья (локальные объекты, по сети не идут).
	class VB_RiftHeart: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Heart";
		model = "\VB_Rift\data\vb_rift_heart.p3d";
		vbInfectRadius = 18.0;	// радиус заражения деревьев, м (0 — не заражать)
		vbInfectMax = 10;		// не больше стольких деревьев на одно сердце
	};

	// Обвивки стволов: число — радиус ствола в см. Обычно их ставит сердце само,
	// но можно поставить и вручную на любое дерево (начало координат — центр ствола у земли).
	class VB_RiftWrapBase: VB_RiftBase
	{
		scope = 0;
		vbAlignToTerrain = 0;
	};
	class VB_RiftWrap_10: VB_RiftWrapBase
	{
		scope = 2;
		displayName = "Rift Tree Wrap 10";
		model = "\VB_Rift\data\vb_rift_wrap_10.p3d";
	};
	class VB_RiftWrap_15: VB_RiftWrapBase
	{
		scope = 2;
		displayName = "Rift Tree Wrap 15";
		model = "\VB_Rift\data\vb_rift_wrap_15.p3d";
	};
	class VB_RiftWrap_20: VB_RiftWrapBase
	{
		scope = 2;
		displayName = "Rift Tree Wrap 20";
		model = "\VB_Rift\data\vb_rift_wrap_20.p3d";
	};
	class VB_RiftWrap_27: VB_RiftWrapBase
	{
		scope = 2;
		displayName = "Rift Tree Wrap 27";
		model = "\VB_Rift\data\vb_rift_wrap_27.p3d";
	};
	class VB_RiftWrap_35: VB_RiftWrapBase
	{
		scope = 2;
		displayName = "Rift Tree Wrap 35";
		model = "\VB_Rift\data\vb_rift_wrap_35.p3d";
	};
	class VB_RiftWrap_45: VB_RiftWrapBase
	{
		scope = 2;
		displayName = "Rift Tree Wrap 45";
		model = "\VB_Rift\data\vb_rift_wrap_45.p3d";
	};

	// Свисающие щупальца: ставятся вручную на мосты, скалы, провода. Начало координат — середина
	// «якоря» (точка крепления), к рельефу не привязаны. Short ~2 м шириной и до 2 м вниз,
	// Long до 5 м вниз, Drape — 6 м с провисом (провода, пролёты мостов).
	class VB_RiftHangBase: VB_RiftBase
	{
		scope = 0;
		vbAlignToTerrain = 0;
	};
	class VB_RiftHang_Short1: VB_RiftHangBase
	{
		scope = 2;
		displayName = "Rift Hang Short 1";
		model = "\VB_Rift\data\vb_rift_hang_short1.p3d";
	};
	class VB_RiftHang_Short2: VB_RiftHangBase
	{
		scope = 2;
		displayName = "Rift Hang Short 2";
		model = "\VB_Rift\data\vb_rift_hang_short2.p3d";
	};
	class VB_RiftHang_Long1: VB_RiftHangBase
	{
		scope = 2;
		displayName = "Rift Hang Long 1";
		model = "\VB_Rift\data\vb_rift_hang_long1.p3d";
	};
	class VB_RiftHang_Long2: VB_RiftHangBase
	{
		scope = 2;
		displayName = "Rift Hang Long 2";
		model = "\VB_Rift\data\vb_rift_hang_long2.p3d";
	};
	class VB_RiftHang_Drape1: VB_RiftHangBase
	{
		scope = 2;
		displayName = "Rift Hang Drape 1";
		model = "\VB_Rift\data\vb_rift_hang_drape1.p3d";
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
