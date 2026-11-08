// INT PartyMode (client) — the player's own settings (spec.md §8, brief 31): kept on HIS PC in
// $profile:INT_Mods/INT_PartyMode/INT_PartyMode_Client.json — everything of the «Настройки» tab, who is shown, the
// alias and the ping look of every friend (keyed by Steam64, so it follows the friend across servers). Lost on a
// reinstall — by design (owner D4).

class INT_PTY_FriendPref
{
	string Id;
	string Alias;			// the player's own name for this friend ("" = his nick)
	int Shown;				// 1 = drawn on the HUD (counts against the server display limit)
	int PingShape;			// -1 = how I see pings by default, else INT_PTY_Shape
	int PingColor;			// 0 = default colour, else ARGB
	int Color;				// 0 = automatic friend colour, else ARGB
}

class INT_PTY_Shape
{
	static const int CROSS = 0;
	static const int CHECK = 1;
	static const int CIRCLE = 2;
	static const int DIAMOND = 3;
	static const int TRIANGLE = 4;
	static const int EXCL = 5;
	static const int COUNT = 6;
}

class INT_PTY_HpStyle
{
	// name tags
	static const int TAG_DOT = 0;
	static const int TAG_BAR = 1;
	static const int TAG_PCT = 2;
	static const int TAG_NUM = 3;
	static const int TAG_NONE = 4;
	// tracker
	static const int TRK_PCT = 0;
	static const int TRK_BAR = 1;
	static const int TRK_DOT = 2;
	static const int TRK_NUM = 3;
}

class INT_PTY_Settings
{
	int Version = 1;

	// name tags above friends
	int TagsOn = 1;
	int TagsOnDead = 1;
	int TagDistance = 900;			// clamped to the server MaxTagDistance
	int TagHpStyle = 1;
	int TagNameAlpha = 85;			// % opacity
	int TagDistAlpha = 70;
	int TagSize = 100;				// %

	// tracker panel
	int TrackerOn = 1;
	int TrackerHpStyle = 1;
	int TrackerAlpha = 90;
	int TrackerSize = 100;			// % (owner 2026-11-01: PvP players found 5 rows too big)
	int TrackerBlink = 1;			// blink when HP < 20 %
	int TrackerOffline = 1;			// show offline friends (greyed)
	float TrackerX = -1;			// screen fraction of the top-left corner, -1 = default place
	float TrackerY = -1;

	// ping markers
	int PingShape = 0;
	int PingColor = -42417;			// 0xFFFF5A4F
	int PingSize = 100;				// %
	int PingViewM = 1000;			// a friend's ping farther than this is not drawn (owner 2026-11-01: default 1 km, max 2 km)
	int PingFarM = 800;				// fully visible farther than this
	int PingNearM = 20;				// fully transparent closer than this
	int PingAimDim = 60;			// % of opacity removed while the crosshair is on a ping
	int PingAimZone = 100;			// % of the standard dim zone around the crosshair (owner D21/D22)

	// extras (owner D11)
	int EdgeArrows = 1;
	int StatusIcons = 1;

	// notifications / interface
	int NotifyOnline = 1;
	int NotifyInvite = 1;
	int NotifySounds = 1;
	int Tooltips = 1;
	int ClickSounds = 1;

	ref array<ref INT_PTY_FriendPref> Friends;

	void INT_PTY_Settings()
	{
		Friends = new array<ref INT_PTY_FriendPref>();
	}

	INT_PTY_FriendPref Pref(string id)
	{
		foreach (INT_PTY_FriendPref p : Friends)
		{
			if (p && p.Id == id)
				return p;
		}
		return null;
	}

	// The pref of a friend, created (shown, defaults) when he has none yet.
	INT_PTY_FriendPref PrefOrNew(string id)
	{
		INT_PTY_FriendPref p = Pref(id);
		if (p)
			return p;
		p = new INT_PTY_FriendPref();
		p.Id = id;
		p.Alias = "";
		p.Shown = 1;
		p.PingShape = -1;
		p.PingColor = 0;
		p.Color = 0;
		Friends.Insert(p);
		return p;
	}

