// ============================================================================
// VanillaPPMap (our VPP fork) - the «ЭКСПЕДИЦИЯ» look kit, ported 2026-11-12
// from INT_Garage (its CL 3_Game kit, itself a port of INT_Trader's
// INT_TRX_Look / INT_TRX_Fx / INT_TRX_Text) for concept B «ОБЗОР»:
//   INT_VPPX_Look     palette, white-mask textures, diamonds, ticks, outlines,
//                     kit text (faux-bold twins), tracked caps, measuring,
//                     fitting, wrapping, keycap
//   INT_VPPX_Fx       fades / slides that respect every widget's own alpha
//   INT_VPPX_Text     upper case (Cyrillic + European), tracked caps, Pad2,
//                     thousands, #STR translation
//   INT_VPPX_Sfx      interface sound hook (no-op for now, see KIT_API.md)
//   INT_VPPX_TextFit  measured text fit (estimate, then shrink / cut after draw)
// Palette, textures and drawing helpers on INT_UIFramework primitives
// (INT_UIF_Draw / INT_UIF_Scale / INT_UIF_Animator; never modify UIF). All
// sizes are design px of the 1920x1080 canvas (INT_UIF_Draw scales them).
// Textures are white masks (VanillaPPMap/GUI/Textures/INT/int_vppx_*_ca.paa,
// byte copies of the Garage's kit masks), tinted here. Client only: nothing
// runs at file scope, the dedicated server just compiles it.
// Every internal call is qualified (INT_VPPX_Look.X): a bare call binds to any
// class of that name in another mod (Hook()/Hud() trap).
// API reference: mod_intenso_VppMap/design/concepts_expedition/KIT_API.md
// ============================================================================

const string INT_VPPX_TEX = "VanillaPPMap/GUI/Textures/INT/";

class INT_VPPX_Look
{
	static const int ORANGE = 0xFFFF7A2F;
	static const int ORANGE_DIM = 0xFF8A4A26;      // orange text on a dimmed row
	static const int ORANGE_WASH = 0x09FF7A2F;     // selected row wash (in game a low-alpha orange reads many times stronger: 0x14 looked ~35 %)
	static const int WHITE = 0xFFF3EFE8;           // warm off-white titles
	static const int TEXT = 0xFFE6E3DE;
	static const int BODY = 0xFFBDBAB5;
	static const int MUTED = 0xFF9A9894;
	static const int FAINT = 0xFF6F6D6A;
	static const int DARK = 0xFF4E4C4A;
	static const int LINE = 0x1FFFFFFF;            // hairlines
	static const int LINE_STRONG = 0x38FFFFFF;
	static const int GREEN = 0xFF4CD07D;           // positive / safe
	static const int RED = 0xFFE5534B;             // refused / danger
	static const int INK = 0xFF140A02;             // text on orange
	static const int PANEL = 0xFF0C0C0C;           // opaque plates over text
	static const int POPUP = 0xFF101010;

	// --- textures ----------------------------------------------------------------
	// name without extension, e.g. "int_vppx_glow_ca"
	static string TexPath(string name)
	{
		return INT_VPPX_TEX + name + ".paa";
	}

	// icon: check, chevron, close, gear, info, pencil, plus, reset, search, warning
	static string IconPath(string icon)
	{
		return INT_VPPX_TEX + "int_vppx_icon_" + icon + "_ca.paa";
	}

	static ImageWidget Fill(Widget parent, float x, float y, float w, float h, int color)
	{
		string tex = INT_VPPX_Look.TexPath("int_vppx_px_ca");
		return INT_UIF_Draw.Texture(parent, x, y, w, h, tex, color);
	}

	static ImageWidget Tex(Widget parent, float x, float y, float w, float h, string name, int color)
	{
		string tex = INT_VPPX_Look.TexPath(name);
		return INT_UIF_Draw.Texture(parent, x, y, w, h, tex, color);
	}

	static ImageWidget IconAt(Widget parent, float x, float y, float size, string icon, int color)
	{
		string tex = INT_VPPX_Look.IconPath(icon);
		return INT_UIF_Draw.Texture(parent, x, y, size, size, tex, color);
	}

	// Diamond of side `size` centred on (cx, cy). Drawn inside the texture, the
	// widget stays unrotated (a rotated image is clipped to its unrotated rect).
	static const float SQRT2 = 1.41421356;

	static ImageWidget Diamond(Widget parent, float cx, float cy, float size, bool filled, int color)
	{
		float w = size * SQRT2;
		// slot 0 = outline, slot 1 = filled: SetDiamondFilled only switches the
		// slot (no texture load on every hover / repaint)
		string lineTex = INT_VPPX_Look.TexPath("int_vppx_diamond_line_ca");
		string fillTex = INT_VPPX_Look.TexPath("int_vppx_diamond_ca");
		ImageWidget img = INT_UIF_Draw.Texture(parent, cx - w * 0.5, cy - w * 0.5, w, w, lineTex, color);
		if (!img)
			return null;
		img.LoadImageFile(1, fillTex);
		int slot = 0;
		if (filled)
			slot = 1;
		img.SetImage(slot);
		return img;
	}

	// For diamonds made by Diamond() (both slots loaded).
	static void SetDiamondFilled(ImageWidget w, bool filled)
	{
		if (!w)
			return;
		int slot = 0;
		if (filled)
			slot = 1;
		w.SetImage(slot);
	}

	// Corner ticks of a preview frame / dialog (2 px L marks).
	static void Ticks(Widget parent, float x, float y, float w, float h, float len, int color)
	{
		INT_VPPX_Look.Fill(parent, x, y, len, 2, color);
		INT_VPPX_Look.Fill(parent, x, y, 2, len, color);
		INT_VPPX_Look.Fill(parent, x + w - len, y, len, 2, color);
		INT_VPPX_Look.Fill(parent, x + w - 2, y, 2, len, color);
		INT_VPPX_Look.Fill(parent, x, y + h - 2, len, 2, color);
		INT_VPPX_Look.Fill(parent, x, y + h - len, 2, len, color);
		INT_VPPX_Look.Fill(parent, x + w - len, y + h - 2, len, 2, color);
		INT_VPPX_Look.Fill(parent, x + w - 2, y + h - len, 2, len, color);
	}

