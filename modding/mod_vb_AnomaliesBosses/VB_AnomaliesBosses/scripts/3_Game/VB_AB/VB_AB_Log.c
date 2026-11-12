// Лог мода: строки "[VB_AB] ..." в script-логе.
class VB_AB_Log
{
	static bool s_Debug;		// settings.json: "DebugLog": 1 — подробный лог (попадания по боссу и т. п.)

	static void Dbg(string msg)
	{
		if (s_Debug)
			Print("[VB_AB] [отладка] " + msg);
	}

	static void Info(string msg)
	{
		Print("[VB_AB] " + msg);
	}

	static void Warn(string msg)
	{
		Print("[VB_AB] ВНИМАНИЕ: " + msg);
	}

	// Класс из конфига на сервере не найден. Один и тот же класс встречается у многих боссов (артефакты OOTS
	// на сервере без OOTS — сотни строк), поэтому предупреждение одно на класс, остальное — в сводке.
	protected static ref map<string, int> s_Missing;

	static void Missing(string where, string cls)
	{
		if (!s_Missing)
			s_Missing = new map<string, int>;
		int n = s_Missing.Get(cls);
		if (n == 0)
			Warn("класс " + cls + " не найден на сервере, строки с ним пропускаются (впервые: " + where + ")");
		s_Missing.Set(cls, n + 1);
	}

	static void MissingSummary()
	{
		if (!s_Missing || s_Missing.Count() == 0)
			return;
		int lines = 0;
		foreach (string cls, int n : s_Missing)
			lines += n;
		Warn("всего пропущено строк из-за отсутствующих классов: " + lines + " (классов: " + s_Missing.Count() + ")");
		s_Missing.Clear();
	}
};
