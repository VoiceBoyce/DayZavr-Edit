// INT_PDA (server) — file layout and recoverable JSON writes (spec.md §6).
//   $profile:INT_Mods/INT_PDA/INT_PDA_CONFIG.json
//   .../Players/<SteamID>.json   accounts without a faction      .../GP/<SteamID>.json   accounts in a faction
//   .../Names/<callsign>.json    callsign -> SteamID index       .../Factions/<id>.json  one faction each
//   .../Spawn/Default.json       spawn set without a faction     .../Spawn/<id>.json     spawn set of a faction
//   .../Relations.json  .../News.json  .../Chat/*.json  .../Mail/<SteamID>.json  .../Logs/
// Thousands of players → one small file per player, never one big list (owner rule 22). MakeDirectory is not
// recursive: every level is created on its own. The self-test kit redirects the folder into a sandbox (SetTestRoot).

class INT_PDA_SE_Paths
{
	static const string MODS = "$profile:INT_Mods";
	static const string ROOT = "$profile:INT_Mods/INT_PDA";

	protected static string s_TestRoot;

	static string Root()
	{
		if (s_TestRoot != "")
			return s_TestRoot;
		return ROOT;
	}

	static string ConfigFile()
	{
		return Root() + "/INT_PDA_CONFIG.json";
	}

	static string PlayersDir()
	{
		return Root() + "/Players";
	}

	static string GpDir()
	{
		return Root() + "/GP";
	}

	static string NamesDir()
	{
		return Root() + "/Names";
	}

	static string FactionsDir()
	{
		return Root() + "/Factions";
	}

	static string SpawnDir()
	{
		return Root() + "/Spawn";
	}

	static string ChatDir()
	{
		return Root() + "/Chat";
	}

	static string MailDir()
	{
		return Root() + "/Mail";
	}

	static string LogDir()
	{
		return Root() + "/Logs";
	}

	static string RelationsFile()
	{
		return Root() + "/Relations.json";
	}

	static string NewsFile()
	{
		return Root() + "/News.json";
	}

	// Every file name built from data that came from a client (SteamID, callsign key, faction id) must pass this:
	// ASCII letters, digits, '_' and '-' only, 1..64 characters — no '/', '\', '.', ':' (no path tricks like
	// "../Players/<id>"). A key that fails gives the path "" (nothing exists there, nothing can be written there).
	static bool SafeKey(string key)
	{
		int n = key.Length();
		if (n < 1 || n > 64)
			return false;
		int c;
		for (int i = 0; i < n; i++)
		{
			c = key.Get(i).ToAscii();
			if (c >= 48 && c <= 57)
				continue;
			if (c >= 65 && c <= 90)
				continue;
			if (c >= 97 && c <= 122)
				continue;
			if (c == 95 || c == 45)
				continue;
			return false;
		}
		return true;
	}

	static string AccountFile(string steam, bool inFaction)
	{
		if (!SafeKey(steam))
			return "";
		if (inFaction)
			return GpDir() + "/" + steam + ".json";
		return PlayersDir() + "/" + steam + ".json";
	}

	static string NameFile(string key)
	{
		if (!SafeKey(key))
			return "";
		return NamesDir() + "/" + key + ".json";
	}

	static string FactionFile(string id)
	{
		if (!SafeKey(id))
			return "";
		return FactionsDir() + "/" + id + ".json";
	}

	static string SpawnFile(string id)
	{
		if (!SafeKey(id))
			return "";
		return SpawnDir() + "/" + id + ".json";
	}

	static string MailFile(string steam)
	{
		if (!SafeKey(steam))
			return "";
		return MailDir() + "/" + steam + ".json";
	}

	static string PublicChatFile()
	{
		return ChatDir() + "/Public.json";
	}

	static string FactionChatFile(string id)
	{
		if (!SafeKey(id))
			return "";
		return ChatDir() + "/Faction_" + id + ".json";
	}

