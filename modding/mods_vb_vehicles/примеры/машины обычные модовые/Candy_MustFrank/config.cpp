////////////////////////////////////////////////////////////////////
//DeRap: config.bin
//Produced from mikero's Dos Tools Dll version 9.10
//https://mikero.bytex.digital/Downloads
//'now' is Sat Oct 14 18:02:26 2023 : 'file' last modified on Sat Oct 14 09:48:24 2023
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgSoundShaders
{
	class baseEngineCandy_MustFrank_SoundShader
	{
		range = 80;
	};
	class Candy_MustFrank_engine_ext_rpm0_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_engine_ext_rpm0",1}};
		frequency = "1 * ((850 + ((rpm - 850)/(8000/5600))) max 850) / 850";
		volume = "0.70 * 1 * engineOn * 0.4 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((850+1200)/2) + 2.5*50),(((850+1200)/2) - 50)]) * ((1 - 0.25*doors) max campos)";
	};
	class Candy_MustFrank_engine_ext_rpm1_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_engine_ext_rpm1",2}};
		frequency = "0.7 * (850 + ((rpm - 850)/(8000/5600))) / 1200";
		volume = "0.80 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.6 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((850+1200)/2) - 2.5*50),(((850+1200)/2) + 50)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) + 2.5*150),(((1200+1700)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class Candy_MustFrank_engine_offload_ext_rpm1_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_engine_ext_offload_rpm1",2}};
		frequency = "1 * (850 + ((rpm - 850)/(8000/5600))) / 1200";
		volume = "0.80 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.6 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((850+1200)/2) - 2.5*50),(((850+1200)/2) + 50)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) + 2.5*150),(((1200+1700)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class Candy_MustFrank_engine_ext_rpm2_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_engine_ext_rpm2",2}};
		frequency = "0.7 * (850 + ((rpm - 850)/(8000/5600))) / 1700";
		volume = "0.85 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.85 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) - 2.5*150),(((1200+1700)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) + 2.5*150),(((1700+2300)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class Candy_MustFrank_engine_offload_ext_rpm2_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_engine_ext_offload_rpm2",2}};
		frequency = "0.7 * (850 + ((rpm - 850)/(8000/5600))) / 1700";
		volume = "0.85 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 0.85 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1200+1700)/2) - 2.5*150),(((1200+1700)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) + 2.5*150),(((1700+2300)/2) - 150)]) * ((1 - 0.25*doors) max campos)";
	};
	class Candy_MustFrank_engine_ext_rpm3_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_engine_ext_rpm3",2}};
		frequency = "0.7 * (850 + ((rpm - 850)/(8000/5600))) / 2300";
		volume = "0.85 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) - 2.5*150),(((1700+2300)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) + 2.5*200),(((2300+3150)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Candy_MustFrank_engine_offload_ext_rpm3_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_engine_ext_offload_rpm3",2}};
		frequency = "0.75 * (850 + ((rpm - 850)/(8000/5600))) / 2300";
		volume = "0.85 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((1700+2300)/2) - 2.5*150),(((1700+2300)/2) + 150)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) + 2.5*200),(((2300+3150)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Candy_MustFrank_engine_ext_rpm4_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_engine_ext_rpm4",2}};
		frequency = "0.75 * (850 + ((rpm - 850)/(8000/5600))) / 3150";
		volume = "0.90 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) - 2.5*200),(((2300+3150)/2) + 200)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) + 2.5*200),(((3150+4400)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Candy_MustFrank_engine_offload_ext_rpm4_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_engine_ext_offload_rpm4",2}};
		frequency = "0.725 * (850 + ((rpm - 850)/(8000/5600))) / 3150";
		volume = "0.90 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((2300+3150)/2) - 2.5*200),(((2300+3150)/2) + 200)]) * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) + 2.5*200),(((3150+4400)/2) - 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Candy_MustFrank_engine_ext_rpm5_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_engine_ext_rpm5",2}};
		frequency = "(0.65 * (850 + ((rpm - 850)/(8000/5600))) / 4400) min (8000/5600)";
		volume = "0.90 * 1 * (thrust factor[0.1,0.45]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) - 2.5*200),(((3150+4400)/2) + 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Candy_MustFrank_engine_offload_ext_rpm5_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_engine_ext_offload_rpm5",2}};
		frequency = "(0.65 * (850 + ((rpm - 850)/(8000/5600))) / 4400) min (8000/5600)";
		volume = "0.90 * 1 * (thrust factor[0.6,0.2]) * (0.7 + 0.3 * (speed factor [10,60])) * engineOn * 1 * ((850 + ((rpm - 850)/(8000/5600))) factor [(((3150+4400)/2) - 2.5*200),(((3150+4400)/2) + 200)]) * ((1 - 0.25*doors) max campos)";
	};
	class Candy_MustFrank_Engine_ext_start_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_Engine_ext_start",1}};
		volume = 0.35;
	};
	class Candy_MustFrank_door_open_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_door_open",1}};
		volume = 0.6;
	};
	class Candy_MustFrank_door_close_SoundShader: baseEngineCandy_MustFrank_SoundShader
	{
		samples[] = {{"Candy_MustFrank\sounds\Candy_MustFrank_door_close",1}};
		volume = 0.7;
	};
};
class CfgSoundSets
{
	class baseEngine_EXT_SoundSet
	{
		sound3DProcessingType = "Vehicle_Ext_3DProcessingType";
		distanceFilter = "softVehiclesDistanceFreqAttenuationFilter";
		volumeCurve = "vehicleEngineAttenuationCurve";
		volumeFactor = 1;
		spatial = 1;
		loop = 1;
	};
	class Candy_MustFrank_engine_ext_rpm0_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_engine_ext_rpm0_SoundShader"};
		volumeFactor = 1;
		startDelay = 1;
	};
	class Candy_MustFrank_engine_offload_ext_rpm1_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_engine_offload_ext_rpm1_SoundShader"};
		volumeFactor = 1;
		startDelay = 1;
	};
	class Candy_MustFrank_engine_ext_rpm1_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_engine_ext_rpm1_SoundShader"};
		volumeFactor = 1;
		startDelay = 1;
	};
	class Candy_MustFrank_engine_ext_rpm2_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_engine_ext_rpm2_SoundShader"};
		volumeFactor = 1;
		startDelay = 1;
	};
	class Candy_MustFrank_engine_offload_ext_rpm2_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_engine_offload_ext_rpm2_SoundShader"};
		volumeFactor = 1;
		startDelay = 1;
	};
	class Candy_MustFrank_engine_ext_rpm3_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_engine_ext_rpm3_SoundShader"};
		volumeFactor = 1;
		startDelay = 1;
	};
	class Candy_MustFrank_engine_offload_ext_rpm3_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_engine_offload_ext_rpm3_SoundShader"};
		volumeFactor = 1;
		startDelay = 1;
	};
	class Candy_MustFrank_engine_ext_rpm4_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_engine_ext_rpm4_SoundShader"};
		volumeFactor = 1;
		startDelay = 1;
	};
	class Candy_MustFrank_engine_offload_ext_rpm4_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_engine_offload_ext_rpm4_SoundShader"};
		volumeFactor = 1;
		startDelay = 1;
	};
	class Candy_MustFrank_engine_ext_rpm5_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_engine_ext_rpm5_SoundShader"};
		volumeFactor = 1;
		startDelay = 1;
	};
	class Candy_MustFrank_engine_offload_ext_rpm5_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_engine_offload_ext_rpm5_SoundShader"};
		volumeFactor = 1;
		startDelay = 1;
	};
	class Candy_MustFrank_Engine_ext_start_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_Engine_ext_start_SoundShader"};
		frequencyFactor = 0.95;
		startDelay = 0;
		positionOffset[] = {0,0,1};
		loop = 0;
	};
	class Candy_MustFrank_door_open_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_door_open_SoundShader"};
		frequencyFactor = 0.95;
		startDelay = 0;
		positionOffset[] = {0,0,0};
		loop = 0;
	};
	class Candy_MustFrank_door_close_SoundSet: baseEngine_EXT_SoundSet
	{
		soundShaders[] = {"Candy_MustFrank_door_close_SoundShader"};
		frequencyFactor = 0.95;
		startDelay = 0;
		positionOffset[] = {0,0,0};
		loop = 0;
	};
};
class CfgMods
{
	class Candy_MustFrank
	{
		dir = "Candy_MustFrank";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "Candy_MustFrank";
		credits = "";
		author = "";
		authorID = "0";
		version = 1;
		extra = 0;
		type = "mod";
		dependencies[] = {"World","Game"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"Candy_MustFrank\scripts\4_World"};
			};
		};
	};
};
class CfgPatches
{
	class Candy_MustFrank
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Vehicles_Wheeled","DZ_Vehicles_Parts","DZ_Data","DZ_Scripts","DZ_Sounds_Effects","DZ_Gear_Containers","DZ_Gear_Camping"};
	};
};
class CfgSlots
{
	class Slot_Candy_MustFrank_Wheel_1_1
	{
		name = "Candy_MustFrank_Wheel_1_1";
		displayName = "Mustang Frankenstein Wheel";
		selection = "wheel_1_1";
		ghostIcon = "wheel";
	};
	class Slot_Candy_MustFrank_Wheel_1_2
	{
		name = "Candy_MustFrank_Wheel_1_2";
		displayName = "Mustang Frankenstein Wheel";
		selection = "wheel_1_2";
		ghostIcon = "wheel";
	};
	class Slot_Candy_MustFrank_Wheel_2_1
	{
		name = "Candy_MustFrank_Wheel_2_1";
		displayName = "Mustang Frankenstein Wheel";
		selection = "wheel_2_1";
		ghostIcon = "wheel";
	};
	class Slot_Candy_MustFrank_Wheel_2_2
	{
		name = "Candy_MustFrank_Wheel_2_2";
		displayName = "Mustang Frankenstein Wheel";
		selection = "wheel_2_2";
		ghostIcon = "wheel";
	};
	class Slot_Candy_MustFrank_Wheel_spare_1
	{
		name = "Candy_MustFrank_Wheel_spare_1";
		displayName = "Mustang Frankenstein Wheel";
		selection = "wheel_spare_1";
		ghostIcon = "wheel";
	};
	class Slot_Candy_MustFrank_Doors_Driver
	{
		name = "Candy_MustFrank_Doors_Driver";
		displayName = "Mustang Frankenstein Doors Driver";
		selection = "doors_driver";
		ghostIcon = "doorfront";
	};
	class Slot_Candy_MustFrank_Doors_CoDriver
	{
		name = "Candy_MustFrank_Doors_CoDriver";
		displayName = "Mustang Frankenstein Doors CoDriver";
		selection = "doors_codriver";
		ghostIcon = "doorfront";
	};
	class Slot_Candy_MustFrank_Doors_Trunk
	{
		name = "Candy_MustFrank_Doors_Trunk";
		displayName = "Mustang Frankenstein Doors Trunk";
		selection = "doors_trunk";
		ghostIcon = "trunk";
	};
	class Slot_Candy_MustFrank_Doors_Trunk0
	{
		name = "Candy_MustFrank_Doors_Trunk0";
		displayName = "Mustang Frankenstein Doors Trunk0";
		selection = "doors_trunk0";
		ghostIcon = "trunk0";
	};
	class Slot_Candy_MustFrank_Doors_Hood
	{
		name = "Candy_MustFrank_Doors_Hood";
		displayName = "Mustang Frankenstein Doors Hood";
		selection = "doors_hood";
		ghostIcon = "hood";
	};
	class Slot_Truck_01_WoodenCrate2
	{
		name="Truck_01_WoodenCrate2";
		displayName="Truck_01_WoodenCrate2";
		selection="Truck_01_WoodenCrate2";
		ghostIcon="set:dayz_inventory image:woodcrate";
	};
};
class CfgVehicles
{
	class CarWheel;
	class Crew;
	class CarDoor;
	class Car;
	class Driver;
	class CoDriver;
	class SimulationModule;
	class Axles;
	class Front;
	class Rear;
	class Left;
	class Right;
	class Wheels;
	class Window;
	class AnimationSources;
	class DamageSystem;
	class GlobalHealth;
	class Health;
	class Doors;
	class DamageZones;
	class Inventory_Base;
	class CarScript;
	class Candy_MustFrank_Wheel: CarWheel
	{
		scope = 2;
		displayName = "Mustang Frankenstein Wheel";
		descriptionShort = "Mustang Frankenstein Wheel_desc";
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Wheel.p3d";
		weight = 15000;
		inventorySlot[] = {"Candy_MustFrank_Wheel_1_1","Candy_MustFrank_Wheel_1_2","Candy_MustFrank_Wheel_2_1","Candy_MustFrank_Wheel_2_2","Candy_MustFrank_Wheel_spare_1"};
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"Candy_MustFrank\data\mustfrank_kuzov_co.paa"};
		hiddenSelectionsMaterials[] = {"Candy_MustFrank\data\mustfrank_kuzov.rvmat"};
		itemSize[] = {6,6};
		rotationFlags = 4;
		repairableWithKits[] = {6};
		repairCosts[] = {30};
		radiusByDamage[] = {0,0.294,0.3,0.3,0.9998,0.25,0.9999,0.2};
		radius = 0.294;
		width = 0.18;
		tyreOffroadResistance = 0.75;
		tyreGrip = 0.8;
		tyreRollResistance = 0.015;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1500;
					healthLevels[] = {{1.01,{""}},{0.7,{""}},{0.5,{""}},{0.3,{""}},{0.01,{""}}};
				};
			};
		};
	};
	class Candy_MustFrank_Wheel_Ruined: Candy_MustFrank_Wheel { scope=2; };
	class Candy_MustFrank_Doors_Driver: CarDoor
	{
		scope = 2;
		displayName = "Mustang Frankenstein Doors Driver";
		descriptionShort = "Mustang Frankenstein Doors Driver_desc";
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Driver.p3d";
		itemBehaviour = 0;
		inventorySlot = "Candy_MustFrank_Doors_Driver";
		rotationFlags = 8;
		weight = 15000;
		itemSize[] = {10,10};
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"Candy_MustFrank\data\mustfrank_kuzov_co.paa"};
		class DamageSystem: DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 20000;
					healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
				};
			};
			class DamageZones: DamageZones
			{
				class Window: Window
				{
					class Health: Health
					{
						hitpoints = 3200;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1.01,{"Candy_MustFrank\data\steklo.rvmat"}},{0.7,{}},{0.5,{"Candy_MustFrank\data\steklo_damage.rvmat"}},{0.3,{"Candy_MustFrank\data\steklo_destruct.rvmat"}},{0.01,"hidden"}};
					};
					componentNames[] = {"dmgzone_window"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {};
				};
				class Doors: Doors
				{
					class Health: Health
					{
						hitpoints = 11000;
						transferToGlobalCoef = 1;
						healthLevels[] = {{1.01,{""}},{0.7,{""}},{0.5,{""}},{0.3,{""}},{0.01,{""}}};
					};
					componentNames[] = {"dmgzone_doors"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {};
				};
			};
		};
	};
	class Candy_MustFrank_Doors_CoDriver: Candy_MustFrank_Doors_Driver
	{
		scope = 2;
		displayName = "Mustang Frankenstein Doors Driver";
		descriptionShort = "Mustang Frankenstein Doors Driver_desc";
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_CoDriver.p3d";
		itemBehaviour = 0;
		inventorySlot = "Candy_MustFrank_Doors_CoDriver";
		rotationFlags = 4;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"Candy_MustFrank\data\mustfrank_kuzov_co.paa"};
	};
	class Candy_MustFrank_Doors_Trunk: Candy_MustFrank_Doors_Driver
	{
		scope = 2;
		displayName = "Mustang Frankenstein Doors Trunk";
		descriptionShort = "Mustang Frankenstein Doors Trunk";
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Trunk.p3d";
		itemBehaviour = 0;
		inventorySlot = "Candy_MustFrank_Doors_Trunk";
		rotationFlags = 4;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"Candy_MustFrank\data\mustfrank_kuzov_co.paa"};
	};
	class Candy_MustFrank_Doors_Trunk0: Inventory_Base
	{
		scope = 2;
		displayName = "Mustang Frankenstein Doors Trunk0";
		descriptionShort = "Mustang Frankenstein Doors Trunk0";
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Trunk0.p3d";
		itemSize[] = {6,6};
		itemBehaviour = 0;
		inventorySlot = "Candy_MustFrank_Doors_Trunk0";
		rotationFlags = 4;
		physLayer = "item_large";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"Candy_MustFrank\data\mustfrank_kuzov_co.paa"};
	};
	class Candy_MustFrank_Doors_Hood: Candy_MustFrank_Doors_Driver
	{
		scope = 2;
		displayName = "Mustang Frankenstein Doors Hood";
		descriptionShort = "Mustang Frankenstein Doors Hood";
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Hood.p3d";
		itemBehaviour = 0;
		inventorySlot = "Candy_MustFrank_Doors_Hood";
		rotationFlags = 4;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"Candy_MustFrank\data\mustfrank_kuzov_co.paa"};
	};
	class Candy_MustFrank_Doors_Driver_White: Candy_MustFrank_Doors_Driver
	{
		scope = 2;
		displayName = "Mustang Frankenstein Doors Driver White";
		descriptionShort = "Mustang Frankenstein Doors Driver White";
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Driver.p3d";
		itemBehaviour = 0;
		inventorySlot = "Candy_MustFrank_Doors_Driver";
		rotationFlags = 8;
		weight = 15000;
		itemSize[] = {10,10};
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"Candy_MustFrank\data\mustfrank_kuzov_co_White.paa"};
	};
	class Candy_MustFrank_Doors_CoDriver_White: Candy_MustFrank_Doors_Driver
	{
		scope = 2;
		displayName = "Mustang Frankenstein Doors Driver White";
		descriptionShort = "Mustang Frankenstein Doors Driver White";
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_CoDriver.p3d";
		itemBehaviour = 0;
		inventorySlot = "Candy_MustFrank_Doors_CoDriver";
		rotationFlags = 4;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"Candy_MustFrank\data\mustfrank_kuzov_co_White.paa"};
	};
	class Candy_MustFrank_Doors_Trunk_White: Candy_MustFrank_Doors_Driver
	{
		scope = 2;
		displayName = "Mustang Frankenstein Doors Trunk White";
		descriptionShort = "Mustang Frankenstein Doors Trunk White";
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Trunk.p3d";
		itemBehaviour = 0;
		inventorySlot = "Candy_MustFrank_Doors_Trunk";
		rotationFlags = 4;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"Candy_MustFrank\data\mustfrank_kuzov_co_White.paa"};
	};
	class Candy_MustFrank_Doors_Trunk0_White: Inventory_Base
	{
		scope = 2;
		displayName = "Mustang Frankenstein Doors Trunk0 White";
		descriptionShort = "Mustang Frankenstein Doors Trunk0 White";
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Trunk0.p3d";
		itemSize[] = {6,6};
		itemBehaviour = 0;
		inventorySlot = "Candy_MustFrank_Doors_Trunk0";
		rotationFlags = 4;
		physLayer = "item_large";
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"Candy_MustFrank\data\mustfrank_kuzov_co_White.paa"};
	};
	class Candy_MustFrank_Doors_Hood_White: Candy_MustFrank_Doors_Driver
	{
		scope = 2;
		displayName = "Mustang Frankenstein Doors Hood White";
		descriptionShort = "Mustang Frankenstein Doors Hood White";
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Hood.p3d";
		itemBehaviour = 0;
		inventorySlot = "Candy_MustFrank_Doors_Hood";
		rotationFlags = 4;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"Candy_MustFrank\data\mustfrank_kuzov_co_White.paa"};
	};
	class Candy_MustFrank: CarScript
	{
		scope = 2;
		displayname = "Mustang Frankenstein";
		Model = "Candy_MustFrank\Candy_MustFrank.p3d";
		attachments[] = {"CarRadiator","CarBattery","Reflector_1_1","Reflector_2_1","SparkPlug","Candy_MustFrank_Wheel_1_1","Candy_MustFrank_Wheel_1_2","Candy_MustFrank_Wheel_2_1","Candy_MustFrank_Wheel_2_2","Candy_MustFrank_Wheel_spare_1","Candy_MustFrank_Doors_Driver","Candy_MustFrank_Doors_CoDriver","Candy_MustFrank_Doors_Trunk","Candy_MustFrank_Doors_Trunk0","Candy_MustFrank_Doors_Hood","truck_01_woodencrate1","back","CanisterGasoline","Cassette"};
		hiddenSelections[] = {"light_1_1","light_2_1","light_brake_1_2","light_brake_2_2","light_reverse_1_2","light_reverse_2_2","light_1_2","light_2_2","light_dashboard","light_offroad","light_offroad2","nomer","camo"};
		hiddenSelectionsTextures[] = {"candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\chrome.paa","candy_mustfrank\data\chrome.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\mustang_int_symboles.paa","candy_mustfrank\data\mustang_gauges.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\numer_mil_co.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa"};
		dashboardMatOn = "Candy_MustFrank\data\dashlight.rvmat";
		dashboardMatOff = "Candy_MustFrank\data\glossy.rvmat";
		frontReflectorMatOn = "Candy_MustFrank\data\swiatlo.rvmat";
		frontReflectorMatOff = "Candy_MustFrank\data\mustfrank_kuzov.rvmat";
		brakeReflectorMatOn = "Candy_MustFrank\data\swiatlo2.rvmat";
		brakeReflectorMatOff = "Candy_MustFrank\data\glossy.rvmat";
		ReverseReflectorMatOn = "Candy_MustFrank\data\swiatlo.rvmat";
		ReverseReflectorMatOff = "Candy_MustFrank\data\chrome_dayz.rvmat";
		TailReflectorMatOn = "Candy_MustFrank\data\swiatlo2.rvmat";
		TailReflectorMatOff = "Candy_MustFrank\data\glossy.rvmat";
		OffroadReflectorMatOn = "Candy_MustFrank\data\dashlight.rvmat";
		OffroadReflectorMatOff = "Candy_MustFrank\data\glossy.rvmat";
		OffroadReflectorMatOn2 = "Candy_MustFrank\data\swiatlo.rvmat";
		OffroadReflectorMatOff2 = "Candy_MustFrank\data\mustfrank_kuzov.rvmat";
		fuelCapacity = 50;
		fuelConsumption = 15.5;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 15000;
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage=9.9999997e-10;
					};
				};
				class Melee
				{
					class Health
					{
						damage=9.9999997e-10;
					};
				};
				class Infected
				{
					class Health
					{
						damage=9.9999997e-10;
					};
				};
			};
			class DamageZones
			{
				class Chassis
				{
					fatalInjuryCoef = -1;
					componentNames[] = {"dmgzone_chassis"};
					class Health
					{
						hitpoints = 15000;
						transferToGlobalCoef = 0;
					};
					inventorySlots[] = {};
				};
				class Front
				{
					displayName = "$STR_CfgVehicleDmg_Bumper0";
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgzone_front"};
					componentNames[] = {"dmgzone_front","dmgzone_bumper_1"};
					class Health
					{
						hitpoints = 15000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body.rvmat"}},{0.7,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body.rvmat"}},{0.5,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body_damage.rvmat"}},{0.3,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body_damage.rvmat"}},{0,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body_destruct.rvmat"}}};
					};
					transferToZonesNames[] = {"Fender_1_1","Fender_2_1"};
					transferToZonesCoefs[] = {0.2,0.2,0.15};
					inventorySlots[] = {"Hatchback_02_Hood","CarRadiator","Hatchback_02_Wheel_1_1","Hatchback_02_Wheel_2_1"};
					inventorySlotsCoefs[] = {0.3,0.3,0.1,0.1};
				};
				class Reflector_1_1
				{
					displayName = "$STR_CfgVehicleDmg_Reflector0";
					fatalInjuryCoef = -1;
					componentNames[] = {"dmgzone_lights_1_1"};
					memoryPoints[] = {"dmgzone_lights_1_1"};
					class Health
					{
						hitpoints = 15000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1,{"dz\vehicles\wheeled\hatchback_02\data\hatchback_02_windows.rvmat"}},{0.7,{}},{0.5,{"dz\vehicles\wheeled\hatchback_02\data\glass_i_damage.rvmat"}},{0.3,{}},{0,{"dz\vehicles\wheeled\hatchback_02\data\glass_i_destruct.rvmat"}}};
					};
					transferToZonesNames[] = {"Front","Fender_1_1"};
					transferToZonesCoefs[] = {1,1};
					inventorySlots[] = {"Reflector_1_1","Hatchback_02_Wheel_1_1"};
					inventorySlotsCoefs[] = {1,0.1};
				};
				class Reflector_2_1: Reflector_1_1
				{
					memoryPoints[] = {"dmgzone_lights_2_1"};
					componentNames[] = {"dmgzone_lights_2_1"};
					transferToZonesNames[] = {"Front","Fender_2_1"};
					inventorySlots[] = {"Reflector_2_1","Hatchback_02_Wheel_2_1"};
				};
				class Back
				{
					displayName = "$STR_CfgVehicleDmg_Bumper0";
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgzone_back"};
					componentNames[] = {"dmgzone_back","dmgzone_bumper_2"};
					class Health
					{
						hitpoints = 15000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1,{"dz\vehicles\wheeled\hatchback_02\data\hatchback_02_body.rvmat"}},{0.7,{"dz\vehicles\wheeled\hatchback_02\data\hatchback_02_body.rvmat"}},{0.5,{"dz\vehicles\wheeled\hatchback_02\data\hatchback_02_body_damage.rvmat"}},{0.3,{"dz\vehicles\wheeled\hatchback_02\data\hatchback_02_body_damage.rvmat"}},{0,{"dz\vehicles\wheeled\hatchback_02\data\hatchback_02_body_destruct.rvmat"}}};
					};
					transferToZonesNames[] = {"Fender_1_2","Fender_2_2"};
					transferToZonesCoefs[] = {0.15,0.15};
					inventorySlots[] = {"Hatchback_02_Trunk","Hatchback_02_Wheel_1_2","Hatchback_02_Wheel_2_2"};
					inventorySlotsCoefs[] = {0.35,0.1,0.1};
				};
				class Roof
				{
					displayName = "$STR_CfgVehicleDmg_Roof0";
					fatalInjuryCoef = -1;
					componentNames[] = {"dmgzone_roof"};
					memoryPoints[] = {"dmgzone_roof"};
					class Health
					{
						hitpoints = 15000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body.rvmat"}},{0.7,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body.rvmat"}},{0.5,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body_damage.rvmat"}},{0.3,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body_damage.rvmat"}},{0,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body_destruct.rvmat"}}};
					};
					inventorySlots[] = {};
				};
				class Fender_1_1
				{
					displayName = "$STR_CfgVehicleDmg_Fender0";
					fatalInjuryCoef = -1;
					componentNames[] = {"dmgzone_fender_1_1"};
					memoryPoints[] = {"dmgzone_fender_1_1"};
					class Health
					{
						hitpoints = 15000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body.rvmat"}},{0.7,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body.rvmat"}},{0.5,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body_damage.rvmat"}},{0.3,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body_damage.rvmat"}},{0,{"dz\vehicles\wheeled\hatchback_02\data\red\hatchback_02_body_destruct.rvmat"}}};
					};
					transferToZonesNames[] = {"Front","Reflector_1_1","Engine"};
					transferToZonesCoefs[] = {0.2,0.05,0.08};
					inventorySlots[] = {"Hatchback_02_Hood","Hatchback_02_Wheel_1_1","Hatchback_02_Door_1_1"};
					inventorySlotsCoefs[] = {0.2,0.1,0.1};
				};
				class Fender_2_1: Fender_1_1
				{
					memoryPoints[] = {"dmgzone_fender_2_1"};
					componentNames[] = {"dmgzone_fender_2_1"};
					transferToZonesNames[] = {"Front","Reflector_2_1","Engine"};
					inventorySlots[] = {"Hatchback_02_Hood","Hatchback_02_Wheel_2_1","Hatchback_02_Door_2_1"};
				};
				class Fender_1_2: Fender_1_1
				{
					memoryPoints[] = {"dmgzone_fender_1_2"};
					componentNames[] = {"dmgzone_fender_1_2"};
					transferToZonesNames[] = {"Back","FuelTank"};
					transferToZonesCoefs[] = {0.15,0.4};
					inventorySlots[] = {"Hatchback_02_Trunk","Hatchback_02_Wheel_1_2","Hatchback_02_Door_1_2"};
					inventorySlotsCoefs[] = {0.2,0.1,0.1};
				};
				class Fender_2_2: Fender_1_1
				{
					memoryPoints[] = {"dmgzone_fender_2_2"};
					componentNames[] = {"dmgzone_fender_2_2"};
					transferToZonesNames[] = {"Back","FuelTank"};
					transferToZonesCoefs[] = {0.15,0.4};
					inventorySlots[] = {"Hatchback_02_Trunk","Hatchback_02_Wheel_2_2","Hatchback_02_Door_2_2"};
					inventorySlotsCoefs[] = {0.2,0.1,0.1};
				};
				class Windowfront
				{
					displayName = "$STR_CfgVehicleDmg_Window0";
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgzone_windowfront"};
					componentNames[] = {"dmgzone_windowfront"};
					class Health
					{
						hitpoints = 15000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1.01,{"Candy_MustFrank\data\steklo.rvmat"}},{0.7,{}},{0.5,{"Candy_MustFrank\data\steklo_damage.rvmat"}},{0.3,{"Candy_MustFrank\data\steklo_destruct.rvmat"}},{0.01,"hidden"}};
					};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {};
				};
				class Windowrear
				{
					displayName = "$STR_CfgVehicleDmg_Window0";
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgzone_windowrear"};
					componentNames[] = {"dmgzone_windowrear"};
					class Health
					{
						hitpoints = 15000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1,{"Candy_MustFrank\data\steklo.rvmat"}},{0.7,{}},{0.5,{"Candy_MustFrank\data\steklo_damage.rvmat"}},{0.3,{"Candy_MustFrank\data\steklo_destruct.rvmat"}},{0,"hidden"}};
					};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {};
				};
				class Windowleft
				{
					displayName = "$STR_CfgVehicleDmg_Window0";
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgzone_windowleft"};
					componentNames[] = {"dmgzone_windowleft"};
					class Health
					{
						hitpoints = 15000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1,{"Candy_MustFrank\data\steklo.rvmat"}},{0.7,{}},{0.5,{"Candy_MustFrank\data\steklo_damage.rvmat"}},{0.3,{"Candy_MustFrank\data\steklo_destruct.rvmat"}},{0,"hidden"}};
					};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {};
				};
				class Windowright
				{
					displayName = "$STR_CfgVehicleDmg_Window0";
					fatalInjuryCoef = -1;
					memoryPoints[] = {"dmgzone_windowright"};
					componentNames[] = {"dmgzone_windowright"};
					class Health
					{
						hitpoints = 15000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1,{"Candy_MustFrank\data\steklo.rvmat"}},{0.7,{}},{0.5,{"Candy_MustFrank\data\steklo_damage.rvmat"}},{0.3,{"Candy_MustFrank\data\steklo_destruct.rvmat"}},{0,"hidden"}};
					};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {};
				};
				class Engine
				{
					displayName = "$STR_CfgVehicleDmg_Engine0";
					fatalInjuryCoef = 0.001;
					memoryPoints[] = {"dmgzone_engine"};
					componentNames[] = {"dmgzone_engine"};
					class Health
					{
						hitpoints = 8000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0}};
					};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {0.1,0.05};
				};
				class FuelTank
				{
					displayName = "$STR_CfgVehicleDmg_FuelTank0";
					fatalInjuryCoef = -1;
					componentNames[] = {"dmgzone_fuelTank"};
					class Health
					{
						hitpoints = 15000;
						transferToGlobalCoef = 0;
						healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
					};
					inventorySlots[] = {};
					inventorySlotsCoefs[] = {};
				};
			};
		};
		class Sounds
		{
			thrust = 0.6;
			thrustTurbo = 1;
			thrustGentle = 0.3;
			thrustSmoothCoef = 0.1;
			camposSmoothCoef = 0.03;
			soundSetsFilter[] = {"Candy_MustFrank_Engine_Offload_Ext_Rpm1_SoundSet","Candy_MustFrank_Engine_Offload_Ext_Rpm2_SoundSet","Candy_MustFrank_Engine_Offload_Ext_Rpm3_SoundSet","Candy_MustFrank_Engine_Offload_Ext_Rpm4_SoundSet","Candy_MustFrank_Engine_Offload_Ext_Rpm5_SoundSet","Candy_MustFrank_Engine_Ext_Rpm0_SoundSet","Candy_MustFrank_Engine_Ext_Rpm1_SoundSet","Candy_MustFrank_Engine_Ext_Rpm2_SoundSet","Candy_MustFrank_Engine_Ext_Rpm3_SoundSet","Candy_MustFrank_Engine_Ext_Rpm4_SoundSet","Candy_MustFrank_Engine_Ext_Rpm5_SoundSet","offroad_Tires_rock_slow_Ext_SoundSet","offroad_Tires_rock_fast_Ext_SoundSet","offroad_Tires_grass_slow_Ext_SoundSet","offroad_Tires_grass_fast_Ext_SoundSet","offroad_Tires_gravel_slow_Ext_SoundSet","offroad_Tires_gravel_fast_Ext_SoundSet","offroad_Tires_gravel_dust_fast_Ext_SoundSet","offroad_Tires_asphalt_slow_Ext_SoundSet","offroad_Tires_asphalt_fast_Ext_SoundSet","offroad_Tires_water_slow_Ext_SoundSet","offroad_Tires_water_fast_Ext_SoundSet","Offroad_skid_dirt_SoundSet","offroad_dirt_turn_SoundSet","offroad_Rain_Ext_SoundSet","offroad_damper_left_SoundSet","offroad_damper_right_SoundSet","Candy_MustFrank_ext_engine_start_SoundSet"};
			soundSetsInt[] = {"offroad_Tires_Asphalt_Slow_General_Int_SoundSet","offroad_Tires_Asphalt_Fast_General_Int_SoundSet","offroad_Wind_SoundSet"};
		};
		class Crew: Crew
		{
			class Driver: Driver{};
			class CoDriver: CoDriver{};
			class Cargo1
			{
				actionSel = "seat_cargo1";
				proxyPos = "crewcargo1";
				getInPos = "pos_cargo1";
				getInDir = "pos_cargo1_dir";
			};
			class Cargo2
			{
				actionSel = "seat_cargo2";
				proxyPos = "crewcargo2";
				getInPos = "pos_cargo2";
				getInDir = "pos_cargo2_dir";
			};
		};
		class SimulationModule: SimulationModule
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
				reactionTime = 1;
				defaultThrust = 0.88999999;
				gentleThrust = 0.69999999;
				turboCoef = 5;
				gentleCoef = 0.94999999;
			};
			class Brake
			{
				pressureBySpeed[] = {0,0.5,10,0.40000001,20,0.30000001,40,0.28,60,0.30000001,80,0.38,100,0.41999999,120,0.47999999,150,0.60000002};
				reactionTime = 0.25;
				driverless = 0.1;
			};
			class Aerodynamics
			{
				frontalArea = 2.1500001;
				dragCoefficient = 0.15000001;
				downforceCoefficient = 0.80000001;
				downforceOffset[] = {0,0.4,-2.2};
			};
			drive = "DRIVE_AWD";
			class Engine
			{
				torqueCurve[] = {650,0,1360,290,3333,369,6036,424,8732,390,11020,0}; 
				inertia = 0.61000001;
				frictionTorque = 122;
				rollingFriction = 2.3;
				viscousFriction = 1.7;
				rpmIdle = 900;
				rpmMin = 1000;
				rpmClutch = 1352;
				rpmRedline = 12000;
			};
			class Clutch
			{
				maxTorqueTransfer=505;
				uncoupleTime=0.02;
				coupleTime=0.17;
			};
			class Gearbox
			{
				type = "GEARBOX_MANUAL";
				reverse=3.7260001;
				ratios[]={3.5,2.1440001,1.469,1.061,0.80400002};
			};
			class CentralDifferential
			{
				ratio = 0.75;
				type = "DIFFERENTIAL_LOCKED";
			};
			class Axles
			{
				class Front
				{
					maxBrakeTorque = 500;
					maxHandbrakeTorque = 0;
					wheelHubMass = 40;/* wheelHubMass = 20; */
					wheelHubRadius = 0.15;
					class Differential
					{
						ratio = 4.1;
						type = "DIFFERENTIAL_OPEN";
					};
					class Suspension
					{
						stiffness = 45100;
						compression = 2200;
						damping = 7600;
						travelMaxUp = 0.2;
						travelMaxDown = 0.2;
					};
					class Wheels
					{
						class Left
						{
							animTurn = "turnfrontleft";
							animRotation = "wheelfrontleft";
							wheelHub = "wheel_1_1_damper_land";
							animDamper = "damper_1_1";
							inventorySlot = "Candy_MustFrank_wheel_1_1";
						};
						class Right
						{
							animTurn = "turnfrontright";
							animRotation = "wheelfrontright";
							wheelHub = "wheel_2_1_damper_land";
							animDamper = "damper_2_1";
							inventorySlot = "Candy_MustFrank_wheel_2_1";
						};
					};
				};
				class Rear
				{
					maxBrakeTorque = 500;
					maxHandbrakeTorque = 4500;
					wheelHubMass = 40;/* wheelHubMass = 20; */
					wheelHubRadius = 0.15;
					class Differential
					{
						ratio = 4.1;
						type = "DIFFERENTIAL_OPEN";
					};
					class Suspension
					{
						stiffness = 45100;
						compression = 2200;
						damping = 7600;
						travelMaxUp = 0.2;
						travelMaxDown = 0.2;
					};
					class Wheels
					{
						class Left
						{
							animTurn = "turnbackleft";
							animRotation = "wheelbackleft";
							wheelHub = "wheel_1_2_damper_land";
							animDamper = "damper_1_2";
							inventorySlot = "Candy_MustFrank_wheel_1_2";
						};
						class Right
						{
							animTurn = "turnbacktright";
							animRotation = "wheelbackright";
							wheelHub = "wheel_2_2_damper_land";
							animDamper = "damper_2_2";
							inventorySlot = "Candy_MustFrank_wheel_2_2";
						};
					};
				};
			};
		};
		class Cargo
		{
			itemsCargoSize[] = {10,100};
			allowOwnedCargoManipulation = 1;
			openable = 0;
		};
		class ObstacleGenerator
		{
			carve = 1;
			timeToStationary = 5;
			moveThreshold = 0.5;
			class Shapes
			{
				class Cylindric
				{
					class Cyl1
					{
						radius = 1;
						height = 1.5;
						center[] = {0,0,0.7};
					};
					class Cyl3
					{
						radius = 1;
						height = 1.5;
						center[] = {0,0,-0.7};
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
				attachmentSlots[] = {"CarRadiator","CarBattery","SparkPlug"};
				icon = "cat_vehicle_engine";
			};
			class Body
			{
				name = "$STR_attachment_Body0";
				description = "";
				attachmentSlots[] = {"Reflector_1_1","Reflector_2_1","Candy_MustFrank_Doors_Driver","Candy_MustFrank_Doors_CoDriver","Candy_MustFrank_Doors_Trunk","Candy_MustFrank_Doors_Trunk0","Candy_MustFrank_Doors_Hood","Cassette"};
				icon = "cat_vehicle_body";
			};
			class Chassis
			{
				name = "$STR_attachment_Chassis0";
				description = "";
				attachmentSlots[] = {"Candy_MustFrank_Wheel_1_1","Candy_MustFrank_Wheel_1_2","Candy_MustFrank_Wheel_2_1","Candy_MustFrank_Wheel_2_2","Candy_MustFrank_Wheel_spare_1"};
				icon = "cat_vehicle_chassis";
			};
			class base
			{
				name = "base";
				description = "";
				icon = "cat_vehicle_chassis";
				attachmentSlots[] = {"truck_01_woodencrate1","back","CanisterGasoline"};
			};
		};
		class AnimationSources: AnimationSources
		{
			class DoorsDriver
			{
				source = "user";
				initPhase = 0;
				animPeriod = 0.5;
			};
			class DoorsCoDriver: DoorsDriver{};
			class DoorsTrunk: DoorsDriver{};
			class DoorsHood: DoorsDriver{};
			class DoorsCargo1: DoorsDriver{};
			class DoorsCargo2: DoorsDriver{};
		};
	};
	class Candy_MustFrank_White: Candy_MustFrank
	{
		scope = 2;
		displayname = "Mustang Frankenstein White";
		hiddenSelections[] = {"light_1_1","light_2_1","light_brake_1_2","light_brake_2_2","light_reverse_1_2","light_reverse_2_2","light_1_2","light_2_2","light_dashboard","light_offroad","light_offroad2","nomer","camo"};
		hiddenSelectionsTextures[] = {"candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\chrome.paa","candy_mustfrank\data\chrome.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\mustang_int_symboles.paa","candy_mustfrank\data\mustang_gauges.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\numer_mil_co.paa","candy_mustfrank\data\mustfrank_kuzov_co_White.paa"};
	};
	class Candy_MustFrank_NoGun: Candy_MustFrank
	{
		scope = 2;
		displayname = "Mustang Frankenstein NoGun";
		Model = "Candy_MustFrank\Candy_MustFrank_NoGun.p3d";
		hiddenSelections[] = {"light_1_1","light_2_1","light_brake_1_2","light_brake_2_2","light_reverse_1_2","light_reverse_2_2","light_1_2","light_2_2","light_dashboard","light_offroad","light_offroad2","nomer","camo"};
		hiddenSelectionsTextures[] = {"candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\chrome.paa","candy_mustfrank\data\chrome.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\mustang_int_symboles.paa","candy_mustfrank\data\mustang_gauges.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\numer_mil_co.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa"};
	};
	class Candy_MustFrank_NoGun_White: Candy_MustFrank
	{
		scope = 2;
		displayname = "Mustang Frankenstein NoGun White";
		Model = "Candy_MustFrank\Candy_MustFrank_NoGun.p3d";
		hiddenSelections[] = {"light_1_1","light_2_1","light_brake_1_2","light_brake_2_2","light_reverse_1_2","light_reverse_2_2","light_1_2","light_2_2","light_dashboard","light_offroad","light_offroad2","nomer","camo"};
		hiddenSelectionsTextures[] = {"candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\chrome.paa","candy_mustfrank\data\chrome.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\mustang_int_symboles.paa","candy_mustfrank\data\mustang_gauges.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\numer_mil_co.paa","candy_mustfrank\data\mustfrank_kuzov_co_White.paa"};
	};
	
	
	
	class Candy_MustFrank_NoGun_White_icecoffee: Candy_MustFrank
	{
		scope = 2;
		displayname = "Mustang Frankenstein NoGun White";
		Model = "Candy_MustFrank\Candy_MustFrank_NoGun.p3d";
		hiddenSelections[] = {"light_1_1","light_2_1","light_brake_1_2","light_brake_2_2","light_reverse_1_2","light_reverse_2_2","light_1_2","light_2_2","light_dashboard","light_offroad","light_offroad2","nomer","camo"};
		hiddenSelectionsTextures[] = {"candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\chrome.paa","candy_mustfrank\data\chrome.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\musatng_tail_emissive.paa","candy_mustfrank\data\mustang_int_symboles.paa","candy_mustfrank\data\mustang_gauges.paa","candy_mustfrank\data\mustfrank_kuzov_co.paa","candy_mustfrank\data\numer_mil_co.paa","candy_mustfrank\data\mustfrank_kuzov_co_White.paa"};
		attachments[] = {"CarRadiator","CarBattery","Reflector_1_1","Reflector_2_1","SparkPlug","Candy_MustFrank_Wheel_1_1","Candy_MustFrank_Wheel_1_2","Candy_MustFrank_Wheel_2_1","Candy_MustFrank_Wheel_2_2","Candy_MustFrank_Wheel_spare_1","Candy_MustFrank_Doors_Driver","Candy_MustFrank_Doors_CoDriver","Candy_MustFrank_Doors_Trunk","Candy_MustFrank_Doors_Trunk0","Candy_MustFrank_Doors_Hood","Truck_01_WoodenCrate2","truck_01_woodencrate1","back","CanisterGasoline","Shoulder", "Melee", "Body", "Legs", "Cassette"};
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name = "$STR_attachment_Engine0";
				description = "";
				attachmentSlots[] = {"CarRadiator","CarBattery","SparkPlug"};
				icon = "cat_vehicle_engine";
			};
			class Body
			{
				name = "$STR_attachment_Body0";
				description = "";
				attachmentSlots[] = {"Reflector_1_1","Reflector_2_1","Candy_MustFrank_Doors_Driver","Candy_MustFrank_Doors_CoDriver","Candy_MustFrank_Doors_Trunk","Candy_MustFrank_Doors_Trunk0","Candy_MustFrank_Doors_Hood","Cassette"};
				icon = "cat_vehicle_body";
			};
			class Chassis
			{
				name = "$STR_attachment_Chassis0";
				description = "";
				attachmentSlots[] = {"Candy_MustFrank_Wheel_1_1","Candy_MustFrank_Wheel_1_2","Candy_MustFrank_Wheel_2_1","Candy_MustFrank_Wheel_2_2","Candy_MustFrank_Wheel_spare_1"};
				icon = "cat_vehicle_chassis";
			};
			class base
			{
				name = "base";
				description = "";
				icon = "cat_vehicle_chassis";
				attachmentSlots[] = {"Shoulder", "Melee", "Body", "Legs", "back", "truck_01_woodencrate1", "Truck_01_WoodenCrate2", "CanisterGasoline"};
			};
		};	
	};	
	
	
	// следующее авто +1000 слотов доп и 2 оружия и одежда
	class Candy_MustFrank_konstanting: Candy_MustFrank
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку konstanting.";
		attachments[] = {"CarRadiator","CarBattery","Reflector_1_1","Reflector_2_1","SparkPlug","Candy_MustFrank_Wheel_1_1","Candy_MustFrank_Wheel_1_2","Candy_MustFrank_Wheel_2_1","Candy_MustFrank_Wheel_2_2","Candy_MustFrank_Wheel_spare_1","Candy_MustFrank_Doors_Driver","Candy_MustFrank_Doors_CoDriver","Candy_MustFrank_Doors_Trunk","Candy_MustFrank_Doors_Trunk0","Candy_MustFrank_Doors_Hood","truck_01_woodencrate1","Truck_01_WoodenCrate2","back","CanisterGasoline", "Head", "Body","Legs", "Vest", "Shoulder", "Melee", "Cassette"};
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name = "$STR_attachment_Engine0";
				description = "";
				attachmentSlots[] = {"CarRadiator","CarBattery","SparkPlug"};
				icon = "cat_vehicle_engine";
			};
			class Body
			{
				name = "$STR_attachment_Body0";
				description = "";
				attachmentSlots[] = {"Reflector_1_1","Reflector_2_1","Candy_MustFrank_Doors_Driver","Candy_MustFrank_Doors_CoDriver","Candy_MustFrank_Doors_Trunk","Candy_MustFrank_Doors_Trunk0","Candy_MustFrank_Doors_Hood","Cassette"};
				icon = "cat_vehicle_body";
			};
			class Chassis
			{
				name = "$STR_attachment_Chassis0";
				description = "";
				attachmentSlots[] = {"Candy_MustFrank_Wheel_1_1","Candy_MustFrank_Wheel_1_2","Candy_MustFrank_Wheel_2_1","Candy_MustFrank_Wheel_2_2","Candy_MustFrank_Wheel_spare_1"};
				icon = "cat_vehicle_chassis";
			};
			class base
			{
				name = "base";
				description = "";
				icon = "cat_vehicle_chassis";
				attachmentSlots[] = {"truck_01_woodencrate1","Truck_01_WoodenCrate2","Melee","Shoulder","Head", "Body","Legs","Vest", "back","CanisterGasoline"};
			};
		};
	};	
	class Candy_MustFrank_Doors_Driver_konstanting: Candy_MustFrank_Doors_Driver
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку konstanting.";
	};	
	class Candy_MustFrank_Doors_CoDriver_konstanting: Candy_MustFrank_Doors_CoDriver
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку konstanting.";
	};			
	class Candy_MustFrank_Doors_Trunk_konstanting: Candy_MustFrank_Doors_Trunk
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку konstanting.";
	};		
	class Candy_MustFrank_Doors_Trunk0_konstanting: Candy_MustFrank_Doors_Trunk0
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку konstanting.";
	};		
	class Candy_MustFrank_Doors_Hood_konstanting: Candy_MustFrank_Doors_Hood
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку konstanting.";
	};		
	
	
	
	
	class Candy_MustFrank_777VOR777: Candy_MustFrank
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку 777VOR777.";
	};	
	class Candy_MustFrank_Doors_Driver_777VOR777: Candy_MustFrank_Doors_Driver
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку 777VOR777.";
	};	
	class Candy_MustFrank_Doors_CoDriver_777VOR777: Candy_MustFrank_Doors_CoDriver
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку 777VOR777.";
	};			
	class Candy_MustFrank_Doors_Trunk_777VOR777: Candy_MustFrank_Doors_Trunk
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку 777VOR777.";
	};		
	class Candy_MustFrank_Doors_Trunk0_777VOR777: Candy_MustFrank_Doors_Trunk0
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку 777VOR777.";
	};		
	class Candy_MustFrank_Doors_Hood_777VOR777: Candy_MustFrank_Doors_Hood
	{
		displayname = "Mustang Frankenstein. Принадлежит игроку 777VOR777.";
	};		
	
	
	// следующее авто +1000 слотов и сундук
	class Candy_MustFrank_xVOLKOLAKx: Candy_MustFrank
	{
		displayname = "Mustang Frankenstein";
		attachments[] = {"CarRadiator","CarBattery","Reflector_1_1","Reflector_2_1","SparkPlug","Candy_MustFrank_Wheel_1_1","Candy_MustFrank_Wheel_1_2","Candy_MustFrank_Wheel_2_1","Candy_MustFrank_Wheel_2_2","Candy_MustFrank_Wheel_spare_1","Candy_MustFrank_Doors_Driver","Candy_MustFrank_Doors_CoDriver","Candy_MustFrank_Doors_Trunk","Candy_MustFrank_Doors_Trunk0","Candy_MustFrank_Doors_Hood","Truck_01_WoodenCrate1","back","CanisterGasoline","Cassette","Truck_01_WoodenCrate2","sea_chest", "Truck_01_WoodenCrate3","Truck_01_WoodenCrate4","Body","Legs"};
		
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name = "$STR_attachment_Engine0";
				description = "";
				attachmentSlots[] = {"CarRadiator","CarBattery","SparkPlug"};
				icon = "cat_vehicle_engine";
			};
			class Body
			{
				name = "$STR_attachment_Body0";
				description = "";
				attachmentSlots[] = {"Reflector_1_1","Reflector_2_1","Candy_MustFrank_Doors_Driver","Candy_MustFrank_Doors_CoDriver","Candy_MustFrank_Doors_Trunk","Candy_MustFrank_Doors_Trunk0","Candy_MustFrank_Doors_Hood","Cassette"};
				icon = "cat_vehicle_body";
			};
			class Chassis
			{
				name = "$STR_attachment_Chassis0";
				description = "";
				attachmentSlots[] = {"Candy_MustFrank_Wheel_1_1","Candy_MustFrank_Wheel_1_2","Candy_MustFrank_Wheel_2_1","Candy_MustFrank_Wheel_2_2","Candy_MustFrank_Wheel_spare_1"};
				icon = "cat_vehicle_chassis";
			};
			class base
			{
				name = "base";
				description = "";
				icon = "cat_vehicle_chassis";
				attachmentSlots[] = {"Truck_01_WoodenCrate1","back","CanisterGasoline","Truck_01_WoodenCrate2","sea_chest", "Truck_01_WoodenCrate3","Truck_01_WoodenCrate4","Body","Legs"};
			};
		};
		class AnimationSources: AnimationSources
		{
			class DoorsDriver
			{
				source = "user";
				initPhase = 0;
				animPeriod = 0.5;
			};
			class DoorsCoDriver: DoorsDriver{};
			class DoorsTrunk: DoorsDriver{};
			class DoorsHood: DoorsDriver{};
			class DoorsCargo1: DoorsDriver{};
			class DoorsCargo2: DoorsDriver{};
		};
	};
	
	
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyVehiclePart: ProxyAttachment
	{
		scope = 2;
		simulation = "ProxyInventory";
		autocenter = 0;
		animated = 0;
		shadow = 1;
		reversed = 0;
	};
	class ProxyCandy_MustFrank_Wheel: ProxyVehiclePart
	{
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Wheel.p3d";
		inventorySlot[] = {"Candy_MustFrank_Wheel_1_1","Candy_MustFrank_Wheel_1_2","Candy_MustFrank_Wheel_2_1","Candy_MustFrank_Wheel_2_2","Candy_MustFrank_Wheel_spare_1"};
	};
	class ProxyCandy_MustFrank_Wheel_Destruct: ProxyVehiclePart
	{
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Wheel_Destruct.p3d";
		inventorySlot[] = {"Candy_MustFrank_Wheel_1_1","Candy_MustFrank_Wheel_1_2","Candy_MustFrank_Wheel_2_1","Candy_MustFrank_Wheel_2_2","Candy_MustFrank_Wheel_spare_1"};
	};
	class ProxyCandy_MustFrank_Doors_CoDriver: ProxyVehiclePart
	{
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_CoDriver.p3d";
		inventorySlot[] = {"Candy_MustFrank_Doors_CoDriver"};
	};
	class ProxyCandy_MustFrank_Doors_Driver: ProxyVehiclePart
	{
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Driver.p3d";
		inventorySlot[] = {"Candy_MustFrank_Doors_Driver"};
	};
	class ProxyCandy_MustFrank_Doors_Trunk: ProxyVehiclePart
	{
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Trunk.p3d";
		inventorySlot[] = {"Candy_MustFrank_Doors_Trunk"};
	};
	class ProxyCandy_MustFrank_Doors_Trunk0: ProxyVehiclePart
	{
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Trunk0.p3d";
		inventorySlot[] = {"Candy_MustFrank_Doors_Trunk0"};
	};
	class ProxyCandy_MustFrank_Doors_Hood: ProxyVehiclePart
	{
		model = "Candy_MustFrank\proxy\Candy_MustFrank_Doors_Hood.p3d";
		inventorySlot[] = {"Candy_MustFrank_Doors_Hood"};
	};
	class ProxyPart
	{
		scope = 2;
		simulation = "ProxyInventory";
		model = "";
		inventorySlot = "";
		autocenter = 0;
		animated = 0;
		shadow = 1;
		reversed = 1;
	};
};
