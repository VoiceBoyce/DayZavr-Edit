// INT Quests (client) — the admin workspace «Схема» (owner pick B2, design/concepts_v1/b2_graph.png, palette of the
// notebook style B). A free field of index cards, one per quest; lines show «requires» (solid inside a branch, dashed
// between branches, red for the selected card). As much room as possible and a deep zoom-out (owner): the window
// takes the whole screen, the field zooms from 1.6 down to a dot overview. Only the cards inside the view have
// widgets (pooled); at the overview level cards are drawn as strokes on the line canvas (no widgets at all).
// Mouse: left = select / drag a card (or pan on empty paper), right = pan, wheel = zoom at the cursor, double click =
// quest editor. Every edit is saved at once (one quest per request); the card layout is saved after a short pause.

class INT_QST_BNode : Managed
{
	int Id;
	float X;
	float Y;
	bool Saved;			// came from Board.json or was moved by hand (false = an automatic place)
	bool Moved;			// moved here and not sent yet (a place from the server does not overwrite it)
}

class INT_QST_BSlot : Managed
{
	Widget Root;
	int Id;
	int Lod = -1;
	float Z = -1;
	bool Full;
	bool Sel;
	bool Linking;
	bool Lit;
	int Rev = -1;
}

// Mouse on the field / the minimap (the board itself is not a widget handler).
class INT_QST_BoardInput extends ScriptedWidgetEventHandler
{
	INT_QST_Board m_Owner;
	bool m_Mini;

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (!m_Owner)
			return false;
		if (m_Mini)
			m_Owner.MiniDown();
		else
			m_Owner.CanvasDown(button);
		return true;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (m_Owner)
			m_Owner.CanvasWheel(wheel);
		return true;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (m_Owner && !m_Mini && button == MouseState.LEFT)
			m_Owner.CanvasDouble();
		return true;
	}
}

// Rows of the board popups (branch filter, check list).
class INT_QST_PopupRow : Managed
{
	string Key;
	string Text;
	string Sub;
	int Color;
	int Id;
}

class INT_QST_PopupSlot extends INT_QST_VSlot
{
	Widget m_Box;
}

class INT_QST_PopupSource extends INT_QST_VSource
{
	ref array<ref INT_QST_PopupRow> m_Rows = new array<ref INT_QST_PopupRow>();
	INT_QST_Board m_Owner;
	int m_Kind;		// 1 branch filter, 2 check list

	override int Count()
	{
		return m_Rows.Count();
	}

	override float RowHeight(int i)
	{
		return 40;
	}

	override INT_QST_VSlot NewSlot()
	{
		return new INT_QST_PopupSlot();
	}

	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_PopupSlot s = INT_QST_PopupSlot.Cast(slot);
		s.m_Box = INT_UIF_Draw.Frame(s.m_Root, 0, 0, w, 40, "Row");
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_PopupSlot s = INT_QST_PopupSlot.Cast(slot);
		INT_UIF_Widgets.ClearChildren(s.m_Box);
		INT_QST_PopupRow r = m_Rows[i];
		Widget hover = INT_UIF_Draw.Frame(s.m_Box, 0, 1, w, 38, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, w, 38, 0x262A2218);
		s.m_Hover = hover;
		float tx = 12;
		if (r.Color != 0)
		{
			INT_UIF_Draw.Line(s.m_Box, 10, 13, 12, 12, r.Color);
			tx = 32;
		}
		int ink = INT_QST_Look.Ink();
		string t = INT_QST_Look.Fit(r.Text, w - tx - 120, 15);
		INT_QST_Look.Heavy(s.m_Box, tx, 0, w - tx - 120, 40, t, 15, ink);
		if (r.Sub != "")
		{
			int ink3 = INT_QST_Look.Ink3();
			INT_QST_Look.Text(s.m_Box, w - 120, 0, 110, 40, r.Sub, 13, ink3, INT_UIF_Align.RIGHT);
		}
		INT_QST_Look.Hair(s.m_Box, 8, 39, w - 16);
	}

	override void OnRowClick(int i, int button)
	{
		if (i < 0 || i >= m_Rows.Count())
			return;
		INT_QST_PopupRow r = m_Rows[i];
		m_Owner.PopupPick(m_Kind, r.Key, r.Id);
	}
}

class INT_QST_Board extends INT_QST_Book
{
	static const float NODE_W = 236;
	static const float NODE_H = 72;
	static const float COL_GAP = 66;
	static const float ROW_GAP = 22;
	static const float BAND_GAP = 96;
	static const float INSP_W = 392;
	static const float TOOL_H = 56;
	static const float ZMIN = 0.05;
	static const float ZMAX = 1.6;
	static const float MINI_W = 250;
	static const float MINI_H = 150;
	static const float LANE_HEAD = 44;		// room for the branch name above its lane
	static const int LANE_MAX = 20;			// cards in one line of a lane (quests open at once wrap like text)
	static const int HUB_MIN = 6;			// a quest required by this many others is a «gate»
	static const int LINE_BUDGET = 1800;	// canvas lines per frame (past the engine's limit the whole UI blinks)
	static const int STROKE_RESERVE = 500;	// part of the budget kept for the cards drawn as strokes
	static const int DASH_MAX = 60;			// dashes on one visible line, longer dashes past that
	static const int C_GATE = 0xFFA8641A;
	static const int C_LIT = 0xFFE0A81C;		// cards that come from the selected quest (marker yellow: no branch has it)

	static const int ID_RELOAD = 1;
	static const int ID_NEW = 2;
	static const int ID_LINK = 3;
	static const int ID_AUTO = 4;
	static const int ID_ZOOM_OUT = 5;
	static const int ID_ZOOM_IN = 6;
	static const int ID_FIT = 7;
	static const int ID_BRANCH = 8;
	static const int ID_CHECK = 9;
	static const int ID_EDIT = 10;
	static const int ID_DELETE = 11;
	static const int ID_ENABLE = 12;
	static const int ID_REQ_DEL = 13;
	static const int ID_REQ_MODE = 14;
	static const int ID_POPUP_BG = 15;
	static const int ID_SAVE = 16;

	static const int ASK_DELETE = 1;
	static const int ASK_RELOAD = 2;
	static const int ASK_AUTO = 3;

	static float s_PanX;		// the view survives closing the window (same session)
	static float s_PanY;
	static float s_Z = -1;
	static string s_Branch = "";

	protected Widget m_Field;		// clip frame of the field
	protected CanvasWidget m_Lines;
	protected Widget m_NodesLayer;
	protected Widget m_LabelsLayer;
	protected ButtonWidget m_FieldHit;
	protected ref INT_QST_BoardInput m_Input;
	protected Widget m_Mini;
	protected CanvasWidget m_MiniLines;
	protected Widget m_Tool;
	protected Widget m_ToolBtns;		// rebuilt (zoom %, filter, check count); the search field is built once
	protected Widget m_Insp;
	protected Widget m_StatusLine;
	protected Widget m_Counters;
	protected Widget m_Layer;
	protected Widget m_Popup;
	protected ref INT_QST_VList m_PopupList;
	protected ref INT_QST_PopupSource m_PopupSrc;
	protected ref INT_QST_Field m_Search;
	protected ref INT_QST_QuestEditor m_Editor;
	// a popup closes from its own row click: its list and source live until the next open, its widgets are hidden
	// and unlinked on the next frame (never free an object or unlink a widget inside its own event)
	protected ref INT_QST_VList m_PopupListOld;
	protected ref INT_QST_PopupSource m_PopupSrcOld;
	protected ref array<Widget> m_Dead = new array<Widget>();
	protected bool m_ToolDirty;			// the toolbar is rebuilt on the next frame (its buttons call for it)
	protected int m_HelloSeqOpen = -1;		// INT_QST_Admin.s_HelloSeq when this opening asked for HELLO
	protected int m_ShapeSeen = -1;			// INT_QST_Client.s_ShapeRevision of the last refresh
	// quest edits in flight, in send order (the server answers in order), and the editor copies kept until the
	// answer: a refused save opens the editor again with the admin's edit
	protected ref array<int> m_SentIds = new array<int>();
	protected ref map<int, ref INT_QST_QuestDef> m_Kept = new map<int, ref INT_QST_QuestDef>();
	protected ref map<int, int> m_KeptBase = new map<int, int>();

	protected float m_CW;
	protected float m_CH;
	protected float m_PanX;
	protected float m_PanY;
	protected float m_Z = 0.5;
	protected ref map<int, ref INT_QST_BNode> m_Nodes = new map<int, ref INT_QST_BNode>();
	protected ref array<ref INT_QST_BSlot> m_Slots = new array<ref INT_QST_BSlot>();
	protected ref map<int, ref INT_QST_BSlot> m_SlotOf = new map<int, ref INT_QST_BSlot>();
	protected ref map<string, int> m_BranchColor = new map<string, int>();
	protected ref array<int> m_Visible = new array<int>();
	protected int m_Sel;
	protected string m_Branch = "";
	protected bool m_LinkMode;
	protected int m_LinkFrom;
	protected int m_DataRev;
	protected bool m_ViewDirty = true;
	protected bool m_InspDirty = true;
	protected bool m_LayoutDirty;
	protected float m_LayoutSaveAt;
	protected int m_Pending;
	protected int m_TempId = -1;
	protected bool m_Hooked;
	protected string m_Status;
	protected int m_StatusColor;
	protected ref array<string> m_SearchHits = new array<string>();
	protected int m_SearchPos;

	// links: a quest required by many others is a «gate» — no lines from it (a start quest required by 312 quests
	// made 312 dashed lines: the canvas overflowed and the window blinked); selected, it lights the cards it opens
	protected ref map<int, int> m_DepCount = new map<int, int>();
	protected ref map<int, bool> m_Hub = new map<int, bool>();
	protected ref array<string> m_GroupBranch = new array<string>();
	protected ref array<int> m_GroupFirst = new array<int>();
	protected ref array<int> m_GroupCount = new array<int>();
	// pooled branch names (rebuilt only when the text or the size changes)
	protected ref map<string, Widget> m_LabelOf = new map<string, Widget>();
	protected ref map<string, string> m_LabelKey = new map<string, string>();
	protected ref map<string, float> m_LabelW = new map<string, float>();
	protected ref map<int, bool> m_Keep = new map<int, bool>();
	protected ref array<int> m_Free = new array<int>();
	protected CanvasWidget m_MiniView;		// the view frame; the cards of the minimap are drawn only on change
	protected string m_MiniKey = "";
	protected int m_LayoutRev;
	protected int m_LineBudget;
	protected bool m_LinesFull;
	protected bool m_FullLogged;
	protected bool m_FitOnHello;

	// selection: m_Picked = every selected card (Shift+click adds/removes, a branch name picks the whole branch);
	// m_Sel = the card the inspector shows (with more than one picked the inspector shows the group)
	protected ref map<int, bool> m_Picked = new map<int, bool>();
	protected ref map<int, vector> m_DragStart = new map<int, vector>();	// start places of the cards a drag moves
	protected bool m_DownEmpty;		// pressed on empty paper: a click without a move clears the selection
	protected bool m_DownShift;
	protected int m_DownCard;
	protected bool m_Moved;
	// branch names on screen (field px) for the mouse
	protected ref array<string> m_LabelHitKey = new array<string>();
	protected ref array<float> m_LabelHitX = new array<float>();
	protected ref array<float> m_LabelHitY = new array<float>();
	protected ref array<float> m_LabelHitW = new array<float>();
	protected ref array<float> m_LabelHitH = new array<float>();

	// mouse
	protected int m_Drag;			// 0 none, 1 pan, 2 card, 3 pressed on a card (not moved yet)
	protected int m_DragButton;
	protected float m_DownX;
	protected float m_DownY;
	protected float m_StartPanX;
	protected float m_StartPanY;

	override void GetWindowSize(out float w, out float h)
	{
		w = 1880;
		h = 1040;
	}

	override float CoverTop()
	{
		return 96;
	}

	override bool AdminOnly()
	{
		return true;
	}

	override bool WantsKeyboard()
	{
		return true;
	}

	override string ToggleInput()
	{
		return "";
	}

	INT_QST_Clicks Clicks()
	{
		return m_Clicks;
	}

	// --- cover ------------------------------------------------------------------------------------------------------
	override void BuildCoverHeader(Widget header, float w)
	{
		Widget plate = INT_UIF_Draw.Frame(header, 24, 14, 330, 54, "TitlePlate");
		int pf = INT_QST_Look.Note();
		int pb = INT_QST_Look.Tan2();
		INT_UIF_Surface.Box(plate, 0, 0, 330, 54, 4, pf, pb);
		INT_QST_Look.Tex(plate, 0, 0, 330, 54, "int_qst_paper_ca", pf);
		INT_QST_Look.Icon(plate, 14, 13, 28, "grid", INT_QST_Look.RedDeep());
		string brand = INT_QST_Look.Spaced("DayZavr Quests");
		INT_QST_Look.Heavy(plate, 54, 6, 270, 18, brand, 11, INT_QST_Look.Red());
		string title = INT_QST_Look.Upper("#STR_INT_QST_B_TITLE");
		INT_QST_Look.Heavy(plate, 54, 22, 270, 28, title, 20, INT_QST_Look.Ink());
		INT_QST_Look.Text(header, 382, 14, 400, 20, "DayZavr Quest System", 14, INT_QST_Look.Tan());
		string adm = INT_QST_Look.Tr("#STR_INT_QST_TAB_ADMIN");
		INT_QST_Look.Icon(header, 382, 42, 16, "shield", INT_QST_Look.Tan2());
		INT_QST_Look.Text(header, 404, 38, 300, 24, adm, 14, INT_QST_Look.Tan2());
		m_Counters = INT_UIF_Draw.Frame(header, 720, 22, 420, 40, "Counters");
		m_StatusLine = INT_UIF_Draw.Frame(header, 1150, 22, 360, 40, "Status");
		// reload
		Widget hv;
		int ic = INT_QST_Look.Tan();
		Widget rb = INT_QST_Look.Button(header, w - 24 - 40 - 12 - 230, 19, 230, 40, "#STR_INT_QST_A_RELOAD", "refresh", INT_QST_Look.BTN_INK, ic, hv);
		ButtonWidget rh = INT_UIF_Draw.Hit(rb, 0, 0, 230, 40, m_Clicks);
		m_Clicks.Bind(rh, ID_RELOAD, 0, hv);
	}

