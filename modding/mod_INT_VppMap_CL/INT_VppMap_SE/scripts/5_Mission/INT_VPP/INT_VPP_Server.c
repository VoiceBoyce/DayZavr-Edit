// INT_VPP «Экспедиция» B — SERVER side (NET, BUILD_PLAN §1 server authority, §2 «Data / net»). Reference: NET_API.md.
// Server-only PBO INT_VppMap_SE (-serverMod) since 2026-11-12: moved out of the client PBO unchanged except the
// file places. Files: $profile:INT_Mods/INT_VppMap/Settings.json, Zones.json (+ .bak / .tmp / .broken-* / .dropped-*),
// moved once from $profile:VPPMap_INT/ by INT_VPP_SE_Migrate (3_Game) before the first read.
// Everything here runs only on the dedicated server (MissionServer exists only there; every entry re-checks).
// Authority: the client sends requests only; admin rights, limits, ids, geometry checks and the zone list live here.
// Write handlers check the engine sender's admin right and the write budget BEFORE reading the payload (a non-admin
// or a flooding sender never gets it deserialised). Rejected requests change nothing, leave one rate-limited log line
// and get a rate-limited INT_ZoneResult + fresh INT_Settings (the client's IsAdmin may be out of date).
// Files written by a newer build (version above ours) are used read-only and never rewritten in this session.
// Admin text labels (2026-11-13): $profile:INT_Mods/INT_VppMap/Labels.json, the same loader / ReplaceVerified rules as
// Zones.json, RPCs INT_LabelSave / INT_LabelDelete through the same INT_GateWrite, answers INT_LabelResult, the list
// (INT_Labels) to every client on change and to the sender on Hello. Additive: PROTOCOL stays 2.
// Server markers (2026-11-14): map admins add / edit / delete the markers of VPPMapConfig.json in game
// (INT_VPP_SrvMarkers below): RPCs INT_SrvMarkerSave / INT_SrvMarkerDelete through the same INT_GateWrite, answers on
// INT_LabelResult (kinds 5 / 6), the new list to every client on the old VPP path (RPC_MissionGameplay SessionMarkers).
// Additive again: PROTOCOL stays 2.

class INT_VPP_ServerFiles
{
	static const string ROOT = "$profile:INT_Mods/INT_VppMap";
	static const string SETTINGS_NAME = "Settings.json";
	static const string ZONES_NAME = "Zones.json";
	static const string SUPERADMINS = "$profile:VPPAdminTools/Permissions/SuperAdmins/SuperAdmins.txt";
	static const string SUPERADMINS_TEST_NAME = "SuperAdmins.txt";
	static const int READ_MAX = 2000000;
	static const int KIND_SETTINGS = 0;
	static const int KIND_ZONES = 1;
	// admin text labels (2026-11-13): always in the new folder (no old place, nothing to move)
	static const string INT_LABELS_NAME = "Labels.json";
	static const int INT_KIND_LABELS = 2;
	// VPPMapConfig.json written by an in-game server-marker change (2026-11-14)
	static const int INT_KIND_MARKERS = 3;
	// TestKit I/O failure injection for ReplaceVerified (never set outside the self-test).
	static const int INT_TEST_FAIL_NONE = 0;
	static const int INT_TEST_FAIL_BAK = 1;               // copy main -> .bak fails
	static const int INT_TEST_FAIL_MAIN = 2;              // copy .tmp -> main fails (old file comes back from .bak)
	static const int INT_TEST_FAIL_MAIN_AND_RESTORE = 3;  // copy .tmp -> main AND .bak -> main fail
	// every Settings / Zones parse fails as if the file had wrong value types (the engine's own "JSON ERROR" log
	// line of a real failed ReadFromString is not produced: script logs of a stand run stay free of (E) lines)
	static const int INT_TEST_FAIL_PARSE = 4;

	protected static string s_TestRoot;
	protected static int s_INT_TestFail;

	static void INT_SetTestFail(int mode)
	{
		s_INT_TestFail = mode;
	}

	// TestKit: sandbox folder under $profile: (e.g. "$profile:INT_Mods/INT_VppMap_Test"); "" = the real folders.
	static void SetTestRoot(string root)
	{
		s_TestRoot = root;
	}

	static string Root()
	{
		if (s_TestRoot != "")
			return s_TestRoot;
		return ROOT;
	}

	// The real folders come from the one-time move (ROOT, or the old folder for a session whose move failed).
	static string SettingsPath()
	{
		string root = INT_VPP_SE_Migrate.INT_SettingsDir();
		if (s_TestRoot != "")
			root = s_TestRoot;
		return root + "/" + SETTINGS_NAME;
	}

	static string ZonesPath()
	{
		string root = INT_VPP_SE_Migrate.INT_ZonesDir();
		if (s_TestRoot != "")
			root = s_TestRoot;
		return root + "/" + ZONES_NAME;
	}

	// Labels.json lives only in $profile:INT_Mods/INT_VppMap/ (or the test root): it is new, the one-time move of
	// the old files never touches it.
	static string INT_LabelsPath()
	{
		string root = INT_VPP_ServerFiles.Root();
		return root + "/" + INT_LABELS_NAME;
	}

	// In test mode the superadmin list is <test root>/SuperAdmins.txt.
	static string SuperAdminsPath()
	{
		if (s_TestRoot != "")
			return s_TestRoot + "/" + SUPERADMINS_TEST_NAME;
		return SUPERADMINS;
	}

	static void MakeDirs()
	{
		INT_VPP_FileMove.INT_EnsureDir(ROOT);
		string root = INT_VPP_ServerFiles.Root();
		INT_VPP_FileMove.INT_EnsureDir(root);
	}

	static bool ReadText(string path, out string content)
	{
		content = "";
		if (!FileExist(path))
			return false;
		FileHandle fh = OpenFile(path, FileMode.READ);
		if (fh == 0)
			return false;
		string txt;
		ReadFile(fh, txt, READ_MAX);
		CloseFile(fh);
		content = txt;
		return true;
	}

	static bool WriteText(string path, string content)
	{
		FileHandle fh = OpenFile(path, FileMode.WRITE);
		if (fh == 0)
			return false;
		FPrint(fh, content);
		CloseFile(fh);
		return true;
	}

	static string TwoDigits(int v)
	{
		string s = v.ToString();
		if (v < 10)
			s = "0" + s;
		return s;
	}

	// UTC stamp for file names: 20261112-153000
	static string Stamp()
	{
		int yy;
		int mo;
		int dd;
		int hh;
		int mi;
		int ss;
		GetYearMonthDayUTC(yy, mo, dd);
		GetHourMinuteSecondUTC(hh, mi, ss);
		string sMo = INT_VPP_ServerFiles.TwoDigits(mo);
		string sDd = INT_VPP_ServerFiles.TwoDigits(dd);
		string sHh = INT_VPP_ServerFiles.TwoDigits(hh);
		string sMi = INT_VPP_ServerFiles.TwoDigits(mi);
		string sSs = INT_VPP_ServerFiles.TwoDigits(ss);
		string res = yy.ToString() + sMo + sDd + "-" + sHh + sMi + sSs;
		return res;
	}

	// Copy of a file before it is replaced for a non-normal reason ("broken", "dropped"); returns the copy path or "".
	static string KeepCopy(string path, string tag)
	{
		if (!FileExist(path))
			return "";
		string stamp = INT_VPP_ServerFiles.Stamp();
		string dst = path + "." + tag + "-" + stamp;
		bool ok = CopyFile(path, dst);
		if (ok)
			return dst;
		return "";
	}

	// Both files exist and hold the same bytes (whole-file ReadFile + length + ==; never string.Replace, which
	// corrupts strings above ~2.5 KB on 1.30). Used for every CopyFile result.
	static bool INT_SameFile(string a, string b)
	{
		string ta;
		string tb;
		if (!INT_VPP_ServerFiles.ReadText(a, ta))
			return false;
		if (!INT_VPP_ServerFiles.ReadText(b, tb))
			return false;
		int la = ta.Length();
		int lb = tb.Length();
		if (la != lb)
			return false;
		if (ta != tb)
			return false;
		return true;
	}

	// An unreadable file is moved aside: byte-identical "<path>.<tag>-<stamp>" copy, then the original is deleted
	// (so the next save does not back it up over a good .bak). Returns the copy, or "" = nothing done (file stays).
	static string INT_SetAside(string path, string tag)
	{
		string dst = INT_VPP_ServerFiles.KeepCopy(path, tag);
		if (dst == "")
			return "";
		if (!INT_VPP_ServerFiles.INT_SameFile(path, dst))
			return "";
		DeleteFile(path);
		return dst;
	}

	// true = the file exists and parses into d; raw = its text (also when it does not parse).
	static bool INT_ParseSettings(string path, INT_VPP_ServerSettings d, out string raw, out string err)
	{
		raw = "";
		err = "";
		string txt;
		if (!INT_VPP_ServerFiles.ReadText(path, txt))
			return false;
		raw = txt;
		if (s_INT_TestFail == INT_TEST_FAIL_PARSE)
		{
			err = "test: parse failure injected";
			return false;
		}
		string perr;
		JsonSerializer js = new JsonSerializer();
		bool ok = js.ReadFromString(d, txt, perr);
		err = perr;
		return ok;
	}

	// Version written in a file that the JSON reader rejected (0 = none found).
	static int INT_RawVersion(string raw, string key)
	{
		int v = 0;
		bool found = INT_VPP_Validate.INT_JsonInt(raw, key, v);
		if (!found)
			return 0;
		return v;
	}

	// Parses a file of the given kind; for zones also checks the count and NextId that were written.
	// Whole-file ReadFile + JsonSerializer.ReadFromString: no string.Replace and no SubstringUtf8, so it has no
	// ~2.5-8 KB single-string limit (the same read/parse path carries multi-MB files in INT_Garage / INT_SLS on
	// 1.30); the maximum Zones.json (64 zones x 32 vertices, ~100 KB) is round-tripped by the TestKit (max.*).
	static bool VerifyFile(string path, int kind, int expectCount, int expectNextId)
	{
		string txt;
		if (!INT_VPP_ServerFiles.ReadText(path, txt))
			return false;
		string err;
		JsonSerializer js = new JsonSerializer();
		if (kind == KIND_SETTINGS)
		{
			INT_VPP_ServerSettings vs = new INT_VPP_ServerSettings();
			bool okS = js.ReadFromString(vs, txt, err);
			return okS;
		}
		if (kind == INT_KIND_MARKERS)
		{
			VPPMapConfig vm = new VPPMapConfig();
			bool okM = js.ReadFromString(vm, txt, err);
			if (!okM)
				return false;
			array<ref MarkerInfo> ml = vm.GetCachedArray();
			if (!ml)
				return false;
			int mc = ml.Count();
			if (mc != expectCount)
				return false;
			return true;
		}
		if (kind == INT_KIND_LABELS)
		{
			INT_VPP_LabelsFile vl = new INT_VPP_LabelsFile();
			bool okL = js.ReadFromString(vl, txt, err);
			if (!okL)
				return false;
			if (!vl.INT_LabelList)
				return false;
			int lc = vl.INT_LabelList.Count();
			if (lc != expectCount)
				return false;
			if (vl.INT_NextId != expectNextId)
				return false;
			return true;
		}
		INT_VPP_ZonesFile vf = new INT_VPP_ZonesFile();
		bool okZ = js.ReadFromString(vf, txt, err);
		if (!okZ)
			return false;
		if (!vf.ZoneList)
			return false;
		int cnt = vf.ZoneList.Count();
		if (cnt != expectCount)
			return false;
		if (vf.NextId != expectNextId)
			return false;
		return true;
	}

