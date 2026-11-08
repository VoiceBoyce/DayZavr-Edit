// INT GlobalChat — the chat window chrome in the «Латунь» look (spec.md §5.1, design B). Built by the chat menu
// (interactive: hits, edit box) and by the view as a non-interactive preview while the editor is open.
// Geometry: the top-left corner is a screen fraction (INT_GC_Style.X/Y, -1 = default bottom-left), the size is in
// design px (scaled with the screen). Named parts are registered for the public API (INT_GC_UI.FindPart).

class INT_GC_Window
{
	static const float HEADER_H = 46;
	static const float INPUT_H = 44;
	static const float PAD = 10;
	static const float SCROLL_W = 4;
	static const float DEFAULT_LEFT = 24;
	static const float DEFAULT_BOTTOM = 120;

	protected Widget m_Parent;
	protected bool m_Interactive;
	protected ScriptedWidgetEventHandler m_Handler;
	protected float m_W;
	protected float m_H;

	Widget m_Root;
	ref INT_UIF_Surface m_Panel;
	Widget m_Header;
	ref INT_GC_Area m_Area;
	Widget m_InputFrame;
	ref INT_UIF_Surface m_InputBox;
	EditBoxWidget m_Edit;
	TextWidget m_Placeholder;
	TextWidget m_Counter;
	ImageWidget m_ScrollTrack;
	ImageWidget m_ScrollThumb;
	Widget m_NewChip;
	TextWidget m_NewText;
	Widget m_Hints;
	ref INT_UIF_Surface m_ChanGlobalBox;
	ref INT_UIF_Surface m_ChanLocalBox;
	TextWidget m_ChanGlobalText;
	TextWidget m_ChanLocalText;
	ImageWidget m_ChanGlobalIcon;
	ImageWidget m_ChanLocalIcon;
	ImageWidget m_EyeIcon;

	ButtonWidget m_HeaderHit;
	ButtonWidget m_ResizeHit;
	ButtonWidget m_GearHit;
	ButtonWidget m_EyeHit;
	ButtonWidget m_SendHit;
	ButtonWidget m_ChanGlobalHit;
	ButtonWidget m_ChanLocalHit;
	ButtonWidget m_LinesHit;
	ButtonWidget m_NewHit;

	protected ref map<string, Widget> m_Parts;

	void INT_GC_Window()
	{
		m_Parts = new map<string, Widget>();
	}

	void ~INT_GC_Window()
	{
		Destroy();
	}

	// --- geometry ---------------------------------------------------------------------------------------------
	static float W()
	{
		return INT_GC_UI.Style().W;
	}

	static float H()
	{
		return INT_GC_UI.Style().H;
	}

	// Screen px of the window's top-left corner (clamped to the screen).
	static void ScreenPos(out float sx, out float sy)
	{
		INT_GC_Style st = INT_GC_UI.Style();
		float sw = INT_UIF_Scale.ScreenW();
		float sh = INT_UIF_Scale.ScreenH();
		float w = INT_UIF_Draw.S(st.W);
		float h = INT_UIF_Draw.S(st.H);
		if (st.X < 0 || st.Y < 0)
		{
			sx = INT_UIF_Draw.S(DEFAULT_LEFT);
			sy = sh - h - INT_UIF_Draw.S(DEFAULT_BOTTOM);
		}
		else
		{
			sx = st.X * sw;
			sy = st.Y * sh;
		}
		float roomX = Math.Max(0, sw - w);
		float roomY = Math.Max(0, sh - h);
		sx = Math.Clamp(sx, 0, roomX);
		sy = Math.Clamp(sy, 0, roomY);
	}

	// Lines viewport inside the window, design px.
	static void LinesRect(float w, float h, out float x, out float y, out float lw, out float lh)
	{
		x = 14;
		y = HEADER_H + 8;
		lw = w - 14 - 22;
		lh = h - HEADER_H - 8 - INPUT_H - 18;
	}

