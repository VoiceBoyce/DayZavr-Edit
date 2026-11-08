// INT PartyMode (client) — name tags over the SHOWN friends (concept C «ЭКСПЕДИЦИЯ», design/concepts_v1/3_c_expedition_hud.png):
// «◆ ВИТЯ 124 м» without a plate over a soft dark halo, the health line under it in the player's style (bar / % / numbers /
// coloured dot / none), «17% · КРИТИЧНО» in red with a red glow under 20 %, status icons (unconscious, bleeding, broken
// legs), a thin stem down to the head. A dead friend (setting TagsOnDead): framed diamond with a skull + «ПОГИБ» over
// the death spot. Constant screen size (TagSize %), anchored over the head bone of the friend's replicated body, else
// over his last server position + 1.9 m; never drawn behind the camera; an off-screen friend goes to the edge arrows.
// Pool: one item per shown friend, built once, rebound by Steam64 every frame (no pointer to a friend is kept).

class INT_PTY_HudTag : Managed
{
	static const float ROW_H = 22;
	static const float GAP = 9;
	static const float DIA = 11.3;			// 8 px square turned 45° (concept .ctag .dm)
	static const float ICON = 15;
	static const float BAR_W = 84;
	static const float HEAD_LIFT = 0.32;	// m above the head bone
	static const float BODYLESS_LIFT = 1.9;	// m above the last known position
	static const float DEAD_LIFT = 0.35;
	static const float APPEAR_S = 0.5;

	Widget m_Root;
	ImageWidget m_Blob;
	ImageWidget m_Glow;
	ImageWidget m_Dia;
	ref INT_PTY_HudText m_Name;
	ref INT_PTY_HudText m_Dist;
	ImageWidget m_HpBg;
	ImageWidget m_HpFill;
	ref INT_PTY_HudText m_HpText;
	ImageWidget m_HpDot;
	ref INT_PTY_HudText m_Low;
	ImageWidget m_Stem;
	ImageWidget m_IcoUncon;
	ImageWidget m_IcoBleed;
	ImageWidget m_IcoLegs;
	// dead look
	ImageWidget m_DeadFill;
	ImageWidget m_DeadLine;
	ImageWidget m_Skull;
	ref INT_PTY_HudText m_DeadName;
	ref INT_PTY_HudText m_DeadDist;
	ref INT_PTY_HudText m_DeadTxt;

	float m_K = 1;
	int m_HpStyle;
	string m_Id;
	bool m_Seen;
	bool m_Visible;
	float m_BornAt = -100;		// clock when the tag (re)appeared
	float m_GoneAt = -100;
	PlayerBase m_BoneBody;
	int m_Bone = -1;

	// layout key (relayout only when one of these changes)
	int m_LayMode = -1;			// 0 normal, 1 low, 2 dead
	string m_LayName = "<none>";
	int m_LayDistLen = -1;
	int m_LayIcons = -1;
	bool m_LayHp;
	// texts cached by value (no string building per frame)
	int m_DistKey = -1;
	int m_HpKey = -1;
	int m_LowKey = -1;
	// last painted look (skip the paint when nothing changed)
	int m_PMode = -1;
	int m_PHp = -1;
	int m_PIcons = -1;
	int m_PAcc;
	int m_PColor;
	float m_PA = -1;
	float m_PDA = -1;
	float m_RowW;
	float m_RowX;

