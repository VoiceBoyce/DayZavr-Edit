// ============================================================================
// DRL_CraftMenu - «Чертёж»: центр окна крафта (макет «макет верстака DRL»).
//
// Изделие в круге (3D, тянуть - вращать, колесо - масштаб), выноски
// ингредиентов и инструментов слева и справа с пунктиром к изделию, обвесы
// станции (AttachmentsEx = 1) строкой сверху, под изделием варианты
// результата, навесное (слоты результата и доп. рецепты-навесное), внизу
// характеристики. Пунктир рисует CanvasWidget из UI/layout/DRL_Canvas.layout
// (в скрипте нет TypeID холста, создать его можно только из layout).
// Все размеры - дизайн-пиксели холста 1920x1080.
// ============================================================================

// Палитра окна: акцент и цвет листа зависят от станции, остальное общее.
// Окно задаёт станцию до построения (SetStation).
class DRL_UIC
{
	static int s_AR = 84;
	static int s_AG = 222;
	static int s_AB = 67;
	static int s_BR = 28;
	static int s_BG = 211;
	static int s_BB = 218;

	// 0 - верстак и меню F9, 1 - швейная машинка, 2 - подъёмник.
	static void SetStation(int kind)
	{
		if (kind == 1)
		{
			s_AR = 245;
			s_AG = 165;
			s_AB = 36;
			s_BR = 226;
			s_BG = 190;
			s_BB = 128;
			return;
		}
		if (kind == 2)
		{
			s_AR = 255;
			s_AG = 201;
			s_AB = 60;
			s_BR = 124;
			s_BG = 182;
			s_BB = 255;
			return;
		}
		s_AR = 84;
		s_AG = 222;
		s_AB = 67;
		s_BR = 28;
		s_BG = 211;
		s_BB = 218;
	}

	static int A(int a = 255)
	{
		return ARGB(a, s_AR, s_AG, s_AB);
	}

	static int BP(int a = 255)
	{
		return ARGB(a, s_BR, s_BG, s_BB);
	}

	static int Ok(int a = 255)
	{
		return ARGB(a, 84, 222, 67);
	}

	static int No(int a = 255)
	{
		return ARGB(a, 255, 77, 77);
	}

	static int NoText()
	{
		return ARGB(255, 255, 107, 107);
	}

	static int Lk(int a = 255)
	{
		return ARGB(a, 255, 201, 60);
	}

	static int Tl(int a = 255)
	{
		return ARGB(a, 28, 211, 218);
	}

	static int Blue(int a = 255)
	{
		return ARGB(a, 143, 183, 255);
	}

	static int White(int a)
	{
		return ARGB(a, 255, 255, 255);
	}

	static int Txt()
	{
		return ARGB(255, 238, 241, 243);
	}

	static int Muted()
	{
		return ARGB(163, 255, 255, 255);
	}

	static int Dim()
	{
		return ARGB(107, 255, 255, 255);
	}

	// Тёмный текст на заливке акцентом.
	static int OnAccent()
	{
		return ARGB(255, 13, 31, 10);
	}

	// Состояние предмета по уровню здоровья (0 нетронутое ... 4 испорчено): процент для полоски.
	static int HealthPct(int level)
	{
		if (level <= 0)
			return 100;
		if (level == 1)
			return 70;
		if (level == 2)
			return 50;
		if (level == 3)
			return 25;
		return 0;
	}

	static int HealthColor(int pct)
	{
		if (pct >= 60)
			return Ok();
		if (pct >= 30)
			return Lk();
		return No();
	}

	static string HealthName(int level)
	{
		if (level <= 0)
			return DRL_UIText.Tr("#STR_DRL_UNTOUCHED");
		if (level == 1)
			return DRL_UIText.Tr("#STR_DRL_WORN");
		if (level == 2)
			return DRL_UIText.Tr("#STR_DRL_DAMAGED");
		if (level == 3)
			return DRL_UIText.Tr("#STR_DRL_BADLY_DAMAGED");
		return DRL_UIText.Tr("#STR_DRL_BROKEN");
	}
}

// Ширина текста в дизайн-пикселях: пробный TextWidget и GetTextSize.
// Оценка INT_UIF_Draw.TextWidth занижает (особенно цифры и жирный шрифт).
class DRL_UIMeasure
{
	protected static ref map<string, float> s_Cache;

	static float TextW(Widget parent, string text, float size, bool bold)
	{
		float f = INT_UIF_Scale.Factor();
		string key = size.ToString() + "|" + f.ToString() + "|" + bold.ToString() + "|" + text;
		float w;
		if (s_Cache && s_Cache.Find(key, w))
			return w;
		w = INT_UIF_Draw.TextWidth(text, size) * 1.2;
		if (!parent || text == "")
			return w;
		TextWidget probe = INT_UIF_Draw.Text(parent, 0, 0, 3000, size * 2, text, size, 0);
		if (!probe)
			return w;
		probe.SetBold(bold);
		probe.Update();
		int mw;
		int mh;
		probe.GetTextSize(mw, mh);
		probe.Unlink();
		if (mw <= 0 || f <= 0)
			return w;
		w = mw / f;
		// Пробный виджет меряет жирный текст чуть уже, чем он рисуется: запас.
		if (bold)
			w = w * 1.06 + 2;
		if (!s_Cache || s_Cache.Count() > 3000)
			s_Cache = new map<string, float>();
		s_Cache.Insert(key, w);
		return w;
	}
}

// Холст для линий. Координаты и толщина - в дизайн-пикселях.
class DRL_UICanvas extends Managed
{
	static const string LAYOUT = "DRL_CraftMenu/UI/layout/DRL_Canvas.layout";

	protected CanvasWidget m_Canvas;

	bool Build(Widget parent, float x, float y, float w, float h)
	{
		Widget root = INT_UIF_Widgets.CreatePath(LAYOUT, parent);
		m_Canvas = CanvasWidget.Cast(root);
		if (!m_Canvas)
			return false;
		INT_UIF_Draw.Place(m_Canvas, x, y, w, h);
		return true;
	}

	Widget GetRoot()
	{
		return m_Canvas;
	}

	void Clear()
	{
		if (m_Canvas)
			m_Canvas.Clear();
	}

	void Show(bool show)
	{
		if (m_Canvas)
			m_Canvas.Show(show);
	}

	void Line(float x1, float y1, float x2, float y2, float width, int color)
	{
		if (!m_Canvas)
			return;
		float f = INT_UIF_Scale.Factor();
		float lw = width * f;
		if (lw < 1)
			lw = 1;
		m_Canvas.DrawLine(x1 * f, y1 * f, x2 * f, y2 * f, lw, color);
	}

	// Пунктир по ломаной xs/ys: dash - штрих, gap - промежуток.
	void Dashed(array<float> xs, array<float> ys, float dash, float gap, float width, int color)
	{
		if (!xs || !ys || xs.Count() < 2)
			return;
		float pattern = dash + gap;
		float phase = 0;
		for (int i = 1; i < xs.Count(); i++)
		{
			float ax = xs[i - 1];
			float ay = ys[i - 1];
			float dx = xs[i] - ax;
			float dy = ys[i] - ay;
			float len = Math.Sqrt(dx * dx + dy * dy);
			if (len < 0.001)
				continue;
			float done = 0;
			while (done < len)
			{
				bool drawing = phase < dash;
				float left = pattern - phase;
				if (drawing)
					left = dash - phase;
				float step = len - done;
				if (left < step)
					step = left;
				if (drawing)
				{
					float t0 = done / len;
					float t1 = (done + step) / len;
					Line(ax + dx * t0, ay + dy * t0, ax + dx * t1, ay + dy * t1, width, color);
				}
				done = done + step;
				phase = phase + step;
				if (phase >= pattern - 0.001)
					phase = 0;
			}
		}
	}

