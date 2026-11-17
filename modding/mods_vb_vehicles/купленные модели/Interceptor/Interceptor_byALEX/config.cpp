class CfgPatches
{
	class Interceptor_byALEX
	{
		units[]=
		{
			"Interceptor"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Vehicles_Wheeled",
			"DZ_Vehicles_Parts",
			"DZ_Sounds_Effects",
			"DZ_Data"
		};
	};
};
class CfgMods
{
	class Interceptor_byALEX
	{
		dir="Interceptor_byALEX";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="Interceptor_byALEX";
		credits="ALEX";
		author="ALEX";
		authorID="76561198179608217";
		version="1";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"World",
			"Mission"
		};
		class defs
		{
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"Interceptor_byALEX/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"Interceptor_byALEX/scripts/5_Mission"
				};
			};
		};
	};
};
class CfgSlots
{
	class Slot_Interceptor_Wheel_1_1
	{
		name="Interceptor_Wheel_1_1";
		displayname="$STR_Interceptor_Wheel_name";
		selection="wheel_1_1";
		ghostIcon="wheel";
	};
	class Slot_Interceptor_Wheel_1_2
	{
		name="Interceptor_Wheel_1_2";
		displayname="$STR_Interceptor_Wheel_name";
		selection="wheel_1_2";
		ghostIcon="wheel";
	};
	class Slot_Interceptor_Wheel_2_1
	{
		name="Interceptor_Wheel_2_1";
		displayname="$STR_Interceptor_Wheel_name";
		selection="wheel_2_1";
		ghostIcon="wheel";
	};
	class Slot_Interceptor_Wheel_2_2
	{
		name="Interceptor_Wheel_2_2";
		displayname="$STR_Interceptor_Wheel_name";
		selection="wheel_2_2";
		ghostIcon="wheel";
	};
	class Slot_Interceptor_Wheel_spare_1
	{
		name="Interceptor_Wheel_spare_1";
		displayname="$STR_Interceptor_Wheel_name";
		selection="wheel_spare_1";
		ghostIcon="wheel";
	};
	class Slot_Interceptor_Doors_Driver
	{
		name="Interceptor_Doors_Driver";
		displayname="$STR_Interceptor_driver_name";
		selection="doors_driver";
		ghostIcon="doorfront";
	};
	class Slot_Interceptor_Doors_coDriver
	{
		name="Interceptor_Doors_coDriver";
		displayname="$STR_Interceptor_codriver_name";
		selection="doors_codriver";
		ghostIcon="doorfront";
	};
	class Slot_Interceptor_Doors_hood
	{
		name="Interceptor_Doors_hood";
		displayname="$STR_Interceptor_hood_name";
		selection="doors_Hood";
		ghostIcon="hood";
	};
	class Slot_grenade_a
	{
		name="grenade_a";
		displayname="$STR_CfgVehicles_RGD5Grenade0";
		selection="VestGrenadeA";
		ghostIcon="grenade";
	};
	class Slot_grenade_b
	{
		name="grenade_b";
		displayname="$STR_CfgVehicles_RGD5Grenade0";
		selection="VestGrenadeB";
		ghostIcon="grenade";
	};
	class Slot_grenade_c
	{
		name="grenade_c";
		displayname="$STR_CfgVehicles_RGD5Grenade0";
		selection="VestGrenadeC";
		ghostIcon="grenade";
	};
	class Slot_grenade_d
	{
		name="grenade_d";
		displayname="$STR_CfgVehicles_RGD5Grenade0";
		selection="VestGrenadeD";
		ghostIcon="grenade";
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
		class Interceptor_Wheel: CarWheel
	{
		scope=2;
		model="Interceptor_byALEX\proxy\Interceptor_Wheel.p3d";
		displayName="$STR_Interceptor_Wheel_name";
		descriptionShort="$STR_Interceptor_wheel_disc";
		itemSize[]={6,6};
		weight=20000;
		physLayer="item_large";
		rotationFlags=8;
		inventorySlot[]=
		{
			"Interceptor_Wheel_1_1",
			"Interceptor_Wheel_1_2",
			"Interceptor_Wheel_2_1",
			"Interceptor_Wheel_2_2",
			"Interceptor_Wheel_spare_1"
		};
		repairableWithKits[]={6};
		repairCosts[]={30};
		radiusByDamage[]={0,0.33000001,0.30000001,0.30000001,0.99980003,0.25,0.99989998,0.2};
		radius=0.33000001;
		width=0.28;
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
					healthLevels[]=
					{
						
						{
							1.001,
							
							{
								"Interceptor_byalex\data\wheel.rvmat"
							}
						},
						
						{
							0.70099998,
							
							{
								"Interceptor_byalex\data\wheel.rvmat"
							}
						},
						
						{
							0.50099999,
							
							{
								"Interceptor_byalex\data\base\body_damage.rvmat"
							}
						},
						
						{
							0.301,
							
							{
								"Interceptor_byalex\data\base\body_damage.rvmat"
							}
						},
						
						{
							0.001,
							
							{
								"Interceptor_byalex\data\base\body_destruct.rvmat"
							}
						}
					};
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
	class Interceptor_Wheel_Ruined: Interceptor_Wheel { scope=2; };
	class Interceptor_Wheel_destroyed: CarWheel
	{
		model="Interceptor_byALEX\proxy\Interceptor_Wheel_destroyed.p3d";
		itemSize[]={6,6};
		weight=20000;
		physLayer="item_large";
		inventorySlot[]=
		{
			"Interceptor_Wheel_1_1",
			"Interceptor_Wheel_1_2",
			"Interceptor_Wheel_2_1",
			"Interceptor_Wheel_2_2",
			"Interceptor_Wheel_spare_1"
		};
		radius=0.27000001;
		width=0.2;
		tyreRollResistance=1;
		tyreRollDrag=75;
		tyreRoughness=1.2;
		tyreTread=0.5;
	};
	class Interceptor: CarScript
	{
		scope=2;
		displayName="$STR_Interceptor_name";
		Model="Interceptor_byALEX\Interceptor.p3d";
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
			"camo1"
		};
		attachments[]=
		{
			"CarBattery",
			"Reflector_1_1",
			"Reflector_2_1",
			"CarRadiator",
			"SparkPlug",
			"Interceptor_Wheel_1_1",
			"Interceptor_Wheel_1_2",
			"Interceptor_Wheel_2_1",
			"Interceptor_Wheel_2_2",
			"Interceptor_Wheel_spare_1",
			"Interceptor_Doors_Driver",
			"Interceptor_Doors_coDriver",
			"Interceptor_Doors_hood",
			"CanisterGasoline",
			"Shoulder",
			"Back",
			"55galDrum_1",
			"55galDrum_2",
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
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\body_co.paa",
			"interceptor_byalex\data\body_co.paa",
			"interceptor_byalex\data\body_co.paa",
			"interceptor_byalex\data\body_co.paa",
			"interceptor_byalex\data\body_co.paa",
			"interceptor_byalex\data\body_co.paa",
			"interceptor_byalex\data\body_co.paa",
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\salon_co.paa",
			"interceptor_byalex\data\body_co.paa"
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
			"interceptor_byalex\data\chassis.rvmat",
			"interceptor_byalex\data\body.rvmat",
			"interceptor_byalex\data\body.rvmat",
			"interceptor_byalex\data\body.rvmat",
			"interceptor_byalex\data\body.rvmat",
			"interceptor_byalex\data\body.rvmat",
			"interceptor_byalex\data\body.rvmat",
			"interceptor_byalex\data\body.rvmat",
			"interceptor_byalex\data\chassis.rvmat",
			"interceptor_byalex\data\salon.rvmat",
			"interceptor_byalex\data\body.rvmat"
		};
		dashboardMatOn="Interceptor_byalex\data\base\dashlight.rvmat";
		dashboardMatOff="interceptor_byalex\data\salon.rvmat";
		frontReflectorMatOn="Interceptor_byalex\data\base\swiatlo.rvmat";
		frontReflectorMatOff="interceptor_byalex\data\body.rvmat";
		brakeReflectorMatOn="dz\vehicles\wheeled\offroadhatchback\data\OffroadHatchback_lights_e.rvmat";
		brakeReflectorMatOff="interceptor_byalex\data\body.rvmat";
		ReverseReflectorMatOn="Interceptor_byalex\data\base\swiatlo.rvmat";
		ReverseReflectorMatOff="interceptor_byalex\data\body.rvmat";
		TailReflectorMatOn="dz\vehicles\wheeled\offroadhatchback\data\OffroadHatchback_lights_e.rvmat";
		TailReflectorMatOff="interceptor_byalex\data\body.rvmat";
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
				"Interceptor_Engine_Offload_Ext_Rpm1_SoundSet",
				"Interceptor_Engine_Offload_Ext_Rpm2_SoundSet",
				"Interceptor_Engine_Offload_Ext_Rpm3_SoundSet",
				"Interceptor_Engine_Offload_Ext_Rpm4_SoundSet",
				"Interceptor_Engine_Offload_Ext_Rpm5_SoundSet",
				"Interceptor_Engine_Ext_Rpm0_SoundSet",
				"Interceptor_Engine_Ext_Rpm1_SoundSet",
				"Interceptor_Engine_Ext_Rpm2_SoundSet",
				"Interceptor_Engine_Ext_Rpm3_SoundSet",
				"Interceptor_Engine_Ext_Rpm4_SoundSet",
				"Interceptor_Engine_Ext_Rpm5_SoundSet",
				"Interceptor_Engine_Ext_Broken_SoundSet",
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
					wheelHubRadius=0.25;
					class Differential
					{
						ratio=4.0999999;
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
							inventorySlot="Interceptor_Wheel_1_1";
						};
						class Right: Right
						{
							animDamper="damper_2_1";
							inventorySlot="Interceptor_Wheel_2_1";
						};
					};
				};
				class Rear: Rear
				{
					maxBrakeTorque=3000;
					maxHandbrakeTorque=2500;
					wheelHubMass=10;
					wheelHubRadius=0.25;
					class Differential
					{
						ratio=4.0999999;
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
							inventorySlot="Interceptor_Wheel_1_2";
						};
						class Right: Right
						{
							animDamper="damper_2_2";
							inventorySlot="Interceptor_Wheel_2_2";
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
									"interceptor_byalex\data\chassis.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"interceptor_byalex\data\chassis.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"Interceptor_byalex\data\base\body_destruct.rvmat"
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
									"interceptor_byalex\data\body.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"interceptor_byalex\data\body.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"Interceptor_byalex\data\base\body_destruct.rvmat"
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
						"Interceptor_Doors_hood",
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
									"interceptor_byalex\data\body.rvmat"
								},
								