	void Build(Widget layer, float k, int hpStyle)
	{
		m_K = k;
		m_HpStyle = hpStyle;
		m_Root = INT_PTY_HudKit.Frame(layer, "INT_PTY_Tag");
		m_Blob = INT_PTY_HudKit.Img(m_Root, "pty_halo_dark_ca", 0xFF000000);
		m_Glow = INT_PTY_HudKit.Img(m_Root, "pty_glow_ca", 0xFFFF3C30);
		m_Stem = INT_PTY_HudKit.Img(m_Root, "pty_px_ca", 0xFFFFFFFF);
		m_HpBg = INT_PTY_HudKit.Img(m_Root, "pty_px_ca", 0xFFFFFFFF);
		m_HpFill = INT_PTY_HudKit.Img(m_Root, "pty_px_ca", 0xFFFFFFFF);
		m_HpDot = INT_UIF_Draw.Image(m_Root, 0, 0, 1, 1, "iuf_circle16", 0xFF56E07E);
		m_Dia = INT_PTY_HudKit.Img(m_Root, "pty_diamond_ca", 0xFFFFFFFF);
		m_Name = new INT_PTY_HudText();
		m_Name.Build(m_Root, 16, INT_UIF_Align.LEFT, true, k, 2.2);
		m_Dist = new INT_PTY_HudText();
		m_Dist.Build(m_Root, 15, INT_UIF_Align.LEFT, false, k, 0.6);
		m_HpText = new INT_PTY_HudText();
		m_HpText.Build(m_Root, 13, INT_UIF_Align.CENTER, true, k, 0.5);
		m_Low = new INT_PTY_HudText();
		m_Low.Build(m_Root, 13, INT_UIF_Align.CENTER, true, k, 1.3);
		m_IcoUncon = INT_PTY_HudKit.Img(m_Root, "pty_icon_unconscious_ca", 0xFFE8E8E8);
		m_IcoBleed = INT_PTY_HudKit.Img(m_Root, "pty_icon_bleeding_ca", 0xFFFF4B3E);
		m_IcoLegs = INT_PTY_HudKit.Img(m_Root, "pty_icon_legs_ca", 0xFFFFC246);
		m_DeadFill = INT_PTY_HudKit.Img(m_Root, "pty_diamond_ca", 0xFF0C0A10);
		m_DeadLine = INT_PTY_HudKit.Img(m_Root, "pty_diamond_line_ca", 0xFFFFFFFF);
		m_Skull = INT_PTY_HudKit.Img(m_Root, "pty_icon_skull_ca", 0xFFFFFFFF);
		m_DeadName = new INT_PTY_HudText();
		m_DeadName.Build(m_Root, 15, INT_UIF_Align.LEFT, true, k, 2.1);
		m_DeadDist = new INT_PTY_HudText();
		m_DeadDist.Build(m_Root, 15, INT_UIF_Align.LEFT, false, k, 0.6);
		m_DeadTxt = new INT_PTY_HudText();
		m_DeadTxt.Build(m_Root, 13, INT_UIF_Align.CENTER, true, k, 3.4);
		m_DeadTxt.SetText(INT_PTY_Text.Upper("#STR_INT_PTY_H_DEAD"));
		if (m_Root)
			m_Root.Show(false);
	}

	void Destroy()
	{
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
	}

	void SetVisible(bool on, float now)
	{
		if (on == m_Visible)
			return;
		m_Visible = on;
		if (m_Root)
			m_Root.Show(on);
		if (on)
		{
			if (now - m_GoneAt > 0.3)
				m_BornAt = now;
		}
		else
		{
			m_GoneAt = now;
		}
	}

	// Screen point of the head (or the fallback) of a friend; false = behind the camera.
	vector AnchorWorld(INT_PTY_Friend f)
	{
		if (f.Dead)
		{
			vector dp = f.DeathPos;
			dp[1] = dp[1] + DEAD_LIFT;
			return dp;
		}
		PlayerBase body = f.Body;
		if (body)
		{
			// H-7: a new body, or the head bone was not there yet (-1: skeleton not ready) -> ask again
			if (body != m_BoneBody || m_Bone < 0)
			{
				m_BoneBody = body;
				int bone = body.GetBoneIndexByName("Head");
				m_Bone = bone;
			}
			vector hp;
			if (m_Bone >= 0)
			{
				hp = body.GetBonePositionWS(m_Bone);
			}
			else
			{
				hp = body.GetPosition();
				hp[1] = hp[1] + 1.6;
			}
			hp[1] = hp[1] + HEAD_LIFT;
			return hp;
		}
		vector lp = f.LastPos;
		lp[1] = lp[1] + BODYLESS_LIFT;
		return lp;
	}

