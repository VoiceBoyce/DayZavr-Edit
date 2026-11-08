// INT PartyMode (client) — shown friends and their live pings on the map mods that are loaded (spec.md §9, owner D13:
// always, no GPS rule; the player himself is never drawn). Soft, research/02_map_integration.md: our own layer goes
// INSIDE the foreign MapWidget, found at run time — no requiredAddons, no modded map class, no foreign type in code,
// nothing written into the maps' marker stores (VPP saves client marks to the player's JSON, the others clear theirs).
//   big map (the open menu, checked every frame: one GetMenu + a class-name compare, cached per menu object)
//     VPP map      menu class "VPPMapMenu"        MapWidget "Map_Widget"   sort 300 (its map has priority 100)
//     RF Map       menu class "RF_MAP_CL_MapHud"  MapWidget "mapWidget"    sort 460 (zone canvas 450 < us < marks 500)
//     vanilla map  menu id MENU_MAP               MapWidget "Map"          sort 300
//     The VPP / RF menus are hidden and shown again, not destroyed: the layer goes away with the menu and is built
//     again on every show.
//   HUD minimap (top-level workspace widgets, looked for once a second while not bound, only if the mod is alive)
//     RF minimap   "rootFrame" -> "mapWidgetWrap" -> "minimapWidget"   sort 460 (type RF_MAP_CL_MinimapHud compiled)
//     @MiniMap     "rootFrame" -> "PlaceHolder"  -> "MapWidget"       sort 300 (type PluginMiniMap compiled — a dead
//                  @MiniMap keeps its define, so the type is the test; repaired for 164200 by the owner's request)
//     Minimaps are paused (hidden) while any menu is open and project only the marks inside their view.
// Data (no request to the server of our own): INT_PTY_Client.s_Friends (IsShown, KnowsPos, WorldPos, NameOf,
// DeathPos) and s_Pings (Gone == 0). One shared list of marks is rebuilt on every INT_PTY_Client.OnChanged event
// (FRIENDS / STATE / PINGS — the state arrives every PullIntervalSec, 3 s) and once a second while a map is drawn
// (alias / colour edits); live friend positions of the network bubble are re-read 10 times a second only while a
// minimap is drawn. Nothing runs while no supported map is drawn.
// Colours and the look of the layer come from the public GUI API (INT_PTY_UI, owned by the HUD): FriendColor(id) (the
// same colour as on the HUD), PingTex(shape, outline), and the part "mapLayer" — scale / opacity / accent (0 = every
// mark in its own colour; else the names of live friends) / hidden — re-read on INT_PTY_UI.OnStyleApplied; minimaps
// also follow INT_PTY_UI.IsVisible() (external hide of the HUD).
// Entry points (INT_PTY_Mission): Tick(dt), Shutdown(). Public: PingColorOf(owner), PingShapeOf(owner), TestState(),
// MenuMapKind(), VisiblePins(hud).

class INT_PTY_MapLayerMark
{
	int m_Kind = -1;					// INT_PTY_MapLayer.MK_*
	vector m_At;
	string m_Name;						// display name, raw (the pin cuts / capitalises it)
	int m_Color;
	int m_Shape;						// pings: INT_PTY_Shape.*
	ref INT_PTY_Friend m_Friend;		// live friend (fast position refresh), null for dead friends and pings
	string m_Owner;						// Steam64: the friend, or the owner of the ping
	int m_Near = -1;					// pings: index of the owner's own mark (its name labels both when they touch)
}

class INT_PTY_MapLayer
{
	// kinds of marks (draw order: pings below, dead friends, live friends on top)
	static const int MK_FRIEND = 0;
	static const int MK_DEAD = 1;
	static const int MK_PING = 2;

	// kinds of host maps
	static const int MAP_NONE = 0;
	static const int MAP_VANILLA = 1;
	static const int MAP_VPP = 2;
	static const int MAP_RF = 3;
	static const int MAP_RF_MINI = 4;
	static const int MAP_MINIMAP = 5;

	static const int SORT_DEFAULT = 300;
	static const int SORT_RF = 460;
	static const int MAX_FRIENDS = 50;		// = INT_PTY_Const.HARD_DISPLAY_LIMIT
	static const int MAX_PINGS = 60;
	static const float SCAN_GAP = 1.0;		// HUD minimap search while not bound
	static const float FULL_GAP = 1.0;		// full rebuild of the marks while a map is drawn
	static const float LIVE_GAP = 0.1;		// live friend positions while a minimap is drawn
	static const float MOVE_SQ = 0.25;		// 0.5 m

