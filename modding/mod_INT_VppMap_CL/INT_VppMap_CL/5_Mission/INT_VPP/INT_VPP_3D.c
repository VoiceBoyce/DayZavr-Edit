// ============================================================================
// VanillaPPMap «Экспедиция» B — 3D markers manager (HUD3D, BUILD_PLAN §2 / §5 / §8).
// ONE update loop for every VPP3DMarker of g_Game (client + server arrays) and the route beacon; no
// per-marker CallLater. Driven by the modded MissionGameplay at the end of this file.
//   cull pass (every 0.2 s, or at once after a rebuild / K / prefs change): per marker
//     no widget tree when: screen blocked (a menu other than chat is open — the map included —, no / dead
//     player), K hid everything (DayZGame.INT_VPP_Is3DTempHidden), master 3D off, layer off,
//     !IsMarkerVisible(), beyond INT_VPP_Prefs.INT_Dist3D(layer) (2D metres) → INT_DropTree (kind 6 down);
//     in range → INT_BuildTree lazily (kind 6 up, at most INT_BUILD_BUDGET new trees per pass).
//     Markers that left the arrays but are still alive (old K list) lose their tree in the sweep.
//   frame pass (every frame, only markers that HAVE a tree): GetScreenPos, SetPos only when the pixel
//     changed, Show only on change, metres text only when the shown value changes.
//   once a second: DayZGame.INT_VPP_CheckServerMarkers (RF inserts into GetServerMarkers() at any time).
// Route beacon (next route point: outline diamond + number + metres, tick down to the ground) lives here too
// (kind 6): route active, «Маршрут» layer on, master on, not K-hidden, CanUse3DMarkers, within the route cap.
// Lifecycle: INT_Start at mission start, INT_Shutdown at mission finish (all trees unlinked). INT_Get() never
// returns null; an instance created outside a mission stays dormant (no trees, no subscriptions).
// ============================================================================

class INT_VPP_Beacon : Managed
{
	static const float INT_W = 200;
	static const float INT_H = 132;
	static const float INT_AX = 100;           // anchor = ground point, bottom centre
	static const float INT_AY = 132;
	static const float INT_DY = 66;            // diamond centre
	static const float INT_SIDE = 30;          // diamond side (widget 42)
	static const int INT_COL_NUM = 0xFFFF9A5E;
	static const int INT_COL_TICK = 0xCCFF7A2F;
	static const int INT_COL_PAD = 0x990B0B0B;
	static const int INT_COL_OUTLINE = 0xFF0B0B0B;
	static const float INT_EDGE = 80;

	protected Widget m_INT_Root;               // weak
	protected bool m_INT_Has;
	protected TextWidget m_INT_Num;
	protected TextWidget m_INT_Dist;
	protected vector m_INT_Target;
	protected vector m_INT_Ground;
	protected int m_INT_Number;
	protected int m_INT_LastKey;
	protected bool m_INT_Shown;
	protected int m_INT_LastPx;
	protected int m_INT_LastPy;
	protected float m_INT_AnchorX;
	protected float m_INT_AnchorY;
	// 3D look (INT_VPP_Prefs, 2026-11-12): size factor of the live tree and its cull margin (screen px)
	protected float m_INT_K = 1.0;
	protected float m_INT_Edge = 80;
	// de-clutter obstacle (screen px from the root corner; text half width follows the shown metres)
	protected float m_INT_ObCx;
	protected float m_INT_ObTop;
	protected float m_INT_ObBottom;
	protected float m_INT_DiaHalfW;
	protected float m_INT_TextHalfW;
	protected float m_INT_DistSize = 18;

	void INT_VPP_Beacon()
	{
		m_INT_Number = -1;
		m_INT_LastKey = -1;
		m_INT_LastPx = -100000;
		m_INT_LastPy = -100000;
	}

	void ~INT_VPP_Beacon()
	{
		INT_Drop();
	}

	bool INT_HasTree()
	{
		if (m_INT_Has && !m_INT_Root)
			INT_Drop();
		return m_INT_Has;
	}

	// new target / number: tree built once, number text only when it changed
	void INT_SetTarget(vector pos, int number)
	{
		if (!INT_HasTree())
			INT_Build();
		if (!m_INT_Has)
			return;
		if (vector.DistanceSq(pos, m_INT_Target) > 0.0001 || m_INT_Number < 0)
		{
			m_INT_Target = pos;
			float gy = GetGame().SurfaceY(pos[0], pos[2]);
			m_INT_Ground = Vector(pos[0], gy + 0.2, pos[2]);
			m_INT_LastKey = -1;
		}
		if (number != m_INT_Number)
		{
			m_INT_Number = number;
			if (m_INT_Num)
				m_INT_Num.SetText(INT_VPPX_Text.Pad2(number));
		}
	}

