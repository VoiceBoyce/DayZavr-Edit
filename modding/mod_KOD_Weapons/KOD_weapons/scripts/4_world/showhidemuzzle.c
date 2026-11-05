	void HideMuzzleKOD (EntityAI ParentItem)
	{
		protected int hideMuzzleId = -1;
		TStringArray selectionNames = new TStringArray;

		ParentItem.ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideMuzzleId = selectionNames.Find("hide_muzzle");
			
		ParentItem.SetSimpleHiddenSelectionState(hideMuzzleId,false);
	};

	void ShowMuzzleKOD (EntityAI ParentItem)
	{
		protected int hideMuzzleId = -1;
		TStringArray selectionNames = new TStringArray;

		ParentItem.ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideMuzzleId = selectionNames.Find("hide_muzzle");

		ParentItem.SetSimpleHiddenSelectionState(hideMuzzleId,true);
	};