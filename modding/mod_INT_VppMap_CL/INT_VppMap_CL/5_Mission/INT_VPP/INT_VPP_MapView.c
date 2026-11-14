// ============================================================================
// VanillaPPMap (our VPP fork) - concept B «ОБЗОР»: the 2D overlay of the map (MAP agent, phase 2, 2026-11-12).
// Host = INT_VPP_Overlay, a layout child of Map_Widget (VPPMapMenu.layout): two CanvasWidgets (static lines:
// zone outlines, route, ruler; live lines: rubber bands, editor ghost) and the INT_VPP_Marks frame for widgets.
// Zones have NO hatch any more (owner 2026-11-13): outline + name + sub-line only. Admin text labels (2026-11-13):
// one tree per label on / near the screen (lazy, a few per frame), «Зоны» layer, INT_VPP_Stat kind 10. The map
// has priority 100 and draws over lower children, so EVERY widget here gets own sort 300 (Look.SortTree, collected
// first). MapToScreen gives absolute screen px: map-local = screen - the map's screen position.
// Positions are recomputed only when the view (GetMapPos / GetScale / map rect) or the data changed; canvas lines
// are counted (INT_VPP_Stat.INT_CanvasLines) and capped at 1800 per frame (more blinks the whole menu).
// No AddUserMark (engine marks render as squares on 1.30). Every widget tree made here is unlinked by this
// object (layer off, data change, INT_Destroy on map close); INT_VPP_Stat kinds 1 (host) and 2 (marker trees).
// Client only: nothing runs at file scope.
// ============================================================================

// Small formatters shared by the map files (design: exp.js X.m / X.coords / X.square / X.runMin ...).
class INT_VPP_Fmt
{
	static const float INT_RUN_MPS = 4.6;
	static const float INT_WALK_MPS = 1.6;
	static const string INT_DIRS = "С,СВ,В,ЮВ,Ю,ЮЗ,З,СЗ";

	protected static ref array<string> s_INT_Dirs;

	static string INT_Pad3(int v)
	{
		string s = v.ToString();
		while (s.Length() < 3)
		{
			s = "0" + s;
		}
		return s;
	}

	// 2D distance (X / Z) in metres
	static float INT_Dist2D(vector a, vector b)
	{
		float dx = b[0] - a[0];
		float dz = b[2] - a[2];
		return Math.Sqrt(dx * dx + dz * dz);
	}

	// "195" (5 m steps) / "1,2" (km)
	static string INT_DistNum(float m)
	{
		if (m != m)
			return "0";
		if (m < 1000)
		{
			int r5 = Math.Round(m / 5.0);
			int mm = r5 * 5;
			return mm.ToString();
		}
		int tenths = Math.Round(m / 100.0);
		int whole = tenths / 10;
		int frac = tenths - whole * 10;
		if (frac == 0)
			return whole.ToString();
		string s = whole.ToString() + "," + frac.ToString();
		return s;
	}

	static string INT_DistUnit(float m)
	{
		if (m < 1000)
			return "м";
		return "км";
	}

	static string INT_Dist(float m)
	{
		string n = INT_VPP_Fmt.INT_DistNum(m);
		string u = INT_VPP_Fmt.INT_DistUnit(m);
		return n + " " + u;
	}

	static int INT_RunMin(float m)
	{
		int v = Math.Round(m / INT_RUN_MPS / 60.0);
		if (v < 1)
			v = 1;
		return v;
	}

	static int INT_WalkMin(float m)
	{
		int v = Math.Round(m / INT_WALK_MPS / 60.0);
		if (v < 1)
			v = 1;
		return v;
	}

	// degrees, 0 = north (+Z), clockwise
	static float INT_Bearing(vector a, vector b)
	{
		float dx = b[0] - a[0];
		float dz = b[2] - a[2];
		float deg = Math.Atan2(dx, dz) * Math.RAD2DEG;
		if (deg < 0)
			deg = deg + 360;
		return deg;
	}

	static string INT_DirWord(float deg)
	{
		if (!s_INT_Dirs)
		{
			s_INT_Dirs = new array<string>();
			string src = INT_DIRS;
			src.Split(",", s_INT_Dirs);
		}
		int idx = Math.Round(deg / 45.0);
		int wrapped = idx - (idx / 8) * 8;
		if (wrapped < 0)
			wrapped = wrapped + 8;
		if (wrapped >= s_INT_Dirs.Count())
			return "";
		return s_INT_Dirs.Get(wrapped);
	}

	// "13 295 · 5 995" (world X · Z)
	static string INT_Coords(vector p)
	{
		int x = Math.Round(p[0]);
		int z = Math.Round(p[2]);
		string sx = INT_VPPX_Text.Thousands(x);
		string sz = INT_VPPX_Text.Thousands(z);
		return sx + " · " + sz;
	}

	// grid square of 100 m: "132 · 093" (column from the west, row from the north)
	static string INT_Square(vector p)
	{
		float ws = INT_VPP_Validate.WorldSize();
		int col = Math.Floor(p[0] / 100.0);
		int row = Math.Floor((ws - p[2]) / 100.0);
		if (col < 0)
			col = 0;
		if (row < 0)
			row = 0;
		string sr = INT_VPP_Fmt.INT_Pad3(row);
		return col.ToString() + " · " + sr;
	}

	// MarkerInfo colour (vector 0..255) -> opaque ARGB
	static int INT_VecToArgb(vector c)
	{
		int r = Math.Clamp(c[0], 0, 255);
		int g = Math.Clamp(c[1], 0, 255);
		int b = Math.Clamp(c[2], 0, 255);
		return ARGB(255, r, g, b);
	}

	static vector INT_ArgbToVec(int argb)
	{
		int r = (argb >> 16) & 0xFF;
		int g = (argb >> 8) & 0xFF;
		int b = argb & 0xFF;
		float fr = r;
		float fg = g;
		float fb = b;
		return Vector(fr, fg, fb);
	}

	static int INT_WithAlpha(int argb, int a)
	{
		int rgb = argb & 0x00FFFFFF;
		return (a << 24) | rgb;
	}
}

// One marker on the map (server / mine / mine-this-server / event). Built by VPPMapMenu, drawn by INT_VPP_MapView.
class INT_VPP_MapItem : Managed
{
	static const int KIND_SERVER = 0;
	static const int KIND_MINE = 1;
	static const int KIND_MINE_SRV = 2;
	static const int KIND_EVENT = 3;

	int m_INT_Kind;
	int m_INT_Layer;
	ref MarkerInfo m_INT_Info;
	string m_INT_Name;
	vector m_INT_Pos;
	int m_INT_Color;
	string m_INT_IconPath;
	bool m_INT_OwnIcon;
	bool m_INT_Visible;
	bool m_INT_3D;
	// map mode (chips of the list, 2026-11-12): INT_VPP_MODE_*, and what the server allows for it
	int m_INT_Mode;
	bool m_INT_Can3;
	bool m_INT_Can2;

	// view state (INT_VPP_MapView only)
	Widget m_INT_Root;
	TextWidget m_INT_NameW;
	TextWidget m_INT_DistW;
	string m_INT_DistText;
	float m_INT_LabelW;
	bool m_INT_LabelOn;
	float m_INT_SX;
	float m_INT_SY;
	bool m_INT_OnScreen;
	// visible and its layer on: gets a tree once it is on / near the screen (review #7: lazy trees)
	bool m_INT_Want;

	bool INT_IsMine()
	{
		if (m_INT_Kind == KIND_MINE || m_INT_Kind == KIND_MINE_SRV)
			return true;
		return false;
	}
}

// One admin text label on the map (2026-11-13). Built by INT_VPP_MapView from INT_VPP_Client.INT_GetLabels().
class INT_VPP_LabelItem : Managed
{
	int m_INT_Id;
	string m_INT_Text;
	int m_INT_Color;
	int m_INT_Size;
	vector m_INT_Pos;
	// view state
	Widget m_INT_Root;
	TextWidget m_INT_TextW;
	float m_INT_W;          // design px of the tree (text width + room)
	float m_INT_H;
	float m_INT_TextDW;     // estimated design width of the text itself (hit test)
	float m_INT_SX;
	float m_INT_SY;
	bool m_INT_OnScreen;

	bool INT_SameLook(INT_VPP_Label l)
	{
		if (!l)
			return false;
		if (l.INT_Text != m_INT_Text)
			return false;
		if (l.INT_Color != m_INT_Color)
			return false;
		if (l.INT_Size != m_INT_Size)
			return false;
		return true;
	}
}

class INT_VPP_MapView : Managed
{
	static const int INT_SORT = 300;
	static const int INT_MAX_LINES = 1800;
	static const int INT_LIVE_RESERVE = 120;
	static const float INT_HIT_R = 16;
	static const float INT_BOX = 48;
	static const int INT_LABEL = 0xFFF3EFE8;
	static const int INT_SUB = 0xFFE5E0D8;
	static const int INT_OUTLINE = 0xFF0B0B0B;
	static const int INT_AMBER = 0xFFFFC246;
	static const int INT_MARKBACK = 0xC00B0B0B;
	static const int INT_DIA_BACK = 0xE0141312;
	static const int INT_PLATE = 0xFF0B0B0B;

	protected MapWidget m_INT_Map;
	protected Widget m_INT_Host;
	protected CanvasWidget m_INT_Canvas;
	protected CanvasWidget m_INT_Live;
	protected Widget m_INT_Marks;
	// sub layers (draw order): zone labels, markers, route, ruler, me, top (hover / selection / ghost)
	protected Widget m_INT_LZones;
	protected Widget m_INT_LLabels;
	protected Widget m_INT_LMarks;
	protected Widget m_INT_LRoute;
	protected Widget m_INT_LRuler;
	protected Widget m_INT_LMe;
	protected Widget m_INT_LTop;
	protected bool m_INT_Bound;

	protected ref array<ref INT_VPP_MapItem> m_INT_Items;
	// zone labels (one tree per zone)
	protected ref array<Widget> m_INT_ZoneTrees;
	protected ref array<int> m_INT_ZoneTreeIds;
	// route widgets: point diamonds + leg plates (one tree each)
	protected ref array<Widget> m_INT_RoutePts;
	protected ref array<TextWidget> m_INT_RoutePtNums;
	protected ref array<Widget> m_INT_RouteLegs;
	protected ref array<TextWidget> m_INT_RouteLegTexts;
	protected ref array<float> m_INT_RouteLegW;
	protected Widget m_INT_RouteFinish;
	// ruler widgets
	protected ref array<Widget> m_INT_RulerPts;
	protected ref array<Widget> m_INT_RulerLegs;
	protected ref array<vector> m_INT_RulerWorld;
	protected bool m_INT_RulerOn;
	// me
	protected Widget m_INT_MeRoot;
	protected ImageWidget m_INT_MeArrow;
	protected bool m_INT_MeShown;
	protected vector m_INT_MePos;
	protected float m_INT_MeHeading;
	// hover / selection / ghost
	protected Widget m_INT_HoverBox;
	protected Widget m_INT_SelBox;
	protected INT_VPP_MapItem m_INT_Hover;
	protected INT_VPP_MapItem m_INT_Sel;
	protected Widget m_INT_Ghost;
	protected ImageWidget m_INT_GhostIcon;
	protected bool m_INT_GhostOn;
	protected vector m_INT_GhostPos;
	protected int m_INT_GhostColor;

	// zone draft (zone editor): shape 0 polygon / 1 circle; edited zone id hidden from the normal layer
	protected ref array<float> m_INT_DraftXs;
	protected ref array<float> m_INT_DraftZs;
	protected int m_INT_DraftShape;
	protected bool m_INT_DraftOn;
	protected bool m_INT_DraftClosed;
	protected float m_INT_DraftCX;
	protected float m_INT_DraftCZ;
	protected float m_INT_DraftR;
	protected int m_INT_DraftColor;
	protected int m_INT_DraftStage;
	protected int m_INT_SkipZoneId;

	// view cache
	protected float m_INT_MapX;
	protected float m_INT_MapY;
	protected float m_INT_MapW;
	protected float m_INT_MapH;
	protected vector m_INT_LastPos;
	protected float m_INT_LastScale = -1;
	protected float m_INT_F = 1;
	// cursor (map-local px) for rubber bands
	protected float m_INT_CurX;
	protected float m_INT_CurY;
	protected bool m_INT_CurIn;

	// dirty flags
	protected bool m_INT_DirtyItems;
	protected bool m_INT_DirtyZones;
	protected bool m_INT_DirtyRoute;
	protected bool m_INT_DirtyRuler;
	protected bool m_INT_DirtyLines;
	protected bool m_INT_DirtyLive;
	protected bool m_INT_DirtyProject;

	protected int m_INT_Lines;
	protected int m_INT_LiveLines;
	protected int m_INT_ProjectN;

