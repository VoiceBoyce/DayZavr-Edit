class KOD_AK458_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new AkmRecoil(this);
	}
};

class KOD_AK458_Plastic : KOD_AK458_Base {};
class KOD_AK458_Wood : KOD_AK458_Base {};