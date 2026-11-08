// INT GlobalChat (server) — the three files (spec.md §4.1, §4.2). Read once at server start: no timers, no file
// watching (owner rule 2), so hand edits take effect after a restart.
//   missing file  -> created with defaults (BannedWords = the owner's list, mutes = one sample entry 111111)
//   unreadable    -> copy .broken-<date>, КРИТИЧНО line; the config is rewritten with defaults, the word list and
//                    the mute list are NOT overwritten (they hold the owner's data) and run on defaults / empty
//   bad values    -> repaired (Sanitize) and saved

class INT_GC_SE_ConfigData
{
	static const int CURRENT_VERSION = 1;

	int ConfigVersion = 1;
	int DebugMode = 0;
	int MaxMessageLength = 200;
	int MinSecondsBetweenMessages = 2;
	int SameMessageCooldownSeconds = 10;
	int StrikesForPermanentMute = 3;
	int ShowJoinLeave = 1;
	ref array<string> Admins;
	ref array<string> VanillaCommandPrefixes;

	void INT_GC_SE_ConfigData()
	{
		Admins = new array<string>();
		VanillaCommandPrefixes = new array<string>();
	}

	// A new file: the commands that must reach vanilla (admin "#", RF_PROMOCODES "@fpromo").
	static INT_GC_SE_ConfigData Defaults()
	{
		INT_GC_SE_ConfigData d = new INT_GC_SE_ConfigData();
		d.VanillaCommandPrefixes.Insert("#");
		d.VanillaCommandPrefixes.Insert("@fpromo");
		return d;
	}

	protected int Bool01(int v)
	{
		if (v != 0)
			return 1;
		return 0;
	}

	int Sanitize()
	{
		int fixes = 0;
		if (ConfigVersion != CURRENT_VERSION)
		{
			ConfigVersion = CURRENT_VERSION;
			fixes++;
		}
		int b = Bool01(DebugMode);
		if (b != DebugMode)
		{
			DebugMode = b;
			fixes++;
		}
		b = Bool01(ShowJoinLeave);
		if (b != ShowJoinLeave)
		{
			ShowJoinLeave = b;
			fixes++;
		}
		int c = Math.Clamp(MaxMessageLength, 20, 500);
		if (c != MaxMessageLength)
		{
			MaxMessageLength = c;
			fixes++;
		}
		c = Math.Clamp(MinSecondsBetweenMessages, 0, 60);
		if (c != MinSecondsBetweenMessages)
		{
			MinSecondsBetweenMessages = c;
			fixes++;
		}
		c = Math.Clamp(SameMessageCooldownSeconds, 0, 600);
		if (c != SameMessageCooldownSeconds)
		{
			SameMessageCooldownSeconds = c;
			fixes++;
		}
		c = Math.Clamp(StrikesForPermanentMute, 1, 10);
		if (c != StrikesForPermanentMute)
		{
			StrikesForPermanentMute = c;
			fixes++;
		}
		if (!Admins)
		{
			Admins = new array<string>();
			fixes++;
		}
		array<string> admins = new array<string>();
		foreach (string a : Admins)
		{
			string id = a + "";
			id.TrimInPlace();
			if (id == "" || admins.Find(id) >= 0)
			{
				fixes++;
				continue;
			}
			if (id != a)
				fixes++;
			admins.Insert(id);
		}
		Admins = admins;
		if (!VanillaCommandPrefixes)
		{
			VanillaCommandPrefixes = new array<string>();
			fixes++;
		}
		array<string> pre = new array<string>();
		foreach (string p : VanillaCommandPrefixes)
		{
			string q = p + "";
			q.TrimInPlace();
			if (q == "" || q.Length() > 24 || pre.Find(q) >= 0 || pre.Count() >= INT_GC_Const.MAX_PREFIXES)
			{
				fixes++;
				continue;
			}
			if (q != p)
				fixes++;
			pre.Insert(q);
		}
		VanillaCommandPrefixes = pre;
		return fixes;
	}
}

