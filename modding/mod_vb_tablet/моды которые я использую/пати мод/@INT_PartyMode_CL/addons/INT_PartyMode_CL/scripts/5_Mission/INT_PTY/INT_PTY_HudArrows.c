// INT PartyMode (client) — edge arrows (owner D11, setting EdgeArrows): a friend tag or a ping outside the screen gets
// a filled arrowhead (pty_edge_arrow_ca, pointing UP in the texture, turned toward the target) on the screen edge in
// the friend's / ping's colour, with the distance under it. Direction from the camera frame (right / up components),
// so targets behind the camera point to the side to turn to. Candidates are collected by the tags and pings parts
// during their update and drawn after them; the pool only grows.

class INT_PTY_HudArrow : Managed
{
	static const float SIZE = 26;

	Widget m_Root;
	ImageWidget m_Out;
	ImageWidget m_Arrow;
	ref INT_PTY_HudText m_Txt;
	int m_DistKey = -1;
	bool m_Visible;
	float m_Sx = -1000;
	float m_Sy = -1000;
	float m_Deg = -1000;
	int m_Rgb = 1;
	float m_Alpha = -1;

	void Build(Widget layer, float k)
	{
		m_Root = INT_PTY_HudKit.Frame(layer, "INT_PTY_Arrow");
		m_Out = INT_PTY_HudKit.Img(m_Root, "pty_edge_arrow_out_ca", 0xFF000000);
		m_Arrow = INT_PTY_HudKit.Img(m_Root, "pty_edge_arrow_ca", 0xFFFFFFFF);
		INT_PTY_HudKit.Square(m_Out, 0, 0, SIZE, k);
		INT_PTY_HudKit.Square(m_Arrow, 0, 0, SIZE, k);
		m_Txt = new INT_PTY_HudText();
		m_Txt.Build(m_Root, 13, INT_UIF_Align.CENTER, true, k, 0.4);
		if (m_Root)
			m_Root.Show(false);
	}

	void Destroy()
	{
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
	}

	void SetVisible(bool on)
	{
		if (on == m_Visible)
			return;
		m_Visible = on;
		if (m_Root)
			m_Root.Show(on);
	}

	// (sx, sy) arrow centre in screen px; (dx, dy) unit direction on screen (y down); deg = turn of the arrow.
	void Show(float sx, float sy, float dx, float dy, float deg, int rgb, float alpha, float dist, float k)
	{
		// every widget call is skipped when its value did not change since the last frame
		if (m_Root && (Math.AbsFloat(sx - m_Sx) > 0.5 || Math.AbsFloat(sy - m_Sy) > 0.5))
			m_Root.SetPos(sx, sy);
		m_Sx = sx;
		m_Sy = sy;
		if (Math.AbsFloat(deg - m_Deg) > 0.3)
		{
			m_Deg = deg;
			if (m_Arrow)
				m_Arrow.SetRotation(0, 0, deg);
			if (m_Out)
				m_Out.SetRotation(0, 0, deg);
			// the distance sits on the inner side of the arrow (toward the screen centre)
			float tx = -dx * 30 - 60;
			float ty = -dy * 30 - 9;
			m_Txt.Rect(tx, ty, 120, 18, k);
		}
		if (rgb != m_Rgb || Math.AbsFloat(alpha - m_Alpha) > 0.003)
		{
			m_Rgb = rgb;
			m_Alpha = alpha;
			INT_PTY_HudKit.Paint(m_Out, 0xFF000000, 0.8 * alpha);
			INT_PTY_HudKit.Paint(m_Arrow, rgb, alpha);
			m_Txt.Paint(0xFFFFFFFF, alpha);
		}
		int key = (int)Math.Round(dist);
		if (dist >= 999.5)
			key = 100000 + (int)Math.Round(dist / 100.0);
		if (key != m_DistKey)
		{
			m_DistKey = key;
			m_Txt.SetText(INT_PTY_HudKit.Dist(dist));
		}
		SetVisible(true);
	}
}

class INT_PTY_HudArrows : Managed
{
	static const float MARGIN = 54;		// design px from the screen edge
	static const int MAX = 64;

	protected Widget m_Layer;
	protected ref array<ref INT_PTY_HudArrow> m_Items;
	protected ref array<float> m_Dx;
	protected ref array<float> m_Dy;
	protected ref array<int> m_Rgb;
	protected ref array<float> m_Dist;
	protected int m_Count;
	protected int m_Shown;
	protected float m_K = 1;
	protected ref array<float> m_AvoidRects;
	protected ref array<float> m_PlacedX;
	protected ref array<float> m_PlacedY;
	protected int m_AvoidN;

	void INT_PTY_HudArrows()
	{
		m_AvoidRects = new array<float>();
		m_PlacedX = new array<float>();
		m_PlacedY = new array<float>();
		for (int v = 0; v < 16; v++)
		{
			m_AvoidRects.Insert(0);
		}
		m_Items = new array<ref INT_PTY_HudArrow>();
		m_Dx = new array<float>();
		m_Dy = new array<float>();
		m_Rgb = new array<int>();
		m_Dist = new array<float>();
		for (int i = 0; i < MAX; i++)
		{
			m_Dx.Insert(0);
			m_Dy.Insert(0);
			m_Rgb.Insert(0);
			m_Dist.Insert(0);
		}
	}

