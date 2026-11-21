class KOD_DVL10_Base: BoltActionRifle_ExternalMagazine_Base
{
	void KOD_DVL10_Base()
	{
		ShowMuzzleKOD(this);
	}	
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);	
		
		EntityAI parent = item.GetHierarchyParent();
		if( parent && parent.FindAttachmentBySlotName("weaponMuzzleAWM") != NULL )
		{
			HideMuzzleKOD(this);
		}	
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);	
		
		EntityAI parent = item.GetHierarchyParent();		
		if( parent && parent.FindAttachmentBySlotName("weaponMuzzleAWM") == NULL )
		{
			ShowMuzzleKOD(this);
		}	
	}	
	
	override RecoilBase SpawnRecoilObject()
	{
		return new CZ527Recoil(this);
	}
};
class KOD_DVL10 extends KOD_DVL10_Base{};