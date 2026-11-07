// INT_PDA — public GUI API of the PDA HUD (spec.md §10, docs/GUI_API.md). For a global GUI editor (INT Customizable
// Huds) and any other client mod. Soft dependency: wrap calls in #ifdef INT_PDA_CL and check
// INT_PDA_UI.GUI_API_VERSION. Later versions only add.
//
// 3_Game cannot see the HUD (5_Mission), so the API speaks in primitive types, widgets and ScriptInvokers only; the
// HUD (INT_PDA_Hud / INT_PDA_HudLayer) registers itself here as an INT_PDA_HudApi.
//
// Parts (INT_PDA_UI.PART_*):
//   "feed"    — PDA feed: the last lines of the public chat (top left, grows down; the player can switch it off in
//               the PDA profile)
//   "toast"   — «Вам пришло сообщение от …» (top right column)
//   "news"    — news banner (top centre)
//   "sos"     — SOS alert of a faction mate (top right column)
//   "battery" — low battery tag / «КПК разряжен» (bottom left, above the vanilla item-in-hands panel)
//   "invite"  — faction invitation (top right column)
//   "faction" — a new line in the faction chat while the PDA is closed (top right column; since version 2)
//
// Every part has its own override (INT_PDA_HudPart): place (top-left corner in screen fractions), width, scale,
// accent colour, opacity, shown / hidden. The overrides live in $profile:INT_Mods/INT_PDA/INT_PDA_Hud.json
// (SaveLayout / LoadLayout). Zero in a field always means «default» (the engine JSON reader gives ZERO fields to
// every object it creates, measured on 1.30), so a hand-edited or partial file is safe.
//
//   INT_PDA_UI.SetEditMode(true);                          // every part shows sample content, outlined, over menus
//   Widget w = INT_PDA_UI.FindPart(INT_PDA_UI.PART_SOS);   // root frame of the part (screen px), null before the
//                                                          // HUD layer exists
//   INT_PDA_UI.SetPartPosition("news", 0.30, 0.05);        // cheap: may be called every frame while dragging
//   INT_PDA_UI.SetPartScale("feed", 1.2);   INT_PDA_UI.SetPartWidth("feed", 520);   INT_PDA_UI.SetPartOpacity("toast", 0.8);
//   INT_PDA_UI.SetPartAccent("sos", ARGB(255, 255, 60, 60));   INT_PDA_UI.SetPartShown("invite", false);
//   INT_PDA_UI.SaveLayout();                               // the player's file
//   int orange = INT_PDA_UI.StyleColor("accent");          // palette of the PDA style (or INT_PDA_Palette.ORANGE)
//   INT_PDA_UI.OnPlaced().Insert(MyFn);                    // void MyFn(string part)
//
// Version 1 (2026-10-23): parts, overrides, file, edit mode, external hide, preview, FindPart / GetPartRect, palette,
// invokers OnBuilt / OnPlaced / OnStyleApplied / OnEditModeChanged / OnPartShown / OnPartHidden / OnPlayerLayoutChanged.
// Version 2 (2026-10-23): part "faction".

// Palette of the PDA style E «ЗОНА 2.0» (same values as INT_PDA_Look in 5_Mission) + the HUD plates.
class INT_PDA_Palette
{
	static const int BG = 0xFF0B0B0C;
	static const int PANEL = 0xFF131315;
	static const int PANEL2 = 0xFF19191C;
	static const int LINE = 0xFF2A2A2F;
	static const int LINE2 = 0xFF3A3A40;
	static const int TEXT = 0xFFECE8E1;
	static const int DIM = 0xFF8C8982;
	static const int DIM2 = 0xFF5F5D58;
	static const int ORANGE = 0xFFFF7A1A;
	static const int ORANGE2 = 0xFFC85A0A;
	static const int OWN = 0xFFFF9A4D;
	static const int ALLY = 0xFF6FD0A0;
	static const int NEUTRAL = 0xFFB5B2AB;
	static const int ENEMY = 0xFFEC4A3C;
	static const int ADMIN = 0xFF8F7B66;
	static const int ON_ORANGE = 0xFF0B0B0C;
	static const int WHITE = 0xFFFFFFFF;
	// HUD plates over the game (concept 5_e_zone2_hud)
	static const int HUD_PLATE = 0xB809090A;		// rgba(9, 9, 10, .72): feed, news, battery
	static const int TOAST_PLATE = 0xD109090A;		// rgba(9, 9, 10, .82): toasts
	static const int SOS_PLATE = 0xCC280806;		// rgba(40, 8, 6, .80)
	static const int SOS_TEXT = 0xFFFFB3AA;
	static const int BAT_TEXT = 0xFFFF7A6A;
}

