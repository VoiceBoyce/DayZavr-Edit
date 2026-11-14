// INT_VppMap_SE (server only) — home of the map's server files: $profile:INT_Mods/INT_VppMap/ (2026-11-12, CL/SE split)
//   VPPMapConfig.json                      VPP's server markers + flags (format unchanged)
//   Settings.json (+ .tmp / .bak)          map admins, limits, 3D caps
//   Zones.json (+ .tmp / .bak)             admin zones (+ .broken-* / .dropped-* copies when needed)
// Old places, moved ONCE: $profile:VPPMapConfig.json, $profile:VPPMap_INT/Settings.json (+ .tmp / .bak),
// $profile:VPPMap_INT/Zones.json (+ .tmp / .bak). Rules: VanillaPPMap 3_Game/INT_VPP/INT_VPP_Files.c (a group moves
// together or not at all; old + new: an unfinished move is finished, a valid new file wins, an unreadable new file is
// set aside and replaced by a valid old one; a failed copy / byte check or a file over the size limit = the old files
// stay and are used THIS session, the next start tries again). Old .broken-* / .dropped-* copies have a time stamp in
// their names and stay where they are.
// INT_RunOnce runs before the first read of any of them (ServerMarkersCache in the MissionServer constructor,
// INT_VPP_ServerState.LoadAll in OnInit, whichever comes first), once per server process.
// INT_GuardMarkers runs before EVERY read of VPPMapConfig.json by VPP's own reader (which throws on bad JSON).

// Valid VPPMapConfig.json = parses into VPP's VPPMapConfig (JsonSerializer.ReadFromString: no exception; a bad file
// still leaves one engine "SCRIPT (E): JSON ERROR" line).
class INT_VPP_SE_MarkersCheck : INT_VPP_FileCheck
{
	override bool INT_Parses(string text)
	{
		VPPMapConfig c = new VPPMapConfig();
		JsonSerializer js = new JsonSerializer();
		string err;
		bool ok = js.ReadFromString(c, text, err);
		return ok;
	}
}

class INT_VPP_SE_SettingsCheck : INT_VPP_FileCheck
{
	override bool INT_Parses(string text)
	{
		INT_VPP_ServerSettings s = new INT_VPP_ServerSettings();
		JsonSerializer js = new JsonSerializer();
		string err;
		bool ok = js.ReadFromString(s, text, err);
		return ok;
	}
}

class INT_VPP_SE_ZonesCheck : INT_VPP_FileCheck
{
	override bool INT_Parses(string text)
	{
		INT_VPP_ZonesFile z = new INT_VPP_ZonesFile();
		JsonSerializer js = new JsonSerializer();
		string err;
		bool ok = js.ReadFromString(z, text, err);
		return ok;
	}
}

class INT_VPP_SE_MigResult : Managed
{
	int m_INT_Markers;
	int m_INT_Settings;
	int m_INT_Zones;
	// folders the three groups are read from / written to this session
	string m_INT_MarkersDir;
	string m_INT_SettingsDir;
	string m_INT_ZonesDir;
}

class INT_VPP_SE_Migrate
{
	static const string INT_OLD_MARKERS_DIR = "$profile:";
	static const string INT_OLD_DIR = "$profile:VPPMap_INT";
	static const string INT_MARKERS_NAME = "VPPMapConfig.json";
	static const string INT_SETTINGS_NAME = "Settings.json";
	static const string INT_ZONES_NAME = "Zones.json";
	static const int INT_GUARD_LOG_GAP_MS = 60000;

	protected static bool s_INT_Done;
	protected static ref INT_VPP_SE_MigResult s_INT_Real;
	// TestKit only: ServerMarkersCache reads / writes this file instead ("" = the real one)
	protected static string s_INT_TestMarkersPath;
	protected static bool s_INT_GuardLogged;
	protected static int s_INT_GuardLogAt;
	// TestKit: files set aside by INT_GuardMarkers
	static int s_INT_GuardSetAside;

	// The real server files, once per process.
	static void INT_RunOnce()
	{
		if (s_INT_Done)
			return;
		s_INT_Done = true;
		INT_VPP_SE_MigResult r = INT_VPP_SE_Migrate.INT_Run(INT_OLD_MARKERS_DIR, INT_OLD_DIR, INT_VPP_FileMove.INT_DIR);
		s_INT_Real = r;
		string rm = INT_VPP_FileMove.INT_ResName(r.m_INT_Markers);
		string rs = INT_VPP_FileMove.INT_ResName(r.m_INT_Settings);
		string rz = INT_VPP_FileMove.INT_ResName(r.m_INT_Zones);
		// one expression (Print of a bare string variable logs it as "string line = '...'")
		string tail = " Settings.json=" + rs + " Zones.json=" + rz + ")";
		Print("[INT_VPP] server files in " + INT_VPP_FileMove.INT_DIR + " (move from the old places: VPPMapConfig.json=" + rm + tail);
	}

