// Создание клана: форма слева, предпросмотр, оплата и условия справа.
// Плюс VB_TabletJoinBlock: приглашение и заявка игрока без клана, в макете они стоят в профиле.

// Клетка выбора эмблемы: живёт всё время экрана, при выборе только перекрашивается, поэтому интерфейс не прыгает.
class VBA_EmblemCell extends Managed
{
	string Value;
	bool Off;
	Widget HitW;
	ImageWidget IcoW;
	ImageWidget LockW;
	ref INT_UIF_Surface Bg;
}

class VB_TabletPageCreate extends VB_TabletPage
{
	protected static const int TAG_CREATE = 1;

	// Черновик не пропадает при переходе на другой экран.
	protected static string s_Name = "";
	protected static string s_Tag = "";
	protected static string s_Slogan = "";
	protected static string s_Shape = "shield";
	protected static string s_Symbol = "crown";
	protected static int s_Color = 0xFFFF7A1A;

	protected ref VBT_Input m_InName;
	protected ref VBT_Input m_InTag;
	protected ref VBT_Input m_InSlogan;
	protected Widget m_R;
	protected TextWidget m_Busy;
	protected ref array<ref VBA_EmblemCell> m_Shapes;
	protected ref array<ref VBA_EmblemCell> m_Symbols;
	protected ref array<Widget> m_ColorHits;
	protected ref array<ref INT_UIF_Surface> m_ColorRings;
	protected Widget m_CreateHit;
	protected ref INT_UIF_Surface m_CreateBg;
	protected ref array<string> m_ChkName;
	protected ref array<bool> m_ChkOk;
	protected ref array<string> m_ChkText;

	override void Build()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (client.Clan)
		{
			m_Menu.GoLater("clan");
			return;
		}
		VBA_Util.Ask(VB_TabletOp.CLAN_LIST);
		VBA_Util.Ask(VB_TabletOp.PROFILE);
		m_Shapes = new array<ref VBA_EmblemCell>();
		m_Symbols = new array<ref VBA_EmblemCell>();
		m_ColorHits = new array<Widget>();
		m_ColorRings = new array<ref INT_UIF_Surface>();
		m_ChkName = new array<string>();
		m_ChkOk = new array<bool>();
		m_ChkText = new array<string>();

