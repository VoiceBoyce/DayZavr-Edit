// ============================================================================
// INT_UIFramework - buttons (v2).
//
//   INT_UIF_Button b = INT_UIF_Button.Create(m_Bag, parent, x, y, w, h, "#STR_X", INT_UIF_Variant.PRIMARY, "plus");
//   b.OnClick().Insert(OnX);        // void OnX(INT_UIF_Button button)
//
// Geometry in design px (1920x1080 canvas). Variants: PRIMARY solid amber,
// ACCENT solid teal, SECONDARY neutral tile, GHOST bare, SUCCESS / DANGER /
// WARNING / INFO soft-tinted. Hover lights the fill and border and fades a
// coloured glow in; press darkens and nudges the label down by a pixel.
// ============================================================================

class INT_UIF_Button extends INT_UIF_Component
{
	protected string m_Text;
	protected string m_IconName;
	protected int m_Variant;
	protected float m_TextSize;
	protected bool m_Hover;
	protected bool m_Down;
	protected ref ScriptInvoker m_OnClick;

	protected ref INT_UIF_Surface m_Glow;
	protected ref INT_UIF_Surface m_Body;
	protected ImageWidget m_Sheen;
	protected RichTextWidget m_Label;
	protected ButtonWidget m_Hit;

	void INT_UIF_Button()
	{
		m_OnClick = new ScriptInvoker();
		m_Variant = INT_UIF_Variant.SECONDARY;
	}

	static INT_UIF_Button Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float h, string text, int variant = INT_UIF_Variant.SECONDARY, string icon = "")
	{
		INT_UIF_Button b = new INT_UIF_Button();
		b.m_Text = text;
		b.m_Variant = variant;
		b.m_IconName = icon;
		b.m_TextSize = 13;
		if (h >= 42)
			b.m_TextSize = 14;
		if (bag)
			bag.Add(b);
		b.BuildAt(parent, x, y, w, h);
		return b;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufButton";
	}

	override void OnBuild()
	{
		m_Glow = INT_UIF_Surface.Glow(m_Root, -16, -14, m_W + 32, m_H + 30, 0);
		m_Body = INT_UIF_Surface.Box(m_Root, 0, 0, m_W, m_H, 8, 0, 0);
		m_Sheen = INT_UIF_Draw.Image(m_Root, 6, 0, m_W - 12, m_H * 0.55, "iuf_grad_v", ARGB(0, 255, 255, 255));
		m_Label = INT_UIF_Draw.Rich(m_Root, 6, 0, m_W - 12, m_H, LabelMarkup(), m_TextSize, 0, INT_UIF_Align.CENTER);
		if (m_Label)
			m_Label.SetBold(true);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, m_W, m_H, this);
	}

	protected string LabelMarkup()
	{
		string t = m_Text;
		if (m_Text.IndexOf("#") == 0)
			t = Widget.TranslateString(m_Text);
		if (m_IconName == "")
			return t;
		if (t == "")
			return INT_UIF_Widgets.IconMarkup(m_IconName);
		return INT_UIF_Widgets.IconMarkup(m_IconName) + "  " + t;
	}

	override void ApplyTheme()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int fill = tm.VariantFill(m_Variant);
		int border = tm.VariantBorder(m_Variant);
		if (m_Hover && m_Enabled)
		{
			fill = tm.VariantFillHover(m_Variant);
			border = tm.VariantBorderHover(m_Variant);
		}
		if (m_Body)
		{
			m_Body.SetFill(fill);
			m_Body.SetBorder(border);
		}
		if (m_Glow)
			m_Glow.SetFill(INT_UIF_Color.WithAlpha(tm.VariantGlow(m_Variant), 0));
		if (m_Sheen)
		{
			int sheenA = 0;
			if (m_Variant == INT_UIF_Variant.PRIMARY || m_Variant == INT_UIF_Variant.ACCENT)
				sheenA = 46;
			m_Sheen.SetColor(ARGB(sheenA, 255, 255, 255));
		}
		if (m_Label)
			m_Label.SetColor(tm.VariantText(m_Variant));
		if (m_Root)
		{
			if (m_Enabled)
				m_Root.SetAlpha(1);
			else
				m_Root.SetAlpha(0.42);
		}
	}

	// (INT_UIF_Button button)
	ScriptInvoker OnClick()
	{
		return m_OnClick;
	}

	string GetText()
	{
		return m_Text;
	}

	void SetText(string text)
	{
		m_Text = text;
		if (m_Label)
			m_Label.SetText(LabelMarkup());
	}

	void SetIcon(string icon)
	{
		m_IconName = icon;
		if (m_Label)
			m_Label.SetText(LabelMarkup());
	}

	void SetVariant(int variant)
	{
		m_Variant = variant;
		ApplyTheme();
	}

	int GetVariant()
	{
		return m_Variant;
	}

	// --- interaction ----------------------------------------------------------------
	protected void HoverTo(bool hover)
	{
		m_Hover = hover;
		if (!m_Enabled || !m_Body)
			return;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		float d = AnimFast();
		if (hover)
		{
			m_Body.FadeFill(tm.VariantFillHover(m_Variant), d);
			m_Body.FadeBorder(tm.VariantBorderHover(m_Variant), d);
			int glow = tm.VariantGlow(m_Variant);
			if (INT_UIF_Color.A(glow) > 0)
				m_Glow.FadeFill(glow, AnimNormal());
		}
		else
		{
			m_Body.FadeFill(tm.VariantFill(m_Variant), AnimNormal());
			m_Body.FadeBorder(tm.VariantBorder(m_Variant), AnimNormal());
			m_Glow.FadeFill(INT_UIF_Color.WithAlpha(tm.VariantGlow(m_Variant), 0), AnimNormal());
		}
	}

	protected void PressTo(bool down)
	{
		m_Down = down;
		if (!m_Label || !m_Body)
			return;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (down)
		{
			m_Body.FadeFill(INT_UIF_Color.Shade(tm.VariantFillHover(m_Variant), 0.86), 0.06);
			INT_UIF_Draw.Move(m_Label, 6, 1);
		}
		else
		{
			int f = tm.VariantFill(m_Variant);
			if (m_Hover)
				f = tm.VariantFillHover(m_Variant);
			m_Body.FadeFill(f, AnimFast());
			INT_UIF_Draw.Move(m_Label, 6, 0);
		}
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_Hit)
			return false;
		HoverTo(true);
		TooltipEnter();
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_Hit)
			return false;
		HoverTo(false);
		if (m_Down)
			PressTo(false);
		TooltipLeave();
		return true;
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT || !m_Enabled)
			return false;
		PressTo(true);
		return false;
	}

	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		if (m_Down)
			PressTo(false);
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		if (!m_Enabled)
			return true;
		SetFocus(null);
		// A click handler may free this button (dialog/menu swap clears its bag):
		// the local ref keeps it alive until this call returns.
		INT_UIF_Button keep = this;
		m_OnClick.Invoke(this);
		return true;
	}
}

