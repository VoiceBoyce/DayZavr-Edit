class KOD_DVL10M1_Base: BoltActionRifle_ExternalMagazine_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new CZ527Recoil(this);
	}
};
class KOD_DVL10M1 extends KOD_DVL10M1_Base{};