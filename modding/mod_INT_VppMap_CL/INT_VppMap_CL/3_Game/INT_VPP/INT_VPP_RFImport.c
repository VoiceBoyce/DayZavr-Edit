// INT_VPP (2026-11-13, owner): one-time import of the player's own RF_MAP markers into our map.
// RF_MAP_CL (RF_MAP_CL_ConfigurationsProvider) keeps a player's markers in
//   $profile:RF/MAP/CACHE/SERVERS/<ip>-<port>.json   with <ip>, <port> = OnlineServices.GetInviteServerInfo(),
//   "" -> "localhost". On PC that info is set only by a console session invite, so RF_MAP_CL reads and writes
//   localhost-0.json on EVERY server: the file is not tied to one server.
// Format (RF_MAP_CL_ServerConfiguration): { "markers": [ { "uuid", "position": [x,y,z], "title", "textColor":
//   {"a","r","g","b"}, "iconPath", "circle": {...} } ], "markersSettings": [ { "uuid", "visibilityState" } ],
//   "isAdminModeEnabled" }; visibilityState 0 hidden, 1 visible3D (also = no settings entry), 2 visible2D.
// Because the file is not tied to a server, the import runs only on a server that says so: Settings.json
// INT_ImportRFMapMarkers 1 (the server that replaced RF_MAP) sends the RPC INT_RFImport after the Hello answer.
// Then, once per session: the RF file (this session's real address first, then RF's own name), read whole (size
// limit), parsed without throwing, the OWN markers converted (name cleaned + clipped like our editor, icon by the
// table below, colour, position, visibility hidden / 2D / 3D -> active / 3D flags), duplicates (same name, 2D
// distance <= 1 m, against this server's list, the global list and the import itself) skipped, at most
// INT_MAX_MARKERS added to VPP's per-server list of THIS server (MapMarkersCache.json, the editor's API, ONE save).
// The server key is remembered in $profile:INT_Mods/INT_VppMap/RFImport.json (own small file); one log line.
// RF's file is only read, never changed or deleted. A bad / too big file: one WARN, nothing recorded, tried again
// next session (at most once per session).
// 3_Game; every internal call qualified (INT_VPP_RFImport.X).

// ---- RF_MAP_CL's JSON (only the keys we read; the engine reader ignores the others) ----
class INT_VPP_RFIColor
{
	int a;
	int r;
	int g;
	int b;
}

class INT_VPP_RFIMarker
{
	string uuid;
	vector position;
	string title;
	ref INT_VPP_RFIColor textColor;
	string iconPath;
}

class INT_VPP_RFISetting
{
	string uuid;
	int visibilityState;
}

class INT_VPP_RFIFile
{
	ref array<ref INT_VPP_RFIMarker> markers;
	ref array<ref INT_VPP_RFISetting> markersSettings;
}

// ---- our record: which servers were imported ($profile:INT_Mods/INT_VppMap/RFImport.json) ----
class INT_VPP_RFIServer
{
	string INT_Server;
	string INT_File;
	int INT_Count;
	string INT_Time;
}

class INT_VPP_RFIState
{
	int INT_FileVersion = 1;
	ref array<ref INT_VPP_RFIServer> INT_Servers = new array<ref INT_VPP_RFIServer>();

	bool INT_Has(string server)
	{
		if (!INT_Servers)
			return false;
		int n = INT_Servers.Count();
		for (int i = 0; i < n; i++)
		{
			INT_VPP_RFIServer s = INT_Servers.Get(i);
			if (s && s.INT_Server == server)
				return true;
		}
		return false;
	}
}

// result of one import attempt (TEST reads every field)
class INT_VPP_RFIResult
{
	int m_INT_Code;
	int m_INT_Added;
	int m_INT_Dups;
	int m_INT_Bad;
	int m_INT_Capped;
	int m_INT_InFile;
	string m_INT_File;
	string m_INT_Detail;
}

