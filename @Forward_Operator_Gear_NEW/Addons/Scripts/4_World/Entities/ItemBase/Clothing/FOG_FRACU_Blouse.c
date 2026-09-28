class FOG_Shirt_FRACU_Top_ColorBase: Toggleable_Sleeves_Shirt_Base 
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
            SetSimpleHiddenSelectionState(0,false); // Hide Half Cuffs
            SetSimpleHiddenSelectionState(1,true); //Show quarter sleeve
        }
        else
        {   
            SetSimpleHiddenSelectionState(0,true); // Hide Half Cuffs
            SetSimpleHiddenSelectionState(1,false); //Show quarter sleeve
        }
    }
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};