	// --- alive / low look ---------------------------------------------------------------------------------------
	void ShowAlive(INT_PTY_HudMate mate, int hp, bool hasHp, int icons, float dist, float nameA, float distA, int accent, float now)
	{
		bool low = hasHp && hp > 0 && hp < 20 && m_HpStyle != INT_PTY_HpStyle.TAG_NONE;
		int mode = 0;
		if (low)
			mode = 1;
		bool showHp = hasHp && m_HpStyle != INT_PTY_HpStyle.TAG_NONE;
		if (m_LayMode == 2)
			m_DistKey = -1;
		SetDistText(m_Dist, dist);
		int distLen = m_Dist.m_Text.LengthUtf8();
		if (mode != m_LayMode || mate.m_Upper != m_LayName || distLen != m_LayDistLen || icons != m_LayIcons || showHp != m_LayHp)
			LayoutAlive(mate, mode, distLen, icons, showHp);
		float appear = INT_PTY_HudKit.OutCubic(INT_PTY_HudKit.Seg(now - m_BornAt, 0, APPEAR_S));
		float a = nameA * appear;
		float da = distA * appear;
		float lift = (1 - appear) * 10;
		if (lift != m_LiftPx)
			ApplyLift(lift);
		// nothing changed since the last frame: the widgets keep their colours (only the place moves)
		if (mode == m_PMode && hp == m_PHp && icons == m_PIcons && accent == m_PAcc && mate.m_Color == m_PColor)
		{
			if (Math.AbsFloat(a - m_PA) < 0.003 && Math.AbsFloat(da - m_PDA) < 0.003)
				return;
		}
		m_PMode = mode;
		m_PHp = hp;
		m_PIcons = icons;
		m_PAcc = accent;
		m_PColor = mate.m_Color;
		m_PA = a;
		m_PDA = da;
		int nameRgb = 0xFFFFFFFF;
		INT_PTY_HudKit.Paint(m_Dia, mate.m_Color, a);
		m_Name.Paint(nameRgb, a);
		m_Dist.Paint(accent, da);
		INT_PTY_HudKit.Paint(m_Stem, 0xFFFFFFFF, 0.55 * a);
		if (low)
		{
			INT_PTY_HudKit.Paint(m_Glow, 0xFFFF3C30, 0.42 * a);
			INT_PTY_HudKit.Paint(m_Blob, 0xFF000000, 0.4 * a);
		}
		else
		{
			INT_PTY_HudKit.Paint(m_Blob, 0xFF000000, 0.7 * a);
		}
		if (showHp)
			PaintHp(hp, low, a);
		if (low)
		{
			if (hp != m_LowKey)
			{
				m_LowKey = hp;
				string lowText = INT_PTY_Text.Upper("#STR_INT_PTY_H_CRITICAL");
				lowText.Replace("%1", hp.ToString() + "%");
				m_Low.SetText(lowText);
			}
			m_Low.Paint(0xFFFF6D61, a);
		}
		INT_PTY_HudKit.Paint(m_IcoUncon, 0xFFE8E8E8, a);
		INT_PTY_HudKit.Paint(m_IcoBleed, 0xFFFF4B3E, a);
		INT_PTY_HudKit.Paint(m_IcoLegs, 0xFFFFC246, a);
	}

	protected void PaintHp(int hp, bool low, float a)
	{
		if (m_HpStyle == INT_PTY_HpStyle.TAG_BAR)
		{
			INT_PTY_HudKit.Paint(m_HpBg, 0xFFFFFFFF, 0.28 * a);
			int fillRgb = 0xFFFFFFFF;
			if (low)
				fillRgb = 0xFFFF4B3E;
			INT_PTY_HudKit.Paint(m_HpFill, fillRgb, a);
			float w = BAR_W * Math.Clamp(hp, 0, 100) / 100.0;
			float k = m_K;
			if (m_HpFill)
			{
				float fw = w * INT_PTY_HudKit.s_F * k;
				float fh = 3 * INT_PTY_HudKit.s_F * k;
				m_HpFill.SetSize(fw, fh);
			}
			return;
		}
		if (m_HpStyle == INT_PTY_HpStyle.TAG_DOT)
		{
			INT_PTY_HudKit.Paint(m_HpDot, INT_PTY_HudKit.HpColor(hp), a);
			return;
		}
		if (hp != m_HpKey)
		{
			m_HpKey = hp;
			if (m_HpStyle == INT_PTY_HpStyle.TAG_NUM)
				m_HpText.SetText(hp.ToString() + " / 100");
			else
				m_HpText.SetText(hp.ToString() + "%");
		}
		int txtRgb = 0xFFFFFFFF;
		if (low)
			txtRgb = 0xFFFF6D61;
		m_HpText.Paint(txtRgb, a);
	}

