class Vp_Set11_Motosuit_ColorBase extends Top_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
}
class Vp_Set11_Motosuit : Vp_Set11_Motosuit_ColorBase
{	
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing mask = Clothing.Cast(parent.FindAttachmentBySlotName("Mask"));
		if ( mask )
		{
			return false;
		}
		Clothing legs = Clothing.Cast(parent.FindAttachmentBySlotName("Legs"));
		if ( legs )
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
			EntityAI Motosuitmask = GetAttachmentByConfigTypeName("Vp_Set11_Motosuit");
			if (Motosuitmask != NULL)
				return false;
		}
		if (slotId == InventorySlots.LEGS) 
		{
			EntityAI Motosuitlegs = GetAttachmentByConfigTypeName("Vp_Set11_Motosuit");
			if (Motosuitlegs != NULL)
				return false;
		}
		return super.CanReceiveAttachment(attachment, slotId);
	}
}