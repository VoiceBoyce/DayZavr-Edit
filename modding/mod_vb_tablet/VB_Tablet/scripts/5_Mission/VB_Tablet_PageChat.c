// Чат клана: пишут все участники, сервер хранит последние 100 сообщений и рассылает список всем в сети.
class VB_TabletPageChat extends VB_TabletPage
{
	protected static const int MSG_MAX = 200;	// как ChatSend на сервере
	protected static const float MSG_GAP = 2;	// как CHAT_GAP_SEC на сервере

	protected static float s_LastSend = -100;

	protected ScrollWidget m_List;
	protected Widget m_Rows;
	protected ScrollWidget m_SideList;
	protected Widget m_Side;
	protected TextWidget m_Count;
	protected ref VBT_Input m_Input;
	protected ref VBT_Button m_Send;
	protected string m_LastText;

	// Что уже нарисовано: новые сообщения дописываются ниже.
	protected int m_Shown;
	protected string m_ShownSig;
	protected float m_ListY;
	protected string m_PrevWho;
	protected string m_PrevDay;
	protected int m_ScrollFrames;

	protected string m_SideSig;
	protected ref array<Widget> m_SideHits = new array<Widget>();
	protected ref array<string> m_SideUids = new array<string>();
	protected ref array<ref INT_UIF_Surface> m_SideBg = new array<ref INT_UIF_Surface>();

	override void Build()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (!client.Clan)
		{
			m_Menu.GoLater("profile");
			return;
		}
		client.Request(VB_TabletOp.CLAN_CHAT);
		Widget t = VBT.Card(m_Root, 0, 0, 856, 720);
		m_List = INT_UIF_Draw.Scroll(t, 0, 1, 856, 630);
		m_Rows = INT_UIF_Draw.ScrollContent(m_List);
		INT_UIF_Draw.Line(t, 0, 632, 856, 1, VB_C_LINE);
		m_Input = new VBT_Input(t, 20, 652, 600, 48, "#STR_VB_TAB_B_CHAT_HINT", MSG_MAX);
		m_Count = INT_UIF_Draw.Text(t, 626, 652, 58, 48, "", 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
		ShowCount("");
		m_Send = new VBT_Button(t, 692, 652, 144, 48, "#STR_VB_TAB_B_CHAT_SEND", true, "send", m_Menu.GetHitHandler(), 14);
		m_Side = INT_UIF_Draw.Frame(m_Root, 872, 0, 300, 720);
		PaintSide();
		SyncList();
	}

	// --- сообщения -----------------------------------------------------------------------------
	protected static string Sig(VB_ClanChatRow r)
	{
		return r.Who + "|" + r.Day + "|" + r.Time + "|" + r.Msg;
	}

	protected void ResetList()
	{
		INT_UIF_Widgets.ClearChildren(m_Rows);
		m_Shown = 0;
		m_ShownSig = "";
		m_ListY = 16;
		m_PrevWho = "";
		m_PrevDay = "";
	}

	// Дописывает новые сообщения. Если сервер срезал старые, список рисуется заново.
	protected void SyncList()
	{
		array<ref VB_ClanChatRow> chat = VB_TabletClient.Get().Chat;
		int n = chat.Count();
		if (n == 0)
		{
			ResetList();
			INT_UIF_Draw.SetScrollContent(m_List, 630);
			INT_UIF_Draw.Text(m_Rows, 0, 296, 856, 30, "#STR_VB_TAB_B_CHAT_NONE", 17, VB_C_DIM2, INT_UIF_Align.CENTER);
			return;
		}
		bool kept = m_Shown > 0 && n >= m_Shown && Sig(chat.Get(m_Shown - 1)) == m_ShownSig;
		if (kept && n == m_Shown)
			return;
		if (!kept)
			ResetList();
		for (int i = m_Shown; i < n; i++)
			AddRow(chat.Get(i));
		m_Shown = n;
		m_ShownSig = Sig(chat.Get(n - 1));
		INT_UIF_Draw.SetScrollContent(m_List, m_ListY + 10);
		// Высота содержимого применяется на следующем кадре: прокрутка вниз идёт из Tick.
		m_ScrollFrames = 3;
	}