	// Layout from the anchor (0, 0) upwards: stem, [critical line], health line, name row; blob behind.
	protected void LayoutAlive(INT_PTY_HudMate mate, int mode, int distLen, int icons, bool showHp)
	{
		m_LayMode = mode;
		m_LayName = mate.m_Upper;
		m_LayDistLen = distLen;
		m_LayIcons = icons;
		m_LayHp = showHp;
		m_HpKey = -1;
		m_Name.SetText(mate.m_Upper);
		float k = m_K;
		bool low = mode == 1;
		SetDeadShown(false);
		INT_PTY_HudKit.Vis(m_Dia, true);
		m_Name.Show(true);
		m_Dist.Show(true);
		INT_PTY_HudKit.Vis(m_Stem, true);
		INT_PTY_HudKit.Vis(m_Glow, low);
		m_Low.Show(low);
		float y = -12;
		INT_PTY_HudKit.Rect(m_Stem, -0.5, -12, 1, 12, k);
		y = y - 5;
		if (low)
		{
			m_Low.Rect(-150, y - 16, 300, 16, k);
			y = y - 16 - 4;
		}
		bool bar = showHp && m_HpStyle == INT_PTY_HpStyle.TAG_BAR;
		bool dot = showHp && m_HpStyle == INT_PTY_HpStyle.TAG_DOT;
		// «15% · КРИТИЧНО» already says the number: no second number line under 20 %
		bool txt = showHp && !bar && !dot && !low;
		INT_PTY_HudKit.Vis(m_HpBg, bar);
		INT_PTY_HudKit.Vis(m_HpFill, bar);
		INT_PTY_HudKit.Vis(m_HpDot, dot);
		m_HpText.Show(txt);
		if (bar)
		{
			INT_PTY_HudKit.Rect(m_HpBg, -BAR_W * 0.5, y - 3, BAR_W, 3, k);
			INT_PTY_HudKit.Rect(m_HpFill, -BAR_W * 0.5, y - 3, BAR_W, 3, k);
			y = y - 3 - 6;
		}
		else if (dot)
		{
			INT_PTY_HudKit.Square(m_HpDot, 0, y - 5, 10, k);
			y = y - 10 - 6;
		}
		else if (txt)
		{
			m_HpText.Rect(-100, y - 16, 200, 16, k);
			y = y - 16 - 5;
		}
		else
		{
			y = y - 2;
		}
		float rowTop = y - ROW_H;
		float rowC = y - ROW_H * 0.5;
		// row: diamond, gap, name, gap, distance, [gap, icons]
		float nameEst = INT_PTY_HudKit.TextW(mate.m_Upper, 16, true, true, 2.2);
		float distEst = INT_PTY_HudKit.TextW(m_Dist.m_Text, 15, false, false, 0.6);
		float nameW = m_Name.MeasureW(ROW_H, k, nameEst);
		float distW = m_Dist.MeasureW(ROW_H, k, distEst);
		int nIcons = 0;
		if ((icons & 1) != 0)
			nIcons++;
		if ((icons & 2) != 0)
			nIcons++;
		if ((icons & 4) != 0)
			nIcons++;
		float iconsW = 0;
		if (nIcons > 0)
			iconsW = 6 + nIcons * (ICON + 3);
		float total = DIA + GAP + nameW + GAP + distW + iconsW;
		float x = -total * 0.5;
		INT_PTY_HudKit.Square(m_Dia, x + DIA * 0.5, rowC, DIA, k);
		x = x + DIA + GAP;
		m_Name.Rect(x, rowTop, nameW + 30, ROW_H, k);
		x = x + nameW + GAP;
		m_Dist.Rect(x, rowTop, distW + 30, ROW_H, k);
		x = x + distW + 6;
		INT_PTY_HudKit.Vis(m_IcoUncon, (icons & 1) != 0);
		INT_PTY_HudKit.Vis(m_IcoBleed, (icons & 2) != 0);
		INT_PTY_HudKit.Vis(m_IcoLegs, (icons & 4) != 0);
		if ((icons & 1) != 0)
		{
			INT_PTY_HudKit.Square(m_IcoUncon, x + ICON * 0.5, rowC, ICON, k);
			x = x + ICON + 3;
		}
		if ((icons & 2) != 0)
		{
			INT_PTY_HudKit.Square(m_IcoBleed, x + ICON * 0.5, rowC, ICON, k);
			x = x + ICON + 3;
		}
		if ((icons & 4) != 0)
			INT_PTY_HudKit.Square(m_IcoLegs, x + ICON * 0.5, rowC, ICON, k);
		m_RowW = total;
		m_RowX = -total * 0.5;
		// soft dark halo behind (concept: 40 px wider each side, from 18 px above the row to 6 px under the stem)
		INT_PTY_HudKit.Rect(m_Blob, -total * 0.5 - 40, rowTop - 18, total + 80, -rowTop + 24, k);
		INT_PTY_HudKit.Rect(m_Glow, -total * 0.5 - 36, rowTop - 16, total + 72, -rowTop + 14, k);
		INT_PTY_HudKit.Vis(m_Blob, true);
	}

