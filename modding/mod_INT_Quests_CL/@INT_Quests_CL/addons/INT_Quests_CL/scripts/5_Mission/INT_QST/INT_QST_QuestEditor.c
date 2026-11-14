// INT Quests (client) — quest editor of the admin board (design B «Полевой дневник»: a paper sheet over the board
// window). The host hands a full-window frame; the editor works on a DEEP COPY of the quest and gives it back through
// Event_Save(quest, branchKey). Three columns: «Основное» (name, branch, order, givers, repeat, time limit,
// requirements, admin note), «Цели» (one card per target, fields by kind), «Описание и награды» (the giver's text,
// items given on accept, rewards). Columns scroll smoothly by pixels; text widgets are not cut by clipchildren in DayZ,
// so a text or input crossing the column edge is hidden (INT_QST_EdColumn). Widgets are built once per Open; a changed part (one target card, the
// requirement rows, the repeat fields) is rebuilt on the next Tick, never inside an event of an input that belongs to
// that part (its handler object would be freed under its own event).

// A block of a column: a frame placed by the column + the input objects that live with it.
class INT_QST_EdBlock : Managed
{
	Widget m_Root;
	float m_H;
	int m_Sec;			// section (rebuilt together); sections keep their order in the column
	int m_Index;			// item inside the section
	ref array<ref Managed> m_Hold = new array<ref Managed>();
	ref array<Widget> m_Cut = new array<Widget>();	// texts/inputs the column hid at its edge (shown again inside)

	void Finish(float w, float h)
	{
		m_H = h;
		INT_UIF_Draw.Resize(m_Root, w, h);
	}
}

// A column of blocks that scrolls smoothly by pixels (wheel, STEP per notch), with a thin scroll mark on the right.
// Boxes and images are cut by the clip frame; texts and inputs are not cut by clipchildren in DayZ, so a text or input
// that crosses the column edge is hidden until it is inside again (owner 2026-10-22: block scrolling jumped and a
// tall card left room for only one target).
class INT_QST_EdColumn : Managed
{
	static const float STEP = 60;

	Widget m_Frame;
	protected Widget m_Clip;
	protected Widget m_Bar;
	protected Widget m_Thumb;
	protected float m_W;
	protected float m_H;
	protected float m_Off;			// scrolled design px
	protected float m_Total;
	protected INT_QST_Clicks m_Clicks;	// the editor's (weak): bindings of a replaced block are dropped
	ref array<ref INT_QST_EdBlock> m_Blocks = new array<ref INT_QST_EdBlock>();

	void Build(Widget parent, float x, float y, float w, float h, INT_QST_Clicks clicks, int bgId)
	{
		m_Clicks = clicks;
		m_W = w;
		m_H = h;
		m_Off = 0;
		m_Frame = INT_UIF_Draw.Frame(parent, x, y, w, h, "Column");
		ButtonWidget bg = INT_UIF_Draw.Hit(m_Frame, 0, 0, w, h, clicks);
		clicks.Bind(bg, bgId);
		m_Clip = INT_UIF_Draw.Clip(m_Frame, 0, 0, w - 8, h, "Clip");
		m_Bar = INT_UIF_Draw.Frame(m_Frame, w - 5, 0, 4, h, "Bar");
		INT_UIF_Draw.Line(m_Bar, 1, 0, 2, h, INT_QST_Look.InkA(30));
		m_Thumb = INT_UIF_Draw.Frame(m_Frame, w - 6, 0, 6, 40, "Thumb");
		INT_UIF_Surface.Box(m_Thumb, 0, 0, 6, 40, 4, INT_QST_Look.InkA(120), 0);
		m_Bar.Show(false);
		m_Thumb.Show(false);
	}

	float BlockW()
	{
		return m_W - 14;
	}

	INT_QST_EdBlock NewBlock(int sec, int index)
	{
		INT_QST_EdBlock b = new INT_QST_EdBlock();
		b.m_Sec = sec;
		b.m_Index = index;
		b.m_H = 10;
		float bw = BlockW();
		Widget root = INT_UIF_Draw.Frame(m_Clip, 0, 0, bw, 10, "Block");
		b.m_Root = root;
		root.Show(false);
		return b;
	}

	protected int InsertPos(int sec)
	{
		int n = m_Blocks.Count();
		for (int i = 0; i < n; i++)
		{
			if (m_Blocks[i].m_Sec > sec)
				return i;
		}
		return n;
	}

	// The old block's widgets go with their click bindings (the editor's map would only grow otherwise). Called from
	// the editor's Tick, never inside an event of the block's own inputs.
	protected void DropBlock(INT_QST_EdBlock old)
	{
		if (!old || !old.m_Root)
			return;
		if (m_Clicks)
			m_Clicks.ForgetUnder(old.m_Root);
		old.m_Root.Unlink();
		old.m_Root = null;
	}

	// Puts `fresh` where the section was (or where it belongs by its id).
	void ReplaceSection(int sec, array<ref INT_QST_EdBlock> fresh)
	{
		int at = -1;
		for (int i = m_Blocks.Count() - 1; i >= 0; i--)
		{
			INT_QST_EdBlock old = m_Blocks[i];
			if (old.m_Sec != sec)
				continue;
			DropBlock(old);
			m_Blocks.RemoveOrdered(i);
			at = i;
		}
		if (at < 0)
			at = InsertPos(sec);
		foreach (INT_QST_EdBlock nb : fresh)
		{
			if (at >= m_Blocks.Count())
				m_Blocks.Insert(nb);
			else
				m_Blocks.InsertAt(nb, at);
			at++;
		}
	}

	void ReplaceBlock(int sec, int index, INT_QST_EdBlock nb)
	{
		int n = m_Blocks.Count();
		for (int i = 0; i < n; i++)
		{
			INT_QST_EdBlock old = m_Blocks[i];
			if (old.m_Sec != sec || old.m_Index != index)
				continue;
			DropBlock(old);
			m_Blocks.Set(i, nb);
			return;
		}
		DropBlock(nb);
	}

	protected float MaxOff()
	{
		float m = m_Total - m_H;
		if (m < 0)
			m = 0;
		return m;
	}

	// Places every block at its place minus the scroll; a block outside the view is hidden, a block on the edge
	// shows its boxes (cut by the clip) and hides the texts/inputs that cross the edge.
	void Arrange()
	{
		int n = m_Blocks.Count();
		float total = 0;
		for (int t = 0; t < n; t++)
			total = total + m_Blocks[t].m_H;
		m_Total = total;
		float maxOff = MaxOff();
		if (m_Off > maxOff)
			m_Off = maxOff;
		if (m_Off < 0)
			m_Off = 0;
		float k = INT_UIF_Scale.Factor();
		float bw = BlockW();
		float y = 0;
		for (int i = 0; i < n; i++)
		{
			INT_QST_EdBlock b = m_Blocks[i];
			float bh = b.m_H;
			float top = y - m_Off;
			y = y + bh;
			if (top + bh <= 0 || top >= m_H)
			{
				b.m_Root.Show(false);
				continue;
			}
			INT_UIF_Draw.Place(b.m_Root, 0, top, bw, bh);
			b.m_Root.Show(true);
			float relTop = -top * k;
			float relBottom = (m_H - top) * k;
			bool whole = top >= 0 && top + bh <= m_H;
			if (!whole || b.m_Cut.Count() > 0)
				CutIn(b, b.m_Root.GetChildren(), 0, relTop, relBottom);
		}
		PlaceThumb(m_Off, total);
	}

	// Hides texts/inputs of a block outside [top, bottom] (block-relative screen px) and shows the ones it hid
	// before that are inside again. Positions of INT_UIF widgets are exact screen px (INT_UIF_Draw.Place).
	protected void CutIn(INT_QST_EdBlock b, Widget w, float oy, float top, float bottom)
	{
		while (w)
		{
			float wx;
			float wy;
			float ww;
			float wh;
			w.GetPos(wx, wy);
			w.GetSize(ww, wh);
			float y1 = oy + wy;
			float y2 = y1 + wh;
			bool inside = y1 >= top - 0.5 && y2 <= bottom + 0.5;
			if (TextWidget.Cast(w) || UIWidget.Cast(w))
			{
				int at = b.m_Cut.Find(w);
				if (!inside && at < 0 && w.IsVisible())
				{
					w.Show(false);
					b.m_Cut.Insert(w);
				}
				else if (inside && at >= 0)
				{
					w.Show(true);
					b.m_Cut.Remove(at);
				}
			}
			else
			{
				Widget kid = w.GetChildren();
				if (kid)
					CutIn(b, kid, y1, top, bottom);
			}
			w = w.GetSibling();
		}
	}

	protected void PlaceThumb(float before, float total)
	{
		bool need = total > m_H + 1;
		m_Bar.Show(need);
		m_Thumb.Show(need);
		if (!need)
			return;
		float th = m_H * m_H / total;
		if (th < 30)
			th = 30;
		float maxS = total - m_H;
		float ty = 0;
		if (maxS > 0)
			ty = (m_H - th) * before / maxS;
		if (ty > m_H - th)
			ty = m_H - th;
		INT_UIF_Draw.Place(m_Thumb, m_W - 6, ty, 6, th);
		Widget box = m_Thumb.GetChildren();
		while (box)
		{
			INT_UIF_Draw.Resize(box, 6, th);
			box = box.GetSibling();
		}
	}

	bool ScrollBy(int wheel)
	{
		float was = m_Off;
		float off = m_Off;
		if (wheel > 0)
			off = off - STEP;
		else if (wheel < 0)
			off = off + STEP;
		float maxOff = MaxOff();
		if (off > maxOff)
			off = maxOff;
		if (off < 0)
			off = 0;
		if (Math.AbsFloat(off - was) < 0.5)
			return false;
		m_Off = off;
		Arrange();
		return true;
	}

	// Scrolls so that the block is on screen (its top when it is taller than the column).
	void ShowBlock(int sec, int index)
	{
		float y = 0;
		int n = m_Blocks.Count();
		for (int i = 0; i < n; i++)
		{
			INT_QST_EdBlock b = m_Blocks[i];
			if (b.m_Sec != sec || b.m_Index != index)
			{
				y = y + b.m_H;
				continue;
			}
			float bottom = y + b.m_H;
			if (y < m_Off || b.m_H > m_H)
				m_Off = y;
			else if (bottom > m_Off + m_H)
				m_Off = bottom - m_H;
			Arrange();
			return;
		}
	}

	bool ContainsMouse(int mx, int my)
	{
		if (!m_Frame)
			return false;
		float sx;
		float sy;
		float sw;
		float sh;
		m_Frame.GetScreenPos(sx, sy);
		m_Frame.GetScreenSize(sw, sh);
		return mx >= sx && mx < sx + sw && my >= sy && my < sy + sh;
	}

	bool HasFocusInside()
	{
		Widget fw = GetFocus();
		if (!fw || !m_Frame)
			return false;
		return INT_UIF_Widgets.IsDescendant(fw, m_Frame);
	}
}

// Choice chips split into as many INT_QST_Choice rows as the width needs (one selection over all rows).
class INT_QST_EdChips : Managed
{
	protected ref array<ref INT_QST_Choice> m_Rows = new array<ref INT_QST_Choice>();
	protected ref array<int> m_Start = new array<int>();
	protected int m_Sel;
	float m_H;
	int m_Tag;
	ref ScriptInvoker Event_Changed = new ScriptInvoker();	// (INT_QST_EdChips c, int index)

	// Same chip measure as INT_QST_Choice.Redraw (13 px text, 22 px padding, 6 px gap).
	static float ChipW(string label)
	{
		string t = INT_QST_Look.Tr(label);
		return INT_UIF_Draw.TextWidth(t, 13) * 1.12 + 22;
	}

	static INT_QST_EdChips Create(Widget parent, float x, float y, float w, string labelKey, array<string> labels, int selected)
	{
		INT_QST_EdChips c = new INT_QST_EdChips();
		c.Build(parent, x, y, w, labelKey, labels, selected);
		return c;
	}

	protected void Build(Widget parent, float x, float y, float w, string labelKey, array<string> labels, int selected)
	{
		m_Sel = selected;
		float cy = y;
		int n = labels.Count();
		int i = 0;
		int row = 0;
		string cap = labelKey;
		while (i < n)
		{
			array<string> part = new array<string>();
			float cx = 0;
			int start = i;
			while (i < n)
			{
				string lab = labels[i];
				float cw = INT_QST_EdChips.ChipW(lab);
				if (cx > 0 && cx + cw > w)
					break;
				part.Insert(lab);
				cx = cx + cw + 6;
				i++;
			}
			int sel = -1;
			if (m_Sel >= start && m_Sel < i)
				sel = m_Sel - start;
			INT_QST_Choice ch = INT_QST_Choice.Create(parent, x, cy, w, cap, part, sel);
			ch.m_Tag = row;
			ch.Event_Changed.Insert(OnRow);
			m_Rows.Insert(ch);
			m_Start.Insert(start);
			cy = cy + INT_QST_Choice.Height(cap) + 4;
			cap = "";
			row++;
		}
		m_H = cy - y - 4;
		if (m_H < 0)
			m_H = 0;
	}

	int GetIndex()
	{
		return m_Sel;
	}

	void OnRow(INT_QST_Choice c, int index)
	{
		int row = c.m_Tag;
		if (row < 0 || row >= m_Start.Count())
			return;
		int gi = m_Start[row] + index;
		int n = m_Rows.Count();
		for (int r = 0; r < n; r++)
		{
			INT_QST_Choice other = m_Rows[r];
			if (r != row && other.GetIndex() >= 0)
				other.SetIndex(-1);
		}
		m_Sel = gi;
		Event_Changed.Invoke(this, gi);
	}
}

// A class field: the text box, a search (or list) button and a line saying what the class is on this client.
class INT_QST_EdClassBox : Managed
{
	ref INT_QST_Field m_Field;
	TextWidget m_Info;
	float m_W;
	bool m_Action;			// the value is an action class (ACTION targets)
	INT_QST_QuestEditor m_Owner;

	void OnChanged(INT_QST_Field f)
	{
		Describe();
		if (m_Owner)
			m_Owner.OnFieldChanged(f);
	}

	void Describe()
	{
		if (!m_Info || !m_Field)
			return;
		string v = m_Field.GetText();
		v.TrimInPlace();
		string t = "";
		int c = INT_QST_Look.Ink3();
		if (v == "")
		{
			t = INT_QST_Look.Tr("#STR_INT_QST_ED_NOT_SET");
		}
		else if (m_Action)
		{
			typename tn = v.ToType();
			if (tn && tn.IsInherited(ActionBase))
			{
				t = INT_QST_Text.ActionName(v);
			}
			else
			{
				t = INT_QST_Look.Tr("#STR_INT_QST_ED_NO_ACTION");
				c = INT_QST_Look.RedA(210);
			}
		}
		else if (v == "SurvivorBase")
		{
			t = INT_QST_Look.Tr("#STR_INT_QST_PK_PLAYER");
		}
		else if (INT_QST_Util.ClassExists(v))
		{
			t = INT_QST_Text.Name(v);
		}
		else
		{
			t = INT_QST_Look.Tr("#STR_INT_QST_ED_NO_CLASS");
			c = INT_QST_Look.RedA(210);
		}
		string fit = INT_QST_Look.Fit(t, m_W, 12);
		m_Info.SetText(fit);
		m_Info.SetColor(c);
	}
}

// --- small list chooser (branches, givers, quests for the requirements, actions) ---------------------------------------
class INT_QST_EdPopRow
{
	string m_Title;
	string m_Sub;
	string m_Value;
	string m_Find;			// folded title + sub (search)
}

class INT_QST_EdPopSlot extends INT_QST_VSlot
{
	Widget m_Mark;
	TextWidget m_TitleB;
	TextWidget m_Title;
	TextWidget m_Sub;
}

class INT_QST_EdPopSource extends INT_QST_VSource
{
	static const float ROW_H = 48;

	ref array<ref INT_QST_EdPopRow> m_All = new array<ref INT_QST_EdPopRow>();
	ref array<int> m_Rows = new array<int>();
	string m_Current;
	INT_QST_EdPopup m_Owner;

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
		return new INT_QST_EdPopSlot();
	}

	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_EdPopSlot s = INT_QST_EdPopSlot.Cast(slot);
		float bw = w - 4;
		float bh = ROW_H - 4;
		Widget hover = INT_UIF_Draw.Frame(s.m_Root, 4, 2, bw, bh, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, bw, bh, 0x1F2A2218);
		s.m_Hover = hover;
		Widget mark = INT_UIF_Draw.Frame(s.m_Root, 4, 2, bw, bh, "Mark");
		INT_QST_Look.Tex(mark, 0, 0, bw, bh, "int_qst_highlight_ca", INT_QST_Look.Marker());
		INT_UIF_Draw.Line(mark, 0, 0, 3, bh, INT_QST_Look.Ink());
		mark.Show(false);
		s.m_Mark = mark;
		int ink = INT_QST_Look.Ink();
		TextWidget twin = INT_QST_Look.Text(s.m_Root, 16.7, 3, w - 30, 24, "", 15, ink);
		TextWidget tt = INT_QST_Look.Text(s.m_Root, 16, 3, w - 30, 24, "", 15, ink);
		TextWidget st = INT_QST_Look.Text(s.m_Root, 16, 26, w - 30, 18, "", 12, INT_QST_Look.Ink3());
		s.m_TitleB = twin;
		s.m_Title = tt;
		s.m_Sub = st;
		INT_QST_Look.Hair(s.m_Root, 10, ROW_H - 1, w - 16);
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_EdPopSlot s = INT_QST_EdPopSlot.Cast(slot);
		if (!s || !s.m_Title)
			return;
		INT_QST_EdPopRow r = m_All[m_Rows[i]];
		string title = INT_QST_Look.Fit(r.m_Title, w - 30, 15);
		string sub = INT_QST_Look.Fit(r.m_Sub, w - 30, 12);
		float ty = 3;
		if (sub == "")
			ty = 12;
		INT_UIF_Draw.Place(s.m_TitleB, 16.7, ty, w - 30, 24);
		INT_UIF_Draw.Place(s.m_Title, 16, ty, w - 30, 24);
		s.m_TitleB.SetText(title);
		s.m_Title.SetText(title);
		s.m_Sub.SetText(sub);
		bool cur = r.m_Value == m_Current;
		s.m_Mark.Show(cur);
	}

	override void OnRowClick(int i, int button)
	{
		if (button != MouseState.LEFT || i < 0 || i >= m_Rows.Count())
			return;
		if (m_Owner)
			m_Owner.Choose(m_Rows[i]);
	}
}

class INT_QST_EdPopup : Managed
{
	static const int ID_CLOSE = 1;
	static const int ID_BG = 2;
	static const float LIST_TOP = 146;

	ref ScriptInvoker Event_Pick = new ScriptInvoker();	// (string value)

