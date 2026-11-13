// Сервер боссов (ТЗ 3): выбор точек на рестарт, пробуждение при подходе игрока, уход без игроков, повтор.
// Запускается из MissionServer.OnInit (5_Mission/VB_AB/VB_AB_Mission.c).

class VB_AB_PointRuntime
{
	VB_AB_PointCfg m_Cfg;
	VB_AB_BossBase m_Boss;
	float m_EmptySec;
	float m_RespawnAt;
	bool m_Done;

	void VB_AB_PointRuntime(VB_AB_PointCfg cfg)
	{
		m_Cfg = cfg;
	}

	vector GetPos()
	{
		return m_Cfg.GetPos();
	}

	bool IsAwake()
	{
		return m_Boss && m_Boss.IsAlive();
	}

	void Update(float now, float dt)
	{
		VB_AB_SettingsFile st = VB_AB_Server.Settings();
		if (m_Boss)
		{
			if (!m_Boss.IsAlive())
				return;
			if (VB_AB_Server.AnyPlayerNear(m_Boss.GetPosition(), st.WakeRadius) || VB_AB_Server.AnyPlayerNear(GetPos(), st.WakeRadius))
				m_EmptySec = 0;
			else
			{
				m_EmptySec += dt;
				if (m_EmptySec >= st.SleepAfterMin * 60)
				{
					VB_AB_Log.Info("босс " + m_Boss.VB_GetId() + " на точке " + m_Cfg.Id + " ушёл: рядом никого " + st.SleepAfterMin + " мин");
					m_Boss.VB_Despawn();
					m_Boss = null;
				}
			}
			return;
		}
		if (m_Done || now < m_RespawnAt)
			return;
		if (VB_AB_Server.AwakeCount() >= st.MaxAwakeBosses)
			return;
		if (VB_AB_Server.AnyPlayerNear(GetPos(), st.WakeRadius))
			Spawn("");
	}

	// bossId пусто — случайный из списка точки
	VB_AB_BossBase Spawn(string bossId)
	{
		if (m_Boss)
			return m_Boss;
		VB_AB_BossCfg cfg = VB_AB_Server.PickBoss(m_Cfg, bossId);
		if (!cfg)
		{
			VB_AB_Log.Warn("точка " + m_Cfg.Id + ": нет подходящего босса");
			m_Done = true;
			return null;
		}
		Object o = GetGame().CreateObjectEx(cfg.Classname, GetPos(), ECE_PLACE_ON_SURFACE | ECE_INITAI | ECE_CREATEPHYSICS | ECE_NOPERSISTENCY_WORLD);
		VB_AB_BossBase b = VB_AB_BossBase.Cast(o);
		if (!b)
		{
			VB_AB_Log.Warn("класс " + cfg.Classname + " — не босс (нет VB_AB_BossBase), точка " + m_Cfg.Id);
			if (o)
				GetGame().ObjectDelete(o);
			m_Done = true;
			return null;
		}
		b.VB_Setup(cfg, this);
		m_Boss = b;
		m_EmptySec = 0;
		VB_AB_Log.Info("босс " + cfg.Id + " проснулся на точке " + m_Cfg.Id + " " + GetPos().ToString());
		return b;
	}

	void OnBossDead(VB_AB_BossBase b)
	{
		if (m_Boss != b)
			return;
		m_Boss = null;
		float respawn = VB_AB_Server.Settings().RespawnMin;
		if (respawn > 0)
			m_RespawnAt = VB_AB_Server.Now() + respawn * 60;
		else
			m_Done = true;
	}
};

class VB_AB_Server
{
	static const int TICK_MS = 1000;
	protected static ref VB_AB_Server s_Instance;

	ref VB_AB_BossData m_Data;
	ref array<ref VB_AB_PointRuntime> m_Active = new array<ref VB_AB_PointRuntime>;
	int m_ArtifactsLeft;
	protected ref array<Man> m_Players = new array<Man>;

	static void Init()
	{
		if (s_Instance)
			return;
		s_Instance = new VB_AB_Server;
		s_Instance.Start();
	}

	static VB_AB_Server Get()
	{
		return s_Instance;
	}

	static VB_AB_SettingsFile Settings()
	{
		if (!s_Instance || !s_Instance.m_Data)
		{
			VB_AB_SettingsFile def = new VB_AB_SettingsFile;
			return def;
		}
		return s_Instance.m_Data.m_Settings;
	}

	static VB_AB_BossData Data()
	{
		if (!s_Instance)
			return null;
		return s_Instance.m_Data;
	}

	static float Now()
	{
		return GetGame().GetTime() / 1000.0;
	}