		Widget L = VBT.Card(m_Root, 0, 0, 724, 720);
		VBT.Cap(L, 28, 24, 300, "#STR_VB_TAB_A_NAME_C");
		m_InName = new VBT_Input(L, 28, 48, 408, 52, "#STR_VB_TAB_A_NAME_HINT", 24, s_Name);
		VBT.Cap(L, 460, 24, 120, "#STR_VB_TAB_A_TAG_C");
		INT_UIF_Draw.Text(L, 560, 24, 136, 18, "#STR_VB_TAB_A_TAG_LEN", 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
		m_InTag = new VBT_Input(L, 460, 48, 236, 52, "#STR_VB_TAB_A_TAG_HINT", 4, s_Tag);
		VBT.Cap(L, 28, 122, 300, "#STR_VB_TAB_A_SLOGAN_C");
		m_InSlogan = new VBT_Input(L, 28, 146, 668, 52, "#STR_VB_TAB_A_SLOGAN_HINT", 60, s_Slogan);
		INT_UIF_Draw.Line(L, 28, 222, 668, 1, VB_C_LINE);

		VBT.Cap(L, 28, 236, 300, "#STR_VB_TAB_A_SHAPE_C");
		BuildCells(L, 28, 258, VBK.Shapes(), m_Shapes, "shp_");
		VBT.Cap(L, 28, 320, 300, "#STR_VB_TAB_A_SYMBOL_C");
		m_Busy = INT_UIF_Draw.Text(L, 396, 320, 300, 18, "", 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
		BuildCells(L, 28, 342, VBK.Symbols(), m_Symbols, "sym_");

		VBT.Cap(L, 28, 516, 500, "#STR_VB_TAB_A_COLOR_C");
		BuildColors(L);
		INT_UIF_Draw.Line(L, 28, 600, 668, 1, VB_C_LINE);
		INT_UIF_Draw.MText(L, 28, 616, 668, 44, "#STR_VB_TAB_A_CREATE_NOTE1", 14, VB_C_DIM);
		INT_UIF_Draw.MText(L, 28, 662, 668, 40, "#STR_VB_TAB_A_CREATE_NOTE2", 14, VB_C_DIM);

		m_R = INT_UIF_Draw.Frame(m_Root, 748, 0, 424, 720);
		RefreshPicks();
		RefreshRight();
	}

	protected void BuildCells(Widget p, float x, float y, array<string> list, array<ref VBA_EmblemCell> cells, string pre)
	{
		float size = 50;
		float gap = 6;
		float k = size * 0.58;
		for (int i = 0; i < list.Count(); i++)
		{
			int col = i % 12;
			int row = i / 12;
			float cx = x + col * (size + gap);
			float cy = y + row * (size + gap);
			VBA_EmblemCell cell = new VBA_EmblemCell();
			cell.Value = list.Get(i);
			cell.Bg = INT_UIF_Surface.Box(p, cx, cy, size, size, 8, VB_C_PANEL2, VB_C_LINE2);
			cell.IcoW = VBT.Ico(p, cx + (size - k) * 0.5, cy + (size - k) * 0.5, k, pre + cell.Value, VB_C_DIM);
			cell.LockW = VBT.Ico(p, cx + size - 17, cy + 3, 14, "lock", VB_C_DIM);
			cell.HitW = HitAt(p, cx, cy, size, size);
			cells.Insert(cell);
		}
	}

	protected void BuildColors(Widget p)
	{
		Widget f = INT_UIF_Draw.Frame(p, 28, 538, 668, 48);
		array<int> colors = VBK.Colors();
		for (int i = 0; i < colors.Count(); i++)
		{
			float x = i * 56;
			INT_UIF_Surface ring = INT_UIF_Surface.Pill(f, x + 2, 1, 46, 46, 0, VB_C_TEXT);
			ring.Show(colors.Get(i) == s_Color);
			m_ColorRings.Insert(ring);
			INT_UIF_Surface.Pill(f, x + 7, 6, 36, 36, colors.Get(i));
			m_ColorHits.Insert(HitAt(f, x + 5, 4, 40, 40));
		}
	}

	protected void PaintCell(VBA_EmblemCell cell, bool isShape)
	{
		string sh = s_Shape;
		string sy = s_Symbol;
		bool sel = sy == cell.Value;
		if (isShape)
		{
			sh = cell.Value;
			sel = s_Shape == cell.Value;
		}
		else
		{
			sy = cell.Value;
		}
		cell.Off = false;
		if (!sel && VBA_Util.EmblemOwner(sh, sy))
			cell.Off = true;
		int fill = VB_C_PANEL2;
		int border = VB_C_LINE2;
		int ink = VB_C_DIM;
		if (sel)
		{
			fill = VB_C_PANEL3;
			border = s_Color;
			ink = s_Color;
		}
		if (cell.Off)
			ink = INT_UIF_Color.WithAlpha(VB_C_DIM, 71);
		cell.Bg.SetFill(fill);
		cell.Bg.SetBorder(border);
		cell.IcoW.SetColor(ink);
		cell.LockW.Show(cell.Off);
	}

	protected void RefreshPicks()
	{
		foreach (VBA_EmblemCell cellA : m_Shapes)
			PaintCell(cellA, true);
		foreach (VBA_EmblemCell cellB : m_Symbols)
			PaintCell(cellB, false);
		int used = VB_TabletClient.Get().Clans.Count();
		int all = m_Shapes.Count() * m_Symbols.Count();
		string line = VBA_Util.Sub("#STR_VB_TAB_A_BUSY", "{n}", used.ToString());
		line.Replace("{all}", all.ToString());
		m_Busy.SetText(line);
	}

	protected void Check(string name, bool ok, string text)
	{
		m_ChkName.Insert(name);
		m_ChkOk.Insert(ok);
		m_ChkText.Insert(text);
	}

	protected bool TitleTaken(string title)
	{
		string low = title + "";
		low.ToLower();
		array<ref VB_ClanRow> all = VB_TabletClient.Get().Clans;
		foreach (VB_ClanRow row : all)
		{
			string other = row.Title + "";
			other.ToLower();
			if (other == low)
				return true;
		}
		return false;
	}

	// Условия как в макете. Запрещённые слова клиент не знает, их проверит сервер.
	protected bool RunChecks()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		m_ChkName.Clear();
		m_ChkOk.Clear();
		m_ChkText.Clear();

		string nameText = "#STR_VB_TAB_A_CHK_FREE_N";
		bool nameOk = true;
		if (s_Name.LengthUtf8() < 3)
		{
			nameOk = false;
			nameText = "#STR_VB_TAB_A_CHK_MIN3";
		}
		else if (TitleTaken(s_Name))
		{
			nameOk = false;
			nameText = "#STR_VB_TAB_A_CHK_TAKEN_N";
		}
		Check("#STR_VB_TAB_A_CHK_NAME", nameOk, nameText);

		string tagText = "#STR_VB_TAB_A_CHK_FREE_T";
		bool tagOk = true;
		if (s_Tag.Length() < 2)
		{
			tagOk = false;
			tagText = "#STR_VB_TAB_A_CHK_MIN2";
		}
		else if (VBA_Util.ClanByTag(s_Tag))
		{
			tagOk = false;
			tagText = "#STR_VB_TAB_A_CHK_TAKEN_T";
		}
		Check("#STR_VB_TAB_A_CHK_TAG", tagOk, tagText);

		VB_ClanRow owner = VBA_Util.EmblemOwner(s_Shape, s_Symbol);
		string emblemText = "#STR_VB_TAB_A_CHK_FREE_E";
		if (owner)
			emblemText = VBA_Util.Sub("#STR_VB_TAB_A_CHK_TAKEN_E", "{name}", owner.Title);
		Check("#STR_VB_TAB_A_CHK_EMBLEM", owner == null, emblemText);

		VB_TabletRates rates = client.Rates;
		int xp = client.Me.Xp(rates);
		int ri = rates.RankOf(xp);
		bool rankOk = ri >= VB_TabletClient.Get().ClanRank;
		string rankText = VBT.RankName(ri);
		if (!rankOk)
		{
			string have = VBT.Num(xp);
			string goal = VBT.Num(rates.RankNeed.Get(VB_TabletClient.Get().ClanRank));
			string target = VBT.T(VBT.RankName(VB_TabletClient.Get().ClanRank));
			rankText = have + "/" + goal + " " + VBT.T("#STR_VB_TAB_TO_RANK") + " «" + target + "»";
		}
		Check("#STR_VB_TAB_A_CHK_RANK", rankOk, rankText);

		int cash = client.Me.Cash;
		bool cashOk = cash >= VB_TabletClient.Get().ClanPrice;
		string cashText = "#STR_VB_TAB_A_CHK_ENOUGH";
		if (!cashOk)
			cashText = VBA_Util.Sub("#STR_VB_TAB_A_CHK_SHORT", "{sum}", VBT.Num(VB_TabletClient.Get().ClanPrice - cash));
		Check("#STR_VB_TAB_A_CHK_BALANCE", cashOk, cashText);

		int count = client.Clans.Count();
		int limit = VB_TabletClient.Get().MaxClans;
		string line = VBA_Util.Sub("#STR_VB_TAB_A_CHK_OF", "{n}", count.ToString());
		line.Replace("{all}", limit.ToString());
		Check("#STR_VB_TAB_A_CHK_CLANS", count < limit, line);

		return m_ChkOk.Find(false) < 0;
	}

