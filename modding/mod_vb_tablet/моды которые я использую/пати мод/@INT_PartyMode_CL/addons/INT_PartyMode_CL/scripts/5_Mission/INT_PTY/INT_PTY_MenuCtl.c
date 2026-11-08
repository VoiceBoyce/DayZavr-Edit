// INT PartyMode (client) — controls of the main window in the Expedition look: buttons (.btn pri / ol / dg / dgf /
// dis / gh), pill toggles (.tg), hairline sliders with a diamond knob (.sl), segmented choices (.seg).
// The window registers the hit areas; these classes only draw and repaint.

class INT_PTY_MButton : Managed
{
	static const int PRI = 0;
	static const int OL = 1;
	static const int DG = 2;
	static const int DGF = 3;
	static const int DIS = 4;
	static const int GH = 5;
	static const int PLAIN = 6;

	Widget m_Root;
	ref INT_UIF_Surface m_Box;
	ref INT_UIF_Surface m_KeyBox;
	TextWidget m_Text;
	TextWidget m_KeyText;
	ImageWidget m_Icon;
	int m_Style;
	bool m_Hover;
	float m_W;
	float m_H;
	float m_X;
	float m_Y;

	// x = left edge (or the right edge with alignRight). The width follows the label.
	static INT_PTY_MButton Create(Widget parent, float x, float y, float h, string icon, string label, int style, bool alignRight = false, float size = 14, string keycap = "", float minW = 0)
	{
		INT_PTY_MButton b = new INT_PTY_MButton();
		string caps = INT_PTY_Text.Caps(label);
		float tw = INT_PTY_MD.Measure(parent, caps, size, true);
		float pad = 20;
		if (style == GH)
			pad = 12;
		float iconW = 0;
		float isz = size + 4;
		if (icon != "")
			iconW = isz + 10;
		float keyW = 0;
		if (keycap != "")
			keyW = INT_PTY_MD.Measure(parent, keycap, 12, true) + 16 + 12;
		float w = pad + iconW + tw + keyW + pad;
		if (w < minW)
			w = minW;
		float left = x;
		if (alignRight)
			left = x - w;
		b.m_W = w;
		b.m_H = h;
		b.m_X = left;
		b.m_Y = y;
		b.m_Style = style;
		b.m_Root = INT_UIF_Draw.Frame(parent, left, y, w, h, "PtyButton");
		b.m_Box = INT_PTY_MD.Box(b.m_Root, 0, 0, w, h, 0, 0);
		float cx = (w - iconW - tw - keyW) * 0.5;
		if (icon != "")
		{
			b.m_Icon = INT_PTY_MD.Ico(b.m_Root, cx, (h - isz) * 0.5, isz, icon, INT_PTY_MC.T);
			cx = cx + iconW;
		}
		b.m_Text = INT_PTY_Look.Text(b.m_Root, cx, 0, tw + 24, h, caps, size, INT_PTY_MC.T, INT_UIF_Align.LEFT, true);
		if (keycap != "")
		{
			float kx = cx + tw + 12;
			float kw = keyW - 12;
			b.m_KeyBox = INT_PTY_MD.Box(b.m_Root, kx, (h - 24) * 0.5, kw, 24, 0, INT_PTY_MC.KEY_EDGE);
			b.m_KeyText = INT_PTY_Look.Text(b.m_Root, kx, (h - 24) * 0.5, kw, 24, keycap, 12, INT_PTY_MC.T, INT_UIF_Align.CENTER, true);
		}
		b.Paint(false);
		return b;
	}

	void SetStyle(int style)
	{
		if (style == m_Style)
			return;
		m_Style = style;
		Paint(true);
	}

	void SetHover(bool on)
	{
		m_Hover = on;
		Paint(true);
	}

	protected void Paint(bool animate)
	{
		int fill = 0;
		int edge = 0;
		int tc = INT_PTY_MC.T;
		int keyEdge = INT_PTY_MC.KEY_EDGE;
		if (m_Style == PRI)
		{
			fill = INT_PTY_MC.O;
			edge = INT_PTY_MC.O;
			tc = INT_PTY_MC.INK;
			keyEdge = ARGB(128, 26, 12, 4);
			if (m_Hover)
			{
				fill = INT_PTY_MC.O_LIGHT;
				edge = INT_PTY_MC.O_LIGHT;
			}
		}
		else if (m_Style == OL)
		{
			edge = INT_PTY_MC.O;
			tc = INT_PTY_MC.O;
			if (m_Hover)
			{
				fill = INT_PTY_MC.O;
				tc = INT_PTY_MC.INK;
			}
		}
		else if (m_Style == DG)
		{
			edge = INT_PTY_MC.DG_EDGE;
			tc = INT_PTY_MC.R_TEXT;
			if (m_Hover)
			{
				fill = ARGB(46, 255, 75, 62);
				edge = INT_PTY_MC.R;
			}
		}
		else if (m_Style == DGF)
		{
			fill = INT_PTY_MC.R;
			edge = INT_PTY_MC.R;
			tc = INT_PTY_MC.INK_RED;
			keyEdge = ARGB(128, 28, 5, 3);
			if (m_Hover)
			{
				fill = ARGB(255, 255, 110, 98);
				edge = fill;
			}
		}
		else if (m_Style == DIS)
		{
			edge = INT_PTY_MC.L;
			tc = INT_PTY_MC.D;
		}
		else if (m_Style == GH)
		{
			tc = INT_PTY_MC.M;
			if (m_Hover)
				tc = INT_PTY_MC.T;
		}
		else
		{
			edge = INT_PTY_MC.BTN_EDGE;
			if (m_Hover)
				edge = ARGB(150, 255, 255, 255);
		}
		if (m_Box)
		{
			if (animate)
			{
				m_Box.FadeFill(fill, 0.12);
				m_Box.FadeBorder(edge, 0.12);
			}
			else
			{
				m_Box.SetFill(fill);
				m_Box.SetBorder(edge);
			}
		}
		SetColorOf(m_Text, tc, animate);
		SetColorOf(m_Icon, tc, animate);
		SetColorOf(m_KeyText, tc, animate);
		if (m_KeyBox)
			m_KeyBox.SetBorder(keyEdge);
	}

