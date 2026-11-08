// INT PartyMode (client) — ping markers P5 «ВСПЫШКА» (design/concepts_v1/p5_flash_sheet.png, src/p5_flash.css), rebuilt
// 1:1 with pre-baked white-mask textures tinted in script:
//   0–55 ms     star flare (pty_fx_flare_ca) 15 → 110 % turning −25° → 0°, white flash disk (pty_fx_flash_ca)
//   55–250 ms   the flare collapses (110 → 62 → 0 %, turning to 40°)
//   110 ms      the icon pops 180 → 100 %, white → the owner colour (320 ms); dark outline twin under it
//   260–960 ms  ONE radar sweep (pty_fx_sweep_ca turned 0 → 360°)
//   then        the colour halo (pty_fx_halo_ca) breathes 55 → 100 % over 2.4 s; distance label above with a colour
//               underline wiped in; the owner's short name under a friend's ping
//   remove      290 ms: reverse flash, the icon whitens and shrinks to 0, labels and halo fade (INT_PTY_Sfx.PingOut)
// Constant screen size × PingSize %: at 100 % the whole P5 group is HALF the sheet size (owner 2026-10-23: icon 22 px,
// flare / flash / sweep / halo / outline in the sheet proportions to it), the owner nick 25 % and the distance label
// 50 % smaller than the sheet, never under the smallest crisp size. Opacity by distance (opaque farther than PingFarM, gone at PingNearM, curve of the
// sheet: 1 − (1 − t)³ on a log scale); a friend's ping farther than PingViewM is not drawn at all (fades out over the
// last 10 %; owner 2026-11-01, PvP feedback: pings 10–15 km away cluttered the screen); dimmed by PingAimDim % as the crosshair (screen centre) comes near it: full dim
// inside AIM_INNER, eased back to full opacity at AIM_OUTER (both in icon radii, so they follow PingSize and the
// screen height like the icon).
// A ping replaced in its slot (5th ping) plays the remove animation at its old place (a ghost item).
// Pool of items rebound every frame by (owner, slot) — no pointer to a ping mark is kept between frames.

class INT_PTY_HudPing : Managed
{
	static const float ICON = 44;
	static const float HALO = 136;
	static const float SWEEP = 128;
	static const float FLASH = 120;
	static const float DARK = 86;
	static const float FLARE = 220;
	static const float GONE_S = 0.3;
	// Strength of the bright layers. The UI blends in linear light, so a soft white / colour glow reads ~2-3x
	// stronger than in the browser concept; these scale the concept alphas down to the look of the sheet
	// (tuned on the stand with INT_PTY_Hud.TestCommand "tune <name> <k>" against design/compare/hud_p5_*.png).
	static float s_HaloK = 0.38;
	static float s_FlashK = 0.35;
	static float s_SweepK = 0.55;
	static float s_FlareK = 0.75;
	static float s_FlareWhite = 0.85;		// flare tint: owner colour mixed toward white (the star is white in P5)
	static float s_GlowK = 0.9;				// owner-colour glow under the star (P5 flare = white rays + colour glow)

	Widget m_Root;
	ImageWidget m_Halo;
	ImageWidget m_Sweep;
	ImageWidget m_Flash;
	ImageWidget m_Dark;
	ImageWidget m_LabPlate;
	ImageWidget m_NamePlate;
	ImageWidget m_Out;
	ImageWidget m_Icon;
	ImageWidget m_Flare;
	ImageWidget m_FlareGlow;
	ImageWidget m_Under;
	ref INT_PTY_HudText m_Lab;
	ref INT_PTY_HudText m_Name;

	float m_K = 1;
	float m_LabSize = 18;
	float m_NameSize = 14;
	string m_Owner;
	int m_Slot = -1;
	float m_Born = -1;
	bool m_Seen;
	bool m_Visible;
	bool m_GoneSeen;
	bool m_Ghost;
	float m_GhostAt;
	vector m_Where;
	int m_Shape = -1;
	int m_Rgb;
	bool m_HasName;
	string m_NameText = "<none>";
	int m_DistKey = -1;
	float m_Dim = 1;
	float m_Sx = -10000;
	float m_Sy = -10000;
	float m_LabW;
	bool m_Rest;				// resting look painted: only the halo breathes until something changes
	float m_LastMult = -1;
	float m_HaloSide;
	float m_HaloA = -1;
	int m_HaloRgb;
	int m_FadeKey = -1;
	float m_Fade;
	int m_LastLab;

