// Участники: слева состав, справа карточка выбранного. Выбран всегда кто-то, по умолчанию сам игрок.
// Экран обновляется на месте, без полной перестройки: так не сбрасываются прокрутка, выбор и открытые окна.
class VB_TabletPageMembers extends VB_TabletPage
{
	protected static const int TAG_KICK = 1;
	protected static const int POP_NONE = 0;
	protected static const int POP_REQUESTS = 1;
	protected static const int POP_INVITE = 2;
	protected static const int POP_DISCORD = 3;

	// Сортировка остаётся между заходами на экран: 0 игрок, 1 ранг, 2 сегодня, 3 всего, 4 в сети.
	protected static int s_Sort = 1;

	protected string m_SelUid;
	protected string m_TargetUid;
	protected string m_AcceptUid;
	protected string m_AcceptName;
	protected bool m_Need;
	protected bool m_NeedNear;
	protected Widget m_TopF;
	protected Widget m_Head;
	protected ScrollWidget m_Scroll;
	protected Widget m_Rows;
	protected Widget m_CardF;
	protected ref array<Widget> m_HeadHits;
	protected ref array<TextWidget> m_HeadTexts;
	protected ref array<Widget> m_RowHits;
	protected ref array<ref INT_UIF_Surface> m_RowBgs;
	protected ref array<string> m_RowUids;

	protected ref VBT_Button m_BtnInvite;
	protected ref VBT_Button m_BtnRequests;
	protected ref VBT_Button m_BtnKick;
	protected ref VBT_Button m_BtnExit;
	protected ref VBA_IconBtn m_BtnDiscord;
	protected ref VBT_Dropdown m_RankDrop;
	protected ref array<string> m_RankIds;
	protected ref array<string> m_RankNames;

	protected ref VBT_Modal m_Modal;
	protected int m_Pop;
	protected ref array<ref VBT_Button> m_ReqYes;
	protected ref array<ref VBT_Button> m_ReqNo;
	protected ref array<ref VBA_IconBtn> m_ReqCopy;
	protected ref array<string> m_ReqUids;
	protected ref array<string> m_ReqNames;
	protected ref array<string> m_ReqDiscords;
	protected ref VBT_Input m_InvField;
	protected ref VBT_Button m_InvCall;
	protected Widget m_NearF;
	protected ref array<string> m_NearNames;
	protected ref array<int> m_NearDist;
	protected ref array<ref VBT_Button> m_NearBtns;
	protected ref array<string> m_Sent;
	protected ref VBT_Input m_DcField;
	protected TextWidget m_DcNote;
	protected ref VBT_Button m_DcSave;
	protected ref VBT_Button m_DcCancel;
	protected ref VBT_Button m_DcDelete;

