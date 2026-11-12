//==============================================================================
// SFP MozgoTras - per-class JSON configuration (server-side)
//
// Creates (server):
//   $profile:SFP/SFP_MozgoTras_Base.json
//   $profile:SFP/SFP_MozgoTras_red.json
//
// File name is the class name. Values are loaded once and cached.
// Server loads + applies to entity and syncs numeric values to clients.
//==============================================================================

class SFP_MozgoTrasConfig
{
	int   Version = 1;

	// --- Gameplay (server) ---
	float AuraCalmRadius  = 70.0;
	float AuraAggroRadius = 50.0;

	float DamagePerSecond = 2.0;
	float StaminaDT       = 0.1;

	float VomitChance     = 0.10;
	float DropItemChance  = 0.20;

	float SuicideHPThreshold = 50.0; // player HP (0..100)
	float SuicideChance      = 0.70; // 0..1

	float TinnitusTimeMin = 8.0;
	float TinnitusTimeMax = 15.0;

	// --- Visuals (client) ---
	// Calm state
	float CalmColorR = 1.0;
	float CalmColorG = 1.0;
	float CalmColorB = 1.0;

	float CalmBlur = 0.0;
	float CalmBloodSaturation = 0.9;
	float CalmShockVignette   = 0.15;
	float CalmTunnelVignette  = 0.0;
	float CalmChromAbb        = 0.0;

	float CalmCameraShakeInterval = 1.0;
	float CalmCameraShakeStrength = 1.2;

	// Aggro state
	float AggroColorR = 0.60;
	float AggroColorG = 0.80;
	float AggroColorB = 1.00;

	float AggroBlurMin = 0.15;
	float AggroBlurMax = 0.85;

	float AggroSaturationMin = 0.05;
	float AggroSaturationMax = 1.0;

	float AggroShockVignetteCoef  = 0.6;
	float AggroTunnelVignetteCoef = 0.8;
	float AggroChromAbbCoef       = 0.4;
	float AggroLensDistortCoef    = 0.4;
	float AggroLensParam3         = 0.0;
	float AggroLensParam4         = 0.0;

	float AggroCameraShakeInterval = 0.6;
	float AggroCameraShakeStrength = 4.5;

	// --- Tinnitus / flashbang imitation ---
	float TinnitusEffectChance = 0.5;
	float FlashbangProgressMult = 0.5;
	float FlashbangVisualMax    = 0.15;
	float FlashbangSoundMax     = 0.5;

	// --- Sound ---
	// These names are mapped to a small registry and synced as indices.
	string CalmSoundSet = "SFP_MozgoTras_PsiAura_Soundset";
	string AggroSoundSet = "SFP_MozgoTras_PsiAuraGuaa_Soundset";
	float LoopSoundVolume = 0.1;
	float LoopSoundPitch  = 0.2;

	// --- Mutant health (server) ---
	// MutantStartHealth >= 0 => SetHealth("","Health", MutantStartHealth)
	// Otherwise MutantStartHealth01 in [0..1] => SetHealth01("","Health", coef)
	float MutantStartHealth   = -1.0;
	float MutantStartHealth01 = 1.0;
};


class SFP_MozgoTrasSoundSetRegistry
{
	// Extend this list if you want to allow additional soundsets through JSON.
	protected static ref array<string> s_SoundSets;

	protected static void EnsureInit()
	{
		if (s_SoundSets)
			return;

		s_SoundSets = new array<string>;
		s_SoundSets.Insert("SFP_MozgoTras_PsiAura_Soundset");
		s_SoundSets.Insert("SFP_MozgoTras_PsiAuraGuaa_Soundset");
	}

	static int GetId(string soundSet)
	{
		EnsureInit();
		int idx = s_SoundSets.Find(soundSet);
		if (idx < 0)
			idx = 0;
		return idx;
	}

	static string GetName(int id)
	{
		EnsureInit();
		if (id < 0 || id >= s_SoundSets.Count())
			return s_SoundSets[0];
		return s_SoundSets[id];
	}
};


class SFP_MozgoTrasConfigManager
{
	protected static ref map<string, ref SFP_MozgoTrasConfig> s_Cache;
	protected static const string PROFILE_DIR = "$profile:SFP";

	protected static void EnsureCache()
	{
		if (!s_Cache)
			s_Cache = new map<string, ref SFP_MozgoTrasConfig>;
	}

	protected static void EnsureDir()
	{
		if (!FileExist(PROFILE_DIR))
			MakeDirectory(PROFILE_DIR);
	}

	protected static ref SFP_MozgoTrasConfig MakeDefaults(string className)
	{
		ref SFP_MozgoTrasConfig cfg = new SFP_MozgoTrasConfig;

		// Defaults are the old hard-coded values per class.
		if (className == "SFP_MozgoTras_red")
		{
			cfg.AuraCalmRadius  = 90.0;
			cfg.AuraAggroRadius = 70.0;
			cfg.DamagePerSecond = 3.0;
			cfg.StaminaDT       = 0.3;
			cfg.VomitChance     = 0.15;
			cfg.DropItemChance  = 0.25;
		}
		else // SFP_MozgoTras_Base (and any future derived types fall back here)
		{
			cfg.AuraCalmRadius  = 70.0;
			cfg.AuraAggroRadius = 50.0;
			cfg.DamagePerSecond = 2.0;
			cfg.StaminaDT       = 0.1;
			cfg.VomitChance     = 0.10;
			cfg.DropItemChance  = 0.20;
		}

		return cfg;
	}

	static ref SFP_MozgoTrasConfig Get(string className)
	{
		EnsureCache();
		ref SFP_MozgoTrasConfig cached;
		if (s_Cache.Find(className, cached))
			return cached;

		EnsureDir();

		string filePath = PROFILE_DIR + "/" + className + ".json";
		ref SFP_MozgoTrasConfig cfg = MakeDefaults(className);

		if (FileExist(filePath))
		{
			// Load & keep defaults for any missing fields
			JsonFileLoader<SFP_MozgoTrasConfig>.JsonLoadFile(filePath, cfg);
		}

		// Always write back (creates file on first run + fills missing fields)
		JsonFileLoader<SFP_MozgoTrasConfig>.JsonSaveFile(filePath, cfg);

		s_Cache.Insert(className, cfg);
		return cfg;
	}
};
