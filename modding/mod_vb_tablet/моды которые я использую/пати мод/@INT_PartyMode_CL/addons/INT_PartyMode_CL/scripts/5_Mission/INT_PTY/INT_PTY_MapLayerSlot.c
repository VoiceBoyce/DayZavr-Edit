// INT PartyMode (client) — one target map of the layer (INT_PTY_MapLayer): a big map of an open menu (vanilla / VPP /
// RF Map) or a HUD minimap (RF Map's built-in minimap / @MiniMap). Our host frame lives INSIDE the foreign MapWidget
// (no modded class, no foreign type); both references are weak and turn null when the owner destroys its widgets
// (MiniMap deletes its HUD on toggle) -> the slot unbinds itself and the layer finds the map again.
// Rules (measured in INT_PvEZ / INT_Quests / INT_PDA on 1.30.164200):
//   - a MapWidget draws over every child whose own sort is below 100 -> every widget of the layer gets the slot's sort
//     (300; RF Map 460: above its zone canvas 450, below its own marks 500), set the INT_PDA way (children listed
//     first, then sorted: SetSort moves a widget to the end of its siblings);
//   - MapToScreen gives absolute screen px: map-local = screen - the map's GetScreenPos;
//   - the VPP map drifts by float noise every frame -> the view compare has tolerances;
//   - every widget of the layer ignores the pointer (UIF primitives): VPP reads the widget under the cursor, RF drags.
// Pins come from a pool: created only when a data change needs more, never per frame. Per frame only existing pins
// move, and only when the view or the positions changed. Minimaps cull by the world rect of the view first, so only
// the pins inside it are projected.

class INT_PTY_MapLayerSlot
{
	MapWidget m_Map;			// weak: null once the owner's widgets are gone
	Widget m_Host;				// weak: our frame inside m_Map
	int m_Kind;					// INT_PTY_MapLayer.MAP_*
	int m_Sort = 300;
	bool m_Compact;				// HUD minimap: small marks, culling, paused while any menu is open
	static const float TOUCH_PX = 30;	// design px: a ping this close to its owner's mark drops its label
	string m_Tag;				// "menu" / "rfmini" / "minimap" (logs, self-test)

	protected ref array<ref INT_PTY_MapLayerPin> m_Pins;
	protected int m_Used;
	protected int m_ContentVer = -1;
	protected int m_PosVer = -1;
	protected bool m_HostShown = true;
	protected float m_Factor;
	protected float m_K = 1;
	protected float m_LastScale = -1;
	protected vector m_LastPos;
	protected float m_MX;
	protected float m_MY;
	protected float m_MW;
	protected float m_MH;
	protected bool m_Bound;		// M-2: Bind ran and Unbind did not (both weak refs may turn null together)

	// self-test counters
	int m_Binds;
	int m_Places;
	int m_Visible;

	void INT_PTY_MapLayerSlot(string tag, bool compact)
	{
		m_Tag = tag;
		m_Compact = compact;
		m_Pins = new array<ref INT_PTY_MapLayerPin>();
	}

	bool IsBound()
	{
		if (!m_Map || !m_Host)
			return false;
		return true;
	}

	// Bound, drawn by the owner and allowed by the layer style.
	bool IsLive()
	{
		if (!IsBound())
			return false;
		if (!m_Map.IsVisibleHierarchy())
			return false;
		return true;
	}

	int PinCount()
	{
		return m_Pins.Count();
	}

	// Follows the map found this frame (null = none): binds to a new one, unbinds when it is gone.
	void Track(MapWidget found, int kind, int sort)
	{
		if (found && found == m_Map && m_Host)
			return;
		if (!found)
		{
			if (m_Bound || m_Map || m_Host)
				Unbind();
			return;
		}
		Bind(found, kind, sort);
	}

