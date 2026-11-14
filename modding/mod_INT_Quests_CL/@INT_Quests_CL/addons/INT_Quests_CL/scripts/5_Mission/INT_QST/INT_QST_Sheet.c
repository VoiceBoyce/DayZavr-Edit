// INT Quests (client) — the quest sheet: the right page of the journal and of the giver window (design B). Branch
// caption + status stamp, title, progress or chips, who gives / takes it, the giver's words on a pinned note
// (blue pen, scrolls by whole lines), targets as a checklist and rewards as cards (both scroll). The sheet is
// rebuilt when the shown quest or its state changes; the lists re-bind only their visible rows.

// One line of the pinned note.
class INT_QST_NoteSlot extends INT_QST_VSlot
{
	TextWidget m_Text;
}

class INT_QST_NoteSource extends INT_QST_VSource
{
	static const float LINE_H = 26;
	static const float SIZE = 16;

	ref array<string> m_Lines = new array<string>();

	override int Count()
	{
		return m_Lines.Count();
	}

	override float RowHeight(int i)
	{
		return LINE_H;
	}

	override INT_QST_VSlot NewSlot()
	{
		return new INT_QST_NoteSlot();
	}

	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_NoteSlot s = INT_QST_NoteSlot.Cast(slot);
		INT_UIF_Draw.Line(s.m_Root, 0, LINE_H - 1, w, 1, ARGB(52, 52, 78, 118));
		s.m_Text = INT_QST_Look.Text(s.m_Root, 0, 0, w, LINE_H - 2, "", SIZE, INT_QST_Look.Blue());
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_NoteSlot s = INT_QST_NoteSlot.Cast(slot);
		if (s.m_Text)
			s.m_Text.SetText(m_Lines[i]);
	}
}

// A row of a sheet column (caption, target, card, plain text).
class INT_QST_Row
{
	static const int CAPTION = 0;
	static const int TARGET = 1;
	static const int CARD = 2;
	static const int TEXT = 3;

	int Kind;
	string Glyph;
	string Text;
	string Sub;
	string Right;
	int Have;
	int Need;
	bool Done;
	bool Taken;		// quest is active: show the checkbox state
	bool Money;
	int Tint;
}

class INT_QST_RowSlot extends INT_QST_VSlot
{
	Widget m_Box;
}

class INT_QST_RowsSource extends INT_QST_VSource
{
	ref array<ref INT_QST_Row> m_Rows = new array<ref INT_QST_Row>();

	void Clear()
	{
		m_Rows.Clear();
	}

	INT_QST_Row Add(int kind, string icon, string text)
	{
		INT_QST_Row r = new INT_QST_Row();
		r.Kind = kind;
		r.Glyph = icon;
		r.Text = text;
		m_Rows.Insert(r);
		return r;
	}

	override int Count()
	{
		return m_Rows.Count();
	}

	override float RowHeight(int i)
	{
		INT_QST_Row r = m_Rows[i];
		if (r.Kind == INT_QST_Row.CAPTION)
		{
			if (i == 0)
				return 34;
			return 46;		// space above a second caption
		}
		if (r.Kind == INT_QST_Row.TARGET)
		{
			if (r.Sub != "")
				return 50;
			return 34;
		}
		if (r.Kind == INT_QST_Row.CARD)
			return 58;
		return 28;
	}

