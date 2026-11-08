modded class MissionServer
{
	protected float m_VBT_Timer;
	protected float m_VBT_Flush;

	override void OnInit()
	{
		super.OnInit();
		VB_TabletConfig.Load();
		VB_TabletClans.Load();
		VB_TabletContracts.Prepare();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		// Отложенная запись (чат клана, настройки показа карты): раз в 5 секунд.
		m_VBT_Flush += timeslice;
		if (m_VBT_Flush >= 5)
		{
			m_VBT_Flush = 0;
			VB_TabletClans.Flush();
			VB_TabletMap.Flush();
		}
		m_VBT_Timer += timeslice;
		if (m_VBT_Timer >= 30)
		{
			m_VBT_Timer = 0;
			VB_TabletPlayers.Tick();
			VB_TabletClans.DailyTick();
			VB_TabletContracts.Tick();
		}
	}

	// Выход без identity и выход с экрана смерти (ваниль тогда не вызывает PlayerDisconnected)
	// подбирает VB_TabletPlayers.Tick: записи тех, кого нет на сервере, сохраняются и выгружаются.
	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		if (identity)
			VB_TabletPlayers.Release(player, identity.GetPlainId());
		super.PlayerDisconnected(player, identity, uid);
	}

	// Остановка сервера: всё, что ждёт записи, на диск.
	override void OnMissionFinish()
	{
		VB_TabletClans.Flush();
		VB_TabletMap.Flush();
		VB_TabletPlayers.SaveAll();
		super.OnMissionFinish();
	}
}
