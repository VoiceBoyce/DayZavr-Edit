// Администратор: все кланы, переименование, смена лидера, роспуск, запрещённые слова.
// Раздел виден только администраторам, сервер всё равно проверяет каждую операцию.
class VB_TabletPageAdmin extends VB_TabletPage
{
	protected float m_ModSentAt = -100;
	static const int TAG_DISSOLVE = 3001;

	static string s_Sel;	// id выбранного клана

	protected ref array<Widget> m_RowHits;
	protected ref array<ref INT_UIF_Surface> m_RowBg;
	protected ref array<string> m_RowIds;
	protected ref VBT_Button m_BtnRename;
	protected ref VBT_Button m_BtnLeader;
	protected ref VBT_Button m_BtnDissolve;
	protected ref VBT_Input m_Word;
	protected ref VBT_Button m_BtnWord;
	protected ref array<Widget> m_ChipHits;
	protected ref array<ImageWidget> m_ChipIcons;
	protected ref array<string> m_ChipWords;
	// Окно: 1 переименование, 2 смена лидера.
	protected ref VBT_Modal m_Modal;
	protected int m_ModalKind;
	protected bool m_WaitMembers;
	protected string m_ModalClan;
	protected ref VBT_Input m_ModName;
	protected ref VBT_Input m_ModTag;
	protected TextWidget m_ModErr;
	protected ref VBT_Button m_ModYes;
	protected ref VBT_Button m_ModNo;
	protected ref VBT_Dropdown m_ModList;
	protected ref array<string> m_ModUids;

	override void Build()
	{
		VBA_Util.Ask(VB_TabletOp.CLAN_LIST);
		VBA_Util.Ask(VB_TabletOp.ADMIN_BANNED);
		VB_TabletClient client = VB_TabletClient.Get();
		array<ref VB_ClanRow> rows = client.Clans;
		if (!VBA_Util.FindClan(s_Sel))
		{
			s_Sel = "";
			if (rows.Count() > 0)
				s_Sel = rows.Get(0).Id;
		}
		BuildTable(rows);
		BuildCard();
		BuildBanned(client);
	}

	// Дней налога, -1 пока льгота.
	protected int DaysOf(VB_ClanRow row)
	{
		if (row.TaxFree > 0)
			return -1;
		return VB_TabletClient.Get().TaxDays(row.Bank);
	}

