// ============================================================================
// INT_UIFramework - input components (v2): Toggle, Checkbox, TextInput,
// Slider, Stepper. Geometry in design px (1920x1080 canvas), built with
// BuildAt on px_frame like every v2 component; interactive parts are px_hit
// targets (or the edit box) with this component as their handler.
// ============================================================================

// Switch with a label (+ optional sub line): track turns teal and glows when
// on, the knob springs across.
class INT_UIF_Toggle extends INT_UIF_Component
{
	static const float TRACK_W = 44;
	static const float TRACK_H = 24;
	static const float KNOB = 18;

	protected string m_Text;
	protected string m_Sub;
	protected bool m_On;
	protected bool m_Hover;
	protected float m_TrackX;
	protected float m_TrackY;
	protected ref ScriptInvoker m_OnChanged;
	protected ref INT_UIF_Surface m_Glow;
	protected ref INT_UIF_Surface m_Track;
	protected ImageWidget m_Knob;
	protected TextWidget m_Label;
	protected TextWidget m_SubLabel;
	protected ButtonWidget m_Hit;

	void INT_UIF_Toggle()
	{
		m_OnChanged = new ScriptInvoker();
	}

	// h 0 = 32 (one line) or 44 (with a sub line).
	static INT_UIF_Toggle Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, string text, bool on = false, string sub = "", float h = 0)
	{
		INT_UIF_Toggle t = new INT_UIF_Toggle();
		t.m_Text = text;
		t.m_Sub = sub;
		t.m_On = on;
		if (h <= 0)
		{
			h = 32;
			if (sub != "")
				h = 44;
		}
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
		return "IufToggle";
	}

	override void OnBuild()
	{
		m_TrackX = m_W - TRACK_W;
		m_TrackY = (m_H - TRACK_H) * 0.5;
		float labelW = m_TrackX - 14;
		if (m_Sub != "")
		{
			m_Label = INT_UIF_Draw.Text(m_Root, 0, m_H * 0.5 - 19, labelW, 20, m_Text, 14, 0);
			m_SubLabel = INT_UIF_Draw.Text(m_Root, 0, m_H * 0.5 + 1, labelW, 18, m_Sub, 12, 0);
		}
		else
		{
			m_Label = INT_UIF_Draw.Text(m_Root, 0, 0, labelW, m_H, m_Text, 14, 0);
		}
		m_Glow = INT_UIF_Surface.Glow(m_Root, m_TrackX - 14, m_TrackY - 12, TRACK_W + 28, TRACK_H + 24, 0);
		m_Track = INT_UIF_Surface.Pill(m_Root, m_TrackX, m_TrackY, TRACK_W, TRACK_H, 0, 0);
		m_Knob = INT_UIF_Draw.Image(m_Root, KnobX(m_On), m_TrackY + 3, KNOB, KNOB, "iuf_circle32", 0);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, m_W, m_H, this);
	}

	protected float KnobX(bool on)
	{
		if (on)
			return m_TrackX + TRACK_W - KNOB - 3;
		return m_TrackX + 3;
	}

	protected int TrackColor()
	{
		if (m_On)
		{
			if (m_Hover)
				return C(INT_UIF_ThemeKeys.ACCENT_HOVER);
			return C(INT_UIF_ThemeKeys.TEAL);
		}
		if (m_Hover)
			return ARGB(56, 255, 255, 255);
		return ARGB(36, 255, 255, 255);
	}

	protected int GlowColor()
	{
		if (m_On)
			return INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 64);
		return INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 0);
	}

	protected int KnobColor()
	{
		if (m_On)
			return ARGB(255, 255, 255, 255);
		return ARGB(255, 196, 202, 209);
	}

	override void ApplyTheme()
	{
		if (m_Track)
			m_Track.SetFill(TrackColor());
		if (m_Glow)
			m_Glow.SetFill(GlowColor());
		if (m_Knob)
			m_Knob.SetColor(KnobColor());
		if (m_Label)
			m_Label.SetColor(C(INT_UIF_ThemeKeys.TEXT));
		if (m_SubLabel)
			m_SubLabel.SetColor(C(INT_UIF_ThemeKeys.TEXT_FAINT));
		if (m_Root)
		{
			if (m_Enabled)
				m_Root.SetAlpha(1);
			else
				m_Root.SetAlpha(0.42);
		}
	}

	// (bool on, INT_UIF_Toggle toggle)
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	bool IsOn()
	{
		return m_On;
	}

	void SetOn(bool on, bool notify = false, bool animate = true)
	{
		m_On = on;
		if (m_Knob && m_Track)
		{
			if (animate)
			{
				float d = AnimNormal();
				INT_UIF_Animator.MoveX(m_Knob, INT_UIF_Draw.S(KnobX(on)), 0.3, INT_UIF_Ease.OUT_BACK);
				m_Track.FadeFill(TrackColor(), d);
				if (m_Glow)
					m_Glow.FadeFill(GlowColor(), d);
				INT_UIF_Animator.ColorTo(m_Knob, KnobColor(), d);
			}
			else
			{
				INT_UIF_Draw.Move(m_Knob, KnobX(on), m_TrackY + 3);
				ApplyTheme();
			}
		}
		if (notify)
		{
			INT_UIF_Toggle keep = this;
			m_OnChanged.Invoke(m_On, this);
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		if (!m_Enabled)
			return true;
		SetFocus(null);
		SetOn(!m_On, true, true);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = true;
		if (m_Enabled && m_Track)
			m_Track.FadeFill(TrackColor(), AnimFast());
		TooltipEnter();
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = false;
		if (m_Track)
			m_Track.FadeFill(TrackColor(), AnimNormal());
		TooltipLeave();
		return true;
	}
}

