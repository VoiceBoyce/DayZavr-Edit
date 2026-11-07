// INT Banking - the ATM window "DAYZAVR BANKING SYSTEM" on INT_UIFramework.
//
// Left: the player's bank card, the ATM side menu (F1..F5), live cooldowns. Right: the page.
//   0 Overview  balance, cash, limit, quick actions, last operations
//   1 Deposit   ATM display + keypad, quick sums, "deposit all", change
//   2 Withdraw  ATM display + keypad, notes preview, "inventory full - free up room" notice (never on the ground)
//   3 Transfer  recipient (Steam64 / paste), display + keypad, confirm dialog
//   4 History   operations, newest first, pages, clear
// Keyboard: digits / numpad, Backspace, Delete (clear), Enter (confirm), F1..F5 (pages), Esc.
// Every answer comes through INT_Bank_Client (queued, never handled inside OnRPC).

class INT_Bank_Menu extends INT_UIF_Menu
{
	static const int PAGE_OVERVIEW = 0;
	static const int PAGE_DEPOSIT = 1;
	static const int PAGE_WITHDRAW = 2;
	static const int PAGE_TRANSFER = 3;
	static const int PAGE_HISTORY = 4;

	static const float LEFT_W = 360;
	static const float PAGE_PAD = 24;

	protected bool m_Ready;
	protected bool m_Hooked;
	protected bool m_OpenSounded;
	protected bool m_Preview;
	protected int m_Page = -1;
	protected float m_Tick;
	protected float m_CdTick;
	protected int m_LastCash = -1;

	// requests in flight
	protected int m_HelloSeq;
	protected int m_OpSeq;
	protected int m_OpKind;
	protected int m_OpAmount;
	protected string m_OpTarget;
	protected int m_HistSeq;
	protected int m_RecentSeq;
	protected int m_LookupSeq;
	protected int m_HistPage;
	protected int m_HistPages = 1;
	protected int m_HistTotal;

	// layout
	protected Widget m_Left;
	protected Widget m_Right;
	protected Widget m_PageHost;
	protected ref array<Widget> m_Pages;
	protected ref INT_Bank_UICard m_Card;
	protected ref INT_Bank_UINav m_Nav;
	protected TextWidget m_CdDep;
	protected TextWidget m_CdWd;
	protected TextWidget m_CdTr;

	// overview
	protected TextWidget m_OvBalance;
	protected TextWidget m_OvBalanceCur;
	protected INT_UIF_ProgressBar m_OvLimitBar;
	protected TextWidget m_OvLimitText;
	protected TextWidget m_OvCash;
	protected TextWidget m_OvAvail;
	protected TextWidget m_OvOps;
	protected INT_UIF_List m_OvRecent;
	protected int m_ShownBalance;
	protected ref INT_UIF_Tween m_CountTween;

	// deposit
	protected ref INT_Bank_UIAmount m_DepAmount;
	protected ref INT_Bank_UIKeypad m_DepKeys;
	protected TextWidget m_DepCash;
	protected TextWidget m_DepNow;
	protected TextWidget m_DepAfter;
	protected INT_Bank_UIButton m_DepGo;
	protected INT_Bank_UIButton m_DepAll;

	// withdraw
	protected ref INT_Bank_UIAmount m_WdAmount;
	protected ref INT_Bank_UIKeypad m_WdKeys;
	protected TextWidget m_WdNow;
	protected TextWidget m_WdLimit;
	protected TextWidget m_WdAfter;
	protected RichTextWidget m_WdNotes;
	protected INT_Bank_UIButton m_WdGo;

	// transfer
	protected ref INT_Bank_UIAmount m_TrAmount;
	protected ref INT_Bank_UIKeypad m_TrKeys;
	protected INT_UIF_TextInput m_TrId;
	protected TextWidget m_TrStatus;
	protected ImageWidget m_TrStatusIcon;
	protected TextWidget m_TrNow;
	protected TextWidget m_TrAfter;
	protected INT_Bank_UIButton m_TrGo;
	protected string m_TrLookedUp;
	protected string m_TrName;
	protected bool m_TrValid;
	protected float m_TrLookupWait = -1;

	// history
	protected INT_UIF_List m_HistList;
	protected TextWidget m_HistPager;
	protected TextWidget m_HistCount;
	protected INT_Bank_UIIconButton m_HistPrev;
	protected INT_Bank_UIIconButton m_HistNext;
	protected INT_Bank_UIButton m_HistClear;

	// overlays
	protected Widget m_Busy;
	protected ImageWidget m_BusySpin;
	protected TextWidget m_BusyText;
	protected Widget m_Result;
	protected Widget m_ResultBox;
	protected ImageWidget m_ResultIcon;
	protected ref INT_UIF_Surface m_ResultBadge;
	protected TextWidget m_ResultTitle;
	protected TextWidget m_ResultAmount;
	protected MultilineTextWidget m_ResultText;
	protected Widget m_Dispenser;
	protected ref array<Widget> m_Notes;
	protected Widget m_StateBox;
	protected TextWidget m_StateTitle;
	protected MultilineTextWidget m_StateText;
	protected ImageWidget m_StateSpin;
	protected INT_UIF_Dialog m_Dialog;

	// keyboard
	protected ref map<int, bool> m_KeyWas;
	protected bool m_EscWas;

	void INT_Bank_Menu()
	{
		m_Pages = new array<Widget>();
		m_Notes = new array<Widget>();
		m_KeyWas = new map<int, bool>();
		m_Preview = INT_Bank_UIState.IsPreview();
		INT_Bank_UILook.Activate();
	}

	void ~INT_Bank_Menu()
	{
		UnhookClient();
		INT_Bank_UILook.Restore();
	}

	// =================================================================================================
	// Window
	// =================================================================================================
	override string GetTitle()
	{
		return INT_BANK_TITLE;
	}

	override string GetSubtitle()
	{
		return "#STR_INT_BANK_SUB";
	}

	override string GetIcon()
	{
		return "";
	}

	override void GetWindowSize(out float w, out float h)
	{
		w = 1300;
		h = 820;
	}

	override bool WantsKeyboard()
	{
		return true;
	}

	// Esc is handled in Update on key RELEASE (overlay -> dialog -> window), so the vanilla pause menu
	// does not open behind the closing window.
	override bool CloseOnEscape()
	{
		return false;
	}

	override void OnBuildHeader(Widget center, Widget right)
	{
		BuildBrand();
		float slot = GetHeaderSlotW();
		Widget pill = INT_UIF_Draw.Frame(right, slot - 246, 24, 190, 28, "SecurePill");
		int brass = INT_Bank_UILook.Brass();
		int fill = INT_UIF_Color.WithAlpha(brass, 22);
		int rim = INT_UIF_Color.WithAlpha(brass, 90);
		INT_UIF_Surface.Pill(pill, 0, 0, 190, 28, fill, rim);
		INT_UIF_Draw.Dot(pill, 12, 10, 8, INT_Bank_UILook.Sage(), true);
		TextWidget secure = INT_UIF_Draw.Text(pill, 28, 0, 156, 28, "#STR_INT_BANK_SECURE", 10, INT_Bank_UILook.BrassLight());
		if (secure)
			secure.SetBold(true);
	}

	protected void BuildBrand()
	{
		if (!m_Header)
			return;
		int brass = INT_Bank_UILook.Brass();
		int glowC = INT_UIF_Color.WithAlpha(brass, 60);
		INT_UIF_Draw.Image(m_Header, -8, -12, 100, 100, "iuf_radial", glowC);
		Widget tile = INT_UIF_Draw.Frame(m_Header, 20, 16, 44, 44, "BankLogo");
		int tileFill = ARGB(240, 38, 30, 18);
		int tileRim = INT_UIF_Color.WithAlpha(brass, 170);
		INT_UIF_Surface.Box(tile, 0, 0, 44, 44, 12, tileFill, tileRim);
		INT_UIF_Draw.Image(tile, 4, 2, 36, 18, "iuf_grad_v", ARGB(36, 255, 255, 255));
		ImageWidget glyph = INT_UIF_Draw.Icon(tile, 11, 11, 22, "wallet", brass);
		if (glyph)
			INT_UIF_Animator.Pulse(glyph, 0.65, 1.0, 2.4);
		if (m_Title)
		{
			INT_UIF_Draw.Place(m_Title, 78, 13, 520, 28);
			m_Title.SetText(INT_BANK_TITLE);
		}
		if (m_Subtitle)
		{
			INT_UIF_Draw.Place(m_Subtitle, 78, 41, 520, 16);
			string sub = INT_Bank_UILook.Tr(GetSubtitle());
			m_Subtitle.SetText(sub);
		}
	}

	override void OnBuild(Widget body)
	{
		float bw = GetBodyW();
		float bh = GetBodyH();
		if (m_WinBg)
			BuildBackdrop(m_WinBg.GetRoot(), m_WinW, m_WinH);

		m_Left = INT_UIF_Draw.Frame(body, 0, 0, LEFT_W, bh, "BankLeft");
		m_Card = new INT_Bank_UICard();
		m_Card.Build(m_Left, 0, 0, LEFT_W, 214);
		m_Nav = new INT_Bank_UINav();
		m_Nav.Build(m_Left, 0, 240, LEFT_W, 290);
		m_Nav.AddItem("#STR_INT_BANK_NAV_OVERVIEW", "home", "F1");
		m_Nav.AddItem("#STR_INT_BANK_NAV_DEPOSIT", "plus", "F2");
		m_Nav.AddItem("#STR_INT_BANK_NAV_WITHDRAW", "wallet", "F3");
		m_Nav.AddItem("#STR_INT_BANK_NAV_TRANSFER", "swap", "F4");
		m_Nav.AddItem("#STR_INT_BANK_NAV_HISTORY", "list", "F5");
		m_Nav.OnChanged().Insert(OnNav);
		BuildCooldowns(m_Left, 0, 538, LEFT_W, bh - 538);

		float rx = LEFT_W + 20;
		float rw = bw - rx;
		m_Right = INT_UIF_Draw.Frame(body, rx, 0, rw, bh, "BankRight");
		int rFill = INT_Bank_UILook.Glass();
		int rRim = INT_Bank_UILook.Hair();
		INT_UIF_Surface.Box(m_Right, 0, 0, rw, bh, 14, rFill, rRim);
		m_PageHost = INT_UIF_Draw.Frame(m_Right, PAGE_PAD, PAGE_PAD, rw - PAGE_PAD * 2, bh - PAGE_PAD * 2, "BankPages");
		float pw = rw - PAGE_PAD * 2;
		float ph = bh - PAGE_PAD * 2;
		BuildOverview(NewPage(pw, ph), pw, ph);
		BuildDeposit(NewPage(pw, ph), pw, ph);
		BuildWithdraw(NewPage(pw, ph), pw, ph);
		BuildTransfer(NewPage(pw, ph), pw, ph);
		BuildHistory(NewPage(pw, ph), pw, ph);
		BuildBusy(m_Right, rw, bh);
		BuildResult(m_Right, rw, bh);
		BuildState(body, bw, bh);

		RevealOnOpen(m_Left);
		RevealOnOpen(m_Right);
		AddHint("0-9", "#STR_INT_BANK_HINT_DIGITS");
		AddHint("Enter", "#STR_INT_BANK_HINT_CONFIRM");
		AddHint("F1-F5", "#STR_INT_BANK_HINT_PAGES");
		AddHint("Esc", "#STR_INT_BANK_HINT_CLOSE");
		ShowPage(PAGE_OVERVIEW, false);
		m_Nav.Select(PAGE_OVERVIEW, false, false);
		ShowState(true, "#STR_INT_BANK_STATE_CONNECTING", "#STR_INT_BANK_STATE_CONNECTING_SUB", true);
	}

