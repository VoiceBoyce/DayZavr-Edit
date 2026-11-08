// INT GlobalChat — line renderer for the open window and the idle feed (spec.md §5.1, §5.4).
// Lines are laid out once per width / font (INT_GC_LineLayout) and only lines inside the viewport get widgets.
// Scrolling moves whole visual rows; a row that would cross the top edge is hidden, never cut (clipchildren does
// not cut text). New lines append incrementally: existing frames are only moved.

class INT_GC_LineView
{
	int Seq;
	Widget Frame;
	ref INT_GC_LineLayout Lay;
	ref array<Widget> RowFrames;
	ref INT_UIF_Surface Plate;
	ImageWidget Grad;
	ImageWidget Bar;
	ButtonWidget NickHit;

	void INT_GC_LineView()
	{
		RowFrames = new array<Widget>();
	}

	void Destroy()
	{
		if (Frame)
			Frame.Unlink();
		Frame = null;
		RowFrames.Clear();
		Plate = null;
		Grad = null;
		Bar = null;
		NickHit = null;
	}
}

class INT_GC_Area
{
	protected Widget m_Clip;
	protected float m_W;
	protected float m_H;
	protected float m_Size = 16;
	protected bool m_ShowTime = true;
	protected bool m_Feed;
	protected int m_MaxRows;				// 0 = as many as fit
	protected ScriptedWidgetEventHandler m_HitHandler;
	protected ref array<ref INT_GC_Line> m_Src;
	protected ref map<int, ref INT_GC_LineLayout> m_Lay;
	protected ref map<int, ref INT_GC_LineView> m_Views;
	protected ref map<Widget, int> m_HitSeq;
	protected ref map<int, ref INT_GC_Line> m_BySeq;
	protected int m_Scroll;					// visual rows scrolled up from the newest
	protected int m_TotalRows;
	protected int m_NewBelow;
	protected int m_FirstVisibleEntry;
	protected int m_VisibleRows;

	// flat list of visual rows (reused, no allocation per refresh)
	protected ref array<int> m_EntLine;		// index into m_Src
	protected ref array<int> m_EntRow;
	protected ref array<float> m_EntH;
	protected ref map<int, int> m_VisMin;	// seq -> first visible row
	protected ref map<int, int> m_VisMax;
	protected ref map<int, float> m_VisTop;	// seq -> frame top

	void INT_GC_Area()
	{
		m_Lay = new map<int, ref INT_GC_LineLayout>();
		m_Views = new map<int, ref INT_GC_LineView>();
		m_HitSeq = new map<Widget, int>();
		m_BySeq = new map<int, ref INT_GC_Line>();
		m_EntLine = new array<int>();
		m_EntRow = new array<int>();
		m_EntH = new array<float>();
		m_VisMin = new map<int, int>();
		m_VisMax = new map<int, int>();
		m_VisTop = new map<int, float>();
	}

	void ~INT_GC_Area()
	{
		Destroy();
	}

	void Create(Widget parent, float x, float y, float w, float h, bool feed, ScriptedWidgetEventHandler hitHandler)
	{
		Destroy();
		m_Feed = feed;
		m_HitHandler = hitHandler;
		m_W = w;
		m_H = h;
		m_Clip = INT_UIF_Draw.Clip(parent, x, y, w, h, "Lines");
	}

	void Destroy()
	{
		ClearViews();
		if (m_Clip)
			m_Clip.Unlink();
		m_Clip = null;
	}

	Widget GetRoot()
	{
		return m_Clip;
	}

	void SetMaxRows(int rows)
	{
		m_MaxRows = rows;
	}

	void SetSource(array<ref INT_GC_Line> src)
	{
		m_Src = src;
	}

	void SetMetrics(float size, bool showTime)
	{
		if (Math.AbsFloat(size - m_Size) < 0.01 && showTime == m_ShowTime)
			return;
		m_Size = size;
		m_ShowTime = showTime;
		Relayout();
	}

	// Moves / resizes the viewport (design px, inside its parent). A new width re-wraps every line.
	void Place(float x, float y, float w, float h)
	{
		if (!m_Clip)
			return;
		INT_UIF_Draw.Place(m_Clip, x, y, w, h);
		bool rewrap = Math.AbsFloat(w - m_W) > 0.5;
		m_W = w;
		m_H = h;
		if (rewrap)
			Relayout();
	}

