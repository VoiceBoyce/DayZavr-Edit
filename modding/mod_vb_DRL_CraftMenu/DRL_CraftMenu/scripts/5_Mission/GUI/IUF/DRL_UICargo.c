/**
 * DRL_UICargo.c — мини-сетка груза изделия (DayZavr 2026-11-25, Баги №75).
 * Прямоугольник cols×rows ячеек в пропорции, длинная сторона — по горизонтали. Ячейка не больше MAX_CELL;
 * если ячейка мельче MIN_CELL (большой груз машины), линии идут через k ячеек — контур и пропорция сохраняются.
 * Используют «Раскрой» (швейная, блок «Вместимость») и «Сборка» (подъёмник, блок характеристик).
 */
class DRL_UICargoGrid extends Managed
{
	static const float MIN_CELL = 4;
	static const float MAX_CELL = 11;

	protected Widget m_Root;
	protected ref INT_UIF_Surface m_Bg;
	protected ref DRL_UICanvas m_Lines;
	protected float m_MaxW;
	protected float m_MaxH;
	protected float m_W;
	protected float m_H;

	void Build(Widget parent, float x, float y, float maxW, float maxH)
	{
		m_MaxW = maxW;
		m_MaxH = maxH;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, maxW, maxH, "DRL_CargoGrid");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, maxW, maxH, 3, DRL_UIC.BP(34), DRL_UIC.BP(170));
		m_Lines = new DRL_UICanvas();
		m_Lines.Build(m_Root, 0, 0, maxW, maxH);
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	float Width()
	{
		return m_W;
	}

	float Height()
	{
		return m_H;
	}

	void Show(bool show)
	{
		if (m_Root)
			m_Root.Show(show);
	}

	void Move(float x, float y)
	{
		INT_UIF_Draw.Move(m_Root, x, y);
	}

	// Размер сетки; 0 — скрыть.
	void Set(int cols, int rows)
	{
		m_Lines.Clear();
		if (cols <= 0 || rows <= 0)
		{
			m_W = 0;
			m_H = 0;
			m_Root.Show(false);
			return;
		}
		if (rows > cols)
		{
			int t = cols;
			cols = rows;
			rows = t;
		}
		float cell = m_MaxW / cols;
		float ch = m_MaxH / rows;
		if (ch < cell)
			cell = ch;
		if (cell > MAX_CELL)
			cell = MAX_CELL;
		m_W = cols * cell;
		m_H = rows * cell;
		if (m_W < 2)
			m_W = 2;
		if (m_H < 2)
			m_H = 2;
		INT_UIF_Draw.Resize(m_Root, m_W, m_H);
		INT_UIF_Draw.Resize(m_Bg.GetRoot(), m_W, m_H);
		INT_UIF_Draw.Resize(m_Lines.GetRoot(), m_W, m_H);

		int k = 1;
		while (cell * k < MIN_CELL)
		{
			k++;
		}
		int line = DRL_UIC.BP(80);
		for (int c = k; c < cols; c = c + k)
		{
			float x = c * cell;
			m_Lines.Line(x, 1, x, m_H - 1, 1, line);
		}
		for (int r = k; r < rows; r = r + k)
		{
			float y = r * cell;
			m_Lines.Line(1, y, m_W - 1, y, 1, line);
		}
		m_Root.Show(true);
	}
}
