// ============================================================================
// INT_UIFramework - display components (v2): Badge (chip), Card (panel),
// Divider, ProgressBar, Ring, Callout, StatTile.
// All geometry in design px (1920x1080 canvas), see INT_UIF_Draw.
// ============================================================================

// Chip: soft tinted pill with text (+ optional icon). Width 0 = fit the text.
class INT_UIF_Badge extends INT_UIF_Component
{
	protected string m_Text;
	protected string m_IconName;
	protected int m_Variant;
	protected ref INT_UIF_Surface m_Bg;
	protected ImageWidget m_Icon;
	protected TextWidget m_Label;

	static INT_UIF_Badge Create(INT_UIF_Bag bag, Widget parent, float x, float y, string text, int variant = INT_UIF_Variant.INFO, float h = 22, string icon = "", float w = 0)
	{
		INT_UIF_Badge b = new INT_UIF_Badge();
		b.m_Text = text;
		b.m_Variant = variant;
		b.m_IconName = icon;
		if (w <= 0)
			w = FitWidth(text, h, icon);
		if (bag)
			bag.Add(b);
		b.BuildAt(parent, x, y, w, h);
		return b;
	}

	static float FitWidth(string text, float h, string icon)
	{
		float size = TextSize(h);
		float w = INT_UIF_Draw.TextWidth(text, size) * 1.12 + h * 0.95 + 4;
		if (icon != "")
			w += size + 6;
		return Math.Round(w);
	}

	protected static float TextSize(float h)
	{
		if (h >= 30)
			return 13;
		if (h >= 24)
			return 12;
		return 11;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufBadge";
	}

	override void OnBuild()
	{
		m_Bg = INT_UIF_Surface.Pill(m_Root, 0, 0, m_W, m_H, 0, 0);
		float size = TextSize(m_H);
		float tx = m_H * 0.45;
		if (m_IconName != "")
		{
			m_Icon = INT_UIF_Draw.Icon(m_Root, tx, (m_H - size) * 0.5, size, m_IconName, 0);
			tx += size + 6;
		}
		m_Label = INT_UIF_Draw.Text(m_Root, tx, 0, m_W - tx - 2, m_H, m_Text, size, 0);
	}

	override void ApplyTheme()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int hue = tm.VariantHue(m_Variant);
		int text = tm.VariantText(m_Variant);
		if (m_Variant == INT_UIF_Variant.PRIMARY || m_Variant == INT_UIF_Variant.ACCENT)
		{
			// Solid variants as chips read as soft too: a chip is never a button.
			text = hue;
			if (m_Variant == INT_UIF_Variant.PRIMARY)
				text = tm.Color(INT_UIF_ThemeKeys.AMBER_TEXT);
		}
		if (m_Variant == INT_UIF_Variant.SECONDARY || m_Variant == INT_UIF_Variant.GHOST)
		{
			hue = tm.Color(INT_UIF_ThemeKeys.TEXT);
			text = tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		}
		if (m_Bg)
		{
			m_Bg.SetFill(INT_UIF_Color.WithAlpha(hue, 33));
			m_Bg.SetBorder(INT_UIF_Color.WithAlpha(hue, 86));
		}
		if (m_Icon)
			m_Icon.SetColor(text);
		if (m_Label)
			m_Label.SetColor(text);
	}

	void SetText(string text)
	{
		m_Text = text;
		if (m_Label)
			m_Label.SetText(text);
	}

	void SetVariant(int variant)
	{
		m_Variant = variant;
		ApplyTheme();
	}
}

// Panel: rounded surface + hairline + caption row (icon + CAPS) + optional
// title and right-aligned meta; children go into GetContent().
class INT_UIF_Card extends INT_UIF_Component
{
	protected string m_Caption;
	protected string m_IconName;
	protected string m_TitleText;
	protected ref INT_UIF_Surface m_Bg;
	protected ImageWidget m_Sheen;
	protected Widget m_CaptionRow;
	protected TextWidget m_Title;
	protected TextWidget m_Meta;
	protected Widget m_Content;
	protected float m_ContentTop;

