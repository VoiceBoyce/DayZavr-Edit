// INT_PDA (client) — «СВЯЗЬ» tab (owner brief «Вкладка чат», concept E chat frame 5_e_zone2_chat). Sub-tabs «Общий /
// Группировка / Личные» + «Обновить»; on the left the private dialogs (newest first, unread badges, «Новый диалог»),
// the nick-colour legend and the «Эфир» plate; on the right the message feed (avatar / «НЕТ ДАННЫХ» plate / faction
// emblem, callsign coloured by relation, faction, time, wrapped text) and the input bar (counter, «Анонимно»,
// «От имени ГП», send). A right click on a dialog card asks «Удалить диалог?» (owner 2026-10-23): yes drops the whole
// dialog from this client (private lines live only here; the server mail is handed out once). Text widgets are not cut by clipchildren, so the feed scrolls by WHOLE messages: a fixed pool
// of row widgets is filled with the messages that fit, newest at the bottom. Lines are wrapped in script with engine
// metrics (a transparent probe TextWidget, widths cached per string). Everything comes from INT_PDA_Client (cache +
// requests); the server decides who sees which author (INT_PDA_SE_Chat.ViewFor).
//
// Self-test (the TestKit selects the tab with "tab 2" first, the menu forwards the rest):
//   chat send <text>            send into the open public / faction channel
//   chat anon <text>            same, anonymously (refused when the server forbids anonymous lines)
//   chat tab <0|1|2>            0 = «Общий», 1 = «Группировка», 2 = «Личные»
//   chat pm <callsign> <text>   open the dialog and send (spaces of the callsign as '_')
//   chat refresh                «Обновить»
//   chat scroll <n>             n messages up (negative = down)
//   chat ctx <n>                context menu of the n-th message from the newest on screen (1 = newest); ctxclose
//   chat reply <n> <text>       answer the n-th message (the server quotes it)
//   chat del <n>                «Удалить диалог?» for the n-th dialog card (1 = top); chat delyes / chat delno answer
// Every command prints one line «[INT_PDA] CHAT_TEST <verb> sub=.. lines=.. shown=.. scroll=a/b ...».

class INT_PDA_PageChatRow
{
	Widget RowRoot;
	ref INT_UIF_Surface PlateBox;
	ImageWidget FlashImg;
	ImageWidget AvatarImg;
	ImageWidget BigEmblemImg;
	Widget AnonPlate;
	TextWidget NameText;
	ImageWidget EmblemImg;
	TextWidget FactionText;
	ref INT_UIF_Surface TagPill;
	TextWidget TagText;
	TextWidget RealText;
	TextWidget TimeText;
	ref array<TextWidget> RowTexts;
	ref array<TextWidget> SysTexts;
	ButtonWidget NameHit;
	ButtonWidget RowHit;				// the whole message: right click = context menu
	ImageWidget QuoteBar;				// reply: the quoted line under the header
	TextWidget QuoteText;
	string HitCallsign;					// callsign a click on the name opens ("" = none)
	int LineId;							// server id of the shown line (0 = can't be answered)
	string LineName;					// author as shown (for «Ответ для …»)
	string LineText;
	int AvatarIdx = -1;					// images loaded into the widgets (no reload while unchanged)
	int EmblemIdx = -1;
	int BigEmblemIdx = -1;

	void INT_PDA_PageChatRow()
	{
		RowTexts = new array<TextWidget>();
		SysTexts = new array<TextWidget>();
	}
}

class INT_PDA_PageChatCard
{
	Widget CardRoot;
	ref INT_UIF_Surface BoxSurf;
	ImageWidget AvatarImg;
	ImageWidget UserIcon;
	TextWidget NameText;
	TextWidget TimeText;
	TextWidget LastText;
	Widget BadgeFrame;
	TextWidget BadgeText;
	ButtonWidget CardHit;
	string DlgKey;
	string DlgName;
	int BaseFill;
	int AvatarIdx = -1;
}

class INT_PDA_PageChat : INT_PDA_Page
{
	static const int SUB_PUBLIC = 0;
	static const int SUB_FACTION = 1;
	static const int SUB_PRIVATE = 2;

	static const int KEEP_PUBLIC = 100;			// lines shown per channel (owner brief: 100 public, 50 faction / dialog)
	static const int KEEP_FACTION = 50;
	static const int KEEP_PRIVATE = 50;
	static const int ROW_POOL = 12;				// message rows (more never fit into the feed)
	static const int MAX_ROWS = 6;				// wrapped text rows of one message
	static const int MAX_SYS_ROWS = 4;
	static const int CARDS = 5;					// dialog cards on «Личные»
	static const int CARDS_SHORT = 3;			// dialog cards on «Общий» / «Группировка» (legend + «Эфир» below)

	// geometry, design px inside the page (1456 x 674), concept E chat frame
	static const float COL_X = 36;
	static const float COL_W = 340;
	static const float RIGHT_EDGE = 1420;
	static const float FEED_X = 410;
	static const float FEED_W = 1010;
	static const float FEED_Y = 88;
	static const float FEED_BOTTOM = 588;
	static const float HEAD_PM = 36;			// dialog header above the feed on «Личные»
	static const float BAR_Y = 598;
	static const float BAR_H = 64;
	static const float AVATAR_SIZE = 58;
	static const float TEXT_X = 74;
	static const float HEAD_H = 26;
	static const float ROW_H = 23;
	static const float SYS_ROW_H = 21;
	static const float ROW_GAP = 12;
	static const float SUB_SIZE = 20;
	static const float NAME_SIZE = 19;
	static const float TEXT_SIZE = 17;
	static const float META_SIZE = 14;
	static const float SYS_SIZE = 15;
	static const float CARD_Y = 30;
	static const float CARD_PITCH = 84;
	static const float CARD_H = 74;
	static const float QUOTE_H = 24;			// quoted line of a reply
	static const float REPLY_BAR_H = 34;		// «Ответ для …» above the input bar
	static const float CTX_ITEM_H = 40;			// context menu (right click on a message)

	// colours of concept E that are not in the shared palette
	static const int PLATE_GP = 0xFF1A0F0E;
	static const int PLATE_GP_LINE = 0xFF6A2B26;
	static const int TAG_GP_FILL = 0xFF2A1512;
	static const int TAG_GP_TEXT = 0xFFEC6A5C;
	static const int TAG_ANON_FILL = 0xFF222226;
	static const int ANON_TEXT = 0xFFA8A49C;
	static const int CARD_SEL_FILL = 0xFF1A130E;
	static const int CARD_SEL_LINE = 0xFF5A3418;
	static const int BAR_FILL = 0xFF101012;
	static const int SEND_HOVER = 0xFFFF9648;

	// client anti-flood survives reopening the PDA (GetTime restarts with a new mission: see Since)
	protected static int s_LastSendMs = -1000000;
	protected static int s_LastTextMs = -1000000;
	protected static string s_LastText = "";
	protected static int s_LastRefreshMs = -1000000;
	protected static int s_LastHashHintMs = -1000000;

	protected Widget m_Main;
	protected Widget m_LockedPlate;
	protected TextWidget m_Probe;
	protected ref map<string, float> m_WidthCache = new map<string, float>();
	protected float m_CacheFactor;
	protected ref array<string> m_Words = new array<string>();
	protected ref array<string> m_Wrap = new array<string>();
	protected bool m_Placed;
	protected bool m_Dirty;
	protected bool m_Marking;
	protected bool m_AskedPublic;
	protected bool m_AskedFaction;

	// sub-tabs
	protected int m_Sub;
	protected ref array<TextWidget> m_SubText = new array<TextWidget>();
	protected ref array<ButtonWidget> m_SubHit = new array<ButtonWidget>();
	protected ref array<Widget> m_SubBadge = new array<Widget>();
	protected ref array<TextWidget> m_SubBadgeText = new array<TextWidget>();
	protected ref array<float> m_SubX = new array<float>();
	protected ref array<float> m_SubW = new array<float>();
	protected ImageWidget m_SubLock;
	protected ImageWidget m_SubLine;
	protected TextWidget m_RefreshText;
	protected ImageWidget m_RefreshIcon;
	protected ButtonWidget m_RefreshHit;

	// left column
	protected Widget m_Left;
	protected ButtonWidget m_ListHit;
	protected ref array<ref INT_PDA_PageChatCard> m_Cards = new array<ref INT_PDA_PageChatCard>();
	protected TextWidget m_NoDialogs;
	protected int m_CardCap = 3;
	protected int m_DlgFirst;
	protected ref array<string> m_DlgKeys = new array<string>();
	protected ref array<int> m_DlgOrder = new array<int>();
	protected Widget m_Legend;
	protected Widget m_Air;
	protected TextWidget m_AirNet;
	protected TextWidget m_AirChannel;
	protected TextWidget m_AirCount;
	protected ref INT_UIF_Surface m_NewBox;
	protected ButtonWidget m_NewHit;
	protected string m_OpenKey;					// open private dialog: CallsignKey of the other side
	protected string m_OpenName;				// its callsign as shown
	protected string m_PendingName;				// «Новый диалог» without lines yet

	// feed
	protected Widget m_Right;
	protected Widget m_DialogHead;
	protected TextWidget m_DialogName;
	protected ButtonWidget m_FeedHit;
	protected Widget m_Feed;
	protected ref array<ref INT_PDA_PageChatRow> m_Rows = new array<ref INT_PDA_PageChatRow>();
	protected TextWidget m_Empty;
	protected ImageWidget m_Track;
	protected ImageWidget m_Thumb;
	protected Widget m_ToNew;
	protected TextWidget m_ToNewText;
	protected ButtonWidget m_ToNewHit;
	protected float m_FeedTop = 88;
	protected float m_FeedH = 500;
	protected int m_Scroll;						// whole messages scrolled up from the newest
	protected int m_MaxScroll;
	protected int m_NewBelow;					// live lines that arrived below a reader scrolled up
	protected int m_VisCount;
	protected int m_Total;
	protected ref array<int> m_VisIdx = new array<int>();
	protected ref array<float> m_VisH = new array<float>();
	protected ref INT_PDA_ChatLine m_LastSeen;	// newest line of the shown channel at the last render

	// input bar
	protected Widget m_Bar;
	protected INT_UIF_TextInput m_InputNarrow;	// next to the toggles
	protected INT_UIF_TextInput m_InputWide;	// no toggles («Личные», or nothing allowed)
	protected bool m_Wide;
	protected TextWidget m_Counter;
	protected ImageWidget m_BarSep;
	protected INT_UIF_Toggle m_Anon;
	protected INT_UIF_Toggle m_AsGp;
	protected bool m_AnonAvail;
	protected bool m_GpAvail;
	protected ref INT_UIF_Surface m_SendBox;
	protected ButtonWidget m_SendHit;
	protected string m_LastRefusal;
	protected int m_Sent;

	// reply (owner 2026-10-23: right click on a message → «Ответить на сообщение» / «Написать в ЛС»)
	protected int m_ReplyId;					// server id of the answered line (0 = no reply)
	protected int m_ReplySub;
	protected string m_ReplyName;
	protected string m_ReplySnippet;
	protected Widget m_ReplyBar;
	protected TextWidget m_ReplyText;
	protected ButtonWidget m_ReplyClose;
	protected Widget m_Ctx;
	protected ref INT_UIF_Surface m_CtxBox;
	protected ref INT_UIF_Surface m_CtxShadow;
	protected ImageWidget m_CtxStripe;
	protected ref array<ImageWidget> m_CtxHover = new array<ImageWidget>();
	protected ref array<ImageWidget> m_CtxIcon = new array<ImageWidget>();
	protected ref array<TextWidget> m_CtxText = new array<TextWidget>();
	protected ref array<ButtonWidget> m_CtxHit = new array<ButtonWidget>();
	protected ref array<int> m_CtxAction = new array<int>();	// per shown item: CTX_REPLY / CTX_PM
	protected float m_CtxW = 280;
	protected bool m_CtxHeld;			// a mouse button was down on the last poll (only a NEW press outside closes the menu)
	// snapshot of the clicked message (rows re-render under an open menu when a new line arrives)
	protected int m_CtxLineId;
	protected string m_CtxName;
	protected string m_CtxText2;
	protected string m_CtxCallsign;
	static const int CTX_REPLY = 0;
	static const int CTX_PM = 1;

	// «Удалить диалог?» (right click on a dialog card)
	protected Widget m_Del;
	protected Widget m_DelBox;
	protected TextWidget m_DelName;
	protected INT_UIF_Button m_DelYes;
	protected INT_UIF_Button m_DelNo;
	protected string m_DelKey;

	// --- public ---------------------------------------------------------------------------------------------------
	// Other pages: open the chat tab on a channel (INT_PDA_Const.CH_PUBLIC / CH_FACTION / CH_PRIVATE).
	void OpenChannel(int channel)
	{
		int sub = SUB_PUBLIC;
		if (channel == INT_PDA_Const.CH_FACTION)
			sub = SUB_FACTION;
		else if (channel == INT_PDA_Const.CH_PRIVATE)
			sub = SUB_PRIVATE;
		BringForward(sub);
	}

	// Other pages: open (or start) the private dialog with a callsign.
	void OpenPrivate(string callsign)
	{
		string cs = callsign + "";
		cs.TrimInPlace();
		if (cs == "")
		{
			BringForward(SUB_PRIVATE);
			return;
		}
		string key = INT_PDA_Util.CallsignKey(cs);
		array<ref INT_PDA_ChatLine> lst;
		bool known = false;
		if (INT_PDA_Client.s_Private.Find(key, lst) && lst && lst.Count() > 0)
			known = true;
		m_OpenKey = key;
		if (known)
		{
			m_PendingName = "";
			m_OpenName = DialogName(key, lst);
		}
		else
		{
			m_PendingName = cs;
			m_OpenName = cs;
		}
		m_DlgFirst = 0;
		m_Scroll = 0;
		m_NewBelow = 0;
		m_LastSeen = null;
		BringForward(SUB_PRIVATE);
	}

	int ActiveSub()
	{
		return m_Sub;
	}

	string OpenDialog()
	{
		return m_OpenName;
	}