	void Bind(MapWidget target, int kind, int sort)
	{
		Unbind();
		if (!target || !GetGame() || !GetGame().GetWorkspace())
			return;
		Widget host = INT_UIF_Draw.Frame(target, 0, 0, 10, 10, "INT_PTY_MapHost");
		if (!host)
			return;
		m_Map = target;
		m_Host = host;
		m_Kind = kind;
		m_Sort = sort;
		m_Factor = INT_UIF_Scale.Factor();
		m_K = INT_PTY_MapLayer.StyleScale();
		m_HostShown = true;
		m_Used = 0;
		m_ContentVer = -1;
		m_PosVer = -1;
		m_LastScale = -1;
		// M-1: a new host starts 10x10 — the first view compare must size it even for a map of the same size
		m_MX = -1;
		m_MY = -1;
		m_MW = -1;
		m_MH = -1;
		m_Bound = true;
		SortAll(m_Host, m_Sort);
		m_Binds++;
		INT_PTY_MapLayer.MarkData();
		string kn = INT_PTY_MapLayer.KindName(kind);
		INT_PTY_Log.Dbg("друзья на карте: " + kn);
	}

	void Unbind()
	{
		if (m_Host)
			m_Host.Unlink();
		m_Host = null;
		m_Map = null;
		m_Bound = false;
		m_Pins.Clear();
		m_Used = 0;
		m_Visible = 0;
		m_ContentVer = -1;
		m_PosVer = -1;
	}

	// Same map, pins built again (resolution or layer scale changed).
	void Rebuild()
	{
		if (!IsBound())
			return;
		MapWidget target = m_Map;
		int kind = m_Kind;
		int sort = m_Sort;
		Bind(target, kind, sort);
	}

	// Every frame. allowed = false: the layer is hidden by its style, or a HUD minimap while a menu is open (our pins
	// at sort 300 would otherwise draw over the inventory's 3D previews).
	void Update(bool allowed)
	{
		if (!m_Bound)
			return;
		// M-2: the owner destroyed its map (and with it our host): unbind even when both refs are null now
		if (!m_Map || !m_Host)
		{
			Unbind();
			return;
		}
		if (!allowed)
		{
			ShowHost(false);
			return;
		}
		ShowHost(true);
		if (!m_Map.IsVisibleHierarchy())
			return;		// VPP hides its map under its marker dialog; RF hides its minimap under menus
		bool view = ViewChanged();
		if (view)
		{
			float f = INT_UIF_Scale.Factor();
			if (Math.AbsFloat(f - m_Factor) > 0.001)
			{
				Rebuild();
				if (!IsBound())
					return;
				ViewChanged();
			}
		}
		bool force = false;
		int cv = INT_PTY_MapLayer.ContentVer();
		if (cv != m_ContentVer)
		{
			ApplyContent();
			m_ContentVer = cv;
			force = true;
		}
		int pv = INT_PTY_MapLayer.PosVer();
		if (view || force || pv != m_PosVer)
		{
			Place();
			m_PosVer = pv;
		}
	}

	protected void ShowHost(bool show)
	{
		if (show == m_HostShown || !m_Host)
			return;
		m_Host.Show(show);
		m_HostShown = show;
	}

	protected bool ViewChanged()
	{
		float scale = m_Map.GetScale();
		vector center = m_Map.GetMapPos();
		float x;
		float y;
		float w;
		float h;
		m_Map.GetScreenPos(x, y);
		m_Map.GetScreenSize(w, h);
		bool same = Math.AbsFloat(scale - m_LastScale) < 0.00001;
		if (same && vector.DistanceSq(center, m_LastPos) > 0.0001)
			same = false;
		if (same && (Math.AbsFloat(x - m_MX) > 0.5 || Math.AbsFloat(y - m_MY) > 0.5))
			same = false;
		if (same && (Math.AbsFloat(w - m_MW) > 0.5 || Math.AbsFloat(h - m_MH) > 0.5))
			same = false;
		if (same)
			return false;
		bool resized = Math.AbsFloat(w - m_MW) > 0.5 || Math.AbsFloat(h - m_MH) > 0.5;
		m_LastScale = scale;
		m_LastPos = center;
		m_MX = x;
		m_MY = y;
		m_MW = w;
		m_MH = h;
		if (resized && m_Host)
		{
			m_Host.SetPos(0, 0);
			m_Host.SetSize(w, h);
		}
		return true;
	}

