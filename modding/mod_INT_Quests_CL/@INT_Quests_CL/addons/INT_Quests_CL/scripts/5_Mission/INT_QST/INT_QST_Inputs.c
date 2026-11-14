// INT Quests (client) — paper-style inputs for the admin windows (design B): a caption in spaced caps over an
// ink-ruled field. The framework's own inputs are themed "modern dark"; these draw on the notebook palette and keep
// the same habits (literal layout paths, events through ScriptInvokers, no work in Update). A window that holds them
// must say WantsKeyboard() = true, else typed keys go to the character.

// One-line field.
class INT_QST_Field extends ScriptedWidgetEventHandler
{
	static const float H = 34;

	protected Widget m_Root;
	protected EditBoxWidget m_Edit;
	protected Widget m_Line;
	protected Widget m_Focus;
	protected bool m_Numeric;
	protected bool m_Decimal;
	protected int m_MaxLen;
	protected string m_Last;
	protected string m_Initial;				// the value given by Create / SetText
	ref ScriptInvoker Event_Changed = new ScriptInvoker();	// (INT_QST_Field f)
	ref ScriptInvoker Event_Submit = new ScriptInvoker();	// (INT_QST_Field f)
	int m_Tag;						// free for the owner (row index, field id)

	// labelKey "" = no caption (the field starts at y). numeric: digits (and '-'); decimal also allows '.'.
	static INT_QST_Field Create(Widget parent, float x, float y, float w, string labelKey, string value, int maxLen = 0, bool numeric = false, bool decimal = false)
	{
		INT_QST_Field f = new INT_QST_Field();
		f.Build(parent, x, y, w, labelKey, value, maxLen, numeric, decimal);
		return f;
	}

	static float Height(string labelKey)
	{
		if (labelKey == "")
			return H;
		return H + 20;
	}

	protected void Build(Widget parent, float x, float y, float w, string labelKey, string value, int maxLen, bool numeric, bool decimal)
	{
		m_MaxLen = maxLen;
		m_Numeric = numeric;
		m_Decimal = decimal;
		float top = 0;
		float total = Height(labelKey);
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, total, "Field");
		if (labelKey != "")
		{
			string cap = INT_QST_Look.Spaced(labelKey);
			INT_QST_Look.Text(m_Root, 0, 0, w, 18, cap, 11, INT_QST_Look.Ink3());
			top = 20;
		}
		int fill = 0x2EFFFFFF;
		int edge = INT_QST_Look.InkA(90);
		INT_UIF_Surface.Box(m_Root, 0, top, w, H, 4, fill, edge);
		m_Focus = INT_UIF_Draw.Frame(m_Root, 0, top, w, H, "Focus");
		int fb = INT_QST_Look.Blue();
		INT_UIF_Surface.Box(m_Focus, 0, 0, w, H, 4, 0x14FFFFFF, fb);
		m_Focus.Show(false);
		m_Edit = INT_UIF_Draw.Edit(m_Root, 8, top, w - 16, H, 17);
		if (m_Edit)
		{
			m_Edit.SetHandler(this);
			m_Edit.SetTextColor(INT_QST_Look.Ink());
			m_Edit.SetText(value);
		}
		m_Last = value;
		m_Initial = value;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	string GetText()
	{
		if (!m_Edit)
			return m_Initial;
		return m_Edit.GetText();
	}

	void SetText(string t)
	{
		m_Last = t;
		m_Initial = t;
		if (m_Edit)
			m_Edit.SetText(t);
	}

	// True when the text differs from what Create / SetText put in (the admin typed something).
	bool IsChanged()
	{
		string t = GetText();
		return t != m_Initial;
	}

	int GetInt()
	{
		string t = GetText();
		return t.ToInt();
	}

	float GetFloat()
	{
		string t = GetText();
		t = INT_QST_Util.ReplaceByte(t, ",", ".");
		return t.ToFloat();
	}

	void Focus()
	{
		if (m_Edit)
			SetFocus(m_Edit);
	}

	bool IsFocused()
	{
		return m_Edit && GetFocus() == m_Edit;
	}

	void Show(bool on)
	{
		if (m_Root)
			m_Root.Show(on);
	}

