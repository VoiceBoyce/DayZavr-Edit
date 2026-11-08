// INT PartyMode (client) — «Двигать в игре» (settings tab «Треккер группы»): the player drags the tracker panel with
// the mouse. A window-less UIF menu (cursor on, the player stands still, the HUD stays visible) registered lazily under
// INT_PTY_Const.MENU_MOVER; the tracker shows an orange move frame (sample rows when no friend is shown). Drag pattern of
// INT_SK_Mover / INT_KF_MoverMenu: the grab starts on a left mouse-down inside the panel, keeps the grab offset and
// follows GetMousePos while the button is held. «Готово», Enter, Esc or a right click finish and save TrackerX / TrackerY
// (MarkDirty + Flush); «Сброс» puts the panel back to the left middle and keeps the mode on. The window comes back
// afterwards when the mover was opened from it.

class INT_PTY_MoverBar : ScriptedWidgetEventHandler
{
	ButtonWidget m_Done;
	ButtonWidget m_Reset;

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
			return false;
		// never close the menu inside its own click: Update acts on the request
		if (w == m_Done)
		{
			INT_PTY_MoverMenu.SetRequest(1);
			return true;
		}
		if (w == m_Reset)
		{
			INT_PTY_MoverMenu.SetRequest(2);
			return true;
		}
		return false;
	}
}

class INT_PTY_MoverMenu : INT_UIF_Menu
{
	static const float BAR_W = 840;
	static const float BAR_H = 64;

	protected static ref INT_PTY_MoverBar s_Bar;		// kept for the session: never freed inside its own click
	protected static int s_Request;						// 1 done, 2 reset
	protected static INT_PTY_MoverMenu s_Open;
	protected static bool s_Wanted;
	protected static bool s_Registered;
	protected static bool s_ReturnToWindow;
	protected static bool s_ReturnPending;
	protected static Man s_Body;						// weak: the body the mover was opened with (H-2)

	protected Widget m_BarRoot;
	protected int m_BarSw = -1;							// H-9: the bar is re-placed on a resolution change
	protected int m_BarSh = -1;
	protected bool m_Explicit;							// H-2: finished by «Готово» / Enter / right click / Esc
	protected bool m_Drag;
	protected bool m_WasDown;
	protected bool m_WasRight;
	protected bool m_WasEnter;
	protected float m_GrabX;
	protected float m_GrabY;
	protected bool m_Finished;

	override bool ShowWindow()
	{
		return false;
	}

	override bool FreezesPlayer()
	{
		return false;		// frozen in OnShow without HoldHud: the HUD must stay visible
	}

	override bool UsesBlur()
	{
		return false;
	}

	// H-2: Esc is an explicit finish like «Готово» (handled in Update), not the framework's plain close.
	override bool CloseOnEscape()
	{
		return false;
	}

	static void SetRequest(int what)
	{
		s_Request = what;
	}

	static bool IsOpen()
	{
		return s_Open != null;
	}

	// From INT_PTY_Hud.OpenMover: the mover opens on the next HUD frame.
	static void Request(bool returnToWindow)
	{
		s_Wanted = true;
		s_ReturnToWindow = returnToWindow;
	}

	static void ResetState()
	{
		s_Wanted = false;
		s_ReturnPending = false;
		s_ReturnToWindow = false;
		s_Request = 0;
		s_Body = null;
	}

	// H-2: the player is alive in the same body as when the mover opened (no window after a death / respawn).
	protected static bool SameLiveBody()
	{
		if (!GetGame() || !s_Body)
			return false;
		Man cur = GetGame().GetPlayer();
		if (!cur || cur != s_Body)
			return false;
		return cur.IsAlive();
	}

	static void EnsureRegistered()
	{
		if (s_Registered)
			return;
		s_Registered = true;
		INT_UIF_MenuRegistry.Register(INT_PTY_Const.MENU_MOVER, INT_PTY_MoverMenu);
	}

