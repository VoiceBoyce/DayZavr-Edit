class FOG_HelmetHeadset_Base: ItemBase
{
	protected ref OpenableBehaviour m_Openable;
	
	void FOG_HelmetHeadset_Base()
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
			SetAnimationPhase("arc_rotate", 0.0);
		}
		else
		{
			SetAnimationPhase("arc_rotate", 1.0);
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
class FOG_Comtac3_Headphones_Base: FOG_HelmetHeadset_Base{};
class FOG_Comtac_IV_Base: FOG_HelmetHeadset_Base{};
class FOG_M32_Helmet_ColorBase: FOG_HelmetHeadset_Base{};