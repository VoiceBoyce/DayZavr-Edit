// INT Quests (client) — the player moves the tracker with the mouse (INT_KillFeed mover pattern). Opened from the
// journal (Настройки → «Двигать в игре») or by INT_QST_UI.StartPlayerMove(). A window-less framework menu: cursor
// on, the player stands still, the tracker shows (samples when nothing is tracked) inside a frame. Drag with the
// left button; − / + change the size; «Сохранить» / Esc keeps the place in the client profile, «Сброс» puts it back
// top-right, «Отмена» restores the place it had before. Opened from the journal settings, it goes back there.

class INT_QST_MoverMenu extends INT_UIF_Menu
{
	static const float BAR_W = 760;
	static const float BAR_H = 70;

	static const int ID_SAVE = 1;
	static const int ID_RESET = 2;
	static const int ID_CANCEL = 3;
	static const int ID_SMALLER = 4;
	static const int ID_BIGGER = 5;

	protected static INT_QST_MoverMenu s_Open;
	// back to the journal settings: 1 asked by the journal, 2 the mover is open, 3 closed -> open the journal
	protected static int s_Return;
	protected float m_StartOpacity;
	protected ref INT_QST_Clicks m_Clicks;
	protected Widget m_Bar;
	protected bool m_Drag;
	protected bool m_WasDown;
	protected float m_GrabX;
	protected float m_GrabY;
	protected bool m_Finished;
	protected float m_StartX;
	protected float m_StartY;
	protected float m_StartScale;
	protected int m_Pending;

	void INT_QST_MoverMenu()
	{
		m_Clicks = new INT_QST_Clicks();
		m_Clicks.Event_Click.Insert(OnClickId);
	}

	override bool ShowWindow()
	{
		return false;
	}

	override bool FreezesPlayer()
	{
		return false;		// frozen in OnShow without hiding the HUD (the tracker must stay visible)
	}

	override bool UsesBlur()
	{
		return false;
	}

	override bool CloseOnEscape()
	{
		return false;
	}

	static bool IsOpen()
	{
		return s_Open != null;
	}

	static INT_QST_MoverMenu GetOpen()
	{
		return s_Open;
	}

	// The journal settings open the mover and want to come back once it closes.
	static void ReturnToJournal()
	{
		s_Return = 1;
	}

	// From MissionGameplay.OnUpdate: opens once no other menu is open.
	static void Poll()
	{
		UIManager um = GetGame().GetUIManager();
		bool busy = false;		// if-chain: `x = (a && b.Call())` is a measured heap trap
		if (um && um.GetMenu())
			busy = true;
		if (s_Return == 3 && !busy)
		{
			s_Return = 0;
			PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
			if (pb && pb.IsAlive())
				INT_UIF_Menu.Open(INT_QST_Const.MENU_JOURNAL);
			return;
		}
		if (!INT_QST_UI.TakeMoveRequest())
			return;
		if (busy)
		{
			INT_QST_UI.StartPlayerMove();		// keep asking until the other menu is gone
			return;
		}
		INT_UIF_Menu.Open(INT_QST_Const.MENU_MOVER);
	}

	override void OnBuild(Widget body)
	{
		BuildBar();
	}

	override void OnShow()
	{
		super.OnShow();
		s_Open = this;
		m_Finished = false;
		m_Drag = false;
		m_WasDown = true;			// the click that opened us must be released first
		INT_QST_Style st = INT_QST_UI.Style();
		m_StartX = st.FreeX;
		m_StartY = st.FreeY;
		m_StartScale = st.Scale;
		m_StartOpacity = st.Opacity;
		if (s_Return == 1)
			s_Return = 2;
		else
			s_Return = 0;
		INT_UIF_Menu.FreezePlayer(true);
		INT_QST_UI.SetEditMode(true);
	}

	override void OnHide()
	{
		super.OnHide();
		Finish(true);
		INT_UIF_Menu.FreezePlayer(false);
		if (s_Open == this)
			s_Open = null;
		if (s_Return == 2)
			s_Return = 3;		// Poll opens the journal again once this menu is gone
	}

	protected void Finish(bool save)
	{
		if (m_Finished)
			return;
		m_Finished = true;
		INT_QST_UI.SetEditMode(false);
		if (save)
			INT_QST_UI.SavePlayerLayout();
	}