	// One move of the three groups from the given old folders to newDir (the TestKit calls it on a sandbox).
	static INT_VPP_SE_MigResult INT_Run(string oldMarkersDir, string oldDir, string newDir)
	{
		INT_VPP_FileMove.INT_EnsureDir(newDir);
		INT_VPP_SE_MigResult r = new INT_VPP_SE_MigResult();

		array<string> markers = new array<string>();
		markers.Insert(INT_MARKERS_NAME);
		INT_VPP_SE_MarkersCheck mc = new INT_VPP_SE_MarkersCheck();
		int resM = INT_VPP_FileMove.INT_MoveGroup(oldMarkersDir, newDir, markers, INT_MARKERS_NAME, mc);
		r.m_INT_Markers = resM;
		r.m_INT_MarkersDir = newDir;
		if (resM == INT_VPP_FileMove.INT_RES_FAILED)
			r.m_INT_MarkersDir = oldMarkersDir;

		array<string> settings = new array<string>();
		settings.Insert(INT_SETTINGS_NAME);
		settings.Insert(INT_SETTINGS_NAME + ".tmp");
		settings.Insert(INT_SETTINGS_NAME + ".bak");
		INT_VPP_SE_SettingsCheck sc = new INT_VPP_SE_SettingsCheck();
		int resS = INT_VPP_FileMove.INT_MoveGroup(oldDir, newDir, settings, INT_SETTINGS_NAME, sc);
		r.m_INT_Settings = resS;
		r.m_INT_SettingsDir = newDir;
		if (resS == INT_VPP_FileMove.INT_RES_FAILED)
			r.m_INT_SettingsDir = oldDir;

		array<string> zones = new array<string>();
		zones.Insert(INT_ZONES_NAME);
		zones.Insert(INT_ZONES_NAME + ".tmp");
		zones.Insert(INT_ZONES_NAME + ".bak");
		INT_VPP_SE_ZonesCheck zc = new INT_VPP_SE_ZonesCheck();
		int resZ = INT_VPP_FileMove.INT_MoveGroup(oldDir, newDir, zones, INT_ZONES_NAME, zc);
		r.m_INT_Zones = resZ;
		r.m_INT_ZonesDir = newDir;
		if (resZ == INT_VPP_FileMove.INT_RES_FAILED)
			r.m_INT_ZonesDir = oldDir;
		return r;
	}

	// What the real start did (runs it first if nobody did yet).
	static INT_VPP_SE_MigResult INT_Real()
	{
		INT_VPP_SE_Migrate.INT_RunOnce();
		return s_INT_Real;
	}

	static string INT_MarkersPath()
	{
		if (s_INT_TestMarkersPath != "")
			return s_INT_TestMarkersPath;
		INT_VPP_SE_Migrate.INT_RunOnce();
		string dir = s_INT_Real.m_INT_MarkersDir;
		string path = INT_VPP_FileMove.INT_Join(dir, INT_MARKERS_NAME);
		return path;
	}

	static string INT_SettingsDir()
	{
		INT_VPP_SE_Migrate.INT_RunOnce();
		return s_INT_Real.m_INT_SettingsDir;
	}

	static string INT_ZonesDir()
	{
		INT_VPP_SE_Migrate.INT_RunOnce();
		return s_INT_Real.m_INT_ZonesDir;
	}

	// TestKit: a sandbox VPPMapConfig.json for the next `new ServerMarkersCache()`; "" = the real one again.
	static void INT_SetTestMarkersPath(string path)
	{
		s_INT_TestMarkersPath = path;
	}

	protected static void INT_GuardLog(string text)
	{
		int now = 0;
		if (GetGame())
			now = GetGame().GetTime();
		int gap = now - s_INT_GuardLogAt;
		if (s_INT_GuardLogged && gap >= 0 && gap < INT_GUARD_LOG_GAP_MS)
			return;
		s_INT_GuardLogged = true;
		s_INT_GuardLogAt = now;
		Print("[INT_VPP] ERROR " + text);
	}

	// Before VPP's reader (JsonFileLoader.JsonLoadFile, which throws on bad JSON) reads VPPMapConfig.json: a file that
	// does not parse is set aside as .broken-<stamp> (byte-identical copy first), then VPP writes its defaults.
	// false = NOT safe to read (the bad file could not be set aside): the caller keeps what it has in memory.
	static bool INT_GuardMarkers(string path)
	{
		if (!FileExist(path))
			return true;
		string txt;
		int st = INT_VPP_FileMove.INT_ReadEx(path, txt);
		if (st == INT_VPP_FileMove.INT_READ_TOO_BIG)
			return true;
		if (st != INT_VPP_FileMove.INT_READ_OK)
			return true;
		INT_VPP_SE_MarkersCheck mc = new INT_VPP_SE_MarkersCheck();
		if (mc.INT_Parses(txt))
			return true;
		string kept = INT_VPP_FileMove.INT_SetAside(path, "broken");
		if (kept == "")
		{
			INT_VPP_SE_Migrate.INT_GuardLog(path + " is not valid JSON and could not be set aside: not read (VPP keeps what it had in memory)");
			return false;
		}
		s_INT_GuardSetAside++;
		Print("[INT_VPP] ERROR " + path + " is not valid JSON: set aside as " + kept + ", VPP writes its default markers");
		return true;
	}
}
