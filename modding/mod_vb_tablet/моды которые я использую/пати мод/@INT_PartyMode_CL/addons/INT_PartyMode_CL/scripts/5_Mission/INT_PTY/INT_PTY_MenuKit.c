// INT PartyMode (client) — kit of the main window (C «ЭКСПЕДИЦИЯ»): palette of the concept, clickable areas,
// event forwarding, geometry / time helpers, block entrances, drawing helpers. All sizes are design px of the
// 1920x1080 canvas (design/concepts_v1/src/c_expedition.css).

// Palette of c_expedition.css (:root).
class INT_PTY_MC
{
	static const int O = 0xFFFF7A2F;			// --o burnt orange
	static const int O_LIGHT = 0xFFFF9A5C;		// orange button under the mouse
	static const int O_PRESS = 0xFFFFD2B8;		// button press flash
	static const int O_SOFT = 0x24FF7A2F;		// --o2 14 %
	static const int T = 0xFFF3EFE8;			// --t text
	static const int M = 0xFF948F88;			// --m muted
	static const int D = 0xFF5D5953;			// --d dark
	static const int L = 0x1CFFFFFF;			// --l hairline 11 %
	static const int L2 = 0x38FFFFFF;			// --l2 22 %
	static const int L_ROW = 0x0FFFFFFF;		// row separators 6 %
	static const int G = 0xFF56E07E;			// --g search green
	static const int R = 0xFFFF4B3E;			// --r red
	static const int R_TEXT = 0xFFFF6D61;		// red text
	static const int W = 0xFFFFC246;			// --w warning
	static const int BG = 0xFF0B0B0B;			// --bg
	static const int BODY = 0xFFCFC9C1;
	static const int LABEL = 0xFFD9D4CC;
	static const int SOFT = 0xFFBDB7AF;
	static const int INK = 0xFF1A0C04;			// text on orange
	static const int INK_RED = 0xFF1C0503;		// text on red
	static const int CARD = 0xFF161514;			// tooltip card
	static const int TOAST = 0xFF151413;
	static const int BAND = 0xFF0D0C0C;			// confirm band
	static const int OFF = 0xFF6C6862;			// grey dot
	static const int KNOB_OFF = 0xFF77726B;
	static const int KEY_EDGE = 0x6BFFFFFF;		// keycap border 42 %
	static const int BTN_EDGE = 0x4DFFFFFF;		// plain button border 30 %
	static const int DG_EDGE = 0xA6FF4B3E;		// danger outline 65 %
}

// A clickable area of the window: what it does, what lights up under the mouse, its tooltip.
class INT_PTY_MenuAct
{
	string Key;
	int Section;
	TextWidget HoverText;
	int TextNormal;
	int TextHover;
	ImageWidget HoverImg;
	int ImgNormal;
	int ImgHover;
	Widget HoverBg;
	float HoverBgAlpha = 1;
	INT_PTY_MButton Btn;
	string TipTitle;
	string TipText;
	float TipW = 340;
	bool Sound = true;
	bool Off;
}

// Widget events -> the window (UIScriptedMenu is not a widget handler itself).
class INT_PTY_MenuHandler : ScriptedWidgetEventHandler
{
	protected INT_PTY_Menu m_Menu;

	void SetMenu(INT_PTY_Menu menu)
	{
		m_Menu = menu;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!m_Menu || button != MouseState.LEFT)
			return false;
		return m_Menu.OnActClick(w);
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (!m_Menu)
			return false;
		return m_Menu.OnActHover(w, true);
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (!m_Menu)
			return false;
		return m_Menu.OnActHover(w, false);
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (!m_Menu)
			return false;
		return m_Menu.OnActWheel(w, wheel);
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (!m_Menu || button != MouseState.LEFT)
			return false;
		return m_Menu.OnActDown(w);
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (!m_Menu)
			return false;
		return m_Menu.OnEditChange(w, finished);
	}

	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if (!m_Menu)
			return false;
		return m_Menu.OnEditKey(w, key);
	}

	// W-7: Esc may arrive as a key-down only (no printable press); the field handles it once (BlurEdit is idempotent).
	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (!m_Menu || key != KeyCode.KC_ESCAPE)
			return false;
		return m_Menu.OnEditKey(w, key);
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (m_Menu)
			m_Menu.OnEditFocus(w, true);
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (m_Menu)
			m_Menu.OnEditFocus(w, false);
		return false;
	}
}