// Square icon-only button (tile + icon). Give it a square rect.
class INT_UIF_IconButton extends INT_UIF_Component
{
	protected string m_IconName;
	protected int m_Variant;
	protected bool m_Hover;
	protected ref ScriptInvoker m_OnClick;

	protected ref INT_UIF_Surface m_Glow;
	protected ref INT_UIF_Surface m_Body;
	protected ImageWidget m_Icon;
	protected ButtonWidget m_Hit;

	void INT_UIF_IconButton()
	{
		m_OnClick = new ScriptInvoker();
		m_Variant = INT_UIF_Variant.SECONDARY;
	}

	static INT_UIF_IconButton Create(INT_UIF_Bag bag, Widget parent, float x, float y, float size, string icon, int variant = INT_UIF_Variant.SECONDARY, string tooltip = "")
	{
		INT_UIF_IconButton b = new INT_UIF_IconButton();
		b.m_IconName = icon;
		b.m_Variant = variant;
		b.SetTooltip(tooltip);
		if (bag)
			bag.Add(b);
		b.BuildAt(parent, x, y, size, size);
		return b;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufIconButton";
	}

	override void OnBuild()
	{
		m_Glow = INT_UIF_Surface.Glow(m_Root, -14, -12, m_W + 28, m_H + 26, 0);
		m_Body = INT_UIF_Surface.Box(m_Root, 0, 0, m_W, m_H, 8, 0, 0);
		float isz = m_W * 0.46;
		m_Icon = INT_UIF_Draw.Icon(m_Root, (m_W - isz) * 0.5, (m_H - isz) * 0.5, isz, m_IconName, 0);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, m_W, m_H, this);
	}

	override void ApplyTheme()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (m_Body)
		{
			if (m_Hover)
			{
				m_Body.SetFill(tm.VariantFillHover(m_Variant));
				m_Body.SetBorder(tm.VariantBorderHover(m_Variant));
			}
			else
			{
				m_Body.SetFill(tm.VariantFill(m_Variant));
				m_Body.SetBorder(tm.VariantBorder(m_Variant));
			}
		}
		if (m_Glow)
			m_Glow.SetFill(INT_UIF_Color.WithAlpha(tm.VariantGlow(m_Variant), 0));
		if (m_Icon)
			m_Icon.SetColor(tm.VariantText(m_Variant));
		if (m_Root)
		{
			if (m_Enabled)
				m_Root.SetAlpha(1);
			else
				m_Root.SetAlpha(0.42);
		}
	}

	// (INT_UIF_IconButton button)
	ScriptInvoker OnClick()
	{
		return m_OnClick;
	}

	void SetIcon(string icon)
	{
		m_IconName = icon;
		INT_UIF_Widgets.SetIcon(m_Icon, icon);
	}

	void SetVariant(int variant)
	{
		m_Variant = variant;
		ApplyTheme();
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = true;
		if (m_Enabled && m_Body)
		{
			INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
			m_Body.FadeFill(tm.VariantFillHover(m_Variant), AnimFast());
			m_Body.FadeBorder(tm.VariantBorderHover(m_Variant), AnimFast());
			int glow = tm.VariantGlow(m_Variant);
			if (INT_UIF_Color.A(glow) > 0)
				m_Glow.FadeFill(glow, AnimNormal());
		}
		TooltipEnter();
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = false;
		if (m_Body)
		{
			INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
			m_Body.FadeFill(tm.VariantFill(m_Variant), AnimNormal());
			m_Body.FadeBorder(tm.VariantBorder(m_Variant), AnimNormal());
			m_Glow.FadeFill(INT_UIF_Color.WithAlpha(tm.VariantGlow(m_Variant), 0), AnimNormal());
		}
		TooltipLeave();
		return true;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		if (!m_Enabled)
			return true;
		SetFocus(null);
		INT_UIF_IconButton keep = this;
		m_OnClick.Invoke(this);
		return true;
	}
}
