//	Место под одно существо в зоне
class VD_AnimalSlot
{
	Object m_Object;
	bool m_IsTracked = false;	// отличает «существо пропало» от «ещё не спавнили»
	int m_RespawnAt = 0;		// раньше этого времени место не заполняем
}

//	Зона из конфига: держит своих существ сама, поэтому смена дня и ночи не сбрасывает таймеры
class VD_GroupAnimals
{
	private ref TStringArray m_Classnames;
	private vector m_Position;
	private int m_DayCount = 0;
	private int m_NightCount = 0;
	private int m_RespawnTime;
	private int m_Radius;
	private int m_Randomplus;
	private int m_Randomminus;
	private ref array<ref VD_AnimalSlot> m_Slots = new array<ref VD_AnimalSlot>;

	float m_NearestPlayer;
	bool m_IsQueued = false;
	bool m_IsLive = false;

	void VD_GroupAnimals(TStringArray v_Classnames, vector v_Position, int v_RespawnTime, int v_Radius, int v_Randomplus, int v_Randomminus)
	{
		m_Classnames = v_Classnames;
		m_Position = v_Position;
		m_RespawnTime = v_RespawnTime * 1000;  // Конвертируем в миллисекунды
		m_Radius = v_Radius;
		m_Randomplus = v_Randomplus;
		m_Randomminus = v_Randomminus;
	}

	void AddCount(bool v_IsNight, int v_Count)
	{
		if (v_IsNight)
			m_NightCount += v_Count;
		else
			m_DayCount += v_Count;

		while (m_Slots.Count() < m_DayCount || m_Slots.Count() < m_NightCount)
		{
			m_Slots.Insert(new VD_AnimalSlot());
		}
	}

	vector GetPosition()
	{
		return m_Position;
	}

	int GetRadius()
	{
		return m_Radius;
	}

	int GetCount(bool v_IsNight)
	{
		if (v_IsNight)
			return m_NightCount;
		return m_DayCount;
	}

	void OnPlayerNearby(vector v_PlayerPosition)
	{
		float v_Distance = VD_StaticFnc.Distance2D(v_PlayerPosition, m_Position);
		if (v_Distance < m_NearestPlayer)
		{
			m_NearestPlayer = v_Distance;
		}
	}

	bool HasTracked()
	{
		foreach (VD_AnimalSlot v_Slot : m_Slots)
		{
			if (v_Slot.m_IsTracked)
			{
				return true;
			}
		}
		return false;
	}

	void Update(array<Man> v_Players, bool v_IsNight, PGFilter v_Filter)
	{
		int v_Wanted = GetCount(v_IsNight);
		bool v_PlayerInRadius = m_NearestPlayer <= m_Radius;
		bool v_PlayerKeepsZone = m_NearestPlayer <= (m_Radius * 1.2);
		int v_Now = GetGame().GetTime();

		for (int v_I = 0; v_I < m_Slots.Count(); v_I++)
		{
			VD_AnimalSlot v_Slot = m_Slots[v_I];
			bool v_IsExtra = v_I >= v_Wanted;  // лишнее место в текущей фазе дня

			if (v_Slot.m_IsTracked)
			{
				if (!v_Slot.m_Object || !v_Slot.m_Object.IsAlive())
				{
					// Убит или удалён извне: таймер только здесь, труп не трогаем
					v_Slot.m_Object = null;
					v_Slot.m_IsTracked = false;
					v_Slot.m_RespawnAt = v_Now + m_RespawnTime;
				}
				else if ((v_IsExtra || !v_PlayerKeepsZone) && !VD_StaticFnc.IsPlayerNear(v_Players, v_Slot.m_Object.GetPosition(), VD_DESPAWN_SAFE_DISTANCE))
				{
					// Живого убираем без таймера: при следующем визите он вернётся
					GetGame().ObjectDelete(v_Slot.m_Object);
					v_Slot.m_Object = null;
					v_Slot.m_IsTracked = false;
				}
			}

			if (!v_Slot.m_IsTracked && !v_IsExtra && v_PlayerInRadius && v_Now >= v_Slot.m_RespawnAt)
			{
				Spawn(v_Slot, v_Players, v_Filter, v_Now);
			}
		}
	}

	private void Spawn(VD_AnimalSlot v_Slot, array<Man> v_Players, PGFilter v_Filter, int v_Now)
	{
		vector v_Position;
		if (!FindSpawnPosition(v_Players, v_Filter, v_Position))
		{
			v_Slot.m_RespawnAt = v_Now + (VD_SPAWN_RETRY_TIME * 1000);
			return;
		}

		Object v_Object = GetGame().CreateObject(m_Classnames.GetRandomElement(), v_Position, false, true);
		if (!v_Object)
		{
			v_Slot.m_RespawnAt = v_Now + m_RespawnTime;
			return;
		}

		v_Slot.m_Object = v_Object;
		v_Slot.m_IsTracked = true;
	}

	//	Точка на навмеше (не вода и не внутри построек) дальше минимальной дистанции от игроков.
	//	С каждой попыткой разброс растёт, чтобы игрок в центре зоны не блокировал спавн
	private bool FindSpawnPosition(array<Man> v_Players, PGFilter v_Filter, out vector v_Result)
	{
		AIWorld v_AIWorld = GetGame().GetWorld().GetAIWorld();
		vector v_Candidate = "0 0 0";
		vector v_Sampled;
		array<Object> v_Excluded = new array<Object>;

		for (int v_I = 0; v_I < VD_SPAWN_ATTEMPTS; v_I++)
		{
			float v_Spread = 1.0 + (v_I * 0.25);
			v_Candidate[0] = m_Position[0] + Math.RandomFloatInclusive(m_Randomminus * v_Spread, m_Randomplus * v_Spread);
			v_Candidate[2] = m_Position[2] + Math.RandomFloatInclusive(m_Randomminus * v_Spread, m_Randomplus * v_Spread);
			v_Candidate[1] = GetGame().SurfaceY(v_Candidate[0], v_Candidate[2]);

			if (!v_AIWorld.SampleNavmeshPosition(v_Candidate, 3.0, v_Filter, v_Sampled))
				continue;

			if (VD_StaticFnc.IsPlayerNear(v_Players, v_Sampled, VD_SPAWN_MIN_PLAYER_DISTANCE))
				continue;

			// Навмеш считает мелководье проходимым
			if (GetGame().SurfaceIsSea(v_Sampled[0], v_Sampled[2]) || GetGame().SurfaceIsPond(v_Sampled[0], v_Sampled[2]))
				continue;

			// Место в рост существа свободно от деревьев, заборов и другой статики
			vector v_BoxCenter = v_Sampled;
			v_BoxCenter[1] = v_BoxCenter[1] + 1.1;
			if (GetGame().IsBoxCollidingGeometry(v_BoxCenter, "0 0 0", "0.6 1.6 0.6", ObjIntersect.Geom, ObjIntersect.View, v_Excluded, null, CollisionFlags.ONLYSTATIC))
				continue;

			v_Result = v_Sampled;
			return true;
		}
		return false;
	}
}
