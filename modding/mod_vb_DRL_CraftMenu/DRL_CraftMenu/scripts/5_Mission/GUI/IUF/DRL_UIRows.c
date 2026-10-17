// ============================================================================
// DRL_CraftMenu - список строк панели деталей.
//
// Один компонент на ингредиенты, инструменты, доп. навесное, разбор,
// требования и вариации. Пул строк фиксированной высоты над массивом данных,
// прокрутка колесом по строке (как сетка и сайдбар): одновременно живут
// только видимые 3D-превью.
// Строка: полоса статуса, мини-превью (3D / картинка / иконка), название,
// подпись, правый текст; по данным — чекбокс или замок, номера вариантов,
// степпер количества, кнопка «к рецепту». Заголовок раздела и пояснение —
// отдельные виды строки.
// Хит-зоны: <префикс>_<слот>, а также _chk, _minus, _plus, _info, _v<n>.
// ============================================================================

const int DRL_UI_ROW_ITEM = 0;
const int DRL_UI_ROW_HEADER = 1;
const int DRL_UI_ROW_NOTE = 2;

const int DRL_UI_ROW_MAX_VARS = 8;

class DRL_UIRowData extends Managed
{
	int m_Kind;
	string m_Title;
	string m_Sub;
	string m_Right;          // правый текст (есть/нужно, статус)
	int m_RightColor;
	int m_Accent;            // цвет полосы статуса (0 — без полосы)
	string m_PreviewClass;   // 3D-превью
	string m_IconPath;       // картинка вместо 3D (CraftIcon)
	string m_Glyph;          // иконка фреймворка, если нет ни 3D, ни картинки
	string m_Badge;          // значок на превью (swap — есть вариации)
	bool m_Selected;
	bool m_Clickable = true;
	bool m_Dim;
	int m_Check = -1;        // -1 без чекбокса, 0/1
	bool m_Locked;           // замок вместо чекбокса
	int m_Qty;               // 0 — без степпера
	int m_VarCount;          // > 1 — номера вариантов в строке подписи
	int m_VarSelected;
	bool m_Info;             // кнопка «к рецепту»
	bool m_InfoEnabled = true;
	int m_Tag = -1;          // индекс в данных владельца

	static DRL_UIRowData Header(string title, string right = "")
	{
		DRL_UIRowData d = new DRL_UIRowData();
		d.m_Kind = DRL_UI_ROW_HEADER;
		d.m_Title = title;
		d.m_Right = right;
		d.m_Clickable = false;
		return d;
	}

	static DRL_UIRowData Note(string text)
	{
		DRL_UIRowData d = new DRL_UIRowData();
		d.m_Kind = DRL_UI_ROW_NOTE;
		d.m_Title = text;
		d.m_Clickable = false;
		return d;
	}
}

