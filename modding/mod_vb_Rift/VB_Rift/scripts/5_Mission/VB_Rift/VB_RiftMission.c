// Изнанка в миссии: сервер на старте ставит гнёзда (VB_RiftNests), клиент — клавиша и окно редактора (VB_RiftEditor).

modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		VB_RiftItems.VB_Session();	// метка этой сессии: «Сердце Изнанки» из прошлой протухнет
		VB_RiftNests.VB_StartServer();
	}
}

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
