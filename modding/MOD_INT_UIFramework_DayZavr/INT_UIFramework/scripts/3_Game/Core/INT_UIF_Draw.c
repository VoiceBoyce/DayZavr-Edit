// ============================================================================
// INT_UIFramework - draw primitives (v2).
//
// Everything is placed in exact pixels on the 1920x1080 design canvas and
// scaled by INT_UIF_Scale, so a screen written once looks the same at every
// resolution. v2 components and consumer screens are composed from these, the
// same way tools/mock_*.py compose the approved design mockups (iuf_kit.py has
// the matching Python blocks).
//
// Every primitive is a small generated layout created through
// INT_UIF_Widgets.Create (literal CreateWidgets paths, see INT_UIF_LayoutTable).
// ============================================================================

class INT_UIF_Draw
{
	// Design pixels -> screen pixels.
	static float S(float v)
	{
		return INT_UIF_Scale.Px(v);
	}

	static void Place(Widget w, float x, float y, float width, float height)
	{
		if (!w)
			return;
		w.SetPos(S(x), S(y));
		w.SetSize(S(width), S(height));
	}

	static void Move(Widget w, float x, float y)
	{
		if (w)
			w.SetPos(S(x), S(y));
	}

	static void Resize(Widget w, float width, float height)
	{
		if (w)
			w.SetSize(S(width), S(height));
	}

	// Empty container.
	static Widget Frame(Widget parent, float x, float y, float w, float h, string name = "")
	{
		Widget f = INT_UIF_Widgets.Create("px_frame.layout", parent);
		if (!f)
			return null;
		if (name != "")
			f.SetName(name);
		Place(f, x, y, w, h);
		return f;
	}

	// Atlas cell (any name from iuf_ui.imageset), tinted.
	static ImageWidget Image(Widget parent, float x, float y, float w, float h, string cellName, int color)
	{
		ImageWidget img = ImageWidget.Cast(INT_UIF_Widgets.Create("px_image.layout", parent));
		if (!img)
			return null;
		Place(img, x, y, w, h);
		img.LoadImageFile(0, INT_UIF_SET + cellName);
		img.SetImage(0);
		img.SetColor(color);
		return img;
	}

	// Stand-alone texture by path.
	static ImageWidget Texture(Widget parent, float x, float y, float w, float h, string path, int color)
	{
		ImageWidget img = ImageWidget.Cast(INT_UIF_Widgets.Create("px_image.layout", parent));
		if (!img)
			return null;
		Place(img, x, y, w, h);
		img.LoadImageFile(0, path);
		img.SetImage(0);
		img.SetColor(color);
		return img;
	}

	static ImageWidget Icon(Widget parent, float x, float y, float size, string icon, int color)
	{
		return Image(parent, x, y, size, size, "iuf_icon_" + icon, color);
	}

	static ImageWidget Line(Widget parent, float x, float y, float w, float h, int color)
	{
		return Image(parent, x, y, w, h, "iuf_white", color);
	}

	// Single-line text. size = glyph height in design px. big = 72 px SDF
	// source, crisp for large numbers.
	static TextWidget Text(Widget parent, float x, float y, float w, float h, string value, float size, int color, int align = INT_UIF_Align.LEFT, bool big = false)
	{
		Widget root = null;
		if (big)
		{
			if (align == INT_UIF_Align.CENTER)
				root = INT_UIF_Widgets.Create("px_big_c.layout", parent);
			else if (align == INT_UIF_Align.RIGHT)
				root = INT_UIF_Widgets.Create("px_big_r.layout", parent);
			else
				root = INT_UIF_Widgets.Create("px_big_l.layout", parent);
		}
		else
		{
			if (align == INT_UIF_Align.CENTER)
				root = INT_UIF_Widgets.Create("px_text_c.layout", parent);
			else if (align == INT_UIF_Align.RIGHT)
				root = INT_UIF_Widgets.Create("px_text_r.layout", parent);
			else
				root = INT_UIF_Widgets.Create("px_text_l.layout", parent);
		}
		TextWidget tw = TextWidget.Cast(root);
		if (!tw)
			return null;
		Place(tw, x, y, w, h);
		tw.SetTextExactSize(INT_UIF_Scale.TextPx(size));
		tw.SetText(value);
		tw.SetColor(color);
		return tw;
	}

