// ============================================================================
// DRL_CraftMenu - детали окна крафта v2 (раскладка старого меню в четыре колонки):
//   DRL_UITileList      сетка плиток с заголовками разделов: ингредиенты и
//                       доп. предметы, доп. навесное, что вернёт разбор;
//   DRL_UICategoryRail  вертикальная полоса иконок категорий;
//   DRL_UISlotStrip     ряд слотов навесного результата;
//   DRL_UIIngrCard      карточка ингредиента под плитками.
// Плитки — пул над массивом данных, прокрутка колесом по строке плиток (как у
// DRL_UIRowList): одновременно живут только видимые 3D-превью.
// Хит-зоны плиток: <префикс>_<слот>, а также _chk, _minus, _plus, _info, _v<n>.
// ============================================================================

const int DRL_UI_TILE_ITEM = 0;
const int DRL_UI_TILE_HEADER = 1;
const int DRL_UI_TILE_NOTE = 2;

const int DRL_UI_TILE_MAX_VARS = 6;

class DRL_UITileData extends Managed
{
	int m_Kind;
	string m_Title;          // имя для подсказки / текст заголовка или пояснения
	string m_Right;          // правый текст заголовка
	int m_RightColor;
	string m_Count;          // текст внизу плитки (есть/нужно, x5)
	int m_CountColor;
	string m_Corner;         // мелкий текст справа сверху (шанс)
	int m_State;             // цвет рамки состояния (0 — нейтральная)
	string m_PreviewClass;   // 3D-превью
	string m_IconPath;       // картинка вместо 3D (CraftIcon)
	string m_Glyph;          // иконка фреймворка, если нет ни 3D, ни картинки
	string m_Badge;          // значок слева сверху (swap — есть вариации)
	bool m_Selected;
	bool m_Clickable = true;
	bool m_Dim;
	int m_Check = -1;        // -1 без чекбокса, 0/1
	bool m_Locked;           // замок вместо чекбокса
	int m_Qty;               // 0 — без степпера
	int m_VarCount;          // > 1 — номера вариантов внизу
	int m_VarSelected;
	bool m_Info;             // кнопка «к рецепту»
	bool m_InfoEnabled = true;
	int m_Tag = -1;          // индекс в данных владельца

	static DRL_UITileData Header(string title, string right = "", int rightColor = 0)
	{
		DRL_UITileData d = new DRL_UITileData();
		d.m_Kind = DRL_UI_TILE_HEADER;
		d.m_Title = title;
		d.m_Right = right;
		d.m_RightColor = rightColor;
		d.m_Clickable = false;
		return d;
	}

	static DRL_UITileData Note(string text)
	{
		DRL_UITileData d = new DRL_UITileData();
		d.m_Kind = DRL_UI_TILE_NOTE;
		d.m_Title = text;
		d.m_Clickable = false;
		return d;
	}
}

