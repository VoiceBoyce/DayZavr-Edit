// Ранги клана: порядок в списке это старшинство. Правки уходят на сервер сразу, как в макете.
// Страница правит копию рангов у клиента, сервер отвечает новой шапкой клана.
class VB_TabletPageRanks extends VB_TabletPage
{
	protected static const int TAG_DELETE = 1;
	// Права в порядке битов VB_ClanPerm. Девятый бит (банкомат клана) убран вместе с банкоматом.
	protected static const int PERM_COUNT = 8;
	protected static const int NAME_MAX = 20;

	// Перестроение создаёт страницу заново, поэтому выбранный ранг в статике.
	protected static string s_Sel = "leader";
	protected static string s_DelId;

	protected bool m_CanEdit;
	protected bool m_Lead;
	protected int m_Level;
	protected int m_TopRank;
	protected bool m_NameDirty;
	protected bool m_Stale;
	protected Widget m_Left;
	protected ref VBT_Input m_Name;
	protected ref VBT_Button m_Add;
	protected ref VBT_Button m_Del;
	protected ref array<Widget> m_RowHits = new array<Widget>();
	protected ref array<Widget> m_RowFrames = new array<Widget>();
	protected ref array<ref INT_UIF_Surface> m_RowBg = new array<ref INT_UIF_Surface>();
	protected ref array<Widget> m_ColorHits = new array<Widget>();
	protected ref array<ref VBT_Switch> m_Switches = new array<ref VBT_Switch>();
	protected ref array<int> m_SwitchBits = new array<int>();

	// Перетаскивание: кнопка мыши опрашивается в Tick, у фреймворка нет события нажатия.
	protected int m_HoverRow = -1;
	protected bool m_Down;
	protected int m_DragFrom = -1;
	protected int m_DragTo;
	protected int m_DragY0;
	protected bool m_Dragging;
	protected bool m_Dragged;
	protected Widget m_DragLine;

	protected bool Edits(VB_Clan c, VB_ClanRank k)
	{
		if (!VB_TabletClient.Get().Can(VB_ClanPerm.RANKS))
			return false;
		return m_Lead || c.RankIndex(k.Id) > m_Level;
	}

	protected bool IsFree(VB_Clan c, int i)
	{
		return i > 0 && i < c.Ranks.Count() - 1 && Edits(c, c.Ranks.Get(i));
	}