	void Relayout()
	{
		ClearViews();
		m_Lay.Clear();
	}

	protected void ClearViews()
	{
		foreach (int seq, INT_GC_LineView v : m_Views)
		{
			if (v)
				v.Destroy();
		}
		m_Views.Clear();
		m_HitSeq.Clear();
	}

	// Forgets lines that left the source (history trimmed, block list).
	void Prune()
	{
		if (!m_Src)
			return;
		m_BySeq.Clear();
		foreach (INT_GC_Line l : m_Src)
			m_BySeq.Set(l.Seq, l);
		array<int> drop = new array<int>();
		foreach (int seq, INT_GC_LineLayout lay : m_Lay)
		{
			if (!m_BySeq.Contains(seq))
				drop.Insert(seq);
		}
		foreach (int d : drop)
		{
			m_Lay.Remove(d);
			INT_GC_LineView v;
			if (m_Views.Find(d, v))
			{
				if (v)
					v.Destroy();
				m_Views.Remove(d);
			}
		}
	}

	INT_GC_LineLayout LayoutOf(INT_GC_Line l)
	{
		INT_GC_LineLayout lay;
		if (m_Lay.Find(l.Seq, lay))
			return lay;
		float w = m_W - 6;
		lay = INT_GC_LineLayout.Build(l, w, m_Size, m_ShowTime);
		m_Lay.Set(l.Seq, lay);
		return lay;
	}

	// --- scrolling ------------------------------------------------------------------------------------------
	int GetScroll()
	{
		return m_Scroll;
	}

	int NewBelow()
	{
		return m_NewBelow;
	}

	int TotalRows()
	{
		return m_TotalRows;
	}

	int VisibleRows()
	{
		return m_VisibleRows;
	}

	int FirstVisibleEntry()
	{
		return m_FirstVisibleEntry;
	}

	void ScrollBy(int rows)
	{
		m_Scroll = m_Scroll + rows;
		ClampScroll();
		if (m_Scroll == 0)
			m_NewBelow = 0;
		Refresh();
	}

	void ScrollToBottom()
	{
		m_Scroll = 0;
		m_NewBelow = 0;
		Refresh();
	}

	protected void ClampScroll()
	{
		int maxScroll = m_TotalRows - m_VisibleRows;
		if (maxScroll < 0)
			maxScroll = 0;
		if (m_Scroll > maxScroll)
			m_Scroll = maxScroll;
		if (m_Scroll < 0)
			m_Scroll = 0;
	}

	// A line arrived at the end of the source. A reader scrolled up keeps his place.
	void OnAppended(INT_GC_Line l)
	{
		if (m_Scroll > 0 && l)
		{
			INT_GC_LineLayout lay = LayoutOf(l);
			m_Scroll = m_Scroll + lay.RowCount();
			m_NewBelow++;
		}
		Refresh();
	}

	// --- the build ------------------------------------------------------------------------------------------
	protected void BuildEntries()
	{
		m_EntLine.Clear();
		m_EntRow.Clear();
		m_EntH.Clear();
		m_TotalRows = 0;
		if (!m_Src)
			return;
		int n = m_Src.Count();
		for (int i = 0; i < n; i++)
		{
			INT_GC_Line l = m_Src.Get(i);
			INT_GC_LineLayout lay = LayoutOf(l);
			int rows = lay.RowCount();
			float pad = lay.TopPad();
			for (int r = 0; r < rows; r++)
			{
				float h = lay.RowH;
				if (r == 0)
					h = h + pad;
				if (r == rows - 1)
					h = h + pad;
				m_EntLine.Insert(i);
				m_EntRow.Insert(r);
				m_EntH.Insert(h);
			}
			m_TotalRows = m_TotalRows + rows;
		}
	}

