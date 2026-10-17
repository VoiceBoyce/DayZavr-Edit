class CfgPatches
{
	class MSFZ_Crossbow_Magazines
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
		magazines[] = {"MSFZ_Crossbow_Mag_10Rnd","MSFZ_Crossbow_Mag_25Rnd","MSFZ_Crossbow_Mag_50Rnd","MSFZ_Crossbow_AP_Mag_10Rnd","MSFZ_Crossbow_AP_Mag_25Rnd","MSFZ_Crossbow_AP_Mag_50Rnd","MSFZ_Crossbow_Cupid_Mag_10Rnd","MSFZ_Crossbow_Cupid_Mag_25Rnd","MSFZ_Crossbow_Cupid_Mag_50Rnd"};
	};
};
class CfgMagazines
{
	class Magazine_Base;
	class MSFZ_Crossbow_Mag_10Rnd: Magazine_Base
	{
		scope = 2;
		displayName = "$STR_Crossbow_Mag_10Rnd";
		descriptionShort = "$STR_Crossbow_Mag_10Rnd_text";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\ammo\MSFZ_Crossbow_BoltMag.p3d";
		weight = 300;
		itemSize[] = {2,2};
		count = 10;
		ammo = "MSFZ_Crossbow_Bolt";
		ammoItems[] = {"MSFZ_Crossbow_BoltStack"};
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
		tracersEvery = 0;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9999999;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_Crossbow_Mag_25Rnd: MSFZ_Crossbow_Mag_10Rnd
	{
		scope = 2;
		displayName = "$STR_Crossbow_Mag_25Rnd";
		descriptionShort = "$STR_Crossbow_Mag_25Rnd_text";
		weight = 300;
		count = 25;
	};
	class MSFZ_Crossbow_Mag_50Rnd: MSFZ_Crossbow_Mag_10Rnd
	{
		scope = 2;
		displayName = "$STR_Crossbow_Mag_50Rnd";
		descriptionShort = "$STR_Crossbow_Mag_50Rnd_text";
		weight = 300;
		count = 50;
	};
	class MSFZ_Crossbow_AP_Mag_10Rnd: Magazine_Base
	{
		scope = 2;
		displayName = "$STR_Crossbow_AP_Mag_10Rnd";
		descriptionShort = "$STR_Crossbow_AP_Mag_10Rnd_text";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\ammo\MSFZ_Crossbow_BoltMag.p3d";
		weight = 300;
		itemSize[] = {2,2};
		count = 10;
		ammo = "MSFZ_Crossbow_Bolt_AP";
		ammoItems[] = {"MSFZ_Crossbow_BoltStack_AP"};
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
		tracersEvery = 0;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9999999;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_Crossbow_AP_Mag_25Rnd: MSFZ_Crossbow_AP_Mag_10Rnd
	{
		scope = 2;
		displayName = "$STR_Crossbow_AP_Mag_25Rnd";
		descriptionShort = "$STR_Crossbow_AP_Mag_25Rnd_text";
		weight = 300;
		count = 25;
	};
	class MSFZ_Crossbow_AP_Mag_50Rnd: MSFZ_Crossbow_AP_Mag_10Rnd
	{
		scope = 2;
		displayName = "$STR_Crossbow_AP_Mag_50Rnd";
		descriptionShort = "$STR_Crossbow_AP_Mag_50Rnd_text";
		weight = 300;
		count = 50;
	};
	class MSFZ_Crossbow_Cupid_Mag_10Rnd: Magazine_Base
	{
		scope = 2;
		displayName = "Магазин для стрел купидона";
		descriptionShort = "Магазин для стрел купидона на 10 снарядов";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\ammo\MSFZ_Crossbow_BoltMag.p3d";
		weight = 300;
		itemSize[] = {2,2};
		count = 10;
		ammo = "MSFZ_Crossbow_CupidsBolt";
		ammoItems[] = {"MSFZ_Ammo_CupidsBolt"};
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
		tracersEvery = 0;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 9999999;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_Crossbow_Cupid_Mag_25Rnd: MSFZ_Crossbow_Cupid_Mag_10Rnd
	{
		scope = 2;
		displayName = "Магазин для стрел купидона";
		descriptionShort = "Магазин для стрел купидона на 25 снарядов";
		weight = 300;
		count = 25;
	};
	class MSFZ_Crossbow_Cupid_Mag_50Rnd: MSFZ_Crossbow_Cupid_Mag_10Rnd
	{
		scope = 2;
		displayName = "Магазин для стрел купидона";
		descriptionShort = "Магазин для стрел купидона на 50 снарядов";
		weight = 300;
		count = 50;
	};
};