	override INT_QST_VSlot NewSlot()
	{
		return new INT_QST_RowSlot();
	}

	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_RowSlot s = INT_QST_RowSlot.Cast(slot);
		s.m_Box = INT_UIF_Draw.Frame(s.m_Root, 0, 0, w, 40, "Row");
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_RowSlot s = INT_QST_RowSlot.Cast(slot);
		INT_UIF_Widgets.ClearChildren(s.m_Box);
		float h = RowHeight(i);
		INT_UIF_Draw.Resize(s.m_Box, w, h);
		INT_QST_Row r = m_Rows[i];
		switch (r.Kind)
		{
			case INT_QST_Row.CAPTION:
				DrawCaption(s.m_Box, r, w, h);
				break;
			case INT_QST_Row.TARGET:
				DrawTarget(s.m_Box, r, w, h, i);
				break;
			case INT_QST_Row.CARD:
				DrawCard(s.m_Box, r, w);
				break;
			default:
				INT_QST_Look.Text(s.m_Box, 4, 0, w - 8, h, r.Text, 14, INT_QST_Look.Ink3());
				break;
		}
	}

	protected void DrawCaption(Widget b, INT_QST_Row r, float w, float h)
	{
		float y = h - 34;
		INT_QST_Look.Icon(b, 0, y + 2, 17, r.Glyph, INT_QST_Look.Ink2());
		string cap = INT_QST_Look.Spaced(r.Text);
		INT_QST_Look.Heavy(b, 26, y, w - 170, 22, cap, 12, INT_QST_Look.Ink());
		if (r.Right != "")
			INT_QST_Look.Text(b, w - 200, y, 200, 22, r.Right, 14, INT_QST_Look.Ink3(), INT_UIF_Align.RIGHT);
		INT_QST_Look.DoubleRule(b, 0, y + 27, w);
	}

	protected void DrawTarget(Widget b, INT_QST_Row r, float w, float h, int i)
	{
		bool first = i == 0 || m_Rows[i - 1].Kind != INT_QST_Row.TARGET;
		if (!first)
			INT_QST_Look.Hair(b, 0, 0, w);
		float cy = 6;
		if (r.Taken)
		{
			string cb = "checkbox_empty";
			int cc = INT_QST_Look.Ink2();
			if (r.Done)
			{
				cb = "checkbox_checked";
				cc = INT_QST_Look.Olive();
			}
			INT_QST_Look.Icon(b, 0, cy, 22, cb, cc);
		}
		else
		{
			INT_QST_Look.Icon(b, 2, cy + 2, 18, r.Glyph, INT_QST_Look.Ink2());
		}
		float tallyW = 0;
		if (r.Need > 1 && r.Need <= 6)
			tallyW = r.Need * 19 + 6;
		float textW = w - 34 - tallyW - 56;
		int tc = INT_QST_Look.Ink();
		if (r.Done)
			tc = INT_QST_Look.Ink3();
		INT_QST_Look.Text(b, 34, cy, textW, 22, r.Text, 15, tc);
		if (r.Done)
		{
			float sw = INT_UIF_Draw.TextWidth(r.Text, 15);
			if (sw > textW)
				sw = textW;
			INT_UIF_Draw.Line(b, 34, cy + 11, sw, 1, INT_QST_Look.InkA(150));
		}
		if (r.Sub != "")
			INT_QST_Look.Text(b, 34, cy + 22, w - 34 - 56, 18, r.Sub, 12, INT_QST_Look.Ink3());
		if (tallyW > 0)
			INT_QST_Look.Tally(b, w - 56 - tallyW, cy + 4, r.Have, r.Need, r.Done);
		int vc = INT_QST_Look.Ink();
		if (r.Done)
			vc = INT_QST_Look.Olive();
		INT_QST_Look.Heavy(b, w - 56, cy, 56, 22, r.Right, 15, vc, INT_UIF_Align.RIGHT);
	}

	protected void DrawCard(Widget b, INT_QST_Row r, float w)
	{
		float h = 50;
		int border = INT_QST_Look.InkA(70);
		int fill = 0x40FFFAEB;
		if (r.Money)
		{
			border = INT_QST_Look.RedA(150);
			fill = 0x59FFFAEB;
		}
		INT_UIF_Surface.Box(b, 0, 0, w, h, 6, fill, border);
		if (r.Money)
			INT_UIF_Draw.Line(b, 0, 0, 4, h, INT_QST_Look.Red());
		INT_UIF_Surface.Box(b, 8, 7, 36, 36, 4, 0x33FFFFFF, INT_QST_Look.InkA(60));
		int ic = r.Tint;
		if (ic == 0)
			ic = INT_QST_Look.Ink2();
		INT_QST_Look.Icon(b, 15, 14, 22, r.Glyph, ic);
		float rightW = 0;
		if (r.Money)
			rightW = INT_QST_Look.StampWidth("#STR_INT_QST_TO_ACCOUNT", 11) + 10;
		else if (r.Right != "")
			rightW = INT_UIF_Draw.TextWidth(r.Right, 16) * 1.1 + 16;
		float nw = w - 56 - rightW - 8;
		string nmText = INT_QST_Look.Fit(r.Text, nw, 15);
		string subText = INT_QST_Look.Fit(r.Sub, nw, 12);
		INT_QST_Look.Heavy(b, 54, 6, nw, 22, nmText, 15, INT_QST_Look.Ink());
		INT_QST_Look.Text(b, 54, 27, nw, 16, subText, 12, INT_QST_Look.Ink3());
		if (r.Money)
			INT_QST_Look.Stamp(b, w - rightW, 13, "#STR_INT_QST_TO_ACCOUNT", INT_QST_Look.Red(), 11);
		else if (r.Right != "")
			INT_QST_Look.Heavy(b, w - rightW, 0, rightW - 12, h, r.Right, 16, INT_QST_Look.Ink(), INT_UIF_Align.RIGHT);
	}
}

