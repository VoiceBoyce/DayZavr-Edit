class KOD_PKP_Tac_Base : AK74_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new AkmRecoil(this);
	}
	
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
        if  ( attachment.IsKindOf("ItemOptics") && !FindAttachmentBySlotName("KODPKPTRail") )
        {         
			return false;
		}
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
};
class KOD_PKP_Tac_Black: KOD_PKP_Tac_Base {};
class KOD_PKP_Tac_FDE: KOD_PKP_Tac_Base {};

class KOD_PKP_Tac_Rail_Base extends Inventory_Base
{
	override bool CanDetachAttachment (EntityAI parent)
	{
		if ( Weapon_Base.Cast(parent) && parent.FindAttachmentBySlotName("weaponOptics") )
		{
			return false;
		}
		return true;
	}
}