	protected int NameColor(string who)
	{
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (c)
		{
			foreach (VB_ClanMember m : c.Members)
			{
				if (m.PlayerName == who)
					return c.Rank(m.RankId).Color;
			}
		}
		return VB_C_DIM;
	}

	protected void AddRow(VB_ClanChatRow r)
	{
		float y = m_ListY;
		if (r.Day != m_PrevDay)
		{
			m_PrevDay = r.Day;
			m_PrevWho = "";
			INT_UIF_Draw.Text(m_Rows, 0, y, 848, 20, VBB.DayLabel(r.Day), 13, VB_C_DIM2, INT_UIF_Align.CENTER);
			y += 32;
		}
		bool own = r.Who == VBB.MyName();
		array<string> lines = new array<string>();
		VBB.Wrap(r.Msg, 520, 15, lines);
		float tw = 0;
		foreach (string part : lines)
			tw = Math.Max(tw, INT_UIF_Draw.TextWidth(part, 15));
		// Ширина текста оценочная, поэтому пузырь с запасом 6%.
		tw = Math.Ceil(tw * 1.06);
		float bw = tw + 30;
		int count = lines.Count();
		float bh = count * 22 + 14;
		float x = 20;
		if (own)
			x = 828 - bw;
		if (r.Who != m_PrevWho)
		{
			if (m_PrevWho != "")
				y += 10;
			if (own)
				INT_UIF_Draw.Text(m_Rows, 628, y, 200, 20, r.Time, 12, VB_C_DIM2, INT_UIF_Align.RIGHT);
			else
			{
				float nw = INT_UIF_Draw.TextWidth(r.Who, 14) + 6;
				INT_UIF_Draw.Text(m_Rows, 20, y, nw, 20, r.Who, 14, NameColor(r.Who));
				INT_UIF_Draw.Text(m_Rows, 20 + nw + 6, y, 80, 20, r.Time, 12, VB_C_DIM2);
			}
			y += 22;
		}
		int fill = VB_C_PANEL2;
		int border = 0;
		if (own)
		{
			fill = VB_C_PANEL3;
			border = VB_C_ACCENT;
		}
		INT_UIF_Surface.Box(m_Rows, x, y, bw, bh, 10, fill, border);
		for (int i = 0; i < count; i++)
			INT_UIF_Draw.Text(m_Rows, x + 15, y + 7 + i * 22, tw + 4, 22, lines.Get(i), 15, VB_C_TEXT);
		m_ListY = y + bh + 6;
		m_PrevWho = r.Who;
	}

	protected void Send()
	{
		string v = m_Input.Get();
		if (v == "")
			return;
		float now = GetGame().GetTickTime();
		if (now - s_LastSend < MSG_GAP)
		{
			INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_B_CHAT_WAIT"));
			return;
		}
		s_LastSend = now;
		VB_TabletClient client = VB_TabletClient.Get();
		ScriptRPC rpc = client.Begin(VB_TabletOp.CHAT_SEND);
		rpc.Write(v);
		client.Post(rpc);
		m_LastText = v;
		m_Input.Set("");
		ShowCount("");
		m_Input.Focus();
	}

	protected void ShowCount(string s)
	{
		int len = s.LengthUtf8();
		int most = MSG_MAX;
		m_Count.SetText(len.ToString() + "/" + most.ToString());
	}

	// Сервер не принял сообщение: текст возвращается в поле, если там ещё пусто.
	override void OnResult(int code)
	{
		if (!m_Input || m_LastText == "")
			return;
		if (code != VB_TabletResult.RATE && code != VB_TabletResult.BANNED_WORD)
			return;
		if (m_Input.Get() == "")
		{
			m_Input.Set(m_LastText);
			ShowCount(m_LastText);
		}
	}

	// --- участники -----------------------------------------------------------------------------
	protected string SideSig(VB_Clan c)
	{
		string s = "";
		foreach (VB_ClanMember m : c.Members)
		{
			s = s + m.Uid + m.RankId;
			if (m.Online)
				s = s + "+";
		}
		foreach (VB_ClanRank r : c.Ranks)
			s = s + r.Title + r.Color.ToString();
		return s;
	}