class INT_QST_Sheet
{
	static const int MODE_ACTIVE = 0;
	static const int MODE_AVAILABLE = 1;
	static const int MODE_DONE = 2;
	static const int MODE_GIVER = 3;

	protected Widget m_Root;
	protected float m_W;
	protected float m_H;
	protected ref INT_QST_VList m_NoteList;
	protected ref INT_QST_VList m_LeftList;
	protected ref INT_QST_VList m_RightList;
	protected ref INT_QST_NoteSource m_NoteSrc = new INT_QST_NoteSource();
	protected ref INT_QST_RowsSource m_LeftSrc = new INT_QST_RowsSource();
	protected ref INT_QST_RowsSource m_RightSrc = new INT_QST_RowsSource();
	int m_QuestId;
	float m_NoteH = 146;
	float m_Bottom = 80;		// space kept free under the columns (buttons)

	void Build(Widget parent, float x, float y, float w, float h)
	{
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "Sheet");
		m_W = w;
		m_H = h;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	void Clear()
	{
		m_NoteList = null;
		m_LeftList = null;
		m_RightList = null;
		INT_UIF_Widgets.ClearChildren(m_Root);
		m_QuestId = 0;
	}

	// Nothing selected / empty list.
	void ShowEmpty(string titleKey, string textKey)
	{
		Clear();
		float y = m_H * 0.32;
		INT_QST_Look.Icon(m_Root, (m_W - 48) * 0.5, y - 64, 48, "book", INT_QST_Look.InkA(110));
		string t = INT_QST_Look.Tr(titleKey);
		INT_QST_Look.Heavy(m_Root, 0, y, m_W, 34, t, 22, INT_QST_Look.Ink2(), INT_UIF_Align.CENTER);
		array<string> lines = new array<string>();
		string body = INT_QST_Look.Tr(textKey);
		INT_QST_Text.Wrap(body, m_W - 160, 15, lines);
		float ly = y + 44;
		foreach (string ln : lines)
		{
			INT_QST_Look.Text(m_Root, 80, ly, m_W - 160, 22, ln, 15, INT_QST_Look.Ink3(), INT_UIF_Align.CENTER);
			ly = ly + 24;
		}
	}

