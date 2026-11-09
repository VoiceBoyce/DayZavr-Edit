class CfgPatches
{
	class MSFZ_Mifical_Pack_Server
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Animals","DZ_AI_Bliss","MSFZ_Mifical_Pack"};
	};
};
class CfgMods
{
	class MSFZ_Mifical_Pack_Server
	{
		dir = "MSFZ_Mifical_Pack_Server";
		name = "MSFZ_Mifical_Pack_Server";
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
				files[] = {"MSFZ_Mifical_Pack_Server/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"MSFZ_Mifical_Pack_Server/scripts/4_World"};
			};
		};
	};
};
