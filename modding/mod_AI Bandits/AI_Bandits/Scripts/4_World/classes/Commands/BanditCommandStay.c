class BanditCommandStay extends DayZInfectedCommandScript
{
	protected InfectedBanditBase m_Bandit;
	protected BanditST m_ST;
	protected vector m_BanditPosition;

	void BanditCommandStay( DayZInfected pInfected )
	{
		m_Bandit = InfectedBanditBase.Cast(pInfected);
		m_ST = m_Bandit.GetAnimST();
	}

	void ~BanditCommandStay()
	{
	}
	
	override void OnActivate()
	{
		m_BanditPosition = m_Bandit.GetPosition();
	}

	override void OnDeactivate()
	{

	}

	override void PreAnimUpdate( float pDt )
	{		
		m_ST.SetEmoteS(this, m_Bandit.GetEmotetoPlay());
		m_ST.SetTalkingS(this, m_Bandit.ShouldTalkNPC());
		m_ST.SetAnimInstanceS(this, m_Bandit.GetAnimInstance());
	}

	override void PrePhysUpdate( float pDt )
	{
		
		SetRotation( vector.Zero );
		PrePhys_SetTranslation( vector.Zero );
		m_Bandit.SetPosition(m_BanditPosition);
	}
	
	override bool PostPhysUpdate( float pDt )
	{		
		return true;
	}
	
	void AnglesToQuat( vector angles, out float[] quat )
	{
		vector rotationMatrix[3];
		Math3D.YawPitchRollMatrix( angles, rotationMatrix );
		Math3D.MatrixToQuat( rotationMatrix, quat );
	}
	
	void SetRotation( vector angles )
	{
		float rotation[4];
		AnglesToQuat( angles, rotation );
		PrePhys_SetRotation( rotation );
	}
};