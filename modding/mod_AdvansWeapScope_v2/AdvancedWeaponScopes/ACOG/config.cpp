class CfgPatches
{
	class AD_ACOG
	{
		units[] = {"AD_ACOG","AD_ACOG_RMR"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Optics"};
	};
};
class cfgVehicles
{
	class ItemOptics_Base;
	class AD_ACOG_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_ACOG0";
		descriptionShort = "$STR_AD_ACOG1";
		model = "AdvancedWeaponScopes\ACOG\acog.p3d";
		animClass = "Binoculars";
		simulation = "itemoptics";
		inventorySlot[] = {"weaponOptics","sniperOptics","weaponOpticsHunting"};
		selectionFireAnim = "zasleh";
		rotationFlags = 16;
		reversed = 0;
		weight = 440;
		itemSize[] = {2,1};
		dispersionModifier = -0.001;
		dispersionCondition = "true";
		recoilModifier[] = {1,1,1};
		hiddenSelectionsTextures[] = {""};
		simpleHiddenSelections[] = {"hide","mount"};
		memoryPointCamera = "eyeScope";
		cameraDir = "cameraDir";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\ACOG\data\opticview_acog_ca.paa","DZ\weapons\attachments\optics\data\lensglass_standard.rvmat","AdvancedWeaponScopes\ACOG\data\ACOGFO.rvmat","AdvancedWeaponScopes\ACOG\data\ACOG_FiberOptic.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_ACOG.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_MRD_ACOG_Rear_damaged.rvmat"}},{0.7,{}},{0.5,{"AdvancedWeaponScopes\ACOG\data\opticview_acog_damaged_ca.paa","DZ\weapons\attachments\optics\data\lensglass_standard_damage.rvmat","AdvancedWeaponScopes\ACOG\data\ACOGFO_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\ACOG_FiberOptic_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_ACOG_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_MRD_ACOG_Rear_damaged.rvmat"}},{0.3,{"AdvancedWeaponScopes\ACOG\data\opticview_acog_destruct_ca.paa","DZ\weapons\attachments\optics\data\lensglass_standard_destruct.rvmat","AdvancedWeaponScopes\ACOG\data\ACOGFO_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\ACOG_FiberOptic_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_ACOG_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_MRD_ACOG_Rear_damaged.rvmat"}},{0.0,{"AdvancedWeaponScopes\ACOG\data\opticview_acog_destruct_ca.paa","DZ\weapons\attachments\optics\data\lensglass_standard_destruct.rvmat","AdvancedWeaponScopes\ACOG\data\ACOGFO_destruct.rvmat","AdvancedWeaponScopes\ACOG\data\ACOG_FiberOptic_destruct.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_ACOG_destruct.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_MRD_ACOG_Rear_destruct.rvmat"}}};
				};
			};
		};
		class OpticsInfo
		{
			memoryPointCamera = "eyeScope";
			cameraDir = "cameraDir";
			modelOptics = "-";
			opticsDisablePeripherialVision = 0.67;
			opticsFlare = 1;
			opticsPPEffects[] = {};
			opticsZoomMin = "0.3926/4";
			opticsZoomMax = "0.3926/4";
			opticsZoomInit = "0.3926/4";
			distanceZoomMin = 100;
			distanceZoomMax = 600;
			discreteDistance[] = {100,200,300,400,500,600};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.375,0.05};
			PPLensProperties[] = {0.5,0.15,0,0};
			PPBlurProperties = 0.6;
		};
		class OpticsInfoWeaponOverride
		{
			memoryPointCamera = "eyeIronsights";
			cameraDir = "cameraDirIronsights";
			opticsZoomMin = 0.5236;
			opticsZoomMax = 0.5236;
			opticsZoomInit = 0.5236;
			distanceZoomMin = 200;
			distanceZoomMax = 200;
			discreteDistance[] = {200};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.375,0.05};
			PPLensProperties[] = {0.5,0.15,0,0};
			PPBlurProperties = 0.6;
			PPDOFProperties[] = {1,0.1,20,200,4,10};
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
	class AD_ACOG: AD_ACOG_Base
	{
		scope = 2;
	};
	class AD_ACOG_RMR_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_ACOG3";
		descriptionShort = "$STR_AD_ACOG1";
		model = "AdvancedWeaponScopes\ACOG\acog_rmr.p3d";
		animClass = "Binoculars";
		simulation = "itemoptics";
		inventorySlot[] = {"weaponOptics","sniperOptics","weaponOpticsHunting"};
		selectionFireAnim = "zasleh";
		rotationFlags = 16;
		reversed = 0;
		weight = 440;
		itemSize[] = {2,1};
		dispersionModifier = -0.000125;
		dispersionCondition = "true";
		recoilModifier[] = {1,1,1};
		hiddenSelections[] = {"reddot"};
		hiddenSelectionsTextures[] = {""};
		simpleHiddenSelections[] = {"hide","mount"};
		attachments[] = {"BatteryD"};
		memoryPointCamera = "eyeScope";
		cameraDir = "cameraDir";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\ACOG\data\opticview_acog_ca.paa","DZ\weapons\attachments\optics\data\lensglass_standard.rvmat","AdvancedWeaponScopes\ACOG\data\ACOGFO.rvmat","AdvancedWeaponScopes\ACOG\data\ACOG_FiberOptic.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_ACOG.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_MRD_ACOG_Rear_damaged.rvmat"}},{0.7,{}},{0.5,{"AdvancedWeaponScopes\ACOG\data\opticview_acog_damaged_ca.paa","DZ\weapons\attachments\optics\data\lensglass_standard_damage.rvmat","AdvancedWeaponScopes\ACOG\data\ACOGFO_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\ACOG_FiberOptic_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_ACOG_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_MRD_ACOG_Rear_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\RMR_damaged.rvmat"}},{0.3,{"AdvancedWeaponScopes\ACOG\data\opticview_acog_destruct_ca.paa","DZ\weapons\attachments\optics\data\lensglass_standard_destruct.rvmat","AdvancedWeaponScopes\ACOG\data\ACOGFO_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\ACOG_FiberOptic_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_ACOG_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_MRD_ACOG_Rear_damaged.rvmat","AdvancedWeaponScopes\ACOG\data\RMR_damaged.rvmat"}},{0.0,{"AdvancedWeaponScopes\ACOG\data\opticview_acog_destruct_ca.paa","DZ\weapons\attachments\optics\data\lensglass_standard_destruct.rvmat","AdvancedWeaponScopes\ACOG\data\ACOGFO_destruct.rvmat","AdvancedWeaponScopes\ACOG\data\ACOG_FiberOptic_destruct.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_ACOG_destruct.rvmat","AdvancedWeaponScopes\ACOG\data\Mount_MRD_ACOG_Rear_destruct.rvmat","AdvancedWeaponScopes\ACOG\data\RMR_destruct.rvmat"}}};
				};
			};
		};
		class OpticsInfo
		{
			memoryPointCamera = "eyeScope";
			cameraDir = "cameraDir";
			modelOptics = "-";
			opticsDisablePeripherialVision = 0.67;
			opticsFlare = 1;
			opticsPPEffects[] = {};
			opticsZoomMin = "0.3926/4";
			opticsZoomMax = "0.3926/4";
			opticsZoomInit = "0.3926/4";
			distanceZoomMin = 100;
			distanceZoomMax = 600;
			discreteDistance[] = {100,200,300,400,500,600};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.375,0.05};
			PPLensProperties[] = {0.5,0.15,0,0};
			PPBlurProperties = 0.6;
			opticSightTexture = "dz\weapons\attachments\optics\data\collimdot_red_ca.paa";
		};
		class OpticsInfoWeaponOverride
		{
			memoryPointCamera = "eyeIronsights";
			cameraDir = "cameraDirIronsights";
			opticsZoomMin = 0.5236;
			opticsZoomMax = 0.5236;
			opticsZoomInit = 0.5236;
			distanceZoomMin = 50;
			distanceZoomMax = 150;
			discreteDistance[] = {50,150};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.375,0.05};
			PPLensProperties[] = {0.5,0.15,0,0};
			PPBlurProperties = 0.6;
			PPDOFProperties[] = {1,0.1,20,200,4,10};
		};
		class EnergyManager
		{
			hasIcon = 1;
			energyUsagePerSecond = 0.02;
			plugType = 1;
			attachmentAction = 1;
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
	class AD_ACOG_RMR: AD_ACOG_RMR_Base
	{
		scope = 2;
	};
};
