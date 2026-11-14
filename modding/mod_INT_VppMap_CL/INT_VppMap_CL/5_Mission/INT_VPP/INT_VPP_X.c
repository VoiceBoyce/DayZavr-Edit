// ============================================================================
// VanillaPPMap (our VPP fork) - «ЭКСПЕДИЦИЯ» controls, ported 2026-11-12 from
// INT_Garage (the Garage has them in 4_World and 5_Mission; the map has no
// 4_World module, so they live here) on the INT_VPPX_Look kit
// (3_Game/INT_VPP/INT_VPP_XLook.c). Trader-generation controls:
//   INT_VPPX_Tabs      big numbered tab words + moving underline with a diamond
//   INT_VPPX_Sort      «CAPTION  value ⌄» + its popup list (dropdown)
//   INT_VPPX_Toggle    «LABEL  [■  ]» switch
//   INT_VPPX_Search    search glyph + edit box on a hairline
//   INT_VPPX_Button    primary orange plate / secondary outline
//   INT_VPPX_Stepper   [−] [ 2 ] [+] [ MAX ]
//   INT_VPPX_Confirm   full-width confirmation band with a preview hook
// Hangar-generation atoms (exp.css):
//   INT_VPPX_HTone     extra palette + tone helpers + sort helpers
//   INT_VPPX_HButton   action button: primary / danger / ghost, icon, keycap, bar
//   INT_VPPX_HBadge    outline chip: diamond + caps (+ percent and a 2 px bar)
//   INT_VPPX_HNote     plate with a tone edge: icon + caps title + wrapped text
//   INT_VPPX_HTip      hover tooltip (own sort 100) with a nub
//   INT_VPPX_HLines    a few one-line TextWidgets wrapped by the kit's measure
//   INT_VPPX_HSlider   hairline track + orange diamond thumb, ticks, labels, ghost
//   INT_VPPX_HSegment  N-cell segment «◇ A │ ◆ B │ ◇ C» (read-only or clickable)
//   INT_VPPX_HInput    text field: kit icon, edit box on an underline, Enter submits
//   INT_VPPX_Check     checkbox (exp.css .ck) - NEW in the map kit, no Garage original
// Each control is its own event handler; sizes are design px (1920x1080).
// Display only: values come in, invokers go out, every decision stays in the
// caller. No sounds here: the window plays INT_VPPX_Sfx from the invokers.
// Internal calls are qualified (this.X / INT_VPPX_Look.X): a bare call binds
// to a class of that name in another mod.
// API reference: mod_intenso_VppMap/design/concepts_expedition/KIT_API.md
// ============================================================================

// A "tone" parameter of the atoms is either a badge tone (INT_VPPX_HBadge.TONE_*,
// 0..4) or a plain ARGB colour (anything else: opaque colours are negative ints).
class INT_VPPX_HTone
{
	static const int AMBER = 0xFFFFC246;
	static const int RED2 = 0xFFFF6D61;
	static const int T2 = 0xFFD9D3CA;
	static const int BODY2 = 0xFFA9A39B;
	static const int TILE = 0xFF111110;
	static const int TILE_HOV = 0xFF161615;
	static const int PANEL2 = 0xFF0E0D0C;
	// additions (exp.css): danger red, badge text tints, unlit cell
	static const int DANGER = 0xFFFF4B3E;
	static const int GREEN = 0xFF56E07E;
	static const int ORANGE_TEXT = 0xFFFF9A5E;
	static const int RUIN = 0xFFB55A52;
	static const int CELL_OFF = 0xFF2A2927;   // rgba(255,255,255,.12) pre-mixed (low alpha reads ~3x stronger)
	static const int TIP_BG = 0xFF161514;
	static const int TIP_EDGE = 0x26FFFFFF;

	static bool IsTone(int tone)
	{
		if (tone >= 0 && tone <= 4)
			return true;
		return false;
	}

	// Text / title colour of a tone.
	static int ToneText(int tone)
	{
		if (!INT_VPPX_HTone.IsTone(tone))
			return tone;
		if (tone == INT_VPPX_HBadge.TONE_AMBER)
			return AMBER;
		if (tone == INT_VPPX_HBadge.TONE_ORANGE)
			return ORANGE_TEXT;
		if (tone == INT_VPPX_HBadge.TONE_GREEN)
			return GREEN;
		if (tone == INT_VPPX_HBadge.TONE_RUIN)
			return RUIN;
		return RED2;
	}

	// Solid edge colour of a tone (note edge, tooltip top line, hover outline).
	static int ToneEdge(int tone)
	{
		if (!INT_VPPX_HTone.IsTone(tone))
			return tone;
		if (tone == INT_VPPX_HBadge.TONE_AMBER)
			return AMBER;
		if (tone == INT_VPPX_HBadge.TONE_ORANGE)
			return INT_VPPX_Look.ORANGE;
		if (tone == INT_VPPX_HBadge.TONE_GREEN)
			return GREEN;
		return DANGER;
	}

	// 1 px badge outline of a tone (exp.css .bdg.*).
	static int ToneBorder(int tone)
	{
		if (!INT_VPPX_HTone.IsTone(tone))
			return tone;
		if (tone == INT_VPPX_HBadge.TONE_AMBER)
			return 0x8CFFC246;
		if (tone == INT_VPPX_HBadge.TONE_ORANGE)
			return 0x99FF7A2F;
		if (tone == INT_VPPX_HBadge.TONE_GREEN)
			return 0x8056E07E;
		if (tone == INT_VPPX_HBadge.TONE_RUIN)
			return 0x47FF4B3E;
		return 0x8CFF4B3E;
	}

	// Hover wash of a badge (exp.css .bdg.hov, a third of the CSS alpha).
	static int ToneWash(int tone)
	{
		if (tone == INT_VPPX_HBadge.TONE_AMBER)
			return 0x0AFFC246;
		if (tone == INT_VPPX_HBadge.TONE_RED)
			return 0x0AFF4B3E;
		return 0x06FFFFFF;
	}

	// Caps title + icon of a note / tooltip (exp.css .tip .tt: orange, amber, red2).
	static int ToneTitle(int tone)
	{
		if (!INT_VPPX_HTone.IsTone(tone))
			return tone;
		if (tone == INT_VPPX_HBadge.TONE_AMBER)
			return AMBER;
		if (tone == INT_VPPX_HBadge.TONE_ORANGE)
			return INT_VPPX_Look.ORANGE;
		if (tone == INT_VPPX_HBadge.TONE_GREEN)
			return GREEN;
		return RED2;
	}

	// Accent paragraph of a note / tooltip (the text after a line break).
	static int ToneAccent(int tone)
	{
		return INT_VPPX_HTone.ToneTitle(tone);
	}

	// "body\naccent" -> body / accent (no line break: accent = "").
	static string SplitBody(string text, out string accent)
	{
		accent = "";
		int at = text.IndexOf("\n");
		if (at < 0)
			return text;
		int n = text.Length();
		string body = text.Substring(0, at);
		string tail = text.Substring(at + 1, n - at - 1);
		body.TrimInPlace();
		tail.TrimInPlace();
		accent = tail;
		return body;
	}

	// SetSort on a list collected in creation order (SetSort moves a widget to the end of its siblings).
	static void SortAll(array<Widget> all, int sort)
	{
		if (!all)
			return;
		int n = all.Count();
		for (int i = 0; i < n; i++)
		{
			Widget w = all.Get(i);
			if (w)
				w.SetSort(sort);
		}
	}

	// Text + its faux-bold twin (kit Text(..., bold = true) at >= 16 px) into the list.
	static void AddText(array<Widget> all, TextWidget t)
	{
		if (!t)
			return;
		all.Insert(t);
		Widget twin = INT_VPPX_Look.Twin(t);
		if (twin)
			all.Insert(twin);
	}
}

class INT_VPPX_Tabs : ScriptedWidgetEventHandler
{
	static const float WORD_SIZE = 36;

	protected Widget m_Root;
	protected ref array<string> m_Labels;
	protected ref array<string> m_Pre;
	protected ref array<int> m_Counts;
	protected ref array<TextWidget> m_Word;
	protected ref array<TextWidget> m_Num;
	protected ref array<TextWidget> m_Badge;
	protected ref array<ButtonWidget> m_Hits;
	protected ref array<float> m_X;
	protected ref array<float> m_W;
	protected Widget m_Ul;
	protected ImageWidget m_UlLine;
	protected int m_Sel;
	protected float m_Left;
	protected float m_Top;
	protected ref ScriptInvoker m_OnChanged;

	void INT_VPPX_Tabs()
	{
		m_Labels = new array<string>();
		m_Pre = new array<string>();
		m_Counts = new array<int>();
		m_Word = new array<TextWidget>();
		m_Num = new array<TextWidget>();
		m_Badge = new array<TextWidget>();
		m_Hits = new array<ButtonWidget>();
		m_X = new array<float>();
		m_W = new array<float>();
		m_Sel = -1;
		m_OnChanged = new ScriptInvoker();
	}

	// (int index, INT_VPPX_Tabs tabs)
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	// The tabs live on a full-canvas frame in `parent`; (x, y) = top-left of the first tab.
	void Build(Widget parent, float x, float y)
	{
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, 1920, 1080, "VppxTabs");
		m_Left = x;
		m_Top = y;
	}

	// label: plain or #STR. `pre` = a caps prefix next to the number («АДМИН»); count < 0 = no badge.
	void AddTab(string label, int count = -1, string pre = "")
	{
		m_Labels.Insert(label);
		m_Pre.Insert(pre);
		m_Counts.Insert(count);
	}

	// Lays the added tabs out (gap 56, 30 once past maxRight) and selects tab 0.
	void Finish(float maxRight)
	{
		float x = m_Left;
		float gap = 56;
		int n = m_Labels.Count();
		if (n <= 0)
			return;
		for (int i = 0; i < n; i++)
		{
			string label = m_Labels.Get(i);
			float ww = INT_VPPX_Look.Measure(label, WORD_SIZE);
			float badgeRoom = 0;
			if (m_Counts.Get(i) >= 0)
				badgeRoom = INT_VPPX_Look.Measure("00", 15, true) + 10;
			float tw = ww + badgeRoom;
			m_X.Insert(x);
			m_W.Insert(ww);
			string pad = INT_VPPX_Text.Pad2(i + 1);
			string num = INT_VPPX_Text.Spaced(pad);
			string pre = m_Pre.Get(i);
			if (pre != "")
			{
				string preCaps = INT_VPPX_Text.Caps(pre);
				num = num + "   ·   " + preCaps;
			}
			TextWidget nt = INT_VPPX_Look.Text(m_Root, x, m_Top, 300, 18, num, 13, INT_VPPX_Look.DARK);
			m_Num.Insert(nt);
			float room = INT_VPPX_Look.Room(ww);
			TextWidget wt = INT_VPPX_Look.Text(m_Root, x - 2, m_Top + 16, room, 48, label, WORD_SIZE, INT_VPPX_Look.MUTED);
			m_Word.Insert(wt);
			TextWidget bt = null;
			if (m_Counts.Get(i) >= 0)
				bt = INT_VPPX_Look.Text(m_Root, x + ww + 8, m_Top + 18, 60, 20, "", 15, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, false);
			m_Badge.Insert(bt);
			ButtonWidget hit = INT_UIF_Draw.Hit(m_Root, x - 10, m_Top - 6, tw + 20, 74, this);
			m_Hits.Insert(hit);
			x = x + tw + gap;
			if (x > maxRight)
				gap = 30;
		}
		for (int c = 0; c < n; c++)
		{
			this.ApplyCount(c);
		}
		float ulX = m_X.Get(0);
		float ulW = m_W.Get(0);
		m_Ul = INT_UIF_Draw.Frame(m_Root, ulX, m_Top + 66, ulW, 12, "VppxTabUl");
		m_UlLine = INT_VPPX_Look.Fill(m_Ul, 0, 5, ulW, 2, INT_VPPX_Look.ORANGE);
		INT_VPPX_Look.Diamond(m_Ul, 0, 6, 10, true, INT_VPPX_Look.ORANGE);
		m_Sel = -1;
		this.Paint(0, true);
	}

	int GetSelected()
	{
		return m_Sel;
	}

	int Count()
	{
		return m_Labels.Count();
	}

	// Badge number of a tab (0 = empty badge).
	void SetCount(int index, int count)
	{
		if (index < 0 || index >= m_Counts.Count())
			return;
		m_Counts.Set(index, count);
		this.ApplyCount(index);
	}

	protected void ApplyCount(int index)
	{
		if (index < 0 || index >= m_Badge.Count())
			return;
		TextWidget b = m_Badge.Get(index);
		if (!b)
			return;
		int n = m_Counts.Get(index);
		if (n <= 0)
		{
			b.SetText("");
			return;
		}
		string t = INT_VPPX_Text.Pad2(n);
		b.SetText(t);
	}

	// notify: fire OnChanged; instant: no slide.
	void Select(int index, bool notify, bool instant)
	{
		if (index < 0 || index >= m_Labels.Count())
			return;
		bool changed = index != m_Sel;
		this.Paint(index, instant);
		if (notify && changed)
			m_OnChanged.Invoke(index, this);
	}

	protected void Paint(int index, bool instant)
	{
		int old = m_Sel;
		m_Sel = index;
		int n = m_Labels.Count();
		for (int i = 0; i < n; i++)
		{
			int wc = INT_VPPX_Look.MUTED;
			int nc = INT_VPPX_Look.DARK;
			if (i == index)
			{
				wc = INT_VPPX_Look.WHITE;
				nc = INT_VPPX_Look.ORANGE;
			}
			TextWidget w = m_Word.Get(i);
			TextWidget nw = m_Num.Get(i);
			if (instant || (i != index && i != old))
			{
				// a refused switch is painted back instantly: stop the colour
				// tweens of the refused tab first
				if (w)
				{
					INT_UIF_Animator.Get().Cancel(w);
					w.SetColor(wc);
				}
				if (nw)
				{
					INT_UIF_Animator.Get().Cancel(nw);
					nw.SetColor(nc);
				}
			}
			else
			{
				if (w)
					INT_UIF_Animator.ColorTo(w, wc, 0.25, INT_UIF_Ease.OUT_CUBIC);
				if (nw)
					INT_UIF_Animator.ColorTo(nw, nc, 0.25, INT_UIF_Ease.OUT_CUBIC);
			}
		}
		if (!m_Ul)
			return;
		float tx = m_X.Get(index);
		float tw = m_W.Get(index);
		if (instant)
		{
			INT_UIF_Animator.Get().Cancel(m_Ul);
			INT_UIF_Animator.Get().Cancel(m_UlLine);
			INT_UIF_Draw.Move(m_Ul, tx, m_Top + 66);
			INT_UIF_Draw.Resize(m_Ul, tw, 12);
			INT_UIF_Draw.Resize(m_UlLine, tw, 2);
			return;
		}
		float stx = INT_UIF_Draw.S(tx);
		float stw = INT_UIF_Draw.S(tw);
		INT_UIF_Animator.MoveX(m_Ul, stx, 0.3, INT_UIF_Ease.IN_OUT_CUBIC);
		INT_UIF_Animator.ResizeW(m_Ul, stw, 0.3, INT_UIF_Ease.IN_OUT_CUBIC);
		INT_UIF_Animator.ResizeW(m_UlLine, stw, 0.3, INT_UIF_Ease.IN_OUT_CUBIC);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		int i = m_Hits.Find(bw);
		if (i < 0 || button != MouseState.LEFT)
			return false;
		this.Select(i, true, false);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		int i = m_Hits.Find(bw);
		if (i < 0 || i == m_Sel)
			return false;
		TextWidget t = m_Word.Get(i);
		if (t)
			t.SetColor(INT_VPPX_Look.BODY);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		int i = m_Hits.Find(bw);
		if (i < 0 || i == m_Sel)
			return false;
		TextWidget t = m_Word.Get(i);
		if (t)
			t.SetColor(INT_VPPX_Look.MUTED);
		return false;
	}
}

