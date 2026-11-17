class CfgPatches
{
	class dodge_rust_Z
	{
		units[]=
		{
			""
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Vehicles_Wheeled",
			"DZ_Vehicles_Parts",
			"DZ_Sounds_Effects",
			"DZ_Data",
			"DZ_Scripts"
		};
	};
};
class CfgMods
{
	class dodge_rust_Z
	{
		dir="dodge_rust_Z";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="dodge_rust_Z";
		credits="Radash";
		author="Radash";
		authorID="0";
		version=1;
		extra=0;
		type="mod";
		dependencies[]=
		{
			"World"
		};
		class defs
		{
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"dodge_rust_Z/scripts/4_World"
				};
			};
		};
	};
};
class CfgVehicles
{
	class Doors;
	class Health;
	class DamageZones;
	class Window;
	class GlobalHealth;
	class DamageSystem;
	class AnimationSources;
	class CarDoor;
	class Inventory_Base;
	class Bottle_Base;
	class Container_Base;
	class CarWheel;
	class CarScript;
	class Crew;
	class Driver;
	class CoDriver;
	class Barrel_ColorBase;
	class SeaChest: Container_Base
	{
		inventorySlot[]+=
		{
			"sea_chest",
			"sea_chest1",
			"sea_chest2",
			"sea_chest3"
		};
	};
	class dodge_rust_Z_wheel: CarWheel
	{
		scope=2;
		model="\dodge_rust_Z\proxy\dodge_rust_Z_wheel.p3d";
		displayName="Колесо dodge_rust_Z";
		descriptionShort="";
		itemSize[]={6,6};
		physLayer="item_large";
		rotationFlags=8;
		inventorySlot[]=
		{
			"dodge_rust_Z_wheel_1_1",
			"dodge_rust_Z_wheel_1_2",
			"dodge_rust_Z_wheel_2_1",
			"dodge_rust_Z_wheel_2_2"
		};
		radiusByDamage[]={0,0.40000001,0.25999999,0.25999999,0.99980003,0.25,0.99989998,0.273};
		radius=0.30000001;
		width=0.36000001;
		tyreOffroadResistance=2.5;
		tyreGrip=2.5;
		tyreRollResistance=0.015;
		tyreTread=2;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"DZ\vehicles\wheeled\OffroadHatchback\data\niva_wheel.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"DZ\vehicles\wheeled\OffroadHatchback\data\niva_wheel.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"DZ\vehicles\wheeled\OffroadHatchback\data\niva_wheel_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"DZ\vehicles\wheeled\OffroadHatchback\data\niva_wheel_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"DZ\vehicles\wheeled\OffroadHatchback\data\niva_wheel_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class dodge_rust_Z_doors_driver: CarDoor
	{
		scope=2;
		displayName="Дверь dodge_rust_Z";
		model="\dodge_rust_Z\proxy\dodge_rust_Z_doors_driver.p3d";
		itemSize[]={10,10};
		itemBehaviour=0;
		inventorySlot="dodge_rust_Z_doors_driver";
		rotationFlags=8;
		physLayer="item_large";
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\body.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dodge_rust_Z\data\Body.rvmat"
		};
		class DamageSystem: DamageSystem
		{
			class GlobalHealth: GlobalHealth
			{
			};
			class DamageZones: DamageZones
			{
				class Window: Window
				{
					class Health: Health
					{
						healthLevels[]=
						{
							
							{
								1,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_glass.rvmat"
								}
							},
							
							{
								0.69999999,
								{}
							},
							
							{
								0.5,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_glass_damage.rvmat"
								}
							},
							
							{
								0.30000001,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_glass_destruct.rvmat"
								}
							},
							
							{
								0,
								"hidden"
							}
						};
					};
				};
				class Doors: Doors
				{
					class Health: Health
					{
						healthLevels[]=
						{
							
							{
								1,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_door.rvmat"
								}
							},
							
							{
								0.69999999,
								{}
							},
							
							{
								0.5,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_door_damage.rvmat"
								}
							},
							
							{
								0.30000001,
								{}
							},
							
