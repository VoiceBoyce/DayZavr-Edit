// ============================================================================
// VanillaPPMap «Экспедиция» B — live widget-tree counters (HUD3D, BUILD_PLAN §5 / §8).
// Every owner of a widget tree calls INT_Up(kind) right after it created the tree and INT_Down(kind) right
// after it unlinked it; the TestKit reads INT_Live(kind) (leak tests: open/close ×N → equal, hidden → 0).
// Kinds: 0 menu root, 1 map overlay host, 2 marker widget, 3 card, 4 editor, 5 zone editor,
//        6 3D marker tree (VPP3DMarker + the route beacon), 7 HUD route ribbon, 8 toast,
//        9 label editor island (2026-11-13), 10 admin text label on the map (one per label tree + the editor preview).
//        11 minimap root (the whole HUD tree, 2026-11-13), 12 minimap overlay tree (marker / route point / zone or
//        label name / edge pin), 13 minimap settings panel in the map.
// INT_CanvasLines = DrawLine calls of the last map frame (MAP writes it).
// 3_Game, no client-only calls: compiles and runs on the dedicated server (counts stay 0 there).
// ============================================================================

class INT_VPP_Stat
{
	static const int INT_KIND_MENU = 0;
	static const int INT_KIND_OVERLAY = 1;
	static const int INT_KIND_MARKER2D = 2;
	static const int INT_KIND_CARD = 3;
	static const int INT_KIND_EDITOR = 4;
	static const int INT_KIND_ZONE_EDITOR = 5;
	static const int INT_KIND_3D = 6;
	static const int INT_KIND_HUD = 7;
	static const int INT_KIND_TOAST = 8;
	static const int INT_KIND_LABEL_EDITOR = 9;
	static const int INT_KIND_LABEL = 10;
	static const int INT_KIND_MINI = 11;
	static const int INT_KIND_MINI_ITEM = 12;
	static const int INT_KIND_MINI_PANEL = 13;
	static const int INT_KIND_MAX = 16;

	static int INT_CanvasLines;

	protected static ref array<int> s_INT_Live;
	protected static int s_INT_Underflows;

	protected static void INT_Ensure()
	{
		if (s_INT_Live)
			return;
		s_INT_Live = new array<int>();
		for (int i = 0; i < INT_KIND_MAX; i++)
		{
			s_INT_Live.Insert(0);
		}
	}

	static void INT_Up(int kind)
	{
		if (kind < 0 || kind >= INT_KIND_MAX)
			return;
		INT_VPP_Stat.INT_Ensure();
		int v = s_INT_Live.Get(kind);
		v++;
		s_INT_Live.Set(kind, v);
	}

	static void INT_Down(int kind)
	{
		if (kind < 0 || kind >= INT_KIND_MAX)
			return;
		INT_VPP_Stat.INT_Ensure();
		int v = s_INT_Live.Get(kind);
		if (v <= 0)
		{
			// a Down without an Up is an owner bug: count it, never go below 0
			s_INT_Underflows++;
			if (s_INT_Underflows <= 5)
				Print("[INT_VPP] Stat underflow kind=" + kind.ToString());
			return;
		}
		v--;
		s_INT_Live.Set(kind, v);
	}

	static int INT_Live(int kind)
	{
		if (kind < 0 || kind >= INT_KIND_MAX)
			return 0;
		INT_VPP_Stat.INT_Ensure();
		return s_INT_Live.Get(kind);
	}

	// extra (tests / diagnostics): Down calls that found the counter at 0
	static int INT_Underflows()
	{
		return s_INT_Underflows;
	}

	// extra: one line with all counters (safe in any Print)
	static string INT_Dump()
	{
		INT_VPP_Stat.INT_Ensure();
		string s = "live";
		for (int i = 0; i <= INT_KIND_MINI_PANEL; i++)
		{
			int v = s_INT_Live.Get(i);
			s = s + " k" + i.ToString() + "=" + v.ToString();
		}
		int lines = INT_CanvasLines;
		s = s + " canvas=" + lines.ToString();
		return s;
	}
}
