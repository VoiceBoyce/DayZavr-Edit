// INT_VPP «Экспедиция» B — data classes (NET, BUILD_PLAN §2 «Data / net»). API + file formats: design/concepts_expedition/NET_API.md
// Field names are JSON keys AND RPC field order: never rename, reorder or retype them.
// JSON-safe types only (int, float, string, array<float>, array<string>, nested classes). No `ref` on method params.
// The engine JSON reader gives 0 / "" / empty arrays to objects it creates, so every reader re-checks with Sanitize().
// Client + shared payload classes only. The server files (INT_VPP_ServerSettings = Settings.json, INT_VPP_ZonesFile =
// Zones.json) and INT_VPP_ClientSettings.CopyFrom live in the server-only PBO INT_VppMap_SE (2026-11-12 CL/SE split).

// What the server sends ONE client (RPC INT_Settings): its own admin flag + the public numbers of Settings.json.
class INT_VPP_ClientSettings
{
	int Protocol;
	// 1 = this client may use the zone editor (UI only; the server re-checks every write).
	int IsAdmin;
	int ZoneEnterNotice;
	float RouteReachRadius;
	int RouteMaxPoints;
	int RulerMaxPoints;
	int Dist3DServerMarkers;
	int Dist3DMyMarkers;
	int Dist3DEvents;
	int Dist3DRoute;
	int MaxZones;
	int MaxZoneVertices;
	// 2026-11-13 (appended last, RPC order kept): 1 = the minimap is shown only while the player carries a GPS
	// receiver that is ON (cosmetic client check, nothing to protect); Settings.json INT_MiniMapRequireGps
	int INT_MiniMapRequireGps;

	void INT_VPP_ClientSettings()
	{
		SetDefaults();
	}

	void SetDefaults()
	{
		Protocol = INT_VPP_Net.PROTOCOL;
		IsAdmin = 0;
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
		INT_MiniMapRequireGps = 0;
	}

	// Same clamps as the server file; the client runs it on every received copy.
	int Sanitize()
	{
		int fixes = 0;
		if (IsAdmin != 0 && IsAdmin != 1)
		{
			IsAdmin = 0;
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
		if (INT_MiniMapRequireGps != 0 && INT_MiniMapRequireGps != 1)
		{
			INT_MiniMapRequireGps = 0;
			fixes++;
		}
		return fixes;
	}
}

// One admin zone. shape INT_VPP_Net.SHAPE_POLYGON (0): xs/zs = vertex world X/Z (same count, 3..32), cx/cz = vertex
// average (label anchor, set by the server), r = 0. shape INT_VPP_Net.SHAPE_CIRCLE (1): cx/cz = centre, r = radius
// 5..5000 m, xs/zs empty. color = ARGB as a signed int, alpha always 255. id 0 = "new" in a save request only.
class INT_VPP_Zone
{
	int id;
	string name;
	string sub;
	int color;
	int shape;
	ref array<float> xs = new array<float>();
	ref array<float> zs = new array<float>();
	float cx;
	float cz;
	float r;

	bool IsCircle()
	{
		if (shape == INT_VPP_Net.SHAPE_CIRCLE)
			return true;
		return false;
	}

	int VertexCount()
	{
		if (!xs || !zs)
			return 0;
		int nx = xs.Count();
		int nz = zs.Count();
		if (nz < nx)
			return nz;
		return nx;
	}

	// Deep copy (arrays copied element by element). Callers bound the array sizes first.
	INT_VPP_Zone Clone()
	{
		INT_VPP_Zone c = new INT_VPP_Zone();
		c.id = id;
		c.name = name;
		c.sub = sub;
		c.color = color;
		c.shape = shape;
		c.cx = cx;
		c.cz = cz;
		c.r = r;
		array<float> cxs = new array<float>();
		array<float> czs = new array<float>();
		if (xs)
		{
			foreach (float vx : xs)
			{
				cxs.Insert(vx);
			}
		}
		if (zs)
		{
			foreach (float vz : zs)
			{
				czs.Insert(vz);
			}
		}
		c.xs = cxs;
		c.zs = czs;
		return c;
	}
}

// One admin text label on the map (owner request 2026-11-13): a single line drawn like a town name, fixed screen
// size, centred on its world point. Labels.json element AND RPC payload: never rename, reorder or retype the fields.
// INT_Id 0 = "new" in a save request only. INT_Text: one line, <= 48 characters and <= 96 bytes (whole UTF-8
// characters). INT_Color: ARGB, alpha 255, one of INT_VPP_Net.INT_LabelColorAt(0..9). INT_Size: design px of 1080p,
// one of INT_VPP_Net.INT_LabelSizeAt(0..5). INT_X / INT_Z: world position of the text centre.
// Field names carry INT_ (own fields: a class of another mod spelled like a plain field breaks the compile).
class INT_VPP_Label
{
	int INT_Id;
	string INT_Text;
	int INT_Color;
	int INT_Size;
	float INT_X;
	float INT_Z;

	INT_VPP_Label INT_Clone()
	{
		INT_VPP_Label c = new INT_VPP_Label();
		c.INT_Id = INT_Id;
		c.INT_Text = INT_Text;
		c.INT_Color = INT_Color;
		c.INT_Size = INT_Size;
		c.INT_X = INT_X;
		c.INT_Z = INT_Z;
		return c;
	}

	vector INT_Pos()
	{
		float px = INT_X;
		float pz = INT_Z;
		return Vector(px, 0, pz);
	}
}