	void INT_Build()
	{
		if (m_INT_Has)
			INT_Drop();
		if (!GetGame() || GetGame().IsDedicatedServer() || !GetGame().GetWorkspace())
			return;
		// size and transparency of everything drawn in 3D (2026-11-12; 100 % / 0 % = the look before)
		float k = 1.0;
		float alphaK = 1.0;
		bool distOn = true;
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
		if (pr)
		{
			k = pr.INT_3DScaleK();
			alphaK = pr.INT_3DAlphaK();
			distOn = pr.INT_Is3DDistOn();
		}
		Widget root = INT_UIF_Draw.Frame(null, 0, 0, INT_W * k, INT_H * k, "INT_VPP_RouteBeacon");
		if (!root)
			return;
		m_INT_Root = root;
		m_INT_Has = true;
		INT_VPP_Stat.INT_Up(INT_VPP_Stat.INT_KIND_3D);
		root.Show(false);
		m_INT_Shown = false;
		m_INT_LastKey = -1;
		m_INT_LastPx = -100000;
		m_INT_LastPy = -100000;
		m_INT_Number = -1;
		m_INT_K = k;
		m_INT_Edge = INT_EDGE * k;
		float ax = INT_AX * k;
		float ay = INT_AY * k;
		float dy = INT_DY * k;
		float side = INT_SIDE * k;
		m_INT_AnchorX = INT_UIF_Draw.S(ax);
		m_INT_AnchorY = INT_UIF_Draw.S(ay);

		float half = side * INT_VPPX_Look.SQRT2 * 0.5;
		// tick from the diamond's lower corner down to the ground point
		float tickTop = dy + half;
		INT_VPPX_Look.Fill(root, ax, tickTop, 1, ay - tickTop, INT_COL_TICK);
		float padSide = side + 3 * k;
		INT_VPPX_Look.Diamond(root, ax, dy, padSide, true, INT_COL_PAD);
		INT_VPPX_Look.Diamond(root, ax, dy, side, false, INT_VPPX_Look.ORANGE);
		// text floor like the markers (VPP3DMarker.INT_TEXT_FLOOR): number and metres never below 75 % of the design
		// size, still centred on the diamond; at k >= the floor the old formulas (pixel-identical)
		float kt = k;
		if (kt < VPP3DMarker.INT_TEXT_FLOOR)
			kt = VPP3DMarker.INT_TEXT_FLOOR;
		TextWidget num = INT_UIF_Draw.Text(root, ax - 30 * kt, dy - 10 * kt, 60 * kt, 20 * kt, "00", 15 * kt, INT_COL_NUM, INT_UIF_Align.CENTER);
		if (num)
			num.SetOutline(1, INT_COL_OUTLINE);
		m_INT_Num = num;
		float distY = dy - half - 30 * k;
		float distX = 0;
		float distW = INT_W * k;
		if (kt > k)
		{
			distY = dy - half - 30 * kt;
			distX = ax - INT_AX * kt;
			distW = INT_W * kt;
		}
		m_INT_DistSize = 18 * kt;
		// «Расстояние» of «ВИД 3D-МЕТОК» off (2026-11-14): the number in the diamond stays, no metres above it
		TextWidget dist = null;
		if (distOn)
		{
			dist = INT_UIF_Draw.Text(root, distX, distY, distW, 24 * kt, "0 м", m_INT_DistSize, INT_VPPX_Look.WHITE, INT_UIF_Align.CENTER);
			if (dist)
				dist.SetOutline(1, INT_COL_OUTLINE);
		}
		m_INT_Dist = dist;
		// obstacle for the markers' label de-clutter: metres text + diamond (screen px from the root corner)
		float padHalf = padSide * INT_VPPX_Look.SQRT2 * 0.5;
		float obTop = distY;
		if (!distOn)
			obTop = dy - padHalf;
		m_INT_ObCx = INT_UIF_Draw.S(ax);
		m_INT_ObTop = INT_UIF_Draw.S(obTop);
		m_INT_ObBottom = INT_UIF_Draw.S(dy + padHalf);
		m_INT_DiaHalfW = INT_UIF_Draw.S(padHalf);
		m_INT_TextHalfW = m_INT_DiaHalfW;
		INT_VPPX_Look.INT_FadeTree(root, alphaK);
	}

