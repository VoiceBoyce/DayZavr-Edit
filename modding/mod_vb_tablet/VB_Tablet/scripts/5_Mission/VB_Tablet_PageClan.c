// Сводка клана. Здесь же общие детали клановых экранов: VBA_Util и VBA_IconBtn.

// Лимиты из data.js. Клиент берёт их константами, сервер проверяет сам по своему конфигу.
const int VBA_APPLY_MAX = 300;
const int VBA_TAX_LOW = 7;
const float VBA_NEAR_M = 50;
const int VBA_NEAR_MAX = 6;		// строк «кого позвать» в окне приглашения
const int VBA_FRIENDS_MAX = 4;	// из них друзей INT_PartyMode
const string VBA_DASH = "—";

// Ответы диалогов выхода и роспуска: общие для сводки и карточки участника.
const int VBA_TAG_LEAVE = 901;
const int VBA_TAG_DISSOLVE = 902;

class VBA_Util
{
	protected static ref map<int, float> s_AskAt;

	// Запрос к серверу не чаще gap секунд. Экран перестраивается от ответа и снова попадает в Build:
	// без этой паузы вышла бы петля OnData -> Build -> Request -> OnData.
	static void Ask(int op, float gap = 3)
	{
		if (!s_AskAt)
			s_AskAt = new map<int, float>();
		float now = GetGame().GetTickTime();
		float last;
		if (s_AskAt.Find(op, last) && now - last < gap)
			return;
		s_AskAt.Set(op, now);
		VB_TabletClient.Get().Request(op);
	}

	// Названия рангов по умолчанию лежат ключами таблицы, свои ранги обычным текстом.
	static string Title(string s)
	{
		if (s.IndexOf("#") == 0)
			return VBT.T(s);
		return s;
	}

	static string Sub(string key, string tag, string value)
	{
		string s = VBT.T(key);
		s.Replace(tag, value);
		return s;
	}

	// Пустая строка: имя Discord верное, иначе ключ текста ошибки.
	static string DiscordError(string v)
	{
		int n = v.Length();
		if (n < 2 || n > 32)
			return "#STR_VB_TAB_A_DC_LEN";
		string allowed = "abcdefghijklmnopqrstuvwxyz0123456789_.";
		for (int i = 0; i < n; i++)
		{
			string ch = v.Get(i);
			if (allowed.IndexOf(ch) < 0)
				return "#STR_VB_TAB_A_DC_CHARS";
		}
		if (v.IndexOf("..") >= 0)
			return "#STR_VB_TAB_A_DC_DOTS";
		return "";
	}

	static string StripAt(string v)
	{
		string s = v;
		while (s.IndexOf("@") == 0)
			s = s.Substring(1, s.Length() - 1);
		return s;
	}

	// День недели с понедельника: 0..6, формула Зеллера, как на сервере.
	static int WeekDay(int y, int m, int d)
	{
		int yy = y;
		int mm = m;
		if (mm < 3)
		{
			mm = mm + 12;
			yy = yy - 1;
		}
		int k = yy % 100;
		int j = yy / 100;
		int v = d + (13 * (mm + 1)) / 5 + k + k / 4 + j / 4 + 5 * j;
		int r = v % 7;
		return (r + 5) % 7;
	}

	static int MonthDays(int m, int y)
	{
		array<int> all = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		int n = all.Get(m - 1);
		if (m == 2 && y % 4 == 0 && (y % 100 != 0 || y % 400 == 0))
			n = 29;
		return n;
	}

	// Дата back дней назад: день, месяц и день недели.
	static void Back(int back, out int d, out int mo, out int wd)
	{
		int y;
		GetYearMonthDay(y, mo, d);
		for (int i = 0; i < back; i++)
		{
			d = d - 1;
			if (d < 1)
			{
				mo = mo - 1;
				if (mo < 1)
				{
					mo = 12;
					y = y - 1;
				}
				d = MonthDays(mo, y);
			}
		}
		wd = WeekDay(y, mo, d);
	}