	// --- body -------------------------------------------------------------------------------------------------------
	override void OnBuild(Widget body)
	{
		float bw = GetBodyW();
		float bh = GetBodyH();
		INT_QST_Look.PaperSheet(body, 0, 0, bw, bh, INT_QST_Look.Paper(), "Paper");
		m_Tool = INT_UIF_Draw.Frame(body, 0, 0, bw, TOOL_H, "Toolbar");
		m_ToolBtns = INT_UIF_Draw.Frame(m_Tool, 0, 0, bw, TOOL_H, "Buttons");
		INT_QST_Look.Hair(m_Tool, 0, TOOL_H - 1, bw);
		float searchX = bw - INSP_W - 14 - 300 - 170 - 24;
		m_Search = INT_QST_Field.Create(m_Tool, searchX, 11, 300, "", "", 60);
		m_Search.Event_Changed.Insert(OnSearchChanged);
		m_Search.Event_Submit.Insert(OnSearchSubmit);
		INT_QST_Look.Icon(m_Tool, searchX + 300 - 26, 20, 16, "search", INT_QST_Look.Ink3());
		m_CW = bw - INSP_W - 14;
		m_CH = bh - TOOL_H - 4;
		// the field: a clip frame, the line canvas, labels, cards, the input hit under everything clickable
		m_Field = INT_UIF_Draw.Clip(body, 0, TOOL_H + 4, m_CW, m_CH, "Field");
		int fieldFill = 0x14FFFFFF;
		INT_UIF_Draw.Line(m_Field, 0, 0, m_CW, m_CH, fieldFill);
		m_Lines = CanvasWidget.Cast(GetGame().GetWorkspace().CreateWidgets("INT_Quests_CL/gui/layouts/int_qst_canvas.layout", m_Field));
		if (m_Lines)
			INT_UIF_Draw.Place(m_Lines, 0, 0, m_CW, m_CH);
		m_LabelsLayer = INT_UIF_Draw.Frame(m_Field, 0, 0, m_CW, m_CH, "Labels");
		m_NodesLayer = INT_UIF_Draw.Frame(m_Field, 0, 0, m_CW, m_CH, "Nodes");
		// minimap (bottom-right corner of the field)
		m_Mini = INT_UIF_Draw.Frame(m_Field, m_CW - MINI_W - 12, m_CH - MINI_H - 12, MINI_W, MINI_H, "Mini");
		int kraft = INT_QST_Look.Kraft();
		int kb = INT_QST_Look.InkA(120);
		INT_UIF_Surface.Box(m_Mini, 0, 0, MINI_W, MINI_H, 6, kraft, kb);
		m_MiniLines = CanvasWidget.Cast(GetGame().GetWorkspace().CreateWidgets("INT_Quests_CL/gui/layouts/int_qst_canvas.layout", m_Mini));
		if (m_MiniLines)
			INT_UIF_Draw.Place(m_MiniLines, 0, 0, MINI_W, MINI_H);
		m_MiniView = CanvasWidget.Cast(GetGame().GetWorkspace().CreateWidgets("INT_Quests_CL/gui/layouts/int_qst_canvas.layout", m_Mini));
		if (m_MiniView)
			INT_UIF_Draw.Place(m_MiniView, 0, 0, MINI_W, MINI_H);
		// one input hit ON TOP of the whole field (frames above a hit would take the mouse); cards and the
		// minimap are found by position
		m_Input = new INT_QST_BoardInput();
		m_Input.m_Owner = this;
		m_FieldHit = INT_UIF_Draw.Hit(m_Field, 0, 0, m_CW, m_CH, m_Input);
		// inspector (right)
		m_Insp = INT_UIF_Draw.Frame(body, bw - INSP_W, TOOL_H + 4, INSP_W, m_CH, "Inspector");
		BuildToolbarNow();
		// overlays (editor, popups) on top of everything
		m_Layer = INT_UIF_Draw.Frame(m_Window, 0, 0, m_WinW, m_WinH, "Layer");
		m_Editor = new INT_QST_QuestEditor();
		m_Editor.Event_Save.Insert(OnEditorSave);
		m_Editor.Event_Delete.Insert(OnEditorDelete);
		m_Editor.Event_Closed.Insert(OnEditorClosed);
		AddKey(INT_QST_Look.Tr("#STR_INT_QST_K_LMB"), "#STR_INT_QST_B_K_SELECT");
		AddKey("Shift", "#STR_INT_QST_B_K_MULTI");
		AddKey(INT_QST_Look.Tr("#STR_INT_QST_K_RMB"), "#STR_INT_QST_B_K_PAN");
		AddKey(INT_QST_Look.Tr("#STR_INT_QST_B_K_WHEEL_KEY"), "#STR_INT_QST_B_K_ZOOM");
		AddKey(INT_QST_Look.Tr("#STR_INT_QST_B_K_DOUBLE_KEY"), "#STR_INT_QST_B_K_EDITOR");
		AddKey("DEL", "#STR_INT_QST_B_K_DELETE");
		AddKey("ESC", "#STR_INT_QST_K_CLOSE");
	}

	// The toolbar is rebuilt on the next frame: its own buttons ask for it (zoom, link, fit), and a button must not be
	// unlinked inside its own click.
	protected void BuildToolbar()
	{
		m_ToolDirty = true;
	}

	protected void BuildToolbarNow()
	{
		m_ToolDirty = false;
		if (!m_ToolBtns)
			return;
		m_Clicks.ForgetUnder(m_ToolBtns);
		INT_UIF_Widgets.ClearChildren(m_ToolBtns);
		float x = 16;
		x = x + ToolButton(x, ID_BRANCH, "filter", BranchButtonText(), false) + 8;
		x = x + ToolButton(x, ID_NEW, "plus", "#STR_INT_QST_B_NEW", false) + 8;
		x = x + ToolButton(x, ID_LINK, "link", "#STR_INT_QST_B_LINK", m_LinkMode) + 8;
		x = x + ToolButton(x, ID_AUTO, "grid", "#STR_INT_QST_B_AUTO", false) + 18;
		x = x + ToolButton(x, ID_ZOOM_OUT, "zoom_out", "", false) + 4;
		int zp = (int)Math.Round(m_Z * 100);
		string zt = zp.ToString() + " %";
		int ink = INT_QST_Look.Ink();
		INT_QST_Look.Heavy(m_ToolBtns, x, 10, 70, 36, zt, 15, ink, INT_UIF_Align.CENTER);
		x = x + 74;
		x = x + ToolButton(x, ID_ZOOM_IN, "zoom_in", "", false) + 4;
		x = x + ToolButton(x, ID_FIT, "fit", "#STR_INT_QST_B_FIT", false) + 18;
		// check (right of the search field)
		float sx = GetBodyW() - INSP_W - 14 - 300 - 170 - 24;
		string chk = INT_QST_Look.Tr("#STR_INT_QST_B_CHECK");
		int pc = ProblemsCount();
		if (pc > 0)
			chk = chk + "  " + pc.ToString();
		ToolButton(sx + 300 + 12, ID_CHECK, "warning", chk, pc > 0);
		// «Сохранить» (owner: just in case — edits are saved at once, the layout a moment after a drag)
		string sv = INT_QST_Look.Tr("#STR_INT_QST_B_SAVE");
		float svw = 40 + INT_UIF_Draw.TextWidth(sv, 14) * 1.12 + 8;
		ToolButton(GetBodyW() - 16 - svw, ID_SAVE, "save", "#STR_INT_QST_B_SAVE", true);
	}

	protected string BranchButtonText()
	{
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		if (m_Branch == "")
			return INT_QST_Look.Fmt("#STR_INT_QST_B_BRANCH_ALL", cat.m_BranchOrder.Count().ToString());
		string bt = cat.BranchTitle(m_Branch);
		return INT_QST_Look.Fmt("#STR_INT_QST_B_BRANCH_ONE", bt);
	}

	// A paper button on the toolbar; returns its width.
	protected float ToolButton(float x, int id, string icon, string label, bool on)
	{
		string t = INT_QST_Look.Tr(label);
		float tw = 0;
		if (t != "")
			tw = INT_UIF_Draw.TextWidth(t, 14) * 1.12 + 8;
		float w = 40 + tw;
		Widget b = INT_UIF_Draw.Frame(m_ToolBtns, x, 10, w, 36, "Tool");
		int fill = 0x26FFFFFF;
		int edge = INT_QST_Look.InkA(90);
		int tc = INT_QST_Look.Ink();
		if (on)
		{
			fill = INT_QST_Look.Ink();
			edge = INT_QST_Look.Ink();
			tc = INT_QST_Look.Cream();
		}
		INT_UIF_Surface.Box(b, 0, 0, w, 36, 6, fill, edge);
		Widget hv = INT_UIF_Draw.Frame(b, 0, 0, w, 36, "Hover");
		INT_UIF_Draw.Line(hv, 1, 1, w - 2, 34, 0x1F2A2218);
		INT_QST_Look.Icon(b, 12, 10, 16, icon, tc);
		if (t != "")
			INT_QST_Look.Heavy(b, 36, 0, tw, 36, t, 14, tc);
		ButtonWidget hit = INT_UIF_Draw.Hit(b, 0, 0, w, 36, m_Clicks);
		m_Clicks.Bind(hit, id, 0, hv);
		return w;
	}

	override void OnReady()
	{
		m_Branch = s_Branch;
		if (m_Branch != "" && !INT_QST_Client.s_Catalog.Branch(m_Branch))
			m_Branch = "";
		RebuildNodes();
		if (s_Z > 0)
		{
			m_Z = s_Z;
			m_PanX = s_PanX;
			m_PanY = s_PanY;
		}
		else
		{
			Fit();
			m_FitOnHello = true;		// the server places come a moment later: fit again then
		}
		BuildToolbarNow();
		m_ViewDirty = true;
		m_InspDirty = true;
		m_ShapeSeen = INT_QST_Client.s_ShapeRevision;
	}

	override void OnShow()
	{
		super.OnShow();
		if (!m_Hooked)
		{
			INT_QST_Client.s_OnChanged.Insert(OnDataChanged);
			INT_QST_Admin.s_OnReply.Insert(OnAdminReply);
			m_Hooked = true;
		}
		// the admin hello kept from an earlier opening may be old (or missing): what the server already has is
		// known only after the hello of THIS opening (HelloArrived)
		m_HelloSeqOpen = INT_QST_Admin.s_HelloSeq;
		INT_QST_Admin.RequestHello();
	}

	protected bool HelloArrived()
	{
		if (m_HelloSeqOpen < 0 || !INT_QST_Admin.s_Hello)
			return false;
		return INT_QST_Admin.s_HelloSeq > m_HelloSeqOpen;
	}

	override void OnHide()
	{
		SaveLayoutNow();
		s_PanX = m_PanX;
		s_PanY = m_PanY;
		s_Z = m_Z;
		s_Branch = m_Branch;
		Unhook();
		super.OnHide();
	}

	void ~INT_QST_Board()
	{
		Unhook();
	}

	protected void Unhook()
	{
		if (!m_Hooked)
			return;
		m_Hooked = false;
		if (INT_QST_Client.s_OnChanged)
			INT_QST_Client.s_OnChanged.Remove(OnDataChanged);
		if (INT_QST_Admin.s_OnReply)
			INT_QST_Admin.s_OnReply.Remove(OnAdminReply);
	}

	// --- data ----------------------------------------------------------------------------------------------------
	// s_OnChanged comes with every kill / progress packet: the board shows no progress, so a progress-only change
	// is skipped, the rest is refreshed from Update at most every DIRTY_GAP (OnDirtyRefresh).
	void OnDataChanged()
	{
		if (IsClosingDown())
			return;
		if (INT_QST_Client.s_ShapeRevision == m_ShapeSeen)
			return;
		MarkDirty();
	}

	override void OnDirtyRefresh()
	{
		if (IsClosingDown())
			return;
		m_ShapeSeen = INT_QST_Client.s_ShapeRevision;
		m_DataRev++;
		RebuildNodes();
		PrunePicked();
		m_ViewDirty = true;
		m_InspDirty = true;
		BuildToolbar();
	}

	// Positions: Board.json first, then automatic places for cards that have none.
	protected void RebuildNodes()
	{
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		AnalyzeLinks();
		m_LayoutRev++;
		INT_QST_AHello ah = INT_QST_Admin.s_Hello;
		if (ah)
		{
			// the server board is the same for every admin: its places win over automatic ones
			foreach (INT_QST_BoardNode bn : ah.Nodes)
			{
				INT_QST_BNode have;
				if (m_Nodes.Find(bn.Id, have))
				{
					if (!have.Moved)
					{
						have.X = bn.X;
						have.Y = bn.Y;
						have.Saved = true;
					}
					continue;
				}
				INT_QST_BNode n = new INT_QST_BNode();
				n.Id = bn.Id;
				n.X = bn.X;
				n.Y = bn.Y;
				n.Saved = true;
				m_Nodes.Set(bn.Id, n);
			}
		}
		// colours per branch
		m_BranchColor.Clear();
		int bi = 0;
		foreach (string bk : cat.m_BranchOrder)
		{
			m_BranchColor.Set(bk, Palette(bi));
			bi++;
		}
		// quests without a place: lay their branch out (only the missing cards move)
		map<string, bool> needs = new map<string, bool>();
		foreach (INT_QST_QuestDef q : cat.m_List)
		{
			if (!m_Nodes.Contains(q.Id))
				needs.Set(q.m_Branch, true);
		}
		if (needs.Count() > 0)
			AutoLayout(false);
	}

	static int Palette(int i)
	{
		int k = i - (i / 12) * 12;
		switch (k)
		{
			case 0:
				return 0xFFA3281E;
			case 1:
				return 0xFF1F3A6E;
			case 2:
				return 0xFF46622A;
			case 3:
				return 0xFF7A4A12;
			case 4:
				return 0xFF5B3A78;
			case 5:
				return 0xFF1F6A66;
			case 6:
				return 0xFFB5651D;
			case 7:
				return 0xFF4A5560;
			case 8:
				return 0xFF8A2F5A;
			case 9:
				return 0xFF2F6B3A;
			case 10:
				return 0xFF6E5A12;
		}
		return 0xFF2B4F8A;
	}

	protected int ColorOf(string branch)
	{
		int c = 0xFF4A3D2C;
		m_BranchColor.Find(branch, c);
		return c;
	}

	// Lanes (owner, 2026-10-22: "ветки горизонтально, квесты в длину"): every branch is a horizontal band, a chain of
	// «requires» goes to the right, a fork starts a new line under it, quests that open at once fill lines of
	// LANE_MAX cards like text. Requirements from another branch and gates do not move cards (they are lines).
	// all = true moves every card (button «Авто-раскладка»), false only places cards without a position.
	protected void AutoLayout(bool all)
	{
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		float bandTop = 0;
		foreach (string b : cat.m_BranchOrder)
		{
			array<INT_QST_QuestDef> list = new array<INT_QST_QuestDef>();
			foreach (INT_QST_QuestDef q : cat.m_List)
			{
				if (q.m_Branch == b)
					list.Insert(q);
			}
			if (list.Count() == 0)
				continue;
			map<int, int> rowOf = new map<int, int>();
			map<int, int> colOf = new map<int, int>();
			array<int> rowEnd = new array<int>();
			// in Order: a quest whose requirements in this lane are placed goes now, its chain follows it
			bool progress = true;
			int pass = 0;
			while (progress && pass < 60)
			{
				progress = false;
				pass++;
				foreach (INT_QST_QuestDef pq : list)
				{
					if (rowOf.Contains(pq.Id) || !LaneReady(pq, b, rowOf))
						continue;
					PlaceChain(pq, b, list, rowOf, colOf, rowEnd, 0);
					progress = true;
				}
			}
			// what is left sits in a loop of requirements: placed as quests that open at once
			foreach (INT_QST_QuestDef lq : list)
			{
				if (!rowOf.Contains(lq.Id))
					PlaceCell(lq, b, rowOf, colOf, rowEnd);
			}
			int rows = rowEnd.Count();
			if (rows < 1)
				rows = 1;
			foreach (INT_QST_QuestDef nq : list)
			{
				INT_QST_BNode n;
				bool exists = m_Nodes.Find(nq.Id, n);
				if (exists && !all)
					continue;
				if (!exists)
				{
					n = new INT_QST_BNode();
					n.Id = nq.Id;
					m_Nodes.Set(nq.Id, n);
				}
				int col = colOf.Get(nq.Id);
				int row = rowOf.Get(nq.Id);
				n.X = col * (NODE_W + COL_GAP);
				n.Y = bandTop + LANE_HEAD + row * (NODE_H + ROW_GAP);
				if (all)
				{
					n.Saved = true;
					n.Moved = true;
				}
			}
			// the next band starts under the lowest card of this branch (hand-placed ones included)
			float bottom = bandTop + LANE_HEAD + rows * (NODE_H + ROW_GAP);
			foreach (INT_QST_QuestDef bq : list)
			{
				INT_QST_BNode bn2;
				if (m_Nodes.Find(bq.Id, bn2) && bn2.Y >= bandTop && bn2.Y + NODE_H + ROW_GAP > bottom)
					bottom = bn2.Y + NODE_H + ROW_GAP;
			}
			bandTop = bottom + BAND_GAP;
		}
		m_LayoutRev++;
		m_ViewDirty = true;
	}

