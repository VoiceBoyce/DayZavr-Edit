// INT PartyMode (client) — small helpers of the HUD: widget creation and placement in design px times a part scale,
// colour + alpha in one call (SetAlpha REPLACES the colour alpha, so colour first, alpha second), easing curves, the
// distance text, a faux-bold text pair, the per-frame context and the cached info of a shown friend.
// Every widget of the HUD is created once (pools) and only moved / recoloured afterwards.

class INT_PTY_HudKit
{
	static float s_F = 1.0;				// screen px per design px (INT_UIF_Scale.Factor), set by the core
	protected static string s_FmtM = "%1 m";
	protected static string s_FmtKm = "%1.%2 km";

	// Texts that do not change per frame: read once per refresh (TranslateString is not free).
	static void CacheTexts()
	{
		string fm = Widget.TranslateString("#STR_INT_PTY_H_M");
		string fkm = Widget.TranslateString("#STR_INT_PTY_H_KM");
		s_FmtM = fm;
		s_FmtKm = fkm;
	}

	static ImageWidget Img(Widget parent, string texName, int rgb)
	{
		ImageWidget img = INT_UIF_Draw.Texture(parent, 0, 0, 1, 1, INT_PTY_TEX + texName + ".paa", rgb);
		return img;
	}

	// An image with a full texture path (ping shapes come from INT_PTY_UI.PingTex).
	static ImageWidget ImgPath(Widget parent, string path, int rgb)
	{
		return INT_UIF_Draw.Texture(parent, 0, 0, 1, 1, path, rgb);
	}

	static Widget Frame(Widget parent, string name)
	{
		return INT_UIF_Draw.Frame(parent, 0, 0, 1, 1, name);
	}

	// Design px (x, y, w, h) times k (part scale) -> screen px relative to the parent.
	static void Rect(Widget w, float x, float y, float wd, float ht, float k)
	{
		if (!w)
			return;
		float f = s_F * k;
		w.SetPos(x * f, y * f);
		w.SetSize(wd * f, ht * f);
	}

	// A square of side `side` design px centred on (cx, cy) design px, times k.
	static void Square(Widget w, float cx, float cy, float side, float k)
	{
		if (!w)
			return;
		float f = s_F * k;
		float s = side * f;
		w.SetPos(cx * f - s * 0.5, cy * f - s * 0.5);
		w.SetSize(s, s);
	}

	static void Paint(Widget w, int rgb, float alpha)
	{
		if (!w)
			return;
		w.SetColor(rgb | 0xFF000000);
		w.SetAlpha(Math.Clamp(alpha, 0, 1));
	}

	// H-8: null-safe one-statement forms for pooled widgets (UIF may not have created one, or another mod unlinked the
	// part frame it got from INT_PTY_UI.FindPart — the core then rebuilds the layer on the next frame).
	static void Vis(Widget w, bool on)
	{
		if (w)
			w.Show(on);
	}

	static void PlaceAt(Widget w, float x, float y)
	{
		if (w)
			w.SetPos(x, y);
	}

	static void SizeTo(Widget w, float wd, float ht)
	{
		if (w)
			w.SetSize(wd, ht);
	}

	static void Turn(Widget w, float deg)
	{
		if (w)
			w.SetRotation(0, 0, deg);
	}

	static int Mix(int c1, int c2, float t)
	{
		return INT_UIF_Color.Lerp(c1, c2, t);
	}

	// --- easing -------------------------------------------------------------------------------------------------
	// 0..1 position of t inside [t0, t1].
	static float Seg(float t, float t0, float t1)
	{
		if (t <= t0)
			return 0;
		if (t >= t1)
			return 1;
		return (t - t0) / (t1 - t0);
	}

	static float Out(float t)
	{
		float u = 1 - t;
		return 1 - u * u;
	}

	static float OutCubic(float t)
	{
		float u = 1 - t;
		return 1 - u * u * u;
	}

	static float In(float t)
	{
		return t * t;
	}

	static float InOut(float t)
	{
		if (t < 0.5)
			return 2 * t * t;
		float u = -2 * t + 2;
		return 1 - u * u * 0.5;
	}

	// CSS cubic-bezier(x1, y1, x2, y2) at progress t (Newton on x, then y).
	static float Bezier(float t, float x1, float y1, float x2, float y2)
	{
		if (t <= 0)
			return 0;
		if (t >= 1)
			return 1;
		float s = t;
		for (int i = 0; i < 6; i++)
		{
			float u1 = 1 - s;
			float x = 3 * u1 * u1 * s * x1 + 3 * u1 * s * s * x2 + s * s * s - t;
			float dx = 3 * u1 * u1 * x1 + 6 * u1 * s * (x2 - x1) + 3 * s * s * (1 - x2);
			if (dx > -0.0001 && dx < 0.0001)
				break;
			s = Math.Clamp(s - x / dx, 0, 1);
		}
		float js = 1 - s;
		return 3 * js * js * s * y1 + 3 * js * s * s * y2 + s * s * s;
	}

