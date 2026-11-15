// Команда чата /rift: войти в редактор гнёзд или выйти из него. Заменяет клавишу Ctrl+F8: так у игроков
// в настройках управления нет лишней строки. Права по-прежнему проверяет сервер (Admins в settings.json).
// Команда в чат не уходит. Ванильный чат перехвачен здесь, окно INT_GlobalChat — в аддоне VB_Rift_GC.
// Редактор открывается кадром позже, когда окно чата уже закрыто: под открытым меню VB_OnKey не работает.

class VB_RiftChatCmd
{
	static const string CMD = "/rift";
	static const int WAIT_MS = 3000;		// столько ждём, пока закроется окно чата

	protected static int s_VB_At;			// когда пришла команда (0 — нет)

	// команда ли это; если да — запомнить, редактор переключит VB_Tick
	static bool VB_Take(string text)
	{
		string t = text;
		t.TrimInPlace();
		t.ToLower();
		if (t != CMD)
			return false;
		s_VB_At = g_Game.GetTime();
		if (s_VB_At == 0)
			s_VB_At = 1;
		return true;
	}

	static void VB_Tick()
	{
		if (s_VB_At == 0)
			return;
		if (g_Game.GetTime() - s_VB_At > WAIT_MS)
		{
			s_VB_At = 0;
			return;
		}
		if (g_Game.GetUIManager().GetMenu())
			return;
		s_VB_At = 0;
		VB_RiftEditor.VB_OnKey();
	}
}

modded class ChatInputMenu
{
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		// пустая строка ванильный чат не отправляет, но окно закрывает как обычно
		if (finished && layoutRoot)
		{
			EditBoxWidget box = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("InputEditBoxWidget"));
			if (box && VB_RiftChatCmd.VB_Take(box.GetText()))
				box.SetText("");
		}
		return super.OnChange(w, x, y, finished);
	}
}
