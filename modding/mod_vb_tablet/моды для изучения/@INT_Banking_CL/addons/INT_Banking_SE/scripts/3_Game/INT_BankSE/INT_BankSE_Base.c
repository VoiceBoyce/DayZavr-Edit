// INT Banking (server) - profile layout, wall clock, admin log.
//
// $profile:INT_Mods/INT_Banking/
//   INT_BankingConfig.json            settings + currency list (written with defaults on first start)
//   PlayersAccounts/<Steam64>.json    one account per player
//   INT_BankingLogs/INT_Banking_<YYYY-MM-DD_HH-MM-SS>.log   admin log, a new file per server start
//
// Files are only read and written by name: FindFile does not list $profile: on 1.30 (measured
// 2026-10-15), and nothing here needs a directory listing.

class INT_BankSE_Paths
{
	static const string BASE = "$profile:INT_Mods";
	static const string ROOT = "$profile:INT_Mods/INT_Banking";

	static string Config()
	{
		return ROOT + "/INT_BankingConfig.json";
	}

	static string AccDir()
	{
		return ROOT + "/PlayersAccounts";
	}

	static string LogDir()
	{
		return ROOT + "/INT_BankingLogs";
	}

	static string AccFile(string uid)
	{
		string dir = AccDir();
		string safe = SafeName(uid);
		return dir + "/" + safe + ".json";
	}

	// File-name safe copy: only [0-9A-Za-z_-] survive (Steam64 ids are digits anyway).
	static string SafeName(string raw)
	{
		string allowed = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-";
		string res = "";
		int n = raw.Length();
		for (int i = 0; i < n; i++)
		{
			string ch = raw.Get(i);
			if (allowed.IndexOf(ch) != -1)
				res = res + ch;
		}
		if (res == "")
			res = "_";
		return res;
	}

	static bool EnsureDir(string dir)
	{
		if (FileExist(dir))
			return true;
		MakeDirectory(dir);
		return FileExist(dir);
	}

	// MakeDirectory is not recursive.
	static bool EnsureAll()
	{
		bool ok = true;
		if (!EnsureDir(BASE))
			ok = false;
		if (!EnsureDir(ROOT))
			ok = false;
		if (!EnsureDir(AccDir()))
			ok = false;
		if (!EnsureDir(LogDir()))
			ok = false;
		return ok;
	}
}

// Wall clock of the server machine (local time, as the admin reads it).
class INT_BankSE_Time
{
	static string Pad2(int v)
	{
		if (v < 10)
			return "0" + v.ToString();
		return v.ToString();
	}

	// "2026-10-15 21:30:05"
	static string Stamp()
	{
		int y;
		int mo;
		int d;
		GetYearMonthDay(y, mo, d);
		int h;
		int mi;
		int s;
		GetHourMinuteSecond(h, mi, s);
		string sy = y.ToString();
		string smo = Pad2(mo);
		string sd = Pad2(d);
		string sh = Pad2(h);
		string smi = Pad2(mi);
		string ss = Pad2(s);
		string res = sy + "-" + smo + "-" + sd + " " + sh + ":" + smi + ":" + ss;
		return res;
	}

	// "2026-10-15_21-30-05" (file names)
	static string FileStamp()
	{
		int y;
		int mo;
		int d;
		GetYearMonthDay(y, mo, d);
		int h;
		int mi;
		int s;
		GetHourMinuteSecond(h, mi, s);
		string sy = y.ToString();
		string smo = Pad2(mo);
		string sd = Pad2(d);
		string sh = Pad2(h);
		string smi = Pad2(mi);
		string ss = Pad2(s);
		string res = sy + "-" + smo + "-" + sd + "_" + sh + "-" + smi + "-" + ss;
		return res;
	}

	// "15.10.2026 21:30" (history rows)
	static string Short()
	{
		int y;
		int mo;
		int d;
		GetYearMonthDay(y, mo, d);
		int h;
		int mi;
		int s;
		GetHourMinuteSecond(h, mi, s);
		string sd = Pad2(d);
		string smo = Pad2(mo);
		string sy = y.ToString();
		string sh = Pad2(h);
		string smi = Pad2(mi);
		string res = sd + "." + smo + "." + sy + " " + sh + ":" + smi;
		return res;
	}

