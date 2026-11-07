// INT_PDA (server) — everything the server keeps (spec.md §6). Read once at mission start: config, factions (one
// small file each), relations, spawn sets, chat history, news. Accounts are read only when needed (HELLO of that
// player, an admin request) and cached while the player is online. Writes happen only on events (registration, a
// change, every Nth chat line, shutdown) — no timers, no scans (owner rules 2, 3). A broken file is never
// overwritten: it is copied to *.broken-<time> and the log says so.

class INT_PDA_SE_Store
{
	static ref INT_PDA_SE_Config s_Cfg;
	static string s_Source;			// how the config came: "file" / "created" / "broken" (self-test, log)
	static ref map<string, ref INT_PDA_FactionData> s_Factions = new map<string, ref INT_PDA_FactionData>();
	static ref INT_PDA_RelationsFile s_Relations;
	static ref map<string, ref INT_PDA_SpawnSet> s_Spawn = new map<string, ref INT_PDA_SpawnSet>();
	static ref INT_PDA_ChatFile s_Public;
	static int s_PublicUnsaved;
	static ref map<string, ref INT_PDA_ChatFile> s_FactionChat = new map<string, ref INT_PDA_ChatFile>();
	static ref map<string, int> s_FactionUnsaved = new map<string, int>();
	static ref INT_PDA_NewsFile s_News;
	static ref map<string, ref INT_PDA_Account> s_Accounts = new map<string, ref INT_PDA_Account>();
	protected static ref set<string> s_BrokenSeen = new set<string>();		// unreadable account / name files already reported
	static const string OWNER_UNREADABLE = "?";							// NameOwner of a broken index file: taken, owner unknown

	static const string SPAWN_DEFAULT = "Default";

	static INT_PDA_SE_Config Cfg()
	{
		if (!s_Cfg)
		{
			s_Cfg = INT_PDA_SE_Config.Defaults();
			s_Cfg.Sanitize();
		}
		return s_Cfg;
	}

	static void LoadAll()
	{
		INT_PDA_SE_Paths.MakeRoot();
		RecoverInterrupted();
		LoadConfig();
		LoadFactions();
		LoadRelations();
		LoadSpawn();
		LoadChat();
		LoadNews();
		s_Accounts.Clear();
		s_BrokenSeen.Clear();
	}

	// Mission start: files whose last write was cut off come back from .tmp / .bak (INT_PDA_SE_Files.RecoverDir).
	protected static void RecoverInterrupted()
	{
		int n = INT_PDA_SE_Files.RecoverDir(INT_PDA_SE_Paths.Root());
		n = n + INT_PDA_SE_Files.RecoverDir(INT_PDA_SE_Paths.FactionsDir());
		n = n + INT_PDA_SE_Files.RecoverDir(INT_PDA_SE_Paths.GpDir());
		n = n + INT_PDA_SE_Files.RecoverDir(INT_PDA_SE_Paths.PlayersDir());
		n = n + INT_PDA_SE_Files.RecoverDir(INT_PDA_SE_Paths.NamesDir());
		n = n + INT_PDA_SE_Files.RecoverDir(INT_PDA_SE_Paths.SpawnDir());
		n = n + INT_PDA_SE_Files.RecoverDir(INT_PDA_SE_Paths.ChatDir());
		n = n + INT_PDA_SE_Files.RecoverDir(INT_PDA_SE_Paths.MailDir());
		if (n > 0)
			INT_PDA_Log.Warn("восстановлено файлов после прерванной записи: " + n.ToString());
	}

	// Mission end: unsaved chat history goes to disk once.
	static void Shutdown()
	{
		if (s_PublicUnsaved > 0)
			SavePublic();
		foreach (string id, int n : s_FactionUnsaved)
		{
			if (n > 0)
				SaveFactionChat(id);
		}
	}

