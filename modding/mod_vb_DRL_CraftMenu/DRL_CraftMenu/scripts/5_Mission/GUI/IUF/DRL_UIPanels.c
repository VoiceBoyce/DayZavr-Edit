// ============================================================================
// DRL_CraftMenu - панели нового окна крафта:
//   DRL_UIVariantStrip  плитки вариантов результата (ResultClasses);
//   DRL_UIHoverCard     подсказка ингредиента поверх сцены превью;
//   DRL_UIPopover       выезжающая панель (вариации ингредиента, требования);
//   DRL_UIProc          процесс крафта на клиенте (как DRL_CraftProcessEntryWidget);
//   DRL_UIQueueBar      очередь процессов в шапке окна.
// ============================================================================

// --- варианты результата ------------------------------------------------------------
class DRL_UIVariantTile extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ref DRL_UIItemView m_View;
	ImageWidget m_Glyph;
	TextWidget m_Num;
	ButtonWidget m_Hit;
	int m_Index = -1;
	bool m_Hover;
	bool m_Selected;

	void Build(Widget parent, int slot, float x, float s, ScriptedWidgetEventHandler handler)
	{
		m_Root = INT_UIF_Draw.Frame(parent, x, 0, s, s, "DRL_VarTile");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, s, s, 8, 0, 0);
		Widget clip = INT_UIF_Draw.Clip(m_Root, 3, 3, s - 6, s - 6, "DRL_VarClip");
		m_View = new DRL_UIItemView();
		m_View.Create(clip, true);
		m_Glyph = INT_UIF_Draw.Icon(m_Root, (s - 18) * 0.5, (s - 18) * 0.5, 18, "info", INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		m_Glyph.Show(false);
		m_Num = INT_UIF_Draw.Text(m_Root, 4, s - 16, 20, 14, "", 10, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		m_Num.SetSort(DRL_UI_SORT_OVER_MODEL);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, s, s, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, "DRL_Var_" + slot.ToString(), handler);
	}

	void Bind(int index, string cls)
	{
		m_Index = index;
		if (index < 0)
		{
			m_Root.Show(false);
			m_View.Clear();
			return;
		}
		m_Root.Show(true);
		bool ok = m_View.SetClass(cls);
		m_Glyph.Show(!ok);
		int num = index + 1;
		m_Num.SetText(num.ToString());
		Paint();
	}

	void Paint()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int fill = tm.Color(INT_UIF_ThemeKeys.TILE);
		int border = tm.Color(INT_UIF_ThemeKeys.TILE_BORDER);
		if (m_Selected)
		{
			fill = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.AMBER), 40);
			border = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.AMBER), 220);
		}
		else if (m_Hover)
		{
			fill = tm.Color(INT_UIF_ThemeKeys.TILE_HOVER);
			border = INT_UIF.Soft(INT_UIF_ThemeKeys.TEAL, 90);
		}
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
	}
}

