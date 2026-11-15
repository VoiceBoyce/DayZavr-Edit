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
			"VB_RiftRoots_Small1", "VB_RiftRoots_Small2",
			"VB_RiftRoots_Crawl3", "VB_RiftRoots_Fork2", "VB_RiftRoots_Cluster3", "VB_RiftRoots_Small3",
			"VB_RiftWrap_10", "VB_RiftWrap_15", "VB_RiftWrap_20", "VB_RiftWrap_27", "VB_RiftWrap_35", "VB_RiftWrap_45",
			"VB_RiftHang_Short1", "VB_RiftHang_Short2", "VB_RiftHang_Long1", "VB_RiftHang_Long2", "VB_RiftHang_Drape1",
			"VB_RiftWall_Low1", "VB_RiftWall_Low2", "VB_RiftWall_Low3", "VB_RiftWall_Tall1", "VB_RiftWall_Tall2",
			"VB_RiftWall_Wide1", "VB_RiftWall_Wide2", "VB_RiftWall_Corner1", "VB_RiftWall_Corner2", "VB_RiftWall_Corner3",
			"VB_RiftHeartChunk", "VB_RiftRootPiece", "VB_RiftHeartDead", "VB_RiftSpores", "VB_RiftVein",
			"VB_RiftListenRoot", "VB_RiftSporeTrap", "VB_RiftLantern", "VB_RiftBlood", "VB_RiftBlade", "VB_RiftCloak",
			"VB_RootGuard_Wolf", "VB_RootGuard_Zanoza", "VB_RootGuard_Ent"
		};
		weapons[] = {};
		requiredVersion = 0.1;
		// INT_UIFramework — окно редактора гнёзд (должен стоять и на сервере, и у игроков)
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Characters_Zombies", "DZ_Characters_Tops", "DZ_Gear_Medical", "DZ_Vehicles_Parts", "DZ_Weapons_Melee_Blade", "INT_UIFramework", "DZ_Animals_canis_lupus", "Mutant_Zanoza", "MSFZ_Mutant_Ent"};
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
		name = "VB_AncientRoot";
		credits = "";
		author = "DayZavr";
		authorID = "";
		version = "0.3";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};

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
			class missionScriptModule
			{
				value = "";
				files[] = {"VB_Rift/scripts/5_Mission"};
			};
		};
	};
};

// Сердцебиение Изнанки: низкое «тук-тук», три варианта (tools/gen_rift_sounds.py).
// Запускает клиентский VB_RiftPulse ночью у сердец ближе 50 м. Настройки 3D как у ванильной SpookyArea.
class CfgSoundShaders
{
	// «Слушающий корень»: стук в стороне ближайшего живого сердца (VB_RiftGear.c)
	class VB_RiftListen_SoundShader
	{
		samples[] = {{"VB_Rift\sounds\vb_rift_heartbeat_1", 1}, {"VB_Rift\sounds\vb_rift_heartbeat_2", 1}, {"VB_Rift\sounds\vb_rift_heartbeat_3", 1}};
		volume = 1.0;
		range = 25;
		rangeCurve[] = {{0, 1}, {8, 0.8}, {25, 0}};
	};
	// «Сердце Изнанки» в руках или рюкзаке: тот же стук, тише и ближе (VB_RiftItems.c)
	class VB_RiftChunkBeat_SoundShader
	{
		samples[] = {{"VB_Rift\sounds\vb_rift_heartbeat_1", 1}, {"VB_Rift\sounds\vb_rift_heartbeat_2", 1}, {"VB_Rift\sounds\vb_rift_heartbeat_3", 1}};
		volume = 0.55;
		range = 30;
		rangeCurve[] = {{0, 1}, {10, 0.6}, {30, 0}};
	};
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
	class VB_RiftListen_SoundSet
	{
		soundShaders[] = {"VB_RiftListen_SoundShader"};
		spatial = 1;
		doppler = 0;
		loop = 0;
		volumeCurve = "LinearCurve";
		sound3DProcessingType = "ObjectsInner13DProcessingType";
	};
	class VB_RiftChunkBeat_SoundSet
	{
		soundShaders[] = {"VB_RiftChunkBeat_SoundShader"};
		spatial = 1;
		doppler = 0;
		loop = 0;
		volumeCurve = "LinearCurve";
		sound3DProcessingType = "ObjectsInner13DProcessingType";
	};
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
	// облако «Споровой ловушки»: тянет зомби вокруг (метров 60)
	class VB_RiftTrapNoise
	{
		type = "shot";
		strength = 40;
	};
	// стук «Сердца Изнанки», которое несут: слабый шум, зомби поблизости идут проверить
	class VB_RiftChunkNoise
	{
		type = "shot";
		strength = 18;
	};
	class VB_RiftCallNoise
	{
		type = "shot";
		strength = 50;
	};
};