	static INT_UIF_Card Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float h, string caption, string icon = "", string title = "")
	{
		INT_UIF_Card c = new INT_UIF_Card();
		c.m_Caption = caption;
		c.m_IconName = icon;
		c.m_TitleText = title;
		if (bag)
			bag.Add(c);
		c.BuildAt(parent, x, y, w, h);
		return c;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufCard";
	}

	override void OnBuild()
	{
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, m_W, m_H, 12, 0, 0);
		m_Sheen = INT_UIF_Draw.Image(m_Root, 14, 0, m_W - 28, 1, "iuf_grad_band", 0);
		m_ContentTop = 20;
		if (m_Caption != "")
		{
			m_CaptionRow = INT_UIF_Draw.Caption(m_Root, 20, 18, m_W - 40, m_Caption, m_IconName);
			m_ContentTop = 46;
		}
		if (m_TitleText != "")
		{
			float ty = 18;
			if (m_Caption != "")
				ty = 36;
			m_Title = INT_UIF_Draw.Text(m_Root, 20, ty, m_W - 40, 26, m_TitleText, 18, 0);
			if (m_Title)
				m_Title.SetBold(true);
			m_ContentTop = ty + 40;
		}
		m_Meta = INT_UIF_Draw.Text(m_Root, 20, 36, m_W - 40, 26, "", 12, 0, INT_UIF_Align.RIGHT);
		m_Content = INT_UIF_Draw.Frame(m_Root, 20, m_ContentTop, m_W - 40, m_H - m_ContentTop - 20, "Content");
	}

	override void ApplyTheme()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (m_Bg)
		{
			m_Bg.SetFill(tm.Color(INT_UIF_ThemeKeys.PANEL));
			m_Bg.SetBorder(tm.Color(INT_UIF_ThemeKeys.PANEL_BORDER));
		}
		if (m_Sheen)
			m_Sheen.SetColor(ARGB(30, 255, 255, 255));
		if (m_Title)
			m_Title.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT));
		if (m_Meta && m_Meta.GetColor() == 0)
			m_Meta.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
	}

	Widget GetContent()
	{
		return m_Content;
	}

	// Content area size in design px.
	float GetContentW()
	{
		return m_W - 40;
	}

	float GetContentH()
	{
		return m_H - m_ContentTop - 20;
	}

	void SetTitle(string title)
	{
		m_TitleText = title;
		if (m_Title)
			m_Title.SetText(title);
	}

	void SetMeta(string text, int color = 0)
	{
		if (!m_Meta)
			return;
		m_Meta.SetText(text);
		if (color == 0)
			color = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);
		m_Meta.SetColor(color);
	}

	// Chip right after the title text (counts, states).
	INT_UIF_Badge AddTitleBadge(string text, int variant = INT_UIF_Variant.INFO)
	{
		if (!m_Title)
			return null;
		// Titles are bold: ~10 % wider than the Book-weight estimate.
		float tx = 20 + INT_UIF_Draw.TextWidth(m_TitleText, 18) * 1.1 + 12;
		return INT_UIF_Badge.Create(m_Bag, m_Root, tx, 39, text, variant, 20);
	}
}

// Hairline separator.
class INT_UIF_Divider extends INT_UIF_Component
{
	protected ImageWidget m_Line;

	static INT_UIF_Divider Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w)
	{
		INT_UIF_Divider d = new INT_UIF_Divider();
		if (bag)
			bag.Add(d);
		d.BuildAt(parent, x, y, w, 1);
		return d;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufDivider";
	}

	override void OnBuild()
	{
		m_Line = INT_UIF_Draw.Line(m_Root, 0, 0, m_W, 1, 0);
	}

	override void ApplyTheme()
	{
		if (m_Line)
			m_Line.SetColor(C(INT_UIF_ThemeKeys.DIVIDER));
	}
}

// Thin pill bar 0..1 with a moving shine; the fill animates to new values.
class INT_UIF_ProgressBar extends INT_UIF_Component
{
	protected float m_Value;
	protected string m_HueKey;
	protected ref INT_UIF_Surface m_Track;
	protected ref INT_UIF_Surface m_Fill;
	protected ImageWidget m_Shine;
	protected TextWidget m_Label;
	protected bool m_ShowPercent;

