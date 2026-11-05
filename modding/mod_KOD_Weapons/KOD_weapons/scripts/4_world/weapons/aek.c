class KOD_AEK971_Base : AKM_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Ak74Recoil(this);
	}
};

class KOD_AEK971 : KOD_AEK971_Base {};