	// A closed quest at a giver: name and why it is closed; text and rewards stay hidden (spoilers).
	void ShowClosed(INT_QST_QuestDef q, string reason)
	{
		Clear();
		if (!q)
			return;
		m_QuestId = q.Id;
		INT_QST_Look.Icon(m_Root, 0, 21, 17, "book", INT_QST_Look.Red());
		string br = INT_QST_Look.Spaced(INT_QST_Client.s_Catalog.BranchTitle(q.m_Branch));
		INT_QST_Look.Heavy(m_Root, 26, 19, m_W - 260, 22, br, 12, INT_QST_Look.Red());
		float stW = INT_QST_Look.StampWidth("#STR_INT_QST_ST_LOCKED", 11, true);
		INT_QST_Look.Stamp(m_Root, m_W - stW, 16, "#STR_INT_QST_ST_LOCKED", INT_QST_Look.Ink3(), 11, "lock");
		float ts = 30;
		while (ts > 18 && INT_UIF_Draw.TextWidth(q.Name, ts) * 1.05 > m_W)
			ts = ts - 2;
		INT_QST_Look.Heavy(m_Root, 0, 46, m_W, 42, q.Name, ts, INT_QST_Look.Ink3());
		INT_QST_Look.Hair(m_Root, 0, 100, m_W);
		float ny = 130;
		float nh = 170;
		INT_UIF_Surface.Shadow(m_Root, -26, ny - 16, m_W + 52, nh + 44, 0x59000000);
		INT_QST_Look.PaperSheet(m_Root, -8, ny, m_W + 16, nh, INT_QST_Look.Note(), "Note");
		INT_QST_Look.PinHead(m_Root, -22, ny - 14);
		INT_QST_Look.Sign(m_Root, 24, ny + 26, 54, "lock");
		string t = INT_QST_Look.Tr("#STR_INT_QST_CLOSED_TITLE");
		INT_QST_Look.Heavy(m_Root, 100, ny + 24, m_W - 120, 30, t, 20, INT_QST_Look.Ink());
		array<string> lines = new array<string>();
		INT_QST_Text.Wrap(reason, m_W - 130, 16, lines);
		float ly = ny + 60;
		foreach (string ln : lines)
		{
			INT_QST_Look.Text(m_Root, 100, ly, m_W - 120, 24, ln, 16, INT_QST_Look.Blue());
			ly = ly + 25;
		}
		string hint = INT_QST_Look.Tr("#STR_INT_QST_LG_HIDDEN");
		INT_QST_Look.Text(m_Root, 100, ny + nh - 34, m_W - 120, 22, hint, 13, INT_QST_Look.Ink3());
	}

