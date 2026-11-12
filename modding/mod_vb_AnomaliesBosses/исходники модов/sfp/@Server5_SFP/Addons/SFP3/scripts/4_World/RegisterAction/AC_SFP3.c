modded class ActionConstructor
{
	
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		// SingleUse Actions	
		actions.Insert(ActionKsivaProvide);
		actions.Insert(ActionOsmotrItem);
		//actions.Insert(ActionBatCaseOpen);
		actions.Insert(ActionKarambaKrut);
		actions.Insert(ActionSkeletonKrut);
		actions.Insert(ActionChetki);
		actions.Insert(ActionSmokSmokers);
		//actions.Insert(ActionWaterPackOpen);
		actions.Insert(ActionKartonPackOpen);
		actions.Insert(ActionPhoneNabor);
        actions.Insert(ActionCraftJuice);
		//actions.Insert(ActionInspectGrave);
	}
};