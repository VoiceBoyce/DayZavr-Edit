class CfgPatches
{
	class TGK_S2_Warta
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Characters"
		};
	};
};
class CfgMods
{
	class TGK_S2_Warta
	{
		dir="TGK_S2_Warta";
		hideName=1;
		hidePicture=1;
		name="TGK_S2_Warta";
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
					"TGK_S2_Warta/set/scripts/4_World"
				};
			};
			class imageSets
			{
				files[]=
				{
					"TGK_S2_Warta/gui/imagesets/Icons.imageset"
				};
			};
		};
	};
};
