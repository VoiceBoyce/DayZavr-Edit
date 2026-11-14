// INT Quests (client) — the HUD tracker (design B: a notebook sheet under a leather strip, 2_b_diary_hud.png). A
// top-level frame (sort 0: at 100/200 3D previews vanish, KillFeed measurement), no focus, ignores the mouse.
// Built in Tick, never inside an RPC handler. Real work only when something changed: the client's change event,
// the style, the screen size, or — once a second, only while tracked quests have inventory targets or timers — a
// cheap signature of what the rows show. Hidden while a menu is open, the HUD is hidden or nothing is tracked.

class INT_QST_Hud : INT_QST_HudApi
{
	static const float DEFAULT_RIGHT = 24;
	static const float DEFAULT_TOP = 112;
	static const float PREVIEW_PAD = 24;

	protected static ref INT_QST_Hud s_Instance;
	static int s_Builds;			// self-test: how many times the content was built

	protected Widget m_Root;
	protected Widget m_Box;
	protected Widget m_Paint;		// everything drawn (Style.Opacity is applied to it; the edit frame stays opaque)
	protected Widget m_EditFrame;
	protected float m_Op = 1;		// Style.Opacity of the current build (text shadows follow it)
	protected bool m_Preview;		// INT_QST_HudPreview: drawn into a page of the journal settings
	protected int m_PreviewMax = 2;
	protected int m_ScreenW;
	protected int m_ScreenH;
	protected bool m_Dirty = true;
	protected bool m_DataDirty;		// client data changed: rebuilt at most every DATA_GAP (kill storms)
	protected float m_SinceBuild = 100;
	static const float DATA_GAP = 0.25;
	protected bool m_Hidden;
	protected bool m_Edit;
	protected bool m_Hooked;
	protected float m_K = 1;
	protected float m_BoxW;
	protected float m_BoxH;
	protected float m_LiveAcc;
	protected string m_Sig;
	protected bool m_NeedsLive;		// a shown quest has inventory targets or a timer
	protected int m_Shown;
	protected float m_Offset;		// toast offset (screen px)
	protected float m_BaseY;

	static INT_QST_Hud Get()
	{
		if (!s_Instance)
		{
			s_Instance = new INT_QST_Hud();
			INT_QST_UI.RegisterHud(s_Instance);
		}
		return s_Instance;
	}

	static INT_QST_Hud Peek()
	{
		return s_Instance;
	}

	static void Shutdown()
	{
		if (s_Instance)
			s_Instance.Destroy();
		INT_QST_UI.RegisterHud(null);
		s_Instance = null;
	}

	protected void Destroy()
	{
		if (m_Hooked && INT_QST_Client.s_OnChanged)
			INT_QST_Client.s_OnChanged.Remove(OnDataChanged);
		m_Hooked = false;
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
		m_Box = null;
		m_Paint = null;
		m_EditFrame = null;
	}

	void OnDataChanged()
	{
		m_DataDirty = true;
	}

	// --- api ------------------------------------------------------------------------------------------------
	override void ApplyStyle()
	{
		m_Dirty = true;
		if (m_Root)
		{
			INT_QST_Style st = INT_QST_UI.Style();
			m_Root.SetSort(st.Sort);
		}
	}

	// Only the opacity changed: rebuilt (a parent's alpha does not reach the children of framework widgets —
	// measured 2026-10-22 — so every widget gets its own alpha at build time, as INT_CIH does).
	override void ApplyOpacity()
	{
		m_Dirty = true;
	}

	// Multiplies the colour alpha of every widget under `root` by k (each widget's own alpha, set by Build).
	static void FadeTree(Widget root, float k)
	{
		if (!root)
			return;
		Widget c = root.GetChildren();
		while (c)
		{
			float a = c.GetAlpha();
			c.SetAlpha(a * k);
			FadeTree(c, k);
			c = c.GetSibling();
		}
	}

	// Only the place changed (SetFreePosition while dragging): the box moves, nothing is rebuilt.
	override void Reposition()
	{
		if (m_Box && !m_Dirty)
			PlaceBox();
		else
			m_Dirty = true;
	}