	// --- config ---------------------------------------------------------------------------------------------------
	static void LoadConfig()
	{
		string path = INT_PDA_SE_Paths.ConfigFile();
		string txt;
		string err;
		if (FileExist(path))
		{
			INT_PDA_SE_Config cfg = INT_PDA_SE_Config.ForRead();
			bool parsed = false;
			if (INT_PDA_SE_Files.ReadText(path, txt))
				parsed = INT_PDA_SE_Json<INT_PDA_SE_Config>.Parse(txt, cfg, err);
			if (!parsed)
			{
				Broken(path, err);
				s_Cfg = INT_PDA_SE_Config.Defaults();
				s_Cfg.Sanitize();
				s_Source = "broken";
				return;
			}
			// keys added after the first release: the reader gives 0 to a missing key, 0 is a valid choice here
			if (txt.IndexOf("\"CallsignReserveDays\"") < 0)
				cfg.CallsignReserveDays = 7;
			if (txt.IndexOf("\"CharacterSelect\"") < 0)
				cfg.CharacterSelect = 1;
			if (txt.IndexOf("\"CharacterRespawnMinutes\"") < 0)
				cfg.CharacterRespawnMinutes = 10;
			int fixes = cfg.Sanitize();
			if (fixes > 0)
				INT_PDA_Log.Warn("конфиг: исправлено значений вне диапазона: " + fixes.ToString());
			s_Cfg = cfg;
			s_Source = "file";
			RewriteIfDifferent(path, txt);
			return;
		}
		s_Cfg = INT_PDA_SE_Config.Defaults();
		s_Cfg.Sanitize();
		s_Source = "created";
		if (INT_PDA_SE_Json<INT_PDA_SE_Config>.Save(path, s_Cfg))
			INT_PDA_Log.Info("создан конфиг по умолчанию: " + path);
		else
			INT_PDA_Log.Err("не удалось создать конфиг: " + path);
	}

	static void SaveConfig()
	{
		INT_PDA_SE_Json<INT_PDA_SE_Config>.Save(INT_PDA_SE_Paths.ConfigFile(), Cfg());
	}

	protected static void RewriteIfDifferent(string path, string oldTxt)
	{
		string norm;
		if (!INT_PDA_SE_Json<INT_PDA_SE_Config>.ToText(s_Cfg, true, norm))
			return;
		if (INT_PDA_SE_Files.SameContent(norm, oldTxt))
			return;
		if (INT_PDA_SE_Files.SaveText(path, norm))
			INT_PDA_Log.Info("конфиг дополнен/нормализован");
	}

	protected static void Broken(string path, string err)
	{
		string kept = INT_PDA_SE_Files.KeepBroken(path);
		INT_PDA_Log.Err("файл не читается, работают значения по умолчанию (файл не перезаписан): " + path);
		INT_PDA_Log.Err("причина: " + INT_PDA_Log.Clip(err, 160));
		if (kept != "")
			INT_PDA_Log.Err("копия битого файла: " + kept);
	}

	// --- accounts -------------------------------------------------------------------------------------------------
	// Cached account or the file (GP first: a faction member). null = not registered (or broken: logged).
	static INT_PDA_Account Account(string steam)
	{
		if (steam == "")
			return null;
		INT_PDA_Account a;
		if (s_Accounts.Find(steam, a))
			return a;
		a = ReadAccount(steam);
		if (a)
			s_Accounts.Set(steam, a);
		return a;
	}

	// The cached account, else the file without caching (a spawn before the player's HELLO: a cache entry made there
	// would outlive a player who leaves before he ever has a session).
	static INT_PDA_Account PeekAccount(string steam)
	{
		INT_PDA_Account a;
		if (s_Accounts.Find(steam, a))
			return a;
		return ReadAccount(steam);
	}

	// Read without caching (admin lists, offline players).
	static INT_PDA_Account ReadAccount(string steam)
	{
		if (!INT_PDA_Util.IsSteamId(steam))
			return null;
		string path = INT_PDA_SE_Paths.AccountFile(steam, true);
		if (!FileExist(path))
			path = INT_PDA_SE_Paths.AccountFile(steam, false);
		if (!FileExist(path))
			return null;
		INT_PDA_Account a = INT_PDA_Account.ForRead();
		string err;
		if (!INT_PDA_SE_Json<INT_PDA_Account>.Load(path, a, err))
		{
			// copied aside and logged once per file (every HELLO / login / admin page would repeat it); the file
			// stays, so the account can't be re-registered by accident — an admin deletes it by SteamID
			if (s_BrokenSeen.Find(path) < 0)
			{
				s_BrokenSeen.Insert(path);
				Broken(path, err);
			}
			return null;
		}
		a.Clean();
		if (a.SteamId != steam)
			a.SteamId = steam;
		return a;
	}

