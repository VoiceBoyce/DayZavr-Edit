// ============================================================================
// DRL_CraftMenu - сетка карточек рецептов.
//
// Фиксированный пул карточек (cols x rows) над отфильтрованным списком;
// прокрутка колесом сдвигает целые ряды и перепривязывает пул, поэтому
// одновременно живёт не больше cols*rows превью-объектов. Как у INT_Trader.
// Карточка: 3D-превью результата (или иконка рецепта CraftIcon), чип статуса,
// название в две строки, время крафта и количество результата.
// Хит-зоны названы DRL_Card_<slot> для dayz-mcp.
// ============================================================================

const int DRL_UI_STATUS_CAN = 0;      // можно скрафтить сейчас
const int DRL_UI_STATUS_CANNOT = 1;   // не хватает ресурсов / навесного
const int DRL_UI_STATUS_LOCKED = 2;   // рецепт не изучен

class DRL_UIRecipeEntry extends Managed
{
	DRL_Craft_Recipe m_Recipe;
	string m_Name;
	string m_PreviewClass;
	string m_IconPath;   // разрешённый путь CraftIcon ("" — показывать 3D)
	int m_Status;
	int m_TimeSec;
	int m_ResultCount;
	// Необязательно (меню изучения): свой чип статуса и своя подпись внизу
	// вместо «можно/нет ресурсов/не изучен» и времени крафта.
	string m_ChipText;
	string m_ChipIcon;
	int m_ChipHue;
	string m_FootText;
	string m_FootIcon;
}

