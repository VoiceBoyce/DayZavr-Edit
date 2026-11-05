class KOD_RSASS_Base : RifleBoltFree_Base
{
	void KOD_RSASS_Base()
	{
		EntityAI ParentItem = this;		
		UnfoldOpticsKOD(ParentItem);
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

class KOD_RSASS_Black : KOD_RSASS_Base {};
class KOD_RSASS_Tan : KOD_RSASS_Base {};