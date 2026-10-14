class CfgPatches
{
	class set_Legion
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters"
		};
	};
};
class CfgMods
{
	class set_Legion
	{
		dir="Telega";
		hideName=1;
		hidePicture=1;
		name="set_Legion";
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
					"set_legion\set\script\4_world"
				};
			};
		};
	};
};
