// Боссы, точки, общие настройки и состояние рестарта (ТЗ, разделы 4–5).
// bosses.json по умолчанию лежит в серверном PBO (VB_AnomaliesBosses_Server/defaults/bosses.json, генерирует tools/ab_bosses.py)
// и копируется в профиль, если там файла нет.

// Нормализация после загрузки: см. комментарий в VB_AB_Config.c (ключи без значения приходят нулями).

class VB_AB_WeightedClass
{
	string Classname;
	float Weight;				// нет или 0 — 1
};

// Строка «всегда или с шансом» (аномалии босса, трофеи)
class VB_AB_ChanceLine
{
	string Classname;
	ref array<ref VB_AB_WeightedClass> OneOf;	// вместо Classname: одна из нескольких по весам
	ref array<int> Count;						// [от, до], по умолчанию 1
	float Chance;								// %; нет, 0 или 100 — всегда

	void VB_Norm()
	{
		if (Chance <= 0)
			Chance = 100;
		if (OneOf)
		{
			foreach (VB_AB_WeightedClass w : OneOf)
			{
				if (w && w.Weight <= 0)
					w.Weight = 1;
			}
		}
	}

	// Класс по Classname или по весам OneOf. Пусто — ничего не выпало.
	string PickClass()
	{
		if (Classname != "")
			return Classname;
		if (!OneOf || OneOf.Count() == 0)
			return "";
		float total = 0;
		foreach (VB_AB_WeightedClass w : OneOf)
			total += Math.Max(0, w.Weight);
		float x = Math.RandomFloat(0, total);
		foreach (VB_AB_WeightedClass w2 : OneOf)
		{
			x -= Math.Max(0, w2.Weight);
			if (x <= 0)
				return w2.Classname;
		}
		return OneOf[OneOf.Count() - 1].Classname;
	}

	int PickCount()
	{
		if (!Count || Count.Count() == 0)
			return 1;
		if (Count.Count() == 1)
			return Math.Max(0, Count[0]);
		return Math.RandomIntInclusive(Math.Max(0, Count[0]), Math.Max(Count[0], Count[1]));
	}

	bool RollChance()
	{
		return Chance >= 100 || Math.RandomFloat(0, 100) < Chance;
	}
};

class VB_AB_AnomalyLine: VB_AB_ChanceLine
{
	string Place;				// Center, Ring, Edge, Any; нет — Any
	int MaxInField;				// 0 — без ограничения

	override void VB_Norm()
	{
		super.VB_Norm();
		if (Place != "Center" && Place != "Ring" && Place != "Edge")
			Place = "Any";
	}
};

class VB_AB_PhaseCfg
{
	float Health;				// доля здоровья, на которой срабатывает (0.6, 0.3)
	ref array<ref VB_AB_AnomalyLine> Anomalies;
};

class VB_AB_FieldCfg
{
	float Radius;				// нет или 0 — 25
	int MaxAnomalies;			// нет или 0 — 12
	float MinGap;				// нет или 0 — 4
	float KeepFromPlayers;		// нет или 0 — 3
	float LeashRadius;			// нет или 0 — радиус поля + 15
	float AfterDeathSec;		// нет или 0 — 120
	// Щит (ТЗ 8.1, 8.2): внутри своей аномалии босс получает меньше урона и лечится; на пороге фазы — неуязвим.
	// Отрицательное значение выключает.
	float ShieldDamage;			// доля урона внутри своей аномалии; нет или 0 — 0,4
	float ShieldHealPct;		// лечение внутри своей аномалии, % здоровья в секунду; нет или 0 — 1
	float PhaseShieldSec;		// неуязвимость на пороге фазы, с; нет или 0 — 3

