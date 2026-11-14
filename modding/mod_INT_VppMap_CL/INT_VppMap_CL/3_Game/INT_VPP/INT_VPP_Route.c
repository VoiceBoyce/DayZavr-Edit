// ============================================================================
// VanillaPPMap «Экспедиция» B — personal route (HUD3D, BUILD_PLAN §2 / §8).
// Model + persistence: points are saved in $profile:INT_Mods/INT_VppMap/Client.json (section INT_Route*, see
// INT_VPP_Prefs.c) after EVERY change. Reach radius / max points come from the server settings
// (INT_VPP_Client, defaults 15 m / 20 until the server answered).
// Reached = 2D (x/z) distance from the player to the NEXT point ≤ INT_ReachRadius(), tested in INT_Tick (the
// HUD calls it every 0.25 s, also while the map is open):
//   «убирать пройденные» on (default): the point is removed, the next one becomes point 0;
//   off: the point stays (drawn as passed), INT_NextIndex() moves on.
// The last point reached → the route is cleared (Count() = 0). Display numbers continue across removals:
// INT_NumberOf(i) = points already removed by reaching + i + 1 («Точка 2 пройдена · дальше 03»).
// Events: m_INT_OnChanged.Invoke() after any change; m_INT_OnReached.Invoke(int number1Based, string name)
// when a point is reached (before the OnChanged of the same change; when it was the last point the route is
// already empty).
// 3_Game, no client-only calls at file scope.
// ============================================================================

class INT_VPP_RoutePoint : Managed
{
	vector m_INT_Pos;
	string m_INT_Name;
}

// metres for HUD / beacon texts: the KEY changes only when the shown text changes
// (below 1 km rounded to 5 m like the design, above in 0.1 km)
class INT_VPP_Metres
{
	static const string INT_TXT_M = "м";
	static const string INT_TXT_KM = "км";
	static const int INT_KM_KEY = 100000;

	static int INT_MetresKey(float m)
	{
		float v = m;
		if (v != v || v < 0)
			v = 0;
		if (v < 997.5)
		{
			int r = Math.Round(v / 5.0);
			return r * 5;
		}
		int k = Math.Round(v / 100.0);
		return INT_KM_KEY + k;
	}

	static string INT_MetresText(int key)
	{
		if (key < INT_KM_KEY)
			return key.ToString() + " " + INT_TXT_M;
		int k = key - INT_KM_KEY;
		int whole = k / 10;
		int frac = k - whole * 10;
		return whole.ToString() + "," + frac.ToString() + " " + INT_TXT_KM;
	}

	static string INT_Metres(float m)
	{
		int key = INT_VPP_Metres.INT_MetresKey(m);
		return INT_VPP_Metres.INT_MetresText(key);
	}

	static float INT_Dist2D(vector a, vector b)
	{
		float dx = b[0] - a[0];
		float dz = b[2] - a[2];
		return Math.Sqrt(dx * dx + dz * dz);
	}

	static float INT_Dist2DSq(vector a, vector b)
	{
		float dx = b[0] - a[0];
		float dz = b[2] - a[2];
		return dx * dx + dz * dz;
	}
}

class INT_VPP_Route : Managed
{
	protected static ref INT_VPP_Route s_INT_Inst;

	ref ScriptInvoker m_INT_OnChanged;
	ref ScriptInvoker m_INT_OnReached;

	protected ref array<ref INT_VPP_RoutePoint> m_INT_Points;
	protected bool m_INT_AutoRemove;
	protected int m_INT_Base;
	protected int m_INT_Next;
	protected string m_INT_World;

	void INT_VPP_Route()
	{
		m_INT_OnChanged = new ScriptInvoker();
		m_INT_OnReached = new ScriptInvoker();
		m_INT_Points = new array<ref INT_VPP_RoutePoint>();
		m_INT_AutoRemove = true;
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		INT_ReadFile(prefs.INT_File());
	}

	// ------------------------------------------------------------------ singleton
	static INT_VPP_Route INT_Get()
	{
		if (!s_INT_Inst)
			s_INT_Inst = new INT_VPP_Route();
		return s_INT_Inst;
	}

	// no creation
	static INT_VPP_Route INT_Peek()
	{
		return s_INT_Inst;
	}

	// mission finish (the state is already on disk: every change saves)
	static void INT_Shutdown()
	{
		INT_VPP_Route inst = s_INT_Inst;
		if (inst)
		{
			if (inst.m_INT_OnChanged)
				inst.m_INT_OnChanged.Clear();
			if (inst.m_INT_OnReached)
				inst.m_INT_OnReached.Clear();
		}
		s_INT_Inst = null;
	}

