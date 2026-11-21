void FoldOpticsKOD (EntityAI ParentItem)
{
		protected int foldingOpticRaisedId = -1;
		protected int foldingOpticLoweredId = -1;
		TStringArray selectionNames = new TStringArray;

		ParentItem.ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		foldingOpticRaisedId = selectionNames.Find("folding_raised");
		foldingOpticLoweredId = selectionNames.Find("folding_lowered");  

		//HideSelection("folding_raised");
		//ShowSelection("folding_lowered");

		ParentItem.SetSimpleHiddenSelectionState(foldingOpticRaisedId,false);
		ParentItem.SetSimpleHiddenSelectionState(foldingOpticLoweredId,true);
			
}

void UnfoldOpticsKOD(EntityAI ParentItem)
{
		protected int foldingOpticRaisedId = -1;
		protected int foldingOpticLoweredId = -1;
		TStringArray selectionNames = new TStringArray;

		ParentItem.ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		foldingOpticRaisedId = selectionNames.Find("folding_raised");
		foldingOpticLoweredId = selectionNames.Find("folding_lowered");  
		//HideSelection("folding_raised");
		//ShowSelection("folding_lowered");

		ParentItem.SetSimpleHiddenSelectionState(foldingOpticRaisedId,true);
		ParentItem.SetSimpleHiddenSelectionState(foldingOpticLoweredId,false);
}

	void HideIronSightsKOD (EntityAI ParentItem)
	{
		protected int hideIronsightsId = -1;
		TStringArray selectionNames = new TStringArray;

		ParentItem.ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideIronsightsId = selectionNames.Find("hide_ironsights");
			
		ParentItem.SetSimpleHiddenSelectionState(hideIronsightsId,false);
	};

	void ShowIronSightsKOD (EntityAI ParentItem)
	{
		protected int hideIronsightsId = -1;
		TStringArray selectionNames = new TStringArray;

		ParentItem.ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideIronsightsId = selectionNames.Find("hide_ironsights");

		ParentItem.SetSimpleHiddenSelectionState(hideIronsightsId,true);
	};
	
	void HideScopeMountKOD (EntityAI ParentItem)
	{
		protected int hideIronsightsId = -1;
		TStringArray selectionNames = new TStringArray;

		ParentItem.ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideIronsightsId = selectionNames.Find("hide_rail");
			
		ParentItem.SetSimpleHiddenSelectionState(hideIronsightsId,false);
	};

	void ShowScopeMountKOD (EntityAI ParentItem)
	{
		protected int hideIronsightsId = -1;
		TStringArray selectionNames = new TStringArray;

		ParentItem.ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideIronsightsId = selectionNames.Find("hide_rail");

		ParentItem.SetSimpleHiddenSelectionState(hideIronsightsId,true);
	};	