								{
									"interceptor_byalex\data\body.rvmat"
								}
							},
							
							{
								0.70099998,
								{}
							},
							
							{
								0.50099999,
								
								{
									"interceptor_byalex\data\body.rvmat"
								},
								
								{
									"interceptor_byalex\data\body.rvmat"
								}
							},
							
							{
								0.301,
								{}
							},
							
							{
								0.001,
								
								{
									"interceptor_byalex\data\body.rvmat"
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
						"Interceptor_Wheel_1_1"
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
						"Interceptor_Wheel_2_1"
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
									"interceptor_byalex\data\body.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"interceptor_byalex\data\body.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"Interceptor_byalex\data\base\body_destruct.rvmat"
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
									"interceptor_byalex\data\body.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"interceptor_byalex\data\body.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"Interceptor_byalex\data\base\body_destruct.rvmat"
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
									"interceptor_byalex\data\body.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"interceptor_byalex\data\body.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"Interceptor_byalex\data\base\body_destruct.rvmat"
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
						"Interceptor_Doors_hood",
						"Interceptor_Wheel_1_1",
						"Interceptor_Doors_Driver"
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
						"Interceptor_Doors_trunk",
						"",
						"Interceptor_Wheel_1_2"
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
						"Interceptor_Doors_hood",
						"Interceptor_Wheel_2_1",
						"Interceptor_Doors_coDriver"
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
						"Interceptor_Doors_trunk",
						"",
						"Interceptor_Wheel_2_2"
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
									"interceptor_byalex\data\chassis.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"interceptor_byalex\data\chassis.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"Interceptor_byalex\data\base\body_destruct.rvmat"
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
									"interceptor_byalex\data\salon.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"interceptor_byalex\data\salon.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"Interceptor_byalex\data\base\body_destruct.rvmat"
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
									"interceptor_byalex\data\engine.rvmat"
								}
							},
							