	// ------------------------------------------------------------------ §8 API
	int INT_Count()
	{
		return m_INT_Points.Count();
	}

	INT_VPP_RoutePoint INT_At(int i)
	{
		if (i < 0 || i >= m_INT_Points.Count())
			return null;
		return m_INT_Points.Get(i);
	}

	// false when INT_MaxPoints() is reached (or the position is not a number)
	bool INT_Add(vector pos, string name)
	{
		if (pos[0] != pos[0] || pos[1] != pos[1] || pos[2] != pos[2])
			return false;
		int n = m_INT_Points.Count();
		if (n >= INT_MaxPoints())
			return false;
		if (n == 0)
		{
			m_INT_Base = 0;
			m_INT_Next = 0;
		}
		INT_StampWorld();
		string clean = INT_VPP_Validate.CleanText(name, INT_VPP_Net.NAME_MAX_BYTES);
		INT_VPP_RoutePoint p = new INT_VPP_RoutePoint();
		p.m_INT_Pos = pos;
		p.m_INT_Name = clean;
		m_INT_Points.Insert(p);
		INT_SaveAndNotify();
		return true;
	}

	void INT_RemoveAt(int i)
	{
		int n = m_INT_Points.Count();
		if (i < 0 || i >= n)
			return;
		m_INT_Points.RemoveOrdered(i);
		if (i < m_INT_Next)
			m_INT_Next = m_INT_Next - 1;
		n = m_INT_Points.Count();
		// nothing left to walk to (only passed points remain) → the route is over
		if (n == 0 || m_INT_Next >= n)
			INT_ResetPoints();
		INT_SaveAndNotify();
	}

	void INT_Clear()
	{
		if (m_INT_Points.Count() == 0 && m_INT_Base == 0 && m_INT_Next == 0)
			return;
		INT_ResetPoints();
		INT_SaveAndNotify();
	}

	// Bulk set (review #8): the whole route at once - one save, one OnChanged. Numbering starts at 1; at most
	// INT_MaxPoints() points (the rest is dropped). false = nothing usable given (the route is cleared then).
	bool INT_SetPoints(array<vector> positions, array<string> names)
	{
		m_INT_Points.Clear();
		m_INT_Base = 0;
		m_INT_Next = 0;
		int n = 0;
		if (positions)
			n = positions.Count();
		int maxPts = INT_MaxPoints();
		for (int i = 0; i < n; i++)
		{
			if (m_INT_Points.Count() >= maxPts)
				break;
			vector pos = positions.Get(i);
			if (pos[0] != pos[0] || pos[1] != pos[1] || pos[2] != pos[2])
				continue;
			string raw = "";
			if (names && i < names.Count())
				raw = names.Get(i);
			string clean = INT_VPP_Validate.CleanText(raw, INT_VPP_Net.NAME_MAX_BYTES);
			INT_VPP_RoutePoint p = new INT_VPP_RoutePoint();
			p.m_INT_Pos = pos;
			p.m_INT_Name = clean;
			m_INT_Points.Insert(p);
		}
		if (m_INT_Points.Count() > 0)
			INT_StampWorld();
		INT_SaveAndNotify();
		if (m_INT_Points.Count() > 0)
			return true;
		return false;
	}

	// «Развернуть» (review #8): the points still to walk, in reverse order, in place - one save, one OnChanged.
	// Passed points (kept when «убирать пройденные» is off) stay where they are, the display numbers stay in order
	// (base + index), so the next point keeps the next number. false = fewer than 2 points to walk.
	bool INT_ReverseRemaining()
	{
		int n = m_INT_Points.Count();
		int first = INT_NextIndex();
		if (first < 0)
			return false;
		if (n - first < 2)
			return false;
		int lo = first;
		int hi = n - 1;
		while (lo < hi)
		{
			// both held by locals while the slots are rewritten
			INT_VPP_RoutePoint a = m_INT_Points.Get(lo);
			INT_VPP_RoutePoint b = m_INT_Points.Get(hi);
			m_INT_Points.Set(lo, b);
			m_INT_Points.Set(hi, a);
			lo++;
			hi--;
		}
		INT_SaveAndNotify();
		return true;
	}

	// active = Count() > 0
	bool INT_IsActive()
	{
		if (m_INT_Points.Count() > 0)
			return true;
		return false;
	}

	int INT_MaxPoints()
	{
		int v = INT_VPP_Net.DEF_ROUTE_MAX_POINTS;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (cl)
		{
			INT_VPP_ClientSettings s = cl.GetSettings();
			if (s)
				v = s.RouteMaxPoints;
		}
		return INT_VPP_Validate.ClampInt(v, 1, INT_VPP_Net.HARD_MAX_ROUTE_POINTS);
	}

