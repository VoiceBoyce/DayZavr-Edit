// Состояния существ от изделий «Древнего корня» (только сервер). Существа — зомби (ZombieBase) и звери
// (AnimalBase): обычные, стражи рощи, мутанты MSFZ. Игроков это не касается.
//   Сон — «Пыльцевая ловушка»: ИИ стоит (AIAgent.SetKeepInIdle, скорость 0) VB_AR_SporeTrap.SLEEP_MS. Удар будит.
//   Терновая рана — «Терновый клинок»: VB_AR_Blade.BLEED_MS раз в секунду минус BLEED_DPS здоровья и шаг вместо бега
//   (OverrideMovementSpeed: зомби 1 — шаг, зверю 2 — шаг; шкала зверей из ванили DayZAnimal.c, зомби 0..3).
//   Кормление клинка: убил клинком или его раной (смерть без убийцы не позже KILL_MS после удара).
//   Крупные (GetMaxHealth("", "Health") > BIG_HP: Хранитель рощи, боссы VB_AnomaliesBosses, крупные мутанты) не засыпают
//   и не переходят на шаг — урон раны идёт. Снимаем только те оверрайды (SetKeepInIdle, OverrideMovementSpeed), что
//   поставили сами: свои ограничения скорости у чужих скриптов (боссы, нашествие) не трогаем.
// Состояние — отдельный объект у существа (создаётся при первом сне или ране): DayZCreatureAI объявлен в 3_Game,
// и modded его из 4_World не делаем — поля живут в modded ZombieBase и AnimalBase, логика общая здесь.

class VB_AR_CreatureState
{
	static const int KILL_MS = 10000;		// смерть без убийцы позже — уже не от клинка (рана идёт BLEED_MS)
	static const float ZOMBIE_SLOW = 1.0;
	static const float ANIMAL_SLOW = 2.0;
	static const float BIG_HP = 1000.0;		// крупнее — сон и шаг не действуют

	protected DayZCreatureAI m_VB_Owner;	// не ref: удалённое существо становится null
	protected int m_VB_SleepUntil;			// g_Game.GetTime(); 0 — не спит
	protected int m_VB_ThornUntil;			// 0 — раны нет
	protected VB_AR_Blade m_VB_Blade;		// последний удар клинком
	protected PlayerBase m_VB_BladeOwner;
	protected int m_VB_BladeHitMs;
	protected bool m_VB_IdleSet;			// мы включили SetKeepInIdle — нам и выключать
	protected bool m_VB_SpeedSet;			// мы включили OverrideMovementSpeed — нам и снимать

	void VB_AR_CreatureState(DayZCreatureAI owner)
	{
		m_VB_Owner = owner;
	}

	// ---------------- доступ ----------------

	static VB_AR_CreatureState VB_Of(DayZCreatureAI c, bool create)
	{
		ZombieBase z = ZombieBase.Cast(c);
		if (z)
			return z.VB_AR_StateOf(create);
		AnimalBase a = AnimalBase.Cast(c);
		if (a)
			return a.VB_AR_StateOf(create);
		return null;
	}

	static bool VB_SleepServer(DayZCreatureAI c, int ms)
	{
		VB_AR_CreatureState st = VB_Of(c, true);
		if (!st)
			return false;
		return st.VB_Sleep(ms);
	}

	// кто бил Терновым клинком: source — сам нож (удар оружием) или игрок с ним в руках
	static VB_AR_Blade VB_BladeOf(EntityAI source, out PlayerBase owner)
	{
		owner = null;
		if (!source)
			return null;

		VB_AR_Blade blade = VB_AR_Blade.Cast(source);
		if (blade)
		{
			owner = PlayerBase.Cast(blade.GetHierarchyRootPlayer());
			return blade;
		}

		owner = PlayerBase.Cast(source);
		if (owner)
			blade = VB_AR_Blade.Cast(owner.GetHumanInventory().GetEntityInHands());
		return blade;
	}

