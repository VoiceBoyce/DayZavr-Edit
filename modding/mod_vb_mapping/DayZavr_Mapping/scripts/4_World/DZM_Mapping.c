// ============================================================================
// DayZavr_Mapping — загрузка маппинга из файлов .map (экспорт DayZ Editor, формат Expansion).
//
// Строка файла: «имя|x y z|рысканье тангаж крен[|масштаб]».
//   имя = класс       — объект по классу: создаёт только сервер, клиенты получают по сети;
//   имя = путь .p3d   — статичная модель: каждая сторона создаёт у себя (локально);
//   «-» перед именем  — скрыть объект карты в этой точке (на сервере и у каждого клиента).
// Файлы и миры перечислены в config.cpp, класс CfgDayZavrMapping.
// Одинаковые строки (в одном файле или в разных) применяются один раз.
// ============================================================================

class DZM_Mapping
{
	static const string CFG_ROOT = "CfgDayZavrMapping";
	static const string LOG_TAG = "[DayZavr_Mapping] ";

	// Радиус поиска скрываемого объекта вокруг точки из файла, м.
	static const float HIDE_RADIUS = 0.5;
	// Сколько ненайденных объектов расписывать в логе подробно (что стоит рядом).
	static const int HIDE_REPORT_MAX = 5;

	// Флаги объекта по классу — как у ванильного спавнера cfggameplay (ObjectSpawnerHandler).
	static const int SPAWN_FLAGS = ECE_SETUP | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS | ECE_NOLIFETIME | ECE_DYNAMIC_PERSISTENCY;

	protected static bool s_Applied;

	// Вызывается из MissionServer.OnInit (сервер) и MissionGameplay.OnMissionStart (клиент).
	static void Apply()
	{
		if (s_Applied)
			return;
		s_Applied = true;

		bool isServer = GetGame().IsServer();
		string world = GetGame().GetWorldName();
		world.ToLower();

		// Одинаковая строка применяется один раз на все файлы: файлы маппинга пересекаются
		// (FullMapping повторяет удаления VS_Base), а редактор иногда пишет одну строку много раз.
		map<string, bool> seen = new map<string, bool>();
		array<ref DZM_File> files = new array<ref DZM_File>();
		int count = GetGame().ConfigGetChildrenCount(CFG_ROOT);
		for (int i = 0; i < count; i++)
		{
			string entry;
			GetGame().ConfigGetChildName(CFG_ROOT, i, entry);
			string path = CFG_ROOT + " " + entry;
			string file = GetGame().ConfigGetTextOut(path + " file");
			string fileWorld = GetGame().ConfigGetTextOut(path + " world");
			fileWorld.ToLower();
			if (fileWorld != "" && fileWorld != world)
			{
				Print(LOG_TAG + entry + ": мир " + fileWorld + ", текущий " + world + " — пропуск");
				continue;
			}
			DZM_File loaded = LoadFile(entry, file, seen, isServer);
			if (loaded)
				files.Insert(loaded);
		}
		seen = null;

		// Сначала скрытие из всех файлов, потом создание: иначе поиск может задеть
		// объект, поставленный другим файлом маппинга.
		int reported = 0;
		foreach (DZM_File hf : files)
			reported = HideFile(hf, isServer, reported);
		foreach (DZM_File sf : files)
			SpawnFile(sf, isServer);

		if (isServer)
			GetGame().GetWorld().ProcessMarkedObjectsForPathgraphUpdate();

		foreach (DZM_File rf : files)
			rf.Report(isServer);
	}

	// Объекты по классу создаёт только сервер, поэтому клиент их строки не держит.
	protected static DZM_File LoadFile(string entry, string file, map<string, bool> seen, bool isServer)
	{
		FileHandle handle = OpenFile(file, FileMode.READ);
		if (handle == 0)
		{
			Print(LOG_TAG + "ОШИБКА: не открывается файл " + file);
			return null;
		}

		DZM_File res = new DZM_File(entry);
		string line;
		while (FGets(handle, line) >= 0)
		{
			line.Replace("\r", "");
			line = line.Trim();
			if (line == "" || line.IndexOf("//") == 0)
				continue;
			DZM_Line parsed = DZM_Line.Parse(line);
			if (!parsed)
			{
				res.m_Bad++;
				continue;
			}
			if (seen.Contains(line))
			{
				res.m_Repeats++;
				continue;
			}
			seen.Set(line, true);
			if (parsed.m_Hide)
				res.m_Hides.Insert(parsed);
			else if (parsed.m_IsModel || isServer)
				res.m_Spawns.Insert(parsed);
		}
		CloseFile(handle);
		return res;
	}