	void Refresh()
	{
		if (!m_Clip)
			return;
		Prune();
		BuildEntries();
		m_VisMin.Clear();
		m_VisMax.Clear();
		m_VisTop.Clear();
		int count = m_EntLine.Count();
		// rows that fit, counted from the newest one up
		float used = 0;
		int fit = 0;
		int e = count - 1 - m_Scroll;
		if (e > count - 1)
			e = count - 1;
		float limitH = m_H;
		while (e >= 0)
		{
			float h = m_EntH.Get(e);
			if (used + h > limitH + 0.5)
				break;
			if (m_MaxRows > 0 && fit >= m_MaxRows)
				break;
			used = used + h;
			fit++;
			e--;
		}
		m_VisibleRows = fit;
		ClampScroll();
		int first = count - m_Scroll - fit;
		if (first < 0)
			first = 0;
		// the idle feed shows whole messages only: a message whose first rows do not fit is left out
		if (m_Feed)
		{
			int keepFirst = first;
			int keepFit = fit;
			while (first < count && fit > 0 && m_EntRow.Get(first) != 0)
			{
				used = used - m_EntH.Get(first);
				first++;
				fit--;
			}
			// the newest message alone is taller than the feed: show its head (time, nick, first rows)
			if (fit == 0 && keepFit > 0)
			{
				first = keepFirst + keepFit - 1;
				while (first > 0 && m_EntRow.Get(first) != 0)
					first--;
				used = 0;
				int nextEntry = first;
				while (nextEntry < count)
				{
					float nh = m_EntH.Get(nextEntry);
					if (used + nh > limitH + 0.5)
						break;
					if (m_MaxRows > 0 && fit >= m_MaxRows)
						break;
					used = used + nh;
					fit++;
					nextEntry++;
				}
			}
			m_VisibleRows = fit;
		}
		m_FirstVisibleEntry = first;
		int last = first + fit - 1;
		// bottom-aligned: the newest visible row touches the bottom edge
		float y = m_H - used;
		for (int k = first; k <= last; k++)
		{
			int li = m_EntLine.Get(k);
			int row = m_EntRow.Get(k);
			INT_GC_Line line = m_Src.Get(li);
			int seq = line.Seq;
			if (!m_VisMin.Contains(seq))
			{
				m_VisMin.Set(seq, row);
				INT_GC_LineLayout lay = LayoutOf(line);
				float offset = 0;
				if (row > 0)
					offset = lay.TopPad() + row * lay.RowH;
				m_VisTop.Set(seq, y - offset);
			}
			m_VisMax.Set(seq, row);
			y = y + m_EntH.Get(k);
		}
		// drop frames that left the viewport
		array<int> gone = new array<int>();
		foreach (int vs, INT_GC_LineView v : m_Views)
		{
			if (!m_VisMin.Contains(vs))
				gone.Insert(vs);
		}
		foreach (int g : gone)
		{
			INT_GC_LineView gv;
			if (m_Views.Find(g, gv))
			{
				if (gv && gv.NickHit)
					m_HitSeq.Remove(gv.NickHit);
				if (gv)
					gv.Destroy();
			}
			m_Views.Remove(g);
		}
		// create / place the visible ones
		foreach (int ss, int minRow : m_VisMin)
		{
			INT_GC_Line sl = m_BySeq.Get(ss);
			if (!sl)
				continue;
			INT_GC_LineView view;
			if (!m_Views.Find(ss, view) || !view || !view.Frame)
			{
				view = Materialize(sl);
				if (!view)
					continue;
				m_Views.Set(ss, view);
			}
			float top = m_VisTop.Get(ss);
			int maxRow = m_VisMax.Get(ss);
			PlaceView(view, top, minRow, maxRow);
		}
	}

	protected void PlaceView(INT_GC_LineView v, float top, int minRow, int maxRow)
	{
		INT_UIF_Draw.Move(v.Frame, 0, top);
		int rows = v.RowFrames.Count();
		for (int r = 0; r < rows; r++)
		{
			Widget rf = v.RowFrames.Get(r);
			bool on = r >= minRow && r <= maxRow;
			if (rf)
				rf.Show(on);
		}
		INT_GC_LineLayout lay = v.Lay;
		float pTop = 0;
		if (minRow > 0)
			pTop = lay.TopPad() + minRow * lay.RowH;
		float pBot = lay.Height();
		if (maxRow < rows - 1)
			pBot = lay.TopPad() + (maxRow + 1) * lay.RowH;
		float ph = pBot - pTop;
		if (v.Plate && v.Plate.GetRoot())
			INT_UIF_Draw.Place(v.Plate.GetRoot(), 0, pTop, m_W, ph);
		if (v.Grad)
			INT_UIF_Draw.Place(v.Grad, 0, pTop, m_W * 0.8, ph);
		if (v.Bar)
			INT_UIF_Draw.Place(v.Bar, 0, pTop, 3, ph);
		if (v.NickHit)
			v.NickHit.Show(minRow == 0);
	}

