modded class MissionServer
{
	protected float m_VBT_Timer;

	override void OnInit()
	{
		super.OnInit();
		VB_TabletConfig.Load();
		VB_TabletClans.Load();
	}

	override void OnMissionStart()
	{
		super.OnMissionStart();
		VB_TabletBank.SpawnAll();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		m_VBT_Timer += timeslice;
		if (m_VBT_Timer >= 30)
		{
			m_VBT_Timer = 0;
			VB_TabletPlayers.Tick();
			VB_TabletClans.DailyTick();
			VB_TabletContracts.Tick();
		}
	}

	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		if (identity)
			VB_TabletPlayers.Release(player, identity.GetPlainId());
		super.PlayerDisconnected(player, identity, uid);
	}
}