	// крупное существо: сон и шаг на него не действуют
	static bool VB_IsBig(DayZCreatureAI c)
	{
		return c && c.GetMaxHealth("", "Health") > BIG_HP;
	}

	// спит от ловушки (нашествие пауков не тянет спящего к роще — VB_AR_WorldSiege.c)
	static bool VB_AsleepServer(DayZCreatureAI c)
	{
		VB_AR_CreatureState st = VB_Of(c, false);
		return st && st.m_VB_SleepUntil > 0;
	}

	// терновая рана держит на шаге (у крупных рана без шага — false)
	static bool VB_SlowedServer(DayZCreatureAI c)
	{
		VB_AR_CreatureState st = VB_Of(c, false);
		return st && st.m_VB_ThornUntil > 0 && !VB_IsBig(c);
	}

	// ---------------- события существа ----------------

	static void VB_OnHit(DayZCreatureAI c, int damageType, EntityAI source)
	{
		VB_AR_CreatureState st = VB_Of(c, false);
		if (st)
			st.VB_Wake();			// любой удар будит

		if (damageType != DamageType.CLOSE_COMBAT || !c.IsAlive())
			return;
		PlayerBase owner;
		VB_AR_Blade blade = VB_BladeOf(source, owner);
		if (!blade || !owner || blade.IsRuined())
			return;

		if (!st)
			st = VB_Of(c, true);
		if (st)
			st.VB_Thorn(blade, owner);
	}

	static void VB_OnKilled(DayZCreatureAI c, Object killer)
	{
		VB_AR_CreatureState st = VB_Of(c, false);
		PlayerBase owner;
		VB_AR_Blade blade = VB_BladeOf(EntityAI.Cast(killer), owner);
		if (!blade && st && (!killer || killer == c))
			blade = st.VB_RecentBlade(owner);
		if (st)
			st.VB_Stop();
		if (blade && owner && owner.IsAlive())
			blade.VB_OnKillServer(owner, c.GetType());
	}

	// ---------------- сон ----------------

	// крупных не усыпляет (false — не уснул)
	bool VB_Sleep(int ms)
	{
		if (!m_VB_Owner || !m_VB_Owner.IsAlive() || VB_IsBig(m_VB_Owner))
			return false;
		AIAgent ai = m_VB_Owner.GetAIAgent();
		if (!ai)
			return false;

		m_VB_SleepUntil = g_Game.GetTime() + ms;
		ai.SetKeepInIdle(true);
		m_VB_IdleSet = true;
		VB_ApplySpeed();
		ScriptCallQueue q = g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		q.Remove(VB_Wake);
		q.CallLater(VB_Wake, ms, false);
		return true;
	}

