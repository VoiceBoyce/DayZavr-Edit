// ============================================================================
// INT_UIFramework - Tooltip (v2): one shared top-level bubble.
//
//   component.SetTooltip("#STR_X_TIP");      // shown on hover by the component
//   INT_UIF_Tooltip.ShowFor(widget, "text");  // centred above a widget
//   INT_UIF_Tooltip.ShowText("text");         // next to the cursor
//   INT_UIF_Tooltip.HideTip();
//
// Dark rounded bubble with a hairline border and a soft shadow, sized to the
// measured text. It waits DELAY seconds (hover intent) and then fades in while
// rising 4 px. Anchored tips sit above the widget, below it when there is no
// room above, and always stay on screen.
// ============================================================================

class INT_UIF_Tooltip
{
	static const float DELAY = 0.3;

	protected static ref INT_UIF_TooltipView s_View;

	protected static INT_UIF_TooltipView View()
	{
		if (!s_View)
			s_View = new INT_UIF_TooltipView();
		return s_View;
	}

	static void ShowFor(Widget anchor, string text, float delay = -1)
	{
		if (text == "" || !GetGame() || GetGame().IsDedicatedServer())
			return;
		if (delay < 0)
			delay = DELAY;
		View().Request(anchor, text, delay);
	}

	static void ShowText(string text, float delay = -1)
	{
		ShowFor(null, text, delay);
	}

	static void HideTip()
	{
		if (s_View)
			s_View.Hide();
	}

	static bool IsShown()
	{
		return s_View && s_View.IsShown();
	}

	static void Shutdown()
	{
		if (s_View)
			s_View.Destroy();
		s_View = null;
	}
}

class INT_UIF_TooltipView extends Managed
{
	protected Widget m_Root;
	protected Widget m_Anchor;
	protected bool m_Anchored;
	protected string m_Text;
	protected bool m_Shown;
	// Weak: the animator owns the timer; it turns null once finished.
	protected INT_UIF_Tween m_Timer;

	void ~INT_UIF_TooltipView()
	{
		Destroy();
	}

	void Request(Widget anchor, string text, float delay)
	{
		CancelTimer();
		m_Anchor = anchor;
		m_Anchored = anchor != null;
		m_Text = text;
		if (delay <= 0)
		{
			ShowNow();
			return;
		}
		m_Timer = INT_UIF_Animator.Wait(delay);
		if (m_Timer)
			m_Timer.OnComplete().Insert(OnDelayDone);
	}

	void OnDelayDone(INT_UIF_Tween t)
	{
		m_Timer = null;
		ShowNow();
	}

	protected void CancelTimer()
	{
		if (m_Timer)
			m_Timer.Cancel();
		m_Timer = null;
	}

	bool IsShown()
	{
		return m_Shown && m_Root && m_Root.IsVisible();
	}

	void Hide()
	{
		CancelTimer();
		m_Shown = false;
		if (!m_Root)
			return;
		INT_UIF_Animator.Get().Cancel(m_Root);
		m_Root.Show(false);
	}

	void Destroy()
	{
		CancelTimer();
		if (m_Root && GetGame())
			m_Root.Unlink();
		m_Root = null;
		m_Shown = false;
	}

	protected void ShowNow()
	{
		// The hovered widget went away (or was hidden) while we waited.
		if (m_Anchored && (!m_Anchor || !m_Anchor.IsVisibleHierarchy()))
			return;
		if (!m_Root)
		{
			m_Root = INT_UIF_Widgets.Create("px_frame.layout", null);
			if (!m_Root)
				return;
			m_Root.SetName("IufTooltip");
			m_Root.SetSort(INT_UIF_SORT_TOOLTIP);
		}
		INT_UIF_Animator.Get().Cancel(m_Root);
		INT_UIF_Widgets.ClearChildren(m_Root);

		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		float factor = INT_UIF_Scale.Factor();
		if (factor <= 0)
			factor = 1;
		// Measure the real text width (engine metrics), estimate as fallback.
		TextWidget probe = INT_UIF_Draw.Text(m_Root, 0, 0, 900, 28, m_Text, 13, 0);
		float textW = INT_UIF_Draw.TextWidth(m_Text, 13);
		if (probe)
		{
			probe.Update();
			int mw;
			int mh;
			probe.GetTextSize(mw, mh);
			if (mw > 0)
				textW = mw / factor;
			probe.Unlink();
		}
		float w = textW + 26;
		float h = 30;

		INT_UIF_Surface.Shadow(m_Root, -18, -12, w + 36, h + 32, ARGB(130, 0, 0, 0));
		int bg = INT_UIF_Color.Lerp(tm.Color(INT_UIF_ThemeKeys.WINDOW), ARGB(255, 255, 255, 255), 0.05);
		INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 8, INT_UIF_Color.WithAlpha(bg, 250), ARGB(38, 255, 255, 255));
		INT_UIF_Draw.Text(m_Root, 0, 0, w, h, m_Text, 13, tm.Color(INT_UIF_ThemeKeys.TEXT), INT_UIF_Align.CENTER);

		float sw = INT_UIF_Draw.S(w);
		float sh = INT_UIF_Draw.S(h);
		float gap = INT_UIF_Draw.S(8);
		float edge = INT_UIF_Draw.S(6);
		int scrW = INT_UIF_Scale.ScreenW();
		int scrH = INT_UIF_Scale.ScreenH();
		float x;
		float y;
		if (m_Anchored)
		{
			float ax;
			float ay;
			float aw;
			float ah;
			m_Anchor.GetScreenPos(ax, ay);
			m_Anchor.GetScreenSize(aw, ah);
			x = ax + (aw - sw) * 0.5;
			y = ay - sh - gap;
			if (y < edge)
				y = ay + ah + gap;
		}
		else
		{
			int mx;
			int my;
			GetMousePos(mx, my);
			x = mx + INT_UIF_Draw.S(14);
			y = my + INT_UIF_Draw.S(22);
			if (y + sh > scrH - edge)
				y = my - sh - gap;
		}
		if (x + sw > scrW - edge)
			x = scrW - edge - sw;
		if (x < edge)
			x = edge;

		m_Root.SetPos(x, y);
		m_Root.SetSize(sw, sh);
		m_Shown = true;
		INT_UIF_Animator.Reveal(m_Root, 0, 4, 0.16);
	}
}
