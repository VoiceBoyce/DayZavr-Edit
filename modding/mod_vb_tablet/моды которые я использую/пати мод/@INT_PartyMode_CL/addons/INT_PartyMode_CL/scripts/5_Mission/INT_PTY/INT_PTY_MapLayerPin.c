// INT PartyMode (client) — one pooled marker of the map layer (INT_PTY_MapLayer, «ЭКСПЕДИЦИЯ» style):
//   friend       coloured diamond (pty_diamond_ca) under a dark hairline (pty_diamond_line_ca) + name in tracked capitals
//   dead friend  dark diamond with a line in his colour + white skull + name + «П О Г И Б» (big maps)
//   ping         the shape (pty_ping_<shape>_ca) in its colour over its dark outline twin (_out_ca) + owner name
// Names sit on a soft dark halo (pty_halo_dark_ca) and carry a text shadow: readable on the paper map and on the dark
// VPP / RF tiles. The marker is built once in design px around an anchor at (0, 0) = the point on the map; afterwards
// the root is only moved (SetPos in map-local screen px), and textures / texts / colours are touched only when its
// mark changes. The label stands right of the marker and flips to the left at the right edge of the map (no host map
// clips text); when neither side fits (narrow minimap) the label is hidden. Compact = HUD minimap: smaller marks,
// short plain-capital names, pings without a label.

class INT_PTY_MapLayerPin
{
	// geometry, design px at layer scale 1
	static const float D_FRIEND = 12;			// diamond side, big map
	static const float D_DEAD = 18;
	static const float D_PING = 10;				// ping icon box (owner 2026-10-23: pings half the first size)
	static const float C_FRIEND = 8;			// compact (minimap)
	static const float C_DEAD = 11;
	static const float C_PING = 7;
	static const float NAME_SIZE = 13;
	static const float PING_NAME_SIZE = 9;		// 11 -25 % = 8.25, held at the smallest crisp size (9)
	static const float SUB_SIZE = 10;
	static const float C_NAME_SIZE = 10;
	static const int NAME_CHARS = 14;			// longer names are cut with an ellipsis
	static const int C_NAME_CHARS = 9;

	// colours (alpha is multiplied by the layer alpha)
	static const int HALO = 0xB3000000;			// 70 % black at the core of the soft round blob (pty_halo_dark_ca)
	static const int OUTLINE = 0xD9000000;		// dark outline twin of a ping shape
	static const int INK = 0xE6120800;			// hairline around a live friend's diamond
	static const int DEAD_FILL = 0xD90C0A10;
	static const int NAME_INK = 0xFFF4F4F4;
	static const int DEAD_NAME = 0xFFBDBDBD;
	static const int DEAD_RED = 0xFFFF6D61;
	static const int SKULL = 0xFFFFFFFF;
	static const string RAW_NONE = "~INT_PTY_NONE~";

	Widget m_Root;
	ImageWidget m_Halo;
	ImageWidget m_Under;
	ImageWidget m_Over;
	ImageWidget m_Icon;
	TextWidget m_Name;
	TextWidget m_Sub;

	protected bool m_Compact;
	protected float m_K = 1;
	protected int m_Kind = -1;
	protected int m_Shape = -1;
	protected int m_Color;
	protected float m_Alpha = -1;
	protected int m_Accent;
	protected string m_Raw = RAW_NONE;			// never a real name: the first SetContent always writes the text
	protected string m_Text;
	protected string m_SubText;
	protected bool m_Shown = true;
	protected int m_Side = -1;					// 0 label right, 1 label left, 2 no label
	protected bool m_HasLabel;
	protected bool m_HasSub;
	protected bool m_Suppressed;				// label dropped while it would print over its owner's name
	protected bool m_Measured;					// label widths taken from the drawn text (GetTextSize) once
	float m_LX;									// last map-local anchor (px)
	float m_LY;

	// label geometry of the current kind (design px, already x m_K; side 0 = right of the anchor)
	protected float m_LabelX;
	protected float m_LabelY;
	protected float m_LabelW;
	protected float m_LabelH;
	protected float m_SubY;
	protected float m_SubW;
	protected float m_SubH;
	protected float m_HaloX;
	protected float m_HaloY;
	protected float m_HaloW;
	protected float m_HaloH;
	protected float m_ExtPx;					// screen px from the anchor to the far end of the label

