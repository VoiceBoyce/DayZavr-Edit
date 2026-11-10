modded class CarScript
{
	override bool CanBeTargetedByAI(EntityAI ai)
	{
		
		InfectedBanditBase bandit = InfectedBanditBase.Cast(ai);	
		if (bandit && (!bandit.IsSniper() && bandit.GetGroup().GetFaction() == "Guards"))
		{
			return false;
		}
		return super.CanBeTargetedByAI( ai );
	}

}