// A page of the window (one tab). The window owns it, builds it into its own frame and forwards events.
class INT_PTY_Page : Managed
{
	protected INT_PTY_Menu m_Menu;
	protected Widget m_P;

	void Attach(INT_PTY_Menu menu, Widget frame)
	{
		m_Menu = menu;
		m_P = frame;
	}

	Widget GetFrame()
	{
		return m_P;
	}

	void Build(bool animate)
	{
	}

	bool OnClick(string key, Widget w)
	{
		return false;
	}

	bool OnWheel(string key, int wheel)
	{
		return false;
	}

	// A press on a slider: true = the window starts dragging `key`.
	bool OnDown(string key)
	{
		return false;
	}

	// While a slider is dragged: the mouse x (screen px) every frame, done = released.
	void OnSlide(string key, int mx, bool done)
	{
	}

	void OnData(int what)
	{
	}

	void Tick(float dt)
	{
	}

	void OnEditDone(string text)
	{
	}

	void OnEditLive(string text)
	{
	}

	void OnLeave()
	{
	}

	// Pixel-smooth lists (INT_PTY_MScroll): build the widgets of pooled row `slot` once, bind item `index` to it,
	// the list moved (connectors, route lines), the cursor entered / left a row.
	void ScrollBuild(int sid, int slot, Widget root)
	{
	}

	void ScrollFill(int sid, int slot, int index)
	{
	}

	void ScrollMoved(int sid)
	{
	}

	void ScrollHover(int sid, int slot, bool on)
	{
	}

	// Test seam: the page's main list jumps to this pixel offset.
	void TestScroll(float px)
	{
	}

	string TestInfo()
	{
		return "";
	}
}

// Geometry, colours and time texts of the window (client only).
class INT_PTY_MenuGeo
{
	// The 8 colour swatches of the concept + white (settings: 9).
	static int Swatch(int i)
	{
		if (i == 0)
			return 0xFFFF5A4F;
		if (i == 1)
			return 0xFFFFB02E;
		if (i == 2)
			return 0xFFFFE066;
		if (i == 3)
			return 0xFF7BE36B;
		if (i == 4)
			return 0xFF3FD0FF;
		if (i == 5)
			return 0xFF5C7CFF;
		if (i == 6)
			return 0xFFB48CFF;
		if (i == 7)
			return 0xFFFF7AD9;
		return 0xFFFFFFFF;
	}

	static int SwatchIndex(int color)
	{
		for (int i = 0; i < 9; i++)
		{
			if (Swatch(i) == color)
				return i;
		}
		return -1;
	}

	// Automatic colour of a friend: stable per Steam64 (the same friend keeps it across sessions).
	static int AutoColor(string id)
	{
		return INT_PTY_UI.AutoColor(id);
	}

	static int FriendColor(string id)
	{
		return INT_PTY_UI.FriendColor(id);
	}

	// How I see this friend's pings / his colour: one rule for the window, the HUD and the maps (INT_PTY_UI).
	static int PingShapeOf(string id)
	{
		return INT_PTY_UI.PingShapeOf(id, false);
	}

	static int PingColorOf(string id)
	{
		return INT_PTY_UI.PingColorOf(id, false);
	}

	static string ShapeName(int shape)
	{
		return "pty_ping_" + ShapeKey(shape) + "_ca";
	}

	static string ShapeOutline(int shape)
	{
		return "pty_ping_" + ShapeKey(shape) + "_out_ca";
	}

	static string ShapeKey(int shape)
	{
		if (shape == INT_PTY_Shape.CHECK)
			return "check";
		if (shape == INT_PTY_Shape.CIRCLE)
			return "circle";
		if (shape == INT_PTY_Shape.DIAMOND)
			return "diamond";
		if (shape == INT_PTY_Shape.TRIANGLE)
			return "triangle";
		if (shape == INT_PTY_Shape.EXCL)
			return "excl";
		return "cross";
	}

