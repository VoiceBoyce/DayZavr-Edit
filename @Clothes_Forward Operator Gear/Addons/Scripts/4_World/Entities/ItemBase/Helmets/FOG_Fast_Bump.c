class FOG_Helmet_FAST_Bump_Base : FOG_Helmet_Base
{
    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {

        if ((slotId == InventorySlots.GetSlotIdFromString("FOG_ear_cover") && FindAttachmentBySlotName("SF_Comtacs")) || slotId == InventorySlots.GetSlotIdFromString("SF_Comtacs") && FindAttachmentBySlotName("FOG_ear_cover"))
            return false;     
        
        return super.CanReceiveAttachment(attachment, slotId);
    }	

	override bool CanDisplayAttachmentSlot( int slot_id )
	{
		if (!super.CanDisplayAttachmentSlot(slot_id))
			return false;

		string slot_name = InventorySlots.GetSlotName(slot_id);
        if ( slot_name == "FOG_ear_cover" )
            return FindAttachmentBySlotName("SF_Comtacs") == NULL;

        if ( slot_name == "SF_Comtacs" )
            return FindAttachmentBySlotName("FOG_ear_cover") == NULL;

        return true;
    }	  	
};