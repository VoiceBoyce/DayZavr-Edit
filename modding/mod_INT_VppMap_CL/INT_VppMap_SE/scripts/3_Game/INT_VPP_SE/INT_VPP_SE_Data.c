// INT_VppMap_SE (server only, -serverMod) — the map's server data classes and the server-only helpers.
// Moved here from the client PBO (VanillaPPMap 3_Game/INT_VPP/INT_VPP_Data.c + INT_VPP_Net.c) on 2026-11-12 so that
// the client mod carries no server code. Names, fields and behaviour unchanged: Settings.json / Zones.json keep their
// format, and the helpers keep their names (modded classes add them back to the shared client classes, so the
// server code calls them exactly as before). The client never needs any of this.
// File places: $profile:INT_Mods/INT_VppMap/ (moved once from the old places, INT_VPP_SE_Migrate.c).

// $profile:INT_Mods/INT_VppMap/Settings.json (server only; created with these defaults when missing).
class INT_VPP_ServerSettings
{
	int ConfigVersion = 1;
	// Steam64 ids (17 digits) of the map admins (zone editor).
	ref array<string> Admins = new array<string>();
	// 1 = every VPPAdminTools superadmin ($profile:VPPAdminTools/Permissions/SuperAdmins/SuperAdmins.txt) is a map admin too.
	int UseVPPAdminToolsSuperAdmins = 1;
	// 1 = clients show a notice when the player enters a zone.
	int ZoneEnterNotice = 0;
	// Route point counts as reached at this 2D distance, metres.
	float RouteReachRadius = 15;
	int RouteMaxPoints = 20;
	int RulerMaxPoints = 20;
	// 3D marker distance caps per category, metres (0 = no 3D markers for that category).
	int Dist3DServerMarkers = 3000;
	int Dist3DMyMarkers = 2000;
	int Dist3DEvents = 3000;
	int Dist3DRoute = 5000;
	// Zone limits for NEW zones (existing zones are never dropped because of these two).
	int MaxZones = 64;
	int MaxZoneVertices = 32;
	// Admin text labels (2026-11-13): limit for NEW labels (0..500; existing labels are never dropped because of it).
	// Added key: a file without it gets 200 (FillMissing); ConfigVersion stays 1 (older builds ignore the key).
	int INT_MaxLabels = 200;
	// Minimap (2026-11-13): 1 = players see the minimap only while they carry a GPS receiver that is ON (with power),
	// as the old @MiniMap + MiniMapGPS did; 0 = always (owner: «по умолчанию без GPS»). Sent to every client in
	// INT_Settings. An old file gets the key with 0 (FillMissing); ConfigVersion stays 1.
	int INT_MiniMapRequireGps = 0;

	// RF_MAP marker import (2026-11-13): 1 = this server replaced RF_MAP; its clients import the player's own RF_MAP
	// markers once into this server's personal list (client INT_VPP_RFImport). Added key: a file without it gets 0.
	int INT_ImportRFMapMarkers = 0;
	// Server markers edited in game by map admins (2026-11-14): limit of the VPPMapConfig.json list for NEW markers
	// (0..1000; existing markers are never dropped because of it). A file without the key gets 300.
	int INT_MaxServerMarkers = 300;

	void SetDefaults()
	{
		ConfigVersion = INT_VPP_Net.SETTINGS_VERSION;
		Admins = new array<string>();
		UseVPPAdminToolsSuperAdmins = 1;
		ZoneEnterNotice = 0;
		RouteReachRadius = INT_VPP_Net.DEF_ROUTE_REACH_RADIUS;
		RouteMaxPoints = INT_VPP_Net.DEF_ROUTE_MAX_POINTS;
		RulerMaxPoints = INT_VPP_Net.DEF_RULER_MAX_POINTS;
		Dist3DServerMarkers = INT_VPP_Net.DEF_DIST3D_SERVER;
		Dist3DMyMarkers = INT_VPP_Net.DEF_DIST3D_MINE;
		Dist3DEvents = INT_VPP_Net.DEF_DIST3D_EVENTS;
		Dist3DRoute = INT_VPP_Net.DEF_DIST3D_ROUTE;
		MaxZones = INT_VPP_Net.HARD_MAX_ZONES;
		MaxZoneVertices = INT_VPP_Net.HARD_MAX_ZONE_VERTICES;
		INT_MaxLabels = INT_VPP_Net.INT_DEF_MAX_LABELS;
		INT_MiniMapRequireGps = 0;
		INT_ImportRFMapMarkers = 0;
		INT_MaxServerMarkers = INT_VPP_Net.INT_DEF_MAX_SRV_MARKERS;
	}