	// review #7: marker trees only for markers on / near the screen, a few per frame; trees far off screen go
	static const int INT_BUILD_BUDGET = 40;
	static const float INT_DROP_PAD = 500;
	protected bool m_INT_BuildPending;
	protected bool m_INT_DirtyDeclutter;
	protected bool m_INT_DirtyMe;
	protected ref array<bool> m_INT_LayerCache;
	// scratch (cleared, never re-allocated per redraw / per change)
	protected ref array<ref INT_VPP_MapItem> m_INT_OldItems;
	protected ref array<vector> m_INT_ScrChain;
	// review #12: zone outlines of the last redraw (self-test): drawn (whole or simplified), off screen, not drawn
	protected int m_INT_ZonesDrawn;
	protected int m_INT_ZonesCulled;
	protected int m_INT_ZonesMissed;
	protected int m_INT_ZonesSimplified;
	// 2026-11-13 (no hatch): lines the zone pass drew and the per-zone outline share of the last redraw (self-test)
	protected int m_INT_ZoneLines;
	protected int m_INT_ZoneShare;

	// admin text labels (2026-11-13): one item per label of INT_VPP_Client; a tree only on / near the screen
	static const int INT_LABEL_BUILD_BUDGET = 20;
	static const int INT_LABEL_OUTLINE = 0xFF0B0B0B;
	protected ref array<ref INT_VPP_LabelItem> m_INT_LabelItems;
	protected ref array<ref INT_VPP_LabelItem> m_INT_OldLabels;
	protected bool m_INT_DirtyLabels;
	protected bool m_INT_LabelBuildPending;
	protected int m_INT_SkipLabelId;
	// the label editor's live preview (its own item; counted as kind 10 like the others)
	protected ref INT_VPP_LabelItem m_INT_LabelDraft;
	protected bool m_INT_LabelDraftOn;
	protected bool m_INT_LabelDraftHasPos;
	static int s_INT_LabelTreesBuilt;
	static int s_INT_LabelTreesKept;
	static int s_INT_TreesBuilt;
	static int s_INT_TreesKept;
	// label de-clutter: screen rects (x1, y1, x2, y2) of the labels shown in this projection; centres of the icons
	static const int INT_MAX_LABELS = 200;
	protected ref array<float> m_INT_LabelRects;
	protected ref array<float> m_INT_IconXY;

	// self-test counters
	static int s_INT_Rebuilds;
	static int s_INT_Projects;

	void INT_VPP_MapView()
	{
		m_INT_Items = new array<ref INT_VPP_MapItem>();
		m_INT_ZoneTrees = new array<Widget>();
		m_INT_ZoneTreeIds = new array<int>();
		m_INT_RoutePts = new array<Widget>();
		m_INT_RoutePtNums = new array<TextWidget>();
		m_INT_RouteLegs = new array<Widget>();
		m_INT_RouteLegTexts = new array<TextWidget>();
		m_INT_RouteLegW = new array<float>();
		m_INT_RulerPts = new array<Widget>();
		m_INT_RulerLegs = new array<Widget>();
		m_INT_RulerWorld = new array<vector>();
		m_INT_DraftXs = new array<float>();
		m_INT_DraftZs = new array<float>();
		m_INT_LabelRects = new array<float>();
		m_INT_IconXY = new array<float>();
		m_INT_SkipZoneId = -1;
		m_INT_LayerCache = new array<bool>();
		for (int l = 0; l < INT_VPP_LAYER_COUNT; l++)
		{
			m_INT_LayerCache.Insert(true);
		}
		m_INT_OldItems = new array<ref INT_VPP_MapItem>();
		m_INT_ScrChain = new array<vector>();
		m_INT_LabelItems = new array<ref INT_VPP_LabelItem>();
		m_INT_OldLabels = new array<ref INT_VPP_LabelItem>();
		m_INT_SkipLabelId = -1;
	}

	void ~INT_VPP_MapView()
	{
		INT_Destroy();
	}

	// ------------------------------------------------------------------ lifecycle
	bool INT_Bind(MapWidget mapW, Widget layoutRoot)
	{
		if (m_INT_Bound || !mapW || !layoutRoot)
			return false;
		m_INT_Map = mapW;
		m_INT_Host = layoutRoot.FindAnyWidget("INT_VPP_Overlay");
		m_INT_Canvas = CanvasWidget.Cast(layoutRoot.FindAnyWidget("INT_VPP_Canvas"));
		m_INT_Live = CanvasWidget.Cast(layoutRoot.FindAnyWidget("INT_VPP_CanvasLive"));
		m_INT_Marks = layoutRoot.FindAnyWidget("INT_VPP_Marks");
		if (!m_INT_Host || !m_INT_Canvas || !m_INT_Live || !m_INT_Marks)
		{
			Print("[INT_VPP] map overlay host missing in VPPMapMenu.layout");
			return false;
		}
		m_INT_F = INT_UIF_Scale.Factor();
		m_INT_LZones = INT_UIF_Draw.Frame(m_INT_Marks, 0, 0, 10, 10, "IntVppLZones");
		m_INT_LLabels = INT_UIF_Draw.Frame(m_INT_Marks, 0, 0, 10, 10, "IntVppLLabels");
		m_INT_LMarks = INT_UIF_Draw.Frame(m_INT_Marks, 0, 0, 10, 10, "IntVppLMarks");
		m_INT_LRoute = INT_UIF_Draw.Frame(m_INT_Marks, 0, 0, 10, 10, "IntVppLRoute");
		m_INT_LRuler = INT_UIF_Draw.Frame(m_INT_Marks, 0, 0, 10, 10, "IntVppLRuler");
		m_INT_LMe = INT_UIF_Draw.Frame(m_INT_Marks, 0, 0, 10, 10, "IntVppLMe");
		m_INT_LTop = INT_UIF_Draw.Frame(m_INT_Marks, 0, 0, 10, 10, "IntVppLTop");
		INT_BuildMe();
		INT_BuildBoxes();
		INT_VPPX_Look.SortTree(m_INT_Host, INT_SORT);
		m_INT_Bound = true;
		INT_VPP_Stat.INT_Up(1);
		m_INT_DirtyItems = true;
		m_INT_DirtyZones = true;
		m_INT_DirtyRoute = true;
		m_INT_DirtyRuler = true;
		m_INT_DirtyLines = true;
		m_INT_DirtyLive = true;
		m_INT_DirtyProject = true;
		m_INT_DirtyLabels = true;
		return true;
	}

	// Unlinks every tree this view made (the host itself goes with the menu's layoutRoot).
	void INT_Destroy()
	{
		if (!m_INT_Bound)
			return;
		m_INT_Bound = false;
		INT_ClearItemWidgets();
		INT_ClearZoneTrees();
		INT_ClearLabelTrees();
		INT_DropLabelTree(m_INT_LabelDraft);
		m_INT_LabelDraft = null;
		m_INT_LabelDraftOn = false;
		m_INT_LabelItems.Clear();
		m_INT_OldLabels.Clear();
		INT_ClearRouteTrees();
		INT_ClearRulerTrees();
		INT_KillTree(m_INT_MeRoot);
		m_INT_MeRoot = null;
		m_INT_MeArrow = null;
		INT_KillTree(m_INT_HoverBox);
		m_INT_HoverBox = null;
		INT_KillTree(m_INT_SelBox);
		m_INT_SelBox = null;
		INT_KillTree(m_INT_Ghost);
		m_INT_Ghost = null;
		m_INT_GhostIcon = null;
		if (m_INT_Canvas)
			m_INT_Canvas.Clear();
		if (m_INT_Live)
			m_INT_Live.Clear();
		INT_VPP_Stat.INT_CanvasLines = 0;
		INT_Unlink(m_INT_LZones);
		INT_Unlink(m_INT_LLabels);
		INT_Unlink(m_INT_LMarks);
		INT_Unlink(m_INT_LRoute);
		INT_Unlink(m_INT_LRuler);
		INT_Unlink(m_INT_LMe);
		INT_Unlink(m_INT_LTop);
		m_INT_LZones = null;
		m_INT_LLabels = null;
		m_INT_LMarks = null;
		m_INT_LRoute = null;
		m_INT_LRuler = null;
		m_INT_LMe = null;
		m_INT_LTop = null;
		m_INT_Items.Clear();
		m_INT_OldItems.Clear();
		m_INT_Hover = null;
		m_INT_Sel = null;
		m_INT_Canvas = null;
		m_INT_Live = null;
		m_INT_Marks = null;
		m_INT_Host = null;
		m_INT_Map = null;
		INT_VPP_Stat.INT_Down(1);
	}

	bool INT_IsBound()
	{
		return m_INT_Bound;
	}

	protected void INT_Unlink(Widget w)
	{
		if (w)
			w.Unlink();
	}

	// an overlay tree (marker trees are counted as INT_VPP_Stat kind 2 in INT_BuildItem / INT_ClearItemWidgets;
	// me, hover / selection boxes, ghost, zone labels, route and ruler widgets are not markers: uncounted)
	protected void INT_KillTree(Widget w)
	{
		if (!w)
			return;
		w.Unlink();
	}

	protected Widget INT_NewTree(Widget layer, float w, float h, string nm)
	{
		if (!layer)
			return null;
		return INT_UIF_Draw.Frame(layer, 0, 0, w, h, nm);
	}

	protected void INT_Sorted(Widget root)
	{
		INT_VPPX_Look.SortTree(root, INT_SORT);
	}

	// ------------------------------------------------------------------ dirty marks (callers)
	// The menu makes new items on every change; a new item of the same MarkerInfo with the same look takes over the
	// old item's tree (review #7: no rebuild-all; an RF marker moved in place keeps its widgets). Items usually keep
	// their order: the cursor finds the donor first, a full search only for the changed ones.
	void INT_SetItems(array<ref INT_VPP_MapItem> items)
	{
		m_INT_OldItems.Clear();
		foreach (INT_VPP_MapItem was : m_INT_Items)
		{
			if (was)
				m_INT_OldItems.Insert(was);
		}
		m_INT_Items.Clear();
		INT_VPP_MapItem hoverWas = m_INT_Hover;
		INT_VPP_MapItem selWas = m_INT_Sel;
		m_INT_Hover = null;
		m_INT_Sel = null;
		int oldN = m_INT_OldItems.Count();
		int cursor = 0;
		if (items)
		{
			foreach (INT_VPP_MapItem it : items)
			{
				if (!it)
					continue;
				m_INT_Items.Insert(it);
				int at = INT_FindDonor(it, cursor, oldN);
				if (at < 0)
					continue;
				cursor = at + 1;
				INT_VPP_MapItem donor = m_INT_OldItems.Get(at);
				INT_Adopt(it, donor);
				if (donor == hoverWas)
					m_INT_Hover = it;
				if (donor == selWas)
					m_INT_Sel = it;
				m_INT_OldItems.Set(at, null);
			}
		}
		// items that are gone: their trees go
		for (int k = 0; k < oldN; k++)
		{
			INT_VPP_MapItem gone = m_INT_OldItems.Get(k);
			if (gone)
				INT_DropItemTree(gone);
		}
		m_INT_OldItems.Clear();
		m_INT_DirtyItems = true;
	}

	protected int INT_FindDonor(INT_VPP_MapItem it, int cursor, int oldN)
	{
		if (!it.m_INT_Info)
			return -1;
		if (cursor < oldN)
		{
			INT_VPP_MapItem c = m_INT_OldItems.Get(cursor);
			if (c && c.m_INT_Info == it.m_INT_Info)
				return cursor;
		}
		for (int j = 0; j < oldN; j++)
		{
			INT_VPP_MapItem o = m_INT_OldItems.Get(j);
			if (o && o.m_INT_Info == it.m_INT_Info)
				return j;
		}
		return -1;
	}

	// the new item takes the donor's tree when the look is the same (else the tree goes; rebuilt lazily)
	protected void INT_Adopt(INT_VPP_MapItem it, INT_VPP_MapItem donor)
	{
		if (!donor.m_INT_Root)
			return;
		bool same = true;
		if (donor.m_INT_Kind != it.m_INT_Kind || donor.m_INT_Layer != it.m_INT_Layer)
			same = false;
		if (donor.m_INT_IconPath != it.m_INT_IconPath || donor.m_INT_Color != it.m_INT_Color)
			same = false;
		if (donor.m_INT_Name != it.m_INT_Name)
			same = false;
		if (!same)
		{
			INT_DropItemTree(donor);
			return;
		}
		it.m_INT_Root = donor.m_INT_Root;
		it.m_INT_NameW = donor.m_INT_NameW;
		it.m_INT_DistW = donor.m_INT_DistW;
		it.m_INT_DistText = donor.m_INT_DistText;
		it.m_INT_LabelW = donor.m_INT_LabelW;
		it.m_INT_LabelOn = donor.m_INT_LabelOn;
		it.m_INT_SX = donor.m_INT_SX;
		it.m_INT_SY = donor.m_INT_SY;
		it.m_INT_OnScreen = donor.m_INT_OnScreen;
		donor.m_INT_Root = null;
		donor.m_INT_NameW = null;
		donor.m_INT_DistW = null;
		s_INT_TreesKept++;
	}