	static INT_UIF_ProgressBar Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float h = 8, float value = 0, string hueKey = "teal")
	{
		INT_UIF_ProgressBar p = new INT_UIF_ProgressBar();
		p.m_Value = Math.Clamp(value, 0, 1);
		p.m_HueKey = hueKey;
		if (bag)
			bag.Add(p);
		p.BuildAt(parent, x, y, w, h);
		return p;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufProgress";
	}

	override void OnBuild()
	{
		m_Track = INT_UIF_Surface.Pill(m_Root, 0, 0, m_W, m_H, 0, 0);
		m_Fill = INT_UIF_Surface.Pill(m_Root, 0, 0, m_H, m_H, 0, 0);
		if (m_Fill)
			m_Shine = INT_UIF_Draw.Image(m_Fill.GetRoot(), 2, 1, 4, m_H * 0.4, "iuf_grad_band", ARGB(70, 255, 255, 255));
		SetValue(m_Value, true);
	}

	override void ApplyTheme()
	{
		if (m_Track)
			m_Track.SetFill(C(INT_UIF_ThemeKeys.TRACK));
		if (m_Fill)
			m_Fill.SetFill(C(m_HueKey));
	}

	float GetValue()
	{
		return m_Value;
	}

	void SetHue(string hueKey)
	{
		m_HueKey = hueKey;
		ApplyTheme();
	}

	void SetValue(float value, bool animate = true)
	{
		m_Value = Math.Clamp(value, 0, 1);
		if (!m_Fill)
			return;
		float fw = m_W * m_Value;
		if (fw < m_H)
			fw = m_H;
		Widget fr = m_Fill.GetRoot();
		fr.Show(m_Value > 0.001);
		if (animate)
			INT_UIF_Animator.ResizeW(fr, INT_UIF_Draw.S(fw), 0.7, INT_UIF_Ease.OUT_CUBIC);
		else
			INT_UIF_Draw.Resize(fr, fw, m_H);
		if (m_Shine)
			INT_UIF_Draw.Resize(m_Shine, fw - 4, m_H * 0.4);
	}

	// Legacy API (old demo): label and percent are no longer drawn on the bar.
	void SetLabel(string text)
	{
	}

	void ShowPercent(bool show)
	{
		m_ShowPercent = show;
	}
}

// Ring: track + masked arc (0..1 clockwise from 12 o'clock) + centre value.
class INT_UIF_Ring extends INT_UIF_Component
{
	protected float m_Value;
	protected string m_HueKey;
	protected Widget m_RingRoot;
	protected ImageWidget m_Track;
	protected ImageWidget m_Arc;
	protected ImageWidget m_Glow;
	protected TextWidget m_ValueText;
	protected TextWidget m_SubText;
	protected bool m_Percent;

	static INT_UIF_Ring Create(INT_UIF_Bag bag, Widget parent, float x, float y, float size, string hueKey = "teal")
	{
		INT_UIF_Ring r = new INT_UIF_Ring();
		r.m_HueKey = hueKey;
		r.m_Percent = true;
		if (bag)
			bag.Add(r);
		r.BuildAt(parent, x, y, size, size);
		return r;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufRing";
	}

	override void OnBuild()
	{
		float g = m_W * 1.3;
		m_Glow = INT_UIF_Draw.Image(m_Root, (m_W - g) * 0.5, (m_H - g) * 0.5, g, g, "iuf_radial", 0);
		m_RingRoot = INT_UIF_Widgets.Create("px_ring.layout", m_Root);
		INT_UIF_Draw.Place(m_RingRoot, 0, 0, m_W, m_H);
		m_Track = ImageWidget.Cast(m_RingRoot.FindAnyWidget("Track"));
		m_Arc = ImageWidget.Cast(m_RingRoot.FindAnyWidget("Arc"));
		m_ValueText = INT_UIF_Draw.Text(m_Root, 0, m_H * 0.27, m_W, m_H * 0.3, "", m_W * 0.23, 0, INT_UIF_Align.CENTER, true);
		m_SubText = INT_UIF_Draw.Text(m_Root, 0, m_H * 0.55, m_W, m_H * 0.16, "", 11, 0, INT_UIF_Align.CENTER);
	}

