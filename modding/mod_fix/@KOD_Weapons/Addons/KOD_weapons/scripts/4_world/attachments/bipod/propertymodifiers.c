modded class PropertyModifiers
{
	vector GetRecoil()
	{
		float recoil;

		if (HasBipodsAndProne(recoil))
		{
			return (m_RecoilModifiers * recoil);
		}
		return m_RecoilModifiers;
	}

	bool HasBipodsAndProne(out float rec)
	{
		Weapon_Base weapon;
		if (!CastTo(weapon, m_OwnerItem))
			return false;
		return weapon.HasBipodsAndProne(rec);
	}
}