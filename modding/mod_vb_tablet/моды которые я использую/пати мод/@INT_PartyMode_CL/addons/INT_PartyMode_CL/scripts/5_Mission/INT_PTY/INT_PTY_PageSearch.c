// INT PartyMode (client) — tab «Игроки в поиске» (3_c_expedition_search.png): left — the big switch «Активировать
// поиск группы» with the live indicator, the hint and three numbered steps; right — the players who switched search
// on («ПРИГЛАСИТЬ» / «ЗАЯВКА ОТПРАВЛЕНА») in a pixel-smooth list (up to 60 rows). The list is requested when the tab
// opens, by «Обновить» and every 20 s while the tab is visible (each request counts toward the server's actions per
// minute).

class INT_PTY_SearchRowW : Managed
{
	Widget Root;
	TextWidget Index;
	ImageWidget Hover;
	TextWidget Nick;
	TextWidget When;
	string Id;
	int Secs;
	float At;
	ref INT_PTY_MButton Btn;
	bool Invited;
	bool Bound;
}

class INT_PTY_PageSearch : INT_PTY_Page
{
	static const float RX = 880;
	static const float RY = 356;
	static const float RH = 90;
	static const int VISIBLE = 7;
	static const float AUTO_S = 20;
	static const float ASK_GAP_S = 15;

	// W-3: the last sent list request, per game session (the page object is rebuilt on every tab switch), so
	// switching tabs cannot burn the server's actions per minute. Reset by INT_PTY_MenuWatch.Reset().
	protected static float s_LastAsk = -1000;

	protected ref INT_PTY_MToggle m_Switch;
	protected Widget m_Ind;
	protected Widget m_ListW;
	protected Widget m_Empty;
	protected TextWidget m_Count;
	protected ref array<ref INT_PTY_SearchRowW> m_Rows;
	protected ref INT_PTY_MScroll m_Sc;
	protected ref INT_PTY_MButton m_Refresh;
	protected ButtonWidget m_RefreshHit;
	protected float m_AutoAcc;
	protected float m_TimeAcc;
	protected int m_BlockShown = -1;
	protected bool m_Asked;
	protected int m_SwitchMismatch;

	void INT_PTY_PageSearch()
	{
		m_Rows = new array<ref INT_PTY_SearchRowW>();
	}

	protected int Count()
	{
		if (!INT_PTY_Client.s_SearchList)
			return 0;
		return INT_PTY_Client.s_SearchList.Count();
	}

	override void Build(bool animate)
	{
		float d = m_Menu.PageDelay();
		BuildLeft(animate, d);
		BuildRightHead(animate, d + 0.06);
		m_ListW = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_SList");
		m_Rows.Clear();
		m_Sc = new INT_PTY_MScroll();
		m_Sc.Init(m_Menu, this, 1, "slist", m_ListW, RX, RY, 960, VISIBLE * RH, RH, RX - 18);
		m_Empty = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_SEmpty");
		RebindList(false);
		if (animate)
		{
			m_Sc.Enter(d + 0.12);
			INT_PTY_MenuFx.BlockIn(m_Empty, 0, 12, d + 0.12, 0.3);
		}
		if (!m_Asked)
		{
			m_Asked = true;
			Request();
			// the auto-refresh counts from the last request really sent (it may be a few seconds old)
			float since = SinceAsk();
			if (since < AUTO_S)
				m_AutoAcc = since;
		}
	}

	static void ResetSession()
	{
		s_LastAsk = -1000;
	}

	// Seconds since the last list request of this game session (a clock that went back counts as long ago).
	protected static float SinceAsk()
	{
		float now = INT_PTY_Client.Clock();
		if (now < s_LastAsk)
			s_LastAsk = -1000;
		return now - s_LastAsk;
	}

	// Entering the tab, «Обновить» and the auto-refresh all ask through here: never more often than ASK_GAP_S.
	protected void Request()
	{
		m_AutoAcc = 0;
		if (!INT_PTY_Client.IsWelcomed() || INT_PTY_Client.IsBlocked())
			return;
		if (SinceAsk() < ASK_GAP_S)
			return;
		s_LastAsk = INT_PTY_Client.Clock();
		INT_PTY_Client.RequestSearchList();
	}

