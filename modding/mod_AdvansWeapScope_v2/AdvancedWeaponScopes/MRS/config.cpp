class CfgPatches
{
	class AD_MRS
	{
		units[] = {"AD_MRS"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Optics"};
	};
};
class cfgVehicles
{
	class ItemOptics_Base;
	class AD_MRS_Base: ItemOptics_Base
	{
		scope = 0;
		displayName = "$STR_AD_MRS0";
		descriptionShort = "$STR_AD_MRS1";
		model = "\AdvancedWeaponScopes\MRS\mrs.p3d";
		attachments[] = {"BatteryD"};
		animClass = "Binoculars";
		simulation = "itemoptics";
		inventorySlot[] = {"weaponOptics","sniperOptics","weaponOpticsHunting"};
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
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\MRS\data\mrs_glass_ca.paa","AdvancedWeaponScopes\MRS\data\mrs.rvmat"}},{0.7,{}},{0.5,{"DZ\weapons\attachments\optics\data\lensglass_damage_ca.paa","AdvancedWeaponScopes\MRS\data\mrs_damaged.rvmat"}},{0.3,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\MRS\data\mrs_damaged.rvmat"}},{0.0,{"DZ\weapons\attachments\optics\data\lensglass_destruct_ca.paa","AdvancedWeaponScopes\MRS\data\mrs_destruct.rvmat"}}};
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
			distanceZoomMax = 300;
			discreteDistance[] = {50,100,150,200,300};
			discreteDistanceInitIndex = 0;
			PPMaskProperties[] = {0.5,0.5,0.4,0.05};
			PPLensProperties[] = {1,0.15,0,0};
			PPBlurProperties = 0.2;
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
	class AD_MRS: AD_MRS_Base
	{
		scope = 2;
	};
};
