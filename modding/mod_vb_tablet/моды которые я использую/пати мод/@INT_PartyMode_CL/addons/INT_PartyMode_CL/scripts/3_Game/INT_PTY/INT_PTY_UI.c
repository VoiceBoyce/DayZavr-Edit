// INT PartyMode — public GUI API of the party HUD (spec.md §10, docs/GUI_API.md). For the owner's global GUI editor
// and any other client mod. Soft dependency: wrap calls in #ifdef INT_PARTYMODE_CL and check
// INT_PTY_UI.GUI_API_VERSION. Later versions only add.
//
// 3_Game cannot see the HUD (5_Mission), so the API speaks in primitive types, widgets and ScriptInvokers only; the
// HUD (INT_PTY_Hud) registers itself here as an INT_PTY_HudApi.
//
// Parts (INT_PTY_UI.PART_*):
//   "tracker"  — the «ГРУППА» panel (left middle by default; the player moves it with «Двигать в игре»)
//   "tags"     — name tags over the shown friends (world-anchored)
//   "pings"    — P5 «ВСПЫШКА» ping markers (world-anchored)
//   "toasts"   — notification cards (top right column)
//   "arrows"   — edge arrows to friends / pings outside the screen
//   "mapLayer" — friends and pings on the map mods (read by INT_PTY_MapLayer: scale / opacity / accent / hidden)
//
// Every part has an override (INT_PTY_HudPart): place (top-left corner in screen fractions — tracker and toasts only),
// scale, opacity, accent colour, hidden. The overrides multiply the player's own settings of the «Настройки» tab
// (TagSize, PingSize, TrackerAlpha…), they never replace them. The tracker place IS the player's setting (TrackerX/Y in
// INT_PartyMode_Client.json); every other override lives in $profile:INT_Mods/INT_PartyMode/INT_PartyMode_Hud.json
// (SaveLayout / LoadLayout). Zero in a field always means «default» (the engine JSON reader gives ZERO fields to every
// object it creates), so a hand-edited or partial file is safe.
//
// Version 1 (2026-10-23): parts, overrides, file, edit mode, external hide, preview, FindPart / GetPartRect, friend
// colours, palette, invokers OnBuilt / OnPlaced / OnStyleApplied / OnEditModeChanged / OnPlayerLayoutChanged.

// One part's override (one element of INT_PartyMode_Hud.json). 0 = default in every field.
class INT_PTY_HudPart
{
	static const float SCALE_MIN = 0.6;
	static const float SCALE_MAX = 1.6;
	static const float OPACITY_MIN = 0.2;

	string Name;
	int Moved;			// 1 = X / Y place the part (toasts; the tracker keeps its place in the player settings)
	float X;			// top-left corner, fraction of the screen width (0..1)
	float Y;			// top-left corner, fraction of the screen height (0..1)
	float Scale;		// 0 = 1.0; else SCALE_MIN .. SCALE_MAX (multiplies the player's own size setting)
	float Opacity;		// 0 = 1.0; else OPACITY_MIN .. 1 (multiplies the player's own opacity settings)
	int Accent;			// ARGB accent colour; 0 = the part's default
	int Hidden;			// 1 = hidden by the player / an editor

	bool IsMoved()
	{
		return Moved != 0;
	}

	bool IsShown()
	{
		return Hidden == 0;
	}

	float ScaleOr1()
	{
		if (Scale <= 0)
			return 1.0;
		return Scale;
	}

	float OpacityOr1()
	{
		if (Opacity <= 0)
			return 1.0;
		return Opacity;
	}

	void Clear()
	{
		Moved = 0;
		X = 0;
		Y = 0;
		Scale = 0;
		Opacity = 0;
		Accent = 0;
		Hidden = 0;
	}

	void CopyFrom(INT_PTY_HudPart o)
	{
		if (!o)
			return;
		Moved = o.Moved;
		X = o.X;
		Y = o.Y;
		Scale = o.Scale;
		Opacity = o.Opacity;
		Accent = o.Accent;
		Hidden = o.Hidden;
	}

