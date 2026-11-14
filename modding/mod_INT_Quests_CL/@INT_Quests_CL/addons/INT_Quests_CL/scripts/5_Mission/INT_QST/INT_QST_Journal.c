// INT Quests (client) — the journal (key P, design B «Полевой дневник», design/concepts_v1/2_b_diary_journal.png).
// Tabs: Активные (taken quests by branch, pushpin = show in the HUD tracker), Доступные (what can be taken now and
// where; quests without a giver are taken right here, owner decision O13), Выполненные, Администрация (admins only).
// Left page: the list (pooled rows, scrolls) and a sticky note with numbers; right page: the quest sheet.

// Plain lines (problems list).
class INT_QST_LineSlot extends INT_QST_VSlot
{
	TextWidget m_Text;
}

class INT_QST_LinesSource extends INT_QST_VSource
{
	ref array<string> m_Lines = new array<string>();

	override int Count()
	{
		return m_Lines.Count();
	}

	override float RowHeight(int i)
	{
		return 24;
	}

	override INT_QST_VSlot NewSlot()
	{
		return new INT_QST_LineSlot();
	}

	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_LineSlot s = INT_QST_LineSlot.Cast(slot);
		int c = INT_QST_Look.RedA(200);
		INT_QST_Look.Icon(s.m_Root, 2, 5, 13, "warning", c);
		s.m_Text = INT_QST_Look.Text(s.m_Root, 22, 0, w - 24, 24, "", 13, INT_QST_Look.Ink2());
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_LineSlot s = INT_QST_LineSlot.Cast(slot);
		if (s.m_Text)
			s.m_Text.SetText(m_Lines[i]);
	}
}

class INT_QST_ListRow
{
	bool Group;
	string Text;		// group: branch title
	int QuestId;
	bool Gone;		// taken, but an admin deleted the quest (not in the catalog any more)
}

class INT_QST_QuestSlot extends INT_QST_VSlot
{
	Widget m_Box;
	ButtonWidget m_BtnHit;	// pushpin / «Убрать» of the row: made once per slot and re-bound (the window's
				// click map would grow with a new hit on every bind)
}

class INT_QST_QuestListSource extends INT_QST_VSource
{
	ref array<ref INT_QST_ListRow> m_Rows = new array<ref INT_QST_ListRow>();
	INT_QST_Journal m_Owner;
	int m_Selected;			// quest id
	int m_Tab;

	override int Count()
	{
		return m_Rows.Count();
	}

	override float RowHeight(int i)
	{
		if (m_Rows[i].Group)
		{
			if (i == 0)
				return 34;
			return 52;
		}
		return 66;
	}

	override INT_QST_VSlot NewSlot()
	{
		return new INT_QST_QuestSlot();
	}

	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_QuestSlot s = INT_QST_QuestSlot.Cast(slot);
		s.m_Box = INT_UIF_Draw.Frame(s.m_Root, 0, 0, w, 66, "Row");
		// after the box: on top of everything the bind draws into it
		s.m_BtnHit = INT_UIF_Draw.Hit(s.m_Root, w - 54, 14, 38, 38, m_Owner.Clicks());
		if (s.m_BtnHit)
			s.m_BtnHit.Show(false);
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_QuestSlot s = INT_QST_QuestSlot.Cast(slot);
		INT_UIF_Widgets.ClearChildren(s.m_Box);
		float h = RowHeight(i);
		INT_UIF_Draw.Resize(s.m_Box, w, h);
		s.m_Hover = null;
		if (s.m_BtnHit)
			s.m_BtnHit.Show(false);
		INT_QST_ListRow r = m_Rows[i];
		if (r.Group)
		{
			float gy = h - 32;
			INT_UIF_Draw.Line(s.m_Box, 18, gy + 12, 8, 8, INT_QST_Look.Ink2());
			string cap = INT_QST_Look.Spaced(r.Text);
			INT_QST_Look.Heavy(s.m_Box, 34, gy + 4, w - 40, 22, cap, 12, INT_QST_Look.Ink2());
			INT_QST_Look.Hair(s.m_Box, 14, h - 1, w - 20);
			return;
		}
		if (r.Gone)
		{
			BindGone(s, r, w, h);
			return;
		}
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(r.QuestId);
		if (!q)
			return;
		bool sel = r.QuestId == m_Selected;
		Widget hover = INT_UIF_Draw.Frame(s.m_Box, 6, 2, w - 6, h - 4, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, w - 6, h - 4, 0x1F2A2218);
		s.m_Hover = hover;
		if (sel)
		{
			INT_QST_Look.Tex(s.m_Box, 6, 2, w - 6, h - 4, "int_qst_highlight_ca", INT_QST_Look.Marker());
			INT_UIF_Draw.Line(s.m_Box, 6, 2, 3, h - 4, INT_QST_Look.Ink());
		}
		int wait;
		int st = INT_QST_Client.StatusOf(q, wait);
		int nameColor = INT_QST_Look.Ink();
		if (st == INT_QST_St.COOLDOWN || st == INT_QST_St.LOCKED)
			nameColor = INT_QST_Look.Ink3();
		float nameW = w - 90;
		string nm = INT_QST_Look.Fit(q.Name, nameW, 17);
		INT_QST_Look.Heavy(s.m_Box, 20, 6, nameW, 26, nm, 17, nameColor);
		// stamp + one chip
		string sk = INT_QST_Text.StatusKey(st);
		int sc = INT_QST_Text.StatusColor(st);
		string sicon = "";
		if (st == INT_QST_St.READY)
			sicon = "check";
		INT_QST_Look.Stamp(s.m_Box, 20, 36, sk, sc, 10, sicon);
		float sw = INT_QST_Look.StampWidth(sk, 10, sicon != "");
		string chip = ChipText(q, st, wait);
		string chipIcon = ChipIcon(q, st);
		if (chip != "")
		{
			float chipMax = w - 70 - (20 + sw + 8) - 34;
			chip = INT_QST_Look.Fit(chip, chipMax, 12);
			float cw;
			INT_QST_Look.Chip(s.m_Box, 20 + sw + 8, 35, chipIcon, chip, cw);
		}
		// pushpin (active tab) / sign (available tab)
		if (m_Tab == INT_QST_Journal.TAB_ACTIVE)
		{
			bool tracked = INT_QST_Client.IsTracked(q.Id);
			INT_QST_Look.PinTile(s.m_Box, w - 54, 14, 38, tracked);
			if (s.m_BtnHit)
			{
				INT_UIF_Draw.Place(s.m_BtnHit, w - 54, 14, 38, 38);
				s.m_BtnHit.Show(true);
				INT_QST_Clicks pinClicks = m_Owner.Clicks();
				pinClicks.Bind(s.m_BtnHit, INT_QST_Journal.ID_PIN, q.Id);
			}
		}
		else if (m_Tab == INT_QST_Journal.TAB_AVAIL)
		{
			string sign = "avail";
			if (st == INT_QST_St.COOLDOWN)
				sign = "cooldown";
			INT_QST_Look.Sign(s.m_Box, w - 50, 16, 34, sign);
		}
		INT_QST_Look.Hair(s.m_Box, 14, h - 1, w - 20);
	}

	// A taken quest an admin deleted: it still takes a slot on the server until the player removes it here.
	protected void BindGone(INT_QST_QuestSlot s, INT_QST_ListRow r, float w, float h)
	{
		bool sel = r.QuestId == m_Selected;
		Widget hover = INT_UIF_Draw.Frame(s.m_Box, 6, 2, w - 6, h - 4, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, w - 6, h - 4, 0x1F2A2218);
		s.m_Hover = hover;
		if (sel)
		{
			INT_QST_Look.Tex(s.m_Box, 6, 2, w - 6, h - 4, "int_qst_highlight_ca", INT_QST_Look.Marker());
			INT_UIF_Draw.Line(s.m_Box, 6, 2, 3, h - 4, INT_QST_Look.Ink());
		}
		float bw = 116;
		float nameW = w - bw - 44;
		string gone = INT_QST_Look.Tr("#STR_INT_QST_FB_GONE");
		gone = INT_QST_Look.Fit(gone, nameW, 16);
		INT_QST_Look.Heavy(s.m_Box, 20, 6, nameW, 26, gone, 16, INT_QST_Look.Ink3());
		int rc = INT_QST_Look.Red();
		INT_QST_Look.Stamp(s.m_Box, 20, 36, "#STR_INT_QST_FB_GONE_STAMP", rc, 10, "trash");
		float sw = INT_QST_Look.StampWidth("#STR_INT_QST_FB_GONE_STAMP", 10, true);
		string idText = "ID " + r.QuestId.ToString();
		INT_QST_Look.Text(s.m_Box, 20 + sw + 10, 35, 120, 22, idText, 12, INT_QST_Look.Ink3());
		Widget hv;
		INT_QST_Look.Button(s.m_Box, w - bw - 14, 16, bw, 34, "#STR_INT_QST_GV_REMOVE", "", INT_QST_Look.BTN_DANGER, 0, hv);
		if (s.m_BtnHit)
		{
			INT_UIF_Draw.Place(s.m_BtnHit, w - bw - 14, 16, bw, 34);
			s.m_BtnHit.Show(true);
			INT_QST_Clicks goneClicks = m_Owner.Clicks();
			goneClicks.Bind(s.m_BtnHit, INT_QST_Journal.ID_DISMISS, r.QuestId, hv);
		}
		INT_QST_Look.Hair(s.m_Box, 14, h - 1, w - 20);
	}

	// The chip on the row: progress, time left, distance to the giver, how many times done.
	protected string ChipText(INT_QST_QuestDef q, int st, int wait)
	{
		if (m_Tab == INT_QST_Journal.TAB_DONE)
		{
			INT_QST_Done d = INT_QST_Client.s_State.FindDone(q.Id);
			if (st == INT_QST_St.COOLDOWN && wait > 0)
				return INT_QST_Look.Dur(wait);
			if (d && d.Times > 1)
				return "× " + d.Times.ToString();
			return "";
		}
		if (st == INT_QST_St.COOLDOWN && wait > 0)
			return INT_QST_Look.Dur(wait);
		if ((st == INT_QST_St.ACTIVE || st == INT_QST_St.READY) && wait >= 0)
			return INT_QST_Look.Dur(wait);
		if (st == INT_QST_St.ACTIVE)
		{
			int pct = (int)Math.Round(INT_QST_Sheet.ProgressPct(q));
			return pct.ToString() + " %";
		}
		if (st == INT_QST_St.READY)
		{
			string fin = q.GiverFinish;
			return INT_QST_Text.GiverDistance(fin);
		}
		if (st == INT_QST_St.AVAILABLE)
		{
			if (q.GiverStart == "")
				return INT_QST_Look.Tr("#STR_INT_QST_FROM_JOURNAL");
			string g = INT_QST_Text.GiverName(q.GiverStart);
			string dist = INT_QST_Text.GiverDistance(q.GiverStart);
			if (dist != "")
				return g + " · " + dist;
			return g;
		}
		return "";
	}

	protected string ChipIcon(INT_QST_QuestDef q, int st)
	{
		if (st == INT_QST_St.COOLDOWN)
			return "clock";
		if ((st == INT_QST_St.ACTIVE || st == INT_QST_St.READY) && q.TimeLimit > 0)
			return "clock";
		if (st == INT_QST_St.ACTIVE)
			return "list";
		if (st == INT_QST_St.AVAILABLE && q.GiverStart == "")
			return "book";
		if (m_Tab == INT_QST_Journal.TAB_DONE)
			return "check";
		return "map_pin";
	}

	override void OnRowClick(int i, int button)
	{
		if (i < 0 || i >= m_Rows.Count())
			return;
		INT_QST_ListRow r = m_Rows[i];
		if (r.Group)
			return;
		if (button == MouseState.RIGHT)
		{
			if (m_Tab == INT_QST_Journal.TAB_ACTIVE)
				m_Owner.ToggleTrack(r.QuestId);
			return;
		}
		m_Owner.Select(r.QuestId);
	}
}