	void Build(Widget layer, float k, float labSize, float nameSize)
	{
		m_K = k;
		m_LabSize = labSize;
		m_NameSize = nameSize;
		m_Root = INT_PTY_HudKit.Frame(layer, "INT_PTY_Ping");
		m_Halo = INT_PTY_HudKit.Img(m_Root, "pty_fx_halo_ca", 0xFFFFFFFF);
		m_Sweep = INT_PTY_HudKit.Img(m_Root, "pty_fx_sweep_ca", 0xFFFFFFFF);
		m_Flash = INT_PTY_HudKit.Img(m_Root, "pty_fx_flash_ca", 0xFFFFFFFF);
		m_Dark = INT_PTY_HudKit.Img(m_Root, "pty_halo_dark_ca", 0xFF000000);
		m_LabPlate = INT_PTY_HudKit.Img(m_Root, "pty_halo_dark_ca", 0xFF000000);
		m_NamePlate = INT_PTY_HudKit.Img(m_Root, "pty_halo_dark_ca", 0xFF000000);
		m_Out = INT_PTY_HudKit.ImgPath(m_Root, INT_PTY_UI.PingTex(0, true), 0xFF000000);
		m_Icon = INT_PTY_HudKit.ImgPath(m_Root, INT_PTY_UI.PingTex(0, false), 0xFFFFFFFF);
		m_Shape = 0;
		m_FlareGlow = INT_PTY_HudKit.Img(m_Root, "pty_fx_halo_ca", 0xFFFFFFFF);
		m_Flare = INT_PTY_HudKit.Img(m_Root, "pty_fx_flare_ca", 0xFFFFFFFF);
		m_Lab = new INT_PTY_HudText();
		m_Lab.Build(m_Root, labSize, INT_UIF_Align.CENTER, true, 1.0, 0.4);
		m_Under = INT_PTY_HudKit.Img(m_Root, "pty_px_ca", 0xFFFFFFFF);
		m_Name = new INT_PTY_HudText();
		m_Name.Build(m_Root, nameSize, INT_UIF_Align.CENTER, true, 1.0, 0.4);
		// fixed geometry (design px × k around the ping point); animated sizes are set per frame
		float lh = labSize + 6;
		m_Lab.Rect(-150 * k, -38 * k - lh, 300 * k, lh, 1.0);
		float nh = nameSize + 6;
		m_Name.Rect(-150 * k, 30 * k, 300 * k, nh, 1.0);
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

	void Free()
	{
		m_Owner = "";
		m_Slot = -1;
		m_Born = -1;
		m_Ghost = false;
		m_GoneSeen = false;
		m_DistKey = -1;
		m_Dim = 1;
		m_Rest = false;
		m_HaloA = -1;
		m_FadeKey = -1;
		SetVisible(false);
	}

	void SetLook(int shape, int rgb, string ownerName)
	{
		if (shape != m_Shape)
		{
			m_Shape = shape;
			if (m_Icon)
				m_Icon.LoadImageFile(0, INT_PTY_UI.PingTex(shape, false));
			if (m_Out)
				m_Out.LoadImageFile(0, INT_PTY_UI.PingTex(shape, true));
		}
		if (rgb != m_Rgb)
			m_Rest = false;
		m_Rgb = rgb;
		m_HasName = ownerName != "";
		if (ownerName != m_NameText)
		{
			m_NameText = ownerName;
			m_Name.SetText(ownerName);
			m_DistKey = -1;
			m_Name.Show(m_HasName);
			if (m_NamePlate)
				m_NamePlate.Show(m_HasName);
		}
	}

	void Place(float sx, float sy)
	{
		if (Math.AbsFloat(sx - m_Sx) < 0.5 && Math.AbsFloat(sy - m_Sy) < 0.5)
			return;
		m_Sx = sx;
		m_Sy = sy;
		if (m_Root)
			m_Root.SetPos(sx, sy);
	}

	// Distance label, rebuilt only when the shown value changes.
	void SetDist(float dist)
	{
		int key = (int)Math.Round(dist);
		if (dist >= 999.5)
			key = 100000 + (int)Math.Round(dist / 100.0);
		if (key == m_DistKey)
			return;
		m_DistKey = key;
		m_Rest = false;
		string t = INT_PTY_HudKit.Dist(dist);
		m_Lab.SetText(t);
		m_LabW = INT_PTY_HudKit.TextW(t, m_LabSize, false, true, 0.4) / m_K;
		float k = m_K;
		float lh = m_LabSize + 6;
		float plateW = m_LabW + 44;
		INT_PTY_HudKit.Rect(m_LabPlate, -plateW * 0.5, -38 - lh / k - 9, plateW, lh / k + 18, k);
		float nameW = INT_PTY_HudKit.TextW(m_NameText, m_NameSize, false, true, 0.4) / k + 44;
		float nh = m_NameSize + 6;
		INT_PTY_HudKit.Rect(m_NamePlate, -nameW * 0.5, 30 - 7, nameW, nh / k + 14, k);
	}

	// One frame of the P5 animation. age = s since the ping appeared; gone = s since the remove started (-1 = alive);
	// mult = distance fade × aim dim × part opacity; labRgb = distance label colour (part accent).
	void Animate(float age, float gone, float mult, int labRgb)
	{
		float k = m_K;
		bool removing = gone >= 0;
		// at rest (after the appear animation) only the halo breathes while nothing else changes
		bool rest = !removing && age > 1.0;
		if (rest && m_Rest && labRgb == m_LastLab && Math.AbsFloat(mult - m_LastMult) < 0.003)
		{
			AnimHalo(age, 1, mult);
			return;
		}
		m_Rest = rest;
		m_LastMult = mult;
		m_LastLab = labRgb;
		float fadeOut = 1;
		if (removing)
			fadeOut = 1 - INT_PTY_HudKit.In(INT_PTY_HudKit.Seg(gone, 0, 0.15));
		AnimFlare(age, gone, mult);
		AnimFlash(age, removing, mult);
		AnimSweep(age, fadeOut, mult);
		AnimHalo(age, fadeOut, mult);
		float darkA = INT_PTY_HudKit.Out(INT_PTY_HudKit.Seg(age, 0.1, 0.26)) * 0.55;
		if (removing)
			darkA = darkA * (1 - INT_PTY_HudKit.In(INT_PTY_HudKit.Seg(gone, 0.06, 0.22)));
		INT_PTY_HudKit.Square(m_Dark, 0, 0, DARK, k);
		INT_PTY_HudKit.Paint(m_Dark, 0xFF000000, darkA * mult);
		AnimIcon(age, gone, mult);
		AnimLabels(age, gone, mult, labRgb);
	}

	protected void AnimFlare(float age, float gone, float mult)
	{
		float sc = 0;
		float rot = 0;
		float op = 0;
		if (gone < 0)
		{
			if (age < 0.055)
			{
				float p1 = INT_PTY_HudKit.Bezier(INT_PTY_HudKit.Seg(age, 0, 0.055), 0.2, 0.9, 0.3, 1);
				sc = 0.15 + 0.95 * p1;
				rot = -25 + 25 * p1;
				op = p1;
			}
			else if (age < 0.12)
			{
				float p2 = INT_PTY_HudKit.InOut(INT_PTY_HudKit.Seg(age, 0.055, 0.12));
				sc = 1.1 - 0.48 * p2;
				rot = 14 * p2;
				op = 1;
			}
			else if (age < 0.25)
			{
				float p3 = INT_PTY_HudKit.In(INT_PTY_HudKit.Seg(age, 0.12, 0.25));
				sc = 0.62 - 0.62 * p3;
				rot = 14 + 26 * p3;
				op = 1 - p3;
			}
		}
		else
		{
			float g = gone - 0.02;
			if (g >= 0 && g < 0.091)
			{
				float q1 = INT_PTY_HudKit.Out(INT_PTY_HudKit.Seg(g, 0, 0.091));
				sc = 0.62 * q1;
				rot = 18 * q1;
				op = 1;
			}
			else if (g >= 0.091 && g < 0.26)
			{
				float q2 = INT_PTY_HudKit.In(INT_PTY_HudKit.Seg(g, 0.091, 0.26));
				sc = 0.62 - 0.58 * q2;
				rot = 18 + 27 * q2;
				op = 1 - q2;
			}
		}
		if (op <= 0.001 || sc <= 0.001)
		{
			INT_PTY_HudKit.Vis(m_Flare, false);
			INT_PTY_HudKit.Vis(m_FlareGlow, false);
			return;
		}
		INT_PTY_HudKit.Vis(m_Flare, true);
		INT_PTY_HudKit.Vis(m_FlareGlow, true);
		INT_PTY_HudKit.Square(m_FlareGlow, 0, 0, FLARE * sc * 0.9, m_K);
		INT_PTY_HudKit.Paint(m_FlareGlow, m_Rgb, op * s_GlowK * mult);
		INT_PTY_HudKit.Square(m_Flare, 0, 0, FLARE * sc, m_K);
		INT_PTY_HudKit.Turn(m_Flare, rot);
		INT_PTY_HudKit.Paint(m_Flare, INT_PTY_HudKit.Mix(m_Rgb, 0xFFFFFFFF, s_FlareWhite), op * s_FlareK * mult);
	}

	protected void AnimFlash(float age, bool removing, float mult)
	{
		if (removing || age >= 0.28)
		{
			INT_PTY_HudKit.Vis(m_Flash, false);
			return;
		}
		float sc;
		float op;
		if (age < 0.05)
		{
			float p = INT_PTY_HudKit.Out(INT_PTY_HudKit.Seg(age, 0, 0.05));
			sc = 0.2 + 0.7 * p;
			op = 0.95 * p;
		}
		else
		{
			float q = INT_PTY_HudKit.Out(INT_PTY_HudKit.Seg(age, 0.05, 0.28));
			sc = 0.9 + 0.6 * q;
			op = 0.95 * (1 - q);
		}
		INT_PTY_HudKit.Vis(m_Flash, true);
		INT_PTY_HudKit.Square(m_Flash, 0, 0, FLASH * sc, m_K);
		INT_PTY_HudKit.Paint(m_Flash, 0xFFFFFFFF, op * op * s_FlashK * mult);
	}

	protected void AnimSweep(float age, float fadeOut, float mult)
	{
		float t = age - 0.26;
		if (t < 0 || t >= 0.7)
		{
			INT_PTY_HudKit.Vis(m_Sweep, false);
			return;
		}
		float u = t / 0.7;
		float rot = 360 * INT_PTY_HudKit.Bezier(u, 0.35, 0.2, 0.55, 1);
		float op = 1;
		if (u < 0.12)
			op = u / 0.12;
		else if (u > 0.72)
			op = 1 - (u - 0.72) / 0.28;
		INT_PTY_HudKit.Vis(m_Sweep, true);
		INT_PTY_HudKit.Square(m_Sweep, 0, 0, SWEEP, m_K);
		INT_PTY_HudKit.Turn(m_Sweep, rot);
		INT_PTY_HudKit.Paint(m_Sweep, INT_PTY_HudKit.Mix(m_Rgb, 0xFFFFFFFF, 0.2), op * fadeOut * s_SweepK * mult);
	}

	protected void AnimHalo(float age, float fadeOut, float mult)
	{
		float fin = INT_PTY_HudKit.Out(INT_PTY_HudKit.Seg(age, 0.16, 0.48));
		float br = 0;
		if (age >= 0.9)
		{
			float ph = (age - 0.9) / 2.4;
			ph = ph - Math.Floor(ph);
			br = 0.5 - 0.5 * Math.Cos(ph * Math.PI2);
		}
		float op = (0.55 + 0.45 * br) * fin * fadeOut;
		float sc = 0.92 + 0.14 * br;
		if (op <= 0.001)
		{
			INT_PTY_HudKit.Vis(m_Halo, false);
			m_HaloA = -1;
			return;
		}
		// the breathing is slow: the widget is touched only when the size / alpha moved enough to be seen
		float side = HALO * sc;
		float ha = op * s_HaloK * mult;
		if (m_HaloA >= 0 && m_HaloRgb == m_Rgb && Math.AbsFloat(side - m_HaloSide) < 0.6 && Math.AbsFloat(ha - m_HaloA) < 0.006)
			return;
		m_HaloSide = side;
		m_HaloA = ha;
		m_HaloRgb = m_Rgb;
		INT_PTY_HudKit.Vis(m_Halo, true);
		INT_PTY_HudKit.Square(m_Halo, 0, 0, side, m_K);
		INT_PTY_HudKit.Paint(m_Halo, m_Rgb, ha);
	}

	protected void AnimIcon(float age, float gone, float mult)
	{
		float s = INT_PTY_HudKit.Seg(age, 0.11, 0.37);
		float sc = 1.8 - 0.8 * INT_PTY_HudKit.Bezier(s, 0.2, 0.8, 0.3, 1);
		float op = INT_PTY_HudKit.Seg(age, 0.11, 0.214);
		float tint = INT_PTY_HudKit.Out(INT_PTY_HudKit.Seg(age, 0.14, 0.46));
		int rgb = INT_PTY_HudKit.Mix(0xFFFFFFFF, m_Rgb, tint);
		if (gone >= 0)
		{
			float shrink = 1 - INT_PTY_HudKit.In(INT_PTY_HudKit.Seg(gone, 0, 0.17));
			sc = sc * shrink;
			op = op * shrink;
			rgb = INT_PTY_HudKit.Mix(rgb, 0xFFFFFFFF, INT_PTY_HudKit.Seg(gone, 0, 0.12));
		}
		if (op <= 0.001 || sc <= 0.001)
		{
			INT_PTY_HudKit.Vis(m_Icon, false);
			INT_PTY_HudKit.Vis(m_Out, false);
			return;
		}
		INT_PTY_HudKit.Vis(m_Icon, true);
		INT_PTY_HudKit.Vis(m_Out, true);
		INT_PTY_HudKit.Square(m_Out, 0, 0, ICON * sc, m_K);
		INT_PTY_HudKit.Square(m_Icon, 0, 0, ICON * sc, m_K);
		INT_PTY_HudKit.Paint(m_Out, 0xFF000000, 0.82 * op * mult);
		INT_PTY_HudKit.Paint(m_Icon, rgb, op * mult);
	}

	protected void AnimLabels(float age, float gone, float mult, int labRgb)
	{
		float labIn = INT_PTY_HudKit.Out(INT_PTY_HudKit.Seg(age, 0.33, 0.55));
		float nameIn = INT_PTY_HudKit.Out(INT_PTY_HudKit.Seg(age, 0.44, 0.66));
		float wipe = INT_PTY_HudKit.Out(INT_PTY_HudKit.Seg(age, 0.38, 0.64));
		if (gone >= 0)
		{
			float fade = 1 - INT_PTY_HudKit.In(INT_PTY_HudKit.Seg(gone, 0, 0.14));
			labIn = labIn * fade;
			nameIn = nameIn * fade;
		}
		float k = m_K;
		float f = INT_PTY_HudKit.s_F;
		float lh = m_LabSize + 6;
		float dy = (1 - labIn) * 8 * k;
		if (m_Lab.m_A)
			m_Lab.m_A.SetPos(-150 * k * f, (-38 * k - lh + dy) * f);
		if (m_Lab.m_B)
			m_Lab.m_B.SetPos((-150 * k + m_Lab.m_Dx) * f, (-38 * k - lh + dy) * f);
		m_Lab.Paint(labRgb, labIn * mult);
		INT_PTY_HudKit.Paint(m_LabPlate, 0xFF000000, 0.6 * labIn * mult);
		float uw = (m_LabW + 4) * wipe;
		if (uw < 0.5)
		{
			INT_PTY_HudKit.Vis(m_Under, false);
		}
		else
		{
			INT_PTY_HudKit.Vis(m_Under, true);
			INT_PTY_HudKit.Rect(m_Under, -uw * 0.5, -35 + dy / k, uw, 2, k);
			INT_PTY_HudKit.Paint(m_Under, m_Rgb, labIn * mult);
		}
		if (!m_HasName)
			return;
		m_Name.Paint(m_Rgb, nameIn * mult);
		INT_PTY_HudKit.Paint(m_NamePlate, 0xFF000000, 0.55 * nameIn * mult);
	}

	void GetRect(out float x, out float y, out float w, out float h)
	{
		float f = INT_PTY_HudKit.s_F * m_K;
		x = m_Sx - 70 * f;
		y = m_Sy - 75 * f;
		w = 140 * f;
		h = 130 * f;
	}
}

class INT_PTY_HudPings : Managed
{
	// AIM DIMMING ZONE — owner tuning point (2026-10-23: «зона в три раза шире»). Radii around the crosshair in ICON
	// RADII (icon radius = 11 design px at PingSize 100 %, design px = 1/1080 of the screen height):
	//   inside AIM_INNER the ping is dimmed by the full PingAimDim %, between AIM_INNER and AIM_OUTER the dimming eases
	//   out (smoothstep), outside AIM_OUTER no dimming. At PingSize 100 % and PingAimZone 100 % (the player's zone
	//   setting, 50–300 %, multiplies both): inner 16.5 px, outer 90 px (the old zone was a hard 30 px circle).
	static const float AIM_INNER = 1.5;
	static const float AIM_OUTER = 8.2;
	static const float LIFT = 0.1;			// m above the hit point
	static const float GROUP = 0.5;			// the P5 group at PingSize 100 % vs the sheet (owner 2026-10-23)
	static const float VIEW_EDGE = 0.1;		// a friend's ping fades out over the last 10 % of PingViewM
	static float s_LabPx = 9;				// distance label at 100 % (sheet 18)
	static float s_NamePx = 10.5;			// owner nick at 100 % (sheet 14)
	static float s_MinPx = 9;				// smallest size the Metron SDF draws crisply at 1080p (stand: 8 px goes soft)