	// 1 px outline (hairline box).
	static void Outline(Widget parent, float x, float y, float w, float h, int color)
	{
		INT_VPPX_Look.Fill(parent, x, y, w, 1, color);
		INT_VPPX_Look.Fill(parent, x, y + h - 1, w, 1, color);
		INT_VPPX_Look.Fill(parent, x, y, 1, h, color);
		INT_VPPX_Look.Fill(parent, x + w - 1, y, 1, h, color);
	}

	// (map) Own sort on `root` and every widget under it. All widgets are collected first (creation
	// order), then sorted: SetSort moves a widget to the end of its siblings, so sorting while walking
	// GetSibling() stops after the first child; sorting the collected list restores the sibling order.
	static void SortTree(Widget root, int sort)
	{
		if (!root)
			return;
		array<Widget> all = new array<Widget>();
		INT_VPPX_Look.CollectTree(root, all);
		int n = all.Count();
		for (int i = 0; i < n; i++)
		{
			Widget w = all.Get(i);
			if (w)
				w.SetSort(sort);
		}
	}

	// (map) `w` and all widgets under it, parents before children, siblings in order.
	static void CollectTree(Widget w, array<Widget> all)
	{
		if (!w || !all)
			return;
		all.Insert(w);
		Widget c = w.GetChildren();
		while (c)
		{
			INT_VPPX_Look.CollectTree(c, all);
			c = c.GetSibling();
		}
	}

	// (3D look, 2026-11-12) Whole-tree transparency: a frame's alpha never reaches its children and SetAlpha
	// REPLACES the colour's alpha, so every widget's OWN alpha is multiplied (collected first, then changed; a
	// text outline keeps its own colour). Called once after a tree is built - never per frame.
	static void INT_FadeTree(Widget root, float k)
	{
		if (!root)
			return;
		if (k >= 0.999)
			return;
		array<Widget> all = new array<Widget>();
		INT_VPPX_Look.CollectTree(root, all);
		int n = all.Count();
		for (int i = 0; i < n; i++)
		{
			Widget w = all.Get(i);
			if (!w)
				continue;
			float a = w.GetAlpha();
			float na = a * k;
			w.SetAlpha(na);
		}
	}

	// --- text ----------------------------------------------------------------------
	// value: plain or #STR_ key.
	static TextWidget Text(Widget parent, float x, float y, float w, float h, string value, float size, int color, int align = INT_UIF_Align.LEFT, bool bold = false)
	{
		string t = INT_VPPX_Text.Tr(value);
		TextWidget tw = INT_UIF_Draw.Text(parent, x, y, w, h, t, size, color, align);
		if (tw && bold)
		{
			tw.SetBold(true);
			// SDF fonts have no bold face: from 16 px a sibling copy shifted a few %
			// of the size gives a crisp faux bold at every resolution (PARTY 2026-10-18).
			if (size >= 16)
				INT_VPPX_Look.Heavy(tw, parent, x, y, w, h, t, size, color, align);
		}
		return tw;
	}

	protected static ref map<Widget, Widget> s_Twins;

	static void Heavy(TextWidget t, Widget parent, float x, float y, float w, float h, string value, float size, int color, int align)
	{
		if (!t)
			return;
		float dx = size * 0.03;
		if (size < 30)
			dx = size * 0.045;
		TextWidget d = INT_UIF_Draw.Text(parent, x + dx, y, w, h, value, size, color, align);
		if (!d)
			return;
		d.SetBold(true);
		d.SetName("VppxHeavy");
		if (!s_Twins)
			s_Twins = new map<Widget, Widget>();
		if (s_Twins.Count() > 600)
			INT_VPPX_Look.PruneTwins();
		s_Twins.Set(t, d);
	}

	// The faux-bold copy of a text made by Text(..., bold = true), or null.
	static Widget Twin(Widget w)
	{
		if (!w || !s_Twins)
			return null;
		Widget d;
		if (s_Twins.Find(w, d))
			return d;
		return null;
	}

	// (map) Drops twin entries of unlinked widgets now; live ones stay. Call on window teardown.
	static void ForgetDeadTwins()
	{
		if (s_Twins)
			INT_VPPX_Look.PruneTwins();
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

	// Text + colour + visibility of a (maybe faux-bold) text widget, both copies.
	static void SetText(TextWidget t, string value)
	{
		if (!t)
			return;
		t.SetText(value);
		TextWidget d = TextWidget.Cast(INT_VPPX_Look.Twin(t));
		if (d)
			d.SetText(value);
	}

	static void SetColor(TextWidget t, int color)
	{
		if (!t)
			return;
		t.SetColor(color);
		Widget d = INT_VPPX_Look.Twin(t);
		if (d)
			d.SetColor(color);
	}

	static void ShowText(TextWidget t, bool show)
	{
		if (!t)
			return;
		t.Show(show);
		Widget d = INT_VPPX_Look.Twin(t);
		if (d)
			d.Show(show);
	}

	// Tracked capitals ("Г А Р А Ж"): no letter spacing in DayZ text.
	static TextWidget Caps(Widget parent, float x, float y, float w, float h, string value, float size, int color, int align = INT_UIF_Align.LEFT, bool bold = false)
	{
		string t = INT_VPPX_Text.Caps(value);
		return INT_VPPX_Look.Text(parent, x, y, w, h, t, size, color, align, bold);
	}

	static void SetCaps(TextWidget t, string value)
	{
		string c = INT_VPPX_Text.Caps(value);
		INT_VPPX_Look.SetText(t, c);
	}

	static MultilineTextWidget Para(Widget parent, float x, float y, float w, float h, string value, float size, int color)
	{
		string t = INT_VPPX_Text.Tr(value);
		return INT_UIF_Draw.MText(parent, x, y, w, h, t, size, color);
	}

	// Width estimates at 1080p (measured on the PARTY/SKILLS stands 2026-10-18):
	//   mixed-case 0.53 em, SDF capitals 0.62 em, faux-bold capitals 0.66/0.72 em,
	//   digits 0.61 em, spaced capitals: letters 0.6 em + spaces 0.33 em.
	static float TextW(string text, float size, bool bold)
	{
		string t = INT_VPPX_Text.Tr(text);
		float k = 0.53;
		if (bold && size >= 30)
			k = 0.6;
		else if (bold)
			k = 0.57;
		int wide = INT_VPPX_Text.WideCount(t);
		int narrow = t.LengthUtf8() - wide;
		return narrow * size * k + wide * size;
	}

	static float CapsW(string text, float size)
	{
		string t = INT_VPPX_Text.Caps(text);
		return INT_VPPX_Look.SpacedWidth(t, size);
	}

	static float SpacedWidth(string spaced, float size)
	{
		float w = 0;
		int n = spaced.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string ch = spaced.SubstringUtf8(i, 1);
			if (ch == " ")
				w = w + size * 0.33;
			else if (INT_VPPX_Text.IsWide(ch))
				w = w + size;
			else
				w = w + size * 0.6;
		}
		return w;
	}