class DRL_UITile extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ref DRL_UIItemView m_View;
	ImageWidget m_Image;
	ImageWidget m_Glyph;
	ImageWidget m_Badge;
	TextWidget m_Corner;
	TextWidget m_Count;
	ButtonWidget m_Hit;

	Widget m_InfoRoot;
	ImageWidget m_InfoIcon;
	ButtonWidget m_InfoHit;

	Widget m_CheckRoot;
	ref INT_UIF_Surface m_CheckBg;
	ImageWidget m_CheckIcon;
	ButtonWidget m_CheckHit;

	Widget m_QtyRoot;
	TextWidget m_QtyText;
	ButtonWidget m_MinusHit;
	ButtonWidget m_PlusHit;

	ref array<ref INT_UIF_Surface> m_VarBg;
	ref array<TextWidget> m_VarText;
	ref array<ButtonWidget> m_VarHit;

	DRL_UITileData m_Data;
	float m_W;
	float m_H;
	float m_X;
	float m_Y;
	bool m_Controls;
	int m_Index = -1;
	bool m_Hover;
	bool m_ModelShown = true;

	// controls — плитка доп. навесного: сверху «к рецепту» и чекбокс,
	// снизу номера вариантов и количество.
	void Build(Widget parent, int slot, float w, float h, bool controls, string prefix, ScriptedWidgetEventHandler handler)
	{
		m_W = w;
		m_H = h;
		m_Controls = controls;
		string base = prefix + "_" + slot.ToString();
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);
		int muted = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		int amber = INT_UIF.Color(INT_UIF_ThemeKeys.AMBER);
		int amberText = INT_UIF.Color(INT_UIF_ThemeKeys.AMBER_TEXT);

		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, w, h, "DRL_Tile");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 10, 0, 0);

		float top = 6;
		float bottom = 22;
		if (controls)
		{
			top = 28;
			bottom = 28;
		}
		float th = h - top - bottom;
		Widget clip = INT_UIF_Draw.Clip(m_Root, 4, top, w - 8, th, "DRL_TileClip");
		m_View = new DRL_UIItemView();
		m_View.Create(clip, true);
		float isz = th - 4;
		if (isz > w - 16)
			isz = w - 16;
		m_Image = INT_UIF_Draw.Image(m_Root, (w - isz) * 0.5, top + (th - isz) * 0.5, isz, isz, "iuf_white", ARGB(235, 255, 255, 255));
		m_Image.Show(false);
		m_Glyph = INT_UIF_Draw.Icon(m_Root, (w - 20) * 0.5, top + (th - 20) * 0.5, 20, "info", faint);
		m_Glyph.Show(false);

		// Значки лежат на превью: поднимаем их над моделью.
		m_Badge = INT_UIF_Draw.Icon(m_Root, 6, 6, 13, "swap", amber);
		m_Badge.SetSort(DRL_UI_SORT_OVER_MODEL);
		m_Badge.Show(false);
		m_Corner = INT_UIF_Draw.Text(m_Root, w - 52, 3, 46, 16, "", 10, amberText, INT_UIF_Align.RIGHT);
		m_Corner.SetSort(DRL_UI_SORT_OVER_MODEL);
		m_Count = INT_UIF_Draw.Text(m_Root, 4, h - bottom, w - 8, bottom - 4, "", 12, 0, INT_UIF_Align.CENTER);
		m_Count.SetBold(true);

		// Основная хит-зона ниже дополнительных: те создаются позже и лежат сверху.
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, base, handler);

		m_VarBg = new array<ref INT_UIF_Surface>();
		m_VarText = new array<TextWidget>();
		m_VarHit = new array<ButtonWidget>();
		if (!controls)
			return;

		m_InfoRoot = INT_UIF_Draw.Frame(m_Root, 4, 4, 22, 22, "DRL_TileInfo");
		m_InfoIcon = INT_UIF_Draw.Icon(m_InfoRoot, 3, 3, 16, "info", muted);
		m_InfoHit = INT_UIF_Draw.Hit(m_InfoRoot, 0, 0, 22, 22, handler);
		DRL_UIKitUtil.ExposeHit(m_InfoHit, base + "_info", handler);

		m_CheckRoot = INT_UIF_Draw.Frame(m_Root, w - 26, 4, 22, 22, "DRL_TileCheck");
		m_CheckBg = INT_UIF_Surface.Box(m_CheckRoot, 0, 0, 22, 22, 6, 0, 0);
		m_CheckIcon = INT_UIF_Draw.Icon(m_CheckRoot, 4, 4, 14, "check", 0);
		m_CheckHit = INT_UIF_Draw.Hit(m_CheckRoot, -4, -4, 30, 30, handler);
		DRL_UIKitUtil.ExposeHit(m_CheckHit, base + "_chk", handler);

		for (int v = 0; v < DRL_UI_TILE_MAX_VARS; v++)
		{
			INT_UIF_Surface vb = INT_UIF_Surface.Box(m_Root, 6 + v * 20, h - 24, 18, 18, 4, 0, 0);
			int num = v + 1;
			TextWidget vt = INT_UIF_Draw.Text(vb.GetRoot(), 0, 0, 18, 18, num.ToString(), 11, 0, INT_UIF_Align.CENTER);
			vt.SetBold(true);
			ButtonWidget vh = INT_UIF_Draw.Hit(vb.GetRoot(), 0, 0, 18, 18, handler);
			DRL_UIKitUtil.ExposeHit(vh, base + "_v" + v.ToString(), handler);
			m_VarBg.Insert(vb);
			m_VarText.Insert(vt);
			m_VarHit.Insert(vh);
		}

		// Количество — сверху между «к рецепту» и чекбоксом: низ занят номерами вариантов.
		m_QtyRoot = INT_UIF_Draw.Frame(m_Root, (w - 58) * 0.5, 5, 58, 20, "DRL_TileQty");
		int qtyFill = INT_UIF.Color(INT_UIF_ThemeKeys.PANEL);
		INT_UIF_Surface.Box(m_QtyRoot, 0, 0, 58, 20, 5, qtyFill, ARGB(26, 255, 255, 255));
		INT_UIF_Draw.Icon(m_QtyRoot, 5, 5, 10, "minus", muted);
		INT_UIF_Draw.Icon(m_QtyRoot, 43, 5, 10, "plus", muted);
		m_QtyText = INT_UIF_Draw.Text(m_QtyRoot, 18, 0, 22, 20, "1", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT), INT_UIF_Align.CENTER);
		m_QtyText.SetBold(true);
		m_MinusHit = INT_UIF_Draw.Hit(m_QtyRoot, 0, 0, 20, 20, handler);
		DRL_UIKitUtil.ExposeHit(m_MinusHit, base + "_minus", handler);
		m_PlusHit = INT_UIF_Draw.Hit(m_QtyRoot, 38, 0, 20, 20, handler);
		DRL_UIKitUtil.ExposeHit(m_PlusHit, base + "_plus", handler);
	}

	// Какая часть плитки под виджетом: "tile", "check", "minus", "plus", "info", "var".
	string PartOf(Widget w, out int arg)
	{
		arg = 0;
		if (!w)
			return "";
		if (w == m_Hit)
			return "tile";
		if (!m_Controls)
			return "";
		if (w == m_CheckHit)
			return "check";
		if (w == m_MinusHit)
			return "minus";
		if (w == m_PlusHit)
			return "plus";
		if (w == m_InfoHit)
			return "info";
		for (int v = 0; v < m_VarHit.Count(); v++)
		{
			if (w == m_VarHit.Get(v))
			{
				arg = v;
				return "var";
			}
		}
		return "";
	}

	void Place(float x, float y)
	{
		m_X = x;
		m_Y = y;
		DRL_UIAnim.MoveTo(m_Root, x, y);
	}

	void Bind(int index, DRL_UITileData d)
	{
		m_Index = index;
		m_Data = d;
		if (!d)
		{
			m_Root.Show(false);
			m_View.Clear();
			return;
		}
		m_Root.Show(true);
		BindThumb(d);
		m_Corner.SetText(d.m_Corner);
		m_Count.SetText(d.m_Count);
		int cc = d.m_CountColor;
		if (cc == 0)
			cc = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT);
		m_Count.SetColor(cc);
		if (m_Controls)
		{
			m_InfoRoot.Show(d.m_Info);
			if (d.m_Info)
			{
				if (d.m_InfoEnabled)
					m_InfoIcon.SetColor(INT_UIF.Color(INT_UIF_ThemeKeys.TEAL));
				else
					m_InfoIcon.SetColor(INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
			}
			bool hasCheck = d.m_Check >= 0 || d.m_Locked;
			m_CheckRoot.Show(hasCheck);
			if (hasCheck)
				PaintCheck(d);
			ShowVars(d.m_VarCount, d.m_VarSelected);
			m_QtyRoot.Show(d.m_Qty > 0);
			if (d.m_Qty > 0)
				m_QtyText.SetText(d.m_Qty.ToString());
		}
		if (d.m_Dim)
			m_Root.SetAlpha(0.55);
		else
			m_Root.SetAlpha(1);
		ApplyOverlay();
		Paint();
	}

	protected void BindThumb(DRL_UITileData d)
	{
		bool useImage = d.m_IconPath != "";
		bool hasModel = false;
		if (useImage)
		{
			m_View.Clear();
			m_Image.LoadImageFile(0, d.m_IconPath);
			m_Image.SetImage(0);
		}
		else if (d.m_PreviewClass != "")
		{
			hasModel = m_View.SetClass(d.m_PreviewClass);
		}
		else
		{
			m_View.Clear();
		}
		m_Image.Show(useImage);
		bool glyph = !useImage && !hasModel;
		m_Glyph.Show(glyph);
		if (glyph)
		{
			string g = d.m_Glyph;
			if (g == "")
				g = "info";
			INT_UIF_Widgets.SetIcon(m_Glyph, g);
		}
		if (d.m_Badge != "")
			INT_UIF_Widgets.SetIcon(m_Badge, d.m_Badge);
	}

	protected void PaintCheck(DRL_UITileData d)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (d.m_Locked)
		{
			m_CheckBg.SetFill(INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.RED), 40));
			m_CheckBg.SetBorder(INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.RED), 120));
			INT_UIF_Widgets.SetIcon(m_CheckIcon, "lock");
			m_CheckIcon.SetColor(tm.Color(INT_UIF_ThemeKeys.RED));
			m_CheckIcon.Show(true);
			return;
		}
		INT_UIF_Widgets.SetIcon(m_CheckIcon, "check");
		if (d.m_Check == 1)
		{
			m_CheckBg.SetFill(tm.Color(INT_UIF_ThemeKeys.TEAL));
			m_CheckBg.SetBorder(tm.Color(INT_UIF_ThemeKeys.TEAL));
			m_CheckIcon.SetColor(ARGB(255, 16, 36, 31));
			m_CheckIcon.Show(true);
		}
		else
		{
			m_CheckBg.SetFill(ARGB(0, 0, 0, 0));
			int border = ARGB(60, 255, 255, 255);
			if (d.m_Dim)
				border = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.RED), 140);
			m_CheckBg.SetBorder(border);
			m_CheckIcon.Show(false);
		}
	}

	protected void ShowVars(int count, int selected)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		for (int v = 0; v < m_VarBg.Count(); v++)
		{
			INT_UIF_Surface vb = m_VarBg.Get(v);
			bool on = v < count;
			vb.Show(on);
			if (!on)
				continue;
			TextWidget vt = m_VarText.Get(v);
			if (v == selected)
			{
				vb.SetFill(tm.Color(INT_UIF_ThemeKeys.AMBER));
				vb.SetBorder(tm.Color(INT_UIF_ThemeKeys.AMBER));
				vt.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_ON_ACCENT));
			}
			else
			{
				vb.SetFill(tm.Color(INT_UIF_ThemeKeys.PANEL));
				vb.SetBorder(ARGB(26, 255, 255, 255));
				vt.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
			}
		}
	}

	void SetHover(bool hover)
	{
		if (m_Hover == hover)
			return;
		m_Hover = hover;
		Paint();
	}

	void Paint()
	{
		if (!m_Data)
			return;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int fill = tm.Color(INT_UIF_ThemeKeys.TILE);
		int border = tm.Color(INT_UIF_ThemeKeys.TILE_BORDER);
		if (m_Data.m_State != 0)
			border = INT_UIF_Color.WithAlpha(m_Data.m_State, 110);
		if (m_Data.m_Check == 1)
		{
			fill = tm.Color(INT_UIF_ThemeKeys.TILE_HOVER);
			border = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.TEAL), 120);
		}
		if (m_Hover && m_Data.m_Clickable)
		{
			fill = tm.Color(INT_UIF_ThemeKeys.TILE_HOVER);
			if (m_Data.m_State == 0 && m_Data.m_Check != 1)
				border = INT_UIF.Soft(INT_UIF_ThemeKeys.TEAL, 90);
		}
		if (m_Data.m_Selected)
		{
			fill = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.AMBER), 36);
			border = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.AMBER), 220);
		}
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
	}

	// Модель и значки над ней (у значков повышенный приоритет — они легли бы
	// поверх выезжающей панели).
	void ShowModel(bool show)
	{
		m_ModelShown = show;
		m_View.Show(show);
		ApplyOverlay();
	}

	protected void ApplyOverlay()
	{
		bool badge = m_Data && m_Data.m_Badge != "";
		m_Badge.Show(badge && m_ModelShown);
		m_Corner.Show(m_ModelShown);
	}

	void Clear()
	{
		if (m_View)
			m_View.Clear();
		m_Index = -1;
		m_Data = null;
	}
}

