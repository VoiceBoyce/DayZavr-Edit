// INT AutoRun - HUD pill (INT_UIFramework component, no input, top-level widget).
// Look (from the user screenshot): dot + "Автобег" + [СПРИНТ|БЕГ] badge + hints.
// Built only from framework primitives on px_frame.layout; show/hide via Animator fades.
class INT_AR_Hud extends INT_UIF_Component
{
	protected TextWidget m_Title;
	protected TextWidget m_BadgeText;
	protected INT_UIF_Surface m_BadgeBox;
	protected Widget m_Dot;
	protected bool m_Sprint = true;
	protected bool m_Shown = false;

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IntAutoRunHud";
	}

	override void OnBuild()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		float w = m_W;
		float h = m_H;
		m_Root.SetSort(INT_UIF_SORT_TOAST - 1000);
		INT_UIF_Surface.Shadow(m_Root, -12, -8, w + 24, h + 18, tm.Color(INT_UIF_ThemeKeys.SHADOW));
		INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 10, tm.Color(INT_UIF_ThemeKeys.SURFACE), ARGB(26, 255, 255, 255));
		m_Dot = INT_UIF_Draw.Dot(m_Root, 14, (h - 8) * 0.5, 8, tm.Color(INT_UIF_ThemeKeys.SUCCESS), true);
		m_Title = INT_UIF_Draw.Text(m_Root, 28, 0, 110, h, "#STR_INT_AR_TITLE", 14, tm.Color(INT_UIF_ThemeKeys.TEXT), INT_UIF_Align.LEFT);
		m_BadgeBox = INT_UIF_Surface.Pill(m_Root, 140, (h - 22) * 0.5, 80, 22, tm.Color(INT_UIF_ThemeKeys.AMBER), 0);
		m_BadgeText = INT_UIF_Draw.Text(m_Root, 140, (h - 22) * 0.5, 80, 22, "#STR_INT_AR_SPRINT", 12, tm.Color(INT_UIF_ThemeKeys.TEXT_ON_ACCENT), INT_UIF_Align.CENTER);
		INT_UIF_Draw.Text(m_Root, 228, 0, w - 228 - 8, h, "#STR_INT_AR_HINT", 12, tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED), INT_UIF_Align.LEFT);
		ApplyBadge();
	}

	override void ApplyTheme()
	{
		if (!m_Root)
			return;
		ApplyBadge();
	}

	void ShowHud()
	{
		if (!m_Root)
			return;
		m_Shown = true;
		m_Root.Show(true);
		INT_UIF_Animator.FadeIn(m_Root, 0.25);
	}

	// Bottom-anchored placement in screen px: center + top computed by Mission
	// from the live quickbar position (INT_AR_HudTop).
	void PlacePill(float topPx)
	{
		if (!m_Root)
			return;
		int sw;
		int sh;
		GetScreenSize(sw, sh);
		if (sw <= 0 || sh <= 0)
			return;
		float wPx = INT_UIF_Scale.Px(m_W);
		m_Root.SetPos((sw - wPx) * 0.5, topPx);
	}

	void HideHud()
	{
		if (!m_Root)
			return;
		m_Shown = false;
		INT_UIF_Animator.FadeOut(m_Root, 0.4, true);
	}

	bool IsShown()
	{
		return m_Shown;
	}

	void SetSprint(bool sprint)
	{
		if (m_Sprint == sprint)
			return;
		m_Sprint = sprint;
		ApplyBadge();
	}

	protected void ApplyBadge()
	{
		if (!m_Root)
			return;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (m_Sprint)
		{
			if (m_BadgeText)
				m_BadgeText.SetText("#STR_INT_AR_SPRINT");
			if (m_BadgeBox)
				m_BadgeBox.FadeFill(tm.Color(INT_UIF_ThemeKeys.AMBER), 0.15);
		}
		else
		{
			if (m_BadgeText)
				m_BadgeText.SetText("#STR_INT_AR_RUN");
			if (m_BadgeBox)
				m_BadgeBox.FadeFill(tm.Color(INT_UIF_ThemeKeys.ACCENT), 0.15);
		}
	}
}