	// Width of a number string: Metron digits run ~0.6 em (was INT_TraderText.NumberWidth).
	static float NumW(string digits, float size)
	{
		int n = digits.LengthUtf8();
		return n * size * 0.6;
	}

	// `text` cut with an ellipsis so that it fits `room` at `size`.
	static string FitText(string text, float size, bool bold, float room)
	{
		string t = INT_VPPX_Text.Tr(text);
		if (INT_VPPX_Look.TextW(t, size, bold) <= room)
			return t;
		int n = t.LengthUtf8();
		while (n > 1)
		{
			n--;
			string cut = t.SubstringUtf8(0, n) + "…";
			if (INT_VPPX_Look.TextW(cut, size, bold) <= room)
				return cut;
		}
		return "…";
	}

	// Tracked capitals of the text that fit the room, cut with " …" (plain text in, caps out).
	static string FitCaps(string text, float size, bool bold, float room)
	{
		string t = INT_VPPX_Text.Tr(text);
		string caps = INT_VPPX_Text.Caps(t);
		if (INT_VPPX_Look.Measure(caps, size, bold) * 1.02 <= room)
			return caps;
		int n = t.LengthUtf8();
		while (n > 1)
		{
			n--;
			string cut = t.SubstringUtf8(0, n);
			cut.TrimInPlace();
			string c = INT_VPPX_Text.Caps(cut) + " …";
			if (INT_VPPX_Look.Measure(c, size, bold) * 1.02 <= room)
				return c;
		}
		return "…";
	}

	// Font size that makes `text` fit `room` (never below `minSize`).
	static float FitSize(string text, float size, bool bold, float room, float minSize)
	{
		float w = INT_VPPX_Look.TextW(text, size, bold);
		if (w <= room)
			return size;
		float s = size * room / w;
		if (s < minSize)
			s = minSize;
		return s;
	}

	// --- measuring (PARTY INT_PTY_MD, calibrated on stand screenshots 2026-10-23, 1080p SDF font):
	// capitals 0.65 em, lowercase 0.575 em, digits 0.58 em, space/narrow 0.30 em; bold x1.05.
	static const string LOWER_CHARS = "абвгдеёжзийклмнопрстуфхцчшщъыьэюяabcdefghjkmnopqrsuvwxyz";
	static const string NARROW_CHARS = " .,:!|ilfrt()[]/-";
	static const string DIGIT_CHARS = "0123456789";
	static const string WIDE_LOWER = "дждшщюымфw";

	static float Measure(string text, float size, bool bold = false)
	{
		string t = INT_VPPX_Text.Tr(text);
		int n = t.LengthUtf8();
		float em = 0;
		for (int i = 0; i < n; i++)
		{
			string ch = t.SubstringUtf8(i, 1);
			em = em + INT_VPPX_Look.CharEm(ch);
		}
		float glyphK = INT_VPPX_Look.GlyphRound(size);
		float w = em * size * glyphK;
		if (bold)
			w = w * 1.05;
		return w;
	}

	static float CapsMeasure(string text, float size, bool bold = false)
	{
		string c = INT_VPPX_Text.Caps(text);
		return INT_VPPX_Look.Measure(c, size, bold);
	}

	// On 4:3 / 5:4 the glyph size is rounded to whole pixels: small text is a few % wider.
	static float GlyphRound(float size)
	{
		int sw = INT_UIF_Scale.ScreenW();
		int sh = INT_UIF_Scale.ScreenH();
		if (sh <= 0 || sw <= 0 || size <= 0)
			return 1;
		float k = INT_UIF_Scale.Factor() * 1080.0 / sh;
		float want = size * k;
		if (want <= 0)
			return 1;
		int px = INT_UIF_Scale.TextPx(size);
		float pxf = px;
		float r = pxf / want;
		if (r < 1)
			r = 1;
		return r;
	}

	static float CharEm(string ch)
	{
		if (ch == " ")
			return 0.30;
		if (ch == "m")
			return 0.87;
		if (ch == "t" || ch == "f")
			return 0.38;
		if (DIGIT_CHARS.IndexOf(ch) >= 0)
			return 0.58;
		if (WIDE_LOWER.IndexOf(ch) >= 0)
			return 0.68;
		if (LOWER_CHARS.IndexOf(ch) >= 0)
			return 0.575;
		if (NARROW_CHARS.IndexOf(ch) >= 0)
			return 0.30;
		if (ch == "—" || ch == "Ш" || ch == "Щ" || ch == "Ж" || ch == "Ю" || ch == "М" || ch == "M" || ch == "W")
			return 0.78;
		if (INT_VPPX_Text.IsWide(ch))
			return 1.0;
		return 0.65;
	}

	// Glyphs past a TextWidget's box are cut: boxes get spare room, positions use the measure.
	static float Room(float measured)
	{
		return measured + 48;
	}

	// Keycap: 1 px border, 13 px caps; returns its width.
	static float Keycap(Widget p, float x, float y, string key, float h = 26, int edge = 0x6BFFFFFF, int textColor = 0xFFE6E3DE)
	{
		string k = INT_VPPX_Text.Upper(key);
		float kw = INT_VPPX_Look.Measure(k, 13, true) + 18;
		if (kw < 30)
			kw = 30;
		INT_VPPX_Look.Fill(p, x, y, kw, h, 0x99060606);
		INT_VPPX_Look.Outline(p, x, y, kw, h, edge);
		INT_VPPX_Look.Text(p, x, y, kw, h, k, 13, textColor, INT_UIF_Align.CENTER, false);
		return kw;
	}

