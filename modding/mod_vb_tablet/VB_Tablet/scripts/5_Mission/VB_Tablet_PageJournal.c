// Журнал клана: шкала активности за 7 дней, четыре типа событий, лента по дням.

// Текст и оформление записи. Строки собираются здесь, чтобы сводка клана и журнал писали одинаково.
class VBA_Journal
{
	// Тип записи: 0 участники, 1 контракты, 2 склад, 3 клан.
	static int KindOf(int ev)
	{
		switch (ev)
		{
			case VB_ClanEvent.JOINED:
			case VB_ClanEvent.LEFT:
			case VB_ClanEvent.KICKED:
			case VB_ClanEvent.RANK:
			case VB_ClanEvent.LEADER:
				return 0;
			case VB_ClanEvent.CONTRACT:
				return 1;
			case VB_ClanEvent.DEPOSIT:
			case VB_ClanEvent.WITHDRAW:
			case VB_ClanEvent.FROM_ACCOUNT:
			case VB_ClanEvent.TO_ACCOUNT:
			case VB_ClanEvent.TAX:
			case VB_ClanEvent.ATM_SET:
			case VB_ClanEvent.ATM_MOVE:
				return 2;
		}
		return 3;
	}

	static string IcoOf(int ev)
	{
		array<string> all = {"flag", "users", "logout", "kick", "rank", "star", "news", "sym_cog", "pin", "coin", "coin", "coin", "check", "coin", "coin", "rank", "flag", "coin", "coin"};
		if (ev < 0 || ev >= all.Count())
			return "flag";
		return all.Get(ev);
	}

	static int KindColor(int kind)
	{
		array<int> all = {VB_C_ACCENT, VB_C_INF, VB_C_GOLD, VB_C_SILVER};
		return all.Get(kind);
	}

	static string KindIcon(int kind)
	{
		array<string> all = {"users", "contract", "vault", "flag"};
		return all.Get(kind);
	}

	// Подстановка в шаблон STR_VB_TAB_J_<номер>.
	static string Text(VB_ClanJournalRow row)
	{
		string s = VBT.T("#STR_VB_TAB_J_" + row.Kind.ToString());
		string arg = VBA_Util.Title(row.Arg);
		s.Replace("{who}", row.Who);
		s.Replace("{arg}", arg);
		s.Replace("{val}", row.Val);
		return s;
	}

	// Сумма справа: числа с разрядами и настоящим минусом, остальное как есть.
	static string ValText(VB_ClanJournalRow row)
	{
		string v = row.Val;
		if (v == "")
			return "";
		string sign = "";
		string digits = v;
		string first = v.Substring(0, 1);
		if (first == "-" || first == "+")
		{
			sign = first;
			digits = v.Substring(1, v.Length() - 1);
		}
		int num = digits.ToInt();
		if (num.ToString() != digits)
			return v;
		if (sign == "-")
			sign = "−";
		return sign + VBT.Num(num);
	}

	static bool IsNegative(VB_ClanJournalRow row)
	{
		return row.Val.IndexOf("-") == 0 || row.Val.IndexOf("−") == 0;
	}

	// Строка для сводки клана: текст и сумма через запятую.
	static string Full(VB_ClanJournalRow row)
	{
		string line = Text(row);
		string val = ValText(row);
		if (val != "")
			line = line + ", " + val;
		return line;
	}
}

class VB_TabletPageJournal extends VB_TabletPage
{
	protected Widget m_Top;
	protected ScrollWidget m_Scroll;
	protected Widget m_List;
	protected int m_Kind = -1;
	protected int m_Day = -1;
	protected bool m_Need;
	protected ref array<Widget> m_DayHits;
	protected ref array<ref INT_UIF_Surface> m_DayBg;
	protected ref array<Widget> m_KindHits;
	protected ref array<ref INT_UIF_Surface> m_KindBg;
	protected ref array<string> m_Keys;

