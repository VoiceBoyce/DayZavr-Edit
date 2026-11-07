// INT_PDA (client) — look of the picked style E «ЗОНА 2.0» (design/concepts_v1/5_e_zone2_*, owner 2026-10-23): matte
// black slate #0B0B0C, burnt orange #FF7A1A, warm off-white text, film grain; big thin tab words, orange underline.
// The INT_UIF components follow it through an own theme that is active only while the PDA is open (the previous theme
// comes back on close). The DayZavr logo is shown ONLY on the registration screen (owner 2026-10-23).
// Colours are plain constants: a call with two call-result arguments of one type gets the earlier one overwritten
// (measured 2026-10-12), so callers hoist colours into locals when they pass two of them.

class INT_PDA_Look
{
	static const string THEME = "int_pda_zone";

	// palette of concept E
	static const int BG = 0xFF0B0B0C;
	static const int PANEL = 0xFF131315;
	static const int PANEL2 = 0xFF19191C;
	static const int LINE = 0xFF2A2A2F;
	static const int LINE2 = 0xFF3A3A40;
	static const int TEXT = 0xFFECE8E1;
	static const int DIM = 0xFF8C8982;
	static const int DIM2 = 0xFF5F5D58;
	static const int ORANGE = 0xFFFF7A1A;
	static const int ORANGE2 = 0xFFC85A0A;
	static const int ORANGE_SOFT = 0x33FF7A1A;
	static const int OWN = 0xFFFF9A4D;
	static const int ALLY = 0xFF6FD0A0;
	static const int NEUTRAL = 0xFFB5B2AB;
	static const int ENEMY = 0xFFEC4A3C;
	static const int ADMIN = 0xFF8F7B66;
	static const int ON_ORANGE = 0xFF0B0B0C;
	static const int SHADOW = 0xB4000000;
	static const int WHITE = 0xFFFFFFFF;

	// device geometry (design px of the 1920x1080 canvas)
	static const float DEV_W = 1500;
	static const float DEV_H = 890;
	static const float SCR_X = 22;
	static const float SCR_Y = 24;
	static const float SCR_W = 1456;
	static const float SCR_H = 800;
	static const float TOP_H = 88;
	static const float FOOT_H = 38;
	static const float BODY_H = 674;

	// atlases (tools/gen_pda_art.py, gen_pda_avatars.py, gen_pda_emblems.py)
	static const string SET_DEV = "set:int_pda_dev image:";
	static const string SET_ICON = "set:int_pda_icons image:";
	static const string SET_AVATAR = "set:int_pda_avatars image:";
	static const string SET_EMBLEM = "set:int_pda_emblemsq image:";
	static const string LOGO = "set:int_pda_logo image:logo";
	static const string NOMAP = "set:int_pda_nomap image:static";

	protected static string s_Prev;
	protected static bool s_Registered;
	protected static bool s_Assets;

	static void EnsureAssets()
	{
		if (s_Assets || !GetGame() || GetGame().IsDedicatedServer())
			return;
		s_Assets = true;
		LoadWidgetStyles("INT_PDA_CL/gui/styles/int_pda.styles");
	}

