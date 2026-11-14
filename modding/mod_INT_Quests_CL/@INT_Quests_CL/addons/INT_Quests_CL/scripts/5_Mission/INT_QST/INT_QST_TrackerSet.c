// INT Quests (client) — journal tab «Настройки» → «Трекер заданий»: the player's size, transparency and place of
// the HUD tracker (owner 2026-10-22: J → Настройки → трекер). Left page: two sliders and a small screen where the
// tracker is dragged with the mouse; «Двигать в игре» opens the mover over the real game view and comes back here.
// Right page: the real tracker drawn at real size over a dark «game» panel (INT_QST_HudPreview). Every change goes
// through the public API (INT_QST_UI) and is saved in the client profile when the mouse is released.
// The mouse is polled from the journal's Update (no widget is unlinked inside its own event).

class INT_QST_SetSlider
{
	Widget m_Frame;
	string m_LabelKey;
	float m_W;
	float m_Min;
	float m_Max;
	float m_Step;
	float m_V;

	void Draw()
	{
		if (!m_Frame)
			return;
		INT_UIF_Widgets.ClearChildren(m_Frame);
		string cap = INT_QST_Look.Spaced(m_LabelKey);
		INT_QST_Look.Heavy(m_Frame, 0, 0, 300, 20, cap, 12, INT_QST_Look.Ink2());
		int vi = Math.Round(m_V);
		string val = vi.ToString() + " %";
		INT_QST_Look.Heavy(m_Frame, m_W - 120, -3, 120, 24, val, 16, INT_QST_Look.Ink(), INT_UIF_Align.RIGHT);
		float frac = 0;
		if (m_Max > m_Min)
			frac = (m_V - m_Min) / (m_Max - m_Min);
		frac = Math.Clamp(frac, 0, 1);
		int track = INT_QST_Look.InkA(45);
		INT_UIF_Surface.Box(m_Frame, 0, 31, m_W, 6, 3, track, 0);
		float fw = m_W * frac;
		if (fw >= 6)
			INT_UIF_Surface.Box(m_Frame, 0, 31, fw, 6, 3, INT_QST_Look.Red(), 0);
		int knobFill = INT_QST_Look.Note();
		int knobEdge = INT_QST_Look.Ink();
		INT_UIF_Surface.Box(m_Frame, fw - 10, 24, 20, 20, 10, knobFill, knobEdge);
		int minI = Math.Round(m_Min);
		int maxI = Math.Round(m_Max);
		string sMin = minI.ToString() + " %";
		string sMax = maxI.ToString() + " %";
		int ink3 = INT_QST_Look.Ink3();
		INT_QST_Look.Text(m_Frame, 0, 44, 120, 18, sMin, 11, ink3);
		INT_QST_Look.Text(m_Frame, m_W - 120, 44, 120, 18, sMax, 11, ink3, INT_UIF_Align.RIGHT);
	}

	// Mouse over the drag zone (the track with some room above and under it)?
	bool Hit(float mx, float my)
	{
		if (!m_Frame)
			return false;
		float x;
		float y;
		float w;
		float h;
		m_Frame.GetScreenPos(x, y);
		m_Frame.GetScreenSize(w, h);
		float pad = INT_UIF_Draw.S(12);
		float top = y + INT_UIF_Draw.S(16);
		float bottom = y + INT_UIF_Draw.S(52);
		return mx >= x - pad && mx <= x + w + pad && my >= top && my <= bottom;
	}

	// Value under the mouse x, snapped to the step.
	float ValueAt(float mx)
	{
		float x;
		float y;
		float w;
		float h;
		m_Frame.GetScreenPos(x, y);
		m_Frame.GetScreenSize(w, h);
		if (w <= 0)
			return m_V;
		float frac = Math.Clamp((mx - x) / w, 0, 1);
		float v = m_Min + frac * (m_Max - m_Min);
		if (m_Step > 0)
			v = Math.Round(v / m_Step) * m_Step;
		return Math.Clamp(v, m_Min, m_Max);
	}
}

