// INT GlobalChat — public GUI API, part 1: the style object (spec.md §7, readme §6).
// Every field is public and may be changed by another mod (a global GUI editor), then INT_GC_UI.ApplyStyle().
// Colours: 0 = the default of the «Латунь» look. Sizes are design pixels of a 1920x1080 screen (scaled with the
// screen). The player's own choices (place, size, font, opacity, fade, time) live here too and are saved in the
// client file: the player's place IS the API place. Fields are only ever ADDED (never renamed or removed).

class INT_GC_Style
{
	bool Visible = true;		// external hide (cinematics etc.); the player's «eye» is INT_GC_Settings.Hidden
	float X = -1;				// top-left corner of the window as a fraction of the screen; -1 = default (bottom left)
	float Y = -1;
	float W = 600;				// window size, design px
	float H = 500;
	float FontSize = 16;
	float Opacity = 0.88;		// panel opacity of the open window, 0.3 .. 1
	int FadeSec = 15;			// idle feed: a line fades after N s; 0 = lines never fade
	int IdleLines = 8;			// idle feed: at most N visual lines
	bool ShowTime = true;
	bool ShowCorners = true;	// brass corner brackets
	bool ShowHints = true;		// key hints under the open window
	bool IdleShadowPanel = false;	// idle feed: soft dark band behind the lines
	int Sort = 0;				// priority of the top-level frames (kept below 100)

	int PanelColor;
	int HeaderColor;
	int BorderColor;
	int AccentColor;			// brass: corners, ring, scroll thumb, focus
	int TextColor;
	int MutedColor;				// secondary text, counters
	int TimeColor;
	int SystemColor;			// СЕРВЕР lines
	int ModColor;				// automoderation lines
	int LocalColor;				// РЯДОМ tag
	int PmColor;				// ЛС tag and text
	int MentionColor;			// highlight of a line that mentions me
	int AdminColor;				// АДМИН plate
	int NoticeColor;			// personal notices
	int InputColor;				// input field fill

	void CopyFrom(INT_GC_Style o)
	{
		if (!o)
			return;
		Visible = o.Visible;
		X = o.X;
		Y = o.Y;
		W = o.W;
		H = o.H;
		FontSize = o.FontSize;
		Opacity = o.Opacity;
		FadeSec = o.FadeSec;
		IdleLines = o.IdleLines;
		ShowTime = o.ShowTime;
		ShowCorners = o.ShowCorners;
		ShowHints = o.ShowHints;
		IdleShadowPanel = o.IdleShadowPanel;
		Sort = o.Sort;
		PanelColor = o.PanelColor;
		HeaderColor = o.HeaderColor;
		BorderColor = o.BorderColor;
		AccentColor = o.AccentColor;
		TextColor = o.TextColor;
		MutedColor = o.MutedColor;
		TimeColor = o.TimeColor;
		SystemColor = o.SystemColor;
		ModColor = o.ModColor;
		LocalColor = o.LocalColor;
		PmColor = o.PmColor;
		MentionColor = o.MentionColor;
		AdminColor = o.AdminColor;
		NoticeColor = o.NoticeColor;
		InputColor = o.InputColor;
	}

	// Keeps values usable; returns the number of corrections.
	int Sanitize()
	{
		int fixes = 0;
		float c = Math.Clamp(W, INT_GC_Style.MIN_W, INT_GC_Style.MAX_W);
		if (c != W)
		{
			W = c;
			fixes++;
		}
		c = Math.Clamp(H, INT_GC_Style.MIN_H, INT_GC_Style.MAX_H);
		if (c != H)
		{
			H = c;
			fixes++;
		}
		c = Math.Clamp(FontSize, 12, 26);
		if (c != FontSize)
		{
			FontSize = c;
			fixes++;
		}
		c = Math.Clamp(Opacity, 0.3, 1.0);
		if (c != Opacity)
		{
			Opacity = c;
			fixes++;
		}
		int ci = Math.Clamp(FadeSec, 0, 300);
		if (ci != FadeSec)
		{
			FadeSec = ci;
			fixes++;
		}
		ci = Math.Clamp(IdleLines, 1, 20);
		if (ci != IdleLines)
		{
			IdleLines = ci;
			fixes++;
		}
		if (X > 1 || Y > 1 || (X < 0 && X != -1) || (Y < 0 && Y != -1))
		{
			X = -1;
			Y = -1;
			fixes++;
		}
		if (Sort < 0 || Sort > 99)
		{
			Sort = 0;
			fixes++;
		}
		return fixes;
	}

	static const float MIN_W = 380;
	static const float MAX_W = 1200;
	static const float MIN_H = 220;
	static const float MAX_H = 900;
}

class INT_GC_BlockEntry
{
	string Id;		// GetId() hash ("" for a nick seen only in the local channel)
	string Name;
}

// Personal, non-visual choices of the player (client file).
class INT_GC_Settings
{
	int ColorIdx = 15;
	bool Sound = true;
	bool MentionSound = true;
	bool Hidden = false;		// the «eye»: idle feed hidden (Enter still opens the window)
	int Channel = 0;			// INT_GC_Const.CH_GLOBAL / CH_LOCAL
	ref array<ref INT_GC_BlockEntry> Blocked;

	void INT_GC_Settings()
	{
		Blocked = new array<ref INT_GC_BlockEntry>();
	}

