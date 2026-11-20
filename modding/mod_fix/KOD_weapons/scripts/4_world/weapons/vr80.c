class KOD_VR80_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new SiagaRecoil(this);
	}
};

class KOD_VR80 : KOD_VR80_Base {};