	// A requirement that shapes the lane: same branch, exists, not a gate, not the quest itself.
	protected bool LaneReq(INT_QST_QuestDef q, int r, string branch)
	{
		if (r == q.Id || IsHub(r))
			return false;
		INT_QST_QuestDef rq = INT_QST_Client.s_Catalog.Get(r);
		return rq && rq.m_Branch == branch;
	}

	protected bool LaneReady(INT_QST_QuestDef q, string branch, map<int, int> rowOf)
	{
		foreach (int r : q.Requires)
		{
			if (LaneReq(q, r, branch) && !rowOf.Contains(r))
				return false;
		}
		return true;
	}

	protected void PlaceChain(INT_QST_QuestDef q, string branch, array<INT_QST_QuestDef> list, map<int, int> rowOf, map<int, int> colOf, array<int> rowEnd, int guard)
	{
		PlaceCell(q, branch, rowOf, colOf, rowEnd);
		if (guard > 200 || IsHub(q.Id))
			return;
		foreach (INT_QST_QuestDef d : list)
		{
			if (rowOf.Contains(d.Id) || d.Requires.Find(q.Id) < 0)
				continue;
			if (LaneReady(d, branch, rowOf))
				PlaceChain(d, branch, list, rowOf, colOf, rowEnd, guard + 1);
		}
	}

	protected void PlaceCell(INT_QST_QuestDef q, string branch, map<int, int> rowOf, map<int, int> colOf, array<int> rowEnd)
	{
		// the requirement in this lane that sits furthest right
		int rr = -1;
		int rc = -1;
		foreach (int r : q.Requires)
		{
			if (!LaneReq(q, r, branch))
				continue;
			int rrow;
			if (!rowOf.Find(r, rrow))
				continue;
			int rcol = colOf.Get(r);
			if (rcol > rc)
			{
				rc = rcol;
				rr = rrow;
			}
		}
		int row;
		int col;
		if (rr >= 0)
		{
			int endR = rowEnd[rr];
			if (endR == rc + 1 && rc + 1 < LANE_MAX * 2)
			{
				// the chain goes on to the right
				row = rr;
				col = rc + 1;
			}
			else
			{
				// a fork (or a very long chain): a new line, still right of the requirement
				row = rowEnd.Count();
				col = rc + 1;
				if (col >= LANE_MAX * 2)
					col = 0;
				rowEnd.Insert(0);
			}
		}
		else
		{
			int last = rowEnd.Count() - 1;
			int endL = 0;
			if (last >= 0)
				endL = rowEnd[last];
			if (last >= 0 && endL < LANE_MAX)
			{
				row = last;
				col = endL;
			}
			else
			{
				row = rowEnd.Count();
				col = 0;
				rowEnd.Insert(0);
			}
		}
		rowOf.Set(q.Id, row);
		colOf.Set(q.Id, col);
		int endNow = rowEnd[row];
		if (col + 1 > endNow)
			rowEnd.Set(row, col + 1);
	}

	// Who requires whom: counts and gates (on every catalog change).
	protected void AnalyzeLinks()
	{
		m_DepCount.Clear();
		m_Hub.Clear();
		foreach (INT_QST_QuestDef q : INT_QST_Client.s_Catalog.m_List)
		{
			foreach (int r : q.Requires)
			{
				if (r == q.Id)
					continue;
				int n = 0;
				m_DepCount.Find(r, n);
				m_DepCount.Set(r, n + 1);
			}
		}
		foreach (int id, int cnt : m_DepCount)
		{
			if (cnt >= HUB_MIN)
				m_Hub.Set(id, true);
		}
	}

	protected bool IsHub(int id)
	{
		return m_Hub.Contains(id);
	}

	// The quests a gate opens, grouped by branch: the first card of each group (top, then left) and the count.
	protected void HubGroups(int hid, bool filtered)
	{
		m_GroupBranch.Clear();
		m_GroupFirst.Clear();
		m_GroupCount.Clear();
		foreach (INT_QST_QuestDef q : INT_QST_Client.s_Catalog.m_List)
		{
			if (q.Id == hid || q.Requires.Find(hid) < 0)
				continue;
			if (filtered && !InFilter(q))
				continue;
			INT_QST_BNode nd;
			if (!m_Nodes.Find(q.Id, nd))
				continue;
			int gi = m_GroupBranch.Find(q.m_Branch);
			if (gi < 0)
			{
				m_GroupBranch.Insert(q.m_Branch);
				m_GroupFirst.Insert(q.Id);
				m_GroupCount.Insert(1);
				continue;
			}
			int cnt = m_GroupCount[gi];
			m_GroupCount.Set(gi, cnt + 1);
			int fid = m_GroupFirst[gi];
			INT_QST_BNode fn;
			if (!m_Nodes.Find(fid, fn))
				continue;
			bool higher = nd.Y < fn.Y - 1;
			bool sameRowLeft = Math.AbsFloat(nd.Y - fn.Y) <= 1 && nd.X < fn.X;
			if (higher || sameRowLeft)
				m_GroupFirst.Set(gi, q.Id);
		}
	}

	protected bool InFilter(INT_QST_QuestDef q)
	{
		return m_Branch == "" || q.m_Branch == m_Branch;
	}

	// --- view -------------------------------------------------------------------------------------------------------
	protected float SX(float wx)
	{
		return (wx - m_PanX) * m_Z;
	}

	protected float SY(float wy)
	{
		return (wy - m_PanY) * m_Z;
	}

	// Mouse in field design px; false when outside the field.
	protected bool MouseField(out float fx, out float fy)
	{
		int mx;
		int my;
		GetMousePos(mx, my);
		float cx;
		float cy;
		m_Field.GetScreenPos(cx, cy);
		float k = INT_UIF_Scale.Factor();
		fx = (mx - cx) / k;
		fy = (my - cy) / k;
		return fx >= 0 && fy >= 0 && fx <= m_CW && fy <= m_CH;
	}

	protected void Fit()
	{
		float minX = 1000000;
		float minY = 1000000;
		float maxX = -1000000;
		float maxY = -1000000;
		int n = 0;
		foreach (INT_QST_QuestDef q : INT_QST_Client.s_Catalog.m_List)
		{
			if (!InFilter(q))
				continue;
			INT_QST_BNode nd;
			if (!m_Nodes.Find(q.Id, nd))
				continue;
			n++;
			if (nd.X < minX)
				minX = nd.X;
			if (nd.Y < minY)
				minY = nd.Y;
			if (nd.X + NODE_W > maxX)
				maxX = nd.X + NODE_W;
			if (nd.Y + NODE_H > maxY)
				maxY = nd.Y + NODE_H;
		}
		if (n == 0)
		{
			m_Z = 0.6;
			m_PanX = -40;
			m_PanY = -40;
			m_ViewDirty = true;
			return;
		}
		// screen margins: lines that come down left of the lanes, the branch name over the top lane
		float mL = 90;
		float mR = 40;
		float mT = 20;
		float mB = 30;
		float top = minY - LANE_HEAD;
		float bw = Math.Max(1, maxX - minX);
		float bh = Math.Max(1, maxY - top);
		float aw = m_CW - mL - mR;
		float ahh = m_CH - mT - mB;
		float z = aw / bw;
		float zy = ahh / bh;
		if (zy < z)
			z = zy;
		m_Z = Math.Clamp(z, ZMIN, 1.0);
		m_PanX = minX - (mL + (aw - bw * m_Z) * 0.5) / m_Z;
		m_PanY = top - (mT + (ahh - bh * m_Z) * 0.5) / m_Z;
		m_ViewDirty = true;
	}

	protected void ZoomAt(float factor, float fx, float fy)
	{
		float wx = fx / m_Z + m_PanX;
		float wy = fy / m_Z + m_PanY;
		m_Z = Math.Clamp(m_Z * factor, ZMIN, ZMAX);
		m_PanX = wx - fx / m_Z;
		m_PanY = wy - fy / m_Z;
		m_ViewDirty = true;
		BuildToolbar();
	}

	protected void CenterOn(int id)
	{
		INT_QST_BNode nd;
		if (!m_Nodes.Find(id, nd))
			return;
		if (m_Z < 0.55)
			m_Z = 0.8;
		m_PanX = nd.X + NODE_W * 0.5 - m_CW * 0.5 / m_Z;
		m_PanY = nd.Y + NODE_H * 0.5 - m_CH * 0.5 / m_Z;
		m_ViewDirty = true;
		BuildToolbar();
	}

	// level of detail: 2 full card, 1 compact card, 0 stroke on the canvas
	protected int Lod()
	{
		if (m_Z >= 0.55)
			return 2;
		if (m_Z >= 0.26)
			return 1;
		return 0;
	}

	// --- drawing ------------------------------------------------------------------------------------------------------
	protected void Redraw()
	{
		m_ViewDirty = false;
		int lod = Lod();
		float w = NODE_W * m_Z;
		float h = NODE_H * m_Z;
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		if (m_Lines)
			m_Lines.Clear();
		m_LinesFull = false;
		// lines first, but part of the budget stays for the cards drawn as strokes
		m_LineBudget = LINE_BUDGET - STROKE_RESERVE;
		DrawEdges();
		m_LineBudget = m_LineBudget + STROKE_RESERVE;
		m_Visible.Clear();
		m_Keep.Clear();
		foreach (INT_QST_QuestDef q : cat.m_List)
		{
			if (!InFilter(q))
				continue;
			INT_QST_BNode nd;
			if (!m_Nodes.Find(q.Id, nd))
				continue;
			float sx = SX(nd.X);
			float sy = SY(nd.Y);
			if (sx + w < 0 || sy + h < 0 || sx > m_CW || sy > m_CH)
				continue;
			m_Visible.Insert(q.Id);
			// the quests that come from the selected one are lit (owner: simpler than lines and counters)
			bool lit = m_Sel != 0 && m_Picked.Count() <= 1 && q.Requires.Find(m_Sel) >= 0;
			if (lod == 0)
			{
				DrawStroke(q, sx, sy, w, h, lit);
				continue;
			}
			m_Keep.Set(q.Id, true);
			bool full = sx >= 0 && sy >= 0 && sx + w <= m_CW && sy + h <= m_CH;
			INT_QST_BSlot slot = SlotFor(q.Id);
			if (!slot)
				continue;
			DrawCard(slot, q, lod, sx, sy, w, h, full, lit);
		}
		// free the slots of cards that left the view
		m_Free.Clear();
		foreach (int sid, INT_QST_BSlot s : m_SlotOf)
		{
			if (!m_Keep.Contains(sid))
				m_Free.Insert(sid);
		}
		foreach (int fid : m_Free)
		{
			INT_QST_BSlot fs = m_SlotOf.Get(fid);
			m_SlotOf.Remove(fid);
			fs.Id = 0;
			fs.Root.Show(false);
		}
		DrawLabels();
		DrawMini();
		if (m_LinesFull && !m_FullLogged)
		{
			m_FullLogged = true;
			int lb = LINE_BUDGET;
			INT_QST_Log.Dbg("схема: линий больше " + lb.ToString() + " за кадр, лишние не рисуются");
		}
	}

	protected INT_QST_BSlot SlotFor(int id)
	{
		INT_QST_BSlot s;
		if (m_SlotOf.Find(id, s))
			return s;
		foreach (INT_QST_BSlot c : m_Slots)
		{
			if (c.Id == 0)
			{
				c.Id = id;
				c.Rev = -1;
				m_SlotOf.Set(id, c);
				return c;
			}
		}
		if (m_Slots.Count() >= 600)
			return null;
		INT_QST_BSlot ns = new INT_QST_BSlot();
		ns.Root = INT_UIF_Draw.Frame(m_NodesLayer, 0, 0, NODE_W, NODE_H, "Card");
		ns.Id = id;
		m_Slots.Insert(ns);
		m_SlotOf.Set(id, ns);
		return ns;
	}

	// A card at the overview level: one thick stroke on the canvas (no widgets).
	protected void DrawStroke(INT_QST_QuestDef q, float sx, float sy, float w, float h, bool lit)
	{
		int c = ColorOf(q.m_Branch);
		if (q.Enabled == 0)
			c = 0xFF9A8D72;
		if (lit)
			c = C_LIT;
		if (m_Picked.Contains(q.Id))
			c = INT_QST_Look.Blue();
		float k = INT_UIF_Scale.Factor();
		float x1 = sx;
		float x2 = sx + w;
		float y = sy + h * 0.5;
		float th = h;
		if (th < 2)
			th = 2;
		Seg(x1 * k, y * k, x2 * k, y * k, th * k, c);
	}

