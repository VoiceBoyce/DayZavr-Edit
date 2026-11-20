class KOD_SCARH_Base : RifleBoltFree_Base
{
	void KOD_SCARH_Base()
	{	
		UnfoldOpticsKOD(this);
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new FALRecoil(this);
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

class KOD_SCARH : KOD_SCARH_Base {};