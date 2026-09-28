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
class FOG_Crye_G3_Shirt_MC extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_MCB extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_MCT extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_MCA extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_MCAL extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_6CD extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_AOR1 extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_AOR2 extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_Black extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_CB extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_RG extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_DCU extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_DTGR extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_Grey extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_M81 extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_MARPAT extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_RDBS extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_TGR extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_RG_BlackTrim extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_MC_FRACU extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_MC_BlackTrim extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_MC_Patagonia extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G2_Shirt_MC extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_Blue extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_AMCU extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_ALPF extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_DNC extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_DPM extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_ERDL_Red extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_KHK extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_Pantera extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_BTGR extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_KBTGR extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_RTGR extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_UCP extends FOG_Crye_G3_Shirt_ColorBase {};
class FOG_Crye_G3_Shirt_White extends FOG_Crye_G3_Shirt_ColorBase {};
