class FOG_Shirt_ArcticJacket_ColorBase: Toggleable_Sleeves_Shirt_Base
{
    string cuffs_texture;

	void FOG_Shirt_ArcticJacket_ColorBase()
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
        SetSimpleHiddenSelectionState(0,true); // Show Hood Down
        SetSimpleHiddenSelectionState(1,false); //Hide Hood up
        SetSimpleHiddenSelectionState(2,false); // Hide Hood up big
    }

    void SetSleevesQuarter()
    {
        SetSimpleHiddenSelectionState(0,false); // Hide Hood Down
        SetSimpleHiddenSelectionState(1,true); //Show Hood up
        SetSimpleHiddenSelectionState(2,false); // Hide Hood up big
    }

    void SetSleevesFull()
    {
        SetSimpleHiddenSelectionState(0,false); // Hide Hood Down
        SetSimpleHiddenSelectionState(1,false); //Hide Hood up
        SetSimpleHiddenSelectionState(2,true); // Show Hood up big
    }

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