	// keeps digits (and the sign / one dot) of a numeric field
	protected string Filter(string t)
	{
		if (!m_Numeric)
		{
			if (m_MaxLen > 0 && t.LengthUtf8() > m_MaxLen)
				return t.SubstringUtf8(0, m_MaxLen);
			return t;
		}
		string r = "";
		bool dot = false;
		int n = t.Length();
		for (int i = 0; i < n; i++)
		{
			string c = t.Get(i);
			if (c == "-" && r == "")
				r = r + c;
			else if (m_Decimal && (c == "." || c == ",") && !dot)
			{
				r = r + ".";
				dot = true;
			}
			else if (c.ToAscii() >= 48 && c.ToAscii() <= 57)
				r = r + c;
		}
		if (m_MaxLen > 0 && r.Length() > m_MaxLen)
			r = r.Substring(0, m_MaxLen);
		return r;
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w != m_Edit)
			return false;
		string t = m_Edit.GetText();
		string f = Filter(t);
		if (f != t)
			m_Edit.SetText(f);
		if (f != m_Last)
		{
			m_Last = f;
			Event_Changed.Invoke(this);
		}
		if (finished)
			Event_Submit.Invoke(this);
		return true;
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (w == m_Edit && m_Focus)
			m_Focus.Show(true);
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (w == m_Edit && m_Focus)
			m_Focus.Show(false);
		return false;
	}
}

// Multi-line field (quest description, greeting). The engine box does not wrap a long line (it ran off to the right,
// measured 2026-10-22), so the text is shown wrapped to the box width (glyph size fixed in the layout: TEXT_PX) and read
// back unwrapped: lines of one paragraph are joined with a space, empty lines between paragraphs are the "\n" of the
// text. A line typed past the edge is wrapped again when the field loses focus.
// Reading back normalises (double spaces, words longer than a line), so a field the admin did not touch gives the
// ORIGINAL text back exactly: the box content is compared with what was put in ("save without changes" changes nothing).
class INT_QST_Area extends ScriptedWidgetEventHandler
{
	static const float TEXT_PX = 15;		// = "exact text size" of int_qst_medit.layout

	protected Widget m_Root;
	protected MultilineEditBoxWidget m_Edit;
	protected Widget m_Focus;
	protected string m_Last;
	protected float m_WrapW;
	protected string m_Orig;			// the text given by Create / SetText
	protected string m_Shown;			// Flow(m_Orig) as put into the box
	protected string m_ShownRaw;			// the box lines read back right after that (engine view of m_Shown)
	protected bool m_RawValid;
	ref ScriptInvoker Event_Changed = new ScriptInvoker();	// (INT_QST_Area a)

	static INT_QST_Area Create(Widget parent, float x, float y, float w, float h, string labelKey, string value)
	{
		INT_QST_Area a = new INT_QST_Area();
		a.Build(parent, x, y, w, h, labelKey, value);
		return a;
	}

