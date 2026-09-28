
modded class PlayerBase
{
    override void SwitchItemSelectionTexture(EntityAI item, string slot_name)
    {
        super.SwitchItemSelectionTexture(item,slot_name);
        
        if (slot_name == "Mask" || slot_name == "Eyewear" || slot_name == "Headgear")
        {
            FOG_Hooded_Parka_ColorBase jacket;
            if (Class.CastTo(jacket, FindAttachmentBySlotName("Body")))
                jacket.HandleSelections();
        }
    }

    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
        FOG_Hooded_Parka_ColorBase jacket;
        if (Class.CastTo(jacket, FindAttachmentBySlotName("Body")))
            if (!jacket.CanWearWith(attachment, slotId))
                return false;
        
        return super.CanReceiveAttachment(attachment, slotId);
    }
};