// One part's override (one element of INT_PDA_Hud.json). 0 = default in every field.
class INT_PDA_HudPart
{
	static const float SCALE_MIN = 0.6;
	static const float SCALE_MAX = 1.6;
	static const float OPACITY_MIN = 0.2;
	static const float WIDTH_MIN = 160;
	static const float WIDTH_MAX = 1600;

	string Name;
	int Moved;			// 1 = X / Y place the part; 0 = the default place
	float X;			// top-left corner, fraction of the screen width (0..1)
	float Y;			// top-left corner, fraction of the screen height (0..1)
	float W;			// width in design px (1920x1080 canvas) at scale 1; 0 = the part's default width
	float Scale;		// 0 = 1.0; else SCALE_MIN .. SCALE_MAX
	float Opacity;		// 0 = 1.0; else OPACITY_MIN .. 1 (the whole part, text included)
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
		W = 0;
		Scale = 0;
		Opacity = 0;
		Accent = 0;
		Hidden = 0;
	}

	void CopyFrom(INT_PDA_HudPart o)
	{
		if (!o)
			return;
		Moved = o.Moved;
		X = o.X;
		Y = o.Y;
		W = o.W;
		Scale = o.Scale;
		Opacity = o.Opacity;
		Accent = o.Accent;
		Hidden = o.Hidden;
	}

	void Sanitize()
	{
		Moved = INT_PDA_Util.Bit(Moved);
		Hidden = INT_PDA_Util.Bit(Hidden);
		if (X != X || Y != Y)
		{
			Moved = 0;
			X = 0;
			Y = 0;
		}
		X = INT_PDA_Util.ClampF(X, 0, 1, 0);
		Y = INT_PDA_Util.ClampF(Y, 0, 1, 0);
		if (W != W || W <= 0)
			W = 0;
		else
			W = INT_PDA_Util.ClampF(W, WIDTH_MIN, WIDTH_MAX, 0);
		if (Scale != Scale || Scale <= 0)
			Scale = 0;
		else
			Scale = INT_PDA_Util.ClampF(Scale, SCALE_MIN, SCALE_MAX, 1);
		if (Opacity != Opacity || Opacity <= 0)
			Opacity = 0;
		else
			Opacity = INT_PDA_Util.ClampF(Opacity, OPACITY_MIN, 1, 1);
	}
}

// The client file.
class INT_PDA_HudFile
{
	int Version = 1;
	ref array<ref INT_PDA_HudPart> Parts;

	static INT_PDA_HudFile ForRead()
	{
		INT_PDA_HudFile f = new INT_PDA_HudFile();
		f.Parts = new array<ref INT_PDA_HudPart>();
		return f;
	}
}

// What the HUD (5_Mission) implements. Default bodies: the API works (and does nothing) without a HUD.
class INT_PDA_HudApi : Managed
{
	// Rebuild a part from its override ("" = every part).
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

class INT_PDA_UI
{
	static const int GUI_API_VERSION = 2;

	static const string PART_FEED = "feed";
	static const string PART_TOAST = "toast";
	static const string PART_NEWS = "news";
	static const string PART_SOS = "sos";
	static const string PART_BATTERY = "battery";
	static const string PART_INVITE = "invite";
	static const string PART_FACTION = "faction";

	static const string CLIENT_DIR = "$profile:INT_Mods/INT_PDA";
	static const string CLIENT_FILE = "$profile:INT_Mods/INT_PDA/INT_PDA_Hud.json";

