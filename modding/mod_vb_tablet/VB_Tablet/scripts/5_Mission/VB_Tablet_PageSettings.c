// Настройки клана: правки копятся черновиком, справа предпросмотр и счёт. Платит склад денег.
class VB_TabletPageSettings extends VB_TabletPage
{
	protected static const int TAG_LEADER = 1;
	protected static const int TAG_DISSOLVE = 2;

	// Черновик живёт в статике: перестроение экрана создаёт страницу заново.
	protected static string s_Owner;
	protected static string s_Name;
	protected static string s_Tag;
	protected static string s_Desc;
	protected static bool s_Open;
	protected static string s_Disc;
	protected static string s_Shape;
	protected static string s_Symbol;
	protected static int s_Color;
	protected static float s_AskedAt = -100;
	protected static bool s_Dissolving;
	protected static string s_LeaderUid;

	protected int m_ClanCount;
	protected Widget m_Right;
	protected TextWidget m_OpenText;
	protected ref VBT_Input m_InName;
	protected ref VBT_Input m_InTag;
	protected ref VBT_Input m_InDesc;
	protected ref VBT_Input m_InDisc;
	protected ref VBT_Switch m_OpenSwitch;
	protected ref VBT_Dropdown m_Others;
	protected ref VBT_Button m_Give;
	protected ref VBT_Button m_Dissolve;
	protected ref VBT_Button m_Apply;
	protected ref VBT_Button m_Reset;
	protected ref array<string> m_OtherUids = new array<string>();
	protected ref array<Widget> m_ShapeHits = new array<Widget>();
	protected ref array<Widget> m_SymbolHits = new array<Widget>();
	protected ref array<Widget> m_ColorHits = new array<Widget>();
	protected ref array<ref INT_UIF_Surface> m_ShapeBg = new array<ref INT_UIF_Surface>();
	protected ref array<ref INT_UIF_Surface> m_SymbolBg = new array<ref INT_UIF_Surface>();

	// Изменения: подпись и цена.
	protected ref array<string> m_ChangeNames = new array<string>();
	protected ref array<int> m_ChangePrices = new array<int>();

