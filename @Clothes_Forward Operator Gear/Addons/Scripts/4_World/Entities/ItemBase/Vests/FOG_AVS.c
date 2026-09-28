class FOG_Vest_AVS_Base : Vest_Base
{
	private void HideUnhideSelection(string selectionName, bool hide = false)
    {
        TStringArray selectionNames = new TStringArray;
        ConfigGetTextArray("simpleHiddenSelections",selectionNames);
        int selectionId = selectionNames.Find(selectionName);
        SetSimpleHiddenSelectionState(selectionId, hide);
    }
	override void EEInit()
	{
		super.EEInit();
		HideUnhideSelection("radioselection", 0);
	}

    override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);
		if (slot_name == "WalkieTalkie")
		{
			HideUnhideSelection("radioselection", 1);
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);
		if (slot_name == "WalkieTalkie")
		{
		HideUnhideSelection("radioselection", 0);
		}
	}	
};