	static string ShapeLabel(int shape)
	{
		return INT_PTY_Text.Tr("#STR_INT_PTY_W_SH_" + shape.ToString());
	}

	// My position (the live player, else the camera) — false before the player exists.
	static bool MyPos(out vector pos)
	{
		pos = vector.Zero;
		if (!GetGame())
			return false;
		Man me = GetGame().GetPlayer();
		if (!me)
			return false;
		pos = me.GetPosition();
		return true;
	}

	// Compass bearing from `a` to `b`: 0 = north (+Z), 90 = east (+X).
	static float Bearing(vector a, vector b)
	{
		float dx = b[0] - a[0];
		float dz = b[2] - a[2];
		float deg = Math.Atan2(dx, dz) * Math.RAD2DEG;
		if (deg < 0)
			deg = deg + 360;
		return deg;
	}

	static float Flat(vector a, vector b)
	{
		float dx = b[0] - a[0];
		float dz = b[2] - a[2];
		return Math.Sqrt(dx * dx + dz * dz);
	}

	// «С», «СВ» … of a bearing.
	static string CompassOf(float deg)
	{
		int idx = (int)Math.Round(deg / 45.0);
		idx = idx % 8;
		return INT_PTY_Text.Tr("#STR_INT_PTY_W_DIR_" + idx.ToString());
	}

	// Distance split for big numbers: "124" + "м", "1,2" + "км".
	static void DistParts(float m, out string num, out string unit)
	{
		if (m < 999.5)
		{
			int whole = (int)Math.Round(m);
			num = whole.ToString();
			unit = INT_PTY_Text.Tr("#STR_INT_PTY_UNIT_M");
			return;
		}
		int tenths = (int)Math.Round(m / 100.0);
		int km = tenths / 10;
		int frac = tenths - km * 10;
		num = km.ToString() + "," + frac.ToString();
		unit = INT_PTY_Text.Tr("#STR_INT_PTY_UNIT_KM");
	}

	// Duration as words: «2 мин», «1 ч 4 мин», «3 д».
	static string Span(float secs)
	{
		int s = (int)secs;
		if (s < 0)
			s = 0;
		int mins = s / 60;
		if (mins < 1)
			mins = 1;
		if (mins < 60)
			return INT_PTY_Text.Fmt("#STR_INT_PTY_W_T_MIN", mins.ToString());
		int hours = mins / 60;
		int rest = mins - hours * 60;
		if (hours < 24)
		{
			if (rest == 0)
				return INT_PTY_Text.Fmt("#STR_INT_PTY_W_T_HOUR", hours.ToString());
			string hs = hours.ToString();
			string rs = rest.ToString();
			return INT_PTY_Text.Fmt("#STR_INT_PTY_W_T_HOUR_MIN", hs, rs);
		}
		int days = hours / 24;
		return INT_PTY_Text.Fmt("#STR_INT_PTY_W_T_DAY", days.ToString());
	}

	// «только что» / «4 мин назад».
	static string Ago(float secs)
	{
		if (secs < 60)
			return INT_PTY_Text.Tr("#STR_INT_PTY_W_T_NOW");
		return INT_PTY_Text.Fmt("#STR_INT_PTY_W_T_AGO", Span(secs));
	}

	// "2026-10-23 14:05" -> "23.10.2026".
	static string DateOf(string since)
	{
		if (since.Length() < 10)
			return since;
		string y = since.Substring(0, 4);
		string mo = since.Substring(5, 2);
		string d = since.Substring(8, 2);
		return d + "." + mo + "." + y;
	}

	// Bound key of an input as shown on a keycap (falls back to the default key).
	static string KeyName(string input, string fallback)
	{
		UAInput inp = GetUApi().GetInputByName(input);
		if (!inp)
			return fallback;
		string k = InputUtils.GetButtonNameFromInput(input, EInputDeviceType.MOUSE_AND_KEYBOARD);
		if (k == "")
			return fallback;
		return INT_PTY_Text.Upper(k);
	}
}