	// Rich text (inline icons via INT_UIF_Widgets.IconMarkup).
	static RichTextWidget Rich(Widget parent, float x, float y, float w, float h, string value, float size, int color, int align = INT_UIF_Align.CENTER)
	{
		Widget root = null;
		if (align == INT_UIF_Align.LEFT)
			root = INT_UIF_Widgets.Create("px_rich_l.layout", parent);
		else if (align == INT_UIF_Align.RIGHT)
			root = INT_UIF_Widgets.Create("px_rich_r.layout", parent);
		else
			root = INT_UIF_Widgets.Create("px_rich_c.layout", parent);
		RichTextWidget rw = RichTextWidget.Cast(root);
		if (!rw)
			return null;
		Place(rw, x, y, w, h);
		rw.SetTextExactSize(INT_UIF_Scale.TextPx(size));
		rw.SetText(value);
		rw.SetColor(color);
		return rw;
	}

	// Wrapping text block.
	static MultilineTextWidget MText(Widget parent, float x, float y, float w, float h, string value, float size, int color)
	{
		MultilineTextWidget mw = MultilineTextWidget.Cast(INT_UIF_Widgets.Create("px_mtext.layout", parent));
		if (!mw)
			return null;
		Place(mw, x, y, w, h);
		mw.SetTextExactSize(INT_UIF_Scale.TextPx(size));
		mw.SetText(value);
		mw.SetColor(color);
		return mw;
	}

	// Edit box whose glyphs are `size` design px, vertically centred in the
	// slot (x, y, w, h). A plain EditBox sizes its glyphs to its own height
	// (exact text size and text_proportion are ignored), so the box is made
	// exactly `size` tall; give the field a wider hit area of its own.
	static EditBoxWidget Edit(Widget parent, float x, float y, float w, float h, float size)
	{
		EditBoxWidget eb = EditBoxWidget.Cast(INT_UIF_Widgets.Create("px_edit.layout", parent));
		if (!eb)
			return null;
		Place(eb, x, y + (h - size) * 0.5, w, size);
		return eb;
	}

	// Container that clips its children to its own rect (scroll viewports).
	// px_frame does not clip (clipchildren 0, so glows and shadows show).
	static Widget Clip(Widget parent, float x, float y, float w, float h, string name = "")
	{
		Widget f = INT_UIF_Widgets.Create("px_clip.layout", parent);
		if (!f)
			return null;
		if (name != "")
			f.SetName(name);
		Place(f, x, y, w, h);
		return f;
	}

	// Vertical scroll area; build children into its "Content" frame
	// (ScrollContent) and size that with SetScrollContent.
	static ScrollWidget Scroll(Widget parent, float x, float y, float w, float h)
	{
		ScrollWidget sw = ScrollWidget.Cast(INT_UIF_Widgets.Create("px_scroll.layout", parent));
		if (!sw)
			return null;
		Place(sw, x, y, w, h);
		return sw;
	}

	static Widget ScrollContent(ScrollWidget sw)
	{
		if (!sw)
			return null;
		return sw.FindAnyWidget("Content");
	}

	// Content height of a Scroll() area in design px.
	static void SetScrollContent(ScrollWidget sw, float h)
	{
		Widget content = ScrollContent(sw);
		if (content)
			content.SetSize(1, S(h));
	}

	// Invisible click/hover target.
	static ButtonWidget Hit(Widget parent, float x, float y, float w, float h, ScriptedWidgetEventHandler handler)
	{
		ButtonWidget b = ButtonWidget.Cast(INT_UIF_Widgets.Create("px_hit.layout", parent));
		if (!b)
			return null;
		Place(b, x, y, w, h);
		if (handler)
			b.SetHandler(handler);
		return b;
	}

