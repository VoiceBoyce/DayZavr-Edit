// INT Quests (client) — pooled vertical list for long lists (done quests, admin lists with hundreds of rows).
// A fixed set of row slots is re-bound while scrolling (pattern of INT_P2P_UIRowList); rows may have different
// heights (group captions vs quest rows). Text is not cut by clipping in DayZ, so a slot that is not fully inside
// the viewport is hidden instead of being half-drawn. Scroll by wheel, by dragging the thumb or by clicking the track.

class INT_QST_VSlot
{
	Widget m_Root;
	ButtonWidget m_Hit;
	Widget m_Hover;		// set by BindSlot when the row has a hover highlight
	int m_Index = -1;
	int m_Kind = -1;
}

// What a list shows: subclass per window.
class INT_QST_VSource
{
	int Count()
	{
		return 0;
	}

	float RowHeight(int i)
	{
		return 40;
	}

	// A new empty slot (subclass it for typed widget fields). The list puts the row click zone into the root
	// first, then BuildSlot adds the row widgets ON TOP (so buttons inside a row get their own clicks).
	INT_QST_VSlot NewSlot()
	{
		return new INT_QST_VSlot();
	}

	// Create the widgets of an empty slot (all kinds; BindSlot shows what the row needs).
	void BuildSlot(INT_QST_VSlot slot, float w)
	{
	}

	void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
	}

	void OnRowClick(int i, int button)
	{
	}
}

class INT_QST_VList
{
	static const int ID_ROW = 1;
	static const int ID_TRACK = 2;
	static const int ID_THUMB = 3;
	static const int ID_BG = 4;

	protected ref INT_QST_VSource m_Src;
	protected ref INT_QST_Clicks m_Clicks;
	protected ref array<ref INT_QST_VSlot> m_Slots = new array<ref INT_QST_VSlot>();
	protected ref array<float> m_Top = new array<float>();	// row tops (design px)
	protected Widget m_Root;
	protected Widget m_View;
	protected Widget m_Bar;
	protected Widget m_Thumb;
	protected float m_W;
	protected float m_H;
	protected float m_Total;
	protected float m_Scroll;
	protected float m_BoundScroll = -1;	// m_Scroll at the last Rebind
	protected bool m_Dragging;
	protected float m_DragY;
	protected float m_DragScroll;
	protected int m_Selected = -1;
	protected int m_BarColor;
	protected int m_ThumbColor;

	void INT_QST_VList()
	{
		m_Clicks = new INT_QST_Clicks();
		m_Clicks.Event_Click.Insert(OnClick);
		m_Clicks.Event_Wheel.Insert(OnWheel);
		m_Clicks.Event_Down.Insert(OnDown);
	}

	void ~INT_QST_VList()
	{
		INT_QST_VListTicker.Remove(this);
	}