	// Recoverable replace (DayZ has no rename: delete + copy is not atomic). Order:
	//   1. content -> .tmp, parsed back (count + NextId); a bad .tmp is deleted, nothing else touched.
	//   2. main -> .bak, and .bak must be byte-identical to main; otherwise ABORT: main untouched, .tmp kept.
	//   3. delete main, copy .tmp -> main, main must be byte-identical to .tmp and parse; then .tmp is deleted.
	//   4. step 3 failed: main comes back from .bak (byte-identical); .tmp (complete, newer) is ALWAYS kept.
	// Any false: the caller rolls its memory change back. The loader reads main, else .tmp, else .bak, so no
	// failure path leaves nothing loadable. One log line per failure.
	static bool ReplaceVerified(string path, string content, int kind, int expectCount, int expectNextId, out string err)
	{
		err = "";
		string tmp = path + ".tmp";
		string bak = path + ".bak";
		int fail = s_INT_TestFail;
		if (!INT_VPP_ServerFiles.WriteText(tmp, content))
		{
			DeleteFile(tmp);
			err = "cannot write " + tmp;
			return false;
		}
		if (!INT_VPP_ServerFiles.VerifyFile(tmp, kind, expectCount, expectNextId))
		{
			DeleteFile(tmp);
			err = "check of " + tmp + " failed";
			return false;
		}
		bool hadOld = FileExist(path);
		if (hadOld)
		{
			if (FileExist(bak))
				DeleteFile(bak);
			bool bakCopied = false;
			if (fail != INT_TEST_FAIL_BAK)
				bakCopied = CopyFile(path, bak);
			bool bakGood = false;
			if (bakCopied)
				bakGood = INT_VPP_ServerFiles.INT_SameFile(path, bak);
			if (!bakGood)
			{
				err = "backup " + bak + " failed (" + path + " untouched, .tmp kept)";
				INT_VPP_ServerState.LogWarn("save: " + err);
				return false;
			}
			if (!DeleteFile(path))
			{
				err = "cannot replace " + path + " (.tmp kept)";
				INT_VPP_ServerState.LogWarn("save: " + err);
				return false;
			}
		}
		bool copied = false;
		if (fail != INT_TEST_FAIL_MAIN && fail != INT_TEST_FAIL_MAIN_AND_RESTORE)
			copied = CopyFile(tmp, path);
		bool good = false;
		if (copied)
		{
			if (INT_VPP_ServerFiles.INT_SameFile(tmp, path))
				good = INT_VPP_ServerFiles.VerifyFile(path, kind, expectCount, expectNextId);
		}
		if (good)
		{
			DeleteFile(tmp);
			return true;
		}
		if (FileExist(path))
			DeleteFile(path);
		if (!hadOld)
		{
			err = "copy to " + path + " failed (.tmp kept)";
			INT_VPP_ServerState.LogWarn("save: " + err);
			return false;
		}
		bool restored = false;
		if (fail != INT_TEST_FAIL_MAIN_AND_RESTORE)
			restored = CopyFile(bak, path);
		bool restoredGood = false;
		if (restored)
			restoredGood = INT_VPP_ServerFiles.INT_SameFile(bak, path);
		if (restoredGood)
		{
			err = "copy to " + path + " failed (old file restored from .bak, .tmp kept)";
			INT_VPP_ServerState.LogWarn("save: " + err);
			return false;
		}
		if (FileExist(path))
			DeleteFile(path);
		err = "copy to " + path + " failed and .bak could not be restored (.tmp = new, .bak = old, both kept; the next start loads .tmp)";
		INT_VPP_ServerState.LogWarn("save: CRITICAL " + err);
		return false;
	}

	// null = missing or unreadable.
	static INT_VPP_ZonesFile ParseZones(string path)
	{
		string txt;
		if (!INT_VPP_ServerFiles.ReadText(path, txt))
			return null;
		INT_VPP_ZonesFile f = new INT_VPP_ZonesFile();
		string err;
		bool ok = false;
		if (s_INT_TestFail == INT_TEST_FAIL_PARSE)
		{
			err = "test: parse failure injected";
		}
		else
		{
			JsonSerializer js = new JsonSerializer();
			ok = js.ReadFromString(f, txt, err);
		}
		if (!ok)
		{
			string shortErr = INT_VPP_Validate.LogText(err);
			INT_VPP_ServerState.LogWarn("cannot parse " + path + ": " + shortErr);
			return null;
		}
		if (!f.ZoneList)
		{
			array<ref INT_VPP_Zone> emptyList = new array<ref INT_VPP_Zone>();
			f.ZoneList = emptyList;
		}
		return f;
	}

	// Labels.json (2026-11-13): null = missing or unreadable.
	static INT_VPP_LabelsFile INT_ParseLabels(string path)
	{
		string txt;
		if (!INT_VPP_ServerFiles.ReadText(path, txt))
			return null;
		INT_VPP_LabelsFile f = new INT_VPP_LabelsFile();
		string err;
		bool ok = false;
		if (s_INT_TestFail == INT_TEST_FAIL_PARSE)
		{
			err = "test: parse failure injected";
		}
		else
		{
			JsonSerializer js = new JsonSerializer();
			ok = js.ReadFromString(f, txt, err);
		}
		if (!ok)
		{
			string shortErr = INT_VPP_Validate.LogText(err);
			INT_VPP_ServerState.LogWarn("cannot parse " + path + ": " + shortErr);
			return null;
		}
		if (!f.INT_LabelList)
		{
			array<ref INT_VPP_Label> emptyList = new array<ref INT_VPP_Label>();
			f.INT_LabelList = emptyList;
		}
		return f;
	}
}

class INT_VPP_ServerState
{
	protected static ref INT_VPP_ServerSettings s_Settings;
	protected static ref INT_VPP_ZonesFile s_Zones;
	protected static ref array<string> s_Admins;
	protected static ref array<string> s_SuperAdmins;
	protected static bool s_SuperLoaded;
	protected static int s_SuperReadAt;
	protected static ref map<string, int> s_RateAt;
	protected static ref map<string, int> s_RateCount;
	protected static ref map<string, int> s_HelloAt;
	protected static ref map<string, int> s_ReplyAt;
	// VPP's 4 old RPCs (missionServer.c): "<uid>|<rpc>" -> last answered time (ms); one answer per 2 s per sender
	protected static ref map<string, int> s_INT_VppRpcAt;
	static const int INT_VPP_RPC_GAP_MS = 2000;
	// true = the file was written by a newer build: used in memory only, never rewritten in this session.
	protected static bool s_SettingsReadOnly;
	protected static bool s_ZonesReadOnly;
	protected static int s_MsgId;
	// admin text labels (2026-11-13)
	protected static ref INT_VPP_LabelsFile s_INT_Labels;
	protected static bool s_INT_LabelsReadOnly;
	protected static int s_INT_LastLabelId;
	protected static bool s_LogStarted;
	protected static int s_LogAt;
	protected static int s_LogCount;
	protected static int s_LogSuppressed;
	protected static bool s_Loaded;
	protected static int s_LastSavedId;

	// ---------- logging (never player text as a format string; Print is plain concatenation) ----------
	static void LogInfo(string text)
	{
		Print("[INT_VPP] " + text);
	}

	static void LogWarn(string text)
	{
		Print("[INT_VPP] WARN " + text);
	}

	// One line per rejected request, at most LOG_MAX_PER_WINDOW lines per LOG_WINDOW_MS (the rest is counted).
	static void LogReject(string rpc, string who, string reason)
	{
		int now = GetGame().GetTime();
		int gap = now - s_LogAt;
		if (!s_LogStarted || gap >= INT_VPP_Net.LOG_WINDOW_MS || gap < 0)
		{
			int suppressed = s_LogSuppressed;
			if (suppressed > 0)
				Print("[INT_VPP] REJECT: " + suppressed.ToString() + " more rejected requests not logged");
			s_LogStarted = true;
			s_LogAt = now;
			s_LogCount = 0;
			s_LogSuppressed = 0;
		}
		if (s_LogCount >= INT_VPP_Net.LOG_MAX_PER_WINDOW)
		{
			s_LogSuppressed++;
			return;
		}
		s_LogCount++;
		string r = INT_VPP_Validate.LogText(reason);
		Print("[INT_VPP] REJECT " + rpc + " from=" + who + " reason=" + r);
	}

	// ---------- load ----------
	static void EnsureMaps()
	{
		if (!s_RateAt)
			s_RateAt = new map<string, int>();
		if (!s_RateCount)
			s_RateCount = new map<string, int>();
		if (!s_HelloAt)
			s_HelloAt = new map<string, int>();
		if (!s_ReplyAt)
			s_ReplyAt = new map<string, int>();
		if (!s_INT_VppRpcAt)
			s_INT_VppRpcAt = new map<string, int>();
		if (!s_Admins)
			s_Admins = new array<string>();
		if (!s_SuperAdmins)
			s_SuperAdmins = new array<string>();
	}

	static void EnsureLoaded()
	{
		if (!s_Loaded)
			INT_VPP_ServerState.LoadAll();
	}

	static void LoadAll()
	{
		s_Loaded = true;
		INT_VPP_ServerState.EnsureMaps();
		// the one-time move from the old places comes before the first read (no-op after the first call)
		INT_VPP_SE_Migrate.INT_RunOnce();
		INT_VPP_ServerFiles.MakeDirs();
		INT_VPP_ServerState.LoadSettings();
		INT_VPP_ServerState.LoadZones();
		INT_VPP_ServerState.INT_LoadLabels();
		INT_VPP_ServerState.RefreshSuperAdmins(true);
		int nz = s_Zones.ZoneList.Count();
		int nl = s_INT_Labels.INT_LabelList.Count();
		int na = s_Admins.Count();
		int ns = s_SuperAdmins.Count();
		int us = s_Settings.UseVPPAdminToolsSuperAdmins;
		string tail = " admins=" + na.ToString() + " superadmins=" + ns.ToString() + " useSuperAdmins=" + us.ToString();
		int rfi = s_Settings.INT_ImportRFMapMarkers;
		tail = tail + " importRFMapMarkers=" + rfi.ToString();
		INT_VPP_ServerState.LogInfo("loaded: zones=" + nz.ToString() + " labels=" + nl.ToString() + tail);
	}

	// Settings.json, else a complete .tmp (newer), else .bak (older), else defaults. An unreadable main file is moved
	// aside as .broken-<stamp>. A file of a NEWER build (ConfigVersion above ours) is used read-only (unknown keys
	// ignored, missing ones defaulted in memory) and never rewritten in this session.
	static void LoadSettings()
	{
		s_SettingsReadOnly = false;
		string path = INT_VPP_ServerFiles.SettingsPath();
		string tmp = path + ".tmp";
		string bak = path + ".bak";
		bool mainExists = FileExist(path);
		INT_VPP_ServerSettings d = new INT_VPP_ServerSettings();
		bool save = false;
		bool parsed = false;
		string src = "main";
		string raw = "";
		string err = "";
		int known = INT_VPP_Net.SETTINGS_VERSION;
		if (mainExists)
			parsed = INT_VPP_ServerFiles.INT_ParseSettings(path, d, raw, err);
		if (mainExists && !parsed)
		{
			int rawVer = INT_VPP_ServerFiles.INT_RawVersion(raw, "ConfigVersion");
			if (rawVer > known)
			{
				s_SettingsReadOnly = true;
				INT_VPP_ServerState.LogWarn("Settings.json ConfigVersion " + rawVer.ToString() + " is newer than this build (" + known.ToString() + ") and unreadable here: running on defaults, the file is never rewritten in this session");
			}
			else
			{
				string kept = INT_VPP_ServerFiles.INT_SetAside(path, "broken");
				string shortErr = INT_VPP_Validate.LogText(err);
				INT_VPP_ServerState.LogWarn("Settings.json unreadable (" + shortErr + "), copy: " + kept);
			}
			d = new INT_VPP_ServerSettings();
		}
		if (!parsed && !s_SettingsReadOnly)
		{
			INT_VPP_ServerSettings dFromTmp = new INT_VPP_ServerSettings();
			string rawT;
			string errT;
			INT_VPP_ServerSettings dFromBak = new INT_VPP_ServerSettings();
			string rawB;
			string errB;
			if (INT_VPP_ServerFiles.INT_ParseSettings(tmp, dFromTmp, rawT, errT))
			{
				d = dFromTmp;
				raw = rawT;
				parsed = true;
				src = "tmp";
			}
			else if (INT_VPP_ServerFiles.INT_ParseSettings(bak, dFromBak, rawB, errB))
			{
				d = dFromBak;
				raw = rawB;
				parsed = true;
				src = "bak";
			}
			save = true;
			if (parsed)
				INT_VPP_ServerState.LogWarn("Settings.json taken from: " + src);
			else if (mainExists)
				INT_VPP_ServerState.LogWarn("Settings.json: no readable copy, running on defaults (written)");
			else
				INT_VPP_ServerState.LogInfo("Settings.json not found, defaults written: " + path);
		}
		if (parsed)
		{
			int ver = d.ConfigVersion;
			if (ver > known)
			{
				s_SettingsReadOnly = true;
				INT_VPP_ServerState.LogWarn("Settings.json (" + src + ") ConfigVersion " + ver.ToString() + " is newer than this build (" + known.ToString() + "): used read-only, never rewritten in this session");
			}
			int miss = d.FillMissing(raw);
			if (miss > 0)
			{
				save = true;
				INT_VPP_ServerState.LogInfo("Settings.json: " + miss.ToString() + " missing keys added");
			}
		}
		int fixes = d.Sanitize();
		if (fixes > 0)
		{
			save = true;
			INT_VPP_ServerState.LogWarn("Settings.json: " + fixes.ToString() + " values out of range, corrected");
		}
		s_Settings = d;
		INT_VPP_ServerState.BuildAdmins();
		if (save && !s_SettingsReadOnly)
		{
			JsonSerializer jw = new JsonSerializer();
			string canon;
			bool built = jw.WriteToString(d, true, canon);
			string werr;
			bool written = false;
			if (built)
				written = INT_VPP_ServerFiles.ReplaceVerified(path, canon, INT_VPP_ServerFiles.KIND_SETTINGS, 0, 0, werr);
			if (!written)
				INT_VPP_ServerState.LogWarn("Settings.json not written: " + werr);
		}
	}

