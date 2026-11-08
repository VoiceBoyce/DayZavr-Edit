// INT PartyMode (client) — the tracker panel (concept C, 3_c_expedition_hud.png left; CSS .ctrk / .tr; the «◆ ГРУППА N»
// head row was removed by the owner 2026-11-01 — PvP feedback, it only took room; size = TrackerSize % × part scale):
// a soft black gradient from the left, one 40 px row per shown friend: diamond in his colour, name, health in the
// player's style (TrackerHpStyle: «86%», bar, coloured dot, «86 / 100»), status icons, distance + a small arrow turned
// toward him relative to the camera. Low HP (< 20 %): red row with a red band that blinks (TrackerBlink); dead: skull +
// «ПОГИБ»; offline (TrackerOffline): greyed «НЕ В СЕТИ». Whole-panel opacity TrackerAlpha (every widget multiplied).
// Place: TrackerX / TrackerY (screen fraction of the top-left corner; -1 = left middle), moved by the mover.

class INT_PTY_HudRowData : Managed
{
	static const int NORMAL = 0;
	static const int LOW = 1;
	static const int DEAD = 2;
	static const int OFFLINE = 3;

	string m_Id;
	string m_Name;
	int m_Rgb;
	int m_State;
	int m_Hp;
	bool m_HasHp;
	float m_Dist;
	bool m_HasDist;
	float m_Bearing;
	int m_Icons;
}

class INT_PTY_HudTrackRow : Managed
{
	static const float H = 40;

	Widget m_Root;
	ImageWidget m_Band;			// red low-HP band (blinks)
	ImageWidget m_Flash;		// short wash when the row appears / the friend dies
	ImageWidget m_Line;
	ImageWidget m_Dia;
	ImageWidget m_Skull;
	ref INT_PTY_HudText m_Name;
	ImageWidget m_BarBg;
	ImageWidget m_BarFill;
	ImageWidget m_Dot;
	ref INT_PTY_HudText m_HpText;
	ref INT_PTY_HudText m_Dist;
	ImageWidget m_Arrow;
	ref INT_PTY_HudText m_Tag;
	ImageWidget m_IcoUncon;
	ImageWidget m_IcoBleed;
	ImageWidget m_IcoLegs;

	float m_K = 1;
	int m_HpStyle;
	string m_Id;
	int m_State = -1;
	int m_HpKey = -1;
	int m_DistKey = -2;
	int m_Icons = -1;
	string m_NameText = "<none>";
	float m_BornAt = -100;
	float m_FlashAt = -100;
	int m_FlashRgb;
	bool m_FlashOn;
	bool m_Visible;
	// last painted look (static parts are repainted only when one of these changes)
	int m_PState = -1;
	int m_PRgb;
	int m_PAcc;
	int m_PHp = -1;
	int m_PIcons = -1;
	float m_PA = -1;
	float m_PAppear = -1;
	float m_PY = -1;
	float m_WantY;
	float m_PBearing = -10000;