	void Sanitize()
	{
		if (Moved != 0)
			Moved = 1;
		if (Hidden != 0)
			Hidden = 1;
		if (X != X || Y != Y)
		{
			Moved = 0;
			X = 0;
			Y = 0;
		}
		X = Math.Clamp(X, 0, 1);
		Y = Math.Clamp(Y, 0, 1);
		if (Scale != Scale || Scale <= 0)
			Scale = 0;
		else
			Scale = Math.Clamp(Scale, SCALE_MIN, SCALE_MAX);
		if (Opacity != Opacity || Opacity <= 0)
			Opacity = 0;
		else
			Opacity = Math.Clamp(Opacity, OPACITY_MIN, 1);
	}
}

// The client file.
class INT_PTY_HudFile
{
	int Version = 1;
	ref array<ref INT_PTY_HudPart> Parts;

	static INT_PTY_HudFile ForRead()
	{
		INT_PTY_HudFile f = new INT_PTY_HudFile();
		f.Parts = new array<ref INT_PTY_HudPart>();
		return f;
	}
}

// What the HUD (5_Mission) implements. Default bodies: the API works (and does nothing) without a HUD.
class INT_PTY_HudApi : Managed
{
	// Re-read the override of a part ("" = every part) and rebuild what depends on it.
	void ApplyPart(string part) {}

	// Only the place of a part changed (cheap, every frame while dragging).
	void PlacePart(string part) {}

	void SetEditMode(bool on) {}

	void RefreshVisibility() {}

	// Sample content of a part for its normal lifetime (editor preview, tests).
	void Preview(string part) {}

	Widget FindPart(string part)
	{
		return null;
	}

	bool GetPartRect(string part, out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		return false;
	}

	bool IsPartActive(string part)
	{
		return false;
	}
}

class INT_PTY_UI
{
	static const int GUI_API_VERSION = 1;

	static const string PART_TRACKER = "tracker";
	static const string PART_TAGS = "tags";
	static const string PART_PINGS = "pings";
	static const string PART_TOASTS = "toasts";
	static const string PART_ARROWS = "arrows";
	static const string PART_MAP = "mapLayer";

	static const string CLIENT_DIR_MODS = "$profile:INT_Mods";
	static const string CLIENT_DIR = "$profile:INT_Mods/INT_PartyMode";
	static const string CLIENT_FILE = "$profile:INT_Mods/INT_PartyMode/INT_PartyMode_Hud.json";

	// automatic friend colours (concept C data.js), picked by the Steam64 so a friend keeps his colour everywhere
	static const int AUTO_COLORS = 8;

	protected static ref map<string, ref INT_PTY_HudPart> s_Parts;
	protected static ref array<string> s_Names;
	protected static INT_PTY_HudApi s_Hud;				// weak: owned by the HUD (5_Mission)
	protected static bool s_Loaded;
	protected static bool s_EditMode;
	protected static bool s_Visible = true;
	protected static ref array<int> s_Auto;
	protected static ref ScriptInvoker s_OnBuilt;
	protected static ref ScriptInvoker s_OnPlaced;
	protected static ref ScriptInvoker s_OnStyleApplied;
	protected static ref ScriptInvoker s_OnEditModeChanged;
	protected static ref ScriptInvoker s_OnPlayerLayoutChanged;

	protected static void EnsureLoaded()
	{
		if (s_Loaded)
			return;
		s_Loaded = true;
		s_Names = new array<string>();
		s_Names.Insert(PART_TRACKER);
		s_Names.Insert(PART_TAGS);
		s_Names.Insert(PART_PINGS);
		s_Names.Insert(PART_TOASTS);
		s_Names.Insert(PART_ARROWS);
		s_Names.Insert(PART_MAP);
		s_Parts = new map<string, ref INT_PTY_HudPart>();
		foreach (string n : s_Names)
		{
			INT_PTY_HudPart p = new INT_PTY_HudPart();
			p.Name = n;
			s_Parts.Set(n, p);
		}
		if (GetGame() && !GetGame().IsDedicatedServer())
			ReadClientFile();
	}