	static void Register()
	{
		if (s_Registered)
			return;
		s_Registered = true;
		INT_UIF_Theme t = new INT_UIF_Theme(THEME, "INT PDA Zone");
		t.SetColor(INT_UIF_ThemeKeys.OVERLAY, ARGB(150, 4, 4, 5));
		t.SetColor(INT_UIF_ThemeKeys.SHADOW, ARGB(210, 0, 0, 0));
		t.SetColor(INT_UIF_ThemeKeys.WINDOW, ARGB(255, 11, 11, 12));
		t.SetColor(INT_UIF_ThemeKeys.WINDOW_BORDER, ARGB(255, 42, 42, 47));
		t.SetColor(INT_UIF_ThemeKeys.HEADER, ARGB(255, 11, 11, 12));
		t.SetColor(INT_UIF_ThemeKeys.PANEL, ARGB(255, 19, 19, 21));
		t.SetColor(INT_UIF_ThemeKeys.PANEL_BORDER, ARGB(255, 42, 42, 47));
		t.SetColor(INT_UIF_ThemeKeys.TILE, ARGB(255, 25, 25, 28));
		t.SetColor(INT_UIF_ThemeKeys.TILE_HOVER, ARGB(255, 34, 34, 38));
		t.SetColor(INT_UIF_ThemeKeys.TILE_BORDER, ARGB(255, 58, 58, 64));
		t.SetColor(INT_UIF_ThemeKeys.SHEEN, ARGB(10, 255, 122, 26));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE, ARGB(255, 19, 19, 21));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE_ALT, ARGB(255, 25, 25, 28));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE_HOVER, ARGB(255, 34, 34, 38));
		t.SetColor(INT_UIF_ThemeKeys.BORDER, ARGB(255, 58, 58, 64));
		t.SetColor(INT_UIF_ThemeKeys.DIVIDER, ARGB(255, 42, 42, 47));
		t.SetColor(INT_UIF_ThemeKeys.TEXT, ARGB(255, 236, 232, 225));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_MUTED, ARGB(255, 140, 137, 130));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_FAINT, ARGB(255, 95, 93, 88));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_DISABLED, ARGB(255, 70, 69, 66));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_ON_ACCENT, ARGB(255, 11, 11, 12));
		t.SetColor(INT_UIF_ThemeKeys.TEAL, ARGB(255, 255, 122, 26));
		t.SetColor(INT_UIF_ThemeKeys.AMBER, ARGB(255, 255, 122, 26));
		t.SetColor(INT_UIF_ThemeKeys.AMBER_TEXT, ARGB(255, 255, 154, 77));
		t.SetColor(INT_UIF_ThemeKeys.RED, ARGB(255, 236, 74, 60));
		t.SetColor(INT_UIF_ThemeKeys.GREEN, ARGB(255, 111, 208, 160));
		t.SetColor(INT_UIF_ThemeKeys.BLUE, ARGB(255, 150, 170, 190));
		t.SetColor(INT_UIF_ThemeKeys.VIOLET, ARGB(255, 172, 160, 220));
		t.SetColor(INT_UIF_ThemeKeys.ACCENT, ARGB(255, 255, 122, 26));
		t.SetColor(INT_UIF_ThemeKeys.ACCENT_HOVER, ARGB(255, 255, 150, 72));
		t.SetColor(INT_UIF_ThemeKeys.ACCENT_SOFT, ARGB(40, 255, 122, 26));
		t.SetColor(INT_UIF_ThemeKeys.SECONDARY, ARGB(255, 25, 25, 28));
		t.SetColor(INT_UIF_ThemeKeys.SECONDARY_HOVER, ARGB(255, 34, 34, 38));
		t.SetColor(INT_UIF_ThemeKeys.SUCCESS, ARGB(255, 111, 208, 160));
		t.SetColor(INT_UIF_ThemeKeys.SUCCESS_HOVER, ARGB(255, 140, 224, 182));
		t.SetColor(INT_UIF_ThemeKeys.WARNING, ARGB(255, 255, 154, 77));
		t.SetColor(INT_UIF_ThemeKeys.WARNING_HOVER, ARGB(255, 255, 178, 116));
		t.SetColor(INT_UIF_ThemeKeys.DANGER, ARGB(255, 236, 74, 60));
		t.SetColor(INT_UIF_ThemeKeys.DANGER_HOVER, ARGB(255, 246, 110, 98));
		t.SetColor(INT_UIF_ThemeKeys.INFO, ARGB(255, 181, 178, 171));
		t.SetColor(INT_UIF_ThemeKeys.TRACK, ARGB(255, 43, 43, 48));
		t.SetColor(INT_UIF_ThemeKeys.KNOB, ARGB(255, 236, 232, 225));
		t.InheritFrom(INT_UIF_ThemePresets.Tactical());
		INT_UIF_ThemeManager.Get().Register(t);
	}

	// Our theme while the PDA is open; the previous one comes back on close.
	static void Activate()
	{
		Register();
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		INT_UIF_Theme cur = tm.Active();
		if (cur && cur.GetName() == THEME)
			return;
		if (cur)
			s_Prev = cur.GetName();
		tm.SetActive(THEME);
	}

	static void Restore()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Peek();
		if (!tm)
			return;
		INT_UIF_Theme cur = tm.Active();
		if (!cur || cur.GetName() != THEME)
			return;
		string back = s_Prev;
		if (back == "" || !tm.Find(back))
			back = "tactical";
		tm.SetActive(back);
	}

	// --- images ---------------------------------------------------------------------------------------------------
	static string IconPath(string name)
	{
		return SET_ICON + name;
	}

	static string AvatarPath(int index)
	{
		int i = INT_PDA_Util.ClampI(index, 0, INT_PDA_Const.AVATARS - 1);
		return SET_AVATAR + "a" + i.ToStringLen(2);
	}

	static string EmblemPath(int index)
	{
		int i = INT_PDA_Util.ClampI(index, 1, INT_PDA_Const.EMBLEMS);
		return SET_EMBLEM + "q" + i.ToStringLen(2);
	}

	// Own-icon image (white mask tinted).
	static ImageWidget DrawIcon(Widget parent, float x, float y, float size, string name, int color)
	{
		return INT_UIF_Draw.Texture(parent, x, y, size, size, IconPath(name), color);
	}

	static ImageWidget DrawAvatar(Widget parent, float x, float y, float size, int index)
	{
		return INT_UIF_Draw.Texture(parent, x, y, size, size, AvatarPath(index), WHITE);
	}

	static ImageWidget DrawEmblem(Widget parent, float x, float y, float size, int index)
	{
		return INT_UIF_Draw.Texture(parent, x, y, size, size, EmblemPath(index), WHITE);
	}

	// The faction card (vertical, owner 2026-10-23): emblem on top, the faction's name under it.
	static Widget DrawFactionCard(Widget parent, float x, float y, float w, int emblem, string name)
	{
		float h = w * 4.0 / 3.0;
		Widget card = INT_UIF_Draw.Frame(parent, x, y, w, h, "INT_PDA_Card");
		INT_UIF_Surface.Box(card, 0, 0, w, h, 6, PANEL2, LINE2);
		float pad = w * 0.06;
		DrawEmblem(card, pad, pad, w - 2 * pad, emblem);
		float textY = pad + (w - 2 * pad);
		float fs = Math.Max(13, w * 0.11);
		// capitals incl. Cyrillic (ToUpper is ASCII only), cut to the card width
		string capsName = Caps(name);
		string up = Fit(capsName, w - 8, fs);
		INT_UIF_Draw.Text(card, 4, textY, w - 8, h - textY, up, fs, TEXT, INT_UIF_Align.CENTER);
		return card;
	}

	// Anonymous avatar: black plate with «НЕТ ДАННЫХ» (owner brief).
	static Widget DrawAnonAvatar(Widget parent, float x, float y, float size)
	{
		Widget f = INT_UIF_Draw.Frame(parent, x, y, size, size, "INT_PDA_Anon");
		INT_UIF_Surface.Box(f, 0, 0, size, size, 4, 0xFF000000, LINE2);
		// a struck-out eye + «НЕТ ДАННЫХ» sized to fit the square (small avatars show the eye only)
		float icon = size * 0.42;
		string txt = Widget.TranslateString("#STR_INT_PDA_NO_DATA");
		float fs = size * 0.16;
		float tw = TextW(txt, fs);
		if (tw > size - 6 && tw > 0)
			fs = fs * (size - 6) / tw;
		if (fs < 9)
		{
			DrawIcon(f, (size - icon) * 0.5, (size - icon) * 0.5, icon, "eye_off", DIM);
			return f;
		}
		DrawIcon(f, (size - icon) * 0.5, size * 0.16, icon, "eye_off", DIM);
		INT_UIF_Draw.Text(f, 0, size * 0.62, size, fs + 4, txt, fs, DIM2, INT_UIF_Align.CENTER);
		return f;
	}

	// Callsign colour by the relation of the author's faction to mine.
	static int RelationColor(string authorFaction)
	{
		INT_PDA_Profile me = INT_PDA_Client.s_Profile;
		if (authorFaction == "" || !me)
			return NEUTRAL;
		if (me.FactionId != "" && me.FactionId == authorFaction)
			return OWN;
		INT_PDA_FactionView f = INT_PDA_Client.s_Faction;
		if (!f)
			return NEUTRAL;
		int rel = f.RelationTo(authorFaction);
		if (rel == INT_PDA_Const.REL_ALLY)
			return ALLY;
		if (rel == INT_PDA_Const.REL_ENEMY)
			return ENEMY;
		return NEUTRAL;
	}

	static string RoleKey(int role)
	{
		if (role == INT_PDA_Const.ROLE_LEADER)
			return "#STR_INT_PDA_ROLE_LEADER";
		if (role == INT_PDA_Const.ROLE_DEPUTY)
			return "#STR_INT_PDA_ROLE_DEPUTY";
		if (role == INT_PDA_Const.ROLE_MEMBER)
			return "#STR_INT_PDA_ROLE_MEMBER";
		return "#STR_INT_PDA_ROLE_NONE";
	}

	static string ResultKey(int code)
	{
		switch (code)
		{
			case INT_PDA_Const.R_OK: return "#STR_INT_PDA_R_OK";
			case INT_PDA_Const.R_CALLSIGN_FORMAT: return "#STR_INT_PDA_R_CALLSIGN_FORMAT";
			case INT_PDA_Const.R_CALLSIGN_TAKEN: return "#STR_INT_PDA_R_CALLSIGN_TAKEN";
			case INT_PDA_Const.R_PASSWORD_FORMAT: return "#STR_INT_PDA_R_PASSWORD_FORMAT";
			case INT_PDA_Const.R_DISCORD_FORMAT: return "#STR_INT_PDA_R_DISCORD_FORMAT";
			case INT_PDA_Const.R_ALREADY_REGISTERED: return "#STR_INT_PDA_R_ALREADY_REGISTERED";
			case INT_PDA_Const.R_NOT_REGISTERED: return "#STR_INT_PDA_R_NOT_REGISTERED";
			case INT_PDA_Const.R_WRONG_LOGIN: return "#STR_INT_PDA_R_WRONG_LOGIN";
			case INT_PDA_Const.R_LOCKED: return "#STR_INT_PDA_R_LOCKED";
			case INT_PDA_Const.R_NOT_LOGGED_IN: return "#STR_INT_PDA_R_NOT_LOGGED_IN";
			case INT_PDA_Const.R_NO_RIGHTS: return "#STR_INT_PDA_R_NO_RIGHTS";
			case INT_PDA_Const.R_NOT_FOUND: return "#STR_INT_PDA_R_NOT_FOUND";
			case INT_PDA_Const.R_NO_MODULE: return "#STR_INT_PDA_R_NO_MODULE";
			case INT_PDA_Const.R_WRITE_FAILED: return "#STR_INT_PDA_R_WRITE_FAILED";
			case INT_PDA_Const.R_TOO_FAST: return "#STR_INT_PDA_R_TOO_FAST";
			case INT_PDA_Const.R_OFFLINE: return "#STR_INT_PDA_R_OFFLINE";
			case INT_PDA_Const.R_IN_FACTION: return "#STR_INT_PDA_R_IN_FACTION";
			case INT_PDA_Const.R_NOT_IN_FACTION: return "#STR_INT_PDA_R_NOT_IN_FACTION";
			case INT_PDA_Const.R_TEXT: return "#STR_INT_PDA_R_TEXT";
			case INT_PDA_Const.R_FULL: return "#STR_INT_PDA_R_FULL";
			case INT_PDA_Const.R_NAME_TAKEN: return "#STR_INT_PDA_R_NAME_TAKEN";
			case INT_PDA_Const.R_NO_RECIPIENTS: return "#STR_INT_PDA_SOS_NOBODY";
			case INT_PDA_Const.R_WAIT: return "#STR_INT_PDA_R_WAIT";
		}
		return "#STR_INT_PDA_R_BAD_DATA";
	}

	// --- text measuring (the INT_GC_Measure way): a transparent probe TextWidget measured by the engine. The UIF
	// estimate (0.53 em) is far too narrow for capital Cyrillic in the PDA's big tab words.
	protected static TextWidget s_Probe;

	static void AttachProbe(Widget host)
	{
		DetachProbe();
		if (!host)
			return;
		s_Probe = INT_UIF_Draw.Text(host, 0, 0, 4000, 60, "", 16, ARGB(0, 0, 0, 0));
		if (s_Probe)
			s_Probe.SetName("INT_PDA_Probe");
	}

	static void DetachProbe()
	{
		if (s_Probe)
			s_Probe.Unlink();
		s_Probe = null;
	}

	// Width in design px of an already translated text at a UIF text size.
	static float TextW(string value, float size)
	{
		string text = value;
		if (value.IndexOf("#") == 0)
			text = Widget.TranslateString(value);
		float w = INT_UIF_Draw.TextWidth(text, size) * 1.25;
		if (!s_Probe || text == "")
			return w;
		int px = INT_UIF_Scale.TextPx(size);
		s_Probe.SetTextExactSize(px);
		s_Probe.SetText(text);
		s_Probe.Update();
		int mw;
		int mh;
		s_Probe.GetTextSize(mw, mh);
		float f = INT_UIF_Scale.Factor();
		if (mw > 0 && f > 0)
			w = mw / f;
		return w;
	}

	// Multiline text with readable leading. The engine puts the lines of a MultilineTextWidget almost on top of each
	// other (18 px font, 2 lines = 45 px); TextWidget.SetTextSpacing(0, v) adds v px between the lines (measured on
	// 1.30.164200 with the TestKit "spacing" probe; the horizontal value changes nothing on a MultilineTextWidget).
	static MultilineTextWidget MText(Widget parent, float x, float y, float w, float h, string text, float size, int color)
	{
		MultilineTextWidget t = INT_UIF_Draw.MText(parent, x, y, w, h, text, size, color);
		if (t)
		{
			int gap = (int)Math.Round(INT_UIF_Draw.S(size * 0.35));
			t.SetTextSpacing(0, gap);
		}
		return t;
	}

	// The text cut with "…" so that it fits width (measured, not estimated).
	static string Fit(string s, float width, float size)
	{
		if (s == "" || TextW(s, size) <= width)
			return s;
		int n = s.LengthUtf8();
		float full = TextW(s, size);
		int keep = n - 1;
		if (full > 0)
			keep = (int)(n * width / full) - 1;
		if (keep < 1)
			keep = 1;
		string cut = s.SubstringUtf8(0, keep) + "…";
		while (keep > 1 && TextW(cut, size) > width)
		{
			keep = keep - 1;
			cut = s.SubstringUtf8(0, keep) + "…";
		}
		return cut;
	}

	// Translated text in capitals. string.ToUpper is ASCII only, so Cyrillic (incl. Ukrainian/Belarusian letters) and
	// the accented Latin letters of the stringtable languages (de fr es it pl cs hu pt) are replaced letter by letter;
	// results are cached per key. Player / server text (faction names, news titles) goes through CapsText, which
	// does not fill the cache.
	protected static ref map<string, string> s_CapsCache = new map<string, string>();
	protected static ref array<string> s_Low;
	protected static ref array<string> s_Up;

	static string Caps(string key)
	{
		string cached;
		if (s_CapsCache.Find(key, cached))
			return cached;
		string s = CapsText(Widget.TranslateString(key));
		if (s_CapsCache.Count() < 2000)
			s_CapsCache.Set(key, s);
		return s;
	}

	// Capitals of a ready text (no translation, no cache).
	static string CapsText(string text)
	{
		if (!s_Low)
		{
			s_Low = new array<string>();
			s_Up = new array<string>();
			string low = "а б в г д е ё ж з и й к л м н о п р с т у ф х ц ч ш щ ъ ы ь э ю я і ї є ґ ў";
			string up = "А Б В Г Д Е Ё Ж З И Й К Л М Н О П Р С Т У Ф Х Ц Ч Ш Щ Ъ Ы Ь Э Ю Я І Ї Є Ґ Ў";
			low.Split(" ", s_Low);
			up.Split(" ", s_Up);
			// Latin-1: à-þ (no ß), ÿ
			low = "à á â ã ä å æ ç è é ê ë ì í î ï ð ñ ò ó ô õ ö ø ù ú û ü ý þ ÿ";
			up = "À Á Â Ã Ä Å Æ Ç È É Ê Ë Ì Í Î Ï Ð Ñ Ò Ó Ô Õ Ö Ø Ù Ú Û Ü Ý Þ Ÿ";
			AddPairs(low, up);
			// Latin Extended-A (pl cs hu and the rest of Central Europe)
			low = "ā ă ą ć ĉ ċ č ď đ ē ĕ ė ę ě ĝ ğ ġ ģ ĥ ħ ĩ ī ĭ į ĵ ķ ĺ ļ ľ ŀ ł ń ņ ň ō ŏ ő œ ŕ ŗ ř ś ŝ ş š ţ ť ŧ ũ ū ŭ ů ű ų ŵ ŷ ź ż ž";
			up = "Ā Ă Ą Ć Ĉ Ċ Č Ď Đ Ē Ĕ Ė Ę Ě Ĝ Ğ Ġ Ģ Ĥ Ħ Ĩ Ī Ĭ Į Ĵ Ķ Ĺ Ļ Ľ Ŀ Ł Ń Ņ Ň Ō Ŏ Ő Œ Ŕ Ŗ Ř Ś Ŝ Ş Š Ţ Ť Ŧ Ũ Ū Ŭ Ů Ű Ų Ŵ Ŷ Ź Ż Ž";
			AddPairs(low, up);
		}
		string s = text + "";
		s.ToUpper();
		// string.Replace breaks strings longer than 2560 bytes (INT_PDA_Util): a long text keeps its ASCII capitals only
		if (s.Length() > 2048)
			return s;
		int n = s_Low.Count();
		if (s_Up.Count() < n)
			n = s_Up.Count();
		for (int i = 0; i < n; i++)
		{
			if (s.IndexOf(s_Low[i]) >= 0)
				s.Replace(s_Low[i], s_Up[i]);
		}
		return s;
	}

	protected static void AddPairs(string low, string up)
	{
		array<string> l = new array<string>();
		array<string> u = new array<string>();
		low.Split(" ", l);
		up.Split(" ", u);
		if (l.Count() != u.Count())
			return;
		for (int i = 0; i < l.Count(); i++)
		{
			s_Low.Insert(l[i]);
			s_Up.Insert(u[i]);
		}
	}
}

