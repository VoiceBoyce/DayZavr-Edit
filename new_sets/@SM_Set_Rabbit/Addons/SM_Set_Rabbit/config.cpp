class CfgPatches
{
	class SM_Set_Rabbit
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
	class SM_Set_Rabbit
	{
		dir="SM_Set_Rabbit";
		hideName=1;
		hidePicture=1;
		name="SM_Set_Rabbit";
		credits="Sobranskiy & PIROTEXNIK";
		author="Sobranskiy & PIROTEXNIK";
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
					"SM_Set_Rabbit\scripts\4_World"
				};
			};
		};
	};
};
