// INT_VPP «Экспедиция» B — protocol constants, pure validation helpers and the CLIENT cache (NET, BUILD_PLAN §2/§3).
// Reference: design/concepts_expedition/NET_API.md.
// 3_Game on purpose: HUD3D (3_Game) and MAP (5_Mission) both read INT_VPP_Client. The CF RPC calls live in 5_Mission
// (INT_VPP_ClientNet.c implements INT_VPP_ClientTransport; the server side is the server-only PBO INT_VppMap_SE,
// INT_VPP_Server.c), so this module does not depend on CF's compile order. INT_VPP_Net holds the protocol of BOTH
// sides (RPC names, limits, the server's rate / log constants included) and stays here as the shared definition.

class INT_VPP_Net
{
	// Bump when any RPC payload or INT_VPP_ClientSettings / INT_VPP_Zone field changes. Both sides drop other versions.
	// 2 (2026-11-12): INT_ZoneResult added (answer to the sender of a save / delete).
	static const int PROTOCOL = 2;
	// Files written by a NEWER build (version above these) are used read-only and never rewritten (NET_API.md).
	static const int SETTINGS_VERSION = 1;
	static const int ZONES_FILE_VERSION = 1;

	// CF RPC namespace + function names. CF calls the handler METHOD by this name, so the names carry INT_.
	static const string RPC_NS = "INT_VPP";
	// client -> server
	static const string RPC_HELLO = "INT_Hello";             // Param1<int protocol>
	static const string RPC_ZONE_SAVE = "INT_ZoneSave";      // Param2<int protocol, INT_VPP_Zone zone> (id 0 = new)
	static const string RPC_ZONE_DELETE = "INT_ZoneDelete";  // Param2<int protocol, int id>
	// server -> client
	static const string RPC_SETTINGS = "INT_Settings";       // Param2<int protocol, INT_VPP_ClientSettings>
	static const string RPC_ZONES = "INT_Zones";             // Param6<int protocol, int msgId, int part, int parts, int changedId, array<INT_VPP_Zone>>
	static const string RPC_ZONE_RESULT = "INT_ZoneResult";  // Param5<int protocol, int kind, int zoneId, bool ok, string reason> (sender only)

	// INT_ZoneResult kinds and limits.
	static const int RESULT_KIND_SAVE = 1;
	static const int RESULT_KIND_DELETE = 2;
	static const int RESULT_REASON_MAX_BYTES = 200;
	// Reason of every save refused because Zones.json comes from a newer build.
	static const string REASON_ZONES_READ_ONLY = "zones read-only";

	static const int SHAPE_POLYGON = 0;
	static const int SHAPE_CIRCLE = 1;

	// Text limits in BYTES (UTF-8: Cyrillic 2 B, CJK 3 B per char). Any RPC string must stay <= RPC_STRING_MAX_BYTES
	// (1024+ bytes throws in ctx.Read, measured 1.30.164078).
	static const int NAME_MAX_BYTES = 64;
	static const int SUB_MAX_BYTES = 96;
	static const int RPC_STRING_MAX_BYTES = 1000;

	// Hard caps (the server settings may lower them, never raise them).
	static const int HARD_MAX_ZONES = 64;
	static const int HARD_MAX_ZONE_VERTICES = 32;
	static const float CIRCLE_MIN_R = 5.0;
	static const float CIRCLE_MAX_R = 5000.0;
	static const float POLYGON_MIN_AREA = 1.0;
	static const int HARD_MAX_ROUTE_POINTS = 50;
	static const int HARD_MAX_RULER_POINTS = 50;
	static const int HARD_MAX_3D_DIST = 20000;
	static const float MIN_ROUTE_REACH_RADIUS = 2.0;
	static const float MAX_ROUTE_REACH_RADIUS = 500.0;
	// World size when no world is loaded (main menu, pure tests).
	static const float FALLBACK_WORLD_SIZE = 32768.0;

	// Defaults (BUILD_PLAN §0).
	static const float DEF_ROUTE_REACH_RADIUS = 15.0;
	static const int DEF_ROUTE_MAX_POINTS = 20;
	static const int DEF_RULER_MAX_POINTS = 20;
	static const int DEF_DIST3D_SERVER = 3000;
	static const int DEF_DIST3D_MINE = 2000;
	static const int DEF_DIST3D_EVENTS = 3000;
	static const int DEF_DIST3D_ROUTE = 5000;

	// Transport.
	static const int ZONES_PER_PART = 4;          // <= 4 * ~450 B per RPC
	static const int MAX_PARTS = 64;              // receiver bound (64 zones / 4 = 16 needed)
	static const int ADMIN_WRITES_PER_SEC = 4;    // per sender, server side
	static const int CLIENT_WRITE_GAP_MS = 250;   // client side pre-throttle (UI only)
	static const int HELLO_MIN_GAP_MS = 2000;     // per sender, server side
	static const int HELLO_FIRST_DELAY_MS = 3000;
	static const int HELLO_RETRY_MS = 15000;
	static const int HELLO_MAX_TRIES = 3;
	static const int REFRESH_MIN_GAP_MS = 30000;  // client RequestRefresh (a Hello on demand)
	static const int REPLY_MIN_GAP_MS = 2000;     // per sender, server side: settings push / answer to an unread request
	static const int SUPERADMINS_REREAD_MS = 10000;
	static const int LOG_WINDOW_MS = 10000;
	static const int LOG_MAX_PER_WINDOW = 20;

