// Рейтинг: вкладки «Кланы» и «Игроки», карточка клана, заявка в клан, карточка игрока.
class VB_TabletPageRating extends VB_TabletTabbedPage
{
	static int s_Tab = 0;
	static int s_Mode = 0;	// кланы: 0 за неделю, 1 за всё время

	protected static const int POP_NONE = 0;
	protected static const int POP_PLAYER = 1;
	protected static const int POP_CLAN = 2;
	protected static const int POP_APPLY = 3;

	protected ref VBT_Modal m_Modal;
	protected int m_Pop;
	protected string m_ClanId;

	protected ref array<Widget> m_ClanHits;
	protected ref array<string> m_ClanIds;
	protected ref array<ref INT_UIF_Surface> m_ClanBgs;
	protected ref array<Widget> m_PlayerHits;
	protected string m_CardName;
	protected int m_CardXp;
	protected string m_CardTag;
	protected int m_CardPlace;
	protected bool m_CardMe;
	protected bool m_CardHas;
	protected ref array<int> m_PlayerRows;	// индекс в присланном рейтинге, -1 сам игрок за пределами списка
	protected ref array<ref INT_UIF_Surface> m_PlayerBgs;

	protected ref VBT_Button m_BtnWeek;
	protected ref VBT_Button m_BtnAll;
	protected ref VBT_Button m_BtnCreate;
	protected ref VBT_Button m_BtnOpen;
	protected ref VBT_Button m_BtnApply;
	protected ref VBT_Button m_BtnRevoke;
	protected ref VBT_Button m_BtnBack;
	protected ref VBT_Button m_BtnSend;
	protected ref VBA_IconBtn m_BtnCopy;
	protected ref VBT_Area m_About;
	protected ref VBT_Input m_FormDc;
	protected TextWidget m_FormCount;
	protected TextWidget m_FormErr;
	protected TextWidget m_Left;
	protected ScrollWidget m_ClanScroll;
	protected float m_ScrollAt = -1;
	protected float m_LeftT = 1;

	override int GetTab()
	{
		return s_Tab;
	}

	override void SetTab(int i)
	{
		s_Tab = i;
	}

