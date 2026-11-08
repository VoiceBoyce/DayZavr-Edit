// INT PartyMode (client) — the HUD (concept C «ЭКСПЕДИЦИЯ» + pings P5 «ВСПЫШКА», spec.md §8, design/BUILD_BRIEF.md):
// name tags (INT_PTY_HudTags), the «ГРУППА» tracker (INT_PTY_HudTracker), ping markers (INT_PTY_HudPings), edge arrows
// (INT_PTY_HudArrows), notification cards (INT_PTY_HudToasts) and the tracker mover (INT_PTY_MoverMenu).
// One top-level frame at sort 0 (a top-level root at 100+ kills 3D previews), built once per resolution; every part is
// a pool built once and only moved / recoloured per frame. Hidden: under any menu (inventory, map, pause, our window…),
// when the player hid the vanilla HUD (HUD_HIDE hotkey), while dead / unconscious / without a body (respawn), or when
// another mod called INT_PTY_UI.SetVisible(false). The mover menu shows the tracker only; the API edit mode shows every
// part with samples, also over menus. Entry points (static) are called from INT_PTY_Mission and the window.

class INT_PTY_Hud
{
	protected static ref INT_PTY_HudCore s_Core;

	static INT_PTY_HudCore Core()
	{
		if (!s_Core && GetGame() && !GetGame().IsDedicatedServer())
		{
			s_Core = new INT_PTY_HudCore();
			s_Core.Init();
		}
		return s_Core;
	}

	static void Init()
	{
		Core();
	}

	static void Tick(float dt)
	{
		if (s_Core)
			s_Core.Tick(dt);
	}

	static void Shutdown()
	{
		INT_PTY_MoverMenu.ResetState();
		if (s_Core)
			s_Core.Destroy();
		s_Core = null;
		// H-6: the API edit mode / external hide end with the session; W-9: «seen / died N min ago» and the
		// search-list request time too (the next session starts clean)
		INT_PTY_UI.ResetSession();
		INT_PTY_MenuWatch.Reset();
	}

	// «Двигать в игре» in the settings tab: the in-game tracker mover (opened on the next frame, never inside the
	// window's own click; the window comes back when the mover is done).
	static void OpenMover()
	{
		INT_PTY_MoverMenu.Request(INT_PTY_Menu.IsOpen());
	}

	// «Сбросить»: the tracker back to its default place (left middle).
	static void ResetTrackerPos()
	{
		INT_PTY_UI.ResetPartPosition(INT_PTY_UI.PART_TRACKER);
	}

	// Settings changed: sizes, styles, colours and names are re-read on the next frame.
	static void Refresh()
	{
		if (s_Core)
			s_Core.MarkRefresh();
	}

	// A notification card on the HUD (kind = INT_UIF_ToastKind.*), shown while the window is closed.
	static void ShowToast(int kind, string title, string text)
	{
		INT_PTY_HudCore c = Core();
		if (c)
			c.Toasts().Push(kind, title, text);
	}

	static void OnClientChanged(int what)
	{
		if (s_Core)
			s_Core.OnClientChanged(what);
	}