	protected void BuildTable(array<ref VB_ClanRow> rows)
	{
		string own = "";
		VB_TabletClient client = VB_TabletClient.Get();
		if (client.Clan)
			own = client.Clan.Id;
		int n = rows.Count();
		int maxClans = VB_TabletClient.Get().MaxClans;
		int maxPeople = VB_TabletClient.Get().ClanMax;
		Widget t = VBT.Card(m_Root, 0, 0, 724, 720);
		VBT.Cap(t, 58, 18, 200, VBT.T("#STR_VB_TAB_C_ADM_CLAN") + " · " + n.ToString() + "/" + maxClans.ToString());
		VBT.Cap(t, 290, 18, 110, "#STR_VB_TAB_C_ADM_LEADER");
		VBT.Cap(t, 410, 18, 70, "#STR_VB_TAB_C_ADM_MEMBERS");
		VBT.Cap(t, 486, 18, 110, "#STR_VB_TAB_C_ADM_BANK");
		VBT.Cap(t, 606, 18, 100, "#STR_VB_TAB_C_ADM_TAX");
		INT_UIF_Draw.Line(t, 0, 46, 724, 1, VB_C_LINE);
		m_RowHits = new array<Widget>();
		m_RowBg = new array<ref INT_UIF_Surface>();
		m_RowIds = new array<string>();
		if (n == 0)
		{
			INT_UIF_Draw.Text(t, 0, 347, 724, 30, "#STR_VB_TAB_C_ADM_EMPTY", 17, VB_C_DIM2, INT_UIF_Align.CENTER);
			return;
		}
		ScrollWidget sw = VBK.Scroll(t, 0, 47, 724, 672);
		Widget list = INT_UIF_Draw.ScrollContent(sw);
		for (int i = 0; i < n; i++)
		{
			VB_ClanRow row = rows.Get(i);
			float y = 3 + i * 50;
			bool on = row.Id == s_Sel;
			int fill = 0;
			int border = 0;
			if (on)
			{
				fill = VB_C_PANEL3;
				border = VB_C_ACCENT;
			}
			m_RowBg.Insert(INT_UIF_Surface.Box(list, 8, y, 700, 46, 6, fill, border));
			VBK.Emblem(list, 20, y + 8, 30, row.EmbShape, row.EmbSymbol, row.Color);
			int ink = VB_C_TEXT;
			if (row.Id == own)
				ink = VB_C_ACCENT;
			string name = row.Title + " [" + row.Tag + "]";
			string size = row.MemberCount.ToString() + "/" + maxPeople.ToString();
			int days = DaysOf(row);
			string tax = VBT.T("#STR_VB_TAB_C_ADM_FREE");
			int taxCol = VB_C_DIM;
			if (days >= 0)
			{
				tax = VBA_Util.Sub("#STR_VB_TAB_C_ADM_DAYS", "{n}", days.ToString());
				if (days <= VBA_TAX_LOW)
					taxCol = VB_C_BAD;
			}
			INT_UIF_Draw.Text(list, 58, y, 226, 46, VBT.Fit(name, 226, 15), 15, ink);
			INT_UIF_Draw.Text(list, 290, y, 112, 46, VBT.Fit(row.Leader, 112, 14), 14, VB_C_DIM);
			INT_UIF_Draw.Text(list, 410, y, 70, 46, size, 14, VB_C_DIM);
			INT_UIF_Draw.Text(list, 486, y, 112, 46, VBT.Num(row.Bank), 14, VB_C_GOLD);
			INT_UIF_Draw.Text(list, 606, y, 96, 46, tax, 14, taxCol);
			m_RowHits.Insert(HitAt(list, 8, y, 700, 46));
			m_RowIds.Insert(row.Id);
		}
		INT_UIF_Draw.SetScrollContent(sw, n * 50 + 6);
		VBK.ScrollTo(sw, 0);
	}

