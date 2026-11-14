// ============================================================================
// VanillaPPMap (our VPP fork) - concept B «ОБЗОР»: the opaque islands at the screen edges (MAP agent, phase 2).
//   top-left     brand DAYZAVR | MAP + tabs (Карта / Метки / Маршрут, admin: 04 Зоны, 05 Надписи since 2026-11-13)
//   (no top-right island and no zoom stack: owner 2026-11-12 — the map closes with M / Esc, zoom = wheel,
//    centre on me = Space)
//   bottom-left  layers 2x3: Сервер, Мои, События, Зоны, Маршрут + «3D» master (gear = per-layer distances popover)
//                + (2026-11-13) a wide 7th row «Миникарта»: check = on / off, the «\» key hint, gear = the minimap
//                settings panel (INT_VPP_MiniPanel.c) in the popover's place (only one of the two is open)
//   bottom       keys strip (changes with the mode)
//   bottom-right coordinates: me / grid square / cursor + distance
//   right        the folded list «СПИСОК N» that expands into the marker list (both start at the top edge)
// Every island is drawn on the menu's screen frame (design px of a W x H screen, W = screen px / factor: islands
// stick to the real edges on 16:9, 21:9 and 4:3). Each island has an opaque plate and a full-size blocker hit, so a
// click on it never reaches the map (the engine MapWidget still sees the LMB press: VPPMapMenu keeps the view).
// Every widget gets own sort 300 (siblings over Map_Widget, priority 100). Built once per map open; unlinked with
// the screen frame by VPPMapMenu.INT_VPP_Teardown. Client only.
// ============================================================================

// Shared look of the map windows (b_survey.css / exp.css values that the kit does not carry).
class INT_VPP_Ui
{
	static const int ISL_BG = 0xFF0D0C0C;      // .isl #0d0c0c
	static const int ISL_EDGE = 0x1AFFFFFF;    // rgba(255,255,255,.1)
	static const int CARD_BG = 0xFF141312;     // .card #141312
	static const int CARD_EDGE = 0x26FFFFFF;   // rgba(255,255,255,.15)
	static const int CHIP_BG = 0xFF121110;     // .chip #121110
	static const int CHIP_EDGE = 0x14FFFFFF;   // rgba(255,255,255,.08)
	static const int CHIP_OFF_TEXT = 0xFF6C6862;
	static const int CHIP_OFF_DIA = 0xFF3A3835;
	static const int LINE2 = 0x38FFFFFF;       // --l2
	static const int T2 = 0xFFD9D3CA;          // --t2
	static const int M = 0xFF948F88;           // --m
	static const int D = 0xFF5D5953;           // --d
	static const int BTN_BG = 0xFF0F0E0D;
	static const int KBD_EDGE = 0x6BFFFFFF;
	static const int INT_SORT = 300;
	// the «Слои» island (2026-11-13: one more row for the minimap): its top is INT_LAYERS_TOP above the screen
	// bottom; the islands of the zone / label editors and the popovers end 8 px above it
	static const float INT_LAYERS_TOP = 266;
	static const float INT_LAYERS_H = 242;

	// opaque plate + 1 px edge (+ 2 px orange top line)
	static Widget INT_Panel(Widget parent, float x, float y, float w, float h, int bg, int edge, bool topLine, string nm)
	{
		Widget f = INT_UIF_Draw.Frame(parent, x, y, w, h, nm);
		if (!f)
			return null;
		INT_VPPX_Look.Fill(f, 0, 0, w, h, bg);
		INT_VPPX_Look.Outline(f, 0, 0, w, h, edge);
		if (topLine)
			INT_VPPX_Look.Fill(f, 0, 0, w, 2, INT_VPPX_Look.ORANGE);
		return f;
	}

	// orange hairline from x1 to x2 at y + diamond 10 at x2 (the card connector)
	static void INT_Pin(Widget parent, float x1, float y, float x2)
	{
		float a = x1;
		float b = x2;
		if (b < a)
		{
			a = x2;
			b = x1;
		}
		INT_VPPX_Look.Fill(parent, a, y, b - a, 1, INT_VPPX_Look.ORANGE);
		INT_VPPX_Look.Diamond(parent, x2, y, 10, true, INT_VPPX_Look.ORANGE);
	}

	// keycap + caps label (tracked, or plain upper case where the strip is short of room); returns the width used
	static float INT_Key(Widget parent, float x, float y, string key, string label, bool tracked = true)
	{
		float kw = INT_VPPX_Look.Keycap(parent, x, y, key, 26, KBD_EDGE, INT_VPPX_Look.TEXT);
		string caps = INT_VPP_Ui.INT_KeyLabel(label, tracked);
		float lw = INT_VPPX_Look.Measure(caps, 13, true);
		INT_VPPX_Look.Text(parent, x + kw + 10, y, lw + 30, 26, caps, 13, M, INT_UIF_Align.LEFT, false);
		return kw + 10 + lw;
	}

	static string INT_KeyLabel(string label, bool tracked)
	{
		if (tracked)
			return INT_VPPX_Text.Caps(label);
		return INT_VPPX_Text.Upper(label);
	}

	// width of one keycap + label as INT_Key draws it
	static float INT_KeyWidth(string key, string label, bool tracked)
	{
		string kk = INT_VPPX_Text.Upper(key);
		float kw = INT_VPPX_Look.Measure(kk, 13, true) + 18;
		if (kw < 30)
			kw = 30;
		string lc = INT_VPP_Ui.INT_KeyLabel(label, tracked);
		float lw = INT_VPPX_Look.Measure(lc, 13, true);
		return kw + 10 + lw;
	}

	static void INT_Sort(Widget root)
	{
		INT_VPPX_Look.SortTree(root, INT_SORT);
	}

	// a stringtable text (2026-11-14: the new texts have RU / EN / DE in stringtable.csv), translated for measuring
	static string INT_Tr(string key)
	{
		string tagged = "#" + key;
		string t = Widget.TranslateString(tagged);
		if (t == "" || t == tagged)
			return key;
		return t;
	}

	static string INT_UifIcon(string name)
	{
		return INT_UIF_SET + "iuf_icon_" + name;
	}
}

// 40x40 icon button (exp.css .ibtn): opaque plate, 1 px edge, icon; «on» = orange edge + icon.
class INT_VPP_IconBtn : ScriptedWidgetEventHandler
{
	protected Widget m_INT_Root;
	protected Widget m_INT_Edge;
	protected Widget m_INT_EdgeOn;
	protected ImageWidget m_INT_Icon;
	protected ButtonWidget m_INT_Hit;
	protected int m_INT_IconColor;
	protected bool m_INT_On;
	protected bool m_INT_Hover;
	protected ref ScriptInvoker m_INT_OnClick;
	int m_INT_Tag;

	void INT_VPP_IconBtn()
	{
		m_INT_OnClick = new ScriptInvoker();
	}

	// (INT_VPP_IconBtn b)
	ScriptInvoker INT_OnClick()
	{
		return m_INT_OnClick;
	}

	void INT_Build(Widget parent, float x, float y, float size, string texPath, float iconSize, int iconColor)
	{
		m_INT_IconColor = iconColor;
		m_INT_Root = INT_UIF_Draw.Frame(parent, x, y, size, size, "IntVppIBtn");
		if (!m_INT_Root)
			return;
		INT_VPPX_Look.Fill(m_INT_Root, 0, 0, size, size, INT_VPP_Ui.BTN_BG);
		m_INT_Edge = INT_UIF_Draw.Frame(m_INT_Root, 0, 0, size, size, "Edge");
		INT_VPPX_Look.Outline(m_INT_Edge, 0, 0, size, size, INT_VPP_Ui.LINE2);
		m_INT_EdgeOn = INT_UIF_Draw.Frame(m_INT_Root, 0, 0, size, size, "EdgeOn");
		INT_VPPX_Look.Outline(m_INT_EdgeOn, 0, 0, size, size, INT_VPPX_Look.ORANGE);
		float io = (size - iconSize) * 0.5;
		m_INT_Icon = INT_UIF_Draw.Texture(m_INT_Root, io, io, iconSize, iconSize, texPath, iconColor);
		m_INT_Hit = INT_UIF_Draw.Hit(m_INT_Root, 0, 0, size, size, this);
		INT_Paint();
	}

	Widget INT_GetRoot()
	{
		return m_INT_Root;
	}

	ImageWidget INT_GetIcon()
	{
		return m_INT_Icon;
	}

	void INT_SetOn(bool on)
	{
		m_INT_On = on;
		INT_Paint();
	}

	void INT_Show(bool show)
	{
		if (m_INT_Root && m_INT_Root.IsVisible() != show)
			m_INT_Root.Show(show);
	}

	protected void INT_Paint()
	{
		if (!m_INT_Edge || !m_INT_EdgeOn || !m_INT_Icon)
			return;
		bool lit = false;
		if (m_INT_On || m_INT_Hover)
			lit = true;
		m_INT_Edge.Show(!lit);
		m_INT_EdgeOn.Show(lit);
		int c = m_INT_IconColor;
		if (lit)
			c = INT_VPPX_Look.ORANGE;
		m_INT_Icon.SetColor(c);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_INT_Hit || button != MouseState.LEFT)
			return false;
		INT_VPP_IconBtn keep = this;
		m_INT_OnClick.Invoke(keep);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_INT_Hit)
			return false;
		m_INT_Hover = true;
		INT_Paint();
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.HOVER);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_INT_Hit)
			return false;
		m_INT_Hover = false;
		INT_Paint();
		return false;
	}
}

// One row of the expanded list (header / marker / zone / route point).
class INT_VPP_ListRow : Managed
{
	static const int TYPE_HEADER = 0;
	static const int TYPE_ITEM = 1;
	static const int TYPE_ZONE = 2;
	static const int TYPE_ROUTE = 3;

	int m_INT_Type;
	string m_INT_Text;
	string m_INT_Sub;
	int m_INT_Color;
	string m_INT_Icon;
	bool m_INT_Dim;
	INT_VPP_MapItem m_INT_Item;
	int m_INT_ZoneId;
	int m_INT_RouteIdx;
	vector m_INT_Pos;
	// mode chip (2026-11-12): markers of «Сервер» / «Мои» / «События» and their group headers only
	bool m_INT_HasMode;
	int m_INT_Layer;
	int m_INT_Mode;          // INT_VPP_MODE_*, header: -1 = mixed
	bool m_INT_Can3;
	bool m_INT_Can2;
}

// Tab switch entrance (2026-11-14, the PartyMode / Trader tab family): the new tab's panel slides 10 design px up into
// place while every widget fades from 0 to its own alpha, the top-level blocks one after another. A new tween on the
// same widget + property replaces the running one (INT_UIF_Animator), so quick clicks never stack tweens; the old
// tab's panel is unlinked at once (a cross-fade would fight the left-column layout).
class INT_VPP_TabFx
{
	static const float INT_DUR = 0.22;
	static const float INT_STAGGER = 0.035;
	static const float INT_SLIDE = 10;

	static void INT_In(Widget root, float delay)
	{
		if (!root)
			return;
		float x;
		float y;
		root.GetPos(x, y);
		INT_UIF_Animator.Get().Cancel(root, INT_UIF_TweenProp.POS_Y);
		float slide = INT_UIF_Draw.S(INT_SLIDE);
		root.SetPos(x, y + slide);
		INT_UIF_Tween mv = INT_UIF_Animator.MoveY(root, y, INT_DUR + 0.08, INT_UIF_Ease.OUT_CUBIC);
		if (mv)
			mv.Delay(delay);
		INT_VPP_TabFx.INT_FadeOne(root, delay);
		Widget c = root.GetChildren();
		int i = 0;
		while (c)
		{
			float d = delay + i * INT_STAGGER;
			INT_VPP_TabFx.INT_FadeTree(c, d);
			i++;
			c = c.GetSibling();
		}
	}

	protected static void INT_FadeTree(Widget w, float delay)
	{
		INT_VPP_TabFx.INT_FadeOne(w, delay);
		Widget c = w.GetChildren();
		while (c)
		{
			INT_VPP_TabFx.INT_FadeTree(c, delay);
			c = c.GetSibling();
		}
	}

	protected static void INT_FadeOne(Widget w, float delay)
	{
		float home = w.GetAlpha();
		if (home <= 0.004)
			return;
		INT_UIF_Animator.Get().Cancel(w, INT_UIF_TweenProp.ALPHA);
		w.SetAlpha(0);
		INT_UIF_Tween t = INT_UIF_Animator.Fade(w, home, INT_DUR, INT_UIF_Ease.OUT_CUBIC);
		if (t)
			t.From(0).Delay(delay);
	}
}

// A big-map panel the player may drag (2026-11-14): its frame, the widget a press must start on (the panel's own
// blocker = its header and every empty spot; buttons / fields / steppers keep their clicks) and where it stands now
// (design px of the map screen).
class INT_VPP_DragEntry : Managed
{
	string m_INT_Key;
	Widget m_INT_Panel;          // weak: the panel's owner unlinks it
	ButtonWidget m_INT_Handle;   // weak
	float m_INT_DefX;
	float m_INT_DefY;
	float m_INT_W;
	float m_INT_H;
	float m_INT_X;
	float m_INT_Y;
}

class INT_VPP_Islands : ScriptedWidgetEventHandler
{
	static const int MODE_MAP = 0;
	static const int MODE_LIST = 1;
	static const int MODE_ROUTE = 2;
	static const int MODE_EDITOR = 3;
	static const int MODE_ZONES = 4;

	static const int KEYS_NONE = -1;
	static const int KEYS_MAP = 0;
	static const int KEYS_EDITOR = 1;
	static const int KEYS_RULER = 2;
	static const int KEYS_ZONES = 3;
	static const int INT_KEYS_LABELS = 4;

	static const float TAB_WORD = 28;
	static const float LIST_W = 420;
	static const float LIST_ROW = 34;
	// the right edge: folded tab and expanded list both start level with the top-left island (no top-right island)
	static const float LIST_Y = 24;
	// 24 px above the coordinates island (134 tall since its Ctrl + C row, 2026-11-14)
	static const float LIST_BOTTOM = 182;
	static const int LAYER_3D = 5;