	// Estimated rendered width of a string (design px). Metron averages ~0.53 em
	// (self-test calibration: measured 200 px vs 0.56 estimate 212 px).
	static float TextWidth(string value, float size)
	{
		string t = value;
		if (value.IndexOf("#") == 0)
			t = Widget.TranslateString(value);
		return t.LengthUtf8() * size * 0.53;
	}

	// --- composites -------------------------------------------------------------------
	// Circle avatar: soft tinted disc + hairline ring + initial letter.
	static Widget Avatar(Widget parent, float x, float y, float size, string letter, int hue)
	{
		Widget f = Frame(parent, x, y, size, size, "Avatar");
		if (!f)
			return null;
		Image(f, 0, 0, size, size, "iuf_circle64", INT_UIF_Color.WithAlpha(hue, 40));
		Image(f, 0, 0, size, size, "iuf_ringline64", INT_UIF_Color.WithAlpha(hue, 100));
		Text(f, 0, 0, size, size, letter, size * 0.42, hue, INT_UIF_Align.CENTER);
		return f;
	}

	// Icon in a soft tinted rounded tile.
	static Widget IconTile(Widget parent, float x, float y, float size, string icon, int hue, int radius = 10)
	{
		Widget f = Frame(parent, x, y, size, size, "IconTile");
		if (!f)
			return null;
		INT_UIF_Surface.Box(f, 0, 0, size, size, radius, INT_UIF_Color.WithAlpha(hue, 36), INT_UIF_Color.WithAlpha(hue, 82));
		float isz = size * 0.44;
		Icon(f, (size - isz) * 0.5, (size - isz) * 0.5, isz, icon, hue);
		return f;
	}

	// Status dot with an optional breathing glow.
	static Widget Dot(Widget parent, float x, float y, float size, int color, bool pulse = false)
	{
		Widget f = Frame(parent, x, y, size, size, "Dot");
		if (!f)
			return null;
		if (pulse)
		{
			float g = size * 3.2;
			ImageWidget glow = Image(f, (size - g) * 0.5, (size - g) * 0.5, g, g, "iuf_radial", INT_UIF_Color.WithAlpha(color, 150));
			INT_UIF_Animator.Pulse(glow, 0.1, 0.62, 1.8);
		}
		Image(f, 0, 0, size, size, "iuf_circle16", color);
		return f;
	}

	// Key cap + label ("Esc  закрыть"). Returns the frame; width via out.
	static Widget Keycap(Widget parent, float x, float y, string key, string label, out float width)
	{
		float kw = TextWidth(key, 11) + 16;
		if (kw < 26)
			kw = 26;
		float lw = TextWidth(label, 12) + 16;
		width = kw + 8 + lw;
		Widget f = Frame(parent, x, y, width, 22, "Keycap");
		if (!f)
			return null;
		INT_UIF_Surface.Box(f, 0, 0, kw, 22, 4, ARGB(16, 255, 255, 255), ARGB(32, 255, 255, 255));
		Text(f, 0, 0, kw, 22, key, 11, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED), INT_UIF_Align.CENTER);
		Text(f, kw + 8, 0, lw, 22, label, 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		return f;
	}

	// Section caption: small icon + spaced capitals in the faint colour.
	static Widget Caption(Widget parent, float x, float y, float w, string value, string icon = "")
	{
		Widget f = Frame(parent, x, y, w, 14, "Caption");
		if (!f)
			return null;
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);
		float tx = 0;
		if (icon != "")
		{
			Icon(f, 0, 0, 13, icon, faint);
			tx = 20;
		}
		Text(f, tx, 0, w - tx, 14, value, 11, faint);
		return f;
	}
}

// Rounded surface: tinted 9-slice fill + 1 px border (alpha 0 = no border).
// Holding the object is only needed to recolour it later; the widgets live in
// the tree either way.
class INT_UIF_Surface extends Managed
{
	Widget m_Root;
	ButtonWidget m_Fill;
	ButtonWidget m_Border;