	// screen rect of the diamond + its metres text while shown (the markers' labels give way to it); false = none
	bool INT_ObstacleRect(out float x1, out float y1, out float x2, out float y2)
	{
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;
		if (!m_INT_Has || !m_INT_Shown)
			return false;
		float hw = m_INT_DiaHalfW;
		if (m_INT_TextHalfW > hw)
			hw = m_INT_TextHalfW;
		float cxs = m_INT_LastPx + m_INT_ObCx;
		x1 = cxs - hw;
		x2 = cxs + hw;
		y1 = m_INT_LastPy + m_INT_ObTop;
		y2 = m_INT_LastPy + m_INT_ObBottom;
		return true;
	}

	// TEST / tools: the live tree's root (null without a tree) and its size factor
	Widget INT_GetRoot()
	{
		return m_INT_Root;
	}

	float INT_GetScaleK()
	{
		return m_INT_K;
	}

	void INT_Hide()
	{
		Widget root = m_INT_Root;
		if (root && m_INT_Shown)
		{
			root.Show(false);
			m_INT_Shown = false;
		}
	}

	void INT_Drop()
	{
		if (!m_INT_Has)
			return;
		Widget root = m_INT_Root;
		if (root)
			root.Unlink();
		m_INT_Root = null;
		m_INT_Num = null;
		m_INT_Dist = null;
		m_INT_Has = false;
		m_INT_Shown = false;
		m_INT_Number = -1;
		INT_VPP_Stat.INT_Down(INT_VPP_Stat.INT_KIND_3D);
	}

	void INT_Frame(vector playerPos, float screenW, float screenH)
	{
		if (!m_INT_Has)
			return;
		Widget root = m_INT_Root;
		if (!root)
			return;
		vector sp = GetGame().GetScreenPos(m_INT_Ground);
		bool on = true;
		if (sp[2] <= 0)
		{
			on = false;
		}
		else if (sp[0] < -m_INT_Edge || sp[0] > screenW + m_INT_Edge)
		{
			on = false;
		}
		else if (sp[1] < -m_INT_Edge || sp[1] > screenH + m_INT_Edge)
		{
			on = false;
		}
		if (on != m_INT_Shown)
		{
			root.Show(on);
			m_INT_Shown = on;
		}
		if (!on)
			return;
		int px = Math.Round(sp[0] - m_INT_AnchorX);
		int py = Math.Round(sp[1] - m_INT_AnchorY);
		if (px != m_INT_LastPx || py != m_INT_LastPy)
		{
			root.SetPos(px, py);
			m_INT_LastPx = px;
			m_INT_LastPy = py;
		}
		float d = INT_VPP_Metres.INT_Dist2D(playerPos, m_INT_Target);
		int key = INT_VPP_Metres.INT_MetresKey(d);
		if (key != m_INT_LastKey)
		{
			m_INT_LastKey = key;
			string mt = INT_VPP_Metres.INT_MetresText(key);
			if (m_INT_Dist)
			{
				m_INT_Dist.SetText(mt);
				// only when the shown value changes: the obstacle width of the metres text
				float tw = INT_VPPX_Look.Measure(mt, m_INT_DistSize);
				m_INT_TextHalfW = INT_UIF_Draw.S(tw * 0.5);
			}
		}
	}
}

class INT_VPP_3D : Managed
{
	static const float INT_CULL_PERIOD = 0.2;
	static const float INT_LABEL_PERIOD = 0.25;
	static const float INT_CHECK_PERIOD = 1.0;
	static const int INT_BUILD_BUDGET = 24;

	protected static ref INT_VPP_3D s_INT_Inst;

	protected bool m_INT_Running;
	// weak: the markers that currently have a tree
	protected ref array<VPP3DMarker> m_INT_Live;
	protected ref INT_VPP_Beacon m_INT_Beacon;
	protected float m_INT_CullAcc;
	protected float m_INT_CheckAcc;
	protected float m_INT_LabelAcc;
	protected int m_INT_Pass;
	protected int m_INT_Budget;
	protected int m_INT_SeenRev;
	protected bool m_INT_ForceCull;
	protected bool m_INT_BuildPending;
	protected bool m_INT_BuildAllowed;
	protected bool m_INT_Blocked;
	protected bool m_INT_Suspended;
	protected bool m_INT_HiddenAll;
	protected bool m_INT_TempHidden;
	protected int m_INT_ScreenW;
	protected int m_INT_ScreenH;
	// preferences cache (INT_Refresh / prefs change / once a second)
	protected bool m_INT_MasterOn;
	protected ref array<bool> m_INT_LayerOn;
	protected ref array<float> m_INT_MaxSq;
	// 3D look the live trees were built with (INT_VPP_Prefs size / transparency); a change drops them all
	protected int m_INT_LookScale = -1;
	protected int m_INT_LookFade = -1;
	// 2026-11-14: names / metres switches (-1 = not read yet, 0 off, 1 on)
	protected int m_INT_LookNames = -1;
	protected int m_INT_LookDist = -1;
	protected int m_INT_LookRebuilds;
	// weak: the singletons we listen to
	protected INT_VPP_Prefs m_INT_BoundPrefs;
	protected INT_VPP_Route m_INT_BoundRoute;

