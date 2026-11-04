class Vp_Set14_Helmet_ColorBase extends Mich2001Helmet {};

class Vp_Set14_Helmet : Vp_Set14_Helmet_ColorBase
{	
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing mask = Clothing.Cast(parent.FindAttachmentBySlotName("Mask"));
		if ( mask )
		{
			return false;
		}
		return true;
	}
}
modded class PlayerBase
{
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if (slotId == InventorySlots.MASK) 
		{
			EntityAI Motohelmet = GetAttachmentByConfigTypeName("Vp_Set14_Helmet");
			if (Motohelmet != NULL)
				return false;
		}
		return super.CanReceiveAttachment(attachment, slotId);
	}
}