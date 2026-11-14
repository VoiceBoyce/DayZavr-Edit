// ============================================================================
// VanillaPPMap «Экспедиция» B — route HUD (HUD3D, BUILD_PLAN §2 «Route HUD», design 2_b_survey_hud.png).
// Top-centre ribbon (kind 7): «◆ МАРШРУТ» + next point name | track: last passed (✓ NN) — next (◆ NN · metres)
// — leg dots — finish (◇ NN · metres to the end). Toast (kind 8) under it: «✓ ТОЧКА N ПРОЙДЕНА   дальше NN»,
// a green bar runs down for 3 s, then it fades and is unlinked.
// The trees exist only while a route is active, the «Маршрут» layer is on, the player's «Окно маршрута» is on
// (2026-11-14, the «Миникарта» panel; off = no ribbon / toast, the route itself, its 3D beacon and the minimap line
// keep working), no menu (map included; the chat input is fine) is open and the player is alive; otherwise they are
// unlinked (kinds 7, 8 down). Route empty → both
// unlinked at once (also the toast of the last point: the route is over, nothing left to show).
// INT_VPP_Route.INT_Tick(playerPos) every 0.25 s while a route exists, ALSO while the map is open.
// Metres are refreshed with that tick; texts change only when the shown value changes; nothing per frame
// except the toast bar / fade while a toast is up. Driven by the modded MissionGameplay in INT_VPP_3D.c.
// ============================================================================

class INT_VPP_Hud : Managed
{
	static const float INT_TICK = 0.25;
	static const float INT_RIB_W = 620;
	static const float INT_RIB_H = 66;
	static const float INT_RIB_Y = 24;
	static const float INT_LX0 = 210;
	static const float INT_LX1 = 570;
	static const float INT_MY = 22;
	static const int INT_MAX_DOTS = 12;
	static const float INT_TOAST_W = 360;
	static const float INT_TOAST_H = 40;
	static const float INT_TOAST_DX = 130;
	static const float INT_TOAST_GAP = 10;
	static const float INT_TOAST_LIFE = 3.0;
	static const float INT_TOAST_FADE = 0.4;

	static const int INT_COL_BG = 0xFF121110;
	static const int INT_COL_EDGE = 0xFF2A2927;
	static const int INT_COL_MUTED = 0xFF948F88;
	static const int INT_COL_GREEN = 0xFF56E07E;
	static const int INT_COL_LINE_DIM = 0xFF713B1C;    // orange at 0.4 pre-mixed over the plate
	static const int INT_COL_DOT = 0xFF8A4A26;

	static const string INT_TXT_ROUTE = "Маршрут";
	static const string INT_TXT_POINT = "Точка";
	static const string INT_TXT_PASSED = "пройдена";
	static const string INT_TXT_NEXT = "дальше";
	static const string INT_TXT_ME = "Я";
	static const string INT_TXT_SEP = " · ";

	protected static ref INT_VPP_Hud s_INT_Inst;

	protected bool m_INT_Running;
	protected float m_INT_TickAcc;
	protected bool m_INT_Dirty;
	protected int m_INT_ScreenW;
	protected int m_INT_ScreenH;
	// weak: the route we listen to
	protected INT_VPP_Route m_INT_BoundRoute;

	// ribbon (kind 7)
	protected Widget m_INT_Rib;
	protected bool m_INT_RibHas;
	protected float m_INT_RibX;
	protected TextWidget m_INT_NextNum;
	protected TextWidget m_INT_NextRest;
	protected TextWidget m_INT_FinText;
	protected float m_INT_NodeMid;
	protected string m_INT_NextNumStr;
	protected string m_INT_FinNumStr;
	protected int m_INT_KeyNext;
	protected int m_INT_KeyFin;

	// toast (kind 8)
	protected Widget m_INT_Toast;
	protected bool m_INT_ToastHas;
	protected float m_INT_ToastAge;
	protected ImageWidget m_INT_ToastBar;
	protected ref array<Widget> m_INT_ToastParts;
	protected ref array<float> m_INT_ToastAlpha;
	protected bool m_INT_ToastFading;

	void INT_VPP_Hud()
	{
		m_INT_ToastParts = new array<Widget>();
		m_INT_ToastAlpha = new array<float>();
		m_INT_KeyNext = -1;
		m_INT_KeyFin = -1;
	}

	void ~INT_VPP_Hud()
	{
		INT_End();
	}