	// ---- admin text labels (owner request 2026-11-13) ----
	// Additive: four new RPC names, PROTOCOL stays 2 (no existing payload or class changed). A side without them drops
	// the other's label RPCs (CF logs one "does not seem to exist" warning per unknown name); old clients also ignore
	// the new result kinds 3 / 4. Refusals of a label request come back as INT_LabelResult (same Param5 as zones).
	static const string INT_RPC_LABEL_SAVE = "INT_LabelSave";      // C->S Param2<int protocol, INT_VPP_Label> (INT_Id 0 = new)
	static const string INT_RPC_LABEL_DELETE = "INT_LabelDelete";  // C->S Param2<int protocol, int id>
	static const string INT_RPC_LABELS = "INT_Labels";             // S->C Param7<int protocol, int msgId, int part, int parts, int changedId, int maxLabels, array<INT_VPP_Label>>
	static const string INT_RPC_LABEL_RESULT = "INT_LabelResult";  // S->C Param5<int protocol, int kind, int labelId, bool ok, string reason> (sender only)
	static const int INT_RESULT_KIND_LABEL_SAVE = 3;
	static const int INT_RESULT_KIND_LABEL_DELETE = 4;
	static const int INT_LABELS_FILE_VERSION = 1;
	static const string INT_REASON_LABELS_READ_ONLY = "labels read-only";
	static const int INT_LABEL_TEXT_MAX_CHARS = 48;
	static const int INT_LABEL_TEXT_MAX_BYTES = 96;
	static const int INT_DEF_MAX_LABELS = 200;      // Settings.json INT_MaxLabels default
	static const int INT_HARD_MAX_LABELS = 500;     // never more, whatever the settings say
	static const int INT_LABELS_PER_PART = 10;      // <= 10 * ~140 B per RPC
	static const int INT_LABEL_MAX_PARTS = 64;      // receiver bound (500 / 10 = 50 needed)
	static const int INT_LABEL_SIZE_COUNT = 6;
	// RF_MAP marker import (owner 2026-11-13, INT_VPP_RFImport.c): additive like the labels, PROTOCOL stays 2. Sent
	// after the Hello answer and ONLY by a server whose Settings.json has INT_ImportRFMapMarkers 1 (so old clients
	// never see the name); the client then imports the player's RF_MAP markers once per server.
	static const string INT_RPC_RF_IMPORT = "INT_RFImport";        // S->C Param2<int protocol, int on>  (on = 1)
	static const int INT_LABEL_COLOR_COUNT = 10;
	static const int INT_LABEL_DEF_SIZE = 24;
	// Server markers edited in game by map admins (owner 2026-11-14): additive like the labels, PROTOCOL stays 2. The
	// requests carry VPP's own MarkerInfo (with INT_SrvId, 0 = new); refusals / successes come back on INT_LabelResult
	// with kinds 5 / 6 (old clients ignore unknown kinds); the new list goes to everybody on VPP's old path
	// (RPC_MissionGameplay / SessionMarkers), so old clients see the change too.
	static const string INT_RPC_SRVMARKER_SAVE = "INT_SrvMarkerSave";      // C->S Param2<int protocol, MarkerInfo>
	static const string INT_RPC_SRVMARKER_DELETE = "INT_SrvMarkerDelete";  // C->S Param2<int protocol, int id>
	static const int INT_RESULT_KIND_SRVMARKER_SAVE = 5;
	static const int INT_RESULT_KIND_SRVMARKER_DELETE = 6;
	static const int INT_DEF_MAX_SRV_MARKERS = 300;     // Settings.json INT_MaxServerMarkers default
	static const int INT_HARD_MAX_SRV_MARKERS = 1000;

	// Allowed text sizes (design px of 1080p, the engine scales them with the screen height, never with the zoom).
	static int INT_LabelSizeAt(int i)
	{
		if (i == 0)
			return 14;
		if (i == 1)
			return 18;
		if (i == 2)
			return 24;
		if (i == 3)
			return 32;
		if (i == 4)
			return 40;
		if (i == 5)
			return 52;
		return 0;
	}

	static int INT_LabelSizeIndex(int size)
	{
		for (int i = 0; i < INT_LABEL_SIZE_COUNT; i++)
		{
			int s = INT_VPP_Net.INT_LabelSizeAt(i);
			if (s == size)
				return i;
		}
		return -1;
	}

	// The palette of markers / zones (INT_VPP_EditorCard.INT_FillPalette, same order) + pure white as the 10th.
	static int INT_LabelColorAt(int i)
	{
		if (i == 0)
			return 0xFFF3EFE8;
		if (i == 1)
			return 0xFFFF7A2F;
		if (i == 2)
			return 0xFFFF4B3E;
		if (i == 3)
			return 0xFFFFC246;
		if (i == 4)
			return 0xFF56E07E;
		if (i == 5)
			return 0xFF3EC7FF;
		if (i == 6)
			return 0xFF6F8CFF;
		if (i == 7)
			return 0xFFB08CFF;
		if (i == 8)
			return 0xFFFF6FAE;
		if (i == 9)
			return 0xFFFFFFFF;
		return 0;
	}

	// Index of the palette colour with the same RGB (alpha ignored), -1 = not a palette colour.
	static int INT_LabelColorIndex(int argb)
	{
		int rgb = argb & 0x00FFFFFF;
		for (int i = 0; i < INT_LABEL_COLOR_COUNT; i++)
		{
			int pc = INT_VPP_Net.INT_LabelColorAt(i);
			int prgb = pc & 0x00FFFFFF;
			if (prgb == rgb)
				return i;
		}
		return -1;
	}
}

// Pure helpers: no state, usable on server, client and in tests. The server-only ones (HasJsonKey, INT_JsonInt,
// IsSteam64, Zone) are added by the server PBO INT_VppMap_SE (modded class, 2026-11-12 CL/SE split).
class INT_VPP_Validate
{
	static int ClampInt(int v, int lo, int hi)
	{
		if (v < lo)
			return lo;
		if (v > hi)
			return hi;
		return v;
	}

	// NaN -> def (Enforce: NaN > x is TRUE, so test v != v first).
	static float ClampFloat(float v, float lo, float hi, float def)
	{
		if (v != v)
			return def;
		if (v < lo)
			return lo;
		if (v > hi)
			return hi;
		return v;
	}

	static bool InRange(float v, float lo, float hi)
	{
		if (v != v)
			return false;
		if (v < lo)
			return false;
		if (v > hi)
			return false;
		return true;
	}