	static bool HasAccount(string steam)
	{
		if (s_Accounts.Contains(steam))
			return true;
		return FileExist(INT_PDA_SE_Paths.AccountFile(steam, true)) || FileExist(INT_PDA_SE_Paths.AccountFile(steam, false));
	}

	static bool IsFactionMemberFile(string steam)
	{
		return FileExist(INT_PDA_SE_Paths.AccountFile(steam, true));
	}

	// Writes into GP/ or Players/ by the faction state and removes the copy in the other folder.
	static bool SaveAccount(INT_PDA_Account a)
	{
		if (!a || !INT_PDA_Util.IsSteamId(a.SteamId))
			return false;
		string path = INT_PDA_SE_Paths.AccountFile(a.SteamId, a.InFaction());
		string other = INT_PDA_SE_Paths.AccountFile(a.SteamId, !a.InFaction());
		if (!INT_PDA_SE_Json<INT_PDA_Account>.Save(path, a))
			return false;
		if (FileExist(other))
			INT_PDA_SE_Files.Remove(other);
		return true;
	}

	static void CacheAccount(INT_PDA_Account a)
	{
		if (a)
			s_Accounts.Set(a.SteamId, a);
	}

	static void ForgetAccount(string steam)
	{
		s_Accounts.Remove(steam);
	}

	// Also for an account whose file does not read (the admin frees the SteamID; the .broken copy stays).
	static bool DeleteAccount(string steam)
	{
		if (!INT_PDA_Util.IsSteamId(steam))
			return false;
		INT_PDA_Account a = Account(steam);
		if (!a && !HasAccount(steam))
			return false;
		if (a)
		{
			DropNameOf(a.Callsign, steam);
			// his reserved old callsigns are free again with the account
			foreach (string oldName : a.OldCallsigns)
				DropNameOf(oldName, steam);
		}
		INT_PDA_SE_Files.Remove(INT_PDA_SE_Paths.AccountFile(steam, true));
		INT_PDA_SE_Files.Remove(INT_PDA_SE_Paths.AccountFile(steam, false));
		INT_PDA_SE_Files.Remove(INT_PDA_SE_Paths.MailFile(steam));
		s_Accounts.Remove(steam);
		return true;
	}

	// --- callsign index -------------------------------------------------------------------------------------------
	// SteamID that owns this callsign ("" = free; OWNER_UNREADABLE = the index file is broken: treated as taken, so a
	// damaged file never hands somebody's callsign to another player).
	static string NameOwner(string callsign)
	{
		string path = INT_PDA_SE_Paths.NameFile(INT_PDA_Util.CallsignKey(callsign));
		if (!FileExist(path))
			return "";
		INT_PDA_NameRef r = new INT_PDA_NameRef();
		string err;
		if (!INT_PDA_SE_Json<INT_PDA_NameRef>.Load(path, r, err) || r.SteamId == "")
		{
			if (s_BrokenSeen.Find(path) < 0)
			{
				s_BrokenSeen.Insert(path);
				Broken(path, err);
			}
			return OWNER_UNREADABLE;
		}
		// an old callsign kept for its owner: free again once the reservation is over (the file goes then)
		if (r.ReservedUntil > 0 && r.ReservedUntil <= INT_PDA_Util.NowSec())
		{
			INT_PDA_SE_Files.Remove(path);
			return "";
		}
		return r.SteamId;
	}

