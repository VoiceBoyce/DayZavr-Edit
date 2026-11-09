// Приборы и мелкие детали из макета на примитивах фреймворка. Координаты в пикселях холста 1920x1080.
class VBT
{
	static string T(string key)
	{
		return Widget.TranslateString(key);
	}

	static ImageWidget Ico(Widget p, float x, float y, float s, string name, int color)
	{
		// Формы и символы эмблем крупно берутся из большого атласа.
		string iconSet = VB_TABLET_ICON;
		if (s >= 24 && (name.IndexOf("shp_") == 0 || name.IndexOf("sym_") == 0))
			iconSet = VB_TABLET_ICON_EM;
		return INT_UIF_Draw.Texture(p, x, y, s, s, iconSet + name, color);
	}

	static Widget Card(Widget p, float x, float y, float w, float h, string cap = "")
	{
		Widget f = INT_UIF_Draw.Frame(p, x, y, w, h);
		INT_UIF_Surface.Box(f, 0, 0, w, h, 10, VB_C_PANEL, VB_C_LINE);
		if (cap != "")
			Cap(f, 20, 16, w - 40, cap);
		return f;
	}

	static TextWidget Cap(Widget p, float x, float y, float w, string text, int color = VB_C_DIM)
	{
		return INT_UIF_Draw.Text(p, x, y, w, 18, text, 13, color);
	}

	// Блик карточки слева. Повторяет скругление: левый край собран из уголков и полосы, отступ 1 пиксель до рамки.
	static void Glow(Widget p, float w, float h, int color = 0)
	{
		if (color == 0)
			color = VB_C_ACCENT;
		// Радиус как у INT_UIF_Surface.Box(..., 10, ...): он выбирает готовый стиль по экранным пикселям.
		float f = INT_UIF_Scale.Factor();
		float px = 10 * f;
		float r = 4;
		if (px >= 14)
			r = 14;
		else if (px >= 12)
			r = 12;
		else if (px >= 10)
			r = 10;
		else if (px >= 8)
			r = 8;
		else if (px >= 6)
			r = 6;
		float inset = 1 / f;
		float c = (r - 1) / f;
		float bw = w - inset * 2;
		float bh = h - inset * 2;
		int tint = INT_UIF_Color.WithAlpha(color, 22);
		INT_UIF_Draw.Texture(p, inset, inset, c, c, VB_TABLET_ICON_EM + "corner_t", tint);
		INT_UIF_Draw.Texture(p, inset, inset + bh - c, c, c, VB_TABLET_ICON_EM + "corner_b", tint);
		INT_UIF_Draw.Texture(p, inset, inset + c, c, bh - c * 2, VB_TABLET_ICON_EM + "solid", tint);
		INT_UIF_Draw.Texture(p, inset + c, inset, bw - c, bh, VB_TABLET_ICON_EM + "glow", tint);
	}

	static void Corners(Widget p, float w, float h, int color = 0, float k = 18)
	{
		if (color == 0)
			color = VB_C_ACCENT;
		INT_UIF_Draw.Line(p, 0, 0, k, 2, color);
		INT_UIF_Draw.Line(p, 0, 0, 2, k, color);
		INT_UIF_Draw.Line(p, w - k, 0, k, 2, color);
		INT_UIF_Draw.Line(p, w - 2, 0, 2, k, color);
		INT_UIF_Draw.Line(p, 0, h - 2, k, 2, color);
		INT_UIF_Draw.Line(p, 0, h - k, 2, k, color);
		INT_UIF_Draw.Line(p, w - k, h - 2, k, 2, color);
		INT_UIF_Draw.Line(p, w - 2, h - k, 2, k, color);
	}

	static void Ticks(Widget p, float x, float y, float w, float h, int n, float k, int color = 0)
	{
		if (color == 0)
			color = VB_C_ACCENT;
		float step = w / n;
		float bw = Math.Max(2, step * 0.56);
		int on = Math.Round(n * Math.Clamp(k, 0, 1));
		for (int i = 0; i < n; i++)
		{
			int c = VB_C_PANEL3;
			if (i < on)
				c = color;
			INT_UIF_Draw.Line(p, x + i * step, y, bw, h, c);
		}
	}