	static int Today()
	{
		int d;
		int mo;
		int wd;
		Back(0, d, mo, wd);
		return wd;
	}

	// Подписи семи последних дней вида ДД.ММ, индекс равен числу дней назад.
	static array<string> DayKeys()
	{
		array<string> keys = new array<string>();
		for (int i = 0; i < 7; i++)
		{
			int d;
			int mo;
			int wd;
			Back(i, d, mo, wd);
			string sd;
			string sm;
			VBT.Pad2(sd, d);
			VBT.Pad2(sm, mo);
			keys.Insert(sd + "." + sm);
		}
		return keys;
	}

	// Сегодня, вчера, дальше дата из записи.
	static string DayLabel(string day, array<string> keys)
	{
		int i = keys.Find(day);
		if (i == 0)
			return VBT.T("#STR_VB_TAB_A_TODAY");
		if (i == 1)
			return VBT.T("#STR_VB_TAB_A_YESTERDAY");
		return day;
	}

	protected static int Score(VB_ClanRow row, int mode)
	{
		if (mode == 1)
			return row.Influence;
		return row.WeekInfluence;
	}

	// Кланы по влиянию: 0 за неделю, 1 за всё время.
	static array<VB_ClanRow> SortedClans(int mode)
	{
		array<VB_ClanRow> list = new array<VB_ClanRow>();
		array<ref VB_ClanRow> all = VB_TabletClient.Get().Clans;
		foreach (VB_ClanRow row : all)
		{
			int mine = Score(row, mode);
			int at = list.Count();
			for (int i = 0; i < list.Count(); i++)
			{
				int other = Score(list.Get(i), mode);
				if (mine > other)
				{
					at = i;
					break;
				}
			}
			list.InsertAt(row, at);
		}
		return list;
	}

	// Место клана за неделю, 0 пока список не пришёл.
	static int ClanPlace(string id)
	{
		array<VB_ClanRow> list = SortedClans(0);
		for (int i = 0; i < list.Count(); i++)
		{
			if (list.Get(i).Id == id)
				return i + 1;
		}
		return 0;
	}

	static VB_ClanRow FindClan(string id)
	{
		array<ref VB_ClanRow> all = VB_TabletClient.Get().Clans;
		foreach (VB_ClanRow row : all)
		{
			if (row.Id == id)
				return row;
		}
		return null;
	}

	static VB_ClanRow ClanByTag(string tag)
	{
		array<ref VB_ClanRow> all = VB_TabletClient.Get().Clans;
		foreach (VB_ClanRow row : all)
		{
			if (row.Tag == tag)
				return row;
		}
		return null;
	}

	// Занята ли эмблема: пара форма + символ, цвет не считается.
	static VB_ClanRow EmblemOwner(string shp, string sym)
	{
		array<ref VB_ClanRow> all = VB_TabletClient.Get().Clans;
		foreach (VB_ClanRow row : all)
		{
			if (row.EmbShape == shp && row.EmbSymbol == sym)
				return row;
		}
		return null;
	}

