// INT GlobalChat (server) — mission hooks: files once at start, «вышел» on disconnect. No OnUpdate, no timers
// (owner rules 1-3).

modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		INT_GC_Log.s_Tag = "[INT_GC_SE]";
		INT_GC_SE_Config.Load();
		INT_GC_SE_Core.Init();
		INT_GC_Log.Info("INT GlobalChat " + INT_GC_Const.VERSION + " протокол " + INT_GC_Const.PROTOCOL.ToString() + " api " + INT_GC_SE_API.API_VERSION.ToString());
		INT_GC_Log.Raw("ready");
	}

	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		INT_GC_SE_Core.OnDisconnected(uid);
		super.PlayerDisconnected(player, identity, uid);
	}

	override void OnMissionFinish()
	{
		INT_GC_SE_Core.Shutdown();
		super.OnMissionFinish();
	}
}