// Заголовок раздела или пояснение во всю ширину списка плиток.
class DRL_UITileHead extends Managed
{
	Widget m_Root;
	TextWidget m_Title;
	TextWidget m_Right;
	ref INT_UIF_Surface m_NoteBg;
	TextWidget m_Note;

	void Build(Widget parent, float w)
	{
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, w, 44, "DRL_TileHead");
		m_Title = INT_UIF_Draw.Text(m_Root, 2, 8, w - 4, 24, "", 15, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_Title.SetBold(true);
		m_Right = INT_UIF_Draw.Text(m_Root, 2, 10, w - 4, 22, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED), INT_UIF_Align.RIGHT);
		// Цвета — в локальные: два вызова одной функции в аргументах Enforce
		// сводит к результату последнего.
		int noteFill = ARGB(0, 0, 0, 0);
		int noteBorder = ARGB(30, 255, 255, 255);
		m_NoteBg = INT_UIF_Surface.Box(m_Root, 0, 0, w, 44, 9, noteFill, noteBorder);
		m_Note = INT_UIF_Draw.Text(m_Root, 8, 0, w - 16, 44, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED), INT_UIF_Align.CENTER);
		m_Root.Show(false);
	}

	void Bind(DRL_UITileData d)
	{
		if (!d)
		{
			m_Root.Show(false);
			return;
		}
		m_Root.Show(true);
		bool header = d.m_Kind == DRL_UI_TILE_HEADER;
		m_Title.Show(header);
		m_Right.Show(header);
		m_NoteBg.Show(!header);
		m_Note.Show(!header);
		if (header)
		{
			m_Title.SetText(d.m_Title);
			m_Right.SetText(d.m_Right);
			int rc = d.m_RightColor;
			if (rc == 0)
				rc = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
			m_Right.SetColor(rc);
		}
		else
		{
			m_Note.SetText(d.m_Title);
		}
	}

	void Place(float x, float y)
	{
		DRL_UIAnim.MoveTo(m_Root, x, y);
	}
}

