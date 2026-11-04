class Vp_Set17_Helmet_ColorBase extends HelmetBase {};

class Vp_Set17_Helmet_Black : Vp_Set17_Helmet_ColorBase
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
			EntityAI Helmet = GetAttachmentByConfigTypeName("Vp_Set17_Helmet_Black");
			if (Helmet != NULL)
				return false;
		}
		return super.CanReceiveAttachment(attachment, slotId);
	}
}