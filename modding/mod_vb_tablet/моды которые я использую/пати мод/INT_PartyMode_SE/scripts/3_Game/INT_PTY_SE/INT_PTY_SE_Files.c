// INT PartyMode (server) — file layout and recoverable writes (spec.md §6).
//   $profile:INT_Mods/INT_PartyMode/INT_PartyMode_Config.json
//   $profile:INT_Mods/INT_PartyMode/DataBaseParty/<steam64>.json
//   $profile:INT_Mods/INT_PartyMode/PartyHistory/<date_time>.log
// MakeDirectory is not recursive: every level is created on its own. The self-test kit redirects the folder into a
// sandbox with SetTestRoot.

class INT_PTY_SE_Paths
{
	static const string MODS = "$profile:INT_Mods";
	static const string ROOT = "$profile:INT_Mods/INT_PartyMode";

	protected static string s_TestRoot;

	static string Root()
	{
		if (s_TestRoot != "")
			return s_TestRoot;
		return ROOT;
	}

	static string ConfigFile()
	{
		return Root() + "/INT_PartyMode_Config.json";
	}

	static string DbDir()
	{
		return Root() + "/DataBaseParty";
	}

	static string PlayerFile(string steam)
	{
		return DbDir() + "/" + steam + ".json";
	}

	static string HistoryDir()
	{
		return Root() + "/PartyHistory";
	}

	static void MakeDirs()
	{
		if (!FileExist(MODS))
			MakeDirectory(MODS);
		if (!FileExist(ROOT))
			MakeDirectory(ROOT);
		if (s_TestRoot != "" && !FileExist(s_TestRoot))
			MakeDirectory(s_TestRoot);
		if (!FileExist(DbDir()))
			MakeDirectory(DbDir());
		if (!FileExist(HistoryDir()))
			MakeDirectory(HistoryDir());
	}

	// Self-test only ("" = back to the real folder). The sandbox must be a direct child of ROOT.
	static void SetTestRoot(string root)
	{
		s_TestRoot = root;
	}
}

class INT_PTY_SE_Files
{
	static const int READ_MAX = 1000000;
	static string s_LastBroken;		// path of the last .broken copy (self-test)

	// Read result: READ_NONE = no file, READ_OK = read, READ_FAIL = the file exists but could not be opened (locked by
	// a backup tool / antivirus). A failed read must never be treated as an empty file (review PTY-02).
	static const int READ_NONE = 0;
	static const int READ_OK = 1;
	static const int READ_FAIL = -1;

	static int ReadState(string path, out string content)
	{
		content = "";
		if (!FileExist(path))
			return READ_NONE;
		FileHandle fh = OpenFile(path, FileMode.READ);
		if (fh == 0)
			return READ_FAIL;
		string txt;
		ReadFile(fh, txt, READ_MAX);
		CloseFile(fh);
		content = txt;
		return READ_OK;
	}

	static bool ReadText(string path, out string content)
	{
		string txt;
		int st = ReadState(path, txt);
		content = txt;
		return st == READ_OK;
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

	// No string.Replace on the whole text: it corrupts strings over 2560 bytes (review PTY-04, ~18 friends).
	static bool SameContent(string a, string b)
	{
		return INT_PTY_Util.SameText(a, b);
	}

	// Copy of a file that cannot be used; returns the copy's path ("" when nothing was copied).
	static string KeepBroken(string path)
	{
		if (!FileExist(path))
			return "";
		string dst = path + ".broken-" + INT_PTY_Util.FileStamp();
		if (CopyFile(path, dst))
		{
			s_LastBroken = dst;
			return dst;
		}
		return "";
	}

	// tmp -> re-read -> replace -> re-read. A crash between the steps leaves <path>.tmp, which Recover() puts back.
	// false = the old content is still in place (or the tmp copy is left for Recover).
	static bool SaveText(string path, string content)
	{
		string tmp = path + ".tmp";
		if (!WriteText(tmp, content))
		{
			DeleteFile(tmp);
			INT_PTY_Log.Err("запись: не удалось создать " + tmp);
			return false;
		}
		string back;
		if (!ReadText(tmp, back) || !SameContent(back, content))
		{
			DeleteFile(tmp);
			INT_PTY_Log.Err("запись: проверка временного файла не прошла " + tmp);
			return false;
		}
		if (FileExist(path) && !DeleteFile(path))
		{
			DeleteFile(tmp);
			INT_PTY_Log.Err("запись: не удалось заменить " + path);
			return false;
		}
		string fin;
		bool ok = CopyFile(tmp, path);
		if (ok)
			ok = ReadText(path, fin) && SameContent(fin, content);
		if (ok)
		{
			DeleteFile(tmp);
			return true;
		}
		INT_PTY_Log.Crit("запись: копия не удалась, оставлен " + tmp);
		return false;
	}

	// A save interrupted after the old file was removed: put <path>.tmp back. true = path exists afterwards.
	// A complete JSON object written by us ends with '}' (an interrupted copy does not).
	static bool LooksComplete(string txt)
	{
		int n = txt.Length();
		while (n > 0)
		{
			string ch = txt.Substring(n - 1, 1);
			if (ch != " " && ch != "\n" && ch != "\r" && ch != "\t")
				return ch == "}";
			n--;
		}
		return false;
	}

	// An interrupted save leaves <path>.tmp (the verified new text). With both files present the main one wins only
	// when it is complete; a cut main file is replaced by a complete .tmp (review PTY-03). true = path exists after.
	static bool Recover(string path)
	{
		string tmp = path + ".tmp";
		if (!FileExist(tmp))
			return FileExist(path);
		string tmpTxt;
		bool tmpOk = ReadText(tmp, tmpTxt) && LooksComplete(tmpTxt);
		if (FileExist(path))
		{
			string mainTxt;
			int st = ReadState(path, mainTxt);
			if (st == READ_FAIL)
				return true;
			if (st == READ_OK && LooksComplete(mainTxt))
			{
				DeleteFile(tmp);
				return true;
			}
			if (!tmpOk)
				return true;
			KeepBroken(path);
			DeleteFile(path);
		}
		if (!tmpOk)
			return FileExist(path);
		if (!CopyFile(tmp, path))
			return false;
		DeleteFile(tmp);
		INT_PTY_Log.Warn("восстановлен после прерванной записи: " + path);
		return true;
	}
}

class INT_PTY_SE_Json<Class T>
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
			INT_PTY_Log.Err("JSON не собирается: " + path);
			return false;
		}
		return INT_PTY_SE_Files.SaveText(path, txt);
	}
}