class DRL_UIVariantStrip extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected TextWidget m_More;
	protected ref array<ref DRL_UIVariantTile> m_Tiles;
	protected ref array<string> m_Classes;
	protected int m_First;
	protected int m_Selected = -1;
	protected float m_Tile;
	protected ref ScriptInvoker m_OnSelected;

	void DRL_UIVariantStrip()
	{
		m_Tiles = new array<ref DRL_UIVariantTile>();
		m_Classes = new array<string>();
		m_OnSelected = new ScriptInvoker();
	}

	void ~DRL_UIVariantStrip()
	{
		Clear();
	}

	// (int index, DRL_UIVariantStrip strip)
	ScriptInvoker OnSelected()
	{
		return m_OnSelected;
	}

	// Высота: подпись 20 + плитка.
	void Build(Widget parent, float x, float y, float w, float tile)
	{
		m_Tile = tile;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, tile + 20, "DRL_Variants");
		INT_UIF_Draw.Caption(m_Root, 2, 0, w - 80, "#STR_DRL_UI_VARIANTS", "swap");
		m_More = INT_UIF_Draw.Text(m_Root, w - 120, 0, 118, 14, "", 11, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT), INT_UIF_Align.RIGHT);
		Widget row = INT_UIF_Draw.Frame(m_Root, 0, 20, w, tile, "DRL_VariantsRow");
		int count = (int)((w + 8) / (tile + 8));
		for (int i = 0; i < count; i++)
		{
			DRL_UIVariantTile t = new DRL_UIVariantTile();
			t.Build(row, i, i * (tile + 8), tile, this);
			t.m_Root.Show(false);
			m_Tiles.Insert(t);
		}
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	void Show(bool show)
	{
		m_Root.Show(show);
	}

	void SetItems(array<string> classes, int selected)
	{
		m_Classes.Clear();
		if (classes)
		{
			foreach (string c : classes)
			{
				m_Classes.Insert(c);
			}
		}
		m_Selected = selected;
		m_First = 0;
		if (m_Selected >= m_Tiles.Count())
			m_First = m_Selected - m_Tiles.Count() + 1;
		Rebind();
	}

	void SetSelected(int index)
	{
		m_Selected = index;
		Rebind();
	}

	int Count()
	{
		return m_Classes.Count();
	}

	protected int MaxFirst()
	{
		int m = m_Classes.Count() - m_Tiles.Count();
		if (m < 0)
			m = 0;
		return m;
	}

	protected void Rebind()
	{
		for (int i = 0; i < m_Tiles.Count(); i++)
		{
			DRL_UIVariantTile t = m_Tiles.Get(i);
			int idx = m_First + i;
			if (idx < m_Classes.Count())
			{
				t.m_Selected = idx == m_Selected;
				t.Bind(idx, m_Classes.Get(idx));
			}
			else
			{
				t.Bind(-1, "");
			}
		}
		// Больше, чем помещается: подсказка и прокрутка колесом.
		int hidden = m_Classes.Count() - m_Tiles.Count();
		if (hidden > 0)
			m_More.SetText(DRL_UIText.Fmt("#STR_DRL_UI_VARIANTS_MORE", m_Classes.Count().ToString()));
		else
			m_More.SetText("");
	}

	void HideModels(bool hide)
	{
		foreach (DRL_UIVariantTile t : m_Tiles)
		{
			if (t.m_Index >= 0)
				t.m_View.Show(!hide);
		}
	}

	void Clear()
	{
		foreach (DRL_UIVariantTile t : m_Tiles)
		{
			if (t)
				t.m_View.Clear();
		}
	}

	protected int SlotOf(Widget w)
	{
		for (int i = 0; i < m_Tiles.Count(); i++)
		{
			if (m_Tiles.Get(i).m_Hit == w)
				return i;
		}
		return -1;
	}

	void TestSelect(int index)
	{
		if (index < 0 || index >= m_Classes.Count())
			return;
		DRL_UIVariantStrip keep = this;
		m_OnSelected.Invoke(index, this);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int slot = SlotOf(w);
		if (slot < 0 || button != MouseState.LEFT)
			return false;
		int idx = m_Tiles.Get(slot).m_Index;
		if (idx >= 0)
			TestSelect(idx);
		return true;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (SlotOf(w) < 0)
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
		int slot = SlotOf(w);
		if (slot < 0)
			return false;
		DRL_UIVariantTile t = m_Tiles.Get(slot);
		t.m_Hover = true;
		t.Paint();
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int slot = SlotOf(w);
		if (slot < 0)
			return false;
		DRL_UIVariantTile t = m_Tiles.Get(slot);
		t.m_Hover = false;
		t.Paint();
		return true;
	}
}

