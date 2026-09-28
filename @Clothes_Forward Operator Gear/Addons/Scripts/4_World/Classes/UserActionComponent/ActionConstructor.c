modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		
		actions.Insert(ActionToggleSleeves);
        actions.Insert(ActionRotateComtacs);
        actions.Insert(ActionRotateArmHeadsets);
        actions.Insert(ActionToggleNVGPosition);
		actions.Insert(ActionTurnOnHelmetLight);
        actions.Insert(ActionTurnOffHelmetLight);
	}
};