	override void Build()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VBA_Util.Ask(VB_TabletOp.RATING);
		VBA_Util.Ask(VB_TabletOp.CLAN_LIST);
		m_ClanHits = new array<Widget>();
		m_ClanIds = new array<string>();
		m_ClanBgs = new array<ref INT_UIF_Surface>();
		m_PlayerHits = new array<Widget>();
		m_PlayerRows = new array<int>();
		m_PlayerBgs = new array<ref INT_UIF_Surface>();
		InitTabs(160);
		m_TabNames.Insert("#STR_VB_TAB_CLANS");
		m_TabNames.Insert("#STR_VB_TAB_PLAYERS");
		PaintTabs();
		if (s_Tab == 0)
			BuildClans(client);
		else
			BuildPlayers(client);
	}

	// --- кланы --------------------------------------------------------------------------------
	protected int ValueOf(VB_ClanRow row)
	{
		if (s_Mode == 1)
			return row.Influence;
		return row.WeekInfluence;
	}

	protected void AddClanHit(Widget hit, string id, INT_UIF_Surface bg)
	{
		m_ClanHits.Insert(hit);
		m_ClanIds.Insert(id);
		m_ClanBgs.Insert(bg);
	}

	protected void BuildClans(VB_TabletClient client)
	{
		array<VB_ClanRow> list = VBA_Util.SortedClans(s_Mode);
		int n = list.Count();
		string own = "";
		if (client.Clan)
			own = client.Clan.Id;
		int top = 1;
		if (n > 0)
			top = Math.Max(1, ValueOf(list.Get(0)));

		array<float> px = {386, 0, 802};
		array<float> py = {0, 18, 18};
		array<float> pw = {400, 370, 370};
		array<float> ph = {168, 150, 150};
		array<int> medal = {VB_C_GOLD, VB_C_SILVER, VB_C_BRONZE};
		for (int i = 0; i < 3 && i < n; i++)
		{
			VB_ClanRow row = list.Get(i);
			float w = pw.Get(i);
			float h = ph.Get(i);
			int m = medal.Get(i);
			int val = ValueOf(row);
			Widget f = INT_UIF_Draw.Frame(m_Root, px.Get(i), py.Get(i), w, h);
			int border = VB_C_LINE;
			if (row.Id == own)
				border = VB_C_ACCENT;
			INT_UIF_Surface.Box(f, 0, 0, w, h, 10, VB_C_PANEL, border);
			VBT.Glow(f, w, h, m);
			INT_UIF_Draw.Line(f, 14, 0, w - 28, 3, m);
			float ts = 36;
			if (i == 0)
				ts = 42;
			INT_UIF_Draw.Text(f, 16, 12, 44, 50, (i + 1).ToString(), ts, m, INT_UIF_Align.LEFT, true);
			Widget s = INT_UIF_Draw.Frame(f, 62, 18, 92, 92);
			INT_UIF_Surface.Box(s, 0, 0, 92, 92, 8, VB_C_PANEL2);
			VBT.Corners(s, 92, 92, m, 10);
			VBK.Emblem(s, 12, 12, 68, row.EmbShape, row.EmbSymbol, row.Color);
			string title = VBT.Fit(row.Title, w - 186, 20);
			INT_UIF_Draw.Text(f, 170, 16, w - 186, 30, title, 20, VB_C_TEXT);
			string sub = "[" + row.Tag + "] · " + VBT.T("#STR_VB_TAB_A_LEVEL_L") + " " + row.Level.ToString();
			INT_UIF_Draw.Text(f, 170, 46, w - 186, 22, sub, 14, VB_C_DIM);
			VBT.Ico(f, 170, 78, 20, "sym_bolt", VB_C_INF);
			INT_UIF_Draw.Text(f, 198, 72, w - 214, 32, VBT.Num(val), 24, VB_C_INF);
			int ticks = (w - 32) / 9;
			VBT.Ticks(f, 16, h - 26, w - 32, 10, ticks, val / (top + 0.0), m);
			AddClanHit(HitAt(f, 0, 0, w, h), row.Id, null);
		}

		Widget t = VBT.Card(m_Root, 0, 184, 800, 536);
		VBT.Cap(t, 24, 18, 40, "#STR_VB_TAB_NUM");
		VBT.Cap(t, 76, 18, 300, "#STR_VB_TAB_CLAN");
		VBT.Cap(t, 400, 18, 70, "#STR_VB_TAB_A_LEVEL_C");
		VBT.Cap(t, 480, 18, 70, "#STR_VB_TAB_A_ROSTER_C");
		INT_UIF_Draw.Text(t, 560, 18, 216, 18, "#STR_VB_TAB_A_INFLUENCE_C", 13, VB_C_DIM, INT_UIF_Align.RIGHT);
		INT_UIF_Draw.Line(t, 0, 46, 800, 1, VB_C_LINE);
		if (n == 0)
			INT_UIF_Draw.Text(t, 0, 252, 800, 32, "#STR_VB_TAB_NO_CLANS", 17, VB_C_DIM, INT_UIF_Align.CENTER);
		m_ClanScroll = INT_UIF_Draw.Scroll(t, 0, 47, 800, 488);
		Widget L = INT_UIF_Draw.ScrollContent(m_ClanScroll);
		int rest = Math.Max(0, n - 3);
		INT_UIF_Draw.SetScrollContent(m_ClanScroll, rest * 47 + 6);
		for (int k = 0; k < rest; k++)
		{
			VB_ClanRow r = list.Get(k + 3);
			DrawClanRow(L, 3 + k * 47, r, k + 4, r.Id == own, top);
		}

		int myPlace = 0;
		for (int q = 0; q < n; q++)
		{
			if (list.Get(q).Id == own)
				myPlace = q + 1;
		}
		if (myPlace > 3)
			m_ScrollAt = Math.Max(0, (myPlace - 4) * 47 - 200);

		BuildMode();
		BuildRewards();
		BuildOwnClan(list, myPlace);
	}

	protected void DrawClanRow(Widget L, float y, VB_ClanRow r, int place, bool mine, int top)
	{
		INT_UIF_Surface bg = null;
		if (mine)
		{
			INT_UIF_Surface.Box(L, 8, y, 776, 44, 6, VB_C_PANEL2);
			INT_UIF_Surface.Box(L, 8, y + 9, 3, 26, 2, VB_C_ACCENT);
		}
		else
		{
			bg = INT_UIF_Surface.Box(L, 8, y, 776, 44, 6, 0);
		}
		int val = ValueOf(r);
		INT_UIF_Draw.Text(L, 24, y, 40, 44, place.ToString(), 16, VB_C_DIM);
		VBK.Emblem(L, 70, y + 6, 32, r.EmbShape, r.EmbSymbol, r.Color);
		int col = VB_C_TEXT;
		if (mine)
			col = VB_C_ACCENT;
		string title = VBT.Fit(r.Title + "  [" + r.Tag + "]", 280, 16);
		INT_UIF_Draw.Text(L, 112, y, 280, 44, title, 16, col);
		INT_UIF_Draw.Text(L, 400, y, 70, 44, r.Level.ToString(), 15, VB_C_DIM);
		INT_UIF_Draw.Text(L, 480, y, 70, 44, r.MemberCount.ToString(), 15, VB_C_DIM);
		VBT.Ticks(L, 560, y + 16, 120, 12, 16, val / (top + 0.0), VB_C_INF);
		INT_UIF_Draw.Text(L, 686, y, 90, 44, VBT.Num(val), 16, VB_C_INF, INT_UIF_Align.RIGHT);
		AddClanHit(HitAt(L, 8, y, 776, 44), r.Id, bg);
	}

	protected void BuildMode()
	{
		m_BtnWeek = new VBT_Button(m_Root, 816, 184, 170, 40, "#STR_VB_TAB_A_FOR_WEEK", s_Mode == 0, "", m_Menu.GetHitHandler(), 14);
		m_BtnAll = new VBT_Button(m_Root, 1002, 184, 170, 40, "#STR_VB_TAB_A_FOR_ALL", s_Mode == 1, "", m_Menu.GetHitHandler(), 14);
	}

	protected void BuildRewards()
	{
		array<int> rewards = {100000, 60000, 30000};
		array<int> medal = {VB_C_GOLD, VB_C_SILVER, VB_C_BRONZE};
		Widget w = INT_UIF_Draw.Frame(m_Root, 816, 240, 356, 226);
		INT_UIF_Surface.Box(w, 0, 0, 356, 226, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Corners(w, 356, 226, VB_C_GOLD, 14);
		VBT.Cap(w, 20, 16, 316, "#STR_VB_TAB_A_WEEK_REWARD_C");
		for (int i = 0; i < 3; i++)
		{
			float y = 46 + i * 40;
			INT_UIF_Draw.Text(w, 20, y, 30, 30, (i + 1).ToString(), 20, medal.Get(i));
			INT_UIF_Draw.Text(w, 50, y, 120, 30, "#STR_VB_TAB_PLACE", 15, VB_C_DIM);
			VBK.MoneyR(w, 336, y + 3, rewards.Get(i), 18);
		}
		INT_UIF_Draw.Line(w, 20, 170, 316, 1, VB_C_LINE);
		INT_UIF_Draw.Text(w, 20, 178, 120, 32, "#STR_VB_TAB_A_RESULTS_IN", 14, VB_C_DIM);
		m_Left = INT_UIF_Draw.Text(w, 140, 178, 196, 32, "", 16, VB_C_TEXT, INT_UIF_Align.RIGHT);
		m_LeftT = 1;
	}

	protected void BuildOwnClan(array<VB_ClanRow> list, int place)
	{
		Widget m = VBT.Card(m_Root, 816, 482, 356, 238, "#STR_VB_TAB_A_YOUR_CLAN_C");
		if (place == 0)
		{
			// Клан есть, а список ещё не пришёл: кнопку создания не показываем.
			if (VB_TabletClient.Get().Clan)
				return;
			INT_UIF_Draw.MText(m, 20, 48, 316, 90, "#STR_VB_TAB_A_RATING_HINT", 16, VB_C_DIM);
			m_BtnCreate = new VBT_Button(m, 20, 170, 316, 48, "#STR_VB_TAB_CREATE", true, "", m_Menu.GetHitHandler());
			return;
		}
		VB_ClanRow me = list.Get(place - 1);
		int val = ValueOf(me);
		VBK.Emblem(m, 20, 44, 52, me.EmbShape, me.EmbSymbol, me.Color);
		string title = VBT.Fit(me.Title, 248, 20);
		INT_UIF_Draw.Text(m, 88, 42, 248, 28, title, 20, VB_C_TEXT);
		string lvl = VBT.T("#STR_VB_TAB_A_LEVEL") + " " + me.Level.ToString();
		INT_UIF_Draw.Text(m, 88, 70, 248, 22, lvl, 14, VB_C_DIM);
		VBT.Cap(m, 20, 110, 150, "#STR_VB_TAB_PLACE_C");
		INT_UIF_Draw.Text(m, 20, 130, 150, 44, place.ToString(), 34, VB_C_ACCENT, INT_UIF_Align.LEFT, true);
		VBT.Cap(m, 180, 110, 156, "#STR_VB_TAB_A_INFLUENCE_C");
		INT_UIF_Draw.Text(m, 180, 130, 156, 44, VBT.Num(val), 26, VB_C_INF);
		if (place == 1)
		{
			INT_UIF_Draw.Text(m, 20, 186, 316, 24, "#STR_VB_TAB_A_FIRST_PLACE", 15, VB_C_ACCENT);
			return;
		}
		int upVal = ValueOf(list.Get(place - 2));
		VBT.Ticks(m, 20, 182, 316, 10, 34, val / Math.Max(1, upVal), VB_C_ACCENT);
		int prev = place - 1;
		string line = VBA_Util.Sub("#STR_VB_TAB_A_TO_PLACE_INF", "{place}", prev.ToString());
		line.Replace("{n}", VBT.Num(upVal - val + 1));
		INT_UIF_Draw.Text(m, 20, 198, 316, 24, line, 14, VB_C_DIM);
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

	protected void OpenClan(string id)
	{
		VB_ClanRow c = VBA_Util.FindClan(id);
		if (!c)
			return;
		ClosePop();
		m_ClanId = id;
		VB_TabletClient client = VB_TabletClient.Get();
		float w = 600;
		float h = 352;
		bool hasDc = c.Discord != "";
		if (hasDc)
			h = 408;
		m_Modal = new VBT_Modal(m_Menu, w, h);
		m_Pop = POP_CLAN;
		Widget b = m_Modal.Body;
		Widget s = INT_UIF_Draw.Frame(b, 28, 28, 112, 112);
		INT_UIF_Surface.Box(s, 0, 0, 112, 112, 8, VB_C_PANEL2);
		VBT.Corners(s, 112, 112, VB_C_ACCENT, 10);
		VBK.Emblem(s, 14, 14, 84, c.EmbShape, c.EmbSymbol, c.Color);
		string title = VBT.Fit(c.Title, 380, 26);
		INT_UIF_Draw.Text(b, 160, 26, 380, 38, title, 26, VB_C_TEXT);
		string sub = "[" + c.Tag + "] · " + VBT.T("#STR_VB_TAB_A_LEVEL_L") + " " + c.Level.ToString();
		INT_UIF_Draw.Text(b, 160, 64, 380, 24, sub, 15, VB_C_DIM);
		string openText = "#STR_VB_TAB_A_OPEN_YES";
		int openCol = VB_C_OK;
		if (!c.Open)
		{
			openText = "#STR_VB_TAB_A_OPEN_NO";
			openCol = VB_C_DIM2;
		}
		VBK.Pill(b, 160, 100, openText, openCol);
		string slogan = "#STR_VB_TAB_A_NO_SLOGAN";
		if (c.Slogan != "")
		{
			string cut = VBT.Fit(c.Slogan, w - 90, 15);
			slogan = "«" + cut + "»";
		}
		INT_UIF_Draw.MText(b, 28, 156, w - 56, 28, slogan, 15, VB_C_DIM);
		INT_UIF_Draw.Line(b, 28, 190, w - 56, 1, VB_C_LINE);

		int cap = VB_TabletClient.Get().ClanMax;
		int place = VBA_Util.ClanPlace(c.Id);
		VBT.Cap(b, 28, 202, 150, "#STR_VB_TAB_A_MEMBERS_C");
		INT_UIF_Draw.Text(b, 28, 222, 150, 32, c.MemberCount.ToString() + "/" + cap.ToString(), 20, VB_C_TEXT);
		VBT.Cap(b, 188, 202, 120, "#STR_VB_TAB_PLACE_C");
		INT_UIF_Draw.Text(b, 188, 222, 120, 32, place.ToString(), 20, VB_C_TEXT);
		VBT.Cap(b, 318, 202, 150, "#STR_VB_TAB_A_LEADER_C");
		string lead = VBT.Fit(c.Leader, 190, 20);
		INT_UIF_Draw.Text(b, 318, 222, 254, 32, lead, 20, VB_C_TEXT);

		m_BtnCopy = null;
		if (hasDc)
		{
			INT_UIF_Surface.Box(b, 28, 266, w - 56, 44, 8, VB_C_PANEL2);
			VBT.Ico(b, 42, 277, 22, "discord", VB_C_ACCENT);
			string dc = VBT.Fit(c.Discord, 430, 15);
			INT_UIF_Draw.Text(b, 76, 266, 430, 44, dc, 15, VB_C_TEXT);
			m_BtnCopy = new VBA_IconBtn(b, w - 72, 272, 36, 32, "copy", m_Menu.GetHitHandler());
		}
		BuildClanAction(b, client, c, w, h - 68);
	}

	protected void BuildClanAction(Widget b, VB_TabletClient client, VB_ClanRow c, float w, float y)
	{
		m_BtnOpen = null;
		m_BtnApply = null;
		m_BtnRevoke = null;
		bool full = c.MemberCount >= VB_TabletClient.Get().ClanMax;
		if (client.Clan && client.Clan.Id == c.Id)
		{
			m_BtnOpen = new VBT_Button(b, 28, y, w - 56, 44, "#STR_VB_TAB_A_OPEN_CLAN", true, "flag", m_Menu.GetHitHandler());
		}
		else if (client.Clan)
		{
			INT_UIF_Draw.Text(b, 28, y, w - 56, 44, "#STR_VB_TAB_A_APPLY_NOCLAN", 15, VB_C_DIM2);
		}
		else if (client.AppliedClan == c.Id)
		{
			INT_UIF_Draw.Text(b, 28, y, 300, 44, "#STR_VB_TAB_A_APPLY_WAIT", 15, VB_C_ACCENT);
			m_BtnRevoke = new VBT_Button(b, w - 188, y, 160, 44, "#STR_VB_TAB_A_REVOKE", false, "", m_Menu.GetHitHandler());
		}
		else if (client.AppliedClan != "")
		{
			string other = VBA_DASH;
			VB_ClanRow row = VBA_Util.FindClan(client.AppliedClan);
			if (row)
				other = row.Title;
			string line = VBA_Util.Sub("#STR_VB_TAB_A_APPLY_OTHER", "{name}", other);
			line = VBT.Fit(line, w - 56, 15);
			INT_UIF_Draw.Text(b, 28, y, w - 56, 44, line, 15, VB_C_DIM2);
		}
		else if (full)
		{
			INT_UIF_Draw.Text(b, 28, y, w - 56, 44, "#STR_VB_TAB_A_INV_FULL", 15, VB_C_DIM2);
		}
		else if (!c.Open)
		{
			INT_UIF_Draw.Text(b, 28, y, w - 56, 44, "#STR_VB_TAB_A_APPLY_CLOSED", 15, VB_C_DIM2);
		}
		else
		{
			m_BtnApply = new VBT_Button(b, 28, y, w - 56, 44, "#STR_VB_TAB_A_APPLY", true, "send", m_Menu.GetHitHandler());
		}
	}

	protected void OpenApply()
	{
		VB_ClanRow c = VBA_Util.FindClan(m_ClanId);
		if (!c)
			return;
		ClosePop();
		float w = 600;
		float h = 438;
		m_Modal = new VBT_Modal(m_Menu, w, h);
		m_Pop = POP_APPLY;
		Widget b = m_Modal.Body;
		string head = VBA_Util.Sub("#STR_VB_TAB_A_APPLY_TITLE", "{name}", c.Title);
		head = VBT.Fit(head, w - 90, 22);
		INT_UIF_Draw.Text(b, 28, 22, w - 90, 32, head, 22, VB_C_TEXT);
		VBT.Cap(b, 28, 70, 300, "#STR_VB_TAB_A_ABOUT_C");
		int cap = VBA_APPLY_MAX;
		m_FormCount = INT_UIF_Draw.Text(b, w - 128, 70, 100, 18, "0/" + cap.ToString(), 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
		m_About = new VBT_Area(b, 28, 92, w - 56, 126, "#STR_VB_TAB_A_ABOUT_HINT");
		VBT.Cap(b, 28, 232, 300, "#STR_VB_TAB_A_DC_CONTACT_C");
		INT_UIF_Draw.Text(b, w - 228, 232, 200, 18, "#STR_VB_TAB_A_OPTIONAL", 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
		m_FormDc = new VBT_Input(b, 28, 254, w - 56, 46, "#STR_VB_TAB_A_DC_HINT", 32, VB_TabletClient.Get().Me.Discord);
		m_FormErr = INT_UIF_Draw.Text(b, 28, 306, w - 56, 24, "", 14, VB_C_BAD);
		m_BtnBack = new VBT_Button(b, w - 328, h - 68, 140, 44, "#STR_VB_TAB_A_BACK", false, "", m_Menu.GetHitHandler());
		m_BtnSend = new VBT_Button(b, w - 178, h - 68, 150, 44, "#STR_VB_TAB_A_SEND", true, "send", m_Menu.GetHitHandler(), 14);
		SetFocus(m_About.Edit);
	}

	protected void SendApply()
	{
		string text = m_About.Get();
		string ds = VBA_Util.StripAt(m_FormDc.Get());
		if (text.LengthUtf8() < 10)
		{
			m_FormErr.SetText("#STR_VB_TAB_A_ABOUT_SHORT");
			return;
		}
		if (ds != "")
		{
			string err = VBA_Util.DiscordError(ds);
			if (err != "")
			{
				string prefix = VBT.T("#STR_VB_TAB_DISCORD");
				string reason = VBT.T(err);
				m_FormErr.SetText(prefix + ": " + reason);
				return;
			}
		}
		VB_TabletClient client = VB_TabletClient.Get();
		string id = m_ClanId;
		ScriptRPC rpc = client.Begin(VB_TabletOp.CLAN_APPLY);
		rpc.Write(id);
		rpc.Write(text);
		rpc.Write(ds);
		client.Post(rpc);
		ClosePop();
	}

	// Карточка игрока видна всем, Discord в ней нет. Сервер присылает показатели только самому игроку,
	// у остальных прочерки. Сводные поля берутся из строки рейтинга.
	protected void OpenPlayer(string name, int xp, string tag, int place, bool isMe)
	{
		ClosePop();
		VB_TabletClient client = VB_TabletClient.Get();
		VB_TabletStats me = client.Me;
		if (!isMe)
		{
			client.RequestCard("", name);
			me = client.CardOf(name);
		}
		m_CardName = name;
		m_CardXp = xp;
		m_CardTag = tag;
		m_CardPlace = place;
		m_CardMe = isMe;
		m_CardHas = me != null;
		float w = 600;
		float h = 384;
		m_Modal = new VBT_Modal(m_Menu, w, h);
		m_Pop = POP_PLAYER;
		Widget b = m_Modal.Body;
		int ri = client.Rates.RankOf(xp);
		Widget s = INT_UIF_Draw.Frame(b, 28, 28, 112, 112);
		INT_UIF_Surface.Box(s, 0, 0, 112, 112, 8, VB_C_PANEL2);
		VBT.Corners(s, 112, 112, VB_C_ACCENT, 10);
		VBT.Insignia(s, 20, 20, 72, ri);
		string shown = VBT.Fit(name, 380, 26);
		INT_UIF_Draw.Text(b, 160, 26, 380, 38, shown, 26, VB_C_TEXT);
		INT_UIF_Draw.Text(b, 160, 64, 380, 24, VBT.RankName(ri), 16, VB_C_ACCENT);
		VB_ClanRow clan = null;
		if (tag != "")
			clan = VBA_Util.ClanByTag(tag);
		if (clan)
		{
			VBK.Emblem(b, 160, 100, 24, clan.EmbShape, clan.EmbSymbol, clan.Color);
			string line = VBT.Fit("[" + clan.Tag + "] " + clan.Title, 348, 15);
			INT_UIF_Draw.Text(b, 192, 98, 348, 28, line, 15, VB_C_DIM);
		}
		else if (tag != "")
		{
			INT_UIF_Draw.Text(b, 160, 98, 300, 28, "[" + tag + "]", 15, VB_C_DIM);
		}
		else
		{
			INT_UIF_Draw.Text(b, 160, 98, 300, 28, "#STR_VB_TAB_A_NO_CLAN_L", 15, VB_C_DIM2);
		}
		array<string> caps = {"#STR_VB_TAB_ZOMBIES", "#STR_VB_TAB_MUTANTS", "#STR_VB_TAB_ANIMALS", "#STR_VB_TAB_DIST", "#STR_VB_TAB_RECORD", "#STR_VB_TAB_A_ACH_C"};
		array<string> vals = new array<string>();
		if (me)
		{
			int got = me.AchCount(client.Rates);
			int total = VB_ACH_COUNT * VB_ACH_STEPS;
			vals.Insert(VBT.Num(me.Zombies));
			vals.Insert(VBT.Num(me.Mutants));
			vals.Insert(VBT.Num(me.Animals));
			vals.Insert(VBT.Km(me.DistM));
			vals.Insert(VBT.LifeText(me.RecordSec));
			vals.Insert(got.ToString() + "/" + total.ToString());
		}
		for (int i = 0; i < 6; i++)
		{
			int col2 = i % 3;
			int row2 = i / 3;
			float x = 28 + col2 * 184;
			float y = 156 + row2 * 70;
			INT_UIF_Surface.Box(b, x, y, 176, 62, 8, VB_C_PANEL2);
			INT_UIF_Draw.Text(b, x + 12, y + 7, 156, 18, caps.Get(i), 12, VB_C_DIM);
			string val = VBA_DASH;
			int ink = VB_C_DIM2;
			if (me)
			{
				val = VBT.Fit(vals.Get(i), 156, 17);
				ink = VB_C_TEXT;
				if (i == 4)
					ink = VB_C_GOLD;
			}
			INT_UIF_Draw.Text(b, x + 12, y + 28, 156, 28, val, 17, ink);
		}
		VBT.Cap(b, 28, 306, 150, "#STR_VB_TAB_PLACE_C");
		INT_UIF_Draw.Text(b, 28, 326, 150, 34, place.ToString(), 24, VB_C_ACCENT);
		VBT.Cap(b, 212, 306, 150, "#STR_VB_TAB_XP");
		INT_UIF_Draw.Text(b, 212, 326, 300, 34, VBT.Num(xp), 24, VB_C_TEXT);
	}

	// --- игроки -------------------------------------------------------------------------------
	protected void ClanOf(Widget f, float x, float y, string tag)
	{
		if (tag == "")
		{
			INT_UIF_Draw.Text(f, x, y, 120, 24, "#STR_VB_TAB_A_NO_CLAN_L", 14, VB_C_DIM2);
			return;
		}
		VB_ClanRow c = VBA_Util.ClanByTag(tag);
		if (c)
		{
			VBK.Emblem(f, x, y + 2, 20, c.EmbShape, c.EmbSymbol, c.Color);
			x = x + 27;
		}
		INT_UIF_Draw.Text(f, x, y, 110, 24, "[" + tag + "]", 14, VB_C_DIM);
	}

	protected void BuildPlayers(VB_TabletClient session)
	{
		VB_TabletRates rates = session.Rates;
		int n = session.RatingNames.Count();
		if (n == 0)
		{
			INT_UIF_Draw.Text(m_Root, 0, 340, 1172, 32, "#STR_VB_TAB_NO_DATA", 17, VB_C_DIM, INT_UIF_Align.CENTER);
			return;
		}
		int top = Math.Max(1, session.RatingXp.Get(0));
		array<float> px = {386, 0, 802};
		array<float> py = {0, 18, 18};
		array<float> pw = {400, 370, 370};
		array<float> ph = {168, 150, 150};
		array<int> medal = {VB_C_GOLD, VB_C_SILVER, VB_C_BRONZE};
		string own = session.Me.PlayerName;
		for (int i = 0; i < 3 && i < n; i++)
		{
			float w = pw.Get(i);
			float h = ph.Get(i);
			int m = medal.Get(i);
			int xp = session.RatingXp.Get(i);
			string name = session.RatingNames.Get(i);
			Widget f = INT_UIF_Draw.Frame(m_Root, px.Get(i), py.Get(i), w, h);
			int border = VB_C_LINE;
			if (name == own)
				border = VB_C_ACCENT;
			INT_UIF_Surface.Box(f, 0, 0, w, h, 10, VB_C_PANEL, border);
			VBT.Glow(f, w, h, m);
			INT_UIF_Draw.Line(f, 14, 0, w - 28, 3, m);
			float ts = 36;
			if (i == 0)
				ts = 42;
			INT_UIF_Draw.Text(f, 16, 12, 44, 50, (i + 1).ToString(), ts, m);
			Widget s = INT_UIF_Draw.Frame(f, 62, 18, 92, 92);
			INT_UIF_Surface.Box(s, 0, 0, 92, 92, 8, VB_C_PANEL2);
			VBT.Corners(s, 92, 92, m, 10);
			int ri = rates.RankOf(xp);
			VBT.Insignia(s, 14, 14, 64, ri, m);
			INT_UIF_Draw.Text(f, 170, 14, w - 186, 30, VBT.Fit(name, w - 186, 20), 20, VB_C_TEXT);
			INT_UIF_Draw.Text(f, 170, 44, w - 186, 22, VBT.RankName(ri), 15, m);
			ClanOf(f, 170, 68, session.RatingTags.Get(i));
			INT_UIF_Draw.Text(f, w - 136, 68, 120, 24, VBT.Short(xp), 16, VB_C_TEXT, INT_UIF_Align.RIGHT);
			VBT.Ticks(f, 16, h - 26, w - 32, 10, (w - 32) / 9, xp / (top + 0.0), m);
			AddPlayerHit(HitAt(f, 0, 0, w, h), i, null);
		}
		Widget t = VBT.Card(m_Root, 0, 184, 800, 536);
		VBT.Cap(t, 24, 18, 40, "#STR_VB_TAB_NUM");
		VBT.Cap(t, 76, 18, 200, "#STR_VB_TAB_PLAYER");
		VBT.Cap(t, 280, 18, 120, "#STR_VB_TAB_CLAN");
		VBT.Cap(t, 410, 18, 180, "#STR_VB_TAB_RANK_C");
		INT_UIF_Draw.Text(t, 620, 18, 156, 18, "#STR_VB_TAB_XP", 13, VB_C_DIM, INT_UIF_Align.RIGHT);
		INT_UIF_Draw.Line(t, 0, 46, 800, 1, VB_C_LINE);
		int place = session.Me.Place;
		int myXp = session.Me.Xp(rates);
		for (int k = 3; k < 13 && k < n; k++)
		{
			float y = 50 + (k - 3) * 47;
			// Игрок за пределами присланных 13 строк: как в макете занимает последнюю строку.
			if (k == 12 && place > 13)
			{
				DrawPlayerRow(t, session, y, place, own, myXp, MyTag(session), true, -1);
				continue;
			}
			string rowName = session.RatingNames.Get(k);
			string rowTag = session.RatingTags.Get(k);
			int rowXp = session.RatingXp.Get(k);
			DrawPlayerRow(t, session, y, k + 1, rowName, rowXp, rowTag, rowName == own, k);
		}
		BuildSide(session);
	}

	protected void AddPlayerHit(Widget hit, int index, INT_UIF_Surface bg)
	{
		m_PlayerHits.Insert(hit);
		m_PlayerRows.Insert(index);
		m_PlayerBgs.Insert(bg);
	}

	protected string MyTag(VB_TabletClient session)
	{
		if (session.Clan)
			return session.Clan.Tag;
		return "";
	}

	protected void DrawPlayerRow(Widget t, VB_TabletClient session, float y, int place, string name, int xp, string tag, bool mine, int index)
	{
		int ra = session.Rates.RankOf(xp);
		int col = VB_C_TEXT;
		int ic = VB_C_DIM;
		INT_UIF_Surface bg = null;
		if (mine)
		{
			INT_UIF_Surface.Box(t, 8, y, 784, 44, 6, VB_C_PANEL2);
			INT_UIF_Draw.Line(t, 8, y + 9, 3, 26, VB_C_ACCENT);
			col = VB_C_ACCENT;
			ic = VB_C_ACCENT;
		}
		else
		{
			bg = INT_UIF_Surface.Box(t, 8, y, 784, 44, 6, 0);
		}
		INT_UIF_Draw.Text(t, 24, y, 44, 44, place.ToString(), 16, VB_C_DIM);
		string shown = VBT.Fit(name, 196, 16);
		INT_UIF_Draw.Text(t, 76, y, 196, 44, shown, 16, col);
		ClanOf(t, 280, y + 10, tag);
		VBT.Insignia(t, 410, y + 8, 28, ra, ic);
		INT_UIF_Draw.Text(t, 446, y, 150, 44, VBT.RankName(ra), 15, col);
		INT_UIF_Draw.Text(t, 620, y, 156, 44, VBT.Num(xp), 16, VB_C_TEXT, INT_UIF_Align.RIGHT);
		AddPlayerHit(HitAt(t, 8, y, 784, 44), index, bg);
	}

	protected void BuildSide(VB_TabletClient session)
	{
		VB_TabletRates rates = session.Rates;
		VB_TabletStats me = session.Me;
		int xp = me.Xp(rates);
		int ri = rates.RankOf(xp);
		Widget m = INT_UIF_Draw.Frame(m_Root, 816, 184, 356, 256);
		INT_UIF_Surface.Box(m, 0, 0, 356, 256, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(m, 356, 256);
		VBT.Corners(m, 356, 256, VB_C_ACCENT, 14);
		VBT.Cap(m, 20, 18, 316, "#STR_VB_TAB_YOUR_PLACE");
		VBT.Insignia(m, 20, 44, 60, ri);
		INT_UIF_Draw.Text(m, 92, 44, 244, 30, VBT.Fit(me.PlayerName, 244, 22), 22, VB_C_TEXT);
		INT_UIF_Draw.Text(m, 92, 76, 244, 22, VBT.RankName(ri), 15, VB_C_ACCENT);
		VBT.Cap(m, 20, 122, 150, "#STR_VB_TAB_PLACE_C");
		INT_UIF_Draw.Text(m, 20, 142, 150, 44, me.Place.ToString(), 34, VB_C_ACCENT);
		VBT.Cap(m, 180, 122, 156, "#STR_VB_TAB_XP");
		INT_UIF_Draw.Text(m, 180, 142, 156, 44, VBT.Num(xp), 26, VB_C_TEXT);
		// Опыт соседа сверху известен, только если он попал в присланные строки.
		int upIndex = me.Place - 2;
		if (me.Place == 1)
		{
			INT_UIF_Draw.Text(m, 20, 200, 316, 24, "#STR_VB_TAB_A_FIRST_PLACE", 15, VB_C_ACCENT);
		}
		else if (upIndex >= 0 && upIndex < session.RatingXp.Count())
		{
			int upXp = session.RatingXp.Get(upIndex);
			VBT.Ticks(m, 20, 194, 316, 10, 34, xp / Math.Max(1, upXp));
			int prev = me.Place - 1;
			string line = VBA_Util.Sub("#STR_VB_TAB_A_TO_PLACE_XP", "{place}", prev.ToString());
			line.Replace("{n}", VBT.Num(upXp - xp + 1));
			INT_UIF_Draw.Text(m, 20, 212, 316, 24, line, 14, VB_C_DIM);
		}

		Widget d = VBT.Card(m_Root, 816, 456, 356, 264, "#STR_VB_TAB_RANKS_SERVER");
		if (session.RatingRanks.Count() < VB_RANK_COUNT)
			return;
		int peak = 1;
		for (int i = 0; i < VB_RANK_COUNT; i++)
			peak = Math.Max(peak, session.RatingRanks.Get(i));
		float cw = 32;
		float axis = 204;
		INT_UIF_Draw.Line(d, 16, axis, 324, 1, VB_C_LINE2);
		for (int k = 0; k < VB_RANK_COUNT; k++)
		{
			float x = 18 + k * cw;
			int c = session.RatingRanks.Get(k);
			bool cur = k == ri;
			int col = VB_C_DIM2;
			if (cur)
				col = VB_C_ACCENT;
			if (c > 0)
			{
				float h = Math.Max(6, c * 128.0 / peak);
				INT_UIF_Draw.Line(d, x + 3, axis - h, cw - 8, h, col);
				int tc = VB_C_DIM;
				if (cur)
					tc = VB_C_ACCENT;
				INT_UIF_Draw.Text(d, x - 4, axis - h - 22, cw + 6, 20, c.ToString(), 13, tc, INT_UIF_Align.CENTER);
			}
			int ic = VB_C_DIM2;
			if (cur)
				ic = VB_C_ACCENT;
			VBT.Insignia(d, x + 3, axis + 10, 24, k, ic);
		}
	}

	// --- события ------------------------------------------------------------------------------
	override void Tick(float dt)
	{
		if (m_ScrollAt >= 0 && m_ClanScroll)
		{
			m_ClanScroll.VScrollToPos(INT_UIF_Draw.S(m_ScrollAt));
			m_ScrollAt = -1;
		}
		if (!m_Left)
			return;
		m_LeftT += dt;
		if (m_LeftT < 1)
			return;
		m_LeftT = 0;
		int y;
		int mo;
		int d;
		int h;
		int mi;
		int s;
		GetYearMonthDay(y, mo, d);
		GetHourMinuteSecond(h, mi, s);
		// До ближайшего понедельника 00:00.
		int wd = VBA_Util.WeekDay(y, mo, d);
		int sec = (6 - wd) * 86400 + 86400 - (h * 3600 + mi * 60 + s);
		int dd = sec / 86400;
		int dayRest = sec % 86400;
		int hh = dayRest / 3600;
		int hourRest = sec % 3600;
		int mn = hourRest / 60;
		string unitD = VBT.T("#STR_VB_TAB_A_D");
		string unitH = VBT.T("#STR_VB_TAB_H");
		string unitM = VBT.T("#STR_VB_TAB_MIN");
		m_Left.SetText(dd.ToString() + " " + unitD + " " + hh.ToString() + " " + unitH + " " + mn.ToString() + " " + unitM);
	}

	override void Leave()
	{
		if (m_Menu)
			ClosePop();
		super.Leave();
	}

	// Открытое окно не сносим: в заявке идёт ввод, карточка клана обновляется со свежим состоянием.
	override bool OnData()
	{
		if (m_Pop == POP_CLAN)
		{
			OpenClan(m_ClanId);
			return true;
		}
		if (m_Pop == POP_PLAYER && !m_CardHas && VB_TabletClient.Get().CardOf(m_CardName))
		{
			OpenPlayer(m_CardName, m_CardXp, m_CardTag, m_CardPlace, m_CardMe);
			return true;
		}
		if (m_Pop != POP_NONE)
			return true;
		return false;
	}

	override void OnInput(VBT_Input field)
	{
		if (field != m_FormDc || m_Pop != POP_APPLY)
			return;
		string raw = field.Edit.GetText();
		string low = raw + "";
		low.ToLower();
		if (low != raw)
			field.Set(low);
		m_FormErr.SetText("");
	}

	override void OnArea(VBT_Area field)
	{
		if (field != m_About || m_Pop != POP_APPLY)
			return;
		string s;
		field.Edit.GetText(s);
		int n = s.LengthUtf8();
		int cap = VBA_APPLY_MAX;
		if (n > cap)
		{
			field.Edit.SetText(s.SubstringUtf8(0, cap));
			n = cap;
		}
		m_FormCount.SetText(n.ToString() + "/" + cap.ToString());
		m_FormErr.SetText("");
	}

	protected bool OnPopHit(Widget w)
	{
		if (m_Modal.IsDismiss(w))
		{
			ClosePop();
			return true;
		}
		VB_TabletClient client = VB_TabletClient.Get();
		if (m_BtnCopy && m_Pop == POP_CLAN && w == m_BtnCopy.Hit)
		{
			VB_ClanRow row = VBA_Util.FindClan(m_ClanId);
			if (row)
				VBK.Copy("https://" + row.Discord);
			return true;
		}
		if (m_BtnOpen && w == m_BtnOpen.Hit)
		{
			ClosePop();
			m_Menu.GoLater("clan");
			return true;
		}
		if (m_BtnApply && w == m_BtnApply.Hit)
		{
			OpenApply();
			return true;
		}
		if (m_BtnRevoke && w == m_BtnRevoke.Hit)
		{
			client.Request(VB_TabletOp.CLAN_APPLY_CANCEL);
			INT_UIF_Toast.Info(VBT.T("#STR_VB_TAB_A_APPLY_REVOKED"));
			return true;
		}
		if (m_BtnBack && w == m_BtnBack.Hit)
		{
			OpenClan(m_ClanId);
			return true;
		}
		if (m_BtnSend && w == m_BtnSend.Hit)
		{
			SendApply();
			return true;
		}
		return true;
	}

	override bool OnHit(Widget w)
	{
		if (m_Pop != POP_NONE && m_Modal && m_Modal.IsOpen())
			return OnPopHit(w);
		if (m_ClanHits)
		{
			int i = m_ClanHits.Find(w);
			if (i >= 0)
			{
				OpenClan(m_ClanIds.Get(i));
				return true;
			}
			int p = m_PlayerHits.Find(w);
			if (p >= 0)
			{
				OnPlayerHit(m_PlayerRows.Get(p));
				return true;
			}
		}
		if (m_BtnWeek && w == m_BtnWeek.Hit)
		{
			s_Mode = 0;
			m_Menu.RequestRepaint();
			return true;
		}
		if (m_BtnAll && w == m_BtnAll.Hit)
		{
			s_Mode = 1;
			m_Menu.RequestRepaint();
			return true;
		}
		if (m_BtnCreate && w == m_BtnCreate.Hit)
		{
			m_Menu.GoLater("create");
			return true;
		}
		return super.OnHit(w);
	}

	protected void OnPlayerHit(int index)
	{
		VB_TabletClient session = VB_TabletClient.Get();
		if (index < 0)
		{
			OpenPlayer(session.Me.PlayerName, session.Me.Xp(session.Rates), MyTag(session), session.Me.Place, true);
			return;
		}
		string name = session.RatingNames.Get(index);
		OpenPlayer(name, session.RatingXp.Get(index), session.RatingTags.Get(index), index + 1, name == session.Me.PlayerName);
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
		if (HoverOne(w, enter, m_BtnWeek) || HoverOne(w, enter, m_BtnAll) || HoverOne(w, enter, m_BtnCreate))
			return true;
		if (HoverOne(w, enter, m_BtnOpen) || HoverOne(w, enter, m_BtnApply) || HoverOne(w, enter, m_BtnRevoke))
			return true;
		if (HoverOne(w, enter, m_BtnBack) || HoverOne(w, enter, m_BtnSend))
			return true;
		if (m_BtnCopy && w == m_BtnCopy.Hit)
		{
			m_BtnCopy.Hover(enter);
			return true;
		}
		if (m_ClanHits)
		{
			int fill = 0;
			if (enter)
				fill = VB_C_PANEL2;
			int i = m_ClanHits.Find(w);
			if (i >= 0)
			{
				INT_UIF_Surface cb = m_ClanBgs.Get(i);
				if (cb)
					cb.SetFill(fill);
				return true;
			}
			int p = m_PlayerHits.Find(w);
			if (p >= 0)
			{
				INT_UIF_Surface pb = m_PlayerBgs.Get(p);
				if (pb)
					pb.SetFill(fill);
				return true;
			}
		}
		return super.OnHover(w, enter);
	}
}
