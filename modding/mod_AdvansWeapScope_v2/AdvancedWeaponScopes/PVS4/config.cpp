class CfgPatches
{
	class AD_PVS4
	{
		units[] = {"AD_PVS4"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Optics"};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class ItemOptics_Base;
	class AD_PVS4_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_PVS46";
		descriptionShort = "$STR_AD_PVS47";
		model = "\AdvancedWeaponScopes\PVS4\pvs4.p3d";
		attachments[] = {"BatteryD"};
		animClass = "Binoculars";
		rotationFlags = 16;
		reversed = 0;
		weight = 1800;
		itemSize[] = {4,3};
		inventorySlot[] = {"weaponOpticsHunting","weaponOptics","sniperOptics"};
		simulation = "itemoptics";
		dispersionModifier = -0.001;
		dispersionCondition = "true";
		recoilModifier[] = {1,1,1};
		memoryPointCamera = "eyeScope";
		cameraDir = "cameraDir";
		NVOptic = 1;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\PVS4\data\metal_body.rvmat"}},{0.7,{"AdvancedWeaponScopes\PVS4\data\metal_body.rvmat"}},{0.5,{"AdvancedWeaponScopes\PVS4\data\metal_body.rvmat"}},{0.3,{"AdvancedWeaponScopes\PVS4\data\PVS4_damage.rvmat"}},{0.0,{"AdvancedWeaponScopes\PVS4\data\PVS4_destruct.rvmat"}}};
				};
			};
		};
		class OpticsInfo
		{
			memoryPointCamera = "eyeScope";
			cameraDir = "cameraDir";
			useModelOptics = 1;
			modelOptics = "\AdvancedWeaponScopes\PVS4\pvs4_opticview.p3d";
			preloadOpticType = "Preload2DOptic_PVS4";
			opticsDisablePeripherialVision = 0.67;
			opticsFlare = 1;
			opticsPPEffects[] = {"OpticsCHAbera3","OpticsBlur1"};
			opticsZoomMin = "0.3926/3.6";
			opticsZoomMax = "0.3926/3.6";
			opticsZoomInit = "0.3926/3.6";
			distanceZoomMin = 100;
			distanceZoomMax = 600;
			discreteDistance[] = {100,200,300,400,500,600};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.5,0.06};
			PPLensProperties[] = {-0.9,1.0,0,0};
			PPBlurProperties = 0.75;
		};
		class AnimationSources
		{
			class hide
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 0;
			};
		};
		class EnergyManager
		{
			hasIcon = 1;
			energyUsagePerSecond = 0.05;
			plugType = 1;
			attachmentAction = 1;
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickup
				{
					soundSet = "PSO11Optic_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "PSO11Optic_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class Preload2DOptic_PVS4: Inventory_Base
	{
		scope = 0;
		model = "\AdvancedWeaponScopes\PVS4\pvs4_opticview.p3d";
	};
	class AD_PVS4: AD_PVS4_Base
	{
		scope = 2;
	};
};
