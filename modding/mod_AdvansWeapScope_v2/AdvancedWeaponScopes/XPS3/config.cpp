class CfgPatches
{
	class AD_XPS34
	{
		units[] = {"AD_XPS34","AD_XPS34_HHS","AD_G33"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Weapons_Optics"};
	};
};
class cfgVehicles
{
	class ItemOptics_Base;
	class AD_XPS34_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_XPS340";
		descriptionShort = "$STR_AD_XPS341";
		model = "AdvancedWeaponScopes\XPS3\xps3.p3d";
		attachments[] = {"BatteryD"};
		animClass = "Binoculars";
		simulation = "itemoptics";
		inventorySlot[] = {"weaponOptics","weaponOpticsHunting","sniperOptics"};
		selectionFireAnim = "zasleh";
		memoryPointCamera = "eyeScope";
		cameraDir = "cameraDir";
		rotationFlags = 17;
		reversed = 0;
		weight = 150;
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
					healthLevels[] = {{1.0,{"DZ\weapons\attachments\optics\data\lensglass_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3.rvmat"}},{0.7,{}},{0.5,{"DZ\weapons\attachments\optics\data\lensglass_damage_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3_damage.rvmat"}},{0.3,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3_destruct.rvmat"}},{0.0,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3_destruct.rvmat"}}};
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
			opticSightTexture = "AdvancedWeaponScopes\XPS3\data\xps34_reticle_ca.paa";
			opticsZoomMin = 0.5236;
			opticsZoomMax = 0.5236;
			opticsZoomInit = 0.5236;
			distanceZoomMin = 50;
			distanceZoomMax = 500;
			discreteDistance[] = {50,100,150,200,300,400,500};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.4,0.05};
			PPLensProperties[] = {1,0.15,0,0};
			PPBlurProperties = 0.2;
		};
		class EnergyManager
		{
			hasIcon = 1;
			energyUsagePerSecond = 0.01;
			plugType = 1;
			attachmentAction = 1;
		};
	};
	class AD_XPS34: AD_XPS34_Base
	{
		scope = 2;
	};
	class AD_XPS34_HHS_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_XPS34HHS0";
		descriptionShort = "$STR_AD_XPS34HHS1";
		model = "AdvancedWeaponScopes\XPS3\xps3_g33.p3d";
		attachments[] = {"BatteryD"};
		animClass = "Binoculars";
		simulation = "itemoptics";
		inventorySlot[] = {"weaponOptics","weaponOpticsHunting","sniperOptics"};
		selectionFireAnim = "zasleh";
		memoryPointCamera = "eyeScope";
		cameraDir = "cameraDir";
		rotationFlags = 17;
		reversed = 0;
		weight = 250;
		itemSize[] = {2,1};
		dispersionModifier = -0.0005;
		dispersionCondition = "true";
		recoilModifier[] = {1,1,1};
		hiddenSelections[] = {"reddot"};
		hiddenSelectionsTextures[] = {""};
		simpleHiddenSelections[] = {"hide"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"DZ\weapons\attachments\optics\data\lensglass_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3.rvmat"}},{0.7,{}},{0.5,{"DZ\weapons\attachments\optics\data\lensglass_damage_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3_damage.rvmat"}},{0.3,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3_destruct.rvmat"}},{0.0,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3_destruct.rvmat"}}};
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
			opticSightTexture = "AdvancedWeaponScopes\XPS3\data\xps34_reticle_ca.paa";
			opticsZoomMin = "0.3926/2";
			opticsZoomMax = "0.3926/2";
			opticsZoomInit = "0.3926/2";
			distanceZoomMin = 50;
			distanceZoomMax = 500;
			discreteDistance[] = {50,100,150,200,300,400,500};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.55,0.18,0.15};
			PPLensProperties[] = {1.0,0.15,0,0};
			PPBlurProperties = 0.4;
		};
		class EnergyManager
		{
			hasIcon = 1;
			energyUsagePerSecond = 0.01;
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
	class AD_XPS34_HHS: AD_XPS34_HHS_Base
	{
		scope = 2;
	};
	class AD_G33_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_G330";
		descriptionShort = "$STR_AD_G331";
		model = "AdvancedWeaponScopes\XPS3\g33.p3d";
		animClass = "Binoculars";
		simulation = "itemoptics";
		inventorySlot[] = {"weaponOptics","weaponOpticsHunting","sniperOptics"};
		selectionFireAnim = "zasleh";
		memoryPointCamera = "eyeScope";
		cameraDir = "cameraDir";
		rotationFlags = 17;
		reversed = 0;
		weight = 100;
		itemSize[] = {1,1};
		simpleHiddenSelections[] = {"hide"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1.0,{"DZ\weapons\attachments\optics\data\lensglass_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3.rvmat"}},{0.7,{}},{0.5,{"DZ\weapons\attachments\optics\data\lensglass_damage_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3_damage.rvmat"}},{0.3,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3_destruct.rvmat"}},{0.0,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\XPS3\data\xps3_destruct.rvmat"}}};
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
			opticsZoomMin = "0.3926/2";
			opticsZoomMax = "0.3926/2";
			opticsZoomInit = "0.3926/2";
			distanceZoomMin = 300;
			distanceZoomMax = 300;
			discreteDistance[] = {300};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.6,0.18,0.15};
			PPLensProperties[] = {1.0,0.1,0,0};
			PPBlurProperties = 0.4;
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
	class AD_G33: AD_G33_Base
	{
		scope = 2;
	};
};
