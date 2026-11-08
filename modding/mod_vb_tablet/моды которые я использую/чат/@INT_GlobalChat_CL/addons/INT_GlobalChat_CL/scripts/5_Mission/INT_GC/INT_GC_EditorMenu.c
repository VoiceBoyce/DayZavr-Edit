// INT GlobalChat — the chat editor (menu 9190202, owner brief p.9-11): nick colour (30 non-red colours), window
// size, font, opacity, fading, idle lines, time, sounds, blocked players, reset of the place. A live preview of the
// window stays on screen (view edit mode); it can be dragged by its header and resized by its corner here too.
// Clicks only queue an action; Update runs it (the editor rebuilds itself, never inside a widget handler).

class INT_GC_EditorHandler : ScriptedWidgetEventHandler
{
	protected INT_GC_EditorMenu m_Menu;

	void SetMenu(INT_GC_EditorMenu menu)
	{
		m_Menu = menu;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (m_Menu)
			return m_Menu.HandleClick(w);
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (m_Menu)
			m_Menu.HandleHover(w, true);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (m_Menu)
			m_Menu.HandleHover(w, false);
		return false;
	}
}

class INT_GC_EditorMenu : INT_UIF_Menu
{
	static const float PW = 660;
	static const float PH = 612;
	static const int BLOCK_PAGE = 4;

	protected static INT_GC_EditorMenu s_Instance;
	protected static bool s_ReopenChat;

	protected ref INT_GC_EditorHandler m_Hand;
	protected Widget m_Panel;
	protected ref map<Widget, string> m_Actions;
	protected ref map<Widget, ref INT_UIF_Surface> m_HoverBoxes;
	protected string m_Pending;
	protected int m_BlockPage;
	protected bool m_Frozen;

	protected int m_Drag;
	protected float m_GrabX;
	protected float m_GrabY;
	protected float m_StartW;
	protected float m_StartH;
	protected float m_StartMX;
	protected float m_StartMY;
	protected float m_LastResize;
	protected bool m_WasDown;

	static const int FADE_STEPS = 9;

	void INT_GC_EditorMenu()
	{
		m_Hand = new INT_GC_EditorHandler();
		m_Hand.SetMenu(this);
		m_Actions = new map<Widget, string>();
		m_HoverBoxes = new map<Widget, ref INT_UIF_Surface>();
	}

	void ~INT_GC_EditorMenu()
	{
		if (s_Instance == this)
			s_Instance = null;
		if (m_Frozen)
			INT_UIF_Menu.FreezePlayer(false);
	}

	static INT_GC_EditorMenu Instance()
	{
		return s_Instance;
	}

	// The mission reopens the chat window once the editor is gone.
	static bool TakeReopen()
	{
		bool r = s_ReopenChat;
		s_ReopenChat = false;
		return r;
	}

	override bool ShowWindow()
	{
		return false;
	}

	override bool FreezesPlayer()
	{
		return false;
	}

	override bool UsesBlur()
	{
		return false;
	}

	override void OnBuild(Widget body)
	{
		BuildPanel();
	}

	override void OnShow()
	{
		super.OnShow();
		s_Instance = this;
		m_WasDown = true;
		if (!m_Frozen)
		{
			INT_UIF_Menu.FreezePlayer(true);
			m_Frozen = true;
		}
		INT_GC_UI.SetEditMode(true);
	}

	override void OnHide()
	{
		super.OnHide();
		if (m_Frozen)
		{
			INT_UIF_Menu.FreezePlayer(false);
			m_Frozen = false;
		}
		INT_GC_UI.SetEditMode(false);
		INT_GC_UI.Style().Sanitize();
		INT_GC_UI.SavePlayerLayout();
		if (s_Instance == this)
			s_Instance = null;
		s_ReopenChat = true;
	}

