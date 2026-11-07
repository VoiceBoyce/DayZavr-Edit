// INT_PDA (client) — mission hooks: client core start/stop, hotkeys (P everywhere with an own working PDA, Alt+L admin
// PDA, Ctrl+Alt+P admin config reload), the vanilla chat switched off (owner 2026-10-22: chat only in the PDA), the
// «Заглянуть в КПК» action event, the new character after a respawn, the respawn after «Переродиться» (the «Персонаж»
// tab). Everything here runs on the client only.

class INT_PDA_Mission
{
	protected static bool s_Hooked;
	static bool s_ReloadHeld;

	static void Init()
	{
		if (s_Hooked)
			return;
		INT_PDA_Events.s_OpenRequest.Insert(INT_PDA_Mission.OnOpenRequest);
		s_Hooked = true;
	}

	static void Shutdown()
	{
		if (s_Hooked && INT_PDA_Events.s_OpenRequest)
			INT_PDA_Events.s_OpenRequest.Remove(INT_PDA_Mission.OnOpenRequest);
		s_Hooked = false;
	}

	// «Заглянуть в КПК» on the PDA in hands.
	static void OnOpenRequest(EntityAI pda)
	{
		if (!INT_PDA_Client.PlayerCanUse())
			return;
		INT_PDA_Client.SetAdminMode(false);
		INT_PDA_Menu.OpenWith(INT_PDA.Cast(pda));
	}

	static const int REBORN_DELAY_MS = 1000;		// the death starts to play before the screen goes black
	static const int REBORN_GIVE_UP_MS = 15000;	// the character never died on this side: no respawn

	static void Update(float timeslice)
	{
		if (!GetGame() || !GetGame().GetPlayer())
			return;
		TickReborn();
		UIScriptedMenu open = GetGame().GetUIManager().GetMenu();
		if (open)
			return;
		if (GetUApi().GetInputByName("UAIntPdaOpen").LocalPress())
		{
			// Ctrl+Alt+P = admin config reload (owner 2026-10-22); a 3-key chord is checked here by key state
			if (KeyState(KeyCode.KC_LCONTROL) != 0 && KeyState(KeyCode.KC_LMENU) != 0)
			{
				ReloadConfig();
				return;
			}
			OpenFromInventory();
			return;
		}
		if (GetUApi().GetInputByName("UAIntPdaReload").LocalPress())
		{
			ReloadConfig();
			return;
		}
		if (GetUApi().GetInputByName("UAIntPdaAdmin").LocalPress())
		{
			// the PDA (also the admin one) is not used dead, unconscious or restrained
			if (!INT_PDA_Client.s_IsAdmin || !INT_PDA_Client.PlayerCanUse())
				return;
			INT_PDA_Client.SetAdminMode(true);
			INT_PDA_Menu.OpenWith(INT_PDA_Client.FindPda(false));
		}
	}

	// «Переродиться»: the server killed the character; once his death reached this client, the vanilla respawn
	// request of the pause menu (InGameMenu.GameRespawn) is sent without the menu. The server then makes the new
	// character of the chosen class (MissionServer.CreateCharacter of INT_PDA_SE).
	protected static int s_DeadSeenMs = -1;

	static void TickReborn()
	{
		if (!INT_PDA_Client.s_RebornPending)
		{
			s_DeadSeenMs = -1;
			return;
		}
		int now = GetGame().GetTime();
		Man me = GetGame().GetPlayer();
		if (me.IsAlive())
		{
			if (now - INT_PDA_Client.s_RebornAtMs > REBORN_GIVE_UP_MS)
			{
				INT_PDA_Client.s_RebornPending = false;
				INT_PDA_Log.Warn("перерождение: персонаж не погиб за 15 с, респаун не запрошен");
			}
			return;
		}
		if (s_DeadSeenMs < 0)
			s_DeadSeenMs = now;
		if (now - s_DeadSeenMs < REBORN_DELAY_MS)
			return;
		INT_PDA_Client.s_RebornPending = false;
		s_DeadSeenMs = -1;
		Reborn();
	}

	protected static void Reborn()
	{
		INT_PDA_Log.Dbg("перерождение: запрос респауна");
		GetGame().GetMenuDefaultCharacterData(false).SetRandomCharacterForced(false);
		GetGame().RespawnPlayer();
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			player.SimulateDeath(true);
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(player.ShowDeadScreen, true, 0);
		}
		MissionGameplay missionGP = MissionGameplay.Cast(GetGame().GetMission());
		if (!missionGP)
			return;
		missionGP.DestroyAllMenus();
		missionGP.SetPlayerRespawning(true);
		missionGP.Continue();
	}

	// P: the own working PDA anywhere in the inventory (also in a vehicle); not dead, unconscious or restrained.
	static void OpenFromInventory()
	{
		if (!INT_PDA_Client.PlayerCanUse())
			return;
		INT_PDA pda = INT_PDA_Client.FindPda(true);
		if (!pda)
		{
			if (INT_PDA_Client.FindPda(false))
				INT_UIF_Toast.Warning("#STR_INT_PDA_TOAST_PDA_OFF");
			return;
		}
		INT_PDA_Client.SetAdminMode(false);
		INT_PDA_Menu.OpenWith(pda);
	}

	static void ReloadConfig()
	{
		if (!INT_PDA_Client.s_IsAdmin)
			return;
		INT_PDA_Client.Send(INT_PDA_Client.Admin(INT_PDA_Const.ADM_RELOAD_CONFIG));
		INT_UIF_Toast.Info("#STR_INT_PDA_TOAST_RELOAD_SENT");
	}
}

modded class MissionGameplay
{
	override void OnInit()
	{
		super.OnInit();
		INT_PDA_Client.Init();
		INT_PDA_Mission.Init();
		INT_UIF_MenuRegistry.Register(INT_PDA_Const.MENU_PDA, INT_PDA_Menu);
		INT_PDA_Hud.Init();
	}

	override void OnMissionFinish()
	{
		INT_PDA_Hud.Shutdown();
		INT_PDA_Mission.Shutdown();
		INT_PDA_Client.Shutdown();
		super.OnMissionFinish();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		INT_PDA_Client.Tick();
		INT_PDA_Mission.Update(timeslice);
		INT_PDA_Hud.Update(timeslice);
	}

	// A new character of this client (vanilla calls it from PlayerBase.OnPlayerLoaded for the controlled player: the
	// first spawn and every respawn). The old body keeps its PDA, so the radio is re-checked for the new one.
	override void OnPlayerRespawned(Man player)
	{
		super.OnPlayerRespawned(player);
		INT_PDA_Client.OnNewCharacter(player);
	}

	// The vanilla chat input never opens while the server switches the vanilla chat off.
	override void ShowChat()
	{
		if (INT_PDA_Client.VanillaChatOff())
			return;
		super.ShowChat();
	}
}

modded class Chat
{
	override void Add(ChatMessageEventParams params)
	{
		if (INT_PDA_Client.VanillaChatOff())
			return;
		super.Add(params);
	}
}