	// The first line of `text` that fits `room`, cut at the last space; `rest` = the remainder.
	static string FirstLine(string text, float size, float room, out string rest)
	{
		string t = INT_VPPX_Text.Tr(text);
		rest = "";
		if (INT_VPPX_Look.Measure(t, size) <= room)
			return t;
		int n = t.LengthUtf8();
		int fit = 0;
		int lastSpace = -1;
		for (int i = 1; i <= n; i++)
		{
			string head = t.SubstringUtf8(0, i);
			if (INT_VPPX_Look.Measure(head, size) > room)
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

	// `text` wrapped into `lines` (cleared first) by the measure, at most maxLines.
	static void Wrap(string text, float size, float room, array<string> lines, int maxLines = 12)
	{
		lines.Clear();
		string rest = INT_VPPX_Text.Tr(text);
		int guard = 0;
		while (rest != "" && guard < maxLines)
		{
			string tail;
			string line = INT_VPPX_Look.FirstLine(rest, size, room, tail);
			lines.Insert(line);
			rest = tail;
			guard++;
		}
	}
}

// Block entrances / exits. A frame's alpha does not reach its children
// (measured 2026-10-22): a fade walks the subtree and fades every widget to
// the alpha it was built with (PARTY INT_PTY_MenuFx, via INT_TRX_Fx).
class INT_VPPX_Fx
{
	// a widget with this name is skipped by FadeIn (keeps its own alpha)
	static const string NO_FADE = "VppxNoFade";
	protected static ref map<Widget, float> s_Home;

	static void ForgetHomes()
	{
		if (s_Home)
			s_Home.Clear();
	}

	static void FadeIn(Widget root, float delay, float dur)
	{
		if (!root)
			return;
		INT_VPPX_Fx.FadeInOne(root, delay, dur);
		Widget c = root.GetChildren();
		while (c)
		{
			INT_VPPX_Fx.FadeIn(c, delay, dur);
			c = c.GetSibling();
		}
	}

	protected static void FadeInOne(Widget w, float delay, float dur)
	{
		if (w.GetName() == NO_FADE)
			return;
		if (!s_Home)
			s_Home = new map<Widget, float>();
		// Never clear live entries: a widget faded in again mid-fade would read its
		// current (near 0) alpha as home and stay invisible. Drop dead keys only.
		if (s_Home.Count() > 4000)
			INT_VPPX_Fx.PruneHomes();
		float home;
		if (!s_Home.Find(w, home))
		{
			home = w.GetAlpha();
			s_Home.Set(w, home);
		}
		INT_UIF_Animator.Get().Cancel(w, INT_UIF_TweenProp.ALPHA);
		if (home <= 0.004)
			return;
		w.SetAlpha(0);
		INT_UIF_Tween t = INT_UIF_Animator.Fade(w, home, dur, INT_UIF_Ease.OUT_CUBIC);
		if (t)
			t.From(0).Delay(delay);
	}

	// (map) Drops the saved alpha of unlinked widgets now; live ones stay (unlike ForgetHomes, safe
	// while another kit UI - HUD, 3D markers - is fading). Call on window teardown.
	static void ForgetDeadHomes()
	{
		if (s_Home)
			INT_VPPX_Fx.PruneHomes();
	}

	protected static void PruneHomes()
	{
		map<Widget, float> keep = new map<Widget, float>();
		foreach (Widget k, float v : s_Home)
		{
			if (k)
				keep.Set(k, v);
		}
		s_Home.Clear();
		foreach (Widget k2, float v2 : keep)
		{
			s_Home.Set(k2, v2);
		}
	}

	// (No Dim: SetAlpha replaces a colour's alpha, so it turns translucent
	// hairlines opaque. Hide blocks instead.)

	// Slides a block from (dx, dy) design px back to its place and fades it in.
	static void BlockIn(Widget w, float dx, float dy, float delay, float dur)
	{
		if (!w)
			return;
		float x;
		float y;
		w.GetPos(x, y);
		INT_UIF_Animator.Get().Cancel(w, INT_UIF_TweenProp.POS_X);
		INT_UIF_Animator.Get().Cancel(w, INT_UIF_TweenProp.POS_Y);
		float sdx = INT_UIF_Draw.S(dx);
		float sdy = INT_UIF_Draw.S(dy);
		w.SetPos(x + sdx, y + sdy);
		if (dx != 0)
		{
			INT_UIF_Tween mx = INT_UIF_Animator.MoveX(w, x, dur, INT_UIF_Ease.OUT_CUBIC);
			if (mx)
				mx.Delay(delay);
		}
		if (dy != 0)
		{
			INT_UIF_Tween my = INT_UIF_Animator.MoveY(w, y, dur, INT_UIF_Ease.OUT_CUBIC);
			if (my)
				my.Delay(delay);
		}
		INT_VPPX_Fx.FadeIn(w, delay, dur);
	}

	// BlockIn from a fixed home (design px in the parent): a block revealed
	// again before its last slide ended would otherwise take the shifted
	// position as its home and drift.
	static void BlockInAt(Widget w, float homeX, float homeY, float dx, float dy, float delay, float dur)
	{
		if (!w)
			return;
		INT_UIF_Animator.Get().Cancel(w, INT_UIF_TweenProp.POS_X);
		INT_UIF_Animator.Get().Cancel(w, INT_UIF_TweenProp.POS_Y);
		INT_UIF_Draw.Move(w, homeX, homeY);
		INT_VPPX_Fx.BlockIn(w, dx, dy, delay, dur);
	}

	// Grows the widget's width from 0 to its current width.
	static void GrowW(Widget w, float delay, float dur)
	{
		if (!w)
			return;
		float sw;
		float sh;
		w.GetSize(sw, sh);
		w.SetSize(0, sh);
		INT_UIF_Tween t = INT_UIF_Animator.ResizeW(w, sw, dur, INT_UIF_Ease.OUT_EXPO);
		if (t)
			t.Delay(delay);
	}

	// Grows the widget's height from 0 to its current height.
	static void GrowH(Widget w, float delay, float dur)
	{
		if (!w)
			return;
		float sw;
		float sh;
		w.GetSize(sw, sh);
		w.SetSize(sw, 0);
		INT_UIF_Tween t = INT_UIF_Animator.ResizeH(w, sh, dur, INT_UIF_Ease.IN_OUT_CUBIC);
		if (t)
			t.Delay(delay);
	}
}

class INT_VPPX_Text
{
	protected static ref array<string> s_Lower;
	protected static ref array<string> s_Upper;

	// Upper case: ASCII by ToUpper(), Cyrillic and European letters by table.
	static string Upper(string value)
	{
		string r = INT_VPPX_Text.Tr(value);
		r.ToUpper();
		INT_VPPX_Text.EnsureCaseTable();
		int n = s_Lower.Count();
		for (int i = 0; i < n; i++)
		{
			string lo = s_Lower.Get(i);
			string up = s_Upper.Get(i);
			r.Replace(lo, up);
		}
		return r;
	}

	protected static void EnsureCaseTable()
	{
		if (s_Lower)
			return;
		s_Lower = new array<string>();
		s_Upper = new array<string>();
		string lo = "абвгдеёжзийклмнопрстуфхцчшщъыьэюяàáâãäåæçèéêëìíîïñòóôõöøùúûüýÿąćęłńśźżčďěňřšťůžőűğışі";
		string up = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÒÓÔÕÖØÙÚÛÜÝŸĄĆĘŁŃŚŹŻČĎĚŇŘŠŤŮŽŐŰĞIŞІ";
		int n = lo.LengthUtf8();
		int m = up.LengthUtf8();
		if (m < n)
			n = m;
		for (int i = 0; i < n; i++)
		{
			s_Lower.Insert(lo.SubstringUtf8(i, 1));
			s_Upper.Insert(up.SubstringUtf8(i, 1));
		}
	}

	// "ГАРАЖ" -> "Г А Р А Ж"; a word gap becomes three spaces.
	static string Spaced(string value)
	{
		string t = INT_VPPX_Text.Tr(value);
		string r = "";
		int n = t.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string ch = t.SubstringUtf8(i, 1);
			if (i > 0)
				r = r + " ";
			if (ch == " ")
				r = r + " ";
			else
				r = r + ch;
		}
		return r;
	}

	// Upper + Spaced (translates #STR_ first).
	static string Caps(string value)
	{
		string u = INT_VPPX_Text.Upper(value);
		return INT_VPPX_Text.Spaced(u);
	}

	// 7 -> "07"
	static string Pad2(int v)
	{
		if (v >= 0 && v < 10)
			return "0" + v.ToString();
		return v.ToString();
	}

	// 1234567 -> "1 234 567" (the trader's Money() without the currency).
	static string Thousands(int value)
	{
		int v = Math.AbsInt(value);
		string digits = v.ToString();
		string res = "";
		int n = digits.Length();
		for (int i = 0; i < n; i++)
		{
			int left = n - i;
			int rem = left % 3;
			if (i > 0 && rem == 0)
				res = res + " ";
			res = res + digits.Get(i);
		}
		if (value < 0)
			res = "-" + res;
		return res;
	}

	static const string NARROW3 = "—…„“”‘’‚–•→←№€™×·";

	// A 3-byte UTF-8 glyph that is not punctuation (CJK etc.): one em wide.
	static bool IsWide(string ch)
	{
		if (ch.Length() < 3)
			return false;
		return NARROW3.IndexOf(ch) < 0;
	}

	static int WideCount(string t)
	{
		int n = t.LengthUtf8();
		if (t.Length() == n)
			return 0;
		int wide = 0;
		for (int i = 0; i < n; i++)
		{
			string ch = t.SubstringUtf8(i, 1);
			if (INT_VPPX_Text.IsWide(ch))
				wide++;
		}
		return wide;
	}

	// "#STR_..." -> translated text; anything else unchanged.
	static string Tr(string value)
	{
		if (value.IndexOf("#STR_") == 0)
			return Widget.TranslateString(value);
		return value;
	}
}

// ---------------------------------------------------------------------------------------------------------
// Interface sound kinds of the kit. The first nine keep the Garage's order; HOVER..DONE were added for the
// map (2026-11-12). Each kind plays config.cpp CfgSoundSets INT_VPP_UI_<Kind>_SoundSet (sounds/int_vpp_ui_*.ogg,
// the DayZavr launcher's interface sounds). Which kind for which map event: design/concepts_expedition/SOUND_PLAN.md.
enum INT_VPPX_SfxKind
{
	CLICK,		// plain button / icon button / stepper step / list row action
	TAB,		// tab switch
	TOGGLE,		// check, toggle, layer chip, 3D switch, segment pick
	SELECT,		// pick in a list / grid / dropdown (marker, colour, icon, zone)
	CONFIRM,	// save / yes / point added / the server confirmed
	ERROR,		// refused: invalid input, server error, no answer
	OPEN,		// window / editor opens
	CLOSE,		// window / card / editor closes, cancel, Esc step
	NOTIFY,		// soft notice (route point reached)
	HOVER,		// mouse enters a control (very quiet)
	TICK,		// slider detent, ruler point, fine steps (very quiet)
	DENY,		// action not allowed (limit reached, not an admin, delete refused)
	WARN,		// destructive confirm band opens (delete marker / zone, clear route)
	DONE		// a whole task finished (last route point reached = route complete)
}

// Interface sounds of the map (2D, UI mixer channel). The same proven chain as INT_Garage (INT_PK_UiSfx),
// INT_P2P_Trading and INT_PartyMode on 1.30: SoundParams -> SoundObjectBuilder -> SoundObject.SetKind(WAVEUI)
// -> AbstractSoundScene.Play2D -> AbstractWave.Play. Nothing goes through SEffectManager, so no EffectSound
// can stay registered there (the @Notifications / Inedia leak class does not apply).
// Our only references are the AbstractWaves in a small ring (INT_KEEP_WAVES): a slot is freed by the wave's own
// Ended / Stopped event, or overwritten by a newer sound when all slots are busy, or dropped after INT_HOLD_MAX.
// INT_Live() = waves started and not yet ended (self-test: back to 0 within 2 s after a burst).
// Rules: client only (no-op on a dedicated server / headless client, without a mission or a sound scene);
// per-kind cooldown (a held key or a fast list does not spam); any two UI sounds >= INT_ANY_GAP apart;
// HOVER / TICK never block other sounds; ERROR / DENY / WARN / NOTIFY / DONE are never blocked by a click.
// Global switch INT_SetEnabled(false) mutes everything. Controls play no sound themselves: the window calls
// INT_VPPX_Sfx.Play from the controls' invokers.
class INT_VPPX_Sfx
{
	static const float INT_ANY_GAP = 0.02;		// s, no two UI sounds closer (one click firing several handlers)
	static const int INT_KEEP_WAVES = 16;		// waves held at once (the longest clip is 0.83 s)
	static const float INT_HOLD_MAX = 6.0;		// s, a held wave whose end event never came is dropped after this
	// sound groups (s_INT_Group)
	static const int INT_GROUP_UI = 0;			// blocked by the any-gap, sets it
	static const int INT_GROUP_LIGHT = 1;		// blocked by the any-gap, never sets it (hover / tick)
	static const int INT_GROUP_ALERT = 2;		// never blocked by the any-gap, sets it (outcome / notices)

