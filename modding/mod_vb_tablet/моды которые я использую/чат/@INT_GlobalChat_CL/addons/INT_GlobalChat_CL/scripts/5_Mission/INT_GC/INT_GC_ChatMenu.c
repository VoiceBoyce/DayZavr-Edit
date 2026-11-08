// INT GlobalChat — the open chat window (menu 9190201, spec.md §5.1-5.5). Opened by Enter through the modded
// MissionGameplay.ShowChat. A window-less INT_UIF_Menu (no dim, no blur, HUD stays): the player is frozen with the
// counted INT_UIF_Menu.FreezePlayer while typing. Drag (header) and resize (corner) poll the mouse in Update like
// the INT_KillFeed mover; every click only sets state, so no widget is freed inside its own handler.

class INT_GC_ChatHandler : ScriptedWidgetEventHandler
{
	protected INT_GC_ChatMenu m_Menu;

	void SetMenu(INT_GC_ChatMenu menu)
	{
		m_Menu = menu;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (m_Menu)
			return m_Menu.HandleClick(w, button);
		return false;
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (m_Menu)
			return m_Menu.HandleDown(w, x, y, button);
		return false;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (m_Menu)
			return m_Menu.HandleWheel(w, wheel);
		return false;
	}

	// Enter in an EditBox arrives here as finished = true (vanilla ChatInputMenu.OnChange); OnKeyPress does not
	// get it (owner report 2026-10-20: Enter did nothing).
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (m_Menu)
			return m_Menu.HandleChange(w, finished);
		return false;
	}

	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (m_Menu)
			return m_Menu.HandleKeyDown(w, key);
		return false;
	}

	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if (m_Menu)
			return m_Menu.HandleKeyPress(w, key);
		return false;
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (m_Menu)
			m_Menu.HandleFocus(w, true);
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (m_Menu)
			m_Menu.HandleFocus(w, false);
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

class INT_GC_ChatMenu : INT_UIF_Menu
{
	static const int DRAG_NONE = 0;
	static const int DRAG_MOVE = 1;
	static const int DRAG_RESIZE = 2;
	static const float OPEN_GUARD = 0.2;		// the Enter that opened the window is not a send
	static const float RESIZE_STEP = 0.08;

	protected static INT_GC_ChatMenu s_Instance;	// weak: the UI manager owns the menu
	protected static string s_Draft = "";

	protected ref INT_GC_Window m_Win;
	protected ref INT_GC_ChatHandler m_Hand;
	protected int m_Channel;
	protected float m_OpenedAt;
	protected int m_Recall = -1;
	protected bool m_Frozen;
	protected bool m_Ready;

	protected int m_Drag;
	protected float m_GrabX;
	protected float m_GrabY;
	protected float m_StartW;
	protected float m_StartH;
	protected float m_StartMX;
	protected float m_StartMY;
	protected float m_LastResize;
	protected bool m_WasDown;

	// nick popup
	protected Widget m_Popup;
	protected ref INT_GC_Line m_PopupLine;
	protected ButtonWidget m_PopReply;
	protected ButtonWidget m_PopCopy;
	protected ButtonWidget m_PopBlock;
	protected ref array<ref INT_UIF_Surface> m_PopRows;
	protected ref array<ButtonWidget> m_PopHits;
	protected int m_PendingAction;			// set by a click, run in Update
	protected float m_LastSendQueued = -10;
	protected bool m_ReAdded;
	protected ref map<int, float> m_KeyAt;
	// raw key state of the previous frame (edge detection; key events of a focused EditBox are not reliable)
	protected bool m_KEsc;
	protected bool m_KTab;
	protected bool m_KUp;
	protected bool m_KDown;
	protected bool m_KPgUp;
	protected bool m_KPgDn;
	protected bool m_KEnter;
	protected bool m_PopupJustOpened;

	static const int ACT_NONE = 0;
	static const int ACT_EDITOR = 1;
	static const int ACT_SEND = 2;
	static const int ACT_POP_REPLY = 3;
	static const int ACT_POP_COPY = 4;
	static const int ACT_POP_BLOCK = 5;
	static const int ACT_CLOSE_POPUP = 6;

