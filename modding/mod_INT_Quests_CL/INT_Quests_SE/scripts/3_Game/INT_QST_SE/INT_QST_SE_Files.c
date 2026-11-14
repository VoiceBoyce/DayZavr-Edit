// INT Quests (server) — file layout, recoverable writes, JSON and the admin log (spec.md §5, §9).
//   $profile:INT_Mods/INT_QUESTS/INT_QUESTS_Config.json
//   $profile:INT_Mods/INT_QUESTS/Givers.json
//   $profile:INT_Mods/INT_QUESTS/Quests/<Branch>/_branch.json, <Branch>_<NN>.json
//   $profile:INT_Mods/INT_QUESTS/Players/<Steam64>.json
//   $profile:INT_Mods/INT_QUESTS/Exports/<time>/<Branch>/..., Imports/<folder>/<Branch>/...
//   $profile:INT_Mods/INT_QUESTS/Logs/INT_QUESTS_<time>.log
// MakeDirectory is not recursive: every level is created on its own. FindFile on "$profile:" works on 1.30.164200
// (measured 2026-10-22: folders attr 1, files 16). The self-test kit redirects the root with SetTestRoot.

class INT_QST_SE_Paths
{
	static const string MODS = "$profile:INT_Mods";
	static const string ROOT = "$profile:INT_Mods/INT_QUESTS";

	protected static string s_TestRoot;

	static string Root()
	{
		if (s_TestRoot != "")
			return s_TestRoot;
		return ROOT;
	}

	static string ConfigFile()
	{
		return Root() + "/INT_QUESTS_Config.json";
	}

	static string GiversFile()
	{
		return Root() + "/Givers.json";
	}

	static string QuestsDir()
	{
		return Root() + "/Quests";
	}

	static string BranchDir(string branch)
	{
		return QuestsDir() + "/" + branch;
	}

	static string PlayersDir()
	{
		return Root() + "/Players";
	}

	static string PlayerFile(string uid)
	{
		return PlayersDir() + "/" + uid + ".json";
	}

	static string ExportsDir()
	{
		return Root() + "/Exports";
	}

	static string ImportsDir()
	{
		return Root() + "/Imports";
	}

	static string LogDir()
	{
		return Root() + "/Logs";
	}

	static void Make(string dir)
	{
		if (!FileExist(dir))
			MakeDirectory(dir);
	}

	static void MakeAll()
	{
		Make(MODS);
		Make(ROOT);
		if (s_TestRoot != "")
			Make(s_TestRoot);
		Make(QuestsDir());
		Make(PlayersDir());
		Make(ExportsDir());
		Make(ImportsDir());
		Make(LogDir());
	}

	// Self-test only ("" = back to the real folder). The sandbox must be a direct child of ROOT.
	static void SetTestRoot(string root)
	{
		s_TestRoot = root;
	}

	// Folder / file name rule for branches: latin letters, digits and '_' (portable between servers).
	static bool IsSafeName(string s)
	{
		int n = s.Length();
		if (n < 1 || n > 64)
			return false;
		string c;
		for (int i = 0; i < n; i++)
		{
			c = s.Get(i);
			bool ok = (c >= "a" && c <= "z") || (c >= "A" && c <= "Z") || (c >= "0" && c <= "9") || c == "_";
			if (!ok)
				return false;
		}
		return true;
	}
}

class INT_QST_SE_Files
{
	static const int READ_MAX = 16000000;

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
		string ca = INT_QST_Util.ReplaceByte(a, "\r", "");
		string cb = INT_QST_Util.ReplaceByte(b, "\r", "");
		ca.TrimInPlace();
		cb.TrimInPlace();
		return ca == cb;
	}

	// Copy of a file that cannot be used; returns the copy's path ("" when nothing was copied).
	static string KeepBroken(string path)
	{
		if (!FileExist(path))
			return "";
		string dst = path + ".broken-" + INT_QST_Util.FileStamp();
		if (CopyFile(path, dst))
			return dst;
		return "";
	}

	// Configs and quests: tmp -> re-read -> .bak of the old file -> replace -> re-read.
	// false = the old content is still in place.
	static bool SaveText(string path, string content)
	{
		string tmp = path + ".tmp";
		string bak = path + ".bak";
		if (!WriteText(tmp, content))
		{
			DeleteFile(tmp);
			INT_QST_Log.Err("запись: не удалось создать " + tmp);
			return false;
		}
		string back;
		if (!ReadText(tmp, back) || !SameContent(back, content))
		{
			DeleteFile(tmp);
			INT_QST_Log.Err("запись: проверка временного файла не прошла " + tmp);
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
				INT_QST_Log.Err("запись: не удалось заменить " + path);
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
		INT_QST_Log.Err("запись: копия не удалась, .tmp оставлен: " + path);
		return false;
	}

	// Player progress (written on every change, owner decision O11): the cheap path. A crash between the steps
	// leaves either the old file or the .tmp, and the reader takes the .tmp when the main file is missing.
	static bool SaveFast(string path, string content)
	{
		string tmp = path + ".tmp";
		if (!WriteText(tmp, content))
			return false;
		// the .tmp must hold the whole text before it replaces the good file (a full disk writes it short)
		string back;
		if (!ReadText(tmp, back) || !SameContent(back, content))
		{
			INT_QST_Log.Err("запись: .tmp не совпал с данными, файл не заменён: " + path);
			return false;
		}
		if (FileExist(path))
			DeleteFile(path);
		bool ok = CopyFile(tmp, path);
		if (ok)
			DeleteFile(tmp);
		return ok;
	}

	static bool ReadFast(string path, out string content)
	{
		if (ReadText(path, content))
			return true;
		return ReadText(path + ".tmp", content);
	}
}

class INT_QST_SE_Json<Class T>
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
			INT_QST_Log.Err("JSON не собирается: " + path);
			return false;
		}
		return INT_QST_SE_Files.SaveText(path, txt);
	}

	// Read an existing file into `data` (pre-filled with defaults). false = file unreadable or broken.
	static bool Load(string path, T data, out string err)
	{
		string txt;
		if (!INT_QST_SE_Files.ReadText(path, txt))
		{
			err = "не открывается";
			return false;
		}
		return Parse(txt, data, err);
	}
}

// Admin log: one file per server start, local time of the machine (owner rules 16, 19).
class INT_QST_SE_FileLog
{
	protected static string s_Path;
	static int s_Lines;			// self-test

	static void Open()
	{
		s_Lines = 0;
		INT_QST_SE_Paths.MakeAll();
		s_Path = INT_QST_SE_Paths.LogDir() + "/INT_QUESTS_" + INT_QST_Util.FileStamp() + ".log";
		Line("=== DayZavr Quest System " + INT_QST_Const.VERSION + " — старт сервера " + INT_QST_Util.Stamp() + " ===");
	}

	static string Path()
	{
		return s_Path;
	}

	// One line "[HH:MM:SS] text".
	static void Line(string text)
	{
		if (s_Path == "")
			return;
		FileHandle fh = OpenFile(s_Path, FileMode.APPEND);
		if (fh == 0)
			return;
		FPrintln(fh, "[" + INT_QST_Util.Clock() + "] " + text);
		CloseFile(fh);
		s_Lines++;
	}
}