	void Build(Widget parent, float k, int hpStyle)
	{
		m_K = k;
		m_HpStyle = hpStyle;
		m_Root = INT_PTY_HudKit.Frame(parent, "INT_PTY_TrackRow");
		INT_PTY_HudKit.Rect(m_Root, 0, 0, 360, H, k);
		m_Band = INT_PTY_HudKit.Img(m_Root, "pty_grad_h_ca", 0xFFFF3C30);
		INT_PTY_HudKit.Rect(m_Band, 0, 0, 360, H, k);
		m_Flash = INT_PTY_HudKit.Img(m_Root, "pty_grad_h_ca", 0xFFFF7A2F);
		INT_PTY_HudKit.Rect(m_Flash, 0, 0, 360, H, k);
		m_Line = INT_PTY_HudKit.Img(m_Root, "pty_px_ca", 0xFFFFFFFF);
		INT_PTY_HudKit.Rect(m_Line, 18, H - 1, 324, 1, k);
		m_Dia = INT_PTY_HudKit.Img(m_Root, "pty_diamond_ca", 0xFFFFFFFF);
		INT_PTY_HudKit.Square(m_Dia, 22.5, H * 0.5, 12.7, k);
		m_Skull = INT_PTY_HudKit.Img(m_Root, "pty_icon_skull_ca", 0xFFFF6D61);
		INT_PTY_HudKit.Square(m_Skull, 22.5, H * 0.5, 16, k);
		m_Name = new INT_PTY_HudText();
		m_Name.Build(m_Root, 16, INT_UIF_Align.LEFT, true, k, 0);
		m_Name.Rect(39, 0, 112, H, k);
		m_BarBg = INT_PTY_HudKit.Img(m_Root, "pty_px_ca", 0xFFFFFFFF);
		INT_PTY_HudKit.Rect(m_BarBg, 155, H * 0.5 - 1.5, 70, 3, k);
		m_BarFill = INT_PTY_HudKit.Img(m_Root, "pty_px_ca", 0xFFFFFFFF);
		INT_PTY_HudKit.Rect(m_BarFill, 155, H * 0.5 - 1.5, 70, 3, k);
		m_Dot = INT_UIF_Draw.Image(m_Root, 0, 0, 1, 1, "iuf_circle16", 0xFF56E07E);
		INT_PTY_HudKit.Square(m_Dot, 159.5, H * 0.5, 9, k);
		m_HpText = new INT_PTY_HudText();
		m_HpText.Build(m_Root, 14, INT_UIF_Align.LEFT, false, k, 0);
		m_HpText.Rect(155, 0, 90, H, k);
		m_Dist = new INT_PTY_HudText();
		m_Dist.Build(m_Root, 15, INT_UIF_Align.RIGHT, false, k, 0);
		m_Dist.Rect(223, 0, 100, H, k);
		m_Arrow = INT_PTY_HudKit.Img(m_Root, "pty_icon_arrow_up_ca", 0xFFFF7A2F);
		INT_PTY_HudKit.Square(m_Arrow, 335, H * 0.5, 13, k);
		m_Tag = new INT_PTY_HudText();
		m_Tag.Build(m_Root, 13, INT_UIF_Align.RIGHT, true, k, 2.6);
		m_Tag.Rect(160, 0, 182, H, k);
		m_IcoUncon = INT_PTY_HudKit.Img(m_Root, "pty_icon_unconscious_ca", 0xFFE8E8E8);
		m_IcoBleed = INT_PTY_HudKit.Img(m_Root, "pty_icon_bleeding_ca", 0xFFFF4B3E);
		m_IcoLegs = INT_PTY_HudKit.Img(m_Root, "pty_icon_legs_ca", 0xFFFFC246);
		if (m_Root)
			m_Root.Show(false);
	}

	void SetVisible(bool on)
	{
		if (on == m_Visible)
			return;
		m_Visible = on;
		if (m_Root)
			m_Root.Show(on);
	}

	// Row place inside the panel (design px); applied in Update together with the appear slide.
	void SetY(float y)
	{
		m_WantY = y;
	}