	// The marks of the layer onto the pool (texts / textures / colours only where they changed).
	protected void ApplyContent()
	{
		int n = INT_PTY_MapLayer.MarkCount();
		float alpha = INT_PTY_MapLayer.StyleAlpha();
		int accent = INT_PTY_MapLayer.StyleAccent();
		int i;
		INT_PTY_MapLayerPin pin;
		INT_PTY_MapLayerMark mk;
		for (i = 0; i < n; i++)
		{
			if (i >= m_Pins.Count())
			{
				INT_PTY_MapLayerPin fresh = new INT_PTY_MapLayerPin();
				if (!fresh.Build(m_Host, m_Compact, m_K))
					break;
				SortAll(fresh.m_Root, m_Sort);
				m_Pins.Insert(fresh);
			}
			pin = m_Pins[i];
			mk = INT_PTY_MapLayer.MarkAt(i);
			pin.SetContent(mk, alpha, accent);
		}
		m_Used = n;
		if (m_Used > m_Pins.Count())
			m_Used = m_Pins.Count();
		for (i = m_Used; i < m_Pins.Count(); i++)
		{
			pin = m_Pins[i];
			pin.Hide();
		}
	}

	// Pins to the current view (map-local px); minimaps project only what lies inside the view's world rect.
	protected void Place()
	{
		m_Places++;
		float edge = INT_UIF_Draw.S(3);
		bool cull = m_Compact;
		float minX = 0;
		float maxX = 0;
		float minZ = 0;
		float maxZ = 0;
		if (cull)
		{
			float rx = m_MX + m_MW;
			float ry = m_MY + m_MH;
			vector tl = Vector(m_MX, m_MY, 0);
			vector br = Vector(rx, ry, 0);
			vector a = m_Map.ScreenToMap(tl);
			vector b = m_Map.ScreenToMap(br);
			float ax0 = a[0];
			float bx0 = b[0];
			float az0 = a[2];
			float bz0 = b[2];
			minX = Math.Min(ax0, bx0);
			maxX = Math.Max(ax0, bx0);
			minZ = Math.Min(az0, bz0);
			maxZ = Math.Max(az0, bz0);
			float pad = (maxX - minX) * 0.03;
			minX = minX - pad;
			maxX = maxX + pad;
			minZ = minZ - pad;
			maxZ = maxZ + pad;
		}
		int vis = 0;
		int i;
		INT_PTY_MapLayerPin pin;
		INT_PTY_MapLayerMark mk;
		vector at;
		vector sp;
		float ax;
		float az;
		float lx;
		float ly;
		for (i = 0; i < m_Used; i++)
		{
			pin = m_Pins[i];
			mk = INT_PTY_MapLayer.MarkAt(i);
			if (!mk)
			{
				pin.Hide();
				continue;
			}
			at = mk.m_At;
			if (cull)
			{
				ax = at[0];
				az = at[2];
				if (ax < minX || ax > maxX || az < minZ || az > maxZ)
				{
					pin.Hide();
					continue;
				}
			}
			sp = m_Map.MapToScreen(at);
			lx = sp[0] - m_MX;
			ly = sp[1] - m_MY;
			pin.Place(lx, ly, m_MW, m_MH, edge);
			if (pin.IsShown())
				vis++;
		}
		m_Visible = vis;
		SuppressTouchingLabels();
	}