	// --- layout -----------------------------------------------------------------------------------------------
	protected void PanelPos(out float px, out float py)
	{
		float f = INT_UIF_Scale.Factor();
		float scrW = INT_UIF_Scale.ScreenW();
		float scrH = INT_UIF_Scale.ScreenH();
		float wx;
		float wy;
		INT_GC_Window.ScreenPos(wx, wy);
		float ww = INT_UIF_Draw.S(INT_GC_UI.Style().W);
		float pw = INT_UIF_Draw.S(PW);
		float ph = INT_UIF_Draw.S(PH);
		float gap = INT_UIF_Draw.S(40);
		// on the side of the screen away from the chat window
		if (wx + ww * 0.5 < scrW * 0.5)
			px = scrW - pw - INT_UIF_Draw.S(60);
		else
			px = INT_UIF_Draw.S(60);
		if (px < wx + ww + gap && px + pw > wx - gap)
			px = (scrW - pw) * 0.5;
		py = (scrH - ph) * 0.5;
		if (px < 0)
			px = 0;
		if (py < 0)
			py = 0;
	}

	protected void BuildPanel()
	{
		if (m_Panel)
			m_Panel.Unlink();
		m_Actions.Clear();
		m_HoverBoxes.Clear();
		INT_GC_Look.EnsureLoaded();
		float px;
		float py;
		PanelPos(px, py);
		m_Panel = INT_UIF_Draw.Frame(GetLayoutRoot(), 0, 0, PW, PH, "INT_GC_Editor");
		if (!m_Panel)
			return;
		m_Panel.SetPos(px, py);
		INT_UIF_Surface.Shadow(m_Panel, -50, -40, PW + 100, PH + 96, ARGB(170, 0, 0, 0));
		int fill = ARGB(246, 24, 19, 13);
		int border = INT_GC_Look.BrassA(110);
		INT_UIF_Surface.Box(m_Panel, 0, 0, PW, PH, 8, fill, border);
		int band = INT_GC_Look.BrassA(36);
		INT_UIF_Draw.Image(m_Panel, 1, 1, PW * 0.7, 50, "iuf_grad_h", band);
		int brass = INT_GC_Look.Brass();
		INT_GC_Look.Corners(m_Panel, PW, PH, 16, brass);
		int head = ARGB(255, 243, 234, 219);
		INT_GC_Look.Cell(m_Panel, 18, 16, 18, 18, "gc_palette", brass);
		TextWidget title = INT_UIF_Draw.Text(m_Panel, 44, 0, 400, 50, "#STR_INT_GC_ED_TITLE", 16, head);
		if (title)
			title.SetText(Widget.TranslateString("#STR_INT_GC_ED_TITLE"));
		int lineCol = INT_GC_Look.BrassA(56);
		INT_UIF_Draw.Line(m_Panel, 0, 49, PW, 1, lineCol);
		ImageWidget closeIcon;
		Widget close = INT_GC_Look.RoundButton(m_Panel, PW - 44, 10, 30, "gc_close", brass, "Close", closeIcon);
		AddHit(close, PW - 44, 10, 30, 30, "close", null);

		float y = 62;
		y = BuildPalette(y);
		y = BuildSize(y);
		y = BuildToggles(y);
		y = BuildBlocked(y);
		BuildFooter();
	}

	protected ButtonWidget AddHit(Widget parent, float x, float y, float w, float h, string action, INT_UIF_Surface hover)
	{
		ButtonWidget hit = INT_UIF_Draw.Hit(m_Panel, x, y, w, h, m_Hand);
		if (!hit)
			return null;
		m_Actions.Set(hit, action);
		if (hover)
			m_HoverBoxes.Set(hit, hover);
		return hit;
	}

	protected void Caption(float x, float y, string key)
	{
		int col = INT_GC_Look.Brass();
		string t = Widget.TranslateString(key);
		INT_UIF_Draw.Text(m_Panel, x, y, 600, 16, t, 11, col);
	}

