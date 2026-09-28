class FOG_Crye_G3_Shirt_ColorBase: Toggleable_Sleeves_Shirt_Base
{
    string cuffs_texture;

	void FOG_Crye_G3_Shirt_ColorBase()
	{
        cuffs_texture = GetObjectTexture(1);
	}

    override void HandleSleeves()
    {
        switch (GetSleevesState()) {
			case SleevesState.ROLLED: {
				SetSleevesHalf();
				break;
			}
			
			case SleevesState.UNROLLED: {
				SetSleevesFull();
				break;
			}

            case SleevesState.HALF: {
				SetSleevesQuarter();
				break;
			}
        }
    }

    void SetSleevesHalf()
    {
        SetSimpleHiddenSelectionState(0,true); // Hide Half Cuffs
        SetSimpleHiddenSelectionState(1,false); //Show quarter sleeve
        SetSimpleHiddenSelectionState(2,false); // Show Quarter Cuffs
        SetSimpleHiddenSelectionState(3,false); //Hide half sleeve
    }

    void SetSleevesQuarter()
    {
        SetSimpleHiddenSelectionState(0,false); // Hide Half Cuffs
        SetSimpleHiddenSelectionState(1,true); //Show quarter sleeve
        SetSimpleHiddenSelectionState(2,true); // Show Quarter Cuffs
        SetSimpleHiddenSelectionState(3,false); //Hide half sleeve
    }

    void SetSleevesFull()
    {
        SetSimpleHiddenSelectionState(0,false); // Hide Half Cuffs
        SetSimpleHiddenSelectionState(1,true); //Show quarter sleeve
        SetSimpleHiddenSelectionState(2,false); // Hide Quarter Cuffs
        SetSimpleHiddenSelectionState(3,true); //Show half sleeve
    }

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