	protected static int HideFile(DZM_File f, bool isServer, int reported)
	{
		foreach (DZM_Line h : f.m_Hides)
		{
			if (HideAt(h.m_Name, h.m_Pos, isServer))
			{
				f.m_Hidden++;
				continue;
			}
			if (reported < HIDE_REPORT_MAX)
			{
				ReportMissing(h.m_Name, h.m_Pos);
				reported++;
			}
		}
		return reported;
	}

	protected static void SpawnFile(DZM_File f, bool isServer)
	{
		foreach (DZM_Line s : f.m_Spawns)
		{
			Object obj = null;
			if (s.m_IsModel)
			{
				// Позицию и поворот задаём после создания: редактор пишет в файл GetPosition()
				// и GetOrientation() (градусы), а CreateStaticObjectUsingP3D понимает их по-своему
				// (дерево, поставленное на 50.5, отдаёт GetPosition 58.25).
				obj = GetGame().CreateStaticObjectUsingP3D(s.m_Name, s.m_Pos, vector.Zero, s.m_Scale, true);
				if (obj)
				{
					obj.SetPosition(s.m_Pos);
					obj.SetOrientation(s.m_Ypr);
					obj.Update();
					f.m_Models++;
				}
			}
			else
			{
				obj = GetGame().CreateObjectEx(s.m_Name, s.m_Pos, SPAWN_FLAGS, RF_IGNORE);
				if (obj)
				{
					obj.SetOrientation(s.m_Ypr);
					if (s.m_Scale != 1)
						obj.SetScale(s.m_Scale);
					f.m_Classes++;
				}
			}
			if (!obj)
				f.AddFailed(s.m_Name);
		}
	}

	// Ищет ближайший к точке объект карты с этим именем и скрывает его.
	protected static bool HideAt(string name, vector pos, bool isServer)
	{
		string want = name;
		want.ToLower();
		bool byModel = DZM_Line.IsModelPath(want);
		if (byModel)
			want = ModelBaseName(want);

		array<Object> objects = new array<Object>();
		array<CargoBase> proxies = new array<CargoBase>();
		GetGame().GetObjectsAtPosition3D(pos, HIDE_RADIUS, objects, proxies);

		Object best = null;
		float bestDist = HIDE_RADIUS + 1;
		foreach (Object obj : objects)
		{
			if (!IsMapObject(obj) || !Matches(obj, want, byModel))
				continue;
			float dist = vector.Distance(obj.GetPosition(), pos);
			if (dist < bestDist)
			{
				best = obj;
				bestDist = dist;
			}
		}
		if (!best)
			return false;

		HideObject(best, isServer);
		return true;
	}

	protected static bool Matches(Object obj, string want, bool byModel)
	{
		if (!byModel)
		{
			string type = obj.GetType();
			type.ToLower();
			if (type == want)
				return true;
		}
		// Объект без конфиг-класса (или удалённый по пути .p3d) сверяем по имени модели.
		string shape = obj.GetShapeName();
		shape.ToLower();
		shape = ModelBaseName(shape);
		if (shape == want)
			return true;
		if (byModel)
			return false;
		// «BushHard_b_naked_2s» -> модель «b_naked_2s».
		int sep = want.IndexOf("_");
		if (sep > 0 && sep < want.Length() - 1)
			return shape == want.Substring(sep + 1, want.Length() - sep - 1);
		return false;
	}

	// В лог: что стоит рядом с точкой, где не нашёлся объект для скрытия.
	protected static void ReportMissing(string name, vector pos)
	{
		array<Object> objects = new array<Object>();
		array<CargoBase> proxies = new array<CargoBase>();
		GetGame().GetObjectsAtPosition3D(pos, 3, objects, proxies);
		string near = "";
		int shown = 0;
		foreach (Object obj : objects)
		{
			if (shown >= 6)
				break;
			float dist = vector.Distance(obj.GetPosition(), pos);
			string shape = obj.GetShapeName();
			near = near + " [" + obj.GetType() + " | " + shape + " | " + dist.ToString() + " м | карта=" + IsMapObject(obj).ToString() + "]";
			shown++;
		}
		Print(LOG_TAG + "  не найден для скрытия: " + name + " в " + pos.ToString() + ", рядом " + objects.Count().ToString() + ":" + near);
	}

	// Тот же признак объекта карты, что в CF_ObjectManager.IsMapObject.
	protected static bool IsMapObject(Object obj)
	{
		if (!obj)
			return false;
		if (obj.GetType() == "" && obj.Type() == Object)
			return true;
		return obj.IsKindOf("House") || obj.IsTree() || obj.IsBush() || obj.IsRock() || obj.IsInherited(Static);
	}