	// Опыт игрока из присланной части рейтинга, -1 если его там нет.
	static int RatingXp(string name)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		int i = client.RatingNames.Find(name);
		if (i < 0)
			return -1;
		return client.RatingXp.Get(i);
	}

	// Вопрос перед выходом или роспуском. Лидер уходит только роспуском или передачей лидерства.
	static void AskExit(VB_TabletMenu menu)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c)
			return;
		if (c.IsLeader(client.MyUid()))
			menu.ConfirmWord(VBA_TAG_DISSOLVE, "#STR_VB_TAB_A_DISSOLVE_ASK", "#STR_VB_TAB_A_DISSOLVE_TEXT", c.Title, "#STR_VB_TAB_A_DISSOLVE_YES");
		else
			menu.Ask(VBA_TAG_LEAVE, "#STR_VB_TAB_A_LEAVE_ASK", "#STR_VB_TAB_A_LEAVE_TEXT", "#STR_VB_TAB_A_CONFIRM", true);
	}

	// true: это был ответ на вопрос AskExit.
	static bool AnswerExit(int tag, bool yes)
	{
		if (tag != VBA_TAG_LEAVE && tag != VBA_TAG_DISSOLVE)
			return false;
		if (!yes)
			return true;
		VB_TabletClient client = VB_TabletClient.Get();
		if (tag == VBA_TAG_LEAVE)
		{
			client.Request(VB_TabletOp.CLAN_LEAVE);
			return true;
		}
		if (!client.Clan)
			return true;
		string word = client.Clan.Title;
		ScriptRPC rpc = client.Begin(VB_TabletOp.CLAN_DISSOLVE);
		rpc.Write(word);
		client.Post(rpc);
		return true;
	}
}

// Кнопка только со значком: у VBT_Button значок стоит левее текста и без текста уходит от центра.
class VBA_IconBtn extends Managed
{
	Widget Hit;
	ref INT_UIF_Surface Bg;

	void VBA_IconBtn(Widget parent, float x, float y, float w, float h, string icon, ScriptedWidgetEventHandler handler)
	{
		Bg = INT_UIF_Surface.Box(parent, x, y, w, h, 8, VB_C_PANEL2, VB_C_LINE2);
		VBT.Ico(parent, x + (w - 18) * 0.5, y + (h - 18) * 0.5, 18, icon, VB_C_TEXT);
		Hit = INT_UIF_Draw.Hit(parent, x, y, w, h, handler);
	}

	void Hover(bool enter)
	{
		int fill = VB_C_PANEL2;
		if (enter)
			fill = VB_C_PANEL3;
		Bg.SetFill(fill);
	}
}

class VB_TabletPageClan extends VB_TabletPage
{
	protected ref array<ref VBT_Button> m_Buttons;
	protected ref VBT_Button m_BtnContracts;
	protected ref VBT_Button m_BtnBank;
	protected ref VBT_Button m_BtnMembers;
	protected ref VBT_Button m_BtnNews;
	protected ref VBT_Button m_BtnJournal;
	protected ref VBT_Button m_BtnSettings;
	protected ref VBT_Button m_BtnSetDc;
	protected ref VBT_Button m_BtnExit;
	protected ref VBA_IconBtn m_BtnCopy;

	protected VBT_Button Btn(Widget p, float x, float y, float w, float h, string text, string icon, float size = 14)
	{
		VBT_Button b = new VBT_Button(p, x, y, w, h, text, false, icon, m_Menu.GetHitHandler(), size);
		m_Buttons.Insert(b);
		return b;
	}

