// INT PartyMode — the C «ЭКСПЕДИЦИЯ» look (spec.md §2 D16; ported from INT_FrameworkSkills INT_PTY_Look, same style family): palette, textures and small drawing helpers on
// INT_UIFramework primitives. All sizes are design px of the 1920x1080 canvas (INT_UIF_Draw scales them).
// Textures are white masks from tools/gen_pty_art.py, tinted here.

const string INT_PTY_TEX = "INT_PartyMode_CL/gui/textures/";

class INT_PTY_Look
{
	static const int ORANGE = 0xFFFF7A2F;
	static const int ORANGE_SOFT = 0x2EFF7A2F;		// 18 % wash
	static const int WHITE = 0xFFF4F4F4;
	static const int TEXT = 0xFFE8E8E8;
	static const int BODY = 0xFFBDBDBD;
	static const int MUTED = 0xFF9A9A9A;
	static const int FAINT = 0xFF6F6F6F;
	static const int DARK = 0xFF5A5A5A;
	static const int LINE = 0x1FFFFFFF;				// 12 %
	static const int LINE_STRONG = 0x38FFFFFF;		// 22 %
	static const int RED = 0xFFE5534B;
	static const int INK = 0xFF120800;				// text on orange
	static const int PANEL = 0xEB080808;				// 92 % black
	static const int BUTTON_BG = 0xD1060606;			// 82 % black

	// --- textures ----------------------------------------------------------------------------------------
	static string TexPath(string name)
	{
		return INT_PTY_TEX + name + ".paa";
	}

	// "run" -> built-in icon; a value with '/' is a texture path of the skill mod.
	static string IconPath(string icon)
	{
		string i = icon;
		if (i == "")
			i = "info";
		if (i.IndexOf("/") >= 0 || i.IndexOf("\\") >= 0)
			return i;
		return INT_PTY_TEX + "pty_icon_" + i + "_ca.paa";
	}

	static ImageWidget Fill(Widget parent, float x, float y, float w, float h, int color)
	{
		return INT_UIF_Draw.Texture(parent, x, y, w, h, TexPath("pty_px_ca"), color);
	}

	static ImageWidget Tex(Widget parent, float x, float y, float w, float h, string name, int color)
	{
		return INT_UIF_Draw.Texture(parent, x, y, w, h, TexPath(name), color);
	}

	static ImageWidget IconAt(Widget parent, float x, float y, float size, string icon, int color)
	{
		return INT_UIF_Draw.Texture(parent, x, y, size, size, IconPath(icon), color);
	}

	// A diamond with side `size`, centred on (cx, cy). The diamond is drawn inside the texture (pty_diamond_*,
	// diagonal = widget side) and the widget is NOT rotated: the engine clips a rotated image to its unrotated
	// rect (measured 2026-10-18: a square at 45 degrees drew an octagon). Any spin of the widget stays inside
	// the inscribed circle, so animations may still rotate it. "pty_square*" names map to the diamond masks.
	static const float SQRT2 = 1.41421356;

	static string DiamondTex(string texName)
	{
		string t = texName;
		t.Replace("pty_square", "pty_diamond");
		return t;
	}

	static ImageWidget Diamond(Widget parent, float cx, float cy, float size, string texName, int color)
	{
		float w = size * SQRT2;
		string tex = DiamondTex(texName);
		return INT_UIF_Draw.Texture(parent, cx - w * 0.5, cy - w * 0.5, w, w, TexPath(tex), color);
	}

	// Moves an existing diamond to a new centre / side (design px).
	static void DiamondPlace(Widget w, float cx, float cy, float size)
	{
		if (!w)
			return;
		float side = size * SQRT2;
		INT_UIF_Draw.Place(w, cx - side * 0.5, cy - side * 0.5, side, side);
		w.SetRotation(0, 0, 0);
	}