	// Cuts to at most maxBytes BYTES without splitting a multibyte UTF-8 character.
	static string ClipBytes(string s, int maxBytes)
	{
		if (maxBytes <= 0)
			return "";
		int len = s.Length();
		if (len <= maxBytes)
			return s;
		int chars = s.LengthUtf8();
		int k = chars * maxBytes / len;
		if (k > maxBytes)
			k = maxBytes;
		if (k < 0)
			k = 0;
		string cut = s.SubstringUtf8(0, k);
		while (k > 0 && cut.Length() > maxBytes)
		{
			k--;
			cut = s.SubstringUtf8(0, k);
		}
		while (k < chars)
		{
			int kn = k + 1;
			string longer = s.SubstringUtf8(0, kn);
			if (longer.Length() > maxBytes)
				break;
			k = kn;
			cut = longer;
		}
		return cut;
	}

	// Player text for names / sub-lines: control characters -> space, clipped to maxBytes (UTF-8 safe), trimmed.
	static string CleanText(string s, int maxBytes)
	{
		if (maxBytes <= 0)
			return "";
		string src = s + "";
		int chars = src.LengthUtf8();
		if (chars > maxBytes)
		{
			src = src.SubstringUtf8(0, maxBytes);
			chars = src.LengthUtf8();
		}
		string res = "";
		int bytes = 0;
		for (int i = 0; i < chars; i++)
		{
			string ch = src.SubstringUtf8(i, 1);
			int b = ch.Length();
			if (b <= 0)
				continue;
			if (b == 1)
			{
				int code = ch.ToAscii();
				if (code < 32 || code == 127)
					ch = " ";
			}
			if (bytes + b > maxBytes)
				break;
			res = res + ch;
			bytes = bytes + b;
		}
		res.TrimInPlace();
		return res;
	}

	// Text for a log line: no '%' (client Print trap), no control characters, short.
	static string LogText(string s)
	{
		string t = INT_VPP_Validate.CleanText(s, 96);
		t.Replace("%", " pct");
		return t;
	}

	static int ForceAlpha(int color)
	{
		int rr = (color >> 16) & 0xFF;
		int gg = (color >> 8) & 0xFF;
		int bb = color & 0xFF;
		int res = ARGB(255, rr, gg, bb);
		return res;
	}

	static float WorldSize()
	{
		float w = INT_VPP_Net.FALLBACK_WORLD_SIZE;
		if (!GetGame())
			return w;
		World world = GetGame().GetWorld();
		if (!world)
			return w;
		int ws = world.GetWorldSize();
		if (ws > 0)
			w = ws;
		return w;
	}

	// Validates AND normalises z in place: name/sub cleaned + clipped (64 / 96 bytes), alpha forced to 255, circle
	// arrays emptied, polygon cx/cz = vertex average and r = 0. false + reason = reject the whole zone.
	static bool ZoneLimits(INT_VPP_Zone z, int maxVertices, out string reason)
	{
		reason = "";
		if (!z)
		{
			reason = "null zone";
			return false;
		}
		if (z.id < 0)
		{
			reason = "bad id";
			return false;
		}
		if (z.shape != INT_VPP_Net.SHAPE_POLYGON && z.shape != INT_VPP_Net.SHAPE_CIRCLE)
		{
			reason = "bad shape";
			return false;
		}
		string nm = INT_VPP_Validate.CleanText(z.name, INT_VPP_Net.NAME_MAX_BYTES);
		if (nm == "")
		{
			reason = "empty name";
			return false;
		}
		string sb = INT_VPP_Validate.CleanText(z.sub, INT_VPP_Net.SUB_MAX_BYTES);
		z.name = nm;
		z.sub = sb;
		int col = INT_VPP_Validate.ForceAlpha(z.color);
		z.color = col;
		float w = INT_VPP_Validate.WorldSize();
		int maxV = INT_VPP_Validate.ClampInt(maxVertices, 3, INT_VPP_Net.HARD_MAX_ZONE_VERTICES);

		if (z.shape == INT_VPP_Net.SHAPE_CIRCLE)
		{
			if (!INT_VPP_Validate.InRange(z.cx, 0, w) || !INT_VPP_Validate.InRange(z.cz, 0, w))
			{
				reason = "centre outside world";
				return false;
			}
			if (!INT_VPP_Validate.InRange(z.r, INT_VPP_Net.CIRCLE_MIN_R, INT_VPP_Net.CIRCLE_MAX_R))
			{
				reason = "radius out of range";
				return false;
			}
			array<float> noXs = new array<float>();
			array<float> noZs = new array<float>();
			z.xs = noXs;
			z.zs = noZs;
			return true;
		}

		if (!z.xs || !z.zs)
		{
			reason = "no vertices";
			return false;
		}
		int n = z.xs.Count();
		int nz = z.zs.Count();
		if (n != nz)
		{
			reason = "vertex arrays differ";
			return false;
		}
		if (n < 3)
		{
			reason = "too few vertices";
			return false;
		}
		if (n > maxV)
		{
			reason = "too many vertices";
			return false;
		}
		float sumX = 0;
		float sumZ = 0;
		float area2 = 0;
		for (int i = 0; i < n; i++)
		{
			float px = z.xs.Get(i);
			float pz = z.zs.Get(i);
			if (!INT_VPP_Validate.InRange(px, 0, w) || !INT_VPP_Validate.InRange(pz, 0, w))
			{
				reason = "vertex outside world";
				return false;
			}
			int j = i + 1;
			if (j >= n)
				j = 0;
			float qx = z.xs.Get(j);
			float qz = z.zs.Get(j);
			area2 = area2 + px * qz - qx * pz;
			sumX = sumX + px;
			sumZ = sumZ + pz;
		}
		float area = Math.AbsFloat(area2) * 0.5;
		if (area < INT_VPP_Net.POLYGON_MIN_AREA)
		{
			reason = "degenerate polygon";
			return false;
		}
		float nf = n;
		float mx = sumX / nf;
		float mz = sumZ / nf;
		z.cx = mx;
		z.cz = mz;
		z.r = 0;
		return true;
	}

