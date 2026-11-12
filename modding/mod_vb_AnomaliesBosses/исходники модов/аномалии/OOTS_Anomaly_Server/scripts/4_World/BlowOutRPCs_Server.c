modded class BlowOut_Set_Phase
{
	override void EEInit()
	{
		super.EEInit();
		
		CreateVisionObject(GetPosition());
		
		SetPhase();
	}
	
	protected void SetPhase()
	{
		GetBlowOutManager().SetPhase(GetPhaseSet());
	}
	
	protected int GetPhaseSet()
	{
		return BlowOutPhases.COOLDOWN;
	}
	
	private void CreateVisionObject(vector pos)
	{
		Object v_object = GetGame().CreateObject("SmallStone", pos, false);
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( RemoveVisionObject, 3000, false, v_object);
	}
	
	private void RemoveVisionObject(Object v_object)
	{
		GetGame().ObjectDelete(v_object);
		GetGame().ObjectDelete(this);
	}
}
modded class BlowOut_Set_Warning
{
	override protected int GetPhaseSet()
	{
		return BlowOutPhases.WARNING;
	}
}
modded class BlowOut_Set_Active
{
	override protected int GetPhaseSet()
	{
		return BlowOutPhases.ACTIVE;
	}
}
modded class BlowOut_Set_Final
{
	override protected int GetPhaseSet()
	{
		return BlowOutPhases.END;
	}
}