	// Счёт добычи: точки в два ряда.
	static void Dots(Widget p, float x, float y, int cols, float k, int color = 0)
	{
		if (color == 0)
			color = VB_C_ACCENT;
		int n = cols * 2;
		int on = Math.Round(n * Math.Clamp(k, 0, 1));
		for (int i = 0; i < n; i++)
		{
			int c = VB_C_PANEL3;
			if (i < on)
				c = color;
			int row = i / cols;
			int col = i % cols;
			INT_UIF_Surface.Pill(p, x + col * 14, y + row * 13, 9, 9, c);
		}
	}

	// Засечки пятёрками: четыре палочки и косая не повторяем поворотом, в игре прямая перекладина.
	static void Tally(Widget p, float x, float y, float h, int count, int goal)
	{
		int n = count;
		if (count > 20)
			n = 20.0 * count / goal;
		float cx = x;
		int groups = Math.Ceil(n / 5.0);
		for (int g = 0; g < groups; g++)
		{
			int left = Math.Min(5, n - g * 5);
			int sticks = Math.Min(4, left);
			for (int i = 0; i < sticks; i++)
			{
				int up = i % 2;
				int cut = i % 3;
				INT_UIF_Draw.Line(p, cx + i * 8, y + up, 3, h - cut, VB_C_TEXT);
			}
			if (left == 5)
			{
				ImageWidget bar = INT_UIF_Draw.Line(p, cx - 5, y + h * 0.5 - 1, 40, 3, VB_C_ACCENT);
				if (bar)
					bar.SetRotation(0, 0, -24);
			}
			cx += 42;
		}
	}

	// Маршрут: пунктир до цели и точка назначения.
	static void Route(Widget p, float x, float y, float w, float k)
	{
		int n = 16;
		float step = (w - 22) / n;
		int on = Math.Round(n * Math.Min(1, k));
		INT_UIF_Surface.Pill(p, x, y + 6, 8, 8, VB_C_ACCENT);
		for (int i = 0; i < n; i++)
		{
			int c = VB_C_LINE2;
			if (i < on)
				c = VB_C_ACCENT;
			INT_UIF_Draw.Line(p, x + 12 + i * step, y + 9, step * 0.6, 2, c);
		}
		int pc = VB_C_DIM2;
		if (k >= 1)
			pc = VB_C_ACCENT;
		Ico(p, x + w - 18, y, 20, "pin", pc);
	}