	float GetW()
	{
		return m_W;
	}

	float GetH()
	{
		return m_H;
	}

	Widget FindPart(string name)
	{
		Widget w;
		if (m_Parts.Find(name, w))
			return w;
		return null;
	}

	protected void Part(string name, Widget w)
	{
		if (w)
			m_Parts.Set(name, w);
	}

	// --- build ------------------------------------------------------------------------------------------------
	void Build(Widget parent, bool interactive, ScriptedWidgetEventHandler handler)
	{
		Destroy();
		INT_GC_Look.EnsureLoaded();
		m_Parent = parent;
		m_Interactive = interactive;
		m_Handler = handler;
		m_W = W();
		m_H = H();
		float sx;
		float sy;
		ScreenPos(sx, sy);
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, m_W, m_H, "INT_GC_Window");
		if (!m_Root)
			return;
		m_Root.SetPos(sx, sy);
		Part("Window", m_Root);
		INT_GC_Style st = INT_GC_UI.Style();

		// panel: soft shadow, warm glass, brass hairline, header band, corner brackets
		INT_UIF_Surface.Shadow(m_Root, -40, -30, m_W + 80, m_H + 76, ARGB(150, 0, 0, 0));
		int panelCol = INT_GC_Look.Panel();
		int borderCol = INT_GC_Look.Border();
		m_Panel = INT_UIF_Surface.Box(m_Root, 0, 0, m_W, m_H, 8, panelCol, borderCol);
		if (m_Panel)
			Part("Panel", m_Panel.GetRoot());
		m_Header = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, HEADER_H, "Header");
		Part("Header", m_Header);
		int bandCol = INT_GC_Look.HeaderBand();
		INT_UIF_Draw.Image(m_Header, 1, 1, m_W * 0.7, HEADER_H - 2, "iuf_grad_h", bandCol);
		int lineCol = INT_GC_Look.BrassA(56);
		INT_UIF_Draw.Line(m_Header, 0, HEADER_H - 1, m_W, 1, lineCol);
		if (st.ShowCorners)
		{
			int cornerCol = INT_GC_Look.Brass();
			INT_GC_Look.Corners(m_Root, m_W, m_H, 16, cornerCol);
		}

		BuildHeader();
		float lx;
		float ly;
		float lw;
		float lh;
		LinesRect(m_W, m_H, lx, ly, lw, lh);
		// the wheel / click target lies UNDER the lines (frames are ignorepointer), so nick hits stay on top
		if (m_Interactive)
		{
			m_LinesHit = INT_UIF_Draw.Hit(m_Root, lx, ly, lw, lh, m_Handler);
			Part("LinesHit", m_LinesHit);
		}
		m_Area = new INT_GC_Area();
		ScriptedWidgetEventHandler hitHandler = null;
		if (m_Interactive)
			hitHandler = m_Handler;
		m_Area.Create(m_Root, lx, ly, lw, lh, false, hitHandler);
		Part("Lines", m_Area.GetRoot());
		BuildScroll(lx, ly, lw, lh);
		BuildNewChip(lx, ly, lw, lh);
		BuildInput();
		BuildResize();
		if (st.ShowHints && m_Interactive)
			BuildHints();
	}

	void Destroy()
	{
		if (m_Area)
			m_Area.Destroy();
		m_Area = null;
		if (m_Hints)
			m_Hints.Unlink();
		m_Hints = null;
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
		m_Parts.Clear();
		m_Edit = null;
		m_Panel = null;
		m_InputBox = null;
		m_ChanGlobalBox = null;
		m_ChanLocalBox = null;
	}

	protected void BuildHeader()
	{
		int brass = INT_GC_Look.Brass();
		int sand = INT_GC_Look.Text();
		// drag grip
		int gripCol = INT_GC_Look.BrassA(150);
		ImageWidget grip = INT_GC_Look.Cell(m_Header, 8, 14, 16, 18, "gc_grip", gripCol);
		Part("Grip", grip);
		// brass ring with the chat glyph
		Widget ring = INT_UIF_Draw.Frame(m_Header, 28, 9, 28, 28, "Ring");
		int ringFill = INT_GC_Look.BrassA(30);
		int ringLine = INT_GC_Look.BrassA(200);
		INT_UIF_Draw.Image(ring, 0, 0, 28, 28, "iuf_circle64", ringFill);
		INT_UIF_Draw.Image(ring, 0, 0, 28, 28, "iuf_ringline64", ringLine);
		int glyphCol = ARGB(255, 232, 200, 138);
		INT_GC_Look.Cell(ring, 7, 7, 14, 14, "gc_chat", glyphCol);
		// channel switch: two pills
		float px = 66;
		string gl = Widget.TranslateString("#STR_INT_GC_CH_GLOBAL");
		string lo = Widget.TranslateString("#STR_INT_GC_CH_LOCAL");
		float gw = INT_GC_Measure.Width(gl, 13) + 42;
		float lw = INT_GC_Measure.Width(lo, 13) + 42;
		Widget gf = INT_UIF_Draw.Frame(m_Header, px, 10, gw, 26, "ChannelGlobal");
		m_ChanGlobalBox = INT_UIF_Surface.Pill(gf, 0, 0, gw, 26, 0, 0);
		m_ChanGlobalIcon = INT_GC_Look.Cell(gf, 11, 6, 14, 14, "gc_global", sand);
		m_ChanGlobalText = INT_UIF_Draw.Text(gf, 30, 0, gw - 34, 26, gl, 13, sand);
		Part("ChannelGlobal", gf);
		px = px + gw + 6;
		Widget lf = INT_UIF_Draw.Frame(m_Header, px, 10, lw, 26, "ChannelLocal");
		m_ChanLocalBox = INT_UIF_Surface.Pill(lf, 0, 0, lw, 26, 0, 0);
		m_ChanLocalIcon = INT_GC_Look.Cell(lf, 11, 6, 14, 14, "gc_local", sand);
		m_ChanLocalText = INT_UIF_Draw.Text(lf, 30, 0, lw - 34, 26, lo, 13, sand);
		Part("ChannelLocal", lf);
		// right: editor (gear) and eye
		ImageWidget gearIcon;
		Widget gear = INT_GC_Look.RoundButton(m_Header, m_W - 76, 8, 30, "gc_gear", brass, "Gear", gearIcon);
		Part("Gear", gear);
		ImageWidget eyeIcon;
		Widget eye = INT_GC_Look.RoundButton(m_Header, m_W - 40, 8, 30, "gc_eye", brass, "Eye", eyeIcon);
		Part("Eye", eye);
		m_EyeIcon = eyeIcon;
		if (m_Interactive)
		{
			m_HeaderHit = INT_UIF_Draw.Hit(m_Header, 0, 0, m_W - 84, HEADER_H, m_Handler);
			m_ChanGlobalHit = INT_UIF_Draw.Hit(m_Header, 66, 10, gw, 26, m_Handler);
			m_ChanLocalHit = INT_UIF_Draw.Hit(m_Header, 66 + gw + 6, 10, lw, 26, m_Handler);
			m_GearHit = INT_UIF_Draw.Hit(m_Header, m_W - 76, 8, 30, 30, m_Handler);
			m_EyeHit = INT_UIF_Draw.Hit(m_Header, m_W - 40, 8, 30, 30, m_Handler);
			Part("HeaderHit", m_HeaderHit);
		}
		PaintChannel(INT_GC_UI.Settings().Channel);
		PaintEye(INT_GC_UI.IsHidden());
	}

	// Active channel: a tinted pill with a coloured rim and bright text (dark text on a bright pill drowns in the
	// anti-aliasing of thin SDF glyphs, measured 2026-10-20); inactive: a faint rim and muted text.
	void PaintChannel(int channel)
	{
		bool isGlobal = channel != INT_GC_Const.CH_LOCAL;
		int brass = INT_GC_Look.Brass();
		int localCol = INT_GC_Look.Local();
		int bright = ARGB(255, 250, 244, 232);
		int muted = INT_GC_Look.Muted();
		int idleBorder = INT_GC_Look.BrassA(70);
		PaintPill(m_ChanGlobalBox, m_ChanGlobalText, m_ChanGlobalIcon, isGlobal, brass, bright, muted, idleBorder);
		PaintPill(m_ChanLocalBox, m_ChanLocalText, m_ChanLocalIcon, !isGlobal, localCol, bright, muted, idleBorder);
		UpdatePlaceholder(channel);
	}

	protected void PaintPill(INT_UIF_Surface box, TextWidget text, ImageWidget icon, bool active, int accent, int bright, int muted, int idleBorder)
	{
		int fill = 0;
		int border = idleBorder;
		int textCol = muted;
		int iconCol = muted;
		if (active)
		{
			fill = INT_UIF_Color.WithAlpha(accent, 40);
			border = accent;
			textCol = bright;
			iconCol = accent;
		}
		if (box)
		{
			box.SetFill(fill);
			box.SetBorder(border);
		}
		if (text)
			text.SetColor(textCol);
		if (icon)
			icon.SetColor(iconCol);
	}

	void PaintEye(bool hidden)
	{
		if (!m_EyeIcon)
			return;
		string cell = "gc_eye";
		if (hidden)
			cell = "gc_eye_off";
		m_EyeIcon.LoadImageFile(0, INT_GC_Look.SET + cell);
		m_EyeIcon.SetImage(0);
	}

	protected void BuildScroll(float lx, float ly, float lw, float lh)
	{
		float sx = m_W - 14;
		int trackCol = INT_GC_Look.BrassA(30);
		int thumbCol = INT_GC_Look.BrassA(200);
		m_ScrollTrack = INT_UIF_Draw.Line(m_Root, sx, ly, SCROLL_W, lh, trackCol);
		m_ScrollThumb = INT_UIF_Draw.Line(m_Root, sx, ly + lh - 40, SCROLL_W, 40, thumbCol);
		Part("Scroll", m_ScrollTrack);
		Part("ScrollThumb", m_ScrollThumb);
	}

	protected void BuildNewChip(float lx, float ly, float lw, float lh)
	{
		float cw = 150;
		float ch = 24;
		m_NewChip = INT_UIF_Draw.Frame(m_Root, lx + (lw - cw) * 0.5, ly + lh - ch - 4, cw, ch, "NewChip");
		if (!m_NewChip)
			return;
		int brass = INT_GC_Look.Brass();
		int fill = ARGB(240, 46, 36, 20);
		INT_UIF_Surface.Pill(m_NewChip, 0, 0, cw, ch, fill, brass);
		INT_GC_Look.Cell(m_NewChip, 10, 6, 12, 12, "gc_down", brass);
		int bright = ARGB(255, 250, 244, 232);
		m_NewText = INT_UIF_Draw.Text(m_NewChip, 26, 0, cw - 32, ch, "", 12, bright, INT_UIF_Align.CENTER);
		if (m_Interactive)
			m_NewHit = INT_UIF_Draw.Hit(m_NewChip, 0, 0, cw, ch, m_Handler);
		m_NewChip.Show(false);
		Part("NewChip", m_NewChip);
	}

	protected void BuildInput()
	{
		float ix = PAD;
		float iy = m_H - INPUT_H - PAD;
		float iw = m_W - 2 * PAD;
		m_InputFrame = INT_UIF_Draw.Frame(m_Root, ix, iy, iw, INPUT_H, "Input");
		Part("Input", m_InputFrame);
		if (!m_InputFrame)
			return;
		int fill = INT_GC_Look.InputFill();
		int border = INT_GC_Look.BrassA(170);
		m_InputBox = INT_UIF_Surface.Box(m_InputFrame, 0, 0, iw, INPUT_H, 6, fill, border);
		float fs = INT_GC_UI.Style().FontSize;
		int muted = INT_GC_Look.Muted();
		m_Placeholder = INT_UIF_Draw.Text(m_InputFrame, 14, 0, iw - 120, INPUT_H, "", fs, muted);
		Part("Placeholder", m_Placeholder);
		if (m_Interactive)
		{
			m_Edit = INT_UIF_Draw.Edit(m_InputFrame, 14, 0, iw - 118, INPUT_H, fs);
			if (m_Edit)
			{
				m_Edit.SetHandler(m_Handler);
				m_Edit.SetColor(ARGB(255, 255, 255, 255));
				int textCol = ARGB(255, 255, 243, 223);
				m_Edit.SetTextColor(textCol);
				Part("Edit", m_Edit);
			}
		}
		m_Counter = INT_UIF_Draw.Text(m_InputFrame, iw - 100, 0, 56, INPUT_H, "", 11, muted, INT_UIF_Align.RIGHT);
		Part("Counter", m_Counter);
		int brass = INT_GC_Look.Brass();
		ImageWidget sendIcon;
		Widget send = INT_GC_Look.RoundButton(m_InputFrame, iw - 38, 6, 32, "gc_send", brass, "Send", sendIcon);
		Part("Send", send);
		if (m_Interactive)
			m_SendHit = INT_UIF_Draw.Hit(m_InputFrame, iw - 38, 6, 32, 32, m_Handler);
		UpdatePlaceholder(INT_GC_UI.Settings().Channel);
		SetCounter(0, INT_GC_Client.MaxLen());
	}

	protected void BuildResize()
	{
		int col = INT_GC_Look.BrassA(170);
		ImageWidget rz = INT_GC_Look.Cell(m_Root, m_W - 18, m_H - 18, 14, 14, "gc_resize", col);
		Part("Resize", rz);
		if (m_Interactive)
			m_ResizeHit = INT_UIF_Draw.Hit(m_Root, m_W - 24, m_H - 24, 24, 24, m_Handler);
	}

	// Key hints under the window (above it when there is no room below). Not a child of the window: a child past
	// its parent's rect is not drawn.
	protected void BuildHints()
	{
		m_Hints = INT_UIF_Draw.Frame(m_Parent, 0, 0, m_W, 22, "Hints");
		if (!m_Hints)
			return;
		Part("Hints", m_Hints);
		float x = 0;
		x = Hint(x, "ENTER", "#STR_INT_GC_HINT_SEND");
		x = Hint(x, "ESC", "#STR_INT_GC_HINT_CLOSE");
		x = Hint(x, "TAB", "#STR_INT_GC_HINT_CHANNEL");
		x = Hint(x, Widget.TranslateString("#STR_INT_GC_KEY_WHEEL"), "#STR_INT_GC_HINT_HISTORY");
		x = Hint(x, Widget.TranslateString("#STR_INT_GC_KEY_RMB"), "#STR_INT_GC_HINT_PLAYER");
		PlaceHints();
	}

	protected float Hint(float x, string key, string labelKey)
	{
		string label = Widget.TranslateString(labelKey);
		float kw = INT_GC_Measure.Width(key, 11) + 14;
		if (kw < 24)
			kw = 24;
		float lw = INT_GC_Measure.Width(label, 12) + 8;
		if (x + kw + lw > m_W + 120)
			return x;
		int capFill = ARGB(230, 24, 19, 13);
		int capBorder = INT_GC_Look.BrassA(120);
		INT_UIF_Surface.Box(m_Hints, x, 1, kw, 20, 4, capFill, capBorder);
		int keyCol = ARGB(255, 243, 234, 219);
		INT_UIF_Draw.Text(m_Hints, x, 1, kw, 20, key, 11, keyCol, INT_UIF_Align.CENTER);
		int labelCol = ARGB(255, 203, 185, 148);
		TextWidget lt = INT_UIF_Draw.Text(m_Hints, x + kw + 6, 0, lw, 22, label, 12, labelCol);
		if (lt)
			lt.SetShadow(2, ARGB(255, 0, 0, 0), 0.8, 1, 1);
		return x + kw + 6 + lw + 10;
	}

	void PlaceHints()
	{
		if (!m_Hints || !m_Root)
			return;
		float sx;
		float sy;
		m_Root.GetScreenPos(sx, sy);
		float sh = INT_UIF_Scale.ScreenH();
		float below = sy + INT_UIF_Draw.S(m_H + 8);
		float hh = INT_UIF_Draw.S(22);
		if (below + hh > sh)
			below = sy - hh - INT_UIF_Draw.S(8);
		m_Hints.SetPos(sx, below);
	}

	// --- state painting ---------------------------------------------------------------------------------------
	void UpdatePlaceholder(int channel)
	{
		if (!m_Placeholder)
			return;
		string key = "#STR_INT_GC_PH_GLOBAL";
		if (channel == INT_GC_Const.CH_LOCAL)
			key = "#STR_INT_GC_PH_LOCAL";
		if (INT_GC_Client.IsMuted())
			key = "#STR_INT_GC_PH_MUTED";
		m_Placeholder.SetText(Widget.TranslateString(key));
		RefreshPlaceholder();
	}

	void RefreshPlaceholder()
	{
		if (!m_Placeholder)
			return;
		bool empty = true;
		if (m_Edit && m_Edit.GetText() != "")
			empty = false;
		m_Placeholder.Show(empty);
	}

	void SetCounter(int used, int limit)
	{
		if (!m_Counter)
			return;
		m_Counter.SetText(used.ToString() + "/" + limit.ToString());
		int col = INT_GC_Look.Muted();
		if (used > limit - 20)
			col = INT_GC_Look.Notice(1);
		m_Counter.SetColor(col);
	}

	void SetFocusPaint(bool focused)
	{
		if (!m_InputBox)
			return;
		int border = INT_GC_Look.BrassA(170);
		if (focused)
			border = INT_GC_Look.Brass();
		m_InputBox.SetBorder(border);
	}

	// Scroll thumb + «N new» chip from the area state.
	void UpdateScroll()
	{
		if (!m_Area || !m_ScrollThumb || !m_ScrollTrack)
			return;
		float lx;
		float ly;
		float lw;
		float lh;
		LinesRect(m_W, m_H, lx, ly, lw, lh);
		int total = m_Area.TotalRows();
		int vis = m_Area.VisibleRows();
		bool scrollable = total > vis && vis > 0;
		m_ScrollTrack.Show(scrollable);
		m_ScrollThumb.Show(scrollable);
		if (scrollable)
		{
			float frac = vis;
			frac = frac / total;
			float th = Math.Max(24, lh * frac);
			float pos = m_Area.FirstVisibleEntry();
			float range = total - vis;
			pos = pos / Math.Max(1, range);
			float ty = ly + (lh - th) * Math.Clamp(pos, 0, 1);
			INT_UIF_Draw.Place(m_ScrollThumb, m_W - 14, ty, SCROLL_W, th);
		}
		int fresh = m_Area.NewBelow();
		if (m_NewChip)
		{
			bool showChip = m_Area.GetScroll() > 0 && fresh > 0;
			m_NewChip.Show(showChip);
			if (showChip && m_NewText)
				m_NewText.SetText(INT_GC_Client.Fmt1("#STR_INT_GC_NEW_LINES", fresh.ToString()));
		}
	}

	// Moves the window (screen px of its top-left corner) without a rebuild.
	void MoveTo(float sx, float sy)
	{
		if (!m_Root)
			return;
		m_Root.SetPos(sx, sy);
		PlaceHints();
	}

	bool ScreenRect(out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		if (!m_Root)
			return false;
		m_Root.GetScreenPos(x, y);
		m_Root.GetScreenSize(w, h);
		return true;
	}
}