	// --- text --------------------------------------------------------------------------------------------
	static TextWidget Text(Widget parent, float x, float y, float w, float h, string value, float size, int color, int align = INT_UIF_Align.LEFT, bool bold = false, bool big = false)
	{
		TextWidget t = INT_UIF_Draw.Text(parent, x, y, w, h, value, size, color, align, big);
		if (t && bold)
		{
			t.SetBold(true);
			// The SDF fonts have no bold face (SetBold only widens the advance). Measured 2026-10-18: an outline
			// glued the letters together and the bitmap Metron-Bold scaled unevenly at 4:3, so bold text from
			// 16 px up is a classic faux bold: a second copy shifted a few % of the size. Sharp at every
			// resolution, positions scale with the canvas.
			if (size >= 16)
				Heavy(t, parent, x, y, w, h, value, size, color, align, big);
		}
		return t;
	}

	// Faux-bold twin: a SIBLING copy in the same parent, same size, shifted ~3 % of the font size. (A copy
	// nested inside the text widget drew its glyphs at a slightly different scale: long titles doubled at the
	// end, measured 2026-10-18.) Entrance animations (INT_PTY_Fx) and SetHeavyText move / change both.
	protected static ref map<Widget, Widget> s_Twins;

	static void Heavy(TextWidget t, Widget parent, float x, float y, float w, float h, string value, float size, int color, int align, bool big)
	{
		if (!t)
			return;
		float dx = size * 0.03;
		if (size < 30)
			dx = size * 0.045;
		TextWidget d = INT_UIF_Draw.Text(parent, x + dx, y, w, h, value, size, color, align, big);
		if (!d)
			return;
		// SetBold does not thicken SDF glyphs but widens their advance: the copy needs it too, or it drifts
		// away from the original along the word (measured 2026-10-18).
		d.SetBold(true);
		d.SetName("PtyHeavy");
		if (!s_Twins)
			s_Twins = new map<Widget, Widget>();
		if (s_Twins.Count() > 64)
			PruneTwins();
		s_Twins.Set(t, d);
	}

	static Widget Twin(Widget w)
	{
		if (!w || !s_Twins)
			return null;
		Widget d;
		if (s_Twins.Find(w, d))
			return d;
		return null;
	}

	protected static void PruneTwins()
	{
		map<Widget, Widget> keep = new map<Widget, Widget>();
		foreach (Widget k, Widget v : s_Twins)
		{
			if (k && v)
				keep.Set(k, v);
		}
		s_Twins.Clear();
		foreach (Widget k2, Widget v2 : keep)
		{
			s_Twins.Set(k2, v2);
		}
	}

	// Text of a heavy (faux bold) widget: both copies.
	static void SetHeavyText(TextWidget t, string value)
	{
		if (!t)
			return;
		t.SetText(value);
		TextWidget d = TextWidget.Cast(Twin(t));
		if (d)
			d.SetText(value);
	}

	// Letter spacing in design px (vanilla layouts use "text spacing 2 0").
	static void Track(TextWidget t, float designPx)
	{
		if (!t)
			return;
		int px = INT_UIF_Scale.TextPx(designPx);
		t.SetTextSpacing(px, 0);
	}

	// Tracked capitals of the E design ("Г Р У П П А").
	static TextWidget Caps(Widget parent, float x, float y, float w, float h, string value, float size, int color, int align = INT_UIF_Align.LEFT, bool bold = false)
	{
		string t = INT_PTY_Text.Caps(value);
		return Text(parent, x, y, w, h, t, size, color, align, bold, false);
	}

	// Plain capitals without spacing (titles).
	static TextWidget Upper(Widget parent, float x, float y, float w, float h, string value, float size, int color, int align = INT_UIF_Align.LEFT, bool bold = true, bool big = false)
	{
		string t = INT_PTY_Text.Upper(value);
		return Text(parent, x, y, w, h, t, size, color, align, bold, big);
	}

	static MultilineTextWidget Para(Widget parent, float x, float y, float w, float h, string value, float size, int color)
	{
		string t = INT_PTY_Text.Tr(value);
		return INT_UIF_Draw.MText(parent, x, y, w, h, t, size, color);
	}

	static float CapsWidth(string value, float size)
	{
		string t = INT_PTY_Text.Caps(value);
		return SpacedWidth(t, size);
	}

