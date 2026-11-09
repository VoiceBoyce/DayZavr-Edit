// Общие детали экранов кланов: эмблема, деньги, поля ввода, переключатель, список, окна поверх экрана.
class VBK
{
	// Списки общие с сервером: VB_ClanLook. Копия, чтобы экран мог её менять, не трогая общий список.
	static array<string> Shapes()
	{
		array<string> a = new array<string>();
		a.Copy(VB_ClanLook.ShapeIds());
		return a;
	}

	static array<string> Symbols()
	{
		array<string> a = new array<string>();
		a.Copy(VB_ClanLook.SymbolIds());
		return a;
	}

	static array<int> Colors()
	{
		array<int> a = new array<int>();
		a.Copy(VB_ClanLook.ColorIds());
		return a;
	}

	// Эмблема клана: форма и символ, символ тёмный по центру.
	static Widget Emblem(Widget p, float x, float y, float s, string shape, string symbol, int color)
	{
		Widget f = INT_UIF_Draw.Frame(p, x, y, s, s);
		VBT.Ico(f, 0, 0, s, "shp_" + shape, color);
		float k = s * 0.52;
		float up = 0;
		if (shape == "banner")
			up = s * 0.05;
		VBT.Ico(f, (s - k) * 0.5, (s - k) * 0.5 - up, k, "sym_" + symbol, 0xFF0E0D0C);
		return f;
	}

	// Ширина суммы. Цифры шире оценки TextWidth фреймворка: измерено на экране, 0,585 высоты на знак.
	static float MoneyW(string txt, float size)
	{
		return txt.Length() * size * 0.585;
	}

	// Сумма: монета и число, цвет золотой.
	static void Money(Widget p, float x, float y, int val, float size, int color = VB_C_GOLD)
	{
		float s = Math.Round(size * 0.95);
		VBT.Ico(p, x, y + (size * 1.3 - s) * 0.5, s, "coin", color);
		string txt = VBT.Num(val);
		INT_UIF_Draw.Text(p, x + s + Math.Round(size * 0.35), y, MoneyW(txt, size) + 24, size * 1.3, txt, size, color);
	}

	// Сумма по правому краю: right это правая граница числа, число прижато к ней самим виджетом.
	static void MoneyR(Widget p, float right, float y, int val, float size, int color = VB_C_GOLD)
	{
		float s = Math.Round(size * 0.95);
		string txt = VBT.Num(val);
		float w = MoneyW(txt, size);
		INT_UIF_Draw.Text(p, right - w - 20, y, w + 20, size * 1.3, txt, size, color, INT_UIF_Align.RIGHT);
		VBT.Ico(p, right - w - Math.Round(size * 0.35) - s, y + (size * 1.3 - s) * 0.5, s, "coin", color);
	}

	// Сумма по центру: cx это середина монеты с числом.
	static void MoneyC(Widget p, float cx, float y, int val, float size, int color = VB_C_GOLD)
	{
		float s = Math.Round(size * 0.95);
		float full = s + Math.Round(size * 0.35) + MoneyW(VBT.Num(val), size);
		Money(p, cx - full * 0.5, y, val, size, color);
	}

	// Плитка показателя: подпись, значение, значок справа.
	static Widget Tile(Widget p, float x, float y, float w, float h, string cap, string val, string icon, int color = VB_C_TEXT)
	{
		Widget f = VBT.Card(p, x, y, w, h);
		VBT.Cap(f, 20, 18, w - 64, cap);
		if (icon != "")
			VBT.Ico(f, w - 42, 16, 22, icon, VB_C_DIM2);
		float size = 26;
		if (h >= 110)
			size = 30;
		INT_UIF_Draw.Text(f, 20, h - size - 24, w - 40, size + 8, val, size, color);
		return f;
	}

	// Ширина плашки Pill: подпись по метрикам движка и по 14 с каждой стороны.
	static float PillW(Widget p, string text)
	{
		return VBT.TextW(p, text, 13) + 28;
	}