	// Значок ранга: звёзды и шевроны.
	static Widget Insignia(Widget p, float x, float y, float s, int rank, int color = 0)
	{
		if (color == 0)
			color = VB_C_ACCENT;
		array<int> chevrons = {1, 2, 3, 1, 2, 3, 1, 2, 3, 0};
		array<int> stars = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3};
		int c = chevrons.Get(rank);
		int st = stars.Get(rank);
		Widget f = INT_UIF_Draw.Frame(p, x, y, s, s);
		float ss = s * 0.3;
		float ch = s * 0.34;
		float gap = s * 0.21;
		float total = 0;
		if (st > 0)
			total += ss;
		if (st > 0 && c > 0)
			total += s * 0.08;
		if (c > 0)
			total += ch + (c - 1) * gap;
		float cy = (s - total) * 0.5;
		if (st > 0)
		{
			float w = st * ss + (st - 1) * s * 0.03;
			for (int i = 0; i < st; i++)
				Ico(f, (s - w) * 0.5 + i * (ss + s * 0.03), cy, ss, "sym_star", color);
			cy += ss;
			if (c > 0)
				cy += s * 0.08;
		}
		for (int k = 0; k < c; k++)
			INT_UIF_Draw.Texture(f, s * 0.14, cy + k * gap, s * 0.72, ch, VB_TABLET_ICON + "chev", color);
		return f;
	}

	// Светлее на 28%: наведение на основную кнопку.
	static int Light(int color)
	{
		int a = (color >> 24) & 255;
		int r = (color >> 16) & 255;
		int g = (color >> 8) & 255;
		int b = color & 255;
		r = r + (255 - r) * 0.28;
		g = g + (255 - g) * 0.28;
		b = b + (255 - b) * 0.28;
		return ARGB(a, r, g, b);
	}

	static string RankName(int rank)
	{
		return "#STR_VB_TAB_RANKN_" + rank.ToString();
	}

	// 1 234 567
	static string Num(int value)
	{
		// Знак отдельно: иначе у «-500» пробел встал бы после минуса.
		if (value < 0)
			return "-" + Num(-value);
		string digits = value.ToString();
		string res = "";
		int n = digits.Length();
		for (int i = 0; i < n; i++)
		{
			int fromEnd = n - i;
			if (i > 0 && fromEnd % 3 == 0)
				res = res + " ";
			res = res + digits.Get(i);
		}
		return res;
	}

	// 250, 5 тыс., 2,5 млн
	static string Short(int v)
	{
		if (v >= 1000000)
			return Tenth(v / 1000000.0) + " " + T("#STR_VB_TAB_M");
		if (v >= 1000)
			return Tenth(v / 1000.0) + " " + T("#STR_VB_TAB_K");
		return v.ToString();
	}

	// Одна цифра после запятой, ноль не пишем.
	static string Tenth(float v)
	{
		int t = Math.Round(v * 10);
		int whole = t / 10;
		int frac = t % 10;
		if (frac == 0)
			return whole.ToString();
		return whole.ToString() + "," + frac.ToString();
	}

	static string Km(int meters)
	{
		return Tenth(meters / 1000.0) + " " + T("#STR_VB_TAB_KM");
	}

	static string LifeText(int sec)
	{
		int h = sec / 3600;
		if (h >= 1000)
			return Num(h) + " " + T("#STR_VB_TAB_H");
		int m = (sec % 3600) / 60;
		return h.ToString() + " " + T("#STR_VB_TAB_H") + " " + m.ToString() + " " + T("#STR_VB_TAB_MIN");
	}

	// Следующая круглая цель без потолка: 10, 25, 50, 100, 250, 500...
	static int Goal(int v)
	{
		int e = 10;
		while (e < 100000000)
		{
			if (v < e)
				return e;
			if (v < e * 2.5)
				return e * 2.5;
			if (v < e * 5)
				return e * 5;
			e = e * 10;
		}
		return e;
	}

	protected static ref map<string, float> s_TextW;

	// Ширина текста по метрикам движка (пробный виджет, в единицах макета). Оценка TextWidth фреймворка
	// (0,53 высоты на знак) для этого шрифта мала, особенно на заглавных: подписи вылезали из рамок.
	// cache false: для полей ввода, где текст меняется на каждый знак.
	static float TextW(Widget p, string text, float size, bool cache = true)
	{
		float f = INT_UIF_Scale.Factor();
		string key = size.ToString() + "|" + f.ToString() + "|" + text;
		float w;
		if (cache && s_TextW && s_TextW.Find(key, w))
			return w;
		w = INT_UIF_Draw.TextWidth(text, size) * 1.2;
		if (!p || text == "")
			return w;
		TextWidget probe = INT_UIF_Draw.Text(p, 0, 0, 3000, size * 2, text, size, 0);
		if (!probe)
			return w;
		probe.Update();
		int mw;
		int mh;
		probe.GetTextSize(mw, mh);
		probe.Unlink();
		if (mw <= 0 || f <= 0)
			return w;
		w = mw / f;
		if (cache)
		{
			if (!s_TextW || s_TextW.Count() > 4000)
				s_TextW = new map<string, float>();
			s_TextW.Insert(key, w);
		}
		return w;
	}

	static string Fit(string s, float w, float size)
	{
		if (INT_UIF_Draw.TextWidth(s, size) <= w)
			return s;
		int n = s.LengthUtf8();
		while (n > 1)
		{
			n--;
			string cut = s.SubstringUtf8(0, n) + "...";
			if (INT_UIF_Draw.TextWidth(cut, size) <= w)
				return cut;
		}
		return s;
	}

	static void Pad2(out string res, int v)
	{
		res = v.ToString();
		if (v < 10)
			res = "0" + res;
	}
}

// Кнопка макета: подложка, значок, подпись. Клик ловит страница через Hit.
class VBT_Button extends Managed
{
	Widget Hit;
	bool Primary;
	bool Danger;
	ref INT_UIF_Surface Bg;

