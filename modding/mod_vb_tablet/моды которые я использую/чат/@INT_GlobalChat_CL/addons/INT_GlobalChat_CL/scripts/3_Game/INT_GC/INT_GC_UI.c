// INT GlobalChat — public GUI API, part 2: the facade (spec.md §7, readme §6).
// For a future global GUI editor (Customizable HUDs) and any other client mod. Soft dependency: wrap calls in
// #ifdef INT_GLOBALCHAT_CL and check INT_GC_UI.GUI_API_VERSION. Only additions in later versions.
//
//   INT_GC_Style st = INT_GC_UI.Style();   st.AccentColor = ARGB(255, 120, 200, 255);   INT_GC_UI.ApplyStyle();
//   INT_GC_UI.SetRect(0.02, 0.40, 640, 520);   INT_GC_UI.SetEditMode(true);   INT_GC_UI.OnPlaced().Insert(MyFn);
//   Widget w = INT_GC_UI.FindPart("Header");
//
// The widgets live in 5_Mission (INT_GC_View) and register here as an INT_GC_ViewApi.

class INT_GC_ViewApi : Managed
{
	void ApplyStyle();
	void RefreshVisibility();
	void SetEditMode(bool on);
	bool GetScreenRect(out float x, out float y, out float w, out float h)
	{
		return false;
	}
	Widget GetFeedRoot()
	{
		return null;
	}
	Widget GetWindowRoot()
	{
		return null;
	}
	Widget FindPart(string name)
	{
		return null;
	}
	void PushPreview(INT_GC_Line line);
	void OpenWindow();
	void CloseWindow();
	bool IsWindowOpen()
	{
		return false;
	}
	void OpenEditor();
}

class INT_GC_UI
{
	static const int GUI_API_VERSION = 1;

	protected static ref INT_GC_Style s_Style;
	protected static ref INT_GC_Settings s_Settings;
	protected static INT_GC_ViewApi s_View;				// weak: owned by the view singleton
	protected static bool s_Loaded;
	protected static bool s_EditMode;
	protected static bool s_PlayerMoveEnabled = true;
	protected static ref ScriptInvoker s_OnBuilt;
	protected static ref ScriptInvoker s_OnStyleApplied;
	protected static ref ScriptInvoker s_OnPlaced;
	protected static ref ScriptInvoker s_OnOpened;
	protected static ref ScriptInvoker s_OnClosed;
	protected static ref ScriptInvoker s_OnHiddenChanged;
	protected static ref ScriptInvoker s_OnPlayerLayoutChanged;

	protected static void EnsureLoaded()
	{
		if (s_Loaded)
			return;
		s_Loaded = true;
		s_Style = new INT_GC_Style();
		s_Settings = new INT_GC_Settings();
		if (GetGame() && !GetGame().IsDedicatedServer())
			INT_GC_ClientFile.Load(s_Style, s_Settings);
	}

	// --- style ----------------------------------------------------------------------------------------------
	static INT_GC_Style Style()
	{
		EnsureLoaded();
		return s_Style;
	}

	static INT_GC_Style DefaultStyle()
	{
		return new INT_GC_Style();
	}

	static void ApplyStyle()
	{
		Style().Sanitize();
		if (s_View)
			s_View.ApplyStyle();
		OnStyleApplied().Invoke();
	}

	// Back to the default look; the player's place and size are kept.
	static void ResetStyle()
	{
		INT_GC_Style st = Style();
		float x = st.X;
		float y = st.Y;
		float w = st.W;
		float h = st.H;
		st.CopyFrom(DefaultStyle());
		st.X = x;
		st.Y = y;
		st.W = w;
		st.H = h;
		ApplyStyle();
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
		INT_GC_Style st = new INT_GC_Style();
		string err;
		JsonSerializer js = new JsonSerializer();
		if (!js.ReadFromString(st, json, err))
			return false;
		st.Sanitize();
		Style().CopyFrom(st);
		ApplyStyle();
		return true;
	}

	// --- player settings (colour, sounds, eye, channel, block list) -----------------------------------------
	static INT_GC_Settings Settings()
	{
		EnsureLoaded();
		return s_Settings;
	}

	static void SaveClient()
	{
		INT_GC_ClientFile.Save(Style(), Settings());
	}

	static void SetColorIdx(int idx)
	{
		Settings().ColorIdx = INT_GC_Palette.Clamp(idx);
		SaveClient();
	}

	static bool Block(string id, string name)
	{
		bool changed = Settings().Block(id, name);
		if (changed)
			SaveClient();
		return changed;
	}

	static bool Unblock(string id, string name)
	{
		bool changed = Settings().Unblock(id, name);
		if (changed)
			SaveClient();
		return changed;
	}

	static bool IsBlocked(string id, string name)
	{
		return Settings().IsBlocked(id, name);
	}

	// --- place ----------------------------------------------------------------------------------------------
	// Window rect: top-left corner in screen fractions (nx, ny; -1 = default place), size in design px.
	static void SetRect(float nx, float ny, float w, float h)
	{
		INT_GC_Style st = Style();
		st.X = nx;
		st.Y = ny;
		st.W = w;
		st.H = h;
		st.Sanitize();
		if (s_View)
			s_View.ApplyStyle();
		OnPlaced().Invoke();
	}

	static void ResetPosition()
	{
		INT_GC_Style st = Style();
		INT_GC_Style d = DefaultStyle();
		st.X = d.X;
		st.Y = d.Y;
		st.W = d.W;
		st.H = d.H;
		if (s_View)
			s_View.ApplyStyle();
		OnPlaced().Invoke();
	}

