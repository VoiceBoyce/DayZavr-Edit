class CfgPatches
{
	class Vp_SET13
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
	class Vp_SET13
	{
		dir="Telega";
		hideName=1;
		hidePicture=1;
		name="Vp_SET13";
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
					"Set_Oni\set\script\4_world"
				};
			};
		};
	};
};