	// Stored inverted so the default (on) does not depend on a static initializer.
	protected static bool s_INT_Muted;
	protected static ref array<string> s_INT_Sets;
	protected static ref array<float> s_INT_Gaps;
	protected static ref array<int> s_INT_Group;
	protected static ref array<float> s_INT_LastAt;
	protected static ref array<bool> s_INT_Valid;
	protected static float s_INT_LastAny;
	protected static ref array<AbstractWave> s_INT_Waves;
	protected static ref array<float> s_INT_HeldAt;
	protected static int s_INT_Played;

	// kind: INT_VPPX_SfxKind.*
	static void Play(int kind)
	{
		if (s_INT_Muted)
			return;
		if (!INT_VPPX_Sfx.INT_CanPlay())
			return;
		INT_VPPX_Sfx.INT_EnsureInit();
		if (kind < 0 || kind >= s_INT_Sets.Count())
			return;
		if (!s_INT_Valid.Get(kind))
			return;

		float now = GetGame().GetTickTime();
		float last = s_INT_LastAt.Get(kind);
		float gap = s_INT_Gaps.Get(kind);
		if (now >= last && now - last < gap)
			return;
		int group = s_INT_Group.Get(kind);
		if (group != INT_GROUP_ALERT)
		{
			if (now >= s_INT_LastAny && now - s_INT_LastAny < INT_ANY_GAP)
				return;
		}

		AbstractSoundScene scene = GetGame().GetSoundScene();
		if (!scene)
			return;
		string setName = s_INT_Sets.Get(kind);
		SoundParams soundParams = new SoundParams(setName);
		if (!soundParams.IsValid())
		{
			s_INT_Valid.Set(kind, false);
			Print("[INT_VPP] interface sound set is not valid: " + setName);
			return;
		}
		SoundObjectBuilder soundBuilder = new SoundObjectBuilder(soundParams);
		SoundObject soundObject = soundBuilder.BuildSoundObject();
		if (!soundObject)
			return;
		soundObject.SetKind(WaveKind.WAVEUI);
		AbstractWave wave = scene.Play2D(soundObject, soundBuilder);
		if (!wave)
			return;
		INT_VPPX_Sfx.INT_Hold(wave, now);
		wave.Loop(false);
		wave.Play();

		s_INT_LastAt.Set(kind, now);
		if (group != INT_GROUP_LIGHT)
			s_INT_LastAny = now;
		s_INT_Played++;
	}