	// --- dead look ---------------------------------------------------------------------------------------------
	void ShowDead(INT_PTY_HudMate mate, float dist, float nameA, float distA, int accent, float now)
	{
		if (m_LayMode != 2)
			m_DistKey = -1;
		SetDistText(m_DeadDist, dist);
		int distLen = m_DeadDist.m_Text.LengthUtf8();
		if (m_LayMode != 2 || mate.m_Upper != m_LayName || distLen != m_LayDistLen)
			LayoutDead(mate, distLen);
		float appear = INT_PTY_HudKit.OutCubic(INT_PTY_HudKit.Seg(now - m_BornAt, 0, APPEAR_S));
		float a = nameA * appear;
		ApplyLift((1 - appear) * 10);
		INT_PTY_HudKit.Paint(m_Blob, 0xFF000000, 0.55 * a);
		INT_PTY_HudKit.Paint(m_DeadFill, 0xFF0C0A10, 0.7 * a);
		INT_PTY_HudKit.Paint(m_DeadLine, mate.m_Color, a);
		INT_PTY_HudKit.Paint(m_Skull, 0xFFFFFFFF, a);
		m_DeadName.Paint(0xFFFFFFFF, a);
		m_DeadDist.Paint(accent, distA * appear);
		m_DeadTxt.Paint(0xFFFF6D61, a);
	}

	protected void LayoutDead(INT_PTY_HudMate mate, int distLen)
	{
		m_LayMode = 2;
		m_LayName = mate.m_Upper;
		m_LayDistLen = distLen;
		m_LayIcons = -1;
		float k = m_K;
		INT_PTY_HudKit.Vis(m_Dia, false);
		m_Name.Show(false);
		m_Dist.Show(false);
		INT_PTY_HudKit.Vis(m_Stem, false);
		INT_PTY_HudKit.Vis(m_Glow, false);
		m_Low.Show(false);
		INT_PTY_HudKit.Vis(m_HpBg, false);
		INT_PTY_HudKit.Vis(m_HpFill, false);
		INT_PTY_HudKit.Vis(m_HpDot, false);
		m_HpText.Show(false);
		INT_PTY_HudKit.Vis(m_IcoUncon, false);
		INT_PTY_HudKit.Vis(m_IcoBleed, false);
		INT_PTY_HudKit.Vis(m_IcoLegs, false);
		SetDeadShown(true);
		m_DeadName.SetText(mate.m_Upper);
		// from the anchor up: «ПОГИБ» (16), 2, name + distance (18), 6, framed diamond (40)
		m_DeadTxt.Rect(-150, -16, 300, 16, k);
		float nameEst = INT_PTY_HudKit.TextW(mate.m_Upper, 15, true, true, 2.1);
		float distEst = INT_PTY_HudKit.TextW(m_DeadDist.m_Text, 15, false, false, 0.6);
		float nameW = m_DeadName.MeasureW(18, k, nameEst);
		float distW = m_DeadDist.MeasureW(18, k, distEst);
		float total = nameW + 8 + distW;
		float x = -total * 0.5;
		m_DeadName.Rect(x, -36, nameW + 30, 18, k);
		m_DeadDist.Rect(x + nameW + 8, -36, distW + 30, 18, k);
		INT_PTY_HudKit.Square(m_DeadFill, 0, -62, 40, k);
		INT_PTY_HudKit.Square(m_DeadLine, 0, -62, 40, k);
		INT_PTY_HudKit.Square(m_Skull, 0, -62, 16, k);
		float bw = total;
		if (bw < 60)
			bw = 60;
		INT_PTY_HudKit.Rect(m_Blob, -bw * 0.5 - 34, -84, bw + 68, 96, k);
		INT_PTY_HudKit.Vis(m_Blob, true);
	}

