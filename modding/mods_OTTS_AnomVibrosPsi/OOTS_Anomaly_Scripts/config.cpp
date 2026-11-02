class CfgPatches
{
	class OOTS_Anomaly_Scripts
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Scripts",
			"DZ_Data",
			"OOTS_Anomaly_Client"
		};
	};
};

class CfgMods
{
	class OOTS_Anomaly_Scripts
	{
		author="";
		type="mod";
		dependencies[]=
		{
			"Game",
			"World"
		};
		class defs
		{
			
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"OOTS_Anomaly_Scripts/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"OOTS_Anomaly_Scripts/scripts/4_World"
				};
			};
		};
	};
};