class KOD_M249_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new M4a1Recoil(this);
	}
};

class KOD_M249 : KOD_M249_Base {};