// INT GlobalChat — the client view (spec.md §5.1): a top-level HUD frame (sort 0) with the idle feed, the editor
// preview window and the bridge to the chat / editor menus. Registers behind the public API as INT_GC_ViewApi.
// Tick runs every frame from the mission but returns at once while there is nothing on screen (owner rule:
// per-frame client checks only while lines are shown, like INT_KillFeed).

class INT_GC_View : INT_GC_ViewApi
{
	static const float FADE_OUT = 1.2;

	protected static ref INT_GC_View s_Instance;

	protected Widget m_Root;
	protected int m_ScreenW;
	protected int m_ScreenH;
	protected ref INT_GC_Area m_Feed;
	protected ref array<ref INT_GC_Line> m_FeedLines;
	protected ref INT_GC_Window m_Preview;
	protected ref array<ref INT_GC_Line> m_PreviewLines;
	protected bool m_Edit;
	protected bool m_Hidden;
	protected bool m_Hooked;

	static INT_GC_View Get()
	{
		if (!s_Instance)
		{
			s_Instance = new INT_GC_View();
			INT_GC_UI.RegisterView(s_Instance);
		}
		return s_Instance;
	}

	static INT_GC_View Peek()
	{
		return s_Instance;
	}

	static void Shutdown()
	{
		if (!s_Instance)
			return;
		s_Instance.Teardown();
		INT_GC_UI.RegisterView(null);
		s_Instance = null;
	}

	void INT_GC_View()
	{
		m_FeedLines = new array<ref INT_GC_Line>();
		m_PreviewLines = new array<ref INT_GC_Line>();
		INT_GC_Client.OnLine().Insert(OnLine);
		m_Hooked = true;
	}

	void ~INT_GC_View()
	{
		Teardown();
	}

	protected void Teardown()
	{
		if (m_Hooked)
		{
			INT_GC_Client.OnLine().Remove(OnLine);
			m_Hooked = false;
		}
		INT_GC_Measure.Detach();
		if (m_Preview)
			m_Preview.Destroy();
		m_Preview = null;
		if (m_Feed)
			m_Feed.Destroy();
		m_Feed = null;
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
	}

	// --- root / feed ------------------------------------------------------------------------------------------
	protected void EnsureRoot()
	{
		int sw = INT_UIF_Scale.ScreenW();
		int sh = INT_UIF_Scale.ScreenH();
		if (m_Root && sw == m_ScreenW && sh == m_ScreenH)
			return;
		bool rebuild = m_Root != null;
		if (m_Preview)
			m_Preview.Destroy();
		m_Preview = null;
		if (m_Feed)
			m_Feed.Destroy();
		m_Feed = null;
		if (m_Root)
			m_Root.Unlink();
		m_ScreenW = sw;
		m_ScreenH = sh;
		INT_GC_Look.EnsureLoaded();
		m_Root = INT_UIF_Draw.Frame(null, 0, 0, 10, 10, "INT_GC_HudRoot");
		if (!m_Root)
			return;
		m_Root.SetPos(0, 0);
		m_Root.SetSize(sw, sh);
		int sort = INT_GC_UI.Style().Sort;
		if (sort > 0)
			m_Root.SetSort(sort);
		INT_GC_Measure.Attach(m_Root);
		BuildFeed();
		if (m_Edit)
			BuildPreview();
		if (rebuild)
			INT_GC_Log.Dbg("слой чата пересоздан: " + sw.ToString() + "x" + sh.ToString());
		INT_GC_UI.OnBuilt().Invoke();
	}

	protected void BuildFeed()
	{
		if (m_Feed)
			m_Feed.Destroy();
		m_Feed = new INT_GC_Area();
		float sx;
		float sy;
		INT_GC_Window.ScreenPos(sx, sy);
		float lx;
		float ly;
		float lw;
		float lh;
		INT_GC_Style st = INT_GC_UI.Style();
		INT_GC_Window.LinesRect(st.W, st.H, lx, ly, lw, lh);
		float f = INT_UIF_Scale.Factor();
		float dx = sx / f + lx;
		float dy = sy / f + ly;
		m_Feed.Create(m_Root, dx, dy, lw, lh, true, null);
		m_Feed.SetMetrics(st.FontSize, st.ShowTime);
		m_Feed.SetMaxRows(st.IdleLines);
		m_Feed.SetSource(m_FeedLines);
		m_Feed.Refresh();
		bool hide = ShouldHideFeed();
		ApplyHide(hide);
	}

	// --- per frame ------------------------------------------------------------------------------------------
	void Tick(float dt)
	{
		if (!GetGame())
			return;
		bool busy = m_FeedLines.Count() > 0 || m_Edit;
		if (!busy && m_Root && m_ScreenW == INT_UIF_Scale.ScreenW())
			return;
		EnsureRoot();
		if (!m_Root)
			return;
		bool hide = ShouldHideFeed();
		if (hide != m_Hidden)
			ApplyHide(hide);
		if (m_FeedLines.Count() > 0)
			Fade();
	}