	protected void DrawCard(INT_QST_BSlot slot, INT_QST_QuestDef q, int lod, float sx, float sy, float w, float h, bool full, bool lit)
	{
		INT_UIF_Draw.Place(slot.Root, sx, sy, w, h);
		slot.Root.Show(true);
		bool sel = m_Picked.Contains(q.Id);
		bool linking = m_LinkMode && q.Id == m_LinkFrom;
		if (slot.Lod == lod && slot.Z == m_Z && slot.Full == full && slot.Sel == sel && slot.Linking == linking && slot.Lit == lit && slot.Rev == m_DataRev)
			return;
		slot.Lit = lit;
		slot.Lod = lod;
		slot.Z = m_Z;
		slot.Full = full;
		slot.Sel = sel;
		slot.Linking = linking;
		slot.Rev = m_DataRev;
		Widget r = slot.Root;
		INT_UIF_Widgets.ClearChildren(r);
		float z = m_Z;
		bool hub = IsHub(q.Id);
		int fill = INT_QST_Look.Note();
		int edge = INT_QST_Look.InkA(120);
		if (q.Enabled == 0)
			fill = 0xFFD8CDB2;
		if (hub)
			edge = C_GATE;
		if (lit)
		{
			fill = 0xFFF8E6A0;
			edge = C_LIT;
		}
		if (sel)
			edge = INT_QST_Look.Blue();
		if (linking)
			edge = INT_QST_Look.Red();
		INT_UIF_Surface.Box(r, 0, 0, w, h, 6, fill, edge);
		if (sel || linking || hub || lit)
			INT_UIF_Surface.Box(r, 1, 1, w - 2, h - 2, 6, 0, edge);
		int bc = ColorOf(q.m_Branch);
		INT_UIF_Draw.Line(r, 0, 3 * z, 5 * z, h - 6 * z, bc);
		if (!full)
			return;
		int ink = INT_QST_Look.Ink();
		int ink3 = INT_QST_Look.Ink3();
		if (q.Enabled == 0)
			ink = INT_QST_Look.Ink3();
		if (lod == 1)
		{
			// readable at the middle zoom: the name grows faster than the card (it only has one line here)
			float cs = Math.Clamp(30 * z, 10, 15);
			string nm1 = INT_QST_Look.Fit(q.Name, w - 18 * z, cs);
			INT_QST_Look.Heavy(r, 12 * z, 0, w - 16 * z, h, nm1, cs, ink);
			return;
		}
		// full card: "NN #id", name, targets · limit · repeat (a gate: how many quests it opens)
		string nn = q.Order.ToString();
		if (q.Order < 10)
			nn = "0" + nn;
		INT_QST_Look.Heavy(r, 14 * z, 5 * z, 40 * z, 18 * z, nn, 12 * z, bc);
		string idt = "#" + q.Id.ToString();
		INT_QST_Look.Text(r, 44 * z, 5 * z, 120 * z, 18 * z, idt, 11 * z, ink3);
		if (q.Enabled == 0)
		{
			string draft = INT_QST_Look.Tr("#STR_INT_QST_B_OFF");
			INT_QST_Look.Text(r, w - 112 * z, 5 * z, 100 * z, 18 * z, draft, 11 * z, INT_QST_Look.Red(), INT_UIF_Align.RIGHT);
		}
		else if (hub)
		{
			string gateTr = INT_QST_Look.Tr("#STR_INT_QST_B_GATE");
			string gate = INT_QST_Look.Caps(gateTr);		// string.ToUpper leaves Cyrillic as it is
			INT_QST_Look.Heavy(r, w - 112 * z, 5 * z, 100 * z, 18 * z, gate, 11 * z, C_GATE, INT_UIF_Align.RIGHT);
		}
		else
		{
			int dot = INT_QST_Look.Olive();
			INT_UIF_Draw.Line(r, w - 18 * z, 10 * z, 7 * z, 7 * z, dot);
		}
		string nm = INT_QST_Look.Fit(q.Name, w - 26 * z, 15 * z);
		INT_QST_Look.Heavy(r, 14 * z, 22 * z, w - 22 * z, 24 * z, nm, 15 * z, ink);
		float mx = 14 * z;
		INT_QST_Look.Icon(r, mx, 50 * z, 13 * z, "crosshair", ink3);
		string tc = q.Targets.Count().ToString();
		INT_QST_Look.Text(r, mx + 17 * z, 46 * z, 30 * z, 20 * z, tc, 12 * z, ink3);
		mx = mx + 46 * z;
		if (q.TimeLimit > 0)
		{
			INT_QST_Look.Icon(r, mx, 50 * z, 13 * z, "clock", ink3);
			string lt = INT_QST_Look.Dur(q.TimeLimit);
			INT_QST_Look.Text(r, mx + 17 * z, 46 * z, 60 * z, 20 * z, lt, 12 * z, ink3);
			mx = mx + 74 * z;
		}
		if (hub)
		{
			int dn = 0;
			m_DepCount.Find(q.Id, dn);
			string dnT = dn.ToString();
			string opens = INT_QST_Look.Fmt("#STR_INT_QST_B_GATE_CARD", dnT);
			INT_QST_Look.Heavy(r, w - 150 * z, 46 * z, 138 * z, 20 * z, opens, 12 * z, C_GATE, INT_UIF_Align.RIGHT);
		}
		else
		{
			string rep = RepeatShort(q);
			INT_QST_Look.Text(r, w - 120 * z, 46 * z, 108 * z, 20 * z, rep, 12 * z, ink3, INT_UIF_Align.RIGHT);
		}
		if (HasProblem(q))
			INT_QST_Look.Icon(r, w - 22 * z, 26 * z, 15 * z, "warning", INT_QST_Look.Red());
	}

	protected string RepeatShort(INT_QST_QuestDef q)
	{
		switch (q.m_Repeat)
		{
			case INT_QST_RM.COOLDOWN:
				return INT_QST_Look.Dur(q.Cooldown);
			case INT_QST_RM.DAILY:
				return INT_QST_Look.Tr("#STR_INT_QST_B_DAILY");
			case INT_QST_RM.WEEKLY:
				return INT_QST_Look.Tr("#STR_INT_QST_B_WEEKLY");
		}
		return INT_QST_Look.Tr("#STR_INT_QST_B_ONCE");
	}

	// Requires that point at nothing, givers that do not exist, no targets.
	protected bool HasProblem(INT_QST_QuestDef q)
	{
		foreach (int r : q.Requires)
		{
			if (!INT_QST_Client.s_Catalog.Get(r))
				return true;
		}
		if (q.GiverStart != "" && !INT_QST_Client.Giver(q.GiverStart))
			return true;
		if (q.GiverFinish != "" && !INT_QST_Client.Giver(q.GiverFinish))
			return true;
		return q.Targets.Count() == 0;
	}

	// Lines: elbow from the right side of the required card to the left side of the card that needs it. A gate has
	// no lines (owner: a start quest needed by 313 quests drew a wall of them); its own line to a card is drawn only
	// for the selected card, and selecting the gate lights the cards it opens.
	protected void DrawEdges()
	{
		if (!m_Lines)
			return;
		float k = INT_UIF_Scale.Factor();
		float z = m_Z;
		float th = 2.0 * k;
		if (z < 0.4)
			th = 1.0 * k;
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		foreach (INT_QST_QuestDef q : cat.m_List)
		{
			if (!InFilter(q))
				continue;
			INT_QST_BNode to;
			if (!m_Nodes.Find(q.Id, to))
				continue;
			foreach (int r : q.Requires)
			{
				if (IsHub(r) && q.Id != m_Sel)
					continue;
				INT_QST_QuestDef rq = cat.Get(r);
				if (!rq || !InFilter(rq))
					continue;
				INT_QST_BNode from;
				if (!m_Nodes.Find(r, from))
					continue;
				int c = INT_QST_Look.InkA(150);
				float w = th;
				bool dashed = rq.m_Branch != q.m_Branch;
				if (dashed)
					c = ARGB(170, 31, 58, 110);
				if (q.Id == m_Sel || r == m_Sel)
				{
					c = INT_QST_Look.Red();
					w = th * 1.6;
				}
				float x1 = SX(from.X + NODE_W) * k;
				float y1 = SY(from.Y + NODE_H * 0.5) * k;
				float x2 = SX(to.X) * k;
				float y2 = SY(to.Y + NODE_H * 0.5) * k;
				Edge(x1, y1, x2, y2, w, c, dashed);
			}
		}
	}

	protected void Edge(float x1, float y1, float x2, float y2, float w, int c, bool dashed)
	{
		float k = INT_UIF_Scale.Factor();
		float g = 18 * m_Z * k;
		if (g < 6)
			g = 6;
		float xl = x1 - NODE_W * m_Z * k;		// left side of the start card
		if (x2 > x1 + g * 2)
		{
			if (Math.AbsFloat(y2 - y1) < 1)
				Line3(x1, y1, x2, y2, w, c, dashed);		// neighbours in a row: one segment
			else
			{
				float xm = (x1 + x2) * 0.5;
				Line3(x1, y1, xm, y1, w, c, dashed);
				Line3(xm, y1, xm, y2, w, c, dashed);
				Line3(xm, y2, x2, y2, w, c, dashed);
			}
		}
		else if (xl <= x2 + g)
		{
			// the target is under the start (a gate over the lanes): out of the left side, down the margin left of
			// the lanes, in from the left — the line crosses no card and no branch name
			float xb = xl - g;
			if (x2 - g < xb)
				xb = x2 - g;
			Line3(xl, y1, xb, y1, w, c, dashed);
			Line3(xb, y1, xb, y2, w, c, dashed);
			Line3(xb, y2, x2, y2, w, c, dashed);
		}
		else
		{
			// the target is left of the start (a chain wrapped to the next line): out to the right, along the gap
			// over the target, in from the left
			float xa = x1 + g;
			float xc = x2 - g;
			float off = (NODE_H * 0.5 + ROW_GAP * 0.5) * m_Z * k;
			float ym = y2 - off;
			if (y2 < y1)
				ym = y2 + off;
			Line3(x1, y1, xa, y1, w, c, dashed);
			Line3(xa, y1, xa, ym, w, c, dashed);
			Line3(xa, ym, xc, ym, w, c, dashed);
			Line3(xc, ym, xc, y2, w, c, dashed);
			Line3(xc, y2, x2, y2, w, c, dashed);
		}
		// arrow head at the target (too small to see on the overview)
		if (m_Z < 0.3)
			return;
		float a = 7 * k * Math.Clamp(m_Z, 0.4, 1.2);
		Seg(x2 - a, y2 - a * 0.6, x2, y2, w, c);
		Seg(x2 - a, y2 + a * 0.6, x2, y2, w, c);
	}

	// A dashed line is cut only along its visible part, at most DASH_MAX dashes (longer dashes past that); the
	// dash phase stays tied to the line start so the dashes do not crawl while the map moves.
	protected void Line3(float x1, float y1, float x2, float y2, float w, int c, bool dashed)
	{
		if (!dashed)
		{
			Seg(x1, y1, x2, y2, w, c);
			return;
		}
		float dx = x2 - x1;
		float dy = y2 - y1;
		float len = Math.Sqrt(dx * dx + dy * dy);
		if (len < 1)
			return;
		float t0;
		float t1;
		if (!ClipT(x1, y1, x2, y2, t0, t1))
			return;
		float k = INT_UIF_Scale.Factor();
		float on = 9 * k;
		float off = 6 * k;
		float a = t0 * len;
		float b = t1 * len;
		float n = (b - a) / (on + off);
		if (n > DASH_MAX)
		{
			float s = n / DASH_MAX;
			on = on * s;
			off = off * s;
		}
		float period = on + off;
		float t = Math.Floor(a / period) * period;
		int guard = 0;
		while (t < b && guard < DASH_MAX + 4)
		{
			float s1 = t;
			if (s1 < a)
				s1 = a;
			float s2 = t + on;
			if (s2 > b)
				s2 = b;
			if (s2 > s1)
				Put(x1 + dx * s1 / len, y1 + dy * s1 / len, x1 + dx * s2 / len, y1 + dy * s2 / len, w, c);
			t = t + period;
			guard++;
		}
	}

	// One segment clipped to the field (canvas px, local to the canvas).
	protected void Seg(float x1, float y1, float x2, float y2, float w, int c)
	{
		float t0;
		float t1;
		if (!ClipT(x1, y1, x2, y2, t0, t1))
			return;
		float dx = x2 - x1;
		float dy = y2 - y1;
		Put(x1 + dx * t0, y1 + dy * t0, x1 + dx * t1, y1 + dy * t1, w, c);
	}

	// Every canvas line goes through here: past the budget nothing more is drawn this frame.
	protected void Put(float x1, float y1, float x2, float y2, float w, int c)
	{
		if (!m_Lines)
			return;
		if (m_LineBudget <= 0)
		{
			m_LinesFull = true;
			return;
		}
		m_LineBudget--;
		m_Lines.DrawLine(x1, y1, x2, y2, w, c);
	}

	// Liang-Barsky: the visible part [t0, t1] of a line inside the field; false when nothing is visible.
	protected bool ClipT(float x1, float y1, float x2, float y2, out float t0, out float t1)
	{
		float k = INT_UIF_Scale.Factor();
		float maxX = m_CW * k;
		float maxY = m_CH * k;
		t0 = 0;
		t1 = 1;
		float dx = x2 - x1;
		float dy = y2 - y1;
		if (!ClipEdge(-dx, x1, t0, t1))
			return false;
		if (!ClipEdge(dx, maxX - x1, t0, t1))
			return false;
		if (!ClipEdge(-dy, y1, t0, t1))
			return false;
		if (!ClipEdge(dy, maxY - y1, t0, t1))
			return false;
		return t1 > t0;
	}

	// Liang-Barsky step.
	protected bool ClipEdge(float p, float q, inout float t0, inout float t1)
	{
		if (p == 0)
			return q >= 0;
		float r = q / p;
		if (p < 0)
		{
			if (r > t1)
				return false;
			if (r > t0)
				t0 = r;
		}
		else
		{
			if (r < t0)
				return false;
			if (r < t1)
				t1 = r;
		}
		return true;
	}

	// Branch names over their lanes. Pooled: a name is rebuilt only when its text or size changes, otherwise only
	// moved (it was rebuilt on every frame of a drag).
	protected void DrawLabels()
	{
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		map<string, vector> top = new map<string, vector>();
		map<string, int> count = new map<string, int>();
		foreach (INT_QST_QuestDef q : cat.m_List)
		{
			if (!InFilter(q))
				continue;
			INT_QST_BNode nd;
			if (!m_Nodes.Find(q.Id, nd))
				continue;
			int cnt = 0;
			count.Find(q.m_Branch, cnt);
			count.Set(q.m_Branch, cnt + 1);
			vector have;
			if (!top.Find(q.m_Branch, have))
				top.Set(q.m_Branch, Vector(nd.X, nd.Y, 0));
			else
			{
				if (nd.X < have[0])
					have[0] = nd.X;
				if (nd.Y < have[1])
					have[1] = nd.Y;
				top.Set(q.m_Branch, have);
			}
		}
		float size = 16 * m_Z * 1.2;
		if (size < 11)
			size = 11;
		if (size > 18)
			size = 18;
		foreach (string lk, Widget lw : m_LabelOf)
			lw.Show(false);
		m_LabelHitKey.Clear();
		m_LabelHitX.Clear();
		m_LabelHitY.Clear();
		m_LabelHitW.Clear();
		m_LabelHitH.Clear();
		foreach (string b, vector p : top)
		{
			float lx = SX(p[0]);
			float ly = SY(p[1]) - size - 12;
			if (lx > m_CW || ly > m_CH || ly < -size || lx < -400)
				continue;
			string title = cat.BranchTitle(b);
			int n = 0;
			count.Find(b, n);
			string nText = n.ToString();
			string raw = title + "  ·  " + nText;
			string t = INT_QST_Look.Caps(raw);		// admin titles are Cyrillic: string.ToUpper skips them
			string sizeText = size.ToString();
			string key = t + "|" + sizeText;
			Widget f;
			if (!m_LabelOf.Find(b, f))
			{
				f = INT_UIF_Draw.Frame(m_LabelsLayer, 0, 0, 10, 10, "Label");
				m_LabelOf.Set(b, f);
			}
			string had;
			m_LabelKey.Find(b, had);
			if (had != key)
			{
				INT_UIF_Widgets.ClearChildren(f);
				float tw = INT_UIF_Draw.TextWidth(t, size) * 1.05 + 30;
				int c = ColorOf(b);
				INT_UIF_Draw.Line(f, 0, size * 0.5 - 2, 10, 10, c);
				INT_QST_Look.Heavy(f, 18, 0, tw, size + 8, t, size, c);
				m_LabelKey.Set(b, key);
				m_LabelW.Set(b, tw);
			}
			if (lx < 0)
				lx = 0;
			float fw = m_LabelW.Get(b);
			INT_UIF_Draw.Place(f, lx, ly, fw, size + 8);
			f.Show(true);
			m_LabelHitKey.Insert(b);
			m_LabelHitX.Insert(lx);
			m_LabelHitY.Insert(ly);
			m_LabelHitW.Insert(fw);
			m_LabelHitH.Insert(size + 8);
		}
	}