	protected void BuildCard()
	{
		Widget p = INT_UIF_Draw.Frame(m_Root, 748, 0, 424, 356);
		INT_UIF_Surface.Box(p, 0, 0, 424, 356, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(p, 424, 356);
		VB_ClanRow row = VBA_Util.FindClan(s_Sel);
		if (!row)
			return;
		VBK.Emblem(p, 24, 24, 72, row.EmbShape, row.EmbSymbol, row.Color);
		INT_UIF_Draw.Text(p, 112, 24, 288, 34, VBT.Fit(row.Title, 288, 22), 22, VB_C_TEXT);
		string sub = "[" + row.Tag + "] · " + VBA_Util.Sub("#STR_VB_TAB_C_ADM_LEVEL", "{n}", row.Level.ToString());
		INT_UIF_Draw.Text(p, 112, 60, 288, 24, sub, 14, VB_C_DIM);
		int days = DaysOf(row);
		string tax = VBT.T("#STR_VB_TAB_C_ADM_FREE");
		if (days >= 0)
			tax = VBA_Util.Sub("#STR_VB_TAB_C_ADM_FOR", "{n}", days.ToString());
		int maxPeople = VB_TabletClient.Get().ClanMax;
		string size = row.MemberCount.ToString() + "/" + maxPeople.ToString();
		array<string> caps = {"#STR_VB_TAB_C_ADM_LEADER", "#STR_VB_TAB_C_ADM_PEOPLE", "#STR_VB_TAB_C_ADM_BANK", "#STR_VB_TAB_C_ADM_TAX"};
		array<string> vals = new array<string>();
		vals.Insert(row.Leader);
		vals.Insert(size);
		vals.Insert(VBT.Num(row.Bank));
		vals.Insert(tax);
		for (int i = 0; i < 4; i++)
		{
			int col = i % 2;
			int line = i / 2;
			float cx = 24 + col * 200;
			float cy = 118 + line * 64;
			VBT.Cap(p, cx, cy, 180, caps.Get(i));
			INT_UIF_Draw.Text(p, cx, cy + 20, 184, 30, VBT.Fit(vals.Get(i), 184, 18), 18, VB_C_TEXT);
		}
		// Три кнопки на всю строку 24..400: каждой ширина подписи с отступами, остаток поровну.
		// Если подписи не влезают и так, шрифт мельче.
		ScriptedWidgetEventHandler handler = m_Menu.GetHitHandler();
		array<string> names = {"#STR_VB_TAB_C_ADM_RENAME", "#STR_VB_TAB_C_ADM_SETLEADER", "#STR_VB_TAB_C_ADM_DISSOLVE"};
		float room = 376 - 16;
		float fs = 13;
		array<float> widths = new array<float>();
		float sum = 0;
		while (true)
		{
			widths.Clear();
			sum = 0;
			foreach (string nm : names)
			{
				float bw = VBT.TextW(p, nm, fs) + 28;
				widths.Insert(bw);
				sum += bw;
			}
			if (sum <= room || fs <= 11)
				break;
			fs -= 1;
		}
		float extra = Math.Max(0, (room - sum) / 3);
		float bx = 24;
		array<ref VBT_Button> made = new array<ref VBT_Button>();
		for (int b = 0; b < 3; b++)
		{
			float w = widths.Get(b) + extra;
			made.Insert(new VBT_Button(p, bx, 272, w, 48, names.Get(b), false, "", handler, fs));
			bx += w + 8;
		}
		m_BtnRename = made.Get(0);
		m_BtnLeader = made.Get(1);
		m_BtnDissolve = made.Get(2);
		m_BtnDissolve.Danger = true;
		m_BtnDissolve.Bg.SetFill(VB_C_BAD);
	}

	protected void BuildBanned(VB_TabletClient client)
	{
		Widget f = VBT.Card(m_Root, 748, 372, 424, 348, "#STR_VB_TAB_C_ADM_WORDS");
		m_Word = new VBT_Input(f, 24, 46, 264, 44, "#STR_VB_TAB_C_ADM_WORD", 24);
		m_BtnWord = new VBT_Button(f, 298, 46, 102, 44, "#STR_VB_TAB_C_ADM_ADD", false, "", m_Menu.GetHitHandler(), 14);
		m_ChipHits = new array<Widget>();
		m_ChipIcons = new array<ImageWidget>();
		m_ChipWords = new array<string>();
		float cx = 24;
		float cy = 106;
		array<string> words = client.Banned;
		foreach (string word : words)
		{
			float tw = INT_UIF_Draw.TextWidth(word, 14) + 42;
			if (cx + tw > 400)
			{
				cx = 24;
				cy = cy + 42;
			}
			// Слова, что не помещаются над подсказкой, остаются в конфиге.
			if (cy > 232)
				break;
			INT_UIF_Surface.Box(f, cx, cy, tw, 34, 12, VB_C_PANEL2, VB_C_LINE2);
			INT_UIF_Draw.Text(f, cx + 14, cy, tw - 36, 34, word, 14, VB_C_TEXT);
			m_ChipIcons.Insert(VBT.Ico(f, cx + tw - 26, cy + 10, 14, "close", VB_C_DIM));
			m_ChipHits.Insert(HitAt(f, cx, cy, tw, 34));
			m_ChipWords.Insert(word);
			cx = cx + tw + 8;
		}
		INT_UIF_Draw.MText(f, 24, 276, 376, 54, "#STR_VB_TAB_C_ADM_WORDS_TEXT", 14, VB_C_DIM2);
	}

	protected void CloseModal()
	{
		SetFocus(null);
		if (m_Modal)
			m_Modal.Close();
		m_Modal = null;
		m_ModalKind = 0;
		m_WaitMembers = false;
		m_ModName = null;
		m_ModTag = null;
		m_ModErr = null;
		m_ModYes = null;
		m_ModNo = null;
		m_ModList = null;
	}

	protected void OpenRename(VB_ClanRow row)
	{
		float w = 520;
		float h = 304;
		m_Modal = new VBT_Modal(m_Menu, w, h);
		m_ModalKind = 1;
		m_ModalClan = row.Id;
		Widget b = m_Modal.Body;
		INT_UIF_Draw.Text(b, 28, 22, w - 90, 32, "#STR_VB_TAB_C_ADM_RENAME_T", 22, VB_C_TEXT);
		VBT.Cap(b, 28, 72, 300, "#STR_VB_TAB_C_ADM_NAME");
		m_ModName = new VBT_Input(b, 28, 94, 316, 48, "#STR_VB_TAB_C_ADM_NAME_HINT", 24, row.Title);
		VBT.Cap(b, 360, 72, 130, "#STR_VB_TAB_C_ADM_TAG");
		m_ModTag = new VBT_Input(b, 360, 94, 132, 48, "2–4", 4, row.Tag);
		m_ModErr = INT_UIF_Draw.Text(b, 28, 152, w - 56, 24, "", 14, VB_C_BAD);
		m_ModNo = new VBT_Button(b, w - 328, h - 68, 140, 44, "#STR_VB_TAB_CANCEL", false, "", m_Menu.GetHitHandler());
		m_ModYes = new VBT_Button(b, w - 178, h - 68, 150, 44, "#STR_VB_TAB_C_ADM_SAVE", true, "", m_Menu.GetHitHandler(), 14);
	}

	protected void SaveRename()
	{
		string name = m_ModName.Get();
		string tag = m_ModTag.Get();
		if (name.LengthUtf8() < 3)
		{
			m_ModErr.SetText("#STR_VB_TAB_C_ADM_ERR_NAME");
			return;
		}
		if (tag.Length() < 2)
		{
			m_ModErr.SetText("#STR_VB_TAB_C_ADM_ERR_TAG");
			return;
		}
		array<ref VB_ClanRow> rows = VB_TabletClient.Get().Clans;
		foreach (VB_ClanRow other : rows)
		{
			if (other.Id == m_ModalClan)
				continue;
			if (other.Title == name)
			{
				m_ModErr.SetText("#STR_VB_TAB_C_ADM_ERR_NAME_BUSY");
				return;
			}
			if (other.Tag == tag)
			{
				m_ModErr.SetText("#STR_VB_TAB_C_ADM_ERR_TAG_BUSY");
				return;
			}
		}
		VB_TabletClient client = VB_TabletClient.Get();
		string clan = m_ModalClan;
		ScriptRPC rpc = client.Begin(VB_TabletOp.ADMIN_RENAME);
		rpc.Write(clan);
		rpc.Write(name);
		rpc.Write(tag);
		client.Post(rpc);
	}

	// Смена лидера: для клана, чей лидер пропал. Состав приходит от сервера.
	protected void OpenLeader(VB_ClanRow row)
	{
		float w = 460;
		float h = 236;
		m_Modal = new VBT_Modal(m_Menu, w, h);
		m_ModalKind = 2;
		m_ModalClan = row.Id;
		m_WaitMembers = true;
		INT_UIF_Draw.Text(m_Modal.Body, 28, 22, w - 90, 32, "#STR_VB_TAB_C_ADM_LEADER_T", 22, VB_C_TEXT);
		VB_TabletClient client = VB_TabletClient.Get();
		client.AdminClan = "";
		string clan = row.Id;
		ScriptRPC rpc = client.Begin(VB_TabletOp.ADMIN_MEMBERS);
		rpc.Write(clan);
		client.Post(rpc);
	}

	protected void PaintLeader()
	{
		m_WaitMembers = false;
		VB_TabletClient client = VB_TabletClient.Get();
		VB_ClanRow row = VBA_Util.FindClan(m_ModalClan);
		if (!row || !m_Modal)
			return;
		float w = 460;
		float h = 236;
		Widget b = m_Modal.Body;
		array<string> names = new array<string>();
		m_ModUids = new array<string>();
		for (int i = 0; i < client.AdminUids.Count(); i++)
		{
			if (client.AdminLeaders.Get(i))
				continue;
			names.Insert(client.AdminNames.Get(i));
			m_ModUids.Insert(client.AdminUids.Get(i));
		}
		if (names.Count() == 0)
		{
			INT_UIF_Draw.Text(b, 28, 76, w - 56, 32, "#STR_VB_TAB_C_ADM_NOBODY", 15, VB_C_DIM2);
			return;
		}
		string now = VBT.T("#STR_VB_TAB_C_ADM_NOW") + " " + VBT.Fit(row.Leader, 300, 14);
		INT_UIF_Draw.Text(b, 28, 60, w - 56, 24, now, 14, VB_C_DIM);
		m_ModList = new VBT_Dropdown(b, 28, 92, w - 56, 44, names, 0);
		m_ModNo = new VBT_Button(b, w - 328, h - 68, 140, 44, "#STR_VB_TAB_CANCEL", false, "", m_Menu.GetHitHandler());
		m_ModYes = new VBT_Button(b, w - 178, h - 68, 150, 44, "#STR_VB_TAB_C_ADM_ASSIGN", true, "", m_Menu.GetHitHandler(), 14);
	}

	protected void SaveLeader()
	{
		if (!m_ModList || m_ModList.Selected < 0 || m_ModList.Selected >= m_ModUids.Count())
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		string clan = m_ModalClan;
		string uid = m_ModUids.Get(m_ModList.Selected);
		ScriptRPC rpc = client.Begin(VB_TabletOp.ADMIN_LEADER);
		rpc.Write(clan);
		rpc.Write(uid);
		client.Post(rpc);
	}

	protected void AddWord()
	{
		string word = m_Word.Get();
		if (word == "")
			return;
		m_Word.Set("");
		VB_TabletClient client = VB_TabletClient.Get();
		ScriptRPC rpc = client.Begin(VB_TabletOp.ADMIN_BAN_ADD);
		rpc.Write(word);
		client.Post(rpc);
	}

	protected void DelWord(string word)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		string w = word;
		ScriptRPC rpc = client.Begin(VB_TabletOp.ADMIN_BAN_DEL);
		rpc.Write(w);
		client.Post(rpc);
	}