	void VB_Wake()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Wake);
		if (m_VB_SleepUntil <= 0)
			return;
		m_VB_SleepUntil = 0;
		if (!m_VB_Owner || !m_VB_Owner.IsAlive())
			return;
		if (m_VB_IdleSet)
		{
			m_VB_IdleSet = false;
			AIAgent ai = m_VB_Owner.GetAIAgent();
			if (ai)
				ai.SetKeepInIdle(false);
		}
		VB_ApplySpeed();
	}

	// ---------------- терновая рана ----------------

	void VB_Thorn(VB_AR_Blade blade, PlayerBase owner)
	{
		m_VB_Blade = blade;
		m_VB_BladeOwner = owner;
		m_VB_BladeHitMs = g_Game.GetTime();
		if (!m_VB_Owner || !m_VB_Owner.IsAlive())
			return;

		bool had = m_VB_ThornUntil > 0;
		m_VB_ThornUntil = g_Game.GetTime() + VB_AR_Blade.BLEED_MS;
		if (had)
			return;				// рана уже идёт — только продлили
		VB_ApplySpeed();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_ThornTick, 1000, true);
	}

	void VB_ThornTick()
	{
		if (!m_VB_Owner || !m_VB_Owner.IsAlive() || g_Game.GetTime() >= m_VB_ThornUntil)
		{
			VB_ThornEnd();
			return;
		}
		m_VB_Owner.AddHealth("", "Health", -VB_AR_Blade.BLEED_DPS);	// может убить: тогда VB_OnKilled -> VB_Stop
	}

	void VB_ThornEnd()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_ThornTick);
		if (m_VB_ThornUntil <= 0)
			return;
		m_VB_ThornUntil = 0;
		VB_ApplySpeed();
	}

	VB_AR_Blade VB_RecentBlade(out PlayerBase owner)
	{
		owner = null;
		if (!m_VB_Blade || g_Game.GetTime() - m_VB_BladeHitMs > KILL_MS)
			return null;
		owner = m_VB_BladeOwner;
		return m_VB_Blade;
	}

	// ---------------- общее ----------------

	// скорость: спит — стоит, ранен — шагом (крупные — нет), иначе как решит ИИ; снимаем только свой оверрайд
	protected void VB_ApplySpeed()
	{
		if (!m_VB_Owner)
			return;
		bool sleep = m_VB_SleepUntil > 0;
		bool walk = m_VB_ThornUntil > 0 && !VB_IsBig(m_VB_Owner);
		if (!sleep && !walk && !m_VB_SpeedSet)
			return;					// не ставили — не трогаем (чужой оверрайд скорости останется)
		DayZCreatureAIInputController ic;
		float slow = ZOMBIE_SLOW;
		DayZInfected inf = DayZInfected.Cast(m_VB_Owner);
		if (inf)
		{
			ic = inf.GetInputController();
		}
		else
		{
			DayZAnimal an = DayZAnimal.Cast(m_VB_Owner);
			if (an)
				ic = an.GetInputController();
			slow = ANIMAL_SLOW;
		}
		if (!ic)
			return;

		if (sleep)
		{
			ic.OverrideMovementSpeed(true, 0);
			m_VB_SpeedSet = true;
		}
		else if (walk)
		{
			ic.OverrideMovementSpeed(true, slow);
			m_VB_SpeedSet = true;
		}
		else
		{
			ic.OverrideMovementSpeed(false, 0);
			m_VB_SpeedSet = false;
		}
	}

	void VB_Stop()
	{
		if (!g_Game)
			return;
		ScriptCallQueue q = g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		q.Remove(VB_Wake);
		q.Remove(VB_ThornTick);
		m_VB_SleepUntil = 0;
		m_VB_ThornUntil = 0;
	}
}

modded class ZombieBase
{
	protected ref VB_AR_CreatureState m_VB_AR_State;

	VB_AR_CreatureState VB_AR_StateOf(bool create)
	{
		if (!m_VB_AR_State && create)
			m_VB_AR_State = new VB_AR_CreatureState(this);
		return m_VB_AR_State;
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (g_Game.IsServer())
			VB_AR_CreatureState.VB_OnHit(this, damageType, source);
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (g_Game.IsServer())
			VB_AR_CreatureState.VB_OnKilled(this, killer);
	}

	override void EEDelete(EntityAI parent)
	{
		if (m_VB_AR_State)
			m_VB_AR_State.VB_Stop();
		super.EEDelete(parent);
	}
}

modded class AnimalBase
{
	protected ref VB_AR_CreatureState m_VB_AR_State;

	VB_AR_CreatureState VB_AR_StateOf(bool create)
	{
		if (!m_VB_AR_State && create)
			m_VB_AR_State = new VB_AR_CreatureState(this);
		return m_VB_AR_State;
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (g_Game.IsServer())
			VB_AR_CreatureState.VB_OnHit(this, damageType, source);
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (g_Game.IsServer())
			VB_AR_CreatureState.VB_OnKilled(this, killer);
	}

	override void EEDelete(EntityAI parent)
	{
		if (m_VB_AR_State)
			m_VB_AR_State.VB_Stop();
		super.EEDelete(parent);
	}
}