// «СОРТИРОВКА  value ⌄» and its popup (a list under the value). Right edge at x + w.
// The menu calls Tick() every frame (unlinks a closed popup outside its own click).
class INT_VPPX_Sort : ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected Widget m_Host;           // where the popup goes (full-canvas overlay)
	protected TextWidget m_Value;
	protected ImageWidget m_Chev;
	protected ButtonWidget m_Hit;
	protected ref array<string> m_Options;
	protected int m_Sel;
	protected bool m_Enabled = true;
	protected float m_X;
	protected float m_Y;
	protected float m_W;
	// popup
	protected Widget m_Pop;
	protected ButtonWidget m_Catch;
	protected ref array<ButtonWidget> m_RowHits;
	protected ref array<ImageWidget> m_RowBg;
	protected ref ScriptInvoker m_OnSelected;

	protected static INT_VPPX_Sort s_Open;
	protected Widget m_DeadPop;      // closed popup, unlinked by Tick (not inside its own click)
	protected Widget m_DeadCatch;

	void INT_VPPX_Sort()
	{
		m_Options = new array<string>();
		m_RowHits = new array<ButtonWidget>();
		m_RowBg = new array<ImageWidget>();
		m_OnSelected = new ScriptInvoker();
	}

	void ~INT_VPPX_Sort()
	{
		if (s_Open == this)
			s_Open = null;
		this.DropDead();
	}

	// Called every frame by the menu: the widgets of a closed popup go now.
	void Tick()
	{
		this.DropDead();
	}

	protected void DropDead()
	{
		if (m_DeadPop)
			m_DeadPop.Unlink();
		m_DeadPop = null;
		if (m_DeadCatch)
			m_DeadCatch.Unlink();
		m_DeadCatch = null;
	}

	// (int index, string text, INT_VPPX_Sort sort)
	ScriptInvoker OnSelected()
	{
		return m_OnSelected;
	}

	// options: plain or #STR. popupHost: a full-canvas overlay frame (design px = canvas px).
	// caption: plain or #STR shown as small caps before the value ("" = none).
	void Build(Widget parent, Widget popupHost, float x, float y, float w, array<string> options, int selected, string caption = "")
	{
		m_Host = popupHost;
		m_X = x;
		m_Y = y;
		m_W = w;
		m_Options.Clear();
		if (options)
		{
			foreach (string o : options)
			{
				m_Options.Insert(o);
			}
		}
		m_Sel = selected;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, 24, "VppxSort");
		float vx = 0;
		if (caption != "")
		{
			float cw = INT_VPPX_Look.CapsMeasure(caption, 11, true);
			INT_VPPX_Look.Caps(m_Root, 0, 2, cw + 20, 20, caption, 11, INT_VPPX_Look.MUTED, INT_UIF_Align.LEFT, false);
			vx = cw + 14;
		}
		m_Value = INT_VPPX_Look.Text(m_Root, vx, 0, w - vx - 22, 24, "", 16, INT_VPPX_Look.TEXT, INT_UIF_Align.LEFT, true);
		m_Chev = INT_VPPX_Look.IconAt(m_Root, w - 16, 5, 14, "chevron", INT_VPPX_Look.MUTED);
		// the chevron mask points right: down = 90 (square widget: rotation is in normalised space)
		if (m_Chev)
			m_Chev.SetRotation(0, 0, 90);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, -4, w, 32, this);
		this.ApplyValue();
	}

	protected void ApplyValue()
	{
		string t = "";
		if (m_Sel >= 0 && m_Sel < m_Options.Count())
		{
			string opt = m_Options.Get(m_Sel);
			t = INT_VPPX_Text.Tr(opt);
		}
		INT_VPPX_Look.SetText(m_Value, t);
	}

	int GetSelected()
	{
		return m_Sel;
	}

	void SetSelected(int index, bool notify)
	{
		if (index < 0 || index >= m_Options.Count())
			return;
		m_Sel = index;
		this.ApplyValue();
		if (notify)
		{
			string opt = m_Options.Get(index);
			string text = INT_VPPX_Text.Tr(opt);
			m_OnSelected.Invoke(index, text, this);
		}
	}

	// SetAlpha replaces a colour's own alpha (hairlines would turn opaque), so a
	// disabled control is hidden, not dimmed.
	void SetEnabled(bool on)
	{
		m_Enabled = on;
		if (m_Root)
			m_Root.Show(on);
		if (!on)
			this.ClosePopup();
	}

	bool IsPopupOpen()
	{
		return m_Pop != null;
	}

	// Esc of the menu: closes the open popup of any sort control; true = one was open.
	static bool CloseActive()
	{
		if (!s_Open)
			return false;
		if (!s_Open.IsPopupOpen())
		{
			s_Open = null;
			return false;
		}
		s_Open.ClosePopup();
		return true;
	}

	void OpenPopup()
	{
		if (m_Pop || !m_Host || !m_Enabled || !m_Root)
			return;
		if (s_Open && s_Open != this)
			s_Open.ClosePopup();
		s_Open = this;
		this.DropDead();
		// past the canvas: a click on the 4:3 / 21:9 strips closes it too
		m_Catch = INT_UIF_Draw.Hit(m_Host, -1200, -800, 4320, 2680, this);
		float rowH = 38;
		float pw = m_W + 40;
		float ph = rowH * m_Options.Count() + 12;
		float px = m_X + m_W - pw;
		float py = m_Y + 32;
		float ax;
		float ay;
		// popup coordinates are canvas design px: the sort frame sits in a parent at its own offset
		Widget par = m_Root.GetParent();
		if (par)
		{
			par.GetScreenPos(ax, ay);
			float cx;
			float cy;
			m_Host.GetScreenPos(cx, cy);
			float f = INT_UIF_Scale.Factor();
			if (f > 0)
			{
				px = px + (ax - cx) / f;
				py = py + (ay - cy) / f;
			}
		}
		m_Pop = INT_UIF_Draw.Frame(m_Host, px, py, pw, ph, "VppxSortPop");
		INT_VPPX_Look.Fill(m_Pop, 0, 0, pw, ph, INT_VPPX_Look.POPUP);
		INT_VPPX_Look.Outline(m_Pop, 0, 0, pw, ph, INT_VPPX_Look.LINE_STRONG);
		INT_VPPX_Look.Fill(m_Pop, 0, 0, pw, 2, INT_VPPX_Look.ORANGE);
		m_RowHits.Clear();
		m_RowBg.Clear();
		int n = m_Options.Count();
		for (int i = 0; i < n; i++)
		{
			float ry = 6 + i * rowH;
			ImageWidget bg = INT_VPPX_Look.Fill(m_Pop, 1, ry, pw - 2, rowH, INT_VPPX_Look.ORANGE_WASH);
			bool isSel = i == m_Sel;
			if (bg)
				bg.Show(isSel);
			m_RowBg.Insert(bg);
			int col = INT_VPPX_Look.BODY;
			float dy = ry + rowH * 0.5;
			if (isSel)
			{
				col = INT_VPPX_Look.WHITE;
				INT_VPPX_Look.Diamond(m_Pop, 18, dy, 8, true, INT_VPPX_Look.ORANGE);
			}
			else
			{
				INT_VPPX_Look.Diamond(m_Pop, 18, dy, 8, false, INT_VPPX_Look.DARK);
			}
			string opt = m_Options.Get(i);
			string label = INT_VPPX_Text.Tr(opt);
			INT_VPPX_Look.Text(m_Pop, 34, ry, pw - 44, rowH, label, 15, col);
			ButtonWidget h = INT_UIF_Draw.Hit(m_Pop, 0, ry, pw, rowH, this);
			m_RowHits.Insert(h);
		}
		INT_VPPX_Fx.BlockIn(m_Pop, 0, -8, 0, 0.18);
		if (m_Chev)
			m_Chev.SetColor(INT_VPPX_Look.ORANGE);
	}

	void ClosePopup()
	{
		// hidden now, unlinked on the next frame: this may run inside the
		// popup's own OnClick
		this.DropDead();
		if (m_Catch)
			m_Catch.Show(false);
		m_DeadCatch = m_Catch;
		m_Catch = null;
		if (m_Pop)
			m_Pop.Show(false);
		m_DeadPop = m_Pop;
		m_Pop = null;
		m_RowHits.Clear();
		m_RowBg.Clear();
		if (m_Chev)
			m_Chev.SetColor(INT_VPPX_Look.MUTED);
		if (s_Open == this)
			s_Open = null;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_Hit)
		{
			if (m_Pop)
				this.ClosePopup();
			else
				this.OpenPopup();
			return true;
		}
		if (w == m_Catch)
		{
			this.ClosePopup();
			return true;
		}
		ButtonWidget bw = ButtonWidget.Cast(w);
		int i = m_RowHits.Find(bw);
		if (i >= 0)
		{
			this.ClosePopup();
			this.SetSelected(i, true);
			return true;
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		int i = m_RowHits.Find(bw);
		if (i >= 0 && i < m_RowBg.Count())
		{
			ImageWidget bg = m_RowBg.Get(i);
			if (bg)
				bg.Show(true);
		}
		if (w == m_Hit && m_Value)
			INT_VPPX_Look.SetColor(m_Value, INT_VPPX_Look.WHITE);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		int i = m_RowHits.Find(bw);
		if (i >= 0 && i < m_RowBg.Count() && i != m_Sel)
		{
			ImageWidget bg = m_RowBg.Get(i);
			if (bg)
				bg.Show(false);
		}
		if (w == m_Hit && m_Value)
			INT_VPPX_Look.SetColor(m_Value, INT_VPPX_Look.TEXT);
		return false;
	}
}

// «LABEL  [■  ]» - right-aligned, ends at x + w.
class INT_VPPX_Toggle : ScriptedWidgetEventHandler
{
	static const float TRACK_W = 40;
	static const float TRACK_H = 20;

	protected Widget m_Root;
	protected ImageWidget m_TrackFill;
	protected Widget m_TrackEdge;
	protected ImageWidget m_Knob;
	protected TextWidget m_Label;
	protected ButtonWidget m_Hit;
	protected bool m_On;
	protected bool m_Enabled = true;
	protected float m_W;
	protected ref ScriptInvoker m_OnChanged;

	void INT_VPPX_Toggle()
	{
		m_OnChanged = new ScriptInvoker();
	}

	// (bool on, INT_VPPX_Toggle t)
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	// label: plain or #STR, drawn as bold small caps.
	void Build(Widget parent, float x, float y, float w, string label, bool on)
	{
		m_W = w;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, 24, "VppxToggle");
		float lw = INT_VPPX_Look.CapsMeasure(label, 11, true);
		float lx = w - TRACK_W - 14 - lw;
		m_Label = INT_VPPX_Look.Caps(m_Root, lx, 2, lw + 20, 20, label, 11, INT_VPPX_Look.TEXT, INT_UIF_Align.LEFT, true);
		float tx = w - TRACK_W;
		m_TrackFill = INT_VPPX_Look.Fill(m_Root, tx, 2, TRACK_W, TRACK_H, INT_VPPX_Look.ORANGE);
		m_TrackEdge = INT_UIF_Draw.Frame(m_Root, tx, 2, TRACK_W, TRACK_H, "Edge");
		INT_VPPX_Look.Outline(m_TrackEdge, 0, 0, TRACK_W, TRACK_H, INT_VPPX_Look.LINE_STRONG);
		m_Knob = INT_VPPX_Look.Fill(m_Root, tx + 3, 5, 14, 14, INT_VPPX_Look.MUTED);
		m_Hit = INT_UIF_Draw.Hit(m_Root, lx - 6, -4, w - lx + 6, 32, this);
		m_On = on;
		this.Paint(false);
	}

	bool IsOn()
	{
		return m_On;
	}

	void SetOn(bool on, bool notify, bool animate = true)
	{
		if (on == m_On)
			return;
		m_On = on;
		this.Paint(animate);
		if (notify)
			m_OnChanged.Invoke(on, this);
	}

	// Hidden while disabled (SetAlpha would turn the hairlines opaque).
	void SetEnabled(bool on)
	{
		m_Enabled = on;
		if (m_Root)
			m_Root.Show(on);
	}

	protected void Paint(bool animate)
	{
		if (!m_Knob || !m_TrackFill || !m_TrackEdge)
			return;
		float tx = m_W - TRACK_W;
		float kx = tx + 3;
		int kc = INT_VPPX_Look.MUTED;
		if (m_On)
		{
			kx = tx + TRACK_W - 17;
			kc = INT_VPPX_Look.INK;
		}
		m_TrackFill.Show(m_On);
		m_TrackEdge.Show(!m_On);
		m_Knob.SetColor(kc);
		if (animate)
		{
			float skx = INT_UIF_Draw.S(kx);
			INT_UIF_Animator.MoveX(m_Knob, skx, 0.16, INT_UIF_Ease.OUT_CUBIC);
		}
		else
		{
			INT_UIF_Draw.Move(m_Knob, kx, 5);
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		if (!m_Enabled)
			return true;
		this.SetOn(!m_On, true);
		return true;
	}
}

// Search: glyph + edit box on a hairline that turns orange while focused.
class INT_VPPX_Search : ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected EditBoxWidget m_Edit;
	protected TextWidget m_Ph;
	protected ImageWidget m_Line;
	protected ImageWidget m_Icon;
	protected ButtonWidget m_Hit;
	protected bool m_Focus;
	protected string m_Last;
	protected float m_W;
	protected ref ScriptInvoker m_OnChanged;

	void INT_VPPX_Search()
	{
		m_OnChanged = new ScriptInvoker();
	}

	// (string text, INT_VPPX_Search s) - on every edit (Enter arrives as OnChange too)
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	// placeholder: plain or #STR (e.g. "Поиск"). Height 40.
	void Build(Widget parent, float x, float y, float w, string placeholder)
	{
		m_W = w;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, 40, "VppxSearch");
		m_Icon = INT_VPPX_Look.IconAt(m_Root, 2, 11, 18, "search", INT_VPPX_Look.MUTED);
		m_Ph = INT_VPPX_Look.Text(m_Root, 34, 0, w - 40, 40, placeholder, 16, INT_VPPX_Look.FAINT);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, 40, this);
		m_Edit = INT_UIF_Draw.Edit(m_Root, 32, 0, w - 36, 40, 17);
		if (m_Edit)
		{
			m_Edit.SetHandler(this);
			m_Edit.SetColor(ARGB(255, 255, 255, 255));
			m_Edit.SetTextColor(INT_VPPX_Look.TEXT);
		}
		m_Line = INT_VPPX_Look.Fill(m_Root, 0, 39, w, 1, INT_VPPX_Look.LINE_STRONG);
		m_Last = "";
	}

	string GetText()
	{
		if (!m_Edit)
			return "";
		return m_Edit.GetText();
	}

	// Sets the text without firing OnChanged.
	void SetText(string text)
	{
		if (m_Edit)
			m_Edit.SetText(text);
		m_Last = text;
		this.UpdatePh();
	}

	bool IsFocused()
	{
		return m_Focus;
	}

	void Blur()
	{
		if (m_Focus)
			SetFocus(null);
		m_Focus = false;
		this.Paint();
	}

	protected void UpdatePh()
	{
		string t = this.GetText();
		bool show = false;
		if (t == "" && !m_Focus)
			show = true;
		if (m_Ph)
			m_Ph.Show(show);
	}

	protected void Paint()
	{
		int lc = INT_VPPX_Look.LINE_STRONG;
		int ic = INT_VPPX_Look.MUTED;
		if (m_Focus)
		{
			lc = INT_VPPX_Look.ORANGE;
			ic = INT_VPPX_Look.ORANGE;
		}
		if (m_Line)
			m_Line.SetColor(lc);
		if (m_Icon)
			m_Icon.SetColor(ic);
		this.UpdatePh();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_Hit && m_Edit)
		{
			SetFocus(m_Edit);
			m_Focus = true;
			this.Paint();
			return true;
		}
		return false;
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (w != m_Edit)
			return false;
		m_Focus = true;
		this.Paint();
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (w != m_Edit)
			return false;
		m_Focus = false;
		this.Paint();
		return false;
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w != m_Edit)
			return false;
		string t = m_Edit.GetText();
		if (t != m_Last)
		{
			m_Last = t;
			m_OnChanged.Invoke(t, this);
		}
		this.UpdatePh();
		return false;
	}
}

