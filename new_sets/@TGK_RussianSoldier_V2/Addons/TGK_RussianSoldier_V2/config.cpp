class CfgPatches
{
	class TGK_RussianSoldier_v2
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Weapons_Explosives",
			"DZ_Characters_Vests",
			"DZ_Characters_Headgear",
			"DZ_Characters_Gloves",
			"DZ_Characters_Backpacks",
			"DZ_Weapons_Melee_Blade",
			"DZ_Gear_Containers",
			"DZ_Gear_Consumables",
			"DZ_Gear_Tools",
			"DZ_Vehicles_Parts",
			"DZ_Gear_Cooking",
			"DZ_Gear_Camping",
			"DZ_Radio",
			"DZ_Gear_Medical"
		};
		skipWhenMissingDependencies=1;
	};
};
class CfgMods
{
	class TGK_RussianSoldier_v2
	{
		dir="TGK_RussianSoldier_v2";
		hideName=1;
		hidePicture=1;
		name="TGK_RussianSoldier_v2";
		author="https://discord.gg/tgkworkshop TWIZICSS";
		version="1.0";
		type="mod";
		dependencies[]=
		{
			"World"
		};
		class defs
		{
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"GK_RussianSoldier_V2/set/Scripts/4_World"
				};
			};
			class imageSets
			{
				files[]=
				{
					"TGK_RussianSoldier_V2/set/gui/imagesets/TGK_CustomIcons_RusV2.imageset"
				};
			};
		};
	};
};