// Rounded check box with a label; the check fades in on a teal box.
class INT_UIF_Checkbox extends INT_UIF_Component
{
	static const float BOX = 20;

	protected string m_Text;
	protected bool m_Checked;
	protected bool m_Hover;
	protected float m_BoxY;
	protected ref ScriptInvoker m_OnChanged;
	protected ref INT_UIF_Surface m_Box;
	protected ImageWidget m_Check;
	protected TextWidget m_Label;
	protected ButtonWidget m_Hit;

	void INT_UIF_Checkbox()
	{
		m_OnChanged = new ScriptInvoker();
	}

	static INT_UIF_Checkbox Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, string text, bool checked = false, float h = 28)
	{
		INT_UIF_Checkbox c = new INT_UIF_Checkbox();
		c.m_Text = text;
		c.m_Checked = checked;
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
		return "IufCheckbox";
	}

	override void OnBuild()
	{
		m_BoxY = (m_H - BOX) * 0.5;
		m_Box = INT_UIF_Surface.Box(m_Root, 0, m_BoxY, BOX, BOX, 6, 0, 0);
		m_Check = INT_UIF_Draw.Icon(m_Root, 3, m_BoxY + 3, 14, "check", 0);
		m_Label = INT_UIF_Draw.Text(m_Root, 32, 0, m_W - 32, m_H, m_Text, 14, 0);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, m_W, m_H, this);
	}

	protected int BoxFill()
	{
		if (m_Checked)
		{
			if (m_Hover)
				return C(INT_UIF_ThemeKeys.ACCENT_HOVER);
			return C(INT_UIF_ThemeKeys.TEAL);
		}
		if (m_Hover)
			return C(INT_UIF_ThemeKeys.TILE_HOVER);
		return C(INT_UIF_ThemeKeys.TILE);
	}

	protected int BoxBorder()
	{
		if (m_Checked)
			return 0;
		if (m_Hover)
			return ARGB(80, 255, 255, 255);
		return ARGB(46, 255, 255, 255);
	}

	override void ApplyTheme()
	{
		if (m_Box)
		{
			m_Box.SetFill(BoxFill());
			m_Box.SetBorder(BoxBorder());
		}
		if (m_Check)
		{
			m_Check.SetColor(C(INT_UIF_ThemeKeys.TEXT_ON_ACCENT));
			m_Check.Show(m_Checked);
		}
		if (m_Label)
			m_Label.SetColor(C(INT_UIF_ThemeKeys.TEXT));
		if (m_Root)
		{
			if (m_Enabled)
				m_Root.SetAlpha(1);
			else
				m_Root.SetAlpha(0.42);
		}
	}

	// (bool checked, INT_UIF_Checkbox checkbox)
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	bool IsChecked()
	{
		return m_Checked;
	}

	void SetChecked(bool checked, bool notify = false, bool animate = true)
	{
		m_Checked = checked;
		if (m_Box && m_Check)
		{
			if (animate)
			{
				m_Box.FadeFill(BoxFill(), AnimFast());
				m_Box.FadeBorder(BoxBorder(), AnimFast());
				if (checked)
					INT_UIF_Animator.FadeIn(m_Check, 0.18);
				else
					INT_UIF_Animator.FadeOut(m_Check, 0.12, true);
			}
			else
			{
				ApplyTheme();
			}
		}
		if (notify)
		{
			INT_UIF_Checkbox keep = this;
			m_OnChanged.Invoke(m_Checked, this);
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		if (!m_Enabled)
			return true;
		SetFocus(null);
		SetChecked(!m_Checked, true, true);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = true;
		if (m_Enabled && m_Box)
		{
			m_Box.FadeFill(BoxFill(), AnimFast());
			m_Box.FadeBorder(BoxBorder(), AnimFast());
		}
		TooltipEnter();
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = false;
		if (m_Box)
		{
			m_Box.FadeFill(BoxFill(), AnimNormal());
			m_Box.FadeBorder(BoxBorder(), AnimNormal());
		}
		TooltipLeave();
		return true;
	}
}