// Button: primary = orange plate with dark caps; secondary = 1 px outline.
// Disabled buttons keep their place, drawn faint.
class INT_VPPX_Button : ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ImageWidget m_Plate;
	protected ImageWidget m_T;
	protected ImageWidget m_B;
	protected ImageWidget m_L;
	protected ImageWidget m_R;
	protected TextWidget m_Text;
	protected ButtonWidget m_Hit;
	protected float m_W;
	protected float m_H;
	protected bool m_Primary;
	protected bool m_Enabled = true;
	protected bool m_Hover;
	protected string m_Label;
	protected ref ScriptInvoker m_OnClick;

	void INT_VPPX_Button()
	{
		m_OnClick = new ScriptInvoker();
	}

	// (INT_VPPX_Button b)
	ScriptInvoker OnClick()
	{
		return m_OnClick;
	}

	void Build(Widget parent, float x, float y, float w, float h, bool primary)
	{
		m_W = w;
		m_H = h;
		m_Primary = primary;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "VppxButton");
		m_Plate = INT_VPPX_Look.Fill(m_Root, 0, 0, w, h, INT_VPPX_Look.ORANGE);
		m_T = INT_VPPX_Look.Fill(m_Root, 0, 0, w, 1, INT_VPPX_Look.LINE_STRONG);
		m_B = INT_VPPX_Look.Fill(m_Root, 0, h - 1, w, 1, INT_VPPX_Look.LINE_STRONG);
		m_L = INT_VPPX_Look.Fill(m_Root, 0, 0, 1, h, INT_VPPX_Look.LINE_STRONG);
		m_R = INT_VPPX_Look.Fill(m_Root, w - 1, 0, 1, h, INT_VPPX_Look.LINE_STRONG);
		// faux bold (a twin): thin dark SDF strokes on the bright orange plate read
		// pale in game (linear blending), the heavier caps stay dark
		m_Text = INT_VPPX_Look.Text(m_Root, 0, 0, w, h, "", 16, INT_VPPX_Look.INK, INT_UIF_Align.CENTER, true);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, this);
		this.Paint();
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	// Plain text or #STR, shown as tracked capitals (fitted to the width, 15..10 px).
	void SetText(string text)
	{
		m_Label = text;
		if (!m_Text)
			return;
		string caps = INT_VPPX_Text.Caps(text);
		float size = 15;
		float room = m_W - 24;
		while (size > 10 && INT_VPPX_Look.Measure(caps, size, true) * 1.15 > room)
		{
			size = size - 1;
		}
		int px = INT_UIF_Scale.TextPx(size);
		m_Text.SetTextExactSize(px);
		TextWidget twin = TextWidget.Cast(INT_VPPX_Look.Twin(m_Text));
		if (twin)
			twin.SetTextExactSize(px);
		INT_VPPX_Look.SetText(m_Text, caps);
	}

	// The text given to SetText (untranslated).
	string GetLabel()
	{
		return m_Label;
	}

	void SetPrimary(bool primary)
	{
		m_Primary = primary;
		this.Paint();
	}

	void SetEnabled(bool on)
	{
		m_Enabled = on;
		this.Paint();
	}

	bool IsEnabled()
	{
		return m_Enabled;
	}

	void Show(bool show)
	{
		if (m_Root)
			m_Root.Show(show);
	}

	// Own sort over a 3D model (100) / back to 0 under a dialog.
	void SetSortAll(int sort)
	{
		if (!m_Root)
			return;
		m_Plate.SetSort(sort);
		m_T.SetSort(sort);
		m_B.SetSort(sort);
		m_L.SetSort(sort);
		m_R.SetSort(sort);
		m_Text.SetSort(sort);
		Widget tw = INT_VPPX_Look.Twin(m_Text);
		if (tw)
			tw.SetSort(sort);
		m_Hit.SetSort(sort);
	}

	protected void Paint()
	{
		if (!m_Plate || !m_T || !m_B || !m_L || !m_R)
			return;
		int plate = INT_VPPX_Look.ORANGE;
		int edge = INT_VPPX_Look.LINE_STRONG;
		int text = INT_VPPX_Look.INK;
		bool showPlate = m_Primary;
		if (m_Primary)
		{
			if (m_Hover && m_Enabled)
				plate = 0xFFFF9452;
			if (!m_Enabled)
			{
				plate = 0xFF2B2622;
				text = INT_VPPX_Look.FAINT;
			}
		}
		else
		{
			text = INT_VPPX_Look.TEXT;
			if (m_Hover && m_Enabled)
			{
				edge = INT_VPPX_Look.ORANGE;
				text = INT_VPPX_Look.WHITE;
			}
			if (!m_Enabled)
			{
				edge = INT_VPPX_Look.LINE;
				text = INT_VPPX_Look.FAINT;
			}
		}
		m_Plate.Show(showPlate);
		m_Plate.SetColor(plate);
		bool showEdge = !m_Primary;
		m_T.Show(showEdge);
		m_B.Show(showEdge);
		m_L.Show(showEdge);
		m_R.Show(showEdge);
		m_T.SetColor(edge);
		m_B.SetColor(edge);
		m_L.SetColor(edge);
		m_R.SetColor(edge);
		INT_VPPX_Look.SetColor(m_Text, text);
	}

	// Self-test / band: press without the mouse; false while disabled.
	bool Press()
	{
		if (!m_Enabled)
			return false;
		m_OnClick.Invoke(this);
		return true;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		this.Press();
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = true;
		this.Paint();
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = false;
		this.Paint();
		return false;
	}
}

// Quantity: [−] [ 2 ] [+] [ MAX ], wheel over it steps.
class INT_VPPX_Stepper : ScriptedWidgetEventHandler
{
	static const float BOX = 40;
	static const float MAX_W = 76;

	protected Widget m_Root;
	protected TextWidget m_Value;
	protected ButtonWidget m_MinusHit;
	protected ButtonWidget m_PlusHit;
	protected ButtonWidget m_MaxHit;
	protected ButtonWidget m_WheelHit;
	protected ImageWidget m_MinusBar;
	protected ImageWidget m_PlusH;
	protected ImageWidget m_PlusV;
	protected TextWidget m_MaxText;
	protected int m_Min = 1;
	protected int m_Max = 1;
	protected int m_Val = 1;
	protected bool m_Enabled = true;
	protected ref ScriptInvoker m_OnChanged;

	void INT_VPPX_Stepper()
	{
		m_OnChanged = new ScriptInvoker();
	}

	// (int value, INT_VPPX_Stepper s)
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	// maxLabel: plain or #STR on the «max» box; "" = no max box (width 136 instead of 212).
	void Build(Widget parent, float x, float y, string maxLabel = "MAX")
	{
		float w = BOX * 3 + 8;
		if (maxLabel != "")
			w = BOX * 3 + 16 + MAX_W;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, BOX, "VppxStepper");
		m_WheelHit = INT_UIF_Draw.Hit(m_Root, 0, 0, BOX * 3 + 8, BOX, this);
		// minus
		INT_VPPX_Look.Outline(m_Root, 0, 0, BOX, BOX, INT_VPPX_Look.LINE_STRONG);
		m_MinusBar = INT_VPPX_Look.Fill(m_Root, 13, 19, 14, 2, INT_VPPX_Look.TEXT);
		m_MinusHit = INT_UIF_Draw.Hit(m_Root, 0, 0, BOX, BOX, this);
		// value
		float vx = BOX + 4;
		INT_VPPX_Look.Fill(m_Root, vx, BOX - 2, BOX, 2, INT_VPPX_Look.LINE_STRONG);
		m_Value = INT_VPPX_Look.Text(m_Root, vx, 0, BOX, BOX, "1", 24, INT_VPPX_Look.WHITE, INT_UIF_Align.CENTER);
		// plus
		float px = BOX * 2 + 8;
		INT_VPPX_Look.Outline(m_Root, px, 0, BOX, BOX, INT_VPPX_Look.LINE_STRONG);
		m_PlusH = INT_VPPX_Look.Fill(m_Root, px + 13, 19, 14, 2, INT_VPPX_Look.TEXT);
		m_PlusV = INT_VPPX_Look.Fill(m_Root, px + 19, 13, 2, 14, INT_VPPX_Look.TEXT);
		m_PlusHit = INT_UIF_Draw.Hit(m_Root, px, 0, BOX, BOX, this);
		// max
		if (maxLabel != "")
		{
			float mx = BOX * 3 + 16;
			INT_VPPX_Look.Outline(m_Root, mx, 0, MAX_W, BOX, INT_VPPX_Look.LINE_STRONG);
			m_MaxText = INT_VPPX_Look.Caps(m_Root, mx, 0, MAX_W, BOX, maxLabel, 11, INT_VPPX_Look.TEXT, INT_UIF_Align.CENTER, true);
			m_MaxHit = INT_UIF_Draw.Hit(m_Root, mx, 0, MAX_W, BOX, this);
		}
		this.Paint();
	}

	int GetValue()
	{
		return m_Val;
	}

	int GetMax()
	{
		return m_Max;
	}

	// Clamps the value into [minValue, maxValue]; fires OnChanged if it moved.
	void SetRange(int minValue, int maxValue)
	{
		m_Min = minValue;
		int hi = maxValue;
		if (hi < minValue)
			hi = minValue;
		m_Max = hi;
		int v = m_Val;
		if (v < m_Min)
			v = m_Min;
		if (v > m_Max)
			v = m_Max;
		bool changed = v != m_Val;
		m_Val = v;
		this.Paint();
		if (changed)
			m_OnChanged.Invoke(m_Val, this);
	}

	void SetValue(int v, bool notify)
	{
		int c = v;
		if (c < m_Min)
			c = m_Min;
		if (c > m_Max)
			c = m_Max;
		bool changed = c != m_Val;
		m_Val = c;
		this.Paint();
		if (notify && changed)
			m_OnChanged.Invoke(m_Val, this);
	}

	void SetEnabled(bool on)
	{
		m_Enabled = on;
		this.Paint();
	}

	protected void Paint()
	{
		if (!m_Value || !m_MinusBar || !m_PlusH || !m_PlusV)
			return;
		m_Value.SetText(m_Val.ToString());
		int minusCol = INT_VPPX_Look.TEXT;
		int plusCol = INT_VPPX_Look.TEXT;
		int maxCol = INT_VPPX_Look.TEXT;
		if (!m_Enabled || m_Val <= m_Min)
			minusCol = INT_VPPX_Look.DARK;
		if (!m_Enabled || m_Val >= m_Max)
			plusCol = INT_VPPX_Look.DARK;
		if (!m_Enabled || m_Max <= m_Min)
			maxCol = INT_VPPX_Look.DARK;
		m_MinusBar.SetColor(minusCol);
		m_PlusH.SetColor(plusCol);
		m_PlusV.SetColor(plusCol);
		INT_VPPX_Look.SetColor(m_MaxText, maxCol);
		int vc = INT_VPPX_Look.WHITE;
		if (!m_Enabled)
			vc = INT_VPPX_Look.MUTED;
		m_Value.SetColor(vc);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!m_Enabled || button != MouseState.LEFT)
		{
			bool mine = false;
			if (w == m_MinusHit || w == m_PlusHit)
				mine = true;
			if (m_MaxHit && w == m_MaxHit)
				mine = true;
			return mine;
		}
		if (w == m_MinusHit)
		{
			this.SetValue(m_Val - 1, true);
			return true;
		}
		if (w == m_PlusHit)
		{
			this.SetValue(m_Val + 1, true);
			return true;
		}
		if (m_MaxHit && w == m_MaxHit)
		{
			this.SetValue(m_Max, true);
			return true;
		}
		return false;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (w != m_WheelHit && w != m_MinusHit && w != m_PlusHit)
			return false;
		if (m_Enabled)
			this.SetValue(m_Val + wheel, true);
		return true;
	}
}

// Confirmation band of the «ЭКСПЕДИЦИЯ» window (concept A frame «confirm»), generic
// form of INT_TraderXConfirm: a full-width band over the dimmed window - a preview
// frame on the left that the CALLER fills through OnPreview (one 3D model on screen:
// hide the window's own preview meanwhile), caption + name + kind chip + explanation
// in the middle, an optional big figure + caption/value rows and the two buttons on
// the right. The menu routes Enter to Confirm(), Esc to Cancel() and calls Tick(dt)
// every frame. Setters (SetKind / SetFigure / AddRow / SetAccent / SetPreviewCaption)
// prepare the NEXT Open and are cleared when the band closes.
class INT_VPPX_Confirm : ScriptedWidgetEventHandler
{
	static const int RESULT_NO = 0;
	static const int RESULT_YES = 1;
	static const float BAND_Y = 456;
	static const float BAND_H = 380;
	// preview clip inside the band, design px of the canvas
	static const float CLIP_X = 92;
	static const float CLIP_Y = 532;
	static const float CLIP_W = 576;
	static const float CLIP_H = 232;
	// own sort of 2D drawn over a 3D preview (INT_Trader measurement: the line is 100)
	static const int SORT_OVER_MODEL = 100;
	// texts of the band (RU constants for now; plain or #STR both work)
	static const string INT_TXT_NO = "Нет";
	static const string INT_TXT_PREVIEW_NONE = "Нет предпросмотра";

	protected Widget m_Root;
	protected ButtonWidget m_Catch;
	protected Widget m_Clip;
	protected TextWidget m_Note;
	protected ref INT_VPPX_Button m_Yes;
	protected ref INT_VPPX_Button m_No;
	protected ref array<Widget> m_Over;
	protected ref array<string> m_RowCaps;
	protected ref array<string> m_RowValues;
	protected ref array<int> m_RowColors;
	protected bool m_HasFigure;
	protected string m_FigureCap;
	protected string m_FigureText;
	protected int m_FigureColor;
	protected string m_Kind;
	protected string m_PreviewCap;
	protected string m_NoteText;
	protected int m_Accent;
	protected string m_YesKey;
	protected string m_NoKey;
	protected bool m_Open;
	protected bool m_Done;
	protected Widget m_Dead;   // the closed band, unlinked by Tick / the next Open (not inside its own click)
	protected ref ScriptInvoker m_OnResult;
	protected ref ScriptInvoker m_OnPreview;
	protected ref ScriptInvoker m_OnTick;

	void INT_VPPX_Confirm()
	{
		m_Over = new array<Widget>();
		m_RowCaps = new array<string>();
		m_RowValues = new array<string>();
		m_RowColors = new array<int>();
		m_OnResult = new ScriptInvoker();
		m_OnPreview = new ScriptInvoker();
		m_OnTick = new ScriptInvoker();
		this.ResetSetup();
	}

	void ~INT_VPPX_Confirm()
	{
		if (m_Dead)
			m_Dead.Unlink();
	}

	// (int result, INT_VPPX_Confirm band) - RESULT_YES / RESULT_NO; the band is already
	// hidden: clear your preview here.
	ScriptInvoker OnResult()
	{
		return m_OnResult;
	}

	// (Widget clip, INT_VPPX_Confirm band) - once per Open, after the band is drawn: build
	// the preview (ItemPreviewWidget etc.) inside `clip` (clipchildren frame, CLIP_W x CLIP_H
	// design px). Nothing built (clip without children) = INT_TXT_PREVIEW_NONE shown.
	ScriptInvoker OnPreview()
	{
		return m_OnPreview;
	}

	// (float dt, INT_VPPX_Confirm band) - every Tick while open (spin the preview).
	ScriptInvoker OnTick()
	{
		return m_OnTick;
	}

	bool IsOpen()
	{
		return m_Open;
	}

	// The preview clip of the open band (null when closed).
	Widget GetPreviewClip()
	{
		return m_Clip;
	}

	// --- setup of the next Open ---------------------------------------------------
	// Small caps caption over the preview frame ("" = none).
	void SetPreviewCaption(string caption)
	{
		m_PreviewCap = caption;
	}

	// Outlined chip under the name ("" = none).
	void SetKind(string kind)
	{
		m_Kind = kind;
	}

	// Colour of the warning glyph, the caption and the chip (default ORANGE; RED for a destructive action).
	void SetAccent(int color)
	{
		m_Accent = color;
	}

	// Big figure at the top right (64 px, shrinks to fit): caption + value (plain or #STR).
	void SetFigure(string caption, string value, int color)
	{
		m_HasFigure = true;
		m_FigureCap = caption;
		m_FigureText = value;
		m_FigureColor = color;
	}

	// Caption (left, small caps) / value (right, 20 px) rows under the figure; as many as
	// fit above the buttons (3 with a figure, 6 without).
	void AddRow(string caption, string value, int color)
	{
		m_RowCaps.Insert(caption);
		m_RowValues.Insert(value);
		m_RowColors.Insert(color);
	}

	protected void ResetSetup()
	{
		m_HasFigure = false;
		m_FigureCap = "";
		m_FigureText = "";
		m_FigureColor = INT_VPPX_Look.WHITE;
		m_Kind = "";
		m_PreviewCap = "";
		m_Accent = INT_VPPX_Look.ORANGE;
		m_RowCaps.Clear();
		m_RowValues.Clear();
		m_RowColors.Clear();
	}

	// Centred caps note in the preview frame (e.g. no model); "" hides it. Works while open.
	void SetPreviewNote(string text)
	{
		m_NoteText = text;
		if (!m_Note)
			return;
		INT_VPPX_Look.SetCaps(m_Note, text);
		bool show = false;
		if (text != "")
			show = true;
		m_Note.Show(show);
	}