	// The minimap: every card of the filter as a small stroke (drawn again only when the cards or the filter
	// change), the view as a frame on its own canvas. Click = center there.
	protected void DrawMini()
	{
		if (!m_MiniLines)
			return;
		float minX;
		float minY;
		float maxX;
		float maxY;
		if (!Bounds(minX, minY, maxX, maxY))
		{
			m_MiniLines.Clear();
			if (m_MiniView)
				m_MiniView.Clear();
			m_MiniKey = "";
			return;
		}
		float k = INT_UIF_Scale.Factor();
		float sc = MiniScale(minX, minY, maxX, maxY);
		float ox = (MINI_W - (maxX - minX) * sc) * 0.5;
		float oy = (MINI_H - (maxY - minY) * sc) * 0.5;
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		string mk = m_DataRev.ToString() + "|" + m_LayoutRev.ToString() + "|" + m_Branch;
		if (mk != m_MiniKey)
		{
			m_MiniKey = mk;
			m_MiniLines.Clear();
			foreach (INT_QST_QuestDef q : cat.m_List)
			{
				if (!InFilter(q))
					continue;
				INT_QST_BNode nd;
				if (!m_Nodes.Find(q.Id, nd))
					continue;
				float x1 = (ox + (nd.X - minX) * sc) * k;
				float x2 = (ox + (nd.X + NODE_W - minX) * sc) * k;
				float y = (oy + (nd.Y + NODE_H * 0.5 - minY) * sc) * k;
				float th = NODE_H * sc * k;
				if (th < 1.5)
					th = 1.5;
				int c = ColorOf(q.m_Branch);
				m_MiniLines.DrawLine(x1, y, x2, y, th, c);
			}
		}
		if (!m_MiniView)
			return;
		m_MiniView.Clear();
		float vx1 = (ox + (m_PanX - minX) * sc) * k;
		float vy1 = (oy + (m_PanY - minY) * sc) * k;
		float vx2 = (ox + (m_PanX + m_CW / m_Z - minX) * sc) * k;
		float vy2 = (oy + (m_PanY + m_CH / m_Z - minY) * sc) * k;
		// the frame stays inside the minimap when the view is bigger than the map
		vx1 = Math.Clamp(vx1, 1, MINI_W * k - 1);
		vx2 = Math.Clamp(vx2, 1, MINI_W * k - 1);
		vy1 = Math.Clamp(vy1, 1, MINI_H * k - 1);
		vy2 = Math.Clamp(vy2, 1, MINI_H * k - 1);
		int vc = INT_QST_Look.Ink();
		m_MiniView.DrawLine(vx1, vy1, vx2, vy1, 1.5 * k, vc);
		m_MiniView.DrawLine(vx2, vy1, vx2, vy2, 1.5 * k, vc);
		m_MiniView.DrawLine(vx2, vy2, vx1, vy2, 1.5 * k, vc);
		m_MiniView.DrawLine(vx1, vy2, vx1, vy1, 1.5 * k, vc);
	}

	protected bool Bounds(out float minX, out float minY, out float maxX, out float maxY)
	{
		minX = 1000000;
		minY = 1000000;
		maxX = -1000000;
		maxY = -1000000;
		int n = 0;
		foreach (INT_QST_QuestDef q : INT_QST_Client.s_Catalog.m_List)
		{
			if (!InFilter(q))
				continue;
			INT_QST_BNode nd;
			if (!m_Nodes.Find(q.Id, nd))
				continue;
			n++;
			if (nd.X < minX)
				minX = nd.X;
			if (nd.Y < minY)
				minY = nd.Y;
			if (nd.X + NODE_W > maxX)
				maxX = nd.X + NODE_W;
			if (nd.Y + NODE_H > maxY)
				maxY = nd.Y + NODE_H;
		}
		return n > 0;
	}

	protected float MiniScale(float minX, float minY, float maxX, float maxY)
	{
		float sx = (MINI_W - 16) / Math.Max(1, maxX - minX);
		float sy = (MINI_H - 16) / Math.Max(1, maxY - minY);
		if (sy < sx)
			return sy;
		return sx;
	}

	void MiniDown()
	{
		float minX;
		float minY;
		float maxX;
		float maxY;
		if (!Bounds(minX, minY, maxX, maxY))
			return;
		int mx;
		int my;
		GetMousePos(mx, my);
		float px;
		float py;
		m_Mini.GetScreenPos(px, py);
		float k = INT_UIF_Scale.Factor();
		float lx = (mx - px) / k;
		float ly = (my - py) / k;
		float sc = MiniScale(minX, minY, maxX, maxY);
		float ox = (MINI_W - (maxX - minX) * sc) * 0.5;
		float oy = (MINI_H - (maxY - minY) * sc) * 0.5;
		float wx = (lx - ox) / sc + minX;
		float wy = (ly - oy) / sc + minY;
		m_PanX = wx - m_CW * 0.5 / m_Z;
		m_PanY = wy - m_CH * 0.5 / m_Z;
		m_ViewDirty = true;
	}

	// --- inspector ------------------------------------------------------------------------------------------------------
	protected void BuildInspector()
	{
		m_InspDirty = false;
		m_Clicks.ForgetUnder(m_Insp);
		INT_UIF_Widgets.ClearChildren(m_Insp);
		float w = INSP_W;
		float h = m_CH;
		INT_UIF_Surface.Shadow(m_Insp, -18, -10, w + 36, h + 34, 0x40000000);
		INT_QST_Look.PaperSheet(m_Insp, 0, 0, w, h, INT_QST_Look.Note(), "Card");
		if (m_Picked.Count() > 1)
		{
			InspectorGroup(w);
			return;
		}
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(m_Sel);
		if (!q)
		{
			InspectorHelp(w);
			return;
		}
		float x = 20;
		float iw = w - 40;
		float y = 16;
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_B_QUEST") + "   ID " + q.Id.ToString();
		INT_QST_Look.Heavy(m_Insp, x, y, iw, 20, cap, 12, INT_QST_Look.Ink3());
		string sk = "#STR_INT_QST_B_ON";
		int sc = INT_QST_Look.Olive();
		if (q.Enabled == 0)
		{
			sk = "#STR_INT_QST_B_OFF";
			sc = INT_QST_Look.Red();
		}
		float stw = INT_QST_Look.StampWidth(sk, 10);
		INT_QST_Look.Stamp(m_Insp, w - 20 - stw, y - 2, sk, sc, 10);
		y = y + 28;
		array<string> lines = new array<string>();
		INT_QST_Text.Wrap(q.Name, iw, 19, lines);
		int k = 0;
		foreach (string ln : lines)
		{
			if (k >= 2)
				break;
			INT_QST_Look.Heavy(m_Insp, x, y, iw, 28, ln, 19, INT_QST_Look.Ink());
			y = y + 26;
			k++;
		}
		y = y + 4;
		int bc = ColorOf(q.m_Branch);
		INT_UIF_Draw.Line(m_Insp, x, y + 6, 10, 10, bc);
		int place;
		int total;
		INT_QST_Client.s_Catalog.BranchPlace(q, place, total);
		string bt = INT_QST_Client.s_Catalog.BranchTitle(q.m_Branch);
		string bline = bt + "  ·  " + place.ToString() + " / " + total.ToString();
		bline = INT_QST_Look.Fit(bline, iw - 18, 14);
		INT_QST_Look.Heavy(m_Insp, x + 18, y, iw - 18, 22, bline, 14, bc);
		y = y + 24;
		string file = FileOf(q.Id);
		if (file != "")
		{
			INT_QST_Look.Icon(m_Insp, x, y + 3, 13, "letter", INT_QST_Look.Ink3());
			string ff = INT_QST_Look.Fit(file, iw - 20, 12);
			INT_QST_Look.Text(m_Insp, x + 20, y, iw - 20, 20, ff, 12, INT_QST_Look.Ink3());
			y = y + 22;
		}
		string ntT = q.Targets.Count().ToString();
		string nrT = q.Rewards.Count().ToString();
		string sum = INT_QST_Look.Fmt("#STR_INT_QST_B_SUMMARY", ntT, nrT);
		INT_QST_Look.Text(m_Insp, x, y, iw, 22, sum, 13, INT_QST_Look.Ink2());
		y = y + 24;
		if (IsHub(q.Id))
		{
			int dn = 0;
			m_DepCount.Find(q.Id, dn);
			HubGroups(q.Id, false);
			string dnT = dn.ToString();
			string gbT = m_GroupBranch.Count().ToString();
			string gl = INT_QST_Look.Fmt("#STR_INT_QST_B_GATE_OPENS", dnT, gbT);
			gl = INT_QST_Look.Fit(gl, iw, 13);
			INT_QST_Look.Heavy(m_Insp, x, y, iw, 22, gl, 13, C_GATE);
			y = y + 24;
		}
		y = y + 4;
		INT_QST_Look.Hair(m_Insp, x, y, iw);
		y = y + 8;
		y = InspLine(x, y, iw, "#STR_INT_QST_I_GIVES", INT_QST_Text.StartText(q));
		y = InspLine(x, y, iw, "#STR_INT_QST_I_TURNIN", INT_QST_Text.FinishText(q));
		y = InspLine(x, y, iw, "#STR_INT_QST_I_REPEAT", INT_QST_Text.Repeat(q));
		y = InspLine(x, y, iw, "#STR_INT_QST_I_LIMIT", INT_QST_Text.Limit(q));
		y = y + 6;
		INT_QST_Look.Hair(m_Insp, x, y, iw);
		y = y + 10;
		// requires
		string rc = INT_QST_Look.Spaced("#STR_INT_QST_B_REQUIRES");
		INT_QST_Look.Heavy(m_Insp, x, y, iw - 100, 22, rc, 12, INT_QST_Look.Ink());
		string mode = "#STR_INT_QST_B_REQ_ALL";
		if (q.RequireAny == 1)
			mode = "#STR_INT_QST_B_REQ_ANY";
		string mt = INT_QST_Look.Tr(mode);
		float mw = INT_UIF_Draw.TextWidth(mt, 12) * 1.1 + 18;
		Widget mb = INT_UIF_Draw.Frame(m_Insp, w - 20 - mw, y - 2, mw, 26, "Mode");
		int mf = INT_QST_Look.Ink();
		INT_UIF_Surface.Box(mb, 0, 0, mw, 26, 4, mf, mf);
		INT_QST_Look.Heavy(mb, 0, 0, mw, 26, mt, 12, INT_QST_Look.Cream(), INT_UIF_Align.CENTER);
		ButtonWidget mh = INT_UIF_Draw.Hit(mb, 0, 0, mw, 26, m_Clicks);
		m_Clicks.Bind(mh, ID_REQ_MODE, q.Id);
		y = y + 30;
		int shown = 0;
		foreach (int rid : q.Requires)
		{
			if (shown >= 7)
				break;
			shown++;
			INT_QST_QuestDef rq = INT_QST_Client.s_Catalog.Get(rid);
			string rt = "#" + rid.ToString() + "  ";
			int rcol = INT_QST_Look.Ink2();
			if (rq)
				rt = rt + rq.Name;
			else
			{
				rt = rt + INT_QST_Look.Tr("#STR_INT_QST_B_MISSING");
				rcol = INT_QST_Look.Red();
			}
			rt = INT_QST_Look.Fit(rt, iw - 40, 13);
			INT_QST_Look.Text(m_Insp, x, y, iw - 36, 24, rt, 13, rcol);
			Widget xb = INT_UIF_Draw.Frame(m_Insp, w - 20 - 26, y, 26, 24, "Del");
			Widget xh = INT_UIF_Draw.Frame(xb, 0, 0, 26, 24, "Hover");
			int xf = INT_QST_Look.RedA(40);
			INT_UIF_Surface.Box(xh, 0, 0, 26, 24, 4, xf, 0);
			INT_QST_Look.Icon(xb, 7, 6, 12, "close", INT_QST_Look.Red());
			ButtonWidget xhit = INT_UIF_Draw.Hit(xb, 0, 0, 26, 24, m_Clicks);
			m_Clicks.Bind(xhit, ID_REQ_DEL, rid, xh);
			y = y + 26;
		}
		if (q.Requires.Count() > shown)
		{
			int more = q.Requires.Count() - shown;
			string mm = INT_QST_Look.Fmt("#STR_INT_QST_TRK_MORE", more.ToString());
			INT_QST_Look.Text(m_Insp, x, y, iw, 22, mm, 13, INT_QST_Look.Ink3());
			y = y + 24;
		}
		string lh = INT_QST_Look.Tr("#STR_INT_QST_B_LINK_HINT");
		lh = INT_QST_Look.Fit(lh, iw, 12);
		INT_QST_Look.Text(m_Insp, x, y, iw, 20, lh, 12, INT_QST_Look.Ink3());
		y = y + 30;
		// availability switch
		Widget sw = INT_UIF_Draw.Frame(m_Insp, x, y, iw, 40, "Enable");
		Widget swh = INT_UIF_Draw.Frame(sw, 0, 0, iw, 40, "Hover");
		INT_UIF_Draw.Line(swh, 0, 0, iw, 40, 0x1A2A2218);
		string et = INT_QST_Look.Tr("#STR_INT_QST_B_ENABLED");
		INT_QST_Look.Heavy(sw, 0, 0, iw - 60, 40, et, 15, INT_QST_Look.Ink());
		INT_QST_Look.Switch(sw, iw - 44, 9, q.Enabled != 0);
		ButtonWidget swhit = INT_UIF_Draw.Hit(sw, 0, 0, iw, 40, m_Clicks);
		m_Clicks.Bind(swhit, ID_ENABLE, q.Id, swh);
		// buttons at the bottom
		Widget hv;
		int yc = INT_QST_Look.Sticky();
		Widget eb = INT_QST_Look.Button(m_Insp, x, h - 120, iw, 48, "#STR_INT_QST_B_OPEN_EDITOR", "pencil", INT_QST_Look.BTN_INK, yc, hv);
		ButtonWidget ehit = INT_UIF_Draw.Hit(eb, 0, 0, iw, 48, m_Clicks);
		m_Clicks.Bind(ehit, ID_EDIT, q.Id, hv);
		int rr = INT_QST_Look.Red();
		Widget db = INT_QST_Look.Button(m_Insp, x, h - 64, iw, 44, "#STR_INT_QST_B_DELETE", "trash", INT_QST_Look.BTN_DANGER, rr, hv);
		ButtonWidget dhit = INT_UIF_Draw.Hit(db, 0, 0, iw, 44, m_Clicks);
		m_Clicks.Bind(dhit, ID_DELETE, q.Id, hv);
	}

	protected float InspLine(float x, float y, float w, string labelKey, string value)
	{
		string lab = INT_QST_Look.Spaced(labelKey);
		INT_QST_Look.Text(m_Insp, x, y, 130, 24, lab, 11, INT_QST_Look.Ink3());
		string v = INT_QST_Look.Fit(value, w - 134, 14);
		INT_QST_Look.Heavy(m_Insp, x + 134, y, w - 134, 24, v, 14, INT_QST_Look.Ink());
		return y + 26;
	}

	protected void InspectorHelp(float w)
	{
		float x = 20;
		float iw = w - 40;
		float y = 18;
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_B_HELP_TITLE");
		INT_QST_Look.Heavy(m_Insp, x, y, iw, 22, cap, 12, INT_QST_Look.Ink());
		INT_QST_Look.DoubleRule(m_Insp, x, y + 28, iw);
		y = y + 44;
		array<string> lines = new array<string>();
		string help = INT_QST_Look.Tr("#STR_INT_QST_B_HELP");
		INT_QST_Text.Wrap(help, iw, 14, lines);
		foreach (string ln : lines)
		{
			INT_QST_Look.Text(m_Insp, x, y, iw, 22, ln, 14, INT_QST_Look.Blue());
			y = y + 23;
		}
		y = y + 16;
		string lc = INT_QST_Look.Spaced("#STR_INT_QST_B_LEGEND");
		INT_QST_Look.Heavy(m_Insp, x, y, iw, 22, lc, 12, INT_QST_Look.Ink());
		y = y + 30;
		LegendLine(x, y, iw, INT_QST_Look.InkA(150), false, "#STR_INT_QST_B_LG_INSIDE");
		LegendLine(x, y + 26, iw, ARGB(170, 31, 58, 110), true, "#STR_INT_QST_B_LG_BETWEEN");
		LegendLine(x, y + 52, iw, INT_QST_Look.Red(), false, "#STR_INT_QST_B_LG_SELECTED");
		LegendLine(x, y + 78, iw, C_LIT, false, "#STR_INT_QST_B_LG_GATE");
	}

