class KOD_SA58_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new FALRecoil(this);
	}
};

class KOD_SA58 : KOD_SA58_Base {};