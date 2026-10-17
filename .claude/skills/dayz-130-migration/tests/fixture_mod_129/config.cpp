class CfgPatches
{
	class FixtureMod129
	{
		requiredAddons[] = {"DZ_Data","DZ_Gear_Camping"};
	};
};
class CfgMods
{
	class FixtureMod129
	{
		type = "mod";
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"FixtureMod129\scripts\4_World"};
			};
		};
	};
};
class CfgVehicles
{
	class BaseBuildingBase;
	class Fence: BaseBuildingBase
	{
		attachments[] = {"Wall_Barbedwire_1","Wall_Barbedwire_2","Wall_Camonet","Att_CombinationLock","Material_Nails"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1000;
				};
			};
		};
	};
	class AK_WoodBttstck;
	class Fixture_AKStock: AK_WoodBttstck
	{
		hiddenSelectionsTextures[] = {"FixtureMod129/data/stock_co.paa"};
	};
};
