// ============================================================================
// INT_UIFramework - Tabs (v2).
//
//   INT_UIF_Tabs tabs = INT_UIF_Tabs.Create(m_Bag, parent, x, y, 360, 40);
//   tabs.SetPageArea(body, 0, 0, bodyW, bodyH);    // optional: one page per tab
//   tabs.AddTab("#STR_X_OVERVIEW", "home");
//   tabs.AddTab("#STR_X_FORMS", "list", 3);       // 3 = count chip
//   Widget forms = tabs.GetPage(1);                 // build the page into it
//   tabs.OnChanged().Insert(OnTab);                 // void OnTab(int index, INT_UIF_Tabs tabs)
//
// SEGMENTED: a tile track with a raised thumb that slides (with a little
// overshoot) behind the active tab. UNDERLINE: bare labels over a hairline
// with a glowing teal bar that slides and stretches to the active tab. The
// active label turns bright and bold, its icon teal. With a page area the
// pages swap with a short slide in the direction of travel.
// ============================================================================

enum INT_UIF_TabStyle
{
	SEGMENTED,
	UNDERLINE
}

class INT_UIF_Tabs extends INT_UIF_Component
{
	static const float INSET = 4;     // SEGMENTED: track padding around the thumb
	static const float GAP = 6;       // UNDERLINE: space between tabs

	protected int m_Style;
	protected int m_Active = -1;
	protected int m_HoverIdx = -1;
	protected ref ScriptInvoker m_OnChanged;

	protected ref array<string> m_Titles;
	protected ref array<string> m_Icons;
	protected ref array<int> m_Counts;
	protected ref array<float> m_TabX;
	protected ref array<float> m_TabW;
	protected ref array<TextWidget> m_Labels;
	protected ref array<ImageWidget> m_IconWs;
	protected ref array<ref INT_UIF_Surface> m_Chips;
	protected ref array<TextWidget> m_ChipTexts;
	protected ref array<ref INT_UIF_Surface> m_Tints;
	protected ref array<ButtonWidget> m_Hits;
	protected ref array<Widget> m_Pages;

	protected ref INT_UIF_Surface m_Track;
	protected ref INT_UIF_Surface m_Thumb;
	protected ref INT_UIF_Surface m_LineGlow;
	protected ImageWidget m_Base;
	protected ImageWidget m_Line;
	protected Widget m_Items;

	protected Widget m_PageHost;
	protected float m_PageX;
	protected float m_PageY;
	protected float m_PageW;
	protected float m_PageH;

	void INT_UIF_Tabs()
	{
		m_OnChanged = new ScriptInvoker();
		m_Titles = new array<string>();
		m_Icons = new array<string>();
		m_Counts = new array<int>();
		m_TabX = new array<float>();
		m_TabW = new array<float>();
		m_Labels = new array<TextWidget>();
		m_IconWs = new array<ImageWidget>();
		m_Chips = new array<ref INT_UIF_Surface>();
		m_ChipTexts = new array<TextWidget>();
		m_Tints = new array<ref INT_UIF_Surface>();
		m_Hits = new array<ButtonWidget>();
		m_Pages = new array<Widget>();
	}

