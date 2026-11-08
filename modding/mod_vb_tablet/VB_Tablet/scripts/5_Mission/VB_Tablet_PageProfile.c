// Профиль: 3D-модель, ранг, текущая жизнь, показатели, блок клана.
class VB_TabletPageProfile extends VB_TabletPage
{
	protected static const int TAG_DISCORD = 1;

	protected ref array<TextWidget> m_Digits;
	protected ImageWidget m_NowLine;
	protected PlayerPreviewWidget m_Preview;
	protected float m_Tick = 1;
	protected int m_DigitCount;
	protected float m_ScaleX;
	protected Widget m_RankHit;
	protected ref VBT_Button m_BtnFind;
	protected ref VB_TabletJoinBlock m_Join = new VB_TabletJoinBlock();
	protected ref VBT_Button m_BtnCreate;
	protected ref VBT_Button m_BtnClan;
	protected Widget m_DiscordHit;
	protected Widget m_DiscordCopy;
	protected ref INT_UIF_Surface m_DiscordBg;

	override void Build()
	{
		VB_TabletClient session = VB_TabletClient.Get();
		// Показатели меняются в игре (убийства, путь, деньги): свежие при каждом открытии, не чаще раза в 5 с.
		VBA_Util.Ask(VB_TabletOp.PROFILE, 5);
		VB_TabletStats me = session.Me;

		Widget card = VBT.Card(m_Root, 0, 0, 340, 720);
		VBT.Corners(card, 340, 720, VB_C_ACCENT, 22);
		BuildPreview(card);
		INT_UIF_Surface.Box(card, 16, 636, 308, 68, 8, VB_C_PANEL2, VB_C_LINE);
		bool hasDiscord = me.Discord != "";
		int dc = VB_C_DIM2;
		if (hasDiscord)
			dc = VB_C_ACCENT;
		VBT.Ico(card, 30, 658, 24, "discord", dc);
		string capText = "#STR_VB_TAB_DISCORD";
		if (session.Clan)
			capText = "#STR_VB_TAB_B_DISCORD_CLAN";
		VBT.Cap(card, 66, 648, 196, capText);
		string shown = "#STR_VB_TAB_NOT_SET";
		int textColor = VB_C_DIM2;
		if (hasDiscord)
		{
			shown = VBT.Fit("@" + me.Discord, 196, 16);
			textColor = VB_C_TEXT;
		}
		INT_UIF_Draw.Text(card, 66, 668, 196, 26, shown, 16, textColor);
		// Клик по строке копирует имя без @, кнопка справа меняет его.
		if (hasDiscord)
			m_DiscordCopy = HitAt(card, 16, 636, 250, 68);
		m_DiscordBg = INT_UIF_Surface.Box(card, 272, 650, 40, 40, 8, VB_C_PANEL2, VB_C_LINE2);
		string editIcon = "plus";
		if (hasDiscord)
			editIcon = "edit";
		VBT.Ico(card, 283, 661, 18, editIcon, VB_C_TEXT);
		m_DiscordHit = HitAt(card, 272, 650, 40, 40);

		Widget right = INT_UIF_Draw.Frame(m_Root, 364, 0, 808, 720);
		BuildHeader(right, session);
		BuildLife(right, session);
		BuildStats(right, session);
		if (session.Clan)
			BuildMyClan(right, session.Clan, session);
		else if (m_Join.HasInvite())
			m_Join.BuildInvite(right, m_Menu);
		else
			BuildJoin(right);
		m_Tick = 1;
		Tick(0);
	}

