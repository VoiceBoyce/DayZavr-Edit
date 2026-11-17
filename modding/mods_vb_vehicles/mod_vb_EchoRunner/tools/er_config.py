# -*- coding: utf-8 -*-
r"""config.cpp, stringtable.csv и звуки мода VB_EchoRunner из конфига Interceptor ALEX (этап 6).

  python er_config.py
  выход: VB_EchoRunner/config.cpp, stringtable.csv, sounds/ (копия звуков ALEX), data/ (недостающие файлы ALEX)

Берём из ALEX: колесо (целое/разбитое), машину (ходовая, звуки, зоны, места), двери и капот — без цветных
вариантов; CfgSoundShaders/Sets. Переименование: классы VB_EchoRunner*, слоты VB_ER_*, пути VB_EchoRunner\...,
звуки VB_ER_*, строки STR_VB_ER_*. Правки: колесо Ø0.92 (radius 0.42, width 0.36), передаточные числа ×1.27,
скрытые выделения neon/screen (машина) и neon (двери), свои навесные (бак ×2, трубопроводы ×3), без запаски и бочек.
"""
import io
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import er_data  # noqa: E402

MOD = os.path.dirname(HERE)
SRC = os.path.join(MOD, "VB_EchoRunner")
ALEX = os.path.join(os.path.dirname(MOD), "купленные модели", "Interceptor", "Interceptor_byALEX")
P = "vb_echorunner\\data\\parts\\"
TXT = io.open(os.path.join(ALEX, "config.cpp"), encoding="utf-8", errors="replace").read().replace("\r\n", "\n")


def block(start, end):
    a = TXT.index(start)
    b = TXT.index(end, a)
    return TXT[a:b]


def rep(s, a, b, count=1):
    assert s.count(a) == count, (a[:80], s.count(a))
    return s.replace(a, b)


bases = block("\tclass Inventory_Base;", "\t\tclass Interceptor_Wheel: CarWheel")
wheel = block("\t\tclass Interceptor_Wheel: CarWheel", "\tclass Interceptor: CarScript")
car = block("\tclass Interceptor: CarScript", "\tclass Interceptor_Doors_Driver: CarDoor")
doors = block("\tclass Interceptor_Doors_Driver: CarDoor", "\tclass Interceptor_Doors_Driver_blue")
hood = block("\tclass Interceptor_Doors_hood: CarDoor", "\tclass Interceptor_Doors_hood_blue")
sounds = TXT[TXT.index("class CfgSoundShaders"):]