	// host: a full-canvas overlay frame. caption / name / text / labels: plain or #STR.
	// yesLabel / noLabel are also the keys PressLabel() takes.
	void Open(Widget host, string caption, string name, string text, string yesLabel, string noLabel = "Нет")
	{
		if (m_Open || !host)
			return;
		if (m_Dead)
			m_Dead.Unlink();
		m_Dead = null;
		m_Open = true;
		m_Done = false;
		m_YesKey = yesLabel;
		m_NoKey = noLabel;
		m_NoteText = "";
		m_Over.Clear();
		m_Root = INT_UIF_Draw.Frame(host, 0, 0, 1920, 1080, "VppxConfirm");
		// the shade and the band reach past the canvas: the screen strips of a 4:3
		// letterbox / the sides of 21:9 are covered too
		int shade = ARGB(214, 5, 5, 5);
		INT_VPPX_Look.Fill(m_Root, -1200, -800, 4320, 2680, shade);
		m_Catch = INT_UIF_Draw.Hit(m_Root, -1200, -800, 4320, 2680, this);

		// band
		float by = BAND_Y;
		int glow = ARGB(16, 255, 122, 47);
		INT_VPPX_Look.Fill(m_Root, -1200, by, 4320, BAND_H, 0xFF0E0D0C);
		INT_VPPX_Look.Tex(m_Root, 60, by - 40, 900, BAND_H + 80, "int_vppx_glow_ca", glow);
		INT_VPPX_Look.Fill(m_Root, -1200, by, 4320, 1, 0x80FF7A2F);
		INT_VPPX_Look.Fill(m_Root, -1200, by + BAND_H - 1, 4320, 1, INT_VPPX_Look.LINE_STRONG);
		INT_VPPX_Look.Fill(m_Root, 1340, by + 30, 1, BAND_H - 60, INT_VPPX_Look.LINE);

		// preview frame (left); the caller fills the clip in OnPreview
		float px = 80;
		float py = by + 32;
		float pw = 600;
		float ph = BAND_H - 64;
		INT_VPPX_Look.Fill(m_Root, px, py, pw, ph, 0xFF11100F);
		INT_VPPX_Look.Tex(m_Root, px + 60, py + 40, pw - 120, ph, "int_vppx_glow_ca", 0x26FF7A2F);
		INT_VPPX_Look.Fill(m_Root, px + 50, py + ph - 46, pw - 100, 1, INT_VPPX_Look.LINE_STRONG);
		INT_VPPX_Look.Diamond(m_Root, px + pw * 0.5, py + ph - 46, 9, true, INT_VPPX_Look.ORANGE);
		INT_VPPX_Look.Ticks(m_Root, px, py, pw, ph, 18, INT_VPPX_Look.ORANGE);
		m_Clip = INT_UIF_Draw.Clip(m_Root, CLIP_X, CLIP_Y, CLIP_W, CLIP_H, "VppxConfirmClip");
		if (m_PreviewCap != "")
		{
			ImageWidget cd = INT_VPPX_Look.Diamond(m_Root, px + 24, py + 24, 7, true, INT_VPPX_Look.ORANGE);
			TextWidget ct = INT_VPPX_Look.Caps(m_Root, px + 38, py + 13, 300, 22, m_PreviewCap, 12, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, true);
			m_Over.Insert(cd);
			m_Over.Insert(ct);
		}
		m_Note = INT_VPPX_Look.Text(m_Root, px, py + ph * 0.5 - 10, pw, 20, "", 12, INT_VPPX_Look.FAINT, INT_UIF_Align.CENTER);
		if (m_Note)
		{
			m_Note.Show(false);
			m_Over.Insert(m_Note);
		}

		// middle: caption, name, kind chip, explanation
		float mx = 740;
		float mw = 560;
		INT_VPPX_Look.IconAt(m_Root, mx, by + 44, 16, "warning", m_Accent);
		INT_VPPX_Look.Caps(m_Root, mx + 26, by + 40, mw, 24, caption, 13, m_Accent, INT_UIF_Align.LEFT, true);
		string nm = INT_VPPX_Text.Tr(name);
		float ns = INT_VPPX_Look.FitSize(nm, 72, false, mw, 34);
		string nfit = INT_VPPX_Look.FitText(nm, ns, false, mw);
		INT_VPPX_Look.Text(m_Root, mx - 4, by + 70, mw + 40, 90, nfit, ns, INT_VPPX_Look.WHITE);
		float ty = by + 176;
		if (m_Kind != "")
		{
			string kcaps = INT_VPPX_Text.Caps(m_Kind);
			float kw = INT_VPPX_Look.SpacedWidth(kcaps, 11) + 20;
			INT_VPPX_Look.Outline(m_Root, mx, ty, kw, 26, m_Accent);
			INT_VPPX_Look.Text(m_Root, mx, ty, kw, 26, kcaps, 11, m_Accent, INT_UIF_Align.CENTER);
			ty = ty + 46;
		}
		array<string> lines = new array<string>();
		INT_VPPX_Look.Wrap(text, 17, mw, lines, 4);
		foreach (string l : lines)
		{
			INT_VPPX_Look.Text(m_Root, mx, ty, mw + 60, 28, l, 17, INT_VPPX_Look.BODY);
			ty = ty + 28;
		}

		// right: figure, rows, buttons
		float rx = 1364;
		float rw = 476;
		float bty = by + BAND_H - 32 - 58;
		float rowY = by + 40;
		if (m_HasFigure)
		{
			INT_VPPX_Look.Caps(m_Root, rx, by + 40, rw, 20, m_FigureCap, 12, INT_VPPX_Look.MUTED, INT_UIF_Align.LEFT, true);
			string fig = INT_VPPX_Text.Tr(m_FigureText);
			float fs = INT_VPPX_Look.FitSize(fig, 64, false, rw, 30);
			string ffit = INT_VPPX_Look.FitText(fig, fs, false, rw);
			INT_VPPX_Look.Text(m_Root, rx - 4, by + 60, rw + 40, 80, ffit, fs, m_FigureColor);
			INT_VPPX_Look.Fill(m_Root, rx, by + 156, rw, 1, INT_VPPX_Look.LINE);
			rowY = by + 174;
		}
		int rows = m_RowCaps.Count();
		float valueW = rw - 200;
		for (int r = 0; r < rows; r++)
		{
			if (rowY > bty - 30)
				break;
			string rc = m_RowCaps.Get(r);
			string rvRaw = m_RowValues.Get(r);
			string rv = INT_VPPX_Text.Tr(rvRaw);
			int rcol = m_RowColors.Get(r);
			INT_VPPX_Look.Caps(m_Root, rx, rowY, 260, 20, rc, 12, INT_VPPX_Look.MUTED, INT_UIF_Align.LEFT, true);
			string rfit = INT_VPPX_Look.FitText(rv, 20, false, valueW);
			INT_VPPX_Look.Text(m_Root, rx + 200, rowY - 6, valueW, 30, rfit, 20, rcol, INT_UIF_Align.RIGHT);
			rowY = rowY + 38;
		}

		m_Yes = new INT_VPPX_Button();
		m_Yes.Build(m_Root, rx, bty, 296, 58, true);
		m_Yes.SetText(yesLabel);
		m_Yes.OnClick().Insert(OnYes);
		m_No = new INT_VPPX_Button();
		m_No.Build(m_Root, rx + 310, bty, rw - 310, 58, false);
		m_No.SetText(noLabel);
		m_No.OnClick().Insert(OnNo);
		INT_VPPX_Look.Text(m_Root, rx, bty + 64, 296, 20, "ENTER", 11, INT_VPPX_Look.FAINT, INT_UIF_Align.CENTER);
		INT_VPPX_Look.Text(m_Root, rx + 310, bty + 64, rw - 310, 20, "ESC", 11, INT_VPPX_Look.FAINT, INT_UIF_Align.CENTER);

		foreach (Widget w : m_Over)
		{
			if (w)
				w.SetSort(SORT_OVER_MODEL);
		}

		// the caller's preview; it may close the band right away
		m_OnPreview.Invoke(m_Clip, this);
		if (!m_Open)
			return;
		if (m_Clip && !m_Clip.GetChildren() && m_NoteText == "")
			this.SetPreviewNote(INT_TXT_PREVIEW_NONE);
		INT_VPPX_Fx.FadeIn(m_Root, 0, 0.18);
	}

	// Self-test: press the button with this label key (yesLabel / noLabel of Open), like a click.
	bool PressLabel(string key)
	{
		if (!m_Open)
			return false;
		if (key == m_YesKey && m_Yes)
			return m_Yes.Press();
		if (key == m_NoKey && m_No)
			return m_No.Press();
		return false;
	}

	void Confirm()
	{
		this.Finish(RESULT_YES);
	}

	void Cancel()
	{
		this.Finish(RESULT_NO);
	}

	protected void Finish(int result)
	{
		if (!m_Open || m_Done)
			return;
		m_Done = true;
		m_Open = false;
		m_Over.Clear();
		if (m_Root)
			m_Root.Show(false);
		m_Dead = m_Root;
		m_Root = null;
		m_Catch = null;
		m_Clip = null;
		m_Note = null;
		// cleared before the result: a handler may set up and open the next band at once
		this.ResetSetup();
		m_OnResult.Invoke(result, this);
	}

	// A closed band is unlinked on the next frame; OnTick fires while open.
	void Tick(float dt)
	{
		if (!m_Open && m_Dead)
		{
			m_Dead.Unlink();
			m_Dead = null;
		}
		if (m_Open)
			m_OnTick.Invoke(dt, this);
	}

	void OnYes(INT_VPPX_Button b)
	{
		this.Confirm();
	}

	void OnNo(INT_VPPX_Button b)
	{
		this.Cancel();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		// a click on the dimmed window outside the band does nothing (no accidental cancel)
		bool caught = false;
		if (w == m_Catch)
			caught = true;
		return caught;
	}
}

// ----------------------------------------------------------------------------
// Action button (exp.css .btn): primary = orange plate + dark caps, danger = red
// plate, ghost = 1 px outline. h 54 main (15 px caps), 40 small (13 px caps);
// an empty text = icon only. [icon] LABEL [keycap], centred; a disabled button
// is an outline with dark caps (keycap hidden); SetProgress draws the 2 px
// cooldown bar along the bottom.
class INT_VPPX_HButton : ScriptedWidgetEventHandler
{
	static const int TONE_PRIMARY = 0;
	static const int TONE_DANGER = 1;
	static const int TONE_GHOST = 2;
	// faux-bold twin shift of the kit (size * 0.045 below 30 px)
	static const float TWIN_K = 0.045;
	// spare room either side of the centred label box (glyphs past a text box are cut)
	static const float TEXT_SPARE = 24;
	static const int INK_DANGER = 0xFF1C0604;
	static const int KEY_EDGE_PRIMARY = 0xFF813E17;   // rgba(26,12,4,.55) pre-mixed on orange
	static const int KEY_EDGE_DANGER = 0xFF82251E;    // rgba(28,6,4,.55) pre-mixed on red
	static const int HOVER_PRIMARY = 0xFFFF8C48;
	static const int HOVER_DANGER = 0xFFFF6A5E;

	protected Widget m_Root;
	// every widget keeps ONE colour alpha in all states: INT_VPPX_Fx.FadeIn restores the
	// alpha it first saw (SetAlpha replaces the colour's alpha), so the ghost hover wash
	// is its own widget and hidden edges keep the disabled hairline colour
	protected ImageWidget m_Plate;
	protected ImageWidget m_Wash;
	protected ImageWidget m_T;
	protected ImageWidget m_B;
	protected ImageWidget m_L;
	protected ImageWidget m_R;
	protected ImageWidget m_IconW;
	protected TextWidget m_Text;
	protected Widget m_TextTwin;
	protected Widget m_Key;
	protected ImageWidget m_KeyT;
	protected ImageWidget m_KeyB;
	protected ImageWidget m_KeyL;
	protected ImageWidget m_KeyR;
	protected TextWidget m_KeyText;
	protected ImageWidget m_Bar;
	protected ButtonWidget m_Hit;
	protected ref array<Widget> m_All;
	protected float m_W;
	protected float m_H;
	protected int m_Tone;
	protected bool m_Enabled = true;
	protected bool m_Hover;
	protected string m_Label;
	protected string m_IconName;
	protected string m_KeyLabel;
	protected float m_Progress = -1;
	protected ref ScriptInvoker m_OnClick;

	void INT_VPPX_HButton()
	{
		m_All = new array<Widget>();
		m_OnClick = new ScriptInvoker();
	}

	// (INT_VPPX_HButton b)
	ScriptInvoker OnClick()
	{
		return m_OnClick;
	}

