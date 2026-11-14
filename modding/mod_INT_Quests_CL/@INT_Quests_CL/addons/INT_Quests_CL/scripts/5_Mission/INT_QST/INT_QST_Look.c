// INT Quests (client) — look of design B «ПОЛЕВОЙ ДНЕВНИК» (owner pick 2026-10-22, design/concepts_v1/2_b_diary_*):
// a leather field notebook with stitched edges, paper pages, ink text, blue-pen speech, rubber stamps, a yellow
// highlighter and red pushpins. Textures are light masks tinted by SetColor (tools/gen_qst_art.py). One font
// (Metron): titles get a faux-bold twin (a shifted copy), as in the other INT mods.
// Colours are constants: a call with two call-result arguments gets one clobbered (BBA lesson), hoist to locals.

class INT_QST_Look
{
	static const string UI = "INT_Quests_CL/data/ui/";
	protected static ref map<string, string> s_CapsMap;	// Cyrillic lower -> upper (Caps)

	// --- palette (b_diary.css) -------------------------------------------------------------------------------------
	static int Leather()		{ return 0xFF35281D; }
	static int LeatherDark()	{ return 0xFF22190F; }
	static int LeatherLine()	{ return 0xFF4E3B29; }
	static int Stitch()		{ return 0x75E4C696; }
	static int Cream()		{ return 0xFFF0E3C4; }
	static int Tan()		{ return 0xFFC8AB7E; }
	static int Tan2()		{ return 0xFFA08A67; }
	static int Paper()		{ return 0xFFD9C9A3; }
	static int PaperDark()		{ return 0xFFC9B78F; }
	static int Note()		{ return 0xFFEBE0C3; }
	static int Sticky()		{ return 0xFFE9D68C; }
	static int Kraft()		{ return 0xFFB59D70; }
	static int Ink()		{ return 0xFF2A2218; }
	static int Ink2()		{ return 0xFF4A3D2C; }
	static int Ink3()		{ return 0xFF66553F; }
	static int Blue()		{ return 0xFF1F3A6E; }
	static int Red()		{ return 0xFFA3281E; }
	static int RedDeep()		{ return 0xFF6E2219; }
	static int Olive()		{ return 0xFF46622A; }
	static int Marker()		{ return 0x8FF7CC40; }
	static int Rule()		{ return 0x40665540; }	// ruled line on paper
	static int Shade()		{ return 0x26000000; }

	static int InkA(int a)
	{
		return ARGB(a, 42, 34, 24);
	}

	static int CreamA(int a)
	{
		return ARGB(a, 240, 227, 196);
	}

	static int RedA(int a)
	{
		return ARGB(a, 163, 40, 30);
	}

	// --- text -----------------------------------------------------------------------------------------------------
	static string Tr(string key)
	{
		if (key.Length() > 0 && key.Get(0) == "#")
			return Widget.TranslateString(key);
		return key;
	}

	static string Upper(string key)
	{
		string s = INT_QST_Look.Tr(key);
		s.ToUpper();
		return s;
	}

	// Capitals for titles written by admins / translated texts: string.ToUpper only changes ASCII letters, the
	// Cyrillic ones are mapped here (short texts only: one SubstringUtf8 per character).
	static string Caps(string text)
	{
		string r = text + "";
		r.ToUpper();
		int n = r.LengthUtf8();
		if (r.Length() == n || n > 400)
			return r;
		if (!s_CapsMap)
		{
			s_CapsMap = new map<string, string>();
			string lo = "абвгдеёжзийклмнопрстуфхцчшщъыьэюяіїєґў";
			string up = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯІЇЄҐЎ";
			int m = lo.LengthUtf8();
			for (int k = 0; k < m; k++)
			{
				string cl = lo.SubstringUtf8(k, 1);
				string cu = up.SubstringUtf8(k, 1);
				s_CapsMap.Set(cl, cu);
			}
		}
		string o = "";
		string c;
		string u;
		for (int i = 0; i < n; i++)
		{
			c = r.SubstringUtf8(i, 1);
			if (s_CapsMap.Find(c, u))
				o = o + u;
			else
				o = o + c;
		}
		return o;
	}

	static string Fmt(string key, string a1, string a2 = "", string a3 = "")
	{
		string s = INT_QST_Look.Tr(key);
		s.Replace("%1", a1);
		s.Replace("%2", a2);
		s.Replace("%3", a3);
		return s;
	}