class INT_QST_TrackerSet
{
	static const float LX = 32;
	static const float LW = 456;
	static const float MINI_H = 256;		// LW × MINI_H = 16:9

	static const int DRAG_NONE = 0;
	static const int DRAG_SIZE = 1;
	static const int DRAG_ALPHA = 2;
	static const int DRAG_PLACE = 3;

	protected Widget m_Page;
	protected ref INT_QST_SetSlider m_Size;
	protected ref INT_QST_SetSlider m_Alpha;
	protected Widget m_Screen;
	protected Widget m_Mark;
	protected TextWidget m_PlaceValue;
	protected Widget m_PrevHost;
	protected ref INT_QST_HudPreview m_Preview;
	protected float m_PrevW;
	protected float m_PrevH;
	protected int m_Drag;
	protected bool m_WasDown = true;		// the click that opened the tab must be released first
	protected float m_GrabX;
	protected float m_GrabY;
	protected bool m_Changed;

	void Build(Widget page, INT_QST_Clicks clicks, float bw, float bh, int idInGame, int idReset, int idShow)
	{
		Detach();
		m_Page = page;
		clicks.ForgetUnder(page);
		INT_UIF_Widgets.ClearChildren(page);
		m_Drag = DRAG_NONE;
		m_WasDown = true;
		INT_QST_Style st = INT_QST_UI.Style();
		// --- left page: the settings
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_H_SETTINGS");
		INT_QST_Look.Heavy(page, LX, 20, 400, 22, cap, 13, INT_QST_Look.Ink());
		INT_QST_Look.DoubleRule(page, LX, 49, LW);
		INT_QST_Look.Icon(page, LX, 68, 20, "pushpin", INT_QST_Look.Red());
		string title = INT_QST_Look.Tr("#STR_INT_QST_SET_TRACKER");
		INT_QST_Look.Heavy(page, LX + 30, 64, LW - 200, 26, title, 18, INT_QST_Look.Ink());
		// show / hide: the switch (the whole label + switch is the click target) and the key the player bound
		bool shown = INT_QST_UI.IsPlayerShown();
		Widget sw = INT_UIF_Draw.Frame(page, LX + LW - 170, 62, 170, 30, "ShowSwitch");
		Widget swHover = INT_UIF_Draw.Frame(sw, 0, 0, 170, 30, "Hover");
		INT_UIF_Draw.Line(swHover, 0, 0, 170, 30, 0x1A2A2218);
		string sl = INT_QST_Look.Tr("#STR_INT_QST_SET_SHOW");
		INT_QST_Look.Heavy(sw, 0, 2, 118, 26, sl, 14, INT_QST_Look.Ink2(), INT_UIF_Align.RIGHT);
		INT_QST_Look.Switch(sw, 128, 4, shown);
		ButtonWidget swHit = INT_UIF_Draw.Hit(sw, 0, 0, 170, 30, clicks);
		clicks.Bind(swHit, idShow, 0, swHover);
		string key = INT_QST_Book.KeyName("UAINTQuestsTracker", "");
		string about;
		if (key != "")
			about = INT_QST_Look.Fmt("#STR_INT_QST_SET_KEY", key);
		else
			about = INT_QST_Look.Tr("#STR_INT_QST_SET_NOKEY");
		about = INT_QST_Look.Fit(about, LW, 13);
		INT_QST_Look.Text(page, LX, 92, LW, 20, about, 13, INT_QST_Look.Ink3());
		m_Size = new INT_QST_SetSlider();
		m_Size.m_Frame = INT_UIF_Draw.Frame(page, LX, 124, LW, 62, "SizeSlider");
		m_Size.m_LabelKey = "#STR_INT_QST_SET_SIZE";
		m_Size.m_W = LW;
		m_Size.m_Min = 60;
		m_Size.m_Max = 160;
		m_Size.m_Step = 5;
		m_Size.m_V = st.Scale * 100;
		m_Size.Draw();
		m_Alpha = new INT_QST_SetSlider();
		m_Alpha.m_Frame = INT_UIF_Draw.Frame(page, LX, 194, LW, 62, "AlphaSlider");
		m_Alpha.m_LabelKey = "#STR_INT_QST_SET_ALPHA";
		m_Alpha.m_W = LW;
		m_Alpha.m_Min = 0;
		m_Alpha.m_Max = 80;
		m_Alpha.m_Step = 5;
		m_Alpha.m_V = (1.0 - st.Opacity) * 100;
		m_Alpha.Draw();
		string pc = INT_QST_Look.Spaced("#STR_INT_QST_SET_PLACE");
		INT_QST_Look.Heavy(page, LX, 266, 300, 20, pc, 12, INT_QST_Look.Ink2());
		m_PlaceValue = INT_QST_Look.Text(page, LX + LW - 220, 264, 220, 22, "", 13, INT_QST_Look.Ink3(), INT_UIF_Align.RIGHT);
		m_Screen = INT_UIF_Draw.Frame(page, LX, 292, LW, MINI_H, "MiniScreen");
		DrawScreen();
		Widget hv;
		int ic = INT_QST_Look.Ink();
		float bwid = (LW - 12) * 0.5;
		Widget b1 = INT_QST_Look.Button(page, LX, 564, bwid, 44, "#STR_INT_QST_SET_INGAME", "fit", INT_QST_Look.BTN_GHOST, ic, hv);
		ButtonWidget h1 = INT_UIF_Draw.Hit(b1, 0, 0, bwid, 44, clicks);
		clicks.Bind(h1, idInGame, 0, hv);
		Widget hv2;
		Widget b2 = INT_QST_Look.Button(page, LX + bwid + 12, 564, bwid, 44, "#STR_INT_QST_SET_RESET", "refresh", INT_QST_Look.BTN_GHOST, ic, hv2);
		ButtonWidget h2 = INT_UIF_Draw.Hit(b2, 0, 0, bwid, 44, clicks);
		clicks.Bind(h2, idReset, 0, hv2);
		// --- right page: the preview over a dark «game» panel
		float rx = INT_QST_Journal.FOLD + 60;
		float rw = bw - rx - 34;
		string pv = INT_QST_Look.Spaced("#STR_INT_QST_SET_PREVIEW");
		INT_QST_Look.Heavy(page, rx, 20, 400, 22, pv, 13, INT_QST_Look.Ink());
		INT_QST_Look.DoubleRule(page, rx, 49, rw);
		string ph = INT_QST_Look.Tr("#STR_INT_QST_SET_PREVIEW_HINT");
		ph = INT_QST_Look.Fit(ph, rw, 13);
		INT_QST_Look.Text(page, rx, 58, rw, 22, ph, 13, INT_QST_Look.Ink3());
		m_PrevW = rw;
		m_PrevH = bh - 92 - 18;
		INT_UIF_Surface.Box(page, rx, 92, rw, m_PrevH, 6, 0xFF2F3A44, INT_QST_Look.LeatherLine());
		INT_UIF_Draw.Line(page, rx + 3, 92 + m_PrevH * 0.62, rw - 6, m_PrevH * 0.38 - 3, 0xFF2A3424);
		m_PrevHost = INT_UIF_Draw.Frame(page, rx, 92, rw, m_PrevH, "PreviewHost");
		m_Preview = new INT_QST_HudPreview();
		m_Preview.Attach(m_PrevHost, 2);
		RebuildPreview();
		ShowPlace();
	}

