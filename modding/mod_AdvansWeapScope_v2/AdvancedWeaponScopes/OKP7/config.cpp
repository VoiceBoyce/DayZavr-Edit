class CfgPatches
{
	class AD_OKP7
	{
		units[] = {"AD_OKP7"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Optics"};
	};
};
class cfgVehicles
{
	class ItemOptics_Base;
	class AD_OKP7_Base: ItemOptics_Base
	{
		scope = 0;
		model = "\AdvancedWeaponScopes\OKP7\okp7.p3d";
		attachments[] = {"BatteryD"};
		animClass = "Binoculars";
		simulation = "itemoptics";
		inventorySlot[] = {"weaponOpticsHunting","weaponOptics","sniperOptics"};
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
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\OKP7\data\Steklo_ca.paa","AdvancedWeaponScopes\OKP7\data\okp7.rvmat"}},{0.7,{}},{0.5,{"DZ\weapons\attachments\optics\data\lensglass_damage_ca.paa","AdvancedWeaponScopes\OKP7\data\okp7_damaged.rvmat"}},{0.3,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\OKP7\data\okp7_destruct.rvmat"}},{0.0,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\OKP7\data\okp7_destruct.rvmat"}}};
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
			opticSightTexture = "AdvancedWeaponScopes\OKP7\data\reticle_ca.paa";
			opticsZoomMin = 0.5236;
			opticsZoomMax = 0.5236;
			opticsZoomInit = 0.5236;
			distanceZoomMin = 50;
			distanceZoomMax = 300;
			discreteDistance[] = {50,100,150,200,300};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.4,0.05};
			PPLensProperties[] = {1,0.15,0,0};
			PPBlurProperties = 0.2;
		};
		class EnergyManager
		{
			hasIcon = 1;
			energyUsagePerSecond = 0.02;
			plugType = 1;
			attachmentAction = 1;
		};
	};
	class AD_OKP7: AD_OKP7_Base
	{
		scope = 2;
		displayName = "$STR_AD_OKP70";
		descriptionShort = "$STR_AD_OKP71";
	};
};
