class KOD_TAR21_Base : RifleBoltFree_Base
{ 
	override RecoilBase SpawnRecoilObject()
	{
		return new AkmRecoil(this);
	}
};

class KOD_TAR21_Black : KOD_TAR21_Base {};
class KOD_TAR21_Green : KOD_TAR21_Base {};
class KOD_TAR21_Tan : KOD_TAR21_Base {};