	void Detach()
	{
		if (m_Preview)
			m_Preview.Detach();
		m_Preview = null;
		m_Page = null;
		m_Screen = null;
		m_Mark = null;
		m_PlaceValue = null;
		m_PrevHost = null;
	}

	// Window closes / tab changes: a drag in progress is saved.
	void Finish()
	{
		if (m_Changed)
			INT_QST_UI.SavePlayerLayout();
		m_Changed = false;
		m_Drag = DRAG_NONE;
	}

	// --- preview -----------------------------------------------------------------------------------------------
	protected void RebuildPreview()
	{
		if (!m_Preview || !m_PrevHost)
			return;
		m_Preview.Rebuild(2);
		float room = m_PrevH - INT_QST_Hud.PREVIEW_PAD * 2;
		if (m_Preview.BoxH() > room)
			m_Preview.Rebuild(1);		// a big scale: one quest fits the page
		float pw = m_Preview.BoxW();
		float x = (m_PrevW - pw) * 0.5;
		if (x < INT_QST_Hud.PREVIEW_PAD)
			x = INT_QST_Hud.PREVIEW_PAD;
		m_Preview.MoveTo(x, INT_QST_Hud.PREVIEW_PAD);
	}

	// --- the small screen ---------------------------------------------------------------------------------------
	protected void DrawScreen()
	{
		if (!m_Screen)
			return;
		INT_UIF_Widgets.ClearChildren(m_Screen);
		INT_UIF_Surface.Box(m_Screen, 0, 0, LW, MINI_H, 6, 0xFF2F3A44, INT_QST_Look.Ink());
		INT_UIF_Draw.Line(m_Screen, 2, MINI_H * 0.62, LW - 4, MINI_H * 0.38 - 2, 0xFF2A3424);
		int grid = 0x1AFFFFFF;
		INT_UIF_Draw.Line(m_Screen, LW / 3, 2, 1, MINI_H - 4, grid);
		INT_UIF_Draw.Line(m_Screen, LW * 2 / 3, 2, 1, MINI_H - 4, grid);
		INT_UIF_Draw.Line(m_Screen, 2, MINI_H / 3, LW - 4, 1, grid);
		INT_UIF_Draw.Line(m_Screen, 2, MINI_H * 2 / 3, LW - 4, 1, grid);
		m_Mark = INT_UIF_Draw.Frame(m_Screen, 0, 0, 40, 30, "Mark");
		ShowPlace();
	}

