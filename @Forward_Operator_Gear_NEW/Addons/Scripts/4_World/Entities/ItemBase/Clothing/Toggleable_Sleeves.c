enum SleevesState 
{
	ROLLED = 0, // Default
	UNROLLED = 1,
	HALF = 2
};

class Toggleable_Sleeves_Shirt_Base: Top_Base
{
    protected SleevesState m_SleevesState;

    void Toggleable_Sleeves_Shirt_Base()
    {
		RegisterNetSyncVariableInt("m_SleevesState", 0, 2);
    }

    override void EEInit()
	{
		super.EEInit();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(HandleSleeves);
	}

    void HandleSleeves()
    {

    }

    void SetSleevesState(int state)
	{
		if (!GetGame().IsServer()) {
			Error("SetSleevesState must be called on server");
			return;
		}
				
		m_SleevesState = state;
		SetSynchDirty();
	}

    int GetSleevesState()
	{
		return m_SleevesState;
	}

    int GetSleevesAmount()
	{
		return 3;
	}

    override void OnStoreSave(ParamsWriteContext ctx)
	{   
		super.OnStoreSave(ctx);		
		ctx.Write(m_SleevesState);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		if (!ctx.Read(m_SleevesState))
			m_SleevesState = SleevesState.ROLLED;

        SetSynchDirty();
		return true;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
        
		HandleSleeves();
	}

    override void SetActions()
	{
		super.SetActions();		
		AddAction(ActionToggleSleeves);
	}
};