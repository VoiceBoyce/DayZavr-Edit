class CfgPatches
{
	class AD_NFATACR_1_8
	{
		units[] = {"AD_NFATACR_1_8"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Optics"};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class ItemOptics_Base;
	class AD_NFATACR_1_8_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_NFATACR_1_80";
		descriptionShort = "$STR_AD_NFATACR_1_81";
		model = "AdvancedWeaponScopes\NF_ATACR_1_8\nfatacr1_8.p3d";
		animClass = "Binoculars";
		rotationFlags = 4;
		reversed = 0;
		weight = 700;
		itemSize[] = {2,1};
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
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\NF_ATACR_1_8\data\mount.rvmat","AdvancedWeaponScopes\NF_ATACR_1_8\data\atacr18.rvmat"}},{0.7,{"AdvancedWeaponScopes\NF_ATACR_1_8\data\mount.rvmat","AdvancedWeaponScopes\NF_ATACR_1_8\data\atacr18.rvmat"}},{0.5,{"AdvancedWeaponScopes\NF_ATACR_1_8\data\mount.rvmat","AdvancedWeaponScopes\NF_ATACR_1_8\data\atacr18.rvmat"}},{0.3,{"AdvancedWeaponScopes\NF_ATACR_1_8\data\mount_damage.rvmat","AdvancedWeaponScopes\NF_ATACR_1_8\data\atacr18_damage.rvmat"}},{0.0,{"AdvancedWeaponScopes\NF_ATACR_1_8\data\mount_destruct.rvmat","AdvancedWeaponScopes\NF_ATACR_1_8\data\atacr18_destruct.rvmat"}}};
				};
			};
		};
		class OpticsInfo
		{
			memoryPointCamera = "eyeScope";
			cameraDir = "cameraDir";
			useModelOptics = 1;
			modelOptics = "AdvancedWeaponScopes\NF_ATACR_1_8\nfatacr1_8_reticle.p3d";
			preloadOpticType = "Preload2DOptic_NFATACR_1_8";
			opticsDisablePeripherialVision = 0.67;
			opticsFlare = 1;
			opticsPPEffects[] = {"OpticsCHAbera3","OpticsBlur1"};
			opticsZoomMin = "0.3926/1";
			opticsZoomMax = "0.3926/8";
			opticsZoomInit = "0.3926/7";
			discretefov[] = {"0.3926/1","0.3926/2","0.3926/4","0.3926/6","0.3926/8"};
			discreteInitIndex = 0;
			distanceZoomMin = 100;
			distanceZoomMax = 600;
			discreteDistance[] = {100,200,300,400,500,600};
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
	class Preload2DOptic_NFATACR_1_8: Inventory_Base
	{
		scope = 0;
		model = "AdvancedWeaponScopes\NF_ATACR_1_8\nfatacr1_8_reticle.p3d";
	};
	class AD_NFATACR_1_8: AD_NFATACR_1_8_Base
	{
		scope = 2;
	};
};
