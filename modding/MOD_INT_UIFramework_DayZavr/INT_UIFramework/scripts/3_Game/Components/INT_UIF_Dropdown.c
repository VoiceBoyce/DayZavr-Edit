// ============================================================================
// INT_UIFramework - Dropdown (v2).
//
//   INT_UIF_Dropdown d = INT_UIF_Dropdown.Create(m_Bag, parent, x, y, 260, options, 0, "#STR_X_PICK");
//   d.OnSelected().Insert(OnPick);   // void OnPick(int index, string text, INT_UIF_Dropdown d)
//
// The field looks like a TextInput (tile, hairline border, teal ring + glow
// while open) with a chevron that turns over. The popup is a TOP-LEVEL widget
// sorted above menus, so neither a card nor the window clips it: a panel with
// a drop shadow, rows that cascade in, a hover tint, a teal check on the
// selected row and a scroll area past MAX_VISIBLE rows. It drops down (or up
// when there is no room below) and closes on a pick, a click outside, Esc
// (INT_UIF_Menu asks CloseActive() first) or when the field disappears.
// ============================================================================

class INT_UIF_Dropdown extends INT_UIF_Component
{
	static const int MAX_VISIBLE = 7;
	static const float ROW_H = 36;
	static const float PAD = 6;

	// The open dropdown (weak: turns null when it is destroyed).
	protected static INT_UIF_Dropdown s_Active;

	protected ref array<string> m_Options;
	protected int m_Selected = -1;
	protected string m_Placeholder;
	protected string m_IconName;
	protected bool m_Hover;
	protected bool m_Open;
	protected bool m_WasPressed;
	protected ref ScriptInvoker m_OnSelected;

	protected ref INT_UIF_Surface m_Glow;
	protected ref INT_UIF_Surface m_Body;
	protected ImageWidget m_IconW;
	protected TextWidget m_Label;
	protected ImageWidget m_Chevron;
	protected ButtonWidget m_Hit;

	protected Widget m_Popup;
	protected ref array<ref INT_UIF_Surface> m_RowTints;
	protected ref array<ButtonWidget> m_RowHits;
	protected int m_PopHover = -1;
	// Long lists: the rows sit in a clipping frame and scroll by whole rows
	// (wheel), with a thin indicator of our own. A ScrollWidget is not used:
	// its native scrollbar is a white bar no style reaches, and without it
	// VScrollToPos01 does not move the content (measured in-game).
	protected Widget m_PopContent;
	protected ImageWidget m_PopThumb;
	protected ref array<Widget> m_PopRowWs;
	protected int m_PopFirst;
	protected int m_PopVisible;
	protected float m_PopThumbX;

	void INT_UIF_Dropdown()
	{
		m_Options = new array<string>();
		m_RowTints = new array<ref INT_UIF_Surface>();
		m_RowHits = new array<ButtonWidget>();
		m_PopRowWs = new array<Widget>();
		m_OnSelected = new ScriptInvoker();
	}

	void ~INT_UIF_Dropdown()
	{
		if (m_Popup && GetGame())
			m_Popup.Unlink();
	}

