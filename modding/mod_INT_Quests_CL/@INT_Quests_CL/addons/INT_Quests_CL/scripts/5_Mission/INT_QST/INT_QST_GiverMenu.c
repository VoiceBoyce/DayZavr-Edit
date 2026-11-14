// INT Quests (client) — talking to a quest giver (design B, design/concepts_v1/2_b_diary_giver.png). The same
// notebook: a photo card of the giver taped to the leather, his greeting on a pinned note, his quests on the left
// page with the same diamond signs as above his head (closed ones grey with the reason; their text and rewards are
// hidden), the quest sheet on the right with one main ink button: ПРИНЯТЬ / СДАТЬ / ОБМЕНЯТЬ (shop-like quests are
// taken and turned in at once). Walking away closes the window.

class INT_QST_GiverRow
{
	int QuestId;
	int St;
	int Wait;
	bool TurnIn;		// active here and this giver takes it
}

class INT_QST_GiverSlot2 extends INT_QST_VSlot
{
	Widget m_Box;
}

class INT_QST_GiverListSource extends INT_QST_VSource
{
	static const float ROW_H = 74;

	ref array<ref INT_QST_GiverRow> m_Rows = new array<ref INT_QST_GiverRow>();
	INT_QST_GiverMenu m_Owner;
	int m_Selected;

	override int Count()
	{
		return m_Rows.Count();
	}

	override float RowHeight(int i)
	{
		return ROW_H;
	}

	override INT_QST_VSlot NewSlot()
	{
		return new INT_QST_GiverSlot2();
	}

	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_GiverSlot2 s = INT_QST_GiverSlot2.Cast(slot);
		s.m_Box = INT_UIF_Draw.Frame(s.m_Root, 0, 0, w, ROW_H, "Row");
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_GiverSlot2 s = INT_QST_GiverSlot2.Cast(slot);
		INT_UIF_Widgets.ClearChildren(s.m_Box);
		s.m_Hover = null;
		INT_QST_GiverRow r = m_Rows[i];
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(r.QuestId);
		if (!q)
			return;
		bool sel = r.QuestId == m_Selected;
		Widget hover = INT_UIF_Draw.Frame(s.m_Box, 6, 4, w - 6, ROW_H - 8, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, w - 6, ROW_H - 8, 0x1F2A2218);
		s.m_Hover = hover;
		if (sel)
		{
			INT_QST_Look.Tex(s.m_Box, 6, 4, w - 6, ROW_H - 8, "int_qst_highlight_ca", INT_QST_Look.Marker());
			INT_UIF_Draw.Line(s.m_Box, 6, 4, 3, ROW_H - 8, INT_QST_Look.Ink());
			INT_QST_Look.Icon(s.m_Box, w - 30, 27, 20, "chevron_right", INT_QST_Look.Ink());
		}
		string sign = SignOf(r);
		INT_QST_Look.Sign(s.m_Box, 18, 17, 40, sign);
		bool closed = r.St == INT_QST_St.LOCKED || r.St == INT_QST_St.COOLDOWN;
		int nc = INT_QST_Look.Ink();
		if (closed)
			nc = 0xFF7A6A52;
		string nm = INT_QST_Look.Fit(q.Name, w - 112, 16);
		INT_QST_Look.Heavy(s.m_Box, 72, 10, w - 112, 26, nm, 16, nc);
		DrawNote(s.m_Box, r, q, w);
		if (i < m_Rows.Count() - 1)
			INT_QST_Look.Hair(s.m_Box, 18, ROW_H - 1, w - 30);
	}

	static string SignOf(INT_QST_GiverRow r)
	{
		if (r.TurnIn && r.St == INT_QST_St.READY)
			return "turnin";
		if (r.St == INT_QST_St.AVAILABLE)
			return "avail";
		if (r.St == INT_QST_St.ACTIVE || r.St == INT_QST_St.READY || r.St == INT_QST_St.EXPIRED)
			return "progress";
		if (r.St == INT_QST_St.COOLDOWN)
			return "cooldown";
		return "lock";
	}

	protected void DrawNote(Widget b, INT_QST_GiverRow r, INT_QST_QuestDef q, float w)
	{
		float y = 40;
		if (r.TurnIn && r.St == INT_QST_St.READY)
		{
			INT_QST_Look.Stamp(b, 72, y, "#STR_INT_QST_ST_READY", INT_QST_Look.Red(), 10, "check");
			return;
		}
		if (r.St == INT_QST_St.AVAILABLE)
		{
			string sk = "#STR_INT_QST_ST_NEW";
			if (q.m_Instant)
				sk = "#STR_INT_QST_ST_TRADE";
			INT_QST_Look.Stamp(b, 72, y, sk, INT_QST_Look.Blue(), 10);
			float sw = INT_QST_Look.StampWidth(sk, 10);
			string lim = INT_QST_Text.Limit(q);
			INT_QST_Look.Icon(b, 72 + sw + 10, y + 4, 14, "clock", INT_QST_Look.Ink2());
			INT_QST_Look.Text(b, 72 + sw + 30, y, w - sw - 120, 22, lim, 13, INT_QST_Look.Ink2());
			return;
		}
		if (r.St == INT_QST_St.ACTIVE || r.St == INT_QST_St.READY || r.St == INT_QST_St.EXPIRED)
		{
			int stc = INT_QST_Text.StatusColor(r.St);
			string stk = INT_QST_Text.StatusKey(r.St);
			INT_QST_Look.Stamp(b, 72, y, stk, stc, 10);
			float sw2 = INT_QST_Look.StampWidth(stk, 10);
			string where = "";
			if (!r.TurnIn && q.GiverFinish != "")
				where = INT_QST_Look.Fmt("#STR_INT_QST_TURNIN_AT", INT_QST_Text.GiverName(q.GiverFinish));
			else if (r.St == INT_QST_St.ACTIVE)
			{
				int pct = (int)Math.Round(INT_QST_Sheet.ProgressPct(q));
				where = pct.ToString() + " %";
			}
			INT_QST_Look.Text(b, 72 + sw2 + 10, y, w - sw2 - 120, 22, where, 13, INT_QST_Look.Ink2());
			return;
		}
		string why = INT_QST_GiverMenu.LockReason(q, r.St, r.Wait);
		string icon = "lock";
		if (r.St == INT_QST_St.COOLDOWN)
			icon = "clock";
		int gc = 0xFF7A6A52;
		INT_QST_Look.Icon(b, 72, y + 4, 14, icon, gc);
		why = INT_QST_Look.Fit(why, w - 140, 13);
		INT_QST_Look.Text(b, 92, y, w - 140, 22, why, 13, gc);
	}

	override void OnRowClick(int i, int button)
	{
		if (i < 0 || i >= m_Rows.Count())
			return;
		m_Owner.Select(m_Rows[i].QuestId);
	}
}