	// Tracker size as screen fractions (width from the style, height from the preview).
	protected void MarkSize(out float fw, out float fh)
	{
		INT_QST_Style st = INT_QST_UI.Style();
		float sw = INT_UIF_Scale.ScreenW();
		float sh = INT_UIF_Scale.ScreenH();
		fw = 0.2;
		fh = 0.2;
		if (sw <= 0 || sh <= 0)
			return;
		fw = INT_UIF_Draw.S(st.Width * st.Scale) / sw;
		float hd = 200 * st.Scale;
		if (m_Preview && m_Preview.BoxH() > 0)
			hd = m_Preview.BoxH();
		fh = INT_UIF_Draw.S(hd) / sh;
		fw = Math.Clamp(fw, 0.02, 1);
		fh = Math.Clamp(fh, 0.02, 1);
	}

	// Where the tracker's top-left corner is, as screen fractions.
	protected void CurrentPlace(float fw, out float fx, out float fy)
	{
		INT_QST_Style st = INT_QST_UI.Style();
		fx = st.FreeX;
		fy = st.FreeY;
		if (fx >= 0 && fy >= 0)
			return;
		float sw = INT_UIF_Scale.ScreenW();
		float sh = INT_UIF_Scale.ScreenH();
		if (sw <= 0 || sh <= 0)
		{
			fx = 0.7;
			fy = 0.1;
			return;
		}
		float x;
		float y;
		INT_QST_Hud.DefaultPlace(fw * sw, x, y);
		fx = x / sw;
		fy = y / sh;
	}