							{
								0,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_door_destruct.rvmat"
								}
							}
						};
					};
				};
			};
		};
	};
	class dodge_rust_Z_doors_codriver: dodge_rust_Z_doors_driver
	{
		displayName="Дверь dodge_rust_Z";
		descriptionShort="";
		model="\dodge_rust_Z\proxy\dodge_rust_Z_doors_codriver.p3d";
		inventorySlot="dodge_rust_Z_doors_codriver";
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\body.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dodge_rust_Z\data\Body.rvmat"
		};
	};
	class dodge_rust_Z_doors_hood: CarDoor
	{
		scope=2;
		displayName="Капот dodge_rust_Z";
		model="\dodge_rust_Z\proxy\dodge_rust_Z_doors_hood.p3d";
		itemSize[]={10,10};
		itemBehaviour=0;
		inventorySlot="dodge_rust_Z_doors_hood";
		rotationFlags=8;
		physLayer="item_large";
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\body.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dodge_rust_Z\data\Body.rvmat"
		};
		class DamageSystem: DamageSystem
		{
			class GlobalHealth: GlobalHealth
			{
			};
			class DamageZones: DamageZones
			{
				class Window: Window
				{
					class Health: Health
					{
						healthLevels[]=
						{
							
							{
								1,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_glass.rvmat"
								}
							},
							
							{
								0.69999999,
								{}
							},
							
							{
								0.5,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_glass_damage.rvmat"
								}
							},
							
							{
								0.30000001,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_glass_destruct.rvmat"
								}
							},
							
							{
								0,
								"hidden"
							}
						};
					};
				};
				class Doors: Doors
				{
					class Health: Health
					{
						healthLevels[]=
						{
							
							{
								1,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_door.rvmat"
								}
							},
							
							{
								0.69999999,
								{}
							},
							
							{
								0.5,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_door_damage.rvmat"
								}
							},
							
							{
								0.30000001,
								{}
							},
							
							{
								0,
								
								{
									"dz\vehicles\wheeled\offroadhatchback\data\niva_door_destruct.rvmat"
								}
							}
						};
					};
				};
			};
		};
	};
	class dodge_rust_Z_bamper: SeaChest
	{
		scope=2;
		displayName="dodge_rust_Z_bamper";
		descriptionShort="dodge_rust_Z_bamper";
		model="dodge_rust_Z\proxy\dodge_rust_Z_bamper.p3d";
		slopeTolerance=0.40000001;
		inventorySlot="dodge_rust_Z_bamper";
		yawPitchRollLimit[]={45,45,45};
		weight=10000;
		itemBehaviour=0;
		itemSize[]={10,10};
		carveNavmesh=1;
		canBeDigged=0;
		rotationFlags=2;
		class Cargo
		{
			itemsCargoSize[]={0,0};
			allowOwnedCargoManipulation=0;
			openable=0;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=25000;
					transferToAttachmentsCoef=0.059999999;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"DZ\gear\camping\data\sea_chest.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"DZ\gear\camping\data\sea_chest.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"DZ\gear\camping\data\sea_chest_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"DZ\gear\camping\data\sea_chest_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"DZ\gear\camping\data\sea_chest_destruct.rvmat"
							}
						}
					};
				};
			};
			class GlobalArmor
			{
				class FragGrenade
				{
					class Health
					{
						damage=8;
					};
					class Blood
					{
						damage=8;
					};
					class Shock
					{
						damage=8;
					};
				};
			};
		};
		soundImpactType="wood";
	};
	class dodge_rust_z_metal: dodge_rust_Z_bamper
	{
		scope=2;
		displayName="dodge_rust_z_metal";
		descriptionShort="dodge_rust_z_metal";
		model="dodge_rust_Z\proxy\dodge_rust_z_metal.p3d";
		slopeTolerance=0.40000001;
		inventorySlot="dodge_rust_z_metal";
	};
	class dodge_rust_Z_kovh: dodge_rust_Z_bamper
	{
		scope=2;
		displayName="dodge_rust_Z_kovh";
		descriptionShort="dodge_rust_Z_kovh";
		model="dodge_rust_Z\proxy\dodge_rust_Z_kovh.p3d";
		slopeTolerance=0.40000001;
		inventorySlot="dodge_rust_Z_kovh";
	};
	class dodge_rust_Z: CarScript
	{
		scope=2;
		displayname="dodge_rust_Z";
		descriptionShort="";
		Model="\dodge_rust_Z\dodge_rust_Z.p3d";
		attachments[]=
		{
			"Cassette",
			"CanisterGasoline",
			"CanisterGasoline2",
			"Truck_01_Barrel1",
			"dodge_rust_Z_kovh",
			"dodge_rust_Z_bamper",
			"dodge_rust_z_metal",
			"CarBattery",
			"Reflector_1_1",
			"Reflector_2_1",
			"dodge_rust_Z_doors_hood",
			"CarRadiator",
			"SparkPlug",
			"dodge_rust_Z_wheel_1_1",
			"dodge_rust_Z_wheel_1_2",
			"dodge_rust_Z_wheel_2_1",
			"dodge_rust_Z_wheel_2_2",
			"dodge_rust_Z_doors_driver",
			"sea_chest",
			"55galdrum_1",
			"dodge_rust_Z_doors_codriver"
		};
		hiddenSelections[]=
		{
			"light_1_1",
			"light_2_1",
			"light_brake_1_2",
			"light_brake_2_2",
			"light_reverse_1_2",
			"light_reverse_2_2",
			"light_1_2",
			"light_2_2",
			"camo"
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
			"dodge_rust_z\data\Body.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dodge_rust_Z\data\dodge_rust_Z_peredoff.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_peredoff.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_zadoff.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_zadoff.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_peredon.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_peredon.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_peredoff.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_peredoff.rvmat",
			"dodge_rust_z\data\Body.rvmat"
		};
		dashboardMatOn="";
		dashboardMatOff="";
		frontReflectorMatOn="dodge_rust_Z\data\dodge_rust_Z_peredon.rvmat";
		frontReflectorMatOff="dodge_rust_Z\data\dodge_rust_Z_peredoff.rvmat";
		brakeReflectorMatOn="dodge_rust_Z\data\dodge_rust_Z_zadon.rvmat";
		brakeReflectorMatOff="dodge_rust_Z\data\dodge_rust_Z_zadoff.rvmat";
		ReverseReflectorMatOn="dodge_rust_Z\data\dodge_rust_Z_zadon.rvmat";
		ReverseReflectorMatOff="dodge_rust_Z\data\dodge_rust_Z_zadoff.rvmat";
		TailReflectorMatOn="dodge_rust_Z\data\dodge_rust_Z_zadon.rvmat";
		TailReflectorMatOff="dodge_rust_Z\data\dodge_rust_Z_zadoff.rvmat";
		weight=1500000;
		fuelCapacity=100;
		fuelConsumption=10;
		class Sounds
		{
			thrust=0.60000002;
			thrustTurbo=1;
			thrustGentle=0.30000001;
			thrustSmoothCoef=0.1;
			camposSmoothCoef=0.029999999;
			soundSetsFilter[]=
			{
				"dodge_rust_Z_Engine_Offload_Ext_Rpm1_SoundSet",
				"dodge_rust_Z_Engine_Offload_Ext_Rpm2_SoundSet",
				"dodge_rust_Z_Engine_Offload_Ext_Rpm3_SoundSet",
				"dodge_rust_Z_Engine_Offload_Ext_Rpm4_SoundSet",
				"dodge_rust_Z_Engine_Offload_Ext_Rpm5_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm0_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm1_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm2_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm3_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm4_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm5_SoundSet",
				"dodge_rust_Z_Engine_Ext_Broken_SoundSet"
			};
			soundSetsInt[]=
			{
				"offroad_Tires_Asphalt_Slow_General_Int_SoundSet",
				"offroad_Tires_Asphalt_Fast_General_Int_SoundSet",
				"offroad_Wind_SoundSet"
			};
		};
		hornSoundSetEXT="";
		hornSoundSetINT="";
		class Crew
		{
			class Driver
			{
				actionSel="seat_driver";
				proxyPos="crewDriver";
				getInPos="pos_driver";
				getInDir="pos_driver_dir";
				isDriver=1;
			};
			class CoDriver
			{
				actionSel="seat_coDriver";
				proxyPos="crewCoDriver";
				getInPos="pos_coDriver";
				getInDir="pos_coDriver_dir";
			};
		};
		class SimulationModule
		{
			class Steering
			{
				maxSteeringAngle=35;
				increaseSpeed[]={0,40,60,30,100,15};
				decreaseSpeed[]={0,90,60,45,100,20};
				centeringSpeed[]={0,0,15,27,60,45,100,63};
			};
			class Throttle
			{
				reactionTime=1;
				defaultThrust=0.88999999;
				gentleThrust=0.69999999;
				turboCoef=2.45;
				gentleCoef=0.94999999;
			};
			class Brake
			{
				pressureBySpeed[]={0,0.5,10,0.40000001,20,0.30000001,40,0.28,60,0.30000001,80,0.38,100,0.41999999,120,0.47999999,150,0.60000002};
				reactionTime=0.25;
				driverless=0.1;
			};
			class Aerodynamics
			{
				frontalArea=2.1500001;
				dragCoefficient=0.15000001;
				downforceCoefficient=0.80000001;
				downforceOffset[]={0,0.40000001,-2.2};
			};
			drive="DRIVE_AWD";
			class Engine
			{
				torqueCurve[]={650,0,1360,290,3333,369,6036,424,8732,390,11020,0};
				inertia=0.61000001;
				frictionTorque=122;
				rollingFriction=2.3;
				viscousFriction=1.7;
				rpmIdle=900;
				rpmMin=1000;
				rpmClutch=1352;
				rpmRedline = 20000;
			};
			class Clutch
			{
				maxTorqueTransfer=505;
				uncoupleTime=0.02;
				coupleTime=0.17;
			};
			class Gearbox
			{
				type="GEARBOX_MANUAL";
				reverse=3.7260001;
				ratios[]={3.5,2.1440001,1.469,1.061,0.80400002};
			};
			class CentralDifferential
			{
				ratio=0.75;
				type="DIFFERENTIAL_LOCKED";
			};
			class Axles
			{
				class Front
				{
					maxBrakeTorque=2100;
					wheelHubMass=20;
					wheelHubRadius=0.15000001;
					class Differential
					{
						ratio=4.0999999;
						type="DIFFERENTIAL_OPEN";
					};
					class Suspension
					{
						stiffness=75000;
						compression=2100;
						damping=7500;
						travelMaxUp=0.16;
						travelMaxDown=0.13;
					};
					class Wheels
					{
						class Left
						{
							animTurn="turnfrontleft";
							animRotation="wheelfrontleft";
							wheelHub="wheel_1_1_damper_land";
							animDamper="damper_1_1";
							inventorySlot="dodge_rust_Z_wheel_1_1";
						};
						class Right
						{
							animTurn="turnfrontright";
							animRotation="wheelfrontright";
							wheelHub="wheel_2_1_damper_land";
							animDamper="damper_2_1";
							inventorySlot="dodge_rust_Z_wheel_2_1";
						};
					};
				};
				class Rear
				{
					maxBrakeTorque=1500;
					maxHandbrakeTorque=4500;
					wheelHubMass=20;
					wheelHubRadius=0.15000001;
					class Differential
					{
						ratio=4.0999999;
						type="DIFFERENTIAL_OPEN";
					};
					class Suspension
					{
						stiffness=75000;
						compression=2100;
						damping=7500;
						travelMaxUp=0.16;
						travelMaxDown=0.13;
					};
					class Wheels
					{
						class Left
						{
							animTurn="turnbackleft";
							animRotation="wheelbackleft";
							wheelHub="wheel_1_2_damper_land";
							animDamper="damper_1_2";
							inventorySlot="dodge_rust_Z_wheel_1_2";
						};
						class Right
						{
							animTurn="turnbacktright";
							animRotation="wheelbackright";
							wheelHub="wheel_2_2_damper_land";
							animDamper="damper_2_2";
							inventorySlot="dodge_rust_Z_wheel_2_2";
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
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 90000;
					healthLevels[] = {{1, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0, {}}};
				};
			};
			
			class DamageZones {
				class Chassis {
					class Health {
						hitpoints = 90000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0, {}}};
					};
					fatalInjuryCoef = -1;
					componentNames[] = {"dmgZone_chassis"};
					inventorySlots[] = {};
				};
				
				class Front {
					class Health {
						hitpoints = 70000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0, {}}};
					};
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgZone_front"};
					componentNames[] = {"dmgZone_front"};
					transferToZonesNames[] = {};
					transferToZonesThreshold[] = {0.5, 0.5, 0.5, 0.1};
					transferToZonesCoefs[] = {0.7, 0.7, 0.5, 0.05};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {0.3, 0.003};
				};
				
				class Reflector_1_1 {
					class Health {
						hitpoints = 11000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0, {}}};
					};
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgZone_lights_1_1"};
					componentNames[] = {"dmgZone_lights_1_1"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {0.1, 0.15, 0.05};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {1};
				};
				
				class Reflector_2_1 : Reflector_1_1 {
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {0.1, 0.15, 0.05};
					memoryPoints[] = {};
					componentNames[] = {};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {1};
				};
				
				class Back {
					class Health {
						hitpoints = 20000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0, {}}};
					};
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgZone_back"};
					componentNames[] = {"dmgZone_back"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {0.3, 0.3, 0.2, 0.2};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {0.8};
				};
				
				class Roof {
					class Health {
						hitpoints = 37000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0, {}}};
					};
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgZone_roof"};
					componentNames[] = {"dmgZone_roof"};
					inventorySlotsCoefs[] = {0};
					inventorySlots[] = {};
				};
				
				class Engine {
					class Health {
						hitpoints = 40000;
						transferToGlobalCoef = 1;
						healthLevels[] = {{1, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0, {}}};
					};
					fatalInjuryCoef = 0.001;
					memoryPoints[] = {"dmgZone_engine"};
					componentNames[] = {"dmgZone_engine"};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {0.002, 0.0002, 0.01, 0.0004};
				};
				
				class FuelTank {
					class Health {
						hitpoints = 52000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0, {}}};
					};
					fatalInjuryCoef = -1;
					componentNames[] = {"dmgZone_fuelTank"};
					inventorySlots[] = {};
				};
			};
		};
		
		class AnimationSources
		{
			class doorsDriver
			{
				source="user";
				initPhase=0;
				animPeriod=0.5;
			};
			class doorsCoDriver: doorsDriver
			{
			};
			class doorsHood: doorsDriver
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
			class HitdoorsDrivers: AnimHitWheel_1_1
			{
				hitpoint="HitdoorsDriver";
			};
			class HitdoorsCoDrivers: AnimHitWheel_1_1
			{
				hitpoint="HitdoorsCoDriver";
			};
			class HitdoorsCargo: AnimHitWheel_1_1
			{
				hitpoint="HitdoorsCargo";
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
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name="Мотор";
				description="";
				attachmentSlots[]=
				{
					"CarBattery",
					"CarRadiator",
					"SparkPlug"
				};
				icon="cat_vehicle_engine";
			};
			class Body
			{
				name="Кузов";
				description="";
				attachmentSlots[]=
				{
					"Reflector_1_1",
					"Reflector_2_1",
					"dodge_rust_Z_doors_driver",
					"dodge_rust_Z_doors_codriver",
					"dodge_rust_Z_doors_hood"
				};
				icon="cat_vehicle_body";
			};
			class Chassis
			{
				name="Шасси";
				description="";
				attachmentSlots[]=
				{
					"dodge_rust_Z_wheel_1_1",
					"dodge_rust_Z_wheel_2_1",
					"dodge_rust_Z_wheel_1_2",
					"dodge_rust_Z_wheel_2_2"
				};
				icon="cat_vehicle_chassis";
			};
			class BaseBuildingContainers
			{
				name="Контейнеры";
				description="";
				attachmentSlots[]=
				{
					"dodge_rust_z_metal",
					"dodge_rust_Z_kovh",
					"dodge_rust_Z_bamper",
					"55galdrum_1",
					"Truck_01_Barrel1",
					"CanisterGasoline",
					"sea_chest",
					"CanisterGasoline2",
					"Cassette"
				};
				icon="cat_common_cargo";
			};
		};
	};
	class dodge_rust_Z_green_doors_driver: dodge_rust_Z_doors_driver
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\Body_green.paa"
		};
	};
	class dodge_rust_Z_green_doors_codriver: dodge_rust_Z_doors_codriver
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\Body_green.paa"
		};
	};
	class dodge_rust_Z_green_doors_hood: dodge_rust_Z_doors_hood
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\Body_green.paa"
		};
	};
	class dodge_rust_Z_green: dodge_rust_Z
	{
		scope=2;
		hiddenSelections[]=
		{
			"light_1_1",
			"light_2_1",
			"light_brake_1_2",
			"light_brake_2_2",
			"light_reverse_1_2",
			"light_reverse_2_2",
			"light_1_2",
			"light_2_2",
			"camo"
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
			"dodge_rust_Z\data\Body_green.paa"
		};
	};
	class dodge_rust_Z_mpg_doors_driver: dodge_rust_Z_doors_driver
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\body_mpg.paa"
		};
	};
	class dodge_rust_Z_mpg_doors_codriver: dodge_rust_Z_doors_codriver
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\body_mpg.paa"
		};
	};
	class dodge_rust_Z_mpg_doors_hood: dodge_rust_Z_doors_hood
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\body_mpg.paa"
		};
	};
	class dodge_rust_Z_mpg: dodge_rust_Z
	{
		scope=2;
		hiddenSelections[]=
		{
			"light_1_1",
			"light_2_1",
			"light_brake_1_2",
			"light_brake_2_2",
			"light_reverse_1_2",
			"light_reverse_2_2",
			"light_1_2",
			"light_2_2",
			"camo"
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
			"dodge_rust_Z\data\body_mpg.paa"
		};
	};
	class dodge_rust_Z_red_doors_driver: dodge_rust_Z_doors_driver
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\body_red.paa"
		};
	};
	class dodge_rust_Z_red_doors_codriver: dodge_rust_Z_doors_codriver
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\body_red.paa"
		};
	};
	class dodge_rust_Z_red_doors_hood: dodge_rust_Z_doors_hood
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"dodge_rust_Z\data\body_red.paa"
		};
	};
	class dodge_rust_Z_red: dodge_rust_Z
	{
		scope=2;
		hiddenSelections[]=
		{
			"light_1_1",
			"light_2_1",
			"light_brake_1_2",
			"light_brake_2_2",
			"light_reverse_1_2",
			"light_reverse_2_2",
			"light_1_2",
			"light_2_2",
			"camo"
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
			"dodge_rust_Z\data\body_red.paa"
		};
	};
	
	
	
	
	
	
	class dodge_rust_Z_Trader: CarScript
	{
		scope=2;
		displayname="Dodge Rust Muscle DayZavr";
		descriptionShort="A real monster, not a just car. Made specifically for the apocalypse in the world of DayZavr. Powerful sound, excellent handling. 1000 slots, sea chest, barrels.";
		Model="\dodge_rust_Z\dodge_rust_Z.p3d";
		attachments[]=
		{
			"Cassette",
			"CanisterGasoline",
			"CanisterGasoline2",
			"Truck_01_Barrel1",
			"dodge_rust_Z_kovh",
			"dodge_rust_Z_bamper",
			"dodge_rust_z_metal",
			"CarBattery",
			"Reflector_1_1",
			"Reflector_2_1",
			"dodge_rust_Z_doors_hood",
			"CarRadiator",
			"SparkPlug",
			"dodge_rust_Z_wheel_1_1",
			"dodge_rust_Z_wheel_1_2",
			"dodge_rust_Z_wheel_2_1",
			"dodge_rust_Z_wheel_2_2",
			"dodge_rust_Z_doors_driver",
			"sea_chest",
			"55galdrum_1",
			"dodge_rust_Z_doors_codriver"
		};
		hiddenSelections[]=
		{
			"light_1_1",
			"light_2_1",
			"light_brake_1_2",
			"light_brake_2_2",
			"light_reverse_1_2",
			"light_reverse_2_2",
			"light_1_2",
			"light_2_2",
			"camo"
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
			"dodge_rust_z\data\Body.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dodge_rust_Z\data\dodge_rust_Z_peredoff.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_peredoff.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_zadoff.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_zadoff.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_peredon.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_peredon.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_peredoff.rvmat",
			"dodge_rust_Z\data\dodge_rust_Z_peredoff.rvmat",
			"dodge_rust_z\data\Body.rvmat"
		};
		dashboardMatOn="";
		dashboardMatOff="";
		frontReflectorMatOn="dodge_rust_Z\data\dodge_rust_Z_peredon.rvmat";
		frontReflectorMatOff="dodge_rust_Z\data\dodge_rust_Z_peredoff.rvmat";
		brakeReflectorMatOn="dodge_rust_Z\data\dodge_rust_Z_zadon.rvmat";
		brakeReflectorMatOff="dodge_rust_Z\data\dodge_rust_Z_zadoff.rvmat";
		ReverseReflectorMatOn="dodge_rust_Z\data\dodge_rust_Z_zadon.rvmat";
		ReverseReflectorMatOff="dodge_rust_Z\data\dodge_rust_Z_zadoff.rvmat";
		TailReflectorMatOn="dodge_rust_Z\data\dodge_rust_Z_zadon.rvmat";
		TailReflectorMatOff="dodge_rust_Z\data\dodge_rust_Z_zadoff.rvmat";
		weight=1500000;
		fuelCapacity=100;
		fuelConsumption=10;
		class Sounds
		{
			thrust=0.60000002;
			thrustTurbo=1;
			thrustGentle=0.30000001;
			thrustSmoothCoef=0.1;
			camposSmoothCoef=0.029999999;
			soundSetsFilter[]=
			{
				"dodge_rust_Z_Engine_Offload_Ext_Rpm1_SoundSet",
				"dodge_rust_Z_Engine_Offload_Ext_Rpm2_SoundSet",
				"dodge_rust_Z_Engine_Offload_Ext_Rpm3_SoundSet",
				"dodge_rust_Z_Engine_Offload_Ext_Rpm4_SoundSet",
				"dodge_rust_Z_Engine_Offload_Ext_Rpm5_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm0_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm1_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm2_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm3_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm4_SoundSet",
				"dodge_rust_Z_Engine_Ext_Rpm5_SoundSet",
				"dodge_rust_Z_Engine_Ext_Broken_SoundSet"
			};
			soundSetsInt[]=
			{
				"offroad_Tires_Asphalt_Slow_General_Int_SoundSet",
				"offroad_Tires_Asphalt_Fast_General_Int_SoundSet",
				"offroad_Wind_SoundSet"
			};
		};
		hornSoundSetEXT="";
		hornSoundSetINT="";
		class Crew
		{
			class Driver
			{
				actionSel="seat_driver";
				proxyPos="crewDriver";
				getInPos="pos_driver";
				getInDir="pos_driver_dir";
				isDriver=1;
			};
			class CoDriver
			{
				actionSel="seat_coDriver";
				proxyPos="crewCoDriver";
				getInPos="pos_coDriver";
				getInDir="pos_coDriver_dir";
			};
		};
		class SimulationModule
		{
			class Steering
			{
				maxSteeringAngle=35;
				increaseSpeed[]={0,40,60,30,100,15};
				decreaseSpeed[]={0,90,60,45,100,20};
				centeringSpeed[]={0,0,15,27,60,45,100,63};
			};
			class Throttle
			{
				reactionTime=1;
				defaultThrust=0.88999999;
				gentleThrust=0.69999999;
				turboCoef=2.45;
				gentleCoef=0.94999999;
			};
			class Brake
			{
				pressureBySpeed[]={0,0.5,10,0.40000001,20,0.30000001,40,0.28,60,0.30000001,80,0.38,100,0.41999999,120,0.47999999,150,0.60000002};
				reactionTime=0.25;
				driverless=0.1;
			};
			class Aerodynamics
			{
				frontalArea=2.1500001;
				dragCoefficient=0.15000001;
				downforceCoefficient=0.80000001;
				downforceOffset[]={0,0.40000001,-2.2};
			};
			drive="DRIVE_AWD";
			class Engine
			{
				torqueCurve[]={650,0,1360,290,3333,369,6036,424,8732,390,11020,0};
				inertia=0.61000001;
				frictionTorque=122;
				rollingFriction=2.3;
				viscousFriction=1.7;
				rpmIdle=900;
				rpmMin=1000;
				rpmClutch=1352;
				rpmRedline = 20000;
			};
			class Clutch
			{
				maxTorqueTransfer=505;
				uncoupleTime=0.02;
				coupleTime=0.17;
			};
			class Gearbox
			{
				type="GEARBOX_MANUAL";
				reverse=3.7260001;
				ratios[]={3.5,2.1440001,1.469,1.061,0.80400002};
			};
			class CentralDifferential
			{
				ratio=0.75;
				type="DIFFERENTIAL_LOCKED";
			};
			class Axles
			{
				class Front
				{
					maxBrakeTorque=2100;
					wheelHubMass=20;
					wheelHubRadius=0.15000001;
					class Differential
					{
						ratio=4.0999999;
						type="DIFFERENTIAL_OPEN";
					};
					class Suspension
					{
						stiffness=75000;
						compression=2100;
						damping=7500;
						travelMaxUp=0.16;
						travelMaxDown=0.13;
					};
					class Wheels
					{
						class Left
						{
							animTurn="turnfrontleft";
							animRotation="wheelfrontleft";
							wheelHub="wheel_1_1_damper_land";
							animDamper="damper_1_1";
							inventorySlot="dodge_rust_Z_wheel_1_1";
						};
						class Right
						{
							animTurn="turnfrontright";
							animRotation="wheelfrontright";
							wheelHub="wheel_2_1_damper_land";
							animDamper="damper_2_1";
							inventorySlot="dodge_rust_Z_wheel_2_1";
						};
					};
				};
				class Rear
				{
					maxBrakeTorque=1500;
					maxHandbrakeTorque=4500;
					wheelHubMass=20;
					wheelHubRadius=0.15000001;
					class Differential
					{
						ratio=4.0999999;
						type="DIFFERENTIAL_OPEN";
					};
					class Suspension
					{
						stiffness=75000;
						compression=2100;
						damping=7500;
						travelMaxUp=0.16;
						travelMaxDown=0.13;
					};
					class Wheels
					{
						class Left
						{
							animTurn="turnbackleft";
							animRotation="wheelbackleft";
							wheelHub="wheel_1_2_damper_land";
							animDamper="damper_1_2";
							inventorySlot="dodge_rust_Z_wheel_1_2";
						};
						class Right
						{
							animTurn="turnbacktright";
							animRotation="wheelbackright";
							wheelHub="wheel_2_2_damper_land";
							animDamper="damper_2_2";
							inventorySlot="dodge_rust_Z_wheel_2_2";
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
		class AnimationSources
		{
			class doorsDriver
			{
				source="user";
				initPhase=0;
				animPeriod=0.5;
			};
			class doorsCoDriver: doorsDriver
			{
			};
			class doorsHood: doorsDriver
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
			class HitdoorsDrivers: AnimHitWheel_1_1
			{
				hitpoint="HitdoorsDriver";
			};
			class HitdoorsCoDrivers: AnimHitWheel_1_1
			{
				hitpoint="HitdoorsCoDriver";
			};
			class HitdoorsCargo: AnimHitWheel_1_1
			{
				hitpoint="HitdoorsCargo";
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
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name="Мотор";
				description="";
				attachmentSlots[]=
				{
					"CarBattery",
					"CarRadiator",
					"SparkPlug"
				};
				icon="cat_vehicle_engine";
			};
			class Body
			{
				name="Кузов";
				description="";
				attachmentSlots[]=
				{
					"Reflector_1_1",
					"Reflector_2_1",
					"dodge_rust_Z_doors_driver",
					"dodge_rust_Z_doors_codriver",
					"dodge_rust_Z_doors_hood"
				};
				icon="cat_vehicle_body";
			};
			class Chassis
			{
				name="Шасси";
				description="";
				attachmentSlots[]=
				{
					"dodge_rust_Z_wheel_1_1",
					"dodge_rust_Z_wheel_2_1",
					"dodge_rust_Z_wheel_1_2",
					"dodge_rust_Z_wheel_2_2"
				};
				icon="cat_vehicle_chassis";
			};
			class BaseBuildingContainers
			{
				name="Контейнеры";
				description="";
				attachmentSlots[]=
				{
					"dodge_rust_z_metal",
					"dodge_rust_Z_kovh",
					"dodge_rust_Z_bamper",
					"55galdrum_1",
					"Truck_01_Barrel1",
					"CanisterGasoline",
					"sea_chest",
					"CanisterGasoline2",
					"Cassette"
				};
				icon="cat_common_cargo";
			};
		};
	};	
	
	
	
	
	
	
	
	
};
class CfgDestroy
{
	class EngineHit
	{
		sound[]={};
	};
};
class CfgSlots
{
	class Slot_CanisterGasoline
	{
		name="CanisterGasoline";
		displayName="CanisterGasoline";
		selection="CanisterGasoline";
		ghostIcon="gascanister";
	};
	class Slot_CanisterGasoline2
	{
		name="CanisterGasoline2";
		displayName="CanisterGasoline";
		selection="CanisterGasoline2";
		ghostIcon="gascanister";
	};
	class Slot_dodge_rust_Z_doors_hood
	{
		name="dodge_rust_Z_doors_hood";
		displayName="Hood";
		selection="doors_hood";
		ghostIcon="hood";
	};
	class Slot_dodge_rust_z_metal
	{
		name="dodge_rust_z_metal";
		displayName="Metal";
		selection="dodge_rust_z_metal";
		ghostIcon="hood";
	};
	class Slot_dodge_rust_Z_kovh
	{
		name="dodge_rust_Z_kovh";
		displayName="Scoop";
		selection="dodge_rust_Z_kovh";
		ghostIcon="hood";
	};
	class Slot_dodge_rust_Z_bamper
	{
		name="dodge_rust_Z_bamper";
		displayName="Bumper";
		selection="dodge_rust_Z_bamper";
		ghostIcon="hood";
	};
	class Slot_dodge_rust_Z_wheel_1_1
	{
		name="dodge_rust_Z_wheel_1_1";
		displayName="Front Left wheel";
		selection="wheel_1_1";
		ghostIcon="wheel";
	};
	class Slot_dodge_rust_Z_wheel_1_2
	{
		name="dodge_rust_Z_wheel_1_2";
		displayName="Rear Left wheel";
		selection="wheel_1_2";
		ghostIcon="wheel";
	};
	class Slot_dodge_rust_Z_wheel_2_1
	{
		name="dodge_rust_Z_wheel_2_1";
		displayName="Front Right wheel";
		selection="wheel_2_1";
		ghostIcon="wheel";
	};
	class Slot_dodge_rust_Z_wheel_2_2
	{
		name="dodge_rust_Z_wheel_2_2";
		displayName="Rear Right wheel";
		selection="wheel_2_2";
		ghostIcon="wheel";
	};
	class Slot_dodge_rust_Z_doors_driver
	{
		name="dodge_rust_Z_doors_driver";
		displayName="Drivers Door";
		selection="doors_driver";
		ghostIcon="doorfront";
	};
	class Slot_dodge_rust_Z_doors_codriver
	{
		name="dodge_rust_Z_doors_codriver";
		displayName="Passenger Door";
		selection="doors_codriver";
		ghostIcon="doorfront";
	};
	class Slot_55galdrum_1
	{
		name="55galdrum_1";
		displayname="barrel";
		selection="55galdrum_1";
		ghostIcon="barrel";
	};
	class Slot_Truck_01_Barrel1
	{
		name="Truck_01_Barrel1";
		displayname="barrel";
		selection="Truck_01_Barrel1";
		ghostIcon="barrel";
	};
	class Slot_sea_chest
	{
		name="sea_chest";
		displayname="sea_chest";
		selection="sea_chest";
		ghostIcon="cat_common_cargo";
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyVehiclePart: ProxyAttachment
	{
		scope=2;
		simulation="ProxyInventory";
		autocenter=0;
		animated=0;
		shadow=1;
		reversed=0;
	};
	class ProxyJerryCan: ProxyAttachment
	{
		model="\dz\vehicles\parts\JerryCan.p3d";
		inventorySlot[]+=
		{
			"CanisterGasoline",
			"CanisterGasoline2"
		};
	};
	class Proxysea_chest: ProxyVehiclePart
	{
		model="\DZ\gear\camping\sea_chest.p3d";
		inventorySlot[]+=
		{
			"sea_chest",
			"sea_chest1",
			"sea_chest2",
			"sea_chest3"
		};
	};
	class Proxydodge_rust_Z_doors_hood: ProxyVehiclePart
	{
		model="\dodge_rust_Z\proxy\dodge_rust_Z_doors_hood.p3d";
		inventorySlot="dodge_rust_Z_doors_hood";
	};
	class Proxydodge_rust_Z_bamper: ProxyVehiclePart
	{
		model="\dodge_rust_Z\proxy\dodge_rust_Z_bamper.p3d";
		inventorySlot="dodge_rust_Z_bamper";
	};
	class Proxydodge_rust_z_metal: ProxyVehiclePart
	{
		model="\dodge_rust_Z\proxy\dodge_rust_z_metal.p3d";
		inventorySlot="dodge_rust_z_metal";
	};
	class Proxydodge_rust_Z_kovh: ProxyVehiclePart
	{
		model="\dodge_rust_Z\proxy\dodge_rust_Z_kovh.p3d";
		inventorySlot="dodge_rust_Z_kovh";
	};
	class Proxydodge_rust_Z_wheel: ProxyVehiclePart
	{
		model="dodge_rust_Z\proxy\dodge_rust_Z_wheel.p3d";
		inventorySlot[]=
		{
			"dodge_rust_Z_wheel_1_1",
			"dodge_rust_Z_wheel_1_2",
			"dodge_rust_Z_wheel_2_1",
			"dodge_rust_Z_wheel_2_2"
		};
	};
	class Proxydodge_rust_Z_doors_driver: ProxyVehiclePart
	{
		model="\dodge_rust_Z\proxy\dodge_rust_Z_doors_driver.p3d";
		inventorySlot="dodge_rust_Z_doors_driver";
	};
	class Proxydodge_rust_Z_doors_codriver: ProxyVehiclePart
	{
		model="\dodge_rust_Z\proxy\dodge_rust_Z_doors_codriver.p3d";
		inventorySlot="dodge_rust_Z_doors_codriver";
	};
	class Proxy55galdrum: ProxyVehiclePart
	{
		model="\DZ\gear\containers\55galdrum.p3d";
		inventorySlot[]=
		{
			"55galdrum_1",
			"55galdrum_2",
			"Truck_01_Barrel1",
			"Truck_01_Barrel2",
			"BarrelColorBaseSlot"
		};
	};
	class ProxyPart
	{
		scope=2;
		simulation="ProxyInventory";
		model="";
		inventorySlot="";
		autocenter=0;
		animated=0;
		shadow=1;
		reversed=1;
	};
};
