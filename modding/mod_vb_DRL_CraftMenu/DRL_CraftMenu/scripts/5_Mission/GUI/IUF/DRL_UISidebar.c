// ============================================================================
// DRL_CraftMenu - сайдбар категорий: строки с иконкой, названием и числом
// рецептов. Пул видимых строк, прокрутка колесом целыми строками (нативный
// ScrollWidget не берёт стили и рисует белую полосу). Как у INT_Trader.
// Хит-зоны строк названы DRL_Cat_<n> (n — номер строки на экране) для dayz-mcp.
// ============================================================================

class DRL_UICategoryEntry extends Managed
{
	int m_Key;           // индекс категории в конфиге
	string m_Name;
	string m_IconPath;   // уже разрешённый путь иконки ("" — без иконки)
	int m_Count;
	string m_Glyph;      // иконка фреймворка, если нет картинки ("" — list)

	void DRL_UICategoryEntry(int key, string name, string iconPath, int count, string glyph = "")
	{
		m_Key = key;
		m_Name = name;
		m_IconPath = iconPath;
		m_Count = count;
		m_Glyph = glyph;
	}
}

class DRL_UISidebarRow extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ImageWidget m_Accent;
	ref INT_UIF_Surface m_IconBg;
	ImageWidget m_Icon;
	TextWidget m_Name;
	TextWidget m_Count;
	ButtonWidget m_Hit;
	int m_Entry = -1;
	float m_Y;
	bool m_Hover;
	bool m_Selected;
}

class DRL_UISidebar extends ScriptedWidgetEventHandler
{
	static const float ROW_H = 42;
	static const float ROW_GAP = 6;

	protected Widget m_Root;
	protected Widget m_List;
	protected ButtonWidget m_ListHit;
	protected ImageWidget m_Thumb;
	protected ref array<ref DRL_UISidebarRow> m_Rows;
	protected ref array<ref DRL_UICategoryEntry> m_Entries;
	protected int m_First;
	protected int m_Selected = -1;
	protected float m_W;
	protected float m_ListH;
	protected ref ScriptInvoker m_OnSelected;

	void DRL_UISidebar()
	{
		m_Rows = new array<ref DRL_UISidebarRow>();
		m_Entries = new array<ref DRL_UICategoryEntry>();
		m_OnSelected = new ScriptInvoker();
	}

	// (int entryIndex, DRL_UISidebar sidebar)
	ScriptInvoker OnSelected()
	{
		return m_OnSelected;
	}

