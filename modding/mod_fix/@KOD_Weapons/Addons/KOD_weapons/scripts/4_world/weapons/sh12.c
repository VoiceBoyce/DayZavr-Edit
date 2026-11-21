class KOD_DrumShotgun_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new SiagaRecoil(this);
	}
};