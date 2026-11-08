// INT PartyMode (client) — tab «Моя группа» (3_c_expedition_group.png): counters with the server display limit,
// the friend list as a «route» of diamond nodes (a pool of 9 rows, scrolled by whole rows with the wheel) and the
// detail panel of the selected friend (alias, ping look, show toggle, remove with confirmation).
// Only shown friends have HP (PULL); distance and direction are computed here from WorldPos().

class INT_PTY_GroupRow : Managed
{
	Widget Root;
	ImageWidget SelBg;
	ImageWidget SelBar;
	ImageWidget HoverBg;
	ImageWidget NodeFill;
	ImageWidget NodeLine;
	TextWidget Name;
	TextWidget Nick;
	ImageWidget StDot;
	ImageWidget StSkull;
	TextWidget St1;
	TextWidget St2;
	TextWidget HpVal;
	TextWidget HpPct;
	ImageWidget HpTrack;
	ImageWidget HpFill;
	TextWidget HpDash;
	TextWidget DistVal;
	ImageWidget DirArrow;
	TextWidget DirText;
	TextWidget DistDash;
	ImageWidget EyeIcon;
	ImageWidget EyeHover;
	ButtonWidget Hit;
	ButtonWidget EyeHit;
	string Id;
	string Sig;
	string EyeSig;		// W-2: the eye icon / hover / tooltip are rebuilt only when this changes
}

class INT_PTY_PageGroup : INT_PTY_Page
{
	static const float ROW0 = 384;
	static const float RH = 66;
	static const int VISIBLE = 9;
	static const float LX = 80;
	static const float DX = 1168;

	protected static string s_SelId;

	protected ref array<ref INT_PTY_GroupRow> m_Rows;
	protected string m_SelId;
	protected float m_Acc;
	protected int m_FriendCount = -1;
	protected bool m_Empty;

	// counters
	protected TextWidget m_CountText;
	protected TextWidget m_ShownText;
	protected TextWidget m_LimitText;
	protected TextWidget m_ShownLabel;
	protected ref array<ImageWidget> m_Segs;
	protected int m_ShownCount = -1;
	protected int m_PassShown = -1;			// W-6: ShownCount of the running RepaintRows pass (-1 = outside a pass)
	protected ButtonWidget m_LimitHit;
	protected ImageWidget m_Route;
	protected ImageWidget m_InfoIcon;

	// detail
	protected Widget m_Det;
	protected Widget m_Conn;
	protected TextWidget m_DName;
	protected TextWidget m_DNick;
	protected Widget m_DStatus;
	protected TextWidget m_DHp;
	protected TextWidget m_DHpUnit;
	protected TextWidget m_DDist;
	protected TextWidget m_DDistUnit;
	protected ImageWidget m_DArrow;
	protected TextWidget m_DDir;
	protected TextWidget m_DPrevText;
	protected TextWidget m_DShapeName;
	protected TextWidget m_DPlaceholder;
	protected EditBoxWidget m_DEdit;
	protected ref INT_PTY_MToggle m_DShow;
	protected ref array<ref INT_UIF_Surface> m_DTiles;
	protected ref array<ImageWidget> m_DTileIcons;
	protected ImageWidget m_DSwRing;
	protected Widget m_DPrevBox;
	protected string m_DSig;
	protected string m_DStatusSig;

	void INT_PTY_PageGroup()
	{
		m_Rows = new array<ref INT_PTY_GroupRow>();
		m_Segs = new array<ImageWidget>();
		m_DTiles = new array<ref INT_UIF_Surface>();
		m_DTileIcons = new array<ImageWidget>();
	}

	protected int Count()
	{
		if (!INT_PTY_Client.s_Friends)
			return 0;
		return INT_PTY_Client.s_Friends.Count();
	}

	protected INT_PTY_Friend FriendAt(int i)
	{
		if (!INT_PTY_Client.s_Friends || i < 0 || i >= INT_PTY_Client.s_Friends.Count())
			return null;
		return INT_PTY_Client.s_Friends.Get(i);
	}

	protected int IndexOf(string id)
	{
		int n = Count();
		for (int i = 0; i < n; i++)
		{
			INT_PTY_Friend f = INT_PTY_Client.s_Friends.Get(i);
			if (f && f.Id == id)
				return i;
		}
		return -1;
	}

	// --- build -------------------------------------------------------------------------------------------------
	override void Build(bool animate)
	{
		float d = m_Menu.PageDelay();
		m_Rows.Clear();
		m_Segs.Clear();
		m_DTiles.Clear();
		m_DTileIcons.Clear();
		m_Det = null;
		m_Conn = null;
		m_ShownCount = -1;
		m_FriendCount = Count();
		if (m_SelId == "")
			m_SelId = s_SelId;
		if (IndexOf(m_SelId) < 0)
		{
			m_SelId = "";
			INT_PTY_Friend first = FriendAt(0);
			if (first)
				m_SelId = first.Id;
		}
		s_SelId = m_SelId;
		m_Empty = m_FriendCount == 0;
		if (m_Empty)
		{
			BuildEmpty(animate, d);
			return;
		}
		BuildStats(animate, d);
		BuildList(animate, d);
		BuildDetail(animate, d + 0.18);
	}

	protected void BuildEmpty(bool animate, float d)
	{
		Widget p = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_GroupEmpty");
		INT_PTY_MD.DmLine(p, 98, 300, 30, INT_PTY_MC.O);
		INT_PTY_MD.Dm(p, 98, 300, 10, INT_PTY_MC.O);
		string title = "#STR_INT_PTY_W_G_EMPTY_T";
		if (!INT_PTY_Client.IsWelcomed())
			title = "#STR_INT_PTY_W_LOADING";
		INT_PTY_MD.Txt(p, 140, 268, 1500, 64, title, 48, INT_PTY_MC.T, INT_UIF_Align.LEFT, false, true);
		INT_PTY_MD.Para(p, 142, 352, 900, "#STR_INT_PTY_W_G_EMPTY", 20, INT_PTY_MC.BODY, 30, 4);
		INT_PTY_MButton b = INT_PTY_MButton.Create(p, 142, 500, 48, "search", "#STR_INT_PTY_W_TAB_SEARCH", INT_PTY_MButton.OL);
		ButtonWidget hit = m_Menu.Hit(b.m_Root, 0, 0, b.m_W, b.m_H, "goto:1");
		m_Menu.SetHoverBtn(hit, b);
		m_Menu.SetTip(hit, "#STR_INT_PTY_W_TAB_SEARCH", "#STR_INT_PTY_W_TIP_TAB_1");
		m_KeepBtn = b;
		if (animate)
			INT_PTY_MenuFx.BlockIn(p, 0, 16, d, 0.35);
	}

	protected ref INT_PTY_MButton m_KeepBtn;