	static string Dur(int seconds)
	{
		string d = INT_QST_Look.Tr("#STR_INT_QST_U_DAY");
		string h = INT_QST_Look.Tr("#STR_INT_QST_U_HOUR");
		string m = INT_QST_Look.Tr("#STR_INT_QST_U_MIN");
		string s = INT_QST_Look.Tr("#STR_INT_QST_U_SEC");
		return INT_QST_Util.DurationShort(seconds, d, h, m, s);
	}

	static string Money(int v)
	{
		return INT_QST_Util.Money(v) + " " + INT_QST_Look.Tr("#STR_INT_QST_RUB");
	}

	static string Meters(float m)
	{
		if (m >= 1000)
		{
			float km = Math.Round(m / 100.0) / 10.0;
			string t = km.ToString();
			t.Replace(".", ",");
			return t + " " + INT_QST_Look.Tr("#STR_INT_QST_U_KM");
		}
		int mi = (int)Math.Round(m);
		return mi.ToString() + " " + INT_QST_Look.Tr("#STR_INT_QST_U_M");
	}

	static TextWidget Text(Widget p, float x, float y, float w, float h, string text, float size, int color, int align = 0)
	{
		return INT_UIF_Draw.Text(p, x, y, w, h, text, size, color, align, false);
	}

	// Cuts text to fit `width` design px at glyph `size` (Metron estimate) with "..." at the end (the font may lack the ellipsis glyph).
	static string Fit(string text, float width, float size)
	{
		int n = text.LengthUtf8();
		int maxChars = (int)(width / (size * 0.56));
		if (n <= maxChars || maxChars < 4)
			return text;
		return text.SubstringUtf8(0, maxChars - 2) + "...";
	}

	// Faux bold: a twin shifted right by ~4 % of the glyph size behind the real text (Metron has no bold face).
	static TextWidget Heavy(Widget p, float x, float y, float w, float h, string text, float size, int color, int align = 0)
	{
		float dx = size * 0.045;
		if (dx < 0.6)
			dx = 0.6;
		INT_UIF_Draw.Text(p, x + dx, y, w, h, text, size, color, align, false);
		return INT_UIF_Draw.Text(p, x, y, w, h, text, size, color, align, false);
	}

