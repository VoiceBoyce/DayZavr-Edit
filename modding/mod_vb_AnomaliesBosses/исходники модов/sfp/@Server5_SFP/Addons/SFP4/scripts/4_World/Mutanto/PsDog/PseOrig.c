class SFP_PseDOG_Base extends AnimalBase
{
	// ==== Параметры призыва ====
	// Радиус, в котором считаем "рядом" клонов
	static const float SFP_CLONE_NEAR_DISTANCE     = 35.0;
	// Кулдаун между проверками / призывами (чтобы не спамить каждый тик)
	static const float SFP_SUMMON_COOLDOWN         = 5.0;
	// Радиус спавна клонов вокруг главного
	static const float SFP_CLONE_SPAWN_MIN_RADIUS  = 4.0;
	static const float SFP_CLONE_SPAWN_MAX_RADIUS  = 10.0;
	
	// Класс клонов
	static const string SFP_CLONE_CLASSNAME        = "SFP_PseDOG_Brown_Cloned";
	// 1.30 port: hard cap of ALIVE clones of one master (the old code counted only clones within 35 m,
	// so clones chasing a player away made the master spawn 6 more every 5 s without any limit)
	static const int   SFP_CLONE_MAX_ALIVE         = 6;
	// clones that ran this far from the master are removed
	static const float SFP_CLONE_MAX_DISTANCE      = 100.0;
	
	// ==== Служебные поля ====
	// Список призванных клонов
	protected ref array<AnimalBase> m_SFP_SummonedClones;
	// Флаг – этот экземпляр умеет призывать (у клонов будет false)
	protected bool m_SFP_CanSummonClones;
	// Флаг – есть ли сейчас агрессия
	protected bool m_SFP_HasAggro;
	// Кулдаун до следующей попытки призыва
	protected float m_SFP_SummonCooldown;
	// Призыв на первом агро уже был
	protected bool m_SFP_InitialAggroSummoned;

	// ==== FX смерти клона (синхронизируется по сети) ====
	// true, когда нужно проиграть FX смерти клона (ставится на сервере)
	protected bool m_SFP_CloneDeathFx;
	// локальный флаг, чтобы не проигрывать FX дважды на клиенте
	protected bool m_SFP_CloneDeathFxPlayed;

	// Конструктор
	void SFP_PseDOG_Base()
	{
		// Регистрируем сетевую переменную для FX смерти клона
		RegisterNetSyncVariableBool("m_SFP_CloneDeathFx");

		m_SFP_SummonedClones        = new array<AnimalBase>();
		m_SFP_CanSummonClones       = true;    // у главного – да, у клонов перепишем в false
		m_SFP_HasAggro              = false;
		m_SFP_SummonCooldown        = 0;
		m_SFP_InitialAggroSummoned  = false;

		m_SFP_CloneDeathFx          = false;
		m_SFP_CloneDeathFxPlayed    = false;
	}
	
	// ==== То, что было у тебя ====
	override bool CanBeSkinned()
	{
		return true;
	}
	
	// ==== Смерть главного мутанта ====
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		// При смерти главного – удаляем всех его клонов
		if (m_SFP_SummonedClones)
		{
			for (int i = 0; i < m_SFP_SummonedClones.Count(); ++i)
			{
				AnimalBase clone = m_SFP_SummonedClones[i];
				if (clone && !clone.IsDamageDestroyed())
				{
					clone.Delete();
				}
			}
			
			m_SFP_SummonedClones.Clear();
		}
	}

	// ==== Синхронизация переменных (для FX смерти клона) ====
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		// FX смерти нужен только для клонов (у них m_SFP_CanSummonClones = false)
		if (!m_SFP_CanSummonClones && m_SFP_CloneDeathFx && !m_SFP_CloneDeathFxPlayed)
		{
			SFP_PlayCloneDeathFx();
			m_SFP_CloneDeathFxPlayed = true;
		}
	}

	protected void SFP_PlayCloneDeathFx()
	{
		// Визуал и звук только на клиентах
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;

		vector pos = GetPosition();

		// Партикл смерти клона
		Particle.PlayInWorld(ParticleList.SFP_PSEDOG_CLONED, pos);

		// Звук смерти клона
		EffectSound s = SEffectManager.PlaySound("SFP_PseDog_DeadClone_Soundset", pos);
		if (s)
		{
			s.SetAutodestroy(true);
		}
	}
	
	// ==== Хук, вызываемый в CommandHandler-е (специально для модов) ====
	override bool ModCommandHandlerInside(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		// Если этот экземпляр не должен призывать – сразу выходим
		if (!m_SFP_CanSummonClones)
			return super.ModCommandHandlerInside(pDt, pCurrentCommandID, pCurrentCommandFinished);
		
		// Обновляем кулдаун
		if (m_SFP_SummonCooldown > 0)
			m_SFP_SummonCooldown -= pDt;
		
		DayZAnimalInputController ic = DayZAnimalInputController.Cast(GetInputController());
		
		// Проверяем только живое существо и наличие контроллера
		if (ic && !IsDamageDestroyed())
		{
			bool isAttacking = ic.IsAttack(); // животное в атаке (агрессия на цель)

			bool prevAggro = m_SFP_HasAggro;
			bool newAggro  = isAttacking;

			m_SFP_HasAggro = newAggro;

			// === МОМЕНТ НАЧАЛА АГРО ===
			// как только перешли из состояния "без агро" в "агро" – сразу призываем клонов
			if (m_SFP_HasAggro && !prevAggro)
			{
				SFP_OnAggroStart();
			}

			// Пока есть агро – периодически дозываем клонов по кулдауну
			if (m_SFP_HasAggro && m_SFP_SummonCooldown <= 0)
			{
				SFP_TrySummonClones();
			}

			// Агро ушло – в следующем бою снова сможем сделать стартовый призыв
			if (!m_SFP_HasAggro)
			{
				m_SFP_InitialAggroSummoned = false;
			}
		}
		
		return super.ModCommandHandlerInside(pDt, pCurrentCommandID, pCurrentCommandFinished);
	}

	// === Стартовый призыв клонов при первом агро ===
	protected void SFP_OnAggroStart()
	{
		// Только сервер создаёт сущности
		if (!GetGame() || !GetGame().IsServer())
			return;

		// Уже призывали при этом бою – выходим
		if (m_SFP_InitialAggroSummoned)
			return;

		SFP_CleanupCloneArray();

		int toSpawn = SFP_CLONE_MAX_ALIVE - m_SFP_SummonedClones.Count();

		if (toSpawn > 0)
		{
			for (int i = 0; i < toSpawn; ++i)
			{
				SFP_SpawnClone();
			}
		}

		// ставим кулдаун, чтобы не сразу же дозывать ещё раз
		m_SFP_SummonCooldown       = SFP_SUMMON_COOLDOWN;
		m_SFP_InitialAggroSummoned = true;
	}
	
	// ==== Попытка призыва клонов (дозапас по бою) ====
	protected void SFP_TrySummonClones()
	{
		// Призываем только когда мутант в агрессии
		if (!m_SFP_HasAggro)
			return;
		
		// Только на сервере создаём объекты
		if (!GetGame().IsServer())
			return;
		
		SFP_CleanupCloneArray();
		
		// Считаем, сколько клонов живы и находятся рядом
		int aliveNearby = SFP_CountNearbyClones();
		
		// Условие по ТЗ: если рядом осталось 2 или 0 мутантов – дозовём до 5
		if (aliveNearby <= 3)
		{
			int toSpawn = SFP_CLONE_MAX_ALIVE - m_SFP_SummonedClones.Count();

			for (int i = 0; i < toSpawn; ++i)
			{
				SFP_SpawnClone();
			}
			
			// Ставим кулдаун, чтобы не спавнить каждый кадр
			m_SFP_SummonCooldown = SFP_SUMMON_COOLDOWN;
		}
	}
	
	// Чистим список от мёртвых / удалённых клонов
	protected void SFP_CleanupCloneArray()
	{
		if (!m_SFP_SummonedClones)
			return;
		
		vector myPos = GetPosition();
		float maxDistSq = SFP_CLONE_MAX_DISTANCE * SFP_CLONE_MAX_DISTANCE;

		for (int i = m_SFP_SummonedClones.Count() - 1; i >= 0; --i)
		{
			AnimalBase clone = m_SFP_SummonedClones[i];
			if (!clone || clone.IsDamageDestroyed())
			{
				m_SFP_SummonedClones.Remove(i);
				continue;
			}

			// clone ran too far from the master: remove it so it no longer counts against the cap
			if (vector.DistanceSq(clone.GetPosition(), myPos) > maxDistSq)
			{
				m_SFP_SummonedClones.Remove(i);
				GetGame().ObjectDelete(clone);
			}
		}
	}
	
	// Считаем живых клонов, которые недалеко от главного
	protected int SFP_CountNearbyClones()
	{
		if (!m_SFP_SummonedClones)
			return 0;
		
		int count          = 0;
		vector myPos       = GetPosition();
		float  maxDistSq   = SFP_CLONE_NEAR_DISTANCE * SFP_CLONE_NEAR_DISTANCE;
		
		for (int i = 0; i < m_SFP_SummonedClones.Count(); ++i)
		{
			AnimalBase clone = m_SFP_SummonedClones[i];
			if (!clone || clone.IsDamageDestroyed())
				continue;
			
			vector d      = clone.GetPosition() - myPos;
			float  distSq = d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
			
			if (distSq <= maxDistSq)
				count++;
		}
		
		return count;
	}
	
	// Создаём одного клона вокруг главного мутанта
	protected void SFP_SpawnClone()
	{
		if (!GetGame().IsServer())
			return;
		
		vector origin = GetPosition();
		
		float angleDeg = Math.RandomFloatInclusive(0.0, 360.0);
		float angleRad = angleDeg * Math.DEG2RAD;
		float radius   = Math.RandomFloatInclusive(SFP_CLONE_SPAWN_MIN_RADIUS, SFP_CLONE_SPAWN_MAX_RADIUS);
		
		vector offset;
		offset[0] = Math.Cos(angleRad) * radius;
		offset[1] = 0;
		offset[2] = Math.Sin(angleRad) * radius;
		
		vector spawnPos = origin + offset;
		spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);

		// 1.30 port: no AI clone into water or into geometry
		array<Object> exclude = new array<Object>;
		exclude.Insert(this);
		if (!SFP_Safety.IsFreeGroundPos(spawnPos, Vector(0.8, 0.9, 1.4), exclude))
			return;

		// Собираем флаги как в ваниле (достаточно и двух):
		// ECE_PLACE_ON_SURFACE - поставить на землю
		// ECE_INITAI           - проинициализировать AI для животного
		int flags = ECE_PLACE_ON_SURFACE + ECE_INITAI;
		
		// Создаём клона с AI
		EntityAI e = EntityAI.Cast(GetGame().CreateObjectEx(SFP_CLONE_CLASSNAME, spawnPos, flags));
		
		if (e && m_SFP_SummonedClones)
		{
			AnimalBase clone = AnimalBase.Cast(e);
			if (clone)
			{
				m_SFP_SummonedClones.Insert(clone);
			}
		}
	}
}