	protected void SetDeadShown(bool on)
	{
		INT_PTY_HudKit.Vis(m_DeadFill, on);
		INT_PTY_HudKit.Vis(m_DeadLine, on);
		INT_PTY_HudKit.Vis(m_Skull, on);
		m_DeadName.Show(on);
		m_DeadDist.Show(on);
		m_DeadTxt.Show(on);
	}

	// Distance text cached by whole metres / tenths of a kilometre.
	protected void SetDistText(INT_PTY_HudText t, float dist)
	{
		int key = (int)Math.Round(dist);
		if (dist >= 999.5)
			key = 100000 + (int)Math.Round(dist / 100.0);
		if (key == m_DistKey && t.m_Text != "<none>")
			return;
		m_DistKey = key;
		t.SetText(INT_PTY_HudKit.Dist(dist));
	}

	// Slide of the appear animation (design px, down = positive) applied to the root.
	protected float m_LiftPx;
	protected float m_Sx;
	protected float m_Sy;
	protected float m_ExtraUp;
	protected float m_PlacedX = -10000;
	protected float m_PlacedY = -10000;

	protected void ApplyLift(float px)
	{
		m_LiftPx = px;
		PlaceRoot();
	}

	void Place(float sx, float sy, float extraUpPx)
	{
		m_Sx = sx;
		m_Sy = sy;
		m_ExtraUp = extraUpPx;
		PlaceRoot();
	}

	protected void PlaceRoot()
	{
		if (!m_Root)
			return;
		float f = INT_PTY_HudKit.s_F * m_K;
		float py = m_Sy + (m_LiftPx - m_ExtraUp) * f;
		if (Math.AbsFloat(m_Sx - m_PlacedX) < 0.5 && Math.AbsFloat(py - m_PlacedY) < 0.5)
			return;
		m_PlacedX = m_Sx;
		m_PlacedY = py;
		m_Root.SetPos(m_Sx, py);
	}

	// Box of what is drawn (screen px) for GetPartRect.
	void GetRect(out float x, out float y, out float w, out float h)
	{
		float f = INT_PTY_HudKit.s_F * m_K;
		float rw = m_RowW;
		if (rw < 60)
			rw = 60;
		x = m_Sx - rw * 0.5 * f;
		y = m_Sy - 90 * f;
		w = rw * f;
		h = 90 * f;
	}
}

class INT_PTY_HudTags : Managed
{
	static const float RF_NICK_RANGE = 10.0;	// @World_HeadNicknames draws its nick within ~9 m
	static const float RF_NICK_LIFT = 34;		// design px

	protected Widget m_Layer;
	protected ref array<ref INT_PTY_HudTag> m_Items;
	protected float m_K = 1;
	protected int m_HpStyle = -1;
	protected int m_Active;
	protected ref INT_PTY_HudMate m_SampleMate;
	protected ref INT_PTY_HudMate m_SampleLow;

	void INT_PTY_HudTags()
	{
		m_Items = new array<ref INT_PTY_HudTag>();
	}

	void Build(Widget layer, float k, int hpStyle)
	{
		Clear();
		m_Layer = layer;
		m_K = k;
		m_HpStyle = hpStyle;
	}

	void Clear()
	{
		foreach (INT_PTY_HudTag t : m_Items)
		{
			t.Destroy();
		}
		m_Items.Clear();
		m_Active = 0;
	}