	static void MakeDir(string path)
	{
		if (!FileExist(path))
			MakeDirectory(path);
	}

	static void MakeRoot()
	{
		MakeDir(MODS);
		MakeDir(ROOT);
		if (s_TestRoot != "")
			MakeDir(s_TestRoot);
		MakeDir(PlayersDir());
		MakeDir(GpDir());
		MakeDir(NamesDir());
		MakeDir(FactionsDir());
		MakeDir(SpawnDir());
		MakeDir(ChatDir());
		MakeDir(MailDir());
		MakeDir(LogDir());
	}

	// Self-test only ("" = back to the real folder). A sandbox must be a direct child of ROOT.
	static void SetTestRoot(string root)
	{
		s_TestRoot = root;
	}

	static bool IsTestRoot()
	{
		return s_TestRoot != "";
	}
}

class INT_PDA_SE_Files
{
	static const int READ_MAX = 4000000;
	static string s_LastBroken;		// path of the last .broken copy (self-test)
	static int s_Reads;				// counters for Debug and the self-test
	static int s_Writes;
	static int s_Lists;				// folder listings (self-test: proves login / chat never list a folder)

	static bool ReadText(string path, out string content)
	{
		content = "";
		if (!FileExist(path))
			return false;
		FileHandle fh = OpenFile(path, FileMode.READ);
		if (fh == 0)
			return false;
		string txt;
		ReadFile(fh, txt, READ_MAX);
		CloseFile(fh);
		content = txt;
		s_Reads++;
		return true;
	}

	static bool WriteText(string path, string content)
	{
		if (path == "")
			return false;
		FileHandle fh = OpenFile(path, FileMode.WRITE);
		if (fh == 0)
			return false;
		FPrint(fh, content);
		CloseFile(fh);
		return true;
	}

	static bool AppendLine(string path, string line)
	{
		if (path == "")
			return false;
		FileHandle fh = OpenFile(path, FileMode.APPEND);
		if (fh == 0)
			return false;
		FPrintln(fh, line);
		CloseFile(fh);
		return true;
	}

	static bool SameContent(string a, string b)
	{
		string ca = INT_PDA_Util.ReplaceByte(a, "\r", "");
		string cb = INT_PDA_Util.ReplaceByte(b, "\r", "");
		ca.TrimInPlace();
		cb.TrimInPlace();
		return ca == cb;
	}

	// Copy of a file that cannot be used; returns the copy's path ("" when nothing was copied).
	static string KeepBroken(string path)
	{
		if (!FileExist(path))
			return "";
		string dst = path + ".broken-" + INT_PDA_Util.FileStamp();
		if (CopyFile(path, dst))
		{
			s_LastBroken = dst;
			return dst;
		}
		return "";
	}

	// tmp -> re-read -> .bak of the old file -> replace -> re-read. false = the old content is still in place.
	static bool SaveText(string path, string content)
	{
		if (path == "")
			return false;
		s_Writes++;
		string tmp = path + ".tmp";
		string bak = path + ".bak";
		if (!WriteText(tmp, content))
		{
			DeleteFile(tmp);
			INT_PDA_Log.Err("запись: не удалось создать " + tmp);
			return false;
		}
		string back;
		if (!ReadText(tmp, back) || !SameContent(back, content))
		{
			DeleteFile(tmp);
			INT_PDA_Log.Err("запись: проверка временного файла не прошла " + tmp);
			return false;
		}
		if (FileExist(path))
		{
			if (FileExist(bak))
				DeleteFile(bak);
			// no .bak = no safety net: the old file is not touched
			if (!CopyFile(path, bak))
			{
				DeleteFile(tmp);
				INT_PDA_Log.Err("запись: не удалось сделать .bak, файл не изменён: " + path);
				return false;
			}
			if (!DeleteFile(path))
			{
				DeleteFile(tmp);
				INT_PDA_Log.Err("запись: не удалось заменить " + path);
				return false;
			}
		}
		bool ok = CopyFile(tmp, path);
		string fin;
		if (ok)
			ok = ReadText(path, fin) && SameContent(fin, content);
		if (ok)
		{
			DeleteFile(tmp);
			if (FileExist(bak))
				DeleteFile(bak);
			return true;
		}
		if (FileExist(bak))
		{
			if (FileExist(path))
				DeleteFile(path);
			CopyFile(bak, path);
		}
		INT_PDA_Log.Err("запись: копия не удалась, .tmp оставлен: " + path);
		return false;
	}