	protected void INT_DropItemTree(INT_VPP_MapItem it)
	{
		if (!it)
			return;
		if (it.m_INT_Root)
		{
			INT_KillTree(it.m_INT_Root);
			INT_VPP_Stat.INT_Down(2);
		}
		it.m_INT_Root = null;
		it.m_INT_NameW = null;
		it.m_INT_DistW = null;
		it.m_INT_DistText = "";
		it.m_INT_OnScreen = false;
		it.m_INT_LabelOn = false;
	}

	array<ref INT_VPP_MapItem> INT_Items()
	{
		return m_INT_Items;
	}

	void INT_MarkLayers()
	{
		m_INT_DirtyItems = true;
		m_INT_DirtyZones = true;
		m_INT_DirtyRoute = true;
		m_INT_DirtyLines = true;
		m_INT_DirtyLabels = true;
	}

	// the label list changed (INT_VPP_Client.m_INT_OnLabels): items follow on the next frame (trees kept when same look)
	void INT_MarkLabels()
	{
		m_INT_DirtyLabels = true;
	}

	// one marker's visibility changed (list chip): only its want flag; its tree goes / comes through the lazy path
	void INT_RefreshItem(INT_VPP_MapItem it)
	{
		if (!it)
			return;
		bool want = false;
		if (it.m_INT_Visible && m_INT_LMarks)
		{
			int layer = it.m_INT_Layer;
			if (layer >= 0 && layer < INT_VPP_LAYER_COUNT && layer < m_INT_LayerCache.Count())
				want = m_INT_LayerCache.Get(layer);
			else
				want = true;
		}
		it.m_INT_Want = want;
		if (!want && it.m_INT_Root)
			INT_DropItemTree(it);
		if (m_INT_Hover == it && !want)
			m_INT_Hover = null;
		if (m_INT_Sel == it && !want)
			m_INT_Sel = null;
		m_INT_DirtyProject = true;
	}

	void INT_MarkZones()
	{
		m_INT_DirtyZones = true;
		m_INT_DirtyLines = true;
	}

	void INT_MarkRoute()
	{
		m_INT_DirtyRoute = true;
		m_INT_DirtyLines = true;
	}

	// ------------------------------------------------------------------ per frame
	void INT_Tick(float dt)
	{
		if (!m_INT_Bound || !m_INT_Map)
			return;
		if (INT_ViewChanged())
		{
			m_INT_DirtyProject = true;
			m_INT_DirtyLines = true;
			m_INT_DirtyLive = true;
		}
		if (m_INT_DirtyItems)
			INT_RebuildItems();
		if (m_INT_DirtyZones)
			INT_RebuildZoneTrees();
		if (m_INT_DirtyLabels)
			INT_RebuildLabelItems();
		if (m_INT_DirtyRoute)
			INT_RebuildRouteTrees();
		if (m_INT_DirtyRuler)
			INT_RebuildRulerTrees();
		if (m_INT_DirtyProject)
			INT_Project();
		else if (m_INT_DirtyMe)
			INT_ProjectMe();
		m_INT_DirtyMe = false;
		if (m_INT_BuildPending)
			INT_BuildSome();
		if (m_INT_LabelBuildPending)
			INT_BuildSomeLabels();
		if (m_INT_DirtyDeclutter)
		{
			m_INT_DirtyDeclutter = false;
			INT_Declutter();
			INT_PlaceBox(m_INT_HoverBox, m_INT_Hover);
			INT_PlaceBox(m_INT_SelBox, m_INT_Sel);
		}
		if (m_INT_DirtyLines)
			INT_RedrawLines();
		if (m_INT_DirtyLive)
			INT_RedrawLive();
		int total = m_INT_Lines + m_INT_LiveLines;
		INT_VPP_Stat.INT_CanvasLines = total;
	}

	// trees for wanted markers that are on screen but have none yet: at most INT_BUILD_BUDGET per frame
	protected void INT_BuildSome()
	{
		m_INT_BuildPending = false;
		if (!m_INT_LMarks)
			return;
		int budget = INT_BUILD_BUDGET;
		float half = INT_UIF_Draw.S(INT_BOX * 0.5);
		foreach (INT_VPP_MapItem it : m_INT_Items)
		{
			if (!it || !it.m_INT_Want || it.m_INT_Root || !it.m_INT_OnScreen)
				continue;
			if (budget <= 0)
			{
				m_INT_BuildPending = true;
				break;
			}
			budget--;
			INT_BuildItem(it);
			if (!it.m_INT_Root)
				continue;
			INT_ShowAt(it.m_INT_Root, it.m_INT_SX, it.m_INT_SY, half, half, true);
			if (m_INT_MeShown)
				INT_DistanceOf(it, m_INT_MePos);
			m_INT_DirtyDeclutter = true;
		}
	}

	// the walk starts at me: its first leg follows every step
	protected bool INT_RouteFromMe()
	{
		if (!m_INT_MeShown)
			return false;
		if (!INT_LayerOn(INT_VPP_LAYER_ROUTE))
			return false;
		INT_VPP_Route route = INT_VPP_Route.INT_Peek();
		if (!route || !route.INT_IsActive())
			return false;
		return true;
	}

	// only me (and the route legs that start at me) moved: no marker re-projection
	protected void INT_ProjectMe()
	{
		float sx;
		float sy;
		if (m_INT_MeRoot)
		{
			bool meOn = false;
			if (m_INT_MeShown)
				meOn = INT_ToLocal(m_INT_MePos, sx, sy);
			float mh = INT_UIF_Draw.S(24);
			INT_ShowAt(m_INT_MeRoot, sx, sy, mh, mh, meOn);
		}
		if (INT_RouteFromMe())
			INT_ProjectRoute();
	}

	protected bool INT_ViewChanged()
	{
		float scale = m_INT_Map.GetScale();
		vector pos = m_INT_Map.GetMapPos();
		float x;
		float y;
		float w;
		float h;
		m_INT_Map.GetScreenPos(x, y);
		m_INT_Map.GetScreenSize(w, h);
		bool same = true;
		if (Math.AbsFloat(scale - m_INT_LastScale) > 0.0000001)
			same = false;
		if (Math.AbsFloat(pos[0] - m_INT_LastPos[0]) > 0.001 || Math.AbsFloat(pos[2] - m_INT_LastPos[2]) > 0.001)
			same = false;
		if (x != m_INT_MapX || y != m_INT_MapY || w != m_INT_MapW || h != m_INT_MapH)
			same = false;
		if (same)
			return false;
		m_INT_LastScale = scale;
		m_INT_LastPos = pos;
		m_INT_MapX = x;
		m_INT_MapY = y;
		m_INT_MapW = w;
		m_INT_MapH = h;
		return true;
	}

	// world -> map-local screen px; false when far outside the map rect
	bool INT_ToLocal(vector world, out float sx, out float sy)
	{
		sx = 0;
		sy = 0;
		if (!m_INT_Map)
			return false;
		vector sp = m_INT_Map.MapToScreen(world);
		sx = sp[0] - m_INT_MapX;
		sy = sp[1] - m_INT_MapY;
		float pad = 60 * m_INT_F;
		if (sx < -pad || sy < -pad || sx > m_INT_MapW + pad || sy > m_INT_MapH + pad)
			return false;
		return true;
	}

	// world -> absolute screen px (cards in the island layer)
	bool INT_ToScreen(vector world, out float ax, out float ay)
	{
		ax = 0;
		ay = 0;
		if (!m_INT_Map)
			return false;
		vector sp = m_INT_Map.MapToScreen(world);
		ax = sp[0];
		ay = sp[1];
		if (ax < m_INT_MapX || ay < m_INT_MapY || ax > m_INT_MapX + m_INT_MapW || ay > m_INT_MapY + m_INT_MapH)
			return false;
		return true;
	}

	// absolute screen px -> world (y = 0)
	vector INT_ScreenToWorld(float ax, float ay)
	{
		if (!m_INT_Map)
			return vector.Zero;
		vector sp = Vector(ax, ay, 0);
		vector w = m_INT_Map.ScreenToMap(sp);
		w[1] = 0;
		return w;
	}

	// metres per screen px at the current view (approximate, from two projected points)
	float INT_Factor()
	{
		return m_INT_F;
	}

	float INT_MetresPerPx()
	{
		if (!m_INT_Map)
			return 1;
		vector c = m_INT_Map.GetMapPos();
		vector e = c;
		e[0] = e[0] + 100;
		vector a = m_INT_Map.MapToScreen(c);
		vector b = m_INT_Map.MapToScreen(e);
		float dpx = Math.AbsFloat(b[0] - a[0]);
		if (dpx < 0.001)
			return 1;
		return 100.0 / dpx;
	}

	void INT_SetCursor(float ax, float ay, bool inside)
	{
		float lx = ax - m_INT_MapX;
		float ly = ay - m_INT_MapY;
		bool changed = false;
		if (Math.AbsFloat(lx - m_INT_CurX) > 0.5 || Math.AbsFloat(ly - m_INT_CurY) > 0.5 || inside != m_INT_CurIn)
			changed = true;
		m_INT_CurX = lx;
		m_INT_CurY = ly;
		m_INT_CurIn = inside;
		if (changed && INT_NeedsRubber())
			m_INT_DirtyLive = true;
	}

	protected bool INT_NeedsRubber()
	{
		if (m_INT_RulerOn && m_INT_RulerWorld.Count() > 0)
			return true;
		if (m_INT_DraftOn && !m_INT_DraftClosed)
			return true;
		return false;
	}

	// ------------------------------------------------------------------ markers
	protected void INT_ClearItemWidgets()
	{
		foreach (INT_VPP_MapItem it : m_INT_Items)
		{
			if (it)
				INT_DropItemTree(it);
		}
	}

	// review #7: no tree is built here any more - only which markers want one (visible, layer on); trees of the
	// others go now, the wanted ones are built lazily when the projection finds them on screen (INT_BuildSome)
	protected void INT_RebuildItems()
	{
		m_INT_DirtyItems = false;
		s_INT_Rebuilds++;
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		for (int l = 0; l < INT_VPP_LAYER_COUNT; l++)
		{
			bool lon = true;
			if (prefs)
				lon = prefs.INT_IsLayerOn(l);
			m_INT_LayerCache.Set(l, lon);
		}
		foreach (INT_VPP_MapItem it : m_INT_Items)
		{
			if (!it)
				continue;
			bool want = false;
			if (it.m_INT_Visible && m_INT_LMarks)
			{
				int layer = it.m_INT_Layer;
				if (layer >= 0 && layer < INT_VPP_LAYER_COUNT)
					want = m_INT_LayerCache.Get(layer);
				else
					want = true;
			}
			it.m_INT_Want = want;
			if (!want && it.m_INT_Root)
				INT_DropItemTree(it);
		}
		m_INT_DirtyProject = true;
		if (m_INT_Hover && !m_INT_Hover.m_INT_Want)
			m_INT_Hover = null;
		if (m_INT_Sel && !m_INT_Sel.m_INT_Want)
			m_INT_Sel = null;
	}

	protected void INT_BuildItem(INT_VPP_MapItem it)
	{
		float r = INT_BOX * 0.5;
		Widget root = INT_NewTree(m_INT_LMarks, INT_BOX, INT_BOX, "IntVppMk");
		if (!root)
			return;
		INT_VPP_Stat.INT_Up(2);
		it.m_INT_Root = root;
		int labelColor = INT_LABEL;
		if (it.m_INT_Kind == INT_VPP_MapItem.KIND_SERVER)
		{
			INT_VPPX_Look.Diamond(root, r, r, 24, true, INT_DIA_BACK);
			INT_VPPX_Look.Diamond(root, r, r, 24, false, INT_VPPX_Look.WHITE);
			INT_UIF_Draw.Texture(root, r - 9, r - 9, 18, 18, it.m_INT_IconPath, it.m_INT_Color);
		}
		else if (it.m_INT_Kind == INT_VPP_MapItem.KIND_EVENT)
		{
			INT_VPPX_Look.Diamond(root, r, r, 21, true, INT_DIA_BACK);
			INT_VPPX_Look.Diamond(root, r, r, 21, false, INT_AMBER);
			INT_UIF_Draw.Texture(root, r - 8, r - 8, 16, 16, it.m_INT_IconPath, it.m_INT_Color);
			labelColor = INT_AMBER;
		}
		else
		{
			INT_UIF_Draw.Texture(root, r - 16, r - 16, 32, 32, INT_VPP_Icons.INT_UI_MARKBACK, INT_MARKBACK);
			INT_UIF_Draw.Texture(root, r - 12, r - 12, 24, 24, it.m_INT_IconPath, it.m_INT_Color);
		}
		// small screens (1280x960: factor 0.67 -> 10 px names) get a step larger labels (QA 2026-11-12)
		float nameSize = 15;
		float distSize = 12;
		float nameY = r - 14;
		float distY = r + 5;
		if (m_INT_F < 0.8)
		{
			nameSize = 18;
			distSize = 14;
			nameY = r - 17;
			distY = r + 5;
		}
		string nm = it.m_INT_Name;
		float nw = INT_VPPX_Look.Measure(nm, nameSize);
		float room = INT_VPPX_Look.Room(nw);
		TextWidget t = INT_VPPX_Look.Text(root, r + 19, nameY, room, nameSize + 5, nm, nameSize, labelColor);
		if (t)
			t.SetOutline(1, INT_OUTLINE);
		TextWidget d = INT_VPPX_Look.Text(root, r + 19, distY, 160, distSize + 4, " ", distSize, INT_SUB);
		if (d)
			d.SetOutline(1, INT_OUTLINE);
		it.m_INT_NameW = t;
		it.m_INT_DistW = d;
		it.m_INT_DistText = "";
		// label width for the de-clutter (design px): the name or a distance line ("1,2 км"), the wider one
		float lw = nw;
		float minW = distSize * 4.6;
		if (lw < minW)
			lw = minW;
		it.m_INT_LabelW = lw;
		it.m_INT_LabelOn = true;
		INT_Sorted(root);
		root.Show(false);
		s_INT_TreesBuilt++;
	}

