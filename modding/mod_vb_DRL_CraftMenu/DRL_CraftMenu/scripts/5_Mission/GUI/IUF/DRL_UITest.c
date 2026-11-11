// ============================================================================
// DRL_CraftMenu - тестовый хук нового интерфейса для dayz-mcp.
//
// Файл drl_ui_test.txt в папке профиля КЛИЕНТА. Первая строка — команды через
// пробел или «;». Файл перечитывается раз в 2 с; изменённая строка выполняется
// заново (добавьте «run N», чтобы повторить ту же команду).
//   open              открыть окно крафта (ID 9501)
//   learn             открыть окно изучения (ID 9503)
//   close             закрыть окно
//   station <класс>   открыть станцию рядом (6 м), в т.ч. вложение: машинка на столе
//   run <x>           любое слово: изменить, чтобы выполнить снова
// Остальные команды — «команда аргумент» (у всех есть аргумент; где он не
// нужен — любой, например x); их выполняет открытое окно (TestCommand):
//   оба окна:  cat <n>, recipe <n>, search <слово>, hover <n>, state x
//   крафт:     avail <0|1> («только доступные»), var <n> (вариант результата),
//              row <n> (клик по плитке n ингредиентов: вариации), hrow <n> (наведение: карточка),
//              drow|chk|qplus|qminus|info|dv <n> (плитка n доп. навесного),
//              prow <n>, pclose x (выезжающая панель), reqs x (требования),
//              plus x, minus x, count <n> (количество), craft x, recraft x,
//              qsel <n>, qcancel <n> (очередь)
//   Индекс плитки — номер в списке вместе с заголовками разделов.
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
		if (line.IndexOf("station ") == 0)
		{
			string stCls = line.Substring(8, line.Length() - 8);
			stCls = stCls.Trim();
			OpenStation(stCls);
			return;
		}
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

	// Как ActionOpenDRLWorkbench на клиенте: окно с выбранной станцией. Ищет и во
	// вложениях (швейная машинка на столе в мире не лежит, dayz-mcp её не находит).
	protected static void OpenStation(string cls)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || cls == "")
			return;
		array<Object> objs = new array<Object>();
		array<CargoBase> prx = new array<CargoBase>();
		GetGame().GetObjectsAtPosition(p.GetPosition(), 6.0, objs, prx);
		EntityAI found = null;
		foreach (Object o : objs)
		{
			EntityAI e = EntityAI.Cast(o);
			if (!e || e.IsInherited(Man))
				continue;
			found = FindStation(e, cls, 0);
			if (found)
				break;
		}
		if (!found)
		{
			Log("station " + cls + " not found");
			return;
		}
		DRL_WorkbenchBG.Set(found);
		GetGame().GetUIManager().EnterScriptedMenu(9502, null);
		string parentType = "none";
		EntityAI parentEnt = found.GetHierarchyParent();
		if (parentEnt)
			parentType = parentEnt.GetType();
		Log("station " + cls + " opened, parent=" + parentType);
	}

	protected static EntityAI FindStation(EntityAI e, string cls, int depth)
	{
		if (!e)
			return null;
		if (e.IsKindOf(cls))
			return e;
		if (depth >= 3 || !e.GetInventory())
			return null;
		for (int i = 0; i < e.GetInventory().AttachmentCount(); i++)
		{
			EntityAI found = FindStation(e.GetInventory().GetAttachmentFromIndex(i), cls, depth + 1);
			if (found)
				return found;
		}
		return null;
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
