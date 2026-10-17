// ============================================================================
// INT_UIFramework - themes: named colour tokens + metric tokens.
// Every colour a component paints comes from here (art is white, SetColor
// tints it), so switching the active theme recolours the whole UI at once.
// ============================================================================

class INT_UIF_ThemeKeys
{
	// Surfaces
	static const string OVERLAY = "overlay";             // full-screen dim behind menus/dialogs
	static const string SHADOW = "shadow";               // window drop shadow
	static const string WINDOW = "window";               // menu window body
	static const string HEADER = "header";               // menu header strip
	static const string SURFACE = "surface";             // cards, bars
	static const string SURFACE_ALT = "surface_alt";     // inputs, zebra rows
	static const string SURFACE_HOVER = "surface_hover"; // hovered row / ghost button
	static const string BORDER = "border";
	static const string DIVIDER = "divider";
	// Text
	static const string TEXT = "text";
	static const string TEXT_MUTED = "text_muted";
	static const string TEXT_DISABLED = "text_disabled";
	static const string TEXT_ON_ACCENT = "text_on_accent";
	// Accent (primary)
	static const string ACCENT = "accent";
	static const string ACCENT_HOVER = "accent_hover";
	static const string ACCENT_SOFT = "accent_soft";     // selection background
	// Variants
	static const string SECONDARY = "secondary";
	static const string SECONDARY_HOVER = "secondary_hover";
	static const string SUCCESS = "success";
	static const string SUCCESS_HOVER = "success_hover";
	static const string WARNING = "warning";
	static const string WARNING_HOVER = "warning_hover";
	static const string DANGER = "danger";
	static const string DANGER_HOVER = "danger_hover";
	static const string INFO = "info";
	// Controls
	static const string TRACK = "track";                 // toggle/slider/progress track
	static const string KNOB = "knob";
	// v2 design tokens
	static const string WINDOW_BORDER = "window_border";
	static const string PANEL = "panel";                 // column cards
	static const string PANEL_BORDER = "panel_border";
	static const string TILE = "tile";                   // rows, tiles, inputs
	static const string TILE_HOVER = "tile_hover";
	static const string TILE_BORDER = "tile_border";
	static const string SHEEN = "sheen";                 // top highlight line
	static const string TEXT_FAINT = "text_faint";       // captions, hints
	static const string TEAL = "teal";
	static const string AMBER = "amber";
	static const string AMBER_TEXT = "amber_text";       // amber readable as text
	static const string RED = "red";
	static const string GREEN = "green";
	static const string BLUE = "blue";
	static const string VIOLET = "violet";

	// Metrics (seconds / 1080p pixels)
	static const string ANIM_FAST = "anim_fast";
	static const string ANIM_NORMAL = "anim_normal";
	static const string ANIM_SLOW = "anim_slow";
	static const string ROW_HEIGHT = "row_height";
	static const string TOAST_WIDTH = "toast_width";
	static const string TOAST_HEIGHT = "toast_height";
}

class INT_UIF_Theme extends Managed
{
	protected string m_Name;
	protected string m_DisplayName;
	protected ref map<string, int> m_Colors;
	protected ref map<string, float> m_Metrics;

	void INT_UIF_Theme(string name, string displayName)
	{
		m_Name = name;
		m_DisplayName = displayName;
		m_Colors = new map<string, int>();
		m_Metrics = new map<string, float>();
	}

	string GetName()
	{
		return m_Name;
	}

	string GetDisplayName()
	{
		return m_DisplayName;
	}

	void SetColor(string key, int argb)
	{
		m_Colors.Set(key, argb);
	}

	bool HasColor(string key)
	{
		return m_Colors.Contains(key);
	}

	// Unknown keys paint magenta so a typo is visible instead of invisible.
	int Color(string key)
	{
		int c;
		if (m_Colors.Find(key, c))
			return c;
		return ARGB(255, 255, 0, 255);
	}

	void SetMetric(string key, float value)
	{
		m_Metrics.Set(key, value);
	}

