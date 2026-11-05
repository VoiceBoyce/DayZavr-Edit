class KOD_M60E3_Base : RifleBoltFree_Base
{
	void KOD_M60E3_Base()
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
		return new AkmRecoil(this);
	}
};

class KOD_M60E3 : KOD_M60E3_Base {};