	// Плашка роли, возвращает ширину.
	static float Pill(Widget p, float x, float y, string text, int color, int fill = 0)
	{
		float w = PillW(p, text);
		INT_UIF_Surface.Box(p, x, y, w, 24, 12, fill, color);
		INT_UIF_Draw.Text(p, x, y, w, 24, text, 13, color, INT_UIF_Align.CENTER);
		return w;
	}

	// Столбик из сегментов: вверх от оси плюс, вниз минус. k от 0 до 1.
	static void Column(Widget p, float x, float axis, float w, float k, bool neg)
	{
		int up = 10;
		int down = 4;
		int i;
		for (i = 0; i < up; i++)
		{
			int c = VB_C_PANEL3;
			if (!neg && k > 0 && i < Math.Round(up * k))
				c = VB_C_GOLD;
			INT_UIF_Draw.Line(p, x, axis - 9 - i * 9, w, 6, c);
		}
		int n = Math.Max(1, Math.Round(down * k));
		for (i = 0; i < down; i++)
		{
			int d = VB_C_PANEL2;
			if (neg && i < n)
				d = VB_C_BAD;
			INT_UIF_Draw.Line(p, x, axis + 4 + i * 9, w, 6, d);
		}
	}

	static void Bar(Widget p, float x, float y, float w, float h, float k, int color = 0)
	{
		if (color == 0)
			color = VB_C_ACCENT;
		INT_UIF_Surface.Pill(p, x, y, w, h, VB_C_PANEL3);
		float on = w * Math.Clamp(k, 0, 1);
		if (on >= h)
			INT_UIF_Surface.Pill(p, x, y, on, h, color);
	}

	static void Copy(string text)
	{
		GetGame().CopyToClipboard(text);
		INT_UIF_Toast.Success(VBT.T("#STR_VB_TAB_COPIED") + " " + text);
	}

	// Текст ответа сервера.
	static string ResultText(int code)
	{
		return "#STR_VB_TAB_R_" + code.ToString();
	}

	static bool ResultIsGood(int code)
	{
		return code == VB_TabletResult.OK || code == VB_TabletResult.CREATED || code == VB_TabletResult.SAVED || code == VB_TabletResult.SENT;
	}

	// Список с прокруткой колесом и тонким ползунком в цвете планшета.
	static ScrollWidget Scroll(Widget parent, float x, float y, float w, float h)
	{
		ScrollWidget sw = ScrollWidget.Cast(INT_UIF_Widgets.CreatePath(VB_TABLET_LAYOUT_SCROLL, parent));
		INT_UIF_Draw.Place(sw, x, y, w, h);
		ImageWidget track = INT_UIF_Draw.Texture(parent, x + w - 5, y, 4, h, VB_TABLET_ICON_EM + "solid", VB_C_LINE);
		ImageWidget thumb = INT_UIF_Draw.Texture(parent, x + w - 5, y, 4, 40, VB_TABLET_ICON_EM + "solid", VB_C_ACCENT);
		track.Show(false);
		thumb.Show(false);
		VB_TabletMenu menu = Menu();
		if (menu)
			menu.AddScrollThumb(new VBT_ScrollThumb(sw, track, thumb, x + w - 5, y, h));
		return sw;
	}

	static void ScrollTo(ScrollWidget sw, float pos)
	{
		VB_TabletMenu menu = Menu();
		if (!menu)
			return;
		VBT_ScrollThumb t = menu.ThumbOf(sw);
		if (t)
			t.To(pos);
	}

	static float ScrollPos(ScrollWidget sw)
	{
		VB_TabletMenu menu = Menu();
		if (!menu)
			return 0;
		VBT_ScrollThumb t = menu.ThumbOf(sw);
		if (t)
			return t.Pos();
		return 0;
	}

	static VB_TabletMenu Menu()
	{
		return VB_TabletMenu.Cast(GetGame().GetUIManager().GetMenu());
	}
}

// Прокрутка списка целиком своя. ScrollWidget с выключенной полосой не двигается ни колесом, ни VScrollToPos,
// поэтому он только обрезает, а смещение держится здесь: контент сдвигается вверх на m_Pos.
// Все значения в единицах холста, как у INT_UIF_Draw.
class VBT_ScrollThumb extends Managed
{
	static const float WHEEL_STEP = 90;

