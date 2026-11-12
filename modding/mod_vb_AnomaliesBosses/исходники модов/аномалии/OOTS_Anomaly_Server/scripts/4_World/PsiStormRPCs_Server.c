modded class PsiStorm_Set_Phase
{
	override protected void SetPhase()
	{
		GetBlowOutManager().SetPhase(GetPhaseSet(), true);
	}
}

modded class PsiStorm_Set_Warning
{
	override protected int GetPhaseSet()
	{
		return BlowOutPhases.WARNING;
	}
}

modded class PsiStorm_Set_Active
{
	override protected int GetPhaseSet()
	{
		return BlowOutPhases.ACTIVE;
	}
}

modded class PsiStorm_Set_Final
{
	override protected int GetPhaseSet()
	{
		return BlowOutPhases.END;
	}
}

