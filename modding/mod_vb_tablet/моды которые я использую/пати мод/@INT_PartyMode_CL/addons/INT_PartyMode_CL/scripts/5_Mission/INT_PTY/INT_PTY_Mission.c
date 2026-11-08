// INT PartyMode — client mission hooks (spec.md §8). OnUpdate drives the client core and the keys of the
// «INT DAYZAVR PARTY» group: P window, T ping, Y my pings / held Y every ping on my screen. A key pressed together
// with Alt or Ctrl belongs to other mods (Alt+Y = INT_Radiation mover) and is ignored.

modded class MissionGameplay
{
	protected float m_INT_PTY_YDownAt = -1;
	protected bool m_INT_PTY_YHeld;

	static const float INT_PTY_HOLD_S = 0.6;

	override void OnInit()
	{
		super.OnInit();
		INT_PTY_Log.s_Tag = "[INT_PTY]";
		INT_PTY_SettingsStore.Get();
		INT_PTY_Client.Init();
		INT_PTY_Hud.Init();
		INT_PTY_Log.Raw("ready " + INT_PTY_Const.VERSION);
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		INT_PTY_Client.Tick(timeslice);
		INT_PTY_Hud.Tick(timeslice);
		INT_PTY_MapLayer.Tick(timeslice);
		INT_PTY_Keys();
	}

	override void OnMissionFinish()
	{
		INT_PTY_MapLayer.Shutdown();
		INT_PTY_Hud.Shutdown();
		INT_PTY_Client.Shutdown();
		super.OnMissionFinish();
	}

	protected bool INT_PTY_Modifier()
	{
		if (KeyState(KeyCode.KC_LMENU) != 0 || KeyState(KeyCode.KC_RMENU) != 0)
			return true;
		return KeyState(KeyCode.KC_LCONTROL) != 0 || KeyState(KeyCode.KC_RCONTROL) != 0;
	}

	protected void INT_PTY_Keys()
	{
		UIManager um = GetGame().GetUIManager();
		bool menuOpen = false;
		if (um && um.GetMenu())
			menuOpen = true;
		bool mod = INT_PTY_Modifier();
		UAInput menuKey = GetUApi().GetInputByName("UAIntPartyMenu");
		if (menuKey && menuKey.LocalPress() && !mod)
		{
			if (!menuOpen || INT_PTY_Menu.IsOpen())
				INT_PTY_Menu.Toggle();
			return;
		}
		if (menuOpen)
		{
			m_INT_PTY_YDownAt = -1;
			return;
		}
		float now = INT_PTY_Client.Clock();
		UAInput ping = GetUApi().GetInputByName("UAIntPartyPing");
		if (ping && ping.LocalPress() && !mod)
			INT_PTY_Client.PlacePing();
		UAInput clear = GetUApi().GetInputByName("UAIntPartyClear");
		if (!clear)
			return;
		if (clear.LocalPress() && !mod)
		{
			m_INT_PTY_YDownAt = now;
			m_INT_PTY_YHeld = false;
		}
		if (m_INT_PTY_YDownAt < 0)
			return;
		if (!m_INT_PTY_YHeld && now - m_INT_PTY_YDownAt >= INT_PTY_HOLD_S && clear.LocalValue() > 0)
		{
			m_INT_PTY_YHeld = true;
			INT_PTY_Client.ClearPings(true);
		}
		if (clear.LocalRelease() || clear.LocalValue() <= 0)
		{
			if (!m_INT_PTY_YHeld)
				INT_PTY_Client.ClearPings(false);
			m_INT_PTY_YDownAt = -1;
		}
	}
}
