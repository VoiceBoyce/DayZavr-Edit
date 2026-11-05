class KOD_Beowulf_Base : RifleBoltLock_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new M4a1Recoil(this);
	}
};
class KOD_Beowulf : KOD_Beowulf_Base{};