// Серверная логика Тролля: призыв Ричаров и удар с толчком.
modded class MSFZ_Troll
{
	protected const int   MSFZ_RICAR_MAX_ALIVE   = 6;      // сколько призванных Ричаров живёт одновременно
	protected const float MSFZ_RICAR_RADIUS      = 10;     // радиус появления вокруг Тролля, м
	protected const float MSFZ_RICAR_RADIUS_UNDERGROUND = 3; // радиус в бункере, м
	protected const float MSFZ_UNDERGROUND_DEPTH = 3;      // Тролль ниже рельефа больше чем на 3 м — значит, он под землёй
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

	// Призвать Ричаров до лимита живых
	protected void MSFZ_SummonRicars()
	{
		if (!GetGame() || !IsAlive() || IsSetForDeletion())
			return;

		for (int i = m_MSFZ_Ricars.Count() - 1; i >= 0; i--)
		{
			EntityAI old = m_MSFZ_Ricars[i];
			if (!old || !old.IsAlive())
				m_MSFZ_Ricars.Remove(i);
		}

		int toSpawn = MSFZ_RICAR_MAX_ALIVE - m_MSFZ_Ricars.Count();
		vector center = GetPosition();

		// Ричары появляются на высоте Тролля. Раньше высота бралась по рельефу (SurfaceY),
		// и Тролль в подземном бункере призывал их на поверхность над бункером.
		// Под землёй радиус меньше, чтобы не ставить Ричаров в стены коридоров.
		bool underground = center[1] < GetGame().SurfaceY(center[0], center[2]) - MSFZ_UNDERGROUND_DEPTH;
		float radius = MSFZ_RICAR_RADIUS;
		if (underground)
			radius = MSFZ_RICAR_RADIUS_UNDERGROUND;

		for (int n = 0; n < toSpawn; n++)
		{
			float x = center[0] + Math.RandomFloat(-radius, radius);
			float z = center[2] + Math.RandomFloat(-radius, radius);
			float y = center[1];
			if (!underground)
				y = GetGame().SurfaceY(x, z); // на поверхности — по рельефу, чтобы не висели над склоном
			vector pos = Vector(x, y, z);
			EntityAI ricar = EntityAI.Cast(GetGame().CreateObject("MSFZ_Ricar", pos, false, true));
			if (ricar)
				m_MSFZ_Ricars.Insert(ricar);
		}
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
