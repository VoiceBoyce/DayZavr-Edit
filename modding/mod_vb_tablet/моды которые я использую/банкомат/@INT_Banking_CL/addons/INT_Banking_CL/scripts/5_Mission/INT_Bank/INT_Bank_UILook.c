// INT Banking - interface sounds and the look (palette, theme, textures). Client only.
//
// Sounds: 2D on the UI mixer channel, the chain vanilla uses for menu music (missionmainmenu.c,
// dynamicmusicplayer.c): SoundParams -> SoundObjectBuilder -> SoundObject.SetKind(WAVEUI) ->
// SoundScene.Play2D -> AbstractWave.Play. Sound sets: CL config.cpp, INT_Bank_<Kind>_SoundSet.

enum INT_Bank_Sfx
{
	KEY,
	CASH_OUT,
	CASH_IN,
	CARD,
	NOTIFY,
	CLICK,
	TAB,
	SELECT,
	CONFIRM,
	ERROR,
	OPEN,
	CLOSE
}

class INT_Bank_UISfx
{
	static const float SAME_KIND_GAP = 0.04;
	static const int KEEP_WAVES = 8;

	protected static ref array<string> s_Sets;
	protected static ref array<float> s_LastAt;
	protected static ref array<bool> s_Valid;
	protected static ref array<AbstractWave> s_Waves;
	protected static int s_WaveAt;

	static void Play(int kind)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		EnsureInit();
		if (kind < 0 || kind >= s_Sets.Count())
			return;
		if (!s_Valid.Get(kind))
			return;
		float now = GetGame().GetTickTime();
		float last = s_LastAt.Get(kind);
		if (now - last < SAME_KIND_GAP)
			return;
		string setName = s_Sets.Get(kind);
		SoundParams soundParams = new SoundParams(setName);
		if (!soundParams.IsValid())
		{
			s_Valid.Set(kind, false);
			Print("[INT_BANK] interface sound set is not valid: " + setName);
			return;
		}
		SoundObjectBuilder soundBuilder = new SoundObjectBuilder(soundParams);
		SoundObject soundObject = soundBuilder.BuildSoundObject();
		if (!soundObject)
			return;
		soundObject.SetKind(WaveKind.WAVEUI);
		AbstractSoundScene scene = GetGame().GetSoundScene();
		if (!scene)
			return;
		AbstractWave wave = scene.Play2D(soundObject, soundBuilder);
		if (!wave)
			return;
		wave.Loop(false);
		wave.Play();
		s_LastAt.Set(kind, now);
		s_Waves.Set(s_WaveAt, wave);
		s_WaveAt++;
		if (s_WaveAt >= KEEP_WAVES)
			s_WaveAt = 0;
	}

	protected static void EnsureInit()
	{
		if (s_Sets)
			return;
		s_Sets = new array<string>();
		s_Sets.Insert("INT_Bank_Key_SoundSet");
		s_Sets.Insert("INT_Bank_CashOut_SoundSet");
		s_Sets.Insert("INT_Bank_CashIn_SoundSet");
		s_Sets.Insert("INT_Bank_Card_SoundSet");
		s_Sets.Insert("INT_Bank_Notify_SoundSet");
		s_Sets.Insert("INT_Bank_Click_SoundSet");
		s_Sets.Insert("INT_Bank_Tab_SoundSet");
		s_Sets.Insert("INT_Bank_Select_SoundSet");
		s_Sets.Insert("INT_Bank_Confirm_SoundSet");
		s_Sets.Insert("INT_Bank_Error_SoundSet");
		s_Sets.Insert("INT_Bank_Open_SoundSet");
		s_Sets.Insert("INT_Bank_Close_SoundSet");
		s_LastAt = new array<float>();
		s_Valid = new array<bool>();
		int count = s_Sets.Count();
		for (int i = 0; i < count; i++)
		{
			s_LastAt.Insert(-100);
			s_Valid.Insert(true);
		}
		s_Waves = new array<AbstractWave>();
		for (int w = 0; w < KEEP_WAVES; w++)
			s_Waves.Insert(null);
		s_WaveAt = 0;
	}
}