class DRL_UIRecipeCard extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ref INT_UIF_Surface m_Glow;
	ref DRL_UIItemView m_View;
	ImageWidget m_StageGlow;
	ImageWidget m_Floor;
	ImageWidget m_IconImage;
	Widget m_NoPreview;
	ref DRL_UIChip m_Status;
	ref DRL_UIChip m_Count;
	MultilineTextWidget m_Name;
	ImageWidget m_ClockIcon;
	TextWidget m_Time;
	ButtonWidget m_Hit;
	float m_W;
	float m_H;
	float m_X;
	float m_Y;
	float m_StageH;
	int m_Index = -1;
	bool m_Hover;
	bool m_Selected;
	bool m_HasModel;
	bool m_Many;
	bool m_OverlayShown = true;
	float m_Spin;

	void Build(Widget parent, int slot, float x, float y, float w, float h, ScriptedWidgetEventHandler handler)
	{
		m_W = w;
		m_H = h;
		m_X = x;
		m_Y = y;
		m_StageH = h - 92;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_RecipeCard");
		m_Glow = INT_UIF_Surface.Glow(m_Root, -14, -12, w + 28, h + 24, 0);
		m_Glow.Show(false);
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 12, 0, 0);

		float sw = w - 16;
		float sh = m_StageH;
		m_StageGlow = INT_UIF_Draw.Image(m_Root, (w - 160) * 0.5, 0, 160, sh + 10, "iuf_radial", ARGB(22, 255, 255, 255));
		m_Floor = INT_UIF_Draw.Image(m_Root, (w - 120) * 0.5, sh - 14, 120, 22, "iuf_radial", ARGB(140, 0, 0, 0));
		Widget clip = INT_UIF_Draw.Clip(m_Root, 8, 8, sw, sh, "DRL_CardPreview");
		m_View = new DRL_UIItemView();
		m_View.Create(clip, true);
		m_View.SetOrientation(Vector(0, 0, 0));

		float isz = sh - 30;
		if (isz > 80)
			isz = 80;
		m_IconImage = INT_UIF_Draw.Image(m_Root, (w - isz) * 0.5, 8 + (sh - isz) * 0.5, isz, isz, "iuf_white", ARGB(230, 255, 255, 255));
		m_IconImage.Show(false);

		m_NoPreview = INT_UIF_Draw.Frame(m_Root, 8, 8, sw, sh, "DRL_CardNoPreview");
		INT_UIF_Draw.Icon(m_NoPreview, (sw - 30) * 0.5, sh * 0.5 - 15, 30, "info", ARGB(70, 255, 255, 255));
		m_NoPreview.Show(false);

		// Чипы лежат на превью: поднимаем их над моделью.
		m_Status = new DRL_UIChip();
		m_Status.Build(m_Root, 10, 10, 20, "DRL_CardStatus");
		m_Status.SetSort(DRL_UI_SORT_OVER_MODEL);
		m_Count = new DRL_UIChip();
		m_Count.Build(m_Root, w - 60, sh - 18, 20, "DRL_CardCount");
		m_Count.SetSort(DRL_UI_SORT_OVER_MODEL);
		m_Count.Show(false);

		m_Name = INT_UIF_Draw.MText(m_Root, 12, sh + 16, w - 24, 36, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		INT_UIF_Draw.Line(m_Root, 12, h - 34, w - 24, 1, INT_UIF.Color(INT_UIF_ThemeKeys.DIVIDER));
		m_ClockIcon = INT_UIF_Draw.Icon(m_Root, 12, h - 24, 13, "clock", INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		m_Time = INT_UIF_Draw.Text(m_Root, 31, h - 30, w - 43, 24, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));

		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, "DRL_Card_" + slot.ToString(), handler);
		Paint();
	}

	void Bind(int index, DRL_UIRecipeEntry e)
	{
		m_Index = index;
		if (!e)
		{
			m_Root.Show(false);
			m_View.Clear();
			return;
		}
		m_Root.Show(true);
		bool useIcon = e.m_IconPath != "";
		if (useIcon)
		{
			m_View.Clear();
			m_HasModel = false;
			m_IconImage.LoadImageFile(0, e.m_IconPath);
			m_IconImage.SetImage(0);
		}
		else
		{
			m_HasModel = m_View.SetClass(e.m_PreviewClass);
		}
		m_IconImage.Show(useIcon);
		m_NoPreview.Show(!useIcon && !m_HasModel);
		m_Floor.Show(m_HasModel);
		m_Spin = 0;
		m_View.SetOrientation(Vector(0, 0, 0));
		m_Name.SetText(e.m_Name);
		Update(e);
	}

	void Update(DRL_UIRecipeEntry e)
	{
		if (!e)
			return;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (e.m_ChipText != "")
			m_Status.Set(e.m_ChipText, e.m_ChipIcon, e.m_ChipHue);
		else if (e.m_Status == DRL_UI_STATUS_LOCKED)
			m_Status.Set("#STR_DRL_UI_CHIP_LOCKED", "lock", tm.Color(INT_UIF_ThemeKeys.AMBER));
		else if (e.m_Status == DRL_UI_STATUS_CAN)
			m_Status.Set("#STR_DRL_UI_CHIP_CAN", "check", tm.Color(INT_UIF_ThemeKeys.GREEN));
		else
			m_Status.Set("#STR_DRL_UI_CHIP_CANNOT", "close", tm.Color(INT_UIF_ThemeKeys.RED));

		bool many = e.m_ResultCount > 1;
		m_Many = many;
		m_Count.Show(many && m_OverlayShown);
		if (many)
		{
			float cw = m_Count.Set("x" + e.m_ResultCount.ToString(), "", tm.Color(INT_UIF_ThemeKeys.TEAL));
			INT_UIF_Draw.Move(m_Count.m_Root, m_W - 10 - cw, m_StageH - 16);
		}
		if (e.m_FootText != "")
		{
			m_Time.SetText(e.m_FootText);
			string footIcon = e.m_FootIcon;
			if (footIcon == "")
				footIcon = "info";
			INT_UIF_Widgets.SetIcon(m_ClockIcon, footIcon);
		}
		else
		{
			m_Time.SetText(DRL_UIText.Time(e.m_TimeSec));
			INT_UIF_Widgets.SetIcon(m_ClockIcon, "clock");
		}
		Paint();
	}

	void MoveTo(float x, float y)
	{
		if (x == m_X && y == m_Y)
			return;
		m_X = x;
		m_Y = y;
		DRL_UIAnim.MoveTo(m_Root, x, y);
	}

	void SetHover(bool hover)
	{
		if (m_Hover == hover)
			return;
		m_Hover = hover;
		Paint(true);
	}

	void SetSelected(bool selected)
	{
		if (m_Selected == selected)
			return;
		m_Selected = selected;
		Paint(true);
		if (selected)
		{
			m_Glow.Show(true);
			INT_UIF_Animator.FadeInTo(m_Glow.GetRoot(), 1, 0.2);
		}
		else
		{
			m_Glow.Show(false);
		}
	}

	protected void Paint(bool animate = false)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int fill = tm.Color(INT_UIF_ThemeKeys.TILE);
		int border = tm.Color(INT_UIF_ThemeKeys.TILE_BORDER);
		int glow = tm.Color(INT_UIF_ThemeKeys.TEXT);
		if (m_Hover)
		{
			fill = tm.Color(INT_UIF_ThemeKeys.TILE_HOVER);
			border = INT_UIF.Soft(INT_UIF_ThemeKeys.TEAL, 90);
			glow = tm.Color(INT_UIF_ThemeKeys.TEAL);
		}
		if (m_Selected)
		{
			border = INT_UIF.Soft(INT_UIF_ThemeKeys.AMBER, 200);
			glow = tm.Color(INT_UIF_ThemeKeys.AMBER);
		}
		if (animate)
		{
			m_Bg.FadeFill(fill, 0.14);
			m_Bg.FadeBorder(border, 0.14);
		}
		else
		{
			m_Bg.SetFill(fill);
			m_Bg.SetBorder(border);
		}
		m_Glow.SetFill(INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.AMBER), 70));
		int stageAlpha = 18;
		if (m_Hover || m_Selected)
			stageAlpha = 34;
		m_StageGlow.SetColor(INT_UIF_Color.WithAlpha(glow, stageAlpha));
	}

	// Наведённая/выбранная карточка медленно крутит модель.
	void Tick(float dt)
	{
		if (!m_HasModel)
			return;
		if (m_Hover || m_Selected)
		{
			m_Spin = m_Spin + dt * 60;
			if (m_Spin > 360)
				m_Spin = m_Spin - 360;
			m_View.SetOrientation(Vector(0, m_Spin, 0));
		}
	}

	// Модель и чипы над ней (у чипов повышенный приоритет — они легли бы
	// поверх выезжающей панели).
	void ShowModel(bool show)
	{
		m_OverlayShown = show;
		m_View.Show(show);
		m_Status.Show(show);
		m_Count.Show(show && m_Many);
	}

	void Clear()
	{
		if (m_View)
			m_View.Clear();
		m_Index = -1;
	}
}