	protected void BuildBackdrop(Widget host, float w, float h)
	{
		if (!host)
			return;
		// abstract warm light: blurred blobs + very faint pinstripes (no figures, text stays readable)
		INT_UIF_Draw.Texture(host, 0, 0, w, h, INT_BANK_TEX_AURORA, ARGB(70, 196, 150, 82));
		INT_UIF_Draw.Texture(host, 0, 0, w, h, INT_BANK_TEX_STRIPES, ARGB(12, 255, 230, 190));
		INT_UIF_Draw.Texture(host, 0, 0, w, h, INT_BANK_TEX_VIGNETTE, ARGB(150, 4, 3, 2));
	}

	protected Widget NewPage(float w, float h)
	{
		Widget p = INT_UIF_Draw.Frame(m_PageHost, 0, 0, w, h, "BankPage");
		p.Show(false);
		m_Pages.Insert(p);
		return p;
	}

	protected void BuildCooldowns(Widget parent, float x, float y, float w, float h)
	{
		Widget box = INT_UIF_Draw.Frame(parent, x, y, w, h, "Cooldowns");
		int bFill = INT_Bank_UILook.Glass();
		int bRim = INT_Bank_UILook.Hair();
		INT_UIF_Surface.Box(box, 0, 0, w, h, 12, bFill, bRim);
		INT_UIF_Draw.Caption(box, 18, 14, w - 36, "#STR_INT_BANK_CD_TITLE", "clock");
		m_CdDep = CdRow(box, 18, 46, w - 36, "#STR_INT_BANK_CD_DEPOSIT");
		m_CdWd = CdRow(box, 18, 72, w - 36, "#STR_INT_BANK_CD_WITHDRAW");
		m_CdTr = CdRow(box, 18, 98, w - 36, "#STR_INT_BANK_CD_TRANSFER");
	}

	protected TextWidget CdRow(Widget parent, float x, float y, float w, string label)
	{
		INT_UIF_Draw.Text(parent, x, y, w * 0.6, 20, label, 13, INT_Bank_UILook.Muted());
		return INT_UIF_Draw.Text(parent, x + w * 0.4, y, w * 0.6, 20, "", 13, INT_Bank_UILook.Sage(), INT_UIF_Align.RIGHT);
	}

	// ---------------------------------------------------------------------------------------------
	// Overview
	// ---------------------------------------------------------------------------------------------
	protected void BuildOverview(Widget p, float w, float h)
	{
		INT_UIF_Draw.Caption(p, 0, 0, w, "#STR_INT_BANK_OV_BALANCE", "wallet");
		m_OvBalance = INT_UIF_Draw.Text(p, 0, 26, w - 160, 70, "0", 60, INT_Bank_UILook.Ink(), INT_UIF_Align.LEFT, true);
		m_OvBalanceCur = INT_UIF_Draw.Text(p, 200, 56, 140, 30, "#STR_INT_BANK_CUR", 22, INT_Bank_UILook.Brass());
		m_OvLimitText = INT_UIF_Draw.Text(p, 0, 104, w, 18, "", 12, INT_Bank_UILook.Faint());
		m_OvLimitBar = INT_UIF_ProgressBar.Create(m_Bag, p, 0, 126, w, 6, 0, INT_UIF_ThemeKeys.TEAL);

		float tw = (w - 32) / 3;
		m_OvCash = Tile(p, 0, 152, tw, "#STR_INT_BANK_OV_CASH", "wallet");
		m_OvAvail = Tile(p, tw + 16, 152, tw, "#STR_INT_BANK_OV_AVAIL", "target");
		m_OvOps = Tile(p, (tw + 16) * 2, 152, tw, "#STR_INT_BANK_OV_OPS", "list");

		float bw = (w - 36) / 4;
		INT_Bank_UIButton b1 = INT_Bank_UIButton.CreateFx(m_Bag, p, 0, 262, bw, 50, "#STR_INT_BANK_NAV_DEPOSIT", INT_UIF_Variant.PRIMARY, "plus");
		b1.SetInkLabel(true);
		b1.OnClick().Insert(OnQuickDeposit);
		INT_Bank_UIButton b2 = INT_Bank_UIButton.CreateFx(m_Bag, p, bw + 12, 262, bw, 50, "#STR_INT_BANK_NAV_WITHDRAW", INT_UIF_Variant.SECONDARY, "wallet");
		b2.OnClick().Insert(OnQuickWithdraw);
		INT_Bank_UIButton b3 = INT_Bank_UIButton.CreateFx(m_Bag, p, (bw + 12) * 2, 262, bw, 50, "#STR_INT_BANK_NAV_TRANSFER", INT_UIF_Variant.SECONDARY, "swap");
		b3.OnClick().Insert(OnQuickTransfer);
		INT_Bank_UIButton b4 = INT_Bank_UIButton.CreateFx(m_Bag, p, (bw + 12) * 3, 262, bw, 50, "#STR_INT_BANK_DEP_ALL", INT_UIF_Variant.SECONDARY, "check");
		b4.OnClick().Insert(OnDepositAll);

		INT_UIF_Draw.Caption(p, 0, 340, w, "#STR_INT_BANK_OV_RECENT", "clock");
		m_OvRecent = INT_UIF_List.Create(m_Bag, p, 0, 368, w, h - 368, "#STR_INT_BANK_HIST_EMPTY");
		m_OvRecent.SetSelectable(false);
	}

	protected TextWidget Tile(Widget p, float x, float y, float w, string label, string icon)
	{
		Widget t = INT_UIF_Draw.Frame(p, x, y, w, 88, "OvTile");
		int tFill = INT_Bank_UILook.GlassHi();
		int tRim = INT_Bank_UILook.Hair();
		INT_UIF_Surface.Box(t, 0, 0, w, 88, 12, tFill, tRim);
		int tileHue = INT_Bank_UILook.Brass();
		INT_UIF_Draw.IconTile(t, 16, 16, 30, icon, tileHue, 8);
		INT_UIF_Draw.Text(t, 56, 14, w - 70, 16, label, 11, INT_Bank_UILook.Faint());
		TextWidget v = INT_UIF_Draw.Text(t, 56, 36, w - 70, 34, "-", 24, INT_Bank_UILook.Ink(), INT_UIF_Align.LEFT, true);
		return v;
	}

	// ---------------------------------------------------------------------------------------------
	// Deposit / Withdraw / Transfer
	// ---------------------------------------------------------------------------------------------
	protected float KeypadX(float w)
	{
		return w - 300;
	}

	protected INT_Bank_UIKeypad BuildKeypad(Widget p, float w, float y)
	{
		INT_Bank_UIKeypad k = new INT_Bank_UIKeypad();
		k.Build(p, KeypadX(w), y, 300, 348);
		k.OnKey().Insert(OnKeypad);
		return k;
	}

	protected void QuickRow(Widget p, float x, float y, float w, string prefix)
	{
		float gap = 8;
		float cw = (w - gap * 3) / 4;
		array<int> sums = {1000, 5000, 10000, 50000};
		for (int i = 0; i < 4; i++)
		{
			int v = sums.Get(i);
			string label = "+" + INT_Bank_Util.Money(v);
			INT_Bank_UIButton c = INT_Bank_UIButton.CreateFx(m_Bag, p, x + i * (cw + gap), y, cw, 34, label, INT_UIF_Variant.SECONDARY, "");
			c.SetSound(INT_Bank_Sfx.KEY);
			c.OnClick().Insert(OnQuickSum);
		}
		float hw = (w - gap) / 2;
		INT_Bank_UIButton mx = INT_Bank_UIButton.CreateFx(m_Bag, p, x, y + 42, hw, 34, "#STR_INT_BANK_MAX", INT_UIF_Variant.GHOST, "chevron_up");
		mx.SetSound(INT_Bank_Sfx.KEY);
		mx.OnClick().Insert(OnQuickMax);
		INT_Bank_UIButton cl = INT_Bank_UIButton.CreateFx(m_Bag, p, x + hw + gap, y + 42, hw, 34, "#STR_INT_BANK_CLEAR", INT_UIF_Variant.GHOST, "close");
		cl.SetSound(INT_Bank_Sfx.KEY);
		cl.OnClick().Insert(OnQuickClear);
	}

	protected TextWidget InfoRow(Widget p, float x, float y, float w, string label)
	{
		INT_UIF_Draw.Text(p, x, y, w * 0.6, 22, label, 13, INT_Bank_UILook.Muted());
		TextWidget v = INT_UIF_Draw.Text(p, x + w * 0.35, y, w * 0.65, 22, "-", 14, INT_Bank_UILook.Ink(), INT_UIF_Align.RIGHT);
		if (v)
			v.SetBold(true);
		INT_UIF_Draw.Line(p, x, y + 27, w, 1, INT_Bank_UILook.Hair());
		return v;
	}

