//============================================================
// SFP Scanner Event - JSON config
//
// File is stored in:
//   $profile:SFP/ScannerSpawnEvent.json
//
// It is created automatically on first server start with this mod.
//============================================================

class SFP_ScannerWaveConfig
{
	int CountMin;
	int CountMax;
	ref array<string> ClassNames;

	void SFP_ScannerWaveConfig()
	{
		ClassNames = new array<string>;
		CountMin = 3;
		CountMax = 7;
	}
}

class SFP_ScannerEventConfig
{
	int ConfigVersion = 1;

	// Random time between waves (seconds). Timer is shown on display (0..99).
	int IntervalMinSeconds = 10;
	int IntervalMaxSeconds = 60;

	// Anti-overflow: if too many alive spawned mobs are still in AliveCountRadius, countdown pauses.
	int PauseIfAliveMoreThan = 20;
	int ResumeWhenAliveLessThan = 15;
	float AliveCountRadius = 70.0;

	// Players within this radius are considered participants for "spawn away from player"
	// and also receive pulse shake/sound.
	float EventPlayerRadius = 80.0;

	// Spawn location rules
	float SpawnRadiusMin = 15.0;
	float SpawnRadiusMax = 70.0;
	float SpawnMinDistanceToPlayer = 12.0;
	int SpawnAttempts = 25;

	// 3 waves by cycle: Easy -> Hard -> VeryHard -> repeat
	ref SFP_ScannerWaveConfig Easy;
	ref SFP_ScannerWaveConfig Hard;
	ref SFP_ScannerWaveConfig VeryHard;

	void SFP_ScannerEventConfig()
	{
		Easy = new SFP_ScannerWaveConfig;
		Hard = new SFP_ScannerWaveConfig;
		VeryHard = new SFP_ScannerWaveConfig;
	}
}

class SFP_ScannerConfigManager
{
	protected static const string CFG_DIR = "$profile:SFP";
	protected static const string CFG_FILE = "$profile:SFP/ScannerSpawnEvent.json";

	protected static ref SFP_ScannerEventConfig s_Config;
	protected static bool s_TriedLoad;

	static SFP_ScannerEventConfig Get()
	{
		EnsureLoaded();
		return s_Config;
	}

	static void Reload()
	{
		s_Config = null;
		s_TriedLoad = false;
		EnsureLoaded();
	}