	ScrollWidget List;
	ImageWidget Thumb;
	ImageWidget Track;
	protected Widget m_Content;
	protected float m_X;
	protected float m_Y;
	protected float m_H;
	protected float m_Pos;
	protected float m_Shown;
	protected bool m_Drag;
	protected float m_Grab;

	void VBT_ScrollThumb(ScrollWidget list, ImageWidget track, ImageWidget thumb, float x, float y, float h)
	{
		List = list;
		Track = track;
		Thumb = thumb;
		m_Content = INT_UIF_Draw.ScrollContent(list);
		m_X = x;
		m_Y = y;
		m_H = h;
	}

	// Высота из заданного размера, а не из GetContentHeight: тот меняется вместе со смещением.
	protected float Full()
	{
		float w;
		float h;
		m_Content.GetSize(w, h);
		return h / INT_UIF_Scale.Factor();
	}

	float Pos()
	{
		return m_Pos;
	}

	// Значение за концом списка ограничится в Update, поэтому «в самый низ» можно задать любым большим числом.
	void To(float pos)
	{
		m_Pos = pos;
	}

	void Wheel(int wheel)
	{
		m_Pos -= wheel * WHEEL_STEP;
	}

	// Содержит ли список этот виджет: колесо крутит только то, что под курсором и не закрыто окном.
	bool Owns(Widget w)
	{
		while (w)
		{
			if (w == List)
				return true;
			w = w.GetParent();
		}
		return false;
	}

	// Перетаскивание ползунка и переход по клику на дорожке: кнопка опрашивается, у картинок нет событий нажатия.
	protected void Drag(float rest)
	{
		if ((GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) == 0 || !Thumb.IsVisibleHierarchy())
		{
			m_Drag = false;
			return;
		}
		float f = INT_UIF_Scale.Factor();
		int mx;
		int my;
		GetMousePos(mx, my);
		float tx;
		float ty;
		float tw;
		float th;
		float bx;
		float by;
		float bw;
		float bh;
		Thumb.GetScreenPos(tx, ty);
		Thumb.GetScreenSize(tw, th);
		Track.GetScreenPos(bx, by);
		Track.GetScreenSize(bw, bh);
		if (!m_Drag)
		{
			if (mx < bx - 6 * f || mx > bx + bw + 6 * f || my < by || my > by + bh)
				return;
			m_Drag = true;
			m_Grab = th * 0.5;
			if (my >= ty && my <= ty + th)
				m_Grab = my - ty;
		}
		float travel = bh - th;
		if (travel > 0)
			m_Pos = (my - m_Grab - by) / travel * rest;
	}

	void Update()
	{
		float full = Full();
		float rest = Math.Max(0, full - m_H);
		bool need = rest > 1;
		Track.Show(need);
		Thumb.Show(need);
		if (need)
			Drag(rest);
		m_Pos = Math.Clamp(m_Pos, 0, rest);
		if (Math.AbsFloat(m_Pos - m_Shown) > 0.01)
		{
			m_Shown = m_Pos;
			m_Content.SetPos(0, -Math.Round(INT_UIF_Draw.S(m_Pos)));
		}
		if (!need)
			return;
		float size = Math.Max(36, m_H * m_H / full);
		INT_UIF_Draw.Place(Thumb, m_X, m_Y + (m_H - size) * m_Pos / rest, 4, size);
	}
}

// Однострочное поле. Пока в нём курсор, персонаж не ходит (VB_TabletMenu.SetTyping).
// Страница узнаёт об изменениях через OnInput(поле) и об Enter через OnSubmit(поле).
class VBT_Input extends ScriptedWidgetEventHandler
{
	EditBoxWidget Edit;
	protected TextWidget m_Hint;
	protected ref INT_UIF_Surface m_Bg;
	protected Widget m_Clip;
	protected int m_Max;
	protected string m_Last;
	protected float m_W;
	protected float m_H;
	protected float m_Size;
	protected bool m_Focused;