	protected void BringForward(int sub)
	{
		int target = sub;
		if (target == SUB_FACTION && !FactionOk())
		{
			RefuseFaction();
			target = SUB_PUBLIC;
		}
		if (target != m_Sub)
			ResetSub(target);
		if (m_Menu && m_Menu.ActiveTab() != INT_PDA_Menu.TAB_CHAT)
		{
			m_Menu.SelectTab(INT_PDA_Menu.TAB_CHAT);
			return;
		}
		RefreshAll();
	}

	// --- build ----------------------------------------------------------------------------------------------------
	override void OnBuild()
	{
		// transparent probe for text metrics (wraps are measured, not guessed)
		m_Probe = INT_UIF_Draw.Text(m_Root, 0, 0, 3000, 40, "", TEXT_SIZE, 0x00000000);
		m_Main = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, m_H, "INT_PDA_ChatMain");
		BuildSubTabs();
		BuildLeft();
		BuildRight();
		BuildBar();
		BuildContext();
		m_LockedPlate = Locked(m_Root, m_W, m_H, "chat", "#STR_INT_PDA_CHAT_LOCKED", "#STR_INT_PDA_CHAT_LOCKED_TEXT");
		if (m_LockedPlate)
			m_LockedPlate.Show(false);
		BuildDeleteAsk();
	}

	// Over the whole page (the veil takes the clicks behind it), like the confirmations of the faction tab.
	protected void BuildDeleteAsk()
	{
		float cw = 560;
		float ch = 256;
		m_Del = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, m_H, "INT_PDA_ChatDelAsk");
		INT_UIF_Draw.Image(m_Del, 0, 0, m_W, m_H, "iuf_white", ARGB(170, 0, 0, 0));
		Hit(m_Del, 0, 0, m_W, m_H);
		m_DelBox = INT_UIF_Draw.Frame(m_Del, (m_W - cw) * 0.5, (m_H - ch) * 0.5, cw, ch, "INT_PDA_ChatDelBox");
		INT_UIF_Surface.Shadow(m_DelBox, -24, -16, cw + 48, ch + 44, ARGB(160, 0, 0, 0));
		INT_UIF_Surface.Box(m_DelBox, 0, 0, cw, ch, 8, INT_PDA_Look.PANEL, INT_PDA_Look.LINE2);
		INT_UIF_Draw.Line(m_DelBox, 0, 0, cw, 3, INT_PDA_Look.ENEMY);
		string title = INT_PDA_Look.Caps("#STR_INT_PDA_CHAT_DEL_Q");
		INT_UIF_Draw.Text(m_DelBox, 28, 22, cw - 56, 28, title, 20, INT_PDA_Look.TEXT);
		m_DelName = INT_UIF_Draw.Text(m_DelBox, 28, 58, cw - 56, 36, "", 28, INT_PDA_Look.ORANGE);
		INT_PDA_Look.MText(m_DelBox, 28, 102, cw - 56, 70, "#STR_INT_PDA_CHAT_DEL_TEXT", 15, INT_PDA_Look.DIM);
		m_DelNo = INT_UIF_Button.Create(m_Bag, m_DelBox, cw - 28 - 300, ch - 64, 140, 44, "#STR_INT_PDA_NO", INT_UIF_Variant.SECONDARY);
		m_DelYes = INT_UIF_Button.Create(m_Bag, m_DelBox, cw - 28 - 150, ch - 64, 150, 44, "#STR_INT_PDA_YES", INT_UIF_Variant.DANGER, "trash");
		if (m_DelNo)
			m_DelNo.OnClick().Insert(OnDelNo);
		if (m_DelYes)
			m_DelYes.OnClick().Insert(OnDelYes);
		m_Del.Show(false);
	}

	protected void BuildSubTabs()
	{
		float x = COL_X;
		int i;
		string sk;
		string cap;
		float tw;
		TextWidget t;
		Widget badge;
		TextWidget bt;
		ButtonWidget hit;
		for (i = 0; i < 3; i++)
		{
			sk = SubKey(i);
			cap = INT_PDA_Look.Caps(sk);
			tw = INT_PDA_Look.TextW(cap, SUB_SIZE) + 4;
			t = INT_UIF_Draw.Text(m_Main, x, 16, tw + 40, 52, cap, SUB_SIZE, INT_PDA_Look.DIM);
			badge = INT_UIF_Draw.Frame(m_Main, x + tw + 10, 31, 30, 22, "INT_PDA_ChatBadge");
			INT_UIF_Surface.Pill(badge, 0, 0, 30, 22, INT_PDA_Look.ORANGE);
			bt = INT_UIF_Draw.Text(badge, 0, 0, 30, 22, "", 14, INT_PDA_Look.ON_ORANGE, INT_UIF_Align.CENTER);
			badge.Show(false);
			hit = Hit(m_Main, x - 8, 16, tw + 56, 52);
			m_SubText.Insert(t);
			m_SubBadge.Insert(badge);
			m_SubBadgeText.Insert(bt);
			m_SubHit.Insert(hit);
			m_SubX.Insert(x);
			m_SubW.Insert(tw);
			x = x + tw + 80;
		}
		m_SubLock = INT_PDA_Look.DrawIcon(m_Main, 0, 34, 16, "lock", INT_PDA_Look.DIM2);
		m_SubLock.Show(false);
		m_SubLine = INT_UIF_Draw.Line(m_Main, COL_X, 65, 10, 3, INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Line(m_Main, COL_X, 68, RIGHT_EDGE - COL_X, 1, INT_PDA_Look.LINE);
		string rcap = INT_PDA_Look.Caps("#STR_INT_PDA_CHAT_REFRESH");
		float rw = INT_PDA_Look.TextW(rcap, 18);
		m_RefreshText = INT_UIF_Draw.Text(m_Main, RIGHT_EDGE - 300, 16, 300, 52, rcap, 18, INT_PDA_Look.DIM, INT_UIF_Align.RIGHT);
		m_RefreshIcon = INT_PDA_Look.DrawIcon(m_Main, RIGHT_EDGE - rw - 30, 31, 22, "refresh", INT_PDA_Look.DIM);
		m_RefreshHit = Hit(m_Main, RIGHT_EDGE - rw - 38, 16, rw + 42, 52);
	}

	protected void BuildLeft()
	{
		m_Left = INT_UIF_Draw.Frame(m_Main, COL_X, FEED_Y, COL_W, 574, "INT_PDA_ChatLeft");
		Label(m_Left, 0, 0, COL_W, "#STR_INT_PDA_CHAT_DIALOGS");
		// wheel catcher under the cards
		m_ListHit = Hit(m_Left, 0, CARD_Y, COL_W, CARDS_SHORT * CARD_PITCH);
		int i;
		INT_PDA_PageChatCard card;
		for (i = 0; i < CARDS; i++)
		{
			card = BuildCard(CARD_Y + i * CARD_PITCH);
			m_Cards.Insert(card);
		}
		m_NoDialogs = INT_UIF_Draw.Text(m_Left, 0, CARD_Y + 8, COL_W, 24, "#STR_INT_PDA_CHAT_NO_DIALOGS", 15, INT_PDA_Look.DIM2);
		// nick colours (the words are placed with measured widths on the first show)
		m_Legend = INT_UIF_Draw.Frame(m_Left, 0, 290, COL_W, 70, "INT_PDA_ChatLegend");
		INT_UIF_Draw.Line(m_Legend, 0, 0, COL_W, 1, INT_PDA_Look.LINE);
		Label(m_Legend, 0, 14, COL_W, "#STR_INT_PDA_CHAT_LEGEND");
		// «Эфир»
		m_Air = Panel(m_Left, 0, 382, COL_W, 120, "#STR_INT_PDA_CHAT_AIR");
		INT_UIF_Draw.Text(m_Air, 18, 40, 150, 22, "#STR_INT_PDA_CHAT_AIR_NET", 15, INT_PDA_Look.DIM);
		m_AirNet = INT_UIF_Draw.Text(m_Air, 150, 40, COL_W - 168, 22, "", 15, INT_PDA_Look.TEXT, INT_UIF_Align.RIGHT);
		INT_UIF_Draw.Text(m_Air, 18, 64, 120, 22, "#STR_INT_PDA_CHAT_AIR_CHANNEL", 15, INT_PDA_Look.DIM);
		m_AirChannel = INT_UIF_Draw.Text(m_Air, 100, 64, COL_W - 118, 22, "", 15, INT_PDA_Look.ORANGE, INT_UIF_Align.RIGHT);
		INT_UIF_Draw.Text(m_Air, 18, 88, 150, 22, "#STR_INT_PDA_CHAT_AIR_COUNT", 15, INT_PDA_Look.DIM);
		m_AirCount = INT_UIF_Draw.Text(m_Air, 150, 88, COL_W - 168, 22, "", 15, INT_PDA_Look.TEXT, INT_UIF_Align.RIGHT);
		// «Новый диалог»
		Widget nb = INT_UIF_Draw.Frame(m_Left, 0, 518, COL_W, 48, "INT_PDA_ChatNew");
		m_NewBox = INT_UIF_Surface.Box(nb, 0, 0, COL_W, 48, 4, INT_PDA_Look.PANEL, INT_PDA_Look.LINE2);
		INT_PDA_Look.DrawIcon(nb, 18, 14, 20, "plus", INT_PDA_Look.ORANGE);
		string ncap = INT_PDA_Look.Caps("#STR_INT_PDA_CHAT_NEW_DIALOG");
		INT_UIF_Draw.Text(nb, 50, 0, COL_W - 60, 48, ncap, 15, INT_PDA_Look.TEXT);
		m_NewHit = Hit(m_Left, 0, 518, COL_W, 48);
	}

	protected INT_PDA_PageChatCard BuildCard(float y)
	{
		INT_PDA_PageChatCard c = new INT_PDA_PageChatCard();
		c.CardRoot = INT_UIF_Draw.Frame(m_Left, 0, y, COL_W, CARD_H, "INT_PDA_ChatCard");
		c.BoxSurf = INT_UIF_Surface.Box(c.CardRoot, 0, 0, COL_W, CARD_H, 6, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
		INT_UIF_Draw.Image(c.CardRoot, 12, 12, 50, 50, "iuf_white", INT_PDA_Look.PANEL2);
		c.UserIcon = INT_PDA_Look.DrawIcon(c.CardRoot, 25, 25, 24, "user", INT_PDA_Look.DIM2);
		c.AvatarImg = INT_PDA_Look.DrawAvatar(c.CardRoot, 12, 12, 50, 0);
		c.NameText = INT_UIF_Draw.Text(c.CardRoot, 74, 10, 180, 26, "", 18, INT_PDA_Look.TEXT);
		c.TimeText = INT_UIF_Draw.Text(c.CardRoot, COL_W - 96, 12, 84, 18, "", 13, INT_PDA_Look.DIM, INT_UIF_Align.RIGHT);
		c.LastText = INT_UIF_Draw.Text(c.CardRoot, 74, 40, COL_W - 126, 22, "", 14, INT_PDA_Look.DIM);
		c.BadgeFrame = INT_UIF_Draw.Frame(c.CardRoot, COL_W - 42, 38, 30, 22, "INT_PDA_ChatCardBadge");
		INT_UIF_Surface.Pill(c.BadgeFrame, 0, 0, 30, 22, INT_PDA_Look.ORANGE);
		c.BadgeText = INT_UIF_Draw.Text(c.BadgeFrame, 0, 0, 30, 22, "", 14, INT_PDA_Look.ON_ORANGE, INT_UIF_Align.CENTER);
		c.BadgeFrame.Show(false);
		c.CardHit = Hit(m_Left, 0, y, COL_W, CARD_H);
		c.BaseFill = INT_PDA_Look.PANEL;
		c.CardRoot.Show(false);
		c.CardHit.Show(false);
		return c;
	}

	protected void BuildRight()
	{
		m_Right = INT_UIF_Draw.Frame(m_Main, FEED_X, 0, FEED_W, m_H, "INT_PDA_ChatRight");
		m_DialogHead = INT_UIF_Draw.Frame(m_Right, 0, FEED_Y - 4, FEED_W, HEAD_PM, "INT_PDA_ChatDialogHead");
		INT_PDA_Look.DrawIcon(m_DialogHead, 0, 7, 18, "mail", INT_PDA_Look.ORANGE);
		m_DialogName = INT_UIF_Draw.Text(m_DialogHead, 28, 0, 700, 30, "", 20, INT_PDA_Look.TEXT);
		INT_UIF_Draw.Line(m_DialogHead, 0, HEAD_PM - 3, FEED_W, 1, INT_PDA_Look.LINE);
		m_DialogHead.Show(false);
		// wheel catcher under the rows (the rows' callsign hits sit above it)
		m_FeedHit = Hit(m_Right, 0, FEED_Y, FEED_W, FEED_BOTTOM - FEED_Y);
		m_Feed = INT_UIF_Draw.Frame(m_Right, 0, FEED_Y, FEED_W, FEED_BOTTOM - FEED_Y, "INT_PDA_ChatFeed");
		int i;
		INT_PDA_PageChatRow r;
		for (i = 0; i < ROW_POOL; i++)
		{
			r = BuildRow();
			m_Rows.Insert(r);
		}
		m_Empty = INT_UIF_Draw.Text(m_Right, 0, 300, FEED_W, 30, "", 17, INT_PDA_Look.DIM2, INT_UIF_Align.CENTER);
		m_Track = INT_UIF_Draw.Line(m_Right, FEED_W + 16, FEED_Y, 2, FEED_BOTTOM - FEED_Y, INT_PDA_Look.LINE);
		m_Thumb = INT_UIF_Draw.Line(m_Right, FEED_W + 16, FEED_Y, 2, 40, INT_PDA_Look.ORANGE);
		m_Track.Show(false);
		m_Thumb.Show(false);
		// «к новым»
		m_ToNew = INT_UIF_Draw.Frame(m_Right, FEED_W - 210, FEED_BOTTOM - 46, 200, 36, "INT_PDA_ChatToNew");
		INT_UIF_Surface.Box(m_ToNew, 0, 0, 200, 36, 6, INT_PDA_Look.PANEL2, INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Icon(m_ToNew, 14, 10, 16, "chevron_down", INT_PDA_Look.ORANGE);
		m_ToNewText = INT_UIF_Draw.Text(m_ToNew, 38, 0, 156, 36, "", 14, INT_PDA_Look.TEXT);
		m_ToNewHit = Hit(m_Right, FEED_W - 210, FEED_BOTTOM - 46, 200, 36);
		m_ToNew.Show(false);
		m_ToNewHit.Show(false);
		// «Ответ для …» above the input bar (the feed gets shorter while it is up)
		m_ReplyBar = INT_UIF_Draw.Frame(m_Right, 0, BAR_Y - REPLY_BAR_H - 6, FEED_W, REPLY_BAR_H, "INT_PDA_ChatReplyBar");
		INT_UIF_Surface.Box(m_ReplyBar, 0, 0, FEED_W, REPLY_BAR_H, 4, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
		INT_UIF_Draw.Line(m_ReplyBar, 0, 0, 3, REPLY_BAR_H, INT_PDA_Look.ORANGE);
		INT_PDA_Look.DrawIcon(m_ReplyBar, 16, 8, 18, "chat", INT_PDA_Look.ORANGE);
		m_ReplyText = INT_UIF_Draw.Text(m_ReplyBar, 46, 0, FEED_W - 110, REPLY_BAR_H, "", 15, INT_PDA_Look.TEXT);
		INT_PDA_Look.DrawIcon(m_ReplyBar, FEED_W - 32, 9, 16, "close", INT_PDA_Look.DIM);
		m_ReplyClose = Hit(m_ReplyBar, FEED_W - 46, 0, 46, REPLY_BAR_H);
		m_ReplyBar.Show(false);
	}

	// Context menu of a message (right click): two items at most, built once, filled per click.
	protected void BuildContext()
	{
		float h = 12 + 2 * CTX_ITEM_H;
		m_Ctx = INT_UIF_Draw.Frame(m_Main, 0, 0, m_CtxW, h, "INT_PDA_ChatCtx");
		m_CtxShadow = INT_UIF_Surface.Shadow(m_Ctx, -16, -10, m_CtxW + 32, h + 28, ARGB(150, 0, 0, 0));
		m_CtxBox = INT_UIF_Surface.Box(m_Ctx, 0, 0, m_CtxW, h, 6, 0xFA131315, INT_PDA_Look.LINE2);
		m_CtxStripe = INT_UIF_Draw.Line(m_Ctx, 0, 0, 3, h, INT_PDA_Look.ORANGE);
		float iy;
		ImageWidget hov;
		for (int i = 0; i < 2; i++)
		{
			iy = 6 + i * CTX_ITEM_H;
			hov = INT_UIF_Draw.Texture(m_Ctx, 6, iy, m_CtxW - 12, CTX_ITEM_H, INT_UIF_WHITE, INT_PDA_Look.PANEL2);
			hov.SetAlpha(0);
			m_CtxHover.Insert(hov);
			m_CtxIcon.Insert(INT_PDA_Look.DrawIcon(m_Ctx, 18, iy + 11, 18, "chat", INT_PDA_Look.ORANGE));
			m_CtxText.Insert(INT_UIF_Draw.Text(m_Ctx, 48, iy, m_CtxW - 60, CTX_ITEM_H, "", 15, INT_PDA_Look.TEXT));
			m_CtxHit.Insert(Hit(m_Ctx, 6, iy, m_CtxW - 12, CTX_ITEM_H));
		}
		m_Ctx.Show(false);
	}

	protected INT_PDA_PageChatRow BuildRow()
	{
		INT_PDA_PageChatRow r = new INT_PDA_PageChatRow();
		r.RowRoot = INT_UIF_Draw.Frame(m_Feed, 0, 0, FEED_W, 60, "INT_PDA_ChatRow");
		r.PlateBox = INT_UIF_Surface.Box(r.RowRoot, -12, 0, FEED_W + 24, 60, 6, 0, 0);
		r.FlashImg = INT_UIF_Draw.Image(r.RowRoot, -12, 0, FEED_W * 0.7, 60, "iuf_grad_h", INT_PDA_Look.ORANGE);
		if (r.FlashImg)
			r.FlashImg.SetAlpha(0);
		r.AvatarImg = INT_PDA_Look.DrawAvatar(r.RowRoot, 0, 0, AVATAR_SIZE, 0);
		r.AvatarIdx = 0;
		r.BigEmblemImg = INT_PDA_Look.DrawEmblem(r.RowRoot, 0, 0, AVATAR_SIZE, 1);
		r.BigEmblemIdx = 1;
		r.AnonPlate = INT_PDA_Look.DrawAnonAvatar(r.RowRoot, 0, 0, AVATAR_SIZE);
		r.NameText = INT_UIF_Draw.Text(r.RowRoot, TEXT_X, 0, 400, HEAD_H, "", NAME_SIZE, INT_PDA_Look.TEXT);
		if (r.NameText)
			r.NameText.SetBold(true);
		r.EmblemImg = INT_PDA_Look.DrawEmblem(r.RowRoot, 0, 3, 20, 1);
		r.EmblemIdx = 1;
		r.FactionText = INT_UIF_Draw.Text(r.RowRoot, 0, 0, 300, HEAD_H, "", META_SIZE, INT_PDA_Look.DIM);
		r.TagPill = INT_UIF_Surface.Pill(r.RowRoot, 0, 3, 120, 20, TAG_GP_FILL);
		r.TagText = INT_UIF_Draw.Text(r.RowRoot, 0, 3, 120, 20, "", 12, TAG_GP_TEXT, INT_UIF_Align.CENTER);
		r.RealText = INT_UIF_Draw.Text(r.RowRoot, 0, 0, 300, HEAD_H, "", META_SIZE, INT_PDA_Look.DIM2);
		r.TimeText = INT_UIF_Draw.Text(r.RowRoot, FEED_W - 180, 0, 180, HEAD_H, "", META_SIZE, INT_PDA_Look.DIM, INT_UIF_Align.RIGHT);
		int k;
		TextWidget tw;
		for (k = 0; k < MAX_ROWS; k++)
		{
			tw = INT_UIF_Draw.Text(r.RowRoot, TEXT_X, HEAD_H + 2 + k * ROW_H, FEED_W - TEXT_X - 8, ROW_H, "", TEXT_SIZE, INT_PDA_Look.TEXT);
			r.RowTexts.Insert(tw);
		}
		for (k = 0; k < MAX_SYS_ROWS; k++)
		{
			tw = INT_UIF_Draw.Text(r.RowRoot, 120, 7 + k * SYS_ROW_H, FEED_W - 240, SYS_ROW_H, "", SYS_SIZE, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
			r.SysTexts.Insert(tw);
		}
		r.QuoteBar = INT_UIF_Draw.Line(r.RowRoot, TEXT_X, HEAD_H + 4, 2, QUOTE_H - 7, INT_PDA_Look.ORANGE);
		r.QuoteText = INT_UIF_Draw.Text(r.RowRoot, TEXT_X + 10, HEAD_H + 1, FEED_W - TEXT_X - 24, QUOTE_H, "", META_SIZE, INT_PDA_Look.DIM);
		r.QuoteBar.Show(false);
		r.QuoteText.Show(false);
		// the whole message under the callsign hit (created first = below it): right click opens the context menu
		r.RowHit = Hit(r.RowRoot, 0, 0, FEED_W, 60);
		r.RowHit.Show(false);
		r.NameHit = Hit(r.RowRoot, TEXT_X, 0, 200, HEAD_H);
		r.RowRoot.Show(false);
		return r;
	}

	protected void BuildBar()
	{
		m_Bar = INT_UIF_Draw.Frame(m_Right, 0, BAR_Y, FEED_W, BAR_H, "INT_PDA_ChatBar");
		INT_UIF_Surface.Box(m_Bar, 0, 0, FEED_W, BAR_H, 6, BAR_FILL, INT_PDA_Look.LINE);
		int mc = MaxChars();
		m_InputNarrow = INT_UIF_TextInput.Create(m_Bag, m_Bar, 10, 8, 430, 48, "#STR_INT_PDA_CHAT_PLACEHOLDER", mc);
		m_InputWide = INT_UIF_TextInput.Create(m_Bag, m_Bar, 10, 8, 790, 48, "#STR_INT_PDA_CHAT_PLACEHOLDER", mc);
		if (m_InputNarrow)
		{
			m_InputNarrow.OnChanged().Insert(OnInputChanged);
			m_InputNarrow.OnSubmit().Insert(OnInputSubmit);
		}
		if (m_InputWide)
		{
			m_InputWide.OnChanged().Insert(OnInputChanged);
			m_InputWide.OnSubmit().Insert(OnInputSubmit);
			m_InputWide.Show(false);
		}
		m_Counter = INT_UIF_Draw.Text(m_Bar, 446, 0, 84, BAR_H, "", 15, INT_PDA_Look.DIM, INT_UIF_Align.RIGHT);
		m_BarSep = INT_UIF_Draw.Line(m_Bar, 542, 14, 1, 36, INT_PDA_Look.LINE);
		m_Anon = INT_UIF_Toggle.Create(m_Bag, m_Bar, 556, 16, 146, "#STR_INT_PDA_CHAT_ANON", false);
		m_AsGp = INT_UIF_Toggle.Create(m_Bag, m_Bar, 716, 16, 178, "#STR_INT_PDA_CHAT_AS_GP", false);
		if (m_Anon)
			m_Anon.OnChanged().Insert(OnAnonToggled);
		if (m_AsGp)
			m_AsGp.OnChanged().Insert(OnGpToggled);
		Widget sb = INT_UIF_Draw.Frame(m_Bar, 908, 9, 92, 46, "INT_PDA_ChatSend");
		m_SendBox = INT_UIF_Surface.Box(sb, 0, 0, 92, 46, 4, INT_PDA_Look.ORANGE, 0);
		INT_PDA_Look.DrawIcon(sb, 33, 10, 26, "send", INT_PDA_Look.ON_ORANGE);
		m_SendHit = Hit(m_Bar, 908, 9, 92, 46);
	}

	// Sub-tab words, badges, «Обновить» and the legend need measured widths (the probe measures only while the page
	// is on screen): the tabs are re-placed on every show, the legend words are created once.
	protected void PlaceStatic()
	{
		if (!m_Placed)
			PlaceLegend();
		m_Placed = true;
		PlaceSubTabs();
		MoveUnderline(false);
	}

	protected void PlaceSubTabs()
	{
		float x = COL_X;
		int i;
		string sk;
		string cap;
		float tw;
		for (i = 0; i < m_SubText.Count(); i++)
		{
			sk = SubKey(i);
			cap = INT_PDA_Look.Caps(sk);
			tw = Width(cap, SUB_SIZE);
			INT_UIF_Draw.Place(m_SubText[i], x, 16, tw + 16, 52);
			INT_UIF_Draw.Move(m_SubBadge[i], x + tw + 10, 31);
			if (i == SUB_FACTION)
				INT_UIF_Draw.Move(m_SubLock, x + tw + 10, 34);
			INT_UIF_Draw.Place(m_SubHit[i], x - 8, 16, tw + 56, 52);
			m_SubX.Set(i, x);
			m_SubW.Set(i, tw);
			x = x + tw + 80;
		}
		string rcap = INT_PDA_Look.Caps("#STR_INT_PDA_CHAT_REFRESH");
		float rw = Width(rcap, 18);
		INT_UIF_Draw.Move(m_RefreshIcon, RIGHT_EDGE - rw - 30, 31);
		INT_UIF_Draw.Place(m_RefreshHit, RIGHT_EDGE - rw - 38, 16, rw + 42, 52);
	}

	protected void PlaceLegend()
	{
		if (!m_Legend)
			return;
		float x = 0;
		float dotW = Width("·", 15);
		int i;
		string key;
		string word;
		float ww;
		int col;
		for (i = 0; i < 4; i++)
		{
			if (i > 0)
			{
				INT_UIF_Draw.Text(m_Legend, x, 38, dotW + 6, 22, "·", 15, INT_PDA_Look.DIM2);
				x = x + dotW + 8;
			}
			key = LegendKey(i);
			word = Widget.TranslateString(key);
			ww = Width(word, 15);
			col = LegendColor(i);
			INT_UIF_Draw.Text(m_Legend, x, 38, ww + 10, 22, word, 15, col);
			x = x + ww + 8;
		}
	}

	protected string LegendKey(int i)
	{
		if (i == 0)
			return "#STR_INT_PDA_CHAT_REL_OWN";
		if (i == 1)
			return "#STR_INT_PDA_CHAT_REL_ALLY";
		if (i == 2)
			return "#STR_INT_PDA_CHAT_REL_NEUTRAL";
		return "#STR_INT_PDA_CHAT_REL_ENEMY";
	}

	protected int LegendColor(int i)
	{
		if (i == 0)
			return INT_PDA_Look.OWN;
		if (i == 1)
			return INT_PDA_Look.ALLY;
		if (i == 2)
			return INT_PDA_Look.NEUTRAL;
		return INT_PDA_Look.ENEMY;
	}

	protected string SubKey(int sub)
	{
		if (sub == SUB_FACTION)
			return "#STR_INT_PDA_CHAT_SUB_FACTION";
		if (sub == SUB_PRIVATE)
			return "#STR_INT_PDA_CHAT_SUB_PRIVATE";
		return "#STR_INT_PDA_CHAT_SUB_PUBLIC";
	}

	// --- page hooks -----------------------------------------------------------------------------------------------
	override void OnShow()
	{
		m_Dirty = false;
		PlaceStatic();
		RefreshAll();
		AskOnce(m_Sub);
		RevealAt(m_Left, COL_X, FEED_Y, 0.04);
		RevealAt(m_Right, FEED_X, 0, 0.1);
	}

	override void OnHide()
	{
		CloseContext();
		CloseDeleteAsk();
		// a hidden edit box must not keep the keyboard (TAB / Esc of the menu)
		Widget f = GetFocus();
		if (f && INT_UIF_Widgets.IsDescendant(f, m_Root))
			SetFocus(null);
	}

	// The menu forwards every client change to every page: only mark, the visible page rebuilds once per frame.
	override void OnChanged(int what)
	{
		if (m_Marking)
			return;
		if (what == INT_PDA_Client.CH_CHAT || what == INT_PDA_Client.CH_PDA || what == INT_PDA_Client.CH_PROFILE)
			m_Dirty = true;
		else if (what == INT_PDA_Client.CH_FACTION || what == INT_PDA_Client.CH_AUTH || what == INT_PDA_Client.CH_WELCOME)
			m_Dirty = true;
	}

	override void OnResult(int requestOp, int code)
	{
		if (requestOp != INT_PDA_Const.OP_CHAT_SEND && requestOp != INT_PDA_Const.OP_PM_SEND)
			return;
		if (code == INT_PDA_Const.R_OK)
			return;
		INT_PDA_Sfx.Play("Error");
		Toast(code);
		m_LastRefusal = INT_PDA_Look.ResultKey(code);
		// give the refused text back so it can be sent again
		INT_UIF_TextInput input = ActiveInput();
		if (input && s_LastText != "")
		{
			string cur = input.GetText();
			if (cur == "")
			{
				input.SetText(s_LastText);
				s_LastTextMs = -1000000;
				UpdateCounter();
			}
		}
		if (requestOp == INT_PDA_Const.OP_PM_SEND && code == INT_PDA_Const.R_NOT_FOUND)
			DropUnknownDialog();
		m_Dirty = true;
	}

	override void Update(float dt)
	{
		PollContext();
		if (!m_Dirty)
			return;
		m_Dirty = false;
		RefreshAll();
	}

	protected void RevealAt(Widget w, float x, float y, float delay)
	{
		if (!w)
			return;
		// back to the base position first: a reveal still running from a fast tab switch must not shift the column
		INT_UIF_Draw.Move(w, x, y);
		Reveal(w, delay);
	}

	// --- access ---------------------------------------------------------------------------------------------------
	protected bool FactionOk()
	{
		if (!HasMod(INT_PDA.SLOT_GP))
			return false;
		return InFaction();
	}

	protected void RefuseFaction()
	{
		if (!HasMod(INT_PDA.SLOT_GP))
			Refuse("#STR_INT_PDA_CHAT_NEED_GP");
		else
			Refuse("#STR_INT_PDA_CHAT_NEED_FACTION");
	}

	protected void Refuse(string key)
	{
		m_LastRefusal = key;
		INT_PDA_Sfx.Play("Error");
		INT_UIF_Toast.Error(key);
		INT_PDA_Log.Dbg("чат: отказ " + key);
	}

	protected int MaxChars()
	{
		return INT_PDA_Util.ClampI(INT_PDA_Client.s_Settings.ChatMaxChars, 1, 450);
	}

	// ms since a stamp; a stamp from an earlier mission (GetTime restarted) counts as long ago
	protected static int Since(int stampMs, int now)
	{
		if (stampMs > now)
			return 1000000000;
		return now - stampMs;
	}

	// --- sub-tabs ---------------------------------------------------------------------------------------------------
	void SelectSub(int sub, bool sound = true)
	{
		int target = INT_PDA_Util.ClampI(sub, SUB_PUBLIC, SUB_PRIVATE);
		if (target == SUB_FACTION && !FactionOk())
		{
			RefuseFaction();
			return;
		}
		if (sound)
			INT_PDA_Sfx.Play("Tab");
		if (target != m_Sub)
			ResetSub(target);
		AskOnce(target);
		RefreshAll();
	}

	protected void ResetSub(int target)
	{
		m_Sub = target;
		m_Scroll = 0;
		m_NewBelow = 0;
		m_LastSeen = null;
		MoveUnderline(m_Placed);
	}

	protected void MoveUnderline(bool animate)
	{
		if (!m_SubLine || m_Sub < 0 || m_Sub >= m_SubX.Count())
			return;
		float lx = m_SubX[m_Sub];
		float lw = m_SubW[m_Sub];
		if (!animate)
		{
			INT_UIF_Draw.Place(m_SubLine, lx, 65, lw, 3);
			return;
		}
		INT_UIF_Animator.MoveX(m_SubLine, INT_UIF_Draw.S(lx), 0.24, INT_UIF_Ease.OUT_CUBIC);
		INT_UIF_Animator.ResizeW(m_SubLine, INT_UIF_Draw.S(lw), 0.24, INT_UIF_Ease.OUT_CUBIC);
	}

	// Opening a channel with nothing cached asks the server once per PDA opening (the login already asks for the
	// public history, faction data asks for the faction one); «Обновить» asks again on demand.
	protected void AskOnce(int sub)
	{
		if (!m_Active || !INT_PDA_Client.s_LoggedIn)
			return;
		int now = GetGame().GetTime();
		if (sub == SUB_PUBLIC && !m_AskedPublic && INT_PDA_Client.s_Public.Count() == 0)
		{
			m_AskedPublic = true;
			s_LastRefreshMs = now;
			INT_PDA_Client.RequestHistory(INT_PDA_Const.CH_PUBLIC);
		}
		else if (sub == SUB_FACTION && !m_AskedFaction && INT_PDA_Client.s_FactionChat.Count() == 0)
		{
			if (!FactionOk())
				return;
			m_AskedFaction = true;
			s_LastRefreshMs = now;
			INT_PDA_Client.RequestHistory(INT_PDA_Const.CH_FACTION);
		}
	}

	protected void DoRefresh()
	{
		int now = GetGame().GetTime();
		if (Since(s_LastRefreshMs, now) < INT_PDA_Const.REQUEST_GAP_MS * 4)
		{
			INT_PDA_Sfx.Play("Error");
			m_LastRefusal = "refresh-gap";
			return;
		}
		s_LastRefreshMs = now;
		INT_PDA_Sfx.Play("Click");
		int channel = INT_PDA_Const.CH_PUBLIC;
		if (m_Sub == SUB_FACTION)
			channel = INT_PDA_Const.CH_FACTION;
		else if (m_Sub == SUB_PRIVATE)
			channel = INT_PDA_Const.CH_PRIVATE;
		INT_PDA_Client.RequestHistory(channel);
		m_LastRefusal = "";
		m_Scroll = 0;
		m_NewBelow = 0;
		if (m_RefreshIcon)
		{
			m_RefreshIcon.SetColor(INT_PDA_Look.ORANGE);
			INT_UIF_Animator.ColorTo(m_RefreshIcon, INT_PDA_Look.DIM, 0.8);
		}
	}

	// --- full refresh -------------------------------------------------------------------------------------------
	protected void RefreshAll()
	{
		if (!m_Active)
		{
			m_Dirty = true;
			return;
		}
		bool ok = HasMod(INT_PDA.SLOT_CHAT);
		if (m_LockedPlate)
			m_LockedPlate.Show(!ok);
		m_Main.Show(ok);
		if (!ok)
			return;
		if (!m_Placed)
			PlaceStatic();
		if (m_Sub == SUB_FACTION && !FactionOk())
			ResetSub(SUB_PUBLIC);
		BuildDialogList();
		if (m_Sub == SUB_PRIVATE)
			PickDialog();
		ApplyMode();
		RenderFeed();
		RenderCards();
		PaintSubTabs();
		UpdateAir();
		UpdateCounter();
	}

	// «Личные»: keep the open dialog's callsign as the server spells it; open the newest one when none is open.
	protected void PickDialog()
	{
		array<ref INT_PDA_ChatLine> lst;
		if (m_OpenKey == "" && m_DlgKeys.Count() > 0)
		{
			m_OpenKey = m_DlgKeys[0];
			m_OpenName = "";
			m_LastSeen = null;
			m_Scroll = 0;
		}
		if (m_OpenKey == "")
			return;
		if (INT_PDA_Client.s_Private.Find(m_OpenKey, lst) && lst && lst.Count() > 0)
			m_OpenName = DialogName(m_OpenKey, lst);
		else if (m_OpenName == "")
			m_OpenName = m_OpenKey;
	}

	// The server does not know the callsign of a new dialog: forget the empty dialog.
	protected void DropUnknownDialog()
	{
		m_PendingName = "";
		array<ref INT_PDA_ChatLine> lst;
		if (INT_PDA_Client.s_Private.Find(m_OpenKey, lst) && lst && lst.Count() > 0)
			return;
		m_OpenKey = "";
		m_OpenName = "";
		m_LastSeen = null;
	}

	protected void ApplyMode()
	{
		bool pm = false;
		if (m_Sub == SUB_PRIVATE)
			pm = true;
		float top = FEED_Y;
		if (pm)
			top = FEED_Y + HEAD_PM;
		// a reply belongs to the channel it was started in (private dialogs have no server ids)
		if (m_ReplyId > 0 && (pm || m_ReplySub != m_Sub))
			ClearReply(false);
		bool replying = m_ReplyId > 0;
		if (m_ReplyBar)
			m_ReplyBar.Show(replying);
		m_FeedTop = top;
		m_FeedH = FEED_BOTTOM - top;
		if (replying)
			m_FeedH = m_FeedH - REPLY_BAR_H - 10;
		INT_UIF_Draw.Place(m_Feed, 0, top, FEED_W, m_FeedH);
		INT_UIF_Draw.Place(m_FeedHit, 0, top, FEED_W, m_FeedH);
		INT_UIF_Draw.Place(m_Track, FEED_W + 16, top, 2, m_FeedH);
		INT_UIF_Draw.Place(m_Empty, 0, top + m_FeedH * 0.5 - 15, FEED_W, 30);
		INT_UIF_Draw.Place(m_ToNew, FEED_W - 210, top + m_FeedH - 46, 200, 36);
		INT_UIF_Draw.Place(m_ToNewHit, FEED_W - 210, top + m_FeedH - 46, 200, 36);
		m_DialogHead.Show(pm);
		if (pm)
		{
			if (m_OpenName != "")
			{
				m_DialogName.SetText(m_OpenName);
				m_DialogName.SetColor(INT_PDA_Look.TEXT);
			}
			else
			{
				m_DialogName.SetText("#STR_INT_PDA_CHAT_NO_OPEN");
				m_DialogName.SetColor(INT_PDA_Look.DIM2);
			}
		}
		m_Legend.Show(!pm);
		m_Air.Show(!pm);
		int cap = CARDS_SHORT;
		if (pm)
			cap = CARDS;
		m_CardCap = cap;
		INT_UIF_Draw.Place(m_ListHit, 0, CARD_Y, COL_W, cap * CARD_PITCH);
		// toggles: «Анонимно» only when the server allows it, «От имени ГП» only with the GP module + membership
		bool anonOk = false;
		if (!pm && INT_PDA_Client.s_Settings.AnonymousAllowed)
			anonOk = true;
		bool gpOk = false;
		if (!pm && FactionOk())
			gpOk = true;
		m_AnonAvail = anonOk;
		m_GpAvail = gpOk;
		if (m_Anon)
		{
			if (!anonOk && m_Anon.IsOn())
				m_Anon.SetOn(false, false, false);
			m_Anon.Show(anonOk);
		}
		if (m_AsGp)
		{
			if (!gpOk && m_AsGp.IsOn())
				m_AsGp.SetOn(false, false, false);
			m_AsGp.Show(gpOk);
		}
		bool wide = true;
		if (anonOk || gpOk)
			wide = false;
		if (m_BarSep)
			m_BarSep.Show(!wide);
		// one draft: the text moves into the field that is shown now
		if (wide != m_Wide)
		{
			INT_UIF_TextInput oldIn = ActiveInput();
			m_Wide = wide;
			INT_UIF_TextInput newIn = ActiveInput();
			if (oldIn && newIn && oldIn != newIn)
			{
				string draft = oldIn.GetText();
				newIn.SetText(draft);
				oldIn.SetText("");
			}
		}
		if (m_InputNarrow)
			m_InputNarrow.Show(!m_Wide);
		if (m_InputWide)
			m_InputWide.Show(m_Wide);
		if (m_Wide)
			INT_UIF_Draw.Place(m_Counter, 808, 0, 84, BAR_H);
		else
			INT_UIF_Draw.Place(m_Counter, 446, 0, 84, BAR_H);
	}

	protected INT_UIF_TextInput ActiveInput()
	{
		if (m_Wide)
			return m_InputWide;
		return m_InputNarrow;
	}

	protected int KeepFor(int sub)
	{
		if (sub == SUB_FACTION)
			return KEEP_FACTION;
		if (sub == SUB_PRIVATE)
			return KEEP_PRIVATE;
		return KEEP_PUBLIC;
	}

	protected array<ref INT_PDA_ChatLine> CurrentLines()
	{
		if (m_Sub == SUB_FACTION)
			return INT_PDA_Client.s_FactionChat;
		if (m_Sub == SUB_PRIVATE)
		{
			if (m_OpenKey == "")
				return null;
			array<ref INT_PDA_ChatLine> lst;
			if (INT_PDA_Client.s_Private.Find(m_OpenKey, lst))
				return lst;
			return null;
		}
		return INT_PDA_Client.s_Public;
	}

	protected string UnreadKey()
	{
		if (m_Sub == SUB_FACTION)
			return "faction";
		if (m_Sub == SUB_PRIVATE)
		{
			if (m_OpenKey == "")
				return "";
			return "pm:" + m_OpenKey;
		}
		return "public";
	}

	protected int SubUnread(int sub)
	{
		if (sub == SUB_FACTION)
			return INT_PDA_Client.Unread("faction");
		if (sub == SUB_PRIVATE)
			return INT_PDA_Client.UnreadPrivate();
		return INT_PDA_Client.Unread("public");
	}

	// The open channel is on screen: its unread counter goes (the menu badge follows through s_OnChanged).
	protected void MarkSeen()
	{
		if (!m_Active)
			return;
		string key = UnreadKey();
		if (key == "")
			return;
		if (INT_PDA_Client.Unread(key) <= 0)
			return;
		m_Marking = true;
		INT_PDA_Client.MarkRead(key);
		m_Marking = false;
	}

	// --- feed -----------------------------------------------------------------------------------------------------
	protected void RenderFeed()
	{
		array<ref INT_PDA_ChatLine> src = CurrentLines();
		int n = 0;
		if (src)
			n = src.Count();
		int start = n - KeepFor(m_Sub);
		if (start < 0)
			start = 0;
		int total = n - start;
		m_Total = total;
		// live lines: a reader scrolled up keeps his place, a reader at the bottom sees the new one flash in
		int added = 0;
		int seen;
		if (m_LastSeen && n > 0)
		{
			seen = src.Find(m_LastSeen);
			if (seen >= 0)
				added = n - 1 - seen;
		}
		if (added > 0 && m_Scroll > 0)
		{
			m_Scroll = m_Scroll + added;
			m_NewBelow = m_NewBelow + added;
		}
		bool flashNew = false;
		if (added > 0 && m_Scroll == 0)
			flashNew = true;
		m_LastSeen = null;
		if (n > 0)
			m_LastSeen = src[n - 1];
		// how far up one may scroll: the messages that fit counted from the oldest shown one
		float avail = m_FeedH;
		float used = 0;
		float need;
		float lh;
		int fitTop = 0;
		int i;
		for (i = start; i < n; i++)
		{
			lh = LineHeight(src[i]);
			need = lh;
			if (fitTop > 0)
				need = need + ROW_GAP;
			if (used + need > avail && fitTop > 0)
				break;
			used = used + need;
			fitTop++;
			if (fitTop >= ROW_POOL)
				break;
		}
		m_MaxScroll = total - fitTop;
		if (m_MaxScroll < 0)
			m_MaxScroll = 0;
		if (m_Scroll > m_MaxScroll)
			m_Scroll = m_MaxScroll;
		if (m_Scroll < 0)
			m_Scroll = 0;
		if (m_Scroll == 0)
			m_NewBelow = 0;
		// the visible messages, from the anchor up (whole messages only)
		m_VisIdx.Clear();
		m_VisH.Clear();
		used = 0;
		int anchor = n - 1 - m_Scroll;
		for (i = anchor; i >= start; i--)
		{
			lh = LineHeight(src[i]);
			need = lh;
			if (m_VisIdx.Count() > 0)
				need = need + ROW_GAP;
			if (used + need > avail && m_VisIdx.Count() > 0)
				break;
			used = used + need;
			m_VisIdx.Insert(i);
			m_VisH.Insert(lh);
			if (m_VisIdx.Count() >= ROW_POOL)
				break;
		}
		int vis = m_VisIdx.Count();
		m_VisCount = vis;
		// the oldest message on top when everything fits (or scrolled to the top), else the anchor at the bottom
		float y = 0;
		if (vis > 0 && m_VisIdx[vis - 1] > start)
			y = avail - used;
		int slot = 0;
		int k;
		int li;
		bool flashIt;
		INT_PDA_PageChatRow rv;
		INT_PDA_ChatLine ln;
		float rh;
		for (k = vis - 1; k >= 0; k--)
		{
			li = m_VisIdx[k];
			flashIt = false;
			if (flashNew && li == n - 1)
				flashIt = true;
			rv = m_Rows[slot];
			ln = src[li];
			rh = m_VisH[k];
			FillRow(rv, ln, y, rh, flashIt);
			y = y + rh + ROW_GAP;
			slot++;
		}
		for (k = slot; k < m_Rows.Count(); k++)
		{
			rv = m_Rows[k];
			rv.RowRoot.Show(false);
		}
		// empty channel / no dialog
		if (total == 0)
		{
			string emptyKey = "#STR_INT_PDA_CHAT_EMPTY";
			if (m_Sub == SUB_PRIVATE && m_OpenKey == "")
				emptyKey = "#STR_INT_PDA_CHAT_PICK";
			m_Empty.SetText(emptyKey);
			m_Empty.Show(true);
		}
		else
		{
			m_Empty.Show(false);
		}
		// «к новым»
		bool scrolled = false;
		if (m_Scroll > 0)
			scrolled = true;
		m_ToNew.Show(scrolled);
		m_ToNewHit.Show(scrolled);
		if (scrolled)
		{
			string tn = INT_PDA_Look.Caps("#STR_INT_PDA_CHAT_TO_NEW");
			if (m_NewBelow > 0)
				tn = tn + " · " + m_NewBelow.ToString();
			m_ToNewText.SetText(tn);
		}
		// scroll bar
		bool barOn = false;
		if (m_MaxScroll > 0 && total > 0)
			barOn = true;
		m_Track.Show(barOn);
		m_Thumb.Show(barOn);
		if (barOn)
		{
			float th = avail * vis / total;
			if (th < 30)
				th = 30;
			float frac = m_Scroll * 1.0 / m_MaxScroll;
			float ty = (avail - th) * (1.0 - frac);
			INT_UIF_Draw.Place(m_Thumb, FEED_W + 16, m_FeedTop + ty, 2, th);
		}
		if (m_AirCount)
			m_AirCount.SetText(total.ToString());
		MarkSeen();
	}

	protected void ScrollFeed(int delta)
	{
		m_Scroll = m_Scroll + delta;
		if (m_Scroll > m_MaxScroll)
			m_Scroll = m_MaxScroll;
		if (m_Scroll < 0)
			m_Scroll = 0;
		if (m_Scroll == 0)
			m_NewBelow = 0;
		RenderFeed();
	}

	// Height of one message (design px, without the gap) — the same wrap the row uses.
	protected float LineHeight(INT_PDA_ChatLine line)
	{
		if (line.Channel == INT_PDA_Const.CH_SYSTEM)
		{
			int sr = WrapText(line.Text, SYS_SIZE, FEED_W - 240, MAX_SYS_ROWS);
			return sr * SYS_ROW_H + 14;
		}
		int rows = WrapText(line.Text, TEXT_SIZE, FEED_W - TEXT_X - 12, MAX_ROWS);
		float h = HEAD_H + 2 + rows * ROW_H;
		if (line.IsReply())
			h = h + QUOTE_H;
		if (h < AVATAR_SIZE)
			h = AVATAR_SIZE;
		if ((line.Flags & INT_PDA_Const.FL_AS_FACTION) != 0)
			h = h + 16;
		return h;
	}

	protected void FillRow(INT_PDA_PageChatRow r, INT_PDA_ChatLine line, float y, float h, bool flash)
	{
		if (!r || !r.RowRoot)
			return;
		INT_UIF_Draw.Place(r.RowRoot, 0, y, FEED_W, h);
		r.RowRoot.Show(true);
		r.HitCallsign = "";
		r.LineId = 0;
		r.LineName = "";
		r.LineText = "";
		r.QuoteBar.Show(false);
		r.QuoteText.Show(false);
		r.RowHit.Show(false);
		if (line.Channel == INT_PDA_Const.CH_SYSTEM)
			FillSystem(r, line, h);
		else
			FillMessage(r, line, h);
		if (!flash || !r.FlashImg)
			return;
		INT_UIF_Draw.Place(r.FlashImg, -12, 0, FEED_W * 0.7, h);
		r.FlashImg.SetAlpha(0.3);
		INT_UIF_Animator.Fade(r.FlashImg, 0, 1.6);
	}

	// System line: centred dim text on a faint plate, time on the right.
	protected void FillSystem(INT_PDA_PageChatRow r, INT_PDA_ChatLine line, float h)
	{
		int k;
		Widget plate = r.PlateBox.GetRoot();
		INT_UIF_Draw.Place(plate, 0, 0, FEED_W, h);
		r.PlateBox.SetFill(INT_PDA_Look.PANEL);
		r.PlateBox.SetBorder(INT_PDA_Look.LINE);
		r.PlateBox.Show(true);
		r.AvatarImg.Show(false);
		r.BigEmblemImg.Show(false);
		r.AnonPlate.Show(false);
		r.NameText.Show(false);
		r.EmblemImg.Show(false);
		r.FactionText.Show(false);
		r.TagPill.Show(false);
		r.TagText.Show(false);
		r.RealText.Show(false);
		r.NameHit.Show(false);
		TextWidget tw;
		for (k = 0; k < r.RowTexts.Count(); k++)
		{
			tw = r.RowTexts[k];
			tw.Show(false);
		}
		int rows = WrapText(line.Text, SYS_SIZE, FEED_W - 240, MAX_SYS_ROWS);
		for (k = 0; k < r.SysTexts.Count(); k++)
		{
			tw = r.SysTexts[k];
			if (k >= rows)
			{
				tw.Show(false);
				continue;
			}
			tw.SetText(m_Wrap[k]);
			tw.Show(true);
		}
		INT_UIF_Draw.Place(r.TimeText, FEED_W - 116, 7, 104, SYS_ROW_H);
		r.TimeText.SetText(line.Time);
		r.TimeText.SetColor(INT_PDA_Look.DIM2);
		r.TimeText.Show(true);
	}

	// Player line. What the server left in the line decides the look (INT_PDA_SE_Chat.ViewFor):
	//  - written as a faction: emblem + faction name on a dark red plate, tag; faction mates / admins also get «· author»
	//  - anonymous for me (no author): «НЕТ ДАННЫХ» plate and author, dim text
	//  - anonymous with the real author (admin copy / my own line): plate, the author dim, tag «АНОНИМНО»
	//  - normal: avatar, callsign in the relation colour (own = OWN), small emblem + faction
	protected void FillMessage(INT_PDA_PageChatRow r, INT_PDA_ChatLine line, float h)
	{
		int k;
		bool anon = (line.Flags & INT_PDA_Const.FL_ANON) != 0;
		bool asGp = (line.Flags & INT_PDA_Const.FL_AS_FACTION) != 0;
		bool full = (line.Flags & INT_PDA_Const.FL_ADMIN_VIEW) != 0;
		string me = INT_PDA_Client.s_KnownCallsign;
		bool hidden = false;
		if (line.Author == "")
			hidden = true;
		bool mine = false;
		if (!hidden && line.Author == me)
			mine = true;
		float top = 0;
		if (asGp)
			top = 8;
		// plate
		Widget plate = r.PlateBox.GetRoot();
		if (asGp)
		{
			INT_UIF_Draw.Place(plate, -12, 0, FEED_W + 24, h);
			r.PlateBox.SetFill(PLATE_GP);
			r.PlateBox.SetBorder(PLATE_GP_LINE);
			r.PlateBox.Show(true);
		}
		else
		{
			r.PlateBox.Show(false);
		}
		// picture
		r.AvatarImg.Show(false);
		r.BigEmblemImg.Show(false);
		r.AnonPlate.Show(false);
		if (asGp)
		{
			r.BigEmblemIdx = LoadEmblem(r.BigEmblemImg, line.FactionEmblem, r.BigEmblemIdx);
			INT_UIF_Draw.Move(r.BigEmblemImg, 0, top);
			r.BigEmblemImg.Show(true);
		}
		else if (anon || hidden || line.Avatar < 0)
		{
			INT_UIF_Draw.Move(r.AnonPlate, 0, top);
			r.AnonPlate.Show(true);
		}
		else
		{
			r.AvatarIdx = LoadAvatar(r.AvatarImg, line.Avatar, r.AvatarIdx);
			INT_UIF_Draw.Move(r.AvatarImg, 0, top);
			r.AvatarImg.Show(true);
		}
		// author
		string nameStr = line.Author;
		int nameCol = INT_PDA_Look.NEUTRAL;
		if (asGp)
		{
			nameStr = line.FactionName + "";
			nameStr.ToUpper();
			nameCol = INT_PDA_Look.RelationColor(line.FactionId);
		}
		else if (hidden)
		{
			nameStr = Widget.TranslateString("#STR_INT_PDA_CHAT_ANON_AUTHOR");
			nameCol = INT_PDA_Look.DIM;
		}
		else if (anon)
		{
			nameCol = INT_PDA_Look.DIM;
		}
		else if (mine)
		{
			nameCol = INT_PDA_Look.OWN;
		}
		else
		{
			nameCol = INT_PDA_Look.RelationColor(line.FactionId);
		}
		float x = TEXT_X;
		float nw = Width(nameStr, NAME_SIZE) * 1.06;
		INT_UIF_Draw.Place(r.NameText, x, top, nw + 24, HEAD_H);
		r.NameText.SetText(nameStr);
		r.NameText.SetColor(nameCol);
		r.NameText.Show(true);
		x = x + nw + 12;
		// small emblem + faction of the author
		bool showFaction = false;
		if (!asGp && !hidden && line.FactionId != "")
			showFaction = true;
		if (showFaction)
		{
			r.EmblemIdx = LoadEmblem(r.EmblemImg, line.FactionEmblem, r.EmblemIdx);
			INT_UIF_Draw.Move(r.EmblemImg, x, top + 3);
			r.EmblemImg.Show(true);
			x = x + 26;
			float fw = Width(line.FactionName, META_SIZE);
			INT_UIF_Draw.Place(r.FactionText, x, top, fw + 16, HEAD_H);
			r.FactionText.SetText(line.FactionName);
			r.FactionText.Show(true);
			x = x + fw + 14;
		}
		else
		{
			r.EmblemImg.Show(false);
			r.FactionText.Show(false);
		}
		// tag
		string tagKey = "";
		int tagFill = TAG_GP_FILL;
		int tagCol = TAG_GP_TEXT;
		if (asGp)
		{
			tagKey = "#STR_INT_PDA_CHAT_TAG_FACTION";
		}
		else if (anon && !hidden)
		{
			tagKey = "#STR_INT_PDA_CHAT_TAG_ANON";
			tagFill = TAG_ANON_FILL;
			tagCol = INT_PDA_Look.DIM;
		}
		Widget pill = r.TagPill.GetRoot();
		if (tagKey != "")
		{
			string tagStr = INT_PDA_Look.Caps(tagKey);
			float tgw = Width(tagStr, 12) + 22;
			INT_UIF_Draw.Place(pill, x, top + 3, tgw, 20);
			r.TagPill.SetFill(tagFill);
			r.TagPill.Show(true);
			INT_UIF_Draw.Place(r.TagText, x, top + 3, tgw, 20);
			r.TagText.SetText(tagStr);
			r.TagText.SetColor(tagCol);
			r.TagText.Show(true);
			x = x + tgw + 10;
		}
		else
		{
			r.TagPill.Show(false);
			r.TagText.Show(false);
		}
		// the real author of a faction statement (faction mates, admins, the author)
		if (asGp && full && !hidden)
		{
			r.RealText.SetText("· " + line.Author);
			INT_UIF_Draw.Place(r.RealText, x, top, 300, HEAD_H);
			r.RealText.Show(true);
		}
		else
		{
			r.RealText.Show(false);
		}
		INT_UIF_Draw.Place(r.TimeText, FEED_W - 180, top, 180, HEAD_H);
		r.TimeText.SetText(line.Time);
		r.TimeText.SetColor(INT_PDA_Look.DIM);
		r.TimeText.Show(true);
		// a click on somebody's callsign opens the private dialog with him
		if (!hidden && !mine)
			r.HitCallsign = line.Author;
		INT_UIF_Draw.Place(r.NameHit, TEXT_X - 4, top, nw + 8, HEAD_H);
		r.NameHit.Show(r.HitCallsign != "");
		// right click anywhere on the message: «Ответить» (public / faction lines with a server id) / «Написать в ЛС»
		r.LineId = line.Id;
		r.LineName = nameStr;
		r.LineText = line.Text;
		INT_UIF_Draw.Place(r.RowHit, 0, 0, FEED_W, h);
		r.RowHit.Show(true);
		// reply: the quoted line (built by the server) between the header and the text
		float qh = 0;
		if (line.IsReply())
		{
			qh = QUOTE_H;
			string quote = QuoteName(line) + ": " + line.ReplyText;
			quote = FitOneLine(quote, META_SIZE, FEED_W - TEXT_X - 34);
			INT_UIF_Draw.Place(r.QuoteBar, TEXT_X, top + HEAD_H + 4, 2, QUOTE_H - 7);
			INT_UIF_Draw.Place(r.QuoteText, TEXT_X + 10, top + HEAD_H + 1, FEED_W - TEXT_X - 24, QUOTE_H);
			r.QuoteText.SetText(quote);
			r.QuoteBar.Show(true);
			r.QuoteText.Show(true);
		}
		// text
		int rows = WrapText(line.Text, TEXT_SIZE, FEED_W - TEXT_X - 12, MAX_ROWS);
		int textCol = INT_PDA_Look.TEXT;
		if (!asGp && (hidden || anon))
			textCol = ANON_TEXT;
		TextWidget tw;
		for (k = 0; k < r.RowTexts.Count(); k++)
		{
			tw = r.RowTexts[k];
			if (k >= rows)
			{
				tw.Show(false);
				continue;
			}
			INT_UIF_Draw.Move(tw, TEXT_X, top + HEAD_H + 2 + qh + k * ROW_H);
			tw.SetText(m_Wrap[k]);
			tw.SetColor(textCol);
			tw.Show(true);
		}
		for (k = 0; k < r.SysTexts.Count(); k++)
		{
			tw = r.SysTexts[k];
			tw.Show(false);
		}
	}

	protected int LoadAvatar(ImageWidget img, int idx, int cached)
	{
		if (!img || idx == cached)
			return cached;
		string p = INT_PDA_Look.AvatarPath(idx);
		img.LoadImageFile(0, p);
		return idx;
	}

	protected int LoadEmblem(ImageWidget img, int idx, int cached)
	{
		if (!img || idx == cached)
			return cached;
		string p = INT_PDA_Look.EmblemPath(idx);
		img.LoadImageFile(0, p);
		return idx;
	}

	// --- dialogs (left column) ------------------------------------------------------------------------------------
	// Keys of INT_PDA_Client.s_Private, newest first (by the last line's "DD.MM HH:MM"); a new dialog without lines
	// stays on top until its first line arrives.
	protected void BuildDialogList()
	{
		m_DlgKeys.Clear();
		m_DlgOrder.Clear();
		map<string, ref array<ref INT_PDA_ChatLine>> priv = INT_PDA_Client.s_Private;
		int n = priv.Count();
		int i;
		int j;
		int pos;
		int ord;
		string k;
		array<ref INT_PDA_ChatLine> lst;
		INT_PDA_ChatLine tail;
		for (i = 0; i < n; i++)
		{
			k = priv.GetKey(i);
			lst = priv.GetElement(i);
			ord = 0;
			if (lst && lst.Count() > 0)
			{
				tail = lst[lst.Count() - 1];
				ord = SortStamp(tail.Time);
			}
			pos = m_DlgKeys.Count();
			for (j = 0; j < m_DlgOrder.Count(); j++)
			{
				if (ord > m_DlgOrder[j])
				{
					pos = j;
					break;
				}
			}
			m_DlgKeys.InsertAt(k, pos);
			m_DlgOrder.InsertAt(ord, pos);
		}
		if (m_PendingName == "")
			return;
		string pk = INT_PDA_Util.CallsignKey(m_PendingName);
		if (m_DlgKeys.Find(pk) >= 0)
		{
			m_PendingName = "";
			return;
		}
		m_DlgKeys.InsertAt(pk, 0);
		m_DlgOrder.InsertAt(99999999, 0);
	}

	// "DD.MM HH:MM" -> MMDDHHMM (sortable int)
	protected int SortStamp(string stamp)
	{
		if (stamp.Length() < 11)
			return 0;
		string dd = stamp.Substring(0, 2);
		string mo = stamp.Substring(3, 2);
		string hh = stamp.Substring(6, 2);
		string mi = stamp.Substring(9, 2);
		int dv = dd.ToInt();
		int mv = mo.ToInt();
		int hv = hh.ToInt();
		int iv = mi.ToInt();
		int v = mv * 1000000;
		v = v + dv * 10000;
		v = v + hv * 100;
		v = v + iv;
		return v;
	}

	protected string ShortTime(string stamp)
	{
		if (stamp.Length() >= 11)
			return stamp.Substring(6, 5);
		return stamp;
	}

	// The other side's callsign as the server spells it.
	protected string DialogName(string key, array<ref INT_PDA_ChatLine> lst)
	{
		string me = INT_PDA_Client.s_KnownCallsign;
		int j;
		INT_PDA_ChatLine l;
		if (lst)
		{
			for (j = lst.Count() - 1; j >= 0; j--)
			{
				l = lst[j];
				if (l.Author != "" && l.Author != me)
					return l.Author;
				if (l.Author == me && l.To != "")
					return l.To;
			}
		}
		if (m_PendingName != "")
		{
			string pk = INT_PDA_Util.CallsignKey(m_PendingName);
			if (pk == key)
				return m_PendingName;
		}
		return key;
	}

	protected void RenderCards()
	{
		int total = m_DlgKeys.Count();
		int cap = m_CardCap;
		int maxFirst = total - cap;
		if (maxFirst < 0)
			maxFirst = 0;
		if (m_DlgFirst > maxFirst)
			m_DlgFirst = maxFirst;
		if (m_DlgFirst < 0)
			m_DlgFirst = 0;
		int c;
		int di;
		bool show;
		INT_PDA_PageChatCard card;
		string key;
		for (c = 0; c < m_Cards.Count(); c++)
		{
			card = m_Cards[c];
			di = m_DlgFirst + c;
			show = false;
			if (c < cap && di < total)
				show = true;
			card.CardRoot.Show(show);
			card.CardHit.Show(show);
			if (!show)
				continue;
			key = m_DlgKeys[di];
			FillCard(card, key);
		}
		m_NoDialogs.Show(total == 0);
	}

	protected void FillCard(INT_PDA_PageChatCard card, string key)
	{
		array<ref INT_PDA_ChatLine> lst;
		INT_PDA_Client.s_Private.Find(key, lst);
		string me = INT_PDA_Client.s_KnownCallsign;
		string nameStr = DialogName(key, lst);
		int avatar = -1;
		string stamp = "";
		string last = "";
		int cnt = 0;
		int j;
		INT_PDA_ChatLine l;
		if (lst)
			cnt = lst.Count();
		if (cnt > 0)
		{
			INT_PDA_ChatLine tail = lst[cnt - 1];
			stamp = ShortTime(tail.Time);
			last = tail.Text;
			if (tail.Author == me)
			{
				string you = Widget.TranslateString("#STR_INT_PDA_CHAT_YOU");
				last = you + ": " + tail.Text;
			}
			for (j = cnt - 1; j >= 0; j--)
			{
				l = lst[j];
				if (l.Author != "" && l.Author != me)
				{
					avatar = l.Avatar;
					break;
				}
			}
		}
		else
		{
			last = Widget.TranslateString("#STR_INT_PDA_CHAT_NEW_HINT");
		}
		card.DlgKey = key;
		card.DlgName = nameStr;
		string nameFit = FitOneLine(nameStr, 18, 180);
		card.NameText.SetText(nameFit);
		card.TimeText.SetText(stamp);
		string lastFit = FitOneLine(last, 14, COL_W - 130);
		card.LastText.SetText(lastFit);
		if (avatar >= 0)
		{
			card.AvatarIdx = LoadAvatar(card.AvatarImg, avatar, card.AvatarIdx);
			card.AvatarImg.Show(true);
			card.UserIcon.Show(false);
		}
		else
		{
			card.AvatarImg.Show(false);
			card.UserIcon.Show(true);
		}
		string uk = "pm:" + key;
		int unread = INT_PDA_Client.Unread(uk);
		bool hasBadge = false;
		if (unread > 0)
			hasBadge = true;
		card.BadgeFrame.Show(hasBadge);
		if (hasBadge)
		{
			string bt = unread.ToString();
			if (unread > 99)
				bt = "99";
			card.BadgeText.SetText(bt);
		}
		int fill = INT_PDA_Look.PANEL;
		int border = INT_PDA_Look.LINE;
		if (hasBadge)
			border = INT_PDA_Look.ORANGE2;
		if (m_Sub == SUB_PRIVATE && key == m_OpenKey)
		{
			fill = CARD_SEL_FILL;
			border = CARD_SEL_LINE;
		}
		card.BaseFill = fill;
		card.BoxSurf.SetFill(fill);
		card.BoxSurf.SetBorder(border);
	}

	// --- «Удалить диалог?» -----------------------------------------------------------------------------------------
	protected bool DeleteAskOpen()
	{
		return m_Del && m_Del.IsVisible();
	}

	protected void AskDeleteDialog(int ci)
	{
		if (ci < 0 || ci >= m_Cards.Count() || !m_Del)
			return;
		INT_PDA_PageChatCard card = m_Cards[ci];
		if (card.DlgKey == "")
			return;
		CloseContext();
		m_DelKey = card.DlgKey;
		string nameFit = INT_PDA_Look.Fit(card.DlgName, 500, 28);
		m_DelName.SetText(nameFit);
		m_Del.Show(true);
		INT_UIF_Draw.Move(m_DelBox, (m_W - 560) * 0.5, (m_H - 256) * 0.5);
		INT_UIF_Animator.Reveal(m_DelBox, 0, 12, 0.22);
		INT_PDA_Sfx.Play("Click");
	}

	protected void CloseDeleteAsk()
	{
		m_DelKey = "";
		if (m_Del)
			m_Del.Show(false);
	}

	protected void OnDelNo(INT_UIF_Button b)
	{
		INT_PDA_Sfx.Play("Click");
		CloseDeleteAsk();
	}

	protected void OnDelYes(INT_UIF_Button b)
	{
		string key = m_DelKey;
		CloseDeleteAsk();
		DeleteDialog(key);
	}

	// The whole dialog goes from this client: its lines, its unread counter, the open / pending state.
	protected void DeleteDialog(string key)
	{
		if (key == "")
			return;
		if (m_PendingName != "" && INT_PDA_Util.CallsignKey(m_PendingName) == key)
			m_PendingName = "";
		if (m_OpenKey == key)
		{
			m_OpenKey = "";
			m_OpenName = "";
			m_Scroll = 0;
			m_NewBelow = 0;
			m_LastSeen = null;
		}
		INT_PDA_Sfx.Play("Click");
		INT_PDA_Client.ForgetDialog(key);
		RefreshAll();
	}

	protected void ScrollCards(int delta)
	{
		m_DlgFirst = m_DlgFirst + delta;
		RenderCards();
	}

	protected void NewDialogPrompt()
	{
		if (!m_Menu)
			return;
		INT_PDA_Prompt p = m_Menu.OpenPrompt("#STR_INT_PDA_CHAT_NEW_DIALOG", "#STR_INT_PDA_F_CALLSIGN", "", false);
		if (p)
			p.m_OnDone.Insert(OnNewDialogDone);
	}

	protected void OnNewDialogDone(bool ok, string a, string b)
	{
		m_Menu.ClosePrompt();
		if (!ok)
			return;
		string cs = a + "";
		cs.TrimInPlace();
		if (!INT_PDA_Util.IsCallsign(cs))
		{
			Refuse("#STR_INT_PDA_R_CALLSIGN_FORMAT");
			return;
		}
		string k1 = INT_PDA_Util.CallsignKey(cs);
		string k2 = INT_PDA_Util.CallsignKey(INT_PDA_Client.s_KnownCallsign);
		if (k1 == k2)
		{
			Refuse("#STR_INT_PDA_CHAT_SELF");
			return;
		}
		OpenPrivate(cs);
		INT_UIF_TextInput input = ActiveInput();
		if (input)
			input.Focus();
	}

	// --- left-column texts / badges ---------------------------------------------------------------------------------
	protected void PaintSubTabs()
	{
		bool facOk = FactionOk();
		int i;
		int n;
		string txt;
		bool show;
		for (i = 0; i < m_SubText.Count(); i++)
		{
			PaintSubTab(i, false);
			n = SubUnread(i);
			if (i == SUB_FACTION && !facOk)
				n = 0;
			show = false;
			if (n > 0)
				show = true;
			m_SubBadge[i].Show(show);
			if (!show)
				continue;
			txt = n.ToString();
			if (n > 99)
				txt = "99";
			m_SubBadgeText[i].SetText(txt);
		}
		if (m_SubLock)
			m_SubLock.Show(!facOk);
	}

	protected void PaintSubTab(int i, bool hover)
	{
		if (i < 0 || i >= m_SubText.Count())
			return;
		int col = INT_PDA_Look.DIM;
		bool locked = false;
		if (i == SUB_FACTION && !FactionOk())
			locked = true;
		if (locked)
			col = INT_PDA_Look.DIM2;
		else if (hover)
			col = INT_PDA_Look.TEXT;
		if (i == m_Sub)
			col = INT_PDA_Look.TEXT;
		m_SubText[i].SetColor(col);
	}

	protected void UpdateAir()
	{
		if (!m_AirNet)
			return;
		if (INT_PDA_Client.IsListening())
		{
			m_AirNet.SetText("#STR_INT_PDA_CHAT_AIR_ON");
			m_AirNet.SetColor(INT_PDA_Look.ALLY);
		}
		else
		{
			m_AirNet.SetText("#STR_INT_PDA_CHAT_AIR_OFF");
			m_AirNet.SetColor(INT_PDA_Look.ENEMY);
		}
		string sk = SubKey(m_Sub);
		string chan = INT_PDA_Look.Caps(sk);
		int mc = MaxChars();
		string chars = Widget.TranslateString("#STR_INT_PDA_CHAT_CHARS");
		string chanText = chan + " · " + mc.ToString() + " " + chars;
		m_AirChannel.SetText(chanText);
	}

	// --- input ----------------------------------------------------------------------------------------------------
	protected void UpdateCounter()
	{
		if (!m_Counter)
			return;
		INT_UIF_TextInput input = ActiveInput();
		int n = 0;
		if (input)
		{
			string cur = input.GetText();
			n = cur.LengthUtf8();
		}
		int mc = MaxChars();
		string usedTxt = n.ToString();
		string maxTxt = mc.ToString();
		m_Counter.SetText(usedTxt + " / " + maxTxt);
		int col = INT_PDA_Look.DIM;
		if (n * 10 >= mc * 9)
			col = INT_PDA_Look.ORANGE;
		m_Counter.SetColor(col);
	}

	protected void OnInputChanged(string text, INT_UIF_TextInput t)
	{
		INT_PDA_Sfx.Play("Key");
		// '#' is not allowed in chat (owner 2026-10-23; the engine reads a text starting with it as a stringtable
		// key): it is taken out as it is typed, with a hint at most every 3 s
		if (t && text.IndexOf("#") >= 0)
		{
			string clean = text + "";
			clean.Replace("#", "");
			t.SetText(clean);
			text = clean;
			int now = GetGame().GetTime();
			if (Since(s_LastHashHintMs, now) > 3000)
			{
				s_LastHashHintMs = now;
				Refuse("#STR_INT_PDA_CHAT_NO_HASH");
			}
		}
		int mc = MaxChars();
		if (t && text.LengthUtf8() > mc)
		{
			string cut = text.SubstringUtf8(0, mc);
			t.SetText(cut);
		}
		UpdateCounter();
	}

	protected void OnInputSubmit(string text, INT_UIF_TextInput t)
	{
		DoSend();
	}

	// «Анонимно» and «От имени ГП» exclude each other (the server drops the anonymous flag of a faction statement).
	protected void OnAnonToggled(bool on, INT_UIF_Toggle t)
	{
		INT_PDA_Sfx.Play("Toggle");
		if (on && m_AsGp && m_AsGp.IsOn())
			m_AsGp.SetOn(false, false, true);
	}

	protected void OnGpToggled(bool on, INT_UIF_Toggle t)
	{
		INT_PDA_Sfx.Play("Toggle");
		if (on && m_Anon && m_Anon.IsOn())
			m_Anon.SetOn(false, false, true);
	}

	// Client anti-flood (the server keeps only a hard 1 s floor): not faster than FloodSeconds, not the same text
	// again within SameTextSeconds.
	protected void DoSend()
	{
		INT_UIF_TextInput input = ActiveInput();
		if (!input)
			return;
		int mc = MaxChars();
		string raw = input.GetText();
		string text = INT_PDA_Util.CleanLine(raw, mc, INT_PDA_Const.MAX_WIRE_BYTES - 100);
		if (text == "")
		{
			INT_PDA_Sfx.Play("Error");
			m_LastRefusal = "empty";
			return;
		}
		if (text.IndexOf("#") >= 0)
		{
			Refuse("#STR_INT_PDA_CHAT_NO_HASH");
			return;
		}
		int now = GetGame().GetTime();
		float floodMs = INT_PDA_Client.s_Settings.FloodSeconds * 1000.0;
		float sameMs = INT_PDA_Client.s_Settings.SameTextSeconds * 1000.0;
		int sinceSend = Since(s_LastSendMs, now);
		int sinceText = Since(s_LastTextMs, now);
		if (sinceSend < floodMs)
		{
			Refuse("#STR_INT_PDA_CHAT_FLOOD");
			return;
		}
		if (text == s_LastText && sinceText < sameMs)
		{
			Refuse("#STR_INT_PDA_CHAT_SAME");
			return;
		}
		if (m_Sub == SUB_PRIVATE)
		{
			if (m_OpenName == "")
			{
				Refuse("#STR_INT_PDA_CHAT_PICK");
				return;
			}
			INT_PDA_Client.SendPm(m_OpenName, text);
		}
		else
		{
			int channel = INT_PDA_Const.CH_PUBLIC;
			if (m_Sub == SUB_FACTION)
			{
				if (!FactionOk())
				{
					RefuseFaction();
					return;
				}
				channel = INT_PDA_Const.CH_FACTION;
			}
			int flags = 0;
			if (m_AnonAvail && m_Anon && m_Anon.IsOn())
				flags = flags | INT_PDA_Const.FL_ANON;
			if (m_GpAvail && m_AsGp && m_AsGp.IsOn())
				flags = flags | INT_PDA_Const.FL_AS_FACTION;
			int replyId = 0;
			if (m_ReplyId > 0 && m_ReplySub == m_Sub)
				replyId = m_ReplyId;
			INT_PDA_Client.SendChat(channel, flags, text, replyId);
			ClearReply(true);
		}
		s_LastSendMs = now;
		s_LastTextMs = now;
		s_LastText = text;
		m_LastRefusal = "";
		m_Sent++;
		input.SetText("");
		UpdateCounter();
		INT_PDA_Sfx.Play("Send");
		m_Scroll = 0;
		m_NewBelow = 0;
		RenderFeed();
		input.Focus();
	}

	// --- reply / context menu ---------------------------------------------------------------------------------------
	// Author of the quoted line as the server left it: a faction statement under the faction, anonymous = «Аноним».
	protected string QuoteName(INT_PDA_ChatLine line)
	{
		if ((line.ReplyFlags & INT_PDA_Const.FL_AS_FACTION) != 0 && line.ReplyAuthor != "")
			return INT_PDA_Look.Caps(line.ReplyAuthor);
		if (line.ReplyAuthor == "")
			return Widget.TranslateString("#STR_INT_PDA_CHAT_ANON_AUTHOR");
		return line.ReplyAuthor;
	}

	protected void StartReply(int lineId, string name, string text)
	{
		if (lineId <= 0 || m_Sub == SUB_PRIVATE)
			return;
		m_ReplyId = lineId;
		m_ReplySub = m_Sub;
		m_ReplyName = name;
		m_ReplySnippet = text;
		string cap = Widget.TranslateString("#STR_INT_PDA_CHAT_REPLYING");
		cap.Replace("%1", name);
		string full = cap + ":  " + text;
		m_ReplyText.SetText(FitOneLine(full, 15, FEED_W - 120));
		m_ReplyText.SetColor(INT_PDA_Look.TEXT);
		INT_PDA_Sfx.Play("Toggle");
		m_Scroll = 0;
		m_NewBelow = 0;
		ApplyMode();
		RenderFeed();
		INT_UIF_TextInput input = ActiveInput();
		if (input)
			input.Focus();
	}

	protected void ClearReply(bool redraw)
	{
		if (m_ReplyId <= 0)
			return;
		m_ReplyId = 0;
		m_ReplyName = "";
		m_ReplySnippet = "";
		if (m_ReplyBar)
			m_ReplyBar.Show(false);
		if (!redraw)
			return;
		ApplyMode();
		RenderFeed();
	}

	protected bool CtxOpen()
	{
		return m_Ctx && m_Ctx.IsVisible();
	}

	// Right click on a message row: the items that make sense for it, next to the cursor (screen px -> page px).
	protected void OpenContext(int ri, float sx, float sy)
	{
		CloseContext();
		if (ri < 0 || ri >= m_Rows.Count())
			return;
		INT_PDA_PageChatRow row = m_Rows[ri];
		bool canReply = row.LineId > 0 && m_Sub != SUB_PRIVATE;
		bool canPm = row.HitCallsign != "";
		if (!canReply && !canPm)
			return;
		m_CtxLineId = row.LineId;
		m_CtxName = row.LineName;
		m_CtxText2 = row.LineText;
		m_CtxCallsign = row.HitCallsign;
		m_CtxAction.Clear();
		if (canReply)
			m_CtxAction.Insert(CTX_REPLY);
		if (canPm)
			m_CtxAction.Insert(CTX_PM);
		int n = m_CtxAction.Count();
		float tw = 0;
		int i;
		for (i = 0; i < 2; i++)
		{
			bool on = i < n;
			m_CtxHover[i].Show(on);
			m_CtxHover[i].SetAlpha(0);
			m_CtxIcon[i].Show(on);
			m_CtxText[i].Show(on);
			m_CtxHit[i].Show(on);
			if (!on)
				continue;
			string key = "#STR_INT_PDA_CHAT_CTX_REPLY";
			string icon = "chat";
			if (m_CtxAction[i] == CTX_PM)
			{
				key = "#STR_INT_PDA_CHAT_CTX_PM";
				icon = "mail";
			}
			m_CtxText[i].SetText(key);
			m_CtxIcon[i].LoadImageFile(0, INT_PDA_Look.IconPath(icon));
			tw = Math.Max(tw, INT_PDA_Look.TextW(Widget.TranslateString(key), 15));
		}
		// width by the longest item, height by the item count
		float w = Math.Clamp(tw + 76, 220, 420);
		float h = 12 + n * CTX_ITEM_H;
		INT_UIF_Draw.Place(m_CtxBox.GetRoot(), 0, 0, w, h);
		if (m_CtxShadow)
			INT_UIF_Draw.Place(m_CtxShadow.GetRoot(), -16, -10, w + 32, h + 28);
		INT_UIF_Draw.Place(m_CtxStripe, 0, 0, 3, h);
		for (i = 0; i < 2; i++)
		{
			INT_UIF_Draw.Place(m_CtxHover[i], 6, 6 + i * CTX_ITEM_H, w - 12, CTX_ITEM_H);
			INT_UIF_Draw.Place(m_CtxText[i], 48, 6 + i * CTX_ITEM_H, w - 60, CTX_ITEM_H);
			INT_UIF_Draw.Place(m_CtxHit[i], 6, 6 + i * CTX_ITEM_H, w - 12, CTX_ITEM_H);
		}
		float rx;
		float ry;
		m_Main.GetScreenPos(rx, ry);
		float f = INT_UIF_Scale.Factor();
		if (f <= 0)
			f = 1;
		float lx = (sx - rx) / f + 6;
		float ly = (sy - ry) / f + 4;
		lx = Math.Clamp(lx, 8, m_W - w - 8);
		ly = Math.Clamp(ly, 8, m_H - h - 8);
		INT_UIF_Draw.Place(m_Ctx, lx, ly, w, h);
		m_Ctx.Show(true);
		// the right button that opened the menu is still down: that press must not close it
		m_CtxHeld = MouseDown();
		INT_PDA_Sfx.Play("Click");
	}

	protected void CloseContext()
	{
		if (m_Ctx)
			m_Ctx.Show(false);
	}

	protected void CtxAct(int item)
	{
		if (item < 0 || item >= m_CtxAction.Count())
			return;
		int act = m_CtxAction[item];
		CloseContext();
		if (act == CTX_REPLY)
		{
			StartReply(m_CtxLineId, m_CtxName, m_CtxText2);
			return;
		}
		if (act == CTX_PM && m_CtxCallsign != "")
		{
			INT_PDA_Sfx.Play("Click");
			OpenPrivate(m_CtxCallsign);
		}
	}

	protected int CtxItemOfHit(Widget w)
	{
		if (!w)
			return -1;
		for (int i = 0; i < m_CtxHit.Count(); i++)
		{
			if (m_CtxHit[i] == w)
				return i;
		}
		return -1;
	}

	protected bool MouseDown()
	{
		return (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) != 0 || (GetMouseState(MouseState.RIGHT) & MB_PRESSED_MASK) != 0;
	}

	// A new press outside the open menu closes it (the hits of other widgets do not report to this page). The mask is
	// "button held", so the press is taken on its first frame only: the right press that opened the menu (the menu
	// sits beside the cursor) closed it on the same frame (owner 2026-10-23: the menu never showed).
	protected void PollContext()
	{
		if (!CtxOpen())
			return;
		bool down = MouseDown();
		bool fresh = down && !m_CtxHeld;
		m_CtxHeld = down;
		if (!fresh)
			return;
		int mx;
		int my;
		GetMousePos(mx, my);
		float cx;
		float cy;
		float cw;
		float ch;
		m_Ctx.GetScreenPos(cx, cy);
		m_Ctx.GetScreenSize(cw, ch);
		if (mx >= cx && mx <= cx + cw && my >= cy && my <= cy + ch)
			return;
		CloseContext();
	}

	// --- widget events --------------------------------------------------------------------------------------------
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (button != MouseState.RIGHT)
			return false;
		int ci = CardOfHit(w);
		if (ci >= 0)
		{
			AskDeleteDialog(ci);
			return true;
		}
		int ri = RowOfHit(w);
		if (ri < 0)
			return false;
		OpenContext(ri, x, y);
		return true;
	}

	override bool HandleEscape()
	{
		if (DeleteAskOpen())
		{
			INT_PDA_Sfx.Play("Click");
			CloseDeleteAsk();
			return true;
		}
		if (CtxOpen())
		{
			CloseContext();
			return true;
		}
		if (m_ReplyId > 0)
		{
			ClearReply(true);
			return true;
		}
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int i;
		int ctxItem = CtxItemOfHit(w);
		if (ctxItem >= 0)
		{
			CtxAct(ctxItem);
			return true;
		}
		// the release of the right press that opened the menu / the «Удалить диалог?» question
		if (button == MouseState.RIGHT && (RowOfHit(w) >= 0 || CardOfHit(w) >= 0))
			return true;
		CloseContext();
		if (w == m_ReplyClose)
		{
			INT_PDA_Sfx.Play("Click");
			ClearReply(true);
			return true;
		}
		for (i = 0; i < m_SubHit.Count(); i++)
		{
			if (w == m_SubHit[i])
			{
				if (i != m_Sub)
					SelectSub(i, true);
				return true;
			}
		}
		if (w == m_RefreshHit)
		{
			DoRefresh();
			return true;
		}
		if (w == m_NewHit)
		{
			NewDialogPrompt();
			return true;
		}
		if (w == m_SendHit)
		{
			DoSend();
			return true;
		}
		if (w == m_ToNewHit)
		{
			INT_PDA_Sfx.Play("Click");
			m_Scroll = 0;
			m_NewBelow = 0;
			RenderFeed();
			return true;
		}
		int ci = CardOfHit(w);
		if (ci >= 0)
		{
			INT_PDA_PageChatCard card = m_Cards[ci];
			INT_PDA_Sfx.Play("Click");
			OpenPrivate(card.DlgName);
			return true;
		}
		int ri = RowOfHit(w);
		if (ri >= 0)
		{
			INT_PDA_PageChatRow row = m_Rows[ri];
			// a left click on the callsign opens the dialog; on the rest of the message it does nothing
			if (w != row.NameHit || row.HitCallsign == "")
				return true;
			INT_PDA_Sfx.Play("Click");
			OpenPrivate(row.HitCallsign);
			return true;
		}
		return false;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (w == m_FeedHit || w == m_ToNewHit || RowOfHit(w) >= 0)
		{
			CloseContext();
			ScrollFeed(wheel);
			return true;
		}
		if (w == m_ListHit || CardOfHit(w) >= 0)
		{
			ScrollCards(-wheel);
			return true;
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		HoverHit(w, true);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		HoverHit(w, false);
		return false;
	}

	protected void HoverHit(Widget w, bool enter)
	{
		int i;
		for (i = 0; i < m_SubHit.Count(); i++)
		{
			if (w == m_SubHit[i])
			{
				PaintSubTab(i, enter);
				return;
			}
		}
		if (w == m_RefreshHit)
		{
			int rc = INT_PDA_Look.DIM;
			if (enter)
				rc = INT_PDA_Look.TEXT;
			m_RefreshText.SetColor(rc);
			m_RefreshIcon.SetColor(rc);
			return;
		}
		if (w == m_NewHit && m_NewBox)
		{
			int nf = INT_PDA_Look.PANEL;
			if (enter)
				nf = INT_PDA_Look.PANEL2;
			m_NewBox.FadeFill(nf, 0.12);
			return;
		}
		if (w == m_SendHit && m_SendBox)
		{
			int sf = INT_PDA_Look.ORANGE;
			if (enter)
				sf = SEND_HOVER;
			m_SendBox.FadeFill(sf, 0.12);
			return;
		}
		int ctxItem = CtxItemOfHit(w);
		if (ctxItem >= 0)
		{
			float ha = 0;
			if (enter)
				ha = 1;
			m_CtxHover[ctxItem].SetAlpha(ha);
			return;
		}
		int ci = CardOfHit(w);
		if (ci < 0)
			return;
		INT_PDA_PageChatCard card = m_Cards[ci];
		int cf = card.BaseFill;
		if (enter)
			cf = INT_PDA_Look.PANEL2;
		card.BoxSurf.FadeFill(cf, 0.12);
	}

	protected int CardOfHit(Widget w)
	{
		if (!w)
			return -1;
		int i;
		INT_PDA_PageChatCard card;
		for (i = 0; i < m_Cards.Count(); i++)
		{
			card = m_Cards[i];
			if (card.CardHit == w)
				return i;
		}
		return -1;
	}

	protected int RowOfHit(Widget w)
	{
		if (!w)
			return -1;
		int i;
		INT_PDA_PageChatRow row;
		for (i = 0; i < m_Rows.Count(); i++)
		{
			row = m_Rows[i];
			if (row.NameHit == w || row.RowHit == w)
				return i;
		}
		return -1;
	}

	// --- text metrics -----------------------------------------------------------------------------------------------
	// Width of a one-line string in design px at glyph size `size`: measured by the probe while the page is on
	// screen (cached), estimated otherwise (not cached).
	protected float Width(string s, float size)
	{
		if (s == "")
			return 0;
		float f = INT_UIF_Scale.Factor();
		if (f != m_CacheFactor)
		{
			m_WidthCache.Clear();
			m_CacheFactor = f;
		}
		int sz = (int)Math.Round(size * 10);
		string key = sz.ToString() + "|" + s;
		float cached;
		if (m_WidthCache.Find(key, cached))
			return cached;
		float w = INT_UIF_Draw.TextWidth(s, size) * 1.04;
		if (!m_Probe || !m_Root || !m_Root.IsVisible() || f <= 0)
			return w;
		int px = INT_UIF_Scale.TextPx(size);
		m_Probe.SetTextExactSize(px);
		m_Probe.SetText(s);
		m_Probe.Update();
		int mw;
		int mh;
		m_Probe.GetTextSize(mw, mh);
		if (mw <= 0)
			return w;
		w = mw / f;
		if (m_WidthCache.Count() > 4000)
			m_WidthCache.Clear();
		m_WidthCache.Set(key, w);
		return w;
	}

	protected float SpaceWidth(float size)
	{
		float ab = Width("ab", size);
		float spaced = Width("a b", size);
		float sp = spaced - ab;
		if (sp < size * 0.18)
			sp = size * 0.28;
		return sp;
	}

	// Largest number of characters of `s` (from the start) whose width fits `maxW`; at least 1.
	protected int FitChars(string s, float size, float maxW)
	{
		int n = s.LengthUtf8();
		if (n <= 1)
			return n;
		int lo = 1;
		int hi = n;
		int mid;
		string part;
		while (lo < hi)
		{
			mid = (lo + hi + 1) / 2;
			part = s.SubstringUtf8(0, mid);
			if (Width(part, size) <= maxW)
				lo = mid;
			else
				hi = mid - 1;
		}
		return lo;
	}

	protected string FitOneLine(string s, float size, float maxW)
	{
		if (Width(s, size) <= maxW)
			return s;
		float dots = Width("…", size);
		int fit = FitChars(s, size, maxW - dots);
		string head = s.SubstringUtf8(0, fit);
		return head + "…";
	}

	// Greedy word wrap into m_Wrap (whole rows of text); a word longer than a row is cut by characters; more than
	// maxRows rows end with «…». Returns the row count (at least 1).
	protected int WrapText(string text, float size, float maxW, int maxRows)
	{
		m_Wrap.Clear();
		m_Words.Clear();
		text.Split(" ", m_Words);
		float spaceW = SpaceWidth(size);
		string row = "";
		float rowW = 0;
		int wi;
		int nWords = m_Words.Count();
		string word;
		string rest;
		string head;
		float ww;
		float need;
		bool fits;
		float realW;
		int fit;
		int total;
		for (wi = 0; wi < nWords; wi++)
		{
			word = m_Words[wi];
			if (word == "")
				continue;
			rest = word;
			while (rest != "")
			{
				ww = Width(rest, size);
				need = ww;
				if (row != "")
					need = ww + spaceW;
				fits = false;
				if (rowW + need <= maxW)
					fits = true;
				// near the edge the sum of word widths may differ from the width of the whole row: measure it
				if (fits && row != "" && rowW + need > maxW - size * 2)
				{
					realW = Width(row + " " + rest, size);
					if (realW > maxW)
						fits = false;
					else
						need = realW - rowW;
				}
				if (fits)
				{
					if (row != "")
						row = row + " ";
					row = row + rest;
					rowW = rowW + need;
					rest = "";
					continue;
				}
				if (row != "")
				{
					m_Wrap.Insert(row);
					row = "";
					rowW = 0;
					continue;
				}
				// an empty row and the word still does not fit: cut it
				fit = FitChars(rest, size, maxW);
				total = rest.LengthUtf8();
				head = rest.SubstringUtf8(0, fit);
				m_Wrap.Insert(head);
				rest = rest.SubstringUtf8(fit, total - fit);
			}
		}
		if (row != "" || m_Wrap.Count() == 0)
			m_Wrap.Insert(row);
		if (m_Wrap.Count() <= maxRows)
			return m_Wrap.Count();
		while (m_Wrap.Count() > maxRows)
			m_Wrap.RemoveOrdered(m_Wrap.Count() - 1);
		string lastRow = m_Wrap[maxRows - 1];
		int lastLen = lastRow.LengthUtf8();
		if (lastLen > 2)
			lastRow = lastRow.SubstringUtf8(0, lastLen - 2);
		m_Wrap.Set(maxRows - 1, lastRow + "…");
		return maxRows;
	}

	// --- self-test ------------------------------------------------------------------------------------------------
	override void OnTestCommand(string cmd)
	{
		if (cmd.IndexOf("chat ") != 0)
			return;
		array<string> parts = new array<string>();
		cmd.Split(" ", parts);
		if (parts.Count() < 2)
			return;
		string verb = parts[1];
		string arg = "";
		if (parts.Count() >= 3)
			arg = parts[2];
		string body;
		string cs;
		int num = arg.ToInt();
		INT_UIF_TextInput input;
		if (verb == "send")
		{
			body = JoinFrom(parts, 2);
			TestSend(body, false);
		}
		else if (verb == "anon")
		{
			body = JoinFrom(parts, 2);
			TestSend(body, true);
		}
		else if (verb == "tab")
		{
			SelectSub(num, false);
		}
		else if (verb == "pm")
		{
			cs = arg + "";
			cs.Replace("_", " ");
			OpenPrivate(cs);
			body = JoinFrom(parts, 3);
			input = ActiveInput();
			if (body != "" && input)
			{
				input.SetText(body);
				DoSend();
			}
		}
		else if (verb == "refresh")
		{
			DoRefresh();
		}
		else if (verb == "scroll")
		{
			ScrollFeed(num);
		}
		else if (verb == "ctx" || verb == "reply")
		{
			// n-th message counted from the newest one on screen (1 = newest)
			int slot = m_VisCount - num;
			if (num < 1 || slot < 0 || slot >= m_Rows.Count())
			{
				m_LastRefusal = "no-row";
			}
			else if (verb == "ctx")
			{
				float px;
				float py;
				INT_PDA_PageChatRow cr = m_Rows[slot];
				cr.RowRoot.GetScreenPos(px, py);
				float ox = INT_UIF_Draw.S(TEXT_X + 120);
				float oy = INT_UIF_Draw.S(10);
				OpenContext(slot, px + ox, py + oy);
			}
			else
			{
				INT_PDA_PageChatRow tr = m_Rows[slot];
				StartReply(tr.LineId, tr.LineName, tr.LineText);
				body = JoinFrom(parts, 3);
				input = ActiveInput();
				if (body != "" && input && m_ReplyId > 0)
				{
					input.SetText(body);
					DoSend();
				}
			}
		}
		else if (verb == "ctxclose")
		{
			CloseContext();
		}
		else if (verb == "del")
		{
			// n-th dialog card from the top (1 = top)
			int card = num - 1;
			if (num < 1 || card >= m_Cards.Count() || !m_Cards[card].CardRoot.IsVisible())
				m_LastRefusal = "no-card";
			else
				AskDeleteDialog(card);
		}
		else if (verb == "delyes")
		{
			OnDelYes(m_DelYes);
		}
		else if (verb == "delno")
		{
			OnDelNo(m_DelNo);
		}
		TestReport(verb);
	}

	protected void TestSend(string text, bool anon)
	{
		if (m_Sub == SUB_PRIVATE)
			SelectSub(SUB_PUBLIC, false);
		INT_UIF_TextInput input = ActiveInput();
		if (!input)
			return;
		input.SetText(text);
		bool wasOn = false;
		if (m_Anon)
			wasOn = m_Anon.IsOn();
		if (anon)
		{
			if (!m_AnonAvail || !m_Anon)
			{
				m_LastRefusal = "anon-off";
				return;
			}
			m_Anon.SetOn(true, true, false);
		}
		DoSend();
		if (anon && m_Anon && !wasOn)
			m_Anon.SetOn(false, false, false);
	}

	protected void TestReport(string verb)
	{
		string openTxt = m_OpenName;
		if (openTxt == "")
			openTxt = "-";
		string refusal = m_LastRefusal;
		if (refusal == "")
			refusal = "-";
		int dialogs = m_DlgKeys.Count();
		bool chatOk = HasMod(INT_PDA.SLOT_CHAT);
		string s = "CHAT_TEST " + verb + " sub=" + m_Sub.ToString() + " lines=" + m_Total.ToString() + " shown=" + m_VisCount.ToString();
		s = s + " scroll=" + m_Scroll.ToString() + "/" + m_MaxScroll.ToString() + " new=" + m_NewBelow.ToString() + " sent=" + m_Sent.ToString();
		s = s + " dialogs=" + dialogs.ToString() + " open=" + openTxt + " module=" + chatOk.ToString() + " refused=" + refusal;
		bool ctxOn = CtxOpen();
		s = s + " reply=" + m_ReplyId.ToString() + " ctx=" + ctxOn.ToString() + " ctxItems=" + m_CtxAction.Count().ToString();
		bool delOn = DeleteAskOpen();
		s = s + " delAsk=" + delOn.ToString() + " privDialogs=" + INT_PDA_Client.s_Private.Count().ToString();
		INT_PDA_Log.Raw(s);
	}

	protected string JoinFrom(array<string> parts, int first)
	{
		string s = "";
		int i;
		for (i = first; i < parts.Count(); i++)
		{
			if (s != "")
				s = s + " ";
			s = s + parts[i];
		}
		return s;
	}
}