	protected void RefreshRight()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		INT_UIF_Widgets.ClearChildren(m_R);
		m_CreateHit = null;

		Widget c = VBT.Card(m_R, 0, 0, 424, 300);
		VBT.Glow(c, 424, 300, s_Color);
		VBT.Corners(c, 424, 300, s_Color, 16);
		VBK.Emblem(c, 142, 26, 140, s_Shape, s_Symbol, s_Color);
		string shown = "#STR_VB_TAB_A_NAME_PREVIEW";
		int nameCol = VB_C_DIM2;
		if (s_Name != "")
		{
			shown = VBT.Fit(s_Name, 392, 28);
			nameCol = VB_C_TEXT;
		}
		INT_UIF_Draw.Text(c, 16, 180, 392, 38, shown, 28, nameCol, INT_UIF_Align.CENTER);
		string tagLine = "#STR_VB_TAB_A_TAG_PREVIEW";
		if (s_Tag != "")
			tagLine = "[" + s_Tag + "]";
		INT_UIF_Draw.Text(c, 16, 220, 392, 24, tagLine, 17, s_Color, INT_UIF_Align.CENTER);
		string founder = VBA_Util.Sub("#STR_VB_TAB_A_FOUNDER", "{name}", client.Me.PlayerName);
		INT_UIF_Draw.Text(c, 16, 256, 392, 22, founder, 14, VB_C_DIM, INT_UIF_Align.CENTER);

