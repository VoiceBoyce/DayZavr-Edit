// INT_PDA (client) — «Администрация» → «Группировки» (owner brief, admin items 11-18; spec.md §3, §5). Left: every
// faction (emblem, name, members / online, leader) + «Создать группировку». Right: the editor of the picked faction in
// four sections — «Основное» (name in any language, unique; emblem from the 45 squares with the live vertical card;
// motto; base; spawn point «x, y, z» + «Моя позиция»; save / delete), «Состав и роли» (callsign, Steam ID, rank, role:
// member / deputy / leader / kick; «Вступить самому»), «Ранги» (add, rename, delete, move; the first one goes to a
// newcomer), «Отношения» (alliance / neutral / hostile + RP reason for every other faction).
// Hosted by INT_PDA_PageAdmin: it builds this into its body and forwards Show / OnChanged / OnResult / Update /
// HandleEscape / OnTestCommand (prefix «adm f » stripped). Every request is OP_ADMIN + ADM_* written exactly in the order
// INT_PDA_SE_Admin reads it. The server silently drops a request that comes sooner than REQUEST_GAP_MS after the previous
// one of the same player, so ours leave through a small paced queue (GAP_MS); replies are matched to our own requests
// (other admin sub-tabs use the same reply channel).
//
// Test commands (after «adm f »): list | select <id> | new <name…> (creates at once) | save | del <id> (no prompt) |
// emblem <1-45> | role <steam> <0-3> (0 = kick) | rank add <name…> | rank del <name…> (both save at once) |
// rank ren <old>|<new> (ADM_RANK_RENAME for a saved rank: the holders keep it) |
// rel <idA> <idB> <0|1|2> <reason…> (0 neutral, 1 ally, 2 hostile) | join <id> | tab <0-3> | pop [close] | state.
// Log markers: «ADMF cmd <verb>», «ADMF reply <sub> <code>», «ADMF state …».

class INT_PDA_AdminFactions : ScriptedWidgetEventHandler
{
	static const int L_FAC = 0;					// pooled lists
	static const int L_MEM = 1;
	static const int L_RANK = 2;
	static const int L_REL = 3;
	static const int SEC_MAIN = 0;				// editor sections
	static const int SEC_MEMBERS = 1;
	static const int SEC_RANKS = 2;
	static const int SEC_RELATIONS = 3;
	static const int FAC_ROWS = 7;
	static const int MEM_ROWS = 7;
	static const int RANK_ROWS = 8;
	static const int REL_ROWS = 6;
	static const float FAC_ROW_H = 62;
	static const float MEM_ROW_H = 48;
	static const float RANK_ROW_H = 42;
	static const float REL_ROW_H = 64;
	static const float FAC_TOP = 102;
	static const float MEM_TOP = 62;
	static const float RANK_TOP = 84;
	static const float REL_TOP = 28;
	static const float LEFT_W = 400;
	static const float ED_X = 420;
	static const float TAB_Y = 78;
	static const float SEC_Y = 132;
	static const float CARD_W = 168;
	static const float POP_CARD_W = 212;
	static const int EMB_COLS = 9;
	static const float EMB_CELL = 64;
	static const float EMB_GAP = 8;
	static const int GAP_MS = 400;				// between two of our requests (server drops < REQUEST_GAP_MS = 250)
	static const int LIST_GAP_MS = 3000;		// re-ask the list / the view on Show at most this often
	static const int AWAIT_MS = 15000;			// a reply that never came is forgotten after this
	static const int LOAD_GIVEUP_MS = 10000;
	static const int QUEUE_MAX = 24;
	static const int RETRY_MAX = 2;				// resends of a request the server's gate answered «too fast»
	static const int RETRY_HOLD_MS = 600;		// on top of GAP_MS before such a resend
	static const int RANKS_MAX = 32;			// INT_PDA_SE_Admin.OnRanks: ReadCount(…, 32)
	static const int NAME_CHARS = 32;			// INT_PDA_FactionData.Clean / OnFactionSave: CleanLine(name, 32, 80)
	static const int MOTTO_CHARS = 120;			// CleanLine(Motto, 120, 400)
	static const int BASE_CHARS = 60;			// CleanLine(Base, 60, 200)
	static const int RANK_CHARS = 24;			// CleanLine(r, 24, 80)
	static const int REASON_CHARS = 120;		// CleanLine(reason, 120, 400)
	static const int SPAWN_CHARS = 48;

	protected INT_PDA_Page m_Owner;
	protected INT_PDA_Menu m_Menu;
	protected INT_UIF_Bag m_Bag;
	protected Widget m_Root;
	protected float m_W;
	protected float m_H;
	protected float m_EW;
	protected bool m_Shown;
	protected bool m_Filling;					// inputs are being filled by code (ignore their change events)
	protected float m_Tick;

	// --- requests ---------------------------------------------------------------------------------------------------
	protected ref array<ref ScriptRPC> m_Queue = new array<ref ScriptRPC>();
	protected ref array<int> m_QueueSub = new array<int>();
	protected ref array<string> m_QueueKey = new array<string>();	// "list", "view:<id>", "rel:<other id>", "" = none
	protected ref array<int> m_QueueTag = new array<int>();		// edit serial the request was built from (0 = none)
	protected ref array<int> m_QueueTry = new array<int>();		// «too fast» resends already spent
	protected ref array<int> m_AwaitSub = new array<int>();
	protected ref array<int> m_AwaitMs = new array<int>();
	protected ref array<string> m_AwaitKey = new array<string>();
	protected ref array<int> m_AwaitTag = new array<int>();
	protected int m_LastSendMs = -100000;
	protected int m_ListAskMs = -100000;
	protected int m_ViewAskMs = -100000;
	protected bool m_ResultSeen;				// OnResult handled the reply, the following CH_ADMIN must not again
	protected bool m_SaveWasNew;
	protected bool m_SaveBusy;
	protected int m_SaveBusyMs;
	protected string m_DeletingId = "";
	protected string m_KickSteam = "";
	protected int m_RenameIdx = -1;
	protected string m_RenameName = "";		// the rank the rename prompt is about (found again by name)
	protected string m_JoinId = "";
	// the request on its way (resent when the server's gate answers «too fast»; ScriptRPC.Send keeps its data)
	protected ref ScriptRPC m_SentRpc;
	protected int m_SentSub;
	protected string m_SentKey = "";
	protected int m_SentTag;
	protected int m_SentTry;
	// edit serials: an answer clears «не сохранено» only when nothing was edited after that save left
	protected int m_MainEdits;
	protected int m_RankEdits;
	protected ref array<int> m_RelEdit = new array<int>();		// per relation row (parallel to m_RelIds)
	// rank renames on their way (ADM_RANK_RENAME, answered in order): a refused one is undone in the local list
	protected ref array<string> m_RenFac = new array<string>();
	protected ref array<string> m_RenOld = new array<string>();
	protected ref array<string> m_RenNew = new array<string>();
	// what every visible row showed when it was drawn (a dropdown pick acts on THAT member / faction)
	protected ref array<string> m_MRowSteam = new array<string>();
	protected ref array<string> m_ERowId = new array<string>();

	// --- selection and editor data ----------------------------------------------------------------------------------
	protected string m_SelId = "";
	protected bool m_New;
	protected bool m_Loading;
	protected bool m_Pulsing;
	protected ref INT_PDA_FactionView m_View;	// the view shown (same object as INT_PDA_Client.s_AdmFaction when fresh)
	protected int m_Emblem = 1;
	protected bool m_MainDirty;
	protected ref array<string> m_Ranks = new array<string>();
	protected bool m_RanksDirty;
	protected ref array<string> m_RelIds = new array<string>();
	protected ref array<string> m_RelNames = new array<string>();
	protected ref array<int> m_RelEmblems = new array<int>();
	protected ref array<int> m_RelVals = new array<int>();
	protected ref array<string> m_RelReasons = new array<string>();
	protected ref array<bool> m_RelDirty = new array<bool>();
	protected ref array<int> m_MemOrder = new array<int>();	// indices into m_View.Members: leader, deputies, members
	protected ref array<int> m_First = new array<int>();		// first shown row of every pooled list
	protected int m_Sec;

	// --- shared widgets ---------------------------------------------------------------------------------------------
	protected ref array<ButtonWidget> m_IcHit = new array<ButtonWidget>();	// clickable icons
	protected ref array<ImageWidget> m_IcImg = new array<ImageWidget>();
	protected ref array<bool> m_IcOn = new array<bool>();
	protected ref array<int> m_IcBase = new array<int>();
	protected ref array<int> m_PgPrev = new array<int>();		// pager icon per list
	protected ref array<int> m_PgNext = new array<int>();
	protected ref array<TextWidget> m_PgText = new array<TextWidget>();
	protected ref array<ButtonWidget> m_AreaHit = new array<ButtonWidget>();	// wheel area per list

	// --- left: faction list -----------------------------------------------------------------------------------------
	protected Widget m_Left;
	protected TextWidget m_LCount;
	protected TextWidget m_LEmpty;
	protected INT_UIF_Button m_CreateBtn;
	protected int m_RefreshIc = -1;
	protected ref array<Widget> m_LRow = new array<Widget>();
	protected ref array<ref INT_UIF_Surface> m_LBox = new array<ref INT_UIF_Surface>();
	protected ref array<ImageWidget> m_LBar = new array<ImageWidget>();
	protected ref array<ImageWidget> m_LEmb = new array<ImageWidget>();
	protected ref array<TextWidget> m_LName = new array<TextWidget>();
	protected ref array<TextWidget> m_LSub = new array<TextWidget>();
	protected ref array<ButtonWidget> m_LHit = new array<ButtonWidget>();

	// --- right: editor ----------------------------------------------------------------------------------------------
	protected Widget m_Editor;
	protected Widget m_EdEmpty;
	protected Widget m_EdBody;
	protected ImageWidget m_HEmb;
	protected TextWidget m_HName;
	protected TextWidget m_HSub;
	protected TextWidget m_HState;
	protected ref array<TextWidget> m_SecText = new array<TextWidget>();
	protected ref array<ButtonWidget> m_SecHit = new array<ButtonWidget>();
	protected ref array<float> m_SecX = new array<float>();
	protected ref array<float> m_SecW = new array<float>();
	protected ref array<Widget> m_SecFrame = new array<Widget>();
	protected ImageWidget m_SecLine;
	// main
	protected Widget m_CardHost;
	protected TextWidget m_EmbNo;
	protected TextWidget m_IdText;
	protected INT_UIF_Button m_EmbBtn;
	protected INT_UIF_TextInput m_InName;
	protected INT_UIF_TextInput m_InMotto;
	protected INT_UIF_TextInput m_InBase;
	protected INT_UIF_TextInput m_InSpawn;
	protected INT_UIF_Button m_PosBtn;
	protected INT_UIF_Button m_SaveBtn;
	protected INT_UIF_Button m_DelBtn;
	// members
	protected TextWidget m_MCaption;
	protected TextWidget m_MEmpty;
	protected INT_UIF_Button m_JoinBtn;
	protected ref array<Widget> m_MRow = new array<Widget>();
	protected ref array<ImageWidget> m_MDot = new array<ImageWidget>();
	protected ref array<ImageWidget> m_MRoleIc = new array<ImageWidget>();
	protected ref array<TextWidget> m_MName = new array<TextWidget>();
	protected ref array<TextWidget> m_MSteam = new array<TextWidget>();
	protected ref array<TextWidget> m_MRank = new array<TextWidget>();
	protected ref array<TextWidget> m_MJoined = new array<TextWidget>();
	protected ref array<ButtonWidget> m_MHit = new array<ButtonWidget>();
	protected ref array<INT_UIF_Dropdown> m_MRole = new array<INT_UIF_Dropdown>();
	// ranks
	protected TextWidget m_RCaption;
	protected TextWidget m_RState;
	protected TextWidget m_REmpty;
	protected INT_UIF_TextInput m_InRank;
	protected INT_UIF_Button m_RankAddBtn;
	protected INT_UIF_Button m_RankSaveBtn;
	protected INT_UIF_Button m_RankResetBtn;
	protected ref array<Widget> m_RRow = new array<Widget>();
	protected ref array<TextWidget> m_RNum = new array<TextWidget>();
	protected ref array<TextWidget> m_RName = new array<TextWidget>();
	protected ref array<TextWidget> m_RUse = new array<TextWidget>();
	protected ref array<ButtonWidget> m_RHit = new array<ButtonWidget>();
	protected ref array<int> m_RIc = new array<int>();			// 4 icon indices per row: up, down, rename, delete
	// relations
	protected TextWidget m_ECaption;
	protected TextWidget m_EEmpty;
	protected ref array<Widget> m_ERow = new array<Widget>();
	protected ref array<ImageWidget> m_EBar = new array<ImageWidget>();
	protected ref array<ImageWidget> m_EEmb = new array<ImageWidget>();
	protected ref array<TextWidget> m_EName = new array<TextWidget>();
	protected ref array<TextWidget> m_EState = new array<TextWidget>();
	protected ref array<ButtonWidget> m_EHit = new array<ButtonWidget>();
	protected ref array<INT_UIF_Dropdown> m_ERel = new array<INT_UIF_Dropdown>();
	protected ref array<INT_UIF_TextInput> m_EReason = new array<INT_UIF_TextInput>();
	protected ref array<INT_UIF_Button> m_ESave = new array<INT_UIF_Button>();

	// --- emblem popup -----------------------------------------------------------------------------------------------
	protected Widget m_Pop;
	protected Widget m_PopBox;
	protected ButtonWidget m_PopBack;
	protected ButtonWidget m_PopBoxHit;
	protected Widget m_PopCard;
	protected TextWidget m_PopCount;
	protected Widget m_PopSel;
	protected Widget m_PopHov;
	protected INT_UIF_Button m_PopDone;
	protected ref array<ButtonWidget> m_PopHits = new array<ButtonWidget>();
	protected bool m_PopOpen;
	protected float m_PopX;
	protected float m_PopY;
	protected float m_PopCardX;