	// h 54 = main action, 40 = small; any other height works (icon-only reset 34).
	void Build(Widget parent, float x, float y, float w, float h, int tone)
	{
		m_W = w;
		m_H = h;
		m_Tone = tone;
		m_All.Clear();
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "VppxHButton");
		if (!m_Root)
			return;
		m_All.Insert(m_Root);
		m_Plate = INT_VPPX_Look.Fill(m_Root, 0, 0, w, h, INT_VPPX_Look.ORANGE);
		m_All.Insert(m_Plate);
		m_Wash = INT_VPPX_Look.Fill(m_Root, 0, 0, w, h, INT_VPPX_Look.ORANGE_WASH);
		if (m_Wash)
		{
			m_Wash.Show(false);
			m_All.Insert(m_Wash);
		}
		m_T = INT_VPPX_Look.Fill(m_Root, 0, 0, w, 1, INT_VPPX_Look.LINE_STRONG);
		m_All.Insert(m_T);
		m_B = INT_VPPX_Look.Fill(m_Root, 0, h - 1, w, 1, INT_VPPX_Look.LINE_STRONG);
		m_All.Insert(m_B);
		m_L = INT_VPPX_Look.Fill(m_Root, 0, 0, 1, h, INT_VPPX_Look.LINE_STRONG);
		m_All.Insert(m_L);
		m_R = INT_VPPX_Look.Fill(m_Root, w - 1, 0, 1, h, INT_VPPX_Look.LINE_STRONG);
		m_All.Insert(m_R);
		m_IconW = INT_UIF_Draw.Icon(m_Root, 0, 0, 18, "check", INT_VPPX_Look.INK);
		if (m_IconW)
		{
			m_IconW.Show(false);
			m_All.Insert(m_IconW);
		}
		// faux bold at 16 (the kit makes the twin from 16 px), resized in INT_Layout
		m_Text = INT_VPPX_Look.Text(m_Root, 0, 0, w, h, "", 16, INT_VPPX_Look.INK, INT_UIF_Align.CENTER, true);
		m_TextTwin = INT_VPPX_Look.Twin(m_Text);
		INT_VPPX_HTone.AddText(m_All, m_Text);
		m_Key = INT_UIF_Draw.Frame(m_Root, 0, 0, 30, 24, "VppxHKey");
		if (m_Key)
		{
			m_All.Insert(m_Key);
			m_KeyT = INT_VPPX_Look.Fill(m_Key, 0, 0, 30, 1, KEY_EDGE_PRIMARY);
			m_All.Insert(m_KeyT);
			m_KeyB = INT_VPPX_Look.Fill(m_Key, 0, 23, 30, 1, KEY_EDGE_PRIMARY);
			m_All.Insert(m_KeyB);
			m_KeyL = INT_VPPX_Look.Fill(m_Key, 0, 0, 1, 24, KEY_EDGE_PRIMARY);
			m_All.Insert(m_KeyL);
			m_KeyR = INT_VPPX_Look.Fill(m_Key, 29, 0, 1, 24, KEY_EDGE_PRIMARY);
			m_All.Insert(m_KeyR);
			m_KeyText = INT_VPPX_Look.Text(m_Key, 0, 0, 30, 24, "", 12, INT_VPPX_Look.INK, INT_UIF_Align.CENTER, true);
			INT_VPPX_HTone.AddText(m_All, m_KeyText);
			m_Key.Show(false);
		}
		m_Bar = INT_VPPX_Look.Fill(m_Root, 0, h - 2, w, 2, INT_VPPX_Look.ORANGE);
		if (m_Bar)
		{
			m_Bar.Show(false);
			m_All.Insert(m_Bar);
		}
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, this);
		if (m_Hit)
			m_All.Insert(m_Hit);
		this.INT_Layout();
		this.INT_Paint();
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	// Plain text or #STR, drawn as tracked capitals ("" = icon only).
	void SetText(string textOrKey)
	{
		if (textOrKey == m_Label)
			return;
		m_Label = textOrKey;
		this.INT_Layout();
	}

	// The text given to SetText (untranslated).
	string GetLabel()
	{
		return m_Label;
	}

	// INT_UIF atlas icon name (logout, home, check, refresh, clock, minus, plus ...); "" = none.
	void SetIcon(string uifIcon)
	{
		if (uifIcon == m_IconName)
			return;
		m_IconName = uifIcon;
		if (m_IconW && uifIcon != "")
		{
			string cell = INT_UIF_SET + "iuf_icon_" + uifIcon;
			m_IconW.LoadImageFile(0, cell);
			m_IconW.SetImage(0);
		}
		this.INT_Layout();
		this.INT_Paint();
	}

	// Keycap after the label ("2×ЛКМ", "ENTER", "#STR_INT_UIF_KEY_DBLCLICK"); "" = none.
	void SetKey(string keycap)
	{
		if (keycap == m_KeyLabel)
			return;
		m_KeyLabel = keycap;
		this.INT_Layout();
		this.INT_Paint();
	}

	void SetTone(int tone)
	{
		if (tone == m_Tone)
			return;
		m_Tone = tone;
		this.INT_Paint();
	}

	void SetEnabled(bool on)
	{
		if (on == m_Enabled)
			return;
		m_Enabled = on;
		if (!on)
			m_Hover = false;
		// the keycap goes with the enabled state: the group is laid out again
		this.INT_Layout();
		this.INT_Paint();
	}

	bool IsEnabled()
	{
		return m_Enabled;
	}

	// 2 px cooldown bar along the bottom, k01 = share left; < 0 hides it.
	void SetProgress(float k01)
	{
		if (!m_Bar)
			return;
		if (k01 < 0)
		{
			m_Progress = -1;
			if (m_Bar.IsVisible())
				m_Bar.Show(false);
			return;
		}
		float k = Math.Clamp(k01, 0, 1);
		float dk = k - m_Progress;
		if (dk < 0.0005 && dk > -0.0005 && m_Bar.IsVisible())
			return;
		m_Progress = k;
		float bw = m_W * k;
		INT_UIF_Draw.Resize(m_Bar, bw, 2);
		if (!m_Bar.IsVisible())
			m_Bar.Show(true);
	}

	void Show(bool show)
	{
		if (!m_Root)
			return;
		if (m_Root.IsVisible() != show)
			m_Root.Show(show);
	}

	// Own sort of every widget (100 over the 3D model, 0 back under a band).
	void SetSortAll(int sort)
	{
		INT_VPPX_HTone.SortAll(m_All, sort);
	}

	// Self-test / band / Enter: a click without the mouse; false while disabled.
	bool Press()
	{
		if (!m_Enabled)
			return false;
		m_OnClick.Invoke(this);
		return true;
	}

	protected float INT_GroupW(bool hasIcon, float isz, bool hasText, float textW, bool hasKey, float keyW, float gap)
	{
		float total = 0;
		int parts = 0;
		if (hasIcon)
		{
			total = total + isz;
			parts++;
		}
		if (hasText)
		{
			total = total + textW;
			parts++;
		}
		if (hasKey)
		{
			total = total + keyW;
			parts++;
		}
		if (parts > 1)
			total = total + gap * (parts - 1);
		return total;
	}

	// Largest label size from `from` down to minSize that lets the group fit `room`.
	protected float INT_FitSize(string capsText, float from, float minSize, bool useIcon, float isz, bool useKey, float keyW, float gap, float room)
	{
		float s = from;
		float tw = INT_VPPX_Look.Measure(capsText, s, true);
		float total = this.INT_GroupW(useIcon, isz, true, tw, useKey, keyW, gap);
		while (total > room && s > minSize)
		{
			s = s - 1;
			tw = INT_VPPX_Look.Measure(capsText, s, true);
			total = this.INT_GroupW(useIcon, isz, true, tw, useKey, keyW, gap);
		}
		return s;
	}

	// [icon] LABEL [key] centred in the button. Too long: the label shrinks to 12 px,
	// then the keycap goes, then 11 px, then the icon goes, then 10 px, then the
	// label is cut (narrow admin / band buttons in long languages).
	protected void INT_Layout()
	{
		if (!m_Root)
			return;
		bool small = false;
		if (m_H < 48)
			small = true;
		float baseSize = 15;
		float isz = 18;
		float gap = 14;
		float padMin = 14;
		float keyFs = 12;
		float keyH = 24;
		if (small)
		{
			baseSize = 13;
			isz = 15;
			gap = 10;
			padMin = 10;
			keyFs = 11;
			keyH = 22;
		}
		string caps = "";
		if (m_Label != "")
			caps = INT_VPPX_Text.Caps(m_Label);
		bool hasText = false;
		if (caps != "")
			hasText = true;
		bool hasIcon = false;
		if (m_IconName != "" && m_IconW)
			hasIcon = true;
		if (hasIcon && !hasText && m_H < 38)
			isz = 16;
		bool hasKey = false;
		if (m_KeyLabel != "" && m_Enabled && m_Key)
			hasKey = true;
		string keyUp = "";
		float keyW = 0;
		if (hasKey)
		{
			keyUp = INT_VPPX_Text.Upper(m_KeyLabel);
			keyW = INT_VPPX_Look.Measure(keyUp, keyFs, true) + 16;
			if (keyW < 30)
				keyW = 30;
		}
		// the content is centred: the CSS padding (24 / 16) is kept whenever it fits
		float room = m_W - padMin * 2;
		float size = baseSize;
		float textW = 0;
		if (hasText)
		{
			size = this.INT_FitSize(caps, baseSize, 12, hasIcon, isz, hasKey, keyW, gap, room);
			textW = INT_VPPX_Look.Measure(caps, size, true);
		}
		float total = this.INT_GroupW(hasIcon, isz, hasText, textW, hasKey, keyW, gap);
		if (hasText && total > room && hasKey)
		{
			hasKey = false;
			size = this.INT_FitSize(caps, baseSize, 12, hasIcon, isz, hasKey, keyW, gap, room);
			textW = INT_VPPX_Look.Measure(caps, size, true);
			total = this.INT_GroupW(hasIcon, isz, hasText, textW, hasKey, keyW, gap);
		}
		if (hasText && total > room)
		{
			size = this.INT_FitSize(caps, size, 11, hasIcon, isz, hasKey, keyW, gap, room);
			textW = INT_VPPX_Look.Measure(caps, size, true);
			total = this.INT_GroupW(hasIcon, isz, hasText, textW, hasKey, keyW, gap);
		}
		if (hasText && total > room && hasIcon)
		{
			hasIcon = false;
			size = this.INT_FitSize(caps, baseSize, 10, hasIcon, isz, hasKey, keyW, gap, room);
			textW = INT_VPPX_Look.Measure(caps, size, true);
			total = this.INT_GroupW(hasIcon, isz, hasText, textW, hasKey, keyW, gap);
		}
		if (hasText && total > room)
		{
			size = this.INT_FitSize(caps, size, 10, hasIcon, isz, hasKey, keyW, gap, room);
			textW = INT_VPPX_Look.Measure(caps, size, true);
			total = this.INT_GroupW(hasIcon, isz, hasText, textW, hasKey, keyW, gap);
		}
		string shown = caps;
		if (hasText && total > room)
		{
			float textRoom = room - (total - textW);
			shown = INT_VPPX_Look.FitCaps(m_Label, size, true, textRoom);
			textW = INT_VPPX_Look.Measure(shown, size, true);
			total = this.INT_GroupW(hasIcon, isz, hasText, textW, hasKey, keyW, gap);
		}
		float x = (m_W - total) * 0.5;
		if (m_IconW)
		{
			if (hasIcon)
			{
				float iy = (m_H - isz) * 0.5;
				INT_UIF_Draw.Place(m_IconW, x, iy, isz, isz);
				x = x + isz + gap;
			}
			m_IconW.Show(hasIcon);
		}
		if (m_Text)
		{
			int px = INT_UIF_Scale.TextPx(size);
			m_Text.SetTextExactSize(px);
			TextWidget twin = TextWidget.Cast(m_TextTwin);
			if (twin)
				twin.SetTextExactSize(px);
			INT_VPPX_Look.SetText(m_Text, shown);
			// centred on the measured slot in a wider box: an estimate error splits both
			// ways and no glyph is cut at the box edge
			float boxX = x - TEXT_SPARE;
			float boxW = textW + TEXT_SPARE * 2;
			INT_UIF_Draw.Place(m_Text, boxX, 0, boxW, m_H);
			float twinDx = size * TWIN_K;
			if (m_TextTwin)
				INT_UIF_Draw.Place(m_TextTwin, boxX + twinDx, 0, boxW, m_H);
			INT_VPPX_Look.ShowText(m_Text, hasText);
			if (hasText)
				x = x + textW + gap;
		}
		if (m_Key)
		{
			if (hasKey)
			{
				float ky = (m_H - keyH) * 0.5;
				INT_UIF_Draw.Place(m_Key, x, ky, keyW, keyH);
				INT_UIF_Draw.Place(m_KeyT, 0, 0, keyW, 1);
				INT_UIF_Draw.Place(m_KeyB, 0, keyH - 1, keyW, 1);
				INT_UIF_Draw.Place(m_KeyL, 0, 0, 1, keyH);
				INT_UIF_Draw.Place(m_KeyR, keyW - 1, 0, 1, keyH);
				if (m_KeyText)
				{
					int kpx = INT_UIF_Scale.TextPx(keyFs);
					m_KeyText.SetTextExactSize(kpx);
					INT_UIF_Draw.Place(m_KeyText, 0, 0, keyW, keyH);
					m_KeyText.SetText(keyUp);
				}
			}
			m_Key.Show(hasKey);
		}
	}

	protected void INT_Paint()
	{
		if (!m_Plate || !m_T || !m_B || !m_L || !m_R)
			return;
		bool showPlate = false;
		bool showWash = false;
		bool showEdge = true;
		// opaque plate colours only (alpha FF in every state)
		int plate = INT_VPPX_Look.ORANGE;
		int edge = INT_VPPX_Look.LINE_STRONG;
		int ink = INT_VPPX_HTone.T2;
		int iconCol = INT_VPPX_HTone.T2;
		int keyEdge = 0x6BFFFFFF;
		int keyInk = INT_VPPX_Look.TEXT;
		if (m_Tone == TONE_DANGER)
			plate = INT_VPPX_HTone.DANGER;
		if (!m_Enabled)
		{
			edge = INT_VPPX_Look.LINE;
			ink = INT_VPPX_Look.DARK;
			iconCol = INT_VPPX_Look.DARK;
		}
		else if (m_Tone == TONE_PRIMARY)
		{
			showPlate = true;
			showEdge = false;
			edge = INT_VPPX_Look.LINE;
			if (m_Hover)
				plate = HOVER_PRIMARY;
			ink = INT_VPPX_Look.INK;
			iconCol = INT_VPPX_Look.INK;
			keyEdge = KEY_EDGE_PRIMARY;
			keyInk = INT_VPPX_Look.INK;
		}
		else if (m_Tone == TONE_DANGER)
		{
			showPlate = true;
			showEdge = false;
			edge = INT_VPPX_Look.LINE;
			if (m_Hover)
				plate = HOVER_DANGER;
			ink = INK_DANGER;
			iconCol = INK_DANGER;
			keyEdge = KEY_EDGE_DANGER;
			keyInk = INK_DANGER;
		}
		else
		{
			if (m_Hover)
			{
				showWash = true;
				edge = INT_VPPX_Look.ORANGE;
				ink = INT_VPPX_Look.WHITE;
				iconCol = INT_VPPX_Look.ORANGE;
			}
		}
		m_Plate.Show(showPlate);
		m_Plate.SetColor(plate);
		if (m_Wash)
			m_Wash.Show(showWash);
		m_T.Show(showEdge);
		m_B.Show(showEdge);
		m_L.Show(showEdge);
		m_R.Show(showEdge);
		m_T.SetColor(edge);
		m_B.SetColor(edge);
		m_L.SetColor(edge);
		m_R.SetColor(edge);
		INT_VPPX_Look.SetColor(m_Text, ink);
		if (m_IconW)
			m_IconW.SetColor(iconCol);
		if (m_KeyT)
		{
			m_KeyT.SetColor(keyEdge);
			m_KeyB.SetColor(keyEdge);
			m_KeyL.SetColor(keyEdge);
			m_KeyR.SetColor(keyEdge);
		}
		INT_VPPX_Look.SetColor(m_KeyText, keyInk);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		this.Press();
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_Hit)
			return false;
		bool hov = false;
		if (m_Enabled)
			hov = true;
		m_Hover = hov;
		this.INT_Paint();
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = false;
		this.INT_Paint();
		return false;
	}
}

// ----------------------------------------------------------------------------
// Outline chip (exp.css .bdg): h 24, diamond 6 + 12 px caps in the tone; busy
// adds «64 %» and a 2 px orange bar along the bottom. Set() shows the badge
// (key "" hides it); Show() overrides.
class INT_VPPX_HBadge : Managed
{
	static const int TONE_AMBER = 0;
	static const int TONE_ORANGE = 1;
	static const int TONE_RED = 2;
	static const int TONE_GREEN = 3;
	static const int TONE_RUIN = 4;
	static const float BADGE_H = 24;

	protected Widget m_Root;
	protected ImageWidget m_Wash;
	protected ImageWidget m_T;
	protected ImageWidget m_B;
	protected ImageWidget m_L;
	protected ImageWidget m_R;
	protected ImageWidget m_Dia;
	protected TextWidget m_Text;
	protected TextWidget m_Pct;
	protected ImageWidget m_Bar;
	protected ref array<Widget> m_All;
	protected int m_Tone = 3;   // TONE_GREEN
	protected float m_W = 40;
	protected bool m_Hover;
	protected string m_Key;
	protected float m_PctV = -1;

	void INT_VPPX_HBadge()
	{
		m_All = new array<Widget>();
	}

