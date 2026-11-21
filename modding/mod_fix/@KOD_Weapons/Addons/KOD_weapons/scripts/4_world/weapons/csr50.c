class KOD_CSR50_Base : BoltActionRifle_InnerMagazine_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Winchester70Recoil(this);
	}
};
class KOD_CSR50 : KOD_CSR50_Base {};