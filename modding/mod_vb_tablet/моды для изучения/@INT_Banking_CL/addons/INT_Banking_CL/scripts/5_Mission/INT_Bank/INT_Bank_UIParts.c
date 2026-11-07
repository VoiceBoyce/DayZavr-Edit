// INT Banking - interface building blocks on INT_UIFramework:
//   INT_Bank_UIButton   framework button + click tick, a short flash and a ~97 % press scale;
//   INT_Bank_UIKeypad   ATM keypad 1-9 / C / 0 / back: beep, press scale, flash; physical keys echo it;
//   INT_Bank_UIAmount   ATM display: caption, big digits "15 000", currency, blinking caret, hint line;
//   INT_Bank_UICard     the player's bank card: brass plate, guilloche, chip, number, holder, sheen sweep;
//   INT_Bank_UINav      vertical ATM side menu with a sliding brass indicator;
//   INT_Bank_UIDialog   confirm dialog with an icon per action and a 60 % dim.
// Sizes are design px (1920x1080 canvas), see INT_UIFramework readme section 4.

class INT_Bank_UIMeasure
{
	// Width of the text a TextWidget shows, in design px (estimate when not laid out yet).
	static float TextW(TextWidget t, string text, float size)
	{
		float est = INT_UIF_Draw.TextWidth(text, size) * 1.15;
		if (!t)
			return est;
		t.Update();
		int mw;
		int mh;
		t.GetTextSize(mw, mh);
		if (mw <= 0)
			return est;
		float factor = INT_UIF_Scale.Factor();
		if (factor <= 0)
			factor = 1;
		return mw / factor;
	}
}

// ----------------------------------------------------------------------------------------------------
class INT_Bank_UIButton extends INT_UIF_Button
{
	protected ref INT_UIF_Surface m_Flash;
	protected bool m_InkLabel;
	protected int m_Sound = INT_Bank_Sfx.CLICK;

	static INT_Bank_UIButton CreateFx(INT_UIF_Bag bag, Widget parent, float x, float y, float w, float h, string text, int variant = INT_UIF_Variant.SECONDARY, string icon = "")
	{
		INT_Bank_UIButton b = new INT_Bank_UIButton();
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

	void SetSound(int kind)
	{
		m_Sound = kind;
	}

	void SetInkLabel(bool ink)
	{
		m_InkLabel = ink;
		ApplyTheme();
	}

	override protected void PressTo(bool down)
	{
		super.PressTo(down);
		if (!m_Body)
			return;
		Widget br = m_Body.GetRoot();
		if (!br)
			return;
		if (down)
			INT_UIF_Draw.Place(br, m_W * 0.015, m_H * 0.03, m_W * 0.97, m_H * 0.94);
		else
			INT_UIF_Draw.Place(br, 0, 0, m_W, m_H);
	}

	// Disabled look by colour (SetAlpha on a frame root does not dim its children).
	override void ApplyTheme()
	{
		super.ApplyTheme();
		if (m_Enabled)
		{
			if (m_InkLabel && m_Label)
				m_Label.SetColor(ARGB(255, 24, 19, 12));
			return;
		}
		if (m_Body)
		{
			m_Body.SetFill(ARGB(150, 46, 40, 33));
			m_Body.SetBorder(ARGB(40, 255, 240, 215));
		}
		if (m_Sheen)
			m_Sheen.SetColor(ARGB(0, 255, 255, 255));
		if (m_Label)
			m_Label.SetColor(ARGB(200, 116, 105, 91));
	}

	void Flash()
	{
		if (!m_Root)
			return;
		if (!m_Flash)
			m_Flash = INT_UIF_Surface.Box(m_Root, 0, 0, m_W, m_H, 8, 0, 0);
		if (!m_Flash)
			return;
		m_Flash.SetFill(ARGB(84, 255, 255, 255));
		m_Flash.FadeFill(ARGB(0, 255, 255, 255), 0.3);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_Hit && button == MouseState.LEFT && m_Enabled)
		{
			INT_Bank_UISfx.Play(m_Sound);
			Flash();
		}
		return super.OnClick(w, x, y, button);
	}
}

