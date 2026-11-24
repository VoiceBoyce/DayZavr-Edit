// Мелкие детали экранов новостей, чата, рангов и настроек.
class VBB
{
	// Текст из таблицы с подстановкой {1} и {2}.
	static string Sub(string key, string a, string b = "")
	{
		string s = VBT.T(key);
		s.Replace("{1}", a);
		s.Replace("{2}", b);
		return s;
	}

	// Ник игрока так, как его пишет сервер в чате и в составе клана.
	static string MyName()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetIdentity())
			return player.GetIdentity().GetName();
		return VB_TabletClient.Get().Me.PlayerName;
	}

	// Кнопка недоступна: не нажимается и приглушена, как .btn.off в макете (прозрачность 42%).
	static void Disable(Widget p, VBT_Button btn, float x, float y, float w, float h)
	{
		btn.Hit.Show(false);
		INT_UIF_Draw.Line(p, x, y, w, h, INT_UIF_Color.WithAlpha(VB_C_PANEL, 148));
	}

	// Перенос текста по ширине: длинное слово без пробелов режется.
	static void Wrap(string s, float w, float size, array<string> res)
	{
		res.Clear();
		array<string> words = new array<string>();
		s.Split(" ", words);
		string line = "";
		foreach (string src : words)
		{
			string word = src + "";
			while (INT_UIF_Draw.TextWidth(word, size) > w)
			{
				int total = word.LengthUtf8();
				int fit = Math.Max(1, w / (size * 0.53));
				if (line != "")
				{
					res.Insert(line);
					line = "";
				}
				res.Insert(word.SubstringUtf8(0, fit));
				int tail = total - fit;
				word = word.SubstringUtf8(fit, tail);
			}
			if (word == "")
				continue;
			string both = word;
			if (line != "")
				both = line + " " + word;
			if (INT_UIF_Draw.TextWidth(both, size) > w)
			{
				res.Insert(line);
				line = word;
			}
			else
				line = both;
		}
		if (line != "")
			res.Insert(line);
	}

	// Перенос с абзацами: Enter в тексте — новая строка, пустая строка между абзацами сохраняется
	// (Split пустые куски выбрасывает, поэтому делим сами). p — виджет для замера шрифта.
	static void WrapText(Widget p, string s, float w, float size, array<string> res)
	{
		res.Clear();
		string t = s + "";
		t.Replace("\r", "");
		// Оценка Wrap (0,53 высоты на знак) мала для настоящего шрифта: поправка по замеру образца этого текста.
		string flat = t + "";
		flat.Replace("\n", " ");
		int n = flat.LengthUtf8();
		if (n > 120)
			n = 120;
		string sample = flat.SubstringUtf8(0, n);
		float est = INT_UIF_Draw.TextWidth(sample, size);
		float real = VBT.TextW(p, sample, size, false);
		float k = 1;
		if (est > 0 && real > est)
			k = real / est;
		float room = w / (k * 1.04);
		array<string> part = new array<string>();
		while (true)
		{
			int at = t.IndexOf("\n");
			string para = t;
			if (at >= 0)
				para = t.Substring(0, at);
			Wrap(para, room, size, part);
			if (part.Count() == 0)
				res.Insert("");
			foreach (string ln : part)
				res.Insert(ln);
			if (at < 0)
				break;
			t = t.Substring(at + 1, t.Length() - at - 1);
		}
		while (res.Count() > 0 && res.Get(res.Count() - 1) == "")
			res.Remove(res.Count() - 1);
	}

	protected static string Stamp(int d, int m)
	{
		string dd;
		string mm;
		VBT.Pad2(dd, d);
		VBT.Pad2(mm, m);
		return dd + "." + mm;
	}

	protected static int MonthDays(int m, int y)
	{
		array<int> all = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		if (m == 2 && y % 4 == 0)
			return 29;
		return all.Get(m - 1);
	}

	// Подпись дня: сегодня, вчера, дальше дата. Сервер присылает день как ДД.ММ.
	static string DayLabel(string day)
	{
		if (day.Length() < 5)
			return day;
		int y;
		int m;
		int d;
		GetYearMonthDay(y, m, d);
		if (day == Stamp(d, m))
			return VBT.T("#STR_VB_TAB_B_TODAY");
		int pm = m;
		int pd = d - 1;
		if (pd < 1)
		{
			pm = m - 1;
			if (pm < 1)
				pm = 12;
			pd = MonthDays(pm, y);
		}
		if (day == Stamp(pd, pm))
			return VBT.T("#STR_VB_TAB_B_YESTERDAY");
		string sd = day.Substring(0, 2);
		string sm = day.Substring(3, 2);
		int nd = sd.ToInt();
		int nm = sm.ToInt();
		if (nm < 1 || nm > 12)
			return day;
		string month = VBT.T("#STR_VB_TAB_B_MON_" + nm.ToString());
		return nd.ToString() + " " + month;
	}
}

