// INT Banking (server) - recoverable JSON files (same contract as INT_P2P_FileStore).
//
// Save:  write <f>.tmp -> re-read + compare -> copy <f> to <f>.bak -> delete <f> -> copy tmp to <f>
//        -> re-read + compare -> delete tmp.
// DayZ has no rename (only DeleteFile / CopyFile), so the delete -> copy window is not atomic. A crash
// inside it leaves <f>.tmp and <f>.bak: Load() promotes a complete .tmp and falls back to .bak when
// <f> is missing or broken. SaveText() returning false means the destination still holds the OLD
// content, so a caller that aborts its operation on false stays consistent.

const int INT_BANKSE_LOAD_OK = 0;
const int INT_BANKSE_LOAD_MISSING = 1;
const int INT_BANKSE_LOAD_BROKEN = 2;

class INT_BankSE_File
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
		if (a == b)
			return true;
		string ca = a + "";
		string cb = b + "";
		ca.Replace("\r", "");
		cb.Replace("\r", "");
		return ca == cb;
	}

	// A complete JSON document starts with { or [ and ends with } or ] (whitespace and a UTF-8 BOM
	// from Notepad are skipped). Truncated writes fail this before the parser is asked.
	static bool LooksComplete(string txt)
	{
		int n = txt.Length();
		int i = 0;
		while (i < n && (IsWs(txt, i) || (i < 3 && IsBom(txt, i))))
			i++;
		int j = n - 1;
		while (j > i && IsWs(txt, j))
			j--;
		if (j - i < 1)
			return false;
		string first = txt.Get(i);
		string last = txt.Get(j);
		if (first == "{" && last == "}")
			return true;
		if (first == "[" && last == "]")
			return true;
		return false;
	}

	static bool IsWs(string s, int index)
	{
		string ch = s.Get(index);
		if (ch == " " || ch == "\t" || ch == "\r" || ch == "\n")
			return true;
		return false;
	}

	static bool IsBom(string s, int index)
	{
		string ch = s.Get(index);
		int code = ch.ToAscii();
		if (code < 0)
			code = code + 256;
		if (code == 239 || code == 187 || code == 191)
			return true;
		return false;
	}

	static string StripBom(string txt)
	{
		int n = txt.Length();
		if (n < 3 || n > 8000)
			return txt;
		if (!IsBom(txt, 0) || !IsBom(txt, 1) || !IsBom(txt, 2))
			return txt;
		return txt.Substring(3, n - 3);
	}

	// Keeps a copy of a bad file as evidence: <f>.broken-<stamp>.
	static void KeepBroken(string path)
	{
		if (!FileExist(path))
			return;
		string dst = path + ".broken-" + INT_BankSE_Time.FileStamp();
		if (CopyFile(path, dst))
			INT_BankSE_Log.Err("битый файл сохранён как " + dst);
		else
			INT_BankSE_Log.Err("не удалось сохранить копию битого файла " + path);
	}

	static bool SaveText(string path, string content)
	{
		string tmp = path + ".tmp";
		string bak = path + ".bak";

		if (!WriteText(tmp, content))
		{
			DeleteFile(tmp);
			INT_BankSE_Log.Err("запись: не удалось записать " + tmp);
			return false;
		}
		string back;
		bool readBack = ReadText(tmp, back);
		if (!readBack || !SameContent(back, content))
		{
			DeleteFile(tmp);
			INT_BankSE_Log.Err("запись: проверка временного файла не прошла " + tmp);
			return false;
		}

		bool hadMain = FileExist(path);
		if (hadMain)
		{
			if (FileExist(bak))
				DeleteFile(bak);
			if (!CopyFile(path, bak))
			{
				DeleteFile(tmp);
				INT_BankSE_Log.Err("запись: не удалось сделать .bak " + bak);
				return false;
			}
			if (!DeleteFile(path))
			{
				DeleteFile(tmp);
				INT_BankSE_Log.Err("запись: не удалось заменить " + path);
				return false;
			}
		}

		bool copied = CopyFile(tmp, path);
		bool verified = false;
		if (copied)
		{
			string fin;
			bool readFin = ReadText(path, fin);
			if (readFin && SameContent(fin, content))
				verified = true;
		}
		if (verified)
		{
			DeleteFile(tmp);
			return true;
		}

		// Replace failed half way: put the old content back so the caller's abort is consistent.
		if (FileExist(path))
			DeleteFile(path);
		if (!hadMain)
		{
			DeleteFile(tmp);
			INT_BankSE_Log.Err("запись: копирование не удалось (новый файл) " + path);
			return false;
		}
		if (CopyFile(bak, path))
		{
			DeleteFile(tmp);
			INT_BankSE_Log.Err("запись: копирование не удалось, старое содержимое восстановлено " + path);
			return false;
		}
		INT_BankSE_Log.Critical("запись: копирование и восстановление .bak не удались, .tmp оставлен до следующего старта: " + path);
		return false;
	}
}