	// One frame of the row. alpha = TrackerAlpha × part opacity; accent = arrows / caption colour.
	void Update(INT_PTY_HudRowData d, float alpha, int accent, bool blink, float now)
	{
		if (d.m_Id != m_Id)
		{
			m_Id = d.m_Id;
			m_State = -1;
			m_HpKey = -1;
			m_DistKey = -2;
			m_Icons = -1;
			m_BornAt = now;
			m_FlashAt = now;
			m_FlashRgb = 0xFFFF7A2F;
		}
		if (d.m_Name != m_NameText)
		{
			m_NameText = d.m_Name;
			m_Name.SetText(INT_PTY_HudKit.Fit(d.m_Name, 16, 110));
		}
		if (d.m_State != m_State)
		{
			if (d.m_State == INT_PTY_HudRowData.DEAD && m_State >= 0)
			{
				m_FlashAt = now;
				m_FlashRgb = 0xFFFF3C30;
			}
			m_State = d.m_State;
			m_HpKey = -1;
			m_DistKey = -2;
			m_Icons = -1;
			ApplyState();
		}
		// appear: slide from -24 px + fade 0.35 s
		float appear = INT_PTY_HudKit.OutCubic(INT_PTY_HudKit.Seg(now - m_BornAt, 0, 0.35));
		float f = INT_PTY_HudKit.s_F * m_K;
		if (m_Root && (appear != m_PAppear || m_WantY != m_PY))
		{
			m_PAppear = appear;
			m_PY = m_WantY;
			INT_PTY_HudKit.PlaceAt(m_Root, -24 * (1 - appear) * f, m_WantY * f);
		}
		float a = alpha * appear;
		if (m_State == INT_PTY_HudRowData.OFFLINE)
			a = a * 0.45;
		// short wash when the row appears / the friend dies
		float since = now - m_FlashAt;
		if (since < 0.85)
		{
			float flash = 1 - INT_PTY_HudKit.Seg(since, 0.15, 0.8);
			flash = flash * INT_PTY_HudKit.Seg(since, 0, 0.15);
			INT_PTY_HudKit.Paint(m_Flash, m_FlashRgb, 0.3 * flash * alpha);
			m_FlashOn = true;
		}
		else if (m_FlashOn)
		{
			m_FlashOn = false;
			INT_PTY_HudKit.Paint(m_Flash, m_FlashRgb, 0);
		}
		bool low = m_State == INT_PTY_HudRowData.LOW;
		bool live = m_State == INT_PTY_HudRowData.NORMAL || low;
		// per-frame parts: the blinking band and the direction arrow
		if (low)
			PaintBand(blink, now, a);
		if (live && d.m_HasDist && m_Arrow && Math.AbsFloat(d.m_Bearing - m_PBearing) > 0.5)
		{
			m_PBearing = d.m_Bearing;
			INT_PTY_HudKit.Turn(m_Arrow, d.m_Bearing);
		}
		// static parts: painted only when something changed
		int distKey = DistKey(d);
		if (m_State == m_PState && d.m_Rgb == m_PRgb && accent == m_PAcc && d.m_Hp == m_PHp && d.m_Icons == m_PIcons && distKey == m_DistKey)
		{
			if (Math.AbsFloat(a - m_PA) < 0.003)
				return;
		}
		m_PState = m_State;
		m_PRgb = d.m_Rgb;
		m_PAcc = accent;
		m_PHp = d.m_Hp;
		m_PIcons = d.m_Icons;
		m_PA = a;
		INT_PTY_HudKit.Paint(m_Line, 0xFFFFFFFF, 0.07 * a);
		if (m_State == INT_PTY_HudRowData.DEAD)
		{
			INT_PTY_HudKit.Paint(m_Skull, 0xFFFF6D61, a);
			m_Name.Paint(0xFFBDB7AF, a);
			m_Tag.Paint(0xFFFF6D61, a);
			return;
		}
		if (m_State == INT_PTY_HudRowData.OFFLINE)
		{
			INT_PTY_HudKit.Paint(m_Dia, 0xFF77726B, a);
			m_Name.Paint(0xFFFFFFFF, a);
			m_Tag.Paint(0xFFBDB7AF, a);
			return;
		}
		int nameRgb = 0xFFFFFFFF;
		int distRgb = 0xFFE8E4DD;
		if (low)
		{
			nameRgb = 0xFFFF6D61;
			distRgb = 0xFFFF6D61;
		}
		INT_PTY_HudKit.Paint(m_Dia, d.m_Rgb, a);
		m_Name.Paint(nameRgb, a);
		UpdateHp(d, low, a);
		UpdateDist(d, distKey, distRgb, accent, a);
		UpdateIcons(d, a);
	}

	// Concept blink: on 45 %, off 45 % of 700 ms (CD.blink lo = 0); steady red band without TrackerBlink.
	protected void PaintBand(bool blink, float now, float a)
	{
		float bandA = 1;
		if (blink)
		{
			float ph = now / 0.7;
			ph = ph - Math.Floor(ph);
			if (ph >= 0.5 && ph < 0.95)
				bandA = 0;
			else if (ph >= 0.45 && ph < 0.5)
				bandA = 1 - (ph - 0.45) / 0.05;
			else if (ph >= 0.95)
				bandA = (ph - 0.95) / 0.05;
		}
		INT_PTY_HudKit.Paint(m_Band, 0xFFFF3C30, 0.22 * bandA * a);
	}

	protected int DistKey(INT_PTY_HudRowData d)
	{
		if (!d.m_HasDist)
			return -1;
		if (d.m_Dist >= 999.5)
			return 100000 + (int)Math.Round(d.m_Dist / 100.0);
		return (int)Math.Round(d.m_Dist);
	}

