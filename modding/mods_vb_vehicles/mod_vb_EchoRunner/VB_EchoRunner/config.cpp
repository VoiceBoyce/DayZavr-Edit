// Echo Runner (VB_EchoRunner) - self-built wedge coupe for DayZavr server 16.
// Based on vanilla Sedan_02 (rear engine, RWD): simulation, sounds, damage zones and script overrides
// were checked against P:\DZ\vehicles\wheeled\config.cpp and scripts\4_world\...\inheritedcars\sedan_02.c (1.30).
// Model axes: nose -Z, driver (LHD) on +X = left side; seats: 0 driver, 1 codriver.

class CfgPatches
{
	class VB_EchoRunner
	{
		units[] = {"VB_EchoRunner"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Vehicles_Wheeled", "DZ_Sounds_Effects"};
	};
};

class CfgMods
{
	class VB_EchoRunner
	{
		dir = "VB_EchoRunner";
		name = "Echo Runner";
		author = "DayZavr";
		type = "mod";
		dependencies[] = {"World"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_EchoRunner/scripts/4_World"};
			};
		};
	};
};

class CfgSlots
{
	class Slot_VB_ER_Wheel_1_1
	{
		name = "VB_ER_Wheel_1_1";
		displayName = "$STR_VB_ER_Slot_Wheel_1_1";
		selection = "wheel_1_1";
		ghostIcon = "set:dayz_inventory image:wheel";
	};
	class Slot_VB_ER_Wheel_1_2: Slot_VB_ER_Wheel_1_1
	{
		name = "VB_ER_Wheel_1_2";
		displayName = "$STR_VB_ER_Slot_Wheel_1_2";
		selection = "wheel_1_2";
	};
	class Slot_VB_ER_Wheel_2_1: Slot_VB_ER_Wheel_1_1
	{
		name = "VB_ER_Wheel_2_1";
		displayName = "$STR_VB_ER_Slot_Wheel_2_1";
		selection = "wheel_2_1";
	};
	class Slot_VB_ER_Wheel_2_2: Slot_VB_ER_Wheel_1_1
	{
		name = "VB_ER_Wheel_2_2";
		displayName = "$STR_VB_ER_Slot_Wheel_2_2";
		selection = "wheel_2_2";
	};
	class Slot_VB_ER_Door_Driver
	{
		name = "VB_ER_Door_Driver";
		displayName = "$STR_VB_ER_Slot_Door_Driver";
		selection = "doors_driver";
		ghostIcon = "set:dayz_inventory image:doorfront";
	};
	class Slot_VB_ER_Door_CoDriver: Slot_VB_ER_Door_Driver
	{
		name = "VB_ER_Door_CoDriver";
		displayName = "$STR_VB_ER_Slot_Door_CoDriver";
		selection = "doors_codriver";
	};
	class Slot_VB_ER_Hood
	{
		name = "VB_ER_Hood";
		displayName = "$STR_VB_ER_Slot_Hood";
		selection = "doors_hood";
		ghostIcon = "set:dayz_inventory image:hood";
	};
	class Slot_VB_ER_Trunk
	{
		name = "VB_ER_Trunk";
		displayName = "$STR_VB_ER_Slot_Trunk";
		selection = "doors_trunk";
		ghostIcon = "set:dayz_inventory image:trunk";
	};
};

class CfgVehicles
{
	class CarWheel;
	class CarDoor;
	class CarScript;
	class Crew;
	class Driver;
	class CoDriver;
	class SimulationModule;
	class Axles;
	class Front;
	class Rear;
	class Wheels;
	class Left;
	class Right;
	class AnimationSources;
	class DamageSystem;
	class GlobalHealth;
	class DamageZones;
	class Window;
	class Doors;
	class Health;