	// --- parts ----------------------------------------------------------------------------------------------------
	static int PartCount()
	{
		EnsureLoaded();
		return s_Names.Count();
	}

	// Allocation-free iteration: for (int i = 0; i < INT_PTY_UI.PartCount(); i++) { string n = INT_PTY_UI.PartName(i); }
	static string PartName(int index)
	{
		EnsureLoaded();
		if (index < 0 || index >= s_Names.Count())
			return "";
		return s_Names.Get(index);
	}

	static void PartNames(array<string> outNames)
	{
		if (!outNames)
			return;
		EnsureLoaded();
		outNames.Clear();
		foreach (string n : s_Names)
		{
			outNames.Insert(n);
		}
	}

	static bool IsPart(string name)
	{
		EnsureLoaded();
		return s_Names.Find(name) >= 0;
	}

	// The part can be placed by SetPartPosition (the others are anchored to the world or to the screen edge).
	static bool HasPosition(string name)
	{
		return name == PART_TRACKER || name == PART_TOASTS;
	}

	// Stringtable key of the part's title (editor lists, edit-mode labels).
	static string PartTitleKey(string name)
	{
		if (name == PART_TRACKER)
			return "#STR_INT_PTY_H_PART_TRACKER";
		if (name == PART_TAGS)
			return "#STR_INT_PTY_H_PART_TAGS";
		if (name == PART_PINGS)
			return "#STR_INT_PTY_H_PART_PINGS";
		if (name == PART_TOASTS)
			return "#STR_INT_PTY_H_PART_TOASTS";
		if (name == PART_ARROWS)
			return "#STR_INT_PTY_H_PART_ARROWS";
		if (name == PART_MAP)
			return "#STR_INT_PTY_H_PART_MAP";
		return "";
	}

	// The live override of a part (null for an unknown name). Change it through the Set* functions (they apply it).
	static INT_PTY_HudPart GetPart(string name)
	{
		EnsureLoaded();
		INT_PTY_HudPart p;
		if (s_Parts.Find(name, p))
			return p;
		return null;
	}

	// Default accent: orange (tracker caption / arrows of the rows, tag distance, toast line); white for the ping
	// distance label; 0 for the arrows and the map layer = every mark in its friend's / ping's own colour.
	static int DefaultAccent(string name)
	{
		if (name == PART_PINGS)
			return 0xFFFFFFFF;
		if (name == PART_ARROWS || name == PART_MAP)
			return 0;
		return INT_PTY_Look.ORANGE;
	}

	static int PartAccent(string name)
	{
		INT_PTY_HudPart p = GetPart(name);
		if (p && p.Accent != 0)
			return p.Accent;
		return DefaultAccent(name);
	}

	static float PartScale(string name)
	{
		INT_PTY_HudPart p = GetPart(name);
		if (!p)
			return 1.0;
		return p.ScaleOr1();
	}

	static float PartOpacity(string name)
	{
		INT_PTY_HudPart p = GetPart(name);
		if (!p)
			return 1.0;
		return p.OpacityOr1();
	}

	static bool IsPartShown(string name)
	{
		INT_PTY_HudPart p = GetPart(name);
		if (!p)
			return false;
		return p.IsShown();
	}

	// --- overrides --------------------------------------------------------------------------------------------------
	// Top-left corner of the part in screen fractions (0..1). Cheap: only moves the part. false = the part has no
	// place of its own (tags, pings, arrows, map layer). The tracker place is the player's setting TrackerX/Y.
	static bool SetPartPosition(string name, float nx, float ny)
	{
		// H-9: NaN never reaches the player file (a NaN fails every comparison, Clamp would pass it through);
		// out-of-range values are clamped to the screen
		if (nx != nx || ny != ny)
			return false;
		float x = Math.Clamp(nx, 0, 1);
		float y = Math.Clamp(ny, 0, 1);
		if (name == PART_TRACKER)
		{
			INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
			if (!st)
				return false;
			st.TrackerX = x;
			st.TrackerY = y;
			INT_PTY_SettingsStore.MarkDirty();
		}
		else if (name == PART_TOASTS)
		{
			INT_PTY_HudPart p = GetPart(name);
			if (!p)
				return false;
			p.Moved = 1;
			p.X = x;
			p.Y = y;
		}
		else
		{
			return false;
		}
		if (s_Hud)
			s_Hud.PlacePart(name);
		OnPlaced().Invoke(name);
		return true;
	}

