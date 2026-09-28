class FOG_BISON_Belt_ColorBase : Belt_Base
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
class FOG_BISON_Belt_CB extends FOG_BISON_Belt_ColorBase {};
class FOG_BISON_Belt_RG extends FOG_BISON_Belt_ColorBase {};
class FOG_BISON_Belt_Black extends FOG_BISON_Belt_ColorBase {};
class FOG_BISON_Belt_MC extends FOG_BISON_Belt_ColorBase {};
class FOG_BISON_Belt_MCB extends FOG_BISON_Belt_ColorBase {};

class FOG_Belt_RTGFB_Base extends Belt_Base {};
class FOG_Belt_RTGFB_CB extends FOG_Belt_RTGFB_Base {};
class FOG_Belt_RTGFB_MC extends FOG_Belt_RTGFB_Base {};
class FOG_Belt_RTGFB_RG extends FOG_Belt_RTGFB_Base {};
class FOG_Belt_RTGFB_Black extends FOG_Belt_RTGFB_Base {};
class FOG_Belt_RTGFB_AOR1 extends FOG_Belt_RTGFB_Base {};
class FOG_Belt_RTGFB_AOR2 extends FOG_Belt_RTGFB_Base {};
class FOG_Belt_RTGFB_ALP extends FOG_Belt_RTGFB_Base {};

class FOG_MRB_Belt_ColorBase extends Belt_Base {};
class FOG_MRB_Belt_CB extends FOG_MRB_Belt_ColorBase {};
class FOG_MRB_Belt_MC extends FOG_MRB_Belt_ColorBase {};
class FOG_MRB_Belt_MCB extends FOG_MRB_Belt_ColorBase {};
class FOG_MRB_Belt_RG extends FOG_MRB_Belt_ColorBase {};
class FOG_MRB_Belt_AOR1 extends FOG_MRB_Belt_ColorBase {};
class FOG_MRB_Belt_AOR2 extends FOG_MRB_Belt_ColorBase {};
class FOG_MRB_Belt_ATACSAU extends FOG_MRB_Belt_ColorBase {};
class FOG_MRB_Belt_Black extends FOG_MRB_Belt_ColorBase {};
class FOG_MRB_Belt_6CD extends FOG_MRB_Belt_ColorBase {};
class FOG_MRB_Belt_MCT extends FOG_MRB_Belt_ColorBase {};