	float Metric(string key, float fallback = 0)
	{
		float v;
		if (m_Metrics.Find(key, v))
			return v;
		return fallback;
	}

	// Copies every token this theme does not define yet (inheritance for
	// custom themes: define only what differs, inherit the rest).
	void InheritFrom(INT_UIF_Theme other)
	{
		if (!other)
			return;
		map<string, int> colors = other.GetColors();
		foreach (string ck, int cv : colors)
		{
			if (!m_Colors.Contains(ck))
				m_Colors.Set(ck, cv);
		}
		map<string, float> metrics = other.GetMetrics();
		foreach (string mk, float mv : metrics)
		{
			if (!m_Metrics.Contains(mk))
				m_Metrics.Set(mk, mv);
		}
	}

	map<string, int> GetColors()
	{
		return m_Colors;
	}

	map<string, float> GetMetrics()
	{
		return m_Metrics;
	}
}

class INT_UIF_ThemePresets
{
	// Default v2 "modern dark": charcoal surfaces, hairline borders, teal for
	// live/positive state, amber for the primary action (tools/iuf_kit.py C{}).
	static INT_UIF_Theme Tactical()
	{
		INT_UIF_Theme t = new INT_UIF_Theme("tactical", "Modern dark");
		t.SetColor(INT_UIF_ThemeKeys.OVERLAY, ARGB(168, 5, 7, 10));
		t.SetColor(INT_UIF_ThemeKeys.SHADOW, ARGB(158, 0, 0, 0));
		t.SetColor(INT_UIF_ThemeKeys.WINDOW, ARGB(251, 16, 19, 22));
		t.SetColor(INT_UIF_ThemeKeys.WINDOW_BORDER, ARGB(22, 255, 255, 255));
		t.SetColor(INT_UIF_ThemeKeys.HEADER, ARGB(251, 16, 19, 22));
		t.SetColor(INT_UIF_ThemeKeys.PANEL, ARGB(255, 21, 25, 29));
		t.SetColor(INT_UIF_ThemeKeys.PANEL_BORDER, ARGB(17, 255, 255, 255));
		t.SetColor(INT_UIF_ThemeKeys.TILE, ARGB(255, 26, 31, 36));
		t.SetColor(INT_UIF_ThemeKeys.TILE_HOVER, ARGB(255, 33, 39, 45));
		t.SetColor(INT_UIF_ThemeKeys.TILE_BORDER, ARGB(14, 255, 255, 255));
		t.SetColor(INT_UIF_ThemeKeys.SHEEN, ARGB(13, 255, 255, 255));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE, ARGB(255, 21, 25, 29));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE_ALT, ARGB(255, 26, 31, 36));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE_HOVER, ARGB(255, 33, 39, 45));
		t.SetColor(INT_UIF_ThemeKeys.BORDER, ARGB(24, 255, 255, 255));
		t.SetColor(INT_UIF_ThemeKeys.DIVIDER, ARGB(15, 255, 255, 255));
		t.SetColor(INT_UIF_ThemeKeys.TEXT, ARGB(255, 236, 239, 242));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_MUTED, ARGB(255, 162, 170, 179));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_FAINT, ARGB(255, 116, 124, 134));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_DISABLED, ARGB(255, 84, 91, 99));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_ON_ACCENT, ARGB(255, 28, 20, 6));
		t.SetColor(INT_UIF_ThemeKeys.TEAL, ARGB(255, 79, 216, 182));
		t.SetColor(INT_UIF_ThemeKeys.AMBER, ARGB(255, 245, 165, 36));
		t.SetColor(INT_UIF_ThemeKeys.AMBER_TEXT, ARGB(255, 248, 188, 90));
		t.SetColor(INT_UIF_ThemeKeys.RED, ARGB(255, 242, 102, 102));
		t.SetColor(INT_UIF_ThemeKeys.GREEN, ARGB(255, 98, 214, 160));
		t.SetColor(INT_UIF_ThemeKeys.BLUE, ARGB(255, 105, 169, 255));
		t.SetColor(INT_UIF_ThemeKeys.VIOLET, ARGB(255, 167, 146, 255));
		// Legacy keys mapped onto the v2 palette.
		t.SetColor(INT_UIF_ThemeKeys.ACCENT, ARGB(255, 79, 216, 182));
		t.SetColor(INT_UIF_ThemeKeys.ACCENT_HOVER, ARGB(255, 112, 228, 199));
		t.SetColor(INT_UIF_ThemeKeys.ACCENT_SOFT, ARGB(36, 79, 216, 182));
		t.SetColor(INT_UIF_ThemeKeys.SECONDARY, ARGB(255, 26, 31, 36));
		t.SetColor(INT_UIF_ThemeKeys.SECONDARY_HOVER, ARGB(255, 33, 39, 45));
		t.SetColor(INT_UIF_ThemeKeys.SUCCESS, ARGB(255, 79, 216, 182));
		t.SetColor(INT_UIF_ThemeKeys.SUCCESS_HOVER, ARGB(255, 112, 228, 199));
		t.SetColor(INT_UIF_ThemeKeys.WARNING, ARGB(255, 245, 165, 36));
		t.SetColor(INT_UIF_ThemeKeys.WARNING_HOVER, ARGB(255, 250, 186, 80));
		t.SetColor(INT_UIF_ThemeKeys.DANGER, ARGB(255, 242, 102, 102));
		t.SetColor(INT_UIF_ThemeKeys.DANGER_HOVER, ARGB(255, 248, 132, 132));
		t.SetColor(INT_UIF_ThemeKeys.INFO, ARGB(255, 105, 169, 255));
		t.SetColor(INT_UIF_ThemeKeys.TRACK, ARGB(20, 255, 255, 255));
		t.SetColor(INT_UIF_ThemeKeys.KNOB, ARGB(255, 240, 244, 248));

		t.SetMetric(INT_UIF_ThemeKeys.ANIM_FAST, 0.14);
		t.SetMetric(INT_UIF_ThemeKeys.ANIM_NORMAL, 0.24);
		t.SetMetric(INT_UIF_ThemeKeys.ANIM_SLOW, 0.4);
		t.SetMetric(INT_UIF_ThemeKeys.ROW_HEIGHT, 56);
		t.SetMetric(INT_UIF_ThemeKeys.TOAST_WIDTH, 384);
		t.SetMetric(INT_UIF_ThemeKeys.TOAST_HEIGHT, 76);
		return t;
	}

	// Warm "field" variant: olive-tinted surfaces, same accents. Inherits the rest.
	static INT_UIF_Theme Khaki()
	{
		INT_UIF_Theme t = new INT_UIF_Theme("khaki", "Field");
		t.SetColor(INT_UIF_ThemeKeys.WINDOW, ARGB(251, 20, 21, 16));
		t.SetColor(INT_UIF_ThemeKeys.HEADER, ARGB(251, 20, 21, 16));
		t.SetColor(INT_UIF_ThemeKeys.PANEL, ARGB(255, 27, 29, 22));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE, ARGB(255, 27, 29, 22));
		t.SetColor(INT_UIF_ThemeKeys.TILE, ARGB(255, 34, 36, 28));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE_ALT, ARGB(255, 34, 36, 28));
		t.SetColor(INT_UIF_ThemeKeys.TILE_HOVER, ARGB(255, 42, 45, 35));
		t.SetColor(INT_UIF_ThemeKeys.SURFACE_HOVER, ARGB(255, 42, 45, 35));
		t.SetColor(INT_UIF_ThemeKeys.SECONDARY, ARGB(255, 34, 36, 28));
		t.SetColor(INT_UIF_ThemeKeys.SECONDARY_HOVER, ARGB(255, 42, 45, 35));
		t.SetColor(INT_UIF_ThemeKeys.TEXT, ARGB(255, 238, 233, 214));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_MUTED, ARGB(255, 176, 170, 146));
		t.SetColor(INT_UIF_ThemeKeys.TEXT_FAINT, ARGB(255, 128, 123, 103));
		t.InheritFrom(Tactical());
		return t;
	}
}