// Text field: tile with hairline border; focus lights the border teal with a
// soft glow. Optional leading icon and a key-cap hint on the right.
class INT_UIF_TextInput extends INT_UIF_Component
{
	protected string m_PlaceholderText;
	protected string m_IconName;
	protected string m_HintKey;
	protected int m_MaxLength;
	protected bool m_Focused;
	protected bool m_Hover;
	protected float m_TextX;
	protected ref ScriptInvoker m_OnChanged;
	protected ref ScriptInvoker m_OnSubmit;
	protected ref INT_UIF_Surface m_Glow;
	protected ref INT_UIF_Surface m_Body;
	protected ImageWidget m_IconW;
	protected TextWidget m_Placeholder;
	protected EditBoxWidget m_Edit;
	protected ButtonWidget m_BodyHit;

	void INT_UIF_TextInput()
	{
		m_OnChanged = new ScriptInvoker();
		m_OnSubmit = new ScriptInvoker();
	}

	static INT_UIF_TextInput Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float h = 40, string placeholder = "", int maxLength = 0, string icon = "", string hintKey = "")
	{
		INT_UIF_TextInput t = new INT_UIF_TextInput();
		t.m_PlaceholderText = placeholder;
		t.m_MaxLength = maxLength;
		t.m_IconName = icon;
		t.m_HintKey = hintKey;
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
		return "IufTextInput";
	}

	override void OnBuild()
	{
		m_Glow = INT_UIF_Surface.Glow(m_Root, -14, -12, m_W + 28, m_H + 24, 0);
		m_Body = INT_UIF_Surface.Box(m_Root, 0, 0, m_W, m_H, 8, 0, 0);
		// Clicking anywhere on the field focuses the edit box (it is shorter
		// than the field).
		m_BodyHit = INT_UIF_Draw.Hit(m_Root, 0, 0, m_W, m_H, this);
		m_TextX = 14;
		if (m_IconName != "")
		{
			m_IconW = INT_UIF_Draw.Icon(m_Root, 14, (m_H - 16) * 0.5, 16, m_IconName, 0);
			m_TextX = 40;
		}
		float right = 14;
		if (m_HintKey != "")
		{
			float kw = INT_UIF_Draw.TextWidth(m_HintKey, 11) + 16;
			if (kw < 26)
				kw = 26;
			Widget cap = INT_UIF_Draw.Frame(m_Root, m_W - 10 - kw, (m_H - 22) * 0.5, kw, 22, "Hint");
			INT_UIF_Surface.Box(cap, 0, 0, kw, 22, 4, ARGB(16, 255, 255, 255), ARGB(32, 255, 255, 255));
			INT_UIF_Draw.Text(cap, 0, 0, kw, 22, m_HintKey, 11, C(INT_UIF_ThemeKeys.TEXT_MUTED), INT_UIF_Align.CENTER);
			right = kw + 20;
		}
		m_Placeholder = INT_UIF_Draw.Text(m_Root, m_TextX, 0, m_W - m_TextX - right, m_H, m_PlaceholderText, 14, 0);
		float eh = 24;
		m_Edit = INT_UIF_Draw.Edit(m_Root, m_TextX - 2, (m_H - eh) * 0.5, m_W - m_TextX - right + 2, eh, 14);
		if (m_Edit)
			m_Edit.SetHandler(this);
	}

	protected int BorderColor()
	{
		if (m_Focused)
			return INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 160);
		if (m_Hover)
			return ARGB(50, 255, 255, 255);
		return ARGB(26, 255, 255, 255);
	}

	protected int GlowColor()
	{
		if (m_Focused)
			return INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 46);
		return INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), 0);
	}

	protected void Paint(bool animate)
	{
		if (!m_Body)
			return;
		if (animate)
		{
			m_Body.FadeBorder(BorderColor(), AnimFast());
			m_Glow.FadeFill(GlowColor(), AnimNormal());
		}
		else
		{
			m_Body.SetBorder(BorderColor());
			m_Glow.SetFill(GlowColor());
		}
		if (m_IconW)
		{
			if (m_Focused)
				m_IconW.SetColor(C(INT_UIF_ThemeKeys.TEAL));
			else
				m_IconW.SetColor(C(INT_UIF_ThemeKeys.TEXT_FAINT));
		}
	}

	override void ApplyTheme()
	{
		if (m_Body)
			m_Body.SetFill(C(INT_UIF_ThemeKeys.TILE));
		Paint(false);
		if (m_Placeholder)
			m_Placeholder.SetColor(C(INT_UIF_ThemeKeys.TEXT_FAINT));
		if (m_Edit)
		{
			m_Edit.SetColor(ARGB(255, 255, 255, 255));
			m_Edit.SetTextColor(C(INT_UIF_ThemeKeys.TEXT));
			m_Edit.Enable(m_Enabled);
		}
		if (m_Root)
		{
			if (m_Enabled)
				m_Root.SetAlpha(1);
			else
				m_Root.SetAlpha(0.42);
		}
	}

	// (string text, INT_UIF_TextInput input)
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	// (string text, INT_UIF_TextInput input) - Enter pressed
	ScriptInvoker OnSubmit()
	{
		return m_OnSubmit;
	}

	string GetText()
	{
		if (!m_Edit)
			return "";
		return m_Edit.GetText();
	}

	void SetText(string text)
	{
		if (m_MaxLength > 0 && text.LengthUtf8() > m_MaxLength)
			text = text.SubstringUtf8(0, m_MaxLength);
		if (m_Edit)
			m_Edit.SetText(text);
		RefreshPlaceholder();
	}

	void SetPlaceholder(string text)
	{
		m_PlaceholderText = text;
		if (m_Placeholder)
			m_Placeholder.SetText(text);
	}

	void Focus()
	{
		if (m_Edit)
			SetFocus(m_Edit);
	}

	protected void RefreshPlaceholder()
	{
		if (m_Placeholder)
			m_Placeholder.Show(GetText() == "");
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_BodyHit)
			return false;
		if (m_Enabled)
			Focus();
		return true;
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w != m_Edit)
			return false;
		string text = m_Edit.GetText();
		// Character counts, not bytes: Cyrillic is 2 bytes per letter.
		if (m_MaxLength > 0 && text.LengthUtf8() > m_MaxLength)
		{
			text = text.SubstringUtf8(0, m_MaxLength);
			m_Edit.SetText(text);
		}
		RefreshPlaceholder();
		INT_UIF_TextInput keep = this;
		m_OnChanged.Invoke(text, this);
		return true;
	}

	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if (w != m_Edit)
			return false;
		if (key == KeyCode.KC_RETURN || key == KeyCode.KC_NUMPADENTER)
		{
			INT_UIF_TextInput keep = this;
			m_OnSubmit.Invoke(GetText(), this);
			return true;
		}
		return false;
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (w != m_Edit)
			return false;
		m_Focused = true;
		Paint(true);
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (w != m_Edit)
			return false;
		m_Focused = false;
		Paint(true);
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_Edit && w != m_BodyHit)
			return false;
		m_Hover = true;
		Paint(true);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_Edit && w != m_BodyHit)
			return false;
		// Moving between the field's own edit box and its body is not leaving.
		if (enterW == m_Edit || enterW == m_BodyHit)
			return false;
		m_Hover = false;
		Paint(true);
		return false;
	}
}

