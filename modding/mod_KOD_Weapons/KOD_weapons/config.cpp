class CfgPatches
{
	class KOD_weapons
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class CfgMods
{
	class KOD_weapons
	{
		dir="KOD_weapons";
		hideName=1;
		hidePicture=1;
		name="KOD_weapons";
		author="Inkota";
		version="1.0";
		type="mod";
		dependencies[]=
		{
			"World",
			"gui"
		};
		class defs
		{
			class imageSets
			{
				files[]=
				{
					"KOD_weapons/gui/imagesets/kod_icons.imageset"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"KOD_weapons/scripts/4_world"
				};
			};
		};
	};
};
