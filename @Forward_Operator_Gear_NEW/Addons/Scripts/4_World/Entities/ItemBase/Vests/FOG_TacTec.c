class FOG_Vest_TacTec_Base : Vest_Base
{
	protected void FOG_HideUnhideSelection(string selectionName, int hide = 0)
    {
        TStringArray selectionNames = new TStringArray;
        ConfigGetTextArray("simpleHiddenSelections",selectionNames);
        int selectionId = selectionNames.Find(selectionName);
		if (selectionId != -1)
			SetSimpleHiddenSelectionState(selectionId, hide);
    }
	override void EEInit()
	{
		super.EEInit();
		FOG_HideUnhideSelection("VestMags_TT", 0);
		FOG_HideUnhideSelection("VestRadio_TT", 0);
	}

    override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);
		if (slot_name == "VestPouch")
		{
			FOG_HideUnhideSelection("VestMags_TT", 1);
		}
		if (slot_name == "WalkieTalkie")
		{
			FOG_HideUnhideSelection("VestRadio_TT", 1);
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);
		if (slot_name == "VestPouch")
		{
		FOG_HideUnhideSelection("VestMags_TT", 0);
		}
		if (slot_name == "WalkieTalkie")
		{
		FOG_HideUnhideSelection("VestRadio_TT", 0);
		}
	}	
};