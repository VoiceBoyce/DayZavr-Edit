class FOG_Hooded_Parka_ColorBase extends Top_Base
{
    
    bool ParentHasHeadgear()
    {
        return GetHierarchyParent() && GetHierarchyParent().FindAttachmentBySlotName("Headgear");
    }

    override void EEInit()
    {
        super.EEInit();

        HandleSelections();
    }

    void HandleSelections()
    {
        SetSimpleHiddenSelectionState(0, !ParentHasHeadgear());
        SetSimpleHiddenSelectionState(1, ParentHasHeadgear());
    }
    
    bool CanWearWith(EntityAI attachment, int slotID)
    {
        return true;
    }

    override void OnWasAttached( EntityAI parent, int slot_id ) 
    {
        super.OnWasAttached(parent, slot_id);
        
        HandleSelections();
    }

    override void OnWasDetached( EntityAI parent, int slot_id ) 
    {
        super.OnWasDetached(parent, slot_id);

        HandleSelections();
    }

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
    
};
class FOG_Hooded_Parka_OG extends FOG_Hooded_Parka_ColorBase {};
class FOG_Hooded_Parka_Black extends FOG_Hooded_Parka_ColorBase {};
class FOG_Hooded_Parka_RG extends FOG_Hooded_Parka_ColorBase {};
class FOG_Hooded_Parka_CB extends FOG_Hooded_Parka_ColorBase {};
class FOG_Hooded_Parka_MC extends FOG_Hooded_Parka_ColorBase {};
class FOG_Hooded_Parka_ALP extends FOG_Hooded_Parka_ColorBase {};