	// Кубическая кривая Безье; dashed = false - сплошная.
	void Bezier(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, bool dashed, float width, int color)
	{
		array<float> xs = new array<float>();
		array<float> ys = new array<float>();
		int steps = 28;
		for (int i = 0; i <= steps; i++)
		{
			float t = i;
			t = t / steps;
			float u = 1 - t;
			float b0 = u * u * u;
			float b1 = 3 * u * u * t;
			float b2 = 3 * u * t * t;
			float b3 = t * t * t;
			xs.Insert(b0 * x0 + b1 * x1 + b2 * x2 + b3 * x3);
			ys.Insert(b0 * y0 + b1 * y1 + b2 * y2 + b3 * y3);
		}
		if (dashed)
		{
			Dashed(xs, ys, 4, 4, width, color);
			return;
		}
		for (int k = 1; k < xs.Count(); k++)
		{
			Line(xs[k - 1], ys[k - 1], xs[k], ys[k], width, color);
		}
	}

	void Circle(float cx, float cy, float r, bool dashed, float width, int color)
	{
		array<float> xs = new array<float>();
		array<float> ys = new array<float>();
		int segs = 96;
		for (int i = 0; i <= segs; i++)
		{
			float a = i;
			a = Math.PI2 * a / segs;
			xs.Insert(cx + Math.Cos(a) * r);
			ys.Insert(cy + Math.Sin(a) * r);
		}
		if (dashed)
		{
			Dashed(xs, ys, 5, 5, width, color);
			return;
		}
		for (int k = 1; k < xs.Count(); k++)
		{
			Line(xs[k - 1], ys[k - 1], xs[k], ys[k], width, color);
		}
	}

	// Сетка листа: мелкие клетки и крупные через каждые четыре.
	void Grid(float w, float h, float cell, int minor, int major)
	{
		int n = 0;
		float x = 0;
		while (x <= w)
		{
			int cx = minor;
			if (n == 0 || n == 4 * (n / 4))
				cx = major;
			Line(x, 0, x, h, 1, cx);
			x = x + cell;
			n++;
		}
		int m = 0;
		float y = 0;
		while (y <= h)
		{
			int cy = minor;
			if (m == 0 || m == 4 * (m / 4))
				cy = major;
			Line(0, y, w, y, 1, cy);
			y = y + cell;
			m++;
		}
	}
}

// --- данные выноски ---------------------------------------------------------------------
const int DRL_BP_INGR = 0;   // расходуемый ингредиент
const int DRL_BP_TOOL = 1;   // инструмент (AttachmentsEx 2): не тратится, изнашивается
const int DRL_BP_ATT = 2;    // обвес станции (AttachmentsEx 1)
const int DRL_BP_MORE = 3;   // «ещё N»: не поместились, полный список - в «Требованиях»

class DRL_UIBpItem extends Managed
{
	int m_Kind;
	int m_Tag = -1;
	string m_Name;
	string m_Preview;      // класс 3D-превью
	string m_Glyph = "gear";
	string m_Have;         // «24»
	string m_Need;         // «/ 8»
	string m_Sub;          // строка инструмента и обвеса
	bool m_Ok;
	bool m_Variants;       // у ингредиента есть замены: значок, клик - панель вариантов
	bool m_Selected;       // панель вариантов открыта для этого ингредиента
	int m_Health = -1;     // 0..100 - полоска состояния обвеса
	int m_GoRecipe = -1;   // RecipeId: недостающее делается здесь же - кнопка «Сделать»
	string m_Tip;
}

// Доп. рецепт-навесное (AttachedRecipeIds) под изделием.
class DRL_UIBpDopData extends Managed
{
	string m_Name;
	bool m_Checked;
	bool m_Locked;
	bool m_Dim;            // нет слота под это навесное
	int m_Qty = 1;
	int m_VarCount;
	string m_Tip;
}

