class SFP_RusHelmetT_base extends Mich2001Helmet
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTurnOnHelmetFlashlight); //use default light actions instead?
		AddAction(ActionTurnOffHelmetFlashlight);
		AddAction(ActionToggleNVG);
	}
	
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		if (slotId == InventorySlots.HEADGEAR)
		{
			ret.Insert(EAttExclusions.EXCLUSION_MASK_0);
			ret.Insert(EAttExclusions.EXCLUSION_MASK_1);
			ret.Insert(EAttExclusions.EXCLUSION_MASK_2);
			ret.Insert(EAttExclusions.EXCLUSION_MASK_3);
			
			ret.Insert(EAttExclusions.SHAVING_HEADGEAR_ATT_0);
		}
		return ret;
	}
	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_HEADSTRAP_0);
	}
};
class SFP_RusHelmetT_Summer extends SFP_RusHelmetT_base {};
class SFP_RusHelmetT2_base extends SFP_RusHelmetT_base {};
class SFP_RusHelmetT2_Summer extends SFP_RusHelmetT_base {};

//------------------------------------
class SFP_Zombie_Base extends ZombieMaleBase
{
	override bool CanBeSkinned()
	{
		return true;
	}
	
}
/*class SFP_izlome_Base extends ZombieMaleBase
{
	override bool CanBeSkinned()
	{
		return true;
	}
	
}
