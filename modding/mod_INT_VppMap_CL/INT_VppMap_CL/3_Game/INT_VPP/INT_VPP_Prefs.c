// ============================================================================
// VanillaPPMap «Экспедиция» B — client preferences (HUD3D, BUILD_PLAN §8).
// File: $profile:INT_Mods/INT_VppMap/Client.json (until 2026-11-12 $profile:VPPMap_INT/Client.json: moved once by
// INT_MigrateOnce, INT_VPP_Files.c rules) — ONE client file for the preferences AND the personal route
// (INT_VPP_Route keeps its section in the same DTO; whoever saves writes the whole file). Never the VPP cache
// files. Format (all keys optional; a missing key = its default):
//   INT_FileVersion       1
//   INT_LayersOff[5]      1 = layer off (index = INT_VPP_LAYER_*), missing entries = on
//   INT_3DOff             1 = master 3D switch off (missing / 0 = on)
//   INT_Dist3D[5]         metres per layer; < 0 or missing = follow the server cap; always clamped to the cap
//   INT_RouteWorld        world the route was made on (another map → the route is dropped at mission start)
//   INT_RouteKeepPassed   1 = «убирать пройденные» off (missing / 0 = on, the default)
//   INT_RouteBase         points already reached and removed (display numbers continue after them)
//   INT_RouteNext         index of the next point when passed points are kept
//   INT_Route[]           { INT_X, INT_Y, INT_Z, INT_Name } in route order (≤ 50, names ≤ 64 B)
//   INT_3DScalePct        size of everything drawn in 3D, % (50..200, step 10; missing / 0 = 100) - 2026-11-12
//   INT_3DFadePct         transparency of it, % (0..80, step 10; missing = 0; never fully invisible)
//   INT_MarkerModes[]     { INT_Key, INT_Mode } player's mode of SERVER / EVENT markers (chips in «МЕТКИ НА КАРТЕ»,
//                         2026-11-12): mode 0 = 3D, 1 = 2D, 2 = hidden; ≤ 500 (oldest dropped), missing = none.
//                         Key = server address:port + marker: own server marker "S|srv|name|x,z" (position rounded
//                         to 1 m), RF / other event "E|srv|name|icon" (events move in place: no position). «Мои» keep
//                         their mode in MapMarkersCache.json (the marker's own 2D / 3D flags), never here.
//   Minimap (2026-11-13, concept A «ПЛАНШЕТКА», INT_VPP_MiniMap.c) - additive; read only when INT_MiniVer = 1 (an
//   older file without them = all minimap defaults); every value clamped / snapped on load:
//   INT_MiniVer           1 = the minimap fields below were written by this build
//   INT_MiniOff           1 = minimap off (key LCtrl + Num / since 2026-11-14, «\» before / the «Миникарта» row;
//                         missing / 0 = on, the default)
//   INT_MiniKeyMig        1 = the one-time move of the show / hide key from the old default «\» to LCtrl + Num / ran
//                         for this profile (2026-11-14, MissionGameplay.INT_VPP_MigrateMiniKey; missing / 0 = not yet)
//   INT_MiniSizePct       size % (60..160, step 10; missing / 0 = 100)
//   (INT_MiniFadePct of the first build of 2026-11-13 is gone: the owner removed the transparency setting because the
//   engine draws a MapWidget opaque whatever its alpha. An old file that still has the key loads as before - the
//   reader ignores unknown keys - and the next save no longer writes it.)
//   INT_MiniZoom          zoom step 0..4 = view width 200 / 350 / 500 / 750 / 1000 m at 100 % (default 1 = 350 m)
//   INT_MiniPlace         0 top-left, 1 top-right (default), 2 middle-left, 3 middle-right, 4 bottom-left,
//                         5 bottom-right, 6 own place (INT_MiniX / INT_MiniY = 0..1 of the free room)
//   2026-11-14 (additive, missing = the old behaviour): INT_3DNamesOff / INT_3DDistOff 1 = no name / no metres text
//   next to the 3D markers (the icon stays; «ВИД 3D-МЕТОК»); INT_RouteHudOff 1 = no in-game route window (the
//   «МАРШРУТ» ribbon at the top; the route, its 3D beacon and the minimap line keep working); INT_PanelPos[] =
//   { INT_Key, INT_X, INT_Y } where the player dragged a big-map panel (design px of the map screen, clamped on use;
//   ≤ INT_PANEL_MAX, missing = the default place).
// Both look values are floats in the file so a hand-edited 9999 / -5 / 12.5 still parses; they are clamped and
// snapped on load (an old file without them loads unchanged and gets them at the next save).
// Defaults: all layers on, 3D on, distances = server caps (INT_VPP_Client settings; INT_VPP_Net defaults until
// the server answered), size 100 %, transparency 0 % (= the look before 2026-11-12). Setters do NOT write the file: call INT_Save() (MAP: on slider release / toggle); a
// dirty state is also written at mission finish. m_INT_OnChanged.Invoke() (no args) after any change, also
// when the server caps arrive.
// 3_Game. Client only in practice; on the dedicated server it never touches files.
// ============================================================================

const int INT_VPP_LAYER_SERVER = 0;
const int INT_VPP_LAYER_MINE = 1;
const int INT_VPP_LAYER_EVENTS = 2;
const int INT_VPP_LAYER_ZONES = 3;
const int INT_VPP_LAYER_ROUTE = 4;
const int INT_VPP_LAYER_COUNT = 5;

// map mode of one marker (2026-11-12, chips of «МЕТКИ НА КАРТЕ»): 3D = map + world, 2D = map only, hidden
const int INT_VPP_MODE_3D = 0;
const int INT_VPP_MODE_2D = 1;
const int INT_VPP_MODE_HIDDEN = 2;
// stored overrides of server / event markers: at most this many (the oldest go), keys at most this long
const int INT_VPP_MODES_MAX = 500;
const int INT_VPP_MODE_KEY_MAX = 400;

// one override of a server / event marker's mode in Client.json (key: INT_VPP_Prefs.INT_ServerKey / INT_EventKey)
class INT_VPP_ModeEntry
{
	string INT_Key;
	int INT_Mode;
}

class INT_VPP_PanelPos
{
	string INT_Key;
	float INT_X;
	float INT_Y;
}

class INT_VPP_RouteFilePoint
{
	float INT_X;
	float INT_Y;
	float INT_Z;
	string INT_Name;
}