	int ActiveCount()
	{
		return m_Active;
	}

	protected INT_PTY_HudTag Take(string id)
	{
		INT_PTY_HudTag free = null;
		foreach (INT_PTY_HudTag t : m_Items)
		{
			if (t.m_Id == id)
				return t;
			if (!free && !t.m_Seen && t.m_Id == "")
				free = t;
		}
		if (!free)
		{
			foreach (INT_PTY_HudTag u : m_Items)
			{
				if (!u.m_Seen && !u.m_Visible)
				{
					free = u;
					break;
				}
			}
		}
		if (!free)
		{
			free = new INT_PTY_HudTag();
			free.Build(m_Layer, m_K, m_HpStyle);
			m_Items.Insert(free);
		}
		free.m_Id = id;
		free.m_LayMode = -1;
		free.m_PMode = -1;
		free.m_DistKey = -1;
		free.m_BornAt = -100;
		free.m_GoneAt = -100;
		return free;
	}

	void Update(INT_PTY_HudCtx ctx, INT_PTY_HudCore core)
	{
		foreach (INT_PTY_HudTag t : m_Items)
		{
			t.m_Seen = false;
		}
		m_Active = 0;
		INT_PTY_Settings st = ctx.m_St;
		bool partOn = INT_PTY_UI.IsPartShown(INT_PTY_UI.PART_TAGS) && st.TagsOn == 1;
		if (ctx.m_Edit)
			UpdateSamples(ctx, core);
		else if (partOn && ctx.m_World && INT_PTY_Client.s_Friends)
			UpdateWorld(ctx, core);
		foreach (INT_PTY_HudTag u : m_Items)
		{
			if (u.m_Seen)
				continue;
			u.SetVisible(false, ctx.m_Now);
		}
	}

	protected void UpdateWorld(INT_PTY_HudCtx ctx, INT_PTY_HudCore core)
	{
		INT_PTY_Settings st = ctx.m_St;
		float maxDist = Math.Min(st.TagDistance, INT_PTY_Client.s_MaxTagDistance);
		float opacity = INT_PTY_UI.PartOpacity(INT_PTY_UI.PART_TAGS);
		float nameA = st.TagNameAlpha * 0.01 * opacity;
		float distA = st.TagDistAlpha * 0.01 * opacity;
		int accent = INT_PTY_UI.PartAccent(INT_PTY_UI.PART_TAGS);
		bool arrows = ctx.m_Arrows;
		foreach (INT_PTY_Friend f : INT_PTY_Client.s_Friends)
		{
			INT_PTY_HudMate mate = core.Mate(f.Id);
			if (!mate)
				continue;
			if (!f.Online)
				continue;
			if (!f.KnowsPos())
				continue;
			if (f.Dead && st.TagsOnDead != 1)
				continue;
			vector feet = f.WorldPos();
			float dist = vector.Distance(ctx.m_MyPos, feet);
			if (dist > maxDist)
				continue;
			INT_PTY_HudTag item = Take(f.Id);
			vector anchor = item.AnchorWorld(f);
			float sx;
			float sy;
			bool front = ctx.Project(anchor, sx, sy);
			if (!front || !ctx.OnScreen(sx, sy, -24))
			{
				if (arrows)
					core.Arrows().Add(ctx, anchor, mate.m_Color, dist);
				item.m_Id = "";
				continue;
			}
			item.m_Seen = true;
			float extra = 0;
			#ifdef RF_NICKNAMES_CL
			if (!f.Dead && f.Body && dist < RF_NICK_RANGE + 2)
				extra = RF_NICK_LIFT * Math.Clamp((RF_NICK_RANGE + 2 - dist) / 2.0, 0, 1);
			#endif
			item.Place(sx, sy, extra);
			item.SetVisible(true, ctx.m_Now);
			m_Active++;
			if (f.Dead)
			{
				item.ShowDead(mate, dist, nameA, distA, accent, ctx.m_Now);
				continue;
			}
			int icons = 0;
			if (st.StatusIcons == 1)
				icons = StatusBits(f);
			item.ShowAlive(mate, f.Hp, f.HasState, icons, dist, nameA, distA, accent, ctx.m_Now);
		}
	}

