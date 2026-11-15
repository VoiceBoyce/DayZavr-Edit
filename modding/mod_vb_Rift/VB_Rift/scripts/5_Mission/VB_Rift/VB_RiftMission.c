// Изнанка в миссии клиента: команда /rift (VB_RiftChatCmd) и кадр редактора гнёзд (VB_RiftEditor).
// Серверный запуск (метка сессии предметов, гнёзда) — в серверном моде VB_Rift_Server (VB_RiftMissionServer.c).

modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		VB_RiftChatCmd.VB_Tick();
		VB_RiftEditor.VB_UpdateStatic(timeslice);
	}

	override void OnMissionFinish()
	{
		VB_RiftEditor.VB_Shutdown();
		super.OnMissionFinish();
	}
}