	void Show(INT_QST_QuestDef q, int mode)
	{
		Clear();
		if (!q)
			return;
		m_QuestId = q.Id;
		int wait;
		int st = INT_QST_Client.StatusOf(q, wait);
		bool taken = st == INT_QST_St.ACTIVE || st == INT_QST_St.READY || st == INT_QST_St.EXPIRED;
		INT_QST_Active a = INT_QST_Client.s_State.FindActive(q.Id);

		// --- caption row: branch · place, status stamp
		int place;
		int total;
		INT_QST_Client.s_Catalog.BranchPlace(q, place, total);
		INT_QST_Look.Icon(m_Root, 0, 21, 17, "book", INT_QST_Look.Red());
		string br = INT_QST_Look.Spaced(INT_QST_Client.s_Catalog.BranchTitle(q.m_Branch));
		float brW = INT_UIF_Draw.TextWidth(br, 12) * 0.86;
		float maxBr = m_W - 380;
		if (brW > maxBr)
			brW = maxBr;
		INT_QST_Look.Heavy(m_Root, 26, 19, brW, 22, br, 12, INT_QST_Look.Red());
		string sk = INT_QST_Text.StatusKey(st);
		if (mode == MODE_GIVER && st == INT_QST_St.AVAILABLE && q.m_Instant)
			sk = "#STR_INT_QST_ST_TRADE";
		float stW = INT_QST_Look.StampWidth(sk, 11);
		int stc = INT_QST_Text.StatusColor(st);
		INT_QST_Look.Stamp(m_Root, m_W - stW, 16, sk, stc, 11);
		string placeT = place.ToString();
		string totalT = total.ToString();
		string pl = INT_QST_Look.Fmt("#STR_INT_QST_PLACE", placeT, totalT);
		string pls = "· " + INT_QST_Look.Spaced(pl);
		float plW = m_W - stW - (34 + brW) - 12;
		INT_QST_Look.Heavy(m_Root, 34 + brW, 19, plW, 22, pls, 12, INT_QST_Look.Ink3());

		// --- title (shrinks for long names)
		float ts = 30;
		while (ts > 18 && INT_UIF_Draw.TextWidth(q.Name, ts) * 1.05 > m_W)
			ts = ts - 2;
		INT_QST_Look.Heavy(m_Root, 0, 46, m_W, 42, q.Name, ts, INT_QST_Look.Ink());

		// --- progress (taken) or chips
		int doneTargets = 0;
		int nt = q.Targets.Count();
		for (int i = 0; i < nt; i++)
		{
			INT_QST_TargetDef t = q.Targets[i];
			if (taken && INT_QST_Client.TargetValue(q, i) >= t.m_Need)
				doneTargets++;
		}
		if (taken)
		{
			string progCap = INT_QST_Look.Spaced("#STR_INT_QST_PROGRESS");
			int capC = INT_QST_Look.Ink3();
			INT_QST_Look.Text(m_Root, 0, 92, 110, 22, progCap, 12, capC);
			float pct = 0;
			if (nt > 0)
				pct = ProgressPct(q);
			INT_QST_Look.Bar(m_Root, 120, 97, 240, 12, pct);
			int pi = (int)Math.Round(pct);
			string pct2 = pi.ToString() + " %";
			int inkC = INT_QST_Look.Ink();
			INT_QST_Look.Heavy(m_Root, 372, 92, 70, 22, pct2, 16, inkC);
			string dtT = doneTargets.ToString();
			string ntT = nt.ToString();
			string td = INT_QST_Look.Fmt("#STR_INT_QST_TARGETS_DONE", dtT, ntT);
			INT_QST_Look.Text(m_Root, 444, 92, m_W - 444, 22, "· " + td, 15, INT_QST_Look.Ink3());
		}
		else
		{
			float cx = 0;
			float cw;
			string lim = INT_QST_Text.Limit(q);
			INT_QST_Look.Chip(m_Root, cx, 91, "clock", lim, cw);
			cx = cx + cw + 10;
			string rep = INT_QST_Text.Repeat(q);
			INT_QST_Look.Chip(m_Root, cx, 91, "refresh", rep, cw);
			cx = cx + cw + 10;
			INT_QST_Done d = INT_QST_Client.s_State.FindDone(q.Id);
			if (d && d.Times > 0)
			{
				string times = INT_QST_Look.Fmt("#STR_INT_QST_DONE_TIMES", d.Times.ToString());
				INT_QST_Look.Chip(m_Root, cx, 91, "check", times, cw);
				cx = cx + cw + 10;
			}
			if (st == INT_QST_St.COOLDOWN && wait > 0)
			{
				string again = INT_QST_Look.Fmt("#STR_INT_QST_AGAIN_IN", INT_QST_Look.Dur(wait));
				INT_QST_Look.Chip(m_Root, cx, 91, "clock", again, cw);
			}
		}

		// --- who / where / repeat / limit
		INT_QST_Look.Hair(m_Root, 0, 122, m_W);
		float ix = 0;
		ix = Info(ix, 128, "person", "#STR_INT_QST_I_GIVES", INT_QST_Text.StartText(q), true);
		ix = Info(ix, 128, "hand", "#STR_INT_QST_I_TURNIN", INT_QST_Text.FinishText(q), true);
		string distKey = q.GiverStart;
		if (taken)
			distKey = q.GiverFinish;
		string dist = INT_QST_Text.GiverDistance(distKey);
		if (dist != "" && mode != MODE_GIVER)
			Info(ix, 128, "map_pin", "#STR_INT_QST_I_DIST", dist, false);
		ix = 0;
		ix = Info(ix, 162, "refresh", "#STR_INT_QST_I_REPEAT", INT_QST_Text.Repeat(q), true);
		if (taken && q.TimeLimit > 0 && a)
		{
			int left = INT_QST_Rules.TimeLeft(q, a, INT_QST_Client.Now());
			string lt = INT_QST_Look.Dur(left);
			if (st == INT_QST_St.EXPIRED)
				lt = INT_QST_Look.Tr("#STR_INT_QST_ST_EXPIRED");
			else if (left < 0)
				lt = INT_QST_Look.Tr("#STR_INT_QST_CLOCK_STOPPED");
			Info(ix, 162, "clock", "#STR_INT_QST_I_LEFT", lt, false);
		}
		else
		{
			Info(ix, 162, "clock", "#STR_INT_QST_I_LIMIT", INT_QST_Text.Limit(q), false);
		}
		INT_QST_Look.Hair(m_Root, 0, 200, m_W);

		// --- the giver's words: a pinned note in blue pen
		float ny = 218;
		float nh = m_NoteH;
		INT_UIF_Surface.Shadow(m_Root, -26, ny - 16, m_W + 52, nh + 44, 0x59000000);
		INT_QST_Look.PaperSheet(m_Root, -8, ny, m_W + 16, nh, INT_QST_Look.Note(), "Note");
		INT_QST_Look.PinHead(m_Root, -22, ny - 14);
		string body = q.m_Text;
		if (body == "")
			body = INT_QST_Look.Tr("#STR_INT_QST_NO_TEXT");
		INT_QST_Text.Wrap(body, m_W - 70, INT_QST_NoteSource.SIZE, m_NoteSrc.m_Lines);
		string sign = INT_QST_Text.StartText(q);
		float listH = nh - 16 - 24;
		m_NoteList = new INT_QST_VList();
		int bar = INT_QST_Look.InkA(30);
		int thumb = INT_QST_Look.InkA(110);
		m_NoteList.Build(m_Root, 16, ny + 10, m_W - 30, listH, m_NoteSrc, bar, thumb);
		INT_QST_Look.Text(m_Root, 0, ny + nh - 26, m_W - 20, 20, "— " + sign, 14, INT_QST_Look.Ink3(), INT_UIF_Align.RIGHT);

		// --- targets | quest items + rewards
		float cy = ny + nh + 22;
		float ch = m_H - m_Bottom - cy;
		if (ch < 60)
			ch = 60;
		float lw = m_W * 0.56;
		float rx = lw + 30;
		float rw = m_W - rx;
		FillTargets(q, taken, doneTargets, mode);
		FillRewards(q, st, mode);
		m_LeftList = new INT_QST_VList();
		m_LeftList.Build(m_Root, 0, cy, lw, ch, m_LeftSrc, bar, thumb);
		m_RightList = new INT_QST_VList();
		m_RightList.Build(m_Root, rx, cy, rw, ch, m_RightSrc, bar, thumb);
	}