	// --- client self-test / stand hooks (INT_PTY_TestKit) ------------------------------------------------------
	//   state | cost | toast <text> | ping [m] | edit on|off | hold on|off | mover | mover done | drag <nx> <ny> | reset
	static string TestCommand(string cmd)
	{
		INT_PTY_HudCore c = Core();
		if (!c)
			return "HUD-TEST no hud";
		array<string> w = new array<string>();
		cmd.Split(" ", w);
		string head = "";
		if (w.Count() > 0)
			head = w.Get(0);
		string arg = "";
		if (w.Count() > 1)
			arg = w.Get(1);
		if (head == "toast")
		{
			string text = cmd.Substring(5, cmd.Length() - 5);
			text.TrimInPlace();
			if (text == "")
				text = "Тестовое уведомление";
			c.Toasts().Push(INT_UIF_ToastKind.INFO, "HUD TEST", text);
		}
		else if (head == "ping")
		{
			float m = 60;
			if (arg != "")
				m = arg.ToFloat();
			c.PreviewPing(m);
		}
		else if (head == "edit")
		{
			bool editOn = arg == "on";
			INT_PTY_UI.SetEditMode(editOn);
		}
		else if (head == "hold")
		{
			INT_PTY_HudToasts.s_Hold = arg == "on";
		}
		else if (head == "aim")
		{
			return "HUD-TEST aim " + c.Pings().AimReport();
		}
		else if (head == "aimping")
		{
			// stand: "aimping <dx design px> [m]" — my ping that far right of the crosshair (aim dimming shots)
			float adx = arg.ToFloat();
			float am = 40;
			if (w.Count() > 2)
			{
				string amv = w.Get(2);
				am = amv.ToFloat();
			}
			c.PreviewPingAt(am, adx);
		}
		else if (head == "map")
		{
			// stand: the vanilla map menu (map layer pins) — "map" opens, "map close" closes
			UIManager um = GetGame().GetUIManager();
			UIScriptedMenu open = um.GetMenu();
			if (arg == "close")
			{
				if (open)
					open.Close();
			}
			else if (arg == "center")
			{
				// "map center [scale]": the open map on the player (the pins around him)
				Widget mroot = null;
				if (open)
					mroot = open.GetLayoutRoot();
				MapWidget mw = null;
				if (mroot)
					mw = MapWidget.Cast(mroot.FindAnyWidget("Map"));
				PlayerBase mp = PlayerBase.Cast(GetGame().GetPlayer());
				if (mw && mp)
				{
					float ms = 0.05;
					if (w.Count() > 2)
					{
						string msv = w.Get(2);
						ms = msv.ToFloat();
					}
					mw.SetScale(ms);
					mw.SetMapPos(mp.GetPosition());
				}
			}
			else if (!um.GetMenu())
			{
				um.EnterScriptedMenu(MENU_MAP, null);
			}
		}
		else if (head == "mover")
		{
			if (arg == "done")
				INT_PTY_MoverMenu.TestFinish();
			else
				OpenMover();
		}
		else if (head == "drag" && w.Count() > 2)
		{
			float dnx = arg.ToFloat();
			string a2 = w.Get(2);
			float dny = a2.ToFloat();
			bool ok = INT_PTY_MoverMenu.TestDrag(dnx, dny);
			if (!ok)
				return Log("HUD-TEST drag failed (mover closed?)");
		}
		else if (head == "reset")
		{
			ResetTrackerPos();
		}
		else if (head == "set" && w.Count() > 2)
		{
			// a setting of the «Настройки» tab, like the window does it: field, MarkDirty, Refresh
			string val = w.Get(2);
			INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
			if (val.IndexOf(".") >= 0)
			{
				float fv = val.ToFloat();
				EnScript.SetClassVar(st, arg, 0, fv);
			}
			else
			{
				int iv = val.ToInt();
				EnScript.SetClassVar(st, arg, 0, iv);
			}
			INT_PTY_SettingsStore.MarkDirty();
			Refresh();
		}
		else if (head == "tune" && w.Count() > 2)
		{
			// stand tuning of the bright P5 layers: tune halo|flash|sweep|flare|white|glow|labpx|namepx|minpx <k> (px: then set PingSize to rebuild)
			string kv = w.Get(2);
			float k = kv.ToFloat();
			if (arg == "halo")
				INT_PTY_HudPing.s_HaloK = k;
			else if (arg == "flash")
				INT_PTY_HudPing.s_FlashK = k;
			else if (arg == "sweep")
				INT_PTY_HudPing.s_SweepK = k;
			else if (arg == "flare")
				INT_PTY_HudPing.s_FlareK = k;
			else if (arg == "white")
				INT_PTY_HudPing.s_FlareWhite = k;
			else if (arg == "glow")
				INT_PTY_HudPing.s_GlowK = k;
			else if (arg == "labpx")
				INT_PTY_HudPings.s_LabPx = k;
			else if (arg == "namepx")
				INT_PTY_HudPings.s_NamePx = k;
			else if (arg == "minpx")
				INT_PTY_HudPings.s_MinPx = k;
		}
		else if (head == "notify")
		{
			// the real notification path (INT_PTY_Notify -> card + sound): notify online|dead|invite [name]
			string who = "Витя";
			if (w.Count() > 2)
				who = w.Get(2);
			if (arg == "online")
				INT_PTY_Notify.Online(who);
			else if (arg == "dead")
				INT_PTY_Notify.Dead(who);
			else if (arg == "invite")
				INT_PTY_Notify.InviteIn(who);
		}
		else if (head == "fx")
		{
			// frozen P5 frame for the sheet comparison: fx appear <ms> | fx remove <ms> | fx off
			int fxMode = 0;
			if (arg == "appear")
				fxMode = 1;
			else if (arg == "remove")
				fxMode = 2;
			float fxMs = 0;
			if (w.Count() > 2)
			{
				string a3 = w.Get(2);
				fxMs = a3.ToFloat();
			}
			INT_PTY_HudPings.s_FxMode = fxMode;
			INT_PTY_HudPings.s_FxMs = fxMs;
		}
		return Log("HUD-TEST " + head + " -> " + c.StateLine());
	}

	protected static string Log(string line)
	{
		INT_PTY_Log.Raw(line);
		return line;
	}
}

// The API bridge: what INT_PTY_UI (3_Game) calls into the HUD.
class INT_PTY_HudBridge : INT_PTY_HudApi
{
	INT_PTY_HudCore m_Core;

	override void ApplyPart(string part)
	{
		if (m_Core)
			m_Core.MarkRefresh();
	}

	override void PlacePart(string part)
	{
		// places are read every frame
	}

	override void SetEditMode(bool on)
	{
		if (m_Core)
			m_Core.MarkRefresh();
	}

	override void RefreshVisibility()
	{
	}

	override void Preview(string part)
	{
		if (!m_Core)
			return;
		if (part == INT_PTY_UI.PART_TOASTS)
			m_Core.Toasts().Preview();
		else if (part == INT_PTY_UI.PART_PINGS)
			m_Core.PreviewPing(60);
		else if (part == INT_PTY_UI.PART_TRACKER)
			m_Core.PreviewTracker();
	}

	override Widget FindPart(string part)
	{
		if (!m_Core)
			return null;
		return m_Core.FindPart(part);
	}

	override bool GetPartRect(string part, out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		if (!m_Core)
			return false;
		return m_Core.GetPartRect(part, x, y, w, h);
	}

