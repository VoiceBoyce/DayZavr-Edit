class CfgPatches
{
	class MSFZ_Crossbow
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Firearms"};
	};
};
class Mode_Safe;
class Mode_SemiAuto;
class Mode_Burst;
class Mode_FullAuto;
class OpticsInfoRifle;
class cfgWeapons
{
	class Rifle_Base;
	class MSFZ_Crossbow_base: Rifle_Base
	{
		scope = 0;
		weight = 3070;
		absorbency = 0;
		repairableWithKits[] = {1};
		repairCosts[] = {25};
		PPDOFProperties[] = {1,0.5,50,180.1,4,10};
		WeaponLength = 0.9;
		itemSize[] = {8,3};
		barrelArmor = 2.743;
		initSpeedMultiplier = 0.7;
		chamberSize = 1;
		chamberedRound = "";
		ironsightsExcludingOptics[] = {"M4_CarryHandleOptic","BUISOptic","M68Optic","M4_T3NRDSOptic","ReflexOptic","ACOGOptic","HuntingOptic"};
		chamberableFrom[] = {"MSFZ_Crossbow_BoltStack","MSFZ_Crossbow_BoltStack_AP","MSFZ_Ammo_CupidsBolt"};
		magazines[] = {"MSFZ_Crossbow_Mag_10Rnd","MSFZ_Crossbow_Mag_25Rnd","MSFZ_Crossbow_Mag_50Rnd","MSFZ_Crossbow_AP_Mag_10Rnd","MSFZ_Crossbow_AP_Mag_25Rnd","MSFZ_Crossbow_AP_Mag_50Rnd","MSFZ_Crossbow_Cupid_Mag_10Rnd","MSFZ_Crossbow_Cupid_Mag_25Rnd","MSFZ_Crossbow_Cupid_Mag_50Rnd"};
		hiddenSelections[] = {"camo","camo1"};
		magazineSwitchTime = 0.38;
		ejectType = 1;
		recoilModifier[] = {1,1,1};
		swayModifier[] = {2.2,2.2,1.2};
		modes[] = {"SemiAuto","Burst"};
		class SemiAuto: Mode_SemiAuto
		{
			soundSetShot[] = {"crossbow_SoundSet","",""};
			soundSetShotExt[] = {{"crossbow_Silenced_SoundSet","M4_silencerTail_SoundSet","M4_silencerInteriorTail_SoundSet"},{"M4_silencerHomeMade_SoundSet","M4_silencerHomeMadeTail_SoundSet","M4_silencerInteriorHomeMadeTail_SoundSet"}};
			reloadTime = 0.12;
			recoil = "recoil_AK74";
			recoilProne = "recoil_AK74_prone";
			dispersion = 0.002;
			magazineSlot = "magazine";
		};
		class Burst: Mode_Burst
		{
			soundSetShot[] = {"crossbow_SoundSet","",""};
			burst = 3;
			reloadTime = 0.098;
			dispersion = 0.0015;
			magazineSlot = "magazine";
		};
		class AnimationSources
		{
			class bullet
			{
				source = "user";
				initPhase = 0;
				animPeriod = 0.01;
			};
		};
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera = "eye";
			discreteDistance[] = {160};
			discreteDistanceInitIndex = 0;
			modelOptics = "-";
			distanceZoomMin = 100;
			distanceZoomMax = 1000;
		};
		class NoiseShoot
		{
			strength = 82;
			type = "shot";
		};
		weaponStateAnim = "dz\anims\anm\player\reloads\VSS\w_VSS_states.anm";
		class InventorySlotsOffsets
		{
			class Shoulder
			{
				position[] = {-0.1,0,0};
				orientation[] = {0,-17,0};
			};
			class Melee
			{
				position[] = {-0.1,-0.01,0};
				orientation[] = {0,20,0};
			};
		};
	};
	class MSFZ_Crossbow: MSFZ_Crossbow_base
	{
		scope = 2;
		displayName = "$STR_MSFZ_Crossbow";
		descriptionShort = "$STR_MSFZ_Crossbow1";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\MSFZ_Crossbow.p3d";
		attachments[] = {"weaponWrap","weaponOpticsHunting","weaponOptics","weaponFlashlight"};
		hiddenSelections[] = {"camo","camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_Stvol.paa","MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 5000;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_Stvol.rvmat","MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_Stvol.rvmat","MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_Stvol_damage.rvmat","MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_Stvol_damage.rvmat","MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_Stvol_destruct.rvmat","MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_Crossbow_Black: MSFZ_Crossbow
	{
		scope = 2;
		displayName = "$STR_MSFZ_Crossbow";
		descriptionShort = "$STR_MSFZ_Crossbow1";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\MSFZ_Crossbow.p3d";
		attachments[] = {"weaponWrap","weaponOpticsHunting","weaponOptics","weaponFlashlight"};
		hiddenSelections[] = {"camo","camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_Black.paa","MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
	};
	class MSFZ_Crossbow_Cosmos: MSFZ_Crossbow
	{
		scope = 2;
		displayName = "$STR_MSFZ_Crossbow";
		descriptionShort = "$STR_MSFZ_Crossbow1";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\MSFZ_Crossbow.p3d";
		attachments[] = {"weaponWrap","weaponOpticsHunting","weaponOptics","weaponFlashlight"};
		hiddenSelections[] = {"camo","camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_Cosmos.paa","MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
	};
	class MSFZ_Crossbow_Forest: MSFZ_Crossbow
	{
		scope = 2;
		displayName = "$STR_MSFZ_Crossbow";
		descriptionShort = "$STR_MSFZ_Crossbow1";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\MSFZ_Crossbow.p3d";
		attachments[] = {"weaponWrap","weaponOpticsHunting","weaponOptics","weaponFlashlight"};
		hiddenSelections[] = {"camo","camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_Forest.paa","MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
	};
	class MSFZ_Crossbow_Mix: MSFZ_Crossbow
	{
		scope = 2;
		displayName = "$STR_MSFZ_Crossbow";
		descriptionShort = "$STR_MSFZ_Crossbow1";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\MSFZ_Crossbow.p3d";
		attachments[] = {"weaponWrap","weaponOpticsHunting","weaponOptics","weaponFlashlight"};
		hiddenSelections[] = {"camo","camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_Mix.paa","MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
	};
	class MSFZ_Crossbow_Old: MSFZ_Crossbow
	{
		scope = 2;
		displayName = "$STR_MSFZ_Crossbow";
		descriptionShort = "$STR_MSFZ_Crossbow1";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\MSFZ_Crossbow.p3d";
		attachments[] = {"weaponWrap","weaponOpticsHunting","weaponOptics","weaponFlashlight"};
		hiddenSelections[] = {"camo","camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_Old.paa","MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
	};
};