	protected float BuildPalette(float y)
	{
		Caption(20, y, "#STR_INT_GC_ED_COLOR");
		float top = y + 22;
		int sel = INT_GC_UI.Settings().ColorIdx;
		int n = INT_GC_Palette.Count();
		float size = 30;
		float gapX = 9.4;
		float gapY = 9;
		for (int i = 0; i < n; i++)
		{
			int colIdx = i;
			int rowIdx = 0;
			if (i >= 15)
			{
				colIdx = i - 15;
				rowIdx = 1;
			}
			float cx = 20 + colIdx * (size + gapX);
			float cy = top + rowIdx * (size + gapY);
			int c = INT_GC_Palette.Color(i);
			if (i == sel)
			{
				int selRing = INT_GC_Look.Brass();
				INT_UIF_Draw.Image(m_Panel, cx - 4, cy - 4, size + 8, size + 8, "iuf_ringline64", selRing);
			}
			INT_UIF_Draw.Image(m_Panel, cx, cy, size, size, "iuf_circle64", c);
			if (i == sel)
			{
				int dark = INT_GC_Look.Dark();
				INT_GC_Look.Cell(m_Panel, cx + 8, cy + 8, 14, 14, "gc_check", dark);
			}
			AddHit(m_Panel, cx, cy, size, size, "color:" + i.ToString(), null);
		}
		float py = top + 2 * (size + gapY) + 2;
		string me = INT_GC_Client.MyName();
		if (me == "")
			me = "Survivor";
		int muted = INT_GC_Look.Muted();
		string seen = Widget.TranslateString("#STR_INT_GC_ED_SEEN");
		float sw = INT_GC_Measure.Width(seen, 13);
		INT_UIF_Draw.Text(m_Panel, 20, py, sw + 6, 22, seen, 13, muted);
		int nickCol = INT_GC_Palette.Color(sel);
		string nick = me + ":";
		float nw = INT_GC_Measure.Width(nick, 14);
		INT_UIF_Draw.Text(m_Panel, 26 + sw, py, nw + 6, 22, nick, 14, nickCol);
		int textCol = INT_GC_Look.Text();
		string hello = Widget.TranslateString("#STR_INT_GC_ED_HELLO");
		INT_UIF_Draw.Text(m_Panel, 30 + sw + nw, py, 300, 22, hello, 14, textCol);
		string cname = Widget.TranslateString(INT_GC_Palette.NameKey(sel));
		INT_UIF_Draw.Text(m_Panel, PW - 220, py, 200, 22, cname, 12, nickCol, INT_UIF_Align.RIGHT);
		float lineY = py + 30;
		int lineCol = INT_GC_Look.BrassA(36);
		INT_UIF_Draw.Line(m_Panel, 0, lineY, PW, 1, lineCol);
		return lineY + 12;
	}

	protected float BuildSize(float y)
	{
		INT_GC_Style st = INT_GC_UI.Style();
		Caption(20, y, "#STR_INT_GC_ED_SIZE");
		Caption(340, y, "#STR_INT_GC_ED_LOOK");
		float r = y + 22;
		int wv = Math.Round(st.W);
		int hv = Math.Round(st.H);
		int fv = Math.Round(st.FontSize);
		int lv = st.IdleLines;
		Stepper(20, r, "#STR_INT_GC_ED_WIDTH", wv.ToString(), "w");
		Stepper(20, r + 34, "#STR_INT_GC_ED_HEIGHT", hv.ToString(), "h");
		Stepper(20, r + 68, "#STR_INT_GC_ED_FONT", fv.ToString(), "font");
		Stepper(20, r + 102, "#STR_INT_GC_ED_LINES", lv.ToString(), "lines");
		int op = Math.Round(st.Opacity * 100);
		Stepper(340, r, "#STR_INT_GC_ED_OPACITY", op.ToString() + "%", "opacity");
		string fade = Widget.TranslateString("#STR_INT_GC_ED_NEVER");
		if (st.FadeSec > 0)
			fade = INT_GC_Client.Fmt1("#STR_INT_GC_ED_SEC", st.FadeSec.ToString());
		Stepper(340, r + 34, "#STR_INT_GC_ED_FADE", fade, "fade");
		string hint = Widget.TranslateString("#STR_INT_GC_ED_DRAG_HINT");
		int muted = INT_GC_Look.Muted();
		MultilineTextWidget mt = INT_UIF_Draw.MText(m_Panel, 340, r + 70, 300, 60, hint, 12, muted);
		float lineY = r + 136;
		int lineCol = INT_GC_Look.BrassA(36);
		INT_UIF_Draw.Line(m_Panel, 0, lineY, PW, 1, lineCol);
		return lineY + 12;
	}

