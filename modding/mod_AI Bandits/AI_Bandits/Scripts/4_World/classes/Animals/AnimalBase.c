modded class AnimalBase
{
	override bool CanBeTargetedByAI(EntityAI ai)
	{

		InfectedBanditBase bandit = InfectedBanditBase.Cast(ai);
		if (bandit && !this.IsDanger())
		{
			return false;
		}
		return super.CanBeTargetedByAI(ai);
	}
}