class INT_GC_SE_WordsData
{
	string Comment = "Запретные слова чата: по одному в кавычках через запятую. Сравнение целым словом без учёта регистра, ё=е, латинские двойники и повторы букв учитываются. Изменения вступают в силу после рестарта сервера.";
	ref array<string> Words;

	void INT_GC_SE_WordsData()
	{
		Words = new array<string>();
	}

	// The owner's list from the brief (2026-10-20), duplicates removed.
	void FillDefaults()
	{
		Words.Clear();
		string src = "Отца,Отцу,Папе,Папу,Папашку,Мамка,Мамаша,Мамашку,Мамаше,Мамочка,Мамку,Мать,Маму,Маме,Безматерие,Отец,Дедушка,Батя,Матерей,Мамок,Папок,Безотцовщина,Папенькин,Потомок,Папочка,Сын,Сыночек";
		AddCsv(src);
		src = "Ниггер,Хохол,Чурка,Чурки,Москаль,Хач,Черножопый,Чернильник,Кацапы,Кацап,Исус,Иисус,Аллах,Mother,Father,Motherfucker,Fatherfucker,Гитлер,Нацист,Нацыст,Нацик,Господь";
		AddCsv(src);
		src = "Лесбиянка,Гей,Сынок,Сына,Сестра,mom,mama,mommy,мамой,Sister,Sisters,выблядок,выродки,выродок,породили,породила,породил";
		AddCsv(src);
	}

	protected void AddCsv(string csv)
	{
		array<string> parts = new array<string>();
		csv.Split(",", parts);
		foreach (string p : parts)
		{
			if (p != "" && Words.Find(p) < 0)
				Words.Insert(p);
		}
	}

	int Sanitize()
	{
		int fixes = 0;
		if (!Words)
		{
			Words = new array<string>();
			fixes++;
		}
		array<string> clean = new array<string>();
		foreach (string w : Words)
		{
			string t = INT_GC_Util.CleanText(w, 64);
			if (t == "" || clean.Find(t) >= 0 || clean.Count() >= INT_GC_Const.MAX_WORDS)
			{
				fixes++;
				continue;
			}
			if (t != w)
				fixes++;
			clean.Insert(t);
		}
		Words = clean;
		return fixes;
	}
}

class INT_GC_SE_MuteEntry
{
	string SteamId;
	string Name;
	string Reason;
	string Date;
}

class INT_GC_SE_MutesData
{
	static const string SAMPLE_ID = "111111";

	string Comment = "Игроки с перманентным баном чата. Одна запись = SteamId (SteamID64), Name (ник для себя), Reason, Date. Образец 111111 ни на кого не действует. Снять бан: удалить запись и перезапустить сервер.";
	ref array<ref INT_GC_SE_MuteEntry> Players;

	void INT_GC_SE_MutesData()
	{
		Players = new array<ref INT_GC_SE_MuteEntry>();
	}

	void FillSample()
	{
		Players.Clear();
		INT_GC_SE_MuteEntry e = new INT_GC_SE_MuteEntry();
		e.SteamId = SAMPLE_ID;
		e.Name = "Образец";
		e.Reason = "пример записи: так выглядит перманентный бан чата";
		e.Date = INT_GC_Util.Stamp();
		Players.Insert(e);
	}

	int Sanitize()
	{
		int fixes = 0;
		if (!Players)
		{
			Players = new array<ref INT_GC_SE_MuteEntry>();
			fixes++;
		}
		array<string> seen = new array<string>();
		for (int i = Players.Count() - 1; i >= 0; i--)
		{
			INT_GC_SE_MuteEntry e = Players.Get(i);
			string id = "";
			if (e)
			{
				id = e.SteamId + "";
				id.TrimInPlace();
			}
			if (id == "" || seen.Find(id) >= 0)
			{
				Players.RemoveOrdered(i);
				fixes++;
				continue;
			}
			if (id != e.SteamId)
			{
				e.SteamId = id;
				fixes++;
			}
			seen.Insert(id);
		}
		return fixes;
	}