// --- выноска ---------------------------------------------------------------------------
class DRL_UIBpCard extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ref INT_UIF_Surface m_Pic;
	Widget m_Clip;
	ref DRL_UIItemView m_View;
	ImageWidget m_Glyph;
	Widget m_Swap;
	TextWidget m_Name;
	TextWidget m_Value;
	TextWidget m_Need;
	TextWidget m_Sub;
	ImageWidget m_DurTrack;
	ImageWidget m_DurFill;
	Widget m_Go;
	ref INT_UIF_Surface m_GoBg;
	TextWidget m_GoText;
	ImageWidget m_GoIcon;
	ButtonWidget m_Hit;
	ButtonWidget m_GoHit;
	ref DRL_UIBpItem m_Data;
	float m_W;
	float m_H;
	bool m_Hover;
	bool m_HasModel;
	bool m_ModelsHidden;

	void Build(Widget parent, int slot, ScriptedWidgetEventHandler handler)
	{
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, 214, 50, "DRL_BpCard");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, 214, 50, 10, 0, 0);
		m_Pic = INT_UIF_Surface.Box(m_Root, 6, 6, 38, 38, 8, 0, 0);
		m_Clip = INT_UIF_Draw.Clip(m_Root, 8, 8, 34, 34, "DRL_BpClip");
		m_View = new DRL_UIItemView();
		m_View.Create(m_Clip, true);
		m_View.SetOrientation(Vector(0, 0, 0));
		int txt = DRL_UIC.Txt();
		int dim = DRL_UIC.Dim();
		m_Glyph = INT_UIF_Draw.Icon(m_Root, 16, 16, 18, "gear", dim);
		m_Glyph.Show(false);

		m_Swap = INT_UIF_Draw.Frame(m_Root, 34, 2, 16, 16, "DRL_BpSwap");
		int lk = DRL_UIC.Lk();
		INT_UIF_Surface.Box(m_Swap, 0, 0, 16, 16, 4, lk, 0);
		int swapIcon = ARGB(255, 43, 26, 0);
		ImageWidget swapGlyph = INT_UIF_Draw.Icon(m_Swap, 3, 3, 10, "swap", swapIcon);
		DRL_UIKitUtil.SetSortDeep(m_Swap, DRL_UI_SORT_OVER_MODEL);
		swapGlyph.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
		m_Swap.Show(false);

		m_Name = INT_UIF_Draw.Text(m_Root, 52, 6, 150, 18, "", 12, txt);
		m_Name.SetBold(true);
		m_Value = INT_UIF_Draw.Text(m_Root, 52, 24, 80, 20, "", 14, txt);
		m_Value.SetBold(true);
		m_Need = INT_UIF_Draw.Text(m_Root, 80, 25, 90, 18, "", 11, dim);
		m_Need.SetBold(true);
		m_Sub = INT_UIF_Draw.Text(m_Root, 52, 24, 150, 18, "", 11, dim);
		m_Sub.SetBold(true);
		int track = DRL_UIC.White(26);
		m_DurTrack = INT_UIF_Draw.Line(m_Root, 52, 40, 150, 3, track);
		m_DurFill = INT_UIF_Draw.Line(m_Root, 52, 40, 50, 3, txt);

		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, 214, 50, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, "DRL_Bp_" + slot.ToString(), handler);

		m_Go = INT_UIF_Draw.Frame(m_Root, 150, 13, 60, 24, "DRL_BpGo");
		int goFill = DRL_UIC.BP(40);
		m_GoBg = INT_UIF_Surface.Box(m_Go, 0, 0, 60, 24, 6, goFill, 0);
		int bp = DRL_UIC.BP();
		m_GoText = INT_UIF_Draw.Text(m_Go, 7, 0, 50, 24, "#STR_DRL_UI_BP_MAKE", 11, bp);
		m_GoText.SetBold(true);
		m_GoIcon = INT_UIF_Draw.Icon(m_Go, 48, 7, 10, "chevron_right", bp);
		m_GoHit = INT_UIF_Draw.Hit(m_Go, 0, 0, 60, 24, handler);
		DRL_UIKitUtil.ExposeHit(m_GoHit, "DRL_BpGo_" + slot.ToString(), handler);
		m_Go.Show(false);
		m_Root.Show(false);
	}

	void Place(DRL_UIBpItem d, float x, float y, float w, float h)
	{
		m_Data = d;
		m_W = w;
		m_H = h;
		m_Root.Show(true);
		INT_UIF_Draw.Place(m_Root, x, y, w, h);
		INT_UIF_Draw.Resize(m_Bg.GetRoot(), w, h);
		INT_UIF_Draw.Resize(m_Hit, w, h);

		float pic = h - 12;
		if (pic > 38)
			pic = 38;
		float py = (h - pic) * 0.5;
		INT_UIF_Draw.Place(m_Pic.GetRoot(), 6, py, pic, pic);
		INT_UIF_Draw.Place(m_Clip, 8, py + 2, pic - 4, pic - 4);
		float gsz = pic * 0.5;
		INT_UIF_Draw.Place(m_Glyph, 6 + (pic - gsz) * 0.5, py + (pic - gsz) * 0.5, gsz, gsz);
		INT_UIF_Draw.Move(m_Swap, 6 + pic - 11, py - 5);

		// 3D-превью или значок.
		m_HasModel = false;
		if (d.m_Preview != "")
			m_HasModel = m_View.SetClass(d.m_Preview);
		else
			m_View.Clear();
		m_View.Show(m_HasModel && !m_ModelsHidden);
		m_Glyph.Show(!m_HasModel);
		INT_UIF_Widgets.SetIcon(m_Glyph, d.m_Glyph);
		m_Swap.Show(d.m_Variants && !m_ModelsHidden);

		// «Сделать» справа.
		float goW = 0;
		bool go = d.m_GoRecipe >= 0;
		m_Go.Show(go);
		if (go)
		{
			string goText = DRL_UIText.Tr("#STR_DRL_UI_BP_MAKE");
			float gt = DRL_UIMeasure.TextW(m_Root, goText, 11, true);
			float gw = gt + 26;
			INT_UIF_Draw.Place(m_Go, w - 8 - gw, (h - 24) * 0.5, gw, 24);
			INT_UIF_Draw.Resize(m_GoBg.GetRoot(), gw, 24);
			INT_UIF_Draw.Resize(m_GoHit, gw, 24);
			INT_UIF_Draw.Place(m_GoText, 7, 0, gt + 4, 24);
			INT_UIF_Draw.Move(m_GoIcon, 10 + gt, 7);
			goW = gw + 6;
		}

		float tx = 6 + pic + 8;
		float tw = w - tx - 8 - goW;
		bool compact = h < 44;
		float nameY = 6;
		float lineY = 24;
		if (compact)
		{
			nameY = 2;
			lineY = 19;
		}
		bool withBar = d.m_Health >= 0;
		if (withBar && !compact)
		{
			nameY = 4;
			lineY = 19;
		}
		INT_UIF_Draw.Place(m_Name, tx, nameY, tw, 18);
		m_Name.SetText(d.m_Name);

		bool counts = d.m_Kind == DRL_BP_INGR;
		m_Value.Show(counts);
		m_Need.Show(counts);
		m_Sub.Show(!counts);
		if (counts)
		{
			m_Value.SetText(d.m_Have);
			float vw = DRL_UIMeasure.TextW(m_Root, d.m_Have, 14, true);
			INT_UIF_Draw.Place(m_Value, tx, lineY, vw + 4, 20);
			INT_UIF_Draw.Place(m_Need, tx + vw + 4, lineY + 1, tw - vw - 4, 18);
			m_Need.SetText(d.m_Need);
		}
		else
		{
			INT_UIF_Draw.Place(m_Sub, tx, lineY, tw, 18);
			m_Sub.SetText(d.m_Sub);
		}

		m_DurTrack.Show(withBar);
		m_DurFill.Show(withBar);
		if (withBar)
		{
			float by = h - 9;
			INT_UIF_Draw.Place(m_DurTrack, tx, by, tw, 3);
			float fw = tw * d.m_Health / 100.0;
			if (fw < 2)
				fw = 2;
			INT_UIF_Draw.Place(m_DurFill, tx, by, fw, 3);
			m_DurFill.SetColor(DRL_UIC.HealthColor(d.m_Health));
		}
		Paint();
	}

	void Hide()
	{
		m_Data = null;
		m_Root.Show(false);
		m_View.Clear();
		m_HasModel = false;
	}

	void SetHover(bool hover)
	{
		if (m_Hover == hover)
			return;
		m_Hover = hover;
		Paint();
	}

	void ShowModel(bool show)
	{
		m_ModelsHidden = !show;
		m_View.Show(show && m_HasModel);
		if (m_Data)
			m_Swap.Show(show && m_Data.m_Variants);
	}

	protected void Paint()
	{
		DRL_UIBpItem d = m_Data;
		if (!d)
			return;
		int fill = ARGB(240, 6, 16, 18);
		int border = DRL_UIC.Ok(102);
		int value = DRL_UIC.Ok();
		int sub = DRL_UIC.Muted();
		int picFill = DRL_UIC.White(13);
		int glyph = ARGB(255, 217, 226, 232);
		if (d.m_Kind == DRL_BP_TOOL)
		{
			fill = ARGB(230, 10, 18, 20);
			border = DRL_UIC.White(56);
		}
		else if (d.m_Kind == DRL_BP_ATT)
		{
			fill = ARGB(242, 4, 22, 26);
			border = DRL_UIC.Tl(140);
			sub = DRL_UIC.Tl();
			glyph = DRL_UIC.Tl();
		}
		else if (d.m_Kind == DRL_BP_MORE)
		{
			fill = ARGB(230, 10, 18, 20);
			border = DRL_UIC.BP(90);
			sub = DRL_UIC.BP();
			glyph = DRL_UIC.BP();
		}
		if (!d.m_Ok && d.m_Kind != DRL_BP_MORE)
		{
			fill = ARGB(240, 40, 10, 12);
			border = DRL_UIC.No(178);
			value = DRL_UIC.NoText();
			sub = DRL_UIC.NoText();
			glyph = ARGB(255, 255, 138, 138);
		}
		if (d.m_Selected)
			border = DRL_UIC.Lk();
		else if (m_Hover)
			border = INT_UIF_Color.WithAlpha(border, 255);
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
		m_Pic.SetFill(picFill);
		m_Pic.SetBorder(0);
		m_Value.SetColor(value);
		m_Sub.SetColor(sub);
		m_Glyph.SetColor(glyph);
	}

	void Clear()
	{
		if (m_View)
			m_View.Clear();
		m_HasModel = false;
	}
}

// --- чип варианта результата -----------------------------------------------------------
class DRL_UIBpChip extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ImageWidget m_Dot;
	TextWidget m_Text;
	ButtonWidget m_Hit;
	float m_W;

	void Build(Widget parent, int slot, ScriptedWidgetEventHandler handler)
	{
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, 80, 28, "DRL_BpChip");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, 80, 28, 8, 0, 0);
		m_Dot = INT_UIF_Draw.Image(m_Root, 10, 9, 10, 10, "iuf_circle16", 0);
		int txt = DRL_UIC.Txt();
		m_Text = INT_UIF_Draw.Text(m_Root, 26, 0, 50, 28, "", 12, txt);
		m_Text.SetBold(true);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, 80, 28, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, "DRL_BpVar_" + slot.ToString(), handler);
		m_Root.Show(false);
	}

	float Measure(string text)
	{
		return DRL_UIMeasure.TextW(m_Root, text, 12, true) + 38;
	}

	void Place(float x, float y, float w, string text, int dot, bool selected)
	{
		m_W = w;
		m_Root.Show(true);
		INT_UIF_Draw.Place(m_Root, x, y, w, 28);
		INT_UIF_Draw.Resize(m_Bg.GetRoot(), w, 28);
		INT_UIF_Draw.Resize(m_Hit, w, 28);
		INT_UIF_Draw.Place(m_Text, 26, 0, w - 30, 28);
		m_Text.SetText(text);
		m_Dot.SetColor(dot);
		int fill = DRL_UIC.White(13);
		int border = DRL_UIC.White(26);
		int fg = DRL_UIC.Muted();
		if (selected)
		{
			fill = DRL_UIC.A(38);
			border = DRL_UIC.A(150);
			fg = DRL_UIC.A();
		}
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
		m_Text.SetColor(fg);
	}

	void Hide()
	{
		m_Root.Show(false);
	}
}

