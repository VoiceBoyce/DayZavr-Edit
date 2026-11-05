class KOD_Kivaari_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new FALRecoil(this);
	}
};

class KOD_Kivaari_Grey : KOD_Kivaari_Base {};
class KOD_Kivaari_Black : KOD_Kivaari_Base {};
class KOD_Kivaari_Green : KOD_Kivaari_Base {};
class KOD_Kivaari_Tan : KOD_Kivaari_Base {};