class INT_QST_GiverMenu extends INT_QST_Book
{
	static const float FOLD = 500;

	static const int ID_MAIN = 1;
	static const int ID_LATER = 2;

	static string s_Key;			// set by the bridge before opening
	static string s_LastKey;
	static int s_LastSelected;

	protected string m_Key;
	protected ref INT_QST_GiverListSource m_Src;
	protected ref INT_QST_VList m_List;
	protected ref INT_QST_Sheet m_Sheet;
	protected Widget m_Buttons;
	protected Widget m_ListHead;
	protected Widget m_Legend;
	protected int m_Selected;
	protected bool m_Hooked;
	// Shop-like exchange = two requests: ACCEPT, then (on its OK) TURNIN. Answers are matched by operation AND quest
	// id; a TURNIN that fails after the ACCEPT went through sends CANCEL, so the quest does not stay taken.
	static const int X_NONE = 0;
	static const int X_ACCEPT = 1;		// ACCEPT sent, waiting
	static const int X_TURNIN = 2;		// ACCEPT ok, TURNIN sent, waiting
	static const float X_TIMEOUT = 10;	// no answer this long: the exchange is forgotten (another click may start one)
	protected int m_InstantId;
	protected int m_InstantStep;
	protected float m_InstantAge;
	protected int m_UndoId;			// CANCEL sent by the exchange itself: its answer is not shown
	protected bool m_EnterArmed;
	protected float m_LiveAcc;
	protected string m_LiveSig;

	override void GetWindowSize(out float w, out float h)
	{
		w = 1400;
		h = 840;
	}

