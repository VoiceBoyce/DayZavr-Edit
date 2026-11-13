// Редактор точек боссов в миссии клиента: Ctrl+F9 (UAVBABEditor) — войти / выйти, кадр редактора,
// уборка маркеров и панелей в конце миссии. Сервер редактора отвечает на RPC сам (VB_AB_EditorServer.c).

modded class MissionGameplay
{
	protected UAInput m_VB_ABEditorInput;
	protected bool m_VB_ABInputResolved;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		// GetInputByName вернёт null, если inputs.xml не подхватился: ищем один раз и проверяем
		if (!m_VB_ABInputResolved)
		{
			m_VB_ABInputResolved = true;
			m_VB_ABEditorInput = GetUApi().GetInputByName("UAVBABEditor");
		}
		if (m_VB_ABEditorInput && m_VB_ABEditorInput.LocalPress())
			VB_AB_Editor.VB_OnKey();

		VB_AB_Editor.VB_UpdateStatic(timeslice);
	}

	override void OnMissionFinish()
	{
		VB_AB_Editor.VB_Shutdown();
		super.OnMissionFinish();
	}
}
