// INT Quests (server) — kill events (spec.md §4.3). EntityAI.EEKilled calls AnalyticsManagerServer.OnEntityKilled
// on the server for every death (infected, animals, players, PlayerBase AI): one hook for all of them, in a tiny
// class that has not changed since 1.29 (entityai.c:1109, analyticsmanagerserver.c:48). The class lives in 3_Game,
// so it is modded here and the event goes to 4_World through a static hub. While no online player has an active
// kill target, s_Armed is false and the hook costs one bool test. Suicide never counts (the old mod counted a
// survivor kill for shooting yourself: an exploit, not kept).

class INT_QST_SE_Hub
{
	static bool s_Armed;
	// (Man killer, EntityAI victim, string weaponType) — weaponType: what dealt the blow ("" when unknown).
	static ref ScriptInvoker s_OnKill = new ScriptInvoker();
	// (Object killer, EntityAI victim) — a death no player holds the cause of (a thrown grenade ...): 4_World decides
	// (Grenade_Base is not visible here; INT_QST_SE_Grenades, config GrenadeKills).
	static ref ScriptInvoker s_OnOrphanKill = new ScriptInvoker();
	static int s_Kills;			// self-test counter
	// The victim of the last kill event the engine named no killer for (null, or the victim itself): a vanilla animal
	// finished by its own scripted wound damage (ComponentAnimalBleeding, DecreaseHealth without a source, called from
	// DayZAnimal.EEHitBy) dies that way. INT_QST_SE_AnimalKills then gives the kill to the last hit (old mod rule).
	static EntityAI s_Unclaimed;		// weak
	static int s_Reclaimed;			// self-test counter: kills given to the last hit

	// One kill. False = no usable killer (the caller may try the last hit); true = done (counted, or nobody's on purpose).
	static bool CountKill(Object killer, EntityAI target)
	{
		s_Unclaimed = null;
		if (!s_Armed || !target)
			return true;
		EntityAI killerEntity = EntityAI.Cast(killer);
		if (!killerEntity || killerEntity == target)
		{
			s_Unclaimed = target;
			return false;
		}
		Man who = null;
		string weapon = "";
		// as in the old mod: the kill belongs to the player whose hierarchy holds what killed (gun, knife, fists).
		// A vehicle run-over or a placed explosive has no player root and counts for nobody; a thrown grenade counts
		// for who pulled its pin only with the config switch GrenadeKills (server 21's old extension).
		who = killerEntity.GetHierarchyRootPlayer();
		Man killerMan = Man.Cast(killerEntity);
		if (killerMan)
		{
			EntityAI inHands = killerMan.GetEntityInHands();
			if (inHands)
				weapon = inHands.GetType();
		}
		else
		{
			weapon = killerEntity.GetType();
		}
		if (!who)
		{
			s_OnOrphanKill.Invoke(killerEntity, target);
			return true;
		}
		if (who == target)
			return true;
		s_Kills++;
		s_OnKill.Invoke(who, target, weapon);
		return true;
	}
}

modded class AnalyticsManagerServer
{
	override void OnEntityKilled(Object killer, EntityAI target)
	{
		super.OnEntityKilled(killer, target);
		INT_QST_SE_Hub.CountKill(killer, target);
	}
}