	void INT_VPP_3D()
	{
		m_INT_Live = new array<VPP3DMarker>();
		m_INT_LayerOn = new array<bool>();
		m_INT_MaxSq = new array<float>();
		for (int i = 0; i < INT_VPP_LAYER_COUNT; i++)
		{
			m_INT_LayerOn.Insert(true);
			m_INT_MaxSq.Insert(0);
		}
		m_INT_MasterOn = true;
		m_INT_SeenRev = -1;
	}

	void ~INT_VPP_3D()
	{
		INT_End();
	}

	// ------------------------------------------------------------------ singleton (§8)
	static INT_VPP_3D INT_Get()
	{
		if (!s_INT_Inst)
			s_INT_Inst = new INT_VPP_3D();
		return s_INT_Inst;
	}

	static INT_VPP_3D INT_Peek()
	{
		return s_INT_Inst;
	}

	// mission start (client)
	static void INT_Start()
	{
		INT_VPP_3D.INT_Shutdown();
		INT_VPP_3D m = INT_VPP_3D.INT_Get();
		m.INT_Begin();
	}

	// mission finish: every tree unlinked, subscriptions dropped, instance forgotten
	static void INT_Shutdown()
	{
		INT_VPP_3D m = s_INT_Inst;
		if (m)
			m.INT_End();
		s_INT_Inst = null;
	}

	// §8: re-read prefs / markers (MAP calls it after toggles). Unwanted trees go at once, new ones next frame.
	void INT_Refresh()
	{
		INT_ReadPrefs();
		m_INT_ForceCull = true;
		if (m_INT_Running)
			INT_Cull(false);
	}

	// ------------------------------------------------------------------ extras (TEST / MAP)
	bool INT_IsRunning()
	{
		return m_INT_Running;
	}

	// trees alive right now (markers + beacon)
	int INT_TreeCount()
	{
		int n = 0;
		int cnt = m_INT_Live.Count();
		for (int i = 0; i < cnt; i++)
		{
			VPP3DMarker mk = m_INT_Live.Get(i);
			if (mk && mk.INT_HasTree())
				n++;
		}
		if (m_INT_Beacon && m_INT_Beacon.INT_HasTree())
			n++;
		return n;
	}

	bool INT_HasBeacon()
	{
		if (m_INT_Beacon && m_INT_Beacon.INT_HasTree())
			return true;
		return false;
	}

	static const int INT_SCREEN_DRAW = 0;
	static const int INT_SCREEN_SUSPEND = 1;
	static const int INT_SCREEN_BLOCK = 2;

	// DRAW: nothing in the way (the chat input is fine);
	// SUSPEND: a menu other than the map is on top (inventory, Esc menu, …): 3D trees are kept but hidden, none built;
	// BLOCK: no / dead player or the map menu is open: no 3D trees at all
	static int INT_ScreenState()
	{
		if (!GetGame())
			return INT_SCREEN_BLOCK;
		Man player = GetGame().GetPlayer();
		if (!player)
			return INT_SCREEN_BLOCK;
		if (!player.IsAlive())
			return INT_SCREEN_BLOCK;
		// review #13: nothing over the unconscious (black) screen - 3D markers, route beacon, route ribbon
		PlayerBase pb = PlayerBase.Cast(player);
		if (pb && pb.IsUnconscious())
			return INT_SCREEN_BLOCK;
		UIManager ui = GetGame().GetUIManager();
		if (!ui)
			return INT_SCREEN_BLOCK;
		UIScriptedMenu menu = ui.GetMenu();
		if (!menu)
			return INT_SCREEN_DRAW;
		ChatInputMenu chat = ChatInputMenu.Cast(menu);
		if (chat)
			return INT_SCREEN_DRAW;
		VPPMapMenu mapMenu = VPPMapMenu.Cast(menu);
		if (mapMenu)
			return INT_SCREEN_BLOCK;
		return INT_SCREEN_SUSPEND;
	}