	// After a callsign change the old one stays his for CallsignReserveDays (owner 2026-10-23): nobody else takes it,
	// and private messages to it still reach him (an answer to an old letter must not go to a stranger).
	static void ReserveOldName(string callsign, string steam)
	{
		if (callsign == "" || steam == "")
			return;
		if (NameOwner(callsign) != steam)
			return;
		int days = Cfg().CallsignReserveDays;
		if (days <= 0)
		{
			DropName(callsign);
			return;
		}
		INT_PDA_NameRef r = new INT_PDA_NameRef();
		r.SteamId = steam;
		r.Callsign = callsign;
		r.ReservedUntil = INT_PDA_Util.NowSec() + days * 86400;
		INT_PDA_SE_Json<INT_PDA_NameRef>.Save(INT_PDA_SE_Paths.NameFile(INT_PDA_Util.CallsignKey(callsign)), r);
	}

	// Drops the index entry only if it belongs to this SteamID (never somebody else's callsign).
	static void DropNameOf(string callsign, string steam)
	{
		if (callsign == "" || steam == "")
			return;
		if (NameOwner(callsign) != steam)
			return;
		DropName(callsign);
	}

	// The faction whose member list holds this SteamID (an account file that does not read still has its place).
	static INT_PDA_FactionData FactionOfMember(string steam)
	{
		foreach (string id, INT_PDA_FactionData f : s_Factions)
		{
			if (f && f.MemberIndex(steam) >= 0)
				return f;
		}
		return null;
	}

	static bool SetName(string callsign, string steam)
	{
		INT_PDA_NameRef r = new INT_PDA_NameRef();
		r.SteamId = steam;
		r.Callsign = callsign;
		return INT_PDA_SE_Json<INT_PDA_NameRef>.Save(INT_PDA_SE_Paths.NameFile(INT_PDA_Util.CallsignKey(callsign)), r);
	}

	static void DropName(string callsign)
	{
		if (callsign == "")
			return;
		INT_PDA_SE_Files.Remove(INT_PDA_SE_Paths.NameFile(INT_PDA_Util.CallsignKey(callsign)));
	}

	// --- factions -------------------------------------------------------------------------------------------------
	static void LoadFactions()
	{
		s_Factions.Clear();
		array<string> names = new array<string>();
		INT_PDA_SE_Files.ListJson(INT_PDA_SE_Paths.FactionsDir(), names);
		string err;
		foreach (string name : names)
		{
			// the file name is the faction id and must be a safe key (A-Z a-z 0-9 _ -): a hand-made "Долг.json" could
			// be read but never saved — refused with a clear line instead of silently losing every change
			string fileId = name.Substring(0, name.Length() - 5);
			if (!INT_PDA_SE_Paths.SafeKey(fileId))
			{
				INT_PDA_Log.Err("группировка не загружена: имя файла должно быть латиницей/цифрами (например gp010.json): " + name);
				continue;
			}
			string path = INT_PDA_SE_Paths.FactionsDir() + "/" + name;
			INT_PDA_FactionData f = INT_PDA_FactionData.ForRead();
			if (!INT_PDA_SE_Json<INT_PDA_FactionData>.Load(path, f, err))
			{
				Broken(path, err);
				continue;
			}
			f.Clean();
			string id = name.Substring(0, name.Length() - 5);
			f.Id = id;
			if (f.Ranks.Count() == 0)
				f.Ranks.Copy(Cfg().DefaultRanks);
			s_Factions.Set(id, f);
		}
	}

	static INT_PDA_FactionData Faction(string id)
	{
		if (id == "")
			return null;
		return s_Factions.Get(id);
	}

	static bool SaveFaction(INT_PDA_FactionData f)
	{
		if (!f || f.Id == "")
			return false;
		s_Factions.Set(f.Id, f);
		return INT_PDA_SE_Json<INT_PDA_FactionData>.Save(INT_PDA_SE_Paths.FactionFile(f.Id), f);
	}

	// Removes the faction file, its spawn set and its chat; members are handled by the caller.
	static void DeleteFaction(string id)
	{
		s_Factions.Remove(id);
		s_Spawn.Remove(id);
		s_FactionChat.Remove(id);
		s_FactionUnsaved.Remove(id);
		INT_PDA_SE_Files.Remove(INT_PDA_SE_Paths.FactionFile(id));
		INT_PDA_SE_Files.Remove(INT_PDA_SE_Paths.SpawnFile(id));
		INT_PDA_SE_Files.Remove(INT_PDA_SE_Paths.FactionChatFile(id));
		bool changed = false;
		for (int i = s_Relations.Items.Count() - 1; i >= 0; i--)
		{
			INT_PDA_RelationEntry e = s_Relations.Items[i];
			if (e.A == id || e.B == id)
			{
				s_Relations.Items.Remove(i);
				changed = true;
			}
		}
		if (changed)
			SaveRelations();
	}

