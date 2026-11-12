// Отдача M4A1, но без падения, когда арбалет не в руках игрока: окна предпросмотра и подсказки (ItemInfo)
// создают объект отдачи для арбалета на земле или в контейнере, а ванильная отдача обращается к игроку без проверки.
class MSFZ_CrossbowRecoil : M4a1Recoil
{
	override void Init()
	{
		if (!m_Player)
			return;
		super.Init();
	}
	override void PostInit(Weapon_Base weapon)
	{
		if (!m_Player)
			return;
		super.PostInit(weapon);
	}
};

class MSFZ_Crossbow_base : RifleBoltLock_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new MSFZ_CrossbowRecoil(this);
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
// Купидоны раньше не имели скриптового класса и становились Magazine_Base: их нельзя было снарядить болтами,
// разрядить и подхватить быстрой перезарядкой (R).
class MSFZ_Crossbow_Cupid_Mag_10Rnd: MagazineStorage {};
class MSFZ_Crossbow_Cupid_Mag_25Rnd: MagazineStorage {};
class MSFZ_Crossbow_Cupid_Mag_50Rnd: MagazineStorage {};