	protected void BuildDeposit(Widget p, float w, float h)
	{
		float lw = KeypadX(w) - 28;
		INT_UIF_Draw.Caption(p, 0, 0, lw, "#STR_INT_BANK_DEP_TITLE", "plus");
		m_DepAmount = new INT_Bank_UIAmount();
		m_DepAmount.Build(p, 0, 30, lw, 140, "#STR_INT_BANK_AMOUNT_DEPOSIT");
		QuickRow(p, 0, 186, lw, "dep");
		m_DepCash = InfoRow(p, 0, 282, lw, "#STR_INT_BANK_INFO_CASH");
		m_DepNow = InfoRow(p, 0, 318, lw, "#STR_INT_BANK_INFO_BALANCE");
		m_DepAfter = InfoRow(p, 0, 354, lw, "#STR_INT_BANK_INFO_AFTER");
		MultilineTextWidget note = INT_UIF_Draw.MText(p, 0, 396, lw, 54, "#STR_INT_BANK_DEP_NOTE", 12, INT_Bank_UILook.Faint());
		float by = h - 54;
		float half = (lw - 12) / 2;
		m_DepAll = INT_Bank_UIButton.CreateFx(m_Bag, p, 0, by, half, 54, "#STR_INT_BANK_DEP_ALL", INT_UIF_Variant.SECONDARY, "check");
		m_DepAll.OnClick().Insert(OnDepositAll);
		m_DepGo = INT_Bank_UIButton.CreateFx(m_Bag, p, half + 12, by, half, 54, "#STR_INT_BANK_DEP_GO", INT_UIF_Variant.PRIMARY, "plus");
		m_DepGo.SetInkLabel(true);
		m_DepGo.OnClick().Insert(OnDepositGo);
		m_DepKeys = BuildKeypad(p, w, 30);
		KeyHints(p, KeypadX(w), 392, 300);
	}

	protected void BuildWithdraw(Widget p, float w, float h)
	{
		float lw = KeypadX(w) - 28;
		INT_UIF_Draw.Caption(p, 0, 0, lw, "#STR_INT_BANK_WD_TITLE", "wallet");
		m_WdAmount = new INT_Bank_UIAmount();
		m_WdAmount.Build(p, 0, 30, lw, 140, "#STR_INT_BANK_AMOUNT_WITHDRAW");
		QuickRow(p, 0, 186, lw, "wd");
		m_WdNow = InfoRow(p, 0, 282, lw, "#STR_INT_BANK_INFO_BALANCE");
		m_WdLimit = InfoRow(p, 0, 318, lw, "#STR_INT_BANK_INFO_LIMIT");
		m_WdAfter = InfoRow(p, 0, 354, lw, "#STR_INT_BANK_INFO_LEFT");
		INT_UIF_Draw.Text(p, 0, 394, lw, 18, "#STR_INT_BANK_WD_NOTES", 11, INT_Bank_UILook.Faint());
		m_WdNotes = INT_UIF_Draw.Rich(p, 0, 414, lw, 44, "", 13, INT_Bank_UILook.Sand(), INT_UIF_Align.LEFT);
		float by = h - 54;
		m_WdGo = INT_Bank_UIButton.CreateFx(m_Bag, p, 0, by, lw, 54, "#STR_INT_BANK_WD_GO", INT_UIF_Variant.PRIMARY, "wallet");
		m_WdGo.SetInkLabel(true);
		m_WdGo.OnClick().Insert(OnWithdrawGo);
		m_WdKeys = BuildKeypad(p, w, 30);
		KeyHints(p, KeypadX(w), 392, 300);
	}

	protected void BuildTransfer(Widget p, float w, float h)
	{
		float lw = KeypadX(w) - 28;
		INT_UIF_Draw.Caption(p, 0, 0, lw, "#STR_INT_BANK_TR_RECIPIENT", "user");
		m_TrId = INT_UIF_TextInput.Create(m_Bag, p, 0, 28, lw - 48, 44, "#STR_INT_BANK_TR_ID_HINT", 17, "user");
		m_TrId.OnChanged().Insert(OnTrIdChanged);
		INT_Bank_UIIconButton paste = INT_Bank_UIIconButton.CreateFx(m_Bag, p, lw - 40, 30, 40, "list", INT_UIF_Variant.SECONDARY, "#STR_INT_BANK_TR_PASTE");
		paste.OnClick().Insert(OnTrPaste);
		m_TrStatusIcon = INT_UIF_Draw.Icon(p, 2, 82, 14, "info", INT_Bank_UILook.Faint());
		m_TrStatus = INT_UIF_Draw.Text(p, 22, 78, lw - 22, 22, "#STR_INT_BANK_TR_ENTER_ID", 13, INT_Bank_UILook.Faint());
		m_TrAmount = new INT_Bank_UIAmount();
		m_TrAmount.Build(p, 0, 110, lw, 132, "#STR_INT_BANK_AMOUNT_TRANSFER");
		QuickRow(p, 0, 254, lw, "tr");
		m_TrNow = InfoRow(p, 0, 348, lw, "#STR_INT_BANK_INFO_BALANCE");
		m_TrAfter = InfoRow(p, 0, 384, lw, "#STR_INT_BANK_INFO_LEFT");
		float by = h - 54;
		m_TrGo = INT_Bank_UIButton.CreateFx(m_Bag, p, 0, by, lw, 54, "#STR_INT_BANK_TR_GO", INT_UIF_Variant.PRIMARY, "swap");
		m_TrGo.SetInkLabel(true);
		m_TrGo.OnClick().Insert(OnTransferGo);
		m_TrKeys = BuildKeypad(p, w, 30);
		KeyHints(p, KeypadX(w), 392, 300);
	}

	protected void KeyHints(Widget p, float x, float y, float w)
	{
		MultilineTextWidget t = INT_UIF_Draw.MText(p, x, y, w, 60, "#STR_INT_BANK_KEYS_NOTE", 11, INT_Bank_UILook.Faint());
	}

	// ---------------------------------------------------------------------------------------------
	// History
	// ---------------------------------------------------------------------------------------------
	protected void BuildHistory(Widget p, float w, float h)
	{
		INT_UIF_Draw.Caption(p, 0, 0, w * 0.6, "#STR_INT_BANK_HIST_TITLE", "list");
		m_HistCount = INT_UIF_Draw.Text(p, w * 0.4, 0, w * 0.6, 20, "", 12, INT_Bank_UILook.Faint(), INT_UIF_Align.RIGHT);
		m_HistList = INT_UIF_List.Create(m_Bag, p, 0, 30, w, h - 30 - 60, "#STR_INT_BANK_HIST_EMPTY");
		m_HistList.SetSelectable(false);
		float by = h - 44;
		m_HistPrev = INT_Bank_UIIconButton.CreateFx(m_Bag, p, 0, by, 40, "chevron_left", INT_UIF_Variant.SECONDARY, "");
		m_HistPrev.OnClick().Insert(OnHistPrev);
		m_HistPager = INT_UIF_Draw.Text(p, 48, by, 140, 40, "1 / 1", 14, INT_Bank_UILook.Ink(), INT_UIF_Align.CENTER);
		m_HistNext = INT_Bank_UIIconButton.CreateFx(m_Bag, p, 196, by, 40, "chevron_right", INT_UIF_Variant.SECONDARY, "");
		m_HistNext.OnClick().Insert(OnHistNext);
		m_HistClear = INT_Bank_UIButton.CreateFx(m_Bag, p, w - 240, by, 240, 40, "#STR_INT_BANK_HIST_CLEAR", INT_UIF_Variant.DANGER, "trash");
		m_HistClear.OnClick().Insert(OnHistClear);
	}

	// ---------------------------------------------------------------------------------------------
	// Overlays
	// ---------------------------------------------------------------------------------------------
	protected void BuildBusy(Widget host, float w, float h)
	{
		m_Busy = INT_UIF_Draw.Frame(host, 0, 0, w, h, "BankBusy");
		INT_UIF_Surface.Box(m_Busy, 0, 0, w, h, 14, ARGB(200, 8, 6, 4), 0);
		INT_UIF_Draw.Hit(m_Busy, 0, 0, w, h, null);
		m_BusySpin = INT_UIF_Draw.Image(m_Busy, w * 0.5 - 28, h * 0.5 - 60, 56, 56, "iuf_ringline64", INT_Bank_UILook.Brass());
		m_BusyText = INT_UIF_Draw.Text(m_Busy, 0, h * 0.5 + 8, w, 24, "#STR_INT_BANK_BUSY", 15, INT_Bank_UILook.Sand(), INT_UIF_Align.CENTER);
		m_Busy.Show(false);
	}

	protected void BuildResult(Widget host, float w, float h)
	{
		m_Result = INT_UIF_Draw.Frame(host, 0, 0, w, h, "BankResult");
		INT_UIF_Surface.Box(m_Result, 0, 0, w, h, 14, ARGB(225, 8, 6, 4), 0);
		INT_UIF_Draw.Hit(m_Result, 0, 0, w, h, null);
		float bw = 520;
		float bh = 430;
		m_ResultBox = INT_UIF_Draw.Frame(m_Result, (w - bw) * 0.5, (h - bh) * 0.5, bw, bh, "ResultBox");
		INT_UIF_Surface.Shadow(m_ResultBox, -24, -12, bw + 48, bh + 44, ARGB(190, 0, 0, 0));
		INT_UIF_Surface.Box(m_ResultBox, 0, 0, bw, bh, 16, ARGB(250, 24, 20, 15), ARGB(110, 216, 174, 98));
		INT_UIF_Draw.Texture(m_ResultBox, 0, 0, bw, bh, INT_BANK_TEX_AURORA, ARGB(46, 196, 150, 82));
		// dispenser slot (withdraw only)
		m_Dispenser = INT_UIF_Draw.Clip(m_ResultBox, bw * 0.5 - 110, 22, 220, 96, "Dispenser");
		Widget slot = INT_UIF_Draw.Frame(m_ResultBox, bw * 0.5 - 120, 14, 240, 14, "Slot");
		INT_UIF_Surface.Box(slot, 0, 0, 240, 14, 7, ARGB(255, 6, 5, 4), ARGB(120, 216, 174, 98));
		for (int i = 0; i < 5; i++)
		{
			Widget note = INT_UIF_Draw.Frame(m_Dispenser, 20 + i * 4, -90, 180, 84, "Note");
			INT_UIF_Surface.Box(note, 0, 0, 180, 84, 6, ARGB(255, 120, 150, 98), ARGB(200, 60, 80, 48));
			INT_UIF_Draw.Texture(note, 0, 0, 180, 84, INT_BANK_TEX_STRIPES, ARGB(60, 230, 240, 200));
			INT_UIF_Draw.Text(note, 0, 24, 180, 36, "5000", 26, ARGB(200, 40, 56, 30), INT_UIF_Align.CENTER, true);
			m_Notes.Insert(note);
		}
		float cy = 136;
		INT_UIF_Surface.Glow(m_ResultBox, bw * 0.5 - 60, cy - 26, 120, 120, ARGB(60, 160, 196, 122));
		m_ResultBadge = INT_UIF_Surface.Box(m_ResultBox, bw * 0.5 - 34, cy, 68, 68, 14, ARGB(255, 160, 196, 122), 0);
		m_ResultIcon = INT_UIF_Draw.Icon(m_ResultBox, bw * 0.5 - 16, cy + 18, 32, "check", ARGB(255, 24, 30, 16));
		m_ResultTitle = INT_UIF_Draw.Text(m_ResultBox, 0, cy + 84, bw, 30, "", 22, INT_Bank_UILook.Ink(), INT_UIF_Align.CENTER);
		if (m_ResultTitle)
			m_ResultTitle.SetBold(true);
		m_ResultAmount = INT_UIF_Draw.Text(m_ResultBox, 0, cy + 118, bw, 46, "", 38, INT_Bank_UILook.BrassLight(), INT_UIF_Align.CENTER, true);
		m_ResultText = INT_UIF_Draw.MText(m_ResultBox, 40, cy + 170, bw - 80, 66, "", 13, INT_Bank_UILook.Muted());
		INT_Bank_UIButton ok = INT_Bank_UIButton.CreateFx(m_Bag, m_ResultBox, bw * 0.5 - 110, bh - 66, 220, 46, "#STR_INT_BANK_DONE", INT_UIF_Variant.PRIMARY, "check");
		ok.SetInkLabel(true);
		ok.OnClick().Insert(OnResultClose);
		m_Result.Show(false);
	}