def rename(s):
    s = re.sub(r"(?i)Interceptor_byALEX\\proxy\\Interceptor_Wheel_destroyed\.p3d", r"VB_EchoRunner\\proxy\\vb_er_wheel_destroyed.p3d", s)
    s = re.sub(r"(?i)Interceptor_byALEX\\proxy\\Interceptor_Wheel\.p3d", r"VB_EchoRunner\\proxy\\vb_er_wheel.p3d", s)
    s = re.sub(r"(?i)Interceptor_byALEX\\proxy\\Interceptor_Doors_Driver\.p3d", r"VB_EchoRunner\\proxy\\vb_er_door_driver.p3d", s)
    s = re.sub(r"(?i)Interceptor_byALEX\\proxy\\Interceptor_Doors_coDriver\.p3d", r"VB_EchoRunner\\proxy\\vb_er_door_codriver.p3d", s)
    s = re.sub(r"(?i)Interceptor_byALEX\\proxy\\Interceptor_Doors_hood\.p3d", r"VB_EchoRunner\\proxy\\vb_er_hood.p3d", s)
    s = re.sub(r"(?i)Interceptor_byALEX\\Interceptor\.p3d", r"VB_EchoRunner\\vb_echorunner.p3d", s)
    s = re.sub(r"(?i)Interceptor_byALEX\\data\\", r"vb_echorunner\\data\\", s)
    s = re.sub(r"(?i)Interceptor_byALEX\\sounds\\", r"VB_EchoRunner\\sounds\\", s)
    # слоты (в кавычках) — раньше классов
    s = s.replace('"Interceptor_Wheel_spare_1"', '""')
    for i in ("1_1", "1_2", "2_1", "2_2"):
        s = s.replace('"Interceptor_Wheel_%s"' % i, '"VB_ER_Wheel_%s"' % i)
    s = s.replace('"Interceptor_Doors_Driver"', '"VB_ER_Door_Driver"')
    s = s.replace('"Interceptor_Doors_coDriver"', '"VB_ER_Door_CoDriver"')
    s = s.replace('"Interceptor_Doors_hood"', '"VB_ER_Hood"')
    s = s.replace('"Interceptor_Doors_trunk"', '""')
    # классы
    s = s.replace("Interceptor_Wheel_destroyed", "VB_EchoRunner_Wheel_Destroyed")
    s = s.replace("Interceptor_Wheel_Ruined", "VB_EchoRunner_Wheel_Ruined")
    s = s.replace("Interceptor_Wheel:", "VB_EchoRunner_Wheel:").replace("class Interceptor_Wheel\n", "class VB_EchoRunner_Wheel\n")
    s = s.replace(": Interceptor_Wheel ", ": VB_EchoRunner_Wheel ")
    s = s.replace("Interceptor_Doors_Driver", "VB_EchoRunner_Door_Driver")
    s = s.replace("Interceptor_Doors_coDriver", "VB_EchoRunner_Door_CoDriver")
    s = s.replace("Interceptor_Doors_hood", "VB_EchoRunner_Hood")
    s = s.replace("class Interceptor: CarScript", "class VB_EchoRunner: CarScript")
    # звуки и строки
    s = re.sub(r"baseEngineInterceptor", "baseEngineVB_ER", s)
    s = re.sub(r"baseInterceptor", "baseVB_ER", s)
    s = re.sub(r"\bInterceptor_(?=\w*Sound)", "VB_ER_", s)
    s = re.sub(r"(?i)\$STR_Interceptor_", "$STR_VB_ER_", s)
    return s


wheel, car, doors, hood, sounds = (rename(x) for x in (wheel, car, doors, hood, sounds))
assert "Interceptor" not in wheel + car + doors + hood, [m.start() for m in re.finditer("Interceptor", car)][:5]

# ---------------- колесо Ø0.92
wheel = rep(wheel, "radiusByDamage[]={0,0.33000001,0.30000001,0.30000001,0.99980003,0.25,0.99989998,0.2};",
            "radiusByDamage[]={0,0.42,0.30,0.39,0.99980003,0.34,0.99989998,0.30};")
wheel = rep(wheel, "radius=0.33000001;", "radius=0.42;")
wheel = rep(wheel, "width=0.28;", "width=0.36;")
wheel = rep(wheel, "radius=0.27000001;", "radius=0.34;")
wheel = rep(wheel, "width=0.2;", "width=0.30;")
wheel = rep(wheel, "weight=20000;", "weight=30000;", 2)
wheel = re.sub(r"healthLevels\[\]=\s*\{.*?\n\t*\};", "healthLevels[]={{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.0,{}}};",
               wheel, count=1, flags=re.S)

# ---------------- машина
car = rep(car, '\t\t\t"camo1"\n\t\t};\n\t\tattachments[]=', '\t\t\t"camo1",\n\t\t\t"neon",\n\t\t\t"screen"\n\t\t};\n\t\tattachments[]=')
car = re.sub(r"attachments\[\]=\s*\{.*?\};", """attachments[]=
		{
			"CarBattery",
			"Reflector_1_1",
			"Reflector_2_1",
			"CarRadiator",
			"SparkPlug",
			"VB_ER_Wheel_1_1",
			"VB_ER_Wheel_1_2",
			"VB_ER_Wheel_2_1",
			"VB_ER_Wheel_2_2",
			"VB_ER_Door_Driver",
			"VB_ER_Door_CoDriver",
			"VB_ER_Hood",
			"VB_ER_Tank_1",
			"VB_ER_Tank_2",
			"VB_ER_Pipes_L",
			"VB_ER_Pipes_R",
			"VB_ER_Pipes_Engine",
			"CanisterGasoline",
			"Shoulder",
			"Back",
			"VestGrenadeA",
			"VestGrenadeB",
			"VestGrenadeC",
			"VestGrenadeD",
			"Cassette"
		};""", car, count=1, flags=re.S)