	protected static ref INT_PTY_MapLayerSlot s_Menu;
	protected static ref INT_PTY_MapLayerSlot s_RfMini;
	protected static ref INT_PTY_MapLayerSlot s_MiniMap;
	protected static ref array<ref INT_PTY_MapLayerMark> s_Marks;
	protected static int s_MarkCount;
	protected static int s_ContentVer;
	protected static int s_PosVer;
	protected static bool s_DataDirty = true;
	protected static bool s_Hooked;
	protected static float s_ScanIn;
	protected static float s_FullIn;
	protected static float s_LiveIn;
	protected static int s_HasRfMini = -1;
	protected static int s_HasMiniMap = -1;
	protected static UIScriptedMenu s_NotMap;		// weak: the last open menu known not to be a map
	protected static UIScriptedMenu s_MenuOwner;	// weak: the menu whose map the menu slot is bound to
	protected static UIScriptedMenu s_NoMapWidget;	// weak (M-3): a known map menu whose MapWidget was not found
	protected static float s_NoMapAt;				// when that lookup failed (retried every NO_MAP_RETRY_S)
	static const float NO_MAP_RETRY_S = 2.0;
	protected static string s_MyName = "";
	protected static string s_DeadText = "";

	// style of the GUI API part "mapLayer" (read from INT_PTY_UI)
	protected static bool s_StyleRead;
	protected static float s_StyleScale = 1;
	protected static float s_StyleAlpha = 1;
	protected static int s_StyleAccent;
	protected static bool s_StyleHidden;

	// self-test counters
	static int s_Collects;
	static int s_LiveRefreshes;
	static int s_DataEvents;

	// frame cost while a map is bound (TickCount calibrated against the game clock; DebugMode 1 logs it every 10 s)
	static const float COST_EVERY = 10.0;
	protected static float s_TicksPerUs = -1;
	protected static int s_CalTick;
	protected static float s_CalTime = -1;
	protected static float s_CalTicks;
	protected static float s_CalSec;
	protected static float s_CostSum;
	protected static float s_CostMax;
	protected static int s_CostFrames;
	protected static float s_CostAt;
	static float s_LastAvgUs = -1;
	static float s_LastMaxUs = -1;

	// --- entry points ------------------------------------------------------------------------------------------------
	static void Tick(float dt)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		int t0 = TickCount(0);
		EnsureInit();
		UIManager um = GetGame().GetUIManager();
		UIScriptedMenu top = null;
		if (um)
			top = um.GetMenu();

		// 1. the big map of the open menu
		int kind = MAP_NONE;
		int sort = SORT_DEFAULT;
		MapWidget found = FindMenuMap(top, kind, sort);
		s_Menu.Track(found, kind, sort);

		// 2. HUD minimaps (only when such a mod is alive, never while a menu is open)
		bool menuOpen = top != null;
		if (!menuOpen && HudMapsPossible())
		{
			bool missing = false;
			if (HasRfMini() && !s_RfMini.IsBound())
				missing = true;
			if (HasMiniMap() && !s_MiniMap.IsBound())
				missing = true;
			if (missing)
			{
				s_ScanIn = s_ScanIn - dt;
				if (s_ScanIn <= 0)
				{
					s_ScanIn = SCAN_GAP;
					ScanHud();
				}
			}
		}

		// 3. data, only while something is drawn
		bool menuLive = false;
		if (!s_StyleHidden && s_Menu.IsLive())
			menuLive = true;
		bool hudLive = false;
		if (!menuOpen && !s_StyleHidden && INT_PTY_UI.IsVisible())
		{
			if (s_RfMini.IsLive() || s_MiniMap.IsLive())
				hudLive = true;
		}
		if (menuLive || hudLive)
		{
			s_FullIn = s_FullIn - dt;
			s_LiveIn = s_LiveIn - dt;
			if (s_DataDirty || s_FullIn <= 0)
			{
				Collect();
				s_DataDirty = false;
				s_FullIn = FULL_GAP;
				s_LiveIn = LIVE_GAP;
			}
			else if (hudLive && s_LiveIn <= 0)
			{
				RefreshLive();
				s_LiveIn = LIVE_GAP;
			}
		}