	void VB_Norm()
	{
		if (ShieldDamage == 0)
			ShieldDamage = 0.4;
		if (ShieldHealPct == 0)
			ShieldHealPct = 1;
		if (PhaseShieldSec == 0)
			PhaseShieldSec = 3;
		if (Radius <= 0)
			Radius = 25;
		if (MaxAnomalies <= 0)
			MaxAnomalies = 12;
		if (MinGap <= 0)
			MinGap = 4;
		if (KeepFromPlayers <= 0)
			KeepFromPlayers = 3;
		if (LeashRadius <= 0)
			LeashRadius = Radius + 15;
		if (AfterDeathSec <= 0)
			AfterDeathSec = 120;
	}
};

class VB_AB_AbilityCfg
{
	string Type;
	float Cooldown;				// нет или 0 — 20
	float Range;				// нет или 0 — 15
	float Damage;
	float DurationSec;
	float Chance;				// %; нет или 0 — всегда
	ref array<string> RequiresAnomaly;
	string Anomaly;

	void VB_Norm()
	{
		if (Cooldown <= 0)
			Cooldown = 20;
		if (Range <= 0)
			Range = 15;
		if (Chance <= 0)
			Chance = 100;
	}
};

class VB_AB_ArtifactsCfg: VB_AB_LootTable
{
	int MaxInField;				// нет или 0 — 6
	float NearAnomalyShare;		// доля у аномалий; нет или 0 — 0,5

	void VB_NormArt()
	{
		VB_NormTable();
		if (MaxInField <= 0)
			MaxInField = 6;
		if (NearAnomalyShare <= 0)
			NearAnomalyShare = 0.5;
	}
};

class VB_AB_TrophyCfg
{
	float CutTimeSec;			// нет или 0 — 15
	float ToolDamage;			// нет или 0 — 15
	float CorpseLifetimeMin;	// нет или 0 — 15
	ref array<ref VB_AB_ChanceLine> Items;

	void VB_Norm()
	{
		if (CutTimeSec <= 0)
			CutTimeSec = 15;
		if (ToolDamage <= 0)
			ToolDamage = 15;
		if (CorpseLifetimeMin <= 0)
			CorpseLifetimeMin = 15;
		if (Items)
		{
			foreach (VB_AB_ChanceLine l : Items)
			{
				if (l)
					l.VB_Norm();
			}
		}
	}
};

class VB_AB_BossCfg
{
	string Id;
	string Name;
	string Classname;
	int Tier;					// нет или 0 — 1
	float Health;				// нет или 0 — 3000
	float Scale;				// 0 — из config.cpp (vbABScale)
	float MeleeMul;				// удар босса × (здоровье и кровь игрока), нет или 0 — 1 (как зомби)
	ref VB_AB_FieldCfg Field;
	ref array<ref VB_AB_AnomalyLine> Anomalies;
	ref array<ref VB_AB_PhaseCfg> Phases;
	ref array<ref VB_AB_AbilityCfg> Abilities;
	ref VB_AB_ArtifactsCfg Artifacts;
	ref VB_AB_TrophyCfg Trophy;

	void VB_Norm()
	{
		if (Tier <= 0)
			Tier = 1;
		if (Health <= 0)
			Health = 3000;
		if (MeleeMul <= 0)
			MeleeMul = 1;
		if (!Field)
			Field = new VB_AB_FieldCfg;
		Field.VB_Norm();
		if (Anomalies)
		{
			foreach (VB_AB_AnomalyLine l : Anomalies)
			{
				if (l)
					l.VB_Norm();
			}
		}
		if (Phases)
		{
			foreach (VB_AB_PhaseCfg ph : Phases)
			{
				if (!ph || !ph.Anomalies)
					continue;
				foreach (VB_AB_AnomalyLine pl : ph.Anomalies)
				{
					if (pl)
						pl.VB_Norm();
				}
			}
		}
		if (Abilities)
		{
			foreach (VB_AB_AbilityCfg ab : Abilities)
			{
				if (ab)
					ab.VB_Norm();
			}
		}
		if (Artifacts)
			Artifacts.VB_NormArt();
		if (!Trophy)
			Trophy = new VB_AB_TrophyCfg;
		Trophy.VB_Norm();
	}
};