class INT_Bank_UIIconButton extends INT_UIF_IconButton
{
	static INT_Bank_UIIconButton CreateFx(INT_UIF_Bag bag, Widget parent, float x, float y, float size, string icon, int variant = INT_UIF_Variant.SECONDARY, string tooltip = "")
	{
		INT_Bank_UIIconButton b = new INT_Bank_UIIconButton();
		b.m_IconName = icon;
		b.m_Variant = variant;
		b.SetTooltip(tooltip);
		if (bag)
			bag.Add(b);
		b.BuildAt(parent, x, y, size, size);
		return b;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_Hit && button == MouseState.LEFT && m_Enabled)
			INT_Bank_UISfx.Play(INT_Bank_Sfx.CLICK);
		return super.OnClick(w, x, y, button);
	}
}

// ----------------------------------------------------------------------------------------------------
// Keypad
// ----------------------------------------------------------------------------------------------------
class INT_Bank_UIKeyCell
{
	string m_Key;
	float m_X;
	float m_Y;
	float m_W;
	float m_H;
	Widget m_Root;
	ref INT_UIF_Surface m_Body;
	ref INT_UIF_Surface m_Flash;
	Widget m_Label;
	ButtonWidget m_Hit;
	bool m_Hover;
	float m_PressLeft;
}

