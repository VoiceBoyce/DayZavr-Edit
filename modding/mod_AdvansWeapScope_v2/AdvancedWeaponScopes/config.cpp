class CfgPatches
{
	class AdvancedWeaponScopes
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgMods
{
	class AdvancedWeaponScopes
	{
		dir = "AdvancedWeaponScopes";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "AdvancedWeaponScopes";
		credits = "Inkota";
		author = "Inkota";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = {"World"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"AdvancedWeaponScopes/Scripts/4_World"};
			};
		};
	};
};