	// Spaced caps caption ("АКТИВНЫЕ ЗАДАНИЯ"): letters get a thin space (Metron has no tracking setting).
	static string Spaced(string key)
	{
		string s = INT_QST_Look.Upper(key);
		string r = "";
		int n = s.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string c = s.SubstringUtf8(i, 1);
			r = r + c;
			if (i < n - 1 && c != " ")
				r = r + " ";
			else if (c == " ")
				r = r + "  ";
		}
		return r;
	}

	static void Caption(Widget p, float x, float y, float w, string key, int color, string icon = "")
	{
		float tx = x;
		if (icon != "")
		{
			INT_QST_Look.Icon(p, x, y + 1, 13, icon, color);
			tx = x + 20;
		}
		string cap = INT_QST_Look.Spaced(key);
		INT_QST_Look.Heavy(p, tx, y - 2, w, 18, cap, 11, color);
	}

	// --- textures -------------------------------------------------------------------------------------------------
	static ImageWidget Tex(Widget p, float x, float y, float w, float h, string name, int color)
	{
		string path = UI + name + ".paa";
		return INT_UIF_Draw.Texture(p, x, y, w, h, path, color);
	}

	// Own icon mask (int_qst_ic_<name>_ca) tinted.
	static ImageWidget Icon(Widget p, float x, float y, float size, string name, int color)
	{
		string path = UI + "int_qst_ic_" + name + "_ca.paa";
		return INT_UIF_Draw.Texture(p, x, y, size, size, path, color);
	}

	// Full-colour diamond sign: avail / turnin / locked / progress / lock / cooldown.
	static ImageWidget Sign(Widget p, float x, float y, float size, string kind)
	{
		string path = UI + "int_qst_sign_" + kind + "_ca.paa";
		return INT_UIF_Draw.Texture(p, x, y, size, size, path, 0xFFFFFFFF);
	}

	// Paper sheet: tinted paper texture over a flat fill (the fill shows while the texture streams in).
	static Widget PaperSheet(Widget p, float x, float y, float w, float h, int tint, string name = "Paper")
	{
		Widget f = INT_UIF_Draw.Frame(p, x, y, w, h, name);
		INT_UIF_Draw.Line(f, 0, 0, w, h, tint);
		INT_QST_Look.Tex(f, 0, 0, w, h, "int_qst_paper_ca", tint);
		return f;
	}

	// Leather cover with a stitched seam `inset` px inside the edge.
	static Widget Cover(Widget p, float x, float y, float w, float h, int tint, float inset, string name = "Cover")
	{
		Widget f = INT_UIF_Draw.Frame(p, x, y, w, h, name);
		INT_UIF_Surface.Box(f, 0, 0, w, h, 14, tint, LeatherLine());
		INT_QST_Look.Tex(f, 4, 4, w - 8, h - 8, "int_qst_leather_ca", tint);
		INT_QST_Look.Stitches(f, inset, inset, w - inset * 2, h - inset * 2);
		return f;
	}

	static void Stitches(Widget p, float x, float y, float w, float h)
	{
		int c = Stitch();
		INT_QST_Look.Tex(p, x, y, w, 4, "int_qst_stitch_h_ca", c);
		INT_QST_Look.Tex(p, x, y + h - 4, w, 4, "int_qst_stitch_h_ca", c);
		INT_QST_Look.Tex(p, x, y, 4, h, "int_qst_stitch_v_ca", c);
		INT_QST_Look.Tex(p, x + w - 4, y, 4, h, "int_qst_stitch_v_ca", c);
	}

	// Rubber stamp: a double-ruled frame with spaced caps (blue = in progress / new, red = ready / done / urgent).
	static Widget Stamp(Widget p, float x, float y, string key, int color, float size = 11, string icon = "")
	{
		string t = INT_QST_Look.Spaced(key);
		float iconW = 0;
		if (icon != "")
			iconW = size + 6;
		float tw = INT_UIF_Draw.TextWidth(t, size) * 1.1 + iconW;
		float w = tw + 16;
		float h = size + 10;
		Widget f = INT_UIF_Draw.Frame(p, x, y, w, h, "Stamp");
		INT_UIF_Surface.Box(f, 0, 0, w, h, 4, 0, color);
		INT_UIF_Surface.Box(f, 2, 2, w - 4, h - 4, 4, 0, color);
		if (icon != "")
			INT_QST_Look.Icon(f, 8, (h - size) * 0.5, size, icon, color);
		INT_QST_Look.Heavy(f, 8 + iconW, 0, tw, h, t, size, color);
		return f;
	}

	static float StampWidth(string key, float size = 11, bool icon = false)
	{
		string t = INT_QST_Look.Spaced(key);
		float w = INT_UIF_Draw.TextWidth(t, size) * 1.1 + 16;
		if (icon)
			w = w + size + 6;
		return w;
	}

	// Small kraft chip with an icon (timer, distance, slots).
	static Widget Chip(Widget p, float x, float y, string icon, string text, out float width)
	{
		float tw = INT_UIF_Draw.TextWidth(text, 12) * 1.08;
		float w = tw + 34;
		width = w;
		Widget f = INT_UIF_Draw.Frame(p, x, y, w, 24, "Chip");
		INT_UIF_Surface.Box(f, 0, 0, w, 24, 4, 0x33FFFFFF, InkA(70));
		INT_QST_Look.Icon(f, 9, 6, 12, icon, Ink2());
		INT_QST_Look.Text(f, 26, 0, tw + 6, 24, text, 12, Ink2());
		return f;
	}

	// Cream keycap on leather (footer hints).
	static float Keycap(Widget p, float x, float y, string key, string label)
	{
		float kw = INT_UIF_Draw.TextWidth(key, 11) * 1.15 + 16;
		if (kw < 28)
			kw = 28;
		Widget k = INT_UIF_Draw.Frame(p, x, y, kw, 26, "Key");
		int kf = Cream();
		int kb = Tan2();
		INT_UIF_Surface.Box(k, 0, 0, kw, 26, 4, kf, kb);
		INT_UIF_Draw.Line(k, 2, 22, kw - 4, 2, 0x33000000);
		INT_QST_Look.Heavy(k, 0, 0, kw, 24, key, 11, Ink(), INT_UIF_Align.CENTER);
		float lw = 0;
		if (label != "")
		{
			string lt = INT_QST_Look.Tr(label);
			lw = INT_UIF_Draw.TextWidth(lt, 13) * 1.05 + 8;
			INT_QST_Look.Text(p, x + kw + 8, y, lw, 26, lt, 13, CreamA(210));
		}
		return kw + 8 + lw;
	}

	// Pushpin (track on HUD): a small paper button with a red pin; dim when off.
	static Widget PinTile(Widget p, float x, float y, float size, bool on)
	{
		Widget f = INT_UIF_Draw.Frame(p, x, y, size, size, "Pin");
		int fill = Note();
		int border = InkA(70);
		INT_UIF_Surface.Box(f, 0, 0, size, size, 4, fill, border);
		int pc = Red();
		if (!on)
			pc = InkA(90);
		INT_QST_Look.Icon(f, size * 0.22, size * 0.2, size * 0.56, "pushpin", pc);
		return f;
	}

	static void Brand(Widget p, float xRight, float y, int color)
	{
		string t = "Made by DayZavr Project";
		float tw = INT_UIF_Draw.TextWidth(t, 12) * 1.15;
		INT_QST_Look.Heavy(p, xRight - tw, y, tw, 18, t, 12, color, INT_UIF_Align.RIGHT);
	}

	// Buttons: INK = dark ink slab with cream text (main action), DANGER = red outline, GHOST = ink outline.
	static const int BTN_INK = 0;
	static const int BTN_DANGER = 1;
	static const int BTN_GHOST = 2;

	// Returns the button frame; `hover` is the highlight to bind to the hit (shown on mouse over).
	static Widget Button(Widget p, float x, float y, float w, float h, string label, string icon, int style, int iconColor, out Widget hover)
	{
		Widget f = INT_UIF_Draw.Frame(p, x, y, w, h, "Btn");
		int fill = Ink();
		int border = LeatherDark();
		int tc = Cream();
		int hf = 0xFF3A3024;
		if (style == BTN_DANGER)
		{
			fill = RedA(20);
			border = RedA(170);
			tc = Red();
			hf = RedA(46);
		}
		else if (style == BTN_GHOST)
		{
			fill = 0x26FFFFFF;
			border = InkA(120);
			tc = Ink();
			hf = 0x52FFFFFF;
		}
		INT_UIF_Surface.Box(f, 0, 0, w, h, 6, fill, border);
		hover = INT_UIF_Draw.Frame(f, 0, 0, w, h, "Hover");
		INT_UIF_Surface.Box(hover, 0, 0, w, h, 6, hf, border);
		string t = INT_QST_Look.Upper(label);
		float size = 14;
		if (h >= 56)
			size = 17;
		float tw = INT_UIF_Draw.TextWidth(t, size) * 1.3;
		float iw = 0;
		if (icon != "")
			iw = size + 10;
		float cx = (w - tw - iw) * 0.5;
		if (cx < 12)
			cx = 12;
		if (icon != "")
			INT_QST_Look.Icon(f, cx, (h - size - 2) * 0.5, size + 2, icon, iconColor);
		INT_QST_Look.Heavy(f, cx + iw, 0, w - cx - iw - 6, h, t, size, tc);
		return f;
	}

	// On/off switch (pill with a knob).
	static Widget Switch(Widget p, float x, float y, bool on)
	{
		Widget f = INT_UIF_Draw.Frame(p, x, y, 40, 22, "Switch");
		int fill = 0x40FFFFFF;
		float kx = 3;
		if (on)
		{
			fill = 0xFFD8453A;
			kx = 21;
		}
		INT_UIF_Surface.Pill(f, 0, 0, 40, 22, fill, 0x40000000);
		INT_UIF_Surface.Pill(f, kx, 3, 16, 16, Cream(), 0);
		return f;
	}

	// Progress bar outlined in ink (pct 0..100).
	static void Bar(Widget p, float x, float y, float w, float h, float pct)
	{
		INT_UIF_Surface.Box(p, x, y, w, h, 4, 0x4DFFFAEB, Ink2());
		float fw = (w - 2) * pct / 100.0;
		if (fw > 0.5)
			INT_UIF_Draw.Line(p, x + 1, y + 1, fw, h - 2, Ink2());
	}

	// Tally boxes ("3 of 5" as small squares), done = olive.
	static void Tally(Widget p, float x, float y, int have, int need, bool done)
	{
		int on = Ink2();
		if (done)
			on = Olive();
		for (int i = 0; i < need; i++)
		{
			float bx = x + i * 19;
			INT_UIF_Surface.Box(p, bx, y, 14, 14, 4, 0, on);
			if (i < have)
				INT_UIF_Draw.Line(p, bx + 3, y + 3, 8, 8, on);
		}
	}

	// Hairline and double rule on paper.
	static void Hair(Widget p, float x, float y, float w)
	{
		INT_UIF_Draw.Line(p, x, y, w, 1, InkA(60));
	}

	static void DoubleRule(Widget p, float x, float y, float w)
	{
		INT_UIF_Draw.Line(p, x, y, w, 1, InkA(110));
		INT_UIF_Draw.Line(p, x, y + 3, w, 1, InkA(110));
	}

	// Red pushpin head over a pinned note.
	static void PinHead(Widget p, float x, float y)
	{
		INT_QST_Look.Icon(p, x, y, 26, "pushpin", Red());
	}

	// Ruled lines (the speech note).
	static void Ruled(Widget p, float x, float y, float w, float h, float step)
	{
		float ly = y + step;
		while (ly < y + h - 2)
		{
			INT_UIF_Draw.Line(p, x, ly, w, 1, Rule());
			ly = ly + step;
		}
	}
}