	override void Build()
	{
		if (!VB_TabletClient.Get().Clan)
		{
			m_Menu.GoLater("create");
			return;
		}
		VBA_Util.Ask(VB_TabletOp.CLAN_JOURNAL);
		m_DayHits = new array<Widget>();
		m_DayBg = new array<ref INT_UIF_Surface>();
		m_KindHits = new array<Widget>();
		m_KindBg = new array<ref INT_UIF_Surface>();
		m_Top = INT_UIF_Draw.Frame(m_Root, 0, 0, 1172, 152);
		Widget t = VBT.Card(m_Root, 0, 168, 1172, 552);
		m_Scroll = VBK.Scroll(t, 0, 1, 1172, 550);
		m_List = INT_UIF_Draw.ScrollContent(m_Scroll);
		Paint();
	}

	protected void Paint()
	{
		m_Keys = VBA_Util.DayKeys();
		PaintTop();
		PaintList();
	}

	protected bool ByKind(VB_ClanJournalRow row)
	{
		return m_Kind < 0 || VBA_Journal.KindOf(row.Kind) == m_Kind;
	}

	protected bool ByDay(VB_ClanJournalRow row)
	{
		return m_Day < 0 || m_Keys.Find(row.Day) == m_Day;
	}

	protected void PaintTop()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		Widget f = m_Top;
		INT_UIF_Widgets.ClearChildren(f);
		m_DayHits.Clear();
		m_DayBg.Clear();
		m_KindHits.Clear();
		m_KindBg.Clear();
		INT_UIF_Surface.Box(f, 0, 0, 1172, 152, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(f, 1172, 152);
		VBT.Cap(f, 24, 16, 300, "#STR_VB_TAB_A_WEEK_ACTIVITY");

		// Сколько записей по дням и типам: день * 4 + тип.
		array<int> counts = new array<int>();
		array<int> perDay = {0, 0, 0, 0, 0, 0, 0};
		array<int> perKind = {0, 0, 0, 0};
		int i;
		for (i = 0; i < 28; i++)
			counts.Insert(0);
		foreach (VB_ClanJournalRow row : client.Journal)
		{
			int kind = VBA_Journal.KindOf(row.Kind);
			int day = m_Keys.Find(row.Day);
			if (day >= 0 && ByKind(row))
			{
				int cell = day * 4 + kind;
				counts.Set(cell, counts.Get(cell) + 1);
				perDay.Set(day, perDay.Get(day) + 1);
			}
			if (ByDay(row))
				perKind.Set(kind, perKind.Get(kind) + 1);
		}
		int mx = 8;
		for (i = 0; i < 7; i++)
			mx = Math.Max(mx, perDay.Get(i));

		for (int n = 0; n < 7; n++)
		{
			int back = 6 - n;
			float x = 24 + n * 82;
			bool on = m_Day == back;
			int fill = 0;
			int border = 0;
			if (on)
			{
				fill = VB_C_PANEL3;
				border = VB_C_ACCENT;
			}
			m_DayBg.Insert(INT_UIF_Surface.Box(f, x - 6, 38, 76, 106, 6, fill, border));
			array<int> seg = new array<int>();
			for (int k = 0; k < 4; k++)
			{
				int amount = counts.Get(back * 4 + k);
				int kc = VBA_Journal.KindColor(k);
				for (int a = 0; a < amount; a++)
					seg.Insert(kc);
			}
			for (int b = 0; b < 8; b++)
			{
				int idx = b * mx / 8;
				int col = VB_C_PANEL3;
				if (idx < seg.Count())
					col = seg.Get(idx);
				INT_UIF_Draw.Line(f, x, 110 - b * 9, 64, 6, col);
			}
			int d;
			int mo;
			int wd;
			VBA_Util.Back(back, d, mo, wd);
			string lbl = VBT.T("#STR_VB_TAB_A_WD_" + wd.ToString()) + " " + d.ToString();
			int lc = VB_C_ACCENT;
			if (back > 0)
				lc = VB_C_DIM;
			INT_UIF_Draw.Text(f, x - 6, 120, 76, 20, lbl, 13, lc, INT_UIF_Align.CENTER);
			m_DayHits.Insert(HitAt(f, x - 6, 38, 76, 106));
		}
		INT_UIF_Draw.Line(f, 606, 18, 1, 116, VB_C_LINE);
		for (int t = 0; t < 4; t++)
		{
			int col2 = t % 2;
			int row2 = t / 2;
			float kx = 624 + col2 * 268;
			float ky = 16 + row2 * 64;
			bool kon = m_Kind == t;
			int tint = VBA_Journal.KindColor(t);
			int kfill = VB_C_PANEL2;
			int kborder = 0;
			int kink = VB_C_DIM;
			if (kon)
			{
				kfill = VB_C_PANEL3;
				kborder = tint;
				kink = VB_C_TEXT;
			}
			m_KindBg.Insert(INT_UIF_Surface.Box(f, kx, ky, 260, 56, 8, kfill, kborder));
			INT_UIF_Surface.Box(f, kx + 10, ky + 14, 3, 28, 2, tint);
			VBT.Ico(f, kx + 24, ky + 17, 22, VBA_Journal.KindIcon(t), tint);
			INT_UIF_Draw.Text(f, kx + 58, ky, 130, 56, "#STR_VB_TAB_A_JKIND_" + t.ToString(), 16, kink);
			int total = perKind.Get(t);
			int nc = VB_C_DIM2;
			if (total > 0)
				nc = VB_C_TEXT;
			INT_UIF_Draw.Text(f, kx + 190, ky, 56, 56, total.ToString(), 18, nc, INT_UIF_Align.RIGHT);
			m_KindHits.Insert(HitAt(f, kx, ky, 260, 56));
		}
	}

