#define _ARMA_

class CfgPatches {
    class RF_ESCAPE_FROM_DAYZ_CL {
        author = "Dad Fedor & romabeorn";
        name = "RF_ESCAPE_FROM_DAYZ_CL";
        url = "https://discord.gg/d9kP7ezX3n";
        units[] = {};
        weapons[] = {};
        requiredAddons[] = {"DZ_Data", "DZ_Sounds_Effects", "RF"};
        requiredVersion = 0.1;
    };

    class DZ_Gear_Camping {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgMods {
    class RF_ESCAPE_FROM_DAYZ_CL {
        type="mod";
        dir="RF_ESCAPE_FROM_DAYZ_CL";
        class defs {
            class gameScriptModule {
                value = "";
                files[] = { "RF_ESCAPE_FROM_DAYZ_CL/scripts/3_Game" };
            };
            class worldScriptModule {
                value = "";
                files[] = { "RF_ESCAPE_FROM_DAYZ_CL/scripts/4_World" };
            };
            class missionScriptModule {
                value = "";
                files[] = { "RF_ESCAPE_FROM_DAYZ_CL/scripts/5_Mission" };
            };
        };
    };
};




class CfgSoundShaders {
	class baseCharacter_SoundShader;
	class RF_ESCAPE_FROM_DAYZ_CL_Sirena_SoundShader: baseCharacter_SoundShader {
		samples[] = {
            {"RF_ESCAPE_FROM_DAYZ_CL\sounds\sirena.ogg", 1},
            {"RF_ESCAPE_FROM_DAYZ_CL\sounds\sirena.ogg", 1},
            {"RF_ESCAPE_FROM_DAYZ_CL\sounds\sirena.ogg", 1},
            {"RF_ESCAPE_FROM_DAYZ_CL\sounds\sirena.ogg", 1},
            {"RF_ESCAPE_FROM_DAYZ_CL\sounds\sirena.ogg", 1}
        };
		range=3500;
		volume=0.2;
	};
    class RF_ESCAPE_FROM_DAYZ_CL_SoundGoingToRaidByTrain_SoundShader: baseCharacter_SoundShader {
		samples[] = {{"RF_ESCAPE_FROM_DAYZ_CL\sounds\Sound_going_raid_train.ogg", 1}};
		volume=0.6;
	};
    class RF_ESCAPE_FROM_DAYZ_CL_SoundTeleportFromZoneThruWall_SoundShader: baseCharacter_SoundShader {
		samples[] = {{"RF_ESCAPE_FROM_DAYZ_CL\sounds\Sound_teleport_from_zone_thru_wall.ogg", 1}};
		volume=0.6;
	};
    class RF_ESCAPE_FROM_DAYZ_CL_SoundTeleportFromRaidByPlane_SoundShader: baseCharacter_SoundShader {
		samples[] = {{"RF_ESCAPE_FROM_DAYZ_CL\sounds\Sound_teleport_from_zone_with_plane.ogg", 1}};
		volume=0.6;
	};
};

class CfgSoundSets {
	class baseCharacter_SoundSet;
	class RF_ESCAPE_FROM_DAYZ_CL_Sirena_SoundSet: baseCharacter_SoundSet {
		soundShaders[] = {"RF_ESCAPE_FROM_DAYZ_CL_Sirena_SoundShader"};
        // volumeFactor=1;
		// frequencyRandomizer=2;
		// spatial=1;
		// doppler=0;
		// loop=0;
		// volumeCurve="heliCrashDistantAttenuationCurve";
		// distanceFilter="explosionDistanceFreqAttenuationFilter";
        volumeFactor=1;
		frequencyRandomizer=2;
		spatial=1;
		doppler=0;
		loop=0;
		volumeCurve="artilleryDistantAttenuationCurve";
		distanceFilter="explosionDistanceFreqAttenuationFilter";
		sound3DProcessingType="ThunderNear3DProcessingType";
	};
    class RF_ESCAPE_FROM_DAYZ_CL_SoundGoingToRaidByTrain_SoundSet: baseCharacter_SoundSet {
		soundShaders[] = {"RF_ESCAPE_FROM_DAYZ_CL_SoundGoingToRaidByTrain_SoundShader"};
	};
    class RF_ESCAPE_FROM_DAYZ_CL_SoundTeleportFromZoneThruWall_SoundSet: baseCharacter_SoundSet {
		soundShaders[] = {"RF_ESCAPE_FROM_DAYZ_CL_SoundTeleportFromZoneThruWall_SoundShader"};
	};
    class RF_ESCAPE_FROM_DAYZ_CL_SoundTeleportFromRaidByPlane_SoundSet: baseCharacter_SoundSet {
		soundShaders[] = {"RF_ESCAPE_FROM_DAYZ_CL_SoundTeleportFromRaidByPlane_SoundShader"};
	};
};