// What the window knows about friends while it is closed: since when a friend is dead / offline (the protocol
// carries no times). Fed by INT_PTY_Client.OnChanged from the first window or notification call on.
class INT_PTY_MenuWatch
{
	protected static bool s_On;
	protected static ref map<string, float> s_DeadAt;
	protected static ref map<string, float> s_OffAt;
	protected static ref array<string> s_SeenOn;

	static void Ensure()
	{
		if (s_On)
			return;
		s_On = true;
		s_DeadAt = new map<string, float>();
		s_OffAt = new map<string, float>();
		s_SeenOn = new array<string>();
		INT_PTY_Client.OnChanged().Insert(INT_PTY_MenuWatch.OnChanged);
		Scan();
	}

	// W-9: end of the game session (INT_PTY_Hud.Shutdown from OnMissionFinish): «seen / died N min ago» and the
	// search-list request time do not survive a reconnect. Ensure() starts again on the next session.
	static void Reset()
	{
		if (s_On)
			INT_PTY_Client.OnChanged().Remove(INT_PTY_MenuWatch.OnChanged);
		s_On = false;
		s_DeadAt = null;
		s_OffAt = null;
		s_SeenOn = null;
		INT_PTY_PageSearch.ResetSession();
	}

	static void OnChanged(int what)
	{
		if ((what & (INT_PTY_What.STATE | INT_PTY_What.FRIENDS)) != 0)
			Scan();
	}

	static void Scan()
	{
		if (!INT_PTY_Client.s_Friends || !s_DeadAt)
			return;
		float now = INT_PTY_Client.Clock();
		foreach (INT_PTY_Friend f : INT_PTY_Client.s_Friends)
		{
			if (!f)
				continue;
			if (f.Dead)
			{
				if (!s_DeadAt.Contains(f.Id))
					s_DeadAt.Set(f.Id, now);
			}
			else
			{
				s_DeadAt.Remove(f.Id);
			}
			if (f.Online)
			{
				if (s_SeenOn.Find(f.Id) < 0)
					s_SeenOn.Insert(f.Id);
				s_OffAt.Remove(f.Id);
			}
			else if (s_SeenOn.Find(f.Id) >= 0 && !s_OffAt.Contains(f.Id))
			{
				s_OffAt.Set(f.Id, now);
			}
		}
	}

	// Seconds since the friend died / went offline, -1 = unknown.
	static float DeadFor(string id)
	{
		float at;
		if (!s_DeadAt || !s_DeadAt.Find(id, at))
			return -1;
		return INT_PTY_Client.Clock() - at;
	}

	static float OffFor(string id)
	{
		float at;
		if (!s_OffAt || !s_OffAt.Find(id, at))
			return -1;
		return INT_PTY_Client.Clock() - at;
	}

	// Test seam: pretend the friend died / left `secs` seconds ago.
	static void TestSet(string id, float deadSecs, float offSecs)
	{
		Ensure();
		float now = INT_PTY_Client.Clock();
		if (deadSecs >= 0)
			s_DeadAt.Set(id, now - deadSecs);
		if (offSecs >= 0)
			s_OffAt.Set(id, now - offSecs);
	}
}

// Block entrances / exits. A frame's alpha does not reach its children (measured 2026-10-22): a fade walks the
// whole subtree and fades every widget to the alpha it was built with.
class INT_PTY_MenuFx
{
	static const string NO_FADE = "PtyNoFade";

	static void FadeIn(Widget root, float delay, float dur)
	{
		if (!root)
			return;
		FadeInOne(root, delay, dur);
		Widget c = root.GetChildren();
		while (c)
		{
			FadeIn(c, delay, dur);
			c = c.GetSibling();
		}
	}

	// Home alpha per widget: a block fading inside another block (a row inside its column) is faded twice, and the
	// second fade must not read the 0 the first one has just set.
	protected static ref map<Widget, float> s_Home;

	static void ForgetHomes()
	{
		if (s_Home)
			s_Home.Clear();
	}

