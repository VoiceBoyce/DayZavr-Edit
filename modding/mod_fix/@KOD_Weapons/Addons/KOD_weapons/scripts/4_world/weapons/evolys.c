class KOD_Evolys_Base : RifleBoltLock_Base
{
	void KOD_Evolys_Base()
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
		return new AkmRecoil(this);
	}
};

class KOD_Evolys : KOD_Evolys_Base {};