	override bool IsPartActive(string part)
	{
		if (!m_Core)
			return false;
		return m_Core.IsPartActive(part);
	}
}

class INT_PTY_HudCore : Managed
{
	static const float MATES_EVERY = 10.0;		// safety rebuild of the shown-friends cache (s); H-5: changes mark it dirty
	static const float COST_EVERY = 10.0;		// debug cost line period (s)
	static const float WARM_S = 2.0;			// prewarm images drawn (invisibly) this long, then hidden
	static const float ALIVE_EVERY = 2.0;		// H-8: at most one rebuild of an unlinked part per this many seconds

	protected Widget m_Root;
	protected Widget m_LayerTags;
	protected Widget m_LayerPings;
	protected Widget m_LayerArrows;
	protected Widget m_LayerTracker;
	protected Widget m_LayerToasts;
	protected int m_Sw;
	protected int m_Sh;
	protected bool m_Shown;
	protected bool m_Refresh = true;
	protected bool m_MatesDirty = true;
	protected float m_MatesAt = -100;
	protected float m_PreviewTrackerUntil = -100;
	protected bool m_Hooked;
	protected Widget m_Warm;					// QA prewarm: one tiny image per HUD texture, kept for the session
	protected bool m_Warmed;
	protected float m_WarmLeft;
	protected float m_AliveAt = -100;
	protected ref array<string> m_MateDrop;		// H-5: mates no longer shown (reused list, no per-rebuild new)

	protected ref INT_PTY_HudCtx m_Ctx;
	protected ref INT_PTY_HudTags m_Tags;
	protected ref INT_PTY_HudPings m_Pings;
	protected ref INT_PTY_HudArrows m_Arrows;
	protected ref INT_PTY_HudTracker m_Tracker;
	protected ref INT_PTY_HudToasts m_Toasts;
	protected ref INT_PTY_HudBridge m_Bridge;
	protected ref map<string, ref INT_PTY_HudMate> m_Mates;

	// built parameters (a change rebuilds that part)
	protected float m_KTags = -1;
	protected int m_TagStyle = -1;
	protected float m_KPings = -1;
	protected float m_KTracker = -1;
	protected int m_TrkStyle = -1;
	protected float m_KToasts = -1;
	protected float m_KArrows = -1;

	// frame cost (TickCount calibrated against the game clock)
	protected float m_TicksPerUs = -1;
	protected int m_CalTick;
	protected float m_CalTime = -1;
	protected float m_CalTicks;
	protected float m_CalSec;
	protected float m_CostSum;
	protected ref array<float> m_PartTicks;		// tags, pings, arrows, tracker, toasts (ticks since the last report)
	protected string m_PartText = "";
	protected float m_CostMax;
	protected int m_CostFrames;
	protected float m_CostAt;
	protected float m_LastAvgUs = -1;
	protected float m_LastMaxUs = -1;

	void Init()
	{
		m_Ctx = new INT_PTY_HudCtx();
		m_Tags = new INT_PTY_HudTags();
		m_Pings = new INT_PTY_HudPings();
		m_Arrows = new INT_PTY_HudArrows();
		m_Tracker = new INT_PTY_HudTracker();
		m_Toasts = new INT_PTY_HudToasts();
		m_Mates = new map<string, ref INT_PTY_HudMate>();
		m_MateDrop = new array<string>();
		m_PartTicks = new array<float>();
		for (int pi = 0; pi < 5; pi++)
		{
			m_PartTicks.Insert(0);
		}
		m_Bridge = new INT_PTY_HudBridge();
		m_Bridge.m_Core = this;
		INT_PTY_UI.RegisterHud(m_Bridge);
		ScriptInvoker inv = INT_PTY_Client.OnChanged();
		if (inv && inv.Count(INT_PTY_Hud.OnClientChanged) == 0)
			inv.Insert(INT_PTY_Hud.OnClientChanged);
		m_Hooked = true;
		INT_PTY_HudKit.CacheTexts();
		if (GetGame().GetWorkspace())
			Prewarm();
	}

	void Destroy()
	{
		if (m_Hooked)
		{
			ScriptInvoker inv = INT_PTY_Client.OnChanged();
			if (inv)
				inv.Remove(INT_PTY_Hud.OnClientChanged);
		}
		m_Hooked = false;
		INT_PTY_UI.RegisterHud(null);
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
		if (m_Warm)
			m_Warm.Unlink();
		m_Warm = null;
	}

