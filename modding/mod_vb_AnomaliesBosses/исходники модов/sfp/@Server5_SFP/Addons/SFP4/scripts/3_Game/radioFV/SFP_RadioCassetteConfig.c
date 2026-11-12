// ============================================================
//  SFP RADIO / CASSETTE JSON CONFIG
//
//  Назначение:
//   - Настройка кассет и списка треков (SoundSet + отображаемое имя)
//   - Ручная длительность трека (в секундах), чтобы радио автоматически
//     переключалось на следующий трек, а не крутило один и тот же по кругу
//
//  ВАЖНО (мультиплеер):
//   - Этот конфиг читается ТОЛЬКО НА СЕРВЕРЕ.
//   - Сервер отправляет ТЕКУЩИЙ SoundSet клиентам через RPC.
//   - Клиентам не нужно иметь этот JSON, чтобы правильно проигрывать звук.
//
//  Путь:
//   $profile:SFP/Radio/SFP_RadioConfig.json
// ============================================================

// Мини-логгер без зависимости от других файлов.
// Пишем только ошибки, чтобы не засорять лог.
static void SFP_RadioCfgError(string msg)
{
	Print("[SFP_RadioConfig] " + msg);
}

class SFP_RadioTrackCfg
{
	string SoundSet = string.Empty;       // Имя SoundSet (CfgSoundSets)
	string Name     = string.Empty;       // Имя трека (показывается игроку)
	float  DurationSeconds = 0.0;         // Длительность в секундах. 0 = не переключать автоматически (будет играть по кругу)
}

class SFP_RadioCassetteCfg
{
	string CassetteClassName   = string.Empty;   // Classname предмета кассеты (например "SFP_radio_cassette_01")
	string CassetteDisplayName = string.Empty;   // Необязательно (для удобства в JSON)

	ref array<ref SFP_RadioTrackCfg> Tracks;

	void SFP_RadioCassetteCfg()
	{
		Tracks = new array<ref SFP_RadioTrackCfg>;
	}
}

class SFP_RadioConfig
{
	// Глобальный переключатель: если false — радио никогда не будет автопереключаться.
	bool AutoNextTrackEnabled = true;

	// Защита: если DurationSeconds слишком маленький — игнорируем автопереключение, чтобы не спамить переключениями.
	float MinDurationSeconds = 5.0;

	// ВАЖНО: треки проигрываются как loop-эффект (в оригинальной логике радио),
	// поэтому при сетевых задержках трек может "успеть" начать новый круг.
	// Этот параметр позволяет переключать трек ЧУТЬ РАНЬШЕ конца (в секундах),
	// чтобы не было слышно, как он стартует с начала.
	float TrackSwitchLeadSeconds = 0.35;

	// Комментарии (JSON не поддерживает комментарии, поэтому храним подсказки строками)
	string _comment_File     = "ФАЙЛ КОНФИГА: $profile:SFP/Radio/SFP_RadioConfig.json (читается/пишется сервером).";
	string _comment_Overall  = "ГЛОБАЛЬНАЯ ЛОГИКА: AutoNextTrackEnabled=true заставляет радио ПЕРЕКЛЮЧАТЬСЯ на следующий трек, когда заканчивается DurationSeconds у текущего трека.";
	string _comment_Duration = "DurationSeconds: длительность трека в СЕКУНДАХ. Пример 3:37 = 217. Если 0 — автопереключение отключено и трек будет играть по кругу.";
	// ВАЖНО: оставляем пустым, чтобы ApplyCommentDefaults() гарантированно
	// пересохранил существующий JSON и добавил новые поля (включая TrackSwitchLeadSeconds).
	string _comment_Lead     = "";
	string _comment_Name     = "Name: название трека, которое будет показываться игроку (например при переключении трека).";
	string _comment_Adding   = "КАК ДОБАВИТЬ СВОЮ КАССЕТУ: 1) в config.cpp своего мода создай предмет, наследующийся от SFP_Cassette_Config; 2) добавь запись в SFPCassettes[] (CassetteClassName) и список Tracks[]. Каждый Track: SoundSet + Name + DurationSeconds.";

	ref array<ref SFP_RadioCassetteCfg> SFPCassettes;

	// files
	static const string DIR  = "$profile:SFP/Radio";
	static const string PATH = "$profile:SFP/Radio/SFP_RadioConfig.json";

	private static ref SFP_RadioConfig s_Instance;

	void SFP_RadioConfig()
	{
		SFPCassettes = new array<ref SFP_RadioCassetteCfg>;
	}

	static SFP_RadioConfig Get()
	{
		if (!s_Instance)
			Load();
		return s_Instance;
	}

