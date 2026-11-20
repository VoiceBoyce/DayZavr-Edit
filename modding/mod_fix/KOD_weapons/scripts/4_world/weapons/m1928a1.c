class KOD_M1928A1_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Ump45Recoil(this);
	}
};

class KOD_M1928A1 : KOD_M1928A1_Base {};