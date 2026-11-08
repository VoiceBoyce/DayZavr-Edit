// INT PartyMode (server) — friend lists on disk (spec.md §6, owner D2):
//   DataBaseParty/<steam64>.json = { Version, Id, Name, Friends: [ { Id, Name, Since } ] }
// A file is read once when its player says HELLO (kept in memory while he is online) and written only when his list
// or his name changes. A player without friends has no file. Offline players are read on demand (removal by a
// friend, server API) and not cached. Friendship is mutual: both files always change together.

class INT_PTY_SE_FriendEntry
{
	string Id;
	string Name;
	string Since;
}

class INT_PTY_SE_PlayerFile
{
	int Version = 1;
	string Id;
	string Name;
	ref array<ref INT_PTY_SE_FriendEntry> Friends;

	// The file exists but could not be read (locked): this record is NOT the player's list and must never be saved
	// over it (review PTY-02). Runtime only.
	[NonSerialized()]
	bool Untrusted;

	void INT_PTY_SE_PlayerFile()
	{
		Friends = new array<ref INT_PTY_SE_FriendEntry>();
	}

	int IndexOf(string id)
	{
		int n = Friends.Count();
		for (int i = 0; i < n; i++)
		{
			INT_PTY_SE_FriendEntry e = Friends.Get(i);
			if (e && e.Id == id)
				return i;
		}
		return -1;
	}

	bool Has(string id)
	{
		return IndexOf(id) >= 0;
	}

	INT_PTY_SE_FriendEntry Find(string id)
	{
		int i = IndexOf(id);
		if (i < 0)
			return null;
		return Friends.Get(i);
	}

	bool Add(string id, string name, string since)
	{
		if (Has(id))
			return false;
		INT_PTY_SE_FriendEntry e = new INT_PTY_SE_FriendEntry();
		e.Id = id;
		e.Name = name;
		e.Since = since;
		Friends.Insert(e);
		return true;
	}

	bool Remove(string id)
	{
		int i = IndexOf(id);
		if (i < 0)
			return false;
		Friends.RemoveOrdered(i);
		return true;
	}

	// Repairs a record read from disk: bad or duplicate ids and the player himself are dropped. Returns fixes.
	int Sanitize(string selfId)
	{
		int fixes = 0;
		if (Version != 1)
		{
			Version = 1;
			fixes++;
		}
		if (Id != selfId)
		{
			Id = selfId;
			fixes++;
		}
		if (!Friends)
		{
			Friends = new array<ref INT_PTY_SE_FriendEntry>();
			fixes++;
		}
		array<ref INT_PTY_SE_FriendEntry> kept = new array<ref INT_PTY_SE_FriendEntry>();
		array<string> seen = new array<string>();
		foreach (INT_PTY_SE_FriendEntry e : Friends)
		{
			if (!e || !INT_PTY_Util.IsSteamId(e.Id) || e.Id == selfId || seen.Find(e.Id) >= 0)
			{
				fixes++;
				continue;
			}
			string clean = INT_PTY_Util.CleanName(e.Name);
			if (clean != e.Name)
			{
				e.Name = clean;
				fixes++;
			}
			seen.Insert(e.Id);
			kept.Insert(e);
		}
		Friends = kept;
		return fixes;
	}
}

class INT_PTY_SE_Store
{
	protected static ref map<string, ref INT_PTY_SE_PlayerFile> s_Cache;	// online players only

	// statistics (debug / self-test)
	static int s_Reads;
	static int s_Writes;
	static int s_Deletes;
	static int s_Broken;

	static void Init()
	{
		if (!s_Cache)
			s_Cache = new map<string, ref INT_PTY_SE_PlayerFile>();
	}

	static void Reset()
	{
		s_Cache = new map<string, ref INT_PTY_SE_PlayerFile>();
	}

	protected static ref map<string, ref INT_PTY_SE_PlayerFile> s_Saved;

	// Self-test: the cache of the real online players is put aside during a test run.
	static void PushForTest()
	{
		Init();
		s_Saved = s_Cache;
		Reset();
	}