	// Keys missing from the raw file text get their defaults (the reader would leave them 0). Returns how many.
	int FillMissing(string raw)
	{
		int miss = 0;
		if (!INT_VPP_Validate.HasJsonKey(raw, "ConfigVersion"))
		{
			ConfigVersion = INT_VPP_Net.SETTINGS_VERSION;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "Admins"))
		{
			Admins = new array<string>();
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "UseVPPAdminToolsSuperAdmins"))
		{
			UseVPPAdminToolsSuperAdmins = 1;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "ZoneEnterNotice"))
		{
			ZoneEnterNotice = 0;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "RouteReachRadius"))
		{
			RouteReachRadius = INT_VPP_Net.DEF_ROUTE_REACH_RADIUS;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "RouteMaxPoints"))
		{
			RouteMaxPoints = INT_VPP_Net.DEF_ROUTE_MAX_POINTS;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "RulerMaxPoints"))
		{
			RulerMaxPoints = INT_VPP_Net.DEF_RULER_MAX_POINTS;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "Dist3DServerMarkers"))
		{
			Dist3DServerMarkers = INT_VPP_Net.DEF_DIST3D_SERVER;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "Dist3DMyMarkers"))
		{
			Dist3DMyMarkers = INT_VPP_Net.DEF_DIST3D_MINE;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "Dist3DEvents"))
		{
			Dist3DEvents = INT_VPP_Net.DEF_DIST3D_EVENTS;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "Dist3DRoute"))
		{
			Dist3DRoute = INT_VPP_Net.DEF_DIST3D_ROUTE;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "MaxZones"))
		{
			MaxZones = INT_VPP_Net.HARD_MAX_ZONES;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "MaxZoneVertices"))
		{
			MaxZoneVertices = INT_VPP_Net.HARD_MAX_ZONE_VERTICES;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "INT_MaxLabels"))
		{
			INT_MaxLabels = INT_VPP_Net.INT_DEF_MAX_LABELS;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "INT_MiniMapRequireGps"))
		{
			INT_MiniMapRequireGps = 0;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "INT_ImportRFMapMarkers"))
		{
			INT_ImportRFMapMarkers = 0;
			miss++;
		}
		if (!INT_VPP_Validate.HasJsonKey(raw, "INT_MaxServerMarkers"))
		{
			INT_MaxServerMarkers = INT_VPP_Net.INT_DEF_MAX_SRV_MARKERS;
			miss++;
		}
		return miss;
	}

	// Clamps every value into its legal range. Admins entries are NOT edited here (the server logs and skips bad ids).
	// A version ABOVE this build is left as it is: the server then uses the file read-only and never rewrites it
	// (a newer build's file must not be turned into a v1 file that drops its keys).
	int Sanitize()
	{
		int fixes = 0;
		if (ConfigVersion < INT_VPP_Net.SETTINGS_VERSION)
		{
			ConfigVersion = INT_VPP_Net.SETTINGS_VERSION;
			fixes++;
		}
		if (!Admins)
		{
			Admins = new array<string>();
			fixes++;
		}
		if (UseVPPAdminToolsSuperAdmins != 0 && UseVPPAdminToolsSuperAdmins != 1)
		{
			UseVPPAdminToolsSuperAdmins = 1;
			fixes++;
		}
		if (ZoneEnterNotice != 0 && ZoneEnterNotice != 1)
		{
			ZoneEnterNotice = 0;
			fixes++;
		}
		float rr = INT_VPP_Validate.ClampFloat(RouteReachRadius, INT_VPP_Net.MIN_ROUTE_REACH_RADIUS, INT_VPP_Net.MAX_ROUTE_REACH_RADIUS, INT_VPP_Net.DEF_ROUTE_REACH_RADIUS);
		if (rr != RouteReachRadius)
		{
			RouteReachRadius = rr;
			fixes++;
		}
		int rp = INT_VPP_Validate.ClampInt(RouteMaxPoints, 1, INT_VPP_Net.HARD_MAX_ROUTE_POINTS);
		if (rp != RouteMaxPoints)
		{
			RouteMaxPoints = rp;
			fixes++;
		}
		int lp = INT_VPP_Validate.ClampInt(RulerMaxPoints, 2, INT_VPP_Net.HARD_MAX_RULER_POINTS);
		if (lp != RulerMaxPoints)
		{
			RulerMaxPoints = lp;
			fixes++;
		}
		int d1 = INT_VPP_Validate.ClampInt(Dist3DServerMarkers, 0, INT_VPP_Net.HARD_MAX_3D_DIST);
		if (d1 != Dist3DServerMarkers)
		{
			Dist3DServerMarkers = d1;
			fixes++;
		}
		int d2 = INT_VPP_Validate.ClampInt(Dist3DMyMarkers, 0, INT_VPP_Net.HARD_MAX_3D_DIST);
		if (d2 != Dist3DMyMarkers)
		{
			Dist3DMyMarkers = d2;
			fixes++;
		}
		int d3 = INT_VPP_Validate.ClampInt(Dist3DEvents, 0, INT_VPP_Net.HARD_MAX_3D_DIST);
		if (d3 != Dist3DEvents)
		{
			Dist3DEvents = d3;
			fixes++;
		}
		int d4 = INT_VPP_Validate.ClampInt(Dist3DRoute, 0, INT_VPP_Net.HARD_MAX_3D_DIST);
		if (d4 != Dist3DRoute)
		{
			Dist3DRoute = d4;
			fixes++;
		}
		int mz = INT_VPP_Validate.ClampInt(MaxZones, 0, INT_VPP_Net.HARD_MAX_ZONES);
		if (mz != MaxZones)
		{
			MaxZones = mz;
			fixes++;
		}
		int mv = INT_VPP_Validate.ClampInt(MaxZoneVertices, 3, INT_VPP_Net.HARD_MAX_ZONE_VERTICES);
		if (mv != MaxZoneVertices)
		{
			MaxZoneVertices = mv;
			fixes++;
		}
		int ml = INT_VPP_Validate.ClampInt(INT_MaxLabels, 0, INT_VPP_Net.INT_HARD_MAX_LABELS);
		if (ml != INT_MaxLabels)
		{
			INT_MaxLabels = ml;
			fixes++;
		}
		if (INT_MiniMapRequireGps != 0 && INT_MiniMapRequireGps != 1)
		{
			INT_MiniMapRequireGps = 0;
			fixes++;
		}
		if (INT_ImportRFMapMarkers != 0 && INT_ImportRFMapMarkers != 1)
		{
			INT_ImportRFMapMarkers = 0;
			fixes++;
		}
		int msm = INT_VPP_Validate.ClampInt(INT_MaxServerMarkers, 0, INT_VPP_Net.INT_HARD_MAX_SRV_MARKERS);
		if (msm != INT_MaxServerMarkers)
		{
			INT_MaxServerMarkers = msm;
			fixes++;
		}
		return fixes;
	}
}