car = rep(car, '\t\t\t"vb_echorunner\\data\\body_co.paa"\n\t\t};\n\t\thiddenSelectionsMaterials[]=',
          '\t\t\t"vb_echorunner\\data\\body_co.paa",\n\t\t\t"%ser_neon_co.paa",\n\t\t\t"%ser_screen_co.paa"\n\t\t};\n'
          '\t\thiddenSelectionsMaterials[]=' % (P, P))
car = rep(car, '\t\t\t"vb_echorunner\\data\\body.rvmat"\n\t\t};\n\t\tdashboardMatOn=',
          '\t\t\t"vb_echorunner\\data\\body.rvmat",\n\t\t\t"%ser_neon_off.rvmat",\n\t\t\t"%ser_screen_off.rvmat"\n\t\t};\n'
          '\t\tdashboardMatOn=' % (P, P))
car = rep(car, "ratio=4.0999999;", "ratio=5.2;", 2)                       # колесо 0.42 вместо 0.33: ×1.27
# коробка — ручная, как у ALEX (11-18 пользователь вернул ручную; автомат 11-17 убран)
car = rep(car, "wheelHubRadius=0.25;", "wheelHubRadius=0.30;", 2)
car = re.sub(r"class GUIInventoryAttachmentsProps\s*\{.*?\n\t\t\};\n", """class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name="$STR_attachment_Engine0";
				description="";
				attachmentSlots[]={"SparkPlug","CarBattery","CarRadiator"};
				icon="cat_vehicle_engine";
			};
			class Body
			{
				name="$STR_attachment_Body0";
				description="";
				attachmentSlots[]={"Reflector_1_1","Reflector_2_1","VB_ER_Door_Driver","VB_ER_Door_CoDriver","VB_ER_Hood","Cassette"};
				icon="cat_vehicle_body";
			};
			class Chassis
			{
				name="$STR_attachment_Chassis0";
				description="";
				attachmentSlots[]={"VB_ER_Wheel_1_1","VB_ER_Wheel_1_2","VB_ER_Wheel_2_1","VB_ER_Wheel_2_2"};
				icon="cat_vehicle_chassis";
			};
			class VB_ER_Rift
			{
				name="$STR_VB_ER_cat_rift";
				description="";
				attachmentSlots[]={"VB_ER_Tank_1","VB_ER_Tank_2","VB_ER_Pipes_L","VB_ER_Pipes_R","VB_ER_Pipes_Engine"};
				icon="cat_vehicle_engine";
			};
			class BaseBuildingContainers
			{
				name="$STR_attachment_containers0";
				description="";
				attachmentSlots[]={"CanisterGasoline","Shoulder","Back","VestGrenadeA","VestGrenadeB","VestGrenadeC","VestGrenadeD"};
				icon="cat_common_cargo";
			};
		};
""", car, count=1, flags=re.S)
assert "VB_ER_Rift" in car and '"VB_ER_Tank_1"' in car

# ---------------- двери: + neon (полосы в салоне)
doors = rep(doors, '\t\t\t"camo3"\n\t\t};\n\t\thiddenSelectionsTextures[]=',
            '\t\t\t"camo3",\n\t\t\t"neon"\n\t\t};\n\t\thiddenSelectionsTextures[]=')
doors = rep(doors, '\t\t\t""\n\t\t};\n\t\thiddenSelectionsMaterials[]=',
            '\t\t\t"",\n\t\t\t"%ser_neon_co.paa"\n\t\t};\n\t\thiddenSelectionsMaterials[]=' % P)