	// Several cards picked: how many, from which branches, how to move them.
	protected void InspectorGroup(float w)
	{
		float x = 20;
		float iw = w - 40;
		float y = 18;
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_B_GROUP_CAP");
		INT_QST_Look.Heavy(m_Insp, x, y, iw, 22, cap, 12, INT_QST_Look.Ink3());
		y = y + 30;
		map<string, int> per = new map<string, int>();
		foreach (int pid, bool pon : m_Picked)
		{
			INT_QST_QuestDef pq = INT_QST_Client.s_Catalog.Get(pid);
			if (!pq)
				continue;
			int pc = 0;
			per.Find(pq.m_Branch, pc);
			per.Set(pq.m_Branch, pc + 1);
		}
		string nT = m_Picked.Count().ToString();
		string bT = per.Count().ToString();
		string head = INT_QST_Look.Fmt("#STR_INT_QST_B_GROUP_N", nT, bT);
		INT_QST_Look.Heavy(m_Insp, x, y, iw, 30, head, 19, INT_QST_Look.Ink());
		y = y + 40;
		INT_QST_Look.Hair(m_Insp, x, y, iw);
		y = y + 12;
		int shown = 0;
		foreach (string bk, int bn : per)
		{
			if (shown >= 12)
				break;
			shown++;
			int col = ColorOf(bk);
			INT_UIF_Draw.Line(m_Insp, x, y + 7, 10, 10, col);
			string bt = INT_QST_Client.s_Catalog.BranchTitle(bk);
			bt = INT_QST_Look.Fit(bt, iw - 80, 14);
			INT_QST_Look.Heavy(m_Insp, x + 18, y, iw - 80, 24, bt, 14, col);
			string cT = bn.ToString();
			INT_QST_Look.Text(m_Insp, x + iw - 60, y, 60, 24, cT, 14, INT_QST_Look.Ink2(), INT_UIF_Align.RIGHT);
			y = y + 26;
		}
		y = y + 14;
		array<string> lines = new array<string>();
		string hint = INT_QST_Look.Tr("#STR_INT_QST_B_GROUP_HINT");
		INT_QST_Text.Wrap(hint, iw, 14, lines);
		foreach (string ln : lines)
		{
			INT_QST_Look.Text(m_Insp, x, y, iw, 22, ln, 14, INT_QST_Look.Blue());
			y = y + 23;
		}
	}

	protected void LegendLine(float x, float y, float w, int c, bool dashed, string key)
	{
		if (dashed)
		{
			INT_UIF_Draw.Line(m_Insp, x, y + 11, 12, 2, c);
			INT_UIF_Draw.Line(m_Insp, x + 18, y + 11, 12, 2, c);
		}
		else
			INT_UIF_Draw.Line(m_Insp, x, y + 11, 30, 2, c);
		string t = INT_QST_Look.Tr(key);
		INT_QST_Look.Text(m_Insp, x + 42, y, w - 42, 24, t, 13, INT_QST_Look.Ink2());
	}

	protected string FileOf(int id)
	{
		INT_QST_AHello ah = INT_QST_Admin.s_Hello;
		if (!ah)
			return "";
		string pre = id.ToString() + "|";
		foreach (string f : ah.Files)
		{
			if (f.IndexOf(pre) == 0)
			{
				int start = pre.Length();
				int len = f.Length() - start;
				return f.Substring(start, len);
			}
		}
		return "";
	}

	// --- header status ------------------------------------------------------------------------------------------------
	protected void DrawHeaderInfo()
	{
		if (m_Counters)
		{
			INT_UIF_Widgets.ClearChildren(m_Counters);
			INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
			string nq = cat.m_List.Count().ToString();
			string nb = cat.m_BranchOrder.Count().ToString();
			string t = INT_QST_Look.Fmt("#STR_INT_QST_B_COUNTERS", nq, nb);
			float tw = INT_UIF_Draw.TextWidth(t, 14) * 1.08 + 36;
			int lf = INT_QST_Look.LeatherDark();
			int lb = INT_QST_Look.LeatherLine();
			INT_UIF_Surface.Box(m_Counters, 0, 0, tw, 40, 6, lf, lb);
			INT_UIF_Draw.Line(m_Counters, 14, 16, 8, 8, INT_QST_Look.Olive());
			INT_QST_Look.Heavy(m_Counters, 30, 0, tw - 30, 40, t, 14, INT_QST_Look.Cream());
		}
		if (m_StatusLine)
		{
			INT_UIF_Widgets.ClearChildren(m_StatusLine);
			string st = m_Status;
			int c = m_StatusColor;
			if (m_Pending > 0)
			{
				st = INT_QST_Look.Fmt("#STR_INT_QST_B_SAVING", m_Pending.ToString());
				c = INT_QST_Look.Sticky();
			}
			if (st != "")
			{
				INT_UIF_Draw.Line(m_StatusLine, 0, 16, 8, 8, c);
				INT_QST_Look.Heavy(m_StatusLine, 16, 0, 340, 40, st, 14, c);
			}
		}
	}

	static const int C_RED = 1;
	static const int C_OK = 2;
	static const int C_WAIT = 3;

	// status from a stringtable key (keeps call results out of one argument list)
	protected void StatusTr(string key, int kind)
	{
		string t = INT_QST_Look.Tr(key);
		int c = INT_QST_Look.Red();
		if (kind == C_OK)
			c = INT_QST_Look.Olive();
		else if (kind == C_WAIT)
			c = INT_QST_Look.Sticky();
		SetStatus(t, c);
	}

	protected void SetStatus(string text, int color)
	{
		m_Status = text;
		m_StatusColor = color;
		DrawHeaderInfo();
	}

	// --- saving ---------------------------------------------------------------------------------------------------------
	// A deep copy (the editor's: JSON round trip, then the strings copied directly — the engine reader keeps only
	// 1023 bytes of one string).
	protected INT_QST_QuestDef CopyOf(INT_QST_QuestDef q)
	{
		if (!q)
			return null;
		INT_QST_QuestDef c = INT_QST_QuestEditor.CopyQuest(q);
		if (!c)
			return null;
		c.Normalize();
		return c;
	}

	// baseRec: record hash of the quest when the copy was taken (INT_QST_Client.QuestRec; 0 = new quest / no check):
	// the server refuses with CONFLICT when somebody changed the quest since.
	protected void SendQuest(INT_QST_QuestDef q, string branch, bool del, int baseRec)
	{
		INT_QST_ASave a = new INT_QST_ASave();
		INT_QST_AChange ch = new INT_QST_AChange();
		ch.Kind = 1;
		ch.Id = q.Id;
		ch.Key = branch;
		ch.BaseRec = baseRec;
		if (del)
			ch.Delete = 1;
		else
			ch.Quest = q;
		a.Changes.Insert(ch);
		INT_QST_Admin.Save(a);
		m_SentIds.Insert(q.Id);
		m_Pending++;
		DrawHeaderInfo();
	}

	// A quick edit from the board: the copy is taken from the catalog right now, so its record is the base.
	protected void SendBoardEdit(INT_QST_QuestDef c, string branch, bool del)
	{
		int baseRec = 0;
		if (c.Id > 0)
			baseRec = INT_QST_Client.QuestRec(c.Id);
		SendQuest(c, branch, del, baseRec);
	}

	void OnEditorSave(INT_QST_QuestDef q, string branch)
	{
		if (!q)
			return;
		// the base is the record the editor opened (an edit made over another admin's change is refused); the
		// sent copy is kept until the answer: a refused save opens it again in the editor
		int baseRec = 0;
		if (m_Editor)
			baseRec = m_Editor.BaseRec();
		m_Kept.Set(q.Id, q);
		m_KeptBase.Set(q.Id, baseRec);
		// a new card keeps the place it got on the field
		SendQuest(q, branch, false, baseRec);
	}

	protected string ErrText(int code)
	{
		string codeText = code.ToString();
		string key = "#STR_INT_QST_E_" + codeText;
		return INT_QST_Look.Tr(key);
	}

	// One SAVE answer: the oldest request in flight. Saved > 0 and nothing refused = «Сохранено»; a refused quest
	// shows the server's reason, and an editor save is opened again with the admin's copy.
	protected void OnSaveAnswer(int code, string text)
	{
		int sentId = 0;
		bool hadSent = m_SentIds.Count() > 0;
		if (hadSent)
		{
			sentId = m_SentIds[0];
			m_SentIds.RemoveOrdered(0);
		}
		if (code != INT_QST_Res.OK)
		{
			string whole = ErrText(code);
			SetStatusRed(whole);
			Flash(whole);
			if (hadSent)
				SaveFailed(sentId, code);
			return;
		}
		INT_QST_ASaveResult res = new INT_QST_ASaveResult();
		JsonSerializer js = new JsonSerializer();
		string err;
		if (!js.ReadFromString(res, text, err))
		{
			// an answer that does not read: nothing is known, the copy stays for a manual retry
			StatusTr("#STR_INT_QST_E_18", C_RED);
			return;
		}
		for (int i = 0; i < res.TempIds.Count() && i < res.NewIds.Count(); i++)
		{
			int tmp = res.TempIds[i];
			int real = res.NewIds[i];
			INT_QST_BNode nd;
			if (m_Nodes.Find(tmp, nd))
			{
				m_Nodes.Remove(tmp);
				nd.Id = real;
				m_Nodes.Set(real, nd);
				m_LayoutDirty = true;
				m_LayoutSaveAt = GetGame().GetTickTime() + 1.0;
			}
			if (m_Sel == tmp)
				m_Sel = real;
			if (m_Picked.Contains(tmp))
			{
				m_Picked.Remove(tmp);
				m_Picked.Set(real, true);
			}
		}
		int failedCount = 0;
		if (res.Failed)
			failedCount = res.Failed.Count();
		bool sentFailed = false;
		for (int f = 0; f < failedCount; f++)
		{
			int fid = res.Failed[f];
			int fcode = INT_QST_Res.BAD_DATA;
			if (res.FailCodes && f < res.FailCodes.Count())
				fcode = res.FailCodes[f];
			if (hadSent && fid == sentId)
				sentFailed = true;
			if (f == 0)
			{
				string first = ErrText(fcode);
				SetStatusRed(first);
				Flash(first);
			}
			SaveFailed(fid, fcode);
		}
		if (hadSent && !sentFailed)
		{
			m_Kept.Remove(sentId);
			m_KeptBase.Remove(sentId);
		}
		if (failedCount == 0)
		{
			if (res.Problems && res.Problems.Count() > 0)
			{
				string p0 = res.Problems[0];
				Flash(p0);
			}
			if (res.Saved > 0)
				StatusTr("#STR_INT_QST_A_SAVED", C_OK);
		}
		m_InspDirty = true;
	}

	protected void SetStatusRed(string text)
	{
		int red = INT_QST_Look.Red();
		SetStatus(text, red);
	}

	// A refused quest: the editor copy (if this save came from the editor) is opened again. On CONFLICT the base
	// becomes the record the board has now: the admin was told, a second «Сохранить» writes over the other edit.
	protected void SaveFailed(int id, int code)
	{
		INT_QST_QuestDef kept;
		if (!m_Kept.Find(id, kept) || !kept)
			return;
		int baseRec = 0;
		m_KeptBase.Find(id, baseRec);
		m_Kept.Remove(id);
		m_KeptBase.Remove(id);
		if (!m_Editor || !m_Layer)
			return;
		string why = ErrText(code);
		if (m_Editor.IsOpen())
		{
			// the admin is editing something else already: say it, do not take that editor away
			Flash(why);
			return;
		}
		if (code == INT_QST_Res.CONFLICT && id > 0)
			baseRec = INT_QST_Client.QuestRec(id);
		bool isNew = id < 0;
		string branch = kept.m_Branch;
		m_Editor.Open(m_Layer, m_WinW, m_WinH, kept, branch, isNew);
		m_Editor.AfterFailedSave(why, baseRec, code == INT_QST_Res.CONFLICT);
	}

	void OnEditorDelete(int id)
	{
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(id);
		if (!q)
			return;
		SendBoardEdit(q, q.m_Branch, true);
		m_Picked.Remove(id);
		if (m_Sel == id)
			m_Sel = FirstPicked();
		m_InspDirty = true;
		m_ViewDirty = true;
	}

	void OnEditorClosed()
	{
		m_InspDirty = true;
		m_ViewDirty = true;
	}

	void OnAdminReply(int op, int code, string text)
	{
		if (IsClosingDown())
			return;
		if (op == INT_QST_A.HELLO)
		{
			RebuildNodes();
			FixUnsaved();
			if (m_FitOnHello)
			{
				m_FitOnHello = false;
				Fit();
			}
			m_ViewDirty = true;
			m_InspDirty = true;
			BuildToolbar();
			return;
		}
		if (op == INT_QST_A.SAVE)
		{
			if (m_Pending > 0)
				m_Pending--;
			OnSaveAnswer(code, text);
			return;
		}
		if (op == INT_QST_A.BOARD_SAVE)
		{
			if (code != INT_QST_Res.OK)
			{
				string bad = ErrText(code);
				SetStatusRed(bad);
			}
			else
				StatusTr("#STR_INT_QST_A_SAVED", C_OK);
			return;
		}
		if (op == INT_QST_A.RELOAD)
		{
			if (code == INT_QST_Res.OK)
			{
				string t = INT_QST_Admin.TextOf(text);
				string done = INT_QST_Look.Fmt("#STR_INT_QST_A_RELOADED", t);
				Flash(done, false);
				INT_QST_Admin.RequestHello();
			}
			else
			{
				string rf = ErrText(code);
				Flash(rf);
			}
		}
	}

	// Sends the cards moved here (Auto 0: the server takes the place) and, once the hello of THIS opening told what
	// the server has, the automatic places of cards it has no place for (Auto 1: the server takes them only when the
	// id still has no place — an old or missing hello must not overwrite the shared Board.json); merged by id.
	protected int SaveLayoutNow()
	{
		if (!m_LayoutDirty)
			return 0;
		m_LayoutDirty = false;
		INT_QST_AHello ah = INT_QST_Admin.s_Hello;
		bool known = HelloArrived();
		map<int, int> onServer = new map<int, int>();
		if (ah)
		{
			for (int i = 0; i < ah.Nodes.Count(); i++)
			{
				INT_QST_BoardNode sn = ah.Nodes[i];
				onServer.Set(sn.Id, i);
			}
		}
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		INT_QST_ABoard b = new INT_QST_ABoard();
		foreach (int id, INT_QST_BNode n : m_Nodes)
		{
			if (id <= 0)
				continue;
			int autoPlace = 0;
			if (!n.Moved)
			{
				if (!known || onServer.Contains(id) || !cat.Get(id))
					continue;
				autoPlace = 1;
			}
			INT_QST_BoardNode bn = new INT_QST_BoardNode();
			bn.Id = id;
			bn.X = Math.Round(n.X);
			bn.Y = Math.Round(n.Y);
			bn.Auto = autoPlace;
			b.Nodes.Insert(bn);
			n.Moved = false;
			n.Saved = true;
		}
		if (b.Nodes.Count() == 0)
			return 0;
		INT_QST_Admin.SaveBoard(b);
		// keep the admin hello in step (a reopened board starts from these places)
		int sent = b.Nodes.Count();
		if (!ah)
			return sent;
		foreach (INT_QST_BoardNode nn : b.Nodes)
		{
			int at;
			if (onServer.Find(nn.Id, at))
			{
				INT_QST_BoardNode old = ah.Nodes[at];
				old.X = nn.X;
				old.Y = nn.Y;
			}
			else
				ah.Nodes.Insert(nn);
		}
		return sent;
	}

