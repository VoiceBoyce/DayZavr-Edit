// INT PartyMode (server) — config file (spec.md §7). Read once at server start: no timers, no file watching
// (owner rule 2), so hand edits take effect after a restart.
//   missing file  -> created with defaults
//   unreadable    -> copy .broken-<date>, КРИТИЧНО line, rewritten with defaults
//   bad values    -> repaired (Sanitize); missing fields are added; the file is saved when anything changed

class INT_PTY_SE_ConfigData
{
	static const int CURRENT_VERSION = 1;

	string Comment = "INT PartyMode: описание полей - readme.md мода. MaxFriends 0 = без ограничения. Изменения - после рестарта.";
	int ConfigVersion = 1;
	int DebugMode = 0;
	int DisplayLimit = 10;
	int MaxTagDistance = 900;
	int PingLimit = 4;
	int MaxPingDistance = 2000;
	int PingCooldownMs = 400;
	int PullIntervalSec = 3;
	int PullMinIntervalSec = 2;
	int MaxRequestsPerMinute = 25;
	int MaxFriends = 0;
	int MaxOutgoingInvites = 10;
	int WriteHistoryLog = 1;

	protected int Bool01(int v)
	{
		if (v != 0)
			return 1;
		return 0;
	}

	protected int Fix(int v, int lo, int hi, inout int fixes)
	{
		int c = Math.Clamp(v, lo, hi);
		if (c != v)
			fixes++;
		return c;
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
			fixes++;
		DebugMode = b;
		b = Bool01(WriteHistoryLog);
		if (b != WriteHistoryLog)
			fixes++;
		WriteHistoryLog = b;
		DisplayLimit = Fix(DisplayLimit, 1, INT_PTY_Const.HARD_DISPLAY_LIMIT, fixes);
		MaxTagDistance = Fix(MaxTagDistance, 50, 20000, fixes);
		PingLimit = Fix(PingLimit, 1, INT_PTY_Const.HARD_PING_LIMIT, fixes);
		MaxPingDistance = Fix(MaxPingDistance, 50, 5000, fixes);
		PingCooldownMs = Fix(PingCooldownMs, 0, 10000, fixes);
		PullMinIntervalSec = Fix(PullMinIntervalSec, 1, 60, fixes);
		PullIntervalSec = Fix(PullIntervalSec, PullMinIntervalSec, 60, fixes);
		MaxRequestsPerMinute = Fix(MaxRequestsPerMinute, 5, 600, fixes);
		MaxFriends = Fix(MaxFriends, 0, 10000, fixes);
		MaxOutgoingInvites = Fix(MaxOutgoingInvites, 1, 100, fixes);
		return fixes;
	}
}

class INT_PTY_SE_Config
{
	protected static ref INT_PTY_SE_ConfigData s_Data;
	static string s_LoadNote;	// "created" | "ok" | "fixed" | "broken" (self-test)

	static INT_PTY_SE_ConfigData Get()
	{
		if (!s_Data)
			s_Data = new INT_PTY_SE_ConfigData();
		return s_Data;
	}

	static void Load()
	{
		INT_PTY_SE_Paths.MakeDirs();
		string path = INT_PTY_SE_Paths.ConfigFile();
		INT_PTY_SE_Files.Recover(path);
		INT_PTY_SE_ConfigData d = new INT_PTY_SE_ConfigData();
		string txt;
		string err;
		bool save = false;
		if (!INT_PTY_SE_Files.ReadText(path, txt))
		{
			s_LoadNote = "created";
			save = true;
			INT_PTY_Log.Info("конфиг не найден — создаю с настройками по умолчанию: " + path);
		}
		else if (!INT_PTY_SE_Json<INT_PTY_SE_ConfigData>.Parse(txt, d, err))
		{
			string copy = INT_PTY_SE_Files.KeepBroken(path);
			INT_PTY_Log.Crit("конфиг не читается (" + err + "), копия " + copy + ", пишу умолчания");
			d = new INT_PTY_SE_ConfigData();
			s_LoadNote = "broken";
			save = true;
		}
		else
		{
			s_LoadNote = "ok";
		}
		int fixes = d.Sanitize();
		if (fixes > 0)
		{
			INT_PTY_Log.Warn("конфиг: исправлено значений вне диапазона: " + fixes.ToString());
			if (s_LoadNote == "ok")
				s_LoadNote = "fixed";
			save = true;
		}
		// missing fields (an older file) come back with their defaults: the file is rewritten once
		string fresh;
		if (!save && INT_PTY_SE_Json<INT_PTY_SE_ConfigData>.ToText(d, true, fresh) && !INT_PTY_SE_Files.SameContent(fresh, txt))
			save = true;
		if (save && !INT_PTY_SE_Json<INT_PTY_SE_ConfigData>.Save(path, d))
			INT_PTY_Log.Err("конфиг не сохранён: " + path);
		s_Data = d;
		INT_PTY_Log.s_Debug = d.DebugMode == 1;
		INT_PTY_Log.Info("конфиг: лимит показа " + d.DisplayLimit.ToString() + ", метки до " + d.MaxTagDistance.ToString() + " м, пингов " + d.PingLimit.ToString() + ", запрос раз в " + d.PullIntervalSec.ToString() + " с, действий в минуту " + d.MaxRequestsPerMinute.ToString());
	}

	// Self-test: replace the live config.
	static void SetForTest(INT_PTY_SE_ConfigData d)
	{
		s_Data = d;
	}
}