	static INT_UIF_Dropdown Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, array<string> options, int selected = -1, string placeholder = "", float h = 40, string icon = "")
	{
		INT_UIF_Dropdown d = new INT_UIF_Dropdown();
		d.m_Placeholder = placeholder;
		d.m_IconName = icon;
		if (options)
		{
			foreach (string o : options)
			{
				d.m_Options.Insert(o);
			}
		}
		if (selected >= -1 && selected < d.m_Options.Count())
			d.m_Selected = selected;
		if (bag)
			bag.Add(d);
		d.BuildAt(parent, x, y, w, h);
		return d;
	}

	// Closes the open popup, if any. True when one was open (Esc handling).
	static bool CloseActive()
	{
		if (!s_Active || !s_Active.IsOpen())
			return false;
		s_Active.ClosePopup();
		return true;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufDropdown";
	}

	override void OnBuild()
	{
		m_Glow = INT_UIF_Surface.Glow(m_Root, -14, -12, m_W + 28, m_H + 24, 0);
		m_Body = INT_UIF_Surface.Box(m_Root, 0, 0, m_W, m_H, 8, 0, 0);
		float tx = 14;
		if (m_IconName != "")
		{
			m_IconW = INT_UIF_Draw.Icon(m_Root, 14, (m_H - 16) * 0.5, 16, m_IconName, 0);
			tx = 40;
		}
		m_Label = INT_UIF_Draw.Text(m_Root, tx, 0, m_W - tx - 40, m_H, "", 14, 0);
		m_Chevron = INT_UIF_Draw.Icon(m_Root, m_W - 30, (m_H - 14) * 0.5, 14, "chevron_down", 0);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, m_W, m_H, this);
		RefreshLabel();
	}

	override void Destroy()
	{
		ClosePopup();
		super.Destroy();
	}

	// --- field --------------------------------------------------------------------
	protected int FieldBorder()
	{
		if (m_Open)
			return INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 160);
		if (m_Hover)
			return ARGB(50, 255, 255, 255);
		return ARGB(26, 255, 255, 255);
	}

	protected int FieldFill()
	{
		if (m_Hover || m_Open)
			return C(INT_UIF_ThemeKeys.TILE_HOVER);
		return C(INT_UIF_ThemeKeys.TILE);
	}

	protected int FieldGlow()
	{
		if (m_Open)
			return INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 46);
		return INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 0);
	}

	protected void PaintField(bool animate)
	{
		if (!m_Body)
			return;
		if (animate)
		{
			m_Body.FadeFill(FieldFill(), AnimFast());
			m_Body.FadeBorder(FieldBorder(), AnimFast());
			m_Glow.FadeFill(FieldGlow(), AnimNormal());
		}
		else
		{
			m_Body.SetFill(FieldFill());
			m_Body.SetBorder(FieldBorder());
			m_Glow.SetFill(FieldGlow());
		}
		int iconC = C(INT_UIF_ThemeKeys.TEXT_FAINT);
		if (m_Open)
			iconC = C(INT_UIF_ThemeKeys.TEAL);
		if (m_IconW)
			m_IconW.SetColor(iconC);
		if (m_Chevron)
		{
			if (m_Open || m_Hover)
				m_Chevron.SetColor(C(INT_UIF_ThemeKeys.TEXT));
			else
				m_Chevron.SetColor(C(INT_UIF_ThemeKeys.TEXT_MUTED));
		}
	}

	override void ApplyTheme()
	{
		PaintField(false);
		if (m_Label)
		{
			if (m_Selected >= 0)
				m_Label.SetColor(C(INT_UIF_ThemeKeys.TEXT));
			else
				m_Label.SetColor(C(INT_UIF_ThemeKeys.TEXT_FAINT));
		}
		if (m_Root)
		{
			if (m_Enabled)
				m_Root.SetAlpha(1);
			else
				m_Root.SetAlpha(0.42);
		}
	}

	protected void RefreshLabel()
	{
		if (!m_Label)
			return;
		if (m_Selected >= 0 && m_Selected < m_Options.Count())
			m_Label.SetText(m_Options.Get(m_Selected));
		else
			m_Label.SetText(m_Placeholder);
		ApplyTheme();
	}

	// --- API ------------------------------------------------------------------------
	// (int index, string text, INT_UIF_Dropdown dropdown)
	ScriptInvoker OnSelected()
	{
		return m_OnSelected;
	}

	void SetPlaceholder(string text)
	{
		m_Placeholder = text;
		RefreshLabel();
	}

	void SetOptions(array<string> options)
	{
		ClosePopup();
		m_Options.Clear();
		if (options)
		{
			foreach (string o : options)
			{
				m_Options.Insert(o);
			}
		}
		if (m_Selected >= m_Options.Count())
			m_Selected = -1;
		RefreshLabel();
	}

	int Count()
	{
		return m_Options.Count();
	}

	int GetSelected()
	{
		return m_Selected;
	}

	string GetSelectedText()
	{
		if (m_Selected < 0 || m_Selected >= m_Options.Count())
			return "";
		return m_Options.Get(m_Selected);
	}

	void SetSelected(int index, bool notify = false)
	{
		if (index < -1 || index >= m_Options.Count())
			index = -1;
		m_Selected = index;
		RefreshLabel();
		if (notify)
		{
			INT_UIF_Dropdown keep = this;
			m_OnSelected.Invoke(m_Selected, GetSelectedText(), this);
		}
	}

	// What a click on popup row `index` does: close and select with notify.
	void Pick(int index)
	{
		ClosePopup();
		SetSelected(index, true);
		if (m_Label && index >= 0)
		{
			m_Label.SetAlpha(0.3);
			INT_UIF_Animator.Fade(m_Label, 1, 0.24, INT_UIF_Ease.OUT_CUBIC);
		}
	}

	bool IsOpen()
	{
		return m_Open;
	}

	Widget GetPopup()
	{
		return m_Popup;
	}

	int GetPopupRowCount()
	{
		return m_RowHits.Count();
	}

	// --- popup ----------------------------------------------------------------------
	void OpenPopup()
	{
		if (m_Open || !m_Root || !m_Enabled || m_Options.Count() == 0)
			return;
		if (s_Active && s_Active != this)
			s_Active.ClosePopup();

		float fx;
		float fy;
		float fw;
		float fh;
		m_Root.GetScreenPos(fx, fy);
		m_Root.GetScreenSize(fw, fh);
		float factor = INT_UIF_Scale.Factor();
		if (factor <= 0)
			factor = 1;
		int n = m_Options.Count();
		int visible = n;
		if (visible > MAX_VISIBLE)
			visible = MAX_VISIBLE;
		// Design px inside the popup; the popup itself sits in screen px.
		float w = fw / factor;
		float h = visible * ROW_H + PAD * 2;
		float gap = INT_UIF_Draw.S(6);
		float sh = INT_UIF_Draw.S(h);
		float py = fy + fh + gap;
		float dy = -8;
		if (py + sh > INT_UIF_Scale.ScreenH() - INT_UIF_Draw.S(8))
		{
			py = fy - gap - sh;
			dy = 8;
		}

		m_Popup = INT_UIF_Widgets.Create("px_frame.layout", null);
		if (!m_Popup)
			return;
		m_Popup.SetName("IufDropdownPopup");
		m_Popup.SetSort(INT_UIF_SORT_POPUP);
		m_Popup.SetPos(fx, py);
		m_Popup.SetSize(fw, sh);

		INT_UIF_Surface.Shadow(m_Popup, -24, -16, w + 48, h + 44, ARGB(170, 0, 0, 0));
		INT_UIF_Surface.Box(m_Popup, 0, 0, w, h, 10, C(INT_UIF_ThemeKeys.TILE_HOVER), ARGB(34, 255, 255, 255));
		INT_UIF_Draw.Image(m_Popup, 10, 0, w - 20, 1, "iuf_white", ARGB(18, 255, 255, 255));

		Widget rows = m_Popup;
		float rx = PAD;
		float ry = PAD;
		float rw = w - PAD * 2;
		m_PopContent = null;
		m_PopThumb = null;
		m_PopFirst = 0;
		m_PopVisible = visible;
		if (n > MAX_VISIBLE)
		{
			Widget clip = INT_UIF_Draw.Clip(m_Popup, PAD, PAD, rw, visible * ROW_H, "Viewport");
			m_PopContent = INT_UIF_Draw.Frame(clip, 0, 0, rw, n * ROW_H, "Rows");
			if (m_PopContent)
			{
				rows = m_PopContent;
				rx = 0;
				ry = 0;
				rw = rw - 10;
				m_PopThumbX = w - PAD - 3;
				INT_UIF_Draw.Image(m_Popup, m_PopThumbX, PAD + 4, 3, visible * ROW_H - 8, "iuf_white", ARGB(14, 255, 255, 255));
				m_PopThumb = INT_UIF_Draw.Image(m_Popup, m_PopThumbX, PAD + 4, 3, 24, "iuf_white", ARGB(90, 255, 255, 255));
			}
		}

		m_RowTints.Clear();
		m_RowHits.Clear();
		m_PopRowWs.Clear();
		for (int i = 0; i < n; i++)
		{
			BuildRow(rows, i, rx, ry + i * ROW_H, rw);
		}
		m_PopHover = -1;
		// Open on the page that shows the selected row, centred.
		if (m_PopContent)
			ScrollRows(m_Selected - (visible - 1) / 2);
		// The rows on screen cascade in.
		int order = 0;
		for (int k = 0; k < n; k++)
		{
			Widget shownRow = m_PopRowWs.Get(k);
			if (shownRow && shownRow.IsVisible())
			{
				INT_UIF_Animator.Reveal(shownRow, 0.03 + order * 0.025, 4, 0.2);
				order++;
			}
		}

		m_Open = true;
		s_Active = this;
		m_WasPressed = true; // the click that opened it is still down
		INT_UIF_Animator.Reveal(m_Popup, 0, dy, 0.22);
		if (m_Chevron)
			INT_UIF_Animator.Rotate(m_Chevron, 180, 0.26, INT_UIF_Ease.OUT_BACK);
		PaintField(true);
		StartTicking();
	}

	protected void BuildRow(Widget rows, int i, float x, float y, float w)
	{
		Widget row = INT_UIF_Draw.Frame(rows, x, y, w, ROW_H, "Row");
		m_PopRowWs.Insert(row);
		if (!row)
		{
			// keep the per-row arrays index-aligned
			m_RowTints.Insert(null);
			m_RowHits.Insert(null);
			return;
		}
		bool sel = i == m_Selected;
		INT_UIF_Surface tint = INT_UIF_Surface.Box(row, 0, 1, w, ROW_H - 2, 6, RowFill(i), 0);
		m_RowTints.Insert(tint);
		int labelC = C(INT_UIF_ThemeKeys.TEXT);
		if (sel)
			labelC = C(INT_UIF_ThemeKeys.TEAL);
		TextWidget label = INT_UIF_Draw.Text(row, 12, 0, w - 44, ROW_H, m_Options.Get(i), 14, labelC);
		if (label && sel)
			label.SetBold(true);
		if (sel)
			INT_UIF_Draw.Icon(row, w - 28, (ROW_H - 14) * 0.5, 14, "check", C(INT_UIF_ThemeKeys.TEAL));
		ButtonWidget hit = INT_UIF_Draw.Hit(row, 0, 0, w, ROW_H, this);
		m_RowHits.Insert(hit);
	}

	protected int RowFill(int i)
	{
		if (i == m_PopHover)
		{
			if (i == m_Selected)
				return INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 40);
			return ARGB(16, 255, 255, 255);
		}
		if (i == m_Selected)
			return INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 26);
		return ARGB(0, 255, 255, 255);
	}

	void ClosePopup()
	{
		if (m_Popup)
		{
			INT_UIF_Animator.Get().Cancel(m_Popup);
			m_Popup.Unlink();
		}
		m_Popup = null;
		m_PopContent = null;
		m_PopThumb = null;
		m_PopFirst = 0;
		m_PopRowWs.Clear();
		m_RowTints.Clear();
		m_RowHits.Clear();
		m_PopHover = -1;
		bool wasOpen = m_Open;
		m_Open = false;
		if (s_Active == this)
			s_Active = null;
		StopTicking();
		if (wasOpen)
		{
			if (m_Chevron)
				INT_UIF_Animator.Rotate(m_Chevron, 0, 0.22, INT_UIF_Ease.OUT_CUBIC);
			PaintField(true);
		}
	}

	override void OnTick(float dt)
	{
		if (!m_Open)
		{
			StopTicking();
			return;
		}
		// The owner menu closed or the field was hidden: never orphan the popup.
		if (!m_Root || !m_Root.IsVisibleHierarchy())
		{
			ClosePopup();
			return;
		}
		bool pressed = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) != 0;
		if (pressed && !m_WasPressed)
		{
			int mx;
			int my;
			GetMousePos(mx, my);
			if (!INT_UIF_Widgets.Contains(m_Popup, mx, my) && !INT_UIF_Widgets.Contains(m_Root, mx, my))
			{
				ClosePopup();
				return;
			}
		}
		m_WasPressed = pressed;
	}

	// Shows rows first .. first + visible - 1 of a scrolling popup. Rows out
	// of the viewport are hidden, so no clipped row can take a click.
	protected void ScrollRows(int first)
	{
		if (!m_PopContent)
			return;
		int n = m_PopRowWs.Count();
		int maxFirst = n - m_PopVisible;
		if (maxFirst < 0)
			maxFirst = 0;
		if (first > maxFirst)
			first = maxFirst;
		if (first < 0)
			first = 0;
		m_PopFirst = first;
		INT_UIF_Draw.Move(m_PopContent, 0, -first * ROW_H);
		for (int i = 0; i < n; i++)
		{
			Widget row = m_PopRowWs.Get(i);
			if (row)
				row.Show(i >= first && i < first + m_PopVisible);
		}
		// Rows moved under a still cursor: drop the hover tint.
		HoverRow(-1);
		PlaceThumb();
	}

	// 0 = top, 1 = bottom of a scrolling popup (tests, previews).
	void ScrollPopupTo(float pos01)
	{
		int maxFirst = m_PopRowWs.Count() - m_PopVisible;
		if (!m_PopContent || maxFirst <= 0)
			return;
		float target = Math.Clamp(pos01, 0, 1) * maxFirst;
		ScrollRows((int)Math.Round(target));
	}

	int GetPopupFirstRow()
	{
		return m_PopFirst;
	}

	bool IsPopupRowShown(int index)
	{
		if (index < 0 || index >= m_PopRowWs.Count())
			return false;
		Widget row = m_PopRowWs.Get(index);
		return row && row.IsVisible();
	}

	protected void PlaceThumb()
	{
		if (!m_PopContent || !m_PopThumb)
			return;
		int n = m_PopRowWs.Count();
		int maxFirst = n - m_PopVisible;
		if (maxFirst <= 0)
			return;
		float trackH = m_PopVisible * ROW_H - 8;
		float thumbH = trackH * m_PopVisible / n;
		if (thumbH < 24)
			thumbH = 24;
		float k = m_PopFirst;
		k = k / maxFirst;
		INT_UIF_Draw.Resize(m_PopThumb, 3, thumbH);
		INT_UIF_Animator.MoveY(m_PopThumb, INT_UIF_Draw.S(PAD + 4 + k * (trackH - thumbH)), 0.14, INT_UIF_Ease.OUT_CUBIC);
	}

	// --- events ---------------------------------------------------------------------
	protected int RowIndex(Widget w)
	{
		ButtonWidget b = ButtonWidget.Cast(w);
		if (!b)
			return -1;
		return m_RowHits.Find(b);
	}

	protected void HoverRow(int idx)
	{
		int old = m_PopHover;
		m_PopHover = idx;
		if (old >= 0 && old < m_RowTints.Count() && m_RowTints.Get(old))
			m_RowTints.Get(old).FadeFill(RowFill(old), AnimNormal());
		if (idx >= 0 && idx < m_RowTints.Count() && m_RowTints.Get(idx))
			m_RowTints.Get(idx).FadeFill(RowFill(idx), AnimFast());
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
			return false;
		if (w == m_Hit)
		{
			SetFocus(null);
			if (!m_Enabled)
				return true;
			if (m_Open)
				ClosePopup();
			else
				OpenPopup();
			return true;
		}
		int idx = RowIndex(w);
		if (idx < 0)
			return false;
		SetFocus(null);
		Pick(idx);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w == m_Hit)
		{
			m_Hover = true;
			if (m_Enabled)
				PaintField(true);
			TooltipEnter();
			return true;
		}
		int idx = RowIndex(w);
		if (idx < 0)
			return false;
		HoverRow(idx);
		return true;
	}

	// One wheel notch scrolls one row.
	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (!m_PopContent || RowIndex(w) < 0)
			return false;
		ScrollRows(m_PopFirst - wheel);
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w == m_Hit)
		{
			m_Hover = false;
			PaintField(true);
			TooltipLeave();
			return true;
		}
		int idx = RowIndex(w);
		if (idx < 0)
			return false;
		if (m_PopHover == idx)
			HoverRow(-1);
		return true;
	}
}