	// --- one line ---------------------------------------------------------------------------------------------
	protected INT_GC_LineView Materialize(INT_GC_Line l)
	{
		INT_GC_LineLayout lay = LayoutOf(l);
		INT_GC_LineView v = new INT_GC_LineView();
		v.Seq = l.Seq;
		v.Lay = lay;
		float h = lay.Height();
		v.Frame = INT_UIF_Draw.Frame(m_Clip, 0, 0, m_W, h, "L" + l.Seq.ToString());
		if (!v.Frame)
			return null;
		// low-alpha bright layers look much stronger in game than their alpha (measured 2026-10-12): keep them faint
		int accent = AccentOf(l);
		bool bar = false;
		if (lay.Plate)
		{
			// window: a faint tint of the kind colour; idle feed over the world: a dark glass band (readable on
			// any background), the colour stays on the bar and the tag
			int plateFill = INT_UIF_Color.WithAlpha(accent, 6);
			if (m_Feed)
				plateFill = ARGB(130, 10, 8, 5);
			v.Plate = INT_UIF_Surface.Box(v.Frame, 0, 0, m_W, h, 4, plateFill, 0);
			bar = true;
		}
		else if (l.Mention)
		{
			int mentionCol = INT_GC_Look.Mention();
			int mentionFill = INT_UIF_Color.WithAlpha(mentionCol, 30);
			v.Grad = INT_UIF_Draw.Image(v.Frame, 0, 0, m_W * 0.8, h, "iuf_grad_h", mentionFill);
			bar = true;
		}
		else if (l.Mine && !m_Feed)
		{
			int mineFill = INT_GC_Look.BrassA(14);
			v.Grad = INT_UIF_Draw.Image(v.Frame, 0, 0, m_W * 0.8, h, "iuf_grad_h", mineFill);
			bar = true;
		}
		if (bar)
		{
			int barCol = accent;
			if (l.Mention)
				barCol = INT_GC_Look.Mention();
			if (l.Mine && !lay.Plate)
				barCol = INT_GC_Look.BrassA(190);
			v.Bar = INT_UIF_Draw.Line(v.Frame, 0, 0, 3, h, barCol);
		}
		int rows = lay.RowCount();
		float pad = lay.TopPad();
		int textCol = TextColorOf(l);
		for (int r = 0; r < rows; r++)
		{
			Widget rf = INT_UIF_Draw.Frame(v.Frame, 0, pad + r * lay.RowH, m_W, lay.RowH, "R" + r.ToString());
			v.RowFrames.Insert(rf);
			if (!rf)
				continue;
			if (r == 0)
				BuildPrefix(rf, l, lay, accent);
			INT_GC_Row row = lay.Rows.Get(r);
			if (row.Text != "")
			{
				TextWidget tw = INT_UIF_Draw.Text(rf, row.X, 0, m_W - row.X + 40, lay.RowH, row.Text, lay.Size, textCol);
				Shade(tw);
			}
		}
		if (m_HitHandler && lay.NickText != "")
		{
			v.NickHit = INT_UIF_Draw.Hit(v.Frame, lay.NickX - 2, pad, lay.NickW + 4, lay.RowH, m_HitHandler);
			if (v.NickHit)
				m_HitSeq.Set(v.NickHit, l.Seq);
		}
		return v;
	}

