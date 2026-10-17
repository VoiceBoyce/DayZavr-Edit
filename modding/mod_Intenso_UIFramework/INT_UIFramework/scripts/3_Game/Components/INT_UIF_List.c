// ============================================================================
// INT_UIFramework - List (v2): scrollable rich rows.
//
//   INT_UIF_List l = INT_UIF_List.Create(m_Bag, parent, x, y, w, h, "#STR_EMPTY");
//   int r = l.AddRow("Serega", "администратор", 7);
//   l.SetRowAvatar(r, "S", "teal");
//   l.AddRowAction(r, "remove", "user_minus", INT_UIF_Variant.DANGER, "#STR_REMOVE");
//   l.SetRowBadge(r, "Админ", INT_UIF_Variant.SUCCESS);
//   l.OnAction().Insert(OnRowAction);   // void OnRowAction(int row, string id, INT_UIF_List l)
//
// Each row is a rounded tile: lead (avatar / icon tile), two lines of text,
// right side (badge, value text or a button). Row actions stay hidden until
// the row is hovered: they fade in at the right edge while the badge slides
// out of their way. Rows cascade in with Reveal().
// ============================================================================

class INT_UIF_ListRow extends Managed
{
	string m_Primary;
	string m_Secondary;
	int m_Data;
	Widget m_Root;
	ref INT_UIF_Surface m_Body;
	ImageWidget m_Accent;
	ButtonWidget m_Hit;
	TextWidget m_PrimaryW;
	TextWidget m_SecondaryW;
	TextWidget m_RightW;
	float m_TextX;
	float m_RightEdge;
	Widget m_BadgeRoot;
	float m_BadgeX;
	Widget m_Actions;
	float m_ActionsW;
	ref array<string> m_ActionIds;
	ref array<INT_UIF_IconButton> m_ActionButtons;
	INT_UIF_Button m_Button;
	string m_ButtonId;

	void INT_UIF_ListRow()
	{
		m_ActionIds = new array<string>();
		m_ActionButtons = new array<INT_UIF_IconButton>();
	}
}

class INT_UIF_List extends INT_UIF_Component
{
	static const float ROW_H = 56;
	static const float ROW_GAP = 8;
	static const float ACTION = 30;

	protected Widget m_ListRoot;
	protected ScrollWidget m_Scroll;
	protected Widget m_Rows;
	protected TextWidget m_Empty;
	protected ref array<ref INT_UIF_ListRow> m_Items;
	protected int m_Selected = -1;
	protected int m_Hover = -1;
	protected bool m_Selectable;
	protected bool m_HoverPinned;
	protected string m_EmptyText;
	protected ref ScriptInvoker m_OnSelected;
	protected ref ScriptInvoker m_OnActivated;
	protected ref ScriptInvoker m_OnAction;

	void INT_UIF_List()
	{
		m_Items = new array<ref INT_UIF_ListRow>();
		m_OnSelected = new ScriptInvoker();
		m_OnActivated = new ScriptInvoker();
		m_OnAction = new ScriptInvoker();
	}