	// tab labels; the 4th and 5th only for map admins
	static const string TXT_TAB_MAP = "Карта";
	static const string TXT_TAB_MARKS = "Метки";
	static const string TXT_TAB_ROUTE = "Маршрут";
	static const string TXT_TAB_ZONES = "Зоны";
	static const string INT_TXT_TAB_LABELS = "Надписи";
	static const string TXT_LAYERS = "Слои";
	static const string TXT_OF = "из";
	static const string TXT_ME = "Я";
	static const string TXT_SQUARE = "Кв.";
	static const string TXT_CURSOR = "Курсор";
	static const string TXT_FROM_YOU = "от вас";
	static const string TXT_LIST = "СПИСОК";
	static const string TXT_MARKS_CAP = "Метки на карте";
	static const string TXT_3D = "3D-метки";
	static const string TXT_3D_K = "3D скрыты · K";
	static const string TXT_3D_TITLE = "Дальность 3D-меток";
	static const string TXT_3D_NOTE = "предел сервера";
	// «ВИД 3D-МЕТОК» section of the same popover (2026-11-12): size + transparency of everything drawn in 3D
	static const string TXT_LOOK_TITLE = "Вид 3D-меток";
	static const string TXT_LOOK_SIZE = "Размер";
	static const string TXT_LOOK_FADE = "Прозрачность";
	static const string TXT_LOOK_RESET = "Сбросить";
	static const string TXT_LOOK_SAMPLE = "Пример";
	static const string TXT_LOOK_SAMPLE_DIST = "350 м";
	// 2026-11-14: texts next to the 3D markers, two switches under the steppers
	static const string TXT_LOOK_NAMES = "Названия";
	static const string TXT_LOOK_DIST = "Расстояние";
	static const string TXT_LOOK_ON = "Вкл";
	static const int LOOK_SWATCH = 0xFF4A4C4E;     // neutral mid grey under the preview
	static const float POP_W = 476;
	static const float LOOK_TOP = 262;             // separator under the distance note
	static const float LOOK_SWATCH_Y = 492;
	static const float LOOK_SWATCH_H = 112;
	static const string TXT_OFF = "выкл";
	static const string TXT_EMPTY_LIST = "Пусто";
	// draggable panels (2026-11-14): keys in Client.json INT_PanelPos (never rename), the press-to-drag threshold
	static const string INT_PAN_TABS = "tabs";
	static const string INT_PAN_LAYERS = "layers";
	static const string INT_PAN_COORDS = "coords";
	static const string INT_PAN_KEYS = "keys";
	static const string INT_PAN_POP3D = "pop3d";
	static const string INT_PAN_MINI = "minimap_panel";
	static const string INT_PAN_ZONES = "zone_editor";
	static const string INT_PAN_LABELS = "label_editor";
	static const string INT_PAN_MARKER = "marker_editor";
	static const string INT_PAN_ROUTE = "route_ribbon";
	static const float INT_DRAG_THRESHOLD = 6;      // design px the cursor travels before a press becomes a drag
	static const float INT_DRAG_DOUBLE = 0.4;       // s between two clicks on a header = put that panel back
	static const string TXT_PANELS_RESET = "Вернуть панели на место";

	protected VPPMapMenu m_INT_Menu;
	protected Widget m_INT_Screen;
	protected float m_INT_W;
	protected float m_INT_H;
	protected bool m_INT_Admin;

	// top-left
	protected Widget m_INT_TL;
	protected ref array<TextWidget> m_INT_TabNum;
	protected ref array<TextWidget> m_INT_TabWord;
	protected ref array<TextWidget> m_INT_TabBadge;
	protected ref array<ButtonWidget> m_INT_TabHit;
	protected ref array<float> m_INT_TabX;
	protected ref array<float> m_INT_TabW;
	protected Widget m_INT_TabUl;
	protected int m_INT_Tab;
	// layers
	protected Widget m_INT_Layers;
	protected TextWidget m_INT_LayersCount;
	protected ref array<ImageWidget> m_INT_ChipBg;
	protected ref array<ImageWidget> m_INT_ChipDia;
	protected ref array<TextWidget> m_INT_ChipName;
	protected ref array<TextWidget> m_INT_ChipCount;
	protected ref array<ref INT_VPPX_Check> m_INT_ChipCheck;
	protected ref array<ButtonWidget> m_INT_ChipHit;
	protected ref array<int> m_INT_ChipColor;
	protected ButtonWidget m_INT_GearHit;
	protected ImageWidget m_INT_Gear;
	protected bool m_INT_Painting;
	// minimap row (2026-11-13) + its settings panel
	protected ref INT_VPPX_Check m_INT_MiniCheck;
	protected ButtonWidget m_INT_MiniHit;
	protected ButtonWidget m_INT_MiniGearHit;
	protected ImageWidget m_INT_MiniGear;
	protected Widget m_INT_MiniGearBox;
	protected ImageWidget m_INT_MiniBg;
	protected ImageWidget m_INT_MiniDia;
	protected TextWidget m_INT_MiniName;
	protected ref INT_VPP_MiniPanel m_INT_MiniPanel;
	static const string TXT_MINI = "Миникарта";
	// 3D popover
	protected Widget m_INT_Pop;
	protected ref array<ButtonWidget> m_INT_PopMinus;
	protected ref array<ButtonWidget> m_INT_PopPlus;
	protected ref array<TextWidget> m_INT_PopVal;
	protected ref array<int> m_INT_PopLayer;
	protected Widget m_INT_DeadPop;
	// «ВИД 3D-МЕТОК»: row 0 = size, row 1 = transparency; the preview tree lives inside the popover (weak)
	protected ref array<ButtonWidget> m_INT_LookMinus;
	protected ref array<ButtonWidget> m_INT_LookPlus;
	protected ref array<TextWidget> m_INT_LookVal;
	protected ButtonWidget m_INT_LookReset;
	protected Widget m_INT_LookPrev;
	protected int m_INT_LookPrevScale = -1;
	protected int m_INT_LookPrevFade = -1;
	protected int m_INT_LookPrevTexts = -1;
	protected ref INT_VPPX_Toggle m_INT_LookNames;
	protected ref INT_VPPX_Toggle m_INT_LookDist;
	protected bool m_INT_LookPainting;
	// keys
	protected Widget m_INT_Keys;
	protected int m_INT_KeysMode = -2;
	// coords
	protected Widget m_INT_Coords;
	protected TextWidget m_INT_MeText;
	protected TextWidget m_INT_SqText;
	protected TextWidget m_INT_SqCap;
	protected TextWidget m_INT_CurText;
	// Ctrl + C (2026-11-14): the hint under the cursor line, «Скопировано» for INT_COPY_FLASH s after a copy
	protected TextWidget m_INT_CopyText;
	protected float m_INT_CopyFlash;
	static const float INT_COPY_FLASH = 1.0;
	static const float INT_COORDS_H = 134;
	protected string m_INT_LastMe;
	protected string m_INT_LastSq;
	protected string m_INT_LastCur;
	// list (folded tab + expanded panel)
	protected Widget m_INT_Drawer;
	protected ButtonWidget m_INT_DrawerHit;
	protected TextWidget m_INT_DrawerCount;
	protected Widget m_INT_List;
	protected Widget m_INT_ListRows;
	protected ButtonWidget m_INT_ListWheel;
	protected ButtonWidget m_INT_ListFold;
	protected TextWidget m_INT_ListCount;
	protected ref array<ref INT_VPP_ListRow> m_INT_Rows;
	protected ref array<ButtonWidget> m_INT_RowHits;
	protected ref array<int> m_INT_RowIdx;
	protected int m_INT_Scroll;
	protected int m_INT_VisibleRows;
	protected Widget m_INT_DeadRows;
	protected Widget m_INT_DeadList;
	protected bool m_INT_ListOpen;

	protected ref array<ButtonWidget> m_INT_Blockers;

	// draggable panels (2026-11-14)
	protected ref array<ref INT_VPP_DragEntry> m_INT_Drags;
	protected INT_VPP_DragEntry m_INT_DragCand;      // weak: the entry a press started on
	protected bool m_INT_Dragging;
	protected bool m_INT_DragLmbWas;
	protected int m_INT_DragMX;
	protected int m_INT_DragMY;
	protected float m_INT_DragSX;
	protected float m_INT_DragSY;
	protected bool m_INT_PanelMoved;
	protected float m_INT_DragClock;
	protected float m_INT_LastClickAt = -10;
	protected string m_INT_LastClickKey;
	protected ButtonWidget m_INT_PanelsReset;
	// tabs (2026-11-14): the first paint after a build is immediate, later tab changes slide / colour-tween (only
	// after the menu allowed it: opening the map shows the active tab at once)
	protected bool m_INT_TabsPainted;
	protected bool m_INT_TabAnimOn;
	static const float INT_TAB_SLIDE = 0.3;
	static const float INT_TAB_TINT = 0.25;

	// mode chips of the visible list rows (2026-11-12): 3D / 2D / × per marker, the same on the group headers
	static const float CHIP_W = 42;
	static const float CHIP_H = 24;
	static const int CHIP_3D_BG = 0xFF2E1D10;
	static const string TXT_MODE_3D = "3D: на карте и в мире";
	static const string TXT_MODE_2D = "2D: только на карте";
	static const string TXT_MODE_OFF = "Скрыта";
	static const string TXT_MODE_MIXED = "Разные режимы";
	static const string TXT_MODE_NO3D = " · без 3D (сервер)";
	static const string TXT_MODE_GROUP = "Вся группа · ";
	protected ref array<ButtonWidget> m_INT_ModeHits;
	protected ref array<int> m_INT_ModeRowIdx;
	protected ref array<ImageWidget> m_INT_ModeBg;
	protected ref array<TextWidget> m_INT_ModeTxt;
	protected ref array<TextWidget> m_INT_ModeName;
	// what each chip / the caption shows now (TextWidget has no getter)
	protected ref array<string> m_INT_ChipShown;
	protected string m_INT_CapShown;
	protected TextWidget m_INT_ListCap;

	// short notice under the top islands (limits, errors), gone after NOTICE_TIME
	static const float NOTICE_TIME = 2.6;
	protected Widget m_INT_Notice;
	protected float m_INT_NoticeLeft;

	void INT_VPP_Islands()
	{
		m_INT_TabNum = new array<TextWidget>();
		m_INT_TabWord = new array<TextWidget>();
		m_INT_TabBadge = new array<TextWidget>();
		m_INT_TabHit = new array<ButtonWidget>();
		m_INT_TabX = new array<float>();
		m_INT_TabW = new array<float>();
		m_INT_ChipBg = new array<ImageWidget>();
		m_INT_ChipDia = new array<ImageWidget>();
		m_INT_ChipName = new array<TextWidget>();
		m_INT_ChipCount = new array<TextWidget>();
		m_INT_ChipCheck = new array<ref INT_VPPX_Check>();
		m_INT_ChipHit = new array<ButtonWidget>();
		m_INT_ChipColor = new array<int>();
		m_INT_PopMinus = new array<ButtonWidget>();
		m_INT_PopPlus = new array<ButtonWidget>();
		m_INT_PopVal = new array<TextWidget>();
		m_INT_PopLayer = new array<int>();
		m_INT_LookMinus = new array<ButtonWidget>();
		m_INT_LookPlus = new array<ButtonWidget>();
		m_INT_LookVal = new array<TextWidget>();
		m_INT_Rows = new array<ref INT_VPP_ListRow>();
		m_INT_RowHits = new array<ButtonWidget>();
		m_INT_RowIdx = new array<int>();
		m_INT_Blockers = new array<ButtonWidget>();
		m_INT_Drags = new array<ref INT_VPP_DragEntry>();
		m_INT_ModeHits = new array<ButtonWidget>();
		m_INT_ModeRowIdx = new array<int>();
		m_INT_ModeBg = new array<ImageWidget>();
		m_INT_ModeTxt = new array<TextWidget>();
		m_INT_ModeName = new array<TextWidget>();
		m_INT_ChipShown = new array<string>();
	}

	// ------------------------------------------------------------------ build / destroy
	void INT_Build(Widget screen, float w, float h, VPPMapMenu menu, bool admin)
	{
		m_INT_Screen = screen;
		m_INT_W = w;
		m_INT_H = h;
		m_INT_Menu = menu;
		m_INT_Admin = admin;
		INT_BuildTopLeft();
		INT_BuildLayers();
		INT_BuildCoords();
		INT_BuildDrawer();
		INT_SetKeys(KEYS_MAP);
	}

	// The screen frame is unlinked by the menu; only refs and popups are dropped here.
	void INT_Destroy()
	{
		INT_CloseMiniPanel();
		if (m_INT_MiniCheck)
			m_INT_MiniCheck.OnChanged().Clear();
		m_INT_MiniCheck = null;
		if (m_INT_Notice)
			m_INT_Notice.Unlink();
		m_INT_Notice = null;
		if (m_INT_Pop)
			m_INT_Pop.Unlink();
		m_INT_Pop = null;
		if (m_INT_DeadPop)
			m_INT_DeadPop.Unlink();
		m_INT_DeadPop = null;
		INT_ForgetLook();
		if (m_INT_DeadRows)
			m_INT_DeadRows.Unlink();
		m_INT_DeadRows = null;
		if (m_INT_DeadList)
			m_INT_DeadList.Unlink();
		m_INT_DeadList = null;
		foreach (INT_VPPX_Check c : m_INT_ChipCheck)
		{
			if (c)
				c.OnChanged().Clear();
		}
		m_INT_ChipCheck.Clear();
		m_INT_Rows.Clear();
		m_INT_RowHits.Clear();
		INT_ForgetChips();
		m_INT_ListCap = null;
		m_INT_Blockers.Clear();
		m_INT_Drags.Clear();
		m_INT_DragCand = null;
		m_INT_Dragging = false;
		m_INT_Menu = null;
	}