	protected void Stepper(float x, float y, string labelKey, string value, string id)
	{
		int text = INT_GC_Look.Text();
		string label = Widget.TranslateString(labelKey);
		INT_UIF_Draw.Text(m_Panel, x, y, 150, 28, label, 13, text);
		float bx = x + 160;
		int border = INT_GC_Look.BrassA(110);
		int brass = INT_GC_Look.Brass();
		INT_UIF_Surface minus = INT_UIF_Surface.Pill(m_Panel, bx, y + 1, 26, 26, 0, border);
		INT_UIF_Draw.Text(m_Panel, bx, y + 1, 26, 26, "-", 16, brass, INT_UIF_Align.CENTER);
		AddHit(m_Panel, bx, y + 1, 26, 26, id + ":-", minus);
		int valFill = INT_GC_Look.BrassA(20);
		INT_UIF_Surface.Box(m_Panel, bx + 30, y + 1, 70, 26, 4, valFill, border);
		int head = ARGB(255, 243, 234, 219);
		INT_UIF_Draw.Text(m_Panel, bx + 30, y + 1, 70, 26, value, 13, head, INT_UIF_Align.CENTER);
		INT_UIF_Surface plus = INT_UIF_Surface.Pill(m_Panel, bx + 104, y + 1, 26, 26, 0, border);
		INT_UIF_Draw.Text(m_Panel, bx + 104, y + 1, 26, 26, "+", 16, brass, INT_UIF_Align.CENTER);
		AddHit(m_Panel, bx + 104, y + 1, 26, 26, id + ":+", plus);
	}

	protected float BuildToggles(float y)
	{
		INT_GC_Style st = INT_GC_UI.Style();
		INT_GC_Settings se = INT_GC_UI.Settings();
		float x = 20;
		x = Toggle(x, y, "#STR_INT_GC_ED_TIME", st.ShowTime, "time");
		x = Toggle(x, y, "#STR_INT_GC_ED_SOUND", se.Sound, "sound");
		x = Toggle(x, y, "#STR_INT_GC_ED_MENTION", se.MentionSound, "mention");
		float lineY = y + 40;
		int lineCol = INT_GC_Look.BrassA(36);
		INT_UIF_Draw.Line(m_Panel, 0, lineY, PW, 1, lineCol);
		return lineY + 12;
	}

	protected float Toggle(float x, float y, string labelKey, bool on, string id)
	{
		string label = Widget.TranslateString(labelKey);
		float w = INT_GC_Measure.Width(label, 13) + 48;
		// on: tinted pill, brass rim and check, bright text (dark text on brass fades, measured 2026-10-20)
		int brass = INT_GC_Look.Brass();
		int muted = INT_GC_Look.Muted();
		int bright = ARGB(255, 250, 244, 232);
		int border = INT_GC_Look.BrassA(80);
		int fill = 0;
		int col = muted;
		int iconCol = muted;
		string icon = "gc_close";
		if (on)
		{
			fill = INT_GC_Look.BrassA(44);
			border = brass;
			col = bright;
			iconCol = brass;
			icon = "gc_check";
		}
		INT_UIF_Surface box = INT_UIF_Surface.Pill(m_Panel, x, y, w, 30, fill, border);
		INT_GC_Look.Cell(m_Panel, x + 12, y + 8, 14, 14, icon, iconCol);
		INT_UIF_Draw.Text(m_Panel, x + 32, y, w - 36, 30, label, 13, col);
		INT_UIF_Surface hover = null;
		if (!on)
			hover = box;
		AddHit(m_Panel, x, y, w, 30, "toggle:" + id, hover);
		return x + w + 10;
	}

