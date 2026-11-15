// Редактор точек боссов в миссии клиента: команда /boss (VB_AB_ChatCmd) — войти / выйти, кадр редактора,
// уборка маркеров и панелей в конце миссии. Сервер редактора отвечает на RPC сам (VB_AB_EditorServer.c).

modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		VB_AB_ChatCmd.VB_Tick();
		VB_AB_Editor.VB_UpdateStatic(timeslice);
	}

	override void OnMissionFinish()
	{
		VB_AB_Editor.VB_Shutdown();
		super.OnMissionFinish();
	}
}