	// the route HUD: any state but DRAW = no HUD trees
	static bool INT_IsScreenBlocked()
	{
		int state = INT_VPP_3D.INT_ScreenState();
		if (state == INT_SCREEN_DRAW)
			return false;
		return true;
	}

	// ------------------------------------------------------------------ per frame (mission OnUpdate)
	void INT_Update(float dt)
	{
		if (!m_INT_Running)
			return;
		int state = INT_VPP_3D.INT_ScreenState();
		bool blocked = false;
		if (state == INT_SCREEN_BLOCK)
			blocked = true;
		bool suspended = false;
		if (state == INT_SCREEN_SUSPEND)
			suspended = true;
		bool tempHidden = g_Game.INT_VPP_Is3DTempHidden();
		if (blocked != m_INT_Blocked || suspended != m_INT_Suspended || tempHidden != m_INT_TempHidden)
		{
			m_INT_Blocked = blocked;
			m_INT_Suspended = suspended;
			m_INT_TempHidden = tempHidden;
			m_INT_ForceCull = true;
		}

		m_INT_CheckAcc += dt;
		if (m_INT_CheckAcc >= INT_CHECK_PERIOD)
		{
			m_INT_CheckAcc = 0;
			INT_Rebind();
			INT_ReadPrefs();
			g_Game.INT_VPP_CheckServerMarkers();
		}

		int rev = g_Game.INT_VPP_3DRevision();
		if (rev != m_INT_SeenRev)
		{
			m_INT_SeenRev = rev;
			m_INT_ForceCull = true;
		}

		m_INT_CullAcc += dt;
		if (m_INT_ForceCull || m_INT_BuildPending || m_INT_CullAcc >= INT_CULL_PERIOD)
		{
			m_INT_CullAcc = 0;
			m_INT_ForceCull = false;
			INT_Cull(true);
		}

		if (m_INT_Blocked || m_INT_TempHidden || !m_INT_MasterOn)
			return;
		if (m_INT_Suspended)
		{
			if (!m_INT_HiddenAll)
			{
				INT_HideAll();
				m_INT_HiddenAll = true;
			}
			return;
		}
		m_INT_HiddenAll = false;
		INT_FramePass();
		m_INT_LabelAcc += dt;
		if (m_INT_LabelAcc >= INT_LABEL_PERIOD)
		{
			m_INT_LabelAcc = 0;
			INT_Declutter();
		}
	}

	// Dense markers in the world (QA 2026-11-12): a label (name + metres) that overlaps the label of a NEARER shown
	// marker hides, its icon stays. 4 times a second over the markers that have a tree (already culled by distance),
	// no allocation; Show() only when a label's state changes.
	protected void INT_Declutter()
	{
		int n = m_INT_Live.Count();
		float pad = INT_UIF_Draw.S(4);
		// the route beacon (diamond + metres) is a fixed obstacle: a label over it hides, the beacon always wins
		bool hasB = false;
		float ox1;
		float oy1;
		float ox2;
		float oy2;
		if (m_INT_Beacon)
			hasB = m_INT_Beacon.INT_ObstacleRect(ox1, oy1, ox2, oy2);
		for (int i = 0; i < n; i++)
		{
			VPP3DMarker a = m_INT_Live.Get(i);
			if (!a || !a.INT_IsShownNow())
				continue;
			// names and metres both off (2026-11-14): no label to hide, none in the way of the others
			if (!a.INT_HasLabel())
				continue;
			float ax1;
			float ay1;
			float ax2;
			float ay2;
			a.INT_LabelRect(ax1, ay1, ax2, ay2);
			float ad = a.INT_LastDist();
			bool keep = true;
			if (hasB)
			{
				if (ax1 < ox2 + pad && ax2 + pad > ox1 && ay1 < oy2 && ay2 > oy1)
					keep = false;
			}
			for (int j = 0; j < n && keep; j++)
			{
				if (j == i)
					continue;
				VPP3DMarker b = m_INT_Live.Get(j);
				if (!b || !b.INT_IsShownNow() || !b.INT_HasLabel())
					continue;
				float bd = b.INT_LastDist();
				if (bd > ad)
					continue;
				if (bd == ad && j > i)
					continue;
				float bx1;
				float by1;
				float bx2;
				float by2;
				b.INT_LabelRect(bx1, by1, bx2, by2);
				if (ax1 < bx2 + pad && ax2 + pad > bx1 && ay1 < by2 && ay2 > by1)
					keep = false;
			}
			a.INT_SetLabelOn(keep);
		}
	}

