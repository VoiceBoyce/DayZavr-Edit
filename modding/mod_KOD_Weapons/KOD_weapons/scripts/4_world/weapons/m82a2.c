class KOD_M82A2_Base : RifleBoltLock_Base
{
	void KOD_M82A2_Base()
	{
		ShowMuzzleKOD(this);
	}	
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);	
		
		EntityAI parent = item.GetHierarchyParent();
		if( parent && parent.FindAttachmentBySlotName("weaponMuzzleM82A2") != NULL )
		{
			HideMuzzleKOD(this);
		}	
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);	
		
		EntityAI parent = item.GetHierarchyParent();		
		if( parent && parent.FindAttachmentBySlotName("weaponMuzzleM82A2") == NULL )
		{
			ShowMuzzleKOD(this);
		}	
	}	
	
	override RecoilBase SpawnRecoilObject()
	{
		return new SvdRecoil(this);
	}
};

class KOD_M82A2_Grey : KOD_M82A2_Base {};
class KOD_M82A2_Tan : KOD_M82A2_Base {};
class KOD_M82A2_Black : KOD_M82A2_Base {};