	// Width of a capitals string as drawn by Text(): measured on stand screenshots 2026-10-18 (1080p).
	//   SDF capitals 0.62 em; faux-bold capitals 0.66 em (16..29 px) / 0.72 em (titles from 30 px).
	// INT_UIF_Draw.TextWidth (0.53 em) is right for mixed-case body text only.
	static float TextW(string text, float size, bool bold)
	{
		string t = INT_PTY_Text.Tr(text);
		float k = 0.62;
		if (bold && size >= 30)
			k = 0.72;
		else if (bold && size >= 16)
			k = 0.66;
		int wide = INT_PTY_Text.WideCount(t);
		int narrow = t.LengthUtf8() - wide;
		return narrow * size * k + wide * size;
	}

	// Font size that makes `text` fit `room` (never below `minSize`).
	static float FitSize(string text, float size, bool bold, float room, float minSize)
	{
		float w = TextW(text, size, bold);
		if (w <= room)
			return size;
		float s = size * room / w;
		if (s < minSize)
			s = minSize;
		return s;
	}

	// `text` cut with an ellipsis so that it fits `room` at `size`.
	static string FitText(string text, float size, bool bold, float room)
	{
		string t = INT_PTY_Text.Tr(text);
		if (TextW(t, size, bold) <= room)
			return t;
		int n = t.LengthUtf8();
		while (n > 1)
		{
			n--;
			string cut = t.SubstringUtf8(0, n) + "…";
			if (TextW(cut, size, bold) <= room)
				return cut;
		}
		return "…";
	}

	// The first line of `text` that fits `room` at `size`, cut at the last space that fits (at a character when one
	// word alone is wider — CJK texts have no spaces); `rest` = what is left, trimmed ("" when all of it fits).
	static string FirstLine(string text, float size, bool bold, float room, out string rest)
	{
		string t = INT_PTY_Text.Tr(text);
		rest = "";
		if (TextW(t, size, bold) <= room)
			return t;
		int n = t.LengthUtf8();
		int fit = 0;
		int lastSpace = -1;
		for (int i = 1; i <= n; i++)
		{
			string head = t.SubstringUtf8(0, i);
			if (TextW(head, size, bold) > room)
				break;
			fit = i;
			string ch = t.SubstringUtf8(i - 1, 1);
			if (ch == " ")
				lastSpace = i - 1;
		}
		int cut = fit;
		string next = "";
		if (fit < n)
			next = t.SubstringUtf8(fit, 1);
		if (next != " " && lastSpace > 0)
			cut = lastSpace;
		if (cut < 1)
			cut = 1;
		string line = t.SubstringUtf8(0, cut);
		string tail = t.SubstringUtf8(cut, n - cut);
		line.TrimInPlace();
		tail.TrimInPlace();
		rest = tail;
		return line;
	}

	// Width of an already spaced capitals string: INT_UIF_Draw.TextWidth counts every character as 0.53 em,
	// but in these strings half of the characters are spaces (measured in game 2026-10-18: capitals ~0.6 em,
	// spaces ~0.33 em at 11 px).
	static float SpacedWidth(string spaced, float size)
	{
		float w = 0;
		int n = spaced.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string ch = spaced.SubstringUtf8(i, 1);
			if (ch == " ")
				w = w + size * 0.33;
			else if (INT_PTY_Text.IsWide(ch))
				w = w + size;
			else
				w = w + size * 0.6;
		}
		return w;
	}

	// Move-mode highlight of an inventory element (w x h design px): light orange tint + 2 px orange outline.
	static Widget MoveFrame(Widget parent, float w, float h)
	{
		Widget f = INT_UIF_Draw.Frame(parent, 0, 0, w, h, "INT_PTY_MoveFrame");
		if (!f)
			return null;
		Fill(f, 0, 0, w, h, ARGB(40, 255, 122, 47));
		Fill(f, 0, 0, w, 2, ORANGE);
		Fill(f, 0, h - 2, w, 2, ORANGE);
		Fill(f, 0, 0, 2, h, ORANGE);
		Fill(f, w - 2, 0, 2, h, ORANGE);
		return f;
	}

	static void SetCaps(TextWidget t, string value)
	{
		if (t)
			t.SetText(INT_PTY_Text.Caps(value));
	}

	static string Num2(int v)
	{
		return INT_PTY_Text.Pad2(v);
	}
}
