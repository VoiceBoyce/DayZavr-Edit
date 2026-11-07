// Ранг: вкладки «Обзор» и «Достижения».
class VB_TabletPageRank extends VB_TabletTabbedPage
{
	static int s_Tab;
	protected ref array<Widget> m_Hits;
	protected Widget m_SourceHit;
	protected Widget m_AllHit;

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
		VB_TabletClient session = VB_TabletClient.Get();
		if (!session.HasProfile)
			session.Request(VB_TabletOp.PROFILE);
		if (!session.HasRating)
			session.Request(VB_TabletOp.RATING);
		InitTabs(110);
		m_TabNames.Insert("#STR_VB_TAB_OVERVIEW");
		m_TabNames.Insert("#STR_VB_TAB_ACHIEVEMENTS");
		PaintTabs();
		if (s_Tab == 1)
			BuildAch(session);
		else
			BuildMain(session);
	}

	protected void BuildMain(VB_TabletClient session)
	{
		VB_TabletRates rates = session.Rates;
		VB_TabletStats me = session.Me;
		int xp = me.Xp(rates);
		int ri = rates.RankOf(xp);
		Widget h = INT_UIF_Draw.Frame(m_Root, 0, 0, 1172, 292);
		INT_UIF_Surface.Box(h, 0, 0, 1172, 292, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(h, 1172, 292);
		Widget s = INT_UIF_Draw.Frame(h, 24, 28, 236, 236);
		INT_UIF_Surface.Box(s, 0, 0, 236, 236, 8, VB_C_PANEL2);
		VBT.Corners(s, 236, 236, VB_C_ACCENT, 16);
		VBT.Insignia(s, 38, 38, 160, ri);
		VBT.Cap(h, 288, 30, 360, "#STR_VB_TAB_YOUR_RANK");
		INT_UIF_Draw.Text(h, 288, 50, 600, 62, VBT.RankName(ri), 50, VB_C_TEXT, INT_UIF_Align.LEFT, true);
		string step = VBT.T("#STR_VB_TAB_STEP") + " " + (ri + 1).ToString() + " " + VBT.T("#STR_VB_TAB_OF") + " " + VB_RANK_COUNT.ToString();
		if (me.Place > 0)
			step = step + " · " + VBT.T("#STR_VB_TAB_PLACE") + " " + me.Place.ToString();
		INT_UIF_Draw.Text(h, 288, 112, 600, 24, step, 16, VB_C_DIM);
		VBT.Cap(h, 288, 160, 200, "#STR_VB_TAB_XP");
		if (ri + 1 < VB_RANK_COUNT)
		{
			int cur = rates.RankNeed.Get(ri);
			int nx = rates.RankNeed.Get(ri + 1);
			INT_UIF_Draw.Text(h, 700, 158, 448, 20, VBT.Num(xp) + "/" + VBT.Num(nx), 15, VB_C_ACCENT, INT_UIF_Align.RIGHT);
			VBT.Ticks(h, 288, 186, 860, 18, 70, (xp - cur) / (nx - cur + 0.0));
			INT_UIF_Draw.Text(h, 288, 216, 800, 24, VBT.Num(nx - xp) + " " + VBT.T("#STR_VB_TAB_XP_L") + " " + VBT.T("#STR_VB_TAB_TO_RANK") + " " + VBT.T(VBT.RankName(ri + 1)), 16, VB_C_DIM);
		}
		else
		{
			INT_UIF_Draw.Text(h, 700, 158, 448, 20, VBT.Num(xp), 15, VB_C_ACCENT, INT_UIF_Align.RIGHT);
			VBT.Ticks(h, 288, 186, 860, 18, 70, 1);
			INT_UIF_Draw.Text(h, 288, 216, 800, 24, "#STR_VB_TAB_TOP_RANK", 16, VB_C_DIM);
		}
		BuildLadder(rates, ri);
		BuildSources(session);
		BuildTop(session);
	}

	protected void BuildLadder(VB_TabletRates rates, int cur)
	{
		Widget f = VBT.Card(m_Root, 0, 308, 1172, 190, "#STR_VB_TAB_LADDER");
		float step = 1052.0 / (VB_RANK_COUNT - 1);
		float x0 = 60;
		float y = 78;
		INT_UIF_Draw.Line(f, x0, y, step * (VB_RANK_COUNT - 1), 2, VB_C_LINE2);
		INT_UIF_Draw.Line(f, x0, y, step * cur, 2, VB_C_ACCENT);
		for (int k = 0; k < VB_RANK_COUNT; k++)
		{
			float x = x0 + k * step;
			bool on = k <= cur;
			bool me = k == cur;
			int border = VB_C_LINE2;
			if (me)
				border = VB_C_ACCENT;
			INT_UIF_Surface.Box(f, x - 28, y - 28, 56, 56, 12, VB_C_PANEL2, border);
			int ic = VB_C_DIM2;
			if (on)
				ic = VB_C_ACCENT;
			VBT.Insignia(f, x - 20, y - 20, 40, k, ic);
			int tc = VB_C_DIM;
			if (me)
				tc = VB_C_ACCENT;
			else if (on)
				tc = VB_C_TEXT;
			INT_UIF_Draw.Text(f, x - 60, y + 36, 120, 24, VBT.RankName(k), 15, tc, INT_UIF_Align.CENTER);
			string need = "#STR_VB_TAB_START";
			if (rates.RankNeed.Get(k) > 0)
				need = VBT.Num(rates.RankNeed.Get(k));
			INT_UIF_Draw.Text(f, x - 60, y + 60, 120, 20, need, 13, VB_C_DIM2, INT_UIF_Align.CENTER);
		}
	}

	protected void Source(Widget f, float x, float w, string icon, string name, string gain, int total)
	{
		INT_UIF_Surface.Box(f, x, 50, 44, 44, 8, VB_C_PANEL2);
		VBT.Ico(f, x + 11, 61, 22, icon, VB_C_ACCENT);
		INT_UIF_Draw.Text(f, x, 102, w - 18, 24, VBT.Fit(name, w - 18, 15), 15, VB_C_TEXT);
		INT_UIF_Draw.Text(f, x, 126, w - 18, 24, gain, 14, VB_C_ACCENT);
		string line = VBT.T("#STR_VB_TAB_TOTAL") + " " + VBT.Short(total);
		float size = 14;
		if (INT_UIF_Draw.TextWidth(line, 14) > w - 18)
			size = 12;
		INT_UIF_Draw.Text(f, x, 152, w - 18, 24, line, size, VB_C_DIM2);
	}

	protected void BuildSources(VB_TabletClient session)
	{
		VB_TabletRates r = session.Rates;
		VB_TabletStats me = session.Me;
		Widget f = VBT.Card(m_Root, 0, 514, 760, 206, "#STR_VB_TAB_SOURCES");
		float w = 120;
		string xpl = " " + VBT.T("#STR_VB_TAB_XP_L");
		Source(f, 20, w, "star", VBT.T("#STR_VB_TAB_SRC_ACH"), VBT.T("#STR_VB_TAB_BY_STEPS"), me.AchXp(r));
		Source(f, 20 + w, w, "zombie", VBT.T("#STR_VB_TAB_SRC_Z"), "+" + r.XpZombie.ToString() + xpl, me.Zombies * r.XpZombie);
		Source(f, 20 + w * 2, w, "crosshair", VBT.T("#STR_VB_TAB_SRC_M"), "+" + r.XpMutant.ToString() + xpl, me.Mutants * r.XpMutant);
		Source(f, 20 + w * 3, w, "stash", VBT.T("#STR_VB_TAB_SRC_A"), "+" + r.XpAnimal.ToString() + xpl, me.Animals * r.XpAnimal);
		int km = me.DistM / 1000;
		Source(f, 20 + w * 4, w, "route", VBT.T("#STR_VB_TAB_SRC_K"), "+" + r.XpKm.ToString() + xpl, km * r.XpKm);
		Source(f, 20 + w * 5, w, "contract", VBT.T("#STR_VB_TAB_SRC_C"), "+" + r.XpContract.ToString() + xpl, me.Contracts * r.XpContract + me.ExtraXp);
		for (int i = 1; i < 6; i++)
			INT_UIF_Draw.Line(f, 10 + i * w, 50, 1, 136, VB_C_LINE);
		m_SourceHit = HitAt(f, 14, 44, w - 8, 140);
	}

	protected void BuildTop(VB_TabletClient session)
	{
		Widget f = VBT.Card(m_Root, 776, 514, 396, 206, "#STR_VB_TAB_TOP");
		INT_UIF_Draw.Text(f, 236, 14, 140, 20, "#STR_VB_TAB_ALL_RATING", 14, VB_C_ACCENT, INT_UIF_Align.RIGHT);
		m_AllHit = HitAt(f, 236, 10, 140, 28);
		int n = Math.Min(4, session.RatingNames.Count());
		string own = session.Me.PlayerName;
		for (int i = 0; i < n; i++)
		{
			float y = 46 + i * 38;
			int xp = session.RatingXp.Get(i);
			string name = session.RatingNames.Get(i);
			int col = VB_C_TEXT;
			int ic = VB_C_DIM;
			if (name == own)
			{
				col = VB_C_ACCENT;
				ic = VB_C_ACCENT;
			}
			INT_UIF_Draw.Text(f, 20, y, 24, 34, (i + 1).ToString(), 15, VB_C_DIM2);
			VBT.Insignia(f, 46, y + 4, 26, session.Rates.RankOf(xp), ic);
			INT_UIF_Draw.Text(f, 82, y, 160, 34, VBT.Fit(name, 160, 16), 16, col);
			INT_UIF_Draw.Text(f, 236, y, 140, 34, VBT.Short(xp), 15, VB_C_DIM, INT_UIF_Align.RIGHT);
		}
	}

	protected void BuildAch(VB_TabletClient session)
	{
		VB_TabletRates r = session.Rates;
		VB_TabletStats me = session.Me;
		int got = me.AchCount(r);
		int all = VB_ACH_COUNT * VB_ACH_STEPS;
		Widget h = INT_UIF_Draw.Frame(m_Root, 0, 0, 1172, 112);
		INT_UIF_Surface.Box(h, 0, 0, 1172, 112, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(h, 1172, 112);
		VBT.Cap(h, 24, 18, 240, "#STR_VB_TAB_ACH_GOT");
		INT_UIF_Draw.Text(h, 24, 40, 160, 44, got.ToString() + "/" + all.ToString(), 34, VB_C_TEXT);
		VBT.Ticks(h, 190, 56, 190, 14, all, got / (all + 0.0));
		INT_UIF_Draw.Line(h, 404, 18, 1, 76, VB_C_LINE);
		VBT.Cap(h, 424, 18, 240, "#STR_VB_TAB_ACH_XP");
		INT_UIF_Draw.Text(h, 424, 40, 240, 44, "+" + VBT.Num(me.AchXp(r)), 34, VB_C_ACCENT);
		INT_UIF_Draw.Line(h, 684, 18, 1, 76, VB_C_LINE);
		VBT.Cap(h, 704, 18, 240, "#STR_VB_TAB_ACH_NEAR");

		int best = -1;
		float bestK = -1;
		for (int a = 0; a < VB_ACH_COUNT; a++)
		{
			int d = me.AchDone(a, r);
			if (d >= VB_ACH_STEPS)
				continue;
			int from = 0;
			if (d > 0)
				from = VB_TabletAch.Step(a, d - 1);
			float k = (me.AchValue(a, r) - from) / (VB_TabletAch.Step(a, d) - from + 0.0);
			if (k > bestK)
			{
				bestK = k;
				best = a;
			}
		}
		if (best >= 0)
		{
			int bd = me.AchDone(best, r);
			VBT.Ico(h, 704, 46, 26, VB_TabletAch.IconOf(best), VB_C_ACCENT);
			INT_UIF_Draw.Text(h, 742, 38, 220, 28, VBT.Fit(VBT.T("#STR_VB_TAB_ACHN_" + best.ToString()), 220, 19), 19, VB_C_TEXT);
			INT_UIF_Draw.Text(h, 742, 66, 220, 24, VBT.Num(me.AchValue(best, r)) + "/" + VBT.Num(VB_TabletAch.Step(best, bd)), 15, VB_C_DIM);
			VBT.Ticks(h, 966, 56, 182, 14, 20, bestK);
		}
		else
			INT_UIF_Draw.Text(h, 704, 40, 440, 44, "#STR_VB_TAB_ACH_ALL", 24, VB_C_GOLD);

		for (int i = 0; i < VB_ACH_COUNT; i++)
			AchCard(me, r, i);
	}

	protected void AchCard(VB_TabletStats me, VB_TabletRates r, int i)
	{
		array<int> medal = {VB_C_BRONZE, VB_C_SILVER, VB_C_GOLD};
		int col4 = i % 4;
		int row = i / 4;
		float x = col4 * 297;
		float y = 128 + row * 304;
		int val = me.AchValue(i, r);
		int d = me.AchDone(i, r);
		bool full = d >= VB_ACH_STEPS;
		int col = VB_C_DIM;
		if (d > 0)
			col = medal.Get(d - 1);
		Widget f = VBT.Card(m_Root, x, y, 281, 288);
		int border = VB_C_LINE2;
		if (d > 0)
			border = col;
		INT_UIF_Surface.Box(f, 20, 20, 48, 48, 10, VB_C_PANEL2, border);
		VBT.Ico(f, 32, 32, 24, VB_TabletAch.IconOf(i), col);
		INT_UIF_Draw.Text(f, 80, 18, 186, 28, VBT.Fit(VBT.T("#STR_VB_TAB_ACHN_" + i.ToString()), 186, 18), 18, VB_C_TEXT);
		INT_UIF_Draw.Text(f, 80, 46, 186, 22, VBT.Fit(VBT.T("#STR_VB_TAB_ACHT_" + i.ToString()), 186, 14), 14, VB_C_DIM);
		for (int k = 0; k < VB_ACH_STEPS; k++)
		{
			float sx = 20 + k * 83;
			bool on = k < d;
			int m = medal.Get(k);
			int sb = 0;
			int stc = VB_C_DIM;
			int rw = VB_C_DIM2;
			if (on)
			{
				sb = m;
				stc = m;
				rw = VB_C_DIM;
			}
			INT_UIF_Surface.Box(f, sx, 92, 75, 62, 8, VB_C_PANEL2, sb);
			INT_UIF_Draw.Text(f, sx, 98, 75, 26, VBT.Short(VB_TabletAch.Step(i, k)), 15, stc, INT_UIF_Align.CENTER);
			INT_UIF_Draw.Text(f, sx, 124, 75, 22, "+" + VBT.Num(VB_TabletAch.Reward(k)), 13, rw, INT_UIF_Align.CENTER);
		}
		if (full)
		{
			INT_UIF_Draw.Text(f, 20, 176, 241, 28, "#STR_VB_TAB_ACH_ALL", 15, VB_C_GOLD);
			VBT.Ticks(f, 20, 206, 241, 12, 24, 1, VB_C_GOLD);
		}
		else
		{
			int nx = VB_TabletAch.Step(i, d);
			int from = 0;
			if (d > 0)
				from = VB_TabletAch.Step(i, d - 1);
			VBT.Cap(f, 20, 178, 120, VBT.T("#STR_VB_TAB_ACH_STEP") + " " + (d + 1).ToString());
			INT_UIF_Draw.Text(f, 100, 176, 161, 22, VBT.Num(val) + "/" + VBT.Num(nx), 15, VB_C_TEXT, INT_UIF_Align.RIGHT);
			VBT.Ticks(f, 20, 206, 241, 12, 24, (val - from) / (nx - from + 0.0));
			INT_UIF_Draw.Text(f, 20, 232, 241, 24, VBT.T("#STR_VB_TAB_ACH_REWARD") + ": +" + VBT.Num(VB_TabletAch.Reward(d)) + " " + VBT.T("#STR_VB_TAB_XP_L"), 14, VB_C_ACCENT);
		}
	}

	override bool OnHit(Widget w)
	{
		if (w && w == m_SourceHit)
		{
			s_Tab = 1;
			m_Menu.RequestRepaint();
			return true;
		}
		if (w && w == m_AllHit)
		{
			VB_TabletPageRating.s_Tab = 1;
			m_Menu.GoLater("rating");
			return true;
		}
		return super.OnHit(w);
	}
}