	// ------------------------------------------------------------------ singleton
	static INT_VPP_Hud INT_Get()
	{
		if (!s_INT_Inst)
			s_INT_Inst = new INT_VPP_Hud();
		return s_INT_Inst;
	}

	static INT_VPP_Hud INT_Peek()
	{
		return s_INT_Inst;
	}

	static void INT_Start()
	{
		INT_VPP_Hud.INT_Shutdown();
		INT_VPP_Hud h = INT_VPP_Hud.INT_Get();
		h.INT_Begin();
	}

	static void INT_Shutdown()
	{
		INT_VPP_Hud h = s_INT_Inst;
		if (h)
			h.INT_End();
		s_INT_Inst = null;
	}

	// ------------------------------------------------------------------ extras (TEST)
	bool INT_HasRibbon()
	{
		if (m_INT_RibHas && m_INT_Rib)
			return true;
		return false;
	}

	bool INT_HasToast()
	{
		if (m_INT_ToastHas && m_INT_Toast)
			return true;
		return false;
	}

	// ------------------------------------------------------------------ per frame
	void INT_Update(float dt)
	{
		if (!m_INT_Running)
			return;
		INT_VPP_Route rt = INT_VPP_Route.INT_Get();
		if (rt != m_INT_BoundRoute)
			INT_Bind(rt);

		bool tick = false;
		m_INT_TickAcc += dt;
		if (m_INT_TickAcc >= INT_TICK)
		{
			m_INT_TickAcc = 0;
			tick = true;
		}
		// the route advances also while the map is open
		if (tick && rt.INT_IsActive())
		{
			Man player = GetGame().GetPlayer();
			if (player && player.IsAlive())
			{
				vector pos = player.GetPosition();
				rt.INT_Tick(pos);
			}
		}

		if (!INT_WantHud(rt))
		{
			INT_DropRibbon();
			INT_DropToast();
			return;
		}

		if (tick)
		{
			int sw;
			int sh;
			GetScreenSize(sw, sh);
			if (sw != m_INT_ScreenW || sh != m_INT_ScreenH)
			{
				m_INT_ScreenW = sw;
				m_INT_ScreenH = sh;
				m_INT_Dirty = true;
				INT_DropToast();
			}
		}

		bool built = false;
		if (m_INT_Dirty || !INT_HasRibbon())
		{
			m_INT_Dirty = false;
			INT_BuildRibbon(rt);
			built = true;
		}
		if (tick || built)
			INT_RefreshMetres(rt);
		if (m_INT_ToastHas)
			INT_ToastStep(dt);
	}

	// ------------------------------------------------------------------ internals
	protected void INT_Begin()
	{
		m_INT_Running = true;
		m_INT_Dirty = true;
		m_INT_TickAcc = 0;
		int sw;
		int sh;
		GetScreenSize(sw, sh);
		m_INT_ScreenW = sw;
		m_INT_ScreenH = sh;
		INT_VPP_Route rt = INT_VPP_Route.INT_Get();
		INT_Bind(rt);
	}

	protected void INT_End()
	{
		INT_DropRibbon();
		INT_DropToast();
		INT_Unbind();
		m_INT_Running = false;
	}

	protected void INT_Bind(INT_VPP_Route rt)
	{
		INT_Unbind();
		if (!rt)
			return;
		if (rt.m_INT_OnChanged)
			rt.m_INT_OnChanged.Insert(this.INT_OnRouteChanged);
		if (rt.m_INT_OnReached)
			rt.m_INT_OnReached.Insert(this.INT_OnRouteReached);
		m_INT_BoundRoute = rt;
	}

	protected void INT_Unbind()
	{
		INT_VPP_Route old = m_INT_BoundRoute;
		if (old)
		{
			if (old.m_INT_OnChanged)
				old.m_INT_OnChanged.Remove(this.INT_OnRouteChanged);
			if (old.m_INT_OnReached)
				old.m_INT_OnReached.Remove(this.INT_OnRouteReached);
		}
		m_INT_BoundRoute = null;
	}

	protected bool INT_WantHud(INT_VPP_Route rt)
	{
		if (!rt || !rt.INT_IsActive())
			return false;
		INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
		if (!pr.INT_IsLayerOn(INT_VPP_LAYER_ROUTE))
			return false;
		// «Окно маршрута» off (2026-11-14)
		if (!pr.INT_IsRouteHudOn())
			return false;
		if (INT_VPP_3D.INT_IsScreenBlocked())
			return false;
		return true;
	}