	protected void Build(Widget parent, float x, float y, float w, float h, string labelKey, string value)
	{
		float top = 0;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "Area");
		if (labelKey != "")
		{
			string cap = INT_QST_Look.Spaced(labelKey);
			INT_QST_Look.Text(m_Root, 0, 0, w, 18, cap, 11, INT_QST_Look.Ink3());
			top = 20;
		}
		float bh = h - top;
		int fill = 0x40FFFAEB;
		int edge = INT_QST_Look.InkA(90);
		INT_UIF_Surface.Box(m_Root, 0, top, w, bh, 4, fill, edge);
		m_Focus = INT_UIF_Draw.Frame(m_Root, 0, top, w, bh, "Focus");
		int fb = INT_QST_Look.Blue();
		INT_UIF_Surface.Box(m_Focus, 0, 0, w, bh, 4, 0x0FFFFFFF, fb);
		m_Focus.Show(false);
		Widget holder = INT_UIF_Draw.Frame(m_Root, 10, top + 6, w - 20, bh - 12, "Holder");
		m_Edit = MultilineEditBoxWidget.Cast(GetGame().GetWorkspace().CreateWidgets("INT_Quests_CL/gui/layouts/int_qst_medit.layout", holder));
		m_WrapW = w - 28;
		if (m_Edit)
		{
			INT_UIF_Draw.Place(m_Edit, 0, 0, w - 20, bh - 12);
			m_Edit.SetHandler(this);
			m_Edit.SetColor(INT_QST_Look.Blue());
		}
		Put(value);
	}

	// The original text into the box (wrapped) and what the box holds now, for the "untouched" check.
	protected void Put(string value)
	{
		m_Orig = value;
		m_Last = value;
		m_Shown = Flow(value);
		m_ShownRaw = "";
		m_RawValid = false;
		if (!m_Edit)
			return;
		m_Edit.SetText(m_Shown);
		int n = m_Edit.GetLinesCount();
		if (n > 0 || m_Shown == "")
		{
			m_ShownRaw = RawLines();
			m_RawValid = true;
		}
	}

	// The box lines as they are (no trimming, no joining of paragraphs), "\n" between them.
	protected string RawLines()
	{
		if (!m_Edit)
			return "";
		int n = m_Edit.GetLinesCount();
		string res = "";
		for (int i = 0; i < n; i++)
		{
			string ln = "";
			m_Edit.GetLine(i, ln);
			ln.Replace("\r", "");		// one wrapped line: far below the Replace limit
			ln.Replace("\n", "");
			if (i > 0)
				res = res + "\n";
			res = res + ln;
		}
		return res;
	}

	// True when the admin changed the text in the box.
	bool IsChanged()
	{
		if (!m_Edit)
			return false;
		string raw = RawLines();
		if (m_RawValid)
			return raw != m_ShownRaw;
		return raw != m_Shown;
	}

	// text -> wrapped lines; one empty line before every paragraph but the first ("a\nb" = a, "", b)
	protected string Flow(string text)
	{
		array<string> paras = new array<string>();
		SplitKeep(text, paras);
		array<string> all = new array<string>();
		array<string> lines = new array<string>();
		for (int i = 0; i < paras.Count(); i++)
		{
			if (i > 0)
				all.Insert("");
			string para = paras[i];
			para.TrimInPlace();
			if (para == "")
				continue;
			INT_QST_Text.Wrap(para, m_WrapW, TEXT_PX, lines);
			foreach (string ln : lines)
				all.Insert(ln);
		}
		string res = "";
		for (int k = 0; k < all.Count(); k++)
		{
			if (k > 0)
				res = res + "\n";
			res = res + all[k];
		}
		return res;
	}

	// "\n"-split that keeps empty pieces (string.Split drops them)
	protected static void SplitKeep(string text, array<string> outList)
	{
		outList.Clear();
		// the description can be long: string.Replace corrupts strings over 2560 bytes (measured), ReplaceByte cuts
		string rest = INT_QST_Util.ReplaceByte(text, "\r", "");
		int guard = 0;
		while (guard < 2000)
		{
			guard++;
			int at = rest.IndexOf("\n");
			if (at < 0)
			{
				outList.Insert(rest);
				return;
			}
			string head = rest.Substring(0, at);
			outList.Insert(head);
			int tail = rest.Length() - at - 1;
			rest = rest.Substring(at + 1, tail);
		}
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	// the lines back into text: a paragraph's lines joined with a space, N empty lines between paragraphs = N "\n";
	// an untouched box gives the original text back exactly
	string GetText()
	{
		if (!m_Edit || !IsChanged())
			return m_Orig;
		return ReadBack();
	}

	protected string ReadBack()
	{
		int n = m_Edit.GetLinesCount();
		string res = "";
		string para = "";
		int empties = 0;
		for (int i = 0; i < n; i++)
		{
			string ln = "";
			m_Edit.GetLine(i, ln);
			ln.Replace("\r", "");
			ln.Replace("\n", "");
			ln.TrimInPlace();
			if (ln == "")
			{
				if (para != "")
					empties++;
				continue;
			}
			if (para != "" && empties > 0)
			{
				res = res + para;
				for (int e = 0; e < empties; e++)
					res = res + "\n";
				para = "";
			}
			empties = 0;
			if (para == "")
				para = ln;
			else
				para = para + " " + ln;
		}
		res = res + para;
		return res;
	}

	void SetText(string t)
	{
		Put(t);
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w != m_Edit)
			return false;
		string t = GetText();
		if (t != m_Last)
		{
			m_Last = t;
			Event_Changed.Invoke(this);
		}
		return true;
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (w == m_Edit && m_Focus)
			m_Focus.Show(true);
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (w == m_Edit && m_Focus)
			m_Focus.Show(false);
		// a line typed past the edge: wrap it now (an untouched box stays as it was put in)
		if (w == m_Edit && IsChanged())
		{
			string t = ReadBack();
			string shown = Flow(t);
			m_Edit.SetText(shown);
		}
		return false;
	}
}