	// QA 2026-10-23: the first tracker row / friend ping of a session froze the HUD 0.4–1.4 s — their pools are created
	// on first use and every texture was loaded then. Here one 1 px image per texture the HUD uses (tracker, tags,
	// halos, all ping shapes + outlines, P5 fx, edge arrows, status / card icons) is created once at mission start,
	// drawn invisibly (alpha 0.01, top-left pixel) for WARM_S and then hidden; it stays alive so the textures stay
	// loaded for the session. Runs once per HUD core (the list below is built once, not per frame).
	protected void Prewarm()
	{
		if (m_Warmed)
			return;
		m_Warmed = true;
		m_WarmLeft = WARM_S;
		INT_UIF_Assets.EnsureLoaded();
		m_Warm = INT_UIF_Draw.Frame(null, 0, 0, 1, 1, "INT_PTY_HudWarm");
		if (!m_Warm)
			return;
		m_Warm.SetPos(0, 0);
		m_Warm.SetSize(1, 1);
		array<string> names = new array<string>();
		names.Insert("pty_px_ca");
		names.Insert("pty_diamond_ca");
		names.Insert("pty_diamond_line_ca");
		names.Insert("pty_glow_ca");
		names.Insert("pty_grad_h_ca");
		names.Insert("pty_halo_dark_ca");
		names.Insert("pty_edge_arrow_ca");
		names.Insert("pty_edge_arrow_out_ca");
		names.Insert("pty_fx_flare_ca");
		names.Insert("pty_fx_flash_ca");
		names.Insert("pty_fx_sweep_ca");
		names.Insert("pty_fx_halo_ca");
		names.Insert("pty_fx_ring_ca");
		names.Insert("pty_icon_arrow_up_ca");
		names.Insert("pty_icon_skull_ca");
		names.Insert("pty_icon_unconscious_ca");
		names.Insert("pty_icon_bleeding_ca");
		names.Insert("pty_icon_legs_ca");
		names.Insert("pty_icon_bell_ca");
		names.Insert("pty_icon_warning_ca");
		names.Insert("pty_icon_user_ca");
		names.Insert("pty_icon_inbox_ca");
		names.Insert("pty_icon_user_plus_ca");
		names.Insert("pty_icon_user_minus_ca");
		names.Insert("pty_icon_send_ca");
		foreach (string tn : names)
		{
			ImageWidget wi = INT_PTY_HudKit.Img(m_Warm, tn, 0xFFFFFFFF);
			WarmImage(wi);
		}
		for (int s = 0; s < INT_PTY_Shape.COUNT; s++)
		{
			string shapeTex = INT_PTY_UI.PingTex(s, false);
			string shapeOut = INT_PTY_UI.PingTex(s, true);
			ImageWidget ws = INT_PTY_HudKit.ImgPath(m_Warm, shapeTex, 0xFFFFFFFF);
			WarmImage(ws);
			ImageWidget wo = INT_PTY_HudKit.ImgPath(m_Warm, shapeOut, 0xFFFFFFFF);
			WarmImage(wo);
		}
		ImageWidget wc = INT_UIF_Draw.Image(m_Warm, 0, 0, 1, 1, "iuf_circle16", 0xFFFFFFFF);
		WarmImage(wc);
		m_Warm.Show(true);
	}

	protected void WarmImage(ImageWidget img)
	{
		if (!img)
			return;
		img.SetPos(0, 0);
		img.SetSize(1, 1);
		img.SetAlpha(0.01);
	}

	protected void TickWarm(float dt)
	{
		if (!m_Warmed && GetGame().GetWorkspace())
			Prewarm();
		if (m_WarmLeft <= 0)
			return;
		m_WarmLeft = m_WarmLeft - dt;
		if (m_WarmLeft <= 0 && m_Warm)
			m_Warm.Show(false);
	}

	// H-8: another mod unlinked a part frame it got from INT_PTY_UI.FindPart (or the whole HUD root): rebuild the HUD
	// layer, or the tracker panel, instead of drawing into dead widgets. At most once per ALIVE_EVERY seconds.
	protected bool CheckAlive(float now)
	{
		bool layersOk = true;
		if (!m_LayerTags || !m_LayerPings || !m_LayerArrows || !m_LayerTracker || !m_LayerToasts)
			layersOk = false;
		bool trackerOk = true;
		if (m_KTracker >= 0 && !m_Tracker.GetPanel())
			trackerOk = false;
		if (layersOk && trackerOk)
			return true;
		if (now - m_AliveAt < ALIVE_EVERY)
			return false;
		m_AliveAt = now;
		INT_PTY_Log.Warn("HUD группы: часть HUD удалена другим модом — пересобираю");
		if (!layersOk)
		{
			if (m_Root)
				m_Root.Unlink();
			m_Root = null;
			EnsureRoot();
			return m_Root != null;
		}
		m_KTracker = -1;
		m_Refresh = true;
		return true;
	}

	INT_PTY_HudToasts Toasts()
	{
		return m_Toasts;
	}

	INT_PTY_HudPings Pings()
	{
		return m_Pings;
	}

	INT_PTY_HudArrows Arrows()
	{
		return m_Arrows;
	}

	void MarkRefresh()
	{
		m_Refresh = true;
		m_MatesDirty = true;
	}

	void OnClientChanged(int what)
	{
		if ((what & INT_PTY_What.FRIENDS) != 0)
			m_MatesDirty = true;
	}

	INT_PTY_HudMate Mate(string id)
	{
		INT_PTY_HudMate m;
		if (m_Mates.Find(id, m))
			return m;
		return null;
	}

	int MateCount()
	{
		return m_Mates.Count();
	}