	override void ApplyTheme()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (m_Track)
			m_Track.SetColor(ARGB(20, 255, 255, 255));
		if (m_Arc)
			m_Arc.SetColor(tm.Color(m_HueKey));
		if (m_Glow)
			m_Glow.SetColor(INT_UIF_Color.WithAlpha(tm.Color(m_HueKey), 30));
		if (m_ValueText)
			m_ValueText.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT));
		if (m_SubText)
			m_SubText.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
	}

	void SetHue(string hueKey)
	{
		m_HueKey = hueKey;
		ApplyTheme();
	}

	void SetSubText(string text)
	{
		if (m_SubText)
			m_SubText.SetText(text);
	}

	// Centre text is the percentage unless set explicitly.
	void SetCenterText(string text)
	{
		m_Percent = false;
		if (m_ValueText)
			m_ValueText.SetText(text);
	}

	void SetValue(float value, bool animate = true)
	{
		float prev = m_Value;
		m_Value = Math.Clamp(value, 0, 1);
		if (!m_Arc)
			return;
		if (!animate)
		{
			m_Arc.SetMaskProgress(m_Value);
			ShowPercent(m_Value);
			return;
		}
		INT_UIF_Tween arc = INT_UIF_Animator.Mask(m_Arc, m_Value, 0.9, INT_UIF_Ease.OUT_CUBIC);
		if (arc)
			arc.From(prev);
		INT_UIF_Tween t = INT_UIF_Animator.Value(prev, m_Value, 0.9, INT_UIF_Ease.OUT_CUBIC);
		if (t)
			t.OnUpdate().Insert(OnCount);
	}

	protected void OnCount(float v, INT_UIF_Tween t)
	{
		ShowPercent(v);
	}

	protected void ShowPercent(float v)
	{
		if (!m_Percent || !m_ValueText)
			return;
		int pct = (int)Math.Round(v * 100);
		m_ValueText.SetText(pct.ToString() + "%");
	}
}

// Callout: tinted box with icon, title and a wrapping explanation.
class INT_UIF_Callout extends INT_UIF_Component
{
	protected string m_TitleText;
	protected string m_BodyText;
	protected string m_IconName;
	protected int m_Variant;
	protected ref INT_UIF_Surface m_Bg;
	protected ImageWidget m_Icon;
	protected TextWidget m_Title;
	protected MultilineTextWidget m_Body;