	protected float BuildBlocked(float y)
	{
		array<ref INT_GC_BlockEntry> list = INT_GC_UI.Settings().Blocked;
		int n = list.Count();
		string cap = INT_GC_Client.Fmt1("#STR_INT_GC_ED_BLOCKED", n.ToString());
		int col = INT_GC_Look.Brass();
		INT_UIF_Draw.Text(m_Panel, 20, y, 400, 16, cap, 11, col);
		float top = y + 22;
		int pages = (n + BLOCK_PAGE - 1) / BLOCK_PAGE;
		if (pages < 1)
			pages = 1;
		if (m_BlockPage >= pages)
			m_BlockPage = pages - 1;
		if (m_BlockPage < 0)
			m_BlockPage = 0;
		int muted = INT_GC_Look.Muted();
		if (n == 0)
		{
			string none = Widget.TranslateString("#STR_INT_GC_ED_BLOCKED_NONE");
			INT_UIF_Draw.Text(m_Panel, 20, top, 600, 30, none, 13, muted);
		}
		int from = m_BlockPage * BLOCK_PAGE;
		int text = INT_GC_Look.Text();
		int rowFill = INT_GC_Look.BrassA(16);
		int border = INT_GC_Look.BrassA(90);
		string unblock = Widget.TranslateString("#STR_INT_GC_ED_UNBLOCK");
		float ubw = INT_GC_Measure.Width(unblock, 12) + 24;
		for (int i = 0; i < BLOCK_PAGE; i++)
		{
			int idx = from + i;
			if (idx >= n)
				break;
			INT_GC_BlockEntry e = list.Get(idx);
			float ry = top + i * 34;
			INT_UIF_Surface.Box(m_Panel, 20, ry, PW - 40, 30, 4, rowFill, 0);
			INT_GC_Look.Cell(m_Panel, 30, ry + 8, 14, 14, "gc_user", muted);
			INT_UIF_Draw.Text(m_Panel, 52, ry, 380, 30, e.Name, 14, text);
			INT_UIF_Surface ub = INT_UIF_Surface.Pill(m_Panel, PW - 30 - ubw, ry + 3, ubw, 24, 0, border);
			INT_UIF_Draw.Text(m_Panel, PW - 30 - ubw, ry + 3, ubw, 24, unblock, 12, col, INT_UIF_Align.CENTER);
			AddHit(m_Panel, PW - 30 - ubw, ry + 3, ubw, 24, "unblock:" + idx.ToString(), ub);
		}
		float pagerY = top + BLOCK_PAGE * 34;
		if (pages > 1)
		{
			int pageNo = m_BlockPage + 1;
			string pageText = pageNo.ToString() + " / " + pages.ToString();
			INT_UIF_Draw.Text(m_Panel, PW * 0.5 - 40, pagerY, 80, 24, pageText, 12, muted, INT_UIF_Align.CENTER);
			INT_UIF_Surface prev = INT_UIF_Surface.Pill(m_Panel, PW * 0.5 - 76, pagerY, 30, 24, 0, border);
			INT_UIF_Draw.Text(m_Panel, PW * 0.5 - 76, pagerY, 30, 24, "<", 13, col, INT_UIF_Align.CENTER);
			AddHit(m_Panel, PW * 0.5 - 76, pagerY, 30, 24, "page:-", prev);
			INT_UIF_Surface next = INT_UIF_Surface.Pill(m_Panel, PW * 0.5 + 46, pagerY, 30, 24, 0, border);
			INT_UIF_Draw.Text(m_Panel, PW * 0.5 + 46, pagerY, 30, 24, ">", 13, col, INT_UIF_Align.CENTER);
			AddHit(m_Panel, PW * 0.5 + 46, pagerY, 30, 24, "page:+", next);
		}
		return pagerY + 30;
	}

	protected void BuildFooter()
	{
		float fy = PH - 52;
		int lineCol = INT_GC_Look.BrassA(36);
		INT_UIF_Draw.Line(m_Panel, 0, fy - 10, PW, 1, lineCol);
		int border = INT_GC_Look.BrassA(110);
		int text = INT_GC_Look.Text();
		string reset = Widget.TranslateString("#STR_INT_GC_ED_RESET");
		float rw = INT_GC_Measure.Width(reset, 13) + 56;
		INT_UIF_Surface rb = INT_UIF_Surface.Box(m_Panel, 20, fy, rw, 36, 4, 0, border);
		INT_GC_Look.Cell(m_Panel, 32, fy + 11, 14, 14, "gc_reset", text);
		INT_UIF_Draw.Text(m_Panel, 52, fy, rw - 24, 36, reset, 13, text);
		AddHit(m_Panel, 20, fy, rw, 36, "reset", rb);
		string done = Widget.TranslateString("#STR_INT_GC_ED_DONE");
		float dw = INT_GC_Measure.Width(done, 14) + 50;
		int brass = INT_GC_Look.Brass();
		int doneFill = INT_GC_Look.BrassA(70);
		int bright = ARGB(255, 255, 248, 236);
		INT_UIF_Surface.Box(m_Panel, PW - 20 - dw, fy, dw, 36, 4, doneFill, brass);
		INT_UIF_Draw.Text(m_Panel, PW - 20 - dw, fy, dw, 36, done, 14, bright, INT_UIF_Align.CENTER);
		AddHit(m_Panel, PW - 20 - dw, fy, dw, 36, "done", null);
	}