class DRL_UIRow extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ImageWidget m_Bar;
	Widget m_Thumb;
	ref INT_UIF_Surface m_ThumbBg;
	ref DRL_UIItemView m_View;
	ImageWidget m_Image;
	ImageWidget m_Glyph;
	ImageWidget m_Badge;
	TextWidget m_Title;
	TextWidget m_Sub;
	TextWidget m_Right;
	TextWidget m_Header;
	TextWidget m_HeaderRight;
	ImageWidget m_HeaderLine;
	TextWidget m_Note;
	ButtonWidget m_Hit;

	Widget m_CheckRoot;
	ref INT_UIF_Surface m_CheckBg;
	ImageWidget m_CheckIcon;
	ButtonWidget m_CheckHit;

	Widget m_QtyRoot;
	ref INT_UIF_Surface m_QtyBg;
	TextWidget m_QtyText;
	ButtonWidget m_MinusHit;
	ButtonWidget m_PlusHit;

	Widget m_InfoRoot;
	ref INT_UIF_Surface m_InfoBg;
	ImageWidget m_InfoIcon;
	ButtonWidget m_InfoHit;

	ref array<ref INT_UIF_Surface> m_VarBg;
	ref array<TextWidget> m_VarText;
	ref array<ButtonWidget> m_VarHit;

	DRL_UIRowData m_Data;
	float m_W;
	float m_H;
	float m_ThumbS;
	int m_Index = -1;
	bool m_Hover;

	void Build(Widget parent, int slot, float y, float w, float h, string prefix, ScriptedWidgetEventHandler handler)
	{
		m_W = w;
		m_H = h;
		m_ThumbS = h - 12;
		string base = prefix + "_" + slot.ToString();
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);

		m_Root = INT_UIF_Draw.Frame(parent, 0, y, w, h, "DRL_Row");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 8, 0, 0);
		m_Bar = INT_UIF_Draw.Line(m_Root, 0, 9, 3, h - 18, 0);

		float ts = m_ThumbS;
		m_Thumb = INT_UIF_Draw.Frame(m_Root, 10, 6, ts, ts, "DRL_RowThumb");
		int thumbFill = ARGB(60, 0, 0, 0);
		int thumbBorder = ARGB(18, 255, 255, 255);
		m_ThumbBg = INT_UIF_Surface.Box(m_Thumb, 0, 0, ts, ts, 6, thumbFill, thumbBorder);
		Widget clip = INT_UIF_Draw.Clip(m_Thumb, 2, 2, ts - 4, ts - 4, "DRL_RowClip");
		m_View = new DRL_UIItemView();
		m_View.Create(clip, true);
		m_Image = INT_UIF_Draw.Image(m_Thumb, 4, 4, ts - 8, ts - 8, "iuf_white", ARGB(235, 255, 255, 255));
		m_Image.Show(false);
		m_Glyph = INT_UIF_Draw.Icon(m_Thumb, (ts - 18) * 0.5, (ts - 18) * 0.5, 18, "info", faint);
		m_Glyph.Show(false);
		m_Badge = INT_UIF_Draw.Icon(m_Thumb, ts - 15, 1, 14, "swap", INT_UIF.Color(INT_UIF_ThemeKeys.AMBER));
		m_Badge.SetSort(DRL_UI_SORT_OVER_MODEL);
		m_Badge.Show(false);

		m_Title = INT_UIF_Draw.Text(m_Root, 0, 6, 10, 20, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_Sub = INT_UIF_Draw.Text(m_Root, 0, 26, 10, 18, "", 11, faint);
		m_Right = INT_UIF_Draw.Text(m_Root, 0, 0, 10, h, "", 13, 0, INT_UIF_Align.RIGHT);
		m_Right.SetBold(true);

		m_Header = INT_UIF_Draw.Text(m_Root, 4, h - 26, w - 8, 18, "", 11, faint);
		m_Header.SetBold(true);
		m_HeaderRight = INT_UIF_Draw.Text(m_Root, 4, h - 26, w - 8, 18, "", 11, faint, INT_UIF_Align.RIGHT);
		m_HeaderLine = INT_UIF_Draw.Line(m_Root, 4, h - 5, w - 8, 1, INT_UIF.Color(INT_UIF_ThemeKeys.DIVIDER));
		m_Note = INT_UIF_Draw.Text(m_Root, 12, 0, w - 24, h, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));

		// Основная хит-зона ниже дополнительных: те создаются позже и лежат сверху.
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, base, handler);

		m_CheckRoot = INT_UIF_Draw.Frame(m_Root, 10, (h - 22) * 0.5, 22, 22, "DRL_RowCheck");
		m_CheckBg = INT_UIF_Surface.Box(m_CheckRoot, 0, 0, 22, 22, 6, 0, 0);
		m_CheckIcon = INT_UIF_Draw.Icon(m_CheckRoot, 4, 4, 14, "check", 0);
		m_CheckHit = INT_UIF_Draw.Hit(m_CheckRoot, -4, -4, 30, 30, handler);
		DRL_UIKitUtil.ExposeHit(m_CheckHit, base + "_chk", handler);

		m_QtyRoot = INT_UIF_Draw.Frame(m_Root, 0, (h - 26) * 0.5, 84, 26, "DRL_RowQty");
		m_QtyBg = INT_UIF_Surface.Box(m_QtyRoot, 0, 0, 84, 26, 6, INT_UIF.Color(INT_UIF_ThemeKeys.TILE), ARGB(24, 255, 255, 255));
		INT_UIF_Draw.Icon(m_QtyRoot, 6, 6, 14, "minus", INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		INT_UIF_Draw.Icon(m_QtyRoot, 64, 6, 14, "plus", INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		m_QtyText = INT_UIF_Draw.Text(m_QtyRoot, 24, 0, 36, 26, "1", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT), INT_UIF_Align.CENTER);
		m_QtyText.SetBold(true);
		m_MinusHit = INT_UIF_Draw.Hit(m_QtyRoot, 0, 0, 26, 26, handler);
		DRL_UIKitUtil.ExposeHit(m_MinusHit, base + "_minus", handler);
		m_PlusHit = INT_UIF_Draw.Hit(m_QtyRoot, 58, 0, 26, 26, handler);
		DRL_UIKitUtil.ExposeHit(m_PlusHit, base + "_plus", handler);

		m_InfoRoot = INT_UIF_Draw.Frame(m_Root, w - 38, (h - 28) * 0.5, 28, 28, "DRL_RowInfo");
		m_InfoBg = INT_UIF_Surface.Box(m_InfoRoot, 0, 0, 28, 28, 6, INT_UIF.Color(INT_UIF_ThemeKeys.TILE), ARGB(24, 255, 255, 255));
		m_InfoIcon = INT_UIF_Draw.Icon(m_InfoRoot, 7, 7, 14, "chevron_right", INT_UIF.Color(INT_UIF_ThemeKeys.TEAL));
		m_InfoHit = INT_UIF_Draw.Hit(m_InfoRoot, 0, 0, 28, 28, handler);
		DRL_UIKitUtil.ExposeHit(m_InfoHit, base + "_info", handler);

		m_VarBg = new array<ref INT_UIF_Surface>();
		m_VarText = new array<TextWidget>();
		m_VarHit = new array<ButtonWidget>();
		for (int v = 0; v < DRL_UI_ROW_MAX_VARS; v++)
		{
			INT_UIF_Surface vb = INT_UIF_Surface.Box(m_Root, 0, 27, 22, 17, 4, 0, 0);
			int num = v + 1;
			TextWidget vt = INT_UIF_Draw.Text(vb.GetRoot(), 0, 0, 22, 17, num.ToString(), 11, 0, INT_UIF_Align.CENTER);
			ButtonWidget vh = INT_UIF_Draw.Hit(vb.GetRoot(), 0, 0, 22, 17, handler);
			DRL_UIKitUtil.ExposeHit(vh, base + "_v" + v.ToString(), handler);
			m_VarBg.Insert(vb);
			m_VarText.Insert(vt);
			m_VarHit.Insert(vh);
		}
	}

	// Какая часть строки под виджетом: "row", "check", "minus", "plus", "info", "var".
	string PartOf(Widget w, out int arg)
	{
		arg = 0;
		if (!w)
			return "";
		if (w == m_Hit)
			return "row";
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

	void Bind(int index, DRL_UIRowData d)
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
		bool item = d.m_Kind == DRL_UI_ROW_ITEM;
		bool header = d.m_Kind == DRL_UI_ROW_HEADER;
		bool note = d.m_Kind == DRL_UI_ROW_NOTE;

		m_Bg.Show(item);
		m_Bar.Show(item && d.m_Accent != 0);
		m_Thumb.Show(item);
		m_Title.Show(item);
		m_Sub.Show(item);
		m_Right.Show(item);
		m_Hit.Show(item);
		m_Header.Show(header);
		m_HeaderRight.Show(header);
		m_HeaderLine.Show(header);
		m_Note.Show(note);
		if (!item)
		{
			m_CheckRoot.Show(false);
			m_QtyRoot.Show(false);
			m_InfoRoot.Show(false);
			ShowVars(0, 0, 0);
			m_View.Clear();
			m_Root.SetAlpha(1);
			if (header)
			{
				m_Header.SetText(d.m_Title);
				m_HeaderRight.SetText(d.m_Right);
			}
			else
			{
				m_Note.SetText(d.m_Title);
			}
			return;
		}

		if (d.m_Accent != 0)
			m_Bar.SetColor(d.m_Accent);

		// Слева: чекбокс/замок, превью.
		float cx = 10;
		bool hasCheck = d.m_Check >= 0 || d.m_Locked;
		m_CheckRoot.Show(hasCheck);
		if (hasCheck)
		{
			PaintCheck(d);
			cx = 40;
		}
		INT_UIF_Draw.Move(m_Thumb, cx, 6);
		BindThumb(d);
		float tx = cx + m_ThumbS + 10;

		// Справа налево: «к рецепту», количество, правый текст.
		float re = m_W - 10;
		m_InfoRoot.Show(d.m_Info);
		if (d.m_Info)
		{
			INT_UIF_Draw.Move(m_InfoRoot, re - 28, (m_H - 28) * 0.5);
			if (d.m_InfoEnabled)
				m_InfoIcon.SetColor(INT_UIF.Color(INT_UIF_ThemeKeys.TEAL));
			else
				m_InfoIcon.SetColor(INT_UIF.Color(INT_UIF_ThemeKeys.RED));
			m_InfoRoot.SetAlpha(1);
			if (!d.m_InfoEnabled)
				m_InfoRoot.SetAlpha(0.5);
			re = re - 36;
		}
		m_QtyRoot.Show(d.m_Qty > 0);
		if (d.m_Qty > 0)
		{
			INT_UIF_Draw.Move(m_QtyRoot, re - 84, (m_H - 26) * 0.5);
			m_QtyText.SetText(d.m_Qty.ToString());
			re = re - 92;
		}
		if (d.m_Right != "")
		{
			// Жирный текст и заглавные шире оценки TextWidth.
			float rw = INT_UIF_Draw.TextWidth(d.m_Right, 13) * 1.3 + 12;
			INT_UIF_Draw.Place(m_Right, re - rw, 0, rw, m_H);
			m_Right.SetText(d.m_Right);
			int rc = d.m_RightColor;
			if (rc == 0)
				rc = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
			m_Right.SetColor(rc);
			re = re - rw - 8;
		}
		else
		{
			m_Right.SetText("");
		}

		float tw = re - tx;
		if (tw < 40)
			tw = 40;
		INT_UIF_Draw.Place(m_Title, tx, 6, tw, 20);
		m_Title.SetText(d.m_Title);
		if (d.m_VarCount > 1)
		{
			m_Sub.Show(false);
			ShowVars(d.m_VarCount, d.m_VarSelected, tx);
		}
		else
		{
			ShowVars(0, 0, 0);
			INT_UIF_Draw.Place(m_Sub, tx, 26, tw, 18);
			m_Sub.SetText(d.m_Sub);
		}

		if (d.m_Dim)
			m_Root.SetAlpha(0.55);
		else
			m_Root.SetAlpha(1);
		Paint();
	}

	protected void BindThumb(DRL_UIRowData d)
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
		m_Badge.Show(d.m_Badge != "");
		if (d.m_Badge != "")
			INT_UIF_Widgets.SetIcon(m_Badge, d.m_Badge);
	}

	protected void PaintCheck(DRL_UIRowData d)
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
			m_CheckBg.SetFill(INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.GREEN), 70));
			m_CheckBg.SetBorder(INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.GREEN), 200));
			m_CheckIcon.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT));
			m_CheckIcon.Show(true);
		}
		else
		{
			m_CheckBg.SetFill(tm.Color(INT_UIF_ThemeKeys.TILE));
			int border = ARGB(60, 255, 255, 255);
			if (d.m_Dim)
				border = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.RED), 140);
			m_CheckBg.SetBorder(border);
			m_CheckIcon.Show(false);
		}
	}

	protected void ShowVars(int count, int selected, float x)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		for (int v = 0; v < m_VarBg.Count(); v++)
		{
			INT_UIF_Surface vb = m_VarBg.Get(v);
			bool on = v < count;
			vb.Show(on);
			if (!on)
				continue;
			INT_UIF_Draw.Move(vb.GetRoot(), x + v * 26, 27);
			TextWidget vt = m_VarText.Get(v);
			if (v == selected)
			{
				vb.SetFill(INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.AMBER), 200));
				vb.SetBorder(tm.Color(INT_UIF_ThemeKeys.AMBER));
				vt.SetColor(ARGB(255, 20, 20, 20));
			}
			else
			{
				vb.SetFill(ARGB(40, 255, 255, 255));
				vb.SetBorder(ARGB(30, 255, 255, 255));
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
		if (!m_Data || m_Data.m_Kind != DRL_UI_ROW_ITEM)
			return;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int fill = tm.Color(INT_UIF_ThemeKeys.TILE);
		int border = tm.Color(INT_UIF_ThemeKeys.TILE_BORDER);
		if (m_Data.m_Selected)
		{
			fill = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.AMBER), 36);
			border = INT_UIF_Color.WithAlpha(tm.Color(INT_UIF_ThemeKeys.AMBER), 190);
		}
		else if (m_Hover && m_Data.m_Clickable)
		{
			fill = tm.Color(INT_UIF_ThemeKeys.TILE_HOVER);
			border = INT_UIF.Soft(INT_UIF_ThemeKeys.TEAL, 90);
		}
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
	}

	void ShowModel(bool show)
	{
		m_View.Show(show);
	}

	void Clear()
	{
		if (m_View)
			m_View.Clear();
		m_Index = -1;
		m_Data = null;
	}
}