	// The place a part stands at by the player / an editor; false = its default place.
	static bool GetPartPosition(string name, out float nx, out float ny)
	{
		nx = 0;
		ny = 0;
		if (name == PART_TRACKER)
		{
			INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
			if (!st || st.TrackerX < 0 || st.TrackerY < 0)
				return false;
			nx = st.TrackerX;
			ny = st.TrackerY;
			return true;
		}
		INT_PTY_HudPart p = GetPart(name);
		if (!p || !p.IsMoved() || !HasPosition(name))
			return false;
		nx = p.X;
		ny = p.Y;
		return true;
	}

	static void ResetPartPosition(string name)
	{
		if (name == PART_TRACKER)
		{
			INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
			if (st)
			{
				st.TrackerX = -1;
				st.TrackerY = -1;
				INT_PTY_SettingsStore.MarkDirty();
			}
		}
		else
		{
			INT_PTY_HudPart p = GetPart(name);
			if (!p)
				return;
			p.Moved = 0;
			p.X = 0;
			p.Y = 0;
		}
		if (s_Hud)
			s_Hud.PlacePart(name);
		OnPlaced().Invoke(name);
	}

	// 0.6 .. 1.6 (0 = default 1); multiplies the player's TagSize / PingSize / TrackerSize.
	static void SetPartScale(string name, float k)
	{
		INT_PTY_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Scale = k;
		Applied(p);
	}

	// 0.2 .. 1 (0 = default 1); multiplies the player's opacity settings (TrackerAlpha, TagNameAlpha…).
	static void SetPartOpacity(string name, float opacity)
	{
		INT_PTY_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Opacity = opacity;
		Applied(p);
	}

	// ARGB accent; 0 = default (see DefaultAccent).
	static void SetPartAccent(string name, int argb)
	{
		INT_PTY_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Accent = argb;
		Applied(p);
	}

	static void SetPartShown(string name, bool shown)
	{
		INT_PTY_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Hidden = 1;
		if (shown)
			p.Hidden = 0;
		Applied(p);
	}

	// Back to the defaults (place, scale, colour, opacity, shown). The tracker goes back to its default place too.
	static void ResetPart(string name)
	{
		INT_PTY_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Clear();
		if (name == PART_TRACKER)
			ResetPartPosition(name);
		Applied(p);
		OnPlaced().Invoke(name);
	}

	static void ResetAll()
	{
		EnsureLoaded();
		foreach (string n : s_Names)
		{
			INT_PTY_HudPart p = GetPart(n);
			if (p)
				p.Clear();
		}
		INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
		if (st)
		{
			st.TrackerX = -1;
			st.TrackerY = -1;
			INT_PTY_SettingsStore.MarkDirty();
		}
		if (s_Hud)
			s_Hud.ApplyPart("");
		OnStyleApplied().Invoke("");
		OnPlaced().Invoke("");
	}

	protected static void Applied(INT_PTY_HudPart p)
	{
		p.Sanitize();
		if (s_Hud)
			s_Hud.ApplyPart(p.Name);
		OnStyleApplied().Invoke(p.Name);
	}

