// VB_MiningResources_Server: настройки при старте, данные игроку при входе, запись прогресса станков при остановке.
modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		VB_MR_Settings.Get();
	}

	// инструменты, время добычи и рецепты — игроку, чтобы подсказки и полоса действия совпадали с сервером
	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		VB_MR_SendShared(player, identity);
	}

	// вернулся к ещё не вышедшему персонажу (вылет клиента): InvokeOnConnect ваниль тут не вызывает,
	// а у перезапущенной игры данных сервера нет
	override void OnClientReconnectEvent(PlayerIdentity identity, PlayerBase player)
	{
		super.OnClientReconnectEvent(identity, player);
		VB_MR_SendShared(player, identity);
	}

	protected void VB_MR_SendShared(PlayerBase player, PlayerIdentity identity)
	{
		if (!player || !identity || !VB_MR_Shared.s_Data)
			return;

		ScriptRPC rpc = new ScriptRPC();
		VB_MR_Shared.Write(rpc, VB_MR_Shared.s_Data);
		rpc.Send(player, VB_MR_RPC_SHARED, true, identity);
	}

	override void OnMissionFinish()
	{
		VB_MR_Progress.Flush();
		super.OnMissionFinish();
	}
}