doors = re.sub(r'(hiddenSelectionsMaterials\[\]=\s*\{[^}]*?)\n\t\t\t""\n\t\t\};',
               r'\1\n\t\t\t"",\n\t\t\t"%ser_neon_off.rvmat"\n\t\t};' % P.replace("\\", "\\\\"), doors, count=1)
assert "er_neon_off.rvmat" in doors

ITEMS = '''
	class VB_EchoRunner_RiftPart: Inventory_Base
	{
		scope=0;
		weight=12000;
		itemSize[]={6,3};
		itemBehaviour=0;
		physLayer="item_large";
		rotationFlags=0;
		hiddenSelections[]={"neon"};
		hiddenSelectionsTextures[]={"%(P)ser_neon_co.paa"};
		hiddenSelectionsMaterials[]={"%(P)ser_neon_idle.rvmat"};		// снятая деталь еле тлеет (превью, 11-18)
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=800;
				};
			};
		};
	};
	class VB_EchoRunner_Tank: VB_EchoRunner_RiftPart
	{
		scope=2;
		displayName="$STR_VB_ER_tank_name";
		descriptionShort="$STR_VB_ER_tank_disc";
		model="VB_EchoRunner\\proxy\\vb_er_tank.p3d";
		weight=35000;
		itemSize[]={6,5};
		inventorySlot[]={"VB_ER_Tank_1","VB_ER_Tank_2"};
		// энергия Изнанки 0–100 (проценты) = количество предмета (полоса в инвентаре, сохраняется ванилью); расход — скрипт машины
		stackedUnit="percentage";
		quantityBar=1;
		varQuantityInit=0;
		varQuantityMin=0;
		varQuantityMax=100;
		varQuantityDestroyOnMin=0;
	};
	// антиграв-модуль ДВ-4 (этап 8): ставится вместо колеса; наклон и роторы — источники user (скрипт машины).
	// Пара «_Ruined» обязательна для любого CarWheel (InventoryItemType.SetReplacementTypes, иначе ошибка VM)
	class VB_EchoRunner_GravModule: CarWheel
	{
		scope=2;
		displayName="$STR_VB_ER_grav_name";
		descriptionShort="$STR_VB_ER_grav_disc";
		model="VB_EchoRunner\\proxy\\vb_er_gravmodule.p3d";
		itemSize[]={6,4};
		weight=40000;
		physLayer="item_large";
		rotationFlags=0;
		itemBehaviour=0;
		inventorySlot[]={"VB_ER_Wheel_1_1","VB_ER_Wheel_1_2","VB_ER_Wheel_2_1","VB_ER_Wheel_2_2"};
		radius=0.42;
		width=0.36;
		tyreRollResistance=0.3;
		tyreRollDrag=60;
		tyreRoughness=1.0;
		tyreTread=0.5;
		hiddenSelections[]={"neon"};
		hiddenSelectionsTextures[]={"%(P)ser_neon_co.paa"};
		hiddenSelectionsMaterials[]={"%(P)ser_neon_idle.rvmat"};		// снятая деталь еле тлеет (превью, 11-18)
		class AnimationSources
		{
			class rotor
			{
				source="user";
				animPeriod=0.01;
				initPhase=0;
			};
			class spin
			{
				source="user";
				animPeriod=0.01;
				initPhase=0;
			};
			class tilt_x
			{
				source="user";
				animPeriod=0.01;
				initPhase=0.5;		// фаза 0..1, 0.5 — ровно (user-источник отрицательные режет)
			};
			class tilt_z
			{
				source="user";
				animPeriod=0.01;
				initPhase=0.5;		// фаза 0..1, 0.5 — ровно (user-источник отрицательные режет)
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=2000;
					healthLevels[]={{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.0,{}}};
				};
			};
		};
	};
	class VB_EchoRunner_GravModule_Ruined: VB_EchoRunner_GravModule { scope=2; };
	class VB_EchoRunner_Pipes_L: VB_EchoRunner_RiftPart
	{
		scope=2;
		displayName="$STR_VB_ER_pipes_l_name";
		descriptionShort="$STR_VB_ER_pipes_disc";
		model="VB_EchoRunner\\proxy\\vb_er_pipes_l.p3d";
		inventorySlot[]={"VB_ER_Pipes_L"};
	};
	class VB_EchoRunner_Pipes_R: VB_EchoRunner_RiftPart
	{
		scope=2;
		displayName="$STR_VB_ER_pipes_r_name";
		descriptionShort="$STR_VB_ER_pipes_disc";
		model="VB_EchoRunner\\proxy\\vb_er_pipes_r.p3d";
		inventorySlot[]={"VB_ER_Pipes_R"};
	};
	class VB_EchoRunner_Pipes_Engine: VB_EchoRunner_RiftPart
	{
		scope=2;
		displayName="$STR_VB_ER_pipes_e_name";
		descriptionShort="$STR_VB_ER_pipes_disc";
		model="VB_EchoRunner\\proxy\\vb_er_pipes_engine.p3d";
		inventorySlot[]={"VB_ER_Pipes_Engine"};
	};
''' % {"P": P}

