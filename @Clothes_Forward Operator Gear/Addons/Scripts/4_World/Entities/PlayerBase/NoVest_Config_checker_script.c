modded class PlayerBase
{
    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
        ItemBase vest = ItemBase.Cast(GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("Vest")));
        ItemBase body = ItemBase.Cast(GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("Body")));

        if (slotId == InventorySlots.GetSlotIdFromString("Vest") && body && body.ConfigGetBool("noVest"))
            return false;

        if (attachment.ConfigGetBool("noVest") && vest)
            return false;

        return super.CanReceiveAttachment(attachment, slotId);
    }
};