class INT_VPP_ClientFile
{
	int INT_FileVersion;
	ref array<int> INT_LayersOff;
	int INT_3DOff;
	ref array<float> INT_Dist3D;
	string INT_RouteWorld;
	int INT_RouteKeepPassed;
	int INT_RouteBase;
	int INT_RouteNext;
	ref array<ref INT_VPP_RouteFilePoint> INT_Route;
	float INT_3DScalePct;
	float INT_3DFadePct;
	ref array<ref INT_VPP_ModeEntry> INT_MarkerModes;
	// minimap (2026-11-13): additive, see the header
	int INT_MiniVer;
	int INT_MiniOff;
	float INT_MiniSizePct;
	float INT_MiniZoom;
	float INT_MiniPlace;
	float INT_MiniX;
	float INT_MiniY;
	// 2026-11-14: 3D texts, the in-game route window, dragged map panels (additive, see the header)
	int INT_3DNamesOff;
	int INT_3DDistOff;
	int INT_RouteHudOff;
	ref array<ref INT_VPP_PanelPos> INT_PanelPos;
	int INT_MiniKeyMig;

	void INT_VPP_ClientFile()
	{
		INT_LayersOff = new array<int>();
		INT_Dist3D = new array<float>();
		INT_Route = new array<ref INT_VPP_RouteFilePoint>();
		INT_MarkerModes = new array<ref INT_VPP_ModeEntry>();
		INT_PanelPos = new array<ref INT_VPP_PanelPos>();
		INT_3DScalePct = INT_VPP_Prefs.INT_SCALE_DEF;
		INT_3DFadePct = INT_VPP_Prefs.INT_FADE_DEF;
		INT_MiniVer = 0;
		INT_MiniOff = 0;
		INT_MiniSizePct = INT_VPP_Prefs.INT_MINI_SIZE_DEF;
		INT_MiniZoom = INT_VPP_Prefs.INT_MINI_ZOOM_DEF;
		INT_MiniPlace = INT_VPP_Prefs.INT_MINI_PLACE_DEF;
		INT_MiniX = 1.0;
		INT_MiniY = 0.0;
	}

	// after a load: no null arrays, bounded sizes, no NaN, clipped names
	void INT_Sanitize()
	{
		if (!INT_LayersOff)
			INT_LayersOff = new array<int>();
		if (!INT_Dist3D)
			INT_Dist3D = new array<float>();
		if (!INT_Route)
			INT_Route = new array<ref INT_VPP_RouteFilePoint>();
		while (INT_LayersOff.Count() > INT_VPP_LAYER_COUNT)
		{
			INT_LayersOff.Remove(INT_LayersOff.Count() - 1);
		}
		while (INT_Dist3D.Count() > INT_VPP_LAYER_COUNT)
		{
			INT_Dist3D.Remove(INT_Dist3D.Count() - 1);
		}
		array<ref INT_VPP_RouteFilePoint> keep = new array<ref INT_VPP_RouteFilePoint>();
		int n = INT_Route.Count();
		for (int i = 0; i < n; i++)
		{
			INT_VPP_RouteFilePoint p = INT_Route.Get(i);
			if (!p)
				continue;
			if (p.INT_X != p.INT_X || p.INT_Z != p.INT_Z || p.INT_Y != p.INT_Y)
				continue;
			if (keep.Count() >= INT_VPP_Net.HARD_MAX_ROUTE_POINTS)
				break;
			string rawName = p.INT_Name;
			string cleanName = INT_VPP_Validate.CleanText(rawName, INT_VPP_Net.NAME_MAX_BYTES);
			p.INT_Name = cleanName;
			keep.Insert(p);
		}
		INT_Route = keep;
		// marker modes: valid key + mode only, at most INT_VPP_MODES_MAX (the last ones in the file = the newest)
		if (!INT_MarkerModes)
			INT_MarkerModes = new array<ref INT_VPP_ModeEntry>();
		array<ref INT_VPP_ModeEntry> modes = new array<ref INT_VPP_ModeEntry>();
		int mn = INT_MarkerModes.Count();
		for (int m = 0; m < mn; m++)
		{
			INT_VPP_ModeEntry e = INT_MarkerModes.Get(m);
			if (!e)
				continue;
			int kl = e.INT_Key.Length();
			if (kl == 0 || kl > INT_VPP_MODE_KEY_MAX)
				continue;
			if (e.INT_Mode < INT_VPP_MODE_3D || e.INT_Mode > INT_VPP_MODE_HIDDEN)
				continue;
			modes.Insert(e);
		}
		while (modes.Count() > INT_VPP_MODES_MAX)
		{
			modes.RemoveOrdered(0);
		}
		INT_MarkerModes = modes;
		// dragged panels: a key and two real numbers, at most INT_PANEL_MAX
		if (!INT_PanelPos)
			INT_PanelPos = new array<ref INT_VPP_PanelPos>();
		array<ref INT_VPP_PanelPos> pans = new array<ref INT_VPP_PanelPos>();
		foreach (INT_VPP_PanelPos pp : INT_PanelPos)
		{
			if (!pp)
				continue;
			int pkl = pp.INT_Key.Length();
			if (pkl == 0 || pkl > 32)
				continue;
			if (pp.INT_X != pp.INT_X || pp.INT_Y != pp.INT_Y)
				continue;
			if (pans.Count() >= INT_VPP_Prefs.INT_PANEL_MAX)
				break;
			pans.Insert(pp);
		}
		INT_PanelPos = pans;
		if (INT_RouteBase < 0)
			INT_RouteBase = 0;
		if (INT_RouteNext < 0)
			INT_RouteNext = 0;
	}
}

// A valid Client.json (the one-time move's checks, INT_VPP_Files.c): parses into INT_VPP_ClientFile, without a throwing loader.
class INT_VPP_ClientFileCheck : INT_VPP_FileCheck
{
	override bool INT_Parses(string text)
	{
		INT_VPP_ClientFile f = new INT_VPP_ClientFile();
		JsonSerializer js = new JsonSerializer();
		string err;
		bool ok = js.ReadFromString(f, text, err);
		return ok;
	}
}