// --- чип доп. навесного ----------------------------------------------------------------
class DRL_UIBpDop extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ref INT_UIF_Surface m_Box;
	ImageWidget m_Check;
	ImageWidget m_Lock;
	TextWidget m_Text;
	TextWidget m_Qty;
	Widget m_Minus;
	Widget m_Plus;
	Widget m_Var;
	ButtonWidget m_Hit;
	ButtonWidget m_MinusHit;
	ButtonWidget m_PlusHit;
	ButtonWidget m_VarHit;
	float m_W;

	void Build(Widget parent, int slot, ScriptedWidgetEventHandler handler)
	{
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, 120, 30, "DRL_BpDop");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, 120, 30, 8, 0, 0);
		m_Box = INT_UIF_Surface.Box(m_Root, 8, 8, 14, 14, 4, 0, 0);
		int txt = DRL_UIC.Txt();
		m_Check = INT_UIF_Draw.Icon(m_Root, 10, 10, 10, "check", txt);
		m_Lock = INT_UIF_Draw.Icon(m_Root, 9, 9, 12, "lock", txt);
		m_Text = INT_UIF_Draw.Text(m_Root, 30, 0, 80, 30, "", 12, txt);
		m_Text.SetBold(true);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, 120, 30, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, "DRL_BpDop_" + slot.ToString(), handler);

		int btnFill = DRL_UIC.White(20);
		m_Minus = INT_UIF_Draw.Frame(m_Root, 0, 5, 20, 20, "DRL_BpDopMinus");
		INT_UIF_Surface.Box(m_Minus, 0, 0, 20, 20, 4, btnFill, 0);
		INT_UIF_Draw.Icon(m_Minus, 5, 5, 10, "minus", txt);
		m_MinusHit = INT_UIF_Draw.Hit(m_Minus, 0, 0, 20, 20, handler);
		DRL_UIKitUtil.ExposeHit(m_MinusHit, "DRL_BpDopM_" + slot.ToString(), handler);
		m_Qty = INT_UIF_Draw.Text(m_Root, 0, 0, 22, 30, "", 12, txt, INT_UIF_Align.CENTER);
		m_Qty.SetBold(true);
		m_Plus = INT_UIF_Draw.Frame(m_Root, 0, 5, 20, 20, "DRL_BpDopPlus");
		INT_UIF_Surface.Box(m_Plus, 0, 0, 20, 20, 4, btnFill, 0);
		INT_UIF_Draw.Icon(m_Plus, 5, 5, 10, "plus", txt);
		m_PlusHit = INT_UIF_Draw.Hit(m_Plus, 0, 0, 20, 20, handler);
		DRL_UIKitUtil.ExposeHit(m_PlusHit, "DRL_BpDopP_" + slot.ToString(), handler);
		m_Var = INT_UIF_Draw.Frame(m_Root, 0, 5, 20, 20, "DRL_BpDopVar");
		int varFill = DRL_UIC.Lk(40);
		INT_UIF_Surface.Box(m_Var, 0, 0, 20, 20, 4, varFill, 0);
		int lk = DRL_UIC.Lk();
		INT_UIF_Draw.Icon(m_Var, 5, 5, 10, "swap", lk);
		m_VarHit = INT_UIF_Draw.Hit(m_Var, 0, 0, 20, 20, handler);
		DRL_UIKitUtil.ExposeHit(m_VarHit, "DRL_BpDopV_" + slot.ToString(), handler);
		m_Root.Show(false);
	}

	// Ширина чипа под данные.
	float Measure(DRL_UIBpDopData d)
	{
		float w = 30 + DRL_UIMeasure.TextW(m_Root, d.m_Name, 12, true) + 10;
		if (d.m_Checked && !d.m_Locked)
			w = w + 70;
		if (d.m_VarCount > 1 && !d.m_Locked)
			w = w + 26;
		return w;
	}

	void Place(float x, float y, float w, DRL_UIBpDopData d)
	{
		m_W = w;
		m_Root.Show(true);
		INT_UIF_Draw.Place(m_Root, x, y, w, 30);
		INT_UIF_Draw.Resize(m_Bg.GetRoot(), w, 30);
		INT_UIF_Draw.Resize(m_Hit, w, 30);
		bool qty = d.m_Checked && !d.m_Locked;
		bool vars = d.m_VarCount > 1 && !d.m_Locked;
		float right = w - 6;
		m_Var.Show(vars);
		if (vars)
		{
			right = right - 20;
			INT_UIF_Draw.Move(m_Var, right, 5);
			right = right - 6;
		}
		m_Minus.Show(qty);
		m_Plus.Show(qty);
		m_Qty.Show(qty);
		if (qty)
		{
			right = right - 20;
			INT_UIF_Draw.Move(m_Plus, right, 5);
			right = right - 24;
			INT_UIF_Draw.Place(m_Qty, right, 0, 24, 30);
			m_Qty.SetText(d.m_Qty.ToString());
			right = right - 20;
			INT_UIF_Draw.Move(m_Minus, right, 5);
			right = right - 6;
		}
		INT_UIF_Draw.Place(m_Text, 30, 0, right - 30, 30);
		m_Text.SetText(d.m_Name);

		int fill = DRL_UIC.White(13);
		int border = DRL_UIC.White(26);
		int fg = DRL_UIC.Muted();
		int boxFill = 0;
		int boxBorder = DRL_UIC.White(90);
		if (d.m_Checked)
		{
			fill = DRL_UIC.Tl(30);
			border = DRL_UIC.Tl(150);
			fg = DRL_UIC.Txt();
			boxFill = DRL_UIC.Tl();
			boxBorder = 0;
		}
		if (d.m_Dim || d.m_Locked)
			fg = DRL_UIC.Dim();
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
		m_Box.SetFill(boxFill);
		m_Box.SetBorder(boxBorder);
		m_Box.Show(!d.m_Locked);
		m_Check.Show(d.m_Checked && !d.m_Locked);
		int checkColor = ARGB(255, 4, 22, 26);
		m_Check.SetColor(checkColor);
		m_Lock.Show(d.m_Locked);
		int lockColor = DRL_UIC.Lk();
		m_Lock.SetColor(lockColor);
		m_Text.SetColor(fg);
	}

	void Hide()
	{
		m_Root.Show(false);
	}
}

// --- сам чертёж ------------------------------------------------------------------------
class DRL_UIBlueprint extends ScriptedWidgetEventHandler
{
	static const float CARD_W = 214;
	static const float CARD_H = 50;
	static const float CARD_H_COMPACT = 40;
	static const float CARD_GAP = 8;
	static const float SIDE = 16;
	static const float CORE_R = 88;
	static const int MAX_SIDE = 24;
	static const int MAX_TOP = 6;
	static const int MAX_VARS = 8;
	static const int MAX_DOP = 8;
	static const int STAT_ROWS = 5;

	protected Widget m_Root;
	protected float m_W;
	protected float m_H;
	protected ref DRL_UICanvas m_Lines;
	protected ImageWidget m_Glow;
	protected ref DRL_UIStage m_Stage;
	protected Widget m_StageHost;
	protected Widget m_Badge;
	protected ref INT_UIF_Surface m_BadgeBg;
	protected TextWidget m_BadgeText;
	protected string m_BadgeValue;
	protected ref array<ref DRL_UIBpCard> m_Cards;
	protected ref array<ref DRL_UIBpItem> m_Side;
	protected ref array<ref DRL_UIBpItem> m_Top;
	protected ref array<ImageWidget> m_Dots;
	protected int m_DotsUsed;

	protected TextWidget m_VarLabel;
	protected ref array<ref DRL_UIBpChip> m_VarChips;
	protected ref array<string> m_VarNames;
	protected ref array<int> m_VarColors;
	protected int m_VarSel;

	protected TextWidget m_SlotsLabel;
	protected ref DRL_UISlotStrip m_Slots;
	protected Widget m_SlotsHost;
	protected int m_SlotCount;
	protected string m_SlotsText;

	protected TextWidget m_DopLabel;
	protected ref array<ref DRL_UIBpDop> m_DopChips;
	protected ref array<ref DRL_UIBpDopData> m_Dop;
	protected string m_DopText;

