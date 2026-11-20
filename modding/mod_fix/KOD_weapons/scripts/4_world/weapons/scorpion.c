class KOD_Scorpion_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Mp5kRecoil(this);
	}
};

class KOD_Scorpion : KOD_Scorpion_Base {};
