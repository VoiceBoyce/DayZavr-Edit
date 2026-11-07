// INT_PDA (client) — «Админ» tab (owner brief, admin items 1-21; spec.md §5: only admins from the server config, Alt+L
// opens the PDA in admin mode without the item and without a login). Style E like the other pages (no own concept
// picture): a sub-tab bar as in the chat concept, dark plates with hairline borders, captions of admin panels in the
// admin accent INT_PDA_Look.ADMIN. Sub-tabs:
//   «Игроки»      — server-paged account table (search: SteamID / exact callsign / part of a callsign), ← → pages,
//                   wheel scroll inside a page; a click opens the stalker card on the right (every field, the password
//                   masked with an eye toggle, role dropdown, expel from the faction, new password, delete account);
//   «Группировки» — INT_PDA_AdminFactions (own file, written separately), hosted below the bar;
//   «Новости»     — the feed (newest first) with delete, the «Новая новость» form with a live preview;
//   «Система»     — the «Рядом» radius, the three disk reloads, an info block.
// The server drops any request that comes sooner than REQUEST_GAP_MS after the previous one of the same player and does
// not answer it, so every request of this page goes through one client queue that keeps that gap (plus a margin).
//
// Self-test (INT_PDA_Menu.OnTestCommand forwards to the active tab; select it first with "tab 5"):
//   adm players [filter]     search (empty = all accounts)       adm page <n>          page n (1-based)
//   adm select <steam>       open the card (searches if needed)  adm pass <steam> <pw> new password (no prompt)
//   adm del <steam>          delete the account (no confirm)     adm role <steam> <0-3> 0 = expel, 1-3 = role
//   adm news <title>|<text>  post a news                         adm newsdel <id>      delete a news
//   adm radius <m>           «Рядом» radius                      adm reload db|factions|config
//   adm tab <0-3>            sub-tab                             adm f <command>       to INT_PDA_AdminFactions
// After the first test command every admin answer is logged as an ИНФО line «админ-тест: ответ …».

// One row of the players table: a pool of ROWS_VIS rows is built once and refilled while scrolling / paging.
class INT_PDA_PageAdminRowW
{
	Widget m_Frame;
	ImageWidget m_Bg;
	ImageWidget m_Bar;
	ImageWidget m_Dot;
	TextWidget m_Callsign;
	TextWidget m_Game;
	TextWidget m_Steam;
	TextWidget m_Faction;
	TextWidget m_Role;
	TextWidget m_Rank;
	TextWidget m_Seen;
	ButtonWidget m_Hit;
	int m_Index = -1;
	string m_SteamId;
}

// One row of the news feed.
class INT_PDA_PageAdminNewsW
{
	Widget m_Frame;
	ImageWidget m_Bg;
	TextWidget m_Title;
	TextWidget m_Time;
	TextWidget m_Text;
	ImageWidget m_Trash;
	ButtonWidget m_Hit;
	ButtonWidget m_TrashHit;
	int m_Id = -1;
	string m_Head;
}

// Short confirm over the page body (title, one line of text, «Отмена» / action). The result goes to m_OnDone(bool ok).
class INT_PDA_PageAdminConfirm : ScriptedWidgetEventHandler
{
	static const float BOX_W = 620;
	static const float BOX_H = 240;

	Widget m_Frame;
	protected ButtonWidget m_Block;
	protected Widget m_BoxW;
	protected float m_BX;
	protected float m_BY;
	protected ImageWidget m_Accent;
	protected TextWidget m_TitleW;
	protected MultilineTextWidget m_TextW;
	protected INT_UIF_Button m_OkBtn;
	protected INT_UIF_Button m_CancelBtn;
	protected bool m_Open;
	ref ScriptInvoker m_OnDone = new ScriptInvoker();

	void Build(INT_UIF_Bag bag, Widget parent, float w, float h)
	{
		m_Frame = INT_UIF_Draw.Frame(parent, 0, 0, w, h, "INT_PDA_AdmConfirm");
		INT_UIF_Draw.Image(m_Frame, 0, 0, w, h, "iuf_white", ARGB(175, 0, 0, 0));
		m_Block = INT_UIF_Draw.Hit(m_Frame, 0, 0, w, h, this);
		m_BX = (w - BOX_W) * 0.5;
		m_BY = (h - BOX_H) * 0.5;
		m_BoxW = INT_UIF_Draw.Frame(m_Frame, m_BX, m_BY, BOX_W, BOX_H, "INT_PDA_AdmConfirmBox");
		INT_UIF_Surface.Shadow(m_BoxW, -24, -16, BOX_W + 48, BOX_H + 44, ARGB(160, 0, 0, 0));
		INT_UIF_Surface.Box(m_BoxW, 0, 0, BOX_W, BOX_H, 8, INT_PDA_Look.PANEL, INT_PDA_Look.LINE2);
		m_Accent = INT_UIF_Draw.Line(m_BoxW, 0, 0, BOX_W, 3, INT_PDA_Look.ORANGE);
		INT_PDA_Look.DrawIcon(m_BoxW, 28, 26, 20, "warning", INT_PDA_Look.ADMIN);
		m_TitleW = INT_UIF_Draw.Text(m_BoxW, 58, 22, BOX_W - 86, 28, "", 20, INT_PDA_Look.TEXT);
		m_TextW = INT_PDA_Look.MText(m_BoxW, 28, 66, BOX_W - 56, 84, "", 16, INT_PDA_Look.DIM);
		string cancel = INT_PDA_Look.Caps("#STR_INT_PDA_CANCEL");
		m_CancelBtn = INT_UIF_Button.Create(bag, m_BoxW, BOX_W - 28 - 360, BOX_H - 68, 160, 44, cancel, INT_UIF_Variant.SECONDARY);
		m_OkBtn = INT_UIF_Button.Create(bag, m_BoxW, BOX_W - 28 - 190, BOX_H - 68, 190, 44, "", INT_UIF_Variant.PRIMARY, "check");
		if (m_CancelBtn)
			m_CancelBtn.OnClick().Insert(OnCancelClick);
		if (m_OkBtn)
			m_OkBtn.OnClick().Insert(OnOkClick);
		m_Frame.Show(false);
	}

	void Open(string title, string text, string okText, bool danger)
	{
		if (!m_Frame)
			return;
		m_TitleW.SetText(title);
		m_TextW.SetText(text);
		int accent = INT_PDA_Look.ORANGE;
		if (danger)
			accent = INT_PDA_Look.ENEMY;
		m_Accent.SetColor(accent);
		if (m_OkBtn)
		{
			m_OkBtn.SetText(okText);
			if (danger)
				m_OkBtn.SetVariant(INT_UIF_Variant.DANGER);
			else
				m_OkBtn.SetVariant(INT_UIF_Variant.PRIMARY);
		}
		m_Open = true;
		m_Frame.Show(true);
		INT_UIF_Animator.Get().Cancel(m_BoxW);
		INT_UIF_Draw.Move(m_BoxW, m_BX, m_BY);
		INT_UIF_Animator.Reveal(m_BoxW, 0, 12, 0.22);
		INT_PDA_Sfx.Play("Click");
	}

	bool IsOpen()
	{
		return m_Open;
	}

	// Hide without an answer (the page was hidden).
	void Close()
	{
		m_Open = false;
		if (m_Frame)
			m_Frame.Show(false);
	}

	void Finish(bool ok)
	{
		if (!m_Open)
			return;
		Close();
		INT_PDA_Sfx.Play("Click");
		m_OnDone.Invoke(ok);
	}

	protected void OnOkClick(INT_UIF_Button b)
	{
		Finish(true);
	}

	protected void OnCancelClick(INT_UIF_Button b)
	{
		Finish(false);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		// the veil swallows clicks: nothing under the confirm reacts while it is open
		if (w == m_Block)
			return true;
		return false;
	}
}

class INT_PDA_PageAdmin : INT_PDA_Page
{
	static const int SUB_PLAYERS = 0;
	static const int SUB_FACTIONS = 1;
	static const int SUB_NEWS = 2;
	static const int SUB_SYSTEM = 3;

	static const float BAR_H = 60;			// sub-tab bar
	static const float AREA_Y = 76;			// top of the sub-tab area (its size goes to INT_PDA_AdminFactions)
	static const int ROWS_VIS = 10;
	static const float ROW_H = 44;
	static const int NEWS_VIS = 5;			// + the «Строка новостей КПК» settings strip under the feed
	static const float NEWS_H = 84;
	static const int NEWS_TITLE_MAX = 48;	// server: CleanLine(title, 48, ...)
	static const int NEWS_TEXT_MAX = 300;	// server: CleanLine(text, 300, ...)
	static const int GAP_MARGIN_MS = 100;	// on top of INT_PDA_Const.REQUEST_GAP_MS (network jitter)
	static const int REPLY_TIMEOUT_MS = 3000;
	static const int QUEUE_MAX = 4;
	static const int DEFAULT_PAGE = 50;		// server default AdminPageSize (learnt from every page that is not the last)
	static const int RETRY_MAX = 2;			// resends of a request the server's gate answered «too fast»
	static const int RETRY_HOLD_MS = 1000;
	static const int POST_BUSY_MS = 10000;	// «Опубликовать» unlocks by itself after this (answer lost)
	static const int KIND_NONE = 0;
	static const int KIND_ACTION = 1;
	static const int KIND_PLAYERS = 2;

	// confirm kinds
	static const int CF_NONE = 0;
	static const int CF_DELETE = 1;
	static const int CF_KICK = 2;
	static const int CF_LEADER = 3;
	static const int CF_NEWS_DEL = 4;
	static const int CF_RELOAD_DB = 5;
	static const int CF_RELOAD_FACTIONS = 6;
	static const int CF_RELOAD_CONFIG = 7;

	// players table columns (x, width) inside the 940 px table
	static const float C_DOT = 14;
	static const float C_CS = 34;
	static const float W_CS = 140;
	static const float C_GAME = 182;
	static const float W_GAME = 100;
	static const float C_STEAM = 290;
	static const float W_STEAM = 172;			// the full 17 digits (admins copy them)
	static const float C_FAC = 470;
	static const float W_FAC = 124;
	static const float C_ROLE = 602;
	static const float W_ROLE = 96;
	static const float C_RANK = 706;
	static const float W_RANK = 96;
	static const float C_SEEN = 810;
	static const float W_SEEN = 118;
	static const float CARD_VAL_W = 238;

	// sub-tab bar
	protected Widget m_Bar;
	protected ref array<TextWidget> m_SubText = new array<TextWidget>();
	protected ref array<ButtonWidget> m_SubHits = new array<ButtonWidget>();
	protected ref array<float> m_SubX = new array<float>();
	protected ref array<float> m_SubW = new array<float>();
	protected ImageWidget m_SubLine;
	protected TextWidget m_RefreshText;
	protected ImageWidget m_RefreshIcon;
	protected ButtonWidget m_RefreshHit;
	protected int m_Sub;
	protected Widget m_Area;
	protected Widget m_NoRights;
	// players
	protected Widget m_PlayersBox;
	protected INT_UIF_TextInput m_Search;
	protected INT_UIF_Button m_FindBtn;
	protected INT_UIF_Button m_AllBtn;
	protected TextWidget m_FoundText;
	protected Widget m_Table;
	protected ButtonWidget m_TableHit;
	protected TextWidget m_TableEmpty;
	protected ImageWidget m_ThumbTrack;
	protected ImageWidget m_Thumb;
	protected ref array<ref INT_PDA_PageAdminRowW> m_PRows = new array<ref INT_PDA_PageAdminRowW>();
	protected INT_UIF_IconButton m_PrevBtn;
	protected INT_UIF_IconButton m_NextBtn;
	protected TextWidget m_PageText;
	protected TextWidget m_RangeText;
	// stalker card
	protected Widget m_DetBody;
	protected Widget m_DetEmpty;
	protected ImageWidget m_DetAvatar;
	protected TextWidget m_DetCallsign;
	protected ImageWidget m_DetDot;
	protected TextWidget m_DetOnline;
	protected TextWidget m_DetSub;
	protected ref array<TextWidget> m_DetVal = new array<TextWidget>();
	protected ImageWidget m_EyeIcon;
	protected TextWidget m_EyeText;
	protected ButtonWidget m_EyeHit;
	protected INT_UIF_Dropdown m_RoleDrop;
	protected INT_UIF_Button m_KickBtn;
	protected INT_UIF_Button m_PassBtn;
	protected INT_UIF_Button m_DelBtn;
	// players state
	protected ref INT_PDA_AdminRow m_Sel;		// the row object of the reply it came from (rows are never mutated)
	protected bool m_ShowPass;
	protected bool m_HasData;
	protected string m_Filter;
	protected int m_Offset;
	protected int m_ReqOffset;
	protected int m_PageSize = 50;
	protected int m_First;
	protected int m_HoverRow = -1;
	protected string m_AutoSelect;
	protected string m_PromptSteam;
	// news
	protected Widget m_NewsBox;
	protected TextWidget m_NewsCount;
	protected ButtonWidget m_NewsAllHit;
	protected TextWidget m_NewsEmpty;
	protected ImageWidget m_NewsTrack;
	protected ImageWidget m_NewsThumb;
	protected ref array<ref INT_PDA_PageAdminNewsW> m_NRows = new array<ref INT_PDA_PageAdminNewsW>();
	protected int m_NewsFirst;
	protected int m_NewsHover = -1;
	protected int m_TrashHover = -1;
	protected INT_UIF_TextInput m_NewsTitleIn;
	protected INT_UIF_TextInput m_NewsTextIn;
	protected TextWidget m_TitleCount;
	protected TextWidget m_TextCount;
	protected TextWidget m_PrevTitle;
	protected MultilineTextWidget m_PrevText;
	protected INT_UIF_Button m_PostBtn;
	// news lifetime / edit (owner 2026-10-23) and the news line settings
	protected INT_UIF_TextInput m_NewsMinIn;
	protected int m_EditNewsId = -1;
	protected TextWidget m_EditTag;
	protected INT_UIF_Button m_EditCancel;
	protected INT_UIF_TextInput m_RotIn;
	protected INT_UIF_TextInput m_DefMinIn;
	protected INT_UIF_Button m_NewsSetBtn;
	// system
	protected Widget m_SysBox;
	protected INT_UIF_TextInput m_RadiusIn;
	protected INT_UIF_Button m_RadiusBtn;
	protected TextWidget m_RadiusNow;
	protected INT_UIF_Button m_RlDbBtn;
	protected INT_UIF_Button m_RlFacBtn;
	protected INT_UIF_Button m_RlCfgBtn;
	protected ref array<TextWidget> m_InfoVal = new array<TextWidget>();
	// «Группировки» (INT_PDA_PageAdminFactions.c)
	protected Widget m_FactionsHost;
	protected ref INT_PDA_AdminFactions m_Factions;
	// confirm
	protected ref INT_PDA_PageAdminConfirm m_Confirm;
	protected int m_CfKind;
	protected string m_CfA;
	protected string m_CfB;
	protected int m_CfN;
	// network: one queue, one gap
	protected ref array<ref ScriptRPC> m_Queue = new array<ref ScriptRPC>();
	protected ref array<int> m_QueueSub = new array<int>();
	protected int m_LastSendMs = -100000;
	protected bool m_WantPlayers;
	protected bool m_WantNews;
	protected bool m_PlayersWaiting;
	protected int m_PlayersSentMs;
	protected int m_Retries;
	protected bool m_ActionWaiting;
	protected int m_ActionSub;
	protected int m_ActionSentMs;
	protected bool m_TestMode;
	// «too fast» (the server's request gate answers OP_RESULT, not OP_ADMIN_DATA): the dropped request goes out again
	protected ref array<int> m_QueueTry = new array<int>();	// retries already spent per queued request
	protected ref ScriptRPC m_LastActRpc;		// the action on its way (ScriptRPC.Send keeps its data: resendable)
	protected int m_ActTry;
	protected int m_LastKind;					// what this page sent last over OP_ADMIN (KIND_*)
	protected int m_HoldUntilMs;				// no request before this (after a «too fast»)
	protected bool m_PostBusy;					// «Опубликовать» waits for its answer (no second news by a double click)
	protected int m_PostBusyMs;
	protected bool m_RadiusDirty;				// the radius field holds an unsaved value

