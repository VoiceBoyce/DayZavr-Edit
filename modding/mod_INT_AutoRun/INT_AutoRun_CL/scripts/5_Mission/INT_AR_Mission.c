// INT AutoRun - client controller (modded MissionGameplay) + server SE check.
// Load: near-zero when idle (a few LocalPress polls); 2 overrides per tick while running.
modded class MissionGameplay
{
	protected UAInput m_INT_AR_Toggle;
	protected UAInput m_INT_AR_Fwd;
	protected UAInput m_INT_AR_Back;
	protected UAInput m_INT_AR_Turbo;
	protected bool m_INT_AR_InputsResolved;
	protected bool m_INT_AR_Running;
	protected bool m_INT_AR_ForcedRun;
	protected bool m_INT_AR_HelloSent;
	protected float m_INT_AR_AckWait;
	protected int m_INT_AR_SentIdx = -1;
	protected bool m_INT_AR_LastSprint = true;
	protected bool m_INT_AR_TestForceRun;
	protected bool m_INT_AR_TestToast;
	protected ref INT_AR_Hud m_INT_AR_Hud;
	protected int m_INT_AR_HudSW;
	protected int m_INT_AR_HudSH;
	protected float m_INT_AR_ResPoll;
	protected int m_INT_AR_TestStage;
	protected float m_INT_AR_TestTimer;
	protected vector m_INT_AR_TestFrom;
	protected int m_INT_AR_TestPass;
	protected int m_INT_AR_TestFail;

	override void OnInit()
	{
		super.OnInit();
		INT_UIF_Runtime.Init();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		if (GetGame().IsDedicatedServer())
			return;
		INT_AR_ResolveInputs();
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		INT_AR_Handshake(player, timeslice);
		INT_AR_API.ApplyHudPos();
		INT_AR_ResCheck(timeslice);
		INT_AR_SelfTest(player, timeslice);
		if (!m_INT_AR_Running)
		{
			INT_AR_CheckStart(player);
			return;
		}
		INT_AR_TickRun(player);
	}

	override void OnMissionFinish()
	{
		INT_AR_StopRun();
		INT_AR_Net.Reset();
		INT_AR_API.BindHud(null);
		m_INT_AR_HudSW = 0;
		m_INT_AR_HudSH = 0;
		m_INT_AR_ResPoll = 0;
		if (m_INT_AR_Hud)
		{
			m_INT_AR_Hud.Destroy();
			m_INT_AR_Hud = null;
		}
		super.OnMissionFinish();
	}

	protected void INT_AR_ResolveInputs()
	{
		if (m_INT_AR_InputsResolved)
			return;
		m_INT_AR_InputsResolved = true;
		m_INT_AR_Toggle = GetUApi().GetInputByName(INT_AR_Const.UA_TOGGLE);
		m_INT_AR_Fwd = GetUApi().GetInputByName("UAMoveForward");
		m_INT_AR_Back = GetUApi().GetInputByName("UAMoveBack");
		m_INT_AR_Turbo = GetUApi().GetInputByName("UATurbo");
	}

	protected void INT_AR_Handshake(PlayerBase player, float timeslice)
	{
		if (INT_AR_Net.IsAcked())
			return;
		if (!m_INT_AR_HelloSent)
		{
			if (!player.GetIdentity())
				return;
			ScriptRPC rpc = new ScriptRPC(); // once per session: hello
			rpc.Write(true);
			rpc.Send(player, INT_AR_Const.RPC_C2S_HELLO, true, null);
			m_INT_AR_HelloSent = true;
			m_INT_AR_AckWait = 0;
			Print("[INT_AR] hello sent");
			return;
		}
		m_INT_AR_AckWait = m_INT_AR_AckWait + timeslice;
	}

	protected bool INT_AR_MenuBlocks()
	{
		if (EditBoxWidget.Cast(GetFocus()))
			return true;
		if (GetGame().GetUIManager().GetMenu())
			return true;
		return false;
	}

	protected void INT_AR_CheckStart(PlayerBase player)
	{
		if (!m_INT_AR_Toggle)
			return;
		if (!m_INT_AR_Toggle.LocalPress())
			return;
		if (INT_AR_MenuBlocks())
			return;
		if (!INT_AR_CanRun(player, true))
			return;
		if (!INT_AR_Net.IsAcked())
		{
			INT_UIF_Toast.Error("#STR_INT_AR_NO_SERVER");
			Print("[INT_AR] start refused: no server ack");
			m_INT_AR_HelloSent = false;
			return;
		}
		m_INT_AR_Running = true;
		m_INT_AR_ForcedRun = false;
		m_INT_AR_SentIdx = -1;
		m_INT_AR_LastSprint = true;
		INT_AR_API.NotifyRun(true);
		INT_AR_EnsureHud();
		if (m_INT_AR_Hud)
			m_INT_AR_Hud.ShowHud();
		INT_UIF_Toast.Success("#STR_INT_AR_ON");
		Print("[INT_AR] run started");
	}

	protected void INT_AR_TickRun(PlayerBase player)
	{
		if (INT_AR_PressedStop())
		{
			INT_AR_StopRun();
			INT_UIF_Toast.Info("#STR_INT_AR_OFF");
			Print("[INT_AR] run stopped by key");
			return;
		}
		if (!INT_AR_CanRun(player, false))
		{
			INT_AR_StopRun();
			return;
		}
		if (m_INT_AR_Toggle && m_INT_AR_Toggle.LocalPress() && !INT_AR_MenuBlocks())
		{
			INT_AR_StopRun();
			INT_UIF_Toast.Info("#STR_INT_AR_OFF");
			Print("[INT_AR] run toggled off");
			return;
		}
		bool sprint = INT_AR_PickSpeed(player);
		HumanInputController hic = player.GetInputController();
		if (!hic)
		{
			INT_AR_StopRun();
			return;
		}
		int idx = DayZPlayerConstants.MOVEMENTIDX_SPRINT;
		if (!sprint)
			idx = DayZPlayerConstants.MOVEMENTIDX_RUN;
		hic.OverrideMovementSpeed(HumanInputControllerOverrideType.ENABLED, idx);
		hic.OverrideMovementAngle(HumanInputControllerOverrideType.ENABLED, 1);
		INT_AR_SendRunState(player, true, idx);
		if (m_INT_AR_Hud)
			m_INT_AR_Hud.SetSprint(sprint);
		if (sprint != m_INT_AR_LastSprint)
		{
			m_INT_AR_LastSprint = sprint;
			INT_AR_API.NotifySpeed(sprint);
		}
	}

	protected void INT_AR_SendRunState(PlayerBase player, bool running, int idx)
	{
		if (!player)
			return;
		if (running && idx == m_INT_AR_SentIdx)
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(running);
		rpc.Write(idx);
		rpc.Send(player, INT_AR_Const.RPC_C2S_RUN, true, null);
		if (running)
			m_INT_AR_SentIdx = idx;
		else
			m_INT_AR_SentIdx = -1;
	}

	protected bool INT_AR_PressedStop()
	{
		if (INT_AR_MenuBlocks())
			return false;
		if (m_INT_AR_Fwd && m_INT_AR_Fwd.LocalPress())
			return true;
		if (m_INT_AR_Back && m_INT_AR_Back.LocalPress())
			return true;
		return false;
	}

	protected bool INT_AR_PickSpeed(PlayerBase player)
	{
		bool wantSprint = true;
		if (m_INT_AR_Turbo && m_INT_AR_Turbo.LocalValue() > 0.5)
			wantSprint = false;
		// Потолок стамины падает от веса снаряжения (при >43 кг он ниже 35% максимума), и стамина
		// просто не дорастала до STAMINA_OK — спринт не возвращался никогда.
		// Бег: ниже 15% от потолка. Спринт: снова от 35% максимума, но не выше потолка —
		// при перегрузе ждём полного восстановления (как ванильный HasEnoughStaminaFor: cur == cap).
		StaminaHandler stamina = player.GetStaminaHandler();
		if (stamina)
		{
			float cap = stamina.GetStaminaCap();
			float cur = stamina.GetStamina();
			if (cap > 0)
			{
				if (cur < cap * INT_AR_Const.STAMINA_LOW)
					m_INT_AR_ForcedRun = true;
				float okLevel = Math.Min(stamina.GetStaminaMax() * INT_AR_Const.STAMINA_OK, cap - 0.5);
				if (cur >= okLevel)
					m_INT_AR_ForcedRun = false;
			}
		}
		if (m_INT_AR_ForcedRun)
			wantSprint = false;
		// Ваниль сама запретила спринт (истощение, переломы и т.п.) — не спорим с ней, бежим.
		HumanInputController hic = player.GetInputController();
		if (hic && hic.LimitsIsSprintDisabled())
			wantSprint = false;
		if (m_INT_AR_TestForceRun)
			wantSprint = false;
		return wantSprint;
	}

	protected bool INT_AR_CanRun(PlayerBase player, bool silent)
	{
		if (!player.IsAlive())
			return false;
		if (player.IsUnconscious())
		{
			INT_AR_AutoStop("#STR_INT_AR_STOP_UNCONSCIOUS", silent);
			return false;
		}
		if (player.IsRestrained())
		{
			INT_AR_AutoStop("#STR_INT_AR_STOP_RESTRAINED", silent);
			return false;
		}
		if (player.IsInVehicle())
		{
			INT_AR_AutoStop("#STR_INT_AR_STOP_VEHICLE", silent);
			return false;
		}
		if (player.IsSwimming())
		{
			INT_AR_AutoStop("#STR_INT_AR_STOP_WATER", silent);
			return false;
		}
		return true;
	}

	protected void INT_AR_AutoStop(string reason, bool silent)
	{
		if (silent)
			return;
		if (m_INT_AR_Running)
			INT_UIF_Toast.Warning(reason);
		Print("[INT_AR] auto stop");
	}

	void INT_AR_StopRun()
	{
		if (!m_INT_AR_Running)
			return;
		m_INT_AR_Running = false;
		INT_AR_API.NotifyRun(false);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		INT_AR_SendRunState(player, false, 0);
		if (player)
		{
			HumanInputController hic = player.GetInputController();
			if (hic)
			{
				hic.OverrideMovementSpeed(HumanInputControllerOverrideType.DISABLED, 0);
				hic.OverrideMovementAngle(HumanInputControllerOverrideType.DISABLED, 0);
			}
		}
		if (m_INT_AR_Hud)
			m_INT_AR_Hud.HideHud();
	}

	protected void INT_AR_ResCheck(float timeslice)
	{
		m_INT_AR_ResPoll = m_INT_AR_ResPoll + timeslice;
		if (m_INT_AR_ResPoll < 2.0)
			return;
		m_INT_AR_ResPoll = 0;
		if (!m_INT_AR_Hud)
			return;
		int sw;
		int sh;
		GetScreenSize(sw, sh);
		if (sw <= 0 || sh <= 0)
			return;
		if (sw == m_INT_AR_HudSW && sh == m_INT_AR_HudSH)
			return;
		bool shown = m_INT_AR_Hud.IsShown();
		m_INT_AR_Hud.Destroy();
		m_INT_AR_Hud = null;
		INT_AR_EnsureHud();
		if (!m_INT_AR_Hud)
			return;
		m_INT_AR_Hud.SetSprint(INT_AR_API.IsSprint());
		if (shown)
			m_INT_AR_Hud.ShowHud();
	}

	protected void INT_AR_EnsureHud()
	{
		if (m_INT_AR_Hud)
			return;
		if (!GetGame().GetWorkspace())
			return;
		m_INT_AR_Hud = new INT_AR_Hud(); // once: hud lives whole session
		Widget root = m_INT_AR_Hud.BuildAt(null, INT_AR_Const.HUD_X, INT_AR_Const.HUD_Y, INT_AR_Const.HUD_W, INT_AR_Const.HUD_H);
		if (!root)
		{
			m_INT_AR_Hud = null;
			return;
		}
		int sw;
		int sh;
		GetScreenSize(sw, sh);
		m_INT_AR_HudSW = sw;
		m_INT_AR_HudSH = sh;
		INT_AR_API.BindHud(root);
		m_INT_AR_Hud.PlacePill(INT_AR_HudTop());
		m_INT_AR_Hud.HideHud();
	}

	protected float INT_AR_QuickbarBottom()
	{
		IngameHud hud = IngameHud.Cast(GetHud());
		if (!hud)
			return -1;
		Widget panel = hud.GetHudPanelWidget();
		if (!panel)
			return -1;
		Widget qb = panel.FindAnyWidget("QuickbarGrid");
		if (!qb)
			return -1;
		if (!qb.IsVisibleHierarchy())
			return -1;
		float qx;
		float qy;
		qb.GetScreenPos(qx, qy);
		float qw;
		float qh;
		qb.GetScreenSize(qw, qh);
		return qy + qh;
	}

	protected float INT_AR_HudTop()
	{
		float hPx = INT_UIF_Scale.Px(INT_AR_Const.HUD_H);
		int sw;
		int sh;
		GetScreenSize(sw, sh);
		if (sh <= 0)
			return INT_AR_Const.HUD_Y;
		float gridBottom = INT_AR_QuickbarBottom();
		if (gridBottom >= 0)
		{
			float slotsTop = gridBottom - INT_AR_Const.SLOT_ABOVE_GRIDBOTTOM * sh / 1080.0;
			float top = slotsTop - INT_AR_Const.PILL_GAP - hPx;
			Print("[INT_AR] place sw=" + sw.ToString() + " sh=" + sh.ToString() + " qb=" + gridBottom.ToString() + " top=" + top.ToString());
			return top;
		}
		Print("[INT_AR] place FALLBACK sw=" + sw.ToString() + " sh=" + sh.ToString());
		return sh - INT_AR_Const.PILL_BOTTOM_MARGIN - hPx;
	}

	protected void INT_AR_Note(bool ok, string name)
	{
		if (ok)
		{
			m_INT_AR_TestPass = m_INT_AR_TestPass + 1;
			Print("[INT_AR] [TEST] PASS " + name);
		}
		else
		{
			m_INT_AR_TestFail = m_INT_AR_TestFail + 1;
			Print("[INT_AR] [TEST] FAIL " + name);
		}
	}

	protected void INT_AR_SelfTest(PlayerBase player, float timeslice)
	{
		string path = "$profile:int_ar_selftest.txt";
		if (!FileExist(path))
		{
			m_INT_AR_TestStage = 0;
			return;
		}
		if (m_INT_AR_TestStage == 0)
		{
			m_INT_AR_TestStage = 1;
			m_INT_AR_TestPass = 0;
			m_INT_AR_TestFail = 0;
			m_INT_AR_TestTimer = 0;
			m_INT_AR_TestForceRun = false;
			m_INT_AR_TestToast = false;
			Print("[INT_AR] [TEST] begin");
		}
		if (m_INT_AR_TestStage == 1)
		{
			if (!INT_AR_Net.IsAcked())
			{
				if (m_INT_AR_AckWait > 10.0)
				{
					INT_AR_Note(false, "handshake no ack in 10s");
					m_INT_AR_TestStage = 99;
				}
				return;
			}
			INT_AR_Note(true, "handshake");
			string probe = Widget.TranslateString("#STR_INT_AR_TITLE");
			INT_AR_Note(probe != "#STR_INT_AR_TITLE", "i18n");
			m_INT_AR_Running = true;
			m_INT_AR_LastSprint = true;
			INT_AR_API.NotifyRun(true);
			INT_AR_EnsureHud();
			if (m_INT_AR_Hud)
				m_INT_AR_Hud.ShowHud();
			m_INT_AR_TestFrom = player.GetPosition();
			m_INT_AR_TestTimer = 0;
			m_INT_AR_TestStage = 2;
			INT_AR_SendRunState(player, true, DayZPlayerConstants.MOVEMENTIDX_SPRINT);
		}
		if (m_INT_AR_TestStage == 2)
		{
			HumanInputController hic = player.GetInputController();
			if (hic)
			{
				hic.OverrideMovementSpeed(HumanInputControllerOverrideType.ENABLED, DayZPlayerConstants.MOVEMENTIDX_SPRINT);
				hic.OverrideMovementAngle(HumanInputControllerOverrideType.ENABLED, 1);
			}
			m_INT_AR_TestTimer = m_INT_AR_TestTimer + timeslice;
			if (m_INT_AR_TestTimer >= 3.0)
			{
				vector to = player.GetPosition();
				vector d = to - m_INT_AR_TestFrom;
				float dist = Math.Sqrt(d[0] * d[0] + d[2] * d[2]);
				INT_AR_Note(dist > 2.0, "move dist=" + dist.ToString());
				m_INT_AR_TestTimer = 0;
				m_INT_AR_TestStage = 3;
				Print("[INT_AR] [TEST] hold 55s begin");
			}
		}
		if (m_INT_AR_TestStage == 3)
		{
			m_INT_AR_TestTimer = m_INT_AR_TestTimer + timeslice;
			if (m_INT_AR_TestTimer >= 5.0 && !m_INT_AR_TestToast)
			{
				m_INT_AR_TestToast = true;
				INT_UIF_Toast.Show("#STR_INT_AR_ON", INT_UIF_ToastKind.SUCCESS, 40.0);
			}
			if (m_INT_AR_TestTimer >= 27.0)
				m_INT_AR_TestForceRun = true;
			if (m_INT_AR_TestTimer >= 55.0)
			{
				m_INT_AR_TestForceRun = false;
				m_INT_AR_ForcedRun = false;
				INT_AR_StopRun();
				m_INT_AR_TestTimer = 0;
				m_INT_AR_TestStage = 4;
			}
		}
		if (m_INT_AR_TestStage == 4)
		{
			m_INT_AR_TestTimer = m_INT_AR_TestTimer + timeslice;
			if (m_INT_AR_TestTimer >= 0.8)
			{
				bool hudGone = true;
				if (m_INT_AR_Hud && m_INT_AR_Hud.GetRoot() && m_INT_AR_Hud.GetRoot().IsVisible())
					hudGone = false;
				INT_AR_Note(hudGone, "stop hides hud");
				Print("[INT_AR] [TEST] DONE pass=" + m_INT_AR_TestPass.ToString() + " fail=" + m_INT_AR_TestFail.ToString());
				m_INT_AR_TestStage = 99;
			}
		}
	}
}

// Client part is also compiled on the dedicated server (-mod): fail loud when SE is absent.
modded class MissionServer
{
	override void OnMissionStart()
	{
		super.OnMissionStart();
		if (!GetGame().IsDedicatedServer())
			return;
		if (!GetGame().ConfigIsExisting("CfgPatches INT_AutoRun_SE"))
			Error("[INT_AR] server part @INT_AutoRun_SE is missing: autorun handshake disabled");
		else
			Print("[INT_AR] client part on server ok");
	}
}
