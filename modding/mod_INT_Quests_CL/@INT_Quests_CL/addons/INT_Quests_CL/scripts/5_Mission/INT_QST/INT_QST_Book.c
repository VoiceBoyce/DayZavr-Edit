// INT Quests (client) — base of the notebook windows (design B, spec.md §7). It is an INT_UIF_Menu (centring,
// open/close animation, player freeze, blur) whose chrome is replaced by a leather cover with a stitched seam, a
// paper spread with a spine shadow, a ribbon bookmark and key hints on the leather — the framework itself is not
// changed (owner rule). Esc closes on RELEASE (closing on press lets the vanilla pause open, P2P lesson); the order
// is: dropdown -> framework modal -> own question card -> window overlay -> close. Death closes the window.

class INT_QST_Book extends INT_UIF_Menu
{
	static const float BOTTOM = 58;		// cover area under the pages (key hints)
	static const float SIDE = 22;

	static const int ID_CLOSE = 9001;
	static const int ID_ASK_YES = 9002;
	static const int ID_ASK_NO = 9003;
	static const int ID_ASK_BG = 9004;

	protected ref INT_QST_Clicks m_Clicks;
	protected Widget m_Cover;
	protected Widget m_Keys;
	protected float m_KeysX;
	protected bool m_EscArmed;
	protected bool m_KeyArmed;
	protected float m_Age;			// seconds since OnShow: the P press that opened the window must not close it

	// flash line on the leather (errors, "tracker full")
	protected Widget m_Flash;
	protected float m_FlashLeft;

	// own question card (paper style confirm)
	protected Widget m_Ask;
	protected int m_AskTag;
	protected int m_AskData;
	protected ref array<Widget> m_AskDead = new array<Widget>();	// answered cards: hidden, unlinked next Update
	protected float m_AskDeadAt = -1;

	// data refresh throttle: s_OnChanged comes with every kill / progress packet; the window marks itself dirty and
	// OnDirtyRefresh runs from Update at most every DIRTY_GAP seconds (the first change after a quiet time at once)
	static const float DIRTY_GAP = 0.25;
	protected bool m_DataDirty;
	protected float m_SinceRefresh = 100;

	void INT_QST_Book()
	{
		m_Clicks = new INT_QST_Clicks();
		m_Clicks.Event_Click.Insert(OnClickId);
		m_Clicks.Event_Wheel.Insert(OnWheelId);
		m_Clicks.Event_Down.Insert(OnDownId);
	}

	override bool CloseOnEscape()
	{
		return false;
	}

	override bool HasFooter()
	{
		return false;
	}

	override string GetTitle()
	{
		return "";
	}

	// Height of the leather above the pages (title plate, tabs, portrait).
	float CoverTop()
	{
		return 110;
	}

	override float GetBodyW()
	{
		return m_WinW - SIDE * 2;
	}

	override float GetBodyH()
	{
		return m_WinH - CoverTop() - BOTTOM;
	}

	// --- hooks for the windows ----------------------------------------------------------------------------------------
	// Title plate, tabs, header buttons (on the leather above the pages).
	void BuildCoverHeader(Widget header, float w)
	{
	}

	void OnClickId(int id, int data, int button)
	{
	}

	void OnWheelId(int id, int wheel)
	{
	}

	void OnDownId(int id, int data, int button)
	{
	}

	// The question card was answered (tag = what was asked).
	void OnAnswer(int tag, int data, bool yes)
	{
	}

	// Data changed (call from the s_OnChanged handler): the refresh comes from Update, throttled.
	void MarkDirty()
	{
		m_DataDirty = true;
	}

	// Throttled refresh of the shown data (the window rebuilds what depends on the client state / catalog).
	// INT_QST_Client.s_ShapeRevision unchanged since the last refresh = only progress counters moved.
	void OnDirtyRefresh()
	{
	}

	// An own overlay (picker, form) closes first on Esc: return true when something was closed.
	bool OnEscapeOverlay()
	{
		return false;
	}

	// Key that also toggles this window (journal: P).
	string ToggleInput()
	{
		return "";
	}

	// Admin windows: closed at once for a player who is not an admin (the server refuses their requests anyway).
	bool AdminOnly()
	{
		return false;
	}

