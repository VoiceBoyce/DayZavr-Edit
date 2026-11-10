class CfgPatches
{
	class AD_DHF5
	{
		units[] = {"AD_DHF5"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Optics"};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class ItemOptics_Base;
	class AD_DHF5_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_DHF56";
		descriptionShort = "$STR_AD_DHF57";
		model = "\AdvancedWeaponScopes\DHF5\DHF5.p3d";
		animClass = "Binoculars";
		rotationFlags = 4;
		reversed = 0;
		weight = 830;
		itemSize[] = {5,1};
		inventorySlot[] = {"sniperOptics","weaponOpticsHunting","weaponOptics"};
		simulation = "itemoptics";
		dispersionModifier = -0.001;
		dispersionCondition = "true";
		recoilModifier[] = {1,1,1};
		memoryPointCamera = "eyeScope";
		cameraDir = "cameraDir";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\DHF5\data\scope.rvmat"}},{0.7,{"AdvancedWeaponScopes\DHF5\data\scope.rvmat"}},{0.5,{"AdvancedWeaponScopes\DHF5\data\scope.rvmat"}},{0.3,{"AdvancedWeaponScopes\DHF5\data\scope_damaged.rvmat"}},{0.0,{"AdvancedWeaponScopes\DHF5\data\scope_destruct.rvmat"}}};
				};
			};
		};
		class OpticsInfo
		{
			memoryPointCamera = "eyeScope";
			cameraDir = "cameraDir";
			useModelOptics = 1;
			modelOptics = "\AdvancedWeaponScopes\DHF5\DHF5_reticle.p3d";
			preloadOpticType = "Preload2DOptic_DHF5";
			opticsDisablePeripherialVision = 0.67;
			opticsFlare = 1;
			opticsPPEffects[] = {"OpticsCHAbera3","OpticsBlur1"};
			opticsZoomMin = "0.3926/5";
			opticsZoomMax = "0.3926/20";
			opticsZoomInit = "0.3926/5";
			discretefov[] = {"0.3926/5","0.3926/12","0.3926/20"};
			discreteInitIndex = 0;
			distanceZoomMin = 100;
			distanceZoomMax = 2000;
			discreteDistance[] = {100,200,300,400,500,600,700,800,900,1000,1100,1200,1300,1400,1500,1600,1700,1800,1900,2000};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.43,0.1};
			PPLensProperties[] = {0.3,0.15,0,0};
			PPBlurProperties = 0.3;
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
	class Preload2DOptic_DHF5: Inventory_Base
	{
		scope = 0;
		model = "\AdvancedWeaponScopes\DHF5\DHF5_reticle.p3d";
	};
	class AD_DHF5: AD_DHF5_Base
	{
		scope = 2;
	};
};