	// ------------------------------------------------------------------ internals
	protected void INT_Begin()
	{
		m_INT_Running = true;
		m_INT_ForceCull = true;
		m_INT_CullAcc = 0;
		m_INT_CheckAcc = 0;
		m_INT_ScreenW = 0;
		m_INT_ScreenH = 0;
		m_INT_Blocked = true;
		m_INT_Suspended = false;
		m_INT_HiddenAll = false;
		m_INT_TempHidden = false;
		INT_Rebind();
		INT_ReadPrefs();
	}

	protected void INT_End()
	{
		INT_DropAll();
		INT_Unbind();
		m_INT_Running = false;
	}

	protected void INT_Rebind()
	{
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
		if (pr != m_INT_BoundPrefs)
		{
			INT_VPP_Prefs oldPr = m_INT_BoundPrefs;
			if (oldPr && oldPr.m_INT_OnChanged)
				oldPr.m_INT_OnChanged.Remove(this.INT_OnPrefsChanged);
			if (pr.m_INT_OnChanged)
				pr.m_INT_OnChanged.Insert(this.INT_OnPrefsChanged);
			m_INT_BoundPrefs = pr;
		}
		INT_VPP_Route rt = INT_VPP_Route.INT_Get();
		if (rt != m_INT_BoundRoute)
		{
			INT_VPP_Route oldRt = m_INT_BoundRoute;
			if (oldRt && oldRt.m_INT_OnChanged)
				oldRt.m_INT_OnChanged.Remove(this.INT_OnRouteChanged);
			if (rt.m_INT_OnChanged)
				rt.m_INT_OnChanged.Insert(this.INT_OnRouteChanged);
			m_INT_BoundRoute = rt;
		}
	}

	protected void INT_Unbind()
	{
		INT_VPP_Prefs oldPr = m_INT_BoundPrefs;
		if (oldPr && oldPr.m_INT_OnChanged)
			oldPr.m_INT_OnChanged.Remove(this.INT_OnPrefsChanged);
		m_INT_BoundPrefs = null;
		INT_VPP_Route oldRt = m_INT_BoundRoute;
		if (oldRt && oldRt.m_INT_OnChanged)
			oldRt.m_INT_OnChanged.Remove(this.INT_OnRouteChanged);
		m_INT_BoundRoute = null;
	}

	void INT_OnPrefsChanged()
	{
		INT_Refresh();
	}

	void INT_OnRouteChanged()
	{
		m_INT_ForceCull = true;
		if (!m_INT_Running)
			return;
		// route over / cleared: the beacon goes at once
		INT_VPP_Route rt = INT_VPP_Route.INT_Peek();
		bool active = false;
		if (rt)
			active = rt.INT_IsActive();
		if (!active)
			INT_DropBeacon();
	}

	protected void INT_ReadPrefs()
	{
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
		m_INT_MasterOn = pr.INT_Is3DOn();
		for (int i = 0; i < INT_VPP_LAYER_COUNT; i++)
		{
			bool on = pr.INT_IsLayerOn(i);
			m_INT_LayerOn.Set(i, on);
			float d = pr.INT_Dist3D(i);
			m_INT_MaxSq.Set(i, d * d);
		}
		// size / transparency changed (a rare user action): every tree goes, the lazy path builds them again
		int sc = pr.INT_Get3DScalePct();
		int fd = pr.INT_Get3DFadePct();
		// names / metres next to the markers (2026-11-14): the same rebuild as a look change
		int nmOn = 0;
		if (pr.INT_Is3DNamesOn())
			nmOn = 1;
		int dsOn = 0;
		if (pr.INT_Is3DDistOn())
			dsOn = 1;
		if (sc != m_INT_LookScale || fd != m_INT_LookFade || nmOn != m_INT_LookNames || dsOn != m_INT_LookDist)
		{
			bool had = false;
			if (m_INT_LookScale >= 0)
				had = true;
			m_INT_LookScale = sc;
			m_INT_LookFade = fd;
			m_INT_LookNames = nmOn;
			m_INT_LookDist = dsOn;
			if (had)
			{
				m_INT_LookRebuilds++;
				INT_DropAll();
				m_INT_ForceCull = true;
			}
		}
	}

	// TEST: the route beacon's root (null without a tree) / how many look changes rebuilt the trees
	Widget INT_GetBeaconRoot()
	{
		if (!m_INT_Beacon)
			return null;
		return m_INT_Beacon.INT_GetRoot();
	}

	int INT_LookRebuilds()
	{
		return m_INT_LookRebuilds;
	}

