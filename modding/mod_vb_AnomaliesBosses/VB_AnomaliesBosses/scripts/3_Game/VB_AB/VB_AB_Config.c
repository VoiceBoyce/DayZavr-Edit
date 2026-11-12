// Конфиги мода в профиле сервера: $profile:VB_Mods/VB_AnomaliesBosses/*.json (ТЗ, раздел 5).
// Читает только сервер. Если файла нет — создаётся с настройками по умолчанию.
// Неизвестные классы предметов пропускаются с предупреждением в логе.
//
// ВАЖНО: JsonFileLoader создаёт объекты сам и НЕ применяет начальные значения полей из кода к ключам,
// которых нет в файле, — там остаётся 0 (проверено на стенде 2026-11-12). Поэтому после загрузки каждый
// класс проходит VB_Norm(): где 0 не имеет смысла, он значит «не задано» и заменяется значением по умолчанию.

class VB_AB_LootItem
{
	string Classname;
	float Chance;					// вес в рулетке таблицы; нет или 0 — 1
	int MinTier;					// уровень детектора; нет или 0 — 1
	int Visible;					// 1 — лежит на виду, детектор не нужен
	ref array<string> Detectors;	// если задан — находят только эти детекторы (уровень не важен)
	int QuantityMin;				// 0 — количество по умолчанию
	int QuantityMax;

	void VB_Norm()
	{
		if (Chance <= 0)
			Chance = 1;
		if (MinTier <= 0)
			MinTier = 1;
	}
};

class VB_AB_LootTable
{
	int Rolls;						// сколько раз тянем; нет или 0 — 1
	float Nothing;					// вес варианта «ничего»
	ref array<ref VB_AB_LootItem> Items;

	void VB_NormTable()
	{
		if (Rolls <= 0)
			Rolls = 1;
		if (!Items)
			return;
		foreach (VB_AB_LootItem it : Items)
		{
			if (it)
				it.VB_Norm();
		}
	}
};

class VB_AB_AnomalyCfg
{
	string Classname;
	float DamageMult;				// множитель урона поверх config.cpp; нет или 0 — 1
	int MaxInField;					// 0 — без ограничения
	float PushRadius;				// Атом: радиус толчка, 0 — из config.cpp
	ref VB_AB_LootTable Loot;		// материал аномалии (ТЗ 2.6)

	void VB_Norm()
	{
		if (DamageMult <= 0)
			DamageMult = 1;
		if (Loot)
			Loot.VB_NormTable();
	}
};

class VB_AB_AnomaliesFile
{
	ref array<ref VB_AB_AnomalyCfg> Anomalies;
};

class VB_AB_DetectorCfg
{
	string Classname;
	int Tier = 1;					// нет или 0 — 1
	float Radius = 10;				// нет или 0 — 10
	float SpawnDistance = 2;		// нет или 0 — 20 % радиуса
	string Signal = "Own";			// Own — свой детектор мода, Generic — чужой (писк от мода)

	void VB_Norm()
	{
		if (Tier <= 0)
			Tier = 1;
		if (Radius <= 0)
			Radius = 10;
		if (SpawnDistance <= 0)
			SpawnDistance = Radius * 0.2;
		if (Signal == "")
			Signal = "Own";
	}
};

class VB_AB_DetectorsFile
{
	ref array<ref VB_AB_DetectorCfg> Detectors;
};

class VB_AB_Cfg
{
	static const string DIR = "$profile:VB_Mods/VB_AnomaliesBosses/";
	protected static ref VB_AB_Cfg s_Instance;

	ref map<string, ref VB_AB_AnomalyCfg> m_Anomalies = new map<string, ref VB_AB_AnomalyCfg>;
	ref map<string, ref VB_AB_DetectorCfg> m_Detectors = new map<string, ref VB_AB_DetectorCfg>;
	protected ref VB_AB_DetectorCfg m_DefaultDetector = new VB_AB_DetectorCfg;

	static VB_AB_Cfg Get()
	{
		if (!s_Instance)
		{
			s_Instance = new VB_AB_Cfg;
			s_Instance.Load();
			VB_AB_Log.MissingSummary();
		}
		return s_Instance;
	}

