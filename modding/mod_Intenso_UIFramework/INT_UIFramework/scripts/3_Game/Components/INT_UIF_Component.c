// ============================================================================
// INT_UIFramework - component base.
//
// A component = one framework .layout + the ScriptedWidgetEventHandler that
// drives it. Build(parent) creates the layout inside a container widget, puts
// this handler on the layout root (events of children bubble up to it), and
// applies the active theme. Events go out through ScriptInvokers.
//
// OWNERSHIP: a widget only keeps a WEAK reference to its handler, so the
// component must be held by a `ref` somewhere or it is collected and its
// widgets die with it (destructor Unlinks). Every factory therefore takes an
// INT_UIF_Bag: pass menu.GetBag(), a parent component's GetBag(), or null if
// you store the component in a `ref` field yourself.
// ============================================================================

class INT_UIF_Bag extends Managed
{
	protected ref array<ref INT_UIF_Component> m_Items;

	void INT_UIF_Bag()
	{
		m_Items = new array<ref INT_UIF_Component>();
	}

	INT_UIF_Component Add(INT_UIF_Component c)
	{
		if (c && m_Items.Find(c) < 0)
			m_Items.Insert(c);
		return c;
	}

	void Release(INT_UIF_Component c)
	{
		int idx = m_Items.Find(c);
		if (idx >= 0)
			m_Items.Remove(idx);
	}

	int Count()
	{
		return m_Items.Count();
	}

	void Clear()
	{
		m_Items.Clear();
	}

	void ApplyTheme()
	{
		foreach (INT_UIF_Component c : m_Items)
		{
			if (c)
				c.ApplyTheme();
		}
	}
}

class INT_UIF_Component extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected bool m_Enabled = true;
	protected bool m_ThemeHooked;
	protected string m_Tooltip;
	protected ref INT_UIF_Bag m_Bag;
	// Design-px size of a v2 component placed with BuildAt.
	protected float m_W;
	protected float m_H;

	void INT_UIF_Component()
	{
		m_Bag = new INT_UIF_Bag();
	}

	void ~INT_UIF_Component()
	{
		UnhookTheme();
		INT_UIF_Animator anim = INT_UIF_Animator.Peek();
		if (anim)
			anim.RemoveTicker(this);
		if (m_Root && GetGame())
			m_Root.Unlink();
	}

	// --- to override ---------------------------------------------------------
	// Layout file name relative to INT_UIFramework/gui/layouts/.
	string GetLayoutName()
	{
		return "";
	}

	// Name given to the root (v2 components share px_frame.layout); "" keeps it.
	string GetRootName()
	{
		return "";
	}

	// Find widgets, wire state. m_Root is valid here.
	void OnBuild()
	{
	}

	// Paint every widget from the active theme. Called on build and on every
	// theme switch.
	void ApplyTheme()
	{
	}

	// Per-frame update, only while registered with StartTicking().
	void OnTick(float dt)
	{
	}

	// --- lifecycle -----------------------------------------------------------
	Widget Build(Widget parent)
	{
		m_Root = INT_UIF_Widgets.Create(GetLayoutName(), parent);
		INT_UIF_Log.Debug("build " + Type().ToString() + " layout='" + GetLayoutName() + "' parent=" + INT_UIF_Widgets.NameOf(parent) + " -> " + INT_UIF_Widgets.NameOf(m_Root));
		if (!m_Root)
			return null;
		if (GetRootName() != "")
			m_Root.SetName(GetRootName());
		m_Root.SetHandler(this);
		OnBuild();
		ApplyTheme();
		HookTheme();
		return m_Root;
	}

	// v2: build at an exact design-px rect (1920x1080 canvas) inside parent.
	Widget BuildAt(Widget parent, float x, float y, float w, float h)
	{
		m_W = w;
		m_H = h;
		Widget r = Build(parent);
		if (r)
			INT_UIF_Draw.Place(r, x, y, w, h);
		return r;
	}

	float GetWidth()
	{
		return m_W;
	}

	float GetHeight()
	{
		return m_H;
	}

	void Destroy()
	{
		UnhookTheme();
		StopTicking();
		m_Bag.Clear();
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
	}

	protected void HookTheme()
	{
		if (m_ThemeHooked)
			return;
		INT_UIF_ThemeManager.Get().GetOnChanged().Insert(OnThemeChanged);
		m_ThemeHooked = true;
	}

	protected void UnhookTheme()
	{
		if (!m_ThemeHooked)
			return;
		m_ThemeHooked = false;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Peek();
		if (tm && tm.GetOnChanged())
			tm.GetOnChanged().Remove(OnThemeChanged);
	}

	void OnThemeChanged(INT_UIF_Theme theme)
	{
		if (m_Root)
			ApplyTheme();
	}

	protected void StartTicking()
	{
		INT_UIF_Animator.Get().AddTicker(this);
	}

	protected void StopTicking()
	{
		INT_UIF_Animator anim = INT_UIF_Animator.Peek();
		if (anim)
			anim.RemoveTicker(this);
	}

	// --- common API ----------------------------------------------------------
	Widget GetRoot()
	{
		return m_Root;
	}

	INT_UIF_Bag GetBag()
	{
		return m_Bag;
	}

	void Show(bool show)
	{
		if (m_Root)
			m_Root.Show(show);
	}

	bool IsVisible()
	{
		return m_Root && m_Root.IsVisible();
	}

	void SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
		ApplyTheme();
	}

	bool IsEnabled()
	{
		return m_Enabled;
	}

	// Proportional placement inside the parent (0..1).
	void SetPlacement(float x, float y, float w, float h)
	{
		if (!m_Root)
			return;
		m_Root.SetPos(x, y);
		m_Root.SetSize(w, h);
	}

	void SetTooltip(string text)
	{
		m_Tooltip = text;
	}

	protected void TooltipEnter()
	{
		if (m_Tooltip != "")
			INT_UIF_Tooltip.ShowFor(m_Root, m_Tooltip);
	}

	protected void TooltipLeave()
	{
		if (m_Tooltip != "")
			INT_UIF_Tooltip.HideTip();
	}

	// Theme shortcuts.
	protected int C(string key)
	{
		return INT_UIF_ThemeManager.Get().Color(key);
	}

	protected float AnimFast()
	{
		return INT_UIF_ThemeManager.Get().Metric(INT_UIF_ThemeKeys.ANIM_FAST, 0.12);
	}

	protected float AnimNormal()
	{
		return INT_UIF_ThemeManager.Get().Metric(INT_UIF_ThemeKeys.ANIM_NORMAL, 0.22);
	}
}