	protected ref INT_QST_Clicks m_Clicks;
	protected ref INT_QST_VList m_List;
	protected ref INT_QST_EdPopSource m_Src;
	protected ref INT_QST_Field m_Search;
	// the popup closes from its own row click: the list, source and search box of the last open live one more
	// open, the widgets are hidden and unlinked on the next Tick (never free / unlink inside its own event)
	protected ref INT_QST_VList m_OldList;
	protected ref INT_QST_EdPopSource m_OldSrc;
	protected ref INT_QST_Field m_OldSearch;
	protected ref array<Widget> m_Dead = new array<Widget>();
	protected ref array<string> m_Tokens = new array<string>();
	protected Widget m_Root;
	protected float m_FilterIn;
	protected bool m_SubmitPending;

	void INT_QST_EdPopup()
	{
		m_Clicks = new INT_QST_Clicks();
		m_Clicks.Event_Click.Insert(OnClickId);
	}

	bool IsOpen()
	{
		return m_Root != null;
	}

	void Open(Widget layer, float w, float h, string titleKey, array<ref INT_QST_EdPopRow> rows, string current)
	{
		if (IsOpen())
			Close();
		if (!layer)
			return;
		m_Clicks.Clear();
		m_FilterIn = 0;
		m_SubmitPending = false;
		m_OldList = m_List;
		m_OldSrc = m_Src;
		m_OldSearch = m_Search;
		m_Src = new INT_QST_EdPopSource();
		m_Src.m_Owner = this;
		m_Src.m_Current = current;
		foreach (INT_QST_EdPopRow r : rows)
			m_Src.m_All.Insert(r);
		float cw = 660;
		if (cw > w - 80)
			cw = w - 80;
		float ch = h - 200;
		if (ch > 720)
			ch = 720;
		if (ch < 380)
			ch = h - 60;
		float cx = (w - cw) * 0.5;
		float cy = (h - ch) * 0.5;
		m_Root = INT_UIF_Draw.Frame(layer, 0, 0, w, h, "EdPopup");
		INT_UIF_Surface.Box(m_Root, 4, 4, w - 8, h - 8, 14, 0x9E0D0905, 0);
		ButtonWidget bg = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, m_Clicks);
		m_Clicks.Bind(bg, ID_BG);
		Widget card = INT_UIF_Draw.Frame(m_Root, cx, cy, cw, ch, "Card");
		INT_UIF_Surface.Shadow(card, -30, -20, cw + 60, ch + 60, 0x8C000000);
		INT_QST_Look.PaperSheet(card, 0, 0, cw, ch, INT_QST_Look.Note(), "Paper");
		INT_QST_Look.PinHead(card, cw * 0.5 - 13, -12);
		ButtonWidget cardHit = INT_UIF_Draw.Hit(card, 0, 0, cw, ch, m_Clicks);
		m_Clicks.Bind(cardHit, ID_BG);
		int ink = INT_QST_Look.Ink();
		INT_QST_Look.Icon(card, 36, 30, 22, "list", ink);
		string title = INT_QST_Look.Tr(titleKey);
		INT_QST_Look.Heavy(card, 68, 24, cw - 200, 34, title, 22, ink);
		BuildClose(card, cw - 36 - 36, 22);
		INT_QST_Look.DoubleRule(card, 36, 66, cw - 72);
		INT_QST_Field f = INT_QST_Field.Create(card, 36, 80, cw - 72, "#STR_INT_QST_ED_POP_SEARCH", "", 60);
		m_Search = f;
		f.Event_Changed.Insert(OnSearchChanged);
		f.Event_Submit.Insert(OnSearchSubmit);
		m_List = new INT_QST_VList();
		int bar = INT_QST_Look.InkA(30);
		int thumb = INT_QST_Look.InkA(120);
		m_List.Build(card, 24, LIST_TOP, cw - 48, ch - LIST_TOP - 46, m_Src, bar, thumb);
		int ink3 = INT_QST_Look.Ink3();
		INT_QST_Look.Text(card, cw * 0.4, ch - 38, cw * 0.6 - 36, 22, "DayZavr Quest System", 12, ink3, INT_UIF_Align.RIGHT);
		Refilter();
		ScrollToCurrent();
		f.Focus();
		INT_QST_Sfx.Play("page");
	}

	// Hides now, unlinks on the next Tick (Close runs inside a row click of the list).
	void Close()
	{
		if (!m_Root)
			return;
		Widget fw = GetFocus();
		if (fw && INT_UIF_Widgets.IsDescendant(fw, m_Root))
			SetFocus(null);
		m_Root.Show(false);
		m_Dead.Insert(m_Root);
		m_Root = null;
		m_Clicks.Clear();
	}

	// Called by the editor every frame, also while it is closed (before it unlinks its own root, the parent).
	void KillDead()
	{
		if (m_Dead.Count() == 0)
			return;
		foreach (Widget dw : m_Dead)
		{
			if (dw)
				dw.Unlink();
		}
		m_Dead.Clear();
	}

	void Tick(float dt)
	{
		if (!IsOpen())
			return;
		if (m_SubmitPending)
		{
			m_SubmitPending = false;
			if (m_FilterIn > 0)
				Refilter();
			if (m_Src && m_Src.m_Rows.Count() > 0)
				Choose(m_Src.m_Rows[0]);
			return;
		}
		if (m_FilterIn > 0)
		{
			m_FilterIn = m_FilterIn - dt;
			if (m_FilterIn <= 0)
				Refilter();
		}
	}

	protected void BuildClose(Widget card, float x, float y)
	{
		Widget f = INT_UIF_Draw.Frame(card, x, y, 36, 36, "Close");
		int fill = 0x26FFFFFF;
		int edge = INT_QST_Look.InkA(90);
		INT_UIF_Surface.Box(f, 0, 0, 36, 36, 6, fill, edge);
		Widget hover = INT_UIF_Draw.Frame(f, 0, 0, 36, 36, "Hover");
		int hf = INT_QST_Look.RedA(40);
		int hb = INT_QST_Look.RedA(200);
		INT_UIF_Surface.Box(hover, 0, 0, 36, 36, 6, hf, hb);
		INT_QST_Look.Icon(f, 10, 10, 16, "close", INT_QST_Look.Ink());
		ButtonWidget hit = INT_UIF_Draw.Hit(f, 0, 0, 36, 36, m_Clicks);
		m_Clicks.Bind(hit, ID_CLOSE, 0, hover);
	}

	protected void Refilter()
	{
		m_FilterIn = 0;
		if (!m_Src || !m_List || !m_Search)
			return;
		string raw = m_Search.GetText();
		raw.TrimInPlace();
		string q = INT_QST_PickIndex.Fold(raw);
		m_Tokens.Clear();
		q.Split(" ", m_Tokens);
		int nt = m_Tokens.Count();
		m_Src.m_Rows.Clear();
		int n = m_Src.m_All.Count();
		for (int k = 0; k < n; k++)
		{
			INT_QST_EdPopRow r = m_Src.m_All[k];
			bool ok = true;
			for (int ti = 0; ti < nt; ti++)
			{
				if (r.m_Find.IndexOf(m_Tokens[ti]) < 0)
				{
					ok = false;
					break;
				}
			}
			if (ok)
				m_Src.m_Rows.Insert(k);
		}
		m_List.Refresh(true);
	}

	protected void ScrollToCurrent()
	{
		if (!m_Src || !m_List)
			return;
		int n = m_Src.m_Rows.Count();
		for (int i = 0; i < n; i++)
		{
			INT_QST_EdPopRow r = m_Src.m_All[m_Src.m_Rows[i]];
			if (r.m_Value == m_Src.m_Current)
			{
				m_List.ScrollToRow(i);
				return;
			}
		}
	}

	void OnSearchChanged(INT_QST_Field f)
	{
		m_FilterIn = 0.12;
	}

	// Enter: the first row, on the next Tick (closing here would destroy the edit box inside its own event).
	void OnSearchSubmit(INT_QST_Field f)
	{
		m_SubmitPending = true;
	}

	void OnClickId(int id, int data, int button)
	{
		if (button != MouseState.LEFT)
			return;
		if (id == ID_CLOSE)
		{
			Close();
			INT_QST_Sfx.Play("close");
		}
	}

	void Choose(int k)
	{
		if (!m_Src || k < 0 || k >= m_Src.m_All.Count())
			return;
		INT_QST_EdPopRow r = m_Src.m_All[k];
		string v = r.m_Value + "";
		Close();
		INT_QST_Sfx.Play("click");
		Event_Pick.Invoke(v);
	}
}

// --- the editor ----------------------------------------------------------------------------------------------------------
class INT_QST_QuestEditor : Managed
{
	// sections: column order = id order (a section rebuilt later keeps its place)
	static const int SEC_L_MAIN = 10;
	static const int SEC_L_REPARGS = 20;
	static const int SEC_L_LIMIT = 30;
	static const int SEC_L_REQHEAD = 40;
	static const int SEC_L_REQS = 50;
	static const int SEC_L_REQADD = 60;
	static const int SEC_L_NOTE = 70;
	static const int SEC_M_HEAD = 110;
	static const int SEC_M_TGTS = 120;
	static const int SEC_R_QIHEAD = 210;
	static const int SEC_R_QIS = 220;
	static const int SEC_R_RWHEAD = 230;
	static const int SEC_R_RWS = 240;

	// clicks (data in brackets)
	static const int ID_BG = 1;
	static const int ID_CLOSE = 2;
	static const int ID_SAVE = 3;
	static const int ID_CANCEL = 4;
	static const int ID_DELETE = 5;
	static const int ID_BRANCH_LIST = 6;
	static const int ID_GIVER = 7;		// (0 = gives, 1 = takes)
	static const int ID_REQ_ADD = 8;
	static const int ID_REQ_DEL = 9;	// (row)
	static const int ID_TGT_ADD = 10;
	static const int ID_TGT_DEL = 11;	// (target)
	static const int ID_TGT_UP = 12;
	static const int ID_TGT_DOWN = 13;
	static const int ID_PICK = 14;		// (input tag)
	static const int ID_ACTIONS = 15;	// (target)
	static const int ID_HERE = 16;		// (input tag)
	static const int ID_QI_ADD = 17;
	static const int ID_QI_DEL = 18;	// (item)
	static const int ID_RW_ADD = 19;	// (INT_QST_RT)
	static const int ID_RW_DEL = 20;	// (reward)
	static const int ID_ASK_YES = 21;
	static const int ID_ASK_NO = 22;

	// input tags: scope * 10000 + index * 100 + prop (indexes stay below 100: MAX_TARGETS 40, MAX_REQUIRES 60)
	static const int SC_MAIN = 1;
	static const int SC_TGT = 2;
	static const int SC_QI = 3;
	static const int SC_RW = 4;

	static const int P_NAME = 1;
	static const int P_BRANCH = 2;
	static const int P_ORDER = 3;
	static const int P_CD_H = 4;
	static const int P_CD_M = 5;
	static const int P_RS_H = 6;
	static const int P_RS_M = 7;
	static const int P_TL_H = 8;
	static const int P_TL_M = 9;
	static const int P_NOTE = 10;
	static const int P_REPEAT = 11;
	static const int P_WEEKDAY = 12;
	static const int P_REQANY = 13;
	static const int P_ENABLED = 14;

	static const int T_KIND = 1;
	static const int T_TARGET = 2;
	static const int T_COUNT = 3;
	static const int T_FILTER = 4;
	static const int T_WORST = 5;
	static const int T_MINQ = 6;
	static const int T_POS = 7;
	static const int T_RADIUS = 8;
	static const int T_GROUND = 9;
	static const int T_MAP = 10;
	static const int T_ZONEC = 11;
	static const int T_ZONER = 12;
	static const int T_TEXT = 13;

	static const int Q_ITEM = 1;
	static const int Q_COUNT = 2;

	static const int R_KIND = 1;
	static const int R_ITEM = 2;
	static const int R_COUNT = 3;
	static const int R_QTY = 4;
	static const int R_STATE = 5;
	static const int R_AMOUNT = 6;
	static const int R_TEXT = 7;

	static const int POP_BRANCH = 1;
	static const int POP_GSTART = 2;
	static const int POP_GFINISH = 3;
	static const int POP_REQ = 4;
	static const int POP_ACTION = 5;

	static const int ASK_CLOSE = 1;
	static const int ASK_DELETE = 2;

	static const int MAX_KEY = 40;		// branch key characters

	ref ScriptInvoker Event_Save = new ScriptInvoker();	// (INT_QST_QuestDef edited, string branchKey)
	ref ScriptInvoker Event_Delete = new ScriptInvoker();	// (int questId), after its own confirm
	ref ScriptInvoker Event_Closed = new ScriptInvoker();	// ()

	protected ref INT_QST_Clicks m_Clicks;
	protected ref INT_QST_ClassPicker m_Picker;
	protected ref INT_QST_EdPopup m_Popup;
	protected ref INT_QST_QuestDef m_Q;
	protected ref INT_QST_EdColumn m_Left;
	protected ref INT_QST_EdColumn m_Mid;
	protected ref INT_QST_EdColumn m_Right;
	protected ref INT_QST_Area m_Desc;
	protected ref array<int> m_RebuildSecs = new array<int>();
	protected ref array<int> m_RebuildBlocks = new array<int>();	// sec * 1000 + index
	protected ref array<int> m_TmpSecs = new array<int>();
	protected ref array<int> m_TmpBlocks = new array<int>();
	protected Widget m_Layer;
	protected Widget m_Root;
	protected Widget m_Ask;
	protected Widget m_Flash;
	protected Widget m_DirtyMark;
	protected TextWidget m_DescBytes;
	protected TextWidget m_BranchInfo;
	protected TextWidget m_GStartText;
	protected TextWidget m_GFinishText;
	protected TextWidget m_TgtCount;
	protected INT_QST_Field m_BranchField;		// held by its block
	protected float m_W;
	protected float m_H;
	protected float m_GiverW;
	protected float m_FlashW;
	protected float m_FlashLeft;
	protected string m_BranchKey;
	protected string m_DescText;
	protected bool m_IsNew;
	protected bool m_Dirty;
	protected int m_Repeat;
	protected int m_AskTag;
	protected int m_PickTag;		// input filled by the class picker
	protected int m_PopMode;
	protected int m_PopIndex;
	protected int m_ScrollSec;		// show this block after the next rebuild (0 = none)
	protected int m_ScrollIdx;
	protected int m_BaseRec;		// record hash of the quest when it was opened (BaseRec of the save)
	protected ref array<Widget> m_Dead = new array<Widget>();	// closed sheet / question: unlinked next Tick

	// What the editor opened, for "save without changes changes nothing": a field the admin did not touch is
	// written back exactly as it came (the save normalises only what was edited). m_*From: per row of m_Q, the
	// row of m_Orig it came from (-1 = added here); kept in step with add / remove / move.
	protected ref INT_QST_QuestDef m_Orig;
	protected ref array<int> m_TgtFrom = new array<int>();
	protected ref array<int> m_RwFrom = new array<int>();
	protected ref array<int> m_QiFrom = new array<int>();
	protected string m_OrigDesc;
	protected int m_OrigRepeat;

	void INT_QST_QuestEditor()
	{
		m_Clicks = new INT_QST_Clicks();
		m_Clicks.Event_Click.Insert(OnClickId);
		m_Clicks.Event_Wheel.Insert(OnWheelId);
		m_Picker = new INT_QST_ClassPicker();
		m_Picker.Event_Pick.Insert(OnPicked);
		m_Popup = new INT_QST_EdPopup();
		m_Popup.Event_Pick.Insert(OnPopupPick);
	}

	// --- public ------------------------------------------------------------------------------------------------------------
	// Edits a deep copy of `source`; a new quest carries the host's negative temporary Id.
	void Open(Widget layer, float w, float h, INT_QST_QuestDef source, string branchKey, bool isNew)
	{
		if (IsOpen())
			Shut(false);
		if (!layer || !source)
			return;
		m_Layer = layer;
		m_W = w;
		m_H = h;
		m_IsNew = isNew;
		m_Dirty = false;
		INT_QST_QuestDef copy = INT_QST_QuestEditor.CopyQuest(source);
		m_Q = copy;
		INT_QST_QuestDef pristine = INT_QST_QuestEditor.CopyQuest(source);
		m_Orig = pristine;
		m_TgtFrom.Clear();
		for (int ti = 0; ti < m_Q.Targets.Count(); ti++)
			m_TgtFrom.Insert(ti);
		m_RwFrom.Clear();
		for (int ri = 0; ri < m_Q.Rewards.Count(); ri++)
			m_RwFrom.Insert(ri);
		m_QiFrom.Clear();
		for (int qi = 0; qi < m_Q.QuestItems.Count(); qi++)
			m_QiFrom.Insert(qi);
		// the base of the save: the record the board has now (another admin's later change is refused)
		m_BaseRec = 0;
		if (!isNew && source.Id > 0)
			m_BaseRec = INT_QST_Client.QuestRec(source.Id);
		string key = branchKey + "";
		key.TrimInPlace();
		if (key == "")
			key = source.m_Branch;
		m_BranchKey = key;
		m_Repeat = INT_QST_RM.FromString(m_Q.RepeatMode);
		m_OrigRepeat = m_Repeat;
		m_DescText = "";
		foreach (string piece : m_Q.Description)
			m_DescText = m_DescText + piece;
		m_OrigDesc = m_DescText;
		m_RebuildSecs.Clear();
		m_RebuildBlocks.Clear();
		m_ScrollSec = 0;
		m_FlashLeft = 0;
		m_Clicks.Clear();
		BuildUi();
		INT_QST_Sfx.Play("page");
	}

	bool IsOpen()
	{
		return m_Root != null;
	}

	void Close()
	{
		Shut(true);
	}

	// Record hash the open quest had when the editor took it (the host sends it as BaseRec; 0 = new / unknown).
	int BaseRec()
	{
		return m_BaseRec;
	}

	// The host opened the editor again with the copy the server refused: the edit counts as unsaved, the reason
	// is on the bottom bar. conflict: another admin changed the quest; the next save is based on their version.
	void AfterFailedSave(string reason, int baseRec, bool conflict)
	{
		if (!IsOpen())
			return;
		m_BaseRec = baseRec;
		MarkDirty();
		string t = INT_QST_Look.Fmt("#STR_INT_QST_FA_ED_NOT_SAVED", reason);
		if (conflict)
			t = INT_QST_Look.Tr("#STR_INT_QST_FA_ED_CONFLICT");
		Flash(t, true);
	}

	// Esc: the class picker / list chooser / question first, then "unsaved changes?" or close. True when handled.
	bool OnEscape()
	{
		if (!IsOpen())
			return false;
		if (m_Picker && m_Picker.OnEscape())
			return true;
		if (m_Popup && m_Popup.IsOpen())
		{
			m_Popup.Close();
			INT_QST_Sfx.Play("close");
			return true;
		}
		if (m_Ask)
		{
			CloseAsk();
			return true;
		}
		RequestClose();
		return true;
	}