	// ================================================================================================================
	// build
	// ================================================================================================================
	void Build(INT_PDA_Page owner, Widget parent, float w, float h)
	{
		int i;
		m_Owner = owner;
		if (owner)
		{
			m_Menu = owner.m_Menu;
			m_Bag = owner.m_Bag;
		}
		m_W = w;
		m_H = h;
		m_EW = w - ED_X;
		m_First.Clear();
		m_PgPrev.Clear();
		m_PgNext.Clear();
		m_PgText.Clear();
		m_AreaHit.Clear();
		for (i = 0; i < 4; i++)
		{
			m_First.Insert(0);
			m_PgPrev.Insert(-1);
			m_PgNext.Insert(-1);
			m_PgText.Insert(null);
			m_AreaHit.Insert(null);
		}
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, w, h, "INT_PDA_AdmFactions");
		BuildList();
		BuildEditor();
		BuildEmblemPopup();
		RefreshFactions();
		RefreshEditor();
		m_Root.Show(false);
	}

	protected ButtonWidget Hit(Widget parent, float x, float y, float w, float h)
	{
		return INT_UIF_Draw.Hit(parent, x, y, w, h, this);
	}

	protected TextWidget FieldCaption(Widget parent, float x, float y, float w, string key)
	{
		string cap = INT_PDA_Look.Caps(key);
		return INT_UIF_Draw.Text(parent, x, y, w, 18, cap, 13, INT_PDA_Look.DIM);
	}

	// A clickable icon: 32 x 32 hit, 18 px glyph. uif = glyph of the UIF atlas (chevrons) instead of int_pda_icons.
	protected int IconBtn(Widget parent, float x, float y, string glyph, bool uif, int color)
	{
		ImageWidget img;
		if (uif)
			img = INT_UIF_Draw.Icon(parent, x + 7, y + 7, 18, glyph, color);
		else
			img = INT_PDA_Look.DrawIcon(parent, x + 7, y + 7, 18, glyph, color);
		ButtonWidget zone = Hit(parent, x, y, 32, 32);
		m_IcHit.Insert(zone);
		m_IcImg.Insert(img);
		m_IcOn.Insert(true);
		m_IcBase.Insert(color);
		return m_IcHit.Count() - 1;
	}

	protected void SetIconOn(int idx, bool on)
	{
		if (idx < 0 || idx >= m_IcImg.Count())
			return;
		m_IcOn.Set(idx, on);
		int c = m_IcBase[idx];
		if (!on)
			c = INT_PDA_Look.LINE2;
		ImageWidget img = m_IcImg[idx];
		if (img)
			img.SetColor(c);
	}

	protected void ShowIcon(int idx, bool show)
	{
		if (idx < 0 || idx >= m_IcImg.Count())
			return;
		ImageWidget img = m_IcImg[idx];
		ButtonWidget zone = m_IcHit[idx];
		if (img)
			img.Show(show);
		if (zone)
			zone.Show(show);
	}

	protected void BuildPager(int list, Widget parent, float x, float y, float w)
	{
		int prev = IconBtn(parent, x, y, "chevron_left", true, INT_PDA_Look.TEXT);
		int next = IconBtn(parent, x + w - 32, y, "chevron_right", true, INT_PDA_Look.TEXT);
		TextWidget t = INT_UIF_Draw.Text(parent, x + 36, y, w - 72, 32, "", 14, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
		m_PgPrev.Set(list, prev);
		m_PgNext.Set(list, next);
		m_PgText.Set(list, t);
	}

	// --- left column ------------------------------------------------------------------------------------------------
	protected void BuildList()
	{
		int i;
		float ry;
		float rw = LEFT_W - 16;
		float rh = FAC_ROW_H - 4;
		Widget row;
		INT_UIF_Surface box;
		ButtonWidget area;
		string lbl;
		m_Left = INT_UIF_Draw.Frame(m_Root, 0, 0, LEFT_W, m_H, "INT_PDA_AdmFList");
		INT_UIF_Surface.Box(m_Left, 0, 0, LEFT_W, m_H, 6, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
		INT_UIF_Draw.Line(m_Left, 0, 10, 3, 26, INT_PDA_Look.ADMIN);
		INT_PDA_Look.DrawIcon(m_Left, 18, 13, 18, "flag", INT_PDA_Look.ADMIN);
		FieldCaption(m_Left, 44, 13, 190, "#STR_INT_PDA_AF_LIST");
		m_LCount = INT_UIF_Draw.Text(m_Left, 240, 13, 100, 18, "", 13, INT_PDA_Look.ORANGE, INT_UIF_Align.RIGHT);
		m_RefreshIc = IconBtn(m_Left, LEFT_W - 46, 6, "refresh", false, INT_PDA_Look.DIM);
		lbl = INT_PDA_Look.Caps("#STR_INT_PDA_AF_CREATE");
		m_CreateBtn = INT_UIF_Button.Create(m_Bag, m_Left, 16, 46, LEFT_W - 32, 44, lbl, INT_UIF_Variant.PRIMARY, "plus");
		if (m_CreateBtn)
			m_CreateBtn.OnClick().Insert(OnCreateClick);
		area = Hit(m_Left, 8, FAC_TOP, rw, FAC_ROWS * FAC_ROW_H);
		m_AreaHit.Set(L_FAC, area);
		for (i = 0; i < FAC_ROWS; i++)
		{
			ry = FAC_TOP + i * FAC_ROW_H;
			row = INT_UIF_Draw.Frame(m_Left, 8, ry, rw, rh, "INT_PDA_AdmFRow");
			box = INT_UIF_Surface.Box(row, 0, 0, rw, rh, 4, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
			m_LRow.Insert(row);
			m_LBox.Insert(box);
			m_LBar.Insert(INT_UIF_Draw.Line(row, 0, 0, 3, rh, INT_PDA_Look.ORANGE));
			m_LEmb.Insert(INT_PDA_Look.DrawEmblem(row, 12, 6, 46, 1));
			m_LName.Insert(INT_UIF_Draw.Text(row, 70, 5, rw - 84, 26, "", 19, INT_PDA_Look.TEXT));
			m_LSub.Insert(INT_UIF_Draw.Text(row, 70, 33, rw - 84, 18, "", 13, INT_PDA_Look.DIM));
			m_LHit.Insert(Hit(row, 0, 0, rw, rh));
		}
		m_LEmpty = INT_UIF_Draw.Text(m_Left, 0, FAC_TOP + 70, LEFT_W, 26, "#STR_INT_PDA_AF_LIST_EMPTY", 16, INT_PDA_Look.DIM2, INT_UIF_Align.CENTER);
		BuildPager(L_FAC, m_Left, 16, m_H - 46, LEFT_W - 32);
	}

	// --- right column -----------------------------------------------------------------------------------------------
	protected void BuildEditor()
	{
		int i;
		float tx = 0;
		float tw;
		float lx;
		float lw;
		float cy = m_H * 0.5 - 90;
		string cap;
		TextWidget t;
		Widget embBox;
		array<string> keys = {"#STR_INT_PDA_AF_SEC_MAIN", "#STR_INT_PDA_AF_SEC_MEMBERS", "#STR_INT_PDA_AF_SEC_RANKS", "#STR_INT_PDA_AF_SEC_RELATIONS"};
		m_Editor = INT_UIF_Draw.Frame(m_Root, ED_X, 0, m_EW, m_H, "INT_PDA_AdmFEditor");
		// nothing picked yet
		m_EdEmpty = INT_UIF_Draw.Frame(m_Editor, 0, 0, m_EW, m_H, "INT_PDA_AdmFNone");
		INT_PDA_Look.DrawIcon(m_EdEmpty, m_EW * 0.5 - 32, cy, 64, "flag", INT_PDA_Look.DIM2);
		cap = INT_PDA_Look.Caps("#STR_INT_PDA_AF_NONE_TITLE");
		INT_UIF_Draw.Text(m_EdEmpty, 0, cy + 84, m_EW, 30, cap, 22, INT_PDA_Look.TEXT, INT_UIF_Align.CENTER);
		INT_UIF_Draw.Text(m_EdEmpty, 0, cy + 124, m_EW, 24, "#STR_INT_PDA_AF_NONE_TEXT", 16, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
		// header: emblem, name, id / members / online, state
		m_EdBody = INT_UIF_Draw.Frame(m_Editor, 0, 0, m_EW, m_H, "INT_PDA_AdmFBody");
		embBox = INT_UIF_Draw.Frame(m_EdBody, 0, 2, 64, 64, "INT_PDA_AdmFHEmb");
		INT_UIF_Surface.Box(embBox, 0, 0, 64, 64, 6, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE2);
		m_HEmb = INT_PDA_Look.DrawEmblem(embBox, 4, 4, 56, 1);
		INT_UIF_Draw.Line(embBox, 4, 60, 56, 3, INT_PDA_Look.ORANGE);
		m_HName = INT_UIF_Draw.Text(m_EdBody, 82, -2, m_EW - 420, 44, "", 34, INT_PDA_Look.TEXT);
		m_HSub = INT_UIF_Draw.Text(m_EdBody, 84, 44, m_EW - 420, 20, "", 14, INT_PDA_Look.DIM);
		m_HState = INT_UIF_Draw.Text(m_EdBody, m_EW - 330, 10, 330, 18, "", 13, INT_PDA_Look.ORANGE, INT_UIF_Align.RIGHT);
		INT_PDA_Look.DrawIcon(m_EdBody, m_EW - 18, 41, 16, "admin", INT_PDA_Look.ADMIN);
		cap = INT_PDA_Look.Caps("#STR_INT_PDA_AF_ADMIN_EDIT");
		INT_UIF_Draw.Text(m_EdBody, m_EW - 330, 40, 304, 18, cap, 13, INT_PDA_Look.ADMIN, INT_UIF_Align.RIGHT);
		// section words with the sliding orange line (like the top tabs)
		for (i = 0; i < 4; i++)
		{
			cap = INT_PDA_Look.Caps(keys[i]);
			tw = INT_PDA_Look.TextW(cap, 18) + 4;
			t = INT_UIF_Draw.Text(m_EdBody, tx, TAB_Y, tw, 36, cap, 18, INT_PDA_Look.DIM);
			m_SecText.Insert(t);
			m_SecHit.Insert(Hit(m_EdBody, tx - 8, TAB_Y, tw + 16, 40));
			m_SecX.Insert(tx);
			m_SecW.Insert(tw);
			tx = tx + tw + 36;
		}
		INT_UIF_Draw.Line(m_EdBody, 0, TAB_Y + 41, m_EW, 1, INT_PDA_Look.LINE);
		lx = m_SecX[0];
		lw = m_SecW[0];
		m_SecLine = INT_UIF_Draw.Line(m_EdBody, lx, TAB_Y + 38, lw, 3, INT_PDA_Look.ORANGE);
		m_SecFrame.Insert(BuildMain());
		m_SecFrame.Insert(BuildMembers());
		m_SecFrame.Insert(BuildRanks());
		m_SecFrame.Insert(BuildRelations());
	}

	protected Widget BuildMain()
	{
		float sh = m_H - SEC_Y;
		float cardH = CARD_W * 4.0 / 3.0;
		float fx = 200;
		float fw = m_EW - fx;
		float half = 520;
		float rx = fx + half + 20;
		float rw = m_EW - rx;
		string lbl;
		Widget f = INT_UIF_Draw.Frame(m_EdBody, 0, SEC_Y, m_EW, sh, "INT_PDA_AdmFMain");
		// the vertical card (live) + emblem picker
		m_CardHost = INT_UIF_Draw.Frame(f, 0, 0, CARD_W, cardH, "INT_PDA_AdmFCard");
		lbl = INT_PDA_Look.Caps("#STR_INT_PDA_AF_EMBLEM_PICK");
		m_EmbBtn = INT_UIF_Button.Create(m_Bag, f, 0, cardH + 12, CARD_W, 40, lbl, INT_UIF_Variant.SECONDARY, "star");
		if (m_EmbBtn)
			m_EmbBtn.OnClick().Insert(OnEmblemClick);
		m_EmbNo = INT_UIF_Draw.Text(f, 0, cardH + 60, CARD_W, 20, "", 13, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
		// name + id
		FieldCaption(f, fx, 0, 200, "#STR_INT_PDA_AF_F_NAME");
		INT_UIF_Draw.Text(f, fx + 200, 0, half - 200, 18, "#STR_INT_PDA_AF_NAME_HINT", 13, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		m_InName = INT_UIF_TextInput.Create(m_Bag, f, fx, 22, half, 48, "#STR_INT_PDA_AF_PH_NAME", NAME_CHARS);
		if (m_InName)
			m_InName.OnChanged().Insert(OnNameChanged);
		FieldCaption(f, rx, 0, rw, "#STR_INT_PDA_AF_F_ID");
		m_IdText = INT_UIF_Draw.Text(f, rx, 22, rw, 48, "", 26, INT_PDA_Look.ADMIN);
		// motto
		FieldCaption(f, fx, 84, fw, "#STR_INT_PDA_AF_F_MOTTO");
		m_InMotto = INT_UIF_TextInput.Create(m_Bag, f, fx, 106, fw, 48, "#STR_INT_PDA_AF_PH_MOTTO", MOTTO_CHARS);
		if (m_InMotto)
			m_InMotto.OnChanged().Insert(OnMainChanged);
		// base
		FieldCaption(f, fx, 168, half, "#STR_INT_PDA_AF_F_BASE");
		m_InBase = INT_UIF_TextInput.Create(m_Bag, f, fx, 190, half, 48, "#STR_INT_PDA_AF_PH_BASE", BASE_CHARS);
		if (m_InBase)
			m_InBase.OnChanged().Insert(OnMainChanged);
		// spawn point
		FieldCaption(f, fx, 252, 300, "#STR_INT_PDA_AF_F_SPAWN");
		INT_UIF_Draw.Text(f, fx + 300, 252, half - 300, 18, "#STR_INT_PDA_AF_SPAWN_HINT", 13, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		m_InSpawn = INT_UIF_TextInput.Create(m_Bag, f, fx, 274, half, 48, "#STR_INT_PDA_AF_PH_SPAWN", SPAWN_CHARS);
		if (m_InSpawn)
			m_InSpawn.OnChanged().Insert(OnMainChanged);
		lbl = INT_PDA_Look.Caps("#STR_INT_PDA_AF_MY_POS");
		m_PosBtn = INT_UIF_Button.Create(m_Bag, f, rx, 274, rw, 48, lbl, INT_UIF_Variant.SECONDARY, "pin");
		if (m_PosBtn)
			m_PosBtn.OnClick().Insert(OnPosClick);
		INT_PDA_Look.MText(f, fx, 336, fw, 44, "#STR_INT_PDA_AF_SPAWN_NOTE", 13, INT_PDA_Look.DIM2);
		// delete / save
		lbl = INT_PDA_Look.Caps("#STR_INT_PDA_AF_DELETE");
		m_DelBtn = INT_UIF_Button.Create(m_Bag, f, fx, sh - 56, 280, 48, lbl, INT_UIF_Variant.DANGER, "trash");
		if (m_DelBtn)
			m_DelBtn.OnClick().Insert(OnDeleteClick);
		lbl = INT_PDA_Look.Caps("#STR_INT_PDA_AF_SAVE");
		m_SaveBtn = INT_UIF_Button.Create(m_Bag, f, m_EW - 280, sh - 56, 280, 48, lbl, INT_UIF_Variant.PRIMARY, "check");
		if (m_SaveBtn)
			m_SaveBtn.OnClick().Insert(OnSaveClick);
		return f;
	}

	protected Widget BuildMembers()
	{
		int i;
		float ry;
		float sh = m_H - SEC_Y;
		float ddx = m_EW - 214;
		float rh = MEM_ROW_H - 4;
		Widget row;
		ButtonWidget area;
		INT_UIF_Dropdown dd;
		string lbl;
		array<string> opts = new array<string>();
		opts.Insert(Widget.TranslateString("#STR_INT_PDA_AF_ROLE_KICK"));
		opts.Insert(Widget.TranslateString("#STR_INT_PDA_ROLE_MEMBER"));
		opts.Insert(Widget.TranslateString("#STR_INT_PDA_ROLE_DEPUTY"));
		opts.Insert(Widget.TranslateString("#STR_INT_PDA_ROLE_LEADER"));
		Widget f = INT_UIF_Draw.Frame(m_EdBody, 0, SEC_Y, m_EW, sh, "INT_PDA_AdmFMembers");
		m_MCaption = INT_UIF_Draw.Text(f, 0, 0, 700, 18, "", 13, INT_PDA_Look.DIM);
		INT_UIF_Surface.Box(f, 0, 26, m_EW, 30, 4, INT_PDA_Look.PANEL2, 0);
		FieldCaption(f, 50, 32, 240, "#STR_INT_PDA_AF_COL_CALLSIGN");
		FieldCaption(f, 300, 32, 210, "#STR_INT_PDA_AF_COL_STEAM");
		FieldCaption(f, 520, 32, 170, "#STR_INT_PDA_AF_COL_RANK");
		FieldCaption(f, 700, 32, 120, "#STR_INT_PDA_AF_COL_JOINED");
		FieldCaption(f, ddx, 32, 200, "#STR_INT_PDA_AF_COL_ROLE");
		area = Hit(f, 0, MEM_TOP, m_EW, MEM_ROWS * MEM_ROW_H);
		m_AreaHit.Set(L_MEM, area);
		for (i = 0; i < MEM_ROWS; i++)
		{
			ry = MEM_TOP + i * MEM_ROW_H;
			row = INT_UIF_Draw.Frame(f, 0, ry, m_EW, rh, "INT_PDA_AdmFMem");
			INT_UIF_Surface.Box(row, 0, 0, m_EW, rh, 4, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
			m_MRow.Insert(row);
			m_MDot.Insert(INT_UIF_Draw.Image(row, 14, 18, 8, 8, "iuf_circle16", INT_PDA_Look.DIM2));
			m_MRoleIc.Insert(INT_UIF_Draw.Icon(row, 28, 14, 16, "crown", INT_PDA_Look.ORANGE));
			m_MName.Insert(INT_UIF_Draw.Text(row, 50, 0, 240, rh, "", 17, INT_PDA_Look.TEXT));
			m_MSteam.Insert(INT_UIF_Draw.Text(row, 300, 0, 210, rh, "", 14, INT_PDA_Look.DIM));
			m_MRank.Insert(INT_UIF_Draw.Text(row, 520, 0, 170, rh, "", 15, INT_PDA_Look.TEXT));
			m_MJoined.Insert(INT_UIF_Draw.Text(row, 700, 0, 120, rh, "", 13, INT_PDA_Look.DIM));
			m_MHit.Insert(Hit(row, 0, 0, m_EW, rh));
			dd = INT_UIF_Dropdown.Create(m_Bag, row, ddx, 4, 200, opts, 1, "", 36);
			if (dd)
				dd.OnSelected().Insert(OnRolePicked);
			m_MRole.Insert(dd);
			m_MRowSteam.Insert("");
		}
		m_MEmpty = INT_UIF_Draw.Text(f, 0, MEM_TOP + 80, m_EW, 26, "#STR_INT_PDA_AF_MEMBERS_EMPTY", 16, INT_PDA_Look.DIM2, INT_UIF_Align.CENTER);
		BuildPager(L_MEM, f, m_EW - 300, MEM_TOP + MEM_ROWS * MEM_ROW_H + 6, 300);
		lbl = INT_PDA_Look.Caps("#STR_INT_PDA_AF_JOIN");
		m_JoinBtn = INT_UIF_Button.Create(m_Bag, f, 0, sh - 52, 300, 48, lbl, INT_UIF_Variant.SECONDARY, "user_plus");
		if (m_JoinBtn)
			m_JoinBtn.OnClick().Insert(OnJoinClick);
		INT_PDA_Look.MText(f, 320, sh - 50, 400, 46, "#STR_INT_PDA_AF_MEMBERS_NOTE", 13, INT_PDA_Look.DIM2);
		return f;
	}

	protected Widget BuildRanks()
	{
		int i;
		float ry;
		float sh = m_H - SEC_Y;
		float rh = RANK_ROW_H - 4;
		Widget row;
		ButtonWidget area;
		string lbl;
		Widget f = INT_UIF_Draw.Frame(m_EdBody, 0, SEC_Y, m_EW, sh, "INT_PDA_AdmFRanks");
		m_RCaption = INT_UIF_Draw.Text(f, 0, 0, 500, 18, "", 13, INT_PDA_Look.DIM);
		m_RState = INT_UIF_Draw.Text(f, m_EW - 400, 0, 400, 18, "", 13, INT_PDA_Look.ORANGE, INT_UIF_Align.RIGHT);
		m_InRank = INT_UIF_TextInput.Create(m_Bag, f, 0, 26, 560, 44, "#STR_INT_PDA_AF_PH_RANK", RANK_CHARS);
		if (m_InRank)
			m_InRank.OnSubmit().Insert(OnRankSubmit);
		lbl = INT_PDA_Look.Caps("#STR_INT_PDA_AF_RANK_ADD");
		m_RankAddBtn = INT_UIF_Button.Create(m_Bag, f, 576, 26, 200, 44, lbl, INT_UIF_Variant.SECONDARY, "plus");
		if (m_RankAddBtn)
			m_RankAddBtn.OnClick().Insert(OnRankAddClick);
		INT_PDA_Look.MText(f, 796, 26, m_EW - 796, 44, "#STR_INT_PDA_AF_RANK_HINT", 13, INT_PDA_Look.DIM2);
		area = Hit(f, 0, RANK_TOP, m_EW, RANK_ROWS * RANK_ROW_H);
		m_AreaHit.Set(L_RANK, area);
		for (i = 0; i < RANK_ROWS; i++)
		{
			ry = RANK_TOP + i * RANK_ROW_H;
			row = INT_UIF_Draw.Frame(f, 0, ry, m_EW, rh, "INT_PDA_AdmFRank");
			INT_UIF_Surface.Box(row, 0, 0, m_EW, rh, 4, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
			m_RRow.Insert(row);
			m_RNum.Insert(INT_UIF_Draw.Text(row, 16, 0, 40, rh, "", 14, INT_PDA_Look.ORANGE));
			m_RName.Insert(INT_UIF_Draw.Text(row, 60, 0, 520, rh, "", 17, INT_PDA_Look.TEXT));
			m_RUse.Insert(INT_UIF_Draw.Text(row, 600, 0, 200, rh, "", 13, INT_PDA_Look.DIM));
			m_RHit.Insert(Hit(row, 0, 0, m_EW, rh));
			m_RIc.Insert(IconBtn(row, m_EW - 178, 3, "chevron_up", true, INT_PDA_Look.TEXT));
			m_RIc.Insert(IconBtn(row, m_EW - 142, 3, "chevron_down", true, INT_PDA_Look.TEXT));
			m_RIc.Insert(IconBtn(row, m_EW - 96, 3, "edit", false, INT_PDA_Look.TEXT));
			m_RIc.Insert(IconBtn(row, m_EW - 48, 3, "trash", false, INT_PDA_Look.ENEMY));
		}
		m_REmpty = INT_UIF_Draw.Text(f, 0, RANK_TOP + 80, m_EW, 26, "#STR_INT_PDA_AF_RANKS_EMPTY", 16, INT_PDA_Look.DIM2, INT_UIF_Align.CENTER);
		BuildPager(L_RANK, f, 0, sh - 46, 300);
		lbl = INT_PDA_Look.Caps("#STR_INT_PDA_AF_RANK_RESET");
		m_RankResetBtn = INT_UIF_Button.Create(m_Bag, f, m_EW - 520, sh - 52, 220, 48, lbl, INT_UIF_Variant.SECONDARY, "refresh");
		if (m_RankResetBtn)
			m_RankResetBtn.OnClick().Insert(OnRankResetClick);
		lbl = INT_PDA_Look.Caps("#STR_INT_PDA_AF_RANK_SAVE");
		m_RankSaveBtn = INT_UIF_Button.Create(m_Bag, f, m_EW - 284, sh - 52, 284, 48, lbl, INT_UIF_Variant.PRIMARY, "check");
		if (m_RankSaveBtn)
			m_RankSaveBtn.OnClick().Insert(OnRankSaveClick);
		return f;
	}

	protected Widget BuildRelations()
	{
		int i;
		float ry;
		float sh = m_H - SEC_Y;
		float rh = REL_ROW_H - 6;
		float rsx = 566;
		float rsw = m_EW - rsx - 76;
		Widget row;
		ButtonWidget area;
		INT_UIF_Dropdown dd;
		INT_UIF_TextInput ti;
		INT_UIF_Button sb;
		array<string> opts = new array<string>();
		opts.Insert(Widget.TranslateString("#STR_INT_PDA_AF_REL_ALLY"));
		opts.Insert(Widget.TranslateString("#STR_INT_PDA_AF_REL_NEUTRAL"));
		opts.Insert(Widget.TranslateString("#STR_INT_PDA_AF_REL_ENEMY"));
		Widget f = INT_UIF_Draw.Frame(m_EdBody, 0, SEC_Y, m_EW, sh, "INT_PDA_AdmFRel");
		m_ECaption = INT_UIF_Draw.Text(f, 0, 0, 500, 18, "", 13, INT_PDA_Look.DIM);
		INT_UIF_Draw.Text(f, m_EW - 520, 0, 520, 18, "#STR_INT_PDA_AF_REL_HINT", 13, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		area = Hit(f, 0, REL_TOP, m_EW, REL_ROWS * REL_ROW_H);
		m_AreaHit.Set(L_REL, area);
		for (i = 0; i < REL_ROWS; i++)
		{
			ry = REL_TOP + i * REL_ROW_H;
			row = INT_UIF_Draw.Frame(f, 0, ry, m_EW, rh, "INT_PDA_AdmFRelRow");
			INT_UIF_Surface.Box(row, 0, 0, m_EW, rh, 4, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
			m_ERow.Insert(row);
			m_EBar.Insert(INT_UIF_Draw.Line(row, 0, 0, 3, rh, INT_PDA_Look.NEUTRAL));
			m_EEmb.Insert(INT_PDA_Look.DrawEmblem(row, 14, 8, 42, 1));
			m_EName.Insert(INT_UIF_Draw.Text(row, 68, 5, 250, 26, "", 18, INT_PDA_Look.TEXT));
			m_EState.Insert(INT_UIF_Draw.Text(row, 68, 32, 260, 18, "", 13, INT_PDA_Look.NEUTRAL));
			m_EHit.Insert(Hit(row, 0, 0, m_EW, rh));
			dd = INT_UIF_Dropdown.Create(m_Bag, row, 336, 11, 214, opts, 1, "", 36);
			if (dd)
				dd.OnSelected().Insert(OnRelPicked);
			m_ERel.Insert(dd);
			m_ERowId.Insert("");
			ti = INT_UIF_TextInput.Create(m_Bag, row, rsx, 7, rsw, 44, "#STR_INT_PDA_AF_PH_REASON", REASON_CHARS);
			if (ti)
			{
				ti.OnChanged().Insert(OnReasonChanged);
				ti.OnSubmit().Insert(OnReasonSubmit);
			}
			m_EReason.Insert(ti);
			sb = INT_UIF_Button.Create(m_Bag, row, m_EW - 64, 7, 56, 44, "", INT_UIF_Variant.PRIMARY, "check");
			if (sb)
				sb.OnClick().Insert(OnRelSaveClick);
			m_ESave.Insert(sb);
		}
		m_EEmpty = INT_UIF_Draw.Text(f, 0, REL_TOP + 80, m_EW, 26, "#STR_INT_PDA_AF_REL_EMPTY", 16, INT_PDA_Look.DIM2, INT_UIF_Align.CENTER);
		BuildLegend(f, 0, sh - 42);
		BuildPager(L_REL, f, m_EW - 300, sh - 46, 300);
		return f;
	}

	// Colour key of the relations: ● союз ● нейтралитет ● вражда.
	protected void BuildLegend(Widget parent, float x, float y)
	{
		int i;
		int rel;
		int c;
		float cx = x;
		string key;
		string word;
		array<int> rels = new array<int>();
		rels.Insert(INT_PDA_Const.REL_ALLY);
		rels.Insert(INT_PDA_Const.REL_NEUTRAL);
		rels.Insert(INT_PDA_Const.REL_ENEMY);
		for (i = 0; i < rels.Count(); i++)
		{
			rel = rels[i];
			c = RelColor(rel);
			key = RelKey(rel);
			word = Widget.TranslateString(key);
			INT_UIF_Draw.Image(parent, cx, y + 8, 10, 10, "iuf_circle16", c);
			INT_UIF_Draw.Text(parent, cx + 18, y, 220, 26, word, 14, INT_PDA_Look.DIM);
			cx = cx + 18 + INT_PDA_Look.TextW(word, 14) + 28;
		}
	}

	// --- emblem popup: 45 squares (9 x 5) + the live vertical card --------------------------------------------------
	protected void BuildEmblemPopup()
	{
		int i;
		int col;
		int grow;
		float cx;
		float cy;
		int rows = (INT_PDA_Const.EMBLEMS + EMB_COLS - 1) / EMB_COLS;
		float gw = EMB_COLS * EMB_CELL + (EMB_COLS - 1) * EMB_GAP;
		float gh = rows * EMB_CELL + (rows - 1) * EMB_GAP;
		float bw = 28 + gw + 32 + POP_CARD_W + 28;
		float bh = 64 + gh + 70;
		float cardH = POP_CARD_W * 4.0 / 3.0;
		string cap;
		string lbl;
		m_PopCardX = 28 + gw + 32;
		m_PopX = (m_W - bw) * 0.5;
		m_PopY = (m_H - bh) * 0.5;
		m_Pop = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, m_H, "INT_PDA_AdmFPop");
		INT_UIF_Draw.Image(m_Pop, 0, 0, m_W, m_H, "iuf_white", ARGB(170, 0, 0, 0));
		m_PopBack = Hit(m_Pop, 0, 0, m_W, m_H);
		m_PopBox = INT_UIF_Draw.Frame(m_Pop, m_PopX, m_PopY, bw, bh, "INT_PDA_AdmFPopBox");
		INT_UIF_Surface.Shadow(m_PopBox, -24, -16, bw + 48, bh + 44, ARGB(160, 0, 0, 0));
		INT_UIF_Surface.Box(m_PopBox, 0, 0, bw, bh, 8, INT_PDA_Look.PANEL, INT_PDA_Look.LINE2);
		m_PopBoxHit = Hit(m_PopBox, 0, 0, bw, bh);
		INT_UIF_Draw.Line(m_PopBox, 0, 0, bw, 3, INT_PDA_Look.ORANGE);
		cap = INT_PDA_Look.Caps("#STR_INT_PDA_AF_EMBLEM_TITLE");
		INT_UIF_Draw.Text(m_PopBox, 28, 20, 500, 28, cap, 20, INT_PDA_Look.TEXT);
		m_PopCount = INT_UIF_Draw.Text(m_PopBox, bw - 228, 24, 200, 20, "", 15, INT_PDA_Look.ORANGE, INT_UIF_Align.RIGHT);
		// hover / selection rings under the cells (the cells' hits stay on top)
		m_PopHov = INT_UIF_Draw.Frame(m_PopBox, 0, 0, EMB_CELL + 6, EMB_CELL + 6, "INT_PDA_AdmFPopHov");
		INT_UIF_Surface.Box(m_PopHov, 0, 0, EMB_CELL + 6, EMB_CELL + 6, 6, 0, INT_PDA_Look.DIM);
		m_PopHov.Show(false);
		m_PopSel = INT_UIF_Draw.Frame(m_PopBox, 0, 0, EMB_CELL + 6, EMB_CELL + 6, "INT_PDA_AdmFPopSel");
		INT_UIF_Surface.Box(m_PopSel, 0, 0, EMB_CELL + 6, EMB_CELL + 6, 6, 0, INT_PDA_Look.ORANGE);
		for (i = 0; i < INT_PDA_Const.EMBLEMS; i++)
		{
			col = i % EMB_COLS;
			grow = i / EMB_COLS;
			cx = 28 + col * (EMB_CELL + EMB_GAP);
			cy = 64 + grow * (EMB_CELL + EMB_GAP);
			INT_UIF_Draw.Image(m_PopBox, cx, cy, EMB_CELL, EMB_CELL, "iuf_white", INT_PDA_Look.PANEL2);
			INT_PDA_Look.DrawEmblem(m_PopBox, cx + 3, cy + 3, EMB_CELL - 6, i + 1);
			m_PopHits.Insert(Hit(m_PopBox, cx, cy, EMB_CELL, EMB_CELL));
		}
		m_PopCard = INT_UIF_Draw.Frame(m_PopBox, m_PopCardX, 64, POP_CARD_W, cardH, "INT_PDA_AdmFPopCard");
		INT_PDA_Look.MText(m_PopBox, 28, 64 + gh + 14, gw, 40, "#STR_INT_PDA_AF_EMBLEM_HINT", 13, INT_PDA_Look.DIM2);
		lbl = INT_PDA_Look.Caps("#STR_INT_PDA_AF_DONE");
		m_PopDone = INT_UIF_Button.Create(m_Bag, m_PopBox, m_PopCardX, bh - 76, POP_CARD_W, 48, lbl, INT_UIF_Variant.PRIMARY, "check");
		if (m_PopDone)
			m_PopDone.OnClick().Insert(OnPopDoneClick);
		m_Pop.Show(false);
	}

	// ================================================================================================================
	// host interface
	// ================================================================================================================
	void Show(bool show)
	{
		int now;
		if (!m_Root)
			return;
		if (!show)
		{
			ClosePopup();
			INT_UIF_Dropdown.CloseActive();
			m_Shown = false;
			m_Root.Show(false);
			return;
		}
		m_Root.Show(true);
		m_Shown = true;
		now = NowMs();
		if (now - m_ListAskMs >= LIST_GAP_MS)
			AskList();
		if (m_SelId != "" && !m_New)
		{
			CheckView();
			if (now - m_ViewAskMs >= LIST_GAP_MS)
				AskView(m_SelId);
		}
		RefreshFactions();
		RefreshEditor();
		RevealAt(m_Left, 0, 0, 0.03);
		RevealAt(m_Editor, ED_X, 0, 0.09);
		RevealRows(L_FAC);
	}

	void OnChanged(int what)
	{
		int sub;
		int code;
		string key;
		int tag;
		if (what == INT_PDA_Client.CH_PROFILE || what == INT_PDA_Client.CH_AUTH)
		{
			if (m_Shown)
				RefreshJoin();
			return;
		}
		if (what != INT_PDA_Client.CH_ADMIN)
			return;
		sub = INT_PDA_Client.s_AdmLastSub;
		code = INT_PDA_Client.s_AdmLastCode;
		if (m_ResultSeen)
			m_ResultSeen = false;
		else if (TakeAwait(sub, code, key, tag))
			HandleReply(sub, code, key, tag);
		if (sub != INT_PDA_Const.ADM_FACTIONS || code != INT_PDA_Const.R_OK)
			return;
		CheckView();
		RefreshFactions();
	}

	// Admin replies come as (OP_ADMIN, code); the sub-op is INT_PDA_Client.s_AdmLastSub (set before the invoke).
	// «Too fast» is the server's request gate (OP_RESULT, no OP_ADMIN_DATA, s_AdmLastSub is stale): our last request
	// was dropped unexecuted and goes out again.
	void OnResult(int requestOp, int code)
	{
		string key;
		int tag;
		if (requestOp != INT_PDA_Const.OP_ADMIN)
			return;
		if (code == INT_PDA_Const.R_TOO_FAST)
		{
			OnTooFast();
			return;
		}
		int sub = INT_PDA_Client.s_AdmLastSub;
		if (!TakeAwait(sub, code, key, tag))
			return;
		m_ResultSeen = true;
		HandleReply(sub, code, key, tag);
	}

	// When this sub-tab sent its last request (the host decides whose request a «too fast» was).
	int LastSendMs()
	{
		if (!m_SentRpc)
			return -100000000;
		return m_LastSendMs;
	}

	void Update(float dt)
	{
		int now;
		int queued;
		string viewKey;
		Pump();
		m_Tick = m_Tick + dt;
		if (m_Tick < 1.0)
			return;
		m_Tick = 0;
		PurgeAwait();
		now = NowMs();
		if (m_SaveBusy && now - m_SaveBusyMs > AWAIT_MS)
			ReleaseSave();
		if (!m_Loading)
			return;
		viewKey = "view:" + m_SelId;
		queued = m_QueueKey.Find(viewKey);
		if (queued < 0 && now - m_ViewAskMs > LOAD_GIVEUP_MS)
		{
			m_Loading = false;
			RefreshState();
		}
	}

	bool HandleEscape()
	{
		if (!m_PopOpen)
			return false;
		INT_PDA_Sfx.Play("Click");
		ClosePopup();
		return true;
	}

	void OnTestCommand(string cmd)
	{
		int num;
		int role;
		string c = cmd + "";
		string verb;
		string arg1 = "";
		string arg2 = "";
		string arg3 = "";
		string rest;
		array<string> raw = new array<string>();
		array<string> parts = new array<string>();
		c.TrimInPlace();
		c.Split(" ", raw);
		foreach (string piece : raw)
		{
			if (piece != "")
				parts.Insert(piece);
		}
		if (parts.Count() == 0)
			return;
		verb = parts[0];
		if (parts.Count() >= 2)
			arg1 = parts[1];
		if (parts.Count() >= 3)
			arg2 = parts[2];
		if (parts.Count() >= 4)
			arg3 = parts[3];
		INT_PDA_Log.Raw("ADMF cmd " + verb);
		if (verb == "list")
		{
			AskList();
			return;
		}
		if (verb == "select")
		{
			SelectFaction(arg1);
			return;
		}
		if (verb == "new")
		{
			rest = JoinFrom(parts, 1);
			StartNew();
			m_Filling = true;
			SetInput(m_InName, rest);
			m_Filling = false;
			RedrawCard();
			SaveMain();
			return;
		}
		if (verb == "save")
		{
			SaveMain();
			return;
		}
		if (verb == "del")
		{
			SendDelete(arg1);
			return;
		}
		if (verb == "emblem")
		{
			num = arg1.ToInt();
			PickEmblem(num);
			return;
		}
		if (verb == "role")
		{
			num = arg2.ToInt();
			if (num <= INT_PDA_Const.ROLE_NONE)
			{
				SendKick(arg1);
				return;
			}
			role = INT_PDA_Util.ClampI(num, INT_PDA_Const.ROLE_MEMBER, INT_PDA_Const.ROLE_LEADER);
			SendRole(arg1, role);
			return;
		}
		if (verb == "rank")
		{
			rest = JoinFrom(parts, 2);
			if (arg1 == "add")
			{
				if (AddRank(rest))
					SaveRanks();
				return;
			}
			if (arg1 == "del")
			{
				num = m_Ranks.Find(rest);
				if (num >= 0)
				{
					DelRank(num);
					SaveRanks();
				}
			}
			if (arg1 == "ren")
			{
				// rank ren <old>|<new>: like the rename prompt (a saved rank goes to the server at once)
				num = rest.IndexOf("|");
				if (num > 0)
				{
					m_RenameName = rest.Substring(0, num);
					arg3 = rest.Substring(num + 1, rest.Length() - num - 1);
					OnRenameDone(true, arg3, "");
				}
			}
			return;
		}
		if (verb == "rel")
		{
			role = arg3.ToInt();
			num = INT_PDA_Util.ClampI(role, INT_PDA_Const.REL_NEUTRAL, INT_PDA_Const.REL_ENEMY);
			rest = JoinFrom(parts, 4);
			SendRelation(arg1, arg2, num, rest);
			return;
		}
		if (verb == "join")
		{
			SendJoin(arg1);
			return;
		}
		if (verb == "tab")
		{
			num = arg1.ToInt();
			ShowSection(num, true);
			return;
		}
		if (verb == "pop")
		{
			if (arg1 == "close")
				ClosePopup();
			else
				OpenPopup();
			return;
		}
		if (verb == "state")
			DumpState();
	}

	protected string JoinFrom(array<string> parts, int start)
	{
		string s = "";
		for (int i = start; i < parts.Count(); i++)
		{
			if (s != "")
				s = s + " ";
			s = s + parts[i];
		}
		return s;
	}

	protected void DumpState()
	{
		array<ref INT_PDA_FactionSummary> facs = INT_PDA_Client.s_AdmFactions;
		int nf = facs.Count();
		int nq = m_Queue.Count();
		int na = m_AwaitSub.Count();
		int nm = m_MemOrder.Count();
		int nr = m_Ranks.Count();
		int ne = m_RelIds.Count();
		string viewId = "-";
		if (m_View)
			viewId = m_View.Id;
		string line = "ADMF state list=" + nf.ToString() + " sel=" + m_SelId + " new=" + m_New.ToString() + " view=" + viewId;
		line = line + " members=" + nm.ToString() + " ranks=" + nr.ToString() + " rels=" + ne.ToString() + " emblem=" + m_Emblem.ToString();
		line = line + " queue=" + nq.ToString() + " await=" + na.ToString() + " dirty=" + m_MainDirty.ToString() + "/" + m_RanksDirty.ToString();
		INT_PDA_Log.Raw(line);
	}

	// ================================================================================================================
	// requests (paced queue) and replies
	// ================================================================================================================
	protected int NowMs()
	{
		if (!GetGame())
			return 0;
		return GetGame().GetTime();
	}

	// key != "" = the same request is never queued twice (list, view of one faction); replace = a newer request with
	// that key takes the queued one's place (the latest edit of one relation wins). tag = the edit serial it was built
	// from (comes back with the answer). False = not queued.
	protected bool Enqueue(ScriptRPC rpc, int sub, string key, int tag = 0, bool replace = false)
	{
		if (!rpc)
			return false;
		int at = -1;
		if (key != "")
			at = m_QueueKey.Find(key);
		if (at >= 0)
		{
			if (!replace)
				return true;
			m_Queue.Set(at, rpc);
			m_QueueTag.Set(at, tag);
			m_QueueTry.Set(at, 0);
			Pump();
			return true;
		}
		if (m_Queue.Count() >= QUEUE_MAX)
		{
			INT_PDA_Log.Warn("админ ГП: очередь запросов полна, запрос отброшен");
			ShowError(INT_PDA_Const.R_TOO_FAST);
			return false;
		}
		m_Queue.Insert(rpc);
		m_QueueSub.Insert(sub);
		m_QueueKey.Insert(key);
		m_QueueTag.Insert(tag);
		m_QueueTry.Insert(0);
		Pump();
		return true;
	}

	protected void Pump()
	{
		if (m_Queue.Count() == 0)
			return;
		int now = NowMs();
		// m_LastSendMs may lie ahead (a hold after «too fast»); far ahead = a new mission restarted the clock
		if (m_LastSendMs - now > RETRY_HOLD_MS + GAP_MS)
			m_LastSendMs = now - GAP_MS;
		if (now - m_LastSendMs < GAP_MS)
			return;
		ScriptRPC rpc = m_Queue[0];
		int sub = m_QueueSub[0];
		string key = m_QueueKey[0];
		int tag = m_QueueTag[0];
		int tries = m_QueueTry[0];
		m_Queue.RemoveOrdered(0);
		m_QueueSub.RemoveOrdered(0);
		m_QueueKey.RemoveOrdered(0);
		m_QueueTag.RemoveOrdered(0);
		m_QueueTry.RemoveOrdered(0);
		m_LastSendMs = now;
		m_AwaitSub.Insert(sub);
		m_AwaitMs.Insert(now);
		m_AwaitKey.Insert(key);
		m_AwaitTag.Insert(tag);
		m_SentRpc = rpc;
		m_SentSub = sub;
		m_SentKey = key;
		m_SentTag = tag;
		m_SentTry = tries;
		INT_PDA_Client.Send(rpc);
	}

	protected void RemoveAwait(int idx)
	{
		if (idx < 0 || idx >= m_AwaitSub.Count())
			return;
		m_AwaitSub.RemoveOrdered(idx);
		m_AwaitMs.RemoveOrdered(idx);
		m_AwaitKey.RemoveOrdered(idx);
		m_AwaitTag.RemoveOrdered(idx);
	}

	protected void PurgeAwait()
	{
		int now = NowMs();
		for (int i = m_AwaitMs.Count() - 1; i >= 0; i--)
		{
			if (now - m_AwaitMs[i] <= AWAIT_MS)
				continue;
			// a rename that never got an answer: its local name stays, its bookkeeping goes (oldest first)
			if (m_AwaitSub[i] == INT_PDA_Const.ADM_RANK_RENAME)
				PopRename();
			RemoveAwait(i);
		}
	}

	// True when this reply answers one of OUR requests (the other admin sub-tabs share the channel); key / tag = what
	// that request was about. A «not found» of ADM_FACTIONS can only answer a faction view (the list never says it):
	// it is matched to the oldest view on its way, so it names the faction it was about.
	protected bool TakeAwait(int sub, int code, out string key, out int tag)
	{
		int i;
		int idx = -1;
		key = "";
		tag = 0;
		PurgeAwait();
		if (sub == INT_PDA_Const.ADM_FACTIONS && code == INT_PDA_Const.R_NOT_FOUND)
		{
			for (i = 0; i < m_AwaitSub.Count(); i++)
			{
				if (m_AwaitSub[i] == sub && m_AwaitKey[i].IndexOf("view:") == 0)
				{
					idx = i;
					break;
				}
			}
		}
		if (idx < 0)
			idx = m_AwaitSub.Find(sub);
		if (idx < 0)
			return false;
		key = m_AwaitKey[idx];
		tag = m_AwaitTag[idx];
		RemoveAwait(idx);
		if (m_SentRpc && sub == m_SentSub && key == m_SentKey)
			m_SentRpc = null;
		return true;
	}

	// The server's gate dropped the request we sent last: back to the head of the queue after a short hold (twice at
	// most; then the admin is told and the request is given up like an unanswered one).
	protected void OnTooFast()
	{
		int i;
		if (!m_SentRpc)
			return;
		for (i = m_AwaitSub.Count() - 1; i >= 0; i--)
		{
			if (m_AwaitSub[i] == m_SentSub && m_AwaitKey[i] == m_SentKey)
			{
				RemoveAwait(i);
				break;
			}
		}
		INT_PDA_Log.Raw("ADMF toofast " + m_SentSub.ToString() + " try " + m_SentTry.ToString());
		if (m_SentTry >= RETRY_MAX)
		{
			ShowError(INT_PDA_Const.R_TOO_FAST);
			if (m_SentSub == INT_PDA_Const.ADM_FACTION_SAVE)
				ReleaseSave();
			else if (m_SentSub == INT_PDA_Const.ADM_RANK_RENAME)
				UndoRename();
			else if (m_SentKey == "view:" + m_SelId)
				m_Loading = false;
			m_SentRpc = null;
			RefreshState();
			return;
		}
		m_Queue.InsertAt(m_SentRpc, 0);
		m_QueueSub.InsertAt(m_SentSub, 0);
		m_QueueKey.InsertAt(m_SentKey, 0);
		m_QueueTag.InsertAt(m_SentTag, 0);
		m_QueueTry.InsertAt(m_SentTry + 1, 0);
		m_SentRpc = null;
		m_LastSendMs = NowMs() + RETRY_HOLD_MS;
	}

	// ADM_FACTIONS: string id ("" = list) -> sub, code, bool full, list | FactionView (INT_PDA_SE_Admin.OnFactions).
	protected void AskList()
	{
		m_ListAskMs = NowMs();
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_FACTIONS);
		INT_PDA_Proto.WriteStr(rpc, "");
		Enqueue(rpc, INT_PDA_Const.ADM_FACTIONS, "list");
	}

	protected void AskView(string id)
	{
		if (id == "")
			return;
		m_ViewAskMs = NowMs();
		m_Loading = true;
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_FACTIONS);
		INT_PDA_Proto.WriteStr(rpc, id);
		Enqueue(rpc, INT_PDA_Const.ADM_FACTIONS, "view:" + id);
		RefreshState();
	}

	// ADM_FACTION_DELETE: string id.
	protected void SendDelete(string id)
	{
		if (id == "")
			return;
		m_DeletingId = id;
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_FACTION_DELETE);
		INT_PDA_Proto.WriteStr(rpc, id);
		Enqueue(rpc, INT_PDA_Const.ADM_FACTION_DELETE, "");
		INT_PDA_Sfx.Play("Send");
	}

	// ADM_SET_ROLE: string faction id, string steam, int role (server clamps MEMBER..LEADER, one leader).
	protected void SendRole(string steam, int role)
	{
		if (m_SelId == "" || steam == "")
			return;
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_SET_ROLE);
		INT_PDA_Proto.WriteStr(rpc, m_SelId);
		INT_PDA_Proto.WriteStr(rpc, steam);
		rpc.Write(role);
		Enqueue(rpc, INT_PDA_Const.ADM_SET_ROLE, "");
	}

	// ADM_KICK: string faction id, string steam.
	protected void SendKick(string steam)
	{
		if (m_SelId == "" || steam == "")
			return;
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_KICK);
		INT_PDA_Proto.WriteStr(rpc, m_SelId);
		INT_PDA_Proto.WriteStr(rpc, steam);
		Enqueue(rpc, INT_PDA_Const.ADM_KICK, "");
	}

	// ADM_JOIN: string faction id (the admin's own account moves over).
	protected void SendJoin(string id)
	{
		if (id == "")
			return;
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_JOIN);
		INT_PDA_Proto.WriteStr(rpc, id);
		Enqueue(rpc, INT_PDA_Const.ADM_JOIN, "");
		INT_PDA_Sfx.Play("Send");
	}

	// ADM_RANKS: string faction id, int n (<= 32), n strings.
	protected void SendRanks()
	{
		int i;
		int n = m_Ranks.Count();
		if (n > RANKS_MAX)
			n = RANKS_MAX;
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_RANKS);
		INT_PDA_Proto.WriteStr(rpc, m_SelId);
		rpc.Write(n);
		for (i = 0; i < n; i++)
			INT_PDA_Proto.WriteStr(rpc, m_Ranks[i]);
		Enqueue(rpc, INT_PDA_Const.ADM_RANKS, "", m_RankEdits);
		INT_PDA_Sfx.Play("Send");
	}

	// ADM_RANK_RENAME: string faction id, string old rank, string new rank (the holders keep it under the new name).
	protected void SendRankRename(string oldName, string newName)
	{
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_RANK_RENAME);
		INT_PDA_Proto.WriteStr(rpc, m_SelId);
		INT_PDA_Proto.WriteStr(rpc, oldName);
		INT_PDA_Proto.WriteStr(rpc, newName);
		if (!Enqueue(rpc, INT_PDA_Const.ADM_RANK_RENAME, ""))
			return;
		m_RenFac.Insert(m_SelId);
		m_RenOld.Insert(oldName);
		m_RenNew.Insert(newName);
		INT_PDA_Sfx.Play("Send");
	}

	// The oldest rename on its way leaves the bookkeeping.
	protected void PopRename()
	{
		if (m_RenOld.Count() == 0)
			return;
		m_RenFac.RemoveOrdered(0);
		m_RenOld.RemoveOrdered(0);
		m_RenNew.RemoveOrdered(0);
	}

	// The oldest rename on its way was refused: the local list gets the old name back (the server kept it).
	protected void UndoRename()
	{
		if (m_RenOld.Count() == 0)
			return;
		string fac = m_RenFac[0];
		string oldName = m_RenOld[0];
		string newName = m_RenNew[0];
		PopRename();
		if (fac != m_SelId || m_New)
			return;
		int idx = m_Ranks.Find(newName);
		if (idx >= 0 && m_Ranks.Find(oldName) < 0)
			m_Ranks.Set(idx, oldName);
		RefreshRanks();
	}

	// ADM_RELATION: string a, string b, int rel, string reason. Key: the selected faction + the other one (one queued
	// save per pair, the newer edit replaces it); tag: the row's edit serial when it was saved.
	protected void SendRelation(string a, string b, int rel, string reason)
	{
		if (a == "" || b == "")
			return;
		string other = "";
		if (a == m_SelId)
			other = b;
		else if (b == m_SelId)
			other = a;
		string key = "relx:" + a + "|" + b;
		int tag = 0;
		int idx = -1;
		if (other != "")
		{
			key = "rel:" + m_SelId + "|" + other;
			idx = m_RelIds.Find(other);
		}
		if (idx >= 0)
			tag = m_RelEdit[idx];
		ScriptRPC rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_RELATION);
		INT_PDA_Proto.WriteStr(rpc, a);
		INT_PDA_Proto.WriteStr(rpc, b);
		rpc.Write(rel);
		INT_PDA_Proto.WriteStr(rpc, reason);
		Enqueue(rpc, INT_PDA_Const.ADM_RELATION, key, tag, true);
		INT_PDA_Sfx.Play("Send");
	}

	protected void HandleReply(int sub, int code, string key, int tag)
	{
		string viewId = "";
		INT_PDA_Log.Raw("ADMF reply " + sub.ToString() + " " + code.ToString());
		if (sub == INT_PDA_Const.ADM_FACTIONS)
		{
			if (code == INT_PDA_Const.R_OK)
			{
				CheckView();
				RefreshFactions();
				return;
			}
			if (key.IndexOf("view:") == 0)
				viewId = key.Substring(5, key.Length() - 5);
			if (viewId == "" || viewId == m_SelId)
				m_Loading = false;
			if (code == INT_PDA_Const.R_NOT_FOUND)
			{
				// only the faction this answer was about: a «not found» of an earlier pick never drops the current one
				if (viewId != "" && viewId == m_SelId && !m_New)
				{
					INT_UIF_Toast.Warning("#STR_INT_PDA_AF_GONE");
					ClearSelection();
				}
				AskList();
				RefreshState();
				return;
			}
			ShowError(code);
			RefreshState();
			return;
		}
		if (sub == INT_PDA_Const.ADM_FACTION_SAVE)
			ReleaseSave();
		if (code != INT_PDA_Const.R_OK)
		{
			ShowError(code);
			if (sub == INT_PDA_Const.ADM_SET_ROLE || sub == INT_PDA_Const.ADM_KICK)
				RefreshMembers();
			if (sub == INT_PDA_Const.ADM_RANK_RENAME)
				UndoRename();
			if (sub == INT_PDA_Const.ADM_FACTION_DELETE && code == INT_PDA_Const.R_NOT_FOUND)
			{
				if (m_DeletingId == m_SelId)
					ClearSelection();
				m_DeletingId = "";
				AskList();
			}
			return;
		}
		if (sub == INT_PDA_Const.ADM_FACTION_SAVE)
		{
			OnSaved(tag);
			return;
		}
		if (sub == INT_PDA_Const.ADM_FACTION_DELETE)
		{
			INT_UIF_Toast.Success("#STR_INT_PDA_AF_DELETED");
			if (m_DeletingId != "" && m_DeletingId == m_SelId)
				ClearSelection();
			m_DeletingId = "";
			AskList();
			return;
		}
		if (sub == INT_PDA_Const.ADM_SET_ROLE)
			INT_UIF_Toast.Success("#STR_INT_PDA_AF_ROLE_SET");
		else if (sub == INT_PDA_Const.ADM_KICK)
			INT_UIF_Toast.Success("#STR_INT_PDA_AF_KICKED");
		else if (sub == INT_PDA_Const.ADM_JOIN)
			INT_UIF_Toast.Success("#STR_INT_PDA_AF_JOIN_OK");
		else if (sub == INT_PDA_Const.ADM_RANKS)
		{
			// edits made after this save left stay «не сохранено»
			if (tag == m_RankEdits)
				m_RanksDirty = false;
			INT_UIF_Toast.Success("#STR_INT_PDA_AF_RANKS_SAVED");
		}
		else if (sub == INT_PDA_Const.ADM_RANK_RENAME)
		{
			PopRename();
			INT_UIF_Toast.Success("#STR_INT_PDA_AF_RANK_RENAMED");
		}
		else if (sub == INT_PDA_Const.ADM_RELATION)
		{
			ClearRelDirty(key, tag);
			INT_UIF_Toast.Success("#STR_INT_PDA_AF_REL_SAVED");
		}
		else
		{
			return;
		}
		if (m_SelId != "" && !m_New)
			AskView(m_SelId);
		AskList();
		RefreshEditor();
	}

	// tag = the main-field edit serial the save was built from: fields edited after «Сохранить» stay unsaved (and are
	// not overwritten by the view that follows).
	protected void OnSaved(int tag)
	{
		string newId = INT_PDA_Client.s_AdmLastId;
		if (m_SaveWasNew)
			INT_UIF_Toast.Success("#STR_INT_PDA_AF_CREATED");
		else
			INT_UIF_Toast.Success("#STR_INT_PDA_AF_SAVED");
		if (m_New && m_SaveWasNew && newId != "")
		{
			m_New = false;
			m_SelId = newId;
			m_View = null;
		}
		if (newId != "" && newId == m_SelId && tag == m_MainEdits)
			m_MainDirty = false;
		AskList();
		if (m_SelId != "" && !m_New)
			AskView(m_SelId);
		RefreshFactions();
		RefreshEditor();
	}

	protected void ReleaseSave()
	{
		m_SaveBusy = false;
		if (m_SaveBtn)
			m_SaveBtn.SetEnabled(true);
	}

	protected void ShowError(int code)
	{
		INT_PDA_Sfx.Play("Error");
		if (m_Owner)
			m_Owner.Toast(code);
		else
			INT_UIF_Toast.Error(INT_PDA_Look.ResultKey(code));
	}

	protected void Fail(string key)
	{
		INT_PDA_Sfx.Play("Error");
		INT_UIF_Toast.Error(key);
	}

	// ================================================================================================================
	// selection and data
	// ================================================================================================================
	// A full view that arrived for the faction on screen (a view of another faction is ignored).
	protected void CheckView()
	{
		INT_PDA_FactionView v = INT_PDA_Client.s_AdmFaction;
		if (!v || v == m_View || m_New)
			return;
		if (v.Id != m_SelId)
			return;
		ApplyView(v);
	}

	// Unsaved edits survive a refresh of the same faction (main fields, ranks, edited relation rows).
	protected void ApplyView(INT_PDA_FactionView v)
	{
		bool other = true;
		if (m_View && m_View.Id == v.Id)
			other = false;
		m_View = v;
		m_Loading = false;
		if (other || !m_MainDirty)
			FillMain();
		if (other || !m_RanksDirty)
		{
			m_Ranks.Copy(v.Ranks);
			m_RanksDirty = false;
		}
		LoadRelations(other);
		LoadMembers();
		if (other)
		{
			m_First.Set(L_MEM, 0);
			m_First.Set(L_RANK, 0);
			m_First.Set(L_REL, 0);
		}
		RefreshEditor();
	}

	protected void SelectFaction(string id)
	{
		if (id == "")
			return;
		ClosePopup();
		if (!m_New && id == m_SelId)
		{
			AskView(id);
			return;
		}
		if (AnyDirty())
			INT_UIF_Toast.Info("#STR_INT_PDA_AF_DISCARDED");
		m_New = false;
		m_SelId = id;
		ClearEditorData();
		FillFromSummary();
		INT_PDA_FactionView cached = INT_PDA_Client.s_AdmFaction;
		if (cached && cached.Id == id)
			ApplyView(cached);
		AskView(id);
		RefreshFactions();
		RefreshEditor();
		RevealAt(m_EdBody, 0, 0, 0.02);
	}

	protected void StartNew()
	{
		ClosePopup();
		if (AnyDirty())
			INT_UIF_Toast.Info("#STR_INT_PDA_AF_DISCARDED");
		m_New = true;
		m_SelId = "";
		m_Loading = false;
		ClearEditorData();
		m_Filling = true;
		SetInput(m_InName, "");
		SetInput(m_InMotto, "");
		SetInput(m_InBase, "");
		SetInput(m_InSpawn, "");
		m_Filling = false;
		m_Emblem = FreeEmblem();
		m_Sec = SEC_MAIN;
		RedrawCard();
		RefreshFactions();
		RefreshEditor();
		RevealAt(m_EdBody, 0, 0, 0.02);
		if (m_InName)
			m_InName.Focus();
	}

	protected void ClearSelection()
	{
		ClosePopup();
		m_New = false;
		m_SelId = "";
		m_Loading = false;
		ClearEditorData();
		RefreshFactions();
		RefreshEditor();
	}

	protected void ClearEditorData()
	{
		m_View = null;
		m_MainDirty = false;
		m_RanksDirty = false;
		m_Ranks.Clear();
		ClearRelations();
		m_MemOrder.Clear();
		m_First.Set(L_MEM, 0);
		m_First.Set(L_RANK, 0);
		m_First.Set(L_REL, 0);
	}

	protected void ClearRelations()
	{
		m_RelIds.Clear();
		m_RelNames.Clear();
		m_RelEmblems.Clear();
		m_RelVals.Clear();
		m_RelReasons.Clear();
		m_RelDirty.Clear();
		m_RelEdit.Clear();
	}

	// Name and emblem from the list while the full view is on its way (inputs stay read from the view later).
	protected void FillFromSummary()
	{
		array<ref INT_PDA_FactionSummary> facs = INT_PDA_Client.s_AdmFactions;
		m_Filling = true;
		SetInput(m_InName, "");
		SetInput(m_InMotto, "");
		SetInput(m_InBase, "");
		SetInput(m_InSpawn, "");
		foreach (INT_PDA_FactionSummary s : facs)
		{
			if (s.Id != m_SelId)
				continue;
			SetInput(m_InName, s.Name);
			m_Emblem = INT_PDA_Util.ClampI(s.Emblem, 1, INT_PDA_Const.EMBLEMS);
			break;
		}
		m_Filling = false;
		m_MainDirty = false;
		RedrawCard();
	}

	protected void FillMain()
	{
		m_Filling = true;
		if (m_View)
		{
			SetInput(m_InName, m_View.Name);
			SetInput(m_InMotto, m_View.Motto);
			SetInput(m_InBase, m_View.Base);
			SetInput(m_InSpawn, m_View.SpawnPos);
			m_Emblem = INT_PDA_Util.ClampI(m_View.Emblem, 1, INT_PDA_Const.EMBLEMS);
		}
		else
		{
			SetInput(m_InName, "");
			SetInput(m_InMotto, "");
			SetInput(m_InBase, "");
			SetInput(m_InSpawn, "");
		}
		m_Filling = false;
		m_MainDirty = false;
		RedrawCard();
	}

	// Relations of the view (every other faction); edited, unsaved rows of the same faction are kept.
	protected void LoadRelations(bool other)
	{
		int i;
		int k;
		INT_PDA_Relation r;
		array<string> keepIds = new array<string>();
		array<int> keepVals = new array<int>();
		array<string> keepReasons = new array<string>();
		array<int> keepEdits = new array<int>();
		if (!other)
		{
			for (i = 0; i < m_RelIds.Count(); i++)
			{
				if (!m_RelDirty[i])
					continue;
				keepIds.Insert(m_RelIds[i]);
				keepVals.Insert(m_RelVals[i]);
				keepReasons.Insert(m_RelReasons[i]);
				keepEdits.Insert(m_RelEdit[i]);
			}
		}
		ClearRelations();
		if (!m_View)
			return;
		array<ref INT_PDA_Relation> rels = m_View.Relations;
		for (i = 0; i < rels.Count(); i++)
		{
			r = rels[i];
			m_RelIds.Insert(r.FactionId);
			m_RelNames.Insert(r.Name);
			m_RelEmblems.Insert(r.Emblem);
			k = keepIds.Find(r.FactionId);
			if (k >= 0)
			{
				m_RelVals.Insert(keepVals[k]);
				m_RelReasons.Insert(keepReasons[k]);
				m_RelDirty.Insert(true);
				m_RelEdit.Insert(keepEdits[k]);
			}
			else
			{
				m_RelVals.Insert(r.Rel);
				m_RelReasons.Insert(r.Reason);
				m_RelDirty.Insert(false);
				m_RelEdit.Insert(0);
			}
		}
	}

	// A saved relation answered OK (key "rel:<selected id>|<other id>"): its row is clean again — unless it is a row of
	// another selection now or it was edited after that save left (tag = its edit serial then).
	protected void ClearRelDirty(string key, int tag)
	{
		if (key.IndexOf("rel:") != 0)
			return;
		string pair = key.Substring(4, key.Length() - 4);
		int bar = pair.IndexOf("|");
		if (bar <= 0)
			return;
		string selId = pair.Substring(0, bar);
		string otherId = pair.Substring(bar + 1, pair.Length() - bar - 1);
		if (selId != m_SelId || m_New)
			return;
		int idx = m_RelIds.Find(otherId);
		if (idx < 0 || m_RelEdit[idx] != tag)
			return;
		m_RelDirty.Set(idx, false);
	}

	protected void MarkMainDirty()
	{
		m_MainDirty = true;
		m_MainEdits++;
	}

	protected void MarkRanksDirty()
	{
		m_RanksDirty = true;
		m_RankEdits++;
	}

	protected void MarkRelDirty(int idx)
	{
		if (idx < 0 || idx >= m_RelDirty.Count())
			return;
		m_RelDirty.Set(idx, true);
		int serial = m_RelEdit[idx] + 1;
		m_RelEdit.Set(idx, serial);
	}

	// Leader, deputies, members; online first inside every role.
	protected void LoadMembers()
	{
		int role;
		int pass;
		int i;
		bool wantOnline;
		INT_PDA_Member m;
		m_MemOrder.Clear();
		if (!m_View)
			return;
		array<ref INT_PDA_Member> mems = m_View.Members;
		for (role = INT_PDA_Const.ROLE_LEADER; role >= INT_PDA_Const.ROLE_NONE; role--)
		{
			for (pass = 0; pass < 2; pass++)
			{
				wantOnline = pass == 0;
				for (i = 0; i < mems.Count(); i++)
				{
					m = mems[i];
					if (m.Role != role || m.Online != wantOnline)
						continue;
					m_MemOrder.Insert(i);
				}
			}
		}
	}

	protected bool AnyDirty()
	{
		if (m_MainDirty || m_RanksDirty)
			return true;
		foreach (bool d : m_RelDirty)
		{
			if (d)
				return true;
		}
		return false;
	}

	protected int OnlineCount()
	{
		int n = 0;
		if (!m_View)
			return 0;
		array<ref INT_PDA_Member> mems = m_View.Members;
		foreach (INT_PDA_Member m : mems)
		{
			if (m.Online)
				n++;
		}
		return n;
	}

	protected int RankUse(string rank)
	{
		int n = 0;
		if (!m_View)
			return 0;
		array<ref INT_PDA_Member> mems = m_View.Members;
		foreach (INT_PDA_Member m : mems)
		{
			if (m.Rank == rank)
				n++;
		}
		return n;
	}

	protected string FactionName(string id)
	{
		if (m_View && m_View.Id == id)
			return m_View.Name;
		array<ref INT_PDA_FactionSummary> facs = INT_PDA_Client.s_AdmFactions;
		foreach (INT_PDA_FactionSummary s : facs)
		{
			if (s.Id == id)
				return s.Name;
		}
		return id;
	}

	// Names are unique, case-insensitive for Cyrillic too — the same rule as INT_PDA_SE_Admin.OnFactionSave (which
	// refuses with R_NAME_TAKEN); this check only saves the round trip.
	protected bool NameTaken(string name)
	{
		string want = INT_PDA_Util.LowerAll(name);
		string have;
		array<ref INT_PDA_FactionSummary> facs = INT_PDA_Client.s_AdmFactions;
		foreach (INT_PDA_FactionSummary s : facs)
		{
			if (!m_New && s.Id == m_SelId)
				continue;
			have = INT_PDA_Util.LowerAll(s.Name);
			if (have == want)
				return true;
		}
		return false;
	}

	// The first emblem no faction uses yet (a new faction starts with it).
	protected int FreeEmblem()
	{
		int e;
		bool used;
		array<ref INT_PDA_FactionSummary> facs = INT_PDA_Client.s_AdmFactions;
		for (e = 1; e <= INT_PDA_Const.EMBLEMS; e++)
		{
			used = false;
			foreach (INT_PDA_FactionSummary s : facs)
			{
				if (s.Emblem == e)
				{
					used = true;
					break;
				}
			}
			if (!used)
				return e;
		}
		return 1;
	}

	protected void SetInput(INT_UIF_TextInput t, string s)
	{
		if (t)
			t.SetText(s);
	}

	protected string GetInput(INT_UIF_TextInput t)
	{
		if (!t)
			return "";
		return t.GetText();
	}

	// ================================================================================================================
	// main section
	// ================================================================================================================
	// ADM_FACTION_SAVE: string id ("" = create), string name, int emblem, string motto, string base, string spawnPos
	// ("x, y, z" or "" = keep) -> sub, code, string id.
	protected void SaveMain()
	{
		string rawName = GetInput(m_InName);
		string rawMotto = GetInput(m_InMotto);
		string rawBase = GetInput(m_InBase);
		string rawSpawn = GetInput(m_InSpawn);
		string name;
		string motto;
		string baseText;
		string spawn = "";
		string id = "";
		int emb = m_Emblem;
		ScriptRPC rpc;
		if (!m_New && m_SelId == "")
			return;
		if (m_SaveBusy)
			return;
		if (!m_New && !m_View)
		{
			Fail("#STR_INT_PDA_AF_WAIT");
			return;
		}
		name = INT_PDA_Util.CleanLine(rawName, NAME_CHARS, INT_PDA_Const.MAX_NAME_BYTES);
		if (name == "")
		{
			Fail("#STR_INT_PDA_AF_NAME_EMPTY");
			return;
		}
		if (NameTaken(name))
		{
			Fail("#STR_INT_PDA_AF_NAME_TAKEN");
			return;
		}
		motto = INT_PDA_Util.CleanLine(rawMotto, MOTTO_CHARS, 400);
		baseText = INT_PDA_Util.CleanLine(rawBase, BASE_CHARS, 200);
		rawSpawn.TrimInPlace();
		if (rawSpawn != "")
		{
			spawn = NormalizePos(rawSpawn);
			if (spawn == "")
			{
				Fail("#STR_INT_PDA_AF_SPAWN_BAD");
				return;
			}
		}
		if (!m_New)
			id = m_SelId;
		rpc = INT_PDA_Client.Admin(INT_PDA_Const.ADM_FACTION_SAVE);
		INT_PDA_Proto.WriteStr(rpc, id);
		INT_PDA_Proto.WriteStr(rpc, name);
		rpc.Write(emb);
		INT_PDA_Proto.WriteStr(rpc, motto);
		INT_PDA_Proto.WriteStr(rpc, baseText);
		INT_PDA_Proto.WriteStr(rpc, spawn);
		m_SaveWasNew = m_New;
		m_SaveBusy = true;
		m_SaveBusyMs = NowMs();
		if (m_SaveBtn)
			m_SaveBtn.SetEnabled(false);
		if (!Enqueue(rpc, INT_PDA_Const.ADM_FACTION_SAVE, "", m_MainEdits))
		{
			ReleaseSave();
			return;
		}
		INT_PDA_Sfx.Play("Send");
	}

	// "x, y, z" / "x y z" -> "x.x, y.y, z.z" (the server replaces ',' by ' ' and reads ToVector; x = z = 0 is refused
	// there with a warning, so it is refused here). "" = not three numbers.
	protected string NormalizePos(string text)
	{
		string s = text + "";
		string q;
		string a;
		string b;
		string c;
		array<string> parts = new array<string>();
		array<float> nums = new array<float>();
		s.Replace(",", " ");
		s.Replace(";", " ");
		s.Split(" ", parts);
		foreach (string p : parts)
		{
			q = p + "";
			q.TrimInPlace();
			if (q == "")
				continue;
			if (!IsNum(q))
				return "";
			nums.Insert(q.ToFloat());
		}
		if (nums.Count() != 3)
			return "";
		float x = nums[0];
		float y = nums[1];
		float z = nums[2];
		if (x == 0 && z == 0)
			return "";
		if (Math.AbsFloat(x) > 100000 || Math.AbsFloat(y) > 100000 || Math.AbsFloat(z) > 100000)
			return "";
		a = Fmt1(x);
		b = Fmt1(y);
		c = Fmt1(z);
		return a + ", " + b + ", " + c;
	}

	protected bool IsNum(string q)
	{
		int i;
		int c;
		int dots = 0;
		int digits = 0;
		int n = q.Length();
		if (n == 0 || n > 14)
			return false;
		for (i = 0; i < n; i++)
		{
			c = q.Get(i).ToAscii();
			if (c >= 48 && c <= 57)
			{
				digits++;
				continue;
			}
			if (c == 46)
			{
				dots++;
				if (dots > 1)
					return false;
				continue;
			}
			if (c == 45 && i == 0)
				continue;
			return false;
		}
		return digits > 0;
	}

	// One decimal, rounded (0.1 m): 5077.25 -> "5077.3".
	protected string Fmt1(float v)
	{
		int t = (int)Math.Round(v * 10);
		bool neg = false;
		if (t < 0)
		{
			neg = true;
			t = -t;
		}
		int whole = t / 10;
		int frac = t % 10;
		string s = whole.ToString() + "." + frac.ToString();
		if (neg)
			s = "-" + s;
		return s;
	}

	protected string CardName()
	{
		string raw = GetInput(m_InName);
		string s = raw + "";
		s.TrimInPlace();
		if (s == "")
			s = Widget.TranslateString("#STR_INT_PDA_AF_CARD_NAME");
		return s;
	}

	// The vertical card (editor + popup) and the small header emblem follow the emblem and the typed name live.
	protected void RedrawCard()
	{
		string nm = CardName();
		string path = INT_PDA_Look.EmblemPath(m_Emblem);
		string no = "№ " + m_Emblem.ToString() + " / " + INT_PDA_Const.EMBLEMS.ToString();
		if (m_CardHost)
		{
			INT_UIF_Widgets.ClearChildren(m_CardHost);
			INT_PDA_Look.DrawFactionCard(m_CardHost, 0, 0, CARD_W, m_Emblem, nm);
		}
		if (m_PopCard && m_PopOpen)
		{
			INT_UIF_Widgets.ClearChildren(m_PopCard);
			INT_PDA_Look.DrawFactionCard(m_PopCard, 0, 0, POP_CARD_W, m_Emblem, nm);
			PlacePopSel();
		}
		if (m_HEmb)
			m_HEmb.LoadImageFile(0, path);
		if (m_EmbNo)
			m_EmbNo.SetText(no);
	}

	protected void PickEmblem(int n)
	{
		int e = INT_PDA_Util.ClampI(n, 1, INT_PDA_Const.EMBLEMS);
		if (e != m_Emblem)
		{
			m_Emblem = e;
			MarkMainDirty();
		}
		INT_PDA_Sfx.Play("Toggle");
		RedrawCard();
		if (m_PopOpen)
			RevealAt(m_PopCard, m_PopCardX, 64, 0);
		RefreshState();
	}

	protected void OpenPopup()
	{
		if (!m_Pop || m_PopOpen)
			return;
		SetFocus(null);
		INT_UIF_Dropdown.CloseActive();
		m_PopOpen = true;
		m_Pop.Show(true);
		if (m_PopHov)
			m_PopHov.Show(false);
		RedrawCard();
		RevealAt(m_PopBox, m_PopX, m_PopY, 0);
		INT_PDA_Sfx.Play("Click");
	}

	protected void ClosePopup()
	{
		if (!m_PopOpen)
			return;
		m_PopOpen = false;
		if (m_Pop)
			m_Pop.Show(false);
	}

	protected void PlacePopSel()
	{
		int idx = m_Emblem - 1;
		int col = idx % EMB_COLS;
		int grow = idx / EMB_COLS;
		float cx = 28 + col * (EMB_CELL + EMB_GAP) - 3;
		float cy = 64 + grow * (EMB_CELL + EMB_GAP) - 3;
		string cnt = m_Emblem.ToString() + " / " + INT_PDA_Const.EMBLEMS.ToString();
		INT_UIF_Draw.Move(m_PopSel, cx, cy);
		if (m_PopCount)
			m_PopCount.SetText(cnt);
	}

	protected void PlacePopHov(int i)
	{
		int col = i % EMB_COLS;
		int grow = i / EMB_COLS;
		float cx = 28 + col * (EMB_CELL + EMB_GAP) - 3;
		float cy = 64 + grow * (EMB_CELL + EMB_GAP) - 3;
		if (!m_PopHov)
			return;
		INT_UIF_Draw.Move(m_PopHov, cx, cy);
		m_PopHov.Show(true);
	}

	// ================================================================================================================
	// ranks section (local list; «Сохранить ранги» sends it whole)
	// ================================================================================================================
	protected bool AddRank(string raw)
	{
		if (!m_View || m_New)
		{
			Fail("#STR_INT_PDA_AF_WAIT");
			return false;
		}
		string r = INT_PDA_Util.CleanLine(raw, RANK_CHARS, INT_PDA_Const.MAX_NAME_BYTES);
		if (r == "")
		{
			Fail(INT_PDA_Look.ResultKey(INT_PDA_Const.R_TEXT));
			return false;
		}
		if (m_Ranks.Count() >= RANKS_MAX)
		{
			Fail(INT_PDA_Look.ResultKey(INT_PDA_Const.R_FULL));
			return false;
		}
		if (m_Ranks.Find(r) >= 0)
		{
			Fail("#STR_INT_PDA_AF_RANK_DUP");
			return false;
		}
		m_Ranks.Insert(r);
		MarkRanksDirty();
		int maxFirst = m_Ranks.Count() - RANK_ROWS;
		if (maxFirst > 0)
			m_First.Set(L_RANK, maxFirst);
		RefreshRanks();
		RefreshState();
		return true;
	}

	protected void DelRank(int idx)
	{
		if (idx < 0 || idx >= m_Ranks.Count())
			return;
		m_Ranks.RemoveOrdered(idx);
		MarkRanksDirty();
		RefreshRanks();
		RefreshState();
	}

	protected void MoveRank(int idx, int dir)
	{
		int to = idx + dir;
		int n = m_Ranks.Count();
		if (idx < 0 || to < 0 || idx >= n || to >= n)
			return;
		string a = m_Ranks[idx];
		string b = m_Ranks[to];
		m_Ranks.Set(idx, b);
		m_Ranks.Set(to, a);
		MarkRanksDirty();
		int first = m_First[L_RANK];
		if (to < first)
			m_First.Set(L_RANK, to);
		if (to >= first + RANK_ROWS)
			m_First.Set(L_RANK, to - RANK_ROWS + 1);
		RefreshRanks();
		RefreshState();
	}

	protected void AskRename(int idx)
	{
		if (!m_Menu || idx < 0 || idx >= m_Ranks.Count())
			return;
		m_RenameIdx = idx;
		string cur = m_Ranks[idx];
		m_RenameName = cur;
		INT_PDA_Prompt p = m_Menu.OpenPrompt("#STR_INT_PDA_AF_RANK_RENAME", "#STR_INT_PDA_AF_F_RANK", "", false, cur);
		if (!p)
			return;
		p.m_OnDone.Insert(OnRenameDone);
		// the menu prompt takes 60 characters, a rank keeps RANK_CHARS: the field stops there
		if (p.m_A)
			p.m_A.OnChanged().Insert(OnRenameTyped);
	}

	protected void OnRenameTyped(string text, INT_UIF_TextInput t)
	{
		if (!t || text.LengthUtf8() <= RANK_CHARS)
			return;
		string cut = text.SubstringUtf8(0, RANK_CHARS);
		t.SetText(cut);
	}

	// A rank the server already has is renamed on the server at once (ADM_RANK_RENAME: its holders keep it under the
	// new name — a renamed name inside «Сохранить ранги» would be a removed rank + a new one and the holders would lose
	// it). A rank added here and not saved yet is renamed locally only.
	protected void OnRenameDone(bool ok, string a, string b)
	{
		if (m_Menu)
			m_Menu.ClosePrompt();
		string was = m_RenameName;
		m_RenameIdx = -1;
		m_RenameName = "";
		if (!ok || was == "")
			return;
		// the list may have been refreshed while the prompt was open: the rank is found by its name
		int idx = m_Ranks.Find(was);
		if (idx < 0)
		{
			Fail(INT_PDA_Look.ResultKey(INT_PDA_Const.R_NOT_FOUND));
			return;
		}
		string r = INT_PDA_Util.CleanLine(a, RANK_CHARS, INT_PDA_Const.MAX_NAME_BYTES);
		if (r == "")
		{
			Fail(INT_PDA_Look.ResultKey(INT_PDA_Const.R_TEXT));
			return;
		}
		if (r == was)
			return;
		int dup = m_Ranks.Find(r);
		if (dup >= 0 && dup != idx)
		{
			Fail("#STR_INT_PDA_AF_RANK_DUP");
			return;
		}
		bool onServer = false;
		if (m_View && !m_New && m_SelId != "" && m_View.Ranks.Find(was) >= 0)
			onServer = true;
		m_Ranks.Set(idx, r);
		if (onServer)
			SendRankRename(was, r);
		else
			MarkRanksDirty();
		RefreshRanks();
		RefreshState();
	}

	protected void SaveRanks()
	{
		if (!m_View || m_New || m_SelId == "")
		{
			Fail("#STR_INT_PDA_AF_WAIT");
			return;
		}
		SendRanks();
	}

	protected void ResetRanks()
	{
		if (m_View)
			m_Ranks.Copy(m_View.Ranks);
		else
			m_Ranks.Clear();
		m_RanksDirty = false;
		RefreshRanks();
		RefreshState();
	}

	// ================================================================================================================
	// relations section
	// ================================================================================================================
	protected void SaveRelation(int idx)
	{
		if (idx < 0 || idx >= m_RelIds.Count() || m_SelId == "" || m_New)
			return;
		int rel = m_RelVals[idx];
		string raw = m_RelReasons[idx];
		string reason = INT_PDA_Util.CleanLine(raw, REASON_CHARS, 400);
		string otherId = m_RelIds[idx];
		// owner 2026-10-22: hostility and alliances need an RP reason
		if (rel != INT_PDA_Const.REL_NEUTRAL && reason == "")
		{
			Fail("#STR_INT_PDA_AF_REASON_NEEDED");
			return;
		}
		SendRelation(m_SelId, otherId, rel, reason);
	}

	protected int RelColor(int rel)
	{
		if (rel == INT_PDA_Const.REL_ALLY)
			return INT_PDA_Look.ALLY;
		if (rel == INT_PDA_Const.REL_ENEMY)
			return INT_PDA_Look.ENEMY;
		return INT_PDA_Look.NEUTRAL;
	}

	protected string RelKey(int rel)
	{
		if (rel == INT_PDA_Const.REL_ALLY)
			return "#STR_INT_PDA_AF_REL_ALLY";
		if (rel == INT_PDA_Const.REL_ENEMY)
			return "#STR_INT_PDA_AF_REL_ENEMY";
		return "#STR_INT_PDA_AF_REL_NEUTRAL";
	}

	// dropdown order: alliance, neutral, hostile
	protected int RelToIndex(int rel)
	{
		if (rel == INT_PDA_Const.REL_ALLY)
			return 0;
		if (rel == INT_PDA_Const.REL_ENEMY)
			return 2;
		return 1;
	}

	protected int IndexToRel(int index)
	{
		if (index == 0)
			return INT_PDA_Const.REL_ALLY;
		if (index == 2)
			return INT_PDA_Const.REL_ENEMY;
		return INT_PDA_Const.REL_NEUTRAL;
	}

	// ================================================================================================================
	// refresh
	// ================================================================================================================
	protected int RowsOf(int list)
	{
		if (list == L_FAC)
			return FAC_ROWS;
		if (list == L_MEM)
			return MEM_ROWS;
		if (list == L_RANK)
			return RANK_ROWS;
		return REL_ROWS;
	}

	protected int ListCount(int list)
	{
		if (list == L_FAC)
			return INT_PDA_Client.s_AdmFactions.Count();
		if (list == L_MEM)
			return m_MemOrder.Count();
		if (list == L_RANK)
			return m_Ranks.Count();
		return m_RelIds.Count();
	}

	protected void ClampFirst(int list)
	{
		int n = ListCount(list);
		int maxFirst = n - RowsOf(list);
		if (maxFirst < 0)
			maxFirst = 0;
		int cur = m_First[list];
		int first = INT_PDA_Util.ClampI(cur, 0, maxFirst);
		m_First.Set(list, first);
	}

	// Scroll by whole rows; true when the first row changed.
	protected bool ScrollList(int list, int delta)
	{
		int n = ListCount(list);
		int maxFirst = n - RowsOf(list);
		if (maxFirst < 0)
			maxFirst = 0;
		int cur = m_First[list];
		int first = INT_PDA_Util.ClampI(cur + delta, 0, maxFirst);
		if (first == cur)
			return false;
		m_First.Set(list, first);
		RefreshListOf(list);
		return true;
	}

	protected void RefreshListOf(int list)
	{
		if (list == L_FAC)
			RefreshFactions();
		else if (list == L_MEM)
			RefreshMembers();
		else if (list == L_RANK)
			RefreshRanks();
		else
			RefreshRelations();
	}

	protected void RefreshPager(int list)
	{
		int n = ListCount(list);
		int rows = RowsOf(list);
		int first = m_First[list];
		int last = first + rows;
		if (last > n)
			last = n;
		bool many = n > rows;
		int prev = m_PgPrev[list];
		int next = m_PgNext[list];
		TextWidget t = m_PgText[list];
		ShowIcon(prev, many);
		ShowIcon(next, many);
		SetIconOn(prev, first > 0);
		SetIconOn(next, last < n);
		if (!t)
			return;
		t.Show(many);
		string a = (first + 1).ToString();
		string b = last.ToString();
		string c = n.ToString();
		t.SetText(a + "–" + b + " / " + c);
	}

	protected Widget RowWidget(int list, int i)
	{
		if (list == L_FAC && i < m_LRow.Count())
			return m_LRow[i];
		if (list == L_MEM && i < m_MRow.Count())
			return m_MRow[i];
		if (list == L_RANK && i < m_RRow.Count())
			return m_RRow[i];
		if (list == L_REL && i < m_ERow.Count())
			return m_ERow[i];
		return null;
	}

	// The visible rows of a list cascade in (data loaded, page turned).
	protected void RevealRows(int list)
	{
		int i;
		int rows = RowsOf(list);
		float x = 0;
		float top = REL_TOP;
		float step = REL_ROW_H;
		Widget row;
		if (list == L_FAC)
		{
			x = 8;
			top = FAC_TOP;
			step = FAC_ROW_H;
		}
		else if (list == L_MEM)
		{
			top = MEM_TOP;
			step = MEM_ROW_H;
		}
		else if (list == L_RANK)
		{
			top = RANK_TOP;
			step = RANK_ROW_H;
		}
		for (i = 0; i < rows; i++)
		{
			row = RowWidget(list, i);
			if (!row || !row.IsVisible())
				continue;
			RevealAt(row, x, top + i * step, 0.02 + i * 0.03);
		}
	}

	protected void RevealAt(Widget w, float x, float y, float delay)
	{
		if (!w)
			return;
		INT_UIF_Animator.Get().Cancel(w);
		INT_UIF_Draw.Move(w, x, y);
		INT_UIF_Animator.Reveal(w, delay, 10, 0.3);
	}

	protected string SummaryLine(INT_PDA_FactionSummary s)
	{
		string ppl = Widget.TranslateString("#STR_INT_PDA_AF_PPL");
		string onl = Widget.TranslateString("#STR_INT_PDA_AF_ONLINE");
		string roleKey = INT_PDA_Look.RoleKey(INT_PDA_Const.ROLE_LEADER);
		string roleL = Widget.TranslateString(roleKey);
		string lead = s.Leader;
		if (lead == "")
			lead = Widget.TranslateString("#STR_INT_PDA_AF_NO_LEADER");
		int members = s.Members;
		int online = s.Online;
		string a = members.ToString();
		string b = online.ToString();
		return a + " " + ppl + " · " + b + " " + onl + " · " + roleL + ": " + lead;
	}

	protected void RefreshFactions()
	{
		int i;
		int idx;
		int fill;
		int border;
		bool sel;
		string path;
		string line;
		INT_PDA_FactionSummary s;
		Widget row;
		INT_UIF_Surface box;
		array<ref INT_PDA_FactionSummary> facs = INT_PDA_Client.s_AdmFactions;
		int n = facs.Count();
		if (!m_LCount)
			return;
		ClampFirst(L_FAC);
		int first = m_First[L_FAC];
		m_LCount.SetText(n.ToString());
		m_LEmpty.Show(n == 0);
		for (i = 0; i < m_LRow.Count(); i++)
		{
			row = m_LRow[i];
			idx = first + i;
			if (idx >= n)
			{
				row.Show(false);
				continue;
			}
			row.Show(true);
			s = facs[idx];
			path = INT_PDA_Look.EmblemPath(s.Emblem);
			m_LEmb[i].LoadImageFile(0, path);
			// frames do not clip text: a long name / leader is cut with «…» inside the row
			line = INT_PDA_Look.Fit(s.Name, LEFT_W - 100, 19);
			m_LName[i].SetText(line);
			line = SummaryLine(s);
			line = INT_PDA_Look.Fit(line, LEFT_W - 100, 13);
			m_LSub[i].SetText(line);
			sel = false;
			if (!m_New && s.Id == m_SelId)
				sel = true;
			m_LBar[i].Show(sel);
			fill = INT_PDA_Look.PANEL;
			border = INT_PDA_Look.LINE;
			if (sel)
			{
				fill = INT_PDA_Look.PANEL2;
				border = INT_PDA_Look.ORANGE2;
			}
			box = m_LBox[i];
			if (box)
			{
				box.SetFill(fill);
				box.SetBorder(border);
			}
		}
		RefreshPager(L_FAC);
	}

	protected void RefreshEditor()
	{
		int i;
		bool has = false;
		if (!m_EdBody)
			return;
		if (m_New || m_SelId != "")
			has = true;
		m_EdEmpty.Show(!has);
		m_EdBody.Show(has);
		if (!has)
			return;
		if (m_New)
			m_Sec = SEC_MAIN;
		for (i = 0; i < m_SecFrame.Count(); i++)
			m_SecFrame[i].Show(i == m_Sec);
		TabColors();
		PlaceTabLine(false);
		RefreshHeader();
		RefreshMainBits();
		RefreshMembers();
		RefreshRanks();
		RefreshRelations();
	}

	protected void TabColors()
	{
		int i;
		int col;
		for (i = 0; i < m_SecText.Count(); i++)
		{
			col = INT_PDA_Look.DIM;
			if (m_New && i != SEC_MAIN)
				col = INT_PDA_Look.DIM2;
			if (i == m_Sec)
				col = INT_PDA_Look.TEXT;
			m_SecText[i].SetColor(col);
		}
	}

	protected void PlaceTabLine(bool animate)
	{
		if (!m_SecLine || m_Sec < 0 || m_Sec >= m_SecX.Count())
			return;
		float lx = m_SecX[m_Sec];
		float lw = m_SecW[m_Sec];
		if (animate)
		{
			INT_UIF_Animator.MoveX(m_SecLine, INT_UIF_Draw.S(lx), 0.24, INT_UIF_Ease.OUT_CUBIC);
			INT_UIF_Animator.ResizeW(m_SecLine, INT_UIF_Draw.S(lw), 0.24, INT_UIF_Ease.OUT_CUBIC);
			return;
		}
		INT_UIF_Animator.Get().Cancel(m_SecLine);
		INT_UIF_Draw.Place(m_SecLine, lx, TAB_Y + 38, lw, 3);
	}

	protected void ShowSection(int s, bool sound)
	{
		int i;
		if (s < 0 || s >= m_SecFrame.Count())
			return;
		if (!m_New && m_SelId == "")
			return;
		if (m_New && s != SEC_MAIN)
		{
			Fail("#STR_INT_PDA_AF_NEW_FIRST");
			return;
		}
		if (s == m_Sec)
			return;
		ClosePopup();
		INT_UIF_Dropdown.CloseActive();
		m_Sec = s;
		for (i = 0; i < m_SecFrame.Count(); i++)
			m_SecFrame[i].Show(i == s);
		TabColors();
		PlaceTabLine(true);
		Widget f = m_SecFrame[s];
		RevealAt(f, 0, SEC_Y, 0.02);
		if (s == SEC_MEMBERS)
			RevealRows(L_MEM);
		else if (s == SEC_RANKS)
			RevealRows(L_RANK);
		else if (s == SEC_RELATIONS)
			RevealRows(L_REL);
		if (sound)
			INT_PDA_Sfx.Play("Tab");
	}

	protected void RefreshHeader()
	{
		string title = "";
		string sub = "";
		string idWord = Widget.TranslateString("#STR_INT_PDA_AF_ID");
		string ppl = Widget.TranslateString("#STR_INT_PDA_AF_PPL");
		string onl = Widget.TranslateString("#STR_INT_PDA_AF_ONLINE");
		int total;
		int online;
		if (!m_HName)
			return;
		if (m_New)
		{
			title = Widget.TranslateString("#STR_INT_PDA_AF_NEW_TITLE");
			sub = Widget.TranslateString("#STR_INT_PDA_AF_NEW_SUB");
		}
		else
		{
			title = FactionName(m_SelId);
			sub = idWord + " " + m_SelId;
			if (m_View)
			{
				total = m_View.Members.Count();
				online = OnlineCount();
				sub = sub + "  ·  " + total.ToString() + " " + ppl + "  ·  " + online.ToString() + " " + onl;
			}
		}
		// string.ToUpper is ASCII only: Caps raises Cyrillic names too; then cut to the header width
		string up = "";
		if (title != "")
			up = INT_PDA_Look.Caps(title);
		up = INT_PDA_Look.Fit(up, m_EW - 420, 34);
		m_HName.SetText(up);
		sub = INT_PDA_Look.Fit(sub, m_EW - 420, 14);
		m_HSub.SetText(sub);
		RefreshState();
	}

	protected void RefreshState()
	{
		string st = "";
		if (!m_HState)
			return;
		if (m_Loading)
			st = INT_PDA_Look.Caps("#STR_INT_PDA_AF_LOADING");
		else if (AnyDirty())
			st = INT_PDA_Look.Caps("#STR_INT_PDA_AF_UNSAVED");
		m_HState.SetText(st);
		if (m_Loading && !m_Pulsing)
		{
			m_Pulsing = true;
			INT_UIF_Animator.Pulse(m_HState, 0.35, 1.0, 1.1);
		}
		else if (!m_Loading && m_Pulsing)
		{
			m_Pulsing = false;
			INT_UIF_Animator.Get().Cancel(m_HState);
			m_HState.SetAlpha(1);
		}
		if (m_RState)
		{
			st = "";
			if (m_RanksDirty)
				st = INT_PDA_Look.Caps("#STR_INT_PDA_AF_UNSAVED");
			m_RState.SetText(st);
		}
	}

	protected void RefreshMainBits()
	{
		string key = "#STR_INT_PDA_AF_SAVE";
		if (m_New)
			key = "#STR_INT_PDA_AF_CREATE_BTN";
		string lbl = INT_PDA_Look.Caps(key);
		string idText = m_SelId;
		if (m_New || idText == "")
			idText = "—";
		if (m_SaveBtn)
			m_SaveBtn.SetText(lbl);
		if (m_DelBtn)
			m_DelBtn.Show(!m_New);
		if (m_IdText)
			m_IdText.SetText(idText);
	}

	protected void RefreshMembers()
	{
		int i;
		int idx;
		int mi;
		int sel;
		int dotColor;
		string rank;
		string joined;
		INT_PDA_Member m;
		ImageWidget ic;
		INT_UIF_Dropdown dd;
		array<ref INT_PDA_Member> mems;
		if (!m_MCaption)
			return;
		if (m_View)
			mems = m_View.Members;
		int n = m_MemOrder.Count();
		int online = OnlineCount();
		string capM = INT_PDA_Look.Caps("#STR_INT_PDA_AF_MEMBERS");
		string onl = Widget.TranslateString("#STR_INT_PDA_AF_ONLINE");
		string capL = INT_PDA_Look.Caps("#STR_INT_PDA_AF_LOADING");
		string txt = capM + "  ·  " + n.ToString() + "  ·  " + online.ToString() + " " + onl;
		if (!m_View && m_Loading)
			txt = capM + "  ·  " + capL;
		m_MCaption.SetText(txt);
		bool noRows = false;
		if (n == 0 && m_View)
			noRows = true;
		m_MEmpty.Show(noRows);
		// the rows are drawn anew: an open role list belonged to the old row content
		CloseRowDrops(m_MRole);
		ClampFirst(L_MEM);
		int first = m_First[L_MEM];
		for (i = 0; i < m_MRow.Count(); i++)
		{
			idx = first + i;
			if (i < m_MRowSteam.Count())
				m_MRowSteam.Set(i, "");
			if (!mems || idx >= n)
			{
				m_MRow[i].Show(false);
				continue;
			}
			mi = m_MemOrder[idx];
			if (mi < 0 || mi >= mems.Count())
			{
				m_MRow[i].Show(false);
				continue;
			}
			m_MRow[i].Show(true);
			m = mems[mi];
			if (i < m_MRowSteam.Count())
				m_MRowSteam.Set(i, m.SteamId);
			dotColor = INT_PDA_Look.DIM2;
			if (m.Online)
				dotColor = INT_PDA_Look.ALLY;
			m_MDot[i].SetColor(dotColor);
			ic = m_MRoleIc[i];
			if (ic)
			{
				if (m.Role == INT_PDA_Const.ROLE_LEADER)
				{
					INT_UIF_Widgets.SetIcon(ic, "crown");
					ic.SetColor(INT_PDA_Look.ORANGE);
					ic.Show(true);
				}
				else if (m.Role == INT_PDA_Const.ROLE_DEPUTY)
				{
					INT_UIF_Widgets.SetIcon(ic, "star");
					ic.SetColor(INT_PDA_Look.OWN);
					ic.Show(true);
				}
				else
				{
					ic.Show(false);
				}
			}
			// frames do not clip text: long callsigns / ranks are cut with «…» inside their columns
			joined = INT_PDA_Look.Fit(m.Callsign, 240, 17);
			m_MName[i].SetText(joined);
			m_MSteam[i].SetText(m.SteamId);
			rank = m.Rank;
			if (rank == "")
				rank = "—";
			rank = INT_PDA_Look.Fit(rank, 170, 15);
			m_MRank[i].SetText(rank);
			joined = DateOf(m.Joined);
			m_MJoined[i].SetText(joined);
			dd = m_MRole[i];
			sel = INT_PDA_Util.ClampI(m.Role, INT_PDA_Const.ROLE_NONE, INT_PDA_Const.ROLE_LEADER);
			if (dd)
				dd.SetSelected(sel, false);
		}
		RefreshPager(L_MEM);
		RefreshJoin();
	}

	protected void RefreshJoin()
	{
		bool inThis = false;
		bool can = true;
		string key = "#STR_INT_PDA_AF_JOIN";
		string lbl;
		INT_PDA_Profile me = INT_PDA_Client.s_Profile;
		if (!m_JoinBtn)
			return;
		if (me && m_SelId != "" && me.FactionId == m_SelId)
			inThis = true;
		if (inThis)
		{
			key = "#STR_INT_PDA_AF_JOINED";
			can = false;
		}
		if (m_New || m_SelId == "")
			can = false;
		lbl = INT_PDA_Look.Caps(key);
		m_JoinBtn.SetText(lbl);
		m_JoinBtn.SetEnabled(can);
	}

	// "YYYY-MM-DD" -> "DD.MM.YYYY"
	protected string DateOf(string stamp)
	{
		if (stamp.Length() < 10)
			return stamp;
		string d = stamp.Substring(8, 2);
		string mo = stamp.Substring(5, 2);
		string y = stamp.Substring(0, 4);
		return d + "." + mo + "." + y;
	}

	protected void RefreshRanks()
	{
		int i;
		int idx;
		int use;
		int base4;
		int icUp;
		int icDown;
		int icEdit;
		int icDel;
		string nm;
		string num;
		string useText;
		if (!m_RCaption)
			return;
		int n = m_Ranks.Count();
		int maxR = RANKS_MAX;
		string capR = INT_PDA_Look.Caps("#STR_INT_PDA_AF_RANKS");
		string ppl = Widget.TranslateString("#STR_INT_PDA_AF_PPL");
		m_RCaption.SetText(capR + "  ·  " + n.ToString() + " / " + maxR.ToString());
		bool noRows = false;
		if (n == 0 && m_View)
			noRows = true;
		m_REmpty.Show(noRows);
		ClampFirst(L_RANK);
		int first = m_First[L_RANK];
		for (i = 0; i < m_RRow.Count(); i++)
		{
			idx = first + i;
			if (idx >= n)
			{
				m_RRow[i].Show(false);
				continue;
			}
			m_RRow[i].Show(true);
			nm = m_Ranks[idx];
			num = (idx + 1).ToStringLen(2);
			m_RNum[i].SetText(num);
			useText = INT_PDA_Look.Fit(nm, 520, 17);
			m_RName[i].SetText(useText);
			use = RankUse(nm);
			useText = "";
			if (use > 0)
				useText = use.ToString() + " " + ppl;
			m_RUse[i].SetText(useText);
			base4 = i * 4;
			icUp = m_RIc[base4];
			icDown = m_RIc[base4 + 1];
			icEdit = m_RIc[base4 + 2];
			icDel = m_RIc[base4 + 3];
			SetIconOn(icUp, idx > 0);
			SetIconOn(icDown, idx < n - 1);
			SetIconOn(icEdit, true);
			SetIconOn(icDel, true);
		}
		RefreshPager(L_RANK);
		bool loaded = false;
		if (m_View && !m_New)
			loaded = true;
		bool canSave = false;
		if (loaded && m_RanksDirty)
			canSave = true;
		bool canAdd = false;
		if (loaded && n < RANKS_MAX)
			canAdd = true;
		if (m_RankSaveBtn)
			m_RankSaveBtn.SetEnabled(canSave);
		if (m_RankResetBtn)
			m_RankResetBtn.SetEnabled(canSave);
		if (m_RankAddBtn)
			m_RankAddBtn.SetEnabled(canAdd);
		RefreshState();
	}

	protected void RefreshRelations()
	{
		int i;
		int idx;
		int emb;
		int sel;
		string path;
		string nm;
		string reason;
		INT_UIF_Dropdown dd;
		INT_UIF_TextInput ti;
		if (!m_ECaption)
			return;
		int n = m_RelIds.Count();
		string capE = INT_PDA_Look.Caps("#STR_INT_PDA_AF_RELATIONS");
		m_ECaption.SetText(capE + "  ·  " + n.ToString());
		bool noRows = false;
		if (n == 0 && m_View)
			noRows = true;
		m_EEmpty.Show(noRows);
		// the rows are drawn anew: an open relation list belonged to the old row content
		CloseRowDrops(m_ERel);
		ClampFirst(L_REL);
		int first = m_First[L_REL];
		m_Filling = true;
		for (i = 0; i < m_ERow.Count(); i++)
		{
			idx = first + i;
			if (i < m_ERowId.Count())
				m_ERowId.Set(i, "");
			if (idx >= n)
			{
				m_ERow[i].Show(false);
				continue;
			}
			m_ERow[i].Show(true);
			if (i < m_ERowId.Count())
				m_ERowId.Set(i, m_RelIds[idx]);
			emb = m_RelEmblems[idx];
			path = INT_PDA_Look.EmblemPath(emb);
			m_EEmb[i].LoadImageFile(0, path);
			nm = INT_PDA_Look.Fit(m_RelNames[idx], 250, 18);
			m_EName[i].SetText(nm);
			dd = m_ERel[i];
			sel = RelToIndex(m_RelVals[idx]);
			if (dd)
				dd.SetSelected(sel, false);
			ti = m_EReason[i];
			reason = m_RelReasons[idx];
			if (ti)
				ti.SetText(reason);
			RefreshRelRowState(i);
		}
		m_Filling = false;
		RefreshPager(L_REL);
	}

	// An open list of these rows closes (the rows are about to show other members / factions).
	protected void CloseRowDrops(array<INT_UIF_Dropdown> drops)
	{
		foreach (INT_UIF_Dropdown d : drops)
		{
			if (d && d.IsOpen())
				d.ClosePopup();
		}
	}

	protected void RefreshRelRowState(int row)
	{
		int idx = m_First[L_REL] + row;
		if (idx < 0 || idx >= m_RelIds.Count() || row < 0 || row >= m_ERow.Count())
			return;
		int rel = m_RelVals[idx];
		int c = RelColor(rel);
		string key = RelKey(rel);
		string st = INT_PDA_Look.Caps(key);
		string un = INT_PDA_Look.Caps("#STR_INT_PDA_AF_UNSAVED");
		bool dirty = m_RelDirty[idx];
		if (dirty)
			st = st + "  ·  " + un;
		st = INT_PDA_Look.Fit(st, 260, 13);
		m_EBar[row].SetColor(c);
		m_EState[row].SetText(st);
		m_EState[row].SetColor(c);
		INT_UIF_Button sb = m_ESave[row];
		if (sb)
			sb.SetEnabled(dirty);
	}

	// ================================================================================================================
	// events: own hit zones
	// ================================================================================================================
	override bool OnClick(Widget w, int x, int y, int button)
	{
		int i;
		if (button != MouseState.LEFT)
			return false;
		ButtonWidget b = ButtonWidget.Cast(w);
		if (!b)
			return false;
		if (m_PopOpen)
		{
			if (b == m_PopBack)
			{
				INT_PDA_Sfx.Play("Click");
				ClosePopup();
				return true;
			}
			if (b == m_PopBoxHit)
				return true;
			i = m_PopHits.Find(b);
			if (i >= 0)
			{
				PickEmblem(i + 1);
				return true;
			}
			return false;
		}
		i = m_IcHit.Find(b);
		if (i >= 0)
		{
			OnIcon(i);
			return true;
		}
		i = m_LHit.Find(b);
		if (i >= 0)
		{
			OnFactionRow(i);
			return true;
		}
		i = m_SecHit.Find(b);
		if (i >= 0)
		{
			ShowSection(i, true);
			return true;
		}
		return false;
	}

	protected void OnFactionRow(int row)
	{
		array<ref INT_PDA_FactionSummary> facs = INT_PDA_Client.s_AdmFactions;
		int idx = m_First[L_FAC] + row;
		if (idx < 0 || idx >= facs.Count())
			return;
		INT_PDA_FactionSummary s = facs[idx];
		INT_PDA_Sfx.Play("Click");
		SelectFaction(s.Id);
	}

	protected void OnIcon(int ic)
	{
		int list;
		int k;
		int row;
		int kind;
		int idx;
		int rows;
		if (ic < 0 || ic >= m_IcOn.Count() || !m_IcOn[ic])
			return;
		if (ic == m_RefreshIc)
		{
			INT_PDA_Sfx.Play("Click");
			AskList();
			if (m_SelId != "" && !m_New)
				AskView(m_SelId);
			return;
		}
		for (list = 0; list < 4; list++)
		{
			rows = RowsOf(list);
			if (ic == m_PgPrev[list])
			{
				INT_PDA_Sfx.Play("Click");
				if (ScrollList(list, -rows))
					RevealRows(list);
				return;
			}
			if (ic == m_PgNext[list])
			{
				INT_PDA_Sfx.Play("Click");
				if (ScrollList(list, rows))
					RevealRows(list);
				return;
			}
		}
		k = m_RIc.Find(ic);
		if (k < 0)
			return;
		row = k / 4;
		kind = k % 4;
		idx = m_First[L_RANK] + row;
		if (idx < 0 || idx >= m_Ranks.Count())
			return;
		INT_PDA_Sfx.Play("Click");
		if (kind == 0)
			MoveRank(idx, -1);
		else if (kind == 1)
			MoveRank(idx, 1);
		else if (kind == 2)
			AskRename(idx);
		else
			DelRank(idx);
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int i;
		ImageWidget img;
		ButtonWidget b = ButtonWidget.Cast(w);
		if (!b)
			return false;
		i = m_IcHit.Find(b);
		if (i >= 0)
		{
			img = m_IcImg[i];
			if (img && m_IcOn[i])
				img.SetColor(INT_PDA_Look.ORANGE);
			return false;
		}
		i = m_LHit.Find(b);
		if (i >= 0)
		{
			HoverFaction(i, true);
			return false;
		}
		i = m_SecHit.Find(b);
		if (i >= 0)
		{
			if (i != m_Sec && !m_New)
				m_SecText[i].SetColor(INT_PDA_Look.TEXT);
			return false;
		}
		if (m_PopOpen)
		{
			i = m_PopHits.Find(b);
			if (i >= 0)
				PlacePopHov(i);
		}
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int i;
		int c;
		ImageWidget img;
		ButtonWidget b = ButtonWidget.Cast(w);
		if (!b)
			return false;
		i = m_IcHit.Find(b);
		if (i >= 0)
		{
			img = m_IcImg[i];
			c = m_IcBase[i];
			if (!m_IcOn[i])
				c = INT_PDA_Look.LINE2;
			if (img)
				img.SetColor(c);
			return false;
		}
		i = m_LHit.Find(b);
		if (i >= 0)
		{
			HoverFaction(i, false);
			return false;
		}
		i = m_SecHit.Find(b);
		if (i >= 0)
		{
			TabColors();
			return false;
		}
		if (m_PopHov && m_PopHits.Find(b) >= 0)
			m_PopHov.Show(false);
		return false;
	}

	// One wheel notch = one row.
	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (m_PopOpen)
			return false;
		ButtonWidget b = ButtonWidget.Cast(w);
		if (!b)
			return false;
		int list = ListOfHit(b);
		if (list < 0)
			return false;
		ScrollList(list, -wheel);
		return true;
	}

	protected int ListOfHit(ButtonWidget b)
	{
		int i = m_AreaHit.Find(b);
		if (i >= 0)
			return i;
		if (m_LHit.Find(b) >= 0)
			return L_FAC;
		if (m_MHit.Find(b) >= 0)
			return L_MEM;
		if (m_RHit.Find(b) >= 0)
			return L_RANK;
		if (m_EHit.Find(b) >= 0)
			return L_REL;
		int ic = m_IcHit.Find(b);
		if (ic < 0)
			return -1;
		if (m_RIc.Find(ic) >= 0)
			return L_RANK;
		for (i = 0; i < 4; i++)
		{
			if (ic == m_PgPrev[i] || ic == m_PgNext[i])
				return i;
		}
		return -1;
	}

	protected void HoverFaction(int row, bool on)
	{
		if (row < 0 || row >= m_LBox.Count())
			return;
		INT_UIF_Surface box = m_LBox[row];
		if (!box || IsSelRow(row))
			return;
		int c = INT_PDA_Look.PANEL;
		if (on)
			c = INT_PDA_Look.PANEL2;
		box.FadeFill(c, 0.12);
	}

	protected bool IsSelRow(int row)
	{
		if (m_New || m_SelId == "")
			return false;
		array<ref INT_PDA_FactionSummary> facs = INT_PDA_Client.s_AdmFactions;
		int idx = m_First[L_FAC] + row;
		if (idx < 0 || idx >= facs.Count())
			return false;
		INT_PDA_FactionSummary s = facs[idx];
		return s.Id == m_SelId;
	}

	// ================================================================================================================
	// events: UIF components
	// ================================================================================================================
	protected void OnCreateClick(INT_UIF_Button b)
	{
		INT_PDA_Sfx.Play("Click");
		StartNew();
	}

	protected void OnEmblemClick(INT_UIF_Button b)
	{
		OpenPopup();
	}

	protected void OnPopDoneClick(INT_UIF_Button b)
	{
		INT_PDA_Sfx.Play("Click");
		ClosePopup();
	}

	protected void OnSaveClick(INT_UIF_Button b)
	{
		SaveMain();
	}

	// «Моя позиция»: the local player's position, 0.1 m, in the server's own "x, y, z" format.
	protected void OnPosClick(INT_UIF_Button b)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
		{
			Fail("#STR_INT_PDA_AF_NO_PLAYER");
			return;
		}
		vector pos = p.GetPosition();
		string sx = Fmt1(pos[0]);
		string sy = Fmt1(pos[1]);
		string sz = Fmt1(pos[2]);
		string txt = sx + ", " + sy + ", " + sz;
		m_Filling = true;
		SetInput(m_InSpawn, txt);
		m_Filling = false;
		MarkMainDirty();
		RefreshState();
		INT_PDA_Sfx.Play("Click");
		INT_UIF_Toast.Info("#STR_INT_PDA_AF_POS_SET");
	}

	// Delete: type the faction's name (or its ID) to confirm.
	protected void OnDeleteClick(INT_UIF_Button b)
	{
		if (m_New || m_SelId == "" || !m_Menu)
			return;
		string nm = FactionName(m_SelId);
		string head = Widget.TranslateString("#STR_INT_PDA_AF_DEL_TITLE");
		string title = head + " «" + nm + "»";
		m_DeletingId = m_SelId;
		INT_PDA_Prompt p = m_Menu.OpenPrompt(title, "#STR_INT_PDA_AF_DEL_CONFIRM", "", false);
		if (p)
			p.m_OnDone.Insert(OnDeleteDone);
	}

	protected void OnDeleteDone(bool ok, string a, string b)
	{
		if (m_Menu)
			m_Menu.ClosePrompt();
		string id = m_DeletingId;
		if (!ok || id == "")
			return;
		string typed = a + "";
		typed.TrimInPlace();
		// string.ToLower is ASCII only: «Долг» and «долг» must match too
		string typedLow = INT_PDA_Util.LowerAll(typed);
		string nm = FactionName(id);
		string nmLow = INT_PDA_Util.LowerAll(nm);
		bool confirmed = false;
		if (typed != "" && typedLow == nmLow)
			confirmed = true;
		if (typed == id)
			confirmed = true;
		if (!confirmed)
		{
			Fail("#STR_INT_PDA_AF_DEL_MISMATCH");
			return;
		}
		SendDelete(id);
	}

	// Role dropdown of a member row: member / deputy / leader -> ADM_SET_ROLE; «Исключить» -> confirm -> ADM_KICK.
	// The pick acts on the member the row showed when it was drawn (rows are redrawn by scrolling / a fresh view; an
	// open list is closed then, see CloseRowDrops) and only if he is still in the faction on screen.
	protected void OnRolePicked(int index, string text, INT_UIF_Dropdown d)
	{
		int row = m_MRole.Find(d);
		if (row < 0 || row >= m_MRowSteam.Count() || !m_View)
			return;
		string steam = m_MRowSteam[row];
		if (steam == "")
			return;
		INT_PDA_Member m = null;
		array<ref INT_PDA_Member> mems = m_View.Members;
		foreach (INT_PDA_Member cand : mems)
		{
			if (cand.SteamId == steam)
			{
				m = cand;
				break;
			}
		}
		if (!m)
		{
			RefreshMembers();
			return;
		}
		if (index == m.Role)
			return;
		INT_PDA_Sfx.Play("Click");
		if (index <= INT_PDA_Const.ROLE_NONE)
		{
			AskKick(m);
			return;
		}
		SendRole(m.SteamId, index);
	}

	protected void AskKick(INT_PDA_Member m)
	{
		if (!m_Menu)
		{
			RefreshMembers();
			return;
		}
		m_KickSteam = m.SteamId;
		INT_PDA_Prompt p = m_Menu.OpenPrompt("#STR_INT_PDA_AF_KICK_TITLE", "#STR_INT_PDA_AF_COL_CALLSIGN", "", false, m.Callsign);
		if (p)
			p.m_OnDone.Insert(OnKickDone);
	}

	protected void OnKickDone(bool ok, string a, string b)
	{
		if (m_Menu)
			m_Menu.ClosePrompt();
		string steam = m_KickSteam;
		m_KickSteam = "";
		if (!ok || steam == "")
		{
			RefreshMembers();
			return;
		}
		SendKick(steam);
	}

	// «Вступить самому» (owner item 18). From another faction the admin moves over (confirmed first).
	protected void OnJoinClick(INT_UIF_Button b)
	{
		INT_PDA_Profile me = INT_PDA_Client.s_Profile;
		INT_PDA_Prompt p;
		if (m_New || m_SelId == "")
			return;
		// the server moves the admin's OWN PDA account: in the Alt+L mode without a login there is nothing to move
		if (!INT_PDA_Client.s_LoggedIn || !me)
		{
			Fail("#STR_INT_PDA_AF_JOIN_LOGIN");
			return;
		}
		if (me && me.FactionId == m_SelId)
		{
			INT_UIF_Toast.Info("#STR_INT_PDA_AF_ALREADY");
			return;
		}
		INT_PDA_Sfx.Play("Click");
		if (me && me.FactionId != "" && m_Menu)
		{
			m_JoinId = m_SelId;
			p = m_Menu.OpenPrompt("#STR_INT_PDA_AF_JOIN_MOVE", "#STR_INT_PDA_AF_F_FROM", "", false, me.FactionName);
			if (p)
				p.m_OnDone.Insert(OnJoinDone);
			return;
		}
		SendJoin(m_SelId);
	}

	protected void OnJoinDone(bool ok, string a, string b)
	{
		if (m_Menu)
			m_Menu.ClosePrompt();
		string id = m_JoinId;
		m_JoinId = "";
		if (!ok || id == "")
			return;
		SendJoin(id);
	}

	protected void OnNameChanged(string text, INT_UIF_TextInput t)
	{
		if (m_Filling)
			return;
		MarkMainDirty();
		RedrawCard();
		RefreshState();
	}

	protected void OnMainChanged(string text, INT_UIF_TextInput t)
	{
		if (m_Filling)
			return;
		MarkMainDirty();
		RefreshState();
	}

	protected void OnRankSubmit(string text, INT_UIF_TextInput t)
	{
		AddFromInput();
	}

	protected void OnRankAddClick(INT_UIF_Button b)
	{
		AddFromInput();
	}

	protected void AddFromInput()
	{
		if (!m_InRank)
			return;
		string raw = m_InRank.GetText();
		if (!AddRank(raw))
			return;
		m_InRank.SetText("");
		INT_PDA_Sfx.Play("Click");
	}

	protected void OnRankSaveClick(INT_UIF_Button b)
	{
		SaveRanks();
	}

	protected void OnRankResetClick(INT_UIF_Button b)
	{
		INT_PDA_Sfx.Play("Click");
		ResetRanks();
	}

	// Relation row of the faction the row showed when it was drawn (not the row index of the moment of the pick).
	protected int RelIdxOfRow(int row)
	{
		if (row < 0 || row >= m_ERowId.Count())
			return -1;
		string id = m_ERowId[row];
		if (id == "")
			return -1;
		return m_RelIds.Find(id);
	}

	protected void OnRelPicked(int index, string text, INT_UIF_Dropdown d)
	{
		int row = m_ERel.Find(d);
		if (row < 0)
			return;
		int idx = RelIdxOfRow(row);
		if (idx < 0 || idx >= m_RelVals.Count())
		{
			RefreshRelations();
			return;
		}
		int rel = IndexToRel(index);
		int cur = m_RelVals[idx];
		if (rel == cur)
			return;
		m_RelVals.Set(idx, rel);
		MarkRelDirty(idx);
		INT_PDA_Sfx.Play("Toggle");
		RefreshRelRowState(row);
		RefreshState();
	}

	protected void OnReasonChanged(string text, INT_UIF_TextInput t)
	{
		if (m_Filling)
			return;
		int row = m_EReason.Find(t);
		if (row < 0)
			return;
		int idx = RelIdxOfRow(row);
		if (idx < 0 || idx >= m_RelReasons.Count())
			return;
		m_RelReasons.Set(idx, text);
		MarkRelDirty(idx);
		RefreshRelRowState(row);
		RefreshState();
	}

	protected void OnReasonSubmit(string text, INT_UIF_TextInput t)
	{
		int row = m_EReason.Find(t);
		if (row < 0)
			return;
		SaveRelation(RelIdxOfRow(row));
	}

	protected void OnRelSaveClick(INT_UIF_Button b)
	{
		int row = m_ESave.Find(b);
		if (row < 0)
			return;
		INT_PDA_Sfx.Play("Click");
		SaveRelation(RelIdxOfRow(row));
	}
}
