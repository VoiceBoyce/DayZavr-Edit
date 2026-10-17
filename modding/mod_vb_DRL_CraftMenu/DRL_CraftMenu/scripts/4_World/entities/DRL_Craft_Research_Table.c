class DRL_Craft_Research_Table : Container_Base
{
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionOpenDRLResearchTable);
		AddAction(ActionDeconstructDRLWorkbench);
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}
}

class DRL_Craft_Research_Table_Static : DRL_Craft_Research_Table
{
	override void SetActions()
	{
		super.SetActions();
		RemoveAction(ActionDeconstructDRLWorkbench);
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}
}