	override float CoverTop()
	{
		return 140;
	}

	// The talk window has no own key: F (interact) opened it, Esc closes it.
	override string ToggleInput()
	{
		return "";
	}

	// --- cover --------------------------------------------------------------------------------------------------------
	override void BuildCoverHeader(Widget header, float w)
	{
		m_Key = s_Key;
		INT_QST_GiverView g = INT_QST_Client.Giver(m_Key);
		string name = INT_QST_Text.GiverName(m_Key);
		// photo card with tape
		Widget card = INT_UIF_Draw.Frame(header, 26, 16, 100, 112, "Photo");
		INT_UIF_Surface.Shadow(card, -18, -10, 136, 148, 0x80000000);
		INT_QST_Look.PaperSheet(card, 0, 0, 100, 112, INT_QST_Look.Note(), "Paper");
		INT_UIF_Draw.Line(card, 8, 8, 84, 78, 0xFF3A2F23);
		INT_QST_Look.Tex(card, 8, 8, 84, 78, "int_qst_portrait_ca", 0xFF1F1913);
		string role = INT_QST_Look.Upper("#STR_INT_QST_GIVER");
		INT_QST_Look.Text(card, 0, 88, 100, 20, role, 12, INT_QST_Look.Ink2(), INT_UIF_Align.CENTER);
		INT_QST_Look.Tex(card, 24, -9, 52, 18, "int_qst_tape_ca", 0xB3ECE4CC);
		// name, system (capitals are wider than the width estimate: 1.25 instead of 1.1, cut when still too long)
		string up = INT_QST_Look.Caps(name);
		float ns = 30;
		while (ns > 18 && INT_UIF_Draw.TextWidth(up, ns) * 1.25 > 470)
			ns = ns - 2;
		if (INT_UIF_Draw.TextWidth(up, ns) * 1.25 > 470)
			up = INT_QST_Look.Fit(up, 376, ns);
		INT_QST_Look.Heavy(header, 150, 22, 480, 40, up, ns, INT_QST_Look.Cream());
		INT_QST_Look.Icon(header, 152, 70, 17, "person", INT_QST_Look.Tan());
		string dist = INT_QST_Text.GiverDistance(m_Key);
		string sub = INT_QST_Look.Tr("#STR_INT_QST_GIVER_SUB");
		INT_QST_Look.Text(header, 176, 66, 440, 24, sub, 16, INT_QST_Look.Tan());
		INT_QST_Look.Icon(header, 152, 100, 16, "book", INT_QST_Look.Tan2());
		INT_QST_Look.Text(header, 176, 96, 440, 22, "DayZavr Quest System", 14, INT_QST_Look.Tan2());
		// greeting note
		float gx = 650;
		float gw = w - gx - 90;
		Widget note = INT_UIF_Draw.Frame(header, gx, 20, gw, 96, "Greeting");
		INT_UIF_Surface.Shadow(note, -20, -10, gw + 40, 130, 0x73000000);
		INT_QST_Look.PaperSheet(note, 0, 0, gw, 96, INT_QST_Look.Note(), "Paper");
		INT_QST_Look.PinHead(header, gx - 14, 8);
		string says = INT_QST_Look.Fmt("#STR_INT_QST_SAYS", name);
		says = INT_QST_Look.Caps(says);
		says = INT_QST_Look.Fit(says, (gw - 40) * 0.8, 11);
		INT_QST_Look.Text(note, 26, 8, gw - 40, 18, says, 11, INT_QST_Look.Ink3());
		string greet = "";
		if (g)
			greet = g.Greeting;
		if (greet == "")
			greet = INT_QST_Look.Tr("#STR_INT_QST_GREET_DEFAULT");
		array<string> lines = new array<string>();
		INT_QST_Text.Wrap("«" + greet + "»", gw - 52, 16, lines);
		float ly = 30;
		int k = 0;
		foreach (string ln : lines)
		{
			if (k >= 3)
				break;
			INT_QST_Look.Text(note, 26, ly, gw - 44, 24, ln, 16, INT_QST_Look.Blue());
			ly = ly + 22;
			k++;
		}
	}

