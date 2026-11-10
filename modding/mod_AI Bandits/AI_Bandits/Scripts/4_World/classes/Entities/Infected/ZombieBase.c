modded class ZombieBase
{
	override bool CanBeTargetedByAI(EntityAI ai)
	{

		InfectedBanditBase bandit = InfectedBanditBase.Cast(ai);
		if (bandit && (bandit.IsSniper() || bandit.GetGroup().GetFaction() == "Bandits"))
		{
			return false;
		}
		return super.CanBeTargetedByAI(ai);
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		AI_EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}

	void AI_EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		bool fromBandits = source && source.IsInherited(InfectedBanditBase);
		if (fromBandits)
		{
			float zoneDamage = damageResult.GetDamage(dmgZone, "Health");
			float healBack = zoneDamage * 10;
			if (healBack > 0.0)
			{
				DecreaseHealth("GlobalHealth", "Health", healBack);
			}
		}
	}
}