	// radius snaps down to 4/6/8/10/12/14.
	static INT_UIF_Surface Box(Widget parent, float x, float y, float w, float h, int radius, int fill, int border = 0)
	{
		Widget root = null;
		if (radius >= 14)
			root = INT_UIF_Widgets.Create("surface_r14.layout", parent);
		else if (radius >= 12)
			root = INT_UIF_Widgets.Create("surface_r12.layout", parent);
		else if (radius >= 10)
			root = INT_UIF_Widgets.Create("surface_r10.layout", parent);
		else if (radius >= 8)
			root = INT_UIF_Widgets.Create("surface_r8.layout", parent);
		else if (radius >= 6)
			root = INT_UIF_Widgets.Create("surface_r6.layout", parent);
		else
			root = INT_UIF_Widgets.Create("surface_r4.layout", parent);
		return Wrap(root, x, y, w, h, fill, border);
	}

	// Fully rounded ends: radius = h / 2, snapped down to an available pill.
	static INT_UIF_Surface Pill(Widget parent, float x, float y, float w, float h, int fill, int border = 0)
	{
		float r = h * 0.5;
		Widget root = null;
		if (r >= 20)
			root = INT_UIF_Widgets.Create("surface_p20.layout", parent);
		else if (r >= 18)
			root = INT_UIF_Widgets.Create("surface_p18.layout", parent);
		else if (r >= 16)
			root = INT_UIF_Widgets.Create("surface_p16.layout", parent);
		else if (r >= 14)
			root = INT_UIF_Widgets.Create("surface_p14.layout", parent);
		else if (r >= 12)
			root = INT_UIF_Widgets.Create("surface_p12.layout", parent);
		else if (r >= 10)
			root = INT_UIF_Widgets.Create("surface_p10.layout", parent);
		else if (r >= 8)
			root = INT_UIF_Widgets.Create("surface_p8.layout", parent);
		else if (r >= 6)
			root = INT_UIF_Widgets.Create("surface_p6.layout", parent);
		else if (r >= 4)
			root = INT_UIF_Widgets.Create("surface_p4.layout", parent);
		else
			root = INT_UIF_Widgets.Create("surface_p3.layout", parent);
		return Wrap(root, x, y, w, h, fill, border);
	}

	// Soft drop shadow (tint black) -- give it ~50 px more than the shape.
	static INT_UIF_Surface Shadow(Widget parent, float x, float y, float w, float h, int color)
	{
		return Wrap(INT_UIF_Widgets.Create("surface_shadow.layout", parent), x, y, w, h, color, 0);
	}

	// Coloured halo (hover glow, selection) -- ~24 px more than the shape.
	static INT_UIF_Surface Glow(Widget parent, float x, float y, float w, float h, int color)
	{
		return Wrap(INT_UIF_Widgets.Create("surface_glow.layout", parent), x, y, w, h, color, 0);
	}

	protected static INT_UIF_Surface Wrap(Widget root, float x, float y, float w, float h, int fill, int border)
	{
		if (!root)
			return null;
		INT_UIF_Surface s = new INT_UIF_Surface();
		s.m_Root = root;
		s.m_Fill = ButtonWidget.Cast(root.FindAnyWidget("Fill"));
		s.m_Border = ButtonWidget.Cast(root.FindAnyWidget("Border"));
		INT_UIF_Draw.Place(root, x, y, w, h);
		s.SetFill(fill);
		s.SetBorder(border);
		return s;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	void SetFill(int color)
	{
		if (m_Fill)
			m_Fill.SetColor(color);
	}

	void SetBorder(int color)
	{
		if (!m_Border)
			return;
		m_Border.Show(INT_UIF_Color.A(color) > 0);
		m_Border.SetColor(color);
	}

	void FadeFill(int color, float duration)
	{
		if (m_Fill)
			INT_UIF_Animator.ColorTo(m_Fill, color, duration);
	}

	void FadeBorder(int color, float duration)
	{
		if (!m_Border)
			return;
		if (INT_UIF_Color.A(color) > 0 && !m_Border.IsVisible())
		{
			m_Border.SetColor(INT_UIF_Color.WithAlpha(color, 0));
			m_Border.Show(true);
		}
		INT_UIF_Animator.ColorTo(m_Border, color, duration);
	}

	void Show(bool show)
	{
		if (m_Root)
			m_Root.Show(show);
	}
}