	override void OnInput(VBT_Input field)
	{
		if (!m_ModTag || field != m_ModTag)
			return;
		// Тег: только заглавная латиница и цифры.
		string raw = field.Get();
		string up = raw + "";
		up.ToUpper();
		string allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		string clean = "";
		int n = up.Length();
		for (int i = 0; i < n; i++)
		{
			string ch = up.Get(i);
			if (allowed.IndexOf(ch) >= 0)
				clean = clean + ch;
		}
		if (clean != raw)
			field.Set(clean);
	}

	override void OnSubmit(VBT_Input field)
	{
		if (m_Word && field == m_Word)
			AddWord();
	}

	override void OnAnswer(int tag, bool yes, string text)
	{
		if (tag != TAG_DISSOLVE || !yes)
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		string clan = s_Sel;
		string word = text;
		ScriptRPC rpc = client.Begin(VB_TabletOp.ADMIN_DISSOLVE);
		rpc.Write(clan);
		rpc.Write(word);
		client.Post(rpc);
	}

	override void OnResult(int code)
	{
		if (!m_Modal || !m_Modal.IsOpen())
			return;
		m_ModSentAt = -100;
		if (VBK.ResultIsGood(code))
		{
			CloseModal();
			m_Menu.RequestRepaint();
		}
		else if (m_ModErr)
			m_ModErr.SetText(VBK.ResultText(code));
		// У окна смены лидера нет строки ошибки: клан пропал или отказ, окно закрывается (тост уже показан).
		else
			CloseModal();
	}