							{
								0.70099998,
								
								{
									"Interceptor_byalex\data\engine.rvmat"
								}
							},
							
							{
								0.50099999,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.001,
								
								{
									"Interceptor_byalex\data\base\body_destruct.rvmat"
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
				attachmentSlots[]=
				{
					"SparkPlug",
					"CarBattery",
					"CarRadiator"
				};
				icon="cat_vehicle_engine";
			};
			class Body
			{
				name="$STR_attachment_Body0";
				description="";
				attachmentSlots[]=
				{
					"Reflector_1_1",
					"Reflector_2_1",
					"Interceptor_Doors_Driver",
					"Interceptor_Doors_coDriver",
					"Interceptor_Doors_hood",
					"Cassette"
				};
				icon="cat_vehicle_body";
			};
			class Chassis
			{
				name="$STR_attachment_Chassis0";
				description="";
				attachmentSlots[]=
				{
					"Interceptor_Wheel_1_1",
					"Interceptor_Wheel_1_2",
					"Interceptor_Wheel_2_1",
					"Interceptor_Wheel_2_2",
					"Interceptor_Wheel_spare_1"
				};
				icon="cat_vehicle_chassis";
			};
			class BaseBuildingContainers
			{
				name="$STR_attachment_containers0";
				description="";
				attachmentSlots[]=
				{
					"55galDrum_1",
					"55galDrum_2",
					"CanisterGasoline",
					"Shoulder",
					"Back",
					"VestGrenadeA",
					"VestGrenadeB",
					"VestGrenadeC",
					"VestGrenadeD"
				};
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
	class Interceptor_Doors_Driver: CarDoor
	{
		scope=2;
		displayName="$STR_Interceptor_driver_name";
		descriptionShort="$STR_Interceptor_doors_driver_disc";
		model="Interceptor_byALEX\proxy\Interceptor_Doors_Driver.p3d";
		weight=15000;
		itemSize[]={6,6};
		itemBehaviour=0;
		inventorySlot="Interceptor_Doors_Driver";
		rotationFlags=8;
		physLayer="item_large";
		hiddenSelections[]=
		{
			"camo1",
			"camo2",
			"camo3"
		};
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_co.paa",
			"",
			""
		};
		hiddenSelectionsMaterials[]=
		{
			"Interceptor_byalex\data\Body.rvmat",
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
									"Interceptor_byalex\data\salon.rvmat"
								}
							},
							
							{
								0.70099998,
								{}
							},
							
							{
								0.50099999,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								{}
							},
							
							{
								0.001,
								
								{
									"Interceptor_byalex\data\base\body_destruct.rvmat"
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
									"Interceptor_byalex\data\Body.rvmat"
								}
							},
							
							{
								0.70099998,
								{}
							},
							
							{
								0.50099999,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								{}
							},
							
							{
								0.001,
								
								{
									"Interceptor_byalex\data\base\body_destruct.rvmat"
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
	class Interceptor_Doors_coDriver: Interceptor_Doors_Driver
	{
		displayName="$STR_Interceptor_codriver_name";
		descriptionShort="$STR_Interceptor_doors_codriver_disc";
		model="Interceptor_byALEX\proxy\Interceptor_Doors_coDriver.p3d";
		inventorySlot="Interceptor_Doors_coDriver";
	};
	class Interceptor_Doors_Driver_blue: Interceptor_Doors_Driver
	{
		displayName="$STR_Interceptor_Blue_driver_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_blue_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_coDriver_blue: Interceptor_Doors_coDriver
	{
		displayName="$STR_Interceptor_Blue_codriver_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_blue_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_Driver_red: Interceptor_Doors_Driver
	{
		displayName="$STR_Interceptor_Red_driver_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_red_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_coDriver_red: Interceptor_Doors_coDriver
	{
		displayName="$STR_Interceptor_Red_codriver_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_red_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_Driver_yellow: Interceptor_Doors_Driver
	{
		displayName="$STR_Interceptor_Yellow_driver_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_yellow_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_coDriver_yellow: Interceptor_Doors_coDriver
	{
		displayName="$STR_Interceptor_Yellow_codriver_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_yellow_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_Driver_Black: Interceptor_Doors_Driver
	{
		displayName="$STR_Interceptor_Black_driver_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_black_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_coDriver_Black: Interceptor_Doors_coDriver
	{
		displayName="$STR_Interceptor_Black_codriver_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_black_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_Driver_Sonic: Interceptor_Doors_Driver
	{
		displayName="$STR_Interceptor_Sonic_driver_name";
		hiddenSelectionsTextures[]=
		{
			"interceptor_byalex\data\body_sonic_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_coDriver_Sonic: Interceptor_Doors_coDriver
	{
		displayName="$STR_Interceptor_Sonic_codriver_name";
		hiddenSelectionsTextures[]=
		{
			"interceptor_byalex\data\body_sonic_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_hood: CarDoor
	{
		scope=2;
		displayName="$STR_Interceptor_hood_name";
		descriptionShort="$STR_Interceptor_hood_disc";
		model="Interceptor_byALEX\proxy\Interceptor_Doors_hood.p3d";
		weight=10000;
		itemSize[]={6,6};
		itemBehaviour=0;
		inventorySlot="Interceptor_Doors_hood";
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
			"Interceptor_byalex\data\Body_co.paa",
			"",
			""
		};
		hiddenSelectionsMaterials[]=
		{
			"Interceptor_byalex\data\Body.rvmat",
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
									"Interceptor_byalex\data\Body.rvmat"
								}
							},
							
							{
								0.70099998,
								{}
							},
							
							{
								0.50099999,
								
								{
									"Interceptor_byalex\data\base\body_damage.rvmat"
								}
							},
							
							{
								0.301,
								{}
							},
							
							{
								0.001,
								
								{
									"Interceptor_byalex\data\base\body_destruct.rvmat"
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
	class Interceptor_Doors_hood_blue: Interceptor_Doors_hood
	{
		displayName="$STR_Interceptor_Blue_hood_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_blue_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_hood_red: Interceptor_Doors_hood
	{
		displayName="$STR_Interceptor_Red_hood_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_red_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_hood_yellow: Interceptor_Doors_hood
	{
		displayName="$STR_Interceptor_Yellow_hood_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_yellow_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_hood_Black: Interceptor_Doors_hood
	{
		displayName="$STR_Interceptor_Black_hood_name";
		hiddenSelectionsTextures[]=
		{
			"Interceptor_byalex\data\Body_black_co.paa",
			"",
			""
		};
	};
	class Interceptor_Doors_hood_Sonic: Interceptor_Doors_hood
	{
		displayName="$STR_Interceptor_Sonic_hood_name";
		hiddenSelectionsTextures[]=
		{
			"interceptor_byalex\data\body_sonic_co.paa",
			"",
			""
		};
	};
	class Interceptor_blue: Interceptor
	{
		displayName="$STR_Interceptor_Blue_name";
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
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\Body_blue_co.paa",
			"interceptor_byalex\data\Body_blue_co.paa",
			"interceptor_byalex\data\Body_blue_co.paa",
			"interceptor_byalex\data\Body_blue_co.paa",
			"interceptor_byalex\data\Body_blue_co.paa",
			"interceptor_byalex\data\Body_blue_co.paa",
			"interceptor_byalex\data\Body_blue_co.paa",
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\salon_co.paa",
			"interceptor_byalex\data\Body_blue_co.paa"
		};
	};
	class Interceptor_red: Interceptor
	{
		displayName="$STR_Interceptor_Red_name";
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
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\Body_red_co.paa",
			"interceptor_byalex\data\Body_red_co.paa",
			"interceptor_byalex\data\Body_red_co.paa",
			"interceptor_byalex\data\Body_red_co.paa",
			"interceptor_byalex\data\Body_red_co.paa",
			"interceptor_byalex\data\Body_red_co.paa",
			"interceptor_byalex\data\Body_red_co.paa",
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\salon_co.paa",
			"interceptor_byalex\data\Body_red_co.paa"
		};
	};
	class Interceptor_yellow: Interceptor
	{
		displayName="$STR_Interceptor_Yellow_name";
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
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\Body_yellow_co.paa",
			"interceptor_byalex\data\Body_yellow_co.paa",
			"interceptor_byalex\data\Body_yellow_co.paa",
			"interceptor_byalex\data\Body_yellow_co.paa",
			"interceptor_byalex\data\Body_yellow_co.paa",
			"interceptor_byalex\data\Body_yellow_co.paa",
			"interceptor_byalex\data\Body_yellow_co.paa",
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\salon_co.paa",
			"interceptor_byalex\data\Body_yellow_co.paa"
		};
	};
	class Interceptor_Black: Interceptor
	{
		displayName="$STR_Interceptor_Black_name";
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
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\Body_Black_co.paa",
			"interceptor_byalex\data\Body_Black_co.paa",
			"interceptor_byalex\data\Body_Black_co.paa",
			"interceptor_byalex\data\Body_Black_co.paa",
			"interceptor_byalex\data\Body_Black_co.paa",
			"interceptor_byalex\data\Body_Black_co.paa",
			"interceptor_byalex\data\Body_Black_co.paa",
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\salon_co.paa",
			"interceptor_byalex\data\Body_Black_co.paa"
		};
	};
	class Interceptor_Sonic: Interceptor
	{
		displayName="$STR_Interceptor_Sonic_name";
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
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\body_sonic_co.paa",
			"interceptor_byalex\data\body_sonic_co.paa",
			"interceptor_byalex\data\body_sonic_co.paa",
			"interceptor_byalex\data\body_sonic_co.paa",
			"interceptor_byalex\data\body_sonic_co.paa",
			"interceptor_byalex\data\body_sonic_co.paa",
			"interceptor_byalex\data\body_sonic_co.paa",
			"interceptor_byalex\data\chassis_co.paa",
			"interceptor_byalex\data\salon_co.paa",
			"interceptor_byalex\data\body_sonic_co.paa"
		};
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
	class ProxyInterceptor_Wheel: ProxyVehiclePart
	{
		model="Interceptor_byALEX\proxy\Interceptor_Wheel.p3d";
		inventorySlot[]=
		{
			"Interceptor_Wheel_1_1",
			"Interceptor_Wheel_1_2",
			"Interceptor_Wheel_2_1",
			"Interceptor_Wheel_2_2",
			"Interceptor_Wheel_spare_1"
		};
	};
	class ProxyInterceptor_Wheel_destroyed: ProxyVehiclePart
	{
		model="Interceptor_byALEX\proxy\Interceptor_Wheel_destroyed.p3d";
		inventorySlot[]=
		{
			"Interceptor_Wheel_1_1",
			"Interceptor_Wheel_1_2",
			"Interceptor_Wheel_2_1",
			"Interceptor_Wheel_2_2",
			"Interceptor_Wheel_spare_1"
		};
	};
	class ProxyInterceptor_Doors_Driver: ProxyVehiclePart
	{
		model="Interceptor_byALEX\proxy\Interceptor_Doors_Driver.p3d";
		inventorySlot="Interceptor_Doors_Driver";
	};
	class ProxyInterceptor_Doors_coDriver: ProxyVehiclePart
	{
		model="Interceptor_byALEX\proxy\Interceptor_Doors_coDriver.p3d";
		inventorySlot="Interceptor_Doors_coDriver";
	};
	class ProxyInterceptor_Doors_hood: ProxyVehiclePart
	{
		model="Interceptor_byALEX\proxy\Interceptor_Doors_hood.p3d";
		inventorySlot="Interceptor_Doors_hood";
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
class CfgSoundShaders
{
	class baseEngineInterceptor_SoundShader
	{
		range=80;
	};
	class Interceptor_Engine_Ext_Rpm0_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Rpm0",
				1
			}
		};
		frequency="1 * ((850 + ((rpm - 850)/(8000/5600))) max 850) / 850";
		volume="0.75 * 1 * engineOn * 0.4 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((850+1200)/2) + 2.5*50),(((850+1200)/2) - 50)]) * ((1 - 0.25*doors) max campos)";
	};
	class Interceptor_Engine_Ext_Rpm1_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Rpm1",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 1200";
		volume="0.75 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.6 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((850+1200)/2) - 2.5*50),(((850+1200)/2) + 50)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) + 2.5*150),(((1200+1700)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class Interceptor_Engine_Offload_Ext_Rpm1_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Offload_Rpm1",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 1200";
		volume="0.75 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.6 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((850+1200)/2) - 2.5*50),(((850+1200)/2) + 50)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) + 2.5*150),(((1200+1700)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class Interceptor_Engine_Ext_Rpm2_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Rpm2",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 1700";
		volume="0.75 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.85 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) - 2.5*150),(((1200+1700)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) + 2.5*150),(((1700+2300)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class Interceptor_Engine_Offload_Ext_Rpm2_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Offload_Rpm2",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 1700";
		volume="0.75 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.85 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) - 2.5*150),(((1200+1700)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) + 2.5*150),(((1700+2300)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class Interceptor_Engine_Ext_Rpm3_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Rpm3",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 2300";
		volume="0.75 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) - 2.5*150),(((1700+2300)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) + 2.5*200),(((2300+3150)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Interceptor_Engine_Offload_Ext_Rpm3_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Offload_Rpm3",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 2300";
		volume="0.75 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) - 2.5*150),(((1700+2300)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) + 2.5*200),(((2300+3150)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Interceptor_Engine_Ext_Rpm4_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Rpm4",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 3150";
		volume="0.75 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) - 2.5*200),(((2300+3150)/2) + 200)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) + 2.5*200),(((3150+4400)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Interceptor_Engine_Offload_Ext_Rpm4_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Offload_Rpm4",
				2
			}
		};
		frequency="1 * (850 + ((rpm - 850)/(8000/5600))) / 3150";
		volume="0.75 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) - 2.5*200),(((2300+3150)/2) + 200)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) + 2.5*200),(((3150+4400)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Interceptor_Engine_Ext_Rpm5_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Rpm5",
				2
			}
		};
		frequency="(1 * (850 + ((rpm - 850)/(8000/5600))) / 4400) min (8000/5600)";
		volume="0.75 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) - 2.5*200),(((3150+4400)/2) + 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Interceptor_Engine_Offload_Ext_Rpm5_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Offload_Rpm5",
				2
			}
		};
		frequency="(1 * (850 + ((rpm - 850)/(8000/5600))) / 4400) min (8000/5600)";
		volume="0.75 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) - 2.5*200),(((3150+4400)/2) + 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Interceptor_Engine_Ext_Broken_SoundShader: baseEngineInterceptor_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\Interceptor_Engine_Ext_Broken",
				2
			}
		};
		frequency=0.89999998;
		volume="0.75 * 1 * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) - 2.5*200),(((3150+4400)/2) + 200)]) * ((1 - 0.25*doors) max campos) * (rpm factor[6000,7200])";
	};
	class baseInterceptorEngine_StartStop_SoundShader
	{
		range=80;
	};
	class Interceptor_Engine_start_SoundShader: baseInterceptorEngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\enginestartstop\engine_start",
				1
			}
		};
		volume=1.2;
	};
	class Interceptor_Engine_failed_start_fuel_SoundShader: baseInterceptorEngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\enginestartstop\engine_failed_start_fuel",
				1
			}
		};
		volume=1;
	};
	class Interceptor_Engine_failed_start_battery_SoundShader: baseInterceptorEngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\enginestartstop\engine_failed_start_battery",
				1
			}
		};
		volume=1;
	};
	class Interceptor_Engine_failed_start_sparkplugs_SoundShader: baseInterceptorEngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\enginestartstop\engine_failed_start_sparkplugs",
				1
			}
		};
		volume=1;
	};
	class Interceptor_Engine_stop_SoundShader: baseInterceptorEngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\enginestartstop\engine_stop",
				1
			}
		};
		volume=1;
	};
	class Interceptor_Engine_stop_fuel_SoundShader: baseInterceptorEngine_StartStop_SoundShader
	{
		samples[]=
		{
			
			{
				"Interceptor_byALEX\sounds\enginestartstop\engine_stop_fuel",
				1
			}
		};
		volume=1;
	};
};
class CfgSoundSets
{
	class baseEngine_EXT_SoundSet
	{
		sound3DProcessingType="Vehicle_Ext_3DProcessingType";
		distanceFilter="softVehiclesDistanceFreqAttenuationFilter";
		volumeCurve="vehicleEngineAttenuationCurve";
		volumeFactor=1;
		spatial=1;
		loop=1;
	};
	class Interceptor_Engine_Ext_Rpm0_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Ext_Rpm0_SoundShader"
		};
		volumeFactor=1;
	};
	class Interceptor_Engine_Offload_Ext_Rpm1_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Offload_Ext_Rpm1_SoundShader"
		};
		volumeFactor=1;
	};
	class Interceptor_Engine_Ext_Rpm1_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Ext_Rpm1_SoundShader"
		};
		volumeFactor=1;
	};
	class Interceptor_Engine_Ext_Rpm2_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Ext_Rpm2_SoundShader"
		};
		volumeFactor=1;
	};
	class Interceptor_Engine_Offload_Ext_Rpm2_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Offload_Ext_Rpm2_SoundShader"
		};
		volumeFactor=1;
	};
	class Interceptor_Engine_Ext_Rpm3_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Ext_Rpm3_SoundShader"
		};
		volumeFactor=1;
	};
	class Interceptor_Engine_Offload_Ext_Rpm3_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Offload_Ext_Rpm3_SoundShader"
		};
		volumeFactor=1;
	};
	class Interceptor_Engine_Ext_Rpm4_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Ext_Rpm4_SoundShader"
		};
		volumeFactor=1;
	};
	class Interceptor_Engine_Offload_Ext_Rpm4_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Offload_Ext_Rpm4_SoundShader"
		};
		volumeFactor=1;
	};
	class Interceptor_Engine_Ext_Rpm5_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Ext_Rpm5_SoundShader"
		};
		volumeFactor=1;
	};
	class Interceptor_Engine_Offload_Ext_Rpm5_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Offload_Ext_Rpm5_SoundShader"
		};
		volumeFactor=1;
	};
	class Interceptor_Engine_Ext_Broken_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_Ext_Broken_SoundShader"
		};
		volumeFactor=1;
	};
	class baseInterceptor_Engine_StartStop_SoundSet
	{
		sound3DProcessingType="VehicleExt_3DProcessingType";
		volumeFactor=1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Interceptor_Engine_start_SoundSet: baseInterceptor_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_engine_start_SoundShader"
		};
		frequencyFactor=0.94999999;
		startDelay=0.25;
		positionOffset[]={0,0,1};
	};
	class Interceptor_Engine_failed_start_fuel_SoundSet: baseInterceptor_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_engine_failed_start_fuel_SoundShader"
		};
		frequencyFactor=0.94999999;
		startDelay=0.25;
		positionOffset[]={0,0,1};
	};
	class Interceptor_Engine_failed_start_battery_SoundSet: baseInterceptor_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_engine_failed_start_battery_SoundShader"
		};
		frequencyFactor=0.94999999;
		startDelay=0.25;
		positionOffset[]={0,0,1};
	};
	class Interceptor_Engine_failed_start_sparkplugs_SoundSet: baseInterceptor_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_engine_failed_start_sparkplugs_SoundShader"
		};
		frequencyFactor=0.94999999;
		startDelay=0.25;
		positionOffset[]={0,0,1};
	};
	class Interceptor_Engine_stop_SoundSet: baseInterceptor_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_engine_stop_SoundShader"
		};
		frequencyFactor=0.94999999;
		positionOffset[]={0,0,1};
	};
	class Interceptor_Engine_stop_fuel_SoundSet: baseInterceptor_Engine_StartStop_SoundSet
	{
		soundShaders[]=
		{
			"Interceptor_Engine_stop_fuel_SoundShader"
		};
		frequencyFactor=0.94999999;
		positionOffset[]={0,0,1};
	};
};
