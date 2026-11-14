// INT Quests (server) — animal kills the engine names no killer for (spec.md §4.3). A vanilla animal takes a hit twice:
// the engine damage, then DayZAnimal.EEHitBy runs ComponentAnimalBleeding.CreateWound, which takes the same health
// again with DecreaseHealth and no source (componentanimalbleeding.c:36, dayzanimal.c:846). When that second part
// finishes the animal, EEKilled gets no player, so sheep, pigs, cows, boars, deer (280-500 HP) were counted for
// nobody, while foxes, hens and wolves (killed by the first part) were (server 19, 2026-11-01). The old mod
// (LightAI.c, modded AnimalBase) gave such a kill to the last hit; this does the same, but only when the engine
// named no killer: a wolf, a car or an explosive that really killed it still counts for nobody.

modded class AnimalBase
{
	protected EntityAI m_INT_QST_LastHit;		// weak: what dealt the last hit (gun, knife, fists = the player)
	protected int m_INT_QST_DeadUnclaimedMs;	// > 0: died with no killer and no hit before it yet

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		// set before super: the scripted wound damage inside super may kill the animal right away
		if (source)
			m_INT_QST_LastHit = source;
		// the engine reported the death before this hit (the killing hit itself): count it now, once, and only right
		// after the death (shooting a corpse later must not count)
		if (m_INT_QST_DeadUnclaimedMs > 0 && source)
		{
			int since = GetGame().GetTime() - m_INT_QST_DeadUnclaimedMs;
			m_INT_QST_DeadUnclaimedMs = 0;
			if (since >= 0 && since <= 1000)
				INT_QST_Reclaim(source);
		}
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		// AnalyticsManagerServer.OnEntityKilled (inside super) found no usable killer for this animal
		if (INT_QST_SE_Hub.s_Unclaimed != this)
			return;
		INT_QST_SE_Hub.s_Unclaimed = null;
		if (m_INT_QST_LastHit)
		{
			INT_QST_Reclaim(m_INT_QST_LastHit);
			return;
		}
		m_INT_QST_DeadUnclaimedMs = GetGame().GetTime();
		if (m_INT_QST_DeadUnclaimedMs <= 0)
			m_INT_QST_DeadUnclaimedMs = 1;
	}

	protected void INT_QST_Reclaim(EntityAI source)
	{
		int before = INT_QST_SE_Hub.s_Kills;
		INT_QST_SE_Hub.CountKill(source, this);
		INT_QST_SE_Hub.s_Unclaimed = null;
		if (INT_QST_SE_Hub.s_Kills > before)
			INT_QST_SE_Hub.s_Reclaimed++;
	}
}