	float INT_ReachRadius()
	{
		float v = INT_VPP_Net.DEF_ROUTE_REACH_RADIUS;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (cl)
		{
			INT_VPP_ClientSettings s = cl.GetSettings();
			if (s)
				v = s.RouteReachRadius;
		}
		return INT_VPP_Validate.ClampFloat(v, INT_VPP_Net.MIN_ROUTE_REACH_RADIUS, INT_VPP_Net.MAX_ROUTE_REACH_RADIUS, INT_VPP_Net.DEF_ROUTE_REACH_RADIUS);
	}

	// «убирать пройденные», default true
	bool INT_AutoRemove()
	{
		return m_INT_AutoRemove;
	}

	void INT_SetAutoRemove(bool on)
	{
		if (m_INT_AutoRemove == on)
			return;
		m_INT_AutoRemove = on;
		if (on && m_INT_Next > 0)
		{
			// drop the points already passed
			int drop = m_INT_Next;
			for (int i = 0; i < drop; i++)
			{
				if (m_INT_Points.Count() > 0)
					m_INT_Points.RemoveOrdered(0);
			}
			m_INT_Base = m_INT_Base + drop;
			m_INT_Next = 0;
			if (m_INT_Points.Count() == 0)
				INT_ResetPoints();
		}
		INT_SaveAndNotify();
	}

	// i = 0: from → point 0; i > 0: point i-1 → point i (2D metres); 0 for a bad index
	float INT_LegLength(int i, vector from)
	{
		int n = m_INT_Points.Count();
		if (i < 0 || i >= n)
			return 0;
		vector a = from;
		if (i > 0)
			a = m_INT_Points.Get(i - 1).m_INT_Pos;
		vector b = m_INT_Points.Get(i).m_INT_Pos;
		return INT_VPP_Metres.INT_Dist2D(a, b);
	}

	// HUD calls it every 0.25 s; at most one point per call
	void INT_Tick(vector playerPos)
	{
		int n = m_INT_Points.Count();
		if (n == 0)
			return;
		if (playerPos[0] != playerPos[0] || playerPos[2] != playerPos[2])
			return;
		int idx = m_INT_Next;
		if (idx < 0 || idx >= n)
		{
			m_INT_Next = 0;
			idx = 0;
		}
		INT_VPP_RoutePoint p = m_INT_Points.Get(idx);
		float r = INT_ReachRadius();
		float d2 = INT_VPP_Metres.INT_Dist2DSq(playerPos, p.m_INT_Pos);
		if (d2 > r * r)
			return;
		int number = m_INT_Base + idx + 1;
		string name = p.m_INT_Name;
		if (m_INT_AutoRemove)
		{
			m_INT_Points.RemoveOrdered(idx);
			m_INT_Base = m_INT_Base + 1;
			if (m_INT_Points.Count() == 0)
				INT_ResetPoints();
		}
		else
		{
			m_INT_Next = idx + 1;
			if (m_INT_Next >= m_INT_Points.Count())
				INT_ResetPoints();
		}
		INT_Save();
		m_INT_OnReached.Invoke(number, name);
		m_INT_OnChanged.Invoke();
	}

	// ------------------------------------------------------------------ extras (MAP / HUD / TEST)
	// index of the next point to walk to (0 with «убирать пройденные»; -1 when the route is empty)
	int INT_NextIndex()
	{
		int n = m_INT_Points.Count();
		if (n == 0)
			return -1;
		if (m_INT_Next < 0 || m_INT_Next >= n)
			return 0;
		return m_INT_Next;
	}

	// display number of point i (1-based, continues after removed points)
	int INT_NumberOf(int i)
	{
		return m_INT_Base + i + 1;
	}

	// points already passed (removed or kept as passed)
	int INT_PassedCount()
	{
		int next = INT_NextIndex();
		if (next < 0)
			next = 0;
		return m_INT_Base + next;
	}

	// 2D metres from `from` to the next point (0 when empty)
	float INT_DistToNext(vector from)
	{
		int idx = INT_NextIndex();
		if (idx < 0)
			return 0;
		INT_VPP_RoutePoint p = m_INT_Points.Get(idx);
		vector nextPos = p.m_INT_Pos;
		return INT_VPP_Metres.INT_Dist2D(from, nextPos);
	}

