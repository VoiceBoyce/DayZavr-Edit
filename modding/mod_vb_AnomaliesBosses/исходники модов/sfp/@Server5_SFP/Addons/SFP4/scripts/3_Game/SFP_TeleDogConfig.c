//==============================================================================
// SFP TeleDog (SFP_DikoDog) - JSON configuration (server-side)
//
// Creates (server):
//   $profile:SFP/teledog_setting.json
//
// Allows server owners to tweak:
//  - Health (starting)
//  - Teleport intervals (min/max) for both "around player" and "behind player"
//==============================================================================

class SFP_TeleDogConfig
{
    int Version = 1;

    // --- Teleport intervals (seconds) ---
    float TeleportNearIntervalMin   = 4.0;
    float TeleportNearIntervalMax   = 10.0;

    float TeleportBehindIntervalMin = 2.0;
    float TeleportBehindIntervalMax = 6.0;

    // --- Health (server) ---
    // StartHealth >= 0 => SetHealth("","Health", StartHealth)
    // Otherwise StartHealth01 in [0..1] => SetHealth01("","Health", coef)
    float StartHealth   = -1.0;
    float StartHealth01 = 1.0;
};


class SFP_TeleDogConfigManager
{
    protected static ref SFP_TeleDogConfig s_Cache;

    protected static const string PROFILE_DIR = "$profile:SFP";
    protected static const string FILE_NAME   = "teledog_setting.json";

    protected static void EnsureDir()
    {
        if (!FileExist(PROFILE_DIR))
            MakeDirectory(PROFILE_DIR);
    }

    static ref SFP_TeleDogConfig Get()
    {
        if (s_Cache)
            return s_Cache;

        EnsureDir();

        string filePath = PROFILE_DIR + "/" + FILE_NAME;
        ref SFP_TeleDogConfig cfg = new SFP_TeleDogConfig;

        if (FileExist(filePath))
        {
            // Load & keep defaults for any missing fields
            JsonFileLoader<SFP_TeleDogConfig>.JsonLoadFile(filePath, cfg);
        }

        // Always write back (creates file on first run + fills missing fields)
        JsonFileLoader<SFP_TeleDogConfig>.JsonSaveFile(filePath, cfg);

        s_Cache = cfg;
        return cfg;
    }
};