	static void PopForTest()
	{
		if (!s_Saved)
			return;
		s_Cache = s_Saved;
		s_Saved = null;
	}

	static INT_PTY_SE_PlayerFile Cached(string steam)
	{
		if (!s_Cache)
			return null;
		INT_PTY_SE_PlayerFile f;
		s_Cache.Find(steam, f);
		return f;
	}

	static int CachedCount()
	{
		if (!s_Cache)
			return 0;
		return s_Cache.Count();
	}

	// The record of an online player: from memory, else from disk (then kept until Release).
	static INT_PTY_SE_PlayerFile Acquire(string steam)
	{
		Init();
		INT_PTY_SE_PlayerFile f = Cached(steam);
		if (f)
			return f;
		f = LoadFromDisk(steam, true);
		s_Cache.Set(steam, f);
		return f;
	}

	static void Release(string steam)
	{
		if (s_Cache)
			s_Cache.Remove(steam);
	}

	// From memory when online, else straight from disk (not cached). Never null.
	// From memory when online, else straight from disk (not cached, never written — review PTY-09). Never null.
	static INT_PTY_SE_PlayerFile Peek(string steam)
	{
		INT_PTY_SE_PlayerFile f = Cached(steam);
		if (f)
			return f;
		return LoadFromDisk(steam, false);
	}

	// writeFixes: a repaired list is saved back (online players); offline reads only look.
	protected static INT_PTY_SE_PlayerFile LoadFromDisk(string steam, bool writeFixes)
	{
		INT_PTY_SE_PlayerFile f = new INT_PTY_SE_PlayerFile();
		f.Id = steam;
		string path = INT_PTY_SE_Paths.PlayerFile(steam);
		INT_PTY_SE_Files.Recover(path);
		string txt;
		int st = INT_PTY_SE_Files.ReadState(path, txt);
		if (st == INT_PTY_SE_Files.READ_NONE)
			return f;
		if (st == INT_PTY_SE_Files.READ_FAIL)
		{
			f.Untrusted = true;
			INT_PTY_Log.Crit("файл друзей есть, но не открывается (занят?) — изменения этого игрока не пишутся до следующего входа: " + path);
			return f;
		}
		s_Reads++;
		string err;
		if (!INT_PTY_SE_Json<INT_PTY_SE_PlayerFile>.Parse(txt, f, err))
		{
			s_Broken++;
			// quarantine once: the copy keeps the data, the next read sees «no file» (no КРИТИЧНО on every API call)
			string copy = INT_PTY_SE_Files.KeepBroken(path);
			if (copy != "")
				DeleteFile(path);
			INT_PTY_Log.Crit("файл друзей не читается (" + err + "), убран в " + copy);
			f = new INT_PTY_SE_PlayerFile();
			f.Id = steam;
			if (copy == "")
				f.Untrusted = true;
			return f;
		}
		int fixes = f.Sanitize(steam);
		if (fixes > 0)
		{
			INT_PTY_Log.Warn("файл друзей " + steam + ": исправлено записей " + fixes.ToString());
			if (writeFixes)
				Save(f);
		}
		return f;
	}

	// An empty list removes the file (a player without friends has none). An untrusted record is never written.
	static bool Save(INT_PTY_SE_PlayerFile f)
	{
		if (!f || !INT_PTY_Util.IsSteamId(f.Id))
			return false;
		if (f.Untrusted)
		{
			INT_PTY_Log.Err("запись файла друзей пропущена — файл не был прочитан: " + f.Id);
			return false;
		}
		INT_PTY_SE_Paths.MakeDirs();
		string path = INT_PTY_SE_Paths.PlayerFile(f.Id);
		if (f.Friends.Count() == 0)
		{
			if (FileExist(path))
			{
				DeleteFile(path);
				s_Deletes++;
			}
			if (FileExist(path + ".tmp"))
				DeleteFile(path + ".tmp");
			return !FileExist(path);
		}
		if (!INT_PTY_SE_Json<INT_PTY_SE_PlayerFile>.Save(path, f))
			return false;
		s_Writes++;
		return true;
	}
}
