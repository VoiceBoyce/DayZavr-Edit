// INT Quests (client) — banners (owner decision O17, design B: a paper strip with a leather block and a rubber stamp
// that slides in from the top and leaves after a few seconds). Notes from the server are queued in the RPC handler
// and drawn in Tick; one banner at a time, each with its sound.

class INT_QST_BannerItem
{
	int Kind;
	int QuestId;
	int A;
	string Text;
}

class INT_QST_Banner
{
	static const float W = 600;
	static const float H = 86;
	static const float TOP = 62;
	static const float SHOW = 3.4;
	static const int QUEUE_MAX = 8;

	protected static ref INT_QST_Banner s_Instance;
	static int s_Shown;			// self-test counter

	protected ref array<ref INT_QST_BannerItem> m_Queue = new array<ref INT_QST_BannerItem>();
	protected Widget m_Root;
	protected Widget m_Strip;
	protected float m_Left;
	protected bool m_Leaving;
	protected bool m_Hooked;

	static INT_QST_Banner Get()
	{
		if (!s_Instance)
			s_Instance = new INT_QST_Banner();
		return s_Instance;
	}

	static void Shutdown()
	{
		if (s_Instance)
			s_Instance.Destroy();
		s_Instance = null;
	}

	protected void Destroy()
	{
		if (m_Hooked && INT_QST_Client.s_OnNote)
			INT_QST_Client.s_OnNote.Remove(OnNote);
		m_Hooked = false;
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
		m_Strip = null;
	}

	void OnNote(int kind, int questId, int a, string text)
	{
		if (kind == INT_QST_Note.QUEST_ITEM)
			return;		// the ACCEPTED banner covers it
		INT_QST_BannerItem it = new INT_QST_BannerItem();
		it.Kind = kind;
		it.QuestId = questId;
		it.A = a;
		it.Text = text;
		// a money note right after COMPLETED of the same quest merges into it
		if (kind == INT_QST_Note.MONEY && m_Queue.Count() > 0)
		{
			INT_QST_BannerItem last = m_Queue[m_Queue.Count() - 1];
			if (last.Kind == INT_QST_Note.COMPLETED && last.QuestId == questId)
			{
				last.A = a;
				return;
			}
		}
		m_Queue.Insert(it);
		while (m_Queue.Count() > QUEUE_MAX)
			m_Queue.RemoveOrdered(0);
	}

	void Tick(float dt)
	{
		if (!m_Hooked)
		{
			INT_QST_Client.s_OnNote.Insert(OnNote);
			m_Hooked = true;
		}
		if (m_Strip)
		{
			m_Left = m_Left - dt;
			if (m_Left <= 0 && !m_Leaving)
			{
				m_Leaving = true;
				INT_UIF_Animator.Fade(m_Strip, 0, 0.35, INT_UIF_Ease.IN_QUAD);
				INT_UIF_Animator.MoveY(m_Strip, INT_UIF_Scale.Px(-30), 0.35, INT_UIF_Ease.IN_QUAD);
			}
			if (m_Left <= -0.4)
			{
				m_Strip.Unlink();
				m_Strip = null;
			}
			return;
		}
		if (m_Queue.Count() == 0)
			return;
		INT_QST_BannerItem it = m_Queue[0];
		m_Queue.RemoveOrdered(0);
		Show(it);
	}

