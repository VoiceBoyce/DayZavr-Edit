class KOD_MDR_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new FamasRecoil(this);
	}		
};

class KOD_MDR_Black : KOD_MDR_Base {};
class KOD_MDR_Tan : KOD_MDR_Base {};