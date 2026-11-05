class KOD_AS50_Base : RifleBoltLock_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new SvdRecoil(this);
	}
};

class KOD_AS50_Black : KOD_AS50_Base {};
class KOD_AS50_Camo1 : KOD_AS50_Base {};
class KOD_AS50_Camo2 : KOD_AS50_Base {};
class KOD_AS50_Camo3 : KOD_AS50_Base {};
class KOD_AS50_Camo4 : KOD_AS50_Base {};