		Widget p = VBT.Card(m_R, 0, 316, 424, 404);
		VBT.Cap(p, 24, 20, 376, "#STR_VB_TAB_A_PAY_C");
		int cash = client.Me.Cash;
		INT_UIF_Draw.Text(p, 24, 44, 190, 26, "#STR_VB_TAB_A_PRICE", 16, VB_C_DIM);
		VBK.MoneyR(p, 400, 42, VB_TabletClient.Get().ClanPrice, 20);
		INT_UIF_Draw.Text(p, 24, 74, 190, 26, "#STR_VB_TAB_A_YOUR_BALANCE", 16, VB_C_DIM);
		int cashCol = VB_C_BAD;
		if (cash >= VB_TabletClient.Get().ClanPrice)
			cashCol = VB_C_GOLD;
		VBK.MoneyR(p, 400, 72, cash, 20, cashCol);
		INT_UIF_Draw.Text(p, 24, 104, 190, 26, "#STR_VB_TAB_A_LEFT_AFTER", 16, VB_C_DIM);
		int rest = Math.Max(0, cash - VB_TabletClient.Get().ClanPrice);
		VBK.MoneyR(p, 400, 102, rest, 20, VB_C_TEXT);
		INT_UIF_Draw.Line(p, 24, 138, 376, 1, VB_C_LINE);