	void INT_GC_ChatMenu()
	{
		m_Hand = new INT_GC_ChatHandler();
		m_Hand.SetMenu(this);
		m_PopRows = new array<ref INT_UIF_Surface>();
		m_PopHits = new array<ButtonWidget>();
		m_KeyAt = new map<int, float>();
	}

	void ~INT_GC_ChatMenu()
	{
		if (s_Instance == this)
			s_Instance = null;
		if (m_Frozen)
			INT_UIF_Menu.FreezePlayer(false);
		m_Frozen = false;
	}

	static INT_GC_ChatMenu Instance()
	{
		return s_Instance;
	}

	INT_GC_Window GetWindow()
	{
		return m_Win;
	}

	// --- INT_UIF_Menu configuration ---------------------------------------------------------------------------
	override bool ShowWindow()
	{
		return false;
	}

	override bool FreezesPlayer()
	{
		return false;			// would hide the HUD; the freeze is done by hand in OnShow
	}

	override bool UsesBlur()
	{
		return false;
	}

	override bool WantsKeyboard()
	{
		return true;
	}

	override void OnBuild(Widget body)
	{
		m_Channel = INT_GC_UI.Settings().Channel;
		BuildWindow();
	}

	protected void BuildWindow()
	{
		string draft = s_Draft;
		if (m_Win && m_Win.m_Edit)
			draft = m_Win.m_Edit.GetText();
		ClosePopup();
		if (m_Win)
			m_Win.Destroy();
		m_Win = new INT_GC_Window();
		m_Win.Build(GetLayoutRoot(), true, m_Hand);
		INT_GC_Style st = INT_GC_UI.Style();
		if (m_Win.m_Area)
		{
			m_Win.m_Area.SetMetrics(st.FontSize, st.ShowTime);
			m_Win.m_Area.SetSource(INT_GC_Client.History());
			m_Win.m_Area.Refresh();
		}
		m_Win.PaintChannel(m_Channel);
		if (m_Win.m_Edit)
		{
			m_Win.m_Edit.SetText(draft);
			UpdateCounter();
		}
		m_Win.UpdateScroll();
	}

	// Full rebuild (style / size change from the API or the editor).
	void Rebuild()
	{
		bool focus = m_Win && m_Win.m_Edit && GetFocus() == m_Win.m_Edit;
		BuildWindow();
		if (focus && m_Win.m_Edit)
			SetFocus(m_Win.m_Edit);
	}

	override void OnShow()
	{
		super.OnShow();
		s_Instance = this;
		m_OpenedAt = GetGame().GetTickTime();
		m_Recall = -1;
		m_Drag = DRAG_NONE;
		m_WasDown = true;
		SeedKeys();
		if (!m_Frozen)
		{
			INT_UIF_Menu.FreezePlayer(true);
			m_Frozen = true;
		}
		if (m_Win && m_Win.m_Area)
			m_Win.m_Area.ScrollToBottom();
		if (m_Win)
			m_Win.UpdateScroll();
		if (m_Win && m_Win.m_Edit)
			SetFocus(m_Win.m_Edit);
		m_Ready = true;
		INT_GC_UI.OnOpened().Invoke();
	}

	override void OnHide()
	{
		if (m_Win && m_Win.m_Edit)
			s_Draft = m_Win.m_Edit.GetText();
		SetFocus(null);
		super.OnHide();
		if (m_Frozen)
		{
			INT_UIF_Menu.FreezePlayer(false);
			m_Frozen = false;
		}
		// the exclude this window re-added goes too, unless another menu still holds the freeze
		Mission mission = GetGame().GetMission();
		if (m_ReAdded && s_FreezeDepth <= 0 && mission && mission.IsInputExcludeActive("menu"))
			mission.RemoveActiveInputExcludes({"menu"});
		m_ReAdded = false;
		if (m_Drag != DRAG_NONE)
			EndDrag();
		if (s_Instance == this)
			s_Instance = null;
		// the Enter / Esc that closed the window must not reopen it, switch the view or open the pause menu
		GetUApi().GetInputByID(UAChat).Supress();
		GetUApi().GetInputByID(UAPersonView).Supress();
		INT_GC_UI.OnClosed().Invoke();
	}

