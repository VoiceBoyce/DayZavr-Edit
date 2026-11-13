// Поле босса (ТЗ 3, 4.2). Открывается при первом агре: шансы бросаются заново, аномалии встают волной,
// засеваются артефакты. После смерти босса поле гаснет за AfterDeathSec, аномалии — по одной.
// Только сервер.

class VB_AB_FieldPlan
{
	string m_Cls;
	string m_Place;
	bool m_Always;		// строка без шанса (при переборе MaxAnomalies такие не выбрасываются)
	vector m_Pos;
};

class VB_AB_Field
{
	static const int SPAWN_STEP_MS = 400;		// между появлением аномалий
	static const float BIG_RADIUS = 8;			// аномалия «большая» (Гроза, Снег, Мёртвая зона)
	static const float BIG_MIN_FIELD = 25;		// большая — только в центр и только в поле от 25 м
	protected static int s_TotalAnomalies;		// на весь сервер (MaxAnomaliesTotal)
	// Гаснущие поля. Поле живёт в боссе (m_VB_Field), а тело после разделки удаляется через 1,5 с — раньше, чем
	// поле догаснет (AfterDeathSec). Без этого списка поле умирало вместе с телом, таймеры снимались в деструкторе,
	// и аномалии с находками оставались до рестарта (пользователь, 2026-11-13).
	protected static ref array<ref VB_AB_Field> s_Dying;

	protected VB_AB_BossBase m_Boss;
	protected VB_AB_BossCfg m_Cfg;
	protected vector m_Center;
	protected ref array<VB_AB_AnomalyBase> m_Anomalies = new array<VB_AB_AnomalyBase>;
	protected ref array<ref VB_AB_FieldPlan> m_Queue = new array<ref VB_AB_FieldPlan>;
	protected bool m_HasCenter;
	protected bool m_Closing;
	protected bool m_Seeded;

	void VB_AB_Field(VB_AB_BossBase boss, VB_AB_BossCfg cfg, vector center)
	{
		m_Boss = boss;
		m_Cfg = cfg;
		m_Center = center;
	}

	void ~VB_AB_Field()
	{
		if (GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(SpawnNext);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(FinishCollapse);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(ExpireExtra);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(RetryCenter);
		}
	}

	vector GetCenter()
	{
		return m_Center;
	}

	float GetRadius()
	{
		return m_Cfg.Field.Radius;
	}

	array<VB_AB_AnomalyBase> GetAnomalies()
	{
		return m_Anomalies;
	}

	bool IsClosing()
	{
		return m_Closing;
	}

	// ---------------------------------------------------------------- для способностей (VB_AB_Abilities.c)
	// Стоит ли в поле хоть одна из перечисленных аномалий (RequiresAnomaly)
	bool HasAnyOf(array<string> classes)
	{
		if (!classes || classes.Count() == 0)
			return true;
		foreach (string cls : classes)
		{
			if (HasAnomaly(cls))
				return true;
		}
		return false;
	}

	// Ближайшая к точке аномалия поля одного из классов (пустой список — любая)
	VB_AB_AnomalyBase NearestOf(vector pos, array<string> classes)
	{
		VB_AB_AnomalyBase best;
		float bestSq = 1e10;
		foreach (VB_AB_AnomalyBase a : m_Anomalies)
		{
			if (!a)
				continue;
			if (classes && classes.Count() > 0 && classes.Find(a.GetType()) == -1)
				continue;
			float dSq = vector.DistanceSq(pos, a.GetPosition());
			if (dSq < bestSq)
			{
				bestSq = dSq;
				best = a;
			}
		}
		return best;
	}

	// Временная аномалия (способность «Аномалия под ногами»): ставится сразу, гаснет через lifeSec.
	// Считается в общий потолок сервера, но не в MaxAnomalies поля; одновременно не больше MAX_EXTRA.
	static const int MAX_EXTRA = 3;
	protected int m_Extra;