		bool ready = RunChecks();
		for (int i = 0; i < m_ChkName.Count(); i++)
		{
			float y = 146 + i * 30;
			bool ok = m_ChkOk.Get(i);
			string ico = "close";
			int icoCol = VB_C_BAD;
			int txtCol = VB_C_BAD;
			if (ok)
			{
				ico = "check";
				icoCol = VB_C_OK;
				txtCol = VB_C_DIM;
			}
			VBT.Ico(p, 24, y + 3, 20, ico, icoCol);
			float nw = INT_UIF_Draw.TextWidth(m_ChkName.Get(i), 16) + 30;
			INT_UIF_Draw.Text(p, 54, y, nw, 26, m_ChkName.Get(i), 16, VB_C_TEXT);
			string line = VBT.Fit(VBA_Util.Title(m_ChkText.Get(i)), 250, 14);
			INT_UIF_Draw.Text(p, 150, y, 250, 26, line, 14, txtCol, INT_UIF_Align.RIGHT);
		}
		BuildCreateButton(p, ready);
	}

	// Кнопка макета: подпись слева, на тёмной плашке цена. Обычная VBT_Button ставит всё по центру.
	protected void BuildCreateButton(Widget p, bool ready)
	{
		int fill = s_Color;
		int ink = VB_C_ON;
		int plate = 0xFF0E0D0C;
		int gold = VB_C_GOLD;
		if (!ready)
		{
			fill = INT_UIF_Color.WithAlpha(s_Color, 107);
			ink = INT_UIF_Color.WithAlpha(VB_C_ON, 107);
			plate = INT_UIF_Color.WithAlpha(plate, 107);
			gold = INT_UIF_Color.WithAlpha(VB_C_GOLD, 107);
		}
		m_CreateBg = INT_UIF_Surface.Box(p, 24, 332, 376, 56, 8, fill);
		VBT.Ico(p, 44, 351, 18, "plusclan", ink);
		INT_UIF_Draw.Text(p, 72, 332, 150, 56, "#STR_VB_TAB_CREATE", 18, ink);
		INT_UIF_Surface.Box(p, 230, 337, 165, 46, 6, plate);
		VBK.MoneyC(p, 312, 348, VB_TabletClient.Get().ClanPrice, 18, gold);
		if (ready)
		{
			m_CreateHit = HitAt(p, 24, 332, 376, 56);
			m_CreateHit.SetName("VBT_Create");
		}
	}

	protected void Send()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		string title = s_Name;
		string tag = s_Tag;
		string slogan = s_Slogan;
		string shp = s_Shape;
		string sym = s_Symbol;
		int color = s_Color;
		ScriptRPC rpc = client.Begin(VB_TabletOp.CLAN_CREATE);
		rpc.Write(title);
		rpc.Write(tag);
		rpc.Write(slogan);
		rpc.Write(shp);
		rpc.Write(sym);
		rpc.Write(color);
		client.Post(rpc);
	}

	// Тег: заглавная латиница и цифры, остальное отбрасывается.
	protected string CleanTag(string raw)
	{
		string up = raw + "";
		up.ToUpper();
		string allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		string res = "";
		for (int i = 0; i < up.Length(); i++)
		{
			string ch = up.Get(i);
			if (allowed.IndexOf(ch) >= 0)
				res = res + ch;
		}
		return res;
	}

	override void OnInput(VBT_Input field)
	{
		if (field == m_InName)
		{
			s_Name = field.Get();
		}
		else if (field == m_InTag)
		{
			string raw = field.Edit.GetText();
			string clean = CleanTag(raw);
			if (clean != raw)
				field.Set(clean);
			s_Tag = clean;
		}
		else if (field == m_InSlogan)
		{
			s_Slogan = field.Get();
		}
		RefreshRight();
	}

	override void OnAnswer(int tag, bool yes, string text)
	{
		if (tag == TAG_CREATE && yes)
			Send();
	}

	override void OnResult(int code)
	{
		if (code != VB_TabletResult.CREATED)
			return;
		s_Name = "";
		s_Tag = "";
		s_Slogan = "";
	}

	// На экране поля ввода: новые данные обновляют только занятость и правую колонку.
	override bool OnData()
	{
		if (VB_TabletClient.Get().Clan)
		{
			m_Menu.GoLater("clan");
			return true;
		}
		if (!m_R)
			return false;
		RefreshPicks();
		RefreshRight();
		return true;
	}

	override void Leave()
	{
		SetFocus(null);
		m_Menu.SetTyping(false);
	}

	protected bool PickCell(Widget w, array<ref VBA_EmblemCell> cells, bool isShape)
	{
		foreach (VBA_EmblemCell cell : cells)
		{
			if (w != cell.HitW)
				continue;
			if (cell.Off)
			{
				string sh = s_Shape;
				string sy = cell.Value;
				if (isShape)
				{
					sh = cell.Value;
					sy = s_Symbol;
				}
				VB_ClanRow owner = VBA_Util.EmblemOwner(sh, sy);
				if (owner)
					INT_UIF_Toast.Error(VBA_Util.Sub("#STR_VB_TAB_A_CHK_TAKEN_E", "{name}", owner.Title));
				return true;
			}
			if (isShape)
				s_Shape = cell.Value;
			else
				s_Symbol = cell.Value;
			RefreshPicks();
			RefreshRight();
			return true;
		}
		return false;
	}

	override bool OnHit(Widget w)
	{
		if (!m_R)
			return false;
		if (PickCell(w, m_Shapes, true) || PickCell(w, m_Symbols, false))
			return true;
		int i = m_ColorHits.Find(w);
		if (i >= 0)
		{
			array<int> colors = VBK.Colors();
			s_Color = colors.Get(i);
			for (int k = 0; k < m_ColorRings.Count(); k++)
				m_ColorRings.Get(k).Show(k == i);
			RefreshPicks();
			RefreshRight();
			return true;
		}
		if (m_CreateHit && w == m_CreateHit)
		{
			// Мат проверяется до подтверждения оплаты, а не после.
			if (VB_TabletBridge.Censored(s_Name) || VB_TabletBridge.Censored(s_Tag) || VB_TabletBridge.Censored(s_Slogan))
				return true;
			string text = VBA_Util.Sub("#STR_VB_TAB_A_CREATE_ASK", "{sum}", VBT.Num(VB_TabletClient.Get().ClanPrice));
			text.Replace("{name}", s_Name);
			m_Menu.Ask(TAG_CREATE, "#STR_VB_TAB_A_CREATE_TITLE", text, "#STR_VB_TAB_A_CREATE_YES");
			return true;
		}
		return false;
	}

	protected bool HoverCell(Widget w, bool enter, array<ref VBA_EmblemCell> cells, string current)
	{
		foreach (VBA_EmblemCell cell : cells)
		{
			if (w != cell.HitW)
				continue;
			if (cell.Value != current)
			{
				int fill = VB_C_PANEL2;
				if (enter)
					fill = VB_C_PANEL3;
				cell.Bg.SetFill(fill);
			}
			return true;
		}
		return false;
	}

	override bool OnHover(Widget w, bool enter)
	{
		if (!m_R)
			return false;
		if (HoverCell(w, enter, m_Shapes, s_Shape) || HoverCell(w, enter, m_Symbols, s_Symbol))
			return true;
		if (m_CreateHit && w == m_CreateHit)
		{
			int fill = s_Color;
			if (enter)
				fill = INT_UIF_Color.Lerp(s_Color, VB_C_WHITE, 0.28);
			m_CreateBg.SetFill(fill);
			return true;
		}
		return false;
	}
}