	protected Widget m_Layer;
	protected ref array<ref INT_PTY_HudPing> m_Items;
	protected float m_K = 1;
	protected float m_LabSize = 18;
	protected float m_NameSize = 14;
	protected float m_LnSpan = 1;			// ln(far / near)
	protected float m_Near = 20;
	protected float m_Far = 800;
	protected float m_View = 1000;
	protected int m_Active;
	protected float m_AimCx;				// stand report (AimReport): crosshair and zone of the last frame
	protected float m_AimCy;
	protected float m_AimIn;
	protected float m_AimOut;
	protected float m_AimTo = 1;
	protected ref array<INT_PTY_HudPing> m_Hint;		// item per mark index last frame
	// stand: a frozen frame of the P5 animation (INT_PTY_Hud.TestCommand "fx appear|remove <ms>")
	static int s_FxMode;
	static float s_FxMs;

	void INT_PTY_HudPings()
	{
		m_Hint = new array<INT_PTY_HudPing>();
		m_Items = new array<ref INT_PTY_HudPing>();
	}

	// k = PingSize × part scale (60–160 % scale from the new default); the geometry runs at k × GROUP, the texts at
	// k × their 100 % size, never under s_MinPx.
	void Build(Widget layer, float k)
	{
		Clear();
		m_Layer = layer;
		m_K = k * GROUP;
		m_LabSize = Math.Clamp(s_LabPx * k, s_MinPx, 24);
		m_NameSize = Math.Clamp(s_NamePx * k, s_MinPx, 20);
	}

