// INT Quests — public GUI API of the HUD tracker (owner decision O9: the API covers ONLY the tracker; spec.md §8).
// For a future global GUI editor and other client mods. Soft dependency: wrap calls in #ifdef INT_QUESTS_CL and check
// INT_QST_UI.GUI_API_VERSION. Only additions in later versions.
//
//   INT_QST_Style st = INT_QST_UI.Style();   st.Scale = 0.9;   INT_QST_UI.ApplyStyle();
//   INT_QST_UI.SetFreePosition(0.75, 0.12);   INT_QST_UI.SetEditMode(true);   INT_QST_UI.OnPlaced().Insert(MyFn);
//   INT_QST_UI.SetOpacity(0.7);   (version 2)
//
// Version 2 (2026-10-22): Style.Opacity, SetOpacity; the player's place, size and opacity come from the journal
// (Настройки → Трекер заданий) and are kept in INT_QUESTS_Hud.json.
// Version 3 (2026-10-22): Style.Look — LOOK_TEXT (default: names, targets and checks as text over the game, no
// background) or LOOK_SHEET (the notebook sheet of v1/v2); TextColor / TextDimColor / TextDoneColor for LOOK_TEXT;
// SetPlayerShown / TogglePlayerShown / IsPlayerShown (the player's show / hide key, saved as Style.Visible).
//
// The tracker itself lives in 5_Mission (INT_QST_Hud) and registers here as an INT_QST_HudApi.

class INT_QST_Style
{
	static const int LOOK_TEXT = 0;		// v3 default: text and icons over the game, no background
	static const int LOOK_SHEET = 1;	// the notebook sheet (paper, leather header, footer)

	int Look = 0;			// v3: LOOK_TEXT
	bool Visible = true;
	// Top-left corner of the tracker as a fraction of the screen (0..1). -1 = default place (top right).
	float FreeX = -1;
	float FreeY = -1;
	float Scale = 1.0;		// 0.6 .. 1.6
	float Opacity = 1.0;		// 0.2 .. 1: the whole tracker, text included (v2)
	float Width = 400;		// design px at scale 1
	int MaxQuests = 5;		// 1 .. INT_QST_Const.TRACK_MAX
	int MaxLines = 3;		// target lines per quest before "+ N"
	bool Compact;			// no branch line and no chips
	bool ShowHeader = true;
	bool ShowFooter = true;
	int PaperAlpha = 242;		// 0..255 of the paper body
	int PaperColor = 0xFFD9C9A3;
	int LeatherColor = 0xFF2E2218;
	int InkColor = 0xFF2A2218;
	int AccentColor = 0xFFA3281E;	// ready bar, stamps, pushpin
	int DoneColor = 0xFF46622A;
	int TextColor = 0xFFF2EBDA;	// LOOK_TEXT: names, open targets, counters (v3)
	int TextDimColor = 0xFFB0AA9C;	// LOOK_TEXT: done targets, «+ ещё N» (v3)
	int TextDoneColor = 0xFF9CCB6B;	// LOOK_TEXT: checks, done counters, a ready quest's name (v3)
	int Sort = 0;			// priority of the top-level frame (kept below 100: 3D previews vanish above it)

	void CopyFrom(INT_QST_Style o)
	{
		if (!o)
			return;
		Look = o.Look;
		Visible = o.Visible;
		FreeX = o.FreeX;
		FreeY = o.FreeY;
		Scale = o.Scale;
		Opacity = o.Opacity;
		Width = o.Width;
		MaxQuests = o.MaxQuests;
		MaxLines = o.MaxLines;
		Compact = o.Compact;
		ShowHeader = o.ShowHeader;
		ShowFooter = o.ShowFooter;
		PaperAlpha = o.PaperAlpha;
		PaperColor = o.PaperColor;
		LeatherColor = o.LeatherColor;
		InkColor = o.InkColor;
		AccentColor = o.AccentColor;
		DoneColor = o.DoneColor;
		TextColor = o.TextColor;
		TextDimColor = o.TextDimColor;
		TextDoneColor = o.TextDoneColor;
		Sort = o.Sort;
	}