	protected void INT_DistanceOf(INT_VPP_MapItem it, vector me)
	{
		if (!it.m_INT_DistW)
			return;
		float dm = INT_VPP_Fmt.INT_Dist2D(me, it.m_INT_Pos);
		string s = INT_VPP_Fmt.INT_Dist(dm);
		if (s == it.m_INT_DistText)
			return;
		it.m_INT_DistText = s;
		it.m_INT_DistW.SetText(s);
	}

	// distance lines under the names (on open and whenever the player moved): only markers that have a tree, and
	// SetText only when the shown text changes; the markers are not re-projected (review #7)
	void INT_UpdateDistances(vector me)
	{
		foreach (INT_VPP_MapItem it : m_INT_Items)
		{
			if (it && it.m_INT_DistW)
				INT_DistanceOf(it, me);
		}
		m_INT_MePos = me;
		m_INT_DirtyMe = true;
		// the first leg starts at me: its line follows
		if (INT_RouteFromMe())
			m_INT_DirtyLines = true;
	}

	INT_VPP_MapItem INT_HitTest(float ax, float ay)
	{
		float lx = ax - m_INT_MapX;
		float ly = ay - m_INT_MapY;
		float rr = INT_HIT_R * m_INT_F;
		float best = rr * rr;
		INT_VPP_MapItem found = null;
		foreach (INT_VPP_MapItem it : m_INT_Items)
		{
			if (!it || !it.m_INT_Root || !it.m_INT_OnScreen)
				continue;
			float dx = it.m_INT_SX - lx;
			float dy = it.m_INT_SY - ly;
			float d2 = dx * dx + dy * dy;
			if (d2 <= best)
			{
				best = d2;
				found = it;
			}
		}
		return found;
	}

	// ------------------------------------------------------------------ me / boxes / ghost
	protected void INT_BuildMe()
	{
		m_INT_MeRoot = INT_NewTree(m_INT_LMe, 48, 48, "IntVppMe");
		if (!m_INT_MeRoot)
			return;
		INT_UIF_Draw.Texture(m_INT_MeRoot, 3, 3, 42, 42, INT_VPP_Icons.INT_UI_ME_RING, INT_VPPX_Look.ORANGE);
		m_INT_MeArrow = INT_UIF_Draw.Texture(m_INT_MeRoot, 11, 11, 26, 26, INT_VPP_Icons.INT_UI_ME, INT_VPPX_Look.ORANGE);
		TextWidget t = INT_VPPX_Look.Caps(m_INT_MeRoot, 48, 13, 40, 18, "Я", 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, false);
		if (t)
			t.SetOutline(1, INT_OUTLINE);
		m_INT_MeRoot.Show(false);
	}

	protected void INT_BuildBoxes()
	{
		m_INT_HoverBox = INT_NewTree(m_INT_LTop, 40, 40, "IntVppHover");
		if (m_INT_HoverBox)
		{
			INT_VPPX_Look.Outline(m_INT_HoverBox, 0, 0, 40, 40, 0x99F3EFE8);
			m_INT_HoverBox.Show(false);
		}
		m_INT_SelBox = INT_NewTree(m_INT_LTop, 40, 40, "IntVppSel");
		if (m_INT_SelBox)
		{
			INT_VPPX_Look.Outline(m_INT_SelBox, 0, 0, 40, 40, INT_VPPX_Look.ORANGE);
			m_INT_SelBox.Show(false);
		}
	}

	void INT_SetMe(bool shown, vector pos, float heading)
	{
		// the leg plates depend on whether the walk starts at me: rebuild only when that flips
		if (shown != m_INT_MeShown)
			m_INT_DirtyRoute = true;
		m_INT_MeShown = shown;
		m_INT_MePos = pos;
		m_INT_MeHeading = heading;
		if (m_INT_MeArrow)
			m_INT_MeArrow.SetRotation(0, 0, heading);
		m_INT_DirtyMe = true;
		if (INT_RouteFromMe())
			m_INT_DirtyLines = true;
	}

	void INT_SetHeading(float heading)
	{
		m_INT_MeHeading = heading;
		if (m_INT_MeArrow)
			m_INT_MeArrow.SetRotation(0, 0, heading);
	}

	bool INT_IsMeShown()
	{
		return m_INT_MeShown;
	}

	void INT_SetHover(INT_VPP_MapItem it)
	{
		if (it == m_INT_Hover)
			return;
		m_INT_Hover = it;
		INT_PlaceBox(m_INT_HoverBox, m_INT_Hover);
	}

	void INT_SetSelected(INT_VPP_MapItem it)
	{
		m_INT_Sel = it;
		INT_PlaceBox(m_INT_SelBox, m_INT_Sel);
		// the selected marker's label goes first in the de-clutter
		m_INT_DirtyProject = true;
	}

	INT_VPP_MapItem INT_GetSelected()
	{
		return m_INT_Sel;
	}

	protected void INT_PlaceBox(Widget box, INT_VPP_MapItem it)
	{
		if (!box)
			return;
		bool show = false;
		if (it && it.m_INT_Root && it.m_INT_OnScreen)
			show = true;
		if (show)
		{
			float half = INT_UIF_Draw.S(20);
			box.SetPos(it.m_INT_SX - half, it.m_INT_SY - half);
		}
		if (box.IsVisible() != show)
			box.Show(show);
	}

	// editor ghost: icon + two rings at a world point (null path = off)
	void INT_SetGhost(bool on, vector pos, string iconPath, int color)
	{
		m_INT_GhostOn = on;
		m_INT_GhostPos = pos;
		m_INT_GhostColor = color;
		if (!on)
		{
			INT_KillTree(m_INT_Ghost);
			m_INT_Ghost = null;
			m_INT_GhostIcon = null;
			m_INT_DirtyLive = true;
			return;
		}
		if (!m_INT_Ghost)
		{
			m_INT_Ghost = INT_NewTree(m_INT_LTop, 48, 48, "IntVppGhost");
			if (m_INT_Ghost)
			{
				INT_UIF_Draw.Texture(m_INT_Ghost, 8, 8, 32, 32, INT_VPP_Icons.INT_UI_MARKBACK, INT_MARKBACK);
				m_INT_GhostIcon = INT_UIF_Draw.Texture(m_INT_Ghost, 11, 11, 26, 26, iconPath, color);
				INT_Sorted(m_INT_Ghost);
			}
		}
		else if (m_INT_GhostIcon)
		{
			m_INT_GhostIcon.LoadImageFile(0, iconPath);
			m_INT_GhostIcon.SetImage(0);
			m_INT_GhostIcon.SetColor(color);
		}
		m_INT_DirtyProject = true;
		m_INT_DirtyLive = true;
	}

	// ------------------------------------------------------------------ ruler
	void INT_SetRuler(bool on, array<vector> pts)
	{
		m_INT_RulerOn = on;
		m_INT_RulerWorld.Clear();
		if (on && pts)
		{
			foreach (vector p : pts)
			{
				m_INT_RulerWorld.Insert(p);
			}
		}
		m_INT_DirtyRuler = true;
		m_INT_DirtyLines = true;
		m_INT_DirtyLive = true;
	}

	// ------------------------------------------------------------------ zone draft
	void INT_SetDraft(bool on, int shape, array<float> xs, array<float> zs, float cx, float cz, float r, int color, bool closed, int stage, int skipId)
	{
		m_INT_DraftOn = on;
		m_INT_DraftShape = shape;
		m_INT_DraftXs.Clear();
		m_INT_DraftZs.Clear();
		if (xs && zs)
		{
			int n = xs.Count();
			if (zs.Count() < n)
				n = zs.Count();
			for (int i = 0; i < n; i++)
			{
				m_INT_DraftXs.Insert(xs.Get(i));
				m_INT_DraftZs.Insert(zs.Get(i));
			}
		}
		m_INT_DraftCX = cx;
		m_INT_DraftCZ = cz;
		m_INT_DraftR = r;
		m_INT_DraftColor = color;
		m_INT_DraftClosed = closed;
		m_INT_DraftStage = stage;
		if (skipId != m_INT_SkipZoneId)
		{
			m_INT_SkipZoneId = skipId;
			m_INT_DirtyZones = true;
		}
		m_INT_DirtyLines = true;
		m_INT_DirtyLive = true;
	}

	// draft vertex 0 within `px` design px of the cursor (closing a polygon by clicking the first point)
	bool INT_NearFirstVertex(float ax, float ay, float px)
	{
		if (m_INT_DraftXs.Count() < 3)
			return false;
		float vx0 = m_INT_DraftXs.Get(0);
		float vz0 = m_INT_DraftZs.Get(0);
		vector v = Vector(vx0, 0, vz0);
		float sx;
		float sy;
		if (!INT_ToLocal(v, sx, sy))
			return false;
		float lx = ax - m_INT_MapX;
		float ly = ay - m_INT_MapY;
		float dx = sx - lx;
		float dy = sy - ly;
		float rr = px * m_INT_F;
		if (dx * dx + dy * dy <= rr * rr)
			return true;
		return false;
	}

	// ------------------------------------------------------------------ zone labels
	protected void INT_ClearZoneTrees()
	{
		foreach (Widget w : m_INT_ZoneTrees)
		{
			INT_KillTree(w);
		}
		m_INT_ZoneTrees.Clear();
		m_INT_ZoneTreeIds.Clear();
	}

	protected bool INT_LayerOn(int layer)
	{
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return true;
		return prefs.INT_IsLayerOn(layer);
	}