	void Build(Widget layer, float k)
	{
		Clear();
		m_Layer = layer;
		m_K = k;
	}

	void Clear()
	{
		foreach (INT_PTY_HudArrow a : m_Items)
		{
			a.Destroy();
		}
		m_Items.Clear();
		m_Count = 0;
		m_Shown = 0;
	}

	void Begin()
	{
		m_Count = 0;
	}

	int ActiveCount()
	{
		return m_Shown;
	}

	// A target outside the screen (world position, colour, distance from the player).
	void Add(INT_PTY_HudCtx ctx, vector worldPos, int rgb, float dist)
	{
		if (m_Count >= MAX)
			return;
		float lx;
		float ly;
		float lz;
		ctx.ToCamera(worldPos, lx, ly, lz);
		float dx = lx;
		float dy = -ly;
		if (lz <= 0)
		{
			// behind the camera: the side to turn to (left / right edge), height only as a slight tilt
			float flat = Math.Sqrt(lx * lx + lz * lz);
			if (flat < 0.001)
				flat = 0.001;
			dx = 1;
			if (lx < 0)
				dx = -1;
			dy = Math.Clamp(-ly / flat, -0.5, 0.5);
		}
		float len = Math.Sqrt(dx * dx + dy * dy);
		if (len < 0.0001)
		{
			dx = 0;
			dy = 1;
		}
		else
		{
			dx = dx / len;
			dy = dy / len;
		}
		m_Dx.Set(m_Count, dx);
		m_Dy.Set(m_Count, dy);
		m_Rgb.Set(m_Count, rgb);
		m_Dist.Set(m_Count, dist);
		m_Count++;
	}

	void Draw(INT_PTY_HudCtx ctx)
	{
		m_Shown = 0;
		bool partOn = INT_PTY_UI.IsPartShown(INT_PTY_UI.PART_ARROWS);
		if (ctx.m_Edit)
		{
			DrawSamples(ctx);
		}
		else if (partOn && ctx.m_Arrows)
		{
			float alpha = 0.95 * INT_PTY_UI.PartOpacity(INT_PTY_UI.PART_ARROWS);
			int accent = INT_PTY_UI.PartAccent(INT_PTY_UI.PART_ARROWS);
			for (int i = 0; i < m_Count; i++)
			{
				int rgb = m_Rgb.Get(i);
				if (accent != 0)
					rgb = accent;
				float cdx = m_Dx.Get(i);
				float cdy = m_Dy.Get(i);
				float cdist = m_Dist.Get(i);
				Put(ctx, m_Shown, cdx, cdy, rgb, alpha, cdist);
				m_Shown++;
			}
		}
		for (int j = m_Shown; j < m_Items.Count(); j++)
		{
			m_Items.Get(j).SetVisible(false);
		}
	}

	protected void Put(INT_PTY_HudCtx ctx, int index, float dx, float dy, int rgb, float alpha, float dist)
	{
		while (m_Items.Count() <= index)
		{
			INT_PTY_HudArrow n = new INT_PTY_HudArrow();
			n.Build(m_Layer, m_K);
			m_Items.Insert(n);
		}
		INT_PTY_HudArrow a = m_Items.Get(index);
		float m = MARGIN * INT_PTY_HudKit.s_F * m_K;
		float cx = ctx.m_Sw * 0.5;
		float cy = ctx.m_Sh * 0.5;
		float left = m;
		float right = ctx.m_Sw - m;
		float top = m;
		float bottom = ctx.m_Sh - m;
		float t = 100000;
		if (dx > 0.0001)
			t = Math.Min(t, (right - cx) / dx);
		else if (dx < -0.0001)
			t = Math.Min(t, (left - cx) / dx);
		if (dy > 0.0001)
			t = Math.Min(t, (bottom - cy) / dy);
		else if (dy < -0.0001)
			t = Math.Min(t, (top - cy) / dy);
		float sx = cx + dx * t;
		float sy = cy + dy * t;
		// never on top of the tracker panel or the notification cards: slide along the edge out of their box
		float pad = 34 * INT_PTY_HudKit.s_F * m_K;
		bool side = false;
		if (sx <= left + 1 || sx >= right - 1)
			side = true;
		for (int r = 0; r < m_AvoidN; r++)
		{
			float ax = m_AvoidRects.Get(r * 4);
			float ay = m_AvoidRects.Get(r * 4 + 1);
			float aw = m_AvoidRects.Get(r * 4 + 2);
			float ah = m_AvoidRects.Get(r * 4 + 3);
			if (sx < ax - pad || sx > ax + aw + pad || sy < ay - pad || sy > ay + ah + pad)
				continue;
			if (side)
			{
				float upY = ay - pad;
				float downY = ay + ah + pad;
				if (sy - upY < downY - sy && upY > top)
					sy = upY;
				else
					sy = Math.Min(downY, bottom);
			}
			else
			{
				float leftX = ax - pad;
				float rightX = ax + aw + pad;
				if (sx - leftX < rightX - sx && leftX > left)
					sx = leftX;
				else
					sx = Math.Min(rightX, right);
			}
		}
		// several arrows on one spot (a friend and his pings): the later one takes the nearest free slot along the
		// edge (base, +1, -1, +2, -2 ... steps), every candidate re-checked against the placed arrows and the boxes
		// (on the top / bottom edge the label sits beside the arrow: the step there is a label width)
		float stepY = 30 * INT_PTY_HudKit.s_F * m_K;
		float stepX = 30 * INT_PTY_HudKit.s_F * m_K;
		if (!side)
			stepX = 70 * INT_PTY_HudKit.s_F * m_K;
		float baseX = sx;
		float baseY = sy;
		for (int pass = 0; pass < 17; pass++)
		{
			float tx = baseX;
			float ty = baseY;
			if (pass > 0)
			{
				int ring = (pass + 1) / 2;
				float off = ring * stepY;
				if (!side)
					off = ring * stepX;
				if (pass % 2 == 0)
					off = -off;
				if (side)
					ty = baseY + off;
				else
					tx = baseX + off;
			}
			if (ty < top - 1 || ty > bottom + 1 || tx < left - 1 || tx > right + 1)
				continue;
			if (!SlotTaken(tx, ty, index, stepX, stepY, pad))
			{
				sx = tx;
				sy = ty;
				break;
			}
		}
		while (m_PlacedX.Count() <= index)
		{
			m_PlacedX.Insert(0);
			m_PlacedY.Insert(0);
		}
		m_PlacedX.Set(index, sx);
		m_PlacedY.Set(index, sy);
		float deg = Math.Atan2(dx, -dy) * Math.RAD2DEG;
		a.Show(sx, sy, dx, dy, deg, rgb, alpha, dist, m_K);
	}