	protected void BuildPrefix(Widget rf, INT_GC_Line l, INT_GC_LineLayout lay, int accent)
	{
		float rh = lay.RowH;
		if (lay.TimeText != "")
		{
			float timeSize = INT_GC_LineLayout.TimeSize(lay.Size);
			int timeCol = INT_GC_Look.Time();
			TextWidget tt = INT_UIF_Draw.Text(rf, lay.TimeX, 0, lay.TimeW + 6, rh, lay.TimeText, timeSize, timeCol);
			Shade(tt);
		}
		if (lay.TagText != "")
		{
			float th = Math.Round(lay.Size * 1.0);
			float ty = (rh - th) * 0.5;
			int chipFill = INT_UIF_Color.WithAlpha(accent, 34);
			int chipBorder = INT_UIF_Color.WithAlpha(accent, 120);
			INT_UIF_Surface.Pill(rf, lay.TagX, ty, lay.TagW, th, chipFill, chipBorder);
			float tx = lay.TagX + 5;
			float ts = INT_GC_LineLayout.TagSize(lay.Size);
			if (lay.TagIcon != "")
			{
				INT_GC_Look.Cell(rf, tx, (rh - ts) * 0.5, ts, ts, lay.TagIcon, accent);
				tx = tx + ts + 4;
			}
			INT_UIF_Draw.Text(rf, tx, 0, lay.TagW, rh, lay.TagText, ts, accent);
		}
		if (lay.HasAdmin)
		{
			float ah = Math.Round(lay.Size * 0.96);
			float ay = (rh - ah) * 0.5;
			int admCol = INT_GC_Look.Admin();
			int admFill = INT_UIF_Color.WithAlpha(admCol, 60);
			INT_UIF_Surface.Pill(rf, lay.AdminX, ay, lay.AdminW, ah, admFill, admCol);
			string adm = Widget.TranslateString("#STR_INT_GC_ADMIN");
			float admSize = INT_GC_LineLayout.TagSize(lay.Size);
			int admText = ARGB(255, 255, 246, 226);
			INT_UIF_Draw.Text(rf, lay.AdminX, 0, lay.AdminW, rh, adm, admSize, admText, INT_UIF_Align.CENTER);
		}
		if (lay.NickText != "")
		{
			int nickCol = NickColorOf(l);
			TextWidget nt = INT_UIF_Draw.Text(rf, lay.NickX, 0, lay.NickW + 8, rh, lay.NickText, lay.Size, nickCol);
			Shade(nt);
		}
	}

	protected void Shade(TextWidget tw)
	{
		if (!tw || !m_Feed)
			return;
		tw.SetShadow(2, ARGB(255, 0, 0, 0), 0.85, 1, 1);
	}

	// --- colours by kind --------------------------------------------------------------------------------------
	static int AccentOf(INT_GC_Line l)
	{
		int kind = l.Kind;
		if (kind == INT_GC_Line.KIND_MOD)
			return INT_GC_Look.Mod();
		if (kind == INT_GC_Line.KIND_NOTICE)
			return INT_GC_Look.Notice(l.Level);
		if (kind == INT_GC_Line.KIND_LOCAL)
			return INT_GC_Look.Local();
		if (kind == INT_GC_Line.KIND_PM_IN || kind == INT_GC_Line.KIND_PM_OUT)
			return INT_GC_Look.Pm();
		if (kind == INT_GC_Line.KIND_SYSTEM && l.Color != 0)
			return l.Color;
		return INT_GC_Look.Brass();
	}

	static int TextColorOf(INT_GC_Line l)
	{
		int kind = l.Kind;
		if (kind == INT_GC_Line.KIND_MOD)
			return ARGB(255, 245, 214, 180);
		if (kind == INT_GC_Line.KIND_SYSTEM)
		{
			if (l.Color != 0)
				return l.Color;
			return INT_GC_Look.SystemText();
		}
		if (kind == INT_GC_Line.KIND_NOTICE)
			return INT_GC_Look.Notice(l.Level);
		if (kind == INT_GC_Line.KIND_JOIN)
			return INT_GC_Look.Muted();
		if (kind == INT_GC_Line.KIND_PM_IN || kind == INT_GC_Line.KIND_PM_OUT)
		{
			int pm = INT_GC_Look.Pm();
			int tx = INT_GC_Look.Text();
			return INT_UIF_Color.Lerp(pm, tx, 0.55);
		}
		return INT_GC_Look.Text();
	}

	static int NickColorOf(INT_GC_Line l)
	{
		if (l.Kind == INT_GC_Line.KIND_PM_OUT)
			return INT_GC_Look.Pm();
		if (l.ColorIdx >= 0)
			return INT_GC_Palette.Color(l.ColorIdx);
		if (l.Kind == INT_GC_Line.KIND_LOCAL)
			return INT_GC_Look.Local();
		return INT_GC_Palette.Color(INT_GC_Const.DEFAULT_COLOR);
	}

	// --- hit lookup / feed helpers ----------------------------------------------------------------------------
	INT_GC_Line LineOfHit(Widget w)
	{
		int seq;
		if (!w || !m_HitSeq.Find(w, seq))
			return null;
		return m_BySeq.Get(seq);
	}

	Widget FrameOf(int seq)
	{
		INT_GC_LineView v;
		if (m_Views.Find(seq, v) && v)
			return v.Frame;
		return null;
	}

	int ViewCount()
	{
		return m_Views.Count();
	}
}
