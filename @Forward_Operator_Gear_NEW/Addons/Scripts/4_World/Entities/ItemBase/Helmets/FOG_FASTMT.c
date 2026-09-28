class FOG_Helmet_FASTMT_Base : FOG_Helmet_Base
{
    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
		if (IsAttachmentMutuallyExclusiveForReceive(slotId, "FOG_ear_cover", "SF_Comtacs"))
			return false;

        if ((slotId == InventorySlots.GetSlotIdFromString("FOG_ear_cover") || slotId == InventorySlots.GetSlotIdFromString("SF_Comtacs")) && FindAttachmentBySlotName("FOG_mand"))
            return false;

        if (slotId == InventorySlots.GetSlotIdFromString("FOG_mand") && (FindAttachmentBySlotName("FOG_ear_cover") || FindAttachmentBySlotName("SF_Comtacs"))) 
            return false;
        
		if (IsAttachmentBlockedByAnyForReceive(slotId, "FOG_MT_Cover", "FOG_small_patch", "SF_BattPack"))
            return false;

        if (IsAttachmentBlockedByForReceive(slotId, "SF_BattPack", "FOG_MT_Cover"))
			return false;

        if (IsAttachmentBlockedByForReceive(slotId, "FOG_small_patch", "FOG_MT_Cover"))
            return false;        
        
        return super.CanReceiveAttachment(attachment, slotId);
    }

	override bool CanDisplayAttachmentSlot( int slot_id )
	{
		if (!super.CanDisplayAttachmentSlot(slot_id))
			return false;

		string slot_name = InventorySlots.GetSlotName(slot_id);            
        if ( slot_name == "FOG_mand" )
            return ( !FindAttachmentBySlotName("FOG_ear_cover") && !FindAttachmentBySlotName("SF_Comtacs") );   
        
        if ( slot_name == "FOG_ear_cover" )
        {
            if (!IsAttachmentSlotVisibleForPair(slot_name, "FOG_ear_cover", "SF_Comtacs"))
                return false;

            return !FindAttachmentBySlotName("FOG_mand");
        }
        
        if ( slot_name == "SF_Comtacs" )
        {
            if (!IsAttachmentSlotVisibleForPair(slot_name, "FOG_ear_cover", "SF_Comtacs"))
                return false;

            return !FindAttachmentBySlotName("FOG_mand");
        }

        if (!IsAttachmentSlotVisibleWhenNoAny(slot_name, "FOG_MT_Cover", "FOG_small_patch", "SF_BattPack"))
            return false;

		if (!IsAttachmentSlotVisibleWhenNo(slot_name, "SF_BattPack", "FOG_MT_Cover"))
			return false;

		if (!IsAttachmentSlotVisibleWhenNo(slot_name, "FOG_small_patch", "FOG_MT_Cover"))
			return false;
		
        return true;
    }
};