	// Natural logarithm (Math.Log2 is not trusted on 1.30: it spun in RFFS). Range-reduced series, ~1e-6.
	static float Ln(float x)
	{
		if (x <= 0)
			return -50;
		float v = x;
		int k = 0;
		while (v > 2.0)
		{
			v = v * 0.5;
			k++;
		}
		while (v < 0.5)
		{
			v = v * 2.0;
			k--;
		}
		float y = (v - 1) / (v + 1);
		float y2 = y * y;
		float term = y;
		float sum = 0;
		for (int i = 0; i < 7; i++)
		{
			sum = sum + term / (2 * i + 1);
			term = term * y2;
		}
		return 2 * sum + k * 0.69314718;
	}

	// «124 м», «1,2 км» (format keys STR_INT_PTY_H_M / _KM).
	static string Dist(float meters)
	{
		if (meters < 999.5)
		{
			int m = (int)Math.Round(meters);
			string a = s_FmtM;
			a.Replace("%1", m.ToString());
			return a;
		}
		int dm = (int)Math.Round(meters / 100.0);
		int whole = dm / 10;
		int tenth = dm % 10;
		string b = s_FmtKm;
		b.Replace("%1", whole.ToString());
		b.Replace("%2", tenth.ToString());
		return b;
	}

	// Rough rendered width of a text line in design px (calibrations of INT_PTY_Look.TextW: capitals 0.62 em, faux
	// bold 0.66 em; mixed text and digits ~0.56 em) + letter spacing.
	static float TextW(string text, float size, bool caps, bool bold, float spacing)
	{
		float k = 0.56;
		if (caps)
			k = 0.62;
		if (caps && bold)
			k = 0.66;
		int n = text.LengthUtf8();
		int wide = INT_PTY_Text.WideCount(text);
		return (n - wide) * size * k + wide * size + n * spacing;
	}

	// Mixed-case text cut with an ellipsis to `room` design px (names in the tracker).
	static string Fit(string text, float size, float room)
	{
		if (TextW(text, size, false, false, 0) <= room)
			return text;
		int n = text.LengthUtf8();
		while (n > 1)
		{
			n--;
			string cut = text.SubstringUtf8(0, n) + "…";
			if (TextW(cut, size, false, false, 0) <= room)
				return cut;
		}
		return "…";
	}

	static int HpColor(int hp)
	{
		if (hp < 20)
			return 0xFFFF4B3E;
		if (hp < 50)
			return 0xFFFFC246;
		return 0xFF56E07E;
	}
}

// One line of text: a TextWidget plus (bold, from 16 px) a faux-bold twin shifted ~4.5 % of the size (the SDF font
// has no bold face, INT_PTY_Look.Heavy). Text, colour and alpha are set only when they change.
class INT_PTY_HudText : Managed
{
	TextWidget m_A;
	TextWidget m_B;
	string m_Text = "<none>";
	int m_Rgb = 1;
	float m_Alpha = -1;
	float m_Size;
	float m_Dx;
	bool m_Shown = true;
	bool m_Tracked;				// widely tracked capitals (≥ .2 em in the concept): letters split by spaces

	// spacing = the concept letter-spacing in design px. DayZ text widgets draw no letter spacing, so the widely
	// tracked small capitals (header «Г Р У П П А», «П О Г И Б», card titles) get a space between letters
	// (INT_PTY_Text.Spaced, as the window does); narrower tracking stays plain.
	void Build(Widget parent, float size, int align, bool bold, float k, float spacing = 0)
	{
		m_Size = size;
		m_Tracked = spacing >= 2.5;
		float px = size * k;
		m_A = INT_UIF_Draw.Text(parent, 0, 0, 10, 10, "", px, 0xFFFFFFFF, align);
		if (!m_A)
			return;
		if (spacing > 0)
			m_A.SetTextSpacing(INT_UIF_Scale.TextPx(spacing * k), 0);
		if (!bold)
			return;
		m_A.SetBold(true);
		if (px < 16)
			return;
		m_Dx = size * 0.045;
		if (size >= 30)
			m_Dx = size * 0.03;
		m_B = INT_UIF_Draw.Text(parent, 0, 0, 10, 10, "", px, 0xFFFFFFFF, align);
		if (!m_B)
			return;
		m_B.SetBold(true);
		if (spacing > 0)
			m_B.SetTextSpacing(INT_UIF_Scale.TextPx(spacing * k), 0);
	}