class INT_VPP_RFImport
{
	static const string INT_RF_DIR = "$profile:RF/MAP/CACHE/SERVERS";
	static const string INT_STATE_FILE = "$profile:INT_Mods/INT_VppMap/RFImport.json";
	static const int INT_MAX_BYTES = 524288;       // an RF file bigger than this is not read (512 KB ~ 1500 markers)
	static const int INT_MAX_MARKERS = 200;        // at most this many markers per import (VPP itself has no limit)
	static const int INT_MAX_SERVERS = 100;        // RFImport.json keeps the newest records
	static const float INT_DUP_METRES = 1.0;
	static const string INT_DEFAULT_NAME = "Метка";

	static const int INT_RES_NONE = 0;
	static const int INT_RES_IMPORTED = 1;         // markers added (or only duplicates), server recorded
	static const int INT_RES_ALREADY = 2;          // this server was imported in an earlier session
	static const int INT_RES_NO_FILE = 3;          // no RF file: nothing to do (not recorded: a later RF file still counts)
	static const int INT_RES_BAD_FILE = 4;         // unreadable / too big / not RF_MAP's JSON: WARN, not recorded
	static const int INT_RES_EMPTY = 5;            // RF file without own markers: recorded
	static const int INT_RES_NO_SERVER = 6;        // no server key (not connected) or no cache
	static const int INT_RES_SESSION_DONE = 7;     // already tried in this session

	protected static bool s_INT_Tried;
	protected static ref INT_VPP_RFIResult s_INT_Last;
	protected static int s_INT_TestMaxBytes;

	// RPC INT_RFImport from our server (only a server with Settings.json INT_ImportRFMapMarkers 1 sends it)
	static INT_VPP_RFIResult INT_OnServerAllows(int on)
	{
		if (on != 1)
			return null;
		if (!GetGame() || GetGame().IsDedicatedServer())
			return null;
		string server = g_Game.GetConnectAddressPort();
		string rf = INT_VPP_RFImport.INT_FindRFFile();
		g_Game.INT_VPP_LoadCacheOnce();
		ClientMarkersCache cache = g_Game.GetClientMarkersCache();
		string state = INT_VPP_RFImport.INT_STATE_FILE;
		INT_VPP_RFIResult res = INT_VPP_RFImport.INT_SessionRun(server, rf, state, cache);
		return res;
	}

	// once per session (a new MissionGameplay resets it); the result is also kept for the TestKit
	static INT_VPP_RFIResult INT_SessionRun(string server, string rfPath, string statePath, ClientMarkersCache cache)
	{
		if (s_INT_Tried)
		{
			INT_VPP_RFIResult done = new INT_VPP_RFIResult();
			done.m_INT_Code = INT_RES_SESSION_DONE;
			return done;
		}
		s_INT_Tried = true;
		INT_VPP_RFIResult res = INT_VPP_RFImport.INT_RunFor(server, rfPath, statePath, cache);
		s_INT_Last = res;
		INT_VPP_RFImport.INT_Log(res, server);
		return res;
	}

	static void INT_ResetSession()
	{
		s_INT_Tried = false;
	}

	// RF_MAP_CL's own file name on this client right now (ConfigurationsProvider: GetInviteServerInfo, "" = localhost)
	static string INT_RFFileName()
	{
		string ip;
		int port;
		OnlineServices.GetInviteServerInfo(ip, port);
		if (ip == "")
			ip = "localhost";
		string nm = ip + "-" + port.ToString() + ".json";
		return nm;
	}

	// the file of this session's real address (RF uses it when the session came from an invite), else RF's own name
	static string INT_FindRFFile()
	{
		string addr;
		int port;
		bool found = false;
		if (GetGame())
			found = GetGame().GetHostAddress(addr, port);
		if (found && addr != "")
		{
			string real = INT_RF_DIR + "/" + addr + "-" + port.ToString() + ".json";
			if (FileExist(real))
				return real;
		}
		string own = INT_VPP_RFImport.INT_RFFileName();
		string path = INT_RF_DIR + "/" + own;
		return path;
	}