class INT_QST_Journal extends INT_QST_Book
{
	static const float FOLD = 520;

	static const int TAB_ACTIVE = 0;
	static const int TAB_AVAIL = 1;
	static const int TAB_DONE = 2;
	static const int TAB_ADMIN = 3;
	static const int TAB_SETTINGS = 4;

	static const int ID_TAB = 1;
	static const int ID_PIN = 2;
	static const int ID_TRACK = 3;
	static const int ID_CANCEL = 4;
	static const int ID_DISMISS = 5;
	static const int ID_TAKE = 6;
	static const int ID_ADMIN_TOOL = 7;
	static const int ID_MOVE_HUD = 8;
	static const int ID_ADMIN_SWITCH = 9;
	static const int ID_TURNIN = 10;	// a ready quest without a finishing giver (it waits for its items)
	static const int ID_SET_INGAME = 11;	// settings: move the tracker over the game view (mover), then back here
	static const int ID_SET_RESET = 12;	// settings: tracker size, transparency and place back to the defaults
	static const int ID_SET_SHOW = 13;	// settings: show / hide the tracker (as the player's key)
	static const int ID_ONMAP = 14;		// a taken quest: show its points on the VPP / RF map (switch only with a map mod)

	static const int ASK_RELOAD = 2;

	static const int ASK_CANCEL = 1;

	static int s_Tab;
	static ref map<int, int> s_SelectedByTab = new map<int, int>();

	protected Widget m_Tabs;
	protected Widget m_SlotsPlate;
	protected Widget m_LeftHead;
	protected Widget m_Sticky;
	protected Widget m_Buttons;
	protected Widget m_AdminPage;
	protected Widget m_SetPage;
	protected ref INT_QST_TrackerSet m_TrackerSet;
	protected bool m_SetSoon;
	protected ref INT_QST_VList m_List;
	protected ref INT_QST_QuestListSource m_Src;
	protected ref INT_QST_Sheet m_Sheet;
	protected ref INT_QST_VList m_ProblemList;
	protected ref INT_QST_LinesSource m_ProblemSrc;
	protected int m_Selected;
	protected float m_LiveAcc;
	protected string m_LiveSig;
	protected bool m_Hooked;
	protected int m_ShapeSeen = -1;		// INT_QST_Client.s_ShapeRevision at the last full rebuild
	// clicks that rebuild the part holding the clicked widget run on the next frame (never unlink a widget
	// inside its own event): a tab, a server switch
	protected bool m_RebuildSoon;
	protected bool m_RebuildTop;
	protected bool m_AdminSoon;

	override void GetWindowSize(out float w, out float h)
	{
		w = 1440;
		h = 840;
	}

	override string ToggleInput()
	{
		return "UAINTQuestsJournal";
	}

	INT_QST_Clicks Clicks()
	{
		return m_Clicks;
	}

	// --- cover ------------------------------------------------------------------------------------------------------
	override void BuildCoverHeader(Widget header, float w)
	{
		// title plate
		Widget plate = INT_UIF_Draw.Frame(header, 24, 14, 300, 54, "TitlePlate");
		int pf = INT_QST_Look.Note();
		int pb = INT_QST_Look.Tan2();
		INT_UIF_Surface.Box(plate, 0, 0, 300, 54, 4, pf, pb);
		INT_QST_Look.Tex(plate, 0, 0, 300, 54, "int_qst_paper_ca", pf);
		INT_QST_Look.Icon(plate, 14, 13, 28, "book", INT_QST_Look.Ink());
		string brand = INT_QST_Look.Spaced("DayZavr Quests");
		INT_QST_Look.Heavy(plate, 54, 6, 240, 18, brand, 11, INT_QST_Look.Red());
		string title = INT_QST_Look.Upper("#STR_INT_QST_JOURNAL");
		INT_QST_Look.Heavy(plate, 54, 22, 240, 28, title, 21, INT_QST_Look.Ink());
		// system name + player
		INT_QST_Look.Text(header, 352, 14, 400, 20, "DayZavr Quest System", 14, INT_QST_Look.Tan());
		INT_QST_Look.Icon(header, 352, 42, 18, "person", INT_QST_Look.Tan());
		string who = INT_QST_Look.Spaced("#STR_INT_QST_STALKER");
		float whoW = INT_UIF_Draw.TextWidth(who, 12) * 1.05 + 10;
		INT_QST_Look.Text(header, 378, 40, whoW, 22, who, 12, INT_QST_Look.Tan2());
		string name = PlayerName();
		INT_QST_Look.Heavy(header, 378 + whoW, 38, 300, 26, name, 17, INT_QST_Look.Cream());
		// slots plate
		m_SlotsPlate = INT_UIF_Draw.Frame(header, w - 80 - 220, 16, 220, 46, "Slots");
		// tabs
		m_Tabs = INT_UIF_Draw.Frame(header, 0, 0, w, CoverTop(), "Tabs");
	}