	protected void BuildPreview(Widget card)
	{
		Widget w = INT_UIF_Widgets.CreatePath(VB_TABLET_LAYOUT_PREVIEW, card);
		m_Preview = PlayerPreviewWidget.Cast(w);
		if (!m_Preview)
			return;
		INT_UIF_Draw.Place(m_Preview, 20, 12, 300, 612);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			m_Preview.SetPlayer(player);
			m_Preview.SetModelPosition("0 0 0.605");
			m_Preview.Refresh();
			m_Preview.UpdateItemInHands(player.GetEntityInHands());
		}
	}

	protected void BuildHeader(Widget r, VB_TabletClient session)
	{
		VB_TabletStats me = session.Me;
		VB_TabletRates rates = session.Rates;
		string shownName = me.PlayerName;
		if (shownName == "")
			shownName = GetGame().GetPlayer().GetIdentity().GetName();
		INT_UIF_Draw.Text(r, 0, -4, 470, 76, VBT.Fit(shownName, 460, 64), 64, VB_C_TEXT, INT_UIF_Align.LEFT, true);
		// Плашка клана под ником: тег в цвете клана или «без клана».
		VB_Clan clan = session.Clan;
		if (clan)
		{
			string tagText = "[" + clan.Tag + "]  " + clan.Title;
			float tw = INT_UIF_Draw.TextWidth(tagText, 13) * 1.15 + 30;
			if (tw > 460)
				tw = 460;
			INT_UIF_Surface.Pill(r, 0, 84, tw, 28, 0, INT_UIF_Color.WithAlpha(clan.Color, 160));
			INT_UIF_Draw.Text(r, 0, 84, tw, 28, VBT.Fit(tagText, tw - 20, 13), 13, clan.Color, INT_UIF_Align.CENTER);
		}
		else
		{
			INT_UIF_Surface.Pill(r, 0, 84, 118, 28, 0, VB_C_LINE2);
			INT_UIF_Draw.Text(r, 0, 84, 118, 28, "#STR_VB_TAB_NO_CLAN", 13, VB_C_DIM, INT_UIF_Align.CENTER);
		}

		int xp = me.Xp(rates);
		int ri = rates.RankOf(xp);
		Widget m = INT_UIF_Draw.Frame(r, 488, 6, 320, 104);
		INT_UIF_Surface.Box(m, 0, 0, 320, 104, 10, VB_C_PANEL2);
		VBT.Corners(m, 320, 104, VB_C_ACCENT, 12);
		VBT.Insignia(m, 16, 12, 52, ri);
		INT_UIF_Draw.Text(m, 80, 12, 226, 30, VBT.RankName(ri), 22, VB_C_TEXT);
		if (ri + 1 < VB_RANK_COUNT)
		{
			int cur = rates.RankNeed.Get(ri);
			int nx = rates.RankNeed.Get(ri + 1);
			string line = VBT.Num(xp) + "/" + VBT.Num(nx) + " " + VBT.T("#STR_VB_TAB_TO_RANK") + " " + VBT.T(VBT.RankName(ri + 1));
			INT_UIF_Draw.Text(m, 80, 42, 226, 22, VBT.Fit(line, 226, 14), 14, VB_C_DIM);
			VBT.Ticks(m, 16, 76, 288, 14, 34, (xp - cur) / (nx - cur + 0.0));
		}
		else
		{
			INT_UIF_Draw.Text(m, 80, 42, 226, 22, "#STR_VB_TAB_TOP_RANK", 14, VB_C_DIM);
			VBT.Ticks(m, 16, 76, 288, 14, 34, 1);
		}
		m_RankHit = HitAt(m, 0, 0, 320, 104);
		m_RankHit.SetName("VBT_ProfileRank");
	}

	protected void BuildLife(Widget r, VB_TabletClient session)
	{
		VB_TabletStats me = session.Me;
		int life = session.LifeNow();
		Widget f = INT_UIF_Draw.Frame(r, 0, 126, 808, 160);
		INT_UIF_Surface.Box(f, 0, 0, 808, 160, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(f, 808, 160);
		INT_UIF_Surface.Pill(f, 24, 21, 10, 10, VB_C_OK);
		VBT.Cap(f, 44, 17, 300, "#STR_VB_TAB_LIFE");

		// Часы шириной по числу знаков: на PVE бывает больше 99 часов.
		m_Digits = new array<TextWidget>();
		int hours = life / 3600;
		string hs;
		VBT.Pad2(hs, hours);
		int hd = hs.Length();
		m_DigitCount = hd + 6;
		float x = 24;
		for (int i = 0; i < m_DigitCount; i++)
		{
			float dw = 36;
			if (i == hd || i == hd + 3)
				dw = 18;
			m_Digits.Insert(INT_UIF_Draw.Text(f, x, 36, dw, 64, "0", 56, VB_C_TEXT, INT_UIF_Align.CENTER, true));
			x += dw;
		}

		float rw = 590;
		float hNow = Math.Min(24, life / 3600.0);
		for (int t = 0; t <= 48; t++)
		{
			bool big = t % 12 == 0;
			float tick = t * 0.5;
			int col = VB_C_LINE2;
			if (tick <= hNow)
				col = VB_C_ACCENT;
			float bw = 1;
			float bh = 9;
			if (big)
			{
				bw = 2;
				bh = 16;
			}
			INT_UIF_Draw.Line(f, 24 + rw * t / 48, 108, bw, bh, col);
		}
		array<string> marks = {"0", "6", "12", "18", "24"};
		for (int m = 0; m < 5; m++)
		{
			string lbl = marks.Get(m);
			if (m > 0)
				lbl = lbl + " " + VBT.T("#STR_VB_TAB_H");
			INT_UIF_Draw.Text(f, 24 + rw * m / 4 - 20, 127, 40, 18, lbl, 12, VB_C_DIM2, INT_UIF_Align.CENTER);
		}
		m_ScaleX = rw;
		m_NowLine = INT_UIF_Draw.Line(f, 24 + rw * hNow / 24 - 1, 102, 3, 26, VB_C_TEXT);
		float rec = Math.Min(24, me.RecordSec / 3600.0);
		INT_UIF_Draw.Line(f, 24 + rw * rec / 24 - 1, 102, 3, 26, VB_C_GOLD);
		INT_UIF_Draw.Line(f, 640, 20, 1, 120, VB_C_LINE);
		VBT.Cap(f, 660, 18, 130, "#STR_VB_TAB_ON_SERVER");
		INT_UIF_Draw.Text(f, 660, 38, 130, 40, me.Days.ToString() + " " + VBT.T("#STR_VB_TAB_DAYS"), 30, VB_C_TEXT);
		VBT.Cap(f, 660, 88, 130, "#STR_VB_TAB_RECORD", VB_C_GOLD);
		INT_UIF_Draw.Text(f, 660, 108, 130, 32, VBT.Fit(VBT.LifeText(me.RecordSec), 130, 20), 20, VB_C_GOLD);
	}

	protected void Cell(Widget f, int i, string cap, string val, int goal)
	{
		float x = i * 202;
		if (i > 0)
			INT_UIF_Draw.Line(f, x, 16, 1, 94, VB_C_LINE);
		VBT.Cap(f, x + 20, 14, 130, cap);
		float size = 30;
		while (size > 18 && INT_UIF_Draw.TextWidth(val, size) > 162)
			size = size - 1;
		INT_UIF_Draw.Text(f, x + 20, 34, 168, 40, val, size, VB_C_TEXT);
		if (goal > 0)
		{
			string g = VBT.T("#STR_VB_TAB_UP_TO") + " " + VBT.Short(goal);
			float room = 162 - INT_UIF_Draw.TextWidth(cap, 13) - 8;
			if (INT_UIF_Draw.TextWidth(g, 13) > room)
				g = VBT.Short(goal);
			if (INT_UIF_Draw.TextWidth(g, 13) <= room)
				INT_UIF_Draw.Text(f, x + 20, 14, 162, 18, g, 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
		}
	}

	// Цель показателя: следующая ступень достижения, после последней круглое число.
	protected int GoalOf(int ach, int val)
	{
		for (int i = 0; i < VB_ACH_STEPS; i++)
		{
			int s = VB_TabletAch.Step(ach, i);
			if (val < s)
				return s;
		}
		return VBT.Goal(val);
	}

	protected void BuildStats(Widget r, VB_TabletClient session)
	{
		VB_TabletStats me = session.Me;
		Widget f = INT_UIF_Draw.Frame(r, 0, 298, 808, 126);
		INT_UIF_Surface.Box(f, 0, 0, 808, 126, 10, VB_C_PANEL, VB_C_LINE);
		int km = me.DistM / 1000;
		int gz = GoalOf(0, me.Zombies);
		int gm = GoalOf(1, me.Mutants);
		int ga = GoalOf(2, me.Animals);
		int gd = GoalOf(4, km);
		Cell(f, 0, "#STR_VB_TAB_ZOMBIES", VBT.Num(me.Zombies), gz);
		VBT.Ticks(f, 20, 86, 162, 20, 24, me.Zombies / (gz + 0.0));
		Cell(f, 1, "#STR_VB_TAB_MUTANTS", VBT.Num(me.Mutants), gm);
		VBT.Tally(f, 226, 80, 30, me.Mutants, gm);
		Cell(f, 2, "#STR_VB_TAB_ANIMALS", VBT.Num(me.Animals), ga);
		VBT.Dots(f, 424, 84, 12, me.Animals / (ga + 0.0));
		Cell(f, 3, "#STR_VB_TAB_DIST", VBT.Km(me.DistM), gd);
		VBT.Route(f, 626, 87, 162, (me.DistM / 1000.0) / gd);
	}

	// Игрок в клане: вместо «Собери свой клан» карточка его клана и переход в клан.
	protected void BuildMyClan(Widget r, VB_Clan c, VB_TabletClient session)
	{
		Widget f = INT_UIF_Draw.Frame(r, 0, 436, 808, 284);
		INT_UIF_Surface.Box(f, 0, 0, 808, 284, 10, VB_C_PANEL, VB_C_LINE);
		int band = INT_UIF_Color.WithAlpha(c.Color, 217);
		INT_UIF_Draw.Texture(f, 4, 1, 800, 8, VB_TABLET_TEX_HAZARD, band);
		Widget s = INT_UIF_Draw.Frame(f, 24, 32, 216, 228);
		INT_UIF_Surface.Box(s, 0, 0, 216, 228, 8, VB_C_PANEL2);
		VBT.Corners(s, 216, 228, c.Color);
		VBK.Emblem(s, 44, 26, 128, c.EmbShape, c.EmbSymbol, c.Color);
		INT_UIF_Draw.Text(s, 0, 170, 216, 22, "[" + c.Tag + "]", 15, c.Color, INT_UIF_Align.CENTER);
		string level = VBA_Util.Sub("#STR_VB_TAB_C_ADM_LEVEL", "{n}", c.Level.ToString());
		INT_UIF_Draw.Text(s, 0, 194, 216, 18, level, 13, VB_C_DIM2, INT_UIF_Align.CENTER);
		INT_UIF_Draw.Text(f, 264, 28, 520, 40, VBT.Fit(c.Title, 520, 32), 32, VB_C_TEXT, INT_UIF_Align.LEFT, true);
		if (c.Slogan != "")
			INT_UIF_Draw.MText(f, 264, 72, 520, 44, c.Slogan, 15, VB_C_DIM);
		VB_ClanMember me = c.Member(session.MyUid());
		string rank = "";
		if (me)
		{
			VB_ClanRank rk = c.Rank(me.RankId);
			if (rk)
				rank = VBA_Util.Title(rk.Title);
		}
		int members = c.Members.Count();
		array<string> icons = {"rank", "users", "vault", "sym_bolt"};
		// Каждое значение отдельной строкой: два вызова одной функции в одном выражении затирают друг друга.
		string membersText = members.ToString();
		string bankText = VBT.Num(c.Bank);
		string influenceText = VBT.Num(c.Influence);
		array<string> values = {rank, membersText, bankText, influenceText};
		array<string> caps = {"#STR_VB_TAB_RANK", "#STR_VB_TAB_A_ALL_MEMBERS", "#STR_VB_TAB_F_BANK", "#STR_VB_TAB_F_INFLUENCE"};
		for (int k = 0; k < 4; k++)
		{
			int col = k % 2;
			int row = k / 2;
			float x = 264 + col * 262;
			float y = 112 + row * 48;
			INT_UIF_Surface.Box(f, x, y, 40, 40, 8, VB_C_PANEL2);
			VBT.Ico(f, x + 10, y + 10, 20, icons.Get(k), VB_C_ACCENT);
			INT_UIF_Draw.Text(f, x + 52, y, 200, 18, caps.Get(k), 12, VB_C_DIM2);
			INT_UIF_Draw.Text(f, x + 52, y + 18, 200, 22, VBT.Fit(values.Get(k), 200, 16), 16, VB_C_TEXT);
		}
		m_BtnClan = new VBT_Button(f, 264, 212, 520, 48, "#STR_VB_TAB_A_OPEN_CLAN", true, "users", m_Menu.GetHitHandler(), 17);
	}

	protected void BuildJoin(Widget r)
	{
		Widget f = INT_UIF_Draw.Frame(r, 0, 436, 808, 284);
		INT_UIF_Surface.Box(f, 0, 0, 808, 284, 10, VB_C_PANEL, VB_C_LINE);
		INT_UIF_Draw.Texture(f, 4, 1, 800, 8, VB_TABLET_TEX_HAZARD, INT_UIF_Color.WithAlpha(VB_C_ACCENT, 217));
		Widget s = INT_UIF_Draw.Frame(f, 24, 32, 216, 228);
		INT_UIF_Surface.Box(s, 0, 0, 216, 228, 8, VB_C_PANEL2);
		VBT.Corners(s, 216, 228, VB_C_ACCENT);
		VBT.Ico(s, 44, 26, 128, "shp_shield", VB_C_ACCENT);
		VBT.Ico(s, 75, 57, 66, "sym_crown", VB_C_ON);
		INT_UIF_Draw.Text(s, 0, 170, 216, 22, "#STR_VB_TAB_YOUR_EMBLEM", 15, VB_C_TEXT, INT_UIF_Align.CENTER);
		INT_UIF_Draw.Text(s, 0, 194, 216, 18, "#STR_VB_TAB_EMBLEM_SUB", 13, VB_C_DIM2, INT_UIF_Align.CENTER);
		INT_UIF_Draw.Text(f, 264, 28, 330, 40, "#STR_VB_TAB_BUILD_CLAN", 32, VB_C_TEXT, INT_UIF_Align.LEFT, true);
		if (!m_Join.BuildApplied(f, m_Menu))
			INT_UIF_Draw.MText(f, 264, 72, 520, 44, "#STR_VB_TAB_BUILD_TEXT", 15, VB_C_DIM);
		m_BtnFind = new VBT_Button(f, 604, 28, 180, 40, "#STR_VB_TAB_FIND_CLAN", false, "users", m_Menu.GetHitHandler(), 14);
		array<string> icons = {"contract", "sym_bolt", "vault", "trophy"};
		array<string> names = {"#STR_VB_TAB_F_CONTRACTS", "#STR_VB_TAB_F_INFLUENCE", "#STR_VB_TAB_F_BANK", "#STR_VB_TAB_F_RATING"};
		for (int k = 0; k < 4; k++)
		{
			int col = k % 2;
			int row = k / 2;
			float x = 264 + col * 262;
			float y = 112 + row * 48;
			INT_UIF_Surface.Box(f, x, y, 40, 40, 8, VB_C_PANEL2);
			VBT.Ico(f, x + 10, y + 10, 20, icons.Get(k), VB_C_ACCENT);
			INT_UIF_Draw.Text(f, x + 52, y, 200, 40, names.Get(k), 16, VB_C_TEXT);
		}
		m_BtnCreate = new VBT_Button(f, 264, 212, 520, 48, "#STR_VB_TAB_CREATE", true, "plusclan", m_Menu.GetHitHandler(), 17);
	}

	override void Tick(float dt)
	{
		m_Tick += dt;
		if (m_Tick < 1)
			return;
		m_Tick = 0;
		int life = VB_TabletClient.Get().LifeNow();
		string hs;
		string ms;
		string ss;
		VBT.Pad2(hs, life / 3600);
		VBT.Pad2(ms, (life % 3600) / 60);
		VBT.Pad2(ss, life % 60);
		string line = hs + ":" + ms + ":" + ss;
		if (line.Length() != m_DigitCount)
		{
			m_Menu.RequestRepaint();
			return;
		}
		for (int i = 0; i < m_DigitCount; i++)
			m_Digits.Get(i).SetText(line.Get(i));
	}

	protected void AskDiscord(string value)
	{
		m_Menu.Prompt(TAG_DISCORD, "#STR_VB_TAB_B_DISCORD_TITLE", "#STR_VB_TAB_B_DISCORD_HINT", value, 32);
	}

	override void OnAnswer(int tag, bool yes, string text)
	{
		if (tag != TAG_DISCORD || !yes)
			return;
		VB_TabletClient session = VB_TabletClient.Get();
		// @ не хранится: на экранах он дописывается при показе.
		string v = text + "";
		v.ToLower();
		while (v.IndexOf("@") == 0)
		{
			int rest = v.Length() - 1;
			v = v.Substring(1, rest);
		}
		if (v == session.Me.Discord)
			return;
		if (v != "")
		{
			int err = VB_TabletCheck.DiscordError(v);
			if (err != 0)
			{
				INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_B_DISCORD_ERR_" + err.ToString()));
				AskDiscord(v);
				return;
			}
		}
		ScriptRPC rpc = session.Begin(VB_TabletOp.DISCORD_SET);
		rpc.Write(v);
		session.Post(rpc);
		if (v == "")
			INT_UIF_Toast.Success(VBT.T("#STR_VB_TAB_B_DISCORD_DELETED"));
		else
			INT_UIF_Toast.Success(VBT.T("#STR_VB_TAB_B_DISCORD_SAVED"));
	}

	override bool OnHit(Widget w)
	{
		if (m_Join.OnHit(w))
			return true;
		if (m_DiscordHit && w == m_DiscordHit)
		{
			AskDiscord(VB_TabletClient.Get().Me.Discord);
			return true;
		}
		if (m_DiscordCopy && w == m_DiscordCopy)
		{
			VBK.Copy(VB_TabletClient.Get().Me.Discord);
			return true;
		}
		if (w == m_RankHit)
		{
			m_Menu.GoLater("rank");
			return true;
		}
		if (m_BtnFind && w == m_BtnFind.Hit)
		{
			VB_TabletPageRating.s_Tab = 0;
			m_Menu.GoLater("rating");
			return true;
		}
		if (m_BtnCreate && w == m_BtnCreate.Hit)
		{
			m_Menu.GoLater("create");
			return true;
		}
		if (m_BtnClan && w == m_BtnClan.Hit)
		{
			m_Menu.GoLater("clan");
			return true;
		}
		return false;
	}

	override bool OnHover(Widget w, bool enter)
	{
		if (m_Join.OnHover(w, enter))
			return true;
		if (m_DiscordHit && w == m_DiscordHit)
		{
			int fill = VB_C_PANEL2;
			if (enter)
				fill = VB_C_PANEL3;
			m_DiscordBg.SetFill(fill);
			return true;
		}
		if (m_BtnFind && w == m_BtnFind.Hit)
		{
			m_BtnFind.Hover(enter);
			return true;
		}
		if (m_BtnClan && w == m_BtnClan.Hit)
		{
			m_BtnClan.Hover(enter);
			return true;
		}
		if (m_BtnCreate && w == m_BtnCreate.Hit)
		{
			m_BtnCreate.Hover(enter);
			return true;
		}
		return false;
	}
}
