class CfgPatches
{
	class AD_SpecterDR
	{
		units[] = {"AD_SpecterDR"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Weapons_Optics"};
	};
};
class cfgVehicles
{
	class ItemOptics_Base;
	class AD_SpecterDR_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_SpecterDR0";
		descriptionShort = "$STR_AD_SpecterDR1";
		model = "AdvancedWeaponScopes\SpecterDR\specterdr.p3d";
		animClass = "Binoculars";
		simulation = "itemoptics";
		inventorySlot[] = {"weaponOptics","sniperOptics","weaponOpticsHunting"};
		rotationFlags = 16;
		reversed = 0;
		weight = 500;
		itemSize[] = {2,1};
		dispersionModifier = -0.001;
		dispersionCondition = "true";
		recoilModifier[] = {1,1,1};
		simpleHiddenSelections[] = {"hide"};
		memoryPointCamera = "eyeScope";
		cameraDir = "cameraDir";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1.0,{"DZ\weapons\attachments\data\scope_alpha_clear_ca.paa","DZ\weapons\attachments\optics\data\lensglass_standard.rvmat","AdvancedWeaponScopes\SpecterDR\data\specterdr.rvmat"}},{0.7,{}},{0.5,{"DZ\weapons\attachments\data\scope_alpha_damaged_ca.paa","DZ\weapons\attachments\optics\data\lensglass_standard_damage.rvmat","AdvancedWeaponScopes\SpecterDR\data\specterdr_damage.rvmat"}},{0.3,{}},{0.0,{"DZ\weapons\attachments\data\scope_alpha_destroyed_ca.paa","DZ\weapons\attachments\optics\data\lensglass_standard_destruct.rvmat","AdvancedWeaponScopes\SpecterDR\data\specterdr_destruct.rvmat"}}};
				};
			};
		};
		class OpticsInfo
		{
			memoryPointCamera = "eyeScope";
			cameraDir = "cameraDir";
			modelOptics = "-";
			opticsDisablePeripherialVision = 0.65;
			opticsFlare = 1;
			opticsPPEffects[] = {};
			opticsZoomMin = "0.3926/1.5";
			opticsZoomMax = "0.3926/6";
			opticsZoomInit = "0.3926/1.5";
			discretefov[] = {"0.3926/1.5","0.3926/6"};
			distanceZoomMin = 100;
			distanceZoomMax = 1000;
			discreteDistance[] = {100,200,300,400,500,600,700,800,900,1000};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {};
			PPLensProperties[] = {};
			PPBlurProperties = 0.0;
		};
		class OpticsInfoWeaponOverride
		{
			memoryPointCamera = "eyeIronsights";
			cameraDir = "cameraDirIronsights";
			opticsZoomMin = 0.5236;
			opticsZoomMax = 0.5236;
			opticsZoomInit = 0.5236;
			distanceZoomMin = 50;
			distanceZoomMax = 50;
			discreteDistance[] = {50};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.375,0.05};
			PPLensProperties[] = {0.5,0.15,0,0};
			PPBlurProperties = 0.6;
			PPDOFProperties[] = {1,0.1,20,200,4,10};
		};
	};
	class AD_SpecterDR: AD_SpecterDR_Base
	{
		scope = 2;
	};
};