	// --- input --------------------------------------------------------------------------------------------------
	bool HandleClick(Widget w)
	{
		string act;
		if (!w || !m_Actions.Find(w, act))
			return false;
		m_Pending = act;
		return true;
	}

	void HandleHover(Widget w, bool enter)
	{
		INT_UIF_Surface box;
		if (!w || !m_HoverBoxes.Find(w, box) || !box)
			return;
		int fill = 0;
		if (enter)
			fill = INT_GC_Look.BrassA(40);
		box.SetFill(fill);
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		string act = m_Pending;
		m_Pending = "";
		if (act != "")
			Run(act);
		PollPreviewDrag();
	}

	protected void Run(string act)
	{
		INT_GC_Sfx.Play(INT_GC_Sfx.CLICK);
		INT_GC_Style st = INT_GC_UI.Style();
		INT_GC_Settings se = INT_GC_UI.Settings();
		if (act == "close" || act == "done")
		{
			RequestClose();
			return;
		}
		if (act == "reset")
		{
			INT_GC_UI.ResetPosition();
			st.FontSize = 16;
			st.Opacity = 0.88;
			st.FadeSec = 15;
			st.IdleLines = 8;
			ApplyAndRebuild();
			return;
		}
		array<string> parts = new array<string>();
		act.Split(":", parts);
		if (parts.Count() < 2)
			return;
		string kind = parts.Get(0);
		string arg = parts.Get(1);
		bool plus = arg == "+";
		if (kind == "color")
		{
			INT_GC_UI.SetColorIdx(arg.ToInt());
			ApplyAndRebuild();
			return;
		}
		if (kind == "w")
			st.W = StepValue(st.W, 20, plus);
		else if (kind == "h")
			st.H = StepValue(st.H, 20, plus);
		else if (kind == "font")
			st.FontSize = StepValue(st.FontSize, 1, plus);
		else if (kind == "lines")
			st.IdleLines = StepValue(st.IdleLines, 1, plus);
		else if (kind == "opacity")
			st.Opacity = StepValue(st.Opacity, 0.05, plus);
		else if (kind == "fade")
			st.FadeSec = NextFade(st.FadeSec, plus);
		else if (kind == "toggle")
			RunToggle(arg);
		else if (kind == "unblock")
			RunUnblock(arg.ToInt());
		else if (kind == "page")
		{
			if (plus)
				m_BlockPage++;
			else
				m_BlockPage--;
		}
		ApplyAndRebuild();
	}

	protected float StepValue(float v, float step, bool plus)
	{
		if (plus)
			return v + step;
		return v - step;
	}

	protected int NextFade(int cur, bool plus)
	{
		array<int> steps = {0, 5, 10, 15, 20, 30, 45, 60, 120};
		int idx = steps.Find(cur);
		if (idx < 0)
			idx = 3;
		if (plus)
			idx++;
		else
			idx--;
		idx = Math.Clamp(idx, 0, steps.Count() - 1);
		return steps.Get(idx);
	}

	protected void RunToggle(string id)
	{
		INT_GC_Style st = INT_GC_UI.Style();
		INT_GC_Settings se = INT_GC_UI.Settings();
		if (id == "time")
			st.ShowTime = !st.ShowTime;
		else if (id == "sound")
			se.Sound = !se.Sound;
		else if (id == "mention")
			se.MentionSound = !se.MentionSound;
	}

	protected void RunUnblock(int idx)
	{
		array<ref INT_GC_BlockEntry> list = INT_GC_UI.Settings().Blocked;
		if (idx < 0 || idx >= list.Count())
			return;
		INT_GC_BlockEntry e = list.Get(idx);
		string id = e.Id;
		string name = e.Name;
		INT_GC_UI.Unblock(id, name);
		INT_GC_Client.Notice(INT_GC_Client.Fmt1("#STR_INT_GC_UNBLOCKED", name), 0);
	}

	protected void ApplyAndRebuild()
	{
		INT_GC_UI.ApplyStyle();
		INT_GC_UI.SaveClient();
		BuildPanel();
	}