	// "gp001", "gp002", ... (file names stay ASCII whatever the faction is called).
	static string NewFactionId()
	{
		int n = 1;
		string id;
		while (n < 10000)
		{
			id = "gp" + n.ToStringLen(3);
			if (!s_Factions.Contains(id) && !FileExist(INT_PDA_SE_Paths.FactionFile(id)))
				return id;
			n++;
		}
		return "gp" + GetGame().GetTime().ToString();
	}

	static int OnlineCount(INT_PDA_FactionData f)
	{
		int n = 0;
		foreach (INT_PDA_Member m : f.Members)
		{
			if (m.Online)
				n++;
		}
		return n;
	}

	// --- relations ------------------------------------------------------------------------------------------------
	static void LoadRelations()
	{
		string path = INT_PDA_SE_Paths.RelationsFile();
		s_Relations = INT_PDA_RelationsFile.ForRead();
		string err;
		if (FileExist(path))
		{
			if (!INT_PDA_SE_Json<INT_PDA_RelationsFile>.Load(path, s_Relations, err))
			{
				Broken(path, err);
				s_Relations = INT_PDA_RelationsFile.ForRead();
			}
		}
		s_Relations.Clean();
	}

	static bool SaveRelations()
	{
		return INT_PDA_SE_Json<INT_PDA_RelationsFile>.Save(INT_PDA_SE_Paths.RelationsFile(), s_Relations);
	}

	// --- spawn sets -----------------------------------------------------------------------------------------------
	static void LoadSpawn()
	{
		s_Spawn.Clear();
		string path = INT_PDA_SE_Paths.SpawnFile(SPAWN_DEFAULT);
		INT_PDA_SpawnSet def = ReadSpawn(path);
		bool defBroken = !def && FileExist(path);		// then the copies can't be told: no clearing this time
		if (!def)
		{
			def = INT_PDA_SpawnSet.DefaultSet();
			if (!FileExist(path))
			{
				if (INT_PDA_SE_Json<INT_PDA_SpawnSet>.Save(path, def))
					INT_PDA_Log.Info("создан набор спавна по умолчанию: " + path);
			}
		}
		s_Spawn.Set(SPAWN_DEFAULT, def);
		foreach (string id, INT_PDA_FactionData f : s_Factions)
		{
			INT_PDA_SpawnSet s = ReadSpawn(INT_PDA_SE_Paths.SpawnFile(id));
			if (s)
				s_Spawn.Set(id, s);
		}
		if (!defBroken)
			ClearDefaultCopies(def);
	}

	// Up to 1.1.0 a new faction got a copy of the Default clothes and items, so its members came back in the start
	// gear. Since 2026-10-30 faction members come back naked: such copies are emptied once (points and owners stay).
	// The marker file makes it once only — a set the owner fills later with the Default things is his choice.
	static const string COPIES_MARKER = "FactionSets_1.1.1.txt";

	protected static void ClearDefaultCopies(INT_PDA_SpawnSet def)
	{
		string marker = INT_PDA_SE_Paths.SpawnDir() + "/" + COPIES_MARKER;
		if (FileExist(marker) || !def || !def.HasGear())
			return;
		array<string> ids = new array<string>();
		foreach (string id, INT_PDA_SpawnSet s : s_Spawn)
		{
			if (id != SPAWN_DEFAULT && s.SameGear(def))
				ids.Insert(id);
		}
		bool allSaved = true;
		string done = "";
		foreach (string cid : ids)
		{
			INT_PDA_SpawnSet c = s_Spawn.Get(cid);
			c.ClearGear();
			if (SaveSpawnSet(cid, c))
				done = done + " " + cid;
			else
				allSaved = false;
		}
		if (done != "")
			INT_PDA_Log.Warn("наборы спавна группировок были копией Default — одежда и вещи убраны (члены группировок появляются голыми):" + done);
		// a failed write is tried again at the next start
		if (allSaved)
			INT_PDA_SE_Files.WriteText(marker, "INT_PDA 1.1.1: наборы группировок, скопированные из Default, очищены" + done + ". Не удаляйте этот файл: иначе наборы, совпадающие с Default, очистятся снова.");
	}