	protected void ApplyState()
	{
		bool dead = m_State == INT_PTY_HudRowData.DEAD;
		bool off = m_State == INT_PTY_HudRowData.OFFLINE;
		bool live = !dead && !off;
		INT_PTY_HudKit.Vis(m_Dia, !dead);
		INT_PTY_HudKit.Vis(m_Skull, dead);
		INT_PTY_HudKit.Vis(m_Band, m_State == INT_PTY_HudRowData.LOW);
		m_Tag.Show(dead || off);
		if (dead)
			m_Tag.SetText(INT_PTY_Text.Upper("#STR_INT_PTY_H_DEAD"));
		else if (off)
			m_Tag.SetText(INT_PTY_Text.Upper("#STR_INT_PTY_H_OFFLINE"));
		bool bar = live && m_HpStyle == INT_PTY_HpStyle.TRK_BAR;
		bool dot = live && m_HpStyle == INT_PTY_HpStyle.TRK_DOT;
		bool txt = live && !bar && !dot;
		INT_PTY_HudKit.Vis(m_BarBg, bar);
		INT_PTY_HudKit.Vis(m_BarFill, bar);
		INT_PTY_HudKit.Vis(m_Dot, dot);
		m_HpText.Show(txt);
		m_Dist.Show(live);
		INT_PTY_HudKit.Vis(m_Arrow, live);
		if (!live)
		{
			INT_PTY_HudKit.Vis(m_IcoUncon, false);
			INT_PTY_HudKit.Vis(m_IcoBleed, false);
			INT_PTY_HudKit.Vis(m_IcoLegs, false);
		}
	}

	protected void UpdateHp(INT_PTY_HudRowData d, bool low, float a)
	{
		if (m_HpStyle == INT_PTY_HpStyle.TRK_BAR)
		{
			INT_PTY_HudKit.Paint(m_BarBg, 0xFFFFFFFF, 0.22 * a);
			int fill = 0xFFFFFFFF;
			if (low)
				fill = 0xFFFF4B3E;
			INT_PTY_HudKit.Paint(m_BarFill, fill, a);
			int key = d.m_Hp;
			if (!d.m_HasHp)
				key = -5;
			if (key != m_HpKey)
			{
				m_HpKey = key;
				float w = 0;
				if (d.m_HasHp)
					w = 70 * Math.Clamp(d.m_Hp, 0, 100) / 100.0;
				INT_PTY_HudKit.Rect(m_BarFill, 155, H * 0.5 - 1.5, w, 3, m_K);
			}
			return;
		}
		if (m_HpStyle == INT_PTY_HpStyle.TRK_DOT)
		{
			int dotRgb = 0xFF6F6F6F;
			if (d.m_HasHp)
				dotRgb = INT_PTY_HudKit.HpColor(d.m_Hp);
			INT_PTY_HudKit.Paint(m_Dot, dotRgb, a);
			return;
		}
		int hk = d.m_Hp;
		if (!d.m_HasHp)
			hk = -5;
		if (hk != m_HpKey)
		{
			m_HpKey = hk;
			if (!d.m_HasHp)
				m_HpText.SetText("—");
			else if (m_HpStyle == INT_PTY_HpStyle.TRK_NUM)
				m_HpText.SetText(d.m_Hp.ToString() + " / 100");
			else
				m_HpText.SetText(d.m_Hp.ToString() + "%");
		}
		int txtRgb = 0xFFFFFFFF;
		if (low)
			txtRgb = 0xFFFF6D61;
		m_HpText.Paint(txtRgb, a);
	}

	protected void UpdateDist(INT_PTY_HudRowData d, int key, int distRgb, int accent, float a)
	{
		if (key != m_DistKey)
		{
			m_DistKey = key;
			if (d.m_HasDist)
				m_Dist.SetText(INT_PTY_HudKit.Dist(d.m_Dist));
			else
				m_Dist.SetText("—");
			m_Icons = -1;
		}
		m_Dist.Paint(distRgb, a);
		INT_PTY_HudKit.Vis(m_Arrow, d.m_HasDist);
		if (d.m_HasDist)
			INT_PTY_HudKit.Paint(m_Arrow, accent, a);
	}

