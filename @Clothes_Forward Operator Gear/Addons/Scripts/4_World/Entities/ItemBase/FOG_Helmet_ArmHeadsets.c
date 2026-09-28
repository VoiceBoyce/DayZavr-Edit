class FOG_HelmetArmHeadset_Base: ItemBase
{
	protected ref OpenableBehaviour m_Openable;
	
	void FOG_HelmetArmHeadset_Base()
	{
		m_Openable = new OpenableBehaviour(false);
		
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
		
		UpdateVisualState();
	}

	override void Open()
	{
		m_Openable.Open();
		SetSynchDirty();

		UpdateVisualState();
	}

	override void Close()
	{
		m_Openable.Close();
		SetSynchDirty();

		UpdateVisualState();
	}
	
	override bool IsOpen()
	{
		return m_Openable.IsOpened();
	}

	protected void UpdateVisualState()
	{
		if ( IsOpen() )
		{
			SetAnimationPhase("arms", 0.0);
		}
		else
		{
			SetAnimationPhase("arms", 1.0);
		}
	}
	
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        UpdateVisualState();
    }
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
		AddAction(ActionClose);
	}   
}
class FOG_AMP_ColorBase: FOG_HelmetArmHeadset_Base{};
class FOG_FastMT_RAC_Base: FOG_HelmetArmHeadset_Base{};