// Строка раскладки: заголовок, пояснение или ряд плиток.
class DRL_UITileLine extends Managed
{
	int m_Kind;
	int m_First;
	int m_Count;
	float m_H;
}

class DRL_UITileList extends ScriptedWidgetEventHandler
{
	static const float HEAD_H = 36;
	static const float NOTE_H = 52;

	protected Widget m_Root;
	protected ButtonWidget m_BgHit;
	protected ImageWidget m_Track;
	protected ImageWidget m_Thumb;
	protected TextWidget m_EmptyText;
	protected string m_EmptyStr;
	protected ref array<ref DRL_UITile> m_Tiles;
	protected ref array<ref DRL_UITileHead> m_Heads;
	protected ref array<ref DRL_UITileData> m_Items;
	protected ref array<ref DRL_UITileLine> m_Lines;
	protected float m_W;
	protected float m_H;
	protected float m_TileW;
	protected float m_TileH;
	protected float m_Gap;
	protected int m_Cols;
	protected int m_First;
	protected int m_Shown;
	protected int m_HoverSlot = -1;
	protected bool m_ModelsHidden;
	protected bool m_Tooltips;
	protected ref ScriptInvoker m_OnAction;
	protected ref ScriptInvoker m_OnHover;

	void DRL_UITileList()
	{
		m_Tiles = new array<ref DRL_UITile>();
		m_Heads = new array<ref DRL_UITileHead>();
		m_Items = new array<ref DRL_UITileData>();
		m_Lines = new array<ref DRL_UITileLine>();
		m_OnAction = new ScriptInvoker();
		m_OnHover = new ScriptInvoker();
	}

	void ~DRL_UITileList()
	{
		ClearModels();
	}

	// (int index, string action, int arg, DRL_UITileList list)
	ScriptInvoker OnAction()
	{
		return m_OnAction;
	}

	// (int index, DRL_UITileList list); -1 — курсор ушёл с плиток
	ScriptInvoker OnHover()
	{
		return m_OnHover;
	}

	// Справа 10 px под полосу прокрутки.
	void Build(Widget parent, float x, float y, float w, float h, int cols, float tileH, float gap, bool controls, string prefix)
	{
		m_W = w;
		m_H = h;
		m_Cols = cols;
		m_TileH = tileH;
		m_Gap = gap;
		float inner = w - 10;
		m_TileW = (inner - (cols - 1) * gap) / cols;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_TileList");
		m_BgHit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, this);
		DRL_UIKitUtil.ExposeHit(m_BgHit, prefix + "_Bg", this);