	// Status icons right before the distance (their place depends on the distance text width).
	protected void UpdateIcons(INT_PTY_HudRowData d, float a)
	{
		if (d.m_Icons != m_Icons)
		{
			m_Icons = d.m_Icons;
			INT_PTY_HudKit.Vis(m_IcoUncon, (m_Icons & 1) != 0);
			INT_PTY_HudKit.Vis(m_IcoBleed, (m_Icons & 2) != 0);
			INT_PTY_HudKit.Vis(m_IcoLegs, (m_Icons & 4) != 0);
			float distW = INT_PTY_HudKit.TextW(m_Dist.m_Text, 15, false, false, 0);
			float x = 323 - distW - 8 - 7;
			if ((m_Icons & 4) != 0)
			{
				INT_PTY_HudKit.Square(m_IcoLegs, x, H * 0.5, 14, m_K);
				x = x - 16;
			}
			if ((m_Icons & 2) != 0)
			{
				INT_PTY_HudKit.Square(m_IcoBleed, x, H * 0.5, 14, m_K);
				x = x - 16;
			}
			if ((m_Icons & 1) != 0)
				INT_PTY_HudKit.Square(m_IcoUncon, x, H * 0.5, 14, m_K);
		}
		if (m_Icons == 0)
			return;
		INT_PTY_HudKit.Paint(m_IcoUncon, 0xFFE8E8E8, a);
		INT_PTY_HudKit.Paint(m_IcoBleed, 0xFFFF4B3E, a);
		INT_PTY_HudKit.Paint(m_IcoLegs, 0xFFFFC246, a);
	}
}

class INT_PTY_HudTracker : Managed
{
	static const float W = 360;
	static const float PAD_T = 6;				// above the first row (was the 41 px head row)
	static const float PAD_B = 6;
	static const float DEFAULT_X = 34;			// design px
	static const float DEFAULT_Y = 0.3611;		// screen fraction (concept: top 390 of 1080)

	protected Widget m_Layer;
	protected Widget m_Panel;
	protected ImageWidget m_BgSolid;
	protected ImageWidget m_BgGrad;
	protected Widget m_MoveFrame;
	protected ref array<ImageWidget> m_MoveParts;
	protected ref array<ref INT_PTY_HudTrackRow> m_Rows;
	protected ref array<ref INT_PTY_HudRowData> m_Data;
	protected int m_DataCount;
	protected int m_Slots;						// H-4: row slots drawn (friend rows + the «+N» row)
	protected ref INT_PTY_HudText m_MoreText;	// H-4: «+N» = friends that do not fit above the screen bottom
	protected int m_MoreKey = -1;
	protected int m_MoreSlot = -1;
	protected float m_K = 1;
	protected int m_HpStyle;
	protected int m_Height = -1;
	protected bool m_Shown;
	protected bool m_Moving;
	protected float m_ShownAt = -100;
	protected int m_PanelAcc = 1;
	protected float m_PanelA = -1;
	protected float m_PanelX = -10000;
	protected float m_PanelY = -10000;

	void INT_PTY_HudTracker()
	{
		m_Rows = new array<ref INT_PTY_HudTrackRow>();
		m_Data = new array<ref INT_PTY_HudRowData>();
		m_MoveParts = new array<ImageWidget>();
	}

	void Build(Widget layer, float k, int hpStyle)
	{
		Clear();
		m_Layer = layer;
		m_K = k;
		m_PanelA = -1;
		m_PanelX = -10000;
		m_PanelY = -10000;
		m_HpStyle = hpStyle;
		m_Panel = INT_PTY_HudKit.Frame(layer, "INT_PTY_Tracker");
		m_BgSolid = INT_PTY_HudKit.Img(m_Panel, "pty_px_ca", 0xFF080808);
		m_BgGrad = INT_PTY_HudKit.Img(m_Panel, "pty_grad_h_ca", 0xFF080808);
		m_MoveFrame = INT_PTY_HudKit.Frame(m_Panel, "INT_PTY_TrackerMove");
		m_MoveParts.Clear();
		for (int i = 0; i < 5; i++)
		{
			int c = 0xFFFF7A2F;
			string tex = "pty_px_ca";
			ImageWidget mp = INT_PTY_HudKit.Img(m_MoveFrame, tex, c);
			m_MoveParts.Insert(mp);
		}
		if (m_MoveFrame)
			m_MoveFrame.Show(false);
		m_MoreText = new INT_PTY_HudText();
		m_MoreText.Build(m_Panel, 15, INT_UIF_Align.LEFT, true, k, 1.3);
		m_MoreText.Show(false);
		m_MoreKey = -1;
		m_MoreSlot = -1;
		m_Slots = 0;
		m_Height = -1;
		if (m_Panel)
			m_Panel.Show(false);
		m_Shown = false;
	}

	void Clear()
	{
		m_Rows.Clear();
		if (m_Panel)
			m_Panel.Unlink();
		m_Panel = null;
		m_Shown = false;
	}

	Widget GetPanel()
	{
		return m_Panel;
	}