	// --- chrome ---------------------------------------------------------------------------------------------------
	override protected void BuildChrome()
	{
		float w = m_WinW;
		float h = m_WinH;
		float top = CoverTop();
		INT_UIF_Surface.Shadow(m_Window, -56, -44, w + 112, h + 112, 0);
		m_Cover = INT_QST_Look.Cover(m_Window, 0, 0, w, h, INT_QST_Look.Leather(), 8);
		m_Header = INT_UIF_Draw.Frame(m_Window, 0, 0, w, top, "Header");
		m_HeaderCenter = INT_UIF_Draw.Frame(m_Header, 0, 0, w, top, "HeaderCenter");
		m_HeaderRight = INT_UIF_Draw.Frame(m_Header, 0, 0, w, top, "HeaderRight");
		BuildCoverHeader(m_Header, w);
		// close: a small dark leather tile with a cream cross
		Widget close = INT_UIF_Draw.Frame(m_Header, w - 24 - 40, 19, 40, 40, "Close");
		int cf = INT_QST_Look.LeatherDark();
		int cb = INT_QST_Look.LeatherLine();
		INT_UIF_Surface.Box(close, 0, 0, 40, 40, 6, cf, cb);
		Widget closeHover = INT_UIF_Draw.Frame(close, 0, 0, 40, 40, "CloseHover");
		int hf = INT_QST_Look.RedA(90);
		int hb = INT_QST_Look.RedA(200);
		INT_UIF_Surface.Box(closeHover, 0, 0, 40, 40, 6, hf, hb);
		INT_QST_Look.Icon(close, 12, 12, 16, "close", INT_QST_Look.Cream());
		ButtonWidget closeHit = INT_UIF_Draw.Hit(close, 0, 0, 40, 40, m_Clicks);
		m_Clicks.Bind(closeHit, ID_CLOSE, 0, closeHover);

		// a dark edge under the paper, then the body (the pages are drawn by the window: Pages())
		float bw = GetBodyW();
		float bh = GetBodyH();
		INT_UIF_Draw.Line(m_Window, SIDE + 3, top + 4, bw - 6, bh, INT_QST_Look.LeatherDark());
		m_Body = INT_UIF_Draw.Frame(m_Window, SIDE, top, bw, bh, "Body");
		m_Content = m_Body;
		m_Keys = INT_UIF_Draw.Frame(m_Window, SIDE + 8, h - BOTTOM + 16, w - SIDE * 2 - 16, 30, "Keys");
		m_KeysX = 0;
		int brand = INT_QST_Look.Tan2();
		INT_QST_Look.Brand(m_Keys, w - SIDE * 2 - 16, 4, brand);
		m_Flash = INT_UIF_Draw.Frame(m_Window, 0, h - BOTTOM + 10, w, 36, "Flash");
		m_Flash.SetSort(50);		// over the ribbon bookmark the pages add later
		m_Flash.Show(false);
	}

	// Key hint on the leather under the pages.
	void AddKey(string key, string label)
	{
		if (!m_Keys)
			return;
		float adv = INT_QST_Look.Keycap(m_Keys, m_KeysX, 0, key, label);
		m_KeysX = m_KeysX + adv + 18;
	}

	// "/" between two keys of the same action (P / ESC).
	void AddKeySlash()
	{
		if (!m_Keys)
			return;
		int c = INT_QST_Look.CreamA(150);
		INT_QST_Look.Text(m_Keys, m_KeysX - 14, 0, 14, 26, "/", 14, c);
		m_KeysX = m_KeysX + 4;
	}

	// Name of the key bound to an input ("P"), for the hints.
	static string KeyName(string input, string fallback)
	{
		string k = InputUtils.GetButtonNameFromInput(input, EUAINPUT_DEVICE_KEYBOARDMOUSE);
		if (k == "")
			return fallback;
		return k;
	}

	// The paper spread in the body: left page | fold | right page (split = x of the fold in body px).
	void Pages(Widget body, float split, bool ribbon = true)
	{
		float bw = GetBodyW();
		float bh = GetBodyH();
		INT_QST_Look.PaperSheet(body, 0, 0, split, bh, INT_QST_Look.PaperDark(), "LeftPage");
		INT_QST_Look.PaperSheet(body, split, 0, bw - split, bh, INT_QST_Look.Paper(), "RightPage");
		INT_QST_Look.Tex(body, split - 34, 0, 68, bh, "int_qst_spine_ca", 0x8C000000);
		INT_UIF_Draw.Line(body, split + 40, 0, 1, bh, INT_QST_Look.RedA(90));
		if (ribbon)
		{
			float ry = CoverTop() + bh - 4;
			int rc = INT_QST_Look.Red();
			INT_QST_Look.Tex(m_Window, SIDE + split + 190, ry, 22, BOTTOM + 30, "int_qst_ribbon_ca", rc);
		}
	}