class INT_Bank_UIKeypad extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ref array<ref INT_Bank_UIKeyCell> m_Cells;
	protected ref ScriptInvoker m_OnKey;		// (string key)  "0".."9", "C", "B" (back)
	protected bool m_Enabled = true;

	void INT_Bank_UIKeypad()
	{
		m_Cells = new array<ref INT_Bank_UIKeyCell>();
		m_OnKey = new ScriptInvoker();
	}

	ScriptInvoker OnKey()
	{
		return m_OnKey;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	void Build(Widget parent, float x, float y, float w, float h)
	{
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "BankKeypad");
		float gap = 10;
		float kw = (w - gap * 2) / 3;
		float kh = (h - gap * 3) / 4;
		array<string> keys = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "C", "0", "B"};
		for (int i = 0; i < 12; i++)
		{
			int col = i % 3;
			int row = i / 3;
			float kx = col * (kw + gap);
			float ky = row * (kh + gap);
			AddCell(keys.Get(i), kx, ky, kw, kh);
		}
	}

	protected void AddCell(string key, float x, float y, float w, float h)
	{
		INT_Bank_UIKeyCell c = new INT_Bank_UIKeyCell();
		c.m_Key = key;
		c.m_X = x;
		c.m_Y = y;
		c.m_W = w;
		c.m_H = h;
		c.m_Root = INT_UIF_Draw.Frame(m_Root, x, y, w, h, "Key" + key);
		int fill = ARGB(236, 36, 30, 23);
		int border = ARGB(34, 255, 236, 200);
		if (key == "C")
			fill = ARGB(220, 70, 34, 26);
		if (key == "B")
			fill = ARGB(220, 58, 47, 30);
		c.m_Body = INT_UIF_Surface.Box(c.m_Root, 0, 0, w, h, 10, fill, border);
		int sheen = ARGB(16, 255, 240, 215);
		INT_UIF_Draw.Image(c.m_Root, 6, 2, w - 12, h * 0.4, "iuf_grad_v", sheen);
		int ink = INT_Bank_UILook.Ink();
		if (key == "C")
		{
			c.m_Label = INT_UIF_Draw.Text(c.m_Root, 0, 0, w, h, "C", 22, ARGB(255, 240, 170, 150), INT_UIF_Align.CENTER, true);
		}
		else if (key == "B")
		{
			int backC = INT_Bank_UILook.BrassLight();
			c.m_Label = INT_UIF_Draw.Icon(c.m_Root, (w - 26) * 0.5, (h - 26) * 0.5, 26, "chevron_left", backC);
		}
		else
		{
			c.m_Label = INT_UIF_Draw.Text(c.m_Root, 0, 0, w, h, key, 26, ink, INT_UIF_Align.CENTER, true);
		}
		c.m_Flash = INT_UIF_Surface.Box(c.m_Root, 0, 0, w, h, 10, ARGB(0, 255, 255, 255), 0);
		c.m_Hit = INT_UIF_Draw.Hit(c.m_Root, 0, 0, w, h, this);
		m_Cells.Insert(c);
	}

	void SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
		if (m_Root)
		{
			if (enabled)
				m_Root.SetAlpha(1);
			else
				m_Root.SetAlpha(0.45);
		}
	}

	protected INT_Bank_UIKeyCell CellOf(Widget w)
	{
		foreach (INT_Bank_UIKeyCell c : m_Cells)
		{
			if (c.m_Hit == w)
				return c;
		}
		return null;
	}

	protected INT_Bank_UIKeyCell CellByKey(string key)
	{
		foreach (INT_Bank_UIKeyCell c : m_Cells)
		{
			if (c.m_Key == key)
				return c;
		}
		return null;
	}

	// Visual + sound of a press (also used for physical keyboard keys).
	void PressVisual(string key)
	{
		INT_Bank_UIKeyCell c = CellByKey(key);
		if (!c)
			return;
		INT_Bank_UISfx.Play(INT_Bank_Sfx.KEY);
		if (c.m_Flash)
		{
			c.m_Flash.SetFill(ARGB(90, 255, 226, 170));
			c.m_Flash.FadeFill(ARGB(0, 255, 226, 170), 0.28);
		}
		Widget body = c.m_Body.GetRoot();
		if (body)
		{
			INT_UIF_Draw.Place(body, c.m_W * 0.03, c.m_H * 0.05, c.m_W * 0.94, c.m_H * 0.9);
			c.m_PressLeft = 0.09;
		}
	}

	// From the window's Update: pressed keys spring back.
	void Tick(float dt)
	{
		foreach (INT_Bank_UIKeyCell c : m_Cells)
		{
			if (c.m_PressLeft <= 0)
				continue;
			c.m_PressLeft = c.m_PressLeft - dt;
			if (c.m_PressLeft > 0 || !c.m_Body)
				continue;
			Widget body = c.m_Body.GetRoot();
			if (body)
				INT_UIF_Draw.Place(body, 0, 0, c.m_W, c.m_H);
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		INT_Bank_UIKeyCell c = CellOf(w);
		if (!c)
			return false;
		if (!m_Enabled || button != MouseState.LEFT)
			return true;
		PressVisual(c.m_Key);
		string key = c.m_Key;
		m_OnKey.Invoke(key);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		INT_Bank_UIKeyCell c = CellOf(w);
		if (!c)
			return false;
		c.m_Hover = true;
		c.m_Body.FadeBorder(ARGB(150, 216, 174, 98), 0.12);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		INT_Bank_UIKeyCell c = CellOf(w);
		if (!c)
			return false;
		c.m_Hover = false;
		c.m_Body.FadeBorder(ARGB(34, 255, 236, 200), 0.18);
		return false;
	}
}

// ----------------------------------------------------------------------------------------------------
// Amount display
// ----------------------------------------------------------------------------------------------------
class INT_Bank_UIAmount
{
	protected Widget m_Root;
	protected ref INT_UIF_Surface m_Box;
	protected ref INT_UIF_Surface m_Glow;
	protected TextWidget m_Caption;
	protected TextWidget m_Value;
	protected TextWidget m_Cur;
	protected ImageWidget m_Caret;
	protected TextWidget m_Hint;
	protected string m_Digits = "";
	protected float m_W;
	protected float m_H;
	protected float m_ValueX = 22;
	protected bool m_Active;

	Widget GetRoot()
	{
		return m_Root;
	}