	bool Build(Widget host, bool compact, float k)
	{
		m_Compact = compact;
		m_K = k;
		m_Root = INT_UIF_Draw.Frame(host, 0, 0, 1, 1, "INT_PTY_MapPin");
		if (!m_Root)
			return false;
		m_Halo = INT_PTY_Look.Tex(m_Root, 0, 0, 10, 10, "pty_halo_dark_ca", 0);
		m_Under = INT_PTY_Look.Tex(m_Root, 0, 0, 10, 10, "pty_diamond_ca", 0);
		m_Over = INT_PTY_Look.Tex(m_Root, 0, 0, 10, 10, "pty_diamond_line_ca", 0);
		m_Icon = INT_PTY_Look.IconAt(m_Root, 0, 0, 10, "skull", 0);
		m_Name = INT_PTY_Look.Text(m_Root, 0, 0, 10, 10, "", NAME_SIZE, NAME_INK);
		m_Sub = INT_PTY_Look.Text(m_Root, 0, 0, 10, 10, "", SUB_SIZE, DEAD_RED);
		if (!m_Halo || !m_Under || !m_Over || !m_Icon || !m_Name || !m_Sub)
		{
			m_Root.Unlink();
			m_Root = null;
			return false;
		}
		m_Name.SetName("INT_PTY_MapName");
		m_Sub.SetName("INT_PTY_MapSub");
		m_Name.SetShadow(1, 0xFF000000, 0.85, 1, 1);
		m_Sub.SetShadow(1, 0xFF000000, 0.85, 1, 1);
		m_Icon.Show(false);
		m_Sub.Show(false);
		return true;
	}

	bool IsShown()
	{
		return m_Shown;
	}

	void Hide()
	{
		if (!m_Shown || !m_Root)
			return;
		m_Root.Show(false);
		m_Shown = false;
	}

	// Stand probe: "kind text @x,y s<side>" (screen px of the anchor).
	string TestLine()
	{
		float x;
		float y;
		m_Root.GetScreenPos(x, y);
		int ix = (int)Math.Round(x);
		int iy = (int)Math.Round(y);
		string k = m_Kind.ToString();
		string sx = ix.ToString();
		string sy = iy.ToString();
		string sd = m_Side.ToString();
		return k + " " + m_Text + " @" + sx + "," + sy + " s" + sd;
	}

	// The mark this pin shows now. Only what changed is touched.
	void SetContent(INT_PTY_MapLayerMark mk, float alpha, int accent)
	{
		if (!mk || !m_Root)
			return;
		bool relayout = false;
		bool recolor = false;
		bool kindDiff = mk.m_Kind != m_Kind;
		if (!kindDiff && mk.m_Kind == INT_PTY_MapLayer.MK_PING && mk.m_Shape != m_Shape)
			kindDiff = true;
		if (kindDiff)
		{
			ApplyKind(mk.m_Kind, mk.m_Shape);
			relayout = true;
			recolor = true;
		}
		if (mk.m_Name != m_Raw)
		{
			m_Raw = mk.m_Name;
			relayout = true;
		}
		if (mk.m_Color != m_Color || alpha != m_Alpha || accent != m_Accent)
			recolor = true;
		if (relayout)
			Layout();
		if (recolor)
		{
			m_Color = mk.m_Color;
			m_Alpha = alpha;
			m_Accent = accent;
			Recolor();
		}
	}

	// Moves the pin to map-local px (lx, ly) inside a map of mw x mh px; hidden outside the map rect.
	void Place(float lx, float ly, float mw, float mh, float edge)
	{
		if (!m_Root)
			return;
		bool inside = true;
		if (lx < edge || ly < edge || lx > mw - edge || ly > mh - edge)
			inside = false;
		if (inside != m_Shown)
		{
			m_Root.Show(inside);
			m_Shown = inside;
		}
		if (!inside)
			return;
		m_Root.SetPos(lx, ly);
		m_LX = lx;
		m_LY = ly;
		if (!m_HasLabel)
			return;
		if (!m_Measured)
			Measure();
		int side = 2;
		if (lx + m_ExtPx <= mw - edge)
			side = 0;
		else if (lx - m_ExtPx >= edge)
			side = 1;
		if (side != m_Side)
		{
			m_Side = side;
			ApplySide();
		}
	}

	void SetLabelSuppressed(bool hide)
	{
		if (hide == m_Suppressed)
			return;
		m_Suppressed = hide;
		ApplySide();
	}

