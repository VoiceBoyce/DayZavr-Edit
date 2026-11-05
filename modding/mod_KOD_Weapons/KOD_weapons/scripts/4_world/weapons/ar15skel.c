class KOD_AR15Skeleton_Base : RifleBoltLock_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new M4a1Recoil(this);
	}
};
class KOD_AR15Skeleton : KOD_AR15Skeleton_Base{};