class DRL_UIRecipeGrid extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ButtonWidget m_BgHit;
	protected ImageWidget m_Track;
	protected ImageWidget m_Thumb;
	protected Widget m_Empty;
	protected TextWidget m_EmptyText;
	protected MultilineTextWidget m_EmptySub;
	protected ref array<ref DRL_UIRecipeCard> m_Cards;
	protected ref array<ref DRL_UIRecipeEntry> m_Items;
	protected int m_Cols;
	protected int m_Rows;
	protected float m_CardW;
	protected float m_CardH;
	protected float m_Gap;
	protected float m_W;
	protected float m_H;
	protected int m_FirstRow;
	protected int m_Selected = -1;
	protected int m_HoverSlot = -1;
	protected bool m_ModelsHidden;
	protected ref ScriptInvoker m_OnSelected;

	void DRL_UIRecipeGrid()
	{
		m_Cards = new array<ref DRL_UIRecipeCard>();
		m_Items = new array<ref DRL_UIRecipeEntry>();
		m_OnSelected = new ScriptInvoker();
	}

	void ~DRL_UIRecipeGrid()
	{
		ClearModels();
	}

	// (int index, DRL_UIRecipeGrid grid)
	ScriptInvoker OnSelected()
	{
		return m_OnSelected;
	}

	void Build(Widget parent, float x, float y, float w, float h, int cols, int rows, float gap)
	{
		m_W = w;
		m_H = h;
		m_Cols = cols;
		m_Rows = rows;
		m_Gap = gap;
		m_CardW = (w - 14 - (cols - 1) * gap) / cols;
		m_CardH = (h - (rows - 1) * gap) / rows;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_RecipeGrid");
		m_BgHit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, this);
		m_BgHit.SetName("DRL_GridBg");

		int slot = 0;
		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				DRL_UIRecipeCard card = new DRL_UIRecipeCard();
				card.Build(m_Root, slot, c * (m_CardW + gap), r * (m_CardH + gap), m_CardW, m_CardH, this);
				card.m_Root.Show(false);
				m_Cards.Insert(card);
				slot++;
			}
		}

		m_Track = INT_UIF_Draw.Line(m_Root, w - 4, 0, 4, h, ARGB(14, 255, 255, 255));
		m_Thumb = INT_UIF_Draw.Line(m_Root, w - 4, 0, 4, 60, INT_UIF.Soft(INT_UIF_ThemeKeys.TEAL, 170));

		m_Empty = INT_UIF_Draw.Frame(m_Root, 0, 0, w - 14, h, "DRL_GridEmpty");
		INT_UIF_Draw.Image(m_Empty, (w - 14) * 0.5 - 120, h * 0.5 - 150, 240, 240, "iuf_radial", ARGB(16, 255, 255, 255));
		INT_UIF_Draw.Icon(m_Empty, (w - 14) * 0.5 - 24, h * 0.5 - 64, 48, "search", INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		m_EmptyText = INT_UIF_Draw.Text(m_Empty, 0, h * 0.5 + 2, w - 14, 24, "#STR_DRL_UI_EMPTY", 17, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED), INT_UIF_Align.CENTER);
		m_EmptySub = INT_UIF_Draw.MText(m_Empty, (w - 14) * 0.5 - 220, h * 0.5 + 34, 440, 40, "#STR_DRL_UI_EMPTY_SUB", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		m_Empty.Show(false);
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	void SetItems(array<ref DRL_UIRecipeEntry> items, DRL_Craft_Recipe keep = null, bool reveal = true)
	{
		m_Items.Clear();
		foreach (DRL_UIRecipeEntry it : items)
		{
			m_Items.Insert(it);
		}
		m_FirstRow = 0;
		m_Selected = -1;
		if (keep)
		{
			int idx = IndexOfRecipe(keep);
			if (idx >= 0)
			{
				m_Selected = idx;
				EnsureVisible(idx);
			}
		}
		Rebind();
		m_Empty.Show(m_Items.Count() == 0);
		if (m_Items.Count() == 0)
			DRL_UIAnim.Reveal(m_Empty, 0, 0, 0.05, 10, 0.3);
		if (reveal)
			RevealCards();
	}

	int Count()
	{
		return m_Items.Count();
	}

	DRL_UIRecipeEntry GetItem(int index)
	{
		if (index < 0 || index >= m_Items.Count())
			return null;
		return m_Items.Get(index);
	}

	int IndexOfRecipe(DRL_Craft_Recipe r)
	{
		int n = m_Items.Count();
		for (int i = 0; i < n; i++)
		{
			if (m_Items.Get(i).m_Recipe == r)
				return i;
		}
		return -1;
	}

	int GetSelected()
	{
		return m_Selected;
	}

	void Select(int index, bool notify)
	{
		if (index >= m_Items.Count())
			index = m_Items.Count() - 1;
		m_Selected = index;
		if (index >= 0)
			EnsureVisible(index);
		Rebind();
		if (notify)
			m_OnSelected.Invoke(index, this);
	}

	// Перерисовать изменяемые значения (статусы после изменения инвентаря).
	void RefreshValues()
	{
		foreach (DRL_UIRecipeCard card : m_Cards)
		{
			if (card.m_Index >= 0)
				card.Update(GetItem(card.m_Index));
		}
	}

	protected int RowCount()
	{
		int n = m_Items.Count();
		return (n + m_Cols - 1) / m_Cols;
	}

	protected int MaxFirstRow()
	{
		int m = RowCount() - m_Rows;
		if (m < 0)
			m = 0;
		return m;
	}

	void ScrollTo(int firstRow, bool animate = true)
	{
		int target = Math.Clamp(firstRow, 0, MaxFirstRow());
		if (target == m_FirstRow)
			return;
		int dir = target - m_FirstRow;
		m_FirstRow = target;
		Rebind();
		if (!animate)
			return;
		float dy = 14;
		if (dir < 0)
			dy = -14;
		foreach (DRL_UIRecipeCard card : m_Cards)
		{
			if (card.m_Index >= 0)
				DRL_UIAnim.Reveal(card.m_Root, card.m_X, card.m_Y, 0, dy, 0.2);
		}
	}

	void EnsureVisible(int index)
	{
		if (index < 0)
			return;
		int row = index / m_Cols;
		if (row < m_FirstRow)
			m_FirstRow = row;
		else if (row >= m_FirstRow + m_Rows)
			m_FirstRow = row - m_Rows + 1;
		m_FirstRow = Math.Clamp(m_FirstRow, 0, MaxFirstRow());
	}

	protected void Rebind()
	{
		int nCards = m_Cards.Count();
		for (int i = 0; i < nCards; i++)
		{
			DRL_UIRecipeCard card = m_Cards.Get(i);
			int r = i / m_Cols;
			int c = i - r * m_Cols;
			int idx = (m_FirstRow + r) * m_Cols + c;
			card.MoveTo(c * (m_CardW + m_Gap), r * (m_CardH + m_Gap));
			if (idx < m_Items.Count())
			{
				card.Bind(idx, m_Items.Get(idx));
				card.SetSelected(idx == m_Selected);
				card.ShowModel(!m_ModelsHidden);
			}
			else
			{
				card.Bind(-1, null);
				card.SetSelected(false);
			}
			card.SetHover(i == m_HoverSlot && card.m_Index >= 0);
		}
		UpdateThumb();
	}

	protected void RevealCards()
	{
		float stepX = m_CardW + m_Gap;
		float stepY = m_CardH + m_Gap;
		foreach (DRL_UIRecipeCard card : m_Cards)
		{
			if (card.m_Index < 0)
				continue;
			float rank = card.m_X / stepX + card.m_Y / stepY;
			DRL_UIAnim.Reveal(card.m_Root, card.m_X, card.m_Y, 0.02 + rank * 0.035, 12, 0.3);
		}
	}

	protected void UpdateThumb()
	{
		bool scrollable = MaxFirstRow() > 0;
		m_Track.Show(scrollable);
		m_Thumb.Show(scrollable);
		if (!scrollable)
			return;
		float th = m_H * m_Rows / RowCount();
		if (th < 40)
			th = 40;
		float ty = (m_H - th) * m_FirstRow / MaxFirstRow();
		INT_UIF_Draw.Place(m_Thumb, m_W - 4, ty, 4, th);
	}

	// Пока оверлей перекрывает сетку, 3D-модели прячутся (рисуются поверх 2D).
	void HideModels(bool hide)
	{
		m_ModelsHidden = hide;
		foreach (DRL_UIRecipeCard card : m_Cards)
		{
			if (card.m_Index >= 0)
				card.ShowModel(!hide);
		}
	}

	void Tick(float dt)
	{
		foreach (DRL_UIRecipeCard card : m_Cards)
		{
			if (card.m_Index >= 0)
				card.Tick(dt);
		}
	}

	void ClearModels()
	{
		foreach (DRL_UIRecipeCard card : m_Cards)
		{
			if (card)
				card.Clear();
		}
	}

	// Тестовый хук: наведение без мыши.
	void PinHover(int slot)
	{
		if (slot < 0 || slot >= m_Cards.Count())
			return;
		m_HoverSlot = slot;
		m_Cards.Get(slot).SetHover(true);
	}

	// --- события ------------------------------------------------------------------
	protected int SlotOf(Widget w)
	{
		int n = m_Cards.Count();
		for (int i = 0; i < n; i++)
		{
			if (m_Cards.Get(i).m_Hit == w)
				return i;
		}
		return -1;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int slot = SlotOf(w);
		if (slot < 0)
		{
			// Клик мимо карточек снимает выбор (как в старом меню).
			if (w != m_BgHit)
				return false;
			if (m_Selected >= 0 && button == MouseState.LEFT)
				Select(-1, true);
			return true;
		}
		int idx = m_Cards.Get(slot).m_Index;
		if (idx < 0)
			return true;
		if (idx != m_Selected)
			Select(idx, true);
		return true;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (w != m_BgHit && SlotOf(w) < 0)
			return false;
		ScrollTo(m_FirstRow - wheel);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int slot = SlotOf(w);
		if (slot < 0)
			return false;
		m_HoverSlot = slot;
		DRL_UIRecipeCard card = m_Cards.Get(slot);
		card.SetHover(card.m_Index >= 0);
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int slot = SlotOf(w);
		if (slot < 0)
			return false;
		if (m_HoverSlot == slot)
			m_HoverSlot = -1;
		m_Cards.Get(slot).SetHover(false);
		return true;
	}
}
