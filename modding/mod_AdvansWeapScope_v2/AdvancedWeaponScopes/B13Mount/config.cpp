class CfgPatches
{
	class AD_B13Mount
	{
		units[] = {"AD_B13Mount","AD_Pilad_B13","AD_LeupoldMk4_B13","AD_DHF5_B13","AD_PVS4_B13","AD_OKP7_B13","AD_RMR_LM_B13","AD_RMR_TM_B13","AD_ACOG_B13","AD_ACOG_RMR_B13","AD_MRS_B13","AD_XPS34_B13","AD_XPS34_HHS_B13","AD_G33_B13","AD_NFATACR_B13","AD_NFATACR_1_8_B13","AD_SpecterDR_B13","AD_NFATACR_B13_RMR"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","AD_Pilad","AD_DHF5","AD_PVS4","DZ_Weapons_Optics","AD_OKP7","AD_RMR","AD_ACOG","AD_MRS","AD_NFATACR","AD_NFATACR_1_8","AD_SpecterDR","AD_LeupoldMk4","AD_XPS34"};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class AD_B13Mount: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_AD_B130";
		descriptionShort = "$STR_AD_B131";
		model = "AdvancedWeaponScopes\B13Mount\b13mount.p3d";
		rotationFlags = 16;
		reversed = 0;
		weight = 175;
		itemSize[] = {2,1};
		inventorySlot[] = {"weaponOpticsAK"};
		repairableWithKits[] = {1};
		repairCosts[] = {25.0};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\B13Mount\data\b13.rvmat"}},{0.7,{"AdvancedWeaponScopes\B13Mount\data\b13.rvmat"}},{0.5,{"AdvancedWeaponScopes\B13Mount\data\b13.rvmat"}},{0.3,{"AdvancedWeaponScopes\B13Mount\data\b13_damaged.rvmat"}},{0.0,{"AdvancedWeaponScopes\B13Mount\data\b13_destruct.rvmat"}}};
				};
			};
		};
	};
	class AD_Pilad_Base;
	class AD_Pilad_B13: AD_Pilad_Base
	{
		scope = 2;
		displayName = "$STR_AD_Pilad4";
		descriptionShort = "$STR_AD_Pilad5";
		model = "\AdvancedWeaponScopes\B13Mount\piladB13.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 1175;
		itemSize[] = {4,2};
	};
	class AD_LeupoldMk4_Base;
	class AD_LeupoldMk4_B13: AD_LeupoldMk4_Base
	{
		scope = 2;
		displayName = "$STR_AD_LeupoldMk4_2";
		descriptionShort = "$STR_AD_LeupoldMk4_3";
		model = "\AdvancedWeaponScopes\B13Mount\b13_LeupoldMk4.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 875;
		itemSize[] = {4,2};
	};
	class AD_DHF5_Base;
	class AD_DHF5_B13: AD_DHF5_Base
	{
		scope = 2;
		displayName = "$STR_AD_DHF54";
		descriptionShort = "$STR_AD_DHF55";
		model = "\AdvancedWeaponScopes\B13Mount\dhf5B13.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 1105;
		itemSize[] = {5,2};
	};
	class AD_PVS4_Base;
	class AD_PVS4_B13: AD_PVS4_Base
	{
		scope = 2;
		displayName = "$STR_AD_PVS44";
		descriptionShort = "$STR_AD_PVS45";
		model = "\AdvancedWeaponScopes\B13Mount\pvs4B13.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 1975;
		itemSize[] = {4,4};
	};
	class AD_OKP7_Base;
	class AD_OKP7_B13: AD_OKP7_Base
	{
		scope = 2;
		displayName = "$STR_AD_OKP74";
		descriptionShort = "$STR_AD_OKP75";
		model = "\AdvancedWeaponScopes\B13Mount\okp7B13.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 450;
		itemSize[] = {2,2};
	};
	class AD_RMR_LM_Base;
	class AD_RMR_LM_B13: AD_RMR_LM_Base
	{
		scope = 2;
		displayName = "$STR_AD_RMR5";
		descriptionShort = "$STR_AD_RMR1";
		model = "AdvancedWeaponScopes\B13Mount\b13_RMRLM.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 400;
		itemSize[] = {2,2};
	};
	class AD_RMR_TM_Base;
	class AD_RMR_TM_B13: AD_RMR_TM_Base
	{
		scope = 2;
		displayName = "$STR_AD_RMR6";
		descriptionShort = "$STR_AD_RMR1";
		model = "AdvancedWeaponScopes\B13Mount\b13_RMRTM.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 420;
		itemSize[] = {2,2};
	};
	class AD_ACOG_Base;
	class AD_ACOG_B13: AD_ACOG_Base
	{
		scope = 2;
		displayName = "$STR_AD_ACOG0";
		descriptionShort = "$STR_AD_ACOG2";
		model = "AdvancedWeaponScopes\B13Mount\b13_acog.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 800;
		itemSize[] = {2,2};
	};
	class AD_ACOG_RMR_Base;
	class AD_ACOG_RMR_B13: AD_ACOG_RMR_Base
	{
		scope = 2;
		displayName = "$STR_AD_ACOG4";
		descriptionShort = "$STR_AD_ACOG2";
		model = "AdvancedWeaponScopes\B13Mount\b13_acogRMR.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 850;
		itemSize[] = {2,2};
	};
	class AD_MRS_Base;
	class AD_MRS_B13: AD_MRS_Base
	{
		scope = 2;
		displayName = "$STR_AD_MRS4";
		descriptionShort = "$STR_AD_MRS5";
		model = "AdvancedWeaponScopes\B13Mount\mrsB13.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 450;
		itemSize[] = {2,2};
	};
	class AD_XPS34_Base;
	class AD_XPS34_B13: AD_XPS34_Base
	{
		scope = 2;
		displayName = "$STR_AD_XPS342";
		descriptionShort = "$STR_AD_XPS343";
		model = "AdvancedWeaponScopes\B13Mount\b13_xps34.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 325;
		itemSize[] = {2,2};
	};
	class AD_XPS34_HHS_Base;
	class AD_XPS34_HHS_B13: AD_XPS34_HHS_Base
	{
		scope = 2;
		displayName = "$STR_AD_XPS34HHS2";
		descriptionShort = "$STR_AD_XPS34HHS3";
		model = "AdvancedWeaponScopes\B13Mount\b13_xps34_g33.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 425;
		itemSize[] = {2,2};
	};
	class AD_G33_Base;
	class AD_G33_B13: AD_G33_Base
	{
		scope = 2;
		displayName = "$STR_AD_G332";
		descriptionShort = "$STR_AD_G333";
		model = "AdvancedWeaponScopes\B13Mount\b13_g33.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 275;
		itemSize[] = {2,2};
	};
	class AD_NFATACR_Base;
	class AD_NFATACR_B13: AD_NFATACR_Base
	{
		scope = 2;
		displayName = "$STR_AD_NFATACR3";
		descriptionShort = "$STR_AD_NFATACR1";
		model = "AdvancedWeaponScopes\B13Mount\b13_NFATACR.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 1400;
		itemSize[] = {5,2};
	};
	class AD_NFATACR_1_8_Base;
	class AD_NFATACR_1_8_B13: AD_NFATACR_1_8_Base
	{
		scope = 2;
		displayName = "$STR_AD_NFATACR_1_83";
		descriptionShort = "$STR_AD_NFATACR1";
		model = "AdvancedWeaponScopes\B13Mount\b13_NFATACR_1_8.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 800;
		itemSize[] = {2,2};
	};
	class AD_SpecterDR_Base;
	class AD_SpecterDR_B13: AD_SpecterDR_Base
	{
		scope = 2;
		displayName = "$STR_AD_SpecterDR4";
		descriptionShort = "$STR_AD_SpecterDR5";
		model = "AdvancedWeaponScopes\B13Mount\b13_specterdr.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 600;
		itemSize[] = {2,2};
	};
	class AD_NFATACR_B13_RMR: AD_NFATACR_Base
	{
		scope = 2;
		displayName = "$STR_AD_NFATACR3";
		descriptionShort = "$STR_AD_NFATACR1";
		model = "AdvancedWeaponScopes\B13Mount\b13_NFATACR_rmr.p3d";
		inventorySlot[] = {"weaponOpticsAK"};
		weight = 1500;
		itemSize[] = {5,2};
		hiddenSelections[] = {"reddot"};
		hiddenSelectionsTextures[] = {""};
		attachments[] = {"BatteryD"};
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
			distanceZoomMin = 300;
			distanceZoomMax = 2000;
			discreteDistance[] = {300,400,500,600,700,800,900,1000,1100,1200,1300,1400,1500,1600,1700,1800,1900,2000};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.43,0.1};
			PPLensProperties[] = {0.3,0.15,0,0};
			PPBlurProperties = 0.3;
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
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\NF_ATACR\data\base.rvmat","AdvancedWeaponScopes\NF_ATACR\data\scope.rvmat","AdvancedWeaponScopes\RMR\data\offsetmount.rvmat"}},{0.7,{"AdvancedWeaponScopes\NF_ATACR\data\base.rvmat","AdvancedWeaponScopes\NF_ATACR\data\scope.rvmat","AdvancedWeaponScopes\RMR\data\offsetmount.rvmat"}},{0.5,{"AdvancedWeaponScopes\NF_ATACR\data\base_damage.rvmat","AdvancedWeaponScopes\NF_ATACR\data\scope_damage.rvmat","AdvancedWeaponScopes\RMR\data\offsetmount_damage.rvmat"}},{0.3,{"AdvancedWeaponScopes\NF_ATACR\data\base_damage.rvmat","AdvancedWeaponScopes\NF_ATACR\data\scope_damage.rvmat","AdvancedWeaponScopes\RMR\data\offsetmount_damage.rvmat"}},{0.0,{"AdvancedWeaponScopes\NF_ATACR\data\base_destruct.rvmat","AdvancedWeaponScopes\NF_ATACR\data\scope_destruct.rvmat","AdvancedWeaponScopes\RMR\data\offsetmount_destruct.rvmat"}}};
				};
			};
		};
	};
};
