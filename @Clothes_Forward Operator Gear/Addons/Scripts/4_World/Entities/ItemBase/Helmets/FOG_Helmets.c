class FOG_Helmet_Base : Mich2001Helmet
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
		HideUnhideSelection("mountbase", 0);
	}

    override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);
		if (slot_name == "NVG")
		{
			HideUnhideSelection("mountbase", 1);
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);
		if (slot_name == "NVG")
		{
		HideUnhideSelection("mountbase", 0);
		}
	}

	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionRotateComtacs);
        AddAction(ActionRotateArmHeadsets);
        AddAction(ActionToggleNVGPosition);
		AddAction(ActionTurnOnHelmetLight);
        AddAction(ActionTurnOffHelmetLight);
	} 
};