// Active theme + change notification. Subscribers: components and menus
// (ScriptInvoker, removed in their destructors - enforce rule 28).
class INT_UIF_ThemeManager
{
	protected static ref INT_UIF_ThemeManager s_Instance;

	protected ref map<string, ref INT_UIF_Theme> m_Themes;
	protected ref array<string> m_Order;
	protected INT_UIF_Theme m_Active;
	protected ref ScriptInvoker m_OnChanged;

	void INT_UIF_ThemeManager()
	{
		m_Themes = new map<string, ref INT_UIF_Theme>();
		m_Order = new array<string>();
		m_OnChanged = new ScriptInvoker();
		Register(INT_UIF_ThemePresets.Tactical());
		Register(INT_UIF_ThemePresets.Khaki());
		m_Active = m_Themes.Get("tactical");
	}

	static INT_UIF_ThemeManager Get()
	{
		if (!s_Instance)
			s_Instance = new INT_UIF_ThemeManager();
		return s_Instance;
	}

	// The instance if it exists, without creating one (safe in destructors).
	static INT_UIF_ThemeManager Peek()
	{
		return s_Instance;
	}

	void Register(INT_UIF_Theme theme)
	{
		if (!theme)
			return;
		if (!m_Themes.Contains(theme.GetName()))
			m_Order.Insert(theme.GetName());
		m_Themes.Set(theme.GetName(), theme);
	}

