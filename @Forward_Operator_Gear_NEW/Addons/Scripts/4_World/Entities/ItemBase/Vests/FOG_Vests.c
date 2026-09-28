class FOG_Vest_RadioHideBase: Vest_Base 
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
		FOG_HideUnhideSelection("selection_radio", 0);
	}

    override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);
		if (slot_name == "WalkieTalkie")
		{
			FOG_HideUnhideSelection("selection_radio", 1);
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);
		if (slot_name == "WalkieTalkie")
		{
		FOG_HideUnhideSelection("selection_radio", 0);
		}
	}	
};

class FOG_Vest_ALSE_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_Vest_AVS_Base: FOG_Vest_RadioHideBase{}
class FOG_Vest_CPC_Base: FOG_Vest_RadioHideBase{}
class FOG_Vest_FCPC_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_Vest_Gen4_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_Vest_JPC_Base: FOG_Vest_RadioHideBase{}
class FOG_Vest_LBT6094_Base: FOG_Vest_RadioHideBase{}
class FOG_Vest_LV119_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_Vest_MMAC_Base: FOG_Vest_RadioHideBase{}
class FOG_Vest_Osprey_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_Vest_SPC_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_Vest_SSMK4_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_Vest_Thor_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_TV110T_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_Vest_Plateframe_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_Vest_DPC_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_Vest_JPC2_ColorBase: FOG_Vest_RadioHideBase{}
class FOG_Vest_PACA_ColorBase: Vest_Base{}
class FOG_Vest_LVMBAV_ColorBase: Vest_Base{}
class FOG_Vest_D3CRX_ColorBase: Vest_Base{}