class CfgPatches
{
	class AD_RMR
	{
		units[] = {"AD_RMR","AD_RMR_LM","AD_RMR_TM"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Optics"};
	};
};
class cfgVehicles
{
	class ItemOptics_Base;
	class AD_RMR_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_RMR0";
		descriptionShort = "$STR_AD_RMR1";
		model = "\AdvancedWeaponScopes\RMR\rmr.p3d";
		attachments[] = {"BatteryD"};
		animClass = "Binoculars";
		simulation = "itemoptics";
		inventorySlot[] = {"pistolOptics","weaponOpticsCrossbow"};
		selectionFireAnim = "zasleh";
		memoryPointCamera = "eyeScope";
		cameraDir = "cameraDir";
		rotationFlags = 17;
		reversed = 0;
		weight = 100;
		itemSize[] = {1,1};
		dispersionModifier = -0.0005;
		dispersionCondition = "true";
		recoilModifier[] = {1,1,1};
		hiddenSelections[] = {"reddot"};
		hiddenSelectionsTextures[] = {""};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\RMR\data\RMR_Front_Glass_ca.paa","AdvancedWeaponScopes\RMR\data\RMR.rvmat"}},{0.7,{}},{0.5,{"DZ\weapons\attachments\optics\data\lensglass_damage_ca.paa","AdvancedWeaponScopes\RMR\data\RMR_damaged.rvmat"}},{0.3,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\RMR\data\RMR_damaged.rvmat"}},{0.0,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\RMR\data\RMR_destruct.rvmat"}}};
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
			opticsZoomMin = 0.5236;
			opticsZoomMax = 0.5236;
			opticsZoomInit = 0.5236;
			distanceZoomMin = 50;
			distanceZoomMax = 150;
			discreteDistance[] = {50,100,150};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.4,0.05};
			PPLensProperties[] = {1,0.15,0,0};
			PPBlurProperties = 0.2;
			PPDOFProperties[] = {1,0.2,20,185,4,10};
			opticSightTexture = "dz\weapons\attachments\optics\data\collimdot_red_ca.paa";
		};
		class EnergyManager
		{
			hasIcon = 1;
			energyUsagePerSecond = 0.02;
			plugType = 1;
			attachmentAction = 1;
		};
	};
	class AD_RMR: AD_RMR_Base
	{
		scope = 2;
	};
	class AD_RMR_LM_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_RMR3";
		descriptionShort = "$STR_AD_RMR1";
		model = "\AdvancedWeaponScopes\RMR\rmr_lowmount.p3d";
		attachments[] = {"BatteryD"};
		animClass = "Binoculars";
		simulation = "itemoptics";
		inventorySlot[] = {"weaponOptics","sniperOptics","weaponOpticsHunting"};
		selectionFireAnim = "zasleh";
		memoryPointCamera = "eyeScope";
		cameraDir = "cameraDir";
		rotationFlags = 17;
		reversed = 0;
		weight = 100;
		itemSize[] = {1,1};
		dispersionModifier = -0.009;
		dispersionCondition = "true";
		recoilModifier[] = {1,1,1};
		hiddenSelections[] = {"reddot"};
		hiddenSelectionsTextures[] = {""};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\RMR\data\RMR_Front_Glass_ca.paa","AdvancedWeaponScopes\RMR\data\RMR.rvmat","AdvancedWeaponScopes\RMR\data\Mount_MRD_low.rvmat"}},{0.7,{}},{0.5,{"DZ\weapons\attachments\optics\data\lensglass_damage_ca.paa","AdvancedWeaponScopes\RMR\data\RMR_damaged.rvmat","AdvancedWeaponScopes\RMR\data\Mount_MRD_low_damaged.rvmat"}},{0.3,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\RMR\data\RMR_damaged.rvmat","AdvancedWeaponScopes\RMR\data\Mount_MRD_low_damaged.rvmat"}},{0.0,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\RMR\data\RMR_destruct.rvmat","AdvancedWeaponScopes\RMR\data\Mount_MRD_low_destruct.rvmat"}}};
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
			opticsZoomMin = 0.5236;
			opticsZoomMax = 0.5236;
			opticsZoomInit = 0.5236;
			distanceZoomMin = 50;
			distanceZoomMax = 50;
			discreteDistance[] = {50};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.4,0.05};
			PPLensProperties[] = {1,0.15,0,0};
			PPBlurProperties = 0.2;
			PPDOFProperties[] = {1,0.2,20,185,4,10};
			opticSightTexture = "dz\weapons\attachments\optics\data\collimdot_red_ca.paa";
		};
		class EnergyManager
		{
			hasIcon = 1;
			energyUsagePerSecond = 0.02;
			plugType = 1;
			attachmentAction = 1;
		};
	};
	class AD_RMR_LM: AD_RMR_LM_Base
	{
		scope = 2;
	};
	class AD_RMR_TM_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_RMR4";
		descriptionShort = "$STR_AD_RMR1";
		model = "\AdvancedWeaponScopes\RMR\rmr_tallmount.p3d";
		attachments[] = {"BatteryD"};
		animClass = "Binoculars";
		simulation = "itemoptics";
		inventorySlot[] = {"weaponOptics","sniperOptics","weaponOpticsHunting"};
		selectionFireAnim = "zasleh";
		memoryPointCamera = "eyeScope";
		cameraDir = "cameraDir";
		rotationFlags = 17;
		reversed = 0;
		weight = 100;
		itemSize[] = {1,2};
		dispersionModifier = -0.009;
		dispersionCondition = "true";
		recoilModifier[] = {1,1,1};
		hiddenSelections[] = {"reddot"};
		hiddenSelectionsTextures[] = {""};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\RMR\data\RMR_Front_Glass_ca.paa","AdvancedWeaponScopes\RMR\data\RMR.rvmat","AdvancedWeaponScopes\RMR\data\Mount_MRD_tall.rvmat"}},{0.7,{}},{0.5,{"DZ\weapons\attachments\optics\data\lensglass_damage_ca.paa","AdvancedWeaponScopes\RMR\data\RMR_damaged.rvmat","AdvancedWeaponScopes\RMR\data\Mount_MRD_tall_damaged.rvmat"}},{0.3,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\RMR\data\RMR_damaged.rvmat","AdvancedWeaponScopes\RMR\data\Mount_MRD_tall_damaged.rvmat"}},{0.0,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\RMR\data\RMR_destruct.rvmat","AdvancedWeaponScopes\RMR\data\Mount_MRD_tall_destruct.rvmat"}}};
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
			opticsZoomMin = 0.5236;
			opticsZoomMax = 0.5236;
			opticsZoomInit = 0.5236;
			distanceZoomMin = 50;
			distanceZoomMax = 50;
			discreteDistance[] = {50};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.4,0.05};
			PPLensProperties[] = {1,0.15,0,0};
			PPBlurProperties = 0.2;
			PPDOFProperties[] = {1,0.2,20,185,4,10};
			opticSightTexture = "dz\weapons\attachments\optics\data\collimdot_red_ca.paa";
		};
		class EnergyManager
		{
			hasIcon = 1;
			energyUsagePerSecond = 0.02;
			plugType = 1;
			attachmentAction = 1;
		};
	};
	class AD_RMR_TM: AD_RMR_TM_Base
	{
		scope = 2;
	};
};
