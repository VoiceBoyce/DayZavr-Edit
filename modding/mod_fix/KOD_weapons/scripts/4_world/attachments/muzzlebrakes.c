class KOD_Compensator_AR extends SuppressorBase
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( parent.FindAttachmentBySlotName("suppressorImpro") == NULL && parent.FindAttachmentBySlotName("weaponMuzzleM4") == NULL )
		{
			if( parent.IsKindOf("FAL_Base") || parent.IsKindOf("CZ550_Base") || parent.IsKindOf("Winchester70_Base") || parent.IsKindOf("LongHorn_Base") || parent.IsKindOf("KOD_HK21_Base") || parent.IsKindOf("KOD_HK417_Base") || parent.IsKindOf("KOD_M110_Base") || parent.IsKindOf("KOD_RSASS_Base") || parent.IsKindOf("KOD_SA58_Base") || parent.IsKindOf("KOD_MDR_Base")) 
			{				
				return true;
			}
			
			if( parent.IsKindOf("KOD_FAMAS_Base") || parent.IsKindOf("KOD_M16A4_Base") || parent.IsKindOf("KOD_MBR_Base") || parent.IsKindOf("KOD_MCR_Base") || parent.IsKindOf("KOD_AR15Skeleton_Base") || parent.IsKindOf("KOD_Mk18_Base") || parent.IsKindOf("KOD_M249_Base") || parent.IsKindOf("KOD_TAR21_Base") ) 
			{				
				return true;
			}
		}
		return false;
	}	
};
class KOD_Compensator_AK extends SuppressorBase
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( parent.FindAttachmentBySlotName("suppressorImpro") == NULL )
		{
			return true;
		}
		return false;
	}
};