	protected static void EnsureLoaded()
	{
		if (s_Config || s_TriedLoad)
			return;

		// Only load/create on server side (or singleplayer).
		if (!(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		s_TriedLoad = true;

		if (!FileExist(CFG_DIR))
			MakeDirectory(CFG_DIR);

		ref SFP_ScannerEventConfig cfg = new SFP_ScannerEventConfig;

		string err;
		if (FileExist(CFG_FILE))
		{
			if (!JsonFileLoader<SFP_ScannerEventConfig>.LoadFile(CFG_FILE, cfg, err))
			{
				SFP_Log.Error("Scanner: Failed to load config, using defaults. Error: " + err);
				SetDefaults(cfg);
				JsonFileLoader<SFP_ScannerEventConfig>.SaveFile(CFG_FILE, cfg, err);
			}
			else
			{
				PatchMissing(cfg);
			}
		}
		else
		{
			SetDefaults(cfg);
			if (!JsonFileLoader<SFP_ScannerEventConfig>.SaveFile(CFG_FILE, cfg, err))
				SFP_Log.Error("Scanner: Failed to save default config: " + err);
			else
				SFP_Log.Info("[SFP Scanner] Created default config: " + CFG_FILE);
		}

		Validate(cfg);

		s_Config = cfg;
		SFP_Log.Info("[SFP Scanner] Config ready: " + CFG_FILE);
	}

	protected static void PatchMissing(SFP_ScannerEventConfig cfg)
	{
		if (!cfg)
			return;

		if (!cfg.Easy)
			cfg.Easy = new SFP_ScannerWaveConfig;
		if (!cfg.Hard)
			cfg.Hard = new SFP_ScannerWaveConfig;
		if (!cfg.VeryHard)
			cfg.VeryHard = new SFP_ScannerWaveConfig;

		if (!cfg.Easy.ClassNames)
			cfg.Easy.ClassNames = new array<string>;
		if (!cfg.Hard.ClassNames)
			cfg.Hard.ClassNames = new array<string>;
		if (!cfg.VeryHard.ClassNames)
			cfg.VeryHard.ClassNames = new array<string>;
	}

	protected static void SetDefaults(SFP_ScannerEventConfig cfg)
	{
		if (!cfg)
			return;

		cfg.ConfigVersion = 1;

		cfg.IntervalMinSeconds = 10;
		cfg.IntervalMaxSeconds = 60;

		cfg.PauseIfAliveMoreThan = 20;
		cfg.ResumeWhenAliveLessThan = 15;
		cfg.AliveCountRadius = 70.0;

		cfg.EventPlayerRadius = 80.0;

		cfg.SpawnRadiusMin = 15.0;
		cfg.SpawnRadiusMax = 70.0;
		cfg.SpawnMinDistanceToPlayer = 12.0;
		cfg.SpawnAttempts = 25;

		PatchMissing(cfg);

		// EASY (Wave 1)
		cfg.Easy.CountMin = 2;
		cfg.Easy.CountMax = 8;
		cfg.Easy.ClassNames.Clear();
		cfg.Easy.ClassNames.Insert("SFP_DikoDog");
		cfg.Easy.ClassNames.Insert("SFP_RatDog");
		cfg.Easy.ClassNames.Insert("SFP_Plot_style1");
		cfg.Easy.ClassNames.Insert("SFP_Plot_style2");
		cfg.Easy.ClassNames.Insert("SFP_SlepoDOG_Brown");
		cfg.Easy.ClassNames.Insert("SFP_SlepoDOG_bulterer");
		cfg.Easy.ClassNames.Insert("SFP_SlepoDOG_Red");
		cfg.Easy.ClassNames.Insert("SFP_SlepoDOG_White");
		cfg.Easy.ClassNames.Insert("SFP_tushkano");
		cfg.Easy.ClassNames.Insert("SFP_Boar_Brown");
		cfg.Easy.ClassNames.Insert("SFP_Boar_black");

		// HARD (Wave 2)
		cfg.Hard.CountMin = 2;
		cfg.Hard.CountMax = 7;
		cfg.Hard.ClassNames.Clear();
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_1");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_2");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_3");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_4");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_5");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_6");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_7");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_8");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_9");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_10");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_11");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_12");
		cfg.Hard.ClassNames.Insert("SFP_ZombieCivilian_13");
		cfg.Hard.ClassNames.Insert("SFP_izlome_1");
		cfg.Hard.ClassNames.Insert("SFP_izlome_2");
		cfg.Hard.ClassNames.Insert("SFP_izlome_3");
		cfg.Hard.ClassNames.Insert("SFP_izlome_4");
		cfg.Hard.ClassNames.Insert("SFP_Mutant_Snork");
		cfg.Hard.ClassNames.Insert("SFP_Mutant_Snork2");
		cfg.Hard.ClassNames.Insert("SFP_PseDOG_Brown");

		// VERY HARD (Wave 3)
		cfg.VeryHard.CountMin = 1;
		cfg.VeryHard.CountMax = 4;
		cfg.VeryHard.ClassNames.Clear();
		cfg.VeryHard.ClassNames.Insert("SFP_Piyavka_invz");
		cfg.VeryHard.ClassNames.Insert("SFP_ANM_Krovosos_inviz");
		cfg.VeryHard.ClassNames.Insert("SFP_ANM_Krovosos_Boloto_inviz");
		cfg.VeryHard.ClassNames.Insert("SFP_ANM_Krovosos_Dark_inviz");
		cfg.VeryHard.ClassNames.Insert("SFP_MozgoTras");
		cfg.VeryHard.ClassNames.Insert("SFP_MozgoTras_red");
		cfg.VeryHard.ClassNames.Insert("SFP_Shipastiy_invz");
		cfg.VeryHard.ClassNames.Insert("SFP_BearZMB");
		cfg.VeryHard.ClassNames.Insert("SFP_Himera");
		cfg.VeryHard.ClassNames.Insert("SFP_Trupojor");
	}

	protected static void Validate(SFP_ScannerEventConfig cfg)
	{
		if (!cfg)
			return;

		PatchMissing(cfg);

		// intervals (keep within display 0..99)
		cfg.IntervalMinSeconds = Math.Clamp(cfg.IntervalMinSeconds, 0, 99);
		cfg.IntervalMaxSeconds = Math.Clamp(cfg.IntervalMaxSeconds, 0, 99);
		if (cfg.IntervalMaxSeconds < cfg.IntervalMinSeconds)
			cfg.IntervalMaxSeconds = cfg.IntervalMinSeconds;

		// if someone accidentally set both to 0, restore defaults
		if (cfg.IntervalMinSeconds == 0 && cfg.IntervalMaxSeconds == 0)
		{
			cfg.IntervalMinSeconds = 10;
			cfg.IntervalMaxSeconds = 60;
		}

		// alive caps
		if (cfg.PauseIfAliveMoreThan < 0)
			cfg.PauseIfAliveMoreThan = 0;
		if (cfg.ResumeWhenAliveLessThan < 0)
			cfg.ResumeWhenAliveLessThan = 0;

		// radiuses
		if (cfg.AliveCountRadius < 0)
			cfg.AliveCountRadius = 0;
		if (cfg.EventPlayerRadius < 0)
			cfg.EventPlayerRadius = 0;
		if (cfg.SpawnRadiusMin < 0)
			cfg.SpawnRadiusMin = 0;
		if (cfg.SpawnRadiusMax < cfg.SpawnRadiusMin)
			cfg.SpawnRadiusMax = cfg.SpawnRadiusMin;
		if (cfg.SpawnMinDistanceToPlayer < 0)
			cfg.SpawnMinDistanceToPlayer = 0;
		if (cfg.SpawnAttempts < 1)
			cfg.SpawnAttempts = 1;

		ValidateWave(cfg.Easy, "Easy");
		ValidateWave(cfg.Hard, "Hard");
		ValidateWave(cfg.VeryHard, "VeryHard");
	}

	protected static void ValidateWave(SFP_ScannerWaveConfig wave, string name)
	{
		if (!wave)
			return;

		if (!wave.ClassNames)
			wave.ClassNames = new array<string>;

		if (wave.CountMin < 0)
			wave.CountMin = 0;
		if (wave.CountMax < wave.CountMin)
			wave.CountMax = wave.CountMin;

		// Remove empty and non-existing classnames to avoid server spam/errors.
		for (int i = wave.ClassNames.Count() - 1; i >= 0; i--)
		{
			string cn = wave.ClassNames[i];
			if (cn == string.Empty || !IsValidVehicleClass(cn))
			{
				SFP_Log.Debug("[SFP Scanner] Invalid classname in wave " + name + ": \"" + cn + "\" (removed)");
				wave.ClassNames.Remove(i);
			}
		}
	}

	protected static bool IsValidVehicleClass(string className)
	{
		return GetGame().ConfigIsExisting("CfgVehicles " + className);
	}
}