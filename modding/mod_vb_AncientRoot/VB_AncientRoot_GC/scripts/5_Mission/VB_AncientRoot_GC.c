// Команда /grove в окне INT_GlobalChat: не отправлять её в чат, окно закрыть, редактор переключит VB_AR_ChatCmd.
// Остальной текст чат обрабатывает сам.

modded class INT_GC_ChatMenu
{
	override protected void Submit()
	{
		if (m_Win && m_Win.m_Edit && VB_AR_ChatCmd.VB_Take(m_Win.m_Edit.GetText()))
		{
			m_Win.m_Edit.SetText("");
			s_Draft = "";
			m_Recall = -1;
			UpdateCounter();
			RequestClose();
			return;
		}
		super.Submit();
	}
}