		int lines = (int)((h + gap) / (tileH + gap));
		if (lines < 1)
			lines = 1;
		int pool = lines * cols;
		for (int i = 0; i < pool; i++)
		{
			DRL_UITile tile = new DRL_UITile();
			tile.Build(m_Root, i, m_TileW, tileH, controls, prefix, this);
			tile.m_Root.Show(false);
			m_Tiles.Insert(tile);
		}
		int heads = (int)(h / HEAD_H) + 1;
		if (heads > 6)
			heads = 6;
		for (int k = 0; k < heads; k++)
		{
			DRL_UITileHead head = new DRL_UITileHead();
			head.Build(m_Root, inner);
			m_Heads.Insert(head);
		}
		m_Track = INT_UIF_Draw.Line(m_Root, w - 4, 0, 4, h, ARGB(14, 255, 255, 255));
		m_Thumb = INT_UIF_Draw.Line(m_Root, w - 4, 0, 4, 40, INT_UIF.Soft(INT_UIF_ThemeKeys.TEAL, 170));
		m_EmptyText = INT_UIF_Draw.Text(m_Root, 0, h * 0.5 - 20, inner, 40, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT), INT_UIF_Align.CENTER);
		m_EmptyText.Show(false);
		UpdateThumb();
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	void Show(bool show)
	{
		if (m_Root)
			m_Root.Show(show);
	}

	// Подсказка с именем предмета при наведении на плитку.
	void SetTooltips(bool on)
	{
		m_Tooltips = on;
	}

	void SetEmptyText(string text)
	{
		m_EmptyStr = text;
		m_EmptyText.SetText(text);
	}

	void SetItems(array<ref DRL_UITileData> items, bool keepScroll = false)
	{
		m_Items.Clear();
		if (items)
		{
			foreach (DRL_UITileData d : items)
			{
				m_Items.Insert(d);
			}
		}
		BuildLines();
		if (!keepScroll)
			m_First = 0;
		m_First = Math.Clamp(m_First, 0, MaxFirst());
		Rebind();
		m_EmptyText.Show(m_Items.Count() == 0 && m_EmptyStr != "");
	}

	int Count()
	{
		return m_Items.Count();
	}

	DRL_UITileData GetItem(int index)
	{
		if (index < 0 || index >= m_Items.Count())
			return null;
		return m_Items.Get(index);
	}

	protected void BuildLines()
	{
		m_Lines.Clear();
		int n = m_Items.Count();
		int i = 0;
		while (i < n)
		{
			DRL_UITileData d = m_Items.Get(i);
			DRL_UITileLine ln = new DRL_UITileLine();
			ln.m_First = i;
			ln.m_Kind = d.m_Kind;
			if (d.m_Kind == DRL_UI_TILE_ITEM)
			{
				int c = 0;
				while (i < n && c < m_Cols && m_Items.Get(i).m_Kind == DRL_UI_TILE_ITEM)
				{
					i++;
					c++;
				}
				ln.m_Count = c;
				ln.m_H = m_TileH + m_Gap;
			}
			else
			{
				ln.m_Count = 1;
				ln.m_H = HEAD_H;
				if (d.m_Kind == DRL_UI_TILE_NOTE)
					ln.m_H = NOTE_H;
				i++;
			}
			m_Lines.Insert(ln);
		}
	}

	// Первая строка, с которой остаток списка помещается целиком.
	protected int MaxFirst()
	{
		float acc = 0;
		int k = m_Lines.Count() - 1;
		while (k >= 0)
		{
			acc = acc + m_Lines.Get(k).m_H;
			if (acc > m_H + m_Gap + 0.5)
				break;
			k--;
		}
		return k + 1;
	}

	void ScrollTo(int first)
	{
		int target = Math.Clamp(first, 0, MaxFirst());
		if (target == m_First)
			return;
		m_First = target;
		Rebind();
	}

	// Перерисовать после правки данных на месте (выделение, флаги).
	void Refresh()
	{
		Rebind();
	}

	protected void Rebind()
	{
		int tileSlot = 0;
		int headSlot = 0;
		float y = 0;
		m_Shown = 0;
		for (int li = m_First; li < m_Lines.Count(); li++)
		{
			DRL_UITileLine ln = m_Lines.Get(li);
			if (y + ln.m_H > m_H + m_Gap + 0.5)
				break;
			if (ln.m_Kind == DRL_UI_TILE_ITEM)
			{
				if (tileSlot + ln.m_Count > m_Tiles.Count())
					break;
				for (int c = 0; c < ln.m_Count; c++)
				{
					DRL_UITile tile = m_Tiles.Get(tileSlot);
					int idx = ln.m_First + c;
					tile.Bind(idx, m_Items.Get(idx));
					tile.Place(c * (m_TileW + m_Gap), y);
					tile.ShowModel(!m_ModelsHidden);
					tile.SetHover(tileSlot == m_HoverSlot);
					tileSlot++;
				}
			}
			else
			{
				if (headSlot >= m_Heads.Count())
					break;
				DRL_UITileHead head = m_Heads.Get(headSlot);
				head.Bind(m_Items.Get(ln.m_First));
				head.Place(0, y);
				headSlot++;
			}
			y = y + ln.m_H;
			m_Shown++;
		}
		for (int t = tileSlot; t < m_Tiles.Count(); t++)
		{
			m_Tiles.Get(t).Bind(-1, null);
		}
		for (int h = headSlot; h < m_Heads.Count(); h++)
		{
			m_Heads.Get(h).Bind(null);
		}
		UpdateThumb();
	}

	protected void UpdateThumb()
	{
		bool scrollable = MaxFirst() > 0;
		m_Track.Show(scrollable);
		m_Thumb.Show(scrollable);
		if (!scrollable)
			return;
		float th = m_H * m_Shown / m_Lines.Count();
		if (th < 30)
			th = 30;
		float ty = (m_H - th) * m_First / MaxFirst();
		INT_UIF_Draw.Place(m_Thumb, m_W - 4, ty, 4, th);
	}

	void HideModels(bool hide)
	{
		m_ModelsHidden = hide;
		foreach (DRL_UITile tile : m_Tiles)
		{
			if (tile.m_Index >= 0)
				tile.ShowModel(!hide);
		}
	}

	void ClearModels()
	{
		foreach (DRL_UITile tile : m_Tiles)
		{
			if (tile)
				tile.Clear();
		}
	}

	// Тестовый хук: действие над плиткой без мыши.
	void TestAction(int index, string action, int arg)
	{
		if (index < 0 || index >= m_Items.Count())
			return;
		DRL_UITileList keep = this;
		m_OnAction.Invoke(index, action, arg, this);
	}

	// --- события ------------------------------------------------------------------
	protected int SlotOf(Widget w, out string part, out int arg)
	{
		part = "";
		arg = 0;
		int n = m_Tiles.Count();
		for (int i = 0; i < n; i++)
		{
			part = m_Tiles.Get(i).PartOf(w, arg);
			if (part != "")
				return i;
		}
		return -1;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
			return false;
		string part;
		int arg;
		int slot = SlotOf(w, part, arg);
		if (slot < 0)
			return w == m_BgHit;
		DRL_UITile tile = m_Tiles.Get(slot);
		DRL_UITileData d = tile.m_Data;
		if (tile.m_Index < 0 || !d)
			return true;
		if (part == "tile" && !d.m_Clickable)
			return true;
		DRL_UITileList keep = this;
		m_OnAction.Invoke(tile.m_Index, part, arg, this);
		return true;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		string part;
		int arg;
		if (w != m_BgHit && SlotOf(w, part, arg) < 0)
			return false;
		ScrollTo(m_First - wheel);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		string part;
		int arg;
		int slot = SlotOf(w, part, arg);
		if (slot < 0)
			return false;
		if (slot != m_HoverSlot)
		{
			if (m_HoverSlot >= 0 && m_HoverSlot < m_Tiles.Count())
				m_Tiles.Get(m_HoverSlot).SetHover(false);
			m_HoverSlot = slot;
			DRL_UITile tile = m_Tiles.Get(slot);
			tile.SetHover(true);
			if (m_Tooltips && tile.m_Data && tile.m_Data.m_Title != "")
				INT_UIF_Tooltip.ShowFor(tile.m_Root, tile.m_Data.m_Title);
			m_OnHover.Invoke(tile.m_Index, this);
		}
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		string part;
		int arg;
		int slot = SlotOf(w, part, arg);
		if (slot < 0)
			return false;
		// Переход между частями одной плитки — не уход.
		string nextPart;
		int nextArg;
		if (enterW && SlotOf(enterW, nextPart, nextArg) == slot)
			return true;
		m_Tiles.Get(slot).SetHover(false);
		if (m_HoverSlot == slot)
		{
			m_HoverSlot = -1;
			if (m_Tooltips)
				INT_UIF_Tooltip.HideTip();
			m_OnHover.Invoke(-1, this);
		}
		return true;
	}
}

