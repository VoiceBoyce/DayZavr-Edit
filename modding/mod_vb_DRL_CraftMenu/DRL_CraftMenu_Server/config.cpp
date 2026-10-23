class CfgPatches
{
	class DRL_CraftMenu_Server
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DRL_CraftMenu"
		};
	};
};

class CfgMods
{
	class DRL_CraftMenu_Server
	{
		dir="DRL_CraftMenu_Server";
		picture="";
		action="";
		hideName=0;
		hidePicture=1;
		name="DRL_CraftMenu_Server";
		credits="";
		author="Drulik";
		authorID="0";
		version="1";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"DRL_CraftMenu_Server/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"DRL_CraftMenu_Server/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"DRL_CraftMenu_Server/scripts/5_Mission"
				};
			};
		};
	};
};