class KOD_AR50A1_Base : BoltActionRifle_InnerMagazine_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new CZ527Recoil(this);
	}
};
class KOD_AR50A1 : KOD_AR50A1_Base {};