	// Average of the target fractions (0..100).
	static float ProgressPct(INT_QST_QuestDef q)
	{
		int n = q.Targets.Count();
		if (n == 0)
			return 0;
		float sum = 0;
		for (int i = 0; i < n; i++)
		{
			INT_QST_TargetDef t = q.Targets[i];
			int v = INT_QST_Client.TargetValue(q, i);
			sum = sum + v / (float)t.m_Need;
		}
		return sum * 100.0 / n;
	}

	// "ICON LABEL value" block of the info rows; returns the next x.
	protected float Info(float x, float y, string icon, string labelKey, string value, bool sep)
	{
		if (x > m_W - 120)
			return x;
		INT_QST_Look.Icon(m_Root, x, y + 8, 17, icon, INT_QST_Look.Ink3());
		string lab = INT_QST_Look.Spaced(labelKey);
		float lw = INT_UIF_Draw.TextWidth(lab, 12) * 1.04 + 8;
		INT_QST_Look.Text(m_Root, x + 24, y + 6, lw, 22, lab, 12, INT_QST_Look.Ink3());
		float vw = INT_UIF_Draw.TextWidth(value, 15) * 1.08 + 4;
		float maxV = m_W - (x + 24 + lw) - 4;
		if (vw > maxV)
			vw = maxV;
		INT_QST_Look.Heavy(m_Root, x + 24 + lw, y + 5, vw, 24, value, 15, INT_QST_Look.Ink());
		float nx = x + 24 + lw + vw + 14;
		if (sep && nx < m_W - 140)
		{
			INT_UIF_Draw.Line(m_Root, nx, y + 8, 1, 18, INT_QST_Look.InkA(70));
			nx = nx + 16;
		}
		return nx;
	}

