// ============================================================================
// DRL_CraftMenu - тестовый хук нового интерфейса для dayz-mcp.
//
// Файл drl_ui_test.txt в папке профиля КЛИЕНТА. Первая строка — команды через
// пробел или «;». Файл перечитывается раз в 2 с; изменённая строка выполняется
// заново (добавьте «run N», чтобы повторить ту же команду).
//   open              открыть новое окно крафта (ID 9511)
//   learn             открыть новое окно изучения (ID 9513)
//   close             закрыть окно
//   run <x>           любое слово: изменить, чтобы выполнить снова
// Остальные команды — «команда аргумент» (у всех есть аргумент; где он не
// нужен — любой, например x); их выполняет открытое окно (TestCommand):
//   оба окна:  cat <n>, recipe <n>, search <слово>, hover <n>, state x
//   крафт:     tab <n> (0 состав, 1 описание, 2 разбор), var <n> (вариант результата),
//              row|chk|qplus|qminus|info|dv <n> (строка n «Состава»),
//              prow <n>, pclose x (выезжающая панель), reqs x (требования),
//              plus x, minus x (количество), craft x, recraft x,
//              hrow <n>, hoff x (подсказка ингредиента), qsel <n>, qcancel <n> (очередь)
//   изучение:  var <n> (способ изучения), row <n> (строка деталей), study x
// Результаты пишутся в скриптовый лог клиента строками «[DRL_UI] [TEST] ...».
// После теста файл удалить.
// ============================================================================

class DRL_UITest
{
	static const string MARKER = "$profile:drl_ui_test.txt";

	protected static string s_LastLine;
	protected static float s_Timer;
	// Команды, ждущие открытия окна, и задержка до их выполнения.
	protected static string s_Pending;
	protected static float s_PendingDelay;

	static void Log(string msg)
	{
		Print("[DRL_UI] [TEST] " + msg);
	}

	// Вызывается из MissionGameplay.OnUpdate.
	static void Tick(float dt)
	{
		if (s_Pending != "")
		{
			s_PendingDelay = s_PendingDelay - dt;
			if (s_PendingDelay <= 0)
			{
				string cmds = s_Pending;
				s_Pending = "";
				ApplyCurrent(cmds);
			}
		}
		s_Timer = s_Timer + dt;
		if (s_Timer < 2.0)
			return;
		s_Timer = 0;
		if (!FileExist(MARKER))
		{
			s_LastLine = "";
			return;
		}
		FileHandle f = OpenFile(MARKER, FileMode.READ);
		if (!f)
			return;
		string line;
		FGets(f, line);
		CloseFile(f);
		line = line.Trim();
		if (line == "" || line == s_LastLine)
			return;
		s_LastLine = line;
		Log("marker: '" + line + "'");
		Run(line);
	}

	protected static void Run(string line)
	{
		string norm = line;
		norm.Replace(";", " ");
		norm.Replace("\t", " ");
		TStringArray words = new TStringArray();
		norm.Split(" ", words);
		int openId = 0;
		string rest = "";
		foreach (string raw : words)
		{
			string w = raw.Trim();
			if (w == "")
				continue;
			if (w == "open")
			{
				openId = DRL_UI_MENU_CRAFT;
				continue;
			}
			if (w == "learn")
			{
				openId = DRL_UI_MENU_LEARNING;
				continue;
			}
			if (rest != "")
				rest = rest + " ";
			rest = rest + w;
		}
		if (openId != 0)
		{
			bool ok = INT_UIF_Menu.Open(openId);
			Log("open " + openId.ToString() + " ok=" + ok.ToString());
			// Окно строится синхронно, но даём время на анимацию открытия.
			if (rest != "")
			{
				s_Pending = rest;
				s_PendingDelay = 0.6;
			}
			return;
		}
		ApplyCurrent(rest);
	}

	protected static void ApplyCurrent(string commands)
	{
		INT_UIF_Menu menu = INT_UIF_Menu.Current();
		if (!DRL_CraftWindow.Cast(menu) && !DRL_LearnWindow.Cast(menu))
		{
			if (commands.Contains("close"))
				return;
			Log("no DRL window open");
			return;
		}
		Apply(menu, commands);
	}

	protected static string Dispatch(INT_UIF_Menu menu, string cmd, string arg)
	{
		DRL_CraftWindow craft = DRL_CraftWindow.Cast(menu);
		if (craft)
			return craft.TestCommand(cmd, arg);
		DRL_LearnWindow learn = DRL_LearnWindow.Cast(menu);
		if (learn)
			return learn.TestCommand(cmd, arg);
		return "";
	}

	// Команды над открытым окном (также из INT_UIF_Menu.OnTestCommand).
	static void Apply(INT_UIF_Menu menu, string commands)
	{
		if (!menu)
			return;
		TStringArray words = new TStringArray();
		commands.Split(" ", words);
		int n = words.Count();
		int i = 0;
		while (i < n)
		{
			string cmd = words.Get(i).Trim();
			string arg = "";
			if (i + 1 < n)
				arg = words.Get(i + 1).Trim();
			if (cmd == "")
			{
				i++;
				continue;
			}
			if (cmd == "close")
			{
				menu.RequestClose();
				Log("close");
				i++;
				continue;
			}
			if (cmd == "run")
			{
				i = i + 2;
				continue;
			}
			string res = Dispatch(menu, cmd, arg);
			if (res != "")
			{
				Log(cmd + " " + arg + " -> " + res);
				i = i + 2;
				continue;
			}
			Log("unknown command '" + cmd + "'");
			i++;
		}
	}
}
