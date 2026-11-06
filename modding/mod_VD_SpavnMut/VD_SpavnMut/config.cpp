class CfgPatches
{
	class VD_SpavnMut
	{
		units[] = {};
		ammo[] = {};
		weapons[] = {};
		magazines[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
		Ultima_Version = 0.1;
	};
};
class CfgMods
{
	class VD_SpavnMut
	{
		dir = "";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "VD_SpavnMut";
		credits = "VooDoo";
		author = "VooDoo";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"VD_SpavnMut/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"VD_SpavnMut/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"VD_SpavnMut/scripts/5_Mission"};
			};
		};
	};
};
