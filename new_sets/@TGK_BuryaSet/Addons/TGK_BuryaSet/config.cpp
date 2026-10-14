class CfgPatches
{
	class TGK_BuryaSet
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
	class TGK_BuryaSet
	{
		dir="TGK_BuryaSet";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="TGK_BuryaSet";
		credits="twizicss";
		author="twizicss";
		authorID="0";
		version="1.0";
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
					"TGK_BuryaSet/Set/script/4_World"
				};
			};
		};
	};
};
