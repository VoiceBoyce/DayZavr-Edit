class Vp_Set18_Mask_ColorBase extends Mask_Base{};

class Vp_Set18_Mask_White : Vp_Set18_Mask_ColorBase
{	
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing Headgear = Clothing.Cast(parent.FindAttachmentBySlotName("Headgear"));
		if ( Headgear )
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
		if (slotId == InventorySlots.HEADGEAR) 
		{
			EntityAI Helmet = GetAttachmentByConfigTypeName("Vp_Set18_Mask_White");
			if (Helmet != NULL)
				return false;
		}
		return super.CanReceiveAttachment(attachment, slotId);
	}
}