	// viewM = PingViewM: a friend's ping is drawn up to this distance (fading out over its last VIEW_EDGE), my own
	// pings at any distance.
	void SetFade(int nearM, int farM, int viewM)
	{
		float newNear = Math.Max(1, nearM);
		float newFar = Math.Max(newNear + 1, farM);
		float newView = Math.Max(1, viewM);
		// the per-ping fade is cached by distance: a changed curve must be recomputed for pings already on screen
		if (newNear != m_Near || newFar != m_Far || newView != m_View)
		{
			foreach (INT_PTY_HudPing fp : m_Items)
			{
				fp.m_FadeKey = -1;
			}
		}
		m_Near = newNear;
		m_Far = newFar;
		m_View = newView;
		m_LnSpan = INT_PTY_HudKit.Ln(m_Far / m_Near);
		if (m_LnSpan < 0.001)
			m_LnSpan = 0.001;
	}

	void Clear()
	{
		foreach (INT_PTY_HudPing p : m_Items)
		{
			p.Destroy();
		}
		m_Items.Clear();
		m_Active = 0;
	}

	int ActiveCount()
	{
		return m_Active;
	}

	// Sheet curve: 1 from PingFarM, 0 at PingNearM, 1 − (1 − t)³ on a log scale (~99 % at 400 m, 82 % at 100 m,
	// 46 % at 40 m for 800 / 20).
	float DistFade(float dist)
	{
		if (dist >= m_Far)
			return 1;
		if (dist <= m_Near)
			return 0;
		float t = INT_PTY_HudKit.Ln(dist / m_Near) / m_LnSpan;
		float u = 1 - t;
		return 1 - u * u * u;
	}