// Блок профиля игрока без клана. Первое приглашение заменяет карточку создания,
// заявка показывается строкой внутри неё. Подключается из VB_TabletPageProfile.
class VB_TabletJoinBlock extends Managed
{
	protected ref VBT_Button m_Yes;
	protected ref VBT_Button m_No;
	protected ref VBT_Button m_Cancel;
	protected string m_ClanId;

	bool HasInvite()
	{
		return VB_TabletClient.Get().Invites.Count() > 0;
	}

	// Вместо карточки «Собери свой клан»: r та же правая колонка профиля.
	void BuildInvite(Widget r, VB_TabletMenu menu)
	{
		VB_ClanInvite v = VB_TabletClient.Get().Invites.Get(0);
		m_ClanId = v.ClanId;
		Widget f = INT_UIF_Draw.Frame(r, 0, 436, 808, 284);
		INT_UIF_Surface.Box(f, 0, 0, 808, 284, 10, VB_C_PANEL, VB_C_ACCENT);
		VBT.Glow(f, 808, 284);
		Widget s = INT_UIF_Draw.Frame(f, 24, 28, 190, 228);
		INT_UIF_Surface.Box(s, 0, 0, 190, 228, 8, VB_C_PANEL2);
		VBT.Corners(s, 190, 228, VB_C_ACCENT);
		VBK.Emblem(s, 30, 22, 130, v.EmbShape, v.EmbSymbol, v.Color);
		INT_UIF_Draw.Text(s, 0, 166, 190, 20, "#STR_VB_TAB_A_LEVEL_C", 13, VB_C_DIM, INT_UIF_Align.CENTER);
		INT_UIF_Draw.Text(s, 0, 186, 190, 30, v.Level.ToString(), 26, VB_C_ACCENT, INT_UIF_Align.CENTER);
		VBT.Ico(f, 238, 26, 20, "invite", VB_C_ACCENT);
		VBT.Cap(f, 266, 27, 400, "#STR_VB_TAB_A_INVITE_C", VB_C_ACCENT);
		string title = VBT.Fit(v.Title, 546, 34);
		INT_UIF_Draw.Text(f, 238, 50, 546, 44, title, 34, VB_C_TEXT, INT_UIF_Align.LEFT, true);
		int cap = VB_TabletClient.Get().ClanMax;
		string line = VBA_Util.Sub("#STR_VB_TAB_A_INVITE_LINE", "{tag}", v.Tag);
		line.Replace("{n}", v.MemberCount.ToString());
		line.Replace("{all}", cap.ToString());
		line.Replace("{by}", v.By);
		INT_UIF_Draw.Text(f, 238, 96, 546, 24, line, 15, VB_C_DIM);
		INT_UIF_Draw.MText(f, 238, 128, 546, 60, v.Slogan, 16, VB_C_DIM);
		m_Yes = new VBT_Button(f, 238, 212, 265, 44, "#STR_VB_TAB_A_JOIN", true, "check", menu.GetHitHandler(), 16);
		m_No = new VBT_Button(f, 519, 212, 265, 44, "#STR_VB_TAB_A_REFUSE", false, "close", menu.GetHitHandler(), 16);
	}