	bool IsActive()
	{
		return m_Shown;
	}

	protected INT_PTY_HudRowData NextData()
	{
		if (m_DataCount >= m_Data.Count())
			m_Data.Insert(new INT_PTY_HudRowData());
		INT_PTY_HudRowData d = m_Data.Get(m_DataCount);
		m_DataCount++;
		return d;
	}

	void SetMoving(bool on)
	{
		m_Moving = on;
	}

	void Update(INT_PTY_HudCtx ctx, INT_PTY_HudCore core, bool forceSamples)
	{
		m_DataCount = 0;
		INT_PTY_Settings st = ctx.m_St;
		bool partOn = INT_PTY_UI.IsPartShown(INT_PTY_UI.PART_TRACKER) && st.TrackerOn == 1;
		bool samples = ctx.m_Edit || m_Moving || forceSamples;
		if (partOn || samples)
			Collect(ctx, core);
		if (m_DataCount == 0 && samples)
			CollectSamples();
		if (m_DataCount == 0 || (!partOn && !samples))
		{
			SetShown(false, ctx.m_Now);
			return;
		}
		SetShown(true, ctx.m_Now);
		// H-4: only the rows that fit between the panel top and the screen bottom (part scale included); the rest is
		// one «+N» row in the last slot
		int fit = RowsFit(ctx.m_Sw, ctx.m_Sh);
		int dataRows = m_DataCount;
		int more = 0;
		if (m_DataCount > fit)
		{
			dataRows = fit - 1;
			more = m_DataCount - dataRows;
		}
		m_Slots = dataRows;
		if (more > 0)
			m_Slots = dataRows + 1;
		float opacity = st.TrackerAlpha * 0.01 * INT_PTY_UI.PartOpacity(INT_PTY_UI.PART_TRACKER);
		if (!partOn)
			opacity = opacity * 0.35;
		int accent = INT_PTY_UI.PartAccent(INT_PTY_UI.PART_TRACKER);
		// panel entrance: slide from -46 px + fade 0.45 s
		float appear = INT_PTY_HudKit.OutCubic(INT_PTY_HudKit.Seg(ctx.m_Now - m_ShownAt, 0, 0.45));
		float a = opacity * appear;
		LayoutPanel(ctx, appear);
		if (accent != m_PanelAcc || Math.AbsFloat(a - m_PanelA) > 0.003)
		{
			m_PanelAcc = accent;
			m_PanelA = a;
			INT_PTY_HudKit.Paint(m_BgSolid, 0xFF080808, 0.8 * a);
			INT_PTY_HudKit.Paint(m_BgGrad, 0xFF080808, 0.8 * a);
		}
		bool blink = st.TrackerBlink == 1;
		for (int i = 0; i < dataRows; i++)
		{
			if (i >= m_Rows.Count())
			{
				INT_PTY_HudTrackRow nr = new INT_PTY_HudTrackRow();
				nr.Build(m_Panel, m_K, m_HpStyle);
				m_Rows.Insert(nr);
			}
			INT_PTY_HudTrackRow row = m_Rows.Get(i);
			row.SetVisible(true);
			row.SetY(PAD_T + i * INT_PTY_HudTrackRow.H);
			row.Update(m_Data.Get(i), a, accent, blink, ctx.m_Now);
		}
		for (int j = dataRows; j < m_Rows.Count(); j++)
		{
			INT_PTY_HudTrackRow hide = m_Rows.Get(j);
			hide.SetVisible(false);
			hide.m_Id = "";
		}
		UpdateMore(dataRows, more, a);
		UpdateMoveFrame(ctx);
	}

	// H-4: how many row slots fit between the panel's own top (player place or default) and the screen bottom.
	protected int RowsFit(float sw, float sh)
	{
		float f = INT_PTY_HudKit.s_F * m_K;
		if (f <= 0 || sh <= 0)
			return 1000;
		float top = DesiredTop(sh);
		float room = (sh - top) / f - PAD_T - PAD_B;
		int fit = (int)Math.Floor(room / INT_PTY_HudTrackRow.H);
		if (fit < 2)
			fit = 2;
		return fit;
	}

	// The panel top before it is kept on the screen (screen px).
	protected float DesiredTop(float sh)
	{
		float nx;
		float ny;
		float top = DEFAULT_Y * sh;
		if (INT_PTY_UI.GetPartPosition(INT_PTY_UI.PART_TRACKER, nx, ny))
			top = ny * sh;
		return Math.Clamp(top, 0, sh);
	}

