class CfgPatches
{
	class AD_LeupoldMk4
	{
		units[] = {"AD_LeupoldMk4"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class cfgVehicles
{
	class ItemOptics_Base;
	class Inventory_Base;
	class AD_LeupoldMk4_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_LeupoldMk4_0";
		descriptionShort = "$STR_AD_LeupoldMk4_1";
		model = "AdvancedWeaponScopes\LeupoldMk4\leupoldmk4.p3d";
		animClass = "Binoculars";
		rotationFlags = 4;
		reversed = 0;
		weight = 700;
		itemSize[] = {4,1};
		inventorySlot[] = {"sniperOptics","weaponOpticsHunting","weaponOptics"};
		simulation = "itemoptics";
		dispersionModifier = -0.001;
		dispersionCondition = "true";
		recoilModifier[] = {1,1,1};
		swayModifier[] = {1,1,1};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\LeupoldMk4\data\scope.rvmat","DZ\weapons\attachments\data\terra.rvmat"}},{0.7,{}},{0.5,{"AdvancedWeaponScopes\LeupoldMk4\data\scope_damage.rvmat","DZ\weapons\attachments\data\terra_damage.rvmat"}},{0.3,{}},{0.0,{"AdvancedWeaponScopes\LeupoldMk4\data\scope_destruct.rvmat","DZ\weapons\attachments\data\terra_destruct.rvmat"}}};
				};
			};
		};
		class OpticsInfo
		{
			memoryPointCamera = "eyeScope";
			cameraDir = "cameraDir";
			useModelOptics = 1;
			modelOptics = "AdvancedWeaponScopes\LeupoldMk4\leupoldmk4_reticle.p3d";
			preloadOpticType = "Preload2DOptic_LeupoldMk4";
			opticsDisablePeripherialVision = 0.67;
			opticsFlare = 1;
			opticsPPEffects[] = {"OpticsCHAbera3","OpticsBlur1"};
			opticsZoomMin = "0.3926/3.5";
			opticsZoomMax = "0.3926/10";
			opticsZoomInit = "0.3926/3.5";
			discretefov[] = {"0.3926/3.5","0.3926/7","0.3926/10"};
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
	class Preload2DOptic_LeupoldMk4: Inventory_Base
	{
		scope = 0;
		model = "AdvancedWeaponScopes\LeupoldMk4\leupoldmk4_reticle.p3d";
	};
	class AD_LeupoldMk4: AD_LeupoldMk4_Base
	{
		scope = 2;
	};
};