	// Label text: control characters -> space (one line), at most 48 characters AND 96 bytes (never a split UTF-8
	// character: 48 Cyrillic letters = 96 B, CJK 32 letters), trimmed.
	static string INT_LabelText(string s)
	{
		string src = s + "";
		int chars = src.LengthUtf8();
		if (chars > INT_VPP_Net.INT_LABEL_TEXT_MAX_CHARS)
			src = src.SubstringUtf8(0, INT_VPP_Net.INT_LABEL_TEXT_MAX_CHARS);
		string res = INT_VPP_Validate.CleanText(src, INT_VPP_Net.INT_LABEL_TEXT_MAX_BYTES);
		return res;
	}

	// Server marker (2026-11-14): validates AND normalises in place (client pre-check and server): name cleaned (64
	// bytes) and not empty, colour components 0..255 (rounded), position inside the world (y 0). The icon is checked
	// by the server (our set or VPP's legacy set, or unchanged).
	static bool INT_SrvMarkerLimits(MarkerInfo m, out string reason)
	{
		reason = "";
		if (!m)
		{
			reason = "no marker";
			return false;
		}
		if (m.INT_GetSrvId() < 0)
		{
			reason = "bad id";
			return false;
		}
		string raw = m.GetName();
		raw.TrimInPlace();
		string nm = INT_VPP_Validate.CleanText(raw, INT_VPP_Net.NAME_MAX_BYTES);
		nm.TrimInPlace();
		if (nm == "")
		{
			reason = "empty name";
			return false;
		}
		m.SetName(nm);
		vector c = m.GetColor();
		for (int i = 0; i < 3; i++)
		{
			float cv = c[i];
			if (cv != cv || cv < 0 || cv > 255)
			{
				reason = "bad color";
				return false;
			}
			c[i] = Math.Round(cv);
		}
		m.SetColor(c);
		vector p = m.GetPosition();
		float ws = INT_VPP_Validate.WorldSize();
		float px = p[0];
		float pz = p[2];
		if (px != px || pz != pz || px < 0 || pz < 0 || px > ws || pz > ws)
		{
			reason = "position outside world";
			return false;
		}
		m.SetPosition(Vector(px, 0, pz));
		string icon = m.GetIconPath();
		int il = icon.Length();
		if (il == 0 || il > 256)
		{
			reason = "bad icon";
			return false;
		}
		return true;
	}

	// Validates AND normalises a label in place (client pre-check and server, same rules): text cleaned + clipped,
	// alpha forced to 255. false + reason = reject. Reasons: "null label", "bad id", "empty text", "bad color" (not a
	// palette colour), "bad size" (not one of the six sizes), "position outside world" (0..world size, NaN rejected).
	static bool INT_LabelLimits(INT_VPP_Label l, out string reason)
	{
		reason = "";
		if (!l)
		{
			reason = "null label";
			return false;
		}
		if (l.INT_Id < 0)
		{
			reason = "bad id";
			return false;
		}
		string t = INT_VPP_Validate.INT_LabelText(l.INT_Text);
		if (t == "")
		{
			reason = "empty text";
			return false;
		}
		l.INT_Text = t;
		int ci = INT_VPP_Net.INT_LabelColorIndex(l.INT_Color);
		if (ci < 0)
		{
			reason = "bad color";
			return false;
		}
		int col = INT_VPP_Validate.ForceAlpha(l.INT_Color);
		l.INT_Color = col;
		int si = INT_VPP_Net.INT_LabelSizeIndex(l.INT_Size);
		if (si < 0)
		{
			reason = "bad size";
			return false;
		}
		float w = INT_VPP_Validate.WorldSize();
		if (!INT_VPP_Validate.InRange(l.INT_X, 0, w) || !INT_VPP_Validate.InRange(l.INT_Z, 0, w))
		{
			reason = "position outside world";
			return false;
		}
		return true;
	}
}

// Id lookup (client + server). MaxId / Next (id allocation) are server only: INT_VppMap_SE adds them.
class INT_VPP_ZoneIds
{
	static int IndexOf(array<ref INT_VPP_Zone> zones, int id)
	{
		if (!zones)
			return -1;
		int n = zones.Count();
		for (int i = 0; i < n; i++)
		{
			INT_VPP_Zone z = zones.Get(i);
			if (z && z.id == id)
				return i;
		}
		return -1;
	}
}

// Label id lookup (client + server). INT_MaxId / INT_Next (id allocation) are server only: INT_VppMap_SE adds them.
class INT_VPP_LabelIds
{
	static int INT_IndexOf(array<ref INT_VPP_Label> labels, int id)
	{
		if (!labels)
			return -1;
		int n = labels.Count();
		for (int i = 0; i < n; i++)
		{
			INT_VPP_Label l = labels.Get(i);
			if (l && l.INT_Id == id)
				return i;
		}
		return -1;
	}
}

// Send seam of the client cache: 5_Mission INT_VPP_ClientRpc sends through CF; a TestKit may plug a fake.
class INT_VPP_ClientTransport : Managed
{
	bool SendHello()
	{
		return false;
	}

	bool SendZoneSave(INT_VPP_Zone z)
	{
		return false;
	}

	bool SendZoneDelete(int id)
	{
		return false;
	}

	// admin text labels (2026-11-13)
	bool INT_SendLabelSave(INT_VPP_Label l)
	{
		return false;
	}

	bool INT_SendLabelDelete(int id)
	{
		return false;
	}

	// server markers (2026-11-14)
	bool INT_SendSrvMarkerSave(MarkerInfo m)
	{
		return false;
	}

	bool INT_SendSrvMarkerDelete(int id)
	{
		return false;
	}
}

// CLIENT singleton: what the server told this client. Lives from the first Get() to INT_VPP_Client.Shutdown() at
// MissionGameplay.OnMissionFinish (data, buffers and invoker subscriptions are dropped there; nothing reaches the main
// menu). Subscribers: OnSettings / OnZones / OnResult are invoked with no arguments; remove your callback when you die.
class INT_VPP_Client : Managed
{
	protected static ref INT_VPP_Client s_INT_Instance;