	static INT_UIF_List Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float h, string emptyText = "")
	{
		INT_UIF_List l = new INT_UIF_List();
		l.m_EmptyText = emptyText;
		if (bag)
			bag.Add(l);
		l.BuildAt(parent, x, y, w, h);
		return l;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufList";
	}

	override void OnBuild()
	{
		m_ListRoot = INT_UIF_Widgets.Create("list.layout", m_Root);
		m_Scroll = INT_UIF_Widgets.FindScroll(m_ListRoot, "Scroll");
		m_Rows = INT_UIF_Widgets.Find(m_ListRoot, "Rows");
		m_Empty = INT_UIF_Widgets.FindText(m_ListRoot, "Empty");
		if (m_Empty)
		{
			m_Empty.SetTextExactSize(INT_UIF_Scale.TextPx(13));
			m_Empty.SetText(m_EmptyText);
		}
		UpdateContentSize();
	}

	// --- events --------------------------------------------------------------------
	// (int row, INT_UIF_List list) - selection changed by the user
	ScriptInvoker OnSelected()
	{
		return m_OnSelected;
	}

	// (int row, INT_UIF_List list) - double click
	ScriptInvoker OnActivated()
	{
		return m_OnActivated;
	}

	// (int row, string actionId, INT_UIF_List list) - row action or row button
	ScriptInvoker OnAction()
	{
		return m_OnAction;
	}

	// --- data ----------------------------------------------------------------------
	void SetEmptyText(string text)
	{
		m_EmptyText = text;
		if (m_Empty)
			m_Empty.SetText(text);
	}

	void SetSelectable(bool selectable)
	{
		m_Selectable = selectable;
	}

	int Count()
	{
		return m_Items.Count();
	}

	int GetSelected()
	{
		return m_Selected;
	}

	int GetData(int row)
	{
		if (row < 0 || row >= m_Items.Count())
			return 0;
		return m_Items.Get(row).m_Data;
	}

	string GetPrimary(int row)
	{
		if (row < 0 || row >= m_Items.Count())
			return "";
		return m_Items.Get(row).m_Primary;
	}

	protected float RowW()
	{
		return m_W - 10;
	}

	protected INT_UIF_ListRow RowAt(int row)
	{
		if (row < 0 || row >= m_Items.Count())
			return null;
		return m_Items.Get(row);
	}

	// Returns the new row index.
	int AddRow(string primary, string secondary = "", int data = 0)
	{
		if (!m_Rows)
			return -1;
		int idx = m_Items.Count();
		float rw = RowW();
		INT_UIF_ListRow row = new INT_UIF_ListRow();
		row.m_Primary = primary;
		row.m_Secondary = secondary;
		row.m_Data = data;
		row.m_TextX = 16;
		row.m_RightEdge = rw - 14;
		row.m_Root = INT_UIF_Draw.Frame(m_Rows, 0, idx * (ROW_H + ROW_GAP), rw, ROW_H, "Row");
		row.m_Body = INT_UIF_Surface.Box(row.m_Root, 0, 0, rw, ROW_H, 10, 0, 0);
		row.m_Accent = INT_UIF_Draw.Line(row.m_Root, 0, 16, 3, ROW_H - 32, 0);
		row.m_Hit = INT_UIF_Draw.Hit(row.m_Root, 0, 0, rw, ROW_H, this);
		if (secondary != "")
		{
			row.m_PrimaryW = INT_UIF_Draw.Text(row.m_Root, row.m_TextX, 9, rw - 40, 20, primary, 14, 0);
			row.m_SecondaryW = INT_UIF_Draw.Text(row.m_Root, row.m_TextX, 29, rw - 40, 18, secondary, 12, 0);
		}
		else
		{
			row.m_PrimaryW = INT_UIF_Draw.Text(row.m_Root, row.m_TextX, 0, rw - 40, ROW_H, primary, 14, 0);
		}
		m_Items.Insert(row);
		PaintRow(idx, false);
		UpdateContentSize();
		return idx;
	}

	protected void ShiftText(INT_UIF_ListRow row, float x)
	{
		row.m_TextX = x;
		float tw = row.m_RightEdge - x - 8;
		if (row.m_SecondaryW)
		{
			INT_UIF_Draw.Place(row.m_PrimaryW, x, 9, tw, 20);
			INT_UIF_Draw.Place(row.m_SecondaryW, x, 29, tw, 18);
		}
		else if (row.m_PrimaryW)
		{
			INT_UIF_Draw.Place(row.m_PrimaryW, x, 0, tw, ROW_H);
		}
	}

	void SetRowAvatar(int rowIdx, string letter, string hueKey)
	{
		INT_UIF_ListRow row = RowAt(rowIdx);
		if (!row)
			return;
		INT_UIF_Draw.Avatar(row.m_Root, 12, 10, 36, letter, C(hueKey));
		ShiftText(row, 60);
	}

	void SetRowIcon(int rowIdx, string icon, string hueKey)
	{
		INT_UIF_ListRow row = RowAt(rowIdx);
		if (!row)
			return;
		INT_UIF_Draw.IconTile(row.m_Root, 12, 10, 36, icon, C(hueKey), 10);
		ShiftText(row, 60);
	}

	INT_UIF_Badge SetRowBadge(int rowIdx, string text, int variant, string icon = "")
	{
		INT_UIF_ListRow row = RowAt(rowIdx);
		if (!row)
			return null;
		float bw = INT_UIF_Badge.FitWidth(text, 22, icon);
		float bx = row.m_RightEdge - bw;
		INT_UIF_Badge b = INT_UIF_Badge.Create(m_Bag, row.m_Root, bx, (ROW_H - 22) * 0.5, text, variant, 22, icon, bw);
		if (b)
			row.m_BadgeRoot = b.GetRoot();
		row.m_BadgeX = bx;
		row.m_RightEdge = bx - 10;
		ShiftText(row, row.m_TextX);
		return b;
	}

	void SetRowRight(int rowIdx, string text, int color)
	{
		INT_UIF_ListRow row = RowAt(rowIdx);
		if (!row)
			return;
		float w = INT_UIF_Draw.TextWidth(text, 14) + 6;
		float x = row.m_RightEdge - w;
		row.m_RightW = INT_UIF_Draw.Text(row.m_Root, x, 0, w, ROW_H, text, 14, color, INT_UIF_Align.RIGHT);
		row.m_RightEdge = x - 10;
		ShiftText(row, row.m_TextX);
	}

	INT_UIF_Button SetRowButton(int rowIdx, string actionId, string text, int variant, string icon = "", float width = 124)
	{
		INT_UIF_ListRow row = RowAt(rowIdx);
		if (!row)
			return null;
		float bx = row.m_RightEdge - width;
		INT_UIF_Button b = INT_UIF_Button.Create(m_Bag, row.m_Root, bx, 12, width, ROW_H - 24, text, variant, icon);
		if (b)
			b.OnClick().Insert(OnRowButtonClick);
		row.m_Button = b;
		row.m_ButtonId = actionId;
		row.m_RightEdge = bx - 10;
		ShiftText(row, row.m_TextX);
		return b;
	}

	// Hover-revealed action at the right edge (added right to left).
	void AddRowAction(int rowIdx, string actionId, string icon, int variant, string tooltip = "")
	{
		INT_UIF_ListRow row = RowAt(rowIdx);
		if (!row)
			return;
		float rw = RowW();
		if (!row.m_Actions)
		{
			row.m_Actions = INT_UIF_Draw.Frame(row.m_Root, 0, 0, rw, ROW_H, "Actions");
			row.m_Actions.SetAlpha(0);
			row.m_Actions.Show(false);
		}
		int k = row.m_ActionIds.Count();
		float ax = rw - 14 - ACTION - k * (ACTION + 8);
		INT_UIF_IconButton b = INT_UIF_IconButton.Create(m_Bag, row.m_Actions, ax, (ROW_H - ACTION) * 0.5, ACTION, icon, variant, tooltip);
		if (b)
			b.OnClick().Insert(OnRowActionClick);
		row.m_ActionIds.Insert(actionId);
		row.m_ActionButtons.Insert(b);
		row.m_ActionsW = (k + 1) * (ACTION + 8);
	}

	void Clear()
	{
		m_Bag.Clear();
		INT_UIF_Widgets.ClearChildren(m_Rows);
		m_Items.Clear();
		m_Selected = -1;
		m_Hover = -1;
		StopTicking();
		UpdateContentSize();
		if (m_Scroll)
			m_Scroll.VScrollToPos(0);
	}

	void Select(int rowIdx, bool notify = true)
	{
		if (rowIdx >= m_Items.Count())
			rowIdx = -1;
		int old = m_Selected;
		m_Selected = rowIdx;
		if (old >= 0)
			PaintRow(old, true);
		if (m_Selected >= 0)
			PaintRow(m_Selected, true);
		if (notify)
			m_OnSelected.Invoke(m_Selected, this);
	}

	// Holds a row in its hover state regardless of the cursor (previews,
	// tests, keyboard focus); -1 releases it.
	void PinHover(int rowIdx)
	{
		m_HoverPinned = rowIdx >= 0;
		SetHover(rowIdx);
	}

	bool IsActionsShown(int rowIdx)
	{
		INT_UIF_ListRow row = RowAt(rowIdx);
		if (!row || !row.m_Actions)
			return false;
		return row.m_Actions.IsVisible();
	}

	// Rows cascade in: fade + rise, staggered.
	void Reveal(float baseDelay = 0)
	{
		int n = m_Items.Count();
		for (int i = 0; i < n; i++)
		{
			INT_UIF_Animator.Reveal(m_Items.Get(i).m_Root, baseDelay + i * 0.04, 8, 0.32);
		}
	}

	// --- painting --------------------------------------------------------------------
	protected void PaintRow(int rowIdx, bool animate)
	{
		INT_UIF_ListRow row = RowAt(rowIdx);
		if (!row || !row.m_Body)
			return;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		bool hot = rowIdx == m_Hover;
		bool sel = rowIdx == m_Selected;
		int fill = tm.Color(INT_UIF_ThemeKeys.TILE);
		int border = tm.Color(INT_UIF_ThemeKeys.TILE_BORDER);
		int accent = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.TEAL), 0);
		if (hot || sel)
		{
			fill = tm.Color(INT_UIF_ThemeKeys.TILE_HOVER);
			border = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.TEAL), 76);
			accent = tm.Color(INT_UIF_ThemeKeys.TEAL);
		}
		if (animate)
		{
			row.m_Body.FadeFill(fill, AnimFast());
			row.m_Body.FadeBorder(border, AnimFast());
			if (row.m_Accent)
				INT_UIF_Animator.ColorTo(row.m_Accent, accent, AnimFast());
		}
		else
		{
			row.m_Body.SetFill(fill);
			row.m_Body.SetBorder(border);
			if (row.m_Accent)
				row.m_Accent.SetColor(accent);
		}
		if (row.m_PrimaryW)
			row.m_PrimaryW.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT));
		if (row.m_SecondaryW)
			row.m_SecondaryW.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
	}

	protected void RevealActions(INT_UIF_ListRow row, bool show)
	{
		if (!row.m_Actions)
			return;
		float shift = row.m_ActionsW + 4;
		if (show)
		{
			INT_UIF_Animator.Fade(row.m_Actions, 1, AnimFast());
			row.m_Actions.Show(true);
			if (row.m_BadgeRoot)
				INT_UIF_Animator.MoveX(row.m_BadgeRoot, INT_UIF_Draw.S(row.m_BadgeX - shift), AnimNormal(), INT_UIF_Ease.OUT_CUBIC);
		}
		else
		{
			INT_UIF_Animator.FadeOut(row.m_Actions, AnimFast(), true);
			if (row.m_BadgeRoot)
				INT_UIF_Animator.MoveX(row.m_BadgeRoot, INT_UIF_Draw.S(row.m_BadgeX), AnimNormal(), INT_UIF_Ease.OUT_CUBIC);
		}
	}

	protected void SetHover(int rowIdx)
	{
		if (rowIdx == m_Hover)
			return;
		int old = m_Hover;
		m_Hover = rowIdx;
		INT_UIF_ListRow oldRow = RowAt(old);
		if (oldRow)
		{
			PaintRow(old, true);
			RevealActions(oldRow, false);
		}
		INT_UIF_ListRow newRow = RowAt(rowIdx);
		if (newRow)
		{
			PaintRow(rowIdx, true);
			RevealActions(newRow, true);
			StartTicking();
		}
		else
		{
			StopTicking();
		}
	}

	// While a row is hot, follow the real cursor: moving onto the row's own
	// action buttons keeps it hot, leaving the row (to anywhere) cools it.
	override void OnTick(float dt)
	{
		INT_UIF_ListRow row = RowAt(m_Hover);
		if (!row)
		{
			StopTicking();
			return;
		}
		if (m_HoverPinned)
			return;
		int mx;
		int my;
		GetMousePos(mx, my);
		if (!INT_UIF_Widgets.Contains(row.m_Root, mx, my) || !row.m_Root.IsVisibleHierarchy())
			SetHover(-1);
	}

	protected void UpdateContentSize()
	{
		int n = m_Items.Count();
		if (m_Rows)
		{
			float h = n * (ROW_H + ROW_GAP);
			if (h < 1)
				h = 1;
			m_Rows.SetSize(1, INT_UIF_Draw.S(h));
		}
		if (m_Empty)
			m_Empty.Show(n == 0);
	}

	override void ApplyTheme()
	{
		if (m_Empty)
			m_Empty.SetColor(C(INT_UIF_ThemeKeys.TEXT_FAINT));
		int n = m_Items.Count();
		for (int i = 0; i < n; i++)
		{
			PaintRow(i, false);
		}
	}

	// --- input -----------------------------------------------------------------------
	protected int IndexOfHit(Widget w)
	{
		if (!w)
			return -1;
		int n = m_Items.Count();
		for (int i = 0; i < n; i++)
		{
			if (m_Items.Get(i).m_Hit == w)
				return i;
		}
		return -1;
	}

	protected void OnRowActionClick(INT_UIF_IconButton button)
	{
		int n = m_Items.Count();
		for (int i = 0; i < n; i++)
		{
			INT_UIF_ListRow row = m_Items.Get(i);
			int k = row.m_ActionButtons.Find(button);
			if (k >= 0)
			{
				string id = row.m_ActionIds.Get(k);
				INT_UIF_List keep = this;
				m_OnAction.Invoke(i, id, this);
				return;
			}
		}
	}

	protected void OnRowButtonClick(INT_UIF_Button button)
	{
		int n = m_Items.Count();
		for (int i = 0; i < n; i++)
		{
			INT_UIF_ListRow row = m_Items.Get(i);
			if (row.m_Button == button)
			{
				string id = row.m_ButtonId;
				INT_UIF_List keep = this;
				m_OnAction.Invoke(i, id, this);
				return;
			}
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int idx = IndexOfHit(w);
		if (idx < 0 || button != MouseState.LEFT)
			return false;
		SetFocus(null);
		if (m_Selectable)
			Select(idx, true);
		return true;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		int idx = IndexOfHit(w);
		if (idx < 0 || button != MouseState.LEFT)
			return false;
		INT_UIF_List keep = this;
		m_OnActivated.Invoke(idx, this);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int idx = IndexOfHit(w);
		if (idx < 0)
			return false;
		m_HoverPinned = false;
		SetHover(idx);
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		// Leaving is resolved by OnTick against the row rectangle.
		return IndexOfHit(w) >= 0;
	}
}