// $profile:INT_Mods/INT_VppMap/Zones.json (server only). NextId is never reused, so a deleted id never comes back.
// FileVersion above INT_VPP_Net.ZONES_FILE_VERSION = written by a newer build: served read-only, never rewritten.
class INT_VPP_ZonesFile
{
	int FileVersion = 1;
	int NextId = 1;
	ref array<ref INT_VPP_Zone> ZoneList = new array<ref INT_VPP_Zone>();
}

// $profile:INT_Mods/INT_VppMap/Labels.json (server only, 2026-11-13). INT_NextId is never reused.
// INT_FileVersion above INT_VPP_Net.INT_LABELS_FILE_VERSION = written by a newer build: served read-only, never rewritten.
class INT_VPP_LabelsFile
{
	int INT_FileVersion = 1;
	int INT_NextId = 1;
	ref array<ref INT_VPP_Label> INT_LabelList = new array<ref INT_VPP_Label>();
}

// What the server sends ONE client is built from the server settings (only the server has them).
modded class INT_VPP_ClientSettings
{
	void CopyFrom(INT_VPP_ServerSettings s, bool admin)
	{
		SetDefaults();
		int adm = 0;
		if (admin)
			adm = 1;
		IsAdmin = adm;
		if (!s)
			return;
		ZoneEnterNotice = s.ZoneEnterNotice;
		RouteReachRadius = s.RouteReachRadius;
		RouteMaxPoints = s.RouteMaxPoints;
		RulerMaxPoints = s.RulerMaxPoints;
		Dist3DServerMarkers = s.Dist3DServerMarkers;
		Dist3DMyMarkers = s.Dist3DMyMarkers;
		Dist3DEvents = s.Dist3DEvents;
		Dist3DRoute = s.Dist3DRoute;
		MaxZones = s.MaxZones;
		MaxZoneVertices = s.MaxZoneVertices;
		INT_MiniMapRequireGps = s.INT_MiniMapRequireGps;
	}
}