// --- полоса категорий ----------------------------------------------------------------
class DRL_UIRailButton extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ImageWidget m_Icon;
	ButtonWidget m_Hit;
	int m_Entry = -1;
	bool m_Hover;
	bool m_Selected;
}

// Иконки категорий столбиком слева от списка рецептов (как в старом меню).
// Имя и число рецептов — в подсказке. Хит-зоны DRL_Cat_<n> для dayz-mcp.
class DRL_UICategoryRail extends ScriptedWidgetEventHandler
{
	static const float GAP = 6;

	protected Widget m_Root;
	protected ButtonWidget m_BgHit;
	protected ImageWidget m_Thumb;
	protected ref array<ref DRL_UIRailButton> m_Buttons;
	protected ref array<ref DRL_UICategoryEntry> m_Entries;
	protected int m_First;
	protected int m_Selected = -1;
	protected float m_Size;
	protected float m_H;
	protected ref ScriptInvoker m_OnSelected;

	void DRL_UICategoryRail()
	{
		m_Buttons = new array<ref DRL_UIRailButton>();
		m_Entries = new array<ref DRL_UICategoryEntry>();
		m_OnSelected = new ScriptInvoker();
	}

	// (int entryIndex, DRL_UICategoryRail rail)
	ScriptInvoker OnSelected()
	{
		return m_OnSelected;
	}

	void Build(Widget parent, float x, float y, float size, float h)
	{
		m_Size = size;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, size + 6, h, "DRL_CatRail");
		m_BgHit = INT_UIF_Draw.Hit(m_Root, 0, 0, size, h, this);
		m_BgHit.SetName("DRL_CatRailBg");
		int visible = (int)((h + GAP) / (size + GAP));
		for (int i = 0; i < visible; i++)
		{
			DRL_UIRailButton b = new DRL_UIRailButton();
			b.m_Root = INT_UIF_Draw.Frame(m_Root, 0, i * (size + GAP), size, size, "DRL_CatBtn");
			b.m_Bg = INT_UIF_Surface.Box(b.m_Root, 0, 0, size, size, 10, 0, 0);
			float isz = size - 20;
			b.m_Icon = INT_UIF_Draw.Image(b.m_Root, (size - isz) * 0.5, (size - isz) * 0.5, isz, isz, "iuf_white", 0);
			b.m_Hit = INT_UIF_Draw.Hit(b.m_Root, 0, 0, size, size, this);
			DRL_UIKitUtil.ExposeHit(b.m_Hit, "DRL_Cat_" + i.ToString(), this);
			b.m_Root.Show(false);
			m_Buttons.Insert(b);
		}
		m_Thumb = INT_UIF_Draw.Line(m_Root, size + 3, 0, 3, 40, INT_UIF.Soft(INT_UIF_ThemeKeys.TEXT, 50));
		m_Thumb.Show(false);
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
		for (int i = 0; i < m_Buttons.Count(); i++)
		{
			DRL_UIRailButton b = m_Buttons.Get(i);
			if (b.m_Entry >= 0)
				DRL_UIAnim.Reveal(b.m_Root, 0, i * (m_Size + GAP), 0.03 + i * 0.025, 8, 0.24);
		}
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

	protected void EnsureVisible(int index)
	{
		int visible = m_Buttons.Count();
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
		int m = m_Entries.Count() - m_Buttons.Count();
		if (m < 0)
			m = 0;
		return m;
	}

	protected void Rebind()
	{
		int n = m_Buttons.Count();
		for (int i = 0; i < n; i++)
		{
			DRL_UIRailButton b = m_Buttons.Get(i);
			int ei = m_First + i;
			DRL_UICategoryEntry e = GetEntry(ei);
			b.m_Entry = -1;
			if (!e)
			{
				b.m_Root.Show(false);
				continue;
			}
			b.m_Entry = ei;
			b.m_Root.Show(true);
			b.m_Selected = ei == m_Selected;
			if (e.m_IconPath != "")
			{
				b.m_Icon.LoadImageFile(0, e.m_IconPath);
				b.m_Icon.SetImage(0);
			}
			else
			{
				string glyph = e.m_Glyph;
				if (glyph == "")
					glyph = "list";
				INT_UIF_Widgets.SetIcon(b.m_Icon, glyph);
			}
			Paint(b);
		}
		int total = m_Entries.Count();
		bool scroll = total > n;
		m_Thumb.Show(scroll);
		if (scroll)
		{
			float th = m_H * n / total;
			float ty = 0;
			if (MaxFirst() > 0)
				ty = (m_H - th) * m_First / MaxFirst();
			INT_UIF_Draw.Place(m_Thumb, m_Size + 3, ty, 3, th);
		}
	}

