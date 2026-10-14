class CfgPatches
{
	class Vp_Set6
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
	};
};
class CfgMods
{
	class Vp_Set6
	{
		dir="Telega";
		hideName=1;
		hidePicture=1;
		name="Vp_Set6";
		credits="https://discord.gg/Y7qy7z4rzv";
		author="https://discord.gg/Y7qy7z4rzv";
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
					"set_hitman\set\script\4_World"
				};
			};
		};
	};
};