class DRL_UIRowList extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ButtonWidget m_BgHit;
	protected ImageWidget m_Track;
	protected ImageWidget m_Thumb;
	protected TextWidget m_EmptyText;
	protected string m_EmptyStr;
	protected ref array<ref DRL_UIRow> m_Rows;
	protected ref array<ref DRL_UIRowData> m_Items;
	protected float m_W;
	protected float m_H;
	protected float m_RowH;
	protected float m_Gap;
	protected int m_Visible;
	protected int m_First;
	protected int m_HoverSlot = -1;
	protected bool m_ModelsHidden;
	protected ref ScriptInvoker m_OnAction;
	protected ref ScriptInvoker m_OnHover;

	void DRL_UIRowList()
	{
		m_Rows = new array<ref DRL_UIRow>();
		m_Items = new array<ref DRL_UIRowData>();
		m_OnAction = new ScriptInvoker();
		m_OnHover = new ScriptInvoker();
	}

	void ~DRL_UIRowList()
	{
		ClearModels();
	}

	// (int index, string action, int arg, DRL_UIRowList list)
	ScriptInvoker OnAction()
	{
		return m_OnAction;
	}

	// (int index, DRL_UIRowList list); -1 — курсор ушёл со строк
	ScriptInvoker OnHover()
	{
		return m_OnHover;
	}

	// maxH — наибольшая высота области: под неё строится пул строк.
	void Build(Widget parent, float x, float y, float w, float maxH, float rowH, float gap, string prefix)
	{
		m_W = w;
		m_RowH = rowH;
		m_Gap = gap;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, maxH, "DRL_RowList");
		m_BgHit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, maxH, this);
		DRL_UIKitUtil.ExposeHit(m_BgHit, prefix + "_Bg", this);
		int pool = (int)((maxH + gap) / (rowH + gap));
		if (pool < 1)
			pool = 1;
		for (int i = 0; i < pool; i++)
		{
			DRL_UIRow row = new DRL_UIRow();
			row.Build(m_Root, i, i * (rowH + gap), w - 10, rowH, prefix, this);
			row.m_Root.Show(false);
			m_Rows.Insert(row);
		}
		m_Track = INT_UIF_Draw.Line(m_Root, w - 4, 0, 4, maxH, ARGB(14, 255, 255, 255));
		m_Thumb = INT_UIF_Draw.Line(m_Root, w - 4, 0, 4, 40, INT_UIF.Soft(INT_UIF_ThemeKeys.TEAL, 170));
		m_EmptyText = INT_UIF_Draw.Text(m_Root, 0, 0, w - 10, 40, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT), INT_UIF_Align.CENTER);
		m_EmptyText.Show(false);
		SetViewportH(maxH);
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

	void SetViewportH(float h)
	{
		m_H = h;
		m_Visible = (int)((h + m_Gap) / (m_RowH + m_Gap));
		if (m_Visible > m_Rows.Count())
			m_Visible = m_Rows.Count();
		if (m_Visible < 1)
			m_Visible = 1;
		INT_UIF_Draw.Resize(m_Root, m_W, h);
		INT_UIF_Draw.Resize(m_BgHit, m_W, h);
		INT_UIF_Draw.Resize(m_Track, 4, h);
		INT_UIF_Draw.Place(m_EmptyText, 0, h * 0.5 - 20, m_W - 10, 40);
		m_First = Math.Clamp(m_First, 0, MaxFirst());
		Rebind();
	}

	void SetEmptyText(string text)
	{
		m_EmptyStr = text;
		m_EmptyText.SetText(text);
	}

	void SetItems(array<ref DRL_UIRowData> items, bool keepScroll = false)
	{
		m_Items.Clear();
		if (items)
		{
			foreach (DRL_UIRowData d : items)
			{
				m_Items.Insert(d);
			}
		}
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

	DRL_UIRowData GetItem(int index)
	{
		if (index < 0 || index >= m_Items.Count())
			return null;
		return m_Items.Get(index);
	}

	protected int MaxFirst()
	{
		int m = m_Items.Count() - m_Visible;
		if (m < 0)
			m = 0;
		return m;
	}

	void ScrollTo(int first)
	{
		int target = Math.Clamp(first, 0, MaxFirst());
		if (target == m_First)
			return;
		m_First = target;
		Rebind();
	}

	protected void Rebind()
	{
		int n = m_Rows.Count();
		for (int i = 0; i < n; i++)
		{
			DRL_UIRow row = m_Rows.Get(i);
			int idx = m_First + i;
			if (i < m_Visible && idx < m_Items.Count())
			{
				row.Bind(idx, m_Items.Get(idx));
				row.ShowModel(!m_ModelsHidden);
				row.SetHover(i == m_HoverSlot);
			}
			else
			{
				row.Bind(-1, null);
			}
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
		float th = m_H * m_Visible / m_Items.Count();
		if (th < 30)
			th = 30;
		float ty = (m_H - th) * m_First / MaxFirst();
		INT_UIF_Draw.Place(m_Thumb, m_W - 4, ty, 4, th);
	}

	void HideModels(bool hide)
	{
		m_ModelsHidden = hide;
		foreach (DRL_UIRow row : m_Rows)
		{
			if (row.m_Index >= 0)
				row.ShowModel(!hide);
		}
	}

	void ClearModels()
	{
		foreach (DRL_UIRow row : m_Rows)
		{
			if (row)
				row.Clear();
		}
	}

	// Тестовый хук: действие над строкой без мыши.
	void TestAction(int index, string action, int arg)
	{
		if (index < 0 || index >= m_Items.Count())
			return;
		DRL_UIRowList keep = this;
		m_OnAction.Invoke(index, action, arg, this);
	}

	// --- события ------------------------------------------------------------------
	protected int SlotOf(Widget w, out string part, out int arg)
	{
		part = "";
		arg = 0;
		int n = m_Rows.Count();
		for (int i = 0; i < n; i++)
		{
			part = m_Rows.Get(i).PartOf(w, arg);
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
		DRL_UIRow row = m_Rows.Get(slot);
		DRL_UIRowData d = row.m_Data;
		if (row.m_Index < 0 || !d || d.m_Kind != DRL_UI_ROW_ITEM)
			return true;
		if (part == "row" && !d.m_Clickable)
			return true;
		DRL_UIRowList keep = this;
		m_OnAction.Invoke(row.m_Index, part, arg, this);
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
			if (m_HoverSlot >= 0 && m_HoverSlot < m_Rows.Count())
				m_Rows.Get(m_HoverSlot).SetHover(false);
			m_HoverSlot = slot;
			DRL_UIRow row = m_Rows.Get(slot);
			row.SetHover(true);
			m_OnHover.Invoke(row.m_Index, this);
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
		// Переход между частями одной строки — не уход.
		string nextPart;
		int nextArg;
		if (enterW && SlotOf(enterW, nextPart, nextArg) == slot)
			return true;
		m_Rows.Get(slot).SetHover(false);
		if (m_HoverSlot == slot)
		{
			m_HoverSlot = -1;
			m_OnHover.Invoke(-1, this);
		}
		return true;
	}
}