	// the whole import for one server key (no session gate; the TestKit calls it with its own paths and cache)
	static INT_VPP_RFIResult INT_RunFor(string server, string rfPath, string statePath, ClientMarkersCache cache)
	{
		INT_VPP_RFIResult res = new INT_VPP_RFIResult();
		res.m_INT_File = rfPath;
		if (server == "" || !cache)
		{
			res.m_INT_Code = INT_RES_NO_SERVER;
			return res;
		}
		INT_VPP_RFIState st = INT_VPP_RFImport.INT_LoadState(statePath);
		if (st.INT_Has(server))
		{
			res.m_INT_Code = INT_RES_ALREADY;
			return res;
		}
		if (!FileExist(rfPath))
		{
			res.m_INT_Code = INT_RES_NO_FILE;
			return res;
		}
		string txt;
		string why = INT_VPP_RFImport.INT_ReadWhole(rfPath, txt);
		if (why != "")
		{
			res.m_INT_Code = INT_RES_BAD_FILE;
			res.m_INT_Detail = why;
			return res;
		}
		// RF_MAP's file always has the "markers" key; any other JSON is not taken
		if (txt.IndexOf("\"markers\"") < 0)
		{
			res.m_INT_Code = INT_RES_BAD_FILE;
			res.m_INT_Detail = "not an RF_MAP marker file (no markers key)";
			return res;
		}
		INT_VPP_RFIFile f = new INT_VPP_RFIFile();
		JsonSerializer js = new JsonSerializer();
		string err;
		bool parsed = js.ReadFromString(f, txt, err);
		if (!parsed)
		{
			res.m_INT_Code = INT_RES_BAD_FILE;
			string errShort = INT_VPP_Validate.CleanText(err, 120);
			res.m_INT_Detail = "not valid JSON: " + errShort;
			return res;
		}
		array<ref INT_VPP_RFIMarker> list = f.markers;
		int inFile = 0;
		if (list)
			inFile = list.Count();
		res.m_INT_InFile = inFile;
		if (inFile == 0)
		{
			res.m_INT_Code = INT_RES_EMPTY;
			INT_VPP_RFImport.INT_Remember(st, statePath, server, rfPath, 0);
			return res;
		}
		map<string, int> vis = new map<string, int>();
		array<ref INT_VPP_RFISetting> sets = f.markersSettings;
		if (sets)
		{
			int ns = sets.Count();
			for (int si = 0; si < ns; si++)
			{
				INT_VPP_RFISetting one = sets.Get(si);
				if (!one)
					continue;
				if (vis.Contains(one.uuid))
					continue;
				vis.Insert(one.uuid, one.visibilityState);
			}
		}
		array<ref MarkerInfo> target = cache.GetCachedArray(server);
		array<ref MarkerInfo> mine = cache.GetCachedArray();
		float world = INT_VPP_Validate.WorldSize();
		for (int i = 0; i < inFile; i++)
		{
			INT_VPP_RFIMarker m = list.Get(i);
			if (!m)
			{
				res.m_INT_Bad++;
				continue;
			}
			vector p = m.position;
			float x = p[0];
			float z = p[2];
			bool okX = INT_VPP_Validate.InRange(x, 0, world);
			bool okZ = INT_VPP_Validate.InRange(z, 0, world);
			if (!okX || !okZ)
			{
				res.m_INT_Bad++;
				continue;
			}
			string nm = INT_VPP_Validate.CleanText(m.title, INT_VPP_Net.NAME_MAX_BYTES);
			if (nm == "")
				nm = INT_DEFAULT_NAME;
			vector pos = Vector(x, 0, z);
			bool dupT = INT_VPP_RFImport.INT_HasTwin(target, nm, pos);
			bool dupM = INT_VPP_RFImport.INT_HasTwin(mine, nm, pos);
			if (dupT || dupM)
			{
				res.m_INT_Dups++;
				continue;
			}
			if (res.m_INT_Added >= INT_MAX_MARKERS)
			{
				res.m_INT_Capped++;
				continue;
			}
			string icon = INT_VPP_RFImport.INT_IconFor(m.iconPath);
			vector col = INT_VPP_RFImport.INT_ColorOf(m.textColor);
			int state = 1;
			int got;
			if (vis.Find(m.uuid, got))
				state = got;
			bool active = true;
			bool act3d = true;
			if (state == 0)
			{
				active = false;
				act3d = false;
			}
			else if (state == 2)
			{
				act3d = false;
			}
			MarkerInfo made = new MarkerInfo(nm, icon, col, pos, active, act3d);
			target.Insert(made);
			res.m_INT_Added++;
		}
		if (res.m_INT_Added > 0)
		{
			cache.UpdateCachedMarkerInfos(target, server);
			cache.SaveCache();
		}
		res.m_INT_Code = INT_RES_IMPORTED;
		INT_VPP_RFImport.INT_Remember(st, statePath, server, rfPath, res.m_INT_Added);
		return res;
	}

