// INT Quests (server) — grenade kills (config GrenadeKills, default 0). Folded in from server 21's old extension
// RF_KASTETQUESTS_EXTENDED (owner 2026-10-22: «добавь этот функционал в наш мод, расширение уберём»): a grenade remembers
// the player who pulled its pin (Pin() forgets him) or who set it off with a shot; when it kills, the kill counts for
// that player. A grenade still in someone's hands is an ordinary kill of the holder (INT_QST_SE_Hub), so only thrown /
// dropped ones come here. Unlike the extension (infected only) any victim counts — server 21's quests aim at infected
// anyway; the owner himself never counts (suicide rule).

modded class Grenade_Base
{
	protected PlayerBase m_INT_QST_Owner;		// weak

	override void Unpin()
	{
		super.Unpin();
		if (GetGame().IsDedicatedServer())
			m_INT_QST_Owner = PlayerBase.Cast(GetHierarchyRootPlayer());
	}

	override void Pin()
	{
		super.Pin();
		m_INT_QST_Owner = null;
	}

	override void EEKilled(Object killer)
	{
		// set off by a shot / a blow: the shooter owns the blast (as the extension did)
		if (GetGame().IsDedicatedServer() && killer)
		{
			EntityAI ke = EntityAI.Cast(killer);
			if (ke && (ke.IsWeapon() || ke.IsMeleeWeapon()))
			{
				PlayerBase shooter = PlayerBase.Cast(ke.GetHierarchyRootPlayer());
				if (shooter)
					m_INT_QST_Owner = shooter;
			}
		}
		super.EEKilled(killer);
	}

	PlayerBase INT_QST_Owner()
	{
		return m_INT_QST_Owner;
	}
}

class INT_QST_SE_Grenades
{
	static int s_Counted;		// self-test counter

	static void OnOrphanKill(Object killer, EntityAI victim)
	{
		if (!victim || INT_QST_SE_Store.s_Config.GrenadeKills != 1)
			return;
		Grenade_Base g = Grenade_Base.Cast(killer);
		if (!g)
			return;
		PlayerBase owner = g.INT_QST_Owner();
		if (!owner || owner == victim)
			return;
		s_Counted++;
		string gType = g.GetType();
		INT_QST_SE_Progress.OnKill(owner, victim, gType);
	}
}