	// --- internals ---------------------------------------------------------------------------------------------------
	protected void ApplyKind(int kind, int shape)
	{
		m_Kind = kind;
		m_Shape = shape;
		string under = INT_PTY_Look.TexPath("pty_diamond_ca");
		string over = INT_PTY_Look.TexPath("pty_diamond_line_ca");
		if (kind == INT_PTY_MapLayer.MK_PING)
		{
			under = INT_PTY_UI.PingTex(shape, true);
			over = INT_PTY_UI.PingTex(shape, false);
		}
		LoadTex(m_Under, under);
		LoadTex(m_Over, over);
		bool dead = kind == INT_PTY_MapLayer.MK_DEAD;
		m_Icon.Show(dead);
		m_HasSub = false;
		if (dead && !m_Compact)
			m_HasSub = true;
		m_HasLabel = true;
		if (m_Compact && kind == INT_PTY_MapLayer.MK_PING)
			m_HasLabel = false;
		float size = NameSize();
		int px = INT_UIF_Scale.TextPx(size * m_K);
		m_Name.SetTextExactSize(px);
		m_SubText = "";
		if (m_HasSub)
		{
			m_SubText = INT_PTY_MapLayer.DeadText();
			int spx = INT_UIF_Scale.TextPx(SUB_SIZE * m_K);
			m_Sub.SetTextExactSize(spx);
			m_Sub.SetText(m_SubText);
		}
		m_Raw = RAW_NONE;		// the label text depends on the kind (size / cut): rewritten by SetContent
	}

	protected float NameSize()
	{
		if (m_Compact)
			return C_NAME_SIZE;
		if (m_Kind == INT_PTY_MapLayer.MK_PING)
			return PING_NAME_SIZE;
		return NAME_SIZE;
	}

	protected void LoadTex(ImageWidget w, string path)
	{
		if (!w)
			return;
		w.LoadImageFile(0, path);
		w.SetImage(0);
	}

	// Name text + every position of the current kind (design px x m_K).
	protected void Layout()
	{
		float k = m_K;
		float size = NameSize();
		int chars = NAME_CHARS;
		if (m_Compact)
			chars = C_NAME_CHARS;
		string cut = Cut(m_Raw, chars);
		if (m_Compact)
		{
			m_Text = INT_PTY_Text.Upper(cut);
			float tw = INT_PTY_Look.TextW(m_Text, size, false);
			m_LabelW = tw * k;
		}
		else
		{
			m_Text = INT_PTY_Text.Caps(cut);
			float sw = INT_PTY_Look.SpacedWidth(m_Text, size);
			m_LabelW = sw * k;
		}
		m_Name.SetText(m_Text);

		// the marker
		float d = D_FRIEND;
		if (m_Kind == INT_PTY_MapLayer.MK_DEAD)
			d = D_DEAD;
		else if (m_Kind == INT_PTY_MapLayer.MK_PING)
			d = D_PING;
		if (m_Compact)
		{
			d = C_FRIEND;
			if (m_Kind == INT_PTY_MapLayer.MK_DEAD)
				d = C_DEAD;
			else if (m_Kind == INT_PTY_MapLayer.MK_PING)
				d = C_PING;
		}
		float box = d * k;
		if (m_Kind != INT_PTY_MapLayer.MK_PING)
			box = d * INT_PTY_Look.SQRT2 * k;		// the diamond is drawn inside its texture, diagonal = side
		float half = box * 0.5;
		INT_UIF_Draw.Place(m_Under, -half, -half, box, box);
		INT_UIF_Draw.Place(m_Over, -half, -half, box, box);
		if (m_Kind == INT_PTY_MapLayer.MK_DEAD)
		{
			float sk = d * 0.72 * k;
			float skHalf = sk * 0.5;
			INT_UIF_Draw.Place(m_Icon, -skHalf, -skHalf, sk, sk);
		}

		// the label (right side; ApplySide mirrors it)
		float gap = 5 * k;
		if (m_Compact)
			gap = 3 * k;
		m_Half = half;
		m_LabelX = half + gap;
		m_LabelH = (size + 5) * k;
		m_LabelY = -m_LabelH * 0.5;
		m_SubW = 0;
		m_SubH = 0;
		m_SubY = 0;
		if (m_HasSub)
		{
			m_LabelY = -m_LabelH + 1 * k;
			m_SubH = (SUB_SIZE + 4) * k;
			m_SubY = 2 * k;
			float subRaw = INT_PTY_Look.SpacedWidth(m_SubText, SUB_SIZE);
			m_SubW = subRaw * k;
		}
		m_Measured = false;
		LabelBox();
		m_Side = -1;
		ApplySide();
	}

	protected float m_Half;