	// Пока открыто окно или набрано слово, экран не перестраиваем: иначе пропадёт ввод.
	override bool OnData()
	{
		if (m_Modal && m_Modal.IsOpen())
		{
			if (m_WaitMembers && VB_TabletClient.Get().AdminClan == m_ModalClan)
				PaintLeader();
			return true;
		}
		return m_Word && m_Word.Get() != "";
	}

	override void Leave()
	{
		CloseModal();
	}

	override bool OnHit(Widget w)
	{
		if (m_Modal && m_Modal.IsOpen())
		{
			if (m_Modal.IsDismiss(w) || (m_ModNo && w == m_ModNo.Hit))
				CloseModal();
			else if (m_ModYes && w == m_ModYes.Hit)
			{
				// Один запрос до ответа сервера: второй клик не шлёт повтор.
				float now = GetGame().GetTickTime();
				if (now - m_ModSentAt < 3)
					return true;
				m_ModSentAt = now;
				if (m_ModalKind == 1)
					SaveRename();
				else
					SaveLeader();
			}
			return true;
		}
		int i = m_RowHits.Find(w);
		if (i >= 0)
		{
			s_Sel = m_RowIds.Get(i);
			m_Menu.RequestRepaint();
			return true;
		}
		i = m_ChipHits.Find(w);
		if (i >= 0)
		{
			DelWord(m_ChipWords.Get(i));
			return true;
		}
		if (w == m_BtnWord.Hit)
		{
			AddWord();
			return true;
		}
		VB_ClanRow row = VBA_Util.FindClan(s_Sel);
		if (!row || !m_BtnRename)
			return false;
		if (w == m_BtnRename.Hit)
		{
			OpenRename(row);
			return true;
		}
		if (w == m_BtnLeader.Hit)
		{
			OpenLeader(row);
			return true;
		}
		if (w == m_BtnDissolve.Hit)
		{
			string text = VBA_Util.Sub("#STR_VB_TAB_C_ADM_DISSOLVE_TEXT", "{name}", row.Title);
			m_Menu.ConfirmWord(TAG_DISSOLVE, "#STR_VB_TAB_C_ADM_DISSOLVE_T", text, row.Title, "#STR_VB_TAB_C_ADM_DISSOLVE");
			return true;
		}
		return false;
	}