	protected void Start()
	{
		VB_AB_Cfg.Get();
		m_Data = new VB_AB_BossData;
		m_Data.Load();
		m_ArtifactsLeft = m_Data.m_Settings.MaxFieldArtifactsPerRestart;
		PickPoints();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, true);
	}

	// Точки на рестарт: включённые, с боссом; не ближе MinPointDistance друг к другу; по весу;
	// прошлый рестарт по возможности не повторяется.
	protected void PickPoints()
	{
		VB_AB_SettingsFile st = m_Data.m_Settings;
		array<VB_AB_PointCfg> fresh = new array<VB_AB_PointCfg>;
		array<VB_AB_PointCfg> last = new array<VB_AB_PointCfg>;
		foreach (VB_AB_PointCfg p : m_Data.m_Points)
		{
			if (p.Disabled != 0 || !PickBoss(p, ""))
				continue;
			if (m_Data.m_State.LastPoints.Find(p.Id) != -1)
				last.Insert(p);
			else
				fresh.Insert(p);
		}
		array<VB_AB_PointCfg> picked = new array<VB_AB_PointCfg>;
		PickFrom(fresh, picked, st);
		PickFrom(last, picked, st);
		m_Data.m_State.LastPoints.Clear();
		string ids = "";
		foreach (VB_AB_PointCfg q : picked)
		{
			m_Active.Insert(new VB_AB_PointRuntime(q));
			m_Data.m_State.LastPoints.Insert(q.Id);
			ids += " " + q.Id;
		}
		m_Data.SaveState();
		VB_AB_Log.Info("точек на рестарт: " + picked.Count() + " из " + m_Data.m_Points.Count() + ":" + ids);
	}

	protected void PickFrom(array<VB_AB_PointCfg> pool, array<VB_AB_PointCfg> picked, VB_AB_SettingsFile st)
	{
		while (picked.Count() < st.ActivePointsPerRestart && pool.Count() > 0)
		{
			float total = 0;
			foreach (VB_AB_PointCfg p : pool)
				total += Math.Max(0.01, p.Weight);
			float x = Math.RandomFloat(0, total);
			int idx = pool.Count() - 1;
			for (int i = 0; i < pool.Count(); i++)
			{
				x -= Math.Max(0.01, pool[i].Weight);
				if (x <= 0)
				{
					idx = i;
					break;
				}
			}
			VB_AB_PointCfg c = pool[idx];
			pool.Remove(idx);
			bool far = true;
			foreach (VB_AB_PointCfg t : picked)
			{
				if (vector.Distance(t.GetPos(), c.GetPos()) < st.MinPointDistance)
				{
					far = false;
					break;
				}
			}
			if (far)
				picked.Insert(c);
		}
	}

	static VB_AB_BossCfg PickBoss(VB_AB_PointCfg p, string bossId)
	{
		if (!s_Instance)
			return null;
		VB_AB_BossData data = s_Instance.m_Data;
		if (bossId != "")
			return data.GetBoss(bossId);
		if (!p.Bosses)
			return null;
		array<VB_AB_BossCfg> ok = new array<VB_AB_BossCfg>;
		foreach (string id : p.Bosses)
		{
			VB_AB_BossCfg b = data.GetBoss(id);
			if (b)
				ok.Insert(b);
		}
		if (ok.Count() == 0)
			return null;
		return ok.GetRandomElement();
	}

	void Tick()
	{
		m_Players.Clear();
		GetGame().GetPlayers(m_Players);
		float now = Now();
		foreach (VB_AB_PointRuntime p : m_Active)
			p.Update(now, TICK_MS / 1000.0);
	}

	// Живые игроки в радиусе r (по горизонтали) от точки, ближайший первым
	static void PlayersNear(vector pos, float r, array<PlayerBase> res)
	{
		res.Clear();
		if (!s_Instance)
			return;
		float r2 = r * r;
		array<float> dist = new array<float>;
		foreach (Man m : s_Instance.m_Players)
		{
			PlayerBase p = PlayerBase.Cast(m);
			if (!p || !p.IsAlive())
				continue;
			vector pp = p.GetPosition();
			float dSq = (pp[0] - pos[0]) * (pp[0] - pos[0]) + (pp[2] - pos[2]) * (pp[2] - pos[2]);
			if (dSq > r2)
				continue;
			int i = 0;
			while (i < dist.Count() && dist[i] < dSq)
				i++;
			dist.InsertAt(dSq, i);
			res.InsertAt(p, i);
		}
	}

	static bool AnyPlayerNear(vector pos, float r)
	{
		if (!s_Instance)
			return false;
		float r2 = r * r;
		foreach (Man m : s_Instance.m_Players)
		{
			if (m && m.IsAlive() && vector.DistanceSq(m.GetPosition(), pos) <= r2)
				return true;
		}
		return false;
	}

	static int AwakeCount()
	{
		if (!s_Instance)
			return 0;
		int n = 0;
		foreach (VB_AB_PointRuntime p : s_Instance.m_Active)
		{
			if (p.IsAwake())
				n++;
		}
		return n;
	}

	static int GetMaxAnomaliesTotal()
	{
		return Settings().MaxAnomaliesTotal;
	}

	static bool TakeArtifactBudget()
	{
		if (!s_Instance)
			return true;
		if (s_Instance.m_Data.m_Settings.MaxFieldArtifactsPerRestart <= 0)
			return true;
		if (s_Instance.m_ArtifactsLeft <= 0)
			return false;
		s_Instance.m_ArtifactsLeft--;
		return true;
	}

	// ---------------------------------------------------------------- редактор (VB_AB_EditorServer.c, ТЗ 6)
	// Старые конфиги после «Перечитать» и снятые точки рестарта: на них ещё смотрят живые боссы, поля и способности
	// (указатели без ref), поэтому держим до рестарта.
	protected ref array<ref VB_AB_BossData> m_OldData = new array<ref VB_AB_BossData>;
	protected ref array<ref VB_AB_PointRuntime> m_Dropped = new array<ref VB_AB_PointRuntime>;

	// Точки этого рестарта (выбранные на старте и добавленные редактором)
	static array<ref VB_AB_PointRuntime> ActivePoints()
	{
		if (!s_Instance)
			return null;
		return s_Instance.m_Active;
	}

	static VB_AB_PointRuntime FindRuntime(int pointId)
	{
		if (!s_Instance)
			return null;
		foreach (VB_AB_PointRuntime r : s_Instance.m_Active)
		{
			if (r && r.m_Cfg && r.m_Cfg.Id == pointId)
				return r;
		}
		return null;
	}

	// Точка в этом рестарте; если её не выбрали на старте (новая или не выпала) — добавляется к выбранным
	static VB_AB_PointRuntime EnsureRuntime(VB_AB_PointCfg cfg)
	{
		if (!s_Instance || !cfg)
			return null;
		VB_AB_PointRuntime r = FindRuntime(cfg.Id);
		if (r)
			return r;
		r = new VB_AB_PointRuntime(cfg);
		s_Instance.m_Active.Insert(r);
		VB_AB_Log.Info("точка " + cfg.Id + " добавлена в этот рестарт (редактор)");
		return r;
	}

	// «Призвать сейчас»: bossId пусто — случайный из списка точки. Лимит MaxAwakeBosses здесь не действует.
	// Уже живой босс на точке остаётся (возвращается он).
	static VB_AB_BossBase SummonAt(VB_AB_PointCfg cfg, string bossId)
	{
		VB_AB_PointRuntime r = EnsureRuntime(cfg);
		if (!r)
			return null;
		if (r.m_Boss && r.m_Boss.IsAlive())
			return r.m_Boss;
		r.m_Boss = null;
		r.m_Done = false;
		r.m_RespawnAt = 0;
		return r.Spawn(bossId);
	}

	// «Убрать босса»: босс уходит, точка до рестарта сама его больше не будит (вернуть — «Призвать сейчас»)
	static bool DespawnAt(int pointId)
	{
		VB_AB_PointRuntime r = FindRuntime(pointId);
		if (!r || !r.m_Boss)
			return false;
		VB_AB_BossBase b = r.m_Boss;
		r.m_Boss = null;
		r.m_Done = true;
		if (b.IsAlive())
			b.VB_Despawn();
		return true;
	}

	// Точка выключена в редакторе: в этом рестарте она больше не будит босса (живой доигрывает)
	static void StopPoint(int pointId)
	{
		VB_AB_PointRuntime r = FindRuntime(pointId);
		if (r)
			r.m_Done = true;
	}

	// Точка снова включена: если босса на ней нет — снова будит его при подходе игрока
	static void ResumePoint(int pointId)
	{
		VB_AB_PointRuntime r = FindRuntime(pointId);
		if (r && !r.m_Boss)
		{
			r.m_Done = false;
			r.m_RespawnAt = 0;
		}
	}

	// Точка удалена: живой босс уходит, точка снимается с рестарта
	static void DropPoint(int pointId)
	{
		VB_AB_PointRuntime r = FindRuntime(pointId);
		if (!r)
			return;
		if (r.m_Boss && r.m_Boss.IsAlive())
			r.m_Boss.VB_Despawn();
		r.m_Boss = null;
		r.m_Done = true;
		s_Instance.m_Dropped.Insert(r);
		s_Instance.m_Active.RemoveItem(r);
	}

	// «Перечитать конфиги»: anomalies.json, detectors.json, settings.json, bosses.json, points.json.
	// Живые боссы доигрывают со старыми настройками, новые и призванные после — с новыми.
	// Точки рестарта берут новые записи по Id; точку, которой больше нет в points.json, снимаем.
	static void ReloadConfigs()
	{
		if (!s_Instance)
			return;
		VB_AB_Cfg.Reload();
		VB_AB_BossData fresh = new VB_AB_BossData;
		fresh.Load();
		s_Instance.m_OldData.Insert(s_Instance.m_Data);
		s_Instance.m_Data = fresh;
		for (int i = s_Instance.m_Active.Count() - 1; i >= 0; i--)
		{
			VB_AB_PointRuntime r = s_Instance.m_Active[i];
			VB_AB_PointCfg p = fresh.FindPoint(r.m_Cfg.Id);
			if (p)
			{
				r.m_Cfg = p;
				continue;
			}
			VB_AB_Log.Info("точки " + r.m_Cfg.Id + " больше нет в points.json, снята с рестарта");
			DropPoint(r.m_Cfg.Id);
		}
	}
};