	static void BuildAdmins()
	{
		array<string> list = new array<string>();
		if (s_Settings && s_Settings.Admins)
		{
			foreach (string raw : s_Settings.Admins)
			{
				string id = raw + "";
				id.TrimInPlace();
				if (id == "")
					continue;
				if (!INT_VPP_Validate.IsSteam64(id))
				{
					string bad = INT_VPP_Validate.LogText(id);
					INT_VPP_ServerState.LogWarn("Settings.json Admins: not a Steam64 id (17 digits), skipped: " + bad);
					continue;
				}
				if (list.Find(id) == -1)
					list.Insert(id);
			}
		}
		s_Admins = list;
	}

	// Re-read at most every SUPERADMINS_REREAD_MS (only when an admin check needs it). Missing file = empty list.
	static void RefreshSuperAdmins(bool force)
	{
		int now = GetGame().GetTime();
		if (!force && s_SuperLoaded)
		{
			int gap = now - s_SuperReadAt;
			if (gap >= 0 && gap < INT_VPP_Net.SUPERADMINS_REREAD_MS)
				return;
		}
		s_SuperLoaded = true;
		s_SuperReadAt = now;
		array<string> list = new array<string>();
		string txt;
		string path = INT_VPP_ServerFiles.SuperAdminsPath();
		if (INT_VPP_ServerFiles.ReadText(path, txt))
		{
			array<string> lines = new array<string>();
			txt.Split("\n", lines);
			foreach (string line : lines)
			{
				string id = line + "";
				id.Replace("\r", "");
				id.Replace(" ", "");
				id.TrimInPlace();
				if (!INT_VPP_Validate.IsSteam64(id))
					continue;
				if (list.Find(id) == -1)
					list.Insert(id);
			}
		}
		s_SuperAdmins = list;
	}

	// Zones.json, else a complete .tmp (newer), else .bak (older), else empty. A file of a NEWER build (FileVersion
	// above ours) is served read-only: zones kept in memory, every save refused, the file never rewritten.
	static void LoadZones()
	{
		s_ZonesReadOnly = false;
		string mainPath = INT_VPP_ServerFiles.ZonesPath();
		string tmp = mainPath + ".tmp";
		string bak = mainPath + ".bak";
		bool mainExists = FileExist(mainPath);
		bool save = false;
		string src = "main";
		int known = INT_VPP_Net.ZONES_FILE_VERSION;
		INT_VPP_ZonesFile f = INT_VPP_ServerFiles.ParseZones(mainPath);
		if (!f && mainExists)
		{
			string rawMain;
			INT_VPP_ServerFiles.ReadText(mainPath, rawMain);
			int rawVer = INT_VPP_ServerFiles.INT_RawVersion(rawMain, "FileVersion");
			if (rawVer > known)
			{
				s_ZonesReadOnly = true;
				INT_VPP_ServerState.LogWarn("Zones.json FileVersion " + rawVer.ToString() + " is newer than this build (" + known.ToString() + ") and unreadable here: no zones served, saves refused, the file is never rewritten in this session");
				f = new INT_VPP_ZonesFile();
			}
		}
		if (!f)
		{
			if (mainExists)
			{
				string kept = INT_VPP_ServerFiles.INT_SetAside(mainPath, "broken");
				INT_VPP_ServerState.LogWarn("Zones.json unreadable, copy: " + kept);
			}
			src = "tmp";
			f = INT_VPP_ServerFiles.ParseZones(tmp);
			if (!f)
			{
				src = "bak";
				f = INT_VPP_ServerFiles.ParseZones(bak);
			}
			if (!f)
			{
				src = "new";
				f = new INT_VPP_ZonesFile();
			}
			save = true;
			if (mainExists || src != "new")
				INT_VPP_ServerState.LogWarn("Zones.json taken from: " + src);
		}
		int fileVer = f.FileVersion;
		if (fileVer > known && !s_ZonesReadOnly)
		{
			s_ZonesReadOnly = true;
			INT_VPP_ServerState.LogWarn("Zones.json (" + src + ") FileVersion " + fileVer.ToString() + " is newer than this build (" + known.ToString() + "): zones served read-only, saves refused, never rewritten in this session");
		}

		// Existing zones are checked against the HARD limits (lowering MaxZones / MaxZoneVertices never drops zones).
		// Read-only: the same checks shape what is served, but nothing is written or copied.
		array<ref INT_VPP_Zone> srcList = f.ZoneList;
		array<ref INT_VPP_Zone> valid = new array<ref INT_VPP_Zone>();
		int dropped = 0;
		if (srcList)
		{
			foreach (INT_VPP_Zone zl : srcList)
			{
				if (!zl)
				{
					dropped++;
					continue;
				}
				int zlId = zl.id;
				if (valid.Count() >= INT_VPP_Net.HARD_MAX_ZONES)
				{
					dropped++;
					INT_VPP_ServerState.LogWarn("zone dropped id=" + zlId.ToString() + " reason=more than the hard limit of zones");
					continue;
				}
				string why;
				bool ok = INT_VPP_Validate.ZoneLimits(zl, INT_VPP_Net.HARD_MAX_ZONE_VERTICES, why);
				if (!ok)
				{
					dropped++;
					INT_VPP_ServerState.LogWarn("zone dropped id=" + zlId.ToString() + " reason=" + why);
					continue;
				}
				valid.Insert(zl);
			}
		}
		int top = INT_VPP_ZoneIds.MaxId(valid);
		array<int> seen = new array<int>();
		int fixedIds = 0;
		foreach (INT_VPP_Zone vz : valid)
		{
			int vid = vz.id;
			if (vid <= 0 || seen.Find(vid) != -1)
			{
				top++;
				vid = top;
				vz.id = vid;
				fixedIds++;
			}
			seen.Insert(vid);
		}
		int nid = INT_VPP_ZoneIds.Next(valid, f.NextId);
		if (nid != f.NextId)
			save = true;
		if (fixedIds > 0)
		{
			save = true;
			INT_VPP_ServerState.LogWarn("Zones.json: " + fixedIds.ToString() + " zones got a new id (missing or duplicate)");
		}
		if (dropped > 0)
		{
			save = true;
			if (mainExists && src == "main" && !s_ZonesReadOnly)
			{
				string keptDropped = INT_VPP_ServerFiles.KeepCopy(mainPath, "dropped");
				INT_VPP_ServerState.LogWarn("Zones.json: " + dropped.ToString() + " zones dropped, original kept: " + keptDropped);
			}
		}
		if (!mainExists)
			save = true;
		if (!s_ZonesReadOnly)
			f.FileVersion = known;
		f.NextId = nid;
		f.ZoneList = valid;
		s_Zones = f;
		int cnt = valid.Count();
		if (s_Settings && cnt > s_Settings.MaxZones)
			INT_VPP_ServerState.LogWarn("Zones.json holds " + cnt.ToString() + " zones, more than MaxZones: no new zones until some are deleted");
		if (save && !s_ZonesReadOnly)
		{
			string err;
			if (!INT_VPP_ServerState.SaveZones(err))
				INT_VPP_ServerState.LogWarn("Zones.json not written: " + err);
		}
	}

	static bool SaveZones(out string err)
	{
		err = "";
		if (!s_Zones)
		{
			err = "no data";
			return false;
		}
		if (s_ZonesReadOnly)
		{
			err = INT_VPP_Net.REASON_ZONES_READ_ONLY;
			return false;
		}
		JsonSerializer js = new JsonSerializer();
		string txt;
		bool built = js.WriteToString(s_Zones, true, txt);
		if (!built)
		{
			err = "json build failed";
			return false;
		}
		string path = INT_VPP_ServerFiles.ZonesPath();
		int cnt = s_Zones.ZoneList.Count();
		int nid = s_Zones.NextId;
		string why;
		bool ok = INT_VPP_ServerFiles.ReplaceVerified(path, txt, INT_VPP_ServerFiles.KIND_ZONES, cnt, nid, why);
		err = why;
		return ok;
	}

	// ---------- admin text labels (2026-11-13) ----------
	// Labels.json, else a complete .tmp (newer), else .bak (older), else empty (written). An unreadable main file is
	// set aside as .broken-<stamp>. A NEWER build's file (INT_FileVersion above ours) is served read-only: labels kept
	// in memory, every save refused, never rewritten in this session. Loaded labels are checked against the HARD
	// rules (500 labels, text, palette colour, size, position; a 0 colour / size = key missing in a hand-written file
	// = white / 24); dropped ones leave a .dropped-<stamp> copy; missing / duplicate ids get new ones.
	static void INT_LoadLabels()
	{
		s_INT_LabelsReadOnly = false;
		string mainPath = INT_VPP_ServerFiles.INT_LabelsPath();
		string tmp = mainPath + ".tmp";
		string bak = mainPath + ".bak";
		bool mainExists = FileExist(mainPath);
		bool save = false;
		string src = "main";
		int known = INT_VPP_Net.INT_LABELS_FILE_VERSION;
		INT_VPP_LabelsFile f = INT_VPP_ServerFiles.INT_ParseLabels(mainPath);
		if (!f && mainExists)
		{
			string rawMain;
			INT_VPP_ServerFiles.ReadText(mainPath, rawMain);
			int rawVer = INT_VPP_ServerFiles.INT_RawVersion(rawMain, "INT_FileVersion");
			if (rawVer > known)
			{
				s_INT_LabelsReadOnly = true;
				INT_VPP_ServerState.LogWarn("Labels.json INT_FileVersion " + rawVer.ToString() + " is newer than this build (" + known.ToString() + ") and unreadable here: no labels served, saves refused, the file is never rewritten in this session");
				f = new INT_VPP_LabelsFile();
			}
		}
		if (!f)
		{
			if (mainExists)
			{
				string kept = INT_VPP_ServerFiles.INT_SetAside(mainPath, "broken");
				INT_VPP_ServerState.LogWarn("Labels.json unreadable, copy: " + kept);
			}
			src = "tmp";
			f = INT_VPP_ServerFiles.INT_ParseLabels(tmp);
			if (!f)
			{
				src = "bak";
				f = INT_VPP_ServerFiles.INT_ParseLabels(bak);
			}
			if (!f)
			{
				src = "new";
				f = new INT_VPP_LabelsFile();
			}
			save = true;
			if (mainExists || src != "new")
				INT_VPP_ServerState.LogWarn("Labels.json taken from: " + src);
		}
		int fileVer = f.INT_FileVersion;
		if (fileVer > known && !s_INT_LabelsReadOnly)
		{
			s_INT_LabelsReadOnly = true;
			INT_VPP_ServerState.LogWarn("Labels.json (" + src + ") INT_FileVersion " + fileVer.ToString() + " is newer than this build (" + known.ToString() + "): labels served read-only, saves refused, never rewritten in this session");
		}

		array<ref INT_VPP_Label> srcList = f.INT_LabelList;
		array<ref INT_VPP_Label> valid = new array<ref INT_VPP_Label>();
		int dropped = 0;
		if (srcList)
		{
			foreach (INT_VPP_Label ll : srcList)
			{
				if (!ll)
				{
					dropped++;
					continue;
				}
				int llId = ll.INT_Id;
				if (valid.Count() >= INT_VPP_Net.INT_HARD_MAX_LABELS)
				{
					dropped++;
					INT_VPP_ServerState.LogWarn("label dropped id=" + llId.ToString() + " reason=more than the hard limit of labels");
					continue;
				}
				if (ll.INT_Color == 0)
				{
					int white = INT_VPP_Net.INT_LabelColorAt(9);
					ll.INT_Color = white;
				}
				if (ll.INT_Size == 0)
					ll.INT_Size = INT_VPP_Net.INT_LABEL_DEF_SIZE;
				string why;
				bool ok = INT_VPP_Validate.INT_LabelLimits(ll, why);
				if (!ok)
				{
					dropped++;
					INT_VPP_ServerState.LogWarn("label dropped id=" + llId.ToString() + " reason=" + why);
					continue;
				}
				valid.Insert(ll);
			}
		}
		int top = INT_VPP_LabelIds.INT_MaxId(valid);
		array<int> seen = new array<int>();
		int fixedIds = 0;
		foreach (INT_VPP_Label vl : valid)
		{
			int vid = vl.INT_Id;
			if (vid <= 0 || seen.Find(vid) != -1)
			{
				top++;
				vid = top;
				vl.INT_Id = vid;
				fixedIds++;
			}
			seen.Insert(vid);
		}
		int nid = INT_VPP_LabelIds.INT_Next(valid, f.INT_NextId);
		if (nid != f.INT_NextId)
			save = true;
		if (fixedIds > 0)
		{
			save = true;
			INT_VPP_ServerState.LogWarn("Labels.json: " + fixedIds.ToString() + " labels got a new id (missing or duplicate)");
		}
		if (dropped > 0)
		{
			save = true;
			if (mainExists && src == "main" && !s_INT_LabelsReadOnly)
			{
				string keptDropped = INT_VPP_ServerFiles.KeepCopy(mainPath, "dropped");
				INT_VPP_ServerState.LogWarn("Labels.json: " + dropped.ToString() + " labels dropped, original kept: " + keptDropped);
			}
		}
		if (!mainExists)
			save = true;
		if (!s_INT_LabelsReadOnly)
			f.INT_FileVersion = known;
		f.INT_NextId = nid;
		f.INT_LabelList = valid;
		s_INT_Labels = f;
		int cnt = valid.Count();
		int lim = INT_VPP_ServerState.INT_LabelLimit();
		if (cnt > lim)
			INT_VPP_ServerState.LogWarn("Labels.json holds " + cnt.ToString() + " labels, more than INT_MaxLabels: no new labels until some are deleted");
		if (save && !s_INT_LabelsReadOnly)
		{
			string err;
			if (!INT_VPP_ServerState.INT_SaveLabels(err))
				INT_VPP_ServerState.LogWarn("Labels.json not written: " + err);
		}
	}