	protected string PlayerName()
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (p && p.GetIdentity())
			return p.GetIdentity().GetName();
		return "";
	}

	protected void DrawSlots()
	{
		if (!m_SlotsPlate)
			return;
		INT_UIF_Widgets.ClearChildren(m_SlotsPlate);
		int lf = INT_QST_Look.LeatherDark();
		int lb = INT_QST_Look.LeatherLine();
		INT_UIF_Surface.Box(m_SlotsPlate, 0, 0, 220, 46, 6, lf, lb);
		INT_QST_Look.Icon(m_SlotsPlate, 14, 14, 18, "list", INT_QST_Look.Tan());
		string lab = INT_QST_Look.Spaced("#STR_INT_QST_QUESTS");
		INT_QST_Look.Text(m_SlotsPlate, 40, 0, 110, 44, lab, 12, INT_QST_Look.Tan());
		int have = INT_QST_Client.s_State.Active.Count();
		int slots = 0;
		if (INT_QST_Client.s_Hello)
			slots = INT_QST_Client.s_Hello.QuestSlots;
		string v = have.ToString() + " / " + slots.ToString();
		INT_QST_Look.Heavy(m_SlotsPlate, 100, 0, 106, 42, v, 21, INT_QST_Look.Cream(), INT_UIF_Align.RIGHT);
		int track = INT_QST_Look.CreamA(40);
		INT_UIF_Draw.Line(m_SlotsPlate, 14, 39, 192, 2, track);
		float fw = 0;
		if (slots > 0)
			fw = 192.0 * have / slots;
		if (fw > 192)
			fw = 192;
		if (fw > 0.5)
			INT_UIF_Draw.Line(m_SlotsPlate, 14, 39, fw, 2, INT_QST_Look.Tan());
	}

	protected void DrawTabs()
	{
		if (!m_Tabs)
			return;
		m_Clicks.ForgetUnder(m_Tabs);
		INT_UIF_Widgets.ClearChildren(m_Tabs);
		float top = CoverTop();
		float x = 40;
		// call results hoisted: a call argument holding two call results can be clobbered (measured trap)
		string activeCount = CountActive().ToString();
		string slotsCount = SlotsText();
		string activeText = activeCount + " / " + slotsCount;
		string availText = CountAvailable().ToString();
		string doneText = CountDone().ToString();
		float tw = Tab(x, TAB_ACTIVE, "book", "#STR_INT_QST_TAB_ACTIVE", activeText);
		x = x + tw + 6;
		tw = Tab(x, TAB_AVAIL, "star", "#STR_INT_QST_TAB_AVAIL", availText);
		x = x + tw + 6;
		tw = Tab(x, TAB_DONE, "check", "#STR_INT_QST_TAB_DONE", doneText);
		x = x + tw + 6;
		tw = Tab(x, TAB_SETTINGS, "fit", "#STR_INT_QST_TAB_SETTINGS", "");
		x = x + tw + 6;
		if (INT_QST_Client.IsAdmin())
			AdminTab();
	}

	protected string SlotsText()
	{
		if (INT_QST_Client.s_Hello)
			return INT_QST_Client.s_Hello.QuestSlots.ToString();
		return "?";
	}

	// One paper index tab; returns its width. An empty count draws no counter.
	protected float Tab(float x, int tab, string icon, string labelKey, string count)
	{
		bool on = s_Tab == tab;
		float top = CoverTop();
		string label = INT_QST_Look.Tr(labelKey);
		float lw = INT_UIF_Draw.TextWidth(label, 15) * 1.22;
		float cw = INT_UIF_Draw.TextWidth(count, 12) * 1.1 + 14;
		float w = 18 + 18 + 8 + lw + 10 + cw + 16;
		if (count == "")
			w = 18 + 18 + 8 + lw + 18;
		float h = 44;
		float y = top - 40;
		int fill = INT_QST_Look.Paper();
		int ink = INT_QST_Look.Ink();
		if (!on)
		{
			fill = INT_QST_Look.Kraft();
			ink = INT_QST_Look.Ink2();
			h = 38;
			y = top - 34;
		}
		Widget t = INT_UIF_Draw.Frame(m_Tabs, x, y, w, h + 8, "Tab");
		int edge = INT_QST_Look.InkA(90);
		INT_UIF_Surface.Box(t, 0, 0, w, h + 8, 8, fill, edge);
		INT_QST_Look.Tex(t, 0, 0, w, h + 8, "int_qst_paper_ca", fill);
		Widget hover = INT_UIF_Draw.Frame(t, 0, 0, w, h, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, w, h, 0x1AFFFFFF);
		float cy = (h - 18) * 0.5 + 2;
		INT_QST_Look.Icon(t, 18, cy, 18, icon, ink);
		INT_QST_Look.Heavy(t, 44, cy - 3, lw + 4, 24, label, 15, ink);
		if (count != "")
		{
			float bx = 44 + lw + 10;
			int bb = INT_QST_Look.InkA(110);
			INT_UIF_Surface.Box(t, bx, cy - 2, cw, 22, 4, 0x26FFFFFF, bb);
			INT_QST_Look.Heavy(t, bx, cy - 2, cw, 22, count, 12, ink, INT_UIF_Align.CENTER);
		}
		ButtonWidget hit = INT_UIF_Draw.Hit(t, 0, 0, w, h, m_Clicks);
		m_Clicks.Bind(hit, ID_TAB, tab, hover);
		return w;
	}

	// Oxblood tab with a shield, right side, admins only.
	protected void AdminTab()
	{
		bool on = s_Tab == TAB_ADMIN;
		float top = CoverTop();
		string label = INT_QST_Look.Tr("#STR_INT_QST_TAB_ADMIN");
		string only = INT_QST_Look.Tr("#STR_INT_QST_ADMIN_ONLY");
		float lw = INT_UIF_Draw.TextWidth(label, 15) * 1.22;
		float ow = INT_UIF_Draw.TextWidth(only, 11) * 1.1 + 30;
		float w = 18 + 18 + 8 + lw + 12 + ow + 14;
		float h = 36;
		float y = top - 32;
		if (on)
		{
			h = 42;
			y = top - 38;
		}
		float x = m_WinW - 42 - w;
		Widget t = INT_UIF_Draw.Frame(m_Tabs, x, y, w, h + 8, "AdminTab");
		int fill = INT_QST_Look.RedDeep();
		int edge = 0xFF3A110C;
		INT_UIF_Surface.Box(t, 0, 0, w, h + 8, 8, fill, edge);
		Widget hover = INT_UIF_Draw.Frame(t, 0, 0, w, h, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, w, h, 0x1AFFFFFF);
		int tc = 0xFFF2DCC8;
		float cy = (h - 18) * 0.5 + 2;
		INT_QST_Look.Icon(t, 18, cy, 18, "shield", tc);
		INT_QST_Look.Heavy(t, 44, cy - 3, lw + 4, 24, label, 15, tc);
		float bx = 44 + lw + 12;
		int ob = 0x80F2DCC8;
		INT_UIF_Surface.Box(t, bx, cy - 2, ow, 22, 4, 0, ob);
		INT_QST_Look.Icon(t, bx + 7, cy + 3, 12, "lock", tc);
		INT_QST_Look.Text(t, bx + 23, cy - 2, ow - 26, 22, only, 11, tc);
		ButtonWidget hit = INT_UIF_Draw.Hit(t, 0, 0, w, h, m_Clicks);
		m_Clicks.Bind(hit, ID_TAB, TAB_ADMIN, hover);
	}

	// --- body ---------------------------------------------------------------------------------------------------------
	override void OnBuild(Widget body)
	{
		Pages(body, FOLD);
		float bh = GetBodyH();
		float bw = GetBodyW();
		m_LeftHead = INT_UIF_Draw.Frame(body, 0, 0, FOLD, 60, "LeftHead");
		m_Src = new INT_QST_QuestListSource();
		m_Src.m_Owner = this;
		m_List = new INT_QST_VList();
		int bar = INT_QST_Look.InkA(30);
		int thumb = INT_QST_Look.InkA(120);
		m_List.Build(body, 8, 62, FOLD - 30, bh - 62 - 228, m_Src, bar, thumb);
		m_Sticky = INT_UIF_Draw.Frame(body, 44, bh - 214, 436, 196, "Sticky");
		float rx = FOLD + 60;
		float rw = bw - rx - 34;
		m_Sheet = new INT_QST_Sheet();
		m_Sheet.m_NoteH = 146;
		m_Sheet.m_Bottom = 84;
		m_Sheet.Build(body, rx, 0, rw, bh);
		m_Buttons = INT_UIF_Draw.Frame(body, rx, bh - 66, rw, 52, "Buttons");
		m_AdminPage = INT_UIF_Draw.Frame(body, 0, 0, bw, bh, "AdminPage");
		m_AdminPage.Show(false);
		m_SetPage = INT_UIF_Draw.Frame(body, 0, 0, bw, bh, "SettingsPage");
		m_SetPage.Show(false);
		m_TrackerSet = new INT_QST_TrackerSet();
		string pKey = KeyName("UAINTQuestsJournal", "J");
		AddKey(pKey, "");
		AddKeySlash();
		AddKey("ESC", "#STR_INT_QST_K_CLOSE");
		AddKey(INT_QST_Look.Tr("#STR_INT_QST_K_LMB"), "#STR_INT_QST_K_SELECT");
		AddKey(INT_QST_Look.Tr("#STR_INT_QST_K_RMB"), "#STR_INT_QST_K_PIN");
	}

	override void OnReady()
	{
		if (s_Tab == TAB_ADMIN && !INT_QST_Client.IsAdmin())
			s_Tab = TAB_ACTIVE;
		RebuildAll(true);
	}

	override void OnShow()
	{
		super.OnShow();
		if (!m_Hooked)
		{
			INT_QST_Client.s_OnChanged.Insert(OnDataChanged);
			INT_QST_Client.s_OnResult.Insert(OnResult);
			INT_QST_Admin.s_OnReply.Insert(OnAdminReply);
			m_Hooked = true;
		}
		if (INT_QST_Client.IsAdmin())
			INT_QST_Admin.RequestHello();
	}

	override void OnHide()
	{
		if (m_TrackerSet)
			m_TrackerSet.Finish();
		Unhook();
		super.OnHide();
	}

	void ~INT_QST_Journal()
	{
		Unhook();
	}

	protected void Unhook()
	{
		if (!m_Hooked)
			return;
		m_Hooked = false;
		if (INT_QST_Client.s_OnChanged)
			INT_QST_Client.s_OnChanged.Remove(OnDataChanged);
		if (INT_QST_Client.s_OnResult)
			INT_QST_Client.s_OnResult.Remove(OnResult);
		if (INT_QST_Admin.s_OnReply)
			INT_QST_Admin.s_OnReply.Remove(OnAdminReply);
	}

	// --- data ---------------------------------------------------------------------------------------------------------
	// s_OnChanged comes with every kill / progress packet (and from a pin click inside this window): the rebuild
	// is throttled and runs from Update (OnDirtyRefresh).
	void OnDataChanged()
	{
		if (IsClosingDown())
			return;
		MarkDirty();
	}

	override void OnDirtyRefresh()
	{
		if (IsClosingDown())
			return;
		if (s_Tab == TAB_SETTINGS)
		{
			// the settings page is not rebuilt by progress packets (a slider may be held)
			m_DataDirty = false;
			DrawSlots();
			return;
		}
		if (INT_QST_Client.s_ShapeRevision == m_ShapeSeen && s_Tab != TAB_ADMIN)
		{
			// only progress counters moved: rows, sheet and note get the new numbers, the lists stay
			m_List.Rebind();
			ShowSelected();
			DrawSticky();
			return;
		}
		RebuildAll(false);
	}

	// Next frame (from a click inside the part that is rebuilt).
	protected void RebuildLater(bool toTop)
	{
		m_RebuildSoon = true;
		if (toTop)
			m_RebuildTop = true;
	}

	protected void RebuildAll(bool toTop)
	{
		m_ShapeSeen = INT_QST_Client.s_ShapeRevision;
		m_DataDirty = false;
		DrawSlots();
		DrawTabs();
		bool admin = s_Tab == TAB_ADMIN;
		bool settings = s_Tab == TAB_SETTINGS;
		bool other = admin || settings;
		m_AdminPage.Show(admin);
		m_SetPage.Show(settings);
		m_List.GetRoot().Show(!other);
		m_Sheet.GetRoot().Show(!other);
		m_Sticky.Show(!other);
		m_Buttons.Show(!other);
		m_LeftHead.Show(!other);
		if (!settings)
		{
			m_TrackerSet.Finish();
			m_TrackerSet.Detach();
		}
		if (admin)
		{
			BuildAdminPage();
			return;
		}
		if (settings)
		{
			BuildSettingsPage();
			return;
		}
		FillRows();
		DrawLeftHead();
		int keep;
		if (!s_SelectedByTab.Find(s_Tab, keep))
			keep = 0;
		if (!HasRow(keep))
			keep = FirstQuest();
		m_Selected = keep;
		m_Src.m_Selected = keep;
		m_Src.m_Tab = s_Tab;
		m_List.Refresh(toTop);
		if (toTop)
			ScrollToSelected();
		ShowSelected();
		DrawSticky();
	}

	protected void FillRows()
	{
		m_Src.m_Rows.Clear();
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		string lastBranch = "";
		foreach (INT_QST_QuestDef q : cat.m_List)
		{
			if (!InTab(q))
				continue;
			if (q.m_Branch != lastBranch)
			{
				lastBranch = q.m_Branch;
				INT_QST_ListRow g = new INT_QST_ListRow();
				g.Group = true;
				g.Text = cat.BranchTitle(q.m_Branch);
				m_Src.m_Rows.Insert(g);
			}
			INT_QST_ListRow r = new INT_QST_ListRow();
			r.QuestId = q.Id;
			m_Src.m_Rows.Insert(r);
		}
		if (s_Tab != TAB_ACTIVE)
			return;
		// taken quests an admin deleted: shown at the end so the player can free their slots
		bool goneGroup = false;
		foreach (INT_QST_Active a : INT_QST_Client.s_State.Active)
		{
			if (!a || cat.Get(a.Id))
				continue;
			if (!goneGroup)
			{
				INT_QST_ListRow gg = new INT_QST_ListRow();
				gg.Group = true;
				gg.Text = "#STR_INT_QST_FB_GONE_GROUP";
				m_Src.m_Rows.Insert(gg);
				goneGroup = true;
			}
			INT_QST_ListRow gr = new INT_QST_ListRow();
			gr.QuestId = a.Id;
			gr.Gone = true;
			m_Src.m_Rows.Insert(gr);
		}
	}

	protected bool InTab(INT_QST_QuestDef q)
	{
		int wait;
		if (s_Tab == TAB_ACTIVE)
			return INT_QST_Client.s_State.FindActive(q.Id) != null;
		if (s_Tab == TAB_DONE)
		{
			INT_QST_Done d = INT_QST_Client.s_State.FindDone(q.Id);
			return d && d.Times > 0;
		}
		if (s_Tab == TAB_AVAIL)
		{
			int st = INT_QST_Client.StatusOf(q, wait);
			return st == INT_QST_St.AVAILABLE;
		}
		return false;
	}

	protected int CountActive()
	{
		return INT_QST_Client.s_State.Active.Count();
	}

	protected int CountDone()
	{
		int n = 0;
		foreach (INT_QST_Done d : INT_QST_Client.s_State.Done)
		{
			if (d && d.Times > 0 && INT_QST_Client.s_Catalog.Get(d.Id))
				n++;
		}
		return n;
	}

	protected int CountAvailable()
	{
		int n = 0;
		int wait;
		foreach (INT_QST_QuestDef q : INT_QST_Client.s_Catalog.m_List)
		{
			if (INT_QST_Client.StatusOf(q, wait) == INT_QST_St.AVAILABLE)
				n++;
		}
		return n;
	}

	protected bool HasRow(int id)
	{
		if (id == 0)
			return false;
		foreach (INT_QST_ListRow r : m_Src.m_Rows)
		{
			if (!r.Group && r.QuestId == id)
				return true;
		}
		return false;
	}

	protected int FirstQuest()
	{
		foreach (INT_QST_ListRow r : m_Src.m_Rows)
		{
			if (!r.Group)
				return r.QuestId;
		}
		return 0;
	}

	protected void ScrollToSelected()
	{
		int n = m_Src.m_Rows.Count();
		for (int i = 0; i < n; i++)
		{
			INT_QST_ListRow r = m_Src.m_Rows[i];
			if (!r.Group && r.QuestId == m_Selected)
			{
				m_List.ScrollToRow(i);
				return;
			}
		}
	}

	void Select(int questId)
	{
		if (questId == m_Selected)
			return;
		m_Selected = questId;
		s_SelectedByTab.Set(s_Tab, questId);
		m_Src.m_Selected = questId;
		m_List.Rebind();
		ShowSelected();
		INT_QST_Sfx.Play("page");
	}

	void ToggleTrack(int questId)
	{
		bool on = !INT_QST_Client.IsTracked(questId);
		if (!INT_QST_Client.SetTracked(questId, on))
		{
			string max = INT_QST_Const.TRACK_MAX.ToString();
			Flash(INT_QST_Look.Fmt("#STR_INT_QST_TRACK_FULL", max));
			return;
		}
		INT_QST_Sfx.Play("pin");
	}

	protected void ShowSelected()
	{
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(m_Selected);
		m_Clicks.ForgetUnder(m_Buttons);
		INT_UIF_Widgets.ClearChildren(m_Buttons);
		m_LiveSig = "";
		if (!q)
		{
			if (s_Tab == TAB_ACTIVE && m_Selected != 0 && INT_QST_Client.s_State.FindActive(m_Selected))
			{
				// taken, but deleted by an admin
				m_Sheet.ShowEmpty("#STR_INT_QST_FB_GONE", "#STR_INT_QST_FB_GONE_TEXT");
				DrawGoneButtons(m_Selected);
				return;
			}
			if (s_Tab == TAB_ACTIVE)
				m_Sheet.ShowEmpty("#STR_INT_QST_EMPTY_ACTIVE", "#STR_INT_QST_EMPTY_ACTIVE_TEXT");
			else if (s_Tab == TAB_AVAIL)
				m_Sheet.ShowEmpty("#STR_INT_QST_EMPTY_AVAIL", "#STR_INT_QST_EMPTY_AVAIL_TEXT");
			else
				m_Sheet.ShowEmpty("#STR_INT_QST_EMPTY_DONE", "#STR_INT_QST_EMPTY_DONE_TEXT");
			return;
		}
		int mode = INT_QST_Sheet.MODE_ACTIVE;
		if (s_Tab == TAB_AVAIL)
			mode = INT_QST_Sheet.MODE_AVAILABLE;
		else if (s_Tab == TAB_DONE)
			mode = INT_QST_Sheet.MODE_DONE;
		m_Sheet.Show(q, mode);
		m_LiveSig = LiveSignature(q);
		DrawButtons(q);
	}

	protected void DrawButtons(INT_QST_QuestDef q)
	{
		float w = GetBodyW() - (FOLD + 60) - 34;
		int wait;
		int st = INT_QST_Client.StatusOf(q, wait);
		Widget hv;
		if (st == INT_QST_St.ACTIVE || st == INT_QST_St.READY)
		{
			// ready and no giver to visit (items to hand over keep it from completing by itself): turn in here
			float tx = 0;
			float trackW = 360;
			if (st == INT_QST_St.READY && q.GiverFinish == "")
			{
				int yc0 = INT_QST_Look.Sticky();
				Widget sb = INT_QST_Look.Button(m_Buttons, 0, 0, 250, 50, "#STR_INT_QST_B_TURNIN", "check", INT_QST_Look.BTN_INK, yc0, hv);
				ButtonWidget sh = INT_UIF_Draw.Hit(sb, 0, 0, 250, 50, m_Clicks);
				m_Clicks.Bind(sh, ID_TURNIN, q.Id, hv);
				tx = 262;
				trackW = 300;
			}
			// «На карте»: only with VPP map / RF Map loaded and a point still to reach (owner 2026-10-22)
			bool mapSwitch = false;		// if-chain: `x = (a && b.Call())` is a measured heap trap
			if (INT_QST_MapLayer.HasMapMod())
				mapSwitch = INT_QST_MapLayer.HasOpenPoint(q);
			if (mapSwitch && tx == 0)
				trackW = 330;		// «ОТСЛЕЖИВАТЬ НА ЭКРАНЕ» clears its switch at 330 (300 overlapped, stand 2026-10-22)
			bool tracked = INT_QST_Client.IsTracked(q.Id);
			int pinColor = 0xFFE0574A;
			Widget tb = INT_QST_Look.Button(m_Buttons, tx, 0, trackW, 50, "#STR_INT_QST_B_TRACK", "pushpin", INT_QST_Look.BTN_INK, pinColor, hv);
			INT_QST_Look.Switch(tb, trackW - 56, 14, tracked);
			ButtonWidget th = INT_UIF_Draw.Hit(tb, 0, 0, trackW, 50, m_Clicks);
			m_Clicks.Bind(th, ID_TRACK, q.Id, hv);
			if (mapSwitch && tx == 0)
			{
				float mapX = trackW + 12;
				float mapW = 210;		// ends at 552: the cancel button starts at 572
				bool onMap = INT_QST_Client.IsOnMap(q.Id);
				Widget hvMap;
				int mapColor = INT_QST_MapLayer.RING;
				Widget mb = INT_QST_Look.Button(m_Buttons, mapX, 0, mapW, 50, "#STR_INT_QST_B_ONMAP", "map_pin", INT_QST_Look.BTN_INK, mapColor, hvMap);
				INT_QST_Look.Switch(mb, mapW - 56, 14, onMap);
				ButtonWidget mh = INT_UIF_Draw.Hit(mb, 0, 0, mapW, 50, m_Clicks);
				m_Clicks.Bind(mh, ID_ONMAP, q.Id, hvMap);
			}
			if (INT_QST_Client.s_Hello && INT_QST_Client.s_Hello.CanCancel)
			{
				int rc = INT_QST_Look.Red();
				Widget cb = INT_QST_Look.Button(m_Buttons, w - 210, 0, 210, 50, "#STR_INT_QST_B_CANCEL", "close", INT_QST_Look.BTN_DANGER, rc, hv);
				ButtonWidget ch = INT_UIF_Draw.Hit(cb, 0, 0, 210, 50, m_Clicks);
				m_Clicks.Bind(ch, ID_CANCEL, q.Id, hv);
			}
			return;
		}
		if (st == INT_QST_St.EXPIRED)
		{
			int rc2 = INT_QST_Look.Red();
			Widget db = INT_QST_Look.Button(m_Buttons, w - 280, 0, 280, 50, "#STR_INT_QST_B_DISMISS", "trash", INT_QST_Look.BTN_DANGER, rc2, hv);
			ButtonWidget dh = INT_UIF_Draw.Hit(db, 0, 0, 280, 50, m_Clicks);
			m_Clicks.Bind(dh, ID_DISMISS, q.Id, hv);
			return;
		}
		if (st == INT_QST_St.AVAILABLE)
		{
			if (q.GiverStart == "")
			{
				int yc = INT_QST_Look.Sticky();
				Widget ab = INT_QST_Look.Button(m_Buttons, 0, 0, 360, 50, "#STR_INT_QST_B_TAKE", "check", INT_QST_Look.BTN_INK, yc, hv);
				ButtonWidget ah = INT_UIF_Draw.Hit(ab, 0, 0, 360, 50, m_Clicks);
				m_Clicks.Bind(ah, ID_TAKE, q.Id, hv);
				return;
			}
			// taken at a giver: say who and how far
			INT_QST_Look.Icon(m_Buttons, 0, 15, 20, "person", INT_QST_Look.Ink2());
			string g = INT_QST_Text.GiverName(q.GiverStart);
			string dist = INT_QST_Text.GiverDistance(q.GiverStart);
			string line = INT_QST_Look.Fmt("#STR_INT_QST_TAKE_AT", g);
			if (dist != "")
				line = line + " · " + dist;
			INT_QST_Look.Heavy(m_Buttons, 30, 0, w - 30, 50, line, 16, INT_QST_Look.Ink2());
		}
	}

	// A taken quest deleted by an admin: one button that drops it (DISMISS; the server removes an active quest whose
	// definition is gone and frees the slot).
	protected void DrawGoneButtons(int questId)
	{
		float w = GetBodyW() - (FOLD + 60) - 34;
		Widget hv;
		int rc = INT_QST_Look.Red();
		Widget db = INT_QST_Look.Button(m_Buttons, w - 280, 0, 280, 50, "#STR_INT_QST_B_DISMISS", "trash", INT_QST_Look.BTN_DANGER, rc, hv);
		ButtonWidget dh = INT_UIF_Draw.Hit(db, 0, 0, 280, 50, m_Clicks);
		m_Clicks.Bind(dh, ID_DISMISS, questId, hv);
	}

	// What can change while the window is open without a server message: inventory counts and timers.
	protected string LiveSignature(INT_QST_QuestDef q)
	{
		string sig = "";
		int n = q.Targets.Count();
		for (int i = 0; i < n; i++)
		{
			INT_QST_TargetDef t = q.Targets[i];
			if (!INT_QST_TT.IsInventory(t.m_Type))
				continue;
			int tv = INT_QST_Client.TargetValue(q, i);
			sig = sig + tv.ToString() + ",";
		}
		int wait;
		int st = INT_QST_Client.StatusOf(q, wait);
		int minutes = wait / 60;
		sig = sig + st.ToString() + ":" + minutes.ToString();
		return sig;
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (IsClosingDown())
			return;
		if (m_RebuildSoon)
		{
			bool top = m_RebuildTop;
			m_RebuildSoon = false;
			m_RebuildTop = false;
			m_AdminSoon = false;
			RebuildAll(top);
		}
		if (m_AdminSoon)
		{
			m_AdminSoon = false;
			if (s_Tab == TAB_ADMIN)
				BuildAdminPage();
		}
		if (s_Tab == TAB_SETTINGS)
		{
			if (m_SetSoon)
			{
				m_SetSoon = false;
				BuildSettingsPage();
			}
			if (!IsAsking())
				m_TrackerSet.Update();
			return;
		}
		m_LiveAcc = m_LiveAcc + timeslice;
		if (m_LiveAcc < 1.0)
			return;
		m_LiveAcc = 0;
		if (IsClosingDown() || s_Tab == TAB_ADMIN || IsAsking())
			return;
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(m_Selected);
		if (!q)
			return;
		string sig = LiveSignature(q);
		if (sig == m_LiveSig)
			return;
		RebuildAll(false);
	}

	// --- left page head + sticky note ---------------------------------------------------------------------------------
	protected void DrawLeftHead()
	{
		m_Clicks.ForgetUnder(m_LeftHead);
		INT_UIF_Widgets.ClearChildren(m_LeftHead);
		string key = "#STR_INT_QST_H_ACTIVE";
		if (s_Tab == TAB_AVAIL)
			key = "#STR_INT_QST_H_AVAIL";
		else if (s_Tab == TAB_DONE)
			key = "#STR_INT_QST_H_DONE";
		string cap = INT_QST_Look.Spaced(key);
		INT_QST_Look.Heavy(m_LeftHead, 32, 20, 330, 22, cap, 13, INT_QST_Look.Ink());
		INT_QST_Look.Icon(m_LeftHead, FOLD - 150, 23, 15, "list", INT_QST_Look.Ink3());
		string by = INT_QST_Look.Tr("#STR_INT_QST_BY_BRANCH");
		INT_QST_Look.Text(m_LeftHead, FOLD - 130, 19, 110, 22, by, 13, INT_QST_Look.Ink3());
		INT_QST_Look.DoubleRule(m_LeftHead, 32, 49, FOLD - 60);
	}

	protected void DrawSticky()
	{
		m_Clicks.ForgetUnder(m_Sticky);
		INT_UIF_Widgets.ClearChildren(m_Sticky);
		float w = 436;
		float h = 196;
		INT_UIF_Surface.Shadow(m_Sticky, -22, -12, w + 44, h + 40, 0x52000000);
		INT_QST_Look.PaperSheet(m_Sticky, 0, 0, w, h, INT_QST_Look.Sticky(), "Paper");
		INT_QST_Look.PinHead(m_Sticky, w * 0.5 - 13, -14);
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_NOTE");
		INT_QST_Look.Heavy(m_Sticky, 24, 18, w - 48, 20, cap, 12, INT_QST_Look.Ink());
		float y = 46;
		if (s_Tab == TAB_ACTIVE)
		{
			int tracked = 0;
			int ready = 0;
			int nearest = -1;
			foreach (INT_QST_Active a : INT_QST_Client.s_State.Active)
			{
				INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(a.Id);
				if (!q)
					continue;
				if (INT_QST_Client.IsTracked(a.Id))
					tracked++;
				int wait;
				int st = INT_QST_Client.StatusOf(q, wait);
				if (st == INT_QST_St.READY)
					ready++;
				if (wait >= 0 && (nearest < 0 || wait < nearest))
					nearest = wait;
			}
			string nt = "—";
			if (nearest >= 0)
				nt = INT_QST_Look.Dur(nearest);
			string trackedText = tracked.ToString() + " / " + INT_QST_Const.TRACK_MAX.ToString();
			y = StickyRow(y, "#STR_INT_QST_N_TRACKED", trackedText);
			y = StickyRow(y, "#STR_INT_QST_N_READY", ready.ToString());
			y = StickyRow(y, "#STR_INT_QST_N_DEADLINE", nt);
			y = StickyRow(y, "#STR_INT_QST_N_DONE_ALL", CountDone().ToString());
			StickyHint(h - 52, "pushpin", "#STR_INT_QST_N_PIN_HINT", INT_QST_Look.Red());
			StickyLink(h - 28, "fit", "#STR_INT_QST_N_MOVE_HUD", ID_MOVE_HUD);
			return;
		}
		if (s_Tab == TAB_AVAIL)
		{
			int free = 0;
			if (INT_QST_Client.s_Hello)
				free = INT_QST_Rules.FreeSlots(INT_QST_Client.s_State, INT_QST_Client.s_Hello.QuestSlots);
			int avail = 0;
			int journal = 0;
			int cooldown = 0;
			int w2;
			foreach (INT_QST_QuestDef q2 : INT_QST_Client.s_Catalog.m_List)
			{
				int st2 = INT_QST_Client.StatusOf(q2, w2);
				if (st2 == INT_QST_St.AVAILABLE)
				{
					avail++;
					if (q2.GiverStart == "")
						journal++;
				}
				else if (st2 == INT_QST_St.COOLDOWN)
					cooldown++;
			}
			y = StickyRow(y, "#STR_INT_QST_N_FREE", free.ToString());
			y = StickyRow(y, "#STR_INT_QST_N_AVAIL", avail.ToString());
			y = StickyRow(y, "#STR_INT_QST_N_JOURNAL", journal.ToString());
			y = StickyRow(y, "#STR_INT_QST_N_COOLDOWN", cooldown.ToString());
			StickyHint(h - 30, "warning", "#STR_INT_QST_N_SIGN_HINT", INT_QST_Look.Ink2());
			return;
		}
		int quests = 0;
		int turns = 0;
		int lastAt = 0;
		int repeatable = 0;
		foreach (INT_QST_Done d : INT_QST_Client.s_State.Done)
		{
			INT_QST_QuestDef q3 = INT_QST_Client.s_Catalog.Get(d.Id);
			if (!q3 || d.Times <= 0)
				continue;
			quests++;
			turns = turns + d.Times;
			if (d.Last > lastAt)
				lastAt = d.Last;
			if (q3.IsRepeatable())
				repeatable++;
		}
		string ago = "—";
		if (lastAt > 0)
		{
			int dt = INT_QST_Client.Now() - lastAt;
			ago = INT_QST_Look.Fmt("#STR_INT_QST_AGO", INT_QST_Look.Dur(dt));
		}
		y = StickyRow(y, "#STR_INT_QST_N_DONE_Q", quests.ToString());
		y = StickyRow(y, "#STR_INT_QST_N_TURNS", turns.ToString());
		y = StickyRow(y, "#STR_INT_QST_N_REPEATABLE", repeatable.ToString());
		y = StickyRow(y, "#STR_INT_QST_N_LAST", ago);
	}

	// "Label ........ value" on the sticky note.
	protected float StickyRow(float y, string labelKey, string value)
	{
		float w = 436;
		string lab = INT_QST_Look.Tr(labelKey);
		float lw = INT_UIF_Draw.TextWidth(lab, 15) * 1.05 + 6;
		INT_QST_Look.Text(m_Sticky, 24, y, lw, 27, lab, 15, INT_QST_Look.Ink2());
		float vw = INT_UIF_Draw.TextWidth(value, 16) * 1.1 + 4;
		float dotsX = 24 + lw + 4;
		float dotsW = w - 24 - vw - 8 - dotsX;
		if (dotsW > 6)
			DottedLeader(dotsX, y + 19, dotsW);
		INT_QST_Look.Heavy(m_Sticky, w - 24 - vw, y, vw, 27, value, 16, INT_QST_Look.Ink(), INT_UIF_Align.RIGHT);
		return y + 27;
	}

	protected void DottedLeader(float x, float y, float w)
	{
		int c = INT_QST_Look.InkA(90);
		float dx = x;
		while (dx < x + w - 2)
		{
			INT_UIF_Draw.Line(m_Sticky, dx, y, 2, 1, c);
			dx = dx + 5;
		}
	}

	protected void StickyHint(float y, string icon, string key, int iconColor)
	{
		INT_QST_Look.Icon(m_Sticky, 24, y + 2, 16, icon, iconColor);
		string t = INT_QST_Look.Tr(key);
		INT_QST_Look.Text(m_Sticky, 46, y, 380, 20, t, 13, INT_QST_Look.Ink3());
	}

	// Clickable line on the sticky note.
	protected void StickyLink(float y, string icon, string key, int id)
	{
		Widget f = INT_UIF_Draw.Frame(m_Sticky, 18, y - 2, 400, 24, "Link");
		Widget hover = INT_UIF_Draw.Frame(f, 0, 0, 400, 24, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, 400, 24, 0x262A2218);
		int c = INT_QST_Look.Blue();
		INT_QST_Look.Icon(f, 6, 4, 16, icon, c);
		string t = INT_QST_Look.Tr(key);
		INT_QST_Look.Heavy(f, 28, 0, 370, 24, t, 13, c);
		float tw = INT_UIF_Draw.TextWidth(t, 13) * 1.05;
		INT_UIF_Draw.Line(f, 28, 20, tw, 1, INT_QST_Look.InkA(120));
		ButtonWidget hit = INT_UIF_Draw.Hit(f, 0, 0, 400, 24, m_Clicks);
		m_Clicks.Bind(hit, id, 0, hover);
	}

	// --- settings page (INT_QST_TrackerSet) --------------------------------------------------------------------------
	protected void BuildSettingsPage()
	{
		float bw = GetBodyW();
		float bh = GetBodyH();
		m_TrackerSet.Build(m_SetPage, m_Clicks, bw, bh, ID_SET_INGAME, ID_SET_RESET, ID_SET_SHOW);
	}

	// Self-test (TestKit `#qst board set ...`): size <pct> | alpha <pct> | place <nx> <ny> | slider size|alpha <frac>
	// | mark <nx> <ny> | reset | state.
	override void OnTestCommand(string command)
	{
		array<string> w = new array<string>();
		command.Split(" ", w);
		if (w.Count() < 2 || w[0] != "set")
			return;
		string what = w[1];
		string a = "";
		string b = "";
		if (w.Count() > 2)
			a = w[2];
		if (w.Count() > 3)
			b = w[3];
		if (s_Tab != TAB_SETTINGS)
		{
			s_Tab = TAB_SETTINGS;
			RebuildAll(true);
		}
		string res = "ok";
		if (what == "size")
			m_TrackerSet.SetSize(a.ToFloat());
		else if (what == "alpha")
			m_TrackerSet.SetTransparency(a.ToFloat());
		else if (what == "place")
		{
			float nx = a.ToFloat();
			float ny = b.ToFloat();
			m_TrackerSet.SetPlace(nx, ny);
		}
		else if (what == "slider")
		{
			bool sz = a == "size";
			float frac = b.ToFloat();
			if (!m_TrackerSet.TestDragSlider(sz, frac))
				res = "slider miss";
		}
		else if (what == "mark")
		{
			float mx = a.ToFloat();
			float my = b.ToFloat();
			if (!m_TrackerSet.TestDragMark(mx, my))
				res = "mark miss";
		}
		else if (what == "reset")
		{
			OnClickId(ID_SET_RESET, 0, 0);
			BuildSettingsPage();
		}
		else if (what == "show")
		{
			OnClickId(ID_SET_SHOW, 0, 0);
			BuildSettingsPage();
			res = "shown " + INT_QST_UI.IsPlayerShown().ToString();
		}
		m_TrackerSet.Finish();
		string st = m_TrackerSet.TestState();
		Print("[INT_QST_T] set " + what + " -> " + res + " | " + st);
	}

	// --- admin page (tools open their own windows; built in the admin stage) -----------------------------------------
	protected void BuildAdminPage()
	{
		m_Clicks.ForgetUnder(m_AdminPage);
		INT_UIF_Widgets.ClearChildren(m_AdminPage);
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_TAB_ADMIN");
		INT_QST_Look.Heavy(m_AdminPage, 32, 20, 400, 22, cap, 13, INT_QST_Look.Red());
		INT_QST_Look.DoubleRule(m_AdminPage, 32, 49, FOLD - 60);
		float y = 76;
		y = AdminTool(y, 1, "grid", "#STR_INT_QST_A_BOARD", "#STR_INT_QST_A_BOARD_TEXT");
		y = AdminTool(y, 2, "person", "#STR_INT_QST_A_GIVERS", "#STR_INT_QST_A_GIVERS_TEXT");
		y = AdminTool(y, 3, "export", "#STR_INT_QST_A_EXCHANGE", "#STR_INT_QST_A_EXCHANGE_TEXT");
		y = AdminTool(y, 4, "refresh", "#STR_INT_QST_A_RELOAD", "#STR_INT_QST_A_RELOAD_TEXT");
		float rx = FOLD + 60;
		float rw = GetBodyW() - rx - 34;
		string info = INT_QST_Look.Spaced("#STR_INT_QST_A_SUMMARY");
		INT_QST_Look.Heavy(m_AdminPage, rx, 20, 500, 22, info, 13, INT_QST_Look.Ink());
		INT_QST_Look.DoubleRule(m_AdminPage, rx, 49, rw);
		int nq = INT_QST_Client.s_Catalog.m_List.Count();
		int nb = INT_QST_Client.s_Catalog.m_BranchOrder.Count();
		int ng = INT_QST_Client.s_Givers.Count();
		string nqT = nq.ToString();
		string nbT = nb.ToString();
		string ngT = ng.ToString();
		string sum = INT_QST_Look.Fmt("#STR_INT_QST_A_SUMMARY_LINE", nqT, nbT, ngT);
		INT_QST_Look.Text(m_AdminPage, rx, 60, rw, 26, sum, 15, INT_QST_Look.Ink2());
		// server switches
		string sc = INT_QST_Look.Spaced("#STR_INT_QST_A_SETTINGS");
		INT_QST_Look.Heavy(m_AdminPage, rx, 104, 500, 22, sc, 13, INT_QST_Look.Ink());
		INT_QST_Look.DoubleRule(m_AdminPage, rx, 133, rw);
		INT_QST_AHello ah = INT_QST_Admin.s_Hello;
		if (!ah)
		{
			string wait = INT_QST_Look.Tr("#STR_INT_QST_A_LOADING");
			INT_QST_Look.Text(m_AdminPage, rx, 146, rw, 26, wait, 14, INT_QST_Look.Ink3());
			return;
		}
		float sy = 142;
		// SIGNS-OFF (owner 2026-10-31): switch 0 = signs above the givers
		// sy = AdminSwitch(rx, sy, rw, 0, "#STR_INT_QST_A_SW_SIGNS", "#STR_INT_QST_A_SW_SIGNS_TEXT", ah.Config.ShowSigns != 0);
		sy = AdminSwitch(rx, sy, rw, 2, "#STR_INT_QST_A_SW_LOCKED", "#STR_INT_QST_A_SW_LOCKED_TEXT", ah.Config.ShowLockedAtGiver != 0);
		sy = AdminSwitch(rx, sy, rw, 3, "#STR_INT_QST_A_SW_CANCEL", "#STR_INT_QST_A_SW_CANCEL_TEXT", ah.Config.CanCancel != 0);
		sy = AdminSwitch(rx, sy, rw, 5, "#STR_INT_QST_A_SW_GRENADE", "#STR_INT_QST_A_SW_GRENADE_TEXT", ah.Config.GrenadeKills != 0);
		sy = AdminSwitch(rx, sy, rw, 4, "#STR_INT_QST_A_SW_DEBUG", "#STR_INT_QST_A_SW_DEBUG_TEXT", ah.Config.DebugMode != 0);
		// problems found while reading the files
		string pcCap = INT_QST_Look.Spaced("#STR_INT_QST_A_PROBLEMS");
		string pcCount = ah.Problems.Count().ToString();
		string pc = pcCap + "  (" + pcCount + ")";
		INT_QST_Look.Heavy(m_AdminPage, rx, sy + 14, 500, 22, pc, 13, INT_QST_Look.Red());
		INT_QST_Look.DoubleRule(m_AdminPage, rx, sy + 43, rw);
		m_ProblemSrc = new INT_QST_LinesSource();
		foreach (string pr : ah.Problems)
			m_ProblemSrc.m_Lines.Insert(INT_QST_Look.Fit(pr, rw - 30, 13));
		if (ah.Problems.Count() == 0)
			m_ProblemSrc.m_Lines.Insert(INT_QST_Look.Tr("#STR_INT_QST_A_NO_PROBLEMS"));
		float lh = GetBodyH() - (sy + 52) - 16;
		m_ProblemList = new INT_QST_VList();
		int bar = INT_QST_Look.InkA(30);
		int thumb = INT_QST_Look.InkA(120);
		m_ProblemList.Build(m_AdminPage, rx, sy + 52, rw, lh, m_ProblemSrc, bar, thumb);
	}

	// One server switch: label, a line of help, the switch; the whole row toggles.
	protected float AdminSwitch(float x, float y, float w, int index, string labelKey, string hintKey, bool on)
	{
		Widget f = INT_UIF_Draw.Frame(m_AdminPage, x, y, w, 46, "Switch");
		Widget hover = INT_UIF_Draw.Frame(f, 0, 0, w, 46, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, w, 46, 0x1A2A2218);
		string lab = INT_QST_Look.Tr(labelKey);
		INT_QST_Look.Heavy(f, 8, 2, w - 80, 24, lab, 15, INT_QST_Look.Ink());
		string hint = INT_QST_Look.Tr(hintKey);
		hint = INT_QST_Look.Fit(hint, w - 90, 12);
		INT_QST_Look.Text(f, 8, 24, w - 80, 18, hint, 12, INT_QST_Look.Ink3());
		INT_QST_Look.Switch(f, w - 52, 12, on);
		INT_QST_Look.Hair(f, 0, 45, w);
		ButtonWidget hit = INT_UIF_Draw.Hit(f, 0, 0, w, 46, m_Clicks);
		m_Clicks.Bind(hit, ID_ADMIN_SWITCH, index, hover);
		return y + 48;
	}

	protected void ToggleSwitch(int index)
	{
		INT_QST_AHello ah = INT_QST_Admin.s_Hello;
		if (!ah)
			return;
		INT_QST_AConfig c = ah.Config;
		// explicit 0 / 1 (a stray 2 in the file would turn into -1 with `1 - x`)
		// SIGNS-OFF (owner 2026-10-31): switch 0
		// if (index == 0)
		// 	c.ShowSigns = INT_QST_Journal.Flip01(c.ShowSigns);
		// else if (index == 2)
		if (index == 2)
			c.ShowLockedAtGiver = INT_QST_Journal.Flip01(c.ShowLockedAtGiver);
		else if (index == 3)
			c.CanCancel = INT_QST_Journal.Flip01(c.CanCancel);
		else if (index == 4)
			c.DebugMode = INT_QST_Journal.Flip01(c.DebugMode);
		else if (index == 5)
			c.GrenadeKills = INT_QST_Journal.Flip01(c.GrenadeKills);
		INT_QST_Admin.SaveConfig(c);
		INT_QST_Sfx.Play("click");
		m_AdminSoon = true;		// from the switch's own click: the page is rebuilt on the next frame
	}

	static int Flip01(int v)
	{
		if (v != 0)
			return 0;
		return 1;
	}

	// Answers of admin operations started from this window.
	void OnAdminReply(int op, int code, string text)
	{
		if (IsClosingDown())
			return;
		if (op == INT_QST_A.HELLO)
		{
			if (s_Tab == TAB_ADMIN)
				BuildAdminPage();
			return;
		}
		if (code != INT_QST_Res.OK)
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_E_" + code.ToString()));
			return;
		}
		if (op == INT_QST_A.CONFIG)
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_A_SAVED"), false);
			return;
		}
		if (op == INT_QST_A.RELOAD)
		{
			string t = INT_QST_Admin.TextOf(text);
			Flash(INT_QST_Look.Fmt("#STR_INT_QST_A_RELOADED", t), false);
			INT_QST_Admin.RequestHello();
		}
	}

	protected float AdminTool(float y, int tool, string icon, string titleKey, string textKey)
	{
		float w = FOLD - 60;
		Widget f = INT_UIF_Draw.Frame(m_AdminPage, 32, y, w, 92, "Tool");
		int fill = 0x33FFFAEB;
		int edge = INT_QST_Look.InkA(80);
		INT_UIF_Surface.Box(f, 0, 0, w, 84, 6, fill, edge);
		Widget hover = INT_UIF_Draw.Frame(f, 0, 0, w, 84, "Hover");
		int hf = INT_QST_Look.RedA(26);
		int hb = INT_QST_Look.RedA(150);
		INT_UIF_Surface.Box(hover, 0, 0, w, 84, 6, hf, hb);
		INT_QST_Look.Icon(f, 18, 26, 30, icon, INT_QST_Look.RedDeep());
		string t = INT_QST_Look.Tr(titleKey);
		INT_QST_Look.Heavy(f, 64, 12, w - 80, 26, t, 17, INT_QST_Look.Ink());
		array<string> lines = new array<string>();
		string body = INT_QST_Look.Tr(textKey);
		INT_QST_Text.Wrap(body, w - 84, 13, lines);
		float ly = 40;
		int k = 0;
		foreach (string ln : lines)
		{
			if (k >= 2)
				break;
			INT_QST_Look.Text(f, 64, ly, w - 80, 18, ln, 13, INT_QST_Look.Ink3());
			ly = ly + 18;
			k++;
		}
		ButtonWidget hit = INT_UIF_Draw.Hit(f, 0, 0, w, 84, m_Clicks);
		m_Clicks.Bind(hit, ID_ADMIN_TOOL, tool, hover);
		return y + 96;
	}

	protected float AdminLine(float x, float y, string labelKey, string value)
	{
		string lab = INT_QST_Look.Tr(labelKey);
		INT_QST_Look.Text(m_AdminPage, x, y, 360, 28, lab, 15, INT_QST_Look.Ink2());
		INT_QST_Look.Heavy(m_AdminPage, x + 360, y, 200, 28, value, 16, INT_QST_Look.Ink());
		return y + 30;
	}

	// --- input --------------------------------------------------------------------------------------------------------
	override void OnClickId(int id, int data, int button)
	{
		if (CommonClick(id))
			return;
		switch (id)
		{
			case ID_TAB:
				if (data == s_Tab)
					return;
				s_Tab = data;
				INT_QST_Sfx.Play("page");
				RebuildLater(true);		// the tabs are redrawn: not inside the clicked tab's event
				return;
			case ID_PIN:
			case ID_TRACK:
				ToggleTrack(data);
				return;
			case ID_CANCEL:
				INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(data);
				if (!q)
					return;
				string text = INT_QST_Look.Fmt("#STR_INT_QST_ASK_CANCEL_TEXT", q.Name);
				Ask(ASK_CANCEL, data, "#STR_INT_QST_ASK_CANCEL", text, "#STR_INT_QST_B_CANCEL", true);
				return;
			case ID_DISMISS:
				INT_QST_Client.Dismiss(data);
				INT_QST_Sfx.Play("click");
				return;
			case ID_TAKE:
				INT_QST_Client.Accept(data, "");
				INT_QST_Sfx.Play("click");
				return;
			case ID_TURNIN:
				INT_QST_Client.TurnIn(data, "");	// no finishing giver: the server takes an empty key
				INT_QST_Sfx.Play("click");
				return;
			case ID_MOVE_HUD:
				// the sticky-note link leads to the tracker settings
				INT_QST_Sfx.Play("page");
				s_Tab = TAB_SETTINGS;
				RebuildLater(true);
				return;
			case ID_SET_INGAME:
				INT_QST_Sfx.Play("click");
				m_TrackerSet.Finish();
				INT_QST_MoverMenu.ReturnToJournal();
				INT_QST_UI.StartPlayerMove();
				RequestClose();
				return;
			case ID_SET_RESET:
				INT_QST_Sfx.Play("click");
				INT_QST_UI.ResetPlayerLayout();
				m_SetSoon = true;		// the page holding the button is rebuilt on the next frame
				return;
			case ID_SET_SHOW:
				INT_QST_Sfx.Play("click");
				INT_QST_UI.TogglePlayerShown();
				m_SetSoon = true;
				return;
			case ID_ONMAP:
				// the client's change event redraws the buttons on a later frame (not inside this click)
				INT_QST_Sfx.Play("click");
				bool onMapNow = INT_QST_Client.IsOnMap(data);
				INT_QST_Client.SetOnMap(data, !onMapNow);
				return;
			case ID_ADMIN_TOOL:
				INT_QST_Sfx.Play("click");
				if (data == 4)
				{
					string rq = INT_QST_Look.Tr("#STR_INT_QST_A_RELOAD_ASK");
					Ask(ASK_RELOAD, 0, "#STR_INT_QST_A_RELOAD", rq, "#STR_INT_QST_A_RELOAD", false);
					return;
				}
				if (data == 1)
				{
					INT_UIF_Menu.Open(INT_QST_Const.MENU_BOARD);
					return;
				}
				INT_QST_AdminUi.Open(data);
				return;
			case ID_ADMIN_SWITCH:
				ToggleSwitch(data);
				return;
		}
	}

	override void OnAnswer(int tag, int data, bool yes)
	{
		if (tag == ASK_CANCEL && yes)
			INT_QST_Client.Cancel(data);
		if (tag == ASK_RELOAD && yes)
			INT_QST_Admin.Reload();
	}

	// Server answers to what this window sends (take, turn in, abandon, remove): errors become a flash line, a taken
	// quest switches to the active tab. Answers to other requests (exploration reports ...) are not this window's.
	void OnResult(int op, int code, int questId)
	{
		if (IsClosingDown())
			return;
		bool mine = op == INT_QST_C2S.ACCEPT || op == INT_QST_C2S.TURNIN || op == INT_QST_C2S.CANCEL || op == INT_QST_C2S.DISMISS;
		if (!mine)
			return;
		if (code != INT_QST_Res.OK)
		{
			string codeText = code.ToString();
			Flash(INT_QST_Look.Tr("#STR_INT_QST_E_" + codeText));
			return;
		}
		if (op == INT_QST_C2S.TURNIN)
		{
			INT_QST_Sfx.Play("complete");
			return;
		}
		if (op == INT_QST_C2S.ACCEPT && s_Tab == TAB_AVAIL)
		{
			s_Tab = TAB_ACTIVE;
			s_SelectedByTab.Set(TAB_ACTIVE, questId);
			INT_QST_Sfx.Play("stamp");
			RebuildAll(true);
		}
	}
}

// Admin windows are built in the admin stage; until then the tools only say so.
class INT_QST_AdminUi
{
	static void Open(int tool)
	{
		if (tool == 2)
		{
			INT_UIF_Menu.Open(INT_QST_Const.MENU_GIVERS);
			return;
		}
		if (tool == 3)
		{
			INT_UIF_Menu.Open(INT_QST_Const.MENU_EXCHANGE);
			return;
		}
		INT_QST_Journal j = INT_QST_Journal.Cast(INT_UIF_Menu.Current());
		if (j)
			j.Flash(INT_QST_Look.Tr("#STR_INT_QST_A_SOON"), false);
	}
}
