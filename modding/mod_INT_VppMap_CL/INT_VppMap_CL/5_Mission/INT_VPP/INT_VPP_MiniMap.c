// ============================================================================
// VanillaPPMap «Экспедиция» — the MINIMAP (owner 2026-11-13: concept A «ПЛАНШЕТКА», variant mm_a_2_alt.png;
// replaces the separate mod @MiniMap). HUD of the world screen, client only:
//   a square map (300 x 300 design px at 100 %) in an island frame (hairline + 2 px orange top) with a «С» chip at
//   the top; north always up, only the player arrow turns; grid numbers are drawn by the MapWidget itself.
//   (The data strip «КВАДРАТ · КУРС · МАСШТАБ» of concept A was removed by the owner on 2026-11-13: «лишнее».
//   Zoom feedback = the map changing its scale.)
// Content = our own data, the big map's rules: server / own / event markers (per-marker mode: × hidden here too,
// layer toggles apply), zones (outline + name), admin text labels (with «Зоны», reduced size, de-cluttered), the
// route (line + numbered points, the next one filled) and edge pins for off-window targets (next route point,
// events) with metres.
// Shown only when: the minimap is on (prefs), a player is alive and conscious, no menu is open (the chat input is
// fine, like the 3D markers and the route ribbon), the vanilla HUD is not hidden (hotkey HUD_HIDE / options
// HUD_DISABLE / the HUD panel hidden), and - when the server sets INT_MiniMapRequireGps 1 - a GPS receiver is ON
// (vanilla MapNavigationBehaviour; cfggameplay ignoreNavItemsOwnership lifts it, like the old MiniMapGPS and the
// vanilla map). Hidden = the whole tree unlinked (INT_VPP_Stat kinds 11 / 12 at 0).
// Cost (owner's standing requirement): ONE MapWidget. The overlay lives in a "world" frame that is MOVED as a whole
// by the player's displacement (one SetPos per frame while moving); items are re-projected only on a re-anchor
// (offset past INT_ANCHOR_T, zoom, data). Lines are drawn into a canvas inside that frame, clipped in script and
// budgeted (INT_MAX_LINES). Only the first route leg (me -> next point) is drawn every frame the player moved, two
// lines. Edge pins: 4 Hz. Per-frame cost is measured (INT_PerfAvgUs/MaxUs).
// Driven by the modded MissionGameplay at the end of this file; keys in Settings → Controls «DAYZAVR INT MODS»
// (data/modded_Inputs.xml): «\» show / hide, LCtrl + Num+ / LCtrl + Num- zoom.
// ============================================================================

// One marker of the minimap (server / own / event), rebuilt when the data changes.
class INT_VPP_MiniItem : Managed
{
	static const int KIND_SERVER = 0;
	static const int KIND_MINE = 1;
	static const int KIND_EVENT = 2;

	int m_INT_Kind;
	vector m_INT_Pos;
	string m_INT_Icon;
	int m_INT_Color;
	string m_INT_Name;
	// view (weak: unlinked by the minimap)
	Widget m_INT_Root;
	float m_INT_LX;
	float m_INT_LY;
	bool m_INT_Want;
}

class INT_VPP_MiniMap : Managed
{
	// design px at 100 %
	static const float INT_WIN = 300;
	static const float INT_MARGIN_SIDE = 48;
	static const float INT_MARGIN_TOP = 24;
	static const float INT_MARGIN_BOTTOM = 134;
	static const float INT_ANCHOR_T = 12;           // re-anchor when the overlay moved this far (design px)
	static const float INT_CANVAS_M = 40;           // world canvas margin around the window (design px)
	static const int INT_MAX_LINES = 600;           // world canvas budget (the big map caps at 1800)
	static const int INT_BUILD_BUDGET = 24;         // item trees built per frame
	static const int INT_MAX_PINS = 4;
	static const int INT_MAX_LABELS = 40;
	static const float INT_SLOW = 0.25;             // edge pins / data polls / HUD + GPS checks
	static const float INT_SETTINGS_WAIT = 20;      // s without INT_Settings (no SE) before showing without the GPS rule
	static const float INT_SCALE_GUESS = 0.1;
	static const float INT_OFFSCREEN = -6000;       // design px: a new tree waits here (visible, laid out) until calibrated
	static const int INT_CENTRE_GUARD = 8;          // frames after the move into place: the centre is re-checked

	static const int INT_COL_PANEL = 0xFF0D0C0C;
	static const int INT_COL_EDGE = 0x1FFFFFFF;
	static const int INT_COL_OUTLINE = 0xFF0B0B0B;
	static const int INT_COL_DIA_BACK = 0xE0141312;
	static const int INT_COL_AMBER = 0xFFFFC246;
	static const int INT_COL_MARKBACK = 0xC00B0B0B;
	static const int INT_COL_PIN_BACK = 0xFF121110;
	static const int INT_COL_ROUTE_UNDER = 0x800B0B0B;

	static const string INT_TXT_NORTH = "С";
	static const string INT_TXT_GPS_TOAST = "Миникарта видна, когда включён GPS-приёмник";

	protected static ref INT_VPP_MiniMap s_INT_Inst;

	// perf (TestKit): our update per frame, 100 ns ticks
	static int s_INT_PerfN;
	static int s_INT_PerfSum;
	static int s_INT_PerfMax;
	static int s_INT_Anchors;
	static int s_INT_Builds;
	static int s_INT_Calibrations;
	static int s_INT_CentreFixes;   // the centre had moved after the tree came into place (a visible jump)
	static int s_INT_LiveDraws;
	static bool s_INT_CalLogged;

	protected bool m_INT_Running;
	protected float m_INT_Since;
	protected bool m_INT_Built;
	protected int m_INT_Cal;
	protected int m_INT_CalTries;
	protected float m_INT_SlowAcc;

	// tree (weak refs; m_INT_Root owns everything)
	protected Widget m_INT_Root;
	protected MapWidget m_INT_Map;
	protected Widget m_INT_MapHost;
	protected Widget m_INT_Clip;
	protected Widget m_INT_World;
	protected CanvasWidget m_INT_Canvas;
	protected CanvasWidget m_INT_Live;
	protected Widget m_INT_Pins;
	protected Widget m_INT_MeRoot;
	protected ImageWidget m_INT_MeArrow;

	// geometry of the current build
	protected float m_INT_F;
	protected float m_INT_K;
	protected float m_INT_KT;
	protected float m_INT_WinD;       // window side, design px
	protected float m_INT_WinP;       // window side, physical px
	protected float m_INT_BoxW;
	protected float m_INT_BoxH;
	protected float m_INT_BoxX;
	protected float m_INT_BoxY;
	protected float m_INT_AnchorP;
	protected int m_INT_BuiltSize = -1;
	protected int m_INT_BuiltPlace = -1;
	protected float m_INT_BuiltX = -1;
	protected float m_INT_BuiltY = -1;
	protected int m_INT_BuiltScrW;
	protected int m_INT_BuiltScrH;

	// view
	protected int m_INT_Zoom = -1;
	protected float m_INT_Scale;
	protected float m_INT_KScale;     // ppm * scale (MapWidget scale is inversely proportional to px per metre)
	protected float m_INT_Ppm;        // measured physical px per metre
	protected float m_INT_TargetPpm;
	protected vector m_INT_SetPos;    // last SetMapPos
	protected vector m_INT_C;         // the map's real centre (clamped at the world edge)
	protected vector m_INT_A;         // anchor: centre when the overlay was projected
	protected float m_INT_OX;
	protected float m_INT_OY;
	protected bool m_INT_HaveSet;
	protected int m_INT_Guard;
	protected float m_INT_MeLX = -9999;
	protected float m_INT_MeLY = -9999;
	protected float m_INT_MeHead = -999;
	protected vector m_INT_LiveFrom;
	protected bool m_INT_LiveValid;

	// data
	protected ref array<ref INT_VPP_MiniItem> m_INT_Items;
	protected ref array<Widget> m_INT_ZoneTrees;
	protected ref array<Widget> m_INT_LabelTrees;
	protected ref array<Widget> m_INT_RouteTrees;
	// keys of the text trees (zone names / labels / route points): a re-anchor reuses a tree with the same key (only
	// SetPos), new keys are built, the rest dropped
	protected ref array<string> m_INT_ZoneKeys;
	protected ref array<string> m_INT_LabelKeys;
	protected ref array<string> m_INT_RouteKeys;
	protected ref array<Widget> m_INT_OldTrees;
	protected ref array<string> m_INT_OldKeys;
	// per-frame checks that may lag a little (HUD hidden, GPS rule): 4 Hz, the rest every frame
	protected float m_INT_CheckAcc;
	protected int m_INT_SlowWhy;
	protected ref array<Widget> m_INT_PinTrees;
	protected ref array<TextWidget> m_INT_PinTexts;
	protected ref array<string> m_INT_PinShown;
	protected ref array<int> m_INT_PinColors;
	protected bool m_INT_DirtyItems;
	protected bool m_INT_DirtyAnchor;
	protected bool m_INT_BuildPending;
	protected int m_INT_SeenSrvRev = -1;
	protected int m_INT_SeenSrvCount = -1;
	protected int m_INT_SeenZonesRev = -1;
	protected int m_INT_SeenLabelsRev = -1;
	protected int m_INT_SeenMarkSig = -1;
	protected int m_INT_SeenRouteSig = -1;
	protected int m_INT_SeenMiniRev = -1;
	protected bool m_INT_PrefsDirty;
	protected INT_VPP_Prefs m_INT_BoundPrefs;
	protected int m_INT_Lines;
	// scratch
	protected ref array<float> m_INT_PX;
	protected ref array<float> m_INT_PY;
	protected ref array<float> m_INT_QX;
	protected ref array<float> m_INT_QY;
	protected ref array<float> m_INT_Rects;
	// test / state
	protected int m_INT_HideReason;
	// TEST hooks: keep the minimap over menus (two MapWidgets at once), a synthetic position (walk / drive cost),
	// a probe line + text across the window edge (clipping)
	static bool s_INT_TestIgnoreScreen;
	protected bool m_INT_TestPosOn;
	protected vector m_INT_TestPos;
	protected Widget m_INT_ProbeText;

	static const int INT_HIDE_NONE = 0;
	static const int INT_HIDE_OFF = 1;
	static const int INT_HIDE_SCREEN = 2;
	static const int INT_HIDE_HUD = 3;
	static const int INT_HIDE_GPS = 4;
	static const int INT_HIDE_WAIT = 5;

	void INT_VPP_MiniMap()
	{
		m_INT_Items = new array<ref INT_VPP_MiniItem>();
		m_INT_ZoneTrees = new array<Widget>();
		m_INT_LabelTrees = new array<Widget>();
		m_INT_RouteTrees = new array<Widget>();
		m_INT_ZoneKeys = new array<string>();
		m_INT_LabelKeys = new array<string>();
		m_INT_RouteKeys = new array<string>();
		m_INT_OldTrees = new array<Widget>();
		m_INT_OldKeys = new array<string>();
		m_INT_CheckAcc = 1;
		m_INT_PinTrees = new array<Widget>();
		m_INT_PinTexts = new array<TextWidget>();
		m_INT_PinShown = new array<string>();
		m_INT_PinColors = new array<int>();
		m_INT_PX = new array<float>();
		m_INT_PY = new array<float>();
		m_INT_QX = new array<float>();
		m_INT_QY = new array<float>();
		m_INT_Rects = new array<float>();
	}

	void ~INT_VPP_MiniMap()
	{
		INT_End();
	}

	// ------------------------------------------------------------------ singleton
	static INT_VPP_MiniMap INT_Get()
	{
		if (!s_INT_Inst)
			s_INT_Inst = new INT_VPP_MiniMap();
		return s_INT_Inst;
	}

	static INT_VPP_MiniMap INT_Peek()
	{
		return s_INT_Inst;
	}

	static void INT_Start()
	{
		INT_VPP_MiniMap.INT_Shutdown();
		INT_VPP_MiniMap m = INT_VPP_MiniMap.INT_Get();
		m.INT_Begin();
	}

	static void INT_Shutdown()
	{
		INT_VPP_MiniMap m = s_INT_Inst;
		if (m)
			m.INT_End();
		s_INT_Inst = null;
	}

	static void INT_PerfReset()
	{
		s_INT_PerfN = 0;
		s_INT_PerfSum = 0;
		s_INT_PerfMax = 0;
		s_INT_Anchors = 0;
		s_INT_LiveDraws = 0;
	}

	static float INT_PerfAvgUs()
	{
		if (s_INT_PerfN <= 0)
			return 0;
		float sum = s_INT_PerfSum;
		float n = s_INT_PerfN;
		return sum / n / 10.0;
	}

	static float INT_PerfMaxUs()
	{
		float mx = s_INT_PerfMax;
		return mx / 10.0;
	}

	protected void INT_Begin()
	{
		m_INT_Running = true;
		m_INT_Since = 0;
		INT_BindPrefs();
	}

	protected void INT_End()
	{
		INT_Drop();
		INT_UnbindPrefs();
		m_INT_Running = false;
	}

	protected void INT_BindPrefs()
	{
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Peek();
		if (pr == m_INT_BoundPrefs)
			return;
		INT_UnbindPrefs();
		if (!pr)
			return;
		if (pr.m_INT_OnChanged)
			pr.m_INT_OnChanged.Insert(this.INT_OnPrefs);
		m_INT_BoundPrefs = pr;
		m_INT_PrefsDirty = true;
	}