	protected void ApplyHide(bool hide)
	{
		m_Hidden = hide;
		if (m_Feed && m_Feed.GetRoot())
			m_Feed.GetRoot().Show(!hide);
	}

	protected bool ShouldHideFeed()
	{
		if (m_Edit)
			return true;
		if (!INT_GC_UI.IsVisible() || INT_GC_UI.IsHidden())
			return true;
		UIManager um = GetGame().GetUIManager();
		if (um && um.GetMenu())
			return true;
		Mission mission = GetGame().GetMission();
		if (mission)
		{
			IngameHud hud = IngameHud.Cast(mission.GetHud());
			if (hud && hud.GetHudVisibility() && hud.GetHudVisibility().IsContextFlagActive(EHudContextFlags.HUD_HIDE))
				return true;
		}
		return false;
	}

	protected void Fade()
	{
		int fadeSec = INT_GC_UI.Style().FadeSec;
		if (fadeSec <= 0)
			return;
		float now = GetGame().GetTickTime();
		bool changed = false;
		for (int i = m_FeedLines.Count() - 1; i >= 0; i--)
		{
			INT_GC_Line l = m_FeedLines.Get(i);
			float age = now - l.Born;
			if (age >= fadeSec + FADE_OUT)
			{
				m_FeedLines.RemoveOrdered(i);
				changed = true;
				continue;
			}
			if (age > fadeSec && m_Feed)
			{
				Widget fw = m_Feed.FrameOf(l.Seq);
				if (fw)
				{
					float a = 1.0 - (age - fadeSec) / FADE_OUT;
					fw.SetAlpha(Math.Clamp(a, 0, 1));
				}
			}
		}
		if (changed && m_Feed)
			m_Feed.Refresh();
	}

	// --- incoming lines -----------------------------------------------------------------------------------------
	void OnLine(INT_GC_Line line)
	{
		if (!line)
			return;
		PlaySoundFor(line);
		INT_GC_ChatMenu menu = INT_GC_ChatMenu.Instance();
		if (menu)
			menu.OnLineAdded(line);
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		m_FeedLines.Insert(line);
		while (m_FeedLines.Count() > 40)
			m_FeedLines.RemoveOrdered(0);
		EnsureRoot();
		if (m_Feed)
			m_Feed.OnAppended(line);
	}

	protected void PlaySoundFor(INT_GC_Line line)
	{
		INT_GC_Settings se = INT_GC_UI.Settings();
		if (line.Kind == INT_GC_Line.KIND_NOTICE)
		{
			if (line.Level >= 1)
				INT_GC_Sfx.Play(INT_GC_Sfx.FAIL);
			return;
		}
		if (line.Mine)
			return;
		bool talk = INT_GC_LineLayout.IsTalk(line.Kind);
		if ((line.Mention || line.Kind == INT_GC_Line.KIND_PM_IN) && se.MentionSound)
		{
			INT_GC_Sfx.Play(INT_GC_Sfx.MENTION);
			return;
		}
		if (talk && se.Sound)
			INT_GC_Sfx.Play(INT_GC_Sfx.MESSAGE);
	}

	// Rebuild after a block: history lines were removed.
	void OnHistoryChanged()
	{
		for (int i = m_FeedLines.Count() - 1; i >= 0; i--)
		{
			INT_GC_Line l = m_FeedLines.Get(i);
			if (INT_GC_Client.History().Find(l) < 0)
				m_FeedLines.RemoveOrdered(i);
		}
		if (m_Feed)
			m_Feed.Refresh();
		if (m_Preview && m_Preview.m_Area)
			m_Preview.m_Area.Refresh();
	}

	// --- editor preview ---------------------------------------------------------------------------------------
	protected void BuildPreviewLines()
	{
		m_PreviewLines.Clear();
		string me = INT_GC_Client.MyName();
		if (me == "")
			me = "Survivor";
		AddPreview(INT_GC_Line.KIND_GLOBAL, "Arcane", 18, false, "#STR_INT_GC_SAMPLE_1");
		AddPreview(INT_GC_Line.KIND_GLOBAL, "Ded_Mazay", 6, true, "#STR_INT_GC_SAMPLE_2");
		AddPreview(INT_GC_Line.KIND_MOD, "", -1, false, "");
		AddPreview(INT_GC_Line.KIND_LOCAL, "Kuzya", 9, false, "#STR_INT_GC_SAMPLE_3");
		AddPreview(INT_GC_Line.KIND_SYSTEM, "", -1, false, "#STR_INT_GC_SAMPLE_4");
		INT_GC_Line mine = AddPreview(INT_GC_Line.KIND_GLOBAL, me, INT_GC_UI.Settings().ColorIdx, false, "#STR_INT_GC_SAMPLE_5");
		if (mine)
			mine.Mine = true;
		AddPreview(INT_GC_Line.KIND_PM_IN, "Critone", 25, false, "#STR_INT_GC_SAMPLE_6");
	}