	// ---------------------------------------------------------------- wheel (one item for all 4 slots)
	class VB_EchoRunner_Wheel: CarWheel
	{
		scope = 2;
		displayName = "$STR_VB_ER_Wheel";
		descriptionShort = "$STR_VB_ER_Wheel_Desc";
		model = "\VB_EchoRunner\proxy\er_wheel.p3d";
		inventorySlot[] = {"VB_ER_Wheel_1_1", "VB_ER_Wheel_1_2", "VB_ER_Wheel_2_1", "VB_ER_Wheel_2_2"};
		rotationFlags = 8;
		radiusByDamage[] = {0, 0.36, 0.3, 0.34, 0.9998, 0.30, 0.9999, 0.26};
		radius = 0.36;
		width = 0.28;
		tyreType = 0;
		tyreOffroadResistance = 0.7;
		tyreRollResistance = 0.012;
		tyreGrip = 0.86;
		tyreRoughness = 1.0;
		tyreLongitudinalFriction = 0.88;
		tyreLateralFriction = 0.97;
	};
	class VB_EchoRunner_Wheel_Ruined: VB_EchoRunner_Wheel
	{
		radius = 0.28;
		width = 0.28;
		tyreOffroadResistance = 0.02;
		tyreRollResistance = 0.6;
		tyreGrip = 0.35;
		tyreRoughness = 0.2;
		tyreLongitudinalFriction = 0.35;
		tyreLateralFriction = 4.95;
	};