	// =================================================================================================================
	// build
	// =================================================================================================================
	override void OnBuild()
	{
		float ah = m_H - AREA_Y - 18;		// breathing room above the news line (concept E)
		BuildBar();
		m_Area = INT_UIF_Draw.Frame(m_Root, 0, AREA_Y, m_W, ah, "INT_PDA_AdmArea");
		BuildPlayers(ah);
		BuildNews(ah);
		BuildSystem(ah);
		m_FactionsHost = INT_UIF_Draw.Frame(m_Area, 0, 0, m_W, ah, "INT_PDA_AdmFactionsHost");
		m_Factions = new INT_PDA_AdminFactions();
		m_Factions.Build(this, m_FactionsHost, m_W, ah);
		m_Factions.Show(false);
		m_FactionsHost.Show(false);
		m_NewsBox.Show(false);
		m_SysBox.Show(false);
		m_PlayersBox.Show(true);
		m_NoRights = Locked(m_Root, m_W, m_H, "admin", "#STR_INT_PDA_ADMIN_NO_RIGHTS_T", "#STR_INT_PDA_ADMIN_NO_RIGHTS");
		m_NoRights.Show(false);
		// the confirm is built last: it lies over everything of the page
		m_Confirm = new INT_PDA_PageAdminConfirm();
		m_Confirm.Build(m_Bag, m_Root, m_W, m_H);
		m_Confirm.m_OnDone.Insert(OnConfirmDone);
		PaintSubTabs(false);
		RenderPlayers();
		RenderDetail();
	}