SLOTS = []
# selection — выделение места в модели: у колёс и дверей обязательно (по нему движок ставит колесо на ось), как у ALEX
for name, disp, icon, sel in (("VB_ER_Wheel_1_1", "$STR_VB_ER_Wheel_name", "wheel", "wheel_1_1"),
                              ("VB_ER_Wheel_1_2", "$STR_VB_ER_Wheel_name", "wheel", "wheel_1_2"),
                              ("VB_ER_Wheel_2_1", "$STR_VB_ER_Wheel_name", "wheel", "wheel_2_1"),
                              ("VB_ER_Wheel_2_2", "$STR_VB_ER_Wheel_name", "wheel", "wheel_2_2"),
                              ("VB_ER_Door_Driver", "$STR_VB_ER_driver_name", "doorfront", "doors_driver"),
                              ("VB_ER_Door_CoDriver", "$STR_VB_ER_codriver_name", "doorfront", "doors_codriver"),
                              ("VB_ER_Hood", "$STR_VB_ER_hood_name", "hood", "doors_hood"),
                              ("VB_ER_Tank_1", "$STR_VB_ER_tank_name", "barrel", ""),
                              ("VB_ER_Tank_2", "$STR_VB_ER_tank_name", "barrel", ""),
                              ("VB_ER_Pipes_L", "$STR_VB_ER_pipes_l_name", "rope", ""),
                              ("VB_ER_Pipes_R", "$STR_VB_ER_pipes_r_name", "rope", ""),
                              ("VB_ER_Pipes_Engine", "$STR_VB_ER_pipes_e_name", "rope", "")):
    selline = ('\t\tselection="%s";\n' % sel) if sel else ""
    SLOTS.append('\tclass Slot_%s\n\t{\n\t\tname="%s";\n\t\tdisplayName="%s";\n%s\t\tghostIcon="%s";\n\t};'
                 % (name, name, disp, selline, icon))