	void VBT_Button(Widget parent, float x, float y, float w, float h, string text, bool primary, string icon, ScriptedWidgetEventHandler handler, float size = 15)
	{
		Primary = primary;
		int fill = VB_C_PANEL2;
		int border = VB_C_LINE2;
		int ink = VB_C_TEXT;
		if (primary)
		{
			fill = VB_C_ACCENT;
			border = 0;
			ink = VB_C_ON;
		}
		Bg = INT_UIF_Surface.Box(parent, x, y, w, h, 8, fill, border);
		// Значок с подписью по центру кнопки, ширина подписи по метрикам движка.
		float tw = VBT.TextW(parent, text, size);
		float full = tw;
		if (icon != "")
			full += 28;
		float tx = x + (w - full) * 0.5;
		if (icon != "")
		{
			VBT.Ico(parent, tx, y + (h - 18) * 0.5, 18, icon, ink);
			tx += 28;
		}
		INT_UIF_Draw.Text(parent, tx, y, tw + 30, h, text, size, ink);
		Hit = INT_UIF_Draw.Hit(parent, x, y, w, h, handler);
		if (text.Length() > 1 && text.Get(0) == "#")
			Hit.SetName("VBT_Btn_" + text.Substring(1, text.Length() - 1));
	}

	void Hover(bool enter)
	{
		int fill = VB_C_PANEL2;
		if (Primary)
			fill = VB_C_ACCENT;
		if (Danger)
			fill = VB_C_BAD;
		if (enter)
		{
			fill = VB_C_PANEL3;
			if (Primary)
				fill = VBT.Light(VB_C_ACCENT);
			if (Danger)
				fill = 0xFFF47A6C;
		}
		Bg.SetFill(fill);
	}
}

// Вкладки в верхней строке.
class VB_TabletTabbedPage extends VB_TabletPage
{
	protected Widget m_Tabs;
	protected ref array<Widget> m_TabHits;
	protected ref array<TextWidget> m_TabTexts;
	protected ref array<string> m_TabNames;
	protected float m_TabsX = 180;

	void InitTabs(float x)
	{
		m_TabsX = x;
		m_TabNames = new array<string>();
		m_TabHits = new array<Widget>();
		m_TabTexts = new array<TextWidget>();
	}

	int GetTab();
	void SetTab(int i);

	void PaintTabs()
	{
		if (m_Tabs)
			m_Tabs.Unlink();
		m_Tabs = INT_UIF_Draw.Frame(m_Menu.GetTopBar(), m_TabsX, 0, 400, 72);
		m_TabHits.Clear();
		m_TabTexts.Clear();
		float x = 0;
		int n = m_TabNames.Count();
		for (int i = 0; i < n; i++)
		{
			float w = INT_UIF_Draw.TextWidth(m_TabNames.Get(i), 18) + 8;
			int col = VB_C_DIM;
			if (i == GetTab())
			{
				col = VB_C_TEXT;
				INT_UIF_Draw.Line(m_Tabs, x, 68, w, 3, VB_C_ACCENT);
			}
			m_TabTexts.Insert(INT_UIF_Draw.Text(m_Tabs, x, 0, w + 30, 72, m_TabNames.Get(i), 18, col));
			Widget hit = HitAt(m_Tabs, x - 10, 0, w + 20, 72);
			hit.SetName("VBT_Tab_" + i.ToString());
			m_TabHits.Insert(hit);
			x += w + 40;
		}
	}

	override void Leave()
	{
		if (m_Tabs)
			m_Tabs.Unlink();
		m_Tabs = null;
	}

	override bool OnHit(Widget w)
	{
		int i = m_TabHits.Find(w);
		if (i < 0)
			return false;
		if (i != GetTab())
		{
			SetTab(i);
			m_Menu.RequestRepaint();
		}
		return true;
	}

	override bool OnHover(Widget w, bool enter)
	{
		int i = m_TabHits.Find(w);
		if (i < 0 || i == GetTab())
			return false;
		int col = VB_C_DIM;
		if (enter)
			col = VB_C_TEXT;
		m_TabTexts.Get(i).SetColor(col);
		return true;
	}
}
