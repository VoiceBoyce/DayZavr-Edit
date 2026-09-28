class FOG_FlightSuit_ColorBase: Top_Base
{
    static const string SLOT_EXCLUSION = "Legs";

    override bool CanPutAsAttachment(EntityAI parent)
    {
        if (!super.CanPutAsAttachment(parent)) {
            return false;
        }

        int slot_to_hide = InventorySlots.GetSlotIdFromString(SLOT_EXCLUSION);

        EntityAI legs = parent.GetInventory().FindAttachment(slot_to_hide);
        if (legs) {
            return false;
        }

        return true;
    }

    override void OnWasAttached(EntityAI parent, int slot_id)
    {
        super.OnWasAttached(parent, slot_id);

        int slot_to_hide = InventorySlots.GetSlotIdFromString(SLOT_EXCLUSION);

        EntityAI legs = parent.GetInventory().FindPlaceholderForSlot(slot_to_hide);
        if (legs) {
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(legs.SetInvisible, 0, 0, true);
        }

        legs = parent.GetInventory().FindAttachment(slot_to_hide);
        if (legs) {
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(legs.SetInvisible, 0, 0, true);
        }
        
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(parent.GetInventory().SetSlotLock, 0, 0, slot_to_hide, true);
    }

    override void OnWasDetached(EntityAI parent, int slot_id)
    {
        super.OnWasDetached(parent, slot_id);

        int slot_to_hide = InventorySlots.GetSlotIdFromString(SLOT_EXCLUSION);

        EntityAI legs = parent.GetInventory().FindPlaceholderForSlot(slot_to_hide);
        if (legs) {
            legs.SetInvisible(false);
        }

        legs = parent.GetInventory().FindAttachment(slot_to_hide);
        if (legs) {
            legs.SetInvisible(false);
        }

        parent.GetInventory().SetSlotLock(slot_to_hide, false);
    }
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
}