	void VBT_Input(Widget p, float x, float y, float w, float h, string placeholder, int maxLen, string value = "", float size = 15)
	{
		m_Max = maxLen;
		m_W = w - 28;
		m_H = h;
		m_Size = size;
		m_Bg = INT_UIF_Surface.Box(p, x, y, w, h, 8, VB_C_PANEL2, VB_C_LINE2);
		m_Hint = INT_UIF_Draw.Text(p, x + 14, y, m_W, h, placeholder, size, VB_C_DIM2);
		// Поле в обрезающей рамке: EditBoxWidget сам строку не прокручивает, длинный текст сдвигается в Fit.
		m_Clip = INT_UIF_Draw.Clip(p, x + 14, y, m_W, h);
		Edit = INT_UIF_Draw.Edit(m_Clip, 0, 0, m_W, h, size);
		Edit.SetHandler(this);
		Edit.SetText(value);
		if (placeholder.Length() > 1 && placeholder.Get(0) == "#")
			Edit.SetName("VBT_In_" + placeholder.Substring(1, placeholder.Length() - 1));
		m_Last = value;
		m_Hint.Show(value == "");
#ifdef DIAG_DEVELOPER
		VB_TabletMenu.DiagWatch(this);
#endif
	}

	// Пока в поле курсор и текст длиннее поля, поле шире рамки и прижато к её правому краю:
	// виден конец строки, где печатают. Без курсора видно начало.
	protected void Fit()
	{
		if (!Edit || !m_Clip)
			return;
		float need = 0;
		if (m_Focused)
			need = VBT.TextW(m_Clip, Edit.GetText(), m_Size, false) + 16;
		if (need > m_W)
			INT_UIF_Draw.Place(Edit, m_W - need, (m_H - m_Size) * 0.5, need, m_Size);
		else
			INT_UIF_Draw.Place(Edit, 0, (m_H - m_Size) * 0.5, m_W, m_Size);
	}

#ifdef DIAG_DEVELOPER
	// Стенд пишет текст в поле без события: в диагностике изменения ловятся опросом.
	void DiagPoll()
	{
		if (Edit && Edit.GetText() != m_Last)
			OnChange(Edit, 0, 0, false);
	}
#endif

	string Get()
	{
		string s = Edit.GetText();
		s.TrimInPlace();
		return s;
	}

	void Set(string value)
	{
		Edit.SetText(value);
		m_Last = value;
		m_Hint.Show(value == "");
		Fit();
	}

	void Focus()
	{
		SetFocus(Edit);
	}

	void SetBorder(int color)
	{
		m_Bg.SetBorder(color);
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		string s = Edit.GetText();
		if (m_Max > 0 && s.LengthUtf8() > m_Max)
		{
			s = s.SubstringUtf8(0, m_Max);
			Edit.SetText(s);
		}
		m_Hint.Show(s == "");
		Fit();
		VB_TabletMenu menu = VBK.Menu();
		if (s != m_Last)
		{
			m_Last = s;
			if (menu)
				menu.InputChanged(this);
		}
		if (finished && menu)
			menu.InputSubmitted(this);
		return false;
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		m_Focused = true;
		Fit();
		m_Bg.SetBorder(VB_C_ACCENT);
		VB_TabletMenu menu = VBK.Menu();
		if (menu)
			menu.SetTyping(true);
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		m_Focused = false;
		Fit();
		m_Bg.SetBorder(VB_C_LINE2);
		VB_TabletMenu menu = VBK.Menu();
		if (menu)
			menu.SetTyping(false);
		return false;
	}
}

// Многострочное поле: заявка, новость. Лимит проверяет страница и сервер.
class VBT_Area extends ScriptedWidgetEventHandler
{
	MultilineEditBoxWidget Edit;
	protected TextWidget m_Hint;
	protected ref INT_UIF_Surface m_Bg;
	protected string m_Last;