	protected void BuildBar()
	{
		m_Bar = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, BAR_H, "INT_PDA_AdmBar");
		INT_UIF_Draw.Line(m_Bar, 40, BAR_H - 1, m_W - 80, 1, INT_PDA_Look.LINE);
		array<string> keys = {"#STR_INT_PDA_ADMIN_SUB_PLAYERS", "#STR_INT_PDA_ADMIN_SUB_FACTIONS", "#STR_INT_PDA_ADMIN_SUB_NEWS", "#STR_INT_PDA_ADMIN_SUB_SYSTEM"};
		float x = 40;
		float tw;
		string cap;
		TextWidget t;
		ButtonWidget hit;
		int i;
		for (i = 0; i < keys.Count(); i++)
		{
			cap = INT_PDA_Look.Caps(keys[i]);
			tw = INT_PDA_Look.TextW(cap, 20) + 6;
			t = INT_UIF_Draw.Text(m_Bar, x, 0, tw, BAR_H - 6, cap, 20, INT_PDA_Look.DIM);
			hit = Hit(m_Bar, x - 8, 0, tw + 16, BAR_H);
			m_SubText.Insert(t);
			m_SubHits.Insert(hit);
			m_SubX.Insert(x);
			m_SubW.Insert(tw);
			x = x + tw + 36;
		}
		m_SubLine = INT_UIF_Draw.Line(m_Bar, 40, BAR_H - 4, 10, 3, INT_PDA_Look.ORANGE);
		cap = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_REFRESH");
		tw = INT_PDA_Look.TextW(cap, 18) + 6;
		float rx = m_W - 40 - tw;
		m_RefreshText = INT_UIF_Draw.Text(m_Bar, rx, 0, tw, BAR_H - 6, cap, 18, INT_PDA_Look.DIM);
		m_RefreshIcon = INT_PDA_Look.DrawIcon(m_Bar, rx - 30, 17, 20, "refresh", INT_PDA_Look.DIM);
		m_RefreshHit = Hit(m_Bar, rx - 38, 0, tw + 46, BAR_H);
	}

	// A plate of style E with an admin caption (shield + caps in the admin accent).
	protected Widget AdmPanel(Widget parent, float x, float y, float w, float h, string captionKey)
	{
		Widget f = Panel(parent, x, y, w, h, "");
		INT_PDA_Look.DrawIcon(f, 18, 13, 15, "admin", INT_PDA_Look.ADMIN);
		string cap = INT_PDA_Look.Caps(captionKey);
		INT_UIF_Draw.Text(f, 40, 12, w - 58, 18, cap, 13, INT_PDA_Look.ADMIN);
		return f;
	}

	protected void TableHead(float x, float w, string key)
	{
		string cap = INT_PDA_Look.Caps(key);
		INT_UIF_Draw.Text(m_Table, x, 0, w, 40, cap, 13, INT_PDA_Look.ADMIN);
	}

	protected TextWidget RowText(Widget parent, float x, float w, float size, int color)
	{
		return INT_UIF_Draw.Text(parent, x, 0, w, ROW_H, "", size, color);
	}

	protected void BuildPlayers(float ah)
	{
		m_PlayersBox = INT_UIF_Draw.Frame(m_Area, 0, 0, m_W, ah, "INT_PDA_AdmPlayers");
		// search row
		m_Search = INT_UIF_TextInput.Create(m_Bag, m_PlayersBox, 40, 0, 470, 44, "#STR_INT_PDA_ADMIN_PL_SEARCH_HINT", 24, "search");
		if (m_Search)
			m_Search.OnSubmit().Insert(OnSearchSubmit);
		string cap = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_PL_FIND");
		m_FindBtn = INT_UIF_Button.Create(m_Bag, m_PlayersBox, 520, 0, 130, 44, cap, INT_UIF_Variant.PRIMARY, "search");
		if (m_FindBtn)
			m_FindBtn.OnClick().Insert(OnFindClick);
		cap = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_PL_ALL");
		m_AllBtn = INT_UIF_Button.Create(m_Bag, m_PlayersBox, 660, 0, 110, 44, cap, INT_UIF_Variant.SECONDARY);
		if (m_AllBtn)
			m_AllBtn.OnClick().Insert(OnAllClick);
		m_FoundText = INT_UIF_Draw.Text(m_PlayersBox, 780, 0, 200, 44, "", 13, INT_PDA_Look.DIM, INT_UIF_Align.RIGHT);
		// table
		m_Table = INT_UIF_Draw.Frame(m_PlayersBox, 40, 58, 940, 484, "INT_PDA_AdmTable");
		INT_UIF_Surface.Box(m_Table, 0, 0, 940, 484, 6, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
		TableHead(C_CS, W_CS, "#STR_INT_PDA_F_CALLSIGN");
		TableHead(C_GAME, W_GAME, "#STR_INT_PDA_ADMIN_PL_COL_GAME");
		TableHead(C_STEAM, W_STEAM, "#STR_INT_PDA_ADMIN_PL_COL_STEAM");
		TableHead(C_FAC, W_FAC, "#STR_INT_PDA_STAT_FACTION");
		TableHead(C_ROLE, W_ROLE, "#STR_INT_PDA_ADMIN_PL_COL_ROLE");
		TableHead(C_RANK, W_RANK, "#STR_INT_PDA_ADMIN_PL_COL_RANK");
		TableHead(C_SEEN, W_SEEN, "#STR_INT_PDA_ADMIN_PL_COL_SEEN");
		INT_UIF_Draw.Line(m_Table, 0, 40, 940, 1, INT_PDA_Look.LINE);
		m_TableHit = Hit(m_Table, 0, 41, 940, 443);
		int i;
		float ry;
		INT_PDA_PageAdminRowW rw;
		for (i = 0; i < ROWS_VIS; i++)
		{
			ry = 41 + i * ROW_H;
			rw = new INT_PDA_PageAdminRowW();
			rw.m_Frame = INT_UIF_Draw.Frame(m_Table, 1, ry, 938, ROW_H, "INT_PDA_AdmRow");
			rw.m_Bg = INT_UIF_Draw.Image(rw.m_Frame, 0, 0, 938, ROW_H - 1, "iuf_white", 0x00000000);
			INT_UIF_Draw.Line(rw.m_Frame, 12, ROW_H - 1, 914, 1, INT_PDA_Look.LINE);
			rw.m_Bar = INT_UIF_Draw.Line(rw.m_Frame, 0, 0, 3, ROW_H - 1, INT_PDA_Look.ORANGE);
			rw.m_Dot = INT_UIF_Draw.Image(rw.m_Frame, C_DOT, (ROW_H - 10) * 0.5, 10, 10, "iuf_circle16", INT_PDA_Look.DIM2);
			rw.m_Callsign = RowText(rw.m_Frame, C_CS, W_CS, 16, INT_PDA_Look.TEXT);
			rw.m_Game = RowText(rw.m_Frame, C_GAME, W_GAME, 15, INT_PDA_Look.DIM);
			rw.m_Steam = RowText(rw.m_Frame, C_STEAM, W_STEAM, 13, INT_PDA_Look.DIM);
			rw.m_Faction = RowText(rw.m_Frame, C_FAC, W_FAC, 15, INT_PDA_Look.TEXT);
			rw.m_Role = RowText(rw.m_Frame, C_ROLE, W_ROLE, 15, INT_PDA_Look.DIM);
			rw.m_Rank = RowText(rw.m_Frame, C_RANK, W_RANK, 15, INT_PDA_Look.DIM);
			rw.m_Seen = RowText(rw.m_Frame, C_SEEN, W_SEEN, 13, INT_PDA_Look.DIM);
			rw.m_Hit = Hit(rw.m_Frame, 0, 0, 938, ROW_H);
			rw.m_Bar.Show(false);
			rw.m_Frame.Show(false);
			m_PRows.Insert(rw);
		}
		m_TableEmpty = INT_UIF_Draw.Text(m_Table, 0, 230, 940, 30, "#STR_INT_PDA_ADMIN_PL_EMPTY", 17, INT_PDA_Look.DIM2, INT_UIF_Align.CENTER);
		m_TableEmpty.Show(false);
		m_ThumbTrack = INT_UIF_Draw.Line(m_Table, 932, 46, 3, 432, ARGB(14, 255, 255, 255));
		m_Thumb = INT_UIF_Draw.Line(m_Table, 932, 46, 3, 40, ARGB(90, 255, 255, 255));
		// pager
		m_PrevBtn = INT_UIF_IconButton.Create(m_Bag, m_PlayersBox, 40, 552, 40, "chevron_left", INT_UIF_Variant.SECONDARY, "");
		if (m_PrevBtn)
			m_PrevBtn.OnClick().Insert(OnPrevClick);
		m_PageText = INT_UIF_Draw.Text(m_PlayersBox, 88, 552, 180, 40, "", 15, INT_PDA_Look.TEXT, INT_UIF_Align.CENTER);
		m_NextBtn = INT_UIF_IconButton.Create(m_Bag, m_PlayersBox, 276, 552, 40, "chevron_right", INT_UIF_Variant.SECONDARY, "");
		if (m_NextBtn)
			m_NextBtn.OnClick().Insert(OnNextClick);
		INT_UIF_Draw.Text(m_PlayersBox, 334, 552, 300, 40, "#STR_INT_PDA_ADMIN_PL_WHEEL", 13, INT_PDA_Look.DIM2);
		m_RangeText = INT_UIF_Draw.Text(m_PlayersBox, 640, 552, 340, 40, "", 14, INT_PDA_Look.DIM, INT_UIF_Align.RIGHT);
		BuildCard();
	}

	protected void BuildCard()
	{
		Widget card = AdmPanel(m_PlayersBox, 1000, 0, 416, 592, "#STR_INT_PDA_ADMIN_PL_CARD");
		m_DetEmpty = INT_UIF_Draw.Frame(card, 0, 40, 416, 552, "INT_PDA_AdmCardEmpty");
		INT_PDA_Look.DrawIcon(m_DetEmpty, 176, 180, 64, "user", INT_PDA_Look.DIM2);
		INT_UIF_Draw.Text(m_DetEmpty, 20, 262, 376, 26, "#STR_INT_PDA_ADMIN_PL_PICK", 16, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
		m_DetBody = INT_UIF_Draw.Frame(card, 0, 0, 416, 592, "INT_PDA_AdmCardBody");
		Widget av = INT_UIF_Draw.Frame(m_DetBody, 18, 44, 84, 84, "INT_PDA_AdmAvatar");
		INT_UIF_Surface.Box(av, 0, 0, 84, 84, 6, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE2);
		m_DetAvatar = INT_PDA_Look.DrawAvatar(av, 3, 3, 78, 0);
		INT_UIF_Draw.Line(av, 3, 78, 78, 3, INT_PDA_Look.ORANGE);
		m_DetCallsign = INT_UIF_Draw.Text(m_DetBody, 118, 40, 284, 40, "", 30, INT_PDA_Look.TEXT);
		m_DetDot = INT_UIF_Draw.Image(m_DetBody, 120, 89, 10, 10, "iuf_circle16", INT_PDA_Look.DIM2);
		m_DetOnline = INT_UIF_Draw.Text(m_DetBody, 138, 82, 264, 24, "", 14, INT_PDA_Look.DIM);
		m_DetSub = INT_UIF_Draw.Text(m_DetBody, 118, 106, 284, 22, "", 14, INT_PDA_Look.DIM);
		INT_UIF_Draw.Line(m_DetBody, 18, 142, 380, 1, INT_PDA_Look.LINE);
		// 0 steam, 1 game name, 2 discord, 3 registered, 4 last seen, 5 faction, 6 role, 7 rank, 8 password
		array<string> keys = {"#STR_INT_PDA_ADMIN_PL_COL_STEAM", "#STR_INT_PDA_ADMIN_PL_COL_GAME", "#STR_INT_PDA_F_DISCORD", "#STR_INT_PDA_ADMIN_PL_REGISTERED", "#STR_INT_PDA_ADMIN_PL_COL_SEEN", "#STR_INT_PDA_STAT_FACTION", "#STR_INT_PDA_ADMIN_PL_COL_ROLE", "#STR_INT_PDA_ADMIN_PL_COL_RANK", "#STR_INT_PDA_F_PASSWORD"};
		float fy;
		int i;
		string cap;
		float lw;
		TextWidget val;
		for (i = 0; i < keys.Count(); i++)
		{
			fy = 150 + i * 30;
			cap = INT_PDA_Look.Caps(keys[i]);
			lw = 140;
			if (i == 8)
				lw = 50;
			INT_UIF_Draw.Text(m_DetBody, 18, fy, lw, 30, cap, 13, INT_PDA_Look.DIM);
			val = INT_UIF_Draw.Text(m_DetBody, 160, fy, CARD_VAL_W, 30, "", 15, INT_PDA_Look.TEXT);
			m_DetVal.Insert(val);
		}
		// the eye toggle sits in the label column of the password row: the value keeps its full width
		fy = 150 + 8 * 30;
		m_EyeIcon = INT_PDA_Look.DrawIcon(m_DetBody, 70, fy + 5, 20, "eye", INT_PDA_Look.DIM);
		m_EyeText = INT_UIF_Draw.Text(m_DetBody, 94, fy, 64, 30, "", 12, INT_PDA_Look.DIM);
		m_EyeHit = Hit(m_DetBody, 18, fy, 140, 30);
		INT_UIF_Draw.Line(m_DetBody, 18, 424, 380, 1, INT_PDA_Look.LINE);
		cap = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_PL_ROLE_IN_GP");
		INT_UIF_Draw.Text(m_DetBody, 18, 434, 380, 18, cap, 13, INT_PDA_Look.ADMIN);
		// options = INT_PDA_Const.ROLE_* (0 none = expel, 1 member, 2 deputy, 3 leader)
		array<string> roles = new array<string>();
		string opt = Widget.TranslateString("#STR_INT_PDA_ADMIN_PL_ROLE_NONE");
		roles.Insert(opt);
		opt = Widget.TranslateString("#STR_INT_PDA_ROLE_MEMBER");
		roles.Insert(opt);
		opt = Widget.TranslateString("#STR_INT_PDA_ROLE_DEPUTY");
		roles.Insert(opt);
		opt = Widget.TranslateString("#STR_INT_PDA_ROLE_LEADER");
		roles.Insert(opt);
		m_RoleDrop = INT_UIF_Dropdown.Create(m_Bag, m_DetBody, 18, 458, 190, roles, 0, "");
		if (m_RoleDrop)
			m_RoleDrop.OnSelected().Insert(OnRolePicked);
		cap = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_PL_KICK");
		m_KickBtn = INT_UIF_Button.Create(m_Bag, m_DetBody, 218, 458, 180, 40, cap, INT_UIF_Variant.WARNING, "user_minus");
		if (m_KickBtn)
			m_KickBtn.OnClick().Insert(OnKickClick);
		cap = INT_PDA_Look.Caps("#STR_INT_PDA_BTN_PASSWORD");
		m_PassBtn = INT_UIF_Button.Create(m_Bag, m_DetBody, 18, 514, 190, 44, cap, INT_UIF_Variant.SECONDARY, "key");
		if (m_PassBtn)
			m_PassBtn.OnClick().Insert(OnPassClick);
		cap = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_PL_DELETE");
		m_DelBtn = INT_UIF_Button.Create(m_Bag, m_DetBody, 218, 514, 180, 44, cap, INT_UIF_Variant.DANGER, "trash");
		if (m_DelBtn)
			m_DelBtn.OnClick().Insert(OnDeleteClick);
	}

	protected void BuildNews(float ah)
	{
		m_NewsBox = INT_UIF_Draw.Frame(m_Area, 0, 0, m_W, ah, "INT_PDA_AdmNews");
		// feed
		Widget feed = AdmPanel(m_NewsBox, 40, 0, 900, 592, "#STR_INT_PDA_ADMIN_NW_FEED");
		m_NewsCount = INT_UIF_Draw.Text(feed, 700, 12, 182, 18, "", 13, INT_PDA_Look.ADMIN, INT_UIF_Align.RIGHT);
		m_NewsAllHit = Hit(feed, 0, 40, 900, NEWS_VIS * NEWS_H);
		int i;
		float ry;
		INT_PDA_PageAdminNewsW nw;
		for (i = 0; i < NEWS_VIS; i++)
		{
			ry = 40 + i * NEWS_H;
			nw = new INT_PDA_PageAdminNewsW();
			nw.m_Frame = INT_UIF_Draw.Frame(feed, 1, ry, 898, NEWS_H, "INT_PDA_AdmNewsRow");
			nw.m_Bg = INT_UIF_Draw.Image(nw.m_Frame, 0, 0, 898, NEWS_H - 1, "iuf_white", 0x00000000);
			INT_UIF_Draw.Line(nw.m_Frame, 18, NEWS_H - 1, 862, 1, INT_PDA_Look.LINE);
			nw.m_Title = INT_UIF_Draw.Text(nw.m_Frame, 24, 12, 490, 28, "", 19, INT_PDA_Look.TEXT);
			nw.m_Time = INT_UIF_Draw.Text(nw.m_Frame, 520, 14, 306, 20, "", 13, INT_PDA_Look.DIM, INT_UIF_Align.RIGHT);
			nw.m_Text = INT_UIF_Draw.Text(nw.m_Frame, 24, 46, 800, 26, "", 15, INT_PDA_Look.DIM);
			nw.m_Hit = Hit(nw.m_Frame, 0, 0, 898, NEWS_H);
			nw.m_Trash = INT_PDA_Look.DrawIcon(nw.m_Frame, 850, (NEWS_H - 22) * 0.5, 22, "trash", INT_PDA_Look.DIM2);
			nw.m_TrashHit = Hit(nw.m_Frame, 838, (NEWS_H - 46) * 0.5, 46, 46);
			nw.m_Frame.Show(false);
			m_NRows.Insert(nw);
		}
		m_NewsEmpty = INT_UIF_Draw.Text(feed, 0, 230, 900, 30, "#STR_INT_PDA_NEWS_NONE", 18, INT_PDA_Look.DIM2, INT_UIF_Align.CENTER);
		m_NewsTrack = INT_UIF_Draw.Line(feed, 890, 46, 3, NEWS_VIS * NEWS_H - 12, ARGB(14, 255, 255, 255));
		m_NewsThumb = INT_UIF_Draw.Line(feed, 890, 46, 3, 40, ARGB(90, 255, 255, 255));
		// «Строка новостей КПК»: rotation of the footer line and the lifetime a new item gets by default
		float sy = 40 + NEWS_VIS * NEWS_H + 8;
		INT_UIF_Draw.Line(feed, 18, sy, 862, 1, INT_PDA_Look.LINE2);
		INT_UIF_Draw.Text(feed, 24, sy + 10, 600, 18, INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_NW_SETTINGS"), 13, INT_PDA_Look.ADMIN);
		Label(feed, 24, sy + 34, 220, "#STR_INT_PDA_ADMIN_NW_ROTATE");
		m_RotIn = INT_UIF_TextInput.Create(m_Bag, feed, 24, sy + 56, 120, 44, "", 3);
		Label(feed, 270, sy + 34, 300, "#STR_INT_PDA_ADMIN_NW_DEFMIN");
		m_DefMinIn = INT_UIF_TextInput.Create(m_Bag, feed, 270, sy + 56, 140, 44, "", 6);
		string setCap = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_SYS_SAVE");
		m_NewsSetBtn = INT_UIF_Button.Create(m_Bag, feed, 680, sy + 56, 196, 44, setCap, INT_UIF_Variant.PRIMARY, "check");
		if (m_NewsSetBtn)
			m_NewsSetBtn.OnClick().Insert(OnNewsSettingsClick);
		// «Новая новость»
		Widget form = AdmPanel(m_NewsBox, 960, 0, 456, 592, "#STR_INT_PDA_ADMIN_NW_NEW");
		Label(form, 18, 44, 280, "#STR_INT_PDA_ADMIN_NW_TITLE");
		m_TitleCount = INT_UIF_Draw.Text(form, 298, 44, 140, 18, "", 13, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		m_NewsTitleIn = INT_UIF_TextInput.Create(m_Bag, form, 18, 66, 420, 44, "", NEWS_TITLE_MAX);
		Label(form, 18, 124, 280, "#STR_INT_PDA_ADMIN_NW_TEXT");
		m_TextCount = INT_UIF_Draw.Text(form, 298, 124, 140, 18, "", 13, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		// UIF has no multi-line input: one long line, the preview below shows the wrapped text
		m_NewsTextIn = INT_UIF_TextInput.Create(m_Bag, form, 18, 146, 420, 44, "", NEWS_TEXT_MAX);
		if (m_NewsTitleIn)
		{
			m_NewsTitleIn.OnChanged().Insert(OnNewsTyped);
			m_NewsTitleIn.OnSubmit().Insert(OnNewsTitleSubmit);
		}
		if (m_NewsTextIn)
			m_NewsTextIn.OnChanged().Insert(OnNewsTyped);
		// lifetime: minutes from now, 0 = until deleted; while an item is edited the tag + «Отменить правку» show
		Label(form, 18, 204, 200, "#STR_INT_PDA_ADMIN_NW_MIN");
		INT_UIF_Draw.Text(form, 200, 204, 238, 18, "#STR_INT_PDA_ADMIN_NW_MIN_SUB", 13, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		m_NewsMinIn = INT_UIF_TextInput.Create(m_Bag, form, 18, 226, 130, 44, "", 6);
		m_EditTag = INT_UIF_Draw.Text(form, 160, 226, 110, 44, "", 13, INT_PDA_Look.ORANGE);
		string cancelCap = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_NW_CANCEL");
		m_EditCancel = INT_UIF_Button.Create(m_Bag, form, 270, 226, 168, 44, cancelCap, INT_UIF_Variant.GHOST, "close");
		if (m_EditCancel)
		{
			m_EditCancel.OnClick().Insert(OnEditCancelClick);
			m_EditCancel.Show(false);
		}
		Label(form, 18, 284, 280, "#STR_INT_PDA_ADMIN_NW_PREVIEW");
		Widget prev = INT_UIF_Draw.Frame(form, 18, 306, 420, 158, "INT_PDA_AdmNewsPreview");
		INT_UIF_Surface.Box(prev, 0, 0, 420, 158, 4, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE);
		INT_UIF_Draw.Line(prev, 0, 0, 3, 158, INT_PDA_Look.ORANGE);
		m_PrevTitle = INT_UIF_Draw.Text(prev, 18, 10, 388, 28, "", 18, INT_PDA_Look.TEXT);
		m_PrevText = INT_PDA_Look.MText(prev, 18, 42, 388, 108, "", 15, INT_PDA_Look.DIM);
		INT_PDA_Look.DrawIcon(form, 18, 480, 16, "news", INT_PDA_Look.DIM2);
		INT_PDA_Look.MText(form, 42, 476, 396, 40, "#STR_INT_PDA_ADMIN_NW_NOTE", 13, INT_PDA_Look.DIM2);
		string cap = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_NW_POST");
		m_PostBtn = INT_UIF_Button.Create(m_Bag, form, 18, 526, 420, 48, cap, INT_UIF_Variant.PRIMARY, "check");
		if (m_PostBtn)
			m_PostBtn.OnClick().Insert(OnPostClick);
	}

	protected void BuildSystem(float ah)
	{
		m_SysBox = INT_UIF_Draw.Frame(m_Area, 0, 0, m_W, ah, "INT_PDA_AdmSystem");
		// settings: the «Рядом» radius
		Widget sp = AdmPanel(m_SysBox, 40, 0, 660, 236, "#STR_INT_PDA_ADMIN_SYS_SETTINGS");
		string cap = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_SYS_RADIUS");
		INT_UIF_Draw.Text(sp, 24, 46, 612, 30, cap, 22, INT_PDA_Look.TEXT);
		INT_UIF_Draw.Text(sp, 24, 80, 612, 22, "#STR_INT_PDA_ADMIN_SYS_RADIUS_SUB", 14, INT_PDA_Look.DIM);
		m_RadiusIn = INT_UIF_TextInput.Create(m_Bag, sp, 24, 140, 130, 48, "", 5);
		if (m_RadiusIn)
		{
			m_RadiusIn.OnSubmit().Insert(OnRadiusSubmit);
			m_RadiusIn.OnChanged().Insert(OnRadiusTyped);
		}
		INT_UIF_Draw.Text(sp, 164, 140, 40, 48, "#STR_INT_PDA_ADMIN_SYS_M", 18, INT_PDA_Look.DIM);
		cap = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_SYS_SAVE");
		m_RadiusBtn = INT_UIF_Button.Create(m_Bag, sp, 210, 140, 180, 48, cap, INT_UIF_Variant.PRIMARY, "check");
		if (m_RadiusBtn)
			m_RadiusBtn.OnClick().Insert(OnRadiusClick);
		m_RadiusNow = INT_UIF_Draw.Text(sp, 400, 140, 236, 48, "", 15, INT_PDA_Look.DIM, INT_UIF_Align.RIGHT);
		// reloads
		Widget rp = AdmPanel(m_SysBox, 40, 256, 660, 336, "#STR_INT_PDA_ADMIN_SYS_RELOAD");
		m_RlDbBtn = ReloadRow(rp, 0, "users", "#STR_INT_PDA_ADMIN_SYS_RL_DB", "#STR_INT_PDA_ADMIN_SYS_RL_DB_SUB");
		m_RlFacBtn = ReloadRow(rp, 1, "flag", "#STR_INT_PDA_ADMIN_SYS_RL_FACTIONS", "#STR_INT_PDA_ADMIN_SYS_RL_FACTIONS_SUB");
		m_RlCfgBtn = ReloadRow(rp, 2, "layers", "#STR_INT_PDA_ADMIN_SYS_RL_CONFIG", "#STR_INT_PDA_ADMIN_SYS_RL_CONFIG_SUB");
		if (m_RlDbBtn)
			m_RlDbBtn.OnClick().Insert(OnReloadDbClick);
		if (m_RlFacBtn)
			m_RlFacBtn.OnClick().Insert(OnReloadFactionsClick);
		if (m_RlCfgBtn)
			m_RlCfgBtn.OnClick().Insert(OnReloadConfigClick);
		// info: 0 version, 1 protocol, 2 my steam, 3 PDA account, 4 admin mode, 5 admin rights
		Widget ip = AdmPanel(m_SysBox, 720, 0, 696, 592, "#STR_INT_PDA_ADMIN_SUB_SYSTEM");
		array<string> keys = {"#STR_INT_PDA_ADMIN_SYS_VERSION", "#STR_INT_PDA_ADMIN_SYS_PROTOCOL", "#STR_INT_PDA_ADMIN_SYS_MY_STEAM", "#STR_INT_PDA_ADMIN_SYS_ACCOUNT", "#STR_INT_PDA_ADMIN_SYS_MODE", "#STR_INT_PDA_ADMIN_SYS_RIGHTS"};
		int i;
		int col;
		int rowI;
		int barC;
		float cx;
		float cy;
		TextWidget val;
		for (i = 0; i < keys.Count(); i++)
		{
			col = i % 2;
			rowI = i / 2;
			cx = 24 + col * 336;
			cy = 52 + rowI * 116;
			barC = INT_PDA_Look.LINE2;
			if (i == 0)
				barC = INT_PDA_Look.ADMIN;
			INT_UIF_Draw.Line(ip, cx, cy, 2, 80, barC);
			cap = INT_PDA_Look.Caps(keys[i]);
			INT_UIF_Draw.Text(ip, cx + 16, cy, 300, 18, cap, 13, INT_PDA_Look.DIM);
			val = INT_UIF_Draw.Text(ip, cx + 16, cy + 24, 304, 44, "", 28, INT_PDA_Look.TEXT);
			m_InfoVal.Insert(val);
		}
		INT_UIF_Draw.Line(ip, 24, 420, 648, 1, INT_PDA_Look.LINE);
		INT_PDA_Look.DrawIcon(ip, 24, 442, 18, "note", INT_PDA_Look.DIM2);
		INT_PDA_Look.MText(ip, 52, 438, 620, 60, "#STR_INT_PDA_ADMIN_SYS_NOTE", 14, INT_PDA_Look.DIM2);
	}

	protected INT_UIF_Button ReloadRow(Widget parent, int i, string iconName, string titleKey, string subKey)
	{
		float y = 44 + i * 94;
		Widget row = INT_UIF_Draw.Frame(parent, 18, y, 624, 84, "INT_PDA_AdmReload");
		INT_UIF_Surface.Box(row, 0, 0, 624, 84, 4, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE);
		INT_UIF_Surface.Box(row, 16, 16, 52, 52, 6, INT_PDA_Look.PANEL, 0);
		INT_PDA_Look.DrawIcon(row, 28, 28, 28, iconName, INT_PDA_Look.ORANGE);
		string cap = INT_PDA_Look.Caps(titleKey);
		INT_UIF_Draw.Text(row, 84, 12, 340, 30, cap, 20, INT_PDA_Look.TEXT);
		INT_UIF_Draw.Text(row, 84, 46, 350, 22, subKey, 14, INT_PDA_Look.DIM);
		string btn = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_SYS_RELOAD_BTN");
		return INT_UIF_Button.Create(m_Bag, row, 444, 20, 164, 44, btn, INT_UIF_Variant.SECONDARY, "refresh");
	}

	// =================================================================================================================
	// page hooks
	// =================================================================================================================
	override void OnShow()
	{
		if (!RefreshRights())
			return;
		SelectSub(m_Sub, false, true);
	}

	override void OnHide()
	{
		// nothing queued survives the tab: a delete sent minutes later would surprise the admin
		DropRequests();
		m_CfKind = CF_NONE;
		if (m_Confirm)
			m_Confirm.Close();
		if (m_Factions && m_Sub == SUB_FACTIONS)
			m_Factions.Show(false);
	}

	override void Update(float dt)
	{
		PumpQueue();
		CheckTimeouts();
		if (m_Sub == SUB_FACTIONS && m_Factions)
			m_Factions.Update(dt);
	}

	override bool HandleEscape()
	{
		if (m_Confirm && m_Confirm.IsOpen())
		{
			m_Confirm.Finish(false);
			return true;
		}
		if (m_Sub == SUB_FACTIONS && m_Factions)
			return m_Factions.HandleEscape();
		return false;
	}

	override void OnChanged(int what)
	{
		bool rights;
		if (what == INT_PDA_Client.CH_ADMIN)
		{
			OnAdminReply();
		}
		else if (what == INT_PDA_Client.CH_NEWS)
		{
			if (m_Active && m_Sub == SUB_NEWS)
				RenderNews();
		}
		else if (what == INT_PDA_Client.CH_WELCOME || what == INT_PDA_Client.CH_AUTH || what == INT_PDA_Client.CH_PROFILE)
		{
			if (m_Active)
			{
				rights = RefreshRights();
				if (rights && m_Sub == SUB_SYSTEM)
					RenderSystem();
			}
		}
		if (m_Active && m_Sub == SUB_FACTIONS && m_Factions)
			m_Factions.OnChanged(what);
	}

	// Only the active page gets results (INT_PDA_Menu.OnClientResult); the admin sub-op is in s_AdmLastSub.
	override void OnResult(int requestOp, int code)
	{
		// the server's request gate answers «too fast» as OP_RESULT (no OP_ADMIN_DATA, s_AdmLastSub is stale): it
		// means OUR last request was dropped, not executed — it goes out again instead of being lost
		if (code == INT_PDA_Const.R_TOO_FAST && requestOp == INT_PDA_Const.OP_NEWS)
		{
			Hold();
			m_WantNews = true;
			return;
		}
		bool tooFast = false;
		if (code == INT_PDA_Const.R_TOO_FAST && requestOp == INT_PDA_Const.OP_ADMIN)
			tooFast = true;
		// both this page and «Группировки» send OP_ADMIN: the one that sent last owns the dropped request
		bool theirs = false;
		if (tooFast && m_Sub == SUB_FACTIONS && m_Factions && m_Factions.LastSendMs() >= m_LastSendMs)
			theirs = true;
		if (tooFast && !theirs && m_LastKind != KIND_NONE)
		{
			OnTooFast();
			return;
		}
		// a «too fast» nobody here is waiting for (s_AdmLastSub would name an older answer)
		if (tooFast && m_Sub != SUB_FACTIONS)
			return;
		if (m_Sub == SUB_FACTIONS)
		{
			if (m_Factions)
				m_Factions.OnResult(requestOp, code);
			return;
		}
		if (requestOp != INT_PDA_Const.OP_ADMIN)
			return;
		int sub = INT_PDA_Client.s_AdmLastSub;
		bool ok = false;
		if (code == INT_PDA_Const.R_OK)
			ok = true;
		if (sub == INT_PDA_Const.ADM_PLAYERS)
		{
			if (!ok)
			{
				INT_PDA_Sfx.Play("Error");
				Toast(code);
			}
			return;
		}
		if (sub != INT_PDA_Const.ADM_SET_PASSWORD && sub != INT_PDA_Const.ADM_DELETE_ACCOUNT && sub != INT_PDA_Const.ADM_SET_ROLE && sub != INT_PDA_Const.ADM_KICK && sub != INT_PDA_Const.ADM_NEWS_POST && sub != INT_PDA_Const.ADM_NEWS_EDIT && sub != INT_PDA_Const.ADM_NEWS_DELETE && sub != INT_PDA_Const.ADM_SETTINGS && sub != INT_PDA_Const.ADM_RELOAD_DB && sub != INT_PDA_Const.ADM_RELOAD_FACTIONS && sub != INT_PDA_Const.ADM_RELOAD_CONFIG)
			return;
		Toast(code);
		if (!ok)
		{
			INT_PDA_Sfx.Play("Error");
			RenderDetail();
			return;
		}
		if (sub == INT_PDA_Const.ADM_SET_PASSWORD || sub == INT_PDA_Const.ADM_SET_ROLE || sub == INT_PDA_Const.ADM_KICK || sub == INT_PDA_Const.ADM_RELOAD_DB)
		{
			RequestPlayers(m_Offset);
		}
		else if (sub == INT_PDA_Const.ADM_DELETE_ACCOUNT)
		{
			m_Sel = null;
			RenderDetail();
			RequestPlayers(m_Offset);
		}
		else if (sub == INT_PDA_Const.ADM_NEWS_POST || sub == INT_PDA_Const.ADM_NEWS_EDIT)
		{
			// published / changed: the form is empty again (and out of the edit mode)
			StopNewsEdit();
			m_WantNews = true;
			PumpQueue();
		}
		else if (sub == INT_PDA_Const.ADM_NEWS_DELETE)
		{
			m_WantNews = true;
			PumpQueue();
		}
		else
		{
			// settings / reloads: the server resent WELCOME before this answer, s_Settings is fresh; only the saved
			// radius replaces what is typed in the field (a reload keeps an unsaved value)
			RenderSystem();
			FillRadiusInput(sub == INT_PDA_Const.ADM_SETTINGS);
			if (sub == INT_PDA_Const.ADM_SETTINGS)
			{
				FillNewsSettings();
				if (m_EditNewsId < 0)
					FillNewsMinutes();
			}
		}
	}

	// Rights can change while the PDA is open (config reload): the tab then shows the «no clearance» plate.
	protected bool RefreshRights()
	{
		bool ok = INT_PDA_Client.s_IsAdmin;
		if (m_NoRights)
			m_NoRights.Show(!ok);
		if (m_Bar)
			m_Bar.Show(ok);
		if (m_Area)
			m_Area.Show(ok);
		if (!ok)
			DropAdminUi();
		return ok;
	}

	// Rights taken away (config reload) while something of this tab is open: the confirm, a prompt of this tab, an open
	// dropdown and the queued requests go — no action can be confirmed after the clearance is gone.
	protected void DropAdminUi()
	{
		if (m_Confirm && m_Confirm.IsOpen())
			m_Confirm.Close();
		m_CfKind = CF_NONE;
		INT_UIF_Dropdown.CloseActive();
		if (m_Menu && m_Menu.HasPrompt())
			m_Menu.ClosePrompt();
		m_PromptSteam = "";
		DropRequests();
	}

	// Forgets every queued / awaited request of this tab (hidden tab, rights gone).
	protected void DropRequests()
	{
		m_Queue.Clear();
		m_QueueSub.Clear();
		m_QueueTry.Clear();
		m_WantPlayers = false;
		m_WantNews = false;
		m_PlayersWaiting = false;
		m_ActionWaiting = false;
		m_LastActRpc = null;
		m_LastKind = KIND_NONE;
		SetPostBusy(false);
	}

	// =================================================================================================================
	// sub-tabs
	// =================================================================================================================
	protected void SelectSub(int sub, bool sound, bool force)
	{
		int nsub = INT_PDA_Util.ClampI(sub, SUB_PLAYERS, SUB_SYSTEM);
		if (nsub == m_Sub && !force)
			return;
		if (m_Sub == SUB_FACTIONS && nsub != SUB_FACTIONS && m_Factions)
			m_Factions.Show(false);
		m_Sub = nsub;
		m_PlayersBox.Show(nsub == SUB_PLAYERS);
		m_NewsBox.Show(nsub == SUB_NEWS);
		m_SysBox.Show(nsub == SUB_SYSTEM);
		m_FactionsHost.Show(nsub == SUB_FACTIONS);
		// «Группировки» has its own refresh
		bool ownRefresh = nsub != SUB_FACTIONS;
		m_RefreshHit.Show(ownRefresh);
		m_RefreshText.Show(ownRefresh);
		m_RefreshIcon.Show(ownRefresh);
		PaintSubTabs(true);
		Widget box = m_PlayersBox;
		if (nsub == SUB_PLAYERS)
		{
			RenderPlayers();
			RenderDetail();
			RequestPlayers(m_Offset);
		}
		else if (nsub == SUB_NEWS)
		{
			box = m_NewsBox;
			RenderNews();
			UpdateNewsPreview();
			m_WantNews = true;
			PumpQueue();
		}
		else if (nsub == SUB_SYSTEM)
		{
			box = m_SysBox;
			RenderSystem();
			FillRadiusInput();
		}
		else
		{
			box = m_FactionsHost;
			if (m_Factions)
				m_Factions.Show(true);
		}
		RevealBox(box);
		if (sound)
			INT_PDA_Sfx.Play("Tab");
	}

	protected void RevealBox(Widget box)
	{
		if (!box)
			return;
		INT_UIF_Animator.Get().Cancel(box);
		INT_UIF_Draw.Move(box, 0, 0);
		Reveal(box, 0.02);
	}

	protected void PaintSubTabs(bool animate)
	{
		int i;
		int col;
		for (i = 0; i < m_SubText.Count(); i++)
		{
			col = INT_PDA_Look.DIM;
			if (i == m_Sub)
				col = INT_PDA_Look.TEXT;
			m_SubText[i].SetColor(col);
		}
		if (!m_SubLine || m_Sub < 0 || m_Sub >= m_SubX.Count())
			return;
		float lx = m_SubX[m_Sub];
		float lw = m_SubW[m_Sub];
		if (animate)
		{
			INT_UIF_Animator.MoveX(m_SubLine, INT_UIF_Draw.S(lx), 0.26, INT_UIF_Ease.OUT_CUBIC);
			INT_UIF_Animator.ResizeW(m_SubLine, INT_UIF_Draw.S(lw), 0.26, INT_UIF_Ease.OUT_CUBIC);
		}
		else
		{
			INT_UIF_Draw.Place(m_SubLine, lx, BAR_H - 4, lw, 3);
		}
	}

	protected void RefreshCurrent()
	{
		if (m_Sub == SUB_PLAYERS)
		{
			RequestPlayers(m_Offset);
		}
		else if (m_Sub == SUB_NEWS)
		{
			m_WantNews = true;
			PumpQueue();
		}
		else if (m_Sub == SUB_SYSTEM)
		{
			RenderSystem();
			FillRadiusInput();
		}
	}

	// =================================================================================================================
	// network: one queue that keeps the server gap
	// =================================================================================================================
	// False = not queued (the queue is full; the admin was told).
	protected bool QueueRpc(ScriptRPC rpc, int sub)
	{
		if (!rpc)
			return false;
		if (m_Queue.Count() >= QUEUE_MAX)
		{
			INT_UIF_Toast.Warning("#STR_INT_PDA_R_TOO_FAST");
			return false;
		}
		m_Queue.Insert(rpc);
		m_QueueSub.Insert(sub);
		m_QueueTry.Insert(0);
		PumpQueue();
		return true;
	}

	// Sends at most one request when the gap allows: queued actions first, then the players page, then the news.
	protected void PumpQueue()
	{
		if (!GetGame())
			return;
		int now = GetGame().GetTime();
		int gap = INT_PDA_Const.REQUEST_GAP_MS + GAP_MARGIN_MS;
		if (now < m_LastSendMs)
			m_LastSendMs = now - gap;
		if (now - m_LastSendMs < gap)
			return;
		// a hold after «too fast»; a hold far in the future = a new mission restarted the clock
		if (m_HoldUntilMs - now > RETRY_HOLD_MS * 2)
			m_HoldUntilMs = 0;
		if (now < m_HoldUntilMs)
			return;
		if (m_Queue.Count() > 0)
		{
			ScriptRPC rpc = m_Queue[0];
			int sub = m_QueueSub[0];
			int tries = 0;
			if (m_QueueTry.Count() > 0)
				tries = m_QueueTry[0];
			INT_PDA_Client.Send(rpc);
			m_Queue.RemoveOrdered(0);
			m_QueueSub.RemoveOrdered(0);
			if (m_QueueTry.Count() > 0)
				m_QueueTry.RemoveOrdered(0);
			m_LastSendMs = now;
			m_ActionWaiting = true;
			m_ActionSub = sub;
			m_ActionSentMs = now;
			m_LastActRpc = rpc;
			m_ActTry = tries;
			m_LastKind = KIND_ACTION;
			TestLog("отправлено: операция " + sub.ToString());
			return;
		}
		if (m_WantPlayers)
		{
			m_WantPlayers = false;
			SendPlayersNow();
			m_LastSendMs = now;
			m_PlayersWaiting = true;
			m_PlayersSentMs = now;
			m_LastKind = KIND_PLAYERS;
			RenderStatus();
			return;
		}
		if (m_WantNews)
		{
			m_WantNews = false;
			INT_PDA_Client.RequestNews();
			m_LastSendMs = now;
		}
	}

	protected void Hold()
	{
		if (GetGame())
			m_HoldUntilMs = GetGame().GetTime() + RETRY_HOLD_MS;
	}

	// The server's gate dropped our last OP_ADMIN request (answered «too fast», nothing was done): an action goes out
	// again (twice at most, then the admin is told), a players page is asked again.
	protected void OnTooFast()
	{
		int kind = m_LastKind;
		m_LastKind = KIND_NONE;
		Hold();
		if (kind == KIND_PLAYERS)
		{
			TestLog("слишком часто: страница игроков ещё раз");
			m_PlayersWaiting = false;
			m_WantPlayers = true;
			RenderStatus();
			return;
		}
		if (kind != KIND_ACTION || !m_ActionWaiting || !m_LastActRpc)
			return;
		m_ActionWaiting = false;
		int sub = m_ActionSub;
		if (m_ActTry >= RETRY_MAX)
		{
			TestLog("слишком часто: операция " + sub.ToString() + " не выполнена");
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Warning("#STR_INT_PDA_ADMIN_NOT_DONE");
			if (sub == INT_PDA_Const.ADM_NEWS_POST || sub == INT_PDA_Const.ADM_NEWS_EDIT)
				SetPostBusy(false);
			RenderDetail();
			return;
		}
		TestLog("слишком часто: операция " + sub.ToString() + " ещё раз");
		m_Queue.InsertAt(m_LastActRpc, 0);
		m_QueueSub.InsertAt(sub, 0);
		m_QueueTry.InsertAt(m_ActTry + 1, 0);
		m_LastActRpc = null;
	}

	// «Опубликовать» is locked from the click to the answer (a double click must not post the news twice).
	protected void SetPostBusy(bool busy)
	{
		m_PostBusy = busy;
		if (busy && GetGame())
			m_PostBusyMs = GetGame().GetTime();
		if (m_PostBtn)
			m_PostBtn.SetEnabled(!busy);
	}

	// A request the server dropped (gap of another sender, lost packet) is never answered: one silent retry for the
	// players page, a warning for an action.
	protected void CheckTimeouts()
	{
		if (!GetGame())
			return;
		int now = GetGame().GetTime();
		if (m_PlayersWaiting && now - m_PlayersSentMs > REPLY_TIMEOUT_MS)
		{
			m_PlayersWaiting = false;
			if (m_LastKind == KIND_PLAYERS)
				m_LastKind = KIND_NONE;
			if (m_Retries < 1)
			{
				m_Retries++;
				m_WantPlayers = true;
			}
			else
			{
				m_Retries = 0;
				INT_UIF_Toast.Warning("#STR_INT_PDA_ADMIN_NO_ANSWER");
			}
			RenderStatus();
		}
		if (m_ActionWaiting && now - m_ActionSentMs > REPLY_TIMEOUT_MS)
		{
			m_ActionWaiting = false;
			if (m_LastKind == KIND_ACTION)
				m_LastKind = KIND_NONE;
			INT_UIF_Toast.Warning("#STR_INT_PDA_ADMIN_NO_ANSWER");
			RenderDetail();
		}
		// the news answer never came (or the post never left): «Опубликовать» unlocks; the admin checks the feed first
		if (m_PostBusy && (now - m_PostBusyMs > POST_BUSY_MS || now < m_PostBusyMs))
			SetPostBusy(false);
	}

	protected void RequestPlayers(int offset)
	{
		int off = offset;
		if (off < 0)
			off = 0;
		m_ReqOffset = off;
		m_WantPlayers = true;
		m_Retries = 0;
		PumpQueue();
		RenderStatus();
	}

	// ADM_PLAYERS: int offset, string filter (INT_PDA_SE_Admin.OnPlayers).
	protected void SendPlayersNow()
	{
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_PLAYERS);
		int off = m_ReqOffset;
		rpc.Write(off);
		INT_PDA_Proto.WriteStr(rpc, m_Filter);
		INT_PDA_Client.Send(rpc);
		TestLog("отправлено: игроки, смещение " + off.ToString());
	}

	// ADM_SET_PASSWORD: string steam, string password.
	protected void SendPass(string steam, string pw)
	{
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_SET_PASSWORD);
		INT_PDA_Proto.WriteStr(rpc, steam);
		INT_PDA_Proto.WriteStr(rpc, pw);
		QueueRpc(rpc, INT_PDA_Const.ADM_SET_PASSWORD);
	}

	// ADM_DELETE_ACCOUNT: string steam.
	protected void SendDelete(string steam)
	{
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_DELETE_ACCOUNT);
		INT_PDA_Proto.WriteStr(rpc, steam);
		QueueRpc(rpc, INT_PDA_Const.ADM_DELETE_ACCOUNT);
	}

	// ADM_SET_ROLE: string faction id (the account's own faction), string steam, int role (server clamps 1..3).
	protected void SendRole(string factionId, string steam, int role)
	{
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_SET_ROLE);
		INT_PDA_Proto.WriteStr(rpc, factionId);
		INT_PDA_Proto.WriteStr(rpc, steam);
		int r = role;
		rpc.Write(r);
		QueueRpc(rpc, INT_PDA_Const.ADM_SET_ROLE);
	}

	// ADM_KICK: string faction id, string steam.
	protected void SendKick(string factionId, string steam)
	{
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_KICK);
		INT_PDA_Proto.WriteStr(rpc, factionId);
		INT_PDA_Proto.WriteStr(rpc, steam);
		QueueRpc(rpc, INT_PDA_Const.ADM_KICK);
	}

	// ADM_NEWS_POST: string title, string text, int minutes (-1 = server default, 0 = until deleted).
	protected void SendNewsPost(string title, string body, int minutes = -1)
	{
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_NEWS_POST);
		INT_PDA_Proto.WriteStr(rpc, title);
		INT_PDA_Proto.WriteStr(rpc, body);
		int mins = minutes;
		rpc.Write(mins);
		if (QueueRpc(rpc, INT_PDA_Const.ADM_NEWS_POST))
			SetPostBusy(true);
	}

	// ADM_NEWS_EDIT: int id, string title, string text, int minutes (the lifetime counts again from now).
	protected void SendNewsEdit(int id, string title, string body, int minutes)
	{
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_NEWS_EDIT);
		int nid = id;
		rpc.Write(nid);
		INT_PDA_Proto.WriteStr(rpc, title);
		INT_PDA_Proto.WriteStr(rpc, body);
		int mins = minutes;
		rpc.Write(mins);
		if (QueueRpc(rpc, INT_PDA_Const.ADM_NEWS_EDIT))
			SetPostBusy(true);
	}

	// ADM_NEWS_DELETE: int id.
	protected void SendNewsDel(int id)
	{
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_NEWS_DELETE);
		int nid = id;
		rpc.Write(nid);
		QueueRpc(rpc, INT_PDA_Const.ADM_NEWS_DELETE);
	}

	// ADM_SETTINGS: float radius (server clamps 1..50), int news rotation s, int default news minutes.
	protected void SendRadius(float radius)
	{
		SendSettings(radius, INT_PDA_Client.s_Settings.NewsRotateSeconds, INT_PDA_Client.s_Settings.NewsMinutesDefault);
	}

	protected void SendSettings(float radius, int rotate, int defMinutes)
	{
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_SETTINGS);
		float rad = radius;
		rpc.Write(rad);
		int rot = rotate;
		rpc.Write(rot);
		int defMin = defMinutes;
		rpc.Write(defMin);
		QueueRpc(rpc, INT_PDA_Const.ADM_SETTINGS);
	}

	// ADM_RELOAD_DB / _FACTIONS / _CONFIG: no payload.
	protected void SendReload(int sub)
	{
		ScriptRPC rpc = INT_PDA_Client.Admin(sub);
		QueueRpc(rpc, sub);
	}

	// Every OP_ADMIN_DATA (all pages get CH_ADMIN): ends the waits, takes a players page.
	protected void OnAdminReply()
	{
		int sub = INT_PDA_Client.s_AdmLastSub;
		int code = INT_PDA_Client.s_AdmLastCode;
		if (m_ActionWaiting && sub == m_ActionSub)
		{
			m_ActionWaiting = false;
			m_LastActRpc = null;
			if (m_LastKind == KIND_ACTION)
				m_LastKind = KIND_NONE;
		}
		if (sub == INT_PDA_Const.ADM_PLAYERS && m_LastKind == KIND_PLAYERS)
			m_LastKind = KIND_NONE;
		if (sub == INT_PDA_Const.ADM_NEWS_POST)
			SetPostBusy(false);
		string line = "ответ: операция " + sub.ToString() + ", код " + code.ToString();
		if (sub == INT_PDA_Const.ADM_PLAYERS && code == INT_PDA_Const.R_OK)
		{
			int got = INT_PDA_Client.s_AdmRows.Count();
			line = line + ", строк " + got.ToString() + " из " + INT_PDA_Client.s_AdmTotal.ToString() + ", смещение " + INT_PDA_Client.s_AdmOffset.ToString();
		}
		TestLog(line);
		if (sub != INT_PDA_Const.ADM_PLAYERS)
			return;
		m_PlayersWaiting = false;
		m_Retries = 0;
		if (code == INT_PDA_Const.R_OK)
			AcceptPlayers();
		if (m_Active && m_Sub == SUB_PLAYERS)
		{
			RenderPlayers();
			RenderDetail();
		}
	}

	protected void AcceptPlayers()
	{
		array<ref INT_PDA_AdminRow> rows = INT_PDA_Client.s_AdmRows;
		int got = rows.Count();
		int total = INT_PDA_Client.s_AdmTotal;
		int off = INT_PDA_Client.s_AdmOffset;
		m_HasData = true;
		// the client reader does not take the server's page size (OnPlayers sends it after the rows): any page that is
		// not the last one is a full page and shows it (not only the first page — a config reload may change it).
		// «Дальше» steps by the rows of THIS page, so a wrong guess can never skip accounts.
		if (got > 0 && off + got < total)
			m_PageSize = got;
		if (off != m_Offset)
			m_First = 0;
		m_Offset = off;
		// a page behind the end (its last account was deleted): back to the real last page
		int ps = PageSize();
		int last;
		if (got == 0 && off > 0 && total > 0 && m_Active)
		{
			last = ((total - 1) / ps) * ps;
			if (last < off)
			{
				RequestPlayers(last);
				return;
			}
		}
		// keep the card on the same account (fresh row object), or open the one a test / search asked for
		string want = m_AutoSelect;
		if (want == "" && m_Sel)
			want = m_Sel.SteamId;
		bool autoPick = m_AutoSelect != "";
		m_AutoSelect = "";
		if (want == "")
			return;
		int i;
		for (i = 0; i < got; i++)
		{
			if (rows[i].SteamId != want)
				continue;
			m_Sel = rows[i];
			if (autoPick)
			{
				m_ShowPass = false;
				if (i < m_First || i >= m_First + ROWS_VIS)
					m_First = i;
			}
			return;
		}
	}

	protected int PageSize()
	{
		if (m_PageSize < 1)
			return DEFAULT_PAGE;
		return m_PageSize;
	}

	protected void TestLog(string text)
	{
		if (m_TestMode)
			INT_PDA_Log.Info("админ-тест: " + text);
		else
			INT_PDA_Log.Dbg("админ: " + text);
	}

	// =================================================================================================================
	// players: render
	// =================================================================================================================
	protected void RenderPlayers()
	{
		if (!m_Table)
			return;
		array<ref INT_PDA_AdminRow> rows = INT_PDA_Client.s_AdmRows;
		int n = 0;
		if (m_HasData)
			n = rows.Count();
		int maxFirst = n - ROWS_VIS;
		if (maxFirst < 0)
			maxFirst = 0;
		m_First = INT_PDA_Util.ClampI(m_First, 0, maxFirst);
		int i;
		int idx;
		int dotC;
		INT_PDA_PageAdminRowW rw;
		INT_PDA_AdminRow r;
		string s;
		string raw;
		for (i = 0; i < m_PRows.Count(); i++)
		{
			rw = m_PRows[i];
			idx = m_First + i;
			if (idx >= n)
			{
				rw.m_Index = -1;
				rw.m_SteamId = "";
				rw.m_Frame.Show(false);
				continue;
			}
			r = rows[idx];
			rw.m_Index = idx;
			rw.m_SteamId = r.SteamId;
			rw.m_Frame.Show(true);
			s = FitText(r.Callsign, W_CS, 16);
			rw.m_Callsign.SetText(s);
			raw = OrDash(r.GameName);
			s = FitText(raw, W_GAME, 15);
			rw.m_Game.SetText(s);
			s = FitText(r.SteamId, W_STEAM, 13);
			rw.m_Steam.SetText(s);
			raw = OrDash(r.FactionName);
			s = FitText(raw, W_FAC, 15);
			rw.m_Faction.SetText(s);
			raw = RoleText(r);
			s = FitText(raw, W_ROLE, 15);
			rw.m_Role.SetText(s);
			raw = OrDash(r.Rank);
			s = FitText(raw, W_RANK, 15);
			rw.m_Rank.SetText(s);
			s = StampText(r.LastSeen, false);
			rw.m_Seen.SetText(s);
			dotC = INT_PDA_Look.DIM2;
			if (r.Online)
				dotC = INT_PDA_Look.ALLY;
			rw.m_Dot.SetColor(dotC);
			PaintRow(i);
		}
		bool empty = false;
		if (m_HasData && n == 0)
			empty = true;
		m_TableEmpty.Show(empty);
		PlaceThumb(m_Thumb, m_ThumbTrack, 932, 46, 432, m_First, ROWS_VIS, n);
		// pager
		int ps = PageSize();
		int total = 0;
		if (m_HasData)
			total = INT_PDA_Client.s_AdmTotal;
		int pages = (total + ps - 1) / ps;
		if (pages < 1)
			pages = 1;
		int cur = m_Offset / ps + 1;
		if (cur > pages)
			cur = pages;
		string pg = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_PL_PAGE") + " " + cur.ToString() + " / " + pages.ToString();
		m_PageText.SetText(pg);
		bool canPrev = false;
		if (m_Offset > 0)
			canPrev = true;
		bool canNext = false;
		if (m_Offset + n < total)
			canNext = true;
		if (m_PrevBtn)
			m_PrevBtn.SetEnabled(canPrev);
		if (m_NextBtn)
			m_NextBtn.SetEnabled(canNext);
		string range = "";
		if (n > 0)
		{
			int a = m_Offset + 1;
			int b = m_Offset + n;
			string ofWord = Widget.TranslateString("#STR_INT_PDA_ADMIN_PL_OF");
			range = a.ToString() + "–" + b.ToString() + " " + ofWord + " " + total.ToString();
		}
		m_RangeText.SetText(range);
		RenderStatus();
	}

	protected void PaintRow(int i)
	{
		if (i < 0 || i >= m_PRows.Count())
			return;
		INT_PDA_PageAdminRowW rw = m_PRows[i];
		bool sel = false;
		if (m_Sel && rw.m_SteamId != "" && rw.m_SteamId == m_Sel.SteamId)
			sel = true;
		int bg = 0x00000000;
		if (sel)
			bg = INT_PDA_Look.ORANGE_SOFT;
		else if (i == m_HoverRow)
			bg = INT_PDA_Look.PANEL2;
		rw.m_Bg.SetColor(bg);
		rw.m_Bar.Show(sel);
		int tc = INT_PDA_Look.TEXT;
		if (sel)
			tc = INT_PDA_Look.ORANGE;
		rw.m_Callsign.SetColor(tc);
	}

	// «Запрос…» / «Найдено: N» / «Аккаунтов: N» and the refresh icon lit while a request is on its way.
	protected void RenderStatus()
	{
		bool busy = false;
		if (m_PlayersWaiting || m_WantPlayers)
			busy = true;
		if (m_FoundText)
		{
			string st = "";
			if (busy)
				st = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_LOADING");
			else if (m_HasData && m_Filter != "")
				st = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_PL_FOUND") + ": " + INT_PDA_Client.s_AdmTotal.ToString();
			else if (m_HasData)
				st = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_PL_TOTAL") + ": " + INT_PDA_Client.s_AdmTotal.ToString();
			m_FoundText.SetText(st);
		}
		if (m_RefreshIcon)
		{
			int ic = INT_PDA_Look.DIM;
			if (busy && m_Sub == SUB_PLAYERS)
				ic = INT_PDA_Look.ORANGE;
			m_RefreshIcon.SetColor(ic);
		}
	}

	// Thin scroll indicator of a row pool (whole-row scrolling: text is not clipped by frames).
	protected void PlaceThumb(ImageWidget thumb, ImageWidget track, float x, float y, float h, int first, int vis, int n)
	{
		if (!thumb)
			return;
		bool show = false;
		if (n > vis)
			show = true;
		thumb.Show(show);
		if (track)
			track.Show(show);
		if (!show)
			return;
		float th = h * vis / n;
		if (th < 24)
			th = 24;
		int maxFirst = n - vis;
		float k = first;
		k = k / maxFirst;
		INT_UIF_Draw.Place(thumb, x, y + k * (h - th), 3, th);
	}

	protected void RenderDetail()
	{
		if (!m_DetBody)
			return;
		bool has = false;
		if (m_Sel)
			has = true;
		m_DetBody.Show(has);
		m_DetEmpty.Show(!has);
		if (!has)
			return;
		INT_PDA_AdminRow r = m_Sel;
		string avPath = INT_PDA_Look.AvatarPath(r.Avatar);
		m_DetAvatar.LoadImageFile(0, avPath);
		m_DetAvatar.SetImage(0);
		string s = FitText(r.Callsign, 284, 30);
		m_DetCallsign.SetText(s);
		int dotC = INT_PDA_Look.DIM2;
		int onC = INT_PDA_Look.DIM;
		string onText = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_PL_OFFLINE");
		if (r.Online)
		{
			dotC = INT_PDA_Look.ALLY;
			onC = INT_PDA_Look.ALLY;
			onText = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_PL_ONLINE");
		}
		m_DetDot.SetColor(dotC);
		m_DetOnline.SetText(onText);
		m_DetOnline.SetColor(onC);
		bool inF = false;
		if (r.FactionId != "")
			inF = true;
		string roleText = RoleText(r);
		string head = Widget.TranslateString("#STR_INT_PDA_NO_FACTION");
		if (inF)
			head = r.FactionName + " · " + roleText;
		s = FitText(head, 284, 14);
		m_DetSub.SetText(s);
		string v = r.SteamId;
		SetDet(0, v);
		v = OrDash(r.GameName);
		SetDet(1, v);
		v = OrDash(r.Discord);
		SetDet(2, v);
		v = StampText(r.Registered, true);
		SetDet(3, v);
		v = StampText(r.LastSeen, true);
		SetDet(4, v);
		v = OrDash(r.FactionName);
		if (!inF)
			v = "—";
		SetDet(5, v);
		SetDet(6, roleText);
		v = OrDash(r.Rank);
		SetDet(7, v);
		// password: the server sends it to admins (owner brief, admin item 4); masked until the eye is pressed
		v = MaskPassword(r.Password);
		string eyeIcon = "eye";
		string eyeKey = "#STR_INT_PDA_ADMIN_PL_SHOW";
		if (m_ShowPass)
		{
			v = OrDash(r.Password);
			eyeIcon = "eye_off";
			eyeKey = "#STR_INT_PDA_ADMIN_PL_HIDE";
		}
		SetDet(8, v);
		string eyePath = INT_PDA_Look.IconPath(eyeIcon);
		m_EyeIcon.LoadImageFile(0, eyePath);
		m_EyeIcon.SetImage(0);
		string eyeText = INT_PDA_Look.Caps(eyeKey);
		m_EyeText.SetText(eyeText);
		int roleIdx = 0;
		if (inF)
			roleIdx = INT_PDA_Util.ClampI(r.Role, INT_PDA_Const.ROLE_NONE, INT_PDA_Const.ROLE_LEADER);
		if (m_RoleDrop)
		{
			m_RoleDrop.SetSelected(roleIdx, false);
			m_RoleDrop.SetEnabled(inF);
		}
		if (m_KickBtn)
			m_KickBtn.SetEnabled(inF);
	}

	protected void SetDet(int i, string value)
	{
		if (i < 0 || i >= m_DetVal.Count())
			return;
		string s = FitText(value, CARD_VAL_W, 15);
		m_DetVal[i].SetText(s);
	}

	// =================================================================================================================
	// players: actions
	// =================================================================================================================
	protected void DoSearch(string filter)
	{
		string f = filter;
		f.TrimInPlace();
		m_Filter = f;
		m_First = 0;
		INT_PDA_Sfx.Play("Click");
		RequestPlayers(0);
	}

	protected void OnSearchSubmit(string text, INT_UIF_TextInput t)
	{
		DoSearch(text);
	}

	protected void OnFindClick(INT_UIF_Button b)
	{
		string text = "";
		if (m_Search)
			text = m_Search.GetText();
		DoSearch(text);
	}

	protected void OnAllClick(INT_UIF_Button b)
	{
		if (m_Search)
			m_Search.SetText("");
		DoSearch("");
	}

	protected void OnPrevClick(INT_UIF_IconButton b)
	{
		INT_PDA_Sfx.Play("Click");
		int ps = PageSize();
		RequestPlayers(m_Offset - ps);
	}

	protected void OnNextClick(INT_UIF_IconButton b)
	{
		INT_PDA_Sfx.Play("Click");
		int step = 0;
		if (m_HasData)
			step = INT_PDA_Client.s_AdmRows.Count();
		if (step <= 0)
			step = PageSize();
		RequestPlayers(m_Offset + step);
	}

	protected void PickRow(int i)
	{
		if (i < 0 || i >= m_PRows.Count())
			return;
		INT_PDA_PageAdminRowW rw = m_PRows[i];
		array<ref INT_PDA_AdminRow> rows = INT_PDA_Client.s_AdmRows;
		if (rw.m_Index < 0 || rw.m_Index >= rows.Count())
			return;
		m_Sel = rows[rw.m_Index];
		m_ShowPass = false;
		INT_PDA_Sfx.Play("Click");
		RenderPlayers();
		RenderDetail();
		RevealCard();
	}

	protected void RevealCard()
	{
		if (!m_DetBody)
			return;
		INT_UIF_Animator.Get().Cancel(m_DetBody);
		INT_UIF_Draw.Move(m_DetBody, 0, 0);
		Reveal(m_DetBody, 0);
	}

	protected bool SelectBySteam(string steam)
	{
		if (!m_HasData || steam == "")
			return false;
		array<ref INT_PDA_AdminRow> rows = INT_PDA_Client.s_AdmRows;
		int i;
		for (i = 0; i < rows.Count(); i++)
		{
			if (rows[i].SteamId != steam)
				continue;
			m_Sel = rows[i];
			m_ShowPass = false;
			if (i < m_First || i >= m_First + ROWS_VIS)
				m_First = i;
			RenderPlayers();
			RenderDetail();
			return true;
		}
		return false;
	}

	// Faction id of an account of the loaded page (ADM_SET_ROLE / ADM_KICK need it).
	protected string FactionOf(string steam)
	{
		if (m_Sel && m_Sel.SteamId == steam)
			return m_Sel.FactionId;
		array<ref INT_PDA_AdminRow> rows = INT_PDA_Client.s_AdmRows;
		int i;
		for (i = 0; i < rows.Count(); i++)
		{
			if (rows[i].SteamId == steam)
				return rows[i].FactionId;
		}
		return "";
	}

	protected void OnRolePicked(int index, string text, INT_UIF_Dropdown d)
	{
		if (!m_Sel || m_Sel.FactionId == "")
		{
			RenderDetail();
			return;
		}
		if (index == m_Sel.Role)
			return;
		string steam = m_Sel.SteamId;
		string fid = m_Sel.FactionId;
		string who = m_Sel.Callsign;
		if (index <= INT_PDA_Const.ROLE_NONE)
		{
			// the server has no «no role» inside a faction: «none» means expel
			AskConfirm(CF_KICK, "#STR_INT_PDA_ADMIN_CF_KICK_T", "#STR_INT_PDA_ADMIN_CF_KICK", "#STR_INT_PDA_ADMIN_PL_KICK", true, steam, fid, 0, who);
			return;
		}
		if (index == INT_PDA_Const.ROLE_LEADER)
		{
			// one leader per faction: the old one becomes a member (INT_PDA_SE_Admin.OnSetRole)
			AskConfirm(CF_LEADER, "#STR_INT_PDA_ADMIN_CF_LEADER_T", "#STR_INT_PDA_ADMIN_CF_LEADER", "#STR_INT_PDA_ADMIN_CF_OK", false, steam, fid, index, who);
			return;
		}
		INT_PDA_Sfx.Play("Click");
		SendRole(fid, steam, index);
	}

	protected void OnKickClick(INT_UIF_Button b)
	{
		if (!m_Sel || m_Sel.FactionId == "")
			return;
		string steam = m_Sel.SteamId;
		string fid = m_Sel.FactionId;
		string who = m_Sel.Callsign;
		AskConfirm(CF_KICK, "#STR_INT_PDA_ADMIN_CF_KICK_T", "#STR_INT_PDA_ADMIN_CF_KICK", "#STR_INT_PDA_ADMIN_PL_KICK", true, steam, fid, 0, who);
	}

	protected void OnDeleteClick(INT_UIF_Button b)
	{
		if (!m_Sel)
			return;
		string steam = m_Sel.SteamId;
		string who = m_Sel.Callsign;
		AskConfirm(CF_DELETE, "#STR_INT_PDA_ADMIN_CF_DELETE_T", "#STR_INT_PDA_ADMIN_CF_DELETE", "#STR_INT_PDA_ADMIN_PL_DELETE", true, steam, "", 0, who);
	}

	// The admin types the new password in plain text (he reads passwords in this tab anyway).
	protected void OnPassClick(INT_UIF_Button b)
	{
		if (!m_Sel || !m_Menu)
			return;
		m_PromptSteam = m_Sel.SteamId;
		INT_PDA_Prompt p = m_Menu.OpenPrompt("#STR_INT_PDA_BTN_PASSWORD", "#STR_INT_PDA_F_NEW_PASSWORD", "", false);
		if (p)
			p.m_OnDone.Insert(OnNewPassDone);
	}

	protected void OnNewPassDone(bool ok, string a, string b)
	{
		if (m_Menu)
			m_Menu.ClosePrompt();
		string steam = m_PromptSteam;
		m_PromptSteam = "";
		if (!ok || steam == "")
			return;
		string pw = a;
		pw.TrimInPlace();
		if (!INT_PDA_Util.IsPassword(pw))
		{
			INT_UIF_Toast.Error("#STR_INT_PDA_R_PASSWORD_FORMAT");
			INT_PDA_Sfx.Play("Error");
			return;
		}
		SendPass(steam, pw);
	}

	// =================================================================================================================
	// confirm
	// =================================================================================================================
	protected void AskConfirm(int kind, string titleKey, string textKey, string okKey, bool danger, string a, string b, int n, string who)
	{
		if (!m_Confirm)
			return;
		m_CfKind = kind;
		m_CfA = a;
		m_CfB = b;
		m_CfN = n;
		string title = INT_PDA_Look.Caps(titleKey);
		string body = Widget.TranslateString(textKey);
		if (who != "")
			body = who + " — " + body;
		string okText = INT_PDA_Look.Caps(okKey);
		m_Confirm.Open(title, body, okText, danger);
	}

	protected void OnConfirmDone(bool ok)
	{
		int kind = m_CfKind;
		string a = m_CfA;
		string b = m_CfB;
		int n = m_CfN;
		m_CfKind = CF_NONE;
		if (!ok)
		{
			RenderDetail();
			return;
		}
		if (kind == CF_DELETE)
			SendDelete(a);
		else if (kind == CF_KICK)
			SendKick(b, a);
		else if (kind == CF_LEADER)
			SendRole(b, a, INT_PDA_Const.ROLE_LEADER);
		else if (kind == CF_NEWS_DEL)
			SendNewsDel(n);
		else if (kind == CF_RELOAD_DB)
			SendReload(INT_PDA_Const.ADM_RELOAD_DB);
		else if (kind == CF_RELOAD_FACTIONS)
			SendReload(INT_PDA_Const.ADM_RELOAD_FACTIONS);
		else if (kind == CF_RELOAD_CONFIG)
			SendReload(INT_PDA_Const.ADM_RELOAD_CONFIG);
	}

	// =================================================================================================================
	// news
	// =================================================================================================================
	protected void RenderNews()
	{
		if (!m_NewsBox)
			return;
		array<ref INT_PDA_News> items = INT_PDA_Client.s_News;
		int n = items.Count();
		int maxFirst = n - NEWS_VIS;
		if (maxFirst < 0)
			maxFirst = 0;
		m_NewsFirst = INT_PDA_Util.ClampI(m_NewsFirst, 0, maxFirst);
		int i;
		int j;
		int nid;
		INT_PDA_PageAdminNewsW nw;
		INT_PDA_News it;
		string s;
		for (i = 0; i < m_NRows.Count(); i++)
		{
			nw = m_NRows[i];
			j = m_NewsFirst + i;
			if (j >= n)
			{
				nw.m_Id = -1;
				nw.m_Head = "";
				nw.m_Frame.Show(false);
				continue;
			}
			// s_News keeps the oldest first: show the newest on top
			it = items[n - 1 - j];
			nid = it.Id;
			nw.m_Id = nid;
			nw.m_Head = it.Title;
			nw.m_Frame.Show(true);
			s = FitText(it.Title, 480, 19);
			nw.m_Title.SetText(s);
			s = "№" + nid.ToString() + " · " + it.Time + " · " + LifeText(it);
			nw.m_Time.SetText(s);
			s = FitText(it.Text, 800, 15);
			nw.m_Text.SetText(s);
			PaintNewsRow(i);
		}
		m_NewsEmpty.Show(n == 0);
		m_NewsCount.SetText(n.ToString());
		// first look at the tab: the settings and the lifetime of a new item come from the server's values
		if (m_RotIn && m_RotIn.GetText() == "")
			FillNewsSettings();
		if (m_NewsMinIn && m_EditNewsId < 0 && m_NewsMinIn.GetText() == "")
			FillNewsMinutes();
		PlaceThumb(m_NewsThumb, m_NewsTrack, 890, 46, NEWS_VIS * NEWS_H - 12, m_NewsFirst, NEWS_VIS, n);
	}

	// «без срока» / «ещё 47 мин» (the server sends what is left; the client counts down from its arrival)
	protected string LifeText(INT_PDA_News it)
	{
		if (!it || it.ExpireMs < 0 || !GetGame())
			return Widget.TranslateString("#STR_INT_PDA_ADMIN_NW_FOREVER");
		int leftMs = it.ExpireMs - GetGame().GetTime();
		int mins = (leftMs + 59999) / 60000;
		if (mins < 0)
			mins = 0;
		string t = Widget.TranslateString("#STR_INT_PDA_ADMIN_NW_LEFT");
		t.Replace("%1", mins.ToString());
		return t;
	}

	protected void PaintNewsRow(int i)
	{
		if (i < 0 || i >= m_NRows.Count())
			return;
		INT_PDA_PageAdminNewsW nw = m_NRows[i];
		int bg = 0x00000000;
		if (i == m_NewsHover || i == m_TrashHover)
			bg = INT_PDA_Look.PANEL2;
		nw.m_Bg.SetColor(bg);
		int tc = INT_PDA_Look.DIM2;
		if (i == m_TrashHover)
			tc = INT_PDA_Look.ENEMY;
		else if (i == m_NewsHover)
			tc = INT_PDA_Look.DIM;
		nw.m_Trash.SetColor(tc);
	}

	protected void AskNewsDelete(int i)
	{
		if (i < 0 || i >= m_NRows.Count())
			return;
		INT_PDA_PageAdminNewsW nw = m_NRows[i];
		if (nw.m_Id < 0)
			return;
		int nid = nw.m_Id;
		string who = nw.m_Head;
		AskConfirm(CF_NEWS_DEL, "#STR_INT_PDA_ADMIN_CF_NEWS_T", "#STR_INT_PDA_ADMIN_CF_NEWS", "#STR_INT_PDA_ADMIN_CF_OK", true, "", "", nid, who);
	}

	protected void OnNewsTyped(string text, INT_UIF_TextInput t)
	{
		INT_PDA_Sfx.Play("Key");
		UpdateNewsPreview();
	}

	protected void OnNewsTitleSubmit(string text, INT_UIF_TextInput t)
	{
		if (m_NewsTextIn)
			m_NewsTextIn.Focus();
	}

	protected void UpdateNewsPreview()
	{
		if (!m_NewsTitleIn || !m_NewsTextIn || !m_PrevTitle)
			return;
		string title = m_NewsTitleIn.GetText();
		string body = m_NewsTextIn.GetText();
		int tl = title.LengthUtf8();
		int bl = body.LengthUtf8();
		int tmax = NEWS_TITLE_MAX;
		int bmax = NEWS_TEXT_MAX;
		m_TitleCount.SetText(tl.ToString() + " / " + tmax.ToString());
		m_TextCount.SetText(bl.ToString() + " / " + bmax.ToString());
		// string.ToUpper is ASCII only: Caps raises Cyrillic too (the banner shows titles in caps)
		string up = "—";
		if (title != "")
			up = INT_PDA_Look.Caps(title);
		string fitted = FitText(up, 388, 18);
		m_PrevTitle.SetText(fitted);
		if (body == "")
			body = "—";
		m_PrevText.SetText(body);
	}

	protected void OnPostClick(INT_UIF_Button b)
	{
		if (!m_NewsTitleIn || !m_NewsTextIn)
			return;
		// the previous news is still on its way: a second click must not post it twice
		if (m_PostBusy)
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Info("#STR_INT_PDA_ADMIN_NW_WAIT");
			return;
		}
		string title = m_NewsTitleIn.GetText();
		string body = m_NewsTextIn.GetText();
		title.TrimInPlace();
		body.TrimInPlace();
		if (title == "" || body == "")
		{
			INT_UIF_Toast.Error("#STR_INT_PDA_R_TEXT");
			INT_PDA_Sfx.Play("Error");
			return;
		}
		int minutes = NewsMinutes();
		INT_PDA_Sfx.Play("Send");
		if (m_EditNewsId >= 0)
			SendNewsEdit(m_EditNewsId, title, body, minutes);
		else
			SendNewsPost(title, body, minutes);
	}

	// The lifetime field: minutes (0 = until deleted); empty = the server's default
	protected int NewsMinutes()
	{
		if (!m_NewsMinIn)
			return -1;
		string s = m_NewsMinIn.GetText();
		s.TrimInPlace();
		if (s == "")
			return -1;
		return INT_PDA_Util.ClampI(s.ToInt(), 0, 525600);
	}

	// A click on a news row of the feed: the item goes into the form for editing (text and lifetime).
	protected void StartNewsEdit(int row)
	{
		if (row < 0 || row >= m_NRows.Count())
			return;
		int nid = m_NRows[row].m_Id;
		if (nid < 0)
			return;
		INT_PDA_News it = null;
		foreach (INT_PDA_News n : INT_PDA_Client.s_News)
		{
			if (n && n.Id == nid)
				it = n;
		}
		if (!it)
			return;
		INT_PDA_Sfx.Play("Click");
		m_EditNewsId = nid;
		if (m_NewsTitleIn)
			m_NewsTitleIn.SetText(it.Title);
		if (m_NewsTextIn)
			m_NewsTextIn.SetText(it.Text);
		if (m_NewsMinIn)
			m_NewsMinIn.SetText(it.Minutes.ToString());
		PaintEditMode();
		UpdateNewsPreview();
	}

	protected void StopNewsEdit()
	{
		m_EditNewsId = -1;
		if (m_NewsTitleIn)
			m_NewsTitleIn.SetText("");
		if (m_NewsTextIn)
			m_NewsTextIn.SetText("");
		FillNewsMinutes();
		PaintEditMode();
		UpdateNewsPreview();
	}

	protected void PaintEditMode()
	{
		bool editing = m_EditNewsId >= 0;
		if (m_EditTag)
		{
			string tag = "";
			if (editing)
				tag = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_NW_EDIT") + " №" + m_EditNewsId.ToString();
			m_EditTag.SetText(tag);
		}
		if (m_EditCancel)
			m_EditCancel.Show(editing);
		if (m_PostBtn)
		{
			string key = "#STR_INT_PDA_ADMIN_NW_POST";
			if (editing)
				key = "#STR_INT_PDA_ADMIN_NW_SAVE";
			m_PostBtn.SetText(INT_PDA_Look.Caps(key));
		}
	}

	// the lifetime field of a NEW item starts with the server default
	protected void FillNewsMinutes()
	{
		if (!m_NewsMinIn || !INT_PDA_Client.s_Settings)
			return;
		m_NewsMinIn.SetText(INT_PDA_Client.s_Settings.NewsMinutesDefault.ToString());
	}

	protected void FillNewsSettings()
	{
		if (!INT_PDA_Client.s_Settings)
			return;
		if (m_RotIn)
			m_RotIn.SetText(INT_PDA_Client.s_Settings.NewsRotateSeconds.ToString());
		if (m_DefMinIn)
			m_DefMinIn.SetText(INT_PDA_Client.s_Settings.NewsMinutesDefault.ToString());
	}

	protected void OnEditCancelClick(INT_UIF_Button b)
	{
		INT_PDA_Sfx.Play("Click");
		StopNewsEdit();
	}

	protected void OnNewsSettingsClick(INT_UIF_Button b)
	{
		if (!m_RotIn || !m_DefMinIn)
			return;
		string rs = m_RotIn.GetText();
		string ds = m_DefMinIn.GetText();
		rs.TrimInPlace();
		ds.TrimInPlace();
		if (rs == "" || ds == "")
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Error("#STR_INT_PDA_R_BAD_DATA");
			return;
		}
		int rot = INT_PDA_Util.ClampI(rs.ToInt(), 3, 300);
		int defMin = INT_PDA_Util.ClampI(ds.ToInt(), 0, 525600);
		INT_PDA_Sfx.Play("Click");
		SendSettings(INT_PDA_Client.s_Settings.NearbyRadius, rot, defMin);
	}

	// =================================================================================================================
	// system
	// =================================================================================================================
	protected void RenderSystem()
	{
		if (!m_SysBox)
			return;
		float rad = INT_PDA_Client.s_Settings.NearbyRadius;
		string rs = RadiusText(rad);
		string tNow = Widget.TranslateString("#STR_INT_PDA_ADMIN_SYS_NOW");
		string tM = Widget.TranslateString("#STR_INT_PDA_ADMIN_SYS_M");
		string nowText = tNow + ": " + rs + " " + tM;
		m_RadiusNow.SetText(nowText);
		SetInfo(0, INT_PDA_Const.VERSION, INT_PDA_Look.TEXT);
		int protoVer = INT_PDA_Const.PROTOCOL;
		SetInfo(1, protoVer.ToString(), INT_PDA_Look.TEXT);
		string me = INT_PDA_Client.MySteam();
		if (me == "")
			me = "—";
		SetInfo(2, me, INT_PDA_Look.TEXT);
		string acc = "—";
		if (INT_PDA_Client.s_LoggedIn && INT_PDA_Client.s_KnownCallsign != "")
			acc = INT_PDA_Client.s_KnownCallsign;
		acc = FitText(acc, 304, 28);
		SetInfo(3, acc, INT_PDA_Look.TEXT);
		string modeText = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_SYS_OFF");
		int modeC = INT_PDA_Look.DIM;
		if (INT_PDA_Client.IsAdminMode())
		{
			modeText = INT_PDA_Look.Caps("#STR_INT_PDA_MOD_ON");
			modeC = INT_PDA_Look.ALLY;
		}
		SetInfo(4, modeText, modeC);
		string rightsText = INT_PDA_Look.Caps("#STR_INT_PDA_MOD_OFF");
		int rightsC = INT_PDA_Look.ENEMY;
		if (INT_PDA_Client.s_IsAdmin)
		{
			rightsText = INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_SYS_YES");
			rightsC = INT_PDA_Look.ALLY;
		}
		SetInfo(5, rightsText, rightsC);
	}

	protected void SetInfo(int i, string value, int color)
	{
		if (i < 0 || i >= m_InfoVal.Count())
			return;
		m_InfoVal[i].SetText(value);
		m_InfoVal[i].SetColor(color);
	}

	// The field shows the server's radius unless the admin typed a value he has not saved yet (a reload / WELCOME
	// answer must not wipe it); force = the saved value just came back.
	protected void FillRadiusInput(bool force = false)
	{
		if (!m_RadiusIn)
			return;
		if (m_RadiusDirty && !force)
			return;
		m_RadiusDirty = false;
		float rad = INT_PDA_Client.s_Settings.NearbyRadius;
		string rs = RadiusText(rad);
		m_RadiusIn.SetText(rs);
	}

	protected void OnRadiusTyped(string text, INT_UIF_TextInput t)
	{
		m_RadiusDirty = true;
	}

	protected string RadiusText(float r)
	{
		int ri = (int)Math.Round(r);
		if (Math.AbsFloat(r - ri) < 0.05)
			return ri.ToString();
		return r.ToString();
	}

	protected void SubmitRadius()
	{
		if (!m_RadiusIn)
			return;
		string s = m_RadiusIn.GetText();
		s.TrimInPlace();
		s.Replace(",", ".");
		float v = s.ToFloat();
		if (s == "" || v < 1 || v > 50)
		{
			INT_UIF_Toast.Error("#STR_INT_PDA_ADMIN_SYS_RADIUS_BAD");
			INT_PDA_Sfx.Play("Error");
			return;
		}
		INT_PDA_Sfx.Play("Click");
		SendRadius(v);
	}

	protected void OnRadiusSubmit(string text, INT_UIF_TextInput t)
	{
		SubmitRadius();
	}

	protected void OnRadiusClick(INT_UIF_Button b)
	{
		SubmitRadius();
	}

	protected void OnReloadDbClick(INT_UIF_Button b)
	{
		AskConfirm(CF_RELOAD_DB, "#STR_INT_PDA_ADMIN_CF_RELOAD_T", "#STR_INT_PDA_ADMIN_CF_RELOAD_DB", "#STR_INT_PDA_ADMIN_SYS_RELOAD_BTN", false, "", "", 0, "");
	}

	protected void OnReloadFactionsClick(INT_UIF_Button b)
	{
		AskConfirm(CF_RELOAD_FACTIONS, "#STR_INT_PDA_ADMIN_CF_RELOAD_T", "#STR_INT_PDA_ADMIN_CF_RELOAD_FACTIONS", "#STR_INT_PDA_ADMIN_SYS_RELOAD_BTN", false, "", "", 0, "");
	}

	protected void OnReloadConfigClick(INT_UIF_Button b)
	{
		AskConfirm(CF_RELOAD_CONFIG, "#STR_INT_PDA_ADMIN_CF_RELOAD_T", "#STR_INT_PDA_ADMIN_CF_RELOAD_CONFIG", "#STR_INT_PDA_ADMIN_SYS_RELOAD_BTN", false, "", "", 0, "");
	}

	// =================================================================================================================
	// widget events (hit zones of this page)
	// =================================================================================================================
	override bool OnClick(Widget w, int x, int y, int button)
	{
		int i;
		for (i = 0; i < m_SubHits.Count(); i++)
		{
			if (w == m_SubHits[i])
			{
				SelectSub(i, true, false);
				return true;
			}
		}
		if (w == m_RefreshHit)
		{
			INT_PDA_Sfx.Play("Click");
			RefreshCurrent();
			return true;
		}
		if (w == m_EyeHit)
		{
			m_ShowPass = !m_ShowPass;
			INT_PDA_Sfx.Play("Toggle");
			RenderDetail();
			return true;
		}
		for (i = 0; i < m_PRows.Count(); i++)
		{
			if (w == m_PRows[i].m_Hit)
			{
				PickRow(i);
				return true;
			}
		}
		for (i = 0; i < m_NRows.Count(); i++)
		{
			if (w == m_NRows[i].m_TrashHit)
			{
				AskNewsDelete(i);
				return true;
			}
			if (w == m_NRows[i].m_Hit)
			{
				StartNewsEdit(i);
				return true;
			}
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int i;
		for (i = 0; i < m_SubHits.Count(); i++)
		{
			if (w == m_SubHits[i] && i != m_Sub)
				m_SubText[i].SetColor(INT_PDA_Look.TEXT);
		}
		if (w == m_RefreshHit)
			m_RefreshText.SetColor(INT_PDA_Look.ORANGE);
		if (w == m_EyeHit)
			m_EyeIcon.SetColor(INT_PDA_Look.ORANGE);
		for (i = 0; i < m_PRows.Count(); i++)
		{
			if (w == m_PRows[i].m_Hit)
			{
				m_HoverRow = i;
				PaintRow(i);
			}
		}
		for (i = 0; i < m_NRows.Count(); i++)
		{
			if (w == m_NRows[i].m_Hit)
			{
				m_NewsHover = i;
				PaintNewsRow(i);
			}
			else if (w == m_NRows[i].m_TrashHit)
			{
				m_TrashHover = i;
				PaintNewsRow(i);
			}
		}
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int i;
		for (i = 0; i < m_SubHits.Count(); i++)
		{
			if (w == m_SubHits[i] && i != m_Sub)
				m_SubText[i].SetColor(INT_PDA_Look.DIM);
		}
		if (w == m_RefreshHit)
			m_RefreshText.SetColor(INT_PDA_Look.DIM);
		if (w == m_EyeHit)
			m_EyeIcon.SetColor(INT_PDA_Look.DIM);
		for (i = 0; i < m_PRows.Count(); i++)
		{
			if (w == m_PRows[i].m_Hit && m_HoverRow == i)
			{
				m_HoverRow = -1;
				PaintRow(i);
			}
		}
		for (i = 0; i < m_NRows.Count(); i++)
		{
			if (w == m_NRows[i].m_Hit && m_NewsHover == i)
			{
				m_NewsHover = -1;
				PaintNewsRow(i);
			}
			else if (w == m_NRows[i].m_TrashHit && m_TrashHover == i)
			{
				m_TrashHover = -1;
				PaintNewsRow(i);
			}
		}
		return false;
	}

	// Whole-row scrolling of the players page and of the news feed (one notch = one row).
	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		int i;
		bool players = false;
		if (w == m_TableHit)
			players = true;
		for (i = 0; i < m_PRows.Count(); i++)
		{
			if (w == m_PRows[i].m_Hit)
				players = true;
		}
		if (players)
		{
			m_First = m_First - wheel;
			RenderPlayers();
			return true;
		}
		bool news = false;
		if (w == m_NewsAllHit)
			news = true;
		for (i = 0; i < m_NRows.Count(); i++)
		{
			if (w == m_NRows[i].m_Hit || w == m_NRows[i].m_TrashHit)
				news = true;
		}
		if (news)
		{
			m_NewsFirst = m_NewsFirst - wheel;
			RenderNews();
			return true;
		}
		return false;
	}

	// =================================================================================================================
	// text helpers
	// =================================================================================================================
	// Text never leaves its column (frames do not clip text): cut by the estimated glyph width (UIF TextWidth rule).
	protected string FitText(string s, float width, float size)
	{
		return INT_PDA_Look.Fit(s, width, size);
	}

	protected string OrDash(string s)
	{
		if (s == "")
			return "—";
		return s;
	}

	protected string RoleText(INT_PDA_AdminRow r)
	{
		if (!r || r.FactionId == "")
			return "—";
		string key = INT_PDA_Look.RoleKey(r.Role);
		return Widget.TranslateString(key);
	}

	// "YYYY-MM-DD HH:MM:SS" (server Stamp) -> "DD.MM.YY HH:MM" (table) or "DD.MM.YYYY HH:MM" (card).
	protected string StampText(string stamp, bool fullYear)
	{
		if (stamp == "")
			return "—";
		if (stamp.Length() < 16)
			return stamp;
		string d = stamp.Substring(8, 2) + "." + stamp.Substring(5, 2) + ".";
		if (fullYear)
			d = d + stamp.Substring(0, 4);
		else
			d = d + stamp.Substring(2, 2);
		return d + " " + stamp.Substring(11, 5);
	}

	protected string MaskPassword(string pw)
	{
		int n = pw.LengthUtf8();
		if (n == 0)
			return "—";
		if (n > 16)
			n = 16;
		string s = "";
		int i;
		for (i = 0; i < n; i++)
			s = s + "•";
		return s;
	}

	// =================================================================================================================
	// self-test
	// =================================================================================================================
	override void OnTestCommand(string cmd)
	{
		if (cmd.IndexOf("adm ") != 0)
			return;
		m_TestMode = true;
		string shown = INT_PDA_Log.Clip(cmd, 120);
		INT_PDA_Log.Info("админ-тест: команда «" + shown + "»");
		string rest = cmd.Substring(4, cmd.Length() - 4);
		rest.TrimInPlace();
		string fwd = "";
		if (rest == "f" || rest.IndexOf("f ") == 0)
		{
			if (rest.Length() > 2)
				fwd = rest.Substring(2, rest.Length() - 2);
			fwd.TrimInPlace();
			if (m_Sub != SUB_FACTIONS)
				SelectSub(SUB_FACTIONS, false, false);
			if (m_Factions)
				m_Factions.OnTestCommand(fwd);
			return;
		}
		string verb = rest;
		string arg = "";
		int sp = rest.IndexOf(" ");
		if (sp > 0)
		{
			verb = rest.Substring(0, sp);
			arg = rest.Substring(sp + 1, rest.Length() - sp - 1);
			arg.TrimInPlace();
		}
		array<string> parts = new array<string>();
		arg.Split(" ", parts);
		string p0 = "";
		string p1 = "";
		if (parts.Count() > 0)
			p0 = parts[0];
		if (parts.Count() > 1)
			p1 = parts[1];
		string fid;
		int num = arg.ToInt();
		float numF = arg.ToFloat();
		int ps;
		int bar;
		string title;
		string body;
		if (verb == "tab")
		{
			SelectSub(num, true, false);
		}
		else if (verb == "players")
		{
			SelectSub(SUB_PLAYERS, false, false);
			if (m_Search)
				m_Search.SetText(arg);
			DoSearch(arg);
		}
		else if (verb == "page")
		{
			SelectSub(SUB_PLAYERS, false, false);
			if (num < 1)
				num = 1;
			ps = PageSize();
			RequestPlayers((num - 1) * ps);
		}
		else if (verb == "select")
		{
			SelectSub(SUB_PLAYERS, false, false);
			if (!SelectBySteam(arg))
			{
				m_AutoSelect = arg;
				m_Filter = arg;
				if (m_Search)
					m_Search.SetText(arg);
				m_First = 0;
				RequestPlayers(0);
			}
		}
		else if (verb == "pass")
		{
			SelectSub(SUB_PLAYERS, false, false);
			if (p0 != "" && p1 != "")
				SendPass(p0, p1);
		}
		else if (verb == "del")
		{
			SelectSub(SUB_PLAYERS, false, false);
			if (arg != "")
				SendDelete(arg);
		}
		else if (verb == "role")
		{
			SelectSub(SUB_PLAYERS, false, false);
			fid = FactionOf(p0);
			num = p1.ToInt();
			if (p1 == "")
				num = -1;
			if (p0 == "" || p1 == "")
				INT_PDA_Log.Warn("админ-тест: формат «adm role <steam> <0-3>»");
			else if (fid == "")
				INT_PDA_Log.Warn("админ-тест: " + p0 + " нет в загруженной странице или он вне группировок");
			else if (num <= INT_PDA_Const.ROLE_NONE)
				SendKick(fid, p0);
			else
				SendRole(fid, p0, num);
		}
		else if (verb == "news")
		{
			SelectSub(SUB_NEWS, false, false);
			bar = arg.IndexOf("|");
			if (bar > 0)
			{
				title = arg.Substring(0, bar);
				body = arg.Substring(bar + 1, arg.Length() - bar - 1);
				title.TrimInPlace();
				body.TrimInPlace();
				SendNewsPost(title, body);
			}
			else
			{
				INT_PDA_Log.Warn("админ-тест: формат «adm news <заголовок>|<текст>»");
			}
		}
		else if (verb == "newsdel")
		{
			SelectSub(SUB_NEWS, false, false);
			SendNewsDel(num);
		}
		else if (verb == "radius")
		{
			SelectSub(SUB_SYSTEM, false, false);
			SendRadius(numF);
		}
		else if (verb == "reload")
		{
			SelectSub(SUB_SYSTEM, false, false);
			if (arg == "db")
				SendReload(INT_PDA_Const.ADM_RELOAD_DB);
			else if (arg == "factions")
				SendReload(INT_PDA_Const.ADM_RELOAD_FACTIONS);
			else if (arg == "config")
				SendReload(INT_PDA_Const.ADM_RELOAD_CONFIG);
			else
				INT_PDA_Log.Warn("админ-тест: reload db|factions|config");
		}
		else
		{
			shown = INT_PDA_Log.Clip(verb, 24);
			INT_PDA_Log.Warn("админ-тест: неизвестная команда " + shown);
		}
	}
}