	static INT_UIF_Tabs Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float h = 40, int style = INT_UIF_TabStyle.SEGMENTED)
	{
		INT_UIF_Tabs t = new INT_UIF_Tabs();
		t.m_Style = style;
		if (bag)
			bag.Add(t);
		t.BuildAt(parent, x, y, w, h);
		return t;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufTabs";
	}

	override void OnBuild()
	{
		if (m_Style == INT_UIF_TabStyle.SEGMENTED)
		{
			m_Track = INT_UIF_Surface.Box(m_Root, 0, 0, m_W, m_H, 10, 0, 0);
			m_Thumb = INT_UIF_Surface.Box(m_Root, INSET, INSET, 10, m_H - INSET * 2, 8, 0, 0);
			if (m_Thumb)
				m_Thumb.Show(false);
		}
		else
		{
			m_Base = INT_UIF_Draw.Image(m_Root, 0, m_H - 1, m_W, 1, "iuf_white", 0);
			m_LineGlow = INT_UIF_Surface.Glow(m_Root, 0, m_H - 14, 10, 26, 0);
			m_Line = INT_UIF_Draw.Image(m_Root, 0, m_H - 2, 10, 2, "iuf_white", 0);
			if (m_LineGlow)
				m_LineGlow.Show(false);
			if (m_Line)
				m_Line.Show(false);
		}
		m_Items = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, m_H, "Items");
	}

	override void ApplyTheme()
	{
		if (m_Track)
		{
			m_Track.SetFill(C(INT_UIF_ThemeKeys.TILE));
			m_Track.SetBorder(ARGB(20, 255, 255, 255));
		}
		if (m_Thumb)
		{
			m_Thumb.SetFill(ThumbFill());
			m_Thumb.SetBorder(ARGB(34, 255, 255, 255));
		}
		if (m_Base)
			m_Base.SetColor(ARGB(20, 255, 255, 255));
		if (m_Line)
			m_Line.SetColor(C(INT_UIF_ThemeKeys.TEAL));
		if (m_LineGlow)
			m_LineGlow.SetFill(INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 80));
		int n = m_Titles.Count();
		for (int i = 0; i < n; i++)
		{
			PaintTab(i, false);
		}
		if (m_Root)
		{
			if (m_Enabled)
				m_Root.SetAlpha(1);
			else
				m_Root.SetAlpha(0.42);
		}
	}

	protected int ThumbFill()
	{
		return INT_UIF_Color.Lerp(C(INT_UIF_ThemeKeys.TILE_HOVER), ARGB(255, 255, 255, 255), 0.05);
	}

	// --- API ------------------------------------------------------------------------
	// (int index, INT_UIF_Tabs tabs)
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	// Pages: every AddTab after this creates a page frame at this rect in host
	// (design px); only the active one is shown.
	void SetPageArea(Widget host, float x, float y, float w, float h)
	{
		m_PageHost = host;
		m_PageX = x;
		m_PageY = y;
		m_PageW = w;
		m_PageH = h;
	}

	// Returns the tab index. count < 0 = no count chip.
	int AddTab(string title, string icon = "", int count = -1)
	{
		m_Titles.Insert(title);
		m_Icons.Insert(icon);
		m_Counts.Insert(count);
		Widget page = null;
		if (m_PageHost)
		{
			page = INT_UIF_Draw.Frame(m_PageHost, m_PageX, m_PageY, m_PageW, m_PageH, "Page");
			if (page)
				page.Show(false);
		}
		m_Pages.Insert(page);
		Rebuild();
		int idx = m_Titles.Count() - 1;
		if (m_Active < 0)
			Select(0, false, false);
		else
			PlaceIndicator(false);
		return idx;
	}

	Widget GetPage(int index)
	{
		if (index < 0 || index >= m_Pages.Count())
			return null;
		return m_Pages.Get(index);
	}

	int Count()
	{
		return m_Titles.Count();
	}

	int GetActive()
	{
		return m_Active;
	}

	void SetTitle(int index, string title)
	{
		if (index < 0 || index >= m_Titles.Count())
			return;
		m_Titles.Set(index, title);
		Rebuild();
		PlaceIndicator(false);
	}

	void SetCount(int index, int count)
	{
		if (index < 0 || index >= m_Counts.Count())
			return;
		bool relayout = (count < 0) != (m_Counts.Get(index) < 0);
		m_Counts.Set(index, count);
		if (relayout || m_Style == INT_UIF_TabStyle.UNDERLINE)
		{
			Rebuild();
			PlaceIndicator(false);
			return;
		}
		TextWidget ct = m_ChipTexts.Get(index);
		if (ct)
		{
			ct.SetText(count.ToString());
			ct.SetAlpha(0.3);
			INT_UIF_Animator.Fade(ct, 1, 0.24, INT_UIF_Ease.OUT_CUBIC);
		}
	}

	void Select(int index, bool notify = true, bool animate = true)
	{
		int n = m_Titles.Count();
		if (index < 0 || index >= n || index == m_Active)
			return;
		int old = m_Active;
		m_Active = index;
		PlaceIndicator(animate);
		for (int i = 0; i < n; i++)
		{
			PaintTab(i, animate);
		}
		SwapPages(old, index, animate);
		if (notify)
		{
			INT_UIF_Tabs keep = this;
			m_OnChanged.Invoke(m_Active, this);
		}
	}

	// --- layout ---------------------------------------------------------------------
	protected float ContentWidth(int i)
	{
		// 1.14: the active label is bold, ~8 % wider than the Book estimate.
		float cw = INT_UIF_Draw.TextWidth(m_Titles.Get(i), 14) * 1.14;
		if (m_Icons.Get(i) != "")
			cw = cw + 24;
		int cnt = m_Counts.Get(i);
		if (cnt >= 0)
			cw = cw + 8 + ChipWidth(cnt);
		return cw;
	}

	protected float ChipWidth(int count)
	{
		string s = count.ToString();
		float cw = INT_UIF_Draw.TextWidth(s, 11) * 1.1 + 14;
		if (cw < 20)
			cw = 20;
		return cw;
	}

	// Recomputes every tab rect and rebuilds the tab widgets (cheap: a few
	// widgets per tab; called on AddTab / SetTitle / chip changes).
	protected void Rebuild()
	{
		if (!m_Items)
			return;
		INT_UIF_Widgets.ClearChildren(m_Items);
		m_TabX.Clear();
		m_TabW.Clear();
		m_Labels.Clear();
		m_IconWs.Clear();
		m_Chips.Clear();
		m_ChipTexts.Clear();
		m_Tints.Clear();
		m_Hits.Clear();
		int n = m_Titles.Count();
		if (n == 0)
			return;
		float segW = (m_W - INSET * 2) / n;
		float cx = 0;
		for (int i = 0; i < n; i++)
		{
			float tx;
			float tw;
			if (m_Style == INT_UIF_TabStyle.SEGMENTED)
			{
				tx = INSET + i * segW;
				tw = segW;
			}
			else
			{
				tx = cx;
				tw = ContentWidth(i) + 28;
				cx = cx + tw + GAP;
			}
			m_TabX.Insert(tx);
			m_TabW.Insert(tw);
			BuildTab(i, tx, tw);
		}
	}

	protected void BuildTab(int i, float tx, float tw)
	{
		float ty = 0;
		float th = m_H;
		if (m_Style == INT_UIF_TabStyle.SEGMENTED)
		{
			ty = INSET;
			th = m_H - INSET * 2;
		}
		INT_UIF_Surface tint = null;
		if (m_Style == INT_UIF_TabStyle.SEGMENTED)
			tint = INT_UIF_Surface.Box(m_Items, tx, ty, tw, th, 8, ARGB(0, 255, 255, 255), 0);
		m_Tints.Insert(tint);

		float cw = ContentWidth(i);
		float x0 = tx + (tw - cw) * 0.5;
		if (x0 < tx + 8)
			x0 = tx + 8;
		ImageWidget iw = null;
		string icon = m_Icons.Get(i);
		if (icon != "")
		{
			iw = INT_UIF_Draw.Icon(m_Items, x0, ty + (th - 16) * 0.5, 16, icon, 0);
			x0 = x0 + 24;
		}
		m_IconWs.Insert(iw);
		float lw = INT_UIF_Draw.TextWidth(m_Titles.Get(i), 14) * 1.14 + 8;
		TextWidget label = INT_UIF_Draw.Text(m_Items, x0, ty, lw, th, m_Titles.Get(i), 14, 0);
		m_Labels.Insert(label);
		x0 = x0 + lw + 2;

		INT_UIF_Surface chip = null;
		TextWidget chipText = null;
		int cnt = m_Counts.Get(i);
		if (cnt >= 0)
		{
			float chw = ChipWidth(cnt);
			float chy = ty + (th - 18) * 0.5;
			chip = INT_UIF_Surface.Pill(m_Items, x0, chy, chw, 18, 0, 0);
			chipText = INT_UIF_Draw.Text(m_Items, x0, chy, chw, 18, cnt.ToString(), 11, 0, INT_UIF_Align.CENTER);
			if (chipText)
				chipText.SetBold(true);
		}
		m_Chips.Insert(chip);
		m_ChipTexts.Insert(chipText);

		ButtonWidget hit = INT_UIF_Draw.Hit(m_Items, tx, 0, tw, m_H, this);
		m_Hits.Insert(hit);
		PaintTab(i, false);
	}

	protected void PlaceIndicator(bool animate)
	{
		if (m_Active < 0 || m_Active >= m_TabX.Count())
			return;
		float tx = m_TabX.Get(m_Active);
		float tw = m_TabW.Get(m_Active);
		if (m_Style == INT_UIF_TabStyle.SEGMENTED)
		{
			if (!m_Thumb)
				return;
			Widget thumb = m_Thumb.GetRoot();
			if (animate && thumb.IsVisible())
			{
				INT_UIF_Animator.MoveX(thumb, INT_UIF_Draw.S(tx), 0.36, INT_UIF_Ease.OUT_BACK);
				INT_UIF_Animator.ResizeW(thumb, INT_UIF_Draw.S(tw), 0.3, INT_UIF_Ease.OUT_CUBIC);
			}
			else
			{
				INT_UIF_Draw.Place(thumb, tx, INSET, tw, m_H - INSET * 2);
				thumb.Show(true);
			}
			return;
		}
		if (!m_Line || !m_LineGlow)
			return;
		float lx = tx + 12;
		float lw = tw - 24;
		Widget glow = m_LineGlow.GetRoot();
		if (animate && m_Line.IsVisible())
		{
			INT_UIF_Animator.MoveX(m_Line, INT_UIF_Draw.S(lx), 0.34, INT_UIF_Ease.OUT_BACK);
			INT_UIF_Animator.ResizeW(m_Line, INT_UIF_Draw.S(lw), 0.28, INT_UIF_Ease.OUT_CUBIC);
			INT_UIF_Animator.MoveX(glow, INT_UIF_Draw.S(lx - 12), 0.34, INT_UIF_Ease.OUT_BACK);
			INT_UIF_Animator.ResizeW(glow, INT_UIF_Draw.S(lw + 24), 0.28, INT_UIF_Ease.OUT_CUBIC);
		}
		else
		{
			INT_UIF_Draw.Place(m_Line, lx, m_H - 2, lw, 2);
			INT_UIF_Draw.Place(glow, lx - 12, m_H - 14, lw + 24, 26);
			m_Line.Show(true);
			glow.Show(true);
		}
	}

	protected void PaintTab(int i, bool animate)
	{
		if (i < 0 || i >= m_Labels.Count())
			return;
		bool active = i == m_Active;
		bool hover = i == m_HoverIdx && m_Enabled;
		int labelC = C(INT_UIF_ThemeKeys.TEXT_MUTED);
		if (active || hover)
			labelC = C(INT_UIF_ThemeKeys.TEXT);
		int iconC = C(INT_UIF_ThemeKeys.TEXT_FAINT);
		if (active)
			iconC = C(INT_UIF_ThemeKeys.TEAL);
		else if (hover)
			iconC = C(INT_UIF_ThemeKeys.TEXT_MUTED);
		int chipFill = ARGB(20, 255, 255, 255);
		int chipC = C(INT_UIF_ThemeKeys.TEXT_MUTED);
		if (active)
		{
			chipFill = INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 40);
			chipC = C(INT_UIF_ThemeKeys.TEAL);
		}
		int tintC = ARGB(0, 255, 255, 255);
		if (hover && !active)
			tintC = ARGB(10, 255, 255, 255);

		TextWidget label = m_Labels.Get(i);
		ImageWidget iw = m_IconWs.Get(i);
		INT_UIF_Surface chip = m_Chips.Get(i);
		TextWidget chipText = m_ChipTexts.Get(i);
		INT_UIF_Surface tint = m_Tints.Get(i);
		if (label)
			label.SetBold(active);
		if (animate)
		{
			float d = AnimNormal();
			if (label)
				INT_UIF_Animator.ColorTo(label, labelC, d);
			if (iw)
				INT_UIF_Animator.ColorTo(iw, iconC, d);
			if (chip)
				chip.FadeFill(chipFill, d);
			if (chipText)
				INT_UIF_Animator.ColorTo(chipText, chipC, d);
			if (tint)
				tint.FadeFill(tintC, AnimFast());
			return;
		}
		if (label)
			label.SetColor(labelC);
		if (iw)
			iw.SetColor(iconC);
		if (chip)
			chip.SetFill(chipFill);
		if (chipText)
			chipText.SetColor(chipC);
		if (tint)
			tint.SetFill(tintC);
	}

	protected void SwapPages(int from, int to, bool animate)
	{
		Widget oldPage = GetPage(from);
		Widget newPage = GetPage(to);
		if (oldPage)
		{
			INT_UIF_Animator.Get().Cancel(oldPage);
			oldPage.Show(false);
		}
		if (!newPage)
			return;
		INT_UIF_Animator.Get().Cancel(newPage);
		if (!animate || from < 0)
		{
			INT_UIF_Draw.Move(newPage, m_PageX, m_PageY);
			newPage.SetAlpha(1);
			newPage.Show(true);
			return;
		}
		float dir = 1;
		if (to < from)
			dir = -1;
		INT_UIF_Draw.Move(newPage, m_PageX + 28 * dir, m_PageY);
		newPage.SetAlpha(0);
		newPage.Show(true);
		INT_UIF_Animator.MoveX(newPage, INT_UIF_Draw.S(m_PageX), 0.34, INT_UIF_Ease.OUT_CUBIC);
		INT_UIF_Animator.Fade(newPage, 1, 0.26, INT_UIF_Ease.OUT_CUBIC);
	}

	// --- events ---------------------------------------------------------------------
	protected int HitIndex(Widget w)
	{
		ButtonWidget b = ButtonWidget.Cast(w);
		if (!b)
			return -1;
		return m_Hits.Find(b);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
			return false;
		int idx = HitIndex(w);
		if (idx < 0)
			return false;
		SetFocus(null);
		if (m_Enabled)
			Select(idx, true, true);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int idx = HitIndex(w);
		if (idx < 0)
			return false;
		int old = m_HoverIdx;
		m_HoverIdx = idx;
		PaintTab(old, true);
		PaintTab(idx, true);
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int idx = HitIndex(w);
		if (idx < 0)
			return false;
		if (m_HoverIdx == idx)
			m_HoverIdx = -1;
		PaintTab(idx, true);
		return true;
	}
}