	// Global switch (a player / server option later). Off = Play does nothing.
	static void INT_SetEnabled(bool on)
	{
		bool muted = true;
		if (on)
			muted = false;
		s_INT_Muted = muted;
	}

	static bool INT_IsEnabled()
	{
		if (s_INT_Muted)
			return false;
		return true;
	}

	// Our sounds that started and have not ended yet (held in the ring, younger than INT_HOLD_MAX).
	// Self-test: play a burst, INT_Live() > 0 right after, == 0 within 2 s.
	static int INT_Live()
	{
		if (!s_INT_Waves || !GetGame())
			return 0;
		float now = GetGame().GetTickTime();
		int live = 0;
		int count = s_INT_Waves.Count();
		float heldAt;
		for (int i = 0; i < count; i++)
		{
			if (!s_INT_Waves.Get(i))
				continue;
			heldAt = s_INT_HeldAt.Get(i);
			if (now >= heldAt && now - heldAt < INT_HOLD_MAX)
				live++;
		}
		return live;
	}

	// Sounds started since the game started (self-test: a burst of N distinct kinds adds N).
	static int INT_Played()
	{
		return s_INT_Played;
	}

	// The CfgSoundSets name of a kind ("" for an unknown kind).
	static string INT_SetOf(int kind)
	{
		INT_VPPX_Sfx.INT_EnsureInit();
		if (kind < 0 || kind >= s_INT_Sets.Count())
			return "";
		return s_INT_Sets.Get(kind);
	}

	// Self-test: the kind's CfgSoundSets entry exists (config loaded, sample path valid as far as SoundParams
	// knows), checked without playing anything. Client only (false on a dedicated server / headless client).
	static bool INT_SetExists(int kind)
	{
		if (!GetGame() || GetGame().IsHeadlessOrDedicatedServer())
			return false;
		string setName = INT_VPPX_Sfx.INT_SetOf(kind);
		if (setName == "")
			return false;
		SoundParams soundParams = new SoundParams(setName);
		if (!soundParams.IsValid())
			return false;
		return true;
	}

	// Client with a running mission and a sound scene; never the dedicated server or a headless client.
	static bool INT_CanPlay()
	{
		if (!GetGame())
			return false;
		if (GetGame().IsHeadlessOrDedicatedServer())
			return false;
		if (!GetGame().GetMission())
			return false;
		if (!GetGame().GetSoundScene())
			return false;
		return true;
	}