	void VBT_Area(Widget p, float x, float y, float w, float h, string placeholder, string value = "")
	{
		m_Bg = INT_UIF_Surface.Box(p, x, y, w, h, 8, VB_C_PANEL2, VB_C_LINE2);
		m_Hint = INT_UIF_Draw.Text(p, x + 14, y + 10, w - 28, 22, placeholder, 15, VB_C_DIM2);
		Edit = MultilineEditBoxWidget.Cast(INT_UIF_Widgets.CreatePath(VB_TABLET_LAYOUT_AREA, p));
		INT_UIF_Draw.Place(Edit, x + 14, y + 10, w - 28, h - 20);
		// Размер текста в макете задан в экранных пикселях: на 2K и 4K он вышел бы мельче остального.
		Edit.SetTextExactSize(INT_UIF_Scale.TextPx(15));
		Edit.SetHandler(this);
		Edit.SetText(value);
		m_Last = value;
		m_Hint.Show(value == "");
		// Многострочное поле не шлёт OnChange на каждый знак: подсказка и счётчик страницы
		// обновляются опросом из VB_TabletMenu.Update.
		VB_TabletMenu.WatchArea(this);
	}

	string Get()
	{
		string s;
		Edit.GetText(s);
		s.TrimInPlace();
		return s;
	}

	void Poll()
	{
		if (!Edit)
			return;
		string s;
		Edit.GetText(s);
		if (s == m_Last)
			return;
		m_Last = s;
		m_Hint.Show(s == "");
		VB_TabletMenu menu = VBK.Menu();
		if (menu)
			menu.AreaChanged(this);
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		Poll();
		return false;
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		m_Bg.SetBorder(VB_C_ACCENT);
		VB_TabletMenu menu = VBK.Menu();
		if (menu)
			menu.SetTyping(true);
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		m_Bg.SetBorder(VB_C_LINE2);
		VB_TabletMenu menu = VBK.Menu();
		if (menu)
			menu.SetTyping(false);
		return false;
	}
}

// Переключатель 44x24. Страница ловит клик через Hit в OnHit и зовёт Toggle().
class VBT_Switch extends Managed
{
	Widget Hit;
	bool On;
	protected Widget m_Root;
	protected Widget m_Paint;

	void VBT_Switch(Widget p, float x, float y, bool on, ScriptedWidgetEventHandler handler)
	{
		On = on;
		m_Root = INT_UIF_Draw.Frame(p, x, y, 44, 24);
		Paint();
		Hit = INT_UIF_Draw.Hit(p, x, y, 44, 24, handler);
	}

	protected void Paint()
	{
		INT_UIF_Widgets.ClearChildren(m_Root);
		int fill = VB_C_PANEL3;
		int knob = VB_C_DIM;
		float kx = 3;
		if (On)
		{
			fill = VB_C_ACCENT;
			knob = VB_C_ON;
			kx = 23;
		}
		INT_UIF_Surface.Pill(m_Root, 0, 0, 44, 24, fill);
		INT_UIF_Surface.Pill(m_Root, kx, 3, 18, 18, knob);
	}

	void Toggle()
	{
		On = !On;
		Paint();
	}
}

// Выпадающий список. Сам открывается и закрывается, выбор отдаёт странице через OnPick(список, индекс).
class VBT_Dropdown extends ScriptedWidgetEventHandler
{
	int Selected;
	protected ref array<string> m_Items;
	protected TextWidget m_Text;
	protected Widget m_Hit;
	protected Widget m_Pop;
	protected Widget m_P;
	protected float m_X;
	protected float m_Y;
	protected float m_W;
	protected float m_H;
	protected bool m_Up;
	protected ref array<Widget> m_Rows;
	protected ref array<ref INT_UIF_Surface> m_RowBg;

	void VBT_Dropdown(Widget p, float x, float y, float w, float h, array<string> items, int selected, bool up = false)
	{
		m_P = p;
		m_X = x;
		m_Y = y;
		m_W = w;
		m_H = h;
		m_Up = up;
		m_Items = items;
		Selected = selected;
		m_Rows = new array<Widget>();
		m_RowBg = new array<ref INT_UIF_Surface>();
		INT_UIF_Surface.Box(p, x, y, w, h, 8, VB_C_PANEL2, VB_C_LINE2);
		m_Text = INT_UIF_Draw.Text(p, x + 14, y, w - 50, h, "", 14, VB_C_TEXT);
		VBT.Ico(p, x + w - 28, y + (h - 14) * 0.5, 14, "chevron_down", VB_C_DIM);
		ShowValue();
		m_Hit = INT_UIF_Draw.Hit(p, x, y, w, h, this);
	}

