class FOG_Crye_G2_Shirt_ColorBase: Toggleable_Sleeves_Shirt_Base
{
    string cuffs_texture;

	void FOG_Crye_G2_Shirt_ColorBase()
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

    void SetSleevesHalf()//Quarter Roll
    {
        SetSimpleHiddenSelectionState(0,true); //Fray
        SetSimpleHiddenSelectionState(1,false); //Sleeve Half
        SetSimpleHiddenSelectionState(2,false); //Underarmor
        SetSimpleHiddenSelectionState(3,true); //personality
    }

    void SetSleevesQuarter()//Ripped
    {
        SetSimpleHiddenSelectionState(0,false); //Fray
        SetSimpleHiddenSelectionState(1,true); //Sleeve Half
        SetSimpleHiddenSelectionState(2,false); //Underarmor
        SetSimpleHiddenSelectionState(3,true); //personality
    }

    void SetSleevesFull()//Ripped With Underarmor
    {
        SetSimpleHiddenSelectionState(0,true); //Fray
        SetSimpleHiddenSelectionState(1,false); //Sleeve Half
        SetSimpleHiddenSelectionState(2,true); //Underarmor
        SetSimpleHiddenSelectionState(3,false); //personality
    }

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