	// TEST: the beacon's de-clutter obstacle rect (false = no beacon shown)
	bool INT_BeaconObstacle(out float x1, out float y1, out float x2, out float y2)
	{
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;
		if (!m_INT_Beacon)
			return false;
		bool has = m_INT_Beacon.INT_ObstacleRect(x1, y1, x2, y2);
		return has;
	}

	// allowBuild = false: only drops what is no longer wanted (immediate refresh)
	protected void INT_Cull(bool allowBuild)
	{
		if (!m_INT_Running)
			return;
		if (m_INT_Blocked || m_INT_TempHidden || !m_INT_MasterOn)
		{
			INT_DropAll();
			m_INT_BuildPending = false;
			return;
		}
		Man player = GetGame().GetPlayer();
		if (!player)
		{
			INT_DropAll();
			return;
		}
		int sw;
		int sh;
		GetScreenSize(sw, sh);
		if (sw != m_INT_ScreenW || sh != m_INT_ScreenH)
		{
			// resolution changed: trees were laid out for the old scale
			if (m_INT_ScreenW != 0)
				INT_DropAll();
			m_INT_ScreenW = sw;
			m_INT_ScreenH = sh;
		}
		vector ppos = player.GetPosition();
		// behind another menu: keep / drop, never build (a tree made now would draw over that menu)
		bool build = allowBuild;
		if (m_INT_Suspended)
			build = false;
		m_INT_Pass++;
		m_INT_Budget = 0;
		if (build)
			m_INT_Budget = INT_BUILD_BUDGET;
		m_INT_BuildAllowed = build;
		m_INT_BuildPending = false;

		array<ref VPP3DMarker> clientArr = g_Game.GetClient3dMarkers();
		INT_CullArray(clientArr, false, ppos);
		array<ref VPP3DMarker> serverArr = g_Game.GetServer3dMarkers();
		INT_CullArray(serverArr, true, ppos);
		INT_Sweep();
		INT_CullBeacon(ppos, build);
	}

	protected void INT_CullArray(array<ref VPP3DMarker> arr, bool serverArray, vector ppos)
	{
		if (!arr)
			return;
		int n = arr.Count();
		for (int i = 0; i < n; i++)
		{
			VPP3DMarker mk = arr.Get(i);
			if (!mk)
				continue;
			mk.INT_MarkSeen(m_INT_Pass);
			int layer = INT_LayerOf(mk, serverArray);
			bool want = false;
			if (mk.IsMarkerVisible() && m_INT_LayerOn.Get(layer))
			{
				float maxSq = m_INT_MaxSq.Get(layer);
				if (maxSq > 0)
				{
					vector mpos = mk.INT_GetDrawPos();
					float d2 = INT_VPP_Metres.INT_Dist2DSq(ppos, mpos);
					if (d2 <= maxSq)
						want = true;
				}
			}
			bool has = mk.INT_HasTree();
			if (want && !has)
			{
				if (m_INT_Budget > 0)
				{
					m_INT_Budget--;
					mk.INT_BuildTree(layer);
					if (mk.INT_HasTree() && m_INT_Live.Find(mk) < 0)
						m_INT_Live.Insert(mk);
				}
				else if (m_INT_BuildAllowed)
				{
					// budget used up: go on next frame
					m_INT_BuildPending = true;
				}
			}
			else if (!want && has)
			{
				mk.INT_DropTree();
			}
		}
	}

	// effective layer; untagged markers are resolved once (client array → «Мои»; server array → own =
	// «Сервер», RF / other = «События», unknown = «Сервер»)
	protected int INT_LayerOf(VPP3DMarker mk, bool serverArray)
	{
		int raw = mk.INT_GetRawLayer();
		if (raw >= 0 && raw < INT_VPP_LAYER_COUNT)
			return raw;
		if (raw >= INT_VPP_LAYER_COUNT)
			return INT_VPP_LAYER_SERVER;
		if (mk.INT_IsResolved())
			return INT_VPP_LAYER_SERVER;
		int layer = INT_VPP_LAYER_MINE;
		if (serverArray)
		{
			layer = g_Game.INT_VPP_ResolveServerLayer(mk);
			if (layer < 0)
				layer = INT_VPP_LAYER_SERVER;
		}
		mk.INT_SetLayer(layer);
		return layer;
	}