	protected void ShowPlace()
	{
		if (!m_Mark)
			return;
		float fw;
		float fh;
		MarkSize(fw, fh);
		float fx;
		float fy;
		CurrentPlace(fw, fx, fy);
		fx = Math.Clamp(fx, 0, Math.Max(0, 1 - fw));
		fy = Math.Clamp(fy, 0, Math.Max(0, 1 - fh));
		float mw = fw * LW;
		float mh = fh * MINI_H;
		INT_UIF_Draw.Place(m_Mark, fx * LW, fy * MINI_H, mw, mh);
		INT_UIF_Widgets.ClearChildren(m_Mark);
		INT_QST_Style st = INT_QST_UI.Style();
		int a = Math.Round(255 * st.Opacity);
		int edge = INT_QST_Look.Sticky();
		if (m_Drag == DRAG_PLACE)
			edge = INT_QST_Look.Red();
		if (st.Look == INT_QST_Style.LOOK_SHEET)
		{
			int paper = (INT_QST_Look.Paper() & 0x00FFFFFF) | (a << 24);
			int lea = (INT_QST_Look.Leather() & 0x00FFFFFF) | (a << 24);
			INT_UIF_Surface.Box(m_Mark, 0, 0, mw, mh, 3, paper, edge);
			float strip = mh * 0.16;
			if (strip < 4)
				strip = 4;
			INT_UIF_Draw.Line(m_Mark, 1, 1, mw - 2, strip, lea);
		}
		else
		{
			// text look: a frame with a few light «text lines»
			INT_UIF_Surface.Box(m_Mark, 0, 0, mw, mh, 3, 0x1AFFFFFF, edge);
			int ink = (st.TextColor & 0x00FFFFFF) | (a << 24);
			float ly = 5;
			int row = 0;
			while (ly + 3 < mh && row < 12)
			{
				float lw = mw - 10;
				if (row % 4 == 0)
					lw = mw * 0.7;
				INT_UIF_Draw.Line(m_Mark, 5, ly, lw, 2, ink);
				ly = ly + 6;
				if (row % 4 == 3)
					ly = ly + 4;
				row++;
			}
		}
		if (m_PlaceValue)
		{
			string key = "#STR_INT_QST_SET_PLACE_OWN";
			if (st.FreeX < 0 || st.FreeY < 0)
				key = "#STR_INT_QST_SET_PLACE_DEF";
			m_PlaceValue.SetText(INT_QST_Look.Tr(key));
		}
	}

	protected bool OverScreen(float mx, float my)
	{
		if (!m_Screen)
			return false;
		float x;
		float y;
		float w;
		float h;
		m_Screen.GetScreenPos(x, y);
		m_Screen.GetScreenSize(w, h);
		return mx >= x && mx <= x + w && my >= y && my <= y + h;
	}

	// Grab point inside the mark (screen px); a press outside the mark grabs its centre.
	protected void GrabPlace(float mx, float my)
	{
		float x;
		float y;
		float w;
		float h;
		m_Mark.GetScreenPos(x, y);
		m_Mark.GetScreenSize(w, h);
		if (mx >= x && mx <= x + w && my >= y && my <= y + h)
		{
			m_GrabX = mx - x;
			m_GrabY = my - y;
			return;
		}
		m_GrabX = w * 0.5;
		m_GrabY = h * 0.5;
	}

	protected void PlaceAt(float mx, float my)
	{
		float sx;
		float sy;
		float sw;
		float sh;
		m_Screen.GetScreenPos(sx, sy);
		m_Screen.GetScreenSize(sw, sh);
		if (sw <= 0 || sh <= 0)
			return;
		float fw;
		float fh;
		MarkSize(fw, fh);
		float nx = (mx - m_GrabX - sx) / sw;
		float ny = (my - m_GrabY - sy) / sh;
		nx = Math.Clamp(nx, 0, Math.Max(0, 1 - fw));
		ny = Math.Clamp(ny, 0, Math.Max(0, 1 - fh));
		SetPlace(nx, ny);
	}

	// --- changes (also used by the self-test) ------------------------------------------------------------------
	void SetSize(float pct)
	{
		if (!m_Size)
			return;
		float v = Math.Clamp(pct, m_Size.m_Min, m_Size.m_Max);
		if (Math.AbsFloat(v - m_Size.m_V) < 0.01)
			return;
		m_Size.m_V = v;
		m_Size.Draw();
		INT_QST_Style st = INT_QST_UI.Style();
		st.Scale = v / 100.0;
		INT_QST_UI.ApplyStyle();
		RebuildPreview();
		ShowPlace();
		m_Changed = true;
	}

	void SetTransparency(float pct)
	{
		if (!m_Alpha)
			return;
		float v = Math.Clamp(pct, m_Alpha.m_Min, m_Alpha.m_Max);
		if (Math.AbsFloat(v - m_Alpha.m_V) < 0.01)
			return;
		m_Alpha.m_V = v;
		m_Alpha.Draw();
		float op = 1.0 - v / 100.0;
		INT_QST_UI.SetOpacity(op);
		RebuildPreview();
		ShowPlace();
		m_Changed = true;
	}

