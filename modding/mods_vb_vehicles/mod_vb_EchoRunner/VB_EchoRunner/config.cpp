// VB_EchoRunner — Echo Runner (сервер 16). Основа — купленный Interceptor ALEX (пак @DayZavrVehicle3_ALX,
// переделка разрешена), сгенерировано tools/er_config.py — правки вносить в генератор.
class CfgPatches
{
	class VB_EchoRunner
	{
		units[]={"VB_EchoRunner"};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"DZ_Data","DZ_Vehicles_Wheeled","DZ_Vehicles_Parts","DZ_Sounds_Effects","VB_EchoCore"};
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
	class Slot_VB_ER_Wheel_1_1
	{
		name="VB_ER_Wheel_1_1";
		displayName="$STR_VB_ER_Wheel_name";
		selection="wheel_1_1";
		ghostIcon="wheel";
	};
	class Slot_VB_ER_Wheel_1_2
	{
		name="VB_ER_Wheel_1_2";
		displayName="$STR_VB_ER_Wheel_name";
		selection="wheel_1_2";
		ghostIcon="wheel";
	};
	class Slot_VB_ER_Wheel_2_1
	{
		name="VB_ER_Wheel_2_1";
		displayName="$STR_VB_ER_Wheel_name";
		selection="wheel_2_1";
		ghostIcon="wheel";
	};
	class Slot_VB_ER_Wheel_2_2
	{
		name="VB_ER_Wheel_2_2";
		displayName="$STR_VB_ER_Wheel_name";
		selection="wheel_2_2";
		ghostIcon="wheel";
	};
	class Slot_VB_ER_Door_Driver
	{
		name="VB_ER_Door_Driver";
		displayName="$STR_VB_ER_driver_name";
		selection="doors_driver";
		ghostIcon="doorfront";
	};
	class Slot_VB_ER_Door_CoDriver
	{
		name="VB_ER_Door_CoDriver";
		displayName="$STR_VB_ER_codriver_name";
		selection="doors_codriver";
		ghostIcon="doorfront";
	};
	class Slot_VB_ER_Hood
	{
		name="VB_ER_Hood";
		displayName="$STR_VB_ER_hood_name";
		selection="doors_hood";
		ghostIcon="hood";
	};
	class Slot_VB_ER_Tank_1
	{
		name="VB_ER_Tank_1";
		displayName="$STR_VB_ER_tank_name";
		ghostIcon="set:vb_echo_icons image:tank";
	};
	class Slot_VB_ER_Tank_2
	{
		name="VB_ER_Tank_2";
		displayName="$STR_VB_ER_tank_name";
		ghostIcon="set:vb_echo_icons image:tank";
	};
	class Slot_VB_ER_Pipes_L
	{
		name="VB_ER_Pipes_L";
		displayName="$STR_VB_ER_pipes_l_name";
		ghostIcon="set:vb_echo_icons image:pipes";
	};
	class Slot_VB_ER_Pipes_R
	{
		name="VB_ER_Pipes_R";
		displayName="$STR_VB_ER_pipes_r_name";
		ghostIcon="set:vb_echo_icons image:pipes";
	};
	class Slot_VB_ER_Pipes_Engine
	{
		name="VB_ER_Pipes_Engine";
		displayName="$STR_VB_ER_pipes_e_name";
		ghostIcon="set:vb_echo_icons image:pipes_engine";
	};
};
class CfgNonAIVehicles
{
	class ProxyVehiclePart;
	class Proxyvb_er_wheel: ProxyVehiclePart
	{
		model="VB_EchoRunner\proxy\vb_er_wheel.p3d";
		inventorySlot[]={"VB_ER_Wheel_1_1","VB_ER_Wheel_1_2","VB_ER_Wheel_2_1","VB_ER_Wheel_2_2"};
	};
	class Proxyvb_er_wheel_destroyed: ProxyVehiclePart
	{
		model="VB_EchoRunner\proxy\vb_er_wheel_destroyed.p3d";
		inventorySlot[]={"VB_ER_Wheel_1_1","VB_ER_Wheel_1_2","VB_ER_Wheel_2_1","VB_ER_Wheel_2_2"};
	};
	class Proxyvb_er_door_driver: ProxyVehiclePart
	{
		model="VB_EchoRunner\proxy\vb_er_door_driver.p3d";
		inventorySlot[]={"VB_ER_Door_Driver"};
	};
	class Proxyvb_er_door_codriver: ProxyVehiclePart
	{
		model="VB_EchoRunner\proxy\vb_er_door_codriver.p3d";
		inventorySlot[]={"VB_ER_Door_CoDriver"};
	};
	class Proxyvb_er_hood: ProxyVehiclePart
	{
		model="VB_EchoRunner\proxy\vb_er_hood.p3d";
		inventorySlot[]={"VB_ER_Hood"};
	};
	class Proxyvb_er_tank_1: ProxyVehiclePart
	{
		model="VB_EchoRunner\proxy\vb_er_tank_1.p3d";
		inventorySlot[]={"VB_ER_Tank_1"};
	};
	class Proxyvb_er_tank_2: ProxyVehiclePart
	{
		model="VB_EchoRunner\proxy\vb_er_tank_2.p3d";
		inventorySlot[]={"VB_ER_Tank_2"};
	};
	class Proxyvb_er_pipes_l: ProxyVehiclePart
	{
		model="VB_EchoRunner\proxy\vb_er_pipes_l.p3d";
		inventorySlot[]={"VB_ER_Pipes_L"};
	};
	class Proxyvb_er_pipes_r: ProxyVehiclePart
	{
		model="VB_EchoRunner\proxy\vb_er_pipes_r.p3d";
		inventorySlot[]={"VB_ER_Pipes_R"};
	};
	class Proxyvb_er_pipes_engine: ProxyVehiclePart
	{
		model="VB_EchoRunner\proxy\vb_er_pipes_engine.p3d";
		inventorySlot[]={"VB_ER_Pipes_Engine"};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class Container_Base;
	class Bottle_Base;
	class DamageSystem;
	class GlobalHealth;
	class Health;
	class DamageZones;
	class CarWheel;
	class CarDoor;
	class Window;
	class Doors;
	class CarScript;
	class Crew;
	class Driver;
	class CoDriver;
	class SimulationModule;
	class Axles;
	class Front;
	class Rear;
	class Left;
	class Right;
	class Wheels;
		class VB_EchoRunner_Wheel: CarWheel
	{
		scope=2;
		model="VB_EchoRunner\proxy\vb_er_wheel.p3d";
		displayName="$STR_VB_ER_Wheel_name";
		descriptionShort="$STR_VB_ER_wheel_disc";
		itemSize[]={6,6};
		weight=30000;
		physLayer="item_large";
		rotationFlags=8;
		inventorySlot[]=
		{
			"VB_ER_Wheel_1_1",
			"VB_ER_Wheel_1_2",
			"VB_ER_Wheel_2_1",
			"VB_ER_Wheel_2_2",
			""
		};
		repairableWithKits[]={6};
		repairCosts[]={30};
		radiusByDamage[]={0,0.42,0.30,0.39,0.99980003,0.34,0.99989998,0.30};
		radius=0.42;
		width=0.36;
		tyreRollDrag=15;
		tyreRollResistance=0.015;
		tyreTread=1;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=15000;
					healthLevels[]={{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.0,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class drop
				{
					soundset="hatchbackwheel_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class VB_EchoRunner_Wheel_Ruined: VB_EchoRunner_Wheel { scope=2; };
	class VB_EchoRunner_Wheel_Destroyed: CarWheel
	{
		model="VB_EchoRunner\proxy\vb_er_wheel_destroyed.p3d";
		itemSize[]={6,6};
		weight=30000;
		physLayer="item_large";
		inventorySlot[]=
		{
			"VB_ER_Wheel_1_1",
			"VB_ER_Wheel_1_2",
			"VB_ER_Wheel_2_1",
			"VB_ER_Wheel_2_2",
			""
		};
		radius=0.34;
		width=0.30;
		tyreRollResistance=1;
		tyreRollDrag=75;
		tyreRoughness=1.2;
		tyreTread=0.5;
	};
	class VB_EchoRunner: CarScript
	{
		scope=2;
		displayName="$STR_VB_ER_name";
		Model="VB_EchoRunner\vb_echorunner.p3d";
		hiddenSelections[]=
		{
			"light_left",
			"light_right",
			"light_brake_1_2",
			"light_brake_2_2",
			"light_reverse_1_2",
			"light_reverse_2_2",
			"light_1_2",
			"light_2_2",
			"light_dashboard",
			"dmgZone_chassis",
			"dmgZone_front",
			"dmgZone_back",
			"dmgZone_roof",
			"dmgZone_fender_1_1",
			"dmgZone_fender_1_2",
			"dmgZone_fender_2_1",
			"dmgZone_fender_2_2",
			"dmgZone_rura",
			"dmgZone_salon",
			"camo1",
			"neon",
			"screen"
		};
		attachments[]=
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
		};
		hiddenSelectionsTextures[]=
		{
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"vb_echorunner\data\chassis_co.paa",
			"vb_echorunner\data\body_co.paa",
			"vb_echorunner\data\body_co.paa",
			"vb_echorunner\data\body_co.paa",
			"vb_echorunner\data\body_co.paa",
			"vb_echorunner\data\body_co.paa",
			"vb_echorunner\data\body_co.paa",
			"vb_echorunner\data\body_co.paa",
			"vb_echorunner\data\chassis_co.paa",
			"vb_echorunner\data\salon_co.paa",
			"vb_echorunner\data\body_co.paa",
			"vb_echorunner\data\parts\er_neon_co.paa",
			"vb_echorunner\data\parts\er_screen_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\vehicles\wheeled\offroadhatchback\data\offroadhatchback_lights.rvmat",
			"dz\vehicles\wheeled\offroadhatchback\data\offroadhatchback_lights.rvmat",
			"dz\vehicles\wheeled\offroadhatchback\data\offroadhatchback_lights.rvmat",
			"dz\vehicles\wheeled\offroadhatchback\data\offroadhatchback_lights.rvmat",
			"dz\vehicles\wheeled\offroadhatchback\data\offroadhatchback_lights.rvmat",
			"dz\vehicles\wheeled\offroadhatchback\data\offroadhatchback_lights.rvmat",
			"dz\vehicles\wheeled\offroadhatchback\data\offroadhatchback_lights.rvmat",
			"dz\vehicles\wheeled\offroadhatchback\data\offroadhatchback_lights.rvmat",
			"dz\vehicles\wheeled\offroadhatchback\data\offroadhatchback_lights.rvmat",
			"vb_echorunner\data\chassis.rvmat",
			"vb_echorunner\data\body.rvmat",
			"vb_echorunner\data\body.rvmat",
			"vb_echorunner\data\body.rvmat",
			"vb_echorunner\data\body.rvmat",
			"vb_echorunner\data\body.rvmat",
			"vb_echorunner\data\body.rvmat",
			"vb_echorunner\data\body.rvmat",
			"vb_echorunner\data\chassis.rvmat",
			"vb_echorunner\data\salon.rvmat",
			"vb_echorunner\data\body.rvmat",
			"vb_echorunner\data\parts\er_neon_off.rvmat",
			"vb_echorunner\data\parts\er_screen_off.rvmat"
		};
		dashboardMatOn="vb_echorunner\data\base\dashlight.rvmat";
		dashboardMatOff="vb_echorunner\data\salon.rvmat";
		frontReflectorMatOn="vb_echorunner\data\base\swiatlo.rvmat";
		frontReflectorMatOff="vb_echorunner\data\body.rvmat";
		brakeReflectorMatOn="dz\vehicles\wheeled\offroadhatchback\data\OffroadHatchback_lights_e.rvmat";
		brakeReflectorMatOff="vb_echorunner\data\body.rvmat";
		ReverseReflectorMatOn="vb_echorunner\data\base\swiatlo.rvmat";
		ReverseReflectorMatOff="vb_echorunner\data\body.rvmat";
		TailReflectorMatOn="dz\vehicles\wheeled\offroadhatchback\data\OffroadHatchback_lights_e.rvmat";
		TailReflectorMatOff="vb_echorunner\data\body.rvmat";
		weight=1460000;
		rotationFlags=64;
		insideSoundCoef=0.89999998;
		brakeFluidCapacity=1;
		oilCapacity=4;
		coolantCapacity=6;
		brakeFluidLeakDebit[]={0,0};
		oilLeakDebit[]={0,0};
		coolantLeakDebit[]={0,0};
		brakeFluidForceCoef[]={0,1,1,1};
		damageFromOil[]={0,0,1,0};
		damageFromCoolant[]={0,0,1,0};
		engineBeltSlot="EngineBelt";
		batterySlot="CarBattery";
		electricPowerResName="power";
		electricConsumptionIgnition=3001;
		electricConsumptionEngine=0;
		electricConsumptionLights=0;
		electricOutputEngine=5;
		selectionDashboard="light_dashboard";
		selectionLightFrontL="light_left";
		selectionLightFrontR="light_right";
		selectionBrakeLights="light_break";
		fuelCapacity=72;
		fuelConsumption=20;
		class Sounds
		{
			thrust=0.60000002;
			thrustTurbo=1;
			thrustGentle=0.30000001;
			thrustSmoothCoef=0.1;
			camposSmoothCoef=0.029999999;
			soundSetsFilter[]=
			{
				"VB_ER_Engine_Offload_Ext_Rpm1_SoundSet",
				"VB_ER_Engine_Offload_Ext_Rpm2_SoundSet",
				"VB_ER_Engine_Offload_Ext_Rpm3_SoundSet",
				"VB_ER_Engine_Offload_Ext_Rpm4_SoundSet",
				"VB_ER_Engine_Offload_Ext_Rpm5_SoundSet",
				"VB_ER_Engine_Ext_Rpm0_SoundSet",
				"VB_ER_Engine_Ext_Rpm1_SoundSet",
				"VB_ER_Engine_Ext_Rpm2_SoundSet",
				"VB_ER_Engine_Ext_Rpm3_SoundSet",
				"VB_ER_Engine_Ext_Rpm4_SoundSet",
				"VB_ER_Engine_Ext_Rpm5_SoundSet",
				"VB_ER_Engine_Ext_Broken_SoundSet",
				"offroad_Tires_rock_slow_Ext_SoundSet",
				"offroad_Tires_rock_fast_Ext_SoundSet",
				"offroad_Tires_grass_slow_Ext_SoundSet",
				"offroad_Tires_grass_fast_Ext_SoundSet",
				"offroad_Tires_gravel_slow_Ext_SoundSet",
				"offroad_Tires_gravel_fast_Ext_SoundSet",
				"offroad_Tires_asphalt_slow_Ext_SoundSet",
				"offroad_Tires_asphalt_fast_Ext_SoundSet",
				"offroad_Tires_water_slow_Ext_SoundSet",
				"offroad_Tires_water_fast_Ext_SoundSet",
				"offroad_skid_dirt_SoundSet",
				"offroad_dirt_turn_SoundSet",
				"offroad_Rain_Ext_SoundSet"
			};
			soundSetsInt[]=
			{
				"offroad_Tires_Asphalt_Slow_General_Int_SoundSet",
				"offroad_Tires_Asphalt_Fast_General_Int_SoundSet",
				"offroad_Wind_SoundSet"
			};
		};
		class Crew: Crew
		{
			class Driver: Driver
			{
			};
			class CoDriver: CoDriver
			{
			};
		};
		class SimulationModule: SimulationModule
		{
			class Steering
			{
				maxSteeringAngle=30;
				increaseSpeed[]={0,40,30,20,100,5};
				decreaseSpeed[]={0,80,60,40,90,20};
				centeringSpeed[]={0,0,15,25,60,40,100,60};
			};
			class Throttle
			{
				reactionTime=0.89999998;
				defaultThrust=0.85000002;
				gentleThrust=0.69999999;
				turboCoef=8;
				gentleCoef=0.75;
			};
			class Brake
			{
				pressureBySpeed[]={0,0.98000002,10,0.81999999,20,0.75,40,0.75999999,60,0.64999998,80,0.68000001,100,0.83999997,120,0.88,160,0.98000002};
				reactionTime=0.050000001;
				driverless=0.13;
			};
			class Aerodynamics
			{
				frontalArea=2.1800001;
				dragCoefficient=0.56;
			};
			drive="DRIVE_AWD";
			class Engine
			{
				torqueCurve[]={650,0,900,208,1400,362,3400,536,6241,300,8000,0};
				inertia=0.69999999;
				frictionTorque=369;
				rollingFriction=0.69999999;
				viscousFriction=0.5;
				rpmIdle=800;
				rpmMin=900;
				rpmClutch=1400;
				rpmRedline=20000;
			};
			class Clutch
			{
				maxTorqueTransfer=640;
				uncoupleTime=0.12;
				coupleTime=0.15000001;
			};
			class Gearbox
			{
				type="GEARBOX_MANUAL";
				reverse=4.526;
				ratios[]={4.2670002,2.0999999,1.561,1.0700001,0.80699998};
			};
			class CentralDifferential
			{
				ratio=1.25;
				type="DIFFERENTIAL_LOCKED";
			};
			class Axles: Axles
			{
				class Front: Front
				{
					maxBrakeTorque=4000;
					maxHandbrakeTorque=3330;
					wheelHubMass=10;
					wheelHubRadius=0.30;
					class Differential
					{
						ratio=5.2;
						type="DIFFERENTIAL_LOCKED";
					};
					class Suspension
					{
						stiffness=60000;
						compression=3100;
						damping=7500;
						travelMaxUp=0.15000001;
						travelMaxDown=0.15000001;
					};
					class Wheels: Wheels
					{
						class Left: Left
						{
							animDamper="damper_1_1";
							inventorySlot="VB_ER_Wheel_1_1";
						};
						class Right: Right
						{
							animDamper="damper_2_1";
							inventorySlot="VB_ER_Wheel_2_1";
						};
					};
				};
				class Rear: Rear
				{
					maxBrakeTorque=3000;
					maxHandbrakeTorque=2500;
					wheelHubMass=10;
					wheelHubRadius=0.30;
					class Differential
					{
						ratio=5.2;
						type="DIFFERENTIAL_LOCKED";
					};
					class Suspension
					{
						stiffness=60000;
						compression=4200;
						damping=7600;
						travelMaxUp=0.090000004;
						travelMaxDown=0.090000004;
					};
					class Wheels: Wheels
					{
						class Left: Left
						{
							animDamper="damper_1_2";
							inventorySlot="VB_ER_Wheel_1_2";
						};
						class Right: Right
						{
							animDamper="damper_2_2";
							inventorySlot="VB_ER_Wheel_2_2";
						};
					};
				};
			};
		};
		class Cargo
		{
			itemsCargoSize[]={10,100};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=10000;
					healthLevels[]=
					{
						
						{
							1.001,
							{}
						},
						
						{
							0.70099998,
							{}
						},
						
						{
							0.50099999,
							{}
						},
						
						{
							0.301,
							{}
						},
						
						{
							0.001,
							{}
						}
					};
				};
			};
			class DamageZones
			{
				class Chassis
				{
					class Health
					{
						hitpoints=3000;
						transferToGlobalCoef=0;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\chassis.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"vb_echorunner\data\chassis.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\base\body_destruct.rvmat"
								}
							}
						};
					};
					fatalInjuryCoef=-1;
					displayName="$STR_CfgVehicleDmg_Chassis0";
					componentNames[]=
					{
						"dmgZone_chassis"
					};
					inventorySlots[]={};
				};
				class Front
				{
					class Health
					{
						hitpoints=2000;
						transferToGlobalCoef=0;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\body.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"vb_echorunner\data\body.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\base\body_destruct.rvmat"
								}
							}
						};
					};
					fatalInjuryCoef=-1;
					displayName="$STR_CfgVehicleDmg_Bumper0";
					memoryPoints[]=
					{
						"dmgZone_front"
					};
					componentNames[]=
					{
						"dmgZone_front"
					};
					transferToZonesNames[]=
					{
						"Fender_1_1",
						"Fender_2_1",
						"Engine",
						"WindowFront"
					};
					transferToZonesThreshold[]={0.5,0.5,0.5,0.1};
					transferToZonesCoefs[]={0.69999999,0.69999999,0.5,0.050000001};
					inventorySlots[]=
					{
						"VB_ER_Hood",
						"CarRadiator"
					};
					inventorySlotsCoefs[]={0.30000001,0.30000001};
				};
				class Reflector_1_1
				{
					fatalInjuryCoef=-1;
					memoryPoints[]=
					{
						"dmgZone_lights_1_1"
					};
					componentNames[]=
					{
						"dmgZone_lights_1_1"
					};
					class Health
					{
						hitpoints=10;
						transferToGlobalCoef=0;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\body.rvmat"
								},
								
								{
									"vb_echorunner\data\body.rvmat"
								}
							},
							
							{
								0.70099998,
								{}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\body.rvmat"
								},
								
								{
									"vb_echorunner\data\body.rvmat"
								}
							},
							
							{
								0.301,
								{}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\body.rvmat"
								},
								
								{
									"dz\vehicles\wheeled\civiliansedan\data\civiliansedan_cr_destruct.rvmat"
								}
							}
						};
					};
					transferToZonesNames[]=
					{
						"Front",
						"Fender_1_1"
					};
					transferToZonesCoefs[]={1,1};
					displayName="$STR_CfgVehicleDmg_Reflector0";
					inventorySlots[]=
					{
						"",
						"VB_ER_Wheel_1_1"
					};
					inventorySlotsCoefs[]={1,0.89999998};
				};
				class Reflector_2_1: Reflector_1_1
				{
					memoryPoints[]=
					{
						"dmgZone_lights_2_1"
					};
					componentNames[]=
					{
						"dmgZone_lights_2_1"
					};
					transferToZonesNames[]=
					{
						"Front",
						"Fender_2_1"
					};
					inventorySlots[]=
					{
						"",
						"VB_ER_Wheel_2_1"
					};
				};
				class Back
				{
					class Health
					{
						hitpoints=1000;
						transferToGlobalCoef=0;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\body.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"vb_echorunner\data\body.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\base\body_destruct.rvmat"
								}
							}
						};
					};
					fatalInjuryCoef=-1;
					displayName="$STR_CfgVehicleDmg_Bumper0";
					memoryPoints[]=
					{
						"dmgZone_back"
					};
					componentNames[]=
					{
						"dmgZone_back"
					};
					transferToZonesNames[]=
					{
						"Fender_1_2",
						"Fender_2_2",
						"WindowLR",
						"WindowRR"
					};
					transferToZonesCoefs[]={0.30000001,0.30000001,0.2,0.2};
					inventorySlots[]=
					{
						"",
						""
					};
					inventorySlotsCoefs[]={0.80000001};
				};
				class Roof
				{
					class Health
					{
						hitpoints=1000;
						transferToGlobalCoef=0;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\body.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"vb_echorunner\data\body.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\base\body_destruct.rvmat"
								}
							}
						};
					};
					fatalInjuryCoef=-1;
					displayName="$STR_CfgVehicleDmg_Roof0";
					memoryPoints[]=
					{
						"dmgZone_roof"
					};
					componentNames[]=
					{
						"dmgZone_roof"
					};
					inventorySlotsCoefs[]={0};
					inventorySlots[]={};
				};
				class Fender_1_1
				{
					class Health
					{
						hitpoints=1000;
						transferToGlobalCoef=0;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\body.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"vb_echorunner\data\body.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\base\body_destruct.rvmat"
								}
							}
						};
					};
					fatalInjuryCoef=-1;
					displayName="$STR_CfgVehicleDmg_Fender0";
					memoryPoints[]=
					{
						"dmgZone_fender_1_1"
					};
					componentNames[]=
					{
						"dmgZone_fender_1_1"
					};
					transferToZonesNames[]=
					{
						"Front",
						"Engine",
						"Reflector_1_1",
						"WindowFront"
					};
					transferToZonesThreshold[]={0.2,0.5,0.89999998,0.5};
					transferToZonesCoefs[]={0.30000001,0.60000002,0.60000002,0.40000001};
					inventorySlots[]=
					{
						"VB_ER_Hood",
						"VB_ER_Wheel_1_1",
						"VB_ER_Door_Driver"
					};
					inventorySlotsCoefs[]={0.30000001,0.30000001,0.30000001};
				};
				class Fender_1_2: Fender_1_1
				{
					memoryPoints[]=
					{
						"dmgZone_fender_1_2"
					};
					componentNames[]=
					{
						"dmgZone_fender_1_2"
					};
					transferToZonesNames[]=
					{
						"Back",
						"FuelTank",
						"WindowLR"
					};
					transferToZonesThreshold[]={0.2,0.2,0.2};
					transferToZonesCoefs[]={0.30000001,0.2,0.69999999};
					inventorySlots[]=
					{
						"",
						"",
						"VB_ER_Wheel_1_2"
					};
					inventorySlotsCoefs[]={0.30000001,0.1};
				};
				class Fender_2_1: Fender_1_1
				{
					memoryPoints[]=
					{
						"dmgZone_fender_2_1"
					};
					componentNames[]=
					{
						"dmgZone_fender_2_1"
					};
					transferToZonesNames[]=
					{
						"Front",
						"Engine",
						"Reflector_2_1",
						"WindowFront",
						"Battery"
					};
					transferToZonesThreshold[]={0.2,0.5,0.89999998,0.5,0.5};
					transferToZonesCoefs[]={0.30000001,0.60000002,0.60000002,0.40000001,0.69999999};
					inventorySlots[]=
					{
						"VB_ER_Hood",
						"VB_ER_Wheel_2_1",
						"VB_ER_Door_CoDriver"
					};
					inventorySlotsCoefs[]={0.30000001,0.2,3};
				};
				class Fender_2_2: Fender_1_1
				{
					memoryPoints[]=
					{
						"dmgZone_fender_2_2"
					};
					componentNames[]=
					{
						"dmgZone_fender_2_2"
					};
					transferToZonesNames[]=
					{
						"Back",
						"FuelTank",
						"WindowRR"
					};
					transferToZonesThreshold[]={0.2,0.2,0.2};
					transferToZonesCoefs[]={0.30000001,0.2,0.69999999};
					inventorySlots[]=
					{
						"",
						"",
						"VB_ER_Wheel_2_2"
					};
					inventorySlotsCoefs[]={0.30000001,0.1};
				};
				class rura
				{
					class Health
					{
						hitpoints=2000;
						transferToGlobalCoef=0;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\chassis.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"vb_echorunner\data\chassis.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\base\body_destruct.rvmat"
								}
							}
						};
					};
					fatalInjuryCoef=-1;
					memoryPoints[]=
					{
						"dmgZone_rura"
					};
					componentNames[]=
					{
						"dmgZone_rura"
					};
					inventorySlotsCoefs[]={0};
				};
				class salon
				{
					class Health
					{
						hitpoints=2000;
						transferToGlobalCoef=0;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\salon.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"vb_echorunner\data\salon.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\base\body_destruct.rvmat"
								}
							}
						};
					};
					fatalInjuryCoef=-1;
					memoryPoints[]=
					{
						"dmgZone_salon"
					};
					componentNames[]=
					{
						"dmgZone_salon"
					};
					inventorySlotsCoefs[]={0};
				};
				class Engine
				{
					class Health
					{
						hitpoints=3000;
						transferToGlobalCoef=1;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\engine.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"vb_echorunner\data\engine.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\base\body_destruct.rvmat"
								}
							}
						};
					};
					fatalInjuryCoef=0.001;
					displayName="$STR_CfgVehicleDmg_Engine0";
					memoryPoints[]=
					{
						"dmgZone_engine"
					};
					componentNames[]=
					{
						"dmgZone_engine"
					};
					inventorySlots[]=
					{
						"CarBattery",
						"CarRadiator"
					};
					inventorySlotsCoefs[]={0.2,0.2,0.40000001};
				};
				class FuelTank
				{
					class Health
					{
						hitpoints=1600;
						transferToGlobalCoef=0;
						healthLevels[]=
						{
							
							{
								1.001,
								{}
							},
							
							{
								0.70099998,
								{}
							},
							
							{
								0.50099999,
								{}
							},
							
							{
								0.301,
								{}
							},
							
							{
								0.001,
								{}
							}
						};
					};
					fatalInjuryCoef=-1;
					displayName="$STR_CfgVehicleDmg_FuelTank0";
					componentNames[]=
					{
						"dmgZone_fuelTank"
					};
					inventorySlots[]={};
				};
			};
		};
		class GUIInventoryAttachmentsProps
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
				icon="set:vb_echo_icons image:tank";
			};
			class BaseBuildingContainers
			{
				name="$STR_attachment_containers0";
				description="";
				attachmentSlots[]={"CanisterGasoline","Shoulder","Back","VestGrenadeA","VestGrenadeB","VestGrenadeC","VestGrenadeD"};
				icon="cat_common_cargo";
			};
		};
		class ObstacleGenerator
		{
			carve=1;
			timeToStationary=5;
			moveThreshold=0.5;
			class Shapes
			{
				class Cylindric
				{
					class Cyl1
					{
						radius=1;
						height=1.5;
						center[]={0,0,0.69999999};
					};
					class Cyl3
					{
						radius=1;
						height=1.5;
						center[]={0,0,-0.69999999};
					};
				};
			};
		};
		class AnimationSources
		{
			class DoorsDriver
			{
				source="user";
				initPhase=0;
				animPeriod=0.5;
			};
			class DoorsCoDriver: DoorsDriver
			{
			};
			class DoorsHood: DoorsDriver
			{
			};
			class DoorsTrunk: DoorsDriver
			{
			};
			class HideDestroyed_1_1
			{
				source="user";
				initPhase=0;
				animPeriod=0.001;
			};
			class HideDestroyed_1_2
			{
				source="user";
				initPhase=0;
				animPeriod=0.001;
			};
			class HideDestroyed_2_1
			{
				source="user";
				initPhase=0;
				animPeriod=0.001;
			};
			class HideDestroyed_2_2
			{
				source="user";
				initPhase=0;
				animPeriod=0.001;
			};
			class AnimHitWheel_1_1
			{
				source="Hit";
				hitpoint="HitWheel_1_1";
				raw=1;
			};
			class AnimHitWheel_1_2: AnimHitWheel_1_1
			{
				hitpoint="HitWheel_1_2";
			};
			class AnimHitWheel_2_1: AnimHitWheel_1_1
			{
				hitpoint="HitWheel_2_1";
			};
			class AnimHitWheel_2_2: AnimHitWheel_1_1
			{
				hitpoint="HitWheel_2_2";
			};
			class HitDoorsHood: AnimHitWheel_1_1
			{
				hitpoint="HitDoorsHood";
			};
			class HitDoorsTrunk: AnimHitWheel_1_1
			{
				hitpoint="HitDoorsTrunk";
			};
			class HitDoorsDrivers: AnimHitWheel_1_1
			{
				hitpoint="HitDoorsDriver";
			};
			class HitDoorsCoDrivers: AnimHitWheel_1_1
			{
				hitpoint="HitDoorsCoDriver";
			};
			class HitDoorsCargo: AnimHitWheel_1_1
			{
				hitpoint="HitDoorsCargo";
			};
		};
	};
	class VB_EchoRunner_Door_Driver: CarDoor
	{
		scope=2;
		displayName="$STR_VB_ER_driver_name";
		descriptionShort="$STR_VB_ER_doors_driver_disc";
		model="VB_EchoRunner\proxy\vb_er_door_driver.p3d";
		weight=15000;
		itemSize[]={6,6};
		itemBehaviour=0;
		inventorySlot="VB_ER_Door_Driver";
		rotationFlags=8;
		physLayer="item_large";
		hiddenSelections[]=
		{
			"camo1",
			"camo2",
			"camo3",
			"neon"
		};
		hiddenSelectionsTextures[]=
		{
			"vb_echorunner\data\Body_co.paa",
			"",
			"",
			"vb_echorunner\data\parts\er_neon_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"vb_echorunner\data\Body.rvmat",
			"",
			"",
			"vb_echorunner\data\parts\er_neon_off.rvmat"
		};
		class DamageSystem: DamageSystem
		{
			class GlobalHealth: GlobalHealth
			{
				class Health: Health
				{
					hitpoints=5000;
					healthLevels[]=
					{
						
						{
							1.001,
							{}
						},
						
						{
							0.70099998,
							{}
						},
						
						{
							0.50099999,
							{}
						},
						
						{
							0.301,
							{}
						},
						
						{
							0.001,
							{}
						}
					};
				};
			};
			class DamageZones: DamageZones
			{
				class Window: Window
				{
					class Health: Health
					{
						hitpoints=5000;
						transferToGlobalCoef=1;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\salon.rvmat"
								}
							},
							
							{
								0.70099998,
								{}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								{}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\base\body_destruct.rvmat"
								}
							}
						};
					};
					componentNames[]=
					{
						"dmgZone_window"
					};
					fatalInjuryCoef=-1;
					inventorySlots[]={};
				};
				class Doors: Doors
				{
					class Health: Health
					{
						hitpoints=5000;
						transferToGlobalCoef=1;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\Body.rvmat"
								}
							},
							
							{
								0.70099998,
								{}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								{}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\base\body_destruct.rvmat"
								}
							}
						};
					};
					componentNames[]=
					{
						"dmgZone_doors"
					};
					fatalInjuryCoef=-1;
					inventorySlots[]={};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUp
				{
					soundSet="hatchbackdoors_driver_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="hatchbackdoors_driver_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class VB_EchoRunner_Door_CoDriver: VB_EchoRunner_Door_Driver
	{
		displayName="$STR_VB_ER_codriver_name";
		descriptionShort="$STR_VB_ER_doors_codriver_disc";
		model="VB_EchoRunner\proxy\vb_er_door_codriver.p3d";
		inventorySlot="VB_ER_Door_CoDriver";
	};
	class VB_EchoRunner_Hood: CarDoor
	{
		scope=2;
		displayName="$STR_VB_ER_hood_name";
		descriptionShort="$STR_VB_ER_hood_disc";
		model="VB_EchoRunner\proxy\vb_er_hood.p3d";
		weight=10000;
		itemSize[]={6,6};
		itemBehaviour=0;
		inventorySlot="VB_ER_Hood";
		rotationFlags=4;
		physLayer="item_large";
		hiddenSelections[]=
		{
			"camo1",
			"camo2",
			"camo3"
		};
		hiddenSelectionsTextures[]=
		{
			"vb_echorunner\data\Body_co.paa",
			"",
			""
		};
		hiddenSelectionsMaterials[]=
		{
			"vb_echorunner\data\Body.rvmat",
			"",
			""
		};
		class DamageSystem: DamageSystem
		{
			class GlobalHealth: GlobalHealth
			{
				class Health: Health
				{
					hitpoints=5000;
					healthLevels[]=
					{
						
						{
							1.001,
							{}
						},
						
						{
							0.70099998,
							{}
						},
						
						{
							0.50099999,
							{}
						},
						
						{
							0.301,
							{}
						},
						
						{
							0.001,
							{}
						}
					};
				};
			};
			class DamageZones: DamageZones
			{
				class Doors: Doors
				{
					class Health: Health
					{
						hitpoints=15000;
						transferToGlobalCoef=1;
						healthLevels[]=
						{
							
							{
								1.001,
								
								{
									"vb_echorunner\data\Body.rvmat"
								}
							},
							
							{
								0.70099998,
								{}
							},
							
							{
								0.50099999,
								
								{
									"vb_echorunner\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								{}
							},
							
							{
								0.001,
								
								{
									"vb_echorunner\data\base\body_destruct.rvmat"
								}
							}
						};
					};
					componentNames[]=
					{
						"dmgZone_doors"
					};
					fatalInjuryCoef=-1;
					inventorySlots[]={};
				};
			};
		};
	};

	class VB_EchoRunner_RiftPart: Inventory_Base
	{
		scope=0;
		weight=12000;
		weightPerQuantityUnit=0;		// у Inventory_Base 1 г на единицу: полный бак (20 000 kE) весил бы +20 кг (E123, 11-23)
		itemSize[]={6,3};
		itemBehaviour=0;
		physLayer="item_large";
		rotationFlags=0;
		hiddenSelections[]={"neon"};
		hiddenSelectionsTextures[]={"vb_echorunner\data\parts\er_neon_co.paa"};
		hiddenSelectionsMaterials[]={"vb_echorunner\data\parts\er_neon_idle.rvmat"};		// снятая деталь еле тлеет (превью, 11-18)
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
		model="VB_EchoRunner\proxy\vb_er_tank.p3d";
		weight=35000;
		itemSize[]={6,5};
		// + гнёзда зарядной станции базы (VB_EchoBase, слоты в ядре VB_EchoCore; станция v2 — 11-20)
		inventorySlot[]={"VB_ER_Tank_1","VB_ER_Tank_2","VB_Echo_ChargeTank1","VB_Echo_ChargeTank2"};
		hiddenSelectionsTextures[]={"vb_echorunner\data\parts\er_neon_empty_co.paa"};		// новый бак пустой — серый; заряженный красит скрипт (11-20)
		hiddenSelectionsMaterials[]={"vb_echorunner\data\parts\er_neon_empty.rvmat"};
		// энергия Эхо 0–20 000 kE (шкала ×32, 11-20) = количество предмета (полоса в инвентаре, сохраняется ванилью); расход — скрипт машины
		// (полёт 2 kE/tick, висение 1 kE/tick, тик = 10 с; решение пользователя 11-20 — единица kE, степени двойки).
		// Носитель энергии ядра VB_EchoCore (блок VB_Echo, ТЗ modding/mod_vb_Echo/ТЗ_VB_Echo.md)
		stackedUnit="percentage";
		quantityBar=1;
		varQuantityInit=0;
		varQuantityMin=0;
		varQuantityMax=20000;
		varQuantityDestroyOnMin=0;
		class VB_Echo
		{
			kind="echo";
			capacity=20000;
			infinite=0;
			transfer=1;
			storage="quantity";
		};
	};
	// бак холодного синтеза (донат): та же модель, бирюзовый неон; заряд всегда полный, расход не списывается,
	// перелить нельзя; работает только парой, с баком Эхо не смешивается (решения пользователя 11-20)
	class VB_EchoRunner_TankFusion: VB_EchoRunner_Tank
	{
		scope=2;
		displayName="$STR_VB_ER_tank_fusion_name";
		descriptionShort="$STR_VB_ER_tank_fusion_disc";
		inventorySlot[]={"VB_ER_Tank_1","VB_ER_Tank_2"};		// синтез не заряжают: гнёзд станции нет
		hiddenSelectionsTextures[]={"vb_echorunner\data\parts\er_neon_fusion_co.paa"};
		hiddenSelectionsMaterials[]={"vb_echorunner\data\parts\er_neon_idle.rvmat"};
		varQuantityInit=20000;
		class VB_Echo
		{
			kind="fusion";
			capacity=20000;
			infinite=1;
			transfer=0;
			storage="quantity";
		};
	};
	// антиграв-модуль ДВ-4 (этап 8): ставится вместо колеса; наклон и роторы — источники user (скрипт машины).
	// Пара «_Ruined» обязательна для любого CarWheel (InventoryItemType.SetReplacementTypes, иначе ошибка VM)
	class VB_EchoRunner_GravModule: CarWheel
	{
		scope=2;
		displayName="$STR_VB_ER_grav_name";
		descriptionShort="$STR_VB_ER_grav_disc";
		model="VB_EchoRunner\proxy\vb_er_gravmodule.p3d";
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
		hiddenSelectionsTextures[]={"vb_echorunner\data\parts\er_neon_empty_co.paa"};		// без энергии — пустой серый (11-20)
		hiddenSelectionsMaterials[]={"vb_echorunner\data\parts\er_neon_empty.rvmat"};
		class AnimationSources
		{
			class rotor
			{
				source="user";
				animPeriod=0.01;
				initPhase=0;
			};
			// у каждой анимации свой источник: общий «rotor» у кольца и крестовины сдвигал запись фаз на соседнюю
			// анимацию (баг 49, E104)
			class rotor_b
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
			class unspin
			{
				source="user";
				animPeriod=0.01;
				initPhase=0;		// Баги №61: поворот модуля обратно на угол колеса (кость hub, VB_EchoRunnerFx)
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
		model="VB_EchoRunner\proxy\vb_er_pipes_l.p3d";
		inventorySlot[]={"VB_ER_Pipes_L"};
		hiddenSelectionsTextures[]={"vb_echorunner\data\parts\er_neon_empty_co.paa"};		// без энергии — пустой серый (11-20)
		hiddenSelectionsMaterials[]={"vb_echorunner\data\parts\er_neon_empty.rvmat"};
	};
	class VB_EchoRunner_Pipes_R: VB_EchoRunner_RiftPart
	{
		scope=2;
		displayName="$STR_VB_ER_pipes_r_name";
		descriptionShort="$STR_VB_ER_pipes_disc";
		model="VB_EchoRunner\proxy\vb_er_pipes_r.p3d";
		inventorySlot[]={"VB_ER_Pipes_R"};
		hiddenSelectionsTextures[]={"vb_echorunner\data\parts\er_neon_empty_co.paa"};		// без энергии — пустой серый (11-20)
		hiddenSelectionsMaterials[]={"vb_echorunner\data\parts\er_neon_empty.rvmat"};
	};
	class VB_EchoRunner_Pipes_Engine: VB_EchoRunner_RiftPart
	{
		scope=2;
		displayName="$STR_VB_ER_pipes_e_name";
		descriptionShort="$STR_VB_ER_pipes_disc";
		model="VB_EchoRunner\proxy\vb_er_pipes_engine.p3d";
		inventorySlot[]={"VB_ER_Pipes_Engine"};
		hiddenSelectionsTextures[]={"vb_echorunner\data\parts\er_neon_empty_co.paa"};		// без энергии — пустой серый (11-20)
		hiddenSelectionsMaterials[]={"vb_echorunner\data\parts\er_neon_empty.rvmat"};
	};
};
class CfgSoundShaders
{
	class VB_ER_GravIdle_SoundShader
	{
		samples[]={{"VB_EchoRunner\sounds\vb_er_grav_idle",1}};
		volume=0.8;
		range=60;
		rangeCurve[]={{0,1},{15,0.7},{35,0.3},{60,0}};
	};
	class VB_ER_GravFly_SoundShader
	{
		samples[]={{"VB_EchoRunner\sounds\vb_er_grav_fly",1}};
		volume=1.0;
		range=150;
		rangeCurve[]={{0,1},{30,0.6},{80,0.25},{150,0}};
	};
	class VB_ER_GravThrust_SoundShader
	{
		samples[]={{"VB_EchoRunner\sounds\vb_er_grav_thrust",1}};
		volume=1.0;
		range=250;
		rangeCurve[]={{0,1},{40,0.6},{120,0.25},{250,0}};
	};
	class baseEngineVB_ER_SoundShader
	{
		range=80;
	};
	class VB_ER_Engine_Ext_Rpm0_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Rpm0",
				1
			}
		};
		frequency="1 * ((850 + ((rpm - 850)/(8000/5600))) max 850) / 850";
		volume="0.75 * 1 * engineOn * 0.4 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((850+1200)/2) + 2.5*50),(((850+1200)/2) - 50)]) * ((1 - 0.25*doors) max campos)";
	};
	class VB_ER_Engine_Ext_Rpm1_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Rpm1",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 1200";
		volume="0.75 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.6 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((850+1200)/2) - 2.5*50),(((850+1200)/2) + 50)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) + 2.5*150),(((1200+1700)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class VB_ER_Engine_Offload_Ext_Rpm1_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Offload_Rpm1",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 1200";
		volume="0.75 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.6 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((850+1200)/2) - 2.5*50),(((850+1200)/2) + 50)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) + 2.5*150),(((1200+1700)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class VB_ER_Engine_Ext_Rpm2_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Rpm2",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 1700";
		volume="0.75 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.85 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) - 2.5*150),(((1200+1700)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) + 2.5*150),(((1700+2300)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class VB_ER_Engine_Offload_Ext_Rpm2_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Offload_Rpm2",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 1700";
		volume="0.75 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.85 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) - 2.5*150),(((1200+1700)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) + 2.5*150),(((1700+2300)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class VB_ER_Engine_Ext_Rpm3_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Rpm3",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 2300";
		volume="0.75 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) - 2.5*150),(((1700+2300)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) + 2.5*200),(((2300+3150)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class VB_ER_Engine_Offload_Ext_Rpm3_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Offload_Rpm3",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 2300";
		volume="0.75 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) - 2.5*150),(((1700+2300)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) + 2.5*200),(((2300+3150)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class VB_ER_Engine_Ext_Rpm4_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Rpm4",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 3150";
		volume="0.75 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) - 2.5*200),(((2300+3150)/2) + 200)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) + 2.5*200),(((3150+4400)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class VB_ER_Engine_Offload_Ext_Rpm4_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Offload_Rpm4",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 3150";
		volume="0.75 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) - 2.5*200),(((2300+3150)/2) + 200)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) + 2.5*200),(((3150+4400)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class VB_ER_Engine_Ext_Rpm5_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Rpm5",
				2
			}
		};
		frequency="(1 * (850 + ((rpm - 850)/(8000/5600))) / 4400) min (8000/5600)";
		volume="0.75 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) - 2.5*200),(((3150+4400)/2) + 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class VB_ER_Engine_Offload_Ext_Rpm5_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Offload_Rpm5",
				2
			}
		};
		frequency="(1 * (850 + ((rpm - 850)/(8000/5600))) / 4400) min (8000/5600)";
		volume="0.75 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) - 2.5*200),(((3150+4400)/2) + 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class VB_ER_Engine_Ext_Broken_SoundShader: baseEngineVB_ER_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\Interceptor_Engine_Ext_Broken",
				2
			}
		};
		frequency=0.89999998;
		volume="0.75 * 1 * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) - 2.5*200),(((3150+4400)/2) + 200)]) * ((1 - 0.25*doors) max campos) * (rpm factor[6000,7200])";
	};
	class baseVB_EREngine_StartStop_SoundShader
	{
		range=80;
	};
	class VB_ER_Engine_start_SoundShader: baseVB_EREngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\enginestartstop\engine_start",
				1
			}
		};
		volume=1.2;
	};
	class VB_ER_Engine_failed_start_fuel_SoundShader: baseVB_EREngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\enginestartstop\engine_failed_start_fuel",
				1
			}
		};
		volume=1;
	};
	class VB_ER_Engine_failed_start_battery_SoundShader: baseVB_EREngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\enginestartstop\engine_failed_start_battery",
				1
			}
		};
		volume=1;
	};
	class VB_ER_Engine_failed_start_sparkplugs_SoundShader: baseVB_EREngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\enginestartstop\engine_failed_start_sparkplugs",
				1
			}
		};
		volume=1;
	};
	class VB_ER_Engine_stop_SoundShader: baseVB_EREngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\enginestartstop\engine_stop",
				1
			}
		};
		volume=1;
	};
	class VB_ER_Engine_stop_fuel_SoundShader: baseVB_EREngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"VB_EchoRunner\sounds\enginestartstop\engine_stop_fuel",
				1
			}
		};
		volume=1;
	};
};
class CfgSoundSets
{
	class VB_ER_GravIdle_SoundSet
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
	class baseEngine_EXT_SoundSet
	{
		sound3DProcessingType="Vehicle_Ext_3DProcessingType";
		distanceFilter="softVehiclesDistanceFreqAttenuationFilter";
		volumeCurve="vehicleEngineAttenuationCurve";
		volumeFactor=1;
		spatial=1;
		loop=1;
	};
	class VB_ER_Engine_Ext_Rpm0_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Ext_Rpm0_SoundShader"
		};
		volumeFactor=1;
	};
	class VB_ER_Engine_Offload_Ext_Rpm1_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Offload_Ext_Rpm1_SoundShader"
		};
		volumeFactor=1;
	};
	class VB_ER_Engine_Ext_Rpm1_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Ext_Rpm1_SoundShader"
		};
		volumeFactor=1;
	};
	class VB_ER_Engine_Ext_Rpm2_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Ext_Rpm2_SoundShader"
		};
		volumeFactor=1;
	};
	class VB_ER_Engine_Offload_Ext_Rpm2_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Offload_Ext_Rpm2_SoundShader"
		};
		volumeFactor=1;
	};
	class VB_ER_Engine_Ext_Rpm3_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Ext_Rpm3_SoundShader"
		};
		volumeFactor=1;
	};
	class VB_ER_Engine_Offload_Ext_Rpm3_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Offload_Ext_Rpm3_SoundShader"
		};
		volumeFactor=1;
	};
	class VB_ER_Engine_Ext_Rpm4_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Ext_Rpm4_SoundShader"
		};
		volumeFactor=1;
	};
	class VB_ER_Engine_Offload_Ext_Rpm4_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Offload_Ext_Rpm4_SoundShader"
		};
		volumeFactor=1;
	};
	class VB_ER_Engine_Ext_Rpm5_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Ext_Rpm5_SoundShader"
		};
		volumeFactor=1;
	};
	class VB_ER_Engine_Offload_Ext_Rpm5_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Offload_Ext_Rpm5_SoundShader"
		};
		volumeFactor=1;
	};
	class VB_ER_Engine_Ext_Broken_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_Ext_Broken_SoundShader"
		};
		volumeFactor=1;
	};
	class baseVB_ER_Engine_StartStop_SoundSet
	{
		sound3DProcessingType="VehicleExt_3DProcessingType";
		volumeFactor=1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class VB_ER_Engine_start_SoundSet: baseVB_ER_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_engine_start_SoundShader"
		};
		frequencyFactor=0.94999999;
		startDelay=0.25;
		positionOffset[]={0,0,1};
	};
	class VB_ER_Engine_failed_start_fuel_SoundSet: baseVB_ER_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_engine_failed_start_fuel_SoundShader"
		};
		frequencyFactor=0.94999999;
		startDelay=0.25;
		positionOffset[]={0,0,1};
	};
	class VB_ER_Engine_failed_start_battery_SoundSet: baseVB_ER_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_engine_failed_start_battery_SoundShader"
		};
		frequencyFactor=0.94999999;
		startDelay=0.25;
		positionOffset[]={0,0,1};
	};
	class VB_ER_Engine_failed_start_sparkplugs_SoundSet: baseVB_ER_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_engine_failed_start_sparkplugs_SoundShader"
		};
		frequencyFactor=0.94999999;
		startDelay=0.25;
		positionOffset[]={0,0,1};
	};
	class VB_ER_Engine_stop_SoundSet: baseVB_ER_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_engine_stop_SoundShader"
		};
		frequencyFactor=0.94999999;
		positionOffset[]={0,0,1};
	};
	class VB_ER_Engine_stop_fuel_SoundSet: baseVB_ER_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"VB_ER_Engine_stop_fuel_SoundShader"
		};
		frequencyFactor=0.94999999;
		positionOffset[]={0,0,1};
	};
};