	override void Build()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c)
		{
			m_Menu.GoLater("profile");
			return;
		}
		if (!client.Can(VB_ClanPerm.SETTINGS))
		{
			m_Menu.GoLater("clan");
			return;
		}
		// Занятые названия, теги и эмблемы проверяются по списку кланов.
		float now = GetGame().GetTickTime();
		if (now - s_AskedAt > 5)
		{
			s_AskedAt = now;
			client.Request(VB_TabletOp.CLAN_LIST);
		}
		m_ClanCount = client.Clans.Count();
		if (s_Owner != c.Id)
			TakeDraft(c);

		Widget L = VBT.Card(m_Root, 0, 0, 724, 720);
		Price(L, 28, 24, 408, "#STR_VB_TAB_B_SET_NAME", client.PriceName);
		m_InName = new VBT_Input(L, 28, 48, 408, 52, "#STR_VB_TAB_B_SET_NAME_HINT", 24, s_Name);
		Price(L, 460, 24, 236, "#STR_VB_TAB_B_SET_TAG", client.PriceTag);
		m_InTag = new VBT_Input(L, 460, 48, 236, 52, "#STR_VB_TAB_B_SET_TAG_HINT", 4, s_Tag);
		Price(L, 28, 122, 484, "#STR_VB_TAB_B_SET_SLOGAN", 0);
		m_InDesc = new VBT_Input(L, 28, 146, 484, 52, "#STR_VB_TAB_B_SET_SLOGAN_HINT", 60, s_Desc);
		VBT.Cap(L, 536, 122, 160, "#STR_VB_TAB_B_SET_OPEN");
		m_OpenText = INT_UIF_Draw.Text(L, 536, 146, 100, 52, "", 16, VB_C_DIM);
		ShowOpen();
		m_OpenSwitch = new VBT_Switch(L, 640, 160, s_Open, m_Menu.GetHitHandler());
		INT_UIF_Draw.Line(L, 28, 214, 668, 1, VB_C_LINE);

		Price(L, 28, 226, 668, "#STR_VB_TAB_B_SET_EMBLEM", client.PriceEmblem);
		array<string> shapes = VBK.Shapes();
		array<string> symbols = VBK.Symbols();
		Pick(L, 28, 250, shapes, true, "shp_", m_ShapeHits, m_ShapeBg);
		Pick(L, 28, 310, symbols, false, "sym_", m_SymbolHits, m_SymbolBg);

		Price(L, 28, 488, 668, "#STR_VB_TAB_B_SET_COLOR", client.PriceColor);
		BuildColors(L);
		INT_UIF_Draw.Line(L, 28, 568, 668, 1, VB_C_LINE);

		Price(L, 28, 584, 320, "#STR_VB_TAB_B_SET_DISCORD", 0);
		m_InDisc = new VBT_Input(L, 28, 608, 320, 44, "#STR_VB_TAB_B_SET_DISCORD_HINT", 60, s_Disc);
		if (c.IsLeader(client.MyUid()))
			BuildLeader(L, c, client.MyUid());

		m_Right = INT_UIF_Draw.Frame(m_Root, 748, 0, 424, 720);
		PaintRight();
	}

	protected void TakeDraft(VB_Clan c)
	{
		s_Owner = c.Id;
		s_Name = c.Title;
		s_Tag = c.Tag;
		s_Desc = c.Slogan;
		s_Open = c.Open;
		s_Disc = c.Discord;
		s_Shape = c.EmbShape;
		s_Symbol = c.EmbSymbol;
		s_Color = c.Color;
	}

	protected void ShowOpen()
	{
		if (s_Open)
		{
			m_OpenText.SetText("#STR_VB_TAB_B_SET_OPEN_ON");
			m_OpenText.SetColor(VB_C_OK);
		}
		else
		{
			m_OpenText.SetText("#STR_VB_TAB_B_SET_OPEN_OFF");
			m_OpenText.SetColor(VB_C_DIM);
		}
	}

	protected void Price(Widget p, float x, float y, float w, string cap, int v)
	{
		VBT.Cap(p, x, y, w - 120, cap);
		if (v > 0)
			VBK.Money(p, x + w - INT_UIF_Draw.TextWidth(VBT.Num(v), 14) - 22, y - 1, v, 14);
		else
			INT_UIF_Draw.Text(p, x + w - 120, y, 120, 18, "#STR_VB_TAB_B_FREE", 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
	}

	// Клан, который занял пару «форма + символ». Цвет не считается.
	protected VB_ClanRow EmblemOwner(string shape, string symbol)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		array<ref VB_ClanRow> rows = client.Clans;
		foreach (VB_ClanRow r : rows)
		{
			if (r.Id != s_Owner && r.EmbShape == shape && r.EmbSymbol == symbol)
				return r;
		}
		return null;
	}

	// Занятые пары затемнены и не выбираются: эмблема не должна повторяться.
	protected void Pick(Widget p, float x, float y, array<string> list, bool isShape, string prefix, array<Widget> hits, array<ref INT_UIF_Surface> bgs)
	{
		float s = 50;
		float g = 6;
		float k = s * 0.58;
		int n = list.Count();
		for (int i = 0; i < n; i++)
		{
			string v = list.Get(i);
			int col = i % 12;
			int row = i / 12;
			float cx = x + col * (s + g);
			float cy = y + row * (s + g);
			bool sel = v == s_Symbol;
			string shape = s_Shape;
			string symbol = v;
			if (isShape)
			{
				sel = v == s_Shape;
				shape = v;
				symbol = s_Symbol;
			}
			bool taken = !sel && EmblemOwner(shape, symbol) != null;
			int fill = VB_C_PANEL2;
			int border = VB_C_LINE2;
			int ink = VB_C_DIM;
			if (sel)
			{
				fill = VB_C_PANEL3;
				border = VB_C_ACCENT;
				ink = VB_C_ACCENT;
			}
			if (taken)
				ink = INT_UIF_Color.WithAlpha(ink, 71);
			bgs.Insert(INT_UIF_Surface.Box(p, cx, cy, s, s, 8, fill, border));
			VBT.Ico(p, cx + (s - k) * 0.5, cy + (s - k) * 0.5, k, prefix + v, ink);
			if (taken)
				VBT.Ico(p, cx + s - 17, cy + 3, 14, "lock", VB_C_DIM);
			hits.Insert(HitAt(p, cx, cy, s, s));
		}
	}

	protected void BuildColors(Widget L)
	{
		Widget f = INT_UIF_Draw.Frame(L, 28, 510, 668, 48);
		array<int> colors = VBK.Colors();
		for (int i = 0; i < colors.Count(); i++)
		{
			int col = colors.Get(i);
			float x = i * 56;
			if (s_Color == col)
				INT_UIF_Surface.Pill(f, x + 2, 1, 46, 46, 0, VB_C_TEXT);
			INT_UIF_Surface.Pill(f, x + 7, 6, 36, 36, col);
			m_ColorHits.Insert(HitAt(f, x + 5, 4, 40, 40));
		}
	}

	// Только лидеру: передача лидерства и опасная зона с роспуском.
	protected void BuildLeader(Widget L, VB_Clan c, string uid)
	{
		VBT.Cap(L, 368, 584, 328, "#STR_VB_TAB_B_SET_GIVE");
		array<string> names = new array<string>();
		m_OtherUids.Clear();
		foreach (VB_ClanMember m : c.Members)
		{
			if (m.Uid == uid)
				continue;
			names.Insert(m.PlayerName);
			m_OtherUids.Insert(m.Uid);
		}
		if (names.Count() > 0)
		{
			m_Others = new VBT_Dropdown(L, 368, 608, 190, 44, names, 0, true);
			m_Give = new VBT_Button(L, 566, 608, 130, 44, "#STR_VB_TAB_B_SET_GIVE_BTN", false, "rank", m_Menu.GetHitHandler(), 14);
		}
		else
			INT_UIF_Draw.Text(L, 368, 608, 328, 44, "#STR_VB_TAB_B_SET_GIVE_NONE", 15, VB_C_DIM2);
		INT_UIF_Draw.Line(L, 28, 660, 668, 1, VB_C_LINE);
		VBT.Cap(L, 28, 680, 400, "#STR_VB_TAB_B_SET_DANGER", VB_C_BAD);
		m_Dissolve = new VBT_Button(L, 506, 668, 190, 40, "#STR_VB_TAB_B_SET_DISSOLVE", false, "logout", m_Menu.GetHitHandler(), 14);
		m_Dissolve.Danger = true;
		m_Dissolve.Bg.SetFill(VB_C_BAD);
	}

	protected string TrimCopy(string s)
	{
		string r = s + "";
		r.TrimInPlace();
		return r;
	}

	protected void CollectChanges(VB_Clan c)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		m_ChangeNames.Clear();
		m_ChangePrices.Clear();
		if (TrimCopy(s_Name) != c.Title)
			AddChange("#STR_VB_TAB_B_CH_NAME", client.PriceName);
		if (s_Tag != c.Tag)
			AddChange("#STR_VB_TAB_B_CH_TAG", client.PriceTag);
		if (s_Shape != c.EmbShape || s_Symbol != c.EmbSymbol)
			AddChange("#STR_VB_TAB_B_CH_EMBLEM", client.PriceEmblem);
		if (s_Color != c.Color)
			AddChange("#STR_VB_TAB_B_CH_COLOR", client.PriceColor);
		if (TrimCopy(s_Desc) != c.Slogan)
			AddChange("#STR_VB_TAB_B_CH_SLOGAN", 0);
		if (s_Open != c.Open)
			AddChange("#STR_VB_TAB_B_CH_OPEN", 0);
		if (DraftInvite() != c.Discord)
			AddChange("#STR_VB_TAB_B_CH_DISCORD", 0);
	}

	protected void AddChange(string name, int price)
	{
		m_ChangeNames.Insert(name);
		m_ChangePrices.Insert(price);
	}

	// Ссылка в том виде, в каком её сохранит сервер. Пока она неверна, остаётся как введена.
	protected string DraftInvite()
	{
		string raw = TrimCopy(s_Disc);
		string v = VB_TabletCheck.ClanInvite(raw);
		if (v == "")
			return raw;
		return v;
	}

	protected int ChangesSum()
	{
		int sum = 0;
		foreach (int p : m_ChangePrices)
			sum += p;
		return sum;
	}

	protected static string LowCopy(string s)
	{
		string r = s + "";
		r.ToLower();
		return r;
	}

	// Текст ошибки или "". Запрещённых слов на клиенте нет: их проверяет сервер при применении.
	protected string Problem(VB_Clan c, int sum)
	{
		array<ref VB_ClanRow> rows = VB_TabletClient.Get().Clans;
		string n = TrimCopy(s_Name);
		string disc = TrimCopy(s_Disc);
		if (disc != "" && VB_TabletCheck.ClanInvite(disc) == "")
			return VBT.T("#STR_VB_TAB_B_ERR_INVITE");
		if (n != c.Title)
		{
			if (n.LengthUtf8() < 3)
				return VBT.T("#STR_VB_TAB_B_ERR_NAME_SHORT");
			string low = LowCopy(n);
			foreach (VB_ClanRow a : rows)
			{
				if (a.Id != c.Id && LowCopy(a.Title) == low)
					return VBT.T("#STR_VB_TAB_B_ERR_NAME_TAKEN");
			}
		}
		if (s_Tag != c.Tag)
		{
			if (s_Tag.Length() < 2)
				return VBT.T("#STR_VB_TAB_B_ERR_TAG_SHORT");
			foreach (VB_ClanRow b : rows)
			{
				if (b.Id != c.Id && b.Tag == s_Tag)
					return VBT.T("#STR_VB_TAB_B_ERR_TAG_TAKEN");
			}
		}
		VB_ClanRow own = EmblemOwner(s_Shape, s_Symbol);
		if (own)
			return VBB.Sub("#STR_VB_TAB_B_ERR_EMBLEM", own.Title);
		if (sum > c.Bank)
			return VBB.Sub("#STR_VB_TAB_B_ERR_BANK", VBT.Num(sum - c.Bank));
		return "";
	}

	// Перерисовка из OnInput и OnData: кнопки этой панели в этот момент никто не нажимает.
	protected void PaintRight()
	{
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (!m_Right || !c)
			return;
		CollectChanges(c);
		int sum = ChangesSum();
		string err = Problem(c, sum);
		INT_UIF_Widgets.ClearChildren(m_Right);

		Widget v = VBT.Card(m_Right, 0, 0, 424, 250);
		VBT.Glow(v, 424, 250);
		VBT.Corners(v, 424, 250, VB_C_ACCENT, 16);
		VBK.Emblem(v, 152, 22, 120, s_Shape, s_Symbol, s_Color);
		string shown = TrimCopy(s_Name);
		if (shown == "")
			shown = VBT.T("#STR_VB_TAB_B_SET_NAME_EMPTY");
		INT_UIF_Draw.Text(v, 16, 150, 392, 38, VBT.Fit(shown, 392, 26), 26, VB_C_TEXT, INT_UIF_Align.CENTER);
		INT_UIF_Draw.Text(v, 16, 190, 392, 24, "[" + s_Tag + "]", 17, VB_C_ACCENT, INT_UIF_Align.CENTER);

		Widget p = VBT.Card(m_Right, 0, 266, 424, 454);
		VBT.Cap(p, 24, 20, 376, "#STR_VB_TAB_B_CHANGES");
		int n = m_ChangeNames.Count();
		if (n == 0)
			INT_UIF_Draw.Text(p, 24, 50, 376, 30, "#STR_VB_TAB_B_CHANGES_NONE", 16, VB_C_DIM2);
		// Шаг макета 34, но до черты над «Итого» входит только пять строк, а изменений бывает семь.
		float step = 34;
		if (n > 5)
			step = 25;
		for (int i = 0; i < n; i++)
		{
			float y = 50 + i * step;
			int price = m_ChangePrices.Get(i);
			VBT.Ico(p, 24, y + 6, 16, "edit", VB_C_ACCENT);
			INT_UIF_Draw.Text(p, 50, y, 190, 30, m_ChangeNames.Get(i), 16, VB_C_TEXT);
			if (price > 0)
				VBK.MoneyR(p, 400, y + 3, price, 18);
			else
				INT_UIF_Draw.Text(p, 250, y, 150, 30, "#STR_VB_TAB_B_FREE", 14, VB_C_DIM2, INT_UIF_Align.RIGHT);
		}
		INT_UIF_Draw.Line(p, 24, 226, 376, 1, VB_C_LINE);
		INT_UIF_Draw.Text(p, 24, 238, 200, 28, "#STR_VB_TAB_B_TOTAL", 16, VB_C_DIM);
		VBK.MoneyR(p, 400, 238, sum, 20);
		INT_UIF_Draw.Text(p, 24, 272, 200, 28, "#STR_VB_TAB_B_IN_BANK", 16, VB_C_DIM);
		VBK.MoneyR(p, 400, 272, c.Bank, 20, VB_C_TEXT);
		INT_UIF_Draw.Text(p, 24, 306, 200, 28, "#STR_VB_TAB_B_LEFT", 16, VB_C_DIM);
		VBK.MoneyR(p, 400, 306, Math.Max(0, c.Bank - sum), 20, VB_C_TEXT);
		INT_UIF_Draw.Text(p, 24, 340, 376, 24, err, 14, VB_C_BAD);
		m_Apply = new VBT_Button(p, 24, 376, 250, 54, "#STR_VB_TAB_B_APPLY", true, "check", m_Menu.GetHitHandler(), 17);
		if (n == 0 || err != "")
			VBB.Disable(p, m_Apply, 24, 376, 250, 54);
		m_Reset = new VBT_Button(p, 284, 376, 116, 54, "#STR_VB_TAB_B_RESET", false, "", m_Menu.GetHitHandler(), 14);
		if (n == 0)
			VBB.Disable(p, m_Reset, 284, 376, 116, 54);
	}

	protected void Apply()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c)
			return;
		CollectChanges(c);
		if (m_ChangeNames.Count() == 0 || Problem(c, ChangesSum()) != "")
			return;
		string title = TrimCopy(s_Name);
		string tag = s_Tag;
		string slogan = TrimCopy(s_Desc);
		string shape = s_Shape;
		string symbol = s_Symbol;
		int color = s_Color;
		bool open = s_Open;
		string discord = "";
		if (TrimCopy(s_Disc) != "")
			discord = VB_TabletCheck.ClanInvite(s_Disc);
		ScriptRPC rpc = client.Begin(VB_TabletOp.CLAN_SETTINGS);
		rpc.Write(title);
		rpc.Write(tag);
		rpc.Write(slogan);
		rpc.Write(shape);
		rpc.Write(symbol);
		rpc.Write(color);
		rpc.Write(open);
		rpc.Write(discord);
		client.Post(rpc);
	}

	protected void Repaint()
	{
		SetFocus(null);
		m_Menu.RequestRepaint();
	}

	// Тег только латиница и цифры в верхнем регистре, как в макете.
	protected string CleanTag(string raw)
	{
		string up = raw + "";
		up.ToUpper();
		string res = "";
		int n = up.Length();
		for (int i = 0; i < n; i++)
		{
			string ch = up.Get(i);
			if ("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789".IndexOf(ch) >= 0)
				res = res + ch;
		}
		return res;
	}

	override void OnInput(VBT_Input field)
	{
		if (field == m_InName)
			s_Name = field.Edit.GetText();
		else if (field == m_InTag)
		{
			string raw = field.Edit.GetText();
			s_Tag = CleanTag(raw);
			if (s_Tag != raw)
				field.Set(s_Tag);
		}
		else if (field == m_InDesc)
			s_Desc = field.Edit.GetText();
		else if (field == m_InDisc)
			s_Disc = field.Edit.GetText();
		else
			return;
		PaintRight();
	}

	override bool OnData()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (!client.Clan)
		{
			if (s_Dissolving)
				INT_UIF_Toast.Info(VBT.T("#STR_VB_TAB_B_SET_DISSOLVED"));
			s_Dissolving = false;
			s_Owner = "";
			m_Menu.GoLater("profile");
			return true;
		}
		if (!client.Can(VB_ClanPerm.SETTINGS))
		{
			m_Menu.GoLater("clan");
			return true;
		}
		// Пришёл список кланов: занятые эмблемы надо затемнить. Черновик в статике не пропадёт.
		if (client.Clans.Count() != m_ClanCount)
		{
			SetFocus(null);
			return false;
		}
		PaintRight();
		return true;
	}

	override void OnAnswer(int tag, bool yes, string text)
	{
		if (!yes)
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c)
			return;
		if (tag == TAG_LEADER)
		{
			string uid = s_LeaderUid;
			ScriptRPC give = client.Begin(VB_TabletOp.CLAN_LEADER);
			give.Write(uid);
			client.Post(give);
			INT_UIF_Toast.Success(VBT.T("#STR_VB_TAB_B_SET_GIVEN"));
			m_Menu.GoLater("clan");
		}
		else if (tag == TAG_DISSOLVE)
		{
			string word = c.Title;
			ScriptRPC rpc = client.Begin(VB_TabletOp.CLAN_DISSOLVE);
			rpc.Write(word);
			client.Post(rpc);
			s_Dissolving = true;
		}
	}

	override bool OnHit(Widget w)
	{
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (!c)
			return false;
		array<string> shapes = VBK.Shapes();
		array<string> symbols = VBK.Symbols();
		int i = m_ShapeHits.Find(w);
		if (i >= 0)
			return PickEmblem(shapes.Get(i), s_Symbol);
		i = m_SymbolHits.Find(w);
		if (i >= 0)
			return PickEmblem(s_Shape, symbols.Get(i));
		i = m_ColorHits.Find(w);
		if (i >= 0)
		{
			array<int> colors = VBK.Colors();
			s_Color = colors.Get(i);
			Repaint();
			return true;
		}
		if (m_OpenSwitch && w == m_OpenSwitch.Hit)
		{
			m_OpenSwitch.Toggle();
			s_Open = m_OpenSwitch.On;
			ShowOpen();
			PaintRight();
			return true;
		}
		if (m_Apply && w == m_Apply.Hit)
		{
			Apply();
			return true;
		}
		if (m_Reset && w == m_Reset.Hit)
		{
			s_Owner = "";
			Repaint();
			return true;
		}
		if (m_Give && w == m_Give.Hit)
		{
			s_LeaderUid = m_OtherUids.Get(m_Others.Selected);
			VB_ClanMember target = c.Member(s_LeaderUid);
			string rank = VBT.T(c.Ranks.Get(1).Title);
			m_Menu.Ask(TAG_LEADER, "#STR_VB_TAB_B_SET_GIVE_Q", VBB.Sub("#STR_VB_TAB_B_SET_GIVE_T", target.PlayerName, rank), "#STR_VB_TAB_B_SET_GIVE_BTN", true);
			return true;
		}
		if (m_Dissolve && w == m_Dissolve.Hit)
		{
			m_Menu.ConfirmWord(TAG_DISSOLVE, "#STR_VB_TAB_B_SET_DISSOLVE_Q", "#STR_VB_TAB_B_SET_DISSOLVE_T", c.Title, "#STR_VB_TAB_B_SET_DISSOLVE_YES");
			return true;
		}
		return false;
	}

	protected bool PickEmblem(string shape, string symbol)
	{
		VB_ClanRow own = EmblemOwner(shape, symbol);
		if (own)
		{
			INT_UIF_Toast.Error(VBB.Sub("#STR_VB_TAB_B_ERR_EMBLEM", own.Title));
			return true;
		}
		s_Shape = shape;
		s_Symbol = symbol;
		Repaint();
		return true;
	}

	protected bool HoverGrid(Widget w, bool enter, array<Widget> hits, array<ref INT_UIF_Surface> bgs, string current, array<string> names)
	{
		int i = hits.Find(w);
		if (i < 0)
			return false;
		if (names.Get(i) != current)
		{
			int fill = VB_C_PANEL2;
			if (enter)
				fill = VB_C_PANEL3;
			bgs.Get(i).SetFill(fill);
		}
		return true;
	}

	override bool OnHover(Widget w, bool enter)
	{
		array<string> shapes = VBK.Shapes();
		array<string> symbols = VBK.Symbols();
		if (HoverGrid(w, enter, m_ShapeHits, m_ShapeBg, s_Shape, shapes))
			return true;
		if (HoverGrid(w, enter, m_SymbolHits, m_SymbolBg, s_Symbol, symbols))
			return true;
		if (m_Apply && w == m_Apply.Hit)
		{
			m_Apply.Hover(enter);
			return true;
		}
		if (m_Reset && w == m_Reset.Hit)
		{
			m_Reset.Hover(enter);
			return true;
		}
		if (m_Give && w == m_Give.Hit)
		{
			m_Give.Hover(enter);
			return true;
		}
		if (m_Dissolve && w == m_Dissolve.Hit)
		{
			m_Dissolve.Hover(enter);
			return true;
		}
		return false;
	}
}
