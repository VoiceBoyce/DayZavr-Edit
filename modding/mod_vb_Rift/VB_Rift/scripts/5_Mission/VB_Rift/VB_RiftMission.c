// Изнанка в миссии клиента: клавиша и окно редактора гнёзд (VB_RiftEditor).
// Серверный запуск (метка сессии предметов, гнёзда) — в серверном моде VB_Rift_Server (VB_RiftMissionServer.c).

modded class MissionGameplay
{
	protected UAInput m_VB_RiftEditorInput;
	protected bool m_VB_RiftInputResolved;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		// GetInputByName вернёт null, если inputs.xml не подхватился: ищем один раз и проверяем
		if (!m_VB_RiftInputResolved)
		{
			m_VB_RiftInputResolved = true;
			m_VB_RiftEditorInput = GetUApi().GetInputByName("UAVBRiftEditor");
		}
		if (m_VB_RiftEditorInput && m_VB_RiftEditorInput.LocalPress())
			VB_RiftEditor.VB_OnKey();

		VB_RiftEditor.VB_UpdateStatic(timeslice);
	}

	override void OnMissionFinish()
	{
		VB_RiftEditor.VB_Shutdown();
		super.OnMissionFinish();
	}
}