	// Wave events (AbstractWave.OnEnd / OnStop invoke with the wave): free its slot. Idempotent.
	static void INT_OnWaveDone(AbstractWave wave)
	{
		if (!wave || !s_INT_Waves)
			return;
		int count = s_INT_Waves.Count();
		for (int i = 0; i < count; i++)
		{
			if (s_INT_Waves.Get(i) == wave)
			{
				s_INT_Waves.Set(i, null);
				s_INT_HeldAt.Set(i, -1);
			}
		}
	}

	// Keeps the wave referenced until its end event (subscribed before Play, so a very short clip cannot end
	// before we hold it). A free slot first, then one past INT_HOLD_MAX, else the oldest is overwritten.
	protected static void INT_Hold(AbstractWave wave, float now)
	{
		AbstractWaveEvents events = wave.GetEvents();
		if (events)
		{
			events.Event_OnSoundWaveEnded.Insert(INT_VPPX_Sfx.INT_OnWaveDone);
			events.Event_OnSoundWaveStopped.Insert(INT_VPPX_Sfx.INT_OnWaveDone);
		}
		int slot = -1;
		int oldest = 0;
		float oldestAt = 1000000000.0;
		float heldAt;
		int count = s_INT_Waves.Count();
		for (int i = 0; i < count; i++)
		{
			if (!s_INT_Waves.Get(i))
			{
				slot = i;
				break;
			}
			heldAt = s_INT_HeldAt.Get(i);
			if (now < heldAt || now - heldAt >= INT_HOLD_MAX)
			{
				slot = i;
				break;
			}
			if (heldAt < oldestAt)
			{
				oldestAt = heldAt;
				oldest = i;
			}
		}
		if (slot < 0)
			slot = oldest;
		s_INT_Waves.Set(slot, wave);
		s_INT_HeldAt.Set(slot, now);
	}

	protected static void INT_AddKind(string setName, float gap, int group)
	{
		s_INT_Sets.Insert(setName);
		s_INT_Gaps.Insert(gap);
		s_INT_Group.Insert(group);
		s_INT_LastAt.Insert(-100);
		s_INT_Valid.Insert(true);
	}

	// Order = INT_VPPX_SfxKind. Gaps: same kind again no sooner than this (s).
	protected static void INT_EnsureInit()
	{
		if (s_INT_Sets)
			return;
		s_INT_Sets = new array<string>();
		s_INT_Gaps = new array<float>();
		s_INT_Group = new array<int>();
		s_INT_LastAt = new array<float>();
		s_INT_Valid = new array<bool>();
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Click_SoundSet", 0.045, INT_GROUP_UI);		// CLICK
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Tab_SoundSet", 0.045, INT_GROUP_UI);		// TAB
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Toggle_SoundSet", 0.045, INT_GROUP_UI);	// TOGGLE
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Select_SoundSet", 0.045, INT_GROUP_UI);	// SELECT
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Confirm_SoundSet", 0.08, INT_GROUP_UI);	// CONFIRM
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Error_SoundSet", 0.15, INT_GROUP_ALERT);	// ERROR
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Open_SoundSet", 0.1, INT_GROUP_UI);		// OPEN
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Close_SoundSet", 0.1, INT_GROUP_UI);		// CLOSE
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Notify_SoundSet", 0.25, INT_GROUP_ALERT);	// NOTIFY
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Hover_SoundSet", 0.08, INT_GROUP_LIGHT);	// HOVER
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Tick_SoundSet", 0.04, INT_GROUP_LIGHT);	// TICK
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Deny_SoundSet", 0.1, INT_GROUP_ALERT);		// DENY
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Warn_SoundSet", 0.25, INT_GROUP_ALERT);	// WARN
		INT_VPPX_Sfx.INT_AddKind("INT_VPP_UI_Done_SoundSet", 0.4, INT_GROUP_ALERT);		// DONE
		s_INT_LastAny = -100;
		s_INT_Waves = new array<AbstractWave>();
		s_INT_HeldAt = new array<float>();
		for (int w = 0; w < INT_KEEP_WAVES; w++)
		{
			s_INT_Waves.Insert(null);
			s_INT_HeldAt.Insert(-1);
		}
	}
}

// ---------------------------------------------------------------------------------------------------------
// Text that fits at every resolution (port of the Garage's measured text fit). Glyphs do not scale exactly
// with the boxes: at 2560x1440 a value that fitted at 1080p was cut (owner report 2026-10-17). Two tools:
//   Set()     a text in a box of maxW design px: shown at once by the estimate, then measured after it is
//             drawn (GetTextSize / Factor); while it overflows the font goes down one step, at minSize the
//             text is cut with an ellipsis.
//   s_Ratio   the estimate's calibration for this resolution: a probe text is measured once per window
//             (Width multiplies by it).
// The window calls Calibrate(root) when it opens, Tick() every frame and Clear() when it closes. Only for
// NON-twinned widgets (a bold kit text >= 16 px has a faux-bold twin: use INT_VPPX_Look.FitText/SetText).
class INT_VPPX_FitJob
{
	TextWidget m_INT_W;
	string m_INT_Text;
	string m_INT_Shown;
	float m_INT_Size;
	float m_INT_MinSize;
	float m_INT_MaxW;
	bool m_INT_Bold;
	int m_INT_Tries;
}

class INT_VPPX_TextFit
{
	static float s_Ratio = 1.08;			// measured / estimated width of regular text here
	protected static bool s_Calibrated;
	protected static bool s_JustCalibrated;
	protected static TextWidget s_Probe;
	protected static int s_ProbeTries;
	protected static ref array<ref INT_VPPX_FitJob> s_Jobs = new array<ref INT_VPPX_FitJob>();
	static const string PROBE = "Автомобиль Hatchback 0123 — 17.10";

	// Estimated width in design px: Latin / Cyrillic ~0.56 em, CJK and other 3-byte UTF-8 characters ~1 em,
	// times the ratio measured for this resolution (s_Ratio).
	static float Width(string text, float size)
	{
		float w = 0;
		int n = text.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string ch = text.SubstringUtf8(i, 1);
			if (ch.Length() >= 3)
				w = w + size;
			else
				w = w + size * 0.56;
		}
		return w * INT_VPPX_TextFit.s_Ratio;
	}