	static void Load()
	{
		if (!FileExist(DIR))
			MakeDirectory(DIR);

		s_Instance = new SFP_RadioConfig();
		string err;

		if (FileExist(PATH))
		{
			if (!JsonFileLoader<SFP_RadioConfig>.LoadFile(PATH, s_Instance, err))
			{
				SFP_RadioCfgError("Load failed: " + err);
				string backup = PATH + ".broken";
				CopyFile(PATH, backup);
				JsonFileLoader<SFP_RadioConfig>.SaveFile(PATH, s_Instance, err);
			}
			else
			{
				// Обновляем подсказки/комментарии на русском даже в уже существующем JSON.
				// (На логику это не влияет, но админам будет понятнее.)
				if (ApplyCommentDefaults(s_Instance))
					JsonFileLoader<SFP_RadioConfig>.SaveFile(PATH, s_Instance, err);
			}
		}
		else
		{
			// Create defaults (names from current list; durations = 0 by default)
			BuildDefault(s_Instance);
			if (!JsonFileLoader<SFP_RadioConfig>.SaveFile(PATH, s_Instance, err))
				SFP_RadioCfgError("Save failed: " + err);
		}
	}

	// Возвращает true, если что-то поменялось.
	static bool ApplyCommentDefaults(SFP_RadioConfig cfg)
	{
		if (!cfg)
			return false;

		bool changed = false;

		string cFile     = "ФАЙЛ КОНФИГА: $profile:SFP/Radio/SFP_RadioConfig.json (читается/пишется сервером).";
		string cOverall  = "ГЛОБАЛЬНАЯ ЛОГИКА: AutoNextTrackEnabled=true заставляет радио ПЕРЕКЛЮЧАТЬСЯ на следующий трек, когда заканчивается DurationSeconds у текущего трека.";
		string cDuration = "DurationSeconds: длительность трека в СЕКУНДАХ. Пример 3:37 = 217. Если 0 — автопереключение отключено и трек будет играть по кругу.";
		string cLead     = "TrackSwitchLeadSeconds: переключать трек на следующий ЧУТЬ РАНЬШЕ конца (например 0.35 сек), чтобы из-за задержек трек не успевал стартовать новый круг. 0 = строго по времени.";
		string cName     = "Name: название трека, которое будет показываться игроку (например при переключении трека).";
		string cAdding   = "КАК ДОБАВИТЬ СВОЮ КАССЕТУ: 1) в config.cpp своего мода создай предмет, наследующийся от SFP_Cassette_Config; 2) добавь запись в SFPCassettes[] (CassetteClassName) и список Tracks[]. Каждый Track: SoundSet + Name + DurationSeconds.";

		if (cfg._comment_File != cFile)      { cfg._comment_File = cFile; changed = true; }
		if (cfg._comment_Overall != cOverall){ cfg._comment_Overall = cOverall; changed = true; }
		if (cfg._comment_Duration != cDuration){ cfg._comment_Duration = cDuration; changed = true; }
		if (cfg._comment_Lead != cLead)      { cfg._comment_Lead = cLead; changed = true; }
		if (cfg._comment_Name != cName)      { cfg._comment_Name = cName; changed = true; }
		if (cfg._comment_Adding != cAdding)  { cfg._comment_Adding = cAdding; changed = true; }

		return changed;
	}