	protected static void FadeInOne(Widget w, float delay, float dur)
	{
		if (w.GetName() == NO_FADE)
			return;
		if (!s_Home)
			s_Home = new map<Widget, float>();
		if (s_Home.Count() > 6000)
			s_Home.Clear();
		float home;
		if (!s_Home.Find(w, home))
		{
			home = w.GetAlpha();
			s_Home.Set(w, home);
		}
		INT_UIF_Animator.Get().Cancel(w, INT_UIF_TweenProp.ALPHA);
		if (home <= 0.004)
			return;
		w.SetAlpha(0);
		INT_UIF_Tween t = INT_UIF_Animator.Fade(w, home, dur, INT_UIF_Ease.OUT_CUBIC);
		if (t)
			t.From(0).Delay(delay);
	}

	static void FadeOut(Widget root, float delay, float dur)
	{
		if (!root)
			return;
		INT_UIF_Animator.Get().Cancel(root, INT_UIF_TweenProp.ALPHA);
		INT_UIF_Tween t = INT_UIF_Animator.Fade(root, 0, dur, INT_UIF_Ease.IN_QUAD);
		if (t)
			t.Delay(delay);
		Widget c = root.GetChildren();
		while (c)
		{
			FadeOut(c, delay, dur);
			c = c.GetSibling();
		}
	}

	// Multiplies the alpha of every widget of a block (dimmed rows): k = 1 restores `home`.
	static void Dim(Widget root, float k)
	{
		if (!root)
			return;
		root.SetAlpha(k);
		Widget c = root.GetChildren();
		while (c)
		{
			Dim(c, k);
			c = c.GetSibling();
		}
	}

	// Slides a block from (dx, dy) design px back to its place and fades every widget in.
	static void BlockIn(Widget w, float dx, float dy, float delay, float dur)
	{
		if (!w)
			return;
		float x;
		float y;
		w.GetPos(x, y);
		INT_UIF_Animator.Get().Cancel(w, INT_UIF_TweenProp.POS_X);
		INT_UIF_Animator.Get().Cancel(w, INT_UIF_TweenProp.POS_Y);
		float sdx = INT_UIF_Draw.S(dx);
		float sdy = INT_UIF_Draw.S(dy);
		w.SetPos(x + sdx, y + sdy);
		if (dx != 0)
		{
			INT_UIF_Tween mx = INT_UIF_Animator.MoveX(w, x, dur, INT_UIF_Ease.OUT_CUBIC);
			if (mx)
				mx.Delay(delay);
		}
		if (dy != 0)
		{
			INT_UIF_Tween my = INT_UIF_Animator.MoveY(w, y, dur, INT_UIF_Ease.OUT_CUBIC);
			if (my)
				my.Delay(delay);
		}
		FadeIn(w, delay, dur);
	}

	static void GrowW(Widget w, float delay, float dur)
	{
		if (!w)
			return;
		float sw;
		float sh;
		w.GetSize(sw, sh);
		w.SetSize(0, sh);
		INT_UIF_Tween t = INT_UIF_Animator.ResizeW(w, sw, dur, INT_UIF_Ease.OUT_EXPO);
		if (t)
			t.Delay(delay);
	}

	static void GrowH(Widget w, float delay, float dur)
	{
		if (!w)
			return;
		float sw;
		float sh;
		w.GetSize(sw, sh);
		w.SetSize(sw, 0);
		INT_UIF_Tween t = INT_UIF_Animator.ResizeH(w, sh, dur, INT_UIF_Ease.IN_OUT_CUBIC);
		if (t)
			t.Delay(delay);
	}

	// A number that changed: the new value rises into place (odometer feel).
	static void Bump(Widget w, float dy)
	{
		if (!w)
			return;
		INT_PTY_Fx.InY(w, dy, 0, 0.26, INT_UIF_Ease.OUT_CUBIC);
	}
}

// Drawing helpers of the window on INT_UIF_Draw / INT_PTY_Look.
class INT_PTY_MD
{
	static TextWidget Txt(Widget p, float x, float y, float w, float h, string text, float size, int color, int align = INT_UIF_Align.LEFT, bool bold = false, bool big = false)
	{
		string t = INT_PTY_Text.Tr(text);
		return INT_PTY_Look.Text(p, x, y, w, h, t, size, color, align, bold, big);
	}

	// Tracked capitals (letter-spacing of the concept).
	static TextWidget Caps(Widget p, float x, float y, float w, float h, string text, float size, int color, int align = INT_UIF_Align.LEFT, bool bold = true)
	{
		return INT_PTY_Look.Caps(p, x, y, w, h, text, size, color, align, bold);
	}