	void Build(Widget parent, float x, float y, float w, float h, string caption)
	{
		m_W = w;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "BankAmount");
		m_Glow = INT_UIF_Surface.Glow(m_Root, -14, -12, w + 28, h + 24, ARGB(0, 216, 174, 98));
		m_Box = INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 12, ARGB(245, 10, 9, 7), ARGB(60, 216, 174, 98));
		// LCD feel: a faint top band
		INT_UIF_Draw.Image(m_Root, 8, 2, w - 16, h * 0.4, "iuf_grad_v", ARGB(22, 255, 236, 200));
		m_Caption = INT_UIF_Draw.Text(m_Root, m_ValueX, 12, w - 44, 18, caption, 11, INT_Bank_UILook.Faint());
		if (m_Caption)
			m_Caption.SetBold(true);
		m_Value = INT_UIF_Draw.Text(m_Root, m_ValueX, 30, w - 150, 58, "0", 46, INT_Bank_UILook.Ink(), INT_UIF_Align.LEFT, true);
		m_Cur = INT_UIF_Draw.Text(m_Root, m_ValueX + 40, 52, 120, 28, "#STR_INT_BANK_CUR", 18, INT_Bank_UILook.Brass());
		m_Caret = INT_UIF_Draw.Line(m_Root, m_ValueX + 30, 38, 3, 42, INT_Bank_UILook.Brass());
		if (m_Caret)
			INT_UIF_Animator.Pulse(m_Caret, 0.0, 1.0, 1.0);
		m_Hint = INT_UIF_Draw.Text(m_Root, m_ValueX, h - 30, w - 44, 18, "", 12, INT_Bank_UILook.Muted());
		Refresh();
	}

	void SetActive(bool active)
	{
		m_Active = active;
		if (m_Caret)
			m_Caret.Show(active);
		if (m_Glow)
		{
			if (active)
				m_Glow.FadeFill(ARGB(36, 216, 174, 98), 0.25);
			else
				m_Glow.FadeFill(ARGB(0, 216, 174, 98), 0.25);
		}
	}

	int Get()
	{
		if (m_Digits == "")
			return 0;
		return m_Digits.ToInt();
	}

	string Digits()
	{
		return m_Digits;
	}

	void Set(int value)
	{
		if (value <= 0)
			m_Digits = "";
		else
			m_Digits = value.ToString();
		Refresh();
	}

	// false = no room for another digit (9 digits)
	bool Append(string d)
	{
		if (m_Digits == "" && d == "0")
		{
			Refresh();
			return true;
		}
		if (m_Digits.Length() >= INT_BANK_AMOUNT_DIGITS)
		{
			Shake();
			return false;
		}
		m_Digits = m_Digits + d;
		Refresh();
		return true;
	}

	void Back()
	{
		int n = m_Digits.Length();
		if (n > 0)
			m_Digits = m_Digits.Substring(0, n - 1);
		Refresh();
	}

	void Clear()
	{
		m_Digits = "";
		Refresh();
	}

	void SetHint(string text, int color)
	{
		if (!m_Hint)
			return;
		m_Hint.SetText(text);
		m_Hint.SetColor(color);
	}

	void Refresh()
	{
		if (!m_Value)
			return;
		string shown = "0";
		int col = INT_Bank_UILook.Faint();
		if (m_Digits != "")
		{
			int v = m_Digits.ToInt();
			shown = INT_Bank_Util.Money(v);
			col = INT_Bank_UILook.Ink();
		}
		m_Value.SetText(shown);
		m_Value.SetColor(col);
		float tw = INT_Bank_UIMeasure.TextW(m_Value, shown, 46);
		float cx = m_ValueX + tw + 6;
		if (m_Caret)
			INT_UIF_Draw.Place(m_Caret, cx, 38, 3, 42);
		if (m_Cur)
			INT_UIF_Draw.Place(m_Cur, cx + 14, 52, 120, 28);
	}

	// A short horizontal shake (refused input).
	void Shake()
	{
		if (!m_Value)
			return;
		INT_Bank_UISfx.Play(INT_Bank_Sfx.ERROR);
		float x0 = INT_UIF_Draw.S(m_ValueX);
		float dx = INT_UIF_Draw.S(10);
		INT_UIF_Draw.Place(m_Value, m_ValueX + 10, 30, m_W - 150, 58);
		INT_UIF_Tween t = INT_UIF_Animator.MoveX(m_Value, x0, 0.32, INT_UIF_Ease.OUT_ELASTIC);
		if (m_Box)
		{
			m_Box.SetBorder(ARGB(200, 214, 104, 86));
			m_Box.FadeBorder(ARGB(60, 216, 174, 98), 0.6);
		}
	}
}

