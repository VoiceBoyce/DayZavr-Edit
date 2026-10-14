modded class PlayerBase
{
    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
        // говно проверять надо
        if (slotId == InventorySlots.VEST)
        {
            // Чекнуть говно  GetItemOnSlot("Body")
            ItemBase shirt = ItemBase.Cast(FindAttachmentBySlotName("Body")); //FindAttachmentBySlotName("Body")
            if (shirt && shirt.IsKindOf("Stalker_2_Monolith_Top_ColorBase"))
            {
                // Пиздабол ТУТУТУТТУТУТУТУТТУТУТТУТУТУТТ СУКА ЕБАНЫЙ КОД ГОВНА
                return false;
            }
        }
        if (slotId == InventorySlots.BODY)
        {
            ItemBase vest = ItemBase.Cast(FindAttachmentBySlotName("Vest"));
            if (vest && attachment.IsKindOf("Stalker_2_Monolith_Top_ColorBase")) {
                return false;
            }
        }
        return super.CanReceiveAttachment(attachment, slotId);
    }
};