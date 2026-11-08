// INT GlobalChat — client mission hooks (spec.md §3.2). The whole contact with vanilla chat is two one-method
// overrides with stable signatures (byte-identical 1.29 -> 1.30):
//   MissionGameplay.ShowChat()  — the vanilla UAChat poll (Enter) opens our window instead of ChatInputMenu
//   Chat.Add(params)            — every vanilla line (nearby, admin, system, BattlEye) goes to our window
// OnUpdate: the client core (HELLO), the view tick (returns at once while nothing is shown) and the LAlt+J key.

modded class MissionGameplay
{
	override void OnInit()
	{
		super.OnInit();
		INT_GC_Log.s_Tag = "[INT_GC]";
		INT_UIF_MenuRegistry.Register(INT_GC_Const.MENU_CHAT, INT_GC_ChatMenu);
		INT_UIF_MenuRegistry.Register(INT_GC_Const.MENU_EDITOR, INT_GC_EditorMenu);
		INT_GC_UI.Style();				// loads the player's file
		INT_GC_View.Get();				// registers the view behind the public API
		INT_GC_Client.Init();
		INT_GC_Log.Raw("ready " + INT_GC_Const.VERSION + " gui api " + INT_GC_UI.GUI_API_VERSION.ToString());
	}

	// Vanilla calls this only when no menu is open, on PC, for the player's own UAChat binding.
	override void ShowChat()
	{
		INT_GC_View view = INT_GC_View.Get();
		if (view)
			view.OpenWindow();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		INT_GC_Client.Tick(timeslice);
		INT_GC_View view = INT_GC_View.Peek();
		if (view)
			view.Tick(timeslice);
		INT_GC_InputPoll();
	}

	protected void INT_GC_InputPoll()
	{
		UIManager um = GetGame().GetUIManager();
		bool menuOpen = false;
		if (um && um.GetMenu())
			menuOpen = true;
		if (!menuOpen && INT_GC_EditorMenu.TakeReopen())
		{
			INT_GC_View v = INT_GC_View.Peek();
			if (v)
				v.OpenWindow();
			return;
		}
		UAInput toggle = GetUApi().GetInputByName("UAIntGcToggle");
		if (!toggle || !toggle.LocalPress())
			return;
		if (menuOpen && !INT_GC_ChatMenu.Instance())
			return;
		INT_GC_UI.ToggleHidden();
		string key = "#STR_INT_GC_SHOWN";
		if (INT_GC_UI.IsHidden())
			key = "#STR_INT_GC_HIDDEN";
		INT_UIF_Toast.Info(Widget.TranslateString(key));
	}

	override void OnMissionFinish()
	{
		INT_GC_View.Shutdown();
		INT_GC_Client.Shutdown();
		super.OnMissionFinish();
	}
}

modded class Chat
{
	// No super: vanilla lines are drawn only in our window. The three vanilla chat filters of the game options
	// (game / admin / player messages) still apply.
	override void Add(ChatMessageEventParams params)
	{
		if (!params)
			return;
		int channel = params.param1;
		if ((channel & CCSystem) || (channel & CCBattlEye))
		{
			if (g_Game.GetProfileOption(EDayZProfilesOptions.GAME_MESSAGES))
				return;
		}
		else if (channel & CCAdmin)
		{
			if (g_Game.GetProfileOption(EDayZProfilesOptions.ADMIN_MESSAGES))
				return;
		}
		else if ((channel & CCDirect) || (channel & CCMegaphone) || (channel & CCTransmitter) || (channel & CCPublicAddressSystem))
		{
			if (g_Game.GetProfileOption(EDayZProfilesOptions.PLAYER_MESSAGES))
				return;
		}
		INT_GC_Client.PushVanilla(channel, params.param2, params.param3, params.param4);
	}
}