// Click / hover / wheel dispatch for the diary windows (menus are not widget event handlers themselves).
class INT_QST_Clicks extends ScriptedWidgetEventHandler
{
	protected ref map<Widget, int> m_Ids = new map<Widget, int>();
	protected ref map<Widget, int> m_Data = new map<Widget, int>();
	protected ref map<Widget, Widget> m_Hover = new map<Widget, Widget>();	// hit -> highlight widget shown on hover
	protected ref array<Widget> m_Grave = new array<Widget>();	// forgotten widgets, released a frame later
	protected float m_GraveAt = -1;
	ref ScriptInvoker Event_Click = new ScriptInvoker();		// (int id, int data, int button)
	ref ScriptInvoker Event_Wheel = new ScriptInvoker();		// (int id, int wheel)
	ref ScriptInvoker Event_Down = new ScriptInvoker();		// (int id, int data, int button)

	void Bind(Widget hit, int id, int data = 0, Widget hover = null)
	{
		if (!hit)
			return;
		m_Ids.Set(hit, id);
		m_Data.Set(hit, data);
		if (hover)
		{
			m_Hover.Set(hit, hover);
			hover.Show(false);
		}
		else
			m_Hover.Remove(hit);
	}

	void Clear()
	{
		m_Ids.Clear();
		m_Data.Clear();
		m_Hover.Clear();
	}