	void INT_ShowNotice(string text)
	{
		if (m_INT_Notice)
			m_INT_Notice.Unlink();
		m_INT_Notice = null;
		float tw = INT_VPPX_Look.Measure(text, 15) + 56;
		float x0 = (m_INT_W - tw) * 0.5;
		m_INT_Notice = INT_VPP_Ui.INT_Panel(m_INT_Screen, x0, 130, tw, 40, INT_VPP_Ui.CARD_BG, INT_VPP_Ui.CARD_EDGE, true, "IntVppNotice");
		if (!m_INT_Notice)
			return;
		INT_VPPX_Look.Diamond(m_INT_Notice, 20, 21, 7, true, INT_VPPX_Look.ORANGE);
		INT_VPPX_Look.Text(m_INT_Notice, 34, 9, tw - 40, 24, text, 15, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		INT_VPP_Ui.INT_Sort(m_INT_Notice);
		m_INT_NoticeLeft = NOTICE_TIME;
	}

	// per frame: popups / rows closed inside their own click are unlinked here
	void INT_Tick(float dt)
	{
		INT_DragTick(dt);
		INT_CopyFlashTick(dt);
		if (m_INT_MiniPanel)
			m_INT_MiniPanel.INT_Tick(dt);
		if (m_INT_Notice)
		{
			m_INT_NoticeLeft = m_INT_NoticeLeft - dt;
			if (m_INT_NoticeLeft <= 0)
			{
				m_INT_Notice.Unlink();
				m_INT_Notice = null;
			}
		}
		if (m_INT_DeadPop)
		{
			m_INT_DeadPop.Unlink();
			m_INT_DeadPop = null;
		}
		// rows (a child of the list) before the list itself
		if (m_INT_DeadRows)
		{
			m_INT_DeadRows.Unlink();
			m_INT_DeadRows = null;
		}
		if (m_INT_DeadList)
		{
			m_INT_DeadList.Unlink();
			m_INT_DeadList = null;
		}
	}

	protected ButtonWidget INT_Blocker(Widget panel, float w, float h)
	{
		// review #14: blockers of unlinked panels (popover, rebuilt top-left / list) leave null weak refs: drop them
		for (int i = m_INT_Blockers.Count() - 1; i >= 0; i--)
		{
			if (!m_INT_Blockers.Get(i))
				m_INT_Blockers.Remove(i);
		}
		ButtonWidget b = INT_UIF_Draw.Hit(panel, 0, 0, w, h, this);
		if (b)
			m_INT_Blockers.Insert(b);
		return b;
	}

	// a blocker for a panel owned by another class (the minimap panel); it is also that panel's drag handle
	ButtonWidget INT_AddBlocker(Widget panel, float w, float h)
	{
		ButtonWidget b = INT_Blocker(panel, w, h);
		return b;
	}

	// ------------------------------------------------------------------ draggable panels (2026-11-14)
	// Every floating panel of the map registers here after it is built (again after a rebuild: the key replaces the
	// old entry). A press on its handle that travels INT_DRAG_THRESHOLD design px drags the panel; the press is on our
	// UI, so the map neither pans nor gets a click (VPPMapMenu.INT_Mouse: a map click must start over the MapWidget,
	// and the view guard keeps the view while LMB is held on UI). The place is kept in Client.json per key and
	// clamped into the screen when applied (a new resolution / a smaller window). Two quick clicks on a handle put
	// that panel back; «Вернуть панели на место» in «Слои» puts all back.
	void INT_RegisterDrag(string key, Widget panel, ButtonWidget handle)
	{
		INT_UnregisterDrag(key);
		if (!panel || !handle)
			return;
		float f = INT_UIF_Draw.S(1.0);
		if (f <= 0)
			return;
		float px;
		float py;
		panel.GetPos(px, py);
		float pw;
		float ph;
		panel.GetSize(pw, ph);
		INT_VPP_DragEntry en = new INT_VPP_DragEntry();
		en.m_INT_Key = key;
		en.m_INT_Panel = panel;
		en.m_INT_Handle = handle;
		en.m_INT_DefX = px / f;
		en.m_INT_DefY = py / f;
		en.m_INT_W = pw / f;
		en.m_INT_H = ph / f;
		en.m_INT_X = en.m_INT_DefX;
		en.m_INT_Y = en.m_INT_DefY;
		m_INT_Drags.Insert(en);
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
		float sx;
		float sy;
		if (pr && pr.INT_GetPanelPos(key, sx, sy))
		{
			INT_PlaceDrag(en, sx, sy);
			m_INT_PanelMoved = true;
		}
	}

	void INT_UnregisterDrag(string key)
	{
		for (int i = m_INT_Drags.Count() - 1; i >= 0; i--)
		{
			INT_VPP_DragEntry en = m_INT_Drags.Get(i);
			if (!en || en.m_INT_Key == key || !en.m_INT_Panel)
			{
				if (en && en == m_INT_DragCand)
				{
					m_INT_DragCand = null;
					m_INT_Dragging = false;
				}
				m_INT_Drags.Remove(i);
			}
		}
	}

	// the panel to (x, y) design px, kept inside the screen
	protected void INT_PlaceDrag(INT_VPP_DragEntry en, float x, float y)
	{
		float maxX = m_INT_W - en.m_INT_W;
		float maxY = m_INT_H - en.m_INT_H;
		if (x > maxX)
			x = maxX;
		if (y > maxY)
			y = maxY;
		if (x < 0)
			x = 0;
		if (y < 0)
			y = 0;
		en.m_INT_X = x;
		en.m_INT_Y = y;
		if (en.m_INT_Panel)
			INT_UIF_Draw.Move(en.m_INT_Panel, x, y);
	}

	protected INT_VPP_DragEntry INT_DragOf(Widget w)
	{
		if (!w)
			return null;
		foreach (INT_VPP_DragEntry en : m_INT_Drags)
		{
			if (!en || !en.m_INT_Panel || !en.m_INT_Handle)
				continue;
			if (en.m_INT_Handle == w && en.m_INT_Panel.IsVisible())
				return en;
		}
		return null;
	}

	protected INT_VPP_DragEntry INT_DragByKey(string key)
	{
		foreach (INT_VPP_DragEntry en : m_INT_Drags)
		{
			if (en && en.m_INT_Key == key && en.m_INT_Panel)
				return en;
		}
		return null;
	}

	protected void INT_DragTick(float dt)
	{
		m_INT_DragClock = m_INT_DragClock + dt;
		int lmbState = GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK;
		bool lmb = false;
		if (lmbState != 0)
			lmb = true;
		int mx;
		int my;
		GetMousePos(mx, my);
		if (lmb && !m_INT_DragLmbWas)
		{
			m_INT_Dragging = false;
			m_INT_DragCand = null;
			Widget under = GetWidgetUnderCursor();
			INT_VPP_DragEntry hit = INT_DragOf(under);
			if (hit)
			{
				m_INT_DragCand = hit;
				m_INT_DragMX = mx;
				m_INT_DragMY = my;
				m_INT_DragSX = hit.m_INT_X;
				m_INT_DragSY = hit.m_INT_Y;
			}
		}
		if (lmb && m_INT_DragCand)
		{
			if (!m_INT_DragCand.m_INT_Panel)
			{
				m_INT_DragCand = null;
				m_INT_Dragging = false;
			}
			else
			{
				float f = INT_UIF_Draw.S(1.0);
				float dx = mx - m_INT_DragMX;
				float dy = my - m_INT_DragMY;
				dx = dx / f;
				dy = dy / f;
				if (!m_INT_Dragging && dx * dx + dy * dy >= INT_DRAG_THRESHOLD * INT_DRAG_THRESHOLD)
					m_INT_Dragging = true;
				if (m_INT_Dragging)
				{
					INT_PlaceDrag(m_INT_DragCand, m_INT_DragSX + dx, m_INT_DragSY + dy);
					m_INT_PanelMoved = true;
				}
			}
		}
		if (!lmb && m_INT_DragLmbWas)
		{
			INT_VPP_DragEntry done = m_INT_DragCand;
			if (done && m_INT_Dragging)
			{
				INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
				if (pr)
				{
					pr.INT_SetPanelPos(done.m_INT_Key, done.m_INT_X, done.m_INT_Y);
					pr.INT_Save();
				}
				m_INT_LastClickKey = "";
			}
			else if (done)
			{
				// two quick clicks on the same handle: that panel goes back to its place
				float since = m_INT_DragClock - m_INT_LastClickAt;
				if (done.m_INT_Key == m_INT_LastClickKey && since <= INT_DRAG_DOUBLE)
				{
					INT_ResetPanel(done.m_INT_Key);
					m_INT_LastClickKey = "";
				}
				else
				{
					m_INT_LastClickKey = done.m_INT_Key;
					m_INT_LastClickAt = m_INT_DragClock;
				}
			}
			m_INT_DragCand = null;
			m_INT_Dragging = false;
		}
		m_INT_DragLmbWas = lmb;
	}

	// true once after a panel moved (dragged / put back): the menu re-places the pin lines of its cards
	bool INT_TakePanelMoved()
	{
		bool moved = m_INT_PanelMoved;
		m_INT_PanelMoved = false;
		return moved;
	}

	bool INT_IsDragging()
	{
		return m_INT_Dragging;
	}

	// one panel back to its default place (double click on its header)
	void INT_ResetPanel(string key)
	{
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
		bool had = false;
		float ox;
		float oy;
		if (pr && pr.INT_GetPanelPos(key, ox, oy))
			had = true;
		if (pr)
		{
			pr.INT_ClearPanelPos(key);
			pr.INT_Save();
		}
		INT_VPP_DragEntry en = INT_DragByKey(key);
		if (en)
		{
			INT_PlaceDrag(en, en.m_INT_DefX, en.m_INT_DefY);
			m_INT_PanelMoved = true;
		}
		if (had)
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
	}

	// «Вернуть панели на место»: every panel back, the saved places forgotten
	void INT_ResetAllPanels()
	{
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
		int saved = 0;
		if (pr)
			saved = pr.INT_PanelPosCount();
		if (pr)
		{
			pr.INT_ClearAllPanelPos();
			pr.INT_Save();
		}
		foreach (INT_VPP_DragEntry en : m_INT_Drags)
		{
			if (en && en.m_INT_Panel)
				INT_PlaceDrag(en, en.m_INT_DefX, en.m_INT_DefY);
		}
		m_INT_PanelMoved = true;
		if (saved > 0)
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
		else
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
	}

	// TEST: a panel's place (design px), false = not registered
	bool INT_TestPanelPos(string key, out float x, out float y)
	{
		x = 0;
		y = 0;
		INT_VPP_DragEntry en = INT_DragByKey(key);
		if (!en)
			return false;
		x = en.m_INT_X;
		y = en.m_INT_Y;
		return true;
	}

	// TEST: a drag without the mouse (the same place / save path)
	void INT_TestDragTo(string key, float x, float y)
	{
		INT_VPP_DragEntry en = INT_DragByKey(key);
		if (!en)
			return;
		INT_PlaceDrag(en, x, y);
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
		if (pr)
		{
			pr.INT_SetPanelPos(key, en.m_INT_X, en.m_INT_Y);
			pr.INT_Save();
		}
		m_INT_PanelMoved = true;
	}

	protected bool INT_IsBlocker(Widget w)
	{
		ButtonWidget b = ButtonWidget.Cast(w);
		if (!b)
			return false;
		if (m_INT_Blockers.Find(b) >= 0)
			return true;
		return false;
	}

	// ------------------------------------------------------------------ top-left: brand + tabs
	protected void INT_BuildTopLeft()
	{
		if (m_INT_TL)
			m_INT_TL.Unlink();
		m_INT_TL = null;
		m_INT_TabNum.Clear();
		m_INT_TabWord.Clear();
		m_INT_TabBadge.Clear();
		m_INT_TabHit.Clear();
		m_INT_TabX.Clear();
		m_INT_TabW.Clear();
		array<string> labels = new array<string>();
		labels.Insert(TXT_TAB_MAP);
		labels.Insert(TXT_TAB_MARKS);
		labels.Insert(TXT_TAB_ROUTE);
		if (m_INT_Admin)
		{
			labels.Insert(TXT_TAB_ZONES);
			labels.Insert(INT_TXT_TAB_LABELS);
		}
		// widths first (the island hugs its content)
		string brandL = INT_VPPX_Text.Caps("DayZavr");
		string brandR = INT_VPPX_Text.Caps("Map");
		float blw = INT_VPPX_Look.Measure(brandL, 22, true);
		float brw = INT_VPPX_Look.Measure(brandR, 22, true);
		float x = 26;
		float logoX = x;
		x = x + 24 + 16;
		float blx = x;
		x = x + blw + 16;
		float barX = x;
		x = x + 1 + 16;
		float brx = x;
		x = x + brw + 30;
		float sepX = x;
		x = x + 1 + 30;
		int n = labels.Count();
		for (int i = 0; i < n; i++)
		{
			string lab = labels.Get(i);
			float ww = INT_VPPX_Look.Measure(lab, TAB_WORD);
			float badgeRoom = 0;
			if (i == 1 || i == 2)
				badgeRoom = INT_VPPX_Look.Measure("00", 15, true) + 8;
			m_INT_TabX.Insert(x);
			m_INT_TabW.Insert(ww);
			x = x + ww + badgeRoom + 34;
		}
		float islW = x - 34 + 30;
		m_INT_TL = INT_VPP_Ui.INT_Panel(m_INT_Screen, 24, 24, islW, 88, INT_VPP_Ui.ISL_BG, INT_VPP_Ui.ISL_EDGE, true, "IntVppTL");
		if (!m_INT_TL)
			return;
		INT_Blocker(m_INT_TL, islW, 88);
		INT_VPPX_Look.Diamond(m_INT_TL, logoX + 12, 44, 16, false, INT_VPPX_Look.ORANGE);
		INT_VPPX_Look.Diamond(m_INT_TL, logoX + 12, 44, 6, true, INT_VPPX_Look.ORANGE);
		INT_VPPX_Look.Text(m_INT_TL, blx, 30, blw + 40, 28, brandL, 22, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, true);
		INT_VPPX_Look.Fill(m_INT_TL, barX, 34, 1, 20, INT_VPP_Ui.LINE2);
		INT_VPPX_Look.Text(m_INT_TL, brx, 30, brw + 40, 28, brandR, 22, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		INT_VPPX_Look.Fill(m_INT_TL, sepX, 31, 1, 26, INT_VPP_Ui.LINE2);
		for (int t = 0; t < n; t++)
		{
			float tx = m_INT_TabX.Get(t);
			float tw = m_INT_TabW.Get(t);
			string tlab = labels.Get(t);
			int num = t + 1;
			string pad = INT_VPPX_Text.Pad2(num);
			string spaced = INT_VPPX_Text.Spaced(pad);
			TextWidget nt = INT_VPPX_Look.Text(m_INT_TL, tx, 14, 80, 18, spaced, 13, INT_VPP_Ui.D, INT_UIF_Align.LEFT, false);
			if (nt)
				nt.SetBold(true);
			float troom = INT_VPPX_Look.Room(tw);
			TextWidget wt = INT_VPPX_Look.Text(m_INT_TL, tx - 1, 32, troom, 40, tlab, TAB_WORD, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
			TextWidget bt = null;
			if (t == 1 || t == 2)
			{
				bt = INT_VPPX_Look.Text(m_INT_TL, tx + tw + 8, 34, 50, 20, " ", 15, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, false);
				if (bt)
					bt.SetBold(true);
			}
			ButtonWidget hit = INT_UIF_Draw.Hit(m_INT_TL, tx - 10, 6, tw + 50, 80, this);
			m_INT_TabNum.Insert(nt);
			m_INT_TabWord.Insert(wt);
			m_INT_TabBadge.Insert(bt);
			m_INT_TabHit.Insert(hit);
		}
		m_INT_TabUl = INT_UIF_Draw.Frame(m_INT_TL, 0, 81, 10, 12, "IntVppTabUl");
		if (m_INT_TabUl)
		{
			float uw = m_INT_TabW.Get(0);
			INT_VPPX_Look.Fill(m_INT_TabUl, 0, 5, uw, 2, INT_VPPX_Look.ORANGE);
			INT_VPPX_Look.Diamond(m_INT_TabUl, 0, 6, 10, true, INT_VPPX_Look.ORANGE);
		}
		INT_VPP_Ui.INT_Sort(m_INT_TL);
		m_INT_TabsPainted = false;
		INT_PaintTabs();
		INT_RegisterDrag(INT_PAN_TABS, m_INT_TL, INT_HandleOf(m_INT_TL));
	}

	// the blocker of a panel (its first full-size hit) = its drag handle
	protected ButtonWidget INT_HandleOf(Widget panel)
	{
		if (!panel)
			return null;
		foreach (ButtonWidget b : m_INT_Blockers)
		{
			if (b && b.GetParent() == panel)
				return b;
		}
		return null;
	}

	void INT_SetAdmin(bool admin)
	{
		if (admin == m_INT_Admin)
			return;
		m_INT_Admin = admin;
		INT_BuildTopLeft();
	}

	void INT_SetTab(int idx)
	{
		bool changed = false;
		if (idx != m_INT_Tab)
			changed = true;
		m_INT_Tab = idx;
		// the same tab again (INT_ApplyMode runs often): nothing, a running slide keeps going
		if (!changed && m_INT_TabsPainted)
			return;
		if (changed && m_INT_TabsPainted && m_INT_TabAnimOn)
		{
			INT_AnimateTabs();
			return;
		}
		INT_PaintTabs();
	}

	// the menu turns the tab animation on once the map is up (no slide from tab 0 when it opens)
	void INT_SetTabAnim(bool on)
	{
		m_INT_TabAnimOn = on;
	}

	// tab change with motion (2026-11-14): words / numbers colour-tween, the underline + diamond slide and resize
	// in the bar's own coordinates (it follows a dragged bar)
	protected void INT_AnimateTabs()
	{
		int n = m_INT_TabWord.Count();
		for (int i = 0; i < n; i++)
		{
			bool on = false;
			if (i == m_INT_Tab)
				on = true;
			TextWidget nt = m_INT_TabNum.Get(i);
			TextWidget wt = m_INT_TabWord.Get(i);
			int nc = INT_VPP_Ui.D;
			int wc = INT_VPP_Ui.M;
			if (on)
			{
				nc = INT_VPPX_Look.ORANGE;
				wc = INT_VPPX_Look.WHITE;
			}
			if (nt)
				INT_UIF_Animator.ColorTo(nt, nc, INT_TAB_TINT, INT_UIF_Ease.OUT_CUBIC);
			if (wt)
				INT_UIF_Animator.ColorTo(wt, wc, INT_TAB_TINT, INT_UIF_Ease.OUT_CUBIC);
		}
		if (!m_INT_TabUl || m_INT_Tab < 0 || m_INT_Tab >= m_INT_TabX.Count())
		{
			if (m_INT_TabUl)
				m_INT_TabUl.Show(false);
			return;
		}
		float ux = m_INT_TabX.Get(m_INT_Tab);
		float uw = m_INT_TabW.Get(m_INT_Tab);
		if (!m_INT_TabUl.IsVisible())
		{
			// back from «no tab»: appear at the target, no slide from a stale place
			m_INT_TabUl.Show(true);
			INT_UIF_Draw.Place(m_INT_TabUl, ux, 81, uw, 12);
			Widget l0 = m_INT_TabUl.GetChildren();
			if (l0)
				INT_UIF_Draw.Resize(l0, uw, 2);
			return;
		}
		float sx = INT_UIF_Draw.S(ux);
		float sw = INT_UIF_Draw.S(uw);
		INT_UIF_Animator.MoveX(m_INT_TabUl, sx, INT_TAB_SLIDE, INT_UIF_Ease.IN_OUT_CUBIC);
		INT_UIF_Animator.ResizeW(m_INT_TabUl, sw, INT_TAB_SLIDE, INT_UIF_Ease.IN_OUT_CUBIC);
		Widget line = m_INT_TabUl.GetChildren();
		if (line)
			INT_UIF_Animator.ResizeW(line, sw, INT_TAB_SLIDE, INT_UIF_Ease.IN_OUT_CUBIC);
	}

	protected void INT_PaintTabs()
	{
		m_INT_TabsPainted = true;
		// a slide / tint still running on the bar is replaced by the immediate paint
		if (m_INT_TabUl)
			INT_UIF_Animator.Get().Cancel(m_INT_TabUl, -1);
		foreach (TextWidget cw : m_INT_TabWord)
		{
			if (cw)
				INT_UIF_Animator.Get().Cancel(cw, -1);
		}
		foreach (TextWidget cn : m_INT_TabNum)
		{
			if (cn)
				INT_UIF_Animator.Get().Cancel(cn, -1);
		}
		int n = m_INT_TabWord.Count();
		for (int i = 0; i < n; i++)
		{
			bool on = false;
			if (i == m_INT_Tab)
				on = true;
			TextWidget nt = m_INT_TabNum.Get(i);
			TextWidget wt = m_INT_TabWord.Get(i);
			int nc = INT_VPP_Ui.D;
			int wc = INT_VPP_Ui.M;
			if (on)
			{
				nc = INT_VPPX_Look.ORANGE;
				wc = INT_VPPX_Look.WHITE;
			}
			if (nt)
				nt.SetColor(nc);
			if (wt)
				wt.SetColor(wc);
		}
		if (!m_INT_TabUl || m_INT_Tab < 0 || m_INT_Tab >= m_INT_TabX.Count())
		{
			if (m_INT_TabUl)
				m_INT_TabUl.Show(false);
			return;
		}
		m_INT_TabUl.Show(true);
		float ux = m_INT_TabX.Get(m_INT_Tab);
		float uw = m_INT_TabW.Get(m_INT_Tab);
		INT_UIF_Draw.Place(m_INT_TabUl, ux, 81, uw, 12);
		Widget line = m_INT_TabUl.GetChildren();
		if (line)
			INT_UIF_Draw.Resize(line, uw, 2);
	}

	void INT_SetTabCount(int idx, int count)
	{
		if (idx < 0 || idx >= m_INT_TabBadge.Count())
			return;
		TextWidget b = m_INT_TabBadge.Get(idx);
		if (!b)
			return;
		string s = " ";
		if (count > 0)
			s = INT_VPPX_Text.Pad2(count);
		b.SetText(s);
	}

	// ------------------------------------------------------------------ layers island (2 x 3)
	protected void INT_BuildLayers()
	{
		float y0 = m_INT_H - INT_VPP_Ui.INT_LAYERS_TOP;
		float lh = INT_VPP_Ui.INT_LAYERS_H;
		m_INT_Layers = INT_VPP_Ui.INT_Panel(m_INT_Screen, 24, y0, 476, lh, INT_VPP_Ui.ISL_BG, INT_VPP_Ui.ISL_EDGE, false, "IntVppLayers");
		if (!m_INT_Layers)
			return;
		INT_Blocker(m_INT_Layers, 476, lh);
		string cap = INT_VPPX_Text.Caps(TXT_LAYERS);
		INT_VPPX_Look.Text(m_INT_Layers, 18, 12, 200, 20, cap, 13, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		m_INT_LayersCount = INT_VPPX_Look.Text(m_INT_Layers, 476 - 18 - 200, 12, 200, 20, " ", 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.RIGHT, false);
		if (m_INT_LayersCount)
			m_INT_LayersCount.SetBold(true);
		array<string> names = new array<string>();
		names.Insert("Сервер");
		names.Insert("Мои");
		names.Insert("События");
		names.Insert("Зоны");
		names.Insert("Маршрут");
		names.Insert(TXT_3D);
		m_INT_ChipColor.Clear();
		m_INT_ChipColor.Insert(0xFFF3EFE8);
		m_INT_ChipColor.Insert(0xFF56E07E);
		m_INT_ChipColor.Insert(0xFFFFC246);
		m_INT_ChipColor.Insert(0xFFCFE35A);
		m_INT_ChipColor.Insert(0xFFFF7A2F);
		m_INT_ChipColor.Insert(0xFF3EC7FF);
		for (int i = 0; i < 6; i++)
		{
			int colIdx = i - (i / 2) * 2;
			int rowIdx = i / 2;
			float cx = 18 + colIdx * 222;
			float cy = 42 + rowIdx * 46;
			Widget chip = INT_UIF_Draw.Frame(m_INT_Layers, cx, cy, 214, 40, "IntVppChip");
			ImageWidget bg = INT_VPPX_Look.Fill(chip, 0, 0, 214, 40, INT_VPP_Ui.CHIP_BG);
			INT_VPPX_Look.Outline(chip, 0, 0, 214, 40, INT_VPP_Ui.CHIP_EDGE);
			ButtonWidget hit = INT_UIF_Draw.Hit(chip, 0, 0, 214, 40, this);
			int dc = m_INT_ChipColor.Get(i);
			ImageWidget dia = INT_VPPX_Look.Diamond(chip, 19, 20, 10, true, dc);
			string nm = names.Get(i);
			TextWidget nt = INT_VPPX_Look.Text(chip, 38, 9, 130, 22, nm, 17, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
			TextWidget ct = null;
			if (i == LAYER_3D)
			{
				m_INT_Gear = INT_VPPX_Look.IconAt(chip, 158, 12, 16, "gear", INT_VPP_Ui.M);
				m_INT_GearHit = INT_UIF_Draw.Hit(chip, 150, 4, 32, 32, this);
			}
			else
			{
				ct = INT_VPPX_Look.Text(chip, 120, 10, 56, 20, " ", 13, INT_VPP_Ui.M, INT_UIF_Align.RIGHT, false);
			}
			INT_VPPX_Check ck = new INT_VPPX_Check();
			ck.Build(chip, 186, 12, 16, true);
			ck.OnChanged().Insert(INT_OnCheck);
			m_INT_ChipBg.Insert(bg);
			m_INT_ChipDia.Insert(dia);
			m_INT_ChipName.Insert(nt);
			m_INT_ChipCount.Insert(ct);
			m_INT_ChipCheck.Insert(ck);
			m_INT_ChipHit.Insert(hit);
		}
		INT_BuildMiniRow();
		// «Вернуть панели на место» (2026-11-14) in the header row between «СЛОИ» and the count
		float rbw = 230;
		float rbx = 110;
		INT_VPPX_Look.Fill(m_INT_Layers, rbx, 8, rbw, 24, INT_VPP_Ui.BTN_BG);
		INT_VPPX_Look.Outline(m_INT_Layers, rbx, 8, rbw, 24, INT_VPP_Ui.LINE2);
		string rbt = INT_VPP_Ui.INT_Tr("STR_INT_VPP_PANELS_RESET");
		string rbc = INT_VPPX_Text.Caps(rbt);
		INT_VPPX_Look.Text(m_INT_Layers, rbx, 8, rbw, 24, rbc, 10, INT_VPP_Ui.T2, INT_UIF_Align.CENTER, true);
		m_INT_PanelsReset = INT_UIF_Draw.Hit(m_INT_Layers, rbx, 8, rbw, 24, this);
		INT_VPP_Ui.INT_Sort(m_INT_Layers);
		INT_RegisterDrag(INT_PAN_LAYERS, m_INT_Layers, INT_HandleOf(m_INT_Layers));
	}

	// the wide row «Миникарта» (mm_settings_1.png): hollow diamond, name, the show / hide key, gear, check
	protected void INT_BuildMiniRow()
	{
		float cy = 42 + 3 * 46;
		float cw = 436;
		Widget chip = INT_UIF_Draw.Frame(m_INT_Layers, 18, cy, cw, 40, "IntVppChipMini");
		if (!chip)
			return;
		m_INT_MiniBg = INT_VPPX_Look.Fill(chip, 0, 0, cw, 40, INT_VPP_Ui.CHIP_BG);
		INT_VPPX_Look.Outline(chip, 0, 0, cw, 40, INT_VPP_Ui.CHIP_EDGE);
		m_INT_MiniHit = INT_UIF_Draw.Hit(chip, 0, 0, cw, 40, this);
		m_INT_MiniDia = INT_VPPX_Look.Diamond(chip, 19, 20, 10, false, INT_VPPX_Look.ORANGE);
		m_INT_MiniName = INT_VPPX_Look.Text(chip, 38, 9, 200, 22, TXT_MINI, 17, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		// the show / hide key as keycaps like the Миникарта panel (2026-11-14: CTRL + NUM /), right edge at 362
		array<string> mkeys = new array<string>();
		INT_VPP_MiniPanel.INT_ComboNames("UAIntVppMiniMap", mkeys);
		if (mkeys.Count() == 0)
		{
			mkeys.Insert("Ctrl");
			mkeys.Insert("Num /");
		}
		float kx = 362;
		for (int ki = mkeys.Count() - 1; ki >= 0; ki--)
		{
			string kn = mkeys.Get(ki);
			string ku = INT_VPPX_Text.Upper(kn);
			float kw = INT_VPPX_Look.Measure(ku, 13, true) + 18;
			if (kw < 30)
				kw = 30;
			kx = kx - kw;
			INT_VPPX_Look.Keycap(chip, kx, 9, kn, 22, INT_VPP_Ui.KBD_EDGE, INT_VPPX_Look.TEXT);
			kx = kx - 4;
		}
		m_INT_MiniGearBox = INT_UIF_Draw.Frame(chip, 372, 6, 30, 28, "IntVppMiniGearBox");
		if (m_INT_MiniGearBox)
		{
			INT_VPPX_Look.Outline(m_INT_MiniGearBox, 0, 0, 30, 28, 0x99FF7A2F);
			m_INT_MiniGearBox.Show(false);
		}
		m_INT_MiniGear = INT_VPPX_Look.IconAt(chip, 379, 12, 16, "gear", INT_VPP_Ui.M);
		m_INT_MiniGearHit = INT_UIF_Draw.Hit(chip, 370, 4, 34, 32, this);
		INT_VPPX_Check ck = new INT_VPPX_Check();
		ck.Build(chip, cw - 28, 12, 16, true);
		ck.OnChanged().Insert(INT_OnMiniCheck);
		m_INT_MiniCheck = ck;
		INT_RepaintMiniRow();
	}

	// on / off of the row from the prefs (no notify)
	void INT_RepaintMiniRow()
	{
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		bool on = true;
		if (prefs)
			on = prefs.INT_MiniIsOn();
		m_INT_Painting = true;
		if (m_INT_MiniCheck)
			m_INT_MiniCheck.SetOn(on, false);
		m_INT_Painting = false;
		if (m_INT_MiniBg)
		{
			int bgc = INT_VPP_Ui.ISL_BG;
			if (on)
				bgc = INT_VPP_Ui.CHIP_BG;
			m_INT_MiniBg.SetColor(bgc);
		}
		if (m_INT_MiniDia)
		{
			int dc = INT_VPP_Ui.CHIP_OFF_DIA;
			if (on)
				dc = INT_VPPX_Look.ORANGE;
			m_INT_MiniDia.SetColor(dc);
		}
		if (m_INT_MiniName)
		{
			int tc = INT_VPP_Ui.CHIP_OFF_TEXT;
			if (on)
				tc = INT_VPPX_Look.WHITE;
			m_INT_MiniName.SetColor(tc);
		}
		bool open = false;
		if (m_INT_MiniPanel)
			open = true;
		if (m_INT_MiniGear)
		{
			int gc = INT_VPP_Ui.M;
			if (open)
				gc = INT_VPPX_Look.ORANGE;
			m_INT_MiniGear.SetColor(gc);
		}
		if (m_INT_MiniGearBox && m_INT_MiniGearBox.IsVisible() != open)
			m_INT_MiniGearBox.Show(open);
		if (m_INT_MiniPanel)
			m_INT_MiniPanel.INT_Paint();
	}

	void INT_OnMiniCheck(bool on, INT_VPPX_Check c)
	{
		if (m_INT_Painting)
			return;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return;
		prefs.INT_MiniSetOn(on);
		prefs.INT_Save();
		INT_RepaintMiniRow();
	}

	// ------------------------------------------------------------------ minimap settings panel
	bool INT_IsMiniPanelOpen()
	{
		if (m_INT_MiniPanel)
			return true;
		return false;
	}

	INT_VPP_MiniPanel INT_GetMiniPanel()
	{
		return m_INT_MiniPanel;
	}

	void INT_OpenMiniPanel()
	{
		if (m_INT_MiniPanel)
			return;
		// one popover at a time: the 3D one goes
		INT_Close3DPop();
		INT_VPP_MiniPanel mp = new INT_VPP_MiniPanel();
		m_INT_MiniPanel = mp;
		float bottom = m_INT_H - INT_VPP_Ui.INT_LAYERS_TOP;
		mp.INT_Build(this, m_INT_Screen, m_INT_W, m_INT_H, bottom);
		INT_RepaintMiniRow();
	}

	void INT_CloseMiniPanel()
	{
		INT_VPP_MiniPanel mp = m_INT_MiniPanel;
		if (!mp)
			return;
		m_INT_MiniPanel = null;
		mp.INT_Destroy();
		INT_RepaintMiniRow();
	}

	// states from the prefs, counts from the menu (no notify); tempHidden = K hides all 3D for now
	void INT_PaintLayers(array<int> counts, bool tempHidden)
	{
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		int lit = 0;
		m_INT_Painting = true;
		for (int i = 0; i < m_INT_ChipCheck.Count(); i++)
		{
			bool on = true;
			if (prefs)
			{
				if (i == LAYER_3D)
					on = prefs.INT_Is3DOn();
				else
					on = prefs.INT_IsLayerOn(i);
			}
			INT_VPPX_Check ck = m_INT_ChipCheck.Get(i);
			if (ck)
				ck.SetOn(on, false);
			// the «3D» cell: switched on but hidden by K reads as off, with a note
			if (i == LAYER_3D)
			{
				TextWidget n3 = m_INT_ChipName.Get(i);
				string t3 = TXT_3D;
				if (on && tempHidden)
				{
					t3 = TXT_3D_K;
					on = false;
				}
				if (n3)
					n3.SetText(t3);
			}
			if (on)
				lit++;
			ImageWidget bg = m_INT_ChipBg.Get(i);
			if (bg)
			{
				int bgc = INT_VPP_Ui.ISL_BG;
				if (on)
					bgc = INT_VPP_Ui.CHIP_BG;
				bg.SetColor(bgc);
			}
			ImageWidget dia = m_INT_ChipDia.Get(i);
			if (dia)
			{
				int dc = INT_VPP_Ui.CHIP_OFF_DIA;
				if (on)
					dc = m_INT_ChipColor.Get(i);
				dia.SetColor(dc);
			}
			TextWidget nt = m_INT_ChipName.Get(i);
			if (nt)
			{
				int tc = INT_VPP_Ui.CHIP_OFF_TEXT;
				if (on)
					tc = INT_VPPX_Look.WHITE;
				nt.SetColor(tc);
			}
			TextWidget ct = m_INT_ChipCount.Get(i);
			if (ct && counts && i < counts.Count())
			{
				int cnt = counts.Get(i);
				string cs = INT_VPPX_Text.Pad2(cnt);
				ct.SetText(cs);
			}
		}
		m_INT_Painting = false;
		if (m_INT_LayersCount)
		{
			string tot = lit.ToString() + " " + TXT_OF + " 6";
			string capsTot = INT_VPPX_Text.Caps(tot);
			m_INT_LayersCount.SetText(capsTot);
		}
		INT_PaintPop();
	}

	void INT_OnCheck(bool on, INT_VPPX_Check c)
	{
		if (m_INT_Painting || !m_INT_Menu)
			return;
		int idx = m_INT_ChipCheck.Find(c);
		if (idx < 0)
			return;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
		if (idx == LAYER_3D)
			m_INT_Menu.INT_Set3DMaster(on);
		else
			m_INT_Menu.INT_ToggleLayer(idx, on);
	}

	// ------------------------------------------------------------------ 3D distance popover
	// the 3D popover OR the minimap panel (Esc, a click on the map, a confirm band close either)
	bool INT_IsPopOpen()
	{
		if (m_INT_Pop)
			return true;
		if (m_INT_MiniPanel)
			return true;
		return false;
	}

	void INT_ClosePop()
	{
		INT_CloseMiniPanel();
		INT_Close3DPop();
	}

	protected void INT_Close3DPop()
	{
		if (!m_INT_Pop)
			return;
		m_INT_Pop.Show(false);
		if (m_INT_DeadPop)
			m_INT_DeadPop.Unlink();
		m_INT_DeadPop = m_INT_Pop;
		m_INT_Pop = null;
		m_INT_PopMinus.Clear();
		m_INT_PopPlus.Clear();
		m_INT_PopVal.Clear();
		m_INT_PopLayer.Clear();
		// the preview tree is a child of the popover: it goes with it
		INT_ForgetLook();
		if (m_INT_Gear)
			m_INT_Gear.SetColor(INT_VPP_Ui.M);
	}

	protected void INT_ForgetLook()
	{
		m_INT_LookMinus.Clear();
		m_INT_LookPlus.Clear();
		m_INT_LookVal.Clear();
		m_INT_LookReset = null;
		m_INT_LookPrev = null;
		m_INT_LookPrevScale = -1;
		m_INT_LookPrevFade = -1;
		m_INT_LookPrevTexts = -1;
		if (m_INT_LookNames)
			m_INT_LookNames.OnChanged().Clear();
		m_INT_LookNames = null;
		if (m_INT_LookDist)
			m_INT_LookDist.OnChanged().Clear();
		m_INT_LookDist = null;
	}

	// TEST: the preview tree of the open popover (null when closed)
	Widget INT_GetLookPreview()
	{
		return m_INT_LookPrev;
	}

	protected void INT_OpenPop()
	{
		if (m_INT_Pop)
			return;
		INT_CloseMiniPanel();
		m_INT_PopMinus.Clear();
		m_INT_PopPlus.Clear();
		m_INT_PopVal.Clear();
		m_INT_PopLayer.Clear();
		m_INT_PopLayer.Insert(INT_VPP_LAYER_SERVER);
		m_INT_PopLayer.Insert(INT_VPP_LAYER_MINE);
		m_INT_PopLayer.Insert(INT_VPP_LAYER_EVENTS);
		m_INT_PopLayer.Insert(INT_VPP_LAYER_ROUTE);
		array<string> names = new array<string>();
		names.Insert("Сервер");
		names.Insert("Мои");
		names.Insert("События");
		names.Insert("Маршрут");
		INT_ForgetLook();
		// distances (4 rows + the cap note), then «ВИД 3D-МЕТОК» (2 steppers + preview over a neutral swatch)
		float ph = LOOK_SWATCH_Y + LOOK_SWATCH_H + 16;
		float py = m_INT_H - INT_VPP_Ui.INT_LAYERS_TOP - 8 - ph;
		m_INT_Pop = INT_VPP_Ui.INT_Panel(m_INT_Screen, 24, py, POP_W, ph, INT_VPP_Ui.CARD_BG, INT_VPP_Ui.CARD_EDGE, true, "IntVppPop");
		if (!m_INT_Pop)
			return;
		INT_Blocker(m_INT_Pop, POP_W, ph);
		string title = INT_VPPX_Text.Caps(TXT_3D_TITLE);
		INT_VPPX_Look.Text(m_INT_Pop, 18, 14, 400, 20, title, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		for (int i = 0; i < 4; i++)
		{
			float ry = 52 + i * 44;
			string nm = names.Get(i);
			INT_VPPX_Look.Text(m_INT_Pop, 18, ry, 200, 32, nm, 17, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
			float bx = 476 - 18 - 32 - 110 - 32;
			INT_VPPX_Look.Fill(m_INT_Pop, bx, ry, 32, 32, INT_VPP_Ui.BTN_BG);
			INT_VPPX_Look.Outline(m_INT_Pop, bx, ry, 32, 32, INT_VPP_Ui.LINE2);
			INT_VPPX_Look.Fill(m_INT_Pop, bx + 10, ry + 15, 12, 2, INT_VPPX_Look.TEXT);
			ButtonWidget mh = INT_UIF_Draw.Hit(m_INT_Pop, bx, ry, 32, 32, this);
			TextWidget vt = INT_VPPX_Look.Text(m_INT_Pop, bx + 32, ry, 110, 32, " ", 15, INT_VPPX_Look.WHITE, INT_UIF_Align.CENTER, false);
			float px2 = bx + 32 + 110;
			INT_VPPX_Look.Fill(m_INT_Pop, px2, ry, 32, 32, INT_VPP_Ui.BTN_BG);
			INT_VPPX_Look.Outline(m_INT_Pop, px2, ry, 32, 32, INT_VPP_Ui.LINE2);
			INT_VPPX_Look.Fill(m_INT_Pop, px2 + 10, ry + 15, 12, 2, INT_VPPX_Look.TEXT);
			INT_VPPX_Look.Fill(m_INT_Pop, px2 + 15, ry + 10, 2, 12, INT_VPPX_Look.TEXT);
			ButtonWidget ph2 = INT_UIF_Draw.Hit(m_INT_Pop, px2, ry, 32, 32, this);
			m_INT_PopMinus.Insert(mh);
			m_INT_PopPlus.Insert(ph2);
			m_INT_PopVal.Insert(vt);
		}
		string note = TXT_3D_NOTE;
		float noteY = 52 + 4 * 44;
		INT_VPPX_Look.Text(m_INT_Pop, 18, noteY, 440, 20, note, 12, INT_VPP_Ui.D, INT_UIF_Align.LEFT, false);
		INT_BuildLookSection();
		INT_VPP_Ui.INT_Sort(m_INT_Pop);
		INT_RegisterDrag(INT_PAN_POP3D, m_INT_Pop, INT_HandleOf(m_INT_Pop));
		if (m_INT_Gear)
			m_INT_Gear.SetColor(INT_VPPX_Look.ORANGE);
		INT_PaintPop();
	}

	// «ВИД 3D-МЕТОК»: «Размер» 50–200 %, «Прозрачность» 0–80 % (steppers like the distances), «Названия» /
	// «Расстояние» switches (2026-11-14: name / metres text next to every 3D marker), «Сбросить», and a live preview
	// of one marker over a neutral swatch (the world's 3D markers are suspended while the map is open)
	protected void INT_BuildLookSection()
	{
		if (!m_INT_Pop)
			return;
		INT_VPPX_Look.Fill(m_INT_Pop, 18, LOOK_TOP, POP_W - 36, 1, INT_VPP_Ui.LINE2);
		string title = INT_VPPX_Text.Caps(TXT_LOOK_TITLE);
		INT_VPPX_Look.Text(m_INT_Pop, 18, LOOK_TOP + 16, 300, 20, title, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		float rw = 120;
		float rx = POP_W - 18 - rw;
		float ry0 = LOOK_TOP + 10;
		INT_VPPX_Look.Fill(m_INT_Pop, rx, ry0, rw, 28, INT_VPP_Ui.BTN_BG);
		INT_VPPX_Look.Outline(m_INT_Pop, rx, ry0, rw, 28, INT_VPP_Ui.LINE2);
		string resetCaps = INT_VPPX_Text.Caps(TXT_LOOK_RESET);
		INT_VPPX_Look.Text(m_INT_Pop, rx, ry0, rw, 28, resetCaps, 12, INT_VPP_Ui.T2, INT_UIF_Align.CENTER, true);
		m_INT_LookReset = INT_UIF_Draw.Hit(m_INT_Pop, rx, ry0, rw, 28, this);
		array<string> names = new array<string>();
		names.Insert(TXT_LOOK_SIZE);
		names.Insert(TXT_LOOK_FADE);
		for (int i = 0; i < 2; i++)
		{
			float ry = LOOK_TOP + 52 + i * 44;
			string nm = names.Get(i);
			INT_VPPX_Look.Text(m_INT_Pop, 18, ry, 200, 32, nm, 17, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
			float bx = POP_W - 18 - 32 - 110 - 32;
			INT_VPPX_Look.Fill(m_INT_Pop, bx, ry, 32, 32, INT_VPP_Ui.BTN_BG);
			INT_VPPX_Look.Outline(m_INT_Pop, bx, ry, 32, 32, INT_VPP_Ui.LINE2);
			INT_VPPX_Look.Fill(m_INT_Pop, bx + 10, ry + 15, 12, 2, INT_VPPX_Look.TEXT);
			ButtonWidget mh = INT_UIF_Draw.Hit(m_INT_Pop, bx, ry, 32, 32, this);
			TextWidget vt = INT_VPPX_Look.Text(m_INT_Pop, bx + 32, ry, 110, 32, " ", 15, INT_VPPX_Look.WHITE, INT_UIF_Align.CENTER, false);
			float px2 = bx + 32 + 110;
			INT_VPPX_Look.Fill(m_INT_Pop, px2, ry, 32, 32, INT_VPP_Ui.BTN_BG);
			INT_VPPX_Look.Outline(m_INT_Pop, px2, ry, 32, 32, INT_VPP_Ui.LINE2);
			INT_VPPX_Look.Fill(m_INT_Pop, px2 + 10, ry + 15, 12, 2, INT_VPPX_Look.TEXT);
			INT_VPPX_Look.Fill(m_INT_Pop, px2 + 15, ry + 10, 2, 12, INT_VPPX_Look.TEXT);
			ButtonWidget ph2 = INT_UIF_Draw.Hit(m_INT_Pop, px2, ry, 32, 32, this);
			m_INT_LookMinus.Insert(mh);
			m_INT_LookPlus.Insert(ph2);
			m_INT_LookVal.Insert(vt);
		}
		// the two text switches (the minimap «ВКЛ» style)
		INT_VPP_Prefs lp = INT_VPP_Prefs.INT_Get();
		bool namesOn = true;
		bool distOn = true;
		if (lp)
		{
			namesOn = lp.INT_Is3DNamesOn();
			distOn = lp.INT_Is3DDistOn();
		}
		float ty0 = LOOK_TOP + 52 + 2 * 44;
		string namesTxt = INT_VPP_Ui.INT_Tr("STR_INT_VPP_LOOK_NAMES");
		string onTxt = INT_VPP_Ui.INT_Tr("STR_INT_VPP_ON");
		INT_VPPX_Look.Text(m_INT_Pop, 18, ty0, 240, 32, namesTxt, 17, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		m_INT_LookNames = new INT_VPPX_Toggle();
		m_INT_LookNames.Build(m_INT_Pop, POP_W - 18 - 120, ty0 + 4, 120, onTxt, namesOn);
		m_INT_LookNames.OnChanged().Insert(INT_OnLookNames);
		float ty1 = ty0 + 44;
		string distTxt = INT_VPP_Ui.INT_Tr("STR_INT_VPP_LOOK_DIST");
		INT_VPPX_Look.Text(m_INT_Pop, 18, ty1, 240, 32, distTxt, 17, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		m_INT_LookDist = new INT_VPPX_Toggle();
		m_INT_LookDist.Build(m_INT_Pop, POP_W - 18 - 120, ty1 + 4, 120, onTxt, distOn);
		m_INT_LookDist.OnChanged().Insert(INT_OnLookDist);
		INT_VPPX_Look.Fill(m_INT_Pop, 18, LOOK_SWATCH_Y, POP_W - 36, LOOK_SWATCH_H, LOOK_SWATCH);
		INT_VPPX_Look.Outline(m_INT_Pop, 18, LOOK_SWATCH_Y, POP_W - 36, LOOK_SWATCH_H, INT_VPP_Ui.LINE2);
	}

	// the preview at the current size / transparency / texts; rebuilt only when one changed (dropped with the popover)
	protected void INT_PaintLookPreview(int scalePct, int fadePct)
	{
		if (!m_INT_Pop)
			return;
		bool showName = true;
		bool showDist = true;
		INT_VPP_Prefs pp = INT_VPP_Prefs.INT_Get();
		if (pp)
		{
			showName = pp.INT_Is3DNamesOn();
			showDist = pp.INT_Is3DDistOn();
		}
		int texts = 0;
		if (showName)
			texts = texts + 1;
		if (showDist)
			texts = texts + 2;
		if (m_INT_LookPrev && scalePct == m_INT_LookPrevScale && fadePct == m_INT_LookPrevFade && texts == m_INT_LookPrevTexts)
			return;
		if (m_INT_LookPrev)
			m_INT_LookPrev.Unlink();
		m_INT_LookPrev = null;
		float k = scalePct;
		k = k / 100.0;
		float fade = fadePct;
		float alphaK = 1.0 - fade / 100.0;
		float x = 18 + 14;
		float y = LOOK_SWATCH_Y + (LOOK_SWATCH_H - VPP3DMarker.INT_ROOT_H * k) * 0.5;
		Widget prev = VPP3DMarker.INT_DrawLook(m_INT_Pop, x, y, k, alphaK, INT_VPP_LAYER_SERVER, "", 0, TXT_LOOK_SAMPLE, TXT_LOOK_SAMPLE_DIST, "IntVppLookPreview", showName, showDist);
		VPP3DMarker.s_INT_OutName = null;
		VPP3DMarker.s_INT_OutDist = null;
		if (!prev)
			return;
		// over the swatch (own sort 300 like the popover; SetSort moves it after its siblings)
		INT_VPP_Ui.INT_Sort(prev);
		m_INT_LookPrev = prev;
		m_INT_LookPrevScale = scalePct;
		m_INT_LookPrevFade = fadePct;
		m_INT_LookPrevTexts = texts;
	}

	// «Названия» / «Расстояние» switched (2026-11-14)
	void INT_OnLookNames(bool on, INT_VPPX_Toggle t)
	{
		if (m_INT_LookPainting || !m_INT_Menu)
			return;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
		bool dist = true;
		if (m_INT_LookDist)
			dist = m_INT_LookDist.IsOn();
		m_INT_Menu.INT_Set3DTexts(on, dist);
		INT_PaintPop();
	}

	void INT_OnLookDist(bool on, INT_VPPX_Toggle t)
	{
		if (m_INT_LookPainting || !m_INT_Menu)
			return;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
		bool names = true;
		if (m_INT_LookNames)
			names = m_INT_LookNames.IsOn();
		m_INT_Menu.INT_Set3DTexts(names, on);
		INT_PaintPop();
	}

	// TEST: the switches' path
	void INT_TestLookTexts(bool names, bool dist)
	{
		if (m_INT_LookNames)
			m_INT_LookNames.SetOn(names, true, false);
		if (m_INT_LookDist)
			m_INT_LookDist.SetOn(dist, true, false);
	}

	// row 0 = size, row 1 = transparency; one step of 10 %
	protected void INT_LookStep(int row, int dir)
	{
		if (!m_INT_Menu || row < 0 || row > 1)
			return;
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return;
		int sc = prefs.INT_Get3DScalePct();
		int fd = prefs.INT_Get3DFadePct();
		int step = INT_VPP_Prefs.INT_LOOK_STEP * dir;
		if (row == 0)
			sc = INT_VPP_Prefs.INT_ClampScalePct(sc + step);
		else
			fd = INT_VPP_Prefs.INT_ClampFadePct(fd + step);
		bool moved = false;
		if (sc != prefs.INT_Get3DScalePct() || fd != prefs.INT_Get3DFadePct())
			moved = true;
		// the end of the range: nothing moves
		if (moved)
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
		else
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
		m_INT_Menu.INT_Set3DLook(sc, fd);
		INT_PaintPop();
	}

	protected void INT_LookReset()
	{
		if (!m_INT_Menu)
			return;
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return;
		bool moved = false;
		if (prefs.INT_Get3DScalePct() != INT_VPP_Prefs.INT_SCALE_DEF || prefs.INT_Get3DFadePct() != INT_VPP_Prefs.INT_FADE_DEF)
			moved = true;
		if (!prefs.INT_Is3DNamesOn() || !prefs.INT_Is3DDistOn())
			moved = true;
		if (moved)
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
		else
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
		// size, transparency, names and metres (2026-11-14) back to the defaults
		m_INT_Menu.INT_Reset3DLook();
		INT_PaintPop();
	}

	// TEST: the same path as the stepper / reset buttons
	void INT_TestLookStep(int row, int dir)
	{
		INT_LookStep(row, dir);
	}

	void INT_TestLookReset()
	{
		INT_LookReset();
	}

	protected void INT_PaintPop()
	{
		if (!m_INT_Pop)
			return;
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		for (int i = 0; i < m_INT_PopVal.Count(); i++)
		{
			TextWidget vt = m_INT_PopVal.Get(i);
			if (!vt)
				continue;
			int layer = m_INT_PopLayer.Get(i);
			float m = 0;
			if (prefs)
				m = prefs.INT_Dist3D(layer);
			string s = TXT_OFF;
			if (m >= 1)
				s = INT_VPP_Fmt.INT_Dist(m);
			vt.SetText(s);
		}
		int sc = INT_VPP_Prefs.INT_SCALE_DEF;
		int fd = INT_VPP_Prefs.INT_FADE_DEF;
		if (prefs)
		{
			sc = prefs.INT_Get3DScalePct();
			fd = prefs.INT_Get3DFadePct();
		}
		if (m_INT_LookVal.Count() >= 2)
		{
			TextWidget sv = m_INT_LookVal.Get(0);
			if (sv)
				sv.SetText(sc.ToString() + " %");
			TextWidget fv = m_INT_LookVal.Get(1);
			if (fv)
				fv.SetText(fd.ToString() + " %");
		}
		// the switches follow the prefs (a reset turns them back on) without calling their handlers
		if (prefs)
		{
			m_INT_LookPainting = true;
			bool nOn = prefs.INT_Is3DNamesOn();
			bool dOn = prefs.INT_Is3DDistOn();
			if (m_INT_LookNames)
				m_INT_LookNames.SetOn(nOn, false);
			if (m_INT_LookDist)
				m_INT_LookDist.SetOn(dOn, false);
			m_INT_LookPainting = false;
		}
		INT_PaintLookPreview(sc, fd);
	}

	// the server cap of a layer's 3D distance (HUD3D: INT_VPP_Prefs.INT_Cap3D)
	static float INT_Cap3D(int layer)
	{
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return 0;
		return prefs.INT_Cap3D(layer);
	}

	// next / previous step of the distance ladder, capped by the server
	static float INT_StepDist(float cur, int dir, float cap)
	{
		array<int> ladder = {0, 100, 250, 500, 750, 1000, 1500, 2000, 3000, 4000, 5000, 7500, 10000, 15000, 20000};
		int n = ladder.Count();
		int at = 0;
		for (int i = 0; i < n; i++)
		{
			float li = ladder.Get(i);
			if (li <= cur + 0.5)
				at = i;
		}
		int next = at + dir;
		if (next < 0)
			next = 0;
		if (next >= n)
			next = n - 1;
		float v = ladder.Get(next);
		if (cap > 0 && v > cap)
			v = cap;
		if (cap <= 0)
			v = 0;
		return v;
	}

	protected void INT_PopStep(int row, int dir)
	{
		if (!m_INT_Menu || row < 0 || row >= m_INT_PopLayer.Count())
			return;
		int layer = m_INT_PopLayer.Get(row);
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return;
		float cur = prefs.INT_Dist3D(layer);
		float cap = INT_VPP_Islands.INT_Cap3D(layer);
		float v = INT_VPP_Islands.INT_StepDist(cur, dir, cap);
		// the end of the ladder or the server cap: nothing moves
		float moved = Math.AbsFloat(v - cur);
		if (moved < 0.5)
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
		else
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
		m_INT_Menu.INT_SetDist3D(layer, v);
		INT_PaintPop();
	}

	// ------------------------------------------------------------------ keys strip
	void INT_SetKeys(int mode)
	{
		if (mode == m_INT_KeysMode)
			return;
		m_INT_KeysMode = mode;
		if (m_INT_Keys)
			m_INT_Keys.Unlink();
		m_INT_Keys = null;
		if (mode == KEYS_NONE)
			return;
		array<string> keys = new array<string>();
		array<string> labels = new array<string>();
		if (mode == KEYS_EDITOR)
		{
			keys.Insert("ENTER");
			labels.Insert("Сохранить");
			keys.Insert("ESC");
			labels.Insert("Отмена");
			keys.Insert("ЛКМ");
			labels.Insert("Выбрать значок");
		}
		else if (mode == KEYS_RULER)
		{
			keys.Insert("ЛКМ");
			labels.Insert("Точка линейки");
			keys.Insert("ПКМ");
			labels.Insert("Убрать последнюю");
			keys.Insert("R");
			labels.Insert("Выйти");
		}
		else if (mode == KEYS_ZONES)
		{
			keys.Insert("ЛКМ");
			labels.Insert("Точка зоны");
			keys.Insert("ПКМ");
			labels.Insert("Убрать точку");
			keys.Insert("ESC");
			labels.Insert("Назад");
		}
		else if (mode == INT_KEYS_LABELS)
		{
			keys.Insert("ЛКМ");
			labels.Insert("Место надписи");
			keys.Insert("ENTER");
			labels.Insert("Сохранить");
			keys.Insert("ESC");
			labels.Insert("Назад");
		}
		else
		{
			keys.Insert("2×ЛКМ");
			labels.Insert("Новая метка");
			keys.Insert("ПКМ");
			labels.Insert("Точка маршрута");
			keys.Insert("КОЛЕСО");
			labels.Insert("Масштаб");
			keys.Insert("ПРОБЕЛ");
			labels.Insert("К себе");
			keys.Insert("R");
			labels.Insert("Линейка");
		}
		// widths first: the strip hugs its keys, between the layers island and the coordinates island (measured
		// 2026-11-12: five tracked keys ran ~100 px under the coordinates on every 16:9 screen). Short of room:
		// plain upper-case labels and a tighter gap, then the last keys go.
		float room = m_INT_W - 24 - 440 - 16 - 524;
		bool tracked = true;
		float gap = 22;
		float total = INT_KeysWidth(keys, labels, tracked, gap);
		if (total > room)
		{
			tracked = false;
			gap = 16;
			total = INT_KeysWidth(keys, labels, tracked, gap);
		}
		while (total > room && keys.Count() > 1)
		{
			int lastKey = keys.Count() - 1;
			keys.Remove(lastKey);
			labels.Remove(lastKey);
			total = INT_KeysWidth(keys, labels, tracked, gap);
		}
		int n = keys.Count();
		float y0 = m_INT_H - 68;
		m_INT_Keys = INT_VPP_Ui.INT_Panel(m_INT_Screen, 524, y0, total, 44, INT_VPP_Ui.ISL_BG, INT_VPP_Ui.ISL_EDGE, false, "IntVppKeys");
		if (!m_INT_Keys)
			return;
		INT_Blocker(m_INT_Keys, total, 44);
		float x = 18;
		for (int k = 0; k < n; k++)
		{
			string kk2 = keys.Get(k);
			string kl2 = labels.Get(k);
			float used = INT_VPP_Ui.INT_Key(m_INT_Keys, x, 9, kk2, kl2, tracked);
			x = x + used + gap;
		}
		INT_VPP_Ui.INT_Sort(m_INT_Keys);
		INT_RegisterDrag(INT_PAN_KEYS, m_INT_Keys, INT_HandleOf(m_INT_Keys));
	}

	protected float INT_KeysWidth(array<string> keys, array<string> labels, bool tracked, float gap)
	{
		float total = 18;
		int n = keys.Count();
		for (int i = 0; i < n; i++)
		{
			string kk = keys.Get(i);
			string kl = labels.Get(i);
			float one = INT_VPP_Ui.INT_KeyWidth(kk, kl, tracked);
			total = total + one + gap;
		}
		total = total - gap + 18;
		return total;
	}

	// ------------------------------------------------------------------ coordinates
	protected void INT_BuildCoords()
	{
		float x0 = m_INT_W - 24 - 440;
		float y0 = m_INT_H - 24 - INT_COORDS_H;
		m_INT_Coords = INT_VPP_Ui.INT_Panel(m_INT_Screen, x0, y0, 440, INT_COORDS_H, INT_VPP_Ui.ISL_BG, INT_VPP_Ui.ISL_EDGE, false, "IntVppCoords");
		if (!m_INT_Coords)
			return;
		INT_Blocker(m_INT_Coords, 440, INT_COORDS_H);
		string me = INT_VPPX_Text.Caps(TXT_ME);
		INT_VPPX_Look.Text(m_INT_Coords, 20, 14, 30, 20, me, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		m_INT_MeText = INT_VPPX_Look.Text(m_INT_Coords, 52, 4, 260, 40, "—", 28, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		m_INT_SqText = INT_VPPX_Look.Text(m_INT_Coords, 440 - 20 - 110, 10, 110, 28, " ", 20, INT_VPP_Ui.T2, INT_UIF_Align.RIGHT, false);
		string sq = INT_VPPX_Text.Caps(TXT_SQUARE);
		m_INT_SqCap = INT_VPPX_Look.Text(m_INT_Coords, 440 - 20 - 110 - 60, 14, 52, 20, sq, 11, INT_VPP_Ui.M, INT_UIF_Align.RIGHT, true);
		INT_VPPX_Look.Fill(m_INT_Coords, 20, 52, 400, 1, INT_VPPX_Look.LINE);
		string cur = INT_VPPX_Text.Caps(TXT_CURSOR);
		float cw = INT_VPPX_Look.Measure(cur, 11, true);
		INT_VPPX_Look.Text(m_INT_Coords, 20, 64, cw + 20, 22, cur, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		m_INT_CurText = INT_VPPX_Look.Text(m_INT_Coords, 20 + cw + 14, 64, 400 - cw - 14, 22, " ", 14, INT_VPP_Ui.T2, INT_UIF_Align.LEFT, false);
		// Ctrl + C copies the point under the cursor (2026-11-14): key chip + hint, «Скопировано» after a copy
		float kw = INT_VPPX_Look.Keycap(m_INT_Coords, 20, 98, "Ctrl+C", 22, INT_VPP_Ui.KBD_EDGE, INT_VPPX_Look.TEXT);
		string hint = INT_VPP_Ui.INT_Tr("STR_INT_VPP_COPY_HINT");
		m_INT_CopyText = INT_VPPX_Look.Text(m_INT_Coords, 20 + kw + 10, 99, 400 - kw - 10, 20, hint, 12, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		m_INT_CopyFlash = 0;
		INT_VPP_Ui.INT_Sort(m_INT_Coords);
		INT_RegisterDrag(INT_PAN_COORDS, m_INT_Coords, INT_HandleOf(m_INT_Coords));
	}

	void INT_SetMeCoords(bool shown, string coords, string square)
	{
		string c = "—";
		string s = " ";
		if (shown)
		{
			c = coords;
			s = square;
		}
		if (c != m_INT_LastMe && m_INT_MeText)
		{
			m_INT_LastMe = c;
			m_INT_MeText.SetText(c);
		}
		if (s != m_INT_LastSq && m_INT_SqText)
		{
			m_INT_LastSq = s;
			m_INT_SqText.SetText(s);
			if (m_INT_SqCap)
				m_INT_SqCap.Show(shown);
		}
	}

	// a copy went to the clipboard (Ctrl + C on the map): «Скопировано · X: … Y: …» in orange for a second
	void INT_FlashCopied(string copied)
	{
		if (!m_INT_CopyText)
			return;
		string done = INT_VPP_Ui.INT_Tr("STR_INT_VPP_COPIED");
		m_INT_CopyText.SetText(done + "   ·   " + copied);
		INT_UIF_Animator.ColorTo(m_INT_CopyText, INT_VPPX_Look.ORANGE, 0.12, INT_UIF_Ease.OUT_CUBIC);
		m_INT_CopyFlash = INT_COPY_FLASH;
	}

	protected void INT_CopyFlashTick(float dt)
	{
		if (m_INT_CopyFlash <= 0)
			return;
		m_INT_CopyFlash = m_INT_CopyFlash - dt;
		if (m_INT_CopyFlash > 0 || !m_INT_CopyText)
			return;
		m_INT_CopyFlash = 0;
		string hint = INT_VPP_Ui.INT_Tr("STR_INT_VPP_COPY_HINT");
		m_INT_CopyText.SetText(hint);
		INT_UIF_Animator.ColorTo(m_INT_CopyText, INT_VPP_Ui.M, 0.3, INT_UIF_Ease.OUT_CUBIC);
	}

	void INT_SetCursorText(string t)
	{
		if (t == m_INT_LastCur || !m_INT_CurText)
			return;
		m_INT_LastCur = t;
		m_INT_CurText.SetText(t);
	}

	// ------------------------------------------------------------------ list: folded tab
	protected void INT_BuildDrawer()
	{
		float x0 = m_INT_W - 46;
		m_INT_Drawer = INT_VPP_Ui.INT_Panel(m_INT_Screen, x0, LIST_Y, 50, 210, INT_VPP_Ui.ISL_BG, INT_VPP_Ui.ISL_EDGE, false, "IntVppDrawer");
		if (!m_INT_Drawer)
			return;
		m_INT_DrawerHit = INT_UIF_Draw.Hit(m_INT_Drawer, 0, 0, 46, 210, this);
		ImageWidget chev = INT_VPPX_Look.IconAt(m_INT_Drawer, 14, 16, 18, "chevron", INT_VPP_Ui.M);
		if (chev)
			chev.SetRotation(0, 0, 180);
		string listIcon = INT_VPP_Ui.INT_UifIcon("list");
		INT_UIF_Draw.Texture(m_INT_Drawer, 13, 48, 20, 20, listIcon, INT_VPPX_Look.WHITE);
		m_INT_DrawerCount = INT_VPPX_Look.Text(m_INT_Drawer, 0, 80, 46, 20, " ", 15, INT_VPPX_Look.ORANGE, INT_UIF_Align.CENTER, false);
		string word = TXT_LIST;
		int n = word.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string ch = word.SubstringUtf8(i, 1);
			INT_VPPX_Look.Text(m_INT_Drawer, 0, 112 + i * 14, 46, 14, ch, 11, INT_VPP_Ui.M, INT_UIF_Align.CENTER, true);
		}
		INT_VPP_Ui.INT_Sort(m_INT_Drawer);
	}

	void INT_SetListCount(int n)
	{
		string s = n.ToString();
		if (m_INT_DrawerCount)
			m_INT_DrawerCount.SetText(s);
		if (m_INT_ListCount)
			m_INT_ListCount.SetText(s);
	}

	// ------------------------------------------------------------------ list: expanded panel
	// the open list panel (tab «Метки»; null when folded): its entrance animation (2026-11-14)
	Widget INT_GetListRoot()
	{
		return m_INT_List;
	}

	bool INT_IsListOpen()
	{
		return m_INT_ListOpen;
	}

	void INT_SetListOpen(bool open, array<ref INT_VPP_ListRow> rows)
	{
		m_INT_ListOpen = open;
		if (!open)
		{
			if (m_INT_List)
			{
				m_INT_List.Show(false);
				if (m_INT_DeadList)
					m_INT_DeadList.Unlink();
				m_INT_DeadList = m_INT_List;
			}
			m_INT_List = null;
			m_INT_ListRows = null;
			m_INT_ListWheel = null;
			m_INT_ListFold = null;
			m_INT_ListCount = null;
			m_INT_ListCap = null;
			m_INT_RowHits.Clear();
			m_INT_RowIdx.Clear();
			INT_ForgetChips();
			if (m_INT_Drawer)
				m_INT_Drawer.Show(true);
			return;
		}
		if (m_INT_Drawer)
			m_INT_Drawer.Show(false);
		if (!m_INT_List)
			INT_BuildListPanel();
		INT_SetRows(rows);
	}

	protected void INT_BuildListPanel()
	{
		float x0 = m_INT_W - LIST_W;
		float y0 = LIST_Y;
		float h = m_INT_H - LIST_Y - LIST_BOTTOM;
		m_INT_List = INT_VPP_Ui.INT_Panel(m_INT_Screen, x0, y0, LIST_W + 4, h, INT_VPP_Ui.ISL_BG, INT_VPP_Ui.ISL_EDGE, true, "IntVppList");
		if (!m_INT_List)
			return;
		m_INT_ListWheel = INT_Blocker(m_INT_List, LIST_W, h);
		string cap = INT_VPPX_Text.Caps(TXT_MARKS_CAP);
		// the caption doubles as the hint line of the mode chips (hover)
		m_INT_ListCap = INT_VPPX_Look.Text(m_INT_List, 20, 14, 300, 20, cap, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		m_INT_ListCount = INT_VPPX_Look.Text(m_INT_List, LIST_W - 20 - 40 - 80, 14, 80, 20, " ", 15, INT_VPPX_Look.ORANGE, INT_UIF_Align.RIGHT, false);
		INT_VPPX_Look.Outline(m_INT_List, LIST_W - 20 - 32, 8, 32, 32, INT_VPP_Ui.LINE2);
		INT_VPPX_Look.IconAt(m_INT_List, LIST_W - 20 - 32 + 8, 16, 16, "chevron", INT_VPPX_Look.TEXT);
		m_INT_ListFold = INT_UIF_Draw.Hit(m_INT_List, LIST_W - 20 - 32, 8, 32, 32, this);
		INT_VPPX_Look.Fill(m_INT_List, 20, 48, LIST_W - 40, 1, INT_VPPX_Look.LINE);
		m_INT_VisibleRows = (h - 60 - 12) / LIST_ROW;
		if (m_INT_VisibleRows < 1)
			m_INT_VisibleRows = 1;
		INT_VPP_Ui.INT_Sort(m_INT_List);
	}

	void INT_SetRows(array<ref INT_VPP_ListRow> rows)
	{
		m_INT_Rows.Clear();
		if (rows)
		{
			foreach (INT_VPP_ListRow r : rows)
			{
				if (r)
					m_INT_Rows.Insert(r);
			}
		}
		int maxScroll = m_INT_Rows.Count() - m_INT_VisibleRows;
		if (maxScroll < 0)
			maxScroll = 0;
		if (m_INT_Scroll > maxScroll)
			m_INT_Scroll = maxScroll;
		if (m_INT_Scroll < 0)
			m_INT_Scroll = 0;
		INT_DrawRows();
	}

	// visible rows only, rebuilt on data / scroll change (never per frame)
	protected void INT_DrawRows()
	{
		if (!m_INT_List)
			return;
		if (m_INT_ListRows)
		{
			m_INT_ListRows.Show(false);
			if (m_INT_DeadRows)
				m_INT_DeadRows.Unlink();
			m_INT_DeadRows = m_INT_ListRows;
		}
		m_INT_RowHits.Clear();
		m_INT_RowIdx.Clear();
		INT_ForgetChips();
		float h = m_INT_H - LIST_Y - LIST_BOTTOM;
		m_INT_ListRows = INT_UIF_Draw.Frame(m_INT_List, 0, 56, LIST_W, h - 60, "IntVppRows");
		if (!m_INT_ListRows)
			return;
		int n = m_INT_Rows.Count();
		if (n == 0)
		{
			INT_VPPX_Look.Text(m_INT_ListRows, 20, 8, 300, 24, TXT_EMPTY_LIST, 15, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
			INT_VPP_Ui.INT_Sort(m_INT_ListRows);
			return;
		}
		int last = m_INT_Scroll + m_INT_VisibleRows;
		if (last > n)
			last = n;
		float y = 0;
		for (int i = m_INT_Scroll; i < last; i++)
		{
			INT_VPP_ListRow r = m_INT_Rows.Get(i);
			if (!r)
				continue;
			// rows with a mode chip: the chip sits left of the distance / count, the name shrinks to the room left
			float chipX = LIST_W - 20 - 64 - 8 - CHIP_W;
			if (r.m_INT_Type == INT_VPP_ListRow.TYPE_HEADER)
			{
				string hc = INT_VPPX_Text.Caps(r.m_INT_Text);
				INT_VPPX_Look.Diamond(m_INT_ListRows, 26, y + 18, 8, true, r.m_INT_Color);
				INT_VPPX_Look.Text(m_INT_ListRows, 40, y + 8, 240, 20, hc, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
				INT_VPPX_Look.Text(m_INT_ListRows, LIST_W - 20 - 64, y + 8, 64, 20, r.m_INT_Sub, 13, INT_VPP_Ui.M, INT_UIF_Align.RIGHT, false);
				if (r.m_INT_HasMode)
					INT_AddChip(i, chipX, y + 5, null, true);
			}
			else
			{
				int tc = INT_VPPX_Look.WHITE;
				if (r.m_INT_Dim)
					tc = INT_VPP_Ui.CHIP_OFF_TEXT;
				if (r.m_INT_Icon != "")
					INT_UIF_Draw.Texture(m_INT_ListRows, 22, y + 7, 20, 20, r.m_INT_Icon, r.m_INT_Color);
				else
					INT_VPPX_Look.Diamond(m_INT_ListRows, 32, y + 17, 9, true, r.m_INT_Color);
				float nameRoom = 250;
				float distX = LIST_W - 20 - 90;
				float distW = 90;
				if (r.m_INT_HasMode)
				{
					nameRoom = chipX - 8 - 52 - 2;
					distX = LIST_W - 20 - 64;
					distW = 64;
				}
				string fit = INT_VPPX_Look.FitText(r.m_INT_Text, 15, false, nameRoom);
				TextWidget nameW = INT_VPPX_Look.Text(m_INT_ListRows, 52, y + 5, nameRoom + 4, 24, fit, 15, tc, INT_UIF_Align.LEFT, false);
				INT_VPPX_Look.Text(m_INT_ListRows, distX, y + 7, distW, 20, r.m_INT_Sub, 13, INT_VPP_Ui.M, INT_UIF_Align.RIGHT, false);
				ButtonWidget hit = INT_UIF_Draw.Hit(m_INT_ListRows, 12, y, LIST_W - 24, LIST_ROW, this);
				if (hit)
				{
					m_INT_RowHits.Insert(hit);
					m_INT_RowIdx.Insert(i);
				}
				// after the row's hit: the chip's own hit lies on top and takes the click (never the row's)
				if (r.m_INT_HasMode)
					INT_AddChip(i, chipX, y + 5, nameW, r.m_INT_Can3);
			}
			y = y + LIST_ROW;
		}
		if (n > m_INT_VisibleRows)
		{
			// scroll position bar (6 px wide at the right edge)
			float trackH = m_INT_VisibleRows * LIST_ROW;
			float barH = trackH * m_INT_VisibleRows / n;
			if (barH < 20)
				barH = 20;
			float barY = (trackH - barH) * m_INT_Scroll / (n - m_INT_VisibleRows);
			INT_VPPX_Look.Fill(m_INT_ListRows, LIST_W - 8, 0, 2, trackH, INT_VPPX_Look.LINE);
			INT_VPPX_Look.Fill(m_INT_ListRows, LIST_W - 8, barY, 2, barH, INT_VPPX_Look.ORANGE);
		}
		INT_VPP_Ui.INT_Sort(m_INT_ListRows);
	}

	// ------------------------------------------------------------------ mode chips (2026-11-12)
	protected void INT_ForgetChips()
	{
		m_INT_ModeHits.Clear();
		m_INT_ModeRowIdx.Clear();
		m_INT_ModeBg.Clear();
		m_INT_ModeTxt.Clear();
		m_INT_ModeName.Clear();
		m_INT_ChipShown.Clear();
	}

	// one chip inside the rows frame (its tree goes with the frame); 3D not allowed = fainter edge + hint
	protected void INT_AddChip(int rowIdx, float x, float y, TextWidget nameW, bool can3)
	{
		ImageWidget bg = INT_VPPX_Look.Fill(m_INT_ListRows, x, y, CHIP_W, CHIP_H, INT_VPP_Ui.BTN_BG);
		int edge = INT_VPP_Ui.LINE2;
		if (!can3)
			edge = INT_VPP_Ui.CHIP_EDGE;
		INT_VPPX_Look.Outline(m_INT_ListRows, x, y, CHIP_W, CHIP_H, edge);
		TextWidget t = INT_VPPX_Look.Text(m_INT_ListRows, x, y + 1, CHIP_W, CHIP_H - 2, " ", 13, INT_VPPX_Look.WHITE, INT_UIF_Align.CENTER, true);
		ButtonWidget hit = INT_UIF_Draw.Hit(m_INT_ListRows, x, y, CHIP_W, CHIP_H, this);
		if (!hit || !t || !bg)
			return;
		m_INT_ModeHits.Insert(hit);
		m_INT_ModeRowIdx.Insert(rowIdx);
		m_INT_ModeBg.Insert(bg);
		m_INT_ModeTxt.Insert(t);
		m_INT_ModeName.Insert(nameW);
		m_INT_ChipShown.Insert("");
		int k = m_INT_ModeHits.Count() - 1;
		INT_PaintChip(k);
	}

	protected void INT_PaintChip(int k)
	{
		if (k < 0 || k >= m_INT_ModeHits.Count())
			return;
		int ri = m_INT_ModeRowIdx.Get(k);
		if (ri < 0 || ri >= m_INT_Rows.Count())
			return;
		INT_VPP_ListRow r = m_INT_Rows.Get(ri);
		TextWidget t = m_INT_ModeTxt.Get(k);
		ImageWidget bg = m_INT_ModeBg.Get(k);
		if (!r || !t || !bg)
			return;
		string s = "—";
		int tc = INT_VPP_Ui.M;
		int bc = INT_VPP_Ui.BTN_BG;
		if (r.m_INT_Mode == INT_VPP_MODE_3D)
		{
			s = "3D";
			tc = INT_VPPX_Look.ORANGE;
			bc = CHIP_3D_BG;
		}
		else if (r.m_INT_Mode == INT_VPP_MODE_2D)
		{
			s = "2D";
			tc = INT_VPPX_Look.WHITE;
		}
		else if (r.m_INT_Mode == INT_VPP_MODE_HIDDEN)
		{
			s = "×";
			tc = INT_VPP_Ui.CHIP_OFF_TEXT;
		}
		t.SetText(s);
		t.SetColor(tc);
		bg.SetColor(bc);
		if (k < m_INT_ChipShown.Count())
			m_INT_ChipShown.Set(k, s);
		TextWidget nm = m_INT_ModeName.Get(k);
		if (nm)
		{
			int nc = INT_VPPX_Look.WHITE;
			if (r.m_INT_Dim)
				nc = INT_VPP_Ui.CHIP_OFF_TEXT;
			nm.SetColor(nc);
		}
	}

	protected void INT_PaintChips()
	{
		int n = m_INT_ModeHits.Count();
		for (int k = 0; k < n; k++)
		{
			INT_PaintChip(k);
		}
	}

	// click on chip k: the marker (or the whole group) to its next mode; only that data changes, the visible chips
	// are repainted (no list rebuild)
	protected void INT_OnChip(int k)
	{
		if (!m_INT_Menu || k < 0 || k >= m_INT_ModeHits.Count())
			return;
		int ri = m_INT_ModeRowIdx.Get(k);
		if (ri < 0 || ri >= m_INT_Rows.Count())
			return;
		INT_VPP_ListRow r = m_INT_Rows.Get(ri);
		if (!r)
			return;
		bool changed = false;
		if (r.m_INT_Type == INT_VPP_ListRow.TYPE_HEADER)
		{
			changed = m_INT_Menu.INT_CycleGroupMode(r, m_INT_Rows);
		}
		else
		{
			changed = m_INT_Menu.INT_CycleRowMode(r);
			// its group header follows
			foreach (INT_VPP_ListRow h : m_INT_Rows)
			{
				if (h && h.m_INT_Type == INT_VPP_ListRow.TYPE_HEADER && h.m_INT_Layer == r.m_INT_Layer)
					m_INT_Menu.INT_SyncRowMode(h);
			}
		}
		if (changed)
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
		else
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
		INT_PaintChips();
		INT_ChipHint(k);
	}

	// the hint of chip k in the caption line (hover), -1 = the caption again
	protected void INT_ChipHint(int k)
	{
		if (!m_INT_ListCap)
			return;
		string cap = INT_VPPX_Text.Caps(TXT_MARKS_CAP);
		if (k < 0 || k >= m_INT_ModeRowIdx.Count())
		{
			m_INT_ListCap.SetText(cap);
			m_INT_CapShown = cap;
			return;
		}
		int ri = m_INT_ModeRowIdx.Get(k);
		if (ri < 0 || ri >= m_INT_Rows.Count())
			return;
		INT_VPP_ListRow r = m_INT_Rows.Get(ri);
		if (!r)
			return;
		string hint = TXT_MODE_MIXED;
		if (r.m_INT_Mode == INT_VPP_MODE_3D)
			hint = TXT_MODE_3D;
		else if (r.m_INT_Mode == INT_VPP_MODE_2D)
			hint = TXT_MODE_2D;
		else if (r.m_INT_Mode == INT_VPP_MODE_HIDDEN)
			hint = TXT_MODE_OFF;
		if (r.m_INT_Type == INT_VPP_ListRow.TYPE_HEADER)
			hint = TXT_MODE_GROUP + hint;
		else if (!r.m_INT_Can3)
			hint = hint + TXT_MODE_NO3D;
		m_INT_ListCap.SetText(hint);
		m_INT_CapShown = hint;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int k = m_INT_ModeHits.Find(ButtonWidget.Cast(w));
		if (k >= 0)
			INT_ChipHint(k);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int k = m_INT_ModeHits.Find(ButtonWidget.Cast(w));
		if (k >= 0)
			INT_ChipHint(-1);
		return false;
	}

	// TEST: the rows of the list, clicks on a row's chip (scrolls it into view first), the shown chip text / hint
	array<ref INT_VPP_ListRow> INT_TestRows()
	{
		return m_INT_Rows;
	}

	int INT_TestChipCount()
	{
		return m_INT_ModeHits.Count();
	}

	protected int INT_TestChipOf(int rowIdx)
	{
		int k = m_INT_ModeRowIdx.Find(rowIdx);
		if (k >= 0)
			return k;
		if (rowIdx < 0 || rowIdx >= m_INT_Rows.Count())
			return -1;
		m_INT_Scroll = rowIdx;
		int maxScroll = m_INT_Rows.Count() - m_INT_VisibleRows;
		if (maxScroll < 0)
			maxScroll = 0;
		if (m_INT_Scroll > maxScroll)
			m_INT_Scroll = maxScroll;
		INT_DrawRows();
		k = m_INT_ModeRowIdx.Find(rowIdx);
		return k;
	}

	bool INT_TestClickChip(int rowIdx)
	{
		int k = INT_TestChipOf(rowIdx);
		if (k < 0)
			return false;
		INT_OnChip(k);
		return true;
	}

	string INT_TestChipText(int rowIdx)
	{
		int k = INT_TestChipOf(rowIdx);
		if (k < 0)
			return "";
		if (k >= m_INT_ChipShown.Count())
			return "";
		string s = m_INT_ChipShown.Get(k);
		return s;
	}

	string INT_TestCaption()
	{
		return m_INT_CapShown;
	}

	void INT_TestHover(int rowIdx)
	{
		int k = INT_TestChipOf(rowIdx);
		INT_ChipHint(k);
	}

	// ------------------------------------------------------------------ visibility per mode
	void INT_ShowIslands(bool coords, bool drawer, bool layers)
	{
		INT_ShowW(m_INT_Coords, coords);
		bool drawerShown = false;
		if (drawer && !m_INT_ListOpen)
			drawerShown = true;
		INT_ShowW(m_INT_Drawer, drawerShown);
		INT_ShowW(m_INT_Layers, layers);
		if (!layers)
			INT_ClosePop();
	}

	protected void INT_ShowW(Widget w, bool show)
	{
		if (w && w.IsVisible() != show)
			w.Show(show);
	}

	// ------------------------------------------------------------------ events
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!m_INT_Menu)
			return false;
		if (button != MouseState.LEFT)
		{
			if (INT_IsBlocker(w))
				return true;
			return false;
		}
		int tab = m_INT_TabHit.Find(ButtonWidget.Cast(w));
		if (tab >= 0)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TAB);
			m_INT_Menu.INT_SelectTab(tab);
			return true;
		}
		if (m_INT_PanelsReset && w == m_INT_PanelsReset)
		{
			INT_ResetAllPanels();
			return true;
		}
		int chip = m_INT_ChipHit.Find(ButtonWidget.Cast(w));
		if (chip >= 0)
		{
			INT_VPPX_Check ck = m_INT_ChipCheck.Get(chip);
			if (ck)
				ck.Toggle();
			return true;
		}
		if (w == m_INT_GearHit)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
			if (m_INT_Pop)
				INT_Close3DPop();
			else
				INT_OpenPop();
			return true;
		}
		if (m_INT_MiniGearHit && w == m_INT_MiniGearHit)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
			if (m_INT_MiniPanel)
				INT_CloseMiniPanel();
			else
				INT_OpenMiniPanel();
			return true;
		}
		if (m_INT_MiniHit && w == m_INT_MiniHit)
		{
			if (m_INT_MiniCheck)
				m_INT_MiniCheck.Toggle();
			return true;
		}
		int pm = m_INT_PopMinus.Find(ButtonWidget.Cast(w));
		if (pm >= 0)
		{
			INT_PopStep(pm, -1);
			return true;
		}
		int pp = m_INT_PopPlus.Find(ButtonWidget.Cast(w));
		if (pp >= 0)
		{
			INT_PopStep(pp, 1);
			return true;
		}
		// a mode chip before its row (the chip lies over the row's hit)
		int mc = m_INT_ModeHits.Find(ButtonWidget.Cast(w));
		if (mc >= 0)
		{
			INT_OnChip(mc);
			return true;
		}
		int lm = m_INT_LookMinus.Find(ButtonWidget.Cast(w));
		if (lm >= 0)
		{
			INT_LookStep(lm, -1);
			return true;
		}
		int lp = m_INT_LookPlus.Find(ButtonWidget.Cast(w));
		if (lp >= 0)
		{
			INT_LookStep(lp, 1);
			return true;
		}
		if (m_INT_LookReset && w == m_INT_LookReset)
		{
			INT_LookReset();
			return true;
		}
		if (w == m_INT_DrawerHit)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TAB);
			m_INT_Menu.INT_SelectTab(1);
			return true;
		}
		if (w == m_INT_ListFold)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TAB);
			m_INT_Menu.INT_SelectTab(0);
			return true;
		}
		int row = m_INT_RowHits.Find(ButtonWidget.Cast(w));
		if (row >= 0)
		{
			int ri = m_INT_RowIdx.Get(row);
			if (ri >= 0 && ri < m_INT_Rows.Count())
			{
				INT_VPP_ListRow r = m_INT_Rows.Get(ri);
				INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.SELECT);
				m_INT_Menu.INT_OnListRow(r);
			}
			return true;
		}
		if (INT_IsBlocker(w))
			return true;
		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		// a double click on an island never reaches the map
		ButtonWidget b = ButtonWidget.Cast(w);
		if (!b)
			return false;
		if (INT_IsBlocker(w))
			return true;
		if (m_INT_TabHit.Find(b) >= 0 || m_INT_ChipHit.Find(b) >= 0 || m_INT_RowHits.Find(b) >= 0)
			return true;
		if (b == m_INT_MiniHit || b == m_INT_MiniGearHit || b == m_INT_PanelsReset)
			return true;
		if (m_INT_ModeHits.Find(b) >= 0)
			return true;
		return false;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		bool inList = false;
		if (m_INT_List && (w == m_INT_ListWheel || m_INT_RowHits.Find(ButtonWidget.Cast(w)) >= 0))
			inList = true;
		if (!inList)
		{
			if (INT_IsBlocker(w))
				return true;
			return false;
		}
		int maxScroll = m_INT_Rows.Count() - m_INT_VisibleRows;
		if (maxScroll < 0)
			maxScroll = 0;
		int next = m_INT_Scroll - wheel * 3;
		if (next < 0)
			next = 0;
		if (next > maxScroll)
			next = maxScroll;
		if (next != m_INT_Scroll)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
			m_INT_Scroll = next;
			INT_DrawRows();
		}
		return true;
	}
}
