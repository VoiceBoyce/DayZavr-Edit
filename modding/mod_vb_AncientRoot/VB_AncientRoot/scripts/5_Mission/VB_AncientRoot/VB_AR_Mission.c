// «Древний корень» в миссии клиента: команда /grove (VB_AR_ChatCmd), кадр редактора гнёзд (VB_AR_Editor), ветвь
// расположения в худе (VB_AR_FavorHud) и сброс присланного расположения на старте и в конце миссии (VB_AR_Favor.VB_Reset:
// статики переживают переподключение, а время миссии начинается с нуля).
// Серверный запуск (метка сессии предметов, гнёзда) — в серверном моде VB_AncientRoot_Server (VB_AR_MissionServer.c).

modded class MissionGameplay
{
	override void OnMissionStart()
	{
		VB_AR_Favor.VB_Reset();
		super.OnMissionStart();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		VB_AR_ChatCmd.VB_Tick();
		VB_AR_Editor.VB_UpdateStatic(timeslice);
		VB_AR_FavorHud.VB_Tick(timeslice);		// ветвь расположения леса в худе
	}

	override void OnMissionFinish()
	{
		VB_AR_FavorHud.VB_Shutdown();
		VB_AR_Editor.VB_Shutdown();
		VB_AR_Favor.VB_Reset();
		super.OnMissionFinish();
	}
}