	protected void BuildState(Widget body, float w, float h)
	{
		m_StateBox = INT_UIF_Draw.Frame(body, 0, 0, w, h, "BankState");
		INT_UIF_Surface.Box(m_StateBox, 0, 0, w, h, 14, ARGB(245, 14, 11, 8), INT_Bank_UILook.Hair());
		INT_UIF_Draw.Hit(m_StateBox, 0, 0, w, h, null);
		m_StateSpin = INT_UIF_Draw.Image(m_StateBox, w * 0.5 - 30, h * 0.5 - 90, 60, 60, "iuf_ringline64", INT_Bank_UILook.Brass());
		m_StateTitle = INT_UIF_Draw.Text(m_StateBox, 0, h * 0.5 - 14, w, 30, "", 20, INT_Bank_UILook.Ink(), INT_UIF_Align.CENTER);
		m_StateText = INT_UIF_Draw.MText(m_StateBox, w * 0.25, h * 0.5 + 22, w * 0.5, 60, "", 13, INT_Bank_UILook.Muted());
		m_StateBox.Show(false);
	}

	protected void ShowState(bool show, string title, string text, bool spinner)
	{
		if (!m_StateBox)
			return;
		m_StateBox.Show(show);
		if (!show)
			return;
		if (m_StateTitle)
			m_StateTitle.SetText(title);
		if (m_StateText)
			m_StateText.SetText(text);
		if (m_StateSpin)
		{
			m_StateSpin.Show(spinner);
			if (spinner)
			{
				INT_UIF_Tween spin = INT_UIF_Animator.Rotate(m_StateSpin, 360, 1.1, INT_UIF_Ease.LINEAR);
				if (spin)
					spin.Loop(-1, false);
			}
		}
	}

	protected void ShowBusy(bool show)
	{
		if (!m_Busy)
			return;
		m_Busy.Show(show);
		if (show)
		{
			INT_UIF_Animator.FadeInColor(m_Busy, 0.15);
			INT_UIF_Tween spin = INT_UIF_Animator.Rotate(m_BusySpin, 360, 0.9, INT_UIF_Ease.LINEAR);
			if (spin)
				spin.Loop(-1, false);
		}
	}

	// kind: INT_Bank_Op of the finished operation
	protected void ShowResult(int kind, string title, int amount, string text, bool good)
	{
		if (!m_Result)
			return;
		m_Result.Show(true);
		INT_UIF_Animator.FadeIn(m_Result, 0.18);
		if (m_ResultBox)
			INT_UIF_Animator.Reveal(m_ResultBox, 0.02, 22, 0.42);
		if (m_ResultTitle)
			m_ResultTitle.SetText(title);
		if (m_ResultAmount)
		{
			string amountText = INT_Bank_UILook.Money(amount);
			m_ResultAmount.SetText(amountText);
		}
		if (m_ResultText)
			m_ResultText.SetText(text);
		int badge = INT_Bank_UILook.Sage();
		string icon = "check";
		if (!good)
		{
			badge = ARGB(255, 226, 158, 72);
			icon = "warning";
		}
		if (m_ResultBadge)
			m_ResultBadge.SetFill(badge);
		if (m_ResultIcon)
			INT_UIF_Widgets.SetIcon(m_ResultIcon, icon);
		bool dispense = kind == INT_Bank_Op.WITHDRAW;
		if (m_Dispenser)
			m_Dispenser.Show(dispense);
		if (dispense)
			PlayDispense();
	}

	// Notes slide out of the slot one after another.
	protected void PlayDispense()
	{
		int n = m_Notes.Count();
		for (int i = 0; i < n; i++)
		{
			Widget note = m_Notes.Get(i);
			if (!note)
				continue;
			INT_UIF_Draw.Move(note, 20 + i * 4, -90);
			float toY = INT_UIF_Draw.S(4 + i * 2);
			INT_UIF_Tween t = INT_UIF_Animator.MoveY(note, toY, 0.45, INT_UIF_Ease.OUT_BACK);
			if (t)
				t.Delay(0.12 + i * 0.22);
		}
	}

	protected bool HasOverlay()
	{
		if (m_Result && m_Result.IsVisible())
			return true;
		return false;
	}

	protected bool IsBusy()
	{
		return m_OpSeq != 0;
	}

	// =================================================================================================
	// Lifecycle
	// =================================================================================================
	override void OnReady()
	{
		HookClient();
		if (m_Preview)
		{
			INT_Bank_UIState.FillSample();
			OnHelloOk();
			return;
		}
		SendHello();
	}

	override void OnShow()
	{
		super.OnShow();
		INT_Bank_UILook.Activate();
		if (!m_OpenSounded)
		{
			m_OpenSounded = true;
			INT_Bank_UISfx.Play(INT_Bank_Sfx.CARD);
		}
		if (m_Card)
			m_Card.Reveal(0.08);
	}

	override void OnHide()
	{
		super.OnHide();
		if (m_OpenSounded)
			INT_Bank_UISfx.Play(INT_Bank_Sfx.CLOSE);
		m_OpenSounded = false;
		UnhookClient();
		INT_Bank_UILook.Restore();
	}

	protected void HookClient()
	{
		if (m_Hooked)
			return;
		m_Hooked = true;
		INT_Bank_Client c = INT_Bank_Client.Get();
		c.OnReply().Insert(OnReply);
		c.OnPush().Insert(OnPush);
	}

	protected void UnhookClient()
	{
		if (!m_Hooked)
			return;
		m_Hooked = false;
		INT_Bank_Client c = INT_Bank_Client.Get();
		if (c && c.OnReply())
			c.OnReply().Remove(OnReply);
		if (c && c.OnPush())
			c.OnPush().Remove(OnPush);
	}

	protected void SendHello()
	{
		m_HelloSeq = INT_Bank_Client.Get().Send(INT_Bank_Op.HELLO);
		if (m_HelloSeq == 0)
			ShowState(true, "#STR_INT_BANK_STATE_ERROR", "#STR_INT_BANK_ERR_INTERNAL", false);
	}

	protected INT_Bank_State St()
	{
		return INT_Bank_Client.Get().State;
	}

	// =================================================================================================
	// Server answers
	// =================================================================================================
	void OnReply(INT_Bank_Reply r)
	{
		if (!r)
			return;
		if (r.Seq == m_HelloSeq && r.Op == INT_Bank_Op.HELLO)
		{
			m_HelloSeq = 0;
			if (r.Status == INT_Bank_Status.OK)
			{
				OnHelloOk();
				return;
			}
			HelloFailed(r.Status);
			return;
		}
		if (r.Seq == m_OpSeq && m_OpSeq != 0)
		{
			m_OpSeq = 0;
			ShowBusy(false);
			OnOpReply(r);
			RefreshAll();
			return;
		}
		if (r.Seq == m_HistSeq && r.Op == INT_Bank_Op.HISTORY)
		{
			m_HistSeq = 0;
			ApplyHistory(r, m_HistList);
			if (r.Status == INT_Bank_Status.OK)
			{
				m_HistPage = r.A;
				m_HistPages = r.B;
				m_HistTotal = r.C;
				RefreshPager();
			}
			RefreshAll();
			return;
		}
		if (r.Seq == m_RecentSeq && r.Op == INT_Bank_Op.HISTORY)
		{
			m_RecentSeq = 0;
			ApplyHistory(r, m_OvRecent);
			RefreshAll();
			return;
		}
		if (r.Seq == m_LookupSeq && r.Op == INT_Bank_Op.LOOKUP)
		{
			m_LookupSeq = 0;
			OnLookup(r);
			return;
		}
		RefreshAll();
	}

	void OnPush(INT_Bank_Reply r)
	{
		if (!r || r.Op != INT_Bank_Op.PUSH_TRANSFER_IN)
			return;
		RefreshAll();
		RequestRecent();
	}

	protected void HelloFailed(int status)
	{
		INT_Bank_UISfx.Play(INT_Bank_Sfx.ERROR);
		string title = "#STR_INT_BANK_STATE_ERROR";
		string text = ErrText(status, 0);
		if (status == INT_Bank_Status.NO_SERVER)
			title = "#STR_INT_BANK_STATE_NOSERVER";
		ShowState(true, title, text, false);
	}

	protected void OnHelloOk()
	{
		m_Ready = true;
		ShowState(false, "", "", false);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		string name = "SURVIVOR";
		string sid = "";
		if (player && player.GetIdentity())
		{
			name = player.GetIdentity().GetName();
			sid = player.GetIdentity().GetPlainId();
		}
		if (m_Preview)
		{
			name = "Survivor";
			sid = "76561198012345678";
		}
		if (m_Card)
			m_Card.SetOwner(name, sid);
		m_ShownBalance = 0;
		INT_Bank_State st = St();
		CountBalance(st.Balance);
		RefreshAll();
		RequestRecent();
		UpdateNotesPreview();
	}

	protected void CountBalance(int to)
	{
		int from = m_ShownBalance;
		m_ShownBalance = to;
		m_CountTween = INT_UIF_Animator.Value(from, to, 0.9, INT_UIF_Ease.OUT_CUBIC);
		if (m_CountTween)
			m_CountTween.OnUpdate().Insert(OnCountTick);
		else
			SetBalanceText(to);
	}

	void OnCountTick(float v, INT_UIF_Tween t)
	{
		int iv = Math.Round(v);
		SetBalanceText(iv);
	}