	// Drops the bindings of every widget inside `root` (root itself stays bound). Call it before a part of a window
	// is cleared and rebuilt, else the maps only grow and keep the removed widgets. The dropped widgets stay
	// referenced until a later frame: the call may come from the click of one of them (never free it in its event).
	void ForgetUnder(Widget root)
	{
		if (!root)
			return;
		float now = 0;
		if (GetGame())
			now = GetGame().GetTickTime();
		if (m_GraveAt >= 0 && now > m_GraveAt)
			m_Grave.Clear();
		array<Widget> drop = new array<Widget>();
		int n = m_Ids.Count();
		for (int i = 0; i < n; i++)
		{
			Widget w = m_Ids.GetKey(i);
			if (!w)
			{
				drop.Insert(w);
				continue;
			}
			Widget p = w.GetParent();
			while (p)
			{
				if (p == root)
				{
					drop.Insert(w);
					break;
				}
				p = p.GetParent();
			}
		}
		foreach (Widget d : drop)
		{
			m_Ids.Remove(d);
			m_Data.Remove(d);
			m_Hover.Remove(d);
			if (d)
				m_Grave.Insert(d);
		}
		m_GraveAt = now;
	}

	int BoundCount()
	{
		return m_Ids.Count();
	}

	protected bool Resolve(Widget w, out int id, out int data)
	{
		if (!m_Ids.Find(w, id))
			return false;
		m_Data.Find(w, data);
		return true;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int id;
		int data;
		if (!Resolve(w, id, data))
			return false;
		if (button != MouseState.LEFT)
			return true;		// the right button is reported from OnMouseButtonDown
		Event_Click.Invoke(id, data, button);
		return true;
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		int id;
		int data;
		if (!Resolve(w, id, data))
			return false;
		Event_Down.Invoke(id, data, button);
		// right click never reaches OnClick on a ButtonWidget: report it here
		if (button == MouseState.RIGHT)
			Event_Click.Invoke(id, data, button);
		return false;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		int id;
		int data;
		if (!Resolve(w, id, data))
			return false;
		Event_Wheel.Invoke(id, wheel);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		Widget h;
		if (m_Hover.Find(w, h) && h)
			h.Show(true);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		Widget h;
		if (m_Hover.Find(w, h) && h)
			h.Show(false);
		return false;
	}
}

// UI sounds (CfgSoundSets INT_QST_<kind>_SoundSet, tools/gen_qst_sfx.py). 2D, UI channel, throttled.
class INT_QST_Sfx
{
	static const float MIN_GAP = 0.045;
	protected static float s_LastAt = -100;
	static int s_Played;		// self-test counter

	static void Play(string kind)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		float now = GetGame().GetTickTime();
		if (now - s_LastAt < MIN_GAP)
			return;
		// "open" -> INT_QST_Open_SoundSet (tools/INT_QST_sounds.cpp.txt)
		string first = kind.Substring(0, 1);
		first.ToUpper();
		string rest = kind.Substring(1, kind.Length() - 1);
		SoundParams params = new SoundParams("INT_QST_" + first + rest + "_SoundSet");
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