	void Build(Widget parent, float x, float y, float w, float h, INT_QST_VSource src, int barColor, int thumbColor)
	{
		m_Src = src;
		m_W = w - 12;
		m_H = h;
		m_BarColor = barColor;
		m_ThumbColor = thumbColor;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "VList");
		ButtonWidget bg = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, m_Clicks);
		m_Clicks.Bind(bg, ID_BG);
		m_View = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, h, "View");
		m_Bar = INT_UIF_Draw.Frame(m_Root, w - 6, 0, 4, h, "Bar");
		INT_UIF_Draw.Line(m_Bar, 1, 0, 2, h, m_BarColor);
		ButtonWidget track = INT_UIF_Draw.Hit(m_Root, w - 12, 0, 12, h, m_Clicks);
		m_Clicks.Bind(track, ID_TRACK);
		m_Thumb = INT_UIF_Draw.Frame(m_Root, w - 8, 0, 8, 40, "Thumb");
		INT_UIF_Surface.Box(m_Thumb, 0, 0, 8, 40, 4, m_ThumbColor, 0);
		ButtonWidget th = INT_UIF_Draw.Hit(m_Thumb, -4, 0, 16, 40, m_Clicks);
		m_Clicks.Bind(th, ID_THUMB);
		Refresh(false);
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	int GetSelected()
	{
		return m_Selected;
	}

	void SetSelected(int i)
	{
		m_Selected = i;
	}

	// Data changed: recompute heights, keep the scroll (clamped) unless asked to go to the top.
	void Refresh(bool toTop)
	{
		m_Top.Clear();
		float y = 0;
		int n = 0;
		if (m_Src)
			n = m_Src.Count();
		for (int i = 0; i < n; i++)
		{
			m_Top.Insert(y);
			y = y + m_Src.RowHeight(i);
		}
		m_Total = y;
		if (toTop)
			m_Scroll = 0;
		ClampScroll();
		EnsureSlots();
		Rebind();
	}

	protected void ClampScroll()
	{
		float maxS = m_Total - m_H;
		if (maxS < 0)
			maxS = 0;
		if (m_Scroll > maxS)
			m_Scroll = maxS;
		if (m_Scroll < 0)
			m_Scroll = 0;
	}

	// Enough slots for a full view of the smallest rows.
	protected void EnsureSlots()
	{
		if (!m_Src || !m_View)
			return;
		float minH = 1000;
		int n = m_Src.Count();
		for (int i = 0; i < n; i++)
		{
			float rh = m_Src.RowHeight(i);
			if (rh < minH)
				minH = rh;
		}
		if (minH < 16)
			minH = 16;
		int need = (int)Math.Ceil(m_H / minH) + 2;
		if (need > n)
			need = n;
		while (m_Slots.Count() < need)
		{
			INT_QST_VSlot s = m_Src.NewSlot();
			if (!s)
				break;
			s.m_Root = INT_UIF_Draw.Frame(m_View, 0, 0, m_W, 40, "Slot");
			if (!s.m_Root)
				break;
			s.m_Hit = INT_UIF_Draw.Hit(s.m_Root, 0, 0, m_W, 40, m_Clicks);
			m_Src.BuildSlot(s, m_W);
			m_Slots.Insert(s);
		}
	}

	// First visible row index.
	protected int FirstVisible()
	{
		int lo = 0;
		int hi = m_Top.Count() - 1;
		while (lo < hi)
		{
			int mid = (lo + hi + 1) / 2;
			if (m_Top[mid] <= m_Scroll)
				lo = mid;
			else
				hi = mid - 1;
		}
		return lo;
	}

	void Rebind()
	{
		if (!m_Src)
			return;
		m_BoundScroll = m_Scroll;
		int n = m_Top.Count();
		int first = 0;
		if (n > 0)
			first = FirstVisible();
		int si = 0;
		for (int i = first; i < n && si < m_Slots.Count(); i++)
		{
			float top = m_Top[i] - m_Scroll;
			float rh = m_Src.RowHeight(i);
			if (top >= m_H)
				break;
			INT_QST_VSlot s = m_Slots[si];
			si++;
			bool inside = top >= -0.5 && top + rh <= m_H + 0.5;
			s.m_Root.Show(inside);
			if (!inside)
				continue;
			INT_UIF_Draw.Place(s.m_Root, 0, top, m_W, rh);
			if (s.m_Hit)
				INT_UIF_Draw.Place(s.m_Hit, 0, 0, m_W, rh);
			s.m_Index = i;
			m_Src.BindSlot(s, i, m_W);
			m_Clicks.Bind(s.m_Hit, ID_ROW, i, s.m_Hover);
		}
		for (int k = si; k < m_Slots.Count(); k++)
		{
			m_Slots[k].m_Root.Show(false);
			m_Slots[k].m_Index = -1;
		}
		PlaceThumb();
	}

	protected void PlaceThumb()
	{
		if (!m_Thumb || !m_Bar)
			return;
		bool need = m_Total > m_H + 1;
		m_Thumb.Show(need);
		m_Bar.Show(need);
		if (!need)
			return;
		float th = m_H * m_H / m_Total;
		if (th < 30)
			th = 30;
		float maxS = m_Total - m_H;
		float ty = 0;
		if (maxS > 0)
			ty = (m_H - th) * m_Scroll / maxS;
		INT_UIF_Draw.Place(m_Thumb, m_W + 4, ty, 8, th);
		Widget box = m_Thumb.GetChildren();
		while (box)
		{
			INT_UIF_Draw.Resize(box, 8, th);
			box = box.GetSibling();
		}
	}

	void ScrollBy(float dy)
	{
		m_Scroll = m_Scroll + dy;
		ClampScroll();
		Rebind();
	}

	void ScrollToRow(int i)
	{
		if (i < 0 || i >= m_Top.Count())
			return;
		float top = m_Top[i];
		float bottom = top + m_Src.RowHeight(i);
		if (top < m_Scroll)
			m_Scroll = top;
		else if (bottom > m_Scroll + m_H)
			m_Scroll = bottom - m_H;
		ClampScroll();
		Rebind();
	}

	protected void OnWheel(int id, int wheel)
	{
		ScrollBy(-wheel * 60);
	}

	protected void OnClick(int id, int data, int button)
	{
		if (id == ID_ROW)
		{
			m_Selected = data;
			if (m_Src)
				m_Src.OnRowClick(data, button);
			return;
		}
		if (id == ID_TRACK)
		{
			int mx;
			int my;
			GetMousePos(mx, my);
			float sx;
			float sy;
			m_Root.GetScreenPos(sx, sy);
			float rel = (my - sy) / INT_UIF_Scale.Factor();
			if (rel < 0)
				rel = 0;
			float maxS = m_Total - m_H;
			m_Scroll = maxS * rel / m_H;
			ClampScroll();
			Rebind();
		}
	}

	protected void OnDown(int id, int data, int button)
	{
		if (id != ID_THUMB || button != MouseState.LEFT)
			return;
		int mx;
		int my;
		GetMousePos(mx, my);
		m_Dragging = true;
		m_DragY = my;
		m_DragScroll = m_Scroll;
		INT_QST_VListTicker.Add(this);
	}

	// Thumb drag, polled every frame while the button is held.
	bool TickDrag()
	{
		if (!m_Dragging)
			return false;
		if ((GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) == 0)
		{
			m_Dragging = false;
			return false;
		}
		int mx;
		int my;
		GetMousePos(mx, my);
		float dy = (my - m_DragY) / INT_UIF_Scale.Factor();
		float th = m_H * m_H / m_Total;
		if (th < 30)
			th = 30;
		float maxS = m_Total - m_H;
		if (m_H - th > 1)
			m_Scroll = m_DragScroll + dy * maxS / (m_H - th);
		ClampScroll();
		// polled every frame while the button is held: rows are re-bound only when the list really moved
		if (Math.AbsFloat(m_Scroll - m_BoundScroll) >= 0.5)
			Rebind();
		return true;
	}
}

// Drives thumb drags from the mission frame (a list is not a widget handler with an update of its own).
class INT_QST_VListTicker
{
	protected static ref array<INT_QST_VList> s_Lists = new array<INT_QST_VList>();

	static void Add(INT_QST_VList l)
	{
		if (s_Lists.Find(l) < 0)
			s_Lists.Insert(l);
	}

	static void Remove(INT_QST_VList l)
	{
		s_Lists.RemoveItem(l);
	}

	static void Tick()
	{
		for (int i = s_Lists.Count() - 1; i >= 0; i--)
		{
			INT_QST_VList l = s_Lists[i];
			if (!l || !l.TickDrag())
				s_Lists.Remove(i);
		}
	}
}