	// A slot is taken when an earlier arrow of this frame sits closer than one step, or it falls into an avoid box.
	protected bool SlotTaken(float x, float y, int index, float stepX, float stepY, float pad)
	{
		for (int q = 0; q < index && q < m_PlacedX.Count(); q++)
		{
			float ox = m_PlacedX.Get(q);
			float oy = m_PlacedY.Get(q);
			if (Math.AbsFloat(ox - x) < stepX && Math.AbsFloat(oy - y) < stepY)
				return true;
		}
		for (int r = 0; r < m_AvoidN; r++)
		{
			float ax = m_AvoidRects.Get(r * 4);
			float ay = m_AvoidRects.Get(r * 4 + 1);
			float aw = m_AvoidRects.Get(r * 4 + 2);
			float ah = m_AvoidRects.Get(r * 4 + 3);
			if (x > ax - pad + 1 && x < ax + aw + pad - 1 && y > ay - pad + 1 && y < ay + ah + pad - 1)
				return true;
		}
		return false;
	}

	// Boxes the arrows keep out of this frame (screen px): the tracker panel, the card stack.
	void ClearAvoid()
	{
		m_AvoidN = 0;
	}

	void AddAvoid(float x, float y, float w, float h)
	{
		if (m_AvoidN >= 4)
			return;
		int at = m_AvoidN * 4;
		m_AvoidRects.Set(at, x);
		m_AvoidRects.Set(at + 1, y);
		m_AvoidRects.Set(at + 2, w);
		m_AvoidRects.Set(at + 3, h);
		m_AvoidN++;
	}

	// Edit mode: one arrow on the left edge, one on the right (friend colours of the concept).
	protected void DrawSamples(INT_PTY_HudCtx ctx)
	{
		float alpha = 0.95 * INT_PTY_UI.PartOpacity(INT_PTY_UI.PART_ARROWS);
		if (!INT_PTY_UI.IsPartShown(INT_PTY_UI.PART_ARROWS))
			alpha = alpha * 0.35;
		int accent = INT_PTY_UI.PartAccent(INT_PTY_UI.PART_ARROWS);
		int c1 = 0xFF3FD0FF;
		int c2 = 0xFF7BE36B;
		if (accent != 0)
		{
			c1 = accent;
			c2 = accent;
		}
		Put(ctx, 0, -0.97, -0.24, c1, alpha, 58);
		Put(ctx, 1, 0.94, 0.34, c2, alpha, 1410);
		m_Shown = 2;
	}

	bool GetRect(out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		if (m_Shown == 0)
			return false;
		float f = INT_PTY_HudKit.s_F * m_K;
		float x1 = 100000;
		float y1 = 100000;
		float x2 = -100000;
		float y2 = -100000;
		for (int i = 0; i < m_Shown && i < m_Items.Count(); i++)
		{
			INT_PTY_HudArrow a = m_Items.Get(i);
			x1 = Math.Min(x1, a.m_Sx - 40 * f);
			y1 = Math.Min(y1, a.m_Sy - 40 * f);
			x2 = Math.Max(x2, a.m_Sx + 40 * f);
			y2 = Math.Max(y2, a.m_Sy + 40 * f);
		}
		x = x1;
		y = y1;
		w = x2 - x1;
		h = y2 - y1;
		return true;
	}
}