	// Text cut to a width with an ellipsis (text widgets do not clip).
	static string Fit(string text, float size, float maxW)
	{
		if (INT_VPPX_TextFit.Width(text, size) <= maxW)
			return text;
		int n = text.LengthUtf8();
		while (n > 1)
		{
			n--;
			string cut = text.SubstringUtf8(0, n) + "…";
			if (INT_VPPX_TextFit.Width(cut, size) <= maxW)
				return cut;
		}
		return "…";
	}

	static float Est(string text, float size, bool bold)
	{
		float w = INT_VPPX_TextFit.Width(text, size);
		if (bold)
			w = w * 1.08;
		return w;
	}

	// text: plain (translate #STR first with INT_VPPX_Text.Tr); size / minSize / maxW: design px.
	static void Set(TextWidget w, string text, float size, float maxW, float minSize, bool bold)
	{
		if (!w)
			return;
		float s = size;
		while (s > minSize && INT_VPPX_TextFit.Est(text, s, bold) > maxW)
			s = s - 1;
		string shown = text;
		if (INT_VPPX_TextFit.Est(shown, s, bold) > maxW)
		{
			float fitSize = s * INT_VPPX_TextFit.BoldRatio(bold);
			shown = INT_VPPX_TextFit.Fit(text, fitSize, maxW);
		}
		int px = INT_UIF_Scale.TextPx(s);
		w.SetTextExactSize(px);
		w.SetText(shown);
		INT_VPPX_FitJob j = INT_VPPX_TextFit.JobOf(w);
		if (!j)
		{
			j = new INT_VPPX_FitJob();
			j.m_INT_W = w;
			s_Jobs.Insert(j);
		}
		j.m_INT_Text = text;
		j.m_INT_Shown = shown;
		j.m_INT_Size = s;
		j.m_INT_MinSize = minSize;
		j.m_INT_MaxW = maxW;
		j.m_INT_Bold = bold;
		j.m_INT_Tries = 0;
	}

	protected static float BoldRatio(bool bold)
	{
		if (bold)
			return 1.08;
		return 1;
	}

	protected static INT_VPPX_FitJob JobOf(TextWidget w)
	{
		foreach (INT_VPPX_FitJob j : s_Jobs)
		{
			if (j.m_INT_W == w)
				return j;
		}
		return null;
	}

	// Width in design px the widget really draws (0 = not drawn yet).
	static float Measured(TextWidget w, bool bold)
	{
		if (!w)
			return 0;
		w.Update();
		int mw;
		int mh;
		w.GetTextSize(mw, mh);
		if (mw <= 0)
			return 0;
		float f = INT_UIF_Scale.Factor();
		if (f <= 0)
			f = 1;
		float d = mw / f;
		if (bold)
			d = d * 1.08;
		return d + 1;
	}

	// A new window: measure the probe again (the resolution may have changed).
	static void Calibrate(Widget parent)
	{
		s_Calibrated = false;
		s_ProbeTries = 0;
		if (s_Probe)
			s_Probe.Unlink();
		int clear = ARGB(0, 0, 0, 0);
		s_Probe = INT_UIF_Draw.Text(parent, 0, 0, 900, 30, PROBE, 14, clear);
	}

	// True once after the calibration finished (the caller re-binds what it laid out by the estimate).
	static bool TakeCalibrated()
	{
		bool b = s_JustCalibrated;
		s_JustCalibrated = false;
		return b;
	}

	static void Tick()
	{
		if (!s_Calibrated && s_Probe)
		{
			float pm = INT_VPPX_TextFit.Measured(s_Probe, false);
			s_ProbeTries++;
			if (pm > 0)
			{
				float est = INT_VPPX_TextFit.Width(PROBE, 14) / s_Ratio;
				if (est > 0)
					s_Ratio = Math.Clamp(pm / est, 0.8, 1.6);
				s_Calibrated = true;
				s_JustCalibrated = true;
				s_Probe.Unlink();
				s_Probe = null;
			}
			else if (s_ProbeTries > 60)
			{
				s_Calibrated = true;
				s_Probe.Unlink();
				s_Probe = null;
			}
		}
		for (int i = s_Jobs.Count() - 1; i >= 0; i--)
		{
			INT_VPPX_FitJob j = s_Jobs.Get(i);
			if (!j.m_INT_W || !j.m_INT_W.IsVisibleHierarchy())
			{
				if (!j.m_INT_W)
					s_Jobs.Remove(i);
				continue;
			}
			float real = INT_VPPX_TextFit.Measured(j.m_INT_W, j.m_INT_Bold);
			if (real <= 0)
			{
				j.m_INT_Tries++;
				if (j.m_INT_Tries > 60)
					s_Jobs.Remove(i);
				continue;
			}
			if (real <= j.m_INT_MaxW)
			{
				s_Jobs.Remove(i);
				continue;
			}
			if (j.m_INT_Size > j.m_INT_MinSize)
			{
				j.m_INT_Size = j.m_INT_Size - 1;
				int spx = INT_UIF_Scale.TextPx(j.m_INT_Size);
				j.m_INT_W.SetTextExactSize(spx);
				continue;
			}
			// smallest size and still too wide: cut by the measured overflow
			int n = j.m_INT_Shown.LengthUtf8();
			if (j.m_INT_Shown != j.m_INT_Text)
				n = n - 1;
			int keep = Math.Floor(n * j.m_INT_MaxW / real) - 1;
			if (keep < 1 || j.m_INT_Tries > 8)
			{
				s_Jobs.Remove(i);
				continue;
			}
			string head = j.m_INT_Text.SubstringUtf8(0, keep);
			string shown = head + "…";
			j.m_INT_Shown = shown;
			j.m_INT_W.SetText(shown);
			j.m_INT_Tries++;
		}
	}

	static void Clear()
	{
		s_Jobs.Clear();
		if (s_Probe)
			s_Probe.Unlink();
		s_Probe = null;
	}
}
