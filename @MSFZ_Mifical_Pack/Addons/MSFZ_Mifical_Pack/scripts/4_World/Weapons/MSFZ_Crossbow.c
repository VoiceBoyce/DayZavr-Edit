class MSFZ_Crossbow_base : RifleBoltLock_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new M4a1Recoil(this);
	}
	override bool IsShowingChamberedBullet()
	{
		return false;
	}
};

class MSFZ_Crossbow:    MSFZ_Crossbow_base {};
class MSFZ_Crossbow_Black:    MSFZ_Crossbow_base {};
class MSFZ_Crossbow_Cosmos:    MSFZ_Crossbow_base {};
class MSFZ_Crossbow_Forest:    MSFZ_Crossbow_base {};
class MSFZ_Crossbow_Mix:    MSFZ_Crossbow_base {};
class MSFZ_Crossbow_Old:    MSFZ_Crossbow_base {};

class MSFZ_Crossbow_Mag_10Rnd:    MagazineStorage {};
class MSFZ_Crossbow_Mag_25Rnd:    MagazineStorage {};
class MSFZ_Crossbow_Mag_50Rnd:    MagazineStorage {};
class MSFZ_Crossbow_AP_Mag_10Rnd: MagazineStorage {};
class MSFZ_Crossbow_AP_Mag_25Rnd: MagazineStorage {};
class MSFZ_Crossbow_AP_Mag_50Rnd: MagazineStorage {};