	// A file and its leftovers (.tmp/.bak) go away.
	static void Remove(string path)
	{
		if (path == "")
			return;
		if (FileExist(path))
			DeleteFile(path);
		if (FileExist(path + ".tmp"))
			DeleteFile(path + ".tmp");
		if (FileExist(path + ".bak"))
			DeleteFile(path + ".bak");
	}

	// Mission start only: a write that died between "old file removed" and "new file copied" leaves just the verified
	// .tmp (or the .bak). Such a file becomes the real one again — first the .tmp (newer), then the .bak. One listing
	// per extension and folder; files that are in place are not touched.
	static int RecoverDir(string dir)
	{
		int n = RecoverExt(dir, ".tmp");
		n = n + RecoverExt(dir, ".bak");
		return n;
	}

	protected static int RecoverExt(string dir, string ext)
	{
		array<string> found = new array<string>();
		string name;
		FileAttr attr;
		FindFileHandle h = FindFile(dir + "/*" + ext, name, attr, FindFileFlags.ALL);
		bool more = name != "";
		while (more)
		{
			if ((attr & FileAttr.DIRECTORY) == 0)
				found.Insert(name);
			more = FindNextFile(h, name, attr);
		}
		CloseFindFile(h);
		int n = 0;
		string txt;
		foreach (string f : found)
		{
			int cut = f.Length() - ext.Length();
			if (cut <= 0)
				continue;
			string base = dir + "/" + f.Substring(0, cut);
			string src = dir + "/" + f;
			if (FileExist(base))
				continue;
			if (!ReadText(src, txt) || txt == "")
				continue;
			if (CopyFile(src, base))
			{
				n++;
				INT_PDA_Log.Warn("запись была прервана — файл восстановлен из " + ext + ": " + base);
			}
		}
		return n;
	}

	// Bare names of the *.json files of a folder (FindFile on $profile: works on 1.30.164200).
	static void ListJson(string dir, array<string> out_names)
	{
		out_names.Clear();
		s_Lists++;
		string name;
		FileAttr attr;
		FindFileHandle h = FindFile(dir + "/*.json", name, attr, FindFileFlags.ALL);
		bool more = name != "";
		while (more)
		{
			if ((attr & FileAttr.DIRECTORY) == 0)
				out_names.Insert(name);
			more = FindNextFile(h, name, attr);
		}
		CloseFindFile(h);
		out_names.Sort();
	}
}

class INT_PDA_SE_Json<Class T>
{
	static bool Parse(string txt, T data, out string err)
	{
		JsonSerializer js = new JsonSerializer();
		return js.ReadFromString(data, txt, err);
	}

	static bool ToText(T data, bool nice, out string txt)
	{
		JsonSerializer js = new JsonSerializer();
		return js.WriteToString(data, nice, txt);
	}

	static bool Save(string path, T data)
	{
		string txt;
		if (!ToText(data, true, txt))
		{
			INT_PDA_Log.Err("JSON не собирается: " + path);
			return false;
		}
		return INT_PDA_SE_Files.SaveText(path, txt);
	}

	// Read a file into `data` (the caller passes an object made by its ForRead()). false = missing or broken.
	static bool Load(string path, T data, out string err)
	{
		string txt;
		err = "";
		if (!INT_PDA_SE_Files.ReadText(path, txt))
		{
			err = "нет файла";
			return false;
		}
		return Parse(txt, data, err);
	}
}