	void Build(Widget parent, float x, float y)
	{
		m_All.Clear();
		m_Root = INT_UIF_Draw.Frame(parent, x, y, 40, BADGE_H, "VppxHBadge");
		if (!m_Root)
			return;
		m_All.Insert(m_Root);
		m_Wash = INT_VPPX_Look.Fill(m_Root, 0, 0, 40, BADGE_H, 0x06FFFFFF);
		m_All.Insert(m_Wash);
		m_T = INT_VPPX_Look.Fill(m_Root, 0, 0, 40, 1, INT_VPPX_Look.LINE_STRONG);
		m_All.Insert(m_T);
		m_B = INT_VPPX_Look.Fill(m_Root, 0, BADGE_H - 1, 40, 1, INT_VPPX_Look.LINE_STRONG);
		m_All.Insert(m_B);
		m_L = INT_VPPX_Look.Fill(m_Root, 0, 0, 1, BADGE_H, INT_VPPX_Look.LINE_STRONG);
		m_All.Insert(m_L);
		m_R = INT_VPPX_Look.Fill(m_Root, 39, 0, 1, BADGE_H, INT_VPPX_Look.LINE_STRONG);
		m_All.Insert(m_R);
		m_Dia = INT_VPPX_Look.Diamond(m_Root, 12, 12, 6, true, INT_VPPX_Look.ORANGE);
		m_All.Insert(m_Dia);
		m_Text = INT_VPPX_Look.Text(m_Root, 23, 0, 100, BADGE_H, "", 12, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		INT_VPPX_HTone.AddText(m_All, m_Text);
		m_Pct = INT_VPPX_Look.Text(m_Root, 60, 0, 60, BADGE_H, "", 12, INT_VPPX_Look.WHITE);
		if (m_Pct)
		{
			m_Pct.Show(false);
			m_All.Insert(m_Pct);
		}
		m_Bar = INT_VPPX_Look.Fill(m_Root, 0, BADGE_H - 2, 10, 2, INT_VPPX_Look.ORANGE);
		if (m_Bar)
		{
			m_Bar.Show(false);
			m_All.Insert(m_Bar);
		}
		m_Root.Show(false);
		this.INT_Paint();
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	// key: plain or #STR; pct >= 0 adds «N %» and the 2 px bar (busy).
	void Set(string key, int tone, float pct = -1)
	{
		if (!m_Root)
			return;
		if (key == "")
		{
			m_Key = "";
			this.Show(false);
			return;
		}
		float dp = pct - m_PctV;
		bool same = false;
		if (key == m_Key && tone == m_Tone && dp < 0.05 && dp > -0.05)
			same = true;
		if (same)
		{
			// colours again: a fade of the tile may have left a stale alpha
			// (INT_VPPX_Fx restores the alpha of the first tone it saw)
			this.INT_Paint();
			this.Show(true);
			return;
		}
		m_Key = key;
		m_Tone = tone;
		m_PctV = pct;
		string caps = INT_VPPX_Text.Caps(key);
		float tw = INT_VPPX_Look.Measure(caps, 12, true);
		float w = 23 + tw + 9;
		bool hasPct = false;
		if (pct >= 0)
			hasPct = true;
		string pv = "";
		float pw = 0;
		if (hasPct)
		{
			int p = Math.Round(Math.Clamp(pct, 0, 100));
			pv = p.ToString() + " %";
			pw = INT_VPPX_Look.Measure(pv, 12, false);
			w = w + 8 + pw;
		}
		m_W = w;
		INT_UIF_Draw.Resize(m_Root, w, BADGE_H);
		INT_UIF_Draw.Place(m_Wash, 0, 0, w, BADGE_H);
		INT_UIF_Draw.Place(m_T, 0, 0, w, 1);
		INT_UIF_Draw.Place(m_B, 0, BADGE_H - 1, w, 1);
		INT_UIF_Draw.Place(m_L, 0, 0, 1, BADGE_H);
		INT_UIF_Draw.Place(m_R, w - 1, 0, 1, BADGE_H);
		if (m_Text)
		{
			INT_UIF_Draw.Place(m_Text, 23, 0, tw + 24, BADGE_H);
			m_Text.SetText(caps);
		}
		if (m_Pct)
		{
			if (hasPct)
			{
				float px = 23 + tw + 8;
				INT_UIF_Draw.Place(m_Pct, px, 0, pw + 24, BADGE_H);
				m_Pct.SetText(pv);
			}
			m_Pct.Show(hasPct);
		}
		if (m_Bar)
		{
			if (hasPct)
			{
				float k = Math.Clamp(pct, 0, 100) / 100;
				float bw = w * k;
				if (bw < 2)
					bw = 2;
				INT_UIF_Draw.Place(m_Bar, 0, BADGE_H - 2, bw, 2);
			}
			m_Bar.Show(hasPct);
		}
		this.INT_Paint();
		this.Show(true);
	}

	// Width of the chip as set (design px).
	float GetW()
	{
		return m_W;
	}

	void SetHover(bool hover)
	{
		if (hover == m_Hover)
			return;
		m_Hover = hover;
		this.INT_Paint();
	}

	void Show(bool show)
	{
		if (!m_Root)
			return;
		if (m_Root.IsVisible() != show)
			m_Root.Show(show);
	}

	void SetSortAll(int sort)
	{
		INT_VPPX_HTone.SortAll(m_All, sort);
	}

	protected void INT_Paint()
	{
		if (!m_T || !m_B || !m_L || !m_R)
			return;
		int txt = INT_VPPX_HTone.ToneText(m_Tone);
		int edge = INT_VPPX_HTone.ToneBorder(m_Tone);
		// exp.css .bdg.hov: a wash on every tone, a solid border on amber / red only
		bool solid = false;
		if (m_Hover && (m_Tone == TONE_AMBER || m_Tone == TONE_RED))
			solid = true;
		if (solid)
			edge = INT_VPPX_HTone.ToneEdge(m_Tone);
		m_T.SetColor(edge);
		m_B.SetColor(edge);
		m_L.SetColor(edge);
		m_R.SetColor(edge);
		if (m_Wash)
		{
			int wash = INT_VPPX_HTone.ToneWash(m_Tone);
			m_Wash.SetColor(wash);
			m_Wash.Show(m_Hover);
		}
		if (m_Dia)
			m_Dia.SetColor(txt);
		if (m_Text)
			m_Text.SetColor(txt);
	}
}

// ----------------------------------------------------------------------------
// Note (hero sale / broken / busy / refusal): opaque PANEL2 plate, 3 px tone edge
// on the left, UIF icon + 12 px caps title in the tone, the text wrapped at 15 px
// BODY2. A line break in the text starts an accent paragraph (RED2 for red, else
// the tone): e.g. the banned item names. The plate is as tall as its text (at most
// the built height). Set() shows the note (title and text "" hide it).
class INT_VPPX_HNote : Managed
{
	static const float TEXT_TOP = 40;
	static const float LINE_H = 22;

	protected Widget m_Root;
	protected ImageWidget m_Plate;
	protected ImageWidget m_Edge;
	protected ImageWidget m_IconW;
	protected TextWidget m_Title;
	protected ref array<TextWidget> m_Lines;
	protected float m_W;
	protected float m_H;
	protected int m_MaxLines;
	protected string m_Sig;

	void INT_VPPX_HNote()
	{
		m_Lines = new array<TextWidget>();
	}

	void Build(Widget parent, float x, float y, float w, float h)
	{
		m_W = w;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "VppxHNote");
		if (!m_Root)
			return;
		m_Plate = INT_VPPX_Look.Fill(m_Root, 0, 0, w, h, INT_VPPX_HTone.PANEL2);
		m_Edge = INT_VPPX_Look.Fill(m_Root, 0, 0, 3, h, INT_VPPX_Look.ORANGE);
		m_IconW = INT_UIF_Draw.Icon(m_Root, 18, 15, 14, "warning", INT_VPPX_Look.ORANGE);
		m_Title = INT_VPPX_Look.Text(m_Root, 40, 12, w - 40, 20, "", 12, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		float room = h - TEXT_TOP - 12;
		int rows = Math.Floor(room / LINE_H);
		if (rows < 1)
			rows = 1;
		m_MaxLines = rows;
		m_Lines.Clear();
		for (int i = 0; i < rows; i++)
		{
			float ly = TEXT_TOP + i * LINE_H;
			TextWidget lt = INT_VPPX_Look.Text(m_Root, 18, ly, w - 18, LINE_H, "", 15, INT_VPPX_HTone.BODY2);
			m_Lines.Insert(lt);
		}
		m_Root.Show(false);
	}

	// tone: INT_VPPX_HBadge.TONE_* or an ARGB colour; icon: INT_UIF atlas name ("" = none);
	// title / text: plain or #STR.
	void Set(int tone, string icon, string title, string text)
	{
		if (!m_Root)
			return;
		if (title == "" && text == "")
		{
			m_Sig = "";
			this.Show(false);
			return;
		}
		string sig = tone.ToString() + "|" + icon + "|" + title + "|" + text;
		if (sig == m_Sig)
		{
			this.Show(true);
			return;
		}
		m_Sig = sig;
		int edge = INT_VPPX_HTone.ToneEdge(tone);
		int head = INT_VPPX_HTone.ToneTitle(tone);
		int accentCol = INT_VPPX_HTone.ToneAccent(tone);
		if (m_Edge)
			m_Edge.SetColor(edge);
		float tx = 18;
		if (m_IconW)
		{
			bool hasIcon = false;
			if (icon != "")
				hasIcon = true;
			if (hasIcon)
			{
				string cell = INT_UIF_SET + "iuf_icon_" + icon;
				m_IconW.LoadImageFile(0, cell);
				m_IconW.SetImage(0);
				m_IconW.SetColor(head);
				tx = 40;
			}
			m_IconW.Show(hasIcon);
		}
		if (m_Title)
		{
			float titleRoom = m_W - tx - 16;
			string tcaps = INT_VPPX_Look.FitCaps(title, 12, true, titleRoom);
			INT_UIF_Draw.Place(m_Title, tx, 12, titleRoom + 40, 20);
			m_Title.SetText(tcaps);
			m_Title.SetColor(head);
		}
		string full = INT_VPPX_Text.Tr(text);
		string accent;
		string body = INT_VPPX_HTone.SplitBody(full, accent);
		float room = (m_W - 36) * 0.95;
		array<string> bodyLines = new array<string>();
		array<string> accLines = new array<string>();
		if (body != "")
			INT_VPPX_Look.Wrap(body, 15, room, bodyLines, m_MaxLines);
		int left = m_MaxLines - bodyLines.Count();
		if (accent != "" && left > 0)
			INT_VPPX_Look.Wrap(accent, 15, room, accLines, left);
		int nb = bodyLines.Count();
		int na = accLines.Count();
		int n = m_Lines.Count();
		for (int i = 0; i < n; i++)
		{
			TextWidget lt = m_Lines.Get(i);
			if (!lt)
				continue;
			string line = "";
			int col = INT_VPPX_HTone.BODY2;
			if (i < nb)
			{
				line = bodyLines.Get(i);
			}
			else if (i < nb + na)
			{
				int ai = i - nb;
				line = accLines.Get(ai);
				col = accentCol;
			}
			lt.SetText(line);
			lt.SetColor(col);
			bool vis = false;
			if (line != "")
				vis = true;
			lt.Show(vis);
		}
		int used = nb + na;
		float ph = TEXT_TOP + used * LINE_H + 12;
		if (used == 0)
			ph = TEXT_TOP;
		if (ph > m_H)
			ph = m_H;
		INT_UIF_Draw.Resize(m_Plate, m_W, ph);
		INT_UIF_Draw.Resize(m_Edge, 3, ph);
		this.Show(true);
	}

	void Show(bool show)
	{
		if (!m_Root)
			return;
		if (m_Root.IsVisible() != show)
			m_Root.Show(show);
	}

	Widget GetRoot()
	{
		return m_Root;
	}
}

// ----------------------------------------------------------------------------
// Hover tooltip (exp.css .tip): bg 0xFF161514, 1 px outline, 2 px tone top line,
// caps title 12 in the tone (diamond, red: «error» icon), text 15 WHITE, a line
// break starts an accent paragraph (banned names in RED2); a 12 px nub under the
// plate points at anchorX. Every widget has its own sort 100 (over the 3D model).
// A replaced / hidden tip is unlinked on the next Tick (never inside its event).
class INT_VPPX_HTip : Managed
{
	static const int SORT = 100;
	static const float PAD = 16;
	static const float LINE_H = 21;

	protected Widget m_Over;
	protected Widget m_Root;
	protected ref array<Widget> m_Dead;
	protected string m_Sig;

	void INT_VPPX_HTip()
	{
		m_Dead = new array<Widget>();
	}

	void ~INT_VPPX_HTip()
	{
		this.INT_DropDead();
		if (m_Root)
			m_Root.Unlink();
	}

	// over: the full-canvas over layer (design px = canvas px).
	void Build(Widget over)
	{
		m_Over = over;
	}

	bool IsShown()
	{
		if (m_Root)
			return true;
		return false;
	}

	// anchorX: x the nub points at; bottomY: bottom edge of the plate (the nub tip
	// is ~8 px lower); w <= 0 = 360 (450 for a long text). tone: INT_VPPX_HBadge.TONE_*
	// or an ARGB colour. title / text: plain or #STR.
	void ShowAt(float anchorX, float bottomY, float w, int tone, string title, string text)
	{
		if (!m_Over)
			return;
		string full = INT_VPPX_Text.Tr(text);
		string sig = anchorX.ToString() + "|" + bottomY.ToString() + "|" + w.ToString();
		sig = sig + "|" + tone.ToString() + "|" + title + "|" + full;
		if (m_Root && sig == m_Sig)
			return;
		this.Hide();
		m_Sig = sig;
		float tw = w;
		if (tw <= 0)
		{
			tw = 360;
			if (full.LengthUtf8() > 90)
				tw = 450;
		}
		if (tw < 200)
			tw = 200;
		string accent;
		string body = INT_VPPX_HTone.SplitBody(full, accent);
		float room = (tw - PAD * 2) * 0.95;
		array<string> bodyLines = new array<string>();
		array<string> accLines = new array<string>();
		if (body != "")
			INT_VPPX_Look.Wrap(body, 15, room, bodyLines, 8);
		if (accent != "")
			INT_VPPX_Look.Wrap(accent, 15, room, accLines, 6);
		int nb = bodyLines.Count();
		int na = accLines.Count();
		float textTop = 37;
		float th = textTop + nb * LINE_H;
		if (na > 0)
			th = th + 6 + na * LINE_H;
		th = th + 14;
		float x = anchorX - tw * 0.5;
		if (x > 1840 - tw)
			x = 1840 - tw;
		if (x < 80)
			x = 80;
		float y = bottomY - th;
		float nubX = anchorX - x;
		if (nubX < 18)
			nubX = 18;
		if (nubX > tw - 18)
			nubX = tw - 18;
		int edge = INT_VPPX_HTone.ToneEdge(tone);
		int head = INT_VPPX_HTone.ToneTitle(tone);
		int accentCol = INT_VPPX_HTone.ToneAccent(tone);

		// every widget gets its sort right after it is made: creation order is kept
		m_Root = INT_UIF_Draw.Frame(m_Over, x, y, tw, th + 12, "VppxHTip");
		if (!m_Root)
			return;
		m_Root.SetSort(SORT);
		this.INT_Sorted(INT_VPPX_Look.Fill(m_Root, 0, 0, tw, th, INT_VPPX_HTone.TIP_BG));
		this.INT_Sorted(INT_VPPX_Look.Fill(m_Root, 0, 0, tw, 1, INT_VPPX_HTone.TIP_EDGE));
		this.INT_Sorted(INT_VPPX_Look.Fill(m_Root, 0, th - 1, tw, 1, INT_VPPX_HTone.TIP_EDGE));
		this.INT_Sorted(INT_VPPX_Look.Fill(m_Root, 0, 0, 1, th, INT_VPPX_HTone.TIP_EDGE));
		this.INT_Sorted(INT_VPPX_Look.Fill(m_Root, tw - 1, 0, 1, th, INT_VPPX_HTone.TIP_EDGE));
		this.INT_Sorted(INT_VPPX_Look.Fill(m_Root, 0, 0, tw, 2, edge));
		// nub (exp.css .tip .nt): a diamond in the plate colour on the bottom hairline, its
		// lower half outside with the 1 px outline; a plate-coloured strip hides the upper
		// half of the outline and the hairline between the two lower edges
		this.INT_Sorted(INT_VPPX_Look.Diamond(m_Root, nubX, th - 1, 12, true, INT_VPPX_HTone.TIP_BG));
		this.INT_Sorted(INT_VPPX_Look.Diamond(m_Root, nubX, th - 1, 12, false, INT_VPPX_HTone.TIP_EDGE));
		this.INT_Sorted(INT_VPPX_Look.Fill(m_Root, nubX - 7.5, th - 10, 15, 10, INT_VPPX_HTone.TIP_BG));
		float titleX = PAD + 7 + 9;
		bool red = false;
		if (tone == INT_VPPX_HBadge.TONE_RED || tone == INT_VPPX_HBadge.TONE_RUIN)
			red = true;
		if (red)
		{
			this.INT_Sorted(INT_UIF_Draw.Icon(m_Root, PAD, 15, 14, "error", head));
			titleX = PAD + 14 + 9;
		}
		else
		{
			this.INT_Sorted(INT_VPPX_Look.Diamond(m_Root, PAD + 3.5, 22, 7, true, head));
		}
		float titleRoom = tw - titleX - PAD;
		string tcaps = INT_VPPX_Look.FitCaps(title, 12, true, titleRoom);
		this.INT_Sorted(INT_VPPX_Look.Text(m_Root, titleX, 13, titleRoom + 30, 18, tcaps, 12, head, INT_UIF_Align.LEFT, true));
		float ly = textTop;
		for (int i = 0; i < nb; i++)
		{
			string bl = bodyLines.Get(i);
			this.INT_Sorted(INT_VPPX_Look.Text(m_Root, PAD, ly, tw - PAD, LINE_H, bl, 15, INT_VPPX_Look.WHITE));
			ly = ly + LINE_H;
		}
		if (na > 0)
			ly = ly + 6;
		for (int a = 0; a < na; a++)
		{
			string al = accLines.Get(a);
			this.INT_Sorted(INT_VPPX_Look.Text(m_Root, PAD, ly, tw - PAD, LINE_H, al, 15, accentCol));
			ly = ly + LINE_H;
		}
	}

	protected void INT_Sorted(Widget w)
	{
		if (w)
			w.SetSort(SORT);
	}

	// Hidden now, unlinked on the next Tick.
	void Hide()
	{
		m_Sig = "";
		if (!m_Root)
			return;
		m_Root.Show(false);
		m_Dead.Insert(m_Root);
		m_Root = null;
	}

	void Tick()
	{
		this.INT_DropDead();
	}

	protected void INT_DropDead()
	{
		if (!m_Dead)
			return;
		int n = m_Dead.Count();
		for (int i = 0; i < n; i++)
		{
			Widget w = m_Dead.Get(i);
			if (w)
				w.Unlink();
		}
		m_Dead.Clear();
	}
}

// Wrapped text as separate TextWidgets (MultilineTextWidget lines overlap).
class INT_VPPX_HLines : Managed
{
	protected Widget m_INT_Root;
	protected ref array<TextWidget> m_INT_Rows;
	protected float m_INT_W;
	protected float m_INT_LineH;
	protected float m_INT_Size;
	protected int m_INT_Max;
	protected int m_INT_Count;
	protected string m_INT_Last;

	void INT_VPPX_HLines()
	{
		m_INT_Rows = new array<TextWidget>();
	}

	void Build(Widget parent, float x, float y, float w, float lineH, float size, int color, int maxLines)
	{
		m_INT_W = w;
		m_INT_LineH = lineH;
		m_INT_Size = size;
		m_INT_Max = maxLines;
		if (m_INT_Max < 1)
			m_INT_Max = 1;
		float h = lineH * m_INT_Max;
		m_INT_Root = INT_UIF_Draw.Frame(parent, x, y, w + 40, h, "VppxHLines");
		if (!m_INT_Root)
			return;
		for (int i = 0; i < m_INT_Max; i++)
		{
			float ly = i * lineH;
			TextWidget t = INT_UIF_Draw.Text(m_INT_Root, 0, ly, w + 40, lineH, "", size, color);
			if (t)
				t.Show(false);
			m_INT_Rows.Insert(t);
		}
	}

	Widget GetRoot()
	{
		return m_INT_Root;
	}

	// Plain text or #STR; returns the number of lines shown (the last one cut with "…").
	int Set(string value)
	{
		string txt = INT_VPPX_Text.Tr(value);
		if (txt == m_INT_Last)
			return m_INT_Count;
		m_INT_Last = txt;
		array<string> lines = new array<string>();
		int cap = m_INT_Max + 1;
		INT_VPPX_Look.Wrap(txt, m_INT_Size, m_INT_W, lines, cap);
		int n = lines.Count();
		if (n > m_INT_Max)
		{
			// more text follows: the last line always ends in "…", cut by the same measure as Wrap
			string joined = lines.Get(m_INT_Max - 1) + " " + lines.Get(m_INT_Max);
			string cut = this.INT_CutLine(joined);
			lines.Set(m_INT_Max - 1, cut);
			n = m_INT_Max;
		}
		// rows actually built (none when Build found no root)
		int rows = m_INT_Rows.Count();
		for (int i = 0; i < rows; i++)
		{
			TextWidget t = m_INT_Rows.Get(i);
			if (!t)
				continue;
			bool on = false;
			if (i < n)
				on = true;
			if (on)
				t.SetText(lines.Get(i));
			else
				t.SetText("");
			if (t.IsVisible() != on)
				t.Show(on);
		}
		m_INT_Count = n;
		return n;
	}

	// Head of `t` + "…" that fits the row width (INT_VPPX_Look.Measure, like Wrap).
	protected string INT_CutLine(string t)
	{
		int len = t.LengthUtf8();
		while (len > 1)
		{
			len--;
			string head = t.SubstringUtf8(0, len);
			head.TrimInPlace();
			string withDots = head + "…";
			float hw = INT_VPPX_Look.Measure(withDots, m_INT_Size);
			if (hw <= m_INT_W)
				return withDots;
		}
		return "…";
	}

	void SetColor(int color)
	{
		foreach (TextWidget t : m_INT_Rows)
		{
			if (t)
				t.SetColor(color);
		}
	}

	void Move(float x, float y)
	{
		INT_UIF_Draw.Move(m_INT_Root, x, y);
	}

	void Show(bool show)
	{
		if (!m_INT_Root)
			return;
		if (m_INT_Root.IsVisible() != show)
			m_INT_Root.Show(show);
	}
}

// ---------------------------------------------------------------------------------------------------------
// Slider: 2 px track, orange fill, 7 ticks, labels under it, diamond thumb (14), outline ghost (12) at the
// saved value. (x, y) of Build = left end of the track's centre line, w = track length.
class INT_VPPX_HSlider : ScriptedWidgetEventHandler
{
	static const float THUMB = 14;
	static const float GHOST = 12;
	static const float HALO = 24;
	static const float TRACK_Y = 12;			// track centre line inside the root
	static const float ANIM_TIME = 0.2;
	static const int TRACK_COLOR = 0x1FFFFFFF;
	static const int TICK_COLOR = 0x1FFFFFFF;
	static const int GHOST_COLOR = 0x59FFFFFF;
	static const int HALO_COLOR = 0x40FF7A2F;

	protected Widget m_INT_Root;
	protected ImageWidget m_INT_Track;
	protected ImageWidget m_INT_Fill;
	protected ImageWidget m_INT_Ghost;
	protected ImageWidget m_INT_Halo;
	protected ImageWidget m_INT_Thumb;
	protected ButtonWidget m_INT_Hit;
	protected ref array<TextWidget> m_INT_Labels;
	protected float m_INT_W;
	protected float m_INT_Min;
	protected float m_INT_Max;
	protected float m_INT_Step;
	protected float m_INT_Value;
	protected float m_INT_GhostV;
	protected bool m_INT_HasGhost;
	protected float m_INT_ShownK;
	protected float m_INT_FromK;
	protected float m_INT_AnimT;
	protected bool m_INT_Animating;
	protected bool m_INT_Dragging;
	protected bool m_INT_Hover;
	protected ref ScriptInvoker m_INT_OnChanged;

	void INT_VPPX_HSlider()
	{
		m_INT_Labels = new array<TextWidget>();
		m_INT_OnChanged = new ScriptInvoker();
	}

	// (float value, INT_VPPX_HSlider s, bool isFinal): drag = false per move, release / wheel / step = true
	ScriptInvoker OnChanged()
	{
		return m_INT_OnChanged;
	}

	void Build(Widget parent, float x, float y, float w, float minV, float maxV, float stepV)
	{
		m_INT_W = w;
		if (m_INT_W < 10)
			m_INT_W = 10;
		m_INT_Min = minV;
		m_INT_Max = maxV;
		if (m_INT_Max <= m_INT_Min)
			m_INT_Max = m_INT_Min + 1;
		m_INT_Step = stepV;
		m_INT_Value = m_INT_Min;
		m_INT_Root = INT_UIF_Draw.Frame(parent, x, y - TRACK_Y, m_INT_W, 46, "VppxHSlider");
		if (!m_INT_Root)
			return;
		m_INT_Track = INT_VPPX_Look.Fill(m_INT_Root, 0, TRACK_Y - 1, m_INT_W, 2, TRACK_COLOR);
		for (int q = 0; q <= 6; q++)
		{
			float tx = (m_INT_W - 1) * q / 6.0;
			INT_VPPX_Look.Fill(m_INT_Root, tx, TRACK_Y + 5, 1, 5, TICK_COLOR);
		}
		m_INT_Fill = INT_VPPX_Look.Fill(m_INT_Root, 0, TRACK_Y - 1, 0, 2, INT_VPPX_Look.ORANGE);
		m_INT_Ghost = INT_VPPX_Look.Diamond(m_INT_Root, 0, TRACK_Y, GHOST, false, GHOST_COLOR);
		m_INT_Halo = INT_VPPX_Look.Diamond(m_INT_Root, 0, TRACK_Y, HALO, false, HALO_COLOR);
		m_INT_Thumb = INT_VPPX_Look.Diamond(m_INT_Root, 0, TRACK_Y, THUMB, true, INT_VPPX_Look.ORANGE);
		if (m_INT_Ghost)
			m_INT_Ghost.Show(false);
		if (m_INT_Halo)
			m_INT_Halo.Show(false);
		m_INT_Hit = INT_UIF_Draw.Hit(m_INT_Root, -12, TRACK_Y - 14, m_INT_W + 24, 28, this);
		m_INT_ShownK = 0;
		this.INT_Place();
	}

	Widget GetRoot()
	{
		return m_INT_Root;
	}

	// Labels 12 px DARK centred at the track fractions (at01[i] 0..1).
	void SetLabels(array<float> at01, array<string> txt)
	{
		foreach (TextWidget old : m_INT_Labels)
		{
			if (old)
				old.Unlink();
		}
		m_INT_Labels.Clear();
		if (!m_INT_Root || !at01 || !txt)
			return;
		int n = at01.Count();
		if (txt.Count() < n)
			n = txt.Count();
		for (int i = 0; i < n; i++)
		{
			float lx = at01.Get(i) * m_INT_W;
			string s = txt.Get(i);
			TextWidget t = INT_VPPX_Look.Text(m_INT_Root, lx - 60, TRACK_Y + 16, 120, 16, s, 12, INT_VPPX_Look.DARK, INT_UIF_Align.CENTER, false);
			m_INT_Labels.Insert(t);
		}
	}

	float GetValue()
	{
		return m_INT_Value;
	}

	float INT_GetMin()
	{
		return m_INT_Min;
	}

	float INT_GetMax()
	{
		return m_INT_Max;
	}

	float INT_GetStep()
	{
		return m_INT_Step;
	}

	bool INT_IsDragging()
	{
		return m_INT_Dragging;
	}

	// Clamped to the range and snapped to the step (the UIF slider's Snap).
	float INT_SnapValue(float v)
	{
		float r = Math.Clamp(v, m_INT_Min, m_INT_Max);
		if (m_INT_Step > 0)
		{
			float n = Math.Round((r - m_INT_Min) / m_INT_Step);
			r = m_INT_Min + n * m_INT_Step;
		}
		return Math.Clamp(r, m_INT_Min, m_INT_Max);
	}

	// notify -> OnChanged(v, this, true), like INT_UIF_Slider.SetValue.
	void SetValue(float v, bool notify = false, bool animate = true)
	{
		float nv = this.INT_SnapValue(v);
		m_INT_Value = nv;
		float k = this.INT_K(nv);
		if (animate && m_INT_Root)
		{
			m_INT_FromK = m_INT_ShownK;
			m_INT_AnimT = 0;
			m_INT_Animating = true;
		}
		else
		{
			m_INT_ShownK = k;
			m_INT_Animating = false;
			this.INT_Place();
		}
		this.INT_UpdateGhost();
		if (notify)
		{
			INT_VPPX_HSlider keep = this;
			m_INT_OnChanged.Invoke(m_INT_Value, this, true);
		}
	}

	// v < min hides the ghost; it is also hidden while it sits under the thumb.
	void SetGhost(float v)
	{
		if (v < m_INT_Min)
		{
			m_INT_HasGhost = false;
		}
		else
		{
			m_INT_HasGhost = true;
			m_INT_GhostV = Math.Clamp(v, m_INT_Min, m_INT_Max);
		}
		this.INT_UpdateGhost();
	}

	// Per frame from the page: drag poll (GetMouseState & MB_PRESSED_MASK) + thumb animation.
	void Tick(float dt)
	{
		if (m_INT_Dragging)
		{
			int ms = GetMouseState(MouseState.LEFT);
			int held = ms & MB_PRESSED_MASK;
			if (held != 0)
			{
				this.INT_FromMouse();
			}
			else
			{
				m_INT_Dragging = false;
				this.INT_PaintHalo();
				INT_VPPX_HSlider keep = this;
				m_INT_OnChanged.Invoke(m_INT_Value, this, true);
			}
		}
		if (m_INT_Animating)
		{
			m_INT_AnimT = m_INT_AnimT + dt;
			float p = m_INT_AnimT / ANIM_TIME;
			if (p >= 1)
			{
				p = 1;
				m_INT_Animating = false;
			}
			float inv = 1 - p;
			float e = 1 - inv * inv * inv;
			float target = this.INT_K(m_INT_Value);
			m_INT_ShownK = m_INT_FromK + (target - m_INT_FromK) * e;
			this.INT_Place();
		}
	}

	protected float INT_K(float v)
	{
		float span = m_INT_Max - m_INT_Min;
		if (span <= 0)
			return 0;
		float k = (v - m_INT_Min) / span;
		return Math.Clamp(k, 0, 1);
	}

	protected void INT_MoveDiamond(Widget w, float cx, float size)
	{
		if (!w)
			return;
		float half = size * INT_VPPX_Look.SQRT2 * 0.5;
		INT_UIF_Draw.Move(w, cx - half, TRACK_Y - half);
	}

	protected void INT_Place()
	{
		if (!m_INT_Root)
			return;
		float k = Math.Clamp(m_INT_ShownK, 0, 1);
		float cx = k * m_INT_W;
		INT_UIF_Draw.Resize(m_INT_Fill, cx, 2);
		this.INT_MoveDiamond(m_INT_Thumb, cx, THUMB);
		this.INT_MoveDiamond(m_INT_Halo, cx, HALO);
	}

	protected void INT_UpdateGhost()
	{
		if (!m_INT_Ghost)
			return;
		bool on = false;
		if (m_INT_HasGhost)
		{
			float d = Math.AbsFloat(m_INT_GhostV - m_INT_Value);
			if (d > 0.001)
				on = true;
		}
		if (on)
		{
			float gk = this.INT_K(m_INT_GhostV);
			this.INT_MoveDiamond(m_INT_Ghost, gk * m_INT_W, GHOST);
		}
		if (m_INT_Ghost.IsVisible() != on)
			m_INT_Ghost.Show(on);
	}

	protected void INT_PaintHalo()
	{
		if (!m_INT_Halo)
			return;
		bool on = false;
		if (m_INT_Hover || m_INT_Dragging)
			on = true;
		if (m_INT_Halo.IsVisible() != on)
			m_INT_Halo.Show(on);
	}

	protected void INT_FromMouse()
	{
		if (!m_INT_Track)
			return;
		int mx;
		int my;
		GetMousePos(mx, my);
		float sx;
		float sy;
		float sw;
		float sh;
		m_INT_Track.GetScreenPos(sx, sy);
		m_INT_Track.GetScreenSize(sw, sh);
		if (sw <= 1)
			return;
		float k = Math.Clamp((mx - sx) / sw, 0, 1);
		float raw = m_INT_Min + k * (m_INT_Max - m_INT_Min);
		float v = this.INT_SnapValue(raw);
		float d = Math.AbsFloat(v - m_INT_Value);
		if (d < 0.0001)
			return;
		m_INT_Value = v;
		m_INT_ShownK = this.INT_K(v);
		m_INT_Animating = false;
		this.INT_Place();
		this.INT_UpdateGhost();
		INT_VPPX_HSlider keep = this;
		m_INT_OnChanged.Invoke(m_INT_Value, this, false);
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (w != m_INT_Hit || button != MouseState.LEFT)
			return false;
		m_INT_Dragging = true;
		this.INT_PaintHalo();
		this.INT_FromMouse();
		return true;
	}

	// One step per notch, notified even at the ends of the range (INT_UIF_Slider.OnMouseWheel:
	// the old window sent the final value on every notch, so the menu's requests stay the same).
	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (w != m_INT_Hit)
			return false;
		float st = m_INT_Step;
		if (st <= 0)
			st = (m_INT_Max - m_INT_Min) / 20.0;
		float nv = m_INT_Value + wheel * st;
		this.SetValue(nv, true, true);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_INT_Hit)
			return false;
		m_INT_Hover = true;
		this.INT_PaintHalo();
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_INT_Hit)
			return false;
		m_INT_Hover = false;
		this.INT_PaintHalo();
		return false;
	}
}