	override void Build()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c)
		{
			m_Menu.GoLater("create");
			return;
		}
		// Опыт соклановцев пока берётся только из присланной части рейтинга.
		VBA_Util.Ask(VB_TabletOp.RATING);
		VBA_Util.Ask(VB_TabletOp.PROFILE);
		m_SelUid = client.MyUid();
		if (client.PickMember != "")
		{
			m_SelUid = client.PickMember;
			client.PickMember = "";
		}
		m_HeadHits = new array<Widget>();
		m_HeadTexts = new array<TextWidget>();
		m_RowHits = new array<Widget>();
		m_RowBgs = new array<ref INT_UIF_Surface>();
		m_RowUids = new array<string>();
		m_RankIds = new array<string>();
		m_RankNames = new array<string>();
		m_ReqYes = new array<ref VBT_Button>();
		m_ReqNo = new array<ref VBT_Button>();
		m_ReqCopy = new array<ref VBA_IconBtn>();
		m_ReqUids = new array<string>();
		m_ReqNames = new array<string>();
		m_ReqDiscords = new array<string>();
		m_NearNames = new array<string>();
		m_NearDist = new array<int>();
		m_NearBtns = new array<ref VBT_Button>();
		m_Sent = new array<string>();

		Widget t = VBT.Card(m_Root, 0, 0, 664, 720);
		m_Head = INT_UIF_Draw.Frame(t, 0, 0, 664, 46);
		INT_UIF_Draw.Line(t, 0, 46, 664, 1, VB_C_LINE);
		m_Scroll = VBK.Scroll(t, 0, 47, 664, 672);
		m_Rows = INT_UIF_Draw.ScrollContent(m_Scroll);
		m_CardF = INT_UIF_Draw.Frame(m_Root, 680, 0, 492, 720);
		m_TopF = INT_UIF_Draw.Frame(m_Menu.GetTopBar(), 680, 0, 310, 72);
		Paint();
	}

	protected void Paint()
	{
		PaintTop();
		PaintRows();
		PaintCard();
	}

	protected void PaintTop()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		INT_UIF_Widgets.ClearChildren(m_TopF);
		m_BtnInvite = null;
		m_BtnRequests = null;
		if (!client.Can(VB_ClanPerm.INVITE))
			return;
		int waiting = client.Clan.Requests.Count();
		string reqText = VBT.T("#STR_VB_TAB_A_REQUESTS") + " · " + waiting.ToString();
		m_BtnRequests = new VBT_Button(m_TopF, 0, 16, 150, 40, reqText, false, "mail", m_Menu.GetHitHandler(), 14);
		m_BtnInvite = new VBT_Button(m_TopF, 160, 16, 150, 40, "#STR_VB_TAB_A_INVITE", true, "invite", m_Menu.GetHitHandler(), 14);
	}

	// Порядок строк: a раньше b при текущей сортировке.
	protected bool Before(VB_Clan c, VB_ClanMember a, VB_ClanMember b)
	{
		switch (s_Sort)
		{
			case 1:
				int ra = c.RankIndex(a.RankId);
				int rb = c.RankIndex(b.RankId);
				if (ra != rb)
					return ra < rb;
				return a.Total > b.Total;
			case 2:
				return a.Today > b.Today;
			case 3:
				return a.Total > b.Total;
			case 4:
				return a.Online && !b.Online;
		}
		return false;
	}

	protected array<VB_ClanMember> SortedMembers(VB_Clan c)
	{
		array<VB_ClanMember> list = new array<VB_ClanMember>();
		if (s_Sort == 0)
		{
			// По имени: движок сортирует строки сам, uid после разделителя возвращает игрока.
			array<string> keys = new array<string>();
			foreach (VB_ClanMember km : c.Members)
			{
				string low = km.PlayerName + "";
				low.ToLower();
				keys.Insert(low + "|" + km.Uid);
			}
			keys.Sort();
			foreach (string key : keys)
			{
				int cut = key.LastIndexOf("|");
				string uid = key.Substring(cut + 1, key.Length() - cut - 1);
				list.Insert(c.Member(uid));
			}
			return list;
		}
		foreach (VB_ClanMember m : c.Members)
		{
			int at = list.Count();
			for (int i = 0; i < list.Count(); i++)
			{
				if (Before(c, m, list.Get(i)))
				{
					at = i;
					break;
				}
			}
			list.InsertAt(m, at);
		}
		return list;
	}

	protected void PaintHead()
	{
		INT_UIF_Widgets.ClearChildren(m_Head);
		m_HeadHits.Clear();
		m_HeadTexts.Clear();
		array<string> names = {"#STR_VB_TAB_PLAYER", "#STR_VB_TAB_RANK_C", "#STR_VB_TAB_A_TODAY_C", "#STR_VB_TAB_A_TOTAL_C", "#STR_VB_TAB_A_ONLINE_C"};
		array<float> xs = {42, 212, 340, 430, 520};
		array<float> ws = {150, 110, 80, 80, 124};
		for (int i = 0; i < 5; i++)
		{
			int col = VB_C_DIM;
			if (i == s_Sort)
				col = VB_C_ACCENT;
			m_HeadTexts.Insert(VBT.Cap(m_Head, xs.Get(i), 18, ws.Get(i), names.Get(i), col));
			m_HeadHits.Insert(HitAt(m_Head, xs.Get(i) - 6, 8, ws.Get(i), 32));
		}
	}

	protected void PaintRows()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		string myUid = client.MyUid();
		PaintHead();
		INT_UIF_Widgets.ClearChildren(m_Rows);
		m_RowHits.Clear();
		m_RowBgs.Clear();
		m_RowUids.Clear();
		array<VB_ClanMember> list = SortedMembers(c);
		int n = list.Count();
		INT_UIF_Draw.SetScrollContent(m_Scroll, n * 48 + 6);
		for (int i = 0; i < n; i++)
		{
			VB_ClanMember m = list.Get(i);
			float y = i * 48;
			bool sel = m.Uid == m_SelUid;
			bool isMe = m.Uid == myUid;
			int fill = 0;
			int border = 0;
			if (isMe)
				fill = VB_C_PANEL2;
			if (sel)
			{
				fill = VB_C_PANEL3;
				border = VB_C_ACCENT;
			}
			m_RowBgs.Insert(INT_UIF_Surface.Box(m_Rows, 8, y + 3, 640, 42, 6, fill, border));
			if (isMe)
				INT_UIF_Surface.Box(m_Rows, 8, y + 11, 3, 26, 2, VB_C_ACCENT);
			int dot = VB_C_DIM2;
			if (m.Online)
				dot = VB_C_OK;
			INT_UIF_Surface.Pill(m_Rows, 24, y + 20, 8, 8, dot);
			string name = VBT.Fit(m.PlayerName, 160, 17);
			INT_UIF_Draw.Text(m_Rows, 42, y, 160, 48, name, 17, VB_C_TEXT);
			VB_ClanRank ra = c.Rank(m.RankId);
			string rankTitle = VBA_Util.Title(ra.Title);
			rankTitle = VBT.Fit(rankTitle, 92, 13);
			VBK.Pill(m_Rows, 212, y + 12, rankTitle, ra.Color);
			string today = "0";
			int todayCol = VB_C_DIM2;
			if (m.Today > 0)
			{
				today = "+" + m.Today.ToString();
				todayCol = VB_C_INF;
			}
			INT_UIF_Draw.Text(m_Rows, 340, y, 80, 48, today, 15, todayCol);
			INT_UIF_Draw.Text(m_Rows, 430, y, 80, 48, VBT.Num(m.Total), 15, VB_C_DIM);
			INT_UIF_Draw.Text(m_Rows, 520, y, 124, 48, LastText(m), 14, VB_C_DIM2);
			m_RowHits.Insert(HitAt(m_Rows, 8, y + 3, 640, 42));
			m_RowUids.Insert(m.Uid);
		}
	}

	protected string LastText(VB_ClanMember m)
	{
		if (m.Online)
			return VBT.T("#STR_VB_TAB_A_NOW");
		if (m.LastSeen != "")
			return m.LastSeen;
		return VBA_DASH;
	}

	protected void PaintCard()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		string myUid = client.MyUid();
		VB_ClanMember m = c.Member(m_SelUid);
		if (!m)
		{
			m_SelUid = myUid;
			m = c.Member(m_SelUid);
		}
		INT_UIF_Widgets.ClearChildren(m_CardF);
		m_RankDrop = null;
		m_BtnKick = null;
		m_BtnExit = null;
		m_BtnDiscord = null;
		if (!m)
			return;
		bool isMe = m.Uid == myUid;
		bool leader = c.IsLeader(myUid);
		bool canRank = client.Can(VB_ClanPerm.ASSIGN);
		bool canKick = client.Can(VB_ClanPerm.KICK);
		bool act = !isMe && c.Outranks(myUid, m.Uid) && (canRank || canKick);

		Widget f = INT_UIF_Draw.Frame(m_CardF, 0, 0, 492, 720);
		INT_UIF_Surface.Box(f, 0, 0, 492, 720, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(f, 492, 720);
		VBT.Corners(f, 492, 720, VB_C_ACCENT, 14);

		string shown = VBT.Fit(m.PlayerName, 440, 30);
		INT_UIF_Draw.Text(f, 24, 16, 440, 40, shown, 30, VB_C_TEXT, INT_UIF_Align.LEFT, true);
		int dot = VB_C_DIM2;
		string state = LastText(m);
		if (m.Online)
		{
			dot = VB_C_OK;
			state = VBT.T("#STR_VB_TAB_A_ONLINE");
		}
		INT_UIF_Surface.Pill(f, 24, 66, 10, 10, dot);
		INT_UIF_Draw.Text(f, 42, 58, 200, 26, state, 14, VB_C_DIM);
		VB_ClanRank cr = c.Rank(m.RankId);
		string rankTitle = VBA_Util.Title(cr.Title);
		rankTitle = VBT.Fit(rankTitle, 160, 13);
		VBK.Pill(f, 492 - 24 - INT_UIF_Draw.TextWidth(rankTitle, 13) - 22, 58, rankTitle, cr.Color);
		INT_UIF_Draw.Line(f, 24, 94, 444, 1, VB_C_LINE);

		PaintDiscord(f, m, isMe, client);
		PaintStats(f, m, isMe, client);
		PaintContribution(f, m, isMe, client);

		// Сколько места занимают действия снизу: столько сегментов остаётся графику недели.
		int segs = 18;
		if (isMe || act)
			segs = 10;
		PaintWeek(f, m, segs);
		if (isMe)
		{
			INT_UIF_Draw.Line(f, 24, 634, 444, 1, VB_C_LINE);
			string exitText = "#STR_VB_TAB_A_LEAVE";
			if (leader)
				exitText = "#STR_VB_TAB_A_DISSOLVE";
			m_BtnExit = new VBT_Button(f, 24, 650, 444, 44, exitText, false, "logout", m_Menu.GetHitHandler(), 14);
			m_BtnExit.Danger = true;
			m_BtnExit.Bg.SetFill(VB_C_BAD);
			return;
		}
		if (act)
		{
			INT_UIF_Draw.Line(f, 24, 634, 444, 1, VB_C_LINE);
			PaintActions(f, c, m, 650, canRank, canKick);
		}
	}

	protected void PaintDiscord(Widget f, VB_ClanMember m, bool isMe, VB_TabletClient client)
	{
		string dc = m.Discord;
		if (isMe)
			dc = client.Me.Discord;
		bool has = dc != "";
		INT_UIF_Surface.Box(f, 24, 104, 444, 44, 8, VB_C_PANEL2);
		int ico = VB_C_DIM2;
		int ink = VB_C_DIM2;
		string text = "#STR_VB_TAB_NOT_SET";
		if (has)
		{
			ico = VB_C_ACCENT;
			ink = VB_C_TEXT;
			text = VBT.Fit("@" + dc, 270, 16);
		}
		VBT.Ico(f, 36, 115, 22, "discord", ico);
		VBT.Cap(f, 68, 117, 80, "#STR_VB_TAB_DISCORD");
		INT_UIF_Draw.Text(f, 146, 104, 270, 44, text, 16, ink);
		if (isMe)
		{
			string name = "plus";
			if (has)
				name = "edit";
			m_BtnDiscord = new VBA_IconBtn(f, 424, 110, 36, 32, name, m_Menu.GetHitHandler());
		}
		else if (has)
		{
			m_BtnDiscord = new VBA_IconBtn(f, 424, 110, 36, 32, "copy", m_Menu.GetHitHandler());
		}
	}

	// Показатели соклановца приходят отдельным запросом, пока их нет, в карточке прочерки. //ает. Опыт чужого берётся из рейтинга, если он там есть.
	protected void PaintStats(Widget f, VB_ClanMember m, bool isMe, VB_TabletClient client)
	{
		VB_TabletRates rates = client.Rates;
		VB_TabletStats me = client.Me;
		VB_TabletStats st = me;
		if (!isMe)
		{
			client.RequestCard(m.Uid, m.PlayerName);
			st = client.CardOf(m.PlayerName);
		}
		int xp = -1;
		if (st)
			xp = st.Xp(rates);
		if (xp >= 0)
		{
			int ri = rates.RankOf(xp);
			VBT.Insignia(f, 24, 158, 56, ri);
			INT_UIF_Draw.Text(f, 94, 156, 374, 30, VBT.RankName(ri), 22, VB_C_TEXT);
			string line;
			float k = 1;
			string sx = VBT.Num(xp);
			if (ri + 1 < VB_RANK_COUNT)
			{
				int cur = rates.RankNeed.Get(ri);
				int nx = rates.RankNeed.Get(ri + 1);
				string sn = VBT.Num(nx);
				line = sx + "/" + sn + " " + VBT.T("#STR_VB_TAB_XP_L");
				k = (xp - cur) / (nx - cur + 0.0);
			}
			else
			{
				line = sx + " " + VBT.T("#STR_VB_TAB_XP_L");
			}
			INT_UIF_Draw.Text(f, 94, 186, 374, 22, line, 14, VB_C_DIM);
			VBT.Ticks(f, 24, 222, 444, 10, 44, k);
		}
		else
		{
			INT_UIF_Draw.Text(f, 24, 156, 444, 30, VBA_DASH, 22, VB_C_DIM2);
			INT_UIF_Draw.Text(f, 24, 186, 444, 22, "#STR_VB_TAB_A_NO_STATS", 14, VB_C_DIM2);
			VBT.Ticks(f, 24, 222, 444, 10, 44, 0);
		}
		array<string> caps = {"#STR_VB_TAB_ZOMBIES", "#STR_VB_TAB_MUTANTS", "#STR_VB_TAB_ANIMALS", "#STR_VB_TAB_DIST", "#STR_VB_TAB_A_THIS_LIFE", "#STR_VB_TAB_RECORD"};
		array<string> vals = new array<string>();
		if (st)
		{
			int life = st.LifeSec;
			if (isMe)
				life = client.LifeNow();
			vals.Insert(VBT.Num(st.Zombies));
			vals.Insert(VBT.Num(st.Mutants));
			vals.Insert(VBT.Num(st.Animals));
			vals.Insert(VBT.Km(st.DistM));
			vals.Insert(VBT.LifeText(life));
			vals.Insert(VBT.LifeText(st.RecordSec));
		}
		for (int i = 0; i < 6; i++)
		{
			int col2 = i % 3;
			int row2 = i / 3;
			float x = 24 + col2 * 151;
			float y = 246 + row2 * 70;
			INT_UIF_Surface.Box(f, x, y, 142, 62, 8, VB_C_PANEL2);
			INT_UIF_Draw.Text(f, x + 12, y + 7, 124, 18, caps.Get(i), 12, VB_C_DIM);
			string val = VBA_DASH;
			int ink = VB_C_DIM2;
			if (st)
			{
				val = vals.Get(i);
				ink = VB_C_TEXT;
				if (i == 5)
					ink = VB_C_GOLD;
			}
			INT_UIF_Draw.Text(f, x + 12, y + 28, 124, 28, val, 17, ink);
		}
		INT_UIF_Draw.Line(f, 24, 392, 444, 1, VB_C_LINE);
	}

	protected void PaintContribution(Widget f, VB_ClanMember m, bool isMe, VB_TabletClient client)
	{
		string today = "0";
		int todayCol = VB_C_DIM2;
		if (m.Today > 0)
		{
			today = "+" + m.Today.ToString();
			todayCol = VB_C_INF;
		}
		string done = VBA_DASH;
		int doneCol = VB_C_DIM2;
		VB_TabletStats st = client.Me;
		if (!isMe)
			st = client.CardOf(m.PlayerName);
		if (st)
		{
			done = VBT.Num(st.Contracts);
			doneCol = VB_C_TEXT;
		}
		VBT.Cap(f, 24, 404, 140, "#STR_VB_TAB_A_CONTRIB_C");
		INT_UIF_Draw.Text(f, 24, 424, 140, 32, VBT.Num(m.Total), 22, VB_C_INF);
		VBT.Cap(f, 175, 404, 140, "#STR_VB_TAB_A_TODAY_C");
		INT_UIF_Draw.Text(f, 175, 424, 140, 32, today, 22, todayCol);
		VBT.Cap(f, 326, 404, 140, "#STR_VB_TAB_A_CONTRACTS_C");
		INT_UIF_Draw.Text(f, 326, 424, 140, 32, done, 22, doneCol);
		INT_UIF_Draw.Line(f, 24, 466, 444, 1, VB_C_LINE);
	}

	// Вклад по дням недели, столбики в масштабе лучшего дня. // остальные дни пустые, итога нет.
	protected void PaintWeek(Widget f, VB_ClanMember m, int segs)
	{
		float axis = 500 + segs * 9;
		int today = VBA_Util.Today();
		VBT.Cap(f, 24, 478, 240, "#STR_VB_TAB_A_WEEK_CONTRIB");
		int best = 1;
		for (int d = 0; d < 7; d++)
			best = Math.Max(best, m.Week.Get(d));
		for (int i = 0; i < 7; i++)
		{
			float x = 24 + i * 65;
			int lit = 0;
			int dayVal = m.Week.Get(i);
			if (dayVal > 0)
				lit = Math.Max(1, Math.Round(segs * dayVal / (best + 0.0)));
			int on = VB_C_INF;
			int lc = VB_C_DIM2;
			if (i == today)
			{
				on = VB_C_ACCENT;
				lc = VB_C_ACCENT;
			}
			for (int k = 0; k < segs; k++)
			{
				int col = VB_C_PANEL3;
				if (k < lit)
					col = on;
				INT_UIF_Draw.Line(f, x, axis - k * 9, 54, 6, col);
			}
			INT_UIF_Draw.Text(f, x, axis + 10, 54, 18, "#STR_VB_TAB_A_WD_" + i.ToString(), 13, lc, INT_UIF_Align.CENTER);
		}
	}

	protected void PaintActions(Widget f, VB_Clan c, VB_ClanMember m, float y, bool canRank, bool canKick)
	{
		bool both = canRank && canKick;
		if (canRank)
		{
			VB_ClanMember mine = c.Member(VB_TabletClient.Get().MyUid());
			int from = c.RankIndex(mine.RankId) + 1;
			m_RankIds.Clear();
			m_RankNames.Clear();
			int sel = 0;
			for (int i = from; i < c.Ranks.Count(); i++)
			{
				VB_ClanRank r = c.Ranks.Get(i);
				if (r.Id == m.RankId)
					sel = m_RankIds.Count();
				m_RankIds.Insert(r.Id);
				m_RankNames.Insert(VBA_Util.Title(r.Title));
			}
			if (m_RankIds.Count() > 0)
			{
				float dw = 444;
				if (both)
					dw = 250;
				m_RankDrop = new VBT_Dropdown(f, 24, y, dw, 44, m_RankNames, sel, true);
			}
		}
		if (canKick)
		{
			float kx = 24;
			float kw = 444;
			if (both)
			{
				kx = 286;
				kw = 182;
			}
			m_BtnKick = new VBT_Button(f, kx, y, kw, 44, "#STR_VB_TAB_A_KICK", false, "kick", m_Menu.GetHitHandler(), 14);
			m_BtnKick.Danger = true;
			m_BtnKick.Bg.SetFill(VB_C_BAD);
		}
	}

	// --- окна ---------------------------------------------------------------------------------
	protected void ClosePop()
	{
		if (m_Modal)
			m_Modal.Close();
		m_Modal = null;
		m_Pop = POP_NONE;
		SetFocus(null);
		m_Menu.SetTyping(false);
	}

	protected void OpenRequests()
	{
		ClosePop();
		VB_Clan c = VB_TabletClient.Get().Clan;
		VB_TabletRates rates = VB_TabletClient.Get().Rates;
		int n = c.Requests.Count();
		int rows = Math.Min(3, n);
		float w = 680;
		float h = 150;
		if (n > 0)
			h = 84 + rows * 172;
		m_Modal = new VBT_Modal(m_Menu, w, h);
		m_Pop = POP_REQUESTS;
		Widget b = m_Modal.Body;
		INT_UIF_Draw.Text(b, 28, 22, 400, 32, "#STR_VB_TAB_A_REQ_TITLE", 22, VB_C_TEXT);
		int cap = VB_TabletClient.Get().ClanMax;
		int have = c.Members.Count();
		string fill = have.ToString() + "/" + cap.ToString();
		INT_UIF_Draw.Text(b, w - 132, 22, 70, 32, fill, 15, VB_C_DIM, INT_UIF_Align.RIGHT);
		m_ReqYes.Clear();
		m_ReqNo.Clear();
		m_ReqCopy.Clear();
		m_ReqUids.Clear();
		m_ReqNames.Clear();
		m_ReqDiscords.Clear();
		if (n == 0)
		{
			INT_UIF_Draw.Text(b, 28, 76, w - 56, 32, "#STR_VB_TAB_A_REQ_NONE", 16, VB_C_DIM2);
			return;
		}
		ScrollWidget sw = VBK.Scroll(b, 0, 72, w, rows * 172);
		Widget L = INT_UIF_Draw.ScrollContent(sw);
		INT_UIF_Draw.SetScrollContent(sw, n * 172);
		for (int i = 0; i < n; i++)
		{
			VB_ClanRequest q = c.Requests.Get(i);
			float y = i * 172;
			int ri = rates.RankOf(q.Xp);
			INT_UIF_Surface.Box(L, 28, y, 624, 160, 8, VB_C_PANEL2);
			VBT.Insignia(L, 42, y + 12, 36, ri);
			string name = VBT.Fit(q.PlayerName, 240, 17);
			INT_UIF_Draw.Text(L, 92, y + 8, 240, 26, name, 17, VB_C_TEXT);
			string line = VBT.T(VBT.RankName(ri)) + " · " + q.Time;
			INT_UIF_Draw.Text(L, 92, y + 34, 240, 20, line, 13, VB_C_DIM);
			bool has = q.Discord != "";
			string dc = "#STR_VB_TAB_A_DC_NONE";
			int ink = VB_C_DIM2;
			if (has)
			{
				dc = VBT.Fit("@" + q.Discord, 256, 14);
				ink = VB_C_TEXT;
			}
			INT_UIF_Draw.Text(L, 340, y + 10, 256, 30, dc, 14, ink, INT_UIF_Align.RIGHT);
			VBA_IconBtn copy = null;
			if (has)
				copy = new VBA_IconBtn(L, 604, y + 10, 36, 30, "copy", m_Menu.GetHitHandler());
			m_ReqCopy.Insert(copy);
			INT_UIF_Draw.MText(L, 42, y + 58, 596, 64, q.Msg, 15, VB_C_DIM);
			m_ReqYes.Insert(new VBT_Button(L, 42, y + 120, 140, 32, "#STR_VB_TAB_A_ACCEPT", true, "check", m_Menu.GetHitHandler(), 13));
			m_ReqNo.Insert(new VBT_Button(L, 192, y + 120, 140, 32, "#STR_VB_TAB_A_REJECT", false, "close", m_Menu.GetHitHandler(), 13));
			m_ReqUids.Insert(q.Uid);
			m_ReqNames.Insert(q.PlayerName);
			m_ReqDiscords.Insert(q.Discord);
		}
	}

	protected void SendRequest(string uid, bool accept)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		string target = uid;
		bool flag = accept;
		ScriptRPC rpc = client.Begin(VB_TabletOp.CLAN_REQUEST);
		rpc.Write(target);
		rpc.Write(flag);
		client.Post(rpc);
	}

	protected bool IsMemberName(VB_Clan c, string name)
	{
		string want = name + "";
		want.ToLower();
		foreach (VB_ClanMember m : c.Members)
		{
			string have = m.PlayerName + "";
			have.ToLower();
			if (have == want)
				return true;
		}
		return false;
	}

	// Игроки рядом: те, кого видит клиент, кроме соклановцев, не больше пяти, ближние первыми.
	protected void FindNear(VB_Clan c)
	{
		m_NearNames.Clear();
		m_NearDist.Clear();
		PlayerBase self = PlayerBase.Cast(GetGame().GetPlayer());
		if (!self)
			return;
		vector at = self.GetPosition();
		array<Man> all = ClientData.m_PlayerBaseList;
		foreach (Man man : all)
		{
			PlayerBase p = PlayerBase.Cast(man);
			if (!p || !p.IsAlive() || !p.GetIdentity())
				continue;
			int dist = vector.Distance(at, p.GetPosition());
			if (dist > VBA_NEAR_M)
				continue;
			string name = p.GetIdentity().GetName();
			if (IsMemberName(c, name))
				continue;
			int slot = m_NearDist.Count();
			for (int i = 0; i < m_NearDist.Count(); i++)
			{
				if (dist < m_NearDist.Get(i))
				{
					slot = i;
					break;
				}
			}
			m_NearDist.InsertAt(dist, slot);
			m_NearNames.InsertAt(name, slot);
		}
		while (m_NearNames.Count() > 5)
		{
			m_NearNames.Remove(5);
			m_NearDist.Remove(5);
		}
	}

	protected void OpenInvite()
	{
		ClosePop();
		VB_Clan c = VB_TabletClient.Get().Clan;
		bool full = c.Members.Count() >= VB_TabletClient.Get().ClanMax;
		float w = 520;
		float h = 136;
		if (!full)
		{
			FindNear(c);
			h = 168 + Math.Max(1, m_NearNames.Count()) * 48 + 16;
		}
		m_Modal = new VBT_Modal(m_Menu, w, h);
		m_Pop = POP_INVITE;
		Widget b = m_Modal.Body;
		INT_UIF_Draw.Text(b, 28, 22, 300, 32, "#STR_VB_TAB_A_INV_TITLE", 22, VB_C_TEXT);
		int cap = VB_TabletClient.Get().ClanMax;
		int have = c.Members.Count();
		string fill = have.ToString() + "/" + cap.ToString();
		INT_UIF_Draw.Text(b, w - 132, 22, 70, 32, fill, 15, VB_C_DIM, INT_UIF_Align.RIGHT);
		m_InvField = null;
		m_InvCall = null;
		m_NearF = null;
		m_NearBtns.Clear();
		if (full)
		{
			INT_UIF_Draw.Text(b, 28, 72, w - 56, 32, "#STR_VB_TAB_A_INV_FULL", 16, VB_C_DIM);
			return;
		}
		m_InvField = new VBT_Input(b, 28, 72, 326, 44, "#STR_VB_TAB_A_INV_NICK", 24);
		m_InvCall = new VBT_Button(b, 364, 72, 128, 44, "#STR_VB_TAB_A_INV_CALL", true, "", m_Menu.GetHitHandler());
		INT_UIF_Draw.Line(b, 28, 132, 464, 1, VB_C_LINE);
		VBT.Cap(b, 28, 146, 300, "#STR_VB_TAB_A_INV_NEAR");
		m_NearF = INT_UIF_Draw.Frame(b, 0, 168, w, h - 168);
		PaintNear();
		m_InvField.Focus();
	}

	protected void PaintNear()
	{
		if (!m_NearF)
			return;
		INT_UIF_Widgets.ClearChildren(m_NearF);
		m_NearBtns.Clear();
		int n = m_NearNames.Count();
		if (n == 0)
			INT_UIF_Draw.Text(m_NearF, 28, 0, 464, 44, "#STR_VB_TAB_A_INV_NOBODY", 15, VB_C_DIM2);
		for (int k = 0; k < n; k++)
		{
			float y = k * 48;
			string nick = m_NearNames.Get(k);
			string shown = VBT.Fit(nick, 220, 16);
			INT_UIF_Draw.Text(m_NearF, 28, y, 220, 44, shown, 16, VB_C_TEXT);
			int meters = m_NearDist.Get(k);
			string dist = meters.ToString() + " " + VBT.T("#STR_VB_TAB_A_M");
			INT_UIF_Draw.Text(m_NearF, 260, y, 90, 44, dist, 14, VB_C_DIM);
			bool sent = m_Sent.Find(nick) >= 0;
			string lbl = "#STR_VB_TAB_A_INV_CALL";
			if (sent)
				lbl = "#STR_VB_TAB_A_INV_SENT";
			VBT_Button btn = new VBT_Button(m_NearF, 364, y + 4, 128, 36, lbl, false, "", m_Menu.GetHitHandler(), 14);
			if (sent)
				btn.Hit.Show(false);
			m_NearBtns.Insert(btn);
		}
	}

	protected void SendInvite(string nick)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		string target = nick;
		ScriptRPC rpc = client.Begin(VB_TabletOp.CLAN_INVITE);
		rpc.Write(target);
		client.Post(rpc);
		if (m_Sent.Find(nick) < 0)
			m_Sent.Insert(nick);
	}

	protected void CallByField()
	{
		string nick = m_InvField.Get();
		if (nick == "")
			return;
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (IsMemberName(c, nick))
		{
			INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_A_INV_ALREADY"));
			return;
		}
		SendInvite(nick);
		m_InvField.Set("");
		m_NeedNear = true;
	}

	protected void OpenDiscord()
	{
		ClosePop();
		string cur = VB_TabletClient.Get().Me.Discord;
		float w = 520;
		float h = 240;
		m_Modal = new VBT_Modal(m_Menu, w, h);
		m_Pop = POP_DISCORD;
		Widget b = m_Modal.Body;
		INT_UIF_Draw.Text(b, 28, 22, 400, 32, "#STR_VB_TAB_A_DC_TITLE", 22, VB_C_TEXT);
		m_DcField = new VBT_Input(b, 28, 70, w - 56, 48, "#STR_VB_TAB_A_DC_HINT", 32, cur);
		m_DcNote = INT_UIF_Draw.Text(b, 28, 124, w - 56, 24, "#STR_VB_TAB_A_DC_NOTE", 14, VB_C_DIM2);
		m_DcDelete = null;
		if (cur != "")
		{
			m_DcDelete = new VBT_Button(b, 28, h - 68, 120, 44, "#STR_VB_TAB_A_DELETE", false, "", m_Menu.GetHitHandler());
			m_DcDelete.Danger = true;
			m_DcDelete.Bg.SetFill(VB_C_BAD);
		}
		m_DcCancel = new VBT_Button(b, w - 328, h - 68, 140, 44, "#STR_VB_TAB_CANCEL", false, "", m_Menu.GetHitHandler());
		m_DcSave = new VBT_Button(b, w - 178, h - 68, 150, 44, "#STR_VB_TAB_A_SAVE", true, "", m_Menu.GetHitHandler());
		m_DcField.Focus();
	}

	protected void SendDiscord(string name)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		string value = name;
		ScriptRPC rpc = client.Begin(VB_TabletOp.DISCORD_SET);
		rpc.Write(value);
		client.Post(rpc);
		ClosePop();
		if (value == "")
			INT_UIF_Toast.Success(VBT.T("#STR_VB_TAB_A_DC_REMOVED"));
		else
			INT_UIF_Toast.Success(VBT.T("#STR_VB_TAB_A_DC_SAVED"));
	}

	protected void SaveDiscord()
	{
		string cur = VB_TabletClient.Get().Me.Discord;
		string v = VBA_Util.StripAt(m_DcField.Get());
		if (v == "")
		{
			if (cur != "")
				SendDiscord("");
			else
				ClosePop();
			return;
		}
		string err = VBA_Util.DiscordError(v);
		if (err != "")
		{
			m_DcNote.SetText(err);
			m_DcNote.SetColor(VB_C_BAD);
			return;
		}
		if (v == cur)
		{
			ClosePop();
			return;
		}
		SendDiscord(v);
	}

	// --- события ------------------------------------------------------------------------------
	override void OnInput(VBT_Input field)
	{
		if (field != m_DcField || m_Pop != POP_DISCORD)
			return;
		string raw = field.Edit.GetText();
		string low = raw + "";
		low.ToLower();
		if (low != raw)
			field.Set(low);
		m_DcNote.SetText("#STR_VB_TAB_A_DC_NOTE");
		m_DcNote.SetColor(VB_C_DIM2);
	}

	override void OnSubmit(VBT_Input field)
	{
		if (field == m_InvField && m_Pop == POP_INVITE)
			CallByField();
		else if (field == m_DcField && m_Pop == POP_DISCORD)
			SaveDiscord();
	}

	override void OnPick(VBT_Dropdown list, int index)
	{
		if (list != m_RankDrop || index >= m_RankIds.Count())
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		VB_ClanMember m = client.Clan.Member(m_SelUid);
		string rank = m_RankIds.Get(index);
		if (!m || m.RankId == rank)
			return;
		string target = m_SelUid;
		ScriptRPC rpc = client.Begin(VB_TabletOp.CLAN_SET_RANK);
		rpc.Write(target);
		rpc.Write(rank);
		client.Post(rpc);
		string note = m.PlayerName + ": " + m_RankNames.Get(index);
		INT_UIF_Toast.Success(note);
	}

	override void OnAnswer(int tag, bool yes, string text)
	{
		if (VBA_Util.AnswerExit(tag, yes) || !yes)
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		string target = m_TargetUid;
		ScriptRPC rpc = client.Begin(VB_TabletOp.CLAN_KICK);
		rpc.Write(target);
		client.Post(rpc);
	}

	// Поля ввода есть только в окнах, поэтому экран обновляется на месте. Окно заявок заново открывается
	// со свежим списком, остальные окна не трогаем: в них идёт ввод.
	override bool OnData()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (!client.Clan)
		{
			ClosePop();
			m_Menu.GoLater("profile");
			return true;
		}
		if (!m_Rows)
			return false;
		if (m_AcceptUid != "" && client.Clan.Member(m_AcceptUid))
		{
			INT_UIF_Toast.Success(VBA_Util.Sub("#STR_VB_TAB_A_ACCEPTED", "{name}", m_AcceptName));
			m_AcceptUid = "";
		}
		Paint();
		if (m_Pop == POP_REQUESTS)
			OpenRequests();
		return true;
	}

	// Перерисовка после клика: кликнутый виджет ещё жив, поэтому в следующем кадре.
	override void Tick(float dt)
	{
		if (m_Need)
		{
			m_Need = false;
			PaintRows();
			PaintCard();
		}
		if (m_NeedNear)
		{
			m_NeedNear = false;
			PaintNear();
		}
	}

	override void Leave()
	{
		if (m_Menu)
			ClosePop();
		if (m_TopF)
			m_TopF.Unlink();
		m_TopF = null;
	}

	protected bool OnPopHit(Widget w)
	{
		if (m_Modal.IsDismiss(w))
		{
			ClosePop();
			return true;
		}
		if (m_Pop == POP_REQUESTS)
			return OnRequestHit(w);
		if (m_Pop == POP_INVITE)
		{
			if (m_InvCall && w == m_InvCall.Hit)
			{
				CallByField();
				return true;
			}
			for (int i = 0; i < m_NearBtns.Count(); i++)
			{
				if (w == m_NearBtns.Get(i).Hit)
				{
					SendInvite(m_NearNames.Get(i));
					m_NeedNear = true;
					return true;
				}
			}
			return true;
		}
		if (m_DcCancel && w == m_DcCancel.Hit)
			ClosePop();
		else if (m_DcSave && w == m_DcSave.Hit)
			SaveDiscord();
		else if (m_DcDelete && w == m_DcDelete.Hit)
			SendDiscord("");
		return true;
	}

	protected bool OnRequestHit(Widget w)
	{
		for (int i = 0; i < m_ReqUids.Count(); i++)
		{
			if (w == m_ReqYes.Get(i).Hit)
			{
				if (VB_TabletClient.Get().Clan.Members.Count() >= VB_TabletClient.Get().ClanMax)
				{
					INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_A_INV_FULL"));
					return true;
				}
				m_AcceptUid = m_ReqUids.Get(i);
				m_AcceptName = m_ReqNames.Get(i);
				SendRequest(m_AcceptUid, true);
				return true;
			}
			if (w == m_ReqNo.Get(i).Hit)
			{
				SendRequest(m_ReqUids.Get(i), false);
				INT_UIF_Toast.Info(VBT.T("#STR_VB_TAB_A_REJECTED"));
				return true;
			}
			VBA_IconBtn copy = m_ReqCopy.Get(i);
			if (copy && w == copy.Hit)
			{
				VBK.Copy(m_ReqDiscords.Get(i));
				return true;
			}
		}
		return true;
	}

	override bool OnHit(Widget w)
	{
		if (!m_Rows)
			return false;
		if (m_Pop != POP_NONE && m_Modal && m_Modal.IsOpen())
			return OnPopHit(w);
		int i = m_RowHits.Find(w);
		if (i >= 0)
		{
			m_SelUid = m_RowUids.Get(i);
			m_Need = true;
			return true;
		}
		int h = m_HeadHits.Find(w);
		if (h >= 0)
		{
			s_Sort = h;
			m_Need = true;
			return true;
		}
		if (m_BtnInvite && w == m_BtnInvite.Hit)
		{
			OpenInvite();
			return true;
		}
		if (m_BtnRequests && w == m_BtnRequests.Hit)
		{
			OpenRequests();
			return true;
		}
		if (m_BtnDiscord && w == m_BtnDiscord.Hit)
		{
			VB_TabletClient client = VB_TabletClient.Get();
			VB_ClanMember m = client.Clan.Member(m_SelUid);
			if (m_SelUid == client.MyUid())
				OpenDiscord();
			else if (m)
				VBK.Copy(m.Discord);
			return true;
		}
		if (m_BtnExit && w == m_BtnExit.Hit)
		{
			VBA_Util.AskExit(m_Menu);
			return true;
		}
		if (m_BtnKick && w == m_BtnKick.Hit)
		{
			AskKick();
			return true;
		}
		return false;
	}

	protected void AskKick()
	{
		VB_ClanMember m = VB_TabletClient.Get().Clan.Member(m_SelUid);
		if (!m)
			return;
		m_TargetUid = m.Uid;
		string text = VBA_Util.Sub("#STR_VB_TAB_A_KICK_TEXT", "{name}", m.PlayerName);
		m_Menu.Ask(TAG_KICK, "#STR_VB_TAB_A_KICK_ASK", text, "#STR_VB_TAB_A_KICK", true);
	}

	protected bool HoverList(Widget w, bool enter, array<ref VBT_Button> list)
	{
		foreach (VBT_Button b : list)
		{
			if (w == b.Hit)
			{
				b.Hover(enter);
				return true;
			}
		}
		return false;
	}

	protected bool HoverOne(Widget w, bool enter, VBT_Button b)
	{
		if (!b || w != b.Hit)
			return false;
		b.Hover(enter);
		return true;
	}

	override bool OnHover(Widget w, bool enter)
	{
		if (!m_Rows)
			return false;
		if (m_Pop != POP_NONE)
			return OnPopHover(w, enter);
		int i = m_RowHits.Find(w);
		if (i >= 0)
		{
			string uid = m_RowUids.Get(i);
			if (uid != m_SelUid && uid != VB_TabletClient.Get().MyUid())
			{
				int fill = 0;
				if (enter)
					fill = VB_C_PANEL2;
				m_RowBgs.Get(i).SetFill(fill);
			}
			return true;
		}
		int h = m_HeadHits.Find(w);
		if (h >= 0)
		{
			if (h != s_Sort)
			{
				int col = VB_C_DIM;
				if (enter)
					col = VB_C_TEXT;
				m_HeadTexts.Get(h).SetColor(col);
			}
			return true;
		}
		if (m_BtnDiscord && w == m_BtnDiscord.Hit)
		{
			m_BtnDiscord.Hover(enter);
			return true;
		}
		if (HoverOne(w, enter, m_BtnInvite) || HoverOne(w, enter, m_BtnRequests) || HoverOne(w, enter, m_BtnKick))
			return true;
		return HoverOne(w, enter, m_BtnExit);
	}

	protected bool OnPopHover(Widget w, bool enter)
	{
		if (HoverOne(w, enter, m_InvCall) || HoverOne(w, enter, m_DcSave) || HoverOne(w, enter, m_DcCancel) || HoverOne(w, enter, m_DcDelete))
			return true;
		if (HoverList(w, enter, m_NearBtns) || HoverList(w, enter, m_ReqYes) || HoverList(w, enter, m_ReqNo))
			return true;
		foreach (VBA_IconBtn copy : m_ReqCopy)
		{
			if (copy && w == copy.Hit)
			{
				copy.Hover(enter);
				return true;
			}
		}
		return false;
	}
}
