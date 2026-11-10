class CfgPatches
{
	class AD_Pilad
	{
		units[] = {"AD_Pilad"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Optics"};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class ItemOptics_Base;
	class AD_Pilad_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_Pilad6";
		descriptionShort = "$STR_AD_Pilad7";
		model = "\AdvancedWeaponScopes\Pilad\pilad.p3d";
		animClass = "Binoculars";
		rotationFlags = 4;
		reversed = 0;
		weight = 900;
		itemSize[] = {4,1};
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
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\Pilad\data\pilad.rvmat"}},{0.7,{"AdvancedWeaponScopes\Pilad\data\pilad.rvmat"}},{0.5,{"AdvancedWeaponScopes\Pilad\data\pilad.rvmat"}},{0.3,{"AdvancedWeaponScopes\Pilad\data\pilad_damaged.rvmat"}},{0.0,{"AdvancedWeaponScopes\Pilad\data\pilad_destruct.rvmat"}}};
				};
			};
		};
		class OpticsInfo
		{
			memoryPointCamera = "eyeScope";
			cameraDir = "cameraDir";
			useModelOptics = 1;
			modelOptics = "AdvancedWeaponScopes\Pilad\opticview_pilad.p3d";
			preloadOpticType = "Preload2DOptic_Pilad";
			opticsDisablePeripherialVision = 0.67;
			opticsFlare = 1;
			opticsPPEffects[] = {"OpticsCHAbera3","OpticsBlur1"};
			opticsZoomMin = "0.3926/4";
			opticsZoomMax = "0.3926/4";
			opticsZoomInit = "0.3926/4";
			discretefov[] = {"0.3926/4"};
			discreteInitIndex = 0;
			distanceZoomMin = 100;
			distanceZoomMax = 800;
			discreteDistance[] = {100,200,300,400,500,600,700,800};
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
	class Preload2DOptic_Pilad: Inventory_Base
	{
		scope = 0;
		model = "AdvancedWeaponScopes\Pilad\opticview_pilad.p3d";
	};
	class AD_Pilad: AD_Pilad_Base
	{
		scope = 2;
	};
};