	VB_AB_AnomalyBase SpawnExtra(string cls, vector pos, float lifeSec)
	{
		if (m_Closing || m_Extra >= MAX_EXTRA)
			return null;
		int total = VB_AB_Server.GetMaxAnomaliesTotal();
		if (total > 0 && s_TotalAnomalies >= total)
			return null;
		VB_AB_AnomalyBase a = VB_AB_AnomalyBase.Cast(GetGame().CreateObjectEx(cls, pos, ECE_PLACE_ON_SURFACE | ECE_NOPERSISTENCY_WORLD));
		if (!a)
			return null;
		a.VB_SetOwner(m_Boss);
		m_Anomalies.Insert(a);
		s_TotalAnomalies++;
		m_Extra++;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ExpireExtra, lifeSec * 1000, false, a);
		return a;
	}

	void ExpireExtra(VB_AB_AnomalyBase a)
	{
		m_Extra = Math.Max(0, m_Extra - 1);
		if (a && m_Anomalies.Find(a) != -1)
			DeleteAnomaly(a);
	}

	bool HasAnomaly(string cls)
	{
		foreach (VB_AB_AnomalyBase a : m_Anomalies)
		{
			if (a && a.GetType() == cls)
				return true;
		}
		foreach (VB_AB_FieldPlan p : m_Queue)
		{
			if (p.m_Cls == cls)
				return true;
		}
		return false;
	}

	// ---------------------------------------------------------------- открытие
	void Open()
	{
		array<ref VB_AB_FieldPlan> plan = Roll(m_Cfg.Anomalies, true);
		string log = "";
		foreach (VB_AB_FieldPlan p : plan)
			log += " " + p.m_Cls + "(" + p.m_Place + ")";
		VB_AB_Log.Info("поле " + m_Cfg.Id + " раскрыто у " + m_Center.ToString() + ", аномалий " + plan.Count() + ":" + log);
		Enqueue(plan, m_Center);
	}

	// Добавки на фазе здоровья: вокруг места, где босс сейчас.
	void AddPhase(VB_AB_PhaseCfg ph, vector around)
	{
		if (m_Closing || !ph || !ph.Anomalies)
			return;
		array<ref VB_AB_FieldPlan> plan = Roll(ph.Anomalies, false);
		for (int i = plan.Count() - 1; i >= 0; i--)
		{
			// «в центр, если её нет»: такая аномалия уже стоит — не добавляем
			if (plan[i].m_Place == "Center" && HasAnomaly(plan[i].m_Cls))
				plan.Remove(i);
		}
		if (plan.Count() > 0)
			VB_AB_Log.Info("поле " + m_Cfg.Id + ": фаза " + ph.Health + ", добавлено аномалий " + plan.Count());
		Enqueue(plan, around);
	}

	// Бросок шансов по строкам. first — первичное раскрытие (соблюдает MaxAnomalies).
	protected array<ref VB_AB_FieldPlan> Roll(array<ref VB_AB_AnomalyLine> lines, bool first)
	{
		array<ref VB_AB_FieldPlan> res = new array<ref VB_AB_FieldPlan>;
		if (!lines)
			return res;
		map<string, int> perClass = new map<string, int>;
		foreach (VB_AB_AnomalyBase a : m_Anomalies)
		{
			if (a)
				perClass.Set(a.GetType(), perClass.Get(a.GetType()) + 1);
		}
		bool centerTaken = m_HasCenter;
		foreach (VB_AB_AnomalyLine l : lines)
		{
			if (!l.RollChance())
				continue;
			string cls = l.PickClass();
			if (cls == "")
				continue;
			int n = l.PickCount();
			for (int k = 0; k < n; k++)
			{
				int limit = l.MaxInField;
				VB_AB_AnomalyCfg acfg = VB_AB_Cfg.Get().GetAnomaly(cls);
				if (acfg && acfg.MaxInField > 0 && (limit <= 0 || acfg.MaxInField < limit))
					limit = acfg.MaxInField;
				if (limit > 0 && perClass.Get(cls) >= limit)
					break;
				string place = l.Place;
				float r = VB_AB_AnomalyDef.Get(cls).Radius;
				if (r >= BIG_RADIUS)
				{
					// большая аномалия: только в центр и только в большом поле
					if (centerTaken || m_Cfg.Field.Radius < BIG_MIN_FIELD)
					{
						VB_AB_Log.Info("поле " + m_Cfg.Id + ": " + cls + " не встаёт (большая, центр занят или поле < 25 м)");
						break;
					}
					place = "Center";
				}
				if (place == "Center")
				{
					if (centerTaken)
						place = "Ring";
					else
						centerTaken = true;
				}
				VB_AB_FieldPlan p = new VB_AB_FieldPlan;
				p.m_Cls = cls;
				p.m_Place = place;
				p.m_Always = l.Chance >= 100;
				res.Insert(p);
				perClass.Set(cls, perClass.Get(cls) + 1);
			}
		}
		// потолок поля: лишние случайные отбрасываются, постоянные остаются
		int cap = m_Cfg.Field.MaxAnomalies;
		int have = m_Anomalies.Count();
		if (cap > 0)
		{
			for (int j = res.Count() - 1; j >= 0 && have + res.Count() > cap; j--)
			{
				if (!res[j].m_Always)
					res.Remove(j);
			}
			while (have + res.Count() > cap && res.Count() > 0)
				res.Remove(res.Count() - 1);
		}
		return res;
	}

	protected void Enqueue(array<ref VB_AB_FieldPlan> plan, vector around)
	{
		array<vector> taken = new array<vector>;
		foreach (VB_AB_AnomalyBase a : m_Anomalies)
		{
			if (a)
				taken.Insert(a.GetPosition());
		}
		foreach (VB_AB_FieldPlan q : m_Queue)
			taken.Insert(q.m_Pos);
		// центр первым: волна идёт от центра
		array<ref VB_AB_FieldPlan> ordered = new array<ref VB_AB_FieldPlan>;
		foreach (VB_AB_FieldPlan c : plan)
		{
			if (c.m_Place == "Center")
				ordered.Insert(c);
		}
		foreach (VB_AB_FieldPlan o : plan)
		{
			if (o.m_Place != "Center")
				ordered.Insert(o);
		}
		bool empty = m_Queue.Count() == 0;
		foreach (VB_AB_FieldPlan p : ordered)
		{
			vector pos;
			if (!PickPos(p, around, taken, pos))
			{
				// центр занят игроком (поле раскрылось, когда тот стоял у босса) — пробуем ещё, пока не отойдёт
				if (p.m_Place == "Center" && m_CenterRetry < CENTER_RETRIES)
				{
					m_CenterPending = p;
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RetryCenter, CENTER_RETRY_MS, false);
					continue;
				}
				VB_AB_Log.Info("поле " + m_Cfg.Id + ": нет места для " + p.m_Cls + " (" + p.m_Place + ")");
				continue;
			}
			p.m_Pos = pos;
			taken.Insert(pos);
			m_Queue.Insert(p);
		}
		if (empty && m_Queue.Count() > 0)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SpawnNext, 300, false);
		else if (empty && !m_Seeded)
			SeedArtifacts();
	}

	static const int CENTER_RETRIES = 8;
	static const int CENTER_RETRY_MS = 2000;
	protected ref VB_AB_FieldPlan m_CenterPending;
	protected int m_CenterRetry;

	void RetryCenter()
	{
		if (m_Closing || !m_CenterPending)
			return;
		m_CenterRetry++;
		array<ref VB_AB_FieldPlan> one = new array<ref VB_AB_FieldPlan>;
		one.Insert(m_CenterPending);
		m_CenterPending = null;
		Enqueue(one, m_Center);
	}

	protected bool PickPos(VB_AB_FieldPlan p, vector around, array<vector> taken, out vector pos)
	{
		float R = m_Cfg.Field.Radius;
		float rMin = 0.2 * R;
		float rMax = R;
		if (p.m_Place == "Center")
		{
			rMin = 0;
			rMax = 0.5;
		}
		else if (p.m_Place == "Ring")
		{
			rMin = 0.3 * R;
			rMax = 0.7 * R;
		}
		else if (p.m_Place == "Edge")
		{
			rMin = 0.7 * R;
			rMax = R;
		}
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		for (int t = 0; t < 30; t++)
		{
			vector c = VB_AB_Finds.RandomGroundPos(around, rMin, rMax);
			if (p.m_Place != "Center" && vector.Distance(c, m_Center) > R + 2)
				continue;
			if (GetGame().SurfaceIsSea(c[0], c[2]) || GetGame().SurfaceIsPond(c[0], c[2]))
				continue;
			bool bad = false;
			foreach (Man m : players)
			{
				if (m && vector.Distance(m.GetPosition(), c) < m_Cfg.Field.KeepFromPlayers)
				{
					bad = true;
					break;
				}
			}
			if (!bad && p.m_Place != "Center")
			{
				foreach (vector tv : taken)
				{
					if (vector.Distance(tv, c) < m_Cfg.Field.MinGap)
					{
						bad = true;
						break;
					}
				}
			}
			if (!bad && VB_InBuilding(c))
				bad = true;
			if (bad)
				continue;
			pos = c;
			return true;
		}
		return false;
	}

	// Точка внутри или вплотную к дому/стене
	protected bool VB_InBuilding(vector c)
	{
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition3D(c, 1.5, objs, null);
		foreach (Object o : objs)
		{
			if (o && (o.IsBuilding() || o.IsInherited(BuildingBase)))
				return true;
		}
		return false;
	}

	void SpawnNext()
	{
		if (m_Closing)
			return;
		if (m_Queue.Count() == 0)
		{
			if (!m_Seeded)
				SeedArtifacts();
			return;
		}
		VB_AB_FieldPlan p = m_Queue[0];
		m_Queue.Remove(0);
		int total = VB_AB_Server.GetMaxAnomaliesTotal();
		if (total <= 0 || s_TotalAnomalies < total)
		{
			VB_AB_AnomalyBase a = VB_AB_AnomalyBase.Cast(GetGame().CreateObjectEx(p.m_Cls, p.m_Pos, ECE_PLACE_ON_SURFACE | ECE_NOPERSISTENCY_WORLD));
			if (a)
			{
				a.VB_SetOwner(m_Boss);
				m_Anomalies.Insert(a);
				s_TotalAnomalies++;
				if (p.m_Place == "Center")
					m_HasCenter = true;
			}
		}
		else
			VB_AB_Log.Warn("достигнут MaxAnomaliesTotal, " + p.m_Cls + " не поставлена");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SpawnNext, SPAWN_STEP_MS, false);
	}

	// Артефакты из таблицы босса: часть у аномалий (0,5–3 м), часть по полю.
	protected void SeedArtifacts()
	{
		m_Seeded = true;
		VB_AB_ArtifactsCfg t = m_Cfg.Artifacts;
		if (!t || !t.Items || t.Items.Count() == 0)
			return;
		int n = 0;
		for (int r = 0; r < t.Rolls; r++)
		{
			if (t.MaxInField > 0 && n >= t.MaxInField)
				break;
			if (!VB_AB_Server.TakeArtifactBudget())
			{
				VB_AB_Log.Info("лимит артефактов за рестарт исчерпан");
				break;
			}
			VB_AB_LootTable one = new VB_AB_LootTable;
			one.Rolls = 1;
			one.Nothing = t.Nothing;
			one.Items = t.Items;
			vector c = m_Center;
			float rMin = 0;
			float rMax = m_Cfg.Field.Radius;
			if (m_Anomalies.Count() > 0 && Math.RandomFloat01() < t.NearAnomalyShare)
			{
				VB_AB_AnomalyBase a = m_Anomalies.GetRandomElement();
				if (a)
				{
					c = a.GetPosition();
					rMin = 0.5;
					rMax = 3.0;
				}
			}
			n += VB_AB_Finds.SeedTable(one, c, rMin, rMax, this);
		}
		VB_AB_Log.Info("поле " + m_Cfg.Id + ": засеяно артефактов " + n);
	}

	// ---------------------------------------------------------------- закрытие
	// После смерти: аномалии гаснут по одной в течение overSec, в конце пропадают и неподобранные находки.
	void Collapse(float overSec)
	{
		if (m_Closing)
			return;
		m_Closing = true;
		if (!s_Dying)
			s_Dying = new array<ref VB_AB_Field>;
		if (s_Dying.Find(this) == -1)
			s_Dying.Insert(this);
		m_Queue.Clear();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(SpawnNext);
		foreach (VB_AB_AnomalyBase a : m_Anomalies)
		{
			if (!a)
				continue;
			a.VB_SetFading(true);		// детекторы меняют писк: находки скоро пропадут (ТЗ 8.19)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DeleteAnomaly, Math.RandomFloat(0.3, 1.0) * overSec * 1000, false, a);
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FinishCollapse, overSec * 1000 + 500, false);
	}

	void DeleteAnomaly(VB_AB_AnomalyBase a)
	{
		if (!a)
			return;
		m_Anomalies.RemoveItem(a);
		s_TotalAnomalies = Math.Max(0, s_TotalAnomalies - 1);
		GetGame().ObjectDelete(a);
	}

	void FinishCollapse()
	{
		CloseNow();
		VB_AB_Log.Info("поле " + m_Cfg.Id + " погасло, находок осталось: " + VB_AB_Finds.Count());
		// себя из списка — следующим тактом, а не изнутри своего же метода (список держит последнюю ссылку)
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_AB_Field.ForgetDying, 0, false, this);
	}

	static void ForgetDying(VB_AB_Field f)
	{
		if (s_Dying)
			s_Dying.RemoveItem(f);
	}

	// Сразу: уход босса, сброс, рестарт.
	void CloseNow()
	{
		m_Closing = true;
		m_Queue.Clear();
		if (GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(SpawnNext);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(DeleteAnomaly);
		}
		foreach (VB_AB_AnomalyBase a : m_Anomalies)
		{
			if (a)
			{
				s_TotalAnomalies = Math.Max(0, s_TotalAnomalies - 1);
				GetGame().ObjectDelete(a);
			}
		}
		m_Anomalies.Clear();
		VB_AB_Finds.RemoveByOwner(this);
	}

	// Босс внутри одной из своих аномалий (щит, этап 7)
	bool IsInsideOwnAnomaly(vector pos)
	{
		foreach (VB_AB_AnomalyBase a : m_Anomalies)
		{
			if (a && a.VB_IsInside(pos, a.VB_GetRadius(), 3))
				return true;
		}
		return false;
	}
};