	static bool INT_SaveLabels(out string err)
	{
		err = "";
		if (!s_INT_Labels)
		{
			err = "no data";
			return false;
		}
		if (s_INT_LabelsReadOnly)
		{
			err = INT_VPP_Net.INT_REASON_LABELS_READ_ONLY;
			return false;
		}
		JsonSerializer js = new JsonSerializer();
		string txt;
		bool built = js.WriteToString(s_INT_Labels, true, txt);
		if (!built)
		{
			err = "json build failed";
			return false;
		}
		string path = INT_VPP_ServerFiles.INT_LabelsPath();
		int cnt = s_INT_Labels.INT_LabelList.Count();
		int nid = s_INT_Labels.INT_NextId;
		string why;
		bool ok = INT_VPP_ServerFiles.ReplaceVerified(path, txt, INT_VPP_ServerFiles.INT_KIND_LABELS, cnt, nid, why);
		err = why;
		return ok;
	}

	// The live list: callers must not modify it.
	static array<ref INT_VPP_Label> INT_GetLabels()
	{
		INT_VPP_ServerState.EnsureLoaded();
		return s_INT_Labels.INT_LabelList;
	}

	// limit for NEW labels (Settings.json INT_MaxLabels, 0..500)
	static int INT_LabelLimit()
	{
		int lim = INT_VPP_Net.INT_DEF_MAX_LABELS;
		if (s_Settings)
			lim = s_Settings.INT_MaxLabels;
		int res = INT_VPP_Validate.ClampInt(lim, 0, INT_VPP_Net.INT_HARD_MAX_LABELS);
		return res;
	}

	static int INT_LastLabelId()
	{
		return s_INT_LastLabelId;
	}

	static bool INT_LabelsReadOnly()
	{
		return s_INT_LabelsReadOnly;
	}

	static int INT_NextLabelId()
	{
		INT_VPP_ServerState.EnsureLoaded();
		return s_INT_Labels.INT_NextId;
	}

	// ---------- queries ----------
	static INT_VPP_ServerSettings GetSettings()
	{
		INT_VPP_ServerState.EnsureLoaded();
		return s_Settings;
	}

	// The live list: callers must not modify it.
	static array<ref INT_VPP_Zone> GetZones()
	{
		INT_VPP_ServerState.EnsureLoaded();
		return s_Zones.ZoneList;
	}

	static int LastSavedId()
	{
		return s_LastSavedId;
	}

	static bool INT_SettingsReadOnly()
	{
		return s_SettingsReadOnly;
	}

	static bool INT_ZonesReadOnly()
	{
		return s_ZonesReadOnly;
	}

	static int INT_NextZoneId()
	{
		INT_VPP_ServerState.EnsureLoaded();
		return s_Zones.NextId;
	}

	static int NextMsgId()
	{
		s_MsgId++;
		if (s_MsgId > 1000000000)
			s_MsgId = 1;
		return s_MsgId;
	}

	// steam64 = PlayerIdentity.GetPlainId() of the ENGINE sender, never a payload field.
	static bool IsAdminId(string steam64)
	{
		INT_VPP_ServerState.EnsureLoaded();
		string id = steam64 + "";
		id.TrimInPlace();
		if (!INT_VPP_Validate.IsSteam64(id))
			return false;
		if (s_Admins && s_Admins.Find(id) != -1)
			return true;
		if (!s_Settings || s_Settings.UseVPPAdminToolsSuperAdmins != 1)
			return false;
		INT_VPP_ServerState.RefreshSuperAdmins(false);
		if (s_SuperAdmins && s_SuperAdmins.Find(id) != -1)
			return true;
		return false;
	}

	// ---------- rate limits (key = PlayerIdentity.GetId(), the uid MissionServer.PlayerDisconnected gets) ----------
	static bool RateAllow(string key)
	{
		INT_VPP_ServerState.EnsureMaps();
		int now = GetGame().GetTime();
		int at = 0;
		bool seen = s_RateAt.Find(key, at);
		int gap = now - at;
		if (!seen || gap >= 1000 || gap < 0)
		{
			s_RateAt.Set(key, now);
			s_RateCount.Set(key, 1);
			return true;
		}
		int cnt = 0;
		s_RateCount.Find(key, cnt);
		if (cnt >= INT_VPP_Net.ADMIN_WRITES_PER_SEC)
			return false;
		int nc = cnt + 1;
		s_RateCount.Set(key, nc);
		return true;
	}

	static bool HelloAllow(string key)
	{
		INT_VPP_ServerState.EnsureMaps();
		int now = GetGame().GetTime();
		int at = 0;
		bool seen = s_HelloAt.Find(key, at);
		if (seen)
		{
			int gap = now - at;
			if (gap >= 0 && gap < INT_VPP_Net.HELLO_MIN_GAP_MS)
				return false;
		}
		s_HelloAt.Set(key, now);
		return true;
	}

	// One answer per REPLY_MIN_GAP_MS per sender for what a refused sender may trigger (settings push, answer to a
	// request that was never read): a flood of refused requests cannot turn into a flood of outgoing RPCs.
	static bool INT_ReplyAllow(string key)
	{
		INT_VPP_ServerState.EnsureMaps();
		int now = GetGame().GetTime();
		int at = 0;
		bool seen = s_ReplyAt.Find(key, at);
		if (seen)
		{
			int gap = now - at;
			if (gap >= 0 && gap < INT_VPP_Net.REPLY_MIN_GAP_MS)
				return false;
		}
		s_ReplyAt.Set(key, now);
		return true;
	}

	// VPP's MarkerCheck / OwnPositionMarkerDisabled / SessionMarkers / ForceMapItem: each re-reads VPPMapConfig.json and
	// SessionMarkers sends the whole marker list, so a client may ask each one at most once per INT_VPP_RPC_GAP_MS
	// (the real client asks once, 5 s after the mission start). Extras are dropped (rate-limited REJECT line).
	static bool INT_VppRpcAllow(PlayerIdentity sender, string rpc)
	{
		if (!sender)
			return false;
		string uid = sender.GetId();
		string who = sender.GetPlainId();
		bool ok = INT_VPP_ServerState.INT_VppRpcAllowKey(uid, who, rpc);
		return ok;
	}

	static bool INT_VppRpcAllowKey(string uid, string who, string rpc)
	{
		INT_VPP_ServerState.EnsureMaps();
		int now = GetGame().GetTime();
		string key = uid + "|" + rpc;
		int at = 0;
		bool seen = s_INT_VppRpcAt.Find(key, at);
		if (seen)
		{
			int gap = now - at;
			if (gap >= 0 && gap < INT_VPP_RPC_GAP_MS)
			{
				INT_VPP_ServerState.LogReject(rpc, who, "too often");
				return false;
			}
		}
		s_INT_VppRpcAt.Set(key, now);
		return true;
	}

	// TestKit: how many of the 4 VPP RPC entries a sender has
	static int INT_VppRpcTracked(string uid)
	{
		if (!s_INT_VppRpcAt)
			return 0;
		array<string> rpcs = new array<string>();
		INT_VPP_ServerState.INT_VppRpcNames(rpcs);
		int n = 0;
		foreach (string r : rpcs)
		{
			string key = uid + "|" + r;
			if (s_INT_VppRpcAt.Contains(key))
				n++;
		}
		return n;
	}

	static void INT_VppRpcNames(array<string> dst)
	{
		dst.Insert("MarkerCheck");
		dst.Insert("OwnPositionMarkerDisabled");
		dst.Insert("SessionMarkers");
		dst.Insert("ForceMapItem");
	}

	static void Forget(string key)
	{
		if (s_RateAt)
			s_RateAt.Remove(key);
		if (s_RateCount)
			s_RateCount.Remove(key);
		if (s_HelloAt)
			s_HelloAt.Remove(key);
		if (s_ReplyAt)
			s_ReplyAt.Remove(key);
		if (s_INT_VppRpcAt)
		{
			array<string> rpcs = new array<string>();
			INT_VPP_ServerState.INT_VppRpcNames(rpcs);
			foreach (string r : rpcs)
			{
				string vk = key + "|" + r;
				s_INT_VppRpcAt.Remove(vk);
			}
		}
	}

	// ---------- mutations (admin check + validation + atomic save + broadcast) ----------
	// z is the request (never stored: a validated copy is). On success the new list goes to every client.
	static bool ApplyZoneSave(string senderId, INT_VPP_Zone z, out string reason)
	{
		reason = "";
		s_LastSavedId = 0;
		INT_VPP_ServerState.EnsureLoaded();
		if (!INT_VPP_ServerState.IsAdminId(senderId))
		{
			reason = "not admin";
			return false;
		}
		if (s_ZonesReadOnly)
		{
			reason = INT_VPP_Net.REASON_ZONES_READ_ONLY;
			return false;
		}
		if (!z)
		{
			reason = "null zone";
			return false;
		}
		int nx = 0;
		int nzc = 0;
		if (z.xs)
			nx = z.xs.Count();
		if (z.zs)
			nzc = z.zs.Count();
		if (nx > INT_VPP_Net.HARD_MAX_ZONE_VERTICES || nzc > INT_VPP_Net.HARD_MAX_ZONE_VERTICES)
		{
			reason = "too many vertices";
			return false;
		}
		INT_VPP_Zone c = z.Clone();
		string why;
		bool valid = INT_VPP_Validate.Zone(c, s_Settings, why);
		if (!valid)
		{
			reason = why;
			return false;
		}
		array<ref INT_VPP_Zone> list = s_Zones.ZoneList;
		int prevNext = s_Zones.NextId;
		int idx = -1;
		INT_VPP_Zone prev = null;
		bool isNew = false;
		if (c.id == 0)
		{
			if (list.Count() >= s_Settings.MaxZones)
			{
				reason = "zone limit";
				return false;
			}
			int newId = INT_VPP_ZoneIds.Next(list, s_Zones.NextId);
			c.id = newId;
			list.Insert(c);
			int afterId = newId + 1;
			s_Zones.NextId = afterId;
			isNew = true;
		}
		else
		{
			idx = INT_VPP_ZoneIds.IndexOf(list, c.id);
			if (idx < 0)
			{
				reason = "unknown id";
				return false;
			}
			prev = list.Get(idx);
			list.Set(idx, c);
		}
		string err;
		bool saved = INT_VPP_ServerState.SaveZones(err);
		if (!saved)
		{
			if (isNew)
			{
				int last = list.Count() - 1;
				if (last >= 0)
					list.RemoveOrdered(last);
			}
			else
			{
				list.Set(idx, prev);
			}
			s_Zones.NextId = prevNext;
			reason = "save failed: " + err;
			return false;
		}
		int savedId = c.id;
		s_LastSavedId = savedId;
		string nameLog = INT_VPP_Validate.LogText(c.name);
		string act = "updated";
		if (isNew)
			act = "created";
		INT_VPP_ServerState.LogInfo("zone " + act + " id=" + savedId.ToString() + " name=" + nameLog + " by=" + senderId);
		INT_VPP_ServerRpc.SendZones(null, savedId);
		return true;
	}