	// Days since 1970-01-01 (H. Hinnant's days_from_civil).
	static int DaysFromCivil(int y, int m, int d)
	{
		int yy = y;
		if (m <= 2)
			yy = yy - 1;
		int era;
		if (yy >= 0)
			era = yy / 400;
		else
			era = (yy - 399) / 400;
		int yoe = yy - era * 400;
		int mp;
		if (m > 2)
			mp = m - 3;
		else
			mp = m + 9;
		int doy = (153 * mp + 2) / 5 + d - 1;
		int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
		return era * 146097 + doe - 719468;
	}

	// UTC unix seconds (history ordering).
	static int Unix()
	{
		int h1;
		int mi1;
		int s1;
		GetHourMinuteSecondUTC(h1, mi1, s1);
		int y;
		int mo;
		int d;
		GetYearMonthDayUTC(y, mo, d);
		int h2;
		int mi2;
		int s2;
		GetHourMinuteSecondUTC(h2, mi2, s2);
		int t1 = h1 * 3600 + mi1 * 60 + s1;
		int t2 = h2 * 3600 + mi2 * 60 + s2;
		if (t2 < t1)
			GetYearMonthDayUTC(y, mo, d);
		int days = DaysFromCivil(y, mo, d);
		return days * 86400 + t2;
	}
}

// Admin log. One file per server start; every line carries the machine date and time.
// The file is opened in APPEND mode per line, so a crash loses nothing already logged.
// Lines are also printed to the script log with the tag [INT_BANK_SE].
class INT_BankSE_Log
{
	protected static string s_File = "";

	static void Init()
	{
		INT_BankSE_Paths.EnsureAll();
		string dir = INT_BankSE_Paths.LogDir();
		string fstamp = INT_BankSE_Time.FileStamp();
		string path = dir + "/INT_Banking_" + fstamp + ".log";
		FileHandle fh = OpenFile(path, FileMode.WRITE);
		if (fh == 0)
		{
			s_File = "";
			Print("[INT_BANK_SE] [ОШИБКА] не удалось создать файл лога " + path);
			return;
		}
		string head = INT_BankSE_Time.Stamp();
		head = head + " | СТАРТ | INT Banking " + INT_BANK_VERSION + " - лог запущен";
		FPrintln(fh, head);
		CloseFile(fh);
		s_File = path;
	}

	static string File()
	{
		return s_File;
	}

	static void Info(string msg)
	{
		Write("ИНФО", msg);
	}

	static void Warn(string msg)
	{
		Write("ВНИМАНИЕ", msg);
	}

	static void Err(string msg)
	{
		Write("ОШИБКА", msg);
	}

	static void Critical(string msg)
	{
		Write("КРИТИЧНО", msg);
	}

	// Money operation lines: "<time> | <KIND> | <details>".
	static void Op(string kind, string msg)
	{
		Write(kind, msg);
	}

	static void Test(string msg)
	{
		Write("ТЕСТ", msg);
	}

	// "Nick (7656119...)"
	static string Who(string name, string uid)
	{
		string n = INT_Bank_Util.CleanName(name, 32);
		if (n == "")
			n = "?";
		return n + " (" + uid + ")";
	}

	static string Pos(vector p)
	{
		int x = Math.Round(p[0]);
		int y = Math.Round(p[1]);
		int z = Math.Round(p[2]);
		string sx = x.ToString();
		string sy = y.ToString();
		string sz = z.ToString();
		return sx + " " + sy + " " + sz;
	}

	protected static void Write(string level, string raw)
	{
		// one line per message, and no bare "%" (the native writers format it)
		string msg = raw + "";
		msg.Replace("%", " pct");
		msg.Replace("\r", " ");
		msg.Replace("\n", " ");
		Print("[INT_BANK_SE] [" + level + "] " + msg);
		if (s_File == "")
			return;
		string line = INT_BankSE_Time.Stamp();
		line = line + " | " + level + " | " + msg;
		FileHandle fh = OpenFile(s_File, FileMode.APPEND);
		if (fh == 0)
			return;
		FPrintln(fh, line);
		CloseFile(fh);
	}
}