// ----------------------------------------------------------------------------
// Segment «◇ A │ ◆ B │ ◇ C» (exp.css .seg3), generalised from the Garage's read-only save-mode segment
// (two fixed Garage labels -> any number of labels given in): one 1 px LINE_STRONG outline with hairline
// separators; the lit cell has an opaque orange wash, a 2 px orange bar along its bottom, a filled orange
// diamond and orange caps, the others an outline DARK diamond and DARK caps. Labels: plain or #STR, drawn
// as tracked caps, 13 px shrinking to 10 to fit.
//   cellsW <= 0  every cell as wide as its label (the Garage form; all cells together at most MAX_W)
//   cellsW >  0  equal cells over cellsW, content centred (concept B .seg3)
// Read-only by default (the Garage form: set by the caller, no hover, no clicks; lockIcon draws the UIF
// «lock» glyph after it). clickable = true: a hit per cell, a click on an unlit cell lights it and fires
// OnChanged; hover brightens an unlit cell's caps.
class INT_VPPX_HSegment : ScriptedWidgetEventHandler
{
	static const float SEG_H = 40;
	static const float PAD = 18;
	static const float DIA_ROOM = 16;          // diamond 7 + gap 9
	static const float MAX_W = 440;            // content-sized cells together; the lock follows
	// opaque pre-mix of rgba(255,122,47,.13) over the window (frame pixel 43,26,16; panels under text are opaque)
	static const int WASH = 0xFF2B1A10;

	protected Widget m_Root;
	protected ref array<ImageWidget> m_Wash;
	protected ref array<ImageWidget> m_Bar;
	protected ref array<ImageWidget> m_Dia;
	protected ref array<TextWidget> m_Label;
	protected ref array<ButtonWidget> m_Hits;
	protected float m_W;
	protected float m_H = 40;
	protected bool m_Lock;
	protected bool m_Clickable;
	protected int m_On = -1;
	protected ref ScriptInvoker m_OnChanged;

	void INT_VPPX_HSegment()
	{
		m_Wash = new array<ImageWidget>();
		m_Bar = new array<ImageWidget>();
		m_Dia = new array<ImageWidget>();
		m_Label = new array<TextWidget>();
		m_Hits = new array<ButtonWidget>();
		m_OnChanged = new ScriptInvoker();
	}

	// (int index, INT_VPPX_HSegment seg) - a click of a clickable segment (Set() does not fire it).
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	// (x, y) = top-left. labels: plain or #STR (at least one). selected: lit cell (-1 = none).
	// h: height (SEG_H 40 = Garage, 44 = concept B).
	void Build(Widget parent, float x, float y, array<string> labels, int selected = 0, float cellsW = 0, float h = 40, bool clickable = false, bool lockIcon = false)
	{
		if (!parent || !labels)
			return;
		int n = labels.Count();
		if (n <= 0)
			return;
		m_Wash.Clear();
		m_Bar.Clear();
		m_Dia.Clear();
		m_Label.Clear();
		m_Hits.Clear();
		m_On = -1;
		m_H = h;
		if (m_H < 24)
			m_H = 24;
		m_Lock = lockIcon;
		m_Clickable = clickable;
		array<string> caps = new array<string>();
		for (int c = 0; c < n; c++)
		{
			string lab = labels.Get(c);
			string cp = INT_VPPX_Text.Caps(lab);
			caps.Insert(cp);
		}
		float size = 13;
		float cellFixed = PAD * 2 + DIA_ROOM;
		array<float> lw = new array<float>();
		this.INT_MeasureAll(caps, size, lw);
		while (size > 10 && this.INT_TooWide(lw, cellFixed, cellsW))
		{
			size = size - 1;
			this.INT_MeasureAll(caps, size, lw);
		}
		array<float> cellX = new array<float>();
		array<float> cellW = new array<float>();
		float at = 0;
		for (int i = 0; i < n; i++)
		{
			float wi = cellFixed + lw.Get(i);
			if (cellsW > 0)
				wi = cellsW / n;
			cellX.Insert(at);
			cellW.Insert(wi);
			at = at + wi;
		}
		m_W = at;
		float rootW = this.GetW();
		m_Root = INT_UIF_Draw.Frame(parent, x, y, rootW, m_H, "VppxHSegment");
		if (!m_Root)
			return;
		for (int k = 0; k < n; k++)
		{
			float kx = cellX.Get(k);
			float kw = cellW.Get(k);
			string kc = caps.Get(k);
			float kl = lw.Get(k);
			this.INT_BuildCell(kx, kw, kc, kl, size);
		}
		INT_VPPX_Look.Outline(m_Root, 0, 0, m_W, m_H, INT_VPPX_Look.LINE_STRONG);
		for (int s = 1; s < n; s++)
		{
			float sx = cellX.Get(s);
			INT_VPPX_Look.Fill(m_Root, sx, 0, 1, m_H, INT_VPPX_Look.LINE_STRONG);
		}
		if (m_Lock)
		{
			float ly = (m_H - 14) * 0.5;
			INT_UIF_Draw.Icon(m_Root, m_W + 14, ly, 14, "lock", INT_VPPX_Look.DARK);
		}
		if (m_Clickable)
		{
			for (int q = 0; q < n; q++)
			{
				float qx = cellX.Get(q);
				float qw = cellW.Get(q);
				ButtonWidget hit = INT_UIF_Draw.Hit(m_Root, qx, 0, qw, m_H, this);
				m_Hits.Insert(hit);
			}
		}
		this.Set(selected);
	}