// Server-only checks of INT_VPP_Validate (the shared pure helpers stay in the client PBO).
modded class INT_VPP_Validate
{
	static bool HasJsonKey(string raw, string key)
	{
		string needle = "\"" + key + "\"";
		int at = raw.IndexOf(needle);
		if (at >= 0)
			return true;
		return false;
	}

	// Integer value of the FIRST "key": <int> in raw JSON text (used only to read the version of a file the JSON
	// reader rejected). Only a 40-byte piece after the key is looked at (no Replace, no long substrings).
	static bool INT_JsonInt(string raw, string key, out int value)
	{
		value = 0;
		string needle = "\"" + key + "\"";
		int at = raw.IndexOf(needle);
		if (at < 0)
			return false;
		int len = raw.Length();
		int nl = needle.Length();
		int startAt = at + nl;
		int take = len - startAt;
		if (take > 40)
			take = 40;
		if (take <= 0)
			return false;
		string rest = raw.Substring(startAt, take);
		int n = rest.Length();
		int i = 0;
		bool colon = false;
		while (i < n)
		{
			string ch = rest.Get(i);
			int code = ch.ToAscii();
			if (code == 58)
			{
				if (colon)
					return false;
				colon = true;
				i++;
				continue;
			}
			if (code <= 32)
			{
				i++;
				continue;
			}
			break;
		}
		if (!colon)
			return false;
		string digits = "";
		if (i < n)
		{
			string first = rest.Get(i);
			if (first == "-")
			{
				digits = "-";
				i++;
			}
		}
		int nDigits = 0;
		while (i < n)
		{
			string d = rest.Get(i);
			int dc = d.ToAscii();
			if (dc < 48 || dc > 57)
				break;
			digits = digits + d;
			nDigits++;
			if (nDigits > 9)
				return false;
			i++;
		}
		if (nDigits == 0)
			return false;
		value = digits.ToInt();
		return true;
	}

	static bool IsSteam64(string s)
	{
		if (s.Length() != 17)
			return false;
		string digits = "0123456789";
		for (int i = 0; i < 17; i++)
		{
			string c = s.Get(i);
			if (digits.IndexOf(c) == -1)
				return false;
		}
		return true;
	}

	// Server-side zone check with the server's MaxZoneVertices.
	static bool Zone(INT_VPP_Zone z, INT_VPP_ServerSettings s, out string reason)
	{
		int maxV = INT_VPP_Net.HARD_MAX_ZONE_VERTICES;
		if (s)
			maxV = s.MaxZoneVertices;
		string why;
		bool ok = INT_VPP_Validate.ZoneLimits(z, maxV, why);
		reason = why;
		return ok;
	}
}

// Id allocation happens only on the server (the client only looks zones up by id).
modded class INT_VPP_ZoneIds
{
	static int MaxId(array<ref INT_VPP_Zone> zones)
	{
		int best = 0;
		if (!zones)
			return best;
		int n = zones.Count();
		for (int i = 0; i < n; i++)
		{
			INT_VPP_Zone z = zones.Get(i);
			if (z && z.id > best)
				best = z.id;
		}
		return best;
	}

	// Next free id: max(hint, highest id + 1), at least 1. Ids are never reused while NextId is kept in the file.
	static int Next(array<ref INT_VPP_Zone> zones, int hint)
	{
		int top = INT_VPP_ZoneIds.MaxId(zones);
		int nid = top + 1;
		if (hint > nid)
			nid = hint;
		if (nid < 1)
			nid = 1;
		return nid;
	}
}

// Label id allocation happens only on the server (the client only looks labels up by id).
modded class INT_VPP_LabelIds
{
	static int INT_MaxId(array<ref INT_VPP_Label> labels)
	{
		int best = 0;
		if (!labels)
			return best;
		int n = labels.Count();
		for (int i = 0; i < n; i++)
		{
			INT_VPP_Label l = labels.Get(i);
			if (l && l.INT_Id > best)
				best = l.INT_Id;
		}
		return best;
	}

	// Next free id: max(hint, highest id + 1), at least 1. Ids are never reused while INT_NextId is kept in the file.
	static int INT_Next(array<ref INT_VPP_Label> labels, int hint)
	{
		int top = INT_VPP_LabelIds.INT_MaxId(labels);
		int nid = top + 1;
		if (hint > nid)
			nid = hint;
		if (nid < 1)
			nid = 1;
		return nid;
	}
}