	// A friend's ping near the PingViewM edge: 1 up to (1 − VIEW_EDGE) × view, linear to 0 at the view distance.
	float ViewFade(float dist)
	{
		if (dist >= m_View)
			return 0;
		float edge = m_View * VIEW_EDGE;
		float from = m_View - edge;
		if (dist <= from || edge < 0.001)
			return 1;
		return (m_View - dist) / edge;
	}

	protected INT_PTY_HudPing FindBound(string owner, int slot)
	{
		foreach (INT_PTY_HudPing p : m_Items)
		{
			if (!p.m_Ghost && p.m_Slot == slot && p.m_Owner == owner)
				return p;
		}
		return null;
	}

	protected INT_PTY_HudPing TakeFree()
	{
		foreach (INT_PTY_HudPing p : m_Items)
		{
			if (!p.m_Ghost && !p.m_Seen && p.m_Owner == "")
				return p;
		}
		INT_PTY_HudPing n = new INT_PTY_HudPing();
		n.Build(m_Layer, m_K, m_LabSize, m_NameSize);
		m_Items.Insert(n);
		return n;
	}

	void Update(INT_PTY_HudCtx ctx, INT_PTY_HudCore core)
	{
		foreach (INT_PTY_HudPing p : m_Items)
		{
			p.m_Seen = false;
		}
		m_Active = 0;
		bool partOn = INT_PTY_UI.IsPartShown(INT_PTY_UI.PART_PINGS);
		if (s_FxMode != 0)
			UpdateFx(ctx);
		else if (ctx.m_Edit)
			UpdateSamples(ctx, core);
		else if (partOn && ctx.m_World && INT_PTY_Client.s_Pings)
			UpdateWorld(ctx, core);
		UpdateGhosts(ctx);
		foreach (INT_PTY_HudPing q : m_Items)
		{
			if (q.m_Seen || q.m_Ghost)
				continue;
			if (q.m_Owner != "")
				q.Free();
		}
	}

