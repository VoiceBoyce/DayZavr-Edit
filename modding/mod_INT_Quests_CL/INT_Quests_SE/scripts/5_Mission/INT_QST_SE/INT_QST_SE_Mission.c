// INT Quests (server) — mission hooks (spec.md §4.1): load once at start, open/close player sessions. No OnUpdate.

modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		INT_QST_SE_Main.Start();
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		INT_QST_SE_Main.OnConnect(player, identity);
	}

	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		INT_QST_SE_Main.OnDisconnect(identity, uid);
		super.PlayerDisconnected(player, identity, uid);
	}

	override void OnMissionFinish()
	{
		INT_QST_SE_Main.Stop();
		super.OnMissionFinish();
	}
}