	protected static ref map<string, ref INT_PDA_HudPart> s_Parts;
	protected static ref array<string> s_Names;
	protected static INT_PDA_HudApi s_Hud;				// weak: owned by INT_PDA_Hud (5_Mission)
	protected static bool s_Loaded;
	protected static bool s_EditMode;
	protected static bool s_Visible = true;
	protected static ref ScriptInvoker s_OnBuilt;
	protected static ref ScriptInvoker s_OnPlaced;
	protected static ref ScriptInvoker s_OnStyleApplied;
	protected static ref ScriptInvoker s_OnEditModeChanged;
	protected static ref ScriptInvoker s_OnPartShown;
	protected static ref ScriptInvoker s_OnPartHidden;
	protected static ref ScriptInvoker s_OnPlayerLayoutChanged;

	protected static void EnsureLoaded()
	{
		if (s_Loaded)
			return;
		s_Loaded = true;
		s_Names = new array<string>();
		s_Names.Insert(PART_FEED);
		s_Names.Insert(PART_TOAST);
		s_Names.Insert(PART_NEWS);
		s_Names.Insert(PART_SOS);
		s_Names.Insert(PART_BATTERY);
		s_Names.Insert(PART_INVITE);
		s_Names.Insert(PART_FACTION);
		s_Parts = new map<string, ref INT_PDA_HudPart>();
		foreach (string n : s_Names)
		{
			INT_PDA_HudPart p = new INT_PDA_HudPart();
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

	// Allocation-free iteration: for (int i = 0; i < INT_PDA_UI.PartCount(); i++) { string n = INT_PDA_UI.PartName(i); }
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
			outNames.Insert(n);
	}

	static bool IsPart(string name)
	{
		EnsureLoaded();
		return s_Names.Find(name) >= 0;
	}

	// Stringtable key of the part's title (editor lists, edit-mode labels).
	static string PartTitleKey(string name)
	{
		if (name == PART_FEED)
			return "#STR_INT_PDA_HUD_PART_FEED";
		if (name == PART_TOAST)
			return "#STR_INT_PDA_HUD_PART_TOAST";
		if (name == PART_NEWS)
			return "#STR_INT_PDA_HUD_PART_NEWS";
		if (name == PART_SOS)
			return "#STR_INT_PDA_HUD_PART_SOS";
		if (name == PART_BATTERY)
			return "#STR_INT_PDA_HUD_PART_BATTERY";
		if (name == PART_INVITE)
			return "#STR_INT_PDA_HUD_PART_INVITE";
		if (name == PART_FACTION)
			return "#STR_INT_PDA_HUD_PART_FACTION";
		return "";
	}

	// The live override of a part (null for an unknown name). Change it through the Set* functions (they apply it).
	static INT_PDA_HudPart GetPart(string name)
	{
		EnsureLoaded();
		INT_PDA_HudPart p;
		if (s_Parts.Find(name, p))
			return p;
		return null;
	}

	static int DefaultAccent(string name)
	{
		if (name == PART_SOS || name == PART_BATTERY)
			return INT_PDA_Palette.ENEMY;
		return INT_PDA_Palette.ORANGE;
	}

	// Accent of a part: the override or the default.
	static int PartAccent(string name)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (p && p.Accent != 0)
			return p.Accent;
		return DefaultAccent(name);
	}

	static float PartScale(string name)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (!p)
			return 1.0;
		return p.ScaleOr1();
	}

