class CfgPatches
{
	class MSFZ_Mifical_Pack
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Animals","DZ_AI_Bliss"};
	};
};
class CfgMods
{
	class MSFZ_Mifical_Pack
	{
		dir = "MSFZ_Mifical_Pack";
		name = "MSFZ_Mifical_Pack";
		credits = "mars";
		author = "mars";
		authorID = "76561199100729161";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"MSFZ_Mifical_Pack/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"MSFZ_Mifical_Pack/scripts/4_World"};
			};
		};
	};
};
