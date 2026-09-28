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
