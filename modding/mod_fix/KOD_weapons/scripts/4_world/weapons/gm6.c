class KOD_GM6Lynx_Base : RifleBoltFree_Base
{ 
	override RecoilBase SpawnRecoilObject()
	{
		return new SvdRecoil(this);
	}
};

class KOD_GM6Lynx : KOD_GM6Lynx_Base {};