	void Rect(float x, float y, float w, float h, float k)
	{
		INT_PTY_HudKit.Rect(m_A, x, y, w, h, k);
		if (m_B)
			INT_PTY_HudKit.Rect(m_B, x + m_Dx, y, w, h, k);
	}

	void SetText(string s)
	{
		if (s == m_Text)
			return;
		m_Text = s;
		string shown = s;
		if (m_Tracked)
			shown = INT_PTY_Text.Spaced(s);
		if (m_A)
			m_A.SetText(shown);
		if (m_B)
			m_B.SetText(shown);
	}

	// Real width of the current text in design px at part scale k: engine metrics after Update() on a wide box of the
	// final height (the font size follows the height); `fallback` (the estimate) when the engine answers 0.
	float MeasureW(float h, float k, float fallback)
	{
		if (!m_A)
			return fallback;
		INT_PTY_HudKit.Rect(m_A, 0, 0, 900, h, k);
		m_A.Update();
		int mw;
		int mh;
		m_A.GetTextSize(mw, mh);
		float f = INT_PTY_HudKit.s_F * k;
		if (mw <= 0 || f <= 0)
			return fallback;
		return mw / f + m_Dx;
	}

	void Paint(int rgb, float alpha)
	{
		float a = Math.Clamp(alpha, 0, 1);
		if (rgb == m_Rgb && Math.AbsFloat(a - m_Alpha) < 0.004)
			return;
		m_Rgb = rgb;
		m_Alpha = a;
		INT_PTY_HudKit.Paint(m_A, rgb, a);
		if (m_B)
			INT_PTY_HudKit.Paint(m_B, rgb, a);
	}

	void Show(bool on)
	{
		if (on == m_Shown)
			return;
		m_Shown = on;
		if (m_A)
			m_A.Show(on);
		if (m_B)
			m_B.Show(on);
	}
}

// What the HUD knows about one shown friend between friend-list changes (rebuilt on FRIENDS / settings changes).
class INT_PTY_HudMate : Managed
{
	string m_Id;
	string m_Name;			// alias or nick
	string m_Upper;			// name in capitals (tags)
	string m_Short;			// capitals, at most 14 characters (under his pings)
	int m_Color;			// friend colour (pref or automatic)
	int m_PingShape;		// how I see his pings
	int m_PingColor;
	int m_Order;			// position in the friend list
}

// Per-frame state shared by the parts (filled once per frame by the core; no allocation).
class INT_PTY_HudCtx : Managed
{
	float m_Now;
	float m_Dt;
	float m_Sw;
	float m_Sh;
	vector m_CamPos;
	vector m_CamDir;
	vector m_Right;
	vector m_Up;
	vector m_MyPos;
	float m_CamYaw;			// degrees, 0 = north, clockwise
	bool m_World;			// camera / player data valid this frame
	bool m_Edit;			// API edit mode: samples instead of world data
	bool m_Arrows;			// edge arrows wanted this frame
	INT_PTY_Settings m_St;

	// Camera-space components of a world offset: x right, y up, z forward.
	void ToCamera(vector worldPos, out float lx, out float ly, out float lz)
	{
		vector d = worldPos - m_CamPos;
		lx = d[0] * m_Right[0] + d[1] * m_Right[1] + d[2] * m_Right[2];
		ly = d[0] * m_Up[0] + d[1] * m_Up[1] + d[2] * m_Up[2];
		lz = d[0] * m_CamDir[0] + d[1] * m_CamDir[1] + d[2] * m_CamDir[2];
	}

	// Screen point of a world position; false behind the camera.
	bool Project(vector worldPos, out float sx, out float sy)
	{
		vector s = GetGame().GetScreenPos(worldPos);
		sx = s[0];
		sy = s[1];
		return s[2] > 0;
	}

	bool OnScreen(float sx, float sy, float margin)
	{
		if (sx < -margin || sy < -margin)
			return false;
		return sx <= m_Sw + margin && sy <= m_Sh + margin;
	}

	// Bearing of a world point relative to where the camera looks (degrees, clockwise, 0 = straight ahead).
	float RelBearing(vector worldPos)
	{
		float dx = worldPos[0] - m_CamPos[0];
		float dz = worldPos[2] - m_CamPos[2];
		float b = Math.Atan2(dx, dz) * Math.RAD2DEG;
		return b - m_CamYaw;
	}
}
