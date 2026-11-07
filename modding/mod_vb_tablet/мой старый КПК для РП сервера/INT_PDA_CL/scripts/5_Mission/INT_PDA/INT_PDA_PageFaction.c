// INT_PDA (client) — «ГРУППИРОВКА» (owner brief «Группировки», spec.md §3 / §5, concept E faction frame
// design/concepts_v1/5_e_zone2_faction.png). Factions are real player groups: leader, deputies, members; ranks are per
// faction; relations (союз / нейтралитет / вражда) carry an RP reason. Leader and deputies are appointed by the admin
// only (not here).
//   - no GP module          -> «Locked» plate;
//   - not in a faction      -> plate + the pending invitation card (accept / decline);
//   - in a faction          -> header band: the vertical faction card, the huge faction name, the motto; on the right
//                              the base line (+ X / Y of the faction spawn), «N / M в сети», chips «ГП · модуль
//                              включён» and «Вы — <роль>»; a hairline; then three columns:
//                              left: relations (emblem, name, reason in 2 lines, relation pill, coloured bar);
//                              centre: members table (300 max, pooled rows scrolled by whole rows / pages; a click
//                                      selects a member for the tools on the right);
//                              right: invite panel (Steam ID + «Рядом») for leader / deputy, or the own rank card for
//                                     a member; under it the actions: rank of the selected member + «Выдать ранг»
//                                     (leader / admin), «Исключить» (leader / deputy), SOS, «Чат группировки» and the
//                                     «Покинуть группировку» link.
// Every tool mirrors the server rule of INT_PDA_SE_Faction (CanManage, OnKick, OnSetRank, OnLeave, OnSos):
//   manage (invite / kick)  = admin or role >= deputy; a deputy cannot kick the leader or another deputy;
//   ranks                   = admin or leader; members carry a Steam ID only for managers (no ID -> no tools);
//   leave                   = anyone in a faction (the leader too — the server allows it, the prompt warns).
// The «Рядом» list is built HERE from the client's own network bubble (ClientData.m_PlayerBaseList, vanilla
// 3_game/client/clientdata.c:7) — the server never scans. Requests are gated on the client (the server drops a
// second request inside REQUEST_GAP_MS without an answer). Every width decision is measured (INT_PDA_Look.TextW / Fit).

// The SOS cooldown shared by «Группировка» and «Карта»: the server keeps ONE gap (SOS_GAP_MS) per player, so the two
// pages keep one clock. A SOS the server refused (no module, not in a faction, the request gate «too fast») gives the
// time back; an accepted one (R_OK) or one that reached nobody (R_NO_RECIPIENTS — the server spent its gap) keeps it.
// The answer reaches the clock through INT_PDA_Client.s_OnResult even when neither page is the active tab; the pages
// also pass it on themselves before they repaint (the call is idempotent).
class INT_PDA_SosClock
{
	protected static int s_SentMs = -100000000;
	protected static bool s_Pending;
	protected static bool s_Hooked;

	static int LeftMs()
	{
		if (!GetGame())
			return 0;
		int since = GetGame().GetTime() - s_SentMs;
		if (since < 0)
		{
			// a new mission restarted the clock
			s_SentMs = -100000000;
			s_Pending = false;
			return 0;
		}
		int ms = INT_PDA_Const.SOS_GAP_MS - since;
		if (ms <= 0)
			return 0;
		return ms;
	}

	static int LeftSec()
	{
		int ms = LeftMs();
		if (ms <= 0)
			return 0;
		return (ms + 999) / 1000;
	}

	// A SOS leaves now.
	static void Start()
	{
		if (!GetGame())
			return;
		s_SentMs = GetGame().GetTime();
		s_Pending = true;
		if (!s_Hooked && INT_PDA_Client.s_OnResult)
		{
			s_Hooked = true;
			INT_PDA_Client.s_OnResult.Insert(INT_PDA_SosClock.OnClientResult);
		}
	}

	static void OnClientResult(int requestOp, int code)
	{
		if (requestOp == INT_PDA_Const.OP_SOS)
			OnServerAnswer(code);
	}

	static void OnServerAnswer(int code)
	{
		if (!s_Pending)
			return;
		s_Pending = false;
		if (code == INT_PDA_Const.R_OK || code == INT_PDA_Const.R_NO_RECIPIENTS)
			return;
		s_SentMs = -100000000;
	}
}

class INT_PDA_PageFaction : INT_PDA_Page
{
	static const int MODE_LOCKED = 0;
	static const int MODE_NONE = 1;
	static const int MODE_LOADING = 2;
	static const int MODE_MAIN = 3;

	static const int ACT_KICK = 1;
	static const int ACT_LEAVE = 2;
	static const int ACT_SOS = 3;

	// action buttons of the right column (index into m_Btn*)
	static const int BTN_RANK = 0;
	static const int BTN_KICK = 1;
	static const int BTN_SOS = 2;
	static const int BTN_CHAT = 3;
	static const int BTN_LEAVE = 4;

	// layouts of the right column
	static const int LAY_LEADER = 0;		// invite + ranks + kick (leader, admin)
	static const int LAY_DEPUTY = 1;		// invite + kick
	static const int LAY_MEMBER = 2;		// own rank card, SOS, chat

	// geometry (design px of the 1456 x 674 page, concept E)
	static const float CARD_W = 111;
	static const float NAME_X = 156;
	static const float NAME_W = 736;
	static const float INFO_X = 916;
	static const float INFO_W = 516;
	static const float HEAD_LINE_Y = 168;
	static const float COL_Y = 184;
	static const float COL_H = 474;
	static const float REL_X = 24;
	static const float REL_W = 400;
	static const float MEM_X = 444;
	static const float MEM_W = 540;
	static const float RIGHT_X = 1004;
	static const float RIGHT_W = 428;
	static const float IN_W = 392;
	static const float INV_H_SHORT = 240;
	static const float INV_H_TALL = 288;
	static const float ACT_H_FULL = 222;
	static const float ACT_H_SHORT = 174;
	static const float BTN_H = 40;

	static const int MEM_ROWS = 8;
	static const float MEM_ROW_H = 50;
	static const float MEM_TOP = 61;
	static const int REL_ROWS = 5;
	static const float REL_ROW_H = 82;
	static const float REL_STEP = 92;
	static const float REL_TOP = 24;
	static const int NEAR_ROWS = 3;
	static const float NEAR_TOP = 140;
	static const float NEAR_ROW_H = 44;
	static const int NEAR_MAX = 40;
	static const int LOAD_TRIES = 5;
	static const int FACTION_GAP_MS = 2500;

	static const int ROW_ME = 0xFF22160C;
	static const int ROW_LINE = 0xFF1E1E22;
	static const int NEAR_LINE = 0xFF222226;
	static const int BTN_BORDER = 0xFF3C3C42;
	static const int FIELD_LINE = 0xFF3A3A40;
	static const int KICK_DIM = 0xFF7A4A44;
	static const int SOS_RED = 0xFFEC6A5C;
	static const int SOS_BORDER = 0xFF5A2B27;
	static const int SOS_FILL_OFF = 0xFF2A1714;
	static const int SOS_HOVER = 0xFFF2645A;
	static const int CHIP_GP_FILL = 0xFF2A1D10;
	static const int CHIP_GP_LINE = 0x8CFF7A1A;
	static const int CHIP_ME_FILL = 0xFF1D241F;
	static const int CHIP_ME_LINE = 0x8C6FD0A0;

	// client gates (static: they outlive one opening of the PDA)
	protected static int s_LastSendMs = -100000000;
	protected static int s_FacReqMs = -100000000;

	protected int m_Mode = -1;
	protected bool m_Dirty;
	protected float m_Tick;
	protected int m_NearTick;
	protected int m_LoadTries;
	protected string m_InviteSeen;

	protected Widget m_LockedW;
	protected Widget m_NoneW;
	protected Widget m_LoadW;
	protected Widget m_MainW;

	// not in a faction: plate + invitation
	protected Widget m_NonePlate;
	protected float m_PlateY;
	protected Widget m_InvBox;
	protected Widget m_InvCardHost;
	protected TextWidget m_InvName;
	protected TextWidget m_InvFrom;
	protected INT_UIF_Button m_AcceptBtn;
	protected INT_UIF_Button m_DeclineBtn;
	protected string m_InvSig;

	// header band
	protected Widget m_Head;
	protected Widget m_CardHost;
	protected string m_CardSig;
	protected TextWidget m_NameW;
	protected TextWidget m_Motto;
	protected TextWidget m_BaseText;
	protected TextWidget m_OnlineText;
	protected Widget m_ChipGp;
	protected ref INT_UIF_Surface m_ChipGpBox;
	protected float m_ChipGpW;
	protected Widget m_ChipMe;
	protected ref INT_UIF_Surface m_ChipMeBox;
	protected TextWidget m_ChipMeText;

	// left: relations
	protected Widget m_RelCol;
	protected TextWidget m_RelPage;
	protected Widget m_RelEmpty;
	protected ButtonWidget m_RelUp;
	protected ButtonWidget m_RelDown;
	protected ImageWidget m_RelUpIcon;
	protected ImageWidget m_RelDownIcon;
	protected Widget m_RelUpBox;
	protected Widget m_RelDownBox;
	protected ref array<Widget> m_RelRowW = new array<Widget>();
	protected ref array<ImageWidget> m_RelAccent = new array<ImageWidget>();
	protected ref array<ImageWidget> m_RelEmb = new array<ImageWidget>();
	protected ref array<TextWidget> m_RelName = new array<TextWidget>();
	protected ref array<ref INT_UIF_Surface> m_RelPill = new array<ref INT_UIF_Surface>();
	protected ref array<TextWidget> m_RelWord = new array<TextWidget>();
	protected ref array<TextWidget> m_RelReason1 = new array<TextWidget>();
	protected ref array<TextWidget> m_RelReason2 = new array<TextWidget>();
	protected ref array<ButtonWidget> m_RelHit = new array<ButtonWidget>();
	protected int m_RelFirst;
	protected ref array<ref INT_PDA_Relation> m_Rels = new array<ref INT_PDA_Relation>();

	// centre: members
	protected Widget m_Center;
	protected TextWidget m_MemCaption;
	protected ButtonWidget m_TableHit;
	protected ButtonWidget m_MemUp;
	protected ButtonWidget m_MemDown;
	protected ImageWidget m_MemUpIcon;
	protected ImageWidget m_MemDownIcon;
	protected Widget m_MemUpBox;
	protected Widget m_MemDownBox;
	protected float m_YouW;
	protected ref array<Widget> m_RowW = new array<Widget>();
	protected ref array<ref INT_UIF_Surface> m_RowBox = new array<ref INT_UIF_Surface>();
	protected ref array<ImageWidget> m_RowMark = new array<ImageWidget>();
	protected ref array<TextWidget> m_RowName = new array<TextWidget>();
	protected ref array<Widget> m_RowMe = new array<Widget>();
	protected ref array<TextWidget> m_RowRank = new array<TextWidget>();
	protected ref array<TextWidget> m_RowRole = new array<TextWidget>();
	protected ref array<ImageWidget> m_RowDot = new array<ImageWidget>();
	protected ref array<ButtonWidget> m_RowHit = new array<ButtonWidget>();
	protected ref array<bool> m_RowIsMe = new array<bool>();
	protected int m_MemFirst;
	protected ref array<ref INT_PDA_Member> m_Sorted = new array<ref INT_PDA_Member>();
	protected ref array<string> m_SortKeys = new array<string>();
	protected ref array<string> m_RankOpts = new array<string>();
	protected string m_RankSig = "#";
	protected string m_SelSteam;

	// right: invite panel (leader / deputy)
	protected Widget m_InvFrame;
	protected ref INT_UIF_Surface m_InvPanelBox;
	protected INT_UIF_TextInput m_SteamInput;
	protected ImageWidget m_SteamLine;
	protected INT_UIF_Button m_InviteBtn;
	protected TextWidget m_SteamHint;
	protected TextWidget m_NearCaption;
	protected TextWidget m_NearEmpty;
	protected ButtonWidget m_NearRefresh;
	protected ImageWidget m_NearRefreshIcon;
	protected float m_NearBtnW;
	protected ref array<Widget> m_NearRowW = new array<Widget>();
	protected ref array<TextWidget> m_NearName = new array<TextWidget>();
	protected ref array<TextWidget> m_NearDist = new array<TextWidget>();
	protected ref array<ButtonWidget> m_NearHit = new array<ButtonWidget>();
	protected ref array<INT_UIF_Button> m_NearBtn = new array<INT_UIF_Button>();
	protected int m_NearFirst;
	protected int m_NearVisible = 2;
	protected ref array<string> m_NearSteams = new array<string>();
	protected ref array<string> m_NearNames = new array<string>();
	protected ref array<float> m_NearDists = new array<float>();

	// right: own card (members without tools)
	protected Widget m_MeFrame;
	protected TextWidget m_MyRank;
	protected TextWidget m_MyRole;
	protected TextWidget m_MyJoined;

	// right: actions
	protected Widget m_ActFrame;
	protected ref INT_UIF_Surface m_ActBox;
	protected float m_ActY = 436;
	protected int m_Layout = -1;
	protected TextWidget m_SelLabel;
	protected TextWidget m_SelName;
	protected INT_UIF_Dropdown m_ActDrop;
	protected float m_GiveW;
	protected int m_HoverBtn = -1;
	protected ref array<Widget> m_BtnFrames = new array<Widget>();
	protected ref array<ref INT_UIF_Surface> m_BtnBoxes = new array<ref INT_UIF_Surface>();
	protected ref array<TextWidget> m_BtnTexts = new array<TextWidget>();
	protected ref array<ImageWidget> m_BtnIcons = new array<ImageWidget>();
	protected ref array<ButtonWidget> m_BtnHits = new array<ButtonWidget>();
	protected ref array<string> m_BtnLabels = new array<string>();
	protected ref array<float> m_BtnW = new array<float>();
	protected ref array<bool> m_BtnOn = new array<bool>();

	// confirm overlay (kick / leave / SOS)
	protected Widget m_Confirm;
	protected Widget m_ConfirmBox;
	protected ButtonWidget m_ConfirmBlock;
	protected ImageWidget m_ConfirmLine;
	protected TextWidget m_ConfirmTitle;
	protected TextWidget m_ConfirmValue;
	protected MultilineTextWidget m_ConfirmText;
	protected INT_UIF_Button m_ConfirmYes;
	protected INT_UIF_Button m_ConfirmNo;
	protected int m_ConfirmAct;
	protected string m_ConfirmArg;