// Range slider: thin track, teal fill, white knob with a glow while dragged.
// Drag, click or wheel; OnChanged fires while dragging (isFinal false) and
// once more on release (isFinal true).
class INT_UIF_Slider extends INT_UIF_Component
{
	static const float KNOB = 18;

	protected string m_LabelText;
	protected float m_Min;
	protected float m_Max;
	protected float m_Value;
	protected float m_Step;
	protected int m_Decimals;
	protected string m_Suffix;
	protected bool m_Dragging;
	protected bool m_Hover;
	protected float m_TrackY;
	protected ref ScriptInvoker m_OnChanged;
	protected ref INT_UIF_Surface m_Track;
	protected ref INT_UIF_Surface m_Fill;
	protected ImageWidget m_KnobGlow;
	protected ImageWidget m_Knob;
	protected TextWidget m_Label;
	protected TextWidget m_ValueText;
	protected ButtonWidget m_Hit;

	void INT_UIF_Slider()
	{
		m_OnChanged = new ScriptInvoker();
	}

	// h 0 = 24 (bare) or 50 (with a label row).
	static INT_UIF_Slider Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float min, float max, float value, float step = 0, string label = "", float h = 0)
	{
		INT_UIF_Slider s = new INT_UIF_Slider();
		s.m_LabelText = label;
		s.m_Min = min;
		s.m_Max = max;
		if (s.m_Max <= s.m_Min)
			s.m_Max = s.m_Min + 1;
		s.m_Step = step;
		s.m_Value = s.Snap(value);
		if (h <= 0)
		{
			h = 24;
			if (label != "")
				h = 50;
		}
		if (bag)
			bag.Add(s);
		s.BuildAt(parent, x, y, w, h);
		return s;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufSlider";
	}

	override void OnBuild()
	{
		if (m_LabelText != "")
		{
			m_Label = INT_UIF_Draw.Text(m_Root, 0, 0, m_W * 0.65, 20, m_LabelText, 13, 0);
			m_ValueText = INT_UIF_Draw.Text(m_Root, m_W * 0.35, 0, m_W * 0.65, 20, "", 14, 0, INT_UIF_Align.RIGHT);
			if (m_ValueText)
				m_ValueText.SetBold(true);
		}
		m_TrackY = m_H - 12;
		m_Track = INT_UIF_Surface.Pill(m_Root, 0, m_TrackY - 3, m_W, 6, 0, 0);
		m_Fill = INT_UIF_Surface.Pill(m_Root, 0, m_TrackY - 3, KNOB, 6, 0, 0);
		m_KnobGlow = INT_UIF_Draw.Image(m_Root, 0, m_TrackY - 22, 44, 44, "iuf_radial", 0);
		m_Knob = INT_UIF_Draw.Image(m_Root, 0, m_TrackY - KNOB * 0.5, KNOB, KNOB, "iuf_circle32", 0);
		float hy = m_TrackY - 14;
		if (hy < 0)
			hy = 0;
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, hy, m_W, m_H - hy, this);
		PlaceKnob(false);
	}

	override void ApplyTheme()
	{
		if (m_Track)
			m_Track.SetFill(C(INT_UIF_ThemeKeys.TRACK));
		if (m_Fill)
			m_Fill.SetFill(C(INT_UIF_ThemeKeys.TEAL));
		if (m_Knob)
			m_Knob.SetColor(ARGB(255, 255, 255, 255));
		if (m_KnobGlow)
			m_KnobGlow.SetColor(GlowColor());
		if (m_Label)
			m_Label.SetColor(C(INT_UIF_ThemeKeys.TEXT_MUTED));
		if (m_ValueText)
			m_ValueText.SetColor(C(INT_UIF_ThemeKeys.TEAL));
		if (m_Root)
		{
			if (m_Enabled)
				m_Root.SetAlpha(1);
			else
				m_Root.SetAlpha(0.42);
		}
	}

	protected int GlowColor()
	{
		int a = 0;
		if (m_Dragging)
			a = 120;
		else if (m_Hover)
			a = 60;
		return INT_UIF_Color.WithAlpha(C(INT_UIF_ThemeKeys.TEAL), a);
	}

	// (float value, INT_UIF_Slider slider, bool isFinal)
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	float GetValue()
	{
		return m_Value;
	}

	void SetRange(float min, float max, float step = 0)
	{
		m_Min = min;
		m_Max = max;
		if (m_Max <= m_Min)
			m_Max = m_Min + 1;
		m_Step = step;
		SetValue(m_Value, false, false);
	}

	// decimals 0 = whole numbers; suffix e.g. " m", "%".
	void SetFormat(int decimals, string suffix = "")
	{
		m_Decimals = decimals;
		m_Suffix = suffix;
		RefreshText();
	}

	void SetValue(float value, bool notify = false, bool animate = true)
	{
		m_Value = Snap(value);
		PlaceKnob(animate);
		if (notify)
		{
			INT_UIF_Slider keep = this;
			m_OnChanged.Invoke(m_Value, this, true);
		}
	}

	float Snap(float v)
	{
		v = Math.Clamp(v, m_Min, m_Max);
		if (m_Step > 0)
			v = m_Min + Math.Round((v - m_Min) / m_Step) * m_Step;
		return Math.Clamp(v, m_Min, m_Max);
	}

	protected float Frac()
	{
		return (m_Value - m_Min) / (m_Max - m_Min);
	}

	string FormatValue(float v)
	{
		string s;
		if (m_Decimals <= 0)
		{
			int iv = (int)Math.Round(v);
			s = iv.ToString();
		}
		else
		{
			float p = Math.Pow(10, m_Decimals);
			float rv = Math.Round(v * p) / p;
			s = rv.ToString();
		}
		return s + m_Suffix;
	}

	protected void RefreshText()
	{
		if (m_ValueText)
			m_ValueText.SetText(FormatValue(m_Value));
	}

	protected void PlaceKnob(bool animate)
	{
		if (!m_Knob)
			return;
		float kx = Frac() * (m_W - KNOB);
		float fw = kx + KNOB * 0.5;
		if (fw < 6)
			fw = 6;
		if (animate)
		{
			INT_UIF_Animator.MoveX(m_Knob, INT_UIF_Draw.S(kx), 0.2, INT_UIF_Ease.OUT_CUBIC);
			INT_UIF_Animator.MoveX(m_KnobGlow, INT_UIF_Draw.S(kx - 13), 0.2, INT_UIF_Ease.OUT_CUBIC);
			INT_UIF_Animator.ResizeW(m_Fill.GetRoot(), INT_UIF_Draw.S(fw), 0.2, INT_UIF_Ease.OUT_CUBIC);
		}
		else
		{
			INT_UIF_Draw.Move(m_Knob, kx, m_TrackY - KNOB * 0.5);
			INT_UIF_Draw.Move(m_KnobGlow, kx - 13, m_TrackY - 22);
			INT_UIF_Draw.Resize(m_Fill.GetRoot(), fw, 6);
		}
		RefreshText();
	}

	protected void UpdateFromMouse()
	{
		if (!m_Track)
			return;
		int mx;
		int my;
		GetMousePos(mx, my);
		Widget track = m_Track.GetRoot();
		if (!track)
			return;
		float sx;
		float sy;
		float sw;
		float sh;
		track.GetScreenPos(sx, sy);
		track.GetScreenSize(sw, sh);
		float kp = INT_UIF_Draw.S(KNOB);
		if (sw <= kp)
			return;
		float k = Math.Clamp((mx - sx - kp * 0.5) / (sw - kp), 0, 1);
		float v = Snap(m_Min + k * (m_Max - m_Min));
		if (v == m_Value)
			return;
		m_Value = v;
		PlaceKnob(false);
		INT_UIF_Slider keep = this;
		m_OnChanged.Invoke(m_Value, this, false);
	}

	override void OnTick(float dt)
	{
		if (!m_Dragging)
		{
			StopTicking();
			return;
		}
		bool held = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) != 0;
		if (held)
		{
			UpdateFromMouse();
			return;
		}
		m_Dragging = false;
		StopTicking();
		if (m_KnobGlow)
			INT_UIF_Animator.ColorTo(m_KnobGlow, GlowColor(), AnimNormal());
		INT_UIF_Slider keep = this;
		m_OnChanged.Invoke(m_Value, this, true);
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT || !m_Enabled)
			return false;
		m_Dragging = true;
		if (m_KnobGlow)
			INT_UIF_Animator.ColorTo(m_KnobGlow, GlowColor(), AnimFast());
		UpdateFromMouse();
		StartTicking();
		return true;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (w != m_Hit || !m_Enabled)
			return false;
		float st = m_Step;
		if (st <= 0)
			st = (m_Max - m_Min) / 20.0;
		SetValue(m_Value + wheel * st, true, true);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = true;
		if (m_KnobGlow)
			INT_UIF_Animator.ColorTo(m_KnobGlow, GlowColor(), AnimFast());
		TooltipEnter();
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = false;
		if (m_KnobGlow)
			INT_UIF_Animator.ColorTo(m_KnobGlow, GlowColor(), AnimNormal());
		TooltipLeave();
		return true;
	}
}