// --- подсказка ингредиента (= панель IngridientDetai* старого меню) -------------------
class DRL_UIHoverCard
{
	protected Widget m_Root;
	protected TextWidget m_Name;
	protected MultilineTextWidget m_Desc;
	protected TextWidget m_State;
	protected TextWidget m_Count;
	protected TextWidget m_Fill;
	protected TextWidget m_Damage;
	protected TextWidget m_Delete;
	protected float m_X;
	protected float m_Y;

	void Build(Widget parent, float x, float y, float w, float h)
	{
		m_X = x;
		m_Y = y;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_HoverCard");
		int cardFill = INT_UIF.Color(INT_UIF_ThemeKeys.PANEL);
		int cardBorder = INT_UIF.Soft(INT_UIF_ThemeKeys.TEAL, 110);
		INT_UIF_Surface.Shadow(m_Root, -16, -12, w + 32, h + 34, ARGB(140, 0, 0, 0));
		INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 12, cardFill, cardBorder);
		INT_UIF_Draw.Image(m_Root, 0, 0, w, h * 0.5, "iuf_grad_v", ARGB(12, 255, 255, 255));
		int text = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT);
		int muted = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		INT_UIF_Draw.Icon(m_Root, 16, 18, 16, "info", INT_UIF.Color(INT_UIF_ThemeKeys.TEAL));
		m_Name = INT_UIF_Draw.Text(m_Root, 40, 12, w - 56, 26, "", 17, text);
		m_Name.SetBold(true);
		m_Desc = INT_UIF_Draw.MText(m_Root, 16, 44, w - 32, h - 44 - 76, "", 12, muted);
		float ly = h - 70;
		float cw = (w - 32) * 0.5;
		m_State = INT_UIF_Draw.Text(m_Root, 16, ly, cw, 20, "", 12, text);
		m_Count = INT_UIF_Draw.Text(m_Root, 16 + cw, ly, cw, 20, "", 12, text);
		m_Fill = INT_UIF_Draw.Text(m_Root, 16, ly + 22, cw, 20, "", 12, text);
		m_Damage = INT_UIF_Draw.Text(m_Root, 16 + cw, ly + 22, cw, 20, "", 12, text);
		m_Delete = INT_UIF_Draw.Text(m_Root, 16, ly + 44, w - 32, 20, "", 12, muted);
		m_Root.Show(false);
	}

	// cls — показываемый класс, c — параметры компонента (как FillIngredientDetailInfo).
	void ShowFor(string cls, DRL_Craft_Component c)
	{
		if (!c)
		{
			Hide();
			return;
		}
		m_Name.SetText(DRL_UIText.DisplayName(cls));
		string ds = "";
		if (!DRL_CraftUIHelper.GetDescriptionShortForClass(cls, ds))
			ds = "";
		m_Desc.SetText(ds);
		m_State.SetText(DRL_UIText.Label("#STR_DRL_STATE", DRL_UIText.MinHealth(c.MinHealthFrac)));
		int cnt = c.Amount;
		if (cnt <= 0)
			cnt = 1;
		m_Count.SetText(DRL_UIText.Label("#STR_DRL_QUANTITY", cnt.ToString()));
		string qs = "—";
		if (c.QuantityFrac > 0.0)
			qs = DRL_UIText.Pct(c.QuantityFrac);
		m_Fill.SetText(DRL_UIText.Label("#STR_DRL_FILL", qs));
		m_Damage.SetText(DRL_UIText.Label("#STR_DRL_DAMAGE", DRL_UIText.Pct(c.ApplyDamageFrac)));
		if (c.Delete)
			m_Delete.SetText(DRL_UIText.Tr("#STR_DRL_DELETE_YES"));
		else
			m_Delete.SetText(DRL_UIText.Tr("#STR_DRL_DELETE_NO"));
		if (!m_Root.IsVisible())
		{
			m_Root.Show(true);
			DRL_UIAnim.Reveal(m_Root, m_X, m_Y, 0, 6, 0.16);
		}
	}

	bool IsShown()
	{
		return m_Root && m_Root.IsVisible();
	}

	void Hide()
	{
		if (m_Root)
			m_Root.Show(false);
	}
}