	override void Build()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c)
		{
			m_Menu.GoLater("create");
			return;
		}
		VBA_Util.Ask(VB_TabletOp.CLAN_LIST);
		VBA_Util.Ask(VB_TabletOp.CLAN_JOURNAL);
		VBA_Util.Ask(VB_TabletOp.CLAN_NEWS);
		m_Buttons = new array<ref VBT_Button>();
		BuildHead(c);
		BuildContracts(c);
		BuildBank(c);
		BuildBest(c);
		BuildNews(client);
		BuildJournal(client);
		BuildBottom(client, c);
	}

	protected void BuildHead(VB_Clan c)
	{
		Widget h = INT_UIF_Draw.Frame(m_Root, 0, 0, 1172, 172);
		INT_UIF_Surface.Box(h, 0, 0, 1172, 172, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(h, 1172, 172);
		Widget s = INT_UIF_Draw.Frame(h, 18, 18, 136, 136);
		INT_UIF_Surface.Box(s, 0, 0, 136, 136, 8, VB_C_PANEL2);
		VBT.Corners(s, 136, 136, VB_C_ACCENT, 14);
		VBK.Emblem(s, 18, 18, 100, c.EmbShape, c.EmbSymbol, c.Color);
		string shown = VBT.Fit(c.Title, 540, 36);
		INT_UIF_Draw.Text(h, 178, 16, 560, 46, shown, 36, VB_C_TEXT, INT_UIF_Align.LEFT, true);
		float nw = Math.Min(540, INT_UIF_Draw.TextWidth(c.Title, 36));
		INT_UIF_Draw.Text(h, 178 + nw + 14, 26, 90, 28, "[" + c.Tag + "]", 18, VB_C_ACCENT);
		if (c.Slogan != "")
		{
			string slogan = VBT.Fit(c.Slogan, 600, 15);
			INT_UIF_Draw.Text(h, 178, 64, 620, 24, "«" + slogan + "»", 15, VB_C_DIM);
		}
		string capLevel = VBT.T("#STR_VB_TAB_A_LEVEL_C");
		string capInf = VBT.T("#STR_VB_TAB_A_INFLUENCE_C");
		string cap = capLevel + " " + c.Level.ToString() + " · " + capInf;
		VBT.Cap(h, 178, 102, 300, cap);
		int need = c.NextLevelNeed();
		string have = VBT.Num(c.Influence);
		string goal = VBT.Num(need);
		INT_UIF_Draw.Text(h, 560, 100, 238, 20, have + "/" + goal, 15, VB_C_INF, INT_UIF_Align.RIGHT);
		VBT.Ticks(h, 178, 126, 620, 16, 52, c.Influence / (need + 0.0), VB_C_INF);

		int count = c.Members.Count();
		int online = 0;
		foreach (VB_ClanMember m : c.Members)
		{
			if (m.Online)
				online++;
		}
		int place = VBA_Util.ClanPlace(c.Id);
		int rivals = VB_TabletClient.Get().Clans.Count();
		float placeK = 0;
		string placeText = VBA_DASH;
		if (place > 0)
		{
			placeK = 1 - (place - 1) / (rivals + 0.0);
			placeText = place.ToString();
		}
		int cap2 = VB_TabletClient.Get().ClanMax;
		array<string> caps = {"#STR_VB_TAB_A_MEMBERS_C", "#STR_VB_TAB_A_ONLINE_C", "#STR_VB_TAB_PLACE_C"};
		array<string> vals = new array<string>();
		vals.Insert(count.ToString() + "/" + cap2.ToString());
		vals.Insert(online.ToString());
		vals.Insert(placeText);
		array<float> ks = new array<float>();
		ks.Insert(count / (cap2 + 0.0));
		ks.Insert(online / Math.Max(1, count));
		ks.Insert(placeK);
		for (int i = 0; i < 3; i++)
		{
			float x = 826 + i * 116;
			INT_UIF_Draw.Line(h, x, 20, 1, 132, VB_C_LINE);
			VBT.Cap(h, x + 16, 24, 96, caps.Get(i));
			INT_UIF_Draw.Text(h, x + 16, 50, 96, 44, vals.Get(i), 30, VB_C_TEXT, INT_UIF_Align.LEFT, true);
			VBT.Ticks(h, x + 16, 120, 86, 14, 12, ks.Get(i));
		}
	}

	// Сервер пока не присылает контракты: прочерк вместо счёта, влияние за день считается по участникам.
	protected void BuildContracts(VB_Clan c)
	{
		Widget a = VBT.Card(m_Root, 0, 188, 380, 224, "#STR_VB_TAB_A_CONTRACTS_TODAY");
		VB_TabletClient client = VB_TabletClient.Get();
		if (client.HasContracts)
		{
			int total = client.Contracts.Count();
			int finished = 0;
			array<ref VB_ClanContract> list = client.Contracts;
			foreach (VB_ClanContract k : list)
			{
				if (k.Stage == VB_ContractStage.DONE)
					finished++;
			}
			int left = total - finished;
			INT_UIF_Draw.Text(a, 20, 44, 140, 48, finished.ToString() + "/" + total.ToString(), 36, VB_C_TEXT, INT_UIF_Align.LEFT, true);
			if (left > 0)
				INT_UIF_Draw.Text(a, 150, 44, 210, 48, VBA_Util.Sub("#STR_VB_TAB_A_LEFT_N", "{n}", left.ToString()), 15, VB_C_DIM, INT_UIF_Align.RIGHT);
			else
				INT_UIF_Draw.Text(a, 150, 44, 210, 48, "#STR_VB_TAB_A_ALL_DONE", 15, VB_C_OK, INT_UIF_Align.RIGHT);
			float part = 0;
			if (total > 0)
				part = finished / (total + 0.0);
			VBT.Ticks(a, 20, 100, 340, 14, 40, part);
		}
		else
		{
			VBA_Util.Ask(VB_TabletOp.CONTRACTS);
			INT_UIF_Draw.Text(a, 20, 44, 140, 48, VBA_DASH, 36, VB_C_DIM2, INT_UIF_Align.LEFT, true);
			VBT.Ticks(a, 20, 100, 340, 14, 40, 0);
		}
		int gain = 0;
		foreach (VB_ClanMember m : c.Members)
			gain += m.Today;
		string sum = VBT.Num(gain);
		INT_UIF_Draw.Text(a, 20, 124, 200, 28, "#STR_VB_TAB_A_INF_DAY", 14, VB_C_DIM);
		INT_UIF_Draw.Text(a, 220, 124, 140, 28, "+" + sum, 16, VB_C_INF, INT_UIF_Align.RIGHT);
		m_BtnContracts = Btn(a, 20, 164, 340, 40, "#STR_VB_TAB_F_CONTRACTS", "contract");
	}

	protected void BuildBank(VB_Clan c)
	{
		Widget b = VBT.Card(m_Root, 396, 188, 380, 224, "#STR_VB_TAB_A_BANK_C");
		VBK.Money(b, 20, 50, c.Bank, 34);
		VBT.Ticks(b, 20, 100, 340, 14, 40, c.Bank / (VB_TabletClient.Get().BankMax + 0.0), VB_C_GOLD);
		bool free = c.TaxFree > 0;
		int days = c.TaxFree;
		if (!free)
			days = c.Bank / VB_TabletClient.Get().ClanTax;
		bool low = !free && days <= VBA_TAX_LOW;
		string lbl = "#STR_VB_TAB_A_TAX_FOR";
		if (free)
			lbl = "#STR_VB_TAB_A_TAX_FREE";
		int capCol = VB_C_DIM;
		int valCol = VB_C_GOLD;
		if (low)
		{
			capCol = VB_C_BAD;
			valCol = VB_C_BAD;
		}
		INT_UIF_Draw.Text(b, 20, 124, 200, 28, lbl, 14, capCol);
		string shown = days.ToString() + " " + VBT.T("#STR_VB_TAB_DAYS");
		INT_UIF_Draw.Text(b, 220, 124, 140, 28, shown, 16, valCol, INT_UIF_Align.RIGHT);
		m_BtnBank = Btn(b, 20, 164, 340, 40, "#STR_VB_TAB_F_BANK", "vault");
	}

	protected void BuildBest(VB_Clan c)
	{
		Widget t = VBT.Card(m_Root, 792, 188, 380, 224, "#STR_VB_TAB_A_BEST_TODAY");
		array<VB_ClanMember> top = new array<VB_ClanMember>();
		foreach (VB_ClanMember m : c.Members)
		{
			if (m.Today <= 0)
				continue;
			int at = top.Count();
			for (int i = 0; i < top.Count(); i++)
			{
				if (m.Today > top.Get(i).Today)
				{
					at = i;
					break;
				}
			}
			top.InsertAt(m, at);
		}
		if (top.Count() == 0)
			INT_UIF_Draw.Text(t, 0, 84, 380, 30, "#STR_VB_TAB_A_NO_BEST", 15, VB_C_DIM2, INT_UIF_Align.CENTER);
		for (int k = 0; k < 3 && k < top.Count(); k++)
		{
			VB_ClanMember row = top.Get(k);
			float y = 42 + k * 38;
			int col = VB_C_ACCENT;
			if (k > 0)
				col = VB_C_DIM;
			INT_UIF_Draw.Text(t, 20, y, 24, 36, (k + 1).ToString(), 17, col);
			int dot = VB_C_DIM2;
			if (row.Online)
				dot = VB_C_OK;
			INT_UIF_Surface.Pill(t, 46, y + 14, 8, 8, dot);
			string name = VBT.Fit(row.PlayerName, 200, 16);
			INT_UIF_Draw.Text(t, 64, y, 200, 36, name, 16, VB_C_TEXT);
			INT_UIF_Draw.Text(t, 264, y, 96, 36, "+" + row.Today.ToString(), 16, VB_C_INF, INT_UIF_Align.RIGHT);
		}
		m_BtnMembers = Btn(t, 20, 164, 340, 40, "#STR_VB_TAB_A_ALL_MEMBERS", "users");
	}

	protected void BuildNews(VB_TabletClient client)
	{
		Widget n = VBT.Card(m_Root, 0, 428, 572, 232, "#STR_VB_TAB_A_LAST_NEWS");
		if (client.News.Count() > 0)
		{
			VB_ClanNews w = client.News.Get(0);
			array<int> colors = {VB_C_ACCENT, VB_C_GOLD, VB_C_OK, VB_C_BAD};
			int tag = Math.Clamp(w.TagIdx, 0, 3);
			VBK.Pill(n, 20, 44, "#STR_VB_TAB_A_NEWSTAG_" + tag.ToString(), colors.Get(tag));
			INT_UIF_Draw.Text(n, 300, 44, 252, 24, w.Author + " · " + w.Time, 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
			string head = VBT.Fit(w.Head, 532, 20);
			INT_UIF_Draw.Text(n, 20, 74, 532, 32, head, 20, VB_C_TEXT);
			string body = w.Body;
			if (body.LengthUtf8() > 130)
				body = body.SubstringUtf8(0, 130) + "...";
			INT_UIF_Draw.MText(n, 20, 108, 532, 50, body, 15, VB_C_DIM);
		}
		else
		{
			INT_UIF_Draw.Text(n, 0, 84, 572, 30, "#STR_VB_TAB_A_NO_NEWS", 15, VB_C_DIM2, INT_UIF_Align.CENTER);
		}
		m_BtnNews = Btn(n, 20, 172, 532, 40, "#STR_VB_TAB_A_ALL_NEWS", "news");
	}

	protected void BuildJournal(VB_TabletClient client)
	{
		Widget j = VBT.Card(m_Root, 588, 428, 584, 232, "#STR_VB_TAB_A_JOURNAL_C");
		array<string> keys = VBA_Util.DayKeys();
		for (int k = 0; k < 3 && k < client.Journal.Count(); k++)
		{
			VB_ClanJournalRow row = client.Journal.Get(k);
			float y = 44 + k * 40;
			int col = VB_C_ACCENT;
			if (k > 0)
				col = VB_C_DIM2;
			VBT.Ico(j, 20, y + 7, 18, VBA_Journal.IcoOf(row.Kind), col);
			string line = VBA_Journal.Full(row);
			line = VBT.Fit(line, 350, 15);
			INT_UIF_Draw.Text(j, 50, y, 350, 32, line, 15, VB_C_TEXT);
			string when = VBA_Util.DayLabel(row.Day, keys) + ", " + row.Time;
			INT_UIF_Draw.Text(j, 404, y, 160, 32, when, 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
		}
		m_BtnJournal = Btn(j, 20, 172, 544, 40, "#STR_VB_TAB_A_ALL_JOURNAL", "clock");
	}

	protected void BuildBottom(VB_TabletClient client, VB_Clan c)
	{
		bool canSet = client.Can(VB_ClanPerm.SETTINGS);
		if (canSet)
			m_BtnSettings = Btn(m_Root, 0, 676, 190, 44, "#STR_VB_TAB_A_SETTINGS", "sym_cog");
		Widget dc = INT_UIF_Draw.Frame(m_Root, 206, 676, 760, 44);
		INT_UIF_Surface.Box(dc, 0, 0, 760, 44, 8, VB_C_PANEL2);
		bool has = c.Discord != "";
		int icoCol = VB_C_DIM2;
		string shown = "#STR_VB_TAB_NOT_SET";
		int txtCol = VB_C_DIM2;
		if (has)
		{
			icoCol = VB_C_ACCENT;
			txtCol = VB_C_TEXT;
			shown = VBT.Fit(c.Discord, 450, 15);
		}
		VBT.Ico(dc, 14, 11, 22, "discord", icoCol);
		VBT.Cap(dc, 46, 13, 130, "#STR_VB_TAB_A_CLAN_DISCORD");
		INT_UIF_Draw.Text(dc, 180, 0, 450, 44, shown, 15, txtCol);
		if (has)
			m_BtnCopy = new VBA_IconBtn(dc, 716, 6, 36, 32, "copy", m_Menu.GetHitHandler());
		else if (canSet)
			m_BtnSetDc = Btn(dc, 640, 6, 112, 32, "#STR_VB_TAB_A_SET", "", 13);
		string exitText = "#STR_VB_TAB_A_LEAVE";
		if (c.IsLeader(client.MyUid()))
			exitText = "#STR_VB_TAB_A_DISSOLVE";
		m_BtnExit = Btn(m_Root, 982, 676, 190, 44, exitText, "logout");
		m_BtnExit.Danger = true;
		m_BtnExit.Bg.SetFill(VB_C_BAD);
	}

	override bool OnData()
	{
		if (!VB_TabletClient.Get().Clan)
		{
			m_Menu.GoLater("profile");
			return true;
		}
		return false;
	}

	override void OnAnswer(int tag, bool yes, string text)
	{
		VBA_Util.AnswerExit(tag, yes);
	}

	override bool OnHit(Widget w)
	{
		if (!m_Buttons)
			return false;
		if (w == m_BtnContracts.Hit)
		{
			m_Menu.GoLater("contracts");
			return true;
		}
		if (w == m_BtnBank.Hit)
		{
			m_Menu.GoLater("bank");
			return true;
		}
		if (w == m_BtnMembers.Hit)
		{
			m_Menu.GoLater("cmembers");
			return true;
		}
		if (w == m_BtnNews.Hit)
		{
			m_Menu.GoLater("cnews");
			return true;
		}
		if (w == m_BtnJournal.Hit)
		{
			m_Menu.GoLater("cjournal");
			return true;
		}
		if ((m_BtnSettings && w == m_BtnSettings.Hit) || (m_BtnSetDc && w == m_BtnSetDc.Hit))
		{
			m_Menu.GoLater("csettings");
			return true;
		}
		if (m_BtnCopy && w == m_BtnCopy.Hit)
		{
			VBK.Copy("https://" + VB_TabletClient.Get().Clan.Discord);
			return true;
		}
		if (w == m_BtnExit.Hit)
		{
			VBA_Util.AskExit(m_Menu);
			return true;
		}
		return false;
	}

	override bool OnHover(Widget w, bool enter)
	{
		if (!m_Buttons)
			return false;
		if (m_BtnCopy && w == m_BtnCopy.Hit)
		{
			m_BtnCopy.Hover(enter);
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