	// --- body ---------------------------------------------------------------------------------------------------------
	override void OnBuild(Widget body)
	{
		Pages(body, FOLD);
		float bh = GetBodyH();
		float bw = GetBodyW();
		m_ListHead = INT_UIF_Draw.Frame(body, 0, 0, FOLD, 60, "ListHead");
		m_Src = new INT_QST_GiverListSource();
		m_Src.m_Owner = this;
		m_List = new INT_QST_VList();
		int bar = INT_QST_Look.InkA(30);
		int thumb = INT_QST_Look.InkA(120);
		m_List.Build(body, 8, 60, FOLD - 28, bh - 60 - 156, m_Src, bar, thumb);
		m_Legend = INT_UIF_Draw.Frame(body, 30, bh - 150, FOLD - 60, 140, "Legend");
		float rx = FOLD + 60;
		float rw = bw - rx - 34;
		m_Sheet = new INT_QST_Sheet();
		m_Sheet.m_NoteH = 120;
		m_Sheet.m_Bottom = 100;
		m_Sheet.Build(body, rx, 0, rw, bh);
		m_Buttons = INT_UIF_Draw.Frame(body, rx, bh - 84, rw, 64, "Buttons");
		AddKey("ESC", "#STR_INT_QST_K_CLOSE");
		AddKey(INT_QST_Look.Tr("#STR_INT_QST_K_LMB"), "#STR_INT_QST_K_SELECT");
		AddKey("ENTER", "#STR_INT_QST_K_ACCEPT");
		DrawLegend();
	}

	override void OnReady()
	{
		Rebuild(true);
	}

	override void OnShow()
	{
		super.OnShow();
		if (!m_Hooked)
		{
			INT_QST_Client.s_OnChanged.Insert(OnDataChanged);
			INT_QST_Client.s_OnResult.Insert(OnResult);
			m_Hooked = true;
		}
	}

	override void OnHide()
	{
		Unhook();
		super.OnHide();
	}

