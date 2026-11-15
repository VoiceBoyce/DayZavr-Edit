// Изнанка в миссии клиента: команда /grove (VB_AR_ChatCmd) и кадр редактора гнёзд (VB_AR_Editor).
// Серверный запуск (метка сессии предметов, гнёзда) — в серверном моде VB_AncientRoot_Server (VB_AR_MissionServer.c).

modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		VB_AR_ChatCmd.VB_Tick();
		VB_AR_Editor.VB_UpdateStatic(timeslice);
		VB_AR_FavorHud.VB_Tick(timeslice);		// знак расположения леса в худе
	}

	override void OnMissionFinish()
	{
		VB_AR_FavorHud.VB_Shutdown();
		VB_AR_Editor.VB_Shutdown();
		super.OnMissionFinish();
	}
}