	// Short line on the leather under the pages, gone after a few seconds.
	void Flash(string text, bool bad = true)
	{
		if (!m_Flash)
			return;
		INT_UIF_Widgets.ClearChildren(m_Flash);
		int c = INT_QST_Look.Sticky();
		if (bad)
			c = 0xFFF08A7E;
		float tw = INT_UIF_Draw.TextWidth(text, 15) * 1.1 + 60;
		float x = (m_WinW - tw) * 0.5;
		INT_UIF_Surface.Box(m_Flash, x, 0, tw, 36, 6, 0xF21A130C, c);
		string icon = "info";
		if (bad)
			icon = "warning";
		INT_QST_Look.Icon(m_Flash, x + 14, 10, 16, icon, c);
		INT_QST_Look.Heavy(m_Flash, x + 40, 0, tw - 50, 36, text, 15, c);
		m_Flash.SetAlpha(1);
		m_Flash.Show(true);
		m_FlashLeft = 3.5;
		if (bad)
			INT_QST_Sfx.Play("error");
	}

	// Paper question card over the window: Yes / No (Esc = No).
	void Ask(int tag, int data, string title, string text, string yesKey, bool danger)
	{
		CloseAsk();
		m_AskTag = tag;
		m_AskData = data;
		m_Ask = INT_UIF_Draw.Frame(m_Window, 0, 0, m_WinW, m_WinH, "Ask");
		INT_UIF_Surface.Box(m_Ask, 4, 4, m_WinW - 8, m_WinH - 8, 14, 0x9E0D0905, 0);
		ButtonWidget bg = INT_UIF_Draw.Hit(m_Ask, 0, 0, m_WinW, m_WinH, m_Clicks);
		m_Clicks.Bind(bg, ID_ASK_BG);
		array<string> lines = new array<string>();
		INT_QST_Text.Wrap(text, 470, 16, lines);
		float cw = 560;
		float ch = 150 + lines.Count() * 25 + 70;
		float cx = (m_WinW - cw) * 0.5;
		float cy = (m_WinH - ch) * 0.5;
		Widget card = INT_UIF_Draw.Frame(m_Ask, cx, cy, cw, ch, "Card");
		INT_UIF_Surface.Shadow(card, -30, -20, cw + 60, ch + 60, 0x8C000000);
		INT_QST_Look.PaperSheet(card, 0, 0, cw, ch, INT_QST_Look.Note(), "Paper");
		INT_QST_Look.PinHead(card, cw * 0.5 - 13, -12);
		string t = INT_QST_Look.Tr(title);
		INT_QST_Look.Heavy(card, 40, 30, cw - 80, 34, t, 22, INT_QST_Look.Ink());
		INT_QST_Look.DoubleRule(card, 40, 70, cw - 80);
		float ly = 88;
		foreach (string ln : lines)
		{
			INT_QST_Look.Text(card, 44, ly, cw - 88, 24, ln, 16, INT_QST_Look.Blue());
			ly = ly + 25;
		}
		Widget hv;
		int yesStyle = INT_QST_Look.BTN_INK;
		int yesIc = INT_QST_Look.Sticky();
		if (danger)
		{
			yesStyle = INT_QST_Look.BTN_DANGER;
			yesIc = INT_QST_Look.Red();
		}
		float by = ch - 76;
		Widget no = INT_QST_Look.Button(card, 40, by, 200, 48, "#STR_INT_QST_B_BACK", "", INT_QST_Look.BTN_GHOST, 0, hv);
		ButtonWidget noHit = INT_UIF_Draw.Hit(no, 0, 0, 200, 48, m_Clicks);
		m_Clicks.Bind(noHit, ID_ASK_NO, 0, hv);
		Widget yes = INT_QST_Look.Button(card, cw - 40 - 260, by, 260, 48, yesKey, "check", yesStyle, yesIc, hv);
		ButtonWidget yesHit = INT_UIF_Draw.Hit(yes, 0, 0, 260, 48, m_Clicks);
		m_Clicks.Bind(yesHit, ID_ASK_YES, 0, hv);
		INT_UIF_Animator.FadeIn(m_Ask, 0.16);
		INT_QST_Sfx.Play("page");
	}