	override void Build()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c)
		{
			m_Menu.GoLater("profile");
			return;
		}
		string uid = client.MyUid();
		VB_ClanMember me = c.Member(uid);
		if (!me)
		{
			client.Request(VB_TabletOp.CLAN);
			return;
		}
		m_CanEdit = client.Can(VB_ClanPerm.RANKS);
		m_Lead = c.IsLeader(uid);
		m_Level = c.RankIndex(me.RankId);
		m_TopRank = m_Level + 1;
		if (m_Lead)
			m_TopRank = 1;
		if (c.Rank(s_Sel).Id != s_Sel)
			s_Sel = c.Ranks.Get(0).Id;
		BuildLeft(c);
		BuildRight(c, c.Rank(s_Sel));
	}

	protected int MembersOf(VB_Clan c, string id)
	{
		int cnt = 0;
		foreach (VB_ClanMember m : c.Members)
		{
			if (c.Rank(m.RankId).Id == id)
				cnt++;
		}
		return cnt;
	}

	protected static int PermCount(VB_ClanRank r)
	{
		int cnt = 0;
		for (int i = 0; i < PERM_COUNT; i++)
		{
			if (r.Can(1 << i))
				cnt++;
		}
		return cnt;
	}

	protected void BuildLeft(VB_Clan c)
	{
		Widget L = VBT.Card(m_Root, 0, 0, 400, 720);
		m_Left = L;
		int n = c.Ranks.Count();
		int most = VB_CLAN_RANKS_MAX;
		int allPerms = PERM_COUNT;
		VBT.Cap(L, 20, 18, 240, VBT.T("#STR_VB_TAB_B_RANKS_C") + " · " + n.ToString() + "/" + most.ToString());
		for (int i = 0; i < n; i++)
		{
			VB_ClanRank x = c.Ranks.Get(i);
			bool on = x.Id == s_Sel;
			float y = 48 + i * 70;
			Widget f = INT_UIF_Draw.Frame(L, 12, y, 376, 62);
			int fill = VB_C_PANEL2;
			int border = 0;
			if (on)
			{
				fill = VB_C_PANEL3;
				border = VB_C_ACCENT;
			}
			m_RowBg.Insert(INT_UIF_Surface.Box(f, 0, 0, 376, 62, 8, fill, border));
			INT_UIF_Surface.Box(f, 0, 12, 4, 38, 2, x.Color);
			string title = VBT.T(x.Title);
			INT_UIF_Draw.Text(f, 18, 8, 250, 26, VBT.Fit(title, 250, 18), 18, VB_C_TEXT);
			int cnt = MembersOf(c, x.Id);
			int perms = PermCount(x);
			string line = VBB.Sub("#STR_VB_TAB_B_RANK_LINE", cnt.ToString(), perms.ToString());
			INT_UIF_Draw.Text(f, 18, 34, 250, 20, line + "/" + allPerms.ToString(), 13, VB_C_DIM);
			if (IsFree(c, i))
				VBT.Ico(f, 340, 20, 22, "grip", VB_C_DIM);
			else if (m_CanEdit)
				VBT.Ico(f, 344, 23, 16, "lock", VB_C_DIM2);
			m_RowHits.Insert(HitAt(f, 0, 0, 376, 62));
			m_RowFrames.Insert(f);
		}
		if (!m_CanEdit)
			return;
		INT_UIF_Draw.MText(L, 20, n * 70 + 56, 360, 48, "#STR_VB_TAB_B_RANKS_HINT", 14, VB_C_DIM2);
		m_Add = new VBT_Button(L, 20, 656, 360, 46, "#STR_VB_TAB_B_RANK_NEW", false, "plus", m_Menu.GetHitHandler(), 14);
		if (n >= VB_CLAN_RANKS_MAX || m_TopRank > n - 1)
			VBB.Disable(L, m_Add, 20, 656, 360, 46);
	}

	protected void BuildRight(VB_Clan c, VB_ClanRank k)
	{
		bool lead = k.Id == "leader";
		bool isBase = k.Id == "member";
		bool edk = Edits(c, k);
		VB_TabletClient client = VB_TabletClient.Get();
		Widget R = INT_UIF_Draw.Frame(m_Root, 416, 0, 756, 720);
		INT_UIF_Surface.Box(R, 0, 0, 756, 720, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(R, 756, 720);
		VBT.Cap(R, 24, 20, 300, "#STR_VB_TAB_B_RANK_NAME");
		string title = VBT.T(k.Title);
		if (edk)
			m_Name = new VBT_Input(R, 24, 44, 400, 48, "#STR_VB_TAB_B_RANK_NAME_HINT", NAME_MAX, title);
		else
			INT_UIF_Draw.Text(R, 24, 44, 400, 48, title, 28, VB_C_TEXT);

		VBT.Cap(R, 452, 20, 280, "#STR_VB_TAB_B_RANK_COLOR");
		array<int> colors = VBK.Colors();
		int i;
		for (i = 0; i < colors.Count(); i++)
		{
			int col = colors.Get(i);
			int cc = i % 6;
			int cr = i / 6;
			float cx = 452 + cc * 46;
			float cy = 44 + cr * 42;
			if (k.Color == col)
				INT_UIF_Surface.Pill(R, cx - 3, cy - 3, 38, 38, 0, VB_C_TEXT);
			INT_UIF_Surface.Pill(R, cx + 1, cy + 1, 30, 30, col);
			if (edk)
				m_ColorHits.Insert(HitAt(R, cx - 2, cy - 2, 36, 36));
		}

		INT_UIF_Draw.Line(R, 24, 142, 708, 1, VB_C_LINE);
		VBT.Cap(R, 24, 158, 400, "#STR_VB_TAB_B_RANK_ACCESS");
		for (i = 0; i < PERM_COUNT; i++)
		{
			int bit = 1 << i;
			int pc = i % 2;
			int pr = i / 2;
			float x = 24 + pc * 362;
			float y = 180 + pr * 57;
			bool on = k.Can(bit);
			int ink = VB_C_DIM;
			if (on)
				ink = VB_C_TEXT;
			INT_UIF_Surface.Box(R, x, y, 346, 51, 8, VB_C_PANEL2);
			INT_UIF_Draw.Text(R, x + 14, y + 5, 262, 22, "#STR_VB_TAB_B_P_" + i.ToString(), 15, ink);
			INT_UIF_Draw.Text(R, x + 14, y + 27, 262, 18, "#STR_VB_TAB_B_PS_" + i.ToString(), 12, VB_C_DIM2);
			if (lead)
			{
				VBT.Ico(R, x + 310, y + 16, 18, "lock", VB_C_DIM2);
				continue;
			}
			VBT_Switch sw = new VBT_Switch(R, x + 288, y + 13, on, m_Menu.GetHitHandler());
			m_Switches.Insert(sw);
			m_SwitchBits.Insert(bit);
			if (edk && client.Can(bit))
				continue;
			sw.Hit.Show(false);
			// Право, которого нет у самого игрока, приглушено; чужой ранг просто только для чтения.
			if (edk)
				INT_UIF_Draw.Line(R, x + 288, y + 13, 44, 24, INT_UIF_Color.WithAlpha(VB_C_PANEL2, 153));
		}

		INT_UIF_Draw.Line(R, 24, 472, 708, 1, VB_C_LINE);
		int cnt = MembersOf(c, k.Id);
		VBT.Cap(R, 24, 488, 400, VBT.T("#STR_VB_TAB_B_RANK_WITH") + " · " + cnt.ToString());
		float px = 24;
		float py = 514;
		if (cnt == 0)
			INT_UIF_Draw.Text(R, 24, py, 400, 26, "#STR_VB_TAB_B_RANK_NOBODY", 15, VB_C_DIM2);
		foreach (VB_ClanMember m : c.Members)
		{
			if (c.Rank(m.RankId).Id != k.Id)
				continue;
			float pw = VBK.PillW(R, m.PlayerName);
			if (px + pw > 732)
			{
				px = 24;
				py += 34;
			}
			VBK.Pill(R, px, py, m.PlayerName, k.Color);
			px += pw + 8;
		}

		string note = "#STR_VB_TAB_B_RANK_NOTE";
		if (lead)
			note = "#STR_VB_TAB_B_RANK_NOTE_LEADER";
		else if (isBase)
			note = "#STR_VB_TAB_B_RANK_NOTE_MEMBER";
		INT_UIF_Draw.MText(R, 24, 596, 700, 44, note, 14, VB_C_DIM);
		bool locked = lead || isBase;
		if (edk && !locked)
		{
			m_Del = new VBT_Button(R, 24, 656, 200, 46, "#STR_VB_TAB_B_RANK_DEL", false, "trash", m_Menu.GetHitHandler(), 14);
			m_Del.Danger = true;
			m_Del.Bg.SetFill(VB_C_BAD);
		}
		if (!edk)
		{
			string why = "#STR_VB_TAB_B_RANK_NO_RIGHT";
			if (m_CanEdit)
				why = "#STR_VB_TAB_B_RANK_ONLY_LOWER";
			INT_UIF_Draw.Text(R, 24, 664, 700, 30, why, 14, VB_C_DIM2);
		}
		else if (!m_Lead)
		{
			float tx = 240;
			if (locked)
				tx = 24;
			INT_UIF_Draw.Text(R, tx, 664, 480, 30, "#STR_VB_TAB_B_RANK_OWN_PERMS", 14, VB_C_DIM2);
		}
	}

	protected void Send()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		array<ref VB_ClanRank> ranks = client.Clan.Ranks;
		ScriptRPC rpc = client.Begin(VB_TabletOp.CLAN_RANKS);
		int n = ranks.Count();
		rpc.Write(n);
		foreach (VB_ClanRank r : ranks)
			r.Write(rpc);
		client.Post(rpc);
	}

	protected void Repaint()
	{
		SetFocus(null);
		m_Menu.RequestRepaint();
	}

	// Название уходит, когда игрок закончил ввод: Enter, клик мимо поля или уход с экрана.
	protected void FlushName()
	{
		if (!m_NameDirty || !m_Name)
			return;
		m_NameDirty = false;
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (!c)
			return;
		VB_ClanRank k = c.Rank(s_Sel);
		string v = m_Name.Get();
		if (k.Id != s_Sel || v == "" || v == VBT.T(k.Title))
		{
			m_Stale = true;
			return;
		}
		k.Title = v;
		Send();
		m_Stale = true;
	}

	protected void AddRank()
	{
		VB_Clan c = VB_TabletClient.Get().Clan;
		string id;
		bool busy = true;
		while (busy)
		{
			int num = Math.RandomInt(1000, 1000000);
			id = "r" + num.ToString();
			busy = c.Rank(id).Id == id;
		}
		array<int> colors = VBK.Colors();
		// Текстом, а не ключом: название уйдёт на сервер с лимитом 20 символов.
		string title = VBT.T("#STR_VB_TAB_B_RANK_DEFAULT");
		VB_ClanRank k = new VB_ClanRank(id, title, colors.Get(5), 0);
		int at = c.Ranks.Count() - 1;
		c.Ranks.InsertAt(k, at);
		s_Sel = id;
		Send();
		Repaint();
	}

	// --- перетаскивание ------------------------------------------------------------------------
	protected void PollDrag()
	{
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (!c)
			return;
		bool down = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) != 0;
		int mx;
		int my;
		GetMousePos(mx, my);
		if (!m_Down)
		{
			if (!down)
				return;
			m_Down = true;
			m_Dragged = false;
			if (m_HoverRow >= 0 && IsFree(c, m_HoverRow))
			{
				m_DragFrom = m_HoverRow;
				m_DragY0 = my;
			}
			return;
		}
		if (!down)
		{
			m_Down = false;
			if (m_Dragging)
				EndDrag(c);
			m_DragFrom = -1;
			return;
		}
		if (m_DragFrom < 0)
			return;
		float d = (my - m_DragY0) / INT_UIF_Scale.Factor();
		if (!m_Dragging)
		{
			if (Math.AbsFloat(d) < 4)
				return;
			m_Dragging = true;
			m_RowFrames.Get(m_DragFrom).SetSort(100);
			m_DragLine = INT_UIF_Draw.Line(m_Left, 12, 0, 376, 3, VB_C_ACCENT);
			m_DragLine.SetSort(99);
			m_DragTo = m_DragFrom;
		}
		int n = c.Ranks.Count();
		float top = Math.Clamp(48 + m_DragFrom * 70 + d, 48 + m_TopRank * 70, 48 + (n - 2) * 70);
		INT_UIF_Draw.Move(m_RowFrames.Get(m_DragFrom), 12, top);
		m_DragTo = Math.Clamp(Math.Round((top - 48) / 70), m_TopRank, n - 2);
		float gap = -5;
		if (m_DragTo > m_DragFrom)
			gap = 66;
		INT_UIF_Draw.Move(m_DragLine, 12, 48 + m_DragTo * 70 + gap);
	}

	protected void EndDrag(VB_Clan c)
	{
		m_Dragging = false;
		m_Dragged = true;
		int i = m_DragFrom;
		while (i < m_DragTo)
		{
			c.Ranks.SwapItems(i, i + 1);
			i++;
		}
		while (i > m_DragTo)
		{
			c.Ranks.SwapItems(i, i - 1);
			i--;
		}
		if (m_DragTo != m_DragFrom)
			Send();
		Repaint();
	}

	// --- события -------------------------------------------------------------------------------
	override void Tick(float dt)
	{
		if (m_Left)
			PollDrag();
		if (m_Dragging)
			return;
		Widget focus = GetFocus();
		Widget edit = null;
		if (m_Name)
			edit = m_Name.Edit;
		if (edit && focus == edit)
			return;
		if (m_NameDirty)
			FlushName();
		if (m_Stale)
		{
			m_Stale = false;
			m_Menu.RequestRepaint();
		}
	}

	override void Leave()
	{
		FlushName();
	}

	override bool OnData()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (!client.Clan)
		{
			m_Menu.GoLater("profile");
			return true;
		}
		// Пока игрок вводит название или тащит ранг, экран не трогаем: перестроится после.
		Widget focus = GetFocus();
		if (m_Dragging || (m_Name && focus && focus == m_Name.Edit))
		{
			m_Stale = true;
			return true;
		}
		return false;
	}

	override void OnResult(int code)
	{
		// Сервер отверг правку: вернуть настоящие ранги.
		if (!VBK.ResultIsGood(code))
			VB_TabletClient.Get().Request(VB_TabletOp.CLAN);
	}

	override void OnInput(VBT_Input field)
	{
		if (field == m_Name)
			m_NameDirty = true;
	}

	override void OnSubmit(VBT_Input field)
	{
		if (field != m_Name)
			return;
		FlushName();
		SetFocus(null);
	}

	override void OnAnswer(int tag, bool yes, string text)
	{
		if (tag != TAG_DELETE || !yes)
			return;
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (!c)
			return;
		int at = c.Ranks.Count();
		for (int i = 0; i < c.Ranks.Count(); i++)
		{
			if (c.Ranks.Get(i).Id == s_DelId)
				at = i;
		}
		if (at >= c.Ranks.Count())
			return;
		c.Ranks.RemoveOrdered(at);
		s_Sel = c.Ranks.Get(0).Id;
		Send();
		Repaint();
	}

	override bool OnHit(Widget w)
	{
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (!c)
			return false;
		int row = m_RowHits.Find(w);
		if (row >= 0)
		{
			// Отпускание мыши после перетаскивания это не выбор ранга.
			if (m_Dragged)
			{
				m_Dragged = false;
				return true;
			}
			FlushName();
			s_Sel = c.Ranks.Get(row).Id;
			Repaint();
			return true;
		}
		VB_ClanRank k = c.Rank(s_Sel);
		int i = m_ColorHits.Find(w);
		if (i >= 0)
		{
			FlushName();
			array<int> colors = VBK.Colors();
			k.Color = colors.Get(i);
			Send();
			Repaint();
			return true;
		}
		foreach (int idx, VBT_Switch sw : m_Switches)
		{
			if (w != sw.Hit)
				continue;
			FlushName();
			sw.Toggle();
			int bit = m_SwitchBits.Get(idx);
			if (sw.On)
				k.Perms = k.Perms | bit;
			else
				k.Perms = k.Perms & (VB_ClanPerm.ALL - bit);
			Send();
			Repaint();
			return true;
		}
		if (m_Add && w == m_Add.Hit)
		{
			FlushName();
			AddRank();
			return true;
		}
		if (m_Del && w == m_Del.Hit)
		{
			FlushName();
			s_DelId = k.Id;
			int last = c.Ranks.Count() - 1;
			string a = VBT.T(k.Title);
			string b = VBT.T(c.Ranks.Get(last).Title);
			m_Menu.Ask(TAG_DELETE, "#STR_VB_TAB_B_RANK_DEL_Q", VBB.Sub("#STR_VB_TAB_B_RANK_DEL_T", a, b), "#STR_VB_TAB_B_DELETE", true);
			return true;
		}
		return false;
	}

	override bool OnHover(Widget w, bool enter)
	{
		int row = m_RowHits.Find(w);
		if (row >= 0)
		{
			if (enter)
				m_HoverRow = row;
			else if (m_HoverRow == row)
				m_HoverRow = -1;
			VB_Clan c = VB_TabletClient.Get().Clan;
			if (c && !m_Dragging && c.Ranks.Get(row).Id != s_Sel)
			{
				int fill = VB_C_PANEL2;
				if (enter)
					fill = VB_C_PANEL3;
				m_RowBg.Get(row).SetFill(fill);
			}
			return true;
		}
		if (m_Add && w == m_Add.Hit)
		{
			m_Add.Hover(enter);
			return true;
		}
		if (m_Del && w == m_Del.Hit)
		{
			m_Del.Hover(enter);
			return true;
		}
		return false;
	}
}
