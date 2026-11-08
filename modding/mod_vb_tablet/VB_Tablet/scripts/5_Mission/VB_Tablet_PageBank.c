// Склад денег: только просмотр. Пополнение, снятие и переводы со счёта идут в любом банкомате INT_Banking
// (аддон VB_Tablet_BANK, вкладка «Клан» в окне банкомата).
class VB_TabletPageBank extends VB_TabletPage
{
	protected TextWidget m_TaxT;
	protected float m_T = 1;

	override void Build()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c)
		{
			m_Menu.GoLater("profile");
			return;
		}
		VBA_Util.Ask(VB_TabletOp.CLAN_BANK);
		// Часы сервера для отсчёта до списания.
		if (!client.HasContracts)
			VBA_Util.Ask(VB_TabletOp.CONTRACTS);
		int days = -1;
		if (c.TaxFree <= 0)
			days = VB_TabletClient.Get().TaxDays(c.Bank);
		bool low = days >= 0 && days <= VBA_TAX_LOW;
		BuildBalance(c);
		BuildTax(c, days, low);
		BuildWeek(c);
		BuildHint();
		BuildHistory(client);
		m_T = 1;
		Tick(0);
	}

	protected void BuildBalance(VB_Clan c)
	{
		Widget a = INT_UIF_Draw.Frame(m_Root, 0, 0, 380, 236);
		INT_UIF_Surface.Box(a, 0, 0, 380, 236, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(a, 380, 236, VB_C_GOLD);
		VBT.Corners(a, 380, 236, VB_C_GOLD, 16);
		VBT.Cap(a, 28, 24, 300, "#STR_VB_TAB_C_ON_BANK");
		VBK.Money(a, 28, 54, c.Bank, 40);
		VBT.Cap(a, 28, 152, 140, "#STR_VB_TAB_C_FILL");
		string fill = VBT.Num(c.Bank) + "/";
		fill = fill + VBT.Num(VB_TabletClient.Get().BankMax);
		INT_UIF_Draw.Text(a, 150, 150, 202, 20, fill, 15, VB_C_DIM, INT_UIF_Align.RIGHT);
		VBT.Ticks(a, 28, 180, 324, 18, 30, c.Bank / (VB_TabletClient.Get().BankMax + 0.0), VB_C_GOLD);
	}

	protected void BuildTax(VB_Clan c, int days, bool low)
	{
		Widget x = INT_UIF_Draw.Frame(m_Root, 396, 0, 380, 236);
		int border = VB_C_LINE;
		int ink = VB_C_TEXT;
		int bar = VB_C_GOLD;
		int hint = VB_C_DIM2;
		if (low)
		{
			border = VB_C_BAD;
			ink = VB_C_BAD;
			bar = VB_C_BAD;
			hint = VB_C_BAD;
		}
		INT_UIF_Surface.Box(x, 0, 0, 380, 236, 10, VB_C_PANEL, border);
		VBT.Cap(x, 24, 24, 160, "#STR_VB_TAB_C_TAX");
		string perDay = VBA_Util.Sub("#STR_VB_TAB_C_TAX_DAY", "{n}", VBT.Num(VB_TabletClient.Get().ClanTax));
		INT_UIF_Draw.Text(x, 170, 22, 186, 20, perDay, 15, VB_C_GOLD, INT_UIF_Align.RIGHT);
		string head = VBA_Util.Sub("#STR_VB_TAB_C_TAX_LEFT", "{n}", days.ToString());
		float k = days / 30.0;
		if (days < 0)
		{
			head = VBA_Util.Sub("#STR_VB_TAB_C_TAX_FREE", "{n}", c.TaxFree.ToString());
			k = 1;
		}
		INT_UIF_Draw.Text(x, 24, 46, 332, 44, head, 28, ink);
		VBT.Ticks(x, 24, 98, 332, 14, 30, k, bar);
		INT_UIF_Draw.Text(x, 24, 118, 200, 28, "#STR_VB_TAB_C_TAX_NEXT", 14, VB_C_DIM);
		m_TaxT = INT_UIF_Draw.Text(x, 216, 118, 140, 28, "", 16, VB_C_TEXT, INT_UIF_Align.RIGHT);
		INT_UIF_Draw.Line(x, 24, 152, 332, 1, VB_C_LINE);
		INT_UIF_Draw.MText(x, 24, 160, 332, 66, "#STR_VB_TAB_C_TAX_TEXT", 14, hint);
	}

	protected void BuildWeek(VB_Clan c)
	{
		Widget w = VBT.Card(m_Root, 792, 0, 380, 236, "#STR_VB_TAB_C_WEEK");
		int top = 1;
		int net = 0;
		int i;
		for (i = 0; i < 7; i++)
		{
			int v = c.Week.Get(i);
			net += v;
			top = Math.Max(top, Math.AbsInt(v));
		}
		float axis = 146;
		int today = VBA_Util.Today();
		string sign = "+";
		int col = VB_C_GOLD;
		if (net < 0)
		{
			sign = "−";
			col = VB_C_BAD;
		}
		int total = Math.AbsInt(net);
		INT_UIF_Draw.Text(w, 200, 14, 160, 22, sign + VBT.Num(total), 18, col, INT_UIF_Align.RIGHT);
		INT_UIF_Draw.Line(w, 20, axis, 340, 1, VB_C_LINE2);
		for (i = 0; i < 7; i++)
		{
			float px = 24 + i * 48;
			int d = c.Week.Get(i);
			int dayCol = VB_C_DIM2;
			if (i == today)
				dayCol = VB_C_ACCENT;
			VBK.Column(w, px, axis, 36, Math.AbsInt(d) / (top + 0.0), d < 0);
			INT_UIF_Draw.Text(w, px, 204, 36, 18, "#STR_VB_TAB_C_WD_" + i.ToString(), 13, dayCol, INT_UIF_Align.CENTER);
		}
	}

	// Пополнение, снятие и переводы со счёта идут в банкоматах банка: здесь только подсказка.
	protected void BuildHint()
	{
		Widget ab = INT_UIF_Draw.Frame(m_Root, 0, 252, 1172, 60);
		INT_UIF_Surface.Box(ab, 0, 0, 1172, 60, 10, VB_C_PANEL, VB_C_LINE);
		INT_UIF_Surface.Box(ab, 16, 12, 36, 36, 8, VB_C_PANEL2);
		VBT.Ico(ab, 25, 21, 18, "vault", VB_C_GOLD);
		VBT.Cap(ab, 68, 12, 300, "#STR_VB_TAB_C_ATM_ANY");
		INT_UIF_Draw.Text(ab, 68, 28, 560, 24, "#STR_VB_TAB_C_ATM_HINT", 14, VB_C_TEXT);
		INT_UIF_Draw.Text(ab, 640, 18, 516, 24, "#STR_VB_TAB_C_ATM_HINT2", 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
	}

	protected string KindText(int kind)
	{
		if (kind == VB_ClanEvent.DEPOSIT)
			return "#STR_VB_TAB_C_OP_DEPOSIT";
		if (kind == VB_ClanEvent.WITHDRAW)
			return "#STR_VB_TAB_C_OP_WITHDRAW";
		if (kind == VB_ClanEvent.FROM_ACCOUNT)
			return "#STR_VB_TAB_C_OP_FROM_ACC";
		if (kind == VB_ClanEvent.TO_ACCOUNT)
			return "#STR_VB_TAB_C_OP_TO_ACC";
		if (kind == VB_ClanEvent.TAX)
			return "#STR_VB_TAB_C_OP_TAX";
		if (kind == VB_ClanEvent.CONTRACT)
			return "#STR_VB_TAB_C_OP_CONTRACT";
		if (kind == VB_ClanEvent.WEEK_PRIZE)
			return "#STR_VB_TAB_C_OP_PRIZE";
		return "#STR_VB_TAB_C_OP_SETTINGS";
	}

	protected void BuildHistory(VB_TabletClient client)
	{
		Widget h = VBT.Card(m_Root, 0, 324, 1172, 396);
		VBT.Cap(h, 70, 18, 240, "#STR_VB_TAB_C_H_WHO");
		VBT.Cap(h, 340, 18, 300, "#STR_VB_TAB_C_H_OP");
		VBT.Cap(h, 740, 18, 200, "#STR_VB_TAB_C_H_SUM");
		VBT.Cap(h, 980, 18, 170, "#STR_VB_TAB_C_H_WHEN");
		INT_UIF_Draw.Line(h, 0, 46, 1172, 1, VB_C_LINE);
		array<ref VB_ClanBankRow> rows = client.History;
		int n = rows.Count();
		if (n == 0)
		{
			INT_UIF_Draw.Text(h, 0, 190, 1172, 30, "#STR_VB_TAB_C_H_EMPTY", 17, VB_C_DIM2, INT_UIF_Align.CENTER);
			return;
		}
		ScrollWidget sw = VBK.Scroll(h, 0, 47, 1172, 348);
		Widget list = INT_UIF_Draw.ScrollContent(sw);
		for (int i = 0; i < n; i++)
		{
			VB_ClanBankRow row = rows.Get(i);
			float y = 4 + i * 68;
			bool plus = row.Sum > 0;
			int col = VB_C_BAD;
			string icon = "minus";
			string sign = "−";
			if (plus)
			{
				col = VB_C_GOLD;
				icon = "plus";
				sign = "+";
			}
			string who = row.Who;
			if (who == "")
				who = VBT.T("#STR_VB_TAB_C_SYSTEM");
			int sum = Math.AbsInt(row.Sum);
			INT_UIF_Surface.Box(list, 24, y + 14, 32, 32, 8, VB_C_PANEL2);
			VBT.Ico(list, 31, y + 21, 18, icon, col);
			INT_UIF_Draw.Text(list, 70, y, 250, 60, VBT.Fit(who, 250, 16), 16, VB_C_TEXT);
			INT_UIF_Draw.Text(list, 340, y, 380, 60, KindText(row.Kind), 15, VB_C_DIM);
			INT_UIF_Draw.Text(list, 740, y, 220, 60, sign + VBT.Num(sum), 17, col);
			INT_UIF_Draw.Text(list, 980, y, 170, 60, row.Time, 14, VB_C_DIM2);
			if (i < n - 1)
				INT_UIF_Draw.Line(list, 24, y + 64, 1124, 1, VB_C_LINE);
		}
		INT_UIF_Draw.SetScrollContent(sw, n * 68 + 12);
		VBK.ScrollTo(sw, 0);
	}

	override void Tick(float dt)
	{
		m_T = m_T + dt;
		if (m_T < 1)
			return;
		m_T = 0;
		if (!m_TaxT)
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
		m_TaxT.SetText(a + ":" + b + ":" + c);
	}
}