	// «Перечитать конфиги» (редактор)
	static void Reload()
	{
		s_Instance = null;
		Get();
	}

	VB_AB_AnomalyCfg GetAnomaly(string type)
	{
		return m_Anomalies.Get(type);
	}

	VB_AB_DetectorCfg GetDetector(string type)
	{
		VB_AB_DetectorCfg d = m_Detectors.Get(type);
		if (d)
			return d;
		return m_DefaultDetector;
	}

	bool IsDetector(string type)
	{
		return m_Detectors.Contains(type);
	}

	// ---------------------------------------------------------------- загрузка
	protected void Load()
	{
		if (!FileExist("$profile:VB_Mods"))
			MakeDirectory("$profile:VB_Mods");
		if (!FileExist(DIR))
			MakeDirectory(DIR);
		LoadDetectors();
		LoadAnomalies();
	}

	protected void LoadDetectors()
	{
		string path = DIR + "detectors.json";
		VB_AB_DetectorsFile f;
		string err;
		if (FileExist(path))
		{
			if (!JsonFileLoader<VB_AB_DetectorsFile>.LoadFile(path, f, err))
				VB_AB_Log.Warn("detectors.json не прочитан: " + err);
		}
		if (!f || !f.Detectors)
		{
			f = DefaultDetectors();
			if (!FileExist(path))
				JsonFileLoader<VB_AB_DetectorsFile>.SaveFile(path, f, err);
		}
		foreach (VB_AB_DetectorCfg d : f.Detectors)
		{
			if (!d || d.Classname == "")
				continue;
			if (!ClassExists(d.Classname))
			{
				VB_AB_Log.Info("детектор " + d.Classname + " на сервере не найден, пропущен");
				continue;
			}
			d.VB_Norm();
			m_Detectors.Set(d.Classname, d);
		}
		VB_AB_Log.Info("детекторов: " + m_Detectors.Count());
	}

	protected void LoadAnomalies()
	{
		string path = DIR + "anomalies.json";
		VB_AB_AnomaliesFile f;
		string err;
		if (FileExist(path))
		{
			if (!JsonFileLoader<VB_AB_AnomaliesFile>.LoadFile(path, f, err))
				VB_AB_Log.Warn("anomalies.json не прочитан: " + err);
		}
		if (!f || !f.Anomalies)
		{
			f = DefaultAnomalies();
			if (!FileExist(path))
				JsonFileLoader<VB_AB_AnomaliesFile>.SaveFile(path, f, err);
		}
		foreach (VB_AB_AnomalyCfg a : f.Anomalies)
		{
			if (!a || a.Classname == "")
				continue;
			if (!ClassExists(a.Classname))
			{
				VB_AB_Log.Warn("аномалия " + a.Classname + " не найдена, пропущена");
				continue;
			}
			a.VB_Norm();
			// материалы аномалий — только детектором (ТЗ 2.6)
			ValidateLoot(a.Loot, "anomalies.json/" + a.Classname, true);
			m_Anomalies.Set(a.Classname, a);
		}
		VB_AB_Log.Info("аномалий в конфиге: " + m_Anomalies.Count());
	}

	// Нормализует таблицу и убирает несуществующие классы. detectorOnly — «на виду» запрещено (материалы).
	void ValidateLoot(VB_AB_LootTable t, string where, bool detectorOnly)
	{
		if (!t)
			return;
		t.VB_NormTable();
		if (!t.Items)
			return;
		for (int i = t.Items.Count() - 1; i >= 0; i--)
		{
			VB_AB_LootItem it = t.Items[i];
			if (!it || !ClassExists(it.Classname))
			{
				string name = "";
				if (it)
					name = it.Classname;
				VB_AB_Log.Missing(where, name);
				t.Items.Remove(i);
				continue;
			}
			if (detectorOnly && it.Visible != 0)
			{
				VB_AB_Log.Warn(where + ": " + it.Classname + " Visible запрещён (материалы только детектором), снят");
				it.Visible = 0;
			}
		}
	}