	// Строка заявки вместо подписи карточки создания. false: заявки нет, подпись остаётся.
	bool BuildApplied(Widget f, VB_TabletMenu menu)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (client.AppliedClan == "")
			return false;
		VBA_Util.Ask(VB_TabletOp.CLAN_LIST);
		string title = VBA_DASH;
		VB_ClanRow row = VBA_Util.FindClan(client.AppliedClan);
		if (row)
			title = row.Title;
		string line = VBA_Util.Sub("#STR_VB_TAB_A_APPLIED_LINE", "{name}", title);
		line = VBT.Fit(line, 390, 15);
		INT_UIF_Draw.Text(f, 264, 70, 390, 34, line, 15, VB_C_ACCENT);
		m_Cancel = new VBT_Button(f, 664, 70, 120, 34, "#STR_VB_TAB_A_REVOKE", false, "", menu.GetHitHandler(), 13);
		return true;
	}

	protected void Answer(bool accept)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		string id = m_ClanId;
		bool flag = accept;
		ScriptRPC rpc = client.Begin(VB_TabletOp.CLAN_INVITE_ANSWER);
		rpc.Write(id);
		rpc.Write(flag);
		client.Post(rpc);
	}

	bool OnHit(Widget w)
	{
		if (m_Yes && w == m_Yes.Hit)
		{
			Answer(true);
			return true;
		}
		if (m_No && w == m_No.Hit)
		{
			Answer(false);
			INT_UIF_Toast.Info(VBT.T("#STR_VB_TAB_A_INVITE_REFUSED"));
			return true;
		}
		if (m_Cancel && w == m_Cancel.Hit)
		{
			VB_TabletClient.Get().Request(VB_TabletOp.CLAN_APPLY_CANCEL);
			INT_UIF_Toast.Info(VBT.T("#STR_VB_TAB_A_APPLY_REVOKED"));
			return true;
		}
		return false;
	}

	bool OnHover(Widget w, bool enter)
	{
		if (m_Yes && w == m_Yes.Hit)
		{
			m_Yes.Hover(enter);
			return true;
		}
		if (m_No && w == m_No.Hit)
		{
			m_No.Hover(enter);
			return true;
		}
		if (m_Cancel && w == m_Cancel.Hit)
		{
			m_Cancel.Hover(enter);
			return true;
		}
		return false;
	}
}