	// "" = the whole file is in txt; else why not
	protected static string INT_ReadWhole(string path, out string txt)
	{
		txt = "";
		FileHandle fh = OpenFile(path, FileMode.READ);
		if (fh == 0)
			return "cannot open";
		int limit = INT_MAX_BYTES;
		if (s_INT_TestMaxBytes > 0)
			limit = s_INT_TestMaxBytes;
		int want = limit + 1;
		string all;
		ReadFile(fh, all, want);
		CloseFile(fh);
		int got = all.Length();
		if (got > limit)
			return "bigger than " + limit.ToString() + " bytes";
		if (got == 0)
			return "empty";
		txt = all;
		return "";
	}

	// same name + 2D distance <= INT_DUP_METRES
	static bool INT_HasTwin(array<ref MarkerInfo> list, string nm, vector pos)
	{
		if (!list)
			return false;
		float lim = INT_DUP_METRES * INT_DUP_METRES;
		int n = list.Count();
		for (int i = 0; i < n; i++)
		{
			MarkerInfo o = list.Get(i);
			if (!o)
				continue;
			string on = o.GetName();
			if (on != nm)
				continue;
			vector op = o.GetPosition();
			float dx = op[0] - pos[0];
			float dz = op[2] - pos[2];
			float d2 = dx * dx + dz * dz;
			if (d2 <= lim)
				return true;
		}
		return false;
	}

	// RF_MAP_CL\GUI\Icons\NN_Name.paa -> one of our icons (owner's table 2026-11-13); our own and old VPP paths stay;
	// anything else -> our default icon
	static string INT_IconFor(string rfPath)
	{
		string p = INT_VPP_Icons.INT_NormPath(rfPath);
		if (p == "")
			return INT_VPP_Icons.INT_DefaultPath();
		if (INT_VPP_Icons.INT_IsOwnPath(p))
		{
			string ownP = INT_VPP_Icons.INT_PathOf(INT_VPP_Icons.INT_IdOfPath(p));
			return ownP;
		}
		if (INT_VPP_Icons.INT_IsLegacyPath(p))
			return rfPath;
		int slash = p.LastIndexOf("/");
		string base = p;
		if (slash >= 0)
		{
			int from = slash + 1;
			int len = p.Length() - from;
			base = p.Substring(from, len);
		}
		int dot = base.LastIndexOf(".");
		if (dot > 0)
			base = base.Substring(0, dot);
		string iconId = INT_VPP_RFImport.INT_RFIconId(base);
		string res = INT_VPP_Icons.INT_PathOf(iconId);
		if (res == "")
			res = INT_VPP_Icons.INT_DefaultPath();
		return res;
	}

	// base name (lower case, no extension) of an RF_MAP icon -> our icon id ("" = unknown -> default)
	static string INT_RFIconId(string base)
	{
		if (base == "00_location")
			return "rally";
		if (base == "01_location_question")
			return "question";
		if (base == "02_exclamation_point")
			return "warning";
		if (base == "03_cross")
			return "skull";
		if (base == "04_person")
			return "rally";
		if (base == "05_star")
			return "star";
		if (base == "06_crossed")
			return "lock";
		if (base == "07_plus")
			return "rally";
		if (base == "08_market")
			return "store";
		if (base == "09_building")
			return "base";
		if (base == "10_box")
			return "chest";
		if (base == "11_car")
			return "car";
		if (base == "12_money")
			return "moneybag";
		if (base == "13_pocket")
			return "coin";
		if (base == "14_gps")
			return "tower";
		if (base == "15_timer")
			return "question";
		return "";
	}