const string INT_BANK_TEX_AURORA = "INT_Banking_CL/gui/textures/int_bank_aurora_ca.paa";
const string INT_BANK_TEX_STRIPES = "INT_Banking_CL/gui/textures/int_bank_stripes_ca.paa";
const string INT_BANK_TEX_VIGNETTE = "INT_Banking_CL/gui/textures/int_bank_vignette_ca.paa";
const string INT_BANK_TEX_SHEEN = "INT_Banking_CL/gui/textures/int_bank_sheen_ca.paa";

// "Brass & parchment" (the INT_P2P_Trading look the owner likes): warm dark glass, brass accent,
// sand text, sage for incoming money, brick for outgoing.
class INT_Bank_UILook
{
	static const string THEME = "int_bank";

	protected static string s_Prev;
	protected static bool s_Registered;

	static void Register()
	{
		if (s_Registered)
			return;
		s_Registered = true;
		INT_UIF_Theme t = new INT_UIF_Theme(THEME, "INT Bank");
		t.SetColor(INT_UIF_ThemeKeys.OVERLAY, ARGB(160, 6, 5, 3));
		t.SetColor(INT_UIF_ThemeKeys.SHADOW, ARGB(200, 4, 2, 0));
		t.SetColor(INT_UIF_ThemeKeys.WINDOW, ARGB(252, 15, 12, 9));
		t.SetColor(INT_UIF_ThemeKeys.WINDOW_BORDER, ARGB(34, 255, 236, 200));
		t.SetColor(INT_UIF_ThemeKeys.HEADER, ARGB(252, 15, 12, 9));
		t.SetColor(INT_UIF_ThemeKeys.PANEL, ARGB(214, 24, 20, 16));
		t.SetColor(INT_UIF_ThemeKeys.PANEL_BORDER, ARGB(20, 255, 240, 215));
		t.SetColor(INT_UIF_ThemeKeys.TILE, ARGB(232, 33, 28, 22));
		t.SetColor(INT_UIF_ThemeKeys.TILE_HOVER, ARGB(240, 46, 39, 30));
		t.SetColor(INT_UIF_ThemeKeys.TILE_BORDER, ARGB(22, 255, 240, 215));
		t.SetColor(INT_UIF_ThemeKeys.SHEEN, ARGB(26, 255, 240, 215));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE, ARGB(214, 24, 20, 16));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE_ALT, ARGB(232, 33, 28, 22));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE_HOVER, ARGB(240, 46, 39, 30));
		t.SetColor(INT_UIF_ThemeKeys.BORDER, ARGB(26, 255, 240, 215));
		t.SetColor(INT_UIF_ThemeKeys.DIVIDER, ARGB(18, 255, 240, 215));
		t.SetColor(INT_UIF_ThemeKeys.TEXT, ARGB(255, 240, 232, 216));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_MUTED, ARGB(255, 170, 160, 143));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_FAINT, ARGB(255, 118, 107, 92));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_DISABLED, ARGB(255, 84, 76, 66));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_ON_ACCENT, ARGB(255, 28, 21, 13));
		t.SetColor(INT_UIF_ThemeKeys.TEAL, ARGB(255, 216, 174, 98));
		t.SetColor(INT_UIF_ThemeKeys.AMBER, ARGB(255, 226, 158, 72));
		t.SetColor(INT_UIF_ThemeKeys.AMBER_TEXT, ARGB(255, 238, 182, 110));
		t.SetColor(INT_UIF_ThemeKeys.RED, ARGB(255, 200, 92, 76));
		t.SetColor(INT_UIF_ThemeKeys.GREEN, ARGB(255, 160, 184, 122));
		t.SetColor(INT_UIF_ThemeKeys.BLUE, ARGB(255, 138, 166, 181));
		t.SetColor(INT_UIF_ThemeKeys.VIOLET, ARGB(255, 176, 150, 206));
		t.SetColor(INT_UIF_ThemeKeys.ACCENT, ARGB(255, 216, 174, 98));
		t.SetColor(INT_UIF_ThemeKeys.ACCENT_HOVER, ARGB(255, 232, 194, 124));
		t.SetColor(INT_UIF_ThemeKeys.ACCENT_SOFT, ARGB(40, 216, 174, 98));
		t.SetColor(INT_UIF_ThemeKeys.SECONDARY, ARGB(232, 33, 28, 22));
		t.SetColor(INT_UIF_ThemeKeys.SECONDARY_HOVER, ARGB(240, 46, 39, 30));
		t.SetColor(INT_UIF_ThemeKeys.SUCCESS, ARGB(255, 160, 184, 122));
		t.SetColor(INT_UIF_ThemeKeys.SUCCESS_HOVER, ARGB(255, 178, 202, 140));
		t.SetColor(INT_UIF_ThemeKeys.WARNING, ARGB(255, 226, 158, 72));
		t.SetColor(INT_UIF_ThemeKeys.WARNING_HOVER, ARGB(255, 238, 178, 100));
		t.SetColor(INT_UIF_ThemeKeys.DANGER, ARGB(255, 200, 92, 76));
		t.SetColor(INT_UIF_ThemeKeys.DANGER_HOVER, ARGB(255, 216, 116, 100));
		t.SetColor(INT_UIF_ThemeKeys.INFO, ARGB(255, 138, 166, 181));
		t.SetColor(INT_UIF_ThemeKeys.TRACK, ARGB(34, 255, 240, 215));
		t.SetColor(INT_UIF_ThemeKeys.KNOB, ARGB(255, 244, 236, 222));
		t.InheritFrom(INT_UIF_ThemePresets.Tactical());
		INT_UIF_ThemeManager.Get().Register(t);
	}

	// Our theme while the ATM is open; the previous one comes back on close.
	static void Activate()
	{
		Register();
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		INT_UIF_Theme cur = tm.Active();
		if (cur && cur.GetName() != THEME)
			s_Prev = cur.GetName();
		if (!cur || cur.GetName() != THEME)
			tm.SetActive(THEME);
	}

	static void Restore()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Peek();
		if (!tm)
			return;
		string back = s_Prev;
		if (back == "")
			back = "tactical";
		INT_UIF_Theme cur = tm.Active();
		if (cur && cur.GetName() == THEME)
			tm.SetActive(back);
	}

	static int Brass()
	{
		return ARGB(255, 216, 174, 98);
	}

	static int BrassLight()
	{
		return ARGB(255, 244, 214, 150);
	}

	static int Sand()
	{
		return ARGB(255, 203, 185, 148);
	}

	static int Ink()
	{
		return ARGB(255, 240, 232, 216);
	}

	static int Muted()
	{
		return ARGB(255, 170, 160, 143);
	}

	static int Faint()
	{
		return ARGB(255, 118, 107, 92);
	}

	static int Sage()
	{
		return ARGB(255, 160, 196, 122);
	}

	static int Brick()
	{
		return ARGB(255, 214, 104, 86);
	}

	static int Steel()
	{
		return ARGB(255, 138, 166, 181);
	}

	static int Glass()
	{
		return ARGB(214, 24, 20, 16);
	}

	static int GlassHi()
	{
		return ARGB(232, 33, 28, 22);
	}

	static int Hair()
	{
		return ARGB(24, 255, 240, 215);
	}

	// Text of a key (or plain text).
	static string Tr(string text)
	{
		if (text.IndexOf("#") == 0)
			return Widget.TranslateString(text);
		return text;
	}

	// "#STR_X" with %1 / %2 replaced.
	static string Fmt(string key, string a1, string a2 = "", string a3 = "")
	{
		string s = Tr(key);
		s.Replace("%1", a1);
		s.Replace("%2", a2);
		s.Replace("%3", a3);
		return s;
	}

	// "15 000 руб."
	static string Money(int v)
	{
		string m = INT_Bank_Util.Money(v);
		string cur = Tr("#STR_INT_BANK_CUR");
		return m + " " + cur;
	}

	// Letter-spaced capitals for small captions ("S E C U R E").
	static string Spaced(string text)
	{
		string t = Tr(text);
		t.ToUpper();
		string res = "";
		int n = t.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string ch = t.SubstringUtf8(i, 1);
			if (i > 0)
				res = res + " ";
			res = res + ch;
		}
		return res;
	}
}