	void Sanitize()
	{
		Look = INT_QST_Util.ClampI(Look, LOOK_TEXT, LOOK_SHEET);
		if (Scale != Scale)
			Scale = 1;
		Scale = INT_QST_Util.ClampF(Scale, 0.6, 1.6, 1.0);
		if (Opacity != Opacity)
			Opacity = 1;
		Opacity = INT_QST_Util.ClampF(Opacity, 0.2, 1.0, 1.0);
		Width = INT_QST_Util.ClampF(Width, 280, 640, 400);
		MaxQuests = INT_QST_Util.ClampI(MaxQuests, 1, INT_QST_Const.TRACK_MAX);
		MaxLines = INT_QST_Util.ClampI(MaxLines, 1, 8);
		PaperAlpha = INT_QST_Util.ClampI(PaperAlpha, 0, 255);
		Sort = INT_QST_Util.ClampI(Sort, 0, 99);
		if (FreeX != FreeX || FreeY != FreeY)
		{
			FreeX = -1;
			FreeY = -1;
		}
		if (FreeX > 1)
			FreeX = 1;
		if (FreeY > 1)
			FreeY = 1;
	}
}

// What the player saved in the journal settings / with the mover (client file).
class INT_QST_HudLayout
{
	int Version = 3;
	float FreeX = -1;
	float FreeY = -1;
	float Scale = 1.0;
	float Opacity = 1.0;		// v2 (a v1 file has none: the value is checked on load)
	int Hidden;			// v3: 1 = hidden by the player (show/hide key or the journal switch); absent = shown
}

class INT_QST_HudApi : Managed
{
	void ApplyStyle();
	void SetEditMode(bool on);
	bool GetScreenRect(out float x, out float y, out float w, out float h)
	{
		return false;
	}
	void RefreshVisibility();
	void Refresh();

	// Only FreeX / FreeY changed: move the box without rebuilding it (default: full apply).
	void Reposition()
	{
		ApplyStyle();
	}

	// Only Opacity changed (default: full apply).
	void ApplyOpacity()
	{
		ApplyStyle();
	}
}

class INT_QST_UI
{
	static const int GUI_API_VERSION = 3;
	static const string CLIENT_DIR = "$profile:INT_Mods/INT_QUESTS";
	static const string CLIENT_FILE = "$profile:INT_Mods/INT_QUESTS/INT_QUESTS_Hud.json";

	protected static ref INT_QST_Style s_Style;
	protected static INT_QST_HudApi s_Hud;				// weak: owned by the HUD singleton
	protected static bool s_EditMode;
	protected static bool s_Visible = true;
	protected static bool s_PlayerMoveEnabled = true;
	protected static bool s_MoveRequested;
	protected static ref ScriptInvoker s_OnBuilt;
	protected static ref ScriptInvoker s_OnStyleApplied;
	protected static ref ScriptInvoker s_OnPlaced;
	protected static ref ScriptInvoker s_OnTrackerChanged;
	protected static ref ScriptInvoker s_OnPlayerLayoutChanged;

	// --- style ----------------------------------------------------------------------------------------------
	static INT_QST_Style Style()
	{
		if (!s_Style)
		{
			s_Style = new INT_QST_Style();
			LoadPlayerLayout();
		}
		return s_Style;
	}

	static INT_QST_Style DefaultStyle()
	{
		return new INT_QST_Style();
	}

	static void ApplyStyle()
	{
		Style().Sanitize();
		if (s_Hud)
			s_Hud.ApplyStyle();
		OnStyleApplied().Invoke();
	}