	void ~INT_QST_GiverMenu()
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
	}

	// s_OnChanged comes with every kill / progress packet: the rebuild is throttled (OnDirtyRefresh from Update).
	void OnDataChanged()
	{
		if (!IsClosingDown())
			MarkDirty();
	}

	override void OnDirtyRefresh()
	{
		if (!IsClosingDown())
			Rebuild(false);
	}

	// The giver's quests: ready to turn in first, then new, in progress, closed.
	protected void FillRows()
	{
		m_Src.m_Rows.Clear();
		bool showLocked = INT_QST_Client.s_Hello && INT_QST_Client.s_Hello.ShowLocked;
		array<ref INT_QST_GiverRow> ready = new array<ref INT_QST_GiverRow>();
		array<ref INT_QST_GiverRow> fresh = new array<ref INT_QST_GiverRow>();
		array<ref INT_QST_GiverRow> going = new array<ref INT_QST_GiverRow>();
		array<ref INT_QST_GiverRow> closed = new array<ref INT_QST_GiverRow>();
		foreach (INT_QST_QuestDef q : INT_QST_Client.s_Catalog.m_List)
		{
			bool starts = q.GiverStart == m_Key;
			bool ends = q.GiverFinish == m_Key;
			if (!starts && !ends)
				continue;
			INT_QST_GiverRow r = new INT_QST_GiverRow();
			r.QuestId = q.Id;
			int wait;
			r.St = INT_QST_Client.StatusOf(q, wait);
			r.Wait = wait;
			bool taken = r.St == INT_QST_St.ACTIVE || r.St == INT_QST_St.READY || r.St == INT_QST_St.EXPIRED;
			r.TurnIn = taken && ends;
			if (taken)
			{
				if (r.TurnIn && r.St == INT_QST_St.READY)
					ready.Insert(r);
				else
					going.Insert(r);
				continue;
			}
			if (!starts)
				continue;
			if (r.St == INT_QST_St.AVAILABLE)
				fresh.Insert(r);
			else if (showLocked && (r.St == INT_QST_St.LOCKED || r.St == INT_QST_St.COOLDOWN))
				closed.Insert(r);
		}
		foreach (INT_QST_GiverRow a : ready)
			m_Src.m_Rows.Insert(a);
		foreach (INT_QST_GiverRow b : fresh)
			m_Src.m_Rows.Insert(b);
		foreach (INT_QST_GiverRow c : going)
			m_Src.m_Rows.Insert(c);
		foreach (INT_QST_GiverRow d : closed)
			m_Src.m_Rows.Insert(d);
	}

	protected void Rebuild(bool toTop)
	{
		m_DataDirty = false;
		FillRows();
		DrawListHead();
		int keep = m_Selected;
		if (toTop && s_LastKey == m_Key)
			keep = s_LastSelected;
		if (!HasRow(keep))
			keep = 0;
		if (keep == 0 && m_Src.m_Rows.Count() > 0)
			keep = m_Src.m_Rows[0].QuestId;
		m_Selected = keep;
		m_Src.m_Selected = keep;
		m_List.Refresh(toTop);
		ShowSelected();
	}

	protected bool HasRow(int id)
	{
		if (id == 0)
			return false;
		foreach (INT_QST_GiverRow r : m_Src.m_Rows)
		{
			if (r.QuestId == id)
				return true;
		}
		return false;
	}

	protected INT_QST_GiverRow RowOf(int id)
	{
		foreach (INT_QST_GiverRow r : m_Src.m_Rows)
		{
			if (r.QuestId == id)
				return r;
		}
		return null;
	}

	void Select(int questId)
	{
		if (questId == m_Selected)
			return;
		m_Selected = questId;
		s_LastKey = m_Key;
		s_LastSelected = questId;
		m_Src.m_Selected = questId;
		m_List.Rebind();
		ShowSelected();
		INT_QST_Sfx.Play("page");
	}

	protected void DrawListHead()
	{
		INT_UIF_Widgets.ClearChildren(m_ListHead);
		string name = INT_QST_Text.GiverName(m_Key);
		string cap = INT_QST_Look.Fmt("#STR_INT_QST_GIVER_QUESTS", name);
		cap = INT_QST_Look.Spaced(cap);
		INT_QST_Look.Heavy(m_ListHead, 30, 20, FOLD - 170, 22, cap, 13, INT_QST_Look.Ink());
		string all = INT_QST_Look.Fmt("#STR_INT_QST_TOTAL", m_Src.m_Rows.Count().ToString());
		INT_QST_Look.Text(m_ListHead, FOLD - 150, 19, 120, 22, all, 14, INT_QST_Look.Ink3(), INT_UIF_Align.RIGHT);
		INT_QST_Look.DoubleRule(m_ListHead, 30, 49, FOLD - 60);
	}

	protected void DrawLegend()
	{
		float w = FOLD - 60;
		INT_QST_Look.DoubleRule(m_Legend, 0, 0, w);
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_LEGEND");
		INT_QST_Look.Heavy(m_Legend, 0, 12, w, 18, cap, 11, INT_QST_Look.Ink2());
		LegendItem(0, 38, "avail", "#STR_INT_QST_LG_AVAIL");
		LegendItem(w * 0.5, 38, "turnin", "#STR_INT_QST_LG_TURNIN");
		LegendItem(0, 70, "progress", "#STR_INT_QST_LG_PROGRESS");
		LegendItem(w * 0.5, 70, "lock", "#STR_INT_QST_LG_LOCKED");
		INT_QST_Look.Icon(m_Legend, 0, 108, 15, "lock", INT_QST_Look.Ink3());
		string hint = INT_QST_Look.Tr("#STR_INT_QST_LG_HIDDEN");
		INT_QST_Look.Text(m_Legend, 22, 104, w - 22, 22, hint, 13, INT_QST_Look.Ink3());
	}

	protected void LegendItem(float x, float y, string sign, string key)
	{
		INT_QST_Look.Sign(m_Legend, x, y, 28, sign);
		string t = INT_QST_Look.Tr(key);
		INT_QST_Look.Text(m_Legend, x + 36, y + 2, 200, 24, t, 14, INT_QST_Look.Ink2());
	}

	// Why a closed quest cannot be taken (first missing quest / time until it comes back).
	static string LockReason(INT_QST_QuestDef q, int st, int wait)
	{
		if (st == INT_QST_St.COOLDOWN)
			return INT_QST_Look.Fmt("#STR_INT_QST_AGAIN_IN", INT_QST_Look.Dur(wait));
		array<int> missing = new array<int>();
		int now = INT_QST_Client.Now();
		int offset = INT_QST_Client.Offset();
		INT_QST_Rules.RequiresMet(q, INT_QST_Client.s_State, INT_QST_Client.s_Catalog, now, offset, missing);
		if (missing.Count() > 0)
		{
			INT_QST_QuestDef m = INT_QST_Client.s_Catalog.Get(missing[0]);
			if (m)
			{
				string need = INT_QST_Look.Fmt("#STR_INT_QST_NEED_FIRST", m.Name);
				int more = missing.Count() - 1;
				if (more > 0 && q.RequireAny == 0)
					need = need + " " + INT_QST_Look.Fmt("#STR_INT_QST_AND_MORE", more.ToString());
				return need;
			}
		}
		return INT_QST_Look.Tr("#STR_INT_QST_ST_LOCKED");
	}

	protected void ShowSelected()
	{
		m_Clicks.ForgetUnder(m_Buttons);
		INT_UIF_Widgets.ClearChildren(m_Buttons);
		m_LiveSig = "";
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(m_Selected);
		INT_QST_GiverRow r = RowOf(m_Selected);
		if (!q || !r)
		{
			m_Sheet.ShowEmpty("#STR_INT_QST_EMPTY_GIVER", "#STR_INT_QST_EMPTY_GIVER_TEXT");
			return;
		}
		if (r.St == INT_QST_St.LOCKED || r.St == INT_QST_St.COOLDOWN)
		{
			string why = LockReason(q, r.St, r.Wait);
			m_Sheet.ShowClosed(q, why);
			return;
		}
		m_Sheet.Show(q, INT_QST_Sheet.MODE_GIVER);
		m_LiveSig = LiveSignature(q);
		DrawButtons(q, r);
	}

	protected void DrawButtons(INT_QST_QuestDef q, INT_QST_GiverRow r)
	{
		float w = GetBodyW() - (FOLD + 60) - 34;
		Widget hv;
		int yc = INT_QST_Look.Sticky();
		if (r.St == INT_QST_St.AVAILABLE)
		{
			string label = "#STR_INT_QST_B_ACCEPT";
			string trade = "";
			if (q.m_Instant)
			{
				label = "#STR_INT_QST_B_TRADE";
				trade = TradeProblem(q);
			}
			if (trade == "")
			{
				Widget mb = INT_QST_Look.Button(m_Buttons, 0, 0, 420, 60, label, "check", INT_QST_Look.BTN_INK, yc, hv);
				ButtonWidget mh = INT_UIF_Draw.Hit(mb, 0, 0, 420, 60, m_Clicks);
				m_Clicks.Bind(mh, ID_MAIN, q.Id, hv);
			}
			else
			{
				// an exchange is offered only when everything to hand over is here (else the quest would stay taken)
				INT_QST_Look.Icon(m_Buttons, 0, 20, 20, "warning", INT_QST_Look.Red());
				string tradeText = INT_QST_Look.Tr(trade);
				tradeText = INT_QST_Look.Fit(tradeText, 390, 16);
				INT_QST_Look.Heavy(m_Buttons, 30, 0, 390, 60, tradeText, 16, INT_QST_Look.Red());
			}
			Widget lb = INT_QST_Look.Button(m_Buttons, 436, 0, 170, 60, "#STR_INT_QST_B_LATER", "", INT_QST_Look.BTN_GHOST, 0, hv);
			ButtonWidget lh = INT_UIF_Draw.Hit(lb, 0, 0, 170, 60, m_Clicks);
			m_Clicks.Bind(lh, ID_LATER, 0, hv);
			SlotsInfo(w);
			return;
		}
		if (r.TurnIn && r.St == INT_QST_St.READY)
		{
			// items missing, or only items that still hold something (the server refuses those: ITEM_NOT_EMPTY)
			string block = "";
			if (!INT_QST_Client.CanTurnIn(q))
				block = "#STR_INT_QST_E_11";
			else if (INT_QST_Rules.HasInventoryTargets(q))
			{
				PlayerBase tp = PlayerBase.Cast(GetGame().GetPlayer());
				if (!INT_QST_Inv.AllCovered(tp, q, true))
					block = "#STR_INT_QST_E_24";
			}
			if (block == "")
			{
				Widget tb = INT_QST_Look.Button(m_Buttons, 0, 0, 420, 60, "#STR_INT_QST_B_TURNIN", "check", INT_QST_Look.BTN_INK, yc, hv);
				ButtonWidget th = INT_UIF_Draw.Hit(tb, 0, 0, 420, 60, m_Clicks);
				m_Clicks.Bind(th, ID_MAIN, q.Id, hv);
			}
			else
			{
				INT_QST_Look.Icon(m_Buttons, 0, 20, 20, "warning", INT_QST_Look.Red());
				string miss = INT_QST_Look.Tr(block);
				miss = INT_QST_Look.Fit(miss, w - 30, 16);
				INT_QST_Look.Heavy(m_Buttons, 30, 0, w - 30, 60, miss, 16, INT_QST_Look.Red());
			}
			return;
		}
		// in progress / taken elsewhere: a line about it
		string line = INT_QST_Look.Tr("#STR_INT_QST_IN_PROGRESS_HINT");
		if (!r.TurnIn && q.GiverFinish != "" && q.GiverFinish != m_Key)
			line = INT_QST_Look.Fmt("#STR_INT_QST_TURNIN_AT", INT_QST_Text.GiverName(q.GiverFinish));
		INT_QST_Look.Icon(m_Buttons, 0, 20, 20, "info", INT_QST_Look.Ink2());
		INT_QST_Look.Heavy(m_Buttons, 30, 0, w - 30, 60, line, 16, INT_QST_Look.Ink2());
	}

	// "" when the inventory covers a shop-like exchange right now, else the text key of why not.
	protected string TradeProblem(INT_QST_QuestDef q)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || !INT_QST_Inv.AllCovered(p, q))
			return "#STR_INT_QST_E_11";
		if (!INT_QST_Inv.AllCovered(p, q, true))
			return "#STR_INT_QST_E_24";
		return "";
	}

	protected void SlotsInfo(float w)
	{
		int have = INT_QST_Client.s_State.Active.Count();
		int slots = 0;
		if (INT_QST_Client.s_Hello)
			slots = INT_QST_Client.s_Hello.QuestSlots;
		// call results hoisted (two of them inside one argument can be clobbered)
		string haveText = have.ToString();
		string slotsText = slots.ToString();
		string counts = haveText + " / " + slotsText;
		string v = INT_QST_Look.Fmt("#STR_INT_QST_SLOTS", counts);
		float tw = INT_UIF_Draw.TextWidth(v, 14) * 1.1;
		INT_QST_Look.Icon(m_Buttons, w - tw - 24, 22, 16, "list", INT_QST_Look.Ink3());
		INT_QST_Look.Text(m_Buttons, w - tw, 0, tw, 60, v, 14, INT_QST_Look.Ink3());
	}

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
		return sig;
	}

	// --- per frame: distance, inventory changes, Enter ---------------------------------------------------------------
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (IsClosingDown())
			return;
		bool enter = KeyState(KeyCode.KC_RETURN) != 0;
		if (enter)
			m_EnterArmed = true;
		else if (m_EnterArmed)
		{
			m_EnterArmed = false;
			if (!IsAsking() && !HasModal() && m_Age > 0.3)
				MainAction(m_Selected);
		}
		if (m_InstantStep != X_NONE)
		{
			m_InstantAge = m_InstantAge + timeslice;
			if (m_InstantAge > X_TIMEOUT)
				EndExchange();
		}
		m_LiveAcc = m_LiveAcc + timeslice;
		if (m_LiveAcc < 0.5)
			return;
		m_LiveAcc = 0;
		if (TooFar())
		{
			RequestClose();
			return;
		}
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(m_Selected);
		if (!q || m_LiveSig == "")
			return;
		string sig = LiveSignature(q);
		if (sig != m_LiveSig)
			Rebuild(false);
	}

	// The server's check (INT_QST_SE_Givers.Near): 3D distance from the giver object (its configured place while the
	// object is not known) of at most TalkDistance + 1 m. Closing later left a band where every action failed TOO_FAR.
	protected bool TooFar()
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		INT_QST_GiverView g = INT_QST_Client.Giver(m_Key);
		if (!p || !g)
			return true;
		float lim = 5;
		if (INT_QST_Client.s_Hello)
			lim = INT_QST_Client.s_Hello.TalkDistance + 1.0;
		vector gp = g.Position;
		INT_QST_GiverSlot slot = INT_QST_GiverReg.Find(m_Key);
		if (slot && slot.m_Obj)
			gp = slot.m_Obj.GetPosition();
		vector pp = p.GetPosition();
		float d = vector.Distance(gp, pp);
		return d > lim;
	}

	protected void MainAction(int questId)
	{
		if (m_InstantStep != X_NONE)
			return;		// an exchange is on its way: one at a time (a second ACCEPT would answer ALREADY_ACTIVE)
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(questId);
		INT_QST_GiverRow r = RowOf(questId);
		if (!q || !r)
			return;
		if (r.St == INT_QST_St.AVAILABLE)
		{
			if (q.m_Instant)
			{
				string trade = TradeProblem(q);
				if (trade != "")
				{
					Flash(INT_QST_Look.Tr(trade));
					return;
				}
				m_InstantId = q.Id;
				m_InstantStep = X_ACCEPT;
				m_InstantAge = 0;
			}
			INT_QST_Client.Accept(q.Id, m_Key);
			INT_QST_Sfx.Play("click");
			return;
		}
		if (r.TurnIn && r.St == INT_QST_St.READY)
		{
			INT_QST_Client.TurnIn(q.Id, m_Key);
			INT_QST_Sfx.Play("click");
		}
	}

	protected void EndExchange()
	{
		m_InstantId = 0;
		m_InstantStep = X_NONE;
		m_InstantAge = 0;
	}

	override void OnClickId(int id, int data, int button)
	{
		if (CommonClick(id))
			return;
		if (id == ID_MAIN)
		{
			MainAction(data);
			return;
		}
		if (id == ID_LATER)
			RequestClose();
	}

	// Answers to ACCEPT / TURNIN (and the exchange's own CANCEL). Other answers (exploration reports ...) are not
	// this window's. An exchange step is matched by operation AND quest id, so an unrelated error leaves it alone.
	// Window closed while an exchange waits: nothing is sent — a TURNIN on its way may still succeed, and a quest
	// left taken is a shop quest that this giver takes back at once (it shows here as ready to turn in).
	void OnResult(int op, int code, int questId)
	{
		if (IsClosingDown())
			return;
		if (op == INT_QST_C2S.CANCEL && m_UndoId != 0 && questId == m_UndoId)
		{
			m_UndoId = 0;		// the exchange took its quest back: the turn-in error is already on the screen
			return;
		}
		if (op != INT_QST_C2S.ACCEPT && op != INT_QST_C2S.TURNIN)
			return;
		string codeText = code.ToString();
		bool exchange = m_InstantStep != X_NONE && questId == m_InstantId;
		if (exchange && op == INT_QST_C2S.ACCEPT && m_InstantStep == X_ACCEPT)
		{
			if (code != INT_QST_Res.OK)
			{
				EndExchange();
				Flash(INT_QST_Look.Tr("#STR_INT_QST_E_" + codeText));
				return;
			}
			INT_QST_Sfx.Play("stamp");
			m_InstantStep = X_TURNIN;
			m_InstantAge = 0;
			INT_QST_Client.TurnIn(questId, m_Key);
			return;
		}
		if (exchange && op == INT_QST_C2S.TURNIN && m_InstantStep == X_TURNIN)
		{
			EndExchange();
			if (code != INT_QST_Res.OK)
			{
				// taken but not exchanged: give it back, the player can try the exchange again
				m_UndoId = questId;
				INT_QST_Client.Cancel(questId);
				Flash(INT_QST_Look.Tr("#STR_INT_QST_E_" + codeText));
				return;
			}
			INT_QST_Sfx.Play("complete");
			return;
		}
		if (code != INT_QST_Res.OK)
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_E_" + codeText));
			return;
		}
		if (op == INT_QST_C2S.ACCEPT)
		{
			INT_QST_Sfx.Play("stamp");
			return;
		}
		INT_QST_Sfx.Play("complete");
	}
}