// --- выезжающая панель ------------------------------------------------------------------
class DRL_UIPopover
{
	protected Widget m_Root;
	protected Widget m_IconHost;
	protected TextWidget m_Title;
	protected TextWidget m_Sub;
	protected INT_UIF_IconButton m_Close;
	protected ref DRL_UIRowList m_List;
	protected string m_Kind;
	protected float m_X;
	protected float m_Y;
	protected float m_W;
	protected ref ScriptInvoker m_OnClose;

	void DRL_UIPopover()
	{
		m_List = new DRL_UIRowList();
		m_OnClose = new ScriptInvoker();
	}

	// () — закрыта крестиком
	ScriptInvoker OnClose()
	{
		return m_OnClose;
	}

	void Build(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float h)
	{
		m_X = x;
		m_Y = y;
		m_W = w;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_Popover");
		INT_UIF_Surface.Shadow(m_Root, -24, -18, w + 48, h + 52, ARGB(150, 0, 0, 0));
		int popFill = INT_UIF.Color(INT_UIF_ThemeKeys.PANEL);
		int popBorder = INT_UIF.Soft(INT_UIF_ThemeKeys.AMBER, 90);
		INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 14, popFill, popBorder);
		INT_UIF_Draw.Image(m_Root, 0, 0, w, 90, "iuf_grad_v", ARGB(14, 255, 255, 255));
		m_IconHost = INT_UIF_Draw.Frame(m_Root, 16, 16, 40, 40, "DRL_PopIcon");
		m_Title = INT_UIF_Draw.Text(m_Root, 68, 14, w - 120, 24, "", 17, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_Title.SetBold(true);
		m_Sub = INT_UIF_Draw.Text(m_Root, 68, 38, w - 120, 18, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		m_Close = INT_UIF_IconButton.Create(bag, m_Root, w - 50, 18, 34, "close", INT_UIF_Variant.GHOST);
		m_Close.OnClick().Insert(OnCloseClick);
		DRL_UIKitUtil.Expose(m_Close, "DRL_PopClose");
		INT_UIF_Draw.Line(m_Root, 16, 70, w - 32, 1, INT_UIF.Color(INT_UIF_ThemeKeys.DIVIDER));
		m_List.Build(m_Root, 14, 82, w - 22, h - 96, 52, 6, "DRL_Pop");
		m_Root.Show(false);
	}

	DRL_UIRowList GetList()
	{
		return m_List;
	}

	string GetKind()
	{
		if (!IsOpen())
			return "";
		return m_Kind;
	}

	bool IsOpen()
	{
		return m_Root && m_Root.IsVisible();
	}

	void Open(string kind, string title, string sub, string icon, int hue, array<ref DRL_UIRowData> rows)
	{
		bool wasOpen = IsOpen() && m_Kind == kind;
		m_Kind = kind;
		INT_UIF_Widgets.ClearChildren(m_IconHost);
		INT_UIF_Draw.IconTile(m_IconHost, 0, 0, 40, icon, hue, 10);
		m_Title.SetText(title);
		m_Sub.SetText(sub);
		m_List.SetItems(rows, wasOpen);
		if (!wasOpen)
		{
			m_Root.Show(true);
			DRL_UIAnim.Reveal(m_Root, m_X, m_Y, 0, 10, 0.22);
		}
	}

	// Обновить строки открытой панели, не сбрасывая прокрутку.
	void SetRows(array<ref DRL_UIRowData> rows)
	{
		m_List.SetItems(rows, true);
	}

	void Close()
	{
		if (!m_Root)
			return;
		m_Root.Show(false);
		m_List.ClearModels();
	}

	void OnCloseClick(INT_UIF_IconButton b)
	{
		Close();
		m_OnClose.Invoke();
	}
}

// --- процесс крафта на клиенте ----------------------------------------------------------
// Локальный отсчёт прогресса между синхронизациями (= DRL_CraftProcessEntryWidget).
class DRL_UIProc extends Managed
{
	ref DRL_CraftProcessSyncInfo m_Info;
	float m_Elapsed;
	float m_Total;