	protected static INT_PDA_SpawnSet ReadSpawn(string path)
	{
		if (!FileExist(path))
			return null;
		INT_PDA_SpawnSet s = INT_PDA_SpawnSet.ForRead();
		string err;
		if (!INT_PDA_SE_Json<INT_PDA_SpawnSet>.Load(path, s, err))
		{
			Broken(path, err);
			return null;
		}
		s.Clean();
		return s;
	}

	static INT_PDA_SpawnSet SpawnSet(string id)
	{
		return s_Spawn.Get(id);
	}

	static bool SaveSpawnSet(string id, INT_PDA_SpawnSet s)
	{
		s_Spawn.Set(id, s);
		return INT_PDA_SE_Json<INT_PDA_SpawnSet>.Save(INT_PDA_SE_Paths.SpawnFile(id), s);
	}

	// --- chat history ---------------------------------------------------------------------------------------------
	static void LoadChat()
	{
		s_Public = ReadChat(INT_PDA_SE_Paths.PublicChatFile(), Cfg().PublicHistory);
		s_PublicUnsaved = 0;
		s_FactionChat.Clear();
		s_FactionUnsaved.Clear();
		foreach (string id, INT_PDA_FactionData f : s_Factions)
		{
			string path = INT_PDA_SE_Paths.FactionChatFile(id);
			if (FileExist(path))
				s_FactionChat.Set(id, ReadChat(path, Cfg().FactionHistory));
		}
		// line ids (replies point at them): go on after the highest stored one; lines of older files get one now
		s_LineSeq = 0;
		SeqFrom(s_Public.Lines);
		foreach (string fid, INT_PDA_ChatFile fc : s_FactionChat)
			SeqFrom(fc.Lines);
		NumberLines(s_Public.Lines);
		foreach (string nid, INT_PDA_ChatFile nc : s_FactionChat)
			NumberLines(nc.Lines);
	}

	// --- line ids (replies) ---------------------------------------------------------------------------------------
	static int s_LineSeq;

	static int NextLineId()
	{
		s_LineSeq++;
		return s_LineSeq;
	}

	protected static void SeqFrom(array<ref INT_PDA_ChatLine> lines)
	{
		foreach (INT_PDA_ChatLine l : lines)
		{
			if (l && l.Id > s_LineSeq)
				s_LineSeq = l.Id;
		}
	}

	protected static void NumberLines(array<ref INT_PDA_ChatLine> lines)
	{
		foreach (INT_PDA_ChatLine l : lines)
		{
			if (l && l.Id <= 0)
				l.Id = NextLineId();
		}
	}

	// A line of a history by its id (null = gone from the history or never there). Histories hold ≤ 500 lines.
	static INT_PDA_ChatLine FindLine(array<ref INT_PDA_ChatLine> lines, int id)
	{
		if (!lines || id <= 0)
			return null;
		for (int i = lines.Count() - 1; i >= 0; i--)
		{
			INT_PDA_ChatLine l = lines[i];
			if (l && l.Id == id)
				return l;
		}
		return null;
	}

	protected static INT_PDA_ChatFile ReadChat(string path, int max)
	{
		INT_PDA_ChatFile f = INT_PDA_ChatFile.ForRead();
		string err;
		if (FileExist(path) && !INT_PDA_SE_Json<INT_PDA_ChatFile>.Load(path, f, err))
		{
			Broken(path, err);
			f = INT_PDA_ChatFile.ForRead();
		}
		f.Clean(max);
		return f;
	}