	bool SetActive(string name)
	{
		INT_UIF_Theme theme;
		if (!m_Themes.Find(name, theme))
		{
			INT_UIF_Log.Error("unknown theme: " + name);
			return false;
		}
		m_Active = theme;
		INT_UIF_Log.Info("theme -> " + name);
		m_OnChanged.Invoke(m_Active);
		return true;
	}

	INT_UIF_Theme Active()
	{
		return m_Active;
	}

	INT_UIF_Theme Find(string name)
	{
		INT_UIF_Theme theme;
		m_Themes.Find(name, theme);
		return theme;
	}

	array<string> GetNames()
	{
		return m_Order;
	}

	int Color(string key)
	{
		return m_Active.Color(key);
	}

	float Metric(string key, float fallback = 0)
	{
		return m_Active.Metric(key, fallback);
	}

	ScriptInvoker GetOnChanged()
	{
		return m_OnChanged;
	}

	// --- variant colours (v2) -------------------------------------------------------
	// Soft variants use the hue at low alpha for the fill and a stronger alpha
	// for the border, with the hue itself as text colour.
	int VariantHue(int variant)
	{
		if (variant == INT_UIF_Variant.SUCCESS || variant == INT_UIF_Variant.ACCENT)
			return Color(INT_UIF_ThemeKeys.TEAL);
		if (variant == INT_UIF_Variant.DANGER)
			return Color(INT_UIF_ThemeKeys.RED);
		if (variant == INT_UIF_Variant.WARNING || variant == INT_UIF_Variant.PRIMARY)
			return Color(INT_UIF_ThemeKeys.AMBER);
		if (variant == INT_UIF_Variant.INFO)
			return Color(INT_UIF_ThemeKeys.BLUE);
		return Color(INT_UIF_ThemeKeys.TEXT_MUTED);
	}

	bool IsSoft(int variant)
	{
		return variant == INT_UIF_Variant.SUCCESS || variant == INT_UIF_Variant.DANGER || variant == INT_UIF_Variant.WARNING || variant == INT_UIF_Variant.INFO;
	}

	int VariantFill(int variant)
	{
		if (variant == INT_UIF_Variant.PRIMARY)
			return Color(INT_UIF_ThemeKeys.AMBER);
		if (variant == INT_UIF_Variant.ACCENT)
			return Color(INT_UIF_ThemeKeys.TEAL);
		if (variant == INT_UIF_Variant.SECONDARY)
			return Color(INT_UIF_ThemeKeys.TILE);
		if (variant == INT_UIF_Variant.GHOST)
			return INT_UIF_Color.WithAlpha(Color(INT_UIF_ThemeKeys.TEXT), 0);
		return INT_UIF_Color.WithAlpha(VariantHue(variant), 31);
	}

