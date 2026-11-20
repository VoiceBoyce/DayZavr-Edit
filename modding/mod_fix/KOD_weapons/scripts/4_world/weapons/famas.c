class KOD_FAMAS_Base : RifleBoltFree_Base
{ 
	override RecoilBase SpawnRecoilObject()
	{
		return new M4a1Recoil(this);
	}
};

class KOD_FAMAS : KOD_FAMAS_Base {};