class CfgPatches
{
	class Disable_Car_Damage_Collision
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
	};
};

class CfgMods
{
	class Disable_Car_Damage_Collision
	{
		dir = "Disable_Car_Damage_Collision";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 0;
		name = "Fix Disable_Car_Damage_Collision";
		credits = "";
		author = "Zypr";
		authorID = "";
		version = "1";
		extra = 0;
		type = "mod";
		dependencies[] = {"World"};

		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"Disable_Car_Damage_Collision/Scripts/4_World"};
			};
		};
	};
};