	static void AddPublic(INT_PDA_ChatLine line)
	{
		if (line.Id <= 0)
			line.Id = NextLineId();
		s_Public.Lines.Insert(line);
		while (s_Public.Lines.Count() > Cfg().PublicHistory)
			s_Public.Lines.RemoveOrdered(0);
		s_PublicUnsaved++;
		if (s_PublicUnsaved >= Cfg().PublicSaveEvery)
			SavePublic();
	}

	static void SavePublic()
	{
		if (INT_PDA_SE_Json<INT_PDA_ChatFile>.Save(INT_PDA_SE_Paths.PublicChatFile(), s_Public))
			s_PublicUnsaved = 0;
	}

	static INT_PDA_ChatFile FactionChat(string id)
	{
		INT_PDA_ChatFile f;
		if (!s_FactionChat.Find(id, f))
		{
			f = INT_PDA_ChatFile.ForRead();
			s_FactionChat.Set(id, f);
		}
		return f;
	}

	static void AddFaction(string id, INT_PDA_ChatLine line)
	{
		INT_PDA_ChatFile f = FactionChat(id);
		if (line.Id <= 0)
			line.Id = NextLineId();
		f.Lines.Insert(line);
		while (f.Lines.Count() > Cfg().FactionHistory)
			f.Lines.RemoveOrdered(0);
		int n = s_FactionUnsaved.Get(id) + 1;
		s_FactionUnsaved.Set(id, n);
		if (n >= Cfg().PublicSaveEvery)
			SaveFactionChat(id);
	}

	static void SaveFactionChat(string id)
	{
		if (!s_Factions.Contains(id))
			return;
		if (INT_PDA_SE_Json<INT_PDA_ChatFile>.Save(INT_PDA_SE_Paths.FactionChatFile(id), FactionChat(id)))
			s_FactionUnsaved.Set(id, 0);
	}

	// --- offline mail ---------------------------------------------------------------------------------------------
	static bool AddMail(string steam, INT_PDA_ChatLine line)
	{
		string path = INT_PDA_SE_Paths.MailFile(steam);
		INT_PDA_ChatFile f = ReadChat(path, Cfg().MailMax);
		f.Lines.Insert(line);
		while (f.Lines.Count() > Cfg().MailMax)
			f.Lines.RemoveOrdered(0);
		return INT_PDA_SE_Json<INT_PDA_ChatFile>.Save(path, f);
	}

	// The waiting mail of a player (the file is removed: the client keeps its private history itself).
	static INT_PDA_ChatFile TakeMail(string steam)
	{
		string path = INT_PDA_SE_Paths.MailFile(steam);
		if (!FileExist(path))
			return null;
		INT_PDA_ChatFile f = ReadChat(path, Cfg().MailMax);
		INT_PDA_SE_Files.Remove(path);
		return f;
	}

	// --- news -----------------------------------------------------------------------------------------------------
	static void LoadNews()
	{
		s_News = INT_PDA_NewsFile.ForRead();
		string path = INT_PDA_SE_Paths.NewsFile();
		string err;
		if (FileExist(path) && !INT_PDA_SE_Json<INT_PDA_NewsFile>.Load(path, s_News, err))
		{
			Broken(path, err);
			s_News = INT_PDA_NewsFile.ForRead();
		}
		s_News.Clean(Cfg().NewsMax);
	}

	// minutes: how long the news is shown (0 = until deleted, -1 = the config default NewsMinutesDefault)
	static INT_PDA_News AddNews(string title, string text, int minutes = -1)
	{
		DropExpiredNews();
		INT_PDA_News n = new INT_PDA_News();
		n.Id = s_News.NextId;
		s_News.NextId = s_News.NextId + 1;
		n.Time = INT_PDA_SE_Clock.Short();
		n.Title = title;
		n.Text = text;
		SetNewsLife(n, minutes);
		s_News.Items.Insert(n);
		s_News.Clean(Cfg().NewsMax);
		INT_PDA_SE_Json<INT_PDA_NewsFile>.Save(INT_PDA_SE_Paths.NewsFile(), s_News);
		return n;
	}