	int Find(string steamId)
	{
		for (int i = 0; i < Players.Count(); i++)
		{
			INT_GC_SE_MuteEntry e = Players.Get(i);
			if (e && e.SteamId == steamId)
				return i;
		}
		return -1;
	}
}

class INT_GC_SE_Config
{
	protected static ref INT_GC_SE_ConfigData s_Data;
	protected static ref array<string> s_Words;
	protected static ref map<string, bool> s_Muted;		// SteamID64 -> true
	protected static ref map<string, bool> s_Admins;

	static INT_GC_SE_ConfigData Data()
	{
		if (!s_Data)
			Load();
		return s_Data;
	}

	static array<string> Words()
	{
		if (!s_Words)
			Load();
		return s_Words;
	}

	static bool IsMuted(string steamId)
	{
		if (!s_Muted || steamId == "")
			return false;
		return s_Muted.Contains(steamId);
	}

	static bool IsAdmin(string steamId)
	{
		if (!s_Admins || steamId == "")
			return false;
		return s_Admins.Contains(steamId);
	}

	static int MutedCount()
	{
		if (!s_Muted)
			return 0;
		return s_Muted.Count();
	}

	static void Load()
	{
		INT_GC_SE_Paths.MakeRoot();
		LoadConfig();
		LoadWords();
		LoadMutes();
		INT_GC_Log.Info("конфиг: DebugMode=" + s_Data.DebugMode.ToString() + " длина=" + s_Data.MaxMessageLength.ToString() + " страйков=" + s_Data.StrikesForPermanentMute.ToString() + " вход/выход=" + s_Data.ShowJoinLeave.ToString());
		INT_GC_Log.Info("слов=" + s_Words.Count().ToString() + " мутов=" + s_Muted.Count().ToString() + " админов=" + s_Admins.Count().ToString() + " префиксов=" + s_Data.VanillaCommandPrefixes.Count().ToString());
	}

	protected static void LoadConfig()
	{
		string path = INT_GC_SE_Paths.ConfigFile();
		INT_GC_SE_ConfigData d = new INT_GC_SE_ConfigData();
		bool save = false;
		string txt;
		if (!INT_GC_SE_Files.ReadText(path, txt))
		{
			d = INT_GC_SE_ConfigData.Defaults();
			save = true;
			INT_GC_Log.Info("создан конфиг по умолчанию: " + path);
		}
		else
		{
			string err;
			if (!INT_GC_SE_Json<INT_GC_SE_ConfigData>.Parse(txt, d, err))
			{
				string kept = INT_GC_SE_Files.KeepBroken(path);
				INT_GC_Log.Crit("конфиг не читается: " + err + "; работаю на значениях по умолчанию; копия: " + kept);
				d = INT_GC_SE_ConfigData.Defaults();
				save = true;
			}
		}
		int fixes = d.Sanitize();
		if (fixes > 0 && !save)
			INT_GC_Log.Warn("конфиг исправлен/дополнен, правок: " + fixes.ToString());
		if (fixes > 0)
			save = true;
		s_Data = d;
		if (save)
			INT_GC_SE_Json<INT_GC_SE_ConfigData>.Save(path, d);
		INT_GC_Log.s_Debug = d.DebugMode == 1;
		s_Admins = new map<string, bool>();
		foreach (string a : d.Admins)
			s_Admins.Set(a, true);
	}