PROXIES = ""
for cls, model, slots in (("vb_er_wheel", "vb_er_wheel", ["VB_ER_Wheel_1_1", "VB_ER_Wheel_1_2", "VB_ER_Wheel_2_1", "VB_ER_Wheel_2_2"]),
                          ("vb_er_wheel_destroyed", "vb_er_wheel_destroyed", ["VB_ER_Wheel_1_1", "VB_ER_Wheel_1_2", "VB_ER_Wheel_2_1", "VB_ER_Wheel_2_2"]),
                          ("vb_er_door_driver", "vb_er_door_driver", ["VB_ER_Door_Driver"]),
                          ("vb_er_door_codriver", "vb_er_door_codriver", ["VB_ER_Door_CoDriver"]),
                          ("vb_er_hood", "vb_er_hood", ["VB_ER_Hood"]),
                          # у каждого бака свой прокси, как у трубок: с одним прокси на два слота баки на машине
                          # не рисовались (стенд 11-18), модель прокси — копия модели бака
                          ("vb_er_tank_1", "vb_er_tank_1", ["VB_ER_Tank_1"]),
                          ("vb_er_tank_2", "vb_er_tank_2", ["VB_ER_Tank_2"]),
                          ("vb_er_pipes_l", "vb_er_pipes_l", ["VB_ER_Pipes_L"]),
                          ("vb_er_pipes_r", "vb_er_pipes_r", ["VB_ER_Pipes_R"]),
                          ("vb_er_pipes_engine", "vb_er_pipes_engine", ["VB_ER_Pipes_Engine"])):
    PROXIES += ('\tclass Proxy%s: ProxyVehiclePart\n\t{\n\t\tmodel="VB_EchoRunner\\proxy\\%s.p3d";\n\t\tinventorySlot[]={%s};\n\t};\n'
                % (cls, model, ",".join('"%s"' % s for s in slots)))

HEAD = '''// VB_EchoRunner — Echo Runner (сервер 16). Основа — купленный Interceptor ALEX (пак @DayZavrVehicle3_ALX,
// переделка разрешена), сгенерировано tools/er_config.py — правки вносить в генератор.
class CfgPatches
{
	class VB_EchoRunner
	{
		units[]={"VB_EchoRunner"};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"DZ_Data","DZ_Vehicles_Wheeled","DZ_Vehicles_Parts","DZ_Sounds_Effects"};
	};
};
class CfgMods
{
	class VB_EchoRunner
	{
		dir="VB_EchoRunner";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="VB_EchoRunner";
		credits="DayZavr";
		author="DayZavr";
		version="1";
		extra=0;
		type="mod";
		inputs="VB_EchoRunner/data/vb_er_inputs.xml";
		dependencies[]={"World"};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]={"VB_EchoRunner/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value="";
				files[]={"VB_EchoRunner/scripts/4_World"};
			};
			class missionScriptModule
			{
				value="";
				files[]={"VB_EchoRunner/scripts/5_Mission"};
			};
		};
	};
};
class CfgSlots
{
%s
};
class CfgNonAIVehicles
{
	class ProxyVehiclePart;
%s};
class CfgVehicles
{
''' % ("\n".join(SLOTS), PROXIES)

# ---------------- гул антиграв-модулей (tools/er_sounds.py): две петли, скрипт смешивает их громкостью
GRAV_SHADERS = '''	class VB_ER_GravIdle_SoundShader
	{
		samples[]={{"VB_EchoRunner\\sounds\\vb_er_grav_idle",1}};
		volume=0.8;
		range=60;
		rangeCurve[]={{0,1},{15,0.7},{35,0.3},{60,0}};
	};
	class VB_ER_GravFly_SoundShader
	{
		samples[]={{"VB_EchoRunner\\sounds\\vb_er_grav_fly",1}};
		volume=1.0;
		range=150;
		rangeCurve[]={{0,1},{30,0.6},{80,0.25},{150,0}};
	};
	class VB_ER_GravThrust_SoundShader
	{
		samples[]={{"VB_EchoRunner\\sounds\\vb_er_grav_thrust",1}};
		volume=1.0;
		range=250;
		rangeCurve[]={{0,1},{40,0.6},{120,0.25},{250,0}};
	};
'''
GRAV_SETS = '''	class VB_ER_GravIdle_SoundSet
	{
		soundShaders[]={"VB_ER_GravIdle_SoundShader"};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=0;
		loop=1;
		volumeCurve="LinearCurve";
		sound3DProcessingType="ObjectsInner13DProcessingType";
	};
	class VB_ER_GravFly_SoundSet
	{
		soundShaders[]={"VB_ER_GravFly_SoundShader"};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=1;
		loop=1;
		volumeCurve="LinearCurve";
		sound3DProcessingType="ObjectsInner13DProcessingType";
	};
	class VB_ER_GravThrust_SoundSet
	{
		soundShaders[]={"VB_ER_GravThrust_SoundShader"};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		doppler=1;
		loop=1;
		volumeCurve="LinearCurve";
		sound3DProcessingType="ObjectsInner13DProcessingType";
	};
'''
sounds = re.sub(r"(class CfgSoundShaders\s*\n\{\n)", lambda m: m.group(1) + GRAV_SHADERS, sounds, count=1)
sounds = re.sub(r"(class CfgSoundSets\s*\n\{\n)", lambda m: m.group(1) + GRAV_SETS, sounds, count=1)
assert "VB_ER_GravIdle_SoundShader" in sounds and "VB_ER_GravFly_SoundSet" in sounds

