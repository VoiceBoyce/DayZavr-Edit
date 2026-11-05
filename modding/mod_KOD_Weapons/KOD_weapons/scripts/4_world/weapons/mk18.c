class KOD_Mk18_Base : RifleBoltLock_Base
{
	void KOD_Mk18_Base()
	{
		ShowIronSightsKOD(this);
	}	
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);	
		
		if (ItemOptics.Cast(item))
		{ 
			HideIronSightsKOD(this);
		}	
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);	
		
		if (ItemOptics.Cast(item))
		{ 
			ShowIronSightsKOD(this);
		}	
	}	
	
	override RecoilBase SpawnRecoilObject()
	{
		return new M4a1Recoil(this);
	}
};
class KOD_Mk18 : KOD_Mk18_Base{};