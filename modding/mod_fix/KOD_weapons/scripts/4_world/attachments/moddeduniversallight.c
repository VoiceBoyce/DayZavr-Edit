modded class UniversalLight
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if ( parent.IsKindOf("KOD_AKALPHA_Base") || parent.IsKindOf("KOD_RSASS_Base") || parent.IsKindOf("KOD_Beowulf_Base") || parent.IsKindOf("KOD_AX50_Base") || parent.IsKindOf("KOD_ASH12_Base") || parent.IsKindOf("KOD_M16A4_Base")) 
		{
			return true;
		}
		
		if ( parent.IsKindOf("KOD_HK417_Base") || parent.IsKindOf("KOD_AK_Base") || parent.IsKindOf("KOD_VR80_Base") || parent.IsKindOf("KOD_RPK16_Base") || parent.IsKindOf("KOD_M110_Base") || parent.IsKindOf("KOD_SCARH_Base") || parent.IsKindOf("KOD_AR15Skeleton_Base") ) 
		{
			return true;
		}		
		
		if ( parent.IsKindOf("KOD_MCR_Base")  || parent.IsKindOf("KOD_SA58_Base") ) 
		{
			return true;
		}		
		
		return super.CanPutAsAttachment(parent);
	}
	
    override void InitAttachmentsSlotsToCheck(out array<int> AttSlots)
    {	
        super.InitAttachmentsSlotsToCheck(AttSlots);
		
		AttSlots.Insert(InventorySlots.GetSlotIdFromString("weaponMuzzleKODVSK94"));

    }
};