	protected void INT_UnbindPrefs()
	{
		INT_VPP_Prefs old = m_INT_BoundPrefs;
		if (old && old.m_INT_OnChanged)
			old.m_INT_OnChanged.Remove(this.INT_OnPrefs);
		m_INT_BoundPrefs = null;
	}

	void INT_OnPrefs()
	{
		m_INT_PrefsDirty = true;
	}

	// ------------------------------------------------------------------ state (TEST + panel)
	bool INT_IsShown()
	{
		if (m_INT_Built && m_INT_Cal >= 2 && m_INT_Root)
			return true;
		return false;
	}

	bool INT_IsBuilt()
	{
		if (m_INT_Built && m_INT_Root)
			return true;
		return false;
	}

	int INT_HideReason()
	{
		return m_INT_HideReason;
	}

	Widget INT_GetRoot()
	{
		return m_INT_Root;
	}

	MapWidget INT_GetMap()
	{
		return m_INT_Map;
	}

	CanvasWidget INT_GetCanvas()
	{
		return m_INT_Canvas;
	}

	Widget INT_GetWorld()
	{
		return m_INT_World;
	}

	float INT_GetPpm()
	{
		return m_INT_Ppm;
	}

	float INT_GetTargetPpm()
	{
		return m_INT_TargetPpm;
	}

	float INT_GetWinPx()
	{
		return m_INT_WinP;
	}

	int INT_LineCount()
	{
		return m_INT_Lines;
	}

	int INT_ItemCount()
	{
		return m_INT_Items.Count();
	}

	int INT_ItemTreeCount()
	{
		int n = 0;
		foreach (INT_VPP_MiniItem it : m_INT_Items)
		{
			if (it && it.m_INT_Root)
				n++;
		}
		return n;
	}

	int INT_ZoneNameCount()
	{
		return m_INT_ZoneTrees.Count();
	}

	int INT_LabelTreeCount()
	{
		return m_INT_LabelTrees.Count();
	}

	int INT_RouteTreeCount()
	{
		return m_INT_RouteTrees.Count();
	}

	int INT_PinCount()
	{
		return m_INT_PinTrees.Count();
	}

	// TEST: the label shown on pin i ("" = none)
	string INT_PinText(int i)
	{
		if (i < 0 || i >= m_INT_PinShown.Count())
			return "";
		return m_INT_PinShown.Get(i);
	}

	int INT_PinColor(int i)
	{
		if (i < 0 || i >= m_INT_PinColors.Count())
			return 0;
		return m_INT_PinColors.Get(i);
	}

	// TEST: is a marker at `pos` drawn now (a tree inside the window)?
	bool INT_HasItemAt(vector pos, float tolM)
	{
		foreach (INT_VPP_MiniItem it : m_INT_Items)
		{
			if (!it || !it.m_INT_Root)
				continue;
			float dx = it.m_INT_Pos[0] - pos[0];
			float dz = it.m_INT_Pos[2] - pos[2];
			if (dx * dx + dz * dz <= tolM * tolM)
				return true;
		}
		return false;
	}

	// TEST: design rect of the whole planchette
	void INT_GetBox(out float x, out float y, out float w, out float h)
	{
		x = m_INT_BoxX;
		y = m_INT_BoxY;
		w = m_INT_BoxW;
		h = m_INT_BoxH;
	}

	// ------------------------------------------------------------------ per frame
	void INT_Update(float dt)
	{
		if (!m_INT_Running)
			return;
		int t0 = TickCount(0);
		m_INT_Since = m_INT_Since + dt;
		INT_BindPrefs();
		m_INT_CheckAcc = m_INT_CheckAcc + dt;
		if (m_INT_CheckAcc >= INT_SLOW)
		{
			m_INT_CheckAcc = 0;
			m_INT_SlowWhy = INT_SlowWhy();
		}
		int why = INT_FastWhy();
		if (why == INT_HIDE_NONE)
			why = m_INT_SlowWhy;
		m_INT_HideReason = why;
		if (why != INT_HIDE_NONE)
		{
			if (m_INT_Built)
				INT_Drop();
			INT_PerfAdd(t0);
			return;
		}
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Peek();
		if (m_INT_Built && m_INT_PrefsDirty && pr)
		{
			// size / place / resolution changed: a new tree; zoom alone: a new scale
			if (INT_NeedRebuild(pr))
				INT_Drop();
		}
		if (!m_INT_Built)
			INT_Build();
		if (!m_INT_Built)
		{
			INT_PerfAdd(t0);
			return;
		}
		if (m_INT_PrefsDirty && pr)
		{
			m_INT_PrefsDirty = false;
			int z = pr.INT_MiniGetZoom();
			if (z != m_INT_Zoom)
				INT_SetZoom(z);
			m_INT_DirtyItems = true;
		}
		INT_Frame(dt);
		INT_PerfAdd(t0);
	}

	protected void INT_PerfAdd(int t0)
	{
		int d = TickCount(t0);
		if (!m_INT_Built)
			return;
		s_INT_PerfN++;
		s_INT_PerfSum = s_INT_PerfSum + d;
		if (d > s_INT_PerfMax)
			s_INT_PerfMax = d;
	}

	// why the minimap is not shown now (INT_HIDE_NONE = shown)
	int INT_WhyHidden()
	{
		int fast = INT_FastWhy();
		if (fast != INT_HIDE_NONE)
			return fast;
		int slow = INT_SlowWhy();
		return slow;
	}

	// every frame: off, or a menu / dead / unconscious / the big map (a menu must never show the minimap a frame)
	protected int INT_FastWhy()
	{
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Peek();
		if (!pr || !pr.INT_MiniIsOn())
			return INT_HIDE_OFF;
		int st = INT_VPP_3D.INT_ScreenState();
		if (st != INT_VPP_3D.INT_SCREEN_DRAW && !s_INT_TestIgnoreScreen)
			return INT_HIDE_SCREEN;
		// the chat input is the only menu the 3D markers draw over; the minimap too (keys stay off meanwhile)
		return INT_HIDE_NONE;
	}

	// 4 Hz: the vanilla HUD hidden, the GPS rule (a quarter of a second late is fine for both)
	protected int INT_SlowWhy()
	{
		if (!s_INT_TestIgnoreScreen && INT_VPP_MiniMap.INT_HudHidden())
			return INT_HIDE_HUD;
		int gps = INT_VPP_MiniMap.INT_GpsState(m_INT_Since);
		if (gps == 1)
			return INT_HIDE_WAIT;
		if (gps == 2)
			return INT_HIDE_GPS;
		return INT_HIDE_NONE;
	}

	// TEST: re-evaluate the 4 Hz part now (the kit does not want to wait for it)
	void INT_TestRecheck()
	{
		m_INT_CheckAcc = INT_SLOW;
	}

	// vanilla HUD hidden: the hotkey (HUD_HIDE), the options (HUD_DISABLE) or the whole HUD panel hidden
	static bool INT_HudHidden()
	{
		if (!GetGame() || !GetGame().GetMission())
			return true;
		Hud h = GetGame().GetMission().GetHud();
		IngameHud ih = IngameHud.Cast(h);
		if (!ih)
			return false;
		IngameHudVisibility vis = ih.GetHudVisibility();
		if (vis)
		{
			if (vis.IsContextFlagActive(EHudContextFlags.HUD_HIDE))
				return true;
			if (vis.IsContextFlagActive(EHudContextFlags.HUD_DISABLE))
				return true;
		}
		bool panel = ih.INT_VPP_HudPanelShown();
		if (!panel)
			return true;
		return false;
	}

	// 0 = no GPS needed or a GPS receiver is on; 1 = waiting for the server settings; 2 = needed and none is on
	static int INT_GpsState(float since)
	{
		bool require = INT_VPP_MiniMap.INT_ServerRequiresGps();
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		bool has = false;
		if (cl && cl.HasSettings())
			has = true;
		if (!has)
		{
			if (since < INT_SETTINGS_WAIT)
				return 1;
			return 0;
		}
		if (!require)
			return 0;
		if (CfgGameplayHandler.GetMapIgnoreNavItemsOwnership())
			return 0;
		if (INT_VPP_MiniMap.INT_PlayerHasGps())
			return 0;
		return 2;
	}