	void OnClickId(int id, int data, int button)
	{
		m_Pending = id;			// handled in Update: never close the menu inside its own click
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (IsClosingDown())
			return;
		// death closes it like the notebook windows (the place it had before comes back, nothing is saved)
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb || !pb.IsAlive())
		{
			m_Drag = false;
			Restore();
			Finish(false);
			CloseNow();
			return;
		}
		if (m_Pending != 0)
		{
			int what = m_Pending;
			m_Pending = 0;
			Do(what);
			return;
		}
		UAInput back = GetUApi().GetInputByID(UAUIBack);
		if (back && back.LocalPress())
		{
			Finish(true);
			RequestClose();
			return;
		}
		int mx;
		int my;
		GetMousePos(mx, my);
		bool down = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) != 0;
		if (down && !m_WasDown && !m_Drag)
			TryGrab(mx, my);
		m_WasDown = down;
		if (!m_Drag)
			return;
		if (!down)
		{
			m_Drag = false;
			return;
		}
		Follow(mx, my);
	}

	protected void Do(int what)
	{
		INT_QST_Style st = INT_QST_UI.Style();
		switch (what)
		{
			case ID_RESET:
				st.Scale = 1.0;
				st.Opacity = 1.0;
				INT_QST_UI.ResetPosition();
				INT_QST_UI.ApplyStyle();
				INT_QST_Sfx.Play("click");
				return;
			case ID_SMALLER:
				st.Scale = st.Scale - 0.1;
				INT_QST_UI.ApplyStyle();
				INT_QST_Sfx.Play("click");
				return;
			case ID_BIGGER:
				st.Scale = st.Scale + 0.1;
				INT_QST_UI.ApplyStyle();
				INT_QST_Sfx.Play("click");
				return;
			case ID_CANCEL:
				Restore();
				Finish(false);
				RequestClose();
				return;
		}
		Finish(true);
		INT_QST_Sfx.Play("pin");
		RequestClose();
	}

	// The place and size the tracker had when the mover opened.
	protected void Restore()
	{
		INT_QST_Style st = INT_QST_UI.Style();
		st.Scale = m_StartScale;
		st.Opacity = m_StartOpacity;
		if (m_StartX >= 0 && m_StartY >= 0)
			INT_QST_UI.SetFreePosition(m_StartX, m_StartY);
		else
			INT_QST_UI.ResetPosition();
		INT_QST_UI.ApplyStyle();
	}

	protected void TryGrab(float mx, float my)
	{
		float x;
		float y;
		float w;
		float h;
		if (!INT_QST_UI.GetScreenRect(x, y, w, h))
			return;
		float pad = INT_UIF_Draw.S(8);
		if (mx < x - pad || mx > x + w + pad || my < y - pad || my > y + h + pad)
			return;
		m_Drag = true;
		m_GrabX = mx - x;
		m_GrabY = my - y;
	}

	protected void Follow(float mx, float my)
	{
		float sw = INT_UIF_Scale.ScreenW();
		float sh = INT_UIF_Scale.ScreenH();
		if (sw <= 0 || sh <= 0)
			return;
		float nx = (mx - m_GrabX) / sw;
		float ny = (my - m_GrabY) / sh;
		INT_QST_UI.SetFreePosition(nx, ny);
	}

	// Leather bar at the bottom centre: title + hint, − / +, Сброс / Отмена / Сохранить.
	protected void BuildBar()
	{
		Widget root = GetLayoutRoot();
		if (!root)
			return;
		float sw = INT_UIF_Scale.ScreenW();
		float sh = INT_UIF_Scale.ScreenH();
		m_Bar = INT_UIF_Draw.Frame(root, 0, 0, BAR_W, BAR_H, "INT_QST_MoverBar");
		if (!m_Bar)
			return;
		float px = (sw - INT_UIF_Draw.S(BAR_W)) * 0.5;
		float py = sh - INT_UIF_Draw.S(BAR_H + 140);
		m_Bar.SetPos(px, py);
		INT_QST_Look.Cover(m_Bar, 0, 0, BAR_W, BAR_H, INT_QST_Look.Leather(), 5);
		string title = INT_QST_Look.Tr("#STR_INT_QST_MOVE_TITLE");
		string hint = INT_QST_Look.Tr("#STR_INT_QST_MOVE_HINT");
		int cream = INT_QST_Look.Cream();
		int tan = INT_QST_Look.Tan();
		INT_QST_Look.Heavy(m_Bar, 20, 10, 300, 26, title, 16, cream);
		INT_QST_Look.Text(m_Bar, 20, 36, 300, 22, hint, 12, tan);
		Btn(318, 44, "-", ID_SMALLER, INT_QST_Look.BTN_GHOST);
		Btn(368, 44, "+", ID_BIGGER, INT_QST_Look.BTN_GHOST);
		Btn(428, 100, "#STR_INT_QST_MOVE_RESET", ID_RESET, INT_QST_Look.BTN_GHOST);
		Btn(536, 100, "#STR_INT_QST_MOVE_CANCEL", ID_CANCEL, INT_QST_Look.BTN_GHOST);
		Btn(644, 100, "#STR_INT_QST_MOVE_SAVE", ID_SAVE, INT_QST_Look.BTN_INK);
	}

	protected void Btn(float x, float w, string label, int id, int style)
	{
		Widget hv;
		int ic = INT_QST_Look.Sticky();
		Widget b = INT_QST_Look.Button(m_Bar, x, 14, w, 42, label, "", style, ic, hv);
		if (style == INT_QST_Look.BTN_GHOST)
		{
			// light text on the leather
			int fill = 0x1AFFFFFF;
			int edge = INT_QST_Look.CreamA(120);
			INT_UIF_Widgets.ClearChildren(b);
			INT_UIF_Surface.Box(b, 0, 0, w, 42, 6, fill, edge);
			hv = INT_UIF_Draw.Frame(b, 0, 0, w, 42, "Hover");
			INT_UIF_Surface.Box(hv, 0, 0, w, 42, 6, 0x33FFFFFF, edge);
			string t = INT_QST_Look.Upper(label);
			int cream = INT_QST_Look.Cream();
			INT_QST_Look.Heavy(b, 0, 0, w, 42, t, 13, cream, INT_UIF_Align.CENTER);
		}
		ButtonWidget hit = INT_UIF_Draw.Hit(b, 0, 0, w, 42, m_Clicks);
		m_Clicks.Bind(hit, id, 0, hv);
	}

	// --- self-test: the same follow path with a given position (no real mouse on the stand) ----------------
	bool TestDrag(float nx, float ny)
	{
		float x;
		float y;
		float w;
		float h;
		if (!INT_QST_UI.GetScreenRect(x, y, w, h))
			return false;
		TryGrab(x + w * 0.5, y + h * 0.5);
		if (!m_Drag)
			return false;
		float sw = INT_UIF_Scale.ScreenW();
		float sh = INT_UIF_Scale.ScreenH();
		Follow(nx * sw + m_GrabX, ny * sh + m_GrabY);
		m_Drag = false;
		return true;
	}

	void TestDo(int what)
	{
		Do(what);
	}
}