// JSON object <-> file. JsonSerializer fills an ALLOCATED object (constructors allocate the arrays).
class INT_BankSE_Json<Class T>
{
	static string ToJson(T data, bool pretty)
	{
		JsonSerializer js = new JsonSerializer();
		string txt;
		if (!js.WriteToString(data, pretty, txt))
			return "";
		return txt;
	}

	static bool Parse(string txt, T data, string path)
	{
		if (!INT_BankSE_File.LooksComplete(txt))
			return false;
		string clean = INT_BankSE_File.StripBom(txt);
		JsonSerializer js = new JsonSerializer();
		string err;
		bool ok = js.ReadFromString(data, clean, err);
		if (!ok)
			INT_BankSE_Log.Err("ошибка JSON в " + path + ": " + err);
		return ok;
	}

	static bool Save(string path, T data)
	{
		string txt = ToJson(data, true);
		if (txt == "")
		{
			INT_BankSE_Log.Err("не удалось сформировать JSON для " + path);
			return false;
		}
		return INT_BankSE_File.SaveText(path, txt);
	}

	// OK / MISSING / BROKEN. Order of trust: complete .tmp (newest intent) > main > .bak.
	// A broken main is copied to .broken-<stamp> before anything replaces it.
	static int Load(string path, T data)
	{
		string tmp = path + ".tmp";
		string bak = path + ".bak";
		string txt;

		if (FileExist(tmp))
		{
			bool tmpRead = INT_BankSE_File.ReadText(tmp, txt);
			if (tmpRead && Parse(txt, data, tmp))
			{
				INT_BankSE_Log.Warn("найден незавершённый .tmp, восстановлен: " + path);
				if (!INT_BankSE_File.SaveText(path, txt))
					INT_BankSE_Log.Critical("восстановление из .tmp не удалось " + path);
				return INT_BANKSE_LOAD_OK;
			}
			INT_BankSE_File.KeepBroken(tmp);
			DeleteFile(tmp);
		}

		bool mainExists = FileExist(path);
		if (mainExists)
		{
			bool mainRead = INT_BankSE_File.ReadText(path, txt);
			if (mainRead && Parse(txt, data, path))
				return INT_BANKSE_LOAD_OK;
			INT_BankSE_File.KeepBroken(path);
		}

		// The .bak is only a fallback for a MISSING main (an interrupted save). A main that exists but
		// does not parse is usually a hand edit with a typo: restoring the older .bak over it would
		// silently undo the admin's change, so the account is reported broken instead.
		if (!mainExists && FileExist(bak))
		{
			string btxt;
			bool bakRead = INT_BankSE_File.ReadText(bak, btxt);
			if (bakRead && Parse(btxt, data, bak))
			{
				INT_BankSE_Log.Warn("основной файл отсутствовал, восстановлено из .bak: " + path);
				if (!INT_BankSE_File.SaveText(path, btxt))
					INT_BankSE_Log.Critical("восстановление из .bak не удалось " + path);
				return INT_BANKSE_LOAD_OK;
			}
		}

		if (mainExists)
			return INT_BANKSE_LOAD_BROKEN;
		return INT_BANKSE_LOAD_MISSING;
	}
}