	// Back to the default look; the player's place, scale and opacity are kept.
	static void ResetStyle()
	{
		INT_QST_Style st = Style();
		float fx = st.FreeX;
		float fy = st.FreeY;
		float sc = st.Scale;
		float op = st.Opacity;
		st.CopyFrom(DefaultStyle());
		st.FreeX = fx;
		st.FreeY = fy;
		st.Scale = sc;
		st.Opacity = op;
		ApplyStyle();
	}

	// Whole-tracker opacity 0.2 .. 1 (text included).
	static void SetOpacity(float opacity)
	{
		INT_QST_Style st = Style();
		st.Opacity = Math.Clamp(opacity, 0.2, 1.0);
		if (s_Hud)
			s_Hud.ApplyOpacity();
		OnStyleApplied().Invoke();
	}

	static string SaveStyle()
	{
		string txt;
		JsonSerializer js = new JsonSerializer();
		js.WriteToString(Style(), false, txt);
		return txt;
	}

	static bool LoadStyle(string json)
	{
		INT_QST_Style st = new INT_QST_Style();
		string err;
		JsonSerializer js = new JsonSerializer();
		if (!js.ReadFromString(st, json, err))
			return false;
		st.Sanitize();
		Style().CopyFrom(st);
		ApplyStyle();
		return true;
	}

	// --- place ----------------------------------------------------------------------------------------------
	// Top-left corner of the tracker in screen fractions. Cheap: may be called every frame while dragging (the box
	// is only moved, its content is not rebuilt).
	static void SetFreePosition(float nx, float ny)
	{
		INT_QST_Style st = Style();
		st.FreeX = Math.Clamp(nx, 0, 1);
		st.FreeY = Math.Clamp(ny, 0, 1);
		if (s_Hud)
			s_Hud.Reposition();
		OnPlaced().Invoke();
	}

	static void ResetPosition()
	{
		INT_QST_Style st = Style();
		st.FreeX = -1;
		st.FreeY = -1;
		if (s_Hud)
			s_Hud.Reposition();
		OnPlaced().Invoke();
	}

	static bool GetScreenRect(out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		if (!s_Hud)
			return false;
		return s_Hud.GetScreenRect(x, y, w, h);
	}

	// --- modes ----------------------------------------------------------------------------------------------
	// Edit mode: sample quests when nothing is tracked, shown over menus, a frame marks the box.
	static void SetEditMode(bool on)
	{
		s_EditMode = on;
		if (s_Hud)
			s_Hud.SetEditMode(on);
	}

	static bool IsEditMode()
	{
		return s_EditMode;
	}

	// External hide (cinematic / screenshot mode of another mod).
	static void SetVisible(bool visible)
	{
		s_Visible = visible;
		if (s_Hud)
			s_Hud.RefreshVisibility();
	}

	static bool IsVisible()
	{
		return s_Visible && Style().Visible;
	}

	// Rebuild the tracker now (another mod changed something it shows).
	static void Refresh()
	{
		if (s_Hud)
			s_Hud.Refresh();
	}

	// --- player move ----------------------------------------------------------------------------------------
	static void SetPlayerMoveEnabled(bool on)
	{
		s_PlayerMoveEnabled = on;
	}

	static bool IsPlayerMoveEnabled()
	{
		return s_PlayerMoveEnabled;
	}

	// Asks the mission to open the move mode on the next frame (it waits until no menu is open).
	static void StartPlayerMove()
	{
		if (s_PlayerMoveEnabled)
			s_MoveRequested = true;
	}

	static bool TakeMoveRequest()
	{
		bool r = s_MoveRequested;
		s_MoveRequested = false;
		return r;
	}

