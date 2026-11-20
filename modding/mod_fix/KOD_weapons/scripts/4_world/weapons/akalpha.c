class KOD_AKALPHA_Base : RifleBoltFree_Base
{
	void KOD_AKALPHA_Base()
	{
		EntityAI ParentItem = this;		
		UnfoldOpticsKOD(ParentItem);
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new AkmRecoil(this);
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);

		if (ItemOptics.Cast(item))
		{ 
			EntityAI ParentItem = this;
			FoldOpticsKOD(ParentItem);
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);

		if (ItemOptics.Cast(item))
		{ 
			EntityAI ParentItem = this;
			UnfoldOpticsKOD(ParentItem);
		}
	}		
};

class KOD_AKALPHA : KOD_AKALPHA_Base {};