class FOG_Helmet_FAST_SF_Base : FOG_Helmet_Base
{
    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
		if (IsAttachmentMutuallyExclusiveForReceive(slotId, "FOG_ear_cover", "SF_Comtacs"))
            return false;

		if (IsAttachmentBlockedByAnyForReceive(slotId, "SF_Cover", "FOG_small_patch", "SF_BattPack"))
            return false;

        if (IsAttachmentBlockedByForReceive(slotId, "SF_BattPack", "SF_Cover"))
			return false;

        if (IsAttachmentBlockedByForReceive(slotId, "FOG_small_patch", "SF_Cover"))
            return false;        
        
        return super.CanReceiveAttachment(attachment, slotId);
    }

	override bool CanDisplayAttachmentSlot( int slot_id )
	{
		if (!super.CanDisplayAttachmentSlot(slot_id))
			return false;

		string slot_name = InventorySlots.GetSlotName(slot_id);
        if (!IsAttachmentSlotVisibleForPair(slot_name, "FOG_ear_cover", "SF_Comtacs"))
			return false;

        if (!IsAttachmentSlotVisibleWhenNoAny(slot_name, "SF_Cover", "FOG_small_patch", "SF_BattPack"))
			return false;

        if (!IsAttachmentSlotVisibleWhenNo(slot_name, "SF_BattPack", "SF_Cover"))
			return false;

        if (!IsAttachmentSlotVisibleWhenNo(slot_name, "FOG_small_patch", "SF_Cover"))
			return false;

        return true;
    }	
};
class FOG_Helmet_AJs_FAST_SF_Base : FOG_Helmet_FAST_SF_Base{};
class FOG_Helmet_FAST_FTHS_MC : FOG_Helmet_FAST_SF_Base{};
class FOG_Helmet_FAST_FTHS_Tan : FOG_Helmet_FAST_FTHS_MC{};