	static bool ApplyZoneDelete(string senderId, int id, out string reason)
	{
		reason = "";
		INT_VPP_ServerState.EnsureLoaded();
		if (!INT_VPP_ServerState.IsAdminId(senderId))
		{
			reason = "not admin";
			return false;
		}
		if (s_ZonesReadOnly)
		{
			reason = INT_VPP_Net.REASON_ZONES_READ_ONLY;
			return false;
		}
		if (id <= 0)
		{
			reason = "bad id";
			return false;
		}
		array<ref INT_VPP_Zone> list = s_Zones.ZoneList;
		int idx = INT_VPP_ZoneIds.IndexOf(list, id);
		if (idx < 0)
		{
			reason = "unknown id";
			return false;
		}
		INT_VPP_Zone prev = list.Get(idx);
		list.RemoveOrdered(idx);
		string err;
		bool saved = INT_VPP_ServerState.SaveZones(err);
		if (!saved)
		{
			list.InsertAt(prev, idx);
			reason = "save failed: " + err;
			return false;
		}
		string nameLog = INT_VPP_Validate.LogText(prev.name);
		INT_VPP_ServerState.LogInfo("zone deleted id=" + id.ToString() + " name=" + nameLog + " by=" + senderId);
		INT_VPP_ServerRpc.SendZones(null, id);
		return true;
	}

	// Admin text labels (2026-11-13). l is the request (never stored: a validated copy is). Success = the new list to
	// every client. Reasons: "not admin", "labels read-only", "null label", the INT_LabelLimits reasons, "label limit",
	// "unknown id", "bad id", "save failed: <detail>".
	static bool INT_ApplyLabelSave(string senderId, INT_VPP_Label l, out string reason)
	{
		reason = "";
		s_INT_LastLabelId = 0;
		INT_VPP_ServerState.EnsureLoaded();
		if (!INT_VPP_ServerState.IsAdminId(senderId))
		{
			reason = "not admin";
			return false;
		}
		if (s_INT_LabelsReadOnly)
		{
			reason = INT_VPP_Net.INT_REASON_LABELS_READ_ONLY;
			return false;
		}
		if (!l)
		{
			reason = "null label";
			return false;
		}
		INT_VPP_Label c = l.INT_Clone();
		string why;
		bool valid = INT_VPP_Validate.INT_LabelLimits(c, why);
		if (!valid)
		{
			reason = why;
			return false;
		}
		array<ref INT_VPP_Label> list = s_INT_Labels.INT_LabelList;
		int prevNext = s_INT_Labels.INT_NextId;
		int idx = -1;
		INT_VPP_Label prev = null;
		bool isNew = false;
		if (c.INT_Id == 0)
		{
			int lim = INT_VPP_ServerState.INT_LabelLimit();
			if (list.Count() >= lim)
			{
				reason = "label limit";
				return false;
			}
			int newId = INT_VPP_LabelIds.INT_Next(list, s_INT_Labels.INT_NextId);
			c.INT_Id = newId;
			list.Insert(c);
			int afterId = newId + 1;
			s_INT_Labels.INT_NextId = afterId;
			isNew = true;
		}
		else
		{
			idx = INT_VPP_LabelIds.INT_IndexOf(list, c.INT_Id);
			if (idx < 0)
			{
				reason = "unknown id";
				return false;
			}
			prev = list.Get(idx);
			list.Set(idx, c);
		}
		string err;
		bool saved = INT_VPP_ServerState.INT_SaveLabels(err);
		if (!saved)
		{
			if (isNew)
			{
				int last = list.Count() - 1;
				if (last >= 0)
					list.RemoveOrdered(last);
			}
			else
			{
				list.Set(idx, prev);
			}
			s_INT_Labels.INT_NextId = prevNext;
			reason = "save failed: " + err;
			return false;
		}
		int savedId = c.INT_Id;
		s_INT_LastLabelId = savedId;
		string textLog = INT_VPP_Validate.LogText(c.INT_Text);
		string act = "updated";
		if (isNew)
			act = "created";
		INT_VPP_ServerState.LogInfo("label " + act + " id=" + savedId.ToString() + " text=" + textLog + " by=" + senderId);
		INT_VPP_ServerRpc.INT_SendLabels(null, savedId);
		return true;
	}

	static bool INT_ApplyLabelDelete(string senderId, int id, out string reason)
	{
		reason = "";
		INT_VPP_ServerState.EnsureLoaded();
		if (!INT_VPP_ServerState.IsAdminId(senderId))
		{
			reason = "not admin";
			return false;
		}
		if (s_INT_LabelsReadOnly)
		{
			reason = INT_VPP_Net.INT_REASON_LABELS_READ_ONLY;
			return false;
		}
		if (id <= 0)
		{
			reason = "bad id";
			return false;
		}
		array<ref INT_VPP_Label> list = s_INT_Labels.INT_LabelList;
		int idx = INT_VPP_LabelIds.INT_IndexOf(list, id);
		if (idx < 0)
		{
			reason = "unknown id";
			return false;
		}
		INT_VPP_Label prev = list.Get(idx);
		list.RemoveOrdered(idx);
		string err;
		bool saved = INT_VPP_ServerState.INT_SaveLabels(err);
		if (!saved)
		{
			list.InsertAt(prev, idx);
			reason = "save failed: " + err;
			return false;
		}
		string textLog = INT_VPP_Validate.LogText(prev.INT_Text);
		INT_VPP_ServerState.LogInfo("label deleted id=" + id.ToString() + " text=" + textLog + " by=" + senderId);
		INT_VPP_ServerRpc.INT_SendLabels(null, id);
		return true;
	}
}

// Server markers edited in game (owner 2026-11-14). The list is VPP's own VPPMapConfig.json (INT_VPP_SE_Migrate
// .INT_MarkersPath(); the S21 converter output lives there too). Every change re-reads the file (admins may hand-edit
// it live, VPP re-reads it for every SessionMarkers too), changes one marker, writes the whole config through
// INT_VPP_ServerFiles.ReplaceVerified (kind INT_KIND_MARKERS: parsed back, marker count checked) and sends the new
// list to every client on VPP's old path (RPC_MissionGameplay / SessionMarkers). Before the FIRST in-game write of a
// config, a byte-identical copy «VPPMapConfig.json.before-ingame-edit-<UTC stamp>» is kept next to it, once per file.
// Markers are addressed by MarkerInfo.INT_SrvId: kept when > 0 and unique, else given in file order from the
// highest id + 1 (INT_AssignIds; the same rule before every send, so a client always holds the server's ids).
class INT_VPP_SrvMarkers
{
	static const string INT_FIRST_TAG = "before-ingame-edit";
	static int s_INT_LastId;
	static int s_INT_Backups;

	// ids: > 0 and unique stay; 0, negative or a repeat get max + 1, max + 2 ... in file order
	static void INT_AssignIds(array<ref MarkerInfo> list)
	{
		if (!list)
			return;
		map<int, bool> used = new map<int, bool>();
		int maxId = 0;
		foreach (MarkerInfo a : list)
		{
			if (!a)
				continue;
			int id = a.INT_GetSrvId();
			if (id <= 0 || used.Contains(id))
			{
				a.INT_SetSrvId(0);
				continue;
			}
			used.Set(id, true);
			if (id > maxId)
				maxId = id;
		}
		foreach (MarkerInfo b : list)
		{
			if (!b || b.INT_GetSrvId() > 0)
				continue;
			maxId++;
			b.INT_SetSrvId(maxId);
		}
	}

	// the config as it is on disk now (null = missing / unreadable: no in-game change then)
	static VPPMapConfig INT_Read(out string err)
	{
		err = "";
		string path = INT_VPP_SE_Migrate.INT_MarkersPath();
		string txt;
		if (!INT_VPP_ServerFiles.ReadText(path, txt))
		{
			err = "no VPPMapConfig.json";
			return null;
		}
		VPPMapConfig cfg = new VPPMapConfig();
		JsonSerializer js = new JsonSerializer();
		string perr;
		bool ok = js.ReadFromString(cfg, txt, perr);
		if (!ok)
		{
			err = "VPPMapConfig.json unreadable";
			return null;
		}
		array<ref MarkerInfo> list = cfg.GetCachedArray();
		if (!list)
		{
			list = new array<ref MarkerInfo>();
			cfg.SetCachedArray(list);
		}
		INT_VPP_SrvMarkers.INT_AssignIds(list);
		return cfg;
	}

	static int INT_IndexOf(array<ref MarkerInfo> list, int id)
	{
		if (!list || id <= 0)
			return -1;
		int n = list.Count();
		for (int i = 0; i < n; i++)
		{
			MarkerInfo m = list.Get(i);
			if (m && m.INT_GetSrvId() == id)
				return i;
		}
		return -1;
	}

	static int INT_Limit()
	{
		int lim = INT_VPP_Net.INT_DEF_MAX_SRV_MARKERS;
		INT_VPP_ServerSettings ss = INT_VPP_ServerState.GetSettings();
		if (ss)
			lim = ss.INT_MaxServerMarkers;
		int res = INT_VPP_Validate.ClampInt(lim, 0, INT_VPP_Net.INT_HARD_MAX_SRV_MARKERS);
		return res;
	}

	// one byte-identical copy of the config before the first in-game write (never again while one exists)
	static bool INT_FirstBackup(string path, out string err)
	{
		err = "";
		if (!FileExist(path))
			return true;
		string dir = path;
		int cut = -1;
		int n = path.Length();
		for (int i = 0; i < n; i++)
		{
			string ch = path.Get(i);
			if (ch == "/" || ch == "\\")
				cut = i;
		}
		if (cut >= 0)
			dir = path.Substring(0, cut);
		string pattern = dir + "/VPPMapConfig.json." + INT_FIRST_TAG + "-*";
		string found;
		FileAttr attr;
		FindFileHandle fh = FindFile(pattern, found, attr, 0);
		if (fh)
		{
			CloseFindFile(fh);
			return true;
		}
		string copy = INT_VPP_ServerFiles.KeepCopy(path, INT_FIRST_TAG);
		if (copy == "" || !INT_VPP_ServerFiles.INT_SameFile(path, copy))
		{
			err = "first backup of VPPMapConfig.json failed";
			return false;
		}
		s_INT_Backups++;
		INT_VPP_ServerState.LogInfo("VPPMapConfig.json kept as " + copy + " before the first in-game marker change");
		return true;
	}

	static bool INT_Write(VPPMapConfig cfg, out string err)
	{
		err = "";
		string path = INT_VPP_SE_Migrate.INT_MarkersPath();
		if (!INT_VPP_SrvMarkers.INT_FirstBackup(path, err))
			return false;
		JsonSerializer js = new JsonSerializer();
		string txt;
		bool ok = js.WriteToString(cfg, true, txt);
		if (!ok || txt == "")
		{
			err = "cannot serialise VPPMapConfig.json";
			return false;
		}
		array<ref MarkerInfo> list = cfg.GetCachedArray();
		int cnt = list.Count();
		string why;
		bool saved = INT_VPP_ServerFiles.ReplaceVerified(path, txt, INT_VPP_ServerFiles.INT_KIND_MARKERS, cnt, 0, why);
		if (!saved)
		{
			err = why;
			return false;
		}
		return true;
	}

	// the new list to every client, on VPP's own path (old clients read it too: same payload)
	static void INT_Broadcast(VPPMapConfig cfg)
	{
		if (!cfg)
			return;
		array<ref MarkerInfo> list = cfg.GetCachedArray();
		if (!list)
			return;
		foreach (MarkerInfo m : list)
		{
			if (!m)
				continue;
			string iconPath = m.GetIconPath();
			iconPath.Replace("\\scripts\\", "\\");
			m.SetIconPath(iconPath);
		}
		Param1<ref array<ref MarkerInfo>> prm = new Param1<ref array<ref MarkerInfo>>(list);
		GetRPCManager().SendRPC("RPC_MissionGameplay", "SessionMarkers", prm, true, null);
	}

