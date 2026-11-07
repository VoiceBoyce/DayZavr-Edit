// Контракты: вкладки «Клановые» и «Личные». Прогресс считает сервер, клиент только показывает.
class VB_TabletPageContracts extends VB_TabletTabbedPage
{
	static int s_Tab;

	protected TextWidget m_Timer;
	protected float m_T = 1;
	protected ref array<ref VBT_Button> m_Buttons;
	protected ref array<int> m_ClaimAt;		// индекс контракта или -1
	protected ref array<int> m_SwapAt;
	protected ref VBT_Button m_BtnLevels;
	protected ref VBT_Modal m_Modal;

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
		VB_Clan c = client.Clan;
		if (!c)
		{
			m_Menu.GoLater("profile");
			return;
		}
		VBA_Util.Ask(VB_TabletOp.CONTRACTS);
		InitTabs(180);
		m_TabNames.Insert("#STR_VB_TAB_C_TAB_CLAN");
		m_TabNames.Insert("#STR_VB_TAB_C_TAB_MY");
		PaintTabs();
		m_Buttons = new array<ref VBT_Button>();
		m_ClaimAt = new array<int>();
		m_SwapAt = new array<int>();
		if (s_Tab == 1)
			BuildMine(c, client);
		else
			BuildClan(c, client);
		BuildContrib(c, client);
		m_T = 1;
		Tick(0);
	}

	// Левая часть полосы: отсчёт до нового набора. Возвращает полосу.
	protected Widget BuildHead(string cap)
	{
		Widget f = INT_UIF_Draw.Frame(m_Root, 0, 0, 1172, 112);
		INT_UIF_Surface.Box(f, 0, 0, 1172, 112, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(f, 1172, 112);
		INT_UIF_Surface.Pill(f, 20, 23, 10, 10, VB_C_ACCENT);
		VBT.Cap(f, 40, 19, 230, cap);
		m_Timer = INT_UIF_Draw.Text(f, 20, 46, 250, 48, "00:00:00", 38, VB_C_TEXT);
		INT_UIF_Draw.Line(f, 292, 18, 1, 76, VB_C_LINE);
		INT_UIF_Draw.Line(f, 540, 18, 1, 76, VB_C_LINE);
		INT_UIF_Draw.Line(f, 770, 18, 1, 76, VB_C_LINE);
		return f;
	}

	protected void BuildDoneBlock(Widget f, array<ref VB_ClanContract> list, string gainCap)
	{
		int done = 0;
		int gain = 0;
		int n = list.Count();
		int i;
		for (i = 0; i < n; i++)
		{
			VB_ClanContract k = list.Get(i);
			if (k.Stage == VB_ContractStage.DONE)
			{
				done++;
				gain += k.Reward;
			}
			int col = VB_C_PANEL3;
			if (k.Stage == VB_ContractStage.DONE)
				col = VB_C_ACCENT;
			else if (k.Stage == VB_ContractStage.READY)
				col = VB_C_GOLD;
			INT_UIF_Surface.Box(f, 312 + i * 32, 86, 26, 8, 2, col);
		}
		VBT.Cap(f, 312, 19, 220, "#STR_VB_TAB_C_DONE_TODAY");
		INT_UIF_Draw.Text(f, 312, 44, 120, 36, done.ToString() + "/" + n.ToString(), 28, VB_C_TEXT);
		VBT.Cap(f, 560, 19, 200, gainCap);
		VBT.Ico(f, 560, 52, 24, "sym_bolt", VB_C_INF);
		INT_UIF_Draw.Text(f, 592, 46, 160, 40, "+" + gain.ToString(), 30, VB_C_INF);
	}

	protected void BuildMine(VB_Clan c, VB_TabletClient client)
	{
		Widget f = BuildHead("#STR_VB_TAB_C_NEW_MY");
		BuildDoneBlock(f, client.MyContracts, "#STR_VB_TAB_C_GAIN_CLAN");
		string lvCap = VBA_Util.Sub("#STR_VB_TAB_C_CLAN_LEVEL", "{n}", c.Level.ToString());
		VBT.Cap(f, 790, 19, 240, lvCap);
		int li = Math.Clamp(c.Level, 1, VB_CLAN_LEVELS) - 1;
		INT_UIF_Draw.Text(f, 790, 40, 362, 30, "#STR_VB_TAB_C_MYINFO_" + li.ToString(), 20, VB_C_TEXT);
		string left = "0/1";
		int leftCol = VB_C_DIM2;
		if (client.MyReplace)
		{
			left = "1/1";
			leftCol = VB_C_DIM;
		}
		INT_UIF_Draw.Text(f, 790, 74, 362, 22, VBT.T("#STR_VB_TAB_C_SWAP_LEFT") + " " + left, 14, leftCol);
		int n = client.MyContracts.Count();
		for (int i = 0; i < n; i++)
			PaintCard(0, 128 + i * 200, client.MyContracts.Get(i), 844, true, i, client);
		Widget u = INT_UIF_Draw.Frame(m_Root, 856, 128, 316, 226);
		INT_UIF_Surface.Box(u, 0, 0, 316, 226, 10, VB_C_PANEL2);
		VBT.Corners(u, 316, 226, VB_C_ACCENT, 14);
		VBT.Cap(u, 20, 18, 276, "#STR_VB_TAB_C_REWARD");
		array<int> rewards = {10, 25, 60};
		for (int t = 0; t < 3; t++)
		{
			float y = 48 + t * 40;
			int value = rewards.Get(t);
			foreach (VB_ClanContract k : client.MyContracts)
			{
				if (k.Tier == t)
					value = k.Reward;
			}
			INT_UIF_Draw.Text(u, 20, y, 170, 34, "#STR_VB_TAB_C_TIER_" + t.ToString(), 16, VB_C_TEXT);
			INT_UIF_Draw.Text(u, 190, y, 106, 34, "+" + value.ToString(), 16, VB_C_INF, INT_UIF_Align.RIGHT);
		}
		INT_UIF_Draw.Text(u, 20, 172, 276, 40, "#STR_VB_TAB_C_REWARD_XP", 14, VB_C_ACCENT);
	}

	protected void BuildClan(VB_Clan c, VB_TabletClient client)
	{
		Widget f = BuildHead("#STR_VB_TAB_C_NEW_CLAN");
		BuildDoneBlock(f, client.Contracts, "#STR_VB_TAB_C_GAIN_DAY");
		array<int> levels = VB_TabletRates.Levels();
		int top = levels.Count();
		int next = c.NextLevelNeed();
		string lvCap = VBA_Util.Sub("#STR_VB_TAB_C_LEVEL", "{n}", c.Level.ToString());
		VBT.Cap(f, 790, 19, 200, lvCap);
		string inf = VBT.Num(c.Influence) + "/";
		inf = inf + VBT.Num(next);
		INT_UIF_Draw.Text(f, 930, 17, 222, 20, inf, 15, VB_C_INF, INT_UIF_Align.RIGHT);
		VBT.Ticks(f, 790, 48, 362, 16, 30, c.Influence / (Math.Max(1, next) + 0.0), VB_C_INF);
		string rest = "#STR_VB_TAB_C_LEVEL_MAX";
		if (c.Level < top)
		{
			int need = Math.Max(0, next - c.Influence);
			int up = c.Level + 1;
			rest = VBA_Util.Sub("#STR_VB_TAB_C_LEVEL_REST", "{n}", VBT.Num(need));
			rest.Replace("{l}", up.ToString());
		}
		INT_UIF_Draw.Text(f, 790, 74, 362, 22, rest, 14, VB_C_DIM);
		int n = client.Contracts.Count();
		for (int i = 0; i < n; i++)
		{
			int row = i / 2;
			int col = i % 2;
			PaintCard(col * 428, 128 + row * 200, client.Contracts.Get(i), 412, false, i, client);
		}
		Widget u = INT_UIF_Draw.Frame(m_Root, 856, 128, 316, 226);
		INT_UIF_Surface.Box(u, 0, 0, 316, 226, 10, VB_C_PANEL2);
		VBT.Corners(u, 316, 226, VB_C_ACCENT, 14);
		if (c.Level < top)
		{
			int open = c.Level + 1;
			VBT.Cap(u, 20, 18, 276, VBA_Util.Sub("#STR_VB_TAB_C_OPEN_AT", "{n}", open.ToString()));
			INT_UIF_Draw.MText(u, 20, 48, 276, 100, "#STR_VB_TAB_C_LVINFO_" + c.Level.ToString(), 20, VB_C_TEXT);
		}
		else
		{
			VBT.Cap(u, 20, 18, 276, lvCap);
			INT_UIF_Draw.MText(u, 20, 48, 276, 100, "#STR_VB_TAB_C_ALL_OPEN", 20, VB_C_TEXT);
		}
		m_BtnLevels = new VBT_Button(u, 20, 164, 276, 44, "#STR_VB_TAB_C_ALL_LEVELS", false, "rank", m_Menu.GetHitHandler());
	}

	protected void PaintCard(float x, float y, VB_ClanContract k, float w, bool my, int index, VB_TabletClient client)
	{
		float h = 192;
		float e = w - 20;
		Widget f = VBT.Card(m_Root, x, y, w, h);
		array<int> tierCol = {VB_C_DIM, VB_C_GOLD, VB_C_BAD};
		int color = tierCol.Get(k.Tier);
		bool ready = k.Stage == VB_ContractStage.READY;
		bool active = k.Stage == VB_ContractStage.ACTIVE;
		if (ready)
		{
			VBT.Glow(f, w, h);
			VBT.Corners(f, w, h, VB_C_ACCENT, 14);
		}
		if (k.Tier == 2)
			INT_UIF_Draw.Texture(f, 4, 1, w - 8, 6, VB_TABLET_TEX_HAZARD, INT_UIF_Color.WithAlpha(VB_C_BAD, 217));
		else if (k.Tier == 1)
			INT_UIF_Surface.Box(f, 0, 18, 3, 56, 2, color);
		INT_UIF_Surface.Box(f, 20, 20, 52, 52, 10, VB_C_PANEL2);
		int iconCol = VB_C_ACCENT;
		if (active)
		{
			iconCol = VB_C_DIM;
			if (k.Tier > 0)
				iconCol = color;
		}
		VBT.Ico(f, 33, 33, 26, k.Ico, iconCol);
		INT_UIF_Draw.Text(f, 88, 18, 210, 30, k.Title, 20, VB_C_TEXT);
		INT_UIF_Draw.Text(f, 88, 48, 300, 24, k.Hint, 15, VB_C_DIM);
		string tierName = "#STR_VB_TAB_C_TIERU_" + k.Tier.ToString();
		float tw0 = INT_UIF_Draw.TextWidth(tierName, 12) + 20;
		INT_UIF_Surface.Box(f, e - tw0, 22, tw0, 22, 10, 0, color);
		INT_UIF_Draw.Text(f, e - tw0, 22, tw0, 22, tierName, 12, color, INT_UIF_Align.CENTER);
		int need = Math.Max(1, k.Need);
		int p = Math.Round(k.Have * 100.0 / need);
		INT_UIF_Draw.Text(f, 20, 90, 240, 22, k.Have.ToString() + "/" + k.Need.ToString(), 16, VB_C_TEXT);
		INT_UIF_Draw.Text(f, e - 100, 90, 100, 22, p.ToString() + "%", 15, VB_C_DIM, INT_UIF_Align.RIGHT);
		int barCol = VB_C_OK;
		if (active)
			barCol = VB_C_ACCENT;
		int cells = Math.Round((e - 20) / 10.3);
		VBT.Ticks(f, 20, 116, e - 20, 12, cells, k.Have / (need + 0.0), barCol);
		VBT.Ico(f, 20, 150, 18, "sym_bolt", VB_C_INF);
		string gain = "+" + k.Reward.ToString();
		float tw = INT_UIF_Draw.TextWidth(gain, 16) + 8;
		INT_UIF_Draw.Text(f, 44, 144, tw, 30, gain, 16, VB_C_INF);
		if (k.Cash > 0)
			VBK.Money(f, 44 + tw + 18, 149, k.Cash, 16);
		if (my)
		{
			string xt = "+" + k.Xp.ToString() + " " + VBT.T("#STR_VB_TAB_C_XP");
			float x0 = 44 + tw + 18;
			VBT.Ico(f, x0, 150, 18, "rank", VB_C_ACCENT);
			INT_UIF_Draw.Text(f, x0 + 24, 144, INT_UIF_Draw.TextWidth(xt, 16) + 8, 30, xt, 16, VB_C_ACCENT);
		}
		if (ready)
		{
			m_Buttons.Insert(new VBT_Button(f, e - 120, 140, 120, 38, "#STR_VB_TAB_C_CLAIM", true, "", m_Menu.GetHitHandler(), 14));
			m_ClaimAt.Insert(index);
			m_SwapAt.Insert(-1);
		}
		else if (k.Stage == VB_ContractStage.DONE)
		{
			float dw = INT_UIF_Draw.TextWidth("#STR_VB_TAB_C_DONE", 14) + 4;
			VBT.Ico(f, e - dw - 22, 151, 16, "check", VB_C_OK);
			INT_UIF_Draw.Text(f, e - dw, 144, dw, 30, "#STR_VB_TAB_C_DONE", 14, VB_C_OK);
			// В макете выполненная карточка приглушена. Прозрачность родителя у движка не наследуется.
			INT_UIF_Surface.Box(f, 0, 0, w, h, 10, INT_UIF_Color.WithAlpha(VB_C_BG, 97));
		}
		else if (my && client.MyReplace)
		{
			m_Buttons.Insert(new VBT_Button(f, e - 120, 140, 120, 38, "#STR_VB_TAB_C_SWAP", false, "refresh", m_Menu.GetHitHandler(), 14));
			m_ClaimAt.Insert(-1);
			m_SwapAt.Insert(index);
		}
	}

	protected void BuildContrib(VB_Clan c, VB_TabletClient client)
	{
		Widget t = VBT.Card(m_Root, 856, 370, 316, 350, "#STR_VB_TAB_C_CONTRIB");
		array<VB_ClanMember> list = new array<VB_ClanMember>();
		foreach (VB_ClanMember m : c.Members)
		{
			int at = list.Count();
			for (int i = 0; i < list.Count(); i++)
			{
				if (m.Today > list.Get(i).Today)
				{
					at = i;
					break;
				}
			}
			list.InsertAt(m, at);
		}
		if (list.Count() == 0)
			return;
		int top = Math.Max(1, list.Get(0).Today);
		string own = client.MyUid();
		int n = Math.Min(5, list.Count());
		for (int k = 0; k < n; k++)
		{
			VB_ClanMember row = list.Get(k);
			float y = 50 + k * 58;
			int ink = VB_C_TEXT;
			if (row.Uid == own)
				ink = VB_C_ACCENT;
			string val = "0";
			int valCol = VB_C_DIM2;
			if (row.Today > 0)
			{
				val = "+" + row.Today.ToString();
				valCol = VB_C_INF;
			}
			INT_UIF_Draw.Text(t, 20, y, 190, 24, VBT.Fit(row.PlayerName, 190, 16), 16, ink);
			INT_UIF_Draw.Text(t, 210, y, 86, 24, val, 15, valCol, INT_UIF_Align.RIGHT);
			VBT.Ticks(t, 20, y + 30, 276, 10, 30, row.Today / (top + 0.0), VB_C_INF);
		}
	}

	// Все уровни: порог влияния и что открывается.
	protected void OpenLevels()
	{
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (!c)
			return;
		float w = 660;
		float h = 444;
		m_Modal = new VBT_Modal(m_Menu, w, h);
		Widget b = m_Modal.Body;
		VBT.Corners(b, w, h, VB_C_ACCENT, 18);
		INT_UIF_Draw.Text(b, 28, 22, w - 120, 30, "#STR_VB_TAB_C_LEVELS", 22, VB_C_TEXT);
		array<int> levels = VB_TabletRates.Levels();
		for (int i = 0; i < levels.Count(); i++)
		{
			int n = i + 1;
			float y = 68 + i * 70;
			bool has = c.Level >= n;
			bool cur = c.Level == n;
			int fill = VB_C_PANEL2;
			int border = 0;
			if (cur)
			{
				fill = VB_C_PANEL3;
				border = VB_C_ACCENT;
			}
			int numCol = VB_C_DIM2;
			int ink = VB_C_DIM;
			int markCol = VB_C_DIM2;
			string mark = "lock";
			if (has)
			{
				numCol = VB_C_ACCENT;
				ink = VB_C_TEXT;
				markCol = VB_C_OK;
				mark = "check";
			}
			int need = levels.Get(i);
			string cond = "#STR_VB_TAB_C_LEVEL_START";
			if (need > 0)
				cond = VBT.Num(need) + " " + VBT.T("#STR_VB_TAB_C_INFLUENCE");
			INT_UIF_Surface.Box(b, 20, y, w - 40, 62, 8, fill, border);
			INT_UIF_Draw.Text(b, 36, y, 44, 62, n.ToString(), 24, numCol, INT_UIF_Align.CENTER);
			INT_UIF_Draw.Text(b, 92, y + 8, 430, 24, "#STR_VB_TAB_C_LVINFO_" + i.ToString(), 16, ink);
			INT_UIF_Draw.Text(b, 92, y + 34, 430, 20, cond, 13, VB_C_DIM2);
			VBT.Ico(b, w - 58, y + 21, 20, mark, markCol);
		}
	}

	override void Tick(float dt)
	{
		m_T = m_T + dt;
		if (m_T < 1)
			return;
		m_T = 0;
		if (!m_Timer)
			return;
		int left = VB_TabletClient.Get().NextDayLeft();
		int hh = left / 3600;
		int mm = (left % 3600) / 60;
		int ss = left % 60;
		string a;
		string b;
		string c;
		VBT.Pad2(a, hh);
		VBT.Pad2(b, mm);
		VBT.Pad2(c, ss);
		m_Timer.SetText(a + ":" + b + ":" + c);
	}

	override bool OnData()
	{
		return m_Modal && m_Modal.IsOpen();
	}

	override void Leave()
	{
		super.Leave();
		if (m_Modal)
			m_Modal.Close();
		m_Modal = null;
	}

	protected void PostIndex(int op, int index)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		ScriptRPC rpc = client.Begin(op);
		rpc.Write(index);
		client.Post(rpc);
	}

	override bool OnHit(Widget w)
	{
		if (!m_Buttons)
			return false;
		if (m_Modal && m_Modal.IsOpen())
		{
			if (m_Modal.IsDismiss(w))
			{
				m_Modal.Close();
				m_Menu.RequestRepaint();
			}
			return true;
		}
		if (super.OnHit(w))
			return true;
		if (m_BtnLevels && w == m_BtnLevels.Hit)
		{
			OpenLevels();
			return true;
		}
		for (int i = 0; i < m_Buttons.Count(); i++)
		{
			if (w != m_Buttons.Get(i).Hit)
				continue;
			int claim = m_ClaimAt.Get(i);
			if (claim >= 0)
				PostIndex(VB_TabletOp.CONTRACT_CLAIM, claim);
			else
				PostIndex(VB_TabletOp.CONTRACT_REPLACE, m_SwapAt.Get(i));
			return true;
		}
		return false;
	}

	override bool OnHover(Widget w, bool enter)
	{
		if (!m_Buttons)
			return false;
		if (super.OnHover(w, enter))
			return true;
		if (m_BtnLevels && w == m_BtnLevels.Hit)
		{
			m_BtnLevels.Hover(enter);
			return true;
		}
		foreach (VBT_Button b : m_Buttons)
		{
			if (w == b.Hit)
			{
				b.Hover(enter);
				return true;
			}
		}
		return false;
	}
}