	protected int Fit(int v, int lo, int hi, inout int fixes)
	{
		int c = Math.Clamp(v, lo, hi);
		if (c != v)
			fixes++;
		return c;
	}

	int Sanitize()
	{
		int fixes = 0;
		TagsOn = Fit(TagsOn, 0, 1, fixes);
		TagsOnDead = Fit(TagsOnDead, 0, 1, fixes);
		TagDistance = Fit(TagDistance, 50, 20000, fixes);
		TagHpStyle = Fit(TagHpStyle, 0, 4, fixes);
		TagNameAlpha = Fit(TagNameAlpha, 10, 100, fixes);
		TagDistAlpha = Fit(TagDistAlpha, 10, 100, fixes);
		TagSize = Fit(TagSize, 60, 160, fixes);
		TrackerOn = Fit(TrackerOn, 0, 1, fixes);
		TrackerHpStyle = Fit(TrackerHpStyle, 0, 3, fixes);
		TrackerAlpha = Fit(TrackerAlpha, 20, 100, fixes);
		TrackerSize = Fit(TrackerSize, 60, 160, fixes);
		TrackerBlink = Fit(TrackerBlink, 0, 1, fixes);
		TrackerOffline = Fit(TrackerOffline, 0, 1, fixes);
		if (TrackerX > 1 || TrackerY > 1 || (TrackerX < 0 && TrackerX != -1) || (TrackerY < 0 && TrackerY != -1))
		{
			TrackerX = -1;
			TrackerY = -1;
			fixes++;
		}
		PingShape = Fit(PingShape, 0, INT_PTY_Shape.COUNT - 1, fixes);
		PingSize = Fit(PingSize, 60, 160, fixes);
		PingViewM = Fit(PingViewM, INT_PTY_Const.PING_VIEW_MIN, INT_PTY_Const.PING_VIEW_MAX, fixes);
		PingFarM = Fit(PingFarM, 20, 5000, fixes);
		PingNearM = Fit(PingNearM, 0, 500, fixes);
		if (PingNearM >= PingFarM)
		{
			PingNearM = 0;
			fixes++;
		}
		PingAimDim = Fit(PingAimDim, 0, 100, fixes);
		PingAimZone = Fit(PingAimZone, 50, 300, fixes);
		EdgeArrows = Fit(EdgeArrows, 0, 1, fixes);
		StatusIcons = Fit(StatusIcons, 0, 1, fixes);
		NotifyOnline = Fit(NotifyOnline, 0, 1, fixes);
		NotifyInvite = Fit(NotifyInvite, 0, 1, fixes);
		NotifySounds = Fit(NotifySounds, 0, 1, fixes);
		Tooltips = Fit(Tooltips, 0, 1, fixes);
		ClickSounds = Fit(ClickSounds, 0, 1, fixes);
		if (!Friends)
		{
			Friends = new array<ref INT_PTY_FriendPref>();
			fixes++;
		}
		array<ref INT_PTY_FriendPref> kept = new array<ref INT_PTY_FriendPref>();
		array<string> seen = new array<string>();
		foreach (INT_PTY_FriendPref p : Friends)
		{
			if (!p || !INT_PTY_Util.IsSteamId(p.Id) || seen.Find(p.Id) >= 0)
			{
				fixes++;
				continue;
			}
			p.Alias = INT_PTY_Util.CleanName(p.Alias);
			p.Shown = Fit(p.Shown, 0, 1, fixes);
			p.PingShape = Fit(p.PingShape, -1, INT_PTY_Shape.COUNT - 1, fixes);
			seen.Insert(p.Id);
			kept.Insert(p);
		}
		Friends = kept;
		return fixes;
	}
}

