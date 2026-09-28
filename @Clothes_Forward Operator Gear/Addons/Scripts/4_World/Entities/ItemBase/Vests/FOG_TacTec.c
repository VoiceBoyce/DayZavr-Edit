class FOG_Vest_TacTec_Base : Vest_Base
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
		HideUnhideSelection("VestMags_TT", 0);
		HideUnhideSelection("VestRadio_TT", 0);
	}

    override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);
		if (slot_name == "VestPouch")
		{
			HideUnhideSelection("VestMags_TT", 1);
		}
		if (slot_name == "WalkieTalkie")
		{
			HideUnhideSelection("VestRadio_TT", 1);
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);
		if (slot_name == "VestPouch")
		{
		HideUnhideSelection("VestMags_TT", 0);
		}
		if (slot_name == "WalkieTalkie")
		{
		HideUnhideSelection("VestRadio_TT", 0);
		}
	}	
};