class INT_VPP_Prefs : Managed
{
	static const string INT_DIR = "$profile:INT_Mods/INT_VppMap";
	static const string INT_PATH = "$profile:INT_Mods/INT_VppMap/Client.json";
	static const string INT_FILE_NAME = "Client.json";
	// before 2026-11-12: moved once into INT_DIR (INT_MigrateOnce)
	static const string INT_OLD_DIR = "$profile:VPPMap_INT";
	static const int INT_FILE_VERSION = 1;
	// look of everything drawn in 3D (markers of all layers + the route beacon), 2026-11-12
	static const int INT_SCALE_MIN = 50;
	static const int INT_SCALE_MAX = 200;
	static const int INT_SCALE_DEF = 100;
	static const int INT_FADE_MIN = 0;
	static const int INT_FADE_MAX = 80;
	static const int INT_FADE_DEF = 0;
	static const int INT_LOOK_STEP = 10;
	// minimap (2026-11-13, owner: concept A in the variant mm_a_2_alt = top-right, 100 %; no transparency setting:
	// removed by the owner the same day, a MapWidget cannot be see-through and our layers always draw at full opacity)
	static const int INT_MINI_VER = 1;
	// dragged big-map panels kept in Client.json (2026-11-14)
	static const int INT_PANEL_MAX = 24;
	static const int INT_MINI_SIZE_MIN = 60;
	static const int INT_MINI_SIZE_MAX = 160;
	static const int INT_MINI_SIZE_DEF = 100;
	static const int INT_MINI_STEP = 10;
	static const int INT_MINI_ZOOMS = 5;
	static const int INT_MINI_ZOOM_DEF = 1;
	static const int INT_MINI_PLACE_TL = 0;
	static const int INT_MINI_PLACE_TR = 1;
	static const int INT_MINI_PLACE_ML = 2;
	static const int INT_MINI_PLACE_MR = 3;
	static const int INT_MINI_PLACE_BL = 4;
	static const int INT_MINI_PLACE_BR = 5;
	static const int INT_MINI_PLACE_OWN = 6;
	static const int INT_MINI_PLACE_DEF = 1;

	protected static ref INT_VPP_Prefs s_INT_Inst;
	// the folder Client.json is read from / written to this session ("" = not decided yet): INT_DIR, or the old
	// folder when the one-time move failed (the next start tries again)
	protected static string s_INT_UseDir;
	protected static int s_INT_MoveResult;

	ref ScriptInvoker m_INT_OnChanged;

	protected ref array<bool> m_INT_LayerOn;
	protected bool m_INT_3DOn;
	protected int m_INT_ScalePct;
	protected int m_INT_FadePct;
	// user distance per layer, < 0 = follow the server cap
	protected ref array<float> m_INT_Dist;
	// server / event marker modes: key → mode, and the keys oldest first (the cap drops from the front)
	protected ref map<string, int> m_INT_Modes;
	protected ref array<string> m_INT_ModeOrder;
	// minimap (2026-11-13)
	protected bool m_INT_MiniOn;
	protected int m_INT_MiniSize;
	protected int m_INT_MiniZoom;
	protected int m_INT_MiniPlace;
	protected float m_INT_MiniX;
	protected float m_INT_MiniY;
	protected int m_INT_MiniRev;
	// 2026-11-14: 3D texts, the in-game route window, dragged map panels (key -> design x, y)
	protected bool m_INT_3DNamesOn = true;
	protected bool m_INT_3DDistOn = true;
	protected bool m_INT_RouteHudOn = true;
	protected ref map<string, vector> m_INT_PanelPos;
	// the show / hide key moved from «\» to LCtrl + Num / once (2026-11-14)
	protected int m_INT_MiniKeyMig;
	protected bool m_INT_Dirty;
	protected ref INT_VPP_ClientFile m_INT_File;
	// weak: the client cache whose OnSettings we listen to (rebound when the mission's instance changes)
	protected INT_VPP_Client m_INT_BoundClient;

	void INT_VPP_Prefs()
	{
		m_INT_OnChanged = new ScriptInvoker();
		m_INT_LayerOn = new array<bool>();
		m_INT_Dist = new array<float>();
		m_INT_Modes = new map<string, int>();
		m_INT_ModeOrder = new array<string>();
		m_INT_PanelPos = new map<string, vector>();
		for (int i = 0; i < INT_VPP_LAYER_COUNT; i++)
		{
			m_INT_LayerOn.Insert(true);
			m_INT_Dist.Insert(-1.0);
		}
		m_INT_3DOn = true;
		m_INT_ScalePct = INT_SCALE_DEF;
		m_INT_FadePct = INT_FADE_DEF;
		INT_MiniDefaults(true);
		INT_Load();
	}

	void ~INT_VPP_Prefs()
	{
		INT_Unbind();
	}

	// ------------------------------------------------------------------ singleton
	static INT_VPP_Prefs INT_Get()
	{
		if (!s_INT_Inst)
			s_INT_Inst = new INT_VPP_Prefs();
		s_INT_Inst.INT_BindClient();
		return s_INT_Inst;
	}

	// no creation (null when nobody asked yet / after INT_Shutdown)
	static INT_VPP_Prefs INT_Peek()
	{
		return s_INT_Inst;
	}

	// One-time move $profile:VPPMap_INT/Client.json -> $profile:INT_Mods/INT_VppMap/Client.json (client only; called
	// from MissionGameplay's constructor and before the first read). Once per game process; later calls do nothing.
	static void INT_MigrateOnce()
	{
		if (s_INT_UseDir != "")
			return;
		s_INT_UseDir = INT_DIR;
		s_INT_MoveResult = INT_VPP_FileMove.INT_RES_NOTHING;
		if (GetGame() && GetGame().IsDedicatedServer())
			return;
		array<string> names = new array<string>();
		names.Insert(INT_FILE_NAME);
		INT_VPP_ClientFileCheck check = new INT_VPP_ClientFileCheck();
		int res = INT_VPP_FileMove.INT_MoveGroup(INT_OLD_DIR, INT_DIR, names, INT_FILE_NAME, check);
		s_INT_MoveResult = res;
		if (res == INT_VPP_FileMove.INT_RES_FAILED)
			s_INT_UseDir = INT_OLD_DIR;
	}

	// Folder / path of Client.json for this session (the new place, or the old one after a failed move).
	static string INT_FileDir()
	{
		INT_VPP_Prefs.INT_MigrateOnce();
		return s_INT_UseDir;
	}

	static string INT_FilePath()
	{
		string dir = INT_VPP_Prefs.INT_FileDir();
		string path = dir + "/" + INT_FILE_NAME;
		return path;
	}

	// TEST: result of this process's move (INT_VPP_FileMove.INT_RES_*)
	static int INT_MoveResult()
	{
		INT_VPP_Prefs.INT_MigrateOnce();
		return s_INT_MoveResult;
	}

	// mission finish: write a dirty state, drop subscribers, forget the instance
	static void INT_Shutdown()
	{
		INT_VPP_Prefs inst = s_INT_Inst;
		if (inst)
		{
			if (inst.m_INT_Dirty)
				inst.INT_WriteFile();
			inst.INT_Unbind();
			if (inst.m_INT_OnChanged)
				inst.m_INT_OnChanged.Clear();
		}
		s_INT_Inst = null;
	}