	protected void SetBalanceText(int v)
	{
		if (!m_OvBalance)
			return;
		string txt = INT_Bank_Util.Money(v);
		m_OvBalance.SetText(txt);
		float tw = INT_Bank_UIMeasure.TextW(m_OvBalance, txt, 60);
		if (m_OvBalanceCur)
			INT_UIF_Draw.Place(m_OvBalanceCur, tw + 14, 56, 140, 30);
	}

	protected void RequestRecent()
	{
		if (m_Preview)
		{
			INT_Bank_Reply sample = INT_Bank_UIState.SampleHistory(0);
			ApplyHistory(sample, m_OvRecent);
			return;
		}
		m_RecentSeq = INT_Bank_Client.Get().Send(INT_Bank_Op.HISTORY, 0);
	}

	protected void RequestHistory(int page)
	{
		if (m_Preview)
		{
			INT_Bank_Reply sample = INT_Bank_UIState.SampleHistory(page);
			ApplyHistory(sample, m_HistList);
			m_HistPage = sample.A;
			m_HistPages = sample.B;
			m_HistTotal = sample.C;
			RefreshPager();
			return;
		}
		m_HistSeq = INT_Bank_Client.Get().Send(INT_Bank_Op.HISTORY, page);
	}

	protected void ApplyHistory(INT_Bank_Reply r, INT_UIF_List list)
	{
		// a refused request (rate, timeout) keeps what the list shows
		if (!list || r.Status != INT_Bank_Status.OK)
			return;
		list.Clear();
		int limit = r.Rows.Count();
		if (list == m_OvRecent && limit > 4)
			limit = 4;
		for (int i = 0; i < limit; i++)
		{
			INT_Bank_HistRow row = r.Rows.Get(i);
			string title = HistTitle(row.Type);
			string sub = row.Time;
			if (row.PeerName != "" || row.PeerId != "")
			{
				string peer = row.PeerName;
				if (peer == "")
					peer = row.PeerId;
				sub = sub + "  ·  " + peer;
			}
			int idx = list.AddRow(title, sub, i);
			bool income = INT_Bank_Util.HistIsIncome(row.Type);
			string hue = INT_UIF_ThemeKeys.RED;
			string icon = "chevron_up";
			string sign = "- ";
			int col = INT_Bank_UILook.Brick();
			if (income)
			{
				hue = INT_UIF_ThemeKeys.GREEN;
				icon = "chevron_down";
				sign = "+ ";
				col = INT_Bank_UILook.Sage();
			}
			if (row.Type == INT_Bank_Hist.TRANSFER_IN || row.Type == INT_Bank_Hist.TRANSFER_OUT)
				icon = "swap";
			if (row.Type == INT_Bank_Hist.BBA_IN || row.Type == INT_Bank_Hist.BBA_OUT)
				icon = "shield";
			if (row.Type == INT_Bank_Hist.QST_IN)
				icon = "star";
			list.SetRowIcon(idx, icon, hue);
			string money = INT_Bank_UILook.Money(row.Amount);
			list.SetRowRight(idx, sign + money, col);
		}
		list.Reveal(0.05);
	}

	protected string HistTitle(int type)
	{
		switch (type)
		{
			case INT_Bank_Hist.DEPOSIT:
				return "#STR_INT_BANK_H_DEPOSIT";
			case INT_Bank_Hist.WITHDRAW:
				return "#STR_INT_BANK_H_WITHDRAW";
			case INT_Bank_Hist.TRANSFER_IN:
				return "#STR_INT_BANK_H_TRANSFER_IN";
			case INT_Bank_Hist.TRANSFER_OUT:
				return "#STR_INT_BANK_H_TRANSFER_OUT";
			case INT_Bank_Hist.P2P_IN:
				return "#STR_INT_BANK_H_P2P_IN";
			case INT_Bank_Hist.P2P_OUT:
				return "#STR_INT_BANK_H_P2P_OUT";
			case INT_Bank_Hist.BBA_IN:
				return "#STR_INT_BANK_H_BBA_IN";
			case INT_Bank_Hist.BBA_OUT:
				return "#STR_INT_BANK_H_BBA_OUT";
			case INT_Bank_Hist.QST_IN:
				return "#STR_INT_BANK_H_QST_IN";
		}
		return "#STR_INT_BANK_H_ADMIN";
	}

	protected void RefreshPager()
	{
		if (m_HistPager)
		{
			int cur = m_HistPage + 1;
			string sCur = cur.ToString();
			string sPages = m_HistPages.ToString();
			string pg = sCur + " / " + sPages;
			m_HistPager.SetText(pg);
		}
		if (m_HistPrev)
			m_HistPrev.SetEnabled(m_HistPage > 0);
		if (m_HistNext)
			m_HistNext.SetEnabled(m_HistPage < m_HistPages - 1);
		if (m_HistCount)
		{
			string cnt = INT_Bank_UILook.Fmt("#STR_INT_BANK_HIST_COUNT", m_HistTotal.ToString());
			m_HistCount.SetText(cnt);
		}
		if (m_HistClear)
			m_HistClear.SetEnabled(m_HistTotal > 0);
	}

	// ---------------------------------------------------------------------------------------------
	// Operation answers
	// ---------------------------------------------------------------------------------------------
	protected void OnOpReply(INT_Bank_Reply r)
	{
		int kind = m_OpKind;
		string sk = kind.ToString();
		string ss = INT_Bank_Util.StatusName(r.Status);
		string sa = r.A.ToString();
		string sb = r.B.ToString();
		string sc = r.C.ToString();
		Print("[INT_BANK] [LIVE] reply op=" + sk + " status=" + ss + " a=" + sa + " b=" + sb + " c=" + sc);
		// NEED_GROUND only comes from an old server part (<= 1.0.4): same notice, nothing is given
		if (r.Status == INT_Bank_Status.NO_ROOM || r.Status == INT_Bank_Status.NEED_GROUND)
		{
			INT_ShowNoRoom();
			return;
		}
		if (r.Status != INT_Bank_Status.OK)
		{
			ShowError(r.Status, r.A);
			return;
		}
		INT_Bank_State st = St();
		CountBalance(st.Balance);
		RequestRecent();
		if (m_Page == PAGE_HISTORY)
			RequestHistory(0);
		if (kind == INT_Bank_Op.DEPOSIT)
		{
			INT_Bank_UISfx.Play(INT_Bank_Sfx.CASH_IN);
			INT_Bank_UISfx.Play(INT_Bank_Sfx.CONFIRM);
			string dt = INT_Bank_UILook.Tr("#STR_INT_BANK_RES_DEPOSIT_TEXT");
			// B = whole change, D = the part credited to the account (did not fit / not payable in notes)
			int changeCash = r.B - r.D;
			if (changeCash > 0)
			{
				string ch = INT_Bank_UILook.Money(changeCash);
				dt = dt + " " + INT_Bank_UILook.Fmt("#STR_INT_BANK_RES_CHANGE", ch);
			}
			if (r.D > 0)
			{
				string cc = INT_Bank_UILook.Money(r.D);
				dt = dt + " " + INT_Bank_UILook.Fmt("#STR_INT_BANK_RES_CHANGE_CREDITED", cc);
			}
			m_DepAmount.Clear();
			ShowResult(kind, INT_Bank_UILook.Tr("#STR_INT_BANK_RES_DEPOSIT"), r.A, dt, true);
			return;
		}
		if (kind == INT_Bank_Op.WITHDRAW)
		{
			INT_Bank_UISfx.Play(INT_Bank_Sfx.CASH_OUT);
			string wt = INT_Bank_UILook.Tr("#STR_INT_BANK_RES_WITHDRAW_TEXT");
			bool good = true;
			if (r.C > 0)
			{
				string f = INT_Bank_UILook.Money(r.C);
				wt = wt + " " + INT_Bank_UILook.Fmt("#STR_INT_BANK_RES_REFUND", f);
				good = false;
			}
			m_WdAmount.Clear();
			ShowResult(kind, INT_Bank_UILook.Tr("#STR_INT_BANK_RES_WITHDRAW"), r.A, wt, good);
			return;
		}
		if (kind == INT_Bank_Op.TRANSFER)
		{
			INT_Bank_UISfx.Play(INT_Bank_Sfx.CONFIRM);
			string who = r.S1;
			if (who == "")
				who = r.S2;
			string tt = INT_Bank_UILook.Fmt("#STR_INT_BANK_RES_TRANSFER_TEXT", who, r.S2);
			m_TrAmount.Clear();
			ShowResult(kind, INT_Bank_UILook.Tr("#STR_INT_BANK_RES_TRANSFER"), r.A, tt, true);
			return;
		}
		if (kind == INT_Bank_Op.CLEAR_HISTORY)
		{
			INT_Bank_UISfx.Play(INT_Bank_Sfx.CONFIRM);
			INT_UIF_Toast.Success("#STR_INT_BANK_HIST_CLEARED");
			RequestHistory(0);
			return;
		}
	}

	// Not all notes fit into the inventory: nothing was given (owner 2026-11-06 - money never goes on the
	// ground). The player sorts the inventory and tries again.
	protected void INT_ShowNoRoom()
	{
		INT_Bank_UISfx.Play(INT_Bank_Sfx.ERROR);
		m_Dialog = INT_Bank_UIDialog.Notice("#STR_INT_BANK_NOROOM_TITLE", "#STR_INT_BANK_NOROOM_TEXT", "#STR_INT_BANK_NOROOM_OK", "warning");
	}

	protected string ErrText(int status, int a)
	{
		string key = "#STR_INT_BANK_ERR_" + INT_Bank_Util.StatusName(status);
		string arg = INT_Bank_Util.Money(a);
		if (status == INT_Bank_Status.COOLDOWN)
			arg = a.ToString();
		return INT_Bank_UILook.Fmt(key, arg);
	}

	protected void ShowError(int status, int a)
	{
		INT_Bank_UISfx.Play(INT_Bank_Sfx.ERROR);
		string text = ErrText(status, a);
		int kind = INT_UIF_ToastKind.ERROR;
		if (status == INT_Bank_Status.COOLDOWN || status == INT_Bank_Status.RATE)
			kind = INT_UIF_ToastKind.WARNING;
		INT_UIF_Toast.Show(text, kind, 4.5);
	}

	// =================================================================================================
	// Actions
	// =================================================================================================
	protected void StartOp(int op, int a, int b, int c, string s)
	{
		if (IsBusy())
			return;
		m_OpKind = op;
		m_OpAmount = a;
		m_OpTarget = s;
		if (m_Preview)
		{
			INT_Bank_Reply fake = INT_Bank_UIState.SampleOp(op, a, b, c, s);
			OnOpReply(fake);
			RefreshAll();
			return;
		}
		m_OpSeq = INT_Bank_Client.Get().Send(op, a, b, c, s);
		if (m_OpSeq != 0)
			ShowBusy(true);
	}

