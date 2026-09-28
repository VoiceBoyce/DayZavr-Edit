class FOG_Hoodie_Jacket_ColorBase: Toggleable_Sleeves_Shirt_Base 
{
    private void HideUnhideSelection(string selectionName, bool hide = false)
    {
        int index = GetHiddenSelectionIndex(selectionName);

        if (index != -1)
        {
            #ifndef SERVER
            SetSimpleHiddenSelectionState(index, hide);
            #endif
        }
    }

    override int GetSleevesAmount()
	{
		return 2;
	}

	override void HandleSleeves()
    {
        if(GetSleevesState() == 0)
        {   
            SetSimpleHiddenSelectionState(0,true); // Hide Half Cuffs
            SetSimpleHiddenSelectionState(1,false); //Show quarter sleeve
        }
        else
        {   
            SetSimpleHiddenSelectionState(0,false); // Hide Half Cuffs
            SetSimpleHiddenSelectionState(1,true); //Show quarter sleeve
        }
    }
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class FOG_Hoodie_Jacket_Black extends FOG_Hoodie_Jacket_ColorBase {};
class FOG_Hoodie_Jacket_Nike_G extends FOG_Hoodie_Jacket_ColorBase {};
class FOG_Hoodie_Jacket_Patagonia extends FOG_Hoodie_Jacket_ColorBase {};
class FOG_Hoodie_Jacket_FOG extends FOG_Hoodie_Jacket_ColorBase {};
class FOG_Hoodie_Jacket_CDAJ extends FOG_Hoodie_Jacket_ColorBase {};
class FOG_Hoodie_Jacket_MC extends FOG_Hoodie_Jacket_ColorBase {};
class FOG_Hoodie_Jacket_ERDL extends FOG_Hoodie_Jacket_ColorBase {};
class FOG_Hoodie_Jacket_PERDL extends FOG_Hoodie_Jacket_ColorBase {};
class FOG_Hoodie_Jacket_Thrasher extends FOG_Hoodie_Jacket_ColorBase {};
class FOG_Hoodie_Jacket_Supreme extends FOG_Hoodie_Jacket_ColorBase {};
class FOG_Hoodie_Jacket_MLG extends FOG_Hoodie_Jacket_ColorBase {};