class CfgVehicles
{
	// «Древний корень»: стражи рощи (ТЗ 2б.1). Текстуры и .rvmat — tools/gen_ar_guards.py; DamageSystem — копия
	// родителя со своими .rvmat в healthLevels на 1.0/0.7 (у существ материал ставит только healthLevels, E77)
	class Animal_CanisLupus_Grey;
	class MSFZ_Mutant_Ent;
	class MSFZ_Mutant_Zanoza1;
	class VB_RootGuard_Wolf: Animal_CanisLupus_Grey
	{
		scope = 2;
		displayName = "Лесовой волк";
		descriptionShort = "Волк, обросший корой и мхом. Под шерстью тлеют золотые прожилки: роща сама зовёт его на защиту.";
		vbRootGuard = 1;
		hiddenSelectionsTextures[] = {"\VB_Rift\data\guards\vb_guard_wolf_co.paa", "\VB_Rift\data\guards\vb_guard_wolf_fur_ca.paa"};
		hiddenSelectionsMaterials[] = {"\VB_Rift\data\guards\vb_guard_wolf.rvmat", "\VB_Rift\data\guards\vb_guard_wolf_fur.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1.0,{"\VB_Rift\data\guards\vb_guard_wolf.rvmat","\VB_Rift\data\guards\vb_guard_wolf_fur.rvmat"}},{0.7,{"\VB_Rift\data\guards\vb_guard_wolf.rvmat","\VB_Rift\data\guards\vb_guard_wolf_fur.rvmat"}},{0.5,{}},{0.3,{}},{0.0,{}}};
				};
				class Blood
				{
					hitpoints = 5000;
				};
				class Shock
				{
					hitpoints = 100;
				};
			};
			class DamageZones
			{
				class Zone_Head
				{
					componentNames[] = {"Zone_Head"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.15;
					canBleed = 0;
					class Health
					{
						hitpoints = 120;
						transferToGlobalCoef = 1;
					};
					class Blood: Health
					{
						hitpoints = 0;
					};
					class Shock: Health
					{
						hitpoints = 0;
					};
				};
				class Zone_Neck: Zone_Head
				{
					componentNames[] = {"Zone_Neck"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 100;
					};
				};
				class Zone_Chest: Zone_Head
				{
					componentNames[] = {"Zone_Chest"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 150;
					};
				};
				class Zone_Belly: Zone_Head
				{
					componentNames[] = {"Zone_Belly"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 150;
					};
				};
				class Zone_Spine: Zone_Head
				{
					componentNames[] = {"Zone_Spine_Front","Zone_Spine_Back"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 150;
					};
				};
				class Zone_Pelvis: Zone_Head
				{
					componentNames[] = {"Zone_Pelvis"};
					transferToZonesNames[] = {"Zone_Spine"};
					transferToZonesCoefs[] = {0.5};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 180;
					};
				};
				class Zone_Legs: Zone_Head
				{
					componentNames[] = {"Zone_Legs_Front","Zone_Legs_Back"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.0;
					class Health: Health
					{
						hitpoints = 100;
					};
				};
			};
		};
	};
	class VB_RootGuard_Zanoza: MSFZ_Mutant_Zanoza1
	{
		scope = 2;
		displayName = "Терновая заноза";
		descriptionShort = "Заноза, проросшая терновником. Шипы на её спине светятся золотом и рвут кожу до крови.";
		vbRootGuard = 1;
		hiddenSelectionsTextures[] = {"\VB_Rift\data\guards\vb_guard_zanoza_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_Rift\data\guards\vb_guard_zanoza.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 300;
					healthLevels[] = {{1,{"\VB_Rift\data\guards\vb_guard_zanoza.rvmat"}},{0.7,{"\VB_Rift\data\guards\vb_guard_zanoza.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Zanoza\data\msfz_zanoza_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Zanoza\data\msfz_zanoza_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Zanoza\data\msfz_zanoza_destruct.rvmat"}}};
				};
				class Blood
				{
					hitpoints = 300;
				};
				class Shock
				{
					hitpoints = 300;
				};
			};
			class DamageZones
			{
				class Zone_Head
				{
					componentNames[] = {"Zone_Head"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.15;
					canBleed = 0;
					class Health
					{
						hitpoints = 300;
						transferToGlobalCoef = 1;
					};
					class Blood: Health
					{
						hitpoints = 0;
					};
					class Shock: Health
					{
						hitpoints = 0;
					};
				};
				class Zone_Neck: Zone_Head
				{
					componentNames[] = {"Zone_Neck"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 300;
					};
				};
				class Zone_Chest: Zone_Head
				{
					componentNames[] = {"Zone_Chest"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 300;
					};
				};
				class Zone_Belly: Zone_Head
				{
					componentNames[] = {"Zone_Belly"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 300;
					};
				};
				class Zone_Spine: Zone_Head
				{
					componentNames[] = {"Zone_Spine_Front","Zone_Spine_Back"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 300;
					};
				};
				class Zone_Pelvis: Zone_Head
				{
					componentNames[] = {"Zone_Pelvis"};
					transferToZonesNames[] = {"Zone_Spine"};
					transferToZonesCoefs[] = {0.5};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 300;
					};
				};
				class Zone_Legs: Zone_Head
				{
					componentNames[] = {"Zone_Legs_Front","Zone_Legs_Back"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0;
					class Health: Health
					{
						hitpoints = 300;
					};
				};
			};
		};
	};
	class VB_RootGuard_Ent: MSFZ_Mutant_Ent
	{
		scope = 2;
		displayName = "Хранитель рощи";
		descriptionShort = "Древний Энт, выросший из корней рощи. Сердцевина в его груди горит золотом. Выходит только к древним рощам.";
		vbRootGuard = 1;
		hiddenSelectionsTextures[] = {"\VB_Rift\data\guards\vb_guard_ent_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_Rift\data\guards\vb_guard_ent.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 2000;
					healthLevels[] = {{1,{"\VB_Rift\data\guards\vb_guard_ent.rvmat"}},{0.7,{"\VB_Rift\data\guards\vb_guard_ent.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Ent\data\MSFZ_Ent_Base_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Ent\data\MSFZ_Ent_Base_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Ent\data\MSFZ_Ent_Base_destruct.rvmat"}}};
				};
			};
			class DamageZones
			{
				class Head
				{
					class Health
					{
						hitpoints = 1800;
						transferToGlobalCoef = 1;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.4;
							};
						};
						class Melee
						{
							class Health
							{
								damage = 0.5;
							};
						};
					};
					componentNames[] = {"Head"};
					fatalInjuryCoef = 0.1;
					inventorySlots[] = {"Headgear","Mask"};
				};
				class Torso
				{
					class Health
					{
						hitpoints = 2000;
						transferToGlobalCoef = 1;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.15;
							};
						};
						class Melee
						{
							class Health
							{
								damage = 0.5;
							};
						};
					};
					componentNames[] = {"Spine1","Spine3"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {"Vest","Body","Back"};
				};
				class LeftArm
				{
					class Health
					{
						hitpoints = 2000;
						transferToGlobalCoef = 0.33;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.15;
							};
						};
						class Melee
						{
							class Health
							{
								damage = 0.5;
							};
						};
					};
					componentNames[] = {"LeftArm","LeftForeArm"};
					fatalInjuryCoef = -1;
				};
				class RightArm
				{
					class Health
					{
						hitpoints = 2000;
						transferToGlobalCoef = 0.33;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.15;
							};
						};
						class Melee
						{
							class Health
							{
								damage = 0.5;
							};
						};
					};
					componentNames[] = {"RightArm","RightForeArm"};
					fatalInjuryCoef = -1;
				};
				class LeftLeg
				{
					class Health
					{
						hitpoints = 2000;
						transferToGlobalCoef = 0.33;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.15;
							};
						};
						class Melee
						{
							class Health
							{
								damage = 0.5;
							};
						};
					};
					componentNames[] = {"LeftLeg","LeftUpLeg"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {"Legs"};
				};
				class RightLeg
				{
					class Health
					{
						hitpoints = 2000;
						transferToGlobalCoef = 0.33;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.15;
							};
						};
						class Melee
						{
							class Health
							{
								damage = 0.5;
							};
						};
					};
					componentNames[] = {"RightLeg","RightUpLeg"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {"Legs"};
				};
				class LeftFoot
				{
					class Health
					{
						hitpoints = 2000;
						transferToGlobalCoef = 0.1;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.15;
							};
						};
						class Melee
						{
							class Health
							{
								damage = 0.5;
							};
						};
					};
					transferToZonesNames[] = {"LeftLeg"};
					transferToZonesCoefs[] = {0.1};
					componentNames[] = {"LeftFoot"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {"Feet"};
				};
				class RightFoot
				{
					class Health
					{
						hitpoints = 2000;
						transferToGlobalCoef = 0.1;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.15;
							};
						};
						class Melee
						{
							class Health
							{
								damage = 0.5;
							};
						};
					};
					transferToZonesNames[] = {"RightLeg"};
					transferToZonesCoefs[] = {0.1};
					componentNames[] = {"RightFoot"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {"Feet"};
				};
			};
		};
	};

	class HouseNoDestruct;
	class Inventory_Base;
	class CarBattery;
	class FangeKnife;
	class Raincoat_ColorBase;

	// Зомби Изнанки: ванильные модели и поведение, свои текстуры и материалы (tools/gen_rift_zombies.py):
	// мертвенно-серая кожа в светящихся жилах, светящиеся красные глаза, одежда в тёмный бордовый.
	// Появляются волной, когда игрок начинает вырезать сердце (VB_RiftHorde); можно спавнить и как обычных зомби.
	// vbRiftZombie = 1 — скрипт VB_RiftZombie: при смерти гаснут (vbRiftDeadMaterial), лопаются спорами,
	// с шансом оставляют «Корень Изнанки». Здоровье вчетверо больше ванильного (VB_RIFT_ZMB_DAMAGE, Баги №41).
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






	// моряк на солдатской базе: у ванильного солдата общее здоровье 115 -> 230





	// Выпадает из зомби Изнанки (VB_RiftZombie.ROOT_CHANCE) и отламывается ножом от корней гнезда ночью
	// (VB_ActionBreakRiftRoot). Материал для крафта (из него — «Жила Изнанки»).
	class VB_RiftRootPiece: Inventory_Base
	{
		scope = 2;
		displayName = "Отросток Древнего корня";
		descriptionShort = "Обломок живого корня. Под корой медленно течёт золотая живица.";
		model = "\VB_Rift\data\vb_rift_root_piece.p3d";
		weight = 300;
		itemSize[] = {1, 3};
		rotationFlags = 2;
	};

	// Выпадает тому, кто вырезал сердце (действие «Вырезать сердце» ножом/топором). Пока его несут — бьётся:
	// стук слышен рядом, зомби идут на шум. После рестарта протухает в VB_RiftHeartDead (VB_RiftItems.c).
	class VB_RiftHeartChunk: Inventory_Base
	{
		scope = 2;
		displayName = "Сердцевина Древнего корня";
		descriptionShort = "Тёплый янтарный ком, вырезанный из сердцевины корня. Внутри медленно пульсирует свет. К рестарту каменеет.";
		model = "\VB_Rift\data\vb_rift_heart_chunk.p3d";
		weight = 900;
		itemSize[] = {2, 2};
		rotationFlags = 2;
	};

	// «Сердце Изнанки», пролежавшее рестарт: живое сердце после рестарта заменяется этим (VB_RiftItems.c).
	// Та же модель в сухих дневных материалах, без свечения.
	class VB_RiftHeartDead: Inventory_Base
	{
		scope = 2;
		displayName = "Окаменевшая сердцевина";
		descriptionShort = "Сердцевина, которая остыла и окаменела, как старый янтарь. Больше не светится.";
		model = "\VB_Rift\data\vb_rift_heart_chunk.p3d";
		weight = 700;
		itemSize[] = {2, 2};
		rotationFlags = 2;
		hiddenSelections[] = {"crust", "veins"};
		hiddenSelectionsTextures[] = {"\VB_Rift\data\vb_rift_crust_day_co.paa", "\VB_Rift\data\vb_rift_vein_day_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_Rift\data\vb_rift_crust_day.rvmat", "\VB_Rift\data\vb_rift_vein_day.rvmat"};
	};

	// «Споры Изнанки»: собирают тряпкой у живого сердца ночью (VB_ActionCollectSpores), иногда с зомби Изнанки.
	// Материал для крафта.
	class VB_RiftSpores: Inventory_Base
	{
		scope = 2;
		displayName = "Золотая пыльца";
		descriptionShort = "Мешочек с пыльцой, собранной у живой сердцевины. Светится сквозь ткань. Через сутки осыпается и гаснет.";
		model = "\VB_Rift\data\vb_rift_spores_item.p3d";
		weight = 80;
		itemSize[] = {1, 1};
		rotationFlags = 2;
	};

	// «Жила Изнанки»: разделывают «Корень Изнанки» в крафте (1 корень -> 2 жилы). Материал для крафта.
	class VB_RiftVein: Inventory_Base
	{
		scope = 2;
		displayName = "Живое лыко";
		descriptionShort = "Волокно, снятое с отростка и смотанное на палочку. Прочное, как тетива, и тёплое: под ним тлеет золото.";
		model = "\VB_Rift\data\vb_rift_vein_item.p3d";
		weight = 50;
		itemSize[] = {1, 2};
		rotationFlags = 2;
	};

	// ---------------- изделия Изнанки (скрипты в VB_RiftGear.c) ----------------

	// «Слушающий корень»: ночью «прислушаться» — стук ближайшего живого сердца в 3 км звучит с его стороны,
	// ближе — громче и чаще, если сердце режут — бешено. Заряды = количество, подзарядка у живого сердца.
	class VB_RiftListenRoot: Inventory_Base
	{
		scope = 2;
		displayName = "Корень-вещун";
		descriptionShort = "Отросток, свёрнутый в раструб и стянутый лыком. Ночью в нём слышно, как поёт ближайшая живая сердцевина: чем ближе, тем громче. Подпитывается у живой сердцевины.";
		model = "\VB_Rift\data\vb_rift_listen_root.p3d";
		weight = 250;
		itemSize[] = {1, 2};
		rotationFlags = 2;
		quantityBar = 1;
		varQuantityInit = 10;
		varQuantityMin = 0;
		varQuantityMax = 10;
		varQuantityDestroyOnMin = 0;
	};

	// «Сердце-аккумулятор»: своя модель в ванильной текстуре аккумулятора, сверху сердце и корни (видны везде, в руках и превью).
	// Ставится вместо автомобильного аккумулятора. Ночью сам заряжается, днём садится.
	// В салоне стучит, когда машина рядом с живым гнездом (500 м).


	// «Споровая ловушка»: раскрыть и бросить. Облако тянет зомби, через 10 с лопается и бьёт зомби рядом.
	class VB_RiftSporeTrap: Inventory_Base
	{
		scope = 2;
		displayName = "Пыльцевая ловушка";
		descriptionShort = "Мешочек пыльцы, перетянутый лыком, с запалом и кольцом. Раскрой и брось: золотое облако тянет нечисть и через десять секунд вспыхивает, ударяя по ней. Людей не трогает.";
		model = "\VB_Rift\data\vb_rift_spore_trap.p3d";
		weight = 300;
		itemSize[] = {2, 2};
		rotationFlags = 2;
	};

	// «Фонарь-сердце»: красный свет без батареек, пульс чаще, когда рядом зомби. Количество = минуты спор.
	class VB_RiftLantern: Inventory_Base
	{
		scope = 2;
		displayName = "Светоч корня";
		descriptionShort = "Маленькая сердцевина в клетке из корней. Светит золотым без батареек и разгорается ярче, когда рядом нечисть. Ночью на свет выходят Стражи корня. Пыльцы хватает часов на шесть.";
		model = "\VB_Rift\data\vb_rift_lantern.p3d";
		weight = 600;
		itemSize[] = {2, 3};
		rotationFlags = 2;
		quantityBar = 1;
		varQuantityInit = 360;
		varQuantityMin = 0;
		varQuantityMax = 360;
		varQuantityDestroyOnMin = 0;
	};

	// «Кровь Изнанки» (ампула): укол — быстро кровь и здоровье; 10 минут зомби рядом светятся, руки дрожат, в глазах красное.
	class VB_RiftBlood: Inventory_Base
	{
		scope = 2;
		displayName = "Живица";
		descriptionShort = "Ампула с густой золотой живицей, оплетённая лыком. Укол быстро возвращает кровь и силы. Минут десять после него нечисть вокруг светится в темноте, а руки дрожат.";
		model = "\VB_Rift\data\vb_rift_blood_ampoule.p3d";
		weight = 60;
		itemSize[] = {1, 1};
		rotationFlags = 17;
	};

	// «Канистра Изнанки» (класс VB_RiftFuelCapsule): залить в бак (+20 л). Полчаса за машиной красный шлейф, на остановках сползаются зомби Изнанки.


	// «Голодный клинок»: убийство зомби возвращает кровь, полный заряд — облако спор вокруг. Час без убийств тянет кровь.
	class VB_RiftBlade: FangeKnife
	{
		scope = 2;
		displayName = "Терновый клинок";
		descriptionShort = "Нож, проросший терновым корнем. Каждое убийство нечисти возвращает владельцу немного крови, а насытившись, клинок осыпает пыльцой. Если долго не кормить, шипы впиваются в руку хозяина.";
		model = "\VB_Rift\data\vb_rift_blade.p3d";
		hiddenSelections[] = {};
	};

	// «Плащ-кокон»: тёплый дождевик; ночью присел и замер — обычные зомби теряют тебя (зомби Изнанки — нет).
	class VB_RiftCloak: Raincoat_ColorBase
	{
		scope = 2;
		displayName = "Плащ лешего";
		descriptionShort = "Дождевик, обросший корой и мхом. Тёплый и не промокает. Ночью, если присесть и замереть, обычные зомби теряют тебя из виду. Стражей корня так не обмануть.";
		heatIsolation = 0.45;
		visibilityModifier = 0.7;
		hiddenSelectionsTextures[] = {"\VB_Rift\data\retex\vb_rift_cloak_g_co.paa", "\VB_Rift\data\retex\vb_rift_cloak_co.paa", "\VB_Rift\data\retex\vb_rift_cloak_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_Rift\data\retex\vb_rift_cloak_g.rvmat", "\VB_Rift\data\retex\vb_rift_cloak.rvmat", "\VB_Rift\data\retex\vb_rift_cloak.rvmat"};
	};

	// «Живой жгут»: лежит в инвентаре и сам закрывает одно кровотечение, «отпив» немного крови.


	// «Нить Изнанки»: пришить к одежде (перетащить на вещь) — вещь сама латается около 1 %/мин 3 часа (до рестарта).


	// Объекты Изнанки. Начало координат модели = уровень земли (у корней — место выхода из земли).
	// Пульс свечения и свет — клиентский скрипт (VB_RiftPulse). Модели генерирует tools/gen_rift_veins.py.
	class VB_RiftBase: HouseNoDestruct
	{
		scope = 0;
		displayName = "Rift";
		vbAlignToTerrain = 1;	// скрипт наклоняет объект по склону, если наклон не задан вручную
		vbAlignRadius = 3.0;	// шаг сетки замера рельефа, м
		hiddenSelections[] = {"crust", "veins"};
		// пути с ведущим «\»: у бинаризованной модели материал без него не грузится и секция не рисуется (E75)
		hiddenSelectionsTextures[] = {"\VB_Rift\data\vb_rift_crust_co.paa", "\VB_Rift\data\vb_rift_vein_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_Rift\data\vb_rift_crust.rvmat", "\VB_Rift\data\vb_rift_vein.rvmat"};
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
	// Корни по стенам домов: ставит сердце само на дома рядом (VB_RiftHouses.c). Начало координат — основание
	// стены, «вперёд» (+Z) — наружу от стены. К рельефу не привязаны, проходимы.
	class VB_RiftWallBase: VB_RiftBase
	{
		scope = 0;
		vbAlignToTerrain = 0;
	};
	class VB_RiftWall_Low1: VB_RiftWallBase
	{
		scope = 2;
		vbHalfWidth = 1.5;
		displayName = "Rift Wall Roots Low 1";
		model = "\VB_Rift\data\vb_rift_wall_low1.p3d";
	};
	class VB_RiftWall_Low2: VB_RiftWallBase
	{
		scope = 2;
		vbHalfWidth = 1.5;
		displayName = "Rift Wall Roots Low 2";
		model = "\VB_Rift\data\vb_rift_wall_low2.p3d";
	};
	class VB_RiftWall_Tall1: VB_RiftWallBase
	{
		scope = 2;
		vbHalfWidth = 1.8;
		displayName = "Rift Wall Roots Tall 1";
		model = "\VB_Rift\data\vb_rift_wall_tall1.p3d";
	};
	class VB_RiftWall_Wide1: VB_RiftWallBase
	{
		scope = 2;
		vbHalfWidth = 2.5;
		displayName = "Rift Wall Roots Wide 1";
		model = "\VB_Rift\data\vb_rift_wall_wide1.p3d";
	};
	class VB_RiftWall_Low3: VB_RiftWallBase
	{
		scope = 2;
		displayName = "Rift Wall Roots Low 3";
		model = "\VB_Rift\data\vb_rift_wall_low3.p3d";
		vbHalfWidth = 1.5;
	};
	class VB_RiftWall_Tall2: VB_RiftWallBase
	{
		scope = 2;
		displayName = "Rift Wall Roots Tall 2";
		model = "\VB_Rift\data\vb_rift_wall_tall2.p3d";
		vbHalfWidth = 1.8;
	};
	class VB_RiftWall_Wide2: VB_RiftWallBase
	{
		scope = 2;
		displayName = "Rift Wall Roots Wide 2";
		model = "\VB_Rift\data\vb_rift_wall_wide2.p3d";
		vbHalfWidth = 2.5;
	};
	// Корни на наружном углу дома: переваливают через ребро на обе стены. Начало координат — ребро у земли,
	// дом сзади-слева: стена A смотрит вперёд (+Z модели), стена B — вправо (+X модели). vbFace — на сколько
	// метров корни заходят на каждую стену.
	class VB_RiftWall_Corner1: VB_RiftWallBase
	{
		scope = 2;
		displayName = "Rift Wall Roots Corner 1";
		model = "\VB_Rift\data\vb_rift_wall_corner1.p3d";
		vbFace = 2.0;
	};
	class VB_RiftWall_Corner2: VB_RiftWallBase
	{
		scope = 2;
		displayName = "Rift Wall Roots Corner 2";
		model = "\VB_Rift\data\vb_rift_wall_corner2.p3d";
		vbFace = 2.0;
	};
	class VB_RiftWall_Corner3: VB_RiftWallBase
	{
		scope = 2;
		displayName = "Rift Wall Roots Corner 3";
		model = "\VB_Rift\data\vb_rift_wall_corner3.p3d";
		vbFace = 2.0;
	};
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
	class VB_RiftRoots_Crawl3: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Crawl 3";
		model = "\VB_Rift\data\vb_rift_roots_crawl3.p3d";
	};
	class VB_RiftRoots_Fork2: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Fork 2";
		model = "\VB_Rift\data\vb_rift_roots_fork2.p3d";
	};
	class VB_RiftRoots_Cluster3: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Cluster 3";
		model = "\VB_Rift\data\vb_rift_roots_cluster3.p3d";
	};
	class VB_RiftRoots_Small3: VB_RiftBase
	{
		scope = 2;
		displayName = "Rift Roots Small 3";
		model = "\VB_Rift\data\vb_rift_roots_small3.p3d";
	};

};