// ----------------------------------------------------------------------------------------------------
// Bank card
// ----------------------------------------------------------------------------------------------------
class INT_Bank_UICard
{
	protected Widget m_Root;
	protected Widget m_Clip;
	protected ImageWidget m_Sheen;
	protected TextWidget m_Number;
	protected TextWidget m_Holder;
	protected TextWidget m_Balance;
	protected float m_W;
	protected float m_H;
	protected float m_SheenWait = 1.2;

	Widget GetRoot()
	{
		return m_Root;
	}

	void Build(Widget parent, float x, float y, float w, float h)
	{
		m_W = w;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "BankCard");
		INT_UIF_Surface.Shadow(m_Root, -22, -10, w + 44, h + 40, ARGB(190, 0, 0, 0));
		INT_UIF_Surface.Glow(m_Root, -16, -14, w + 32, h + 28, ARGB(40, 216, 174, 98));
		INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 14, ARGB(255, 52, 40, 24), ARGB(150, 230, 190, 120));
		m_Clip = INT_UIF_Draw.Clip(m_Root, 2, 2, w - 4, h - 4, "CardClip");
		float cw = w - 4;
		float ch = h - 4;
		// warm metal gradient + guilloche + a darker band at the bottom
		INT_UIF_Draw.Image(m_Clip, 0, 0, cw, ch, "iuf_radial", ARGB(120, 196, 150, 80));
		INT_UIF_Draw.Image(m_Clip, 0, 0, cw, ch * 0.6, "iuf_grad_v", ARGB(70, 255, 220, 160));
		INT_UIF_Draw.Texture(m_Clip, 0, 0, cw, ch, INT_BANK_TEX_AURORA, ARGB(110, 255, 214, 150));
		INT_UIF_Draw.Texture(m_Clip, 0, 0, cw, ch, INT_BANK_TEX_STRIPES, ARGB(26, 255, 236, 200));
		INT_UIF_Draw.Line(m_Clip, 0, ch - 46, cw, 46, ARGB(70, 10, 8, 5));
		// sheen sweep
		m_Sheen = INT_UIF_Draw.Texture(m_Clip, -cw, 0, cw * 0.9, ch, INT_BANK_TEX_SHEEN, ARGB(60, 255, 245, 220));

		// wordmark
		TextWidget brand = INT_UIF_Draw.Text(m_Clip, 22, 16, 220, 22, "DAYZAVR", 17, ARGB(255, 255, 236, 196));
		if (brand)
			brand.SetBold(true);
		INT_UIF_Draw.Text(m_Clip, 22, 37, 220, 14, "B A N K", 9, ARGB(220, 255, 226, 170));
		INT_UIF_Draw.Text(m_Clip, cw - 132, 18, 110, 16, "DEBIT", 11, ARGB(200, 255, 236, 196), INT_UIF_Align.RIGHT);
		// chip
		Widget chip = INT_UIF_Draw.Frame(m_Clip, 24, 66, 46, 36, "Chip");
		INT_UIF_Surface.Box(chip, 0, 0, 46, 36, 7, ARGB(255, 226, 192, 120), ARGB(200, 120, 90, 40));
		INT_UIF_Draw.Line(chip, 0, 12, 46, 1, ARGB(150, 120, 90, 40));
		INT_UIF_Draw.Line(chip, 0, 24, 46, 1, ARGB(150, 120, 90, 40));
		INT_UIF_Draw.Line(chip, 16, 0, 1, 36, ARGB(150, 120, 90, 40));
		INT_UIF_Draw.Line(chip, 30, 0, 1, 36, ARGB(150, 120, 90, 40));
		INT_UIF_Draw.Image(chip, 2, 1, 42, 14, "iuf_grad_v", ARGB(90, 255, 255, 255));
		// contactless waves
		INT_UIF_Draw.Image(m_Clip, 80, 72, 24, 24, "iuf_ringline32", ARGB(150, 255, 230, 180));
		INT_UIF_Draw.Image(m_Clip, 86, 78, 12, 12, "iuf_ringline16", ARGB(150, 255, 230, 180));
		// number, holder, balance
		m_Number = INT_UIF_Draw.Text(m_Clip, 24, 112, cw - 48, 28, "", 21, ARGB(255, 255, 240, 214));
		INT_UIF_Draw.Text(m_Clip, 24, ch - 40, 160, 12, "#STR_INT_BANK_CARD_HOLDER", 8, ARGB(200, 230, 200, 150));
		m_Holder = INT_UIF_Draw.Text(m_Clip, 24, ch - 27, cw * 0.55, 18, "", 13, ARGB(255, 255, 240, 214));
		if (m_Holder)
			m_Holder.SetBold(true);
		INT_UIF_Draw.Text(m_Clip, cw * 0.5, ch - 40, cw * 0.5 - 70, 12, "#STR_INT_BANK_CARD_BALANCE", 8, ARGB(200, 230, 200, 150), INT_UIF_Align.RIGHT);
		m_Balance = INT_UIF_Draw.Text(m_Clip, cw * 0.4, ch - 27, cw * 0.6 - 70, 18, "", 13, ARGB(255, 255, 240, 214), INT_UIF_Align.RIGHT);
		if (m_Balance)
			m_Balance.SetBold(true);
		// two interlocking circles (card network mark)
		INT_UIF_Draw.Image(m_Clip, cw - 62, ch - 40, 28, 28, "iuf_circle32", ARGB(220, 210, 120, 60));
		INT_UIF_Draw.Image(m_Clip, cw - 44, ch - 40, 28, 28, "iuf_circle32", ARGB(200, 240, 200, 110));
	}

	void SetOwner(string name, string steamId)
	{
		if (m_Holder)
		{
			string n = name;
			n.ToUpper();
			m_Holder.SetText(n);
		}
		if (m_Number)
		{
			string num = "**** **** **** ****";
			if (steamId.Length() >= 8)
			{
				string head = steamId.Substring(0, 4);
				string tail = steamId.Substring(steamId.Length() - 4, 4);
				num = head + "  ****  ****  " + tail;
			}
			m_Number.SetText(num);
		}
	}

	void SetBalance(int balance)
	{
		if (!m_Balance)
			return;
		string b = INT_Bank_UILook.Money(balance);
		m_Balance.SetText(b);
	}

	// From the window's Update: a light band sweeps over the card every ~4 s.
	void Tick(float dt)
	{
		if (!m_Sheen)
			return;
		m_SheenWait = m_SheenWait - dt;
		if (m_SheenWait > 0)
			return;
		m_SheenWait = 4.2;
		float to = INT_UIF_Draw.S(m_W * 1.1);
		INT_UIF_Draw.Place(m_Sheen, -m_W, 0, m_W * 0.9, m_H - 4);
		INT_UIF_Animator.MoveX(m_Sheen, to, 1.6, INT_UIF_Ease.IN_OUT_SINE);
	}

	// Tilt-in on open (slides up a little while fading in).
	void Reveal(float delay)
	{
		if (m_Root)
			INT_UIF_Animator.Reveal(m_Root, delay, 18, 0.5);
	}
}