	// The admin edits a published news item: text and lifetime (counted again from now). null = no such id.
	static INT_PDA_News EditNews(int id, string title, string text, int minutes)
	{
		INT_PDA_News n = FindNews(id);
		if (!n)
			return null;
		n.Title = title;
		n.Text = text;
		SetNewsLife(n, minutes);
		DropExpiredNews();
		INT_PDA_SE_Json<INT_PDA_NewsFile>.Save(INT_PDA_SE_Paths.NewsFile(), s_News);
		return n;
	}

	static INT_PDA_News FindNews(int id)
	{
		foreach (INT_PDA_News n : s_News.Items)
		{
			if (n && n.Id == id)
				return n;
		}
		return null;
	}

	protected static void SetNewsLife(INT_PDA_News n, int minutes)
	{
		int m = minutes;
		if (m < 0)
			m = Cfg().NewsMinutesDefault;
		m = INT_PDA_Util.ClampI(m, 0, 525600);
		n.Minutes = m;
		n.ExpireAt = 0;
		n.LeftSec = -1;
		if (m > 0)
		{
			n.ExpireAt = INT_PDA_Util.NowSec() + m * 60;
			n.LeftSec = m * 60;
		}
	}

	static bool NewsLive(INT_PDA_News n, int now)
	{
		return n && (n.ExpireAt <= 0 || n.ExpireAt > now);
	}

	// Live news, oldest first, the newest `max` of them; LeftSec filled for the wire.
	static void LiveNews(array<INT_PDA_News> outList, int max)
	{
		outList.Clear();
		int now = INT_PDA_Util.NowSec();
		foreach (INT_PDA_News n : s_News.Items)
		{
			if (!NewsLive(n, now))
				continue;
			n.LeftSec = -1;
			if (n.ExpireAt > 0)
				n.LeftSec = n.ExpireAt - now;
			outList.Insert(n);
		}
		while (outList.Count() > max)
			outList.RemoveOrdered(0);
	}

	// Expired items leave the file on the next write (no timer: news is written only on admin actions).
	protected static void DropExpiredNews()
	{
		int now = INT_PDA_Util.NowSec();
		for (int i = s_News.Items.Count() - 1; i >= 0; i--)
		{
			if (!NewsLive(s_News.Items[i], now))
				s_News.Items.RemoveOrdered(i);
		}
	}

	static bool DeleteNews(int id)
	{
		for (int i = 0; i < s_News.Items.Count(); i++)
		{
			if (s_News.Items[i].Id == id)
			{
				s_News.Items.RemoveOrdered(i);
				INT_PDA_SE_Json<INT_PDA_NewsFile>.Save(INT_PDA_SE_Paths.NewsFile(), s_News);
				return true;
			}
		}
		return false;
	}
}

// Server-machine time for chat lines and records (owner rule 19).
class INT_PDA_SE_Clock
{
	// "DD.MM HH:MM"
	static string Short()
	{
		int y;
		int mo;
		int d;
		int h;
		int mi;
		int s;
		GetYearMonthDay(y, mo, d);
		GetHourMinuteSecond(h, mi, s);
		return d.ToStringLen(2) + "." + mo.ToStringLen(2) + " " + h.ToStringLen(2) + ":" + mi.ToStringLen(2);
	}

	// Whole days between a "YYYY-MM-DD..." stamp and today (0 for a broken stamp).
	static int DaysSince(string stamp)
	{
		if (stamp.Length() < 10)
			return 0;
		int y0 = stamp.Substring(0, 4).ToInt();
		int m0 = stamp.Substring(5, 2).ToInt();
		int d0 = stamp.Substring(8, 2).ToInt();
		int y;
		int mo;
		int d;
		GetYearMonthDay(y, mo, d);
		int days = DayNumber(y, mo, d) - DayNumber(y0, m0, d0);
		if (days < 0)
			return 0;
		return days;
	}

	// Days since 0000-03-01 (civil calendar), good for differences.
	static int DayNumber(int y, int m, int d)
	{
		if (m <= 2)
		{
			y = y - 1;
			m = m + 12;
		}
		return 365 * y + y / 4 - y / 100 + y / 400 + (153 * (m - 3) + 2) / 5 + d;
	}
}
