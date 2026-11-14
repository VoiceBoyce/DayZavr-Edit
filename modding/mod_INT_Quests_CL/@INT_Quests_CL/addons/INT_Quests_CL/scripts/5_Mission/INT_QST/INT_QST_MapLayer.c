// INT Quests (client) — where to go, on the VPP map and on RF Map (owner 2026-10-22: «если карта не стоит — никуда не
// выводит, если стоит — отображает на ней цель, куда надо прийти»; RF Map for server 21). Soft, with no dependency
// (the INT_PvEZ way, measured there on 1.30.164200): every frame the open menu is checked; when it is
//   VPP map  — class VPPMapMenu,       MapWidget "Map_Widget" (priority 100)
//   RF Map   — class RF_MAP_CL_MapHud, MapWidget "mapWidget"  (priority 100; RF's own marks 450/500 stay above us;
//              the menu is hidden and shown again, not destroyed: the layer is rebuilt on every show)
// our own layer goes INSIDE that map widget — a canvas with the target circles plus a pin and the quest name, kept
// inside the map rect (neither map clips its children). Nothing of either map is modded or written: VPP saves its
// client markers to the player's MapMarkersCache.json and both clear their user marks, so neither path is used.
// A server without these maps never sees the class names and needs nothing.
// Points: every unfinished «исследовать» (EXPLORE) target of the player's taken quests (not expired).

class INT_QST_MapPoint
{
	vector m_At;
	float m_Radius;
	string m_Name;
}

class INT_QST_MapLayer
{
	static const float REDRAW_GAP = 0.05;
	// EVERY widget of the layer gets it. VPP: Map_Widget has priority 100 -> 300. RF Map: its zone canvas (450) is drawn
	// over children below it (our pins faded under the «Tier» zones, stand 2026-10-22), its own marks are 500 and its
	// panels 1000 -> 460.
	static const int LAYER_SORT = 300;
	static const int LAYER_SORT_RF = 460;
	protected static int s_Sort = 300;
	static const int RING = 0xFFF2C14E;
	static const int RING_SOFT = 0x66F2C14E;
	static const int LABEL = 0xFFF2EBDA;

	protected static MapWidget s_Map;			// weak: null once the menu's widgets are gone
	protected static Widget s_Host;
	protected static CanvasWidget s_Canvas;
	protected static Widget s_Labels;
	protected static float s_LastScale = -1;
	protected static vector s_LastPos;
	protected static float s_LX;
	protected static float s_LY;
	protected static float s_LW;
	protected static float s_LH;
	protected static bool s_Dirty;
	protected static bool s_ViewDirty;
	protected static float s_Cooldown;
	protected static bool s_Hooked;
	protected static ref array<ref INT_QST_MapPoint> s_Points = new array<ref INT_QST_MapPoint>();

	static int s_Redraws;		// self-test counters
	static int s_Drawn;
	static string s_Kind;		// "vpp" / "rf" (the last map found)

	static void Init()
	{
		if (s_Hooked || !INT_QST_Client.s_OnChanged)
			return;
		INT_QST_Client.s_OnChanged.Insert(INT_QST_MapLayer.MarkDirty);
		s_Hooked = true;
	}

	static void Shutdown()
	{
		if (s_Hooked && INT_QST_Client.s_OnChanged)
			INT_QST_Client.s_OnChanged.Remove(INT_QST_MapLayer.MarkDirty);
		s_Hooked = false;
		Detach();
	}

	static void MarkDirty()
	{
		s_Dirty = true;
	}

	static bool IsAttached()
	{
		return s_Map != null && s_Host != null;
	}

	protected static int s_HasMapMod = -1;

	// VPP map or RF Map loaded in this game? (their CfgPatches; the journal shows the «На карте» switch only then)
	static bool HasMapMod()
	{
		if (s_HasMapMod < 0)
		{
			s_HasMapMod = 0;
			if (GetGame().ConfigIsExisting("CfgPatches VanillaPlusPlus") || GetGame().ConfigIsExisting("CfgPatches RF_MAP_CL"))
				s_HasMapMod = 1;
		}
		return s_HasMapMod == 1;
	}

	// The quest has a point the map would show now (an unfinished EXPLORE target with a position)?
	static bool HasOpenPoint(INT_QST_QuestDef q)
	{
		if (!q)
			return false;
		int n = q.Targets.Count();
		for (int i = 0; i < n; i++)
		{
			INT_QST_TargetDef t = q.Targets[i];
			if (t.m_Type != INT_QST_TT.EXPLORE || !t.m_HasPos)
				continue;
			int have = INT_QST_Client.TargetValue(q, i);
			if (have < t.m_Need)
				return true;
		}
		return false;
	}