	ref ScriptInvoker OnSettings;
	ref ScriptInvoker OnZones;
	// INT_ZoneResult arrived (no args): read LastResultKind / LastResultId / LastResultOk / LastError.
	ref ScriptInvoker OnResult;

	protected ref INT_VPP_ClientSettings m_Settings;
	protected ref array<ref INT_VPP_Zone> m_Zones;
	protected bool m_HasSettings;
	protected bool m_HasZones;
	protected int m_ZonesRevision;
	protected int m_LastChangedId;
	protected string m_LastError;
	protected bool m_WroteOnce;
	protected int m_LastWriteAt;
	// last INT_ZoneResult
	protected int m_ResultKind;
	protected int m_ResultId;
	protected bool m_ResultOk;
	protected int m_ResultsRevision;
	// RequestRefresh throttle
	protected bool m_Refreshed;
	protected int m_RefreshAt;
	// weak: owned by the mission (INT_VPP_ClientNet.c)
	protected INT_VPP_ClientTransport m_Transport;
	// zones receive buffer (one message = parts 0..parts-1 with the same msgId)
	protected int m_RxMsgId;
	protected int m_RxParts;
	protected int m_RxGot;
	protected ref map<int, ref array<ref INT_VPP_Zone>> m_RxBuf;

	// ---- admin text labels (2026-11-13) ----
	// INT_Labels arrived complete (no args): INT_GetLabels / INT_LabelsRevision / INT_LabelsChangedId
	ref ScriptInvoker m_INT_OnLabels;
	// INT_LabelResult arrived (no args): INT_LabelResultKind / Id / Ok, INT_LabelError
	ref ScriptInvoker m_INT_OnLabelResult;
	protected ref array<ref INT_VPP_Label> m_INT_Labels;
	protected bool m_INT_HasLabels;
	protected int m_INT_LabelsRevision;
	protected int m_INT_LabelsChangedId;
	protected int m_INT_LabelLimit;
	protected string m_INT_LabelError;
	// server markers (2026-11-14): last local refusal / server answer
	protected string m_INT_SrvMarkerError;
	protected int m_INT_SResultKind;
	protected int m_INT_SResultId;
	protected bool m_INT_SResultOk;
	protected int m_INT_SResultsRevision;
	protected int m_INT_LResultKind;
	protected int m_INT_LResultId;
	protected bool m_INT_LResultOk;
	protected int m_INT_LResultsRevision;
	protected int m_INT_LRxMsgId;
	protected int m_INT_LRxParts;
	protected int m_INT_LRxGot;
	protected ref map<int, ref array<ref INT_VPP_Label>> m_INT_LRxBuf;

	void INT_VPP_Client()
	{
		OnSettings = new ScriptInvoker();
		OnZones = new ScriptInvoker();
		OnResult = new ScriptInvoker();
		m_INT_OnLabels = new ScriptInvoker();
		m_INT_OnLabelResult = new ScriptInvoker();
		m_Settings = new INT_VPP_ClientSettings();
		m_Zones = new array<ref INT_VPP_Zone>();
		m_INT_Labels = new array<ref INT_VPP_Label>();
		m_INT_LabelLimit = INT_VPP_Net.INT_DEF_MAX_LABELS;
	}

	static INT_VPP_Client Get()
	{
		if (!s_INT_Instance)
			s_INT_Instance = new INT_VPP_Client();
		return s_INT_Instance;
	}

	// No creation: null outside a mission (or before anyone asked).
	static INT_VPP_Client Peek()
	{
		return s_INT_Instance;
	}

	// Mission finish: drop everything (data, receive buffer, subscribers, transport).
	static void Shutdown()
	{
		INT_VPP_Client inst = s_INT_Instance;
		if (inst)
		{
			inst.ClearData();
			if (inst.OnSettings)
				inst.OnSettings.Clear();
			if (inst.OnZones)
				inst.OnZones.Clear();
			if (inst.OnResult)
				inst.OnResult.Clear();
			if (inst.m_INT_OnLabels)
				inst.m_INT_OnLabels.Clear();
			if (inst.m_INT_OnLabelResult)
				inst.m_INT_OnLabelResult.Clear();
			inst.SetTransport(null);
		}
		s_INT_Instance = null;
	}

	void SetTransport(INT_VPP_ClientTransport t)
	{
		m_Transport = t;
	}

	void ClearData()
	{
		m_Settings = new INT_VPP_ClientSettings();
		m_Zones = new array<ref INT_VPP_Zone>();
		m_HasSettings = false;
		m_HasZones = false;
		m_ZonesRevision = 0;
		m_LastChangedId = 0;
		m_LastError = "";
		m_WroteOnce = false;
		m_LastWriteAt = 0;
		m_ResultKind = 0;
		m_ResultId = 0;
		m_ResultOk = false;
		m_ResultsRevision = 0;
		m_Refreshed = false;
		m_RefreshAt = 0;
		m_RxMsgId = 0;
		m_RxParts = 0;
		m_RxGot = 0;
		m_RxBuf = null;
		m_INT_Labels = new array<ref INT_VPP_Label>();
		m_INT_HasLabels = false;
		m_INT_LabelsRevision = 0;
		m_INT_LabelsChangedId = 0;
		m_INT_LabelLimit = INT_VPP_Net.INT_DEF_MAX_LABELS;
		m_INT_LabelError = "";
		m_INT_LResultKind = 0;
		m_INT_LResultId = 0;
		m_INT_LResultOk = false;
		m_INT_LResultsRevision = 0;
		m_INT_LRxMsgId = 0;
		m_INT_LRxParts = 0;
		m_INT_LRxGot = 0;
		m_INT_LRxBuf = null;
	}

	// ---- read side ----
	bool HasSettings()
	{
		return m_HasSettings;
	}

	bool HasZones()
	{
		return m_HasZones;
	}

	// Never null: defaults until the server answered.
	INT_VPP_ClientSettings GetSettings()
	{
		return m_Settings;
	}

	// Never null. Read-only for callers (it is replaced as a whole on every update).
	array<ref INT_VPP_Zone> GetZones()
	{
		return m_Zones;
	}

	int ZonesCount()
	{
		return m_Zones.Count();
	}

