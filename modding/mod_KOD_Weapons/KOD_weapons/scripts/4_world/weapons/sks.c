class KOD_SKS_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new SKSRecoil(this);
	}
};

class KOD_SKS : KOD_SKS_Base {};