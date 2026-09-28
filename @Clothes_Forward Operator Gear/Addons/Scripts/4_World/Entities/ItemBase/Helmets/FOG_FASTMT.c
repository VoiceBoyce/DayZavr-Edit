class FOG_Helmet_FASTMT_Base : FOG_Helmet_Base
{
    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
        if ((slotId == InventorySlots.GetSlotIdFromString("FOG_ear_cover") || slotId == InventorySlots.GetSlotIdFromString("SF_Comtacs")) && FindAttachmentBySlotName("FOG_mand"))
            return false;

        if (slotId == InventorySlots.GetSlotIdFromString("FOG_mand") && (FindAttachmentBySlotName("FOG_ear_cover") || FindAttachmentBySlotName("SF_Comtacs"))) 
            return false;

        if ((slotId == InventorySlots.GetSlotIdFromString("FOG_ear_cover") && FindAttachmentBySlotName("SF_Comtacs")) || slotId == InventorySlots.GetSlotIdFromString("SF_Comtacs") && FindAttachmentBySlotName("FOG_ear_cover"))
            return false;
        
        if (slotId == InventorySlots.GetSlotIdFromString("FOG_MT_Cover") && (FindAttachmentBySlotName("FOG_small_patch") || FindAttachmentBySlotName("SF_BattPack")))
            return false;

        if ((slotId == InventorySlots.GetSlotIdFromString("SF_BattPack") || slotId == InventorySlots.GetSlotIdFromString("FOG_small_patch")) && FindAttachmentBySlotName("FOG_MT_Cover")) 
            return false;        
        
        return super.CanReceiveAttachment(attachment, slotId);
    }

	override bool CanDisplayAttachmentSlot( int slot_id )
	{
		if (!super.CanDisplayAttachmentSlot(slot_id))
			return false;

		string slot_name = InventorySlots.GetSlotName(slot_id);            
        if ( slot_name == "FOG_mand" )
            return ( !FindAttachmentBySlotName("FOG_ear_cover") || !FindAttachmentBySlotName("SF_Comtacs") );   
        
        if ( slot_name == "FOG_ear_cover" )
            return ( !FindAttachmentBySlotName("FOG_mand") || !FindAttachmentBySlotName("SF_Comtacs") );  
        
        if ( slot_name == "SF_Comtacs" )
            return ( !FindAttachmentBySlotName("FOG_mand") || !FindAttachmentBySlotName("FOG_ear_cover") );           
		
        return true;
    }
};