	static bool ClassExists(string cls)
	{
		if (cls == "")
			return false;
		return GetGame().ConfigIsExisting("CfgVehicles " + cls) || GetGame().ConfigIsExisting("CfgWeapons " + cls) || GetGame().ConfigIsExisting("CfgMagazines " + cls);
	}

	// ---------------------------------------------------------------- по умолчанию
	protected static VB_AB_DetectorsFile DefaultDetectors()
	{
		VB_AB_DetectorsFile f = new VB_AB_DetectorsFile;
		f.Detectors = new array<ref VB_AB_DetectorCfg>;
		f.Detectors.Insert(MakeDetector("VB_AB_Detector_Otklik", 1, 10, 2, "Own"));
		f.Detectors.Insert(MakeDetector("VB_AB_Detector_Oko", 1, 12, 2.4, "Own"));
		f.Detectors.Insert(MakeDetector("VB_AB_Detector_Medved", 2, 15, 3, "Own"));
		f.Detectors.Insert(MakeDetector("VB_AB_Detector_Svarog", 3, 20, 4, "Own"));
		f.Detectors.Insert(MakeDetector("VB_AB_Detector_Veles", 3, 25, 5, "Own"));
		// детекторы OOTS, если они есть на сервере (на остальных строки пропускаются)
		f.Detectors.Insert(MakeDetector("Anomaly_Detector_Otklik", 1, 10, 2, "Generic"));
		f.Detectors.Insert(MakeDetector("Anomaly_Detector_Oko", 1, 12, 2.4, "Generic"));
		f.Detectors.Insert(MakeDetector("Anomaly_Detector_Medved", 2, 15, 3, "Generic"));
		f.Detectors.Insert(MakeDetector("Anomaly_Detector_Svarog", 3, 20, 4, "Generic"));
		f.Detectors.Insert(MakeDetector("Anomaly_Detector_Veles", 3, 25, 5, "Generic"));
		return f;
	}

	protected static VB_AB_DetectorCfg MakeDetector(string cls, int tier, float radius, float spawn, string signal)
	{
		VB_AB_DetectorCfg d = new VB_AB_DetectorCfg;
		d.Classname = cls;
		d.Tier = tier;
		d.Radius = radius;
		d.SpawnDistance = spawn;
		d.Signal = signal;
		return d;
	}

	// По умолчанию каждая аномалия мода даёт свой материал VB_AB_Mat_<имя> (1–3 шт., уровень детектора 1).
	protected static VB_AB_AnomaliesFile DefaultAnomalies()
	{
		VB_AB_AnomaliesFile f = new VB_AB_AnomaliesFile;
		f.Anomalies = new array<ref VB_AB_AnomalyCfg>;
		int n = GetGame().ConfigGetChildrenCount("CfgVehicles");
		string cls;
		for (int i = 0; i < n; i++)
		{
			GetGame().ConfigGetChildName("CfgVehicles", i, cls);
			if (cls.IndexOf("VB_AB_Anomaly_") != 0)
				continue;
			if (GetGame().ConfigGetInt("CfgVehicles " + cls + " scope") != 2)
				continue;
			VB_AB_AnomalyCfg a = new VB_AB_AnomalyCfg;
			a.Classname = cls;
			a.Loot = new VB_AB_LootTable;
			a.Loot.Rolls = 1;
			a.Loot.Nothing = 30;
			a.Loot.Items = new array<ref VB_AB_LootItem>;
			VB_AB_LootItem it = new VB_AB_LootItem;
			string mat = cls;
			mat.Replace("VB_AB_Anomaly_", "VB_AB_Mat_");
			it.Classname = mat;
			it.Chance = 100;
			it.MinTier = 1;
			it.QuantityMin = 1;
			it.QuantityMax = 3;
			a.Loot.Items.Insert(it);
			if (cls == "VB_AB_Anomaly_Atom")
			{
				a.MaxInField = 2;
				a.PushRadius = 8;
			}
			f.Anomalies.Insert(a);
		}
		return f;
	}
};