	// After the server board of THIS opening came: cards it has no place for are sent once as automatic places (then
	// every admin sees the same map; the server keeps a place it already has).
	protected void FixUnsaved()
	{
		INT_QST_AHello ah = INT_QST_Admin.s_Hello;
		if (!ah || !HelloArrived())
			return;
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		map<int, bool> onServer = new map<int, bool>();
		foreach (INT_QST_BoardNode sn : ah.Nodes)
			onServer.Set(sn.Id, true);
		foreach (int id, INT_QST_BNode n : m_Nodes)
		{
			if (id > 0 && !onServer.Contains(id) && cat.Get(id))
			{
				m_LayoutDirty = true;
				m_LayoutSaveAt = GetGame().GetTickTime() + 2.0;
				return;
			}
		}
	}

	// --- actions ---------------------------------------------------------------------------------------------------------
	protected void NewQuest()
	{
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		string branch = m_Branch;
		INT_QST_QuestDef sel = cat.Get(m_Sel);
		if (branch == "" && sel)
			branch = sel.m_Branch;
		if (branch == "" && cat.m_BranchOrder.Count() > 0)
			branch = cat.m_BranchOrder[0];
		if (branch == "")
			branch = "New_Branch";
		int order = 0;
		foreach (INT_QST_QuestDef q : cat.m_List)
		{
			if (q.m_Branch == branch && q.Order > order)
				order = q.Order;
		}
		INT_QST_QuestDef nq = new INT_QST_QuestDef();
		nq.Id = m_TempId;
		m_TempId--;
		nq.Order = order + 1;
		nq.Enabled = 0;
		nq.Name = INT_QST_Look.Tr("#STR_INT_QST_B_NEW_NAME");
		nq.m_Branch = branch;
		nq.Normalize();
		// its card appears in the middle of the view
		INT_QST_BNode nd = new INT_QST_BNode();
		nd.Id = nq.Id;
		nd.X = m_PanX + (m_CW * 0.5) / m_Z - NODE_W * 0.5;
		nd.Y = m_PanY + (m_CH * 0.5) / m_Z - NODE_H * 0.5;
		nd.Saved = true;
		nd.Moved = true;
		m_Nodes.Set(nq.Id, nd);
		m_LayoutRev++;
		m_Editor.Open(m_Layer, m_WinW, m_WinH, nq, branch, true);
	}

	protected void OpenEditor(int id)
	{
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(id);
		if (!q)
			return;
		m_Editor.Open(m_Layer, m_WinW, m_WinH, q, q.m_Branch, false);
		INT_QST_Sfx.Play("page");
	}