	void Tick(float dt)
	{
		if (m_Picker)
			m_Picker.Tick(dt);
		// closed parts go now, children first (the popup lives inside the sheet)
		if (m_Popup)
			m_Popup.KillDead();
		KillDead();
		if (!IsOpen())
			return;
		if (m_Popup)
			m_Popup.Tick(dt);
		if (m_RebuildSecs.Count() > 0 || m_RebuildBlocks.Count() > 0)
			ProcessRebuilds();
		if (m_FlashLeft > 0)
		{
			m_FlashLeft = m_FlashLeft - dt;
			if (m_FlashLeft <= 0 && m_Flash)
				INT_UIF_Animator.FadeOut(m_Flash, 0.3);
		}
	}

	// An edit box has the focus: the host must not treat keys as its shortcuts.
	bool IsTyping()
	{
		if (!IsOpen())
			return false;
		Widget fw = GetFocus();
		if (!fw)
			return false;
		if (EditBoxWidget.Cast(fw))
			return true;
		if (MultilineEditBoxWidget.Cast(fw))
			return true;
		return false;
	}

	// The widgets are hidden now and unlinked on the next Tick, the input objects stay until the next Open (Close
	// runs inside a click of the sheet's own buttons: «Сохранить», «Закрыть», the question card).
	protected void Shut(bool notify)
	{
		if (!m_Root)
			return;
		if (m_Picker)
			m_Picker.Close();
		if (m_Popup)
			m_Popup.Close();
		Widget fw = GetFocus();
		if (fw && INT_UIF_Widgets.IsDescendant(fw, m_Root))
			SetFocus(null);
		m_Root.Show(false);
		m_Dead.Insert(m_Root);
		m_Root = null;
		m_Ask = null;
		m_Flash = null;
		m_DirtyMark = null;
		m_DescBytes = null;
		m_BranchInfo = null;
		m_GStartText = null;
		m_GFinishText = null;
		m_TgtCount = null;
		m_BranchField = null;
		m_Clicks.Clear();
		m_RebuildSecs.Clear();
		m_RebuildBlocks.Clear();
		if (notify)
			Event_Closed.Invoke();
	}

	// --- data ----------------------------------------------------------------------------------------------------------------
	// Deep copy: a JSON round trip (every serialized field, future ones too), then the strings are copied directly —
	// the engine JSON reader keeps only 1023 bytes of one string (measured), a long admin note would come back cut.
	static INT_QST_QuestDef CopyQuest(INT_QST_QuestDef src)
	{
		INT_QST_QuestDef c = new INT_QST_QuestDef();
		JsonSerializer js = new JsonSerializer();
		string txt;
		string err;
		js.WriteToString(src, false, txt);
		js.ReadFromString(c, txt, err);
		c.m_Branch = src.m_Branch;
		c.Name = src.Name + "";
		c.Note = src.Note + "";
		c.GiverStart = src.GiverStart + "";
		c.GiverFinish = src.GiverFinish + "";
		c.RepeatMode = src.RepeatMode + "";
		c.Description = new array<string>();
		foreach (string piece : src.Description)
			c.Description.Insert(piece);
		if (!c.Requires)
			c.Requires = new array<int>();
		if (!c.Targets)
			c.Targets = new array<ref INT_QST_TargetDef>();
		if (!c.Rewards)
			c.Rewards = new array<ref INT_QST_RewardDef>();
		if (!c.QuestItems)
			c.QuestItems = new array<ref INT_QST_ItemRef>();
		int nt = src.Targets.Count();
		if (c.Targets.Count() == nt)
		{
			for (int i = 0; i < nt; i++)
			{
				INT_QST_TargetDef st = src.Targets[i];
				INT_QST_TargetDef ct = c.Targets[i];
				if (!st || !ct)
					continue;
				ct.Kind = st.Kind + "";
				ct.Target = st.Target + "";
				ct.Filter = st.Filter + "";
				ct.WorstState = st.WorstState + "";
				ct.Position = st.Position + "";
				ct.ZoneCenter = st.ZoneCenter + "";
				ct.Text = st.Text + "";
			}
		}
		int nr = src.Rewards.Count();
		if (c.Rewards.Count() == nr)
		{
			for (int r = 0; r < nr; r++)
			{
				INT_QST_RewardDef sr = src.Rewards[r];
				INT_QST_RewardDef cr = c.Rewards[r];
				if (!sr || !cr)
					continue;
				cr.Kind = sr.Kind + "";
				cr.Item = sr.Item + "";
				cr.Quantity = sr.Quantity + "";
				cr.State = sr.State + "";
				cr.Text = sr.Text + "";
			}
		}
		int ni = src.QuestItems.Count();
		if (c.QuestItems.Count() == ni)
		{
			for (int q = 0; q < ni; q++)
			{
				INT_QST_ItemRef si = src.QuestItems[q];
				INT_QST_ItemRef ci = c.QuestItems[q];
				if (si && ci)
					ci.Item = si.Item + "";
			}
		}
		return c;
	}

	static int Tag(int scope, int index, int prop)
	{
		return scope * 10000 + index * 100 + prop;
	}

	// Latin letters, digits and '_' (folder names must travel between servers and systems).
	static string SanitizeKey(string s)
	{
		string r = "";
		int n = s.Length();
		for (int i = 0; i < n; i++)
		{
			string ch = s.Get(i);
			int a = ch.ToAscii();
			bool ok = false;
			if (a >= 48 && a <= 57)
				ok = true;
			else if (a >= 65 && a <= 90)
				ok = true;
			else if (a >= 97 && a <= 122)
				ok = true;
			else if (a == 95)
				ok = true;
			if (ok)
				r = r + ch;
			if (r.Length() >= MAX_KEY)
				break;
		}
		return r;
	}

	static bool ValidKey(string s)
	{
		if (s == "")
			return false;
		string clean = INT_QST_QuestEditor.SanitizeKey(s);
		return clean == s;
	}

	// seconds with the hours or the minutes part replaced (seconds kept)
	static int WithHM(int secs, bool hours, int value)
	{
		int s = secs;
		if (s < 0)
			s = 0;
		int hh = s / 3600;
		int mm = (s % 3600) / 60;
		int ss = s % 60;
		int v = value;
		if (v < 0)
			v = 0;
		if (hours)
		{
			hh = v;
		}
		else
		{
			if (v > 59)
				v = 59;
			mm = v;
		}
		return hh * 3600 + mm * 60 + ss;
	}

	static string CountText(float c)
	{
		int n = (int)Math.Round(c);
		if (n < 1)
			n = 1;
		return n.ToString();
	}

	static string NumText(float f)
	{
		if (f == 0)
			return "0";
		return INT_QST_Util.FloatText(f);
	}

	// Vanilla continuous actions (ActionContinuousBase descendants: the server counts actions in
	// ActionContinuousBase.OnFinishProgress), checked in P:\scripts 1.30.164200.
	static void CommonActions(array<string> list)
	{
		list.Insert("ActionDigWorms");
		list.Insert("ActionMineRock");
		list.Insert("ActionMineTree");
		list.Insert("ActionMineBush");
		list.Insert("ActionMineTreeBark");
		list.Insert("ActionSkinning");
		list.Insert("ActionSawPlanks");
		list.Insert("ActionBuildPart");
		list.Insert("ActionDismantlePart");
		list.Insert("ActionRepairCarPart");
		list.Insert("ActionDigInStash");
		list.Insert("ActionWaterPlant");
		list.Insert("ActionBandageTarget");
		list.Insert("ActionCPR");
		list.Insert("ActionUnlockDoors");
		list.Insert("ActionCookOnStick");
		list.Insert("ActionFillFuel");
		list.Insert("ActionCraftBolts");
		list.Insert("ActionBuildStoneCircle");
		list.Insert("ActionBuildOven");
	}

	// --- building ------------------------------------------------------------------------------------------------------------
	protected void BuildUi()
	{
		float w = m_W;
		float h = m_H;
		m_Root = INT_UIF_Draw.Frame(m_Layer, 0, 0, w, h, "QuestEditor");
		INT_UIF_Surface.Box(m_Root, 4, 4, w - 8, h - 8, 14, 0xB30D0905, 0);
		ButtonWidget bg = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, m_Clicks);
		m_Clicks.Bind(bg, ID_BG);
		float pw = w - 40;
		float ph = h - 36;
		Widget paper = INT_UIF_Draw.Frame(m_Root, 20, 18, pw, ph, "Sheet");
		INT_UIF_Surface.Shadow(paper, -30, -20, pw + 60, ph + 60, 0x8C000000);
		INT_QST_Look.PaperSheet(paper, 0, 0, pw, ph, INT_QST_Look.Paper(), "Paper");
		ButtonWidget pbg = INT_UIF_Draw.Hit(paper, 0, 0, pw, ph, m_Clicks);
		m_Clicks.Bind(pbg, ID_BG);
		INT_QST_Look.PinHead(paper, 70, -12);
		INT_QST_Look.PinHead(paper, pw - 96, -12);
		BuildHeader(paper, pw);

		float top = 84;
		float colTop = top + 34;
		float colH = ph - 84 - colTop;
		float gutter = 30;
		float avail = pw - 56 - gutter * 2;
		float lw = avail * 0.30;
		float mw = avail * 0.37;
		float rw = avail - lw - mw;
		float lx = 28;
		float mx = lx + lw + gutter;
		float rx = mx + mw + gutter;
		int sep = INT_QST_Look.InkA(45);
		INT_UIF_Draw.Line(paper, mx - gutter * 0.5, top, 1, colH + 34, sep);
		INT_UIF_Draw.Line(paper, rx - gutter * 0.5, top, 1, colH + 34, sep);
		int ink = INT_QST_Look.Ink();
		INT_QST_Look.Caption(paper, lx, top, lw, "#STR_INT_QST_ED_COL_MAIN", ink, "book");
		INT_QST_Look.Caption(paper, mx, top, mw, "#STR_INT_QST_ED_COL_TARGETS", ink, "crosshair");
		INT_QST_Look.Caption(paper, rx, top, rw, "#STR_INT_QST_ED_COL_TEXT", ink, "letter");
		INT_QST_Look.DoubleRule(paper, lx, top + 22, lw);
		INT_QST_Look.DoubleRule(paper, mx, top + 22, mw);
		INT_QST_Look.DoubleRule(paper, rx, top + 22, rw);

		m_Left = new INT_QST_EdColumn();
		m_Left.Build(paper, lx, colTop, lw, colH, m_Clicks, ID_BG);
		m_Mid = new INT_QST_EdColumn();
		m_Mid.Build(paper, mx, colTop, mw, colH, m_Clicks, ID_BG);

		// the giver's text: fixed on top of the right column, the right blocks under it
		float descH = 250;
		if (colH < 640)
			descH = 200;
		INT_QST_Area area = INT_QST_Area.Create(paper, rx, colTop, rw - 14, descH, "#STR_INT_QST_ED_DESC", m_DescText);
		m_Desc = area;
		area.Event_Changed.Insert(OnDescChanged);
		int ink3 = INT_QST_Look.Ink3();
		TextWidget bytes = INT_QST_Look.Text(paper, rx, colTop, rw - 14, 18, "", 11, ink3, INT_UIF_Align.RIGHT);
		m_DescBytes = bytes;
		UpdateDescBytes();
		m_Right = new INT_QST_EdColumn();
		m_Right.Build(paper, rx, colTop + descH + 14, rw, colH - descH - 14, m_Clicks, ID_BG);