	// Every frame on the client (one GetMenu + a class name compare when no map is open).
	static void Tick(float dt)
	{
		MapWidget open = FindMap();
		if (open != s_Map)
		{
			Detach();
			if (open)
				Attach(open);
		}
		if (!s_Map || !s_Canvas)
			return;
		if (!s_Map.IsVisibleHierarchy())
			return;		// VPP hides its map while its marker dialog is open
		if (ViewChanged())
			s_ViewDirty = true;
		if (s_Cooldown > 0)
			s_Cooldown = s_Cooldown - dt;
		if ((s_Dirty || s_ViewDirty) && s_Cooldown <= 0)
		{
			Redraw();
			s_Cooldown = REDRAW_GAP;
		}
	}

	protected static MapWidget FindMap()
	{
		if (!GetGame())
			return null;
		UIManager um = GetGame().GetUIManager();
		if (!um)
			return null;
		UIScriptedMenu menu = um.GetMenu();
		if (!menu)
			return null;
		string cls = menu.ClassName();
		string widgetName = "";
		if (cls == "VPPMapMenu")
		{
			widgetName = "Map_Widget";
			s_Kind = "vpp";
			s_Sort = LAYER_SORT;
		}
		else if (cls == "RF_MAP_CL_MapHud")
		{
			widgetName = "mapWidget";
			s_Kind = "rf";
			s_Sort = LAYER_SORT_RF;
		}
		else
			return null;
		Widget root = menu.GetLayoutRoot();
		if (!root)
			return null;
		return MapWidget.Cast(root.FindAnyWidget(widgetName));
	}

	protected static void Attach(MapWidget mapWidget)
	{
		if (!GetGame() || !GetGame().GetWorkspace())
			return;
		s_Map = mapWidget;
		s_Host = INT_UIF_Draw.Frame(mapWidget, 0, 0, 10, 10, "INT_QST_MapHost");
		if (!s_Host)
		{
			s_Map = null;
			return;
		}
		s_Canvas = CanvasWidget.Cast(GetGame().GetWorkspace().CreateWidgets("INT_Quests_CL/gui/layouts/int_qst_canvas.layout", s_Host));
		s_Labels = INT_UIF_Draw.Frame(s_Host, 0, 0, 10, 10, "INT_QST_MapLabels");
		SortAll(s_Host);
		s_LastScale = -1;
		s_Dirty = true;
		INT_QST_Log.Dbg("метки заданий на карте: " + s_Kind);
	}

	static void Detach()
	{
		if (s_Host)
			s_Host.Unlink();
		s_Host = null;
		s_Canvas = null;
		s_Labels = null;
		s_Map = null;
	}

	protected static bool ViewChanged()
	{
		float scale = s_Map.GetScale();
		vector pos = s_Map.GetMapPos();
		float x;
		float y;
		float w;
		float h;
		s_Map.GetScreenPos(x, y);
		s_Map.GetScreenSize(w, h);
		// with tolerances: the VPP map moves by float noise every frame (an exact compare redrew ~10 times a second)
		bool same = Math.AbsFloat(scale - s_LastScale) < 0.00001;
		if (same && vector.DistanceSq(pos, s_LastPos) > 0.0001)
			same = false;
		if (same && (Math.AbsFloat(x - s_LX) > 0.5 || Math.AbsFloat(y - s_LY) > 0.5))
			same = false;
		if (same && (Math.AbsFloat(w - s_LW) > 0.5 || Math.AbsFloat(h - s_LH) > 0.5))
			same = false;
		if (same)
			return false;
		s_LastScale = scale;
		s_LastPos = pos;
		s_LX = x;
		s_LY = y;
		s_LW = w;
		s_LH = h;
		return true;
	}

	protected static void SortAll(Widget w)
	{
		if (!w)
			return;
		w.SetSort(s_Sort);
		Widget c = w.GetChildren();
		while (c)
		{
			SortAll(c);
			c = c.GetSibling();
		}
	}

	protected static void ClearLabels()
	{
		if (!s_Labels)
			return;
		Widget c = s_Labels.GetChildren();
		while (c)
		{
			Widget next = c.GetSibling();
			c.Unlink();
			c = next;
		}
	}

	// Unfinished EXPLORE targets of the taken quests.
	static void Collect(array<ref INT_QST_MapPoint> outPoints)
	{
		outPoints.Clear();
		if (!INT_QST_Client.s_State || !INT_QST_Client.s_Catalog)
			return;
		foreach (INT_QST_Active a : INT_QST_Client.s_State.Active)
		{
			INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(a.Id);
			if (!q || !INT_QST_Client.IsOnMap(a.Id))
				continue;
			int wait;
			int st = INT_QST_Client.StatusOf(q, wait);
			if (st == INT_QST_St.EXPIRED)
				continue;
			int n = q.Targets.Count();
			for (int i = 0; i < n; i++)
			{
				INT_QST_TargetDef t = q.Targets[i];
				if (t.m_Type != INT_QST_TT.EXPLORE || !t.m_HasPos)
					continue;
				int have = INT_QST_Client.TargetValue(q, i);
				if (have >= t.m_Need)
					continue;
				INT_QST_MapPoint p = new INT_QST_MapPoint();
				p.m_At = t.m_Pos;
				p.m_Radius = t.Radius;
				p.m_Name = q.Name;
				outPoints.Insert(p);
			}
		}
	}