// Новости клана: читают все участники, пишут те, чей ранг это разрешает. Закреплённая одна и всегда сверху.
class VB_TabletPageNews extends VB_TabletPage
{
	protected static const int TAG_DELETE = 1;
	protected static const int TITLE_MAX = 60;
	protected static const int TEXT_MAX = 400;
	protected static const int ACT_PIN = 0;
	protected static const int ACT_EDIT = 1;
	protected static const int ACT_DEL = 2;

	// Форма живёт в статике: перестроение экрана создаёт страницу заново.
	protected static int s_FormId;
	protected static int s_FormTag;
	protected static string s_FormTitle;
	protected static string s_FormText;
	protected static bool s_FormPin;
	protected static bool s_Pending;
	protected static string s_SentTitle;
	protected static string s_SentText;
	protected static int s_DelId;
	protected static float s_AskedAt = -100;
	protected static string s_FormClan;		// клан, к которому относится черновик: в другом клане форма пустая
	protected static float s_SentAt = -100;
	// Новость, открытая на чтение целиком (0 — лента). s_WantId: открыть при входе (плитка на странице клана);
	// s_Keep: страницу перестраивает она сама — открытая новость остаётся, при входе из меню — лента.
	protected static int s_OpenId;
	protected static int s_WantId;
	protected static bool s_Keep;

	protected bool m_Ed;
	protected ref array<Widget> m_CardHits = new array<Widget>();
	protected ref array<int> m_CardIds = new array<int>();
	protected ref array<ref INT_UIF_Surface> m_CardBoxes = new array<ref INT_UIF_Surface>();
	protected ref VBT_Button m_Back;
	protected Widget m_Left;
	protected ref array<Widget> m_ActHits = new array<Widget>();
	protected ref array<int> m_ActKinds = new array<int>();
	protected ref array<int> m_ActIds = new array<int>();
	protected ref array<ImageWidget> m_ActIcons = new array<ImageWidget>();
	protected ref array<Widget> m_TagHits = new array<Widget>();
	protected ref VBT_Input m_Title;
	protected ref VBT_Area m_Area;
	protected ref VBT_Switch m_Pin;
	protected ref VBT_Button m_Save;
	protected ref VBT_Button m_Cancel;
	protected TextWidget m_CountTitle;
	protected TextWidget m_CountText;

	protected static int TagColor(int i)
	{
		array<int> all = {VB_C_ACCENT, VB_C_GOLD, VB_C_OK, VB_C_BAD};
		return all.Get(Math.Clamp(i, 0, 3));
	}

	protected static void Blank()
	{
		s_Pending = false;
		s_FormId = 0;
		s_FormTag = 0;
		s_FormTitle = "";
		s_FormText = "";
		s_FormPin = false;
	}

	static void OpenLater(int id)
	{
		s_WantId = id;
	}

