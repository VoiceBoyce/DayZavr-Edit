class KOD_M16A4_Base : RifleBoltLock_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new M4a1Recoil(this);
	}
};
class KOD_M16A4 : KOD_M16A4_Base{};