	// 2D metres from `from` through every remaining point to the last one
	float INT_RemainingFrom(vector from)
	{
		int idx = INT_NextIndex();
		if (idx < 0)
			return 0;
		INT_VPP_RoutePoint first = m_INT_Points.Get(idx);
		vector prev = first.m_INT_Pos;
		float total = INT_VPP_Metres.INT_Dist2D(from, prev);
		int n = m_INT_Points.Count();
		for (int i = idx + 1; i < n; i++)
		{
			INT_VPP_RoutePoint p = m_INT_Points.Get(i);
			vector cur = p.m_INT_Pos;
			float leg = INT_VPP_Metres.INT_Dist2D(prev, cur);
			total = total + leg;
			prev = cur;
		}
		return total;
	}

	// mission start: a route made on another map is dropped
	void INT_CheckWorld()
	{
		string cur = INT_CurrentWorld();
		if (cur == "")
			return;
		if (m_INT_World != "" && m_INT_World != cur && m_INT_Points.Count() > 0)
		{
			Print("[INT_VPP] route of world " + m_INT_World + " dropped on " + cur);
			m_INT_World = cur;
			INT_ResetPoints();
			INT_SaveAndNotify();
			return;
		}
		m_INT_World = cur;
	}

	// writes Client.json (preferences + this route)
	void INT_Save()
	{
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		prefs.INT_WriteFile();
	}

	// ------------------------------------------------------------------ file section
	void INT_ReadFile(INT_VPP_ClientFile f)
	{
		if (!f)
			return;
		bool removePassed = true;
		if (f.INT_RouteKeepPassed == 1)
			removePassed = false;
		m_INT_AutoRemove = removePassed;
		m_INT_World = f.INT_RouteWorld;
		m_INT_Points.Clear();
		int n = 0;
		if (f.INT_Route)
			n = f.INT_Route.Count();
		for (int i = 0; i < n; i++)
		{
			INT_VPP_RouteFilePoint fp = f.INT_Route.Get(i);
			if (!fp)
				continue;
			if (m_INT_Points.Count() >= INT_VPP_Net.HARD_MAX_ROUTE_POINTS)
				break;
			vector filePos = Vector(fp.INT_X, fp.INT_Y, fp.INT_Z);
			string label = fp.INT_Name;
			INT_VPP_RoutePoint p = new INT_VPP_RoutePoint();
			p.m_INT_Pos = filePos;
			p.m_INT_Name = label;
			m_INT_Points.Insert(p);
		}
		m_INT_Base = f.INT_RouteBase;
		if (m_INT_Base < 0)
			m_INT_Base = 0;
		m_INT_Next = f.INT_RouteNext;
		int cnt = m_INT_Points.Count();
		if (cnt == 0)
		{
			m_INT_Base = 0;
			m_INT_Next = 0;
		}
		else if (m_INT_Next < 0 || m_INT_Next >= cnt || m_INT_AutoRemove)
		{
			m_INT_Next = 0;
		}
	}

	void INT_FillFile(INT_VPP_ClientFile f)
	{
		if (!f)
			return;
		int keep = 0;
		if (!m_INT_AutoRemove)
			keep = 1;
		f.INT_RouteKeepPassed = keep;
		f.INT_RouteWorld = m_INT_World;
		f.INT_RouteBase = m_INT_Base;
		f.INT_RouteNext = m_INT_Next;
		if (!f.INT_Route)
			f.INT_Route = new array<ref INT_VPP_RouteFilePoint>();
		f.INT_Route.Clear();
		int n = m_INT_Points.Count();
		for (int i = 0; i < n; i++)
		{
			INT_VPP_RoutePoint p = m_INT_Points.Get(i);
			INT_VPP_RouteFilePoint fp = new INT_VPP_RouteFilePoint();
			vector pos = p.m_INT_Pos;
			fp.INT_X = pos[0];
			fp.INT_Y = pos[1];
			fp.INT_Z = pos[2];
			fp.INT_Name = p.m_INT_Name;
			f.INT_Route.Insert(fp);
		}
	}

	// ------------------------------------------------------------------ internals
	protected void INT_ResetPoints()
	{
		m_INT_Points.Clear();
		m_INT_Base = 0;
		m_INT_Next = 0;
	}

	protected void INT_SaveAndNotify()
	{
		INT_Save();
		m_INT_OnChanged.Invoke();
	}

	protected void INT_StampWorld()
	{
		string cur = INT_CurrentWorld();
		if (cur != "")
			m_INT_World = cur;
	}

	protected string INT_CurrentWorld()
	{
		if (!GetGame())
			return "";
		if (!GetGame().GetWorld())
			return "";
		string w = GetGame().GetWorldName();
		w.ToLower();
		return w;
	}
}