	protected void ShowValue()
	{
		if (Selected >= 0 && Selected < m_Items.Count())
			m_Text.SetText(VBT.Fit(VBT.T(m_Items.Get(Selected)), m_W - 50, 14));
	}

	void SetSelected(int i)
	{
		Selected = i;
		ShowValue();
	}

	protected void Open()
	{
		int n = m_Items.Count();
		float h = n * 36 + 12;
		float y = m_Y + m_H + 4;
		if (m_Up)
			y = m_Y - h - 4;
		m_Pop = INT_UIF_Draw.Frame(m_P, m_X, y, m_W, h);
		m_Pop.SetSort(300);
		m_RowBg.Clear();
		m_Rows.Clear();
		INT_UIF_Surface.Box(m_Pop, 0, 0, m_W, h, 8, VB_C_PANEL, VB_C_LINE2);
		for (int i = 0; i < n; i++)
		{
			int fill = 0;
			int ink = VB_C_DIM;
			if (i == Selected)
			{
				fill = VB_C_PANEL3;
				ink = VB_C_TEXT;
			}
			m_RowBg.Insert(INT_UIF_Surface.Box(m_Pop, 6, 6 + i * 36, m_W - 12, 36, 6, fill));
			INT_UIF_Draw.Text(m_Pop, 18, 6 + i * 36, m_W - 36, 36, m_Items.Get(i), 14, ink);
			m_Rows.Insert(INT_UIF_Draw.Hit(m_Pop, 6, 6 + i * 36, m_W - 12, 36, this));
		}
	}

	void Close()
	{
		VB_TabletMenu menu = VBK.Menu();
		if (m_Pop)
		{
			m_Pop.Show(false);
			if (menu)
				menu.Trash(m_Pop);
		}
		m_Pop = null;
		m_Rows.Clear();
		m_RowBg.Clear();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_Hit)
		{
			if (m_Pop)
				Close();
			else
				Open();
			return true;
		}
		int i = m_Rows.Find(w);
		if (i < 0)
			return false;
		Selected = i;
		ShowValue();
		Close();
		VB_TabletMenu menu = VBK.Menu();
		if (menu)
			menu.DropdownPicked(this, i);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int i = m_Rows.Find(w);
		if (i >= 0 && i != Selected)
			m_RowBg.Get(i).SetFill(VB_C_PANEL2);
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int i = m_Rows.Find(w);
		if (i >= 0 && i != Selected)
			m_RowBg.Get(i).SetFill(0);
		return true;
	}
}

// Окно поверх всего экрана планшета. Содержимое рисует страница в Body, клики идут ей в OnHit.
class VBT_Modal extends Managed
{
	Widget Root;
	Widget Body;
	Widget CloseHit;	// null, если окно закрывается только своими кнопками
	Widget Shade;
	protected VB_TabletMenu m_Menu;

	void VBT_Modal(VB_TabletMenu menu, float w, float h, bool closable = true, int border = VB_C_LINE2)
	{
		m_Menu = menu;
		Root = INT_UIF_Draw.Frame(menu.GetScreen(), 0, 0, 1468, 848);
		Root.SetSort(200);
		INT_UIF_Draw.Line(Root, 0, 0, 1468, 848, 0x99000000);
		Shade = INT_UIF_Draw.Hit(Root, 0, 0, 1468, 848, menu.GetHitHandler());
		Body = INT_UIF_Draw.Frame(Root, (1468 - w) * 0.5, (848 - h) * 0.5, w, h);
		INT_UIF_Surface.Box(Body, 0, 0, w, h, 12, VB_C_PANEL, border);
		if (closable)
		{
			VBT.Ico(Body, w - 46, 26, 20, "close", VB_C_DIM);
			CloseHit = INT_UIF_Draw.Hit(Body, w - 54, 18, 36, 36, menu.GetHitHandler());
		}
	}

	// Закрывается в следующем кадре: кликнутая кнопка ещё жива.
	void Close()
	{
		if (!Root)
			return;
		Root.Show(false);
		m_Menu.Trash(Root);
		Root = null;
		Body = null;
		CloseHit = null;
		Shade = null;
	}

