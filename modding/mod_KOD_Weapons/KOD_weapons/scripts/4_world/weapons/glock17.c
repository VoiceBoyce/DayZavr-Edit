class KOD_Glock17_Base : Pistol_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new GlockRecoil(this);
	}
	
	void KOD_Glock17_Base()
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
};
class KOD_Glock17_Custom_Base : KOD_Glock17_Base {};
class KOD_Glock17_Custom_Black : KOD_Glock17_Custom_Base {};
class KOD_Glock17_Custom_Tan : KOD_Glock17_Custom_Base {};
class KOD_Glock17_Custom_TanBlack : KOD_Glock17_Custom_Base {};
class KOD_Glock17_Custom_BlackTan : KOD_Glock17_Custom_Base {};

class KOD_Glock17_Standard_Base : KOD_Glock17_Base {};
class KOD_Glock17_Standard_Black : KOD_Glock17_Standard_Base {};
class KOD_Glock17_Standard_Tan : KOD_Glock17_Standard_Base {};
class KOD_Glock17_Standard_TanBlack : KOD_Glock17_Standard_Base {};
class KOD_Glock17_Standard_BlackTan : KOD_Glock17_Standard_Base {};