	protected Widget m_Stats;
	protected ref INT_UIF_Surface m_StatsBg;
	protected ref array<TextWidget> m_StatLabels;
	protected ref array<TextWidget> m_StatValues;
	protected ref array<ImageWidget> m_StatSeps;
	protected ref array<string> m_StatLText;
	protected ref array<string> m_StatVText;

	protected Widget m_Seal;
	protected bool m_Locked;
	protected bool m_Covered;
	protected bool m_AllOk;
	protected int m_HoverCard = -1;
	protected ref ScriptInvoker m_OnAction;

	void DRL_UIBlueprint()
	{
		m_Cards = new array<ref DRL_UIBpCard>();
		m_Side = new array<ref DRL_UIBpItem>();
		m_Top = new array<ref DRL_UIBpItem>();
		m_Dots = new array<ImageWidget>();
		m_VarChips = new array<ref DRL_UIBpChip>();
		m_VarNames = new array<string>();
		m_VarColors = new array<int>();
		m_DopChips = new array<ref DRL_UIBpDop>();
		m_Dop = new array<ref DRL_UIBpDopData>();
		m_StatLabels = new array<TextWidget>();
		m_StatValues = new array<TextWidget>();
		m_StatSeps = new array<ImageWidget>();
		m_StatLText = new array<string>();
		m_StatVText = new array<string>();
		for (int s = 0; s < STAT_ROWS; s++)
		{
			m_StatLText.Insert("");
			m_StatVText.Insert("");
		}
		m_OnAction = new ScriptInvoker();
		m_Stage = new DRL_UIStage();
	}

	void ~DRL_UIBlueprint()
	{
		ClearModels();
	}

	// (string what, int a, int b): "item" (тег, вид), "go" (RecipeId), "more",
	// "var" (индекс), "dop" (индекс, 0 галочка / 1 минус / 2 плюс / 3 вариант).
	ScriptInvoker OnAction()
	{
		return m_OnAction;
	}