		BuildLeft();
		BuildMid();
		BuildRight();
		m_Left.Arrange();
		m_Mid.Arrange();
		m_Right.Arrange();
		BuildBottom(paper, pw, ph);
	}

	protected void BuildHeader(Widget paper, float pw)
	{
		int ink = INT_QST_Look.Ink();
		INT_QST_Look.Icon(paper, 28, 24, 24, "pencil", ink);
		string tk = "#STR_INT_QST_ED_TITLE";
		if (m_IsNew)
			tk = "#STR_INT_QST_ED_TITLE_NEW";
		string title = INT_QST_Look.Tr(tk);
		INT_QST_Look.Heavy(paper, 62, 16, 520, 36, title, 24, ink);
		float x = 62 + INT_UIF_Draw.TextWidth(title, 24) * 1.08 + 16;
		if (m_IsNew)
		{
			int blue = INT_QST_Look.Blue();
			INT_QST_Look.Stamp(paper, x, 21, "#STR_INT_QST_ED_ST_NEW", blue, 11);
			x = x + INT_QST_Look.StampWidth("#STR_INT_QST_ED_ST_NEW", 11) + 12;
		}
		else
		{
			string idText = "#" + m_Q.Id.ToString();
			int ink3 = INT_QST_Look.Ink3();
			INT_QST_Look.Heavy(paper, x, 20, 140, 28, idText, 17, ink3);
			x = x + INT_UIF_Draw.TextWidth(idText, 17) * 1.1 + 16;
		}
		int red = INT_QST_Look.Red();
		Widget dm = INT_QST_Look.Stamp(paper, x, 21, "#STR_INT_QST_ED_ST_CHANGED", red, 11, "pencil");
		m_DirtyMark = dm;
		dm.Show(false);
		// brand + admin line on the right, then the close tile
		int grey = INT_QST_Look.Ink3();
		INT_QST_Look.Text(paper, pw - 90 - 380, 14, 380, 20, "DayZavr Quest System", 14, grey, INT_UIF_Align.RIGHT);
		string adm = INT_QST_Look.Spaced("#STR_INT_QST_ED_ADMIN");
		INT_QST_Look.Heavy(paper, pw - 90 - 380, 36, 380, 18, adm, 11, red, INT_UIF_Align.RIGHT);
		BuildClose(paper, pw - 28 - 40, 16);
		INT_QST_Look.DoubleRule(paper, 28, 66, pw - 56);
	}

	protected void BuildClose(Widget p, float x, float y)
	{
		Widget f = INT_UIF_Draw.Frame(p, x, y, 40, 40, "Close");
		int fill = 0x26FFFFFF;
		int edge = INT_QST_Look.InkA(90);
		INT_UIF_Surface.Box(f, 0, 0, 40, 40, 6, fill, edge);
		Widget hover = INT_UIF_Draw.Frame(f, 0, 0, 40, 40, "Hover");
		int hf = INT_QST_Look.RedA(40);
		int hb = INT_QST_Look.RedA(200);
		INT_UIF_Surface.Box(hover, 0, 0, 40, 40, 6, hf, hb);
		INT_QST_Look.Icon(f, 12, 12, 16, "close", INT_QST_Look.Ink());
		ButtonWidget hit = INT_UIF_Draw.Hit(f, 0, 0, 40, 40, m_Clicks);
		m_Clicks.Bind(hit, ID_CLOSE, 0, hover);
	}

	protected void BuildBottom(Widget paper, float pw, float ph)
	{
		float by = ph - 66;
		INT_QST_Look.Hair(paper, 28, ph - 80, pw - 56);
		float leftEnd = 28;
		if (!m_IsNew)
		{
			int rc = INT_QST_Look.Red();
			TextButton(paper, 28, by, 210, 48, "#STR_INT_QST_ED_B_DELETE", "trash", INT_QST_Look.BTN_DANGER, rc, ID_DELETE, 0);
			leftEnd = 28 + 210;
		}
		int sc = INT_QST_Look.Sticky();
		float saveX = pw - 28 - 260;
		TextButton(paper, saveX, by, 260, 48, "#STR_INT_QST_ED_B_SAVE", "save", INT_QST_Look.BTN_INK, sc, ID_SAVE, 0);
		float cancelX = saveX - 14 - 190;
		TextButton(paper, cancelX, by, 190, 48, "#STR_INT_QST_ED_B_CANCEL", "", INT_QST_Look.BTN_GHOST, 0, ID_CANCEL, 0);
		float fx = leftEnd + 20;
		m_FlashW = cancelX - 20 - fx;
		Widget fl = INT_UIF_Draw.Frame(paper, fx, by, m_FlashW, 48, "Flash");
		m_Flash = fl;
		fl.Show(false);
	}

	// --- inputs helpers ----------------------------------------------------------------------------------------------------
	protected void Wire(INT_QST_EdBlock b, INT_QST_Field f, int scope, int index, int prop)
	{
		f.m_Tag = INT_QST_QuestEditor.Tag(scope, index, prop);
		f.Event_Changed.Insert(OnFieldChanged);
		b.m_Hold.Insert(f);
	}

	protected void WireChips(INT_QST_EdBlock b, INT_QST_EdChips c, int scope, int index, int prop)
	{
		c.m_Tag = INT_QST_QuestEditor.Tag(scope, index, prop);
		c.Event_Changed.Insert(OnChipsChanged);
		b.m_Hold.Insert(c);
	}

	protected void WireCheck(INT_QST_EdBlock b, INT_QST_Check c, int scope, int index, int prop)
	{
		c.m_Tag = INT_QST_QuestEditor.Tag(scope, index, prop);
		c.Event_Changed.Insert(OnCheckChanged);
		b.m_Hold.Insert(c);
	}

	protected void TextButton(Widget p, float x, float y, float w, float h, string labelKey, string icon, int style, int iconColor, int id, int data)
	{
		Widget hv;
		Widget f = INT_QST_Look.Button(p, x, y, w, h, labelKey, icon, style, iconColor, hv);
		ButtonWidget hit = INT_UIF_Draw.Hit(f, 0, 0, w, h, m_Clicks);
		m_Clicks.Bind(hit, id, data, hv);
	}

	// Small square button with an icon (flip = the icon turned by 180 degrees: "up" from the chevron).
	protected void IconBtn(Widget p, float x, float y, float w, float h, string icon, int color, int id, int data, bool flip)
	{
		Widget f = INT_UIF_Draw.Frame(p, x, y, w, h, "IconBtn");
		int fill = 0x26FFFFFF;
		int edge = INT_QST_Look.InkA(90);
		INT_UIF_Surface.Box(f, 0, 0, w, h, 4, fill, edge);
		Widget hv = INT_UIF_Draw.Frame(f, 0, 0, w, h, "Hover");
		int hb = INT_QST_Look.Ink();
		INT_UIF_Surface.Box(hv, 0, 0, w, h, 4, 0x52FFFFFF, hb);
		float s = 16;
		if (h < 30)
			s = 14;
		ImageWidget ic = INT_QST_Look.Icon(f, (w - s) * 0.5, (h - s) * 0.5, s, icon, color);
		if (flip && ic)
			ic.SetRotation(0, 0, 180);
		ButtonWidget hit = INT_UIF_Draw.Hit(f, 0, 0, w, h, m_Clicks);
		m_Clicks.Bind(hit, id, data, hv);
	}

	protected void HereButton(Widget p, float x, float y, int tag)
	{
		int blue = INT_QST_Look.Blue();
		TextButton(p, x, y, 160, 34, "#STR_INT_QST_ED_B_HERE", "map_pin", INT_QST_Look.BTN_GHOST, blue, ID_HERE, tag);
	}

	// Caption + a box with the value and a chevron; returns the value text.
	protected TextWidget SelectBox(Widget p, float x, float y, float w, string labelKey, string value, int id, int data)
	{
		string cap = INT_QST_Look.Spaced(labelKey);
		int ink3 = INT_QST_Look.Ink3();
		INT_QST_Look.Text(p, x, y, w, 18, cap, 11, ink3);
		Widget f = INT_UIF_Draw.Frame(p, x, y + 20, w, 34, "Select");
		int fill = 0x2EFFFFFF;
		int edge = INT_QST_Look.InkA(90);
		INT_UIF_Surface.Box(f, 0, 0, w, 34, 4, fill, edge);
		Widget hover = INT_UIF_Draw.Frame(f, 0, 0, w, 34, "Hover");
		int hb = INT_QST_Look.Blue();
		INT_UIF_Surface.Box(hover, 0, 0, w, 34, 4, 0x14FFFFFF, hb);
		string t = INT_QST_Look.Fit(value, w - 44, 15);
		int ink = INT_QST_Look.Ink();
		TextWidget tw = INT_QST_Look.Text(f, 10, 0, w - 44, 34, t, 15, ink);
		INT_QST_Look.Icon(f, w - 26, 10, 14, "chevron_down", INT_QST_Look.Ink2());
		ButtonWidget hit = INT_UIF_Draw.Hit(f, 0, 0, w, 34, m_Clicks);
		m_Clicks.Bind(hit, id, data, hover);
		return tw;
	}

	protected void SetSelect(TextWidget tw, string text)
	{
		if (!tw)
			return;
		string t = INT_QST_Look.Fit(text, m_GiverW, 15);
		tw.SetText(t);
	}

	// Class field + search button (+ count field on the right when countProp > 0). pickKind 0 = action list.
	protected INT_QST_EdClassBox ClassRow(INT_QST_EdBlock b, float x, float y, float w, string labelKey, string value, int pickKind, int scope, int index, int prop, int countProp, string countValue)
	{
		float fw = w;
		if (countProp > 0)
			fw = w - 108;
		INT_QST_EdClassBox box = new INT_QST_EdClassBox();
		box.m_Owner = this;
		box.m_W = fw - 4;
		if (pickKind == 0)
			box.m_Action = true;
		INT_QST_Field f = INT_QST_Field.Create(b.m_Root, x, y, fw - 44, labelKey, value, 120);
		int tag = INT_QST_QuestEditor.Tag(scope, index, prop);
		f.m_Tag = tag;
		f.Event_Changed.Insert(box.OnChanged);
		box.m_Field = f;
		int ink = INT_QST_Look.Ink();
		if (pickKind == 0)
			IconBtn(b.m_Root, x + fw - 38, y + 20, 38, 34, "list", ink, ID_ACTIONS, index, false);
		else
			IconBtn(b.m_Root, x + fw - 38, y + 20, 38, 34, "search", ink, ID_PICK, tag, false);
		int ink3 = INT_QST_Look.Ink3();
		TextWidget info = INT_QST_Look.Text(b.m_Root, x + 2, y + 56, fw - 4, 18, "", 12, ink3);
		box.m_Info = info;
		box.Describe();
		b.m_Hold.Insert(box);
		if (countProp > 0)
		{
			// digits as the server limits allow: targets up to MAX_TARGET_COUNT, items up to MAX_ITEM_COUNT
			int countLen = 4;
			if (scope == SC_TGT)
				countLen = 6;
			INT_QST_Field cf = INT_QST_Field.Create(b.m_Root, x + w - 96, y, 96, "#STR_INT_QST_ED_COUNT", countValue, countLen, true);
			Wire(b, cf, scope, index, countProp);
		}
		return box;
	}

	// Two numeric fields (hours / minutes style) and a grey hint under them.
	protected void HoursMinutes(INT_QST_EdBlock b, float y, string k1, int v1, int p1, string k2, int v2, int p2, string hintKey)
	{
		string t1 = v1.ToString();
		string t2 = v2.ToString();
		INT_QST_Field f1 = INT_QST_Field.Create(b.m_Root, 0, y, 124, k1, t1, 4, true);
		Wire(b, f1, SC_MAIN, 0, p1);
		INT_QST_Field f2 = INT_QST_Field.Create(b.m_Root, 136, y, 124, k2, t2, 2, true);
		Wire(b, f2, SC_MAIN, 0, p2);
		if (hintKey == "")
			return;
		string hint = INT_QST_Look.Tr(hintKey);
		float hw = m_Left.BlockW() - 4;
		hint = INT_QST_Look.Fit(hint, hw, 12);
		INT_QST_Look.Text(b.m_Root, 2, y + 56, hw, 18, hint, 12, INT_QST_Look.Ink3());
	}

	// --- left column ---------------------------------------------------------------------------------------------------------
	protected void BuildLeft()
	{
		array<ref INT_QST_EdBlock> mains = new array<ref INT_QST_EdBlock>();
		mains.Insert(BlockName());
		mains.Insert(BlockBranch());
		mains.Insert(BlockOrder());
		mains.Insert(BlockGivers());
		mains.Insert(BlockRepeat());
		m_Left.ReplaceSection(SEC_L_MAIN, mains);
		array<ref INT_QST_EdBlock> args = new array<ref INT_QST_EdBlock>();
		FillRepeatArgs(args);
		m_Left.ReplaceSection(SEC_L_REPARGS, args);
		array<ref INT_QST_EdBlock> lim = new array<ref INT_QST_EdBlock>();
		lim.Insert(BlockLimit());
		m_Left.ReplaceSection(SEC_L_LIMIT, lim);
		array<ref INT_QST_EdBlock> rh = new array<ref INT_QST_EdBlock>();
		rh.Insert(BlockReqHead());
		m_Left.ReplaceSection(SEC_L_REQHEAD, rh);
		array<ref INT_QST_EdBlock> reqs = new array<ref INT_QST_EdBlock>();
		FillReqRows(reqs);
		m_Left.ReplaceSection(SEC_L_REQS, reqs);
		array<ref INT_QST_EdBlock> ra = new array<ref INT_QST_EdBlock>();
		ra.Insert(BlockReqAdd());
		m_Left.ReplaceSection(SEC_L_REQADD, ra);
		array<ref INT_QST_EdBlock> note = new array<ref INT_QST_EdBlock>();
		note.Insert(BlockNote());
		m_Left.ReplaceSection(SEC_L_NOTE, note);
	}

	protected INT_QST_EdBlock BlockName()
	{
		INT_QST_EdBlock b = m_Left.NewBlock(SEC_L_MAIN, 0);
		float w = m_Left.BlockW();
		INT_QST_Field f = INT_QST_Field.Create(b.m_Root, 0, 0, w, "#STR_INT_QST_ED_NAME", m_Q.Name, 100);
		Wire(b, f, SC_MAIN, 0, P_NAME);
		b.Finish(w, 66);
		return b;
	}

	protected INT_QST_EdBlock BlockBranch()
	{
		INT_QST_EdBlock b = m_Left.NewBlock(SEC_L_MAIN, 1);
		float w = m_Left.BlockW();
		INT_QST_Field f = INT_QST_Field.Create(b.m_Root, 0, 0, w - 46, "#STR_INT_QST_ED_BRANCH", m_BranchKey, MAX_KEY);
		Wire(b, f, SC_MAIN, 0, P_BRANCH);
		m_BranchField = f;
		int ink = INT_QST_Look.Ink();
		IconBtn(b.m_Root, w - 38, 20, 38, 34, "list", ink, ID_BRANCH_LIST, 0, false);
		int ink3 = INT_QST_Look.Ink3();
		TextWidget info = INT_QST_Look.Text(b.m_Root, 2, 57, w - 4, 18, "", 12, ink3);
		m_BranchInfo = info;
		UpdateBranchInfo();
		b.Finish(w, 84);
		return b;
	}

	protected INT_QST_EdBlock BlockOrder()
	{
		INT_QST_EdBlock b = m_Left.NewBlock(SEC_L_MAIN, 2);
		float w = m_Left.BlockW();
		string ov = m_Q.Order.ToString();
		INT_QST_Field f = INT_QST_Field.Create(b.m_Root, 0, 0, 150, "#STR_INT_QST_ED_ORDER", ov, 5, true);
		Wire(b, f, SC_MAIN, 0, P_ORDER);
		bool on = false;
		if (m_Q.Enabled != 0)
			on = true;
		INT_QST_Check c = INT_QST_Check.Create(b.m_Root, 176, 24, w - 176, "#STR_INT_QST_ED_ENABLED", on);
		WireCheck(b, c, SC_MAIN, 0, P_ENABLED);
		b.Finish(w, 66);
		return b;
	}

	protected INT_QST_EdBlock BlockGivers()
	{
		INT_QST_EdBlock b = m_Left.NewBlock(SEC_L_MAIN, 3);
		float w = m_Left.BlockW();
		float half = (w - 12) * 0.5;
		m_GiverW = half - 44;
		string gs = GiverLabel(m_Q.GiverStart, true);
		string gf = GiverLabel(m_Q.GiverFinish, false);
		TextWidget ts = SelectBox(b.m_Root, 0, 0, half, "#STR_INT_QST_ED_GSTART", gs, ID_GIVER, 0);
		TextWidget tf = SelectBox(b.m_Root, half + 12, 0, half, "#STR_INT_QST_ED_GFINISH", gf, ID_GIVER, 1);
		m_GStartText = ts;
		m_GFinishText = tf;
		b.Finish(w, 66);
		return b;
	}

	protected INT_QST_EdBlock BlockRepeat()
	{
		INT_QST_EdBlock b = m_Left.NewBlock(SEC_L_MAIN, 4);
		float w = m_Left.BlockW();
		array<string> labels = new array<string>();
		labels.Insert("#STR_INT_QST_ED_R_ONCE");
		labels.Insert("#STR_INT_QST_ED_R_COOLDOWN");
		labels.Insert("#STR_INT_QST_ED_R_DAILY");
		labels.Insert("#STR_INT_QST_ED_R_WEEKLY");
		INT_QST_EdChips c = INT_QST_EdChips.Create(b.m_Root, 0, 0, w, "#STR_INT_QST_ED_REPEAT", labels, m_Repeat);
		WireChips(b, c, SC_MAIN, 0, P_REPEAT);
		b.Finish(w, c.m_H + 10);
		return b;
	}

	// Fields of the chosen repeat mode (rebuilt when the mode changes).
	protected void FillRepeatArgs(array<ref INT_QST_EdBlock> blocks)
	{
		INT_QST_EdBlock b = m_Left.NewBlock(SEC_L_REPARGS, 0);
		float w = m_Left.BlockW();
		float y = 0;
		if (m_Repeat == INT_QST_RM.COOLDOWN)
		{
			int ch = m_Q.Cooldown / 3600;
			int cm = (m_Q.Cooldown % 3600) / 60;
			HoursMinutes(b, y, "#STR_INT_QST_ED_HOURS", ch, P_CD_H, "#STR_INT_QST_ED_MINUTES", cm, P_CD_M, "#STR_INT_QST_ED_CD_HINT");
			y = y + 80;
		}
		else if (m_Repeat == INT_QST_RM.DAILY)
		{
			HoursMinutes(b, y, "#STR_INT_QST_ED_RS_HOUR", m_Q.ResetHour, P_RS_H, "#STR_INT_QST_ED_MINUTES", m_Q.ResetMinute, P_RS_M, "#STR_INT_QST_ED_RS_HINT");
			y = y + 80;
		}
		else if (m_Repeat == INT_QST_RM.WEEKLY)
		{
			array<string> wd = new array<string>();
			wd.Insert("#STR_INT_QST_ED_WD_1");
			wd.Insert("#STR_INT_QST_ED_WD_2");
			wd.Insert("#STR_INT_QST_ED_WD_3");
			wd.Insert("#STR_INT_QST_ED_WD_4");
			wd.Insert("#STR_INT_QST_ED_WD_5");
			wd.Insert("#STR_INT_QST_ED_WD_6");
			wd.Insert("#STR_INT_QST_ED_WD_7");
			int day = INT_QST_Util.ClampI(m_Q.ResetWeekDay, 1, 7) - 1;
			INT_QST_EdChips c = INT_QST_EdChips.Create(b.m_Root, 0, y, w, "#STR_INT_QST_ED_WEEKDAY", wd, day);
			WireChips(b, c, SC_MAIN, 0, P_WEEKDAY);
			y = y + c.m_H + 10;
			HoursMinutes(b, y, "#STR_INT_QST_ED_RS_HOUR", m_Q.ResetHour, P_RS_H, "#STR_INT_QST_ED_MINUTES", m_Q.ResetMinute, P_RS_M, "#STR_INT_QST_ED_RS_HINT");
			y = y + 80;
		}
		else
		{
			string once = INT_QST_Look.Tr("#STR_INT_QST_ED_R_ONCE_HINT");
			once = INT_QST_Look.Fit(once, w - 4, 13);
			INT_QST_Look.Text(b.m_Root, 2, 0, w - 4, 24, once, 13, INT_QST_Look.Ink3());
			y = 32;
		}
		b.Finish(w, y);
		blocks.Insert(b);
	}

	protected INT_QST_EdBlock BlockLimit()
	{
		INT_QST_EdBlock b = m_Left.NewBlock(SEC_L_LIMIT, 0);
		float w = m_Left.BlockW();
		int lh = m_Q.TimeLimit / 3600;
		int lm = (m_Q.TimeLimit % 3600) / 60;
		HoursMinutes(b, 0, "#STR_INT_QST_ED_LIMIT_H", lh, P_TL_H, "#STR_INT_QST_ED_MINUTES", lm, P_TL_M, "#STR_INT_QST_ED_LIMIT_HINT");
		b.Finish(w, 82);
		return b;
	}

	protected INT_QST_EdBlock BlockReqHead()
	{
		INT_QST_EdBlock b = m_Left.NewBlock(SEC_L_REQHEAD, 0);
		float w = m_Left.BlockW();
		int ink2 = INT_QST_Look.Ink2();
		INT_QST_Look.Caption(b.m_Root, 0, 10, w - 150, "#STR_INT_QST_ED_REQUIRES", ink2, "lock");
		array<string> labels = new array<string>();
		labels.Insert("#STR_INT_QST_ED_REQ_ALL");
		labels.Insert("#STR_INT_QST_ED_REQ_ANY");
		int sel = INT_QST_Util.ClampI(m_Q.RequireAny, 0, 1);
		INT_QST_EdChips c = INT_QST_EdChips.Create(b.m_Root, w - 146, 2, 146, "", labels, sel);
		WireChips(b, c, SC_MAIN, 0, P_REQANY);
		INT_QST_Look.Hair(b.m_Root, 0, 39, w);
		b.Finish(w, 42);
		return b;
	}

	protected void FillReqRows(array<ref INT_QST_EdBlock> blocks)
	{
		float w = m_Left.BlockW();
		int n = m_Q.Requires.Count();
		if (n == 0)
		{
			INT_QST_EdBlock eb = m_Left.NewBlock(SEC_L_REQS, 0);
			string noneText = INT_QST_Look.Tr("#STR_INT_QST_ED_REQ_NONE");
			INT_QST_Look.Text(eb.m_Root, 4, 0, w - 8, 30, noneText, 13, INT_QST_Look.Ink3());
			eb.Finish(w, 30);
			blocks.Insert(eb);
			return;
		}
		int red = INT_QST_Look.Red();
		for (int i = 0; i < n; i++)
		{
			int rid = m_Q.Requires[i];
			INT_QST_EdBlock b = m_Left.NewBlock(SEC_L_REQS, i);
			INT_QST_QuestDef rq = INT_QST_Client.s_Catalog.Get(rid);
			string text = "#" + rid.ToString() + "   ";
			int c = INT_QST_Look.Ink();
			if (rq)
			{
				text = text + rq.Name;
			}
			else
			{
				string miss = INT_QST_Look.Tr("#STR_INT_QST_ED_REQ_MISSING");
				text = text + miss;
				c = INT_QST_Look.RedA(210);
			}
			text = INT_QST_Look.Fit(text, w - 46, 14);
			INT_QST_Look.Text(b.m_Root, 6, 0, w - 46, 32, text, 14, c);
			IconBtn(b.m_Root, w - 30, 3, 26, 26, "close", red, ID_REQ_DEL, i, false);
			INT_QST_Look.Hair(b.m_Root, 0, 31, w);
			b.Finish(w, 32);
			blocks.Insert(b);
		}
	}

	protected INT_QST_EdBlock BlockReqAdd()
	{
		INT_QST_EdBlock b = m_Left.NewBlock(SEC_L_REQADD, 0);
		float w = m_Left.BlockW();
		int ink = INT_QST_Look.Ink();
		TextButton(b.m_Root, 0, 8, 260, 38, "#STR_INT_QST_ED_B_ADD_REQ", "plus", INT_QST_Look.BTN_GHOST, ink, ID_REQ_ADD, 0);
		b.Finish(w, 56);
		return b;
	}

	protected INT_QST_EdBlock BlockNote()
	{
		INT_QST_EdBlock b = m_Left.NewBlock(SEC_L_NOTE, 0);
		float w = m_Left.BlockW();
		INT_QST_Field f = INT_QST_Field.Create(b.m_Root, 0, 0, w, "#STR_INT_QST_ED_NOTE", m_Q.Note, 300);
		Wire(b, f, SC_MAIN, 0, P_NOTE);
		string hint = INT_QST_Look.Tr("#STR_INT_QST_ED_NOTE_HINT");
		hint = INT_QST_Look.Fit(hint, w - 4, 12);
		INT_QST_Look.Text(b.m_Root, 2, 56, w - 4, 18, hint, 12, INT_QST_Look.Ink3());
		b.Finish(w, 82);
		return b;
	}

	// --- middle column: targets ----------------------------------------------------------------------------------------------
	protected void BuildMid()
	{
		array<ref INT_QST_EdBlock> head = new array<ref INT_QST_EdBlock>();
		head.Insert(BlockTargetHead());
		m_Mid.ReplaceSection(SEC_M_HEAD, head);
		array<ref INT_QST_EdBlock> cards = new array<ref INT_QST_EdBlock>();
		FillTargets(cards);
		m_Mid.ReplaceSection(SEC_M_TGTS, cards);
	}

	protected INT_QST_EdBlock BlockTargetHead()
	{
		INT_QST_EdBlock b = m_Mid.NewBlock(SEC_M_HEAD, 0);
		float w = m_Mid.BlockW();
		int ink2 = INT_QST_Look.Ink2();
		TextWidget cnt = INT_QST_Look.Text(b.m_Root, 2, 6, w - 260, 34, "", 14, ink2);
		m_TgtCount = cnt;
		UpdateTargetCount();
		int ink = INT_QST_Look.Ink();
		TextButton(b.m_Root, w - 240, 4, 240, 38, "#STR_INT_QST_ED_B_ADD_TARGET", "plus", INT_QST_Look.BTN_GHOST, ink, ID_TGT_ADD, 0);
		b.Finish(w, 54);
		return b;
	}

	protected void UpdateTargetCount()
	{
		if (!m_TgtCount)
			return;
		string have = m_Q.Targets.Count().ToString();
		string most = INT_QST_Const.MAX_TARGETS.ToString();
		string line = INT_QST_Look.Fmt("#STR_INT_QST_ED_TARGETS_N", have, most);
		m_TgtCount.SetText(line);
	}

	protected void FillTargets(array<ref INT_QST_EdBlock> blocks)
	{
		int n = m_Q.Targets.Count();
		if (n > 0)
		{
			for (int i = 0; i < n; i++)
				blocks.Insert(BuildTargetCard(i));
			return;
		}
		INT_QST_EdBlock b = m_Mid.NewBlock(SEC_M_TGTS, 0);
		float w = m_Mid.BlockW();
		array<string> lines = new array<string>();
		string text = INT_QST_Look.Tr("#STR_INT_QST_ED_NO_TARGETS");
		INT_QST_Text.Wrap(text, w - 40, 14, lines);
		float y = 12;
		int ink3 = INT_QST_Look.Ink3();
		INT_QST_Look.Icon(b.m_Root, 4, y + 2, 16, "info", ink3);
		foreach (string ln : lines)
		{
			INT_QST_Look.Text(b.m_Root, 30, y, w - 34, 22, ln, 14, ink3);
			y = y + 22;
		}
		b.Finish(w, y + 12);
		blocks.Insert(b);
	}

	protected INT_QST_EdBlock BuildTargetCard(int i)
	{
		INT_QST_TargetDef t = m_Q.Targets[i];
		int kind = INT_QST_TT.FromString(t.Kind);
		INT_QST_EdBlock b = m_Mid.NewBlock(SEC_M_TGTS, i);
		float w = m_Mid.BlockW();
		float iw = w - 24;
		Widget bgf = INT_UIF_Draw.Frame(b.m_Root, 0, 0, w, 10, "CardBg");
		float y = 10;
		int no = i + 1;
		string num = "#" + no.ToString();
		int ink = INT_QST_Look.Ink();
		INT_QST_Look.Heavy(b.m_Root, 12, y + 2, 40, 26, num, 16, ink);
		int n = m_Q.Targets.Count();
		float bx = w - 12 - 26;
		int red = INT_QST_Look.Red();
		IconBtn(b.m_Root, bx, y, 26, 26, "close", red, ID_TGT_DEL, i, false);
		if (i < n - 1)
			IconBtn(b.m_Root, bx - 30, y, 26, 26, "chevron_down", ink, ID_TGT_DOWN, i, false);
		if (i > 0)
			IconBtn(b.m_Root, bx - 60, y, 26, 26, "chevron_down", ink, ID_TGT_UP, i, true);
		array<string> kinds = new array<string>();
		kinds.Insert("#STR_INT_QST_ED_K_KILL");
		kinds.Insert("#STR_INT_QST_ED_K_DELIVER");
		kinds.Insert("#STR_INT_QST_ED_K_SEARCH");
		kinds.Insert("#STR_INT_QST_ED_K_CRAFT");
		kinds.Insert("#STR_INT_QST_ED_K_FISH");
		kinds.Insert("#STR_INT_QST_ED_K_HARVEST");
		kinds.Insert("#STR_INT_QST_ED_K_EXPLORE");
		kinds.Insert("#STR_INT_QST_ED_K_ACTION");
		float kw = bx - 60 - 10 - 50;
		INT_QST_EdChips kc = INT_QST_EdChips.Create(b.m_Root, 50, y, kw, "", kinds, kind - 1);
		WireChips(b, kc, SC_TGT, i, T_KIND);
		float kh = kc.m_H;
		if (kh < 28)
			kh = 28;
		y = y + kh + 12;
		if (kind == INT_QST_TT.KILL)
			y = KillFields(b, t, i, iw, y);
		else if (kind == INT_QST_TT.DELIVER || kind == INT_QST_TT.SEARCH)
			y = ItemFields(b, t, i, iw, y);
		else if (kind == INT_QST_TT.CRAFT || kind == INT_QST_TT.FISH || kind == INT_QST_TT.HARVEST)
			y = CountedFields(b, t, i, iw, y, kind);
		else if (kind == INT_QST_TT.EXPLORE)
			y = ExploreFields(b, t, i, iw, y);
		else if (kind == INT_QST_TT.ACTION)
			y = ActionFields(b, t, i, iw, y);
		// the line the player reads
		INT_QST_Field tf = INT_QST_Field.Create(b.m_Root, 12, y, iw, "#STR_INT_QST_ED_T_TEXT", t.Text, 120);
		Wire(b, tf, SC_TGT, i, T_TEXT);
		y = y + 62;
		INT_UIF_Draw.Resize(bgf, w, y);
		int fill = 0x33FFFAEB;
		int edge = INT_QST_Look.InkA(80);
		INT_UIF_Surface.Box(bgf, 0, 0, w, y, 6, fill, edge);
		INT_UIF_Draw.Line(bgf, 0, 0, 3, y, INT_QST_Look.InkA(150));
		b.Finish(w, y + 12);
		return b;
	}

	protected float KillFields(INT_QST_EdBlock b, INT_QST_TargetDef t, int i, float iw, float y)
	{
		string cnt = INT_QST_QuestEditor.CountText(t.Count);
		ClassRow(b, 12, y, iw, "#STR_INT_QST_ED_T_KILL_WHO", t.Target, INT_QST_ClassPicker.K_CREATURE, SC_TGT, i, T_TARGET, T_COUNT, cnt);
		float ky = y + 76;
		ClassRow(b, 12, ky, iw, "#STR_INT_QST_ED_T_WEAPON", t.Filter, INT_QST_ClassPicker.K_ITEM, SC_TGT, i, T_FILTER, 0, "");
		ky = ky + 76;
		// kill zone: centre + my position (+ radius on the same row when there is room)
		bool oneRow = iw >= 560;
		float cw = iw - 168;
		if (oneRow)
			cw = iw - 168 - 152;
		INT_QST_Field zc = INT_QST_Field.Create(b.m_Root, 12, ky, cw, "#STR_INT_QST_ED_T_ZONE", t.ZoneCenter, 80);
		Wire(b, zc, SC_TGT, i, T_ZONEC);
		int zoneTag = INT_QST_QuestEditor.Tag(SC_TGT, i, T_ZONEC);
		HereButton(b.m_Root, 12 + cw + 8, ky + 20, zoneTag);
		string zr = INT_QST_QuestEditor.NumText(t.ZoneRadius);
		if (oneRow)
		{
			INT_QST_Field zf = INT_QST_Field.Create(b.m_Root, 12 + iw - 144, ky, 144, "#STR_INT_QST_ED_T_RADIUS", zr, 8, true, true);
			Wire(b, zf, SC_TGT, i, T_ZONER);
			return ky + 62;
		}
		ky = ky + 62;
		INT_QST_Field zf2 = INT_QST_Field.Create(b.m_Root, 12, ky, 160, "#STR_INT_QST_ED_T_RADIUS", zr, 8, true, true);
		Wire(b, zf2, SC_TGT, i, T_ZONER);
		return ky + 62;
	}

	protected float ItemFields(INT_QST_EdBlock b, INT_QST_TargetDef t, int i, float iw, float y)
	{
		string cnt = INT_QST_QuestEditor.CountText(t.Count);
		ClassRow(b, 12, y, iw, "#STR_INT_QST_ED_T_ITEM", t.Target, INT_QST_ClassPicker.K_ITEM, SC_TGT, i, T_TARGET, T_COUNT, cnt);
		float iy = y + 76;
		array<string> labels = new array<string>();
		labels.Insert("#STR_INT_QST_ED_ST_ANY");
		labels.Insert("#STR_INT_QST_ED_ST_0");
		labels.Insert("#STR_INT_QST_ED_ST_1");
		labels.Insert("#STR_INT_QST_ED_ST_2");
		labels.Insert("#STR_INT_QST_ED_ST_3");
		labels.Insert("#STR_INT_QST_ED_ST_4");
		int ws = INT_QST_State.LevelFromString(t.WorstState) + 1;
		INT_QST_EdChips sc = INT_QST_EdChips.Create(b.m_Root, 12, iy, iw, "#STR_INT_QST_ED_T_WORST", labels, ws);
		WireChips(b, sc, SC_TGT, i, T_WORST);
		iy = iy + sc.m_H + 10;
		string mq = t.MinQuantity.ToString();
		string mqLabel = INT_QST_Look.Fmt("#STR_INT_QST_ED_T_MINQ", "%");
		INT_QST_Field mf = INT_QST_Field.Create(b.m_Root, 12, iy, 200, mqLabel, mq, 3, true);
		Wire(b, mf, SC_TGT, i, T_MINQ);
		return iy + 62;
	}

	protected float CountedFields(INT_QST_EdBlock b, INT_QST_TargetDef t, int i, float iw, float y, int kind)
	{
		string lk = "#STR_INT_QST_ED_T_CRAFT";
		if (kind == INT_QST_TT.FISH)
			lk = "#STR_INT_QST_ED_T_FISH";
		else if (kind == INT_QST_TT.HARVEST)
			lk = "#STR_INT_QST_ED_T_HARVEST";
		string cnt = INT_QST_QuestEditor.CountText(t.Count);
		ClassRow(b, 12, y, iw, lk, t.Target, INT_QST_ClassPicker.K_ITEM, SC_TGT, i, T_TARGET, T_COUNT, cnt);
		return y + 76;
	}

	protected float ExploreFields(INT_QST_EdBlock b, INT_QST_TargetDef t, int i, float iw, float y)
	{
		float pw = iw - 168;
		INT_QST_Field pf = INT_QST_Field.Create(b.m_Root, 12, y, pw, "#STR_INT_QST_ED_T_POS", t.Position, 80);
		Wire(b, pf, SC_TGT, i, T_POS);
		int posTag = INT_QST_QuestEditor.Tag(SC_TGT, i, T_POS);
		HereButton(b.m_Root, 12 + pw + 8, y + 20, posTag);
		float ey = y + 62;
		string rv = INT_QST_QuestEditor.NumText(t.Radius);
		INT_QST_Field rf = INT_QST_Field.Create(b.m_Root, 12, ey, 130, "#STR_INT_QST_ED_T_RADIUS", rv, 8, true, true);
		Wire(b, rf, SC_TGT, i, T_RADIUS);
		bool ground = false;
		if (t.GroundY != 0)
			ground = true;
		bool onMap = false;
		if (t.ShowOnMap != 0)
			onMap = true;
		float cx = 12 + 146;
		float cw = (iw - 146) * 0.5;
		if (cw >= 200)
		{
			INT_QST_Check g1 = INT_QST_Check.Create(b.m_Root, cx, ey + 24, cw, "#STR_INT_QST_ED_T_GROUND", ground);
			WireCheck(b, g1, SC_TGT, i, T_GROUND);
			INT_QST_Check m1 = INT_QST_Check.Create(b.m_Root, cx + cw, ey + 24, cw, "#STR_INT_QST_ED_T_MAP", onMap);
			WireCheck(b, m1, SC_TGT, i, T_MAP);
			return ey + 62;
		}
		float cw2 = iw - 146;
		INT_QST_Check g2 = INT_QST_Check.Create(b.m_Root, cx, ey + 4, cw2, "#STR_INT_QST_ED_T_GROUND", ground);
		WireCheck(b, g2, SC_TGT, i, T_GROUND);
		INT_QST_Check m2 = INT_QST_Check.Create(b.m_Root, cx, ey + 34, cw2, "#STR_INT_QST_ED_T_MAP", onMap);
		WireCheck(b, m2, SC_TGT, i, T_MAP);
		return ey + 66;
	}

	protected float ActionFields(INT_QST_EdBlock b, INT_QST_TargetDef t, int i, float iw, float y)
	{
		string cnt = INT_QST_QuestEditor.CountText(t.Count);
		ClassRow(b, 12, y, iw, "#STR_INT_QST_ED_T_ACTION", t.Target, 0, SC_TGT, i, T_TARGET, T_COUNT, cnt);
		float ay = y + 76;
		ClassRow(b, 12, ay, iw, "#STR_INT_QST_ED_T_ACTION_ON", t.Filter, INT_QST_ClassPicker.K_ANY, SC_TGT, i, T_FILTER, 0, "");
		return ay + 76;
	}

	// --- right column: items given on accept, rewards -----------------------------------------------------------------------
	protected void BuildRight()
	{
		array<ref INT_QST_EdBlock> qh = new array<ref INT_QST_EdBlock>();
		qh.Insert(BlockItemsHead());
		m_Right.ReplaceSection(SEC_R_QIHEAD, qh);
		array<ref INT_QST_EdBlock> qis = new array<ref INT_QST_EdBlock>();
		FillQuestItems(qis);
		m_Right.ReplaceSection(SEC_R_QIS, qis);
		array<ref INT_QST_EdBlock> rh = new array<ref INT_QST_EdBlock>();
		rh.Insert(BlockRewardsHead());
		m_Right.ReplaceSection(SEC_R_RWHEAD, rh);
		array<ref INT_QST_EdBlock> rws = new array<ref INT_QST_EdBlock>();
		FillRewards(rws);
		m_Right.ReplaceSection(SEC_R_RWS, rws);
	}

	protected INT_QST_EdBlock BlockItemsHead()
	{
		INT_QST_EdBlock b = m_Right.NewBlock(SEC_R_QIHEAD, 0);
		float w = m_Right.BlockW();
		int ink2 = INT_QST_Look.Ink2();
		INT_QST_Look.Caption(b.m_Root, 0, 14, w - 170, "#STR_INT_QST_ED_QITEMS", ink2, "letter");
		int ink = INT_QST_Look.Ink();
		TextButton(b.m_Root, w - 160, 6, 160, 36, "#STR_INT_QST_ED_B_ADD_ITEM", "plus", INT_QST_Look.BTN_GHOST, ink, ID_QI_ADD, 0);
		INT_QST_Look.Hair(b.m_Root, 0, 47, w);
		b.Finish(w, 52);
		return b;
	}

	protected void FillQuestItems(array<ref INT_QST_EdBlock> blocks)
	{
		int n = m_Q.QuestItems.Count();
		if (n > 0)
		{
			for (int i = 0; i < n; i++)
				blocks.Insert(BuildQuestItem(i));
			return;
		}
		INT_QST_EdBlock b = m_Right.NewBlock(SEC_R_QIS, 0);
		float w = m_Right.BlockW();
		string noneText = INT_QST_Look.Tr("#STR_INT_QST_ED_QI_NONE");
		INT_QST_Look.Text(b.m_Root, 4, 0, w - 8, 30, noneText, 13, INT_QST_Look.Ink3());
		b.Finish(w, 34);
		blocks.Insert(b);
	}

	protected INT_QST_EdBlock BuildQuestItem(int i)
	{
		INT_QST_ItemRef it = m_Q.QuestItems[i];
		INT_QST_EdBlock b = m_Right.NewBlock(SEC_R_QIS, i);
		float w = m_Right.BlockW();
		Widget bgf = INT_UIF_Draw.Frame(b.m_Root, 0, 0, w, 10, "CardBg");
		int red = INT_QST_Look.Red();
		IconBtn(b.m_Root, w - 12 - 26, 10, 26, 26, "close", red, ID_QI_DEL, i, false);
		int c = it.Count;
		if (c < 1)
			c = 1;
		string cnt = c.ToString();
		ClassRow(b, 12, 10, w - 24 - 34, "#STR_INT_QST_ED_T_ITEM", it.Item, INT_QST_ClassPicker.K_ITEM, SC_QI, i, Q_ITEM, Q_COUNT, cnt);
		float y = 10 + 76 + 4;
		INT_UIF_Draw.Resize(bgf, w, y);
		int fill = 0x33FFFAEB;
		int edge = INT_QST_Look.InkA(80);
		INT_UIF_Surface.Box(bgf, 0, 0, w, y, 6, fill, edge);
		b.Finish(w, y + 10);
		return b;
	}

	protected INT_QST_EdBlock BlockRewardsHead()
	{
		INT_QST_EdBlock b = m_Right.NewBlock(SEC_R_RWHEAD, 0);
		float w = m_Right.BlockW();
		int ink2 = INT_QST_Look.Ink2();
		INT_QST_Look.Caption(b.m_Root, 0, 22, w - 300, "#STR_INT_QST_ED_REWARDS", ink2, "star");
		int ink = INT_QST_Look.Ink();
		int red = INT_QST_Look.Red();
		TextButton(b.m_Root, w - 290, 14, 140, 36, "#STR_INT_QST_ED_B_ADD_ITEM", "plus", INT_QST_Look.BTN_GHOST, ink, ID_RW_ADD, INT_QST_RT.ITEM);
		TextButton(b.m_Root, w - 140, 14, 140, 36, "#STR_INT_QST_ED_B_ADD_MONEY", "plus", INT_QST_Look.BTN_GHOST, red, ID_RW_ADD, INT_QST_RT.MONEY);
		INT_QST_Look.Hair(b.m_Root, 0, 55, w);
		b.Finish(w, 60);
		return b;
	}

	protected void FillRewards(array<ref INT_QST_EdBlock> blocks)
	{
		int n = m_Q.Rewards.Count();
		if (n > 0)
		{
			for (int i = 0; i < n; i++)
				blocks.Insert(BuildRewardCard(i));
			return;
		}
		INT_QST_EdBlock b = m_Right.NewBlock(SEC_R_RWS, 0);
		float w = m_Right.BlockW();
		string noneText = INT_QST_Look.Tr("#STR_INT_QST_ED_RW_NONE");
		INT_QST_Look.Text(b.m_Root, 4, 0, w - 8, 30, noneText, 13, INT_QST_Look.Ink3());
		b.Finish(w, 34);
		blocks.Insert(b);
	}

	protected INT_QST_EdBlock BuildRewardCard(int i)
	{
		INT_QST_RewardDef r = m_Q.Rewards[i];
		int kind = INT_QST_RT.FromString(r.Kind);
		INT_QST_EdBlock b = m_Right.NewBlock(SEC_R_RWS, i);
		float w = m_Right.BlockW();
		float iw = w - 24;
		Widget bgf = INT_UIF_Draw.Frame(b.m_Root, 0, 0, w, 10, "CardBg");
		float y = 10;
		int no = i + 1;
		string num = "#" + no.ToString();
		int ink = INT_QST_Look.Ink();
		INT_QST_Look.Heavy(b.m_Root, 12, y + 2, 40, 26, num, 16, ink);
		int red = INT_QST_Look.Red();
		IconBtn(b.m_Root, w - 12 - 26, y, 26, 26, "close", red, ID_RW_DEL, i, false);
		array<string> kinds = new array<string>();
		kinds.Insert("#STR_INT_QST_ED_B_ADD_ITEM");
		kinds.Insert("#STR_INT_QST_ED_B_ADD_MONEY");
		int ks = -1;
		if (kind == INT_QST_RT.ITEM)
			ks = 0;
		else if (kind == INT_QST_RT.MONEY)
			ks = 1;
		INT_QST_EdChips kc = INT_QST_EdChips.Create(b.m_Root, 50, y, w - 50 - 50, "", kinds, ks);
		WireChips(b, kc, SC_RW, i, R_KIND);
		float kh = kc.m_H;
		if (kh < 28)
			kh = 28;
		y = y + kh + 12;
		int ink3 = INT_QST_Look.Ink3();
		if (kind == INT_QST_RT.MONEY)
		{
			string av = r.Amount.ToString();
			INT_QST_Field af = INT_QST_Field.Create(b.m_Root, 12, y, 200, "#STR_INT_QST_ED_RW_AMOUNT", av, 9, true);
			Wire(b, af, SC_RW, i, R_AMOUNT);
			if (INT_QST_Client.s_Hello && !INT_QST_Client.s_Hello.Money)
			{
				string nb = INT_QST_Look.Tr("#STR_INT_QST_ED_RW_NO_BANK");
				nb = INT_QST_Look.Fit(nb, iw - 216, 12);
				int warn = INT_QST_Look.RedA(210);
				INT_QST_Look.Text(b.m_Root, 228, y + 20, iw - 216, 34, nb, 12, warn);
			}
			y = y + 62;
		}
		else
		{
			int rc = r.Count;
			if (rc < 1)
				rc = 1;
			string rcText = rc.ToString();
			ClassRow(b, 12, y, iw, "#STR_INT_QST_ED_T_ITEM", r.Item, INT_QST_ClassPicker.K_ITEM, SC_RW, i, R_ITEM, R_COUNT, rcText);
			y = y + 76;
			INT_QST_Field qf = INT_QST_Field.Create(b.m_Root, 12, y, 170, "#STR_INT_QST_ED_RW_QTY", r.Quantity, 8);
			Wire(b, qf, SC_RW, i, R_QTY);
			string qh = INT_QST_Look.Fmt("#STR_INT_QST_ED_RW_QTY_HINT", "%");
			qh = INT_QST_Look.Fit(qh, iw - 186, 12);
			INT_QST_Look.Text(b.m_Root, 196, y + 20, iw - 186, 34, qh, 12, ink3);
			y = y + 62;
			array<string> states = new array<string>();
			states.Insert("#STR_INT_QST_ED_ST_ASIS");
			states.Insert("#STR_INT_QST_ED_ST_0");
			states.Insert("#STR_INT_QST_ED_ST_1");
			states.Insert("#STR_INT_QST_ED_ST_2");
			states.Insert("#STR_INT_QST_ED_ST_3");
			states.Insert("#STR_INT_QST_ED_ST_4");
			int ss = INT_QST_State.LevelFromString(r.State) + 1;
			INT_QST_EdChips sc = INT_QST_EdChips.Create(b.m_Root, 12, y, iw, "#STR_INT_QST_ED_RW_STATE", states, ss);
			WireChips(b, sc, SC_RW, i, R_STATE);
			y = y + sc.m_H + 10;
		}
		INT_QST_Field tf = INT_QST_Field.Create(b.m_Root, 12, y, iw, "#STR_INT_QST_ED_RW_TEXT", r.Text, 80);
		Wire(b, tf, SC_RW, i, R_TEXT);
		y = y + 62;
		INT_UIF_Draw.Resize(bgf, w, y);
		int fill = 0x33FFFAEB;
		int edge = INT_QST_Look.InkA(80);
		if (kind == INT_QST_RT.MONEY)
			edge = INT_QST_Look.RedA(150);
		INT_UIF_Surface.Box(bgf, 0, 0, w, y, 6, fill, edge);
		if (kind == INT_QST_RT.MONEY)
			INT_UIF_Draw.Line(bgf, 0, 0, 3, y, red);
		b.Finish(w, y + 12);
		return b;
	}

	// --- texts on the sheet ------------------------------------------------------------------------------------------------
	protected string GiverLabel(string key, bool start)
	{
		if (key == "")
		{
			if (start)
				return INT_QST_Look.Tr("#STR_INT_QST_ED_G_NONE_START");
			return INT_QST_Look.Tr("#STR_INT_QST_ED_G_NONE_FINISH");
		}
		INT_QST_GiverView g = INT_QST_Client.Giver(key);
		if (!g)
			return INT_QST_Look.Fmt("#STR_INT_QST_ED_G_UNKNOWN", key);
		if (g.Title != "")
			return g.Title;
		return key;
	}

	protected void UpdateBranchInfo()
	{
		if (!m_BranchInfo)
			return;
		string key = m_BranchKey;
		string t;
		int c;
		if (key == "")
		{
			t = INT_QST_Look.Tr("#STR_INT_QST_ED_BR_EMPTY");
			c = INT_QST_Look.RedA(210);
		}
		else if (INT_QST_Client.s_Catalog.Branch(key))
		{
			string title = INT_QST_Client.s_Catalog.BranchTitle(key);
			t = INT_QST_Look.Fmt("#STR_INT_QST_ED_BR_KNOWN", title);
			c = INT_QST_Look.Ink2();
		}
		else
		{
			t = INT_QST_Look.Tr("#STR_INT_QST_ED_BR_NEW");
			c = INT_QST_Look.Blue();
		}
		float bw = m_Left.BlockW() - 4;
		string fit = INT_QST_Look.Fit(t, bw, 12);
		m_BranchInfo.SetText(fit);
		m_BranchInfo.SetColor(c);
	}

	protected void UpdateDescBytes()
	{
		if (!m_DescBytes)
			return;
		int bytes = m_DescText.Length();
		string bt = bytes.ToString();
		string mt = INT_QST_Const.MAX_TEXT_BYTES.ToString();
		string line = INT_QST_Look.Fmt("#STR_INT_QST_ED_DESC_BYTES", bt, mt);
		int c = INT_QST_Look.Ink3();
		if (bytes > INT_QST_Const.MAX_TEXT_BYTES)
			c = INT_QST_Look.Red();
		m_DescBytes.SetText(line);
		m_DescBytes.SetColor(c);
	}

	protected void MarkDirty()
	{
		if (m_Dirty)
			return;
		m_Dirty = true;
		if (m_DirtyMark)
			m_DirtyMark.Show(true);
	}

	// Short line on the bottom bar, gone after a few seconds (validation problems, limits).
	protected void Flash(string text, bool bad)
	{
		if (!m_Flash)
			return;
		INT_UIF_Animator.Get().Cancel(m_Flash);
		INT_UIF_Widgets.ClearChildren(m_Flash);
		int c = INT_QST_Look.Sticky();
		if (bad)
			c = 0xFFF08A7E;
		float maxW = m_FlashW;
		string t = INT_QST_Look.Fit(text, maxW - 70, 15);
		float tw = INT_UIF_Draw.TextWidth(t, 15) * 1.1 + 60;
		if (tw > maxW)
			tw = maxW;
		float x = (maxW - tw) * 0.5;
		INT_UIF_Surface.Box(m_Flash, x, 4, tw, 40, 6, 0xF21A130C, c);
		string icon = "info";
		if (bad)
			icon = "warning";
		INT_QST_Look.Icon(m_Flash, x + 14, 16, 16, icon, c);
		INT_QST_Look.Heavy(m_Flash, x + 40, 4, tw - 50, 40, t, 15, c);
		m_Flash.SetAlpha(1);
		m_Flash.Show(true);
		m_FlashLeft = 4.5;
		if (bad)
			INT_QST_Sfx.Play("error");
	}

	protected void FlashMax(int most)
	{
		string m = most.ToString();
		string t = INT_QST_Look.Fmt("#STR_INT_QST_ED_E_MAX", m);
		Flash(t, true);
	}

	// --- rebuilds (next Tick) ------------------------------------------------------------------------------------------------
	protected void MarkSection(int sec)
	{
		if (m_RebuildSecs.Find(sec) < 0)
			m_RebuildSecs.Insert(sec);
	}

	protected void MarkBlock(int sec, int index)
	{
		int code = sec * 1000 + index;
		if (m_RebuildBlocks.Find(code) < 0)
			m_RebuildBlocks.Insert(code);
	}

	protected INT_QST_EdColumn ColumnOf(int sec)
	{
		if (sec < 100)
			return m_Left;
		if (sec < 200)
			return m_Mid;
		return m_Right;
	}

	protected void ProcessRebuilds()
	{
		m_TmpSecs.Clear();
		m_TmpBlocks.Clear();
		foreach (int s1 : m_RebuildSecs)
			m_TmpSecs.Insert(s1);
		foreach (int b1 : m_RebuildBlocks)
			m_TmpBlocks.Insert(b1);
		m_RebuildSecs.Clear();
		m_RebuildBlocks.Clear();
		bool doLeft = false;
		bool doMid = false;
		bool doRight = false;
		foreach (int sec : m_TmpSecs)
		{
			RebuildSection(sec);
			if (sec < 100)
				doLeft = true;
			else if (sec < 200)
				doMid = true;
			else
				doRight = true;
		}
		foreach (int code : m_TmpBlocks)
		{
			int bsec = code / 1000;
			int bidx = code % 1000;
			if (m_TmpSecs.Find(bsec) >= 0)
				continue;
			RebuildBlock(bsec, bidx);
			if (bsec < 100)
				doLeft = true;
			else if (bsec < 200)
				doMid = true;
			else
				doRight = true;
		}
		if (doLeft)
			m_Left.Arrange();
		if (doMid)
			m_Mid.Arrange();
		if (doRight)
			m_Right.Arrange();
		if (m_ScrollSec > 0)
		{
			INT_QST_EdColumn col = ColumnOf(m_ScrollSec);
			col.ShowBlock(m_ScrollSec, m_ScrollIdx);
			m_ScrollSec = 0;
		}
	}

	protected void RebuildSection(int sec)
	{
		array<ref INT_QST_EdBlock> fresh = new array<ref INT_QST_EdBlock>();
		if (sec == SEC_L_REPARGS)
		{
			FillRepeatArgs(fresh);
			m_Left.ReplaceSection(sec, fresh);
		}
		else if (sec == SEC_L_REQS)
		{
			FillReqRows(fresh);
			m_Left.ReplaceSection(sec, fresh);
		}
		else if (sec == SEC_M_TGTS)
		{
			FillTargets(fresh);
			m_Mid.ReplaceSection(sec, fresh);
			UpdateTargetCount();
		}
		else if (sec == SEC_R_QIS)
		{
			FillQuestItems(fresh);
			m_Right.ReplaceSection(sec, fresh);
		}
		else if (sec == SEC_R_RWS)
		{
			FillRewards(fresh);
			m_Right.ReplaceSection(sec, fresh);
		}
	}

	protected void RebuildBlock(int sec, int index)
	{
		if (sec == SEC_M_TGTS && index < m_Q.Targets.Count())
		{
			INT_QST_EdBlock tb = BuildTargetCard(index);
			m_Mid.ReplaceBlock(sec, index, tb);
		}
		else if (sec == SEC_R_QIS && index < m_Q.QuestItems.Count())
		{
			INT_QST_EdBlock qb = BuildQuestItem(index);
			m_Right.ReplaceBlock(sec, index, qb);
		}
		else if (sec == SEC_R_RWS && index < m_Q.Rewards.Count())
		{
			INT_QST_EdBlock rb = BuildRewardCard(index);
			m_Right.ReplaceBlock(sec, index, rb);
		}
	}

	// --- input events --------------------------------------------------------------------------------------------------------
	void OnFieldChanged(INT_QST_Field f)
	{
		if (!f || !m_Q || !IsOpen())
			return;
		int tag = f.m_Tag;
		int scope = tag / 10000;
		int idx = (tag % 10000) / 100;
		int prop = tag % 100;
		string v = f.GetText();
		string tv = v + "";
		tv.TrimInPlace();
		MarkDirty();
		if (scope == SC_MAIN)
			ApplyMain(prop, f, v);
		else if (scope == SC_TGT)
			ApplyTarget(idx, prop, f, v, tv);
		else if (scope == SC_QI)
			ApplyItem(idx, prop, f, tv);
		else if (scope == SC_RW)
			ApplyReward(idx, prop, f, v, tv);
	}

	protected void ApplyMain(int prop, INT_QST_Field f, string v)
	{
		int num = f.GetInt();
		if (prop == P_NAME)
		{
			m_Q.Name = v;
		}
		else if (prop == P_BRANCH)
		{
			string key = INT_QST_QuestEditor.SanitizeKey(v);
			if (key != v)
				f.SetText(key);
			m_BranchKey = key;
			UpdateBranchInfo();
		}
		else if (prop == P_ORDER)
		{
			m_Q.Order = num;
		}
		else if (prop == P_CD_H || prop == P_CD_M)
		{
			bool cdHours = false;
			if (prop == P_CD_H)
				cdHours = true;
			int cd = INT_QST_QuestEditor.WithHM(m_Q.Cooldown, cdHours, num);
			m_Q.Cooldown = cd;
		}
		else if (prop == P_RS_H)
		{
			int rh = INT_QST_Util.ClampI(num, 0, 23);
			m_Q.ResetHour = rh;
		}
		else if (prop == P_RS_M)
		{
			int rm = INT_QST_Util.ClampI(num, 0, 59);
			m_Q.ResetMinute = rm;
		}
		else if (prop == P_TL_H || prop == P_TL_M)
		{
			bool tlHours = false;
			if (prop == P_TL_H)
				tlHours = true;
			int tl = INT_QST_QuestEditor.WithHM(m_Q.TimeLimit, tlHours, num);
			m_Q.TimeLimit = tl;
		}
		else if (prop == P_NOTE)
		{
			m_Q.Note = v;
		}
	}

	protected void ApplyTarget(int idx, int prop, INT_QST_Field f, string v, string tv)
	{
		if (idx < 0 || idx >= m_Q.Targets.Count())
			return;
		INT_QST_TargetDef t = m_Q.Targets[idx];
		if (prop == T_TARGET)
		{
			t.Target = tv;
		}
		else if (prop == T_COUNT)
		{
			int cnt = f.GetInt();
			t.Count = cnt;
		}
		else if (prop == T_FILTER)
		{
			t.Filter = tv;
		}
		else if (prop == T_MINQ)
		{
			int raw = f.GetInt();
			int mq = INT_QST_Util.ClampI(raw, 0, 100);
			t.MinQuantity = mq;
		}
		else if (prop == T_POS)
		{
			t.Position = tv;
		}
		else if (prop == T_RADIUS)
		{
			float rad = f.GetFloat();
			if (rad != rad || rad < 0)
				rad = 0;
			t.Radius = rad;
		}
		else if (prop == T_ZONEC)
		{
			t.ZoneCenter = tv;
		}
		else if (prop == T_ZONER)
		{
			float zr = f.GetFloat();
			if (zr != zr || zr < 0)
				zr = 0;
			t.ZoneRadius = zr;
		}
		else if (prop == T_TEXT)
		{
			t.Text = v;
		}
	}

	protected void ApplyItem(int idx, int prop, INT_QST_Field f, string tv)
	{
		if (idx < 0 || idx >= m_Q.QuestItems.Count())
			return;
		INT_QST_ItemRef it = m_Q.QuestItems[idx];
		if (prop == Q_ITEM)
		{
			it.Item = tv;
		}
		else if (prop == Q_COUNT)
		{
			int cnt = f.GetInt();
			it.Count = cnt;
		}
	}

	protected void ApplyReward(int idx, int prop, INT_QST_Field f, string v, string tv)
	{
		if (idx < 0 || idx >= m_Q.Rewards.Count())
			return;
		INT_QST_RewardDef r = m_Q.Rewards[idx];
		if (prop == R_ITEM)
		{
			r.Item = tv;
		}
		else if (prop == R_COUNT)
		{
			int cnt = f.GetInt();
			r.Count = cnt;
		}
		else if (prop == R_QTY)
		{
			r.Quantity = tv;
		}
		else if (prop == R_AMOUNT)
		{
			int am = f.GetInt();
			if (am < 0)
				am = 0;
			r.Amount = am;
		}
		else if (prop == R_TEXT)
		{
			r.Text = v;
		}
	}

	void OnChipsChanged(INT_QST_EdChips c, int index)
	{
		if (!c || !m_Q || !IsOpen())
			return;
		int tag = c.m_Tag;
		int scope = tag / 10000;
		int idx = (tag % 10000) / 100;
		int prop = tag % 100;
		MarkDirty();
		if (scope == SC_MAIN)
		{
			if (prop == P_REPEAT)
			{
				m_Repeat = index;
				string rk = INT_QST_RM.ToKey(index);
				m_Q.RepeatMode = rk;
				MarkSection(SEC_L_REPARGS);
			}
			else if (prop == P_WEEKDAY)
			{
				int wd = index + 1;
				m_Q.ResetWeekDay = wd;
			}
			else if (prop == P_REQANY)
			{
				m_Q.RequireAny = index;
			}
			return;
		}
		if (scope == SC_TGT)
		{
			if (idx < 0 || idx >= m_Q.Targets.Count())
				return;
			INT_QST_TargetDef t = m_Q.Targets[idx];
			if (prop == T_KIND)
			{
				string kk = INT_QST_TT.ToKey(index + 1);
				t.Kind = kk;
				MarkBlock(SEC_M_TGTS, idx);
			}
			else if (prop == T_WORST)
			{
				string wk = INT_QST_State.KeyFromLevel(index - 1);
				t.WorstState = wk;
			}
			return;
		}
		if (scope == SC_RW)
		{
			if (idx < 0 || idx >= m_Q.Rewards.Count())
				return;
			INT_QST_RewardDef r = m_Q.Rewards[idx];
			if (prop == R_KIND)
			{
				string rkind = "ITEM";
				if (index == 1)
					rkind = "MONEY";
				r.Kind = rkind;
				MarkBlock(SEC_R_RWS, idx);
			}
			else if (prop == R_STATE)
			{
				string sk = INT_QST_State.KeyFromLevel(index - 1);
				r.State = sk;
			}
		}
	}

	void OnCheckChanged(INT_QST_Check c, bool on)
	{
		if (!c || !m_Q || !IsOpen())
			return;
		int tag = c.m_Tag;
		int scope = tag / 10000;
		int idx = (tag % 10000) / 100;
		int prop = tag % 100;
		int v = 0;
		if (on)
			v = 1;
		MarkDirty();
		if (scope == SC_MAIN && prop == P_ENABLED)
		{
			m_Q.Enabled = v;
			return;
		}
		if (scope != SC_TGT || idx < 0 || idx >= m_Q.Targets.Count())
			return;
		INT_QST_TargetDef t = m_Q.Targets[idx];
		if (prop == T_GROUND)
			t.GroundY = v;
		else if (prop == T_MAP)
			t.ShowOnMap = v;
	}

	void OnDescChanged(INT_QST_Area a)
	{
		if (!a || !IsOpen())
			return;
		string txt = a.GetText();
		m_DescText = txt;
		UpdateDescBytes();
		MarkDirty();
	}

	void OnClickId(int id, int data, int button)
	{
		if (button != MouseState.LEFT || !IsOpen())
			return;
		switch (id)
		{
			case ID_BG:
				return;
			case ID_CLOSE:
			case ID_CANCEL:
				RequestClose();
				return;
			case ID_SAVE:
				TrySave();
				return;
			case ID_DELETE:
				AskDelete();
				return;
			case ID_ASK_YES:
				Answer(true);
				return;
			case ID_ASK_NO:
				Answer(false);
				return;
			case ID_BRANCH_LIST:
				OpenBranchPopup();
				return;
			case ID_GIVER:
				OpenGiverPopup(data == 0);
				return;
			case ID_REQ_ADD:
				OpenReqPopup();
				return;
			case ID_REQ_DEL:
				RemoveRequire(data);
				return;
			case ID_TGT_ADD:
				AddTarget();
				return;
			case ID_TGT_DEL:
				RemoveTarget(data);
				return;
			case ID_TGT_UP:
				MoveTarget(data, -1);
				return;
			case ID_TGT_DOWN:
				MoveTarget(data, 1);
				return;
			case ID_PICK:
				OpenPicker(data);
				return;
			case ID_ACTIONS:
				OpenActionPopup(data);
				return;
			case ID_HERE:
				PutHere(data);
				return;
			case ID_QI_ADD:
				AddQuestItem();
				return;
			case ID_QI_DEL:
				RemoveQuestItem(data);
				return;
			case ID_RW_ADD:
				AddReward(data);
				return;
			case ID_RW_DEL:
				RemoveReward(data);
				return;
		}
	}

	// Wheel over a column scrolls it by INT_QST_EdColumn.STEP px (routed by the mouse position: inputs take the wheel themselves).
	// Stand test: the wheel over a column (0 left, 1 targets, 2 right) without a mouse.
	void TestWheel(int column, int wheel)
	{
		INT_QST_EdColumn col = m_Left;
		if (column == 1)
			col = m_Mid;
		else if (column == 2)
			col = m_Right;
		if (col)
			col.ScrollBy(wheel);
	}

	void OnWheelId(int id, int wheel)
	{
		if (!IsOpen() || m_Ask)
			return;
		if (m_Picker.IsOpen() || m_Popup.IsOpen())
			return;
		int mx;
		int my;
		GetMousePos(mx, my);
		INT_QST_EdColumn col = null;
		if (m_Left && m_Left.ContainsMouse(mx, my))
			col = m_Left;
		else if (m_Mid && m_Mid.ContainsMouse(mx, my))
			col = m_Mid;
		else if (m_Right && m_Right.ContainsMouse(mx, my))
			col = m_Right;
		if (!col)
			return;
		if (col.HasFocusInside())
			SetFocus(null);
		col.ScrollBy(wheel);
	}

	// --- list changes ----------------------------------------------------------------------------------------------------------
	protected void AddTarget()
	{
		if (m_Q.Targets.Count() >= INT_QST_Const.MAX_TARGETS)
		{
			FlashMax(INT_QST_Const.MAX_TARGETS);
			return;
		}
		INT_QST_TargetDef t = new INT_QST_TargetDef();
		t.Kind = "KILL";
		t.Count = 1;
		m_Q.Targets.Insert(t);
		m_TgtFrom.Insert(-1);
		m_ScrollSec = SEC_M_TGTS;
		m_ScrollIdx = m_Q.Targets.Count() - 1;
		MarkSection(SEC_M_TGTS);
		MarkDirty();
		INT_QST_Sfx.Play("click");
	}

	protected void RemoveTarget(int i)
	{
		if (i < 0 || i >= m_Q.Targets.Count())
			return;
		m_Q.Targets.RemoveOrdered(i);
		if (i < m_TgtFrom.Count())
			m_TgtFrom.RemoveOrdered(i);
		MarkSection(SEC_M_TGTS);
		MarkDirty();
		INT_QST_Sfx.Play("click");
	}

	protected void MoveTarget(int i, int dir)
	{
		int j = i + dir;
		int n = m_Q.Targets.Count();
		if (i < 0 || i >= n || j < 0 || j >= n)
			return;
		INT_QST_TargetDef ta = m_Q.Targets[i];
		INT_QST_TargetDef tb = m_Q.Targets[j];
		m_Q.Targets.Set(i, tb);
		m_Q.Targets.Set(j, ta);
		if (i < m_TgtFrom.Count() && j < m_TgtFrom.Count())
		{
			int fa = m_TgtFrom[i];
			int fb = m_TgtFrom[j];
			m_TgtFrom.Set(i, fb);
			m_TgtFrom.Set(j, fa);
		}
		m_ScrollSec = SEC_M_TGTS;
		m_ScrollIdx = j;
		MarkSection(SEC_M_TGTS);
		MarkDirty();
		INT_QST_Sfx.Play("click");
	}

	protected void AddQuestItem()
	{
		if (m_Q.QuestItems.Count() >= INT_QST_Const.MAX_QUEST_ITEMS)
		{
			FlashMax(INT_QST_Const.MAX_QUEST_ITEMS);
			return;
		}
		INT_QST_ItemRef it = new INT_QST_ItemRef();
		it.Count = 1;
		m_Q.QuestItems.Insert(it);
		m_QiFrom.Insert(-1);
		m_ScrollSec = SEC_R_QIS;
		m_ScrollIdx = m_Q.QuestItems.Count() - 1;
		MarkSection(SEC_R_QIS);
		MarkDirty();
		INT_QST_Sfx.Play("click");
	}

	protected void RemoveQuestItem(int i)
	{
		if (i < 0 || i >= m_Q.QuestItems.Count())
			return;
		m_Q.QuestItems.RemoveOrdered(i);
		if (i < m_QiFrom.Count())
			m_QiFrom.RemoveOrdered(i);
		MarkSection(SEC_R_QIS);
		MarkDirty();
		INT_QST_Sfx.Play("click");
	}

	protected void AddReward(int kind)
	{
		if (m_Q.Rewards.Count() >= INT_QST_Const.MAX_REWARDS)
		{
			FlashMax(INT_QST_Const.MAX_REWARDS);
			return;
		}
		INT_QST_RewardDef r = new INT_QST_RewardDef();
		r.Kind = "ITEM";
		if (kind == INT_QST_RT.MONEY)
			r.Kind = "MONEY";
		r.Count = 1;
		m_Q.Rewards.Insert(r);
		m_RwFrom.Insert(-1);
		m_ScrollSec = SEC_R_RWS;
		m_ScrollIdx = m_Q.Rewards.Count() - 1;
		MarkSection(SEC_R_RWS);
		MarkDirty();
		INT_QST_Sfx.Play("click");
	}

	protected void RemoveReward(int i)
	{
		if (i < 0 || i >= m_Q.Rewards.Count())
			return;
		m_Q.Rewards.RemoveOrdered(i);
		if (i < m_RwFrom.Count())
			m_RwFrom.RemoveOrdered(i);
		MarkSection(SEC_R_RWS);
		MarkDirty();
		INT_QST_Sfx.Play("click");
	}

	protected void RemoveRequire(int i)
	{
		if (i < 0 || i >= m_Q.Requires.Count())
			return;
		m_Q.Requires.RemoveOrdered(i);
		MarkSection(SEC_L_REQS);
		MarkDirty();
		INT_QST_Sfx.Play("click");
	}

	// "My position" into a point field ("x y z" of the player).
	protected void PutHere(int tag)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;
		vector pos = p.GetPosition();
		string txt = INT_QST_Util.VecText(pos);
		int idx = (tag % 10000) / 100;
		int prop = tag % 100;
		if (idx < 0 || idx >= m_Q.Targets.Count())
			return;
		INT_QST_TargetDef t = m_Q.Targets[idx];
		if (prop == T_POS)
			t.Position = txt;
		else if (prop == T_ZONEC)
			t.ZoneCenter = txt;
		MarkBlock(SEC_M_TGTS, idx);
		MarkDirty();
		INT_QST_Sfx.Play("click");
	}

	// --- class picker -----------------------------------------------------------------------------------------------------------
	protected void OpenPicker(int tag)
	{
		int scope = tag / 10000;
		int idx = (tag % 10000) / 100;
		int prop = tag % 100;
		int kind = INT_QST_ClassPicker.K_ITEM;
		string cur = "";
		if (scope == SC_TGT)
		{
			if (idx < 0 || idx >= m_Q.Targets.Count())
				return;
			INT_QST_TargetDef t = m_Q.Targets[idx];
			int tk = INT_QST_TT.FromString(t.Kind);
			if (prop == T_TARGET)
			{
				cur = t.Target;
				if (tk == INT_QST_TT.KILL)
					kind = INT_QST_ClassPicker.K_CREATURE;
			}
			else if (prop == T_FILTER)
			{
				cur = t.Filter;
				if (tk == INT_QST_TT.ACTION)
					kind = INT_QST_ClassPicker.K_ANY;
			}
		}
		else if (scope == SC_QI)
		{
			if (idx < 0 || idx >= m_Q.QuestItems.Count())
				return;
			INT_QST_ItemRef it = m_Q.QuestItems[idx];
			cur = it.Item;
		}
		else if (scope == SC_RW)
		{
			if (idx < 0 || idx >= m_Q.Rewards.Count())
				return;
			INT_QST_RewardDef r = m_Q.Rewards[idx];
			cur = r.Item;
		}
		m_PickTag = tag;
		m_Picker.Open(m_Root, m_W, m_H, kind, cur);
	}

	void OnPicked(string cls)
	{
		if (!IsOpen() || !m_Q)
			return;
		int tag = m_PickTag;
		int scope = tag / 10000;
		int idx = (tag % 10000) / 100;
		int prop = tag % 100;
		if (scope == SC_TGT)
		{
			if (idx < 0 || idx >= m_Q.Targets.Count())
				return;
			INT_QST_TargetDef t = m_Q.Targets[idx];
			if (prop == T_TARGET)
				t.Target = cls;
			else if (prop == T_FILTER)
				t.Filter = cls;
			MarkBlock(SEC_M_TGTS, idx);
		}
		else if (scope == SC_QI)
		{
			if (idx < 0 || idx >= m_Q.QuestItems.Count())
				return;
			INT_QST_ItemRef it = m_Q.QuestItems[idx];
			it.Item = cls;
			MarkBlock(SEC_R_QIS, idx);
		}
		else if (scope == SC_RW)
		{
			if (idx < 0 || idx >= m_Q.Rewards.Count())
				return;
			INT_QST_RewardDef r = m_Q.Rewards[idx];
			r.Item = cls;
			MarkBlock(SEC_R_RWS, idx);
		}
		MarkDirty();
	}

	// --- list choosers ----------------------------------------------------------------------------------------------------------
	protected static void AddRow(array<ref INT_QST_EdPopRow> rows, string title, string sub, string value)
	{
		INT_QST_EdPopRow r = new INT_QST_EdPopRow();
		r.m_Title = title;
		r.m_Sub = sub;
		r.m_Value = value;
		string find = INT_QST_PickIndex.Fold(title + " " + sub);
		r.m_Find = find;
		rows.Insert(r);
	}

	protected void OpenBranchPopup()
	{
		array<ref INT_QST_EdPopRow> rows = new array<ref INT_QST_EdPopRow>();
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		map<string, int> counts = new map<string, int>();
		foreach (INT_QST_QuestDef q : cat.m_List)
		{
			int had = 0;
			counts.Find(q.m_Branch, had);
			counts.Set(q.m_Branch, had + 1);
		}
		foreach (string key : cat.m_BranchOrder)
		{
			int qn = 0;
			counts.Find(key, qn);
			string qnText = qn.ToString();
			string nq = INT_QST_Look.Fmt("#STR_INT_QST_ED_N_QUESTS", qnText);
			string sub = key + "   ·   " + nq;
			string title = cat.BranchTitle(key);
			INT_QST_QuestEditor.AddRow(rows, title, sub, key);
		}
		m_PopMode = POP_BRANCH;
		m_Popup.Open(m_Root, m_W, m_H, "#STR_INT_QST_ED_POP_BRANCH", rows, m_BranchKey);
	}

	protected void OpenGiverPopup(bool start)
	{
		array<ref INT_QST_EdPopRow> rows = new array<ref INT_QST_EdPopRow>();
		string noneText = INT_QST_Look.Tr("#STR_INT_QST_ED_G_NONE_FINISH");
		if (start)
			noneText = INT_QST_Look.Tr("#STR_INT_QST_ED_G_NONE_START");
		INT_QST_QuestEditor.AddRow(rows, noneText, "", "");
		string npc = INT_QST_Look.Tr("#STR_INT_QST_ED_G_NPC");
		string obj = INT_QST_Look.Tr("#STR_INT_QST_ED_G_OBJECT");
		string offText = INT_QST_Look.Tr("#STR_INT_QST_ED_G_OFF");
		foreach (INT_QST_GiverView g : INT_QST_Client.s_Givers)
		{
			if (!g)
				continue;
			string gt = g.Title;
			if (gt == "")
				gt = g.Key;
			string kindText = npc;
			if (g.Kind == 1)
				kindText = obj;
			string sub = g.Key + "   ·   " + kindText;
			if (g.Disabled)
				sub = sub + "   ·   " + offText;
			INT_QST_QuestEditor.AddRow(rows, gt, sub, g.Key);
		}
		string cur = m_Q.GiverStart;
		m_PopMode = POP_GSTART;
		if (!start)
		{
			cur = m_Q.GiverFinish;
			m_PopMode = POP_GFINISH;
		}
		m_Popup.Open(m_Root, m_W, m_H, "#STR_INT_QST_ED_POP_GIVER", rows, cur);
	}

	protected void OpenReqPopup()
	{
		if (m_Q.Requires.Count() >= INT_QST_Const.MAX_REQUIRES)
		{
			FlashMax(INT_QST_Const.MAX_REQUIRES);
			return;
		}
		array<ref INT_QST_EdPopRow> rows = new array<ref INT_QST_EdPopRow>();
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		foreach (INT_QST_QuestDef q : cat.m_List)
		{
			if (q.Id == m_Q.Id || m_Q.Requires.Find(q.Id) >= 0)
				continue;
			string idText = q.Id.ToString();
			string bt = cat.BranchTitle(q.m_Branch);
			string sub = "#" + idText + "   ·   " + bt;
			INT_QST_QuestEditor.AddRow(rows, q.Name, sub, idText);
		}
		m_PopMode = POP_REQ;
		m_Popup.Open(m_Root, m_W, m_H, "#STR_INT_QST_ED_POP_REQ", rows, "");
	}

	protected void OpenActionPopup(int ti)
	{
		if (ti < 0 || ti >= m_Q.Targets.Count())
			return;
		INT_QST_TargetDef t = m_Q.Targets[ti];
		array<string> list = new array<string>();
		INT_QST_QuestEditor.CommonActions(list);
		array<ref INT_QST_EdPopRow> rows = new array<ref INT_QST_EdPopRow>();
		foreach (string cls : list)
		{
			string an = INT_QST_Text.ActionName(cls);
			INT_QST_QuestEditor.AddRow(rows, an, cls, cls);
		}
		m_PopMode = POP_ACTION;
		m_PopIndex = ti;
		m_Popup.Open(m_Root, m_W, m_H, "#STR_INT_QST_ED_POP_ACTION", rows, t.Target);
	}

	void OnPopupPick(string value)
	{
		if (!IsOpen() || !m_Q)
			return;
		if (m_PopMode == POP_BRANCH)
		{
			m_BranchKey = value;
			if (m_BranchField)
				m_BranchField.SetText(value);
			UpdateBranchInfo();
		}
		else if (m_PopMode == POP_GSTART)
		{
			m_Q.GiverStart = value;
			string ls = GiverLabel(value, true);
			SetSelect(m_GStartText, ls);
		}
		else if (m_PopMode == POP_GFINISH)
		{
			m_Q.GiverFinish = value;
			string lf = GiverLabel(value, false);
			SetSelect(m_GFinishText, lf);
		}
		else if (m_PopMode == POP_REQ)
		{
			int rid = value.ToInt();
			if (rid == 0 || m_Q.Requires.Find(rid) >= 0 || m_Q.Requires.Count() >= INT_QST_Const.MAX_REQUIRES)
				return;
			// a requirement that already needs this quest would close a loop (same check as the board's link mode)
			if (INT_QST_Board.MakesLoop(m_Q.Id, rid))
			{
				string loop = INT_QST_Look.Tr("#STR_INT_QST_B_LINK_LOOP");
				Flash(loop, true);
				return;
			}
			m_Q.Requires.Insert(rid);
			m_ScrollSec = SEC_L_REQS;
			m_ScrollIdx = m_Q.Requires.Count() - 1;
			MarkSection(SEC_L_REQS);
		}
		else if (m_PopMode == POP_ACTION)
		{
			if (m_PopIndex < 0 || m_PopIndex >= m_Q.Targets.Count())
				return;
			INT_QST_TargetDef t = m_Q.Targets[m_PopIndex];
			t.Target = value;
			MarkBlock(SEC_M_TGTS, m_PopIndex);
		}
		MarkDirty();
	}

	// --- question card -----------------------------------------------------------------------------------------------------------
	protected void AskQ(int tag, string titleKey, string text, string yesKey, bool danger)
	{
		CloseAsk();
		m_AskTag = tag;
		Widget a = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, m_H, "Ask");
		m_Ask = a;
		INT_UIF_Surface.Box(a, 4, 4, m_W - 8, m_H - 8, 14, 0x9E0D0905, 0);
		ButtonWidget bg = INT_UIF_Draw.Hit(a, 0, 0, m_W, m_H, m_Clicks);
		m_Clicks.Bind(bg, ID_BG);
		array<string> lines = new array<string>();
		INT_QST_Text.Wrap(text, 470, 16, lines);
		float cw = 560;
		float ch = 150 + lines.Count() * 25 + 70;
		float cx = (m_W - cw) * 0.5;
		float cy = (m_H - ch) * 0.5;
		Widget card = INT_UIF_Draw.Frame(a, cx, cy, cw, ch, "Card");
		INT_UIF_Surface.Shadow(card, -30, -20, cw + 60, ch + 60, 0x8C000000);
		INT_QST_Look.PaperSheet(card, 0, 0, cw, ch, INT_QST_Look.Note(), "Paper");
		INT_QST_Look.PinHead(card, cw * 0.5 - 13, -12);
		string t = INT_QST_Look.Tr(titleKey);
		int ink = INT_QST_Look.Ink();
		INT_QST_Look.Heavy(card, 40, 30, cw - 80, 34, t, 22, ink);
		INT_QST_Look.DoubleRule(card, 40, 70, cw - 80);
		float ly = 88;
		int blue = INT_QST_Look.Blue();
		foreach (string ln : lines)
		{
			INT_QST_Look.Text(card, 44, ly, cw - 88, 24, ln, 16, blue);
			ly = ly + 25;
		}
		int yesStyle = INT_QST_Look.BTN_INK;
		int yesIc = INT_QST_Look.Sticky();
		if (danger)
		{
			yesStyle = INT_QST_Look.BTN_DANGER;
			yesIc = INT_QST_Look.Red();
		}
		float by = ch - 76;
		TextButton(card, 40, by, 200, 48, "#STR_INT_QST_B_BACK", "", INT_QST_Look.BTN_GHOST, 0, ID_ASK_NO, 0);
		TextButton(card, cw - 40 - 260, by, 260, 48, yesKey, "check", yesStyle, yesIc, ID_ASK_YES, 0);
		INT_UIF_Animator.FadeIn(a, 0.16);
		INT_QST_Sfx.Play("page");
	}

	// Answered from its own buttons: hidden now, unlinked on the next Tick.
	protected void CloseAsk()
	{
		if (!m_Ask)
			return;
		m_Clicks.ForgetUnder(m_Ask);
		m_Ask.Show(false);
		m_Dead.Insert(m_Ask);
		m_Ask = null;
	}

	protected void KillDead()
	{
		if (m_Dead.Count() == 0)
			return;
		foreach (Widget dw : m_Dead)
		{
			if (dw)
				dw.Unlink();
		}
		m_Dead.Clear();
	}

	protected void Answer(bool yes)
	{
		int tag = m_AskTag;
		CloseAsk();
		INT_QST_Sfx.Play("click");
		if (!yes)
			return;
		if (tag == ASK_CLOSE)
		{
			m_Dirty = false;
			Close();
			return;
		}
		if (tag == ASK_DELETE && m_Q)
		{
			int qid = m_Q.Id;
			m_Dirty = false;
			Event_Delete.Invoke(qid);
			Close();
		}
	}

	protected void RequestClose()
	{
		if (m_Dirty)
		{
			string text = INT_QST_Look.Tr("#STR_INT_QST_ED_ASK_CLOSE_TEXT");
			AskQ(ASK_CLOSE, "#STR_INT_QST_ED_ASK_CLOSE", text, "#STR_INT_QST_ED_B_CLOSE", true);
			return;
		}
		Close();
		INT_QST_Sfx.Play("close");
	}

	protected void AskDelete()
	{
		if (m_IsNew || !m_Q)
			return;
		string text = INT_QST_Look.Fmt("#STR_INT_QST_ED_ASK_DELETE_TEXT", m_Q.Name);
		AskQ(ASK_DELETE, "#STR_INT_QST_ED_ASK_DELETE", text, "#STR_INT_QST_ED_B_DELETE", true);
	}

	// --- save ----------------------------------------------------------------------------------------------------------------------
	protected void Problem(array<string> problems, string key, string a1)
	{
		string p = INT_QST_Look.Fmt(key, a1);
		problems.Insert(p);
	}

	protected void Problem2(array<string> problems, string key, string a1, string a2)
	{
		string p = INT_QST_Look.Fmt(key, a1, a2);
		problems.Insert(p);
	}

	// An existing branch whose key differs only in letter case ("" = none): the server matches keys
	// case-insensitively, a second folder would split the branch.
	protected string CaseTwin(string key)
	{
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		if (cat.Branch(key))
			return "";
		string low = INT_QST_Util.Lower(key);
		foreach (string bk : cat.m_BranchOrder)
		{
			string bl = INT_QST_Util.Lower(bk);
			if (bl == low)
				return bk;
		}
		return "";
	}

	protected void Validate(array<string> problems)
	{
		string nm = INT_QST_Util.OneLine(m_Q.Name, INT_QST_Const.MAX_NAME_BYTES);
		if (nm == "")
			Problem(problems, "#STR_INT_QST_ED_E_NAME", "");
		if (!INT_QST_QuestEditor.ValidKey(m_BranchKey))
		{
			Problem(problems, "#STR_INT_QST_ED_E_BRANCH", "");
		}
		else if (m_BranchKey.Get(0) == "_")
		{
			// the server loader skips folders that start with "_" (_deleted ...)
			Problem(problems, "#STR_INT_QST_FA_E_BRANCH_UNDERSCORE", "");
		}
		else
		{
			string twin = CaseTwin(m_BranchKey);
			if (twin != "")
				Problem(problems, "#STR_INT_QST_FA_E_BRANCH_CASE", twin);
		}
		// limits the server applies on save (Normalize): refused here, so nothing changes silently
		if (m_Q.TimeLimit > INT_QST_Const.MAX_TIME_LIMIT)
		{
			int tlh = INT_QST_Const.MAX_TIME_LIMIT / 3600;
			string tlText = tlh.ToString();
			Problem(problems, "#STR_INT_QST_FA_E_LIMIT_LONG", tlText);
		}
		if (m_Repeat == INT_QST_RM.COOLDOWN && m_Q.Cooldown > INT_QST_Const.MAX_COOLDOWN)
		{
			int cdh = INT_QST_Const.MAX_COOLDOWN / 3600;
			string cdText = cdh.ToString();
			Problem(problems, "#STR_INT_QST_FA_E_CD_LONG", cdText);
		}
		foreach (int lr : m_Q.Requires)
		{
			if (lr == m_Q.Id || !INT_QST_Board.MakesLoop(m_Q.Id, lr))
				continue;
			string lrText = lr.ToString();
			Problem(problems, "#STR_INT_QST_FA_E_REQ_LOOP", lrText);
		}
		int nt = m_Q.Targets.Count();
		if (nt == 0 && m_Q.GiverFinish == "")
			Problem(problems, "#STR_INT_QST_ED_E_NO_TARGETS", "");
		for (int i = 0; i < nt; i++)
			CheckTarget(i, problems);
		if (m_Repeat == INT_QST_RM.COOLDOWN && m_Q.Cooldown <= 0)
			Problem(problems, "#STR_INT_QST_ED_E_COOLDOWN", "");
		if (m_Q.Requires.Find(m_Q.Id) >= 0)
			Problem(problems, "#STR_INT_QST_ED_E_SELF_REQ", "");
		int itemMost = INT_QST_Const.MAX_ITEM_COUNT;
		string itemMostText = itemMost.ToString();
		int ni = m_Q.QuestItems.Count();
		for (int k = 0; k < ni; k++)
		{
			INT_QST_ItemRef it = m_Q.QuestItems[k];
			int qn = k + 1;
			string qnText = qn.ToString();
			if (it.Count < 1 || it.Count > INT_QST_Const.MAX_ITEM_COUNT)
				Problem2(problems, "#STR_INT_QST_FA_E_QI_COUNT", qnText, itemMostText);
			string item = it.Item + "";
			item.TrimInPlace();
			if (item != "")
				continue;
			Problem(problems, "#STR_INT_QST_ED_E_QI", qnText);
		}
		int nr = m_Q.Rewards.Count();
		for (int r = 0; r < nr; r++)
		{
			INT_QST_RewardDef rd = m_Q.Rewards[r];
			int rn = r + 1;
			string rnText = rn.ToString();
			int rk = INT_QST_RT.FromString(rd.Kind);
			string ritem = rd.Item + "";
			ritem.TrimInPlace();
			if (rk == INT_QST_RT.NONE)
				Problem(problems, "#STR_INT_QST_ED_E_RW_KIND", rnText);
			else if (rk == INT_QST_RT.ITEM && ritem == "")
				Problem(problems, "#STR_INT_QST_ED_E_RW_ITEM", rnText);
			else if (rk == INT_QST_RT.MONEY && rd.Amount <= 0)
				Problem(problems, "#STR_INT_QST_ED_E_RW_MONEY", rnText);
			else if (rk == INT_QST_RT.ITEM && (rd.Count < 1 || rd.Count > INT_QST_Const.MAX_ITEM_COUNT))
				Problem2(problems, "#STR_INT_QST_FA_E_RW_COUNT", rnText, itemMostText);
		}
		if (m_DescText.Length() > INT_QST_Const.MAX_TEXT_BYTES)
		{
			string most = INT_QST_Const.MAX_TEXT_BYTES.ToString();
			Problem(problems, "#STR_INT_QST_ED_E_TEXT_LONG", most);
		}
	}

	protected void CheckTarget(int i, array<string> problems)
	{
		INT_QST_TargetDef t = m_Q.Targets[i];
		int no = i + 1;
		string noText = no.ToString();
		int k = INT_QST_TT.FromString(t.Kind);
		if (k == INT_QST_TT.NONE)
		{
			Problem(problems, "#STR_INT_QST_ED_E_T_KIND", noText);
			return;
		}
		if (k == INT_QST_TT.EXPLORE)
		{
			vector pv;
			if (!INT_QST_Util.ParseVec(t.Position, pv))
				Problem(problems, "#STR_INT_QST_ED_E_T_POS", noText);
			return;
		}
		// 0 is read as 1 (old files), the server cuts above MAX_TARGET_COUNT
		int need = (int)Math.Round(t.Count);
		if (need < 0 || need > INT_QST_Const.MAX_TARGET_COUNT)
		{
			int most = INT_QST_Const.MAX_TARGET_COUNT;
			string mostText = most.ToString();
			Problem2(problems, "#STR_INT_QST_FA_E_T_COUNT", noText, mostText);
		}
		string cls = t.Target + "";
		cls.TrimInPlace();
		if (cls == "")
		{
			if (k == INT_QST_TT.ACTION)
				Problem(problems, "#STR_INT_QST_ED_E_T_ACTION", noText);
			else
				Problem(problems, "#STR_INT_QST_ED_E_T_CLASS", noText);
		}
		if (k != INT_QST_TT.KILL)
			return;
		string zc = t.ZoneCenter + "";
		zc.TrimInPlace();
		if (zc == "" && t.ZoneRadius <= 0)
			return;
		vector zv;
		if (!INT_QST_Util.ParseVec(zc, zv))
			Problem(problems, "#STR_INT_QST_ED_E_T_ZONE", noText);
		else if (t.ZoneRadius <= 0)
			Problem(problems, "#STR_INT_QST_ED_E_T_ZONE_R", noText);
	}

	// Unused fields of a kind go back to their JSON defaults, kind keys are written in capitals.
	protected void CleanTarget(INT_QST_TargetDef t)
	{
		int k = INT_QST_TT.FromString(t.Kind);
		string kk = INT_QST_TT.ToKey(k);
		t.Kind = kk;
		string tt = t.Target + "";
		tt.TrimInPlace();
		t.Target = tt;
		string tf = t.Filter + "";
		tf.TrimInPlace();
		t.Filter = tf;
		string tp = t.Position + "";
		tp.TrimInPlace();
		t.Position = tp;
		string tz = t.ZoneCenter + "";
		tz.TrimInPlace();
		t.ZoneCenter = tz;
		if (k != INT_QST_TT.KILL && k != INT_QST_TT.ACTION)
			t.Filter = "";
		if (k != INT_QST_TT.KILL)
		{
			t.ZoneCenter = "";
			t.ZoneRadius = 0;
		}
		if (k != INT_QST_TT.DELIVER && k != INT_QST_TT.SEARCH)
		{
			t.WorstState = "";
			t.MinQuantity = 0;
		}
		if (k != INT_QST_TT.EXPLORE)
		{
			t.Position = "";
			t.Radius = 0;
			t.GroundY = 0;
			t.ShowOnMap = 0;
		}
		else
		{
			t.Target = "";
		}
	}

	protected void CleanReward(INT_QST_RewardDef r)
	{
		int k = INT_QST_RT.FromString(r.Kind);
		if (k == INT_QST_RT.MONEY)
		{
			r.Kind = "MONEY";
			r.Item = "";
			r.Quantity = "";
			r.State = "";
			return;
		}
		r.Kind = "ITEM";
		r.Amount = 0;
		string ri = r.Item + "";
		ri.TrimInPlace();
		r.Item = ri;
		string rq = r.Quantity + "";
		rq.TrimInPlace();
		r.Quantity = rq;
	}

	// --- unchanged parts (written back exactly as they came) ---------------------------------------------------------------
	static bool SameTarget(INT_QST_TargetDef a, INT_QST_TargetDef b)
	{
		if (!a || !b)
			return false;
		if (a.Kind != b.Kind || a.Target != b.Target || a.Count != b.Count || a.Filter != b.Filter)
			return false;
		if (a.WorstState != b.WorstState || a.MinQuantity != b.MinQuantity || a.Position != b.Position)
			return false;
		if (a.Radius != b.Radius || a.GroundY != b.GroundY || a.ShowOnMap != b.ShowOnMap)
			return false;
		if (a.ZoneCenter != b.ZoneCenter || a.ZoneRadius != b.ZoneRadius || a.Text != b.Text)
			return false;
		return true;
	}

	static bool SameReward(INT_QST_RewardDef a, INT_QST_RewardDef b)
	{
		if (!a || !b)
			return false;
		if (a.Kind != b.Kind || a.Item != b.Item || a.Count != b.Count || a.Quantity != b.Quantity)
			return false;
		if (a.State != b.State || a.Amount != b.Amount || a.Text != b.Text)
			return false;
		return true;
	}

	static bool SameItem(INT_QST_ItemRef a, INT_QST_ItemRef b)
	{
		if (!a || !b)
			return false;
		return a.Item == b.Item && a.Count == b.Count;
	}

	static bool SameInts(array<int> a, array<int> b)
	{
		if (!a || !b || a.Count() != b.Count())
			return false;
		int n = a.Count();
		for (int i = 0; i < n; i++)
		{
			if (a[i] != b[i])
				return false;
		}
		return true;
	}

	// Row i of the edited list came from row `from` of the opened quest and was not changed.
	protected bool TargetUntouched(int i)
	{
		if (!m_Orig || i < 0 || i >= m_TgtFrom.Count())
			return false;
		int from = m_TgtFrom[i];
		if (from < 0 || from >= m_Orig.Targets.Count())
			return false;
		INT_QST_TargetDef ot = m_Orig.Targets[from];
		INT_QST_TargetDef et = m_Q.Targets[i];
		return INT_QST_QuestEditor.SameTarget(et, ot);
	}

	protected bool RewardUntouched(int i)
	{
		if (!m_Orig || i < 0 || i >= m_RwFrom.Count())
			return false;
		int from = m_RwFrom[i];
		if (from < 0 || from >= m_Orig.Rewards.Count())
			return false;
		INT_QST_RewardDef orw = m_Orig.Rewards[from];
		INT_QST_RewardDef erw = m_Q.Rewards[i];
		return INT_QST_QuestEditor.SameReward(erw, orw);
	}

	protected bool ItemUntouched(int i)
	{
		if (!m_Orig || i < 0 || i >= m_QiFrom.Count())
			return false;
		int from = m_QiFrom[i];
		if (from < 0 || from >= m_Orig.QuestItems.Count())
			return false;
		INT_QST_ItemRef oi = m_Orig.QuestItems[from];
		INT_QST_ItemRef ei = m_Q.QuestItems[i];
		return INT_QST_QuestEditor.SameItem(ei, oi);
	}

	// Only what the admin changed is normalised (trimmed, re-cut, cleaned by kind): "save without changes" sends the
	// quest exactly as it came (the record hash stays, nothing of an old file changes behind the admin's back).
	protected void Finalize()
	{
		if (!m_Orig || m_Q.Name != m_Orig.Name)
		{
			string nm = INT_QST_Util.OneLine(m_Q.Name, INT_QST_Const.MAX_NAME_BYTES);
			m_Q.Name = nm;
		}
		if (m_Orig && m_DescText == m_OrigDesc)
		{
			// the pieces as they were (the converter cut them its own way)
			m_Q.Description.Clear();
			foreach (string op : m_Orig.Description)
				m_Q.Description.Insert(op);
		}
		else if (m_DescText.LengthUtf8() < 8000)
		{
			// pieces of at most TEXT_PIECE_BYTES; past ~8000 characters SubstringUtf8 fails, the byte splitter takes over
			INT_QST_Util.TextToPieces(m_DescText, m_Q.Description);
		}
		else
		{
			INT_QST_Util.SplitLong(m_DescText, INT_QST_Const.TEXT_PIECE_BYTES, m_Q.Description);
		}
		if (!m_Orig || m_Repeat != m_OrigRepeat)
		{
			string rk = INT_QST_RM.ToKey(m_Repeat);
			m_Q.RepeatMode = rk;
			if (m_Repeat != INT_QST_RM.COOLDOWN)
				m_Q.Cooldown = 0;
		}
		int nt = m_Q.Targets.Count();
		for (int ti = 0; ti < nt; ti++)
		{
			INT_QST_TargetDef t = m_Q.Targets[ti];
			if (t && !TargetUntouched(ti))
				CleanTarget(t);
		}
		int nr = m_Q.Rewards.Count();
		for (int ri = 0; ri < nr; ri++)
		{
			INT_QST_RewardDef r = m_Q.Rewards[ri];
			if (r && !RewardUntouched(ri))
				CleanReward(r);
		}
		int ni = m_Q.QuestItems.Count();
		for (int qi = 0; qi < ni; qi++)
		{
			INT_QST_ItemRef it = m_Q.QuestItems[qi];
			if (!it || ItemUntouched(qi))
				continue;
			string item = it.Item + "";
			item.TrimInPlace();
			it.Item = item;
		}
		if (!m_Orig || !INT_QST_QuestEditor.SameInts(m_Q.Requires, m_Orig.Requires))
		{
			array<int> req = new array<int>();
			foreach (int rid : m_Q.Requires)
			{
				if (rid != m_Q.Id && req.Find(rid) < 0)
					req.Insert(rid);
			}
			m_Q.Requires.Clear();
			foreach (int keep : req)
				m_Q.Requires.Insert(keep);
		}
		m_Q.m_Branch = m_BranchKey;
	}

	protected void TrySave()
	{
		if (m_Desc)
		{
			string dt = m_Desc.GetText();
			m_DescText = dt;
		}
		array<string> problems = new array<string>();
		Validate(problems);
		int np = problems.Count();
		if (np > 0)
		{
			string first = problems[0];
			if (np > 1)
			{
				int more = np - 1;
				string moreText = more.ToString();
				string tail = INT_QST_Look.Fmt("#STR_INT_QST_ED_E_MORE", moreText);
				first = first + "   ·   " + tail;
			}
			Flash(first, true);
			return;
		}
		Finalize();
		INT_QST_QuestDef outQ = m_Q;
		string key = m_BranchKey + "";
		m_Dirty = false;
		INT_QST_Sfx.Play("stamp");
		Event_Save.Invoke(outQ, key);
		Close();
	}
}