	// Every frame from the HUD tick: opens the mover (closing our window first, outside of its click), and brings the
	// window back once the mover is gone.
	static void Poll()
	{
		if (!GetGame())
			return;
		UIManager um = GetGame().GetUIManager();
		if (!um)
			return;
		if (s_ReturnPending)
		{
			if (um.GetMenu())
				return;
			s_ReturnPending = false;
			if (!SameLiveBody())
				return;
			if (!INT_PTY_Menu.IsOpen())
				INT_PTY_Menu.Toggle();
			return;
		}
		if (!s_Wanted)
			return;
		UIScriptedMenu cur = um.GetMenu();
		if (cur && !INT_PTY_Menu.IsOpen())
			return;		// another menu is open: wait for it
		s_Wanted = false;
		EnsureRegistered();
		INT_UIF_Menu.Open(INT_PTY_Const.MENU_MOVER);
	}

	override void OnBuild(Widget body)
	{
		BuildBar();
	}

	override void OnShow()
	{
		super.OnShow();
		s_Open = this;
		s_Request = 0;
		s_Body = null;
		if (GetGame())
			s_Body = GetGame().GetPlayer();
		m_Explicit = false;
		m_Finished = false;
		m_Drag = false;
		m_WasDown = true;			// the click that opened us must be released first
		m_WasRight = true;
		m_WasEnter = true;
		INT_UIF_Menu.FreezePlayer(true);
		INT_PTY_Sfx.Open();
		INT_PTY_Log.Dbg("перемещение треккера: начато");
	}

	override void OnHide()
	{
		super.OnHide();
		Finish();
		INT_UIF_Menu.FreezePlayer(false);
		if (s_Open == this)
			s_Open = null;
		// H-2: back to the window only after an explicit finish, alive, in the same body. A close by the engine
		// (CloseAll on a new body, death) or by another mod's menu brings nothing back.
		if (s_ReturnToWindow && m_Explicit)
		{
			s_ReturnToWindow = false;
			if (SameLiveBody())
				s_ReturnPending = true;
		}
	}

	// The place on screen becomes the player's (client profile).
	protected void Finish()
	{
		if (m_Finished)
			return;
		m_Finished = true;
		m_Drag = false;
		INT_PTY_SettingsStore.MarkDirty();
		INT_PTY_SettingsStore.Flush();
		INT_PTY_UI.OnPlaced().Invoke(INT_PTY_UI.PART_TRACKER);
		INT_PTY_Sfx.Close();
		INT_PTY_Log.Dbg("перемещение треккера: сохранено");
	}