class VB_AB_BossesFile
{
	ref array<ref VB_AB_BossCfg> Bosses;
};

class VB_AB_PointCfg
{
	int Id;
	ref array<float> Position;
	ref array<string> Bosses;
	float Weight;				// нет или 0 — 1
	int Disabled;				// 1 — точка выключена

	void VB_Norm()
	{
		if (Weight <= 0)
			Weight = 1;
	}

	vector GetPos()
	{
		if (!Position || Position.Count() < 3)
			return vector.Zero;
		return Vector(Position[0], Position[1], Position[2]);
	}

	// Редактор: позиция в формате points.json (три числа, до сантиметра)
	void SetPos(vector v)
	{
		if (!Position)
			Position = new array<float>;
		Position.Clear();
		Position.Insert(Math.Round(v[0] * 100) / 100);
		Position.Insert(Math.Round(v[1] * 100) / 100);
		Position.Insert(Math.Round(v[2] * 100) / 100);
	}

	bool HasBoss(string id)
	{
		return Bosses && Bosses.Find(id) != -1;
	}
};

class VB_AB_PointsFile
{
	ref array<ref VB_AB_PointCfg> Points;
};

class VB_AB_SettingsFile
{
	ref array<string> Admins;
	int ActivePointsPerRestart = 4;			// нет или 0 — 4
	int MaxAwakeBosses = 3;					// нет или 0 — 3
	float MinPointDistance = 1000;			// нет или 0 — 1000
	float WakeRadius = 300;					// нет или 0 — 300
	float SleepAfterMin = 10;				// нет или 0 — 10
	float RespawnMin = 0;					// 0 — до следующего рестарта
	int MaxAnomaliesTotal = 80;				// нет или 0 — 80
	int MaxFieldArtifactsPerRestart = 40;	// нет или 0 — 40
	float ResetAfterSec = 60;				// все ушли из поля и поводка столько секунд — поле сворачивается, босс лечится
	int DebugLog;								// 1 — подробный лог

	void VB_Norm()
	{
		VB_AB_Log.s_Debug = DebugLog != 0;
		if (!Admins)
			Admins = new array<string>;
		if (ActivePointsPerRestart <= 0)
			ActivePointsPerRestart = 4;
		if (MaxAwakeBosses <= 0)
			MaxAwakeBosses = 3;
		if (MinPointDistance <= 0)
			MinPointDistance = 1000;
		if (WakeRadius <= 0)
			WakeRadius = 300;
		if (SleepAfterMin <= 0)
			SleepAfterMin = 10;
		if (MaxAnomaliesTotal <= 0)
			MaxAnomaliesTotal = 80;
		if (MaxFieldArtifactsPerRestart <= 0)
			MaxFieldArtifactsPerRestart = 40;
		if (ResetAfterSec <= 0)
			ResetAfterSec = 60;
	}
};

class VB_AB_StateFile
{
	ref array<int> LastPoints;
};

class VB_AB_BossData
{
	static const string DIR = "$profile:VB_Mods/VB_AnomaliesBosses/";
	static const string DEFAULT_BOSSES = "VB_AnomaliesBosses_Server/defaults/bosses.json";

	ref VB_AB_SettingsFile m_Settings;
	ref map<string, ref VB_AB_BossCfg> m_Bosses = new map<string, ref VB_AB_BossCfg>;
	ref array<ref VB_AB_PointCfg> m_Points = new array<ref VB_AB_PointCfg>;
	ref VB_AB_StateFile m_State;

	void Load()
	{
		if (!FileExist("$profile:VB_Mods"))
			MakeDirectory("$profile:VB_Mods");
		if (!FileExist(DIR))
			MakeDirectory(DIR);
		LoadSettings();
		LoadBosses();
		VB_AB_Log.MissingSummary();
		LoadPoints();
		LoadState();
	}

