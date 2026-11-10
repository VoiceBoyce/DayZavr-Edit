class AD_M16A2 : M16A2_Base 
{
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
        if  ( attachment.IsKindOf("ItemOptics") )
        {     
			if ( attachment.IsKindOf("AD_ACOG_Base") || attachment.IsKindOf("AD_ACOG_RMR_Base"))
			{            
				return true;
			}   
			return false;
		};			
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
	override bool CanEnterIronsights()
	{
		ItemOptics optic = GetAttachedOptics();
		if (optic && AD_ACOG.Cast(optic) || AD_ACOG_RMR.Cast(optic))
			return true;
		return super.CanEnterIronsights();
	}
};