	protected int Room()
	{
		INT_Bank_State st = St();
		if (st.MaxBalance <= 0)
			return 2000000000;
		int room = st.MaxBalance - st.Balance;
		if (room < 0)
			room = 0;
		return room;
	}

	protected int Cash()
	{
		int localCash = INT_Bank_Client.Get().CountCashLocal();
		if (localCash >= 0)
			return localCash;
		return St().Cash;
	}

	void OnDepositGo(INT_UIF_Button b)
	{
		SubmitDeposit();
	}

	protected void SubmitDeposit()
	{
		if (!m_Ready || IsBusy())
			return;
		INT_Bank_State st = St();
		int amount = m_DepAmount.Get();
		if (amount <= 0)
		{
			m_DepAmount.Shake();
			return;
		}
		if (st.CdDeposit > 0)
		{
			ShowError(INT_Bank_Status.COOLDOWN, st.CdDeposit);
			return;
		}
		int cash = Cash();
		if (amount > cash)
		{
			m_DepAmount.Shake();
			ShowError(INT_Bank_Status.NO_CASH, cash);
			return;
		}
		if (st.MaxDeposit > 0 && amount > st.MaxDeposit)
		{
			m_DepAmount.Shake();
			ShowError(INT_Bank_Status.LIMIT_OP, st.MaxDeposit);
			return;
		}
		if (amount > Room())
		{
			m_DepAmount.Shake();
			ShowError(INT_Bank_Status.LIMIT_BALANCE, st.MaxBalance);
			return;
		}
		StartOp(INT_Bank_Op.DEPOSIT, amount, 0, 0, "");
	}

	void OnDepositAll(INT_UIF_Button b)
	{
		if (!m_Ready || IsBusy())
			return;
		INT_Bank_State st = St();
		if (st.CdDeposit > 0)
		{
			ShowError(INT_Bank_Status.COOLDOWN, st.CdDeposit);
			return;
		}
		int cash = Cash();
		if (cash <= 0)
		{
			ShowError(INT_Bank_Status.NO_CASH, 0);
			return;
		}
		StartOp(INT_Bank_Op.DEPOSIT, 0, 1, 0, "");
	}

	void OnWithdrawGo(INT_UIF_Button b)
	{
		SubmitWithdraw();
	}

	protected void SubmitWithdraw()
	{
		if (!m_Ready || IsBusy())
			return;
		INT_Bank_State st = St();
		int amount = m_WdAmount.Get();
		if (amount <= 0)
		{
			m_WdAmount.Shake();
			return;
		}
		if (st.CdWithdraw > 0)
		{
			ShowError(INT_Bank_Status.COOLDOWN, st.CdWithdraw);
			return;
		}
		if (st.MaxWithdraw > 0 && amount > st.MaxWithdraw)
		{
			m_WdAmount.Shake();
			ShowError(INT_Bank_Status.LIMIT_OP, st.MaxWithdraw);
			return;
		}
		if (amount > st.Balance)
		{
			m_WdAmount.Shake();
			ShowError(INT_Bank_Status.NO_FUNDS, st.Balance);
			return;
		}
		StartOp(INT_Bank_Op.WITHDRAW, amount, 0, 0, "");
	}

	void OnTransferGo(INT_UIF_Button b)
	{
		SubmitTransfer();
	}

	protected void SubmitTransfer()
	{
		if (!m_Ready || IsBusy())
			return;
		INT_Bank_State st = St();
		string target = INT_Bank_Util.Digits(m_TrId.GetText());
		int amount = m_TrAmount.Get();
		if (!INT_Bank_Util.IsSteam64(target))
		{
			ShowError(INT_Bank_Status.TARGET_INVALID, 0);
			return;
		}
		if (!m_TrValid || m_TrLookedUp != target)
		{
			ShowError(INT_Bank_Status.TARGET_NOT_FOUND, 0);
			return;
		}
		if (amount <= 0)
		{
			m_TrAmount.Shake();
			return;
		}
		if (st.CdTransfer > 0)
		{
			ShowError(INT_Bank_Status.COOLDOWN, st.CdTransfer);
			return;
		}
		if (st.MaxTransfer > 0 && amount > st.MaxTransfer)
		{
			m_TrAmount.Shake();
			ShowError(INT_Bank_Status.LIMIT_OP, st.MaxTransfer);
			return;
		}
		if (amount > st.Balance)
		{
			m_TrAmount.Shake();
			ShowError(INT_Bank_Status.NO_FUNDS, st.Balance);
			return;
		}
		string who = m_TrName;
		if (who == "")
			who = target;
		string sum = INT_Bank_UILook.Money(amount);
		string text = INT_Bank_UILook.Fmt("#STR_INT_BANK_TR_CONFIRM_TEXT", sum, who, target);
		m_OpAmount = amount;
		m_OpTarget = target;
		m_Dialog = INT_Bank_UIDialog.Ask("#STR_INT_BANK_TR_CONFIRM_TITLE", text, "#STR_INT_BANK_TR_GO", "#STR_INT_BANK_CANCEL", INT_UIF_Variant.PRIMARY, "swap");
		if (m_Dialog)
			m_Dialog.OnResult().Insert(OnTransferConfirm);
	}

	void OnTransferConfirm(int result, INT_UIF_Dialog d)
	{
		if (result != INT_UIF_Dialog.RESULT_YES)
			return;
		StartOp(INT_Bank_Op.TRANSFER, m_OpAmount, 0, 0, m_OpTarget);
	}

	void OnTrIdChanged(string text, INT_UIF_TextInput input)
	{
		string digits = INT_Bank_Util.Digits(text);
		if (digits != text)
		{
			m_TrId.SetText(digits);
			return;
		}
		INT_Bank_UISfx.Play(INT_Bank_Sfx.KEY);
		m_TrValid = false;
		m_TrName = "";
		if (INT_Bank_Util.IsSteam64(digits))
		{
			SetTrStatus("#STR_INT_BANK_TR_CHECKING", INT_Bank_UILook.Muted(), "clock");
			m_TrLookupWait = 0.35;
		}
		else
		{
			m_TrLookupWait = -1;
			if (digits == "")
				SetTrStatus("#STR_INT_BANK_TR_ENTER_ID", INT_Bank_UILook.Faint(), "info");
			else
				SetTrStatus("#STR_INT_BANK_TR_BAD_ID", INT_Bank_UILook.Faint(), "info");
		}
		RefreshAll();
	}

	protected void SetTrStatus(string text, int color, string icon)
	{
		if (m_TrStatus)
		{
			m_TrStatus.SetText(text);
			m_TrStatus.SetColor(color);
		}
		if (m_TrStatusIcon)
		{
			INT_UIF_Widgets.SetIcon(m_TrStatusIcon, icon);
			m_TrStatusIcon.SetColor(color);
		}
	}

	protected void SendLookup()
	{
		string target = INT_Bank_Util.Digits(m_TrId.GetText());
		if (!INT_Bank_Util.IsSteam64(target))
			return;
		if (m_Preview)
		{
			INT_Bank_Reply fake = INT_Bank_UIState.SampleLookup(target);
			OnLookup(fake);
			return;
		}
		m_LookupSeq = INT_Bank_Client.Get().Send(INT_Bank_Op.LOOKUP, 0, 0, 0, target);
	}

	protected void OnLookup(INT_Bank_Reply r)
	{
		string current = INT_Bank_Util.Digits(m_TrId.GetText());
		if (r.S2 != current)
			return;
		m_TrLookedUp = current;
		if (r.Status == INT_Bank_Status.OK)
		{
			m_TrValid = true;
			m_TrName = r.S1;
			string shown = r.S1;
			if (shown == "")
				shown = INT_Bank_UILook.Tr("#STR_INT_BANK_TR_NONAME");
			string line = INT_Bank_UILook.Fmt("#STR_INT_BANK_TR_FOUND", shown);
			SetTrStatus(line, INT_Bank_UILook.Sage(), "check");
			INT_Bank_UISfx.Play(INT_Bank_Sfx.SELECT);
		}
		else
		{
			m_TrValid = false;
			m_TrName = "";
			string err = ErrText(r.Status, 0);
			SetTrStatus(err, INT_Bank_UILook.Brick(), "error");
		}
		RefreshAll();
	}

	void OnTrPaste(INT_UIF_IconButton b)
	{
		string clip;
		GetGame().CopyFromClipboard(clip);
		string digits = INT_Bank_Util.Digits(clip);
		if (digits.Length() > 17)
			digits = digits.Substring(0, 17);
		m_TrId.SetText(digits);
		OnTrIdChanged(digits, m_TrId);
	}

	void OnHistPrev(INT_UIF_IconButton b)
	{
		if (m_HistPage > 0)
			RequestHistory(m_HistPage - 1);
	}

	void OnHistNext(INT_UIF_IconButton b)
	{
		if (m_HistPage < m_HistPages - 1)
			RequestHistory(m_HistPage + 1);
	}

	void OnHistClear(INT_UIF_Button b)
	{
		if (IsBusy())
			return;
		m_Dialog = INT_Bank_UIDialog.Ask("#STR_INT_BANK_HIST_CLEAR", "#STR_INT_BANK_HIST_CLEAR_TEXT", "#STR_INT_BANK_HIST_CLEAR_YES", "#STR_INT_BANK_CANCEL", INT_UIF_Variant.DANGER, "trash");
		if (m_Dialog)
			m_Dialog.OnResult().Insert(OnHistClearConfirm);
	}

	void OnHistClearConfirm(int result, INT_UIF_Dialog d)
	{
		if (result != INT_UIF_Dialog.RESULT_YES)
			return;
		StartOp(INT_Bank_Op.CLEAR_HISTORY, 0, 0, 0, "");
	}

	void OnResultClose(INT_UIF_Button b)
	{
		CloseResult();
	}

	protected void CloseResult()
	{
		if (m_Result)
			INT_UIF_Animator.FadeOut(m_Result, 0.16);
	}

	void OnQuickDeposit(INT_UIF_Button b)
	{
		GoPage(PAGE_DEPOSIT);
	}

	void OnQuickWithdraw(INT_UIF_Button b)
	{
		GoPage(PAGE_WITHDRAW);
	}

	void OnQuickTransfer(INT_UIF_Button b)
	{
		GoPage(PAGE_TRANSFER);
	}

	protected INT_Bank_UIAmount ActiveAmount()
	{
		if (m_Page == PAGE_DEPOSIT)
			return m_DepAmount;
		if (m_Page == PAGE_WITHDRAW)
			return m_WdAmount;
		if (m_Page == PAGE_TRANSFER)
			return m_TrAmount;
		return null;
	}