// UI sounds (CfgSoundSets INT_PDA_<Name>_SoundSet, tools/gen_pda_sfx.py). 2D, UI channel, throttled.
class INT_PDA_Sfx
{
	static const float MIN_GAP = 0.04;
	protected static float s_LastAt = -100;
	static int s_Played;		// self-test counter

	// name = Click, Tab, Key, Toggle, Boot, Off, LoginOk, Error, Msg, Pm, News, Sos, Invite, LowBat, MapPing, Send
	static void Play(string name)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		if (INT_PDA_Client.s_Prefs && INT_PDA_Client.s_Prefs.Sounds == 0)
			return;
		float now = GetGame().GetTickTime();
		if (now - s_LastAt < MIN_GAP)
			return;
		SoundParams params = new SoundParams("INT_PDA_" + name + "_SoundSet");
		if (!params.IsValid())
			return;
		SoundObjectBuilder builder = new SoundObjectBuilder(params);
		SoundObject so = builder.BuildSoundObject();
		if (!so)
			return;
		so.SetKind(WaveKind.WAVEUI);
		AbstractSoundScene scene = GetGame().GetSoundScene();
		if (!scene)
			return;
		AbstractWave wave = scene.Play2D(so, builder);
		if (wave)
		{
			s_LastAt = now;
			s_Played++;
		}
	}
}
