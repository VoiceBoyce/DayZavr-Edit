class CfgPatches
{
    class ArtifactScriptsServer
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]=
        {
			"DD_Artifacts_Scripts"
        };
    };
};

class CfgMods
{
	class ArtifactScriptsServer
	{
		dir = "ArtifactScriptsServer";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 1;
		name = "ArtifactScriptsServer";
		credits = "";
		creditsJson = "";
		author = "";
		authorID = "0"; 
		version = 1.0.0;
		extra = 0;
		type = "mod";
		
		dependencies[] = { "World" };
		
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"ArtifactScriptsServer/Scripts/4_World"};
			};
		};
	};
};