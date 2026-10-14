class CfgPatches
{
	class TGK_CasualSoldier
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts"
		};
	};
};
class CfgMods
{
	class TGK_CasualSoldier
	{
		dir="TGK_CasualSoldier";
		hideName=1;
		hidePicture=1;
		name="TGK_CasualSoldier";
		credits="https://discord.gg/tgkworkshop";
		author="https://discord.gg/tgkworkshop";
		authorID="0";
		version=1;
		extra=0;
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
					"TGK_CasualSoldier/set/Scripts/4_World"
				};
			};
		};
	};
};