	// A ping that touches its owner's own mark on screen loses its label: the owner's name already stands there
	// (otherwise the two names print on top of each other — a friend who pinged where he stands, stand 2026-10-23).
	protected void SuppressTouchingLabels()
	{
		float touch = INT_UIF_Draw.S(TOUCH_PX);
		float touchSq = touch * touch;
		INT_PTY_MapLayerPin pin;
		INT_PTY_MapLayerPin owner;
		INT_PTY_MapLayerMark mk;
		bool hide;
		float dx;
		float dy;
		for (int i = 0; i < m_Used; i++)
		{
			mk = INT_PTY_MapLayer.MarkAt(i);
			if (!mk || mk.m_Kind != INT_PTY_MapLayer.MK_PING)
				continue;
			pin = m_Pins[i];
			hide = false;
			if (pin.IsShown() && mk.m_Near >= 0 && mk.m_Near < m_Used)
			{
				owner = m_Pins[mk.m_Near];
				if (owner.IsShown())
				{
					dx = pin.m_LX - owner.m_LX;
					dy = pin.m_LY - owner.m_LY;
					if (dx * dx + dy * dy < touchSq)
						hide = true;
				}
			}
			pin.SetLabelSuppressed(hide);
		}
	}

	// Every widget of a subtree gets the sort. SetSort moves the widget to the end of its siblings (measured on the
	// stand): walking GetSibling() while sorting stopped after the first child. The children are listed first, then
	// sorted in creation order, which keeps that order (INT_PDA_PageMap.SortAll).
	static void SortAll(Widget w, int sort)
	{
		if (!w)
			return;
		w.SetSort(sort);
		array<Widget> kids = new array<Widget>();
		Widget c = w.GetChildren();
		while (c)
		{
			kids.Insert(c);
			c = c.GetSibling();
		}
		for (int i = 0; i < kids.Count(); i++)
		{
			SortAll(kids[i], sort);
		}
	}

	// --- stand probes ------------------------------------------------------------------------------------------------
	string TestPins()
	{
		if (!IsBound())
			return "-";
		string r = "";
		INT_PTY_MapLayerPin pin;
		for (int i = 0; i < m_Used; i++)
		{
			pin = m_Pins[i];
			if (!pin.IsShown())
				continue;
			string ln = pin.TestLine();
			r = r + ln + "; ";
		}
		return r;
	}

	int TestCheck(out int shown, out float worst)
	{
		shown = 0;
		worst = 0;
		if (!IsBound() || !m_Map.IsVisibleHierarchy())
			return -1;
		int bad = 0;
		float edge = INT_UIF_Draw.S(3);
		INT_PTY_MapLayerPin pin;
		INT_PTY_MapLayerMark mk;
		vector sp;
		float sx;
		float sy;
		float px;
		float py;
		float d;
		bool inside;
		for (int i = 0; i < m_Used; i++)
		{
			pin = m_Pins[i];
			mk = INT_PTY_MapLayer.MarkAt(i);
			if (!mk)
				continue;
			sp = m_Map.MapToScreen(mk.m_At);
			sx = sp[0];
			sy = sp[1];
			inside = true;
			if (sx - m_MX < edge || sy - m_MY < edge || sx - m_MX > m_MW - edge || sy - m_MY > m_MH - edge)
				inside = false;
			if (inside != pin.IsShown())
			{
				bad++;
				continue;
			}
			if (!inside)
				continue;
			pin.m_Root.GetScreenPos(px, py);
			d = Math.AbsFloat(px - sx) + Math.AbsFloat(py - sy);
			shown++;
			if (d > worst)
				worst = d;
			if (d > 3)
				bad++;
		}
		return bad;
	}

	// "menu vpp pins 4/12 places 31" (self-test).
	string Describe()
	{
		string kn = INT_PTY_MapLayer.KindName(m_Kind);
		if (!IsBound())
			kn = "-";
		string vis = m_Visible.ToString();
		string used = m_Used.ToString();
		string pl = m_Places.ToString();
		string bi = m_Binds.ToString();
		return m_Tag + " " + kn + " pins " + vis + "/" + used + " places " + pl + " binds " + bi;
	}
}
