class DRL_Craft_Research_Book : Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionOpenDRLResearchTable);
		AddAction(ActionOpenDRLResearchBookInHands);
	}
}