	// Приём из Dabs Framework (SuppressedObject), на котором работает DayZ Editor Loader:
	// сдвиг объекта через SetTransform/SetPosition оставляет на клиенте «призрак»,
	// поэтому объект сжимаем, делаем невидимым и убираем ему физику.
	protected static void HideObject(Object obj, bool isServer)
	{
		vector origin = obj.GetPosition();
		bool affectsPath = obj.CanAffectPathgraph();
		vector minMax[2];
		float radius = obj.ClippingInfo(minMax);

		// SetScale(0) рушит кучу, крошечный масштаб безопасен.
		obj.SetScale(0.000001);
		obj.ClearFlags(obj.GetFlags(), true);
		obj.ClearEventMask(obj.GetEventMask());
		obj.SetEventMask(EntityEvent.NOTVISIBLE);
		obj.Update();
		dBodyDestroy(obj);

		if (isServer && affectsPath)
		{
			vector r = Vector(radius, radius, radius);
			GetGame().UpdatePathgraphRegion(origin - r, origin + r);
		}
	}

	// «dz\plants\tree\t_betulapendula_1f.p3d» -> «t_betulapendula_1f».
	static string ModelBaseName(string path)
	{
		string s = path;
		s.Replace("/", "\\");
		int slash = s.LastIndexOf("\\");
		if (slash >= 0)
			s = s.Substring(slash + 1, s.Length() - slash - 1);
		int dot = s.LastIndexOf(".");
		if (dot > 0)
			s = s.Substring(0, dot);
		return s;
	}
}

// Один файл маппинга: разобранные строки и итог применения.
class DZM_File
{
	string m_Entry;
	ref array<ref DZM_Line> m_Hides = new array<ref DZM_Line>();
	ref array<ref DZM_Line> m_Spawns = new array<ref DZM_Line>();
	int m_Bad;
	int m_Repeats;
	int m_Hidden;
	int m_Models;
	int m_Classes;
	int m_Failed;
	// Класс/модель, которые не создались -> сколько раз.
	ref map<string, int> m_FailedNames = new map<string, int>();

	void DZM_File(string entry)
	{
		m_Entry = entry;
	}

	void AddFailed(string name)
	{
		m_Failed++;
		m_FailedNames.Set(name, m_FailedNames.Get(name) + 1);
	}

	void Report(bool isServer)
	{
		string side = "клиент";
		if (isServer)
			side = "сервер";
		string msg = DZM_Mapping.LOG_TAG + m_Entry + " (" + side + "): скрыто " + m_Hidden.ToString() + "/" + m_Hides.Count().ToString();
		msg = msg + ", моделей " + m_Models.ToString() + ", по классу " + m_Classes.ToString();
		msg = msg + ", не создано " + m_Failed.ToString() + ", повторов " + m_Repeats.ToString() + ", битых строк " + m_Bad.ToString();
		Print(msg);
		foreach (string name, int n : m_FailedNames)
			Print(DZM_Mapping.LOG_TAG + "  не создан: " + name + " x" + n.ToString());
	}
}

// Одна разобранная строка файла .map.
class DZM_Line
{
	string m_Name;
	vector m_Pos;
	vector m_Ypr;
	float m_Scale = 1;
	bool m_Hide;
	bool m_IsModel;

	static bool IsModelPath(string name)
	{
		return name.Contains("\\") || name.Contains("/") || name.Contains(".p3d");
	}

	static DZM_Line Parse(string raw)
	{
		string line = raw;
		line.Replace("\r", "");
		line = line.Trim();
		if (line == "" || line.IndexOf("//") == 0)
			return null;

		TStringArray parts = new TStringArray();
		line.Split("|", parts);
		if (parts.Count() < 3)
			return null;

		DZM_Line res = new DZM_Line();
		string name = parts[0].Trim();
		if (name.IndexOf("-") == 0)
		{
			res.m_Hide = true;
			name = name.Substring(1, name.Length() - 1);
			name = name.Trim();
		}
		if (name == "")
			return null;

		res.m_Name = name;
		res.m_IsModel = IsModelPath(name);
		string posText = parts[1].Trim();
		string yprText = parts[2].Trim();
		res.m_Pos = posText.ToVector();
		res.m_Ypr = yprText.ToVector();
		if (parts.Count() > 3)
		{
			string scaleText = parts[3].Trim();
			float scale = scaleText.ToFloat();
			if (scale > 0)
				res.m_Scale = scale;
		}
		return res;
	}
}