	// Stand: every shown ping with its distance from the crosshair (design px and icon radii) and its aim factor.
	string AimReport()
	{
		float f = INT_PTY_HudKit.s_F;
		if (f <= 0)
			f = 1;
		float inPx = m_AimIn / f;
		float outPx = m_AimOut / f;
		string sIn = inPx.ToString();
		string sOut = outPx.ToString();
		string sTo = m_AimTo.ToString();
		string r = "zone in " + sIn + " out " + sOut + " px, dimTo " + sTo;
		float iconR = INT_PTY_HudPing.ICON * 0.5 * f * m_K;
		foreach (INT_PTY_HudPing p : m_Items)
		{
			if (!p.m_Visible || p.m_Ghost)
				continue;
			float ax = p.m_Sx - m_AimCx;
			float ay = p.m_Sy - m_AimCy;
			float d = Math.Sqrt(ax * ax + ay * ay);
			float radii = 0;
			if (iconR > 0)
				radii = d / iconR;
			float want = AimDim(ax, ay, m_AimIn, m_AimOut, m_AimTo);
			int ol = p.m_Owner.Length();
			string tail = p.m_Owner;
			if (ol > 2)
				tail = p.m_Owner.Substring(ol - 2, 2);
			float dPx = d / f;
			string sSlot = p.m_Slot.ToString();
			string sD = dPx.ToString();
			string sR = radii.ToString();
			string sDim = p.m_Dim.ToString();
			string sWant = want.ToString();
			r = r + " | " + tail + "/" + sSlot + " d " + sD + " px (" + sR + " r) dim " + sDim + " want " + sWant;
		}
		return r;
	}

	// Opacity factor of a ping (ax, ay) screen px from the crosshair: dimTo inside `inner`, 1 outside `outer`,
	// smoothstep between.
	static float AimDim(float ax, float ay, float inner, float outer, float dimTo)
	{
		float d2 = ax * ax + ay * ay;
		if (d2 >= outer * outer)
			return 1;
		if (d2 <= inner * inner)
			return dimTo;
		float d = Math.Sqrt(d2);
		float span = outer - inner;
		if (span < 0.001)
			return 1;
		float u = (d - inner) / span;
		float e = u * u * (3 - 2 * u);
		return dimTo + (1 - dimTo) * e;
	}