out = HEAD + bases + wheel + car + doors + hood + ITEMS + "};\n" + sounds
out = out.replace("\n", "\r\n")
with open(os.path.join(SRC, "config.cpp"), "w", encoding="utf-8", newline="") as f:
    f.write(out)

# ---------------- данные ALEX, на которые ссылается конфиг
rels = set(m.group(1).lower() for m in re.finditer(r'"vb_echorunner\\data\\([^"]+)"', out, re.I))
rels = {r for r in rels if not r.startswith("parts\\")}
print("данные из конфига:", sorted(rels), "скопировано", er_data.ensure(rels))

# ---------------- звуки
dst = os.path.join(SRC, "sounds")
if not os.path.isdir(dst):
    shutil.copytree(os.path.join(ALEX, "sounds"), dst)

# ---------------- stringtable
keys = sorted(set(re.findall(r"\$(STR_VB_ER_\w+)", out)))
S = {
    "STR_VB_ER_name": ("Echo Runner", "Echo Runner"),
    "STR_VB_ER_Wheel_name": ("Echo Runner Wheel", "Колесо Echo Runner"),
    "STR_VB_ER_wheel_disc": ("Mud tyre on a steel beadlock rim. Fits only the Echo Runner.",
                             "Грязевая шина на стальном диске с бедлоком. Подходит только к Echo Runner."),
    "STR_VB_ER_driver_name": ("Echo Runner Driver Door", "Дверь водителя Echo Runner"),
    "STR_VB_ER_codriver_name": ("Echo Runner Co-driver Door", "Дверь пассажира Echo Runner"),
    "STR_VB_ER_doors_driver_disc": ("Riveted driver door with a window net.", "Клёпаная дверь водителя с сеткой на окне."),
    "STR_VB_ER_doors_codriver_disc": ("Riveted co-driver door with a window net.", "Клёпаная дверь пассажира с сеткой на окне."),
    "STR_VB_ER_hood_name": ("Echo Runner Hood", "Капот Echo Runner"),
    "STR_VB_ER_hood_disc": ("Hood of the Echo Runner. Protects the engine.", "Капот Echo Runner. Защищает двигатель."),
    "STR_VB_ER_tank_name": ("Rift Tank", "Бак Изнанки"),
    "STR_VB_ER_tank_disc": ("Storage tank for Rift energy. Mounts in the Echo Runner's trunk.",
                            "Накопитель энергии Изнанки. Ставится в багажник Echo Runner."),
    "STR_VB_ER_pipes_l_name": ("Rift Line (left)", "Трубопровод Изнанки (левый)"),
    "STR_VB_ER_pipes_r_name": ("Rift Line (right)", "Трубопровод Изнанки (правый)"),
    "STR_VB_ER_pipes_e_name": ("Rift Line (engine)", "Трубопровод Изнанки (мотор)"),
    "STR_VB_ER_pipes_disc": ("Glowing line that carries Rift energy from the tanks. Fits only the Echo Runner.",
                             "Светящаяся магистраль: несёт энергию Изнанки от баков. Подходит только к Echo Runner."),
    "STR_VB_ER_cat_rift": ("Rift", "Изнанка"),
    "STR_VB_ER_grav_name": ("DV-4 Antigravity Module", "Антиграв-модуль ДВ-4"),
    "STR_VB_ER_grav_disc": ("Mounts in place of an Echo Runner wheel. With all four modules, the Rift lines and charged Rift tanks the car flies.",
                            "Ставится вместо колеса Echo Runner. С четырьмя модулями, трубопроводами и заряженными баками Изнанки машина летает."),
    # клавиши полёта (data/vb_er_inputs.xml)
    "STR_VB_ER_in_group": ("Echo Runner (flight)", "Echo Runner (полёт)"),
    "STR_VB_ER_in_forward": ("Flight: forward", "Полёт: вперёд"),
    "STR_VB_ER_in_back": ("Flight: back", "Полёт: назад"),
    "STR_VB_ER_in_left": ("Flight: turn left", "Полёт: поворот влево"),
    "STR_VB_ER_in_right": ("Flight: turn right", "Полёт: поворот вправо"),
    "STR_VB_ER_in_up": ("Flight: nose up, climb", "Полёт: нос вверх, набор высоты"),
    "STR_VB_ER_in_down": ("Flight: nose down, descend", "Полёт: нос вниз, снижение"),
}
missing = [k for k in keys if k not in S]
for k in missing:
    S[k] = (k[11:].replace("_", " "), k[11:].replace("_", " "))
    print("WARN строка без перевода:", k)