	// ---------------------------------------------------------------- gull-wing doors (rotationFlags: left 8, right 4)
	class VB_EchoRunner_Door_Driver: CarDoor
	{
		scope = 2;
		displayName = "$STR_VB_ER_Door_Driver";
		descriptionShort = "$STR_VB_ER_Door_Desc";
		model = "\VB_EchoRunner\proxy\er_door_driver.p3d";
		inventorySlot[] = {"VB_ER_Door_Driver"};
		rotationFlags = 8;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"\VB_EchoRunner\data\er_body_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_EchoRunner\data\er_body.rvmat"};
		class DamageSystem: DamageSystem
		{
			class GlobalHealth: GlobalHealth{};
			class DamageZones: DamageZones
			{
				class Window: Window
				{
					class Health: Health
					{
						healthLevels[] = {{1.0, {"dz\vehicles\wheeled\sedan_02\data\glass_i.rvmat"}}, {0.7, {}}, {0.5, {"dz\vehicles\wheeled\Sedan_02\data\glass_i_damage.rvmat"}}, {0.3, {"dz\vehicles\wheeled\Sedan_02\data\glass_i_destruct.rvmat"}}, {0.0, "hidden"}};
					};
				};
				class Doors: Doors
				{
					class Health: Health
					{
						RefTexsMats[] = {"\VB_EchoRunner\data\er_body.rvmat"};
						healthLevels[] = {{1.0, {"\VB_EchoRunner\data\er_body.rvmat"}}, {0.7, {"\VB_EchoRunner\data\er_body.rvmat"}}, {0.5, {"\VB_EchoRunner\data\er_body.rvmat"}}, {0.3, {"\VB_EchoRunner\data\er_body.rvmat"}}, {0.0, {"\VB_EchoRunner\data\er_body.rvmat"}}};
					};
				};
			};
		};
	};
	class VB_EchoRunner_Door_CoDriver: VB_EchoRunner_Door_Driver
	{
		displayName = "$STR_VB_ER_Door_CoDriver";
		model = "\VB_EchoRunner\proxy\er_door_codriver.p3d";
		inventorySlot[] = {"VB_ER_Door_CoDriver"};
		rotationFlags = 4;
	};

	// ---------------------------------------------------------------- lids: front = luggage (radiator), rear = engine
	class VB_EchoRunner_Hood: CarDoor
	{
		scope = 2;
		displayName = "$STR_VB_ER_Hood";
		descriptionShort = "$STR_VB_ER_Hood_Desc";
		model = "\VB_EchoRunner\proxy\er_hood.p3d";
		inventorySlot[] = {"VB_ER_Hood"};
		rotationFlags = 8;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"\VB_EchoRunner\data\er_body_co.paa"};
		hiddenSelectionsMaterials[] = {"\VB_EchoRunner\data\er_body.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1.0, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0.0, {}}};
				};
			};
		};
	};
	class VB_EchoRunner_Trunk: VB_EchoRunner_Hood
	{
		displayName = "$STR_VB_ER_Trunk";
		descriptionShort = "$STR_VB_ER_Trunk_Desc";
		model = "\VB_EchoRunner\proxy\er_trunk.p3d";
		inventorySlot[] = {"VB_ER_Trunk"};
	};

	// ---------------------------------------------------------------- the car
	class VB_EchoRunner: CarScript
	{
		scope = 2;
		displayName = "$STR_VB_ER_Car";
		descriptionShort = "$STR_VB_ER_Car_Desc";
		model = "\VB_EchoRunner\echorunner.p3d";
		attachments[] = {"CarBattery", "Reflector_1_1", "Reflector_2_1", "CarRadiator", "SparkPlug", "VB_ER_Hood", "VB_ER_Trunk", "VB_ER_Door_Driver", "VB_ER_Door_CoDriver", "VB_ER_Wheel_1_1", "VB_ER_Wheel_1_2", "VB_ER_Wheel_2_1", "VB_ER_Wheel_2_2"};
		// 0..8 are the CarScript light ABI (positional), then body camo and pipes
		hiddenSelections[] = {"light_1_1", "light_2_1", "light_brake_1_2", "light_brake_2_2", "light_reverse_1_2", "light_reverse_2_2", "light_1_2", "light_2_2", "light_dashboard", "camo", "neon"};
		hiddenSelectionsTextures[] = {"", "", "", "", "", "", "", "", "", "\VB_EchoRunner\data\er_body_co.paa", ""};
		hiddenSelectionsMaterials[] = {"\VB_EchoRunner\data\er_lamp.rvmat", "\VB_EchoRunner\data\er_lamp.rvmat", "\VB_EchoRunner\data\er_tail.rvmat", "\VB_EchoRunner\data\er_tail.rvmat", "\VB_EchoRunner\data\er_lamp.rvmat", "\VB_EchoRunner\data\er_lamp.rvmat", "\VB_EchoRunner\data\er_tail.rvmat", "\VB_EchoRunner\data\er_amber.rvmat", "\VB_EchoRunner\data\er_chrome.rvmat", "\VB_EchoRunner\data\er_body.rvmat", "\VB_EchoRunner\data\er_neon.rvmat"};
		dashboardMatOn = "\VB_EchoRunner\data\er_chrome_e.rvmat";
		dashboardMatOff = "\VB_EchoRunner\data\er_chrome.rvmat";
		frontReflectorMatOn = "\VB_EchoRunner\data\er_lamp_e.rvmat";
		frontReflectorMatOff = "\VB_EchoRunner\data\er_lamp.rvmat";
		brakeReflectorMatOn = "\VB_EchoRunner\data\er_tail_e.rvmat";
		brakeReflectorMatOff = "\VB_EchoRunner\data\er_tail.rvmat";
		ReverseReflectorMatOn = "\VB_EchoRunner\data\er_lamp_e.rvmat";
		ReverseReflectorMatOff = "\VB_EchoRunner\data\er_lamp.rvmat";
		TailReflectorMatOn = "\VB_EchoRunner\data\er_tail_e.rvmat";
		TailReflectorMatOff = "\VB_EchoRunner\data\er_tail.rvmat";
		fuelCapacity = 50;
		fuelConsumption = 9;
		class Crew: Crew
		{
			class Driver: Driver{};
			class CoDriver: CoDriver{};
		};
		class SimulationModule: SimulationModule
		{
			class Steering
			{
				maxSteeringAngle = 32;
				increaseSpeed[] = {0, 45, 30, 28, 60, 13, 100, 6};
				decreaseSpeed[] = {0, 90, 60, 70, 100, 50};
				centeringSpeed[] = {0, 0, 15, 35, 60, 65, 100, 90};
			};
			class Throttle
			{
				reactionTime = 0.7;
				defaultThrust = 0.75;
				gentleThrust = 0.5;
				turboCoef = 4.0;
				gentleCoef = 0.5;
			};
			class Brake
			{
				pressureBySpeed[] = {0, 0.85, 10, 0.75, 20, 0.45, 40, 0.35, 80, 0.4, 100, 0.45, 120, 0.65};
				gentleCoef = 0.7;
				minPressure = 0.3;
				reactionTime = 0.35;
				driverless = 0.1;
			};
			class Aerodynamics
			{
				frontalArea = 1.8;
				dragCoefficient = 0.33;
			};
			drive = "DRIVE_RWD";
			class Engine
			{
				torqueCurve[] = {700, 0, 1000, 110, 2000, 165, 3000, 190, 4500, 175, 6000, 140, 6800, 0};
				inertia = 0.25;
				frictionTorque = 90;
				rollingFriction = 0.5;
				viscousFriction = 0;
				rpmIdle = 900;
				rpmMin = 1000;
				rpmClutch = 1400;
				rpmRedline = 6200;
			};
			class Clutch
			{
				maxTorqueTransfer = 260;
				uncoupleTime = 0.1;
				coupleTime = 0.3;
			};
			class Gearbox
			{
				type = "GEARBOX_MANUAL";
				reverse = 3.27;
				ratios[] = {3.36, 2.05, 1.37, 1.03, 0.79};
			};
			class Axles: Axles
			{
				class Front: Front
				{
					maxBrakeTorque = 1400;
					wheelHubMass = 5;
					wheelHubRadius = 0.17;
					class Suspension
					{
						stiffness = 30000;
						compression = 1700;
						damping = 5000;
						travelMaxUp = 0.08;
						travelMaxDown = 0.09;
					};
					class Wheels: Wheels
					{
						class Left: Left
						{
							inventorySlot = "VB_ER_Wheel_1_1";
						};
						class Right: Right
						{
							inventorySlot = "VB_ER_Wheel_2_1";
						};
					};
				};
				class Rear: Rear
				{
					maxBrakeTorque = 1200;
					maxHandbrakeTorque = 2400;
					wheelHubMass = 5;
					wheelHubRadius = 0.17;
					class Differential
					{
						ratio = 3.44;
						type = "DIFFERENTIAL_OPEN";
					};
					class Suspension
					{
						stiffness = 36000;
						compression = 2000;
						damping = 5600;
						travelMaxUp = 0.09;
						travelMaxDown = 0.1;
					};
					class Wheels: Wheels
					{
						class Left: Left
						{
							inventorySlot = "VB_ER_Wheel_1_2";
						};
						class Right: Right
						{
							inventorySlot = "VB_ER_Wheel_2_2";
						};
					};
				};
			};
		};
		class Cargo
		{
			itemsCargoSize[] = {10, 20};
			allowOwnedCargoManipulation = 1;
			openable = 0;
		};
		class AnimationSources: AnimationSources
		{
			class hide_bumper_front
			{
				source = "user";
				initPhase = 0;
				animPeriod = 0.001;
			};
			class hide_bumper_rear: hide_bumper_front{};
		};
		class Sounds
		{
			thrust = 0.6;
			thrustTurbo = 1;
			thrustGentle = 0.3;
			thrustSmoothCoef = 0.02;
			camposSmoothCoef = 0.03;
			soundSetsFilter[] = {"Sedan_02_Engine_Offload_Ext_Rpm1_SoundSet", "Sedan_02_Engine_Offload_Ext_Rpm2_SoundSet", "Sedan_02_Engine_Offload_Ext_Rpm3_SoundSet", "Sedan_02_Engine_Offload_Ext_Rpm4_SoundSet", "Sedan_02_Engine_Offload_Ext_Rpm5_SoundSet", "Sedan_02_Engine_Ext_Rpm0_SoundSet", "Sedan_02_Engine_Ext_Rpm1_SoundSet", "Sedan_02_Engine_Ext_Rpm2_SoundSet", "Sedan_02_Engine_Ext_Rpm3_SoundSet", "Sedan_02_Engine_Ext_Rpm4_SoundSet", "Sedan_02_Engine_Ext_Rpm5_SoundSet", "Sedan_02_Engine_Ext_Broken_SoundSet", "Sedan_02_Tires_ice_slow_Ext_SoundSet", "Sedan_02_Tires_ice_fast_Ext_SoundSet", "Sedan_02_Tires_snow_slow_Ext_SoundSet", "Sedan_02_Tires_snow_fast_Ext_SoundSet", "Sedan_02_Tires_rock_slow_Ext_SoundSet", "Sedan_02_Tires_rock_fast_Ext_SoundSet", "Sedan_02_Tires_grass_slow_Ext_SoundSet", "Sedan_02_Tires_grass_fast_Ext_SoundSet", "Sedan_02_Tires_gravel_slow_Ext_SoundSet", "Sedan_02_Tires_gravel_fast_Ext_SoundSet", "Sedan_02_Tires_gravel_dust_fast_Ext_SoundSet", "Sedan_02_Tires_asphalt_slow_Ext_SoundSet", "Sedan_02_Tires_asphalt_fast_Ext_SoundSet", "Sedan_02_Tires_water_slow_Ext_SoundSet", "Sedan_02_Tires_water_fast_Ext_SoundSet", "Sedan_02_skid_dirt_SoundSet", "Sedan_02_dirt_turn_SoundSet", "Sedan_02_Rain_Ext_SoundSet", "Sedan_02_damper_left_SoundSet", "Sedan_02_damper_right_SoundSet"};
			soundSetsInt[] = {"Sedan_02_Tires_Asphalt_Fast_General_Int_SoundSet", "Sedan_02_Wind_SoundSet"};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1000;
					healthLevels[] = {{1.0, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0.0, {}}};
				};
			};
			class DamageZones
			{
				class Chassis
				{
					displayName = "$STR_CfgVehicleDmg_Chassis0";
					fatalInjuryCoef = -1;
					componentNames[] = {"dmgZone_chassis"};
					class Health
					{
						healthLevels[] = {{1.0, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0.0, {}}};
						hitpoints = 3000;
						transferToGlobalCoef = 0;
					};
					inventorySlots[] = {};
				};
				class Front
				{
					displayName = "$STR_CfgVehicleDmg_Bumper0";
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgZone_front"};
					componentNames[] = {"dmgZone_front"};
					class Health
					{
						healthLevels[] = {{1.0, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0.0, {}}};
						hitpoints = 1000;
						transferToGlobalCoef = 0;
					};
					transferToZonesNames[] = {"FuelTank"};
					transferToZonesCoefs[] = {0.2};
					inventorySlots[] = {"VB_ER_Hood", "CarRadiator", "Reflector_1_1", "Reflector_2_1", "VB_ER_Wheel_1_1", "VB_ER_Wheel_2_1"};
					inventorySlotsCoefs[] = {0.25, 0.3, 0.3, 0.3, 0.15, 0.15};
				};
				// CarScript.EEItemAttached for the headlight bulbs writes the health of these two zones:
				// without them OnDebugSpawn dies on HeadlightH7 (VM exception "Could not find zone Reflector_1_1").
				class Reflector_1_1
				{
					displayName = "$STR_CfgVehicleDmg_Reflector0";
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgZone_lights_1_1"};
					componentNames[] = {"dmgZone_lights_1_1"};
					class Health
					{
						healthLevels[] = {{1.0, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0.0, {}}};
						hitpoints = 10;
						transferToGlobalCoef = 0;
					};
					transferToZonesNames[] = {"Front"};
					transferToZonesCoefs[] = {1.0};
					inventorySlots[] = {"Reflector_1_1"};
					inventorySlotsCoefs[] = {1.0};
				};
				class Reflector_2_1: Reflector_1_1
				{
					memoryPoints[] = {"dmgZone_lights_2_1"};
					componentNames[] = {"dmgZone_lights_2_1"};
					inventorySlots[] = {"Reflector_2_1"};
				};
				class Back
				{
					displayName = "$STR_CfgVehicleDmg_Bumper0";
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgZone_back"};
					componentNames[] = {"dmgZone_back"};
					class Health
					{
						healthLevels[] = {{1.0, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0.0, {}}};
						hitpoints = 1000;
						transferToGlobalCoef = 0;
					};
					transferToZonesNames[] = {"Engine"};
					transferToZonesCoefs[] = {0.17};
					inventorySlots[] = {"VB_ER_Trunk", "VB_ER_Wheel_1_2", "VB_ER_Wheel_2_2"};
					inventorySlotsCoefs[] = {0.15, 0.1, 0.1};
				};
				class Roof
				{
					displayName = "$STR_CfgVehicleDmg_Roof0";
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgZone_roof"};
					componentNames[] = {"dmgZone_roof"};
					class Health
					{
						healthLevels[] = {{1.0, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0.0, {}}};
						hitpoints = 600;
						transferToGlobalCoef = 0;
					};
					inventorySlots[] = {"VB_ER_Door_Driver", "VB_ER_Door_CoDriver"};
					inventorySlotsCoefs[] = {0.1, 0.1};
				};
				class Engine
				{
					displayName = "$STR_CfgVehicleDmg_Engine0";
					fatalInjuryCoef = 0.001;
					memoryPoints[] = {"dmgZone_engine"};
					componentNames[] = {"dmgZone_engine"};
					class Health
					{
						healthLevels[] = {{1.0, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0.0, {}}};
						hitpoints = 1000;
						transferToGlobalCoef = 1;
					};
					inventorySlots[] = {"CarBattery", "SparkPlug"};
					inventorySlotsCoefs[] = {0.1, 0.05, 0.05};
				};
				class FuelTank
				{
					displayName = "$STR_CfgVehicleDmg_FuelTank0";
					fatalInjuryCoef = -1;
					componentNames[] = {"dmgZone_fuelTank"};
					class Health
					{
						healthLevels[] = {{1.0, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0.0, {}}};
						hitpoints = 400;
						transferToGlobalCoef = 0;
					};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {};
				};
			};
		};
		class ObstacleGenerator
		{
			carve = 1;
			timeToStationary = 5.0;
			moveThreshold = 0.5;
			class Shapes
			{
				class Cylindric
				{
					class Cyl1
					{
						radius = 1.0;
						height = 1.5;
						center[] = {0, 0, 0.8};
					};
					class Cyl3
					{
						radius = 1.0;
						height = 1.5;
						center[] = {0, 0, -0.8};
					};
				};
			};
		};
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name = "$STR_attachment_Engine0";
				description = "";
				icon = "set:dayz_inventory image:cat_vehicle_engine";
				attachmentSlots[] = {"CarBattery", "SparkPlug"};
			};
			class Trunk
			{
				name = "$STR_attachment_Body0";
				description = "";
				icon = "set:dayz_inventory image:cat_vehicle_body";
				attachmentSlots[] = {"CarRadiator"};
			};
			class Body
			{
				name = "$STR_attachment_Body0";
				description = "";
				icon = "set:dayz_inventory image:cat_vehicle_body";
				attachmentSlots[] = {"Reflector_1_1", "Reflector_2_1", "VB_ER_Door_Driver", "VB_ER_Door_CoDriver", "VB_ER_Hood", "VB_ER_Trunk"};
			};
			class Chassis
			{
				name = "$STR_attachment_Chassis0";
				description = "";
				icon = "set:dayz_inventory image:cat_vehicle_chassis";
				attachmentSlots[] = {"VB_ER_Wheel_1_1", "VB_ER_Wheel_1_2", "VB_ER_Wheel_2_1", "VB_ER_Wheel_2_2"};
			};
		};
	};
};

class CfgNonAIVehicles
{
	class ProxyVehiclePart;
	class Proxyer_wheel: ProxyVehiclePart
	{
		model = "VB_EchoRunner\proxy\er_wheel.p3d";
		inventorySlot[] = {"VB_ER_Wheel_1_1", "VB_ER_Wheel_1_2", "VB_ER_Wheel_2_1", "VB_ER_Wheel_2_2"};
	};
	class Proxyer_door_driver: ProxyVehiclePart
	{
		model = "VB_EchoRunner\proxy\er_door_driver.p3d";
		inventorySlot = "VB_ER_Door_Driver";
	};
	class Proxyer_door_codriver: ProxyVehiclePart
	{
		model = "VB_EchoRunner\proxy\er_door_codriver.p3d";
		inventorySlot = "VB_ER_Door_CoDriver";
	};
	class Proxyer_hood: ProxyVehiclePart
	{
		model = "VB_EchoRunner\proxy\er_hood.p3d";
		inventorySlot = "VB_ER_Hood";
	};
	class Proxyer_trunk: ProxyVehiclePart
	{
		model = "VB_EchoRunner\proxy\er_trunk.p3d";
		inventorySlot = "VB_ER_Trunk";
	};
};