	// --- drag / resize of the preview window ----------------------------------------------------------------
	protected void PollPreviewDrag()
	{
		bool down = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) != 0;
		int mx;
		int my;
		GetMousePos(mx, my);
		if (m_Drag == 0 && down && !m_WasDown)
			TryGrab(mx, my);
		else if (m_Drag != 0 && down)
			Follow(mx, my);
		else if (m_Drag != 0 && !down)
			Release();
		m_WasDown = down;
	}

	protected bool Inside(float mx, float my, float x, float y, float w, float h)
	{
		return mx >= x && my >= y && mx <= x + w && my <= y + h;
	}

	protected void TryGrab(int mx, int my)
	{
		if (!INT_GC_UI.IsPlayerMoveEnabled())
			return;
		if (m_Panel)
		{
			float ex;
			float ey;
			float ew;
			float eh;
			m_Panel.GetScreenPos(ex, ey);
			m_Panel.GetScreenSize(ew, eh);
			if (Inside(mx, my, ex, ey, ew, eh))
				return;
		}
		float x;
		float y;
		float w;
		float h;
		if (!INT_GC_UI.GetScreenRect(x, y, w, h))
			return;
		float corner = INT_UIF_Draw.S(28);
		float header = INT_UIF_Draw.S(INT_GC_Window.HEADER_H);
		if (Inside(mx, my, x + w - corner, y + h - corner, corner, corner))
		{
			m_Drag = 2;
		}
		else if (Inside(mx, my, x, y, w, header))
		{
			m_Drag = 1;
		}
		else
		{
			return;
		}
		m_GrabX = mx - x;
		m_GrabY = my - y;
		m_StartMX = mx;
		m_StartMY = my;
		m_StartW = INT_GC_UI.Style().W;
		m_StartH = INT_GC_UI.Style().H;
		m_LastResize = 0;
		INT_GC_Style st = INT_GC_UI.Style();
		float scrW = INT_UIF_Scale.ScreenW();
		float scrH = INT_UIF_Scale.ScreenH();
		st.X = x / scrW;
		st.Y = y / scrH;
	}

	protected void Follow(int mx, int my)
	{
		INT_GC_Style st = INT_GC_UI.Style();
		float scrW = INT_UIF_Scale.ScreenW();
		float scrH = INT_UIF_Scale.ScreenH();
		float now = GetGame().GetTickTime();
		INT_GC_View view = INT_GC_View.Peek();
		if (m_Drag == 1)
		{
			float w = INT_UIF_Draw.S(st.W);
			float h = INT_UIF_Draw.S(st.H);
			float roomX = Math.Max(0, scrW - w);
			float roomY = Math.Max(0, scrH - h);
			float nx = Math.Clamp(mx - m_GrabX, 0, roomX);
			float ny = Math.Clamp(my - m_GrabY, 0, roomY);
			st.X = nx / scrW;
			st.Y = ny / scrH;
			if (view && view.GetPreview())
				view.GetPreview().MoveTo(nx, ny);
			return;
		}
		if (now - m_LastResize < 0.08)
			return;
		float f = INT_UIF_Scale.Factor();
		float dw = (mx - m_StartMX) / f;
		float dh = (my - m_StartMY) / f;
		float nw = Math.Clamp(m_StartW + dw, INT_GC_Style.MIN_W, INT_GC_Style.MAX_W);
		float nh = Math.Clamp(m_StartH + dh, INT_GC_Style.MIN_H, INT_GC_Style.MAX_H);
		if (Math.AbsFloat(nw - st.W) < 1 && Math.AbsFloat(nh - st.H) < 1)
			return;
		m_LastResize = now;
		st.W = Math.Round(nw);
		st.H = Math.Round(nh);
		INT_GC_UI.ApplyStyle();
	}

	protected void Release()
	{
		int mode = m_Drag;
		m_Drag = 0;
		INT_GC_UI.Style().Sanitize();
		INT_GC_UI.ApplyStyle();
		INT_GC_UI.SavePlayerLayout();
		INT_GC_UI.OnPlaced().Invoke();
		if (mode == 2)
			BuildPanel();
	}

	// self-test
	void TestRun(string act)
	{
		Run(act);
	}
}