// ----------------------------------------------------------------------------------------------------
// Side menu
// ----------------------------------------------------------------------------------------------------
class INT_Bank_UINavItem
{
	Widget m_Root;
	ButtonWidget m_Hit;
	ref INT_UIF_Surface m_Hover;
	ImageWidget m_Icon;
	TextWidget m_Label;
	TextWidget m_KeyCap;
	float m_Y;
}

class INT_Bank_UINav extends ScriptedWidgetEventHandler
{
	static const float ITEM_H = 50;
	static const float GAP = 6;

	protected Widget m_Root;
	protected Widget m_Indicator;
	protected ref INT_UIF_Surface m_IndicatorBody;
	protected ref array<ref INT_Bank_UINavItem> m_Items;
	protected ref ScriptInvoker m_OnChanged;	// (int index)
	protected int m_Active = -1;
	protected float m_W;

	void INT_Bank_UINav()
	{
		m_Items = new array<ref INT_Bank_UINavItem>();
		m_OnChanged = new ScriptInvoker();
	}

	ScriptInvoker OnChanged()
	{
		return m_OnChanged;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	int GetActive()
	{
		return m_Active;
	}

	void Build(Widget parent, float x, float y, float w, float h)
	{
		m_W = w;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "BankNav");
		m_Indicator = INT_UIF_Draw.Frame(m_Root, 0, 0, w, ITEM_H, "NavIndicator");
		INT_UIF_Surface.Glow(m_Indicator, -10, -8, w + 20, ITEM_H + 16, ARGB(34, 216, 174, 98));
		m_IndicatorBody = INT_UIF_Surface.Box(m_Indicator, 0, 0, w, ITEM_H, 10, ARGB(46, 216, 174, 98), ARGB(120, 216, 174, 98));
		INT_UIF_Draw.Line(m_Indicator, 0, 10, 3, ITEM_H - 20, INT_Bank_UILook.Brass());
		m_Indicator.Show(false);
	}

