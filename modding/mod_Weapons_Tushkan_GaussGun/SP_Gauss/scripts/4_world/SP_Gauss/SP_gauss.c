class SP_Gauss_Base extends RifleBoltLock_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new SvdRecoil(this);
	}

};
class SP_Gauss extends SP_Gauss_Base {};