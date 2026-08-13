class MSFZ_Magic_Kirka extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(Action_MSFZ_MineRock);
		AddAction(ActionMineTree);
		AddAction(ActionMineTreeBark);
		AddAction(ActionMineBush);
		AddAction(ActionDismantlePart);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionSkinning);
		AddAction(ActionCraftBolts);
	}
};