	// --- file -------------------------------------------------------------------------------------------------------
	// Writes INT_PartyMode_Hud.json and the player settings (the tracker place lives there).
	static void SaveLayout()
	{
		INT_PTY_SettingsStore.Flush();
		string txt = ExportLayout();
		if (txt == "")
			return;
		if (!FileExist(CLIENT_DIR_MODS))
			MakeDirectory(CLIENT_DIR_MODS);
		if (!FileExist(CLIENT_DIR))
			MakeDirectory(CLIENT_DIR);
		// PTY-11: .tmp -> read back -> replace (a crash mid-write never leaves a half file)
		if (!INT_PTY_ClientFile.SaveText(CLIENT_FILE, txt))
		{
			INT_PTY_Log.Warn("не удалось сохранить расположение HUD группы");
			return;
		}
		OnPlayerLayoutChanged().Invoke();
	}

	// Re-reads the player's file (unknown parts and broken values are dropped).
	static void LoadLayout()
	{
		EnsureLoaded();
		ReadClientFile();
		if (s_Hud)
			s_Hud.ApplyPart("");
		OnStyleApplied().Invoke("");
		OnPlaced().Invoke("");
	}

	// The overrides as JSON (presets of an editor). The tracker place is not in it (player setting).
	static string ExportLayout()
	{
		EnsureLoaded();
		INT_PTY_HudFile f = INT_PTY_HudFile.ForRead();
		foreach (string n : s_Names)
		{
			INT_PTY_HudPart p = GetPart(n);
			if (p)
				f.Parts.Insert(p);
		}
		string txt;
		JsonSerializer js = new JsonSerializer();
		if (!js.WriteToString(f, true, txt))
			return "";
		return txt;
	}

	// Takes a layout made by ExportLayout (parts missing in it keep their values). Not saved: call SaveLayout.
	static bool ImportLayout(string json)
	{
		EnsureLoaded();
		if (!Merge(json))
			return false;
		if (s_Hud)
			s_Hud.ApplyPart("");
		OnStyleApplied().Invoke("");
		OnPlaced().Invoke("");
		return true;
	}

	protected static void ReadClientFile()
	{
		// PTY-11: the helper also finishes an interrupted save (.tmp left without the file)
		string txt;
		if (!INT_PTY_ClientFile.Read(CLIENT_FILE, txt))
			return;
		if (!Merge(txt))
		{
			// a copy of the broken file is kept before the defaults are used (the next save overwrites it)
			INT_PTY_ClientFile.KeepBroken(CLIENT_FILE);
			INT_PTY_Log.Warn("INT_PartyMode_Hud.json не прочитан (копия .broken), HUD группы по умолчанию");
		}
	}

	protected static bool Merge(string json)
	{
		INT_PTY_HudFile f = INT_PTY_HudFile.ForRead();
		string err;
		JsonSerializer js = new JsonSerializer();
		if (!js.ReadFromString(f, json, err))
			return false;
		if (!f.Parts)
			return true;
		foreach (INT_PTY_HudPart src : f.Parts)
		{
			if (!src)
				continue;
			INT_PTY_HudPart dst = GetPart(src.Name);
			if (!dst)
				continue;
			dst.CopyFrom(src);
			dst.Sanitize();
		}
		return true;
	}

	// --- modes ------------------------------------------------------------------------------------------------------
	// Edit mode: every part shows sample content without a lifetime, outlined and labelled, also over menus (a hidden
	// part is drawn faint so an editor can switch it back on). Tags / pings / arrows samples stand at fixed places.
	static void SetEditMode(bool on)
	{
		if (s_EditMode == on)
			return;
		s_EditMode = on;
		if (s_Hud)
			s_Hud.SetEditMode(on);
		OnEditModeChanged().Invoke(on);
	}

	static bool IsEditMode()
	{
		return s_EditMode;
	}

	// External hide of the whole HUD layer (cinematic / screenshot mode of another mod).
	static void SetVisible(bool visible)
	{
		s_Visible = visible;
		if (s_Hud)
			s_Hud.RefreshVisibility();
	}

	static bool IsVisible()
	{
		return s_Visible;
	}

	// H-6: end of the game session (INT_PTY_Hud.Shutdown): edit mode and the external hide do not survive a reconnect.
	// Silent on purpose — the HUD and its listeners are being torn down.
	static void ResetSession()
	{
		s_EditMode = false;
		s_Visible = true;
	}