	int AddItem(string label, string icon, string keyHint)
	{
		int index = m_Items.Count();
		INT_Bank_UINavItem it = new INT_Bank_UINavItem();
		it.m_Y = index * (ITEM_H + GAP);
		it.m_Root = INT_UIF_Draw.Frame(m_Root, 0, it.m_Y, m_W, ITEM_H, "NavItem");
		it.m_Hover = INT_UIF_Surface.Box(it.m_Root, 0, 0, m_W, ITEM_H, 10, ARGB(0, 255, 240, 215), 0);
		it.m_Icon = INT_UIF_Draw.Icon(it.m_Root, 18, (ITEM_H - 18) * 0.5, 18, icon, INT_Bank_UILook.Muted());
		it.m_Label = INT_UIF_Draw.Text(it.m_Root, 50, 0, m_W - 96, ITEM_H, label, 15, INT_Bank_UILook.Muted());
		if (it.m_Label)
			it.m_Label.SetBold(true);
		Widget cap = INT_UIF_Draw.Frame(it.m_Root, m_W - 40, (ITEM_H - 22) * 0.5, 26, 22, "NavKey");
		INT_UIF_Surface.Box(cap, 0, 0, 26, 22, 5, ARGB(40, 0, 0, 0), ARGB(70, 255, 236, 200));
		it.m_KeyCap = INT_UIF_Draw.Text(cap, 0, 0, 26, 22, keyHint, 11, INT_Bank_UILook.Sand(), INT_UIF_Align.CENTER);
		if (it.m_KeyCap)
			it.m_KeyCap.SetBold(true);
		it.m_Hit = INT_UIF_Draw.Hit(it.m_Root, 0, 0, m_W, ITEM_H, this);
		m_Items.Insert(it);
		return index;
	}