	void Build(Widget parent, float x, float y, float w, float h)
	{
		m_W = w;
		m_ListH = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_Sidebar");
		m_List = INT_UIF_Draw.Clip(m_Root, 0, 0, w, h, "DRL_SidebarRows");
		m_ListHit = INT_UIF_Draw.Hit(m_List, 0, 0, w, h, this);
		m_ListHit.SetName("DRL_SidebarBg");
		int visible = (h + ROW_GAP) / (ROW_H + ROW_GAP);
		for (int i = 0; i < visible; i++)
		{
			m_Rows.Insert(BuildRow(i, i * (ROW_H + ROW_GAP), w - 8));
		}
		m_Thumb = INT_UIF_Draw.Line(m_Root, w - 3, 0, 3, 40, INT_UIF.Soft(INT_UIF_ThemeKeys.TEXT, 50));
		m_Thumb.Show(false);
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	protected DRL_UISidebarRow BuildRow(int slot, float y, float w)
	{
		DRL_UISidebarRow r = new DRL_UISidebarRow();
		r.m_Y = y;
		r.m_Root = INT_UIF_Draw.Frame(m_List, 0, y, w, ROW_H, "DRL_CatRow");
		r.m_Bg = INT_UIF_Surface.Box(r.m_Root, 0, 0, w, ROW_H, 10, 0, 0);
		r.m_Accent = INT_UIF_Draw.Line(r.m_Root, 0, 10, 3, ROW_H - 20, 0);
		r.m_IconBg = INT_UIF_Surface.Box(r.m_Root, 10, 7, 28, 28, 8, 0, 0);
		r.m_Icon = INT_UIF_Draw.Image(r.m_Root, 15, 12, 18, 18, "iuf_white", 0);
		r.m_Name = INT_UIF_Draw.Text(r.m_Root, 48, 0, w - 48 - 46, ROW_H, "", 13, 0);
		r.m_Count = INT_UIF_Draw.Text(r.m_Root, w - 46, 0, 36, ROW_H, "", 12, 0, INT_UIF_Align.RIGHT);
		r.m_Hit = INT_UIF_Draw.Hit(r.m_Root, 0, 0, w, ROW_H, this);
		DRL_UIKitUtil.ExposeHit(r.m_Hit, "DRL_Cat_" + slot.ToString(), this);
		r.m_Root.Show(false);
		return r;
	}

	void SetEntries(array<ref DRL_UICategoryEntry> entries, int selected)
	{
		m_Entries.Clear();
		foreach (DRL_UICategoryEntry e : entries)
		{
			m_Entries.Insert(e);
		}
		m_Selected = selected;
		m_First = 0;
		EnsureVisible(selected);
		Rebind();
		Reveal();
	}

	int Count()
	{
		return m_Entries.Count();
	}

	DRL_UICategoryEntry GetEntry(int i)
	{
		if (i < 0 || i >= m_Entries.Count())
			return null;
		return m_Entries.Get(i);
	}

	int GetSelected()
	{
		return m_Selected;
	}

	int FindKey(int key)
	{
		int n = m_Entries.Count();
		for (int i = 0; i < n; i++)
		{
			if (m_Entries.Get(i).m_Key == key)
				return i;
		}
		return -1;
	}

	void Select(int index, bool notify)
	{
		if (index < 0 || index >= m_Entries.Count())
			return;
		m_Selected = index;
		EnsureVisible(index);
		Rebind();
		if (notify)
			m_OnSelected.Invoke(index, this);
	}

	void SetCount(int index, int count)
	{
		DRL_UICategoryEntry e = GetEntry(index);
		if (!e || e.m_Count == count)
			return;
		e.m_Count = count;
		Rebind();
	}

	protected void EnsureVisible(int index)
	{
		int visible = m_Rows.Count();
		if (index < 0)
			return;
		if (index < m_First)
			m_First = index;
		else if (index >= m_First + visible)
			m_First = index - visible + 1;
		m_First = Math.Clamp(m_First, 0, MaxFirst());
	}

	protected int MaxFirst()
	{
		int m = m_Entries.Count() - m_Rows.Count();
		if (m < 0)
			m = 0;
		return m;
	}

	protected void Rebind()
	{
		int n = m_Rows.Count();
		for (int i = 0; i < n; i++)
		{
			DRL_UISidebarRow r = m_Rows.Get(i);
			int ei = m_First + i;
			DRL_UICategoryEntry e = GetEntry(ei);
			r.m_Entry = -1;
			if (!e)
			{
				r.m_Root.Show(false);
				continue;
			}
			r.m_Entry = ei;
			r.m_Root.Show(true);
			r.m_Selected = ei == m_Selected;
			r.m_Name.SetText(e.m_Name);
			r.m_Count.SetText(e.m_Count.ToString());
			if (e.m_IconPath != "")
			{
				r.m_Icon.LoadImageFile(0, e.m_IconPath);
				r.m_Icon.SetImage(0);
				r.m_Icon.Show(true);
			}
			else
			{
				string glyph = e.m_Glyph;
				if (glyph == "")
					glyph = "list";
				INT_UIF_Widgets.SetIcon(r.m_Icon, glyph);
				r.m_Icon.Show(true);
			}
			PaintRow(r, false);
		}
		int total = m_Entries.Count();
		bool scroll = total > n;
		m_Thumb.Show(scroll);
		if (scroll)
		{
			float th = m_ListH * n / total;
			float ty = 0;
			if (MaxFirst() > 0)
				ty = (m_ListH - th) * m_First / MaxFirst();
			INT_UIF_Draw.Place(m_Thumb, m_W - 3, ty, 3, th);
		}
	}

	protected void PaintRow(DRL_UISidebarRow r, bool animate)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int hue = tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		int fill = ARGB(0, 0, 0, 0);
		int border = ARGB(0, 0, 0, 0);
		int text = tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		if (r.m_Hover)
		{
			fill = tm.Color(INT_UIF_ThemeKeys.TILE);
			text = tm.Color(INT_UIF_ThemeKeys.TEXT);
		}
		if (r.m_Selected)
		{
			fill = tm.Color(INT_UIF_ThemeKeys.TILE_HOVER);
			border = tm.Color(INT_UIF_ThemeKeys.TILE_BORDER);
			text = tm.Color(INT_UIF_ThemeKeys.TEXT);
			hue = tm.Color(INT_UIF_ThemeKeys.AMBER);
		}
		if (animate)
			r.m_Bg.FadeFill(fill, 0.12);
		else
			r.m_Bg.SetFill(fill);
		r.m_Bg.SetBorder(border);
		r.m_Accent.SetColor(tm.Color(INT_UIF_ThemeKeys.AMBER));
		r.m_Accent.Show(r.m_Selected);
		r.m_IconBg.SetFill(INT_UIF_Color.WithAlpha(hue, 34));
		r.m_IconBg.SetBorder(INT_UIF_Color.WithAlpha(hue, 70));
		r.m_Icon.SetColor(hue);
		r.m_Name.SetColor(text);
		r.m_Name.SetBold(r.m_Selected);
		r.m_Count.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
	}

	protected void Reveal()
	{
		int n = m_Rows.Count();
		for (int i = 0; i < n; i++)
		{
			DRL_UISidebarRow r = m_Rows.Get(i);
			if (r.m_Entry >= 0)
				DRL_UIAnim.Reveal(r.m_Root, 0, r.m_Y, 0.03 + i * 0.03, 8, 0.26);
		}
	}

	protected DRL_UISidebarRow RowOf(Widget w)
	{
		foreach (DRL_UISidebarRow r : m_Rows)
		{
			if (r.m_Hit == w)
				return r;
		}
		return null;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		DRL_UISidebarRow r = RowOf(w);
		if (!r || r.m_Entry < 0)
			return w == m_ListHit;
		if (r.m_Entry != m_Selected)
			Select(r.m_Entry, true);
		return true;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (w != m_ListHit && !RowOf(w))
			return false;
		int target = Math.Clamp(m_First - wheel, 0, MaxFirst());
		if (target != m_First)
		{
			m_First = target;
			Rebind();
		}
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		DRL_UISidebarRow r = RowOf(w);
		if (!r || r.m_Entry < 0)
			return false;
		r.m_Hover = true;
		PaintRow(r, true);
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		DRL_UISidebarRow r = RowOf(w);
		if (!r)
			return false;
		r.m_Hover = false;
		PaintRow(r, true);
		return true;
	}
}