lines = ['"Language","original","english","russian",']
for k in sorted(S):
    en, ru = S[k]
    lines.append('"%s","%s","%s","%s",' % (k, en, en, ru))
with open(os.path.join(SRC, "stringtable.csv"), "w", encoding="utf-8", newline="\r\n") as f:
    f.write("\n".join(lines) + "\n")

# ---------------- клавиши полёта (как у RFFS: свои действия на тех же клавишах; без <context>, см. скилл dayz-aviation)
INPUTS = [("UAVBERForward", "forward", "kW"), ("UAVBERBack", "back", "kS"), ("UAVBERLeft", "left", "kA"),
          ("UAVBERRight", "right", "kD"), ("UAVBERUp", "up", "kLShift"), ("UAVBERDown", "down", "kLControl")]
xml = ['<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>', "<modded_inputs>", "\t<inputs>", "\t\t<actions>"]
xml += ['\t\t\t<input name="%s" loc="STR_VB_ER_in_%s" />' % (n, k) for n, k, _ in INPUTS]
xml += ["\t\t</actions>", '\t\t<sorting name="vb_echorunner" loc="STR_VB_ER_in_group">']
xml += ['\t\t\t<input name="%s" />' % n for n, _, _ in INPUTS]
xml += ["\t\t</sorting>", "\t</inputs>", "\t<preset>"]
for n, _, key in INPUTS:
    xml += ['\t\t<input name="%s">' % n, '\t\t\t<btn name="%s"/>' % key, "\t\t</input>"]
xml += ["\t</preset>", "</modded_inputs>"]
for _, k, _ in INPUTS:
    assert "STR_VB_ER_in_" + k in S, k
os.makedirs(os.path.join(SRC, "data"), exist_ok=True)
with open(os.path.join(SRC, "data", "vb_er_inputs.xml"), "w", encoding="utf-8", newline="\r\n") as f:
    f.write("\n".join(xml) + "\n")

with open(os.path.join(SRC, "pbo.json"), "w", encoding="utf-8") as f:
    f.write('{\n    "compress": {"exclude": [], "include": []},\n    "headers": [\n'
            '        {"name": "prefix", "value": "VB_EchoRunner"},\n        {"name": "product", "value": "dayz ugc"},\n'
            '        {"name": "version", "value": "1"}\n    ]\n}\n')
print("OK config.cpp строк", out.count("\n"), "строк перевода", len(S))
