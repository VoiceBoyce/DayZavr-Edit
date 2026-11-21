class KOD_Raptr_Base : RifleBoltLock_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new SiagaRecoil(this);
	}
};
class KOD_Raptr : KOD_Raptr_Base{};