	// Halo and the reach of the label from the current widths (estimated, then measured).
	protected void LabelBox()
	{
		float k = m_K;
		float far = m_LabelW;
		float top = m_LabelY;
		float bottom = m_LabelY + m_LabelH;
		if (m_HasSub)
		{
			if (m_SubW > far)
				far = m_SubW;
			bottom = m_SubY + m_SubH;
		}
		// the blob is dense only in its inner ~40 % (gen_pty_art.tex_halo_dark): pad it like the HUD tags do
		float padX = 26 * k;
		float padY = 12 * k;
		if (m_Compact)
		{
			padX = 12 * k;
			padY = 6 * k;
		}
		m_HaloX = -m_Half - padX;
		m_HaloW = m_LabelX + far + padX - m_HaloX;
		m_HaloY = top - padY;
		m_HaloH = bottom - top + padY * 2;
		float ext = m_LabelX + far + 4 * k;
		m_ExtPx = INT_UIF_Draw.S(ext);
	}

	// The estimated widths run a few px short on long spaced names (stand 2026-10-23: 117 drawn vs 111 estimated):
	// once the text is drawn, its real size replaces the estimate, so the halo, the flip at the edge and the
	// mirrored label sit right. GetTextSize answers 0 until the text is laid out -> tried again on the next place.
	protected void Measure()
	{
		int tw;
		int th;
		m_Name.GetTextSize(tw, th);
		if (tw <= 0)
			return;
		float f = INT_UIF_Scale.Factor();
		if (f <= 0.01)
			f = 1;
		float nameW = tw / f;
		float subW = m_SubW;
		if (m_HasSub)
		{
			int sw;
			int sh;
			m_Sub.GetTextSize(sw, sh);
			if (sw <= 0)
				return;
			subW = sw / f;
		}
		m_Measured = true;
		bool diff = Math.AbsFloat(nameW - m_LabelW) > 0.5;
		if (!diff && Math.AbsFloat(subW - m_SubW) > 0.5)
			diff = true;
		if (!diff)
			return;
		m_LabelW = nameW;
		m_SubW = subW;
		LabelBox();
		m_Side = -1;
	}

	protected void ApplySide()
	{
		bool on = m_HasLabel && m_Side != 2 && !m_Suppressed;
		m_Name.Show(on);
		m_Halo.Show(on);
		m_Sub.Show(on && m_HasSub);
		if (!on)
			return;
		float nameW = m_LabelW + 6 * m_K;
		float subW = m_SubW + 6 * m_K;
		float nx = m_LabelX;
		float sx = m_LabelX;
		float hx = m_HaloX;
		if (m_Side == 1)
		{
			nx = -(m_LabelX + nameW);
			sx = -(m_LabelX + subW);
			hx = -(m_HaloX + m_HaloW);
		}
		INT_UIF_Draw.Place(m_Name, nx, m_LabelY, nameW, m_LabelH);
		INT_UIF_Draw.Place(m_Halo, hx, m_HaloY, m_HaloW, m_HaloH);
		if (m_HasSub)
			INT_UIF_Draw.Place(m_Sub, sx, m_SubY, subW, m_SubH);
	}

	protected void Recolor()
	{
		float a = m_Alpha;
		int c = INT_UIF_Color.WithAlpha(m_Color, 255);
		m_Halo.SetColor(Fade(HALO, a));
		if (m_Kind == INT_PTY_MapLayer.MK_PING)
		{
			m_Under.SetColor(Fade(OUTLINE, a));
			m_Over.SetColor(Fade(c, a));
			m_Name.SetColor(Fade(c, a));
			return;
		}
		if (m_Kind == INT_PTY_MapLayer.MK_DEAD)
		{
			m_Under.SetColor(Fade(DEAD_FILL, a));
			m_Over.SetColor(Fade(c, a));
			m_Icon.SetColor(Fade(SKULL, a));
			m_Name.SetColor(Fade(DEAD_NAME, a));
			m_Sub.SetColor(Fade(DEAD_RED, a));
			return;
		}
		m_Under.SetColor(Fade(c, a));
		m_Over.SetColor(Fade(INK, a));
		int ink = NAME_INK;
		if (m_Accent != 0)
			ink = INT_UIF_Color.WithAlpha(m_Accent, 255);
		m_Name.SetColor(Fade(ink, a));
	}

	// The colour with its alpha multiplied by `a` (SetAlpha would REPLACE the alpha, so colours carry it).
	static int Fade(int argb, float a)
	{
		float al = INT_UIF_Color.A(argb) * a;
		int ai = (int)Math.Round(al);
		if (ai < 0)
			ai = 0;
		if (ai > 255)
			ai = 255;
		return INT_UIF_Color.WithAlpha(argb, ai);
	}

	// At most `chars` characters, an ellipsis when cut.
	static string Cut(string value, int chars)
	{
		string t = value;
		t.TrimInPlace();
		int n = t.LengthUtf8();
		if (n <= chars)
			return t;
		string head = t.SubstringUtf8(0, chars - 1);
		head.TrimInPlace();
		return head + "…";
	}
}