	static void SavePlayerLayout()
	{
		INT_QST_Style st = Style();
		INT_QST_HudLayout lay = new INT_QST_HudLayout();
		lay.FreeX = st.FreeX;
		lay.FreeY = st.FreeY;
		lay.Scale = st.Scale;
		lay.Opacity = st.Opacity;
		lay.Hidden = 0;
		if (!st.Visible)
			lay.Hidden = 1;
		string txt;
		JsonSerializer js = new JsonSerializer();
		if (!js.WriteToString(lay, true, txt))
			return;
		if (!FileExist("$profile:INT_Mods"))
			MakeDirectory("$profile:INT_Mods");
		if (!FileExist(CLIENT_DIR))
			MakeDirectory(CLIENT_DIR);
		FileHandle fh = OpenFile(CLIENT_FILE, FileMode.WRITE);
		if (fh == 0)
		{
			INT_QST_Log.Warn("не удалось сохранить положение трекера");
			return;
		}
		FPrint(fh, txt);
		CloseFile(fh);
		OnPlayerLayoutChanged().Invoke();
	}

	static void LoadPlayerLayout()
	{
		if (!s_Style)
			s_Style = new INT_QST_Style();
		if (!FileExist(CLIENT_FILE))
			return;
		FileHandle fh = OpenFile(CLIENT_FILE, FileMode.READ);
		if (fh == 0)
			return;
		string txt;
		ReadFile(fh, txt, 65536);
		CloseFile(fh);
		INT_QST_HudLayout lay = new INT_QST_HudLayout();
		string err;
		JsonSerializer js = new JsonSerializer();
		if (!js.ReadFromString(lay, txt, err))
			return;
		if (lay.FreeX >= 0 && lay.FreeX <= 1 && lay.FreeY >= 0 && lay.FreeY <= 1)
		{
			s_Style.FreeX = lay.FreeX;
			s_Style.FreeY = lay.FreeY;
		}
		if (lay.Scale >= 0.6 && lay.Scale <= 1.6)
			s_Style.Scale = lay.Scale;
		if (lay.Opacity >= 0.2 && lay.Opacity <= 1.0)
			s_Style.Opacity = lay.Opacity;
		s_Style.Visible = lay.Hidden != 1;
	}

	// The player's show / hide (key UAINTQuestsTracker, journal switch); saved at once.
	static void SetPlayerShown(bool shown)
	{
		Style().Visible = shown;
		if (s_Hud)
			s_Hud.RefreshVisibility();
		SavePlayerLayout();
	}

	static void TogglePlayerShown()
	{
		bool shown = Style().Visible;
		SetPlayerShown(!shown);
	}

	static bool IsPlayerShown()
	{
		return Style().Visible;
	}

	// The player's place, size and opacity back to the defaults (saved).
	static void ResetPlayerLayout()
	{
		INT_QST_Style st = Style();
		st.Scale = 1.0;
		st.Opacity = 1.0;
		st.Visible = true;
		ResetPosition();
		ApplyStyle();
		SavePlayerLayout();
	}

	// --- events (ScriptInvoker) -----------------------------------------------------------------------------
	static ScriptInvoker OnBuilt()						// () the tracker layer was (re)built
	{
		if (!s_OnBuilt)
			s_OnBuilt = new ScriptInvoker();
		return s_OnBuilt;
	}

	static ScriptInvoker OnStyleApplied()				// ()
	{
		if (!s_OnStyleApplied)
			s_OnStyleApplied = new ScriptInvoker();
		return s_OnStyleApplied;
	}

	static ScriptInvoker OnPlaced()						// () the tracker moved
	{
		if (!s_OnPlaced)
			s_OnPlaced = new ScriptInvoker();
		return s_OnPlaced;
	}

	static ScriptInvoker OnTrackerChanged()				// (int shownQuests) the content was rebuilt
	{
		if (!s_OnTrackerChanged)
			s_OnTrackerChanged = new ScriptInvoker();
		return s_OnTrackerChanged;
	}

	static ScriptInvoker OnPlayerLayoutChanged()		// () the player saved a new place
	{
		if (!s_OnPlayerLayoutChanged)
			s_OnPlayerLayoutChanged = new ScriptInvoker();
		return s_OnPlayerLayoutChanged;
	}

	// --- internal: the HUD registers itself ----------------------------------------------------------------
	static void RegisterHud(INT_QST_HudApi hud)
	{
		s_Hud = hud;
	}
}