	protected static void LoadWords()
	{
		string path = INT_GC_SE_Paths.WordsFile();
		INT_GC_SE_WordsData w = new INT_GC_SE_WordsData();
		string txt;
		if (!INT_GC_SE_Files.ReadText(path, txt))
		{
			w.FillDefaults();
			w.Sanitize();
			INT_GC_SE_Json<INT_GC_SE_WordsData>.Save(path, w);
			INT_GC_Log.Info("создан список запретных слов по умолчанию: " + path);
		}
		else
		{
			string err;
			if (!INT_GC_SE_Json<INT_GC_SE_WordsData>.Parse(txt, w, err))
			{
				string kept = INT_GC_SE_Files.KeepBroken(path);
				INT_GC_Log.Crit("список слов не читается: " + err + "; работаю на списке по умолчанию, файл НЕ перезаписан; копия: " + kept);
				w = new INT_GC_SE_WordsData();
				w.FillDefaults();
				w.Sanitize();
			}
			else
			{
				int fixes = w.Sanitize();
				if (fixes > 0)
				{
					INT_GC_Log.Warn("список слов: убрано пустых/повторов: " + fixes.ToString());
					INT_GC_SE_Json<INT_GC_SE_WordsData>.Save(path, w);
				}
			}
		}
		s_Words = w.Words;
	}

	protected static void LoadMutes()
	{
		s_Muted = new map<string, bool>();
		INT_GC_SE_MutesData m = ReadMutes(true);
		foreach (INT_GC_SE_MuteEntry e : m.Players)
		{
			if (e.SteamId != INT_GC_SE_MutesData.SAMPLE_ID)
				s_Muted.Set(e.SteamId, true);
		}
	}

	// Reads the mute file. createIfMissing: write the sample file when there is none.
	protected static INT_GC_SE_MutesData ReadMutes(bool createIfMissing)
	{
		string path = INT_GC_SE_Paths.MutesFile();
		INT_GC_SE_MutesData m = new INT_GC_SE_MutesData();
		string txt;
		if (!INT_GC_SE_Files.ReadText(path, txt))
		{
			m.FillSample();
			if (createIfMissing)
			{
				INT_GC_SE_Json<INT_GC_SE_MutesData>.Save(path, m);
				INT_GC_Log.Info("создан файл мутов с образцом 111111: " + path);
			}
			return m;
		}
		string err;
		if (!INT_GC_SE_Json<INT_GC_SE_MutesData>.Parse(txt, m, err))
		{
			string kept = INT_GC_SE_Files.KeepBroken(path);
			INT_GC_Log.Crit("файл мутов не читается: " + err + "; копия: " + kept);
			m = new INT_GC_SE_MutesData();
			return m;
		}
		int fixes = m.Sanitize();
		if (fixes > 0)
			INT_GC_Log.Warn("файл мутов: убрано пустых/повторов: " + fixes.ToString());
		return m;
	}

	// Permanent chat ban: memory now, then the file is re-read and the new entry appended (the owner may have edited it).
	static bool AddMute(string steamId, string name, string reason)
	{
		if (steamId == "")
			return false;
		if (!s_Muted)
			s_Muted = new map<string, bool>();
		s_Muted.Set(steamId, true);
		// only the new entry is added: what the owner changed on disk meanwhile stays as it is
		INT_GC_SE_MutesData m = ReadMutes(false);
		if (m.Find(steamId) >= 0)
			return true;
		INT_GC_SE_MuteEntry e = new INT_GC_SE_MuteEntry();
		e.SteamId = steamId;
		e.Name = INT_GC_Util.CleanText(name, 64);
		e.Reason = reason;
		e.Date = INT_GC_Util.Stamp();
		m.Players.Insert(e);
		bool ok = INT_GC_SE_Json<INT_GC_SE_MutesData>.Save(INT_GC_SE_Paths.MutesFile(), m);
		if (!ok)
			INT_GC_Log.Crit("мут записан только в память (файл не сохранён): " + steamId);
		return ok;
	}

	// Self-test only: reload everything from the current (sandbox or real) folder.
	static void Reload()
	{
		s_Data = null;
		s_Words = null;
		s_Muted = null;
		Load();
	}
}
