class KOD_Vector_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Ump45Recoil(this);
	}
};
class KOD_Vector : KOD_Vector_Base {};