	// --- frame ----------------------------------------------------------------------------------------------------
	void Tick(float dt)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		int t0 = TickCount(0);
		TickWarm(dt);
		INT_PTY_MoverMenu.Poll();
		bool edit = INT_PTY_UI.IsEditMode();
		bool mover = INT_PTY_MoverMenu.IsOpen();
		Man man = GetGame().GetPlayer();
		PlayerBase pb = PlayerBase.Cast(man);
		bool playerOk = false;
		if (pb && pb.IsAlive())
		{
			if (!pb.IsUnconscious())
				playerOk = true;
		}
		bool menu = false;
		UIManager um = GetGame().GetUIManager();
		if (um && um.GetMenu())
			menu = true;
		bool show = edit || mover;
		if (!show && !menu && playerOk)
		{
			if (!VanillaHudHidden())
				show = true;
		}
		if (!INT_PTY_UI.IsVisible())
			show = false;
		if (!show)
		{
			SetShown(false);
			return;
		}
		if (!GetGame().GetWorkspace())
			return;
		EnsureRoot();
		if (!m_Root)
			return;
		float now = INT_PTY_Client.Clock();
		if (!CheckAlive(now))
			return;
		if (m_Refresh)
			ApplySettings();
		if (m_MatesDirty || now - m_MatesAt > MATES_EVERY)
			RebuildMates(now);
		FillCtx(dt, now, man, edit);
		SetShown(true);
		bool full = edit || !mover;
		ShowLayers(full);
		m_Tracker.SetMoving(mover);
		if (full)
		{
			m_Arrows.Begin();
			int tp = TickCount(0);
			m_Tags.Update(m_Ctx, this);
			PartCost(0, tp);
			tp = TickCount(0);
			m_Pings.Update(m_Ctx, this);
			PartCost(1, tp);
			tp = TickCount(0);
			m_Arrows.ClearAvoid();
			float ax;
			float ay;
			float aw;
			float ah;
			if (m_Tracker.GetRect(m_Sw, m_Sh, ax, ay, aw, ah))
				m_Arrows.AddAvoid(ax, ay, aw, ah);
			if (m_Toasts.GetRect(m_Sw, m_Sh, ax, ay, aw, ah))
				m_Arrows.AddAvoid(ax, ay, aw, ah);
			m_Arrows.Draw(m_Ctx);
			PartCost(2, tp);
		}
		int tt = TickCount(0);
		m_Tracker.Update(m_Ctx, this, now < m_PreviewTrackerUntil);
		PartCost(3, tt);
		if (full)
		{
			tt = TickCount(0);
			m_Toasts.Update(m_Ctx, true);
			PartCost(4, tt);
		}
		MeasureCost(t0);
	}

	// The player hid the vanilla HUD with its hotkey (IngameHudVisibility HUD_HIDE).
	protected bool VanillaHudHidden()
	{
		Mission mission = GetGame().GetMission();
		if (!mission)
			return false;
		IngameHud ih = IngameHud.Cast(mission.GetHud());
		if (!ih)
			return false;
		IngameHudVisibility vis = ih.GetHudVisibility();
		if (!vis)
			return false;
		return vis.IsContextFlagActive(EHudContextFlags.HUD_HIDE);
	}

	protected void SetShown(bool on)
	{
		if (on == m_Shown)
			return;
		m_Shown = on;
		if (m_Root)
			m_Root.Show(on);
	}

	protected void ShowLayers(bool full)
	{
		if (m_LayerTags)
			m_LayerTags.Show(full);
		if (m_LayerPings)
			m_LayerPings.Show(full);
		if (m_LayerArrows)
			m_LayerArrows.Show(full);
		if (m_LayerToasts)
			m_LayerToasts.Show(full);
	}

	protected void FillCtx(float dt, float now, Man man, bool edit)
	{
		INT_PTY_HudCtx c = m_Ctx;
		c.m_Dt = dt;
		c.m_Now = now;
		c.m_Sw = m_Sw;
		c.m_Sh = m_Sh;
		c.m_Edit = edit;
		INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
		c.m_St = st;
		c.m_World = man != null;
		c.m_Arrows = c.m_St.EdgeArrows == 1;
		if (!c.m_World)
			return;
		vector myPos = man.GetPosition();
		vector camPos = GetGame().GetCurrentCameraPosition();
		c.m_MyPos = myPos;
		c.m_CamPos = camPos;
		vector dir = GetGame().GetCurrentCameraDirection();
		float len = dir.Length();
		if (len < 0.0001)
		{
			dir = man.GetDirection();
			len = dir.Length();
		}
		if (len > 0.0001)
			dir = dir * (1.0 / len);
		c.m_CamDir = dir;
		float hx = dir[2];
		float hz = -dir[0];
		float hl = Math.Sqrt(hx * hx + hz * hz);
		if (hl < 0.0001)
		{
			vector md = man.GetDirection();
			hx = md[2];
			hz = -md[0];
			hl = Math.Sqrt(hx * hx + hz * hz);
			if (hl < 0.0001)
			{
				hx = 1;
				hz = 0;
				hl = 1;
			}
		}
		vector right;
		right[0] = hx / hl;
		right[1] = 0;
		right[2] = hz / hl;
		c.m_Right = right;
		// up = forward × right
		vector up;
		up[0] = dir[1] * right[2] - dir[2] * right[1];
		up[1] = dir[2] * right[0] - dir[0] * right[2];
		up[2] = dir[0] * right[1] - dir[1] * right[0];
		c.m_Up = up;
		float yaw = Math.Atan2(dir[0], dir[2]) * Math.RAD2DEG;
		c.m_CamYaw = yaw;
	}

	// --- building -------------------------------------------------------------------------------------------------
	protected void EnsureRoot()
	{
		int sw = INT_UIF_Scale.ScreenW();
		int sh = INT_UIF_Scale.ScreenH();
		if (m_Root && sw == m_Sw && sh == m_Sh)
			return;
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
		m_Sw = sw;
		m_Sh = sh;
		INT_UIF_Assets.EnsureLoaded();
		INT_PTY_HudKit.s_F = INT_UIF_Scale.Factor();
		m_Root = INT_UIF_Draw.Frame(null, 0, 0, 1, 1, "INT_PTY_HudRoot");
		if (!m_Root)
			return;
		m_Root.SetPos(0, 0);
		m_Root.SetSize(sw, sh);
		m_LayerTags = Layer("INT_PTY_HudTags");
		m_LayerPings = Layer("INT_PTY_HudPings");
		m_LayerArrows = Layer("INT_PTY_HudArrows");
		m_LayerTracker = Layer("INT_PTY_HudTracker");
		m_LayerToasts = Layer("INT_PTY_HudToasts");
		// world markers under the panels: a name tag or a ping projected over the tracker / a card is drawn under it
		// (stand 2026-10-23: a dead friend's tag was painted over the tracker rows)
		SortLayer(m_LayerTags, 1);
		SortLayer(m_LayerPings, 2);
		SortLayer(m_LayerArrows, 3);
		SortLayer(m_LayerTracker, 4);
		SortLayer(m_LayerToasts, 5);
		m_Shown = true;
		// every part is rebuilt into the new layers
		m_KTags = -1;
		m_KPings = -1;
		m_KTracker = -1;
		m_KToasts = -1;
		m_KArrows = -1;
		m_Refresh = true;
	}

	protected void SortLayer(Widget w, int sort)
	{
		if (w)
			w.SetSort(sort);
	}

	protected Widget Layer(string name)
	{
		Widget w = INT_UIF_Draw.Frame(m_Root, 0, 0, 1, 1, name);
		if (!w)
			return null;
		w.SetPos(0, 0);
		w.SetSize(m_Sw, m_Sh);
		return w;
	}

	// Settings + API overrides -> part parameters; a part is rebuilt only when its size or style changed.
	protected void ApplySettings()
	{
		m_Refresh = false;
		INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
		if (!st)
			return;
		INT_PTY_HudKit.CacheTexts();
		bool built = false;
		float kTags = st.TagSize * 0.01 * INT_PTY_UI.PartScale(INT_PTY_UI.PART_TAGS);
		if (kTags != m_KTags || st.TagHpStyle != m_TagStyle)
		{
			m_KTags = kTags;
			m_TagStyle = st.TagHpStyle;
			m_Tags.Build(m_LayerTags, kTags, st.TagHpStyle);
			built = true;
		}
		float kPings = st.PingSize * 0.01 * INT_PTY_UI.PartScale(INT_PTY_UI.PART_PINGS);
		if (kPings != m_KPings)
		{
			m_KPings = kPings;
			m_Pings.Build(m_LayerPings, kPings);
			built = true;
		}
		m_Pings.SetFade(st.PingNearM, st.PingFarM, st.PingViewM);
		float kTrk = st.TrackerSize * 0.01 * INT_PTY_UI.PartScale(INT_PTY_UI.PART_TRACKER);
		if (kTrk != m_KTracker || st.TrackerHpStyle != m_TrkStyle)
		{
			m_KTracker = kTrk;
			m_TrkStyle = st.TrackerHpStyle;
			m_Tracker.Build(m_LayerTracker, kTrk, st.TrackerHpStyle);
			built = true;
		}
		float kToasts = INT_PTY_UI.PartScale(INT_PTY_UI.PART_TOASTS);
		if (kToasts != m_KToasts)
		{
			m_KToasts = kToasts;
			m_Toasts.Build(m_LayerToasts, kToasts);
			built = true;
		}
		float kArrows = INT_PTY_UI.PartScale(INT_PTY_UI.PART_ARROWS);
		if (kArrows != m_KArrows)
		{
			m_KArrows = kArrows;
			m_Arrows.Build(m_LayerArrows, kArrows);
			built = true;
		}
		if (built)
			INT_PTY_UI.OnBuilt().Invoke();
	}

	// Shown friends: name, colour and ping look. H-5: rebuilt only when marked dirty (friend list changed — OnChanged
	// FRIENDS — or settings / API Refresh) plus a slow safety rebuild every MATES_EVERY s; the entries are reused and the
	// capitals (INT_PTY_Text.Upper, ~95 Replace) are recomputed only for a name that changed.
	protected void RebuildMates(float now)
	{
		m_MatesDirty = false;
		m_MatesAt = now;
		if (!INT_PTY_Client.s_Friends)
		{
			m_Mates.Clear();
			return;
		}
		INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
		foreach (string markId, INT_PTY_HudMate mo : m_Mates)
		{
			if (mo)
				mo.m_Order = -1;
		}
		int order = 0;
		foreach (INT_PTY_Friend f : INT_PTY_Client.s_Friends)
		{
			if (!f)
				continue;
			INT_PTY_FriendPref p = st.Pref(f.Id);
			if (!p || p.Shown != 1)
				continue;
			// call results go through locals before they are stored in the object (enforce-compile-traps)
			string id = f.Id;
			string name = INT_PTY_Client.NameOf(id);
			INT_PTY_HudMate m = Mate(id);
			if (!m)
			{
				m = new INT_PTY_HudMate();
				m.m_Id = id;
				m.m_Name = "<none>";
				m_Mates.Set(id, m);
			}
			if (m.m_Name != name)
			{
				string upper = INT_PTY_Text.Upper(name);
				string shortName = upper;
				if (shortName.LengthUtf8() > 14)
					shortName = shortName.SubstringUtf8(0, 13) + "…";
				m.m_Name = name;
				m.m_Upper = upper;
				m.m_Short = shortName;
			}
			int color = INT_PTY_UI.FriendColor(id);
			int pingShape = INT_PTY_UI.PingShapeOf(id, false);
			int pingColor = INT_PTY_UI.PingColorOf(id, false);
			m.m_Color = color;
			m.m_PingShape = pingShape;
			m.m_PingColor = pingColor;
			m.m_Order = order;
			order++;
		}
		// friends no longer shown (or removed) leave the cache; keys collected first, removed after the walk
		m_MateDrop.Clear();
		foreach (string dropId, INT_PTY_HudMate dm : m_Mates)
		{
			if (!dm || dm.m_Order < 0)
				m_MateDrop.Insert(dropId);
		}
		foreach (string gone : m_MateDrop)
		{
			m_Mates.Remove(gone);
		}
	}

	// --- previews (API Preview / stand) ---------------------------------------------------------------------------
	// A ping of mine `meters` ahead of the camera, on this screen only (not sent; Y removes it like any own ping).
	void PreviewPing(float meters)
	{
		if (!INT_PTY_Client.s_Pings || !GetGame().GetPlayer())
			return;
		vector from = GetGame().GetCurrentCameraPosition();
		vector dir = GetGame().GetCurrentCameraDirection();
		dir[1] = 0;
		float len = dir.Length();
		if (len < 0.0001)
			return;
		float m = meters / len;
		vector at = from + dir * m;
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		INT_PTY_PingMark mark = new INT_PTY_PingMark();
		string owner = INT_PTY_Client.s_MySteam;
		int slot = 90 + INT_PTY_Client.s_Pings.Count();
		float born = INT_PTY_Client.Clock();
		mark.Owner = owner;
		mark.SlotIdx = slot;
		mark.Where = at;
		mark.Born = born;
		mark.Gone = 0;
		INT_PTY_Client.s_Pings.Insert(mark);
		INT_PTY_Sfx.Ping(true);
	}

	// Stand: one ping of mine on the crosshair line `meters` away, `dxDesign` design px right of the screen centre
	// (aim dimming checks without turning the camera); earlier preview pings (slots 90+) are dropped.
	void PreviewPingAt(float meters, float dxDesign)
	{
		if (!INT_PTY_Client.s_Pings || !GetGame().GetPlayer())
			return;
		for (int i = INT_PTY_Client.s_Pings.Count() - 1; i >= 0; i--)
		{
			INT_PTY_PingMark old = INT_PTY_Client.s_Pings.Get(i);
			if (old && old.SlotIdx >= 90)
				INT_PTY_Client.s_Pings.Remove(i);
		}
		vector from = GetGame().GetCurrentCameraPosition();
		vector dir = GetGame().GetCurrentCameraDirection();
		dir.Normalize();
		vector at0 = from + dir * meters;
		vector right = Vector(dir[2], 0, -dir[0]);
		right.Normalize();
		vector p0 = GetGame().GetScreenPos(at0);
		vector at1 = at0 + right;
		vector p1 = GetGame().GetScreenPos(at1);
		float per = p1[0] - p0[0];
		if (Math.AbsFloat(per) < 0.01)
			return;
		float s = dxDesign * INT_PTY_HudKit.s_F / per;
		vector at = at0 + right * s;
		at[1] = at[1] - INT_PTY_HudPings.LIFT;
		INT_PTY_PingMark mark = new INT_PTY_PingMark();
		mark.Owner = INT_PTY_Client.s_MySteam;
		mark.SlotIdx = 90;
		mark.Where = at;
		mark.Born = INT_PTY_Client.Clock() - 2.0;
		mark.Gone = 0;
		INT_PTY_Client.s_Pings.Insert(mark);
	}

	void PreviewTracker()
	{
		m_PreviewTrackerUntil = INT_PTY_Client.Clock() + 4.0;
	}

	// --- API ------------------------------------------------------------------------------------------------------
	Widget FindPart(string part)
	{
		if (part == INT_PTY_UI.PART_TRACKER)
			return m_Tracker.GetPanel();
		if (part == INT_PTY_UI.PART_TAGS)
			return m_LayerTags;
		if (part == INT_PTY_UI.PART_PINGS)
			return m_LayerPings;
		if (part == INT_PTY_UI.PART_ARROWS)
			return m_LayerArrows;
		if (part == INT_PTY_UI.PART_TOASTS)
			return m_LayerToasts;
		return null;
	}

	bool GetPartRect(string part, out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		if (!m_Shown)
			return false;
		if (part == INT_PTY_UI.PART_TRACKER)
			return m_Tracker.GetRect(m_Sw, m_Sh, x, y, w, h);
		if (part == INT_PTY_UI.PART_TAGS)
			return m_Tags.GetRect(x, y, w, h);
		if (part == INT_PTY_UI.PART_PINGS)
			return m_Pings.GetRect(x, y, w, h);
		if (part == INT_PTY_UI.PART_ARROWS)
			return m_Arrows.GetRect(x, y, w, h);
		if (part == INT_PTY_UI.PART_TOASTS)
			return m_Toasts.GetRect(m_Sw, m_Sh, x, y, w, h);
		return false;
	}

	bool IsPartActive(string part)
	{
		if (!m_Shown)
			return false;
		if (part == INT_PTY_UI.PART_TRACKER)
			return m_Tracker.IsActive();
		if (part == INT_PTY_UI.PART_TAGS)
			return m_Tags.ActiveCount() > 0;
		if (part == INT_PTY_UI.PART_PINGS)
			return m_Pings.ActiveCount() > 0;
		if (part == INT_PTY_UI.PART_ARROWS)
			return m_Arrows.ActiveCount() > 0;
		if (part == INT_PTY_UI.PART_TOASTS)
			return m_Toasts.ActiveCount() > 0;
		return false;
	}

	// --- frame cost -----------------------------------------------------------------------------------------------
	// TickCount is calibrated against GetTickTime over the first seconds; then the average / worst frame cost of the
	// HUD goes to the log every 10 s when the server runs with DebugMode 1.
	protected void MeasureCost(int t0)
	{
		int ticks = TickCount(t0);
		float t = GetGame().GetTickTime();
		if (m_TicksPerUs < 0)
		{
			if (m_CalTime >= 0)
			{
				int d = TickCount(m_CalTick);
				float dt = t - m_CalTime;
				if (d > 0 && dt > 0 && dt < 5.0)
				{
					m_CalTicks = m_CalTicks + d;
					m_CalSec = m_CalSec + dt;
				}
				if (m_CalSec >= 3.0)
				{
					float tpu = m_CalTicks / (m_CalSec * 1000000.0);
					if (tpu > 0.0005)
						m_TicksPerUs = tpu;
				}
			}
			m_CalTick = TickCount(0);
			m_CalTime = t;
			for (int pk = 0; pk < 5; pk++)
			{
				m_PartTicks.Set(pk, 0);
			}
			return;
		}
		float us = ticks / m_TicksPerUs;
		m_CostSum = m_CostSum + us;
		m_CostFrames++;
		if (us > m_CostMax)
			m_CostMax = us;
		if (t - m_CostAt < COST_EVERY)
			return;
		m_CostAt = t;
		m_LastAvgUs = m_CostSum / Math.Max(1, m_CostFrames);
		float perFrame = 1.0 / (m_TicksPerUs * Math.Max(1, m_CostFrames));
		int c0 = (int)Math.Round(m_PartTicks.Get(0) * perFrame);
		int c1 = (int)Math.Round(m_PartTicks.Get(1) * perFrame);
		int c2 = (int)Math.Round(m_PartTicks.Get(2) * perFrame);
		int c3 = (int)Math.Round(m_PartTicks.Get(3) * perFrame);
		int c4 = (int)Math.Round(m_PartTicks.Get(4) * perFrame);
		m_PartText = " [метки " + c0.ToString() + ", пинги " + c1.ToString() + ", стрелки " + c2.ToString() + ", треккер " + c3.ToString() + ", карточки " + c4.ToString() + "]";
		for (int pj = 0; pj < 5; pj++)
		{
			m_PartTicks.Set(pj, 0);
		}
		m_LastMaxUs = m_CostMax;
		m_CostSum = 0;
		m_CostMax = 0;
		m_CostFrames = 0;
		INT_PTY_Log.Dbg("HUD: кадр " + CostText());
	}

	protected void PartCost(int part, int since)
	{
		int d = TickCount(since);
		m_PartTicks.Set(part, m_PartTicks.Get(part) + d);
	}

	protected string CostText()
	{
		int avg = (int)Math.Round(m_LastAvgUs);
		int mx = (int)Math.Round(m_LastMaxUs);
		string s = "ср. " + avg.ToString() + " мкс, макс " + mx.ToString() + " мкс (меток " + m_Tags.ActiveCount().ToString();
		s = s + ", пингов " + m_Pings.ActiveCount().ToString() + ", стрелок " + m_Arrows.ActiveCount().ToString() + ")" + m_PartText;
		return s;
	}

	string StateLine()
	{
		int shown = 0;
		if (m_Shown)
			shown = 1;
		int edit = 0;
		if (INT_PTY_UI.IsEditMode())
			edit = 1;
		int mover = 0;
		if (INT_PTY_MoverMenu.IsOpen())
			mover = 1;
		int trk = 0;
		if (m_Tracker.IsActive())
			trk = 1;
		string s = "shown " + shown.ToString() + " tags " + m_Tags.ActiveCount().ToString() + " pings " + m_Pings.ActiveCount().ToString();
		s = s + " arrows " + m_Arrows.ActiveCount().ToString() + " tracker " + trk.ToString() + " mates " + m_Mates.Count().ToString();
		s = s + " toasts " + m_Toasts.ActiveCount().ToString() + "/" + m_Toasts.QueueCount().ToString() + " edit " + edit.ToString();
		s = s + " mover " + mover.ToString() + " cost " + CostText();
		return s;
	}
}