	// name / colour / position / icon of a request, cleaned into `out` (the stored marker `prev` keeps its own icon
	// when the request did not change it, even one outside our sets: a hand-written config may name any texture)
	static bool INT_Check(MarkerInfo req, MarkerInfo prev, out string reason)
	{
		reason = "";
		if (!req)
		{
			reason = "no marker";
			return false;
		}
		string why;
		if (!INT_VPP_Validate.INT_SrvMarkerLimits(req, why))
		{
			reason = why;
			return false;
		}
		string icon = req.GetIconPath();
		bool iconOk = false;
		if (INT_VPP_Icons.INT_IsOwnPath(icon) || INT_VPP_Icons.INT_IsLegacyPath(icon))
			iconOk = true;
		if (!iconOk && prev && icon == prev.GetIconPath())
			iconOk = true;
		if (!iconOk)
		{
			reason = "icon not allowed";
			return false;
		}
		return true;
	}

	static bool INT_ApplySave(string senderId, string senderName, MarkerInfo req, out string reason)
	{
		reason = "";
		s_INT_LastId = 0;
		INT_VPP_ServerState.EnsureLoaded();
		if (!INT_VPP_ServerState.IsAdminId(senderId))
		{
			reason = "not admin";
			return false;
		}
		string err;
		VPPMapConfig cfg = INT_VPP_SrvMarkers.INT_Read(err);
		if (!cfg)
		{
			reason = err;
			return false;
		}
		array<ref MarkerInfo> list = cfg.GetCachedArray();
		int askedId = 0;
		if (req)
			askedId = req.INT_GetSrvId();
		int idx = -1;
		MarkerInfo prev = null;
		if (askedId > 0)
		{
			idx = INT_VPP_SrvMarkers.INT_IndexOf(list, askedId);
			if (idx < 0)
			{
				reason = "unknown id";
				return false;
			}
			prev = list.Get(idx);
		}
		else if (list.Count() >= INT_VPP_SrvMarkers.INT_Limit())
		{
			reason = "server marker limit";
			return false;
		}
		if (!INT_VPP_SrvMarkers.INT_Check(req, prev, reason))
			return false;
		string nm = req.GetName();
		string icon = req.GetIconPath();
		vector col = req.GetColor();
		vector pos = req.GetPosition();
		bool act = req.IsActive();
		bool act3d = req.Is3DActive();
		bool hideName = req.INT_IsName3DHidden();
		MarkerInfo made = new MarkerInfo(nm, icon, col, pos, act, act3d);
		made.INT_SetName3DHidden(hideName);
		bool isNew = false;
		if (idx < 0)
		{
			int maxId = 0;
			foreach (MarkerInfo o : list)
			{
				if (o && o.INT_GetSrvId() > maxId)
					maxId = o.INT_GetSrvId();
			}
			int newId = maxId + 1;
			made.INT_SetSrvId(newId);
			list.Insert(made);
			isNew = true;
		}
		else
		{
			made.INT_SetSrvId(askedId);
			list.Set(idx, made);
		}
		string werr;
		if (!INT_VPP_SrvMarkers.INT_Write(cfg, werr))
		{
			reason = "save failed: " + werr;
			return false;
		}
		int savedId = made.INT_GetSrvId();
		s_INT_LastId = savedId;
		string act2 = "updated";
		if (isNew)
			act2 = "created";
		string nmLog = INT_VPP_Validate.LogText(nm);
		string byLog = INT_VPP_Validate.LogText(senderName);
		int px = Math.Round(pos[0]);
		int pz = Math.Round(pos[2]);
		INT_VPP_ServerState.LogInfo("server marker " + act2 + " id=" + savedId.ToString() + " name=" + nmLog + " at=" + px.ToString() + "," + pz.ToString() + " by=" + byLog + " (" + senderId + ")");
		INT_VPP_SrvMarkers.INT_Broadcast(cfg);
		return true;
	}

	static bool INT_ApplyDelete(string senderId, string senderName, int id, out string reason)
	{
		reason = "";
		INT_VPP_ServerState.EnsureLoaded();
		if (!INT_VPP_ServerState.IsAdminId(senderId))
		{
			reason = "not admin";
			return false;
		}
		if (id <= 0)
		{
			reason = "bad id";
			return false;
		}
		string err;
		VPPMapConfig cfg = INT_VPP_SrvMarkers.INT_Read(err);
		if (!cfg)
		{
			reason = err;
			return false;
		}
		array<ref MarkerInfo> list = cfg.GetCachedArray();
		int idx = INT_VPP_SrvMarkers.INT_IndexOf(list, id);
		if (idx < 0)
		{
			reason = "unknown id";
			return false;
		}
		MarkerInfo prev = list.Get(idx);
		string nmLog = INT_VPP_Validate.LogText(prev.GetName());
		list.RemoveOrdered(idx);
		string werr;
		if (!INT_VPP_SrvMarkers.INT_Write(cfg, werr))
		{
			reason = "save failed: " + werr;
			return false;
		}
		string byLog = INT_VPP_Validate.LogText(senderName);
		INT_VPP_ServerState.LogInfo("server marker deleted id=" + id.ToString() + " name=" + nmLog + " by=" + byLog + " (" + senderId + ")");
		INT_VPP_SrvMarkers.INT_Broadcast(cfg);
		return true;
	}
}

// CF RPC handlers + senders. CF calls the handler methods BY NAME (= INT_VPP_Net.RPC_* strings).
class INT_VPP_ServerRpc : Managed
{
	// TestKit evidence (written from locals only): handler bodies that reached ctx.Read, last answer built.
	static int s_INT_WriteReads;
	static int s_INT_HelloReads;
	static int s_INT_ResultCount;
	static int s_INT_ResultKind;
	static int s_INT_ResultId;
	static bool s_INT_ResultOk;
	static string s_INT_ResultReason;
	static int s_INT_PushCount;
	static bool s_INT_PushAdmin;
	static int s_INT_RepliesDropped;
	// admin text labels (2026-11-13): INT_SendLabels calls to everybody (identity null) / to one client
	static int s_INT_LabelBroadcasts;
	static int s_INT_LabelSends;

	protected bool m_Registered;