	protected void SetColorOf(Widget w, int c, bool animate)
	{
		if (!w)
			return;
		if (animate)
			INT_UIF_Animator.ColorTo(w, c, 0.12, INT_UIF_Ease.OUT_CUBIC);
		else
			w.SetColor(c);
	}

	// Press feedback: a light flash of the fill, then back to the style.
	void Press()
	{
		if (!m_Box || !m_Box.m_Fill)
			return;
		m_Box.m_Fill.SetColor(INT_PTY_MC.O_PRESS);
		Paint(true);
	}

	void SetLabel(string label, float size = 14)
	{
		if (m_Text)
			m_Text.SetText(INT_PTY_Text.Caps(label));
	}

	void Show(bool show)
	{
		if (m_Root)
			m_Root.Show(show);
	}
}

// Pill toggle (.tg): 50x26, knob 18; the big search switch is 92x48 with a 38 knob.
class INT_PTY_MToggle : Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Track;
	ImageWidget m_Knob;
	float m_W;
	float m_H;
	float m_K;
	bool m_On;

	static INT_PTY_MToggle Create(Widget parent, float x, float y, float w, float h, bool on)
	{
		INT_PTY_MToggle t = new INT_PTY_MToggle();
		t.m_W = w;
		t.m_H = h;
		t.m_K = h - 8;
		t.m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "PtyToggle");
		t.m_Track = INT_UIF_Surface.Pill(t.m_Root, 0, 0, w, h, 0, INT_PTY_MC.L2);
		t.m_Knob = INT_UIF_Draw.Image(t.m_Root, 4, 4, t.m_K, t.m_K, "iuf_circle32", INT_PTY_MC.KNOB_OFF);
		t.Set(on, false);
		return t;
	}

	void Set(bool on, bool animate)
	{
		m_On = on;
		float kx = 4;
		int fill = 0;
		int edge = INT_PTY_MC.L2;
		int kc = INT_PTY_MC.KNOB_OFF;
		if (on)
		{
			kx = m_W - 4 - m_K;
			fill = INT_PTY_MC.O;
			edge = INT_PTY_MC.O;
			kc = 0xFFFFFFFF;
		}
		if (animate)
		{
			if (m_Knob)
			{
				INT_UIF_Animator.MoveX(m_Knob, INT_UIF_Draw.S(kx), 0.2, INT_UIF_Ease.OUT_CUBIC);
				INT_UIF_Animator.ColorTo(m_Knob, kc, 0.2, INT_UIF_Ease.OUT_CUBIC);
			}
			if (m_Track)
			{
				m_Track.FadeFill(fill, 0.2);
				m_Track.FadeBorder(edge, 0.2);
			}
			return;
		}
		if (m_Knob)
		{
			INT_UIF_Draw.Move(m_Knob, kx, 4);
			m_Knob.SetColor(kc);
		}
		if (m_Track)
		{
			m_Track.SetFill(fill);
			m_Track.SetBorder(edge);
		}
	}
}

// Hairline slider (.sl): 2 px track, orange fill, 12 px diamond knob, optional limit tick at the right end.
class INT_PTY_MSlider : Managed
{
	Widget m_Root;
	ImageWidget m_Track;
	ImageWidget m_Fill;
	ImageWidget m_Knob;
	float m_W;
	float m_Frac;

	static INT_PTY_MSlider Create(Widget parent, float x, float y, float w, float frac, bool lim)
	{
		INT_PTY_MSlider s = new INT_PTY_MSlider();
		s.m_W = w;
		s.m_Root = INT_UIF_Draw.Frame(parent, x, y, w, 16, "PtySlider");
		s.m_Track = INT_PTY_MD.Line(s.m_Root, 0, 7, w, 2, 0x26FFFFFF);
		s.m_Fill = INT_PTY_MD.Line(s.m_Root, 0, 7, 1, 2, INT_PTY_MC.O);
		if (lim)
			INT_PTY_MD.Line(s.m_Root, w - 1, 2, 1, 12, INT_PTY_MC.L2);
		s.m_Knob = INT_PTY_MD.Dm(s.m_Root, 0, 8, 12, INT_PTY_MC.O);
		s.Set(frac, false);
		return s;
	}