	protected void UpdateWorld(INT_PTY_HudCtx ctx, INT_PTY_HudCore core)
	{
		INT_PTY_Settings st = ctx.m_St;
		string me = INT_PTY_Client.s_MySteam;
		float opacity = INT_PTY_UI.PartOpacity(INT_PTY_UI.PART_PINGS);
		int labRgb = INT_PTY_UI.PartAccent(INT_PTY_UI.PART_PINGS);
		float dimTo = 1 - st.PingAimDim * 0.01;
		// the player's zone setting PingAimZone (50–300 % of the standard zone) widens / narrows both radii
		float zoneK = st.PingAimZone * 0.01;
		float iconR = INT_PTY_HudPing.ICON * 0.5 * INT_PTY_HudKit.s_F * m_K;
		float aimIn = AIM_INNER * iconR * zoneK;
		float aimOut = AIM_OUTER * iconR * zoneK;
		m_AimIn = aimIn;
		m_AimOut = aimOut;
		m_AimTo = dimTo;
		float cx = ctx.m_Sw * 0.5;
		float cy = ctx.m_Sh * 0.5;
		m_AimCx = cx;
		m_AimCy = cy;
		float now = ctx.m_Now;
		int myShape = INT_PTY_UI.PingShapeOf(me, true);
		int myRgb = INT_PTY_UI.PingColorOf(me, true);
		int mi = -1;
		foreach (INT_PTY_PingMark mark : INT_PTY_Client.s_Pings)
		{
			mi++;
			bool mine = mark.Owner == me;
			float dist = vector.Distance(ctx.m_MyPos, mark.Where);
			INT_PTY_HudMate mate = null;
			if (!mine)
			{
				mate = core.Mate(mark.Owner);
				if (!mate)
					continue;
				// a friend's ping beyond PingViewM (owner 2026-11-01, PvP feedback): no marker, no edge arrow, no
				// remove sound; its item is freed and it comes back at rest (no flash) once the player is in range
				if (dist >= m_View)
					continue;
			}
			// the item of the same mark index last frame is checked first (the list rarely changes order)
			INT_PTY_HudPing item = null;
			if (mi < m_Hint.Count())
			{
				INT_PTY_HudPing hinted = m_Hint.Get(mi);
				if (hinted && !hinted.m_Ghost && hinted.m_Slot == mark.SlotIdx && hinted.m_Owner == mark.Owner)
					item = hinted;
			}
			if (!item)
				item = FindBound(mark.Owner, mark.SlotIdx);
			if (item && item.m_Born != mark.Born && mark.Gone == 0)
			{
				// the slot got a new place: the old ping plays its remove animation where it stood
				if (item.m_Visible)
					StartGhost(item, now);
				item.m_Born = mark.Born;
				item.m_GoneSeen = false;
				item.m_DistKey = -1;
			}
			if (!item)
			{
				item = TakeFree();
				item.m_Owner = mark.Owner;
				item.m_Slot = mark.SlotIdx;
				item.m_Born = mark.Born;
				item.m_DistKey = -1;
				item.m_Dim = 1;
				// a ping already on its way out when first seen: no sound
				item.m_GoneSeen = mark.Gone > 0;
			}
			item.m_Seen = true;
			while (m_Hint.Count() <= mi)
			{
				m_Hint.Insert(null);
			}
			m_Hint.Set(mi, item);
			int shape = myShape;
			int rgb = myRgb;
			string ownerName = "";
			if (!mine)
			{
				shape = mate.m_PingShape;
				rgb = mate.m_PingColor;
				ownerName = mate.m_Short;
			}
			item.SetLook(shape, rgb, ownerName);
			item.m_Where = mark.Where;
			float gone = -1;
			if (mark.Gone > 0)
			{
				gone = now - mark.Gone;
				if (!item.m_GoneSeen)
				{
					item.m_GoneSeen = true;
					INT_PTY_Sfx.PingOut();
				}
			}
			if (gone > INT_PTY_HudPing.GONE_S)
			{
				item.SetVisible(false);
				continue;
			}
			vector at = mark.Where;
			at[1] = at[1] + LIFT;
			float sx;
			float sy;
			// too close to be drawn (PingNearM): no marker and no edge arrow either
			int fadeKey = (int)(dist * 4);
			if (fadeKey != item.m_FadeKey)
			{
				item.m_FadeKey = fadeKey;
				float df = DistFade(dist);
				if (!mine)
					df = df * ViewFade(dist);
				item.m_Fade = df;
			}
			float fade = item.m_Fade;
			if (fade <= 0.001 && gone < 0)
			{
				item.SetVisible(false);
				continue;
			}
			bool front = ctx.Project(at, sx, sy);
			// on screen = the icon and its labels fit (40 px inside the edges); else the edge arrow takes over
			if (!front || !ctx.OnScreen(sx, sy, -40))
			{
				item.SetVisible(false);
				if (ctx.m_Arrows && gone < 0)
					core.Arrows().Add(ctx, at, rgb, dist);
				continue;
			}
			float ax = sx - cx;
			float ay = sy - cy;
			float target = AimDim(ax, ay, aimIn, aimOut, dimTo);
			float step = Math.Clamp(ctx.m_Dt * 9.0, 0, 1);
			item.m_Dim = item.m_Dim + (target - item.m_Dim) * step;
			// fully dimmed under the crosshair (PingAimDim up to 100 %, e.g. through a scope): hidden, nothing painted
			if (gone < 0 && fade * item.m_Dim * opacity <= 0.004)
			{
				item.SetVisible(false);
				item.m_LastMult = -1;
				continue;
			}
			item.Place(sx, sy);
			item.SetDist(dist);
			item.SetVisible(true);
			item.Animate(now - mark.Born, gone, fade * item.m_Dim * opacity, labRgb);
			m_Active++;
		}
	}