	void Select(int index, bool notify, bool animate)
	{
		if (index < 0 || index >= m_Items.Count())
			return;
		bool changed = index != m_Active;
		m_Active = index;
		INT_Bank_UINavItem target = m_Items.Get(index);
		if (m_Indicator)
		{
			bool first = !m_Indicator.IsVisible();
			m_Indicator.Show(true);
			if (animate && !first)
				INT_UIF_Animator.MoveY(m_Indicator, INT_UIF_Draw.S(target.m_Y), 0.28, INT_UIF_Ease.OUT_BACK);
			else
				INT_UIF_Draw.Move(m_Indicator, 0, target.m_Y);
		}
		int n = m_Items.Count();
		for (int i = 0; i < n; i++)
		{
			INT_Bank_UINavItem it = m_Items.Get(i);
			int col = INT_Bank_UILook.Muted();
			if (i == index)
				col = INT_Bank_UILook.BrassLight();
			if (it.m_Label)
				INT_UIF_Animator.ColorTo(it.m_Label, col, 0.2);
			if (it.m_Icon)
				INT_UIF_Animator.ColorTo(it.m_Icon, col, 0.2);
		}
		if (changed && notify)
		{
			int sel = index;
			m_OnChanged.Invoke(sel);
		}
	}

	protected int IndexOf(Widget w)
	{
		int n = m_Items.Count();
		for (int i = 0; i < n; i++)
		{
			INT_Bank_UINavItem it = m_Items.Get(i);
			if (it.m_Hit == w)
				return i;
		}
		return -1;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int i = IndexOf(w);
		if (i < 0)
			return false;
		if (button != MouseState.LEFT)
			return true;
		if (i != m_Active)
			INT_Bank_UISfx.Play(INT_Bank_Sfx.TAB);
		Select(i, true, true);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int i = IndexOf(w);
		if (i < 0)
			return false;
		INT_Bank_UINavItem it = m_Items.Get(i);
		it.m_Hover.FadeFill(ARGB(16, 255, 240, 215), 0.12);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int i = IndexOf(w);
		if (i < 0)
			return false;
		INT_Bank_UINavItem it = m_Items.Get(i);
		it.m_Hover.FadeFill(ARGB(0, 255, 240, 215), 0.2);
		return false;
	}
}

// ----------------------------------------------------------------------------------------------------
// Confirm dialog
// ----------------------------------------------------------------------------------------------------
class INT_Bank_UIDialog extends INT_UIF_Dialog
{
	// One button (OK), warning look.
	static INT_Bank_UIDialog Notice(string title, string text, string ok, string icon)
	{
		INT_Bank_UIDialog d = new INT_Bank_UIDialog();
		d.SetContent(title, text, icon, INT_UIF_ToastKind.WARNING);
		d.AddButton(ok, INT_UIF_Variant.PRIMARY);
		INT_Bank_UISfx.Play(INT_Bank_Sfx.OPEN);
		INT_UIF_Dialog.Present(d);
		return d;
	}

	static INT_Bank_UIDialog Ask(string title, string text, string yes, string no, int yesVariant, string icon)
	{
		INT_Bank_UIDialog d = new INT_Bank_UIDialog();
		int kind = INT_UIF_ToastKind.SUCCESS;
		if (yesVariant == INT_UIF_Variant.DANGER)
			kind = INT_UIF_ToastKind.ERROR;
		else if (yesVariant == INT_UIF_Variant.WARNING)
			kind = INT_UIF_ToastKind.WARNING;
		d.SetContent(title, text, icon, kind);
		d.AddButton(no, INT_UIF_Variant.SECONDARY);
		d.AddButton(yes, yesVariant);
		INT_Bank_UISfx.Play(INT_Bank_Sfx.OPEN);
		INT_UIF_Dialog.Present(d);
		return d;
	}

	override void OnBuild()
	{
		super.OnBuild();
		foreach (INT_UIF_Button b : m_ButtonList)
		{
			if (b)
				b.OnClick().Insert(OnAnyButton);
		}
	}

	void OnAnyButton(INT_UIF_Button b)
	{
		INT_Bank_UISfx.Play(INT_Bank_Sfx.CLICK);
	}

	override void ApplyTheme()
	{
		super.ApplyTheme();
		if (m_Shade)
			m_Shade.SetColor(ARGB(205, 6, 5, 3));
	}

	override void PlayOpen()
	{
		super.PlayOpen();
		if (m_Shade)
		{
			m_Shade.SetColor(ARGB(205, 6, 5, 3));
			INT_UIF_Animator.FadeInColor(m_Shade, 0.22);
		}
	}
}