	void DRL_UIProc(DRL_CraftProcessSyncInfo info)
	{
		m_Info = info;
		m_Total = 1.0;
		if (info && info.m_TotalDurationSec > 0.0)
			m_Total = info.m_TotalDurationSec;
		m_Elapsed = 0.0;
		if (info)
			m_Elapsed = m_Total - info.m_RemainingTimeSec;
		if (m_Elapsed < 0.0)
			m_Elapsed = 0.0;
	}

	bool IsQueued()
	{
		return m_Info && m_Info.IsQueued();
	}

	void Tick(float dt)
	{
		if (IsQueued())
			return;
		m_Elapsed = m_Elapsed + dt;
		if (m_Elapsed > m_Total)
			m_Elapsed = m_Total;
	}

	int Percent()
	{
		if (m_Total <= 0.0)
			return 0;
		float pct = (m_Elapsed / m_Total) * 100.0;
		return (int)Math.Clamp(Math.Round(pct), 0, 100);
	}

	int RemainingSec()
	{
		return (int)Math.Ceil(m_Total - m_Elapsed);
	}

	string GetProcessId()
	{
		if (m_Info)
			return m_Info.m_ProcessId;
		return "";
	}

	int GetRecipeId()
	{
		if (m_Info)
			return m_Info.m_RecipeId;
		return -1;
	}
}

// --- очередь процессов в шапке ----------------------------------------------------------
class DRL_UIQueueTile extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ref DRL_UIItemView m_View;
	ImageWidget m_Image;
	ImageWidget m_Glyph;
	ImageWidget m_Track;
	ImageWidget m_Fill;
	ImageWidget m_QueuedIcon;
	Widget m_Cancel;
	ButtonWidget m_CancelHit;
	ButtonWidget m_Hit;
	float m_S;
	int m_Index = -1;
	bool m_Hover;

