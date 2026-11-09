class CfgPatches
{
	class MSFZ_Crossbow_Bolt
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Weapons_Magazines"};
		magazines[] = {"MSFZ_Crossbow_Mag_10Rnd","MSFZ_Crossbow_Mag_25Rnd","MSFZ_Crossbow_Mag_50Rnd","MSFZ_Crossbow_AP_Mag_10Rnd","MSFZ_Crossbow_AP_Mag_25Rnd","MSFZ_Crossbow_AP_Mag_50Rnd","MSFZ_Crossbow_Cupid_Mag_10Rnd","MSFZ_Crossbow_Cupid_Mag_25Rnd","MSFZ_Crossbow_Cupid_Mag_50Rnd"};
	};
};
class CfgMagazines
{
	class Magazine_Base;
	class Ammunition_Base;
	class MSFZ_Crossbow_BoltStack: Ammunition_Base
	{
		scope = 2;
		displayName = "$STR_Crossbow_BoltStack";
		descriptionShort = "$STR_Crossbow_BoltStack_text";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\ammo\MSFZ_Crossbow_BoltStack.p3d";
		iconCartridge = 3;
		weight = 40;
		itemSize[] = {4,1};
		count = 20;
		ammo = "MSFZ_Crossbow_Bolt";
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_Crossbow_BoltStack_AP: MSFZ_Crossbow_BoltStack
	{
		scope = 2;
		displayName = "$STR_Crossbow_BoltStack_AP";
		descriptionShort = "$STR_Crossbow_BoltStack_AP_text";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\ammo\MSFZ_Crossbow_BoltStack.p3d";
		iconCartridge = 3;
		weight = 40;
		itemSize[] = {4,1};
		count = 20;
		ammo = "MSFZ_Crossbow_Bolt_AP";
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
	};
	class MSFZ_Ammo_CupidsBolt: MSFZ_Crossbow_BoltStack
	{
		scope = 2;
		displayName = "$STR_CfgMagazines_Ammo_CupidsBolt0";
		descriptionShort = "$STR_CfgMagazines_Ammo_CupidsBolt1";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\ammo\MSFZ_Crossbow_BoltStack.p3d";
		iconCartridge = 3;
		weight = 40;
		itemSize[] = {4,1};
		count = 20;
		ammo = "MSFZ_Crossbow_CupidsBolt";
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Crossbow\data\MSFZ_Crossbow_frame.paa"};
	};
};