	protected void Done()
	{
		m_Explicit = true;
		Finish();
		RequestClose();
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (IsClosingDown())
			return;
		// H-9: a resolution change re-places (rebuilds) the hint bar
		if (INT_UIF_Scale.ScreenW() != m_BarSw || INT_UIF_Scale.ScreenH() != m_BarSh)
			RebuildBar();
		if (s_Request != 0)
		{
			int what = s_Request;
			s_Request = 0;
			if (what == 2)
			{
				m_Drag = false;
				INT_PTY_UI.ResetPartPosition(INT_PTY_UI.PART_TRACKER);
				INT_PTY_Sfx.Click();
				return;
			}
			Done();
			return;
		}
		bool enter = KeyState(KeyCode.KC_RETURN) != 0 || KeyState(KeyCode.KC_NUMPADENTER) != 0;
		if (enter && !m_WasEnter)
		{
			Done();
			return;
		}
		m_WasEnter = enter;
		bool right = (GetMouseState(MouseState.RIGHT) & MB_PRESSED_MASK) != 0;
		if (right && !m_WasRight)
		{
			Done();
			return;
		}
		m_WasRight = right;
		UAInput back = GetUApi().GetInputByID(UAUIBack);
		if (back && back.LocalPress())
		{
			Done();
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

	protected void TryGrab(float mx, float my)
	{
		float x;
		float y;
		float w;
		float h;
		if (!INT_PTY_UI.GetPartRect(INT_PTY_UI.PART_TRACKER, x, y, w, h))
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
		INT_PTY_UI.SetPartPosition(INT_PTY_UI.PART_TRACKER, nx, ny);
	}

	protected void RebuildBar()
	{
		if (m_BarRoot)
			m_BarRoot.Unlink();
		m_BarRoot = null;
		BuildBar();
	}

	// Hint bar at the top centre in the Expedition look: diamond, title, one line of help, «Сброс», «Готово».
	protected void BuildBar()
	{
		m_BarSw = INT_UIF_Scale.ScreenW();
		m_BarSh = INT_UIF_Scale.ScreenH();
		Widget root = GetLayoutRoot();
		if (!root)
			return;
		float sw = INT_UIF_Scale.ScreenW();
		m_BarRoot = INT_UIF_Draw.Frame(root, 0, 0, BAR_W, BAR_H, "INT_PTY_MoverBar");
		if (!m_BarRoot)
			return;
		m_BarRoot.SetSort(460);
		float px = (sw - INT_UIF_Draw.S(BAR_W)) * 0.5;
		float py = INT_UIF_Draw.S(24);
		m_BarRoot.SetPos(px, py);
		INT_PTY_Look.Fill(m_BarRoot, 0, 0, BAR_W, BAR_H, ARGB(240, 8, 8, 8));
		INT_PTY_Look.Fill(m_BarRoot, 0, 0, BAR_W, 2, INT_PTY_Look.ORANGE);
		INT_PTY_Look.Diamond(m_BarRoot, 26, 22, 8, "pty_diamond_ca", INT_PTY_Look.ORANGE);
		INT_PTY_Look.Caps(m_BarRoot, 40, 9, 400, 26, "#STR_INT_PTY_H_MOVE_TITLE", 12, INT_PTY_Look.WHITE, INT_UIF_Align.LEFT, true);
		string hint = INT_PTY_Text.Tr("#STR_INT_PTY_H_MOVE_HINT");
		INT_PTY_Look.Text(m_BarRoot, 22, 33, 560, 24, hint, 13, INT_PTY_Look.BODY);
		if (!s_Bar)
			s_Bar = new INT_PTY_MoverBar();
		Widget rs = INT_UIF_Draw.Frame(m_BarRoot, BAR_W - 252, 14, 112, 36, "INT_PTY_MoverReset");
		int edgeC = ARGB(102, 255, 255, 255);
		INT_PTY_Look.Fill(rs, 0, 0, 112, 1, edgeC);
		INT_PTY_Look.Fill(rs, 0, 35, 112, 1, edgeC);
		INT_PTY_Look.Fill(rs, 0, 0, 1, 36, edgeC);
		INT_PTY_Look.Fill(rs, 111, 0, 1, 36, edgeC);
		INT_PTY_Look.Caps(rs, 0, 0, 112, 36, "#STR_INT_PTY_H_MOVE_RESET", 12, INT_PTY_Look.MUTED, INT_UIF_Align.CENTER, true);
		s_Bar.m_Reset = INT_UIF_Draw.Hit(rs, 0, 0, 112, 36, s_Bar);
		Widget dn = INT_UIF_Draw.Frame(m_BarRoot, BAR_W - 128, 14, 112, 36, "INT_PTY_MoverDone");
		INT_PTY_Look.Fill(dn, 0, 0, 112, 36, INT_PTY_Look.ORANGE);
		INT_PTY_Look.Caps(dn, 0, 0, 112, 36, "#STR_INT_PTY_H_MOVE_DONE", 12, INT_PTY_Look.INK, INT_UIF_Align.CENTER, true);
		s_Bar.m_Done = INT_UIF_Draw.Hit(dn, 0, 0, 112, 36, s_Bar);
	}

	// --- stand self-test: the same follow path with a given place (no real mouse on the stand) -----------------
	static bool TestDrag(float nx, float ny)
	{
		if (!s_Open)
			return false;
		return s_Open.DoTestDrag(nx, ny);
	}

	// grab the middle of the panel, drop it so that its top-left lands on (nx, ny)
	bool DoTestDrag(float nx, float ny)
	{
		float x;
		float y;
		float w;
		float h;
		if (!INT_PTY_UI.GetPartRect(INT_PTY_UI.PART_TRACKER, x, y, w, h))
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

	static void TestFinish()
	{
		SetRequest(1);
	}
}
