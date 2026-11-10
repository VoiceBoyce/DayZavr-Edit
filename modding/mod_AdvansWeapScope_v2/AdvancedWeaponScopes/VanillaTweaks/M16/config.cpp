class CfgPatches
{
	class AD_VanillaTweaks_M16
	{
		units[] = {};
		weapons[] = {"AD_M16A2"};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Weapons_Firearms_M16A2"};
	};
};
class cfgWeapons
{
	class M16A2_Base;
	class AD_M16A2: M16A2_Base
	{
		scope = 2;
		displayName = "$STR_AD_M16A2_0";
		descriptionShort = "$STR_AD_M16A2_1";
		attachments[] = {"weaponWrap","weaponMuzzleM4","weaponBayonet","weaponOptics"};
	};
};
