class KOD_HK21_Base : RifleBoltLock_Base
{ 
	override RecoilBase SpawnRecoilObject()
	{
		return new FALRecoil(this);
	}
};
class KOD_HK21_Old : KOD_HK21_Base {};
class KOD_HK21_Black : KOD_HK21_Base {};