	// --- left: status, switch, hint, steps ----------------------------------------------------------------------
	protected void BuildLeft(bool animate, float d)
	{
		Widget p = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_SLeft");
		INT_PTY_MD.Caps(p, 80, 246, 400, 20, "#STR_INT_PTY_W_S_STATUS", 13, INT_PTY_MC.O);
		INT_PTY_MD.Line(p, 80, 296, 700, 1, INT_PTY_MC.L);
		m_Switch = INT_PTY_MToggle.Create(p, 80, 334, 92, 48, INT_PTY_Client.s_Searching);
		TextWidget title = INT_PTY_MD.Txt(p, 196, 318, 600, 48, "#STR_INT_PTY_W_S_SWITCH", 34, INT_PTY_MC.T);
		m_Ind = INT_UIF_Draw.Frame(p, 0, 0, 1920, 1080, "INT_PTY_SInd");
		PaintInd();
		INT_PTY_MD.Line(p, 80, 418, 700, 1, INT_PTY_MC.L);
		ButtonWidget hit = m_Menu.Hit(p, 80, 318, 700, 90, "search:toggle");
		m_Menu.SetHoverText(hit, title, INT_PTY_MC.T, INT_PTY_MC.O_LIGHT);
		m_Menu.SetTip(hit, "#STR_INT_PTY_W_TIP_SWITCH_H", "#STR_INT_PTY_W_TIP_SWITCH", 380);
		INT_PTY_MD.Ico(p, 80, 448, 20, "info", INT_PTY_MC.O);
		float hh = INT_PTY_MD.Para(p, 114, 444, 650, "#STR_INT_PTY_W_S_HINT", 18, INT_PTY_MC.BODY, 26, 3);
		// steps (.steps): diamond nodes on a hairline
		float y = 444 + hh + 40;
		float first = y + 14;
		float last = first;
		Widget steps = INT_UIF_Draw.Frame(p, 0, 0, 1920, 1080, "INT_PTY_SSteps");
		ImageWidget stepLine = INT_PTY_MD.Line(steps, 86, first, 1, 10, INT_PTY_MC.L2);
		for (int i = 1; i <= 3; i++)
		{
			last = y + 14;
			INT_PTY_MD.Dm(steps, 86, last, 13, INT_PTY_MC.BG);
			INT_PTY_MD.DmLine(steps, 86, last, 12, INT_PTY_MC.O);
			string num = INT_PTY_Text.Spaced(INT_PTY_Text.Pad2(i));
			INT_PTY_Look.Text(steps, 118, y + 4, 80, 18, num, 13, INT_PTY_MC.O, INT_UIF_Align.LEFT, true);
			string key = "#STR_INT_PTY_W_S_STEP_" + i.ToString();
			float th = INT_PTY_MD.Para(steps, 118, y + 26, 620, key, 18, INT_PTY_MC.T, 25, 3);
			y = y + 26 + th + 22;
		}
		if (stepLine)
			INT_UIF_Draw.Resize(stepLine, 1, last - first);
		if (!animate)
			return;
		INT_PTY_MenuFx.BlockIn(p, 0, 14, d, 0.32);
		INT_PTY_MenuFx.GrowH(stepLine, d + 0.2, 0.4);
	}

	protected void PaintInd()
	{
		if (!m_Ind)
			return;
		INT_UIF_Widgets.ClearChildren(m_Ind);
		string lab = "#STR_INT_PTY_W_SEARCH_OFF";
		int col = INT_PTY_MC.M;
		bool on = INT_PTY_Client.s_Searching;
		if (on)
		{
			lab = "#STR_INT_PTY_W_SEARCH_ON";
			col = INT_PTY_MC.G;
			ImageWidget glow = INT_PTY_MD.Glow(m_Ind, 203, 383, 30, ARGB(140, 86, 224, 126));
			ImageWidget dot = INT_PTY_MD.Dot(m_Ind, 203, 383, 10, INT_PTY_MC.G);
			if (glow)
				glow.SetName(INT_PTY_MenuFx.NO_FADE);
			if (dot)
				dot.SetName(INT_PTY_MenuFx.NO_FADE);
			INT_UIF_Animator.Pulse(dot, 0.15, 1, 1.0);
			INT_UIF_Animator.Pulse(glow, 0.05, 0.55, 1.0);
		}
		else
		{
			INT_PTY_MD.Dot(m_Ind, 203, 383, 10, INT_PTY_MC.OFF);
		}
		INT_PTY_MD.Caps(m_Ind, 222, 373, 400, 22, lab, 14, col);
	}