	// 1 unconscious, 2 bleeding, 4 broken legs: the live body (synced vars) when we have it, else the last PULL.
	// H-1: ONE rule with the server's F_BROKEN_LEG: only untreated BROKEN_LEGS shows the icon (a splint does not), so
	// the icon does not change when the friend enters / leaves the network bubble.
	static int StatusBits(INT_PTY_Friend f)
	{
		int bits = 0;
		PlayerBase b = f.Body;
		if (b)
		{
			if (b.IsUnconscious())
				bits = bits | 1;
			if (b.IsBleeding())
				bits = bits | 2;
			if (b.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
				bits = bits | 4;
			return bits;
		}
		if (!f.HasState)
			return 0;
		if (f.HasFlag(INT_PTY_Const.F_UNCONSCIOUS))
			bits = bits | 1;
		if (f.HasFlag(INT_PTY_Const.F_BLEEDING))
			bits = bits | 2;
		if (f.HasFlag(INT_PTY_Const.F_BROKEN_LEG))
			bits = bits | 4;
		return bits;
	}

	// Edit mode: two samples at fixed places (normal + critical) so an editor sees size, colour and opacity.
	protected void UpdateSamples(INT_PTY_HudCtx ctx, INT_PTY_HudCore core)
	{
		if (!m_SampleMate)
		{
			m_SampleMate = new INT_PTY_HudMate();
			m_SampleMate.m_Id = "sample1";
			m_SampleMate.m_Upper = "ВИТЯ";
			m_SampleMate.m_Color = 0xFFFF5A4F;
			m_SampleLow = new INT_PTY_HudMate();
			m_SampleLow.m_Id = "sample2";
			m_SampleLow.m_Upper = "KOLYA_SNIPER";
			m_SampleLow.m_Color = 0xFFFFB02E;
		}
		INT_PTY_Settings st = ctx.m_St;
		float opacity = INT_PTY_UI.PartOpacity(INT_PTY_UI.PART_TAGS);
		if (!INT_PTY_UI.IsPartShown(INT_PTY_UI.PART_TAGS))
			opacity = opacity * 0.35;
		float nameA = st.TagNameAlpha * 0.01 * opacity;
		float distA = st.TagDistAlpha * 0.01 * opacity;
		int accent = INT_PTY_UI.PartAccent(INT_PTY_UI.PART_TAGS);
		float f = INT_PTY_HudKit.s_F;
		INT_PTY_HudTag a = Take(m_SampleMate.m_Id);
		a.m_Seen = true;
		a.Place(ctx.m_Sw * 0.5 - 170 * f, ctx.m_Sh * 0.34, 0);
		a.SetVisible(true, ctx.m_Now);
		a.ShowAlive(m_SampleMate, 86, true, 0, 124, nameA, distA, accent, ctx.m_Now);
		INT_PTY_HudTag b = Take(m_SampleLow.m_Id);
		b.m_Seen = true;
		b.Place(ctx.m_Sw * 0.5 + 170 * f, ctx.m_Sh * 0.34, 0);
		b.SetVisible(true, ctx.m_Now);
		b.ShowAlive(m_SampleLow, 17, true, 2, 342, nameA, distA, accent, ctx.m_Now);
		m_Active = 2;
	}

	bool GetRect(out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		bool any = false;
		float x1 = 0;
		float y1 = 0;
		float x2 = 0;
		float y2 = 0;
		foreach (INT_PTY_HudTag t : m_Items)
		{
			if (!t.m_Visible)
				continue;
			float rx;
			float ry;
			float rw;
			float rh;
			t.GetRect(rx, ry, rw, rh);
			if (!any)
			{
				x1 = rx;
				y1 = ry;
				x2 = rx + rw;
				y2 = ry + rh;
				any = true;
				continue;
			}
			x1 = Math.Min(x1, rx);
			y1 = Math.Min(y1, ry);
			x2 = Math.Max(x2, rx + rw);
			y2 = Math.Max(y2, ry + rh);
		}
		if (!any)
			return false;
		x = x1;
		y = y1;
		w = x2 - x1;
		h = y2 - y1;
		return true;
	}
}