	protected void StartGhost(INT_PTY_HudPing from, float now)
	{
		INT_PTY_HudPing g = TakeFree();
		g.m_Ghost = true;
		g.m_GhostAt = now;
		g.m_Owner = from.m_Owner;
		g.m_Slot = -2;
		g.m_Born = from.m_Born;
		g.m_Where = from.m_Where;
		g.m_Dim = from.m_Dim;
		g.SetLook(from.m_Shape, from.m_Rgb, from.m_NameText);
		g.m_DistKey = -1;
	}

	protected void UpdateGhosts(INT_PTY_HudCtx ctx)
	{
		int labRgb = INT_PTY_UI.PartAccent(INT_PTY_UI.PART_PINGS);
		float opacity = INT_PTY_UI.PartOpacity(INT_PTY_UI.PART_PINGS);
		foreach (INT_PTY_HudPing g : m_Items)
		{
			if (!g.m_Ghost)
				continue;
			float t = ctx.m_Now - g.m_GhostAt;
			if (t > INT_PTY_HudPing.GONE_S || !ctx.m_World || ctx.m_Edit)
			{
				g.Free();
				continue;
			}
			vector at = g.m_Where;
			at[1] = at[1] + LIFT;
			float sx;
			float sy;
			if (!ctx.Project(at, sx, sy) || !ctx.OnScreen(sx, sy, 20))
			{
				g.SetVisible(false);
				continue;
			}
			float dist = vector.Distance(ctx.m_MyPos, g.m_Where);
			float gf = DistFade(dist);
			if (g.m_Owner != INT_PTY_Client.s_MySteam)
				gf = gf * ViewFade(dist);
			g.Place(sx, sy);
			g.SetDist(dist);
			g.SetVisible(true);
			g.Animate(5.0, t, gf * g.m_Dim * opacity, labRgb);
		}
	}

	// One ping of the sheet (cyan check «LISA», 410 m) frozen at s_FxMs of the appear or the remove animation.
	protected void UpdateFx(INT_PTY_HudCtx ctx)
	{
		INT_PTY_HudPing item = FindBound("fx", -9);
		if (!item)
		{
			item = TakeFree();
			item.m_Owner = "fx";
			item.m_Slot = -9;
		}
		item.m_Seen = true;
		item.SetLook(INT_PTY_Shape.CHECK, 0xFF3FD0FF, "LISA");
		item.Place(ctx.m_Sw * 0.5, ctx.m_Sh * 0.42);
		item.SetDist(410);
		item.SetVisible(true);
		float age = s_FxMs * 0.001;
		float gone = -1;
		if (s_FxMode == 2)
		{
			age = 5.0;
			gone = s_FxMs * 0.001;
		}
		item.m_Rest = false;
		item.Animate(age, gone, 1.0, 0xFFFFFFFF);
		m_Active++;
	}

	// Edit mode: three samples replaying the appear animation every 4 s (mine, a friend's with his name, a far one).
	protected void UpdateSamples(INT_PTY_HudCtx ctx, INT_PTY_HudCore core)
	{
		float opacity = INT_PTY_UI.PartOpacity(INT_PTY_UI.PART_PINGS);
		if (!INT_PTY_UI.IsPartShown(INT_PTY_UI.PART_PINGS))
			opacity = opacity * 0.35;
		int labRgb = INT_PTY_UI.PartAccent(INT_PTY_UI.PART_PINGS);
		float loop = ctx.m_Now / 4.0;
		float age = (loop - Math.Floor(loop)) * 4.0;
		int myShape = INT_PTY_UI.PingShapeOf("", true);
		int myRgb = INT_PTY_UI.PingColorOf("", true);
		SampleAt(ctx, -1, 0.5, 0.56, myShape, myRgb, "", 212, age, opacity, labRgb);
		SampleAt(ctx, -3, 0.64, 0.47, INT_PTY_Shape.CHECK, 0xFF3FD0FF, "LISA", 410, age, opacity, labRgb);
		SampleAt(ctx, -4, 0.36, 0.44, INT_PTY_Shape.CIRCLE, 0xFFFFB02E, "KOLYA", 640, age, opacity, labRgb);
	}

	protected void SampleAt(INT_PTY_HudCtx ctx, int slot, float nx, float ny, int shape, int rgb, string ownerName, float dist, float age, float opacity, int labRgb)
	{
		INT_PTY_HudPing item = FindBound("sample", slot);
		if (!item)
		{
			item = TakeFree();
			item.m_Owner = "sample";
			item.m_Slot = slot;
		}
		item.m_Seen = true;
		item.SetLook(shape, rgb, ownerName);
		item.Place(ctx.m_Sw * nx, ctx.m_Sh * ny);
		item.SetDist(dist);
		item.SetVisible(true);
		item.Animate(age, -1, opacity, labRgb);
		m_Active++;
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
		foreach (INT_PTY_HudPing p : m_Items)
		{
			if (!p.m_Visible)
				continue;
			float rx;
			float ry;
			float rw;
			float rh;
			p.GetRect(rx, ry, rw, rh);
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