	protected array<ref INT_VPP_Zone> INT_Zones()
	{
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl)
			return null;
		return cl.GetZones();
	}

	protected void INT_RebuildZoneTrees()
	{
		m_INT_DirtyZones = false;
		INT_ClearZoneTrees();
		if (!m_INT_LZones || !INT_LayerOn(INT_VPP_LAYER_ZONES))
			return;
		array<ref INT_VPP_Zone> zones = INT_Zones();
		if (!zones)
			return;
		foreach (INT_VPP_Zone z : zones)
		{
			if (!z || z.id == m_INT_SkipZoneId)
				continue;
			int col = INT_VPP_Validate.ForceAlpha(z.color);
			string capsName = INT_VPPX_Text.Caps(z.name);
			float nw = INT_VPPX_Look.Measure(capsName, 13, true);
			float sw = INT_VPPX_Look.Measure(z.sub, 12);
			float bw = nw;
			if (sw > bw)
				bw = sw;
			bw = bw + 40;
			Widget root = INT_NewTree(m_INT_LZones, bw, 36, "IntVppZoneLbl");
			if (!root)
				continue;
			TextWidget t = INT_VPPX_Look.Text(root, 0, 0, bw, 18, capsName, 13, col, INT_UIF_Align.CENTER, false);
			if (t)
			{
				t.SetBold(true);
				t.SetOutline(1, INT_OUTLINE);
			}
			if (z.sub != "")
			{
				TextWidget s = INT_VPPX_Look.Text(root, 0, 17, bw, 16, z.sub, 12, INT_SUB, INT_UIF_Align.CENTER, false);
				if (s)
					s.SetOutline(1, INT_OUTLINE);
			}
			INT_Sorted(root);
			root.Show(false);
			m_INT_ZoneTrees.Insert(root);
			m_INT_ZoneTreeIds.Insert(z.id);
		}
		m_INT_DirtyProject = true;
	}

	// ------------------------------------------------------------------ route
	protected void INT_ClearRouteTrees()
	{
		foreach (Widget p : m_INT_RoutePts)
		{
			INT_KillTree(p);
		}
		foreach (Widget l : m_INT_RouteLegs)
		{
			INT_KillTree(l);
		}
		m_INT_RoutePts.Clear();
		m_INT_RoutePtNums.Clear();
		m_INT_RouteLegs.Clear();
		m_INT_RouteLegTexts.Clear();
		m_INT_RouteLegW.Clear();
		INT_KillTree(m_INT_RouteFinish);
		m_INT_RouteFinish = null;
	}

	// chain of the walk: me (when shown), then the points from the next one on (passed points are not walked)
	protected int INT_RouteChain(array<vector> chain)
	{
		chain.Clear();
		INT_VPP_Route route = INT_VPP_Route.INT_Get();
		if (!route)
			return 0;
		int n = route.INT_Count();
		int next = route.INT_NextIndex();
		if (n <= 0 || next < 0)
			return 0;
		if (m_INT_MeShown)
			chain.Insert(m_INT_MePos);
		for (int i = next; i < n; i++)
		{
			INT_VPP_RoutePoint rp = route.INT_At(i);
			if (rp)
				chain.Insert(rp.m_INT_Pos);
		}
		return n;
	}

	protected void INT_RebuildRouteTrees()
	{
		m_INT_DirtyRoute = false;
		INT_ClearRouteTrees();
		if (!m_INT_LRoute || !INT_LayerOn(INT_VPP_LAYER_ROUTE))
			return;
		INT_VPP_Route route = INT_VPP_Route.INT_Get();
		if (!route)
			return;
		int n = route.INT_Count();
		int nextIdx = route.INT_NextIndex();
		if (n <= 0 || nextIdx < 0)
			return;
		array<vector> chain = m_INT_ScrChain;
		INT_RouteChain(chain);
		int legs = chain.Count() - 1;
		// leg plates (under the points)
		for (int l = 0; l < legs; l++)
		{
			vector a = chain.Get(l);
			vector b = chain.Get(l + 1);
			float lm = INT_VPP_Fmt.INT_Dist2D(a, b);
			string lt = INT_VPP_Fmt.INT_Dist(lm);
			float tw = INT_VPPX_Look.Measure(lt, 13) + 16;
			bool nextLeg = false;
			if (l == 0)
				nextLeg = true;
			Widget plate = INT_NewTree(m_INT_LRoute, tw, 22, "IntVppLeg");
			if (!plate)
				continue;
			int edge = 0x73FF7A2F;
			int txt = INT_VPPX_Look.WHITE;
			if (nextLeg)
			{
				edge = INT_VPPX_Look.ORANGE;
				txt = INT_VPPX_Look.ORANGE;
			}
			INT_VPPX_Look.Fill(plate, 0, 0, tw, 22, INT_PLATE);
			INT_VPPX_Look.Outline(plate, 0, 0, tw, 22, edge);
			TextWidget ltw = INT_VPPX_Look.Text(plate, 0, 0, tw, 22, lt, 13, txt, INT_UIF_Align.CENTER, false);
			INT_Sorted(plate);
			plate.Show(false);
			m_INT_RouteLegs.Insert(plate);
			m_INT_RouteLegTexts.Insert(ltw);
			m_INT_RouteLegW.Insert(tw);
		}
		// numbered diamonds: passed (dimmed, auto-remove off) / next (orange) / later
		for (int i = 0; i < n; i++)
		{
			INT_VPP_RoutePoint rp = route.INT_At(i);
			if (!rp)
				continue;
			bool isNext = false;
			if (i == nextIdx)
				isNext = true;
			bool passed = false;
			if (i < nextIdx)
				passed = true;
			Widget pt = INT_NewTree(m_INT_LRoute, 34, 34, "IntVppRpt");
			if (!pt)
				continue;
			int fill = 0xFF141312;
			int numCol = INT_VPPX_Look.WHITE;
			int edgeCol = INT_VPPX_Look.ORANGE;
			if (isNext)
			{
				fill = INT_VPPX_Look.ORANGE;
				numCol = INT_VPPX_Look.INK;
			}
			else if (passed)
			{
				edgeCol = INT_VPPX_Look.ORANGE_DIM;
				numCol = INT_VPPX_Look.FAINT;
			}
			INT_UIF_Draw.Texture(pt, 0, 0, 34, 34, INT_VPP_Icons.INT_UI_ROUTEPT_FILL, fill);
			INT_UIF_Draw.Texture(pt, 0, 0, 34, 34, INT_VPP_Icons.INT_UI_ROUTEPT, edgeCol);
			int num = route.INT_NumberOf(i);
			string ns = INT_VPPX_Text.Pad2(num);
			TextWidget nt = INT_VPPX_Look.Text(pt, 0, 0, 34, 34, ns, 13, numCol, INT_UIF_Align.CENTER, false);
			if (nt)
				nt.SetBold(true);
			INT_Sorted(pt);
			pt.Show(false);
			m_INT_RoutePts.Insert(pt);
			m_INT_RoutePtNums.Insert(nt);
		}
		if (n - nextIdx >= 2)
		{
			string fin = INT_VPPX_Text.Caps("финиш");
			float fw = INT_VPPX_Look.Measure(fin, 13, true) + 8;
			m_INT_RouteFinish = INT_NewTree(m_INT_LRoute, fw, 18, "IntVppFinish");
			if (m_INT_RouteFinish)
			{
				TextWidget ft = INT_VPPX_Look.Text(m_INT_RouteFinish, 0, 0, fw, 18, fin, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.RIGHT, false);
				if (ft)
				{
					ft.SetBold(true);
					ft.SetOutline(1, INT_OUTLINE);
				}
				INT_Sorted(m_INT_RouteFinish);
				m_INT_RouteFinish.Show(false);
			}
		}
		m_INT_DirtyProject = true;
		m_INT_DirtyLines = true;
	}

	// ------------------------------------------------------------------ ruler widgets
	protected void INT_ClearRulerTrees()
	{
		foreach (Widget p : m_INT_RulerPts)
		{
			INT_KillTree(p);
		}
		foreach (Widget l : m_INT_RulerLegs)
		{
			INT_KillTree(l);
		}
		m_INT_RulerPts.Clear();
		m_INT_RulerLegs.Clear();
	}

	protected void INT_RebuildRulerTrees()
	{
		m_INT_DirtyRuler = false;
		INT_ClearRulerTrees();
		if (!m_INT_RulerOn || !m_INT_LRuler)
			return;
		int n = m_INT_RulerWorld.Count();
		for (int i = 0; i < n; i++)
		{
			Widget pt = INT_NewTree(m_INT_LRuler, 16, 16, "IntVppRulPt");
			if (!pt)
				continue;
			INT_VPPX_Look.Diamond(pt, 8, 8, 11, true, INT_VPPX_Look.WHITE);
			INT_Sorted(pt);
			pt.Show(false);
			m_INT_RulerPts.Insert(pt);
		}
		for (int l = 0; l + 1 < n; l++)
		{
			vector a = m_INT_RulerWorld.Get(l);
			vector b = m_INT_RulerWorld.Get(l + 1);
			float lm = INT_VPP_Fmt.INT_Dist2D(a, b);
			string lt = INT_VPP_Fmt.INT_Dist(lm);
			float tw = INT_VPPX_Look.Measure(lt, 13) + 16;
			Widget plate = INT_NewTree(m_INT_LRuler, tw, 22, "IntVppRulLeg");
			if (!plate)
				continue;
			INT_VPPX_Look.Fill(plate, 0, 0, tw, 22, INT_PLATE);
			INT_VPPX_Look.Outline(plate, 0, 0, tw, 22, 0x99F3EFE8);
			INT_VPPX_Look.Text(plate, 0, 0, tw, 22, lt, 13, INT_VPPX_Look.WHITE, INT_UIF_Align.CENTER, false);
			INT_Sorted(plate);
			plate.Show(false);
			m_INT_RulerLegs.Insert(plate);
		}
		m_INT_DirtyProject = true;
	}

	// ------------------------------------------------------------------ projection (view / data change only)
	protected void INT_ShowAt(Widget w, float sx, float sy, float halfW, float halfH, bool on)
	{
		if (!w)
			return;
		if (on)
			w.SetPos(sx - halfW, sy - halfH);
		if (w.IsVisible() != on)
			w.Show(on);
	}

	// Dense markers: a marker whose label (name + distance) would overlap a label already shown in this projection,
	// or the icon of another marker (icons always stay), keeps its icon and loses the label. Order: the selected
	// marker, then server / event markers, then own ones. Runs only on projection (view or data change), never per
	// frame; Show() only when a label's state changes.
	protected void INT_Declutter()
	{
		m_INT_LabelRects.Clear();
		m_INT_IconXY.Clear();
		float ih = INT_UIF_Draw.S(14);
		foreach (INT_VPP_MapItem ic : m_INT_Items)
		{
			if (!ic || !ic.m_INT_Root || !ic.m_INT_OnScreen)
				continue;
			if (m_INT_IconXY.Count() >= INT_MAX_LABELS * 2)
				break;
			m_INT_IconXY.Insert(ic.m_INT_SX);
			m_INT_IconXY.Insert(ic.m_INT_SY);
		}
		int icn = m_INT_IconXY.Count();
		float gx = INT_UIF_Draw.S(19);
		float gTop = INT_UIF_Draw.S(14);
		float gBottom = INT_UIF_Draw.S(21);
		if (m_INT_F < 0.8)
		{
			gTop = INT_UIF_Draw.S(17);
			gBottom = INT_UIF_Draw.S(23);
		}
		float pad = INT_UIF_Draw.S(4);
		for (int ord = 0; ord < 3; ord++)
		{
			foreach (INT_VPP_MapItem it : m_INT_Items)
			{
				if (!it || !it.m_INT_Root || !it.m_INT_OnScreen)
					continue;
				int want = 2;
				if (it == m_INT_Sel)
					want = 0;
				else if (it.m_INT_Kind == INT_VPP_MapItem.KIND_SERVER || it.m_INT_Kind == INT_VPP_MapItem.KIND_EVENT)
					want = 1;
				if (want != ord)
					continue;
				float x1 = it.m_INT_SX + gx;
				float y1 = it.m_INT_SY - gTop;
				float lwPx = INT_UIF_Draw.S(it.m_INT_LabelW);
				float x2 = x1 + lwPx;
				float y2 = it.m_INT_SY + gBottom;
				bool fits = true;
				int rn = m_INT_LabelRects.Count();
				if (rn >= INT_MAX_LABELS * 4)
					fits = false;
				for (int k = 0; k < rn && fits; k = k + 4)
				{
					float ox1 = m_INT_LabelRects.Get(k);
					float oy1 = m_INT_LabelRects.Get(k + 1);
					float ox2 = m_INT_LabelRects.Get(k + 2);
					float oy2 = m_INT_LabelRects.Get(k + 3);
					if (x1 < ox2 + pad && x2 + pad > ox1 && y1 < oy2 && y2 > oy1)
						fits = false;
				}
				// another marker's icon under the label (its own icon sits left of x1)
				for (int q = 0; q < icn && fits; q = q + 2)
				{
					float cx = m_INT_IconXY.Get(q);
					float cy = m_INT_IconXY.Get(q + 1);
					if (cx == it.m_INT_SX && cy == it.m_INT_SY)
						continue;
					if (x1 < cx + ih && x2 > cx - ih && y1 < cy + ih && y2 > cy - ih)
						fits = false;
				}
				if (fits)
				{
					m_INT_LabelRects.Insert(x1);
					m_INT_LabelRects.Insert(y1);
					m_INT_LabelRects.Insert(x2);
					m_INT_LabelRects.Insert(y2);
				}
				INT_SetLabel(it, fits);
			}
		}
	}

	protected void INT_SetLabel(INT_VPP_MapItem it, bool on)
	{
		if (it.m_INT_LabelOn == on)
			return;
		it.m_INT_LabelOn = on;
		if (it.m_INT_NameW)
			it.m_INT_NameW.Show(on);
		if (it.m_INT_DistW)
			it.m_INT_DistW.Show(on);
	}

	// +1 per projection (cards pinned to points re-place themselves when it changes)
	int INT_ProjectCount()
	{
		return m_INT_ProjectN;
	}

	protected void INT_Project()
	{
		m_INT_DirtyProject = false;
		s_INT_Projects++;
		m_INT_ProjectN++;
		m_INT_F = INT_UIF_Scale.Factor();
		float half = INT_UIF_Draw.S(INT_BOX * 0.5);
		float sx;
		float sy;
		bool on;
		// markers: wanted ones are projected; a tree far outside the map goes, an on-screen marker without a tree
		// is built by INT_BuildSome (a few per frame)
		float drop = INT_DROP_PAD * m_INT_F;
		float minX = -drop;
		float minY = -drop;
		float maxX = m_INT_MapW + drop;
		float maxY = m_INT_MapH + drop;
		foreach (INT_VPP_MapItem it : m_INT_Items)
		{
			if (!it)
				continue;
			if (!it.m_INT_Want)
			{
				it.m_INT_OnScreen = false;
				continue;
			}
			on = INT_ToLocal(it.m_INT_Pos, sx, sy);
			it.m_INT_SX = sx;
			it.m_INT_SY = sy;
			it.m_INT_OnScreen = on;
			if (!it.m_INT_Root)
			{
				if (on)
					m_INT_BuildPending = true;
				continue;
			}
			if (!on)
			{
				if (sx < minX || sy < minY || sx > maxX || sy > maxY)
				{
					INT_DropItemTree(it);
					continue;
				}
			}
			INT_ShowAt(it.m_INT_Root, sx, sy, half, half, on);
		}
		m_INT_DirtyDeclutter = true;
		// zone labels (a label wider than its zone on screen hides: zoomed out it would cover the map, QA 2026-11-12)
		array<ref INT_VPP_Zone> zones = INT_Zones();
		int zn = m_INT_ZoneTrees.Count();
		float zmpp = INT_MetresPerPx();
		for (int zi = 0; zi < zn; zi++)
		{
			Widget zt = m_INT_ZoneTrees.Get(zi);
			INT_VPP_Zone z = null;
			if (zones)
			{
				int zid = m_INT_ZoneTreeIds.Get(zi);
				int zx = INT_VPP_ZoneIds.IndexOf(zones, zid);
				if (zx >= 0)
					z = zones.Get(zx);
			}
			if (!z || !zt)
			{
				INT_ShowAt(zt, 0, 0, 0, 0, false);
				continue;
			}
			float zw;
			float zh;
			zt.GetScreenSize(zw, zh);
			vector anchor = Vector(z.cx, 0, z.cz);
			bool zon = INT_ToLocal(anchor, sx, sy);
			float zy = sy - zh * 0.5;
			float extentM = 0;
			if (z.IsCircle())
			{
				float rpx = z.r / zmpp;
				zy = sy + rpx + 6 * m_INT_F;
				extentM = z.r * 2;
			}
			else
			{
				extentM = INT_ZoneExtent(z);
			}
			float extentPx = extentM / zmpp;
			if (extentPx < zw * 0.8)
				zon = false;
			INT_ShowAt(zt, sx, zy, zw * 0.5, 0, zon);
		}
		// admin text labels: centred on their point at a fixed screen size; far-off trees go, missing ones are built
		INT_ProjectLabels();
		// route points + legs
		INT_ProjectRoute();
		// ruler
		int rn = m_INT_RulerPts.Count();
		float rh = INT_UIF_Draw.S(8);
		for (int ri = 0; ri < rn && ri < m_INT_RulerWorld.Count(); ri++)
		{
			on = INT_ToLocal(m_INT_RulerWorld.Get(ri), sx, sy);
			INT_ShowAt(m_INT_RulerPts.Get(ri), sx, sy, rh, rh, on);
		}
		int ln = m_INT_RulerLegs.Count();
		for (int li = 0; li < ln; li++)
		{
			if (li + 1 >= m_INT_RulerWorld.Count())
				break;
			Widget rplate = m_INT_RulerLegs.Get(li);
			vector ra = m_INT_RulerWorld.Get(li);
			vector rb = m_INT_RulerWorld.Get(li + 1);
			INT_PlaceLegPlate(rplate, ra, rb);
		}
		// me
		if (m_INT_MeRoot)
		{
			bool meOn = false;
			if (m_INT_MeShown)
				meOn = INT_ToLocal(m_INT_MePos, sx, sy);
			float mh = INT_UIF_Draw.S(24);
			INT_ShowAt(m_INT_MeRoot, sx, sy, mh, mh, meOn);
		}
		// ghost
		if (m_INT_Ghost)
		{
			bool gon = INT_ToLocal(m_INT_GhostPos, sx, sy);
			float gh = INT_UIF_Draw.S(24);
			INT_ShowAt(m_INT_Ghost, sx, sy, gh, gh, gon);
		}
		INT_PlaceBox(m_INT_HoverBox, m_INT_Hover);
		INT_PlaceBox(m_INT_SelBox, m_INT_Sel);
	}

	// the larger side of a polygon zone's bounding box (metres)
	protected float INT_ZoneExtent(INT_VPP_Zone z)
	{
		int n = z.VertexCount();
		if (n <= 0)
			return 0;
		float x0 = z.xs.Get(0);
		float x1 = x0;
		float z0 = z.zs.Get(0);
		float z1 = z0;
		for (int i = 1; i < n; i++)
		{
			float vx = z.xs.Get(i);
			float vz = z.zs.Get(i);
			if (vx < x0)
				x0 = vx;
			if (vx > x1)
				x1 = vx;
			if (vz < z0)
				z0 = vz;
			if (vz > z1)
				z1 = vz;
		}
		float ex = x1 - x0;
		float ez = z1 - z0;
		if (ez > ex)
			return ez;
		return ex;
	}

	protected void INT_PlaceLegPlate(Widget plate, vector a, vector b)
	{
		if (!plate)
			return;
		float ax;
		float ay;
		float bx;
		float by;
		bool ina = INT_ToLocal(a, ax, ay);
		bool inb = INT_ToLocal(b, bx, by);
		float pw;
		float ph;
		plate.GetScreenSize(pw, ph);
		float mx = (ax + bx) * 0.5;
		float my = (ay + by) * 0.5;
		bool on = false;
		if (ina || inb)
			on = true;
		float dx = bx - ax;
		float dy = by - ay;
		// a plate wider than its leg on screen hides (zoomed out)
		if (Math.Sqrt(dx * dx + dy * dy) < pw + 20 * m_INT_F)
			on = false;
		INT_ShowAt(plate, mx, my, pw * 0.5, ph * 0.5, on);
	}

	protected void INT_ProjectRoute()
	{
		INT_VPP_Route route = INT_VPP_Route.INT_Get();
		if (!route)
			return;
		array<vector> chain = m_INT_ScrChain;
		INT_RouteChain(chain);
		int firstPt = 0;
		if (m_INT_MeShown && chain.Count() > 0)
			firstPt = 1;
		float sx;
		float sy;
		float ph = INT_UIF_Draw.S(17);
		int pn = m_INT_RoutePts.Count();
		int rn = route.INT_Count();
		for (int i = 0; i < pn; i++)
		{
			if (i >= rn)
				break;
			INT_VPP_RoutePoint rp = route.INT_At(i);
			if (!rp)
				continue;
			bool on = INT_ToLocal(rp.m_INT_Pos, sx, sy);
			INT_ShowAt(m_INT_RoutePts.Get(i), sx, sy, ph, ph, on);
			if (i == pn - 1 && m_INT_RouteFinish)
			{
				float fw;
				float fh;
				m_INT_RouteFinish.GetScreenSize(fw, fh);
				INT_ShowAt(m_INT_RouteFinish, sx - INT_UIF_Draw.S(22) - fw, sy, 0, fh * 0.5, on);
			}
		}
		int ln = m_INT_RouteLegs.Count();
		for (int l = 0; l < ln; l++)
		{
			if (l + 1 >= chain.Count())
				break;
			vector la = chain.Get(l);
			vector lb = chain.Get(l + 1);
			// the first leg (from me) changes with every step: its text follows
			if (l == 0 && firstPt == 1 && l < m_INT_RouteLegTexts.Count())
			{
				TextWidget t0 = m_INT_RouteLegTexts.Get(0);
				float lm = INT_VPP_Fmt.INT_Dist2D(la, lb);
				string lt = INT_VPP_Fmt.INT_Dist(lm);
				if (t0)
					t0.SetText(lt);
			}
			Widget lplate = m_INT_RouteLegs.Get(l);
			INT_PlaceLegPlate(lplate, la, lb);
		}
	}

	// ------------------------------------------------------------------ canvas
	protected bool INT_Line(CanvasWidget c, float x1, float y1, float x2, float y2, float w, int color, bool live)
	{
		if (!c)
			return false;
		int used = m_INT_Lines + m_INT_LiveLines;
		if (used >= INT_MAX_LINES)
			return false;
		// both ends past the same edge: invisible, not drawn, not counted
		if (x1 < 0 && x2 < 0)
			return true;
		if (y1 < 0 && y2 < 0)
			return true;
		if (x1 > m_INT_MapW && x2 > m_INT_MapW)
			return true;
		if (y1 > m_INT_MapH && y2 > m_INT_MapH)
			return true;
		float ww = w * m_INT_F;
		if (ww < 1)
			ww = 1;
		c.DrawLine(x1, y1, x2, y2, ww, color);
		if (live)
			m_INT_LiveLines++;
		else
			m_INT_Lines++;
		return true;
	}

	// maxSeg: the zone layer's per-zone budget (review #12); never below 8 segments
	protected bool INT_Circle(CanvasWidget c, float cx, float cy, float rpx, float w, int color, bool dashed, bool live, int maxSeg = 64)
	{
		int n = rpx / 4;
		if (n < 16)
			n = 16;
		if (n > 64)
			n = 64;
		if (n > maxSeg)
			n = maxSeg;
		if (n < 8)
			n = 8;
		float px = cx + rpx;
		float py = cy;
		for (int i = 1; i <= n; i++)
		{
			float a = Math.PI2 * i / n;
			float nx = cx + Math.Cos(a) * rpx;
			float ny = cy + Math.Sin(a) * rpx;
			bool skip = false;
			if (dashed)
			{
				int odd = i - (i / 2) * 2;
				if (odd == 0)
					skip = true;
			}
			if (!skip)
			{
				if (!INT_Line(c, px, py, nx, ny, w, color, live))
					return false;
			}
			px = nx;
			py = ny;
		}
		return true;
	}

	protected void INT_RedrawLines()
	{
		m_INT_DirtyLines = false;
		if (!m_INT_Canvas)
			return;
		m_INT_Canvas.Clear();
		m_INT_Lines = 0;
		// what must always draw first gets its reserve: route + ruler (2 lines a leg)
		array<vector> chain = m_INT_ScrChain;
		chain.Clear();
		bool routeOn = INT_LayerOn(INT_VPP_LAYER_ROUTE);
		if (routeOn)
			INT_RouteChain(chain);
		int reserve = INT_LIVE_RESERVE + chain.Count() * 2 + m_INT_RulerWorld.Count() * 2;
		int zoneBudget = INT_MAX_LINES - reserve;
		if (INT_LayerOn(INT_VPP_LAYER_ZONES))
			INT_DrawZones(zoneBudget);
		if (routeOn)
			INT_DrawRoute(chain);
		if (m_INT_RulerOn)
			INT_DrawRuler();
	}

	// review #12: every zone on screen gets its outline within the 1800-line budget. Zones off screen cost nothing;
	// the zones on screen share the outline budget: a polygon with more vertices than its share is simplified
	// (every k-th vertex, no segment under 2 px, a tiny one becomes a small box), a circle gets fewer segments
	// (never under 8). No hatch (owner 2026-11-13): the WHOLE zone budget goes to the outlines. No zone is dropped
	// silently: m_INT_ZonesMissed counts the ones that still found no room (self-test: 64 x 32 vertices, 0 missed).
	protected void INT_DrawZones(int budget)
	{
		m_INT_ZonesDrawn = 0;
		m_INT_ZonesCulled = 0;
		m_INT_ZonesMissed = 0;
		m_INT_ZonesSimplified = 0;
		m_INT_ZoneLines = 0;
		m_INT_ZoneShare = 0;
		int linesBefore = m_INT_Lines;
		array<ref INT_VPP_Zone> zones = INT_Zones();
		if (!zones)
			return;
		float mpp = INT_MetresPerPx();
		int onScreen = 0;
		foreach (INT_VPP_Zone zc : zones)
		{
			if (!zc || zc.id == m_INT_SkipZoneId)
				continue;
			if (INT_ZoneOnScreen(zc, mpp))
				onScreen++;
		}
		// the whole budget for the outlines, shared by the zones on screen
		int share = 64;
		if (onScreen > 0)
			share = budget / onScreen;
		if (share > 64)
			share = 64;
		if (share < 4)
			share = 4;
		m_INT_ZoneShare = share;
		foreach (INT_VPP_Zone z : zones)
		{
			if (!z || z.id == m_INT_SkipZoneId)
				continue;
			if (!INT_ZoneOnScreen(z, mpp))
			{
				m_INT_ZonesCulled++;
				continue;
			}
			if (m_INT_Lines + 4 > budget)
			{
				m_INT_ZonesMissed++;
				continue;
			}
			if (INT_DrawZoneOutline(z, mpp, share))
				m_INT_ZonesDrawn++;
			else
				m_INT_ZonesMissed++;
		}
		int zl = m_INT_Lines - linesBefore;
		m_INT_ZoneLines = zl;
	}

	// the zone's screen rectangle touches the map rect (world bounding box projected: the map is axis-aligned)
	protected bool INT_ZoneOnScreen(INT_VPP_Zone z, float mpp)
	{
		if (z.IsCircle())
		{
			vector c = Vector(z.cx, 0, z.cz);
			vector cs = m_INT_Map.MapToScreen(c);
			float cx = cs[0] - m_INT_MapX;
			float cy = cs[1] - m_INT_MapY;
			float rpx = z.r / mpp;
			if (cx + rpx < 0 || cy + rpx < 0 || cx - rpx > m_INT_MapW || cy - rpx > m_INT_MapH)
				return false;
			return true;
		}
		int n = z.VertexCount();
		if (n < 2)
			return false;
		float x0 = z.xs.Get(0);
		float x1 = x0;
		float z0 = z.zs.Get(0);
		float z1 = z0;
		for (int i = 1; i < n; i++)
		{
			float vx = z.xs.Get(i);
			float vz = z.zs.Get(i);
			if (vx < x0)
				x0 = vx;
			if (vx > x1)
				x1 = vx;
			if (vz < z0)
				z0 = vz;
			if (vz > z1)
				z1 = vz;
		}
		vector wa = Vector(x0, 0, z0);
		vector wb = Vector(x1, 0, z1);
		vector sa = m_INT_Map.MapToScreen(wa);
		vector sb = m_INT_Map.MapToScreen(wb);
		float ax = sa[0] - m_INT_MapX;
		float ay = sa[1] - m_INT_MapY;
		float bx = sb[0] - m_INT_MapX;
		float by = sb[1] - m_INT_MapY;
		float lx = ax;
		float hx = bx;
		if (bx < ax)
		{
			lx = bx;
			hx = ax;
		}
		float ly = ay;
		float hy = by;
		if (by < ay)
		{
			ly = by;
			hy = ay;
		}
		if (hx < 0 || hy < 0 || lx > m_INT_MapW || ly > m_INT_MapH)
			return false;
		return true;
	}

	// false = the line budget ran out while drawing it
	protected bool INT_DrawZoneOutline(INT_VPP_Zone z, float mpp, int share)
	{
		int base = INT_VPP_Validate.ForceAlpha(z.color);
		int edge = INT_VPP_Fmt.INT_WithAlpha(base, 0xF2);
		if (z.IsCircle())
		{
			vector c = Vector(z.cx, 0, z.cz);
			vector cs = m_INT_Map.MapToScreen(c);
			float cx = cs[0] - m_INT_MapX;
			float cy = cs[1] - m_INT_MapY;
			float rpx = z.r / mpp;
			if (share < 16)
				m_INT_ZonesSimplified++;
			if (!INT_Circle(m_INT_Canvas, cx, cy, rpx, 2, edge, false, false, share))
				return false;
			// the inner dashed ring only when the budget is generous
			float inner = rpx - 7 * m_INT_F;
			if (inner > 6 && share >= 48)
			{
				int ring = INT_VPP_Fmt.INT_WithAlpha(base, 0x66);
				INT_Circle(m_INT_Canvas, cx, cy, inner, 1, ring, true, false, share);
			}
			return true;
		}
		int n = z.VertexCount();
		if (n < 2)
			return true;
		int step = 1;
		if (n > share)
			step = (n + share - 1) / share;
		if (step > 1)
			m_INT_ZonesSimplified++;
		float x0 = z.xs.Get(0);
		float z0 = z.zs.Get(0);
		vector v0 = Vector(x0, 0, z0);
		vector s0 = m_INT_Map.MapToScreen(v0);
		float fx = s0[0] - m_INT_MapX;
		float fy = s0[1] - m_INT_MapY;
		float px = fx;
		float py = fy;
		int segs = 0;
		for (int i = step; i < n; i = i + step)
		{
			float xi = z.xs.Get(i);
			float zi = z.zs.Get(i);
			vector vi = Vector(xi, 0, zi);
			vector si = m_INT_Map.MapToScreen(vi);
			float nx = si[0] - m_INT_MapX;
			float ny = si[1] - m_INT_MapY;
			float dx = nx - px;
			float dy = ny - py;
			// a segment under 2 px reads as nothing at this zoom: merge it into the next one
			if (dx * dx + dy * dy < 4)
				continue;
			if (!INT_Line(m_INT_Canvas, px, py, nx, ny, 2, edge, false))
				return false;
			segs++;
			px = nx;
			py = ny;
		}
		if (segs == 0)
		{
			// the whole zone is smaller than a few px: a small box, so it never vanishes
			float h = 3;
			if (!INT_Line(m_INT_Canvas, fx - h, fy - h, fx + h, fy - h, 2, edge, false))
				return false;
			if (!INT_Line(m_INT_Canvas, fx + h, fy - h, fx + h, fy + h, 2, edge, false))
				return false;
			if (!INT_Line(m_INT_Canvas, fx + h, fy + h, fx - h, fy + h, 2, edge, false))
				return false;
			if (!INT_Line(m_INT_Canvas, fx - h, fy + h, fx - h, fy - h, 2, edge, false))
				return false;
			return true;
		}
		if (!INT_Line(m_INT_Canvas, px, py, fx, fy, 2, edge, false))
			return false;
		return true;
	}

	// TEST (review #12): outlines of the last redraw
	void INT_ZoneStats(out int drawn, out int culled, out int missed, out int simplified)
	{
		drawn = m_INT_ZonesDrawn;
		culled = m_INT_ZonesCulled;
		missed = m_INT_ZonesMissed;
		simplified = m_INT_ZonesSimplified;
	}

	// TEST (2026-11-13, no hatch): lines the zone pass drew in the last redraw, and the outline share per zone
	int INT_ZoneLines()
	{
		return m_INT_ZoneLines;
	}

	int INT_ZoneShareLast()
	{
		return m_INT_ZoneShare;
	}

	protected void INT_DrawRoute(array<vector> chain)
	{
		int cn = chain.Count();
		if (cn < 2)
			return;
		float px;
		float py;
		vector s0 = m_INT_Map.MapToScreen(chain.Get(0));
		px = s0[0] - m_INT_MapX;
		py = s0[1] - m_INT_MapY;
		for (int i = 1; i < cn; i++)
		{
			vector si = m_INT_Map.MapToScreen(chain.Get(i));
			float nx = si[0] - m_INT_MapX;
			float ny = si[1] - m_INT_MapY;
			INT_Line(m_INT_Canvas, px, py, nx, ny, 6, 0x8C0B0B0B, false);
			float w = 2;
			int col = 0xE6FF7A2F;
			if (i == 1)
			{
				w = 3;
				col = INT_VPPX_Look.ORANGE;
			}
			INT_Line(m_INT_Canvas, px, py, nx, ny, w, col, false);
			px = nx;
			py = ny;
		}
	}

	protected void INT_DrawRuler()
	{
		int n = m_INT_RulerWorld.Count();
		if (n < 2)
			return;
		vector s0 = m_INT_Map.MapToScreen(m_INT_RulerWorld.Get(0));
		float px = s0[0] - m_INT_MapX;
		float py = s0[1] - m_INT_MapY;
		for (int i = 1; i < n; i++)
		{
			vector si = m_INT_Map.MapToScreen(m_INT_RulerWorld.Get(i));
			float nx = si[0] - m_INT_MapX;
			float ny = si[1] - m_INT_MapY;
			INT_Line(m_INT_Canvas, px, py, nx, ny, 5, 0x990B0B0B, false);
			INT_Line(m_INT_Canvas, px, py, nx, ny, 2, INT_VPPX_Look.WHITE, false);
			px = nx;
			py = ny;
		}
	}

	// rubber bands (ruler / zone draft) + ghost rings + draft geometry: redrawn on cursor or view change
	protected void INT_RedrawLive()
	{
		m_INT_DirtyLive = false;
		if (!m_INT_Live)
			return;
		m_INT_Live.Clear();
		m_INT_LiveLines = 0;
		float sx;
		float sy;
		if (m_INT_GhostOn)
		{
			vector gs = m_INT_Map.MapToScreen(m_INT_GhostPos);
			float gx = gs[0] - m_INT_MapX;
			float gy = gs[1] - m_INT_MapY;
			int g1 = INT_VPP_Fmt.INT_WithAlpha(m_INT_GhostColor, 0xCC);
			int g2 = INT_VPP_Fmt.INT_WithAlpha(m_INT_GhostColor, 0x59);
			INT_Circle(m_INT_Live, gx, gy, 30 * m_INT_F, 1.5, g1, false, true);
			INT_Circle(m_INT_Live, gx, gy, 46 * m_INT_F, 1, g2, true, true);
		}
		if (m_INT_RulerOn && m_INT_CurIn)
		{
			int rc = m_INT_RulerWorld.Count();
			if (rc > 0)
			{
				vector rl = m_INT_Map.MapToScreen(m_INT_RulerWorld.Get(rc - 1));
				float rx = rl[0] - m_INT_MapX;
				float ry = rl[1] - m_INT_MapY;
				INT_Line(m_INT_Live, rx, ry, m_INT_CurX, m_INT_CurY, 1, 0xB3F3EFE8, true);
			}
		}
		if (m_INT_DraftOn)
			INT_DrawDraft();
	}

	protected void INT_DrawDraft()
	{
		int col = INT_VPP_Validate.ForceAlpha(m_INT_DraftColor);
		float box = 4 * m_INT_F;
		if (m_INT_DraftShape == INT_VPP_Net.SHAPE_CIRCLE)
		{
			if (m_INT_DraftStage <= 0)
				return;
			vector c = Vector(m_INT_DraftCX, 0, m_INT_DraftCZ);
			vector cs = m_INT_Map.MapToScreen(c);
			float cx = cs[0] - m_INT_MapX;
			float cy = cs[1] - m_INT_MapY;
			INT_Box(cx, cy, box, col);
			float rpx = 0;
			if (m_INT_DraftR > 0)
				rpx = m_INT_DraftR / INT_MetresPerPx();
			if (!m_INT_DraftClosed && m_INT_CurIn)
			{
				float ddx = m_INT_CurX - cx;
				float ddy = m_INT_CurY - cy;
				rpx = Math.Sqrt(ddx * ddx + ddy * ddy);
				INT_Line(m_INT_Live, cx, cy, m_INT_CurX, m_INT_CurY, 1, 0xB3F3EFE8, true);
			}
			if (rpx > 2)
				INT_Circle(m_INT_Live, cx, cy, rpx, 2, col, false, true);
			return;
		}
		int n = m_INT_DraftXs.Count();
		if (n <= 0)
			return;
		float fx = 0;
		float fy = 0;
		float px = 0;
		float py = 0;
		for (int i = 0; i < n; i++)
		{
			float dx0 = m_INT_DraftXs.Get(i);
			float dz0 = m_INT_DraftZs.Get(i);
			vector v = Vector(dx0, 0, dz0);
			vector s = m_INT_Map.MapToScreen(v);
			float x = s[0] - m_INT_MapX;
			float y = s[1] - m_INT_MapY;
			if (i == 0)
			{
				fx = x;
				fy = y;
				INT_Box(x, y, box * 1.6, INT_VPPX_Look.WHITE);
			}
			else
			{
				INT_Line(m_INT_Live, px, py, x, y, 2, col, true);
			}
			INT_Box(x, y, box, col);
			px = x;
			py = y;
		}
		if (m_INT_DraftClosed)
		{
			INT_Line(m_INT_Live, px, py, fx, fy, 2, col, true);
			return;
		}
		if (m_INT_CurIn)
			INT_Line(m_INT_Live, px, py, m_INT_CurX, m_INT_CurY, 1, 0xB3F3EFE8, true);
	}

	protected void INT_Box(float x, float y, float h, int col)
	{
		INT_Line(m_INT_Live, x - h, y - h, x + h, y - h, 2, col, true);
		INT_Line(m_INT_Live, x + h, y - h, x + h, y + h, 2, col, true);
		INT_Line(m_INT_Live, x + h, y + h, x - h, y + h, 2, col, true);
		INT_Line(m_INT_Live, x - h, y + h, x - h, y - h, 2, col, true);
	}

	// ------------------------------------------------------------------ admin text labels (2026-11-13)
	// The «Зоны» layer carries them (no layer cell of their own). Items follow INT_VPP_Client on a data / layer change
	// (INT_MarkLabels / INT_MarkLayers); a tree whose label kept text / colour / size is KEPT (moved by projection
	// only). Trees exist only for labels on / near the screen: built INT_LABEL_BUILD_BUDGET per frame, dropped when
	// they leave the screen by more than INT_DROP_PAD. Nothing runs per frame while nothing changes.
	protected bool INT_LabelsLayerOn()
	{
		if (!m_INT_LLabels)
			return false;
		bool on = INT_LayerOn(INT_VPP_LAYER_ZONES);
		return on;
	}

	protected void INT_DropLabelTree(INT_VPP_LabelItem it)
	{
		if (!it)
			return;
		if (it.m_INT_Root)
		{
			it.m_INT_Root.Unlink();
			INT_VPP_Stat.INT_Down(INT_VPP_Stat.INT_KIND_LABEL);
		}
		it.m_INT_Root = null;
		it.m_INT_TextW = null;
		it.m_INT_OnScreen = false;
	}

	protected void INT_ClearLabelTrees()
	{
		foreach (INT_VPP_LabelItem it : m_INT_LabelItems)
		{
			if (it)
				INT_DropLabelTree(it);
		}
	}

	protected void INT_RebuildLabelItems()
	{
		m_INT_DirtyLabels = false;
		if (!INT_LabelsLayerOn())
		{
			INT_ClearLabelTrees();
			m_INT_LabelItems.Clear();
			INT_DropLabelTree(m_INT_LabelDraft);
			m_INT_DirtyProject = true;
			return;
		}
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		array<ref INT_VPP_Label> labels = null;
		if (cl)
			labels = cl.INT_GetLabels();
		m_INT_OldLabels.Clear();
		foreach (INT_VPP_LabelItem was : m_INT_LabelItems)
		{
			if (was)
				m_INT_OldLabels.Insert(was);
		}
		m_INT_LabelItems.Clear();
		if (labels)
		{
			foreach (INT_VPP_Label l : labels)
			{
				if (!l || l.INT_Id == m_INT_SkipLabelId)
					continue;
				INT_VPP_LabelItem keep = null;
				int on = m_INT_OldLabels.Count();
				for (int k = 0; k < on; k++)
				{
					INT_VPP_LabelItem o = m_INT_OldLabels.Get(k);
					if (o && o.m_INT_Id == l.INT_Id)
					{
						if (o.INT_SameLook(l))
						{
							keep = o;
							s_INT_LabelTreesKept++;
						}
						else
						{
							INT_DropLabelTree(o);
						}
						m_INT_OldLabels.Set(k, null);
						break;
					}
				}
				if (!keep)
				{
					keep = new INT_VPP_LabelItem();
					keep.m_INT_Id = l.INT_Id;
					keep.m_INT_Text = l.INT_Text;
					keep.m_INT_Color = l.INT_Color;
					keep.m_INT_Size = l.INT_Size;
				}
				vector lp = l.INT_Pos();
				keep.m_INT_Pos = lp;
				m_INT_LabelItems.Insert(keep);
			}
		}
		// labels that are gone (or hidden while edited): their trees go
		foreach (INT_VPP_LabelItem gone : m_INT_OldLabels)
		{
			if (gone)
				INT_DropLabelTree(gone);
		}
		m_INT_OldLabels.Clear();
		// the editor's preview lost its tree while the layer was off: back with the layer
		if (m_INT_LabelDraft && !m_INT_LabelDraft.m_INT_Root && m_INT_LabelDraftOn && m_INT_LabelDraftHasPos)
			INT_BuildLabelTree(m_INT_LabelDraft);
		m_INT_DirtyProject = true;
	}

	// one label tree: a frame sized to the text, the text centred, dark outline (town-name look); >= 40 px from the
	// 72 px SDF source (crisp). The size is design px: the engine scales it with the screen height, never the zoom.
	protected void INT_BuildLabelTree(INT_VPP_LabelItem it)
	{
		if (!it || !m_INT_LLabels)
			return;
		float size = it.m_INT_Size;
		if (size <= 0)
			size = INT_VPP_Net.INT_LABEL_DEF_SIZE;
		string txt = it.m_INT_Text;
		if (txt == "")
			txt = " ";
		bool big = false;
		if (size >= 40)
			big = true;
		float tw = INT_VPPX_Look.Measure(txt, size);
		float w = tw + size + 24;
		float h = size * 1.5;
		Widget root = INT_NewTree(m_INT_LLabels, w, h, "IntVppLabel");
		if (!root)
			return;
		INT_VPP_Stat.INT_Up(INT_VPP_Stat.INT_KIND_LABEL);
		int col = it.m_INT_Color;
		TextWidget t = INT_UIF_Draw.Text(root, 0, 0, w, h, txt, size, col, INT_UIF_Align.CENTER, big);
		if (t)
		{
			int ol = 1;
			if (size >= 32)
				ol = 2;
			t.SetOutline(ol, INT_LABEL_OUTLINE);
		}
		it.m_INT_Root = root;
		it.m_INT_TextW = t;
		it.m_INT_W = w;
		it.m_INT_H = h;
		it.m_INT_TextDW = tw;
		INT_Sorted(root);
		root.Show(false);
		s_INT_LabelTreesBuilt++;
	}

	// screen position of an item; true = some part of it is on the map rect
	protected bool INT_PlaceLabel(INT_VPP_LabelItem it)
	{
		float sx;
		float sy;
		INT_ToLocal(it.m_INT_Pos, sx, sy);
		it.m_INT_SX = sx;
		it.m_INT_SY = sy;
		float w = it.m_INT_W;
		float h = it.m_INT_H;
		if (w <= 0)
		{
			float sz = it.m_INT_Size;
			float est = INT_VPPX_Look.Measure(it.m_INT_Text, sz);
			w = est + sz + 24;
			h = sz * 1.5;
		}
		float hw = INT_UIF_Draw.S(w * 0.5);
		float hh = INT_UIF_Draw.S(h * 0.5);
		bool on = true;
		if (sx + hw < 0 || sy + hh < 0 || sx - hw > m_INT_MapW || sy - hh > m_INT_MapH)
			on = false;
		it.m_INT_OnScreen = on;
		return on;
	}

	protected void INT_ProjectLabels()
	{
		float drop = INT_DROP_PAD * m_INT_F;
		foreach (INT_VPP_LabelItem it : m_INT_LabelItems)
		{
			if (!it)
				continue;
			bool on = INT_PlaceLabel(it);
			if (!it.m_INT_Root)
			{
				if (on)
					m_INT_LabelBuildPending = true;
				continue;
			}
			float hw = INT_UIF_Draw.S(it.m_INT_W * 0.5);
			float hh = INT_UIF_Draw.S(it.m_INT_H * 0.5);
			if (!on)
			{
				float sx = it.m_INT_SX;
				float sy = it.m_INT_SY;
				if (sx + hw < -drop || sy + hh < -drop || sx - hw > m_INT_MapW + drop || sy - hh > m_INT_MapH + drop)
				{
					INT_DropLabelTree(it);
					continue;
				}
			}
			INT_ShowAt(it.m_INT_Root, it.m_INT_SX, it.m_INT_SY, hw, hh, on);
		}
		// the editor's preview
		if (m_INT_LabelDraft)
		{
			bool don = false;
			if (m_INT_LabelDraftOn && m_INT_LabelDraftHasPos)
				don = INT_PlaceLabel(m_INT_LabelDraft);
			if (m_INT_LabelDraft.m_INT_Root)
			{
				float dhw = INT_UIF_Draw.S(m_INT_LabelDraft.m_INT_W * 0.5);
				float dhh = INT_UIF_Draw.S(m_INT_LabelDraft.m_INT_H * 0.5);
				INT_ShowAt(m_INT_LabelDraft.m_INT_Root, m_INT_LabelDraft.m_INT_SX, m_INT_LabelDraft.m_INT_SY, dhw, dhh, don);
			}
		}
	}

	// trees for labels on screen without one: at most INT_LABEL_BUILD_BUDGET per frame
	protected void INT_BuildSomeLabels()
	{
		m_INT_LabelBuildPending = false;
		if (!m_INT_LLabels)
			return;
		int budget = INT_LABEL_BUILD_BUDGET;
		foreach (INT_VPP_LabelItem it : m_INT_LabelItems)
		{
			if (!it || it.m_INT_Root || !it.m_INT_OnScreen)
				continue;
			if (budget <= 0)
			{
				m_INT_LabelBuildPending = true;
				break;
			}
			budget--;
			INT_BuildLabelTree(it);
			if (!it.m_INT_Root)
				continue;
			// the real size is known now: place it again
			bool on = INT_PlaceLabel(it);
			float hw = INT_UIF_Draw.S(it.m_INT_W * 0.5);
			float hh = INT_UIF_Draw.S(it.m_INT_H * 0.5);
			INT_ShowAt(it.m_INT_Root, it.m_INT_SX, it.m_INT_SY, hw, hh, on);
		}
	}

	// The label editor (2026-11-13): the edited label (skipId, -1 = none) is hidden from the layer and drawn as a
	// live preview with the editor's text / colour / size at its (new) point. on false = no preview, nothing hidden.
	void INT_SetLabelDraft(bool on, bool hasPos, vector pos, string text, int color, int size, int skipId)
	{
		int skip = skipId;
		if (!on)
			skip = -1;
		if (skip != m_INT_SkipLabelId)
		{
			m_INT_SkipLabelId = skip;
			m_INT_DirtyLabels = true;
		}
		m_INT_LabelDraftOn = on;
		m_INT_LabelDraftHasPos = hasPos;
		string t = INT_VPP_Validate.INT_LabelText(text);
		bool show = false;
		if (on && hasPos && t != "" && INT_LabelsLayerOn())
			show = true;
		if (!show)
		{
			INT_DropLabelTree(m_INT_LabelDraft);
			m_INT_LabelDraft = null;
			m_INT_DirtyProject = true;
			return;
		}
		bool same = false;
		if (m_INT_LabelDraft && m_INT_LabelDraft.m_INT_Root)
		{
			if (m_INT_LabelDraft.m_INT_Text == t && m_INT_LabelDraft.m_INT_Color == color && m_INT_LabelDraft.m_INT_Size == size)
				same = true;
		}
		if (!same)
		{
			INT_DropLabelTree(m_INT_LabelDraft);
			INT_VPP_LabelItem d = new INT_VPP_LabelItem();
			d.m_INT_Id = skipId;
			d.m_INT_Text = t;
			d.m_INT_Color = color;
			d.m_INT_Size = size;
			m_INT_LabelDraft = d;
			INT_BuildLabelTree(d);
		}
		m_INT_LabelDraft.m_INT_Pos = pos;
		m_INT_DirtyProject = true;
	}

	// The label under the cursor (absolute screen px): its id, 0 = none. Only labels with a tree on screen; the text
	// rect (estimated width x line height) plus a few px; the last drawn wins.
	int INT_LabelHitTest(float ax, float ay)
	{
		float lx = ax - m_INT_MapX;
		float ly = ay - m_INT_MapY;
		float pad = 4 * m_INT_F;
		int found = 0;
		foreach (INT_VPP_LabelItem it : m_INT_LabelItems)
		{
			if (!it || !it.m_INT_Root || !it.m_INT_OnScreen)
				continue;
			float hw = INT_UIF_Draw.S(it.m_INT_TextDW * 0.5) + pad;
			float hh = INT_UIF_Draw.S(it.m_INT_Size * 0.6) + pad;
			if (lx >= it.m_INT_SX - hw && lx <= it.m_INT_SX + hw && ly >= it.m_INT_SY - hh && ly <= it.m_INT_SY + hh)
				found = it.m_INT_Id;
		}
		return found;
	}

	// TEST hooks (2026-11-13)
	int INT_LabelItemCount()
	{
		return m_INT_LabelItems.Count();
	}

	int INT_LabelTreeCount()
	{
		int n = 0;
		foreach (INT_VPP_LabelItem it : m_INT_LabelItems)
		{
			if (it && it.m_INT_Root)
				n++;
		}
		return n;
	}

	Widget INT_LabelRootOf(int id)
	{
		foreach (INT_VPP_LabelItem it : m_INT_LabelItems)
		{
			if (it && it.m_INT_Id == id)
				return it.m_INT_Root;
		}
		return null;
	}

	TextWidget INT_LabelTextOf(int id)
	{
		foreach (INT_VPP_LabelItem it : m_INT_LabelItems)
		{
			if (it && it.m_INT_Id == id)
				return it.m_INT_TextW;
		}
		return null;
	}

	Widget INT_LabelDraftRoot()
	{
		if (!m_INT_LabelDraft)
			return null;
		return m_INT_LabelDraft.m_INT_Root;
	}

	// screen centre of a label (absolute px); false = no tree / off screen
	bool INT_LabelScreenPos(int id, out float ax, out float ay)
	{
		ax = 0;
		ay = 0;
		foreach (INT_VPP_LabelItem it : m_INT_LabelItems)
		{
			if (it && it.m_INT_Id == id && it.m_INT_Root && it.m_INT_OnScreen)
			{
				ax = it.m_INT_SX + m_INT_MapX;
				ay = it.m_INT_SY + m_INT_MapY;
				return true;
			}
		}
		return false;
	}

	// ------------------------------------------------------------------ counts for the layer chips / list
	int INT_CountLayer(int layer)
	{
		int n = 0;
		foreach (INT_VPP_MapItem it : m_INT_Items)
		{
			if (it && it.m_INT_Layer == layer)
				n++;
		}
		return n;
	}
}