	void SetPlace(float nx, float ny)
	{
		INT_QST_UI.SetFreePosition(nx, ny);
		ShowPlace();
		m_Changed = true;
	}

	// --- mouse (from the journal's Update) ---------------------------------------------------------------------
	void Update()
	{
		if (!m_Page)
			return;
		int mx;
		int my;
		GetMousePos(mx, my);
		bool down = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) != 0;
		if (down && !m_WasDown)
			StartDrag(mx, my);
		m_WasDown = down;
		if (m_Drag == DRAG_NONE)
			return;
		if (!down)
		{
			bool wasPlace = m_Drag == DRAG_PLACE;
			m_Drag = DRAG_NONE;
			if (wasPlace)
				ShowPlace();
			Finish();
			return;
		}
		Follow(mx, my);
	}

	protected void StartDrag(float mx, float my)
	{
		if (m_Size && m_Size.Hit(mx, my))
			m_Drag = DRAG_SIZE;
		else if (m_Alpha && m_Alpha.Hit(mx, my))
			m_Drag = DRAG_ALPHA;
		else if (OverScreen(mx, my))
		{
			m_Drag = DRAG_PLACE;
			GrabPlace(mx, my);
		}
		else
			return;
		Follow(mx, my);
	}

	protected void Follow(float mx, float my)
	{
		if (m_Drag == DRAG_SIZE)
		{
			float sv = m_Size.ValueAt(mx);
			SetSize(sv);
		}
		else if (m_Drag == DRAG_ALPHA)
		{
			float av = m_Alpha.ValueAt(mx);
			SetTransparency(av);
		}
		else if (m_Drag == DRAG_PLACE)
		{
			PlaceAt(mx, my);
		}
	}

	// --- self-test ----------------------------------------------------------------------------------------------
	string TestState()
	{
		INT_QST_Style st = INT_QST_UI.Style();
		int sc = Math.Round(st.Scale * 100);
		int op = Math.Round(st.Opacity * 100);
		string fx = st.FreeX.ToString();
		string fy = st.FreeY.ToString();
		float pw = 0;
		float ph = 0;
		if (m_Preview)
		{
			pw = m_Preview.BoxW();
			ph = m_Preview.BoxH();
		}
		return "scale " + sc.ToString() + " opacity " + op.ToString() + " place " + fx + " " + fy + " preview " + pw.ToString() + "x" + ph.ToString();
	}

	// Simulated mouse: a press/drag/release at given fractions of a control (0..1 across it).
	bool TestDragSlider(bool size, float frac)
	{
		INT_QST_SetSlider s = m_Alpha;
		if (size)
			s = m_Size;
		if (!s || !s.m_Frame)
			return false;
		float x;
		float y;
		float w;
		float h;
		s.m_Frame.GetScreenPos(x, y);
		s.m_Frame.GetScreenSize(w, h);
		float mx = x + w * frac;
		float my = y + INT_UIF_Draw.S(34);
		if (!s.Hit(mx, my))
			return false;
		StartDrag(mx, my);
		m_Drag = DRAG_NONE;
		Finish();
		return true;
	}

	bool TestDragMark(float nx, float ny)
	{
		if (!m_Screen || !m_Mark)
			return false;
		float x;
		float y;
		float w;
		float h;
		m_Mark.GetScreenPos(x, y);
		m_Mark.GetScreenSize(w, h);
		float cx = x + w * 0.5;
		float cy = y + h * 0.5;
		if (!OverScreen(cx, cy))
			return false;
		StartDrag(cx, cy);
		float sx;
		float sy;
		float sw;
		float sh;
		m_Screen.GetScreenPos(sx, sy);
		m_Screen.GetScreenSize(sw, sh);
		Follow(sx + nx * sw + m_GrabX, sy + ny * sh + m_GrabY);
		m_Drag = DRAG_NONE;
		ShowPlace();
		Finish();
		return true;
	}
}