	// the server's INT_MiniMapRequireGps (false until the settings arrived)
	static bool INT_ServerRequiresGps()
	{
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl || !cl.HasSettings())
			return false;
		INT_VPP_ClientSettings s = cl.GetSettings();
		if (!s)
			return false;
		if (s.INT_MiniMapRequireGps == 1)
			return true;
		return false;
	}

	// a GPS receiver that is ON (with power) in the player's possession: vanilla MapNavigationBehaviour (GPSReceiver
	// registers itself on OnWorkStart / unregisters on OnWorkStop and when it leaves the player)
	static bool INT_PlayerHasGps()
	{
		if (!GetGame())
			return false;
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb)
			return false;
		MapNavigationBehaviour nb = pb.GetMapNavigationBehaviour();
		if (!nb)
			return false;
		int nt = nb.GetNavigationType();
		int gpsBit = EMapNavigationType.GPS;
		if ((nt & gpsBit) != 0)
			return true;
		return false;
	}

	protected bool INT_NeedRebuild(INT_VPP_Prefs pr)
	{
		int sz = pr.INT_MiniGetSizePct();
		int pl = pr.INT_MiniGetPlace();
		float fx = pr.INT_MiniGetX();
		float fy = pr.INT_MiniGetY();
		if (sz != m_INT_BuiltSize || pl != m_INT_BuiltPlace)
			return true;
		if (pl == INT_VPP_Prefs.INT_MINI_PLACE_OWN)
		{
			if (Math.AbsFloat(fx - m_INT_BuiltX) > 0.0005 || Math.AbsFloat(fy - m_INT_BuiltY) > 0.0005)
				return true;
		}
		return false;
	}

	// ------------------------------------------------------------------ geometry
	// design rect of the planchette for a size / place on a W x 1080 design screen (also the panel's schematic)
	static void INT_BoxFor(int sizePct, int place, float fx, float fy, float scrW, float scrH, out float x, out float y, out float w, out float h)
	{
		float k = sizePct;
		k = k / 100.0;
		float win = INT_WIN * k;
		w = win + 2;
		h = win + 3;
		float left = INT_MARGIN_SIDE;
		float right = scrW - INT_MARGIN_SIDE - w;
		float top = INT_MARGIN_TOP;
		float bottom = scrH - INT_MARGIN_BOTTOM - h;
		float midY = (scrH - h) * 0.5;
		if (right < left)
			right = left;
		if (bottom < top)
			bottom = top;
		x = right;
		y = top;
		if (place == INT_VPP_Prefs.INT_MINI_PLACE_TL)
		{
			x = left;
			y = top;
		}
		else if (place == INT_VPP_Prefs.INT_MINI_PLACE_ML)
		{
			x = left;
			y = midY;
		}
		else if (place == INT_VPP_Prefs.INT_MINI_PLACE_MR)
		{
			x = right;
			y = midY;
		}
		else if (place == INT_VPP_Prefs.INT_MINI_PLACE_BL)
		{
			x = left;
			y = bottom;
		}
		else if (place == INT_VPP_Prefs.INT_MINI_PLACE_BR)
		{
			x = right;
			y = bottom;
		}
		else if (place == INT_VPP_Prefs.INT_MINI_PLACE_OWN)
		{
			// own place: 0..1 of the free room between the screen edges (8 px inset), whatever the size / screen
			float minX = 8;
			float maxX = scrW - 8 - w;
			float minY = 8;
			float maxY = scrH - 8 - h;
			if (maxX < minX)
				maxX = minX;
			if (maxY < minY)
				maxY = minY;
			x = minX + (maxX - minX) * fx;
			y = minY + (maxY - minY) * fy;
		}
	}

	// the inverse for the panel's drag: 0..1 of the free room from a design top-left corner
	static void INT_FracFor(int sizePct, float x, float y, float scrW, float scrH, out float fx, out float fy)
	{
		float k = sizePct;
		k = k / 100.0;
		float win = INT_WIN * k;
		float w = win + 2;
		float h = win + 3;
		float minX = 8;
		float maxX = scrW - 8 - w;
		float minY = 8;
		float maxY = scrH - 8 - h;
		fx = 0;
		fy = 0;
		if (maxX > minX)
			fx = (x - minX) / (maxX - minX);
		if (maxY > minY)
			fy = (y - minY) / (maxY - minY);
		fx = INT_VPP_Prefs.INT_MiniClampFrac(fx, 1.0);
		fy = INT_VPP_Prefs.INT_MiniClampFrac(fy, 0.0);
	}

	// text scale (markers, names, pins, «С»): follows the size between 80 % and 120 % (60 % stays readable)
	static float INT_TextK(float k)
	{
		float kt = k;
		if (kt < 0.8)
			kt = 0.8;
		if (kt > 1.2)
			kt = 1.2;
		return kt;
	}

	// design screen: W x H with the UIF factor (H = 1080 on 16:9, taller on 4:3 / 5:4)
	static void INT_DesignScreen(out float w, out float h)
	{
		float f = INT_UIF_Scale.Factor();
		int sw = INT_UIF_Scale.ScreenW();
		int sh = INT_UIF_Scale.ScreenH();
		w = 1920;
		h = 1080;
		if (f <= 0 || sw <= 0 || sh <= 0)
			return;
		w = sw / f;
		h = sh / f;
	}

	// ------------------------------------------------------------------ build / drop
	protected void INT_Build()
	{
		if (!GetGame() || !GetGame().GetWorkspace())
			return;
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Peek();
		if (!pr)
			return;
		Widget root = GetGame().GetWorkspace().CreateWidgets("VanillaPPMap/GUI/Layouts/INT_VPP_MiniMap.layout");
		if (!root)
			return;
		m_INT_Root = root;
		m_INT_Built = true;
		s_INT_Builds++;
		INT_VPP_Stat.INT_Up(INT_VPP_Stat.INT_KIND_MINI);
		m_INT_Map = MapWidget.Cast(root.FindAnyWidget("INT_VPP_MiniMapW"));
		m_INT_MapHost = root.FindAnyWidget("INT_VPP_MiniMapHost");
		m_INT_Clip = root.FindAnyWidget("INT_VPP_MiniClip");
		m_INT_World = root.FindAnyWidget("INT_VPP_MiniWorld");
		m_INT_Canvas = CanvasWidget.Cast(root.FindAnyWidget("INT_VPP_MiniCanvas"));
		m_INT_Live = CanvasWidget.Cast(root.FindAnyWidget("INT_VPP_MiniLive"));
		m_INT_Pins = root.FindAnyWidget("INT_VPP_MiniPins");
		if (!m_INT_Map || !m_INT_MapHost || !m_INT_Clip || !m_INT_World || !m_INT_Canvas || !m_INT_Live || !m_INT_Pins)
		{
			Print("[INT_VPP] minimap layout incomplete");
			INT_Drop();
			return;
		}
		int size = pr.INT_MiniGetSizePct();
		int place = pr.INT_MiniGetPlace();
		float fx = pr.INT_MiniGetX();
		float fy = pr.INT_MiniGetY();
		m_INT_BuiltSize = size;
		m_INT_BuiltPlace = place;
		m_INT_BuiltX = fx;
		m_INT_BuiltY = fy;
		m_INT_BuiltScrW = INT_UIF_Scale.ScreenW();
		m_INT_BuiltScrH = INT_UIF_Scale.ScreenH();
		m_INT_F = INT_UIF_Scale.Factor();
		float k = size;
		k = k / 100.0;
		m_INT_K = k;
		m_INT_KT = INT_VPP_MiniMap.INT_TextK(k);
		m_INT_WinD = INT_WIN * k;
		m_INT_WinP = INT_UIF_Draw.S(m_INT_WinD);
		m_INT_AnchorP = INT_UIF_Draw.S(INT_ANCHOR_T);
		float scrW;
		float scrH;
		INT_VPP_MiniMap.INT_DesignScreen(scrW, scrH);
		float bx;
		float by;
		float bw;
		float bh;
		INT_VPP_MiniMap.INT_BoxFor(size, place, fx, fy, scrW, scrH, bx, by, bw, bh);
		m_INT_BoxX = bx;
		m_INT_BoxY = by;
		m_INT_BoxW = bw;
		m_INT_BoxH = bh;
		// a NEW MapWidget keeps a default size until its first layout, and SetMapPos stores the corner from the size
		// it has at that moment: a hidden tree is never laid out, so a map centred while hidden stood ~800 m off the
		// player once shown (measured 2026-11-13, the jump shrinks as the window grows). The tree stays VISIBLE but
		// off-screen while it calibrates, is centred again once laid out and only then moves into place (INT_CalDone)
		INT_UIF_Draw.Place(root, INT_OFFSCREEN, INT_OFFSCREEN, bw, bh);
		float win = m_INT_WinD;
		INT_UIF_Draw.Place(m_INT_MapHost, 1, 2, win, win);
		INT_UIF_Draw.Place(m_INT_Map, 0, 0, win, win);
		INT_UIF_Draw.Place(m_INT_Clip, 1, 2, win, win);
		INT_UIF_Draw.Place(m_INT_World, 0, 0, win, win);
		float cm = INT_CANVAS_M;
		INT_UIF_Draw.Place(m_INT_Canvas, -cm, -cm, win + cm * 2, win + cm * 2);
		INT_UIF_Draw.Place(m_INT_Live, 0, 0, win, win);
		INT_UIF_Draw.Place(m_INT_Pins, 0, 0, win, win);
		// no transparency setting (removed by the owner 2026-11-13): the MapWidget ignores every alpha path (measured:
		// SetAlpha, SetColor alpha, a parent frame's alpha with inheritalpha 1, SOURCEALPHA | BLEND flags, layout mode
		// blend + "src alpha" - the map stayed pixel-identical), so our layers always draw at full opacity
		INT_BuildFrame();
		INT_BuildMe();
		m_INT_Cal = 0;
		m_INT_Guard = 0;
		m_INT_CalTries = 0;
		m_INT_HaveSet = false;
		m_INT_Zoom = pr.INT_MiniGetZoom();
		m_INT_TargetPpm = INT_VPP_MiniMap.INT_PpmFor(m_INT_Zoom, m_INT_F);
		m_INT_DirtyItems = true;
		m_INT_DirtyAnchor = true;
		m_INT_LiveValid = false;
		m_INT_MeLX = -9999;
		m_INT_MeLY = -9999;
		m_INT_MeHead = -999;
		m_INT_SeenMarkSig = -1;
		m_INT_SeenRouteSig = -1;
		m_INT_SeenSrvRev = -1;
		m_INT_SeenSrvCount = -1;
		m_INT_SeenZonesRev = -1;
		m_INT_SeenLabelsRev = -1;
		m_INT_PrefsDirty = false;
		m_INT_SlowAcc = INT_SLOW;
		Man pl = GetGame().GetPlayer();
		if (pl)
		{
			vector p = pl.GetPosition();
			m_INT_Map.SetScale(INT_SCALE_GUESS);
			m_INT_Scale = INT_SCALE_GUESS;
			m_INT_Map.SetMapPos(p);
			m_INT_SetPos = p;
			m_INT_C = p;
			m_INT_A = p;
			m_INT_HaveSet = true;
		}
	}


	void INT_Drop()
	{
		if (m_INT_ProbeText)
			INT_DropTree(m_INT_ProbeText);
		m_INT_ProbeText = null;
		foreach (INT_VPP_MiniItem it : m_INT_Items)
		{
			if (it && it.m_INT_Root)
				INT_DropTree(it.m_INT_Root);
			if (it)
				it.m_INT_Root = null;
		}
		m_INT_Items.Clear();
		INT_DropList(m_INT_ZoneTrees);
		INT_DropList(m_INT_LabelTrees);
		INT_DropList(m_INT_RouteTrees);
		INT_DropList(m_INT_OldTrees);
		m_INT_ZoneKeys.Clear();
		m_INT_LabelKeys.Clear();
		m_INT_RouteKeys.Clear();
		m_INT_OldKeys.Clear();
		INT_DropList(m_INT_PinTrees);
		m_INT_PinTexts.Clear();
		m_INT_PinShown.Clear();
		m_INT_PinColors.Clear();
		if (m_INT_Built)
		{
			Widget root = m_INT_Root;
			if (root)
				root.Unlink();
			INT_VPP_Stat.INT_Down(INT_VPP_Stat.INT_KIND_MINI);
		}
		m_INT_Root = null;
		m_INT_Map = null;
		m_INT_MapHost = null;
		m_INT_Clip = null;
		m_INT_World = null;
		m_INT_Canvas = null;
		m_INT_Live = null;
		m_INT_Pins = null;
		m_INT_MeRoot = null;
		m_INT_MeArrow = null;
		m_INT_Built = false;
		m_INT_Cal = 0;
		m_INT_Lines = 0;
		m_INT_BuildPending = false;
	}

	// an overlay tree: unlinked here, counted down (its parent goes with the root anyway)
	protected void INT_DropTree(Widget w)
	{
		if (w)
			w.Unlink();
		INT_VPP_Stat.INT_Down(INT_VPP_Stat.INT_KIND_MINI_ITEM);
	}

	protected void INT_DropList(array<Widget> list)
	{
		int n = list.Count();
		for (int i = 0; i < n; i++)
		{
			Widget w = list.Get(i);
			INT_DropTree(w);
		}
		list.Clear();
	}

	protected Widget INT_NewTree(Widget parent, float w, float h, string nm)
	{
		Widget t = INT_UIF_Draw.Frame(parent, 0, 0, w, h, nm);
		if (t)
			INT_VPP_Stat.INT_Up(INT_VPP_Stat.INT_KIND_MINI_ITEM);
		return t;
	}

	// own sort 300 on every widget of a finished tree (MapWidget priority 100)
	protected void INT_Finish(Widget t)
	{
		if (!t)
			return;
		INT_VPPX_Look.SortTree(t, 300);
	}

	// ------------------------------------------------------------------ frame, «С»
	protected void INT_BuildFrame()
	{
		Widget root = m_INT_Root;
		float win = m_INT_WinD;
		float kt = m_INT_KT;
		float bw = m_INT_BoxW;
		float bh = m_INT_BoxH;
		// frame: hairline around everything + the 2 px orange top (island look)
		Widget edge = INT_UIF_Draw.Frame(root, 0, 0, bw, bh, "IntVppMiniEdge");
		if (edge)
		{
			INT_VPPX_Look.Outline(edge, 0, 0, bw, bh, INT_COL_EDGE);
			INT_VPPX_Look.Fill(edge, 0, 0, bw, 2, INT_VPPX_Look.ORANGE);
			INT_VPPX_Look.SortTree(edge, 300);
		}
		// «С» chip at the top centre (static: the map never turns)
		float nw = 30 * kt;
		float nh = 22 * kt;
		Widget north = INT_UIF_Draw.Frame(root, 1 + (win - nw) * 0.5, 2, nw, nh, "IntVppMiniNorth");
		if (north)
		{
			INT_VPPX_Look.Fill(north, 0, 0, nw, nh, INT_COL_PANEL);
			float ns = 13 * kt;
			INT_VPPX_Look.Text(north, 0, 0, nw, nh, INT_TXT_NORTH, ns, INT_VPPX_Look.ORANGE, INT_UIF_Align.CENTER, true);
			INT_VPPX_Look.SortTree(north, 300);
		}
	}

	// me: thin ring + heading arrow (square widget, turned in-plane); in the static pins layer
	protected void INT_BuildMe()
	{
		float kt = m_INT_KT;
		float rs = 30 * kt;
		float asz = 21 * kt;
		Widget me = INT_UIF_Draw.Frame(m_INT_Pins, 0, 0, rs, rs, "IntVppMiniMe");
		if (!me)
			return;
		INT_UIF_Draw.Texture(me, 0, 0, rs, rs, INT_VPP_Icons.INT_UI_ME_RING, INT_VPPX_Look.ORANGE);
		float ao = (rs - asz) * 0.5;
		m_INT_MeArrow = INT_UIF_Draw.Texture(me, ao, ao, asz, asz, INT_VPP_Icons.INT_UI_ME, INT_VPPX_Look.ORANGE);
		INT_VPPX_Look.SortTree(me, 300);
		m_INT_MeRoot = me;
	}

	// physical px per metre for a zoom step (the same metres per DESIGN px at every size: a bigger window shows more)
	static float INT_PpmFor(int zoom, float f)
	{
		float viewM = INT_VPP_Prefs.INT_MiniZoomMetres(zoom);
		float ppmD = INT_WIN / viewM;
		return ppmD * f;
	}

	protected void INT_SetZoom(int z)
	{
		m_INT_Zoom = z;
		m_INT_TargetPpm = INT_VPP_MiniMap.INT_PpmFor(z, m_INT_F);
		if (m_INT_KScale > 0 && m_INT_Map)
		{
			m_INT_Scale = m_INT_KScale / m_INT_TargetPpm;
			INT_ScaleAndCentre(m_INT_Scale);
			// re-measured next frame (stays visible), then everything is re-projected
			m_INT_Cal = 1;
			m_INT_CalTries = 0;
		}
	}

	// ------------------------------------------------------------------ calibration (MapWidget scale -> px per metre)
	protected float INT_MeasurePpm()
	{
		if (!m_INT_Map)
			return 0;
		vector c = m_INT_Map.GetMapPos();
		vector e = c;
		e[0] = c[0] + 100;
		vector a = m_INT_Map.MapToScreen(c);
		vector b = m_INT_Map.MapToScreen(e);
		float d = b[0] - a[0];
		return d / 100.0;
	}

	protected void INT_CalStep()
	{
		float ppm = INT_MeasurePpm();
		s_INT_Calibrations++;
		if (ppm <= 0.0001 || ppm != ppm)
		{
			// not laid out yet (first frame) - try again, then show anyway
			m_INT_CalTries++;
			if (m_INT_CalTries > 6)
			{
				Print("[INT_VPP] minimap: MapToScreen gave no scale, shown uncalibrated");
				m_INT_Ppm = m_INT_TargetPpm;
				INT_CalDone();
			}
			return;
		}
		m_INT_KScale = ppm * m_INT_Scale;
		float rel = Math.AbsFloat(ppm - m_INT_TargetPpm) / m_INT_TargetPpm;
		if (rel <= 0.02 || m_INT_CalTries >= 4)
		{
			m_INT_Ppm = ppm;
			INT_CalDone();
			return;
		}
		m_INT_CalTries++;
		m_INT_Scale = m_INT_KScale / m_INT_TargetPpm;
		INT_ScaleAndCentre(m_INT_Scale);
		m_INT_Cal = 1;
	}

	// SetScale does not keep the centre (measured 2026-11-13: after a rebuild the map stood ~700 m off the player
	// while our overlay was right): every scale change puts the centre back and reads the real one (world edge clamp)
	protected void INT_ScaleAndCentre(float scale)
	{
		if (!m_INT_Map)
			return;
		m_INT_Map.SetScale(scale);
		m_INT_Map.SetMapPos(m_INT_SetPos);
		vector real = m_INT_Map.GetMapPos();
		m_INT_C = real;
	}

	protected void INT_CalDone()
	{
		// the centre once more (the layout of a new widget settles in its first frames)
		if (m_INT_Map)
		{
			m_INT_Map.SetMapPos(m_INT_SetPos);
			vector real = m_INT_Map.GetMapPos();
			m_INT_C = real;
		}
		if (!s_INT_CalLogged)
		{
			s_INT_CalLogged = true;
			string det = "scale=" + m_INT_Scale.ToString() + " ppm=" + m_INT_Ppm.ToString() + " target=" + m_INT_TargetPpm.ToString();
			det = det + " win=" + m_INT_WinP.ToString() + " tries=" + m_INT_CalTries.ToString();
			Print("[INT_VPP] minimap calibrated: " + det);
		}
		m_INT_Cal = 2;
		m_INT_DirtyAnchor = true;
		m_INT_LiveValid = false;
		// into place (it was laid out off-screen, so the centre above is the real one)
		if (m_INT_Root)
			INT_UIF_Draw.Move(m_INT_Root, m_INT_BoxX, m_INT_BoxY);
		m_INT_Guard = INT_CENTRE_GUARD;
	}

	// ------------------------------------------------------------------ per frame (built)
	protected void INT_Frame(float dt)
	{
		Man player = GetGame().GetPlayer();
		if (!player || !m_INT_Map)
			return;
		vector p = player.GetPosition();
		if (m_INT_TestPosOn)
			p = m_INT_TestPos;
		// centre on the PLAYER (smooth: every frame the view moved by a quarter of a pixel or more)
		bool moved = false;
		float eps = 0.25;
		if (m_INT_Ppm > 0)
			eps = 0.25 / m_INT_Ppm;
		if (!m_INT_HaveSet || Math.AbsFloat(p[0] - m_INT_SetPos[0]) > eps || Math.AbsFloat(p[2] - m_INT_SetPos[2]) > eps)
		{
			m_INT_Map.SetMapPos(p);
			m_INT_SetPos = p;
			m_INT_HaveSet = true;
			vector real = m_INT_Map.GetMapPos();
			m_INT_C = real;
			moved = true;
		}
		else if (m_INT_Guard > 0)
		{
			// the first frames in place: the map must still stand where we put it
			vector now = m_INT_Map.GetMapPos();
			float jump = INT_VPP_Fmt.INT_Dist2D(now, m_INT_C);
			if (jump > 0.5)
			{
				s_INT_CentreFixes++;
				Print("[INT_VPP] minimap centre moved " + jump.ToString() + " m after the move into place, re-centred");
				m_INT_Map.SetMapPos(m_INT_SetPos);
				vector again = m_INT_Map.GetMapPos();
				m_INT_C = again;
				m_INT_DirtyAnchor = true;
			}
		}
		if (m_INT_Guard > 0 && m_INT_Cal >= 2)
			m_INT_Guard--;
		if (m_INT_Cal < 2)
		{
			if (m_INT_Cal == 0)
				m_INT_Cal = 1;
			else
				INT_CalStep();
			if (m_INT_Cal < 2)
				return;
		}
		INT_PollData(dt);
		if (m_INT_DirtyItems)
		{
			m_INT_DirtyItems = false;
			INT_CollectItems();
			m_INT_DirtyAnchor = true;
		}
		float ppm = m_INT_Ppm;
		float ox = -(m_INT_C[0] - m_INT_A[0]) * ppm;
		float oy = (m_INT_C[2] - m_INT_A[2]) * ppm;
		float tp = m_INT_AnchorP;
		if (Math.AbsFloat(ox) > tp || Math.AbsFloat(oy) > tp)
			m_INT_DirtyAnchor = true;
		if (m_INT_DirtyAnchor)
		{
			m_INT_DirtyAnchor = false;
			INT_Anchor();
			ox = 0;
			oy = 0;
		}
		if (Math.AbsFloat(ox - m_INT_OX) > 0.01 || Math.AbsFloat(oy - m_INT_OY) > 0.01)
		{
			m_INT_OX = ox;
			m_INT_OY = oy;
			if (m_INT_World)
				m_INT_World.SetPos(ox, oy);
		}
		if (m_INT_BuildPending)
			INT_BuildSome();
		INT_UpdateMe(player, p);
		INT_DrawLiveLeg(p);
		m_INT_SlowAcc = m_INT_SlowAcc + dt;
		if (m_INT_SlowAcc >= INT_SLOW)
		{
			m_INT_SlowAcc = 0;
			INT_UpdatePins(p);
		}
	}

	// local physical px of a world point relative to the CURRENT centre (static layers: me, pins, live leg)
	protected void INT_Local(vector w, out float lx, out float ly)
	{
		float half = m_INT_WinP * 0.5;
		lx = half + (w[0] - m_INT_C[0]) * m_INT_Ppm;
		ly = half - (w[2] - m_INT_C[2]) * m_INT_Ppm;
	}

	// local physical px of a world point inside the world frame (relative to the anchor)
	protected void INT_WorldLocal(vector w, out float lx, out float ly)
	{
		float half = m_INT_WinP * 0.5;
		lx = half + (w[0] - m_INT_A[0]) * m_INT_Ppm;
		ly = half - (w[2] - m_INT_A[2]) * m_INT_Ppm;
	}

	protected void INT_UpdateMe(Man player, vector p)
	{
		if (!m_INT_MeRoot)
			return;
		float lx;
		float ly;
		INT_Local(p, lx, ly);
		if (Math.AbsFloat(lx - m_INT_MeLX) > 0.5 || Math.AbsFloat(ly - m_INT_MeLY) > 0.5)
		{
			m_INT_MeLX = lx;
			m_INT_MeLY = ly;
			float rs = INT_UIF_Draw.S(30 * m_INT_KT);
			m_INT_MeRoot.SetPos(lx - rs * 0.5, ly - rs * 0.5);
		}
		vector ypr = player.GetYawPitchRoll();
		float h = ypr[0];
		if (h < 0)
			h = h + 360;
		if (Math.AbsFloat(h - m_INT_MeHead) >= 1.0)
		{
			m_INT_MeHead = h;
			if (m_INT_MeArrow)
				m_INT_MeArrow.SetRotation(0, 0, h);
		}
	}

	// ------------------------------------------------------------------ data
	// 4 Hz: markers (own lists, the server list and its revision), zones / labels revisions, route signature
	protected void INT_PollData(float dt)
	{
		array<ref MarkerInfo> srv = g_Game.GetServerMarkers();
		int srvCount = -1;
		if (srv)
			srvCount = srv.Count();
		int rev = g_Game.INT_VPP_3DRevision();
		if (rev != m_INT_SeenSrvRev || srvCount != m_INT_SeenSrvCount)
		{
			m_INT_SeenSrvRev = rev;
			m_INT_SeenSrvCount = srvCount;
			m_INT_DirtyItems = true;
		}
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (cl)
		{
			int zr = cl.ZonesRevision();
			if (zr != m_INT_SeenZonesRev)
			{
				m_INT_SeenZonesRev = zr;
				m_INT_DirtyAnchor = true;
			}
			int lr = cl.INT_LabelsRevision();
			if (lr != m_INT_SeenLabelsRev)
			{
				m_INT_SeenLabelsRev = lr;
				m_INT_DirtyAnchor = true;
			}
		}
		if (m_INT_SlowAcc + dt < INT_SLOW)
			return;
		int ms = INT_MarkSig();
		if (ms != m_INT_SeenMarkSig)
		{
			m_INT_SeenMarkSig = ms;
			m_INT_DirtyItems = true;
		}
		int rs = INT_RouteSig();
		if (rs != m_INT_SeenRouteSig)
		{
			m_INT_SeenRouteSig = rs;
			m_INT_DirtyAnchor = true;
			m_INT_LiveValid = false;
		}
	}

	// own markers (cache, this server's list): count + positions + flags; changes made with the map closed (kit,
	// other mods) show up within 0.25 s
	protected int INT_MarkSig()
	{
		ClientMarkersCache cache = g_Game.GetClientMarkersCache();
		if (!cache)
			return 0;
		array<ref MarkerInfo> mine = cache.GetCachedArray();
		int sig = 17;
		sig = INT_SigOf(mine, sig);
		string ap = g_Game.GetConnectAddressPort();
		if (ap != "")
		{
			array<ref MarkerInfo> per = cache.GetCachedArray(ap);
			sig = INT_SigOf(per, sig);
		}
		return sig;
	}

	protected int INT_SigOf(array<ref MarkerInfo> list, int seed)
	{
		int sig = seed;
		if (!list)
			return sig * 31 + 7;
		int n = list.Count();
		sig = sig * 31 + n;
		for (int i = 0; i < n; i++)
		{
			MarkerInfo m = list.Get(i);
			if (!m)
				continue;
			vector p = m.GetPosition();
			int px = Math.Round(p[0]);
			int pz = Math.Round(p[2]);
			int fl = 0;
			if (m.IsActive())
				fl = 1;
			sig = sig * 31 + px;
			sig = sig * 31 + pz + fl;
		}
		return sig;
	}

	protected int INT_RouteSig()
	{
		INT_VPP_Route rt = INT_VPP_Route.INT_Peek();
		if (!rt)
			return 0;
		int n = rt.INT_Count();
		int next = rt.INT_NextIndex();
		int sig = n * 131 + next + 3;
		for (int i = 0; i < n; i++)
		{
			INT_VPP_RoutePoint rp = rt.INT_At(i);
			if (!rp)
				continue;
			int px = Math.Round(rp.m_INT_Pos[0]);
			int pz = Math.Round(rp.m_INT_Pos[2]);
			sig = sig * 31 + px;
			sig = sig * 31 + pz;
		}
		return sig;
	}

	// the big map's rules: «Мои» = the MarkerInfo's own 2D flag; «Сервер» / «События» = the effective mode
	// (DayZGame.INT_VPP_SrvMode, the player's override capped by the server): × hidden, 2D / 3D shown; layers apply
	protected void INT_CollectItems()
	{
		foreach (INT_VPP_MiniItem old : m_INT_Items)
		{
			if (old && old.m_INT_Root)
				INT_DropTree(old.m_INT_Root);
			if (old)
				old.m_INT_Root = null;
		}
		m_INT_Items.Clear();
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Peek();
		if (!pr)
			return;
		bool layerMine = pr.INT_IsLayerOn(INT_VPP_LAYER_MINE);
		bool layerSrv = pr.INT_IsLayerOn(INT_VPP_LAYER_SERVER);
		bool layerEv = pr.INT_IsLayerOn(INT_VPP_LAYER_EVENTS);
		if (layerMine)
		{
			g_Game.INT_VPP_LoadCacheOnce();
			ClientMarkersCache cache = g_Game.GetClientMarkersCache();
			if (cache)
			{
				array<ref MarkerInfo> mine = cache.GetCachedArray();
				INT_AddMine(mine);
				string ap = g_Game.GetConnectAddressPort();
				if (ap != "")
				{
					array<ref MarkerInfo> per = cache.GetCachedArray(ap);
					INT_AddMine(per);
				}
			}
		}
		array<ref MarkerInfo> srv = g_Game.GetServerMarkers();
		if (srv && (layerSrv || layerEv))
		{
			int n = srv.Count();
			for (int i = 0; i < n; i++)
			{
				MarkerInfo s = srv.Get(i);
				if (!s)
					continue;
				bool own = g_Game.INT_VPP_IsOwnServerMarker(s);
				if (own && !layerSrv)
					continue;
				if (!own && !layerEv)
					continue;
				if (!s.IsActive())
					continue;
				int sm = g_Game.INT_VPP_SrvMode(s);
				if (sm == INT_VPP_MODE_HIDDEN)
					continue;
				int kind = INT_VPP_MiniItem.KIND_EVENT;
				if (own)
					kind = INT_VPP_MiniItem.KIND_SERVER;
				INT_AddItem(s, kind);
			}
		}
	}

	protected void INT_AddMine(array<ref MarkerInfo> list)
	{
		if (!list)
			return;
		int n = list.Count();
		for (int i = 0; i < n; i++)
		{
			MarkerInfo m = list.Get(i);
			if (!m)
				continue;
			if (!m.IsActive())
				continue;
			INT_AddItem(m, INT_VPP_MiniItem.KIND_MINE);
		}
	}

	protected void INT_AddItem(MarkerInfo m, int kind)
	{
		INT_VPP_MiniItem it = new INT_VPP_MiniItem();
		it.m_INT_Kind = kind;
		it.m_INT_Pos = m.GetPosition();
		// the same icon as the big map draws: empty or not loadable (RF configs naming textures no mod has) -> our
		// icon by name / words (INT_VPP_IconFallback, server 21 work 2026-11-13; one FileExist per path, cached)
		string raw = m.GetIconPath();
		string nm = m.GetName();
		string path = INT_VPP_IconFallback.INT_Resolve(raw, nm);
		if (path == "")
			path = INT_VPP_Icons.INT_DefaultPath();
		it.m_INT_Icon = path;
		vector c = m.GetColor();
		it.m_INT_Color = INT_VPP_Fmt.INT_VecToArgb(c);
		it.m_INT_Name = m.GetName();
		m_INT_Items.Insert(it);
	}

	// ------------------------------------------------------------------ anchor: project everything once
	protected void INT_Anchor()
	{
		s_INT_Anchors++;
		m_INT_A = m_INT_C;
		m_INT_OX = 0;
		m_INT_OY = 0;
		if (m_INT_World)
			m_INT_World.SetPos(0, 0);
		m_INT_LiveValid = false;
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Peek();
		bool zonesOn = true;
		bool routeOn = true;
		if (pr)
		{
			zonesOn = pr.INT_IsLayerOn(INT_VPP_LAYER_ZONES);
			routeOn = pr.INT_IsLayerOn(INT_VPP_LAYER_ROUTE);
		}
		// markers: positions; a tree only inside the window + the anchor band (the clip frame cuts the rest)
		float wp = m_INT_WinP;
		float band = INT_UIF_Draw.S(INT_ANCHOR_T + 16 * m_INT_KT);
		bool pending = false;
		foreach (INT_VPP_MiniItem it : m_INT_Items)
		{
			if (!it)
				continue;
			float lx;
			float ly;
			INT_WorldLocal(it.m_INT_Pos, lx, ly);
			it.m_INT_LX = lx;
			it.m_INT_LY = ly;
			bool want = false;
			if (lx >= -band && lx <= wp + band && ly >= -band && ly <= wp + band)
				want = true;
			it.m_INT_Want = want;
			if (!want)
			{
				if (it.m_INT_Root)
				{
					INT_DropTree(it.m_INT_Root);
					it.m_INT_Root = null;
				}
				continue;
			}
			if (it.m_INT_Root)
				INT_PlaceItem(it);
			else
				pending = true;
		}
		m_INT_BuildPending = pending;
		if (pending)
			INT_BuildSome();
		// lines (zones + route legs after the next point), zone names, labels, route points
		if (m_INT_Canvas)
			m_INT_Canvas.Clear();
		m_INT_Lines = 0;
		INT_StashOld();
		m_INT_Rects.Clear();
		if (routeOn)
			INT_DrawRoute();
		if (zonesOn)
		{
			INT_DrawZones();
			INT_DrawLabels();
		}
		INT_DropList(m_INT_OldTrees);
		m_INT_OldKeys.Clear();
	}

	// the text trees of the last projection become "old"; a tree with the same key is taken back by INT_TakeOld
	protected void INT_StashOld()
	{
		INT_Stash(m_INT_ZoneTrees, m_INT_ZoneKeys);
		INT_Stash(m_INT_LabelTrees, m_INT_LabelKeys);
		INT_Stash(m_INT_RouteTrees, m_INT_RouteKeys);
	}

	protected void INT_Stash(array<Widget> trees, array<string> keys)
	{
		int n = trees.Count();
		for (int i = 0; i < n; i++)
		{
			Widget w = trees.Get(i);
			string k = "";
			if (i < keys.Count())
				k = keys.Get(i);
			m_INT_OldTrees.Insert(w);
			m_INT_OldKeys.Insert(k);
		}
		trees.Clear();
		keys.Clear();
	}

	protected Widget INT_TakeOld(string key)
	{
		int at = m_INT_OldKeys.Find(key);
		if (at < 0)
			return null;
		Widget w = m_INT_OldTrees.Get(at);
		m_INT_OldTrees.Remove(at);
		m_INT_OldKeys.Remove(at);
		return w;
	}

	protected void INT_PlaceItem(INT_VPP_MiniItem it)
	{
		if (!it.m_INT_Root)
			return;
		float box = INT_UIF_Draw.S(28 * m_INT_KT);
		it.m_INT_Root.SetPos(it.m_INT_LX - box * 0.5, it.m_INT_LY - box * 0.5);
	}

	protected void INT_BuildSome()
	{
		int budget = INT_BUILD_BUDGET;
		bool left = false;
		foreach (INT_VPP_MiniItem it : m_INT_Items)
		{
			if (!it || !it.m_INT_Want || it.m_INT_Root)
				continue;
			if (budget <= 0)
			{
				left = true;
				break;
			}
			budget--;
			INT_BuildItem(it);
			INT_PlaceItem(it);
		}
		m_INT_BuildPending = left;
	}

	// small icons (concept A): own server marker = dark diamond + hairline + tinted icon; event = amber diamond;
	// «Мои» = dark under-mask + tinted icon
	protected void INT_BuildItem(INT_VPP_MiniItem it)
	{
		float kt = m_INT_KT;
		float box = 28 * kt;
		float r = box * 0.5;
		Widget t = INT_NewTree(m_INT_World, box, box, "IntVppMiniMk");
		if (!t)
			return;
		if (it.m_INT_Kind == INT_VPP_MiniItem.KIND_SERVER)
		{
			float ds = 19 * kt;
			INT_VPPX_Look.Diamond(t, r, r, ds, true, INT_COL_DIA_BACK);
			INT_VPPX_Look.Diamond(t, r, r, ds, false, INT_VPPX_Look.WHITE);
			float isz = 12 * kt;
			INT_UIF_Draw.Texture(t, r - isz * 0.5, r - isz * 0.5, isz, isz, it.m_INT_Icon, it.m_INT_Color);
		}
		else if (it.m_INT_Kind == INT_VPP_MiniItem.KIND_EVENT)
		{
			float es = 17 * kt;
			INT_VPPX_Look.Diamond(t, r, r, es, true, INT_COL_DIA_BACK);
			INT_VPPX_Look.Diamond(t, r, r, es, false, INT_COL_AMBER);
			float eis = 11 * kt;
			INT_UIF_Draw.Texture(t, r - eis * 0.5, r - eis * 0.5, eis, eis, it.m_INT_Icon, it.m_INT_Color);
		}
		else
		{
			float bs = 22 * kt;
			INT_UIF_Draw.Texture(t, r - bs * 0.5, r - bs * 0.5, bs, bs, INT_VPP_Icons.INT_UI_MARKBACK, INT_COL_MARKBACK);
			float ms = 16 * kt;
			INT_UIF_Draw.Texture(t, r - ms * 0.5, r - ms * 0.5, ms, ms, it.m_INT_Icon, it.m_INT_Color);
		}
		INT_Finish(t);
		it.m_INT_Root = t;
	}

	// ------------------------------------------------------------------ lines (clipped in script, budgeted)
	// Liang–Barsky: the part of segment a-b inside [x0, x1] x [y0, y1]; false = nothing inside
	static bool INT_ClipSeg(float x0, float y0, float x1, float y1, inout float ax, inout float ay, inout float bx, inout float by)
	{
		float dx = bx - ax;
		float dy = by - ay;
		float t0 = 0;
		float t1 = 1;
		float nx = -dx;
		float ny = -dy;
		float q0 = ax - x0;
		float q1 = x1 - ax;
		float q2 = ay - y0;
		float q3 = y1 - ay;
		if (!INT_VPP_MiniMap.INT_ClipT(nx, q0, t0, t1))
			return false;
		if (!INT_VPP_MiniMap.INT_ClipT(dx, q1, t0, t1))
			return false;
		if (!INT_VPP_MiniMap.INT_ClipT(ny, q2, t0, t1))
			return false;
		if (!INT_VPP_MiniMap.INT_ClipT(dy, q3, t0, t1))
			return false;
		float nax = ax + dx * t0;
		float nay = ay + dy * t0;
		float nbx = ax + dx * t1;
		float nby = ay + dy * t1;
		ax = nax;
		ay = nay;
		bx = nbx;
		by = nby;
		return true;
	}

	// one edge of Liang–Barsky: p = direction term, q = distance term
	static bool INT_ClipT(float pv, float qv, inout float t0, inout float t1)
	{
		if (Math.AbsFloat(pv) < 0.000001)
		{
			if (qv < 0)
				return false;
			return true;
		}
		float r = qv / pv;
		if (pv < 0)
		{
			if (r > t1)
				return false;
			if (r > t0)
				t0 = r;
			return true;
		}
		if (r < t0)
			return false;
		if (r < t1)
			t1 = r;
		return true;
	}

	// one line of the WORLD canvas (coordinates: world-frame local px; the canvas sits INT_CANVAS_M outside)
	protected bool INT_WorldLine(float ax, float ay, float bx, float by, float w, int color)
	{
		if (!m_INT_Canvas)
			return false;
		if (m_INT_Lines >= INT_MAX_LINES)
			return false;
		float m = INT_UIF_Draw.S(INT_CANVAS_M);
		float lim = m_INT_WinP + m;
		float x1 = ax;
		float y1 = ay;
		float x2 = bx;
		float y2 = by;
		if (!INT_VPP_MiniMap.INT_ClipSeg(-m, -m, lim, lim, x1, y1, x2, y2))
			return true;
		m_INT_Canvas.DrawLine(x1 + m, y1 + m, x2 + m, y2 + m, w, color);
		m_INT_Lines++;
		INT_VPP_Stat.INT_CanvasLines = m_INT_Lines;
		return true;
	}

	protected float INT_LineW(float design)
	{
		float w = design * m_INT_F;
		if (w < 1)
			w = 1;
		return w;
	}

	// route: legs after the next point (the first leg me -> next is the live canvas) + point diamonds
	protected void INT_DrawRoute()
	{
		INT_VPP_Route rt = INT_VPP_Route.INT_Peek();
		if (!rt || !rt.INT_IsActive())
			return;
		int n = rt.INT_Count();
		int next = rt.INT_NextIndex();
		if (next < 0)
			return;
		float wUnder = INT_LineW(5);
		float wMain = INT_LineW(2);
		for (int i = next; i < n - 1; i++)
		{
			INT_VPP_RoutePoint a = rt.INT_At(i);
			INT_VPP_RoutePoint b = rt.INT_At(i + 1);
			if (!a || !b)
				continue;
			float ax;
			float ay;
			float bx;
			float by;
			INT_WorldLocal(a.m_INT_Pos, ax, ay);
			INT_WorldLocal(b.m_INT_Pos, bx, by);
			INT_WorldLine(ax, ay, bx, by, wUnder, INT_COL_ROUTE_UNDER);
			INT_WorldLine(ax, ay, bx, by, wMain, 0xD9FF7A2F);
		}
		// point diamonds: only those fully inside the window (their number is text)
		float kt = m_INT_KT;
		float inset = INT_UIF_Draw.S(INT_ANCHOR_T + 2);
		float wp = m_INT_WinP;
		for (int j = next; j < n; j++)
		{
			INT_VPP_RoutePoint rp = rt.INT_At(j);
			if (!rp)
				continue;
			float px;
			float py;
			INT_WorldLocal(rp.m_INT_Pos, px, py);
			if (px < inset || px > wp - inset || py < inset || py > wp - inset)
				continue;
			bool isNext = false;
			if (j == next)
				isNext = true;
			bool isLast = false;
			if (j == n - 1)
				isLast = true;
			int num = rt.INT_NumberOf(j);
			INT_BuildRoutePoint(px, py, isNext, isLast, num);
		}
	}

	protected void INT_BuildRoutePoint(float px, float py, bool isNext, bool isLast, int num)
	{
		float kt = m_INT_KT;
		float box = 26 * kt;
		float r = box * 0.5;
		float bp0 = INT_UIF_Draw.S(box);
		string kind = "m";
		if (isNext)
			kind = "n";
		else if (isLast)
			kind = "l";
		string key = "r|" + num.ToString() + "|" + kind;
		Widget old = INT_TakeOld(key);
		if (old)
		{
			old.SetPos(px - bp0 * 0.5, py - bp0 * 0.5);
			m_INT_RouteTrees.Insert(old);
			m_INT_RouteKeys.Insert(key);
			return;
		}
		Widget t = INT_NewTree(m_INT_World, box, box, "IntVppMiniRt");
		if (!t)
			return;
		if (isNext)
		{
			float ns = 17 * kt;
			INT_VPPX_Look.Diamond(t, r, r, ns + 2, true, INT_COL_OUTLINE);
			INT_VPPX_Look.Diamond(t, r, r, ns, true, INT_VPPX_Look.ORANGE);
			string pad = INT_VPPX_Text.Pad2(num);
			float ts = 9 * kt;
			if (ts < 8)
				ts = 8;
			INT_VPPX_Look.Text(t, 0, r - ts * 0.75, box, ts + 4, pad, ts, INT_VPPX_Look.INK, INT_UIF_Align.CENTER, true);
		}
		else if (isLast)
		{
			float fs = 14 * kt;
			INT_VPPX_Look.Diamond(t, r, r, fs, true, INT_COL_PIN_BACK);
			INT_VPPX_Look.Diamond(t, r, r, fs, false, INT_VPPX_Look.ORANGE);
			INT_VPPX_Look.Diamond(t, r, r, 5 * kt, true, INT_VPPX_Look.ORANGE);
		}
		else
		{
			float ms = 11 * kt;
			INT_VPPX_Look.Diamond(t, r, r, ms, true, INT_COL_PIN_BACK);
			INT_VPPX_Look.Diamond(t, r, r, ms, false, INT_VPPX_Look.ORANGE);
		}
		INT_Finish(t);
		float bp = INT_UIF_Draw.S(box);
		t.SetPos(px - bp * 0.5, py - bp * 0.5);
		m_INT_RouteTrees.Insert(t);
		m_INT_RouteKeys.Insert(key);
	}

	// first leg me -> next point: a static canvas, redrawn when the player moved half a pixel or the route changed
	protected void INT_DrawLiveLeg(vector p)
	{
		if (!m_INT_Live)
			return;
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Peek();
		bool routeOn = true;
		if (pr)
			routeOn = pr.INT_IsLayerOn(INT_VPP_LAYER_ROUTE);
		INT_VPP_Route rt = INT_VPP_Route.INT_Peek();
		bool active = false;
		if (rt && routeOn && rt.INT_IsActive())
			active = true;
		if (!active)
		{
			if (m_INT_LiveValid)
				return;
			m_INT_Live.Clear();
			m_INT_LiveValid = true;
			return;
		}
		if (m_INT_LiveValid)
		{
			float dm = INT_VPP_Fmt.INT_Dist2D(p, m_INT_LiveFrom);
			float dpx = dm * m_INT_Ppm;
			if (dpx < 0.5)
				return;
		}
		m_INT_LiveFrom = p;
		m_INT_LiveValid = true;
		m_INT_Live.Clear();
		int next = rt.INT_NextIndex();
		if (next < 0)
			return;
		INT_VPP_RoutePoint np = rt.INT_At(next);
		if (!np)
			return;
		float ax;
		float ay;
		float bx;
		float by;
		INT_Local(p, ax, ay);
		INT_Local(np.m_INT_Pos, bx, by);
		float wp = m_INT_WinP;
		if (!INT_VPP_MiniMap.INT_ClipSeg(0, 0, wp, wp, ax, ay, bx, by))
			return;
		int under = INT_VPP_Fmt.INT_WithAlpha(INT_COL_OUTLINE, 128);
		int main = INT_VPP_Fmt.INT_WithAlpha(INT_VPPX_Look.ORANGE, 255);
		float w1 = INT_LineW(5);
		float w2 = INT_LineW(2.5);
		m_INT_Live.DrawLine(ax, ay, bx, by, w1, under);
		m_INT_Live.DrawLine(ax, ay, bx, by, w2, main);
		s_INT_LiveDraws++;
	}

	// zones: outline (polygon / circle) clipped to the canvas, name at the visible part of the window
	protected void INT_DrawZones()
	{
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl)
			return;
		array<ref INT_VPP_Zone> zones = cl.GetZones();
		if (!zones)
			return;
		float wLine = INT_LineW(1.5);
		int nz = zones.Count();
		for (int i = 0; i < nz; i++)
		{
			INT_VPP_Zone z = zones.Get(i);
			if (!z)
				continue;
			m_INT_PX.Clear();
			m_INT_PY.Clear();
			if (z.IsCircle())
			{
				vector c = Vector(z.cx, 0, z.cz);
				float ccx;
				float ccy;
				INT_WorldLocal(c, ccx, ccy);
				float rp = z.r * m_INT_Ppm;
				// quick reject: the circle misses the canvas
				float m = INT_UIF_Draw.S(INT_CANVAS_M);
				if (ccx + rp < -m || ccx - rp > m_INT_WinP + m || ccy + rp < -m || ccy - rp > m_INT_WinP + m)
					continue;
				int seg = Math.Round(rp / 4.0);
				if (seg < 16)
					seg = 16;
				if (seg > 64)
					seg = 64;
				for (int s = 0; s < seg; s++)
				{
					float ang = Math.PI2 * s / seg;
					float vx = ccx + Math.Cos(ang) * rp;
					float vy = ccy + Math.Sin(ang) * rp;
					m_INT_PX.Insert(vx);
					m_INT_PY.Insert(vy);
				}
			}
			else
			{
				int nv = z.VertexCount();
				if (nv < 3)
					continue;
				for (int v = 0; v < nv; v++)
				{
					float vxw = z.xs.Get(v);
					float vzw = z.zs.Get(v);
					vector wv = Vector(vxw, 0, vzw);
					float lx;
					float ly;
					INT_WorldLocal(wv, lx, ly);
					m_INT_PX.Insert(lx);
					m_INT_PY.Insert(ly);
				}
			}
			int pn = m_INT_PX.Count();
			int edge = INT_VPP_Fmt.INT_WithAlpha(z.color, 0xF2);
			for (int e = 0; e < pn; e++)
			{
				int e2 = e + 1;
				if (e2 >= pn)
					e2 = 0;
				// hoisted: several call results of one type as arguments get clobbered (enforce-compile-traps)
				float sx1 = m_INT_PX.Get(e);
				float sy1 = m_INT_PY.Get(e);
				float sx2 = m_INT_PX.Get(e2);
				float sy2 = m_INT_PY.Get(e2);
				if (!INT_WorldLine(sx1, sy1, sx2, sy2, wLine, edge))
					break;
			}
			INT_ZoneName(z);
		}
	}

	// Sutherland–Hodgman of m_INT_PX/PY against [lo, hi]² into m_INT_QX/QY; the centroid of what is left
	protected bool INT_ClipPolyCentroid(float lo, float hi, out float cx, out float cy)
	{
		cx = 0;
		cy = 0;
		m_INT_QX.Clear();
		m_INT_QY.Clear();
		int n = m_INT_PX.Count();
		for (int i = 0; i < n; i++)
		{
			m_INT_QX.Insert(m_INT_PX.Get(i));
			m_INT_QY.Insert(m_INT_PY.Get(i));
		}
		array<float> tx = new array<float>();
		array<float> ty = new array<float>();
		for (int edge = 0; edge < 4; edge++)
		{
			tx.Clear();
			ty.Clear();
			int cnt = m_INT_QX.Count();
			if (cnt == 0)
				return false;
			for (int k = 0; k < cnt; k++)
			{
				int k2 = k + 1;
				if (k2 >= cnt)
					k2 = 0;
				float ax = m_INT_QX.Get(k);
				float ay = m_INT_QY.Get(k);
				float bx = m_INT_QX.Get(k2);
				float by = m_INT_QY.Get(k2);
				bool ina = INT_VPP_MiniMap.INT_Inside(edge, ax, ay, lo, hi);
				bool inb = INT_VPP_MiniMap.INT_Inside(edge, bx, by, lo, hi);
				if (ina)
				{
					tx.Insert(ax);
					ty.Insert(ay);
				}
				if (ina != inb)
				{
					float ix;
					float iy;
					INT_VPP_MiniMap.INT_Cut(edge, ax, ay, bx, by, lo, hi, ix, iy);
					tx.Insert(ix);
					ty.Insert(iy);
				}
			}
			m_INT_QX.Clear();
			m_INT_QY.Clear();
			int tn = tx.Count();
			for (int q = 0; q < tn; q++)
			{
				m_INT_QX.Insert(tx.Get(q));
				m_INT_QY.Insert(ty.Get(q));
			}
		}
		int rn = m_INT_QX.Count();
		if (rn < 3)
			return false;
		float sx = 0;
		float sy = 0;
		for (int r = 0; r < rn; r++)
		{
			sx = sx + m_INT_QX.Get(r);
			sy = sy + m_INT_QY.Get(r);
		}
		float fn = rn;
		cx = sx / fn;
		cy = sy / fn;
		return true;
	}

	static bool INT_Inside(int edge, float x, float y, float lo, float hi)
	{
		if (edge == 0)
			return x >= lo;
		if (edge == 1)
			return x <= hi;
		if (edge == 2)
			return y >= lo;
		return y <= hi;
	}

	static void INT_Cut(int edge, float ax, float ay, float bx, float by, float lo, float hi, out float ix, out float iy)
	{
		float t = 0;
		if (edge == 0 || edge == 1)
		{
			float ex = lo;
			if (edge == 1)
				ex = hi;
			float ddx = bx - ax;
			if (Math.AbsFloat(ddx) > 0.000001)
				t = (ex - ax) / ddx;
			ix = ex;
			iy = ay + (by - ay) * t;
			return;
		}
		float ey = lo;
		if (edge == 3)
			ey = hi;
		float ddy = by - ay;
		if (Math.AbsFloat(ddy) > 0.000001)
			t = (ey - ay) / ddy;
		ix = ax + (bx - ax) * t;
		iy = ey;
	}

	// the zone's name (tracked caps, its colour, dark outline) at the centre of its visible part, whole box inside
	protected void INT_ZoneName(INT_VPP_Zone z)
	{
		string nm = z.name;
		if (nm == "")
			return;
		float wp = m_INT_WinP;
		float lo = INT_UIF_Draw.S(INT_ANCHOR_T + 2);
		float hi = wp - lo;
		float cx;
		float cy;
		if (!INT_ClipPolyCentroid(lo, hi, cx, cy))
			return;
		float kt = m_INT_KT;
		float ts = 10 * kt;
		if (ts < 9)
			ts = 9;
		string caps = INT_VPPX_Text.Caps(nm);
		float roomD = (hi - lo) / m_INT_F;
		string shown = INT_VPPX_Look.FitText(caps, ts, true, roomD);
		float twD = INT_VPPX_Look.Measure(shown, ts, true) + 6;
		float thD = ts + 6;
		float twP = INT_UIF_Draw.S(twD);
		float thP = INT_UIF_Draw.S(thD);
		// never over the player arrow (the name of a zone the player stands in): under it
		float half = wp * 0.5;
		float meR = INT_UIF_Draw.S(18 * m_INT_KT);
		if (Math.AbsFloat(cy - half) < thP * 0.5 + meR && Math.AbsFloat(cx - half) < twP * 0.5 + meR)
			cy = half + meR + thP * 0.5 + 2;
		float x0 = cx - twP * 0.5;
		float y0 = cy - thP * 0.5;
		if (x0 < lo)
			x0 = lo;
		if (y0 < lo)
			y0 = lo;
		if (x0 + twP > hi)
			x0 = hi - twP;
		if (y0 + thP > hi)
			y0 = hi - thP;
		if (x0 < lo || y0 < lo)
			return;
		int col = INT_VPP_Fmt.INT_WithAlpha(z.color, 0xFF);
		string key = "z|" + z.id.ToString() + "|" + shown + "|" + col.ToString();
		Widget t = INT_TakeOld(key);
		if (t)
		{
			t.SetPos(x0, y0);
			INT_KeepText(m_INT_ZoneTrees, m_INT_ZoneKeys, t, key, x0, y0, twP, thP);
			return;
		}
		t = INT_NewTree(m_INT_World, twD, thD, "IntVppMiniZn");
		if (!t)
			return;
		TextWidget tw = INT_VPPX_Look.Text(t, 0, 0, twD, thD, shown, ts, col, INT_UIF_Align.CENTER, true);
		if (tw)
			tw.SetOutline(1, INT_COL_OUTLINE);
		INT_Finish(t);
		t.SetPos(x0, y0);
		INT_KeepText(m_INT_ZoneTrees, m_INT_ZoneKeys, t, key, x0, y0, twP, thP);
	}

	protected void INT_KeepText(array<Widget> trees, array<string> keys, Widget t, string key, float x0, float y0, float wp, float hp)
	{
		trees.Insert(t);
		keys.Insert(key);
		m_INT_Rects.Insert(x0);
		m_INT_Rects.Insert(y0);
		m_INT_Rects.Insert(x0 + wp);
		m_INT_Rects.Insert(y0 + hp);
	}

	// admin text labels (with «Зоны»): 0.42 of their size (10..22 design px), biggest first, never over another
	// label or a zone name, whole box inside the window; at most INT_MAX_LABELS
	protected void INT_DrawLabels()
	{
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl)
			return;
		array<ref INT_VPP_Label> labels = cl.INT_GetLabels();
		if (!labels)
			return;
		int n = labels.Count();
		if (n == 0)
			return;
		// order by size, biggest first (index list, sizes are few: 6 passes)
		array<int> sizes = {52, 40, 32, 24, 18, 14};
		float wp = m_INT_WinP;
		float lo = INT_UIF_Draw.S(INT_ANCHOR_T + 2);
		float hi = wp - lo;
		int made = 0;
		foreach (int sz : sizes)
		{
			for (int i = 0; i < n; i++)
			{
				if (made >= INT_MAX_LABELS)
					return;
				INT_VPP_Label l = labels.Get(i);
				if (!l || l.INT_Size != sz)
					continue;
				vector lp = Vector(l.INT_X, 0, l.INT_Z);
				float lx;
				float ly;
				INT_WorldLocal(lp, lx, ly);
				if (lx < lo || lx > hi || ly < lo || ly > hi)
					continue;
				float szf = sz;
				float ts = szf * 0.42 * m_INT_KT;
				if (ts < 10)
					ts = 10;
				if (ts > 22)
					ts = 22;
				string txt = l.INT_Text;
				float twD = INT_VPPX_Look.Measure(txt, ts, false) + 8;
				float thD = ts + 6;
				float twP = INT_UIF_Draw.S(twD);
				float thP = INT_UIF_Draw.S(thD);
				float x0 = lx - twP * 0.5;
				float y0 = ly - thP * 0.5;
				if (x0 < lo || y0 < lo || x0 + twP > hi || y0 + thP > hi)
					continue;
				if (INT_Overlaps(x0, y0, x0 + twP, y0 + thP))
					continue;
				int col = INT_VPP_Fmt.INT_WithAlpha(l.INT_Color, 0xFF);
				string key = "l|" + l.INT_Id.ToString() + "|" + txt + "|" + ts.ToString() + "|" + col.ToString();
				Widget t = INT_TakeOld(key);
				if (t)
				{
					t.SetPos(x0, y0);
					INT_KeepText(m_INT_LabelTrees, m_INT_LabelKeys, t, key, x0, y0, twP, thP);
					made++;
					continue;
				}
				t = INT_NewTree(m_INT_World, twD, thD, "IntVppMiniLb");
				if (!t)
					continue;
				TextWidget tw = INT_VPPX_Look.Text(t, 0, 0, twD, thD, txt, ts, col, INT_UIF_Align.CENTER, false);
				if (tw)
					tw.SetOutline(1, INT_COL_OUTLINE);
				INT_Finish(t);
				t.SetPos(x0, y0);
				INT_KeepText(m_INT_LabelTrees, m_INT_LabelKeys, t, key, x0, y0, twP, thP);
				made++;
			}
		}
	}

	protected bool INT_Overlaps(float x1, float y1, float x2, float y2)
	{
		int n = m_INT_Rects.Count() / 4;
		for (int i = 0; i < n; i++)
		{
			float ax = m_INT_Rects.Get(i * 4);
			float ay = m_INT_Rects.Get(i * 4 + 1);
			float bx = m_INT_Rects.Get(i * 4 + 2);
			float by = m_INT_Rects.Get(i * 4 + 3);
			if (x2 <= ax || x1 >= bx || y2 <= ay || y1 >= by)
				continue;
			return true;
		}
		return false;
	}

	// ------------------------------------------------------------------ edge pins (4 Hz, static layer)
	// off-window targets: the next route point (orange) and events (amber, nearest first) - a diamond at the edge +
	// metres; at most INT_MAX_PINS; trees reused by index, texts set only when they change
	protected void INT_UpdatePins(vector p)
	{
		if (!m_INT_Pins)
			return;
		array<vector> want = new array<vector>();
		array<int> cols = new array<int>();
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Peek();
		bool routeOn = true;
		if (pr)
			routeOn = pr.INT_IsLayerOn(INT_VPP_LAYER_ROUTE);
		float wp = m_INT_WinP;
		float inset = INT_UIF_Draw.S(10 * m_INT_KT);
		INT_VPP_Route rt = INT_VPP_Route.INT_Peek();
		if (routeOn && rt && rt.INT_IsActive())
		{
			int next = rt.INT_NextIndex();
			INT_VPP_RoutePoint np = null;
			if (next >= 0)
				np = rt.INT_At(next);
			if (np)
			{
				float rx;
				float ry;
				INT_Local(np.m_INT_Pos, rx, ry);
				if (rx < inset || rx > wp - inset || ry < inset || ry > wp - inset)
				{
					want.Insert(np.m_INT_Pos);
					cols.Insert(INT_VPPX_Look.ORANGE);
				}
			}
		}
		// events off the window, nearest first
		array<float> evD = new array<float>();
		array<vector> evP = new array<vector>();
		foreach (INT_VPP_MiniItem it : m_INT_Items)
		{
			if (!it || it.m_INT_Kind != INT_VPP_MiniItem.KIND_EVENT)
				continue;
			float ex;
			float ey;
			INT_Local(it.m_INT_Pos, ex, ey);
			if (ex >= inset && ex <= wp - inset && ey >= inset && ey <= wp - inset)
				continue;
			float d = INT_VPP_Fmt.INT_Dist2D(p, it.m_INT_Pos);
			int at = 0;
			while (at < evD.Count() && evD.Get(at) <= d)
			{
				at++;
			}
			evD.InsertAt(d, at);
			evP.InsertAt(it.m_INT_Pos, at);
		}
		int en = evP.Count();
		for (int e = 0; e < en; e++)
		{
			if (want.Count() >= INT_MAX_PINS)
				break;
			want.Insert(evP.Get(e));
			cols.Insert(INT_COL_AMBER);
		}
		int wn = want.Count();
		// drop extra trees, rebuild a tree whose colour changed
		while (m_INT_PinTrees.Count() > wn)
		{
			int last = m_INT_PinTrees.Count() - 1;
			INT_DropTree(m_INT_PinTrees.Get(last));
			m_INT_PinTrees.Remove(last);
			m_INT_PinTexts.Remove(last);
			m_INT_PinShown.Remove(last);
			m_INT_PinColors.Remove(last);
		}
		for (int i = 0; i < wn; i++)
		{
			vector wpos = want.Get(i);
			int col = cols.Get(i);
			if (i < m_INT_PinTrees.Count() && m_INT_PinColors.Get(i) != col)
			{
				INT_DropTree(m_INT_PinTrees.Get(i));
				m_INT_PinTrees.Set(i, null);
			}
			if (i >= m_INT_PinTrees.Count())
			{
				m_INT_PinTrees.Insert(null);
				m_INT_PinTexts.Insert(null);
				m_INT_PinShown.Insert("");
				m_INT_PinColors.Insert(col);
			}
			if (!m_INT_PinTrees.Get(i))
				INT_BuildPin(i, col);
			INT_PlacePin(i, wpos, p);
		}
	}

	protected void INT_BuildPin(int i, int col)
	{
		float kt = m_INT_KT;
		float box = 18 * kt;
		Widget t = INT_NewTree(m_INT_Pins, box, box, "IntVppMiniPin");
		if (!t)
			return;
		float r = box * 0.5;
		float ds = 10 * kt;
		INT_VPPX_Look.Diamond(t, r, r, ds, true, INT_COL_PIN_BACK);
		INT_VPPX_Look.Diamond(t, r, r, ds, false, col);
		float ts = 11 * kt;
		TextWidget tw = INT_VPPX_Look.Text(t, box + 2, r - ts * 0.75, 70 * kt, ts + 5, " ", ts, col, INT_UIF_Align.LEFT, false);
		if (tw)
			tw.SetOutline(1, INT_COL_OUTLINE);
		INT_VPPX_Look.SortTree(t, 300);
		m_INT_PinTrees.Set(i, t);
		m_INT_PinTexts.Set(i, tw);
		m_INT_PinShown.Set(i, "");
		m_INT_PinColors.Set(i, col);
	}

	// the diamond where the line centre -> target leaves the window (inset), the metres inside the window
	protected void INT_PlacePin(int i, vector target, vector me)
	{
		Widget t = m_INT_PinTrees.Get(i);
		if (!t)
			return;
		float wp = m_INT_WinP;
		float half = wp * 0.5;
		float inset = INT_UIF_Draw.S(10 * m_INT_KT);
		float lx;
		float ly;
		INT_Local(target, lx, ly);
		float dx = lx - half;
		float dy = ly - half;
		float ax = Math.AbsFloat(dx);
		float ay = Math.AbsFloat(dy);
		if (ax < 0.001)
			ax = 0.001;
		if (ay < 0.001)
			ay = 0.001;
		float tx = (half - inset) / ax;
		float ty = (half - inset) / ay;
		float tt = tx;
		if (ty < tt)
			tt = ty;
		float qx = half + dx * tt;
		float qy = half + dy * tt;
		float box = INT_UIF_Draw.S(18 * m_INT_KT);
		t.SetPos(qx - box * 0.5, qy - box * 0.5);
		TextWidget tw = m_INT_PinTexts.Get(i);
		if (!tw)
			return;
		float d = INT_VPP_Fmt.INT_Dist2D(me, target);
		int key = INT_VPP_Metres.INT_MetresKey(d);
		string txt = INT_VPP_Metres.INT_MetresText(key);
		// text side: to the inside of the window (left / right edge: beside the diamond; top / bottom: under / over)
		float kt = m_INT_KT;
		float tsD = 11 * kt;
		float twD = INT_VPPX_Look.Measure(txt, tsD, false) + 6;
		float boxD = 18 * kt;
		float txD = boxD + 2;
		float tyD = boxD * 0.5 - tsD * 0.75;
		int align = INT_UIF_Align.LEFT;
		bool sideEdge = false;
		if (tx <= ty)
			sideEdge = true;
		if (sideEdge)
		{
			if (dx > 0)
			{
				txD = -twD - 2;
				align = INT_UIF_Align.RIGHT;
			}
		}
		else
		{
			txD = boxD * 0.5 - twD * 0.5;
			align = INT_UIF_Align.CENTER;
			tyD = boxD + 1;
			if (dy > 0)
				tyD = -tsD - 6;
		}
		string shown = m_INT_PinShown.Get(i);
		if (shown != txt)
		{
			m_INT_PinShown.Set(i, txt);
			tw.SetText(txt);
		}
		INT_UIF_Draw.Place(tw, txD, tyD, twD, tsD + 5);
	}

	// ------------------------------------------------------------------ TEST hooks
	// the tree of the marker at `pos` (null = not drawn)
	Widget INT_TestItemRoot(vector pos, float tolM)
	{
		foreach (INT_VPP_MiniItem it : m_INT_Items)
		{
			if (!it || !it.m_INT_Root)
				continue;
			float dx = it.m_INT_Pos[0] - pos[0];
			float dz = it.m_INT_Pos[2] - pos[2];
			if (dx * dx + dz * dz <= tolM * tolM)
				return it.m_INT_Root;
		}
		return null;
	}

	// TEST: the icon path drawn for the item at pos ("" = no item)
	string INT_TestItemIcon(vector pos, float tolM)
	{
		foreach (INT_VPP_MiniItem it : m_INT_Items)
		{
			if (!it)
				continue;
			float dx = it.m_INT_Pos[0] - pos[0];
			float dz = it.m_INT_Pos[2] - pos[2];
			if (dx * dx + dz * dz <= tolM * tolM)
				return it.m_INT_Icon;
		}
		return "";
	}

	// the text trees (zone names + labels) for the "fully inside" check
	void INT_TestTextTrees(array<Widget> dst)
	{
		foreach (Widget z : m_INT_ZoneTrees)
		{
			if (z)
				dst.Insert(z);
		}
		foreach (Widget l : m_INT_LabelTrees)
		{
			if (l)
				dst.Insert(l);
		}
	}

	Widget INT_TestClip()
	{
		return m_INT_Clip;
	}

	// TEST: metres between the map's real centre and the position it follows (player / test position)
	float INT_TestCentreError()
	{
		if (!m_INT_Map)
			return -1;
		vector c = m_INT_Map.GetMapPos();
		float d = INT_VPP_Fmt.INT_Dist2D(c, m_INT_SetPos);
		return d;
	}

	void INT_SetTestPos(bool on, vector pos)
	{
		m_INT_TestPosOn = on;
		m_INT_TestPos = pos;
	}

	// mode 0: MapWidget.SetAlpha(a); 1: SetColor(white with alpha a) and SetAlpha(1); 2: back to the prefs;
	// 3: the host frame's alpha (MapWidget inheritalpha 1); 4: MapWidget flags SOURCEALPHA | BLEND + SetAlpha(a)
	void INT_TestMapAlpha(int mode, float a)
	{
		if (!m_INT_Map || !m_INT_MapHost)
			return;
		if (mode == 3)
		{
			m_INT_Map.SetColor(0xFFFFFFFF);
			m_INT_Map.SetAlpha(1);
			m_INT_MapHost.SetAlpha(a);
			return;
		}
		if (mode == 4)
		{
			m_INT_MapHost.SetAlpha(1);
			m_INT_Map.SetFlags(WidgetFlags.SOURCEALPHA | WidgetFlags.BLEND);
			m_INT_Map.SetAlpha(a);
			return;
		}
		if (mode == 0)
		{
			m_INT_Map.SetColor(0xFFFFFFFF);
			m_INT_Map.SetAlpha(a);
			return;
		}
		if (mode == 1)
		{
			int ai = Math.Round(a * 255);
			int col = INT_VPP_Fmt.INT_WithAlpha(0xFFFFFFFF, ai);
			m_INT_Map.SetAlpha(1);
			m_INT_Map.SetColor(col);
			return;
		}
		m_INT_Map.SetColor(0xFFFFFFFF);
		m_INT_Map.SetAlpha(1);
		m_INT_MapHost.SetAlpha(1);
		m_INT_Map.ClearFlags(WidgetFlags.SOURCEALPHA | WidgetFlags.BLEND);
	}

	// a red diagonal through the whole world canvas (40 px past the window on every side) and a long text whose
	// right half lies outside the window: does the clip frame cut canvas pixels / text?
	void INT_TestProbeClip(bool on)
	{
		if (m_INT_ProbeText)
			INT_DropTree(m_INT_ProbeText);
		m_INT_ProbeText = null;
		if (!on || !m_INT_Canvas || !m_INT_World)
			return;
		float m = INT_UIF_Draw.S(INT_CANVAS_M);
		float full = m_INT_WinP + m * 2;
		m_INT_Canvas.DrawLine(0, 0, full, full, 3, 0xFFFF2020);
		m_INT_Canvas.DrawLine(0, full * 0.5, full, full * 0.5, 3, 0xFF20FF20);
		float wd = m_INT_WinD;
		Widget t = INT_NewTree(m_INT_World, 240, 24, "IntVppMiniProbe");
		if (!t)
			return;
		TextWidget tw = INT_VPPX_Look.Text(t, 0, 0, 240, 24, "ПРОБА ОБРЕЗКИ ТЕКСТА 0123456789", 16, 0xFFFFFF00, INT_UIF_Align.LEFT, true);
		INT_Finish(t);
		float x = INT_UIF_Draw.S(wd - 110);
		float y = INT_UIF_Draw.S(wd * 0.25);
		t.SetPos(x, y);
		m_INT_ProbeText = t;
	}

	// ------------------------------------------------------------------ keys (MissionGameplay.OnUpdate, world only)
	void INT_KeyToggle()
	{
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
		if (!pr)
			return;
		bool on = true;
		if (pr.INT_MiniIsOn())
			on = false;
		pr.INT_MiniSetOn(on);
		pr.INT_Save();
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
		if (on && INT_VPP_MiniMap.INT_GpsState(m_INT_Since) == 2)
			INT_UIF_Toast.Info(INT_TXT_GPS_TOAST);
	}

	// dir -1 = closer (Ctrl + Num+), +1 = wider (Ctrl + Num-); only while the minimap is shown
	void INT_KeyZoom(int dir)
	{
		if (!INT_IsShown())
			return;
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
		if (!pr)
			return;
		int cur = pr.INT_MiniGetZoom();
		int next = INT_VPP_Prefs.INT_MiniClampZoom(cur + dir);
		if (next == cur)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
			return;
		}
		pr.INT_MiniSetZoom(next);
		pr.INT_Save();
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
	}
}

