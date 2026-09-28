class FOG_Helmet_FAST_Bump_Base : FOG_Helmet_Highcut_Base
{
	// mountbase selection used for the wire, mountbase mesh no longer exists
	private void HideUnhideSelection(string selectionName, bool hide = false)
    {
        TStringArray selectionNames = new TStringArray;
        ConfigGetTextArray("simpleHiddenSelections",selectionNames);
        int selectionId = selectionNames.Find(selectionName);
		if (selectionId > -1)
			SetSimpleHiddenSelectionState(selectionId, hide);
    }
	override void EEInit()
	{
		super.EEInit();
		HideUnhideSelection("mountbase", false);
	}

    override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);
		if (slot_name == "NVG")
		{
			HideUnhideSelection("mountbase", true);
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);
		if (slot_name == "NVG")
		{
		HideUnhideSelection("mountbase", false);
		}
	}
};