	// Sample content of one part for its normal lifetime (toasts: one sample card; pings: one sample ping 60 m ahead).
	static void Preview(string name)
	{
		if (s_Hud && IsPart(name))
			s_Hud.Preview(name);
	}

	// --- widgets ----------------------------------------------------------------------------------------------------
	// Root frame of a part (screen px; exists from the first frame the HUD layer is built).
	static Widget FindPart(string name)
	{
		if (!s_Hud)
			return null;
		return s_Hud.FindPart(name);
	}

	// Screen rect of a part in screen px; false while the part shows nothing. Tags / pings / arrows: the box around
	// everything of that part on screen now.
	static bool GetPartRect(string name, out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		if (!s_Hud)
			return false;
		return s_Hud.GetPartRect(name, x, y, w, h);
	}

	// The part has real content on screen right now (a row, a tag, a ping, a card, an arrow).
	static bool IsPartActive(string name)
	{
		if (!s_Hud)
			return false;
		return s_Hud.IsPartActive(name);
	}

	// --- colours ----------------------------------------------------------------------------------------------------
	// The 8 colour swatches of the concept (+ white = 9th swatch of the settings), same order as the window.
	static int Swatch(int index)
	{
		if (!s_Auto)
		{
			s_Auto = new array<int>();
			s_Auto.Insert(0xFFFF5A4F);
			s_Auto.Insert(0xFFFFB02E);
			s_Auto.Insert(0xFFFFE066);
			s_Auto.Insert(0xFF7BE36B);
			s_Auto.Insert(0xFF3FD0FF);
			s_Auto.Insert(0xFF5C7CFF);
			s_Auto.Insert(0xFFB48CFF);
			s_Auto.Insert(0xFFFF7AD9);
			s_Auto.Insert(0xFFFFFFFF);
		}
		if (index < 0 || index >= s_Auto.Count())
			return 0xFFFFFFFF;
		return s_Auto.Get(index);
	}

	// Automatic colour of a friend: stable for his Steam64 (the same hash as the window, INT_PTY_MenuGeo.AutoColor).
	static int AutoColor(string id)
	{
		int h = 7;
		int n = id.Length();
		string digits = "0123456789";
		for (int i = 0; i < n; i++)
		{
			string ch = id.Get(i);
			int d = digits.IndexOf(ch);
			if (d < 0)
				d = 0;
			h = (h * 31 + d) % 1000003;
		}
		return Swatch(h % AUTO_COLORS);
	}

	// The colour a friend is drawn with: the player's choice (pref Color) or the automatic one.
	static int FriendColor(string id)
	{
		INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
		if (st)
		{
			INT_PTY_FriendPref p = st.Pref(id);
			if (p && p.Color != 0)
				return p.Color;
		}
		return AutoColor(id);
	}

	// How this player sees the pings of `owner` (brief): his own pings = settings PingShape / PingColor; a friend's =
	// the pref PingShape / PingColor the player set for him, else the player's default shape in the friend's colour.
	static int PingShapeOf(string owner, bool mine)
	{
		INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
		if (!st)
			return INT_PTY_Shape.CROSS;
		int s = st.PingShape;
		if (!mine)
		{
			INT_PTY_FriendPref p = st.Pref(owner);
			if (p && p.PingShape >= 0)
				s = p.PingShape;
		}
		if (s < 0 || s >= INT_PTY_Shape.COUNT)
			s = INT_PTY_Shape.CROSS;
		return s;
	}

	static int PingColorOf(string owner, bool mine)
	{
		INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
		if (!st)
			return 0xFFFF5A4F;
		if (mine)
		{
			if (st.PingColor == 0)
				return 0xFFFF5A4F;
			return st.PingColor | 0xFF000000;
		}
		INT_PTY_FriendPref p = st.Pref(owner);
		if (p && p.PingColor != 0)
			return p.PingColor | 0xFF000000;
		return FriendColor(owner);
	}

