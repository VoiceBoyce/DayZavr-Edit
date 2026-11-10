class CfgPatches
{
	class AD_NFATACR
	{
		units[] = {"AD_NFATACR"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Optics"};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class ItemOptics_Base;
	class AD_NFATACR_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_NFATACR0";
		descriptionShort = "$STR_AD_NFATACR1";
		model = "AdvancedWeaponScopes\NF_ATACR\nfatacr.p3d";
		animClass = "Binoculars";
		rotationFlags = 4;
		reversed = 0;
		weight = 1113;
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
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\NF_ATACR\data\base.rvmat","AdvancedWeaponScopes\NF_ATACR\data\scope.rvmat"}},{0.7,{"AdvancedWeaponScopes\NF_ATACR\data\base.rvmat","AdvancedWeaponScopes\NF_ATACR\data\scope.rvmat"}},{0.5,{"AdvancedWeaponScopes\NF_ATACR\data\base.rvmat","AdvancedWeaponScopes\NF_ATACR\data\scope.rvmat"}},{0.3,{"AdvancedWeaponScopes\NF_ATACR\data\base_damage.rvmat","AdvancedWeaponScopes\NF_ATACR\data\scope_damage.rvmat"}},{0.0,{"AdvancedWeaponScopes\NF_ATACR\data\base_destruct.rvmat","AdvancedWeaponScopes\NF_ATACR\data\scope_destruct.rvmat"}}};
				};
			};
		};
		class OpticsInfo
		{
			memoryPointCamera = "eyeScope";
			cameraDir = "cameraDir";
			useModelOptics = 1;
			modelOptics = "AdvancedWeaponScopes\NF_ATACR\NFATACR_reticle.p3d";
			preloadOpticType = "Preload2DOptic_NFATACR";
			opticsDisablePeripherialVision = 0.67;
			opticsFlare = 1;
			opticsPPEffects[] = {"OpticsCHAbera3","OpticsBlur1"};
			opticsZoomMin = "0.3926/7";
			opticsZoomMax = "0.3926/35";
			opticsZoomInit = "0.3926/7";
			discretefov[] = {"0.3926/7","0.3926/12","0.3926/22","0.3926/35"};
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
	};
	class Preload2DOptic_NFATACR: Inventory_Base
	{
		scope = 0;
		model = "AdvancedWeaponScopes\NF_ATACR\NFATACR_reticle.p3d";
	};
	class AD_NFATACR: AD_NFATACR_Base
	{
		scope = 2;
	};
};