	protected void Show(INT_QST_BannerItem it)
	{
		if (!m_Root)
		{
			m_Root = INT_UIF_Draw.Frame(null, 0, 0, 10, 10, "INT_QST_BannerRoot");
			if (!m_Root)
				return;
		}
		m_Root.SetPos(0, 0);
		int scrW = INT_UIF_Scale.ScreenW();
		int scrH = INT_UIF_Scale.ScreenH();
		m_Root.SetSize(scrW, scrH);
		string stampKey;
		int stampColor;
		string icon;
		string sound;
		Describe(it.Kind, stampKey, stampColor, icon, sound);
		string line = "";
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(it.QuestId);
		if (q)
			line = q.Name;
		if (it.Kind == INT_QST_Note.TARGET_DONE && q && it.A >= 0 && it.A < q.Targets.Count())
		{
			INT_QST_TargetDef t = q.Targets[it.A];
			line = INT_QST_Text.Target(t);
		}
		else if (it.Kind == INT_QST_Note.COMPLETED && it.A > 0)
		{
			line = line + "  ·  + " + INT_QST_Look.Money(it.A);
		}
		else if (it.Kind == INT_QST_Note.MONEY)
		{
			line = "+ " + INT_QST_Look.Money(it.A);
		}
		else if (it.Kind == INT_QST_Note.CATALOG)
		{
			line = INT_QST_Look.Tr("#STR_INT_QST_BN_CATALOG_TEXT");
		}
		else if (it.Kind == INT_QST_Note.REWARD_GROUND)
		{
			line = INT_QST_Look.Tr("#STR_INT_QST_BN_GROUND_TEXT");
		}
		else if (it.Kind == INT_QST_Note.RESET)
		{
			string again = INT_QST_Look.Tr("#STR_INT_QST_BN_RESET_TEXT");
			line = line + "  ·  " + again;
		}
		float sw = INT_UIF_Scale.ScreenW() / INT_UIF_Scale.Factor();
		float x = (sw - W) * 0.5;
		m_Strip = INT_UIF_Draw.Frame(m_Root, x, TOP, W, H, "Banner");
		INT_UIF_Surface.Shadow(m_Strip, -26, -14, W + 52, H + 44, 0x66000000);
		INT_QST_Look.PaperSheet(m_Strip, 0, 0, W, H, 0xFFDDCFAC, "Paper");
		int lea = INT_QST_Look.Leather();
		INT_UIF_Draw.Line(m_Strip, 0, 0, 72, H, lea);
		INT_QST_Look.Tex(m_Strip, 0, 0, 72, H, "int_qst_leather_ca", lea);
		int tan = INT_QST_Look.Tan();
		INT_QST_Look.Icon(m_Strip, 21, (H - 30) * 0.5, 30, icon, tan);
		INT_QST_Look.Stamp(m_Strip, 92, 12, stampKey, stampColor, 11, "check");
		float keyW = 110;
		string fit = INT_QST_Look.Fit(line, W - 92 - keyW - 10, 22);
		int ink = INT_QST_Look.Ink();
		INT_QST_Look.Heavy(m_Strip, 92, 40, W - 92 - keyW - 10, 34, fit, 22, ink);
		// [P] journal
		string key = INT_QST_Book.KeyName("UAINTQuestsJournal", "J");
		float kw = INT_UIF_Draw.TextWidth(key, 13) * 1.15 + 14;
		if (kw < 28)
			kw = 28;
		float kx = W - keyW + 4;
		int ink2 = INT_QST_Look.Ink2();
		INT_UIF_Surface.Box(m_Strip, kx, 30, kw, 26, 4, 0, ink2);
		INT_QST_Look.Heavy(m_Strip, kx, 30, kw, 26, key, 13, ink, INT_UIF_Align.CENTER);
		string jt = INT_QST_Look.Tr("#STR_INT_QST_BN_JOURNAL");
		int ink3 = INT_QST_Look.Ink3();
		INT_QST_Look.Text(m_Strip, kx + kw + 6, 30, 80, 26, jt, 13, ink3);
		m_Strip.SetAlpha(0);
		INT_UIF_Animator.Reveal(m_Strip, 0, 22, 0.32);
		m_Left = SHOW;
		m_Leaving = false;
		INT_QST_Sfx.Play(sound);
		s_Shown++;
	}

	// Stamp text, colour, icon and sound of a note kind.
	protected void Describe(int kind, out string stampKey, out int color, out string icon, out string sound)
	{
		icon = "book";
		color = INT_QST_Look.Blue();
		sound = "notify";
		switch (kind)
		{
			case INT_QST_Note.ACCEPTED:
				stampKey = "#STR_INT_QST_BN_ACCEPTED";
				sound = "stamp";
				return;
			case INT_QST_Note.TARGET_DONE:
				stampKey = "#STR_INT_QST_BN_TARGET";
				color = INT_QST_Look.Olive();
				icon = "check";
				sound = "check";
				return;
			case INT_QST_Note.READY:
				stampKey = "#STR_INT_QST_BN_READY";
				color = INT_QST_Look.Red();
				icon = "hand";
				return;
			case INT_QST_Note.COMPLETED:
				stampKey = "#STR_INT_QST_BN_COMPLETED";
				color = INT_QST_Look.Red();
				icon = "star";
				sound = "complete";
				return;
			case INT_QST_Note.FAILED:
				stampKey = "#STR_INT_QST_BN_FAILED";
				color = INT_QST_Look.RedDeep();
				icon = "warning";
				sound = "fail";
				return;
			case INT_QST_Note.CANCELLED:
				stampKey = "#STR_INT_QST_BN_CANCELLED";
				color = INT_QST_Look.Ink3();
				icon = "close";
				sound = "fail";
				return;
			case INT_QST_Note.REWARD_GROUND:
				stampKey = "#STR_INT_QST_BN_GROUND";
				color = INT_QST_Look.Red();
				icon = "box";
				return;
			case INT_QST_Note.MONEY:
				stampKey = "#STR_INT_QST_BN_MONEY";
				color = INT_QST_Look.Red();
				icon = "bank";
				sound = "complete";
				return;
			case INT_QST_Note.CATALOG:
				stampKey = "#STR_INT_QST_BN_CATALOG";
				icon = "refresh";
				return;
			case INT_QST_Note.RESET:
				stampKey = "#STR_INT_QST_BN_RESET";
				color = INT_QST_Look.Red();
				icon = "refresh";
				sound = "fail";
				return;
		}
		stampKey = "#STR_INT_QST_BN_ACCEPTED";
	}
}