	DRL_UIStage GetStage()
	{
		return m_Stage;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	array<TextWidget> GetStatLabels()
	{
		return m_StatLabels;
	}

	array<TextWidget> GetStatValues()
	{
		return m_StatValues;
	}

	Widget GetStatsRoot()
	{
		return m_Stats;
	}

	void Build(Widget parent, float x, float y, float w, float h)
	{
		m_W = w;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_Blueprint");
		int glowColor = DRL_UIC.BP(26);
		m_Glow = INT_UIF_Draw.Image(m_Root, 0, 0, 300, 300, "iuf_radial", glowColor);

		m_Lines = new DRL_UICanvas();
		m_Lines.Build(m_Root, 0, 0, w, h);

		for (int d = 0; d < MAX_SIDE + MAX_TOP + 2; d++)
		{
			ImageWidget dot = INT_UIF_Draw.Image(m_Root, 0, 0, 8, 8, "iuf_circle16", 0);
			dot.Show(false);
			m_Dots.Insert(dot);
		}

		// Изделие: сцена без подложки (3D, вращение и масштаб мышью).
		m_StageHost = INT_UIF_Draw.Frame(m_Root, 0, 0, w, h, "DRL_BpStageHost");
		m_Stage.Build(m_StageHost, w * 0.5 - 76, h * 0.5 - 76, 152, 152, true);

		m_Badge = INT_UIF_Draw.Frame(m_Root, 0, 0, 50, 24, "DRL_BpBadge");
		int ok = DRL_UIC.Ok();
		m_BadgeBg = INT_UIF_Surface.Box(m_Badge, 0, 0, 50, 24, 6, ok, 0);
		int onAccent = DRL_UIC.OnAccent();
		m_BadgeText = INT_UIF_Draw.Text(m_Badge, 0, 0, 50, 24, "", 13, onAccent, INT_UIF_Align.CENTER);
		m_BadgeText.SetBold(true);
		// Над 3D-моделью; при равном приоритете текст ушёл бы под непрозрачную заливку.
		DRL_UIKitUtil.SetSortDeep(m_Badge, DRL_UI_SORT_OVER_MODEL);
		m_BadgeText.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
		m_Badge.Show(false);

		for (int c = 0; c < MAX_SIDE + MAX_TOP; c++)
		{
			DRL_UIBpCard card = new DRL_UIBpCard();
			card.Build(m_Root, c, this);
			m_Cards.Insert(card);
		}

		int bpText = DRL_UIC.BP(180);
		m_VarLabel = INT_UIF_Draw.Text(m_Root, 0, 0, 400, 14, "", 9, bpText, INT_UIF_Align.CENTER);
		m_VarLabel.SetBold(true);
		for (int v = 0; v < MAX_VARS; v++)
		{
			DRL_UIBpChip chip = new DRL_UIBpChip();
			chip.Build(m_Root, v, this);
			m_VarChips.Insert(chip);
		}

		m_SlotsLabel = INT_UIF_Draw.Text(m_Root, 0, 0, 400, 14, "", 9, bpText, INT_UIF_Align.CENTER);
		m_SlotsLabel.SetBold(true);
		m_SlotsHost = INT_UIF_Draw.Frame(m_Root, 0, 0, 520, 32, "DRL_BpSlots");
		m_Slots = new DRL_UISlotStrip();
		m_Slots.Build(m_SlotsHost, 0, 0, 520, 32);

		m_DopLabel = INT_UIF_Draw.Text(m_Root, 0, 0, 400, 14, "", 9, bpText, INT_UIF_Align.CENTER);
		m_DopLabel.SetBold(true);
		for (int p = 0; p < MAX_DOP; p++)
		{
			DRL_UIBpDop dop = new DRL_UIBpDop();
			dop.Build(m_Root, p, this);
			m_DopChips.Insert(dop);
		}

		// Характеристики - «штамп» внизу листа.
		m_Stats = INT_UIF_Draw.Frame(m_Root, 0, 0, 500, 46, "DRL_BpStats");
		int statsFill = ARGB(200, 4, 12, 14);
		int statsBorder = DRL_UIC.BP(90);
		m_StatsBg = INT_UIF_Surface.Box(m_Stats, 0, 0, 500, 46, 8, statsFill, statsBorder);
		int sepColor = DRL_UIC.BP(56);
		int txt = DRL_UIC.Txt();
		for (int s = 0; s < STAT_ROWS; s++)
		{
			TextWidget lbl = INT_UIF_Draw.Text(m_Stats, 0, 6, 100, 14, "", 9, bpText);
			lbl.SetBold(true);
			m_StatLabels.Insert(lbl);
			TextWidget val = INT_UIF_Draw.Text(m_Stats, 0, 20, 100, 20, "", 14, txt);
			val.SetBold(true);
			m_StatValues.Insert(val);
			ImageWidget sep = INT_UIF_Draw.Line(m_Stats, 0, 0, 1, 46, sepColor);
			m_StatSeps.Insert(sep);
		}
		m_Stats.Show(false);

		// Печать «не изучен».
		m_Seal = INT_UIF_Draw.Frame(m_Root, w * 0.5 - 130, h * 0.5 - 40, 260, 80, "DRL_BpSeal");
		int sealFill = ARGB(225, 30, 22, 4);
		int sealBorder = DRL_UIC.Lk(200);
		INT_UIF_Surface.Box(m_Seal, 0, 0, 260, 80, 12, sealFill, sealBorder);
		int lk = DRL_UIC.Lk();
		ImageWidget sealIcon = INT_UIF_Draw.Icon(m_Seal, 20, 26, 28, "lock", lk);
		TextWidget sealTitle = INT_UIF_Draw.Text(m_Seal, 62, 14, 190, 28, "#STR_DRL_UI_BP_LOCKED", 20, lk);
		sealTitle.SetBold(true);
		int muted = DRL_UIC.Muted();
		TextWidget sealSub = INT_UIF_Draw.Text(m_Seal, 62, 42, 190, 20, "#STR_DRL_UI_BP_LOCKED_SUB", 11, muted);
		DRL_UIKitUtil.SetSortDeep(m_Seal, DRL_UI_SORT_OVER_MODEL);
		sealIcon.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
		sealTitle.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
		sealSub.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
		m_Seal.Show(false);
	}

	// --- данные ---------------------------------------------------------------------------
	void SetItems(array<ref DRL_UIBpItem> side, array<ref DRL_UIBpItem> top)
	{
		m_Side.Clear();
		m_Top.Clear();
		if (side)
		{
			foreach (DRL_UIBpItem a : side)
			{
				m_Side.Insert(a);
			}
		}
		if (top)
		{
			foreach (DRL_UIBpItem b : top)
			{
				if (m_Top.Count() < MAX_TOP)
					m_Top.Insert(b);
			}
		}
	}

	void SetVariants(array<string> names, array<int> colors, int selected, string label)
	{
		m_VarNames.Clear();
		m_VarColors.Clear();
		if (names)
		{
			for (int i = 0; i < names.Count() && i < MAX_VARS; i++)
			{
				m_VarNames.Insert(names[i]);
				int col = DRL_UIC.White(120);
				if (colors && i < colors.Count())
					col = colors[i];
				m_VarColors.Insert(col);
			}
		}
		m_VarSel = selected;
		m_VarLabel.SetText(label);
	}

	void SetSlots(array<string> slots, array<bool> filled, string label)
	{
		m_SlotCount = 0;
		if (slots)
			m_SlotCount = slots.Count();
		m_Slots.SetSlots(slots, filled);
		m_SlotsText = label;
		m_SlotsLabel.SetText(label);
	}

	void SetDop(array<ref DRL_UIBpDopData> dop, string label)
	{
		m_Dop.Clear();
		if (dop)
		{
			foreach (DRL_UIBpDopData d : dop)
			{
				if (m_Dop.Count() < MAX_DOP)
					m_Dop.Insert(d);
			}
		}
		m_DopText = label;
		m_DopLabel.SetText(label);
	}

	// Число на изделии («×5»), "" - скрыть.
	void SetBadge(string text)
	{
		m_BadgeValue = text;
	}

	// Всё готово к крафту - круг изделия зелёный.
	void SetAllOk(bool ok)
	{
		m_AllOk = ok;
	}

	void SetLocked(bool locked)
	{
		m_Locked = locked;
		m_Seal.Show(locked && !m_Covered);
	}

	DRL_UIBpItem GetSideItem(int i)
	{
		if (i < 0 || i >= m_Side.Count())
			return null;
		return m_Side[i];
	}

	// Панель поверх чертежа: 3D-модели прячутся (рисуются поверх 2D).
	void SetCovered(bool covered)
	{
		m_Covered = covered;
		m_Stage.SetHidden(DRL_UIStage.HIDE_POPOVER, covered);
		foreach (DRL_UIBpCard card : m_Cards)
		{
			card.ShowModel(!covered);
		}
		m_Badge.Show(!covered && m_BadgeValue != "");
		m_Seal.Show(m_Locked && !covered);
	}

	void Tick(float dt)
	{
		m_Stage.Tick(dt);
	}

	void ClearModels()
	{
		if (m_Stage)
			m_Stage.Clear();
		foreach (DRL_UIBpCard card : m_Cards)
		{
			if (card)
				card.Clear();
		}
	}

	// Скрыть всё содержимое (рецепт не выбран).
	void HideAll()
	{
		foreach (DRL_UIBpCard card : m_Cards)
		{
			card.Hide();
		}
		foreach (DRL_UIBpChip chip : m_VarChips)
		{
			chip.Hide();
		}
		foreach (DRL_UIBpDop dop : m_DopChips)
		{
			dop.Hide();
		}
		m_Lines.Clear();
		foreach (ImageWidget dot : m_Dots)
		{
			dot.Show(false);
		}
		m_VarLabel.Show(false);
		m_SlotsLabel.Show(false);
		m_SlotsHost.Show(false);
		m_DopLabel.Show(false);
		m_Stats.Show(false);
		m_Badge.Show(false);
		m_Locked = false;
		m_Seal.Show(false);
		m_Glow.Show(false);
		m_Stage.Clear();
	}

	// --- раскладка --------------------------------------------------------------------------
	void Relayout()
	{
		float w = m_W;
		float h = m_H;
		float cx = w * 0.5;
		bool hasTop = m_Top.Count() > 0;
		bool hasVars = m_VarNames.Count() > 1;
		bool hasSlots = m_SlotCount > 0;
		bool hasDop = m_Dop.Count() > 0;
		bool hasStats = StatsFilled();

		float top = 0;
		if (hasTop)
			top = 70;
		float bottom = 12;
		if (hasStats)
			bottom = 12 + 46 + 14;
		float under = 0;
		if (hasVars)
			under = under + 52;
		if (hasSlots)
			under = under + 56;
		if (hasDop)
			under = under + 54;

		// Изделие по центру свободной части; при нехватке высоты круг меньше.
		float r = CORE_R;
		float regionTop = top + 22;
		float regionBottom = h - bottom - under - 6;
		float room = (regionBottom - regionTop) * 0.5 - 14;
		if (room < r)
			r = room;
		if (r < 48)
			r = 48;
		float cy = (regionTop + regionBottom) * 0.5;
		if (cy - r - 14 < regionTop)
			cy = regionTop + r + 14;

		m_Lines.Clear();
		m_DotsUsed = 0;

		// Круг изделия, пунктирное кольцо и подсветка.
		int ring = DRL_UIC.BP(115);
		int glow = DRL_UIC.BP(30);
		if (m_AllOk)
		{
			ring = DRL_UIC.Ok(153);
			glow = DRL_UIC.Ok(34);
		}
		int outer = DRL_UIC.BP(64);
		m_Lines.Circle(cx, cy, r, false, 1.5, ring);
		m_Lines.Circle(cx, cy, r + 12, true, 1, outer);
		float gs = r * 2 + 150;
		m_Glow.Show(true);
		INT_UIF_Draw.Place(m_Glow, cx - gs * 0.5, cy - gs * 0.5, gs, gs);
		m_Glow.SetColor(glow);
		float ss = r * 1.72;
		INT_UIF_Draw.Place(m_Stage.GetRoot(), cx - ss * 0.5, cy - ss * 0.5, ss, ss);
		m_Stage.Resize(ss, ss);

		m_Badge.Show(m_BadgeValue != "" && !m_Covered);
		if (m_BadgeValue != "")
		{
			float bw = DRL_UIMeasure.TextW(m_Root, m_BadgeValue, 13, true) + 16;
			INT_UIF_Draw.Place(m_Badge, cx + r * 0.7 - bw * 0.5, cy + r * 0.62, bw, 24);
			INT_UIF_Draw.Resize(m_BadgeBg.GetRoot(), bw, 24);
			INT_UIF_Draw.Resize(m_BadgeText, bw, 24);
			m_BadgeText.SetText(m_BadgeValue);
		}

		LayoutSide(cx, cy, r, top);
		LayoutTop(cx, cy, r);

		float y = cy + r + 24;
		float maxW = w - 2 * (SIDE + CARD_W + 16);
		y = LayoutVariants(cx, y, maxW, hasVars);
		y = LayoutSlots(cx, y, maxW, hasSlots);
		y = LayoutDop(cx, y, maxW, hasDop);
		LayoutStats(cx, h - 12 - 46, maxW, hasStats);

		for (int i = m_DotsUsed; i < m_Dots.Count(); i++)
		{
			m_Dots[i].Show(false);
		}
		INT_UIF_Draw.Move(m_Seal, cx - 130, cy - 40);
		m_Seal.Show(m_Locked && !m_Covered);
	}

	protected void LayoutSide(float cx, float cy, float r, float top)
	{
		int n = m_Side.Count();
		int capFull = Capacity(top, CARD_H, CARD_GAP);
		int capCompact = Capacity(top, CARD_H_COMPACT, 4);
		float ch = CARD_H;
		float gap = CARD_GAP;
		int perSide = (n + 1) / 2;
		if (perSide > capFull)
		{
			ch = CARD_H_COMPACT;
			gap = 4;
		}
		int cap = capFull;
		if (ch < CARD_H)
			cap = capCompact;

		// Сколько выносок поместится; лишнее - карточка «ещё N».
		int shown = n;
		bool more = false;
		if (n > cap * 2)
		{
			shown = cap * 2 - 1;
			more = true;
		}
		int left = (shown + 1) / 2;
		if (more)
			left = cap;
		int right = shown - left;
		int rightSlots = right;
		if (more)
			rightSlots = right + 1;

		int cardIdx = 0;
		for (int i = 0; i < shown; i++)
		{
			bool isLeft = i < left;
			int k = i;
			int count = left;
			if (!isLeft)
			{
				k = i - left;
				count = rightSlots;
			}
			float y = ColumnY(k, count, ch, gap, cy, top);
			PlaceSide(cardIdx, m_Side[i], isLeft, y, ch, cx, cy, r);
			cardIdx++;
		}
		if (more)
		{
			DRL_UIBpItem m = new DRL_UIBpItem();
			m.m_Kind = DRL_BP_MORE;
			m.m_Ok = true;
			int rest = n - shown;
			m.m_Name = DRL_UIText.Fmt("#STR_DRL_UI_BP_MORE", rest.ToString());
			m.m_Sub = DRL_UIText.Tr("#STR_DRL_UI_BP_MORE_SUB");
			m.m_Glyph = "list";
			float my = ColumnY(rightSlots - 1, rightSlots, ch, gap, cy, top);
			PlaceSide(cardIdx, m, false, my, ch, cx, cy, r);
			cardIdx++;
		}
		for (int j = cardIdx; j < MAX_SIDE; j++)
		{
			m_Cards[j].Hide();
		}
	}

	protected int Capacity(float top, float ch, float gap)
	{
		float avail = m_H - top - 12;
		int cap = (int)((avail + gap) / (ch + gap));
		if (cap < 1)
			cap = 1;
		if (cap * 2 > MAX_SIDE)
			cap = MAX_SIDE / 2;
		return cap;
	}

	// Верх k-й карточки столбца из count штук: столбец по центру изделия, но в пределах листа.
	protected float ColumnY(int k, int count, float ch, float gap, float cy, float top)
	{
		float total = count * ch + (count - 1) * gap;
		float start = cy - total * 0.5;
		float minY = top + 6;
		float maxY = m_H - 6 - total;
		if (start > maxY)
			start = maxY;
		if (start < minY)
			start = minY;
		return start + k * (ch + gap);
	}

	protected void PlaceSide(int cardIdx, DRL_UIBpItem d, bool isLeft, float y, float ch, float cx, float cy, float r)
	{
		if (cardIdx >= MAX_SIDE)
			return;
		float x = m_W - SIDE - CARD_W;
		if (isLeft)
			x = SIDE;
		DRL_UIBpCard card = m_Cards[cardIdx];
		card.Place(d, x, y, CARD_W, ch);
		card.ShowModel(!m_Covered);

		float py = y + ch * 0.5;
		float px = x;
		if (isLeft)
			px = x + CARD_W;
		float ang = Math.Atan2(py - cy, px - cx);
		float ex = cx + Math.Cos(ang) * r;
		float ey = cy + Math.Sin(ang) * r;
		float bend = (ex - px) * 0.5;
		int col = LineColor(d);
		m_Lines.Bezier(px, py, px + bend, py, ex - bend * 0.5, ey, ex, ey, true, 1.5, col);
		Dot(ex, ey, 8, DotColor(d));
		Dot(px, py, 6, DotColor(d));
	}

	protected void LayoutTop(float cx, float cy, float r)
	{
		int n = m_Top.Count();
		for (int i = 0; i < MAX_TOP; i++)
		{
			DRL_UIBpCard card = m_Cards[MAX_SIDE + i];
			if (i >= n)
			{
				card.Hide();
				continue;
			}
			float tw = (m_W - 2 * SIDE - (n - 1) * 10) / n;
			if (tw > 260)
				tw = 260;
			float x0 = cx - (n * tw + (n - 1) * 10) * 0.5;
			float x = x0 + i * (tw + 10);
			DRL_UIBpItem d = m_Top[i];
			card.Place(d, x, 12, tw, 46);
			card.ShowModel(!m_Covered);
			float bx = x + tw * 0.5;
			float by = 58;
			float ang = Math.Atan2(by - cy, bx - cx);
			float ex = cx + Math.Cos(ang) * r;
			float ey = cy + Math.Sin(ang) * r;
			int col = LineColor(d);
			m_Lines.Bezier(bx, by, bx, by + 30, ex, ey - 30, ex, ey, true, 1.5, col);
			Dot(ex, ey, 8, DotColor(d));
		}
	}

	protected float LayoutVariants(float cx, float y, float maxW, bool show)
	{
		m_VarLabel.Show(show);
		if (!show)
		{
			foreach (DRL_UIBpChip hidden : m_VarChips)
			{
				hidden.Hide();
			}
			return y;
		}
		INT_UIF_Draw.Place(m_VarLabel, cx - 200, y, 400, 14);
		array<float> widths = new array<float>();
		float total = 0;
		int n = m_VarNames.Count();
		for (int i = 0; i < n; i++)
		{
			float cw = m_VarChips[i].Measure(m_VarNames[i]);
			widths.Insert(cw);
			total = total + cw;
		}
		total = total + (n - 1) * 6;
		// Не влезает - чипы ужимаются поровну (текст обрежется).
		float shrink = 1;
		if (total > maxW && total > 0)
			shrink = maxW / total;
		float x = cx - total * shrink * 0.5;
		for (int c = 0; c < MAX_VARS; c++)
		{
			DRL_UIBpChip chip = m_VarChips[c];
			if (c >= n)
			{
				chip.Hide();
				continue;
			}
			float wc = widths[c] * shrink;
			chip.Place(x, y + 18, wc, m_VarNames[c], m_VarColors[c], c == m_VarSel);
			x = x + wc + 6 * shrink;
		}
		return y + 52;
	}

	protected float LayoutSlots(float cx, float y, float maxW, bool show)
	{
		m_SlotsLabel.Show(show);
		m_SlotsHost.Show(show);
		if (!show)
			return y;
		INT_UIF_Draw.Place(m_SlotsLabel, cx - 200, y, 400, 14);
		int n = m_SlotCount;
		int fit = (int)((maxW + 6) / 38);
		if (n > fit)
			n = fit;
		float sw = n * 38 - 6;
		INT_UIF_Draw.Move(m_SlotsHost, cx - sw * 0.5, y + 18);
		return y + 56;
	}

	protected float LayoutDop(float cx, float y, float maxW, bool show)
	{
		m_DopLabel.Show(show);
		if (!show)
		{
			foreach (DRL_UIBpDop hidden : m_DopChips)
			{
				hidden.Hide();
			}
			return y;
		}
		INT_UIF_Draw.Place(m_DopLabel, cx - 200, y, 400, 14);
		array<float> widths = new array<float>();
		float total = 0;
		int n = m_Dop.Count();
		for (int i = 0; i < n; i++)
		{
			float dw = m_DopChips[i].Measure(m_Dop[i]);
			widths.Insert(dw);
			total = total + dw;
		}
		total = total + (n - 1) * 6;
		float shrink = 1;
		if (total > maxW && total > 0)
			shrink = maxW / total;
		float x = cx - total * shrink * 0.5;
		for (int c = 0; c < MAX_DOP; c++)
		{
			DRL_UIBpDop chip = m_DopChips[c];
			if (c >= n)
			{
				chip.Hide();
				continue;
			}
			float wc = widths[c] * shrink;
			chip.Place(x, y + 18, wc, m_Dop[c]);
			x = x + wc + 6 * shrink;
		}
		return y + 54;
	}

	// Строка характеристик: TextWidget не отдаёт текст обратно, поэтому он хранится здесь.
	void SetStat(int idx, string label, string value)
	{
		if (idx < 0 || idx >= STAT_ROWS)
			return;
		m_StatLText.Set(idx, label);
		m_StatVText.Set(idx, value);
		m_StatLabels[idx].SetText(label);
		m_StatValues[idx].SetText(value);
	}

	protected bool StatsFilled()
	{
		if (!m_Stats || !m_Stats.IsVisible())
			return false;
		foreach (string v : m_StatVText)
		{
			if (v != "" && v != "—")
				return true;
		}
		return false;
	}

	// Ячейки «подпись / значение» по ширине содержимого.
	protected void LayoutStats(float cx, float y, float maxW, bool show)
	{
		if (!show)
		{
			m_Stats.Show(false);
			return;
		}
		array<float> widths = new array<float>();
		float total = 0;
		for (int i = 0; i < STAT_ROWS; i++)
		{
			string lt = m_StatLText[i];
			string vt = m_StatVText[i];
			float cw = 0;
			if (lt != "" || vt != "")
			{
				float lw = DRL_UIMeasure.TextW(m_Root, lt, 9, true);
				float vw = DRL_UIMeasure.TextW(m_Root, vt, 14, true);
				cw = lw;
				if (vw > cw)
					cw = vw;
				cw = cw + 24;
			}
			widths.Insert(cw);
			total = total + cw;
		}
		float shrink = 1;
		if (total > maxW && total > 0)
			shrink = maxW / total;
		float sw = total * shrink;
		INT_UIF_Draw.Place(m_Stats, cx - sw * 0.5, y, sw, 46);
		INT_UIF_Draw.Resize(m_StatsBg.GetRoot(), sw, 46);
		float x = 0;
		bool first = true;
		for (int c = 0; c < STAT_ROWS; c++)
		{
			float wc = widths[c] * shrink;
			bool on = wc > 0;
			m_StatLabels[c].Show(on);
			m_StatValues[c].Show(on);
			m_StatSeps[c].Show(on && !first);
			if (!on)
				continue;
			INT_UIF_Draw.Place(m_StatLabels[c], x + 12, 7, wc - 16, 14);
			INT_UIF_Draw.Place(m_StatValues[c], x + 12, 20, wc - 16, 20);
			INT_UIF_Draw.Place(m_StatSeps[c], x, 0, 1, 46);
			x = x + wc;
			first = false;
		}
	}

	protected int LineColor(DRL_UIBpItem d)
	{
		if (d.m_Kind == DRL_BP_MORE)
			return DRL_UIC.BP(120);
		if (!d.m_Ok)
			return DRL_UIC.No(204);
		if (d.m_Kind == DRL_BP_ATT)
			return DRL_UIC.Tl(191);
		if (d.m_Kind == DRL_BP_TOOL)
			return DRL_UIC.White(89);
		return DRL_UIC.Ok(178);
	}

	protected int DotColor(DRL_UIBpItem d)
	{
		if (d.m_Kind == DRL_BP_MORE)
			return DRL_UIC.BP();
		if (!d.m_Ok)
			return DRL_UIC.No();
		if (d.m_Kind == DRL_BP_ATT)
			return DRL_UIC.Tl();
		if (d.m_Kind == DRL_BP_TOOL)
			return DRL_UIC.White(153);
		return DRL_UIC.Ok();
	}

	protected void Dot(float x, float y, float size, int color)
	{
		if (m_DotsUsed >= m_Dots.Count())
			return;
		ImageWidget dot = m_Dots[m_DotsUsed];
		m_DotsUsed++;
		INT_UIF_Draw.Place(dot, x - size * 0.5, y - size * 0.5, size, size);
		dot.SetColor(color);
		dot.Show(true);
	}

	// --- события ----------------------------------------------------------------------------
	protected int CardOf(Widget w, out bool isGo)
	{
		isGo = false;
		for (int i = 0; i < m_Cards.Count(); i++)
		{
			DRL_UIBpCard card = m_Cards[i];
			if (card.m_Hit == w)
				return i;
			if (card.m_GoHit == w)
			{
				isGo = true;
				return i;
			}
		}
		return -1;
	}

	protected int ChipOf(Widget w)
	{
		for (int i = 0; i < m_VarChips.Count(); i++)
		{
			if (m_VarChips[i].m_Hit == w)
				return i;
		}
		return -1;
	}

	protected int DopOf(Widget w, out int action)
	{
		action = 0;
		for (int i = 0; i < m_DopChips.Count(); i++)
		{
			DRL_UIBpDop d = m_DopChips[i];
			if (d.m_Hit == w)
				return i;
			if (d.m_MinusHit == w)
			{
				action = 1;
				return i;
			}
			if (d.m_PlusHit == w)
			{
				action = 2;
				return i;
			}
			if (d.m_VarHit == w)
			{
				action = 3;
				return i;
			}
		}
		return -1;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
			return false;
		bool isGo;
		int ci = CardOf(w, isGo);
		if (ci >= 0)
		{
			DRL_UIBpItem d = m_Cards[ci].m_Data;
			if (!d)
				return true;
			if (isGo && d.m_GoRecipe >= 0)
			{
				m_OnAction.Invoke("go", d.m_GoRecipe, 0);
				return true;
			}
			if (d.m_Kind == DRL_BP_MORE)
			{
				m_OnAction.Invoke("more", 0, 0);
				return true;
			}
			m_OnAction.Invoke("item", d.m_Tag, d.m_Kind);
			return true;
		}
		int vi = ChipOf(w);
		if (vi >= 0)
		{
			m_OnAction.Invoke("var", vi, 0);
			return true;
		}
		int action;
		int di = DopOf(w, action);
		if (di >= 0)
		{
			m_OnAction.Invoke("dop", di, action);
			return true;
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		bool isGo;
		int ci = CardOf(w, isGo);
		if (ci >= 0)
		{
			DRL_UIBpCard card = m_Cards[ci];
			card.SetHover(true);
			m_HoverCard = ci;
			if (card.m_Data && card.m_Data.m_Tip != "" && !isGo)
				INT_UIF_Tooltip.ShowFor(card.m_Root, card.m_Data.m_Tip);
			return true;
		}
		int action;
		int di = DopOf(w, action);
		if (di >= 0 && action == 0 && di < m_Dop.Count() && m_Dop[di].m_Tip != "")
		{
			INT_UIF_Tooltip.ShowFor(m_DopChips[di].m_Root, m_Dop[di].m_Tip);
			return true;
		}
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		bool isGo;
		int ci = CardOf(w, isGo);
		if (ci >= 0)
		{
			bool nextGo;
			if (enterW && CardOf(enterW, nextGo) == ci)
				return true;
			m_Cards[ci].SetHover(false);
			if (m_HoverCard == ci)
				m_HoverCard = -1;
			INT_UIF_Tooltip.HideTip();
			return true;
		}
		int action;
		if (DopOf(w, action) >= 0)
		{
			INT_UIF_Tooltip.HideTip();
			return true;
		}
		return false;
	}

	// Тестовый хук: клик по выноске / чипу без мыши.
	void TestClick(string what, int index, int action)
	{
		if (what == "card" && index >= 0 && index < m_Cards.Count())
		{
			OnClick(m_Cards[index].m_Hit, 0, 0, MouseState.LEFT);
			return;
		}
		if (what == "go" && index >= 0 && index < m_Cards.Count())
		{
			OnClick(m_Cards[index].m_GoHit, 0, 0, MouseState.LEFT);
			return;
		}
		if (what == "var" && index >= 0 && index < m_VarChips.Count())
		{
			OnClick(m_VarChips[index].m_Hit, 0, 0, MouseState.LEFT);
			return;
		}
		if (what == "dop" && index >= 0 && index < m_DopChips.Count())
		{
			DRL_UIBpDop d = m_DopChips[index];
			ButtonWidget target = d.m_Hit;
			if (action == 1)
				target = d.m_MinusHit;
			else if (action == 2)
				target = d.m_PlusHit;
			else if (action == 3)
				target = d.m_VarHit;
			OnClick(target, 0, 0, MouseState.LEFT);
		}
	}

	int VisibleCards()
	{
		int n = 0;
		foreach (DRL_UIBpCard card : m_Cards)
		{
			if (card.m_Data)
				n++;
		}
		return n;
	}
}