	protected static void Redraw()
	{
		s_Dirty = false;
		s_ViewDirty = false;
		s_Redraws++;
		s_Drawn = 0;
		float mx;
		float my;
		float mw;
		float mh;
		s_Map.GetScreenPos(mx, my);
		s_Map.GetScreenSize(mw, mh);
		s_Host.SetPos(0, 0);
		s_Host.SetSize(mw, mh);
		s_Canvas.SetPos(0, 0);
		s_Canvas.SetSize(mw, mh);
		s_Labels.SetPos(0, 0);
		s_Labels.SetSize(mw, mh);
		s_Canvas.Clear();
		ClearLabels();
		Collect(s_Points);
		foreach (INT_QST_MapPoint p : s_Points)
		{
			vector cw = Vector(p.m_At[0], 0, p.m_At[2]);
			vector ew = Vector(p.m_At[0] + p.m_Radius, 0, p.m_At[2]);
			vector c = s_Map.MapToScreen(cw);
			vector e = s_Map.MapToScreen(ew);
			float cx = c[0] - mx;
			float cy = c[1] - my;
			float rPx = Math.AbsFloat(e[0] - c[0]);
			if (cx + rPx < 0 || cx - rPx > mw || cy + rPx < 0 || cy - rPx > mh)
				continue;
			if (rPx >= 6)
			{
				DrawCircle(cx, cy, rPx, mw, mh, 2, RING);
				if (rPx > 10)
					DrawCircle(cx, cy, rPx - 3, mw, mh, 1, RING_SOFT);
			}
			if (cx >= 0 && cx <= mw && cy >= 0 && cy <= mh)
			{
				Pin(cx, cy, p.m_Name, mw, mh);
				s_Drawn++;
			}
		}
	}

	// Circle by segments; a segment is drawn only when both ends lie inside the map rect (VPP does not clip children).
	protected static void DrawCircle(float cx, float cy, float r, float mw, float mh, float width, int color)
	{
		int segs = 72;
		if (r < 30)
			segs = 28;
		else if (r > 400)
			segs = 144;
		float step = Math.PI2 / segs;
		float px = cx + r;
		float py = cy;
		for (int i = 1; i <= segs; i++)
		{
			float ang = step * i;
			float nx = cx + r * Math.Cos(ang);
			float ny = cy + r * Math.Sin(ang);
			bool in0 = px >= 0 && py >= 0 && px <= mw && py <= mh;
			bool in1 = nx >= 0 && ny >= 0 && nx <= mw && ny <= mh;
			if (in0 && in1)
				s_Canvas.DrawLine(px, py, nx, ny, width, color);
			px = nx;
			py = ny;
		}
	}

	// A pin standing on the point and the quest name under it (map-local screen px; the holder's children in design px).
	protected static void Pin(float x, float y, string name, float mw, float mh)
	{
		if (!s_Labels)
			return;
		string txt = INT_QST_Look.Fit(name, 300, 15);
		float tw = INT_UIF_Draw.TextWidth(txt, 15) * 1.08 + 8;
		if (tw < 40)
			tw = 40;
		Widget holder = INT_UIF_Draw.Frame(s_Labels, 0, 0, tw, 56, "INT_QST_MapPin");
		if (!holder)
			return;
		float ix = (tw - 34) * 0.5;
		INT_QST_Look.Icon(holder, ix + 2, 2, 34, "map_pin", 0xD9000000);
		INT_QST_Look.Icon(holder, ix, 0, 34, "map_pin", RING);
		// a dark plate under the name: readable on light maps (RF) and on satellite tiles (VPP)
		INT_UIF_Surface.Box(holder, 0, 34, tw, 22, 4, 0xA6000000, 0);
		TextWidget t = INT_QST_Look.Text(holder, 0, 34, tw, 22, txt, 15, LABEL, INT_UIF_Align.CENTER);
		if (t)
			t.SetShadow(2, 0xFF000000, 0.9, 1, 1);
		// call results hoisted: two of them in one argument list get clobbered (measured trap)
		float wpx = INT_UIF_Draw.S(tw);
		float hpx = INT_UIF_Draw.S(56);
		float pinH = INT_UIF_Draw.S(34);
		float maxX = Math.Max(2, mw - wpx - 2);
		float maxY = Math.Max(2, mh - hpx - 2);
		float lx = Math.Clamp(x - wpx * 0.5, 2, maxX);
		float ly = Math.Clamp(y - pinH, 2, maxY);
		holder.SetPos(lx, ly);
		SortAll(holder);
	}

	// Self-test.
	static string TestState()
	{
		array<ref INT_QST_MapPoint> pts = new array<ref INT_QST_MapPoint>();
		Collect(pts);
		string att = "no";
		if (IsAttached())
			att = "yes";
		string np = pts.Count().ToString();
		string nr = s_Redraws.ToString();
		string nd = s_Drawn.ToString();
		return "attached " + att + " map " + s_Kind + " points " + np + " redraws " + nr + " drawn " + nd;
	}
}
