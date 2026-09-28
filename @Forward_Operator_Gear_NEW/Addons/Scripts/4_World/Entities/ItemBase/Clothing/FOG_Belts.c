class FOG_Belt_Base extends Belt_Base
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionWringClothes);
    }
};
class FOG_BISON_Belt_ColorBase : FOG_Belt_Base
{
	private void HideUnhideSelection(string selectionName, int hide = 0)
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
		HideUnhideSelection("selection_holster", 0);
	}

    override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);
		if (slot_name == "FOG_Ronin_Holsterslot")
		{
			HideUnhideSelection("selection_holster", 1);
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);
		if (slot_name == "FOG_Ronin_Holsterslot")
		{
		HideUnhideSelection("selection_holster", 0);
		}
	}
};
class FOG_Belt_RTGFB_Base extends FOG_Belt_Base {};
class FOG_MRB_Belt_ColorBase extends FOG_Belt_Base {};
class FOG_Belt_TacticalFanny_Base extends FOG_Belt_Base {};