	void INT_Register()
	{
		if (m_Registered)
			return;
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_HELLO, this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_ZONE_SAVE, this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_ZONE_DELETE, this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_LABEL_SAVE, this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_LABEL_DELETE, this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_SRVMARKER_SAVE, this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_SRVMARKER_DELETE, this, SingleplayerExecutionType.Server);
		m_Registered = true;
	}

	// ---------- CF entry points: side + engine sender only, then the static handlers below ----------
	// client -> server: Param1<int protocol>. Answer (to the sender only): INT_Settings + INT_Zones.
	void INT_Hello(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server)
			return;
		if (!GetGame().IsDedicatedServer())
			return;
		if (!sender)
			return;
		string key = sender.GetId();
		string who = sender.GetPlainId();
		INT_VPP_ServerRpc.INT_HandleHello(key, who, sender, ctx);
	}

	// client -> server: Param2<int protocol, INT_VPP_Zone>. Admin only, 4/s per sender.
	void INT_ZoneSave(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server)
			return;
		if (!GetGame().IsDedicatedServer())
			return;
		if (!sender)
			return;
		string key = sender.GetId();
		string who = sender.GetPlainId();
		INT_VPP_ServerRpc.INT_HandleZoneSave(key, who, sender, ctx);
	}

	// client -> server: Param2<int protocol, int id>. Admin only, shares the 4/s budget with saves.
	void INT_ZoneDelete(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server)
			return;
		if (!GetGame().IsDedicatedServer())
			return;
		if (!sender)
			return;
		string key = sender.GetId();
		string who = sender.GetPlainId();
		INT_VPP_ServerRpc.INT_HandleZoneDelete(key, who, sender, ctx);
	}

	// client -> server: Param2<int protocol, INT_VPP_Label>. Admin only, shares the 4/s budget with the zone writes.
	void INT_LabelSave(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server)
			return;
		if (!GetGame().IsDedicatedServer())
			return;
		if (!sender)
			return;
		string key = sender.GetId();
		string who = sender.GetPlainId();
		INT_VPP_ServerRpc.INT_HandleLabelSave(key, who, sender, ctx);
	}

	// client -> server: Param2<int protocol, int id>. Admin only, same budget.
	void INT_LabelDelete(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server)
			return;
		if (!GetGame().IsDedicatedServer())
			return;
		if (!sender)
			return;
		string key = sender.GetId();
		string who = sender.GetPlainId();
		INT_VPP_ServerRpc.INT_HandleLabelDelete(key, who, sender, ctx);
	}

	// client -> server (2026-11-14): Param2<int protocol, MarkerInfo> (INT_SrvId 0 = new). Admin only, same budget.
	void INT_SrvMarkerSave(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server)
			return;
		if (!GetGame().IsDedicatedServer())
			return;
		if (!sender)
			return;
		string key = sender.GetId();
		string who = sender.GetPlainId();
		string nm = sender.GetName();
		INT_VPP_ServerRpc.INT_HandleSrvMarkerSave(key, who, nm, sender, ctx);
	}

	// client -> server (2026-11-14): Param2<int protocol, int id>. Admin only, same budget.
	void INT_SrvMarkerDelete(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server)
			return;
		if (!GetGame().IsDedicatedServer())
			return;
		if (!sender)
			return;
		string key = sender.GetId();
		string who = sender.GetPlainId();
		string nm = sender.GetName();
		INT_VPP_ServerRpc.INT_HandleSrvMarkerDelete(key, who, nm, sender, ctx);
	}

	// Server markers (2026-11-14): INT_GateWrite (admin + 4/s budget) FIRST, ctx.Read only after; answers on
	// INT_LabelResult with kinds 5 / 6.
	static void INT_HandleSrvMarkerSave(string key, string who, string whoName, PlayerIdentity replyTo, ParamsReadContext ctx)
	{
		string rpc = INT_VPP_Net.INT_RPC_SRVMARKER_SAVE;
		int kind = INT_VPP_Net.INT_RESULT_KIND_SRVMARKER_SAVE;
		if (!INT_VPP_ServerRpc.INT_GateWrite(rpc, kind, key, who, replyTo))
			return;
		s_INT_WriteReads++;
		Param2<int, ref MarkerInfo> data;
		if (!ctx.Read(data) || !data)
		{
			INT_VPP_ServerState.LogReject(rpc, who, "read failed");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "read failed", false);
			return;
		}
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			string whyProto = "protocol " + intProto.ToString();
			INT_VPP_ServerState.LogReject(rpc, who, whyProto);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, whyProto, false);
			return;
		}
		MarkerInfo m = data.param2;
		int askedId = 0;
		if (m)
			askedId = m.INT_GetSrvId();
		string reason;
		bool ok = INT_VPP_SrvMarkers.INT_ApplySave(who, whoName, m, reason);
		if (!ok)
		{
			INT_VPP_ServerState.LogReject(rpc, who, reason);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, askedId, reason, false);
			return;
		}
		int savedId = INT_VPP_SrvMarkers.s_INT_LastId;
		INT_VPP_ServerRpc.INT_SendResult(replyTo, kind, savedId, true, "");
	}

	static void INT_HandleSrvMarkerDelete(string key, string who, string whoName, PlayerIdentity replyTo, ParamsReadContext ctx)
	{
		string rpc = INT_VPP_Net.INT_RPC_SRVMARKER_DELETE;
		int kind = INT_VPP_Net.INT_RESULT_KIND_SRVMARKER_DELETE;
		if (!INT_VPP_ServerRpc.INT_GateWrite(rpc, kind, key, who, replyTo))
			return;
		s_INT_WriteReads++;
		Param2<int, int> data;
		if (!ctx.Read(data) || !data)
		{
			INT_VPP_ServerState.LogReject(rpc, who, "read failed");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "read failed", false);
			return;
		}
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			string whyProto = "protocol " + intProto.ToString();
			INT_VPP_ServerState.LogReject(rpc, who, whyProto);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, whyProto, false);
			return;
		}
		int id = data.param2;
		string reason;
		bool ok = INT_VPP_SrvMarkers.INT_ApplyDelete(who, whoName, id, reason);
		if (!ok)
		{
			INT_VPP_ServerState.LogReject(rpc, who, reason);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, id, reason, false);
			return;
		}
		INT_VPP_ServerRpc.INT_SendResult(replyTo, kind, id, true, "");
	}

	// ---------- handlers (key = sender.GetId(), who = sender.GetPlainId(); replyTo null = TestKit, nothing sent) ----------
	// Order (dayz-server-authority R2/R3/R6): who + budget FIRST, ctx.Read only after. A 1024+ byte string throws
	// inside ctx.Read and a huge array is fully deserialised, so a non-admin / flooding sender must never get there.
	static void INT_HandleHello(string key, string who, PlayerIdentity replyTo, ParamsReadContext ctx)
	{
		string rpc = INT_VPP_Net.RPC_HELLO;
		if (!INT_VPP_ServerState.HelloAllow(key))
		{
			INT_VPP_ServerState.LogReject(rpc, who, "hello too often");
			return;
		}
		s_INT_HelloReads++;
		Param1<int> data;
		if (!ctx.Read(data))
		{
			INT_VPP_ServerState.LogReject(rpc, who, "read failed");
			return;
		}
		if (!data)
			return;
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			string whyProto = "protocol " + intProto.ToString();
			INT_VPP_ServerState.LogReject(rpc, who, whyProto);
			return;
		}
		bool admin = INT_VPP_ServerState.IsAdminId(who);
		INT_VPP_ServerRpc.INT_PushSettings(replyTo, admin);
		// 2026-11-13: a server that replaced RF_MAP lets its clients import the players' RF_MAP markers
		INT_VPP_ServerRpc.INT_SendRFImport(replyTo);
		if (!replyTo)
			return;
		INT_VPP_ServerRpc.SendZones(replyTo, 0);
		INT_VPP_ServerRpc.INT_SendLabels(replyTo, 0);
	}

	static void INT_HandleZoneSave(string key, string who, PlayerIdentity replyTo, ParamsReadContext ctx)
	{
		string rpc = INT_VPP_Net.RPC_ZONE_SAVE;
		int kind = INT_VPP_Net.RESULT_KIND_SAVE;
		if (!INT_VPP_ServerRpc.INT_GateWrite(rpc, kind, key, who, replyTo))
			return;
		s_INT_WriteReads++;
		Param2<int, ref INT_VPP_Zone> data;
		if (!ctx.Read(data))
		{
			INT_VPP_ServerState.LogReject(rpc, who, "read failed");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "read failed", false);
			return;
		}
		if (!data)
		{
			INT_VPP_ServerState.LogReject(rpc, who, "read failed");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "read failed", false);
			return;
		}
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			string whyProto = "protocol " + intProto.ToString();
			INT_VPP_ServerState.LogReject(rpc, who, whyProto);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, whyProto, false);
			return;
		}
		INT_VPP_Zone z = data.param2;
		int askedId = 0;
		if (z)
			askedId = z.id;
		string reason;
		bool ok = INT_VPP_ServerState.ApplyZoneSave(who, z, reason);
		if (!ok)
		{
			INT_VPP_ServerState.LogReject(rpc, who, reason);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, askedId, reason, false);
			return;
		}
		int savedId = INT_VPP_ServerState.LastSavedId();
		INT_VPP_ServerRpc.INT_SendResult(replyTo, kind, savedId, true, "");
	}

	static void INT_HandleZoneDelete(string key, string who, PlayerIdentity replyTo, ParamsReadContext ctx)
	{
		string rpc = INT_VPP_Net.RPC_ZONE_DELETE;
		int kind = INT_VPP_Net.RESULT_KIND_DELETE;
		if (!INT_VPP_ServerRpc.INT_GateWrite(rpc, kind, key, who, replyTo))
			return;
		s_INT_WriteReads++;
		Param2<int, int> data;
		if (!ctx.Read(data))
		{
			INT_VPP_ServerState.LogReject(rpc, who, "read failed");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "read failed", false);
			return;
		}
		if (!data)
		{
			INT_VPP_ServerState.LogReject(rpc, who, "read failed");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "read failed", false);
			return;
		}
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			string whyProto = "protocol " + intProto.ToString();
			INT_VPP_ServerState.LogReject(rpc, who, whyProto);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, whyProto, false);
			return;
		}
		int id = data.param2;
		string reason;
		bool ok = INT_VPP_ServerState.ApplyZoneDelete(who, id, reason);
		if (!ok)
		{
			INT_VPP_ServerState.LogReject(rpc, who, reason);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, id, reason, false);
			return;
		}
		INT_VPP_ServerRpc.INT_SendResult(replyTo, kind, id, true, "");
	}

	// Admin text labels (2026-11-13): the same order as the zone handlers - INT_GateWrite (admin + 4/s budget) FIRST,
	// ctx.Read only after; every answer is an INT_LabelResult (kinds 3 / 4).
	static void INT_HandleLabelSave(string key, string who, PlayerIdentity replyTo, ParamsReadContext ctx)
	{
		string rpc = INT_VPP_Net.INT_RPC_LABEL_SAVE;
		int kind = INT_VPP_Net.INT_RESULT_KIND_LABEL_SAVE;
		if (!INT_VPP_ServerRpc.INT_GateWrite(rpc, kind, key, who, replyTo))
			return;
		s_INT_WriteReads++;
		Param2<int, ref INT_VPP_Label> data;
		if (!ctx.Read(data))
		{
			INT_VPP_ServerState.LogReject(rpc, who, "read failed");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "read failed", false);
			return;
		}
		if (!data)
		{
			INT_VPP_ServerState.LogReject(rpc, who, "read failed");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "read failed", false);
			return;
		}
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			string whyProto = "protocol " + intProto.ToString();
			INT_VPP_ServerState.LogReject(rpc, who, whyProto);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, whyProto, false);
			return;
		}
		INT_VPP_Label l = data.param2;
		int askedId = 0;
		if (l)
			askedId = l.INT_Id;
		string reason;
		bool ok = INT_VPP_ServerState.INT_ApplyLabelSave(who, l, reason);
		if (!ok)
		{
			INT_VPP_ServerState.LogReject(rpc, who, reason);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, askedId, reason, false);
			return;
		}
		int savedId = INT_VPP_ServerState.INT_LastLabelId();
		INT_VPP_ServerRpc.INT_SendResult(replyTo, kind, savedId, true, "");
	}

	static void INT_HandleLabelDelete(string key, string who, PlayerIdentity replyTo, ParamsReadContext ctx)
	{
		string rpc = INT_VPP_Net.INT_RPC_LABEL_DELETE;
		int kind = INT_VPP_Net.INT_RESULT_KIND_LABEL_DELETE;
		if (!INT_VPP_ServerRpc.INT_GateWrite(rpc, kind, key, who, replyTo))
			return;
		s_INT_WriteReads++;
		Param2<int, int> data;
		if (!ctx.Read(data))
		{
			INT_VPP_ServerState.LogReject(rpc, who, "read failed");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "read failed", false);
			return;
		}
		if (!data)
		{
			INT_VPP_ServerState.LogReject(rpc, who, "read failed");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "read failed", false);
			return;
		}
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			string whyProto = "protocol " + intProto.ToString();
			INT_VPP_ServerState.LogReject(rpc, who, whyProto);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, whyProto, false);
			return;
		}
		int id = data.param2;
		string reason;
		bool ok = INT_VPP_ServerState.INT_ApplyLabelDelete(who, id, reason);
		if (!ok)
		{
			INT_VPP_ServerState.LogReject(rpc, who, reason);
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, id, reason, false);
			return;
		}
		INT_VPP_ServerRpc.INT_SendResult(replyTo, kind, id, true, "");
	}

	// Admin (engine sender's Steam64, SuperAdmins.txt re-read at most every 10 s) and the 4/s write budget, both
	// BEFORE the payload is read. Refused: one log line (rate-limited) + a rate-limited answer with fresh settings.
	static bool INT_GateWrite(string rpc, int kind, string key, string who, PlayerIdentity replyTo)
	{
		if (!INT_VPP_ServerState.IsAdminId(who))
		{
			INT_VPP_ServerState.LogReject(rpc, who, "not admin");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "not admin", true);
			return false;
		}
		if (!INT_VPP_ServerState.RateAllow(key))
		{
			INT_VPP_ServerState.LogReject(rpc, who, "rate limit");
			INT_VPP_ServerRpc.INT_ReplyFail(replyTo, key, who, kind, 0, "too fast", true);
			return false;
		}
		return true;
	}

	// Refusal answer. limited = the request was not read (not admin / too fast): the answer itself is rate-limited.
	// Otherwise (an admin's read request, already inside the 4/s budget) the result always goes; the settings push
	// (current IsAdmin + limits) at most once per REPLY_MIN_GAP_MS per sender.
	static void INT_ReplyFail(PlayerIdentity replyTo, string key, string who, int kind, int zoneId, string reason, bool limited)
	{
		bool allow = INT_VPP_ServerState.INT_ReplyAllow(key);
		if (limited && !allow)
		{
			s_INT_RepliesDropped++;
			return;
		}
		INT_VPP_ServerRpc.INT_SendResult(replyTo, kind, zoneId, false, reason);
		if (!allow)
			return;
		bool admin = INT_VPP_ServerState.IsAdminId(who);
		INT_VPP_ServerRpc.INT_PushSettings(replyTo, admin);
	}

	static void INT_PushSettings(PlayerIdentity replyTo, bool admin)
	{
		s_INT_PushCount++;
		s_INT_PushAdmin = admin;
		if (!replyTo)
			return;
		INT_VPP_ServerRpc.SendSettingsTo(replyTo, admin);
	}

	// server -> the SENDER only: Param5<int protocol, int kind, int zoneId, bool ok, string reason (<= 200 B)>.
	// Kinds 1 / 2 (zones) travel as INT_ZoneResult, kinds 3 / 4 (labels, 2026-11-13) as INT_LabelResult.
	static void INT_SendResult(PlayerIdentity identity, int kind, int zoneId, bool ok, string reason)
	{
		string r = INT_VPP_Validate.CleanText(reason, INT_VPP_Net.RESULT_REASON_MAX_BYTES);
		s_INT_ResultCount++;
		s_INT_ResultKind = kind;
		s_INT_ResultId = zoneId;
		s_INT_ResultOk = ok;
		s_INT_ResultReason = r;
		if (!identity)
			return;
		int intProto = INT_VPP_Net.PROTOCOL;
		string fn = INT_VPP_Net.RPC_ZONE_RESULT;
		if (kind == INT_VPP_Net.INT_RESULT_KIND_LABEL_SAVE || kind == INT_VPP_Net.INT_RESULT_KIND_LABEL_DELETE)
			fn = INT_VPP_Net.INT_RPC_LABEL_RESULT;
		// server markers (2026-11-14) answer on the labels' result RPC (old clients ignore unknown kinds)
		if (kind == INT_VPP_Net.INT_RESULT_KIND_SRVMARKER_SAVE || kind == INT_VPP_Net.INT_RESULT_KIND_SRVMARKER_DELETE)
			fn = INT_VPP_Net.INT_RPC_LABEL_RESULT;
		Param5<int, int, int, bool, string> prm = new Param5<int, int, int, bool, string>(intProto, kind, zoneId, ok, r);
		GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, fn, prm, true, identity);
	}

	// server -> ONE client (2026-11-13): Param2<int protocol, int on = 1>, only when Settings.json has
	// INT_ImportRFMapMarkers 1 (a server without the setting sends nothing, so old clients never see the name).
	// identity null = TestKit: counted, nothing sent.
	static int s_INT_RFImportSent;

	static void INT_SendRFImport(PlayerIdentity identity)
	{
		INT_VPP_ServerSettings ss = INT_VPP_ServerState.GetSettings();
		if (!ss)
			return;
		if (ss.INT_ImportRFMapMarkers != 1)
			return;
		s_INT_RFImportSent++;
		if (!identity)
			return;
		int intProto = INT_VPP_Net.PROTOCOL;
		int on = 1;
		Param2<int, int> prm = new Param2<int, int>(intProto, on);
		GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_RF_IMPORT, prm, true, identity);
	}

	static int INT_RFImportSent()
	{
		return INT_VPP_ServerRpc.s_INT_RFImportSent;
	}

	// server -> ONE client: its own admin flag + public numbers.
	static void SendSettingsTo(PlayerIdentity identity, bool admin)
	{
		if (!identity)
			return;
		INT_VPP_ServerSettings ss = INT_VPP_ServerState.GetSettings();
		INT_VPP_ClientSettings cs = new INT_VPP_ClientSettings();
		cs.CopyFrom(ss, admin);
		int intProto = INT_VPP_Net.PROTOCOL;
		cs.Protocol = intProto;
		Param2<int, ref INT_VPP_ClientSettings> prm = new Param2<int, ref INT_VPP_ClientSettings>(intProto, cs);
		GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_SETTINGS, prm, true, identity);
	}

	// server -> one client (identity) or ALL clients (null): the full zone list in parts of ZONES_PER_PART.
	static void SendZones(PlayerIdentity identity, int changedId)
	{
		array<ref INT_VPP_Zone> all = INT_VPP_ServerState.GetZones();
		int n = 0;
		if (all)
			n = all.Count();
		int per = INT_VPP_Net.ZONES_PER_PART;
		int parts = (n + per - 1) / per;
		if (parts < 1)
			parts = 1;
		int msgId = INT_VPP_ServerState.NextMsgId();
		int intProto = INT_VPP_Net.PROTOCOL;
		for (int p = 0; p < parts; p++)
		{
			array<ref INT_VPP_Zone> chunk = new array<ref INT_VPP_Zone>();
			int first = p * per;
			int last = first + per;
			if (last > n)
				last = n;
			for (int k = first; k < last; k++)
			{
				INT_VPP_Zone zk = all.Get(k);
				chunk.Insert(zk);
			}
			Param6<int, int, int, int, int, ref array<ref INT_VPP_Zone>> prm = new Param6<int, int, int, int, int, ref array<ref INT_VPP_Zone>>(intProto, msgId, p, parts, changedId, chunk);
			GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_ZONES, prm, true, identity);
		}
	}

	// server -> one client (identity) or ALL clients (null): the full label list in parts of INT_LABELS_PER_PART
	// (10 x ~140 B), with the limit for new labels (Settings.json INT_MaxLabels) in every part.
	static void INT_SendLabels(PlayerIdentity identity, int changedId)
	{
		if (identity)
			s_INT_LabelSends++;
		else
			s_INT_LabelBroadcasts++;
		array<ref INT_VPP_Label> all = INT_VPP_ServerState.INT_GetLabels();
		int n = 0;
		if (all)
			n = all.Count();
		int per = INT_VPP_Net.INT_LABELS_PER_PART;
		int parts = (n + per - 1) / per;
		if (parts < 1)
			parts = 1;
		int msgId = INT_VPP_ServerState.NextMsgId();
		int intProto = INT_VPP_Net.PROTOCOL;
		int maxLabels = INT_VPP_ServerState.INT_LabelLimit();
		for (int p = 0; p < parts; p++)
		{
			array<ref INT_VPP_Label> chunk = new array<ref INT_VPP_Label>();
			int first = p * per;
			int last = first + per;
			if (last > n)
				last = n;
			for (int k = first; k < last; k++)
			{
				INT_VPP_Label lk = all.Get(k);
				chunk.Insert(lk);
			}
			Param7<int, int, int, int, int, int, ref array<ref INT_VPP_Label>> prm = new Param7<int, int, int, int, int, int, ref array<ref INT_VPP_Label>>(intProto, msgId, p, parts, changedId, maxLabels, chunk);
			GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_LABELS, prm, true, identity);
		}
	}
}

