// DayZavr: текстовый блок с прокруткой колесом мыши (для «Деталей рецепта»).
// RichTextWidget с переносом строк сдвигает содержимое (SetContentOffset), сам виджет на месте.
// Тонкий ползунок справа виден только тогда, когда текст не помещается. Нативный ScrollWidget
// не используется: он не берёт стили фреймворка (как в DRL_UISidebar).
class DZV_UIScrollText extends ScriptedWidgetEventHandler
{
	static const string LAYOUT = "DRL_CraftMenu/UI/layout/DZV_ScrollText.layout";

	protected RichTextWidget m_Text;
	protected ButtonWidget m_Hit;
	protected ImageWidget m_Thumb;
	protected float m_X;
	protected float m_Y;
	protected float m_W;
	protected float m_H;
	protected float m_Offset;   // в экранных пикселях, как GetContentHeight

	void ~DZV_UIScrollText()
	{
		if (GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(UpdateThumb);
	}

	void Build(Widget parent, float x, float y, float w, float h, float size, int color)
	{
		m_X = x;
		m_Y = y;
		m_W = w;
		m_H = h;
		m_Text = RichTextWidget.Cast(INT_UIF_Widgets.CreatePath(LAYOUT, parent));
		if (m_Text)
		{
			INT_UIF_Draw.Place(m_Text, x, y, w - 8, h);
			m_Text.SetTextExactSize(INT_UIF_Scale.TextPx(size));
			m_Text.SetColor(color);
		}
		m_Thumb = INT_UIF_Draw.Line(parent, x + w - 3, y, 3, h, INT_UIF.Soft(INT_UIF_ThemeKeys.TEXT, 50));
		m_Thumb.Show(false);
		m_Hit = INT_UIF_Draw.Hit(parent, x, y, w, h, this);
		m_Hit.SetName("DRL_RecipeDescScroll");
	}

	void SetText(string text)
	{
		if (!m_Text)
			return;
		m_Text.SetText(text);
		m_Offset = 0;
		m_Text.SetContentOffset(0);
		UpdateThumb();
		// высота содержимого точно известна после перерасчёта разметки — обновим ещё раз
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(UpdateThumb);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateThumb, 50, false);
	}

	protected float ViewHeight()
	{
		float sw, sh;
		m_Text.GetScreenSize(sw, sh);
		return sh;
	}

	protected float MaxOffset()
	{
		if (!m_Text)
			return 0;
		float over = m_Text.GetContentHeight() - ViewHeight();
		if (over < 0)
			return 0;
		return over;
	}

	void UpdateThumb()
	{
		if (!m_Text || !m_Thumb)
			return;
		float maxOff = MaxOffset();
		if (maxOff <= 1)
		{
			m_Thumb.Show(false);
			return;
		}
		float content = m_Text.GetContentHeight();
		float th = m_H * ViewHeight() / content;
		if (th < 12)
			th = 12;
		float ty = (m_H - th) * m_Offset / maxOff;
		INT_UIF_Draw.Place(m_Thumb, m_X + m_W - 3, m_Y + ty, 3, th);
		m_Thumb.Show(true);
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (!m_Text)
			return false;
		float maxOff = MaxOffset();
		if (maxOff <= 0)
			return false;
		float step = ViewHeight() / 3;   // примерно строка текста
		m_Offset = Math.Clamp(m_Offset - wheel * step, 0, maxOff);
		m_Text.SetContentOffset(m_Offset);
		UpdateThumb();
		return true;
	}
}