	static string CapsOf(string text)
	{
		return INT_PTY_Text.Caps(text);
	}

	// Rendered width in design px, estimated per character. Calibrated on stand screenshots 2026-10-23 (1080p, the
	// SDF font of INT_UIF_Draw.Text): capitals 0.65 em, lowercase 0.565 em, digits 0.58 em, space 0.30 em, narrow
	// punctuation 0.30 em; bold (SetBold widens the advance) x1.05. TextWidget.GetTextSize was NOT usable: the same
	// strings came back ~9 % wider in another boot. Callers give text boxes spare room anyway (Room()).
	static const string LOWER_CHARS = "абвгдеёжзийклмнопрстуфхцчшщъыьэюяabcdefghjkmnopqrsuvwxyz";
	static const string NARROW_CHARS = " .,:!|ilfrt()[]/-";
	static const string DIGIT_CHARS = "0123456789";
	static const string WIDE_LOWER = "дждшщюымфw";

	static float Measure(Widget p, string text, float size, bool bold = false, bool big = false)
	{
		string t = INT_PTY_Text.Tr(text);
		int n = t.LengthUtf8();
		float em = 0;
		for (int i = 0; i < n; i++)
		{
			string ch = t.SubstringUtf8(i, 1);
			em = em + CharEm(ch);
		}
		float w = em * size * GlyphRound(size);
		if (bold)
			w = w * 1.05;
		return w;
	}

	// On 4:3 / 5:4 screens the glyph size is rounded to whole pixels (INT_UIF_Scale.TextPx), so small text comes out
	// a few per cent wider than its design size (measured at 1280x960: 13 px captions ~2-3 % wider). 1 on 16:9.
	static float GlyphRound(float size)
	{
		int sw = INT_UIF_Scale.ScreenW();
		int sh = INT_UIF_Scale.ScreenH();
		if (sh <= 0 || sw <= 0 || size <= 0)
			return 1;
		float k = INT_UIF_Scale.Factor() * 1080.0 / sh;
		float want = size * k;
		if (want <= 0)
			return 1;
		int px = INT_UIF_Scale.TextPx(size);
		float pxf = px;
		float r = pxf / want;
		if (r < 1)
			r = 1;
		return r;
	}

	static float CharEm(string ch)
	{
		if (ch == " ")
			return 0.30;
		// Latin m / t / f measured on the stand 2026-10-23 (German, Hungarian UI): m 0.87 em, t and f 0.38 em — the
		// generic lowercase / narrow values put the arrow and the (i) icon onto «124 m», «Dimmen beim Zielen»
		if (ch == "m")
			return 0.87;
		if (ch == "t" || ch == "f")
			return 0.38;
		if (DIGIT_CHARS.IndexOf(ch) >= 0)
			return 0.58;
		if (WIDE_LOWER.IndexOf(ch) >= 0)
			return 0.68;
		if (LOWER_CHARS.IndexOf(ch) >= 0)
			return 0.575;
		if (NARROW_CHARS.IndexOf(ch) >= 0)
			return 0.30;
		if (ch == "—" || ch == "Ш" || ch == "Щ" || ch == "Ж" || ch == "Ю" || ch == "М" || ch == "M" || ch == "W")
			return 0.78;
		if (INT_PTY_Text.IsWide(ch))
			return 1.0;
		return 0.65;
	}

	// The engine's own number (calibration seam).
	static float MeasureRaw(Widget p, string text, float size, bool bold, bool big)
	{
		string t = INT_PTY_Text.Tr(text);
		TextWidget probe = INT_UIF_Draw.Text(p, 0, 0, 1800, size + 10, t, size, 0, INT_UIF_Align.LEFT, big);
		if (!probe)
			return -1;
		if (bold)
			probe.SetBold(true);
		probe.Update();
		int mw;
		int mh;
		probe.GetTextSize(mw, mh);
		probe.Unlink();
		float f = INT_UIF_Scale.Factor();
		if (f <= 0)
			f = 1;
		return mw / f;
	}