	static INT_UIF_Callout Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float h, string title, string text, int variant = INT_UIF_Variant.WARNING, string icon = "warning")
	{
		INT_UIF_Callout c = new INT_UIF_Callout();
		c.m_TitleText = title;
		c.m_BodyText = text;
		c.m_Variant = variant;
		c.m_IconName = icon;
		if (bag)
			bag.Add(c);
		c.BuildAt(parent, x, y, w, h);
		return c;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufCallout";
	}

	override void OnBuild()
	{
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, m_W, m_H, 10, 0, 0);
		float tx = 16;
		if (m_IconName != "")
		{
			m_Icon = INT_UIF_Draw.Icon(m_Root, 14, 14, 16, m_IconName, 0);
			tx = 40;
		}
		if (m_TitleText != "")
		{
			m_Title = INT_UIF_Draw.Text(m_Root, tx, 10, m_W - tx - 14, 22, m_TitleText, 13, 0);
			m_Body = INT_UIF_Draw.MText(m_Root, tx, 34, m_W - tx - 14, m_H - 42, m_BodyText, 12, 0);
		}
		else
		{
			m_Body = INT_UIF_Draw.MText(m_Root, tx, 13, m_W - tx - 14, m_H - 20, m_BodyText, 12, 0);
		}
	}

	override void ApplyTheme()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int hue = tm.VariantHue(m_Variant);
		int title = tm.VariantText(m_Variant);
		if (m_Variant == INT_UIF_Variant.SECONDARY || m_Variant == INT_UIF_Variant.GHOST)
		{
			hue = tm.Color(INT_UIF_ThemeKeys.TEXT);
			title = tm.Color(INT_UIF_ThemeKeys.TEXT);
		}
		if (m_Variant == INT_UIF_Variant.PRIMARY)
			title = tm.Color(INT_UIF_ThemeKeys.AMBER_TEXT);
		if (m_Bg)
		{
			if (m_Variant == INT_UIF_Variant.SECONDARY || m_Variant == INT_UIF_Variant.GHOST)
			{
				m_Bg.SetFill(ARGB(7, 255, 255, 255));
				m_Bg.SetBorder(ARGB(14, 255, 255, 255));
			}
			else
			{
				m_Bg.SetFill(INT_UIF_Color.WithAlpha(hue, 18));
				m_Bg.SetBorder(INT_UIF_Color.WithAlpha(hue, 66));
			}
		}
		if (m_Icon)
		{
			if (m_Variant == INT_UIF_Variant.SECONDARY || m_Variant == INT_UIF_Variant.GHOST)
				m_Icon.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
			else
				m_Icon.SetColor(title);
		}
		if (m_Title)
			m_Title.SetColor(title);
		if (m_Body)
		{
			if (m_Title)
				m_Body.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
			else
				m_Body.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		}
	}

	void SetText(string title, string text)
	{
		m_TitleText = title;
		m_BodyText = text;
		if (m_Title)
			m_Title.SetText(title);
		if (m_Body)
			m_Body.SetText(text);
	}
}

// Stat tile: label + big value (count-up animation for numbers).
class INT_UIF_StatTile extends INT_UIF_Component
{
	protected string m_LabelText;
	protected string m_ValueText;
	protected string m_HueKey;
	protected string m_Suffix;
	protected ref INT_UIF_Surface m_Bg;
	protected TextWidget m_Label;
	protected TextWidget m_Value;

	static INT_UIF_StatTile Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float h, string label, string value, string hueKey = "text")
	{
		INT_UIF_StatTile t = new INT_UIF_StatTile();
		t.m_LabelText = label;
		t.m_ValueText = value;
		t.m_HueKey = hueKey;
		if (bag)
			bag.Add(t);
		t.BuildAt(parent, x, y, w, h);
		return t;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufStatTile";
	}

	override void OnBuild()
	{
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, m_W, m_H, 10, 0, 0);
		m_Label = INT_UIF_Draw.Text(m_Root, 14, 10, m_W - 28, 18, m_LabelText, 12, 0);
		m_Value = INT_UIF_Draw.Text(m_Root, 14, 30, m_W - 28, m_H - 38, m_ValueText, 26, 0, INT_UIF_Align.LEFT, true);
	}

	override void ApplyTheme()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (m_Bg)
		{
			m_Bg.SetFill(tm.Color(INT_UIF_ThemeKeys.TILE));
			m_Bg.SetBorder(tm.Color(INT_UIF_ThemeKeys.TILE_BORDER));
		}
		if (m_Label)
			m_Label.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		if (m_Value)
			m_Value.SetColor(tm.Color(m_HueKey));
	}

	void SetValue(string value, string hueKey = "")
	{
		m_ValueText = value;
		if (hueKey != "")
			m_HueKey = hueKey;
		if (m_Value)
			m_Value.SetText(value);
		ApplyTheme();
	}

	// Counts up from 0 to value (int) with an optional suffix ("ч", "м").
	void CountTo(int value, string suffix = "", float duration = 0.8)
	{
		m_Suffix = suffix;
		INT_UIF_Tween t = INT_UIF_Animator.Value(0, value, duration, INT_UIF_Ease.OUT_CUBIC);
		if (t)
			t.OnUpdate().Insert(OnCount);
	}

	protected void OnCount(float v, INT_UIF_Tween t)
	{
		int n = (int)Math.Round(v);
		string s = n.ToString();
		if (m_Suffix != "")
			s = s + " " + m_Suffix;
		if (m_Value)
			m_Value.SetText(s);
	}
}