// [ - ]  value  [ + ]  — numeric stepper (settings rows in the reference look).
class INT_UIF_Stepper extends INT_UIF_Component
{
	protected float m_Min;
	protected float m_Max;
	protected float m_Value;
	protected float m_Step;
	protected string m_Suffix;
	protected ref ScriptInvoker m_OnChanged;
	protected INT_UIF_IconButton m_Minus;
	protected INT_UIF_IconButton m_Plus;
	protected ref INT_UIF_Surface m_Box;
	protected TextWidget m_ValueText;

	void INT_UIF_Stepper()
	{
		m_OnChanged = new ScriptInvoker();
	}

	static INT_UIF_Stepper Create(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float min, float max, float value, float step = 1, string suffix = "", float h = 36)
	{
		INT_UIF_Stepper s = new INT_UIF_Stepper();
		s.m_Min = min;
		s.m_Max = max;
		s.m_Step = step;
		if (s.m_Step <= 0)
			s.m_Step = 1;
		s.m_Suffix = suffix;
		s.m_Value = Math.Clamp(value, min, max);
		if (bag)
			bag.Add(s);
		s.BuildAt(parent, x, y, w, h);
		return s;
	}

	override string GetLayoutName()
	{
		return "px_frame.layout";
	}

	override string GetRootName()
	{
		return "IufStepper";
	}