// Recoverable client-side files (review PTY-11): write <path>.tmp -> read back -> replace -> read back. A save that was
// cut after the old file was removed leaves the .tmp, which Read() puts back. A file that exists but cannot be parsed
// is copied to <path>.broken-<date> once before anything overwrites it (KeepBroken).
class INT_PTY_ClientFile
{
	static bool Read(string path, out string txt)
	{
		txt = "";
		string tmp = path + ".tmp";
		if (!FileExist(path) && FileExist(tmp))
		{
			if (CopyFile(tmp, path))
				DeleteFile(tmp);
		}
		if (!FileExist(path))
			return false;
		FileHandle fh = OpenFile(path, FileMode.READ);
		if (fh == 0)
			return false;
		string content;
		ReadFile(fh, content, 1000000);
		CloseFile(fh);
		txt = content;
		return true;
	}

	protected static bool WriteRaw(string path, string content)
	{
		FileHandle fh = OpenFile(path, FileMode.WRITE);
		if (fh == 0)
			return false;
		FPrint(fh, content);
		CloseFile(fh);
		return true;
	}

	static bool SaveText(string path, string content)
	{
		string tmp = path + ".tmp";
		if (!WriteRaw(tmp, content))
			return false;
		string back;
		FileHandle rh = OpenFile(tmp, FileMode.READ);
		if (rh == 0)
			return false;
		ReadFile(rh, back, 1000000);
		CloseFile(rh);
		if (!INT_PTY_Util.SameText(back, content))
		{
			DeleteFile(tmp);
			return false;
		}
		if (FileExist(path) && !DeleteFile(path))
		{
			DeleteFile(tmp);
			return false;
		}
		if (!CopyFile(tmp, path))
			return false;
		DeleteFile(tmp);
		return true;
	}

	static void KeepBroken(string path)
	{
		if (!FileExist(path))
			return;
		string dst = path + ".broken-" + INT_PTY_Util.FileStamp();
		CopyFile(path, dst);
	}
}

class INT_PTY_SettingsStore
{
	static const string DIR_MODS = "$profile:INT_Mods";
	static const string DIR = "$profile:INT_Mods/INT_PartyMode";
	static const string FILE = "$profile:INT_Mods/INT_PartyMode/INT_PartyMode_Client.json";

	protected static ref INT_PTY_Settings s_Data;
	protected static bool s_Dirty;

	static INT_PTY_Settings Get()
	{
		if (!s_Data)
			Load();
		return s_Data;
	}

	static void Load()
	{
		INT_PTY_Settings d = new INT_PTY_Settings();
		string txt;
		if (INT_PTY_ClientFile.Read(FILE, txt))
		{
			string err;
			JsonSerializer js = new JsonSerializer();
			if (!js.ReadFromString(d, txt, err))
			{
				INT_PTY_ClientFile.KeepBroken(FILE);
				INT_PTY_Log.Warn("настройки игрока не читаются (" + err + ") — копия .broken, умолчания");
				d = new INT_PTY_Settings();
			}
		}
		d.Sanitize();
		s_Data = d;
	}

	// Marks the file for writing; Flush() writes it at most once per call site burst (window close, mission end).
	static void MarkDirty()
	{
		s_Dirty = true;
	}

	static void Flush()
	{
		if (!s_Dirty || !s_Data)
			return;
		s_Dirty = false;
		if (!FileExist(DIR_MODS))
			MakeDirectory(DIR_MODS);
		if (!FileExist(DIR))
			MakeDirectory(DIR);
		string txt;
		JsonSerializer js = new JsonSerializer();
		if (!js.WriteToString(s_Data, true, txt))
			return;
		if (!INT_PTY_ClientFile.SaveText(FILE, txt))
		{
			s_Dirty = true;
			INT_PTY_Log.Warn("настройки игрока не сохранены: " + FILE);
		}
	}

	// A removed friend leaves nothing behind (review PTY-08: an old Shown = 1 must not bypass the display limit).
	static void ForgetFriend(string id)
	{
		INT_PTY_Settings st = Get();
		for (int i = st.Friends.Count() - 1; i >= 0; i--)
		{
			INT_PTY_FriendPref p = st.Friends.Get(i);
			if (p && p.Id == id)
				st.Friends.RemoveOrdered(i);
		}
		MarkDirty();
	}
}