	protected void UpdateMore(int slot, int more, float a)
	{
		if (!m_MoreText)
			return;
		if (more <= 0)
		{
			m_MoreText.Show(false);
			m_MoreKey = -1;
			return;
		}
		if (more != m_MoreKey || slot != m_MoreSlot)
		{
			m_MoreKey = more;
			m_MoreSlot = slot;
			m_MoreText.SetText("+" + more.ToString());
			float my = PAD_T + slot * INT_PTY_HudTrackRow.H;
			m_MoreText.Rect(39, my, 300, INT_PTY_HudTrackRow.H, m_K);
		}
		m_MoreText.Show(true);
		m_MoreText.Paint(0xFFBDB7AF, a);
	}

	protected void SetShown(bool on, float now)
	{
		if (on == m_Shown)
			return;
		m_Shown = on;
		if (m_Panel)
			m_Panel.Show(on);
		if (on)
			m_ShownAt = now;
	}

	protected void Collect(INT_PTY_HudCtx ctx, INT_PTY_HudCore core)
	{
		if (!INT_PTY_Client.s_Friends)
			return;
		INT_PTY_Settings st = ctx.m_St;
		bool showOffline = st.TrackerOffline == 1;
		foreach (INT_PTY_Friend f : INT_PTY_Client.s_Friends)
		{
			INT_PTY_HudMate mate = core.Mate(f.Id);
			if (!mate)
				continue;
			if (!f.Online && !showOffline)
				continue;
			INT_PTY_HudRowData d = NextData();
			d.m_Id = f.Id;
			d.m_Name = mate.m_Name;
			d.m_Rgb = mate.m_Color;
			d.m_Hp = f.Hp;
			d.m_HasHp = f.HasState;
			d.m_Icons = 0;
			d.m_HasDist = false;
			if (!f.Online)
			{
				d.m_State = INT_PTY_HudRowData.OFFLINE;
				continue;
			}
			if (f.Dead)
			{
				d.m_State = INT_PTY_HudRowData.DEAD;
				continue;
			}
			d.m_State = INT_PTY_HudRowData.NORMAL;
			// 0 HP of a live friend = no body yet (connecting / respawning), not a critical one
			if (f.HasState && f.Hp > 0 && f.Hp < 20)
				d.m_State = INT_PTY_HudRowData.LOW;
			if (st.StatusIcons == 1)
			{
				int bits = INT_PTY_HudTags.StatusBits(f);
				d.m_Icons = bits;
			}
			if (ctx.m_World && f.KnowsPos())
			{
				vector wp = f.WorldPos();
				d.m_HasDist = true;
				float dist = vector.Distance(ctx.m_MyPos, wp);
				float bearing = ctx.RelBearing(wp);
				d.m_Dist = dist;
				d.m_Bearing = bearing;
			}
		}
	}

	// Mover / edit mode without shown friends: rows of the concept so the player sees what he moves.
	protected void CollectSamples()
	{
		SampleRow("sample1", "Витя", 0xFFFF5A4F, INT_PTY_HudRowData.NORMAL, 86, 124, 0);
		SampleRow("sample2", "Kolya_Sniper", 0xFFFFB02E, INT_PTY_HudRowData.LOW, 17, 342, 40);
		SampleRow("sample3", "Дед", 0xFFB48CFF, INT_PTY_HudRowData.DEAD, 0, 0, 0);
		SampleRow("sample4", "Lisa", 0xFF3FD0FF, INT_PTY_HudRowData.NORMAL, 100, 58, 180);
		SampleRow("sample5", "Ghost_RU", 0xFF9AA3AD, INT_PTY_HudRowData.OFFLINE, 0, 0, 0);
	}

	protected void SampleRow(string id, string name, int rgb, int state, int hp, float dist, float bearing)
	{
		INT_PTY_HudRowData d = NextData();
		d.m_Id = id;
		d.m_Name = name;
		d.m_Rgb = rgb;
		d.m_State = state;
		d.m_Hp = hp;
		d.m_HasHp = true;
		d.m_Dist = dist;
		d.m_HasDist = dist > 0;
		d.m_Bearing = bearing;
		d.m_Icons = 0;
	}

