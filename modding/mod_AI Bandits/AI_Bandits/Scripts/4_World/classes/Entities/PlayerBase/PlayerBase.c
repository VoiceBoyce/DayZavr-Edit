modded class PlayerBase
{
	override bool CanBeTargetedByAI(EntityAI ai)
	{
		InfectedBanditBase bandit = InfectedBanditBase.Cast(ai);
		if (bandit && bandit.GetPose() > 0)
		{
			return false;
		}

		if (bandit && bandit.GetGroup().GetFaction() == "Guards" && !IsRaised())
		{
			return false;
		}

		return super.CanBeTargetedByAI(ai);
	}
}
