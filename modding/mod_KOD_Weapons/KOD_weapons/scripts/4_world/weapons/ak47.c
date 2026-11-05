class KOD_AK47_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new AkmRecoil(this);
	}
};

class KOD_AK47 : KOD_AK47_Base {};