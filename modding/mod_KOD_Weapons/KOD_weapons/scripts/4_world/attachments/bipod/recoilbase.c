modded class RecoilBase
{
	override vector GetRecoilModifier(Weapon_Base weapon)
	{
		if( weapon )
		{
			return weapon.GetPropertyModifierObject().GetRecoil();
		}
		else return "1 1 1";
	}
}