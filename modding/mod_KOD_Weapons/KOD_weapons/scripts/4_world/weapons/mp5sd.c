class KOD_MP5SD_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Mp5kRecoil(this);
	}
	
};

class KOD_MP5SD : KOD_MP5SD_Base {};