	protected void Paint(DRL_UIRailButton b)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int fill = ARGB(0, 0, 0, 0);
		int border = ARGB(0, 0, 0, 0);
		int icon = tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		if (b.m_Hover)
		{
			fill = tm.Color(INT_UIF_ThemeKeys.TILE);
			border = tm.Color(INT_UIF_ThemeKeys.TILE_BORDER);
			icon = tm.Color(INT_UIF_ThemeKeys.TEXT);
		}
		if (b.m_Selected)
		{
			fill = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.AMBER), 36);
			border = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.AMBER), 115);
			icon = tm.Color(INT_UIF_ThemeKeys.AMBER);
		}
		b.m_Bg.SetFill(fill);
		b.m_Bg.SetBorder(border);
		b.m_Icon.SetColor(icon);
	}

	protected DRL_UIRailButton ButtonOf(Widget w)
	{
		foreach (DRL_UIRailButton b : m_Buttons)
		{
			if (b.m_Hit == w)
				return b;
		}
		return null;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		DRL_UIRailButton b = ButtonOf(w);
		if (!b || b.m_Entry < 0)
			return w == m_BgHit;
		if (b.m_Entry != m_Selected)
			Select(b.m_Entry, true);
		return true;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (w != m_BgHit && !ButtonOf(w))
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
		DRL_UIRailButton b = ButtonOf(w);
		if (!b || b.m_Entry < 0)
			return false;
		b.m_Hover = true;
		Paint(b);
		DRL_UICategoryEntry e = GetEntry(b.m_Entry);
		if (e)
		{
			string tip = DRL_UIText.Tr(e.m_Name);
			INT_UIF_Tooltip.ShowFor(b.m_Root, tip + " · " + e.m_Count.ToString());
		}
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		DRL_UIRailButton b = ButtonOf(w);
		if (!b)
			return false;
		b.m_Hover = false;
		Paint(b);
		INT_UIF_Tooltip.HideTip();
		return true;
	}
}

// --- слоты навесного результата -------------------------------------------------------
// Иконка слота из CfgSlots; бирюзовая — слот займёт выбранное доп. навесное.
class DRL_UISlotStrip extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ref array<ref INT_UIF_Surface> m_Bg;
	protected ref array<ImageWidget> m_Icons;
	protected ref array<ButtonWidget> m_Hits;
	protected ref array<string> m_Names;
	protected TextWidget m_More;
	protected TextWidget m_None;
	protected float m_Size;

	void DRL_UISlotStrip()
	{
		m_Bg = new array<ref INT_UIF_Surface>();
		m_Icons = new array<ImageWidget>();
		m_Hits = new array<ButtonWidget>();
		m_Names = new array<string>();
	}

	void Build(Widget parent, float x, float y, float w, float size)
	{
		m_Size = size;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, size, "DRL_SlotStrip");
		int n = (int)((w - 40 + 6) / (size + 6));
		for (int i = 0; i < n; i++)
		{
			float sx = i * (size + 6);
			INT_UIF_Surface bg = INT_UIF_Surface.Box(m_Root, sx, 0, size, size, 8, 0, 0);
			float isz = size - 14;
			ImageWidget icon = INT_UIF_Draw.Image(bg.GetRoot(), 7, 7, isz, isz, "iuf_white", 0);
			ButtonWidget hit = INT_UIF_Draw.Hit(bg.GetRoot(), 0, 0, size, size, this);
			hit.SetName("DRL_Slot_" + i.ToString());
			bg.Show(false);
			m_Bg.Insert(bg);
			m_Icons.Insert(icon);
			m_Hits.Insert(hit);
			m_Names.Insert("");
		}
		m_More = INT_UIF_Draw.Text(m_Root, n * (size + 6), 0, 40, size, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		m_None = INT_UIF_Draw.Text(m_Root, 2, 0, w, size, "#STR_DRL_UI_NO_SLOTS", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
	}

	// filled — те же индексы, что у slots.
	void SetSlots(array<string> slots, array<bool> filled)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int total = 0;
		if (slots)
			total = slots.Count();
		for (int i = 0; i < m_Bg.Count(); i++)
		{
			INT_UIF_Surface bg = m_Bg.Get(i);
			bool on = i < total;
			bg.Show(on);
			if (!on)
			{
				m_Names.Set(i, "");
				continue;
			}
			string slot = slots.Get(i);
			string path = "set:dayz_inventory image:missing";
			string iconPath;
			if (DRL_CraftStatsHelper.GetSlotIconPath(slot, iconPath))
				path = DRL_CraftUIHelper.ResolveImagePath(iconPath);
			ImageWidget icon = m_Icons.Get(i);
			icon.LoadImageFile(0, path);
			icon.SetImage(0);
			bool isFilled = filled && i < filled.Count() && filled.Get(i);
			if (isFilled)
			{
				bg.SetFill(INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.TEAL), 30));
				bg.SetBorder(INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.TEAL), 110));
				icon.SetColor(tm.Color(INT_UIF_ThemeKeys.TEAL));
			}
			else
			{
				bg.SetFill(tm.Color(INT_UIF_ThemeKeys.TILE));
				bg.SetBorder(tm.Color(INT_UIF_ThemeKeys.TILE_BORDER));
				icon.SetColor(INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED), 220));
			}
			string name = slot;
			int slotId = InventorySlots.GetSlotIdFromString(slot);
			if (slotId != InventorySlots.INVALID)
			{
				string dn = InventorySlots.GetSlotDisplayName(slotId);
				if (dn != "")
					name = dn;
			}
			m_Names.Set(i, name);
		}
		int hidden = total - m_Bg.Count();
		if (hidden > 0)
			m_More.SetText("+" + hidden.ToString());
		else
			m_More.SetText("");
		m_None.Show(total == 0);
	}

	protected int SlotOf(Widget w)
	{
		for (int i = 0; i < m_Hits.Count(); i++)
		{
			if (m_Hits.Get(i) == w)
				return i;
		}
		return -1;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int i = SlotOf(w);
		if (i < 0 || m_Names.Get(i) == "")
			return false;
		INT_UIF_Tooltip.ShowFor(m_Bg.Get(i).GetRoot(), m_Names.Get(i));
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (SlotOf(w) < 0)
			return false;
		INT_UIF_Tooltip.HideTip();
		return true;
	}
}