	bool IsOpen()
	{
		return Root != null;
	}

	// Клик по затемнению или крестику: страница закрывает окно.
	bool IsDismiss(Widget w)
	{
		if (!Root || !w || !CloseHit)
			return false;
		return w == CloseHit || w == Shade;
	}
}

// Диалоги: вопрос, ввод строки, подтверждение точным словом. Ответ приходит в VB_TabletPage.OnAnswer.
class VBT_Dialog extends Managed
{
	int Tag;
	ref VBT_Modal Modal;
	ref VBT_Input Field;
	ref VBT_Button Yes;
	ref VBT_Button No;
	string Word;
	protected VB_TabletMenu m_Menu;

	void VBT_Dialog(VB_TabletMenu menu, int tag)
	{
		m_Menu = menu;
		Tag = tag;
	}

	void BuildAsk(string title, string text, string yes, bool danger, float h)
	{
		float w = 520;
		Modal = new VBT_Modal(m_Menu, w, h, false);
		Widget b = Modal.Body;
		INT_UIF_Draw.Text(b, 28, 24, w - 56, 28, title, 22, VB_C_TEXT);
		INT_UIF_Draw.MText(b, 28, 66, w - 56, h - 140, text, 15, VB_C_DIM);
		Buttons(b, w, h, yes, danger);
	}

	void BuildPrompt(string title, string hint, string value, int maxLen)
	{
		float w = 520;
		float h = 214;
		Modal = new VBT_Modal(m_Menu, w, h, false);
		Widget b = Modal.Body;
		INT_UIF_Draw.Text(b, 28, 24, w - 56, 28, title, 22, VB_C_TEXT);
		Field = new VBT_Input(b, 28, 70, w - 56, 48, hint, maxLen, value);
		Buttons(b, w, h, "#STR_VB_TAB_DONE", false);
		Field.Focus();
	}

	void BuildConfirm(string title, string text, string word, string yes)
	{
		float w = 560;
		float h = 316;
		Word = word;
		Modal = new VBT_Modal(m_Menu, w, h, false, VB_C_BAD);
		Widget b = Modal.Body;
		INT_UIF_Draw.Text(b, 28, 22, w - 56, 32, title, 22, VB_C_TEXT);
		INT_UIF_Draw.MText(b, 28, 62, w - 56, 70, text, 15, VB_C_DIM);
		string line = VBT.T("#STR_VB_TAB_CONFIRM_WORD") + " " + word;
		INT_UIF_Draw.Text(b, 28, 140, w - 56, 22, VBT.Fit(line, w - 56, 14), 14, VB_C_DIM2);
		Field = new VBT_Input(b, 28, 166, w - 56, 44, word, 40);
		Buttons(b, w, h, yes, true);
		Yes.Hit.Show(false);
		Yes.Bg.SetFill(VB_C_PANEL3);
		Field.Focus();
	}

	protected void Buttons(Widget b, float w, float h, string yes, bool danger)
	{
		No = new VBT_Button(b, w - 328, h - 68, 140, 44, "#STR_VB_TAB_CANCEL", false, "", m_Menu.GetHitHandler());
		Yes = new VBT_Button(b, w - 178, h - 68, 150, 44, yes, !danger, "", m_Menu.GetHitHandler());
		if (danger)
		{
			Yes.Danger = true;
			Yes.Bg.SetFill(VB_C_BAD);
		}
	}

	// true: клик наш. Ответ отдаёт меню, чтобы окно закрылось до него.
	bool OnHit(Widget w, out bool answered, out bool result)
	{
		if (w == Yes.Hit)
		{
			answered = true;
			result = true;
			return true;
		}
		if (w == No.Hit)
		{
			answered = true;
			result = false;
			return true;
		}
		return w == Modal.Shade;
	}

	void OnField()
	{
		if (Word == "")
			return;
		bool ok = Field.Get() == Word;
		Yes.Hit.Show(ok);
		int fill = VB_C_PANEL3;
		if (ok)
			fill = VB_C_BAD;
		Yes.Bg.SetFill(fill);
	}
}