	static float PartOpacity(string name)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (!p)
			return 1.0;
		return p.OpacityOr1();
	}

	static bool IsPartShown(string name)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (!p)
			return false;
		return p.IsShown();
	}

	// --- overrides --------------------------------------------------------------------------------------------------
	// Top-left corner of the part in screen fractions (0..1). Cheap: only moves the part.
	static void SetPartPosition(string name, float nx, float ny)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Moved = 1;
		p.X = INT_PDA_Util.ClampF(nx, 0, 1, 0);
		p.Y = INT_PDA_Util.ClampF(ny, 0, 1, 0);
		if (s_Hud)
			s_Hud.PlacePart(name);
		OnPlaced().Invoke(name);
	}

	static void ResetPartPosition(string name)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Moved = 0;
		p.X = 0;
		p.Y = 0;
		if (s_Hud)
			s_Hud.PlacePart(name);
		OnPlaced().Invoke(name);
	}

	// Width in design px at scale 1 (0 = default). Heights follow the content.
	static void SetPartWidth(string name, float w)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (!p)
			return;
		p.W = w;
		Applied(p);
	}

	static void SetPartScale(string name, float k)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Scale = k;
		Applied(p);
	}

	// 0.2 .. 1 (0 = default 1).
	static void SetPartOpacity(string name, float opacity)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Opacity = opacity;
		Applied(p);
	}

	// ARGB accent (bars, icon squares, captions); 0 = default.
	static void SetPartAccent(string name, int argb)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Accent = argb;
		Applied(p);
	}

	static void SetPartShown(string name, bool shown)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Hidden = 1;
		if (shown)
			p.Hidden = 0;
		Applied(p);
	}

	// Back to the defaults (place, size, colour, opacity, shown).
	static void ResetPart(string name)
	{
		INT_PDA_HudPart p = GetPart(name);
		if (!p)
			return;
		p.Clear();
		Applied(p);
		OnPlaced().Invoke(name);
	}

	static void ResetAll()
	{
		EnsureLoaded();
		foreach (string n : s_Names)
		{
			INT_PDA_HudPart p = GetPart(n);
			if (p)
				p.Clear();
		}
		if (s_Hud)
			s_Hud.ApplyPart("");
		OnStyleApplied().Invoke("");
		OnPlaced().Invoke("");
	}

	protected static void Applied(INT_PDA_HudPart p)
	{
		p.Sanitize();
		if (s_Hud)
			s_Hud.ApplyPart(p.Name);
		OnStyleApplied().Invoke(p.Name);
	}

	// --- file -------------------------------------------------------------------------------------------------------
	static void SaveLayout()
	{
		string txt = ExportLayout();
		if (txt == "")
			return;
		if (!FileExist("$profile:INT_Mods"))
			MakeDirectory("$profile:INT_Mods");
		if (!FileExist(CLIENT_DIR))
			MakeDirectory(CLIENT_DIR);
		FileHandle fh = OpenFile(CLIENT_FILE, FileMode.WRITE);
		if (fh == 0)
		{
			INT_PDA_Log.Warn("не удалось сохранить расположение HUD КПК");
			return;
		}
		FPrint(fh, txt);
		CloseFile(fh);
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

	// The whole layout as JSON (presets of an editor).
	static string ExportLayout()
	{
		EnsureLoaded();
		INT_PDA_HudFile f = INT_PDA_HudFile.ForRead();
		foreach (string n : s_Names)
		{
			INT_PDA_HudPart p = GetPart(n);
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
		if (!FileExist(CLIENT_FILE))
			return;
		FileHandle fh = OpenFile(CLIENT_FILE, FileMode.READ);
		if (fh == 0)
			return;
		string txt;
		ReadFile(fh, txt, 65536);
		CloseFile(fh);
		if (!Merge(txt))
			INT_PDA_Log.Warn("INT_PDA_Hud.json не прочитан, HUD КПК по умолчанию");
	}

	protected static bool Merge(string json)
	{
		INT_PDA_HudFile f = INT_PDA_HudFile.ForRead();
		string err;
		JsonSerializer js = new JsonSerializer();
		if (!js.ReadFromString(f, json, err))
			return false;
		if (!f.Parts)
			return true;
		foreach (INT_PDA_HudPart src : f.Parts)
		{
			if (!src)
				continue;
			INT_PDA_HudPart dst = GetPart(src.Name);
			if (!dst)
				continue;
			dst.CopyFrom(src);
			dst.Sanitize();
		}
		return true;
	}

	// --- modes ------------------------------------------------------------------------------------------------------
	// Edit mode: every part shows sample content without a lifetime, outlined and labelled, also over menus
	// (a hidden part is drawn faint so an editor can switch it back on).
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

	// Sample content of one part for its normal lifetime (no server involved).
	static void Preview(string name)
	{
		if (s_Hud && IsPart(name))
			s_Hud.Preview(name);
	}

	// --- widgets ----------------------------------------------------------------------------------------------------
	// Root frame of a part (exists from the first frame the HUD layer is built; hidden while the part shows nothing).
	static Widget FindPart(string name)
	{
		if (!s_Hud)
			return null;
		return s_Hud.FindPart(name);
	}

	// Screen rect of a part in screen px; false while the part is not on screen.
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

	// The part has real content right now (a line in the feed, a toast on screen…).
	static bool IsPartActive(string name)
	{
		if (!s_Hud)
			return false;
		return s_Hud.IsPartActive(name);
	}

	// --- style ------------------------------------------------------------------------------------------------------
	// Palette of the PDA style by key (StyleKeys lists them); 0 for an unknown key.
	static int StyleColor(string key)
	{
		if (key == "bg")
			return INT_PDA_Palette.BG;
		if (key == "panel")
			return INT_PDA_Palette.PANEL;
		if (key == "panel2")
			return INT_PDA_Palette.PANEL2;
		if (key == "line")
			return INT_PDA_Palette.LINE;
		if (key == "line2")
			return INT_PDA_Palette.LINE2;
		if (key == "text")
			return INT_PDA_Palette.TEXT;
		if (key == "dim")
			return INT_PDA_Palette.DIM;
		if (key == "dim2")
			return INT_PDA_Palette.DIM2;
		if (key == "accent")
			return INT_PDA_Palette.ORANGE;
		if (key == "accent2")
			return INT_PDA_Palette.ORANGE2;
		if (key == "own")
			return INT_PDA_Palette.OWN;
		if (key == "ally")
			return INT_PDA_Palette.ALLY;
		if (key == "neutral")
			return INT_PDA_Palette.NEUTRAL;
		if (key == "enemy")
			return INT_PDA_Palette.ENEMY;
		if (key == "admin")
			return INT_PDA_Palette.ADMIN;
		if (key == "hudPlate")
			return INT_PDA_Palette.HUD_PLATE;
		if (key == "toastPlate")
			return INT_PDA_Palette.TOAST_PLATE;
		if (key == "sosPlate")
			return INT_PDA_Palette.SOS_PLATE;
		if (key == "sosText")
			return INT_PDA_Palette.SOS_TEXT;
		if (key == "batteryText")
			return INT_PDA_Palette.BAT_TEXT;
		return 0;
	}

	static void StyleKeys(array<string> outKeys)
	{
		if (!outKeys)
			return;
		outKeys.Clear();
		outKeys.Insert("bg");
		outKeys.Insert("panel");
		outKeys.Insert("panel2");
		outKeys.Insert("line");
		outKeys.Insert("line2");
		outKeys.Insert("text");
		outKeys.Insert("dim");
		outKeys.Insert("dim2");
		outKeys.Insert("accent");
		outKeys.Insert("accent2");
		outKeys.Insert("own");
		outKeys.Insert("ally");
		outKeys.Insert("neutral");
		outKeys.Insert("enemy");
		outKeys.Insert("admin");
		outKeys.Insert("hudPlate");
		outKeys.Insert("toastPlate");
		outKeys.Insert("sosPlate");
		outKeys.Insert("sosText");
		outKeys.Insert("batteryText");
	}

	// --- events (ScriptInvoker) -----------------------------------------------------------------------------------
	static ScriptInvoker OnBuilt()						// () the HUD layer was (re)built (first frame, resolution change)
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

	static ScriptInvoker OnStyleApplied()				// (string part) size / scale / colour / opacity / shown changed ("" = all)
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

	static ScriptInvoker OnPartShown()					// (string part) a part appeared on screen
	{
		if (!s_OnPartShown)
			s_OnPartShown = new ScriptInvoker();
		return s_OnPartShown;
	}

	static ScriptInvoker OnPartHidden()					// (string part) a part left the screen
	{
		if (!s_OnPartHidden)
			s_OnPartHidden = new ScriptInvoker();
		return s_OnPartHidden;
	}

	static ScriptInvoker OnPlayerLayoutChanged()		// () the layout file was saved
	{
		if (!s_OnPlayerLayoutChanged)
			s_OnPlayerLayoutChanged = new ScriptInvoker();
		return s_OnPlayerLayoutChanged;
	}

	// --- internal: the HUD registers itself -------------------------------------------------------------------------
	static void RegisterHud(INT_PDA_HudApi api)
	{
		s_Hud = api;
	}
}
