class KOD_M110_Base : RifleBoltLock_Base
{
	void KOD_M110_Base()
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
		return new M4a1Recoil(this);
	}
};
class KOD_M110_Tan : KOD_M110_Base{};
class KOD_M110_Black : KOD_M110_Base{};