	protected INT_Bank_UIKeypad ActiveKeys()
	{
		if (m_Page == PAGE_DEPOSIT)
			return m_DepKeys;
		if (m_Page == PAGE_WITHDRAW)
			return m_WdKeys;
		if (m_Page == PAGE_TRANSFER)
			return m_TrKeys;
		return null;
	}

	// Largest sensible amount for the current page.
	protected int PageMax()
	{
		INT_Bank_State st = St();
		int v = 0;
		if (m_Page == PAGE_DEPOSIT)
		{
			v = Cash();
			if (st.MaxDeposit > 0 && v > st.MaxDeposit)
				v = st.MaxDeposit;
			int room = Room();
			if (v > room)
				v = room;
		}
		else if (m_Page == PAGE_WITHDRAW)
		{
			v = st.Balance;
			if (st.MaxWithdraw > 0 && v > st.MaxWithdraw)
				v = st.MaxWithdraw;
		}
		else if (m_Page == PAGE_TRANSFER)
		{
			v = st.Balance;
			if (st.MaxTransfer > 0 && v > st.MaxTransfer)
				v = st.MaxTransfer;
		}
		if (v < 0)
			v = 0;
		return v;
	}

	void OnQuickSum(INT_UIF_Button b)
	{
		INT_Bank_UIAmount a = ActiveAmount();
		if (!a)
			return;
		string label = b.GetText();
		int add = INT_Bank_Util.ParseAmount(label);
		int now = a.Get();
		int sum = now + add;
		int max = PageMax();
		if (sum > max)
		{
			sum = max;
			a.Shake();
		}
		a.Set(sum);
		AmountChanged();
	}

	void OnQuickMax(INT_UIF_Button b)
	{
		INT_Bank_UIAmount a = ActiveAmount();
		if (!a)
			return;
		int max = PageMax();
		a.Set(max);
		AmountChanged();
	}

	void OnQuickClear(INT_UIF_Button b)
	{
		INT_Bank_UIAmount a = ActiveAmount();
		if (!a)
			return;
		a.Clear();
		AmountChanged();
	}

	void OnKeypad(string key)
	{
		ApplyKey(key);
	}

	protected void ApplyKey(string key)
	{
		INT_Bank_UIAmount a = ActiveAmount();
		if (!a)
			return;
		if (key == "C")
			a.Clear();
		else if (key == "B")
			a.Back();
		else
			a.Append(key);
		AmountChanged();
	}

	protected void AmountChanged()
	{
		if (m_Page == PAGE_WITHDRAW)
			UpdateNotesPreview();
		RefreshAll();
	}

	protected void UpdateNotesPreview()
	{
		if (!m_WdNotes)
			return;
		int amount = m_WdAmount.Get();
		if (amount <= 0)
		{
			m_WdNotes.SetText("-");
			return;
		}
		array<string> classes = new array<string>();
		array<int> pieces = new array<int>();
		bool ok = INT_Bank_Client.Get().PreviewNotes(amount, classes, pieces);
		if (!ok || classes.Count() == 0)
		{
			string bad = INT_Bank_UILook.Tr("#STR_INT_BANK_ERR_NOT_PAYABLE");
			m_WdNotes.SetText(bad);
			return;
		}
		string txt = "";
		int n = classes.Count();
		for (int i = 0; i < n; i++)
		{
			string cls = classes.Get(i);
			int pc = pieces.Get(i);
			string nm = INT_Bank_UIState.NoteName(cls);
			if (txt != "")
				txt = txt + "   ";
			txt = txt + nm + " x" + pc.ToString();
		}
		m_WdNotes.SetText(txt);
	}

	// =================================================================================================
	// Pages
	// =================================================================================================
	void OnNav(int index)
	{
		ShowPage(index, true);
	}

	protected void GoPage(int index)
	{
		if (m_Nav)
		{
			if (index != m_Page)
				INT_Bank_UISfx.Play(INT_Bank_Sfx.TAB);
			m_Nav.Select(index, false, true);
		}
		ShowPage(index, true);
	}

	protected void ShowPage(int index, bool animate)
	{
		if (index < 0 || index >= m_Pages.Count())
			return;
		int prev = m_Page;
		m_Page = index;
		int n = m_Pages.Count();
		for (int i = 0; i < n; i++)
		{
			Widget p = m_Pages.Get(i);
			if (!p)
				continue;
			bool on = i == index;
			p.Show(on);
		}
		Widget cur = m_Pages.Get(index);
		if (animate && cur && prev != index)
		{
			INT_UIF_Draw.Move(cur, 0, 0);
			INT_UIF_Animator.Reveal(cur, 0, 12, 0.3);
		}
		if (m_DepAmount)
			m_DepAmount.SetActive(index == PAGE_DEPOSIT);
		if (m_WdAmount)
			m_WdAmount.SetActive(index == PAGE_WITHDRAW);
		if (m_TrAmount)
			m_TrAmount.SetActive(index == PAGE_TRANSFER);
		if (index == PAGE_HISTORY && m_Ready)
			RequestHistory(0);
		if (index == PAGE_WITHDRAW)
			UpdateNotesPreview();
		SetFocus(null);
		RefreshAll();
	}

	// =================================================================================================
	// Refresh (labels from the cached state)
	// =================================================================================================
	protected void RefreshAll()
	{
		INT_Bank_State st = St();
		int cash = Cash();
		if (m_Card)
			m_Card.SetBalance(st.Balance);
		string sCash = INT_Bank_UILook.Money(cash);
		string sBal = INT_Bank_UILook.Money(st.Balance);
		// overview
		if (m_OvCash)
			m_OvCash.SetText(INT_Bank_Util.Money(cash));
		int avail = st.Balance;
		if (st.MaxWithdraw > 0 && avail > st.MaxWithdraw)
			avail = st.MaxWithdraw;
		if (m_OvAvail)
			m_OvAvail.SetText(INT_Bank_Util.Money(avail));
		if (m_OvOps)
			m_OvOps.SetText(st.HistoryCount.ToString());
		if (m_OvLimitText)
		{
			string lim;
			if (st.MaxBalance > 0)
			{
				string sMax = INT_Bank_UILook.Money(st.MaxBalance);
				lim = INT_Bank_UILook.Fmt("#STR_INT_BANK_OV_LIMIT", sMax);
			}
			else
			{
				lim = INT_Bank_UILook.Tr("#STR_INT_BANK_OV_NOLIMIT");
			}
			m_OvLimitText.SetText(lim);
		}
		if (m_OvLimitBar)
		{
			float frac = 0;
			if (st.MaxBalance > 0)
				frac = st.Balance / (float)st.MaxBalance;
			m_OvLimitBar.SetValue(Math.Clamp(frac, 0, 1));
		}
		// deposit
		int dep = 0;
		if (m_DepAmount)
			dep = m_DepAmount.Get();
		SetText(m_DepCash, sCash);
		SetText(m_DepNow, sBal);
		int after = st.Balance + dep;
		string sAfter = INT_Bank_UILook.Money(after);
		SetText(m_DepAfter, sAfter);
		if (m_DepAmount)
		{
			string dh = INT_Bank_UILook.Fmt("#STR_INT_BANK_DEP_HINT", sCash);
			int dhc = INT_Bank_UILook.Muted();
			if (dep > cash)
			{
				dh = INT_Bank_UILook.Fmt("#STR_INT_BANK_ERR_NO_CASH", sCash);
				dhc = INT_Bank_UILook.Brick();
			}
			m_DepAmount.SetHint(dh, dhc);
		}
		// withdraw
		int wd = 0;
		if (m_WdAmount)
			wd = m_WdAmount.Get();
		SetText(m_WdNow, sBal);
		string sLimit = INT_Bank_UILook.Tr("#STR_INT_BANK_NOLIMIT");
		if (st.MaxWithdraw > 0)
			sLimit = INT_Bank_UILook.Money(st.MaxWithdraw);
		SetText(m_WdLimit, sLimit);
		int left = st.Balance - wd;
		string sLeft = INT_Bank_UILook.Money(left);
		SetText(m_WdAfter, sLeft);
		if (m_WdAfter)
		{
			if (left < 0)
				m_WdAfter.SetColor(INT_Bank_UILook.Brick());
			else
				m_WdAfter.SetColor(INT_Bank_UILook.Ink());
		}
		if (m_WdAmount)
		{
			string wh = INT_Bank_UILook.Fmt("#STR_INT_BANK_WD_HINT", sBal);
			int whc = INT_Bank_UILook.Muted();
			if (wd > st.Balance)
			{
				wh = INT_Bank_UILook.Fmt("#STR_INT_BANK_ERR_NO_FUNDS", sBal);
				whc = INT_Bank_UILook.Brick();
			}
			else if (st.MaxWithdraw > 0 && wd > st.MaxWithdraw)
			{
				string sMw = INT_Bank_UILook.Money(st.MaxWithdraw);
				wh = INT_Bank_UILook.Fmt("#STR_INT_BANK_ERR_LIMIT_OP", sMw);
				whc = INT_Bank_UILook.Brick();
			}
			m_WdAmount.SetHint(wh, whc);
		}
		// transfer
		int tr = 0;
		if (m_TrAmount)
			tr = m_TrAmount.Get();
		SetText(m_TrNow, sBal);
		int trLeft = st.Balance - tr;
		string sTrLeft = INT_Bank_UILook.Money(trLeft);
		SetText(m_TrAfter, sTrLeft);
		if (m_TrAmount)
		{
			string th = INT_Bank_UILook.Fmt("#STR_INT_BANK_WD_HINT", sBal);
			int thc = INT_Bank_UILook.Muted();
			if (tr > st.Balance)
			{
				th = INT_Bank_UILook.Fmt("#STR_INT_BANK_ERR_NO_FUNDS", sBal);
				thc = INT_Bank_UILook.Brick();
			}
			m_TrAmount.SetHint(th, thc);
		}
		RefreshButtons();
		RefreshCooldowns();
	}

	protected void SetText(TextWidget w, string text)
	{
		if (w)
			w.SetText(text);
	}