	// --- right: counter + list ------------------------------------------------------------------------------------
	protected void BuildRightHead(bool animate, float d)
	{
		Widget p = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_SHead");
		m_Count = INT_PTY_MD.Txt(p, RX - 2, 262, 120, 82, INT_PTY_Text.Pad2(Count()), 64, INT_PTY_MC.T, INT_UIF_Align.LEFT, false, true);
		INT_PTY_MD.Caps(p, RX + 89, 310, 500, 20, "#STR_INT_PTY_W_S_COUNT", 13, INT_PTY_MC.M);
		m_Refresh = INT_PTY_MButton.Create(p, 1840, 300, 36, "reset", "#STR_INT_PTY_W_S_REFRESH", INT_PTY_MButton.GH, true, 13);
		m_RefreshHit = m_Menu.Hit(m_Refresh.m_Root, 0, 0, m_Refresh.m_W, m_Refresh.m_H, "search:refresh");
		m_Menu.SetHoverBtn(m_RefreshHit, m_Refresh);
		m_Menu.SetTip(m_RefreshHit, "#STR_INT_PTY_W_TIP_REFRESH_H", "#STR_INT_PTY_W_TIP_REFRESH", 360);
		INT_PTY_MD.Line(p, RX, 356, 960, 1, INT_PTY_MC.L);
		if (animate)
			INT_PTY_MenuFx.BlockIn(p, 0, 14, d, 0.32);
	}

	// New data: the count, the empty state, the rows (the offset stays where the player scrolled).
	protected void RebindList(bool keep)
	{
		int n = Count();
		if (m_Count)
			INT_PTY_Look.SetHeavyText(m_Count, INT_PTY_Text.Pad2(n));
		if (m_Empty)
		{
			INT_UIF_Widgets.ClearChildren(m_Empty);
			if (n == 0)
			{
				INT_PTY_MD.DmLine(m_Empty, RX + 6, 404, 12, INT_PTY_MC.D);
				INT_PTY_MD.Txt(m_Empty, RX + 32, 384, 900, 40, "#STR_INT_PTY_W_S_EMPTY_T", 28, INT_PTY_MC.M);
				INT_PTY_MD.Para(m_Empty, RX + 32, 430, 820, "#STR_INT_PTY_W_S_EMPTY", 17, INT_PTY_MC.D, 25, 3);
			}
		}
		m_BlockShown = INT_PTY_Client.BlockedSeconds();
		if (m_Sc)
			m_Sc.SetCount(n, keep);
	}

	override void ScrollBuild(int sid, int slot, Widget root)
	{
		INT_PTY_SearchRowW w = new INT_PTY_SearchRowW();
		w.Root = root;
		Widget p = root;
		w.Hover = INT_PTY_Look.Tex(p, 0, 0, 960, RH, "pty_grad_h_ca", ARGB(30, 255, 122, 47));
		if (w.Hover)
			w.Hover.SetRotation(0, 0, 180);
		INT_PTY_MD.Line(p, 0, RH - 1, 960, 1, ARGB(18, 255, 255, 255));
		w.Index = INT_PTY_Look.Text(p, 0, 34, 60, 22, "", 15, INT_PTY_MC.D, INT_UIF_Align.LEFT, true);
		INT_PTY_MD.DmLine(p, 76, 45, 12, INT_PTY_MC.G);
		w.Nick = INT_PTY_MD.Txt(p, 102, 26, 330, 40, "", 28, INT_PTY_MC.T);
		INT_PTY_MD.Ico(p, 432, 37, 16, "clock", INT_PTY_MC.M);
		w.When = INT_PTY_MD.Txt(p, 457, 32, 260, 26, "", 15, INT_PTY_MC.M);
		ButtonWidget hit = m_Menu.Hit(p, 0, 0, 760, RH, "srow:" + slot.ToString());
		m_Menu.SetSound(hit, false);
		m_Sc.SetSlotHover(slot, w.Hover);
		m_Rows.Insert(w);
	}

	override void ScrollFill(int sid, int slot, int index)
	{
		if (slot < 0 || slot >= m_Rows.Count() || index < 0 || index >= Count())
			return;
		INT_PTY_SearchRowW w = m_Rows.Get(slot);
		INT_PTY_SearchRow r = INT_PTY_Client.s_SearchList.Get(index);
		if (!r)
			return;
		w.Id = r.Id;
		w.Secs = r.Secs;
		w.At = r.At;
		w.Invited = r.Invited;
		w.Bound = true;
		string ix = INT_PTY_Text.Spaced(INT_PTY_Text.Pad2(index + 1));
		string nick = INT_PTY_Look.FitText(r.Name, 28, false, 320);
		if (w.Index)
			w.Index.SetText(ix);
		if (w.Nick)
			w.Nick.SetText(nick);
		RepaintWhen(w);
		BuildAction(w);
	}