	static bool GetScreenRect(out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		if (!s_View)
			return false;
		return s_View.GetScreenRect(x, y, w, h);
	}

	// Built-in drag (header) and resize (corner) of the open window. A GUI editor may switch them off.
	static void SetPlayerMoveEnabled(bool on)
	{
		s_PlayerMoveEnabled = on;
	}

	static bool IsPlayerMoveEnabled()
	{
		return s_PlayerMoveEnabled;
	}

	static void SavePlayerLayout()
	{
		SaveClient();
		OnPlayerLayoutChanged().Invoke();
	}

	// --- visibility -----------------------------------------------------------------------------------------
	// External hide (a cinematic / screenshot mode of another mod). The open window is not affected.
	static void SetVisible(bool visible)
	{
		Style().Visible = visible;
		if (s_View)
			s_View.RefreshVisibility();
	}

	static bool IsVisible()
	{
		return Style().Visible;
	}

	// The player's «eye» (LAlt+J): idle feed hidden; Enter still opens the window.
	static void SetHidden(bool hidden)
	{
		Settings().Hidden = hidden;
		SaveClient();
		if (s_View)
			s_View.RefreshVisibility();
		OnHiddenChanged().Invoke(hidden);
	}

	static bool IsHidden()
	{
		return Settings().Hidden;
	}

	static void ToggleHidden()
	{
		SetHidden(!IsHidden());
	}

	// Edit mode: sample lines stay on screen (no fading, not hidden by menus), a frame marks the window rect.
	static void SetEditMode(bool on)
	{
		s_EditMode = on;
		if (s_View)
			s_View.SetEditMode(on);
	}

	static bool IsEditMode()
	{
		return s_EditMode;
	}

	// --- window -----------------------------------------------------------------------------------------------
	static void Open()
	{
		if (s_View)
			s_View.OpenWindow();
	}

	static void Close()
	{
		if (s_View)
			s_View.CloseWindow();
	}

	static bool IsOpen()
	{
		if (!s_View)
			return false;
		return s_View.IsWindowOpen();
	}

	static void OpenEditor()
	{
		if (s_View)
			s_View.OpenEditor();
	}

	// --- lines ------------------------------------------------------------------------------------------------
	// A system line made on this client (another client mod). color 0 = the system colour of the style.
	static void PushLocalLine(string text, int color = 0)
	{
		INT_GC_Client.PushLocal(text, color, "");
	}

	// Shows a line without the server (editor previews, tests). Not added to the history.
	static void PushPreview(INT_GC_Line line)
	{
		if (s_View && line)
			s_View.PushPreview(line);
	}

	// Every line that reaches the chat (also blocked-free vanilla lines): (INT_GC_Line line)
	static ScriptInvoker OnMessage()
	{
		return INT_GC_Client.OnLine();
	}

	// --- widgets ----------------------------------------------------------------------------------------------
	static Widget GetFeedRoot()
	{
		if (!s_View)
			return null;
		return s_View.GetFeedRoot();
	}

	static Widget GetWindowRoot()
	{
		if (!s_View)
			return null;
		return s_View.GetWindowRoot();
	}

	// Named parts of the open window: "Panel", "Header", "Title", "Channel", "Lines", "Input", "Counter", "Resize",
	// "Hints", "Scroll". null when the window is closed.
	static Widget FindPart(string name)
	{
		if (!s_View)
			return null;
		return s_View.FindPart(name);
	}

	// --- colours of vanilla colour classes ------------------------------------------------------------------
	static int VanillaColor(string colorClass)
	{
		if (colorClass == "colorStatusChannel")
			return ARGB(255, 143, 186, 232);
		if (colorClass == "colorAction")
			return ARGB(255, 232, 200, 138);
		if (colorClass == "colorFriendly")
			return ARGB(255, 140, 214, 128);
		if (colorClass == "colorImportant")
			return ARGB(255, 255, 128, 92);
		return 0;
	}

	// --- events (ScriptInvoker) -----------------------------------------------------------------------------
	static ScriptInvoker OnBuilt()						// () the widgets were (re)built
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

	static ScriptInvoker OnPlaced()						// () the window rect changed
	{
		if (!s_OnPlaced)
			s_OnPlaced = new ScriptInvoker();
		return s_OnPlaced;
	}

	static ScriptInvoker OnOpened()						// () the chat window opened
	{
		if (!s_OnOpened)
			s_OnOpened = new ScriptInvoker();
		return s_OnOpened;
	}

	static ScriptInvoker OnClosed()						// ()
	{
		if (!s_OnClosed)
			s_OnClosed = new ScriptInvoker();
		return s_OnClosed;
	}

	static ScriptInvoker OnHiddenChanged()				// (bool hidden)
	{
		if (!s_OnHiddenChanged)
			s_OnHiddenChanged = new ScriptInvoker();
		return s_OnHiddenChanged;
	}

	static ScriptInvoker OnPlayerLayoutChanged()		// () the player saved a new place / size / look
	{
		if (!s_OnPlayerLayoutChanged)
			s_OnPlayerLayoutChanged = new ScriptInvoker();
		return s_OnPlayerLayoutChanged;
	}

	// --- internal: the view registers itself ----------------------------------------------------------------
	static void RegisterView(INT_GC_ViewApi view)
	{
		s_View = view;
	}
}