	override void OnBuild()
	{
		float bw = m_W - 2 * m_H - 16;
		m_Minus = INT_UIF_IconButton.Create(m_Bag, m_Root, 0, 0, m_H, "minus", INT_UIF_Variant.SECONDARY);
		m_Box = INT_UIF_Surface.Box(m_Root, m_H + 8, 0, bw, m_H, 8, 0, 0);
		m_ValueText = INT_UIF_Draw.Text(m_Root, m_H + 8, 0, bw, m_H, "", 14, 0, INT_UIF_Align.CENTER);
		if (m_ValueText)
			m_ValueText.SetBold(true);
		m_Plus = INT_UIF_IconButton.Create(m_Bag, m_Root, m_W - m_H, 0, m_H, "plus", INT_UIF_Variant.SECONDARY);
		if (m_Minus)
			m_Minus.OnClick().Insert(OnMinus);
		if (m_Plus)
			m_Plus.OnClick().Insert(OnPlus);
		Refresh(false);
	}

	override void ApplyTheme()
	{
		if (m_Box)
		{
			m_Box.SetFill(C(INT_UIF_ThemeKeys.TILE));
			m_Box.SetBorder(ARGB(24, 255, 255, 255));
		}
		if (m_ValueText)
			m_ValueText.SetColor(C(INT_UIF_ThemeKeys.TEXT));
	}

