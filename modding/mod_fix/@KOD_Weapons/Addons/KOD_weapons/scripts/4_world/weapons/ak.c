class KOD_AK_Base : RifleBoltFree_Base
{
	void KOD_AK_Base()
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

class KOD_AK15_Base : KOD_AK_Base {};
class KOD_AK15 : KOD_AK15_Base {};

class KOD_AK12_Base : KOD_AK_Base {};
class KOD_AK12 : KOD_AK12_Base {};