	override void SetEditMode(bool on)
	{
		m_Edit = on;
		m_Dirty = true;
	}

	override bool GetScreenRect(out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		if (!m_Box || m_Hidden)
			return false;
		m_Box.GetScreenPos(x, y);
		m_Box.GetScreenSize(w, h);
		return true;
	}

	override void RefreshVisibility()
	{
		m_Dirty = true;
	}

	override void Refresh()
	{
		m_Dirty = true;
	}

	// --- frame ----------------------------------------------------------------------------------------------
	void Tick(float dt)
	{
		if (!m_Hooked)
		{
			INT_QST_Client.s_OnChanged.Insert(OnDataChanged);
			m_Hooked = true;
		}
		int sw = INT_UIF_Scale.ScreenW();
		int sh = INT_UIF_Scale.ScreenH();
		if (!m_Root || sw != m_ScreenW || sh != m_ScreenH)
		{
			if (m_Root)
				m_Root.Unlink();
			m_ScreenW = sw;
			m_ScreenH = sh;
			m_Root = INT_UIF_Draw.Frame(null, 0, 0, 10, 10, "INT_QST_HudRoot");
			if (!m_Root)
				return;
			m_Root.SetPos(0, 0);
			m_Root.SetSize(sw, sh);
			INT_QST_Style st0 = INT_QST_UI.Style();
			if (st0.Sort > 0)
				m_Root.SetSort(st0.Sort);
			m_Box = null;
			m_Dirty = true;
			INT_QST_UI.OnBuilt().Invoke();
		}
		bool hide = ShouldHide();
		if (hide != m_Hidden)
		{
			m_Hidden = hide;
			m_Root.Show(!hide);
		}
		m_SinceBuild = m_SinceBuild + dt;
		if (hide)
			return;
		if (m_DataDirty && m_SinceBuild >= DATA_GAP)
		{
			m_DataDirty = false;
			m_Dirty = true;
		}
		if (!m_Dirty && m_NeedsLive)
		{
			m_LiveAcc = m_LiveAcc + dt;
			if (m_LiveAcc >= 1.0)
			{
				m_LiveAcc = 0;
				if (Signature() != m_Sig)
					m_Dirty = true;
			}
		}
		if (m_Dirty)
			Build();
		UpdateToastOffset();
	}

