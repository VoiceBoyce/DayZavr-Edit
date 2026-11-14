// INT Quests (client) — mission hooks: network client, per-frame tick (giver lookup, exploration check, HUD,
// markers), the journal key J (owner 2026-10-22, was P; an input poll, as every keybind in DayZ) and the tracker
// show/hide key (no default key: players bind it themselves). On the server this module is loaded too (the CL pbo
// goes with -mod everywhere) but MissionGameplay never exists there.

modded class MissionGameplay
{
	override void OnInit()
	{
		super.OnInit();
		INT_QST_Log.s_Tag = "[INT_QST]";
		INT_QST_Client.Init();
		INT_QST_Ui.Init();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		INT_QST_Client.Tick(timeslice);
		INT_QST_Ui.Tick(timeslice);
		INT_QST_JournalKey();
		INT_QST_TrackerKey();
	}

	// Show / hide the HUD tracker (the player's choice, kept in the client profile).
	protected void INT_QST_TrackerKey()
	{
		UAInput key = GetUApi().GetInputByName("UAINTQuestsTracker");
		if (!key || !key.LocalPress())
			return;
		UIManager um = GetGame().GetUIManager();
		if (um && um.GetMenu())
			return;		// chat typing, inventory, other windows
		INT_QST_UI.TogglePlayerShown();
	}

	protected void INT_QST_JournalKey()
	{
		UAInput key = GetUApi().GetInputByName("UAINTQuestsJournal");
		if (!key || !key.LocalPress())
			return;
		// LAlt+J belongs to INT_GlobalChat (show/hide the chat)
		if (KeyState(KeyCode.KC_LMENU) != 0 || KeyState(KeyCode.KC_RMENU) != 0)
			return;
		UIManager um = GetGame().GetUIManager();
		if (um && um.GetMenu())
			return;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || !p.IsAlive())
			return;
		if (p.IsInVehicle())
			return;		// the window freezes the controls: never while driving
		INT_QST_UiBridge.Get().OpenJournal();
	}

	override void OnMissionFinish()
	{
		INT_QST_Ui.Shutdown();
		INT_QST_Client.Shutdown();
		super.OnMissionFinish();
	}
}
