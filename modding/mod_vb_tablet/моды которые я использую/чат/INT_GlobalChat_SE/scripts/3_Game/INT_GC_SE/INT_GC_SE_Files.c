// INT GlobalChat (server) — file layout and recoverable JSON writes (spec.md §4.1).
//   $profile:INT_Mods/INT_GlobalChat/INT_GlobalChat_Config.json
//   $profile:INT_Mods/INT_GlobalChat/BannedWords.json
//   $profile:INT_Mods/INT_GlobalChat/PlayersWithBannedChat.json
// MakeDirectory is not recursive: every level is created on its own. The self-test kit redirects the folder into a
// sandbox with SetTestRoot.

class INT_GC_SE_Paths
{
	static const string MODS = "$profile:INT_Mods";
	static const string ROOT = "$profile:INT_Mods/INT_GlobalChat";

	protected static string s_TestRoot;

	static string Root()
	{
		if (s_TestRoot != "")
			return s_TestRoot;
		return ROOT;
	}

	static string ConfigFile()
	{
		return Root() + "/INT_GlobalChat_Config.json";
	}

	static string WordsFile()
	{
		return Root() + "/BannedWords.json";
	}

	static string MutesFile()
	{
		return Root() + "/PlayersWithBannedChat.json";
	}

	static void MakeRoot()
	{
		if (!FileExist(MODS))
			MakeDirectory(MODS);
		if (!FileExist(ROOT))
			MakeDirectory(ROOT);
		if (s_TestRoot != "" && !FileExist(s_TestRoot))
			MakeDirectory(s_TestRoot);
	}

	// Self-test only ("" = back to the real folder). The sandbox must be a direct child of ROOT.
	static void SetTestRoot(string root)
	{
		s_TestRoot = root;
	}
}

class INT_GC_SE_Files
{
	static const int READ_MAX = 4000000;
	static string s_LastBroken;		// path of the last .broken copy (self-test)

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
		return true;
	}

	static bool WriteText(string path, string content)
	{
		FileHandle fh = OpenFile(path, FileMode.WRITE);
		if (fh == 0)
			return false;
		FPrint(fh, content);
		CloseFile(fh);
		return true;
	}

	static bool SameContent(string a, string b)
	{
		string ca = INT_GC_Util.ReplaceByte(a, "\r", "");
		string cb = INT_GC_Util.ReplaceByte(b, "\r", "");
		ca.TrimInPlace();
		cb.TrimInPlace();
		return ca == cb;
	}

	// Copy of a file that cannot be used; returns the copy's path ("" when nothing was copied).
	static string KeepBroken(string path)
	{
		if (!FileExist(path))
			return "";
		string dst = path + ".broken-" + INT_GC_Util.FileStamp();
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
		string tmp = path + ".tmp";
		string bak = path + ".bak";
		if (!WriteText(tmp, content))
		{
			DeleteFile(tmp);
			INT_GC_Log.Err("запись: не удалось создать " + tmp);
			return false;
		}
		string back;
		if (!ReadText(tmp, back) || !SameContent(back, content))
		{
			DeleteFile(tmp);
			INT_GC_Log.Err("запись: проверка временного файла не прошла " + tmp);
			return false;
		}
		if (FileExist(path))
		{
			if (FileExist(bak))
				DeleteFile(bak);
			CopyFile(path, bak);
			if (!DeleteFile(path))
			{
				DeleteFile(tmp);
				INT_GC_Log.Err("запись: не удалось заменить " + path);
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
			return true;
		}
		if (FileExist(bak))
		{
			if (FileExist(path))
				DeleteFile(path);
			CopyFile(bak, path);
		}
		INT_GC_Log.Crit("запись: копия не удалась, .tmp оставлен: " + path);
		return false;
	}
}

class INT_GC_SE_Json<Class T>
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
			INT_GC_Log.Err("JSON не собирается: " + path);
			return false;
		}
		return INT_GC_SE_Files.SaveText(path, txt);
	}
}