	override void Build()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (!client.Clan)
		{
			m_Menu.GoLater("profile");
			return;
		}
		if (!s_Keep)
			s_OpenId = s_WantId;
		s_WantId = 0;
		s_Keep = false;
		// Черновик прежнего клана не переносится: «правка №5» перезаписала бы новость №5 другого клана.
		if (s_FormClan != client.Clan.Id)
		{
			Blank();
			s_FormClan = client.Clan.Id;
		}
		float now = GetGame().GetTickTime();
		if (now - s_AskedAt > 5)
		{
			s_AskedAt = now;
			client.Request(VB_TabletOp.CLAN_NEWS);
		}
		m_Ed = client.Can(VB_ClanPerm.NEWS);
		m_Left = INT_UIF_Draw.Frame(m_Root, 0, 0, 1172, 720);
		PaintLeft();
		if (m_Ed)
			BuildForm();
	}

	protected void PaintLeft()
	{
		INT_UIF_Widgets.ClearChildren(m_Left);
		m_ActHits.Clear();
		m_ActKinds.Clear();
		m_ActIds.Clear();
		m_ActIcons.Clear();
		m_CardHits.Clear();
		m_CardIds.Clear();
		m_CardBoxes.Clear();
		m_Back = null;
		VB_TabletClient client = VB_TabletClient.Get();
		array<ref VB_ClanNews> all = client.News;
		float w = 1172;
		if (m_Ed)
			w = 760;
		if (all.Count() == 0)
		{
			Widget f = VBT.Card(m_Left, 0, 0, w, 720);
			VBT.Ico(f, w * 0.5 - 30, 250, 60, "news", VB_C_DIM2);
			INT_UIF_Draw.Text(f, 0, 326, w, 34, "#STR_VB_TAB_B_NEWS_NONE", 24, VB_C_TEXT, INT_UIF_Align.CENTER);
			string hint = "#STR_VB_TAB_B_NEWS_NONE_READ";
			if (m_Ed)
				hint = "#STR_VB_TAB_B_NEWS_NONE_WRITE";
			INT_UIF_Draw.Text(f, 0, 366, w, 26, hint, 16, VB_C_DIM, INT_UIF_Align.CENTER);
			return;
		}
		// Открытая новость удалена или пропала из списка — снова лента.
		if (s_OpenId != 0)
		{
			VB_ClanNews opened = Find(s_OpenId);
			if (opened)
			{
				Reader(opened, w);
				return;
			}
			s_OpenId = 0;
		}
		VB_ClanNews pinned = null;
		int rest = 0;
		foreach (VB_ClanNews item : all)
		{
			if (item.Pin && !pinned)
				pinned = item;
			else
				rest++;
		}
		float y = 0;
		if (pinned)
		{
			Hero(pinned, w);
			y = 206;
		}
		// Лента прокручивается, полоса прокрутки не должна закрывать значки действий.
		float view = 720 - y;
		float rowW = w;
		if (rest * 104 - 8 > view)
			rowW = w - 14;
		ScrollWidget sw = VBK.Scroll(m_Left, 0, y, w, view);
		Widget content = INT_UIF_Draw.ScrollContent(sw);
		INT_UIF_Draw.SetScrollContent(sw, rest * 104);
		float rowY = 0;
		foreach (VB_ClanNews row : all)
		{
			if (row == pinned)
				continue;
			Row(content, row, rowY, rowW);
			rowY += 104;
		}
	}

	protected float TagPill(Widget p, float x, float y, int tag)
	{
		return VBK.Pill(p, x, y, "#STR_VB_TAB_B_NTU_" + tag.ToString(), TagColor(tag));
	}

	protected void Actions(Widget p, float w, float y, VB_ClanNews n)
	{
		array<string> icons = {"pin", "edit", "trash"};
		for (int i = 0; i < 3; i++)
		{
			float x = w - 116 + i * 34;
			int col = VB_C_DIM;
			if (i == ACT_PIN && n.Pin)
				col = VB_C_ACCENT;
			ImageWidget ic = VBT.Ico(p, x + 5, y + 5, 20, icons.Get(i), col);
			m_ActHits.Insert(HitAt(p, x, y, 30, 30));
			m_ActKinds.Insert(i);
			m_ActIds.Insert(n.NewsId);
			m_ActIcons.Insert(ic);
		}
	}

	// Вся карточка — кнопка «читать целиком»; значки действий создаются позже и лежат поверх.
	protected void CardHit(Widget f, float w, float h, VB_ClanNews n, INT_UIF_Surface box)
	{
		m_CardHits.Insert(HitAt(f, 0, 0, w, h));
		m_CardIds.Insert(n.NewsId);
		m_CardBoxes.Insert(box);
	}

	protected void Hero(VB_ClanNews n, float w)
	{
		Widget f = INT_UIF_Draw.Frame(m_Left, 0, 0, w, 190);
		INT_UIF_Surface box = INT_UIF_Surface.Box(f, 0, 0, w, 190, 10, VB_C_PANEL, VB_C_LINE);
		VBT.Glow(f, w, 190);
		VBT.Corners(f, w, 190, VB_C_ACCENT, 16);
		VBT.Ico(f, 24, 23, 18, "pin", VB_C_ACCENT);
		VBT.Cap(f, 50, 24, 160, "#STR_VB_TAB_B_PINNED", VB_C_ACCENT);
		TagPill(f, 170, 20, n.TagIdx);
		INT_UIF_Draw.Text(f, 24, 54, w - 48, 42, VBT.Fit(n.Head, w - 48, 30), 30, VB_C_TEXT);
		// Две строки текста с ровным шагом; не влезло — «...» и «Читать полностью».
		array<string> lines = new array<string>();
		VBB.WrapText(f, n.Body, w - 48, 16, lines);
		bool more = lines.Count() > 2;
		for (int i = 0; i < lines.Count() && i < 2; i++)
		{
			string ln = lines.Get(i);
			if (i == 1 && more)
				ln = VBT.Fit(ln + " " + lines.Get(2) + "...", w - 48, 16);
			INT_UIF_Draw.Text(f, 24, 98 + i * 24, w - 48, 24, ln, 16, VB_C_DIM);
		}
		INT_UIF_Draw.Text(f, 24, 156, w - 48, 22, n.Author + " · " + n.Time, 13, VB_C_DIM2);
		if (more)
			INT_UIF_Draw.Text(f, 24, 156, w - 48, 22, "#STR_VB_TAB_B_NEWS_MORE", 13, VB_C_ACCENT, INT_UIF_Align.RIGHT);
		CardHit(f, w, 190, n, box);
		if (m_Ed)
			Actions(f, w - 10, 16, n);
	}

	protected void Row(Widget p, VB_ClanNews n, float y, float w)
	{
		Widget f = INT_UIF_Draw.Frame(p, 0, y, w, 96);
		INT_UIF_Surface box = INT_UIF_Surface.Box(f, 0, 0, w, 96, 10, VB_C_PANEL, VB_C_LINE);
		INT_UIF_Surface.Box(f, 0, 14, 3, 68, 2, TagColor(n.TagIdx));
		float tw = TagPill(f, 20, 14, n.TagIdx);
		float room = w - tw - 210;
		INT_UIF_Draw.Text(f, 20 + tw + 12, 12, room, 28, VBT.Fit(n.Head, room, 19), 19, VB_C_TEXT);
		string one = n.Body + "";
		one.Replace("\r", "");
		one.Replace("\n", " ");
		string fit = VBT.Fit(one, w - 40, 15);
		INT_UIF_Draw.Text(f, 20, 44, w - 40, 24, fit, 15, VB_C_DIM);
		INT_UIF_Draw.Text(f, 20, 68, w - 40, 20, n.Author + " · " + n.Time, 13, VB_C_DIM2);
		if (fit != one)
			INT_UIF_Draw.Text(f, 20, 68, w - 40, 20, "#STR_VB_TAB_B_NEWS_MORE", 13, VB_C_ACCENT, INT_UIF_Align.RIGHT);
		CardHit(f, w, 96, n, box);
		if (m_Ed)
			Actions(f, w - 8, 10, n);
	}

	// Новость целиком: «К новостям», тип, заголовок и весь текст с переносом; длинный текст прокручивается.
	protected void Reader(VB_ClanNews n, float w)
	{
		Widget f = INT_UIF_Draw.Frame(m_Left, 0, 0, w, 720);
		INT_UIF_Surface.Box(f, 0, 0, w, 720, 10, VB_C_PANEL, VB_C_LINE);
		m_Back = new VBT_Button(f, 24, 20, 190, 38, "#STR_VB_TAB_B_NEWS_BACK", false, "", m_Menu.GetHitHandler(), 14);
		float x = 232;
		if (n.Pin)
		{
			VBT.Ico(f, x, 30, 18, "pin", VB_C_ACCENT);
			x += 28;
		}
		TagPill(f, x, 24, n.TagIdx);
		if (m_Ed)
			Actions(f, w - 10, 24, n);
		array<string> head = new array<string>();
		VBB.WrapText(f, n.Head, w - 48, 28, head);
		float y = 80;
		foreach (string hl : head)
		{
			INT_UIF_Draw.Text(f, 24, y, w - 48, 38, hl, 28, VB_C_TEXT);
			y += 38;
		}
		INT_UIF_Draw.Text(f, 24, y + 2, w - 48, 22, n.Author + " · " + n.Time, 13, VB_C_DIM2);
		y += 34;
		INT_UIF_Draw.Line(f, 24, y, w - 48, 1, VB_C_LINE);
		y += 16;
		array<string> body = new array<string>();
		VBB.WrapText(f, n.Body, w - 62, 17, body);
		float view = 720 - y - 20;
		ScrollWidget sw = VBK.Scroll(f, 24, y, w - 48, view);
		Widget content = INT_UIF_Draw.ScrollContent(sw);
		INT_UIF_Draw.SetScrollContent(sw, body.Count() * 27);
		float ly = 0;
		foreach (string bl : body)
		{
			INT_UIF_Draw.Text(content, 0, ly, w - 62, 27, bl, 17, VB_C_TEXT);
			ly += 27;
		}
	}

	protected void BuildForm()
	{
		Widget f = INT_UIF_Draw.Frame(m_Root, 776, 0, 396, 720);
		INT_UIF_Surface.Box(f, 0, 0, 396, 720, 10, VB_C_PANEL, VB_C_LINE);
		bool editing = s_FormId != 0;
		string head = "#STR_VB_TAB_B_NEWS_NEW";
		string yes = "#STR_VB_TAB_B_PUBLISH";
		if (editing)
		{
			head = "#STR_VB_TAB_B_NEWS_EDIT";
			yes = "#STR_VB_TAB_B_SAVE";
		}
		VBT.Cap(f, 24, 20, 348, head);
		VBT.Cap(f, 24, 54, 348, "#STR_VB_TAB_B_NEWS_TYPE");
		m_TagHits.Clear();
		for (int i = 0; i < 4; i++)
		{
			int col = i % 2;
			int row = i / 2;
			float x = 24 + col * 178;
			float y = 78 + row * 44;
			bool on = s_FormTag == i;
			int fill = VB_C_PANEL2;
			int border = VB_C_LINE2;
			int ink = VB_C_DIM;
			if (on)
			{
				fill = VB_C_PANEL3;
				border = TagColor(i);
				ink = VB_C_TEXT;
			}
			INT_UIF_Surface.Box(f, x, y, 170, 38, 8, fill, border);
			INT_UIF_Draw.Text(f, x, y, 170, 38, "#STR_VB_TAB_B_NT_" + i.ToString(), 14, ink, INT_UIF_Align.CENTER);
			m_TagHits.Insert(HitAt(f, x, y, 170, 38));
		}
		VBT.Cap(f, 24, 178, 240, "#STR_VB_TAB_B_NEWS_HEAD");
		m_CountTitle = INT_UIF_Draw.Text(f, 272, 178, 100, 18, "", 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
		m_Title = new VBT_Input(f, 24, 200, 348, 48, "#STR_VB_TAB_B_NEWS_HEAD_HINT", TITLE_MAX, s_FormTitle);
		VBT.Cap(f, 24, 262, 240, "#STR_VB_TAB_B_NEWS_TEXT");
		m_CountText = INT_UIF_Draw.Text(f, 272, 262, 100, 18, "", 13, VB_C_DIM2, INT_UIF_Align.RIGHT);
		m_Area = new VBT_Area(f, 24, 284, 348, 250, "#STR_VB_TAB_B_NEWS_TEXT_HINT", s_FormText);
		ShowCounts();
		INT_UIF_Draw.Text(f, 24, 546, 270, 36, "#STR_VB_TAB_B_NEWS_PIN", 15, VB_C_TEXT);
		m_Pin = new VBT_Switch(f, 328, 552, s_FormPin, m_Menu.GetHitHandler());
		m_Save = new VBT_Button(f, 24, 596, 348, 52, yes, true, "check", m_Menu.GetHitHandler(), 16);
		if (editing)
			m_Cancel = new VBT_Button(f, 24, 656, 348, 44, "#STR_VB_TAB_CANCEL", false, "", m_Menu.GetHitHandler(), 14);
	}

	protected void ShowCounts()
	{
		int a = s_FormTitle.LengthUtf8();
		int b = s_FormText.LengthUtf8();
		int maxA = TITLE_MAX;
		int maxB = TEXT_MAX;
		m_CountTitle.SetText(a.ToString() + "/" + maxA.ToString());
		m_CountText.SetText(b.ToString() + "/" + maxB.ToString());
	}

	protected void Repaint()
	{
		s_Keep = true;
		SetFocus(null);
		m_Menu.RequestRepaint();
	}

	protected VB_ClanNews Find(int id)
	{
		array<ref VB_ClanNews> all = VB_TabletClient.Get().News;
		foreach (VB_ClanNews n : all)
		{
			if (n.NewsId == id)
				return n;
		}
		return null;
	}

	protected void Save()
	{
		string head = m_Title.Get();
		string body = m_Area.Get();
		if (head == "" || body == "")
		{
			INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_B_NEWS_NEED"));
			return;
		}
		if (VB_TabletBridge.Censored(head) || VB_TabletBridge.Censored(body))
			return;
		// Пока ответа нет, второй клик не отправляет ту же новость ещё раз (сервер создал бы дубль).
		float now = GetGame().GetTickTime();
		if (s_Pending && now - s_SentAt < 5)
			return;
		s_SentAt = now;
		VB_TabletClient client = VB_TabletClient.Get();
		int id = s_FormId;
		int tag = s_FormTag;
		bool pin = s_FormPin;
		ScriptRPC rpc = client.Begin(VB_TabletOp.NEWS_SAVE);
		rpc.Write(id);
		rpc.Write(tag);
		rpc.Write(head);
		rpc.Write(body);
		rpc.Write(pin);
		client.Post(rpc);
		s_Pending = true;
		s_SentTitle = head;
		s_SentText = body;
	}

	protected void RunAction(int kind, int id)
	{
		VB_ClanNews n = Find(id);
		if (!n)
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		if (kind == ACT_PIN)
		{
			ScriptRPC rpc = client.Begin(VB_TabletOp.NEWS_PIN);
			rpc.Write(id);
			client.Post(rpc);
		}
		else if (kind == ACT_EDIT)
		{
			s_FormId = n.NewsId;
			s_FormTag = n.TagIdx;
			s_FormTitle = n.Head;
			s_FormText = n.Body;
			s_FormPin = n.Pin;
			Repaint();
		}
		else
		{
			s_DelId = id;
			m_Menu.Ask(TAG_DELETE, "#STR_VB_TAB_B_NEWS_DEL_Q", VBB.Sub("#STR_VB_TAB_B_NEWS_DEL_T", n.Head), "#STR_VB_TAB_B_DELETE", true);
		}
	}

	override void OnAnswer(int tag, bool yes, string text)
	{
		if (tag != TAG_DELETE || !yes)
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		int id = s_DelId;
		ScriptRPC rpc = client.Begin(VB_TabletOp.NEWS_DELETE);
		rpc.Write(id);
		client.Post(rpc);
		if (s_FormId == id)
		{
			Blank();
			Repaint();
		}
	}

	override void OnInput(VBT_Input field)
	{
		if (field != m_Title)
			return;
		s_FormTitle = m_Title.Edit.GetText();
		ShowCounts();
	}

	override void OnArea(VBT_Area field)
	{
		string s;
		field.Edit.GetText(s);
		if (s.LengthUtf8() > TEXT_MAX)
		{
			s = s.SubstringUtf8(0, TEXT_MAX);
			field.Edit.SetText(s);
		}
		s_FormText = s;
		ShowCounts();
	}

	override void OnResult(int code)
	{
		if (!VBK.ResultIsGood(code))
			s_Pending = false;
	}

	override bool OnData()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (!client.Clan)
		{
			m_Menu.GoLater("profile");
			return true;
		}
		if (client.Can(VB_ClanPerm.NEWS) != m_Ed)
			return false;
		// Сервер отвечает на сохранение только новым списком: форма очищается, когда новость в нём появилась.
		if (s_Pending)
		{
			array<ref VB_ClanNews> all = client.News;
			foreach (VB_ClanNews n : all)
			{
				if (n.Head == s_SentTitle && n.Body == s_SentText)
				{
					s_Pending = false;
					if (s_FormId != 0)
						INT_UIF_Toast.Success(VBT.T("#STR_VB_TAB_B_NEWS_UPDATED"));
					else
						INT_UIF_Toast.Success(VBT.T("#STR_VB_TAB_B_NEWS_POSTED"));
					Blank();
					SetFocus(null);
					return false;
				}
			}
		}
		PaintLeft();
		return true;
	}

	override bool OnHit(Widget w)
	{
		int i = m_ActHits.Find(w);
		if (i >= 0)
		{
			RunAction(m_ActKinds.Get(i), m_ActIds.Get(i));
			return true;
		}
		// Открыть/закрыть чтение — через перестроение: виджет, по которому щёлкнули, внутри обработчика не удаляем.
		if (m_Back && w == m_Back.Hit)
		{
			s_OpenId = 0;
			Repaint();
			return true;
		}
		i = m_CardHits.Find(w);
		if (i >= 0)
		{
			s_OpenId = m_CardIds.Get(i);
			Repaint();
			return true;
		}
		i = m_TagHits.Find(w);
		if (i >= 0)
		{
			s_FormTag = i;
			Repaint();
			return true;
		}
		if (m_Pin && w == m_Pin.Hit)
		{
			m_Pin.Toggle();
			s_FormPin = m_Pin.On;
			return true;
		}
		if (m_Save && w == m_Save.Hit)
		{
			Save();
			return true;
		}
		if (m_Cancel && w == m_Cancel.Hit)
		{
			Blank();
			Repaint();
			return true;
		}
		return false;
	}

	override bool OnHover(Widget w, bool enter)
	{
		int i = m_ActHits.Find(w);
		if (i >= 0)
		{
			int kind = m_ActKinds.Get(i);
			VB_ClanNews n = Find(m_ActIds.Get(i));
			if (kind == ACT_PIN && n && n.Pin)
				return true;
			int col = VB_C_DIM;
			if (enter)
			{
				col = VB_C_TEXT;
				if (kind == ACT_DEL)
					col = VB_C_BAD;
			}
			m_ActIcons.Get(i).SetColor(col);
			return true;
		}
		int card = m_CardHits.Find(w);
		if (card >= 0)
		{
			int edge = VB_C_LINE;
			if (enter)
				edge = VB_C_ACCENT;
			m_CardBoxes.Get(card).SetBorder(edge);
			return true;
		}
		if (m_Back && w == m_Back.Hit)
		{
			m_Back.Hover(enter);
			return true;
		}
		if (m_Save && w == m_Save.Hit)
		{
			m_Save.Hover(enter);
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