	// Link mode: first card = the condition, second card = the quest that requires it.
	protected void LinkClick(int id)
	{
		if (m_LinkFrom == 0)
		{
			m_LinkFrom = id;
			StatusTr("#STR_INT_QST_B_LINK_SECOND", C_WAIT);
			m_ViewDirty = true;
			return;
		}
		if (id == m_LinkFrom)
		{
			m_LinkFrom = 0;
			m_ViewDirty = true;
			return;
		}
		INT_QST_QuestDef target = INT_QST_Client.s_Catalog.Get(id);
		if (!target)
			return;
		if (target.Requires.Find(m_LinkFrom) >= 0)
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_B_LINK_EXISTS"));
			m_LinkFrom = 0;
			m_ViewDirty = true;
			return;
		}
		if (target.Requires.Count() >= INT_QST_Const.MAX_REQUIRES)
		{
			int most = INT_QST_Const.MAX_REQUIRES;
			string mostText = most.ToString();
			string full = INT_QST_Look.Fmt("#STR_INT_QST_ED_E_MAX", mostText);
			Flash(full);
			m_LinkFrom = 0;
			m_ViewDirty = true;
			return;
		}
		if (INT_QST_Board.MakesLoop(id, m_LinkFrom))
		{
			string loop = INT_QST_Look.Tr("#STR_INT_QST_B_LINK_LOOP");
			Flash(loop);
			m_LinkFrom = 0;
			m_ViewDirty = true;
			return;
		}
		INT_QST_QuestDef c = CopyOf(target);
		if (!c)
			return;
		c.Requires.Insert(m_LinkFrom);
		SendBoardEdit(c, target.m_Branch, false);
		m_LinkFrom = 0;
		SelectOnly(id);
		m_InspDirty = true;
		m_ViewDirty = true;
		INT_QST_Sfx.Play("pin");
	}

	// True when quest `questId` requiring `reqId` would close a loop of requirements: `reqId` is the quest itself or
	// already needs it, directly or through other quests (catalog). Used by the link mode and by the quest editor.
	// An explicit stack + a seen set: the old recursive walk had only a depth guard and went exponential on dense
	// requirement graphs (a hang).
	static bool MakesLoop(int questId, int reqId)
	{
		if (reqId == questId)
			return true;
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		array<int> stack = new array<int>();
		map<int, bool> seen = new map<int, bool>();
		stack.Insert(reqId);
		seen.Set(reqId, true);
		int steps = 0;
		while (stack.Count() > 0 && steps < 200000)
		{
			steps++;
			int last = stack.Count() - 1;
			int cur = stack[last];
			stack.RemoveOrdered(last);
			INT_QST_QuestDef q = cat.Get(cur);
			if (!q)
				continue;
			foreach (int r : q.Requires)
			{
				if (r == questId)
					return true;
				if (seen.Contains(r))
					continue;
				seen.Set(r, true);
				stack.Insert(r);
			}
		}
		return false;
	}

	protected void RemoveRequire(int questId, int reqId)
	{
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(questId);
		if (!q)
			return;
		INT_QST_QuestDef c = CopyOf(q);
		if (!c)
			return;
		c.Requires.RemoveItem(reqId);
		SendBoardEdit(c, q.m_Branch, false);
		INT_QST_Sfx.Play("click");
	}

	protected void ToggleRequireMode(int questId)
	{
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(questId);
		if (!q)
			return;
		INT_QST_QuestDef c = CopyOf(q);
		if (!c)
			return;
		// explicit 0 / 1 (a value other than 0 or 1 from a hand-edited file stays a valid mode)
		if (c.RequireAny == 1)
			c.RequireAny = 0;
		else
			c.RequireAny = 1;
		SendBoardEdit(c, q.m_Branch, false);
		INT_QST_Sfx.Play("click");
	}

	protected void ToggleEnabled(int questId)
	{
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(questId);
		if (!q)
			return;
		INT_QST_QuestDef c = CopyOf(q);
		if (!c)
			return;
		if (c.Enabled == 0)
			c.Enabled = 1;
		else
			c.Enabled = 0;
		SendBoardEdit(c, q.m_Branch, false);
		INT_QST_Sfx.Play("check");
	}

	// --- popups (branch filter, check list) -------------------------------------------------------------------------
	protected void OpenPopup(int kind)
	{
		ClosePopup();
		// the list of the popup closed last stays alive one more open (it may be inside its own event right now)
		m_PopupListOld = m_PopupList;
		m_PopupSrcOld = m_PopupSrc;
		m_Popup = INT_UIF_Draw.Frame(m_Layer, 0, 0, m_WinW, m_WinH, "Popup");
		INT_UIF_Surface.Box(m_Popup, 4, 4, m_WinW - 8, m_WinH - 8, 14, 0x730D0905, 0);
		ButtonWidget bg = INT_UIF_Draw.Hit(m_Popup, 0, 0, m_WinW, m_WinH, m_Clicks);
		m_Clicks.Bind(bg, ID_POPUP_BG);
		m_PopupSrc = new INT_QST_PopupSource();
		m_PopupSrc.m_Owner = this;
		m_PopupSrc.m_Kind = kind;
		string titleKey = "#STR_INT_QST_B_POP_BRANCH";
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		if (kind == 1)
		{
			INT_QST_PopupRow all = new INT_QST_PopupRow();
			all.Key = "";
			all.Text = INT_QST_Look.Tr("#STR_INT_QST_B_ALL_BRANCHES");
			all.Sub = cat.m_List.Count().ToString();
			m_PopupSrc.m_Rows.Insert(all);
			foreach (string bk : cat.m_BranchOrder)
			{
				int n = 0;
				foreach (INT_QST_QuestDef q : cat.m_List)
				{
					if (q.m_Branch == bk)
						n++;
				}
				INT_QST_PopupRow r = new INT_QST_PopupRow();
				r.Key = bk;
				r.Text = cat.BranchTitle(bk);
				r.Sub = n.ToString();
				r.Color = ColorOf(bk);
				m_PopupSrc.m_Rows.Insert(r);
			}
		}
		else
		{
			titleKey = "#STR_INT_QST_B_POP_CHECK";
			CollectProblems(m_PopupSrc.m_Rows);
			if (m_PopupSrc.m_Rows.Count() == 0)
			{
				INT_QST_PopupRow ok = new INT_QST_PopupRow();
				ok.Text = INT_QST_Look.Tr("#STR_INT_QST_A_NO_PROBLEMS");
				m_PopupSrc.m_Rows.Insert(ok);
			}
		}
		float cw = 720;
		float chh = 640;
		float cx = (m_WinW - cw) * 0.5;
		float cy = (m_WinH - chh) * 0.5;
		Widget card = INT_UIF_Draw.Frame(m_Popup, cx, cy, cw, chh, "Card");
		INT_UIF_Surface.Shadow(card, -30, -20, cw + 60, chh + 60, 0x8C000000);
		INT_QST_Look.PaperSheet(card, 0, 0, cw, chh, INT_QST_Look.Note(), "Paper");
		INT_QST_Look.PinHead(card, cw * 0.5 - 13, -12);
		string t = INT_QST_Look.Spaced(titleKey);
		INT_QST_Look.Heavy(card, 30, 24, cw - 60, 26, t, 14, INT_QST_Look.Ink());
		INT_QST_Look.DoubleRule(card, 30, 56, cw - 60);
		m_PopupList = new INT_QST_VList();
		int bar = INT_QST_Look.InkA(30);
		int thumb = INT_QST_Look.InkA(120);
		m_PopupList.Build(card, 24, 70, cw - 48, chh - 90, m_PopupSrc, bar, thumb);
		INT_UIF_Animator.FadeIn(m_Popup, 0.14);
		INT_QST_Sfx.Play("page");
	}

	// Closed from the popup's own row click (PopupPick) or background click: the list, its clicks and the invoker
	// that runs stay alive (until the next OpenPopup), the widgets are hidden now and unlinked on the next frame.
	protected void ClosePopup()
	{
		if (!m_Popup)
			return;
		m_Clicks.ForgetUnder(m_Popup);
		m_Popup.Show(false);
		m_Dead.Insert(m_Popup);
		m_Popup = null;
	}

	// Widgets of closed overlays (Update, outside every widget event).
	protected void KillDead()
	{
		if (m_Dead.Count() == 0)
			return;
		foreach (Widget dw : m_Dead)
		{
			if (dw)
				dw.Unlink();
		}
		m_Dead.Clear();
	}

	void PopupPick(int kind, string key, int id)
	{
		ClosePopup();
		if (kind == 1)
		{
			m_Branch = key;
			Fit();
			BuildToolbar();
			INT_QST_Sfx.Play("click");
			return;
		}
		if (id > 0)
		{
			INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(id);
			if (q && !InFilter(q))
				m_Branch = "";
			SelectOnly(id);
			CenterOn(id);
			m_InspDirty = true;
			BuildToolbar();
		}
	}

	protected int ProblemsCount()
	{
		array<ref INT_QST_PopupRow> rows = new array<ref INT_QST_PopupRow>();
		CollectProblems(rows);
		return rows.Count();
	}

	// Server problems (file reading) + board checks (missing requirements / givers, quests without targets).
	protected void CollectProblems(array<ref INT_QST_PopupRow> rows)
	{
		INT_QST_AHello ah = INT_QST_Admin.s_Hello;
		if (ah)
		{
			foreach (string p : ah.Problems)
			{
				INT_QST_PopupRow sr = new INT_QST_PopupRow();
				sr.Text = p;
				sr.Color = INT_QST_Look.RedDeep();
				rows.Insert(sr);
			}
		}
		foreach (INT_QST_QuestDef q : INT_QST_Client.s_Catalog.m_List)
		{
			if (!HasProblem(q))
				continue;
			INT_QST_PopupRow r = new INT_QST_PopupRow();
			r.Id = q.Id;
			r.Text = "#" + q.Id.ToString() + "  " + q.Name;
			r.Sub = ProblemOf(q);
			r.Color = INT_QST_Look.Red();
			rows.Insert(r);
		}
	}

	protected string ProblemOf(INT_QST_QuestDef q)
	{
		foreach (int r : q.Requires)
		{
			if (!INT_QST_Client.s_Catalog.Get(r))
				return INT_QST_Look.Tr("#STR_INT_QST_B_P_REQ");
		}
		if (q.GiverStart != "" && !INT_QST_Client.Giver(q.GiverStart))
			return INT_QST_Look.Tr("#STR_INT_QST_B_P_GIVER");
		if (q.GiverFinish != "" && !INT_QST_Client.Giver(q.GiverFinish))
			return INT_QST_Look.Tr("#STR_INT_QST_B_P_GIVER");
		return INT_QST_Look.Tr("#STR_INT_QST_B_P_TARGETS");
	}

	// --- search -----------------------------------------------------------------------------------------------------
	void OnSearchChanged(INT_QST_Field f)
	{
		m_SearchPos = 0;
		JumpToSearch(false);
	}

	void OnSearchSubmit(INT_QST_Field f)
	{
		JumpToSearch(true);
	}

	protected void JumpToSearch(bool next)
	{
		if (!m_Search)
			return;
		string query = m_Search.GetText();
		query.TrimInPlace();
		if (query == "")
			return;
		string low = INT_QST_Util.Lower(query);
		int idq = query.ToInt();
		array<int> hits = new array<int>();
		foreach (INT_QST_QuestDef q : INT_QST_Client.s_Catalog.m_List)
		{
			string nm = INT_QST_Util.Lower(q.Name);
			if (nm.IndexOf(low) >= 0 || (idq > 0 && q.Id == idq))
				hits.Insert(q.Id);
		}
		if (hits.Count() == 0)
		{
			StatusTr("#STR_INT_QST_B_NOT_FOUND", C_RED);
			return;
		}
		if (next)
			m_SearchPos++;
		if (m_SearchPos >= hits.Count())
			m_SearchPos = 0;
		int id = hits[m_SearchPos];
		INT_QST_QuestDef hq = INT_QST_Client.s_Catalog.Get(id);
		if (hq && !InFilter(hq))
			m_Branch = "";
		SelectOnly(id);
		CenterOn(id);
		m_InspDirty = true;
		int pos1 = m_SearchPos + 1;
		string posT = pos1.ToString();
		string allT = hits.Count().ToString();
		string found = INT_QST_Look.Fmt("#STR_INT_QST_B_FOUND", posT, allT);
		SetStatus(found, INT_QST_Look.Olive());
	}

	// --- mouse ------------------------------------------------------------------------------------------------------------
	protected int CardAt(float fx, float fy)
	{
		// topmost first: the last drawn card wins
		for (int i = m_Visible.Count() - 1; i >= 0; i--)
		{
			int id = m_Visible[i];
			INT_QST_BNode nd;
			if (!m_Nodes.Find(id, nd))
				continue;
			float sx = SX(nd.X);
			float sy = SY(nd.Y);
			if (fx >= sx && fy >= sy && fx <= sx + NODE_W * m_Z && fy <= sy + NODE_H * m_Z)
				return id;
		}
		return 0;
	}

	void CanvasDown(int button)
	{
		if (m_Editor && m_Editor.IsOpen())
			return;
		float fx;
		float fy;
		MouseField(fx, fy);
		if (button == MouseState.LEFT && fx >= m_CW - MINI_W - 12 && fy >= m_CH - MINI_H - 12 && fx <= m_CW - 12 && fy <= m_CH - 12)
		{
			MiniDown();
			return;
		}
		m_DownX = fx;
		m_DownY = fy;
		m_DragButton = button;
		m_StartPanX = m_PanX;
		m_StartPanY = m_PanY;
		SetFocus(null);
		if (button == MouseState.RIGHT || button == MouseState.MIDDLE)
		{
			m_Drag = 1;
			return;
		}
		if (button != MouseState.LEFT)
			return;
		bool shift = KeyState(KeyCode.KC_LSHIFT) != 0 || KeyState(KeyCode.KC_RSHIFT) != 0;
		m_DownShift = shift;
		m_DownCard = 0;
		m_DownEmpty = false;
		m_Moved = false;
		// a branch name picks the whole branch (Shift adds it); dragging the name moves the branch
		if (!m_LinkMode)
		{
			string lb = LabelAt(fx, fy);
			if (lb != "")
			{
				PickBranch(lb, shift);
				StartGroupDrag();
				return;
			}
		}
		int id = CardAt(fx, fy);
		if (id == 0)
		{
			m_Drag = 1;			// pan; released without a move = clear the selection
			m_DownEmpty = true;
			return;
		}
		if (m_LinkMode)
		{
			m_Drag = 0;
			LinkClick(id);
			return;
		}
		m_DownCard = id;
		if (shift)
		{
			if (m_Picked.Contains(id))
			{
				// Shift+click on a picked card takes it out (no drag)
				m_Picked.Remove(id);
				if (m_Sel == id)
					m_Sel = FirstPicked();
				m_InspDirty = true;
				m_ViewDirty = true;
				INT_QST_Sfx.Play("click");
				return;
			}
			m_Picked.Set(id, true);
			m_Sel = id;
		}
		else if (!m_Picked.Contains(id))
		{
			m_Picked.Clear();
			m_Picked.Set(id, true);
			m_Sel = id;
		}
		else
			m_Sel = id;		// a card of the group: the group stays (a drag moves all of it)
		m_InspDirty = true;
		m_ViewDirty = true;
		INT_QST_Sfx.Play("click");
		StartGroupDrag();
	}

	// --- selection ---------------------------------------------------------------------------------------------------
	protected void SelectOnly(int id)
	{
		m_Picked.Clear();
		if (id > 0)
			m_Picked.Set(id, true);
		m_Sel = id;
		m_InspDirty = true;
		m_ViewDirty = true;
	}

	protected int FirstPicked()
	{
		foreach (int k, bool v : m_Picked)
			return k;
		return 0;
	}

	// cards of quests that are gone leave the selection
	protected void PrunePicked()
	{
		array<int> gone = new array<int>();
		foreach (int k, bool v : m_Picked)
		{
			if (!INT_QST_Client.s_Catalog.Get(k))
				gone.Insert(k);
		}
		foreach (int g : gone)
			m_Picked.Remove(g);
		if (m_Sel > 0 && !INT_QST_Client.s_Catalog.Get(m_Sel))
			m_Sel = FirstPicked();
		if (gone.Count() > 0)
			m_InspDirty = true;
	}

	// every card of a branch (inside the filter); add = keep what was picked
	protected void PickBranch(string branch, bool add)
	{
		if (!add)
			m_Picked.Clear();
		int first = 0;
		foreach (INT_QST_QuestDef q : INT_QST_Client.s_Catalog.m_List)
		{
			if (q.m_Branch != branch || !InFilter(q) || !m_Nodes.Contains(q.Id))
				continue;
			m_Picked.Set(q.Id, true);
			if (first == 0)
				first = q.Id;
		}
		if (first != 0)
			m_Sel = first;
		m_InspDirty = true;
		m_ViewDirty = true;
		INT_QST_Sfx.Play("click");
	}

	protected string LabelAt(float fx, float fy)
	{
		for (int i = m_LabelHitKey.Count() - 1; i >= 0; i--)
		{
			float lx = m_LabelHitX[i];
			float ly = m_LabelHitY[i];
			if (fx >= lx && fy >= ly && fx <= lx + m_LabelHitW[i] && fy <= ly + m_LabelHitH[i])
				return m_LabelHitKey[i];
		}
		return "";
	}

	// the drag moves every picked card: their start places
	protected void StartGroupDrag()
	{
		m_DragStart.Clear();
		foreach (int pid, bool pon : m_Picked)
		{
			INT_QST_BNode pn;
			if (m_Nodes.Find(pid, pn))
				m_DragStart.Set(pid, Vector(pn.X, pn.Y, 0));
		}
		m_Drag = 3;
	}

	// world offset from the start places
	protected void MoveGroup(float ox, float oy)
	{
		foreach (int did, vector st : m_DragStart)
		{
			INT_QST_BNode dn;
			if (!m_Nodes.Find(did, dn))
				continue;
			dn.X = st[0] + ox;
			dn.Y = st[1] + oy;
			dn.Saved = true;
			dn.Moved = true;
		}
		m_ViewDirty = true;
	}

	void CanvasWheel(int wheel)
	{
		if (m_Editor && m_Editor.IsOpen())
			return;
		float fx;
		float fy;
		if (!MouseField(fx, fy))
		{
			fx = m_CW * 0.5;
			fy = m_CH * 0.5;
		}
		float factor = 1.15;
		if (wheel < 0)
			factor = 1.0 / 1.15;
		ZoomAt(factor, fx, fy);
	}

	void CanvasDouble()
	{
		if (m_Editor && m_Editor.IsOpen())
			return;
		float fx;
		float fy;
		MouseField(fx, fy);
		int id = CardAt(fx, fy);
		if (id != 0)
			OpenEditor(id);
	}

	protected void TickMouse()
	{
		if (m_Drag == 0)
			return;
		int mask = MB_PRESSED_MASK;
		bool down = (GetMouseState(m_DragButton) & mask) != 0;
		float fx;
		float fy;
		MouseField(fx, fy);
		if (!down)
		{
			if (m_Drag == 2)
			{
				m_LayoutDirty = true;
				m_LayoutSaveAt = GetGame().GetTickTime() + 1.5;
				m_LayoutRev++;
				m_ViewDirty = true;
			}
			else if (m_Drag == 3 && !m_DownShift && m_DownCard != 0 && m_Picked.Count() > 1)
				SelectOnly(m_DownCard);		// a plain click on one card of a group: only it stays picked
			else if (m_Drag == 1 && m_DownEmpty && !m_Moved && !m_DownShift && (m_Picked.Count() > 0 || m_Sel != 0))
				SelectOnly(0);			// a click on empty paper clears the selection (owner)
			m_Drag = 0;
			return;
		}
		float dx = fx - m_DownX;
		float dy = fy - m_DownY;
		if (dx * dx + dy * dy > 25)
			m_Moved = true;
		if (m_Drag == 1)
		{
			m_PanX = m_StartPanX - dx / m_Z;
			m_PanY = m_StartPanY - dy / m_Z;
			m_ViewDirty = true;
			return;
		}
		if (m_Drag == 3 && m_Moved)
			m_Drag = 2;
		if (m_Drag == 2)
			MoveGroup(dx / m_Z, dy / m_Z);
	}

	// --- frame ----------------------------------------------------------------------------------------------------------
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (IsClosingDown())
			return;
		KillDead();
		if (m_ToolDirty)
			BuildToolbarNow();
		if (m_Editor)
			m_Editor.Tick(timeslice);
		TickMouse();
		if (m_ViewDirty)
			Redraw();
		if (m_InspDirty)
		{
			BuildInspector();
			DrawHeaderInfo();
		}
		if (m_LayoutDirty && GetGame().GetTickTime() >= m_LayoutSaveAt && m_Drag == 0)
			SaveLayoutNow();
		// Del deletes the selected card (not while typing)
		bool typing = EditBoxWidget.Cast(GetFocus()) || MultilineEditBoxWidget.Cast(GetFocus());
		if (m_Editor && m_Editor.IsTyping())
			typing = true;
		if (!typing && m_Sel > 0 && m_Picked.Count() <= 1 && !IsAsking() && (!m_Editor || !m_Editor.IsOpen()) && !m_Popup)
		{
			if (KeyState(KeyCode.KC_DELETE) != 0)
			{
				ClearKey(KeyCode.KC_DELETE);
				AskDelete(m_Sel);
			}
		}
	}

	protected void AskDelete(int id)
	{
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(id);
		if (!q)
			return;
		string text = INT_QST_Look.Fmt("#STR_INT_QST_B_ASK_DELETE_TEXT", q.Name);
		Ask(ASK_DELETE, id, "#STR_INT_QST_B_ASK_DELETE", text, "#STR_INT_QST_B_DELETE", true);
	}

	override bool OnEscapeOverlay()
	{
		if (m_Editor && m_Editor.IsOpen())
		{
			m_Editor.OnEscape();
			return true;
		}
		if (m_Popup)
		{
			ClosePopup();
			return true;
		}
		if (m_LinkMode)
		{
			m_LinkMode = false;
			m_LinkFrom = 0;
			m_ViewDirty = true;
			BuildToolbar();
			SetStatus("", 0);
			return true;
		}
		if (m_Picked.Count() > 1)
		{
			SelectOnly(0);
			return true;
		}
		return false;
	}

	override void OnClickId(int id, int data, int button)
	{
		if (CommonClick(id))
			return;
		switch (id)
		{
			case ID_RELOAD:
				string rq = INT_QST_Look.Tr("#STR_INT_QST_A_RELOAD_ASK");
				Ask(ASK_RELOAD, 0, "#STR_INT_QST_A_RELOAD", rq, "#STR_INT_QST_A_RELOAD", false);
				return;
			case ID_NEW:
				NewQuest();
				return;
			case ID_LINK:
				m_LinkMode = !m_LinkMode;
				m_LinkFrom = 0;
				m_ViewDirty = true;
				BuildToolbar();
				if (m_LinkMode)
					StatusTr("#STR_INT_QST_B_LINK_FIRST", C_WAIT);
				else
					SetStatus("", 0);
				INT_QST_Sfx.Play("click");
				return;
			case ID_AUTO:
				string aq = INT_QST_Look.Tr("#STR_INT_QST_B_ASK_AUTO_TEXT");
				Ask(ASK_AUTO, 0, "#STR_INT_QST_B_AUTO", aq, "#STR_INT_QST_B_AUTO", false);
				return;
			case ID_ZOOM_OUT:
				ZoomAt(1.0 / 1.25, m_CW * 0.5, m_CH * 0.5);
				return;
			case ID_ZOOM_IN:
				ZoomAt(1.25, m_CW * 0.5, m_CH * 0.5);
				return;
			case ID_FIT:
				Fit();
				BuildToolbar();
				return;
			case ID_BRANCH:
				OpenPopup(1);
				return;
			case ID_CHECK:
				OpenPopup(2);
				return;
			case ID_EDIT:
				OpenEditor(data);
				return;
			case ID_DELETE:
				AskDelete(data);
				return;
			case ID_ENABLE:
				ToggleEnabled(data);
				return;
			case ID_REQ_DEL:
				RemoveRequire(m_Sel, data);
				return;
			case ID_REQ_MODE:
				ToggleRequireMode(data);
				return;
			case ID_POPUP_BG:
				ClosePopup();
				return;
			case ID_SAVE:
				m_LayoutDirty = true;
				int sent = SaveLayoutNow();
				if (sent == 0 && m_Pending == 0)
					StatusTr("#STR_INT_QST_B_SAVED_ALL", C_OK);
				INT_QST_Sfx.Play("click");
				return;
		}
	}

	// Stand commands (TestKit "#qst board ..."): the mouse cannot be driven there.
	override void OnTestCommand(string command)
	{
		array<string> w = new array<string>();
		command.Split(" ", w);
		if (w.Count() == 0)
			return;
		string c = w[0];
		string a1 = "";
		string a2 = "";
		if (w.Count() > 1)
			a1 = w[1];
		if (w.Count() > 2)
			a2 = w[2];
		if (c == "branch")
		{
			m_Branch = a1;
			if (a1 == "all")
				m_Branch = "";
			m_MiniKey = "";
			Fit();
			BuildToolbar();
		}
		else if (c == "zoom")
		{
			ZoomAt(a1.ToFloat() / m_Z, m_CW * 0.5, m_CH * 0.5);
		}
		else if (c == "fit")
		{
			Fit();
			BuildToolbar();
		}
		else if (c == "sel")
		{
			SelectOnly(a1.ToInt());
		}
		else if (c == "add")
		{
			// as Shift+click on a card
			int addId = a1.ToInt();
			if (addId > 0)
			{
				m_Picked.Set(addId, true);
				m_Sel = addId;
				m_InspDirty = true;
				m_ViewDirty = true;
			}
		}
		else if (c == "pickbranch")
		{
			PickBranch(a1, false);
		}
		else if (c == "movepicked")
		{
			// as a drag of the selection by (x, y) world units
			StartGroupDrag();
			float mox = a1.ToFloat();
			float moy = a2.ToFloat();
			MoveGroup(mox, moy);
			m_Drag = 0;
			m_LayoutDirty = true;
			m_LayoutSaveAt = GetGame().GetTickTime() + 1.5;
			m_LayoutRev++;
		}
		else if (c == "center")
		{
			SelectOnly(a1.ToInt());
			CenterOn(m_Sel);
		}
		else if (c == "pan")
		{
			m_PanX = a1.ToFloat();
			m_PanY = a2.ToFloat();
			m_ViewDirty = true;
		}
		else if (c == "auto")
		{
			OnAnswer(ASK_AUTO, 0, true);
		}
		else if (c == "edwheel" && m_Editor && m_Editor.IsOpen())
		{
			int wcol = a1.ToInt();
			int wsteps = a2.ToInt();
			m_Editor.TestWheel(wcol, wsteps);
		}
		else if (c == "editor" && m_Editor && m_Editor.IsOpen())
		{
			// a click id of the open quest editor (3 = «Сохранить»)
			int eid = a1.ToInt();
			int edata = a2.ToInt();
			m_Editor.OnClickId(eid, edata, 0);
		}
		else if (c == "stats")
		{
			Redraw();
			int used = LINE_BUDGET - m_LineBudget;
			string zT = m_Z.ToString();
			string visT = m_Visible.Count().ToString();
			string usedT = used.ToString();
			string hubT = m_Hub.Count().ToString();
			string slotT = m_Slots.Count().ToString();
			string fullT = m_LinesFull.ToString();
			string s = "схема: z=" + zT + " видно=" + visT + " линий=" + usedT;
			s = s + " ворот=" + hubT + " слотов=" + slotT + " переполнение=" + fullT;
			INT_QST_Log.Info(s);
		}
	}

	override void OnAnswer(int tag, int data, bool yes)
	{
		if (!yes)
			return;
		if (tag == ASK_DELETE)
			OnEditorDelete(data);
		else if (tag == ASK_RELOAD)
			INT_QST_Admin.Reload();
		else if (tag == ASK_AUTO)
		{
			AutoLayout(true);
			m_LayoutDirty = true;
			m_LayoutSaveAt = GetGame().GetTickTime() + 0.2;
			Fit();
			BuildToolbar();
		}
	}
}