	override void ScrollHover(int sid, int slot, bool on)
	{
		if (slot < 0 || slot >= m_Rows.Count())
			return;
		INT_PTY_SearchRowW w = m_Rows.Get(slot);
		if (w.Btn && !w.Invited && !INT_PTY_Client.IsBlocked())
			w.Btn.SetHover(on);
	}

	protected void BuildAction(INT_PTY_SearchRowW w)
	{
		if (w.Btn && w.Btn.m_Root)
		{
			m_Menu.DropHitsUnder(INT_PTY_Menu.SEC_PAGE, w.Btn.m_Root);
			w.Btn.m_Root.Unlink();
		}
		w.Btn = null;
		if (w.Invited)
		{
			w.Btn = INT_PTY_MButton.Create(w.Root, 960, 23, 44, "check", "#STR_INT_PTY_W_S_SENT", INT_PTY_MButton.DIS, true);
			ButtonWidget sh = m_Menu.Hit(w.Btn.m_Root, 0, 0, w.Btn.m_W, w.Btn.m_H, "sent:" + w.Id);
			m_Menu.SetOff(sh, true);
			m_Menu.SetTip(sh, "#STR_INT_PTY_W_TIP_SENT_H", "#STR_INT_PTY_W_TIP_SENT", 340);
			return;
		}
		if (INT_PTY_Client.IsBlocked())
		{
			int secs = INT_PTY_Client.BlockedSeconds();
			string ss = secs.ToString();
			string wait = INT_PTY_Text.Fmt("#STR_INT_PTY_W_WAIT", ss);
			string tip = INT_PTY_Text.Fmt("#STR_INT_PTY_W_TIP_BLOCK", ss);
			w.Btn = INT_PTY_MButton.Create(w.Root, 960, 23, 44, "hourglass", wait, INT_PTY_MButton.DIS, true);
			ButtonWidget bh = m_Menu.Hit(w.Btn.m_Root, 0, 0, w.Btn.m_W, w.Btn.m_H, "blocked");
			m_Menu.SetOff(bh, true);
			m_Menu.SetTip(bh, "#STR_INT_PTY_W_TIP_BLOCK_H", tip, 360);
			return;
		}
		w.Btn = INT_PTY_MButton.Create(w.Root, 960, 23, 44, "user_plus", "#STR_INT_PTY_W_S_INVITE", INT_PTY_MButton.OL, true);
		ButtonWidget ih = m_Menu.Hit(w.Btn.m_Root, 0, 0, w.Btn.m_W, w.Btn.m_H, "invite:" + w.Id);
		m_Menu.SetHoverBtn(ih, w.Btn);
		m_Menu.SetTip(ih, "#STR_INT_PTY_W_TIP_INVITE_H", "#STR_INT_PTY_W_TIP_INVITE", 340);
	}

	protected void RepaintWhen(INT_PTY_SearchRowW w)
	{
		if (!w.When)
			return;
		float secs = w.Secs + (INT_PTY_Client.Clock() - w.At);
		string span = INT_PTY_MenuGeo.Span(secs);
		w.When.SetText(INT_PTY_Text.Fmt("#STR_INT_PTY_W_S_IN", span));
	}

	// The rate limit: buttons that would send turn into a countdown and back.
	protected void RepaintBlock(bool force)
	{
		int b = INT_PTY_Client.BlockedSeconds();
		if (!force && b == m_BlockShown)
			return;
		m_BlockShown = b;
		if (!m_Sc)
			return;
		for (int i = 0; i < m_Rows.Count(); i++)
		{
			INT_PTY_SearchRowW w = m_Rows.Get(i);
			if (m_Sc.SlotIndex(i) < 0 || w.Invited)
				continue;
			BuildAction(w);
			m_Sc.Recapture(i);
		}
	}