	// Сначала кто в сети, внутри старшие по рангу.
	protected void PaintSide()
	{
		VB_Clan c = VB_TabletClient.Get().Clan;
		string sig = SideSig(c);
		if (sig == m_SideSig)
			return;
		m_SideSig = sig;
		INT_UIF_Widgets.ClearChildren(m_Side);
		m_SideHits.Clear();
		m_SideUids.Clear();
		m_SideBg.Clear();
		Widget t = VBT.Card(m_Side, 0, 0, 300, 720, "#STR_VB_TAB_B_MEMBERS");
		array<ref VB_ClanMember> sorted = new array<ref VB_ClanMember>();
		array<int> keys = new array<int>();
		int online = 0;
		foreach (VB_ClanMember m : c.Members)
		{
			int key = c.RankIndex(m.RankId);
			if (m.Online)
				online++;
			else
				key += 100;
			int at = 0;
			while (at < keys.Count() && keys.Get(at) <= key)
				at++;
			if (at == keys.Count())
			{
				keys.Insert(key);
				sorted.Insert(m);
			}
			else
			{
				keys.InsertAt(key, at);
				sorted.InsertAt(m, at);
			}
		}
		int total = sorted.Count();
		INT_UIF_Draw.Text(t, 160, 14, 120, 22, online.ToString() + "/" + total.ToString(), 15, VB_C_DIM, INT_UIF_Align.RIGHT);
		m_SideList = INT_UIF_Draw.Scroll(t, 0, 46, 300, 672);
		Widget l = INT_UIF_Draw.ScrollContent(m_SideList);
		INT_UIF_Draw.SetScrollContent(m_SideList, total * 44 + 6);
		for (int i = 0; i < total; i++)
		{
			VB_ClanMember who = sorted.Get(i);
			VB_ClanRank ra = c.Rank(who.RankId);
			float y = i * 44;
			m_SideBg.Insert(INT_UIF_Surface.Box(l, 8, y + 2, 276, 40, 6, 0));
			int dot = VB_C_DIM2;
			int ink = VB_C_DIM;
			if (who.Online)
			{
				dot = VB_C_OK;
				ink = VB_C_TEXT;
			}
			INT_UIF_Surface.Pill(l, 20, y + 18, 8, 8, dot);
			INT_UIF_Draw.Text(l, 38, y, 240, 28, VBT.Fit(who.PlayerName, 240, 15), 15, ink);
			string title = VBT.T(ra.Title);
			INT_UIF_Draw.Text(l, 38, y + 21, 240, 18, VBT.Fit(title, 240, 12), 12, ra.Color);
			m_SideHits.Insert(HitAt(l, 8, y + 2, 276, 40));
			m_SideUids.Insert(who.Uid);
		}
	}

	// --- события -------------------------------------------------------------------------------
	override bool OnData()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (!client.Clan)
		{
			m_Menu.GoLater("profile");
			return true;
		}
		PaintSide();
		SyncList();
		return true;
	}

	override void Tick(float dt)
	{
		if (m_ScrollFrames > 0 && m_List)
		{
			m_ScrollFrames--;
			m_List.VScrollToPos01(1);
		}
	}

	override void OnInput(VBT_Input field)
	{
		if (field == m_Input)
			ShowCount(m_Input.Edit.GetText());
	}

	override void OnSubmit(VBT_Input field)
	{
		if (field == m_Input)
			Send();
	}

	override bool OnHit(Widget w)
	{
		if (m_Send && w == m_Send.Hit)
		{
			Send();
			return true;
		}
		int i = m_SideHits.Find(w);
		if (i >= 0)
		{
			VB_TabletClient.Get().PickMember = m_SideUids.Get(i);
			m_Menu.GoLater("cmembers");
			return true;
		}
		return false;
	}

	override bool OnHover(Widget w, bool enter)
	{
		if (m_Send && w == m_Send.Hit)
		{
			m_Send.Hover(enter);
			return true;
		}
		int i = m_SideHits.Find(w);
		if (i >= 0)
		{
			int fill = 0;
			if (enter)
				fill = VB_C_PANEL2;
			m_SideBg.Get(i).SetFill(fill);
			return true;
		}
		return false;
	}
}