	protected bool ShouldHide()
	{
		if (m_Edit)
			return false;		// the mover shows it even when the player hid it (nothing to drag otherwise)
		if (!INT_QST_UI.IsVisible())
			return true;
		if (!GetGame())
			return true;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || !p.IsAlive())
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
		if (!INT_QST_Client.s_CatalogReady || !INT_QST_Client.s_Server)
			return true;
		return INT_QST_Client.s_Server.Tracked.Count() == 0;
	}

	// The quests shown, in tracking order (still active, known to the catalog).
	protected void ShownQuests(array<INT_QST_QuestDef> outList)
	{
		outList.Clear();
		INT_QST_Style st = INT_QST_UI.Style();
		int most = st.MaxQuests;
		if (m_Preview && most > m_PreviewMax)
			most = m_PreviewMax;
		if (INT_QST_Client.s_Server)
		{
			foreach (int id : INT_QST_Client.s_Server.Tracked)
			{
				if (outList.Count() >= most)
					break;
				INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(id);
				if (q && INT_QST_Client.s_State.FindActive(id))
					outList.Insert(q);
			}
		}
		if (outList.Count() == 0 && (m_Edit || m_Preview))
		{
			// edit mode / preview with nothing tracked: the first quests of the catalog as samples
			foreach (INT_QST_QuestDef s : INT_QST_Client.s_Catalog.m_List)
			{
				if (outList.Count() >= 2 || outList.Count() >= most)
					break;
				outList.Insert(s);
			}
		}
	}

	// What the rows show and can change without a server message.
	protected string Signature()
	{
		array<INT_QST_QuestDef> list = new array<INT_QST_QuestDef>();
		ShownQuests(list);
		string sig = "";
		foreach (INT_QST_QuestDef q : list)
		{
			int wait;
			int st = INT_QST_Client.StatusOf(q, wait);
			int minutes = wait / 60;
			sig = sig + q.Id.ToString() + ":" + st.ToString() + ":" + minutes.ToString() + ";";
			int n = q.Targets.Count();
			for (int i = 0; i < n; i++)
			{
				INT_QST_TargetDef t = q.Targets[i];
				if (INT_QST_TT.IsInventory(t.m_Type))
					sig = sig + INT_QST_Client.TargetValue(q, i).ToString() + ",";
			}
		}
		return sig;
	}

	// --- build ----------------------------------------------------------------------------------------------
	protected void Build()
	{
		m_Dirty = false;
		m_DataDirty = false;
		m_SinceBuild = 0;
		m_LiveAcc = 0;
		if (!m_Root)
			return;
		if (m_Box)
			m_Box.Unlink();
		m_Box = null;
		m_Paint = null;
		m_EditFrame = null;
		INT_QST_Style st = INT_QST_UI.Style();
		m_K = st.Scale;
		array<INT_QST_QuestDef> list = new array<INT_QST_QuestDef>();
		ShownQuests(list);
		m_Shown = list.Count();
		m_Sig = Signature();
		m_NeedsLive = false;
		if (list.Count() == 0)
			return;
		if (!m_Preview)
			s_Builds++;
		float w = st.Width;
		m_BoxW = w;
		m_Op = st.Opacity;
		m_Box = INT_UIF_Draw.Frame(m_Root, 0, 0, w * m_K, 100, "INT_QST_Tracker");
		m_Paint = INT_UIF_Draw.Frame(m_Box, 0, 0, w * m_K, 100, "Paint");
		float y;
		if (st.Look == INT_QST_Style.LOOK_SHEET)
			y = BuildSheet(st, w, list);
		else
			y = BuildText(st, w, list);
		m_BoxH = y;
		INT_UIF_Draw.Resize(m_Box, w * m_K, y * m_K);
		INT_UIF_Draw.Resize(m_Paint, w * m_K, y * m_K);
		if (st.Opacity < 0.999)
			FadeTree(m_Paint, st.Opacity);
		if (m_Edit)
		{
			m_EditFrame = INT_UIF_Draw.Frame(m_Box, (-6) * m_K, (-6) * m_K, (w + 12) * m_K, (y + 12) * m_K, "Edit");
			INT_UIF_Surface.Box(m_EditFrame, 0, 0, (w + 12) * m_K, (y + 12) * m_K, 10, 0, 0xFFE9D68C);
		}
		if (m_Preview)
		{
			INT_UIF_Draw.Move(m_Box, PREVIEW_PAD, PREVIEW_PAD);
			return;
		}
		Place();
		INT_QST_UI.OnTrackerChanged().Invoke(list.Count());
	}

	// --- look 0 (default): text only over the game, no background (owner 2026-10-22: «плашка мешает играть»).
	// Per quest: its name (a green check when every target is done), then the target lines — box or check, text
	// (done: dimmed and struck through), «have / need» on the right; «+ ещё N» past MaxLines. Light text with the
	// shadow of INT_GlobalChat (readable over sky and grass).
	protected float BuildText(INT_QST_Style st, float w, array<INT_QST_QuestDef> list)
	{
		float y = 0;
		for (int i = 0; i < list.Count(); i++)
		{
			if (i > 0)
				y = y + 12;
			INT_QST_QuestDef qi = list[i];
			y = DrawTextEntry(m_Paint, st, qi, w, y);
		}
		return y;
	}

	protected float DrawTextEntry(Widget p, INT_QST_Style st, INT_QST_QuestDef q, float w, float y0)
	{
		int wait;
		int status = INT_QST_Client.StatusOf(q, wait);
		bool ready = status == INT_QST_St.READY;
		if (q.TimeLimit > 0 || INT_QST_Rules.HasInventoryTargets(q))
			m_NeedsLive = true;
		float y = y0;
		float tx = 0;
		int doneC = st.TextDoneColor;
		if (ready)
		{
			ShadowIcon(p, 0, y + 4, 18, "check", doneC);
			tx = 24;
		}
		string nm = INT_QST_Look.Fit(q.Name, w - tx, 16);
		int titleC = st.TextColor;
		if (ready)
			titleC = doneC;
		TextWidget tt = INT_QST_Look.Heavy(p, tx * m_K, y * m_K, (w - tx) * m_K, 24 * m_K, nm, 16 * m_K, titleC);
		Shade(tt);
		y = y + 27;
		int nt = q.Targets.Count();
		int shown = 0;
		int hiddenLeft = 0;
		int dim = st.TextDimColor;
		for (int i = 0; i < nt; i++)
		{
			INT_QST_TargetDef t = q.Targets[i];
			int have = INT_QST_Client.TargetValue(q, i);
			bool done = have >= t.m_Need;
			if (shown >= st.MaxLines)
			{
				hiddenLeft++;
				continue;
			}
			shown++;
			if (done)
			{
				ShadowIcon(p, 4, y + 4, 15, "check", doneC);
			}
			else
			{
				int boxEdge = st.TextColor;
				INT_UIF_Surface.Box(p, 6 * m_K, (y + 6) * m_K, 11 * m_K, 11 * m_K, 3, 0x66000000, boxEdge);
			}
			string nn = have.ToString() + " / " + t.m_Need.ToString();
			float nnW = INT_UIF_Draw.TextWidth(nn, 14) * 1.1 + 4;
			string txt = INT_QST_Text.Target(t);
			txt = INT_QST_Look.Fit(txt, w - 26 - nnW - 10, 14);
			int lc = st.TextColor;
			if (done)
				lc = dim;
			TextWidget lw = INT_QST_Look.Text(p, 26 * m_K, y * m_K, (w - 26 - nnW - 10) * m_K, 22 * m_K, txt, 14 * m_K, lc);
			Shade(lw);
			if (done)
			{
				float sw = INT_UIF_Draw.TextWidth(txt, 14) * 0.98;
				int sc = (dim & 0x00FFFFFF) | 0xC0000000;
				INT_UIF_Draw.Line(p, 26 * m_K, (y + 11) * m_K, sw * m_K, 1, sc);
			}
			int vc = st.TextColor;
			if (done)
				vc = doneC;
			TextWidget vw = INT_QST_Look.Heavy(p, (w - nnW) * m_K, y * m_K, nnW * m_K, 22 * m_K, nn, 14 * m_K, vc, INT_UIF_Align.RIGHT);
			Shade(vw);
			y = y + 22;
		}
		if (hiddenLeft > 0)
		{
			string more = INT_QST_Look.Fmt("#STR_INT_QST_TRK_MORE", hiddenLeft.ToString());
			TextWidget mw = INT_QST_Look.Text(p, 26 * m_K, y * m_K, (w - 26) * m_K, 20 * m_K, more, 13 * m_K, dim);
			Shade(mw);
			y = y + 20;
		}
		return y;
	}

	// The chat's shadow; its opacity follows the tracker's (SetAlpha does not touch the shadow).
	protected void Shade(TextWidget t)
	{
		if (t)
			t.SetShadow(2, 0xFF000000, 0.85 * m_Op, 1, 1);
	}

	// Icon over a dark copy one pixel down-right (icons have no shadow of their own).
	protected void ShadowIcon(Widget p, float x, float y, float size, string icon, int color)
	{
		int sh = 0xD9000000;
		INT_QST_Look.Icon(p, (x + 1) * m_K, (y + 1) * m_K, size * m_K, icon, sh);
		INT_QST_Look.Icon(p, x * m_K, y * m_K, size * m_K, icon, color);
	}

	// --- look 1: the notebook sheet (paper, leather header, footer) — kept for the public API ---------------------
	protected float BuildSheet(INT_QST_Style st, float w, array<INT_QST_QuestDef> list)
	{
		Widget body = INT_UIF_Draw.Frame(m_Paint, 0, 0, w * m_K, 100, "Body");
		float y = 0;
		if (st.ShowHeader)
			y = DrawHeader(st, w, list.Count());
		float bodyTop = y;
		Widget content = INT_UIF_Draw.Frame(m_Paint, 0, 0, w * m_K, 100, "Content");
		for (int i = 0; i < list.Count(); i++)
		{
			if (i > 0)
			{
				int hc = INT_QST_Look.InkA(55);
				INT_UIF_Draw.Line(content, 16 * m_K, y * m_K, (w - 32) * m_K, 1, hc);
			}
			INT_QST_QuestDef qi = list[i];
			y = DrawEntry(content, st, qi, w, y);
		}
		if (st.ShowFooter)
			y = DrawFooter(content, st, w, y);
		// paper body under the content (drawn first: the content frame was created after it)
		float bh = y - bodyTop;
		int pa = st.PaperAlpha;
		int paper = (st.PaperColor & 0x00FFFFFF) | (pa << 24);
		INT_UIF_Surface.Shadow(body, (-22) * m_K, (bodyTop - 10) * m_K, (w + 44) * m_K, (bh + 40) * m_K, 0x59000000);
		INT_UIF_Surface.Box(body, 0, bodyTop * m_K, w * m_K, bh * m_K, 8, paper, 0x8C3C2A16);
		INT_QST_Look.Tex(body, 0, bodyTop * m_K, w * m_K, bh * m_K, "int_qst_paper_ca", paper);
		return y;
	}

	protected float DrawHeader(INT_QST_Style st, float w, int count)
	{
		Widget h = INT_UIF_Draw.Frame(m_Paint, 0, 0, w * m_K, 40 * m_K, "Header");
		int lea = st.LeatherColor;
		INT_UIF_Surface.Box(h, 0, 0, w * m_K, 40 * m_K, 8, lea, 0xFF120D08);
		INT_QST_Look.Tex(h, 2 * m_K, 2 * m_K, (w - 4) * m_K, 36 * m_K, "int_qst_leather_ca", lea);
		int stc = INT_QST_Look.Stitch();
		INT_QST_Look.Tex(h, 10 * m_K, 4 * m_K, (w - 20) * m_K, 3 * m_K, "int_qst_stitch_h_ca", stc);
		int tan = INT_QST_Look.Tan();
		INT_QST_Look.Icon(h, 14 * m_K, 11 * m_K, 18 * m_K, "book", tan);
		string title = INT_QST_Look.Spaced("#STR_INT_QST_QUESTS");
		float tw = INT_UIF_Draw.TextWidth(title, 14) * 0.95;
		int cream = INT_QST_Look.Cream();
		INT_QST_Look.Heavy(h, 42 * m_K, 0, (tw + 4) * m_K, 40 * m_K, title, 14 * m_K, cream);
		float bx = 42 + tw + 10;
		string cnt = count.ToString();
		int bb = INT_QST_Look.CreamA(115);
		INT_UIF_Surface.Box(h, bx * m_K, 10 * m_K, 26 * m_K, 20 * m_K, 4, 0, bb);
		INT_QST_Look.Heavy(h, bx * m_K, 10 * m_K, 26 * m_K, 20 * m_K, cnt, 13 * m_K, cream, INT_UIF_Align.CENTER);
		int t2 = INT_QST_Look.Tan2();
		INT_QST_Look.Text(h, (w - 214) * m_K, 0, 200 * m_K, 40 * m_K, "DayZavr Quest System", 12 * m_K, t2, INT_UIF_Align.RIGHT);
		return 40;
	}

	protected float DrawEntry(Widget p, INT_QST_Style st, INT_QST_QuestDef q, float w, float y0)
	{
		int wait;
		int status = INT_QST_Client.StatusOf(q, wait);
		bool ready = status == INT_QST_St.READY;
		if (q.TimeLimit > 0 || INT_QST_Rules.HasInventoryTargets(q))
			m_NeedsLive = true;
		Widget e = INT_UIF_Draw.Frame(p, 0, y0 * m_K, w * m_K, 100, "Entry");
		Widget bg = INT_UIF_Draw.Frame(e, 0, 0, w * m_K, 100, "Bg");
		float y = 10;
		int ink = st.InkColor;
		int ink3 = INT_QST_Look.Ink3();
		if (!st.Compact)
		{
			string brTitle = INT_QST_Client.s_Catalog.BranchTitle(q.m_Branch);
			string br = INT_QST_Look.Caps(brTitle);
			br = INT_QST_Look.Fit(br, w - 40, 14);		// capitals are wider than the 12 px estimate
			INT_QST_Look.Text(e, 18 * m_K, y * m_K, (w - 36) * m_K, 18 * m_K, br, 12 * m_K, ink3);
			y = y + 18;
		}
		// name + chip
		string chip = "";
		string chipIcon = "clock";
		if (!st.Compact)
		{
			if (wait >= 0 && (status == INT_QST_St.ACTIVE || status == INT_QST_St.READY))
				chip = INT_QST_Look.Dur(wait);
			else if (ready && q.GiverFinish != "")
			{
				chip = INT_QST_Text.GiverDistance(q.GiverFinish);
				chipIcon = "map_pin";
			}
		}
		float chipW = 0;
		if (chip != "")
			chipW = INT_UIF_Draw.TextWidth(chip, 12) * 1.08 + 34;
		string nm = INT_QST_Look.Fit(q.Name, w - 36 - chipW - 8, 16);
		INT_QST_Look.Heavy(e, 18 * m_K, y * m_K, (w - 36 - chipW) * m_K, 24 * m_K, nm, 16 * m_K, ink);
		if (chip != "")
		{
			Widget cf = INT_UIF_Draw.Frame(e, (w - 18 - chipW) * m_K, (y + 1) * m_K, chipW * m_K, 22 * m_K, "Chip");
			int cb = INT_QST_Look.InkA(70);
			INT_UIF_Surface.Box(cf, 0, 0, chipW * m_K, 22 * m_K, 4, 0x33FFFFFF, cb);
			int ink2 = INT_QST_Look.Ink2();
			INT_QST_Look.Icon(cf, 8 * m_K, 5 * m_K, 12 * m_K, chipIcon, ink2);
			INT_QST_Look.Text(cf, 24 * m_K, 0, (chipW - 26) * m_K, 22 * m_K, chip, 12 * m_K, ink2);
		}
		y = y + 28;
		// target lines
		int nt = q.Targets.Count();
		int shown = 0;
		int hiddenLeft = 0;
		for (int i = 0; i < nt; i++)
		{
			INT_QST_TargetDef t = q.Targets[i];
			int have = INT_QST_Client.TargetValue(q, i);
			bool done = have >= t.m_Need;
			if (shown >= st.MaxLines)
			{
				hiddenLeft++;
				continue;
			}
			shown++;
			if (done)
			{
				int dc = st.DoneColor;
				INT_QST_Look.Icon(e, 18 * m_K, (y + 4) * m_K, 16 * m_K, "check", dc);
			}
			else
			{
				int bc = INT_QST_Look.Ink2();
				INT_UIF_Surface.Box(e, 20 * m_K, (y + 6) * m_K, 12 * m_K, 12 * m_K, 4, 0, bc);
			}
			string tt = INT_QST_Text.Target(t);
			string nn = have.ToString() + " / " + t.m_Need.ToString();
			float nnW = INT_UIF_Draw.TextWidth(nn, 14) * 1.1 + 4;
			tt = INT_QST_Look.Fit(tt, w - 44 - nnW - 22, 14);
			int tc = ink;
			if (done)
				tc = ink3;
			INT_QST_Look.Text(e, 44 * m_K, y * m_K, (w - 44 - nnW - 22) * m_K, 24 * m_K, tt, 14 * m_K, tc);
			if (done)
			{
				float sw = INT_UIF_Draw.TextWidth(tt, 14) * 0.98;
				int sc = INT_QST_Look.InkA(140);
				INT_UIF_Draw.Line(e, 44 * m_K, (y + 12) * m_K, sw * m_K, 1, sc);
			}
			int vc = ink;
			if (done)
				vc = st.DoneColor;
			INT_QST_Look.Heavy(e, (w - 18 - nnW) * m_K, y * m_K, nnW * m_K, 24 * m_K, nn, 14 * m_K, vc, INT_UIF_Align.RIGHT);
			y = y + 24;
		}
		if (hiddenLeft > 0)
		{
			string more = INT_QST_Look.Fmt("#STR_INT_QST_TRK_MORE", hiddenLeft.ToString());
			INT_QST_Look.Text(e, 44 * m_K, y * m_K, (w - 60) * m_K, 20 * m_K, more, 13 * m_K, ink3);
			y = y + 20;
		}
		if (ready)
		{
			y = y + 4;
			int acc = st.AccentColor;
			INT_QST_Look.Stamp(e, 18 * m_K, y * m_K, "#STR_INT_QST_TRK_READY", acc, 10 * m_K, "check");
			float stw = INT_QST_Look.StampWidth("#STR_INT_QST_TRK_READY", 10, true);
			string who = INT_QST_Look.Tr("#STR_INT_QST_G_AUTO");
			if (q.GiverFinish != "")
				who = INT_QST_Look.Fmt("#STR_INT_QST_TRK_TURNIN", INT_QST_Text.GiverName(q.GiverFinish));
			who = INT_QST_Look.Fit(who, w - stw - 70, 14);
			INT_QST_Look.Icon(e, (18 + stw + 10) * m_K, (y + 3) * m_K, 16 * m_K, "hand", ink);
			INT_QST_Look.Text(e, (18 + stw + 32) * m_K, y * m_K, (w - stw - 60) * m_K, 22 * m_K, who, 14 * m_K, ink);
			y = y + 26;
		}
		y = y + 10;
		if (ready)
		{
			INT_UIF_Draw.Line(bg, 0, 0, w * m_K, y * m_K, 0x8FF7CC40);
			int acc2 = st.AccentColor;
			INT_UIF_Draw.Line(bg, 0, 0, 4 * m_K, y * m_K, acc2);
		}
		INT_UIF_Draw.Resize(e, w * m_K, y * m_K);
		return y0 + y;
	}

	protected float DrawFooter(Widget p, INT_QST_Style st, float w, float y0)
	{
		int hc = INT_QST_Look.InkA(55);
		INT_UIF_Draw.Line(p, 0, y0 * m_K, w * m_K, 1, hc);
		Widget f = INT_UIF_Draw.Frame(p, 0, y0 * m_K, w * m_K, 34 * m_K, "Footer");
		string key = INT_QST_Book.KeyName("UAINTQuestsJournal", "J");
		float kw = INT_UIF_Draw.TextWidth(key, 13) * 1.15 + 14;
		if (kw < 24)
			kw = 24;
		int ink2 = INT_QST_Look.Ink2();
		int ink = st.InkColor;
		INT_UIF_Surface.Box(f, 16 * m_K, 6 * m_K, kw * m_K, 22 * m_K, 4, 0, ink2);
		INT_QST_Look.Heavy(f, 16 * m_K, 6 * m_K, kw * m_K, 22 * m_K, key, 13 * m_K, ink, INT_UIF_Align.CENTER);
		string jt = INT_QST_Look.Tr("#STR_INT_QST_TRK_JOURNAL");
		int ink3 = INT_QST_Look.Ink3();
		INT_QST_Look.Text(f, (16 + kw + 8) * m_K, 0, 200 * m_K, 34 * m_K, jt, 13 * m_K, ink3);
		string on = INT_QST_Look.Fmt("#STR_INT_QST_TRK_ON_SCREEN", m_Shown.ToString());
		float ow = INT_UIF_Draw.TextWidth(on, 13) * 1.05;
		INT_QST_Look.Text(f, (w - 16 - ow) * m_K, 0, ow * m_K, 34 * m_K, on, 13 * m_K, ink3);
		int acc = st.AccentColor;
		INT_QST_Look.Icon(f, (w - 16 - ow - 22) * m_K, 9 * m_K, 15 * m_K, "pushpin", acc);
		return y0 + 34;
	}

	// Box position in screen px (style FreeX/FreeY or the default top-right place).
	protected void Place()
	{
		if (!m_Box)
			return;
		PlaceBox();
		INT_QST_UI.OnPlaced().Invoke();
	}

	protected void PlaceBox()
	{
		if (!m_Box)
			return;
		INT_QST_Style st = INT_QST_UI.Style();
		float sw = m_ScreenW;
		float sh = m_ScreenH;
		float bw = INT_UIF_Draw.S(m_BoxW * m_K);
		float bh = INT_UIF_Draw.S(m_BoxH * m_K);
		float x;
		float y;
		if (st.FreeX < 0 || st.FreeY < 0)
		{
			x = sw - INT_UIF_Draw.S(DEFAULT_RIGHT) - bw;
			y = INT_UIF_Draw.S(DEFAULT_TOP);
		}
		else
		{
			x = st.FreeX * sw;
			y = st.FreeY * sh;
		}
		x = Math.Clamp(x, 0, Math.Max(0, sw - bw));
		y = Math.Clamp(y, 0, Math.Max(0, sh - INT_UIF_Draw.S(60)));
		m_BaseY = y;
		m_Box.SetPos(x, y + m_Offset);
	}

	// INT_UIFramework toasts sit in the top-right column: move down under them when they overlap (KillFeed rule).
	protected void UpdateToastOffset()
	{
		if (!m_Box)
			return;
		float want = 0;
		if (!m_Edit)
		{
			INT_UIF_ToastManager tm = INT_UIF_ToastManager.Peek();
			int n = 0;
			if (tm)
				n = tm.VisibleCount();
			if (n > 0)
			{
				float toastW = INT_UIF_ThemeManager.Get().Metric(INT_UIF_ThemeKeys.TOAST_WIDTH, 384);
				float toastH = INT_UIF_ThemeManager.Get().Metric(INT_UIF_ThemeKeys.TOAST_HEIGHT, 76);
				float colX = m_ScreenW - INT_UIF_Draw.S(toastW) - INT_UIF_Draw.S(24);
				float bottom = INT_UIF_Draw.S(24) + n * (INT_UIF_Draw.S(toastH) + INT_UIF_Draw.S(12));
				float fx;
				float fy;
				m_Box.GetPos(fx, fy);
				float fw;
				float fh;
				m_Box.GetSize(fw, fh);
				if (fx + fw > colX && m_BaseY < bottom)
					want = bottom - m_BaseY + INT_UIF_Draw.S(6);
			}
		}
		if (Math.AbsFloat(want - m_Offset) < 0.5)
			return;
		m_Offset = want;
		float px;
		float py;
		m_Box.GetPos(px, py);
		m_Box.SetPos(px, m_BaseY + m_Offset);
	}

	// The default place (top right) of a box `bwPx` screen px wide, in screen px.
	static void DefaultPlace(float bwPx, out float x, out float y)
	{
		x = INT_UIF_Scale.ScreenW() - INT_UIF_Draw.S(DEFAULT_RIGHT) - bwPx;
		y = INT_UIF_Draw.S(DEFAULT_TOP);
	}

	// --- self-test ------------------------------------------------------------------------------------------
	int ShownCount()
	{
		if (m_Hidden || !m_Box)
			return 0;
		return m_Shown;
	}
}

// The same tracker drawn into a page of the journal settings (real size, the player's scale and opacity, at most
// `most` quests; samples from the catalog when nothing is tracked). Not registered with INT_QST_UI, no events.
class INT_QST_HudPreview extends INT_QST_Hud
{
	void Attach(Widget parent, int most)
	{
		m_Root = parent;
		m_Preview = true;
		m_PreviewMax = most;
		m_ScreenW = INT_UIF_Scale.ScreenW();
		m_ScreenH = INT_UIF_Scale.ScreenH();
	}

	void Rebuild(int most)
	{
		m_PreviewMax = most;
		Build();
	}

	// Size of the drawn box in design px (0 when nothing is drawn).
	float BoxW()
	{
		if (!m_Box)
			return 0;
		return m_BoxW * m_K;
	}

	float BoxH()
	{
		if (!m_Box)
			return 0;
		return m_BoxH * m_K;
	}

	// Top-left corner inside the host frame (design px).
	void MoveTo(float x, float y)
	{
		if (m_Box)
			INT_UIF_Draw.Move(m_Box, x, y);
	}

	void Detach()
	{
		if (m_Box)
			m_Box.Unlink();
		m_Box = null;
		m_Paint = null;
		m_Root = null;
	}
}