	// --- build ----------------------------------------------------------------------------------------------------
	override void OnBuild()
	{
		m_RankOpts.Insert(Widget.TranslateString("#STR_INT_PDA_FAC_NO_RANK"));
		m_LockedW = Locked(m_Root, m_W, m_H, "flag", "#STR_INT_PDA_FAC_LOCK_TITLE", "#STR_INT_PDA_FAC_LOCK_TEXT");
		BuildNone();
		BuildLoad();
		m_MainW = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, m_H, "INT_PDA_FacMain");
		BuildHead();
		BuildRelations();
		BuildCenter();
		BuildInvitePanel();
		BuildMePanel();
		BuildActions();
		BuildConfirm();
		m_LockedW.Show(false);
		m_NoneW.Show(false);
		m_LoadW.Show(false);
		m_MainW.Show(false);
	}

	protected void BuildNone()
	{
		m_NoneW = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, m_H, "INT_PDA_FacNone");
		m_NonePlate = INT_UIF_Draw.Frame(m_NoneW, 0, m_H * 0.5 - 90, m_W, 160, "INT_PDA_FacNonePlate");
		INT_PDA_Look.DrawIcon(m_NonePlate, m_W * 0.5 - 32, 0, 64, "flag", INT_PDA_Look.DIM2);
		INT_UIF_Draw.Text(m_NonePlate, 0, 84, m_W, 30, INT_PDA_Look.Caps("#STR_INT_PDA_FAC_NONE_TITLE"), 22, INT_PDA_Look.TEXT, INT_UIF_Align.CENTER);
		INT_UIF_Draw.Text(m_NonePlate, 0, 124, m_W, 24, "#STR_INT_PDA_FAC_NONE_TEXT", 16, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
		// the invitation card (owner 2026-10-23: vertical faction card with the emblem and the live name)
		float bw = 680;
		float bh = 420;
		m_InvBox = INT_UIF_Draw.Frame(m_NoneW, (m_W - bw) * 0.5, 214, bw, bh, "INT_PDA_FacInvBox");
		INT_UIF_Surface.Shadow(m_InvBox, -24, -16, bw + 48, bh + 44, ARGB(160, 0, 0, 0));
		INT_UIF_Surface.Box(m_InvBox, 0, 0, bw, bh, 8, INT_PDA_Look.PANEL, INT_PDA_Look.LINE2);
		INT_UIF_Draw.Line(m_InvBox, 0, 0, bw, 3, INT_PDA_Look.ORANGE);
		INT_PDA_Look.DrawIcon(m_InvBox, 28, 20, 18, "invite", INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Text(m_InvBox, 54, 20, 600, 20, INT_PDA_Look.Caps("#STR_INT_PDA_FAC_INV_IN_TITLE"), 13, INT_PDA_Look.ORANGE);
		m_InvCardHost = INT_UIF_Draw.Frame(m_InvBox, 28, 56, 210, 280, "INT_PDA_FacInvCard");
		m_InvName = INT_UIF_Draw.Text(m_InvBox, 268, 58, 384, 52, "", 40, INT_PDA_Look.TEXT);
		INT_UIF_Draw.Text(m_InvBox, 268, 124, 384, 18, INT_PDA_Look.Caps("#STR_INT_PDA_FAC_INV_FROM"), 13, INT_PDA_Look.DIM);
		m_InvFrom = INT_UIF_Draw.Text(m_InvBox, 268, 144, 384, 36, "", 28, INT_PDA_Look.OWN);
		INT_PDA_Look.MText(m_InvBox, 268, 196, 384, 90, "#STR_INT_PDA_FAC_INV_TEXT", 15, INT_PDA_Look.DIM);
		m_DeclineBtn = INT_UIF_Button.Create(m_Bag, m_InvBox, 268, 344, 180, 50, "#STR_INT_PDA_FAC_DECLINE", INT_UIF_Variant.SECONDARY, "close");
		m_AcceptBtn = INT_UIF_Button.Create(m_Bag, m_InvBox, 462, 344, 190, 50, "#STR_INT_PDA_FAC_ACCEPT", INT_UIF_Variant.PRIMARY, "check");
		if (m_DeclineBtn)
			m_DeclineBtn.OnClick().Insert(OnDeclineClick);
		if (m_AcceptBtn)
			m_AcceptBtn.OnClick().Insert(OnAcceptClick);
		m_InvBox.Show(false);
	}

	protected void BuildLoad()
	{
		m_LoadW = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, m_H, "INT_PDA_FacLoad");
		INT_PDA_Look.DrawIcon(m_LoadW, m_W * 0.5 - 24, m_H * 0.5 - 64, 48, "refresh", INT_PDA_Look.DIM2);
		INT_UIF_Draw.Text(m_LoadW, 0, m_H * 0.5, m_W, 30, INT_PDA_Look.Caps("#STR_INT_PDA_FAC_LOADING"), 20, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
	}

	// Header band: vertical faction card, huge thin name, motto; base / online / chips on the right; hairline under it.
	protected void BuildHead()
	{
		m_Head = INT_UIF_Draw.Frame(m_MainW, 0, 0, m_W, HEAD_LINE_Y + 2, "INT_PDA_FacHead");
		float cardH = CARD_W * 4.0 / 3.0;
		m_CardHost = INT_UIF_Draw.Frame(m_Head, 24, 8, CARD_W, cardH, "INT_PDA_FacCard");
		m_NameW = INT_UIF_Draw.Text(m_Head, NAME_X, 2, NAME_W + 4, 110, "", 96, INT_PDA_Look.TEXT);
		m_Motto = INT_UIF_Draw.Text(m_Head, NAME_X + 4, 112, NAME_W, 32, "", 22, INT_PDA_Look.DIM);
		Widget info = INT_UIF_Draw.Frame(m_Head, INFO_X, 0, INFO_W, HEAD_LINE_Y, "INT_PDA_FacHeadInfo");
		INT_PDA_Look.DrawIcon(info, 0, 21, 22, "pin", INT_PDA_Look.ORANGE);
		m_BaseText = INT_UIF_Draw.Text(info, 34, 16, INFO_W - 34, 32, "", 19, INT_PDA_Look.TEXT);
		INT_PDA_Look.DrawIcon(info, 0, 66, 24, "users", INT_PDA_Look.ORANGE);
		m_OnlineText = INT_UIF_Draw.Text(info, 34, 54, INFO_W - 34, 48, "", 32, INT_PDA_Look.TEXT);
		string gpCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_CHIP_GP");
		m_ChipGpW = ChipW(gpCap);
		m_ChipGp = INT_UIF_Draw.Frame(info, 0, 112, m_ChipGpW, 26, "INT_PDA_FacChipGp");
		m_ChipGpBox = INT_UIF_Surface.Pill(m_ChipGp, 0, 0, m_ChipGpW, 26, CHIP_GP_FILL, CHIP_GP_LINE);
		TextWidget gpText = INT_UIF_Draw.Text(m_ChipGp, 0, 0, m_ChipGpW, 26, gpCap, 13, INT_PDA_Look.ORANGE, INT_UIF_Align.CENTER);
		if (gpText)
			gpText.SetBold(true);
		m_ChipMe = INT_UIF_Draw.Frame(info, m_ChipGpW + 10, 112, 160, 26, "INT_PDA_FacChipMe");
		m_ChipMeBox = INT_UIF_Surface.Pill(m_ChipMe, 0, 0, 160, 26, CHIP_ME_FILL, CHIP_ME_LINE);
		m_ChipMeText = INT_UIF_Draw.Text(m_ChipMe, 0, 0, 160, 26, "", 13, INT_PDA_Look.ALLY, INT_UIF_Align.CENTER);
		if (m_ChipMeText)
			m_ChipMeText.SetBold(true);
		INT_UIF_Draw.Line(m_Head, 24, HEAD_LINE_Y, m_W - 48, 1, INT_PDA_Look.LINE);
	}

	// Width of a pill chip for a caption of 13 px bold capitals (bold is ~8 % wider than the measured regular text).
	protected float ChipW(string cap)
	{
		float tw = INT_PDA_Look.TextW(cap, 13);
		return tw * 1.08 + 28;
	}

	// Left column: «ОТНОШЕНИЯ», whole-row pages of relation plates.
	protected void BuildRelations()
	{
		m_RelCol = INT_UIF_Draw.Frame(m_MainW, REL_X, COL_Y, REL_W, COL_H, "INT_PDA_FacRel");
		Label(m_RelCol, 0, 2, 200, "#STR_INT_PDA_FAC_RELATIONS");
		m_RelPage = INT_UIF_Draw.Text(m_RelCol, 196, 2, 142, 18, "", 13, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		m_RelUpBox = INT_UIF_Draw.Frame(m_RelCol, REL_W - 54, 0, 24, 22, "INT_PDA_FacRelUp");
		INT_UIF_Surface.Box(m_RelUpBox, 0, 0, 24, 22, 4, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE2);
		m_RelUpIcon = INT_UIF_Draw.Icon(m_RelUpBox, 6, 5, 12, "chevron_up", INT_PDA_Look.DIM);
		m_RelUp = Hit(m_RelCol, REL_W - 54, 0, 24, 22);
		m_RelDownBox = INT_UIF_Draw.Frame(m_RelCol, REL_W - 24, 0, 24, 22, "INT_PDA_FacRelDown");
		INT_UIF_Surface.Box(m_RelDownBox, 0, 0, 24, 22, 4, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE2);
		m_RelDownIcon = INT_UIF_Draw.Icon(m_RelDownBox, 6, 5, 12, "chevron_down", INT_PDA_Look.DIM);
		m_RelDown = Hit(m_RelCol, REL_W - 24, 0, 24, 22);
		m_RelEmpty = INT_UIF_Draw.Frame(m_RelCol, 0, REL_TOP, REL_W, REL_ROW_H, "INT_PDA_FacRelEmpty");
		INT_UIF_Surface.Box(m_RelEmpty, 0, 0, REL_W, REL_ROW_H, 6, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
		INT_UIF_Draw.Text(m_RelEmpty, 20, 0, REL_W - 40, REL_ROW_H, "#STR_INT_PDA_FAC_REL_NONE", 15, INT_PDA_Look.DIM2);
		for (int i = 0; i < REL_ROWS; i++)
			BuildRelationRow(i);
	}

	protected void BuildRelationRow(int i)
	{
		float y = REL_TOP + i * REL_STEP;
		Widget row = INT_UIF_Draw.Frame(m_RelCol, 0, y, REL_W, REL_ROW_H, "INT_PDA_FacRelRow");
		INT_UIF_Surface.Box(row, 0, 0, REL_W, REL_ROW_H, 6, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
		ImageWidget accent = INT_UIF_Draw.Line(row, 0, 3, 4, REL_ROW_H - 6, INT_PDA_Look.NEUTRAL);
		ImageWidget emb = INT_PDA_Look.DrawEmblem(row, 18, 15, 52, 1);
		TextWidget nameW = INT_UIF_Draw.Text(row, 84, 8, 200, 28, "", 20, INT_PDA_Look.TEXT);
		if (nameW)
			nameW.SetBold(true);
		TextWidget r1 = INT_UIF_Draw.Text(row, 84, 36, 200, 20, "", 14, INT_PDA_Look.DIM);
		TextWidget r2 = INT_UIF_Draw.Text(row, 84, 56, 200, 20, "", 14, INT_PDA_Look.DIM);
		INT_UIF_Surface pill = INT_UIF_Surface.Pill(row, 290, 29, 96, 24, 0, INT_PDA_Look.NEUTRAL);
		TextWidget word = INT_UIF_Draw.Text(row, 290, 29, 96, 24, "", 12, INT_PDA_Look.NEUTRAL, INT_UIF_Align.CENTER);
		if (word)
			word.SetBold(true);
		ButtonWidget hit = Hit(row, 0, 0, REL_W, REL_ROW_H);
		row.Show(false);
		m_RelRowW.Insert(row);
		m_RelAccent.Insert(accent);
		m_RelEmb.Insert(emb);
		m_RelName.Insert(nameW);
		m_RelPill.Insert(pill);
		m_RelWord.Insert(word);
		m_RelReason1.Insert(r1);
		m_RelReason2.Insert(r2);
		m_RelHit.Insert(hit);
	}

	// Centre column: «СОСТАВ · …» caption + pager, the members table.
	protected void BuildCenter()
	{
		m_Center = INT_UIF_Draw.Frame(m_MainW, MEM_X, COL_Y, MEM_W, COL_H, "INT_PDA_FacMembers");
		m_MemCaption = INT_UIF_Draw.Text(m_Center, 0, 2, 420, 18, "", 13, INT_PDA_Look.DIM);
		m_MemUpBox = INT_UIF_Draw.Frame(m_Center, MEM_W - 54, 0, 24, 22, "INT_PDA_FacMemUp");
		INT_UIF_Surface.Box(m_MemUpBox, 0, 0, 24, 22, 4, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE2);
		m_MemUpIcon = INT_UIF_Draw.Icon(m_MemUpBox, 6, 5, 12, "chevron_up", INT_PDA_Look.DIM);
		m_MemUp = Hit(m_Center, MEM_W - 54, 0, 24, 22);
		m_MemDownBox = INT_UIF_Draw.Frame(m_Center, MEM_W - 24, 0, 24, 22, "INT_PDA_FacMemDown");
		INT_UIF_Surface.Box(m_MemDownBox, 0, 0, 24, 22, 4, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE2);
		m_MemDownIcon = INT_UIF_Draw.Icon(m_MemDownBox, 6, 5, 12, "chevron_down", INT_PDA_Look.DIM);
		m_MemDown = Hit(m_Center, MEM_W - 24, 0, 24, 22);
		INT_UIF_Surface.Box(m_Center, 0, 24, MEM_W, COL_H - 24, 6, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
		HeadCell(m_Center, 16, 180, "#STR_INT_PDA_FAC_COL_CALLSIGN", INT_UIF_Align.LEFT);
		HeadCell(m_Center, 206, 150, "#STR_INT_PDA_FAC_COL_RANK", INT_UIF_Align.LEFT);
		HeadCell(m_Center, 366, 110, "#STR_INT_PDA_FAC_COL_ROLE", INT_UIF_Align.LEFT);
		HeadCell(m_Center, MEM_W - 76, 60, "#STR_INT_PDA_FAC_COL_NET", INT_UIF_Align.RIGHT);
		INT_UIF_Draw.Line(m_Center, 1, 60, MEM_W - 2, 1, INT_PDA_Look.LINE);
		string youCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_YOU");
		float youTw = INT_PDA_Look.TextW(youCap, 11);
		m_YouW = Math.Max(30, youTw + 14);
		// wheel catcher under the rows (rows and their tools are created later = on top)
		m_TableHit = Hit(m_Center, 1, MEM_TOP, MEM_W - 2, MEM_ROWS * MEM_ROW_H);
		for (int i = 0; i < MEM_ROWS; i++)
			BuildMemberRow(i);
	}

	protected void HeadCell(Widget parent, float x, float w, string key, int align)
	{
		string cap = INT_PDA_Look.Caps(key);
		INT_UIF_Draw.Text(parent, x, 32, w, 20, cap, 13, INT_PDA_Look.DIM, align);
	}

	protected void BuildMemberRow(int i)
	{
		float y = MEM_TOP + i * MEM_ROW_H;
		float rw = MEM_W - 2;
		float rh = MEM_ROW_H - 1;
		Widget row = INT_UIF_Draw.Frame(m_Center, 1, y, rw, MEM_ROW_H, "INT_PDA_FacRow");
		INT_UIF_Surface box = INT_UIF_Surface.Box(row, 0, 0, rw, rh, 4, INT_PDA_Look.PANEL, 0);
		INT_UIF_Draw.Line(row, 0, rh, rw, 1, ROW_LINE);
		ImageWidget mark = INT_UIF_Draw.Line(row, 0, 0, 3, rh, INT_PDA_Look.ORANGE);
		TextWidget nameW = INT_UIF_Draw.Text(row, 16, 0, 180, rh, "", 18, INT_PDA_Look.TEXT);
		if (nameW)
			nameW.SetBold(true);
		Widget youTag = INT_UIF_Draw.Frame(row, 120, 15, m_YouW, 19, "INT_PDA_FacYou");
		INT_UIF_Surface.Box(youTag, 0, 0, m_YouW, 19, 4, 0, INT_PDA_Look.ORANGE);
		string youCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_YOU");
		INT_UIF_Draw.Text(youTag, 0, 0, m_YouW, 19, youCap, 11, INT_PDA_Look.ORANGE, INT_UIF_Align.CENTER);
		TextWidget rankW = INT_UIF_Draw.Text(row, 206, 0, 150, rh, "", 17, INT_PDA_Look.DIM);
		TextWidget roleW = INT_UIF_Draw.Text(row, 366, 0, 120, rh, "", 17, INT_PDA_Look.DIM);
		ImageWidget dot = INT_UIF_Draw.Image(row, rw - 30, 20, 10, 10, "iuf_circle16", INT_PDA_Look.DIM2);
		ButtonWidget hit = Hit(row, 0, 0, rw, rh);
		mark.Show(false);
		youTag.Show(false);
		row.Show(false);
		m_RowW.Insert(row);
		m_RowBox.Insert(box);
		m_RowMark.Insert(mark);
		m_RowName.Insert(nameW);
		m_RowMe.Insert(youTag);
		m_RowRank.Insert(rankW);
		m_RowRole.Insert(roleW);
		m_RowDot.Insert(dot);
		m_RowHit.Insert(hit);
		m_RowIsMe.Insert(false);
	}

	// Right column, top: «ПРИГЛАСИТЬ В ГРУППИРОВКУ» (Steam ID with an underline + «Рядом (N м)»). Height by layout.
	protected void BuildInvitePanel()
	{
		m_InvFrame = INT_UIF_Draw.Frame(m_MainW, RIGHT_X, COL_Y, RIGHT_W, INV_H_SHORT, "INT_PDA_FacInvite");
		m_InvPanelBox = INT_UIF_Surface.Box(m_InvFrame, 0, 0, RIGHT_W, INV_H_SHORT, 6, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
		string title = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_INVITE_TITLE");
		INT_UIF_Draw.Text(m_InvFrame, 18, 14, IN_W, 18, title, 13, INT_PDA_Look.ORANGE);
		// button widths from the measured label (bold ~10 % wider) + the «+» icon and padding
		float lab14 = INT_PDA_Look.TextW("#STR_INT_PDA_FAC_INVITE_BTN", 14);
		float ibw = lab14 * 1.1 + 52;
		ibw = Math.Clamp(ibw, 120, 180);
		float lab13 = INT_PDA_Look.TextW("#STR_INT_PDA_FAC_INVITE_BTN", 13);
		float nbw = lab13 * 1.1 + 46;
		m_NearBtnW = Math.Clamp(nbw, 110, 170);
		float fw = IN_W - 10 - ibw;
		m_SteamInput = INT_UIF_TextInput.Create(m_Bag, m_InvFrame, 18, 40, fw, 46, "#STR_INT_PDA_FAC_STEAM_PH", 17);
		if (m_SteamInput)
		{
			m_SteamInput.OnChanged().Insert(OnSteamChanged);
			m_SteamInput.OnSubmit().Insert(OnSteamSubmit);
		}
		m_SteamLine = INT_UIF_Draw.Line(m_InvFrame, 21, 84, fw - 6, 2, FIELD_LINE);
		m_InviteBtn = INT_UIF_Button.Create(m_Bag, m_InvFrame, 18 + fw + 10, 40, ibw, 46, "#STR_INT_PDA_FAC_INVITE_BTN", INT_UIF_Variant.PRIMARY, "plus");
		if (m_InviteBtn)
			m_InviteBtn.OnClick().Insert(OnInviteClick);
		m_SteamHint = INT_UIF_Draw.Text(m_InvFrame, 18, 92, IN_W, 16, "", 12, INT_PDA_Look.DIM2);
		SetSteamHint("#STR_INT_PDA_FAC_STEAM_HINT", INT_PDA_Look.DIM2);
		m_NearCaption = INT_UIF_Draw.Text(m_InvFrame, 18, 114, IN_W - 40, 18, "", 13, INT_PDA_Look.DIM);
		m_NearRefreshIcon = INT_PDA_Look.DrawIcon(m_InvFrame, 18 + IN_W - 18, 114, 18, "refresh", INT_PDA_Look.DIM);
		m_NearRefresh = Hit(m_InvFrame, 18 + IN_W - 28, 106, 36, 32);
		m_NearEmpty = INT_UIF_Draw.Text(m_InvFrame, 18, NEAR_TOP + 10, IN_W, 24, "#STR_INT_PDA_FAC_NEAR_NONE", 15, INT_PDA_Look.DIM2, INT_UIF_Align.CENTER);
		for (int i = 0; i < NEAR_ROWS; i++)
			BuildNearRow(i);
		m_InvFrame.Show(false);
	}

	protected void BuildNearRow(int i)
	{
		float y = NEAR_TOP + i * NEAR_ROW_H;
		Widget row = INT_UIF_Draw.Frame(m_InvFrame, 18, y, IN_W, NEAR_ROW_H, "INT_PDA_FacNear");
		INT_UIF_Draw.Line(row, 0, 0, IN_W, 1, NEAR_LINE);
		Widget av = INT_UIF_Draw.Frame(row, 0, 7, 30, 30, "INT_PDA_FacNearAv");
		INT_UIF_Surface.Box(av, 0, 0, 30, 30, 4, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE2);
		INT_PDA_Look.DrawIcon(av, 6, 6, 18, "user", INT_PDA_Look.DIM);
		TextWidget nameW = INT_UIF_Draw.Text(row, 42, 0, 150, NEAR_ROW_H, "", 17, INT_PDA_Look.TEXT);
		if (nameW)
			nameW.SetBold(true);
		TextWidget distW = INT_UIF_Draw.Text(row, 200, 0, 60, NEAR_ROW_H, "", 14, INT_PDA_Look.DIM);
		ButtonWidget hit = Hit(row, 0, 0, IN_W, NEAR_ROW_H);
		INT_UIF_Button btn = INT_UIF_Button.Create(m_Bag, row, IN_W - m_NearBtnW, 7, m_NearBtnW, 30, "#STR_INT_PDA_FAC_INVITE_BTN", INT_UIF_Variant.PRIMARY, "plus");
		if (btn)
			btn.OnClick().Insert(OnNearInviteClick);
		row.Show(false);
		m_NearRowW.Insert(row);
		m_NearName.Insert(nameW);
		m_NearDist.Insert(distW);
		m_NearHit.Insert(hit);
		m_NearBtn.Insert(btn);
	}

	// Right column, top, for members without tools: own rank, role, joined date, the SOS hint and who manages.
	protected void BuildMePanel()
	{
		m_MeFrame = INT_UIF_Draw.Frame(m_MainW, RIGHT_X, COL_Y, RIGHT_W, INV_H_TALL, "INT_PDA_FacMe");
		INT_UIF_Surface.Box(m_MeFrame, 0, 0, RIGHT_W, INV_H_TALL, 6, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
		Label(m_MeFrame, 18, 16, IN_W, "#STR_INT_PDA_FAC_MY_RANK");
		m_MyRank = INT_UIF_Draw.Text(m_MeFrame, 18, 38, IN_W, 44, "", 32, INT_PDA_Look.TEXT);
		m_MyRole = INT_UIF_Draw.Text(m_MeFrame, 18, 84, IN_W, 20, "", 14, INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Line(m_MeFrame, 18, 116, IN_W, 1, INT_PDA_Look.LINE);
		Label(m_MeFrame, 18, 130, IN_W, "#STR_INT_PDA_FAC_COL_JOINED");
		m_MyJoined = INT_UIF_Draw.Text(m_MeFrame, 18, 150, IN_W, 34, "", 24, INT_PDA_Look.TEXT);
		INT_UIF_Draw.Line(m_MeFrame, 18, 196, IN_W, 1, INT_PDA_Look.LINE);
		INT_PDA_Look.DrawIcon(m_MeFrame, 18, 214, 18, "sos", SOS_RED);
		string hint = Widget.TranslateString("#STR_INT_PDA_FAC_SOS_HINT");
		string hintFit = INT_PDA_Look.Fit(hint, IN_W - 28, 14);
		INT_UIF_Draw.Text(m_MeFrame, 46, 210, IN_W - 28, 26, hintFit, 14, INT_PDA_Look.DIM);
		string note = Widget.TranslateString("#STR_INT_PDA_FAC_MEMBER_NOTE");
		string note1;
		string note2;
		SplitTwo(note, IN_W, 13, note1, note2);
		INT_UIF_Draw.Text(m_MeFrame, 18, 242, IN_W, 18, note1, 13, INT_PDA_Look.DIM2);
		INT_UIF_Draw.Text(m_MeFrame, 18, 260, IN_W, 18, note2, 13, INT_PDA_Look.DIM2);
		m_MeFrame.Show(false);
	}

	// Right column, bottom: selected member, rank + «Выдать ранг», «Исключить», SOS, «Чат группировки», «Покинуть».
	// Positions / widths are set by ApplyLayout.
	protected void BuildActions()
	{
		m_ActFrame = INT_UIF_Draw.Frame(m_MainW, RIGHT_X, m_ActY, RIGHT_W, ACT_H_FULL, "INT_PDA_FacActions");
		m_ActBox = INT_UIF_Surface.Box(m_ActFrame, 0, 0, RIGHT_W, ACT_H_FULL, 6, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
		m_SelLabel = INT_UIF_Draw.Text(m_ActFrame, 18, 14, IN_W, 18, "", 13, INT_PDA_Look.DIM);
		m_SelName = INT_UIF_Draw.Text(m_ActFrame, 80, 11, 300, 24, "", 16, INT_PDA_Look.ORANGE);
		if (m_SelName)
			m_SelName.SetBold(true);
		string giveCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_GIVE_RANK");
		float giveTw = INT_PDA_Look.TextW(giveCap, 14);
		float giveW = giveTw + 66;
		m_GiveW = Math.Clamp(giveW, 150, IN_W - 160);
		float dropW = IN_W - 10 - m_GiveW;
		m_ActDrop = INT_UIF_Dropdown.Create(m_Bag, m_ActFrame, 18, 40, dropW, m_RankOpts, -1, "", BTN_H);
		if (m_ActDrop)
			m_ActDrop.OnSelected().Insert(OnActRankPicked);
		string kickCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_BTN_KICK");
		string sosCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_BTN_SOS");
		string chatCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_BTN_CHAT");
		// order = BTN_RANK, BTN_KICK, BTN_SOS, BTN_CHAT, BTN_LEAVE
		BuildBtn(giveCap, "star");
		BuildBtn(kickCap, "trash");
		BuildBtn(sosCap, "sos");
		BuildBtn(chatCap, "chat");
		BuildLeaveLink();
	}

	protected void BuildBtn(string label, string iconName)
	{
		Widget b = INT_UIF_Draw.Frame(m_ActFrame, 18, 40, 100, BTN_H, "INT_PDA_FacBtn");
		INT_UIF_Surface box = INT_UIF_Surface.Box(b, 0, 0, 100, BTN_H, 4, INT_PDA_Look.PANEL, BTN_BORDER);
		ImageWidget ic = INT_PDA_Look.DrawIcon(b, 12, 11, 18, iconName, INT_PDA_Look.TEXT);
		TextWidget tw = INT_UIF_Draw.Text(b, 40, 0, 60, BTN_H, label, 14, INT_PDA_Look.TEXT);
		ButtonWidget hit = Hit(b, 0, 0, 100, BTN_H);
		m_BtnFrames.Insert(b);
		m_BtnBoxes.Insert(box);
		m_BtnIcons.Insert(ic);
		m_BtnTexts.Insert(tw);
		m_BtnHits.Insert(hit);
		m_BtnLabels.Insert(label);
		m_BtnW.Insert(100);
		m_BtnOn.Insert(true);
	}

	// «Покинуть группировку»: a small text link under the actions (index BTN_LEAVE, no plate).
	protected void BuildLeaveLink()
	{
		string leave = Widget.TranslateString("#STR_INT_PDA_FAC_BTN_LEAVE");
		string leaveFit = INT_PDA_Look.Fit(leave, IN_W - 30, 14);
		float lw = INT_PDA_Look.TextW(leaveFit, 14);
		Widget b = INT_UIF_Draw.Frame(m_ActFrame, 18, 186, lw + 30, 22, "INT_PDA_FacLeave");
		ImageWidget ic = INT_PDA_Look.DrawIcon(b, 0, 3, 16, "logout", INT_PDA_Look.DIM);
		TextWidget tw = INT_UIF_Draw.Text(b, 24, 0, lw + 6, 22, leaveFit, 14, INT_PDA_Look.DIM);
		ButtonWidget hit = Hit(b, 0, 0, lw + 30, 22);
		m_BtnFrames.Insert(b);
		m_BtnBoxes.Insert(null);
		m_BtnIcons.Insert(ic);
		m_BtnTexts.Insert(tw);
		m_BtnHits.Insert(hit);
		m_BtnLabels.Insert(leaveFit);
		m_BtnW.Insert(lw + 30);
		m_BtnOn.Insert(true);
	}

	protected void BuildConfirm()
	{
		float cw = 560;
		float ch = 256;
		m_Confirm = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, m_H, "INT_PDA_FacConfirm");
		INT_UIF_Draw.Image(m_Confirm, 0, 0, m_W, m_H, "iuf_white", ARGB(170, 0, 0, 0));
		m_ConfirmBlock = Hit(m_Confirm, 0, 0, m_W, m_H);
		m_ConfirmBox = INT_UIF_Draw.Frame(m_Confirm, (m_W - cw) * 0.5, (m_H - ch) * 0.5, cw, ch, "INT_PDA_FacConfirmBox");
		INT_UIF_Surface.Shadow(m_ConfirmBox, -24, -16, cw + 48, ch + 44, ARGB(160, 0, 0, 0));
		INT_UIF_Surface.Box(m_ConfirmBox, 0, 0, cw, ch, 8, INT_PDA_Look.PANEL, INT_PDA_Look.LINE2);
		m_ConfirmLine = INT_UIF_Draw.Line(m_ConfirmBox, 0, 0, cw, 3, INT_PDA_Look.ORANGE);
		m_ConfirmTitle = INT_UIF_Draw.Text(m_ConfirmBox, 28, 22, cw - 56, 28, "", 20, INT_PDA_Look.TEXT);
		m_ConfirmValue = INT_UIF_Draw.Text(m_ConfirmBox, 28, 58, cw - 56, 36, "", 28, INT_PDA_Look.ORANGE);
		m_ConfirmText = INT_PDA_Look.MText(m_ConfirmBox, 28, 102, cw - 56, 70, "", 15, INT_PDA_Look.DIM);
		m_ConfirmNo = INT_UIF_Button.Create(m_Bag, m_ConfirmBox, cw - 28 - 300, ch - 64, 140, 44, "#STR_INT_PDA_CANCEL", INT_UIF_Variant.SECONDARY);
		m_ConfirmYes = INT_UIF_Button.Create(m_Bag, m_ConfirmBox, cw - 28 - 150, ch - 64, 150, 44, "#STR_INT_PDA_OK", INT_UIF_Variant.PRIMARY, "check");
		if (m_ConfirmNo)
			m_ConfirmNo.OnClick().Insert(OnConfirmNo);
		if (m_ConfirmYes)
			m_ConfirmYes.OnClick().Insert(OnConfirmYes);
		m_Confirm.Show(false);
	}

	// --- right column layout ---------------------------------------------------------------------------------------
	protected int LayoutOf()
	{
		if (CanRank())
			return LAY_LEADER;
		if (CanManage())
			return LAY_DEPUTY;
		return LAY_MEMBER;
	}

	// leader: invite 240 + actions 222 (rank row); deputy: invite 288 + actions 174; member: own card 288 + actions 174.
	protected void ApplyLayout(int lay)
	{
		if (lay == m_Layout)
			return;
		m_Layout = lay;
		bool invite = lay != LAY_MEMBER;
		bool ranks = lay == LAY_LEADER;
		m_InvFrame.Show(invite);
		m_MeFrame.Show(!invite);
		float invH = INV_H_TALL;
		float actH = ACT_H_SHORT;
		m_NearVisible = NEAR_ROWS;
		if (ranks)
		{
			invH = INV_H_SHORT;
			actH = ACT_H_FULL;
			m_NearVisible = 2;
		}
		INT_UIF_Draw.Resize(m_InvFrame, RIGHT_W, invH);
		Widget invRoot = null;
		if (m_InvPanelBox)
			invRoot = m_InvPanelBox.GetRoot();
		INT_UIF_Draw.Resize(invRoot, RIGHT_W, invH);
		m_ActY = COL_Y + invH + 12;
		INT_UIF_Draw.Place(m_ActFrame, RIGHT_X, m_ActY, RIGHT_W, actH);
		Widget actRoot = null;
		if (m_ActBox)
			actRoot = m_ActBox.GetRoot();
		INT_UIF_Draw.Resize(actRoot, RIGHT_W, actH);
		if (m_ActDrop)
			m_ActDrop.Show(ranks);
		m_BtnFrames[BTN_RANK].Show(ranks);
		float y = 40;
		if (ranks)
		{
			PlaceBtn(BTN_RANK, 18 + IN_W - m_GiveW, 40, m_GiveW);
			y = 88;
		}
		bool kick = lay != LAY_MEMBER;
		m_BtnFrames[BTN_KICK].Show(kick);
		float half = (IN_W - 10) * 0.5;
		if (kick)
		{
			PlaceBtn(BTN_KICK, 18, y, half);
			PlaceBtn(BTN_SOS, 18 + half + 10, y, half);
		}
		else
		{
			PlaceBtn(BTN_SOS, 18, y, IN_W);
		}
		PlaceBtn(BTN_CHAT, 18, y + 48, IN_W);
		INT_UIF_Draw.Move(m_BtnFrames[BTN_LEAVE], 18, y + 98);
		int maxFirst = m_NearSteams.Count() - m_NearVisible;
		if (maxFirst < 0)
			maxFirst = 0;
		m_NearFirst = INT_PDA_Util.ClampI(m_NearFirst, 0, maxFirst);
	}

	protected void PlaceBtn(int id, float x, float y, float w)
	{
		INT_UIF_Draw.Place(m_BtnFrames[id], x, y, w, BTN_H);
		INT_UIF_Surface box = m_BtnBoxes[id];
		Widget boxRoot = null;
		if (box)
			boxRoot = box.GetRoot();
		INT_UIF_Draw.Resize(boxRoot, w, BTN_H);
		INT_UIF_Draw.Resize(m_BtnHits[id], w, BTN_H);
		m_BtnW[id] = w;
		CenterBtn(id);
	}

	// Icon + label centred on the plate (label measured, cut with «…» if the plate is too narrow).
	protected void CenterBtn(int id)
	{
		float w = m_BtnW[id];
		string label = m_BtnLabels[id];
		string shown = INT_PDA_Look.Fit(label, w - 52, 14);
		float tw = INT_PDA_Look.TextW(shown, 14);
		float sx = (w - tw - 28) * 0.5;
		if (sx < 12)
			sx = 12;
		INT_UIF_Draw.Move(m_BtnIcons[id], sx, 11);
		INT_UIF_Draw.Place(m_BtnTexts[id], sx + 28, 0, tw + 8, BTN_H);
		m_BtnTexts[id].SetText(shown);
	}

	// Colours of an action plate: on / off, hover. SOS is a filled red plate, «Исключить» a red outline.
	protected void PaintBtn(int id, bool lit, bool fade)
	{
		if (id < 0 || id >= m_BtnBoxes.Count())
			return;
		m_BtnOn[id] = lit;
		if (id == BTN_LEAVE)
		{
			PaintLeave();
			return;
		}
		int fill = INT_PDA_Look.PANEL;
		int border = BTN_BORDER;
		int tc = INT_PDA_Look.TEXT;
		if (id == BTN_KICK)
		{
			border = SOS_BORDER;
			tc = SOS_RED;
		}
		else if (id == BTN_SOS)
		{
			fill = INT_PDA_Look.ENEMY;
			border = INT_PDA_Look.ENEMY;
			tc = INT_PDA_Look.WHITE;
		}
		if (!lit)
		{
			if (id == BTN_SOS)
			{
				fill = SOS_FILL_OFF;
				border = SOS_BORDER;
				tc = KICK_DIM;
			}
			else
			{
				border = INT_PDA_Look.LINE;
				tc = INT_PDA_Look.DIM2;
			}
		}
		if (lit && id == m_HoverBtn)
		{
			if (id == BTN_SOS)
				fill = SOS_HOVER;
			else
				fill = INT_PDA_Look.PANEL2;
		}
		INT_UIF_Surface box = m_BtnBoxes[id];
		if (box)
		{
			if (fade)
				box.FadeFill(fill, 0.12);
			else
				box.SetFill(fill);
			box.SetBorder(border);
		}
		m_BtnTexts[id].SetColor(tc);
		m_BtnIcons[id].SetColor(tc);
	}

	protected void PaintLeave()
	{
		int col = INT_PDA_Look.DIM;
		if (m_HoverBtn == BTN_LEAVE)
			col = SOS_RED;
		m_BtnTexts[BTN_LEAVE].SetColor(col);
		m_BtnIcons[BTN_LEAVE].SetColor(col);
	}

	// Selection caption, the rank dropdown of the selected member and the state of every action plate.
	protected void RenderSelection()
	{
		INT_PDA_Member m = SelectedMember();
		if (!m)
			m_SelSteam = "";
		bool mgr = CanManage();
		string cap;
		string capFit;
		string callsign;
		float lw;
		int col = INT_PDA_Look.DIM;
		if (!mgr)
		{
			cap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_ACTIONS");
			m_SelName.SetText("");
		}
		else if (!m)
		{
			cap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_PICK");
			col = INT_PDA_Look.DIM2;
			m_SelName.SetText("");
		}
		else
		{
			cap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_SEL");
			lw = INT_PDA_Look.TextW(cap, 13);
			callsign = INT_PDA_Look.Fit(m.Callsign, (IN_W - lw - 12) * 0.93, 16);
			INT_UIF_Draw.Place(m_SelName, 18 + lw + 10, 11, IN_W - lw - 10, 24);
			m_SelName.SetText(callsign);
		}
		capFit = INT_PDA_Look.Fit(cap, IN_W, 13);
		m_SelLabel.SetText(capFit);
		m_SelLabel.SetColor(col);
		bool canRank = false;
		bool canKick = false;
		if (m)
		{
			if (CanRankMember(m))
				canRank = true;
			if (CanKickMember(m))
				canKick = true;
		}
		if (m_ActDrop)
		{
			m_ActDrop.SetEnabled(canRank);
			int sel = -1;
			string placeholder = Widget.TranslateString("#STR_INT_PDA_FAC_COL_RANK");
			INT_PDA_FactionView f = INT_PDA_Client.s_Faction;
			if (m && f)
			{
				placeholder = m.Rank;
				if (m.Rank == "")
					sel = 0;
				else
					sel = f.Ranks.Find(m.Rank) + 1;
				if (sel == 0 && m.Rank != "")
					sel = -1;
			}
			m_ActDrop.SetPlaceholder(placeholder);
			m_ActDrop.SetSelected(sel);
		}
		PaintBtn(BTN_KICK, canKick, false);
		PaintBtn(BTN_CHAT, true, false);
		PaintBtn(BTN_LEAVE, true, false);
		PaintRankBtn();
		UpdateSosButton();
	}

	// «Выдать ранг» is lit when the leader picked a rank that differs from the member's current one.
	protected void PaintRankBtn()
	{
		INT_PDA_Member m = SelectedMember();
		bool lit = false;
		if (m && m_ActDrop && CanRankMember(m))
		{
			int dropIdx = m_ActDrop.GetSelected();
			string rank = DropRank(dropIdx);
			if (dropIdx >= 0 && rank != m.Rank)
				lit = true;
		}
		PaintBtn(BTN_RANK, lit, false);
	}

	// Dropdown index -> rank text ("" = no rank, index 0).
	protected string DropRank(int index)
	{
		INT_PDA_FactionView f = INT_PDA_Client.s_Faction;
		if (!f || index <= 0 || index - 1 >= f.Ranks.Count())
			return "";
		return f.Ranks[index - 1];
	}

	protected INT_PDA_Member SelectedMember()
	{
		if (m_SelSteam == "")
			return null;
		return FindMember(m_SelSteam);
	}

	// Managers pick the member the right-hand tools work on (a second click on the same row drops the choice).
	protected bool SelectSteam(string steam)
	{
		if (m_Mode != MODE_MAIN || !CanManage())
			return false;
		INT_PDA_Member m = FindMember(steam);
		if (!m)
			return false;
		if (m_SelSteam == steam)
			m_SelSteam = "";
		else
			m_SelSteam = steam;
		RenderMembers();
		RenderSelection();
		return true;
	}

	// --- life cycle -----------------------------------------------------------------------------------------------
	override void OnShow()
	{
		m_LoadTries = 0;
		m_Tick = 0;
		m_NearTick = 0;
		m_Mode = -1;
		Refresh();
		if (InFaction())
			AskFaction(false);
		CheckInviteSound();
		if (m_Mode == MODE_MAIN)
			RevealRows();
	}

	override void OnHide()
	{
		CloseConfirm();
		m_HoverBtn = -1;
	}

	override bool HandleEscape()
	{
		if (m_Confirm && m_Confirm.IsVisible())
		{
			CloseConfirm();
			INT_PDA_Sfx.Play("Click");
			return true;
		}
		return false;
	}

	override void OnChanged(int what)
	{
		if (what == INT_PDA_Client.CH_FACTION || what == INT_PDA_Client.CH_PROFILE || what == INT_PDA_Client.CH_AUTH || what == INT_PDA_Client.CH_WELCOME)
		{
			if (m_Active)
				Refresh();
			else
				m_Dirty = true;
		}
		else if (what == INT_PDA_Client.CH_INVITE)
		{
			if (m_Active)
				Refresh();
			else
				m_Dirty = true;
			CheckInviteSound();
		}
		else if (what == INT_PDA_Client.CH_PDA)
		{
			// battery / module events come often: only a change of the mode (GP module in or out) rebuilds the page
			if (m_Active && PickMode() != m_Mode)
				Refresh();
		}
	}

	override void Update(float dt)
	{
		m_Tick = m_Tick + dt;
		if (m_Tick < 1.0)
			return;
		m_Tick = 0;
		if (m_Dirty || PickMode() != m_Mode)
		{
			Refresh();
			return;
		}
		if (m_Mode == MODE_LOADING && m_LoadTries < LOAD_TRIES)
		{
			if (AskFaction(false))
				m_LoadTries++;
		}
		if (m_Mode != MODE_MAIN)
			return;
		UpdateSosButton();
		if (!CanManage())
			return;
		m_NearTick++;
		if (m_NearTick >= 2)
		{
			// the «Рядом» list is client-only data: a cheap rescan every 2 s while the invite panel is shown
			m_NearTick = 0;
			ScanNearby();
			RenderNearby();
		}
	}

	// --- mode -----------------------------------------------------------------------------------------------------
	protected int PickMode()
	{
		if (!HasMod(INT_PDA.SLOT_GP))
			return MODE_LOCKED;
		if (!InFaction())
			return MODE_NONE;
		INT_PDA_FactionView f = INT_PDA_Client.s_Faction;
		if (!f || f.Id != INT_PDA_Client.s_Profile.FactionId)
			return MODE_LOADING;
		return MODE_MAIN;
	}

	protected void Refresh()
	{
		int mode = PickMode();
		bool changed = false;
		if (mode != m_Mode)
			changed = true;
		m_Mode = mode;
		m_Dirty = false;
		m_LockedW.Show(mode == MODE_LOCKED);
		m_NoneW.Show(mode == MODE_NONE);
		m_LoadW.Show(mode == MODE_LOADING);
		m_MainW.Show(mode == MODE_MAIN);
		if (mode != MODE_MAIN)
		{
			CloseConfirm();
			m_SelSteam = "";
		}
		bool invNew = false;
		if (mode == MODE_NONE)
			invNew = RefreshInvite();
		else if (mode == MODE_MAIN)
			RefreshFaction();
		else if (mode == MODE_LOADING)
			AskFaction(false);
		if (!m_Active)
			return;
		if (changed)
			RevealMode();
		else if (invNew)
			RevealAt(m_InvBox, (m_W - 680) * 0.5, 214, 0.02);
	}

	// Reveal reads the CURRENT position as the target: a second reveal while the first one still moves would leave the
	// widget off its place, so every reveal first puts the widget back on its design position.
	protected void RevealAt(Widget w, float x, float y, float delay)
	{
		if (!w)
			return;
		INT_UIF_Draw.Move(w, x, y);
		Reveal(w, delay);
	}

	protected void RevealMode()
	{
		if (m_Mode == MODE_MAIN)
		{
			RevealAt(m_Head, 0, 0, 0.04);
			RevealAt(m_RelCol, REL_X, COL_Y, 0.1);
			RevealAt(m_Center, MEM_X, COL_Y, 0.14);
			if (m_InvFrame.IsVisible())
				RevealAt(m_InvFrame, RIGHT_X, COL_Y, 0.18);
			if (m_MeFrame.IsVisible())
				RevealAt(m_MeFrame, RIGHT_X, COL_Y, 0.18);
			RevealAt(m_ActFrame, RIGHT_X, m_ActY, 0.24);
		}
		else if (m_Mode == MODE_NONE)
		{
			RevealAt(m_NonePlate, 0, m_PlateY, 0.04);
			if (m_InvBox.IsVisible())
				RevealAt(m_InvBox, (m_W - 680) * 0.5, 214, 0.12);
		}
		else if (m_Mode == MODE_LOCKED)
		{
			RevealAt(m_LockedW, 0, 0, 0.04);
		}
	}

	protected void RevealRows()
	{
		int shown = 0;
		float y;
		for (int i = 0; i < m_RowW.Count(); i++)
		{
			if (!m_RowW[i].IsVisible())
				continue;
			y = MEM_TOP + i * MEM_ROW_H;
			RevealAt(m_RowW[i], 1, y, 0.16 + shown * 0.025);
			shown++;
		}
	}

	// --- not in a faction: invitation -----------------------------------------------------------------------------
	// Returns true when the card has just appeared.
	protected bool RefreshInvite()
	{
		bool has = INT_PDA_Client.s_InviteFaction != "";
		bool wasShown = m_InvBox.IsVisible();
		m_InvBox.Show(has);
		m_PlateY = m_H * 0.5 - 90;
		if (has)
			m_PlateY = 24;
		INT_UIF_Draw.Move(m_NonePlate, 0, m_PlateY);
		if (!has)
			return false;
		string sig = INT_PDA_Client.s_InviteFaction + "|" + INT_PDA_Client.s_InviteName + "|" + INT_PDA_Client.s_InviteEmblem.ToString();
		if (sig != m_InvSig)
		{
			m_InvSig = sig;
			INT_UIF_Widgets.ClearChildren(m_InvCardHost);
			string cardName = CardName(INT_PDA_Client.s_InviteName, 210);
			INT_PDA_Look.DrawFactionCard(m_InvCardHost, 0, 0, 210, INT_PDA_Client.s_InviteEmblem, cardName);
		}
		string up = INT_PDA_Look.Caps(INT_PDA_Client.s_InviteName);
		string shortName = INT_PDA_Look.Fit(up, 384, 40);
		m_InvName.SetText(shortName);
		string fromFit = INT_PDA_Look.Fit(INT_PDA_Client.s_InviteFrom, 384, 28);
		m_InvFrom.SetText(fromFit);
		if (wasShown)
			return false;
		return true;
	}

	// The name for a faction card: capitals (Cyrillic too) cut to the card width at the card's own text size
	// (INT_PDA_Look.DrawFactionCard: size max(13, w * 0.11), text box w - 8).
	protected string CardName(string name, float w)
	{
		float fs = Math.Max(13, w * 0.11);
		string up = INT_PDA_Look.Caps(name);
		return INT_PDA_Look.Fit(up, w - 8, fs);
	}

	protected void CheckInviteSound()
	{
		string inv = INT_PDA_Client.s_InviteFaction;
		if (inv == "")
		{
			m_InviteSeen = "";
			return;
		}
		if (inv == m_InviteSeen || !m_Active || m_Mode != MODE_NONE)
			return;
		m_InviteSeen = inv;
		INT_PDA_Sfx.Play("Invite");
	}

	protected void OnAcceptClick(INT_UIF_Button b)
	{
		DoAnswer(true);
	}

	protected void OnDeclineClick(INT_UIF_Button b)
	{
		DoAnswer(false);
	}

	// --- in a faction ---------------------------------------------------------------------------------------------
	protected void RefreshFaction()
	{
		INT_PDA_FactionView f = INT_PDA_Client.s_Faction;
		INT_PDA_Profile p = INT_PDA_Client.s_Profile;
		if (!f || !p)
			return;
		// a field of a dropdown that is open would point at another member after a re-sort
		INT_UIF_Dropdown.CloseActive();
		RenderHead(f, p);
		int lay = LayoutOf();
		ApplyLayout(lay);
		UpdateRankOptions(f);
		SortMembers();
		RenderMembers();
		SortRelations();
		RenderRelations();
		RenderMe(f, p);
		RenderSelection();
		if (CanManage())
		{
			ScanNearby();
			RenderNearby();
		}
	}

	protected void RenderHead(INT_PDA_FactionView f, INT_PDA_Profile p)
	{
		string sig = f.Id + "|" + f.Name + "|" + f.Emblem.ToString();
		if (sig != m_CardSig)
		{
			m_CardSig = sig;
			INT_UIF_Widgets.ClearChildren(m_CardHost);
			string cardName = CardName(f.Name, CARD_W);
			INT_PDA_Look.DrawFactionCard(m_CardHost, 0, 0, CARD_W, f.Emblem, cardName);
		}
		// the huge name: 96 px, smaller (down to 60) for a long name, then cut with «…»
		string nameCap = INT_PDA_Look.Caps(f.Name);
		float nameSize = 96;
		float nameTw = INT_PDA_Look.TextW(nameCap, nameSize);
		if (nameTw > NAME_W && nameTw > 0)
			nameSize = Math.Max(60, nameSize * NAME_W / nameTw);
		string nameFit = INT_PDA_Look.Fit(nameCap, NAME_W, nameSize);
		int namePx = INT_UIF_Scale.TextPx(nameSize);
		m_NameW.SetTextExactSize(namePx);
		m_NameW.SetText(nameFit);
		string motto = "";
		if (f.Motto != "")
		{
			float quotesW = INT_PDA_Look.TextW("«»", 22);
			string mottoFit = INT_PDA_Look.Fit(f.Motto, NAME_W - quotesW, 22);
			motto = "«" + mottoFit + "»";
		}
		m_Motto.SetText(motto);
		// base line: «База: <base> · X … · Y …» (X / Y of the faction spawn when the server sent one)
		string coords = "";
		int px;
		int pz;
		if (SpawnXZ(f.SpawnPos, px, pz))
			coords = " · X " + px.ToString() + " · Y " + pz.ToString();
		string baseLine = Widget.TranslateString("#STR_INT_PDA_FAC_NO_BASE");
		int baseCol = INT_PDA_Look.DIM;
		if (f.Base != "")
		{
			string baseCap = Widget.TranslateString("#STR_INT_PDA_FAC_BASE");
			baseLine = baseCap + ": " + f.Base;
			baseCol = INT_PDA_Look.TEXT;
		}
		float coordsW = 0;
		if (coords != "")
			coordsW = INT_PDA_Look.TextW(coords, 19);
		string baseFit = INT_PDA_Look.Fit(baseLine, INFO_W - 34 - coordsW, 19);
		m_BaseText.SetText(baseFit + coords);
		m_BaseText.SetColor(baseCol);
		int online = 0;
		int total = 0;
		int i;
		INT_PDA_Member m;
		for (i = 0; i < f.Members.Count(); i++)
		{
			m = f.Members[i];
			if (!m)
				continue;
			total++;
			if (m.Online)
				online++;
		}
		string onWord = Widget.TranslateString("#STR_INT_PDA_FAC_ONLINE");
		m_OnlineText.SetText(online.ToString() + " / " + total.ToString() + " " + onWord);
		// chip «ВЫ — <РОЛЬ>»
		string youCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_YOU");
		string roleKey = INT_PDA_Look.RoleKey(p.Role);
		string roleCap = INT_PDA_Look.Caps(roleKey);
		string meCap = youCap + " — " + roleCap;
		float maxW = INFO_W - m_ChipGpW - 10;
		float mw = ChipW(meCap);
		if (mw > maxW)
		{
			mw = maxW;
			meCap = INT_PDA_Look.Fit(meCap, (mw - 28) / 1.08, 13);
		}
		INT_UIF_Draw.Resize(m_ChipMe, mw, 26);
		Widget chipRoot = null;
		if (m_ChipMeBox)
			chipRoot = m_ChipMeBox.GetRoot();
		INT_UIF_Draw.Resize(chipRoot, mw, 26);
		INT_UIF_Draw.Resize(m_ChipMeText, mw, 26);
		m_ChipMeText.SetText(meCap);
	}

	// Own rank card (members without tools).
	protected void RenderMe(INT_PDA_FactionView f, INT_PDA_Profile p)
	{
		string myRank = p.Rank;
		int rankColor = INT_PDA_Look.TEXT;
		if (myRank == "")
		{
			myRank = "—";
			rankColor = INT_PDA_Look.DIM2;
		}
		string rankFit = INT_PDA_Look.Fit(myRank, IN_W, 32);
		m_MyRank.SetText(rankFit);
		m_MyRank.SetColor(rankColor);
		string roleKey = INT_PDA_Look.RoleKey(p.Role);
		string roleCap = INT_PDA_Look.Caps(roleKey);
		m_MyRole.SetText(roleCap);
		string joined = "—";
		INT_PDA_Member m;
		for (int i = 0; i < f.Members.Count(); i++)
		{
			m = f.Members[i];
			if (m && IsMe(m) && m.Joined != "")
			{
				joined = DateShort(m.Joined);
				break;
			}
		}
		m_MyJoined.SetText(joined);
	}

	// "x, y, z" (or "x z") of the faction spawn -> map X / Y. False when there is none.
	protected bool SpawnXZ(string spawn, out int px, out int pz)
	{
		px = 0;
		pz = 0;
		string s = spawn + "";
		if (s == "")
			return false;
		s.Replace(",", " ");
		array<string> parts = new array<string>();
		s.Split(" ", parts);
		array<string> nums = new array<string>();
		string t;
		int i;
		for (i = 0; i < parts.Count(); i++)
		{
			t = parts[i];
			t.TrimInPlace();
			if (t != "")
				nums.Insert(t);
		}
		if (nums.Count() < 2)
			return false;
		int last = nums.Count() - 1;
		string sx = nums[0];
		string sz = nums[last];
		float fx = sx.ToFloat();
		float fz = sz.ToFloat();
		if (fx == 0 && fz == 0)
			return false;
		px = (int)Math.Round(fx);
		pz = (int)Math.Round(fz);
		return true;
	}

	protected void UpdateRankOptions(INT_PDA_FactionView f)
	{
		string sig = "";
		int i;
		for (i = 0; i < f.Ranks.Count(); i++)
			sig = sig + f.Ranks[i] + "|";
		if (sig == m_RankSig)
			return;
		m_RankSig = sig;
		m_RankOpts.Clear();
		m_RankOpts.Insert(Widget.TranslateString("#STR_INT_PDA_FAC_NO_RANK"));
		for (i = 0; i < f.Ranks.Count(); i++)
			m_RankOpts.Insert(f.Ranks[i]);
		if (m_ActDrop)
			m_ActDrop.SetOptions(m_RankOpts);
	}

	// Leader, deputies, then online, then by callsign. Keys "<group><online><callsign key>!<index>" sort as strings
	// ('!' sorts before every callsign character, so a shorter callsign comes first).
	protected void SortMembers()
	{
		m_Sorted.Clear();
		m_SortKeys.Clear();
		INT_PDA_FactionView f = INT_PDA_Client.s_Faction;
		if (!f)
			return;
		int n = f.Members.Count();
		int i;
		int group;
		int idx;
		int klen;
		string key;
		string ck;
		string tail;
		INT_PDA_Member m;
		for (i = 0; i < n; i++)
		{
			m = f.Members[i];
			if (!m)
				continue;
			group = 2;
			if (m.Role == INT_PDA_Const.ROLE_LEADER)
				group = 0;
			else if (m.Role == INT_PDA_Const.ROLE_DEPUTY)
				group = 1;
			key = group.ToString();
			if (m.Online)
				key = key + "0";
			else
				key = key + "1";
			ck = INT_PDA_Util.CallsignKey(m.Callsign);
			key = key + ck + "!" + i.ToStringLen(3);
			m_SortKeys.Insert(key);
		}
		m_SortKeys.Sort();
		for (i = 0; i < m_SortKeys.Count(); i++)
		{
			key = m_SortKeys[i];
			klen = key.Length();
			tail = key.Substring(klen - 3, 3);
			idx = tail.ToInt();
			if (idx < 0 || idx >= n)
				continue;
			m = f.Members[idx];
			m_Sorted.Insert(m);
		}
		int maxFirst = m_Sorted.Count() - MEM_ROWS;
		if (maxFirst < 0)
			maxFirst = 0;
		m_MemFirst = INT_PDA_Util.ClampI(m_MemFirst, 0, maxFirst);
	}

	protected void RenderMembers()
	{
		INT_UIF_Dropdown.CloseActive();
		INT_PDA_FactionView f = INT_PDA_Client.s_Faction;
		int n = m_Sorted.Count();
		int i;
		int idx;
		int col;
		int last;
		int firstNo;
		float avail;
		float tw;
		bool me;
		bool picked;
		bool paged = false;
		if (n > m_RowW.Count())
			paged = true;
		string txt;
		string fit;
		string roleKey;
		string roleTxt;
		INT_PDA_Member m;
		for (i = 0; i < m_RowW.Count(); i++)
		{
			idx = m_MemFirst + i;
			m = null;
			if (idx < n)
				m = m_Sorted[idx];
			if (!m)
			{
				m_RowW[i].Show(false);
				continue;
			}
			m_RowW[i].Show(true);
			me = IsMe(m);
			m_RowIsMe[i] = me;
			m_RowMark[i].Show(me);
			col = INT_PDA_Look.PANEL;
			if (me)
				col = ROW_ME;
			m_RowBox[i].SetFill(col);
			picked = false;
			if (m.SteamId != "" && m.SteamId == m_SelSteam)
				picked = true;
			col = 0;
			if (picked)
				col = INT_PDA_Look.ORANGE2;
			m_RowBox[i].SetBorder(col);
			// callsign (+ «ВЫ» tag right after it)
			col = INT_PDA_Look.TEXT;
			avail = 180;
			if (me)
			{
				col = INT_PDA_Look.ORANGE;
				avail = 180 - m_YouW - 8;
			}
			fit = INT_PDA_Look.Fit(m.Callsign, avail * 0.93, 18);
			m_RowName[i].SetText(fit);
			m_RowName[i].SetColor(col);
			if (me)
			{
				tw = INT_PDA_Look.TextW(fit, 18) * 1.07;
				INT_UIF_Draw.Move(m_RowMe[i], 16 + tw + 8, 15);
			}
			m_RowMe[i].Show(me);
			// rank: text for everybody (concept E); the leader gives ranks in the actions panel («Выдать ранг»)
			txt = m.Rank;
			col = INT_PDA_Look.DIM;
			if (txt == "")
			{
				txt = "—";
				col = INT_PDA_Look.DIM2;
			}
			fit = INT_PDA_Look.Fit(txt, 146, 17);
			m_RowRank[i].SetText(fit);
			m_RowRank[i].SetColor(col);
			roleKey = INT_PDA_Look.RoleKey(m.Role);
			roleTxt = Widget.TranslateString(roleKey);
			fit = INT_PDA_Look.Fit(roleTxt, 118, 17);
			m_RowRole[i].SetText(fit);
			col = INT_PDA_Look.DIM;
			if (m.Role == INT_PDA_Const.ROLE_LEADER)
				col = INT_PDA_Look.ORANGE;
			else if (m.Role == INT_PDA_Const.ROLE_DEPUTY)
				col = INT_PDA_Look.OWN;
			m_RowRole[i].SetColor(col);
			col = INT_PDA_Look.DIM2;
			if (m.Online)
				col = INT_PDA_Look.ALLY;
			m_RowDot[i].SetColor(col);
		}
		m_MemUpBox.Show(paged);
		m_MemDownBox.Show(paged);
		m_MemUp.Show(paged);
		m_MemDown.Show(paged);
		// «СОСТАВ · 6» / «СОСТАВ · 1–8 ИЗ 23»
		string memCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_MEMBERS");
		string capLine = memCap + " · " + n.ToString();
		if (paged)
		{
			last = Math.Min(n, m_MemFirst + m_RowW.Count());
			firstNo = m_MemFirst + 1;
			string ofCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_OF");
			capLine = memCap + " · " + firstNo.ToString() + "–" + last.ToString() + " " + ofCap + " " + n.ToString();
		}
		m_MemCaption.SetText(capLine);
	}

	protected void ScrollMembers(int delta)
	{
		int maxFirst = m_Sorted.Count() - m_RowW.Count();
		if (maxFirst < 0)
			maxFirst = 0;
		int first = INT_PDA_Util.ClampI(m_MemFirst + delta, 0, maxFirst);
		if (first == m_MemFirst)
			return;
		m_MemFirst = first;
		RenderMembers();
	}

	// Allies, then enemies, then neutrals; by name inside a group.
	protected void SortRelations()
	{
		m_Rels.Clear();
		m_SortKeys.Clear();
		INT_PDA_FactionView f = INT_PDA_Client.s_Faction;
		if (!f)
			return;
		int n = f.Relations.Count();
		int i;
		int group;
		int idx;
		int klen;
		string key;
		string tail;
		INT_PDA_Relation r;
		for (i = 0; i < n; i++)
		{
			r = f.Relations[i];
			if (!r)
				continue;
			group = 2;
			if (r.Rel == INT_PDA_Const.REL_ALLY)
				group = 0;
			else if (r.Rel == INT_PDA_Const.REL_ENEMY)
				group = 1;
			key = group.ToString() + r.Name + "!" + i.ToStringLen(3);
			m_SortKeys.Insert(key);
		}
		m_SortKeys.Sort();
		for (i = 0; i < m_SortKeys.Count(); i++)
		{
			key = m_SortKeys[i];
			klen = key.Length();
			tail = key.Substring(klen - 3, 3);
			idx = tail.ToInt();
			if (idx < 0 || idx >= n)
				continue;
			r = f.Relations[idx];
			m_Rels.Insert(r);
		}
		int maxFirst = m_Rels.Count() - REL_ROWS;
		if (maxFirst < 0)
			maxFirst = 0;
		m_RelFirst = INT_PDA_Util.ClampI(m_RelFirst, 0, maxFirst);
	}

	protected void RenderRelations()
	{
		int n = m_Rels.Count();
		m_RelEmpty.Show(n == 0);
		int i;
		int idx;
		int col;
		float pw;
		float px;
		float textW;
		string wordKey;
		string wordCap;
		string reason;
		string path;
		string relName;
		string line1;
		string line2;
		INT_PDA_Relation r;
		INT_UIF_Surface pill;
		Widget pillRoot;
		for (i = 0; i < m_RelRowW.Count(); i++)
		{
			idx = m_RelFirst + i;
			r = null;
			if (idx < n)
				r = m_Rels[idx];
			if (!r)
			{
				m_RelRowW[i].Show(false);
				continue;
			}
			m_RelRowW[i].Show(true);
			col = INT_PDA_Look.NEUTRAL;
			wordKey = "#STR_INT_PDA_FAC_REL_NEUTRAL";
			if (r.Rel == INT_PDA_Const.REL_ALLY)
			{
				col = INT_PDA_Look.ALLY;
				wordKey = "#STR_INT_PDA_FAC_REL_ALLY";
			}
			else if (r.Rel == INT_PDA_Const.REL_ENEMY)
			{
				col = INT_PDA_Look.ENEMY;
				wordKey = "#STR_INT_PDA_FAC_REL_ENEMY";
			}
			m_RelAccent[i].SetColor(col);
			path = INT_PDA_Look.EmblemPath(r.Emblem);
			m_RelEmb[i].LoadImageFile(0, path);
			// the relation pill on the right, as wide as its word
			wordCap = INT_PDA_Look.Caps(wordKey);
			pw = INT_PDA_Look.TextW(wordCap, 12) * 1.08 + 24;
			px = REL_W - 14 - pw;
			pill = m_RelPill[i];
			pillRoot = null;
			if (pill)
			{
				pill.SetBorder(col);
				pillRoot = pill.GetRoot();
			}
			INT_UIF_Draw.Place(pillRoot, px, 29, pw, 24);
			INT_UIF_Draw.Place(m_RelWord[i], px, 29, pw, 24);
			m_RelWord[i].SetText(wordCap);
			m_RelWord[i].SetColor(col);
			// name + reason in two lines, left of the pill
			textW = px - 84 - 12;
			relName = INT_PDA_Look.Fit(r.Name, textW * 0.93, 20);
			m_RelName[i].SetText(relName);
			reason = r.Reason;
			if (reason == "")
				reason = Widget.TranslateString("#STR_INT_PDA_FAC_REASON_NONE");
			SplitTwo(reason, textW, 14, line1, line2);
			m_RelReason1[i].SetText(line1);
			m_RelReason2[i].SetText(line2);
			if (line2 == "")
			{
				INT_UIF_Draw.Place(m_RelName[i], 84, 14, textW, 28);
				INT_UIF_Draw.Place(m_RelReason1[i], 84, 42, textW, 20);
			}
			else
			{
				INT_UIF_Draw.Place(m_RelName[i], 84, 6, textW, 28);
				INT_UIF_Draw.Place(m_RelReason1[i], 84, 34, textW, 20);
			}
			INT_UIF_Draw.Place(m_RelReason2[i], 84, 54, textW, 20);
		}
		bool paged = false;
		if (n > REL_ROWS)
			paged = true;
		m_RelUpBox.Show(paged);
		m_RelDownBox.Show(paged);
		m_RelUp.Show(paged);
		m_RelDown.Show(paged);
		string page = "";
		int last;
		int firstNo;
		if (paged)
		{
			last = Math.Min(n, m_RelFirst + REL_ROWS);
			firstNo = m_RelFirst + 1;
			page = firstNo.ToString() + "–" + last.ToString() + " / " + n.ToString();
		}
		m_RelPage.SetText(page);
	}

	protected void ScrollRelations(int delta)
	{
		int maxFirst = m_Rels.Count() - REL_ROWS;
		if (maxFirst < 0)
			maxFirst = 0;
		int first = INT_PDA_Util.ClampI(m_RelFirst + delta, 0, maxFirst);
		if (first == m_RelFirst)
			return;
		m_RelFirst = first;
		RenderRelations();
	}

	// --- «Рядом» (client-only: the players of my network bubble within NearbyRadius) ----------------------------
	protected void ScanNearby()
	{
		m_NearSteams.Clear();
		m_NearNames.Clear();
		m_NearDists.Clear();
		if (m_Mode != MODE_MAIN || !CanManage())
			return;
		PlayerBase mePlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (!mePlayer)
			return;
		array<Man> bubble = ClientData.m_PlayerBaseList;
		if (!bubble)
			return;
		vector myPos = mePlayer.GetPosition();
		vector otherPos;
		float radius = INT_PDA_Client.s_Settings.NearbyRadius;
		int count = bubble.Count();
		int i;
		int k;
		int at;
		float dist;
		Man other;
		PlayerIdentity ident;
		string steam;
		string nick;
		for (i = 0; i < count; i++)
		{
			other = bubble[i];
			if (!other || other == mePlayer || !other.IsAlive())
				continue;
			ident = other.GetIdentity();
			if (!ident)
				continue;
			otherPos = other.GetPosition();
			dist = vector.Distance(myPos, otherPos);
			if (dist > radius)
				continue;
			steam = ident.GetPlainId();
			if (!INT_PDA_Util.IsSteamId(steam) || IsMemberSteam(steam) || m_NearSteams.Find(steam) >= 0)
				continue;
			nick = ident.GetName();
			at = m_NearDists.Count();
			for (k = 0; k < m_NearDists.Count(); k++)
			{
				if (dist < m_NearDists[k])
				{
					at = k;
					break;
				}
			}
			m_NearDists.InsertAt(dist, at);
			m_NearSteams.InsertAt(steam, at);
			m_NearNames.InsertAt(nick, at);
			if (m_NearSteams.Count() >= NEAR_MAX)
				break;
		}
	}

	protected void RenderNearby()
	{
		int n = m_NearSteams.Count();
		int maxFirst = n - m_NearVisible;
		if (maxFirst < 0)
			maxFirst = 0;
		m_NearFirst = INT_PDA_Util.ClampI(m_NearFirst, 0, maxFirst);
		int radius = (int)Math.Round(INT_PDA_Client.s_Settings.NearbyRadius);
		string nearCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_NEARBY");
		string mCap = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_M");
		string cap = nearCap + " (" + radius.ToString() + " " + mCap + ")";
		if (n > 0)
			cap = cap + " · " + n.ToString();
		m_NearCaption.SetText(cap);
		m_NearEmpty.Show(n == 0);
		string meterTxt = Widget.TranslateString("#STR_INT_PDA_FAC_M");
		int i;
		int idx;
		int meters;
		float dw;
		float nameAvail;
		float nameTw;
		string nick;
		string distTxt;
		for (i = 0; i < m_NearRowW.Count(); i++)
		{
			idx = m_NearFirst + i;
			if (i >= m_NearVisible || idx >= n)
			{
				m_NearRowW[i].Show(false);
				continue;
			}
			m_NearRowW[i].Show(true);
			meters = (int)Math.Round(m_NearDists[idx]);
			distTxt = meters.ToString() + " " + meterTxt;
			dw = INT_PDA_Look.TextW(distTxt, 14);
			nameAvail = IN_W - 42 - m_NearBtnW - 16 - dw - 10;
			nick = INT_PDA_Look.Fit(m_NearNames[idx], nameAvail * 0.93, 17);
			nameTw = INT_PDA_Look.TextW(nick, 17) * 1.07;
			INT_UIF_Draw.Place(m_NearName[i], 42, 0, nameTw + 4, NEAR_ROW_H);
			INT_UIF_Draw.Place(m_NearDist[i], 42 + nameTw + 10, 0, dw + 8, NEAR_ROW_H);
			m_NearName[i].SetText(nick);
			m_NearDist[i].SetText(distTxt);
		}
	}

	protected void ScrollNearby(int delta)
	{
		int maxFirst = m_NearSteams.Count() - m_NearVisible;
		if (maxFirst < 0)
			maxFirst = 0;
		int first = INT_PDA_Util.ClampI(m_NearFirst + delta, 0, maxFirst);
		if (first == m_NearFirst)
			return;
		m_NearFirst = first;
		RenderNearby();
	}

	protected void OnNearInviteClick(INT_UIF_Button b)
	{
		int i = m_NearBtn.Find(b);
		if (i < 0)
			return;
		int idx = m_NearFirst + i;
		if (idx >= m_NearSteams.Count())
			return;
		string steam = m_NearSteams[idx];
		DoInvite(steam);
	}

	// --- invite by Steam ID ---------------------------------------------------------------------------------------
	protected void SetSteamHint(string key, int col)
	{
		string hint = Widget.TranslateString(key);
		string hintFit = INT_PDA_Look.Fit(hint, IN_W, 12);
		m_SteamHint.SetText(hintFit);
		m_SteamHint.SetColor(col);
	}

	protected void OnSteamChanged(string text, INT_UIF_TextInput t)
	{
		INT_PDA_Sfx.Play("Key");
		string s = text + "";
		s.TrimInPlace();
		string hint = "#STR_INT_PDA_FAC_STEAM_HINT";
		int col = INT_PDA_Look.DIM2;
		int lineCol = FIELD_LINE;
		if (INT_PDA_Util.IsSteamId(s))
		{
			col = INT_PDA_Look.ALLY;
			lineCol = INT_PDA_Look.ORANGE;
		}
		else if (s.Length() >= 17)
		{
			col = INT_PDA_Look.ENEMY;
			lineCol = INT_PDA_Look.ENEMY;
			hint = "#STR_INT_PDA_FAC_BAD_STEAM";
		}
		else if (s != "")
		{
			lineCol = INT_PDA_Look.ORANGE2;
		}
		SetSteamHint(hint, col);
		if (m_SteamLine)
			m_SteamLine.SetColor(lineCol);
	}

	protected void OnSteamSubmit(string text, INT_UIF_TextInput t)
	{
		DoInvite(text);
	}

	protected void OnInviteClick(INT_UIF_Button b)
	{
		string text = "";
		if (m_SteamInput)
			text = m_SteamInput.GetText();
		DoInvite(text);
	}

	protected void ResetSteamField()
	{
		if (m_SteamInput)
			m_SteamInput.SetText("");
		SetSteamHint("#STR_INT_PDA_FAC_STEAM_HINT", INT_PDA_Look.DIM2);
		if (m_SteamLine)
			m_SteamLine.SetColor(FIELD_LINE);
	}

	// --- ranks / kick ---------------------------------------------------------------------------------------------
	// Rank dropdown of the actions panel: only picks; «Выдать ранг» sends.
	protected void OnActRankPicked(int index, string text, INT_UIF_Dropdown d)
	{
		PaintRankBtn();
	}

	protected void OnGiveRank()
	{
		INT_PDA_Member m = SelectedMember();
		if (!m)
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Warning("#STR_INT_PDA_FAC_PICK");
			return;
		}
		if (!m_ActDrop)
			return;
		int index = m_ActDrop.GetSelected();
		string rank = DropRank(index);
		if (index < 0 || rank == m.Rank)
		{
			INT_PDA_Sfx.Play("Error");
			return;
		}
		string steam = m.SteamId;
		DoRank(steam, rank);
	}

	protected void OnKickClick()
	{
		INT_PDA_Member m = SelectedMember();
		if (!m)
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Warning("#STR_INT_PDA_FAC_PICK");
			return;
		}
		if (!CanKickMember(m))
		{
			INT_PDA_Sfx.Play("Error");
			Toast(INT_PDA_Const.R_NO_RIGHTS);
			return;
		}
		INT_PDA_Sfx.Play("Click");
		string steam = m.SteamId;
		string callsign = m.Callsign;
		OpenConfirm(ACT_KICK, steam, callsign);
	}

	// --- action plates --------------------------------------------------------------------------------------------
	protected void OnActButton(int i)
	{
		if (i == BTN_RANK)
		{
			OnGiveRank();
			return;
		}
		if (i == BTN_KICK)
		{
			OnKickClick();
			return;
		}
		if (i == BTN_CHAT)
		{
			OpenFactionChat();
			return;
		}
		if (i == BTN_SOS)
		{
			OnSosClick();
			return;
		}
		if (i == BTN_LEAVE)
		{
			INT_PDA_Sfx.Play("Click");
			string leaveName = "";
			INT_PDA_FactionView f = INT_PDA_Client.s_Faction;
			if (f)
				leaveName = f.Name;
			OpenConfirm(ACT_LEAVE, "", leaveName);
		}
	}

	protected void OnSosClick()
	{
		INT_PDA_Sfx.Play("Click");
		if (!HasMod(INT_PDA.SLOT_GPS))
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Error("#STR_INT_PDA_FAC_SOS_NEED_GPS");
			return;
		}
		if (SosLeftSec() > 0)
		{
			INT_UIF_Toast.Warning("#STR_INT_PDA_FAC_SOS_WAIT");
			return;
		}
		string bigText = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_BTN_SOS");
		PlayerBase pl = PlayerBase.Cast(GetGame().GetPlayer());
		vector pos;
		int px;
		int pz;
		if (pl)
		{
			pos = pl.GetPosition();
			px = (int)Math.Round(pos[0]);
			pz = (int)Math.Round(pos[2]);
			bigText = bigText + " · X " + px.ToString() + " · Y " + pz.ToString();
		}
		OpenConfirm(ACT_SOS, "", bigText);
	}

	// «Чат группировки»: the chat tab on the faction channel.
	protected void OpenFactionChat()
	{
		INT_PDA_PageChat chatPage = INT_PDA_PageChat.Cast(m_Menu.PageOf(INT_PDA_Menu.TAB_CHAT));
		if (!chatPage)
			return;
		chatPage.OpenChannel(INT_PDA_Const.CH_FACTION);
	}

	protected void UpdateSosButton()
	{
		if (m_BtnTexts.Count() <= BTN_SOS || m_BtnIcons.Count() <= BTN_SOS)
			return;
		int left = SosLeftSec();
		bool gps = HasMod(INT_PDA.SLOT_GPS);
		string label = INT_PDA_Look.Caps("#STR_INT_PDA_FAC_BTN_SOS");
		if (left > 0)
		{
			string secTxt = Widget.TranslateString("#STR_INT_PDA_FAC_SEC");
			label = label + " · " + left.ToString() + " " + secTxt;
		}
		if (label != m_BtnLabels[BTN_SOS])
		{
			m_BtnLabels[BTN_SOS] = label;
			CenterBtn(BTN_SOS);
		}
		bool lit = true;
		if (!gps || left > 0)
			lit = false;
		PaintBtn(BTN_SOS, lit, false);
	}

	// One clock with the map's SOS button (INT_PDA_SosClock).
	protected int SosLeftSec()
	{
		return INT_PDA_SosClock.LeftSec();
	}

	// --- confirm overlay ------------------------------------------------------------------------------------------
	protected void OpenConfirm(int act, string arg, string bigText)
	{
		m_ConfirmAct = act;
		m_ConfirmArg = arg;
		string title = "#STR_INT_PDA_FAC_SOS_Q";
		string body = "#STR_INT_PDA_FAC_SOS_TEXT";
		int lineColor = INT_PDA_Look.ENEMY;
		if (act == ACT_KICK)
		{
			title = "#STR_INT_PDA_FAC_KICK_Q";
			body = "#STR_INT_PDA_FAC_KICK_TEXT";
		}
		else if (act == ACT_LEAVE)
		{
			title = "#STR_INT_PDA_FAC_LEAVE_Q";
			body = "#STR_INT_PDA_FAC_LEAVE_TEXT";
			lineColor = INT_PDA_Look.ORANGE;
			if (MyRole() == INT_PDA_Const.ROLE_LEADER)
				body = "#STR_INT_PDA_FAC_LEAVE_LEADER";
		}
		string titleCap = INT_PDA_Look.Caps(title);
		m_ConfirmTitle.SetText(titleCap);
		string valueFit = INT_PDA_Look.Fit(bigText, 500, 28);
		m_ConfirmValue.SetText(valueFit);
		m_ConfirmText.SetText(body);
		m_ConfirmLine.SetColor(lineColor);
		if (m_ConfirmYes)
			m_ConfirmYes.SetVariant(INT_UIF_Variant.DANGER);
		m_Confirm.Show(true);
		INT_UIF_Draw.Move(m_ConfirmBox, (m_W - 560) * 0.5, (m_H - 256) * 0.5);
		INT_UIF_Animator.Reveal(m_ConfirmBox, 0, 12, 0.22);
	}

	protected void CloseConfirm()
	{
		m_ConfirmAct = 0;
		m_ConfirmArg = "";
		if (m_Confirm)
			m_Confirm.Show(false);
	}

	protected void OnConfirmNo(INT_UIF_Button b)
	{
		INT_PDA_Sfx.Play("Click");
		CloseConfirm();
	}

	protected void OnConfirmYes(INT_UIF_Button b)
	{
		int act = m_ConfirmAct;
		string arg = m_ConfirmArg;
		CloseConfirm();
		if (act == ACT_KICK)
			DoKick(arg);
		else if (act == ACT_LEAVE)
			DoLeave();
		else if (act == ACT_SOS)
			DoSos();
	}

	// --- requests (gated; every one mirrors the server rule first) ------------------------------------------------
	protected bool CanSendNow()
	{
		int now = GetGame().GetTime();
		int since = now - s_LastSendMs;
		if (since >= 0 && since < INT_PDA_Const.REQUEST_GAP_MS + 50)
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Warning("#STR_INT_PDA_R_TOO_FAST");
			return false;
		}
		s_LastSendMs = now;
		return true;
	}

	// OP_FACTION on show / while loading: one per FACTION_GAP_MS (force: one per request gap). Silent when gated.
	protected bool AskFaction(bool force)
	{
		if (!InFaction())
			return false;
		int now = GetGame().GetTime();
		int gap = FACTION_GAP_MS;
		if (force)
			gap = INT_PDA_Const.REQUEST_GAP_MS + 50;
		int since = now - s_FacReqMs;
		if (since >= 0 && since < gap)
			return false;
		int sinceSend = now - s_LastSendMs;
		if (sinceSend >= 0 && sinceSend < INT_PDA_Const.REQUEST_GAP_MS + 50)
			return false;
		s_FacReqMs = now;
		s_LastSendMs = now;
		INT_PDA_Client.RequestFaction();
		return true;
	}

	protected void DoInvite(string steamIn)
	{
		string steam = steamIn + "";
		steam.TrimInPlace();
		if (m_Mode != MODE_MAIN || !CanManage())
		{
			TestLog("приглашение: нет прав");
			INT_PDA_Sfx.Play("Error");
			Toast(INT_PDA_Const.R_NO_RIGHTS);
			return;
		}
		if (!INT_PDA_Util.IsSteamId(steam))
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Error("#STR_INT_PDA_FAC_BAD_STEAM");
			return;
		}
		if (IsMemberSteam(steam))
		{
			INT_PDA_Sfx.Play("Error");
			Toast(INT_PDA_Const.R_IN_FACTION);
			return;
		}
		if (!CanSendNow())
			return;
		INT_PDA_Client.SendInvite(steam);
		INT_PDA_Sfx.Play("Send");
	}

	protected void DoRank(string steam, string rank)
	{
		INT_PDA_FactionView f = INT_PDA_Client.s_Faction;
		INT_PDA_Member m = FindMember(steam);
		if (m_Mode != MODE_MAIN || !f || !m || !CanRankMember(m))
		{
			TestLog("звание: нет прав или нет бойца");
			INT_PDA_Sfx.Play("Error");
			Toast(INT_PDA_Const.R_NO_RIGHTS);
			RenderMembers();
			return;
		}
		if (rank != "" && f.Ranks.Find(rank) < 0)
		{
			INT_PDA_Sfx.Play("Error");
			Toast(INT_PDA_Const.R_BAD_DATA);
			RenderMembers();
			return;
		}
		if (rank == m.Rank)
			return;
		if (!CanSendNow())
		{
			RenderMembers();
			return;
		}
		INT_PDA_Client.SendSetRank(steam, rank);
		INT_PDA_Sfx.Play("Click");
	}

	protected void DoKick(string steam)
	{
		INT_PDA_Member m = FindMember(steam);
		if (m_Mode != MODE_MAIN || !m || !CanKickMember(m))
		{
			TestLog("исключение: нет прав или нет бойца");
			INT_PDA_Sfx.Play("Error");
			Toast(INT_PDA_Const.R_NO_RIGHTS);
			return;
		}
		if (!CanSendNow())
			return;
		INT_PDA_Client.SendKick(steam);
		INT_PDA_Sfx.Play("Click");
	}

	protected void DoLeave()
	{
		if (!InFaction())
		{
			INT_PDA_Sfx.Play("Error");
			Toast(INT_PDA_Const.R_NOT_IN_FACTION);
			return;
		}
		if (!CanSendNow())
			return;
		INT_PDA_Client.SendLeave();
		INT_PDA_Sfx.Play("Click");
	}

	protected void DoSos()
	{
		if (m_Mode != MODE_MAIN)
		{
			Toast(INT_PDA_Const.R_NOT_IN_FACTION);
			return;
		}
		if (!HasMod(INT_PDA.SLOT_GPS))
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Error("#STR_INT_PDA_FAC_SOS_NEED_GPS");
			return;
		}
		if (SosLeftSec() > 0)
		{
			INT_UIF_Toast.Warning("#STR_INT_PDA_FAC_SOS_WAIT");
			return;
		}
		if (!CanSendNow())
			return;
		INT_PDA_SosClock.Start();
		INT_PDA_Client.SendSos();
		INT_PDA_Sfx.Play("Send");
		UpdateSosButton();
	}

	protected void DoAnswer(bool accept)
	{
		if (INT_PDA_Client.s_InviteFaction == "")
		{
			TestLog("ответ на приглашение: приглашения нет");
			return;
		}
		if (!CanSendNow())
			return;
		INT_PDA_Sfx.Play("Click");
		INT_PDA_Client.SendInviteAnswer(accept);
		if (!accept)
			INT_UIF_Toast.Info("#STR_INT_PDA_FAC_DECLINED");
	}

	override void OnResult(int requestOp, int code)
	{
		if (requestOp == INT_PDA_Const.OP_INVITE)
		{
			ResultToast(code, "#STR_INT_PDA_FAC_INVITE_SENT");
			if (code == INT_PDA_Const.R_OK)
				ResetSteamField();
		}
		else if (requestOp == INT_PDA_Const.OP_INVITE_ANSWER)
		{
			ResultToast(code, "#STR_INT_PDA_FAC_JOINED");
			if (m_Active)
				Refresh();
		}
		else if (requestOp == INT_PDA_Const.OP_KICK)
		{
			ResultToast(code, "#STR_INT_PDA_FAC_KICKED");
		}
		else if (requestOp == INT_PDA_Const.OP_SET_RANK)
		{
			ResultToast(code, "#STR_INT_PDA_FAC_RANK_SET");
			if (code != INT_PDA_Const.R_OK && m_Mode == MODE_MAIN)
			{
				RenderMembers();
				RenderSelection();
			}
		}
		else if (requestOp == INT_PDA_Const.OP_LEAVE)
		{
			ResultToast(code, "#STR_INT_PDA_FAC_LEFT");
		}
		else if (requestOp == INT_PDA_Const.OP_SOS)
		{
			// a refused SOS gives the cooldown back, an accepted / unheard one keeps it (INT_PDA_SosClock)
			INT_PDA_SosClock.OnServerAnswer(code);
			if (code == INT_PDA_Const.R_NO_RECIPIENTS)
			{
				INT_PDA_Sfx.Play("Error");
				INT_UIF_Toast.Warning("#STR_INT_PDA_SOS_NOBODY");
			}
			else
			{
				ResultToast(code, "#STR_INT_PDA_FAC_SOS_SENT");
			}
			if (m_Mode == MODE_MAIN)
				UpdateSosButton();
		}
	}

	protected void ResultToast(int code, string okKey)
	{
		if (code == INT_PDA_Const.R_OK)
		{
			INT_UIF_Toast.Success(okKey);
			return;
		}
		INT_PDA_Sfx.Play("Error");
		Toast(code);
	}

	// --- rights (mirror of INT_PDA_SE_Faction) --------------------------------------------------------------------
	protected int MyRole()
	{
		INT_PDA_Profile p = INT_PDA_Client.s_Profile;
		if (!p || !p.InFaction())
			return INT_PDA_Const.ROLE_NONE;
		return p.Role;
	}

	// CanManage: admin (config Steam ID — the session flag, not only Alt+L) or deputy and above.
	protected bool CanManage()
	{
		if (!InFaction())
			return false;
		if (INT_PDA_Client.s_IsAdmin)
			return true;
		return MyRole() >= INT_PDA_Const.ROLE_DEPUTY;
	}

	// OnSetRank: admin or the leader.
	protected bool CanRank()
	{
		if (!InFaction())
			return false;
		if (INT_PDA_Client.s_IsAdmin)
			return true;
		return MyRole() == INT_PDA_Const.ROLE_LEADER;
	}

	protected bool CanRankMember(INT_PDA_Member m)
	{
		if (!m || m.SteamId == "")
			return false;
		return CanRank();
	}

	// OnKick: managers; never self; a deputy (non-admin) never kicks the leader or another deputy.
	protected bool CanKickMember(INT_PDA_Member m)
	{
		if (!m || m.SteamId == "" || IsMe(m))
			return false;
		if (!CanManage())
			return false;
		if (INT_PDA_Client.s_IsAdmin)
			return true;
		if (MyRole() < INT_PDA_Const.ROLE_LEADER && m.Role >= INT_PDA_Const.ROLE_DEPUTY)
			return false;
		return true;
	}

	protected bool IsMe(INT_PDA_Member m)
	{
		INT_PDA_Profile p = INT_PDA_Client.s_Profile;
		if (!p || !m)
			return false;
		if (m.SteamId != "" && m.SteamId == p.SteamId)
			return true;
		string a = INT_PDA_Util.CallsignKey(m.Callsign);
		string b = INT_PDA_Util.CallsignKey(p.Callsign);
		return a == b;
	}

	protected INT_PDA_Member FindMember(string steam)
	{
		INT_PDA_FactionView f = INT_PDA_Client.s_Faction;
		if (!f || steam == "")
			return null;
		INT_PDA_Member m;
		for (int i = 0; i < f.Members.Count(); i++)
		{
			m = f.Members[i];
			if (m && m.SteamId == steam)
				return m;
		}
		return null;
	}

	protected bool IsMemberSteam(string steam)
	{
		INT_PDA_Member m = FindMember(steam);
		return m != null;
	}

	// --- small helpers --------------------------------------------------------------------------------------------
	// "YYYY-MM-DD" -> "DD.MM.YY"
	protected string DateShort(string day)
	{
		if (day.Length() < 10)
			return day;
		return day.Substring(8, 2) + "." + day.Substring(5, 2) + "." + day.Substring(2, 2);
	}

	// Two lines of at most `width` each (measured): the words of line 1 as long as they fit, the rest on line 2 cut
	// with «…». Text without spaces (Chinese / Japanese) or one over-long word is split by characters.
	protected void SplitTwo(string s, float width, float size, out string line1, out string line2)
	{
		line1 = s;
		line2 = "";
		if (s == "")
			return;
		if (INT_PDA_Look.TextW(s, size) <= width)
			return;
		array<string> words = new array<string>();
		s.Split(" ", words);
		string acc = "";
		string cand;
		string rest = "";
		int used = 0;
		int i;
		int n;
		int restLen;
		for (i = 0; i < words.Count(); i++)
		{
			if (acc == "")
				cand = words[i];
			else
				cand = acc + " " + words[i];
			if (INT_PDA_Look.TextW(cand, size) > width)
				break;
			acc = cand;
			used = i + 1;
		}
		if (acc == "")
		{
			n = PrefixFit(s, width, size);
			acc = s.SubstringUtf8(0, n);
			restLen = s.LengthUtf8() - n;
			rest = s.SubstringUtf8(n, restLen);
		}
		else
		{
			for (i = used; i < words.Count(); i++)
			{
				if (rest == "")
					rest = words[i];
				else
					rest = rest + " " + words[i];
			}
		}
		rest.TrimInPlace();
		line1 = acc;
		line2 = INT_PDA_Look.Fit(rest, width, size);
	}

	// How many leading characters of s fit into width (measured).
	protected int PrefixFit(string s, float width, float size)
	{
		int n = s.LengthUtf8();
		float full = INT_PDA_Look.TextW(s, size);
		int keep = n;
		if (full > 0)
			keep = (int)(n * width / full);
		keep = INT_PDA_Util.ClampI(keep, 1, n);
		string part = s.SubstringUtf8(0, keep);
		while (keep > 1 && INT_PDA_Look.TextW(part, size) > width)
		{
			keep = keep - 1;
			part = s.SubstringUtf8(0, keep);
		}
		string longer;
		while (keep < n)
		{
			longer = s.SubstringUtf8(0, keep + 1);
			if (INT_PDA_Look.TextW(longer, size) > width)
				break;
			keep = keep + 1;
		}
		return keep;
	}

	protected void TestLog(string text)
	{
		INT_PDA_Log.Dbg("ГП-вкладка: " + text);
	}

	// --- input ----------------------------------------------------------------------------------------------------
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!w)
			return false;
		if (w == m_ConfirmBlock)
			return true;
		int i;
		for (i = 0; i < m_BtnHits.Count(); i++)
		{
			if (w == m_BtnHits[i])
			{
				OnActButton(i);
				return true;
			}
		}
		for (i = 0; i < m_RowHit.Count(); i++)
		{
			if (w == m_RowHit[i])
			{
				OnRowClick(i);
				return true;
			}
		}
		if (w == m_MemUp)
		{
			INT_PDA_Sfx.Play("Click");
			ScrollMembers(-MEM_ROWS);
			return true;
		}
		if (w == m_MemDown)
		{
			INT_PDA_Sfx.Play("Click");
			ScrollMembers(MEM_ROWS);
			return true;
		}
		if (w == m_RelUp)
		{
			INT_PDA_Sfx.Play("Click");
			ScrollRelations(-REL_ROWS);
			return true;
		}
		if (w == m_RelDown)
		{
			INT_PDA_Sfx.Play("Click");
			ScrollRelations(REL_ROWS);
			return true;
		}
		if (w == m_NearRefresh)
		{
			INT_PDA_Sfx.Play("Click");
			ScanNearby();
			RenderNearby();
			return true;
		}
		return false;
	}

	// A click on a member row selects it for the tools on the right (managers only, members with a Steam ID).
	protected void OnRowClick(int row)
	{
		if (!CanManage())
			return;
		int idx = m_MemFirst + row;
		if (idx >= m_Sorted.Count())
			return;
		INT_PDA_Member m = m_Sorted[idx];
		if (!m || m.SteamId == "")
			return;
		string steam = m.SteamId;
		if (SelectSteam(steam))
			INT_PDA_Sfx.Play("Click");
	}

	// One wheel notch = one whole row.
	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		ButtonWidget b = ButtonWidget.Cast(w);
		if (!b)
			return false;
		if (b == m_TableHit || m_RowHit.Find(b) >= 0)
		{
			ScrollMembers(-wheel);
			return true;
		}
		if (m_RelHit.Find(b) >= 0)
		{
			ScrollRelations(-wheel);
			return true;
		}
		if (m_NearHit.Find(b) >= 0)
		{
			ScrollNearby(-wheel);
			return true;
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		Hover(w, true);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		Hover(w, false);
		return false;
	}

	protected void Hover(Widget w, bool enter)
	{
		if (!w)
			return;
		int i;
		int col;
		for (i = 0; i < m_BtnHits.Count(); i++)
		{
			if (w != m_BtnHits[i])
				continue;
			if (enter)
				m_HoverBtn = i;
			else if (m_HoverBtn == i)
				m_HoverBtn = -1;
			PaintBtn(i, m_BtnOn[i], true);
			return;
		}
		for (i = 0; i < m_RowHit.Count(); i++)
		{
			if (w != m_RowHit[i])
				continue;
			col = INT_PDA_Look.PANEL;
			if (m_RowIsMe[i])
				col = ROW_ME;
			if (enter)
				col = INT_PDA_Look.PANEL2;
			m_RowBox[i].FadeFill(col, 0.12);
			return;
		}
		col = INT_PDA_Look.DIM;
		if (enter)
			col = INT_PDA_Look.TEXT;
		if (w == m_MemUp)
			m_MemUpIcon.SetColor(col);
		else if (w == m_MemDown)
			m_MemDownIcon.SetColor(col);
		else if (w == m_RelUp)
			m_RelUpIcon.SetColor(col);
		else if (w == m_RelDown)
			m_RelDownIcon.SetColor(col);
		else if (w == m_NearRefresh)
		{
			if (enter)
				col = INT_PDA_Look.ORANGE;
			m_NearRefreshIcon.SetColor(col);
		}
	}

	// --- self-test ------------------------------------------------------------------------------------------------
	// faction refresh | faction invite <steam> | faction rank <steam> <rank…> ("-" = no rank) | faction kick <steam>
	// faction leave | faction accept | faction decline | faction near | faction sos | faction select <steam> | faction state
	// Kick / leave / SOS go out directly (no confirm overlay). «select» picks the member for the right-hand tools (a
	// second select of the same member drops it). Every command ends with a «FACTION» state line.
	override void OnTestCommand(string cmd)
	{
		if (cmd.IndexOf("faction") != 0)
			return;
		array<string> parts = new array<string>();
		cmd.Split(" ", parts);
		string sub = "state";
		string steam = "";
		string rank = "";
		int i;
		if (parts.Count() >= 2)
			sub = parts[1];
		if (parts.Count() >= 3)
			steam = parts[2];
		for (i = 3; i < parts.Count(); i++)
		{
			if (rank != "")
				rank = rank + " ";
			rank = rank + parts[i];
		}
		if (rank == "-")
			rank = "";
		if (sub == "refresh")
		{
			bool asked = AskFaction(true);
			Refresh();
			INT_PDA_Log.Info("FACTION refresh: запрос " + asked.ToString());
		}
		else if (sub == "invite")
		{
			DoInvite(steam);
		}
		else if (sub == "rank")
		{
			DoRank(steam, rank);
		}
		else if (sub == "kick")
		{
			DoKick(steam);
		}
		else if (sub == "leave")
		{
			DoLeave();
		}
		else if (sub == "accept")
		{
			DoAnswer(true);
		}
		else if (sub == "decline")
		{
			DoAnswer(false);
		}
		else if (sub == "near")
		{
			ScanNearby();
			RenderNearby();
			LogNearby();
		}
		else if (sub == "sos")
		{
			DoSos();
		}
		else if (sub == "select")
		{
			if (!SelectSteam(steam))
				TestLog("выбор: нет прав или нет бойца");
		}
		LogState(sub);
	}

	protected void LogState(string sub)
	{
		string line = "FACTION " + sub + ": режим " + m_Mode.ToString();
		line = line + ", бойцов " + m_Sorted.Count().ToString() + ", отношений " + m_Rels.Count().ToString();
		bool mgr = CanManage();
		bool rk = CanRank();
		line = line + ", управление " + mgr.ToString() + ", звания " + rk.ToString();
		line = line + ", рядом " + m_NearSteams.Count().ToString() + ", SOS " + SosLeftSec().ToString() + " с";
		line = line + ", приглашение '" + INT_PDA_Client.s_InviteFaction + "'";
		line = line + ", выбран '" + m_SelSteam + "', раскладка " + m_Layout.ToString();
		INT_PDA_Log.Info(line);
	}

	protected void LogNearby()
	{
		int meters;
		string nick;
		for (int i = 0; i < m_NearSteams.Count(); i++)
		{
			meters = (int)Math.Round(m_NearDists[i]);
			nick = INT_PDA_Log.Clip(m_NearNames[i], 24);
			INT_PDA_Log.Info("FACTION near " + i.ToString() + ": " + nick + " " + m_NearSteams[i] + " " + meters.ToString() + " м");
		}
	}
}