// Static hooks for a TestKit (server side, no RPC needed). Steam64 strings stand for the engine sender.
class INT_VPP_ServerApi
{
	static bool IsAdminId(string steam64)
	{
		return INT_VPP_ServerState.IsAdminId(steam64);
	}

	static int ZonesCount()
	{
		array<ref INT_VPP_Zone> list = INT_VPP_ServerState.GetZones();
		if (!list)
			return 0;
		return list.Count();
	}

	// A copy (tests must not edit the live list).
	static INT_VPP_Zone FindZone(int id)
	{
		array<ref INT_VPP_Zone> list = INT_VPP_ServerState.GetZones();
		int idx = INT_VPP_ZoneIds.IndexOf(list, id);
		if (idx < 0)
			return null;
		INT_VPP_Zone live = list.Get(idx);
		return live.Clone();
	}

	static bool ApplyZoneSave(string senderId, INT_VPP_Zone z, out string reason)
	{
		string why;
		bool ok = INT_VPP_ServerState.ApplyZoneSave(senderId, z, why);
		reason = why;
		return ok;
	}

	static bool ApplyZoneDelete(string senderId, int id, out string reason)
	{
		string why;
		bool ok = INT_VPP_ServerState.ApplyZoneDelete(senderId, id, why);
		reason = why;
		return ok;
	}

	static int LastSavedId()
	{
		return INT_VPP_ServerState.LastSavedId();
	}

	static bool RateAllow(string key)
	{
		return INT_VPP_ServerState.RateAllow(key);
	}

	static bool HelloAllow(string key)
	{
		return INT_VPP_ServerState.HelloAllow(key);
	}

	static void ForgetSender(string key)
	{
		INT_VPP_ServerState.Forget(key);
	}

	// VPP's 4 old RPCs (2026-11-12 review): the same gate the handlers use, with a fake sender key
	static bool INT_VppRpcAllow(string key, string rpc)
	{
		bool ok = INT_VPP_ServerState.INT_VppRpcAllowKey(key, key, rpc);
		return ok;
	}

	static int INT_VppRpcTracked(string key)
	{
		return INT_VPP_ServerState.INT_VppRpcTracked(key);
	}

	static INT_VPP_ServerSettings GetSettings()
	{
		return INT_VPP_ServerState.GetSettings();
	}

	// Files of a sandbox folder ("$profile:INT_Mods/INT_VppMap_Test") or "" for the real ones; call Reload() after.
	static void SetTestRoot(string root)
	{
		INT_VPP_ServerFiles.SetTestRoot(root);
	}

	// Re-reads Settings.json, Zones.json, Labels.json and SuperAdmins.txt; broadcast = send the new lists to every client.
	static void Reload(bool broadcast)
	{
		INT_VPP_ServerState.LoadAll();
		if (!broadcast)
			return;
		INT_VPP_ServerRpc.SendZones(null, 0);
		INT_VPP_ServerRpc.INT_SendLabels(null, 0);
	}

	// ---- added 2026-11-12 (review fixes #2 #3 #10 #11 #12) ----
	// I/O failure injection for the recoverable replace: INT_VPP_ServerFiles.INT_TEST_FAIL_* (0 = off). Always reset.
	static void INT_SetTestFail(int mode)
	{
		INT_VPP_ServerFiles.INT_SetTestFail(mode);
	}

	static bool INT_SettingsReadOnly()
	{
		return INT_VPP_ServerState.INT_SettingsReadOnly();
	}

	static bool INT_ZonesReadOnly()
	{
		return INT_VPP_ServerState.INT_ZonesReadOnly();
	}

	static string INT_ZonesPath()
	{
		return INT_VPP_ServerFiles.ZonesPath();
	}

	static string INT_SettingsPath()
	{
		return INT_VPP_ServerFiles.SettingsPath();
	}

	// Zones.json on disk parses and holds exactly the live count + NextId.
	static bool INT_VerifyZonesFile()
	{
		string path = INT_VPP_ServerFiles.ZonesPath();
		int cnt = INT_VPP_ServerApi.ZonesCount();
		int nid = INT_VPP_ServerState.INT_NextZoneId();
		bool ok = INT_VPP_ServerFiles.VerifyFile(path, INT_VPP_ServerFiles.KIND_ZONES, cnt, nid);
		return ok;
	}

	// The REAL RPC handlers with a fake sender (key = GetId() stand-in, steam64 = GetPlainId() stand-in) and a
	// context the test wrote; nothing is sent (no identity). Evidence: INT_WriteReads / INT_HelloReads / INT_Result*.
	static void INT_TestHandleHello(string key, string steam64, ParamsReadContext ctx)
	{
		INT_VPP_ServerRpc.INT_HandleHello(key, steam64, null, ctx);
	}

	static void INT_TestHandleZoneSave(string key, string steam64, ParamsReadContext ctx)
	{
		INT_VPP_ServerRpc.INT_HandleZoneSave(key, steam64, null, ctx);
	}

	static void INT_TestHandleZoneDelete(string key, string steam64, ParamsReadContext ctx)
	{
		INT_VPP_ServerRpc.INT_HandleZoneDelete(key, steam64, null, ctx);
	}

	static int INT_WriteReads()
	{
		return INT_VPP_ServerRpc.s_INT_WriteReads;
	}

	static int INT_HelloReads()
	{
		return INT_VPP_ServerRpc.s_INT_HelloReads;
	}

	static int INT_ResultCount()
	{
		return INT_VPP_ServerRpc.s_INT_ResultCount;
	}

	static int INT_ResultKind()
	{
		return INT_VPP_ServerRpc.s_INT_ResultKind;
	}

	static int INT_ResultId()
	{
		return INT_VPP_ServerRpc.s_INT_ResultId;
	}

	static bool INT_ResultOk()
	{
		return INT_VPP_ServerRpc.s_INT_ResultOk;
	}

	static string INT_ResultReason()
	{
		return INT_VPP_ServerRpc.s_INT_ResultReason;
	}

	static int INT_PushCount()
	{
		return INT_VPP_ServerRpc.s_INT_PushCount;
	}

	static bool INT_PushAdmin()
	{
		return INT_VPP_ServerRpc.s_INT_PushAdmin;
	}

	// Builds a result exactly like a real answer (no identity = not sent) and returns the reason as it would travel.
	static string INT_TestResultReason(string reason)
	{
		INT_VPP_ServerRpc.INT_SendResult(null, INT_VPP_Net.RESULT_KIND_SAVE, 0, false, reason);
		return INT_VPP_ServerRpc.s_INT_ResultReason;
	}

	// ---- admin text labels (2026-11-13) ----
	static int INT_LabelsCount()
	{
		array<ref INT_VPP_Label> list = INT_VPP_ServerState.INT_GetLabels();
		if (!list)
			return 0;
		return list.Count();
	}

	// A copy (tests must not edit the live list).
	static INT_VPP_Label INT_FindLabel(int id)
	{
		array<ref INT_VPP_Label> list = INT_VPP_ServerState.INT_GetLabels();
		int idx = INT_VPP_LabelIds.INT_IndexOf(list, id);
		if (idx < 0)
			return null;
		INT_VPP_Label live = list.Get(idx);
		return live.INT_Clone();
	}

	static bool INT_ApplyLabelSave(string senderId, INT_VPP_Label l, out string reason)
	{
		string why;
		bool ok = INT_VPP_ServerState.INT_ApplyLabelSave(senderId, l, why);
		reason = why;
		return ok;
	}

	static bool INT_ApplyLabelDelete(string senderId, int id, out string reason)
	{
		string why;
		bool ok = INT_VPP_ServerState.INT_ApplyLabelDelete(senderId, id, why);
		reason = why;
		return ok;
	}

	static int INT_LastLabelId()
	{
		return INT_VPP_ServerState.INT_LastLabelId();
	}

	static int INT_NextLabelId()
	{
		return INT_VPP_ServerState.INT_NextLabelId();
	}

	static int INT_LabelLimit()
	{
		return INT_VPP_ServerState.INT_LabelLimit();
	}

	static bool INT_LabelsReadOnly()
	{
		return INT_VPP_ServerState.INT_LabelsReadOnly();
	}

	static string INT_LabelsPath()
	{
		return INT_VPP_ServerFiles.INT_LabelsPath();
	}

	// Labels.json on disk parses and holds exactly the live count + INT_NextId.
	static bool INT_VerifyLabelsFile()
	{
		string path = INT_VPP_ServerFiles.INT_LabelsPath();
		int cnt = INT_VPP_ServerApi.INT_LabelsCount();
		int nid = INT_VPP_ServerState.INT_NextLabelId();
		bool ok = INT_VPP_ServerFiles.VerifyFile(path, INT_VPP_ServerFiles.INT_KIND_LABELS, cnt, nid);
		return ok;
	}

	// The REAL label handlers with a fake sender and a context the test wrote; nothing is sent (no identity).
	static void INT_TestHandleLabelSave(string key, string steam64, ParamsReadContext ctx)
	{
		INT_VPP_ServerRpc.INT_HandleLabelSave(key, steam64, null, ctx);
	}

	static void INT_TestHandleLabelDelete(string key, string steam64, ParamsReadContext ctx)
	{
		INT_VPP_ServerRpc.INT_HandleLabelDelete(key, steam64, null, ctx);
	}

	static int INT_LabelBroadcasts()
	{
		return INT_VPP_ServerRpc.s_INT_LabelBroadcasts;
	}
}

modded class MissionServer
{
	protected ref INT_VPP_ServerRpc m_INT_VPP_ServerRpc;

	void MissionServer()
	{
		if (!GetGame().IsDedicatedServer())
			return;
		INT_VPP_ServerRpc rpc = new INT_VPP_ServerRpc();
		m_INT_VPP_ServerRpc = rpc;
		rpc.INT_Register();
	}

	override void OnInit()
	{
		super.OnInit();
		if (!GetGame().IsDedicatedServer())
			return;
		INT_VPP_ServerState.LoadAll();
	}

	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		super.PlayerDisconnected(player, identity, uid);
		INT_VPP_ServerState.Forget(uid);
	}
}