	// --- events ---------------------------------------------------------------------------------------------------
	override bool OnClick(string key, Widget w)
	{
		if (key == "search:toggle")
		{
			if (!INT_PTY_Client.IsWelcomed())
				return true;
			if (INT_PTY_Client.IsBlocked())
			{
				INT_PTY_Client.SetSearch(!INT_PTY_Client.s_Searching);
				return true;
			}
			bool want = !INT_PTY_Client.s_Searching;
			if (m_Switch)
				m_Switch.Set(want, true);
			INT_PTY_Sfx.Toggle();
			INT_PTY_Client.SetSearch(want);
			return true;
		}
		if (key == "search:refresh")
		{
			INT_PTY_Sfx.Click();
			if (INT_PTY_Client.IsBlocked())
			{
				INT_PTY_Client.RequestSearchList();
				return true;
			}
			Request();
			return true;
		}
		if (key.IndexOf("invite:") == 0)
		{
			string id = key.Substring(7, key.Length() - 7);
			INT_PTY_Sfx.Click();
			INT_PTY_Client.Invite(id);
			return true;
		}
		return false;
	}

	override bool OnWheel(string key, int wheel)
	{
		bool mine = false;
		if (key.IndexOf("slist:") == 0 || key.IndexOf("srow:") == 0)
			mine = true;
		if (key.IndexOf("invite:") == 0 || key.IndexOf("sent:") == 0 || key == "blocked")
			mine = true;
		if (!mine)
			return false;
		if (m_Sc)
			m_Sc.Wheel(wheel);
		return true;
	}

	override bool OnDown(string key)
	{
		if (key != "slist:bar" || !m_Sc)
			return false;
		m_Sc.BarDown();
		return true;
	}

	override void OnSlide(string key, int mx, bool done)
	{
		if (key == "slist:bar" && m_Sc)
			m_Sc.BarDrag(done);
	}

	override void TestScroll(float px)
	{
		if (m_Sc)
			m_Sc.Jump(px);
	}

	override void OnData(int what)
	{
		if ((what & INT_PTY_What.SEARCH) != 0)
		{
			if (m_Switch && m_Switch.m_On != INT_PTY_Client.s_Searching)
				m_Switch.Set(INT_PTY_Client.s_Searching, true);
			PaintInd();
			if (m_Ind)
				INT_PTY_MenuFx.BlockIn(m_Ind, 10, 0, 0, 0.24);
		}
		if ((what & INT_PTY_What.SEARCH_LIST) != 0)
			RebindList(true);
		if ((what & INT_PTY_What.BLOCK) != 0)
		{
			RepaintBlock(true);
			// a refused toggle (rate limit) must not leave the switch flipped for the whole block
			if (m_Switch && m_Switch.m_On != INT_PTY_Client.s_Searching)
				m_Switch.Set(INT_PTY_Client.s_Searching, true);
		}
	}

	override void Tick(float dt)
	{
		if (m_Sc)
			m_Sc.Tick(dt);
		m_AutoAcc = m_AutoAcc + dt;
		if (m_AutoAcc >= AUTO_S)
			Request();
		m_TimeAcc = m_TimeAcc + dt;
		if (m_TimeAcc < 1.0)
			return;
		m_TimeAcc = 0;
		if (m_Sc)
		{
			for (int i = 0; i < m_Rows.Count(); i++)
			{
				if (m_Sc.SlotIndex(i) >= 0)
					RepaintWhen(m_Rows.Get(i));
			}
		}
		RepaintBlock(false);
		// the switch follows the server state when a request was refused
		if (m_Switch && m_Switch.m_On != INT_PTY_Client.s_Searching && !INT_PTY_Client.IsBlocked())
			m_SwitchMismatch = m_SwitchMismatch + 1;
		else
			m_SwitchMismatch = 0;
		if (m_SwitchMismatch >= 3 && m_Switch)
		{
			m_Switch.Set(INT_PTY_Client.s_Searching, true);
			m_SwitchMismatch = 0;
		}
	}

	override string TestInfo()
	{
		int on = 0;
		if (INT_PTY_Client.s_Searching)
			on = 1;
		int rows = 0;
		float off = 0;
		if (m_Sc)
		{
			off = m_Sc.Offset();
			for (int i = 0; i < m_Rows.Count(); i++)
			{
				int idx = m_Sc.SlotIndex(i);
				if (idx >= 0 && m_Sc.ItemFade(idx) > 0.5)
					rows++;
			}
		}
		int offI = (int)off;
		return "search list=" + Count().ToString() + " rows=" + rows.ToString() + " searching=" + on.ToString() + " scroll=" + offI.ToString();
	}
}