	// Texture of a ping shape (INT_PTY_Shape.*); outline = the dark twin drawn under it.
	static string PingTex(int shape, bool outline)
	{
		string n = "cross";
		if (shape == INT_PTY_Shape.CHECK)
			n = "check";
		else if (shape == INT_PTY_Shape.CIRCLE)
			n = "circle";
		else if (shape == INT_PTY_Shape.DIAMOND)
			n = "diamond";
		else if (shape == INT_PTY_Shape.TRIANGLE)
			n = "triangle";
		else if (shape == INT_PTY_Shape.EXCL)
			n = "excl";
		if (outline)
			return INT_PTY_TEX + "pty_ping_" + n + "_out_ca.paa";
		return INT_PTY_TEX + "pty_ping_" + n + "_ca.paa";
	}

	// Palette of the Expedition style by key (StyleKeys lists them); 0 for an unknown key.
	static int StyleColor(string key)
	{
		if (key == "accent")
			return INT_PTY_Look.ORANGE;
		if (key == "accentSoft")
			return INT_PTY_Look.ORANGE_SOFT;
		if (key == "white")
			return INT_PTY_Look.WHITE;
		if (key == "text")
			return INT_PTY_Look.TEXT;
		if (key == "body")
			return INT_PTY_Look.BODY;
		if (key == "muted")
			return INT_PTY_Look.MUTED;
		if (key == "faint")
			return INT_PTY_Look.FAINT;
		if (key == "line")
			return INT_PTY_Look.LINE;
		if (key == "lineStrong")
			return INT_PTY_Look.LINE_STRONG;
		if (key == "red")
			return INT_PTY_Look.RED;
		if (key == "panel")
			return INT_PTY_Look.PANEL;
		if (key == "critical")
			return 0xFFFF6D61;
		if (key == "hpGood")
			return 0xFF56E07E;
		if (key == "hpMid")
			return 0xFFFFC246;
		if (key == "hpLow")
			return 0xFFFF4B3E;
		return 0;
	}

	static void StyleKeys(array<string> outKeys)
	{
		if (!outKeys)
			return;
		outKeys.Clear();
		outKeys.Insert("accent");
		outKeys.Insert("accentSoft");
		outKeys.Insert("white");
		outKeys.Insert("text");
		outKeys.Insert("body");
		outKeys.Insert("muted");
		outKeys.Insert("faint");
		outKeys.Insert("line");
		outKeys.Insert("lineStrong");
		outKeys.Insert("red");
		outKeys.Insert("panel");
		outKeys.Insert("critical");
		outKeys.Insert("hpGood");
		outKeys.Insert("hpMid");
		outKeys.Insert("hpLow");
	}

	// --- events (ScriptInvoker) -----------------------------------------------------------------------------------
	static ScriptInvoker OnBuilt()						// () the HUD layer was (re)built (first frame, resolution, sizes)
	{
		if (!s_OnBuilt)
			s_OnBuilt = new ScriptInvoker();
		return s_OnBuilt;
	}

	static ScriptInvoker OnPlaced()						// (string part) a part moved ("" = all)
	{
		if (!s_OnPlaced)
			s_OnPlaced = new ScriptInvoker();
		return s_OnPlaced;
	}

	static ScriptInvoker OnStyleApplied()				// (string part) scale / colour / opacity / shown changed ("" = all)
	{
		if (!s_OnStyleApplied)
			s_OnStyleApplied = new ScriptInvoker();
		return s_OnStyleApplied;
	}

	static ScriptInvoker OnEditModeChanged()			// (bool on)
	{
		if (!s_OnEditModeChanged)
			s_OnEditModeChanged = new ScriptInvoker();
		return s_OnEditModeChanged;
	}

	static ScriptInvoker OnPlayerLayoutChanged()		// () the layout file was saved
	{
		if (!s_OnPlayerLayoutChanged)
			s_OnPlayerLayoutChanged = new ScriptInvoker();
		return s_OnPlayerLayoutChanged;
	}

	// --- internal: the HUD registers itself -------------------------------------------------------------------------
	static void RegisterHud(INT_PTY_HudApi api)
	{
		s_Hud = api;
	}
}