	int Sanitize()
	{
		int fixes = 0;
		int ci = INT_GC_Palette.Clamp(ColorIdx);
		if (ci != ColorIdx)
		{
			ColorIdx = ci;
			fixes++;
		}
		if (Channel != INT_GC_Const.CH_GLOBAL && Channel != INT_GC_Const.CH_LOCAL)
		{
			Channel = INT_GC_Const.CH_GLOBAL;
			fixes++;
		}
		if (!Blocked)
		{
			Blocked = new array<ref INT_GC_BlockEntry>();
			fixes++;
		}
		for (int i = Blocked.Count() - 1; i >= 0; i--)
		{
			INT_GC_BlockEntry e = Blocked.Get(i);
			if (!e || (e.Id == "" && e.Name == ""))
			{
				Blocked.RemoveOrdered(i);
				fixes++;
			}
		}
		return fixes;
	}

	bool IsBlocked(string id, string name)
	{
		string lname = INT_GC_Util.Lower(name);
		foreach (INT_GC_BlockEntry e : Blocked)
		{
			if (id != "" && e.Id == id)
				return true;
			if (name != "" && e.Name != "" && INT_GC_Util.Lower(e.Name) == lname)
				return true;
		}
		return false;
	}

	// Returns true when the list changed.
	bool Block(string id, string name)
	{
		if (IsBlocked(id, name))
			return false;
		INT_GC_BlockEntry e = new INT_GC_BlockEntry();
		e.Id = id;
		e.Name = INT_GC_Util.CleanText(name, 64);
		Blocked.Insert(e);
		return true;
	}

	bool Unblock(string id, string name)
	{
		bool changed = false;
		string lname = INT_GC_Util.Lower(name);
		for (int i = Blocked.Count() - 1; i >= 0; i--)
		{
			INT_GC_BlockEntry e = Blocked.Get(i);
			bool same = false;
			if (id != "" && e.Id == id)
				same = true;
			if (name != "" && INT_GC_Util.Lower(e.Name) == lname)
				same = true;
			if (same)
			{
				Blocked.RemoveOrdered(i);
				changed = true;
			}
		}
		return changed;
	}
}

// The client file $profile:INT_Mods/INT_GlobalChat/INT_GlobalChat_Client.json (spec.md §5.7).
class INT_GC_ClientFile
{
	int V = 1;
	float X = -1;
	float Y = -1;
	float W = 600;
	float H = 500;
	float FontSize = 16;
	float Opacity = 0.88;
	int FadeSec = 15;
	bool ShowTime = true;
	ref INT_GC_Settings Settings;

	void INT_GC_ClientFile()
	{
		Settings = new INT_GC_Settings();
	}

	static const string DIR = "$profile:INT_Mods/INT_GlobalChat";
	static const string PATH = "$profile:INT_Mods/INT_GlobalChat/INT_GlobalChat_Client.json";

	static bool Save(INT_GC_Style st, INT_GC_Settings se)
	{
		INT_GC_ClientFile f = new INT_GC_ClientFile();
		f.X = st.X;
		f.Y = st.Y;
		f.W = st.W;
		f.H = st.H;
		f.FontSize = st.FontSize;
		f.Opacity = st.Opacity;
		f.FadeSec = st.FadeSec;
		f.ShowTime = st.ShowTime;
		f.Settings = se;
		string txt;
		JsonSerializer js = new JsonSerializer();
		if (!js.WriteToString(f, true, txt))
			return false;
		if (!FileExist("$profile:INT_Mods"))
			MakeDirectory("$profile:INT_Mods");
		if (!FileExist(DIR))
			MakeDirectory(DIR);
		FileHandle fh = OpenFile(PATH, FileMode.WRITE);
		if (fh == 0)
		{
			INT_GC_Log.Warn("не удалось сохранить настройки чата");
			return false;
		}
		FPrint(fh, txt);
		CloseFile(fh);
		return true;
	}

	// Fills st / se from the file; false when there is no usable file (defaults stay).
	static bool Load(INT_GC_Style st, INT_GC_Settings se)
	{
		if (!FileExist(PATH))
			return false;
		FileHandle fh = OpenFile(PATH, FileMode.READ);
		if (fh == 0)
			return false;
		string txt;
		ReadFile(fh, txt, 262144);
		CloseFile(fh);
		INT_GC_ClientFile f = new INT_GC_ClientFile();
		string err;
		JsonSerializer js = new JsonSerializer();
		if (!js.ReadFromString(f, txt, err))
		{
			INT_GC_Log.Warn("файл настроек чата не читается: " + err);
			return false;
		}
		st.X = f.X;
		st.Y = f.Y;
		st.W = f.W;
		st.H = f.H;
		st.FontSize = f.FontSize;
		st.Opacity = f.Opacity;
		st.FadeSec = f.FadeSec;
		st.ShowTime = f.ShowTime;
		st.Sanitize();
		if (f.Settings)
		{
			f.Settings.Sanitize();
			se.ColorIdx = f.Settings.ColorIdx;
			se.Sound = f.Settings.Sound;
			se.MentionSound = f.Settings.MentionSound;
			se.Hidden = f.Settings.Hidden;
			se.Channel = f.Settings.Channel;
			se.Blocked.Clear();
			foreach (INT_GC_BlockEntry b : f.Settings.Blocked)
				se.Blocked.Insert(b);
		}
		return true;
	}
}
