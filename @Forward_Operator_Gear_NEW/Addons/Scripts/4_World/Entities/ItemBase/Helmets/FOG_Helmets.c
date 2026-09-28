class FOG_Helmet_Base : Mich2001Helmet
{
	protected bool IsAttachmentMutuallyExclusiveForReceive(int slotId, string slotA, string slotB)
	{
		if ((slotId == InventorySlots.GetSlotIdFromString(slotA) && FindAttachmentBySlotName(slotB)) || (slotId == InventorySlots.GetSlotIdFromString(slotB) && FindAttachmentBySlotName(slotA)))
			return true;

		return false;
	}

	protected bool IsAttachmentSlotVisibleForPair(string slot_name, string slotA, string slotB)
	{
		if (slot_name == slotA)
			return FindAttachmentBySlotName(slotB) == NULL;

		if (slot_name == slotB)
			return FindAttachmentBySlotName(slotA) == NULL;

		return true;
	}

	protected bool IsAttachmentBlockedByAnyForReceive(int slotId, string slotToAttach, string blockerA, string blockerB)
	{
		if (slotId == InventorySlots.GetSlotIdFromString(slotToAttach) && (FindAttachmentBySlotName(blockerA) || FindAttachmentBySlotName(blockerB)))
			return true;

		return false;
	}

	protected bool IsAttachmentBlockedByForReceive(int slotId, string slotToAttach, string blocker)
	{
		if (slotId == InventorySlots.GetSlotIdFromString(slotToAttach) && FindAttachmentBySlotName(blocker))
			return true;

		return false;
	}

	protected bool IsAttachmentSlotVisibleWhenNoAny(string slot_name, string slotToShow, string blockerA, string blockerB)
	{
		if (slot_name == slotToShow)
			return FindAttachmentBySlotName(blockerA) == NULL && FindAttachmentBySlotName(blockerB) == NULL;

		return true;
	}

	protected bool IsAttachmentSlotVisibleWhenNo(string slot_name, string slotToShow, string blocker)
	{
		if (slot_name == slotToShow)
			return FindAttachmentBySlotName(blocker) == NULL;

		return true;
	}

	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionRotateComtacs);
        AddAction(ActionRotateArmHeadsets);
        AddAction(ActionToggleNVGPosition);
		AddAction(ActionTurnOnHelmetLight);
        AddAction(ActionTurnOffHelmetLight);
	} 
};
class FOG_Helmet_Highcut_Base : FOG_Helmet_Base
{
    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
		if (IsAttachmentMutuallyExclusiveForReceive(slotId, "FOG_ear_cover", "SF_Comtacs"))
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

        return true;
    }	
};
class FOG_Helmet_Airframe_Base : FOG_Helmet_Highcut_Base{};
class FOG_Helmet_MTEK_Flux_Base : FOG_Helmet_Highcut_Base{};
class FOG_Helmet_Caimen_Base : FOG_Helmet_Highcut_Base{};

class FOG_Helmet_MICH2000_Base : FOG_Helmet_Base{};
class FOG_Helmet_Exfil_Base : FOG_Helmet_Base{};
class FOG_Helmet_HGU56_ColorBase : FOG_Helmet_Base{};