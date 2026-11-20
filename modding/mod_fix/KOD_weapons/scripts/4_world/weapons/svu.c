class KOD_SVU_Base : RifleBoltFree_Base
{ 
	void KOD_SVU_Base()
	{		
		UnfoldOpticsKOD(this);	
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new SvdRecoil(this);
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
};

class KOD_SVU : KOD_SVU_Base {};