	INT_VPP_Zone FindZone(int id)
	{
		int idx = INT_VPP_ZoneIds.IndexOf(m_Zones, id);
		if (idx < 0)
			return null;
		return m_Zones.Get(idx);
	}

	// +1 on every complete zones update (dirty check for views).
	int ZonesRevision()
	{
		return m_ZonesRevision;
	}

	// Id saved/deleted by the change that caused the last update (0 = full list on hello).
	int LastChangedId()
	{
		return m_LastChangedId;
	}

	bool IsAdmin()
	{
		if (!m_HasSettings)
			return false;
		if (!m_Settings)
			return false;
		if (m_Settings.IsAdmin == 1)
			return true;
		return false;
	}

	// Why the last Request* returned false (local check), or - after OnResult with LastResultOk() false - the
	// server's reason for refusing the last save / delete. "" after a successful result.
	string LastError()
	{
		return m_LastError;
	}

	// Last INT_ZoneResult: INT_VPP_Net.RESULT_KIND_SAVE / RESULT_KIND_DELETE (0 = none yet).
	int LastResultKind()
	{
		return m_ResultKind;
	}

	// Saved / deleted zone id on success (the new id of a created zone); on a refusal the id that was asked for
	// (0 = a new zone, or the server refused before reading the request: "not admin", "too fast").
	int LastResultId()
	{
		return m_ResultId;
	}

	bool LastResultOk()
	{
		return m_ResultOk;
	}

	// +1 per INT_ZoneResult (dirty check).
	int ResultsRevision()
	{
		return m_ResultsRevision;
	}

	// ---- send side (server re-checks everything) ----
	bool SendHello()
	{
		if (!m_Transport)
			return false;
		return m_Transport.SendHello();
	}

	// Settings + zones again (e.g. when the map opens): a player made map admin - or removed - while connected
	// gets the new IsAdmin without a reconnect (answer = OnSettings + OnZones). At most once per
	// INT_VPP_Net.REFRESH_MIN_GAP_MS; false = not sent (too soon / no transport).
	bool RequestRefresh()
	{
		if (!m_Transport)
			return false;
		int now = GetGame().GetTime();
		if (m_Refreshed)
		{
			int gap = now - m_RefreshAt;
			if (gap >= 0 && gap < INT_VPP_Net.REFRESH_MIN_GAP_MS)
				return false;
		}
		bool sent = m_Transport.SendHello();
		if (!sent)
			return false;
		m_Refreshed = true;
		m_RefreshAt = now;
		return true;
	}

	// TEST (stand TestKit): forget the last refresh, so the next RequestRefresh is not throttled
	void INT_TestResetRefresh()
	{
		m_Refreshed = false;
		m_RefreshAt = 0;
	}

	// Pre-checks with the same rules as the server; false + LastError() when not sent. Does not modify z.
	bool RequestZoneSave(INT_VPP_Zone z)
	{
		m_LastError = "";
		if (!z)
		{
			m_LastError = "null zone";
			return false;
		}
		if (!this.IsAdmin())
		{
			m_LastError = "not admin";
			return false;
		}
		if (!m_Transport)
		{
			m_LastError = "no transport";
			return false;
		}
		if (!this.WriteAllowed())
		{
			m_LastError = "too fast";
			return false;
		}
		int vc = 0;
		if (z.xs)
			vc = z.xs.Count();
		if (vc > INT_VPP_Net.HARD_MAX_ZONE_VERTICES)
		{
			m_LastError = "too many vertices";
			return false;
		}
		INT_VPP_Zone c = z.Clone();
		string why;
		bool valid = INT_VPP_Validate.ZoneLimits(c, m_Settings.MaxZoneVertices, why);
		if (!valid)
		{
			m_LastError = why;
			return false;
		}
		if (c.id == 0 && m_Zones.Count() >= m_Settings.MaxZones)
		{
			m_LastError = "zone limit";
			return false;
		}
		bool sent = m_Transport.SendZoneSave(c);
		if (sent)
			this.MarkWrite();
		return sent;
	}

	bool RequestZoneDelete(int id)
	{
		m_LastError = "";
		if (id <= 0)
		{
			m_LastError = "bad id";
			return false;
		}
		if (!this.IsAdmin())
		{
			m_LastError = "not admin";
			return false;
		}
		if (!m_Transport)
		{
			m_LastError = "no transport";
			return false;
		}
		if (!this.WriteAllowed())
		{
			m_LastError = "too fast";
			return false;
		}
		bool sent = m_Transport.SendZoneDelete(id);
		if (sent)
			this.MarkWrite();
		return sent;
	}

	protected bool WriteAllowed()
	{
		if (!m_WroteOnce)
			return true;
		int now = GetGame().GetTime();
		int gap = now - m_LastWriteAt;
		if (gap < INT_VPP_Net.CLIENT_WRITE_GAP_MS && gap >= 0)
			return false;
		return true;
	}

	protected void MarkWrite()
	{
		int now = GetGame().GetTime();
		m_LastWriteAt = now;
		m_WroteOnce = true;
	}

	// ---- receive side (called by the 5_Mission receivers; a TestKit may call them directly) ----
	void ApplySettings(INT_VPP_ClientSettings s)
	{
		if (!s)
			return;
		s.Sanitize();
		m_Settings = s;
		m_HasSettings = true;
		OnSettings.Invoke();
	}

	// INT_ZoneResult: the server's answer to THIS client's save / delete (only the sender gets it). A refusal also
	// brings fresh settings (OnSettings: IsAdmin may have changed). Unknown kinds are ignored.
	void ApplyResult(int kind, int zoneId, bool ok, string reason)
	{
		if (kind != INT_VPP_Net.RESULT_KIND_SAVE && kind != INT_VPP_Net.RESULT_KIND_DELETE)
			return;
		string r = INT_VPP_Validate.CleanText(reason, INT_VPP_Net.RESULT_REASON_MAX_BYTES);
		if (!ok && r == "")
			r = "refused";
		m_ResultKind = kind;
		m_ResultId = zoneId;
		m_ResultOk = ok;
		if (ok)
			m_LastError = "";
		else
			m_LastError = r;
		m_ResultsRevision++;
		OnResult.Invoke();
	}