	// drop the trees of markers that are gone from both arrays (still alive elsewhere) and forget dead entries
	protected void INT_Sweep()
	{
		for (int i = m_INT_Live.Count() - 1; i >= 0; i--)
		{
			VPP3DMarker mk = m_INT_Live.Get(i);
			if (!mk)
			{
				m_INT_Live.Remove(i);
				continue;
			}
			if (!mk.INT_HasTree())
			{
				m_INT_Live.Remove(i);
				continue;
			}
			if (mk.INT_SeenPass() != m_INT_Pass)
			{
				mk.INT_DropTree();
				m_INT_Live.Remove(i);
			}
		}
	}

	protected void INT_CullBeacon(vector ppos, bool allowBuild)
	{
		bool want = false;
		vector target;
		int number = 0;
		INT_VPP_Route rt = INT_VPP_Route.INT_Peek();
		if (rt && rt.INT_IsActive() && m_INT_LayerOn.Get(INT_VPP_LAYER_ROUTE) && g_Game.CanUse3DMarkers())
		{
			float maxSq = m_INT_MaxSq.Get(INT_VPP_LAYER_ROUTE);
			int idx = rt.INT_NextIndex();
			if (idx >= 0 && maxSq > 0)
			{
				INT_VPP_RoutePoint p = rt.INT_At(idx);
				if (p)
				{
					target = p.m_INT_Pos;
					float d2 = INT_VPP_Metres.INT_Dist2DSq(ppos, target);
					if (d2 <= maxSq)
					{
						want = true;
						number = rt.INT_NumberOf(idx);
					}
				}
			}
		}
		if (!want)
		{
			INT_DropBeacon();
			return;
		}
		if (!m_INT_Beacon)
		{
			if (!allowBuild)
				return;
			m_INT_Beacon = new INT_VPP_Beacon();
		}
		if (!allowBuild && !m_INT_Beacon.INT_HasTree())
			return;
		m_INT_Beacon.INT_SetTarget(target, number);
	}

	protected void INT_DropBeacon()
	{
		if (m_INT_Beacon)
			m_INT_Beacon.INT_Drop();
		m_INT_Beacon = null;
	}

	protected void INT_DropAll()
	{
		int n = m_INT_Live.Count();
		for (int i = 0; i < n; i++)
		{
			VPP3DMarker mk = m_INT_Live.Get(i);
			if (mk)
				mk.INT_DropTree();
		}
		m_INT_Live.Clear();
		INT_DropBeacon();
	}

	// behind another menu: hide every tree (once), keep them
	protected void INT_HideAll()
	{
		int n = m_INT_Live.Count();
		for (int i = 0; i < n; i++)
		{
			VPP3DMarker mk = m_INT_Live.Get(i);
			if (mk)
				mk.INT_Hide();
		}
		if (m_INT_Beacon)
			m_INT_Beacon.INT_Hide();
	}

	protected void INT_FramePass()
	{
		Man player = GetGame().GetPlayer();
		if (!player)
			return;
		vector ppos = player.GetPosition();
		float sw = m_INT_ScreenW;
		float sh = m_INT_ScreenH;
		int n = m_INT_Live.Count();
		for (int i = 0; i < n; i++)
		{
			VPP3DMarker mk = m_INT_Live.Get(i);
			if (mk)
				mk.INT_Frame(ppos, sw, sh);
		}
		if (m_INT_Beacon)
			m_INT_Beacon.INT_Frame(ppos, sw, sh);
	}
}

// Mission hooks for the 3D manager AND the route HUD (always super first).
modded class MissionGameplay
{
	override void OnMissionStart()
	{
		super.OnMissionStart();
		if (GetGame().IsDedicatedServer())
			return;
		INT_VPP_Route rt = INT_VPP_Route.INT_Get();
		rt.INT_CheckWorld();
		INT_VPP_3D.INT_Start();
		INT_VPP_Hud.INT_Start();
	}

	override void OnMissionFinish()
	{
		super.OnMissionFinish();
		if (GetGame().IsDedicatedServer())
			return;
		INT_VPP_Hud.INT_Shutdown();
		INT_VPP_3D.INT_Shutdown();
		// the 3D marker objects live on DayZGame (they survive the mission): kill them and their trees here too
		g_Game.SetClient3dMarkers(NULL);
		g_Game.SetServer3dMarkers(NULL);
		g_Game.INT_VPP_EndSession();
		INT_VPP_Route.INT_Shutdown();
		INT_VPP_Prefs.INT_Shutdown();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		INT_VPP_3D m3d = INT_VPP_3D.INT_Peek();
		if (m3d)
			m3d.INT_Update(timeslice);
		INT_VPP_Hud hud = INT_VPP_Hud.INT_Peek();
		if (hud)
			hud.INT_Update(timeslice);
	}
}