	protected void LoadSettings()
	{
		string path = DIR + "settings.json";
		string err;
		if (FileExist(path) && !JsonFileLoader<VB_AB_SettingsFile>.LoadFile(path, m_Settings, err))
			VB_AB_Log.Warn("settings.json не прочитан: " + err);
		if (!m_Settings)
		{
			m_Settings = new VB_AB_SettingsFile;
			m_Settings.Admins = new array<string>;
			m_Settings.Admins.Insert("76561200715668795");		// админ по умолчанию (пользователь, 2026-11-13)
			if (!FileExist(path))
				JsonFileLoader<VB_AB_SettingsFile>.SaveFile(path, m_Settings, err);
		}
		m_Settings.VB_Norm();
	}

	protected void LoadBosses()
	{
		string path = DIR + "bosses.json";
		if (!FileExist(path) && FileExist(DEFAULT_BOSSES))
			CopyFile(DEFAULT_BOSSES, path);
		VB_AB_BossesFile f;
		string err;
		if (FileExist(path) && !JsonFileLoader<VB_AB_BossesFile>.LoadFile(path, f, err))
			VB_AB_Log.Warn("bosses.json не прочитан: " + err);
		if (!f || !f.Bosses)
			return;
		foreach (VB_AB_BossCfg b : f.Bosses)
		{
			if (!b || b.Id == "")
				continue;
			if (!VB_AB_Cfg.ClassExists(b.Classname))
			{
				VB_AB_Log.Warn("bosses.json/" + b.Id + ": класс " + b.Classname + " не найден, босс пропущен");
				continue;
			}
			b.VB_Norm();
			ValidateLines(b.Anomalies, "bosses.json/" + b.Id);
			if (b.Phases)
			{
				foreach (VB_AB_PhaseCfg ph : b.Phases)
					ValidateLines(ph.Anomalies, "bosses.json/" + b.Id + "/фаза");
			}
			if (b.Artifacts)
				VB_AB_Cfg.Get().ValidateLoot(b.Artifacts, "bosses.json/" + b.Id + "/Artifacts", false);
			if (b.Trophy)
				ValidateChance(b.Trophy.Items, "bosses.json/" + b.Id + "/Trophy");
			m_Bosses.Set(b.Id, b);
		}
		VB_AB_Log.Info("боссов: " + m_Bosses.Count());
	}

	protected void ValidateLines(array<ref VB_AB_AnomalyLine> lines, string where)
	{
		if (!lines)
			return;
		for (int i = lines.Count() - 1; i >= 0; i--)
		{
			VB_AB_AnomalyLine l = lines[i];
			if (!l || !ValidateLineClasses(l, where))
				lines.Remove(i);
		}
	}

	protected void ValidateChance(array<ref VB_AB_ChanceLine> lines, string where)
	{
		if (!lines)
			return;
		for (int i = lines.Count() - 1; i >= 0; i--)
		{
			VB_AB_ChanceLine l = lines[i];
			if (!l || !ValidateLineClasses(l, where))
				lines.Remove(i);
		}
	}

	// false — в строке не осталось ни одного существующего класса
	protected bool ValidateLineClasses(VB_AB_ChanceLine l, string where)
	{
		if (l.Classname != "")
		{
			if (VB_AB_Cfg.ClassExists(l.Classname))
				return true;
			VB_AB_Log.Missing(where, l.Classname);
			return false;
		}
		if (!l.OneOf)
			return false;
		for (int i = l.OneOf.Count() - 1; i >= 0; i--)
		{
			if (!l.OneOf[i] || !VB_AB_Cfg.ClassExists(l.OneOf[i].Classname))
			{
				VB_AB_Log.Warn(where + ": в OneOf пропущен неизвестный класс");
				l.OneOf.Remove(i);
			}
		}
		return l.OneOf.Count() > 0;
	}