		// 4. the slots (bound ones only; a HUD slot hides its pins while a menu is open)
		bool showMenu = true;
		bool showHud = true;
		if (s_StyleHidden)
		{
			showMenu = false;
			showHud = false;
		}
		if (menuOpen || !INT_PTY_UI.IsVisible())
			showHud = false;
		s_Menu.Update(showMenu);
		s_RfMini.Update(showHud);
		s_MiniMap.Update(showHud);
		if (s_Menu.IsBound() || s_RfMini.IsBound() || s_MiniMap.IsBound())
			MeasureCost(t0);
	}

	protected static void MeasureCost(int t0)
	{
		int ticks = TickCount(t0);
		float t = GetGame().GetTickTime();
		if (s_TicksPerUs < 0)
		{
			if (s_CalTime >= 0)
			{
				int d = TickCount(s_CalTick);
				float cdt = t - s_CalTime;
				if (d > 0 && cdt > 0 && cdt < 5.0)
				{
					s_CalTicks = s_CalTicks + d;
					s_CalSec = s_CalSec + cdt;
				}
				if (s_CalSec >= 3.0)
				{
					float tpu = s_CalTicks / (s_CalSec * 1000000.0);
					if (tpu > 0.0005)
						s_TicksPerUs = tpu;
				}
			}
			s_CalTick = TickCount(0);
			s_CalTime = t;
			return;
		}
		float us = ticks / s_TicksPerUs;
		s_CostSum = s_CostSum + us;
		s_CostFrames++;
		if (us > s_CostMax)
			s_CostMax = us;
		if (t - s_CostAt < COST_EVERY)
			return;
		s_CostAt = t;
		float frames = s_CostFrames;
		if (frames < 1)
			frames = 1;
		s_LastAvgUs = s_CostSum / frames;
		s_LastMaxUs = s_CostMax;
		s_CostSum = 0;
		s_CostMax = 0;
		s_CostFrames = 0;
		int avg = (int)Math.Round(s_LastAvgUs);
		int mx = (int)Math.Round(s_LastMaxUs);
		string sa = avg.ToString();
		string sm = mx.ToString();
		string sn = s_MarkCount.ToString();
		INT_PTY_Log.Dbg("карта: кадр " + sa + " мкс (макс " + sm + "), меток " + sn);
	}

	static void Shutdown()
	{
		if (s_Hooked)
		{
			ScriptInvoker inv = INT_PTY_Client.OnChanged();
			if (inv)
				inv.Remove(INT_PTY_MapLayer.OnData);
			ScriptInvoker sty = INT_PTY_UI.OnStyleApplied();
			if (sty)
				sty.Remove(INT_PTY_MapLayer.OnStyle);
		}
		s_Hooked = false;
		s_StyleRead = false;
		if (s_Menu)
			s_Menu.Unbind();
		if (s_RfMini)
			s_RfMini.Unbind();
		if (s_MiniMap)
			s_MiniMap.Unbind();
		if (s_Marks)
			s_Marks.Clear();
		s_MarkCount = 0;
		s_ContentVer++;
		s_PosVer++;
		s_DataDirty = true;
		s_NotMap = null;
		s_MenuOwner = null;
		s_NoMapWidget = null;
		s_MyName = "";
		s_DeadText = "";
		s_HasRfMini = -1;
		s_HasMiniMap = -1;
		s_ScanIn = 0;
	}

	protected static void EnsureInit()
	{
		if (!s_Menu)
			s_Menu = new INT_PTY_MapLayerSlot("menu", false);
		if (!s_RfMini)
			s_RfMini = new INT_PTY_MapLayerSlot("rfmini", true);
		if (!s_MiniMap)
			s_MiniMap = new INT_PTY_MapLayerSlot("minimap", true);
		if (!s_Marks)
			s_Marks = new array<ref INT_PTY_MapLayerMark>();
		if (!s_StyleRead)
		{
			s_StyleRead = true;
			ReadStyle();
		}
		if (s_Hooked)
			return;
		ScriptInvoker inv = INT_PTY_Client.OnChanged();
		if (!inv)
			return;
		if (inv.Count(INT_PTY_MapLayer.OnData) == 0)
			inv.Insert(INT_PTY_MapLayer.OnData);
		ScriptInvoker sty = INT_PTY_UI.OnStyleApplied();
		if (sty && sty.Count(INT_PTY_MapLayer.OnStyle) == 0)
			sty.Insert(INT_PTY_MapLayer.OnStyle);
		s_Hooked = true;
	}

	// INT_PTY_Client.OnChanged (INT_PTY_What bits): friends / state / pings changed -> rebuild the marks.
	static void OnData(int what)
	{
		int mask = INT_PTY_What.FRIENDS | INT_PTY_What.STATE | INT_PTY_What.PINGS;
		if ((what & mask) == 0)
			return;
		s_DataDirty = true;
		s_DataEvents++;
	}

	static void MarkData()
	{
		s_DataDirty = true;
	}

	// --- finding the maps --------------------------------------------------------------------------------------------
	protected static MapWidget FindMenuMap(UIScriptedMenu top, out int kind, out int sort)
	{
		kind = MAP_NONE;
		sort = SORT_DEFAULT;
		if (!top)
			return null;
		if (top == s_NotMap)
			return null;
		// M-3: a map menu without the expected MapWidget (an update renamed it) is not searched every frame
		float clock = INT_PTY_Client.Clock();
		if (top == s_NoMapWidget && clock - s_NoMapAt < NO_MAP_RETRY_S && clock >= s_NoMapAt)
			return null;
		if (top == s_MenuOwner && s_Menu.IsBound())
		{
			// same menu, map still bound: no FindAnyWidget walk every frame
			kind = s_Menu.m_Kind;
			sort = s_Menu.m_Sort;
			return s_Menu.m_Map;
		}
		Widget root = top.GetLayoutRoot();
		if (!root)
			return null;
		string cls = top.ClassName();
		string wname = "";
		if (cls == "VPPMapMenu")
		{
			wname = "Map_Widget";
			kind = MAP_VPP;
		}
		else if (cls == "RF_MAP_CL_MapHud")
		{
			wname = "mapWidget";
			kind = MAP_RF;
			sort = SORT_RF;
		}
		else if (top.GetID() == MENU_MAP)
		{
			wname = "Map";
			kind = MAP_VANILLA;
		}
		if (wname == "")
		{
			s_NotMap = top;
			kind = MAP_NONE;
			return null;
		}
		MapWidget mw = MapWidget.Cast(root.FindAnyWidget(wname));
		if (!mw)
		{
			kind = MAP_NONE;
			s_NoMapWidget = top;
			s_NoMapAt = clock;
			return null;
		}
		s_NoMapWidget = null;
		s_MenuOwner = top;
		return mw;
	}

	protected static bool HasRfMini()
	{
		if (s_HasRfMini < 0)
		{
			s_HasRfMini = 0;
			string tn = "RF_MAP_CL_MinimapHud";
			typename t = tn.ToType();
			if (t)
				s_HasRfMini = 1;
		}
		return s_HasRfMini == 1;
	}

	protected static bool HasMiniMap()
	{
		if (s_HasMiniMap < 0)
		{
			s_HasMiniMap = 0;
			string tn = "PluginMiniMap";
			typename t = tn.ToType();
			if (t)
				s_HasMiniMap = 1;
		}
		return s_HasMiniMap == 1;
	}

	protected static bool HudMapsPossible()
	{
		if (HasRfMini())
			return true;
		return HasMiniMap();
	}

	// The minimap HUDs are created on the workspace root ("rootFrame" is a common name, the children identify them).
	protected static void ScanHud()
	{
		WorkspaceWidget ws = GetGame().GetWorkspace();
		if (!ws)
			return;
		bool wantRf = false;
		if (HasRfMini() && !s_RfMini.IsBound())
			wantRf = true;
		bool wantMini = false;
		if (HasMiniMap() && !s_MiniMap.IsBound())
			wantMini = true;
		Widget c = ws.GetChildren();
		Widget wrap;
		Widget holder;
		MapWidget rf;
		MapWidget mm;
		while (c)
		{
			if (c.GetName() == "rootFrame")
			{
				if (wantRf)
				{
					wrap = c.FindAnyWidget("mapWidgetWrap");
					if (wrap)
					{
						rf = MapWidget.Cast(wrap.FindAnyWidget("minimapWidget"));
						if (rf)
						{
							s_RfMini.Bind(rf, MAP_RF_MINI, SORT_RF);
							wantRf = false;
						}
					}
				}
				if (wantMini)
				{
					holder = c.FindAnyWidget("PlaceHolder");
					if (holder)
					{
						mm = MapWidget.Cast(holder.FindAnyWidget("MapWidget"));
						if (mm)
						{
							s_MiniMap.Bind(mm, MAP_MINIMAP, SORT_DEFAULT);
							wantMini = false;
						}
					}
				}
			}
			c = c.GetSibling();
		}
	}

	// --- marks -------------------------------------------------------------------------------------------------------
	static int MarkCount()
	{
		return s_MarkCount;
	}

	static INT_PTY_MapLayerMark MarkAt(int i)
	{
		if (!s_Marks || i < 0 || i >= s_MarkCount || i >= s_Marks.Count())
			return null;
		return s_Marks[i];
	}

	static int ContentVer()
	{
		return s_ContentVer;
	}

	static int PosVer()
	{
		return s_PosVer;
	}

	// Rebuilds the shared list of marks: live pings, then dead friends, then live friends (drawn on top).
	protected static void Collect()
	{
		s_Collects++;
		int n = 0;
		int changed = 0;
		string me = INT_PTY_Client.s_MySteam;
		Man player = GetGame().GetPlayer();
		array<ref INT_PTY_PingMark> pings = INT_PTY_Client.s_Pings;
		array<ref INT_PTY_Friend> friends = INT_PTY_Client.s_Friends;
		vector at;
		string name;
		int color;
		int shape;
		int count = 0;
		if (pings)
		{
			foreach (INT_PTY_PingMark p : pings)
			{
				if (!p || p.Gone != 0)
					continue;
				if (count >= MAX_PINGS)
					break;
				if (p.Owner != me && !INT_PTY_Client.IsShown(p.Owner))
					continue;
				at = p.Where;
				if (!INT_PTY_Util.SanePos(at))
					continue;
				name = PingOwnerName(p.Owner);
				color = PingColorOf(p.Owner);
				shape = PingShapeOf(p.Owner);
				changed = changed | SetMark(n, MK_PING, at, name, color, shape, null, p.Owner);
				n++;
				count++;
			}
		}
		count = 0;
		if (friends)
		{
			bool dead;
			int kind;
			INT_PTY_Friend live;
			for (int pass = 0; pass < 2; pass++)
			{
				foreach (INT_PTY_Friend f : friends)
				{
					if (!f || f.Id == me || count >= MAX_FRIENDS)
						continue;
					dead = f.Dead;
					if (pass == 0 && !dead)
						continue;
					if (pass == 1 && dead)
						continue;
					if (!INT_PTY_Client.IsShown(f.Id))
						continue;
					if (dead)
					{
						at = f.DeathPos;
						kind = MK_DEAD;
						live = null;
					}
					else
					{
						if (!f.Online && !f.Body)
							continue;		// offline: not in the world
						if (!f.KnowsPos())
							continue;
						if (f.Body && f.Body == player)
							continue;		// never the player himself
						at = f.WorldPos();
						kind = MK_FRIEND;
						live = f;
					}
					if (!INT_PTY_Util.SanePos(at))
						continue;
					if (at[0] == 0 && at[2] == 0)
						continue;		// unknown position
					name = INT_PTY_Client.NameOf(f.Id);
					color = INT_PTY_UI.FriendColor(f.Id);
					changed = changed | SetMark(n, kind, at, name, color, 0, live, f.Id);
					n++;
					count++;
				}
			}
		}
		if (n != s_MarkCount)
			changed = changed | 1;
		s_MarkCount = n;
		changed = changed | LinkPingsToOwners();
		INT_PTY_MapLayerMark old;
		for (int i = n; i < s_Marks.Count(); i++)
		{
			old = s_Marks[i];
			old.m_Friend = null;		// do not keep removed friends alive
		}
		if ((changed & 1) != 0)
			s_ContentVer++;
		if (changed != 0)
			s_PosVer++;
	}

	// Every ping learns the index of its owner's own mark (live or dead friend), so the slot can drop the ping's label
	// while the two touch on screen (a friend who pinged where he stands). Returns 2 when a link changed.
	protected static int LinkPingsToOwners()
	{
		int res = 0;
		INT_PTY_MapLayerMark pk;
		INT_PTY_MapLayerMark fk;
		int near;
		for (int i = 0; i < s_MarkCount; i++)
		{
			pk = s_Marks[i];
			if (pk.m_Kind != MK_PING)
				continue;
			near = -1;
			for (int j = 0; j < s_MarkCount; j++)
			{
				fk = s_Marks[j];
				if (fk.m_Kind == MK_PING || fk.m_Owner != pk.m_Owner)
					continue;
				near = j;
				break;
			}
			if (near != pk.m_Near)
			{
				pk.m_Near = near;
				res = 2;
			}
		}
		return res;
	}

	// Writes mark i; returns 1 = content changed, 2 = moved.
	protected static int SetMark(int i, int kind, vector at, string name, int color, int shape, INT_PTY_Friend live, string owner)
	{
		while (s_Marks.Count() <= i)
		{
			INT_PTY_MapLayerMark fresh = new INT_PTY_MapLayerMark();
			s_Marks.Insert(fresh);
		}
		INT_PTY_MapLayerMark mk = s_Marks[i];
		int res = 0;
		if (mk.m_Kind != kind || mk.m_Name != name || mk.m_Color != color || mk.m_Shape != shape)
			res = 1;
		float d2 = vector.DistanceSq(mk.m_At, at);
		if (d2 > MOVE_SQ)
			res = res | 2;
		mk.m_Kind = kind;
		mk.m_At = at;
		mk.m_Name = name;
		mk.m_Color = color;
		mk.m_Shape = shape;
		mk.m_Friend = live;
		mk.m_Owner = owner;
		if (kind != MK_PING)
			mk.m_Near = -1;
		return res;
	}

	// Positions of the live friends only (their replicated entity moves every frame; minimaps need it).
	protected static void RefreshLive()
	{
		s_LiveRefreshes++;
		int moved = 0;
		INT_PTY_MapLayerMark mk;
		vector w;
		float d2;
		for (int i = 0; i < s_MarkCount; i++)
		{
			mk = s_Marks[i];
			if (mk.m_Kind != MK_FRIEND || !mk.m_Friend)
				continue;
			w = mk.m_Friend.WorldPos();
			d2 = vector.DistanceSq(w, mk.m_At);
			if (d2 <= MOVE_SQ)
				continue;
			mk.m_At = w;
			moved++;
		}
		if (moved > 0)
			s_PosVer++;
	}

	// --- ping look, names --------------------------------------------------------------------------------------------
	// How I see the pings of `owner`: the one rule of INT_PTY_UI (window, HUD and maps agree).
	static int PingColorOf(string owner)
	{
		bool mine = owner == INT_PTY_Client.s_MySteam;
		int c = INT_PTY_UI.PingColorOf(owner, mine);
		return INT_UIF_Color.WithAlpha(c, 255);
	}

	static int PingShapeOf(string owner)
	{
		bool mine = owner == INT_PTY_Client.s_MySteam;
		return INT_PTY_UI.PingShapeOf(owner, mine);
	}

	static string PingOwnerName(string owner)
	{
		if (owner == INT_PTY_Client.s_MySteam)
			return MyName();
		return INT_PTY_Client.NameOf(owner);
	}

	static string MyName()
	{
		if (s_MyName != "")
			return s_MyName;
		Man pl = GetGame().GetPlayer();
		if (pl && pl.GetIdentity())
			s_MyName = pl.GetIdentity().GetName();
		if (s_MyName != "")
			return s_MyName;
		return Widget.TranslateString("#STR_INT_PTY_M_ME");
	}

	// «П О Г И Б» under a dead friend's name (big maps).
	static string DeadText()
	{
		if (s_DeadText == "")
			s_DeadText = INT_PTY_Text.Caps("#STR_INT_PTY_M_DEAD");
		return s_DeadText;
	}

	static string KindName(int kind)
	{
		if (kind == MAP_VANILLA)
			return "vanilla";
		if (kind == MAP_VPP)
			return "vpp";
		if (kind == MAP_RF)
			return "rf";
		if (kind == MAP_RF_MINI)
			return "rfmini";
		if (kind == MAP_MINIMAP)
			return "minimap";
		return "-";
	}

	// --- style (GUI API part "mapLayer", INT_PTY_UI) -----------------------------------------------------------------
	// INT_PTY_UI.OnStyleApplied (string part): "" = every part.
	static void OnStyle(string part)
	{
		if (part == "" || part == INT_PTY_UI.PART_MAP)
			ReadStyle();
	}

	// scale (0.6..1.6) of every mark, opacity (0.2..1), accent = colour of the live friends' names (0 = white), hidden.
	protected static void ReadStyle()
	{
		float sc = INT_PTY_UI.PartScale(INT_PTY_UI.PART_MAP);
		float al = INT_PTY_UI.PartOpacity(INT_PTY_UI.PART_MAP);
		int accent = INT_PTY_UI.PartAccent(INT_PTY_UI.PART_MAP);
		bool hidden = false;
		if (!INT_PTY_UI.IsPartShown(INT_PTY_UI.PART_MAP))
			hidden = true;
		sc = Math.Clamp(sc, 0.5, 2.0);
		al = Math.Clamp(al, 0.1, 1.0);
		bool rebuild = false;
		if (Math.AbsFloat(sc - s_StyleScale) > 0.001)
			rebuild = true;
		s_StyleScale = sc;
		s_StyleAlpha = al;
		s_StyleAccent = accent;
		s_StyleHidden = hidden;
		s_ContentVer++;
		s_DataDirty = true;
		if (!rebuild)
			return;
		if (s_Menu)
			s_Menu.Rebuild();
		if (s_RfMini)
			s_RfMini.Rebuild();
		if (s_MiniMap)
			s_MiniMap.Rebuild();
	}

	static float StyleScale()
	{
		return s_StyleScale;
	}

	static float StyleAlpha()
	{
		return s_StyleAlpha;
	}

	static int StyleAccent()
	{
		return s_StyleAccent;
	}

	static bool IsHidden()
	{
		return s_StyleHidden;
	}

	// --- self-test -----------------------------------------------------------------------------------------------------
	// "marks 5 content 3 pos 9 collects 4 live 0 | menu vpp pins 4/5 places 6 binds 1 | rfmini - ... | minimap - ..."
	static string TestState()
	{
		EnsureInit();
		string nm = s_MarkCount.ToString();
		string cv = s_ContentVer.ToString();
		string pv = s_PosVer.ToString();
		string co = s_Collects.ToString();
		string lr = s_LiveRefreshes.ToString();
		string de = s_DataEvents.ToString();
		string a = s_Menu.Describe();
		string b = s_RfMini.Describe();
		string c = s_MiniMap.Describe();
		string head = "marks " + nm + " content " + cv + " pos " + pv;
		string mid = " collects " + co + " live " + lr + " events " + de;
		string tail = " | " + a + " | " + b + " | " + c;
		return head + mid + tail;
	}

	// Kind of the big map the layer is drawn on now (MAP_*; MAP_NONE when none).
	static int MenuMapKind()
	{
		if (!s_Menu || !s_Menu.IsBound())
			return MAP_NONE;
		return s_Menu.m_Kind;
	}

	// --- stand probes (INT_PTY_TestKit) ---------------------------------------------------------------------------------
	// The bound big map (hud = the first bound minimap), null when none.
	static MapWidget TestMap(bool hud)
	{
		EnsureInit();
		if (!hud)
			return s_Menu.m_Map;
		if (s_RfMini.IsBound())
			return s_RfMini.m_Map;
		return s_MiniMap.m_Map;
	}

	protected static INT_PTY_MapLayerSlot TestSlot(bool hud)
	{
		EnsureInit();
		if (!hud)
			return s_Menu;
		if (s_RfMini.IsBound())
			return s_RfMini;
		return s_MiniMap;
	}

	// "kind text @x,y side; ..." of every shown pin.
	static string TestPins(bool hud)
	{
		INT_PTY_MapLayerSlot sl = TestSlot(hud);
		return sl.TestPins();
	}

	// Shown pins that are not where MapToScreen puts their mark (> 3 px), plus pins whose visibility disagrees
	// with the map rect; -1 = not bound. shown / worst (px) through out.
	static int TestCheck(bool hud, out int shown, out float worst)
	{
		INT_PTY_MapLayerSlot sl = TestSlot(hud);
		return sl.TestCheck(shown, worst);
	}

	// Pins drawn (inside the view) on the big map / on the minimaps now.
	static int VisiblePins(bool hud)
	{
		EnsureInit();
		if (!hud)
			return s_Menu.m_Visible;
		return s_RfMini.m_Visible + s_MiniMap.m_Visible;
	}
}