// --- карточка ингредиента (= подсказка IngridientDetai* старого меню) -----------------
// Показывает наведённую плитку и остаётся на ней, когда курсор ушёл.
class DRL_UIIngrCard extends Managed
{
	static const int ROWS = 6;

	protected Widget m_Root;
	protected Widget m_Body;
	protected ref DRL_UIItemView m_View;
	protected ImageWidget m_Glyph;
	protected TextWidget m_Name;
	protected MultilineTextWidget m_Sub;
	protected TextWidget m_Empty;
	protected ref array<TextWidget> m_Labels;
	protected ref array<TextWidget> m_Values;

	void DRL_UIIngrCard()
	{
		m_Labels = new array<TextWidget>();
		m_Values = new array<TextWidget>();
	}

	void ~DRL_UIIngrCard()
	{
		Clear();
	}

	// Высота под шесть строк: 72 + 6 * 21 + 8 = 206.
	void Build(Widget parent, float x, float y, float w, float h)
	{
		int text = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT);
		int muted = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);
		int tileFill = INT_UIF.Color(INT_UIF_ThemeKeys.TILE);
		int tileBorder = INT_UIF.Color(INT_UIF_ThemeKeys.TILE_BORDER);
		int panelFill = INT_UIF.Color(INT_UIF_ThemeKeys.PANEL);
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_IngrCard");
		INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 10, tileFill, tileBorder);
		m_Body = INT_UIF_Draw.Frame(m_Root, 0, 0, w, h, "DRL_IngrCardBody");
		INT_UIF_Surface.Box(m_Body, 12, 12, 58, 50, 8, panelFill, 0);
		Widget clip = INT_UIF_Draw.Clip(m_Body, 14, 14, 54, 46, "DRL_IngrCardClip");
		m_View = new DRL_UIItemView();
		m_View.Create(clip, true);
		m_View.SetOrientation(Vector(0, 30, 0));
		m_Glyph = INT_UIF_Draw.Icon(m_Body, 31, 27, 20, "info", faint);
		m_Glyph.Show(false);
		m_Name = INT_UIF_Draw.Text(m_Body, 82, 10, w - 94, 24, "", 15, text);
		m_Name.SetBold(true);
		// Две строки: подсказка про варианты и короткое описание длиннее одной.
		// MultilineText сам не обрезает — лишние строки режет рамка.
		Widget subClip = INT_UIF_Draw.Clip(m_Body, 82, 35, w - 94, 26, "DRL_IngrCardSub");
		m_Sub = INT_UIF_Draw.MText(subClip, 0, 0, w - 94, 48, "", 12, muted);
		float ry = 74;
		for (int i = 0; i < ROWS; i++)
		{
			TextWidget lb = INT_UIF_Draw.Text(m_Body, 14, ry, w * 0.55, 19, "", 13, muted);
			TextWidget vl = INT_UIF_Draw.Text(m_Body, w * 0.4, ry, w * 0.6 - 14, 19, "", 13, text, INT_UIF_Align.RIGHT);
			vl.SetBold(true);
			m_Labels.Insert(lb);
			m_Values.Insert(vl);
			ry = ry + 21;
		}
		m_Empty = INT_UIF_Draw.Text(m_Root, 10, h * 0.5 - 10, w - 20, 20, "#STR_DRL_UI_ING_HINT", 13, faint, INT_UIF_Align.CENTER);
		ShowEmpty();
	}

	protected void SetRow(int i, string label, string value, int color = 0)
	{
		if (i < 0 || i >= ROWS)
			return;
		m_Labels.Get(i).SetText(label);
		m_Values.Get(i).SetText(value);
		if (color == 0)
			color = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT);
		m_Values.Get(i).SetColor(color);
	}

	// cls — показываемый класс; c — параметры компонента (как FillIngredientDetailInfo).
	void ShowFor(string cls, DRL_Craft_Component c, string needText, string haveText, int haveColor, bool hasVariants)
	{
		if (!c)
		{
			ShowEmpty();
			return;
		}
		m_Empty.Show(false);
		m_Body.Show(true);
		bool ok = m_View.SetClass(cls);
		m_Glyph.Show(!ok);
		m_Name.SetText(DRL_UIText.DisplayName(cls));
		if (hasVariants)
		{
			m_Sub.SetText("#STR_DRL_UI_ING_VARIANTS");
			m_Sub.SetColor(INT_UIF.Color(INT_UIF_ThemeKeys.AMBER_TEXT));
		}
		else
		{
			string ds = "";
			if (!DRL_CraftUIHelper.GetDescriptionShortForClass(cls, ds))
				ds = "";
			m_Sub.SetText(ds);
			m_Sub.SetColor(INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		}
		string fill = "—";
		if (c.QuantityFrac > 0.0)
			fill = DRL_UIText.Pct(c.QuantityFrac);
		string wear = "—";
		if (c.ApplyDamageFrac > 0.0)
			wear = DRL_UIText.Pct(c.ApplyDamageFrac);
		string consumed = DRL_UIText.Tr("#STR_DRL_UI_NO");
		if (c.Delete)
			consumed = DRL_UIText.Tr("#STR_DRL_UI_YES");
		string minState = DRL_UIText.MinHealth(c.MinHealthFrac);
		SetRow(0, "#STR_DRL_UI_ING_NEED", needText);
		SetRow(1, "#STR_DRL_UI_ING_HAVE", haveText, haveColor);
		SetRow(2, "#STR_DRL_UI_ING_MINSTATE", minState);
		SetRow(3, "#STR_DRL_UI_ING_FILL", fill);
		SetRow(4, "#STR_DRL_UI_ING_WEAR", wear);
		SetRow(5, "#STR_DRL_UI_ING_CONSUMED", consumed);
	}

	void ShowEmpty()
	{
		if (m_View)
			m_View.Clear();
		if (m_Body)
			m_Body.Show(false);
		if (m_Empty)
			m_Empty.Show(true);
	}

	void Clear()
	{
		if (m_View)
			m_View.Clear();
	}
}
