//==============================================================================
// SFP Alarm Stolb - JSON configuration & per-instance settings
//
// Creates (server):
//   $profile:SFP/AlarmStolb.json
//
// Supports:
//   - DefaultAutoOffSeconds (default 120 = 2 minutes)
//   - DefaultSoundType (default 3)
//   - Per-stolb overrides keyed by world Position
//==============================================================================

class SFP_AlarmStolbPoleConfig
{
	vector Position;
	int SoundType = 3;          // 1..4
	int AutoOffSeconds = 120;   // seconds
};

class SFP_AlarmStolbSettings
{
	int Version = 1;
	int DefaultSoundType = 3;        // 1..4
	int DefaultAutoOffSeconds = 120; // seconds
	ref array<ref SFP_AlarmStolbPoleConfig> Poles;

	void SFP_AlarmStolbSettings()
	{
		Poles = new array<ref SFP_AlarmStolbPoleConfig>;
	}
};

class SFP_AlarmStolbConfigManager
{
	protected static ref SFP_AlarmStolbConfigManager s_Instance;
	protected ref SFP_AlarmStolbSettings m_Settings;
	protected bool m_Loaded;

	static SFP_AlarmStolbConfigManager GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new SFP_AlarmStolbConfigManager;
		}
		return s_Instance;
	}

	void SFP_AlarmStolbConfigManager()
	{
		m_Loaded = false;
	}

	string GetProfileDir()
	{
		return "$profile:SFP";
	}

	string GetFilePath()
	{
		return GetProfileDir() + "/AlarmStolb.json";
	}

	void EnsureLoaded()
	{
		if (m_Loaded)
			return;

		Load();
		m_Loaded = true;
	}

	protected void EnsureDefaults()
	{
		if (!m_Settings)
			m_Settings = new SFP_AlarmStolbSettings;

		if (!m_Settings.Poles)
			m_Settings.Poles = new array<ref SFP_AlarmStolbPoleConfig>;

		if (m_Settings.DefaultAutoOffSeconds <= 0)
			m_Settings.DefaultAutoOffSeconds = 120;

		if (m_Settings.DefaultSoundType < 1 || m_Settings.DefaultSoundType > 4)
			m_Settings.DefaultSoundType = 3;
	}

	protected void EnsureDir()
	{
		string dir = GetProfileDir();
		if (!FileExist(dir))
			MakeDirectory(dir);
	}

	void Load()
	{
		EnsureDir();

		string path = GetFilePath();
		if (FileExist(path))
		{
			JsonFileLoader<SFP_AlarmStolbSettings>.JsonLoadFile(path, m_Settings);
		}

		EnsureDefaults();

		// Write back normalized file (creates file on first run and fills missing fields)
		Save();
	}

	void Save()
	{
		EnsureDir();
		JsonFileLoader<SFP_AlarmStolbSettings>.JsonSaveFile(GetFilePath(), m_Settings);
	}

	// Find config by position within tolerance (meters)
	SFP_AlarmStolbPoleConfig FindByPosition(vector pos, float tolerance = 0.05)
	{
		EnsureLoaded();

		if (!m_Settings || !m_Settings.Poles)
			return NULL;

		foreach (SFP_AlarmStolbPoleConfig cfg : m_Settings.Poles)
		{
			if (!cfg)
				continue;

			if (vector.Distance(cfg.Position, pos) <= tolerance)
				return cfg;
		}

		return NULL;
	}

	// Get or create entry for this position (server)
	SFP_AlarmStolbPoleConfig GetOrCreateByPosition(vector pos)
	{
		EnsureLoaded();

		SFP_AlarmStolbPoleConfig cfg = FindByPosition(pos);
		if (cfg)
			return cfg;

		cfg = new SFP_AlarmStolbPoleConfig;
		cfg.Position = pos;
		cfg.SoundType = m_Settings.DefaultSoundType;
		cfg.AutoOffSeconds = m_Settings.DefaultAutoOffSeconds;

		m_Settings.Poles.Insert(cfg);
		Save();
		return cfg;
	}

	int SanitizeSoundType(int soundType)
	{
		if (soundType < 1 || soundType > 4)
			return 3;
		return soundType;
	}

	int SanitizeAutoOffSeconds(int seconds)
	{
		if (seconds <= 0)
			return 120;
		return seconds;
	}
};