	protected void RefreshButtons()
	{
		INT_Bank_State st = St();
		bool busy = IsBusy() || !m_Ready;
		if (m_DepGo)
		{
			string dl = INT_Bank_UILook.Tr("#STR_INT_BANK_DEP_GO");
			if (st.CdDeposit > 0)
				dl = dl + " (" + st.CdDeposit.ToString() + ")";
			m_DepGo.SetText(dl);
			m_DepGo.SetEnabled(!busy && st.CdDeposit <= 0);
		}
		if (m_DepAll)
			m_DepAll.SetEnabled(!busy && st.CdDeposit <= 0);
		if (m_WdGo)
		{
			string wl = INT_Bank_UILook.Tr("#STR_INT_BANK_WD_GO");
			if (st.CdWithdraw > 0)
				wl = wl + " (" + st.CdWithdraw.ToString() + ")";
			m_WdGo.SetText(wl);
			m_WdGo.SetEnabled(!busy && st.CdWithdraw <= 0);
		}
		if (m_TrGo)
		{
			string tl = INT_Bank_UILook.Tr("#STR_INT_BANK_TR_GO");
			if (st.CdTransfer > 0)
				tl = tl + " (" + st.CdTransfer.ToString() + ")";
			m_TrGo.SetText(tl);
			m_TrGo.SetEnabled(!busy && st.CdTransfer <= 0 && m_TrValid);
		}
	}

	protected void RefreshCooldowns()
	{
		INT_Bank_State st = St();
		CdText(m_CdDep, st.CdDeposit);
		CdText(m_CdWd, st.CdWithdraw);
		CdText(m_CdTr, st.CdTransfer);
	}

	protected void CdText(TextWidget w, int left)
	{
		if (!w)
			return;
		if (left <= 0)
		{
			w.SetText("#STR_INT_BANK_CD_READY");
			w.SetColor(INT_Bank_UILook.Sage());
			return;
		}
		string t = INT_Bank_UILook.Fmt("#STR_INT_BANK_CD_WAIT", left.ToString());
		w.SetText(t);
		w.SetColor(ARGB(255, 226, 158, 72));
	}

	// =================================================================================================
	// Frame update: keyboard, timers, cooldown countdown, cash watch
	// =================================================================================================
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (m_Card)
			m_Card.Tick(timeslice);
		if (m_DepKeys)
			m_DepKeys.Tick(timeslice);
		if (m_WdKeys)
			m_WdKeys.Tick(timeslice);
		if (m_TrKeys)
			m_TrKeys.Tick(timeslice);
		if (m_TrLookupWait >= 0)
		{
			m_TrLookupWait = m_TrLookupWait - timeslice;
			if (m_TrLookupWait < 0)
				SendLookup();
		}
		// cooldowns tick down locally between server answers
		m_CdTick = m_CdTick + timeslice;
		if (m_CdTick >= 1.0)
		{
			m_CdTick = 0;
			INT_Bank_State st = St();
			bool any = false;
			if (st.CdDeposit > 0)
			{
				st.CdDeposit = st.CdDeposit - 1;
				any = true;
			}
			if (st.CdWithdraw > 0)
			{
				st.CdWithdraw = st.CdWithdraw - 1;
				any = true;
			}
			if (st.CdTransfer > 0)
			{
				st.CdTransfer = st.CdTransfer - 1;
				any = true;
			}
			int cash = Cash();
			if (any || cash != m_LastCash)
			{
				m_LastCash = cash;
				RefreshAll();
			}
		}
		HandleKeys();
	}

	protected bool Edge(int key)
	{
		bool down = KeyState(key) != 0;
		bool was = false;
		if (m_KeyWas.Contains(key))
			was = m_KeyWas.Get(key);
		m_KeyWas.Set(key, down);
		return down && !was;
	}

	protected void HandleKeys()
	{
		// Esc on release: overlay -> dialog -> window
		bool escDown = KeyState(KeyCode.KC_ESCAPE) != 0;
		bool escUp = m_EscWas && !escDown;
		m_EscWas = escDown;
		if (escUp)
		{
			OnEscape();
			return;
		}
		if (HasModal() || IsClosingDown())
			return;
		// F1..F5 switch pages
		if (Edge(KeyCode.KC_F1))
			NavKey(PAGE_OVERVIEW);
		if (Edge(KeyCode.KC_F2))
			NavKey(PAGE_DEPOSIT);
		if (Edge(KeyCode.KC_F3))
			NavKey(PAGE_WITHDRAW);
		if (Edge(KeyCode.KC_F4))
			NavKey(PAGE_TRANSFER);
		if (Edge(KeyCode.KC_F5))
			NavKey(PAGE_HISTORY);
		// a focused edit box (Steam64 field) takes the keyboard itself
		Widget focus = GetFocus();
		if (focus && EditBoxWidget.Cast(focus))
		{
			ResetDigitKeys();
			return;
		}
		INT_Bank_UIKeypad keys = ActiveKeys();
		if (!keys || HasOverlay() || IsBusy())
		{
			ResetDigitKeys();
			return;
		}
		DigitKey(KeyCode.KC_0, KeyCode.KC_NUMPAD0, "0", keys);
		DigitKey(KeyCode.KC_1, KeyCode.KC_NUMPAD1, "1", keys);
		DigitKey(KeyCode.KC_2, KeyCode.KC_NUMPAD2, "2", keys);
		DigitKey(KeyCode.KC_3, KeyCode.KC_NUMPAD3, "3", keys);
		DigitKey(KeyCode.KC_4, KeyCode.KC_NUMPAD4, "4", keys);
		DigitKey(KeyCode.KC_5, KeyCode.KC_NUMPAD5, "5", keys);
		DigitKey(KeyCode.KC_6, KeyCode.KC_NUMPAD6, "6", keys);
		DigitKey(KeyCode.KC_7, KeyCode.KC_NUMPAD7, "7", keys);
		DigitKey(KeyCode.KC_8, KeyCode.KC_NUMPAD8, "8", keys);
		DigitKey(KeyCode.KC_9, KeyCode.KC_NUMPAD9, "9", keys);
		if (Edge(KeyCode.KC_BACK))
		{
			keys.PressVisual("B");
			ApplyKey("B");
		}
		if (Edge(KeyCode.KC_DELETE))
		{
			keys.PressVisual("C");
			ApplyKey("C");
		}
		bool enter = Edge(KeyCode.KC_RETURN);
		bool enterPad = Edge(KeyCode.KC_NUMPADENTER);
		if (enter || enterPad)
			SubmitPage();
	}

	protected void ResetDigitKeys()
	{
		m_KeyWas.Clear();
	}

	protected void DigitKey(int main, int pad, string digit, INT_Bank_UIKeypad keys)
	{
		bool a = Edge(main);
		bool b = Edge(pad);
		if (!a && !b)
			return;
		keys.PressVisual(digit);
		ApplyKey(digit);
	}

	protected void NavKey(int page)
	{
		if (HasOverlay())
			return;
		GoPage(page);
	}

	protected void SubmitPage()
	{
		INT_Bank_UISfx.Play(INT_Bank_Sfx.CLICK);
		if (m_Page == PAGE_DEPOSIT)
			SubmitDeposit();
		else if (m_Page == PAGE_WITHDRAW)
			SubmitWithdraw();
		else if (m_Page == PAGE_TRANSFER)
			SubmitTransfer();
	}

	protected void OnEscape()
	{
		if (INT_UIF_Dropdown.CloseActive())
			return;
		if (HasModal() && m_Modal)
		{
			m_Modal.Cancel();
			return;
		}
		if (m_Result && m_Result.IsVisible())
		{
			CloseResult();
			return;
		}
		if (IsBusy())
			return;
		RequestClose();
	}

	// =================================================================================================
	// Test hook (preview / screenshots): "overview", "deposit", "withdraw", "transfer", "history",
	// "result", "noroom", "busy", "noserver"
	// =================================================================================================
	// Live test step (int_bank_live.txt): drives the REAL window against the server.
	//   page N | amount N | id STEAM64 | submit | depall | yes | no | ok | state
	void LiveCommand(string command)
	{
		string c = command;
		c.TrimInPlace();
		array<string> parts = new array<string>();
		c.Split(" ", parts);
		if (parts.Count() == 0)
			return;
		string verb = parts.Get(0);
		string arg = "";
		if (parts.Count() > 1)
			arg = parts.Get(1);
		if (verb == "page")
			GoPage(arg.ToInt());
		else if (verb == "amount")
		{
			INT_Bank_UIAmount a = ActiveAmount();
			if (a)
			{
				a.Set(arg.ToInt());
				AmountChanged();
			}
		}
		else if (verb == "id")
		{
			m_TrId.SetText(arg);
			OnTrIdChanged(arg, m_TrId);
		}
		else if (verb == "submit")
			SubmitPage();
		else if (verb == "depall")
			OnDepositAll(null);
		else if (verb == "yes" && m_Dialog)
			m_Dialog.Finish(INT_UIF_Dialog.RESULT_YES);
		else if (verb == "no" && m_Dialog)
			m_Dialog.Finish(INT_UIF_Dialog.RESULT_NO);
		else if (verb == "ok")
			CloseResult();
		INT_Bank_State st = St();
		string sBal = st.Balance.ToString();
		int cash = Cash();
		string sCash = cash.ToString();
		string sCd = st.CdDeposit.ToString();
		string sCw = st.CdWithdraw.ToString();
		string line = "[INT_BANK] [LIVE] " + c + " -> balance=" + sBal + " cash=" + sCash + " cdDep=" + sCd + " cdWd=" + sCw;
		Print("" + line);
	}

	override void OnTestCommand(string command)
	{
		string c = command;
		c.ToLower();
		if (c.IndexOf("deposit") != -1)
		{
			GoPage(PAGE_DEPOSIT);
			m_DepAmount.Set(12500);
			AmountChanged();
		}
		else if (c.IndexOf("withdraw") != -1)
		{
			GoPage(PAGE_WITHDRAW);
			m_WdAmount.Set(17385);
			AmountChanged();
		}
		else if (c.IndexOf("transfer") != -1)
		{
			GoPage(PAGE_TRANSFER);
			m_TrId.SetText("76561198087654321");
			OnTrIdChanged("76561198087654321", m_TrId);
			m_TrLookupWait = -1;
			SendLookup();
			m_TrAmount.Set(25000);
			AmountChanged();
		}
		else if (c.IndexOf("history") != -1)
		{
			GoPage(PAGE_HISTORY);
		}
		if (c.IndexOf("result") != -1)
		{
			m_OpKind = INT_Bank_Op.WITHDRAW;
			INT_Bank_Reply ok = INT_Bank_UIState.SampleOp(INT_Bank_Op.WITHDRAW, 17385, 0, 1, "");
			OnOpReply(ok);
		}
		if (c.IndexOf("noroom") != -1)
		{
			m_OpKind = INT_Bank_Op.WITHDRAW;
			m_OpAmount = 17385;
			INT_ShowNoRoom();
		}
		if (c.IndexOf("busy") != -1)
			ShowBusy(true);
		if (c.IndexOf("noserver") != -1)
			HelloFailed(INT_Bank_Status.NO_SERVER);
	}
}