	// ------------------------------------------------------------------ §8 API
	bool INT_IsLayerOn(int layer)
	{
		if (layer < 0 || layer >= INT_VPP_LAYER_COUNT)
			return false;
		return m_INT_LayerOn.Get(layer);
	}

	void INT_SetLayerOn(int layer, bool on)
	{
		if (layer < 0 || layer >= INT_VPP_LAYER_COUNT)
			return;
		bool cur = m_INT_LayerOn.Get(layer);
		if (cur == on)
			return;
		m_INT_LayerOn.Set(layer, on);
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	// master 3D switch (the «3D» cell; K stays the temporary DayZGame.TempToggleAll3dMarkers)
	bool INT_Is3DOn()
	{
		return m_INT_3DOn;
	}

	void INT_Set3DOn(bool on)
	{
		if (m_INT_3DOn == on)
			return;
		m_INT_3DOn = on;
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	// metres, clamped to the server cap of the category (0 = no 3D for it)
	float INT_Dist3D(int layer)
	{
		if (layer < 0 || layer >= INT_VPP_LAYER_COUNT)
			return 0;
		float cap = INT_Cap3D(layer);
		float user = m_INT_Dist.Get(layer);
		if (user < 0)
			return cap;
		if (user > cap)
			return cap;
		return user;
	}

	void INT_SetDist3D(int layer, float m)
	{
		if (layer < 0 || layer >= INT_VPP_LAYER_COUNT)
			return;
		if (m != m)
			return;
		float cap = INT_Cap3D(layer);
		float v = m;
		if (v < 0)
			v = 0;
		float store = v;
		// at (or past) the cap: follow the cap, also when the server raises it later
		if (v >= cap)
			store = -1.0;
		float cur = m_INT_Dist.Get(layer);
		if (cur == store)
			return;
		m_INT_Dist.Set(layer, store);
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	// size of everything drawn in 3D (markers of all layers + the route beacon), % 50..200, step 10, default 100
	int INT_Get3DScalePct()
	{
		return m_INT_ScalePct;
	}

	void INT_Set3DScalePct(int pct)
	{
		int v = INT_VPP_Prefs.INT_ClampScalePct(pct);
		if (v == m_INT_ScalePct)
			return;
		m_INT_ScalePct = v;
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	// transparency of it, % 0..80, step 10, default 0 (never fully invisible)
	int INT_Get3DFadePct()
	{
		return m_INT_FadePct;
	}

	void INT_Set3DFadePct(int pct)
	{
		int v = INT_VPP_Prefs.INT_ClampFadePct(pct);
		if (v == m_INT_FadePct)
			return;
		m_INT_FadePct = v;
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	// name / metres text next to every 3D marker (2026-11-14, «ВИД 3D-МЕТОК»; off = the icon only). The names of the
	// player's own markers also follow the marker's own «Название в 3D» (MarkerInfo.INT_IsName3DHidden)
	bool INT_Is3DNamesOn()
	{
		return m_INT_3DNamesOn;
	}

	void INT_Set3DNamesOn(bool on)
	{
		if (on == m_INT_3DNamesOn)
			return;
		m_INT_3DNamesOn = on;
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	bool INT_Is3DDistOn()
	{
		return m_INT_3DDistOn;
	}

	void INT_Set3DDistOn(bool on)
	{
		if (on == m_INT_3DDistOn)
			return;
		m_INT_3DDistOn = on;
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	// «Сбросить» of «ВИД 3D-МЕТОК»: size 100 %, transparency 0 %, names and metres on; one change event
	void INT_Reset3DLook()
	{
		bool same = true;
		if (m_INT_ScalePct != INT_SCALE_DEF || m_INT_FadePct != INT_FADE_DEF)
			same = false;
		if (!m_INT_3DNamesOn || !m_INT_3DDistOn)
			same = false;
		if (same)
			return;
		m_INT_ScalePct = INT_SCALE_DEF;
		m_INT_FadePct = INT_FADE_DEF;
		m_INT_3DNamesOn = true;
		m_INT_3DDistOn = true;
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	// the in-game route window (the «МАРШРУТ» ribbon at the top while the map is closed), 2026-11-14
	bool INT_IsRouteHudOn()
	{
		return m_INT_RouteHudOn;
	}

	void INT_SetRouteHudOn(bool on)
	{
		if (on == m_INT_RouteHudOn)
			return;
		m_INT_RouteHudOn = on;
		INT_MiniChanged();
	}

	// the one-time move of the minimap show / hide key (2026-11-14): true = done for this profile
	bool INT_IsMiniKeyMigrated()
	{
		if (m_INT_MiniKeyMig == 1)
			return true;
		return false;
	}

	void INT_SetMiniKeyMigrated()
	{
		if (m_INT_MiniKeyMig == 1)
			return;
		m_INT_MiniKeyMig = 1;
		m_INT_Dirty = true;
	}

	// ------------------------------------------------------------------ dragged big-map panels (2026-11-14)
	// design px of the map screen; false = the panel stays at its default place
	bool INT_GetPanelPos(string key, out float x, out float y)
	{
		x = 0;
		y = 0;
		vector v;
		if (!m_INT_PanelPos.Find(key, v))
			return false;
		x = v[0];
		y = v[1];
		return true;
	}

	// kept in memory at once, written at the next INT_Save (the drag end saves)
	void INT_SetPanelPos(string key, float x, float y)
	{
		if (key == "" || x != x || y != y)
			return;
		if (!m_INT_PanelPos.Contains(key) && m_INT_PanelPos.Count() >= INT_PANEL_MAX)
			return;
		vector v = Vector(x, y, 0);
		m_INT_PanelPos.Set(key, v);
		m_INT_Dirty = true;
	}

	void INT_ClearPanelPos(string key)
	{
		if (!m_INT_PanelPos.Contains(key))
			return;
		m_INT_PanelPos.Remove(key);
		m_INT_Dirty = true;
	}

	void INT_ClearAllPanelPos()
	{
		if (m_INT_PanelPos.Count() == 0)
			return;
		m_INT_PanelPos.Clear();
		m_INT_Dirty = true;
	}

	int INT_PanelPosCount()
	{
		return m_INT_PanelPos.Count();
	}

	// both at once (the «Сбросить» button): one change event
	void INT_Set3DLook(int scalePct, int fadePct)
	{
		int sc = INT_VPP_Prefs.INT_ClampScalePct(scalePct);
		int fd = INT_VPP_Prefs.INT_ClampFadePct(fadePct);
		if (sc == m_INT_ScalePct && fd == m_INT_FadePct)
			return;
		m_INT_ScalePct = sc;
		m_INT_FadePct = fd;
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	// multiplier of every design size / offset of a 3D tree (1 = the look before 2026-11-12)
	float INT_3DScaleK()
	{
		float f = m_INT_ScalePct;
		return f / 100.0;
	}

	// multiplier of every widget's own alpha in a 3D tree (1 = opaque as designed)
	float INT_3DAlphaK()
	{
		float f = m_INT_FadePct;
		return 1.0 - f / 100.0;
	}

	// clamp + snap to the step; NaN = the default
	static int INT_ClampScalePct(float v)
	{
		if (v != v)
			return INT_SCALE_DEF;
		float c = v;
		if (c < INT_SCALE_MIN)
			c = INT_SCALE_MIN;
		if (c > INT_SCALE_MAX)
			c = INT_SCALE_MAX;
		float steps = c / INT_LOOK_STEP;
		int r = Math.Round(steps);
		int res = r * INT_LOOK_STEP;
		if (res < INT_SCALE_MIN)
			res = INT_SCALE_MIN;
		if (res > INT_SCALE_MAX)
			res = INT_SCALE_MAX;
		return res;
	}

	static int INT_ClampFadePct(float v)
	{
		if (v != v)
			return INT_FADE_DEF;
		float c = v;
		if (c < INT_FADE_MIN)
			c = INT_FADE_MIN;
		if (c > INT_FADE_MAX)
			c = INT_FADE_MAX;
		float steps = c / INT_LOOK_STEP;
		int r = Math.Round(steps);
		int res = r * INT_LOOK_STEP;
		if (res < INT_FADE_MIN)
			res = INT_FADE_MIN;
		if (res > INT_FADE_MAX)
			res = INT_FADE_MAX;
		return res;
	}

	// the look values of a loaded file: a missing / 0 size (an old file) = the default, everything else clamped
	static void INT_LookFromFile(INT_VPP_ClientFile f, out int scalePct, out int fadePct)
	{
		scalePct = INT_SCALE_DEF;
		fadePct = INT_FADE_DEF;
		if (!f)
			return;
		float rawScale = f.INT_3DScalePct;
		if (rawScale != 0)
			scalePct = INT_VPP_Prefs.INT_ClampScalePct(rawScale);
		float rawFade = f.INT_3DFadePct;
		fadePct = INT_VPP_Prefs.INT_ClampFadePct(rawFade);
	}

	// TEST: reads a Client.json-format file the way INT_Load does (no singleton, no side effects)
	static bool INT_TestReadLook(string path, out int scalePct, out int fadePct, out int layersOff)
	{
		scalePct = -1;
		fadePct = -1;
		layersOff = -1;
		if (!FileExist(path))
			return false;
		INT_VPP_ClientFile f = new INT_VPP_ClientFile();
		string err;
		bool ok = JsonFileLoader<INT_VPP_ClientFile>.LoadFile(path, f, err);
		if (!ok || !f)
			return false;
		f.INT_Sanitize();
		int sc;
		int fd;
		INT_VPP_Prefs.INT_LookFromFile(f, sc, fd);
		scalePct = sc;
		fadePct = fd;
		int off = 0;
		int n = f.INT_LayersOff.Count();
		for (int i = 0; i < n; i++)
		{
			if (f.INT_LayersOff.Get(i) == 1)
				off++;
		}
		layersOff = off;
		return true;
	}

	// ------------------------------------------------------------------ marker modes (server / event markers)
	// the player's mode for a key, -1 = none (the server's flags decide)
	int INT_GetMarkerMode(string key)
	{
		int v;
		if (m_INT_Modes.Find(key, v))
			return v;
		return -1;
	}

	void INT_SetMarkerMode(string key, int mode)
	{
		if (!INT_PutMode(key, mode))
			return;
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	// a whole group at once (one change event; the caller saves once)
	void INT_SetMarkerModes(array<string> keys, int mode)
	{
		if (!keys)
			return;
		bool any = false;
		foreach (string k : keys)
		{
			if (INT_PutMode(k, mode))
				any = true;
		}
		if (!any)
			return;
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	int INT_MarkerModeCount()
	{
		return m_INT_ModeOrder.Count();
	}

	// a server marker renamed / moved by an admin (2026-11-14): its override goes to the new key; true = moved
	bool INT_MoveMarkerMode(string oldKey, string newKey)
	{
		if (oldKey == "" || newKey == "" || oldKey == newKey)
			return false;
		int mode;
		if (!m_INT_Modes.Find(oldKey, mode))
			return false;
		m_INT_Modes.Remove(oldKey);
		int at = m_INT_ModeOrder.Find(oldKey);
		if (at >= 0)
			m_INT_ModeOrder.RemoveOrdered(at);
		INT_PutMode(newKey, mode);
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
		return true;
	}

	// own server marker: server + name + position rounded to 1 m
	static string INT_ServerKey(string server, MarkerInfo m)
	{
		if (!m)
			return "";
		vector p = m.GetPosition();
		int px = Math.Round(p[0]);
		int pz = Math.Round(p[2]);
		string nm = m.GetName();
		string key = "S|" + server + "|" + nm + "|" + px.ToString() + "," + pz.ToString();
		return key;
	}

	// RF / other event: server + name + icon (events move in place, so no position)
	static string INT_EventKey(string server, MarkerInfo m)
	{
		if (!m)
			return "";
		string nm = m.GetName();
		string ic = m.GetIconPath();
		string key = "E|" + server + "|" + nm + "|" + ic;
		return key;
	}

	// the server's flags are the cap: no 3D → 2D, no map → hidden
	static int INT_CapMode(int mode, bool can3, bool can2)
	{
		int r = mode;
		if (r == INT_VPP_MODE_3D && !can3)
			r = INT_VPP_MODE_2D;
		if (r == INT_VPP_MODE_2D && !can2)
			r = INT_VPP_MODE_HIDDEN;
		return r;
	}

	// next mode of the cycle 3D → 2D → hidden → 3D, skipping what the server does not allow (cur < 0 = mixed: 3D first)
	static int INT_NextMode(int cur, bool can3, bool can2)
	{
		int at = cur;
		if (at < 0 || at > INT_VPP_MODE_HIDDEN)
			at = -1;
		for (int s = 1; s <= 3; s++)
		{
			int c = at + s;
			if (c > INT_VPP_MODE_HIDDEN)
				c = c - 3;
			if (c == INT_VPP_MODE_3D && !can3)
				continue;
			if (c == INT_VPP_MODE_2D && !can2)
				continue;
			return c;
		}
		return INT_VPP_MODE_HIDDEN;
	}

	// true = something changed (new key, other mode); the cap drops the oldest keys
	protected bool INT_PutMode(string key, int mode)
	{
		int kl = key.Length();
		if (kl == 0 || kl > INT_VPP_MODE_KEY_MAX)
			return false;
		if (mode < INT_VPP_MODE_3D || mode > INT_VPP_MODE_HIDDEN)
			return false;
		int cur;
		if (m_INT_Modes.Find(key, cur))
		{
			if (cur == mode)
				return false;
			int at = m_INT_ModeOrder.Find(key);
			if (at >= 0)
				m_INT_ModeOrder.RemoveOrdered(at);
		}
		m_INT_Modes.Set(key, mode);
		m_INT_ModeOrder.Insert(key);
		while (m_INT_ModeOrder.Count() > INT_VPP_MODES_MAX)
		{
			string old = m_INT_ModeOrder.Get(0);
			m_INT_ModeOrder.RemoveOrdered(0);
			m_INT_Modes.Remove(old);
		}
		return true;
	}

	// TEST: the marker modes of a Client.json-format file the way INT_Load reads them
	static int INT_TestReadModes(string path)
	{
		if (!FileExist(path))
			return -1;
		INT_VPP_ClientFile f = new INT_VPP_ClientFile();
		string err;
		bool ok = JsonFileLoader<INT_VPP_ClientFile>.LoadFile(path, f, err);
		if (!ok || !f)
			return -1;
		f.INT_Sanitize();
		int n = f.INT_MarkerModes.Count();
		return n;
	}

	// ------------------------------------------------------------------ minimap (2026-11-13, INT_VPP_MiniMap.c)
	// Every setter clamps / snaps, marks dirty, bumps INT_MiniRev and fires m_INT_OnChanged (nothing when the value
	// does not change). The setters never write the file: the panel / the keys call INT_Save().
	bool INT_MiniIsOn()
	{
		return m_INT_MiniOn;
	}

	void INT_MiniSetOn(bool on)
	{
		if (m_INT_MiniOn == on)
			return;
		m_INT_MiniOn = on;
		INT_MiniChanged();
	}

	int INT_MiniGetSizePct()
	{
		return m_INT_MiniSize;
	}

	void INT_MiniSetSizePct(int pct)
	{
		int v = INT_VPP_Prefs.INT_MiniClampSize(pct);
		if (v == m_INT_MiniSize)
			return;
		m_INT_MiniSize = v;
		INT_MiniChanged();
	}

	// zoom step 0..4 (0 = closest, 200 m wide at 100 %)
	int INT_MiniGetZoom()
	{
		return m_INT_MiniZoom;
	}

	void INT_MiniSetZoom(int step)
	{
		int v = INT_VPP_Prefs.INT_MiniClampZoom(step);
		if (v == m_INT_MiniZoom)
			return;
		m_INT_MiniZoom = v;
		INT_MiniChanged();
	}

	// 0..5 = the six places of the schematic, 6 = own place (INT_MiniGetX / INT_MiniGetY)
	int INT_MiniGetPlace()
	{
		return m_INT_MiniPlace;
	}

	void INT_MiniSetPlace(int place)
	{
		int v = INT_VPP_Prefs.INT_MiniClampPlace(place);
		if (v == m_INT_MiniPlace)
			return;
		m_INT_MiniPlace = v;
		INT_MiniChanged();
	}

	// own place: fx / fy = 0..1 of the free room (0 = left / top edge margin, 1 = right / bottom edge margin)
	float INT_MiniGetX()
	{
		return m_INT_MiniX;
	}

	float INT_MiniGetY()
	{
		return m_INT_MiniY;
	}

	void INT_MiniSetOwnPlace(float fx, float fy)
	{
		float x = INT_VPP_Prefs.INT_MiniClampFrac(fx, 1.0);
		float y = INT_VPP_Prefs.INT_MiniClampFrac(fy, 0.0);
		bool same = true;
		if (m_INT_MiniPlace != INT_MINI_PLACE_OWN)
			same = false;
		if (Math.AbsFloat(x - m_INT_MiniX) > 0.0005 || Math.AbsFloat(y - m_INT_MiniY) > 0.0005)
			same = false;
		if (same)
			return;
		m_INT_MiniPlace = INT_MINI_PLACE_OWN;
		m_INT_MiniX = x;
		m_INT_MiniY = y;
		INT_MiniChanged();
	}

	// «Сбросить»: size, zoom and place back to the defaults (on / off untouched), one event
	void INT_MiniReset()
	{
		bool same = true;
		if (m_INT_MiniSize != INT_MINI_SIZE_DEF || !m_INT_RouteHudOn)
			same = false;
		if (m_INT_MiniZoom != INT_MINI_ZOOM_DEF || m_INT_MiniPlace != INT_MINI_PLACE_DEF)
			same = false;
		if (same)
			return;
		INT_MiniDefaults(false);
		INT_MiniChanged();
	}

	// +1 on every minimap change (the minimap polls it; cheaper than a subscription per frame)
	int INT_MiniRev()
	{
		return m_INT_MiniRev;
	}

	// view width in metres of a zoom step at 100 % size (the window is 300 design px wide there)
	static int INT_MiniZoomMetres(int step)
	{
		int s = INT_VPP_Prefs.INT_MiniClampZoom(step);
		if (s == 0)
			return 200;
		if (s == 1)
			return 350;
		if (s == 2)
			return 500;
		if (s == 3)
			return 750;
		return 1000;
	}

	static int INT_MiniClampSize(float v)
	{
		if (v != v)
			return INT_MINI_SIZE_DEF;
		float c = v;
		if (c < INT_MINI_SIZE_MIN)
			c = INT_MINI_SIZE_MIN;
		if (c > INT_MINI_SIZE_MAX)
			c = INT_MINI_SIZE_MAX;
		float steps = c / INT_MINI_STEP;
		int r = Math.Round(steps);
		int res = r * INT_MINI_STEP;
		if (res < INT_MINI_SIZE_MIN)
			res = INT_MINI_SIZE_MIN;
		if (res > INT_MINI_SIZE_MAX)
			res = INT_MINI_SIZE_MAX;
		return res;
	}

	static int INT_MiniClampZoom(float v)
	{
		if (v != v)
			return INT_MINI_ZOOM_DEF;
		int r = Math.Round(v);
		if (r < 0)
			r = 0;
		if (r > INT_MINI_ZOOMS - 1)
			r = INT_MINI_ZOOMS - 1;
		return r;
	}

	static int INT_MiniClampPlace(float v)
	{
		if (v != v)
			return INT_MINI_PLACE_DEF;
		int r = Math.Round(v);
		if (r < 0 || r > INT_MINI_PLACE_OWN)
			return INT_MINI_PLACE_DEF;
		return r;
	}

	static float INT_MiniClampFrac(float v, float def)
	{
		if (v != v)
			return def;
		float c = v;
		if (c < 0)
			c = 0;
		if (c > 1)
			c = 1;
		return c;
	}

	// the minimap values of a loaded file (an older file without them = all defaults, everything clamped)
	static void INT_MiniFromFile(INT_VPP_ClientFile f, out bool on, out int sizePct, out int zoom, out int place, out float fx, out float fy)
	{
		on = true;
		sizePct = INT_MINI_SIZE_DEF;
		zoom = INT_MINI_ZOOM_DEF;
		place = INT_MINI_PLACE_DEF;
		fx = 1.0;
		fy = 0.0;
		if (!f || f.INT_MiniVer != INT_MINI_VER)
			return;
		if (f.INT_MiniOff == 1)
			on = false;
		float rawSize = f.INT_MiniSizePct;
		if (rawSize != 0)
			sizePct = INT_VPP_Prefs.INT_MiniClampSize(rawSize);
		float rawZoom = f.INT_MiniZoom;
		zoom = INT_VPP_Prefs.INT_MiniClampZoom(rawZoom);
		float rawPlace = f.INT_MiniPlace;
		place = INT_VPP_Prefs.INT_MiniClampPlace(rawPlace);
		float rawX = f.INT_MiniX;
		float rawY = f.INT_MiniY;
		fx = INT_VPP_Prefs.INT_MiniClampFrac(rawX, 1.0);
		fy = INT_VPP_Prefs.INT_MiniClampFrac(rawY, 0.0);
	}

	// TEST: reads a Client.json-format file the way INT_Load does (no singleton, no side effects); false = no file
	static bool INT_TestReadMini(string path, out bool on, out int sizePct, out int zoom, out int place, out float fx, out float fy)
	{
		on = true;
		sizePct = -1;
		zoom = -1;
		place = -1;
		fx = -1;
		fy = -1;
		if (!FileExist(path))
			return false;
		INT_VPP_ClientFile f = new INT_VPP_ClientFile();
		string err;
		bool ok = JsonFileLoader<INT_VPP_ClientFile>.LoadFile(path, f, err);
		if (!ok || !f)
			return false;
		f.INT_Sanitize();
		bool mOn;
		int mSize;
		int mZoom;
		int mPlace;
		float mX;
		float mY;
		INT_VPP_Prefs.INT_MiniFromFile(f, mOn, mSize, mZoom, mPlace, mX, mY);
		on = mOn;
		sizePct = mSize;
		zoom = mZoom;
		place = mPlace;
		fx = mX;
		fy = mY;
		return true;
	}

	// TEST: Client.json read again into this instance, as at the start of a session (one minimap event after it)
	void INT_TestReload()
	{
		INT_Load();
		INT_MiniChanged();
	}

	protected void INT_MiniDefaults(bool alsoOn)
	{
		if (alsoOn)
			m_INT_MiniOn = true;
		m_INT_RouteHudOn = true;
		m_INT_MiniSize = INT_MINI_SIZE_DEF;
		m_INT_MiniZoom = INT_MINI_ZOOM_DEF;
		m_INT_MiniPlace = INT_MINI_PLACE_DEF;
		m_INT_MiniX = 1.0;
		m_INT_MiniY = 0.0;
	}

	protected void INT_MiniChanged()
	{
		m_INT_MiniRev++;
		m_INT_Dirty = true;
		m_INT_OnChanged.Invoke();
	}

	// writes Client.json now (preferences + the route's section)
	void INT_Save()
	{
		INT_WriteFile();
	}

	// ------------------------------------------------------------------ extras
	// server cap of a layer in metres (0 = no 3D for it; zones never have 3D)
	float INT_Cap3D(int layer)
	{
		INT_VPP_ClientSettings s = null;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (cl)
			s = cl.GetSettings();
		int cap = 0;
		if (layer == INT_VPP_LAYER_SERVER)
		{
			cap = INT_VPP_Net.DEF_DIST3D_SERVER;
			if (s)
				cap = s.Dist3DServerMarkers;
		}
		else if (layer == INT_VPP_LAYER_MINE)
		{
			cap = INT_VPP_Net.DEF_DIST3D_MINE;
			if (s)
				cap = s.Dist3DMyMarkers;
		}
		else if (layer == INT_VPP_LAYER_EVENTS)
		{
			cap = INT_VPP_Net.DEF_DIST3D_EVENTS;
			if (s)
				cap = s.Dist3DEvents;
		}
		else if (layer == INT_VPP_LAYER_ROUTE)
		{
			cap = INT_VPP_Net.DEF_DIST3D_ROUTE;
			if (s)
				cap = s.Dist3DRoute;
		}
		cap = INT_VPP_Validate.ClampInt(cap, 0, INT_VPP_Net.HARD_MAX_3D_DIST);
		float f = cap;
		return f;
	}

	bool INT_IsDirty()
	{
		return m_INT_Dirty;
	}

	// the loaded file (INT_VPP_Route reads its section from it); never null
	INT_VPP_ClientFile INT_File()
	{
		if (!m_INT_File)
			m_INT_File = new INT_VPP_ClientFile();
		return m_INT_File;
	}

	// writes the whole file: preferences from this object, the route from INT_VPP_Route if it exists
	// (else the route section as loaded)
	void INT_WriteFile()
	{
		if (GetGame() && GetGame().IsDedicatedServer())
			return;
		INT_VPP_ClientFile f = INT_File();
		f.INT_FileVersion = INT_FILE_VERSION;
		f.INT_LayersOff.Clear();
		f.INT_Dist3D.Clear();
		for (int i = 0; i < INT_VPP_LAYER_COUNT; i++)
		{
			int off = 0;
			if (!m_INT_LayerOn.Get(i))
				off = 1;
			f.INT_LayersOff.Insert(off);
			f.INT_Dist3D.Insert(m_INT_Dist.Get(i));
		}
		int off3d = 0;
		if (!m_INT_3DOn)
			off3d = 1;
		f.INT_3DOff = off3d;
		float scaleOut = m_INT_ScalePct;
		float fadeOut = m_INT_FadePct;
		f.INT_3DScalePct = scaleOut;
		f.INT_3DFadePct = fadeOut;
		int namesOff = 0;
		if (!m_INT_3DNamesOn)
			namesOff = 1;
		f.INT_3DNamesOff = namesOff;
		int distOff = 0;
		if (!m_INT_3DDistOn)
			distOff = 1;
		f.INT_3DDistOff = distOff;
		int hudOff = 0;
		if (!m_INT_RouteHudOn)
			hudOff = 1;
		f.INT_RouteHudOff = hudOff;
		f.INT_MiniKeyMig = m_INT_MiniKeyMig;
		f.INT_PanelPos.Clear();
		foreach (string pk, vector pv : m_INT_PanelPos)
		{
			INT_VPP_PanelPos pe = new INT_VPP_PanelPos();
			pe.INT_Key = pk;
			pe.INT_X = pv[0];
			pe.INT_Y = pv[1];
			f.INT_PanelPos.Insert(pe);
		}
		f.INT_MarkerModes.Clear();
		foreach (string mk : m_INT_ModeOrder)
		{
			int mv;
			if (!m_INT_Modes.Find(mk, mv))
				continue;
			INT_VPP_ModeEntry me = new INT_VPP_ModeEntry();
			me.INT_Key = mk;
			me.INT_Mode = mv;
			f.INT_MarkerModes.Insert(me);
		}
		// minimap (2026-11-13)
		f.INT_MiniVer = INT_MINI_VER;
		int miniOff = 0;
		if (!m_INT_MiniOn)
			miniOff = 1;
		f.INT_MiniOff = miniOff;
		float miniSize = m_INT_MiniSize;
		float miniZoom = m_INT_MiniZoom;
		float miniPlace = m_INT_MiniPlace;
		f.INT_MiniSizePct = miniSize;
		f.INT_MiniZoom = miniZoom;
		f.INT_MiniPlace = miniPlace;
		f.INT_MiniX = m_INT_MiniX;
		f.INT_MiniY = m_INT_MiniY;
		INT_VPP_Route rt = INT_VPP_Route.INT_Peek();
		if (rt)
			rt.INT_FillFile(f);
		string dir = INT_VPP_Prefs.INT_FileDir();
		INT_VPP_FileMove.INT_EnsureDir(dir);
		string path = INT_VPP_Prefs.INT_FilePath();
		string err;
		bool ok = JsonFileLoader<INT_VPP_ClientFile>.SaveFile(path, f, err);
		if (!ok)
			Print("[INT_VPP] Client.json not written: " + INT_VPP_Validate.LogText(err));
		m_INT_Dirty = false;
	}

	// ------------------------------------------------------------------ internals
	protected void INT_Load()
	{
		INT_VPP_ClientFile f = new INT_VPP_ClientFile();
		bool server = false;
		if (GetGame() && GetGame().IsDedicatedServer())
			server = true;
		// the one-time move of the old file comes first (client only; nothing on the server)
		string path = INT_VPP_Prefs.INT_FilePath();
		if (!server && FileExist(path))
		{
			string err;
			bool ok = JsonFileLoader<INT_VPP_ClientFile>.LoadFile(path, f, err);
			if (!ok || !f)
			{
				Print("[INT_VPP] Client.json unreadable, defaults used: " + INT_VPP_Validate.LogText(err));
				// keep the player's broken file for a look, start clean
				string broken = path + ".broken";
				if (FileExist(broken))
					DeleteFile(broken);
				CopyFile(path, broken);
				f = new INT_VPP_ClientFile();
			}
		}
		f.INT_Sanitize();
		m_INT_File = f;
		int nOff = f.INT_LayersOff.Count();
		int nDist = f.INT_Dist3D.Count();
		for (int i = 0; i < INT_VPP_LAYER_COUNT; i++)
		{
			bool on = true;
			if (i < nOff)
			{
				if (f.INT_LayersOff.Get(i) == 1)
					on = false;
			}
			m_INT_LayerOn.Set(i, on);
			float d = -1.0;
			if (i < nDist)
			{
				float fd = f.INT_Dist3D.Get(i);
				if (fd == fd && fd >= 0)
					d = fd;
			}
			m_INT_Dist.Set(i, d);
		}
		bool on3d = true;
		if (f.INT_3DOff == 1)
			on3d = false;
		m_INT_3DOn = on3d;
		int lookScale;
		int lookFade;
		INT_VPP_Prefs.INT_LookFromFile(f, lookScale, lookFade);
		m_INT_ScalePct = lookScale;
		m_INT_FadePct = lookFade;
		bool miniOn;
		int miniSize;
		int miniZoom;
		int miniPlace;
		float miniX;
		float miniY;
		INT_VPP_Prefs.INT_MiniFromFile(f, miniOn, miniSize, miniZoom, miniPlace, miniX, miniY);
		m_INT_MiniOn = miniOn;
		m_INT_MiniSize = miniSize;
		m_INT_MiniZoom = miniZoom;
		m_INT_MiniPlace = miniPlace;
		m_INT_MiniX = miniX;
		m_INT_MiniY = miniY;
		bool namesOn = true;
		if (f.INT_3DNamesOff == 1)
			namesOn = false;
		m_INT_3DNamesOn = namesOn;
		bool distOn = true;
		if (f.INT_3DDistOff == 1)
			distOn = false;
		m_INT_3DDistOn = distOn;
		bool hudOn = true;
		if (f.INT_RouteHudOff == 1)
			hudOn = false;
		m_INT_RouteHudOn = hudOn;
		int keyMig = 0;
		if (f.INT_MiniKeyMig == 1)
			keyMig = 1;
		m_INT_MiniKeyMig = keyMig;
		m_INT_PanelPos.Clear();
		foreach (INT_VPP_PanelPos lp : f.INT_PanelPos)
		{
			if (!lp)
				continue;
			vector lv = Vector(lp.INT_X, lp.INT_Y, 0);
			m_INT_PanelPos.Set(lp.INT_Key, lv);
		}
		m_INT_Modes.Clear();
		m_INT_ModeOrder.Clear();
		foreach (INT_VPP_ModeEntry le : f.INT_MarkerModes)
		{
			if (le)
				INT_PutMode(le.INT_Key, le.INT_Mode);
		}
		m_INT_Dirty = false;
	}

	protected void INT_BindClient()
	{
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (cl == m_INT_BoundClient)
			return;
		INT_Unbind();
		if (!cl)
			return;
		if (cl.OnSettings)
			cl.OnSettings.Insert(this.INT_OnServerSettings);
		m_INT_BoundClient = cl;
	}

	protected void INT_Unbind()
	{
		INT_VPP_Client old = m_INT_BoundClient;
		if (old && old.OnSettings)
			old.OnSettings.Remove(this.INT_OnServerSettings);
		m_INT_BoundClient = null;
	}

	// the caps changed: every clamped value may have changed
	void INT_OnServerSettings()
	{
		if (m_INT_OnChanged)
			m_INT_OnChanged.Invoke();
	}
}