	// --- lines ------------------------------------------------------------------------------------------------
	void OnLineAdded(INT_GC_Line line)
	{
		if (!m_Win || !m_Win.m_Area)
			return;
		m_Win.m_Area.OnAppended(line);
		m_Win.UpdateScroll();
	}

	// --- per frame ----------------------------------------------------------------------------------------------
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (!m_Win)
			return;
		GuardInput();
		PollKeys();
		RunPending();
		bool down = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) != 0;
		if (m_Drag != DRAG_NONE)
		{
			if (!down)
				EndDrag();
			else
				FollowDrag();
		}
		m_WasDown = down;
	}

	protected void RunPending()
	{
		int act = m_PendingAction;
		if (act == ACT_NONE)
			return;
		m_PendingAction = ACT_NONE;
		if (act == ACT_EDITOR)
		{
			INT_GC_Sfx.Play(INT_GC_Sfx.CLICK);
			INT_GC_UI.OpenEditor();
			return;
		}
		if (act == ACT_SEND)
		{
			Submit();
			return;
		}
		if (act == ACT_POP_REPLY)
		{
			PopupReply();
			return;
		}
		if (act == ACT_POP_COPY)
		{
			PopupCopy();
			return;
		}
		if (act == ACT_POP_BLOCK)
		{
			PopupBlock();
			return;
		}
		if (act == ACT_CLOSE_POPUP)
			ClosePopup();
	}

	// --- drag / resize ----------------------------------------------------------------------------------------
	protected void StartDrag(int mode)
	{
		if (!INT_GC_UI.IsPlayerMoveEnabled())
			return;
		int mx;
		int my;
		GetMousePos(mx, my);
		float sx;
		float sy;
		float sw;
		float sh;
		m_Win.ScreenRect(sx, sy, sw, sh);
		m_Drag = mode;
		m_GrabX = mx - sx;
		m_GrabY = my - sy;
		m_StartMX = mx;
		m_StartMY = my;
		m_StartW = INT_GC_UI.Style().W;
		m_StartH = INT_GC_UI.Style().H;
		m_LastResize = 0;
		ClosePopup();
	}

	protected void FollowDrag()
	{
		int mx;
		int my;
		GetMousePos(mx, my);
		INT_GC_Style st = INT_GC_UI.Style();
		float scrW = INT_UIF_Scale.ScreenW();
		float scrH = INT_UIF_Scale.ScreenH();
		if (m_Drag == DRAG_MOVE)
		{
			float w = INT_UIF_Draw.S(st.W);
			float h = INT_UIF_Draw.S(st.H);
			float roomX = Math.Max(0, scrW - w);
			float roomY = Math.Max(0, scrH - h);
			float nx = Math.Clamp(mx - m_GrabX, 0, roomX);
			float ny = Math.Clamp(my - m_GrabY, 0, roomY);
			st.X = nx / scrW;
			st.Y = ny / scrH;
			m_Win.MoveTo(nx, ny);
			return;
		}
		float f = INT_UIF_Scale.Factor();
		float sx;
		float sy;
		INT_GC_Window.ScreenPos(sx, sy);
		float maxW = (scrW - sx) / f;
		float maxH = (scrH - sy) / f;
		float dw = (mx - m_StartMX) / f;
		float dh = (my - m_StartMY) / f;
		float capW = Math.Min(INT_GC_Style.MAX_W, maxW);
		float capH = Math.Min(INT_GC_Style.MAX_H, maxH);
		float nw = Math.Clamp(m_StartW + dw, INT_GC_Style.MIN_W, capW);
		float nh = Math.Clamp(m_StartH + dh, INT_GC_Style.MIN_H, capH);
		if (st.X < 0 || st.Y < 0)
		{
			st.X = sx / scrW;
			st.Y = sy / scrH;
		}
		float now = GetGame().GetTickTime();
		if (now - m_LastResize < RESIZE_STEP)
			return;
		if (Math.AbsFloat(nw - st.W) < 1 && Math.AbsFloat(nh - st.H) < 1)
			return;
		m_LastResize = now;
		st.W = Math.Round(nw);
		st.H = Math.Round(nh);
		Rebuild();
	}

	protected void EndDrag()
	{
		int mode = m_Drag;
		m_Drag = DRAG_NONE;
		if (mode == DRAG_RESIZE)
			Rebuild();
		INT_GC_UI.Style().Sanitize();
		INT_GC_UI.SavePlayerLayout();
		INT_GC_UI.OnPlaced().Invoke();
		if (m_Win && m_Win.m_Edit)
			SetFocus(m_Win.m_Edit);
	}

	// --- input from the handler ---------------------------------------------------------------------------------
	bool HandleClick(Widget w, int button)
	{
		if (!m_Win || !w)
			return false;
		if (IsPopupHit(w))
			return true;
		if (w == m_Win.m_GearHit)
		{
			m_PendingAction = ACT_EDITOR;
			return true;
		}
		if (w == m_Win.m_EyeHit)
		{
			INT_GC_Sfx.Play(INT_GC_Sfx.CLICK);
			INT_GC_UI.ToggleHidden();
			PaintEye();
			FocusEdit();
			return true;
		}
		if (w == m_Win.m_SendHit)
		{
			m_PendingAction = ACT_SEND;
			return true;
		}
		if (w == m_Win.m_ChanGlobalHit)
		{
			SetChannel(INT_GC_Const.CH_GLOBAL);
			return true;
		}
		if (w == m_Win.m_ChanLocalHit)
		{
			SetChannel(INT_GC_Const.CH_LOCAL);
			return true;
		}
		if (w == m_Win.m_NewHit)
		{
			if (m_Win.m_Area)
				m_Win.m_Area.ScrollToBottom();
			m_Win.UpdateScroll();
			FocusEdit();
			return true;
		}
		return false;
	}

	bool HandleDown(Widget w, int x, int y, int button)
	{
		if (!m_Win || !w)
			return false;
		if (IsPopupHit(w))
			return false;
		if (m_Popup)
			m_PendingAction = ACT_CLOSE_POPUP;
		if (button == MouseState.LEFT && w == m_Win.m_HeaderHit)
		{
			StartDrag(DRAG_MOVE);
			return true;
		}
		if (button == MouseState.LEFT && w == m_Win.m_ResizeHit)
		{
			StartDrag(DRAG_RESIZE);
			return true;
		}
		if (m_Win.m_Area)
		{
			INT_GC_Line line = m_Win.m_Area.LineOfHit(w);
			if (line)
			{
				m_PendingAction = ACT_NONE;
				OpenPopup(line, x, y);
				return true;
			}
		}
		if (w == m_Win.m_LinesHit)
		{
			FocusEdit();
			return true;
		}
		return false;
	}

	bool HandleWheel(Widget w, int wheel)
	{
		if (!m_Win || !m_Win.m_Area)
			return false;
		if (w != m_Win.m_LinesHit && !m_Win.m_Area.LineOfHit(w))
			return false;
		m_Win.m_Area.ScrollBy(wheel * 3);
		m_Win.UpdateScroll();
		return true;
	}

	bool HandleChange(Widget w, bool finished = false)
	{
		if (!m_Win || w != m_Win.m_Edit)
			return false;
		if (finished)
		{
			QueueSend();
			return true;
		}
		string text = m_Win.m_Edit.GetText();
		int limit = INT_GC_Client.MaxLen();
		// a /pm prefix does not count against the message limit
		int extra = 0;
		string lower = INT_GC_Util.Lower(text);
		if (lower.IndexOf("/pm ") == 0 || lower.IndexOf("/лс ") == 0)
			extra = 70;
		if (text.LengthUtf8() > limit + extra)
		{
			text = text.SubstringUtf8(0, limit + extra);
			m_Win.m_Edit.SetText(text);
		}
		UpdateCounter();
		return true;
	}

	// The keys of the window are read from the raw key state in Update (PollKeys); here they are only consumed so
	// the EditBox does not move the focus (Tab) or the caret (arrows) on top of our action.
	bool HandleKeyDown(Widget w, int key)
	{
		if (!m_Win)
			return false;
		if (key == KeyCode.KC_ESCAPE || key == KeyCode.KC_TAB || key == KeyCode.KC_UP || key == KeyCode.KC_DOWN)
		{
			DoKey(key);
			return true;
		}
		if (key == KeyCode.KC_PRIOR || key == KeyCode.KC_NEXT)
		{
			DoKey(key);
			return true;
		}
		return false;
	}

	// Both sources (the EditBox key event and the raw key poll) end here; one action per key press.
	protected void DoKey(int key)
	{
		if (!m_Win)
			return;
		float now = GetGame().GetTickTime();
		float last;
		if (m_KeyAt.Find(key, last) && now - last < 0.15)
			return;
		m_KeyAt.Set(key, now);
		if (key == KeyCode.KC_ESCAPE)
		{
			if (m_Popup)
				ClosePopup();
			else
				RequestClose();
			return;
		}
		if (key == KeyCode.KC_TAB)
		{
			int next = INT_GC_Const.CH_LOCAL;
			if (m_Channel == INT_GC_Const.CH_LOCAL)
				next = INT_GC_Const.CH_GLOBAL;
			SetChannel(next);
			return;
		}
		if (key == KeyCode.KC_UP)
		{
			Recall(1);
			return;
		}
		if (key == KeyCode.KC_DOWN)
		{
			Recall(-1);
			return;
		}
		if ((key == KeyCode.KC_PRIOR || key == KeyCode.KC_NEXT) && m_Win.m_Area)
		{
			int page = Math.Max(1, m_Win.m_Area.VisibleRows() - 1);
			if (key == KeyCode.KC_NEXT)
				page = -page;
			m_Win.m_Area.ScrollBy(page);
			m_Win.UpdateScroll();
		}
	}

	// While the player types, game keys must stay off. The "menu" exclude (162 vanilla inputs, Tab and V among
	// them) is a plain set: any vanilla menu that closes removes it, and then Tab opened the inventory over the chat
	// and V switched the view (stand 2026-10-20). So it is asserted every frame; a few typing keys outside it
	// (map M, head / vehicle lights L) are suppressed for the next frame. Nothing stays set after the window closes.
	protected void GuardInput()
	{
		Mission mission = GetGame().GetMission();
		if (mission && !mission.IsInputExcludeActive("menu"))
		{
			mission.AddActiveInputExcludes({"menu"});
			if (!m_ReAdded)
				INT_GC_Log.Warn("исключение ввода menu было снято другим меню — восстановлено, пока открыт чат");
			m_ReAdded = true;
		}
		Sup("UAGear");
		Sup("UAMapToggle");
		Sup("UAToggleHeadlight");
		Sup("UAToggleVehicleLights");
		Sup("UAPersonView");
	}

	protected void Sup(string inputName)
	{
		UAInput i = GetUApi().GetInputByName(inputName);
		if (i)
			i.Supress();
	}

	protected void SeedKeys()
	{
		m_KEsc = KeyState(KeyCode.KC_ESCAPE) != 0;
		m_KTab = KeyState(KeyCode.KC_TAB) != 0;
		m_KUp = KeyState(KeyCode.KC_UP) != 0;
		m_KDown = KeyState(KeyCode.KC_DOWN) != 0;
		m_KPgUp = KeyState(KeyCode.KC_PRIOR) != 0;
		m_KPgDn = KeyState(KeyCode.KC_NEXT) != 0;
		bool enter = false;
		if (KeyState(KeyCode.KC_RETURN) != 0 || KeyState(KeyCode.KC_NUMPADENTER) != 0)
			enter = true;
		m_KEnter = enter;
	}

	protected void PollKeys()
	{
		bool esc = KeyState(KeyCode.KC_ESCAPE) != 0;
		bool tab = KeyState(KeyCode.KC_TAB) != 0;
		bool up = KeyState(KeyCode.KC_UP) != 0;
		bool dn = KeyState(KeyCode.KC_DOWN) != 0;
		bool pgUp = KeyState(KeyCode.KC_PRIOR) != 0;
		bool pgDn = KeyState(KeyCode.KC_NEXT) != 0;
		bool enter = false;
		if (KeyState(KeyCode.KC_RETURN) != 0 || KeyState(KeyCode.KC_NUMPADENTER) != 0)
			enter = true;
		bool escEdge = esc && !m_KEsc;
		bool tabEdge = tab && !m_KTab;
		bool upEdge = up && !m_KUp;
		bool dnEdge = dn && !m_KDown;
		bool pgUpEdge = pgUp && !m_KPgUp;
		bool pgDnEdge = pgDn && !m_KPgDn;
		bool enterEdge = enter && !m_KEnter;
		m_KEsc = esc;
		m_KTab = tab;
		m_KUp = up;
		m_KDown = dn;
		m_KPgUp = pgUp;
		m_KPgDn = pgDn;
		m_KEnter = enter;
		if (escEdge)
		{
			DoKey(KeyCode.KC_ESCAPE);
			return;
		}
		if (enterEdge)
			QueueSend();
		if (tabEdge)
			DoKey(KeyCode.KC_TAB);
		if (upEdge)
			DoKey(KeyCode.KC_UP);
		if (dnEdge)
			DoKey(KeyCode.KC_DOWN);
		if (pgUpEdge)
			DoKey(KeyCode.KC_PRIOR);
		if (pgDnEdge)
			DoKey(KeyCode.KC_NEXT);
	}

	bool HandleKeyPress(Widget w, int key)
	{
		if (!m_Win || w != m_Win.m_Edit)
			return false;
		if (key == KeyCode.KC_RETURN || key == KeyCode.KC_NUMPADENTER)
		{
			QueueSend();
			return true;
		}
		return false;
	}

	// One send per Enter even when both the change event and the key event report it.
	protected void QueueSend()
	{
		float now = GetGame().GetTickTime();
		if (now - m_LastSendQueued < 0.15)
			return;
		m_LastSendQueued = now;
		m_PendingAction = ACT_SEND;
	}

	void HandleFocus(Widget w, bool focused)
	{
		if (!m_Win || w != m_Win.m_Edit)
			return;
		m_Win.SetFocusPaint(focused);
	}

	void HandleHover(Widget w, bool enter)
	{
		int n = m_PopHits.Count();
		for (int i = 0; i < n; i++)
		{
			if (m_PopHits.Get(i) != w)
				continue;
			INT_UIF_Surface row = m_PopRows.Get(i);
			if (!row)
				continue;
			int fill = 0;
			if (enter)
				fill = INT_GC_Look.BrassA(46);
			row.SetFill(fill);
		}
	}

	// --- actions ------------------------------------------------------------------------------------------------
	protected void FocusEdit()
	{
		if (m_Win && m_Win.m_Edit)
			SetFocus(m_Win.m_Edit);
	}

	void PaintEye()
	{
		if (m_Win)
			m_Win.PaintEye(INT_GC_UI.IsHidden());
	}

	void SetChannel(int channel)
	{
		if (channel == m_Channel)
		{
			FocusEdit();
			return;
		}
		m_Channel = channel;
		INT_GC_UI.Settings().Channel = channel;
		INT_GC_UI.SaveClient();
		INT_GC_Sfx.Play(INT_GC_Sfx.CLICK);
		if (m_Win)
			m_Win.PaintChannel(channel);
		FocusEdit();
	}

	protected void UpdateCounter()
	{
		if (!m_Win || !m_Win.m_Edit)
			return;
		string text = m_Win.m_Edit.GetText();
		int n = text.LengthUtf8();
		m_Win.SetCounter(n, INT_GC_Client.MaxLen());
		m_Win.RefreshPlaceholder();
	}

	protected void Recall(int dir)
	{
		array<string> sent = INT_GC_Client.SentHistory();
		int n = sent.Count();
		if (n == 0 || !m_Win || !m_Win.m_Edit)
			return;
		if (m_Recall < 0)
			m_Recall = n;
		m_Recall = m_Recall - dir;
		if (m_Recall < 0)
			m_Recall = 0;
		if (m_Recall >= n)
		{
			m_Recall = n;
			m_Win.m_Edit.SetText("");
		}
		else
		{
			string t = sent.Get(m_Recall);
			m_Win.m_Edit.SetText(t);
		}
		UpdateCounter();
	}

	protected void Submit()
	{
		if (!m_Win || !m_Win.m_Edit)
			return;
		float now = GetGame().GetTickTime();
		string text = m_Win.m_Edit.GetText();
		if (now - m_OpenedAt < OPEN_GUARD && text == "")
			return;
		int r = INT_GC_Client.Submit(text, m_Channel);
		if (r == INT_GC_Client.R_EMPTY)
		{
			s_Draft = "";
			RequestClose();
			return;
		}
		if (r == INT_GC_Client.R_FLOOD || r == INT_GC_Client.R_PM_USAGE || r == INT_GC_Client.R_NOGLOBAL)
		{
			FocusEdit();
			return;
		}
		// sent, a command, a blocked word, a ban notice or help: the input is consumed
		m_Win.m_Edit.SetText("");
		s_Draft = "";
		m_Recall = -1;
		UpdateCounter();
		if (r == INT_GC_Client.R_SENT || r == INT_GC_Client.R_COMMAND)
		{
			INT_GC_Sfx.Play(INT_GC_Sfx.SEND);
			RequestClose();
			return;
		}
		FocusEdit();
	}

	// --- nick popup ---------------------------------------------------------------------------------------------
	protected bool IsPopupHit(Widget w)
	{
		if (!m_Popup || !w)
			return false;
		if (w == m_PopReply)
		{
			m_PendingAction = ACT_POP_REPLY;
			return true;
		}
		if (w == m_PopCopy)
		{
			m_PendingAction = ACT_POP_COPY;
			return true;
		}
		if (w == m_PopBlock)
		{
			m_PendingAction = ACT_POP_BLOCK;
			return true;
		}
		return false;
	}

	protected void OpenPopup(INT_GC_Line line, int x, int y)
	{
		ClosePopup();
		m_PopupLine = line;
		float f = INT_UIF_Scale.Factor();
		float pw = 236;
		float rowH = 34;
		bool canBlock = !line.Mine;
		int items = 2;
		if (canBlock)
			items = 3;
		float ph = 40 + items * rowH + 8;
		float scrW = INT_UIF_Scale.ScreenW();
		float scrH = INT_UIF_Scale.ScreenH();
		float offX = INT_UIF_Draw.S(8);
		float offY = INT_UIF_Draw.S(4);
		float maxPX = scrW - INT_UIF_Draw.S(pw);
		float maxPY = scrH - INT_UIF_Draw.S(ph);
		float px = Math.Clamp(x + offX, 0, maxPX);
		float py = Math.Clamp(y + offY, 0, maxPY);
		m_Popup = INT_UIF_Draw.Frame(GetLayoutRoot(), 0, 0, pw, ph, "Popup");
		if (!m_Popup)
			return;
		m_Popup.SetPos(px, py);
		m_Popup.SetSort(50);
		INT_UIF_Surface.Shadow(m_Popup, -30, -22, pw + 60, ph + 56, ARGB(160, 0, 0, 0));
		int fill = ARGB(250, 26, 21, 14);
		int border = INT_GC_Look.BrassA(120);
		INT_UIF_Surface.Box(m_Popup, 0, 0, pw, ph, 6, fill, border);
		int nickCol = INT_GC_Area.NickColorOf(line);
		INT_UIF_Draw.Image(m_Popup, 12, 16, 9, 9, "iuf_circle16", nickCol);
		int head = ARGB(255, 243, 234, 219);
		INT_UIF_Draw.Text(m_Popup, 28, 0, pw - 36, 40, line.Name, 14, head);
		int lineCol = INT_GC_Look.BrassA(50);
		INT_UIF_Draw.Line(m_Popup, 8, 39, pw - 16, 1, lineCol);
		float iy = 44;
		m_PopReply = PopupItem(iy, "gc_reply", "#STR_INT_GC_POP_REPLY", INT_GC_Look.Text());
		iy = iy + rowH;
		m_PopCopy = PopupItem(iy, "gc_copy", "#STR_INT_GC_POP_COPY", INT_GC_Look.Text());
		iy = iy + rowH;
		if (canBlock)
			m_PopBlock = PopupItem(iy, "gc_block", "#STR_INT_GC_POP_BLOCK", INT_GC_Look.Mod());
		INT_GC_Sfx.Play(INT_GC_Sfx.CLICK);
	}

	protected ButtonWidget PopupItem(float y, string icon, string labelKey, int color)
	{
		float pw = 236;
		INT_UIF_Surface row = INT_UIF_Surface.Box(m_Popup, 6, y, pw - 12, 32, 4, 0, 0);
		m_PopRows.Insert(row);
		INT_GC_Look.Cell(m_Popup, 16, y + 9, 14, 14, icon, color);
		string label = Widget.TranslateString(labelKey);
		INT_UIF_Draw.Text(m_Popup, 40, y, pw - 50, 32, label, 14, color);
		ButtonWidget hit = INT_UIF_Draw.Hit(m_Popup, 6, y, pw - 12, 32, m_Hand);
		m_PopHits.Insert(hit);
		return hit;
	}

	void ClosePopup()
	{
		if (m_Popup)
			m_Popup.Unlink();
		m_Popup = null;
		m_PopReply = null;
		m_PopCopy = null;
		m_PopBlock = null;
		m_PopRows.Clear();
		m_PopHits.Clear();
	}

	protected void PopupReply()
	{
		INT_GC_Line line = m_PopupLine;
		ClosePopup();
		if (!line || !m_Win || !m_Win.m_Edit)
			return;
		string nick = line.Name;
		if (nick.IndexOf(" ") >= 0)
			nick = "\"" + nick + "\"";
		m_Win.m_Edit.SetText("/pm " + nick + " ");
		UpdateCounter();
		FocusEdit();
	}

	protected void PopupCopy()
	{
		INT_GC_Line line = m_PopupLine;
		ClosePopup();
		if (!line)
			return;
		GetGame().CopyToClipboard(line.Name);
		INT_GC_Client.Notice(INT_GC_Client.Fmt1("#STR_INT_GC_COPIED", line.Name), 0);
		FocusEdit();
	}

	protected void PopupBlock()
	{
		INT_GC_Line line = m_PopupLine;
		ClosePopup();
		if (!line || line.Mine)
			return;
		string name = line.Name;
		string id = line.SenderId;
		INT_GC_UI.Block(id, name);
		INT_GC_Client.RemoveFrom(id, name);
		INT_GC_View view = INT_GC_View.Peek();
		if (view)
			view.OnHistoryChanged();
		if (m_Win && m_Win.m_Area)
			m_Win.m_Area.Refresh();
		INT_GC_Client.Notice(INT_GC_Client.Fmt1("#STR_INT_GC_BLOCKED", name), 0);
		if (m_Win)
			m_Win.UpdateScroll();
		FocusEdit();
	}

	// --- self-test ------------------------------------------------------------------------------------------------
	override void OnTestCommand(string command)
	{
		if (command == "popup" && m_Win && m_Win.m_Area)
		{
			array<ref INT_GC_Line> hist = INT_GC_Client.History();
			for (int i = hist.Count() - 1; i >= 0; i--)
			{
				INT_GC_Line l = hist.Get(i);
				if (l && l.Name != "" && !l.Mine)
				{
					OpenPopup(l, 700, 600);
					return;
				}
			}
		}
	}

	// Typing from the self-test (the same path as the keyboard): text, then Enter.
	void TestType(string text, bool send)
	{
		if (!m_Win || !m_Win.m_Edit)
			return;
		m_Win.m_Edit.SetText(text);
		HandleChange(m_Win.m_Edit, false);
		if (send)
		{
			m_OpenedAt = -10;
			HandleChange(m_Win.m_Edit, true);
		}
	}

	void TestDrag(float dxPx, float dyPx, bool resize)
	{
		if (!m_Win)
			return;
		INT_GC_Style st = INT_GC_UI.Style();
		float sx;
		float sy;
		INT_GC_Window.ScreenPos(sx, sy);
		float scrW = INT_UIF_Scale.ScreenW();
		float scrH = INT_UIF_Scale.ScreenH();
		if (resize)
		{
			float f = INT_UIF_Scale.Factor();
			st.X = sx / scrW;
			st.Y = sy / scrH;
			st.W = st.W + dxPx / f;
			st.H = st.H + dyPx / f;
			st.Sanitize();
			Rebuild();
		}
		else
		{
			st.X = Math.Clamp((sx + dxPx) / scrW, 0, 1);
			st.Y = Math.Clamp((sy + dyPx) / scrH, 0, 1);
			st.Sanitize();
			float nx;
			float ny;
			INT_GC_Window.ScreenPos(nx, ny);
			m_Win.MoveTo(nx, ny);
		}
		INT_GC_UI.SavePlayerLayout();
	}

	int TestChannel()
	{
		return m_Channel;
	}
}