	void Build(Widget parent, int slot, float x, float y, float s, ScriptedWidgetEventHandler handler)
	{
		m_S = s;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, s, s, "DRL_QTile");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, s, s, 8, 0, 0);
		Widget clip = INT_UIF_Draw.Clip(m_Root, 3, 3, s - 6, s - 9, "DRL_QClip");
		m_View = new DRL_UIItemView();
		m_View.Create(clip, true);
		m_Image = INT_UIF_Draw.Image(m_Root, 7, 6, s - 14, s - 14, "iuf_white", ARGB(235, 255, 255, 255));
		m_Image.Show(false);
		m_Glyph = INT_UIF_Draw.Icon(m_Root, (s - 16) * 0.5, (s - 16) * 0.5, 16, "gear", INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		m_Track = INT_UIF_Draw.Line(m_Root, 5, s - 6, s - 10, 3, ARGB(40, 255, 255, 255));
		m_Fill = INT_UIF_Draw.Line(m_Root, 5, s - 6, 1, 3, INT_UIF.Color(INT_UIF_ThemeKeys.TEAL));
		m_Track.SetSort(DRL_UI_SORT_OVER_MODEL);
		m_Fill.SetSort(DRL_UI_SORT_OVER_MODEL);
		m_QueuedIcon = INT_UIF_Draw.Icon(m_Root, 3, 3, 12, "clock", INT_UIF.Color(INT_UIF_ThemeKeys.AMBER));
		m_QueuedIcon.SetSort(DRL_UI_SORT_OVER_MODEL);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, s, s, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, "DRL_Q_" + slot.ToString(), handler);
		m_Cancel = INT_UIF_Draw.Frame(m_Root, s - 19, 1, 18, 18, "DRL_QCancel");
		INT_UIF_Surface.Box(m_Cancel, 0, 0, 18, 18, 4, INT_UIF.Color(INT_UIF_ThemeKeys.RED), 0);
		INT_UIF_Draw.Icon(m_Cancel, 4, 4, 10, "close", ARGB(255, 255, 255, 255));
		m_CancelHit = INT_UIF_Draw.Hit(m_Cancel, 0, 0, 18, 18, handler);
		DRL_UIKitUtil.ExposeHit(m_CancelHit, "DRL_QCancel_" + slot.ToString(), handler);
		DRL_UIKitUtil.SetSortDeep(m_Cancel, DRL_UI_SORT_OVER_MODEL);
		m_Cancel.Show(false);
	}

	// proc == null — пустой слот очереди.
	void Bind(int index, DRL_UIProc proc)
	{
		m_Index = index;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (!proc)
		{
			m_View.Clear();
			m_Image.Show(false);
			m_Glyph.Show(false);
			m_Track.Show(false);
			m_Fill.Show(false);
			m_QueuedIcon.Show(false);
			m_Cancel.Show(false);
			m_Hit.Show(false);
			m_Bg.SetFill(ARGB(5, 255, 255, 255));
			m_Bg.SetBorder(ARGB(22, 255, 255, 255));
			return;
		}
		m_Hit.Show(true);
		DRL_Craft_Recipe r = null;
		if (g_Craft_Config)
			r = g_Craft_Config.FindById(proc.GetRecipeId());
		// Иконка рецепта или 3D-превью результата, как в старой плитке процесса.
		bool useIcon = r && r.CraftIcon != "";
		bool hasModel = false;
		if (useIcon)
		{
			m_View.Clear();
			m_Image.LoadImageFile(0, DRL_CraftUIHelper.ResolveImagePath(r.CraftIcon));
			m_Image.SetImage(0);
		}
		else if (proc.m_Info)
		{
			hasModel = m_View.SetClass(proc.m_Info.m_ResultClass);
		}
		m_Image.Show(useIcon);
		m_Glyph.Show(!useIcon && !hasModel);
		bool queued = proc.IsQueued();
		m_QueuedIcon.Show(queued);
		m_Track.Show(!queued);
		m_Fill.Show(!queued);
		m_Bg.SetFill(tm.Color(INT_UIF_ThemeKeys.TILE));
		if (queued)
			m_Bg.SetBorder(INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.AMBER), 120));
		else
			m_Bg.SetBorder(INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.TEAL), 140));
		m_Cancel.Show(m_Hover);
		UpdateProgress(proc);
	}

	void UpdateProgress(DRL_UIProc proc)
	{
		if (!proc || proc.IsQueued())
			return;
		float fw = (m_S - 10) * proc.Percent() / 100.0;
		if (fw < 1)
			fw = 1;
		INT_UIF_Draw.Resize(m_Fill, fw, 3);
	}

	void SetHover(bool hover)
	{
		m_Hover = hover;
		m_Cancel.Show(hover && m_Index >= 0);
	}
}

