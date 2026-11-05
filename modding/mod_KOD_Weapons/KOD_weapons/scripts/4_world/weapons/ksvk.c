class KOD_KSVK_Base: BoltActionRifle_ExternalMagazine_Base
{
	void KOD_KSVK_Base()
	{
		UnfoldOpticsKOD(this);
	}	
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);	
		
		if (ItemOptics.Cast(item))
		{ 
			FoldOpticsKOD(this);
		}	
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);	
		
		if (ItemOptics.Cast(item))
		{ 
			UnfoldOpticsKOD(this);
		}	
	}	
	
	override RecoilBase SpawnRecoilObject()
	{
		return new CZ527Recoil(this);
	}
};
class KOD_KSVK: KOD_KSVK_Base {};