	protected void PaintList()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		Widget L = m_List;
		INT_UIF_Widgets.ClearChildren(L);
		array<VB_ClanJournalRow> rows = new array<VB_ClanJournalRow>();
		foreach (VB_ClanJournalRow row : client.Journal)
		{
			if (ByKind(row) && ByDay(row))
				rows.Insert(row);
		}
		int total = rows.Count();
		if (total == 0)
		{
			INT_UIF_Draw.SetScrollContent(m_Scroll, 550);
			VBK.ScrollTo(m_Scroll, 0);
			INT_UIF_Draw.Text(L, 0, 250, 1172, 30, "#STR_VB_TAB_A_NO_RECORDS", 17, VB_C_DIM2, INT_UIF_Align.CENTER);
			return;
		}
		float y = 14;
		int from = 0;
		while (from < total)
		{
			string day = rows.Get(from).Day;
			int to = from;
			while (to + 1 < total && rows.Get(to + 1).Day == day)
				to++;
			int count = to - from + 1;
			PaintGroupDay(L, y, day);
			if (count > 1)
				INT_UIF_Draw.Line(L, 219, y + 24, 2, (count - 1) * 52, VB_C_LINE2);
			for (int k = 0; k < count; k++)
				PaintRow(L, y + k * 52, rows.Get(from + k));
			float h = Math.Max(92, count * 52);
			y = y + h + 6;
			from = to + 1;
			if (from < total)
			{
				INT_UIF_Draw.Line(L, 24, y, 1106, 1, VB_C_LINE);
				y = y + 8;
			}
		}
		INT_UIF_Draw.SetScrollContent(m_Scroll, y + 12);
		VBK.ScrollTo(m_Scroll, 0);
	}

	// Левая колонка группы: число и день недели, старше недели просто дата из записи.
	protected void PaintGroupDay(Widget L, float y, string day)
	{
		int back = m_Keys.Find(day);
		if (back < 0)
		{
			INT_UIF_Draw.Text(L, 24, y + 6, 110, 40, day, 16, VB_C_DIM);
			return;
		}
		int d;
		int mo;
		int wd;
		VBA_Util.Back(back, d, mo, wd);
		int numCol = VB_C_DIM;
		int capCol = VB_C_DIM2;
		string cap = "#STR_VB_TAB_A_WDF_" + wd.ToString();
		if (back == 0)
		{
			numCol = VB_C_TEXT;
			capCol = VB_C_ACCENT;
			cap = "#STR_VB_TAB_A_TODAY_C";
		}
		else if (back == 1)
		{
			cap = "#STR_VB_TAB_A_YESTERDAY_C";
		}
		INT_UIF_Draw.Text(L, 24, y + 6, 100, 46, d.ToString(), 36, numCol, INT_UIF_Align.LEFT, true);
		VBT.Cap(L, 24, y + 54, 110, cap, capCol);
	}

	protected void PaintRow(Widget L, float y, VB_ClanJournalRow row)
	{
		int kind = VBA_Journal.KindOf(row.Kind);
		int tint = VBA_Journal.KindColor(kind);
		int col = tint;
		if (row.Kind == VB_ClanEvent.KICKED)
			col = VB_C_BAD;
		string val = VBA_Journal.ValText(row);
		float free = 876;
		if (val != "")
			free = 724;
		INT_UIF_Draw.Text(L, 130, y, 66, 48, row.Time, 14, VB_C_DIM2, INT_UIF_Align.RIGHT);
		INT_UIF_Surface.Pill(L, 204, y + 8, 32, 32, VB_C_PANEL2, col);
		VBT.Ico(L, 212, y + 16, 16, VBA_Journal.IcoOf(row.Kind), col);
		string text = VBA_Journal.Text(row);
		string who = row.Who;
		if (who != "" && text.IndexOf(who) == 0)
		{
			float nw = INT_UIF_Draw.TextWidth(who, 17) + 6;
			string rest = text.Substring(who.Length(), text.Length() - who.Length());
			rest.TrimInPlace();
			rest = VBT.Fit(rest, free - nw, 16);
			INT_UIF_Draw.Text(L, 252, y, nw, 48, who, 17, VB_C_TEXT);
			INT_UIF_Draw.Text(L, 252 + nw, y, free - nw, 48, rest, 16, VB_C_DIM);
		}
		else
		{
			text = VBT.Fit(text, free, 16);
			INT_UIF_Draw.Text(L, 252, y, free, 48, text, 16, VB_C_TEXT);
		}
		if (val != "")
		{
			int valCol = tint;
			if (VBA_Journal.IsNegative(row))
				valCol = VB_C_BAD;
			INT_UIF_Draw.Text(L, 980, y, 150, 48, val, 19, valCol, INT_UIF_Align.RIGHT);
		}
	}

	override bool OnData()
	{
		if (!VB_TabletClient.Get().Clan)
		{
			m_Menu.GoLater("profile");
			return true;
		}
		if (!m_Top)
			return false;
		Paint();
		return true;
	}

	// Перерисовка после клика: кликнутый виджет ещё жив, поэтому в следующем кадре.
	override void Tick(float dt)
	{
		if (!m_Need)
			return;
		m_Need = false;
		Paint();
	}

	override bool OnHit(Widget w)
	{
		if (!m_DayHits)
			return false;
		int i = m_DayHits.Find(w);
		if (i >= 0)
		{
			int back = 6 - i;
			if (m_Day == back)
				m_Day = -1;
			else
				m_Day = back;
			m_Need = true;
			return true;
		}
		int k = m_KindHits.Find(w);
		if (k >= 0)
		{
			if (m_Kind == k)
				m_Kind = -1;
			else
				m_Kind = k;
			m_Need = true;
			return true;
		}
		return false;
	}

	override bool OnHover(Widget w, bool enter)
	{
		if (!m_DayHits)
			return false;
		int i = m_DayHits.Find(w);
		if (i >= 0)
		{
			if (6 - i != m_Day)
			{
				int fill = 0;
				if (enter)
					fill = VB_C_PANEL2;
				m_DayBg.Get(i).SetFill(fill);
			}
			return true;
		}
		int k = m_KindHits.Find(w);
		if (k >= 0)
		{
			if (k != m_Kind)
			{
				int kfill = VB_C_PANEL2;
				if (enter)
					kfill = VB_C_PANEL3;
				m_KindBg.Get(k).SetFill(kfill);
			}
			return true;
		}
		return false;
	}
}
