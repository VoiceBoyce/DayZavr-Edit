modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		
		actions.Insert(ActionTogglePlacePsiTower);
		actions.Insert(ActionPlacePsiTower);
		actions.Insert(ActionDecodePsiTower);
	}
}