	protected void INT_MeasureAll(array<string> caps, float size, array<float> outW)
	{
		outW.Clear();
		int n = caps.Count();
		for (int i = 0; i < n; i++)
		{
			string c = caps.Get(i);
			float w = INT_VPPX_Look.Measure(c, size, true);
			outW.Insert(w);
		}
	}

	protected bool INT_TooWide(array<float> lw, float cellFixed, float cellsW)
	{
		int n = lw.Count();
		if (n <= 0)
			return false;
		if (cellsW > 0)
		{
			float each = cellsW / n;
			for (int i = 0; i < n; i++)
			{
				float need = lw.Get(i) + cellFixed;
				if (need > each)
					return true;
			}
			return false;
		}
		float total = 0;
		for (int j = 0; j < n; j++)
		{
			total = total + lw.Get(j) + cellFixed;
		}
		if (total > MAX_W)
			return true;
		return false;
	}

	protected void INT_BuildCell(float x, float w, string caps, float labelW, float size)
	{
		ImageWidget wash = INT_VPPX_Look.Fill(m_Root, x + 1, 1, w - 1, m_H - 2, WASH);
		m_Wash.Insert(wash);
		ImageWidget bar = INT_VPPX_Look.Fill(m_Root, x + 1, m_H - 3, w - 1, 2, INT_VPPX_Look.ORANGE);
		m_Bar.Insert(bar);
		// content (diamond room + label) centred in the cell: = PAD on both sides for a label-sized cell
		float off = (w - DIA_ROOM - labelW) * 0.5;
		float dcx = x + off + 3.5;
		ImageWidget dia = INT_VPPX_Look.Diamond(m_Root, dcx, m_H * 0.5, 7, false, INT_VPPX_Look.DARK);
		m_Dia.Insert(dia);
		float tx = x + off + DIA_ROOM;
		TextWidget lab = INT_VPPX_Look.Text(m_Root, tx, 0, w + 24, m_H, caps, size, INT_VPPX_Look.DARK, INT_UIF_Align.LEFT, true);
		m_Label.Insert(lab);
	}

	// Lights cell `index` (-1 = none). Does not fire OnChanged.
	void Set(int index)
	{
		int on = index;
		if (on < -1 || on >= m_Label.Count())
			on = -1;
		if (on == m_On)
			return;
		m_On = on;
		int n = m_Label.Count();
		for (int i = 0; i < n; i++)
		{
			bool lit = false;
			if (i == on)
				lit = true;
			int col = INT_VPPX_Look.DARK;
			if (lit)
				col = INT_VPPX_Look.ORANGE;
			ImageWidget wash = m_Wash.Get(i);
			if (wash)
				wash.Show(lit);
			ImageWidget bar = m_Bar.Get(i);
			if (bar)
				bar.Show(lit);
			ImageWidget dia = m_Dia.Get(i);
			if (dia)
			{
				INT_VPPX_Look.SetDiamondFilled(dia, lit);
				dia.SetColor(col);
			}
			TextWidget lab = m_Label.Get(i);
			INT_VPPX_Look.SetColor(lab, col);
		}
	}

	int GetSelected()
	{
		return m_On;
	}

	// Width with the lock glyph if any (design px).
	float GetW()
	{
		if (m_Lock)
			return m_W + 28;
		return m_W;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	void Show(bool show)
	{
		if (!m_Root)
			return;
		if (m_Root.IsVisible() != show)
			m_Root.Show(show);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		int i = m_Hits.Find(bw);
		if (i < 0 || button != MouseState.LEFT)
			return false;
		if (i == m_On)
			return true;
		this.Set(i);
		INT_VPPX_HSegment keep = this;
		m_OnChanged.Invoke(i, keep);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		int i = m_Hits.Find(bw);
		if (i < 0 || i == m_On)
			return false;
		TextWidget lab = m_Label.Get(i);
		INT_VPPX_Look.SetColor(lab, INT_VPPX_Look.BODY);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		int i = m_Hits.Find(bw);
		if (i < 0 || i == m_On)
			return false;
		TextWidget lab = m_Label.Get(i);
		INT_VPPX_Look.SetColor(lab, INT_VPPX_Look.DARK);
		return false;
	}
}

// ----------------------------------------------------------------------------
// Text field (exp.css .inp; the Garage rename field): kit icon 17 MUTED (default
// «pencil», "" = none), edit box 18 px WHITE (a plain
// EditBox sizes its glyphs to its own height: the box is 18 tall), placeholder,
// underline LINE_STRONG -> 2 px ORANGE while focused. Clicking anywhere on the
// field focuses it. Enter arrives as OnChange(finished = true) or OnKeyPress:
// one OnSubmit per press. The text is cut to maxLen characters while typing.
class INT_VPPX_HInput : ScriptedWidgetEventHandler
{
	static const float INT_H = 42;
	static const float INT_TEXT_X = 29;
	static const int INT_SUBMIT_GAP_MS = 150;
	static const int INT_PH = 0xFF77726B;

	protected Widget m_INT_Root;
	protected ImageWidget m_INT_Pencil;
	protected TextWidget m_INT_Ph;
	protected EditBoxWidget m_INT_Edit;
	protected ImageWidget m_INT_Line;
	protected ButtonWidget m_INT_Hit;
	protected float m_INT_W;
	protected int m_INT_Max;
	protected bool m_INT_Focus;
	protected string m_INT_Last;
	protected int m_INT_LastSubmitMs = -100000;
	protected ref ScriptInvoker m_INT_OnSubmit;
	protected ref ScriptInvoker m_INT_OnChanged;

	void INT_VPPX_HInput()
	{
		m_INT_OnSubmit = new ScriptInvoker();
		m_INT_OnChanged = new ScriptInvoker();
	}

	// (string text, INT_VPPX_HInput i) - Enter
	ScriptInvoker OnSubmit()
	{
		return m_INT_OnSubmit;
	}

	// (string text, INT_VPPX_HInput i) - every edit
	ScriptInvoker OnChanged()
	{
		return m_INT_OnChanged;
	}

	// placeholderKey: plain, #STR or ""; maxLen: characters (0 = no limit); kitIcon: kit icon name. Height 42.
	void Build(Widget parent, float x, float y, float w, string placeholderKey, int maxLen, string kitIcon = "pencil")
	{
		m_INT_W = w;
		m_INT_Max = maxLen;
		m_INT_Root = INT_UIF_Draw.Frame(parent, x, y, w, INT_H, "VppxHNameInput");
		if (!m_INT_Root)
			return;
		if (kitIcon != "")
		{
			string iconTex = INT_VPPX_Look.IconPath(kitIcon);
			m_INT_Pencil = INT_UIF_Draw.Texture(m_INT_Root, 0, 12, 17, 17, iconTex, INT_VPPX_Look.MUTED);
		}
		m_INT_Ph = INT_VPPX_Look.Text(m_INT_Root, INT_TEXT_X, 0, w - INT_TEXT_X, INT_H, placeholderKey, 17, INT_PH);
		m_INT_Hit = INT_UIF_Draw.Hit(m_INT_Root, 0, 0, w, INT_H, this);
		m_INT_Edit = INT_UIF_Draw.Edit(m_INT_Root, INT_TEXT_X - 2, 0, w - INT_TEXT_X, INT_H, 18);
		if (m_INT_Edit)
		{
			m_INT_Edit.SetHandler(this);
			m_INT_Edit.SetColor(ARGB(255, 255, 255, 255));
			m_INT_Edit.SetTextColor(INT_VPPX_Look.WHITE);
		}
		m_INT_Line = INT_VPPX_Look.Fill(m_INT_Root, 0, INT_H - 1, w, 1, INT_VPPX_Look.LINE_STRONG);
		m_INT_Last = "";
		this.INT_Paint();
	}

	Widget GetRoot()
	{
		return m_INT_Root;
	}

	string GetText()
	{
		if (!m_INT_Edit)
			return "";
		return m_INT_Edit.GetText();
	}

	// Sets the text (cut to maxLen) without firing OnChanged.
	void SetText(string t)
	{
		string s = t;
		if (m_INT_Max > 0 && s.LengthUtf8() > m_INT_Max)
			s = s.SubstringUtf8(0, m_INT_Max);
		// m_INT_Last first: an OnChange raised by SetText then sees no change
		m_INT_Last = s;
		if (m_INT_Edit)
			m_INT_Edit.SetText(s);
		this.INT_UpdatePh();
	}

	// The engine focus counts too (a focus event can be missed: the Esc order must still blur).
	bool IsFocused()
	{
		if (m_INT_Focus)
			return true;
		if (!m_INT_Edit)
			return false;
		Widget f = GetFocus();
		if (f && f == m_INT_Edit)
			return true;
		return false;
	}

	void Blur()
	{
		bool mine = m_INT_Focus;
		Widget f = GetFocus();
		if (f && m_INT_Edit && f == m_INT_Edit)
			mine = true;
		if (mine)
			SetFocus(null);
		m_INT_Focus = false;
		this.INT_Paint();
	}

	protected void INT_UpdatePh()
	{
		if (!m_INT_Ph)
			return;
		string t = this.GetText();
		bool show = false;
		if (t == "" && !m_INT_Focus)
			show = true;
		if (m_INT_Ph.IsVisible() != show)
			m_INT_Ph.Show(show);
	}

	protected void INT_Paint()
	{
		if (m_INT_Line)
		{
			if (m_INT_Focus)
			{
				INT_UIF_Draw.Place(m_INT_Line, 0, INT_H - 2, m_INT_W, 2);
				m_INT_Line.SetColor(INT_VPPX_Look.ORANGE);
			}
			else
			{
				INT_UIF_Draw.Place(m_INT_Line, 0, INT_H - 1, m_INT_W, 1);
				m_INT_Line.SetColor(INT_VPPX_Look.LINE_STRONG);
			}
		}
		this.INT_UpdatePh();
	}

	protected static bool INT_EnterDown()
	{
		if (KeyState(KeyCode.KC_RETURN) != 0)
			return true;
		if (KeyState(KeyCode.KC_NUMPADENTER) != 0)
			return true;
		return false;
	}

	// One OnSubmit per press, whichever of the two events brings it.
	protected void INT_Submit()
	{
		if (!GetGame())
			return;
		int now = GetGame().GetTime();
		if (now - m_INT_LastSubmitMs < INT_SUBMIT_GAP_MS)
			return;
		m_INT_LastSubmitMs = now;
		INT_VPPX_HInput keep = this;
		string t = this.GetText();
		m_INT_OnSubmit.Invoke(t, keep);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!m_INT_Hit || w != m_INT_Hit)
			return false;
		if (m_INT_Edit)
		{
			SetFocus(m_INT_Edit);
			m_INT_Focus = true;
			this.INT_Paint();
		}
		return true;
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (!m_INT_Edit || w != m_INT_Edit)
			return false;
		m_INT_Focus = true;
		this.INT_Paint();
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (!m_INT_Edit || w != m_INT_Edit)
			return false;
		m_INT_Focus = false;
		this.INT_Paint();
		return false;
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (!m_INT_Edit || w != m_INT_Edit)
			return false;
		string t = m_INT_Edit.GetText();
		// characters, not bytes: Cyrillic is 2 bytes per letter
		if (m_INT_Max > 0 && t.LengthUtf8() > m_INT_Max)
		{
			t = t.SubstringUtf8(0, m_INT_Max);
			m_INT_Edit.SetText(t);
		}
		this.INT_UpdatePh();
		INT_VPPX_HInput keep = this;
		if (t != m_INT_Last)
		{
			m_INT_Last = t;
			m_INT_OnChanged.Invoke(t, keep);
		}
		// a "finished" that is not Enter (the focus moving away) submits nothing
		if (finished && INT_VPPX_HInput.INT_EnterDown())
			this.INT_Submit();
		return true;
	}

	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if (!m_INT_Edit || w != m_INT_Edit)
			return false;
		if (key == KeyCode.KC_RETURN || key == KeyCode.KC_NUMPADENTER)
		{
			this.INT_Submit();
			return true;
		}
		return false;
	}
}

// ----------------------------------------------------------------------------
// Checkbox (exp.css .ck) - NEW in the map kit (the Garage has none), built from the same parts as
// INT_VPPX_Toggle: size x size box with a 1 px LINE_STRONG edge; on = orange plate + INK check glyph (kit
// icon «check»). Its own hit (box + 4 px around) toggles it -> OnChanged(bool on, INT_VPPX_Check c); a row
// that owns a bigger hit (a layer chip) calls Toggle() / SetOn() itself. Disabled: clicks ignored, a dark
// plate / faint glyph (no SetAlpha: it would turn the hairlines opaque).
class INT_VPPX_Check : ScriptedWidgetEventHandler
{
	static const int PLATE_OFF = 0xFF0F0E0D;       // exp.css #0f0e0d (opaque)
	static const int PLATE_DISABLED = 0xFF2B2622;

	protected Widget m_Root;
	protected ImageWidget m_Back;
	protected ImageWidget m_Plate;
	protected Widget m_Edge;
	protected ImageWidget m_Glyph;
	protected ButtonWidget m_Hit;
	protected float m_Size;
	protected bool m_On;
	protected bool m_Enabled = true;
	protected ref ScriptInvoker m_OnChanged;

	void INT_VPPX_Check()
	{
		m_OnChanged = new ScriptInvoker();
	}

	// (bool on, INT_VPPX_Check c)
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	// size: 18 (exp.css .ck) or 16 (layer chip .chip .ck).
	void Build(Widget parent, float x, float y, float size = 18, bool on = false)
	{
		m_Size = size;
		if (m_Size < 10)
			m_Size = 10;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, m_Size, m_Size, "VppxCheck");
		if (!m_Root)
			return;
		m_Back = INT_VPPX_Look.Fill(m_Root, 0, 0, m_Size, m_Size, PLATE_OFF);
		m_Plate = INT_VPPX_Look.Fill(m_Root, 0, 0, m_Size, m_Size, INT_VPPX_Look.ORANGE);
		m_Edge = INT_UIF_Draw.Frame(m_Root, 0, 0, m_Size, m_Size, "Edge");
		INT_VPPX_Look.Outline(m_Edge, 0, 0, m_Size, m_Size, INT_VPPX_Look.LINE_STRONG);
		float g = m_Size * 0.72;
		float go = (m_Size - g) * 0.5;
		m_Glyph = INT_VPPX_Look.IconAt(m_Root, go, go, g, "check", INT_VPPX_Look.INK);
		m_Hit = INT_UIF_Draw.Hit(m_Root, -4, -4, m_Size + 8, m_Size + 8, this);
		m_On = on;
		this.Paint();
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	bool IsOn()
	{
		return m_On;
	}

	void SetOn(bool on, bool notify)
	{
		if (on == m_On)
			return;
		m_On = on;
		this.Paint();
		if (notify)
		{
			INT_VPPX_Check keep = this;
			m_OnChanged.Invoke(m_On, keep);
		}
	}

	// Flips it and notifies (false while disabled).
	bool Toggle()
	{
		if (!m_Enabled)
			return false;
		bool next = true;
		if (m_On)
			next = false;
		this.SetOn(next, true);
		return true;
	}

	void SetEnabled(bool on)
	{
		m_Enabled = on;
		this.Paint();
	}

	bool IsEnabled()
	{
		return m_Enabled;
	}

	void Show(bool show)
	{
		if (!m_Root)
			return;
		if (m_Root.IsVisible() != show)
			m_Root.Show(show);
	}

	protected void Paint()
	{
		if (!m_Plate || !m_Edge || !m_Glyph)
			return;
		int plate = INT_VPPX_Look.ORANGE;
		int glyph = INT_VPPX_Look.INK;
		if (!m_Enabled)
		{
			plate = PLATE_DISABLED;
			glyph = INT_VPPX_Look.FAINT;
		}
		m_Plate.SetColor(plate);
		m_Plate.Show(m_On);
		m_Glyph.SetColor(glyph);
		m_Glyph.Show(m_On);
		bool edge = true;
		if (m_On)
			edge = false;
		m_Edge.Show(edge);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		this.Toggle();
		return true;
	}
}
