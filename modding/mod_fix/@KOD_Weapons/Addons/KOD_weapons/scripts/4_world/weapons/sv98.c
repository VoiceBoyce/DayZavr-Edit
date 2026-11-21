class KOD_SV98_Base : BoltActionRifle_ExternalMagazine_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new CZ527Recoil(this);
	}
};
class KOD_SV98_Green extends KOD_SV98_Base {};
class KOD_SV98_Black extends KOD_SV98_Base {};