	// Panel place and background size; the rows are children of the panel.
	protected void LayoutPanel(INT_PTY_HudCtx ctx, float appear)
	{
		if (!m_Panel)
			return;
		float f = INT_PTY_HudKit.s_F * m_K;
		float h = PAD_T + m_Slots * INT_PTY_HudTrackRow.H + PAD_B;
		int hk = m_Slots;
		if (hk != m_Height)
		{
			m_Height = hk;
			INT_PTY_HudKit.Rect(m_Panel, 0, 0, W, h, m_K);
			// Rect also put the panel at 0,0: forget the cached place so it is set again below (a friend coming online
			// moved the tracker into the top-left corner — stand 2026-10-23)
			m_PanelX = -10000;
			m_PanelY = -10000;
			// whole screen px for both halves and the panel place: a fractional join left a light 1 px seam line
			// (stand 2026-10-23), an overlapping one a dark line
			float pw = Math.Round(W * f);
			float ph = Math.Round(h * f);
			float half = Math.Round(pw * 0.5);
			INT_PTY_HudKit.PlaceAt(m_BgSolid, 0, 0);
			INT_PTY_HudKit.SizeTo(m_BgSolid, half, ph);
			INT_PTY_HudKit.PlaceAt(m_BgGrad, half, 0);
			INT_PTY_HudKit.SizeTo(m_BgGrad, pw - half, ph);
			LayoutMoveFrame(h);
		}
		float px;
		float py;
		GetTopLeft(ctx.m_Sw, ctx.m_Sh, px, py);
		float nx = Math.Round(px - 46 * (1 - appear) * f);
		py = Math.Round(py);
		if (Math.AbsFloat(nx - m_PanelX) > 0.5 || Math.AbsFloat(py - m_PanelY) > 0.5)
		{
			m_PanelX = nx;
			m_PanelY = py;
			INT_PTY_HudKit.PlaceAt(m_Panel, nx, py);
		}
	}

	// Top-left corner in screen px (player place or the default), kept on the screen.
	void GetTopLeft(float sw, float sh, out float px, out float py)
	{
		float f = INT_PTY_HudKit.s_F * m_K;
		float pw = W * f;
		float ph = (PAD_T + Math.Max(m_Slots, 1) * INT_PTY_HudTrackRow.H + PAD_B) * f;
		float nx;
		float ny;
		if (INT_PTY_UI.GetPartPosition(INT_PTY_UI.PART_TRACKER, nx, ny))
		{
			px = nx * sw;
			py = ny * sh;
		}
		else
		{
			px = DEFAULT_X * INT_PTY_HudKit.s_F;
			py = DEFAULT_Y * sh;
		}
		px = Math.Clamp(px, 0, Math.Max(0, sw - pw));
		py = Math.Clamp(py, 0, Math.Max(0, sh - ph));
	}

	bool GetRect(float sw, float sh, out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		if (!m_Panel || !m_Shown)
			return false;
		float f = INT_PTY_HudKit.s_F * m_K;
		GetTopLeft(sw, sh, x, y);
		w = W * f;
		h = (PAD_T + m_Slots * INT_PTY_HudTrackRow.H + PAD_B) * f;
		return true;
	}

	// Orange outline + light tint while the mover or the edit mode is on (INT_PTY_Look.MoveFrame look).
	protected void LayoutMoveFrame(float h)
	{
		if (m_MoveParts.Count() < 5)
			return;
		INT_PTY_HudKit.Rect(m_MoveParts.Get(0), 0, 0, W, h, m_K);
		INT_PTY_HudKit.Rect(m_MoveParts.Get(1), 0, 0, W, 2, m_K);
		INT_PTY_HudKit.Rect(m_MoveParts.Get(2), 0, h - 2, W, 2, m_K);
		INT_PTY_HudKit.Rect(m_MoveParts.Get(3), 0, 0, 2, h, m_K);
		INT_PTY_HudKit.Rect(m_MoveParts.Get(4), W - 2, 0, 2, h, m_K);
	}

	protected void UpdateMoveFrame(INT_PTY_HudCtx ctx)
	{
		bool on = m_Moving || ctx.m_Edit;
		if (!m_MoveFrame)
			return;
		m_MoveFrame.Show(on);
		if (!on)
			return;
		INT_PTY_HudKit.Paint(m_MoveParts.Get(0), 0xFFFF7A2F, 0.16);
		for (int i = 1; i < 5; i++)
		{
			INT_PTY_HudKit.Paint(m_MoveParts.Get(i), 0xFFFF7A2F, 1);
		}
	}
}