	// RF textColor (ints 0..255) -> MarkerInfo colour (vector 0..255); no colour = white. The engine reader turns a
	// missing / null textColor into a NEW object with every field 0 (measured), so a=r=g=b=0 also means "none".
	static vector INT_ColorOf(INT_VPP_RFIColor c)
	{
		vector white = "255 255 255";
		if (!c)
			return white;
		if (c.a == 0 && c.r == 0 && c.g == 0 && c.b == 0)
			return white;
		int r = Math.Clamp(c.r, 0, 255);
		int g = Math.Clamp(c.g, 0, 255);
		int b = Math.Clamp(c.b, 0, 255);
		vector v = Vector(r, g, b);
		return v;
	}

	// RFImport.json: missing or broken = nothing imported yet (a broken file is replaced at the next record)
	static INT_VPP_RFIState INT_LoadState(string statePath)
	{
		INT_VPP_RFIState st = new INT_VPP_RFIState();
		if (!FileExist(statePath))
			return st;
		string txt;
		string why = INT_VPP_RFImport.INT_ReadWhole(statePath, txt);
		if (why != "")
			return st;
		if (txt.IndexOf("\"INT_Servers\"") < 0)
			return st;
		JsonSerializer js = new JsonSerializer();
		string err;
		INT_VPP_RFIState rd = new INT_VPP_RFIState();
		bool ok = js.ReadFromString(rd, txt, err);
		if (!ok)
			return st;
		if (!rd.INT_Servers)
			rd.INT_Servers = new array<ref INT_VPP_RFIServer>();
		return rd;
	}

	protected static void INT_Remember(INT_VPP_RFIState st, string statePath, string server, string rfPath, int count)
	{
		INT_VPP_RFIServer rec = new INT_VPP_RFIServer();
		rec.INT_Server = server;
		int slash = rfPath.LastIndexOf("/");
		string fileName = rfPath;
		if (slash >= 0)
		{
			int from = slash + 1;
			int len = rfPath.Length() - from;
			fileName = rfPath.Substring(from, len);
		}
		rec.INT_File = fileName;
		rec.INT_Count = count;
		rec.INT_Time = INT_VPP_FileMove.INT_Stamp();
		st.INT_Servers.Insert(rec);
		while (st.INT_Servers.Count() > INT_MAX_SERVERS)
			st.INT_Servers.RemoveOrdered(0);
		int cut = statePath.LastIndexOf("/");
		if (cut > 0)
		{
			string dir = statePath.Substring(0, cut);
			INT_VPP_FileMove.INT_EnsureDir(dir);
		}
		JsonSerializer js = new JsonSerializer();
		string txt;
		bool built = js.WriteToString(st, true, txt);
		if (!built)
			return;
		FileHandle fh = OpenFile(statePath, FileMode.WRITE);
		if (fh == 0)
			return;
		FPrint(fh, txt);
		CloseFile(fh);
	}

	protected static void INT_Log(INT_VPP_RFIResult res, string server)
	{
		if (!res)
			return;
		string srv = INT_VPP_Validate.LogText(server);
		string file = INT_VPP_Validate.LogText(res.m_INT_File);
		if (res.m_INT_Code == INT_RES_IMPORTED || res.m_INT_Code == INT_RES_EMPTY)
		{
			string line = res.m_INT_Added.ToString() + " of " + res.m_INT_InFile.ToString();
			line = line + " (duplicates " + res.m_INT_Dups.ToString() + ", bad " + res.m_INT_Bad.ToString() + ", over limit " + res.m_INT_Capped.ToString() + ")";
			line = line + " from " + file + " into the list of server " + srv;
			Print("[INT_VPP] RF_MAP markers imported: " + line);
			return;
		}
		if (res.m_INT_Code == INT_RES_BAD_FILE)
		{
			string det = INT_VPP_Validate.LogText(res.m_INT_Detail);
			Print("[INT_VPP] WARN RF_MAP marker file " + file + " not imported: " + det + " (tried again next session)");
		}
	}

	// ---- TEST ----
	static INT_VPP_RFIResult INT_Last()
	{
		return s_INT_Last;
	}

	static bool INT_TestTried()
	{
		return s_INT_Tried;
	}

	static void INT_TestSetTried(bool tried)
	{
		s_INT_Tried = tried;
	}

	static void INT_TestSetMaxBytes(int bytes)
	{
		s_INT_TestMaxBytes = bytes;
	}
}