	// Text box width for a measured text: glyphs past a TextWidget's box are cut (the footer signature lost its
	// first letters), so boxes always get spare room; positions still use the measured width.
	static float Room(float measured)
	{
		return measured + 48;
	}

	static float CapsW(Widget p, string text, float size, bool bold = true)
	{
		return Measure(p, INT_PTY_Text.Caps(text), size, bold);
	}

	static ImageWidget Line(Widget p, float x, float y, float w, float h, int color)
	{
		return INT_PTY_Look.Fill(p, x, y, w, h, color);
	}

	static INT_UIF_Surface Box(Widget p, float x, float y, float w, float h, int fill, int border)
	{
		return INT_UIF_Surface.Box(p, x, y, w, h, 4, fill, border);
	}

	static ImageWidget Dot(Widget p, float cx, float cy, float size, int color)
	{
		return INT_UIF_Draw.Image(p, cx - size * 0.5, cy - size * 0.5, size, size, "iuf_circle16", color);
	}

	static ImageWidget Glow(Widget p, float cx, float cy, float size, int color)
	{
		return INT_UIF_Draw.Image(p, cx - size * 0.5, cy - size * 0.5, size, size, "iuf_radial", color);
	}

	static ImageWidget Ico(Widget p, float x, float y, float size, string icon, int color)
	{
		return INT_PTY_Look.IconAt(p, x, y, size, icon, color);
	}

	// Filled diamond of side `size` centred on (cx, cy).
	static ImageWidget Dm(Widget p, float cx, float cy, float size, int color)
	{
		return INT_PTY_Look.Diamond(p, cx, cy, size, "pty_diamond_ca", color);
	}

	// Outlined diamond (hollow node).
	static ImageWidget DmLine(Widget p, float cx, float cy, float size, int color)
	{
		return INT_PTY_Look.Diamond(p, cx, cy, size, "pty_diamond_line_ca", color);
	}

	// Ping icon (shape) with its dark outline twin under it, centred on (cx, cy).
	static ImageWidget Ping(Widget p, float cx, float cy, float size, int shape, int color, bool outline)
	{
		int dark = ARGB(209, 6, 8, 10);
		string outTex = INT_PTY_MenuGeo.ShapeOutline(shape);
		string tex = INT_PTY_MenuGeo.ShapeName(shape);
		if (outline)
			INT_PTY_Look.Tex(p, cx - size * 0.5, cy - size * 0.5, size, size, outTex, dark);
		return INT_PTY_Look.Tex(p, cx - size * 0.5, cy - size * 0.5, size, size, tex, color);
	}

	// Keycap of the concept: 1 px border 42 %, bold 13 px text; returns its width.
	static float Keycap(Widget p, float x, float y, string key, float h = 26, int edge = 0, int textColor = 0)
	{
		if (edge == 0)
			edge = INT_PTY_MC.KEY_EDGE;
		if (textColor == 0)
			textColor = INT_PTY_MC.T;
		float kw = Measure(p, key, 13, true) + 18;
		if (kw < 30)
			kw = 30;
		Box(p, x, y, kw, h, 0, edge);
		Txt(p, x, y, kw, h, key, 13, textColor, INT_UIF_Align.CENTER, true);
		return kw;
	}

	// Wraps `text` into lines that fit `room` (design px) at `size`.
	static void Wrap(string text, float size, float room, array<string> lines, int maxLines = 6)
	{
		lines.Clear();
		string rest = INT_PTY_Text.Tr(text);
		int guard = 0;
		while (rest != "" && guard < maxLines)
		{
			string tail;
			string line = INT_PTY_Look.FirstLine(rest, size, false, room, tail);
			lines.Insert(line);
			rest = tail;
			guard++;
		}
	}

	// A paragraph as one TextWidget per line (lines `step` apart); returns the height used.
	static float Para(Widget p, float x, float y, float w, string text, float size, int color, float step, int maxLines = 6)
	{
		array<string> lines = new array<string>();
		Wrap(text, size, w, lines, maxLines);
		float yy = y;
		foreach (string l : lines)
		{
			Txt(p, x, yy, w + 40, step, l, size, color);
			yy = yy + step;
		}
		return yy - y;
	}
}
