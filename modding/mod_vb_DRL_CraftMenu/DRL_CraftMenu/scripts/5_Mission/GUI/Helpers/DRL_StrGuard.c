// ВРЕМЕННАЯ ДИАГНОСТИКА (2026-11-26, Баги №89): сторож порчи строк рецептов.
// В швейной после смены категории поле Result рецептов превращалось в мусор из освобождённой памяти
// (превью «i», манекен голый). Снимок Result всех рецептов при приёме конфига и сверка на шагах окна
// и каждый кадр. Первое расхождение - в лог клиента: где замечено, последняя чистая точка, стек.
// Убрать, когда причина будет подтверждена.
class DRL_StrGuard
{
	protected static ref map<int, string> s_Snap;
	protected static string s_LastOk = "";
	protected static int s_Reports;
	protected static int s_Checks;
	protected static int s_Frames;

	static void Snap()
	{
		s_Snap = new map<int, string>();
		s_Reports = 0;
		s_LastOk = "снимок";
		if (!g_Craft_Config || !g_Craft_Config.CraftItems)
			return;
		foreach (DRL_Craft_Recipe r : g_Craft_Config.CraftItems)
		{
			if (r)
				s_Snap.Set(r.RecipeId, "#" + r.Result);
		}
		Print("[DRL_StrGuard] снимок: " + s_Snap.Count().ToString() + " рецептов");
	}

	// Шаг окна: при успехе запоминается как последняя чистая точка.
	static void Check(string where)
	{
		if (Scan(where))
		{
			s_LastOk = where;
			s_Frames = 0;
		}
	}

	// Каждый кадр: место не запоминаем, только считаем кадры после последнего шага.
	static void Frame()
	{
		s_Frames++;
		Scan("кадр " + s_Frames.ToString() + " после <" + s_LastOk + ">");
	}

	protected static bool Scan(string where)
	{
		if (s_Reports >= 3 || !s_Snap || !g_Craft_Config || !g_Craft_Config.CraftItems)
			return true;
		s_Checks++;
		foreach (DRL_Craft_Recipe r : g_Craft_Config.CraftItems)
		{
			if (!r)
				continue;
			string was;
			if (!s_Snap.Find(r.RecipeId, was))
				continue;
			string now = "#" + r.Result;
			if (now == was)
				continue;
			s_Reports++;
			string stack;
			DumpStackString(stack);
			Print("[DRL_StrGuard] ИСПОРЧЕНО: рецепт " + r.RecipeId.ToString() + " было <" + was + "> | замечено: <" + where + "> | последняя чистая точка: <" + s_LastOk + "> | проверок: " + s_Checks.ToString());
			Print("[DRL_StrGuard] стек: " + stack);
			// новый снимок, чтобы следующая порча (другого рецепта) тоже попала в лог; не больше 3 записей
			int reports = s_Reports;
			Snap();
			s_Reports = reports;
			return false;
		}
		return true;
	}
}