	// (float value, INT_UIF_Stepper stepper)
	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	float GetValue()
	{
		return m_Value;
	}

	void SetValue(float value, bool notify = false)
	{
		float v = Math.Clamp(value, m_Min, m_Max);
		bool changed = v != m_Value;
		m_Value = v;
		Refresh(changed);
		if (notify && changed)
		{
			INT_UIF_Stepper keep = this;
			m_OnChanged.Invoke(m_Value, this);
		}
	}

	protected void Refresh(bool pop)
	{
		if (m_ValueText)
		{
			int iv = (int)Math.Round(m_Value);
			string s = iv.ToString();
			if (Math.AbsFloat(m_Value - iv) > 0.001)
				s = m_Value.ToString();
			m_ValueText.SetText(s + m_Suffix);
			if (pop)
			{
				m_ValueText.SetAlpha(0.35);
				INT_UIF_Animator.Fade(m_ValueText, 1, 0.22, INT_UIF_Ease.OUT_CUBIC);
			}
		}
		if (m_Minus)
			m_Minus.SetEnabled(m_Value > m_Min);
		if (m_Plus)
			m_Plus.SetEnabled(m_Value < m_Max);
	}

	void OnMinus(INT_UIF_IconButton button)
	{
		SetValue(m_Value - m_Step, true);
	}

	void OnPlus(INT_UIF_IconButton button)
	{
		SetValue(m_Value + m_Step, true);
	}
}
