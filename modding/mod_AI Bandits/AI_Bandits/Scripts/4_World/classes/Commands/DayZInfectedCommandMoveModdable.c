modded class DayZInfectedCommandMoveModdable
{
	
	override void PreAnimUpdate( float pDt )
	{
		InfectedBanditBase bandit = InfectedBanditBase.Cast(GetEntity());
		if (bandit && bandit.GetAnimST())
		{
			bandit.GetAnimST().SetPoseM(this, bandit.GetPose());
			bandit.GetAnimST().SetAnimInstanceM(this, bandit.GetAnimInstance());
			bandit.GetAnimST().SetTalkingM(this, bandit.ShouldTalkNPC());
			bandit.GetAnimST().SetEmoteM(this, bandit.GetEmotetoPlay());
			bandit.GetAnimST().SetRaisedM(this, bandit.GetRaised());
			bandit.GetAnimST().SetAimYM(this, bandit.GetAimY());
			bandit.GetAnimST().SetStanceM(this, bandit.GetStance());
			if (bandit.GetLookX() !=0 || bandit.GetLookY() !=0)
			{
				bandit.GetAnimST().SetLookM(this, true);
				bandit.GetAnimST().SetLookXM(this, bandit.GetLookX());
				bandit.GetAnimST().SetLookYM(this, bandit.GetLookY());
			}
			
			
			bandit.GetAnimST().SetMovementDirectionM(this, bandit.GetMovementDirection());
		}
		super.PreAnimUpdate( pDt );	
	}
	
}