	protected INT_GC_Line AddPreview(int kind, string name, int colorIdx, bool admin, string textKey)
	{
		INT_GC_Line l = new INT_GC_Line();
		l.Kind = kind;
		l.Name = name;
		l.ColorIdx = colorIdx;
		l.Admin = admin;
		if (textKey != "")
			l.Text = Widget.TranslateString(textKey);
		if (kind == INT_GC_Line.KIND_MOD)
			l.Text = INT_GC_Client.Fmt1("#STR_INT_GC_MOD_VIOLATION", "Bober");
		if (kind == INT_GC_Line.KIND_SYSTEM)
			l.Tag = "#STR_INT_GC_TAG_SERVER";
		if (kind == INT_GC_Line.KIND_LOCAL)
			l.Tag = "#STR_INT_GC_TAG_LOCAL";
		int h;
		int m;
		INT_GC_Client.ServerTime(h, m);
		l.Hour = h;
		l.Minute = m;
		l.Seq = -1000 - m_PreviewLines.Count();
		m_PreviewLines.Insert(l);
		return l;
	}

	protected void BuildPreview()
	{
		if (m_Preview)
			m_Preview.Destroy();
		if (!m_Root)
			return;
		BuildPreviewLines();
		m_Preview = new INT_GC_Window();
		m_Preview.Build(m_Root, false, null);
		INT_GC_Style st = INT_GC_UI.Style();
		if (m_Preview.m_Area)
		{
			m_Preview.m_Area.SetMetrics(st.FontSize, st.ShowTime);
			m_Preview.m_Area.SetSource(m_PreviewLines);
			m_Preview.m_Area.Refresh();
		}
		m_Preview.UpdateScroll();
	}

	INT_GC_Window GetPreview()
	{
		return m_Preview;
	}

	// --- INT_GC_ViewApi -----------------------------------------------------------------------------------------
	override void ApplyStyle()
	{
		EnsureRoot();
		if (!m_Root)
			return;
		INT_GC_Measure.ClearCache();
		int sort = INT_GC_UI.Style().Sort;
		m_Root.SetSort(sort);
		BuildFeed();
		if (m_Edit)
			BuildPreview();
		INT_GC_ChatMenu menu = INT_GC_ChatMenu.Instance();
		if (menu)
			menu.Rebuild();
	}

	override void RefreshVisibility()
	{
		bool hide = ShouldHideFeed();
		ApplyHide(hide);
		INT_GC_ChatMenu menu = INT_GC_ChatMenu.Instance();
		if (menu)
			menu.PaintEye();
	}

	override void SetEditMode(bool on)
	{
		m_Edit = on;
		EnsureRoot();
		if (on)
		{
			BuildPreview();
		}
		else
		{
			if (m_Preview)
				m_Preview.Destroy();
			m_Preview = null;
		}
		bool hide = ShouldHideFeed();
		ApplyHide(hide);
	}

	override bool GetScreenRect(out float x, out float y, out float w, out float h)
	{
		INT_GC_Window.ScreenPos(x, y);
		INT_GC_Style st = INT_GC_UI.Style();
		w = INT_UIF_Draw.S(st.W);
		h = INT_UIF_Draw.S(st.H);
		return true;
	}

	override Widget GetFeedRoot()
	{
		if (!m_Feed)
			return null;
		return m_Feed.GetRoot();
	}

	override Widget GetWindowRoot()
	{
		INT_GC_ChatMenu menu = INT_GC_ChatMenu.Instance();
		if (menu && menu.GetWindow())
			return menu.GetWindow().m_Root;
		if (m_Preview)
			return m_Preview.m_Root;
		return null;
	}

	override Widget FindPart(string name)
	{
		INT_GC_ChatMenu menu = INT_GC_ChatMenu.Instance();
		if (menu && menu.GetWindow())
			return menu.GetWindow().FindPart(name);
		if (m_Preview)
			return m_Preview.FindPart(name);
		return null;
	}

	override void PushPreview(INT_GC_Line line)
	{
		if (!line)
			return;
		if (line.Seq == 0)
			line.Seq = -5000 - m_PreviewLines.Count();
		m_PreviewLines.Insert(line);
		if (m_Preview && m_Preview.m_Area)
			m_Preview.m_Area.Refresh();
	}

	override void OpenWindow()
	{
		INT_UIF_Menu.Open(INT_GC_Const.MENU_CHAT);
	}

	override void CloseWindow()
	{
		INT_GC_ChatMenu menu = INT_GC_ChatMenu.Instance();
		if (menu)
			menu.RequestClose();
	}

	override bool IsWindowOpen()
	{
		return INT_GC_ChatMenu.Instance() != null;
	}

	override void OpenEditor()
	{
		INT_UIF_Menu.Open(INT_GC_Const.MENU_EDITOR);
	}

	// self-test
	int FeedCount()
	{
		return m_FeedLines.Count();
	}

	INT_GC_Area GetFeed()
	{
		return m_Feed;
	}

	void TestClearFeed()
	{
		m_FeedLines.Clear();
		if (m_Feed)
			m_Feed.Refresh();
	}
}