// A row of choice chips (repeat mode, target kind, giver kind). One is selected; click selects.
class INT_QST_Choice extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ref array<string> m_Labels = new array<string>();
	protected ref array<Widget> m_Chips = new array<Widget>();
	protected ref array<Widget> m_OldChips = new array<Widget>();	// hidden, unlinked on the next redraw
	protected ref map<Widget, int> m_HitIndex = new map<Widget, int>();
	protected int m_Sel;
	protected float m_W;
	protected float m_Top;
	ref ScriptInvoker Event_Changed = new ScriptInvoker();	// (INT_QST_Choice c, int index)
	int m_Tag;

	// labels: stringtable keys or plain text.
	static INT_QST_Choice Create(Widget parent, float x, float y, float w, string labelKey, array<string> labels, int selected)
	{
		INT_QST_Choice c = new INT_QST_Choice();
		c.Build(parent, x, y, w, labelKey, labels, selected);
		return c;
	}

	protected void Build(Widget parent, float x, float y, float w, string labelKey, array<string> labels, int selected)
	{
		m_W = w;
		foreach (string l : labels)
			m_Labels.Insert(l);
		m_Sel = selected;
		float h = 30;
		if (labelKey != "")
			h = h + 20;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "Choice");
		if (labelKey != "")
		{
			string cap = INT_QST_Look.Spaced(labelKey);
			INT_QST_Look.Text(m_Root, 0, 0, w, 18, cap, 11, INT_QST_Look.Ink3());
			m_Top = 20;
		}
		Redraw();
	}

	static float Height(string labelKey)
	{
		if (labelKey == "")
			return 30;
		return 50;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	int GetIndex()
	{
		return m_Sel;
	}

	void SetIndex(int i, bool notify = false)
	{
		m_Sel = i;
		Redraw();
		if (notify)
			Event_Changed.Invoke(this, i);
	}

	// A click redraws from inside the clicked chip's own event: the chips of this redraw are only hidden and
	// unlinked on the NEXT redraw (never unlink a widget inside its own event).
	protected void Redraw()
	{
		foreach (Widget old : m_OldChips)
		{
			if (old)
				old.Unlink();
		}
		m_OldChips.Clear();
		foreach (Widget c : m_Chips)
		{
			if (!c)
				continue;
			c.Show(false);
			m_OldChips.Insert(c);
		}
		m_Chips.Clear();
		m_HitIndex.Clear();
		float cx = 0;
		int n = m_Labels.Count();
		for (int i = 0; i < n; i++)
		{
			string t = INT_QST_Look.Tr(m_Labels[i]);
			float tw = INT_UIF_Draw.TextWidth(t, 13) * 1.12 + 22;
			if (cx + tw > m_W && cx > 0)
				break;
			bool on = i == m_Sel;
			Widget chip = INT_UIF_Draw.Frame(m_Root, cx, m_Top, tw, 28, "Chip");
			int fill = 0x26FFFFFF;
			int edge = INT_QST_Look.InkA(90);
			int tc = INT_QST_Look.Ink2();
			if (on)
			{
				fill = INT_QST_Look.Ink();
				edge = INT_QST_Look.Ink();
				tc = INT_QST_Look.Cream();
			}
			INT_UIF_Surface.Box(chip, 0, 0, tw, 28, 4, fill, edge);
			INT_QST_Look.Heavy(chip, 0, 0, tw, 28, t, 13, tc, INT_UIF_Align.CENTER);
			ButtonWidget hit = INT_UIF_Draw.Hit(chip, 0, 0, tw, 28, this);
			m_HitIndex.Set(hit, i);
			m_Chips.Insert(chip);
			cx = cx + tw + 6;
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int i;
		if (!m_HitIndex.Find(w, i))
			return false;
		if (button != MouseState.LEFT)
			return true;
		if (i != m_Sel)
		{
			m_Sel = i;
			Redraw();
			INT_QST_Sfx.Play("click");
			Event_Changed.Invoke(this, i);
		}
		return true;
	}
}

// Checkbox with a label.
class INT_QST_Check extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected Widget m_Box;
	protected ButtonWidget m_Hit;
	protected bool m_On;
	protected string m_Label;
	protected float m_W;
	ref ScriptInvoker Event_Changed = new ScriptInvoker();	// (INT_QST_Check c, bool on)
	int m_Tag;

	static INT_QST_Check Create(Widget parent, float x, float y, float w, string labelKey, bool on)
	{
		INT_QST_Check c = new INT_QST_Check();
		c.m_Label = labelKey;
		c.m_W = w;
		c.m_On = on;
		c.m_Root = INT_UIF_Draw.Frame(parent, x, y, w, 26, "Check");
		c.m_Box = INT_UIF_Draw.Frame(c.m_Root, 0, 0, 26, 26, "Box");
		string t = INT_QST_Look.Tr(labelKey);
		INT_QST_Look.Text(c.m_Root, 32, 0, w - 32, 26, t, 14, INT_QST_Look.Ink());
		c.m_Hit = INT_UIF_Draw.Hit(c.m_Root, 0, 0, w, 26, c);
		c.Redraw();
		return c;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	bool IsOn()
	{
		return m_On;
	}

	void SetOn(bool on)
	{
		m_On = on;
		Redraw();
	}

	protected void Redraw()
	{
		INT_UIF_Widgets.ClearChildren(m_Box);
		string icon = "checkbox_empty";
		int c = INT_QST_Look.Ink2();
		if (m_On)
		{
			icon = "checkbox_checked";
			c = INT_QST_Look.Olive();
		}
		INT_QST_Look.Icon(m_Box, 2, 2, 22, icon, c);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		m_On = !m_On;
		Redraw();
		INT_QST_Sfx.Play("check");
		Event_Changed.Invoke(this, m_On);
		return true;
	}
}