	// One part of a zones message. Returns true when it was accepted (complete or not).
	bool ApplyZonesPart(int msgId, int part, int parts, int changedId, array<ref INT_VPP_Zone> list)
	{
		if (parts < 1 || parts > INT_VPP_Net.MAX_PARTS)
			return false;
		if (part < 0 || part >= parts)
			return false;
		if (!m_RxBuf || msgId != m_RxMsgId)
		{
			m_RxMsgId = msgId;
			m_RxParts = parts;
			m_RxGot = 0;
			m_RxBuf = new map<int, ref array<ref INT_VPP_Zone>>();
		}
		if (parts != m_RxParts)
			return false;
		if (m_RxBuf.Contains(part))
			return false;
		array<ref INT_VPP_Zone> keep = new array<ref INT_VPP_Zone>();
		if (list)
		{
			foreach (INT_VPP_Zone inZone : list)
			{
				if (!inZone)
					continue;
				if (!inZone.xs)
				{
					array<float> ex = new array<float>();
					inZone.xs = ex;
				}
				if (!inZone.zs)
				{
					array<float> ez = new array<float>();
					inZone.zs = ez;
				}
				keep.Insert(inZone);
			}
		}
		m_RxBuf.Set(part, keep);
		m_RxGot++;
		if (m_RxGot < m_RxParts)
			return true;

		array<ref INT_VPP_Zone> all = new array<ref INT_VPP_Zone>();
		for (int p = 0; p < m_RxParts; p++)
		{
			array<ref INT_VPP_Zone> chunk = m_RxBuf.Get(p);
			if (!chunk)
				continue;
			foreach (INT_VPP_Zone partZone : chunk)
			{
				if (all.Count() >= INT_VPP_Net.HARD_MAX_ZONES)
					break;
				all.Insert(partZone);
			}
		}
		m_Zones = all;
		m_HasZones = true;
		m_ZonesRevision++;
		m_LastChangedId = changedId;
		m_RxBuf = null;
		m_RxGot = 0;
		m_RxParts = 0;
		OnZones.Invoke();
		return true;
	}

	// ======================================================= admin text labels (2026-11-13)
	bool INT_HasLabels()
	{
		return m_INT_HasLabels;
	}

	// Never null. Read-only for callers (replaced as a whole on every update).
	array<ref INT_VPP_Label> INT_GetLabels()
	{
		return m_INT_Labels;
	}

	int INT_LabelsCount()
	{
		return m_INT_Labels.Count();
	}

	INT_VPP_Label INT_FindLabel(int id)
	{
		int idx = INT_VPP_LabelIds.INT_IndexOf(m_INT_Labels, id);
		if (idx < 0)
			return null;
		return m_INT_Labels.Get(idx);
	}

	// +1 on every complete labels update (dirty check for views)
	int INT_LabelsRevision()
	{
		return m_INT_LabelsRevision;
	}

	// id saved / deleted by the change behind the last update (0 = full list on hello)
	int INT_LabelsChangedId()
	{
		return m_INT_LabelsChangedId;
	}

	// the server's limit for NEW labels (Settings.json INT_MaxLabels, sent with every list)
	int INT_LabelLimit()
	{
		return m_INT_LabelLimit;
	}

	// why the last INT_RequestLabel* returned false (local check), or the server's reason after a refused
	// INT_LabelResult; "" after a successful result
	string INT_LabelError()
	{
		return m_INT_LabelError;
	}

	// INT_VPP_Net.INT_RESULT_KIND_LABEL_SAVE (3) / _DELETE (4), 0 = none yet
	int INT_LabelResultKind()
	{
		return m_INT_LResultKind;
	}

	int INT_LabelResultId()
	{
		return m_INT_LResultId;
	}

	bool INT_LabelResultOk()
	{
		return m_INT_LResultOk;
	}

	int INT_LabelResultsRevision()
	{
		return m_INT_LResultsRevision;
	}

	// Pre-checks with the server's rules; false + INT_LabelError() when not sent. Does not modify l.
	bool INT_RequestLabelSave(INT_VPP_Label l)
	{
		m_INT_LabelError = "";
		if (!l)
		{
			m_INT_LabelError = "null label";
			return false;
		}
		if (!this.IsAdmin())
		{
			m_INT_LabelError = "not admin";
			return false;
		}
		if (!m_Transport)
		{
			m_INT_LabelError = "no transport";
			return false;
		}
		if (!this.WriteAllowed())
		{
			m_INT_LabelError = "too fast";
			return false;
		}
		INT_VPP_Label c = l.INT_Clone();
		string why;
		bool valid = INT_VPP_Validate.INT_LabelLimits(c, why);
		if (!valid)
		{
			m_INT_LabelError = why;
			return false;
		}
		if (c.INT_Id == 0 && m_INT_Labels.Count() >= m_INT_LabelLimit)
		{
			m_INT_LabelError = "label limit";
			return false;
		}
		bool sent = m_Transport.INT_SendLabelSave(c);
		if (sent)
			this.MarkWrite();
		return sent;
	}

	bool INT_RequestLabelDelete(int id)
	{
		m_INT_LabelError = "";
		if (id <= 0)
		{
			m_INT_LabelError = "bad id";
			return false;
		}
		if (!this.IsAdmin())
		{
			m_INT_LabelError = "not admin";
			return false;
		}
		if (!m_Transport)
		{
			m_INT_LabelError = "no transport";
			return false;
		}
		if (!this.WriteAllowed())
		{
			m_INT_LabelError = "too fast";
			return false;
		}
		bool sent = m_Transport.INT_SendLabelDelete(id);
		if (sent)
			this.MarkWrite();
		return sent;
	}

	// ---- server markers (2026-11-14) ----
	string INT_SrvMarkerError()
	{
		return m_INT_SrvMarkerError;
	}

	int INT_SrvMarkerResultKind()
	{
		return m_INT_SResultKind;
	}