	protected void FillTargets(INT_QST_QuestDef q, bool taken, int doneTargets, int mode)
	{
		m_LeftSrc.Clear();
		string capKey = "#STR_INT_QST_TARGETS";
		if (q.m_Instant)
			capKey = "#STR_INT_QST_BRING";
		INT_QST_Row cap = m_LeftSrc.Add(INT_QST_Row.CAPTION, "crosshair", capKey);
		int nt = q.Targets.Count();
		if (taken)
		{
			string dT = doneTargets.ToString();
			string nT = nt.ToString();
			cap.Right = INT_QST_Look.Fmt("#STR_INT_QST_DONE_OF", dT, nT);
		}
		if (nt == 0)
		{
			m_LeftSrc.Add(INT_QST_Row.TEXT, "", INT_QST_Look.Tr("#STR_INT_QST_NO_TARGETS"));
			return;
		}
		for (int i = 0; i < nt; i++)
		{
			INT_QST_TargetDef t = q.Targets[i];
			string tIcon = INT_QST_Text.TargetIcon(t.m_Type);
			string tText = INT_QST_Text.Target(t);
			INT_QST_Row r = m_LeftSrc.Add(INT_QST_Row.TARGET, tIcon, tText);
			r.Sub = INT_QST_Text.TargetCond(t);
			r.Need = t.m_Need;
			r.Taken = taken;
			if (taken || INT_QST_TT.IsInventory(t.m_Type))
				r.Have = INT_QST_Client.TargetValue(q, i);
			r.Done = taken && r.Have >= r.Need;
			r.Right = r.Have.ToString() + "/" + r.Need.ToString();
		}
	}

	protected void FillRewards(INT_QST_QuestDef q, int st, int mode)
	{
		m_RightSrc.Clear();
		if (q.QuestItems.Count() > 0)
		{
			string qiKey = "#STR_INT_QST_ON_ACCEPT";
			if (st == INT_QST_St.ACTIVE || st == INT_QST_St.READY || st == INT_QST_St.EXPIRED)
				qiKey = "#STR_INT_QST_GIVEN_ITEMS";
			m_RightSrc.Add(INT_QST_Row.CAPTION, "letter", qiKey);
			foreach (INT_QST_ItemRef it : q.QuestItems)
			{
				INT_QST_Row qi = m_RightSrc.Add(INT_QST_Row.CARD, "letter", INT_QST_Text.Name(it.Item));
				qi.Sub = INT_QST_Look.Tr("#STR_INT_QST_QUEST_ITEM");
				qi.Right = "× " + it.Count.ToString();
				qi.Tint = INT_QST_Look.Blue();
			}
		}
		m_RightSrc.Add(INT_QST_Row.CAPTION, "star", "#STR_INT_QST_REWARD");
		if (q.Rewards.Count() == 0)
		{
			m_RightSrc.Add(INT_QST_Row.TEXT, "", INT_QST_Look.Tr("#STR_INT_QST_NO_REWARD"));
			return;
		}
		foreach (INT_QST_RewardDef rd : q.Rewards)
		{
			string rIcon = INT_QST_Text.RewardIcon(rd);
			string rName = INT_QST_Text.RewardName(rd);
			INT_QST_Row r = m_RightSrc.Add(INT_QST_Row.CARD, rIcon, rName);
			if (rd.m_Type == INT_QST_RT.MONEY)
			{
				r.Money = true;
				r.Tint = INT_QST_Look.Red();
				r.Sub = INT_QST_Look.Tr("#STR_INT_QST_BANK_TRANSFER");
			}
			else
			{
				r.Sub = INT_QST_Look.Tr("#STR_INT_QST_TO_INVENTORY");
				if (rd.m_State >= 0)
					r.Sub = r.Sub + " · " + INT_QST_Text.StateName(rd.m_State);
				r.Right = "× " + rd.Count.ToString();
			}
		}
	}

	// Wheel over the page: lists handle their own wheel; nothing else scrolls.
	void Refresh()
	{
		if (m_LeftList)
			m_LeftList.Rebind();
	}
}