	void Set(float frac, bool animate)
	{
		m_Frac = Math.Clamp(frac, 0, 1);
		float fw = m_W * m_Frac;
		float side = 12 * INT_PTY_Look.SQRT2;
		float kx = fw - side * 0.5;
		if (animate)
		{
			INT_UIF_Animator.ResizeW(m_Fill, INT_UIF_Draw.S(fw), 0.16, INT_UIF_Ease.OUT_CUBIC);
			INT_UIF_Animator.MoveX(m_Knob, INT_UIF_Draw.S(kx), 0.16, INT_UIF_Ease.OUT_CUBIC);
			return;
		}
		INT_UIF_Draw.Resize(m_Fill, fw, 2);
		INT_UIF_Draw.Move(m_Knob, kx, 8 - side * 0.5);
	}

	// Value 0..1 under the mouse x (screen px).
	float FracAt(int mx)
	{
		if (!m_Track)
			return m_Frac;
		float sx;
		float sy;
		float sw;
		float sh;
		m_Track.GetScreenPos(sx, sy);
		m_Track.GetScreenSize(sw, sh);
		if (sw <= 1)
			return m_Frac;
		return Math.Clamp((mx - sx) / sw, 0, 1);
	}
}

// Segmented choice (.seg): labels side by side, hairline separators, active = white + orange underline.
class INT_PTY_MSeg : Managed
{
	Widget m_Root;
	ref array<TextWidget> m_Texts;
	ref array<float> m_X;
	ref array<float> m_Wd;
	ImageWidget m_Ul;
	int m_Sel;
	float m_Total;

	static INT_PTY_MSeg Create(Widget parent, float x, float y, float w, array<string> labels, int sel, float size = 14)
	{
		INT_PTY_MSeg s = new INT_PTY_MSeg();
		s.m_Texts = new array<TextWidget>();
		s.m_X = new array<float>();
		s.m_Wd = new array<float>();
		s.m_Total = w;
		s.m_Root = INT_UIF_Draw.Frame(parent, x, y, w, 38, "PtySeg");
		int n = labels.Count();
		array<float> bases = new array<float>();
		float sum = 0;
		for (int i = 0; i < n; i++)
		{
			float tw = INT_PTY_MD.Measure(parent, labels.Get(i), size) + 18;
			bases.Insert(tw);
			sum = sum + tw;
		}
		float extra = 0;
		if (n > 0)
			extra = (w - sum) / n;
		float k = 1;
		if (extra < 0)
		{
			extra = 0;
			if (sum > 0)
				k = w / sum;
		}
		float cx = 0;
		for (int j = 0; j < n; j++)
		{
			float cw = bases.Get(j) * k + extra;
			s.m_X.Insert(cx);
			s.m_Wd.Insert(cw);
			if (j > 0)
				INT_PTY_MD.Line(s.m_Root, cx, 10, 1, 16, INT_PTY_MC.L);
			TextWidget t = INT_PTY_MD.Txt(s.m_Root, cx, 0, cw, 36, labels.Get(j), size, INT_PTY_MC.M, INT_UIF_Align.CENTER);
			s.m_Texts.Insert(t);
			cx = cx + cw;
		}
		INT_PTY_MD.Line(s.m_Root, 0, 37, w, 1, INT_PTY_MC.L);
		s.m_Ul = INT_PTY_MD.Line(s.m_Root, 0, 36, 10, 2, INT_PTY_MC.O);
		s.m_Sel = -1;
		s.Select(sel, false);
		return s;
	}

	int Count()
	{
		return m_Texts.Count();
	}

	void Select(int i, bool animate)
	{
		if (i < 0 || i >= m_Texts.Count())
			return;
		m_Sel = i;
		for (int k = 0; k < m_Texts.Count(); k++)
		{
			int c = INT_PTY_MC.M;
			if (k == i)
				c = INT_PTY_MC.T;
			TextWidget t = m_Texts.Get(k);
			if (!t)
				continue;
			if (animate)
				INT_UIF_Animator.ColorTo(t, c, 0.18, INT_UIF_Ease.OUT_CUBIC);
			else
				t.SetColor(c);
		}
		float ux = m_X.Get(i) + m_Wd.Get(i) * 0.1;
		float uw = m_Wd.Get(i) * 0.8;
		if (!m_Ul)
			return;
		if (animate)
		{
			INT_UIF_Animator.MoveX(m_Ul, INT_UIF_Draw.S(ux), 0.25, INT_UIF_Ease.IN_OUT_CUBIC);
			INT_UIF_Animator.ResizeW(m_Ul, INT_UIF_Draw.S(uw), 0.25, INT_UIF_Ease.IN_OUT_CUBIC);
			return;
		}
		INT_UIF_Draw.Place(m_Ul, ux, 36, uw, 2);
	}
}