	int INT_SrvMarkerResultId()
	{
		return m_INT_SResultId;
	}

	bool INT_SrvMarkerResultOk()
	{
		return m_INT_SResultOk;
	}

	int INT_SrvMarkerResultsRevision()
	{
		return m_INT_SResultsRevision;
	}

	// pre-checks with the server's rules; false + INT_SrvMarkerError() when not sent. m is normalised in place.
	bool INT_RequestSrvMarkerSave(MarkerInfo m)
	{
		m_INT_SrvMarkerError = "";
		if (!m)
		{
			m_INT_SrvMarkerError = "no marker";
			return false;
		}
		if (!this.IsAdmin())
		{
			m_INT_SrvMarkerError = "not admin";
			return false;
		}
		if (!m_Transport)
		{
			m_INT_SrvMarkerError = "no transport";
			return false;
		}
		if (!this.WriteAllowed())
		{
			m_INT_SrvMarkerError = "too fast";
			return false;
		}
		string why;
		if (!INT_VPP_Validate.INT_SrvMarkerLimits(m, why))
		{
			m_INT_SrvMarkerError = why;
			return false;
		}
		bool sent = m_Transport.INT_SendSrvMarkerSave(m);
		if (sent)
			this.MarkWrite();
		return sent;
	}

	bool INT_RequestSrvMarkerDelete(int id)
	{
		m_INT_SrvMarkerError = "";
		if (id <= 0)
		{
			m_INT_SrvMarkerError = "bad id";
			return false;
		}
		if (!this.IsAdmin())
		{
			m_INT_SrvMarkerError = "not admin";
			return false;
		}
		if (!m_Transport)
		{
			m_INT_SrvMarkerError = "no transport";
			return false;
		}
		if (!this.WriteAllowed())
		{
			m_INT_SrvMarkerError = "too fast";
			return false;
		}
		bool sent = m_Transport.INT_SendSrvMarkerDelete(id);
		if (sent)
			this.MarkWrite();
		return sent;
	}

	// INT_LabelResult with kind 5 / 6: the answer to THIS client's server-marker save / delete; other kinds ignored
	void INT_ApplySrvMarkerResult(int kind, int id, bool ok, string reason)
	{
		if (kind != INT_VPP_Net.INT_RESULT_KIND_SRVMARKER_SAVE && kind != INT_VPP_Net.INT_RESULT_KIND_SRVMARKER_DELETE)
			return;
		string r = INT_VPP_Validate.CleanText(reason, INT_VPP_Net.RESULT_REASON_MAX_BYTES);
		if (!ok && r == "")
			r = "refused";
		m_INT_SResultKind = kind;
		m_INT_SResultId = id;
		m_INT_SResultOk = ok;
		if (ok)
			m_INT_SrvMarkerError = "";
		else
			m_INT_SrvMarkerError = r;
		m_INT_SResultsRevision++;
	}

	// INT_LabelResult: the server's answer to THIS client's label save / delete. Unknown kinds are ignored.
	void INT_ApplyLabelResult(int kind, int labelId, bool ok, string reason)
	{
		if (kind != INT_VPP_Net.INT_RESULT_KIND_LABEL_SAVE && kind != INT_VPP_Net.INT_RESULT_KIND_LABEL_DELETE)
			return;
		string r = INT_VPP_Validate.CleanText(reason, INT_VPP_Net.RESULT_REASON_MAX_BYTES);
		if (!ok && r == "")
			r = "refused";
		m_INT_LResultKind = kind;
		m_INT_LResultId = labelId;
		m_INT_LResultOk = ok;
		if (ok)
			m_INT_LabelError = "";
		else
			m_INT_LabelError = r;
		m_INT_LResultsRevision++;
		m_INT_OnLabelResult.Invoke();
	}

	// One part of a labels message. true = accepted (complete or not). maxLabels = the server's limit for new labels.
	bool INT_ApplyLabelsPart(int msgId, int part, int parts, int changedId, int maxLabels, array<ref INT_VPP_Label> list)
	{
		if (parts < 1 || parts > INT_VPP_Net.INT_LABEL_MAX_PARTS)
			return false;
		if (part < 0 || part >= parts)
			return false;
		if (!m_INT_LRxBuf || msgId != m_INT_LRxMsgId)
		{
			m_INT_LRxMsgId = msgId;
			m_INT_LRxParts = parts;
			m_INT_LRxGot = 0;
			m_INT_LRxBuf = new map<int, ref array<ref INT_VPP_Label>>();
		}
		if (parts != m_INT_LRxParts)
			return false;
		if (m_INT_LRxBuf.Contains(part))
			return false;
		array<ref INT_VPP_Label> keep = new array<ref INT_VPP_Label>();
		if (list)
		{
			foreach (INT_VPP_Label inLabel : list)
			{
				if (inLabel)
					keep.Insert(inLabel);
			}
		}
		m_INT_LRxBuf.Set(part, keep);
		m_INT_LRxGot++;
		if (m_INT_LRxGot < m_INT_LRxParts)
			return true;

		array<ref INT_VPP_Label> all = new array<ref INT_VPP_Label>();
		for (int p = 0; p < m_INT_LRxParts; p++)
		{
			array<ref INT_VPP_Label> chunk = m_INT_LRxBuf.Get(p);
			if (!chunk)
				continue;
			foreach (INT_VPP_Label partLabel : chunk)
			{
				if (all.Count() >= INT_VPP_Net.INT_HARD_MAX_LABELS)
					break;
				all.Insert(partLabel);
			}
		}
		int lim = INT_VPP_Validate.ClampInt(maxLabels, 0, INT_VPP_Net.INT_HARD_MAX_LABELS);
		m_INT_LabelLimit = lim;
		m_INT_Labels = all;
		m_INT_HasLabels = true;
		m_INT_LabelsRevision++;
		m_INT_LabelsChangedId = changedId;
		m_INT_LRxBuf = null;
		m_INT_LRxGot = 0;
		m_INT_LRxParts = 0;
		m_INT_OnLabels.Invoke();
		return true;
	}
}