	override bool OnHover(Widget w, bool enter)
	{
		if (m_Modal && m_Modal.IsOpen())
		{
			if (m_ModYes && w == m_ModYes.Hit)
				m_ModYes.Hover(enter);
			else if (m_ModNo && w == m_ModNo.Hit)
				m_ModNo.Hover(enter);
			return true;
		}
		int i = m_RowHits.Find(w);
		if (i >= 0)
		{
			if (m_RowIds.Get(i) != s_Sel)
			{
				int fill = 0;
				if (enter)
					fill = VB_C_PANEL2;
				m_RowBg.Get(i).SetFill(fill);
			}
			return true;
		}
		i = m_ChipHits.Find(w);
		if (i >= 0)
		{
			int col = VB_C_DIM;
			if (enter)
				col = VB_C_BAD;
			m_ChipIcons.Get(i).SetColor(col);
			return true;
		}
		if (w == m_BtnWord.Hit)
		{
			m_BtnWord.Hover(enter);
			return true;
		}
		if (!m_BtnRename)
			return false;
		if (w == m_BtnRename.Hit)
		{
			m_BtnRename.Hover(enter);
			return true;
		}
		if (w == m_BtnLeader.Hit)
		{
			m_BtnLeader.Hover(enter);
			return true;
		}
		if (w == m_BtnDissolve.Hit)
		{
			m_BtnDissolve.Hover(enter);
			return true;
		}
		return false;
	}
}
