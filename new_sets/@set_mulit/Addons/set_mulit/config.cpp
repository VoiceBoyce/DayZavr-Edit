class CfgPatches
{
	class set_mulit
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Characters_Vests"
		};
	};
};
class CfgMods
{
	class set_mulit
	{
		dir="set_mulit";
		hideName=1;
		hidePicture=1;
		name="set_mulit";
		credits="twizicss https://discord.gg/kCUPNwPRuR";
		author="twizicss https://discord.gg/kCUPNwPRuR";
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
				files[]={};
			};
		};
	};
};