	static void BuildDefault(SFP_RadioConfig cfg)
	{
		if (!cfg)
			return;

		cfg.SFPCassettes.Clear();

		// Cassette 01
		ref SFP_RadioCassetteCfg c1 = new SFP_RadioCassetteCfg();
		c1.CassetteClassName = "SFP_radio_cassette_01";
		c1.CassetteDisplayName = "Кассета 01";
		AddTrack(c1, "SFP_Cassette01_Prayers_Tears_In_The_Rain_SoundSet", "Prayers Tears In The Rain", 214.0);
		AddTrack(c1, "SFP_Cassette01_Oddnesse_Summers_Almost_Over_SoundSet", "Oddnesse Summers Almost Over", 162.0);
		AddTrack(c1, "SFP_Cassette01_Oddnesse_Somewhere_Somehow_SoundSet", "Oddnesse Somewhere Somehow", 164.0);
		AddTrack(c1, "SFP_Cassette01_Giorgio_Moroder_Right_Combination_SoundSet", "Giorgio_Moroder_Right_Combination", 221.0);
		AddTrack(c1, "SFP_Cassette01_Rammstein_Sonne_SoundSet", "Rammstein_Sonne", 272.0);
		AddTrack(c1, "SFP_Cassette01_Singers_Edge_Fire_SoundSet", "Singers_Edge_Fire", 248.0);
		cfg.SFPCassettes.Insert(c1);

		// Cassette 02
		ref SFP_RadioCassetteCfg c2 = new SFP_RadioCassetteCfg();
		c2.CassetteClassName = "SFP_radio_cassette_02";
		c2.CassetteDisplayName = "Кассета 02";
		AddTrack(c2, "SFP_Cassette02_Evalyn_A_Pill_to_Crush_SoundSet", "Evalyn A Pill to Crush", 221.0);
		AddTrack(c2, "SFP_Cassette02_Nick_Rezo_Melllo_In_This_Shirt_SoundSet", "Nick Rezo Melllo In his Shirt", 300.0);
		AddTrack(c2, "SFP_Cassette02_Night_Club_Civil_War_SoundSet", "Night Club Civil War", 244.0);
		AddTrack(c2, "SFP_Cassette02_ONEIL_KANVISE_FAVIA_ORGAN_Shut_Your_Mouth_SoundSet", "ONEIL KANVISE FAVIA ORGAN Shut Your Mouth", 175.0);
		AddTrack(c2, "SFP_Cassette02_The_Dumplings_Frank_SoundSet", "The Dumplings Frank", 196.0);
		AddTrack(c2, "SFP_Cassette02_Up_The_RIvals_Name_Me_SoundSet", "Up The RIvals Name Me", 120.0);
		cfg.SFPCassettes.Insert(c2);

		// Cassette 03
		ref SFP_RadioCassetteCfg c3 = new SFP_RadioCassetteCfg();
		c3.CassetteClassName = "SFP_radio_cassette_03";
		c3.CassetteDisplayName = "Кассета 03";
		AddTrack(c3, "SFP_Cassette03_7B_Molodie_Vetra_SoundSet", "7B Molodie Vetra", 265.0);
		AddTrack(c3, "SFP_Cassette03_Kola_Komarov_Mne_Povezet_SoundSet", "Kola Komarov Mne Povezet", 185.0);
		AddTrack(c3, "SFP_Cassette03_Kombine_Bugalter_SoundSet", "Kombine Bugalter", 270.0);
		AddTrack(c3, "SFP_Cassette03_Kombine_Pupuri_SoundSet", "Kombine Pupuri", 463.0);
		AddTrack(c3, "SFP_Cassette03_Komisare_You_Exit_SoundSet", "Komisare You Exit", 303.0);
		AddTrack(c3, "SFP_Cassette03_Olga_Voskoniyan_Gold_Obruc_SoundSet", "Olga Voskoniyan Gold Obruc", 325.0);
		AddTrack(c3, "SFP_Cassette03_Nastavshevs_Lubennikov_Nenaste_SoundSet", "Nastavshevs_Lubennikov_Nenaste", 360.0);
		cfg.SFPCassettes.Insert(c3);

		// Cassette 04 (empty placeholder, fill in JSON if you use it)
		ref SFP_RadioCassetteCfg c4 = new SFP_RadioCassetteCfg();
		c4.CassetteClassName = "SFP_radio_cassette_04";
		c4.CassetteDisplayName = "Кассета 04";
		cfg.SFPCassettes.Insert(c4);

		// Clear Sky Bar
		ref SFP_RadioCassetteCfg cs = new SFP_RadioCassetteCfg();
		cs.CassetteClassName = "SFP_radio_cassette_ClearSkyBar";
		cs.CassetteDisplayName = "Clear Sky Bar";
		AddTrack(cs, "SFP_RadioPlayHolodBar_SoundSet", "Holod bar theme", 0.0);
		cfg.SFPCassettes.Insert(cs);
	}

	static void AddTrack(SFP_RadioCassetteCfg cassette, string soundSet, string name, float dur)
	{
		if (!cassette)
			return;
		ref SFP_RadioTrackCfg t = new SFP_RadioTrackCfg();
		t.SoundSet = soundSet;
		t.Name = name;
		t.DurationSeconds = dur;
		cassette.Tracks.Insert(t);
	}

	// ------------------------
	//  Lookup helpers
	// ------------------------
	SFP_RadioCassetteCfg GetCassette(string cassetteType)
	{
		if (!cassetteType || cassetteType == string.Empty)
			return null;

		foreach (SFP_RadioCassetteCfg c : SFPCassettes)
		{
			if (c && c.CassetteClassName == cassetteType)
				return c;
		}
		return null;
	}

	int GetTrackCount(string cassetteType)
	{
		SFP_RadioCassetteCfg c = GetCassette(cassetteType);
		if (!c || !c.Tracks)
			return 0;
		return c.Tracks.Count();
	}

	SFP_RadioTrackCfg GetTrack(string cassetteType, int index)
	{
		SFP_RadioCassetteCfg c = GetCassette(cassetteType);
		if (!c || !c.Tracks)
			return null;

		if (index < 0 || index >= c.Tracks.Count())
			return null;

		return c.Tracks.Get(index);
	}
}
