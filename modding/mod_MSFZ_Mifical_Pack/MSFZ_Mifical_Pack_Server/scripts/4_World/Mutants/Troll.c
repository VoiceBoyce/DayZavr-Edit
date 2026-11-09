// Серверная логика Тролля: призыв Ричаров из грозовой тучи и удар с толчком.
// Туча (MSFZ_TrollStorm, клиентский PBO) встаёт рядом с игроком сбоку или за спиной, Ричары выходят из неё по одному.
modded class MSFZ_Troll
{
	protected const int   MSFZ_RICAR_MAX_ALIVE   = 6;      // сколько призванных Ричаров живёт одновременно
	protected const float MSFZ_UNDERGROUND_DEPTH = 3;      // Тролль ниже рельефа больше чем на 3 м — значит, он под землёй
	protected const float MSFZ_STORM_DIST_MIN    = 10;     // туча встаёт в 10–15 м от игрока
	protected const float MSFZ_STORM_DIST_MAX    = 15;
	protected const float MSFZ_STORM_SAFE_ANGLE  = 70;     // не со стороны Тролля: сектор ±70° от направления на него пропускаем
	protected const float MSFZ_STORM_PLAYER_SEARCH = 60;   // если у Тролля нет цели — ближайший игрок в 60 м
	protected const float MSFZ_STORM_SPREAD      = 4;      // Ричары выходят в радиусе 4 м от центра тучи
	protected const float MSFZ_STORM_SPREAD_UNDERGROUND = 2; // в бункере ближе, чтобы не попасть в стены
	protected const int   MSFZ_STORM_FIRST_MS    = 1500;   // первый Ричар через 1,5 с после появления тучи
	protected const int   MSFZ_STORM_STEP_MS     = 1000;   // следующие — раз в секунду
	protected const int   MSFZ_STORM_LINGER_MS   = 8000;   // туча висит ещё 8 с после последнего Ричара
	protected const int   MSFZ_STORM_FADE_MS     = 5000;   // и 5 с рассеивается, потом удаляется
	protected const int   MSFZ_RICAR_DESPAWN_MS  = 180000; // Ричары исчезают через 3 минуты после смерти Тролля
	protected const float MSFZ_PUSH_HIT          = 1500;   // толчок при попадании
	protected const float MSFZ_PUSH_BLOCKED      = 3000;   // толчок по блоку (Тролль сносит блок)

	protected ref array<EntityAI> m_MSFZ_Ricars = new array<EntityAI>;
	protected bool m_MSFZ_SummonOnAggroDone;  // призыв в начале боя уже был (сбрасывается, когда Тролль успокоится)
	protected bool m_MSFZ_SummonHalfHpDone;   // призыв при 50% здоровья уже был

	override bool ModCommandHandlerAfter(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		if (GetGame().IsServer() && IsAlive() && pCurrentCommandID == DayZInfectedConstants.COMMANDID_MOVE)
		{
			DayZInfectedInputController inputController = GetInputController();
			if (inputController)
			{
				int mindState = inputController.GetMindState();
				if (mindState == DayZInfectedConstants.MINDSTATE_CHASE && !m_MSFZ_SummonOnAggroDone)
				{
					m_MSFZ_SummonOnAggroDone = true;
					// не создаём объекты внутри обработчика команд ИИ — на следующем кадре
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MSFZ_SummonRicars, 0, false);
				}
				else if (mindState == DayZInfectedConstants.MINDSTATE_CALM)
				{
					m_MSFZ_SummonOnAggroDone = false;
				}
			}
		}

		return super.ModCommandHandlerAfter(pDt, pCurrentCommandID, pCurrentCommandFinished);
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		if (GetGame().IsServer() && IsAlive() && !m_MSFZ_SummonHalfHpDone && GetHealth01("", "") <= 0.5)
		{
			m_MSFZ_SummonHalfHpDone = true;
			// не создаём объекты внутри обработчика урона — на следующем кадре
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MSFZ_SummonRicars, 0, false);
		}
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (GetGame().IsServer())
			MSFZ_ScheduleRicarsDespawn();
	}

	override void EEDelete(EntityAI parent)
	{
		// живой Тролль пропал (очистка мира, админ) — призванные уходят вслед за ним
		if (GetGame().IsServer() && IsAlive())
			MSFZ_ScheduleRicarsDespawn();

		super.EEDelete(parent);
	}

	override bool FightLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_ATTACK)
			return MSFZ_MutantMelee.AttackHit(this, MSFZ_PUSH_HIT, MSFZ_PUSH_BLOCKED, false);

		return super.FightLogic(pCurrentCommandID, pInputController, pDt);
	}

	// Поднять грозовую тучу: из неё по одному выходят Ричары до лимита живых
	protected void MSFZ_SummonRicars()
	{
		if (!GetGame() || !IsAlive() || IsSetForDeletion())
			return;

		int toSpawn = MSFZ_RICAR_MAX_ALIVE - MSFZ_CountRicars();
		if (toSpawn <= 0)
			return;

		vector center = GetPosition();

		// В бункере туча встаёт на самом Тролле: вокруг игрока там стены, и Ричары оказались бы в них.
		// Высота по рельефу (SurfaceY) под землёй не годится: Ричары появились бы на поверхности над бункером.
		bool underground = center[1] < GetGame().SurfaceY(center[0], center[2]) - MSFZ_UNDERGROUND_DEPTH;
		vector stormPos;
		if (underground || !MSFZ_FindStormPos(stormPos))
		{
			stormPos = center;
			if (!underground)
				stormPos[1] = GetGame().SurfaceY(center[0], center[2]);
		}

		MSFZ_TrollStorm storm = MSFZ_TrollStorm.Cast(GetGame().CreateObjectEx("MSFZ_TrollStorm", stormPos, ECE_SETUP | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD));
		if (!storm)
			return;

		ScriptCallQueue queue = GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM);
		for (int n = 0; n < toSpawn; n++)
		{
			queue.CallLater(MSFZ_StormSpawnRicar, MSFZ_STORM_FIRST_MS + n * MSFZ_STORM_STEP_MS, false, storm, underground);
		}

		int lastMs = MSFZ_STORM_FIRST_MS + (toSpawn - 1) * MSFZ_STORM_STEP_MS;
		queue.CallLater(storm.MSFZ_Dissipate, lastMs + MSFZ_STORM_LINGER_MS, false);
		queue.CallLater(storm.DeleteSafe, lastMs + MSFZ_STORM_LINGER_MS + MSFZ_STORM_FADE_MS, false);
	}

	// Очередной Ричар выходит из тучи
	protected void MSFZ_StormSpawnRicar(MSFZ_TrollStorm storm, bool underground)
	{
		// Тролль погиб или пропал — призыв обрывается, туча доживает своё и уходит
		if (!storm || !GetGame() || !IsAlive() || IsSetForDeletion())
			return;

		// две тучи подряд (начало боя и 50% здоровья) не должны превысить лимит
		if (MSFZ_CountRicars() >= MSFZ_RICAR_MAX_ALIVE)
			return;

		float spread = MSFZ_STORM_SPREAD;
		if (underground)
			spread = MSFZ_STORM_SPREAD_UNDERGROUND;

		vector stormPos = storm.GetPosition();
		float x = stormPos[0] + Math.RandomFloat(-spread, spread);
		float z = stormPos[2] + Math.RandomFloat(-spread, spread);
		float y = stormPos[1];
		if (!underground)
			y = GetGame().SurfaceY(x, z); // на поверхности — по рельефу, чтобы не висели над склоном

		EntityAI ricar = EntityAI.Cast(GetGame().CreateObject("MSFZ_Ricar", Vector(x, y, z), false, true));
		if (ricar)
		{
			m_MSFZ_Ricars.Insert(ricar);
			storm.MSFZ_Strike();
		}
	}

	// Точка для тучи: 10–15 м от игрока, с которым дерётся Тролль, сбоку или за спиной (не со стороны Тролля)
	protected bool MSFZ_FindStormPos(out vector pos)
	{
		PlayerBase player;
		DayZInfectedInputController inputController = GetInputController();
		if (inputController)
			player = PlayerBase.Cast(inputController.GetTargetEntity());
		if (!player || !player.IsAlive())
			player = MSFZ_NearestPlayer(MSFZ_STORM_PLAYER_SEARCH);
		if (!player)
			return false;

		vector playerPos = player.GetPosition();
		vector toTroll = GetPosition() - playerPos;
		toTroll[1] = 0;
		if (toTroll.Length() < 0.1)
			toTroll = "0 0 1";
		toTroll.Normalize();

		for (int attempt = 0; attempt < 8; attempt++)
		{
			float angle = Math.RandomFloat(MSFZ_STORM_SAFE_ANGLE, 360 - MSFZ_STORM_SAFE_ANGLE) * Math.DEG2RAD;
			float sinA = Math.Sin(angle);
			float cosA = Math.Cos(angle);
			float dist = Math.RandomFloat(MSFZ_STORM_DIST_MIN, MSFZ_STORM_DIST_MAX);
			float x = playerPos[0] + (toTroll[0] * cosA - toTroll[2] * sinA) * dist;
			float z = playerPos[2] + (toTroll[0] * sinA + toTroll[2] * cosA) * dist;

			// не в воде и без большого перепада высоты с игроком (обрыв, игрок на крыше или этаже)
			if (GetGame().SurfaceIsSea(x, z) || GetGame().SurfaceIsPond(x, z))
				continue;
			float y = GetGame().SurfaceY(x, z);
			if (Math.AbsFloat(y - playerPos[1]) > 5)
				continue;

			pos = Vector(x, y, z);
			return true;
		}

		return false;
	}

	protected PlayerBase MSFZ_NearestPlayer(float maxDist)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);

		PlayerBase best;
		float bestDistSq = maxDist * maxDist;
		vector myPos = GetPosition();
		foreach (Man man : players)
		{
			PlayerBase candidate = PlayerBase.Cast(man);
			if (!candidate || !candidate.IsAlive())
				continue;
			float distSq = vector.DistanceSq(myPos, candidate.GetPosition());
			if (distSq < bestDistSq)
			{
				bestDistSq = distSq;
				best = candidate;
			}
		}

		return best;
	}

	// Сколько призванных Ричаров живы (мёртвых и удалённых убирает из списка)
	protected int MSFZ_CountRicars()
	{
		for (int i = m_MSFZ_Ricars.Count() - 1; i >= 0; i--)
		{
			EntityAI old = m_MSFZ_Ricars[i];
			if (!old || !old.IsAlive())
				m_MSFZ_Ricars.Remove(i);
		}
		return m_MSFZ_Ricars.Count();
	}

	// Удалить всех призванных (живых и тела) через MSFZ_RICAR_DESPAWN_MS
	protected void MSFZ_ScheduleRicarsDespawn()
	{
		foreach (EntityAI summoned : m_MSFZ_Ricars)
		{
			if (summoned)
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(summoned.DeleteSafe, MSFZ_RICAR_DESPAWN_MS, false);
		}
		m_MSFZ_Ricars.Clear();
	}
};