// the vanilla HUD panel (m_HudPanelWidget is protected): hidden by a menu, a cinematic camera, a mod
modded class IngameHud
{
	bool INT_VPP_HudPanelShown()
	{
		if (!m_HudPanelWidget)
			return false;
		return m_HudPanelWidget.IsVisible();
	}
}

// Mission hooks: start / finish, the per-frame update and the three keys (always super first).
modded class MissionGameplay
{
	protected UAInput m_INT_VPP_KeyMini;
	protected UAInput m_INT_VPP_KeyZoomIn;
	protected UAInput m_INT_VPP_KeyZoomOut;
	// Measured 2026-11-13 (real keys on the stand, INT_AutoRun loaded): ONE LCtrl + Num+ press fired our combo AND
	// INT_AutoRun's plain Num+ (UAAutoRunToggle). Fix: while the modifier of our zoom combos is held, every input
	// bound to the bare main key of a combo (Num+ / Num-, found by a scan of the active inputs) is Supress()-ed each
	// frame - the press of the next frame is swallowed, so the plain-key action never sees the combo. Plain Num+
	// without the modifier is untouched. Rescanned every 5 s (the player may rebind in the options).
	// 2026-11-14: the show / hide key is a combo too (LCtrl + Num /): all three combos are scanned, each shadowed
	// input keeps the modifier kind of the combo it collides with (a rebind may give the keys different modifiers).
	protected ref array<UAInput> m_INT_VPP_Shadowed;
	protected ref array<int> m_INT_VPP_ShadowMods;
	protected float m_INT_VPP_ScanAcc;
	static int s_INT_VPP_ShadowedCount;
	static string s_INT_VPP_ShadowedNames;
	// TEST: presses seen by our handlers (the kit compares them with other mods' inputs)
	static int s_INT_VPP_MiniPresses;
	static int s_INT_VPP_ZoomInPresses;
	static int s_INT_VPP_ZoomOutPresses;

	override void OnMissionStart()
	{
		super.OnMissionStart();
		if (GetGame().IsDedicatedServer())
			return;
		INT_VPP_MiniMap.INT_Start();
		UAInputAPI api = GetUApi();
		if (api)
		{
			m_INT_VPP_KeyMini = api.GetInputByName("UAIntVppMiniMap");
			m_INT_VPP_KeyZoomIn = api.GetInputByName("UAIntVppMiniMapZoomIn");
			m_INT_VPP_KeyZoomOut = api.GetInputByName("UAIntVppMiniMapZoomOut");
			INT_VPP_MigrateMiniKey(api);
		}
	}

	// 2026-11-14: the show / hide key moved from «\» to LCtrl + Num /. DayZ keeps every input of the player's saved
	// preset (<profile>\<name>.dayz_preset_User.xml, re-exported at each start), so a player who ran the «\» build
	// would keep «\» for ever. ONCE per Client.json (INT_MiniKeyMig): a keyboard binding that is exactly the old
	// default (one key «\», no combo) becomes LCtrl + Num / and the preset is exported (as Settings -> Controls ->
	// Apply does). Any other binding - the player's own choice - stays as it is.
	protected void INT_VPP_MigrateMiniKey(UAInputAPI api)
	{
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
		if (!pr || !api || pr.INT_IsMiniKeyMigrated())
			return;
		UAInput inp = m_INT_VPP_KeyMini;
		bool moved = false;
		if (inp)
		{
			int keep = inp.AlternativeIndex();
			int n = inp.AlternativeCount();
			int hit = -1;
			for (int a = 0; a < n; a++)
			{
				inp.SelectAlternative(a);
				if (inp.IsCombo() || inp.BindingCount() != 1)
					continue;
				if (!inp.CheckBindDevice(0, EUAINPUT_DEVICE_KEYBOARD))
					continue;
				int b = inp.Binding(0);
				string bn = api.GetButtonName(b);
				if (bn == "\\")
				{
					hit = a;
					break;
				}
			}
			if (hit >= 0)
			{
				inp.SelectAlternative(hit);
				inp.ClearDeviceBind(EUAINPUT_DEVICE_KEYBOARDMOUSE);
				inp.BindCombo("kLControl");
				inp.BindCombo("kDivide");
				moved = true;
			}
			inp.SelectAlternative(keep);
		}
		pr.INT_SetMiniKeyMigrated();
		pr.INT_Save();
		if (!moved)
			return;
		api.Export();
		string now = INT_VPP_MiniPanel.INT_KeyName("UAIntVppMiniMap", "-");
		Print("[INT_VPP] minimap show / hide key: the old default \\ moved to " + now + " (once per profile)");
	}

	override void OnMissionFinish()
	{
		super.OnMissionFinish();
		if (GetGame().IsDedicatedServer())
			return;
		INT_VPP_MiniMap.INT_Shutdown();
		m_INT_VPP_KeyMini = null;
		m_INT_VPP_KeyZoomIn = null;
		m_INT_VPP_KeyZoomOut = null;
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		INT_VPP_MiniMap mm = INT_VPP_MiniMap.INT_Peek();
		if (!mm)
			return;
		m_INT_VPP_ScanAcc = m_INT_VPP_ScanAcc - timeslice;
		if (m_INT_VPP_ScanAcc <= 0)
		{
			m_INT_VPP_ScanAcc = 5.0;
			INT_VPP_ScanCombos();
		}
		INT_VPP_ShadowBareKeys();
		INT_VPP_MiniKeys(mm);
		mm.INT_Update(timeslice);
	}

	// modifier of a keyboard combo binding by its localised name: 1 Ctrl, 2 Shift, 3 Alt, 0 none / unknown
	static int INT_VPP_ModKindOf(string name)
	{
		if (name.IndexOf("Ctrl") >= 0 || name.IndexOf("CTRL") >= 0 || name.IndexOf("Strg") >= 0)
			return 1;
		if (name.IndexOf("Shift") >= 0 || name.IndexOf("SHIFT") >= 0)
			return 2;
		if (name.IndexOf("Alt") >= 0 || name.IndexOf("ALT") >= 0)
			return 3;
		return 0;
	}

	// main key + modifier kind of the first keyboard combo alternative of `inp` (main 0 = none); keeps the alternative
	protected int INT_VPP_ComboMain(UAInput inp, out int modKind)
	{
		modKind = 0;
		if (!inp)
			return 0;
		int keep = inp.AlternativeIndex();
		int main = 0;
		int n = inp.AlternativeCount();
		for (int a = 0; a < n; a++)
		{
			inp.SelectAlternative(a);
			if (!inp.IsCombo() || inp.BindingCount() < 2)
				continue;
			if (!inp.CheckBindDevice(0, EUAINPUT_DEVICE_KEYBOARD))
				continue;
			int mod = inp.Binding(0);
			int last = inp.BindingCount() - 1;
			main = inp.Binding(last);
			string mn = GetUApi().GetButtonName(mod);
			modKind = MissionGameplay.INT_VPP_ModKindOf(mn);
			break;
		}
		inp.SelectAlternative(keep);
		return main;
	}

	// one combo of ours into the scan lists (main key + modifier kind); no combo / an unknown modifier = skipped
	protected void INT_VPP_AddCombo(UAInput inp, array<int> mains, array<int> mods)
	{
		int kind;
		int main = INT_VPP_ComboMain(inp, kind);
		if (main == 0 || kind == 0)
			return;
		mains.Insert(main);
		mods.Insert(kind);
	}

	protected void INT_VPP_ScanCombos()
	{
		if (!m_INT_VPP_Shadowed)
			m_INT_VPP_Shadowed = new array<UAInput>();
		if (!m_INT_VPP_ShadowMods)
			m_INT_VPP_ShadowMods = new array<int>();
		m_INT_VPP_Shadowed.Clear();
		m_INT_VPP_ShadowMods.Clear();
		s_INT_VPP_ShadowedCount = 0;
		s_INT_VPP_ShadowedNames = "";
		UAInputAPI api = GetUApi();
		if (!api)
			return;
		array<int> mains = new array<int>();
		array<int> mods = new array<int>();
		INT_VPP_AddCombo(m_INT_VPP_KeyMini, mains, mods);
		INT_VPP_AddCombo(m_INT_VPP_KeyZoomIn, mains, mods);
		INT_VPP_AddCombo(m_INT_VPP_KeyZoomOut, mains, mods);
		int nc = mains.Count();
		if (nc == 0)
			return;
		array<int> ids = new array<int>();
		api.GetActiveInputs(ids);
		string names = "";
		int distinct = 0;
		foreach (int id : ids)
		{
			UAInput inp = api.GetInputByID(id);
			if (!inp || inp == m_INT_VPP_KeyZoomIn || inp == m_INT_VPP_KeyZoomOut || inp == m_INT_VPP_KeyMini)
				continue;
			int keep = inp.AlternativeIndex();
			int alts = inp.AlternativeCount();
			int hitMask = 0;
			for (int a = 0; a < alts; a++)
			{
				inp.SelectAlternative(a);
				if (inp.IsCombo() || inp.BindingCount() != 1)
					continue;
				int b = inp.Binding(0);
				if (b == 0)
					continue;
				for (int c = 0; c < nc; c++)
				{
					if (b == mains.Get(c))
						hitMask = hitMask | (1 << mods.Get(c));
				}
			}
			inp.SelectAlternative(keep);
			if (hitMask == 0)
				continue;
			for (int k = 1; k <= 3; k++)
			{
				int bit = 1 << k;
				if ((hitMask & bit) == 0)
					continue;
				m_INT_VPP_Shadowed.Insert(inp);
				m_INT_VPP_ShadowMods.Insert(k);
			}
			distinct++;
			if (names.Length() < 200)
				names = names + id.ToString() + " ";
		}
		s_INT_VPP_ShadowedCount = distinct;
		s_INT_VPP_ShadowedNames = names;
	}

	protected void INT_VPP_ShadowBareKeys()
	{
		if (!m_INT_VPP_Shadowed || !m_INT_VPP_ShadowMods)
			return;
		int n = m_INT_VPP_Shadowed.Count();
		if (n == 0 || n != m_INT_VPP_ShadowMods.Count())
			return;
		bool ctrl = false;
		bool shift = false;
		bool alt = false;
		if (KeyState(KeyCode.KC_LCONTROL) != 0 || KeyState(KeyCode.KC_RCONTROL) != 0)
			ctrl = true;
		if (KeyState(KeyCode.KC_LSHIFT) != 0 || KeyState(KeyCode.KC_RSHIFT) != 0)
			shift = true;
		if (KeyState(KeyCode.KC_LMENU) != 0 || KeyState(KeyCode.KC_RMENU) != 0)
			alt = true;
		if (!ctrl && !shift && !alt)
			return;
		bool held;
		int kind;
		for (int i = 0; i < n; i++)
		{
			kind = m_INT_VPP_ShadowMods.Get(i);
			held = false;
			if (kind == 1 && ctrl)
				held = true;
			else if (kind == 2 && shift)
				held = true;
			else if (kind == 3 && alt)
				held = true;
			if (!held)
				continue;
			UAInput sh = m_INT_VPP_Shadowed.Get(i);
			if (sh)
				sh.Supress();
		}
	}

	// keys only in the world: no menu at all (not even the chat input), no text field busy, the player can act
	protected void INT_VPP_MiniKeys(INT_VPP_MiniMap mm)
	{
		if (!m_INT_VPP_KeyMini && !m_INT_VPP_KeyZoomIn && !m_INT_VPP_KeyZoomOut)
			return;
		bool toggle = false;
		bool zin = false;
		bool zout = false;
		if (m_INT_VPP_KeyMini && m_INT_VPP_KeyMini.LocalPress())
			toggle = true;
		if (m_INT_VPP_KeyZoomIn && m_INT_VPP_KeyZoomIn.LocalPress())
			zin = true;
		if (m_INT_VPP_KeyZoomOut && m_INT_VPP_KeyZoomOut.LocalPress())
			zout = true;
		if (!toggle && !zin && !zout)
			return;
		UIManager um = GetGame().GetUIManager();
		if (um && um.GetMenu())
			return;
		if (g_Game.IsKeyboardBusy())
			return;
		int st = INT_VPP_3D.INT_ScreenState();
		if (st != INT_VPP_3D.INT_SCREEN_DRAW)
			return;
		if (toggle)
		{
			s_INT_VPP_MiniPresses++;
			mm.INT_KeyToggle();
		}
		if (zin)
		{
			s_INT_VPP_ZoomInPresses++;
			mm.INT_KeyZoom(-1);
		}
		if (zout)
		{
			s_INT_VPP_ZoomOutPresses++;
			mm.INT_KeyZoom(1);
		}
	}
}