class DRL_UIQueueBar extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected TextWidget m_Count;
	protected TextWidget m_More;
	protected ref array<ref DRL_UIQueueTile> m_Tiles;
	protected ref array<ref DRL_UIProc> m_Procs;
	protected ref ScriptInvoker m_OnAction;

	void DRL_UIQueueBar()
	{
		m_Tiles = new array<ref DRL_UIQueueTile>();
		m_Procs = new array<ref DRL_UIProc>();
		m_OnAction = new ScriptInvoker();
	}

	void ~DRL_UIQueueBar()
	{
		Clear();
	}

	// (int procIndex, string action ("select" / "cancel"), DRL_UIQueueBar bar)
	ScriptInvoker OnAction()
	{
		return m_OnAction;
	}

	void Build(Widget parent, float w, float h)
	{
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, w, h, "DRL_Queue");
		float ts = 48;
		float ty = (h - ts) * 0.5;
		INT_UIF_Draw.Caption(m_Root, 0, ty + 4, 90, "#STR_DRL_UI_QUEUE", "clock");
		m_Count = INT_UIF_Draw.Text(m_Root, 0, ty + 20, 90, 24, "", 17, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_Count.SetBold(true);
		m_More = INT_UIF_Draw.Text(m_Root, 46, ty + 24, 44, 18, "", 11, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		float x = 96;
		int slot = 0;
		while (x + ts <= w)
		{
			DRL_UIQueueTile t = new DRL_UIQueueTile();
			t.Build(m_Root, slot, x, ty, ts, this);
			t.m_Root.Show(false);
			m_Tiles.Insert(t);
			x = x + ts + 8;
			slot++;
		}
	}

	// Плитки: процессы, затем пустые слоты до maxSlots (сколько помещается).
	void SetProcs(array<ref DRL_UIProc> procs, int maxSlots)
	{
		m_Procs.Clear();
		if (procs)
		{
			foreach (DRL_UIProc p : procs)
			{
				m_Procs.Insert(p);
			}
		}
		int n = m_Procs.Count();
		m_Count.SetText(n.ToString() + "/" + maxSlots.ToString());
		int shown = maxSlots;
		if (n > shown)
			shown = n;
		for (int i = 0; i < m_Tiles.Count(); i++)
		{
			DRL_UIQueueTile t = m_Tiles.Get(i);
			if (i < shown)
			{
				t.m_Root.Show(true);
				if (i < n)
					t.Bind(i, m_Procs.Get(i));
				else
					t.Bind(-1, null);
			}
			else
			{
				t.m_View.Clear();
				t.m_Root.Show(false);
			}
		}
		int over = n - m_Tiles.Count();
		if (over > 0)
			m_More.SetText("+" + over.ToString());
		else
			m_More.SetText("");
	}

	void Tick()
	{
		for (int i = 0; i < m_Tiles.Count(); i++)
		{
			DRL_UIQueueTile t = m_Tiles.Get(i);
			if (t.m_Index >= 0 && t.m_Index < m_Procs.Count())
				t.UpdateProgress(m_Procs.Get(t.m_Index));
		}
	}

	void HideModels(bool hide)
	{
		foreach (DRL_UIQueueTile t : m_Tiles)
		{
			if (t.m_Index >= 0)
				t.m_View.Show(!hide);
		}
	}

	void Clear()
	{
		foreach (DRL_UIQueueTile t : m_Tiles)
		{
			if (t)
				t.m_View.Clear();
		}
	}

	protected int SlotOf(Widget w, out bool cancel)
	{
		cancel = false;
		for (int i = 0; i < m_Tiles.Count(); i++)
		{
			DRL_UIQueueTile t = m_Tiles.Get(i);
			if (t.m_Hit == w)
				return i;
			if (t.m_CancelHit == w)
			{
				cancel = true;
				return i;
			}
		}
		return -1;
	}

	void TestAction(int index, string action)
	{
		if (index < 0 || index >= m_Procs.Count())
			return;
		DRL_UIQueueBar keep = this;
		m_OnAction.Invoke(index, action, this);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		bool cancel;
		int slot = SlotOf(w, cancel);
		if (slot < 0 || button != MouseState.LEFT)
			return false;
		int idx = m_Tiles.Get(slot).m_Index;
		if (idx < 0)
			return true;
		if (cancel)
			TestAction(idx, "cancel");
		else
			TestAction(idx, "select");
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		bool cancel;
		int slot = SlotOf(w, cancel);
		if (slot < 0)
			return false;
		m_Tiles.Get(slot).SetHover(true);
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		bool cancel;
		int slot = SlotOf(w, cancel);
		if (slot < 0)
			return false;
		bool nextCancel;
		if (enterW && SlotOf(enterW, nextCancel) == slot)
			return true;
		m_Tiles.Get(slot).SetHover(false);
		return true;
	}
}
