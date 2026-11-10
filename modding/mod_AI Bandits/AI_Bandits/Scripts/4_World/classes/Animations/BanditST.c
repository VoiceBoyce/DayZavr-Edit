class BanditST
{
	//protected int m_Int_InjuryLevel;
	protected int m_CMD_TURN;
    protected int m_Int_Pose;
	protected int m_Int_AnimInstance;
	protected bool m_Bool_Talking;
	protected int m_Int_Emote;
	protected bool m_Bool_Raised;
	protected float m_Float_AimY;
	protected int m_Int_Stance;
	protected bool m_Bool_Look;
	protected float m_Float_LookX;
	protected float m_Float_LookY;
	protected float m_Float_MovementDirection;

	void BanditST( InfectedBanditBase bandit )
	{
		DayZCreatureAnimInterface ai = bandit.GetAnimInterface();
		
		//m_Int_InjuryLevel = ai.BindVariableInt( "InjuryLevel" );
		m_CMD_TURN = ai.BindCommand( "CMD_TURN" );

        m_Int_Pose = ai.BindVariableInt("Pose");
		m_Int_AnimInstance = ai.BindVariableInt("AnimInstance");
		m_Bool_Talking = ai.BindVariableBool("Talking");
		m_Int_Emote = ai.BindVariableInt("Emote");
		m_Bool_Raised = ai.BindVariableBool("Raised");
		m_Float_AimY = ai.BindVariableFloat("AimY");
		m_Int_Stance = ai.BindVariableInt("Stance");
		m_Bool_Look = ai.BindVariableBool("Look");
		m_Float_LookX = ai.BindVariableFloat("LookDirX");
		m_Float_LookY = ai.BindVariableFloat("LookDirY");
		m_Float_MovementDirection = ai.BindVariableFloat("MovementDirection");
		
	}
	/*	
	void SetInjuryLevelM( DayZInfectedCommandMoveModdable script, int param )
	{
		script.PreAnim_SetInt( m_Int_InjuryLevel, param );
	}
	*/
	void CallTurnS( DayZInfectedCommandScript script, int param1, float param2 )
	{
		script.PreAnim_CallCommand( m_CMD_TURN, param1, param2 );
	}
	void CallTurnM( DayZInfectedCommandMoveModdable script, int param1, float param2 )
	{
		script.PreAnim_CallCommand( m_CMD_TURN, param1, param2 );
	}
	
    void SetPoseM( DayZInfectedCommandMoveModdable script, int param )
	{
		script.PreAnim_SetInt( m_Int_Pose, param );
	}
	
	void SetAnimInstanceM( DayZInfectedCommandMoveModdable script, int param )
	{
		script.PreAnim_SetInt( m_Int_AnimInstance, param );
	}
	
	void SetAnimInstanceS( DayZInfectedCommandScript script, int param )
	{
		script.PreAnim_SetInt( m_Int_AnimInstance, param );
	}
	
	void SetTalkingM( DayZInfectedCommandMoveModdable script, bool param )
	{
		script.PreAnim_SetBool( m_Bool_Talking, param );
	}
	
	void SetTalkingS( DayZInfectedCommandScript script, bool param )
	{
		script.PreAnim_SetBool( m_Bool_Talking, param );
	}
	
	void SetEmoteM( DayZInfectedCommandMoveModdable script, int param )
	{
		script.PreAnim_SetInt( m_Int_Emote, param );
	}
	
	void SetEmoteS( DayZInfectedCommandScript script, int param )
	{
		script.PreAnim_SetInt( m_Int_Emote, param );
	}
	
	void SetRaisedM( DayZInfectedCommandMoveModdable script, bool param )
	{
		script.PreAnim_SetBool( m_Bool_Raised, param );
	}
	
	void SetRaisedS( DayZInfectedCommandScript script, bool param )
	{
		script.PreAnim_SetBool( m_Bool_Raised, param );
	}
	
	void SetAimYM( DayZInfectedCommandMoveModdable script, float param )
	{
		script.PreAnim_SetFloat( m_Float_AimY, param );
	}
	
	void SetAimYS( DayZInfectedCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_Float_AimY, param );
	}
	
	void SetStanceM( DayZInfectedCommandMoveModdable script, int param )
	{
		script.PreAnim_SetInt( m_Int_Stance, param );
	}
	
	void SetStanceS( DayZInfectedCommandScript script, int param )
	{
		script.PreAnim_SetInt( m_Int_Stance, param );
	}
	
	void SetLookM( DayZInfectedCommandMoveModdable script, bool param )
	{
		script.PreAnim_SetBool( m_Bool_Look, param );
	}
	
	void SetLookXM( DayZInfectedCommandMoveModdable script, float param )
	{
		script.PreAnim_SetFloat( m_Float_LookX, param );
	}
	void SetLookYM( DayZInfectedCommandMoveModdable script, float param )
	{
		script.PreAnim_SetFloat( m_Float_LookY, param );
	}
	
	void SetMovementDirectionS( DayZInfectedCommandScript script, float param )
	{
		script.PreAnim_SetFloat( m_Float_MovementDirection, param );
	}
	
	void SetMovementDirectionM( DayZInfectedCommandMoveModdable script, float param )
	{
		script.PreAnim_SetFloat( m_Float_MovementDirection, param );
	}

};