	// Self-test: answer the open question card.
	void TestAnswer(bool yes)
	{
		if (IsAsking())
			Answer(yes);
	}

	bool IsAsking()
	{
		return m_Ask != null;
	}

	// The card is answered from a click of its own buttons: it is hidden now and unlinked in a later Update (never
	// unlink a widget inside its own event).
	protected void CloseAsk()
	{
		if (m_Ask)
		{
			m_Clicks.ForgetUnder(m_Ask);
			m_Ask.Show(false);
			m_AskDead.Insert(m_Ask);
			m_AskDeadAt = GetGame().GetTickTime();
		}
		m_Ask = null;
	}

	protected void KillDeadAsks()
	{
		if (m_AskDead.Count() == 0)
			return;
		float now = GetGame().GetTickTime();
		if (now <= m_AskDeadAt)
			return;
		foreach (Widget w : m_AskDead)
		{
			if (w)
				w.Unlink();
		}
		m_AskDead.Clear();
	}

	protected void Answer(bool yes)
	{
		int tag = m_AskTag;
		int data = m_AskData;
		CloseAsk();
		INT_QST_Sfx.Play("click");
		OnAnswer(tag, data, yes);
	}

	// --- input ------------------------------------------------------------------------------------------------------
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		m_Age = m_Age + timeslice;
		KillDeadAsks();
		INT_QST_VListTicker.Tick();
		if (m_FlashLeft > 0)
		{
			m_FlashLeft = m_FlashLeft - timeslice;
			if (m_FlashLeft <= 0 && m_Flash)
				INT_UIF_Animator.FadeOut(m_Flash, 0.3);
		}
		if (IsClosingDown())
			return;
		if (AdminOnly() && !INT_QST_Client.IsAdmin())
		{
			CloseNow();
			return;
		}
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb || !pb.IsAlive())
		{
			CloseNow();
			return;
		}
		m_SinceRefresh = m_SinceRefresh + timeslice;
		if (m_DataDirty && m_SinceRefresh >= DIRTY_GAP)
		{
			m_DataDirty = false;
			m_SinceRefresh = 0;
			OnDirtyRefresh();
			if (IsClosingDown())
				return;
		}
		HandleEscape();
		string toggle = ToggleInput();
		if (toggle == "" || m_Age < 0.3 || EditBoxWidget.Cast(GetFocus()) || MultilineEditBoxWidget.Cast(GetFocus()))
			return;
		UAInput inp = GetUApi().GetInputByName(toggle);
		if (!inp)
			return;
		if (inp.LocalPress())
			m_KeyArmed = true;
		else if (m_KeyArmed && !inp.LocalHold())
		{
			m_KeyArmed = false;
			if (!HasModal() && !IsAsking() && !OnEscapeOverlay())
				RequestClose();
		}
	}

	// Same order as the framework (dropdown, dialog, window), on RELEASE (INT_P2P_Menu.HandleEscape).
	protected void HandleEscape()
	{
		UAInput back = GetUApi().GetInputByID(UAUIBack);
		if (!back)
			return;
		if (back.LocalPress())
		{
			m_EscArmed = true;
			return;
		}
		if (!m_EscArmed || KeyState(KeyCode.KC_ESCAPE) != 0)
			return;
		m_EscArmed = false;
		if (INT_UIF_Dropdown.CloseActive())
			return;
		if (HasModal() && m_Modal)
		{
			m_Modal.Cancel();
			return;
		}
		if (IsAsking())
		{
			Answer(false);
			return;
		}
		if (OnEscapeOverlay())
			return;
		RequestClose();
	}

	override void OnShow()
	{
		super.OnShow();
		m_Age = 0;
		m_EscArmed = false;
		m_KeyArmed = false;
		INT_QST_Sfx.Play("open");
	}

	override void OnHide()
	{
		super.OnHide();
		CloseAsk();
		INT_QST_Sfx.Play("close");
	}

	// Close button and question card are common: true when handled.
	protected bool CommonClick(int id)
	{
		if (id == ID_CLOSE)
		{
			RequestClose();
			return true;
		}
		if (id == ID_ASK_YES)
		{
			Answer(true);
			return true;
		}
		if (id == ID_ASK_NO)
		{
			Answer(false);
			return true;
		}
		if (id == ID_ASK_BG)
			return true;
		return false;
	}
}