	int VariantFillHover(int variant)
	{
		if (variant == INT_UIF_Variant.PRIMARY)
			return ARGB(255, 250, 186, 80);
		if (variant == INT_UIF_Variant.ACCENT)
			return Color(INT_UIF_ThemeKeys.ACCENT_HOVER);
		if (variant == INT_UIF_Variant.SECONDARY)
			return Color(INT_UIF_ThemeKeys.TILE_HOVER);
		if (variant == INT_UIF_Variant.GHOST)
			return INT_UIF_Color.WithAlpha(Color(INT_UIF_ThemeKeys.TEXT), 14);
		return INT_UIF_Color.WithAlpha(VariantHue(variant), 54);
	}

	int VariantBorder(int variant)
	{
		if (variant == INT_UIF_Variant.PRIMARY || variant == INT_UIF_Variant.ACCENT || variant == INT_UIF_Variant.GHOST)
			return 0;
		if (variant == INT_UIF_Variant.SECONDARY)
			return ARGB(24, 255, 255, 255);
		return INT_UIF_Color.WithAlpha(VariantHue(variant), 92);
	}

	int VariantBorderHover(int variant)
	{
		if (variant == INT_UIF_Variant.PRIMARY || variant == INT_UIF_Variant.ACCENT || variant == INT_UIF_Variant.GHOST)
			return 0;
		if (variant == INT_UIF_Variant.SECONDARY)
			return ARGB(46, 255, 255, 255);
		return INT_UIF_Color.WithAlpha(VariantHue(variant), 150);
	}

	int VariantText(int variant)
	{
		if (variant == INT_UIF_Variant.PRIMARY || variant == INT_UIF_Variant.ACCENT)
			return Color(INT_UIF_ThemeKeys.TEXT_ON_ACCENT);
		if (variant == INT_UIF_Variant.SECONDARY)
			return Color(INT_UIF_ThemeKeys.TEXT);
		if (variant == INT_UIF_Variant.GHOST)
			return Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		if (variant == INT_UIF_Variant.WARNING)
			return Color(INT_UIF_ThemeKeys.AMBER_TEXT);
		return VariantHue(variant);
	}

	// Glow behind the control on hover (alpha 0 = no glow).
	int VariantGlow(int variant)
	{
		if (variant == INT_UIF_Variant.SECONDARY || variant == INT_UIF_Variant.GHOST)
			return 0;
		return INT_UIF_Color.WithAlpha(VariantHue(variant), 70);
	}

	// Legacy names used by older components.
	int VariantColor(int variant)
	{
		return VariantFill(variant);
	}

	int VariantHoverColor(int variant)
	{
		return VariantFillHover(variant);
	}

	int VariantTextColor(int variant)
	{
		return VariantText(variant);
	}

	int ToastColor(int kind)
	{
		if (kind == INT_UIF_ToastKind.SUCCESS)
			return Color(INT_UIF_ThemeKeys.TEAL);
		if (kind == INT_UIF_ToastKind.WARNING)
			return Color(INT_UIF_ThemeKeys.AMBER);
		if (kind == INT_UIF_ToastKind.ERROR)
			return Color(INT_UIF_ThemeKeys.RED);
		return Color(INT_UIF_ThemeKeys.BLUE);
	}
}

// Short facade for consumer code: INT_UIF.Color(INT_UIF_ThemeKeys.ACCENT).
class INT_UIF
{
	static int Color(string key)
	{
		return INT_UIF_ThemeManager.Get().Color(key);
	}

	static float Metric(string key, float fallback = 0)
	{
		return INT_UIF_ThemeManager.Get().Metric(key, fallback);
	}

	static float Px(float px1080)
	{
		return INT_UIF_Scale.Px(px1080);
	}

	// Palette colour at a given alpha (0..255): INT_UIF.Soft("teal", 31).
	static int Soft(string key, int alpha)
	{
		return INT_UIF_Color.WithAlpha(INT_UIF_ThemeManager.Get().Color(key), alpha);
	}
}
