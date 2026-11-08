// INT PartyMode (server) — mission hooks: files once at start, connect / reconnect / disconnect events. No OnUpdate,
// no timers (owner rules 1-3). Every override calls super first (or last for the disconnect cleanup, which needs
// our uid map before vanilla drops the body).

modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		INT_PTY_Log.s_Tag = "[INT_PTY_SE]";
		INT_PTY_SE_Config.Load();
		INT_PTY_SE_Store.Init();
		INT_PTY_SE_History.Open();
		INT_PTY_SE_Core.Init();
		INT_PTY_Log.Info("INT PartyMode " + INT_PTY_Const.VERSION + " протокол " + INT_PTY_Const.PROTOCOL.ToString() + " api " + INT_PTY_SE_API.API_VERSION.ToString());
		INT_PTY_Log.Raw("ready");
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		INT_PTY_SE_Core.OnConnect(player, identity);
	}

	override void OnClientReconnectEvent(PlayerIdentity identity, PlayerBase player)
	{
		super.OnClientReconnectEvent(identity, player);
		INT_PTY_SE_Core.OnConnect(player, identity);
	}

	// Vanilla returns early for a disconnect WITHOUT a body (missionserver.c ClientDisconnectedEventTypeID: «Player is
	// empty») and never calls PlayerDisconnected then — our record would stay as a ghost (review PTY-06). Event only.
	override void OnEvent(EventType eventTypeId, Param params)
	{
		if (eventTypeId == ClientDisconnectedEventTypeID)
		{
			ClientDisconnectedEventParams disco;
			Class.CastTo(disco, params);
			if (disco && disco.param1 && !disco.param2)
			{
				string ghostUid = disco.param1.GetId();
				INT_PTY_SE_Core.OnDisconnected(ghostUid);
			}
		}
		super.OnEvent(eventTypeId, params);
	}

	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		INT_PTY_SE_Core.OnDisconnected(uid);
		super.PlayerDisconnected(player, identity, uid);
	}

	override void OnMissionFinish()
	{
		INT_PTY_SE_Core.Shutdown();
		INT_PTY_SE_History.Close();
		super.OnMissionFinish();
	}
}
