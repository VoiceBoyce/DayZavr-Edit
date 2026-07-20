class CfgPatches {
	class YourPatchesHere {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};

class CfgMods {
	class YourModHere {
		dir = "Loadingscreen";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "Loadingscreen";
		credits = "";
		author = "";
		authorID = "0";
		version = 0.1;
		extra = 0;
		type = "mod";
		dependencies[] = {"Game"};
		
		class defs {
			class gameScriptModule {
				value = "";
				files[] = {"Loadingscreen/scripts/3_Game"};
			};
		};
	};
};