	protected void BuildStats(bool animate, float d)
	{
		Widget p = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 400, "INT_PTY_GStats");
		m_CountText = INT_PTY_MD.Txt(p, 78, 236, 140, 76, INT_PTY_Text.Pad2(Count()), 64, INT_PTY_MC.T, INT_UIF_Align.LEFT, false, true);
		INT_PTY_MD.Caps(p, 80, 316, 200, 20, "#STR_INT_PTY_W_G_FRIENDS", 13, INT_PTY_MC.M);
		m_ShownText = INT_PTY_MD.Txt(p, 214, 236, 90, 76, "00", 64, INT_PTY_MC.T, INT_UIF_Align.LEFT, false, true);
		m_LimitText = INT_PTY_MD.Txt(p, 286, 270, 140, 34, "", 26, INT_PTY_MC.M);
		// segment bar: one segment per allowed friend, placed by RepaintCounters after the «/ 10» text
		int lim = INT_PTY_Client.s_DisplayLimit;
		for (int i = 0; i < lim; i++)
		{
			ImageWidget seg = INT_PTY_MD.Line(p, 350, 285, 18, 4, 0x29FFFFFF);
			m_Segs.Insert(seg);
		}
		m_ShownLabel = INT_PTY_Look.Text(p, 214, 316, 560, 20, "", 13, INT_PTY_MC.M, INT_UIF_Align.LEFT, true);
		m_InfoIcon = INT_PTY_MD.Ico(p, 600, 318, 15, "info", INT_PTY_MC.M);
		m_LimitHit = m_Menu.Hit(p, 206, 286, 560, 56, "limit");
		m_Menu.SetSound(m_LimitHit, false);
		m_ShownCount = -1;
		RepaintCounters(false);
		// column heads
		Widget cols = INT_UIF_Draw.Frame(p, 0, 0, 1920, 400, "INT_PTY_GCols");
		INT_PTY_MD.Caps(cols, LX + 52, 354, 200, 20, "#STR_INT_PTY_W_G_COL_FRIEND", 13, INT_PTY_MC.D);
		INT_PTY_MD.Caps(cols, LX + 350, 354, 200, 20, "#STR_INT_PTY_W_G_COL_STATUS", 13, INT_PTY_MC.D);
		INT_PTY_MD.Caps(cols, LX + 570, 354, 200, 20, "#STR_INT_PTY_W_G_COL_HP", 13, INT_PTY_MC.D);
		INT_PTY_MD.Caps(cols, LX + 740, 354, 200, 20, "#STR_INT_PTY_W_G_COL_DIST", 13, INT_PTY_MC.D);
		INT_PTY_MD.Ico(cols, LX + 916, 355, 17, "eye", INT_PTY_MC.D);
		INT_PTY_MD.Line(cols, LX, 380, 960, 1, INT_PTY_MC.L);
		if (animate)
		{
			INT_PTY_MenuFx.BlockIn(p, 0, 14, d, 0.32);
		}
	}

	protected void RepaintCounters(bool animate)
	{
		int shown = INT_PTY_Client.ShownCount();
		int lim = INT_PTY_Client.s_DisplayLimit;
		if (m_CountText)
			INT_PTY_Look.SetHeavyText(m_CountText, INT_PTY_Text.Pad2(Count()));
		if (shown == m_ShownCount)
			return;
		bool changed = m_ShownCount >= 0;
		m_ShownCount = shown;
		if (m_ShownText)
		{
			m_ShownText.SetText(INT_PTY_Text.Pad2(shown));
			if (animate && changed)
				INT_PTY_MenuFx.Bump(m_ShownText, 16);
		}
		if (m_LimitText)
		{
			// «06» big, «/ 10» small after it, the segment bar after a clear gap, centred on the small text
			string limTxt = "/ " + lim.ToString();
			m_LimitText.SetText(limTxt);
			Widget sp = m_LimitText.GetParent();
			string shownTxt = INT_PTY_Text.Pad2(shown);
			float sw = INT_PTY_MD.Measure(sp, shownTxt, 64, false, true);
			float lx = 214 + sw + 10;
			INT_UIF_Draw.Move(m_LimitText, lx, 270);
			float lw2 = INT_PTY_MD.Measure(sp, limTxt, 26);
			PlaceSegs(lx + lw2 + 26, 285);
		}
		for (int i = 0; i < m_Segs.Count(); i++)
		{
			int c = 0x29FFFFFF;
			if (i < shown)
				c = INT_PTY_MC.O;
			ImageWidget s = m_Segs.Get(i);
			if (!s)
				continue;
			if (animate)
				INT_UIF_Animator.ColorTo(s, c, 0.2, INT_UIF_Ease.OUT_CUBIC);
			else
				s.SetColor(c);
		}
		string shownS = shown.ToString();
		string limS = lim.ToString();
		string fa = INT_PTY_Text.Fmt("#STR_INT_PTY_W_G_SHOWN", shownS, limS);
		string a = INT_PTY_Text.Caps(fa);
		string b = INT_PTY_Text.Caps("#STR_INT_PTY_W_G_LIMIT");
		string full = a + "   ·   " + b;
		if (m_ShownLabel)
		{
			m_ShownLabel.SetText(full);
			float lw = INT_PTY_MD.Measure(m_ShownLabel.GetParent(), full, 13, true);
			if (m_InfoIcon)
				INT_UIF_Draw.Move(m_InfoIcon, 214 + lw + 10, 318);
		}
		if (m_LimitHit)
		{
			string tt = INT_PTY_Text.Fmt("#STR_INT_PTY_W_TIP_LIMIT_H", shownS, limS);
			m_Menu.SetTip(m_LimitHit, tt, "#STR_INT_PTY_W_TIP_LIMIT", 360);
		}
	}

	// Segments from x (design px), 4 px tall at y; 18 px with 5 px gaps (the concept), narrower for big limits.
	protected void PlaceSegs(float x, float y)
	{
		int n = m_Segs.Count();
		if (n <= 0)
			return;
		float room = 1030 - x;
		if (room > 300)
			room = 300;
		float gap = 5;
		float segW = 18;
		if (n * (segW + gap) - gap > room)
		{
			gap = 2;
			segW = (room - gap * (n - 1)) / n;
			if (segW < 2)
				segW = 2;
		}
		for (int i = 0; i < n; i++)
		{
			ImageWidget s = m_Segs.Get(i);
			if (s)
				INT_UIF_Draw.Place(s, x + i * (segW + gap), y, segW, 4);
		}
	}

	// --- list: pixel-smooth scroller (owner 2026-10-23), scrollbar left of the route ----------------------------
	protected ref INT_PTY_MScroll m_List;
	protected float m_SavedOff;

	protected void BuildList(bool animate, float d)
	{
		Widget p = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_GList");
		m_Route = INT_PTY_MD.Line(p, 100, ROW0 + 33, 1, 10, INT_PTY_MC.L2);
		m_Rows.Clear();
		m_List = new INT_PTY_MScroll();
		m_List.Init(m_Menu, this, 0, "glist", p, LX, ROW0, 960, VISIBLE * RH, RH, LX - 16);
		m_List.SetCount(Count(), false);
		if (m_SavedOff > 0)
			m_List.Jump(m_SavedOff);
		int sel = IndexOf(m_SelId);
		if (sel >= 0)
			m_List.Reveal(sel);
		if (animate)
		{
			m_List.Enter(d + 0.1);
			INT_PTY_MenuFx.FadeIn(m_Route, d + 0.08, 0.4);
		}
	}

	override void ScrollBuild(int sid, int slot, Widget root)
	{
		INT_PTY_GroupRow r = new INT_PTY_GroupRow();
		r.Root = root;
		Widget w = root;
		r.HoverBg = INT_PTY_MD.Line(w, 0, 0, 960, RH, ARGB(12, 255, 255, 255));
		r.SelBg = INT_PTY_Look.Tex(w, 0, 0, 960, RH, "pty_grad_h_ca", ARGB(16, 255, 122, 47));
		r.SelBar = INT_PTY_MD.Line(w, 0, 0, 3, RH, INT_PTY_MC.O);
		INT_PTY_MD.Line(w, 0, RH - 1, 960, 1, INT_PTY_MC.L_ROW);
		// node: a dark diamond under it hides the route line
		INT_PTY_MD.Dm(w, 20, 33, 13, INT_PTY_MC.BG);
		r.NodeFill = INT_PTY_MD.Dm(w, 20, 33, 12, INT_PTY_MC.O);
		r.NodeLine = INT_PTY_MD.DmLine(w, 20, 33, 12, INT_PTY_MC.O);
		r.Name = INT_PTY_MD.Txt(w, 52, 8, 290, 32, "", 23, INT_PTY_MC.T);
		r.Nick = INT_PTY_MD.Txt(w, 52, 38, 290, 20, "", 14, INT_PTY_MC.M);
		r.StDot = INT_PTY_MD.Dot(w, 354, 21, 8, INT_PTY_MC.G);
		r.StSkull = INT_PTY_MD.Ico(w, 350, 13, 16, "skull", INT_PTY_MC.R);
		r.St1 = INT_PTY_Look.Text(w, 367, 12, 220, 18, "", 13, INT_PTY_MC.T, INT_UIF_Align.LEFT, true);
		r.St2 = INT_PTY_MD.Txt(w, 350, 34, 230, 20, "", 14, INT_PTY_MC.M);
		r.HpVal = INT_PTY_MD.Txt(w, 568, 10, 80, 38, "", 30, INT_PTY_MC.T, INT_UIF_Align.LEFT, false, true);
		r.HpPct = INT_PTY_MD.Txt(w, 610, 20, 30, 22, "%", 15, INT_PTY_MC.M);
		r.HpTrack = INT_PTY_MD.Line(w, 570, 49, 112, 2, 0x21FFFFFF);
		r.HpFill = INT_PTY_MD.Line(w, 570, 49, 56, 2, INT_PTY_MC.T);
		r.HpDash = INT_PTY_MD.Txt(w, 570, 16, 60, 34, "—", 24, INT_PTY_MC.D);
		r.DistVal = INT_PTY_MD.Txt(w, 740, 16, 120, 34, "", 23, INT_PTY_MC.T);
		r.DirArrow = INT_PTY_MD.Ico(w, 800, 26, 14, "arrow_up", INT_PTY_MC.M);
		r.DirText = INT_PTY_Look.Text(w, 818, 22, 60, 22, "", 13, INT_PTY_MC.M, INT_UIF_Align.LEFT, true);
		r.DistDash = INT_PTY_MD.Txt(w, 740, 16, 60, 34, "—", 24, INT_PTY_MC.D);
		r.EyeHover = INT_PTY_MD.Line(w, 906, 15, 36, 36, ARGB(46, 255, 122, 47));
		r.EyeIcon = INT_PTY_MD.Ico(w, 913, 22, 22, "eye", INT_PTY_MC.T);
		r.Hit = m_Menu.Hit(w, 0, 0, 896, RH, "row:" + slot.ToString());
		m_Menu.SetSound(r.Hit, false);
		r.EyeHit = m_Menu.Hit(w, 896, 6, 60, 54, "eye:" + slot.ToString());
		m_Menu.SetHoverBg(r.EyeHit, r.EyeHover, 1);
		m_List.SetSlotHover(slot, r.HoverBg);
		m_Rows.Insert(r);
	}

	override void ScrollFill(int sid, int slot, int index)
	{
		if (slot < 0 || slot >= m_Rows.Count())
			return;
		INT_PTY_GroupRow r = m_Rows.Get(slot);
		r.Sig = "";
		FillRow(r, FriendAt(index), true);
	}

	// The route line runs between the first and the last node inside the list; the connector follows the row.
	override void ScrollMoved(int sid)
	{
		if (m_Route && m_List)
		{
			int n = Count();
			float top = m_List.ItemY(0) + 33;
			float bottom = m_List.ItemY(n - 1) + 33;
			if (top < ROW0)
				top = ROW0;
			if (bottom > ROW0 + VISIBLE * RH)
				bottom = ROW0 + VISIBLE * RH;
			if (n > 1 && bottom > top)
			{
				m_Route.Show(true);
				INT_UIF_Draw.Place(m_Route, 100, top, 1, bottom - top);
			}
			else
			{
				m_Route.Show(false);
			}
		}
		PlaceConnector();
	}

	protected void RepaintRows(bool force)
	{
		if (!m_List)
			return;
		int slots = m_Rows.Count();
		// W-6: the shown count once per pass, not once per hidden row (friends × prefs each time)
		m_PassShown = INT_PTY_Client.ShownCount();
		for (int i = 0; i < slots; i++)
		{
			int idx = m_List.SlotIndex(i);
			if (idx < 0)
				continue;
			INT_PTY_GroupRow r = m_Rows.Get(i);
			if (FillRow(r, FriendAt(idx), force))
				m_List.Recapture(i);
		}
		m_PassShown = -1;
		PlaceConnector();
	}

	// Colour with the alpha multiplied (dimmed hidden rows: SetAlpha would REPLACE the alpha).
	protected int K(int c, float k)
	{
		if (k >= 0.999)
			return c;
		int a = INT_UIF_Color.A(c);
		return INT_UIF_Color.WithAlpha(c, (int)(a * k));
	}

	protected bool FillRow(INT_PTY_GroupRow r, INT_PTY_Friend f, bool force)
	{
		if (!f)
			return false;
		string id = f.Id;
		INT_PTY_FriendPref pref = INT_PTY_SettingsStore.Get().Pref(id);
		string alias = "";
		if (pref)
			alias = pref.Alias;
		bool shown = INT_PTY_Client.IsShown(id);
		bool sel = id == m_SelId;
		int state = RowState(f, shown);
		string hpTxt = "";
		int hp = f.Hp;
		bool hasHp = shown && f.Online && !f.Dead && f.HasState;
		if (hasHp)
			hpTxt = hp.ToString();
		string distTxt = "";
		string dirTxt = "";
		float bearing = 0;
		bool hasDist = false;
		vector me;
		if (f.Online && f.KnowsPos() && INT_PTY_MenuGeo.MyPos(me))
		{
			vector there = f.WorldPos();
			float dist = INT_PTY_MenuGeo.Flat(me, there);
			distTxt = INT_PTY_Util.DistText(dist);
			bearing = INT_PTY_MenuGeo.Bearing(me, there);
			dirTxt = INT_PTY_MenuGeo.CompassOf(bearing);
			hasDist = true;
		}
		string s2 = Sub2(f, shown, state);
		int limFull = 0;
		int scount = 0;
		int slim = INT_PTY_Client.s_DisplayLimit;
		if (!shown)
		{
			scount = m_PassShown;
			if (scount < 0)
				scount = INT_PTY_Client.ShownCount();
		}
		if (!shown && scount >= slim)
			limFull = 1;
		int col = INT_PTY_MenuGeo.FriendColor(id);
		int shownI = 0;
		if (shown)
			shownI = 1;
		int selI = 0;
		if (sel)
			selI = 1;
		int bearI = (int)bearing;
		string sig = id + "|" + alias + "|" + f.Nick + "|" + shownI.ToString();
		sig = sig + "|" + selI.ToString() + "|" + state.ToString() + "|" + hpTxt;
		sig = sig + "|" + distTxt + "|" + bearI.ToString() + "|" + s2 + "|" + limFull.ToString() + "|" + col.ToString();
		if (!force && sig == r.Sig)
			return false;
		r.Sig = sig;
		r.Id = id;
		float k = 1;
		if (!shown)
			k = 0.42;
		// selection
		if (r.SelBg)
			r.SelBg.Show(sel);
		if (r.SelBar)
			r.SelBar.Show(sel);
		// node
		bool hollow = state == 2 || state == 3;
		int nodeCol = col;
		if (state == 3)
			nodeCol = INT_PTY_MC.KNOB_OFF;
		if (r.NodeFill)
		{
			r.NodeFill.Show(!hollow);
			r.NodeFill.SetColor(nodeCol);
		}
		if (r.NodeLine)
		{
			r.NodeLine.Show(hollow);
			r.NodeLine.SetColor(nodeCol);
		}
		// name (+ nick when there is an alias)
		string big = f.Nick;
		if (alias != "")
			big = alias;
		big = INT_PTY_Look.FitText(big, 23, false, 280);
		if (r.Name)
		{
			r.Name.SetText(big);
			r.Name.SetColor(K(INT_PTY_MC.T, k));
			if (alias != "")
				INT_UIF_Draw.Move(r.Name, 52, 6);
			else
				INT_UIF_Draw.Move(r.Name, 52, 17);
		}
		if (r.Nick)
		{
			r.Nick.Show(alias != "");
			r.Nick.SetText(INT_PTY_Look.FitText(f.Nick, 14, false, 280));
			r.Nick.SetColor(K(INT_PTY_MC.M, k));
		}
		// status
		string s1 = "#STR_INT_PTY_W_ST_ONLINE";
		int s1c = INT_PTY_MC.T;
		int dotc = INT_PTY_MC.G;
		if (state == 2)
		{
			s1 = "#STR_INT_PTY_W_ST_DEAD";
			s1c = INT_PTY_MC.R;
		}
		else if (state == 3)
		{
			s1 = "#STR_INT_PTY_W_ST_OFFLINE";
			s1c = INT_PTY_MC.M;
			dotc = INT_PTY_MC.OFF;
		}
		float s1y = 23;
		if (s2 != "")
			s1y = 12;
		if (r.St1)
		{
			r.St1.SetText(INT_PTY_Text.Caps(s1));
			r.St1.SetColor(K(s1c, k));
			INT_UIF_Draw.Move(r.St1, 372, s1y);
		}
		if (r.StDot)
		{
			r.StDot.Show(state != 2);
			r.StDot.SetColor(K(dotc, k));
			INT_UIF_Draw.Move(r.StDot, 356, s1y + 5);
		}
		if (r.StSkull)
		{
			r.StSkull.Show(state == 2);
			r.StSkull.SetColor(K(INT_PTY_MC.R, k));
			INT_UIF_Draw.Move(r.StSkull, 350, s1y + 1);
		}
		if (r.St2)
		{
			r.St2.Show(s2 != "");
			r.St2.SetText(s2);
			int s2c = INT_PTY_MC.M;
			if (state == 1 || (f.Online && !f.Dead && f.HasFlag(INT_PTY_Const.F_BLEEDING)))
				s2c = INT_PTY_MC.R_TEXT;
			else if (f.Online && !f.Dead && f.HasFlag(INT_PTY_Const.F_UNCONSCIOUS))
				s2c = INT_PTY_MC.W;
			r.St2.SetColor(K(s2c, k));
		}
		// health
		int hpc = INT_PTY_MC.T;
		if (hasHp && hp < 20)
			hpc = INT_PTY_MC.R;
		if (r.HpVal)
		{
			r.HpVal.Show(hasHp);
			r.HpVal.SetText(hpTxt);
			r.HpVal.SetColor(K(hpc, k));
		}
		if (r.HpPct)
		{
			r.HpPct.Show(hasHp);
			int pc = INT_PTY_MC.M;
			if (hasHp && hp < 20)
				pc = INT_PTY_MC.R;
			r.HpPct.SetColor(K(pc, k));
			float hw = INT_PTY_MD.Measure(r.Root, hpTxt, 30, false, true);
			INT_UIF_Draw.Move(r.HpPct, 570 + hw - 1, 20);
		}
		if (r.HpTrack)
		{
			r.HpTrack.Show(hasHp);
			r.HpTrack.SetColor(K(0x21FFFFFF, k));
		}
		if (r.HpFill)
		{
			r.HpFill.Show(hasHp && hp > 0);
			INT_UIF_Draw.Resize(r.HpFill, 112 * Math.Clamp(hp / 100.0, 0, 1), 2);
			r.HpFill.SetColor(K(hpc, k));
		}
		if (r.HpDash)
			r.HpDash.Show(!hasHp);
		// distance + direction
		if (r.DistVal)
		{
			r.DistVal.Show(hasDist);
			r.DistVal.SetText(distTxt);
			r.DistVal.SetColor(K(INT_PTY_MC.T, k));
		}
		float dw = INT_PTY_MD.Measure(r.Root, distTxt, 23);
		float ax = 740 + dw + 12;
		if (r.DirArrow)
		{
			r.DirArrow.Show(hasDist);
			INT_UIF_Draw.Move(r.DirArrow, ax, 26);
			r.DirArrow.SetRotation(0, 0, bearing);
			r.DirArrow.SetColor(K(INT_PTY_MC.M, k));
		}
		if (r.DirText)
		{
			r.DirText.Show(hasDist);
			r.DirText.SetText(dirTxt);
			r.DirText.SetColor(K(INT_PTY_MC.M, k));
			INT_UIF_Draw.Move(r.DirText, ax + 18, 22);
		}
		if (r.DistDash)
			r.DistDash.Show(!hasDist);
		// eye — W-2: only when its own state changes (shown / limit full / the counts in its tooltip), so a moving
		// friend's row (distance changes every 0.5 s) does not rebuild the tooltip or reset the hover colour.
		string eyeSig = shownI.ToString() + "|" + limFull.ToString();
		if (!shown)
			eyeSig = eyeSig + "|" + scount.ToString() + "|" + slim.ToString();
		if (eyeSig == r.EyeSig)
			return true;
		r.EyeSig = eyeSig;
		if (r.EyeIcon)
		{
			string eye = "eye_off";
			int ec = INT_PTY_MC.D;
			if (shown)
			{
				eye = "eye";
				ec = INT_PTY_MC.T;
			}
			r.EyeIcon.LoadImageFile(0, INT_PTY_Look.IconPath(eye));
			r.EyeIcon.SetImage(0);
			r.EyeIcon.SetColor(ec);
			m_Menu.SetHoverImg(r.EyeHit, r.EyeIcon, ec, INT_PTY_MC.O);
		}
		if (shown)
		{
			m_Menu.SetTip(r.EyeHit, "#STR_INT_PTY_W_TIP_EYE_ON_H", "#STR_INT_PTY_W_TIP_EYE_ON", 340);
		}
		else
		{
			// a hidden friend's eye carries the display limit (concept frame): full -> hide another one first
			string sc = scount.ToString();
			string sl = slim.ToString();
			string lt = INT_PTY_Text.Fmt("#STR_INT_PTY_W_TIP_LIMIT_H", sc, sl);
			string body = INT_PTY_Text.Fmt("#STR_INT_PTY_W_TIP_EYE_OFF", sl);
			if (limFull == 1)
				body = INT_PTY_Text.Tr("#STR_INT_PTY_W_TIP_LIMIT");
			m_Menu.SetTip(r.EyeHit, lt, body, 340);
		}
		return true;
	}

	// 0 online, 1 online with HP < 20 %, 2 dead, 3 offline.
	protected int RowState(INT_PTY_Friend f, bool shown)
	{
		if (!f.Online)
			return 3;
		if (f.Dead)
			return 2;
		if (shown && f.HasState && f.Hp < 20)
			return 1;
		return 0;
	}

	protected string Sub2(INT_PTY_Friend f, bool shown, int state)
	{
		if (state == 3)
		{
			float off = INT_PTY_MenuWatch.OffFor(f.Id);
			if (off >= 0)
				return INT_PTY_Text.Fmt("#STR_INT_PTY_W_ST_SEEN", INT_PTY_MenuGeo.Span(off));
			if (f.Since != "")
				return INT_PTY_Text.Fmt("#STR_INT_PTY_W_ST_SINCE", INT_PTY_MenuGeo.DateOf(f.Since));
			return "";
		}
		if (state == 2)
		{
			float dead = INT_PTY_MenuWatch.DeadFor(f.Id);
			if (dead >= 0)
				return INT_PTY_MenuGeo.Ago(dead);
			return "";
		}
		if (state == 1)
			return INT_PTY_Text.Fmt("#STR_INT_PTY_W_ST_LOW", "20%");
		if (!shown || !f.HasState)
			return "";
		if (f.HasFlag(INT_PTY_Const.F_UNCONSCIOUS))
			return INT_PTY_Text.Tr("#STR_INT_PTY_W_ST_UNCON");
		if (f.HasFlag(INT_PTY_Const.F_BLEEDING))
			return INT_PTY_Text.Tr("#STR_INT_PTY_W_ST_BLEED");
		if (f.HasFlag(INT_PTY_Const.F_BROKEN_LEG))
			return INT_PTY_Text.Tr("#STR_INT_PTY_W_ST_LEG");
		return "";
	}

	// The orange connector from the selected row to the detail panel (.conn).
	protected void PlaceConnector()
	{
		if (!m_Conn || !m_List)
			return;
		int idx = IndexOf(m_SelId);
		if (idx < 0 || m_List.ItemFade(idx) < 0.5)
		{
			m_Conn.Show(false);
			return;
		}
		m_Conn.Show(true);
		INT_UIF_Draw.Move(m_Conn, 0, m_List.ItemY(idx) + 33);
	}

	// --- detail ----------------------------------------------------------------------------------------------------
	protected void BuildDetail(bool animate, float d)
	{
		INT_PTY_Friend f = INT_PTY_Client.Friend(m_SelId);
		if (m_Det)
		{
			m_Menu.DropHitsUnder(INT_PTY_Menu.SEC_PAGE, m_Det);
			m_Det.Unlink();
			m_Det = null;
		}
		if (m_Conn)
		{
			m_Conn.Unlink();
			m_Conn = null;
		}
		m_DTiles.Clear();
		m_DTileIcons.Clear();
		m_DSig = "";
		m_DStatusSig = "";
		m_DEdit = null;
		m_Det = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_GDet");
		Widget p = m_Det;
		INT_PTY_MD.Line(p, 1120, 244, 1, 740, INT_PTY_MC.L);
		if (!f)
			return;
		// connector (a frame moved to the row's centre line)
		m_Conn = INT_UIF_Draw.Frame(m_P, 0, 0, 1200, 12, "INT_PTY_GConn");
		ImageWidget connLine = INT_PTY_MD.Line(m_Conn, 1040, 0, 80, 1, INT_PTY_MC.O);
		INT_PTY_MD.Dm(m_Conn, 1120, 0, 10, INT_PTY_MC.O);
		PlaceConnector();
		int idx = IndexOf(f.Id) + 1;
		INT_PTY_MD.Caps(p, DX, 244, 600, 20, INT_PTY_Text.Fmt("#STR_INT_PTY_W_G_SELECTED", INT_PTY_Text.Pad2(idx)), 13, INT_PTY_MC.O);
		string name = INT_PTY_Client.NameOf(f.Id);
		float ns = INT_PTY_Look.FitSize(name, 100, false, 660, 40);
		m_DName = INT_PTY_MD.Txt(p, DX - 6, 262, 700, 110, name, ns, INT_PTY_MC.T, INT_UIF_Align.LEFT, false, true);
		m_DNick = INT_PTY_MD.Txt(p, DX, 376, 400, 26, f.Nick, 19, INT_PTY_MC.M);
		m_DStatus = INT_UIF_Draw.Frame(p, 0, 0, 1920, 1080, "INT_PTY_GDetSt");
		// stats tiles
		INT_PTY_MD.Line(p, DX, 426, 672, 1, INT_PTY_MC.L);
		INT_PTY_MD.Line(p, DX, 526, 672, 1, INT_PTY_MC.L);
		INT_PTY_MD.Line(p, 1376, 426, 1, 100, INT_PTY_MC.L);
		INT_PTY_MD.Line(p, 1608, 426, 1, 100, INT_PTY_MC.L);
		m_DHp = INT_PTY_MD.Txt(p, DX, 436, 150, 58, "—", 48, INT_PTY_MC.T, INT_UIF_Align.LEFT, false, true);
		m_DHpUnit = INT_PTY_MD.Txt(p, DX + 70, 452, 60, 32, "%", 26, INT_PTY_MC.M);
		INT_PTY_MD.Caps(p, DX, 494, 200, 20, "#STR_INT_PTY_W_G_HP", 13, INT_PTY_MC.M);
		m_DDist = INT_PTY_MD.Txt(p, 1400, 436, 160, 58, "—", 48, INT_PTY_MC.T, INT_UIF_Align.LEFT, false, true);
		m_DDistUnit = INT_PTY_MD.Txt(p, 1480, 452, 60, 32, "", 26, INT_PTY_MC.M);
		INT_PTY_MD.Caps(p, 1400, 494, 200, 20, "#STR_INT_PTY_W_G_DIST", 13, INT_PTY_MC.M);
		m_DArrow = INT_PTY_MD.Ico(p, 1632, 446, 34, "arrow_up", INT_PTY_MC.O);
		m_DDir = INT_PTY_MD.Txt(p, 1676, 436, 160, 58, "—", 48, INT_PTY_MC.T, INT_UIF_Align.LEFT, false, true);
		INT_PTY_MD.Caps(p, 1632, 494, 200, 20, "#STR_INT_PTY_W_G_DIR", 13, INT_PTY_MC.M);
		// rename
		INT_PTY_FriendPref pref = INT_PTY_SettingsStore.Get().PrefOrNew(f.Id);
		INT_PTY_MD.Caps(p, DX, 560, 300, 20, "#STR_INT_PTY_W_G_RENAME", 13, INT_PTY_MC.M);
		ButtonWidget fieldHit = m_Menu.Hit(p, DX, 584, 300, 56, "rename");
		m_Menu.SetTip(fieldHit, "#STR_INT_PTY_W_TIP_RENAME_H", "#STR_INT_PTY_W_TIP_RENAME", 360);
		m_DPlaceholder = INT_PTY_MD.Txt(p, DX, 590, 270, 48, f.Nick, 22, INT_PTY_MC.D);
		m_DEdit = INT_UIF_Draw.Edit(p, DX, 590, 266, 48, 22);
		if (m_DEdit)
		{
			m_DEdit.SetColor(ARGB(255, 255, 255, 255));
			m_DEdit.SetTextColor(INT_PTY_MC.T);
			m_DEdit.SetText(pref.Alias);
			m_Menu.RegisterEdit(m_DEdit);
		}
		if (m_DPlaceholder)
			m_DPlaceholder.Show(pref.Alias == "");
		INT_PTY_MD.Ico(p, DX + 280, 605, 18, "edit", INT_PTY_MC.M);
		INT_PTY_MD.Line(p, DX, 637, 300, 1, INT_PTY_MC.L2);
		INT_PTY_MD.Line(p, DX, 637, 120, 1, INT_PTY_MC.O);
		INT_PTY_MD.Txt(p, DX, 648, 340, 22, "#STR_INT_PTY_W_G_RENAME_HINT", 14, INT_PTY_MC.M);
		// show on screen
		INT_PTY_MD.Caps(p, DX, 700, 250, 20, "#STR_INT_PTY_W_G_SHOW", 13, INT_PTY_MC.LABEL);
		m_DShow = INT_PTY_MToggle.Create(p, DX + 264, 697, 50, 26, INT_PTY_Client.IsShown(f.Id));
		ButtonWidget showHit = m_Menu.Hit(p, DX, 690, 320, 40, "dshow");
		m_Menu.SetTip(showHit, "#STR_INT_PTY_W_TIP_SHOW_H", "#STR_INT_PTY_W_TIP_SHOW", 340);
		// remove
		INT_PTY_MButton rm = INT_PTY_MButton.Create(p, DX, 744, 44, "user_minus", "#STR_INT_PTY_W_G_REMOVE", INT_PTY_MButton.DG);
		m_RemoveBtn = rm;
		ButtonWidget rmHit = m_Menu.Hit(rm.m_Root, 0, 0, rm.m_W, rm.m_H, "remove");
		m_Menu.SetHoverBtn(rmHit, rm);
		m_Menu.SetTip(rmHit, "#STR_INT_PTY_W_TIP_REMOVE_H", "#STR_INT_PTY_W_TIP_REMOVE", 340);
		// pings of this friend: 6 shapes + 8 colours + preview
		float rx = 1520;
		INT_PTY_MD.Caps(p, rx, 560, 320, 20, "#STR_INT_PTY_W_G_PINGS", 13, INT_PTY_MC.M);
		int shape = INT_PTY_MenuGeo.PingShapeOf(f.Id);
		for (int i = 0; i < INT_PTY_Shape.COUNT; i++)
		{
			float tx = rx + i * 54;
			INT_UIF_Surface tile = INT_PTY_MD.Box(p, tx, 584, 46, 46, 0, INT_PTY_MC.L2);
			m_DTiles.Insert(tile);
			ImageWidget ic = INT_PTY_MD.Ping(p, tx + 23, 607, 24, i, INT_PTY_MC.M, false);
			m_DTileIcons.Insert(ic);
			ButtonWidget th = m_Menu.Hit(p, tx, 584, 46, 46, "pshape:" + i.ToString());
			m_Menu.SetTip(th, INT_PTY_MenuGeo.ShapeLabel(i), "#STR_INT_PTY_W_TIP_SHAPE", 320);
		}
		m_DShapeName = INT_PTY_MD.Txt(p, rx, 640, 320, 22, "", 14, INT_PTY_MC.M);
		m_DSwRing = INT_PTY_MD.DmLine(p, rx + 15, 690, 28, 0xFFFFFFFF);
		for (int j = 0; j < 8; j++)
		{
			float sx = rx + 15 + j * 34;
			INT_PTY_MD.Dm(p, sx, 690, 18, INT_PTY_MenuGeo.Swatch(j));
			ButtonWidget sh = m_Menu.Hit(p, sx - 16, 674, 32, 32, "pcolor:" + j.ToString());
			m_Menu.SetTip(sh, "#STR_INT_PTY_W_TIP_COLOR_H", "#STR_INT_PTY_W_TIP_COLOR", 320);
		}
		m_DPrevBox = INT_UIF_Draw.Frame(p, rx, 720, 44, 44, "INT_PTY_GPrev");
		INT_PTY_MD.Caps(p, rx + 62, 722, 280, 20, "#STR_INT_PTY_W_G_PING_PREVIEW", 13, INT_PTY_MC.M);
		m_DPrevText = INT_PTY_MD.Txt(p, rx + 62, 744, 280, 22, "", 15, INT_PTY_MC.T);
		RepaintDetail(true);
		if (animate)
		{
			INT_PTY_MenuFx.BlockIn(p, 0, 18, d, 0.36);
			if (m_Conn)
			{
				INT_PTY_MenuFx.GrowW(connLine, d + 0.14, 0.28);
				INT_PTY_MenuFx.FadeIn(m_Conn, d + 0.12, 0.2);
			}
		}
	}

	protected ref INT_PTY_MButton m_RemoveBtn;

	protected void RepaintDetail(bool force)
	{
		INT_PTY_Friend f = INT_PTY_Client.Friend(m_SelId);
		if (!f || !m_Det)
			return;
		bool shown = INT_PTY_Client.IsShown(f.Id);
		int state = RowState(f, shown);
		// status next to the nick
		string st = "#STR_INT_PTY_W_ST_ONLINE";
		int stc = INT_PTY_MC.G;
		if (state == 2)
		{
			st = "#STR_INT_PTY_W_ST_DEAD";
			stc = INT_PTY_MC.R;
		}
		else if (state == 3)
		{
			st = "#STR_INT_PTY_W_ST_OFFLINE";
			stc = INT_PTY_MC.M;
		}
		string stSig = st + f.Nick;
		if (force || stSig != m_DStatusSig)
		{
			m_DStatusSig = stSig;
			INT_UIF_Widgets.ClearChildren(m_DStatus);
			float nw = INT_PTY_MD.Measure(m_DStatus, f.Nick, 19);
			float sx = DX + nw + 18;
			if (state == 2)
				INT_PTY_MD.Ico(m_DStatus, sx - 2, 381, 15, "skull", stc);
			else if (state == 3)
				INT_PTY_MD.Dot(m_DStatus, sx + 4, 389, 8, INT_PTY_MC.OFF);
			else
				INT_PTY_MD.Dot(m_DStatus, sx + 4, 389, 8, INT_PTY_MC.G);
			INT_PTY_Look.Text(m_DStatus, sx + 17, 379, 300, 20, INT_PTY_Text.Caps(st), 13, stc, INT_UIF_Align.LEFT, true);
		}
		// HP / distance / direction
		string hpTxt = "—";
		bool hasHp = shown && f.Online && !f.Dead && f.HasState;
		if (hasHp)
			hpTxt = f.Hp.ToString();
		string dNum = "—";
		string dUnit = "";
		string dirTxt = "—";
		float bearing = 0;
		bool hasDist = false;
		vector me;
		if (f.Online && f.KnowsPos() && INT_PTY_MenuGeo.MyPos(me))
		{
			vector there = f.WorldPos();
			INT_PTY_MenuGeo.DistParts(INT_PTY_MenuGeo.Flat(me, there), dNum, dUnit);
			bearing = INT_PTY_MenuGeo.Bearing(me, there);
			dirTxt = INT_PTY_MenuGeo.CompassOf(bearing);
			hasDist = true;
		}
		string pingName = INT_PTY_Client.NameOf(f.Id);
		string prevTxt = pingName;
		if (hasDist)
			prevTxt = pingName + " · " + dNum + " " + dUnit;
		int shape = INT_PTY_MenuGeo.PingShapeOf(f.Id);
		int pcol = INT_PTY_MenuGeo.PingColorOf(f.Id);
		int bearI = (int)bearing;
		int shownI = 0;
		if (shown)
			shownI = 1;
		string sig = hpTxt + "|" + dNum + dUnit + "|" + dirTxt + "|" + bearI.ToString();
		sig = sig + "|" + prevTxt + "|" + shape.ToString() + "|" + pcol.ToString() + "|" + shownI.ToString();
		if (!force && sig == m_DSig)
			return;
		m_DSig = sig;
		int hpc = INT_PTY_MC.T;
		if (hasHp && f.Hp < 20)
			hpc = INT_PTY_MC.R;
		if (m_DHp)
		{
			m_DHp.SetText(hpTxt);
			m_DHp.SetColor(hpc);
		}
		if (m_DHpUnit)
		{
			m_DHpUnit.Show(hasHp);
			float hpw = INT_PTY_MD.Measure(m_Det, hpTxt, 48, false, true);
			INT_UIF_Draw.Move(m_DHpUnit, DX + hpw + 10, 452);
		}
		if (m_DDist)
			m_DDist.SetText(dNum);
		if (m_DDistUnit)
		{
			m_DDistUnit.Show(hasDist);
			m_DDistUnit.SetText(dUnit);
			float dnw = INT_PTY_MD.Measure(m_Det, dNum, 48, false, true);
			INT_UIF_Draw.Move(m_DDistUnit, 1400 + dnw + 10, 452);
		}
		if (m_DArrow)
		{
			m_DArrow.Show(hasDist);
			m_DArrow.SetRotation(0, 0, bearing);
		}
		if (m_DDir)
		{
			m_DDir.SetText(dirTxt);
			if (hasDist)
				INT_UIF_Draw.Move(m_DDir, 1676, 436);
			else
				INT_UIF_Draw.Move(m_DDir, 1632, 436);
		}
		if (m_DShow && m_DShow.m_On != shown)
			m_DShow.Set(shown, true);
		// ping look
		for (int i = 0; i < m_DTiles.Count(); i++)
		{
			INT_UIF_Surface tile = m_DTiles.Get(i);
			ImageWidget ic = m_DTileIcons.Get(i);
			bool on = i == shape;
			if (tile)
			{
				if (on)
				{
					tile.SetFill(INT_PTY_MC.O_SOFT);
					tile.SetBorder(INT_PTY_MC.O);
				}
				else
				{
					tile.SetFill(0);
					tile.SetBorder(INT_PTY_MC.L2);
				}
			}
			if (ic)
			{
				if (on)
					ic.SetColor(INT_PTY_MC.O);
				else
					ic.SetColor(INT_PTY_MC.M);
			}
		}
		string shapeA = INT_PTY_Text.Tr("#STR_INT_PTY_W_G_SHAPE");
		string shapeB = INT_PTY_MenuGeo.ShapeLabel(shape);
		if (m_DShapeName)
			m_DShapeName.SetText(shapeA + " " + shapeB);
		int sw = INT_PTY_MenuGeo.SwatchIndex(pcol);
		if (m_DSwRing)
		{
			m_DSwRing.Show(sw >= 0 && sw < 8);
			if (sw >= 0 && sw < 8)
				INT_PTY_Look.DiamondPlace(m_DSwRing, 1520 + 15 + sw * 34, 690, 28);
		}
		if (m_DPrevBox)
		{
			INT_UIF_Widgets.ClearChildren(m_DPrevBox);
			INT_PTY_MD.Box(m_DPrevBox, 0, 0, 44, 44, 0, INT_PTY_MC.L);
			INT_PTY_MD.Ping(m_DPrevBox, 22, 22, 22, shape, pcol, true);
		}
		if (m_DPrevText)
			m_DPrevText.SetText(prevTxt);
	}

	// --- events -------------------------------------------------------------------------------------------------
	override bool OnClick(string key, Widget w)
	{
		if (key.IndexOf("row:") == 0)
		{
			string ss = key.Substring(4, key.Length() - 4);
			int slot = ss.ToInt();
			INT_PTY_Friend f = FriendAt(m_List.SlotIndex(slot));
			if (!f)
				return true;
			Select(f.Id);
			return true;
		}
		if (key.IndexOf("eye:") == 0)
		{
			string es = key.Substring(4, key.Length() - 4);
			int eslot = es.ToInt();
			INT_PTY_Friend ef = FriendAt(m_List.SlotIndex(eslot));
			if (ef)
				ToggleShown(ef.Id, eslot);
			return true;
		}
		if (key == "rename")
		{
			m_Menu.FocusEdit();
			return true;
		}
		if (key == "dshow")
		{
			ToggleShown(m_SelId, -1);
			return true;
		}
		if (key == "remove")
		{
			INT_PTY_Sfx.Click();
			m_Menu.AskRemove(m_SelId);
			return true;
		}
		if (key.IndexOf("pshape:") == 0)
		{
			string shs = key.Substring(7, key.Length() - 7);
			int shape = shs.ToInt();
			INT_PTY_FriendPref p = INT_PTY_SettingsStore.Get().PrefOrNew(m_SelId);
			p.PingShape = shape;
			Changed();
			INT_PTY_Sfx.Click();
			return true;
		}
		if (key.IndexOf("pcolor:") == 0)
		{
			string cs = key.Substring(7, key.Length() - 7);
			int ci = cs.ToInt();
			INT_PTY_FriendPref pc = INT_PTY_SettingsStore.Get().PrefOrNew(m_SelId);
			int ncol = INT_PTY_MenuGeo.Swatch(ci);
			// owner D19: the friend has ONE colour everywhere (list, tag, tracker, map, his pings)
			pc.Color = ncol;
			pc.PingColor = 0;
			Changed();
			INT_PTY_Sfx.Click();
			return true;
		}
		return false;
	}

	protected void Changed()
	{
		INT_PTY_SettingsStore.MarkDirty();
		INT_PTY_Hud.Refresh();
		RepaintDetail(true);
	}

	protected void Select(string id)
	{
		if (id == m_SelId)
			return;
		m_Menu.BlurEdit();
		INT_PTY_Sfx.Click();
		m_SelId = id;
		s_SelId = id;
		RepaintRows(false);
		BuildDetail(true, 0);
	}

	protected void ToggleShown(string id, int slot)
	{
		if (id == "")
			return;
		bool now = INT_PTY_Client.IsShown(id);
		if (!INT_PTY_Client.SetShown(id, !now))
		{
			// the server limit is full: the eye shakes (the limit toast comes from the client core)
			if (slot >= 0 && slot < m_Rows.Count())
				INT_PTY_Fx.Shake(m_Rows.Get(slot).EyeIcon);
			return;
		}
		INT_PTY_Sfx.Toggle();
		INT_PTY_Hud.Refresh();
		RepaintCounters(true);
		RepaintRows(false);
		RepaintDetail(true);
	}

	override bool OnWheel(string key, int wheel)
	{
		if (key.IndexOf("row:") != 0 && key.IndexOf("eye:") != 0 && key.IndexOf("glist:") != 0)
			return false;
		if (m_List)
			m_List.Wheel(wheel);
		return true;
	}

	override bool OnDown(string key)
	{
		if (key != "glist:bar" || !m_List)
			return false;
		m_List.BarDown();
		return true;
	}

	override void OnSlide(string key, int mx, bool done)
	{
		if (key == "glist:bar" && m_List)
			m_List.BarDrag(done);
	}

	override void TestScroll(float px)
	{
		if (m_List)
			m_List.Jump(px);
	}

	override void OnEditLive(string text)
	{
		if (!m_DEdit)
			return;
		string t = text;
		if (t.LengthUtf8() > INT_PTY_Const.MAX_NAME_CHARS)
		{
			t = t.SubstringUtf8(0, INT_PTY_Const.MAX_NAME_CHARS);
			m_DEdit.SetText(t);
		}
		if (m_DPlaceholder)
			m_DPlaceholder.Show(t == "");
		INT_PTY_Friend f = INT_PTY_Client.Friend(m_SelId);
		if (!f || !m_DName)
			return;
		string shownName = t;
		if (shownName == "")
			shownName = f.Nick;
		float ns = INT_PTY_Look.FitSize(shownName, 100, false, 660, 40);
		m_DName.SetTextExactSize(INT_UIF_Scale.TextPx(ns));
		m_DName.SetText(shownName);
	}

	override void OnEditDone(string text)
	{
		INT_PTY_Friend f = INT_PTY_Client.Friend(m_SelId);
		if (!f)
			return;
		string clean = INT_PTY_Util.CleanName(text);
		if (clean == f.Nick)
			clean = "";
		INT_PTY_FriendPref p = INT_PTY_SettingsStore.Get().PrefOrNew(f.Id);
		if (p.Alias == clean)
			return;
		p.Alias = clean;
		INT_PTY_SettingsStore.MarkDirty();
		INT_PTY_Hud.Refresh();
		INT_PTY_Sfx.Click();
		RepaintRows(true);
		RepaintDetail(true);
	}

	override void OnData(int what)
	{
		if ((what & INT_PTY_What.FRIENDS) != 0)
		{
			int n = Count();
			bool selGone = m_SelId != "" && IndexOf(m_SelId) < 0;
			if ((n == 0) != m_Empty || selGone || m_Segs.Count() != INT_PTY_Client.s_DisplayLimit)
			{
				if (m_List)
					m_SavedOff = m_List.Offset();
				m_Menu.RebuildPage(false);
				return;
			}
			if (n != m_FriendCount && m_List)
				m_List.SetCount(n, true);
			m_FriendCount = n;
			RepaintCounters(true);
			RepaintRows(false);
			RepaintDetail(false);
			return;
		}
		if ((what & INT_PTY_What.STATE) != 0 && !m_Empty)
		{
			RepaintRows(false);
			RepaintDetail(false);
		}
	}

	override void Tick(float dt)
	{
		if (m_Empty)
			return;
		if (m_List)
			m_List.Tick(dt);
		m_Acc = m_Acc + dt;
		if (m_Acc < 0.5)
			return;
		m_Acc = 0;
		RepaintRows(false);
		RepaintDetail(false);
	}

	override void OnLeave()
	{
		s_SelId = m_SelId;
	}

	override string TestInfo()
	{
		int shown = INT_PTY_Client.ShownCount();
		int rows = 0;
		float off = 0;
		if (m_List)
		{
			off = m_List.Offset();
			for (int i = 0; i < m_Rows.Count(); i++)
			{
				int idx = m_List.SlotIndex(i);
				if (idx >= 0 && m_List.ItemFade(idx) > 0.5)
					rows++;
			}
		}
		int offI = (int)off;
		string s = "group friends=" + Count().ToString() + " rows=" + rows.ToString() + " shown=" + shown.ToString();
		s = s + " sel=" + m_SelId + " scroll=" + offI.ToString();
		if (m_Empty)
			s = s + " empty";
		return s;
	}

	// Test seam: select a friend by index of the list.
	void TestSelect(int idx)
	{
		INT_PTY_Friend f = FriendAt(idx);
		if (!f)
			return;
		if (m_List)
			m_List.Reveal(idx);
		Select(f.Id);
	}

	string SelectedId()
	{
		return m_SelId;
	}
}