	protected void LoadPoints()
	{
		string path = DIR + "points.json";
		VB_AB_PointsFile f;
		string err;
		if (FileExist(path) && !JsonFileLoader<VB_AB_PointsFile>.LoadFile(path, f, err))
			VB_AB_Log.Warn("points.json не прочитан: " + err);
		if (!f)
		{
			f = new VB_AB_PointsFile;
			f.Points = new array<ref VB_AB_PointCfg>;
			if (!FileExist(path))
				JsonFileLoader<VB_AB_PointsFile>.SaveFile(path, f, err);
		}
		if (f.Points)
		{
			foreach (VB_AB_PointCfg p : f.Points)
			{
				if (p && p.Position && p.Position.Count() >= 3)
				{
					p.VB_Norm();
					m_Points.Insert(p);
				}
			}
		}
		VB_AB_Log.Info("точек: " + m_Points.Count());
	}

	void SavePoints()
	{
		VB_AB_PointsFile f = new VB_AB_PointsFile;
		f.Points = m_Points;
		string err;
		JsonFileLoader<VB_AB_PointsFile>.SaveFile(DIR + "points.json", f, err);
	}

	protected void LoadState()
	{
		string path = DIR + "state.json";
		string err;
		if (FileExist(path) && !JsonFileLoader<VB_AB_StateFile>.LoadFile(path, m_State, err))
			VB_AB_Log.Warn("state.json не прочитан: " + err);
		if (!m_State)
			m_State = new VB_AB_StateFile;
		if (!m_State.LastPoints)
			m_State.LastPoints = new array<int>;
	}

	void SaveState()
	{
		string err;
		JsonFileLoader<VB_AB_StateFile>.SaveFile(DIR + "state.json", m_State, err);
	}

	VB_AB_BossCfg GetBoss(string id)
	{
		return m_Bosses.Get(id);
	}

	bool IsAdmin(string plainId)
	{
		return m_Settings && m_Settings.Admins.Find(plainId) != -1;
	}

	// ---------------------------------------------------------------- редактор (VB_AB_EditorServer.c)
	// Удалённые точки живут до рестарта: на них ещё может смотреть VB_AB_PointRuntime (указатель без ref).
	protected ref array<ref VB_AB_PointCfg> m_RemovedPoints = new array<ref VB_AB_PointCfg>;

	VB_AB_PointCfg FindPoint(int id)
	{
		foreach (VB_AB_PointCfg p : m_Points)
		{
			if (p && p.Id == id)
				return p;
		}
		return null;
	}

	// Новый номер: больше всех живых и удалённых в этом запуске (номер не переиспользуется до рестарта)
	int NextPointId()
	{
		int top = 0;
		foreach (VB_AB_PointCfg p : m_Points)
		{
			if (p && p.Id > top)
				top = p.Id;
		}
		foreach (VB_AB_PointCfg r : m_RemovedPoints)
		{
			if (r && r.Id > top)
				top = r.Id;
		}
		return top + 1;
	}

	// Новая точка с одним боссом (bossId пусто — без босса), сразу в points.json
	VB_AB_PointCfg AddPoint(vector pos, string bossId)
	{
		VB_AB_PointCfg p = new VB_AB_PointCfg;
		p.Id = NextPointId();
		p.SetPos(pos);
		p.Bosses = new array<string>;
		if (bossId != "")
			p.Bosses.Insert(bossId);
		p.Weight = 1;
		m_Points.Insert(p);
		SavePoints();
		return p;
	}

	// Убрать точку из points.json. Рестартовую часть (босс, PointRuntime) снимает VB_AB_Server.DropPoint — раньше этого.
	bool RemovePoint(int id)
	{
		VB_AB_PointCfg p = FindPoint(id);
		if (!p)
			return false;
		m_RemovedPoints.Insert(p);
		m_Points.RemoveItem(p);
		SavePoints();
		return true;
	}

	void SaveSettings()
	{
		string err;
		if (!JsonFileLoader<VB_AB_SettingsFile>.SaveFile(DIR + "settings.json", m_Settings, err))
			VB_AB_Log.Warn("settings.json не записан: " + err);
	}

	// settings.json с диска (админа можно дописать без рестарта)
	void ReloadSettings()
	{
		m_Settings = null;
		LoadSettings();
	}
};
