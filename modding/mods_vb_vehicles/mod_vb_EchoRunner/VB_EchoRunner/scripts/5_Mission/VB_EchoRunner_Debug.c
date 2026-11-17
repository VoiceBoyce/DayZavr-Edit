// Echo Runner: отладка полёта на стенде dayz-mcp (только DayZDiag, DIAG_DEVELOPER). Стенд передаёт нажатия клавиш
// в миссию, но не в игровые действия; цифровой блок дублирует клавиши полёта: 8/2 — вперёд/назад, 4/6 — поворот,
// 9/3 — нос вверх/вниз. Значения читает VB_EchoRunner.VB_ReadInputsOf. В обычной игре код не собирается.

#ifdef DIAG_DEVELOPER
modded class MissionGameplay
{
	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);
		VB_ERDebug.Key(key, 1.0);
	}

	override void OnKeyRelease(int key)
	{
		super.OnKeyRelease(key);
		VB_ERDebug.Key(key, 0.0);
	}
}
#endif