	void INT_OnRouteChanged()
	{
		m_INT_Dirty = true;
		INT_VPP_Route rt = INT_VPP_Route.INT_Peek();
		bool active = false;
		if (rt)
			active = rt.INT_IsActive();
		if (!active)
		{
			INT_DropRibbon();
			INT_DropToast();
		}
	}

	void INT_OnRouteReached(int number1Based, string name)
	{
		if (!m_INT_Running)
			return;
		INT_VPP_Route rt = INT_VPP_Route.INT_Peek();
		// the last point: the route is already cleared, the HUD goes (no toast) - the whole task is done
		if (!rt || !rt.INT_IsActive())
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DONE);
			return;
		}
		if (!INT_WantHud(rt))
			return;
		int nextIdx = rt.INT_NextIndex();
		int nextNum = rt.INT_NumberOf(nextIdx);
		INT_ShowToast(number1Based, nextNum);
		// several points in one tick: one sound (NOTIFY has its own 0.25 s gap)
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.NOTIFY);
	}

	// canvas width in design px (the canvas is 1080 tall; wider screens are wider canvases)
	protected float INT_CanvasW()
	{
		float f = INT_UIF_Scale.Factor();
		int sw = INT_UIF_Scale.ScreenW();
		if (f <= 0 || sw <= 0)
			return 1920;
		return sw / f;
	}

	protected void INT_BuildRibbon(INT_VPP_Route rt)
	{
		INT_DropRibbon();
		if (!GetGame() || !GetGame().GetWorkspace())
			return;
		int n = rt.INT_Count();
		int next = rt.INT_NextIndex();
		if (n <= 0 || next < 0)
			return;
		float x0 = (INT_CanvasW() - INT_RIB_W) * 0.5;
		Widget root = INT_UIF_Draw.Frame(null, x0, INT_RIB_Y, INT_RIB_W, INT_RIB_H, "INT_VPP_HudRibbon");
		if (!root)
			return;
		m_INT_Rib = root;
		m_INT_RibHas = true;
		m_INT_RibX = x0;
		INT_VPP_Stat.INT_Up(INT_VPP_Stat.INT_KIND_HUD);

		INT_VPPX_Look.Fill(root, 0, 0, INT_RIB_W, INT_RIB_H, INT_COL_BG);
		INT_VPPX_Look.Fill(root, 0, INT_RIB_H - 1, INT_RIB_W, 1, INT_COL_EDGE);
		INT_VPPX_Look.Diamond(root, 22, 21, 8, true, INT_VPPX_Look.ORANGE);
		INT_VPPX_Look.Caps(root, 34, 12, 170, 18, INT_TXT_ROUTE, 12, INT_VPPX_Look.ORANGE);

		INT_VPP_RoutePoint np = rt.INT_At(next);
		int nextNum = rt.INT_NumberOf(next);
		string nextPad = INT_VPPX_Text.Pad2(nextNum);
		string nextName = "";
		if (np)
			nextName = np.m_INT_Name;
		if (nextName == "")
			nextName = INT_TXT_POINT + " " + nextPad;
		string shownName = INT_VPPX_Look.FitText(nextName, 13, false, 176);
		INT_VPPX_Look.Text(root, 18, 34, 186, 20, shownName, 13, INT_COL_MUTED);

		int remaining = n - next;
		float mid = INT_LX1;
		float lineEnd = INT_LX1;
		if (remaining >= 2)
			mid = (INT_LX0 + INT_LX1) * 0.5;
		INT_VPPX_Look.Fill(root, INT_LX0, INT_MY, lineEnd - INT_LX0, 1, INT_COL_LINE_DIM);
		INT_VPPX_Look.Fill(root, INT_LX0, INT_MY - 1, mid - INT_LX0, 3, INT_VPPX_Look.ORANGE);

		// left node: the last passed point (✓ NN), or «Я» before the first one
		int passed = rt.INT_PassedCount();
		if (passed > 0)
		{
			int lastNum = nextNum - 1;
			string lastPad = INT_VPPX_Text.Pad2(lastNum);
			INT_VPPX_Look.Fill(root, INT_LX0 - 9, INT_MY - 9, 18, 18, INT_COL_BG);
			INT_VPPX_Look.IconAt(root, INT_LX0 - 7, INT_MY - 7, 14, "check", INT_COL_GREEN);
			INT_VPPX_Look.Text(root, INT_LX0 - 60, INT_MY + 12, 120, 18, lastPad, 13, INT_COL_GREEN, INT_UIF_Align.CENTER);
		}
		else
		{
			INT_VPPX_Look.Diamond(root, INT_LX0, INT_MY, 10, true, INT_COL_DOT);
			INT_VPPX_Look.Text(root, INT_LX0 - 60, INT_MY + 12, 120, 18, INT_TXT_ME, 13, INT_COL_MUTED, INT_UIF_Align.CENTER);
		}

		// leg dots: the points between the next one and the finish
		int between = remaining - 2;
		if (between > 0)
		{
			int dots = between;
			if (dots > INT_MAX_DOTS)
				dots = INT_MAX_DOTS;
			float step = (INT_LX1 - mid) / (dots + 1);
			for (int d = 1; d <= dots; d++)
			{
				float dx = mid + step * d;
				INT_VPPX_Look.Diamond(root, dx, INT_MY, 6, true, INT_COL_DOT);
			}
		}

		// finish node: ◇ NN · metres to the end
		m_INT_FinText = null;
		m_INT_FinNumStr = "";
		if (remaining >= 2)
		{
			int finNum = rt.INT_NumberOf(n - 1);
			m_INT_FinNumStr = INT_VPPX_Text.Pad2(finNum);
			INT_VPPX_Look.Diamond(root, INT_LX1, INT_MY, 14, true, INT_COL_BG);
			INT_VPPX_Look.Diamond(root, INT_LX1, INT_MY, 13, false, INT_VPPX_Look.ORANGE);
			TextWidget fin = INT_VPPX_Look.Text(root, INT_LX1 - 70, INT_MY + 12, 140, 18, m_INT_FinNumStr, 13, INT_COL_MUTED, INT_UIF_Align.CENTER);
			m_INT_FinText = fin;
		}

		// next node: ◆ NN · metres (number orange, the rest white; laid out when the metres change)
		INT_VPPX_Look.Diamond(root, mid, INT_MY, 14, true, INT_VPPX_Look.ORANGE);
		m_INT_NodeMid = mid;
		m_INT_NextNumStr = nextPad;
		TextWidget numText = INT_VPPX_Look.Text(root, mid - 40, INT_MY + 10, 40, 22, nextPad, 17, INT_VPPX_Look.ORANGE);
		m_INT_NextNum = numText;
		TextWidget restText = INT_VPPX_Look.Text(root, mid, INT_MY + 10, 150, 22, INT_TXT_SEP, 17, INT_VPPX_Look.WHITE);
		m_INT_NextRest = restText;
		m_INT_KeyNext = -1;
		m_INT_KeyFin = -1;
	}

	protected void INT_RefreshMetres(INT_VPP_Route rt)
	{
		if (!INT_HasRibbon())
			return;
		Man player = GetGame().GetPlayer();
		if (!player)
			return;
		vector ppos = player.GetPosition();
		float dNext = rt.INT_DistToNext(ppos);
		int kN = INT_VPP_Metres.INT_MetresKey(dNext);
		if (kN != m_INT_KeyNext)
		{
			m_INT_KeyNext = kN;
			string metres = INT_VPP_Metres.INT_MetresText(kN);
			string rest = INT_TXT_SEP + metres;
			float wNum = INT_VPPX_Look.Measure(m_INT_NextNumStr, 17);
			float wRest = INT_VPPX_Look.Measure(rest, 17);
			float total = wNum + wRest;
			float start = m_INT_NodeMid - total * 0.5;
			float maxStart = INT_RIB_W - 10 - total;
			if (start > maxStart)
				start = maxStart;
			if (m_INT_NextNum)
				INT_UIF_Draw.Place(m_INT_NextNum, start, INT_MY + 10, wNum + 16, 22);
			if (m_INT_NextRest)
			{
				INT_UIF_Draw.Place(m_INT_NextRest, start + wNum, INT_MY + 10, wRest + 24, 22);
				m_INT_NextRest.SetText(rest);
			}
		}
		if (m_INT_FinText)
		{
			float dFin = rt.INT_RemainingFrom(ppos);
			int kF = INT_VPP_Metres.INT_MetresKey(dFin);
			if (kF != m_INT_KeyFin)
			{
				m_INT_KeyFin = kF;
				string finMetres = INT_VPP_Metres.INT_MetresText(kF);
				string finLabel = m_INT_FinNumStr + INT_TXT_SEP + finMetres;
				m_INT_FinText.SetText(finLabel);
			}
		}
	}

	protected void INT_DropRibbon()
	{
		if (!m_INT_RibHas)
			return;
		Widget root = m_INT_Rib;
		if (root)
			root.Unlink();
		m_INT_Rib = null;
		m_INT_NextNum = null;
		m_INT_NextRest = null;
		m_INT_FinText = null;
		m_INT_RibHas = false;
		m_INT_KeyNext = -1;
		m_INT_KeyFin = -1;
		INT_VPP_Stat.INT_Down(INT_VPP_Stat.INT_KIND_HUD);
	}

	// ------------------------------------------------------------------ toast
	protected void INT_ShowToast(int number, int nextNumber)
	{
		INT_DropToast();
		if (!GetGame() || !GetGame().GetWorkspace())
			return;
		float x = (INT_CanvasW() - INT_RIB_W) * 0.5 + INT_TOAST_DX;
		float y = INT_RIB_Y + INT_RIB_H + INT_TOAST_GAP;
		Widget root = INT_UIF_Draw.Frame(null, x, y, INT_TOAST_W, INT_TOAST_H, "INT_VPP_HudToast");
		if (!root)
			return;
		m_INT_Toast = root;
		m_INT_ToastHas = true;
		m_INT_ToastAge = 0;
		m_INT_ToastFading = false;
		INT_VPP_Stat.INT_Up(INT_VPP_Stat.INT_KIND_TOAST);

		INT_VPPX_Look.Fill(root, 0, 0, INT_TOAST_W, INT_TOAST_H, INT_COL_BG);
		INT_VPPX_Look.IconAt(root, 16, 12, 16, "check", INT_COL_GREEN);
		string title = INT_TXT_POINT + " " + number.ToString() + " " + INT_TXT_PASSED;
		INT_VPPX_Look.Caps(root, 44, 10, 230, 20, title, 14, INT_VPPX_Look.WHITE);
		string nextPad = INT_VPPX_Text.Pad2(nextNumber);
		string nextLabel = INT_TXT_NEXT + " " + nextPad;
		INT_VPPX_Look.Text(root, 250, 10, 94, 20, nextLabel, 13, INT_COL_MUTED, INT_UIF_Align.RIGHT);
		ImageWidget bar = INT_VPPX_Look.Fill(root, 0, INT_TOAST_H - 2, INT_TOAST_W, 2, INT_COL_GREEN);
		m_INT_ToastBar = bar;

		m_INT_ToastParts.Clear();
		m_INT_ToastAlpha.Clear();
		INT_VPPX_Look.CollectTree(root, m_INT_ToastParts);
		int cnt = m_INT_ToastParts.Count();
		for (int i = 0; i < cnt; i++)
		{
			Widget w = m_INT_ToastParts.Get(i);
			float a = 1.0;
			if (w)
				a = w.GetAlpha();
			m_INT_ToastAlpha.Insert(a);
		}
	}

	protected void INT_ToastStep(float dt)
	{
		if (!m_INT_ToastHas)
			return;
		if (!m_INT_Toast)
		{
			INT_DropToast();
			return;
		}
		m_INT_ToastAge += dt;
		float age = m_INT_ToastAge;
		if (age < INT_TOAST_LIFE)
		{
			float left = 1.0 - age / INT_TOAST_LIFE;
			if (m_INT_ToastBar)
				INT_UIF_Draw.Resize(m_INT_ToastBar, INT_TOAST_W * left, 2);
			return;
		}
		float fade = age - INT_TOAST_LIFE;
		if (fade >= INT_TOAST_FADE)
		{
			INT_DropToast();
			return;
		}
		if (!m_INT_ToastFading)
		{
			m_INT_ToastFading = true;
			if (m_INT_ToastBar)
				m_INT_ToastBar.Show(false);
		}
		float k = 1.0 - fade / INT_TOAST_FADE;
		int cnt = m_INT_ToastParts.Count();
		for (int i = 0; i < cnt; i++)
		{
			Widget w = m_INT_ToastParts.Get(i);
			if (!w)
				continue;
			float startAlpha = m_INT_ToastAlpha.Get(i);
			w.SetAlpha(startAlpha * k);
		}
	}

	protected void INT_DropToast()
	{
		if (!m_INT_ToastHas)
			return;
		Widget root = m_INT_Toast;
		if (root)
			root.Unlink();
		m_INT_Toast = null;
		m_INT_ToastBar = null;
		m_INT_ToastHas = false;
		m_INT_ToastFading = false;
		m_INT_ToastParts.Clear();
		m_INT_ToastAlpha.Clear();
		INT_VPP_Stat.INT_Down(INT_VPP_Stat.INT_KIND_TOAST);
	}
}
